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
            $class=getSubJurisdiction($_SESSION['jurisdiction_class']);
            $jurisdiction=$_SESSION['jurisdiction_id'];

            $nation=array();
            $region=array();
            $district=array();
            $club=array();
            $nodes=getVirtualJurisdiction($jurisdiction,$class);
            if($nodes->getNumRows()>0){
                $jurisdictions=array();
                while ( $nodes->hasNext () ) {
                    $row = $nodes->next ();
                    if(!is_array($jurisdictions[$row['nation_id']])){
                        $jurisdictions[$row['nation_id']]['id']=$row['nation_id'];
                        $jurisdictions[$row['nation_id']]['class']='nation';
                        $jurisdictions[$row['nation_id']]['name']=$row['nation'];
                        $jurisdictions[$row['nation_id']]['child']=array();
                    }
                    if(!is_array($jurisdictions[$row['nation_id']]['child'][$row['region_id']])){
                        $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['id']=$row['region_id'];
                        $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['class']='region';
                        $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['name']=$row['region'];
                        $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child']=array();
                    }

                    if(!is_array($jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']])){
                        $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['id']=$row['district_id'];
                        $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['class']='district';
                        $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['name']=$row['district'];
                        $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['child']=array();
                    }
                    $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['child'][$row['club_id']]['id']=$row['club_id'];
                    $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['child'][$row['club_id']]['name']=$row['club'];
                    $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['child'][$row['club_id']]['class']='club';
                }
                $return="<ul>".showChild($jurisdictions)."</ul>";

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
                    $jurisdiction=json_decode($_POST['jurisdictions']);
                    $_allSelected=$_POST['allSelected'];
                    die(getTicketsAgency($date,$game,$tid,$jurisdiction,$_allSelected));
                }
                elseif($_POST['section']=="ticketPaid"){
                    $_games=json_decode($_POST['games']);
                    $date=$_POST['date_start'];
                    $jurisdiction=(isset($_POST['jurisdictions'])?json_decode($_POST['jurisdictions']):$_SESSION['jurisdiction_id']);
                    $_allSelected=$_POST['allSelected'];
                    $_winmajor=$_POST['winmajor'];
                    $_ticketInfo=$_POST['ticketInfo'];
                    $_tid=$_POST['ticket'];
                    $pdf=$_POST['pdf'];
                    die(getTicketsPaid($date,$_games,$jurisdiction,$_tid,$_allSelected,$_winmajor,$pdf,$_ticketInfo));
                }

                elseif($_POST['section']=="ticketNotPaid"){
                    $_games=json_decode($_POST['games']);
                    $date=$_POST['date_start'];
                    $jurisdiction=(isset($_POST['jurisdictions'])?json_decode($_POST['jurisdictions']):$_SESSION['jurisdiction_id']);
                    $_allSelected=$_POST['allSelected'];
                    $_tid=$_POST['ticket'];
                    $pdf=$_POST['pdf'];
                    die(getTicketsNotPaid($date,$_games,$jurisdiction,$_tid,$_allSelected));
                }

                elseif($_POST['section']=="financial"){
                    $jurisdiction=json_decode($_POST['jurisdictions']);
                    $date=$_POST['date_start'];
                    $date_end=$_POST['date_end'];
                    $_group=$_POST['group'];
                    $pdf=$_POST['pdf'];
                    if($_group=='4'){
                        die(getFinancialTotals($date,$date_end,$jurisdiction,$_group,$_POST['allSelected']));
                    }
                    die(getFinancialRecords($date,$date_end,$jurisdiction,$_group,$_POST['allSelected'],$pdf,$_POST['type']));
                }
                elseif($_POST['section']=="overall"){
                    $_jurisdiction=json_decode($_POST['jurisdictions']);
                    $_date=$_POST['date_start'];
                    $_date_end=$_POST['date_end'];
                    $_users=$_POST['users'];
                    $_group=$_POST['group'];
                    die(getOverall($_date,$_date_end,$_jurisdiction,$_users,$_group,$_POST['allSelected']));
                }
                elseif($_POST['section']=="stats"){
                    die(getAgenciesStats(json_decode($_POST['jurisdictions']),$_POST['allSelected'],$_POST['period'],$_POST['datefrom'],$_POST['inactivity']));
                }
                elseif($_POST['section']=="cashier"){
                    $start=$_POST['date_start'];
                    $end=$_POST['date_end'];
                    $group_by=$_POST['group'];
                    $jurisdiction = json_decode($_POST['jurisdictions']);
                    die(getCashierClosuer($start,$end,$jurisdiction,$_POST['allSelected'],$group_by));
                }
                else {
                    $jurisdiction=json_decode($_POST['jurisdictions']);
                    die(getJurisdictionVirtualDetails($jurisdiction,$_POST['allSelected']));
                }

            }
            else{
                if($_POST['section']=="ticketPaid" || $_POST['section']=="financial" || $_POST['section']=="ticket" || $_POST['section']=="overall" || $_POST['section']=="ticketNotPaid"){
                    die(json_encode("<div class='alert alert-error'>Please login again!</div>"));
                }
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
            $_jurisdiction=json_decode($_POST['jurisdictions']);
            die(getVirtualUsers($_jurisdiction,true,$_POST['allSelected']));
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
        case 'searchClosure':{
            $date=$_POST['date'];
            $jurisdiction=json_decode($_POST['jurisdictions']);
            die(getAgencyClosure($jurisdiction,$date));
            break;
        }

        case 'saveVirtualCommissions':{
             die(saveVirtualComissionsClosure($_POST['club'],$_POST['time'],$_POST['raport'],$_POST['plati'],$_POST['tcatre'],$_POST['tdin'],$_POST['tdsediu'],$_POST['tdiffprofit'],$_POST['tdiffpierdere'],$_POST['anulate'],$_POST['raportfiscal']));
            break;
        }
        default:
            die('Invalid operation');
            break;
    }
}


/**
 * @param $jurisdiction
 * @param $class
 * @return bool|int|RecordSet
 */
