<?

$cache_filename = 'game_stats_gm.'.session_id();
$excel_filename = 'game_stats_gm.'.session_id().'.xls';

if ( $prev )
{	if ( ! strstr($prev,'gm') )
    $prev .= '-gm';
}
else
    $prev = 'gm';

$func = ($customer_row['member_type'] == 'FINANCIAL' ? 'getInDollars' : 'getInFunTokens' );



$val_fmt_arr = array (
    'Day' => 'return getClassLink("'.refPage('customers/reports/stats&fs=yes&phase=showall&customer_id='.$customer_id.'&prev='.$prev.'&doQuery=yes&report_type='.$report_type.'&start_date='.$start_date.'&end_date='.$end_date.'&date=".$rec["Day"]."&game='.$game.'&max_rows='.$max_rows).'", shortDate($rec["Day"]), "contentlink");',
    'Bets' =>'return '.$func.'($rec["Bets"]+0);',
    'Payouts' =>'return '.$func.'($rec["Payouts"]+0);',
    'Total Casino Revenue' =>'return '.$func.'($rec["Total Casino Revenue"]+0);',
    'Total Payout Percentage'=>'return number_format($rec["Total Payout Percentage"]+0,2)."%";'
);

$exl_val_fmt_arr = array (

    'Bets' =>'return '.$func.'($rec["Bets"]+0);',
    'Payouts' =>'return '.$func.'($rec["Payouts"]+0);',
    'Total Casino Revenue' =>'return '.$func.'($rec["Total Casino Revenue"]+0);',
    'Total Payout Percentage'=>'return number_format($rec["Total Payout Percentage"]+0,2)."%";'
);


$cell_fmt_arr = array (
    'Day' => 'class=content align=right',
    'Bets' => 'class=content align=right',
    'Payouts' => 'class=content align=right',
    'Total Casino Revenue' => 'class=content align=right',
    'Total Payout Percentage' => 'class=content align=right'
);

$sum_fmt_arr = array(
    'Bets'=>'return '.$func.'($totals["Bets"]+0);',
    'Payouts'=>'return '.$func.'($totals["Payouts"]+0);',
    'Total Casino Revenue'=>'return '.$func.'($totals["Total Casino Revenue"]+0);',
    'Total Payout Percentage'=>'return ( $totals["Bets"] > 0 ? number_format($totals["Payouts"]/$totals["Bets"]*100,2) : 0)."%";'
);


