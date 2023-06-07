<script src="<?= secure_host ?>/media/ui/js/jquery.timepicker.js" type="text/javascript"></script>
<script type="text/javascript">
    $(function() {
        $( "#from" ).datetimepicker({
            changeMonth: true,
            numberOfMonths:1,
            onChangeMonthYear:function(year, month, i){
                var day = i.selectedDay;
                $(this).datepicker('setDate', new Date(year, month-1, day));
            }

        });
        $( "#to" ).datetimepicker({
            changeMonth: true,
            numberOfMonths:1,
            onChangeMonthYear:function(year, month, i){
                var day = i.selectedDay;
                $(this).datepicker('setDate', new Date(year, month-1, day));
            }

        });
    });

</script>
<!--<div id="video" style="float:right;position:absolute;right:30px;top:140px;" align="left">
    <iframe src="" id="videoframe" width="340" height="258" frameborder="0">
        <p><?/*=$lang->getLang("Your browser does not support iframes")*/?></p>
    </iframe>
</div>-->

<?php
$dbh = new DatabaseHandler();
$dbh->connect();
include_once('transcript.inc.php');
//Modificata Manuel Rinaldi
//Modificata Marian 2013-04-11
define("SEARCH_MAX_HOURS", 40);
globalise('action');
globalise('username');
globalise('userid');
globalise('from');
globalise('to');
globalise('tbl_id');
globalise('tbl_name');
globalise('handid');
globalise('games');

if($start_time=strtotime($from)){
    $from = date("Y-m-d H:i", $start_time);
}else{
    $from = date("Y-m-d H:i", strtotime("now - 2 hours"));
}

if($end_time=strtotime($to)){
    $to = date("Y-m-d H:i", $end_time);
}else{
    $to = date("Y-m-d H:i", strtotime("now"));
}

if(($end_time - $start_time) / 60 / 60 > SEARCH_MAX_HOURS){
    addError("",  $lang->getLang("You have selected an interval too long"));
}

?>
<div  class=bodyHD align="center"><?=$lang->getLang("User Hands")?></div>

<?php form_head(SELF, $name="searchhand", $method="POST", $onsubmit="") ?>

<table align="center">
    <tr>
        <td class="label"><?=$lang->getLang("Username");?></td> <td> <input type="text" name="username" value="<?=$username?>"  /> </td>
    </tr>
    <tr>
        <td class="label"><?=$lang->getLang("User id");?></td> <td> <input type="text" name="userid" value="<?=$userid?>"  /> </td>
    </tr>
    <tr>
        <td class="label"><?=$lang->getLang("Hand Id");?></td> <td> <input type="text" name="handid" value="<?=$handid?>"  /> </td>
    </tr>
    <tr>
        <td class="label"><?=$lang->getLang("Date");?></td> <td><?createDateHelper("date_helper", SEARCH_MAX_HOURS, "from", "to", $name)?></td>
    </tr>
    <tr>
        <td class="label"><?=$lang->getLang("From");?></td> <td> <input type='text' name='from' id='from'  value="<?=$from?>" ></td>
    </tr>
    <tr>
        <td class="label"><?=$lang->getLang("Until");?></td> <td> <input type='text' name='to' id='to' value="<?=$to?>"></td>
    </tr>
    <tr>
        <td class="label"><?=$lang->getLang("Table Id");?></td> <td> <input type="text" name="tbl_id" value="<?=$tbl_id?>" /></td>
    </tr>
    <tr>
        <td class="label"><?=$lang->getLang("Game");?>:</td><td><select name="games">
                <option <? if($games == 0){ ?> selected <? } ?> value="0"><?=$lang->getLang("All");?></option>
                <?php
                $sql = "Select gam_id , gam_name from game";
                $rs=$dbh->exec($sql);
                while($rs->hasNext()){
                    $row = $rs->next();
                    ?> <option <? if($games == $row["gam_id"]){ ?> selected <? } ?>  value='<?=$row["gam_id"]?>' ><?=$row["gam_name"]?></option><?php
                }
                ?>
            </select>
        </td>
    <tr>
        <td colspan="2">
            <input type="hidden" value="searchuserhands" name="action"/>
            <input type="submit" value="<?=$lang->getLang("Search");?>" > </td>
    </tr>
