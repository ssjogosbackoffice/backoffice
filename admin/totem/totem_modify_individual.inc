<?php
check_access ( 'ttm_totem_reset', $redirect = true );

if (! isset ( $_POST ['ttcode'] )) {
    addError ( "", $lang->getLang("Totem code don't found. Check the get parameters" ));
    showErrors ();
    die ();
}

$totem_code = $_POST['ttcode'];
function send($totem_code, $totem_status, $user_id, $totemMD5) {
    global $dbh;
    $md5 = hash ( 'md5', '3012' . $totem_code . $totemMD5 );

    if (! is_null ( $user_id )) {
        $sql = "select pun_customer_number from punter where pun_id = " . $user_id;
        $user_number = $dbh->queryOne ( $sql );

        $result = "";
        // Viene generata la chiave per l'autenticit� del messaggio
        $DataToSend = "operationtype=3012&cd=" . $totem_code . "&userid=" . $user_number . "&status=" . $totem_status . "&md5=" . $md5 . "&suboperation=1";
        error_log ( "CURL Request: " . WEB_APP_URL );
        // Apro la connessione
    }else{
        $result = "";
        // Viene generata la chiave per l'autenticit� del messaggio
        $DataToSend = "operationtype=3012&cd=" . $totem_code . "&status=" . $totem_status . "&md5=" . $md5 . "&suboperation=1";
        error_log ( "CURL Request: " . WEB_APP_URL );
    }
    sendToWebApp ( $DataToSend,WEB_APP_URL );
}

function clearTotem($totem_code) {
    global $dbh;
    $sql = "select ttm_totem_md5 from totem where ttm_code= " . $dbh->quote ( $totem_code );
    $totemMD5 = $dbh->queryOne ( $sql );

    $md5 = hash ( 'md5', '3012' . $totem_code . $totemMD5 );

    $result = "";
    // Viene generata la chiave per l'autenticit� del messaggio
    $DataToSend = "operationtype=3012&cd=" . $totem_code . "&md5=" . $md5 . "&suboperation=3";
    error_log ( "CURL Request: " . WEB_APP_URL );
    // Apro la connessione
    sendToWebApp ( $DataToSend,WEB_APP_URL );
}

function logOnTable($ttm_code, $logNote, $totem_club) {
    global $dbh,$lang;
    $sql = "select ttm_id from totem where ttm_code = " . $dbh->quote ( $ttm_code );
    $ttm_id = $dbh->queryOne ( $sql );
    $datecreate = date ( "y-m-d H:i:s" );
    $ip = $_SERVER ["REMOTE_ADDR"];
    $sqlrequest3 = "INSERT INTO totem_log(ttl_ttm_id, ttl_ip, ttl_note, ttl_date, ttl_tlt_id, ttl_club_id) VALUES(" . $dbh->quote ( $ttm_id, 'integer' ) . "," . $dbh->quote ( $ip ) . "," . $dbh->quote ( $logNote ) . "," . $dbh->quote ( $datecreate, 'date' ) . ", 3," . $dbh->quote ( $totem_club ) . ")";
    $result3 = $dbh->exec ( $sqlrequest3 );

}

if (isset ( $_POST ['modify'] )) {

    $ttm_comm_key = $_POST ["ttm_comm_key$totem_code"];
    $ttm_act_status = $_POST ["ttm_act_status$totem_code"];
    $ttm_access = $_POST ["ttm_access$totem_code"];
    $ttm_note = $_POST ["ttm_note$totem_code"];
    $ttm_ip = $_POST ["ttm_ip$totem_code"];
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
    $ttm_act_status = $_POST ["ttm_act_status"];
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
        addError ( '', $lang->getlang("Query not executed" ));
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
    <table cellspacing="1" cellpadding="10" border="0">

        <form action="<?=$_SERVER['PHP_SELF']."?".$_SERVER['QUERY_STRING']?>"
              method="POST">
            <input type="hidden" name="ttm_code"
                   value="<?=strtoupper($totem_code)?>" />
            <tr>
                <td class="formheading"><?=$lang->getLang("Totem code")?>:</td>
                <td class="formheading"><?=strtoupper($totem_code)?></td>
            </tr>
            <tr>
                <td class="label"><?=$lang->getLang("Comm key")?></td>
                <td><input type="text" name="ttm_comm_key<?=$totem_code?>" value="<?=$ttm_comm_key?>"></td>
            </tr>
            <tr>
                <td class="label"><?=$lang->getLang("Activation Status")?>:</td>
                <td><select name="ttm_act_status<?=$totem_code?>">
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
                <td><select name="ttm_access<?=$totem_code?>">
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
                <td><input type="tex" name="ttm_note<?=$totem_code?>" value="<?=$ttm_note?>"></td>
            </tr>
            <tr>
                <td class="label"><?=$lang->getLang("Totem Ip")?>:</td>
                <td><input type="text" name="ttm_ip<?=$totem_code?>" value="<?=$ttm_ip?>"></td>
            </tr>
        </form>
        <tr>
            <td><button id="modify<?=$totem_code?>">Modify
                    <script>
                        $(function() {
                            $( '#modify<?=$totem_code?>')
                                .button()
                                .click(function() {
                                    $.post("totem/totem_modify_individual.inc?ttcode=<?=$totem_code?>",
                                        {modify:'1',
                                            ttm_act_status<?=$totem_code?>:$('select[name=ttm_act_status<?=$totem_code?>] option:selected').val(),
                                            ttm_access<?=$totem_code?>:$('select[name=ttm_access<?=$totem_code?>] option:selected').val(),
                                            ttm_note<?=$totem_code?>:$('input:text[name=ttm_note<?=$totem_code?>]').val(),
                                            ttm_ip<?=$totem_code?>:$('input:text[name=ttm_ip<?=$totem_code?>]').val(),
                                            ttm_comm_key<?=$totem_code?>:$('input:text[name=ttm_comm_key<?=$totem_code?>]').val()}, function(data){
                                            $('#modify<?=$totem_code?>').parent().closest('div').empty().append( data );
                                        });
                                });
                            return false;
                        });
                    </script>

                </button></td>
            <td> <?php if(($ttm_access == 'allow')||($ttm_access == 'ALLOW')){?>
                    <button id="reset<?=$totem_code?>"><?=$lang->getLang("Reset to initial settings")?>
                        <script>
                            $(function() {
                                $( '#reset<?=$totem_code?>')
                                    .button()
                                    .click(function() {
                                        $.post("totem/totem_modify_individual.inc?ttcode=<?=$totem_code?>",
                                            {reset:'1',
                                                ttm_act_status<?=$totem_code?>:$('select[name=ttm_act_status<?=$totem_code?>] option:selected').val()}, function(data){
                                                $('#modify<?=$totem_code?>').parent().closest('div').empty().append( data );
                                            });
                                    });
                                return false;
                            });
                        </script>
                    </button>
                <?php } ?></td>
        </tr>
    </table>
<?=showResult();?>
<?=showErrors();?>