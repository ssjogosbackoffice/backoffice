<?php check_access('view_account_statement',$doredirect=true);?>
<?php include('header.inc'); ?>
<?php

function checkFields($start_day, $start_month, $start_year, $end_day, $end_month, $end_year, $payment_method) {
    if ( empty($start_day) || empty($start_month) || empty($start_year) )
        addError("", "The full start date must be selected");
    elseif ( ! checkDate($start_month,$start_day,$start_year) )
        addError("", "The start date you selected is not a valid calendar date");

    if ( ! $end_day || ! $end_month || ! $end_year )
        addError("", "The full end date must be selected");
    elseif ( ! checkDate($end_month,$end_day,$end_year) )
        addError("", "The end date you selected is not a valid calendar date");

    if ( ! areErrors() )
        if ( mktime(0,0,0,$start_month,$start_day,$start_year) > mktime(0,0,0,$end_month,$end_day,$end_year) )
            addError("","The end date is earlier than the start date");

    if ( $payment_method && ! getPaymentMethodName($payment_method) )
        addError("", "Invalid payment method");

    return ! areErrors();
}


function dumpToExcelFile($cols_arr) {
    global $excel_fname, $records, $show_all_games;

    $f = fopen(temp_dir."/$excel_fname", 'w');
    fputs($f, "<table>");
    fputs($f, "<tr><th>".implode('</th><th>', $cols_arr)."</th></tr>\n");

    $num_recs = count($records);

    for ( $i=0; $i<$num_recs; $i++)  {
        //Apply formatting functions to column values, if present
        $arr = array();
        $current = $records[$i];

        reset($cols_arr);

        foreach($cols_arr as $key=>$val) {
            if ( 'last game time' == $val )
                continue;

            if ( 'balance' == $val )
                $arr[$val] = number_format($current[$val],2);
            else
                $arr[$val] = $current[$val];
        }
        //showval($arr);
        // highlight voided entries
        if(!empty($current['pre_void']) || !empty($current['ctr_void'])  )
            fputs($f, "<tr><td style=\"background-color: #FF9999;\">".implode('</td><td style="background-color: #FF9999;">', $arr)."</td></tr>\n");
        else
            fputs($f, "<tr><td>".implode('</td><td>', $arr)."</td></tr>\n");
    }
    fputs($f, "</table>");
    fclose($f);
}


$customer_id = (int)( isset($_GET['customer_id'])  ? $_GET['customer_id'] : $_POST['customer_id'] );

if (  0 == $customer_id )
    die("Invalid customer_id: $customer_id");

$customer_row  = getPunter($customer_id);

