<?
$cache_filename = 'game_stats.'.session_id();
$excel_filename = 'game_stats.'.session_id().'.xls';

$func = ($member_type == 'FINANCIAL' ? 'getInDollars' : 'getInFunTokens' );
$val_fmt_arr = array (
    'Game' => 'if ( $rec["Bets"] > 0 ) return getClassLink("'.refPage('customers/reports/stats&customer_id='.$customer_id.'&phase=showgame&fs=yes&report_type='.$report_type.'&prev=mn&phase=showgame&doQuery=yes&max_rows='.$max_rows.'&start_date='.$start_date.'&end_date='.$end_date).'&game=".$rec["Game Id"], $rec["Game"], "contentlink"); else return $rec["Game"];',
    'Bets' =>'return '.$func.'($rec["Bets"]+0);',
    'Payouts' =>'return '.$func.'($rec["Payouts"]+0);',
    'Casino Revenue' =>'return '.$func.'($rec["Casino Revenue"]+0);',
    'Payout Percentage'=>'return number_format($rec["Payout Percentage"]+0,2)."%";'
);


$exl_val_fmt_arr = array (
    'Bets' =>'return '.$func.'($rec["Bets"]+0);',
    'Payouts' =>'return '.$func.'($rec["Payouts"]+0);',
    'Casino Revenue' =>'return '.$func.'($rec["Casino Revenue"]+0);',
    'Payout Percentage'=>'return number_format($rec["Payout Percentage"]+0,2)."%";'
);


$cell_fmt_arr = array (
    'Bets' => 'class=content align=right',
    'Payouts' => 'class=content align=right',
    'Casino Revenue' => 'class=content align=right',
    'Payout Percentage' => 'class=content align=right',
);


$sum_fmt_arr = array(
    'Bets'=>'return '.$func.'($totals["Bets"]+0);',
    'Payouts'=>'return '.$func.'($totals["Payouts"]+0);',
    'Casino Revenue'=>'return '.$func.'($totals["Casino Revenue"]+0);',
    'Payout Percentage'=>'return ( $totals["Bets"] > 0 ? number_format($totals["Payouts"]/$totals["Bets"]*100,2) : 0)."%";'
);

