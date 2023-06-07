<?php
/**
 *
 * @author Marian
 */
require_once 'account_functions.php';
if(isset($_GET['action'])){
    switch($_GET['action'])
    {
        case "1":
            $jurisdiction=$_GET['id'];
            $class=getSubJurisdiction($_GET['class']);
            if($class==''){
                $class=getSubJurisdiction($_SESSION['jurisdiction_class']);
            }
            if($jurisdiction=='#'){
                $jurisdiction=$_SESSION['jurisdiction_id'];
            }
            $nodes=getVirtualJurisdiction($jurisdiction,$class);

            if($nodes->getNumRows()>0){
                $return='<ul>';
                while ( $nodes->hasNext () ) {
                    $row = $nodes->next ();
                    $return.="<li rel='".$class."'";
                    if($class!='club'){
                        $return.="class='jstree-closed $class' ";
                    }
                    else{
                        $return.="class='$class' ";
                    }
                    $return.=" data-jstree='{ \"type\" : \"$class\" }' id='".$row[$class."_id"]."'  >".$row["$class"]."</li> ";
                }
                $return.="</ul>";
            }else{
                $return .="<div class='alert alert-error'><strong>-1</strong></div>";
            }
            die(var_dump($return));
            break;
        default:
            die('Invalid operation');
            break;
    }
}

if(isset($_POST['action'])){
    switch($_POST['action'])
    {
        case '1':{
            if(isset($_SESSION['admin_id'])){
                if($_POST['section']=="ticket"){
                    $tid=$_POST['ticket'];
                    $date=$_POST['date_start'];
                    $game=$_POST['games'];
                    $jurisdiction=$_SESSION['jurisdiction_id'];
                    die(getTicketsAgency($date,$game,$tid,$jurisdiction));
                }
                elseif($_POST['section']=="financial"){
                    $jurisdiction=$_SESSION['jurisdiction_id'];
                    $date=$_POST['date_start'];
                    $group=$_POST['group'];
                    die(getFinancialRecords($date,$jurisdiction,$group));
                }
                else {
                    $jurisdiction=$_SESSION['jurisdiction_id'];
                    die(getJurisdictionVirtualDetails($jurisdiction));
                }

            }
            else{
                die("<div class='alert alert-error'>Please login again!</div>");
            }
            break;
        }
        case '2':{
            $_jurisdiction=$_POST['club'];
            die(showUsersConnectedByClub($_jurisdiction));
            break;
        }
        case '3':
        {
            $jur_id=$_POST['jurid'];
            getAgencyGames($jur_id);
            break;
        }

        case '4':
        {
            $_jurisdiction=$_POST['jurisdictions'];
            die(getVirtualUsers($_jurisdiction,true));
            break;
        }
        case '5':
        {
            $_customer_id=$_POST['customer'];
            $_amount=$_POST['amount'];
            $_type=$_POST['type'];
            die(doDepositWithdraw($_customer_id,$_amount,$_type));
            break;
        }


    }
}


/**
 * @param $jurisdiction
 * @param $class
 * @return bool|int|RecordSet
 */
function getVirtualJurisdiction($jurisdiction,$class){
    global $dbh;
    $today= date("Y-m-d", time());
    $sql="SELECT n.jur_name nation, n.jur_id nation_id, r.jur_name region, r.jur_id region_id, d.jur_name district, d.jur_id district_id,
        c.jur_name club, c.jur_id club_id
        FROM virtual_games_user
        LEFT JOIN punter ON vgu_pun_id = pun_id
        LEFT JOIN jurisdiction c ON pun_betting_club = c.jur_id
        LEFT JOIN jurisdiction d ON c.jur_parent_id = d.jur_id
        LEFT JOIN jurisdiction r ON d.jur_parent_id = r.jur_id
        LEFT JOIN jurisdiction n ON r.jur_parent_id = n.jur_id
         ";

    if($class=='nation')
    {
        $sql.="
         GROUP BY n.jur_id";
    }
    elseif($class=='region')
    {
        $sql.="WHERE n.jur_id=$jurisdiction
        GROUP BY r.jur_id";

    }
    elseif($class=='district'){
        $sql.="WHERE r.jur_id=$jurisdiction
        GROUP BY d.jur_id";
    }
    elseif($class=='club'){

        if($_SESSION['jurisdiction_class']=='club'){
            $sql.="WHERE c.jur_id=$jurisdiction
            GROUP BY c.jur_id";
        }
        else{
            $sql.="WHERE d.jur_id=$jurisdiction
            GROUP BY c.jur_id";
        }
    }

    $result=$dbh->doCachedQuery($sql,60);
    return $result;

}