if ( isset($_POST['do_search']) ) {
    $payment_method = $_POST['payment_method'];
    $show_all_games = $_POST['show_all_games'];
    $recs_per_page  =  (int)$_POST['recs_per_page'];
    $start_rec      = (int)$_POST['start_rec'];

    //end date parameters
    $start_day   = (int)$_POST['start_day'];
    $start_month = (int)$_POST['start_month'];
    $start_year  = (int)$_POST['start_year'];

    //end date parameters
    $end_day         = (int)$_POST['end_day'];
    $end_month   = (int)$_POST['end_month'];
    $end_year    = (int)$_POST['end_year'];


    if ( checkFields($start_day, $start_month, $start_year, $end_day, $end_month, $end_year, $payment_method) ) {
        $start_date = toDatabaseDate($start_day,$start_month,$start_year);
        $end_date   = toDatabaseDate($end_day,$end_month,$end_year);

        $start_date_display = shortDate($start_date);
        $end_date_display   = shortDate($end_date);

        $start_date_fname = ereg_replace('-', '', $start_date);
        $end_date_fname   = ereg_replace('-', '', $end_date);

        $max_date_mktime  =  mktime(0,0,0,$end_month,$end_day+1,$end_year);
        $max_date         = phpTsToDbTs($max_date_mktime);

        $excel_fname = 'acct-stmnt-'.$customer_id.'-'.$start_date_fname.'-'.$end_date_fname . '.xls';
        $cache_fname = 'acct-stmnt-'.$customer_id.'-'.session_id();
        $cache_fname_no_sid = 'acct-stmnt-'.$customer_id;
        // get all transactions

        $sql  = "select ctr_id AS id, TO_CHAR(ctr_time, 'YYYY-mm-dd hh24:mi:ss') as time, tty_name as \"transaction type\"" .
            //"  , ctr_amount as amount, ctr_transaction_num || ' ' || ctr_settle_track_num  as \"transaction num\"" .
            "  , ctr_amount as amount, ctr_id as \"transaction num\"" .
            "  , ctr_pun_id as \"customer id\", ctr_res_id as \"result id\", " .
            "  COALESCE(pme_name,(SELECT gam_name FROM game WHERE gam_id = " .
            "          (SELECT res_gam_id FROM result WHERE res_id = ctr_res_id)))  AS \"payment method\"," .
            " ctr_status, ctr_void" .
            " from ( (customer_transaction join punter on ctr_pun_id = pun_id)" .
            "          join transaction_type on ctr_tty_id = tty_id ) left outer join payment_method on ctr_pme_code = pme_code" .
            " where ctr_time >= '$start_date' and ctr_time < '$max_date' AND pun_id = $customer_id AND " .
            " ( ( ctr_tty_id = 4 ) OR " . // game result
            "   ( ctr_tty_id = 10 ) OR " . // Tip
            "   ( ctr_status = 'completed' and ctr_amount != 0) " .
            " )" .
            " order by ctr_time";
        $rs = $dbh->exec($sql);
        $num_recs = $rs->getNumRows();

        if ( $num_recs > 0 ) {
            $display_results = true;
            if ( empty($show_all_games) ) {
                $current = $rs->current();  //get a transaction record

                while ( $current ) //while there are more records
                {
                    $is_game_blk    = false;
                    $scout_rec      = $current; //this will be used to scout ahead for game transactions
                    $last_game_time = '';

                    //if the current transaction record is a game transaction
                    if ( 'game result' == $current['transaction type'] )
                    {
                        $blk_num_games  = 0; //number of games in playing block
                        $blk_amnt       = 0; //total amount won/lost in palying block
                        $blk_first_game_time = $current['time'];
                        $blk_ctr_void = false;

                        // while we are still seeing game transactions (i.e. we are still in a playing block)
                        while ( !empty($scout_rec) )
                        {
                            //if scouted record is not a game transaction
                            if ( 'game result' != $scout_rec['transaction type']  )
                            {
                                $rs->previous(); //go back so next iteration of records doesn't miss one
                                break;
                            }

                            $blk_last_game_time = $scout_rec['time'];  // store last game time value
                            $blk_num_games++;

                            if ( $blk_num_games > 1 )
                                $is_game_blk = true;

                            //if the game transaction has been voided
                            //then we set the summary record's ctr_void to true, otherwise
                            //we the amount (-ve or +ve) to the playing block total amount
                            if ( $scout_rec['ctr_void'] >= 1 )
                            {
                                // set playing block to void, to highlight that it includes at least
                                // one void game record
                                $blk_ctr_void = true;
                            }
                            else
                                $blk_amnt += $scout_rec['Amount'];  // add rec amount to block total

                            $scout_rec = $rs->next();    //get another record
                        }
                        if ( true == $is_game_blk )
                        {
                            if ( true == $blk_ctr_void )
                                $current['ctr_void'] = 1;

                            $current['transaction type'] = "$blk_num_games Game Results";
                            $current['amount']           = $blk_amnt;
                            $current['time']             = $blk_first_game_time;
                            $current['last game time']   = $blk_last_game_time;
                        }
                    }

                    $records[] = $current; //add the modified summary row to the record array

                    $current = $rs->next();  //get the next transaction record
                }
                unset($rs);
            }
            else
            {
                $records = $rs->Records;
            }

            $num_recs = count($records);

            // clculate the ending balance of each transaction

            $last_rec = $records[$num_recs-1];
            $last_rec_time = ( empty($last_rec['last game time']) ? $last_rec['time'] : $last_rec['last game time'] );

            $last_rec_arr = splitDate($last_rec_time);

            //get the zeroed time for the next day
            $next_day_start_mt = mktime(0, 0, 0, $last_rec_arr['month'], $last_rec_arr['day']+1, $last_rec_arr['year']);
            $next_day_start_db = phpTsToDb($next_day_start_mt);


            // add amounts for transactions that occurred after last record returned
            // this sum will be subtracted from the current balance, to get the balance after
            // the last transaction

            $sql  = "select coalesce(sum(ctr_amount),0) as sum from customer_transaction " .
                "where ctr_time >= '$next_day_start_db' " .
                "and (ctr_void IS NULL OR ctr_void = 0 ) " .
                "and (ctr_tty_id = 4 OR ctr_status = 'completed')   " .
                "and ctr_pun_id = $customer_id";

            $rs = $dbh->exec($sql);
            $row = $rs->next();
            $sum = $row['sum'];

            $current_balance = $customer_row['credits'];  // current customer balance
            $bal_prev = $current_balance - $sum;

            for ( $i=($num_recs-1); $i>=0; $i-- )
            {
                // Set the balance column value for each record
                $records[$i]['balance'] = $bal_prev;

                if (  $records[$i]['ctr_void'] >= 1 )
                {
                    if ( empty($show_all_games) )  // If we're summarizing,
                        if ( strstr($records[$i]['transaction type'], 'Game Results') ) //if the rec is a game result summ
                            $bal_prev -= $records[$i]['amount'];

                    // else bal_prev stays the same
                }
                else
                    $bal_prev -= $records[$i]['amount'];  // bal is this trans close bal - amount

                // set the links
                if ( 'game result' == $records[$i]['transaction type'] )
                {
                    $url = secure_host."/customers/reports/bet_details_pop.html" .
                        "?res=".$records[$i]['Result id']."&customer_id=$customer_id";

                    $link = "javascript:openWindow('$url', 'adbetpop', 400, 400, 'no', 'yes')";
                    $records[$i]['transaction num'] = getClassLink($link, $records[$i]['transaction num'], 'contentlink');
                }
                elseif ( ! strstr($records[$i]['transaction type'], 'game results') )
                {
                    $url = secure_host."/financial_reports/trans_detail_pop.html?id=".$records[$i]['id'];
                    $link = "javascript:openWindow('$url', 'adtranspop', 400, 400, 'no', 'yes')";
                    $records[$i]['transaction num'] = getClassLink($link, $records[$i]['transaction num'], 'contentlink');
                }
                else {
                    $records[$i]['transaction num'] = '';
                }
            }

            $cols = array_keys($records[0]);
            dumpToExcelFile($cols);
            //if ( $num_recs > $recs_per_page )
            {
                // Write results to cache file

                include 'cache_file_classes_inc.php';

                $cr = new cache_file_writer($cache_fname);
                $cr->open();

                if ( $cr->is_open() )
                {
                    $cr->write_all($records, $cols);
                    $cr->close();
                }
            }
        }
    }
}
?>
<?php include 'xmlhttprequest_inc.php';?>
<?php

