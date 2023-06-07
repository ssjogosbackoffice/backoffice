<?php check_access('casino_view_agency_ticket', true);?>
    <script type="text/javascript">
        $(function() {
            $( "#date_start" ).datepicker({
                changeMonth: true,
                numberOfMonths:1
            });
        });
    </script>
<?php

form_head()?>
<?php printHiddenInput('doQuery', 'yes');?>
<?php printHiddenInput('search_type', 'date_range');?>
<?php
$doQuery = $_POST['doQuery'];
$row_to_page = (isset($_POST['resforpage'])) ? $_POST['resforpage'] : 20;
$date_start = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d", time());
$tid = $_POST['ticketid'] ;
$game=$_POST['games'];


?>
    <div class="centered bodyHD"><?=$lang->getLang("Search ticket")?></div><br>
    <table align="center" cellpadding="4" cellspacing="1" border="0">
        <tr valign=middle>
            <td class=label><?=$lang->getLang("Date")?> </td>
            <td class=content>
                <input type='text' name='date_start' id='date_start'  value="<?=$date_start?>" >
            </td>
        </tr>
        <?php
        $sql = "select gam_id, gam_name from game where gam_category='casino_agency'";
        $resGames = $dbh->doCachedQuery($sql,0);
        $gameList = array();
        while($resGames->hasNext()){
            $row = $resGames->next();
            $gameList[$row['gam_id']] = $row['gam_name'];
        }
        $activatedGames = $_POST['games'];
        ?>
        <tr>
            <td class="label"><?=$lang->getLang("Select the games")?></td>
            <td>
                <select name=games>
                    <option value=""><?=$lang->getLang("Select a game")?>:</option>
                    <?php foreach($gameList as $key => $value){
                        ?>
                        <option  value="<?=$key?>"<?php if($activatedGames==$key){?>selected="selected" <?php  }?>  ><?=$value?> </option><?php
                    } ?>
                </select>
                <input type=hidden name="previousgame" value="<?=$activatedGames?>">
            </td>
        </tr>

        <tr>
            <td class="label"><?=$lang->getLang("Ticket Id")?></td>
            <td class="content"> <input type='text' name='ticketid' id='ticketid'  value="" ></td>
        </tr>
        <tr>
            <td class=content>&nbsp;</td>
            <td class=content><input type="submit" value="<?=$lang->getLang("View")?>"/></td>
        </tr>
    </table>

<?php
if($doQuery){
    if($date_start ==''){
        addError("", $lang->getLang("Please choose a date"));
    }
    if($tid==''){
        addError("", $lang->getLang("Please enter the ticked id"));
    }
    if($game==''){
        addError("", $lang->getLang("Please choose a game"));
    }

    if(areErrors()){
        showErrors();
    }
    else{
        $tid=  preg_replace('#\D.*$#', '', $tid);


        $sql=" SELECT pun_username, PRE_RES_ID, pre_game_num, PRE_PUN_ID, PRE_BET, PRE_WIN, PRE_BALANCE, PRE_TBL_ID, PRE_PUN_IP, pre_bet_string , res_id, pre_time, res_num_betters, res_total_bets, res_total_payouts, res_total_rake, gam_name, pre_balance
FROM punter_result, result, game,punter";


        $jur_id = $_SESSION["jurisdiction_id"];

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
        $sql.=" AND res_id=pre_res_id
                AND res_gam_id=$game
                AND gam_id = res_gam_id
                AND pre_pun_id = pun_id
                AND res_date BETWEEN  '$date_start 00:00:00' AND '$date_start 23:59:59'
                AND pre_game_num = $tid";
        $rs=$dbh->exec($sql);
        $numrow=$rs->getNumRows();
        $result=array();
        while($rs->hasNext()){

            $row=$rs->next();
            $detHandUser     = explode(";", $row['pre_bet_string']);

            array_push($result,$row);
        }



        $rss= new Recordset($result);
        $cols_arr = array('Hand ID','Game Nr','Date','User Name', 'Ip', 'Game', 'Bet', 'Win', 'Balance','Video');
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
            'Total Bets'    =>'return getInDollars($rec["res_total_bets"]);',
            'Total Payouts' =>'return getInDollars($rec["res_total_payouts"]);',
            'Video'         =>'return getClassLink("javascript:openLink(\'http://recording.cslivegames.com/www/video.php?filename=$rec[res_id].flv&w=320&h=240\')", "visualizza video");'
        );


        if($numrow!=0){
            $qp = new QueryPrinter($rss);
            $qp->printTable($rss,$cols_arr,"",$val_fmt_arr,$cell_fmt_arr, 0, 0,"",$width="100%", 1, $numrow,false);
        }else{
            addError("", $lang->getLang("No result found"));
            showErrors();
        }
    }
}

