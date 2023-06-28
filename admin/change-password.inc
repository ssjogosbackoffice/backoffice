<?php
require_once 'checkpass.inc';
?>
<div class=bodyHD><?=$lang->getLang("Change your password")?></div>
<?php
function checkFields($current_password, $new_password, $new_password_confirm)  {
    global $lang;
    $all_valid = true;
    if ( isBlank($current_password) )
    {   
        addError("", $lang->getLang("Your current password must be entered"));
        $all_valid = false;
    }

    if ( isBlank($new_password) ) 
    {
        addError("", $lang->getLang("Your new password must be entered"));
        $all_valid = false;
    } else {
        $new_password_len = strlen($new_password);
        if ( $new_password_len < 6 || $new_password_len > 50 ) {
            addError("", $lang->getLang("Your new password must be 6 to 50 characters long"));
            $all_valid = false;
        }
    }

    if ( isBlank($new_password_confirm) ) 
    {
        addError("", $lang->getLang("Your new password confirmation must be entered"));
        $all_valid = false;
    }

    if ( !isBlank($new_password) && !isBlank($new_password_confirm) ) 
    {
        if ( $current_password == $new_password ) 
        {
            addError("", $lang->getLang("Your new password is identical to your current password"));
            $all_valid = false;
        } 
        elseif ( $new_password != $new_password_confirm ) 
        {
            addError("", $lang->getLang("Your new password and new password confirmation do not match"));
            $all_valid = false;
        }
    }

    if ($all_valid) 
    {
        $all_valid = checkpass_run($_SESSION["admin_username"], $new_password);

    }

    return $all_valid;
}

globalise('form_submitted');
globalise('new_password');
globalise('new_password_confirm');
globalise('current_password');

if(isset($_GET['f']))
{
    printInfoMessage($lang->getLang("Your password expired, please change it!"));
}

if ( $form_submitted )
{  
    if ( $_SESSION['password_submit_code'] == $_POST['submit_code'] )
    {
?>
        <p class=mesg><?=$lang->getLang("Your password has been changed")?>.</p>
<?
    } else {
        if ( checkFields($current_password, $new_password, $new_password_confirm)  )
        {
            $aus_id = $_SESSION['admin_id'];
            $current_password_md5 =  md5($current_password);
            $new_password_md5 = md5($new_password);

            $sql  = "select count(*) as num from admin_user where aus_id = $aus_id";
            $sql .= " and aus_password =  '$current_password_md5'";
            $rs  = $dbh->exec($sql);
            $row = $rs->next();
            $count = $row['num'];

            if ( $count > 0) 
            {
                $sql  = "update admin_user set aus_password = '$new_password_md5', aus_password_set_time=CURRENT_TIMESTAMP";

                if($_SESSION['must_change_pwd'] == true)
                {
                    $sql .= ", aus_access='ALLOW'";
                }

                $sql .= " where aus_id = $aus_id";
                $rs = $dbh->exec($sql);

                if ( $rs )
                {
                    unset($_SESSION['must_change_pwd']);
                    $_SESSION['password_submit_code'] = $_POST['submit_code'];
                    $config = array(
                        'private_key_bits' => 2048,
                        'private_key_type' => OPENSSL_KEYTYPE_RSA
                    );
                    $res = openssl_pkey_new($config);
                    openssl_pkey_export($res, $privKey, $new_password_md5);
                    $public_key = openssl_pkey_get_details($res);
                    $public_key = $public_key['key'];
                    openssl_get_publickey($pub_key);
                    openssl_public_encrypt($new_password_md5, $crypted, $public_key,OPENSSL_PKCS1_OAEP_PADDING);
                    $value =  base64_encode($crypted);
                    $sql = "update admin_user set aus_ssl_key = '$value', aus_has_key = 1 where aus_id = $aus_id";
                    $rs = $dbh->exec($sql);
                    file_put_contents(KEYS_DIR."/privateKey_".$aus_id.".pem",$privKey);
                    //openssl_pkey_export_to_file($privKey,"keys/privateKey_".$aus_id.".pem", NULL);
                    printInfoMessage($lang->getLang("Your password has been changed").".");
                    $download_cert = check_access ( 'download_certs');
                    if($download_cert) printInfoMessage($lang->getLang("It's very important to download")."<a href='certificate/download.php?id=".$aus_id."' >".$lang->getLang("this file")."</a>".$lang->getLang("to be allowed to login the next time"));
                } else {
                    addError("", $lang->getLang("A database error occurred when attempting to change your password"));
                } 
            } else {
                addError("", $lang->getLang("The current password you entered is incorrect"));
            }
        }
    }
}


if ( !isset($form_submitted) || areErrors() )
{ 
    showErrors();
?>
    <?form_head();?>
    <input type="hidden" name="form_submitted" value="yes">
    <input type="hidden" name="submit_code" value="<?=$_SESSION['admin_id'].mktime();?>">
    <table  cellpadding=2 cellspacing=1 border=0>
        <tr>
            <td class=label><?=$lang->getLang("Your password has to")?></td>
            <td class=content>
                <div class="tip"> - <?=$lang->getLang("be at least 6 char length")?><br />
                                - <?=$lang->getLang("contain at least one capital letter")?><br />
                                - <?=$lang->getLang("contain at least one number")?><br />
                                - <?=$lang->getLang("contain at least one special char(ex: !@#$%^&* )")?><br />
                                - <?=$lang->getLang("not be the same with your email")?><br />


                </div>
            </td>
        </tr>
        <tr>
            <td class=label><?=$lang->getLang("Current Password")?></td>
            <td class=content>
                <input type="password" name="current_password" value="<?=$current_password;?>" size=18>
            </td>
        </tr>
        <tr>
            <td class=label><?=$lang->getLang("New Password")?></td>
            <td class=content>
                <input type="password" name="new_password" value="<?=$new_password;?>" size=18>
            </td>
        </tr>
        <tr>
            <td class=label>
                <?=$lang->getLang("Re-type New Password")?>
            </td>
            <td class=content>
                <input type="password" name="new_password_confirm" value="<?=$new_password_confirm;?>" size=18>
            </td>
        </tr>
        <tr>
            <td></td>
            <td><input type="image" src="<?=image_dir?>/button_submit.gif"></td>
        </tr>
    </table>
    </form>
<?
}
?>