function getVirtualJurisdiction($jurisdiction,$class){
    global $dbh;
    $sql="SELECT n.jur_name nation, n.jur_id nation_id,n.jur_code nation_code,
        r.jur_name region, r.jur_id region_id, r.jur_code region_code,
        d.jur_name district, d.jur_id district_id,d.jur_code district_code,
        c.jur_name club, c.jur_id club_id, vgs_jur_code,c.jur_code club_code
        FROM  jurisdiction n
        JOIN jurisdiction r ON r.jur_parent_id = n.jur_id
        JOIN jurisdiction d ON d.jur_parent_id = r.jur_id
        JOIN  jurisdiction c ON c.jur_parent_id = d.jur_id
        JOIN virtual_games ON vgs_jur_code = c.jur_code AND c.jur_class = 'club'
       ";

  if($class=='region')
    {
        $sql.="WHERE n.jur_id=$jurisdiction";
    }
    elseif($class=='district'){
        $sql.="WHERE r.jur_id=$jurisdiction";
    }
    elseif($class=='club'){

        if($_SESSION['jurisdiction_class']=='club'){
            $sql.="WHERE c.jur_id=$jurisdiction";
        }
        else{
            $sql.="WHERE d.jur_id=$jurisdiction";
        }
    }
    $sql.=" GROUP BY c.jur_id ORDER BY n.jur_id,r.jur_id,d.jur_id,c.jur_id";

    $result=$dbh->doCachedQuery($sql,3600*5);
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
 * @param $jurisdiction
 * @return string
 */
function getJurisdictionVirtualDetails($jurisdiction,$allSelected=false){
    global $dbh,$lang;
    $today= date("Y-m-d", time());
    if(is_array($jurisdiction)){
        $jurisdiction=implode(',',$jurisdiction);
    }
    $users=getConectedUsers(true);

/*    $users=explode('~',$users);

    $connected=array();
    foreach($users as $k=>$v){
        list($pl,$val) = explode(":", $v );
        list($prefix,$userid)=explode('_',$pl);
        array_push($connected,$userid);
    }*/
    $connected=array();
    foreach($users->Records as $k=>$v){
        array_push($connected,$v['usl_pun_id']);
    }


    $sql="SELECT n.jur_name nation, n.jur_id nation_id, r.jur_name region, r.jur_id region_id, d.jur_name district, d.jur_id district_id,
            c.jur_name club, c.jur_id club_id, VGU_JUR_CODE, VGU_GAM_ID, pun_id, pun_username,
            coalesce(count(pre_game_num), 0) ticket_played, sum(pre_bet) ticket_bet, sum(pre_win) ticket_win,
            pre_balance, pcr_credits, pun_reg_date, pun_last_logged_in,cur_code_for_web
            FROM virtual_games_user
            LEFT JOIN punter ON vgu_pun_id = pun_id
            LEFT JOIN punter_credit ON pun_id = pcr_pun_id
            LEFT JOIN jurisdiction c ON pun_betting_club = c.jur_id
            LEFT JOIN jurisdiction d ON c.jur_parent_id = d.jur_id
            LEFT JOIN jurisdiction r ON d.jur_parent_id = r.jur_id
            LEFT JOIN jurisdiction n ON r.jur_parent_id = n.jur_id
            LEFT JOIN punter_result ON    pre_time >= '$today 00:00:00' AND  pre_time <= '$today 23:59:59' AND pun_id = pre_pun_id
            LEFT JOIN currency on cur_id=c.jur_currency ";

    switch($_SESSION["jurisdiction_class"]){
        case "club":
            $sql .= " WHERE c.jur_id = ".$_SESSION['jurisdiction_id'];
            break;
        case "district":
            $sql .= " WHERE d.jur_id = ".$_SESSION['jurisdiction_id'];
            break;
        case "region":
            $sql .= " WHERE r.jur_id = ".$_SESSION['jurisdiction_id'];
            break;
        case "nation":
            $sql .= " WHERE n.jur_id = ".$_SESSION['jurisdiction_id'];
            break;
        case "casino":
            $sql .= " WHERE 1=1";
            break;
        default:
            $sql .= " WHERE 1=0";
            break;
    }

    if(!$allSelected || $allSelected=='false'){
        $sql.=" AND pun_betting_club in ($jurisdiction) ";
    }

    $sql.=" GROUP BY pun_betting_club,pun_id";
    $starttime = microtime(true);

    $result=$dbh->doCachedQuery($sql,10);
    $endtime = microtime(true);
    $duration = $endtime - $starttime;
    $duration=number_format($duration, 4, ',', '')."s";
    $time="<div class='tip fleft'>".$lang->getLang("Time taken to execute your request").":".$duration."</div><br /><br /><br />";
    $thisclubplayers=0;
    $thisclubplayersconnected=0;
    $return ='<div class="accordion" id="accordion2"><div><div><div> <div><table>';
    $club='';
    while($result->hasNext()){
        $row  = $result->next();
        if($row['club']!=$club){
            if($thisclubplayers>0){
                if($thisclubplayers ==$thisclubplayersconnected){
                    $color='green';
                }
                elseif($thisclubplayersconnected==0){
                    $color='red';
                }
                else{
                    $color='orange';
                }
            }
            if($club!=''){
                $append="<svg height='15' width='15'><circle cx='9' cy='9' r='6'  fill='$color' /></svg>";
                $return.="<script>$('#".$club."_status').empty().append(\" Status ".$append."  ".number_format($thisclubplayersconnected*100/$thisclubplayers,2)."% (".$thisclubplayersconnected."/".$thisclubplayers." )  \" )</script>";
            }
            $thisclubplayers=0;
            $thisclubplayersconnected=0;
            $return.="</table>";
            $return.="</div>
                    </div>
                    </div>
                    </div>";

            $return.='  <div class="accordion-group">
                            <div class="accordion-heading">
                              <a  class="accordion-toggle " data-toggle="collapse"  href="#club'.$row["club_id"].'"><span class="label label-info">'.$row['club'].'</span>
                              <span class="label label-default fright" id="'.$row['club'].'_status"></span></a>
                            </div>
                            <div id="club'.$row['club_id'].'" class="accordion-body collapse in">
                              <div class="accordion-inner">';
            $return.="<div class='well'>";
            $return.=getJurisdictionHierarchy($row['nation'],$row['region'],$row['district'],$row['club'])."</ br>";
            $return.="<table class='display displayInfo table table-striped table-bordered table-condensed' >
                        <thead>
                        <tr>
                            <th>".$lang->getLang("Club")."</th>
                            <th>".$lang->getLang("Cashier")."</th>
                            <th>".$lang->getLang("Game")."</th>
                            <th>".$lang->getLang("Agency Code")."</th>";
            if(check_access('virtual_cashier_show_credit')){
                $return.="<th>".$lang->getLang("Available Credit")."</th>";
            }
            $return.=" <th>".$lang->getLang("Status")."</th>
                        <th>".$lang->getLang("Last Login")."</th>
                        </tr>
                        </thead>";
            $club=$row['club'];
        }
        $thisclubplayers++;
        if(in_array($row['pun_id'],$connected)){
            $thisclubplayersconnected++;
        }
        $cashier=getGameAndCashier($row['pun_username']);
        $return.=" <tr>
                            <td>".$row['club']."</td>
                            <td>".getClassLink("javascript:openWindow('".refPage("customers/customer_view&customer_id={$row["pun_id"]}")."','user{$row["pun_username"]}','950','980','0','1')", $cashier['cashier'], "contentlink")."</td>
                            <td>".getClassLink("javascript:openWindow('".refPage("customers/customer_view&customer_id={$row["pun_id"]}")."','user{$row["pun_username"]}','950','980','0','1')", $cashier['game'], "contentlink")."</td>
                            <td>".$row['vgu_jur_code']."</td>";
        if(check_access('virtual_cashier_show_credit')){
            $return.="<td>".getInDollars($row['pcr_credits'],2,$row['cur_code_for_web'])."</td>";
        }
        $return.="<td>";
        if(in_array($row['pun_id'],$connected)){
            $return.="<span class='text-success'>".$lang->getLang("Connected")."</span>";
        }
        else{
            $return.="<span class='text-error'>".$lang->getLang("Not Connected")."</span>";
        }
        $return.="</td>
                    <td>".$row['pun_last_logged_in']."</td>
                  </tr> ";
    }
    $return.="</table>";
    if($thisclubplayers>0){
        if($thisclubplayers ==$thisclubplayersconnected){
            $color='green';
        }
        elseif($thisclubplayersconnected==0){
            $color='red';
        }
        else{
            $color='orange';
        }
    }
    if($club!=''){
        $append="<svg height='15' width='15'><circle cx='9' cy='9' r='6'  fill='$color' /></svg>";
        $return.="<script>$('#".$club."_status').empty().append(\" Status ".$append."  ".number_format($thisclubplayersconnected*100/$thisclubplayers,2)."% (".$thisclubplayersconnected."/".$thisclubplayers." )  \" )</script>";
    }
    $return.="</div></div>
                            </div>
                          </div>";

    $return = $time.$return;
    return $return;
}

/**
 * @param $url
 * @return string
 */
function getConectedUsers($local=false){
   if($local){
    global $dbh;
    $sql="Select * from system_cache.user_logged ";
    return $dbh->doCachedQuery($sql,0);
   }
   else {
       $ch = curl_init();
       $timeout = 5;
       curl_setopt($ch, CURLOPT_URL, ONLINEPLRS);
       curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
       curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, $timeout);
       $data = curl_exec($ch);
       curl_close($ch);
       $start = stripos($data, "<body") + 6;
       $end = stripos($data, "</body");
       $body = substr($data, $start, $end - $start);
       return $body;
   }
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

function getCashierClosuer($start,$end,$jurisdiction,$allSelected,$group_by){

    global $dbh, $lang;
    $conditions = '';
//print_r($group_by);die;
    if(is_array($jurisdiction)){
        $jurisdiction=implode(',',$jurisdiction);
    }
    if(!$allSelected || $allSelected=='false'){
        $conditions.=" AND jur_id in ($jurisdiction) ";
    }
    $group = '';
    if ($group_by == 0) {
        $group = " GROUP BY jur_id";
    }
    $sql = "SELECT cl.*, jur_name, pun_username, gam_name
            FROM virtual_cashier_closure cl, punter, jurisdiction, game
            WHERE vcl_jur_code = jur_code
            $conditions
            AND pun_betting_club = jur_id
            AND pun_id = vcl_pun_id
            AND gam_id = vcl_gam_id
            AND vcl_starting BETWEEN '$start 00:00:00' AND '$end 23:59:59'
            $group";


    $rs=$dbh->exec($sql);

    if($rs->getNumRows()>0) {
        $return = "<table class='display displayCashier table table-striped table-bordered table-condensed' width='100%' >
                                <thead>
                                <tr>
                                    <th>" . $lang->getLang("Jurisdiction Name") . "</th>
                                    <th>" . $lang->getLang("Start Date / End Date / Time") . "</th>
                                    <th>" . $lang->getLang("Game Name") . "</th>
                                    <th>" . $lang->getLang("Operator") . "</th>
                                    <th>" . $lang->getLang("Game Username") . "</th>
                                    <th>" . $lang->getLang("Start credit") . "</th>
                                    <th>" . $lang->getLang("End credit") . "</th>
                                    <th>" . $lang->getLang("Total bets") . "</th>
                                    <th>" . $lang->getLang("Total wins") . "</th>
                                    <th>" . $lang->getLang("Tickets") . "</th>
                                </tr>
                                </thead>";
        while ($rs->hasNext()) {
            $rec = $rs->next();
            $return .= "<tr>";
            $return .= "<td>".$rec['jur_name']."</td>";
            $return .= "<td>".$rec['vcl_starting']." / ".$rec['vcl_ending']." / ".$rec['vcl_time']."</td>";
            $return .= "<td>".$rec['gam_name']."</td>";
            $return .= $group_by==0?'<td>-</td>':"<td>".$rec['vcl_username']."</td>";
            $return .= $group_by==0?'<td>-</td>':"<td>".$rec['pun_username']."</td>";
            $return .= $group_by==0?'<td>-</td>':"<td>".$rec['vcl_starting_credit']."</td>";
            $return .= $group_by==0?'<td>-</td>':"<td>".$rec['vcl_ending_credit']."</td>";
            $return .= "<td>".$rec['vcl_total_bets']."</td>";
            $return .= "<td>".$rec['vcl_total_wins']."</td>";
            $return .= "<td>".$rec['vcl_tickets_played']."</td>";
            $return .= "</tr>";
        }
        $return.="</table>";
    } else {
        $return="<div class='alert alert-error'><button type='button' class='close' data-dismiss='alert'>&times;</button><strong>".$lang->getLang('No result found')."</strong></div>";
    }
    return json_encode($return);
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
function getTicketsAgency($date,$game,$tid,$jurisdiction,$allSelected=false){
    global $dbh,$lang;
    if(is_array($jurisdiction)){
        $jurisdiction=implode(',',$jurisdiction);
    }
    $tid=  preg_replace('#\D.*$#', '', $tid);

    if($game==215){
        $sql=" SELECT  pun_username,
                IDTicket PRE_RES_ID,
                CodiceTicket pre_game_num,
                c.jur_name club_name,
               IDUtente PRE_PUN_ID,
               IF(TStato <> 4, coalesce(Importo, 0) + coalesce(Tax, 0), 0) PRE_BET,
               if (Tax>0  AND TVincita > 0 , (Importo) * 0.01, 0) tot_win_tax,
               TVincita PRE_WIN,
               IPAddress PRE_PUN_IP,
               TStamp pre_time,
               '-' PRE_BALANCE,
               Pagato is_payed,
               Tstato,
               TAccreditato,
               Tax pre_bet_tax,
               c.jur_id jur_id,
               cur_code_for_web,
               'true' betting,
               0 pre_win_tax
               FROM Ticket_dev, virtual_games_user, punter, jurisdiction c, jurisdiction d, jurisdiction r, jurisdiction n,currency
               WHERE ExtIDUtente = vgu_pun_id
               AND pun_id = vgu_pun_id
               AND cur_id=c.jur_currency
                AND TStamp BETWEEN '$date 00:00:00' AND '$date 23:59:59'
                AND c.jur_id = PUN_BETTING_CLUB
                AND c.jur_parent_id = d.jur_id
                AND d.jur_parent_id = r. jur_id
                AND r.jur_parent_id = n.jur_id
                AND n.jur_parent_id = 1  ";
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
            //    $sql .= " AND n.jur_id = $jur_id";
                break;
        }
        if(!$allSelected || $allSelected=='false'){
            $sql.=" AND pun_betting_club in ($jurisdiction) ";
        }
        if($tid!=''){ $sql.=" AND (CodiceTicket = $tid OR IDTicket=$tid) ";}
    }
    elseif($game==216){
        $sql=" SELECT  pun_username,
                LTK_ID PRE_RES_ID,
                ltk_code pre_game_num,
                c.jur_name club_name,
               LTK_PUN_ID PRE_PUN_ID,
               IF(LTK_STATE <> 4, coalesce(LTK_AMOUNT, 0) + coalesce(LTK_TAX, 0), 0) PRE_BET,
               if (LTK_TAX>0 AND LTK_AMOUNT > 600, (LTK_AMOUNT - 600) * 0.25, 0) tot_win_tax, LTK_WIN PRE_WIN,
               LTK_IPADD PRE_PUN_IP, LTK_TSTAMP pre_time, '-' PRE_BALANCE, LTK_PAYED is_payed, LTK_STATE Tstato, '-' TAccreditato,
               LTK_TAX pre_bet_tax,
               c.jur_id jur_id,
               cur_code_for_web,
               'true' betting,
               0 pre_win_tax
               FROM live_tickets, virtual_games_user, punter, jurisdiction c, jurisdiction d, jurisdiction r, jurisdiction n,currency
               WHERE LTK_PUN_ID = vgu_pun_id
               AND pun_id = vgu_pun_id
                AND LTK_TSTAMP BETWEEN  '$date 00:00:00' AND '$date 23:59:59'
                AND c.jur_id = PUN_BETTING_CLUB
                AND c.jur_parent_id = d.jur_id
                and c.jur_currency=cur_id
                AND d.jur_parent_id = r. jur_id
                AND r.jur_parent_id = n.jur_id
                AND n.jur_parent_id = 1 ";
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
        }
        if($tid!=''){ $sql.=" AND (CodiceTicket = $tid OR IDTicket=$tid) ";}
   }
    else{
    $sql=" SELECT
            pun_username,
            PRE_RES_ID,
            pre_game_num,
            club.jur_name  as club_name,
            PRE_PUN_ID,
            PRE_BET,
            (if (pre_bet_tax>0, (pre_win) * 0.01, 0)) tot_win_tax,
            PRE_WIN,
            PRE_BALANCE,
            PRE_TBL_ID,
            PRE_PUN_IP,
            pre_bet_string ,
            res_id,
            pre_time,
            pre_balance,
            IF (pre_bets_paid like '%payed', 1, 0) is_payed,
            if (pre_bet_string like '%handerased', 1, 0) is_erased,
            cur_code_for_web,
            'false' betting,
            pre_win_tax
    ";
    if(TAX_CALCULATION_ACTIVE=='ON'){
        $sql .=" ,pre_bet_tax ";
    }

    $sql.=" FROM punter_result, result, punter JOIN jurisdiction club ON club.jur_id  = pun_betting_club
    JOIN currency on cur_id=club.jur_currency   ";
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
                AND res_gam_id=$game
                AND pre_pun_id = pun_id
                AND pre_time BETWEEN  '$date 00:00:00' AND '$date 23:59:59' ";

    if(!$allSelected || $allSelected=='false'){
        $sql.=" AND pun_betting_club in ($jurisdiction) ";
        }
    if($tid!=''){ $sql.=" AND pre_game_num = $tid";}
    }

    $starttime = microtime(true);
    $rs=$dbh->exec($sql);
    $endtime = microtime(true);
    $duration = $endtime - $starttime;
    $duration=number_format($duration, 4, ',', '')."s";
    $time="<div class='tip fleft'>".$lang->getLang("Time taken to execute your request").":".$duration."</div><br /><br /><br />";
    if($rs->getNumRows()>0){
        $return="<table class='display displayTicket table table-striped table-bordered table-condensed' width='100%' >
                                <thead>
                                <tr>
                                    <th>".$lang->getLang("Hand ID")."</th>
                                    <th>".$lang->getLang("Game Nr")."</th>
                                    <th>".$lang->getLang("Date")."</th>
                                    <th>".$lang->getLang("Club")."</th>
                                    <th>".$lang->getLang("Cashier")."</th>
                                    <th>".$lang->getLang("Bet")."</th> ";
        if(TAX_CALCULATION_ACTIVE=='ON'){
            $return.=" <th>".$lang->getLang("Bet Tax")."</th>";
        }
        $return.="<th>".$lang->getLang("Win")."</th>
                                    <th>".$lang->getLang("Rake")."</th> ";
        if(check_access('virtual_cashier_show_credit')){
            $return.=" <th>".$lang->getLang("Balance")."</th>";
        }
        $return.="<th>".$lang->getLang("Status")."</th>
         <th>".$lang->getLang("Ip")."</th> </tr> </thead>";
        while($rs->hasNext()){
            $rec=$rs->next();
            if(TAX_CALCULATION_ACTIVE!='ON'){
                $rec['pre_bet_tax']=0;
                $rec['tot_win_tax']=0;
            }
            $rec['tot_win_tax']+=$rec['pre_win_tax'];

            $cashier=getGameAndCashier($rec['pun_username']);
            $return .="<tr><td>".getClassLink("javascript:openWindow('/casino_games/hand_details.php?gmn={$rec["pre_game_num"]}&uid={$rec["pre_pun_id"]}&hand_id={$rec["pre_res_id"]}&betting={$rec["betting"]}','hand{$rec["pre_res_id"]}','950','980','0','1')", $rec["pre_res_id"], "contentlink")."</td>";
            $return .="<td>".getClassLink("javascript:openWindow('/casino_games/hand_details.php?gmn={$rec["pre_game_num"]}&uid={$rec["pre_pun_id"]}&hand_id={$rec["pre_res_id"]}&betting={$rec["betting"]}','hand{$rec["pre_res_id"]}','950','980','0','1')", $rec["pre_game_num"], "contentlink")."</td>";
            $return .="<td>".($rec["pre_time"])."</td>";
            $return .="<td>".($rec["club_name"])."</td>";
            $return .="<td>".getClassLink("javascript:openWindow('".refPage("customers/customer_view&customer_id={$rec["pre_pun_id"]}")."','user{$rec["pun_username"]}','950','980','0','1')", $cashier['cashier'], "contentlink")."</td>";
            $return .="<td>".getInDollars($rec["pre_bet"],2,$rec['cur_code_for_web'])."</td>";
            if(TAX_CALCULATION_ACTIVE=='ON'){
                $return .="<td>".($rec['pre_bet_tax'] !='' ? getInDollars(($rec['pre_bet']- $rec['pre_bet_tax']),2,$rec['cur_code_for_web'])."<br />
                                <span class='tip'>Tax : ".getInDollars($rec['pre_bet_tax'],3,$rec['cur_code_for_web'])."</span>" : '--' )."</td>";
            }
            $return .="<td>".getInDollars($rec["pre_win"]-$rec['tot_win_tax'])."<br />
                    <span class='tip'>Tax : ".getInDollars($rec['tot_win_tax'],3,$rec['cur_code_for_web'])."</span></td>";
            $return .="<td>".getInDollars($rec["pre_bet"]-$rec["pre_win"],2,$rec['cur_code_for_web'])."</td>";
            if(check_access('virtual_cashier_show_credit')){
                $return .="<td>".getInDollars($rec["pre_balance"],2,$rec['cur_code_for_web'])."</td>";
            }
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
            $return.="</td>";
            $return .="<td>".getClassLink("javascript:openWindow('".refPage('customers/search&field=4&search_string=' . urlencode(ereg_replace("[^0-9.]", "", $rec["pre_pun_ip"]))) ."','ip','950','980','0','1')", $rec["pre_pun_ip"], "contentlink")."</td></tr>";
        }
        $return.="</table>";
    }
    else{
        $return="<div class='alert alert-error'><button type='button' class='close' data-dismiss='alert'>&times;</button><strong>".$lang->getLang('No result found')."</strong></div>";
    }





        $returnStr=array();
        $returnStr[0] = $time;
        $returnStr[1] = $return;
        return json_encode($returnStr);

}


/**
 * @param $date
 * @param $games
 * @param $jurisdiction
 * @param bool $tid
 * @param bool $allSelected
 * @param bool $winmajor
 * @return string
 * @desc Returns the paid tickets
 */
function getTicketsPaid($date,$games,$jurisdiction,$tid=false,$allSelected=false,$winmajor=true,$pdf=false,$ticketInfo=false){
    global $dbh,$lang;
    if(is_array($jurisdiction)){
        $jurisdiction=implode(',',$jurisdiction);
    }
    if(is_array($games)){
        $games=implode(',',$games);
    }
        $sql=" select vtp_time,vtp_cashout_tax, vtp_gam_id, gam_name, c.jur_name club_name, vtp_ticket_id, vtp_res_id, vtp_pun_id, vtp_amount_payed,cur_code_for_web,cur_id as currency,vtp.*
              from punter, jurisdiction c, jurisdiction d, jurisdiction r, jurisdiction n, game,currency,  virtual_ticket_payed left
              join virtual_ticket_payed_info vtp on vtp_id=vti_vtp_id
                where c.jur_id = pun_betting_club
                and cur_id=c.jur_currency
                and vtp_pun_id = pun_id
                and vtp_time BETWEEN '".$dbh->escape($date)." 00:00:00' AND '".$dbh->escape($date)." 23:59:59'
                and vtp_gam_id = gam_id
                and c.jur_parent_id = d.jur_id
                and d.jur_parent_id = r.jur_id
                and r.jur_parent_id = n.jur_id ";

    if($winmajor!="false"){
        $sql.=" AND vtp_amount_payed>0 ";
    }
    $sql.=" AND gam_id in ($games) ";
    $jur_id=$_SESSION['jurisdiction_id'];
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
            default:
                $sql .= " WHERE 1=0";
                break;
        }

        if(!$allSelected || $allSelected=='false'){
            $sql.=" AND pun_betting_club in ($jurisdiction) ";
        }
    if($tid!=''){ $sql.=" AND vtp_ticket_id = ".$dbh->prepareString($tid);}

    $starttime = microtime(true);
    $rs=$dbh->exec($sql);
    $endtime = microtime(true);
    $duration = $endtime - $starttime;
    $totalPaid=0;
    $totalTax=0;
    $duration=number_format($duration, 4, ',', '')."s";
    $time="<div class='tip fleft'>".$lang->getLang("Time taken to execute your request").":".$duration."</div><br /><br /><br />";
    if($rs->getNumRows()>0){
        $allresult="<table class='display table table-striped table-bordered table-condensed' >
                                <thead>
                                <tr>";
        $allresult.="<th>".$lang->getLang("Total Paid Win")."</th>
                     <th>".$lang->getLang("Total Paid Tax")."</th>
                     <th>".$lang->getLang("Total Paid")."</th>";
        $allresult.="</tr></thead>";


        /*<div class='btn-group'>
							<button class='btn btn-warning btn-sm dropdown-toggle' data-toggle='dropdown'><i class='fa fa-bars'></i> Export Table Data</button>
							<ul class='dropdown-menu ' role='menu'>
								<li><a href='#' onclick=\"$('.displayTicketPaid').tableExport({type:'csv',escape:'false'});\"> <img src=\"/media/htmlTableExport/icons/csv.png\" width=\"24px\"> CSV</a></li>
								<li><a href='#' onclick=\"$('.displayTicketPaid').tableExport({type:'txt',escape:'false'});\"><img src='/media/htmlTableExport/icons/txt.png' width='24px'> TXT</a></li>
								<li class='divider'></li>

								<li><a href='#' onclick=\"$('.displayTicketPaid').tableExport({type:'excel',escape:'false'});\"><img src='/media/htmlTableExport/icons/xls.png' width='24px'> XLS</a></li>
								<li><a href='#' onclick=\"$('.displayTicketPaid').tableExport({type:'doc',escape:'false'});\"><img src='/media/htmlTableExport/icons/word.png' width='24px'> Word</a></li>
								<li class='divider'></li>
								<li><a href='#' onclick=\"$('.displayTicketPaid').tableExport({type:'png',escape:'false'});\"><img src='/media/htmlTableExport/icons/png.png' width='24px'> PNG</a></li>
								<li><a href='#' onclick=\"$('.displayTicketPaid').tableExport({type:'pdf',pdfFontSize:'7',escape:'false'});\"><img src='/media/htmlTableExport/icons/pdf.png' width='24px'> PDF</a></li>


							</ul>
						</div>*/

        $return="
        <table width='100%' class='display displayTicketPaid table table-striped table-bordered table-condensed'>
                                <thead>
                                <tr>
                                    <th>".$lang->getLang("Res ID")."</th>
                                    <th>".$lang->getLang("Id Ticket")."</th>
                                    <th>".$lang->getLang("Date")."</th>
                                    <th>".$lang->getLang("Club")."</th>
                                    <th>".$lang->getLang("Game")."</th>
                                    <th>".$lang->getLang("Total Paid")."</th>
                                    <th>".$lang->getLang("Win Tax")."</th>";
        if($ticketInfo!="false") {
            $return .= "  <th>" . $lang->getLang("Ticket info") . "</th> ";
        }
         $return.="</thead>";
        while($rs->hasNext()){
            $rec=$rs->next();

            if($date<='2015-02-13'){
                if(TAX_CALCULATION_ACTIVE=='ON'){
                $amountPaidTax=($rec['vtp_amount_payed']>600)?($rec['vtp_amount_payed']-600)/100*25 : 0;
                }
                else{
                    $amountPaidTax=0;
                }
                $amountPaidTax+=$rec['vtp_cashout_tax'];
                $amountPaid=$rec['vtp_amount_payed']-$amountPaidTax;
            }
            else{
                if(TAX_CALCULATION_ACTIVE=='ON'){
                    $amountPaidTax=$rec['vtp_amount_payed']/100;
                }
                else{
                    $amountPaidTax=0;
                }
                $amountPaidTax+=$rec['vtp_cashout_tax'];
                $amountPaid=$rec['vtp_amount_payed']-$amountPaidTax;
            }
            if($rec["currency"]!=$_SESSION["currency_id"]){
                if($_SESSION["currency_id"]==1){
                    $amountPaid=bcdiv($amountPaid,$_SESSION["currencies"][$rec["currency"]]["value"],6);
                    $amountPaidTax=bcdiv($amountPaidTax,$_SESSION["currencies"][$rec["currency"]]["value"],6);

                }
                else{
                    $amountPaid=bcmul($amountPaid,$_SESSION["currencies"][$_SESSION["currency_id"]]["value"],6);
                    $amountPaidTax=bcmul($amountPaidTax,$_SESSION["currencies"][$_SESSION["currency_id"]]["value"],4);
                }
            }

            $totalPaid+=$amountPaid;
            $totalTax+=$amountPaidTax;
            $betting='';
            $gameNr=substr($rec["vtp_ticket_id"], 0, strpos($rec["vtp_ticket_id"], '-'));
            if($rec['vtp_gam_id']==215 || $rec['vtp_gam_id']==216){
                $betting="&betting=true";
                $gameNr=$rec["vtp_ticket_id"];
            }

            $return .="<tr><td style='text-align:center;padding:5px'>".getClassLink("javascript:openWindow('/casino_games/hand_details.php?gmn={$gameNr}&uid={$rec["vtp_pun_id"]}&hand_id={$rec["vtp_res_id"]}".$betting."','hand{$rec["vtp_res_id"]}','950','980','0','1')", $rec["vtp_res_id"], "contentlink")."</td>";
            $return .="<td style='text-align:center;padding:5px'>".getClassLink("javascript:openWindow('/casino_games/hand_details.php?gmn={$gameNr}&uid={$rec["vtp_pun_id"]}&hand_id={$rec["vtp_res_id"]}".$betting."','hand{$rec["vtp_res_id"]}','950','980','0','1')", $rec["vtp_ticket_id"], "contentlink")."</td>";
            $return .="<td style='text-align:center;padding:5px'>".($rec["vtp_time"])."</td>";
            $return .="<td style='text-align:center;padding:5px'>".($rec["club_name"])."</td>";
            $return .="<td style='text-align:center;padding:5px'>".($rec["gam_name"])."</td>";
            $return .="<td style='text-align:center;padding:5px'>".getInDollars($amountPaid,2,$rec['cur_code_for_web'])."</td>";
            $return .="<td style='text-align:center;padding:5px'>".getInDollars($amountPaidTax,2,$rec['cur_code_for_web'])."</td>";
            if($ticketInfo!="false") {
                $tinfo= $rec['vti_name'] . "," . $rec['vti_lastname'] . "," . $rec['vti_document_code'];
                if($tinfo==',,'){
                    $tinfo='-';
                }
                $return .= "<td style='text-align:center;padding:5px'>" .$tinfo . "</td>";
            }

            $return.="</tr>";
        }
        $return.="</table>";
        $allresult.="<tr><td>".getInDollars($totalPaid,2,$_SESSION['currency_html'])."</td><td>".getInDollars($totalTax,2,$_SESSION['currency_html'])."</td><td>".getInDollars($totalPaid+$totalTax,2,$_SESSION['currency_html'])."</td></tr></table><br />";
    }
    else{
        $return="<div class='alert alert-error'><button type='button' class='close' data-dismiss='alert'>&times;</button><strong>".$lang->getLang('No result found')."</strong></div>";
    }

    if($pdf){
        return $return;
    }
    else{
        $returnStr=array();
        $returnStr[0] = $time;
        $returnStr[1] = $allresult;
        $returnStr[2] = $return;
        return json_encode($returnStr);
    }

}




