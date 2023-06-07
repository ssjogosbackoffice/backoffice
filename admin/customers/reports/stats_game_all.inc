<?

$cache_filename = 'game_stats_gmall.'.session_id();
$excel_filename = 'game_stats_gmall.'.session_id().'.xls';

$heading_format_arr = array();
//drillDownLink(refPage($page, $header_page)."&mem_type=".getPostGetVar('mem_type')."&islive=".getPostGetVar('islive')."&game_id=$game_id&drill_down=$drill_down&date=$db_date", $game_name);

$func = ($member_type == 'FINANCIAL' ? 'getInDollars' : 'getInFunTokens' );

$val_fmt_arr = array (
    'Time' => 'return getClassLink("javascript:openWindow(\''.secure_host.'/admin/customers/reports/bet_details_pop.html?res=".$rec["Result Id"]."&customer_id='.$customer_id.'\',\'resdetailspop\', 450,450, \'no\',\'yes\')", shortDate($rec["Time"],true), "contentlink");',
    'Bets' =>'return '.$func.'($rec["Bets"]+0);',
    'Payouts' =>'return '.$func.'($rec["Payouts"]+0);',
    'Casino Revenue' =>'return '.$func.'($rec["Casino Revenue"]+0);',
    'Payout Percentage'=>'return number_format($rec["Payout Percentage"]+0,2)."%";'
);

$exl_val_fmt_arr = array (
    'Time' => 'return shortDate($rec["Time"],true);',
    'Bets' =>'return '.$func.'($rec["Bets"]+0);',
    'Payouts' =>'return '.$func.'($rec["Payouts"]+0);',
    'Casino Revenue' =>'return '.$func.'($rec["Casino Revenue"]+0);',
    'Payout Percentage'=>'return number_format($rec["Payout Percentage"]+0,2)."%";'
);



$cell_fmt_arr = array (
    'Time' => 'class=content align=right',
    'Bets' => 'class=content align=right',
    'Payouts' => 'class=content align=right',
    'Casino Revenue' => 'class=content align=right',
    'Payout Percentage' => 'class=content align=right',
    'Game Number' => 'class=content align=right',
);


$sum_fmt_arr = array(
    'Bets'=>'return '.$func.'($totals["Bets"]+0);',
    'Payouts'=>'return '.$func.'($totals["Payouts"]+0);',
    'Casino Revenue'=>'return '.$func.'($totals["Casino Revenue"]+0);',
    'Payout Percentage'=>'return ( $totals["Bets"] > 0 ? number_format($totals["Payouts"]/$totals["Bets"]*100,2) : 0)."%";'
);


if ( 1 == $num_days )
{  $date_db = $start_date;
    $day   = $start_day;
    $month = $start_month;
    $year   = $start_year;

    $date_disp =	shortDate($date_db);
}

if ( $doQuery )
{	$curr_mktime = $start_date_mktime;
    $curr_day = $start_day;
    $curr_month = $start_month;
    $curr_year = $start_year;

    $num_rows = 0;
    $i=0;

    $max_date_mktime  =  mktime(0,0,0,$month,$day+1,$year);
    $max_date = phpTsToDbTs($max_date_mktime);

    $sql  = 'SELECT res_game_num, res_date as "Time", gam_id as "Game Id", gam_name as "Game",';
    $sql .= ' res_id as "Result Id", res_game_num as "Game Number",  sum(pre_bet) as "Bets",';
    $sql .= ' sum(pre_win) as "Payouts", pre_void';
    $sql .= " FROM punter_result, result, game";
    $sql .= " WHERE pre_res_id = res_id";
    if ( $game )
        $sql .= " AND res_gam_id = $game";
    $sql .= " AND res_gam_id = gam_id";
    $sql .= " and res_date >= '$date'";
    $sql .= " and res_date < '$max_date'";

    if ( 'FINANCIAL' == $customer_row['member_type'] )
        $sql .= " and pre_member_type = 'FINANCIAL'";
    $sql .= "and pre_pun_id = $customer_id";

    $sql .= ' group by res_date, res_game_num,gam_id, gam_name,res_id,pre_member_type ';
    $sql .= ' order by res_date';



    $rs = $dbh->exec($sql);
    $num_rows = $rs->getNumRows();

    if ( $num_rows > 0 )
    {	$rs->resetRecPtr();

        while ( $rs->next() )
        {	$rec = & $rs->currentRef();
            $rec['Casino Revenue'] = $rec['Bets'] - $rec['Payouts'];
            $rec['Payout Percentage'] = ( $rec['Bets'] > 0 ? $rec['Payouts']  / $rec['Bets'] * 100 : 0);
        }

        if ( $num_rows > $max_rows )
            $rs->serialize($serialize_filename);
        else
            $noserialize = true;

        $qp = new QueryPrinter($rs);
        $cols_arr = array('Time','Bets', 'Payouts', 'Casino Revenue', 'Payout Percentage', 'Game', 'Game Number');
        $qp->dumpToExcelFile($cols_arr,$exl_val_fmt_arr,$excel_filename);

        $start_row = 1;
    }
}


if ( $num_rows > 0 )
{	$post_vars = array('page'=>$page,'fs'=>$fs, 'start_date'=>$start_date,'end_date'=>$end_date,'game'=>$game, 'phase'=>$phase, 'date'=>$date, 'member_type'=>$member_type,'report_type'=>$report_type,'prev'=>$old_prev, 'customer_id'=>$customer_id);
    $cols_arr = array('Time','Bets', 'Payouts', 'Casino Revenue', 'Payout Percentage', 'Game', 'Game Number');
    if ( ! $noserialize )
        $rs = RecordSet::unserialize($serialize_filename);
    $qp = new QueryPrinter($rs);
    //$header = '<tr><td class=label>Day</td><td class=label>Bets</td><td class=label>Payouts</td><td class=label>Casino Revenue</td><td class=label>Payout Percentage</td><td class=label>Game</td>';
    $qp->printTable($cols_arr, $post_vars, $val_fmt_arr, $cell_fmt_arr, $sum_fmt_arr, $excel_filename, $header="", $width="", $start_row, $max_rows);

}
else
{	?>
    <br>
    <?	printInfoMessage("0 Records found");
}
?>