if ( $doQuery )
{	//end_date_filter =
    $max_date_mktime  =  mktime(0,0,0,$end_month,$end_day+1,$end_year);
    $max_date = phpTsToDbTs($max_date_mktime);


    file_exists(cache_file_dir."/$cache_filename") && unlink(cache_file_dir."/$cache_filename");
    if (  'NONFINANCIAL' == $customer_row['member_type'] )
    {

        // handle voided results
        // NOTE: any changes to $sql must be reflected in $vql so that they're both accessing the same things.

        $vql = "FROM punter_result, result WHERE " .
            "pre_res_id =  res_id AND " .
            "res_date   >= '" . $start_date . "' AND " .
            "res_date   <  '" . $max_date . "' AND " .
            "pre_pun_id =  "  . $customer_id . " AND " .
            "pre_void > 0 AND " .
            "res_gam_id = gam_id";


        $sql  = "SELECT gam_id As \"Game Id\", gam_name AS \"Game\", SUM(pre_bet) - (SELECT coalesce(sum(pre_bet), 0) " . $vql . ") AS \"Bets\", " .
            "SUM(pre_win) - (SELECT coalesce(sum(pre_win), 0) " . $vql . ") AS \"Payouts\"";

        $sql .= " FROM game JOIN ( punter_result JOIN result ON  pre_res_id = res_id ";
        $sql .= "  and res_date >= '$start_date'";
        $sql .= " AND res_date < '$max_date' )";
        $sql .= " ON gam_id = res_gam_id";
        $sql .= " WHERE pre_pun_id = $customer_id";

        if ( $game )
            $sql .= " AND res_gam_id = $game ";


        $sql .= " GROUP BY gam_id, gam_name ORDER BY gam_name";
    }
    else
    {
        $sql  = "SELECT cus_res_gam_id As \"Game Id\", gam_name AS \"Game\", SUM(cus_res_bet) AS \"Bets\", SUM(cus_res_win) AS \"Payouts\"";
        $sql .= " FROM game left outer join customer_result_daily on gam_id = cus_res_gam_id";
        $sql .= "  where cus_res_day >= '$start_date'";
        $sql .= " and cus_res_day < '$max_date' ";
        if ( $game )
            $sql .= " AND cus_res_gam_id = $game";
        $sql .= " and cus_res_pun_id = $customer_id";
        $sql .= " GROUP BY cus_res_gam_id, gam_name ORDER BY gam_name";
    }



    $rs = $dbh->exec($sql);
    $num_rows = $rs->getNumRows();
    $start_row = 1;



    if ( $num_rows > 0 )
    {	while ( $rs->next() )
    {	$rec = & $rs->currentRef();
        $rec['Casino Revenue'] = $rec['Bets'] - $rec['Payouts'];
        $rec['Payout Percentage'] = ( $rec['Bets'] > 0 ? $rec['Payouts']  / $rec['Bets'] * 100 : 0);
    }
    }

///////////////////////////////////////////
    //get  results for today's date if requested
    $today_mktime = mktime(0,0,0,date('m'),date('d'), date('Y')); //get today's date, no time
    $today_date = phpTsToDbTs($today_mktime); //today's date in database timestampm not time

    //if search included  today's date
    if ( $start_mktime <= $today_mktime && $end_mktime >= $today_mktime &&  'FINANCIAL' == $member_type )
    {
        // handle voided results
        // NOTE: any changes to $sql must be reflected in $vql so that they're both accessing the same things.

        $vql = "FROM punter_result, result WHERE " .
            "pre_res_id =  res_id AND " .
            "res_date   >= '" . $today_date . "' AND " .
            "res_date   <  '" . $max_date . "' AND " .
            "pre_pun_id =  "  . $customer_id . " AND " .
            "pre_member_type = 'FINANCIAL' AND " .
            "pre_void > 0 AND " .
            "res_gam_id = gam_id";



        //select sum of bets and payouts for today
        $sql  = "SELECT gam_id As \"Game Id\", gam_name AS \"Game\", SUM(pre_bet) - (SELECT coalesce(sum(pre_bet), 0) " . $vql . ") AS \"Bets\", SUM(pre_win) - (SELECT coalesce(sum(pre_win), 0) " . $vql . ") AS \"Payouts\"";
        $sql .= " FROM game JOIN ( punter_result JOIN result ON  pre_res_id = res_id ";
        $sql .= "  and res_date >= '$today_date'";
        $sql .= " AND res_date < '$max_date' )";
        $sql .= " ON gam_id = res_gam_id";
        $sql .= " WHERE pre_pun_id = $customer_id";
        $sql .= " and pre_member_type = 'FINANCIAL'";

        if ( $game )
            $sql .= " AND gam_id = $game ";

        $sql .= " GROUP BY gam_id, gam_name ORDER BY gam_name";

        $rs2 = $dbh->exec($sql);
        $num_rows2 = $rs2->getNumRows();

        //if records found
        if ( $num_rows2 > 0 )
        {  	$rs2->resetRecPtr();
            while ( $rs2->hasNext() )  //append other columns to records
            {  $rec2 = $rs2->next();

                $rs->resetRecPtr();  //to start searching for same game

                //search for matching games in original result set for previous days
                $found = false;
                while ( $rs->next() )
                {  $rec = & $rs->currentRef();
                    if ( $rec['Game Id'] == $rec2['Game Id'] ) //match found
                    {  ///////add fields to original recordset
                        $rec['Bets'] += $rec2['Bets'];
                        $rec['Payouts'] += $rec2['Payouts'];
                        $rec['Casino Revenue'] = $rec['Bets'] - $rec['Payouts'];
                        $rec['Payout Percentage'] = ( $rec['Bets'] > 0 ? $rec['Payouts']  / $rec['Bets'] * 100 : 0);
                        $found = true;
                        break; //since found
                    }
                }
                if ( ! $found )
                {  $rec2['Casino Revenue'] = $rec2['Bets'] - $rec2['Payouts'];
                    $rec2['Payout Percentage'] = ( $rec2['Bets'] > 0 ? $rec2['Payouts']  / $rec2['Bets'] * 100 : 0);
                    $arr = array($rec2);
                    $rs->appendRecords($arr); //append totals for game to original recordset (game played today only)
                }
            }

            $num_rows = $rs->getNumRows(); //new number of rows
        }

    }

    if ( $num_rows > 0 )
    {
        $post_vars = array('page'=>$page,'fs'=>$fs, 'start_day'=>$start_day,'start_month'=>$start_month,'start_year'=>$start_year, 'end_day'=>$end_day,'end_month'=>$end_month,'end_year'=>$end_year,'payment_method'=>$payment_method, 'search_type'=>$search_type,'game'=>$game,'phase'=>$phase, 'report_type'=>$report_type,'prev'=>$old_prev,'max_rows'=>$max_rows, 'member_type'=>$member_type, 'customer_id'=>$customer_id);
        $cols_arr = array('Game','Bets', 'Payouts', 'Casino Revenue', 'Payout Percentage');

        $qp = new QueryPrinter($rs);
        $qp->dumpToExcelFile($cols_arr,$exl_val_fmt_arr,$excel_filename);
        $qp->printTable($cols_arr, $post_vars, $val_fmt_arr, $cell_fmt_arr, $sum_fmt_arr, $excel_filename, $header="", $width="", $start_row, $bygamemax_rows=60);
    }
    else
    {
        ?>
        <br>
        <?			       printInfoMessage("0 Records found");
    }
}
?>