function getTicketsNotPaid($date,$games,$jurisdiction,$tid=false,$allSelected=false){
    global $dbh,$lang;
    if(is_array($jurisdiction)){
        $jurisdiction=implode(',',$jurisdiction);
    }
    if(in_array('215',$games)){
        $betting=true;
    }
    if(in_array('216',$games)){
        $livebetting=true;
    }
    if(is_array($games)){
        $games=implode(',',$games);
    }
    $jur_id=$_SESSION['jurisdiction_id'];
    $conditions='';
    switch($_SESSION["jurisdiction_class"]){
        case "club":
            $conditions .= " And c.jur_id = $jur_id";
            break;
        case "district":
            $conditions .= " And d.jur_id = $jur_id";
            break;
        case "region":
            $conditions .= " And r.jur_id = $jur_id";
            break;
        case "nation":
            $conditions .= " And n.jur_id = $jur_id";
            break;
        case "casino":
            $conditions .= " And 1=1";
            break;
        default:
            $conditions .= " And 1=0";
            break;
    }
    if(!$allSelected || $allSelected=='false'){
        $conditions.=" AND c.jur_id in ($jurisdiction) ";
    }


    $sql="select pre_pun_id pun_id,pre_res_id res_id, c.jur_name club_name, gam_name, pre_time TIMEC, pre_bet BET, pre_win WIN, pre_rake RAKE, pre_game_num TICKET_ID,cur_code_for_web,cur_id  currency,'false' betting
          from punter_result force index (pre_time), game, jurisdiction c, jurisdiction d, jurisdiction r, jurisdiction n, virtual_games_user,currency
          where pre_time BETWEEN '$date 00:00:00' AND '$date 23:59:59'
          and cur_id=c.jur_currency
          and pre_pun_id = vgu_pun_id
          and vgu_jur_code = c.jur_code
          and gam_id = vgu_gam_id
          and pre_bets_paid not like '%payed'
          and pre_win > 0
          and c.jur_parent_id=d.jur_id
          and d.jur_parent_id=r.jur_id
          and r.jur_parent_id=n.jur_id
          and n.jur_parent_id=1
          ";
    $sql.=$conditions;
    $sql.=" AND gam_id in ($games) ";
    if($tid!=''){ $sql.=" AND pre_game_num = ".$dbh->prepareString($tid);}
    if($betting){
    $sql.=" UNION ALL
          select ExtIDUtente pun_id,IdTicket res_id, c.jur_name club_name, 'betting', TStamp TIMEC, Importo + Tax BET, TVincita WIN, Importo + Tax - TVincita RAKE, CodiceTicket TICKET_ID,cur_code_for_web,cur_id currency,'true' betting
          from Ticket_dev, jurisdiction c, jurisdiction d, jurisdiction r, jurisdiction n, virtual_games_user,currency
          where  ExtIDUtente = VGU_PUN_ID
          and vgu_jur_code = c.jur_code
          and cur_id=c.jur_currency
          and Pagato = 0
          and TStamp BETWEEN '$date 00:00:00' AND '$date 23:59:59'
          and TVincita > 0
          and Tstato = 1
          and c.jur_parent_id=d.jur_id
          and d.jur_parent_id=r.jur_id
          and r.jur_parent_id=n.jur_id
          and n.jur_parent_id=1";
        $sql.=$conditions;
        if($tid!=''){ $sql.=" AND CodiceTicket = ".$dbh->prepareString($tid);}
    }
    if($livebetting){
        $sql.=" UNION ALL
          select ltk_pun_id pun_id,ltk_id res_id, c.jur_name club_name, 'livebetting', ltk_tstamp TIMEC, ltk_amount + ltk_tax BET, ltk_win WIN, ltk_amount + ltk_tax - ltk_win RAKE, ltk_code TICKET_ID,cur_code_for_web,cur_id currency,'true' betting
          from live_tickets, jurisdiction c, jurisdiction d, jurisdiction r, jurisdiction n, virtual_games_user,currency
          where  ltk_pun_id = VGU_PUN_ID
          and vgu_jur_code = c.jur_code
          and cur_id=c.jur_currency
          and ltk_payed is null
          and ltk_tstamp BETWEEN '$date 00:00:00' AND '$date 23:59:59'
          and ltk_win > 0
          and ltk_state = 1
          and c.jur_parent_id=d.jur_id
          and d.jur_parent_id=r.jur_id
          and r.jur_parent_id=n.jur_id
          and n.jur_parent_id=1";
        $sql.=$conditions;
        if($tid!=''){ $sql.=" AND ltk_code = ".$dbh->prepareString($tid);}
    }


    $starttime = microtime(true);
    $rs=$dbh->exec($sql);
    $endtime = microtime(true);
    $duration = $endtime - $starttime;
    $duration=number_format($duration, 4, ',', '')."s";
    $time="<div class='tip fleft'>".$lang->getLang("Time taken to execute your request").":".$duration."</div><br /><br /><br />";
    if($rs->getNumRows()>0){
        $totalBet=0;
        $totalWin=0;
        $totalRake=0;
        $allresult="<table class='display table table-striped table-bordered table-condensed' >
                                <thead>
                                <tr>";
        $allresult.="<th>".$lang->getLang("Total Bet")."</th>
                     <th>".$lang->getLang("Total Win")."</th>
                     <th>".$lang->getLang("Total Rake")."</th>";
        $allresult.="</tr></thead>";
        $return=" <table width='100%' class='display displayTicketNotPaid table table-striped table-bordered table-condensed'>
                                <thead>
                                <tr>
                                    <th>".$lang->getLang("Res ID")."</th>
                                    <th>".$lang->getLang("Id Ticket")."</th>
                                    <th>".$lang->getLang("Date")."</th>
                                    <th>".$lang->getLang("Club")."</th>
                                    <th>".$lang->getLang("Game")."</th>
                                    <th>".$lang->getLang("Bet")."</th>
                                    <th>".$lang->getLang("Win")."</th>
                                    <th>".$lang->getLang("Rake")."</th> </thead>";
        while($rs->hasNext()){
            $rec=$rs->next();
            if($rec['betting']==true){
                $betting="&betting=true";
            }
            $return .="<tr><td style='text-align:center;padding:5px'>".getClassLink("javascript:openWindow('/casino_games/hand_details.php?gmn={$rec['ticket_id']}&uid={$rec["pun_id"]}&hand_id={$rec["res_id"]}$betting','hand{$rec["res_id"]}','950','980','0','1')", $rec["res_id"], "contentlink")."</td>";
            $return .="<td style='text-align:center;padding:5px'>".getClassLink("javascript:openWindow('/casino_games/hand_details.php?gmn={$rec['ticket_id']}&uid={$rec["pun_id"]}&hand_id={$rec["res_id"]}$betting','hand{$rec["res_id"]}','950','980','0','1')", $rec["ticket_id"], "contentlink")."</td>";
            $return .="<td style='text-align:center;padding:5px'>".($rec["timec"])."</td>";
            $return .="<td style='text-align:center;padding:5px'>".($rec["club_name"])."</td>";
            $return .="<td style='text-align:center;padding:5px'>".($rec["gam_name"])."</td>";
            $return .="<td style='text-align:center;padding:5px'>".getInDollars($rec["bet"],2,$rec['cur_code_for_web'])."</td>";
            $return .="<td style='text-align:center;padding:5px'>".getInDollars($rec["win"],2,$rec['cur_code_for_web'])."</td>";
            $return .="<td style='text-align:center;padding:5px'>".getInDollars($rec["rake"],2,$rec['cur_code_for_web'])."</td></tr>";

            if($rec["currency"]!=$_SESSION["currency_id"]){
                if($_SESSION["currency_id"]==1){
                    $totalBet+=bcdiv($rec["bet"],$_SESSION["currencies"][$rec["currency"]]["value"],3);
                    $totalWin+=bcdiv($rec["win"],$_SESSION["currencies"][$rec["currency"]]["value"],3);
                    $totalRake+=bcdiv($rec["rake"],$_SESSION["currencies"][$rec["currency"]]["value"],3);

                }
                else{
                    $totalBet+=bcmul($rec["bet"],$_SESSION["currencies"][$_SESSION["currency_id"]]["value"],3);
                    $totalWin+=bcmul($rec["win"],$_SESSION["currencies"][$_SESSION["currency_id"]]["value"],3);
                    $totalRake+=bcmul($rec["rake"],$_SESSION["currencies"][$_SESSION["currency_id"]]["value"],3);
                }
            }
            elsE{
                $totalBet+=$rec["bet"];
                $totalWin+=$rec["win"];
                $totalRake+=$rec["rake"];
            }
        }
        $return.="</table>";
        $allresult.="<tr><td>".getInDollars($totalBet,2,$_SESSION['currency_html'])."</td><td>".getInDollars($totalWin,2,$_SESSION['currency_html'])."</td><td>".getInDollars($totalRake,2,$_SESSION['currency_html'])."</td></tr></table><br />";
    }
    else{
        $return="<div class='alert alert-error'><button type='button' class='close' data-dismiss='alert'>&times;</button><strong>".$lang->getLang('No result found')."</strong></div>";
    }


        $returnStr=array();
        $returnStr[0] = $time;
        $returnStr[1] = $allresult;
        $returnStr[2] = $return;
        return json_encode($returnStr);


}


/**
 * @param $date
 * @param $date_end
 * @param $jurisdiction
 * @param $group
 * @param bool $allSelected
 */
