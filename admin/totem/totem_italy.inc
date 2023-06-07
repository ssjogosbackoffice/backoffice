<?php
check_access('totem_configure_it' , $redirect = true);

//gameid:7001;7003;9001;2002;2001;3001;5001;6001;1004~bill:5;10;20;50;0;0;0;0;0;0;0;0;0;0;0;0~withdrawal:1~skins:default~tpoint:0~tpcode:123321
?>
    <script type="text/javascript">
        $(document).ready(function(){
            $("input#codepass").bind("keydown", function (e) {
                return (e.keyCode >= 48 && e.keyCode <= 54) || (e.keyCode >= 96 && e.keyCode <= 102) || e.keyCode == 8
            });
        });


            function openConfigurationTotem(){
                location.href = "<?=$_SERVER['PHP_SELF']?>"+"?page=totem/totem_italy&header_page=";
            }
    </script>
<?php
include_once 'JurisdictionsList.class.inc';
define("LABEL_GAME", 'gameid');
define("LABEL_BILL" , 'bill');
define("LABEL_WITHDRAWAL", 'withdrawal');
define("LABEL_SKINS", "skins");
define("LABEL_TPOINT", "tpoint");
define("LABEL_TCODE", "tpcode");
if(isset($_GET['ttcode'])){

    $ttmCode = $_GET['ttcode'];
    if(isset($_POST['modify'])){

        $tpcode = $_POST['code_pass'];
        $tpoint = $_POST['code_en'];
        $sql = "Select ttm_totem_md5 from totem where ttm_code = " . $dbh->quote($ttmCode);
        $md5Key = $dbh->queryOne($sql);
        $md5Key = md5($md5Key);
        $buffer = $tpcode;
        // get the amount of bytes to pad
        // very simple ASCII key and IV
        $key = substr($md5Key, 0, 24);
        //$iv = mcrypt_create_iv (mcrypt_get_block_size (MCRYPT_TripleDES, MCRYPT_MODE_CBC), MCRYPT_DEV_RANDOM);
        // hex encode the return value
        $tpcode = (base64_encode(mcrypt_encrypt(MCRYPT_TripleDES, $key, $buffer, MCRYPT_MODE_CBC, "00000000")));

        $sql = "Select ttm_conf_param from totem where ttm_code = " . $dbh->quote($ttmCode);
        $confparam = $dbh->queryOne($sql);
        $confparam=substr($confparam,0,strpos($confparam,'tpcode'));
        $confparam=$confparam."tpcode:".$tpcode;
        $update = "UPDATE totem set ttm_conf_param = " .$dbh->quote($confparam) ." where ttm_code = " . $dbh->quote($ttmCode);
        $res = $dbh->exec($update);

        if($res === 1){
            addResult('', $lang->getLang('Parameters successfully updated'));
        }else{
            addError('', $lang->getLang("Parameters weren't updated"));
        }
    }
    showResult();
    showErrors();

    $sql = "SELECT ttm_conf_param, ttm_totem_md5 from totem where ttm_code = "  . $dbh->quote($ttmCode);
    $resQuery = $dbh->queryRow($sql);
    $totemConfigure = $resQuery['ttm_conf_param'];
    $totemListConf = split('~', $totemConfigure);
    $tpoint = "n";
    $tpcode = "";
    foreach($totemListConf as $key => $value){
        list($label, $content) =  split(":", $value);
        if($label == LABEL_TPOINT){
            $tpoint = $content;
        }
        if($label == LABEL_TCODE){
            $tpcode = $content;
        }
    }

    $md5Key = $resQuery['ttm_totem_md5'];
    $md5Key = md5($md5Key);
    $key = substr($md5Key, 0, 24);
    //$iv = mcrypt_create_iv (mcrypt_get_block_size (MCRYPT_TripleDES, MCRYPT_MODE_CBC), MCRYPT_DEV_RANDOM);
    $buffer = trim(base64_decode($tpcode));
    $tpcode = trim(mcrypt_decrypt(MCRYPT_TripleDES, $key, $buffer, MCRYPT_MODE_CBC, "00000000"));
    $disable = "disabled";
    if($_SESSION['aty_code'] == "SUPERUSER"){
        $disable = "";
    }
    ?>

    <center>
        <div class="bodyHD">
            <?=$lang->getlang("SETTINGS FOR TOTEM CODE")?> <?=strtoupper($ttmCode)?>
        </div>
        <br/>
        <form action="<?=$_SERVER['PHP_SELF']?>?<?=$_SERVER['QUERY_STRING']?>" method="POST" >
            <div class="bodyHD">
                <?=$lang->getLang("CHOOSE TOTEM PASS")?>
            </div>
            <table>
                <tr>
                    <td class="label"><?=$lang->getLang("Code Enabled")?>:</td>
                    <td class="content">
                        <select name="code_en" id="codeenable" >
                            <option value="y" <?php if($tpoint=='y') { ?> selected <?php } ?> >y</option>
                            <option value="n" <?php if($tpoint=='n') { ?> selected <?php } ?> >n </option>

                        </select>
                    </td>
                </tr>
                <tr>
                    <td class="label"><?=$lang->getLang("Code password")?>:</td>
                    <td class="content">
                        <input type="text" value="<?=$tpcode?>" name="code_pass" id="codepass" size="8" maxlength="6" />
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
        <input type="submit" value="<?=$lang-getlang('Select')?>" />
    </form>
<?php

}

?>