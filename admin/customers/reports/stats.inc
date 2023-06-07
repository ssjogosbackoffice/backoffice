<?
globalize ('fs');
globalize ('doQuery');
globalize ('reference_number');
globalize ('game');
globalize('date');
globalize('phase');
globalize('prev');
globalize('num_rows_displayed');
globalize('num_rows');
globalize('max_rows');
globalize('start_row');
globalize('report_type');
globalize('customer_id');

$old_prev = $prev;
$customer_id = (int)$customer_id;
$customer_row = getCustomer($customer_id);
$member_type = $customer_row['member_type'];

$serialize_filename = 'game-stats.sz.'.session_id();
?>
    <div class=bodyHD>
        View Customers - <?=$customer_row['full_name'];?> (<?=$customer_row['member_type']?>)
    </div>
    <div class=bodyHD2>
        Betting History
    </div>
<div class=subnavmenu>
<?printLink(refpage("customers/customers"), "Main Menu");?>
    &nbsp;&gt;&nbsp;
<?printLink(refpage("customers/customer_view&customer_id=$customer_id"), "Personal Details");?> &gt;
<? if ( ! $fs  )
    echo "Game result search";


$game = (int)$game;
$game_name = getGameName($game);

if ( $fs  )
{	$start_date = getPostGetVar('start_date');
    $end_date = getPostGetVar('end_date');

    if ( $start_date  )
    {	$start_date_arr = splitDate($start_date);
        $end_date_arr   = splitDate($end_date);

        //start date parameters
        $start_day   = (int)$start_date_arr['day'];
        $start_month = (int)$start_date_arr['month'];
        $start_year  = (int)$start_date_arr['year'];
        //end date parameters
        $end_day   	 = (int)$end_date_arr['day'];
        $end_month   = (int)$end_date_arr['month'];
        $end_year    = (int)$end_date_arr['year'];
    }
    else
    {	//start date parameters
        $start_day   = (int)$_POST['start_day'];
        $start_month = (int)$_POST['start_month'];
        $start_year  = (int)$_POST['start_year'];
        //end date parameters
        $end_day   	 = (int)$_POST['end_day'];
        $end_month   = (int)$_POST['end_month'];
        $end_year    = (int)$_POST['end_year'];
    }
    $start_mktime = mktime(0,0,0,$start_month, $start_day, $start_year);
    $end_mktime   = mktime(0,0,0,$end_month, $end_day, $end_year);


    if ( $date )
    {	//search date
        $date_arr = splitDate($date);
        $day  = $date_arr['day'];
        $month  = $date_arr['month'];
        $year = $date_arr['year'];
        $hour = $date_arr['hour'];

        $date_mktime = mktime($hour,0,0,$month,$day,$year);
        $date_db = phpTsToDb($date_mktime);
        $date_disp = shortDate($date_db);
    }


    function checkFields($start_day, $start_month, $start_year, $end_day, $end_month, $end_year, $game)
    {	if ( ! $start_day || ! $start_month || ! $start_year )
        addError("", "The full start date must be selected");
    else
        if ( ! checkDate($start_month,$start_day,$start_year) )
            addError("", "The start date you selected is not a valid calendar date");

        if ( ! $end_day || ! $end_month || ! $end_year )
            addError("", "The full end date must be selected");
        else
            if ( ! checkDate($end_month,$end_day,$end_year) )
                addError("", "The end date you selected is not a valid calendar date");

        if ( ! areErrors() )
            if ( mktime(0,0,0,$start_month,$start_day,$start_year) > mktime(0,0,0,$end_month,$end_day,$end_year) )
                addError("","The end date is earlier than the start date");

        if ( $game && ! getGameName((int)$game) )
            addError("", "Invalid Game");

        return ! areErrors();
    }



    if ( checkFields($start_day, $start_month, $start_year, $end_day, $end_month, $end_year, $game) )
    {	$start_date = toDatabaseTimestamp($start_day,$start_month,$start_year);
        $end_date   = toDatabaseTimestamp($end_day,$end_month,$end_year);

        $start_date_display = shortDate($start_date);
        $end_date_display = shortDate($end_date);


        $duration_sec = $end_date_mktime - $start_date_mktime;

        $old_prev = $prev;
        $prev_arr = array();
        if ( $prev )
            $prev_arr = explode('-',$prev);

        $link_prev_arr = $prev_arr;
        array_pop($link_prev_arr);
        $link_prev = implode('-', $link_prev_arr);

        $get_str = "customer_id=$customer_id&doQuery=yes&fs=yes&start_date=$start_date&end_date=$end_date&date=$date&member_type=$member_type&prev=$link_prev&report_type=$report_type&max_rows=$max_rows";

        $rt = ( $report_type == 'game' ? "by game" : "by date");
        ?>
        <?
        if ( 'showall' == $phase )
        {	if ( in_array('mn', $prev_arr) )
        {	printLink(refPage("$page&$get_str"), "Totals by game for all games for $start_date_display to $end_date_display");
            echo ' &gt; ';
        }

            if ( in_array('gm', $prev_arr) )
            {
                $st_game_name = ucwords($game_name);

                $txt = ( $game ? "Daily totals for $st_game_name $start_date_display to $end_date_display" : "Daily totals for all games $start_date_display - $end_date_display" );

                printLink(refPage("$page&$get_str&phase=showgame&game=$game"), $txt);
                echo ' &gt; ';
            }

            if ( $game )
                echo "<b>All Results for $game_name for $date_disp</b>";
            else
                echo "<b>All Results for all games for $date_disp</b>";
            ?></div><br><?
            include(include_content('customers/reports/stats_game_all.inc'));
        }
        elseif ( 'showgame' == $phase )
        {	if ( in_array('mn', $prev_arr) )
        {
            if ( 'game' == $report_type )
                printLink(refPage("$page&$get_str"), "Totals (by game) for $start_date_display to $end_date_display");
            else
                printLink(refPage("$page&$get_str"), "Daily Totals for $start_date_display to $end_date_display");

            echo ' &gt; ';
        }

            echo "<b>Daily Totals for $game_name for $start_date_display to $end_date_display</b>";
            ?></div><br><?
            include(include_content('customers/reports/stats_game.inc'));
        }
        else
        {	if ( $game  )
        {	echo "<b>Daily Totals for $game_name for $start_date_display to $end_date_display</b>";
            ?></div><br><?
            include(include_content('customers/reports/stats_game.inc'));
        }
        else
        {
            if ( 'game' == $report_type )
            {
                echo "<b>Totals by game for $start_date_display to $end_date_display</b>";
                ?></div><br><?
                include(include_content('customers/reports/stats_main.inc'));
            }
            else
            {
                echo "<b>Daily Totals for all games for $start_date_display to $end_date_display</b>";
                ?></div><br><?
                include(include_content('customers/reports/stats_game.inc'));
            }
        }
        }

    }
}
else
{?></div><br><?
}