</table>
</form>
<?
switch ($action) {
    case "searchuserhands" :
        if($games == "5002"){
            searchuserhandsRoulette();
        }else{
            searchuserhands();
        }
        break;
}

function searchuserhandsRoulette(){
    global $dbh, $username, $userid, $from, $to, $tbl_id, $tbl_name, $handid, $games,$lang;

    $paramCount  = 0;

    $fieldFromDate = $from;
    $fieldToDate   = $to;

    settype($tbl_id, "integer");
    settype($userid, "integer");

    $result      = array ();
    if(empty($handid)){
        if(empty($username) && empty($userid)){
            addError("", $lang->getLang("Username or user id required"));
        }
    }

    if(areErrors()){
        showErrors();
        clearErrors();
        return false;
    }

    $sql = "SELECT res_id, ctr_id, ctr_pun_id, pun_username, pre_time, ctr_amount, ctr_balance_available, gam_name, ctr_tty_id, ctr_notes
      FROM customer_transaction
      JOIN
        result on res_id = ctr_res_id
      JOIN
        punter on pun_id = ctr_pun_id
      JOIN
        game on gam_id = res_gam_id
      WHERE
        ctr_pun_id = (SELECT pun_id from punter where pun_id=$userid OR pun_customer_number = $userid)
      AND res_id = ctr_res_id
      AND res_gam_id = 5002
      AND (ctr_tty_id = 12 OR ctr_tty_id = 13)
      AND ctr_time > DATE_FORMAT(  '$fieldFromDate', '%Y-%m-%d %H%i' )
      AND ctr_time < DATE_FORMAT(  '$fieldToDate', '%Y-%m-%d %H%i' )";

    $rs=$dbh->exec($sql);
    $numrow=$rs->getNumRows();
    $result=array();
    while($rs->hasNext()){

        $row=$rs->next();
        array_push($result,$row);
    }

    $rss= new Recordset($result);
    $cols_arr = array('Hand ID','Date','User Name', 'Game', 'Amount', 'Balance');
    $val_fmt_arr = array (
        'Hand ID'       =>'return getClassLink("javascript:openWindow(\'/casino_games/hand_details_roulette.php?gmn={$rec["ctr_id"]}&uid={$rec["ctr_pun_id"]}&hand_id={$rec["res_id"]}\',\'hand{$rec["res_id"]}\',\'900\',\'980\',\'0\',\'1\')", $rec["res_id"], "contentlink");',
        'Date'          =>'return ($pre_time"]);',
        'User Name'     =>'return getClassLink("' . refPage('customers/customer_view&customer_id={$rec["ctr_pun_id"]}') . '", $rec["pun_username"]);',
        'Amount'           =>'return getInDollars($rec["ctr_amount"]);',
        'Balance'       =>'return getInDollars($rec["ctr_balance_available"]);',
        'Game'          =>'return getGamesName($rec["gam_name"]);'
    );


    if($numrow!=0){
        $qp = new QueryPrinter($rss);
        $qp->printTable($rss,$cols_arr,"",$val_fmt_arr,$cell_fmt_arr, 0, 0,"",$width="100%", 1, $numrow,false);
    }else{
        addError("", "Nessun Risultato Trovato");
        showErrors();
    }

}