/**
 * @param $jurisdictionClass
 * @return string
 */
function getSubJurisdiction($jurisdictionClass){
    $subjurisdiction='';

    if($jurisdictionClass=='casino')
    {
        $subjurisdiction='nation';
    }
    elseif($jurisdictionClass=='nation')
    {
        $subjurisdiction='region';
    }
    elseif($jurisdictionClass=='region')
    {
        $subjurisdiction='district';
    }
    elseif($jurisdictionClass=='district'){
        $subjurisdiction='club';
    }
    elseif($jurisdictionClass=='club'){
        $subjurisdiction='club';
    }
    return $subjurisdiction;
}


/**
 * @param $club
 * @return mixed
 */
function getGameAndCashier($club){

    $info=explode('_',$club);
    $last=count($info)-2;
    if(is_numeric($info[$last])){

        $return['cashier']=$info[$last];
    }
    else{
        $return['cashier']=1;
    }
    $return['game']=$info[0];
    return $return;
}

/**
 * @param $nation
 * @param $region
 * @param $district
 * @param $club
 * @return string
 */
function getJurisdictionHierarchy($nation,$region,$district,$club){
    if($_SESSION['jurisdiction_class']=='casino'){
        $return= "<span class='nation large'>Nation $nation </span><img src='/media/images/arrow_r.gif'><span class='region large'>Region $region </span> <img src='/media/images/arrow_r.gif'> <span class='district large'>District $district </span> <img src='/media/images/arrow_r.gif'> <span class='club large'>Club $club</span>";
    }
    if($_SESSION['jurisdiction_class']=='nation'){
        $return="<span class='region large'>Region $region </span> <img src='/media/images/arrow_r.gif'> <span class='district large'>District $district </span> <img src='/media/images/arrow_r.gif'> <span class='club large'>Club $club</span>";
    }
    if($_SESSION['jurisdiction_class']=='region'){
        $return=" <span class='district large'>District $district </span> <img src='/media/images/arrow_r.gif'> <span class='club large'>Club $club</span>";
    }
    if($_SESSION['jurisdiction_class']=='district' || $_SESSION['jurisdiction_class']=='club' ){
        $return="<span class='club large'>Club $club</span>";
    }
    return $return;
}

/**
 * @param $date
 * @param $game
 * @param $tid
 * @param $jurisdiction
 * @return string
 */