if ( $doQuery )
{	$curr_mktime = $start_date_mktime;
    $curr_day = $start_day;
    $curr_month = $start_month;
    $curr_year = $start_year;

    $num_rows = 0;
    $i=0;

    $max_date_mktime  =  mktime(0,0,0,$end_month,$end_day+1,$end_year);
    $max_date = phpTsToDbTs($max_date_mktime);

    if (  'NONFINANCIAL' == $customer_row['member_type'] )
    {
        // handle voided results

        // NOTE: any changes to $sql must be reflected in $vql so that they're both accessing the same things.

        $vql = "FROM punter_result, result WHERE " .
            "pre_res_id =  res_id AND " .
            "res_date   >= '" . $start_date . "' AND " .
            "res_date   <  '" . $max_date . "' AND " .
            "pre_pun_id =  "  . $customer_id . " AND " .
            "pre_void > 0";

        if($game)
            $vql .= " AND res_gam_id = $game";

        $sql  = 'SELECT '.sqlTimestampToY_M_D('res_date'). ' as "Day", sum(pre_bet) - (SELECT coalesce(sum(pre_bet), 0) ' . $vql .') as "Bets", ' .
            'sum(pre_win) - (SELECT coalesce(sum(pre_win), 0) ' . $vql . ') as "Payouts"';

        $sql .= " FROM punter_result, result";
        $sql .= " WHERE pre_res_id = res_id";
        $sql .= " and res_date >= '$start_date'";
        $sql .= " and res_date < '$max_date'";

        if ( $game )
            $sql .= " AND res_gam_id = $game";

        $sql .= " and pre_pun_id = $customer_id";

        $sql .= " group by ".sqlTimestampToY_M_D('res_date');
    }
    else
    {
        $sql  = "SELECT cus_res_day As \"Day\",  SUM(cus_res_bet) AS \"Bets\", SUM(cus_res_win) AS \"Payouts\"";
        $sql .= " FROM customer_result_daily";
        $sql .= "  where cus_res_day >= '$start_date'";
        $sql .= " and cus_res_day < '$max_date' ";
        if ( $game )
            $sql .= " AND cus_res_gam_id = $game";
        $sql .= " and cus_res_pun_id = $customer_id";
        $sql .= " GROUP BY cus_res_day ORDER BY cus_res_day";
    }

    $rs = $dbh->exec($sql);
    $num_rows = $rs->getNumRows();

    $rs->resetRecPtr();
    while ( $rs->next() )
    {  $rec = & $rs->currentRef();
        $rec['Total Casino Revenue']    = $rec['Bets'] - $rec['Payouts'];
        $rec['Total Payout Percentage'] = ( $rec['Bets'] > 0 ? $rec['Payouts']  / $rec['Bets'] * 100 : 0);
        $rec['Game'] = ( $game ? ucwords($game_name) : 'All Games');
    }

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
            "pre_void > 0";

        if($game)
            $vql .= " AND res_gam_id = $game";

        //select sum of bets and payouts for today
        $sql  = 'SELECT '.sqlTimestampToY_M_D('res_date'). ' as "Day", sum(pre_bet) - (SELECT coalesce(sum(pre_bet), 0) ' . $vql . ') as "Bets", ' .
            'sum(pre_win) - (SELECT coalesce(sum(pre_win), 0) ' . $vql . ') as "Payouts"';

        $sql .= " FROM punter_result, result";
        $sql .= " WHERE pre_res_id = res_id";
        $sql .= " and res_date >= '$today_date'";
        $sql .= " and res_date < '$max_date'";
        $sql .= " AND pre_pun_id = $customer_id";
        $sql .= " and pre_member_type = 'FINANCIAL'";

        if ( $game )
            $sql .= " AND res_gam_id = $game";

        $sql .= " group by ".sqlTimestampToY_M_D('res_date');
        $rs2 = $dbh->exec($sql);
        $num_rows2 = $rs2->getNumRows();

        //if records found
        if ( $num_rows2 > 0 )
        {  	$rs2->resetRecPtr();
            while ( $rs2->next() )  //append other columns to records
            {  $rec2 = & $rs2->currentRef();
                $rec2['Total Casino Revenue']    = $rec2['Bets'] - $rec2['Payouts'];
                $rec2['Total Payout Percentage'] = ( $rec2['Bets'] > 0 ? $rec2['Payouts']  / $rec2['Bets'] * 100 : 0);
                $rec2['Game'] = ( $game ? ucwords($game_name) : 'All Games');
            }
        }
        $rs->appendRecords($rs2->Records); //append today's records to original recordset
        $num_rows = $rs->getNumRows(); //new number of rows
    }


    if ( $num_rows > 0 )
    {
        if ( $num_rows > $max_rows )
            $rs->serialize($serialize_filename);
        else
            $noserialize = true;

        $qp = new QueryPrinter($rs);
        $cols_arr = array('Day','Bets', 'Payouts', 'Total Casino Revenue', 'Total Payout Percentage', 'Game');
        $qp->dumpToExcelFile($cols_arr,$exl_val_fmt_arr,$excel_filename);
    }

    $start_row = 1;
}

globalize('num_rows_displayed');

if ( $num_rows > 0 )
{	$post_vars = array('page'=>$page,'fs'=>$fs, 'start_date'=>$start_date,'end_date'=>$end_date, 'game'=>$gam_id,'phase'=>$phase,'report_type'=>$report_type,'prev'=>$old_prev, 'member_type'=>$member_type,'customer_id'=>$customer_id);
    $cols_arr = array('Day','Bets', 'Payouts', 'Total Casino Revenue', 'Total Payout Percentage', 'Game');
    if ( ! $noserialize )
        $rs = RecordSet::unserialize($serialize_filename);
    $qp = new QueryPrinter($rs);
    $header = '<tr><td class=label>Day</td><td class=label>Bets</td><td class=label>Payouts</td><td class=label>Casino Revenue</td><td class=label>Payout Percentage</td><td class=label>Game</td>';
    $qp->printTable($cols_arr, $post_vars, $val_fmt_arr, $cell_fmt_arr, $sum_fmt_arr, $excel_filename, $header="", $width="", $start_row, $max_rows);

}
else
{	?>
    <br>
    <?	printInfoMessage("0 Records found");
}
?>