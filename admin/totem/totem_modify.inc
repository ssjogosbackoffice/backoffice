<?php
check_access ( 'ttm_totem_admin', $redirect = true );

if (! isset ( $_GET ['ttcode'] )) {
    addError ( "",  $lang->getLang("Totem code don't found. Check the get parameters" ));
    showErrors ();
    die ();
}

$totem_code = $_GET ['ttcode'];

function send($totem_code, $totem_status, $user_id, $totemMD5) {
    global $dbh;
    $md5 = hash ( 'md5', '3012' . $totem_code . $totemMD5 );

    if (! is_null ( $user_id )) {
        $sql = "select pun_customer_number from punter where pun_id = " . $user_id;
        $user_number = $dbh->queryOne ( $sql );

        $result = "";
        // Viene generata la chiave per l'autenticit� del messaggio
        $DataToSend = "operationtype=3012&cd=" . $totem_code . "&userid=" . $user_number . "&status=" . $totem_status . "&md5=" . $md5 . "&suboperation=1";
        error_log ( "CURL Request: " . WEB_APP_TOTEM );
        // Apro la connessione
    }else{
        $result = "";
        // Viene generata la chiave per l'autenticit� del messaggio
        $DataToSend = "operationtype=3012&cd=" . $totem_code . "&status=" . $totem_status . "&md5=" . $md5 . "&suboperation=1";
        error_log ( "CURL Request: " . WEB_APP_TOTEM );
    }
    sendToWebApp ( $DataToSend,WEB_APP_TOTEM );
}

function clearTotem($totem_code) {
    global $dbh,$lang;
    $sql = "select ttm_totem_md5 from totem where ttm_code= " . $dbh->quote ( $totem_code );
    $totemMD5 = $dbh->queryOne ( $sql );

    $md5 = hash ( 'md5', '3012' . $totem_code . $totemMD5 );

    $result = "";
    // Viene generata la chiave per l'autenticit� del messaggio
    $DataToSend = "operationtype=3012&cd=" . $totem_code . "&md5=" . $md5 . "&suboperation=3";
    error_log ( "CURL Request: " . WEB_APP_TOTEM );
    // Apro la connessione
    sendToWebApp ( $DataToSend,WEB_APP_TOTEM );
}

function logOnTable($ttm_code, $logNote, $totem_club) {
    global $dbh;
    $sql = "select ttm_id from totem where ttm_code = " . $dbh->quote ( $ttm_code );
    $ttm_id = $dbh->queryOne ( $sql );
    $datecreate = date ( "y-m-d H:i:s" );
    $ip = $_SERVER ["REMOTE_ADDR"];
    $sqlrequest3 = "INSERT INTO totem_log(ttl_ttm_id, ttl_ip, ttl_note, ttl_date, ttl_tlt_id, ttl_club_id) VALUES(" . $dbh->quote ( $ttm_id, 'integer' ) . "," . $dbh->quote ( $ip ) . "," . $dbh->quote ( $logNote ) . "," . $dbh->quote ( $datecreate, 'date' ) . ", 3," . $dbh->quote ( $totem_club ) . ")";
    $result3 = $dbh->exec ( $sqlrequest3 );

}

if (isset ( $_POST ['modify'] )) {
    $ttm_comm_key = $_POST ['ttm_comm_key'];
    $ttm_act_status = $_POST ['ttm_act_status'];
    $ttm_access = $_POST ['ttm_access'];
    $ttm_note = $_POST ['ttm_note'];
    $ttm_conf_param = $_POST ['ttm_conf_param'];
    $ttm_ip = $_POST ['ttm_ip'];
    $sql = "Update totem set ttm_comm_key = " . $dbh->quote ( $ttm_comm_key ) . "
							, ttm_act_status = " . $ttm_act_status . "
							, ttm_access =  " . $dbh->quote ( $ttm_access ) . "
							,  ttm_note = " . $dbh->quote ( $ttm_note ) . " 
							, ttm_ip =  " . $dbh->quote ( $ttm_ip ) . "
							  WHERE UPPER(ttm_code) = UPPER(" . $dbh->quote ( $totem_code ) . ")";

    $res = $dbh->exec ( $sql );
    if ($res === 1) {
        $sql = "Select ttm_code , ttm_pun_id , ttm_totem_md5 from totem where ttm_code = " . $dbh->quote ( $totem_code );
        $result = $dbh->queryRow ( $sql );
        addResult ( '', $lang->getLang('Query successfully executed' ));
        logOnTable ( $totem_code, 'Totem modified by users ' . $_SESSION ['admin_username'], 'NULL' );
        send ( $result ['ttm_code'], $ttm_access, $result ['ttm_pun_id'], $result ['ttm_totem_md5'] );
    } else {
        addError ( '', $lang->getlang("Query not executed" ));
    }
}

