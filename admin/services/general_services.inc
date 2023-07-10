<?php
require_once '../../config/backoffice.conf.inc';
include_once '../../includes/classes/WebRequest.class.inc';
require_once '../../vendor/pear/mail/Mail.php';
// require_once 'Mail/mime.php' ;
require_once '../configuration/game_functions.inc' ;
require_once '../configuration/skins_functions.inc' ;
require_once '../../includes/classes/Skin.class.inc';

if (isset($_GET['mp'])){
    $file = "../../cache/documents/".$_GET['mp'].".xls";
    header("Pragma: public"); // required
    header("Expires: 0");
    header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
    header("Cache-Control: private",false); // required for certain browsers
    header("Content-Type: application/force-download");
    header("Content-Disposition: attachment; filename=".$_GET['mp'].".xls;");
    ob_clean();
    flush();
    readfile($file);
    die(var_dump($file));
    exit;

}

/*if(!empty($_SERVER['HTTP_X_REQUESTED_WITH']) && strtolower($_SERVER['HTTP_X_REQUESTED_WITH']) == 'xmlhttprequest' ) {
    $data = json_decode(file_get_contents('php://input'), true);
    if($data['action']){
        $_REQUEST['action']=$data['action'];
    }
}*/

$action = $_REQUEST['action'];
if (isset($action)){
    switch ($action)
    {
        case "0":
        {
            $date = isset($_POST['day']) ? $_POST['day'] : date('Y-m-d',time() - 60 * 60 * 24);
            generateLga($date);
            break;
        }

        case '1':
        {
            getAllPlayersLike($_POST['player']);
            break;
        }
        case '2':{
            $date_end=$_POST['date_end'];
            $day_end=$_POST['day_end'];
            $start=$_POST['start'];
            $rowsperpage=$_POST['rows'];
            die(getLgaRecords($date_end,$day_end,$start,$rowsperpage));
            break;
        }
        case '3':{
            $day_start=$_POST['day_start'];
            $day_end=$_POST['day_end'];
            $start=$_POST['start'];
            $rowsperpage=$_POST['rows'];
            die(getLgaTax($day_start,$day_end,$start,$rowsperpage));
            break;
        }

        case '4':{
            $u=$_POST['user'];
            $_days=$_POST['days'];
            die(getUserHistory($u,$_days));
            break;
        }
        case '5':{
            $id=$_POST['bnr'];
            die(deleteBanner($id));
            break;
        }

        case '6':{
            $event=$_POST['event_id'];
            die(json_encode(getEventDetails($event)));
            break;
        }
        case '7':{
            $_user=$_POST['user'];
            $_date=$_POST['date'];
            $endDate=$_POST['endDate'];
            die(getUserLiveDetails($_user,$_date,$endDate));
            break;
        }

        case '8':{
            global $dbh;
            $_user=$_POST['user'];
            $type='deposit';
            $_amount=$_POST['amount'];
            $reason=$_POST['note'];
            $aus_id=$_SESSION['admin_id'];
            $_ticket=$_POST['ticket'];
            $res_id=$_POST['result'];
            $game_id=$_POST['game'];
            if(!(checkHashedPassword($_SESSION['admin_username'],$_POST['token']))) {
                die("Invalid password!");
            }
            else{
                if(!ticketwasrefunded($_ticket)){
                    $dbh->begin();
                    if(addTicketResult($res_id,$_user,$_amount,$game_id,$_ticket)){

                        if(strlen(doAdjustment($_user, $type, $_amount, $reason, $aus_id, $mngr_id="NULL"))>0){
                            if(updateTicket($_ticket,$_amount,$reason,$aus_id)){
                                $dbh->commit();
                                die('1');
                            }
                            else{
                                $dbh->rollback();
                                die('An error has occurred.Please try again!');

                            }
                        }
                        else{
                            $dbh->rollback();
                            die('An error has occurred.Please try again!');

                        }
                    }
                    $dbh->rollback();
                    die('An error has occurred.Please try again!');

                }
                else{
                    die('Ticket already refunded');
                }

            }
            break;
        }
        case'9':
        {
            $start = $_REQUEST["iDisplayStart"];
            $length = $_REQUEST["iDisplayLength"];
            $totals=$_REQUEST['totals'];
            $companies=$_REQUEST['clubs'];
            echo getUserCredits($companies,$totals,$start,$length);
            break;
        }

        case'10':
        {
            $_promotionId = $_REQUEST["promotion"];
            $_year = $_REQUEST["year"];
            $_activePlayers=$_REQUEST['active'];
            die(getPromotionUsersActive($_promotionId,$_year,$_activePlayers));
            break;
        }
        case'11':
        {
            $_affId = $_REQUEST["afid"];

            die(getAffiliateInfo($_affId));
            break;
        }
        case '12':{
            $_currency=$_REQUEST['curName'];
            $newValue=$_REQUEST['curVal'];
            $_SESSION['currencies'][$_currency]['value']=$newValue;
            die('1');
            break;
        }
        case '13':{
            $_currency=$_REQUEST['curName'];
            $_SESSION['currencies'][$_currency]['value']= $_SESSION['currencies'][$_currency]['origvalue'];
            die($_SESSION['currencies'][$_currency]['value']);
            break;
        }
        case "getBettingHandHistory":
        {
            $options=array("action"=>"get_details_of_ticket","ticketCode"=>$_REQUEST['ticket'],"manage"=>"0");
            die(doBettingRequest($options,"/admin/remote/ticketsProvider.php"));

            break;
        }
        case "getInactivePlayers":{
            die(getInactivePlayers($_POST['months']));
            break;
        }
        case "generateInactiveCsv":{
            generateInactivePlayersCsv();
            break;
        }

        case "grabPartnerDetails":{
            die(getPartnerDetails($_POST['partner'],$_POST['type']));
            break;
        }

        case "grabGameLimitDetails":{
            die(getGameLimitDetails($_POST['user'],$_POST['username']));
            break;
        }

        case "deleteUserGameLimit":{
            die(deleteUserGameLimit($_POST['limit_id']));
            break;
        }

        case "deletePartner":{
            if(check_access('manage_partners_modification')) {

                die(delPartner($_POST['partner']));
            }
            break;
        }
        case "blockPartners":{
            if(check_access('manage_partners_modification')) {
                die(setPartnerAccess($_POST['partner'], trim(strtolower($_POST['type']))));
            }
            break;
        }
        case "updatePartner":{


            if(check_access('manage_partners_modification')) {
                $ip=str_replace(' ', '',  $_POST['partnerIp']);

                //sends a url request with the specified parameters
                $partnerId = $_POST['partner'];
                $url = $_SEREVER['WEB_SERVICES'];
                $srvResponse = editPartnerCurlRequest($url, $partnerId);
                die(updatePartner($_POST['partner'], $_POST['partnerName'], $_POST['partnerClub'], $_POST['partnerWebsite'],$ip, $_POST['partnerKey'], $_POST['partnerSubdomain'], $_POST['partnerEId'], $_POST['partnerForceWithdraw'], $_POST['partnerNote'],$_POST['partnerProviders'],$_POST['partnerParams'],$_POST['partnerIsSeamless'],$_POST['partnerClient']));
            }
            break;
        }
        case "updateUserGameLimit":{

            if(check_access('casino_games_limit_modify')) {

                die(updateUserGameLimit($_POST['type'],$_POST['userGameLimit'],$_POST['currentDepositLimit'],$_POST['currentWithdrawLimit'],$_POST['currentBetLimit'],$_POST['currentWinLimit'],$_POST['currentProvider'],$_POST['currentLock']));
            }
            break;
        }
        case "addPartner":{
            if(!isset($_SESSION['jurisdiction_class'])){
                die('Your session has expired, please login again');
            }
            if(isset($_POST['webtype'])){
                $webtype=$_POST['webtype']."://";
            }
            if(isset($_POST['http'])){
                $http=$_POST['http']."://";
            }
            if(is_array($_POST['ips'])){
                $_POST['ips']=implode('|', $_POST['ips']);
            }
            $ip=str_replace(' ', '', $_POST['ips']);

            $_POST['addPartnerProviders']=implode(',',$_POST['addPartnerProviders']);
            $website=$webtype.$http.$_POST['addPartnerWebsite'];
            $url = $_SEREVER['WEB_SERVICES'];
            $partnerId = addPartner($_POST['addPartnerName'],
                $_POST['addClub'],
                $website,
                $ip,
                $_POST['addPartnerKey'],
                $_POST['addPartnerSubdomain'],
                $_POST['addPartnerEId'],
                $_POST['addPartnerForceWithdraw'],
                $_POST['addPartnerStatus'],
                $_POST['addPartnerNotes'],
                $_POST['addPartnerProviders'],
                $_POST['addPartnerParams'],
                $_POST['addPartnerIsSeamless'],
                $_POST['addPartnerClient']
            );
            $curlResponse = addPartnerCurlRequest($url,$partnerId);
            if($curlResponse == "0"){
                die("2");
            }

            break;
        }
        case "addGameLimit":{
            $limitId = addGameLimit(
                $_POST['searchByadd'],
                $_POST['searchUseradd'],
                $_POST['jurisdiction_level_1'],
                $_POST['jurisdiction'],
                $_POST['deposit_limit_add'],
                $_POST['withdraw_limit_add'],
                $_POST['bet_limit_add'],
                $_POST['win_limit_add'],
                $_POST['searchProvideradd'],
                $_POST['Lockadd']
            );
            die($limitId);
            break;
        }
        case "updateGameLimit":{
            $limitId = updateGameLimit(
                $_POST['update_jur_type'],
                $_POST['update_jur_id'],
                $_POST['deposit_limit_update'],
                $_POST['withdraw_limit_update'],
                $_POST['bet_limit_update'],
                $_POST['win_limit_update'],
                $_POST['searchProviderupdate'],
                $_POST['Lockupdate']
            );
            die('2');
            break;
        }
        case "sendFinancialReport":{
            die(sendFinancialReportByEmail($_POST['email']));
            break;
        }
        case "getUserAvailablePromotions":{
            if($_POST['username'] && $_POST['username']!=''){
                $currentUser=getPunterByUsername($_POST['username']);
            }
            die(getUserAvailablePromotions($currentUser['pun_id'],$currentUser['cur_code_for_web']));
            break;
        }

        case "checkAffiliateAlreadyExists":{
            $sql="Select count(pun_id) from punter where pun_username=".$dbh->prepareString($_REQUEST['username']);
            $exists=$dbh->queryOne($sql);
            if($exists=="0"){
                $sql="Select count(aus_id) from admin_user where aus_username=".$dbh->prepareString($_REQUEST['username']);
                $exists=$dbh->queryOne($sql);
            }
            if($exists=="0"){
                $exists=true;
            }
            else{
                $exists="Username already exists";
            }
            die(json_encode($exists));
            break;
        }

        case "checkEmailAlreadyExists":{
            $sql="Select count(pun_id) from punter where pun_email=".$dbh->prepareString($_REQUEST['email']);
            $exists=$dbh->queryOne($sql);
            if($exists=="0"){
                $sql="Select count(aus_id) from admin_user where aus_email=".$dbh->prepareString($_REQUEST['email']);
                $exists=$dbh->queryOne($sql);
            }
            if($exists=="0"){
                $exists=true;
            }
            else{
                $exists="Email already exists";
            }
            die(json_encode($exists));
            break;
        }

        case "getJurisdictionAccessType":{
            if(!$_REQUEST['jurisdiction']){
                return "-1";
            }
            $sql="Select JUR_ACCESS_TYPE from jurisdiction where jur_id=".$dbh->prepareString($_REQUEST['jurisdiction']);
            $access=$dbh->queryOne($sql);
            die($access);
            break;
        }
        case 'getUserHandDetails':{
            if(isLoggedIn()){
                require_once 'GameTranscript.class.inc';
                global $dbh;


                $result=GameTranscript::getBet($_POST['hand'],$_POST["user"],$_POST["gmn"],true );
                if(!$result){
                    $result="No bets";
                }
                else{
                    $sql = "SELECT pre_bet,
                pre_win
                 ";
                    if(EXT_PARTNER_ID>0){
                        $sql .="FROM ext_punter_result, punter
                            WHERE pre_res_id = ".$_POST['hand']."
                            AND pre_pun_id = pun_customer_number
                            AND pun_customer_number = " .$dbh->escape($_GET["uid"]);                }
                    else{
                        $sql .=" FROM punter_result
    		                     JOIN punter on pun_id = pre_pun_id
    		                     WHERE pre_res_id=".$_POST['hand']."
    		                     AND pre_pun_id = " .$dbh->escape($_POST["user"]);
                    }
                    $bwin=$dbh->queryRow($sql);
                    $result ="<table  cellspacing='10' cellpadding='4' rules=rows>
                                <tr><td style='background-color:#4A7EB6;color:white;' >Total bet</td><td>".getInDollars($bwin['pre_bet'])."</td></tr>
                                <tr><td style='background-color:#4A7EB6;color:white;' >Total win</td><td>".getInDollars($bwin['pre_win'])."</td></tr>
                                </table><br /><br />".$result;


                }
                die($result);


            }else{
                die("-1");
            }
            break;
        }
        case "getPoker2dHands":{

            die(searchPokerHands2d($_REQUEST));
            break;
        }
        case "getAccountFinnancialSummary":{

            die(_getAccountFinnancialSummary());
            break;
        }
        case "sendJurisdictionCredits":{
            require_once 'EntityCreditsHandler.class.inc';
            if(!(checkHashedPassword($_SESSION['admin_username'],$_POST['token']))) {
                die("Invalid password!");
            }
            else {
                $from = new EntityCreditsHandler($_SESSION['jurisdiction_id']);
                $to = new EntityCreditsHandler($_POST['jurisdiction'],'club');
                if($_POST['type']=='deposit'){
                    $result= $from->sendCredits($to, $_POST['amount'], $_POST['note']);
                }
                else{
                    $result=$from->withdrawCredits($to, $_POST['amount'], $_POST['note']);
                }
                if(areErrors()){
                    die(showErrors());
                }
                if($result){
                    echo($to->getAvailableCredit());
                    die();
                }
                else{
                    die("Unable to execute your request");
                }

                break;
            }
        }
        case "getPartners":{
              getPartners($_REQUEST);
         break;
        }
        case "getGamesLimit":{
              getGamesLimit($_REQUEST);
         break;
        }
        case "getPendingTrans":{
              getPendingTrans($_REQUEST);
         break;
        }
        case "deleteGameLimit":{
              deleteGameLimit($_REQUEST);
         break;
        }
        case "getUsers":{
            $username=(isset($_REQUEST['type']) && $_REQUEST['type']==1? $_REQUEST['value'] : null);
            $email=(isset($_REQUEST['type']) && $_REQUEST['type']==2? $_REQUEST['value'] : null);
            $exactResult=(isset($_REQUEST['exactResult']) && $_REQUEST['exactResult']==2? true : null);
             $users=getUsers($username,$email,$exactResult);
            die(json_encode($users->toArray()));
            break;
        }
        case "getUsersId":{
            $username=$_REQUEST['value'];
            $exact=$_REQUEST['exact'];
             $users=getUsersId($username,$exact);
            die(json_encode($users->toArray()));
            break;
        }
        case "getUserSession":{
            $username=(isset($_REQUEST['username']) && $_REQUEST['username']!=''? $_REQUEST['username'] : null);
            $extuid=(isset($_REQUEST['extuid']) && $_REQUEST['extuid']!=''? $_REQUEST['extuid'] : null);
             $userSession=getUserSession($username,$extuid);
            die($userSession);
            break;
        }
        case "getGames" :{

           $return ='<table class="table table-striped display">
                <thead>
                <tr>
                    <th>'.$lang->getLang("Id").'</th>
                    <th>'.$lang->getLang("Name").'</th>
                    <th>'.$lang->getLang("Min Bet").'</th>
                    <th>'.$lang->getLang("Max Bet").'</th>
                    <th>'.$lang->getLang("Max Payout").'</th>
                    <th>'.$lang->getLang("Min Pos Bet").'</th>
                    <th>'.$lang->getLang("Max Pos Bet").'</th>
                    <th>'.$lang->getLang("Small blind").'</th>
                    <th>'.$lang->getLang("Big blind").'</th>
                    <th>'.$lang->getLang("Ext Code").'</th>
                    <th>'.$lang->getLang("Category").'</th>
                    <th>'.$lang->getLang("Type").'</th>
                    <th>'.$lang->getLang("Localization").'</th>
                    <th>'.$lang->getLang("Admin history class").'</th>
                    <th>'.$lang->getLang("Is common draw").'</th>
                    <th>'.$lang->getLang("Params").'</th>
                    <th>'.$lang->getLang("Client type").'</th>
                    <th>'.$lang->getLang("Game version").'</th>
                    <th>'.$lang->getLang("Is private").'</th>
                    <th>'.$lang->getLang("Action").'</th>
                </tr>
                </thead>
                <tbody>';
                $rs = getGamesList();
                while ( $rs->hasNext () ) {
                    $row = $rs->next ();
                    $return.="  <tr>
                        <td>".$row['gam_id']."</td>
                        <td>".$row['gam_name']."</td>
                        <td>".$row['gam_min_bet']."</td>
                        <td>".$row['gam_max_bet']."</td>
                        <td>".$row['gam_max_payout']."</td>
                        <td>".$row['gam_min_pos_bet']."</td>
                        <td>".$row['gam_max_pos_bet']."</td>
                        <td>".$row['gam_small_blind']."</td>
                        <td>".$row['gam_big_blind']."</td>
                        <td>".$row['gam_ext_code']."</td>
                        <td>".$row['gam_category']."</td>
                        <td>".$row['gam_type']."</td>
                        <td>".$row['gam_localization']."</td>
                        <td>".$row['gam_admin_history_class']."</td>
                        <td>".($row['gam_is_common_draw']==1?'Yes':'No')."</td>
                        <td>".$row['gam_params']."</td>
                        <td>".$row['gam_client']."</td>
                        <td>".$row['gam_ver']."</td>
                        <td>".($row['gam_is_private']==1?'True':'False')."</td>
                        <td> <a href='#myModal' class='updateGame btn btn-primary btn-small' style='color:white' data-toggle='modal' >Modify</a></td>
                    </tr>";
                 }
            $return.="   </tbody>
            </table>";


            die($return);

            break;
        }
        case "getTournament" :{

            $from = $_POST['from'];
            $until = $_POST['until'];
            $status = $_POST['status'];

            $return ='<table class="table table-striped display">
                <thead>
                <tr>
                    <th>'.$lang->getLang("Game").'</th>
                    <th>'.$lang->getLang("Name").'</th>
                    <th>'.$lang->getLang("Description").'</th>
                    <th>'.$lang->getLang("Start time").'</th>
                    <th>'.$lang->getLang("Start hour").'</th>
                    <th>'.$lang->getLang("Min inscriptions").'</th>
                    <th>'.$lang->getLang("Max inscriptions").'</th>
                    <th>'.$lang->getLang("Amount").'</th>
                    <th>'.$lang->getLang("Game duration").'</th>
                    <th>'.$lang->getLang("Time wait bstart").'</th>
                    <th>'.$lang->getLang("Time wait astart").'</th>
                    <th>'.$lang->getLang("Status").'</th>
                    <th>'.$lang->getLang("Action").'</th>
                </tr>
                </thead>
                <tbody>';
                $rs = getTournamentList($from,$until,$status);
                while ( $rs->hasNext () ) {
                    $row = $rs->next ();

                    $status = '';

                    switch ($row['gtt_status']) {
                        case 0:
                            $status = "<span class='muted'>".$lang->getLang('Closed')."</span>";
                            break;
                        case 1:
                            $status = "<span class='text-success'>".$lang->getLang('Open')."</span>";
                            break;
                        case 2:
                            $status = "<span class='text-error'>".$lang->getLang('Erased')."</span>";
                            break;

                    }

                    $return.="  <tr data-entry='".$row['gtt_id']."'>
                        <td data-game='".$row['gam_id']."'>".$row['gam_name']." [".$row['gam_id']."]</td>
                        <td>".$row['gtt_name']."</td>
                        <td>".$row['gtt_description']."</td>
                        <td>".$row['gtt_start_time']."</td>
                        <td>".($row['gtt_start_hour']=='' || $row['gtt_start_hour'] == null?'-':$row['gtt_start_hour'])."</td>
                        <td>".$row['gtt_min_inscriptions']."</td>
                        <td>".$row['gtt_max_inscriptions']."</td>
                        <td>".getInDollars($row['gtt_amount'])."</td>
                        <td>".$row['gtt_game_duration_time']."</td>
                        <td>".$row['gtt_time_wait_bstart']."</td>
                        <td>".$row['gtt_time_wait_astart']."</td>
                        <td>".$status."</td>
                        <td> <a href='#tournamentModal' class='updateTournamentGame btn btn-primary btn-small' style='color:white' data-toggle='modal' >Modify</a></td>
                    </tr>";
                 }
            $return.="   </tbody>
            </table>";


           die($return);

            break;
        }
        case "getGeneralGames" :{
            $generalGames=getGeneralGames()?>
            <table class="table table-striped display">
                <thead>
                <tr>
                    <th><?=$lang->getLang("Id")?></th>
                    <th><?=$lang->getLang("Category")?></th>
                    <th><?=$lang->getLang("Button image")?></th>
                    <th><?=$lang->getLang("Game Id")?></th>
                    <th><?=$lang->getLang("Width")?></th>
                    <th><?=$lang->getLang("Height")?></th>
                    <th><?=$lang->getLang("Provide code")?></th>
                    <th><?=$lang->getLang("Has play for fun")?></th>
                    <th><?=$lang->getLang("Web name")?></th>
                    <th><?=$lang->getLang("Action")?></th>

                </tr>
                </thead>
                <tbody>
                <?  while ( $generalGames->hasNext () ) {
                    $row = $generalGames->next ();
                    ?>
                    <tr>
                        <td><?=$row['ggi_id'];?></td>
                        <td><?=$row['ggi_category'];?></td>
                        <td><?=$row['ggi_button_image_name'];?></td>
                        <td><?=$row['ggi_game_id'];?></td>
                        <td><?=$row['ggi_width'];?></td>
                        <td><?=$row['ggi_height'];?></td>
                        <td><?=$row['ggi_pes_group'];?></td>
                        <td><?=($row['ggi_has_forfun']==1?'Yes':'No')?></td>
                        <td><?=$row['ggi_web_name'];?></td>
                        <td> <a href="#insertGeneralModal" class="updateGeneralGame btn btn-primary btn-small" style="color:white" data-toggle="modal" >Modify</a></td>
                    </tr>
                    <?
                }?>
                </tbody>
            </table>
         <?
           die();
            break;
        }
        case "getProvidersList": {
            $providers = getProvidersList($_REQUEST['value']);
            die(json_encode($providers->toArray()));
            break;
        }
        case "getSkins": {
            $providers=getProviders();
            //var_dump($providers);
            ?>

         <table class="table display">
                                <thead>
                                <tr>
                                    <th><?=$lang->getLang("Id")?></th>
        <th><?=$lang->getLang("Skin Name")?></th>
        <th><?=$lang->getLang("URLS")?></th>
        <th><?=$lang->getLang("Folder")?></th>
        <th><?=$lang->getLang("Status")?></th>
        <th><?=$lang->getLang("Allowed ips")?></th>
        <th><?=$lang->getLang("Club")?></th>
        <th><?=$lang->getLang("Key")?></th>
        <th><?=$lang->getLang("Email")?>/SMTP</th>
        <th><?=$lang->getLang("Providers enabled")?></th>
        <th><?=$lang->getLang("Params")?></th>
        <th><?=$lang->getLang("Action")?></th>
        </tr>
        </thead>
        <tbody>
        <?php
        $rs = getSkins ();
        while ( $rs->hasNext () ) {
            $row = $rs->next ();
            ?>
            <tr>
                <td><?=$row['skn_id'];?></td>
                <td><strong><?=$row['skn_name']?></strong></td>
                <td>
                    <button class="btn btn-small showDetails">View</button>
                    <div class="details aleft">
                        <span class="bold underline">Domain:</span><span class="domain"><?=$row['skn_domain'];?></span><br />
                        <span class="bold underline">Website:</span><span class="website"><?=$row['skn_url'];?></span><br />
                        <span class="bold underline">Media:</span><span class="media"><?=$row['skn_media_url'];?></span><br />
                        <span class="bold underline">Gamelauncher:</span><span class="gamelauncher"><?=$row['skn_game_launcher'];?></span><br />
                        <span class="bold underline">Betting:</span><span class="betting"><?=$row['skn_betting_url'];?></span><br />
                        <span class="bold underline">Backoffice:</span><span class="backoffice"><?=$row['skn_backoffice_url'];?></span><br />
                        <span class="bold underline">Activation:</span><span class="activation"><?=$row['skn_reg_activation_url'];?></span><br />
                        <span class="bold underline">Gateway:</span><span class="gateway"><?=$row['skn_gateway_response_url']?></span><br />
                        <span class="bold underline">Kiosk:</span><span class="kiosk"><?=$row['skn_kiosk_url']?></span><br />
                        <span class="bold underline">Banners:</span><span class="banners"><?=$row['skn_banner_url']?></span>
                    </div>
                </td>
                <td><?=$row['skn_foldername']?></td>
                <td><?=$row['skn_status']==1?'<span class="text-success">Enabled</span>':'<span class="text-error">Disabled</span>' ?></td>
                <td>
                    <button class="btn btn-small showDetails">View</button>
                    <div class="details">
                        <?foreach (explode('|',$row['skn_allowip']) as $sknK=>$sknV){
                            ?>
                            <span class="ip"><?=$sknV?></span><br />
                            <?
                        }
                        ?>
                    </div>
                <td><?=getClubName($row['skn_jur_id'])?></td>
                <td><?=$row['skn_key']?></td>
                <td> <button class="btn btn-small showDetails">View</button>
                    <div class="details">
                        <span class="email"><?=$row['skn_email']?></span><br /><span class="smtp"><?=$row['skn_smtp']?></span>
                    </div></td>
                <td>
                    <button class="btn btn-small showDetails">View</button>
                    <div class="details">
                        <?$providersEnabled=explode(',',$row['skn_pes_code_enabled']);
                               $providerFound=false;
                               $providers->resetRecPtr();
                         while ($providers->hasNext()){
                             $provider=$providers->next();
                             if(in_array($provider['pes_game_code'],$providersEnabled)){if($providerFound){?>,<?}$providerFound=true;?><?=$provider['pes_name']?><?}}
                     ?>
                    </div>
                </td>
                <td>
                    <button class="btn btn-small showDetails">View</button>
                    <div class="details">
                        <?foreach (explode('~',$row['skn_params']) as $sknK=>$sknV){
                            ?>
                            <span class="params"><?=$sknV?></span>span<br />
                            <?
                        }
                        ?>
                    </div>
                </td>


                <td class="content" style="min-width: 60px;">

                    <a href="#myModal" class="updateSkins btn btn-success btn-small" style="color:white" data-toggle="modal" >Modify</a>
                </td>
            </tr>
        <?php }?>
        </tbody>
        </table>
        <?

            break;
        }
        case "addSkin" :{

            $responseId = addSkin($_REQUEST);
            if(isJSON($responseId)){
                die($responseId);
            } else {
                $url = $_SEREVER['WEB_SERVICES'];
                $serverResponse = addSkinCurlRequest($url, $responseId);
                die(json_encode([1,'Successfully created skin']));
            }

            die($response);
            break;
        }
        case "updateSkin":
        {
            $skinId = $_REQUEST['skinid'];
            $url = $_SEREVER['WEB_SERVICES'];
            editSkinCurlRequest($url, $skinId);
            echo(updateSkin($_REQUEST));
            die();
            break;
        }
        case "getProviders":
        {
            $currencies=getAllCurrencies();

            ?>

            <table class="table table-striped display">
                <thead>
                <tr>
                    <th><?=$lang->getLang("Id")?></th>
                    <th><?=$lang->getLang("Provider Name")?></th>
                    <th><?=$lang->getLang("Allowed ip")?></th>
                    <th><?=$lang->getLang("Provider Group")?></th>
                    <th><?=$lang->getLang("Remote id")?></th>
                    <th><?=$lang->getLang("Game code")?></th>
                    <th><?=$lang->getLang("Game id")?></th>
                    <th><?=$lang->getLang("Key")?></th>
                    <th><?=$lang->getLang("Website")?></th>
                    <th><?=$lang->getLang("Status")?></th>
                    <th><?=$lang->getLang("Transfer type")?></th>
                    <th><?=$lang->getLang("Params")?></th>
                    <th><?=$lang->getLang("Master currency")?></th>
                    <th><?=$lang->getLang("Other currency")?></th>
                    <th><?=$lang->getLang("Has multi currency")?></th>
                    <th><?=$lang->getLang("Action")?></th>

                </tr>
        </thead>
        <tbody>
        <?php
        $rs=getProviders();
        while ($rs->hasNext()){
            $row=$rs->next();
            ?>
            <tr>
                <td><?=$row['pes_id']?></td>
                <td><strong><?=$row['pes_name']?></strong></td>
                <td><?=$row['pes_allowip']?></td>
                <td><?=$row['pes_group']?></td>
                <td><?=$row['pes_remote_id'];?></td>
                <td><?=checkGames($row['pes_game_code'])?></td>
                <td><?=$row['pes_gam_id'];?></td>
                <td><?=$row['pes_remote_key']?></td>
                <td><?=$row['pes_remote_url'];?></td>
                <td><?=($row['pes_status']==1?'<span class="text-success">Enabled</span>':($row['pes_status']==2?'<span class="text-success">Enabled with withdraw</span>':'<span class="text-error">Disabled</span>')) ?></td>
                <td><?=$row['pes_transfer_type']?></td>
                <td> <button class="btn btn-small showDetails">View</button>
                    <div class="details">
                        <?foreach (explode('~',$row['pes_params']) as $sknK=>$sknV){
                            ?><span class="params"><?=$sknV?></span><br />
                            <?}?>
                    </div>
                </td>
                <td>
                    <?
                    $currencies->resetRecPtr();
                    while ($currencies->hasNext()) {
                        $rowCurr = $currencies->next();
                       if($rowCurr['cur_id']==$row['pes_master_currency']){
                        ?>
                           <?=$rowCurr['cur_code']?>

                    <? }
                    } ?>

                   </td>
                <td class="content">
                   <? $row['pes_other_currency']=explode(';',$row['pes_other_currency']);
                   $found=false;
                   foreach ($row['pes_other_currency'] as $pok=>$pov){
                       $currencies->resetRecPtr();
                       while ($currencies->hasNext()) {
                            $rowCurr = $currencies->next();
                           if ($rowCurr['cur_id'] == $pov) {
                               if($found) { ?>;<?}$found=true;?><?= $rowCurr['cur_code'] ?>
                           <? }
                       }
                       }
                   ?>

                    </td>
                <td class="content"><?=$row['pes_has_multy_cur_param']==1?'<span class="text-success">Yes</span>':'<span class="text-error">No</span>' ?></td>

                <td class="content" style="min-width: 60px;">
<!--                <a href="#myModal" class="updateProviders btn btn-success btn-small" style="color:white" data-toggle="modal" >Modify</a>-->
                    <div class="btn-group">
                        <button class="btn btn-small dropdown-toggle" data-toggle="dropdown"><?=$lang->getLang("Action")?><span class="caret"></span></button>
                        <ul class="dropdown-menu" style="min-width:80px; left:unset; right:0 !important;">
                            <li class="modifyProvider"><a href="#myModal" class="updateProviders" data-toggle="modal" ><i class="icon-pencil"></i>  <?=$lang->getLang("Modify")?></a></li>
                            <li class="clonePartner"><a href="#myModal"  class="cloneProviders" data-toggle="modal" ><i class="icon-file"> </i> <?=$lang->getLang("Clone")?></a></li>
                        </ul>
                    </div>
                </td>
            </tr>

        <?php }?>
        </tbody>
        </table>
            <?
            break;
        }
        case "addProvider" :{
            $providerId = addProvider($_REQUEST);
            if($providerId == '2' || $providerId == '3' || $providerId == '4' || $providerId == '5' ) {
                echo $providerId;
                die();
            }
            else{
                $url = $_SEREVER['WEB_SERVICES'];
                $response = addProviderCurlRequest($url, $providerId);
                if($response == 0){
                    echo '1';
                    die();
                } else {
                    echo '4';
                    die();
                }
            }

            break;
        }
        case "updateProvider" :{
            $providerId = $_REQUEST['providerid'];
            $url = $_SEREVER['WEB_SERVICES'];
            $serverResponse = editProviderCurlRequest($url, $providerId);
            echo(updateProvider($_REQUEST));
            die();
            break;
        }
        case "getSkinMenu":{
            $skin=new Skin($_REQUEST['jurisdiction']);
            if(isset($_REQUEST['type'])){
                $skin->setType($_REQUEST['type']);
            }
            echo($skin->createMenuOptions($skin->getMenuFromDb()));
            die();
            break;
        }
        case "updateSkinMenu":{
            $skin=new Skin($_REQUEST['jurisdiction']);
            if(isset($_REQUEST['type'])){
                $skin->setType($_REQUEST['type']);
            }
            echo($skin->setMenu(json_decode($_REQUEST['menu']),$_REQUEST['underLvl']));
            die();
            break;
        }


        case "getSkinProviders":{
            $skin=new Skin($_REQUEST['jurisdiction']);
            if(isset($_REQUEST['type'])){
                $skin->setType($_REQUEST['type']);
            }
            echo getProvidersListEnabledForSkin($_REQUEST['jurisdiction']);
            die();
            break;
        }


        case "getSkinGames":{
            $skin=new Skin($_REQUEST['jurisdiction']);
            if(isset($_REQUEST['type'])){
                $skin->setType($_REQUEST['type']);
            }

            echo $skin->createGamesSelectNew($_REQUEST['providers']);
            die();
            break;
        }

        case "updateSkinGames":{
            if ($_REQUEST['jurisdiction'] == 1) {
                echo '0';
                die();
            } // end if
            $skin=new Skin($_REQUEST['jurisdiction']);
            if(isset($_REQUEST['type'])){
                $skin->setType($_REQUEST['type']);
            }

            //$games = json_decode($_REQUEST['games']);
            
            //var_dump("====> games: ");
            //var_dump($games);
            //var_dump("====> end games: ");
            $underLvl = $_REQUEST['underLvl'];
            //var_dump($underLvl);
            
            $response =$skin->setGames(json_decode($_REQUEST['games']),$_REQUEST['underLvl']);
            echo rtrim(ltrim($response));
            //echo($skin->setMenu(json_decode($_REQUEST['menu'])));
            die();
            break;
        }

        case "getSkinSettings":{
            $skin=new Skin($_REQUEST['jurisdiction']);
            if(isset($_REQUEST['type'])){
                $skin->setType($_REQUEST['type']);
            }
            echo $skin->createSettingsView();
            die();
            break;
        }
        case "updateSkinSettings":{

            $skin=new Skin($_REQUEST['jurisdiction']);
            $params = array();
            //var_dump($params);
            if(isset($_REQUEST['type'])){
                $skin->setType($_REQUEST['type']);
            }
            parse_str($_REQUEST['settings'], $params);
            echo $skin->setSkinProperties($params,$_REQUEST['underLvl']);
            die();
            break;
        }
        case "getChipDumpingClients":{
            die(getChipDumpingClients($_REQUEST));
            break;
        }
        case "saveChipDumpingClient":{
            if(saveChipDumpingClient($_POST['name'],$_POST['type'],$_POST['id'])){
                die(json_encode([1]));
            }
            else{
                die(json_encode([-1,"Unable to save client"]));
            }
            break;
        }


        case "getChipDumping":{

            die(getChipDumping($_REQUEST));
            break;
        }   case "getChipDumpingTotals":{

            die(getChipDumpingTotals($_REQUEST));
            break;
        }

        case "saveChipDumping":{
            $id=null;
            if($_POST['chipdumpingid']>0){
                $id=$_POST['chipdumpingid'];
            }

            if(saveChipDumping($_POST['jur'],$_POST['username'],$_POST['club'],$_POST['from'],$_POST['receivefrom'],$_POST['to'],$_POST['giveto'],$_POST['type'],$_POST['amount'],$_POST['date'],$id)){


                die(json_encode([1]));
            }
            else{
                die(json_encode([-1,"Unable to save chip dumping"]));
            }
            break;
        }
        case "deleteChipDumping":{
            if(isset($_POST['id']) && $_POST['id']!=''){
             $return = $dbh->exec('Delete from poker.chip_dumping where cpd_id='.$dbh->escape($_POST['id']));
                die(($return===1?'1':'-1'));
            }
            else{
                die(-1);
            }
            break;
        }
        case "generateChipDumpingReport":{
            die(generateChipDumpingReport($_REQUEST));
            break;
        }

        case "getPartnerData": {
            if(isset($_POST['id']) && $_POST['id']!='') {
                $id = $_POST['id'];
                $data = getPartnerCloneData($id);
                die($data);
            } else {
                die(-1);
            }
            break;
        }

        case "displayPlayingUsers": {
            $data = getExternalServiceIdList();
            //var_dump($data);
            if ($data == "NO INFO"){
                $output = getDatatablesNoInputData();
                die($output);
            } else if(is_string($data) && $data != "NO INFO") {
                $result =  getDatatablesData($data);
                die($result);
            } else {
                $output = getDatatablesNoInputData();
                die($output);
            }
            break;
        }
        case "pendingTransaction":{
            $params = "&uid=".$_POST['uid']."&ausid=".$_SESSION['admin_id']."&transid=".$_POST['tr']."&optype=".$_POST['do'];
            pendingTransaction($params);
            break;
        }
        case "deleteFromCache": {
            $idToDelete = $_POST['selectedId'];
            //var_dump($idToDelete);
            deletedSelectedEntriesFromCache($idToDelete);
            break;
        }

        default:{
            die("Invalid operation!");
        }

    }
}


function deletedSelectedEntriesFromCache($idToDelete){
    $url = WEB_APP_PATH;
    $data="operationcode=1009&type=2&lock=";
    $webReq = new WebRequest($url);

    //lock=PLL_<userid>~PLL_<userid>....

    //for each id compose the string with the data
    foreach($idToDelete as $value) {
        var_dump($value);
        $data .= "PLL_" . $value . "~";
    }
    //var_dump($data);
    $data = substr($data, 0, -1);
    //var_dump($data);
//
    $webReq->setOptions($data);
    $webReq->sendRequest();
    return  $webReq->getResponse();
}

function getExternalServiceIdList() {
    $url = WEB_APP_PATH;
    $data="operationcode=1009&type=1";
    $webReq = new WebRequest($url);
    $webReq->setOptions($data);
    $webReq->sendRequest();
    return  $webReq->getResponse();
}
function pendingTransaction($params) {
    $url = WEB_APP_PATH;
    $data="operationcode=2004".$params;
    $webReq = new WebRequest($url);
    $webReq->setOptions($data);
    $webReq->sendRequest();
    die($webReq->getResponse());
}

function getDatatablesNoInputData() {
    $tableArr = [0 => [0 => "None", 1 => "None", 2 => "None", 3 => "None"]
    ];
    $totals = 0;
    $output = array(
        "draw"            => intval( $_GET['draw'] ),
        "recordsTotal"    => intval($totals),
        "recordsFiltered" => intval($totals),
        "data"            => $tableArr
    );
    return json_encode($output);
}

function getDatatablesData($data ) {
    global $dbh;
    $idArray = array();
    $dataArr = explode("~", $data);
    foreach ($dataArr as $singleEntry) {
        $entry = explode("_", $singleEntry);
        for ($i = 0; $i < count($entry); $i++) {
            if(is_numeric($entry[$i])) {
                array_push ($idArray, $entry[$i]);
            }
        }
    }
    //Create limit part of the sql querry
    $start = $_GET['start'];
    $length = $_GET['length'];
    $limitQuerry = "LIMIT $start , $length";

    //Create order by part of sql querry
    $columns = array('pun_username','jur_name','pcr_credits');
    $columnNumber =$_GET['order'][0]['column'];
    $ascOrDesc = $_GET['order'][0]['dir'];
    $colName = $columns[$columnNumber];
    $orderByQuerry = " ORDER BY " . $colName . " " .$ascOrDesc ;

    $idString = "";
    foreach ($idArray as $id) {
        $idString .= " $id, ";
    }
    $idString = substr($idString, 0, -2);

    $sql = "SELECT pun_username, jur_name, pcr_credits, pun_id  
            FROM punter, jurisdiction, punter_credit
            WHERE pun_id = pcr_pun_id
            AND pun_betting_club = jur_id
            AND pun_id in ( $idString ) $orderByQuerry $limitQuerry";
    $totalsSql = "SELECT count(*)   
            FROM punter, jurisdiction, punter_credit
            WHERE pun_id = pcr_pun_id
            AND pun_betting_club = jur_id
            AND pun_id in ( $idString ) ";

    // Search by part of sql querry
    if($_GET['search']['value'] != "") {
        $searchableColumn1 = "pun_username";
        $searchableColumn2 = "jur_name";
        $searchableColumn3 = "pcr_credits";
        $searchKeyword = $dbh->prepareString($_GET['search']['value']);
        $searchKeyword = trim($searchKeyword, "'");
        $sqlLikeStmnt = " AND ($searchableColumn1 LIKE '%$searchKeyword%' or $searchableColumn2 LIKE '%$searchKeyword%' or $searchableColumn3 LIKE '%$searchKeyword%')";

        $sql = "SELECT pun_username, jur_name, pcr_credits, pun_id  
            FROM punter, jurisdiction, punter_credit
            WHERE pun_id = pcr_pun_id
            AND pun_betting_club = jur_id
            AND pun_id in ( $idString ) $sqlLikeStmnt $orderByQuerry $limitQuerry";
        $totalsSql .= $sqlLikeStmnt;
    }

    $result = $dbh->exec($sql);
    $totals = $dbh->queryOne($totalsSql);

    $tableArr= [];
    while($result->hasNext()) {
        $row = $result->next();
        $id = $row['pun_id'];
        $section = array();
        $username = $row['pun_username'];
        $section[0] = "<a href = '/?page=customers/customer_view&amp;customer_id=$id&amp;header_page=' target='_blank' class='contentlink'> $username </a>";
        $section[1] = $row['jur_name'];
        $section[2] = $row['pcr_credits'];

        $section[3] = "<label class='checkbox inline'><input type='checkbox' class='deleteFromCache' data-id='$id'/>*Select to remove</label>";
        array_push($tableArr, $section);
    }

    $output= array(
        "draw"            => intval( $_REQUEST['draw'] ),
        "recordsTotal"    => intval($totals),
        "recordsFiltered" => intval($totals),
        "data"            => $tableArr
    );
    //var_dump($idArray);
    return json_encode($output);
}

function getPartnerCloneData($id) {
    global $dbh;
//    $sql = "SELECT ptn_website, ptn_allowip, ptn_subdomain, ptn_status FROM partners LEFT JOIN providers ON  WHERE ptn_id = '$id'";
    $sql = "SELECT * FROM partners WHERE ptn_id = '$id'";
//    $sql = "SELECT p.*, group_concat( concat(pes_name,'#',pes_game_code)) as `providers` FROM egamessystem.partners p LEFT JOIN egamessystem.providers ON pes_game_code like concat('%', ptn_pes_code_enabled , '%') WHERE ptn_id = '$id' GROUP BY ptn_id";
    $result = $dbh->exec($sql);
    if($result->hasNext()) {
        $row = $result->next();
        $cloneData = array();
        $cloneData[0] = $row['ptn_website'];
        $cloneData[1] = $row['ptn_allowip'];
        $cloneData[2] = $row['ptn_subdomain'];
        $cloneData[3] = $row['ptn_status'];
        $cloneData[4] = $row['ptn_params'];
        $cloneData[5] = $row['ptn_external_id'];
        $cloneData[6] = $row['ptn_pes_code_enabled'];
        $cloneData[7] = $row['ptn_notes'];
        $cloneData[8] = $row['ptn_force_withdraw'];
        $cloneData[9] = $row['ptn_is_seamless'];
        $cloneData[10] = $row['ptn_client'];

        return json_encode($cloneData);
    } else {
        return -2;
    }
}


/**
 * @param $month
 * @return bool|string
 */
function generateLga($month){

    if(file_exists("../cache/$month.xls")){
        return "File already exists";
    }
    else{
        global $dbh;
        $sql="SELECT  pun_username,
                                  pun_id,
                                  pun_first_name,
                                  pun_last_name,
                                  pun_email,
                                  cms_available_credit,
                                  cms_tot_bet,
                                  cur_name,
                                  cou_name,
                                  pun_dob,
                                  pun_last_logged_in
                          FROM    punter,
                                  customer_monthly_summary,
                                  country,
                                  currency
                          WHERE   cms_pun_id = pun_id
                          AND     cou_id = pun_cou_id
                          AND     cms_cur_code_id = cur_code_id
                          AND     cms_day ='".$month."'
                          GROUP BY pun_id";
        $result=$dbh->exec($sql);
        if($result->getNumRows()>0){
            require_once 'PHPExcel.php';
            require_once 'PHPExcel/Writer/Excel2007.php';
            require_once 'PHPExcel/Style/Fill.php';
            require_once 'PHPExcel/Style/Color.php';
            $rowId = 1;
            $objPHPExcel = new PHPExcel();
            $objPHPExcel->getProperties()->setCreator("CsLiveGames");
            $objPHPExcel->getProperties()->setLastModifiedBy("CsLiveGames");
            $objPHPExcel->getProperties()->setTitle("Monthly Reports for " . $month);
            $objPHPExcel->setActiveSheetIndex(0);
            $objPHPExcel->getActiveSheet()->SetCellValue("A".$rowId, "Username:");
            $objPHPExcel->getActiveSheet()->SetCellValue("B".$rowId, "User code");
            $objPHPExcel->getActiveSheet()->SetCellValue("C".$rowId, "Name ");
            $objPHPExcel->getActiveSheet()->getStyle("A".$rowId.":J".$rowId)->getFont()->getColor()->setARGB(PHPExcel_Style_Color::COLOR_WHITE);
            $objPHPExcel->getActiveSheet()->getStyle("A".$rowId.":J".$rowId)->getFill()->setFillType(PHPExcel_Style_Fill::FILL_SOLID)->getStartColor()->setARGB('FF0000DD');
            $objPHPExcel->getActiveSheet()->SetCellValue("D".$rowId, "Last Name");
            $objPHPExcel->getActiveSheet()->SetCellValue("E".$rowId, "Email");
            $objPHPExcel->getActiveSheet()->SetCellValue("F".$rowId, "Amount");
            $objPHPExcel->getActiveSheet()->SetCellValue("F".$rowId, "Currency");
            $objPHPExcel->getActiveSheet()->SetCellValue("G".$rowId, "Country ");
            $objPHPExcel->getActiveSheet()->SetCellValue("H".$rowId, "Birthday");
            $objPHPExcel->getActiveSheet()->SetCellValue("I".$rowId, "Last Login");
            $objPHPExcel->getActiveSheet()->SetCellValue("J".$rowId, "Total bets");

            while($result->hasNext())
            {
                $rowId++;
                $row=$result->next();
                $objPHPExcel->getActiveSheet()->getColumnDimension('A')->setAutoSize(true);
                $objPHPExcel->getActiveSheet()->getColumnDimension('B')->setAutoSize(true);
                $objPHPExcel->getActiveSheet()->getColumnDimension('C')->setAutoSize(true);
                $objPHPExcel->getActiveSheet()->getColumnDimension('D')->setAutoSize(true);
                $objPHPExcel->getActiveSheet()->getColumnDimension('E')->setAutoSize(true);
                $objPHPExcel->getActiveSheet()->getColumnDimension('F')->setAutoSize(true);
                $objPHPExcel->getActiveSheet()->getColumnDimension('G')->setAutoSize(true);
                $objPHPExcel->getActiveSheet()->getColumnDimension('H')->setAutoSize(true);
                $objPHPExcel->getActiveSheet()->getColumnDimension('I')->setAutoSize(true);
                $objPHPExcel->getActiveSheet()->getColumnDimension('J')->setAutoSize(true);

                $objPHPExcel->getActiveSheet()->SetCellValue("A".$rowId, $row['pun_username']);
                $objPHPExcel->getActiveSheet()->SetCellValue("B".$rowId, $row["pun_id"]);
                $objPHPExcel->getActiveSheet()->SetCellValue("C".$rowId, $row["pun_first_name"]);
                $objPHPExcel->getActiveSheet()->SetCellValue("D".$rowId, $row["pun_last_name"]);
                $objPHPExcel->getActiveSheet()->SetCellValue("E".$rowId, $row["pun_email"]);
                $objPHPExcel->getActiveSheet()->SetCellValue("F".$rowId, $row["cms_available_credit"]);
                $objPHPExcel->getActiveSheet()->SetCellValue("G".$rowId, $row["cur_name"]);
                $objPHPExcel->getActiveSheet()->SetCellValue("H".$rowId, $row["pun_dob"]);
                $objPHPExcel->getActiveSheet()->SetCellValue("I".$rowId, $row["pun_last_logged_in"]);
                $objPHPExcel->getActiveSheet()->SetCellValue("J".$rowId, $row["cms_tot_bet"]);
            }
            $objWriter = new PHPExcel_Writer_Excel2007($objPHPExcel);
            $objWriter->save("../cache/$month.xlsx");
        }
        return true;
    }

}


/**
 * @param $date_end
 * @param $day
 * @param $startrow
 * @param $records
 */
function getLgaRecords($date_end,$day,$startrow,$records){

    global $dbh;
    $sql="SELECT  pun_username,
                                  pun_id,
                                  pun_first_name,
                                  pun_last_name,
                                  pun_email,
                                  cms_available_credit,
                                  cms_tot_bet,
                                  ";
    if(date("n", strtotime($date_end)) != date("n")) {
        $day=date("Y-m-t", strtotime($date_end));
    }

    $sql.=" cur_name,
                                  cou_name,
                                  pun_dob,
                                  pun_last_logged_in
                          FROM    punter,
                                  customer_monthly_summary,
                                  country,
                                  currency
                          WHERE   cms_pun_id = pun_id
                          AND     cou_id = pun_cou_id
                          AND     cms_cur_code_id = cur_code_id
                          AND     cms_day='$day'
                          GROUP BY pun_id
                          LIMIT $startrow,$records";

    $starttime = microtime(true);
    $result=$dbh->exec($sql);
    $endtime = microtime(true);
    $duration = $endtime - $starttime;
    $duration=number_format($duration, 4, ',', '')."s";
    while($result->hasNext())
    {
        $row=$result->next();
        ?>
        <tr>
            <td class="text-success"><p><?=$row['pun_username']?></p></td>
            <td class="text-info"><p><?=$row['pun_id']?></p></td>
            <td class="text-success"><p><?=$row['pun_first_name']?></p></td>
            <td class="text-info"><p><?=$row['pun_last_name']?></p></td>
            <td class="text-success"><p><?=$row['pun_email']?></p></td>
            <td class="text-info"><p class='text-right'><?=getInDollars($row["cms_available_credit"])?></p></td>
            <td class="text-success"><p><?=$row['cur_name']?></p></td>
            <td class="text-info"><p><?=$row['cou_name']?></p></td>
            <td class="text-info"><p><?=$row['pun_dob']?></p></td>
            <td class="text-info"><p><?=$row['pun_last_logged_in']?></p></td>
            <td class="text-info"><p><?=getInDollars($row['cms_tot_bet'])?></p></td>
        </tr>
        <?
    }
    ?>
    <script>duration('<?=$duration?>','duration');</script>
    <?
}


/**
 * @param $day_start
 * @param $day_end
 * @param $startrow
 * @param $records
 */
function getLgaTax($day_start,$day_end,$startrow,$records){
    global $dbh;
    $sql="select pun_username username,
                             ctr_time date,
                             ABS(ctr_amount) bet_amount,
                             ctr_notes note from
                             customer_transaction, punter
                             where ctr_time BETWEEN '$day_start 00:00:00' AND '$day_end 23:59:59'
                             and ctr_pun_id = pun_id
                             and ctr_tty_id = 8
                             and ctr_gam_id = 210
                             and ctr_pme_code = 'CSH'
                             order by ctr_time asc
                             LIMIT $startrow,$records";
    $starttime = microtime(true);
    $result=$dbh->exec($sql);
    $endtime = microtime(true);
    $duration = $endtime - $starttime;
    $duration=number_format($duration, 4, ',', '')."s";
    while($result->hasNext())
    {
        $row=$result->next();
        ?>
        <tr>
            <td class="text-success"><p><?=$row['username']?></p></td>
            <td class="text-info"><p><?=$row['date']?></p></td>
            <td class="text-success"><p><?=getInDollars($row['bet_amount'])?></p></td>
            <td class="text-info"><p><?=$row['note']?></p></td>
        </tr>
        <?
    }
    ?>
    <script>duration('<?=$duration?>','duration');</script>
    <?


}


/**
 * @param $player
 */
function getAllPlayersLike($player){
    global $dbh;
    $jur_class=$_SESSION['jurisdiction_class'];
    $jur_id=$_SESSION['jurisdiction_id'];
    $sql='select pun_username,pun_id from punter ';
    if($jur_class == "club"){
        $sql .= " WHERE pun_betting_club = " . $jur_id."
		 		  GROUP BY ttm_code";
    }
    if($jur_class == "district"){
        $sql .= "JOIN jurisdiction c on c.jur_id = pun_betting_club
					 		JOIN jurisdiction d on d.jur_id = c.jur_parent_id
					 		WHERE d.jur_id = " . $jur_id;
    }
    elseif($jur_class=="region"){
        $sql .="JOIN jurisdiction c on c.jur_id = pun_betting_club
							JOIN jurisdiction d on d.jur_id = c.jur_parent_id
							JOIN jurisdiction r on r.jur_id = d.jur_parent_id
							WHERE r.jur_id=".$jur_id;
    }
    elseif ($jur_class=="nation"){
        $sql .="JOIN jurisdiction c on c.jur_id = pun_betting_club
							JOIN jurisdiction d on d.jur_id = c.jur_parent_id
							JOIN jurisdiction r on r.jur_id = d.jur_parent_id
							JOIN jurisdiction n on n.jur_id = r.jur_parent_id
							WHERE n.jur_id=".$jur_id;
    }
    elseif($jur_class=="casino"){
        $sql .="JOIN jurisdiction c on c.jur_id = pun_betting_club
							JOIN jurisdiction d on d.jur_id = c.jur_parent_id
							JOIN jurisdiction r on r.jur_id = d.jur_parent_id
							JOIN jurisdiction n on n.jur_id = r.jur_parent_id
							JOIN jurisdiction ca on ca.jur_id = n.jur_parent_id
							WHERE ca.jur_id = ".$jur_id;
    }
    $sql .=" AND pun_username like '$player%'";
    $result=$dbh->doCachedQuery($sql,0);
    $return=" ";
    while($result->hasNext()){
        $row=$result->next();
        $return .=$row['pun_id'].";".$row['pun_username'].",";
    }
    $return =substr($return, 0, -1);
    die ($return);
}


/**
 * @param $user
 * @return string
 */
function getUserHistory($user,$days=90){
    global $dbh,$lang;
    $date = date("Y-m-d", strtotime("yesterday - $days days"));
    $totals="<h3>".$lang->getLang("Totals")."</h3>
            <table class='table table-hover table-striped table-bordered'>
                                <thead>
                                <tr class='btn-primary' style='color:#ffffff'>
                                    <th style='color:#ffffff'><strong>".$lang->getLang('Bet')."</strong></th>
                                    <th style='color:#ffffff'><strong>".$lang->getLang('Win')."</strong></th>
                                    <th style='color:#ffffff'><strong>Rake</strong></th>
                                </tr>
                    </thead> ";

    $html ="<h3>".$lang->getLang('Details by days')."</h3>
              <table class='table table-hover table-striped table-bordered'>
                    <thead>
                    <tr class='btn-primary' style='color:#ffffff'>
                        <th style='color:#ffffff'>".$lang->getLang('Date')."</th>
                        <th style='color:#ffffff'><strong>".$lang->getLang('Game')."</strong></th>
                        <th style='color:#ffffff'><strong>".$lang->getLang('Bet')."</strong></th>
                        <th style='color:#ffffff'><strong>".$lang->getLang('Win')."</strong></th>
                        <th style='color:#ffffff'><strong>Rake</strong></th>
                    </tr>
        </thead> ";
    $sql = "SELECT cus_res_day, SUM(cus_res_bet) bets, SUM(cus_res_win) wins,gam_name
                            FROM customer_result_daily
                            JOIN game on gam_id=cus_res_gam_id";

    if($_SESSION['jurisdiction_class']=='casino'){
        $sql.=" JOIN jurisdiction club ON club.jur_id = cus_res_club_id
                JOIN jurisdiction district ON club.jur_parent_id = district.jur_id
                JOIN jurisdiction region ON district.jur_parent_id = region.jur_id
                JOIN jurisdiction nation ON region.jur_parent_id = nation.jur_id
                WHERE 1=1 ";
    }
    if($_SESSION['jurisdiction_class']=='nation'){
        $sql.=" JOIN jurisdiction club ON club.jur_id = cus_res_club_id
                JOIN jurisdiction district ON club.jur_parent_id = district.jur_id
                JOIN jurisdiction region ON district.jur_parent_id = region.jur_id
                JOIN jurisdiction nation ON region.jur_parent_id = nation.jur_id
                        WHERE nation.jur_id=".$_SESSION['jurisdiction_id'];
    }
    if($_SESSION['jurisdiction_class']=='region'){
        $sql.=" JOIN jurisdiction club ON club.jur_id = cus_res_club_id
                JOIN jurisdiction district ON club.jur_parent_id = district.jur_id
                JOIN jurisdiction region ON district.jur_parent_id = region.jur_id
                        WHERE region.jur_id=".$_SESSION['jurisdiction_id'];
    }
    if($_SESSION['jurisdiction_class']=='district'){

        $sql.=" JOIN jurisdiction club ON club.jur_id = cus_res_club_id
                JOIN jurisdiction district ON club.jur_parent_id = district.jur_id
                WHERE district.jur_id=".$_SESSION['jurisdiction_id'];
    }
    if($_SESSION['jurisdiction_class']=='club'){
        $sql.=" JOIN jurisdiction club ON club.jur_id = cus_res_club_id
                WHERE jur_id=".$_SESSION['jurisdiction_id'];
    }
    $sql .=" AND cus_res_day >= '$date' AND cus_res_pun_id='$user'  GROUP BY cus_res_day,gam_id ";

    $rs = $dbh->exec($sql);
    $totbets=0;
    $totwin=0;
    while($rs->hasNext())
    {
        $row=$rs->next();
        $html .="<tr><td>".$row['cus_res_day']."</td>
                     <td>".$row['gam_name']."</td>
                     <td><strong>".getInDollars($row['bets'])."</strong></td>
                     <td><strong>".getInDollars($row['wins'])."</strong></td>
                     <td><strong>".getInDollars($row['bets']-$row['wins'])."</strong></td>
               </tr>";
        $totbets +=$row['bets'];
        $totwin +=$row['wins'];
    }
    $totals.="<tr>
                     <td><strong>".getInDollars($totbets)."</strong></td>
                            <td><strong>".getInDollars($totwin)."</strong></td>
                            <td><strong>".getInDollars($totbets-$totwin)."</strong></td>
               </tr></table><br>";

    $html.="</table>";
    return $totals.$html;
}


/**
 * @param $id
 * @return string
 */
function deleteBanner($id){
    global $dbh;
    $slq='DELETE FROM banner WHERE bnn_id='.$dbh->escape($id);
    return ($dbh->exec($slq) ? "1":"0");
}


/**
 * @param $event
 * @return array
 */
function getEventDetails($event){
    list($user_betting, $pass_betting, $host_betting, $db_betting,$db_port) = explode(":",$_SERVER["BETTING_DSN"]);
    // list($user_betting, $pass_betting, $host_betting, $db_betting) = explode(":",'betting_slave:1321y2egfhqfsah12123:10.15.0.36:betting_lga');
    Db::setConnectionInfo($db_betting,$user_betting,$pass_betting,"mysql",$host_betting,$db_port);
    $sql="Select IDEvento, IDBet, Descrizione from EventiTipi e JOIN Tipi tp on tp.IDTipo = e.IDTipo where IDEvento = ".$event;
    $result = Db::getResult($sql);
    return $result;
}


/**
 * @param $_user
 * @param $_date
 * @return string
 */
function getUserLiveDetails($_user,$_date,$endDate){
    global $dbh,$lang;
    $sql="select ctr_time, ctr_pun_id, ctr_notes, ctr_transaction_num,  case when ctr_tty_id = 8 then ABS(ctr_amount) else 0 end tot_bet,
    case when ctr_tty_id = 7 then ctr_amount else 0 end  tot_win, ctr_balance_available
    from customer_transaction where ctr_time >= '$_date 00:00:00' and ctr_time < '$endDate 23:59:59'
    and ctr_gam_id IN (213,210) 
    and ctr_pun_id = $_user";
    $starttime = microtime(true);

    $rs=$dbh->exec($sql);
    $endtime = microtime(true);
    $duration = $endtime - $starttime;
    $duration=number_format($duration, 4, ',', '')."s";
    $time="<div class='tip fleft'>".$lang->getLang("Time taken to execute your request").":".$duration."</div><br /><br /><br />";
    if($rs->getNumRows()>0){
        $return="<table class='display userlivedetails table table-striped table-bordered table-condensed'>
                                <thead>
                                <tr>
                                    <th>".$lang->getLang("Transaction Id")."</th>
                                    <th>".$lang->getLang("Date")."</th>
                               
                                    <th>".$lang->getLang("Total bet")."</th>
                                    <th>".$lang->getLang("Total win")."</th>                
                                    <th>".$lang->getLang("Balance")."</th>
                                    <th>".$lang->getLang("Notes")."</th>
                                </tr>
                                </thead>";
        while($rs->hasNext()){
            $rec=$rs->next();
            $return .="<tr><td>".$rec['ctr_transaction_num']."</td>";
            $return .="<td>".($rec["ctr_time"])."</td>";
//            $return .="<td>".getInDollars($rec["tot_bet_prematch"])."</td>";
//            $return .="<td>".getInDollars($rec["tot_win_prematch"])."</td>";
//            $return .="<td>".getInDollars($rec["tot_bet_live"])."</td>";
//            $return .="<td>".getInDollars($rec["tot_win_live"])."</td>";
            $return .="<td>".getInDollars($rec["tot_bet"])."</td>";
            $return .="<td>".getInDollars($rec["tot_win"])."</td>";
            $return .="<td>".getInDollars($rec["ctr_balance_available"])."</td>";
            $return .="<td style='text-align:left'>".($rec["ctr_notes"])."</td></tr>";
        }
        $return.="</table>";
    }
    else{
        $return="<div class='alert alert-error'><button type='button' class='close' data-dismiss='alert'>&times;</button><strong>".$lang->getLang('No result found')."</strong></div>";
    }
    $return = $time.$return;

    return $return;
}


/**
 * @param $customer_id
 * @param $trans_type
 * @param $refund_type
 * @param $transaction_number
 * @param $amount
 * @param $reason
 * @param $aus_id
 */
function updateCustomerTrans($customer_id,$trans_type,$refund_type,$transaction_number, $amount, $reason, $aus_id){
    global $dbh;
    $ctr_id = $dbh->nextSequence('ctr_id_seq');
    $sql  = "INSERT INTO customer_transaction";
    $sql .= " ( ctr_id, ctr_pun_id,ctr_balance_available, ctr_amount,  ctr_tty_id, ctr_status, ctr_pme_code";
    $sql .= ",  ctr_aus_id,   ctr_time, ctr_transaction_num, ctr_notes)";
    $sql .= "   VALUES ($ctr_id, $customer_id,(SELECT pcr_credits from punter_credit where pcr_pun_id=$customer_id), $amount, $trans_type";
    $sql .= ", 'completed', '$refund_type', $aus_id, CURRENT_TIMESTAMP";
    $sql .= ", '$transaction_number', '$reason')";
    $dbh->exec($sql);

}


/**
 * @param $customer_id
 * @param $type
 * @param $amount
 * @param $reason
 * @param $aus_id
 * @param string $mngr_id
 * @return string
 */
function doAdjustment ($customer_id, $type, $amount, $reason, $aus_id, $mngr_id="NULL") {
    global $dbh, $console_port,$lang;

    //$dbh->lockRow('punter_credit', array('pcr_pun_id'=>$customer_id));

    switch ($type) {
        case "deposit":
            $trans_type = T_TYPE_CORRECTION;
            $refund_type = "CRN";
            $transaction_number = nextTxnNumberAdjustment();
            updateCustomerTrans($customer_id,$trans_type,$refund_type,$transaction_number, $amount, $reason, $aus_id);
            doDeposit($customer_id, $amount);
            break;
        case "withdrawl":

            $trans_type = T_TYPE_CORRECTION;
            $refund_type = "CRN";
            $transaction_number = nextTxnNumberAdjustment();
            updateCustomerTrans($customer_id,$trans_type,$refund_type,$transaction_number, -$amount, $reason, $aus_id);
            doWithdrawal($customer_id, $amount);
            break;
        case "bonus":

            $trans_type = T_TYPE_CORRECTION;
            $refund_type = "BON";
            $transaction_number = nextTxnBonusAdjustment();
            updateCustomerTrans($customer_id,$trans_type,$refund_type,$transaction_number, $amount, $reason, $aus_id);
            doBonusDeposit($customer_id, $amount);
            break;
        default:
            addError("", $lang->getLang("Invalid type"));
            showErrors();
            exit();
            break;
    }
    addPunterNote($customer_id, $aus_id, "Corrective transaction $type #$transaction_number: $reason " . getInDollars($amount));
    return $transaction_number;
}

function updateTicket($ticketId,$amount,$note,$admin_id){
    global $dbh;
    if($amount>0){
        $status=2;
    }
    else{
        $status=3;
    }
    $sql="Update virtual_game_complaint set
            vgc_status=".$status.",
            vgc_refunded=".$amount.",
            vgc_solving_note=".$dbh->prepareString($note).",
            vgc_resolved_time=now(),
            vgc_aus_id=".$admin_id."
            WHERE vgc_ticket_id=".$dbh->prepareString($ticketId);
    return $dbh->exec($sql);
}


function addTicketResult($res_id,$pun_id,$amount,$game_id,$ticket){
    global $dbh,$lang;
    if($game_id==215 || $game_id==216){
        return true;
    }
    $ticket=  preg_replace('#\D.*$#', '', $ticket);
    $gamenr=$dbh->nextSequence($game_id."_SEQ");
    $sql="insert into punter_result (PRE_RES_ID, PRE_PUN_ID, PRE_TIME, PRE_BET, PRE_WIN, PRE_GAME_NUM, PRE_BALANCE, PRE_RAKE, PRE_BET_STRING,PRE_VOID_DETAILS,PRE_PUN_IP)
             VALUES(".$dbh->prepareString($res_id).",
                 ".$dbh->prepareString($pun_id).",
                 NOW(),
                 0,
                ".$dbh->escape($amount).",
                ".$gamenr.",
                (select pcr_credits from punter_credit where pcr_pun_id=$pun_id),
                ".$dbh->escape($amount).",
                'Refunded;".$dbh->escape($res_id).";$ticket',
                'Refunded for game number ".$dbh->escape($ticket)."',
                 '".getIpAddress()."')";

    $result= $dbh->exec($sql);
    if($result){
        $sql="Update punter_result SET PRE_VOID_DETAILS='Refunded at game number ".$gamenr."'
        where pre_res_id=".$res_id." AND pre_game_num=$ticket ";
        return $dbh->exec($sql);
    }
    else{
        return false;
    }

}

function ticketwasrefunded($ticket){
    global $dbh;
    $sql="Select vgc_status from virtual_game_complaint where VGC_TICKET_ID=".$dbh->prepareString($ticket);
    $refunded=$dbh->queryOne($sql);
    if($refunded==2 || $refunded==3){
        return true;
    }
    return false;
}


function getUserCredits($companies,$totals,$start,$length){
    global $dbh;
    $sql="SELECT pun_username,pun_betting_club, pcr_credits,jur_name
          from punter,punter_credit,jurisdiction
          where  pun_betting_club in (".$dbh->escape($companies).")
          AND pun_id=pcr_pun_id
          AND pun_betting_club=jur_id
          GROUP BY pun_id limit ".$dbh->escape($start).",".$dbh->escape($length);
    $rs=$dbh->exec($sql);
    $users=array();
    while ( $rs->hasNext () ) {
        $section=array();
        $row = $rs->next ();
        $section[0]=$row['pun_username'];
        $section[1]=$row['jur_name'];
        $section[2]=getInDollars($row['pcr_credits']);
        array_push($users,$section);
    }

    return toJsonTable($totals,$users);

}

function getPromotionUsersActive($promotionId,$year,$type){
    global $dbh;
    $year=$dbh->escape(substr($year,0,4));
    $sql="SELECT pun_username,
                jur_name,
                ppr_times_used,
                ppr_total_bonus,
                ppr_bonus_left,
                ppr_bonus_converted_in_real,
                ppr_tot_bet,
                ppr_tot_win,
                pcr_credits,
                pcr_bonus
          FROM punter_promotion
          JOIN promotion on ppr_prm_id=prm_id AND prm_start_date > '".$year."-01-01' AND prm_start_date <= '".$year."-12-31'
          LEFT JOIN punter on ppr_pun_id=pun_id
          LEFT JOIN punter_credit on pcr_pun_id=ppr_pun_id
          LEFT JOIN jurisdiction on jur_id=pun_betting_club
          WHERE ppr_prm_id= ".$dbh->escape($promotionId);
    if($type==2){
        $sql.=" AND ppr_status=2";
    }
    else{
        $sql.=" AND (ppr_status!=2 OR ppr_status IS NULL)";
    }

    $rs=$dbh->exec($sql);
    $users=array();
    while ( $rs->hasNext () ) {
        $section=array();
        $row = $rs->next ();
        $section[0]=$row['pun_username'];
        $section[1]=$row['jur_name'];
        $section[2]=$row['ppr_times_used'];
        $section[3]=getInDollars($row['ppr_total_bonus']);
        $section[4]=getInDollars($row['ppr_bonus_left']);
        $section[5]=getInDollars($row['ppr_bonus_converted_in_real']);
        $section[6]=getInDollars($row['ppr_tot_bet']);
        $section[7]=getInDollars($row['ppr_tot_win']);
        $section[8]=getInDollars($row['pcr_credits']);
        $section[9]=getInDollars($row['pcr_bonus']);
        array_push($users,$section);
    }

    return json_encode($users);
}

function getAffiliateInfo($affId){
    global $dbh;
    $sql="select c.jur_name club_name,c.jur_id club_id,aus_username,act_credits,act_overdraft,aff_id,act_overdraft_start_time,act_tot_overdraft_received,cur_code_for_web
            FROM jurisdiction c
            JOIN admin_user on c.jur_id=aus_jurisdiction_id
            JOIN affiliate on aff_aus_id=aus_id
            JOIN affiliate_credit on act_aff_id=aff_id
            JOIN currency on jur_currency=cur_id
            WHERE c.jur_class='club'
                  AND aff_is_club=1
                  AND aff_id=".$dbh->escape($affId);
    $info=$dbh->queryRow($sql);
    return $info['club_name']."~".getInDollars($info['act_credits'],2,$info["cur_code_for_web"])."~".getInDollars($info['act_overdraft'],2,$info["cur_code_for_web"])."~".getInDollars($info['act_tot_overdraft_received'],2,$info["cur_code_for_web"])."~".$info['act_overdraft_start_time'];
}

/**
 * @param $data
 * @param $url
 * @return mixed
 */
function doBettingRequest($data,$url){
    $webReq = new WebRequest(BETTING_URL.$url);
    $data_json = json_encode($data);
    $options= array(
        "User-Agent: pb2xml",
        "X-Requested-With: XMLHttpRequest");
    $webReq->setOptions($data_json);
    $webReq->setParameter(CURLOPT_HTTPHEADER,$options);
    $webReq->sendRequest();
    $response = $webReq->getResponse();
    return $response;
}


function getInactivePlayers($months=6){
    global $dbh;
    $aColumns = array('pun_username', 'pun_last_logged_in' ,'pcr_credits');
    $sIndexColumn = "pun_username";

    $sTable = "punter LEFT JOIN punter_credit
                ON pun_id=pcr_pun_id " ;
    $sTable.= doJurisdictionCheck();
    $sTable.=" AND pun_last_logged_in < DATE_SUB(NOW(), INTERVAL $months MONTH)
                AND pun_member_type NOT IN ('TOTEM', 'VIRTUAL') ";


    if($_POST['status']==1){
        $sTable.=" AND pcr_credits>0";
    }
    if($_POST['status']==2){
        $sTable.=" AND (pcr_credits=0 OR pcr_credits IS NULL)";
    }
    $sLimit = "";
    if ( isset( $_POST['iDisplayStart'] ) && $_POST['iDisplayLength'] != '-1' )
    {
        $sLimit = "LIMIT ".intval( $_POST['iDisplayStart'] ).", ".
            intval( $_POST['iDisplayLength'] );
    }


    $sOrder = "";
    if ( isset( $_POST['iSortCol_0'] ) )
    {
        $sOrder = "ORDER BY  ";
        for ( $i=0 ; $i<intval( $_POST['iSortingCols'] ) ; $i++ )
        {
            if ( $_POST[ 'bSortable_'.intval($_POST['iSortCol_'.$i]) ] == "true" )
            {
                $sOrder .= "`".$aColumns[ intval( $_POST['iSortCol_'.$i] ) ]."` ".
                    ($_POST['sSortDir_'.$i]==='asc' ? 'asc' : 'desc') .", ";
            }
        }
        $sOrder = substr_replace( $sOrder, "", -2 );
        if ( $sOrder == "ORDER BY" )
        {
            $sOrder = "";
        }
    }

    $sWhere = " ";
    if ( isset($_POST['sSearch']) && $_POST['sSearch'] != ""  )
    {
        $sWhere = "AND ";
        $sWhere .="pun_username  LIKE '%".mysql_real_escape_string( $_POST['sSearch'] )."%' ";
    }
    $sQuery = "
    SELECT SQL_CALC_FOUND_ROWS `".str_replace(" , ", " ", implode("`, `", $aColumns))."`
    FROM   $sTable
    $sWhere
    $sOrder
    $sLimit";

    $rResult = $dbh->exec($sQuery)  ;
    $sQuery = " SELECT FOUND_ROWS() ";
    $rResultFilterTotal = $dbh->queryOne($sQuery) ;
    $iFilteredTotal = $rResultFilterTotal;

    /*  $sQuery = " SELECT COUNT(`".$sIndexColumn."`)
      FROM   $sTable ";
      $rResultTotal =  $dbh->queryOne( $sQuery) ;
      $iTotal = $rResultTotal ;*/

    $output = array(
        "sEcho" => intval($_POST['sEcho']),
        "iTotalRecords" => $rResultFilterTotal,
        "iTotalDisplayRecords" => $iFilteredTotal,
        "aaData" => array()
    );

    $tableArray=array();
    while($rResult->hasNext()){
        $row=$rResult->next();
        $section=array();
        $section[0]=$row['pun_username'];
        $section[1]=$row['pun_last_logged_in'];
        $section[2]=getInDollars($row["pcr_credits"]);
        array_push($tableArray,$section);

    }
    $output['aaData']=$tableArray;
    echo json_encode( $output);
}

function generateInactivePlayersCsv(){
    ini_set('memory_limit', '999M' );
    global $dbh;
    $csv_filename=md5($_SESSION['aus_username']."_csv").".csv";
    unlink("../documents/".$csv_filename);

    $sql=" SELECT pun_username, pcr_credits, pun_last_logged_in from punter,punter_credit
                WHERE pun_id=pcr_pun_id
                AND pun_last_logged_in < DATE_SUB(NOW(), INTERVAL 6 MONTH)
                AND pun_member_type NOT IN ('TOTEM', 'VIRTUAL')";
    $status='All';
    if($_POST['status']==1){
        $sql.=" AND pcr_credits>0";
        $status='With credits';
    }
    if($_POST['status']==2){
        $sql.=" AND (pcr_credits=0 OR pcr_credits IS NULL)";
        $status='Without credits';
    }
    $result = $dbh->doCachedQuery($sql,600)  ;
    $dbh->free();

    $fp = fopen("../documents/".$csv_filename, 'w');
    fputcsv($fp, array("ADMIN: ".$_SESSION['admin_username'],"DATE: ".date('Y-m-d h:i'),"STATUS: ".$status));
    fputcsv($fp, array(" "," "," "));
    fputcsv($fp, array(" "," "," "));
    fputcsv($fp, array("USERNAME","DATE","STATUS"));
    while ( $result->hasNext () ) {
        $row = $result->next ();
        fputcsv($fp, $row);
        unset($result->Records[$result->getCurrentIndex()]);
    }
    fclose($fp);
}


function getPartnerDetails($partnerID,$type='display'){
    global $dbh,$lang;
    $providers=$dbh->doCachedQuery("Select * from providers");
    $partner=$dbh->queryRow("SELECT partners.*,jur_name,jur_id from partners join jurisdiction  on ptn_jur_id=jur_id where ptn_id=".$dbh->escape($partnerID));

    $partner['ptn_allowip']=str_replace("|",',', $partner['ptn_allowip']);
    $currentProviders=explode(",",$partner['ptn_pes_code_enabled']);
    $return="<div style='margin:35px'>
        <h3>".$lang->getLang('Details for partner %',$partner['ptn_name'])."</h3>";
    if($type=='display'){
        $return.="
    <table class='partnerDetails table table-bordered'>
             <tr><td class='blackTd'>Id</td><td>".$partner['ptn_id']."</td></tr>
             <tr><td class='blackTd'>".$lang->getLang('Name')."</td><td>".$partner['ptn_name']."</td></tr>
             <tr><td class='blackTd'>".$lang->getLang('Club')."</td><td>".$partner['jur_name']."</td></tr>
             <tr><td class='blackTd'>".$lang->getLang('Website')."</td><td><span class='partnerDetailsTd'>".$partner['ptn_website']."</span></td></tr>
             <tr><td class='blackTd'>".$lang->getLang('Allowed ip')."</td><td><span class='partnerDetailsTd'>".$partner['ptn_allowip']."</span></td></tr>
             <tr><td class='blackTd'>".$lang->getLang('Key')."</td><td>".$partner['ptn_key']."</td></tr>
             <tr><td class='blackTd'>".$lang->getLang('Subdomain')."</td><td>".$partner['ptn_subdomain']."</td></tr>
             <tr><td class='blackTd'>".$lang->getLang('External Id')."</td><td>".$partner['ptn_external_id']."</td></tr>
             <tr><td class='blackTd'>".$lang->getLang('Force withdraw')."</td><td>".($partner['ptn_force_withdraw']==1?'<span class="text-success">Enabled</span>':'<span class="text-error">Disabled</span>')."</td></tr>
             <tr><td class='blackTd'>".$lang->getLang('Status')."</td><td>".($partner['ptn_status']==1?'<span class="text-success">Enabled</span>':($partner['ptn_status']==2?  '<span class="text-success">Enabled - Forced rollback</span>':'<span class="text-error">Disabled</span>'))."</td></tr>
             <tr><td class='blackTd'>".$lang->getLang('Provider enabled')."</td><td>";
        while($providers->hasNext()){
            $row=$providers->next();
            if (in_array($row['pes_game_code'],$currentProviders) && $row['pes_game_code']!='') {
                $return.=$row['pes_name'].",";
            }
        }
        $return=trim($return, ",");
        $return.="</td></tr>";
             $return.=" <tr><td class='blackTd'>".$lang->getLang('Params')."</td><td>".$partner['ptn_params']."</td></tr>";

        if($partner['ptn_notes']!=''){
            $return.=" <tr><td class='blackTd'>".$lang->getLang('Notes')."</td><td>".$partner['ptn_notes']."</td></tr>";
        }
    }
    else{
        $allClubs=getAllClubsUnderJurisdiction($_SESSION['jurisdiction_id']);
        $clubs="<select name='currentClub' id='currentClub'>";
        while($allClubs->hasNext()){
            $row=$allClubs->next();
            $clubs.="<option value='".$row['club_id']."'".($row['club_id']==$partner['jur_id']?'selected':'')." >".$row['club_name']."</option>";
        }
        $clubs.="</select>";


    // get the ips to be stored separately
        $ipList = explode(",", $partner['ptn_allowip']);

        $return.="
    <table class='partnerDetails table table-bordered'>
             <tr><td class='blackTd'>Id</td><td><span id='currentPartnerId'>".$partner['ptn_id']."</span></td></tr>
             <tr><td class='blackTd'>Name</td><td><input type='text' name='currentPartnerName' id='currentPartnerName' value='".$partner['ptn_name']."'></td></tr>
             <tr><td class='blackTd'>Club</td><td>".$clubs."</td></tr>
             <tr><td class='blackTd'>Website</td><td><input type='text' name='currentPartnerWebsite' id='currentPartnerWebsite' value='".$partner['ptn_website']."'></td></tr>";

        if (count($ipList) == 1) {
            $return .=" <tr><td class='blackTd'>Allowed ip</td><td><div id='ip-field'><div class='entry input-append'><input class='form control serializeIp' name='currentPartnerIp' type='text' value='$ipList[0]'/><button class='btn btn-success edt-btn-add' type='button'><span class='glyphicon glyphicon-plus'>+</span></button></div></div></td></tr>";
        } else {
            $return .=" <tr><td class='blackTd'>Allowed ip</td> <td><div id='ip-field'>";
            for ($i = 0; $i < count($ipList); $i++){
                if ($i == 0)
                    $return .="<div class='entry input-append'><input class='form control serializeIp' name='currentPartnerIp' type='text' value='$ipList[$i]'/><button class='btn btn-success edt-btn-add' type='button'><span class='glyphicon glyphicon-plus'>+</span></button></div>";
                else
                    $return .="<div class='entry input-append'><input class='form control serializeIp' name='currentPartnerIp' type='text' value='$ipList[$i]'/><button class='btn btn-remove btn-danger' type='button'><span class='glyphicon glyphicon-minus'>-</span></button></div>";
            }

            $return .= " </div></td></tr>";
           // $return .=" <tr><td class='blackTd'>Allowed ip</td><td><div id='field'><div class='entry input-append'><input class='form control' name='addParnerIp[]' type='text' placeholder='Allowed Ips'/><button class='btn btn-success btn-add' type='button'><span class='glyphicon glyphicon-plus'>+</span></button></div></div></td></tr>
           //  <tr><td class='blackTd'>Allowed ip</td><td><input type='text' name='currentPartnerIp' id='currentPartnerIp' value='".$partner['ptn_allowip']."'></td></tr>";
        }

             $return .= "<tr><td class='blackTd'>Key</td><td><input type='text' name='currentPartnerKey' id='currentPartnerKey' value='".$partner['ptn_key']."'></td></tr>
             <tr><td class='blackTd'>Subdomain</td><td><input type='text' name='currentPartnerSubdomain' id='currentPartnerSubdomain' value='".$partner['ptn_subdomain']."'></td></tr>
             <tr><td class='blackTd'>External Id</td><td><input type='text' name='currentPartnerEId' id='currentPartnerEId' value=".$partner['ptn_external_id']."></td></tr>
             <tr><td class='blackTd'>Force withdraw</td><td><select name='currentPartnerForceWithdraw' id='currentPartnerForceWithdraw'>
                                <option value='0' class='text-error' ".($partner['ptn_force_withdraw']==0 ?'selected':'').">Disabled</option>
                                <option value='1' ".($partner['ptn_force_withdraw']==1 ?'selected':'')." >Enabled</option>
                            </select></td></tr>
             <tr><td class='blackTd'>Status</td><td>".($partner['ptn_status']==1?'<span class="text-success">Enabled</span>':($partner['ptn_status']==2?  '<span class="text-success">Enabled - Forced rollback</span>':'<span class="text-error">Disabled</span>'))."</td></tr>
             <tr><td class='blackTd'>".$lang->getLang('Providers enabled')."</td><td>
              <select  name='currentPartnerProviders' id='currentPartnerProviders' class='currentPartnerProviders' multiple='multiple' >";
        while($providers->hasNext()){
            $row=$providers->next();
            $return.="<option value='".$row['pes_game_code']."'  ".((in_array($row['pes_game_code'],$currentProviders) && $row['pes_game_code']!='')?'selected':'')." >".$row['pes_name']."</option>";
        }
        $return.="</select><br /><br />
                    <div class=\"alert alert-warning hidden\" style=\"width: 80%\">".$lang->getLang("Partner has a null value")."</div>
                </td></tr>
             <tr><td class='blackTd'>".$lang->getLang('Params')."</td><td>  <input type='text' name='currentPartnerParams' id='currentPartnerParams' value=".$partner['ptn_params']."></td></tr>
             <tr><td class='blackTd'>Is seamless</td><td><select name='currentPartnerIsSeamless' id='currentPartnerIsSeamless'>
                                <option value='0' class='text-error' ".($partner['ptn_is_seamless']==0 ?'selected':'').">False</option>
                                <option value='1' ".($partner['ptn_is_seamless']==1 ?'selected':'')." >True</option>
                            </select></td></tr>
             <tr><td class='blackTd'>".$lang->getLang('Client type')."</td><td><input type='text' name='currentPartnerClient' id='currentPartnerClient' value='".$partner['ptn_client']."'></td></tr>
             <tr><td class='blackTd'>".$lang->getLang('Notes')."</td><td><textarea name='currentPartnerNotes' id='currentPartnerNotes'>".$partner['ptn_notes']."</textarea></td></tr>
             <tr><td colspan=2><button class='btn btn-primary fright updateCurrentPartner'>Update</button></td></tr>";
    }
    $return.=" </table></div>";
    return $return;
}

function getGameLimitDetails($game_limit_id,$username){
    global $dbh,$lang;
    $providers=$dbh->doCachedQuery("Select * from providers");
    $gameLimit=$dbh->queryRow("SELECT pgl.*, COALESCE(pun_username, '-') as username, COALESCE(jur_name, '-') as jurisdiction
                                  FROM egamessystem.punter_games_limit pgl
                                  LEFT JOIN egamessystem.punter ON pgl.pgl_pun_id = pun_id
                                  LEFT JOIN egamessystem.jurisdiction ON pgl.pgl_jur_id = jur_id
                                WHERE pgl_id = $game_limit_id");



    $currentProviders=explode(",",$gameLimit['pgl_pes_code_enabled']);
    $return="<div style='margin:35px'>
            <input type='hidden' id='gamelimit_id' value='".$game_limit_id."'>
            <h3>".$lang->getLang('Details for ' .($gameLimit['username']!='-'?'user':'jurisdiction').' game limit')." - " .($gameLimit['username']!='-'?$gameLimit['username']:$gameLimit['jurisdiction'])."</h3>";
    $return .= "<table class='gameLimitDetails table table-bordered'>";
    $return .= "<tr><td class='blackTd'>".$lang->getLang('Deposit Limit')."</td><td><input type='text' class='inputnumber' name='currentDepositLimit' id='currentDepositLimit' value='".$gameLimit['pgl_deposit_limit']."'></td></tr>
             <tr><td class='blackTd'>".$lang->getLang('Withdraw Limit')."</td><td><input type='text' class='inputnumber' name='currentWithdrawLimit' id='currentWithdrawLimit' value='".$gameLimit['pgl_withdraw_limit']."'></td></tr>
             <tr><td class='blackTd'>".$lang->getLang('Bet Limit')."</td><td><input type='text' class='inputnumber' name='currentBetLimit' id='currentBetLimit' value='".$gameLimit['pgl_bet_limit']."'></td></tr>
             <tr><td class='blackTd'>".$lang->getLang('Win Limit')."</td><td><input type='text' class='inputnumber' name='currentWinLimit' id='currentWinLimit' value='".$gameLimit['pgl_win_limit']."'></td></tr>
             
             <tr><td class='blackTd'>".$lang->getLang('Provider')."</td><td>
              <select  name='currentProvider' id='currentProvider' class='currentProvider' >";
                $return.="<option value='ALL'  ".(($gameLimit['pgl_pes_game_code']=='ALL')?'selected':'')." >ALL</option>";
                while($providers->hasNext()){
                    $row=$providers->next();
                    $return.="<option value='".$row['pes_game_code']."'  ".(($gameLimit['pgl_pes_game_code'] == $row['pes_game_code'])?'selected':'')." >".$row['pes_name']."</option>";
                }
        $return.="</select><br /><br />
                    <div class=\"alert alert-warning hidden\" style=\"width: 80%\">".$lang->getLang("Game limit has a null value")."</div>
                </td></tr>
            
             <tr><td class='blackTd'>".$lang->getLang('Is lock')."</td><td><select name='currentLock' id='currentLock'>
                                <option value='0' class='text-error' ".($gameLimit['pgl_lock']==0 ?'selected':'').">False</option>
                                <option value='1' ".($gameLimit['pgl_lock']==1 ?'selected':'')." >True</option>
                            </select></td></tr>
             <tr><td colspan=2><button class='btn btn-primary fright updateCurrentUserGameLimit'>Update</button></td></tr>";

    $return.=" </table></div>";
    
    return $return;
}

function deleteUserGameLimit($limit_id)
{
    global $dbh;

    $gameLimit=$dbh->queryRow("DELETE from egamessystem.punter_games_limit WHERE pgl_id = $limit_id");

     if($dbh->exec($gameLimit)){
        return '1';
    }
    return '0';

}

function delPartner($partnerID){
    global $dbh;
    $sql="DELETE FROM partners where ptn_id=".$dbh->escape($partnerID);

    if($dbh->exec($sql)){
        return '1';
    }
    return '0';
}


function setPartnerAccess($partnerID,$type){
    global $dbh;
    if($type=='1'){
        $type=0;
    }
    else{
        $type=1;
    }
    if($dbh->exec("Update partners set ptn_status=$type where ptn_id=".$dbh->escape($partnerID))){
        return '1';
    }
    return '0';
}

function updatePartner($partnerID,$name,$club,$site,$ip,$key,$subdomain,$externalId,$forceWithdraw,$note,$providers,$params,$isSeamless,$client){
    global $dbh;
    //var_dump($ip);
    $ipList = explode("&", $ip);
    //var_dump($ipList);
    $ip = "";
    for ($i = 0; $i < count($ipList); $i++) {
        $stringPositionOfEqual = strpos($ipList[$i], "=");
        //var_dump($stringPositionOfEqual);
        $myIp = substr($ipList[$i], $stringPositionOfEqual + 1);
        if(count($ipList) > 1) {
            $ip .= "$myIp|";
        }
        else {
            $ip .= "$myIp";
        }
    }
    if(count($ipList) > 1) {
        $ip = substr($ip, 0, -1);
    }
    //var_dump($ip);
    //$ip=str_replace(",",'|', $ip);
    $sql="Update partners set ptn_name=".$dbh->prepareString($name)
        .",ptn_jur_id=".$dbh->escape($club)
        .",ptn_website=".$dbh->prepareString($site)
        .",ptn_allowip=".$dbh->prepareString($ip)
        .",ptn_key=".$dbh->prepareString($key)
        .",ptn_subdomain=".(!empty($subdomain)? $dbh->prepareString($subdomain) : 'NULL')
        .",ptn_external_id=".$dbh->prepareString($externalId)
        .",ptn_force_withdraw=".$dbh->prepareString($forceWithdraw)
        .",ptn_notes=".$dbh->prepareString($note)
        .",ptn_pes_code_enabled=".$dbh->prepareString($providers)
        .",ptn_params=".$dbh->prepareString($params)
        .",ptn_is_seamless=".$dbh->prepareString($isSeamless)
        .",ptn_client=".$dbh->prepareString($client)
        ." WHERE ptn_id=".$dbh->escape($partnerID);


    if($dbh->exec($sql)){
        return '1';
    }
    return '0';
}



function addPartner($name,$club,$site,$ip,$key,$subdomain,$externalId,$forceWithdraw,$status,$note,$providers,$params,$isSeamless,$client){
    global $dbh,$dbLog;
    require_once 'Jurisdiction.class.inc';
    $sql="SELECT * FROM partners where ptn_name=".$dbh->prepareString($name);
    $result=$dbh->queryRow($sql);
    if($result>0)
    {
        $dbLog->info($result);
        return 'Partner already exists!';
    }
    $ptn=$dbh->queryOne("select last_seq_id('PTN_ID_SEQ')");
    $ip = str_replace(",",'|', $ip);
    if($externalId == '')
    {
        $externalId = $ptn;
    }
    $dbh->begin();
    if($_POST['addNewClub']!='') {
        ob_start();
        $newJurisdiction = new Jurisdiction();
        $newJurisdiction->setName($_POST['club_name']);
        $newJurisdiction->setClass('club');
        $newJurisdiction->setCode($_POST['account_code']);
        $newJurisdiction->setAddress1($_POST['address1']);
        $newJurisdiction->setAddress2($_POST['address2']);
        $newJurisdiction->setParentId($_POST['district']);
        $newJurisdiction->setParentClass('district');
        $newJurisdiction->setPostc($_POST['postc']);
        $newJurisdiction->setPhone($_POST['phone']);
        $newJurisdiction->setNotes($_POST['notes']);
        $newJurisdiction->setHasLive(($_POST['has_live'] == "ALLOW"));
        $newJurisdiction->setCurrency($_POST['currency']);
        $newJurisdiction->setVatCode($_POST["vat_code"]);
        $newJurisdiction->setCountry($_POST['country']);
        $newJurisdiction->setCity($_POST['city']);
        $newJurisdiction->setAccessType($_POST['accessType']);
        $newJurisdiction->setLatitude($_POST['latitude']);
        $newJurisdiction->setLongitude($_POST['longitude']);
        $newJurisdiction->setStartHours($_POST['hoursStart']);
        $newJurisdiction->setEndHours($_POST['hoursEnd']);
        $club=$newJurisdiction->createJurisdiction(false);
        ob_end_clean();
    }

    if(areErrors() || $club<1){

        $dbh->rollback();
        return showErrors();
    }

    $sql="INSERT INTO  partners (ptn_id,ptn_name,ptn_jur_id,ptn_website,ptn_allowip,ptn_key,ptn_subdomain,ptn_external_id,ptn_force_withdraw,ptn_status,ptn_notes,ptn_pes_code_enabled,ptn_params,ptn_is_seamless,ptn_client)"
        ." VALUES(".$ptn.",".$dbh->prepareString($name).",".$dbh->escape($club).",".$dbh->prepareString($site).","
        .$dbh->prepareString($ip).",".$dbh->prepareString($key).",".(!empty($subdomain) ? $dbh->prepareString($subdomain) : 'NULL').",".$dbh->prepareString($externalId).","
        .$dbh->escape($forceWithdraw).",".$dbh->escape($status).","
        .$dbh->prepareString($note).",".$dbh->prepareString($providers).",".$dbh->prepareString($params).",".$dbh->prepareString($isSeamless).",".$dbh->prepareString($client).")";

    if($dbh->exec($sql)){

        $dbh->commit();
        return $ptn;
    }
    $dbh->rollback();
    return '0';
}

function addGameLimit($searchByadd,$searchUseradd,$jurisdiction_level_1,$jurisdiction_id,$deposit_limit_add,$withdraw_limit_add,$bet_limit_add,$win_limit_add,$searchProvideradd,$Lockadd){
    global $dbh,$dbLog;
    require_once 'Jurisdiction.class.inc';



    $dbh->begin();

    if($searchByadd == 1) {
        $sql = "INSERT INTO egamessystem.punter_games_limit 
        (PGL_PUN_ID, PGL_JUR_ID, PGL_DEPOSIT_LIMIT, PGL_WITHDRAW_LIMIT, PGL_BET_LIMIT, PGL_WIN_LIMIT, PGL_PES_GAME_CODE, PGL_LOCK) VALUES 
        ('$searchUseradd',(SELECT pun_betting_club FROM punter WHERE pun_id = $searchUseradd ),'$deposit_limit_add','$withdraw_limit_add','$bet_limit_add','$win_limit_add','$searchProvideradd','$Lockadd')";
    }else if($searchByadd == 2) {

        $jurisdiction = '';
        if ($jurisdiction_level_1 == 'casino') {
            $jurisdiction = 'n.jur_id';
        } else if ($jurisdiction_level_1 == 'nation') {
            $jurisdiction = 'n.jur_id';
        } else if ($jurisdiction_level_1 == 'region') {
            $jurisdiction = 'r.jur_id';
        } else if ($jurisdiction_level_1 == 'district') {
            $jurisdiction = 'd.jur_id';
        } else if ($jurisdiction_level_1 == 'club') {
            $jurisdiction = 'c.jur_id';
        }

        $sql = "INSERT INTO egamessystem.punter_games_limit (PGL_PUN_ID,PGL_JUR_ID, PGL_DEPOSIT_LIMIT, PGL_WITHDRAW_LIMIT, PGL_BET_LIMIT, PGL_WIN_LIMIT, PGL_PES_GAME_CODE, PGL_LOCK)
            VALUES ( 0, $jurisdiction_id, $deposit_limit_add, $withdraw_limit_add, $bet_limit_add, $win_limit_add, '$searchProvideradd', $Lockadd )";
    }

//    die($sql);
//    die($dbh->exec($sql));
    if($dbh->exec($sql)){
        $dbh->commit();
        return '2';
    }
    $dbh->rollback();
    return '0';
}

function updateUserGameLimit($type,$userGameLimit,$currentDepositLimit,$currentWithdrawLimit,$currentBetLimit,$currentWinLimit,$currentProvider,$currentLock)
{
    global $dbh,$dbLog;

        //$ip=str_replace(",",'|', $ip);
    $sql = "UPDATE egamessystem.punter_games_limit SET 
               PGL_DEPOSIT_LIMIT = $currentDepositLimit, 
               PGL_WITHDRAW_LIMIT = $currentWithdrawLimit, 
               PGL_BET_LIMIT = $currentBetLimit, 
               PGL_WIN_LIMIT = $currentWinLimit, 
               PGL_PES_GAME_CODE = '$currentProvider', 
               PGL_LOCK = $currentLock 
            WHERE PGL_ID = $userGameLimit";

    if ($dbh->exec($sql)) {
        return '1';
    }
    return '0';

}

function updateGameLimit($jurisdiction_level_1,$jurisdiction_id,$deposit_limit_add,$withdraw_limit_add,$bet_limit_add,$win_limit_add,$searchProvideradd,$Lockadd){
    global $dbh,$dbLog;
    require_once 'Jurisdiction.class.inc';

//    $jurisdiction = '';
//    if ($jurisdiction_level_1 == 'casino') {
//        $jurisdiction = 'n.jur_id';
//    } else if ($jurisdiction_level_1 == 'nation') {
//        $jurisdiction = 'n.jur_id';
//    } else if ($jurisdiction_level_1 == 'region') {
//        $jurisdiction = 'r.jur_id';
//    } else if ($jurisdiction_level_1 == 'district') {
//        $jurisdiction = 'd.jur_id';
//    } else if ($jurisdiction_level_1 == 'club') {
//        $jurisdiction = 'c.jur_id';
//    }

    $sql_update = "UPDATE egamessystem.punter_games_limit
                    SET PGL_DEPOSIT_LIMIT = $deposit_limit_add, 
                        PGL_WITHDRAW_LIMIT = $withdraw_limit_add, 
                        PGL_BET_LIMIT = $bet_limit_add, 
                        PGL_WIN_LIMIT = $win_limit_add, 
                        PGL_PES_GAME_CODE = '$searchProvideradd', 
                        PGL_LOCK = $Lockadd
                    WHERE pgl_jur_id = $jurisdiction_id";

    if ($dbh->exec($sql_update)) {
        return '1';
    }
    return '0';

}



function sendFinancialReportByEmail($email){
    global $dbh;
    $skinInformation=$dbh->doCachedQuery('SELECT skn_email,skn_smtp from skins,
                    (select coalesce(jur_skn_id,1) jur_skn_id
                        from jurisdiction where jur_id = '.$_SESSION['jurisdiction_id'].'
                    ) t
                    WHERE t.jur_skn_id = skn_id',0 );

    if($skinInformation->getNumRows()<1){
        error_log('Invalid skin settings');
        return -1;
    }
    $skinInformationArr=$skinInformation->Records[0];
    /** Configuration of the email  ****/
    $skinmaildetails=explode('#',$skinInformationArr['skn_email']);
    $userMailSender=$skinmaildetails[0];
    $userMailSenderPsswd=$skinmaildetails[1];
    $mailSmtpHost=$skinInformationArr['skn_smtp'];
    /***** End configuration of the email ***/

    $file="../documents/".md5($_SESSION['admin_username']."financial_partners").".csv";
    $from = $userMailSender;

    $to = $email;
    $subject=  'Partners financial reports';
    $host = $mailSmtpHost;
    $crlf = "\n";
    $headers = array('From' => $from, 'To' => $to, 'Subject' => $subject);
    $mime = new Mail_mime($crlf);
    $html = '<html><body>Partners financial reports in the attached csv</body></html>';

    $mime->setHTMLBody($html);
    $mime->addAttachment($file);
    $body = $mime->get();
    $headers = $mime->headers($headers);
    $smtpArray=array();
    $smtpArray['host']=$host;
    if($userMailSenderPsswd!=''){
        $smtpArray['auth'] = true;
        $smtpArray['username'] = $userMailSender;
        $smtpArray['password'] = $userMailSenderPsswd;
    }
    $smtp = Mail::factory('smtp',$smtpArray);
    $mail = $smtp->send($to, $headers, $body);
    if ( PEAR::isError($mail) )
    {
        $msg="-1";
        error_log("Message not sent:".$mail->getMessage());
    }
    else
    {
        $msg = "1";
    }
    return $msg;
}


function getUserAvailablePromotions($userId,$currency='&euro;'){
    global $dbh,$lang;
    if($userId!=''){
        $sql=" Select ppr_status,ppr_start_date,ppr_end_date,ppr_total_bonus,ppr_bonus_left,ppr_times_used,ppr_initial_credit,ppr_bonus_converted_in_real,ppr_tot_bet,ppr_tot_win,prm_name,prm_description
          from punter_promotion join promotion on ppr_prm_id=prm_id
          where ppr_pun_id=".$dbh->escape($userId);
        $availablePromotions=$dbh->exec($sql);
        if($availablePromotions->getNumRows()>0){
            ?>
            <table class='display table'>
                <thead>
                <tr>
                    <th>Name</th>
                    <th class='no-sort'>Description</th>
                    <th><?=$lang->getLang("Status")?></th>
                    <th class="unwrappable"><?=$lang->getLang("Start date")?></th>
                    <th class="unwrappable"><?=$lang->getLang("End date")?></th>
                    <th>Time used</th>
                    <th>Total bonus</th>
                    <th>Bonus left</th>
                    <th>Bonus real</th>
                    <th>Total bet</th>
                    <th>Total win</th>
                </tr>
                </thead>
                <? while($availablePromotions->hasNext()){
                    $row=$availablePromotions->next();
                    ?>
                    <tr>
                        <td><?=$row['prm_name']?></td>
                        <td><?=$row['prm_description']?></td>
                        <td><?if($row["prm_status"]=='2'){?>
                                <span class="error uppercased"> <?=$lang->getLang('Inactive')?></span>
                            <? }else{?>
                                <span class="result uppercased"> <?=$lang->getLang('Active')?></span>
                            <?}?>
                        </td>
                        <td><?=$row['ppr_start_date'];?></td>
                        <td><?=$row['ppr_end_date'];?></td>

                        <td><?=$row['ppr_times_used']?></td>
                        <td><?=getInDollars($row['ppr_total_bonus'],2,$currency)?></td>
                        <td><?=getInDollars($row['ppr_bonus_left'])?></td>
                        <td><?=getInDollars($row['ppr_bonus_converted_in_real'])?></td>
                        <td><?=getInDollars($row['ppr_tot_bet'])?></td>
                        <td><?=getInDollars($row['ppr_tot_win'])?></td>
                    </tr>

                    <?
                }
                ?>
            </table>
            <?
        }
        else{?>
            <div class="alert alert-danger">
                <?=$lang->getLang("This user has no promotion")?>
            </div>
            <?
        }
        ?>
    <?}else{?>
        <div class="alert alert-danger" >
            <?=$lang->getLang("Please select an user")?>
        </div>
    <? }
    return false;
}


function searchPokerHands2d($request){
    global $dbh, $lang;
    if(empty($request['searchValue'])){ ?>
        <h4 class="alert alert-error fade in"><button type='button' class='close fade in' data-dismiss='alert'>&times;</button><?=$lang->getLang('Username or user id or hand id required');?></h4>
        <?
    }
    else{
        require_once('transcript.inc.php');
        require_once('../poker/common.php');
        $sql = " select pa.* from casinogames.punter pa WHERE 1=1 ";
        if ($request['searchBy'] == "1") {
            $sql .= " AND pun_username=" . $dbh->prepareString($request['searchValue']);
        } elseif ($request['searchBy'] == 2) {
            $sql .= " AND pun_id=".$request['searchValue'] ;
        }
        $customer = $dbh->queryRow($sql, true);

        $_SESSION['poker_user_id'] = $customer['pun_id'];
        $_SESSION['poker_username'] = $customer['pun_username'];

        if($request['groupBy']=='2') {

            /*$sTable ="(poker.punter_cash_table
                join poker.punter_cash_table_hands ON poker.punter_cash_table.`pct_table_id` = poker.punter_cash_table_hands.`pch_pct_id` )";
            $sQuery = " SELECT SQL_CALC_FOUND_ROWS
                poker.punter_cash_table.`pct_pun_id` AS `player_id`,
                poker.punter_cash_table.`pct_id` AS `hand_id`,
                '' AS `ip`,
                poker.punter_cash_table_hands.`pch_user_cards` AS `holecards`,
                poker.punter_cash_table_hands.`pch_common_cards` AS `board_cards`,
                poker.punter_cash_table.`pct_table_id` AS `tablename`,
                poker.punter_cash_table.`pct_total_bet`*100 AS `total_pot`,
                poker.punter_cash_table.`pct_total_rake`*100 AS `total_rake`,
                min(poker.punter_cash_table_hands.`pch_deal_date`) AS `hand_start`,
                max(poker.punter_cash_table_hands.`pch_deal_date`) AS `hand_end`,
                min(poker.punter_cash_table_hands.`pch_deal_id`) AS `first_game_id`,
                max(poker.punter_cash_table_hands.`pch_deal_id`) AS `last_game_id`,
                poker.punter_cash_table.`pct_total_hands` AS `hands_played`,
                poker.punter_cash_table_hands.`pch_bet`*100 AS `bets`,
               poker.punter_cash_table_hands.`pch_win`*100 AS `won`,
                (select count(*) from poker.punter_cash_table_hands prs where prs.`pch_deal_id`=poker.punter_cash_table_hands.`pch_deal_id`) AS nr_players
            from
                 $sTable";*/
            $columns = array('hand_id','hand_start','player_id', 'bets','won','tablename','ip', 'holecards','board_cards','nr_players','total_pot','total_rake' );
            $sTable ="( poker.punter_cash_table_hands )";
            $sQuery = " SELECT SQL_CALC_FOUND_ROWS
                poker.punter_cash_table_hands.`pch_pun_id` AS `player_id`,
                poker.punter_cash_table_hands.`pch_deal_id` AS `hand_id`,
                '' AS `ip`,
                poker.punter_cash_table_hands.`pch_user_cards` AS `holecards`,
                poker.punter_cash_table_hands.`pch_common_cards` AS `board_cards`,
                poker.punter_cash_table_hands.`pch_pct_id` AS `tablename`,
                sum(poker.punter_cash_table_hands.`pch_bet`*100) AS `total_pot`,
                sum(poker.punter_cash_table_hands.`pch_rake`*100) AS `total_rake`,
                min(poker.punter_cash_table_hands.`pch_deal_date`) AS `hand_start`,
                max(poker.punter_cash_table_hands.`pch_deal_date`) AS `hand_end`,
                min(poker.punter_cash_table_hands.`pch_deal_id`) AS `first_game_id`,
                max(poker.punter_cash_table_hands.`pch_deal_id`) AS `last_game_id`,
                poker.punter_cash_table_hands.`pch_bet`*100 AS `bets`,
                poker.punter_cash_table_hands.`pch_win`*100 AS `won`,
                (select count(*) from poker.punter_cash_table_hands prs where prs.`pch_deal_id`=poker.punter_cash_table_hands.`pch_deal_id`) AS nr_players
            from
                 $sTable ";


            if ( isset($request['start']) && $request['length'] != -1 ) {
                $limit = "LIMIT ".intval($request['start']).", ".intval($request['length']);
            }
            $order = '  ';

            if ( isset($request['order']) && count($request['order']) ) {
                $orderBy = array();
                for ( $i=0, $ien=count($request['order']) ; $i<$ien ; $i++ ) {
                    $columnIdx = intval($request['order'][$i]['column']);
                    $requestColumn = $request['columns'][$columnIdx];
                    $column = $columns[ $columnIdx ];

                    if ( $requestColumn['orderable'] == 'true' ) {
                        $dir = $request['order'][$i]['dir'] === 'asc' ?
                            'ASC' :
                            'DESC';
                        $orderBy[] = '`'.$column.'` '.$dir;
                    }
                }
                $order = 'ORDER BY '.implode(', ', $orderBy);
            }

            $sWhere=" WHERE pch_pun_id =" . $_SESSION['poker_user_id'] . "
            and poker.punter_cash_table_hands.`pch_deal_date` BETWEEN '".$request['from'].":00'
            and '".$request['to'].":59'";
            if(isset($request['tblId'])){
                $sWhere.=" and poker.punter_cash_table_hands.pch_pct_id=".$dbh->escape($request['tblId']);
            }

            $extrasWhere='';
            if ( isset($request['search']) && $request['search']['value'] != '' ) {
                $extrasWhere = " AND ";
                $extrasWhere .=" pch_deal_id  LIKE '%".$dbh->escape( $request['search']['value'] )."%' ";
            }

            $group=" group by
            poker.punter_cash_table_hands.pch_deal_id";


            $sQuery .= "
                $sWhere
                 $extrasWhere
                $group
                $order

                $limit";

            $rResult = $dbh->exec($sQuery)  ;
            $sQuery = " SELECT FOUND_ROWS() ";
            $iFilteredTotal = $dbh->queryOne($sQuery) ;
            $rResultFilterTotal = $dbh->queryOne("SELECT count(*) from $sTable $sWhere");

            $tableArray = array();
            while($rResult->hasNext()){
                $row=$rResult->next();
                $row['holecards'] = chunk_split($row['holecards'], 2, ' ');
                $row['board_cards'] = chunk_split($row['board_cards'], 2, ' ');
                $section = array();
                $section[0] = getClassLink("javascript:openWindow('/poker/hand_details.php?hand_id={$row["hand_id"]}&type=2d',
                                                        'hand{$row["hand_id"]}',
                                                        '900','650','0','1')", $row["hand_id"], "contentlink");
                $section[1] = $row["hand_start"];
                $section[2] = getClassLink("javascript:openWindow('" . refPage("customers/customer_view&customer_id={$customer["pun_id"]}") . "','user{$customer["pun_username"]}','950','980','0','1')", $customer["pun_username"], "contentlink");
                $section[3] = getInDollars($row["bets"] / 100);
                $section[4] = getInDollars($row["won"] / 100);
                $section[5] = $row["tablename"];
                $section[6] = long2ip($row["ip"]);
                $section[7] = ShowCards($row['holecards']);
                $section[8] = ShowCards($row['board_cards']);
                $section[9] = $row["nr_players"];
                $section[10] = getInDollars($row["total_pot"] / 100);
                $section[11] = getInDollars($row["total_rake"] / 100);
                array_push($tableArray, $section);
            }

            $output = array(
                "draw" => intval($request['draw']),
                "recordsTotal" => intval($rResultFilterTotal),
                "recordsFiltered" => intval($iFilteredTotal),
                "data" => $tableArray
            );
            echo json_encode($output);

        }
        elseif($request['groupBy']==3){
            $columns = array('pti_tti_id','pti_user_entry_date','pun_username','pti_win_amount','pti_buyin_amount','pti_fee_amount', 'pti_rebuy_amount','pti_rebuy_fee','pti_addon_amount','pti_addon_fee','pti_finish_place','pti_ip_address' );
            $sTable ="( poker.punter_tournament_info left join  casinogames.punter on pun_id=pti_pun_id )";
            $sQuery = "  SELECT SQL_CALC_FOUND_ROWS `".str_replace(" , ", " ", implode("`, `", $columns))."`
                       FROM   $sTable ";

            if ( isset($request['start']) && $request['length'] != -1 ) {
                $limit = "LIMIT ".intval($request['start']).", ".intval($request['length']);
            }
            $order = '  ';

            if ( isset($request['order']) && count($request['order']) ) {
                $orderBy = array();
                for ( $i=0, $ien=count($request['order']) ; $i<$ien ; $i++ ) {
                    $columnIdx = intval($request['order'][$i]['column']);
                    $requestColumn = $request['columns'][$columnIdx];
                    $column = $columns[ $columnIdx ];

                    if ( $requestColumn['orderable'] == 'true' ) {
                        $dir = $request['order'][$i]['dir'] === 'asc' ?
                            'ASC' :
                            'DESC';
                        $orderBy[] = '`'.$column.'` '.$dir;
                    }
                }
                $order = 'ORDER BY '.implode(', ', $orderBy);
            }

            $sWhere=" WHERE pti_pun_id =" . $_SESSION['poker_user_id'] . "
            and poker.punter_tournament_info.`pti_user_entry_date` BETWEEN '".$request['from'].":00'
            and '".$request['to'].":59'";

            $extrasWhere='';
            if ( isset($request['search']) && $request['search']['value'] != '' ) {
                $extrasWhere = " AND ";
                $extrasWhere .=" pti_tti_id  LIKE '%".$dbh->escape( $request['search']['value'] )."%' ";
            }

            $group=" ";

            $sQuery .= "
                $sWhere
                $extrasWhere
                $group
                $order
                $limit";
            $rResult = $dbh->exec($sQuery)  ;
            $sQuery = " SELECT FOUND_ROWS() ";
            $iFilteredTotal = $dbh->queryOne($sQuery) ;
            $rResultFilterTotal = $dbh->queryOne("SELECT count(*) from $sTable $sWhere");

            $tableArray = array();
            while($rResult->hasNext()){
                $row=$rResult->next();
                $section = array();
                $section[0] = $row["pti_tti_id"];
                $section[1] = $row["pti_user_entry_date"];
                $section[2] = getClassLink("javascript:openWindow('" . refPage("customers/customer_view&customer_id={$_SESSION['poker_user_id']}") . "','user{$customer["pun_username"]}','950','980','0','1')", $customer["pun_username"], "contentlink");
                $section[3] = getInDollars($row["pti_win_amount"] / 100);
                $section[4] = getInDollars($row["pti_buyin_amount"] / 100);
                $section[5] = getInDollars($row["pti_fee_amount"] / 100);
                $section[6] = getInDollars($row["pti_rebuy_amount"] / 100);
                $section[7] = getInDollars($row["pti_rebuy_fee"] / 100);
                $section[8] = getInDollars($row["pti_addon_amount"] / 100);
                $section[9] = getInDollars($row["pti_addon_fee"] / 100);
                $section[10] = $row["pti_finish_place"] ;
                $section[11] = getClassLink("javascript:openWindow('".refPage('customers/search&field=4&search_string=' . urlencode(ereg_replace("[^0-9.]", "", $row["pti_ip_address"]))) ."','ip','950','980','0','1')",$row["pti_ip_address"], "contentlink");
                array_push($tableArray, $section);
            }

            $output = array(
                "draw" => intval($request['draw']),
                "recordsTotal" => intval($rResultFilterTotal),
                "recordsFiltered" => intval($iFilteredTotal),
                "data" => $tableArray
            );
            echo json_encode($output);




        }
        else{
            $query= "select pct_table_id, pct_start_date,
            sum(pct_total_hands) pct_total_hands,
            sum(pct_win_hands) pct_win_hands,
            sum(pct_total_bet) pct_total_bet,
            sum(pct_total_win) pct_total_win,
            sum(pct_total_rake) pct_total_rake from
            poker.punter_cash_table
            where
            pct_pun_id =" . $_SESSION['poker_user_id'] . "
            and poker.punter_cash_table.`pct_start_date` > '".$request['from']."'
            and poker.punter_cash_table.`pct_end_date` < '".$request['to']."'
            group by pct_table_id
            order by min(poker.punter_cash_table.`pct_start_date`) desc ";

            $starttime = microtime(true);
            $rs = $dbh->exec($query, false, true);
            $endtime = microtime(true);
            $duration = $endtime - $starttime;
            $duration = number_format($duration, 4, ',', '') . "s";


            if ( $rs->getNumRows()>0 )
            {
                ?>
                <div class="tip fleft"><?=$lang->getLang("Time taken to execute your request")?>: <?=$duration?></div><br /> <br />
                <table class='table display table-hover table-striped table-bordered' id="historyTable" >
                    <thead>
                    <tr>
                        <th class="unwrappable"><?=$lang->getLang("Table")?></th>
                        <th class="unwrappable"><?=$lang->getLang("Date")?></th>
                        <th class="unwrappable"><?=$lang->getLang("User Name")?></th>
                        <th class="unwrappable"><?=$lang->getLang("Total Hands")?></th>
                        <th class="unwrappable"><?=$lang->getLang("Win Hands")?></th>
                        <th class="unwrappable"><?=$lang->getLang("Total bet")?></th>
                        <th class="unwrappable"><?=$lang->getLang("Total win")?></th>
                        <th class="unwrappable"><?=$lang->getLang("Total rake")?></th>
                    </tr>
                    </thead>
                    <tbody>
                    <?
                    while ( $rs->hasNext() )
                    {
                        $row=$rs->next();
                        ?>
                        <tr>
                            <td><?=$row['pct_table_id'];?></td>
                            <td><?=($row["pct_start_date"])?></td>
                            <td> <?=getClassLink("javascript:openWindow('".refPage("customers/customer_view&customer_id={$customer["pun_id"]}")."','user{$customer["pun_username"]}','950','980','0','1')", $customer["pun_username"], "contentlink");?></td>
                            <td><?=$row["pct_total_hands"];?></td>
                            <td><?=$row["pct_win_hands"];?></td>
                            <td><?=getInDollars($row["pct_total_bet"]);?></td>
                            <td><?=getInDollars($row["pct_total_win"]);?></td>
                            <td><?=getInDollars($row["pct_total_rake"]);?></td>
                        </tr>
                        <?
                    }
                    ?>
                    </tbody>
                </table>
                <?
            }
            else{
                ?>
                <h4 class="alert alert-error fade in"><button type='button' class='close fade in' data-dismiss='alert'>&times;</button><?=$lang->getLang('No result found');?></h4>
                <?
            }
        }
    }
}



function _getAccountFinnancialSummary(){
    global $lang;
    require_once 'FinancialCasino.class.inc';
    require_once 'FinancialBetting.class.inc';

    $today=$yesterday = date("Y-m-d", strtotime("-1 day"));
    $lastWeek = date("Y-m-d", strtotime("-1 week"));
    $lastMonth = date("Y-m-d", strtotime("-1 month"));

    $reportYesterday = new FinancialCasino($yesterday, $today);
    $reportLastWeek = new FinancialCasino($lastWeek, $today);
    $reportLastMonth = new FinancialCasino($lastMonth, $today);

    $reportYesterdayBetting = new FinancialBetting($yesterday, $today);
    $reportLastWeekBetting = new FinancialBetting($lastWeek, $today);
    $reportLastMonthBetting = new FinancialBetting($lastMonth, $today);
    ?>
    <table class='table display table-bordered '>
        <thead>
        <tr>
            <th><?= $lang->getLang("Time") ?></th>
            <th><?= $lang->getLang("Casino") ?></th>
            <th><?= $lang->getLang("Live Casino") ?></th>
            <th><?= $lang->getLang("Poker") ?></th>
            <th><?= $lang->getLang("Betting") ?></th>
            <th><?= $lang->getLang("Total") ?></th>
            <th><?= $lang->getLang("Number of players") ?></th>
            <? if ($_SESSION['jurisdiction_class'] != 'casino') { ?>
                <th id="showHideCredits"><?= $lang->getLang("Available credit") ?> <span>&#9660;</span></th>
            <? } ?>
        </tr></thead>
        <tr>

            <td>
                <table>
                    <tr>
                        <td style="border-left: 0">Yesterday<br/>( <span class="tip"><?= $yesterday ?>
                                - <?= $today ?></span> )
                        </td>
                    </tr>
                    <tr>
                        <td style="border-left: 0">Last Week<br/>( <span class="tip"><?= $lastWeek ?> - <?= $today ?></span>
                            )
                        </td>
                    </tr>
                    <tr>
                        <td style="border-left: 0">Last Month<br/>( <span class="tip"><?= $lastMonth ?>
                                - <?= $today ?></span> )
                        </td>
                    </tr>
                </table>
            </td>
            <td>
                <table>
                    <tr>
                        <td style="border-left: 0"><?= $reportYesterday->getCasinoNet(true) ?><br/><br/></td>
                    </tr>
                    <tr>
                        <td style="border-left: 0"><?= $reportLastWeek->getCasinoNet(true) ?><br/><br/></td>
                    </tr>
                    <tr>
                        <td style="border-left: 0"><?= $reportLastMonth->getCasinoNet(true) ?></td>
                    </tr>
                </table>
            </td>
            <td>
                <table style="width:100%">
                    <tr>
                        <td style="border-left: 0"><?= $reportYesterday->getCategoryNet('casino_live', true) ?><br/><br/>
                        </td>
                    </tr>
                    <tr>
                        <td style="border-left: 0"><?= $reportLastWeek->getCategoryNet('casino_live', true) ?><br/><br/>
                        </td>
                    </tr>
                    <tr>
                        <td style="border-left: 0"><?= $reportLastMonth->getCategoryNet('casino_live', true) ?></td>
                    </tr>
                </table>
            </td>
            <td>
                <table style="width:100%">
                    <tr>
                        <td style="border-left: 0"><?= $reportYesterday->getPokerNet(true) ?><br/><br/></td>
                    </tr>
                    <tr>
                        <td style="border-left: 0"><?= $reportLastWeek->getPokerNet(true) ?><br/><br/></td>
                    </tr>
                    <tr>
                        <td style="border-left: 0"><?= $reportLastMonth->getPokerNet(true) ?></td>
                    </tr>
                </table>
            </td>
            <td>
                <table style="width:100%">
                    <tr>
                        <td style="border-left: 0"><?= $reportYesterdayBetting->getTotalsByClass($_SESSION['jurisdiction_class'], true) ?>
                            <br/><br/></td>
                    </tr>
                    <tr>
                        <td style="border-left: 0"><?= $reportLastWeekBetting->getTotalsByClass($_SESSION['jurisdiction_class'], true) ?>
                            <br/><br/></td>
                    </tr>
                    <tr>
                        <td style="border-left: 0"><?= $reportLastMonthBetting->getTotalsByClass($_SESSION['jurisdiction_class'], true) ?></td>
                    </tr>
                </table>
            </td>
            <td>
                <table style="width:100%">
                    <tr>
                        <td style="border-left: 0"><?= getInDollars($reportYesterdayBetting->getTotalsByClass($_SESSION['jurisdiction_class']) + $reportYesterday->getPokerNet() + $reportYesterday->getCategoryNet('casino_live') + $reportYesterday->getCasinoNet()) ?>
                            <br/><br/></td>
                    </tr>
                    <tr>
                        <td style="border-left: 0"><?= getInDollars($reportLastWeekBetting->getTotalsByClass($_SESSION['jurisdiction_class']) + $reportLastWeek->getPokerNet() + $reportLastWeek->getCategoryNet('casino_live') + $reportLastWeek->getCasinoNet()) ?>
                            <br/><br/></td>
                    </tr>
                    <tr>
                        <td style="border-left: 0"><?= getInDollars($reportLastMonthBetting->getTotalsByClass($_SESSION['jurisdiction_class']) + $reportLastMonth->getPokerNet() + $reportLastMonth->getCategoryNet('casino_live') + $reportLastMonth->getCasinoNet()) ?></td>
                    </tr>
                </table>
            </td>
            <td>
                <table style="width:100%">
                    <tr>
                        <td style="border-left: 0"><?= getNumberOfNewPlayers($yesterday, $today); ?><br/><br/></td>
                    </tr>
                    <tr>
                        <td style="border-left: 0"><?= getNumberOfNewPlayers($lastWeek, $today); ?><br/><br/></td>
                    </tr>
                    <tr>
                        <td style="border-left: 0"><?= getNumberOfNewPlayers($lastMonth, $today); ?></td>
                    </tr>
                </table>
            </td>
            <? if ($_SESSION['jurisdiction_class'] != 'casino') { ?>
                <td style=" text-align: left;">
                    <table style="width:100%" class="showHideCredits">
                        <tr>
                            <td style="border-left: 0">Credit : <?= getInDollars($_SESSION['jurisdiction_credit']); ?>
                                <br/><br/></td>
                        </tr>
                        <tr>
                            <td style="border-left: 0">Overdraft
                                : <?= getInDollars($_SESSION['jurisdiction_overdraft']); ?></td>
                        </tr>
                    </table>
                </td>
            <? } ?>


        </tr></table>
    <?
}

function getUsers($username=null,$email=null,$exact=false){
    global $dbh;
    include_once 'JurisdictionsList.class.inc';
    $jurisdiction = JurisdictionsList::getInstance ( $dbh );
    $clubs=implode(",",$jurisdiction->getChilJurisdictionsIdList($_SESSION['jurisdiction_id'],'club'));
    //Select punter details record using punter id
    $sql  = "select pun_username as username ,pun_id as id , pun_customer_number as customer_number from punter, punter_credit,jurisdiction, currency where 1=1  ";
    if($username){
        $sql .= " AND pun_username  ".($exact? "=".$dbh->prepareString($username) : "like '%".$dbh->escape($username)."%'");
   }
    if($email){
        $sql .= " AND pun_email  ".($exact? "=".$dbh->prepareString($email) : "like '%".$dbh->escape($email)."%'");
    }
    $sql .= " and pun_id = pcr_pun_id and jur_id=pun_betting_club and jur_currency=cur_id  AND pun_betting_club in (".$dbh->escape($clubs).") and pun_id = pcr_pun_id limit 0,50";
    $rs = $dbh->exec($sql);

    return $rs;
}

function getUsersId($username=null,$exact=false){
    global $dbh;
    include_once 'JurisdictionsList.class.inc';
    $jurisdiction = JurisdictionsList::getInstance ( $dbh );
    //Select punter details record using punter id
    $sql  = "select pun_username as username ,pun_id as id from punter where 1=1  ";
    if($username){
        $sql .= " AND pun_username like '%".$dbh->escape($username)."%'";
    }
    if($username && $exact==1){
        $sql .= " AND pun_username = '".$dbh->escape($username)."'";
    }

    $rs = $dbh->exec($sql);

    return $rs;
}


function getUserSession($username,$extuid){
    $data="operationtype=3023&uname=".$username."&extuid=".$extuid."&opt=1";
    $webReq = new WebRequest(WEB_APP_WEBSITES);
    $webReq->setOptions($data);
    $webReq->sendRequest();
    return $webReq->getResponse();
}




function getChipDumpingClients($request){
    global $dbh, $lang;

            $columns = array('clt_id','clt_name','clt_type', 'clt_registration');
            $sTable ="( poker.clients )";
            $sQuery = " SELECT SQL_CALC_FOUND_ROWS 
             poker.clients.* 
            from
                 $sTable ";


            if ( isset($request['start']) && $request['length'] != -1 ) {
                $limit = "LIMIT ".intval($request['start']).", ".intval($request['length']);
            }
            $order = '  ';

            if ( isset($request['order']) && count($request['order']) ) {
                $orderBy = array();
                for ( $i=0, $ien=count($request['order']) ; $i<$ien ; $i++ ) {
                    $columnIdx = intval($request['order'][$i]['column']);
                    $requestColumn = $request['columns'][$columnIdx];
                    $column = $columns[ $columnIdx ];

                    if ( $requestColumn['orderable'] == 'true' ) {
                        $dir = $request['order'][$i]['dir'] === 'asc' ?
                            'ASC' :
                            'DESC';
                        $orderBy[] = '`'.$column.'` '.$dir;
                    }
                }
                $order = 'ORDER BY '.implode(', ', $orderBy);
            }

            $sWhere=" WHERE 1=1 ";

            $extrasWhere='';
            if ( isset($request['search']) && $request['search']['value'] != '' ) {
                $extrasWhere = " AND ";
                $extrasWhere .=" (clt_name  LIKE '%".$dbh->escape( $request['search']['value'] )."%' OR ";
                $extrasWhere .=" clt_type  LIKE '%".$dbh->escape( $request['search']['value'] )."%' OR ";
                $extrasWhere .=" clt_registration  LIKE '%".$dbh->escape( $request['search']['value'] )."%' ) ";
            }

            $group=" ";


            $sQuery .= "
                $sWhere
                 $extrasWhere
                $group
                $order

                $limit";

            $rResult = $dbh->exec($sQuery)  ;
            $sQuery = " SELECT FOUND_ROWS() ";
            $iFilteredTotal = $dbh->queryOne($sQuery) ;
            $rResultFilterTotal = $dbh->queryOne("SELECT count(*) from $sTable $sWhere");

            $tableArray = array();
            while($rResult->hasNext()){
                $row=$rResult->next();
                $section = array();
                $section[0] = $row["clt_id"];
                $section[1] = $row["clt_name"];
                $section[2] = ($row["clt_type"]==1?'Partner':($row["clt_type"]==3?'Nation':'Club'));
                $section[3] = $row["clt_registration"];
                $section[4] = "<button class='btn modifyClients btn-small btn-info' type='button' >Modify</button>";

                array_push($tableArray, $section);
            }

            $output = array(
                "draw" => intval($request['draw']),
                "recordsTotal" => intval($rResultFilterTotal),
                "recordsFiltered" => intval($iFilteredTotal),
                "data" => $tableArray
            );
            echo json_encode($output);



}


function saveChipDumpingClient($clientName,$clientType,$clientId=null){
    global $dbh;
    if($clientId&& $clientId!=''){
        $sql="Update poker.clients set clt_name=".$dbh->prepareString($clientName).", clt_type=".$dbh->escape($clientType)." WHERE clt_id=".$dbh->escape($clientId);
    }
    else{
        $sql  = "INSERT INTO  poker.clients";
        $sql .= " ( clt_name, clt_type)";
        $sql .= "   VALUES (".$dbh->prepareString($clientName).",".$dbh->escape($clientType).")";
    }
    return $dbh->exec($sql);

}


function getChipDumping($request){
    global $dbh, $lang;

    $columns = array('cpd_id','cpd_date','cpd_user_clt_id','cpd_username','clt_type','cpd_amount','cpd_receive_clt_id','cpd_receive_from_uname','cpd_give_to_uname','cpd_give_to_clt_id');
    $sTable ="( poker.chip_dumping left join poker.clients on clt_id=cpd_clt_id )";
    $sQuery = " SELECT SQL_CALC_FOUND_ROWS 
             poker.chip_dumping.*, (SELECT clt_name from poker.clients where cpd_user_clt_id=clt_id) as cpd_user_clt_id,
             (SELECT clt_name from poker.clients where cpd_receive_clt_id=clt_id) as cpd_receive_clt_id,
             (SELECT clt_name from poker.clients where cpd_give_to_clt_id=clt_id) as cpd_give_to_clt_id
            from
                 $sTable ";


    if ( isset($request['start']) && $request['length'] != -1 ) {
        $limit = "LIMIT ".intval($request['start']).", ".intval($request['length']);
    }
    $order = '  ';

    if ( isset($request['order']) && count($request['order']) ) {
        $orderBy = array();
        for ( $i=0, $ien=count($request['order']) ; $i<$ien ; $i++ ) {
            $columnIdx = intval($request['order'][$i]['column']);
            $requestColumn = $request['columns'][$columnIdx];
            $column = $columns[ $columnIdx ];

            if ( $requestColumn['orderable'] == 'true' ) {
                $dir = $request['order'][$i]['dir'] === 'asc' ?
                    'ASC' :
                    'DESC';
                $orderBy[] = '`'.$column.'` '.$dir;
            }
        }
        $order = 'ORDER BY '.implode(', ', $orderBy);
    }

    $sWhere=" WHERE 1=1 ";

    $extrasWhere='';
    if ( isset($request['search']) && $request['search']['value'] != '' ) {
        $extrasWhere .= " AND ";
        $extrasWhere .=" (cpd_username  LIKE '%".$dbh->escape( $request['search']['value'] )."%' OR ";
        $extrasWhere .=" cpd_receive_clt_id  LIKE '%".$dbh->escape( $request['search']['value'] )."%' OR ";
        $extrasWhere .=" cpd_give_to_clt_id  LIKE '%".$dbh->escape( $request['search']['value'] )."%'  OR ";
        $extrasWhere .=" cpd_user_clt_id  LIKE '%".$dbh->escape( $request['search']['value'] )."%' OR  ";
        $extrasWhere .=" cpd_date  LIKE '%".$dbh->escape( $request['search']['value'] )."%'  OR ";
        $extrasWhere .=" cpd_receive_from_uname  LIKE '%".$dbh->escape( $request['search']['value'] )."%'  OR ";
        $extrasWhere .=" cpd_give_to_uname  LIKE '%".$dbh->escape( $request['search']['value'] )."%'  OR ";
        $extrasWhere .=" cpd_date  LIKE '%".$dbh->escape( $request['search']['value'] )."%' )";
    }
    if ( isset($request['jur']) && $request['jur'] != '' ) {
        $extrasWhere .= " AND ";
        $extrasWhere .=" cpd_clt_id  =".$dbh->escape( $request['jur'])." ";
    }
    if ( isset($request['type']) && $request['type'] != '' ) {
        $extrasWhere .= " AND ";
        $extrasWhere .=" clt_type  =".$dbh->escape( $request['type'])." ";
    }

    if ( isset($request['startDate']) && $request['startDate'] != '' ) {
        $extrasWhere .= " AND ";
        $extrasWhere .=" cpd_date  >= ".$dbh->prepareString( $request['startDate'])." ";
    }
    if ( isset($request['endDate']) && $request['endDate'] != '' ) {
        $extrasWhere .= " AND ";
        $extrasWhere .=" cpd_date  <= ".$dbh->prepareString( $request['endDate'])." ";
    }

    $group=" ";




    $sQuery .= "
                $sWhere
                 $extrasWhere
                $group
                $order

                $limit";

    $rResult = $dbh->exec($sQuery)  ;
    $sQuery = " SELECT FOUND_ROWS() ";
    $iFilteredTotal = $dbh->queryOne($sQuery) ;
    $rResultFilterTotal = $dbh->queryOne("SELECT count(*) from $sTable $sWhere");

    $tableArray = array();
    while($rResult->hasNext()){
        $row=$rResult->next();
        $section = array();
        $section[0] = $row["cpd_id"];
        $section[1] = $row["cpd_date"];
        $section[2] = $row["cpd_user_clt_id"];
        $section[3] = $row["cpd_username"];
        $section[4] = ($row["cpd_give_money"]==1?'Give':'Receive');
        $section[5] = $row["cpd_amount"];
        $section[6] = $row["cpd_receive_clt_id"];
        $section[7] = $row["cpd_receive_from_uname"];
        $section[8] = $row["cpd_give_to_clt_id"];
        $section[9] = $row["cpd_give_to_uname"];

        $section[10] = "<div class='btn-group'><button class='btn btn-info btn-small btnModify' type='button'>Modify</button><button class='btn btn-danger btn-small btnDelete'>Delete</button> </div>";

        array_push($tableArray, $section);
    }

    $output = array(
        "draw" => intval($request['draw']),
        "recordsTotal" => intval($rResultFilterTotal),
        "recordsFiltered" => intval($iFilteredTotal),
        "data" => $tableArray
    );
    echo json_encode($output);



}
function getChipDumpingTotals($request){
    global $dbh, $lang;

    $sTable ="( poker.chip_dumping left join poker.clients on clt_id=cpd_clt_id )";
    $sQuery = " SELECT sum(if(cpd_give_money=1,cpd_amount,0)) give_money , sum(if(cpd_receive_money=1,cpd_amount,0)) receive_money
              from
                 $sTable ";
    $sWhere=" WHERE 1=1 ";
    $extrasWhere='';
    if ( isset($request['jur']) && $request['jur'] != '' ) {
        $extrasWhere .= " AND ";
        $extrasWhere .=" cpd_clt_id  =".$dbh->escape( $request['jur'])." ";
    }
    if ( isset($request['type']) && $request['type'] != '' ) {
        $extrasWhere .= " AND ";
        $extrasWhere .=" clt_type  =".$dbh->escape( $request['type'])." ";
    }

    if ( isset($request['startDate']) && $request['startDate'] != '' ) {
        $extrasWhere .= " AND ";
        $extrasWhere .=" cpd_date  >= ".$dbh->prepareString( $request['startDate'])." ";
    }
    if ( isset($request['endDate']) && $request['endDate'] != '' ) {
        $extrasWhere .= " AND ";
        $extrasWhere .=" cpd_date  <= ".$dbh->prepareString( $request['endDate'])." ";
    }
    $sQuery .= "  $sWhere
                 $extrasWhere
               ";

    $result = $dbh->exec($sQuery)  ;

   ?>
    <table class="table table-striped display">
    <thead>
    <tr>
        <th><?=$lang->getLang("Total Give")?></th>
        <th><?=$lang->getLang("Total Receive")?></th>
        <th><?=$lang->getLang("Difference")?></th>
        </tr>
    </thead>
    <tbody>
    <tr>
        <td><?=getInDollars($result->Records[0]['give_money'])?></td>
        <td><?=getInDollars($result->Records[0]['receive_money'])?></td>
        <td><?=getInDollars($result->Records[0]['give_money']-$result->Records[0]['receive_money'])?></td>
    </tr>
    </tbody>
    </table>
    <?
    die();
}
function saveChipDumping($jurisdiction,$username,$user,$from,$receivefrom,$to,$giveto,$type,$amount,$date,$id=null){
    global $dbh;
      if($id==null) {
          $sql = "INSERT INTO  poker.chip_dumping";
          $sql .= " ( cpd_clt_id,cpd_username,cpd_user_clt_id,cpd_give_money,cpd_receive_money,cpd_amount,cpd_date,cpd_receive_from_uname,cpd_receive_clt_id,cpd_give_to_uname,cpd_give_to_clt_id)";
          $sql .= "   VALUES (" . $dbh->prepareString($jurisdiction) .
              "," . $dbh->prepareString($username) . ","
              . $dbh->escape($user) . ","
              . (($type == 'give') ? 1 : 0) . ","
              . (($type == 'receive') ? 1 : 0) . ","
              . $dbh->prepareString($amount) . ","
              . $dbh->prepareString($date) . ","
              . $dbh->prepareString($receivefrom) . ","
              . $dbh->prepareString($from) . ","
              . $dbh->prepareString($giveto) . ","
              . $dbh->prepareString($to) . ")";
      }
      else{
          $sql=" Update poker.chip_dumping set
      cpd_username=". $dbh->prepareString($username).",
      cpd_user_clt_id=" . $dbh->escape($user) .",
      cpd_give_money=". (($type == 'give') ? 1 : 0) . ",
      cpd_receive_money=". (($type == 'receive') ? 1 : 0) . ",
      cpd_amount=".$dbh->prepareString($amount).",
      cpd_date=".$dbh->prepareString($date).",
      cpd_receive_from_uname=".$dbh->prepareString($receivefrom).",
      cpd_receive_clt_id=". $dbh->prepareString($from).",
      cpd_give_to_uname=".$dbh->prepareString($giveto).",
      cpd_give_to_clt_id=".$dbh->prepareString($to)." 
      Where cpd_id=".$dbh->escape($id)."
      ";
      }
    return $dbh->exec($sql);

}

function generateChipDumpingReport($request){
    global $dbh, $lang;

    $columns = array('cpd_id','cpd_username','cpd_amount','cpd_date','cpd_receive_from_uname','cpd_give_to_uname');
    $sTable ="( poker.chip_dumping )";
    $sQuery = " SELECT SQL_CALC_FOUND_ROWS 
             poker.chip_dumping.*, (SELECT clt_name from poker.clients where cpd_user_clt_id=clt_id) as cpd_user_clt_id,
             (SELECT clt_name from poker.clients where cpd_receive_clt_id=clt_id) as cpd_receive_clt_id,
             (SELECT clt_name from poker.clients where cpd_give_to_clt_id=clt_id) as cpd_give_to_clt_id
            from
                 $sTable ";


    /*if ( isset($request['start']) && $request['length'] != -1 ) {
        $limit = "LIMIT ".intval($request['start']).", ".intval($request['length']);
    }*/
    $order = '  ';
    $limit = '  ';



    $sWhere=" WHERE 1=1 ";

    $extrasWhere='';

    if ( isset($request['jur']) && $request['jur'] != '' ) {
        $extrasWhere .= " AND ";
        $extrasWhere .=" cpd_clt_id  =".$dbh->escape( $request['jur'])." ";
    }
    if ( isset($request['month']) && $request['month'] != '' ) {
        $extrasWhere .= " AND ";
        $extrasWhere .=" cpd_date  >= ".$dbh->prepareString( $request['month']."-01")." AND ";
        $extrasWhere .=" cpd_date  <= ".$dbh->prepareString( $request['month']."-31")." ";
    }

    $group=" ";




    $sQuery .= "
                $sWhere
                 $extrasWhere
                $group
                $order

                $limit";

    $rResult = $dbh->exec($sQuery)  ;
    $sQuery = " SELECT FOUND_ROWS() ";
    $iFilteredTotal = $dbh->queryOne($sQuery) ;
    $rResultFilterTotal = $dbh->queryOne("SELECT count(*) from $sTable $sWhere");

    $tableArray = array();
    while($rResult->hasNext()){
        $row=$rResult->next();
        $section = array();
        $section[0] = $row["cpd_username"];
        $section[1] = $row["cpd_user_clt_id"];
        $section[2] = ($row["cpd_give_money"]==1?$row["cpd_amount"]:'');
        $section[3] = ($row["cpd_receive_money"]==1?$row["cpd_amount"]:'');
        $section[4] = ($row["cpd_give_money"]==1?$row["cpd_amount"]:-$row["cpd_amount"]);
        $section[5] = $row["cpd_date"];
        $section[6] = $row["cpd_receive_from_uname"];
        $section[7] = $row["cpd_receive_clt_id"];
        $section[8] = $row["cpd_give_to_uname"];
        $section[9] = $row["cpd_give_to_clt_id"];



        array_push($tableArray, $section);
    }

    $output = array(
        "draw" => intval($request['draw']),
        "recordsTotal" => intval($rResultFilterTotal),
        "recordsFiltered" => intval($iFilteredTotal),
        "data" => $tableArray
    );
    echo json_encode($output);

}


//function getUserSession($username,$extuid){
//    $data="operationtype=3023&uname=".$username."&extuid=".$extuid."&opt=1";
//    $webReq = new WebRequest(WEB_APP_WEBSITES);
//    $webReq->setOptions($data);
//    $webReq->sendRequest();
//    return $webReq->getResponse();
//}

function isJSON($string){
    return is_string($string) && is_array(json_decode($string, true)) ? true : false;
}

//TODO(Alexa): Incorporate this
function addPartnerCurlRequest($url, $partnerId) {
    $data="operationcode=2011&op=1&sername=partner&auxid=$partnerId";
    $webReq = new WebRequest($url);
    $webReq->setOptions($data);
    $webReq->sendRequest();
    return  $webReq->getResponse();
}

function editPartnerCurlRequest($url, $partnerId) {
    $data="operationcode=2011&op=0&sername=partner&auxid=$partnerId";
    $webReq = new WebRequest($url);
    $webReq->setOptions($data);
    $webReq->sendRequest();
    return  $webReq->getResponse();
}

function addProviderCurlRequest($url, $providerId) {
    $data="operationcode=2011&op=1&sername=provider&auxid=$providerId";
    $webReq = new WebRequest($url);
    $webReq->setOptions($data);
    $webReq->sendRequest();
    return  $webReq->getResponse();
}

function editProviderCurlRequest($url, $providerId) {
    $data="operationcode=2011&op=0&sername=provider&auxid=$providerId";
    $webReq = new WebRequest($url);
    $webReq->setOptions($data);
    $webReq->sendRequest();
    return  $webReq->getResponse();
}

function addSkinCurlRequest($url, $skinId) {
    $data="operationcode=2011&op=1&sername=skins&auxid=$skinId";
    $webReq = new WebRequest($url);
    $webReq->setOptions($data);
    $webReq->sendRequest();
    return  $webReq->getResponse();
}

function editSkinCurlRequest($url, $skinId) {
    $data="operationcode=2011&op=0&sername=skins&auxid=$skinId";
    $webReq = new WebRequest($url);
    $webReq->setOptions($data);
    $webReq->sendRequest();
    return  $webReq->getResponse();
}

?>