function searchuserhands(){
    global $dbh, $username, $userid, $from, $to, $tbl_id, $tbl_name, $handid, $games,$lang;
    $paramCount  = 0;

    $fieldFromDate = $from;
    $fieldToDate   = $to;

    settype($tbl_id, "integer");
    settype($userid, "integer");

    $result      = array ();
    if(empty($handid)){
        if(empty($username) && empty($userid)){
            addError("", $lang->getLang("Username or user id required"));
        }
    }

    if(areErrors()){
        showErrors();
        clearErrors();
        return false;
    }
    $jur_id = $_SESSION["jurisdiction_id"];
    //, tbl_id, tbl_name, tth_maxplayersnum

    if(EXT_PARTNER_ID>0){

       $sql="SELECT pun_username, PRE_RES_ID, pre_game_num, PRE_PUN_ID, PRE_BET, PRE_WIN, PRE_BALANCE, PRE_TBL_ID, PRE_PUN_IP, pre_bet_string , res_id, pre_time, res_num_betters, res_total_bets, res_total_payouts, res_total_rake,  pre_balance, (select gam_name from game where res_gam_id = gam_id) gam_name
            FROM partner_users, punter_result, result, punter ";
        if($_SESSION["jurisdiction_class"] != "casino"){
            $sql .= " LEFT JOIN jurisdiction club     ON club.jur_id     = pun_betting_club
  		       LEFT JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
             LEFT JOIN jurisdiction regional ON regional.jur_id = district.jur_parent_id
             LEFT JOIN jurisdiction nation   ON nation.jur_id   = regional.jur_parent_id";
        }
        $sql.="WHERE result.res_date BETWEEN '$fieldFromDate' AND '$fieldToDate' ";


        switch($_SESSION["jurisdiction_class"]){
            case "club":
                $sql .= " AND club.jur_id = $jur_id";
                break;
            case "district":
                $sql .= " AND district.jur_id = $jur_id";
                break;
            case "region":
                $sql .= " AND regional.jur_id = $jur_id";
                break;
            case "nation":
                $sql .= " AND  nation.jur_id = $jur_id";
                break;
            case "casino":
                $sql .= " ";
                break;
            default:
                $sql .= " WHERE 1=0";
                break;
        }

        $sql.="AND pnu_ptn_id=".EXT_PARTNER_ID;
        if(empty($handid)){
            if(!empty($username)){
                $sql .= " AND pun_username='$username' ";
            }
            if($userid!=""){
                $sql .= " AND pnu_id=$userid ";
            }
        }else{
            $sql .= " AND res_id= $handid ";
        }
   $sql.="AND res_id = pre_res_id
        AND pnu_local_id = pre_pun_id
        AND pun_customer_number = pnu_id
        ORDER BY res_date , pre_game_num ASC";
    }
    else{

    $sql = "SELECT pun_username, PRE_RES_ID, pre_game_num, PRE_PUN_ID, PRE_BET,
      	    PRE_WIN, PRE_BALANCE, PRE_TBL_ID, PRE_PUN_IP, pre_bet_string , res_id,PRE_VOID_DETAILS,
      	pre_time, res_num_betters, res_total_bets, res_total_payouts, res_total_rake,
      	    gam_name, pre_balance
      	 FROM
      	   punter_result
      	 JOIN
      	   punter on pun_id = pre_pun_id
      	 JOIN
      	   result on res_id = pre_res_id
      	 JOIN
      	   game on gam_id = res_gam_id";





    if($_SESSION["jurisdiction_class"] != "casino"){
        $sql .= " JOIN jurisdiction club     ON club.jur_id     = pun_betting_club
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
    //$sql .= " AND tbl_themeid = (res_gam_id - (res_gam_id % 1000))";
    if(empty($handid)){
        if(!empty($username)){
            $sql .= " AND pre_pun_id=(Select pun_id from punter where pun_username='$username' )";
        }
        if($userid!=""){
            $sql .= " AND (pun_id=$userid OR pun_customer_number = $userid) ";
        }
        if(!empty($tbl_id)){
            $sql .= " AND tables.tbl_id= $tbl_id";
        }
        $sql .= " AND result.res_date BETWEEN '$fieldFromDate' AND '$fieldToDate' ";
    }else{
        $sql .= " AND result.res_id= $handid ";
    }

    if($games != 0){
        $sql .= " AND tbl_id + tbl_themeid = $games";
    }

    $sql .= " ORDER BY res_date , pre_game_num ASC";
     //die($sql);
    }
    $starttime = microtime(true);
    $rs=$dbh->exec($sql);
    $endtime = microtime(true);
    $duration = $endtime - $starttime;
    $duration=number_format($duration, 4, ',', '')."s";
    $numrow=$rs->getNumRows();
    $result=array();
    while($rs->hasNext()){

        $row=$rs->next();
        $detHandUser     = explode(";", $row['pre_bet_string']);
        $row['cards']    = getCard($detHandUser[1]);
        $row['handrank'] = getResultHand($detHandUser[2]);
        array_push($result,$row);
    }



    $rss= new Recordset($result);
    $cols_arr = array('Hand ID','Game Nr','Date','User Name', 'Ip', 'Game', 'Bet', 'Win', 'Balance','Refund');
    $val_fmt_arr = array (
        'Hand ID'       =>'return getClassLink("javascript:openWindow(\'/casino_games/hand_details.php?gmn={$rec["pre_game_num"]}&uid={$rec["pre_pun_id"]}&hand_id={$rec["res_id"]}\',\'hand{$rec["pre_res_id"]}\',\'950\',\'980\',\'0\',\'1\')", $rec["res_id"], "contentlink");',
        'Game Nr'       =>'return getClassLink("javascript:openWindow(\'/casino_games/hand_details.php?gmn={$rec["pre_game_num"]}&uid={$rec["pre_pun_id"]}&hand_id={$rec["res_id"]}\',\'hand{$rec["pre_res_id"]}\',\'950\',\'980\',\'0\',\'1\')", $rec["pre_game_num"], "contentlink");',
        'Date'          =>'return ($rec["pre_time"]);',
        'User Name'     =>'return getClassLink("' . refPage('customers/customer_view&customer_id={$rec["pre_pun_id"]}') . '", $rec["pun_username"]);',
        'Bet'           =>'return getInDollars($rec["pre_bet"]);',
        'Win'           =>'return getInDollars($rec["pre_win"]);',
        'Balance'       =>'return getInDollars($rec["pre_balance"]);',
        'Game'          =>'return getGamesName($rec["gam_name"]);',
        'Ip'            =>'return getClassLink(refPage(\'customers/search&field=4&search_string=\' . urlencode(ereg_replace("[^0-9\.]", "", $rec["pre_pun_ip"]))), $rec["pre_pun_ip"]);',
        'Cards'         =>'return ($rec["cards"]);',
        'Hand Rank'     =>'return ($rec["handrank"]);',
        'Num Players'   =>'return ($rec["res_num_betters"]);',
        'Total Bets'    =>'return getInDollars($rec["res_total_bets"]);',
        'Total Payouts' =>'return getInDollars($rec["res_total_payouts"]);',
        'Total Rake'    =>'return getInDollars($rec["res_total_rake"]);',
        'Refund'        =>'return (checkRefund($rec["pre_void_details"]));'
    );

    ?>
    <div class="tip fleft"><?=$lang->getLang("Time took to execute your request")?>: <?=$duration?></div>
<?
    if($numrow!=0){
        $qp = new QueryPrinter($rss);
        $qp->printTable($rss,$cols_arr,"",$val_fmt_arr,$cell_fmt_arr, 0, 0,"",$width="100%", 1, $numrow,false);
    }else{
        addError("", $lang->getLang("No result found"));
        showErrors();
    }
}
function checkRefund($refund){
    if($refund==''){
        return "<button>Refund</button>";
    }
    else{
        return $refund;
    }
}
setupDateInputJs("from", "to");


?>
<!--
<script type="text/javascript">
    function openLink(link){
        document.all.videoframe.src = link;
    }
</script>-->