if (isset ( $_POST ['reset'] )) {
    $ttm_act_status = $_POST ['ttm_act_status'];
    $sql = "Update totem set ttm_comm_key = NULL
	, ttm_act_status = " . $ttm_act_status . "
	, ttm_access =  'deny'
	, ttm_note = NULL
	, ttm_ip =  NULL
	, ttm_club_id = NULL
	, ttm_pun_id = NULL
	, ttm_sess_id = NULL
	, ttm_last_login = NULL
	, ttm_conf_param = NULL
	WHERE UPPER(ttm_code) = UPPER(" . $dbh->quote ( $totem_code ) . ")";

    $res = $dbh->exec ( $sql );
    if ($res === 1) {
        addResult ( '', $lang->getLang('Query successfully executed' ));
        clearTotem ( $totem_code );
        logOnTable ( $totem_code, 'Totem resetted by users ' . $_SESSION ['admin_username'], 'NULL' );
    } else {
        addError ( '', $lang->getlang('Query not executed' ));
    }
}

$sql = "SELECT * FROM totem where ttm_code = " . $dbh->quote ( $totem_code );
$res = $dbh->queryRow ( $sql );

$ttm_comm_key = $res ['ttm_comm_key'];
$ttm_act_status = $res ['ttm_act_status'];
$ttm_totem_md5 = $res ['ttm_totem_md5'];
$ttm_access = $res ['ttm_access'];
$ttm_reg_date = $res ['ttm_reg_date'];
$ttm_last_login = $res ['ttm_last_login'];
$ttm_note = $res ['ttm_note'];
$ttm_ip = $res ['ttm_ip'];

?>

<form action="<?=$_SERVER['PHP_SELF']."?".$_SERVER['QUERY_STRING']?>"
      method="POST">
    <table cellspacing="1" cellpadding="10" border="0">
        <input type="hidden" name="ttm_code"
               value="<?=strtoupper($totem_code)?>" />
        <tr>
            <td class="formheading"><?=$lang->getLang("Totem code")?>:</td>
            <td class="formheading"><?=strtoupper($totem_code)?></td>
        </tr>
        <tr>
            <td class="label"><?=$lang->getLang("Comm key")?></td>
            <td><input name="ttm_comm_key" value="<?=$ttm_comm_key?>"></td>
        </tr>
        <tr>
            <td class="label"><?=$lang->getLang("Activation Status")?>:</td>
            <td><select name="ttm_act_status">
                    <option <?php if($ttm_act_status == 1){ ?> selected <?php }?>
                        value="1"><?=$lang->getlang('Not Activated')?></option>
                    <option <?php if($ttm_act_status == 2){ ?> selected <?php }?>
                        value="2"><?=$lang->getlang('Activated')?></option>
                </select></td>

        </tr>
        <tr>
            <td class="label"><?=$lang->getLang("Hardware key")?>:</td>
            <td><?=$ttm_totem_md5?></td>
        </tr>
        <tr>
            <td class="label"><?=$lang->getLang("Access")?>:</td>
            <td><select name="ttm_access">
                    <option <?php if($ttm_access == 'allow'){?> selected <?php } ?>
                        value="allow"><?=$lang->getLang("ALLOW")?></option>
                    <option <?php if($ttm_access == 'deny'){?> selected <?php } ?>
                        value="deny"><?=$lang->getLang("DENY")?></option>
                </select></td>
        </tr>
        <tr>
            <td class="label"><?=$lang->getLang("Registration Date")?>:</td>
            <td><?=$ttm_reg_date?></td>
        </tr>
        <tr>
            <td class="label"><?=$lang->getLang("Last login")?>:</td>
            <td><?=$ttm_last_login?></td>
        </tr>

        <tr>
            <td class="label"><?=$lang->getLang("Totem note")?>:</td>
            <td><input name="ttm_note" value="<?=$ttm_note?>"></td>
        </tr>
        <tr>
            <td class="label"><?=$lang->getLang("Totem Ip")?>:</td>
            <td><input name="ttm_ip" value="<?=$ttm_ip?>"></td>
        </tr>
        <tr>
            <td><input type="submit" name="modify" value="<?=$lang->getLang("Modify")?>"></td>
            <td> <?php if(strtolower($ttm_access) == 'deny'){?>  <input type="submit" name="reset"
                                                                        value="<?=$lang->getLang("Reset to initial settings")?>" /> <?php } ?></td>
        </tr>
    </table>
</form>

<?=showResult();?>
<?=showErrors();?>