if ( $display_results )
{
    $get_str = 'sid='.session_id().'&num_recs='.$num_recs.'&recs_per_page='.$recs_per_page .
        '&cf=' . $cache_fname_no_sid;

    $prev_rec_cont = "Previous [%num%] records";
    $next_rec_cont = "Next [%num%] records";

    $prev_cont_arr  = explode('[%num%]', $prev_rec_cont);
    $next_cont_arr  = explode('[%num%]', $next_rec_cont);
    ?>
    <script language="Javascript">

        function getPreviousBatch (start_rec)
        {
            retrieveURL('<?=secure_host;?>/admin/customers/reports/account_statement_inr.php?<?=$get_str?>&start_rec=' + start_rec);

            // number of records in current batch
            var recs_curr_batch = Math.min(<?=$num_recs;?> - (start_rec + <?=$recs_per_page;?>), <?=$recs_per_page;?>);

            // show and update "next" button

            if ( null == document.getElementById('next_btn') )
            {
                insertNextBtn();
            }

            var next_btn = document.getElementById('next_btn');
            next_btn.value = "<?=$next_cont_arr[0]?>" + recs_curr_batch + "<?=$next_cont_arr[1]?>";
            next_btn.onclick = function () { getNextBatch (start_rec + <?=$recs_per_page;?>) };

            var prev_btn = document.getElementById('prev_btn');

            if ( start_rec == 0 )
            {
                removePreviousBtn();
            }
            else
            {
                // update "previous" button
                prev_btn.value = "Previous <?=$recs_per_page?> records";
                prev_btn.onclick = function () { getPreviousBatch (start_rec - <?=$recs_per_page;?>) };
            }
        }


        function getNextBatch (start_rec)
        {

            retrieveURL('<?=secure_host;?>/customers/reports/account_statement_inr.php?<?=$get_str?>&start_rec=' + start_rec);
            // show previous button - should always display "Previous $recs_per_page records"": it never changes

            if ( null == document.getElementById('prev_btn') )
                insertPreviousBtn();

            var prev_btn = document.getElementById('prev_btn');

            var next_btn = document.getElementById('next_btn');

            prev_btn.onclick = function () { getPreviousBatch (start_rec - <?=$recs_per_page;?>) };
            prev_btn.setAttribute('value', "Previous <?=$recs_per_page?> records");

            // show and update "next" button

            var recs_next_batch = Math.min(<?=$num_recs;?> - start_rec, <?=$recs_per_page;?>);
            var start_rec_2nd_batch = start_rec + recs_next_batch; // number in screen following next one

            if ( start_rec_2nd_batch < <?=$num_recs;?> )
            {
                var recs_2nd_batch = Math.min(<?=$num_recs;?> - start_rec_2nd_batch, <?=$recs_per_page;?>);

                next_btn.value = "<?=$next_cont_arr[0]?>" + recs_2nd_batch + "<?=$next_cont_arr[1]?>";
                next_btn.onclick = function () { getNextBatch (start_rec_2nd_batch) };
            }
            else
            {
                removeNextBtn();
            }
        }


        function insertPreviousBtn ()
        {
            var area = document.getElementById('scroll_btns');
            var next_btn = document.getElementById('next_btn');

            var prev_btn = document.createElement('input');
            prev_btn.setAttribute('type', 'button');
            prev_btn.setAttribute('id', 'prev_btn');
            area.insertBefore(prev_btn,next_btn);

        }


        function removePreviousBtn ()
        {
            var prev_btn = document.getElementById('prev_btn');
            var parent = prev_btn.parentNode;

            parent.removeChild(prev_btn);
        }


        function insertNextBtn ()
        {
            var area = document.getElementById('scroll_btns');

            var next_btn = document.createElement('input');
            next_btn.setAttribute('type', 'button');
            next_btn.setAttribute('id', 'next_btn');
            area.appendChild(next_btn);
        }


        function removeNextBtn ()
        {
            var next_btn = document.getElementById('next_btn');
            var parent = next_btn.parentNode;

            parent.removeChild(next_btn);
        }
    </script>
<?php  } ?>