function getFinancialTotals($date,$date_end,$jurisdiction,$group,$allSelected=false){
    global $dbh,$lang;
    $date=$dbh->escape($date);
    $date_end=$dbh->escape($date_end);
    $allSelected=$dbh->escape($allSelected);
    if(is_array($jurisdiction)){
        $jurisdiction=implode(',',$jurisdiction);
    }
    $jur_id = $_SESSION["jurisdiction_id"];
    $today=date('Y-m-d');
    $yesterday=date('Y-m-d',time() - 60 * 60 * 24);
    $sql="Select *  FROM virtual_daily_summary
                    JOIN punter ON pun_id = vds_pun_id AND vds_date BETWEEN '$date' AND '$date_end'    ";
    if(!$allSelected || $allSelected=='false'){
        $sql.=" AND pun_betting_club in ($jurisdiction) ";
    }
    $sql .=" JOIN jurisdiction club on pun_betting_club=club.jur_id
             JOIN currency on club.jur_currency=cur_id ";
    if($_SESSION["jurisdiction_class"] != "casino"){
        $sql .= " JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
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
    $starttime = microtime(true);
    $rs=$dbh->exec($sql);
    $endtime = microtime(true);
    $duration = $endtime - $starttime;
    $duration=number_format($duration, 4, ',', '')."s";
    $time="<div class='tip fleft'>".$lang->getLang("Time taken to execute your request").":".$duration."</div><br /><br /><br />";

    if($rs->getNumRows()>0){
        $return="<table class='display displayFinancial table table-striped table-bordered table-condensed'  width='100%'>
                                <thead>
                                <tr>";
            $return.="<th>".$lang->getLang("Club")."</th>";
            $return.="<th>".$lang->getLang("Total Bet")."</th>";
            $return.="<th>".$lang->getLang("Total Win")."</th>";
            $return.="<th class='unwrappable'>".$lang->getLang("Rake")."</th>";
            $return.="<th>".$lang->getLang("Ticket played")."</th>";
            $return.="<th>".$lang->getLang("Total Bet6")."</th>
                      <th>".$lang->getLang("Total Dogs")."</th>";
            $return.="<th>".$lang->getLang("Total Roulette")."</th>
                      <th>".$lang->getLang("Total Soccer")."</th>";
            $return.="<th class='unwrappable'>".$lang->getLang("Total betting")."</th>";
            $return.="</tr>
        </thead>";
        while($rs->hasNext()){
            $row=$rs->next();
            $return.="<tr><td></td>";
            $return.="<td>".getInDollars($row['vds_bet'])."</td>";
            $return.="<td>".getInDollars($row['vds_win'])."</td>";
            $return.="<td>".getInDollars($row['vds_rake'])."</td>";
            $return.="<td></td>";
            $return.="<td></td>";
            $return.="<td></td>";
            $return.="<td></td>";
            $return.="<td></td>";
            $return.="<td></td>";
            $return.="</tr>";

        }
        $return.="</table>";
    }
else{
    $return="<div class='alert alert-error'><button type='button' class='close' data-dismiss='alert'>&times;</button><strong>".$lang->getLang('No result found')."</strong></div>";
}

    $returnStr=array();
    $returnStr[0] = $time;
    $returnStr[1]= '';
    $returnStr[2] = $return;
    return json_encode($returnStr);


}




/**
 * @param $date
 * @param $jurisdiction
 * @return string
 */
function getFinancialRecords($date,$date_end,$jurisdiction,$group,$allSelected=false,$pdf=false,$type="0"){
    global $dbh,$lang;
    $date=$dbh->escape($date);
    $date_end=$dbh->escape($date_end);
    $allSelected=$dbh->escape($allSelected);
    if(is_array($jurisdiction)){
        $jurisdiction=implode(',',$jurisdiction);
    }
    $jur_id = $_SESSION["jurisdiction_id"];
    $today=date('Y-m-d');
    $yesterday=date('Y-m-d',time() - 60 * 60 * 24);
    if($date < $today){
        if($date_end==$today){
            $sql=" SELECT pun_username, pun_id, club.jur_name,cur_code_for_web,cur_id as currency,
                     sum(coalesce(tickets_played, 0))tickets_played,
                     sum(coalesce(total_bet, 0))total_bet,
                     sum(coalesce(total_win, 0))total_win,
                     sum(coalesce(total_win_tax, 0))total_win_tax,
                     sum(coalesce(total_balance, 0))total_balance,
                     cashier_ip,
                     sum(coalesce(tickets_won, 0)) tickets_won,
                     sum(coalesce(tickets_erased, 0)) tickets_erased,
                     sum(coalesce(tickets_payed, 0)) tickets_payed,
                     sum(coalesce(tickets_payed_win, 0)) tickets_payed_win,
                     sum(coalesce (tickets_payed_win_tax)) tickets_payed_win_tax,
                     sum(coalesce(tickets_specials, 0)) tickets_specials,
                    (select pcr_credits from punter_credit where pcr_pun_id = pun_id) current_credit,
                     sum(coalesce(pre_bet_tax, 0)) pre_bet_tax,
                     sum(coalesce(pre_win_tax, 0)) pre_win_tax,
                     
                     vgu_gam_id
                        FROM virtual_games_user
                        LEFT JOIN
                        ( SELECT pre_pun_id user_id, count(pre_game_num) tickets_played, 
                        sum(coalesce(PRE_BET, 0)) total_bet, sum(coalesce(PRE_WIN, 0)) total_win, 
                        coalesce(PRE_BALANCE, 0) total_balance, 
                        (sum(if (pre_bet_tax>0 , (pre_win) *0.01, 0) +pre_win_tax)) total_win_tax, 
                        PRE_PUN_IP cashier_ip, sum(case when pre_win > 0 then 1 else 0 end) tickets_won, 
                        sum(case when pre_bet_string like '%handerased' then 1 else 0 end) tickets_erased,
                        0 tickets_payed_win,
                        0 tickets_payed,
                        0 tickets_payed_win_tax,
                        sum(case when pre_bet_string like '%spec%' then 1 else 0 end) tickets_specials,
                         sum(COALESCE(pre_bet_tax, 0)) pre_bet_tax,
                         sum(COALESCE(pre_win_tax, 0)) pre_win_tax
                            FROM punter_result force index (pre_time)
                            WHERE pre_time BETWEEN '$today 00:00:00' AND '$date_end 23:59:59'
                            group by pre_pun_id
                            UNION ALL


                           SELECT ExtIDUtente user_id, count(IDTicket) tickets_played,sum(IF(TStato <> 4, coalesce(Importo, 0) + coalesce(Tax, 0), 0)) total_bet, 0 total_win, 0 total_balance, 0 total_win_tax, IPAddress cashier_ip, 0 tickets_won, sum(case when TStato = 4 then 1 else 0 end) tickets_erased, 0 tickets_payed_win,
                          0 tickets_payed,
                          0 tickets_payed_win_tax,
                          0 tickets_specials, sum(COALESCE(Tax, 0)) pre_bet_tax,
                          0 pre_win_tax
                          FROM Ticket_dev
                          WHERE TStamp BETWEEN '$today 00:00:00' AND '$date_end 23:59:59'
                          group by ExtIDUtente

                        UNION ALL
                          SELECT ExtIDUtente user_id,  count(IDTicket) tickets_played,0 total_bet, sum(coalesce(TVincita, 0)) total_win, 0 total_balance, sum(if (Tax > 0 , TVincita *0.01, 0)) total_win_tax, IPAddress cashier_ip, sum(case when TVincita > 0 then 1 else 0 end) tickets_won, 0 tickets_erased, sum(case when Pagato = 1 then TVincita else 0 end) tickets_payed_win,
                          sum(case when Pagato = 1 then 1 else 0 end) tickets_payed,
                          sum(case when Pagato = 1 and Tax > 0  then (TVincita) * 0.01 else 0 end) tickets_payed_win_tax,
                          0 tickets_specials, 0 pre_bet_tax,
                          0 pre_win_tax 
                          FROM Ticket_dev
                          WHERE TStampAccredito BETWEEN  '$today 00:00:00' AND '$date_end 23:59:59'
                          group by ExtIDUtente

                         UNION ALL
                          SELECT ltk_pun_id user_id, count(ltk_id) tickets_played,sum(IF(LTK_STATE <> 4, coalesce(LTK_AMOUNT, 0) + coalesce(LTK_TAX, 0), 0)) total_bet, 0 total_win, 0 total_balance, 0 total_win_tax, ltk_ipadd cashier_ip, 0 tickets_won, sum(case when ltk_state = 4 then 1 else 0 end) tickets_erased, 0 tickets_payed_win,
                          0 tickets_payed,
                          0 tickets_payed_win_tax,
                          0 tickets_specials, sum(COALESCE(LTK_TAX, 0)) pre_bet_tax,
                          0 pre_win_tax 
                          FROM live_tickets
                          WHERE LTK_TSTAMP BETWEEN '$today 00:00:00' AND '$date_end 23:59:59'
                          group by ltk_pun_id

                        UNION ALL
                          SELECT ltk_pun_id user_id,  count(ltk_id) tickets_played,0 total_bet, sum(coalesce(ltk_win, 0)) total_win, 0 total_balance, sum(if (LTK_TAX > 0 , LTK_WIN *0.01, 0)) total_win_tax, ltk_ipadd cashier_ip, sum(case when ltk_win > 0 then 1 else 0 end) tickets_won, 0 tickets_erased, sum(case when ltk_payed = 1 then ltk_win else 0 end) tickets_payed_win,
                          sum(case when ltk_payed = 1 then 1 else 0 end) tickets_payed,
                          sum(case when ltk_payed = 1 and ltk_tax > 0  then (ltk_win) * 0.01 else 0 end) tickets_payed_win_tax,
                          0 tickets_specials, 0 pre_bet_tax,
                          0 pre_win_tax 
                          FROM live_tickets
                          WHERE LTK_WIN_TSTAMP BETWEEN  '$today 00:00:00' AND '$date_end 23:59:59'
                          group by ltk_pun_id

                        UNION ALL
                          SELECT vds_pun_id user_id,
                          sum(coalesce(vds_tickets,0)) tickets_played,
                          sum(coalesce(vds_bet, 0)) total_bet,
                          sum(coalesce(vds_win, 0)) total_win,
                          sum(coalesce(vds_start_credit, 0)) total_balance,
                          sum(coalesce(vds_win_tax, 0)) total_win_tax,
                          vds_cashier_ip cashier_ip,
                          sum(coalesce(vds_ticket_won, 0)) tickets_won,
                          sum(coalesce(vds_ticket_erased, 0)) tickets_erased,
                          sum(coalesce(vds_ticket_paid_amount, 0)) tickets_payed_win,
                          sum(coalesce(vds_ticket_paid, 0)) tickets_payed,
                          sum(coalesce(vds_ticket_paid_amount_tax, 0)) tickets_payed_win_tax,
                          sum(coalesce(vds_ticket_special, 0)) tickets_specials,
                          sum(coalesce(vds_bet_tax, 0)) pre_bet_tax,
                          0 pre_win_tax
                          FROM virtual_daily_summary
                          WHERE vds_date BETWEEN '$date' AND '$yesterday'
                           group by user_id
                           UNION ALL
                           SELECT vtp_pun_id user_id,
                            0 tickets_played,
                            0 total_bet,
                            0 total_win,
                            0 total_balance,
                            0 total_win_tax,
                            null cashier_ip,
                            0 tickets_won,
                            0 tickets_erased,
                            sum(coalesce(vtp_amount_payed, 0)) tickets_payed_win,
                            count(vtp_ticket_id) tickets_payed,
                            (sum((case when 1 = 1 then (vtp_amount_payed) * 0.01 else 0 end) + vtp_cashout_tax)) tickets_payed_win_tax,
                            0 tickets_specials,
                            0 pre_bet_tax,
                            0 pre_win_tax
                            FROM virtual_ticket_payed
                            WHERE vtp_time BETWEEN '$today 00:00:00' AND '$today 23:59:59'
                            group by user_id

                        ) t ON vgu_pun_id = user_id
                      JOIN punter ON user_id = pun_id ";
        }
        else{
            $sql="SELECT pun_username, pun_id, club.jur_name,cur_code_for_web,cur_id as currency,
                    sum(coalesce(vds_tickets,0)) tickets_played,
                    sum(coalesce(vds_bet, 0)) total_bet,
                    sum(coalesce(vds_win, 0)) total_win, sum(coalesce(vds_win_tax, 0)) total_win_tax,
                    sum(coalesce(vds_start_credit, 0)) total_balance, vds_cashier_ip cashier_ip,
                    sum(coalesce(vds_ticket_won, 0)) tickets_won, sum(coalesce(vds_ticket_erased, 0)) tickets_erased,
                    sum(coalesce(vds_ticket_paid, 0)) tickets_payed, sum(coalesce(vds_ticket_paid_amount, 0)) tickets_payed_win,
                    sum(coalesce(vds_ticket_paid_amount_tax, 0)) tickets_payed_win_tax,
                    sum(coalesce(vds_ticket_special, 0)) tickets_specials,
                    (select pcr_credits from punter_credit where pcr_pun_id = pun_id) current_credit ,sum(coalesce(vds_bet_tax, 0)) pre_bet_tax,
                    vds_gam_id vgu_gam_id
                    FROM virtual_daily_summary
                    JOIN punter ON pun_id = vds_pun_id AND vds_date BETWEEN '$date' AND '$date_end'   ";
        }
    }
    else{
        $sql="SELECT pun_username, pun_id, club.jur_name,cur_code_for_web,cur_id as currency,
              sum(coalesce(tickets_played,0)) tickets_played,
              sum(coalesce(total_bet,0)) total_bet,
              sum(coalesce(total_win,0)) total_win,
              sum(coalesce(total_win_tax,0)) total_win_tax,
              sum(coalesce(total_balance,0)) total_balance, cashier_ip,
              sum(coalesce(tickets_won,0)) tickets_won,
              sum(coalesce(tickets_erased,0))tickets_erased,
              sum(coalesce(tickets_payed,0)) tickets_payed,
              sum(coalesce(tickets_payed_win,0)) tickets_payed_win,
              sum(coalesce(tickets_payed_win_tax, 0)) tickets_payed_win_tax,
              sum(coalesce(tickets_specials,0)) tickets_specials,
              (select pcr_credits from punter_credit where pcr_pun_id = pun_id) current_credit,
              sum(coalesce(pre_bet_tax,0)) pre_bet_tax,
              vgu_gam_id
              FROM virtual_games_user
                  LEFT JOIN ( SELECT pre_pun_id user_id, count(pre_game_num) tickets_played, 
                  sum(coalesce(PRE_BET, 0)) total_bet, sum(coalesce(PRE_WIN, 0)) total_win, 
                  coalesce(PRE_BALANCE, 0) total_balance, 
                  (sum(if (pre_bet_tax>0 , (pre_win) *0.01, 0)+pre_win_tax)) total_win_tax, 
                  PRE_PUN_IP cashier_ip, 
                  sum(case when pre_win > 0 then 1 else 0 end) tickets_won, 
                  sum(case when pre_bet_string like '%handerased' then 1 else 0 end) tickets_erased,
                  0 tickets_payed_win,
                  0 tickets_payed,
                  0 tickets_payed_win_tax,
                  sum(case when pre_bet_string like '%spec%' then 1 else 0 end) tickets_specials, sum(COALESCE(pre_bet_tax, 0)) pre_bet_tax
                  FROM punter_result force index (pre_time)
                  WHERE  pre_time BETWEEN '$date 00:00:00' AND '$date_end 23:59:59'
                  group by pre_pun_id
                  UNION ALL



                      SELECT ExtIDUtente user_id, count(IDTicket) tickets_played,sum(IF(TStato <> 4, coalesce(Importo, 0) + coalesce(Tax, 0), 0)) total_bet, 0 total_win, 0 total_balance, 0 total_win_tax, IPAddress cashier_ip, 0 tickets_won, sum(case when TStato = 4 then 1 else 0 end) tickets_erased, 0 tickets_payed_win,
                          0 tickets_payed,
                          0 tickets_payed_win_tax,
                          0 tickets_specials, sum(COALESCE(Tax, 0)) pre_bet_tax
                          FROM Ticket_dev
                          WHERE TStamp BETWEEN '$date 00:00:00' AND '$date_end 23:59:59'
                          group by ExtIDUtente

                             UNION ALL
                          SELECT ExtIDUtente user_id, 0 tickets_played,
                          0 total_bet, sum(coalesce(TVincita, 0)) total_win,
                          0 total_balance, sum(if (Tax > 0 , TVincita *0.01, 0)) total_win_tax,
                          IPAddress cashier_ip, sum(case when TVincita > 0 then 1 else 0 end) tickets_won, 0 tickets_erased,
			  0 tickets_payed_win,
                          0 tickets_payed,
                          0 tickets_payed_win_tax,
                          0 tickets_specials, 0 pre_bet_tax
                          FROM Ticket_dev
                          WHERE TStampAccredito BETWEEN  '$date 00:00:00' AND '$date_end 23:59:59'
                          group by ExtIDUtente



                         UNION ALL
                          SELECT ltk_pun_id user_id, count(ltk_id) tickets_played,sum(IF(LTK_STATE <> 4, coalesce(LTK_AMOUNT, 0) + coalesce(LTK_TAX, 0), 0)) total_bet, 0 total_win, 0 total_balance, 0 total_win_tax, ltk_ipadd cashier_ip, 0 tickets_won, sum(case when ltk_state = 4 then 1 else 0 end) tickets_erased, 0 tickets_payed_win,
                          0 tickets_payed,
                          0 tickets_payed_win_tax,
                          0 tickets_specials, sum(COALESCE(LTK_TAX, 0)) pre_bet_tax
                          FROM live_tickets
                          WHERE LTK_TSTAMP BETWEEN '$date 00:00:00' AND '$date_end 23:59:59'
                          group by ltk_pun_id

                        UNION ALL
                          SELECT ltk_pun_id user_id,  count(ltk_id) tickets_played,0 total_bet, sum(coalesce(ltk_win, 0)) total_win, 0 total_balance, sum(if (LTK_TAX > 0 , LTK_WIN *0.01, 0)) total_win_tax, ltk_ipadd cashier_ip, sum(case when ltk_win > 0 then 1 else 0 end) tickets_won, 0 tickets_erased, sum(case when ltk_payed = 1 then ltk_win else 0 end) tickets_payed_win,
                          sum(case when ltk_payed = 1 then 1 else 0 end) tickets_payed,
                          sum(case when ltk_payed = 1 and ltk_tax > 0  then (ltk_win) * 0.01 else 0 end) tickets_payed_win_tax,
                          0 tickets_specials, 0 pre_bet_tax
                          FROM live_tickets
                          WHERE LTK_WIN_TSTAMP BETWEEN  '$date 00:00:00' AND '$date_end 23:59:59'
                          group by ltk_pun_id


                UNION ALL
                SELECT vtp_pun_id user_id,
                0 tickets_played,
                0 total_bet,
                0 total_win,
                0 total_balance,
                0 total_win_tax,
                null cashier_ip,
                0 tickets_won,
                0 tickets_erased,
                sum(coalesce(vtp_amount_payed, 0)) tickets_payed_win,
                count(vtp_ticket_id) tickets_payed,
                sum((case when 1 = 1 then (vtp_amount_payed) * 0.01 else 0 end) + vtp_cashout_tax)  tickets_payed_win_tax,
                0 tickets_specials,
                0 pre_bet_tax
                FROM virtual_ticket_payed
                WHERE vtp_time BETWEEN '$date 00:00:00' AND '$date 23:59:59'
                group by user_id

                ) t ON vgu_pun_id = user_id
                JOIN punter ON user_id = pun_id ";
    }

    if(!$allSelected || $allSelected=='false'){
        $sql.=" AND pun_betting_club in ($jurisdiction) ";
    }

    $sql .=" JOIN jurisdiction club on pun_betting_club=club.jur_id
             JOIN currency on club.jur_currency=cur_id ";
    if($_SESSION["jurisdiction_class"] != "casino"){
        $sql .= " JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
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
    if($group=='0'){
        $sql .=" group by pun_betting_club";
    }
    elseif($group==3){
        $sql.=" group by vgu_gam_id";
    }
    else{
        $sql .=" group by  pun_id";
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
        $totalspecialtickets=0;
        $totalWinTax=0;
        $totalWinPaidTax=0;

        $allresult="<table class='display totalFinancial table table-striped table-bordered table-condensed' >
                                <thead>
                                <tr>
                                   <th>".$lang->getLang("Total Bet")."</th>";
        if(TAX_CALCULATION_ACTIVE=='ON'){
            $allresult.=" <th>".$lang->getLang("Total Bet Tax")."</th>";
        }
        $allresult.="<th>".$lang->getLang("Total Win")."</th>";
        if($type!="0"){
            $allresult.="<th>".$lang->getLang("Total cashier")."</th>";
        }else{
            $allresult.="<th>".$lang->getLang("Total Rake")."</th>";
        }

        $allresult.="<th>".$lang->getLang("Tickets played")."</th>
                     <th>".$lang->getLang("Tickets win")."</th>
                     <th>".$lang->getLang("Tickets paid")."</th>
                     <th>".$lang->getLang("Tickets paid amount")."</th>
                     <th>".$lang->getLang("Tickets erased")."</th>";
        $allresult.="</tr></thead>";


        $return="<table class='display displayFinancial table table-striped table-bordered table-condensed'  width='100%'>
                                <thead>
                                <tr>";
                                if($group!=3){
                                 $return.="<th>".$lang->getLang("Club")."</th>";
                                }
                                if($group!=3 && $group!=0){
                                    $return.="<th>".$lang->getLang("Cashier")."</th>";
                                }
                                if($group!=0){
                                    $return.="<th>".$lang->getLang("Game")."</th>";
                                }
                                $return.="<th class='unwrappable'>".$lang->getLang("Total Bet")."</th>";
        if(TAX_CALCULATION_ACTIVE=='ON'){
                           $return.="<th>".$lang->getLang("Total Bet Tax")."</th>";
        }
        $return.="                  <th>".$lang->getLang("Total Win")."</th>";

        if($type!="0"){
            $return.="<th>".$lang->getLang("Total cashier")."</th>";
        }
        else{
            $return.="<th>".$lang->getLang("Total Rake")."</th>";
        }

        if(check_access('virtual_cashier_show_credit') && $group==1){
                         $return.=" <th>".$lang->getLang("Start Credit")."</th>
                                    <th>".$lang->getLang("Current Credit")."</th>";
        }

        $return.="<th class='unwrappable'>".$lang->getLang("Tickets played")."</th>
                                    <th class='unwrappable'>".$lang->getLang("Tickets win")."</th>
                                     <th class='unwrappable'>".$lang->getLang("Tickets paid")."</th>
                                     <th class='unwrappable'>".$lang->getLang("Tickets paid amount")."</th>
                                     <th class='unwrappable'>".$lang->getLang("Tickets erased")."</th>";

        if($group!=0 && $group!=3){
            $return.="<th>".$lang->getLang("Ip")."</th>";
        }

        $return.="</tr>
        </thead>";
        while($rs->hasNext()){
            $rec=$rs->next();
           /* if(TAX_CALCULATION_ACTIVE!='ON'){
                $rec['pre_bet_tax']=0;
                $rec['tickets_payed_win_tax']=0;
                $rec['total_win_tax']=0;
            }*/
            if($rec['pre_bet_tax'] > 0){
                $totalbetTax+=($rec['pre_bet_tax']);
            }
            if($rec["currency"]!=$_SESSION["currency_id"]){
                if($_SESSION["currency_id"]==1){
                    $rec["total_bet"]=bcdiv($rec["total_bet"],$_SESSION["currencies"][$rec["currency"]]["value"],5);
                    $rec["total_win"]=bcdiv($rec["total_win"],$_SESSION["currencies"][$rec["currency"]]["value"],5);
                    $rec["tickets_payed_win"]=bcdiv($rec["tickets_payed_win"],$_SESSION["currencies"][$rec["currency"]]["value"],5);
                    $rec["total_win_tax"]=bcdiv($rec["total_win_tax"],$_SESSION["currencies"][$rec["currency"]]["value"],5);
                    $rec["tickets_payed_win_tax"]=bcdiv($rec["tickets_payed_win_tax"],$_SESSION["currencies"][$rec["currency"]]["value"],5);

                }
                else{
                    $rec["total_bet"]=bcmul($rec["total_bet"],$_SESSION["currencies"][$_SESSION["currency_id"]]["value"],5);
                    $rec["total_win"]=bcmul($rec["total_win"],$_SESSION["currencies"][$_SESSION["currency_id"]]["value"],5);
                    $rec["tickets_payed_win"]=bcmul($rec["tickets_payed_win"],$_SESSION["currencies"][$_SESSION["currency_id"]]["value"],5);
                    $rec["total_win_tax"]=bcmul($rec["total_win_tax"],$_SESSION["currencies"][$_SESSION["currency_id"]]["value"],5);
                    $rec["tickets_payed_win_tax"]=bcmul($rec["tickets_payed_win_tax"],$_SESSION["currencies"][$_SESSION["currency_id"]]["value"],5);
                }
            }



            $totalbet+=$rec['total_bet'];
            $totalwin+=$rec['total_win'];
            $ticketspaidamount+=$rec['tickets_payed_win'];
            $totalWinTax+=$rec['total_win_tax'];
            $totalWinPaidTax+=$rec["tickets_payed_win_tax"];


            $ticketsplayed+=$rec['tickets_played'];
            $ticketswin+=$rec['tickets_won'];
            $ticketspaid+=$rec['tickets_payed'];
            $ticketserased+=$rec['tickets_erased'];

            $totalspecialtickets+=$rec['tickets_specials'];

            $cashier=getGameAndCashier($rec['pun_username']);
            $return.="<tr>";
            if($group!=3){
                $return .="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>".($rec["jur_name"]).(($rec["currency"]!=$_SESSION["currency_id"] && $rec["currency"]!="") ? "<br><span class=changeCurr style=\"font-weight:bold;color:red;cursor:pointer;\" data-curr=".$rec["currency"].">".$rec['cur_code_for_web']."</span>" : '' )." </td>";
            }
            if($group!=3 && $group!=0){
                $return.="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>".getClassLink("javascript:openWindow('".refPage("customers/customer_view&customer_id={$rec["pun_id"]}")."','user{$rec["pun_username"]}','950','980','0','1')", $cashier['cashier'], "contentlink")."</td>";
            }
            if($group!=0){
                $return .="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>". getClassLink("javascript:openWindow('".refPage("customers/customer_view&customer_id={$rec["pun_id"]}")."','user{$rec["pun_username"]}','950','980','0','1')", $cashier['game'], "contentlink");

                if($group==3){
                    $return.=(($rec["currency"]!=$_SESSION["currency_id"] && $rec["currency"]!="") ? "<br><span class=changeCurr style=\"font-weight:bold;color:red;cursor:pointer;\" data-curr=".$rec["currency"].">".$_SESSION['currency_html']."</span>" : '' );

                }
                $return.="</td>";
            }

            $return .="<td style='white-space: nowrap;vertical-align:top'>".getInDollars($rec["total_bet"],2,$_SESSION['currency_html'])."</td>";
            if(TAX_CALCULATION_ACTIVE=='ON'){
                $return .="<td style='white-space: nowrap;vertical-align:top'>".($rec['pre_bet_tax'] !='' ? getInDollars(($rec['total_bet']- $rec['pre_bet_tax']),3)."<br />
                <span class='tip'>Tax : ".getInDollars($rec['pre_bet_tax'],3)."</span>" : '--' )."</td>";
            }
            $return .="<td style='white-space: nowrap;vertical-align:top'>".getInDollars($rec["total_win"]-$rec['total_win_tax'],2)."<br />
                     <span class='tip'>Tax : ".getInDollars($rec['total_win_tax'],3)."</span></td>";


            if($type!="0"){
                $return .="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>".getInDollars($rec["total_bet"]-($rec["tickets_payed_win"]-$rec['tickets_payed_win_tax']),2)."</td>";
            }else{
                $return .="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>".getInDollars($rec["total_bet"]-$rec["total_win"],2)."</td>";
            }




            if(check_access('virtual_cashier_show_credit') && $group==1){
                $return .="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>".getInDollars($rec["total_balance"]=='0'?$rec["current_credit"]:$rec["total_balance"] ,2)."</td>";
                $return .="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>".getInDollars($rec["current_credit"],2)."</td>";
            }
            $return .="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>".$rec["tickets_played"]."<br /><span class='tip'>special:".$rec['tickets_specials']."</span></td>";
            $return .="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>".$rec["tickets_won"]."</td>";
            $return .="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>".$rec["tickets_payed"]."</td>";
            $return .="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>".getInDollars($rec["tickets_payed_win"]-$rec['tickets_payed_win_tax'],2)."<br />
                     <span class='tip'>Tax : ".getInDollars($rec['tickets_payed_win_tax'],3,$rec['cur_code_for_web'])."</span></td>";
            $return .="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>".$rec["tickets_erased"]."</td>";
            if($group!=0 && $group!=3){
                $return.="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>".($rec['cashier_ip']!=''? getClassLink("javascript:openWindow('".refPage('customers/search&field=4&search_string=' . urlencode(ereg_replace("[^0-9.]", "", $rec["cashier_ip"]))) ."','ip','950','980','0','1')", $rec["cashier_ip"], "contentlink"):'--')."</td>";
            }
            $return .="</tr>";
        }
        $return.="</table>";

        $allresult .="<tr>
                    <td>".getInDollars($totalbet,2,$_SESSION['currency_html'])."</td> ";
        if(TAX_CALCULATION_ACTIVE=='ON'){
            $allresult .=" <td>".getInDollars(($totalbet-$totalbetTax),3)."<br /><span class='tip'>Tax : ".getInDollars($totalbetTax,3)."</span></td>";
        }
        $allresult .="
                    <td>".getInDollars($totalwin-$totalWinTax,2,$_SESSION['currency_html'])."<br />
                        <span class='tip'>Tax : ".getInDollars($totalWinTax,3)."</span>
                    </td>";


        if($type!="0"){
            $allresult.="<td>".getInDollars($totalbet-($ticketspaidamount-$totalWinPaidTax))."</td>";
        }else{

            $allresult.="<td>".getInDollars($totalbet-$totalwin)."</td>";
        }
        $allresult.="
                    <td>".$ticketsplayed."<br /><span class='tip'>special:".$totalspecialtickets."</span></td>
                    <td>".$ticketswin."</td>
                    <td>".$ticketspaid."</td>
                    <td>".getInDollars($ticketspaidamount-$totalWinPaidTax)."<br /><span class='tip'>Tax : ".getInDollars($totalWinPaidTax,3)."</span></td>
                    <td>".$ticketserased."</td>
                    </tr></table><br />";
    }
    else{
        $return="<div class='alert alert-error'><button type='button' class='close' data-dismiss='alert'>&times;</button><strong>".$lang->getLang('No result found')."</strong></div>";
    }
    if($pdf){
        return $return;
    }
    else{
        $returnStr=array();
        $returnStr[0] = $time;
        $returnStr[1]= $allresult;
        $returnStr[2] = $return;
        return json_encode($returnStr);
    }
}

/**
 * @param $date
 * @param $jurisdiction
 * @param $users
 * @param $group
 * @return string
 */
function getOverall($date,$date_end,$jurisdiction,$userType,$group,$allSelected=false){
    global $dbh,$lang;
    $date=$dbh->escape($date);
    $date_end=$dbh->escape($date_end);
    if(is_array($jurisdiction)){
        $jurisdiction=implode(',',$jurisdiction);
    }

    $users=getConectedUsers(true);
 /*   $users=explode('~',$users);
    $connected=array();
    foreach($users as $k=>$v){
        list($pl,$val) = explode(":", $v );
        list($prefix,$userid)=explode('_',$pl);
        array_push($connected,$userid);
    }*/


    $connected=array();
    foreach($users->Records as $k=>$v){
        array_push($connected,$v['usl_pun_id']);
    }


    $sql=" SELECT n.jur_name nation, n.jur_id nation_id,
                r.jur_name region, r.jur_id region_id,
                d.jur_name district, d.jur_id district_id,
                club.jur_name club, club.jur_id club_id,
                pun_username,
                pun_last_logged_in,
                pun_id,cur_code_for_web,cur_id as currency, ";
    $jur_id = $_SESSION["jurisdiction_id"];
    $today=date('Y-m-d');
    $yesterday=date('Y-m-d',time() - 60 * 60 * 24);


    if($date < $today){
        if($date_end==$today){
            $sql.="    sum(coalesce(tickets_played, 0)) tickets_played,
                       sum(coalesce(total_bet,0)) total_bet,
                       sum(coalesce(total_win,0)) total_win,
                       sum(coalesce(total_win_tax,0)) total_win_tax,
                       sum(coalesce(total_balance,0)) total_balance,
                       cashier_ip,
                       sum(coalesce(tickets_won,0)) tickets_won,
                       sum(coalesce(tickets_erased,0)) tickets_erased,
                       sum(coalesce(tickets_payed,0)) tickets_payed,
                       sum(coalesce(tickets_payed_win,0)) tickets_payed_win,
                       sum(coalesce(tickets_payed_win_tax,0)) tickets_payed_win_tax,
                       sum(coalesce(tickets_specials,0)) tickets_specials,
                       sum(coalesce(pre_bet_tax,0)) pre_bet_tax,
                
                       vgu_gam_id
                        FROM virtual_games_user
                        LEFT JOIN
                        ( SELECT pre_pun_id user_id, count(pre_game_num) tickets_played, 
                        sum(coalesce(PRE_BET, 0)) total_bet, 
                        sum(coalesce(PRE_WIN, 0)) total_win, 
                        coalesce(PRE_BALANCE, 0) total_balance, 
                        sum(if (pre_bet_tax>0 , (pre_win) *0.01, 0)+pre_win_tax) total_win_tax, 
                        PRE_PUN_IP cashier_ip, sum(case when pre_win > 0 then 1 else 0 end) tickets_won, 
                        sum(case when pre_bet_string like '%handerased' then 1 else 0 end) tickets_erased,
                        0 tickets_payed_win,
                        0 tickets_payed,
                        0 tickets_payed_win_tax,
                          sum(case when pre_bet_string like '%spec%' then 1 else 0 end) tickets_specials, 
                          sum(COALESCE(pre_bet_tax, 0)) pre_bet_tax 
                            FROM punter_result force index (pre_time)
                            WHERE pre_time BETWEEN '$today 00:00:00' AND '$date_end 23:59:59'
                            group by pre_pun_id
                        UNION ALL
                          SELECT vds_pun_id user_id, sum(coalesce(vds_tickets,0)) tickets_played, sum(coalesce(vds_bet, 0)) total_bet, sum(coalesce(vds_win, 0)) total_win, sum(coalesce(vds_start_credit, 0)) total_balance, sum(coalesce(vds_win_tax, 0)) total_win_tax, vds_cashier_ip cashier_ip, sum(coalesce(vds_ticket_won, 0)) tickets_won, sum(coalesce(vds_ticket_erased, 0)) tickets_erased,
                          0 tickets_payed_win,
                          0 tickets_payed,
                          0 tickets_payed_win_tax,
                          sum(coalesce(vds_ticket_special, 0)) tickets_specials, sum(coalesce(vds_bet_tax, 0)) pre_bet_tax
                          FROM virtual_daily_summary
                          WHERE vds_date BETWEEN '$date' AND '$yesterday'
                          group by user_id
                 UNION ALL


                           SELECT ExtIDUtente user_id, count(IDTicket) tickets_played,sum(IF(TStato <> 4, coalesce(Importo, 0) + coalesce(Tax, 0), 0)) total_bet, 0 total_win, 0 total_balance, 0 total_win_tax, IPAddress cashier_ip, 0 tickets_won, sum(case when TStato = 4 then 1 else 0 end) tickets_erased, 0 tickets_payed_win,
                          0 tickets_payed,
                          0 tickets_payed_win_tax,
                          0 tickets_specials, sum(COALESCE(Tax, 0)) pre_bet_tax
                          FROM Ticket_dev
                          WHERE TStamp BETWEEN '$today 00:00:00' AND '$date_end 23:59:59'
                          group by ExtIDUtente

                        UNION ALL
                          SELECT ExtIDUtente user_id,  count(IDTicket) tickets_played,0 total_bet, sum(coalesce(TVincita, 0)) total_win, 0 total_balance, sum(if (Tax > 0 , TVincita *0.01, 0)) total_win_tax, IPAddress cashier_ip, sum(case when TVincita > 0 then 1 else 0 end) tickets_won, 0 tickets_erased, sum(case when Pagato = 1 then TVincita else 0 end) tickets_payed_win,
                          sum(case when Pagato = 1 then 1 else 0 end) tickets_payed,
                          sum(case when Pagato = 1 and Tax > 0  then (TVincita) * 0.01 else 0 end) tickets_payed_win_tax,
                          0 tickets_specials, 0 pre_bet_tax
                          FROM Ticket_dev
                          WHERE TStampAccredito BETWEEN  '$today 00:00:00' AND '$date_end 23:59:59'
                          group by ExtIDUtente

                        UNION ALL

                SELECT vtp_pun_id user_id,
                0 tickets_played,
                0 total_bet,
                0 total_win,
                0 total_balance,
                0 total_win_tax,
                null cashier_ip,
                0 tickets_won,
                0 tickets_erased,
                sum(coalesce(vtp_amount_payed, 0)) tickets_payed_win,
                count(vtp_ticket_id) tickets_payed,
                (sum(case when 1=1 then (vtp_amount_payed) * 0.01 else 0 end)+vtp_cashout_tax) tickets_payed_win_tax,
                0 tickets_specials,
                0 pre_bet_tax
                FROM virtual_ticket_payed
                WHERE vtp_time BETWEEN '$today 00:00:00' AND '$date_end 23:59:59'
                group by user_id

                        ) t ON vgu_pun_id = user_id
                      JOIN punter ON user_id = pun_id ";
        }
        else{
            $sql.="    sum(coalesce(vds_tickets,0)) tickets_played, sum(coalesce(vds_bet, 0)) total_bet,
                    sum(coalesce(vds_win, 0)) total_win, sum(coalesce(vds_win_tax, 0)) total_win_tax,
                    sum(coalesce(vds_start_credit, 0)) total_balance, vds_cashier_ip cashier_ip,
                    sum(coalesce(vds_ticket_won, 0)) tickets_won, sum(coalesce(vds_ticket_erased, 0)) tickets_erased,
                    sum(coalesce(vds_ticket_paid, 0)) tickets_payed, sum(coalesce(vds_ticket_paid_amount, 0)) tickets_payed_win,
                    sum(coalesce(vds_ticket_paid_amount_tax, 0)) tickets_payed_win_tax,
                    sum(coalesce(vds_ticket_special, 0)) tickets_specials,
                    sum(coalesce(vds_bet_tax, 0)) pre_bet_tax,
                    vds_gam_id vgu_gam_id
                    FROM virtual_daily_summary
                    JOIN punter ON pun_id = vds_pun_id AND vds_date BETWEEN '$date' AND '$date_end'   ";
        }
    }
    else{
        $sql.="   sum(coalesce(tickets_played,0)) tickets_played,
                  sum(coalesce(total_bet,0)) total_bet,
                  sum(coalesce(total_win,0)) total_win,
                  sum(coalesce(total_win_tax,0)) total_win_tax,
                  sum(coalesce(total_balance,0)) total_balance,
                  cashier_ip,
                  sum(coalesce(tickets_won,0)) tickets_won,
                  sum(coalesce(tickets_erased,0)) tickets_erased,
                  sum(coalesce(tickets_payed,0)) tickets_payed,
                  sum(coalesce(tickets_payed_win,0)) tickets_payed_win,
                 sum(coalesce(tickets_payed_win_tax, 0)) tickets_payed_win_tax,
                  sum(coalesce(tickets_specials,0)) tickets_specials,
                  sum(coalesce(pre_bet_tax,0)) pre_bet_tax,
                 vgu_gam_id
                   FROM virtual_games_user
                  LEFT JOIN ( SELECT pre_pun_id user_id, 
                  count(pre_game_num) tickets_played, 
                  sum(coalesce(PRE_BET, 0)) total_bet, 
                  sum(coalesce(PRE_WIN, 0)) total_win, 
                  coalesce(PRE_BALANCE, 0) total_balance, 
                  sum(if (pre_bet_tax>0 , (pre_win) *0.01, 0)+pre_win_tax) total_win_tax, 
                  PRE_PUN_IP cashier_ip, sum(case when pre_win > 0 then 1 else 0 end) tickets_won, sum(case when pre_bet_string like '%handerased' then 1 else 0 end) tickets_erased,
                  0 tickets_payed_win,
                  0 tickets_payed,
                  0 tickets_payed_win_tax,
                  sum(case when pre_bet_string like '%spec%' then 1 else 0 end) tickets_specials, sum(COALESCE(pre_bet_tax, 0)) pre_bet_tax
                  FROM punter_result force index (pre_time)
                  WHERE  pre_time BETWEEN '$date 00:00:00' AND '$date_end 23:59:59'
                  group by pre_pun_id
                    UNION ALL
                    SELECT ExtIDUtente user_id, count(IDTicket) tickets_played, sum(IF(TStato <> 4, coalesce(Importo, 0) + coalesce(Tax, 0), 0)) total_bet, sum(coalesce(TVincita, 0)) total_win, 0 total_balance, sum(if (Tax > 0 , (TVincita) *0.01, 0)) total_win_tax, IPAddress cashier_ip, sum(case when TVincita > 0 then 1 else 0 end) tickets_won, sum(case when TStato = 4 then 1 else 0 end) tickets_erased,
                    0 tickets_payed_win,
                    0 tickets_payed,
                    0 tickets_payed_win_tax,
                    0 tickets_specials, sum(COALESCE(Tax, 0)) pre_bet_tax
                    FROM Ticket_dev
                   WHERE TStamp  BETWEEN '$date 00:00:00' AND '$date_end 23:59:59'
                    group by ExtIDUtente
                    UNION ALL

                    SELECT vtp_pun_id user_id,
                    0 tickets_played,
                    0 total_bet,
                    0 total_win,
                    0 total_balance,
                    0 total_win_tax,
                    null cashier_ip,
                    0 tickets_won,
                    0 tickets_erased,
                    sum(coalesce(vtp_amount_payed, 0)) tickets_payed_win,
                    count(vtp_ticket_id) tickets_payed,
                    sum((case when 1=1 then (vtp_amount_payed) * 0.01 else 0 end)+vtp_cashout_tax) tickets_payed_win_tax,
                    0 tickets_specials,
                    0 pre_bet_tax
                    FROM virtual_ticket_payed
                    WHERE vtp_time BETWEEN '$today 00:00:00' AND '$today 23:59:59'
                    group by user_id

                    ) t ON vgu_pun_id = user_id
                 JOIN punter ON user_id = pun_id";
    }


    if(!$allSelected || $allSelected=='false'){
        $sql.=" AND pun_betting_club in ($jurisdiction) ";
    }
    $sql .="
        LEFT JOIN jurisdiction club ON pun_betting_club = club.jur_id
        LEFT JOIN jurisdiction d ON club.jur_parent_id = d.jur_id
        LEFT JOIN jurisdiction r ON d.jur_parent_id = r.jur_id
        LEFT JOIN jurisdiction n ON r.jur_parent_id = n.jur_id
        AND n.jur_parent_id=1
        LEFT JOIN currency on club.jur_currency=cur_id";

    if($group=='0'){
        $sql .=" group by pun_betting_club";
    }
    elseif($group==3){
        $sql.=" group by vgu_gam_id";
    }
    else{
        $sql .=" group by  pun_id";
    }
    $starttime = microtime(true);
    $rs=$dbh->exec($sql);
    $endtime = microtime(true);
    $duration = $endtime - $starttime;
    $duration=number_format($duration, 4, ',', '')."s";
    $time="<div class='tip fleft'>".$lang->getLang("Time taken to execute your request").":".$duration."</div><br /><br /><br />";

    $totalbet=0;
    $totalwin=0;
    $totalbetTax=0;
    $ticketsplayed=0;
    $ticketswin=0;
    $ticketspaid=0;
    $ticketserased=0;
    $ticketspaidwin=0;
    $totalspecialtickets=0;
    $totalWinTax=0;
    $totalWinPaidTax=0;
    if($rs->getNumRows()>0){
        $allresult="<table class='display table table-striped table-bordered table-condensed'>
                                <thead>
                                <tr>
                                   <th>".$lang->getLang("Total Bet")."</th>";
        if(TAX_CALCULATION_ACTIVE=='ON'){
            $allresult.=" <th>".$lang->getLang("Total Bet Tax")."</th>";
        }
        $allresult.="<th>".$lang->getLang("Total Win")."</th>
                     <th>".$lang->getLang("Total Rake")."</th>
                     <th>".$lang->getLang("Tickets played")."</th>
                     <th>".$lang->getLang("Tickets win")."</th>";
        if($group==2){
            $allresult.="<th>".$lang->getLang("Tickets paid")."</th>
                     <th>".$lang->getLang("Tickets paid amount")."</th>
                     <th>".$lang->getLang("Tickets erased")."</th>";

        }
        $allresult.="</tr></thead>";

        $return="<table class='display displayOverall table table-striped table-bordered table-condensed'  width='100%' >
                                <thead>
                                <tr> ";
        if($group!=3){
            $return.="<th>".$lang->getLang("Nation")."</th>
                                    <th>".$lang->getLang("Region")."</th>
                                    <th>".$lang->getLang("District")."</th>
                                    <th>".$lang->getLang("Club")."</th>";
                                    }
        if($group=='1'){
            $return .=" <th>".$lang->getLang("Cashier")."</th>";
            $return .=" <th>".$lang->getLang("Game")."</th>";
        }
        if($group=='3'){
            $return .=" <th>".$lang->getLang("Game")."</th>";
        }

        $return.="<th>".$lang->getLang("Total Bet")."</th>";
        if(TAX_CALCULATION_ACTIVE=='ON'){
            $return.=" <th>".$lang->getLang("Total Bet Tax")."</th>";
        }
        $return.=" <th>".$lang->getLang("Total Win")."</th>
                                    <th>".$lang->getLang("Total Rake")."</th>
                                    <th>".$lang->getLang("Tickets played")."</th>
                                    <th>".$lang->getLang("Tickets win")."</th>";
        if($group!=3){
                   $return.="<th>".$lang->getLang("Status")."</th>
                                    <th>".$lang->getLang("Last login")."</th>
                                    <th>".$lang->getLang("Ip")."</th>";
        }
         $return.="</tr></thead>";
        while($rs->hasNext()){
            $rec=$rs->next();
            $cashier=getGameAndCashier($rec['pun_username']);

            /*if(TAX_CALCULATION_ACTIVE!='ON'){
                $rec['pre_bet_tax']=0;
                $rec['tickets_payed_win_tax']=0;
                $rec['total_win_tax']=0;
            }*/
            if($userType=='0' || ($userType=='1' && in_array($rec['pun_id'],$connected)) || ($userType=='2' && (!in_array($rec['pun_id'],$connected)))){
                if($rec["currency"]!=$_SESSION["currency_id"]){
                    if($_SESSION["currency_id"]==1){
                        $rec["total_bet"]=bcdiv($rec["total_bet"],$_SESSION["currencies"][$rec["currency"]]["value"],3);
                        $rec["total_win"]=bcdiv($rec["total_win"],$_SESSION["currencies"][$rec["currency"]]["value"],3);
                        $rec["tickets_payed_win"]=bcdiv($rec["tickets_payed_win"],$_SESSION["currencies"][$rec["currency"]]["value"],3);
                        $rec["total_win_tax"]=bcdiv($rec["total_win_tax"],$_SESSION["currencies"][$rec["currency"]]["value"],3);
                        $rec["tickets_payed_win_tax"]=bcdiv($rec["tickets_payed_win_tax"],$_SESSION["currencies"][$rec["currency"]]["value"],3);
                        if($rec['pre_bet_tax'] > 0){
                            $rec["pre_bet_tax"]=bcdiv($rec["pre_bet_tax"],$_SESSION["currencies"][$rec["currency"]]["value"],3);
                        }
                    }
                    else{
                        $rec["total_bet"]=bcmul($rec["total_bet"],$_SESSION["currencies"][$_SESSION["currency_id"]]["value"],3);
                        $rec["total_win"]=bcmul($rec["total_win"],$_SESSION["currencies"][$_SESSION["currency_id"]]["value"],3);
                        $rec["tickets_payed_win"]=bcmul($rec["tickets_payed_win"],$_SESSION["currencies"][$_SESSION["currency_id"]]["value"],3);
                        $rec["total_win_tax"]=bcmul($rec["total_win_tax"],$_SESSION["currencies"][$_SESSION["currency_id"]]["value"],3);
                        $rec["tickets_payed_win_tax"]=bcmul($rec["tickets_payed_win_tax"],$_SESSION["currencies"][$_SESSION["currency_id"]]["value"],3);
                        if($rec['pre_bet_tax'] > 0){
                            $rec["pre_bet_tax"]=bcmul($rec["pre_bet_tax"],$_SESSION["currencies"][$_SESSION["currency_id"]]["value"],3);
                        }
                    }
                }


                $totalbet+=$rec['total_bet'];
                $totalwin+=$rec['total_win'];
                $ticketspaidwin+=$rec['tickets_payed_win'];
                $totalWinTax+=$rec['total_win_tax'];
                $totalWinPaidTax+=$rec["tickets_payed_win_tax"];
                if($rec['pre_bet_tax'] > 0){
                    $totalbetTax+=($rec['pre_bet_tax']);
                }



                $ticketsplayed+=$rec['tickets_played'];
                $ticketswin+=$rec['tickets_won'];
                $ticketspaid+=$rec['tickets_payed'];
                $ticketserased+=$rec['tickets_erased'];
                $totalspecialtickets+=$rec['tickets_specials'];

                $return.="<tr>";
                if($group!=3){
                $return .="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>".($rec['nation']).(($rec["currency"]!=$_SESSION["currency_id"] && $rec["currency"]!="") ? "<br><span class=changeCurr style=\"font-weight:bold;color:red;cursor:pointer;\" data-curr=".$rec["currency"].">".$rec["cur_code_for_web"]."</span>" : '' )."</td>";
                $return .="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>".($rec['region'])."</td>";
                $return .="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>".($rec["district"])."</td>";
                $return .="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>".($rec['club'])."</td>";
                }
                if($group=='1'){
                    $return .="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>".getClassLink("javascript:openWindow('".refPage("customers/customer_view&customer_id={$rec["pun_id"]}")."','user{$rec["pun_username"]}','950','980','0','1')", $cashier['cashier'], "contentlink")."</td>";
                    $return .="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>".getClassLink("javascript:openWindow('".refPage("customers/customer_view&customer_id={$rec["pun_id"]}")."','user{$rec["pun_username"]}','950','980','0','1')", $cashier['game'], "contentlink")."</td>";
                }
                if($group==3){
                    $return .="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>".getClassLink("javascript:openWindow('".refPage("customers/customer_view&customer_id={$rec["pun_id"]}")."','user{$rec["pun_username"]}','950','980','0','1')", $cashier['game'], "contentlink").(($rec["currency"]!=$_SESSION["currency_id"] && $rec["currency"]!="") ? "<br><span class=changeCurr style=\"font-weight:bold;color:red;cursor:pointer;\" data-curr=".$rec["currency"].">".$rec["cur_code_for_web"]."</span>" : '' )." </td>";
                }
                $return .="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>".getInDollars($rec["total_bet"],2)."</td>";
                if(TAX_CALCULATION_ACTIVE=='ON'){
                    $return .="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>".($rec['pre_bet_tax'] !='' ? getInDollars(($rec['total_bet']- $rec['pre_bet_tax']),3)."<br />
                    <span class='tip'>Tax : ".getInDollars($rec['pre_bet_tax'],3)."</span>" : '--' )."</td>";
                }

                $return .="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>".getInDollars($rec["total_win"]-$rec['total_win_tax'],2)."
                <br /><span class='tip'>Tax:".getInDollars($rec['total_win_tax'])."</span>
                </td>";
                $return .="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>".getInDollars($rec["total_bet"]-$rec['total_win'],2)."</td>";
                $return .="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>".$rec["tickets_played"]."<br /><span class='tip'>special:".$rec['tickets_specials']."</span></td>";
                $return .="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>".$rec["tickets_won"]."</td>";
                if($group!=3){
                $return .="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>";

                if($group=='0'){
                    $return.="<button class='btn btn-small' onclick=getConnectedUsers(this,'{$rec['club_id']}')>".$lang->getLang("Show connected")."</button>";
                }else{
                    if(in_array($rec['pun_id'],$connected)){
                        $return.="<span class='text-success'>".$lang->getLang("Connected")."</span>";
                    }
                    else{
                        $return.="<span class='text-error'>".$lang->getLang("Not Connected")."</span>";
                    }
                }
                $return .="</td>";
                $return .="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>".$rec["pun_last_logged_in"]."</td>";
                $return .="<td style='white-space: nowrap;vertical-align:top;text-align:center;'>".($rec['cashier_ip']!=''? getClassLink("javascript:openWindow('".refPage('customers/search&field=4&search_string=' . urlencode(ereg_replace("[^0-9.]", "", $rec["cashier_ip"]))) ."','ip','950','980','0','1')", $rec["cashier_ip"], "contentlink"):'--')."</td>";
                }
                 $return.="</tr>";
            }
        }
        $return.="</table>";
        $allresult .="<tr>
                    <td>".getInDollars($totalbet,2,$_SESSION['currency_html'])."</td> ";
        if(TAX_CALCULATION_ACTIVE=='ON'){
            $allresult .=" <td>".getInDollars($totalbet-$totalbetTax)."<br /><span class='tip'>Tax : ".getInDollars($totalbetTax)."</span></td>";
        }
        $allresult .="
                    <td>".getInDollars($totalwin-$totalWinTax)."
                    <br /><span class='tip'>Tax:".getInDollars($totalWinTax)."</span>
                    </td>
                     <td>".getInDollars($totalbet-$totalwin)."</td>
                    <td>".$ticketsplayed."<br /><span class='tip'>special:".$totalspecialtickets."</span></td>
                    <td>".$ticketswin."</td>";
        if($group==2){
            $allresult.="<td>".$ticketspaid."</td>
                        <td>".getInDollars($ticketspaidwin-$totalWinPaidTax)."<br /><span class='tip'>Tax:".$totalWinPaidTax."</span></td>
                        <td>".$ticketserased."</td>";
        }
        $allresult.="</tr></table><br />";
    }
    else{
        $return="<div class='alert alert-error'><button type='button' class='close' data-dismiss='alert'>&times;</button><strong>".$lang->getLang('No result found')."</strong></div>";
        if($group==2){
            $allresult=$return;
        }
    }
    $returnStr=array();

    if($group!=2){
        $returnStr[0] = $time;
        $returnStr[1]= $allresult;
        $returnStr[2] = $return;
    }
    else{
        $returnStr[0] = $time;
        $returnStr[1]= $allresult;
        $returnStr[2] = '';
    }


    return json_encode($returnStr);
}

/**
 * @param $club
 * @return string
 */
function showUsersConnectedByClub($club){
    global $dbh,$lang;
    $users=getConectedUsers(true);
 /*   $users=explode('~',$users);
    $connected=array();
    foreach($users as $k=>$v){
        list($pl,$val) = explode(":", $v );
        list($prefix,$userid)=explode('_',$pl);
        array_push($connected,$userid);
    }*/

    $connected=array();
    foreach ($users->Records as $k=>$v){
        array_push($connected,$v['usl_pun_id']);
    }
    $sql='SELECT pun_id  from punter JOIN virtual_games_user on vgu_pun_id=pun_id where pun_betting_club='.$dbh->escape($club);
    $rs=$dbh->exec($sql);
    $thisclubplayers=$rs->getNumRows();
    $thisclubplayersconnected=0;
    while($rs->hasNext()){
        $row=$rs->next();
        if(in_array($row['pun_id'],$connected)){
            $thisclubplayersconnected++;
        }
    }
    if($thisclubplayers ==$thisclubplayersconnected){
        $append="<svg height='15' width='15'><circle cx='9' cy='9' r='6'  fill='green' /></svg>";
        $return=$append."  ".number_format($thisclubplayersconnected*100/$thisclubplayers,2)."% (".$thisclubplayersconnected."/".$thisclubplayers." )  ";
    }
    elseif($thisclubplayersconnected==0){
        $append="<svg height='15' width='15'><circle cx='9' cy='9' r='6'  fill='red' /></svg>";
        $return=$append."  ".number_format($thisclubplayersconnected*100/$thisclubplayers,2)."% (".$thisclubplayersconnected."/".$thisclubplayers." )  ";
    }
    else{
        $append="<svg height='15' width='15'><circle cx='9' cy='9' r='6'  fill='orange' /></svg>";
        $return=$append."  ".number_format($thisclubplayersconnected*100/$thisclubplayers,2)."% (".$thisclubplayersconnected."/".$thisclubplayers." )  ";
    }
    return $return;
}

/**
 * @param $childArray
 */
function showChild($childArray,$onlyChild=false,$agencyCode=false,$class=false){

    foreach($childArray as $key => $value){
        if(!$class) {
            if ($value['class'] == 'casino') {
                $class_type = "casino";
            } elseif ($value['class'] == 'nation') {
                $class_type = "nation";
            } elseif ($value['class'] == 'region') {
                $class_type = "region";
            } elseif ($value['class'] == 'district') {
                $class_type = "district";
            } elseif ($value['class'] == 'club') {
                $class_type = "club";
            }
        }
        if(!is_null($value['child'])){
            if(!$onlyChild ||($onlyChild && strpos(getAllSubJurisdictions($_SESSION['jurisdiction_class']), strtolower($class_type))!==false )){ ?>
            <li data-jstree='{ "type" : "<?=$class_type?>" }' class="jstree-closed <?=$class_type?>" id="<?=$value['id']?>"><?=(($agencyCode && $class_type!='casino')? "[".$value['code']."] " :'' )?><?=strtoupper($value['name'])?><ul><?php
            }
            showChild($value['child'],$onlyChild,$agencyCode);

            if(!$onlyChild ||($onlyChild && strpos(getAllSubJurisdictions($_SESSION['jurisdiction_class']), strtolower($class_type))!==false )){ ?>
            </ul></li><?php }
        }else{
            if(!$onlyChild ||($onlyChild && strpos(getAllSubJurisdictions($_SESSION['jurisdiction_class']), strtolower($class_type))!==false ) || $class_type=='club'){ ?>
           <li data-jstree='{ "type" : "<?=$class_type?>" }' class="<?=$class_type?>" id="<?=$value['id']?>"><?=($agencyCode? "[".$value['code']."] " :'' )?><?=strtoupper($value['name'])?></li><?php }
        }
    }
}

/**
 * @param $jur_id
 */
function getAgencyGames($jur_id)
{
    global $dbh,$lang;
    include_once 'StringTokenizer.class.php';
    $thisGames=getAgencyLicense($jur_id);
    $games=split(";", $thisGames);
    $thisConstant=$games['0'];
    $gamesinfo=$games['1'];
    $gamess=array();
    $results = new StringTokenizer ( $gamesinfo, "~" );
    while ( $results->hasMoreTokens () ) {
        list($gameid,$gamename)=explode("&", $results->nextToken ());
        $gamess[$gameid] = $gamename;
    }

    $sql = "SELECT gam_id, gam_name FROM game WHERE gam_type = 'agency' ";

    if($jur_id == 300){
        $sql .= " AND gam_name LIKE '[Adjara]%' ";
    }else{
        $sql .= " AND gam_name NOT LIKE '[Adjara]%' ";
    }

    $result=$dbh->exec($sql);
    if(!$result->hasNext())
    {
        ?>
        <span class="tip"><?=$lang->getLang("No result found")?></span>
        <?
        return;
    }
    else
    {
        ?>
        <div id="checkboxes">
            <?
            while($result->hasNext())
            {
                $row = $result->next();
                ?>
                <label class="checkbox inline" >
                    <input type="checkbox" class="hidden" value="<?=$row['gam_id']?>&<?=$row['gam_name']?>"<? foreach ($gamess as $k=>$v){ if($row['gam_id']==$k){?> checked="checked"<?}}?> name="<?=$row['gam_id']?>&<?=$row['gam_name']?>">
                    <img class="virtualImages <?if(array_key_exists($row['gam_id'],$gamess)){?> virtualNotSelected virtualSelected <? }?>"  src="media/images/<?=$row['gam_id']?>.png"><br /><br /><b>  <?=$row['gam_name']?></b> <br />
                    <?if(array_key_exists($row['gam_id'],$gamess)){?> <span class="text-success">(<?=$lang->getLang("Currently Activated")?>)</span> <? }else {?><span class="text-error">(<?=$lang->getLang("Currently Not Activated")?>)</span>  <?}?>
                </label>
            <?
            }
            ?>
        </div><br />

        <button id="ConfirmGames" class="btn btn-primary"><?=$lang->getLang("Confirm")?>
            <script>
                $(function() {

                    $(':checkbox').change(function() {
                        $(this).next().toggleClass('virtualSelected');
                    });

                    $( '#ConfirmGames')
                        .button()
                        .click(function() {
                            var selected = [];
                            $('#checkboxes input:checked').each(function() {
                                selected.push($(this).attr('value'));
                            });
                            var ajaxTime= new Date().getTime();
                            $.post("totem/totem_query_service.inc",{action:'10',license:';'+selected, jurid:<?=$jur_id?>}, function(data){
                                jAlert("<?=$lang->getLang("Virtual Updated")?>");
                                $.post("/virtual/virtual_functions.inc",{action:"3",jurid:<?=$jur_id?>}, function(data){
                                    var totalTime = new Date().getTime()-ajaxTime;
                                    $("#virtualgames").empty().append("<div class='tip fleft'><?=$lang->getLang("Time taken to execute your request")?>:"+ (totalTime/1000).toFixed(4) +" s</div><br /><br /><br />"+ data );
                                });
                            });
                        });
                    return false;
                });
            </script>
        </button>
    <?
    }
}

/**
 * @param $jur_id
 * @return mixed
 */
function getAgencyLicense($jur_id)
{
    global $dbh,$lang;
    $sql="SELECT vgs_license_code
		FROM  `virtual_games`
		JOIN jurisdiction ON jur_code = vgs_jur_code
		WHERE jur_id = '$jur_id'";
    $games=$dbh->queryOne($sql);
    return $games;
}


function getVirtualUsers($jurisdiction,$html=false,$allSelected=false){
    global $dbh,$lang;
    if(is_array($jurisdiction)){
        $jurisdiction=implode(',',$jurisdiction);
    }
    $sql="SELECT vgu_jur_code,vgu_operator_id,vgu_pun_id,pcr_credits,gam_name,club.jur_name,cur_code_for_web
          FROM virtual_games_user
          JOIN punter_credit on vgu_pun_id=pcr_pun_id
          JOIN game on vgu_gam_id=gam_id
          JOIN jurisdiction club on club.jur_code=vgu_jur_code
          JOIN currency on club.jur_currency=cur_id
           ";
    if($_SESSION["jurisdiction_class"] != "casino"){
        $sql .= "
  		       JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
             JOIN jurisdiction regional ON regional.jur_id = district.jur_parent_id
             JOIN jurisdiction nation   ON nation.jur_id   = regional.jur_parent_id";
    }
    switch($_SESSION["jurisdiction_class"]){
        case "club":
            $sql .= " WHERE club.jur_id = ".$_SESSION['jurisdiction_id'];
            break;
        case "district":
            $sql .= " WHERE district.jur_id = ".$_SESSION['jurisdiction_id'];
            break;
        case "region":
            $sql .= " WHERE regional.jur_id = ".$_SESSION['jurisdiction_id'];
            break;
        case "nation":
            $sql .= " WHERE nation.jur_id = ".$_SESSION['jurisdiction_id'];
            break;
        case "casino":
            $sql .= " WHERE 1=1";
            break;
        default:
            $sql .= " WHERE 1=0";
            break;
    }

    if(!$allSelected || $allSelected=='false'){
        $sql.=" AND club.jur_id in ($jurisdiction) ";
    }

    $users=$dbh->exec($sql);
    $return=$users;
    if($html){
        if($users->getNumRows()>0){
            $return= "
             <table class='display table table-bordered table-striped table-hover' cellpadding='1' cellspacing='0' border='1' width='100%'>
                                       <thead>
                                       <tr>
                                       <th>".$lang->getLang("Agency")."</th>
                                       <th>".$lang->getLang("Game")."</th>
                                       <th>".$lang->getLang("Agency code")."</th>
                                       <th>".$lang->getLang("Operator")."</th>
                                       <th>".$lang->getLang("Available Credit")."</th>
                                       <th>".$lang->getLang("Action")."</th>
                                       </tr>
                                       </thead>
                                       <tbody> ";
            while($users->hasNext()){
                $rec=$users->next();
                $return.="<tr>
                            <td>".$rec['jur_name']."</td>
                            <td>".$rec['gam_name']."</td>
                            <td>".$rec['vgu_jur_code']."</td>
                            <td>".$rec['vgu_operator_id']."</td>
                            <td>".getInDollars($rec['pcr_credits'],2,$rec['cur_code_for_web'])."</td>
                             <td style='width:150px'>
                             <input type='hidden' name=customer value=".$rec['vgu_pun_id'].">
                             <button class='btn btn-small btn-primary tableClass' data-toggle='popover' data-placement='top' data-html='true'
                               data-content='".$lang->getLang('Amount').": <input type=\"number\" min=\"0\" step=\"0.01\" ><button class=\"btn btn-info btn-small confirmdeposit\">".$lang->getLang('Confirm')."</button> <button class=\"btn btn-small cancel\">".$lang->getLang('Cancel')."</button><br /><div class=\"depositResultDiv\"></div>' title='' data-original-title='".$lang->getLang('Deposit')."'>".$lang->getLang('Deposit')."
                             </button>
                             <button class='btn btn-small btn-danger tableClass' data-toggle='popover' data-placement='left' data-html='true'
                               data-content='".$lang->getLang('Amount').": <input type=\"number\" min=\"0\" step=\"0.01\" ><button class=\"btn btn-info btn-small confirmwithdraw\">".$lang->getLang('Confirm')."</button> <button class=\"btn btn-small cancel\">".$lang->getLang('Cancel')."</button><br /><div class=\"depositResultDiv\"></div>' title='' data-original-title='".$lang->getLang('Withdraw')."'>".$lang->getLang('Withdraw')."
                             </button>
                             </td>
                           </tr>";
            }
            $return.="</tbody></table>";

        }
        else{
            $return="<div class='alert alert-error'><button type='button' class='close' data-dismiss='alert'>&times;</button><strong>".$lang->getLang('No result found')."</strong></div>";
        }
    }
    return $return;
}


function checkFields($club_id, $amount, $payment_method, $password,$type,$customer_id) {
    global $dbh,$lang;
    if ( isBlank($amount) || $amount <= 0)
        addError("", "An amount must be entered");
    elseif ( ! isValidMoney($amount) )
        addError("", "Invalid amount entered - must be a ".((isset($_SESSION['currency_html']) && $_SESSION['currency_html']!='') ? $_SESSION['currency_html'] : "&euro;" )." amount (cents optional)");

    if ( isBlank($payment_method) )
        addError("", "A payment method must be selected");
    elseif ( !('cashier cheque' == $payment_method || 'cashier cash' == $payment_method || 'cashier credit card' == $payment_method || 'cashier bonus' == $payment_method || 'wmc' == $payment_method) )
    {
        addError("", "Invalid payment method");
    }

    if($type=='deposit'){
        $sql =  "SELECT jur_available_credit,jur_currency" .
            "   FROM  jurisdiction " .
            "   WHERE jur_id = " . $club_id;
        //echo $sql;
        $rs       = $dbh->exec($sql);
        $num_rows = $rs->getNumRows();



        if ( $num_rows > 0 ) {
            $row = $rs->next();
            $available_credit = $row['jur_available_credit'];


            if ( $available_credit - $amount < 0 && $_SESSION['jurisdiction_class']!='casino' ){
                $max_deposit = getInDollars($available_credit);
                addError("", $lang->getLang("Cannot create customer - cannot process deposit. Your club does not have enough available credit. Maximum deposit allowed is %",$max_deposit));
                return false;
            }
        }
        else {
            addError("", $lang->getLang("Critical error: Missing account!"));
        }
        $sql="Select pun_daily_deposits_limit,jur_currency from punter join jurisdiction on jur_id=pun_betting_club where pun_id = ".$dbh->escape($customer_id);
        $limits=$dbh->queryRow($sql);


        if($limits['jur_currency']!=1){
            $limits['pun_daily_deposits_limit']=($limits['pun_daily_deposits_limit']*$_SESSION['currencies'][$limits['jur_currency']]['value']*4);
        }
        if (  $amount > $limits['pun_daily_deposits_limit']) {
            addError("", $lang->getLang("Maximum deposit allowed is %",  $limits['pun_daily_deposits_limit']));
        }
    }
    return ! areErrors();
}


function doDepositWithdraw($customer_id,$amount,$type,$password=''){
    global $dbh,$lang;
    $amount=$dbh->escape($amount);
    $type=$dbh->escape($type);
    include_once 'JurisdictionsList.class.inc';
    $thisJurClass = JurisdictionsList::getInstance ( $dbh );
    if ( $customer_id ) {
        $customer_id = (int)$customer_id;
        $customer_row = getCustomer($customer_id);
        if ( !$customer_row ) {
            return $lang->getLang("Invalid customer ID");
        }
    }
    else{
        return $lang->getLang("Missing customer ID");
    }

    if ( $customer_row['betting_club'] && !in_array($customer_row['betting_club'], $thisJurClass->getChilJurisdictionsIdList($_SESSION['jurisdiction_id']))) {
        $club_name = getClubName($customer_row['betting_club']);
        return $lang->getLang("Transactions cannot be performed for this customer at this club because the customer's account is held at another club (%)",$club_name);
    }
    if ( checkFields($_SESSION['jurisdiction_id'], $amount, 'cashier cash', $password,$type,$customer_id ) ) {
        $dbh->begin();
        if($type=='withdraw' &&  $customer_row['credits'] < $amount ){
            $balance = getInDollars($customer_row['available_balance']);
            $dbh->rollback();
            return "Insufficient funds ($balance) for withdrawal";
        }
        else{
            $jurisdictionsArray=array();
            $alljurisdictions=getAllEntitiesBetween($_SESSION['jurisdiction_class'],'club',false,$customer_id);
            $trnsId= 'ADM'.$dbh->nextSequence('CTR_TRAN_NUM_SEQ').rand_str(4);
            if ( 'deposit' == $type ){
                $transfer_type=25;
            }
            else{
                $transfer_type=26;
                $alljurisdictions=array_reverse($alljurisdictions);
            }
            $keys = array_keys($alljurisdictions);
            if($type=='withdraw'){
                if(!is_int(adminCreditLog($_POST['note'],$transfer_type,$_SESSION['admin_id'],$customer_row['betting_club'],false,$customer_id, -$amount,1,$trnsId))){
                    $dbh->rollback();
                    return  'An error has occurred when adding the withdraw transaction record';
                }
            }

            for ($i = 0; $i < count($keys); $i++) {

                $cur = $alljurisdictions[$keys[$i]];
                $next = $alljurisdictions[$keys[$i+1]];

                if($next!=''){
                    if ( 'deposit' == $type )
                    {
                        if(!is_int(adminCreditLog($_POST['note'],$transfer_type,$_SESSION['admin_id'],$cur,$next,false,$amount,$i,$trnsId))){
                            $dbh->rollback();
                            return 'An error has occurred when adding the deposit transaction record';
                        };
                    }
                    else
                    {
                        if(!is_int(adminCreditLog($_POST['note'],$transfer_type,$_SESSION['admin_id'],$next,$cur,false,-$amount,$i,$trnsId))){
                            $dbh->rollback();
                            return 'An error has occurred when adding the withdraw transaction record';
                        };
                    }
                }
            }
            if($type=='deposit'){
                if(!is_int(adminCreditLog($_POST['note'],$transfer_type,$_SESSION['admin_id'],$customer_row['betting_club'],false,$customer_id, $amount,1,$trnsId))){
                    $dbh->rollback();
                    return 'An error has occurred when adding the transaction record of the deposit';
                }
            }
            if($type=='withdraw' ){
                $transaction_number = do_cash_withdrawal ($amount, $customer_id, $_SESSION['admin_id'], $_SESSION['jurisdiction_id'],$trnsId,26);
            }
            else{
                require_once("queue_func.php");
                if(queue_begin("TDPL-$customer_id", 120)){
                    $transaction_number = do_cash_deposit ($amount, $customer_id, $_SESSION['admin_id'], $_SESSION['jurisdiction_id'],$trnsId,25);
                }else{
                    $dbh->rollback();
                    return($lang->getLang("Another transaction for this customer is running, wait a few for a new transaction, thanks."));
                }
            }
            if ( $transaction_number ){
                if($type=='deposit'){
                return "1 ".getInDollars($customer_row['credits']+$amount);
                }
                else{
                    return "1 ".getInDollars($customer_row['credits']-$amount);
                }
            }else{
                return $lang->getLang("Database Error");
            }
        }
    }
    else{
        return showErrors();
    }
}


function getAgenciesStats($jurisdiction,$allSelected,$_period,$_datefrom,$_inactivityDays=31){
    global $dbh,$lang;

        if(is_array($jurisdiction)){
        $jurisdiction=implode(',',$jurisdiction);
    }
    $date='2014-01-01';
    if($_period=='1w'){
        $date=date("Y-m-d", strtotime("-1 week"));
    }
    elseif($_period=='2w'){
        $date=date("Y-m-d", strtotime("-2 weeks"));
    }
    elseif($_period=='1m'){
        $date=date("Y-m-d", strtotime("-1 month"));
    }
    elseif($_period=='2m'){
        $date=date("Y-m-d", strtotime("-2 months"));
    }
    elseif($_period=='3m'){
        $date=date("Y-m-d", strtotime("-2 months"));
    }
    elseif($_period=='6m'){
        $date=date("Y-m-d", strtotime("-6 months"));
    }
    else{
        $date=$_datefrom;
    }
    if($_inactivityDays==''){
        $now = time();
        $_inactivityDays=floor($now-strtotime($date)/(3600*24));
    }

    $sql="SELECT club.jur_id,
                 club.jur_code,
                 club.jur_name,
                 sum(coalesce(vds_tickets,0)) vds_tickets,
                 sum(coalesce(vds_bet,0)) vds_bet,
                 (select count(*) from virtual_games_user where vgu_jur_code=vgs_jur_code) nr_cashier,
                 coalesce(datediff(now(), max(vds_date)), datediff(now(),  '$date')) inactivity_days,
                 coalesce(max(vds_date), '$date') last_played,
                 cur_code_for_web
            FROM virtual_games
            JOIN jurisdiction club on club.jur_code=vgs_jur_code AND jur_class='club'
            LEFT JOIN currency on cur_id=club.jur_currency
            LEFT JOIN virtual_daily_summary on club.jur_id=vds_club_id AND vds_date > '$date'";
    if($_SESSION["jurisdiction_class"] != "casino"){
        $sql .= "
  		     JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
             JOIN jurisdiction regional ON regional.jur_id = district.jur_parent_id
             JOIN jurisdiction nation   ON nation.jur_id   = regional.jur_parent_id";
    }
    switch($_SESSION["jurisdiction_class"]){
        case "club":
            $sql .= " WHERE club.jur_id = ".$_SESSION['jurisdiction_id'];
            break;
        case "district":
            $sql .= " WHERE district.jur_id = ".$_SESSION['jurisdiction_id'];
            break;
        case "region":
            $sql .= " WHERE regional.jur_id = ".$_SESSION['jurisdiction_id'];
            break;
        case "nation":
            $sql .= " WHERE nation.jur_id = ".$_SESSION['jurisdiction_id'];
            break;
        case "casino":
            $sql .= " WHERE 1=1";
            break;
        default:
            $sql .= " WHERE 1=0";
    }

    if(!$allSelected || $allSelected=='false'){
        $sql.=" AND jur_id in ($jurisdiction) ";
    }
    $sql.=" GROUP by club.jur_id";
    $starttime = microtime(true);
    $rs=$dbh->doCachedQuery($sql,3600);
    $endtime = microtime(true);
    $duration = $endtime - $starttime;
    $duration=number_format($duration, 4, ',', '')."s";
    $activeTotal=0;
    $totalCashier=0;
    $inactiveTotal=0;
    $totalAgencies=0;
    $totalTicketNr=0;
    $totalTicketAmount=0;
    $time="<div class='tip fleft'>".$lang->getLang("Time taken to execute your request").":".$duration."</div><br /><br /><br />";
    if($rs->getNumRows()>0){
        $return="<table class='display displayStats table table-striped table-bordered table-condensed'>
                                <thead>
                                    <tr>
                                        <th>".$lang->getLang("Agency")."</th>
                                        <th>".$lang->getLang("Nr of cashier")."</th>
                                        <th>".$lang->getLang("Status")."</th>
                                        <th>".$lang->getLang("Inactivity days")."</th>
                                        <th>".$lang->getLang("Average tickets nr")."</th>
                                        <th>".$lang->getLang("Average ticket amount")."</th>
                                    </tr>
                                </thead>";
        $allreturn="<table class='display displayStats table table-striped table-bordered table-condensed'>
                                <thead>
                                    <tr>
                                        <th>".$lang->getLang("Nr of agencies")."</th>
                                        <th>".$lang->getLang("Nr of cashier")."</th>
                                        <th>".$lang->getLang("Total Active")."</th>
                                        <th>".$lang->getLang("Total Inactive")."</th>
                                        <th>".$lang->getLang("Average tickets nr")."</th>
                                        <th>".$lang->getLang("Average ticket amount")."</th>
                                    </tr>
                                </thead>";
        while($rs->hasNext()){
            $row  = $rs->next();

            (($row['inactivity_days'] < $_inactivityDays)&&($date<$row['last_played']))?  $activeTotal++ :  $inactiveTotal++;
            $totalAgencies++;
            $totalCashier+=$row['nr_cashier'];
            $totalTicketNr+=$row['vds_tickets'];
            $totalTicketAmount+=$row["vds_bet"];

            $return.="<tr>
                        <td>[".$row['jur_code']."] ".$row['jur_name']."</td>".
                        "<td style='width:100px'>".$row['nr_cashier']."</td>".
                        "<td>".((($row['inactivity_days'] < $_inactivityDays)&&($date<$row['last_played'])) ? '<span class="text-success">'.$lang->getLang("Active").'</span>': "<span class='error'>".$lang->getLang('Inactive')."</span>" )."</td>".
                        "<td>".$row['inactivity_days'].' days <br><span class="tip">'.($row['inactivity_days'] > $_inactivityDays? 'Last played before ': "Last played on " ).$row['last_played']."</span></td>".
                        "<td style='width:150px'>".($row['vds_tickets']!=0?number_format($row['vds_tickets']/$row['nr_cashier'], 2, ',', '') : 0)."</td>".
                        "<td style='width:180px'>".getInDollars($row["vds_bet"]!=0 ? $row["vds_bet"]/$row['vds_tickets'] : 0 ,2,$row["cur_code_for_web"])."<br />(<span class='tip'>".getInDollars($row["vds_bet"],2,$row["cur_code_for_web"])."</span>)</td>
                      </tr>";
        }
        $return.="</table>";
        $allreturn.="<tr>
                        <td>".$totalAgencies."</td>".
                        "<td style='width:100px'>".$totalCashier."</td>".
                        "<td>".$activeTotal."</td>".
                        "<td>".$inactiveTotal."</td>".
                        "<td style='width:150px'>".number_format($totalTicketNr/$totalCashier, 2, ',', '')."</td>".
                        "<td style='width:180px'>".getInDollars($totalTicketAmount/$totalCashier)."</td>
                      </tr></table>";
    }
    else
    {
        $return="<div class='alert alert-error'><button type='button' class='close' data-dismiss='alert'>&times;</button><strong>".$lang->getLang('No result found')."</strong></div>";
    }
    $return = $time.$allreturn.$return;
    return $return;
}


function getVirtualHierarchy(){
    $class=getSubJurisdiction($_SESSION['jurisdiction_class']);
    $jurisdiction=$_SESSION['jurisdiction_id'];
    $nodes=getVirtualJurisdiction($jurisdiction,$class);
    if($nodes->getNumRows()>0){
        $jurisdictions=array();
        while ( $nodes->hasNext () ) {
            $row = $nodes->next ();
            if(!is_array($jurisdictions[$row['nation_id']])){
                $jurisdictions[$row['nation_id']]['id']=$row['nation_id'];
                $jurisdictions[$row['nation_id']]['class']='nation';
                $jurisdictions[$row['nation_id']]['name']=$row['nation'];
                $jurisdictions[$row['nation_id']]['code']=strtoupper($row['nation_code']);
                $jurisdictions[$row['nation_id']]['child']=array();
            }

            if(!is_array($jurisdictions[$row['nation_id']]['child'][$row['region_id']])){
                $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['id']=$row['region_id'];
                $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['code']=strtoupper($row['region_code']);
                $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['class']='region';
                $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['name']=$row['region'];
                $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child']=array();
            }

            if(!is_array($jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']])){
                $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['id']=$row['district_id'];
                $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['code']=strtoupper($row['district_code']);
                $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['class']='district';
                $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['name']=$row['district'];
                $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['child']=array();
            }

            $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['child'][$row['club_id']]['id']=$row['club_id'];
            $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['child'][$row['club_id']]['code']=strtoupper($row['club_code']);
            $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['child'][$row['club_id']]['name']=$row['club'];
            $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['child'][$row['club_id']]['class']='club';

        }
        $return=$jurisdictions;

    }else{
        $return ="<div class='alert alert-error'><strong>-1</strong></div>";
    }
    return ($return);
}



function getAgencyDetails($clubId){
    global $dbh;
    $sql="SELECT n.jur_name nation, n.jur_id nation_id,n.jur_code nation_code,
        r.jur_name region, r.jur_id region_id, r.jur_code region_code,
        d.jur_name district, d.jur_id district_id,d.jur_code district_code,
        c.jur_name club, c.jur_id club_id, vgs_jur_code,c.jur_code club_code,c.jur_address1 club_address
        FROM  jurisdiction n
        JOIN jurisdiction r ON r.jur_parent_id = n.jur_id
        JOIN jurisdiction d ON d.jur_parent_id = r.jur_id
        JOIN  jurisdiction c ON c.jur_parent_id = d.jur_id
        JOIN virtual_games ON vgs_jur_code = c.jur_code AND c.jur_class = 'club'
        WHERE c.jur_id=".$dbh->escape($clubId);

    return $dbh->queryRow($sql);
}


function getAgencyClosure($agency,$day){
    global $dbh,$lang;
    if(is_array($agency)){
        $agency=implode(',',$agency);
    }
    $sql ="SELECT virtual_cashier_closure.*,
                 sum(if (vcl_gam_id < 1000 , vcl_total_bets, 0)) as totalsport,
                 sum(if (vcl_gam_id < 1000 , vcl_total_wins, 0)) as totalsportpaid,
                 sum(if (vcl_gam_id >= 1000 AND vcl_gam_id<>9026, vcl_total_bets, 0)) as totalnonsport,
                 sum(if (vcl_gam_id >= 1000 AND vcl_gam_id<>9026, vcl_total_wins, 0)) as totalnonsportpaid,
                 sum(if (vcl_gam_id = 9026, vcl_total_bets, 0)) as totalnumere,
                 sum(if (vcl_gam_id = 9026, vcl_total_wins, 0)) as totalnumerepaid
    from virtual_cashier_closure
    join jurisdiction on jur_code=vcl_jur_code
    where jur_id = $agency
    and vcl_starting between '".$dbh->escape($day)." 00:00:00' and '".$dbh->escape($day)." 23:59:59' and jur_class='club' group by vcl_jur_code";
    $agencyInfo=$dbh->queryRow($sql);

    list($cashierid,$company,$zRaport,$otherInfo)=explode('###',$agencyInfo['vcl_info']);
    $cashierid=explode(':',$cashierid);
    $company=explode(':',$company);
    list($zRaportSport,$zRaportNonsport,$zRaportNumere)=explode('~',$zRaport);
    list($zRaportSport,$zRaportSportPaid)=explode(";",$zRaportSport);
    $zRaportSport=explode(":",$zRaportSport);

    list($zRaportNonsport,$zRaportNonsportPaid)=explode(";",$zRaportNonsport);
    $zRaportNonsport=explode(":",$zRaportNonsport);

    list($zRaportNumere,$zRaportNumerePaid)=explode(";",$zRaportNumere);
    $zRaportNumere=explode(":",$zRaportNumere);

    $otherInfo=explode('~',$otherInfo);
    $details=array();
    foreach($otherInfo as $k=>$v){
        $v=explode(":",$v);
        $details[$v[0]]=$v[1];
    }

   if($details['plati']=='NONE'){
       $html='';
   }
    else {

        $html = "


      <button id='printButton' class='btn btn-success'>Print</button>
      <div class='clearfix'></div><br />
                    <script>
                    $('#printButton').unbind('click')
                        $('#printButton').bind('click',function() {
                            var prtContent = document.getElementById('printDiv');
                            console.log(prtContent);
                            var WinPrint = window.open('', '', 'left=0,top=0,width=800,height=900,toolbar=0,scrollbars=0,status=0');
                            WinPrint.document.write('<link rel=\"stylesheet\" type=\"text/css\" href=\"" . secure_host . "/media/style.css\" />');
                            WinPrint.document.write('<link rel=\"stylesheet\" type=\"text/css\" href=\"" . secure_host . "/media/bootstrap/css/bootstrap.css\"  />');
                            WinPrint.document.write(' <link rel=\"stylesheet\" type=\"text/css\" href=\"" . secure_host . "/media/table.css\" />');

                WinPrint.document.write(prtContent.innerHTML);
                WinPrint.document.close();
                WinPrint.focus();
                WinPrint.print();
                WinPrint.close();
                });
                </script>";
    }
        $html.="
<div id='printDiv' style='width:50%;background-color:white;padding:20px;border-radius:3px;'>
<div style='width:100%'><strong>".$company[1]."</strong><br />Agency : ".$agencyInfo['vcl_jur_code']."</div>
    <div style='width:100%'>
        <div class='aright fleft half'><br />
                <strong>REGISTRU DE CASA</strong>
        </div>
        <div class='fright' >Cont de casa<br />
            <div style='border:1px solid black' class='centered'>".$cashierid[1]."</div>
         </div>
         <div class='clearfix'></div>
      </div>";

    $html.="<br /><table style='border:2px;width: 50%;text-align:center' class='fright' >
                <tr>
                    <td class='borderless'></td><td class='borderless' style='text-align:center'>Data</td><td class='borderless'></td>
                 </tr>
                 <tr style='border:2px solid black'>
                    <td  style='text-align:center;border-right:1px solid black'>".$lang->getLang("Day")."</td><td  style='text-align:center;border-right:1px solid black'>".$lang->getLang("Month")."<td  style='text-align:center'>".$lang->getLang("Year")."</td>
                 </tr>
                 <tr style='border:2px solid black'>
                    <td  style='text-align:center;border-right:1px solid black'>".date('d',strtotime($day))."</td><td  style='text-align:center;border-right:1px solid black'>".date('m',strtotime($day))."</td><td  style='text-align:center'>".date('Y',strtotime($day))."</td>
                 </tr>
            </table>
            <table style='border:0;width: 100%;' id='virtualClosureTable'>
                <tr style='border:1px solid rgb(221, 221, 221);'>
                    <td  style='border:1px solid rgb(221, 221, 221);'>Nr Crit</td><td  style='border:1px solid rgb(221, 221, 221);'><div style='text-align:center'>Operatiuni</div></td><td style='border:1px solid rgb(221, 221, 221);text-align:center'>Incasari</td><td style='text-align:center'>Plati</td>
                </tr>
                 <tr>
                    <td class='borderless'></td><td class='borderless' style='text-align:center'><strong>Raport ziua precedenta</strong> </td><td  style='text-align:right;border:2px solid black'>";
    if($details['raportzlast']=='NONE'){
        $html.="<input type='text' id='raportz' >";
    }
    else{
        $html.=$details['raportzlast'];
    }
    $html.="</td><td class='borderless'></td>
                </tr>
                <tr style='border-top:2px solid black;'>
                    <td>1</td><td>Raport Z incasari sport</td><td style='text-align:right'>".$zRaportSport[1]."( ".$agencyInfo['totalsport']." )</td><td style='text-align:right;  background-color: rgb(128, 128, 128);' >-</td>
                </tr>
                <tr>
                    <td>2</td><td>Raport Z incasari nonsport</td><td style='text-align:right'>".$zRaportNonsport[1]."( ".$agencyInfo['totalnonsport']." )</td><td  style='text-align:right;  background-color: rgb(128, 128, 128);' >-</td>
                </tr>
                <tr>
                    <td>3</td><td>Raport Z incasari numere</td><td style='text-align:right;'>".$zRaportNumere[1]."( ".$agencyInfo['totalnumere']." )</td><td  style='text-align:right;  background-color: rgb(128, 128, 128);' >-</td>
                </tr>
                <tr>
                    <td>4</td><td  style='background-color:rgb(255, 226, 244)'></td><td style='text-align:right;'>-</td><td  style='text-align:right;  background-color: rgb(128, 128, 128);'>-</td>
                </tr>
                <tr>
                    <td>5</td><td>Plati pariuri sport</td><td  style='text-align:right;  background-color: rgb(128, 128, 128);'>-</td><td style='text-align:right;'>".($zRaportSportPaid>0 || $agencyInfo['totalsportpaid']>0? $zRaportSportPaid."(".$agencyInfo['totalsportpaid'].")" : "-" )."</td>
                </tr>
                <tr>
                    <td>6</td><td>Plati pariuri nonsport</td><td  style='text-align:right;  background-color: rgb(128, 128, 128);'>-</td><td style='text-align:right;'>".($zRaportNonsportPaid>0 || $agencyInfo['totalnonsportpaid']? $zRaportNonsportPaid."(".$agencyInfo['totalnonsportpaid'].")" : "-" )."</td>
                </tr>
                <tr>
                    <td>7</td><td>Plati pariuri numere</td><td  style='text-align:right;  background-color: rgb(128, 128, 128);'>-</td><td style='text-align:right;'>".($zRaportNumerePaid>0 || $agencyInfo['totalnumerepaid']? $zRaportNumerePaid."(".$agencyInfo['totalnumerepaid'].")" : "-" )."</td>
                </tr>
                <tr>
                    <td>8</td><td style='background-color:rgb(255, 226, 244)'></td><td  style='text-align:right;  background-color: rgb(128, 128, 128);'>-</td><td></td>
                </tr>
                <tr>
                    <td>9</td><td>Plati furnizori(facturi, chitante, bonuri fiscale)</td><td style='text-align:right;  background-color: rgb(128, 128, 128);'>-</td><td style='text-align:right;'>";

    if($details['plati']=='NONE'){
        $html.="<input type='text' id='plati' >";
    }
    else{
        $html.=$details['plati'];
    }
    $html.=" </td>
                </tr>
                <tr>
                    <td>10</td><td>Transfer catre AG</td><td  style='text-align:right;  background-color: rgb(128, 128, 128);'></td><td style='text-align:right;'>";
    if($details['tcatreag']=='NONE'){
        $html.="<input type='text' id='tcatre' >";
    }
    else{
        $html.=$details['tcatreag'];
    }
    $html.="</td>
                </tr>
                <tr>
                    <td>11</td><td>Transfer din AG</td><td  style='text-align:right;'>";
    if($details['tdinag']=='NONE'){
        $html.="<input type='text' id='tdin' >";
    }
    else{
        $html.=$details['tdinag'];
    }
    $html.="</td><td  style='text-align:right;  background-color: rgb(128, 128, 128);'></td>
                </tr>
                <tr>
                    <td>12</td><td>Transfer de la sediu</td><td  style='text-align:right;'>";
    if($details['tdelasediu']=='NONE'){
        $html.="<input type='text' id='tdsediu' >";
    }
    else{
        $html.=$details['tdelasediu'];
    }
    $html.="</td><td  style='text-align:right;  background-color: rgb(128, 128, 128);'></td>
                </tr>
                <tr>
                    <td>13</td><td>Diferenta de bani profit</td><td style='text-align:right;'>";

    if($details['diffprofit']=='NONE'){
        $html.="<input type='text' id='tdiffprofit' >";
    }
    else{
        $html.=$details['diffprofit'];
    }
    $html.="</td><td  style='text-align:right;  background-color: rgb(128, 128, 128);'>-</td>
                </tr>
                <tr>
                    <td>14</td><td>Diferenta de bani pierdere</td><td  style='text-align:right;  background-color: rgb(128, 128, 128);'></td><td style='text-align:right;'>";

    if($details['diffpierdere']=='NONE'){
        $html.="<input type='text' id='tdiffpierdere' >";
    }
    else{
        $html.=$details['diffpierdere'];
    }
    $totalZIn=$zRaportSport[1]+ $zRaportNonsport[1]+$zRaportNumere[1]+$details['tdinag']+$details['tdelasediu']+$details['diffprofit'];
    $totalZOut=$zRaportSportPaid+$zRaportNonsportPaid+$zRaportNumerePaid+$details['plati']+$details['tcatreag']+$details['diffpierdere'];
    $html.="</td>
                </tr>

                 <tr style='border-bottom:2px solid black;'>
                    <td></td><td class='centered'>TOTAL ZI</td><td style='text-align:right;'>".($totalZIn)."( ".($agencyInfo['totalsport']+$agencyInfo['totalnonsport']+$agencyInfo['totalnumere'])." )</td><td style='text-align:right;'>".($totalZOut)."(".($agencyInfo['totalsportpaid']+ $agencyInfo['totalnonsportpaid']+$agencyInfo['totalnumerepaid']).")</td></td>
                </tr>
                </table>
                <table  style='border:0;width: 75%;'>
                    <tr><td class='half centered' style='border:1px solid rgb(221, 221, 221);'><div class='bold centered'>SOLD FINAL</div></td><td style='border:2px solid black;text-align:center'>".$agencyInfo['vcl_ending_credit']."</td></tr>
                    </table><br />
                <div class='fleft'>Cashier ".$agencyInfo['vcl_username']."</div><div class='fright'>Compartiment financiar-contabil</div><br /><br />
                <div style='width:100%;border-bottom:1px solid black'>Observatii</div><br />

                <table class='virtualClosureEpilog'>
                <tr><td>Raport Z (casa marcat )</td><td style='min-width:100px'>".($zRaportSport[1]+ $zRaportNonsport[1]+$zRaportNumere[1])."</td></tr>
                <tr><td>Anulate sau pierdute</td><td style='min-width:100px'>";

    if($details['anulate']=='NONE'){
        $html.="<input type='text' id='anulate' >";
    }
    else{
        $html.=$details['anulate'];
    }
    $html.="</td></tr>
                <tr><td style='font-weight:bold'>Registru special casa de marcat</td><td style='min-width:100px'>";
    if($details['fiscalticket']=='NONE'){
        $html.="<input type='text' id='raportfiscal' >";
    }
    else{
        $html.=$details['fiscalticket'];
    }
    $html.="</td></tr>
                </table>";
    if($details['plati']=='NONE'){
        $html.="<input type='hidden' id='timeOfClosure' value='".$agencyInfo['vcl_time']."'>";
        $html.="<input type='hidden' id='clubCodeOfClosure' value='".$agencyInfo['vcl_jur_code']."'>";
        $html.="<br /><button id='saveCommissionsClosure' class='btn btn-primary btn-small'>".$lang->getLang('Save')."</button>";
    }
     $html.="</div>";
    return $html;
}


function saveVirtualComissionsClosure($agency, $time,$raportzlast,$plati,$transferCatreAgentie,$transferDinAgentie,$transferDeLaSediu,$differentaProfit,$differentaPierdere,$anulate,$fiscalticket){
 global $dbh;

    if(isset($_SESSION['admin_id'])) {
        if ($raportzlast == '') {
            $raportzlast = 0;
        }
        if ($plati == '') {
            $plati = 0;
        }
        if ($transferCatreAgentie == '') {
            $transferCatreAgentie = 0;
        }
        if ($transferDinAgentie == '') {
            $transferDinAgentie = 0;
        }
        if ($transferDeLaSediu == '') {
            $transferDeLaSediu = 0;
        }
        if ($differentaProfit == '') {
            $differentaProfit = 0;
        }
        if ($differentaPierdere == '') {
            $differentaPierdere = 0;
        }
        if ($anulate == '') {
            $anulate = 0;
        }
        if ($fiscalticket == '') {
            $fiscalticket = 0;
        }
        $updated = "~raportzlast:" . $raportzlast
            . "~plati:" . $plati
            . "~tcatreag:" . $transferCatreAgentie
            . "~tdinag:" . $transferDinAgentie
            . "~tdelasediu:" . $transferDeLaSediu
            . "~diffprofit:" . $differentaProfit
            . "~diffpierdere:" . $differentaPierdere
            . "~anulate:" . $anulate
            . "~fiscalticket:" . $fiscalticket;
        $sql = "update virtual_cashier_closure set vcl_info=(CONCAT(SUBSTRING_INDEX(vcl_info, 'plati', 1),'$updated'))  where vcl_time='" . $dbh->escape($time) . "' AND vcl_jur_code=" . $dbh->prepareString($agency);
        //cashierid:11556###company:RG FAIRPLAY###raportzsport:0.00;0.00~raportznonsport:3.00;0.00~raportznumere:2.00;0.00###plati:NONE~tcatreag:NONE~tdinag:NONE~tdelasediu:NONE~diffprofit:NONE~diffpierdere:NONE
        if ($dbh->exec($sql)) {
            doAdminUserLog($_SESSION['admin_id'], 'virtual_cashier_closure', "Updated info for club $agency , time $time to $updated");
            return "1";
        } else {
            return "-1";
        }
    }
    else{
        return -2;
    }


}
?>