function getTicketsAgency($date,$game,$tid,$jurisdiction){
    global $dbh,$lang;
    $tid=  preg_replace('#\D.*$#', '', $tid);
    $betting='';
    if($game==215){
     $betting="&betting=true";
        $sql="
        SELECT  pun_username,
                IDTicket PRE_RES_ID,
                CodiceTicket pre_game_num,
                c.jur_name club_name,
               IDUtente PRE_PUN_ID,
               IF(TStato <> 4, coalesce(Importo, 0) + coalesce(Tax, 0), 0)  PRE_BET,
               if (Tax>0, (TVincita) * 0.01, 0) tot_win_tax,
               TVincita PRE_WIN,
               IPAddress PRE_PUN_IP,
               TStamp pre_time,
               '-' PRE_BALANCE,
               Pagato is_payed,
               Tstato,
               TAccreditato,
               Tax pre_bet_tax,
               (case when Pagato = 1 and Tax > 0 then (TVincita) * 0.01 else 0 end) tickets_payed_win_tax,
               c.jur_id jur_id
               FROM Ticket_dev, virtual_games_user, punter, jurisdiction c, jurisdiction d, jurisdiction r, jurisdiction n
               WHERE ExtIDUtente = vgu_pun_id
               AND pun_id = vgu_pun_id
                AND TStamp BETWEEN '$date 00:00:00' AND '$date 23:59:59'
                AND c.jur_id = PUN_BETTING_CLUB
                AND c.jur_parent_id = d.jur_id
                AND d.jur_parent_id = r. jur_id
                AND r.jur_parent_id = n.jur_id
                AND n.jur_parent_id = 1
        ";
        $jur_id = $_SESSION["jurisdiction_id"];
        switch($_SESSION["jurisdiction_class"]){
            case "club":
                $sql .= " AND c.jur_id = $jur_id";
                break;
            case "district":
                $sql .= " AND d.jur_id = $jur_id";
                break;
            case "region":
                $sql .= " AND r.jur_id = $jur_id";
                break;
            case "nation":
                $sql .= " AND n.jur_id = $jur_id";
                break;
            case "casino":
                $sql .= " AND 1=1";
                break;
        }
        if($tid!=''){ $sql.=" AND (CodiceTicket = $tid OR IDTicket=$tid) ";}
    }
    else{
    $sql=" SELECT pun_username,
            PRE_RES_ID, pre_game_num, club.jur_name  as club_name, PRE_PUN_ID, PRE_BET, PRE_WIN, PRE_BALANCE, PRE_TBL_ID, PRE_PUN_IP, pre_bet_string , res_id, pre_time, pre_balance,
            coalesce(case when pre_bets_paid like '%payed' and pre_bet_tax>0 then (pre_win) *0.01 else 0 end) tickets_payed_win_tax,
             if (pre_bet_tax>0 , (pre_win) * 0.01, 0) tot_win_tax,
             IF (pre_bets_paid like '%payed', 1, 0) is_payed,if (pre_bet_string like '%handerased', 1, 0) is_erased
    ";
    if(TAX_CALCULATION_ACTIVE=='ON'){
        $sql .=" ,pre_bet_tax ";
    }

    $sql.=" FROM punter_result, result, punter JOIN jurisdiction club ON club.jur_id  = pun_betting_club ";
    $jur_id = $_SESSION["jurisdiction_id"];

    if($_SESSION["jurisdiction_class"] != "casino"){
        $sql .= "
  		       JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
             JOIN jurisdiction regional ON regional.jur_id = district.jur_parent_id
             JOIN jurisdiction nation   ON nation.jur_id   = regional.jur_parent_id";
    }
    switch($_SESSION["jurisdiction_class"]){
        case "club":
            $sql .= " WHERE club.jur_id = $jur_id";
            break;
        case "district":
            $sql .= " WHERE district.jur_id = $jur_id";
            break;
        case "region":
            $sql .= " WHERE regional.jur_id = $jur_id";
            break;
        case "nation":
            $sql .= " WHERE nation.jur_id = $jur_id";
            break;
        case "casino":
            $sql .= " WHERE 1=1";
            break;
        default:
            $sql .= " WHERE 1=0";
            break;
    }
    $sql.=" AND res_id=pre_res_id
                AND pre_time BETWEEN  '$date 00:00:00' AND '$date 23:59:59'
                AND res_gam_id=$game
                AND pre_pun_id = pun_id
                AND pun_betting_club = $jurisdiction ";

    if($tid!=''){ $sql.=" AND pre_game_num = $tid";}
    }
    $starttime = microtime(true);

    $rs=$dbh->exec($sql);
    $endtime = microtime(true);
    $duration = $endtime - $starttime;
    $duration=number_format($duration, 4, ',', '')."s";
    $time="<div class='tip fleft'>".$lang->getLang("Time taken to execute your request").":".$duration."</div><br /><br /><br />";
    if($rs->getNumRows()>0){
        $return="<table class='display displayTicket table table-striped table-bordered table-condensed'>
                                <thead>
                                <tr>
                                    <th>".$lang->getLang("Hand ID")."</th>
                                    <th>".$lang->getLang("Game Nr")."</th>
                                    <th>".$lang->getLang("Date")."</th>
                                    <th>".$lang->getLang("Club")."</th>
                                    <th>".$lang->getLang("Cashier")."</th>
                                    <th>".$lang->getLang("Bet")."</th>
                                    <th>".$lang->getLang("Win")."</th>
                                    ";
        $return.="<th>".$lang->getLang("Status")."</th></tr> </thead>";
        while($rs->hasNext()){
            $rec=$rs->next();
            $cashier=getGameAndCashier($rec['pun_username']);
            $return .="<tr><td>".getClassLink("javascript:openWindow('/casino_games/hand_details.php?gmn={$rec["pre_game_num"]}&uid={$rec["pre_pun_id"]}&hand_id={$rec["pre_res_id"]}".$betting."','hand{$rec["pre_res_id"]}','950','980','0','1')", $rec["pre_res_id"], "contentlink")."</td>";
            $return .="<td>".getClassLink("javascript:openWindow('/casino_games/hand_details.php?gmn={$rec["pre_game_num"]}&uid={$rec["pre_pun_id"]}&hand_id={$rec["pre_res_id"]}".$betting."','hand{$rec["pre_res_id"]}','950','980','0','1')", $rec["pre_game_num"], "contentlink")."</td>";
            $return .="<td>".($rec["pre_time"])."</td>";
            $return .="<td>".($rec["club_name"])."</td>";
            $return .="<td>".getClassLink("javascript:openWindow('".refPage("customers/customer_view&customer_id={$rec["pre_pun_id"]}")."','user{$rec["pun_username"]}','950','980','0','1')", $cashier['cashier'], "contentlink")."</td>";
            $return .="<td>".getInDollars($rec["pre_bet"])."</td>";
            $return .="<td>".getInDollars($rec["pre_win"]-$rec['tot_win_tax'])."<br />
                    <span class='tip'>Tax : ".getInDollars($rec['tot_win_tax'],3)."</span></td>";
            $return.="<td>";
            if($rec["is_erased"]=='1' || ($game==215 && $rec['tstato']==4)){
                $return.="<span class='error uppercased'>".$lang->getLang('Erased')."</span>";
            }
            elseif($rec["is_payed"]=='1') {
                $return.="<span class='result uppercased'>". $lang->getLang('Paid')."</span>";
            }
            else{
                $return.="<span class='text-error uppercased'>".$lang->getLang('Not paid')."</span>";
            }
            $return.="</td></tr>";
        }
        $return.="</table>";
    }
    else{
        $return="<div class='alert alert-error'><button type='button' class='close' data-dismiss='alert'>&times;</button><strong>".$lang->getLang("No result found")."</strong></div>";
    }
    $return = $time.$return;

    return $return;
}