<div class=bodyHD>View Customers - <span style="color:#BF0E38;"><?=$customer_row['full_name'];?></span>  - <span style="color:#BF0E38;"><?=$customer_row['customer_number'];?></span> (<?=$customer_row['member_type']?>)</div>
<div class=bodyHD2>Account Statement</div>
<div class=subnavmenu>
    <?printLink(refpage("customers/customers"), "Main Menu");?>
    &nbsp;&gt;&nbsp;
    <?printLink(refpage("customers/customer_view&customer_id=$customer_id", "customers/customer_view_head"), "Personal Details");?>
    &nbsp;&gt;&nbsp;
    <b>Account Statement</b>
</div>
<br>
<table cellpadding=2 cellspacing=1 border=0 width=400>
    <tr>
        <td class=label>Time</td>
        <td class=label>Current Balance</td>
    </tr>
    <tr>
        <td class=content><?=shortDate(dbDateToday(false))?></td>
        <td class=content><?printInDollars($customer_row['credits'])?></td>
    </tr>
</table>
<br>
<?php
if ( isset($_POST['do_search']) ) {
    $ret_msg = "<span style=\"font-weight:bold;color:navy\">$num_recs records found</span><br>" .
        "<span style=\"font-weight:normal;color:navy\">Start Date:</span> $start_date_display" . '<br>' .
        "<span style=\"font-weight:normal;color:navy\">End Date:</span> $end_date_display" . '<br>';

    if ( true == $display_results ) {
        ?>              <table cellpadding="0" cellspacing="0" border="0">
            <tr>
                <td>
                    <table cellpadding="0" cellspacing="0" border="0" width="100%">
                        <tr valign="top">
                            <td style="font-size:8pt"><?echo $ret_msg;?></td>
                            <td align="right">
                                <a href="<?=temp_download_url."/$excel_fname?".mktime()?>">
                                    <img alt="Download as Excel File" src="<?=image_dir?>/icon_download.jpg" border="0"></a>
                                <?printClassLink(temp_download_url."/$excel_fname?".mktime(), "Download Excel File");?>
                            </td>
                        </tr>
                    </table>
                    <br>


                    <div id="urlContent">
                        <?php

                        $use_global_vars = true;
                        include 'account_statement_inr.php';
                        ?>
                    </div>

                    <div id="scroll_btns">
                        <?php
                        if ( $num_recs > $recs_per_page ) {
                            $last_rec= $start_rec + $recs_per_page -1; // last row in this screen

                            if ( $num_recs_remain > 0 )  // if we have more records to display
                            {
                                $num_next_scr = min($num_recs_remain, $recs_per_page);
                                $next_rec = $last_rec + 1;

                                ?>
                                <input type="button" id="next_btn" value="Next <?=$num_next_scr?> records" onClick="getNextBatch(<?=$next_rec?>)"/>
                            <?php
                            }
                        }

                        ?>
                    </div>
                </td>
            </tr>
        </table>
        </div>
    <?php
    }
    else
    {
        printInfoMessage($ret_msg);
    }
}
?>


