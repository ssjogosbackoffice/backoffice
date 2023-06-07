<?php
check_access('totem_configure' , $redirect = true);
?>
<script type="text/javascript">
    <!--
    function openConfigurationTotem(){
        location.href = "<?=$_SERVER['PHP_SELF']?>"+"?page=totem/totem_configure&header_page=";
    }
    //-->
</script>
<?php
include_once 'JurisdictionsList.class.inc';
define("LABEL_GAME", 'gameid');
define("LABEL_BILL" , 'bill');
define("LABEL_WITHDRAWAL", 'withdrawal');
define("LABEL_SKINS", "skins");
if(isset($_GET['ttcode'])){

    $ttmCode = $_GET['ttcode'];
    if(isset($_POST['modify'])){

        $gamesPOST = $_POST['games'];
        $gamesList = implode(";",$gamesPOST);
        $currency = $_POST['currency'];

        $sql = "Select tbc_channels from totem_bill_channel where tbc_cur_id = $currency and tbc_tba_id = 1";
        $listChannels = $dbh->queryOne($sql);
        $withdrawalMethod = $_POST['withdrawal'];
        $skin = $_POST['skin'];
        $confparam = LABEL_GAME . ":" . $gamesList . "~" . LABEL_BILL . ":" . $listChannels . "~" . LABEL_WITHDRAWAL . ":" . $withdrawalMethod . "~" . LABEL_SKINS . ":" . $skin;

        $update = "UPDATE totem set ttm_conf_param = " .$dbh->quote($confparam) ." where ttm_code = " . $dbh->quote($ttmCode);
        $res = $dbh->exec($update);

        if($res === 1){
            addResult('', $lang->getLang('parameters update successfully'));
        }else{
            addError('', $lang->getLang('parameters not update'));
        }
    }
    showResult();
    showErrors();

    $sql = "SELECT ttm_conf_param from totem where ttm_code = "  . $dbh->quote($ttmCode);

    $totemConfigure = $dbh->queryOne($sql);
    $totemListConf = split('~', $totemConfigure);

    $gameContent = "";
    $billContent = "";
    $withdrawContent = "";
    $skin = "default";
    foreach($totemListConf as $key => $value){
        list($label, $content) =  split(":", $value);
        if($label == LABEL_GAME){
            $gameContent = $content;
        }
        if($label == LABEL_BILL){
            $billContent = $content;
        }
        if($label == LABEL_WITHDRAWAL){
            $withdrawContent = $content;
        }
        if($label == LABEL_SKINS){
            $skin = $content;
        }
    }

    $activatedGames = split(";",$gameContent);
    $activateBill = split(";", $billContent);


  //  $sql = "SELECT SUM(tbl_id + tbl_themeid) as gameid , tbl_name from tables group by tbl_name";
    $sql = "SELECT gam_id as gameid , gam_name from game group by gam_name";
    $res = $dbh->exec($sql);
    $gameList = array();
    while($res->hasNext()){
        $row = $res->next();
        array_push($gameList , array($row['gameid'],$row['gam_name']));
    }
    $gameCount = count($gameList);

    $sql = "Select cur_id, cur_name  from currency ";
    $currency = $dbh->exec($sql);
    ?>
    <center>
        <div class="bodyHD">
            <?=$lang->getLang("SETTINGS FOR TOTEM CODE")?> <?=strtoupper($ttmCode)?>
        </div>
        <br/>
        <form action="<?=$_SERVER['PHP_SELF']?>?<?=$_SERVER['QUERY_STRING']?>" method="POST" >
            <div class="bodyHD">
                <?=$lang->getlang("CHOOSE GAME FOR TOTEM")?>
            </div>
            <table>
                <tr>
                    <td>
                        <p style="height: 150px; width:300px; overflow: auto; border: 5px solid #eee; background: #eee; color: #000; margin-bottom: 1.5em;">
                            <?php foreach($gameList as $key => $value){
                                $selected = false;
                                for($z = 0 ; $z < count($activatedGames) ; $z++){
                                    if($activatedGames[$z] == $value[0]){
                                        $selected = true;
                                        break;
                                    }
                                }
                                ?><label><input type="checkbox"  <?php if($selected){?> checked <?php } ?> name="games[]" value="<?=$value[0]?>" /><?=$value[1]?> </label><br><?php
                            } ?>
                        </p>
                    </td>
                </tr>
            </table>

            <hr/>
            <div class="bodyHD">
                <?=$lang->getLang("SET BILL ACCEPTOR CHANNEL VALUE")?>
            </div>
            <div>
                <table>
                    <td class="label"><?=$lang->getLang("Set Currency")?></td>
                    <td class="content">
                        <select name="currency">
                            <?php
                            while($currency->hasNext()){
                                $row = $currency->next();
                                ?> <option value="<?=$row['cur_id']?>" ><?=$row['cur_name']?></option><?php
                            }
                            ?>
                    </td>
                    </select>
                </table>
            </div>
            <hr/>
            <div class="bodyHD">
                CHOOSE WITHDRAWAL METHOD
            </div>
            <table>
                <td class="label"><?=$lang->getLang("Withdrawal Method")?></td>
                <td class="content">
                    <select name="withdrawal">
                        <option <?php if($withdrawContent == "1"){ ?> selected <?php } ?> value="1">Automatically Accept</option>
                        <option <?php if($withdrawContent == "2"){ ?> selected <?php } ?> value="2">Pending With Totem Block</option>
                        <option <?php if($withdrawContent == "3"){ ?> selected <?php } ?> value="3">Pending Without Totem Block</option>
                    </select>
                </td>
            </table>

            <?php
            $sqlSkins = "Select skn_name , skn_foldername from skins where skn_status = 1 order by skn_name ASC";
            $skinsList = $dbh->exec($sqlSkins);

            ?>

            <div class="bodyHD">
                <?=$lang->getLang("CHOOSE TOTEM SKIN")?>
            </div>
            <table>
                <td class="label">Skins</td>
                <td class="content">
                    <select name="skin">
                        <?php
                        while($skinsList->hasNext()){
                            $row = $skinsList->next();
                            $skin_folder = $row['skn_foldername'];
                            echo $skin . " " . $skin_folder;
                            settype($skin_folder, 'text');
                            ?><option <?php if(trim($skin) == trim($skin_folder)){ ?> selected <?php } ?> value="<?=$row['skn_foldername']?>" ><?=$row['skn_name']?></option><?php
                        }
                        ?>
                    </select>
                </td>
            </table>

            <input type="submit" value="Apply" name="modify" />
            <input type="button" value="Back" onclick="openConfigurationTotem();" name="back" />
        </form>
    </center>


<?php
}else{
    $jurClass = JurisdictionsList::getInstance ( $dbh );
    $clubList = $jurClass->getChilJurisdictionsDataList ( $_SESSION ['jurisdiction_id'], 'club' );
    $clubIdList = array();
    foreach($clubList as $key => $value){
        array_push($clubIdList, $value[0]);
    }
    $sql = "SELECT ttm_code from totem where ttm_club_id in (".implode(',',$clubIdList ).")";
    $totemList = $dbh->exec($sql);


    ?>
    <div class="bodyHD"><?=$lang->getLang("Select the totem that you want configure.")?></div>
    <br/>
    <form action="<?=SELF?>" method="GET">
        <?php
        $queryString = $_SERVER['QUERY_STRING'];
        $queryList = split("&", $queryString);
        foreach($queryList as $key => $value){
            list($param, $val) = split("=", $value);
            ?><input type="hidden" name="<?=urlencode($param)?>" value="<?=$val?>" /><?php
        }
        ?>
        <select name="ttcode">
            <?php
            while($totemList->hasNext()){
                $row = $totemList->next();
                ?><option value="<?=$row['ttm_code']?>" ><?=$row['ttm_code']?></option><?php
            }
            ?>
        </select>
        <input type="submit" value="<?=$lang->getLang('Select')?>" />
    </form>
<?php

}

?>