showErrors();
if ( !$phase || ! $old_prev )
{
    ?>
    <P>
        <?showErrors();?>
    <P>
        <?form_head()?>
        <?printHiddenInput('fs', 'yes');?>
        <?printHiddenInput('doQuery', 'yes');?>
        <?printHiddenInput('search_type', 'date_range');?>
        <?printHiddenInput('customer_id');?>
    <table cellpadding=4 cellspacing=1 border=0>
        <tr>
            <td class=formheading colspan=2>
                Game Activity Search
            </td>
        </tr>
        <tr valign=top>
            <td class=label>
                Start Date
            </td>
            <td class=content>
                <?date_selector('start_day', 'start_month', 'start_year', 2002, date('Y'),  dbDateToday());?>
            </td>
        </tr>
        <tr valign=top>
            <td class=label>
                End Date
            </td>
            <td class=content>
                <?date_selector('end_day', 'end_month', 'end_year', 2002,  date('Y'), dbDateToday());?>
            </td>
        </tr>
        <tr valign=top>
            <td class=label>
                Game
            </td>
            <td class=content>
                <select name="game">
                    <option value=""> - All games - 	<?	$dbh = new DatabaseHandler();
                        $dbh->connect();
                        $rs = $dbh->exec("select gam_id, gam_name from game order by gam_name");

                        while ( $pm = $rs->next() )
                        {	printOption('game', $pm['gam_id'], ucwords($pm['gam_name']));
                        }
                        ?>			</select>
            </td>
        </tr>
        <tr valign=top>
            <td class=label>
                Report Type
            </td>
            <td class=content>
                <input type="radio" name="report_type" value="date" <?if ('date'==$report_type || ! $report_type ) echo ' checked'?>> By Date
                &nbsp;&nbsp;&nbsp;&nbsp;
                <input type="radio" name="report_type" value="game" <?if ('game'==$report_type ) echo ' checked'?>> By Game
            </td>
        </tr>
        <tr>
            <td class=label>Records per page</td>
            <td class=content>
                <select name="max_rows">
                    <?printOption('max_rows', 40,'40');?>
                    <?printOption('max_rows', 60,'60', $is_default=true);?>
                    <?printOption('max_rows', 100,'100');?>
                </select>
            </td>
        </tr>
        <tr>
            <td>&nbsp;</td>
            <td><input type =image src="<?=image_dir?>/button_submit.gif"></td>
        </tr>
    </table>
<?
}
?>