/**
 * @param $date
 * @param $jurisdiction
 * @return string
 */
function getFinancialRecords($date,$jurisdiction,$group){
    global $dbh,$lang;
    $date=$dbh->escape($date);
    $jurisdiction=$dbh->escape($jurisdiction);
    if(date('Y-m-d')>$date){
        $sql="SELECT pun_username,
                pun_id,
                gam_id,
                club.jur_name,
                coalesce(vds_tickets, 0) tickets_played,
                sum(coalesce(vds_bet, 0)) total_bet,
                sum(coalesce(vds_win, 0)) total_win,
                sum(coalesce(vds_start_credit, 0)) total_balance,
                sum(coalesce(vds_win_tax, 0)) total_win_tax,
                vds_cashier_ip,
                vds_date,
                gam_name,
                sum(coalesce(vds_ticket_won, 0)) tickets_won,
                sum(coalesce(vds_ticket_erased, 0)) tickets_erased,
                sum(coalesce(vds_ticket_paid, 0)) tickets_payed,
                sum(coalesce(vds_ticket_paid_amount, 0)) tickets_payed_win,
                sum(coalesce(vds_ticket_paid_amount_tax, 0)) tickets_payed_win_tax,
                sum(coalesce(vds_ticket_special, 0)) tickets_specials,
               (select pcr_credits from punter_credit where pcr_pun_id = pun_id) current_credit ";
        if(TAX_CALCULATION_ACTIVE=='ON'){
            $sql.=" ,sum(coalesce(vds_bet_tax, 0)) pre_bet_tax ";
        }
        $sql.="  FROM virtual_games_user
                 JOIN punter ON vgu_pun_id = pun_id AND pun_betting_club =$jurisdiction
                JOIN jurisdiction on pun_betting_club=jur_id
                LEFT JOIN virtual_daily_summary on vgu_pun_id=vds_pun_id AND vds_date ='$date'
                LEFT JOIN jurisdiction club on pun_betting_club=club.jur_id
                LEFT JOIN game ON vgu_gam_id = gam_id
                ";
        if($group=='1'){
            $sql .=" group by  pun_id";
        }
        else{
            $sql .=" group by  gam_id";
        }
    }else{
    $sql="
            SELECT pun_username,gam_id, pre_res_id, pre_game_num, sum(tickets_played) tickets_played, pun_id,
            sum(total_bet) total_bet, sum(total_win) total_win, total_balance,
            sum(total_win_tax) total_win_tax, PRE_PUN_IP, gam_name, jur_name,
            sum(tickets_won) tickets_won,
            sum(tickets_erased) tickets_erased,
            sum(tickets_payed) tickets_payed,
            sum(tickets_payed_win) tickets_payed_win,
            sum(tickets_payed_win_tax) tickets_payed_win_tax,
            current_credit, coalesce(sum(pre_bet_tax)) pre_bet_tax
            FROM
            (
          SELECT pun_username,
                vgu_gam_id gam_id,
                pre_res_id,
                pre_game_num,
                count(pre_game_num) tickets_played,
                pun_id,
                sum(coalesce(PRE_BET, 0)) total_bet,
                sum(coalesce(PRE_WIN, 0)) total_win,
                coalesce(PRE_BALANCE, 0) total_balance,
                sum(if (pre_bet_tax>0 , (pre_win) *0.01, 0)) total_win_tax,
                PRE_PUN_IP,
                gam_name,
                jur_name,
                coalesce(sum(case when pre_win > 0 then 1 else 0 end)) tickets_won,
                coalesce(sum(case when pre_bet_string like '%handerased' then 1 else 0 end)) tickets_erased,
                0 tickets_payed_win,
                0 tickets_payed,
                0 tickets_payed_win_tax,
                (select pcr_credits from punter_credit where pcr_pun_id = pun_id) current_credit,
                  coalesce(sum(pre_bet_tax), 0) pre_bet_tax ";
    $sql.="  FROM virtual_games_user
        JOIN punter ON vgu_pun_id = pun_id AND pun_betting_club =$jurisdiction
        JOIN jurisdiction on pun_betting_club=jur_id
        LEFT JOIN punter_result ON pre_pun_id = pun_id AND pre_time >= '$date 00:00:00' AND pre_time <= '$date 23:59:59'
        LEFT JOIN result ON pre_res_id = res_id
        LEFT JOIN game ON res_gam_id = gam_id
         group by pun_id
        UNION ALL
        SELECT pun_username,
        215 gam_id,
        IDTicket pre_res_id,
        CodiceTicket pre_game_num,
        count(CodiceTicket) tickets_played,
        pun_id,
        sum(IF(TStato <> 4, coalesce(Importo, 0) + coalesce(Tax, 0), 0)) total_bet,
        sum(coalesce(TVincita, 0)) total_win,
        0 total_balance,
        sum(if (Tax > 0 , (TVincita) *0.01, 0)) total_win_tax,
        IPAddress PRE_PUN_IP,
        'Betting_Agency' gam_name,
        jur_name, coalesce(sum(case when TVincita > 0 then 1 else 0 end)) tickets_won,
        sum(case when TStato = 4 then 1 else 0 end) tickets_erased,
        0 tickets_payed_win,
        0 tickets_payed,
        0 tickets_payed_win_tax,
        (select pcr_credits from punter_credit where pcr_pun_id = pun_id) current_credit,
        sum(COALESCE(Tax, 0)) pre_bet_tax
        FROM virtual_games_user JOIN punter ON vgu_pun_id = pun_id
        JOIN jurisdiction on pun_betting_club=jur_id AND pun_betting_club =$jurisdiction
        LEFT JOIN Ticket_dev ON ExtIDUtente = pun_id AND TStamp BETWEEN '$date 00:00:00' AND '$date 23:59:59'
        group by pun_id
        UNION ALL
        SELECT pun_username,
        vtp_gam_id gam_id,
        vtp_res_id pre_res_id,
        0 pre_gam_num,
        0 tickets_played,
        vtp_pun_id pun_id,
        0 total_bet,
        0 total_win,
        0 total_balance,
        0 total_win_tax,
        null pre_pun_ip,
        gam_name,
        jur_name,
        0 tickets_won,
        0 tickets_erased,
        sum(coalesce(vtp_amount_payed, 0)) tickets_payed_win,
        count(vtp_ticket_id) tickets_payed,
        sum(case when 1=1 then (vtp_amount_payed) * 0.01 else 0 end) tickets_payed_win_tax,
        0 current_credit,
        0 pre_bet_tax
        FROM virtual_ticket_payed
        JOIN punter ON vtp_pun_id = pun_id AND pun_betting_club = $jurisdiction
        JOIN jurisdiction on pun_betting_club=jur_id
        JOIN game ON vtp_gam_id = gam_id
        WHERE vtp_time BETWEEN '$date 00:00:00' AND '$date 23:59:59'
        group by pun_id ) t ";

        if($group=='1'){
            $sql .=" group by  pun_id";
        }
        else{
            $sql .=" group by  gam_id";
        }
    }
    $starttime = microtime(true);
    $rs=$dbh->exec($sql);
    $endtime = microtime(true);
    $duration = $endtime - $starttime;
    $duration=number_format($duration, 4, ',', '')."s";
    $time="<div class='tip fleft'>".$lang->getLang("Time taken to execute your request").":".$duration."</div><br /><br /><br />";

    if($rs->getNumRows()>0){
        $totalbet=0;
        $totalwin=0;
        $totalbetTax=0;
        $ticketsplayed=0;
        $ticketswin=0;
        $ticketspaid=0;
        $ticketserased=0;
        $ticketspaidamount=0;
        $totalWinPaidTax=0;


        $allresult="<table class='display table table-striped table-bordered table-condensed'>
                                <thead>
                                <tr>
                                   <th>".$lang->getLang("Total Bet")."</th>";
        $allresult.="<th>".$lang->getLang("Tickets played")."</th>
                     <th>".$lang->getLang("Tickets paid")."</th>
                     <th>".$lang->getLang("Tickets paid amount")."</th>
                     <th>".$lang->getLang("Tickets erased")."</th>
                     <th>".$lang->getLang("Difference")."</th>";


        $allresult.="</tr></thead>";


        $return="<table class='display displayFinancial table table-striped table-bordered table-condensed'>
                                <thead>
                                <tr>
                                    <th>".$lang->getLang("Club")."</th>
                                    <th>".$lang->getLang("Cashier")."</th>
                                    <th>".$lang->getLang("Game")."</th>
                                    <th class='unwrappable'>".$lang->getLang("Total Bet")."</th>";
        $return.="<th class='unwrappable'>".$lang->getLang("Tickets played")."</th>
                                     <th class='unwrappable'>".$lang->getLang("Tickets paid")."</th>
                                     <th class='unwrappable'>".$lang->getLang("Tickets paid amount")."</th>
                                     <th class='unwrappable'>".$lang->getLang("Tickets erased")."</th>
                                     <th class='unwrappable'>".$lang->getLang("Difference")."</th>
                                </tr>
                                </thead>";
        while($rs->hasNext()){
            $rec=$rs->next();
            $totalbet+=$rec['total_bet'];
            $totalwin+=$rec['total_win'];
            $ticketsplayed+=$rec['tickets_played'];
            $ticketswin+=$rec['tickets_won'];
            $ticketspaid+=$rec['tickets_payed'];
            $ticketserased+=$rec['tickets_erased'];
            $ticketspaidamount+=$rec['tickets_payed_win'];
            $totalWinPaidTax+=$rec["tickets_payed_win_tax"];

            $cashier=getGameAndCashier($rec['pun_username']);
            $return .="<tr><td>".($rec["jur_name"])."</td>";
            $return.="<td>".getClassLink("javascript:openWindow('".refPage("customers/customer_view&customer_id={$rec["pun_id"]}")."','user{$rec["pun_username"]}','950','980','0','1')", $cashier['cashier'], "contentlink")."</td>";
            $return .="<td>".($cashier['game']!=''? getClassLink("javascript:openWindow('".refPage("customers/customer_view&customer_id={$rec["pun_id"]}")."','user{$rec["pun_username"]}','950','980','0','1')", $cashier['game'], "contentlink") :'--')."</td>";
            $return .="<td>".getInDollars($rec["total_bet"])."</td>";
            $return .="<td>".$rec["tickets_played"]."</td>";
            $return .="<td>".$rec["tickets_payed"]."</td>";
            $return .="<td>".getInDollars($rec["tickets_payed_win"]-$rec["tickets_payed_win_tax"])."<br /><span class='tip'>Tax : ".getInDollars($rec["tickets_payed_win_tax"],3)."</span></td>";
            $return .="<td>".$rec["tickets_erased"]."</td>";
            $return .="<td>".getInDollars($rec["total_bet"]-($rec["tickets_payed_win"]-$rec["tickets_payed_win_tax"]))."</td></tr>";
        }
        $return.="</table>";

        $allresult .="<tr>
                    <td>".getInDollars($totalbet)."</td>
                    <td>".$ticketsplayed."</td>
                    <td>".$ticketspaid."</td>
                    <td>".getInDollars($ticketspaidamount-$totalWinPaidTax)."<br /><span class='tip'>Tax : ".getInDollars($totalWinPaidTax,3)."</span></td>
                    <td>".$ticketserased."</td>
                     <td>".getInDollars($totalbet-($ticketspaidamount-$totalWinPaidTax))."</td>
                    </tr></table><br />";
    }
    else{
        $return="<div class='alert alert-error'><button type='button' class='close' data-dismiss='alert'>&times;</button><strong>".$lang->getLang("No result found")."</strong></div>";
    }
    $return = $time.$allresult.$return;
    return $return;
}






?>