<p>
    <?showErrors();?>
</p>
<p>
    <?form_head()?>
    <input type="hidden" name="do_search" value="yes">
    <input type="hidden" name="customer_id" value="<?=$customer_id?>">
<table cellpadding=4 cellspacing=1 border=0>
    <tr>
        <td class=formheading colspan=2>
            Transaction Search
        </td>
    </tr>
    <tr valign=top>
        <td class=label>
            Start Date
        </td>
        <td class=content>
            <?php
            //  if ($_SESSION["admin_id"] != 1320) {
            date_selector('start_day', 'start_month', 'start_year', 2002, date('Y'),  dbDateToday(false));
            //  } else {
            //			  limited_date_selector('start_day', 'start_month', 'start_year');
            //  }
            ?>
        </td>
    </tr>
    <tr valign=top>
        <td class=label>
            End Date
        </td>
        <td class=content>
            <?php
            if ($_SESSION["admin_id"] != 1320) {
                date_selector('end_day', 'end_month', 'end_year', 2002, date('Y'),  dbDateToday(false));
            } else {
                limited_date_selector('end_day', 'end_month', 'end_year');
            }
            ?>
        </td>
    </tr>
    <tr valign=top>
        <td class=label>
            Game Results
        </td>
        <td class=content>
            <input type="radio" name="show_all_games" value="" <?if ( empty($show_all_games) ) echo 'checked'?>> Summarize
            &nbsp;&nbsp;&nbsp;&nbsp;
            <input type="radio" name="show_all_games" value="yes" <?if ( 'yes' == $show_all_games ) echo 'checked'?>> Show all
        </td>
    </tr>
    <tr valign=top>
        <td class=label>
            Maximum Records per Page
        </td>
        <td class=content>
            <select name="recs_per_page">
                <?printOption('recs_per_page', 20, 20);?>
                <?printOption('recs_per_page', 50, 50);?>
                <?printOption('recs_per_page', 100, 100, $is_default=TRUE);?>
                <?printOption('recs_per_page', 200, 200);?>
            </select>
        </td>
    </tr>
    <tr>
        <td>&nbsp;</td>
        <td><input type=image src="<?=image_dir?>/button_submit.gif"></td>
    </tr>
</table>
</p>
<? include('footer.inc'); ?>
