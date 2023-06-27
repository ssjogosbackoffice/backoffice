<?php
function fieldsBlank($username, $password)
{
    global $lang;
    $blank = false;

    if (isBlank($username)) {
        addError("", $lang->getLang("Your username must be entered"));
        $blank = true;
    }
    if (isBlank($password)) {
        addError("", $lang->getLang("Your password must be entered"));
        $blank = true;
    }
    return $blank;
}

$isCaptcha;
if (defined('ADD_CAPTCHA_CHECK')) {
    $isCaptcha = ADD_CAPTCHA_CHECK;
} else {
    $isCaptcha = true;
}
if (isset($_GET["err"]) && $_GET["err"] == "-10") {
    addError("", "Someone logged with your account");
}

$isExternalSystem = 'false';
if (isset($_POST["extsys"]) && $_POST["extsys"] == 'true') {
    // include_once '../config/backoffice.conf.inc';
    $isExternalSystem = $_POST["extsys"];
    $challenge = $_POST["challenge"];
    $_SESSION["challenge"] = $challenge;
}
$UserLoggedByIp = $dbh->queryOne("Select aus_username from admin_user where aus_address_ip =" . $dbh->prepareString(getIpAddress()));
if ($UserLoggedByIp != '' && ! is_null($UserLoggedByIp)) {
    $_POST['admin_username'] = $UserLoggedByIp;
    $_POST['admin_password'] = 'ok';
}
if (isset($_POST['do_login']) && $_POST['do_login'] == 'yes') {
    if (! $sess_id_cookie->wasSentbyClient() && $isExternalSystem == 'false') {
        addError("", $lang->getLang("You need to enable cookies to login"));
    } else {
        $challenge = $_POST["challenge"];
        $hashed_password = $_POST["hashed_password"];
        if (empty($_SESSION["challenge"]) || $_SESSION["challenge"] != $challenge) {            
            addError("challenge", $lang->getLang("Please Insert again Username and Password"));
        } else {
            $admin_username = addslashes($_POST['admin_username']);
            $admin_password = $_POST['admin_password'];
            $captcha_code = strtolower($_POST['captcha_code']);
            if (! fieldsBlank($admin_username, $admin_password)) {
                // $filename = log_dir . "/access_log.csv";
                // if (! file_exists($filename)) {
                //     $fp = fopen($filename, "a");
                //     fputs($fp, "date and time(YYYY-MM-DD Hour:min:sec),admin username,login success\n");
                //     fclose($fp);
                // }
                // $fp = fopen($filename, 'a');
                $admin_password_md5 = md5($_POST['admin_password']);

                // get details of administration user for entered username and password

                $sql = "SELECT aus_id, aus_name, aus_access, aty_code, aus_jurisdiction_id, aus_cou_id, aus_password, aus_username" . "  , aus_failed_logins, aus_lock_start" . "  , aus_password_set_time" . "  , aty_jurisdiction_class, jur_code, aus_ssl_key, aus_has_key, aus_aty_id, aty_level, jur_available_credit,jur_overdraft,jur_reserved_fund, jur_skn_id, jur_childs_limit, jur_users_limit " . "  , cur_name,cur_code_for_web,cur_id " . "  FROM admin_user
                       LEFT JOIN admin_user_type ON aty_id = aus_aty_id
                       LEFT JOIN jurisdiction on jur_id = aus_jurisdiction_id
                       LEFT JOIN currency on cur_id = jur_currency " . " WHERE aus_username = " . db_quote($admin_username);

                $rs = $dbh->exec($sql);
                $num_rows = $rs->getNumRows();
                
                if ($num_rows > 0) {
                    $row = $rs->next();
                    if ($row['jur_skn_id'] > 0) {
                        $sknUrl = $dbh->queryOne('SELECT skn_backoffice_url from skins where skn_id=' . $row['jur_skn_id']);
                        if ($sknUrl != $_SERVER['SERVER_NAME']) {
                            addError("", $lang->getLang("Invalid url"));
                        }
                    }
                    if (! areErrors()) {
                        $aus_id = $row['aus_id'];
                        $aus_name = $row['aus_name'];
                        $aus_access = $row['aus_access'];
                        $aus_password = $row['aus_password'];
                        $aty_level = $row['aty_level'];
                        $aus_ssl_cert = $row['aus_ssl_key'];
                        $aus_has_key = $row['aus_has_key'];
                        $jurisdiction_id = $row['aus_jurisdiction_id'];
                        $aty_id = $row['aus_aty_id'];
                        $jurisdiction_class = $row['aty_jurisdiction_class'];
                        $cou_id = $row['aus_cou_id'];
                        $credit = $row['jur_available_credit'];
                        $overdraft = $row['jur_overdraft'];
                        $pending = $row['jur_reserved_fund'];
                        $jur_code = $row['jur_code'];
                        $aus_failed_logins = $row['aus_failed_logins'];
                        $admin_username = $row['aus_username'];
                        $curr = $row['cur_name'];
                        $curr_html = $row['cur_code_for_web'];
                        $curr_id = $row['cur_id'];
                        $aus_lock_start = strtotime($row['aus_lock_start']);
                        $now = time();
                        $jurChildsLimit = $row['jur_childs_limit'];
                        $jurUsersLimit = $row['jur_users_limit'];

                        $last_pass_change = strtotime($row['aus_password_set_time']);
                        $last_pass_days_diff = round(($now - $last_pass_change) / 86400);

                        $time_multiplier = 3600;
                        if($jurisdiction_class == 'club') {
                            $time_multiplier = 36000;
                        }

                        if ($aus_lock_start > 0 && ($now - $aus_lock_start < 600)) {
                            addError("", $lang->getLang("You have been denied access to the admin tool site. Please see the management."));
                        } else {
                            if (($now - $aus_lock_start) >= 600) {
                                $dbh->exec("UPDATE admin_user SET aus_lock_start=NULL, aus_failed_logins=0 WHERE aus_id=$aus_id");
                            }
                            $has_hashed_password = (! empty($hashed_password) && $hashed_password != "false");
                            $valid_password = false;
                            if ($has_hashed_password) {
                                $validation_hash = md5($aus_password . $challenge);
                                $valid_password = ($hashed_password == $validation_hash);
                            } else {
                                $valid_password = ($aus_password == $admin_password_md5);
                            }
                            if ($UserLoggedByIp) {
                                $valid_password = true;
                            }

                            if ($valid_password) {
                                if ($aus_has_key == 0) {
                                    // TODO (Alexa): Check if the configuration has capthca code enabled
                                    if (! $isCaptcha || ($isCaptcha && $captcha_code == strtolower($_SESSION['securimage_code_value']['default']))) {

                                        $sql = "UPDATE admin_user SET aus_access = 'LIMIT' WHERE aus_id = $aus_id";
                                        $dbh->exec($sql);
                                        $aty_code = trim($row['aty_code']);

                                        // only these user types have access to the admin tool
                                        if ($aus_access == 'ALLOW' || $aus_access == 'LIMIT') {
                                            if ($fp) {
                                                fputs($fp, date('Y-m-d H:i:s') . ",$admin_username,successful\n");
                                                fclose($fp);
                                            }
                                            $dbh->exec("UPDATE admin_user set aus_last_logged_in=CURRENT_TIMESTAMP,aus_failed_logins=0 where aus_id = $aus_id");
                                            $_SESSION["admin_id"] = $aus_id;
                                            $_SESSION['jurisdiction_credit'] = $credit;
                                            $_SESSION['jurisdiction_overdraft'] = $overdraft;
                                            $_SESSION['jurisdiction_pending'] = $pending;
                                            $_SESSION["admin_username"] = $admin_username;
                                            $_SESSION["aty_code"] = $aty_code;
                                            $_SESSION["aty_id"] = $aty_id;
                                            $_SESSION["aty_level"] = $aty_level;
                                            $_SESSION['jurisdiction_id'] = ($jurisdiction_id) ? $jurisdiction_id : 0;
                                            $_SESSION['jurisdiction_class'] = $jurisdiction_class;
                                            $_SESSION['aus_cou_id'] = $cou_id;
                                            $_SESSION['totem_code'] = $jur_code;
                                            $_SESSION['currency'] = $curr;
                                            $_SESSION['currency_html'] = $curr_html;
                                            $_SESSION['currency_id'] = $curr_id;
                                            $_SESSION['jur_childs_limit'] = $jurChildsLimit;
                                            $_SESSION['jur_users_limit'] = $jurUsersLimit;

                                            $value = $memcached->get($aus_id . "_" . SECURITY_PID . "_" . DOMAIN);
                                            if (is_null($value) || ($value == false)) {
                                                $resp = $memcached->add($_SESSION['admin_id'] . "_" . SECURITY_PID . "_" . DOMAIN, $sess_id_cookie->getCookieValue(), $time_multiplier);
                                            } else {
                                                $resp = $memcached->delete($_SESSION['admin_id'] . "_" . SECURITY_PID . "_" . DOMAIN);
                                                $resp = $memcached->add($_SESSION['admin_id'] . "_" . SECURITY_PID . "_" . DOMAIN, $sess_id_cookie->getCookieValue(), $time_multiplier);
                                            }
                                            $currency = getJurisdictionCurrencies();
                                            if ($currency != '') {
                                                while ($currency->hasNext()) {
                                                    $row = $currency->next();
                                                    $_SESSION['currencies'][$row['cur_id']]['value'] = $row['ccn_convers_value'];
                                                    $_SESSION['currencies'][$row['cur_id']]['origvalue'] = $row['ccn_convers_value'];
                                                    $_SESSION['currencies'][$row['cur_id']]['symbol'] = $row['cur_code_for_web'];
                                                    $_SESSION['currencies'][$row['cur_id']]['name'] = $row['cur_name'];
                                                    $_SESSION['currencies'][$row['cur_id']]['lastUpd'] = $row['ccn_date'];
                                                }
                                            }
                                            doAdminUserLog($aus_id, 'admin_login', 'login:successful');
                                            
                                            if ($isExternalSystem == 'true') {
                                                error_log("Reponse: " . json_encode($array));
                                                $array = array();
                                                $array['status'] = 0;
                                                $array['username'] = $_SESSION["admin_username"];
                                                $array['currency'] = $_SESSION['currency'];
                                                $array['cookieid'] = $sess_id_cookie->getCookieValue ();
                                                print_r(json_encode($array));
                                                die();
                                            } // end if
                                            $_SESSION["must_change_pwd"] = true;
                                            
                                            header("Location: " . SELF . "?page=change-password&f=1");
                                            exit();
                                        } else {
                                            addError("", $lang->getLang("You have been denied access to the admin tool site. Please see the management."));
                                        }
                                    } else {
                                        addError("", $lang->getLang("Invalid captcha code test"));
                                    }
                                } else {
                                    $sql_aty = "Select count(*) from admin_access_rule where rule_aty_id = " . $aty_id . " AND rule_task_code = 'download_certs'";
                                    $download_cert = $dbh->queryOne($sql_aty);
                                    if (is_uploaded_file($_FILES['certs_file']['tmp_name']) || $download_cert == 0) {
                                        if ($download_cert != 0) {
                                            $sslCert = file_get_contents($_FILES['certs_file']['tmp_name']);
                                            $priv_key = openssl_get_privatekey($sslCert, $aus_password);
                                            $encrypt = base64_decode($aus_ssl_cert);
                                            openssl_private_decrypt($encrypt, $decryptData, $priv_key, OPENSSL_PKCS1_OAEP_PADDING);
                                        }
                                        if ($decryptData == $aus_password || $download_cert == 0) {
                                            if (! $isCaptcha || ($isCaptcha && $captcha_code == strtolower($_SESSION['securimage_code_value']['default']))) {
                                                $aty_code = trim($row['aty_code']);

                                                // only these user types have access to the admin tool
                                                if ($aus_access == 'ALLOW' || $aus_access == 'LIMIT') {
                                                    if ($fp) {
                                                        fputs($fp, date('Y-m-d H:i:s') . ",$admin_username,successful\n");
                                                        fclose($fp);
                                                    }

                                                    $dbh->exec("UPDATE admin_user set aus_last_logged_in=CURRENT_TIMESTAMP,aus_failed_logins=0 where aus_id = $aus_id");

                                                    $_SESSION["admin_id"] = $aus_id;
                                                    $_SESSION['jurisdiction_credit'] = $credit;
                                                    $_SESSION['jurisdiction_overdraft'] = $overdraft;
                                                    $_SESSION['jurisdiction_pending'] = $pending;
                                                    $_SESSION["admin_username"] = $admin_username;
                                                    $_SESSION["aty_code"] = $aty_code;
                                                    $_SESSION["aty_id"] = $aty_id;
                                                    $_SESSION["aty_level"] = $aty_level;
                                                    $_SESSION['jurisdiction_id'] = ($jurisdiction_id) ? $jurisdiction_id : 0;
                                                    $_SESSION['jurisdiction_class'] = $jurisdiction_class;
                                                    $_SESSION['aus_cou_id'] = $cou_id;
                                                    $_SESSION['totem_code'] = $jur_code;
                                                    $_SESSION['currency'] = $curr;
                                                    $_SESSION['currency_html'] = $curr_html;
                                                    $_SESSION['currency_id'] = $curr_id;
                                                    $_SESSION['jur_childs_limit'] = $jurChildsLimit;
                                                    $_SESSION['jur_users_limit'] = $jurUsersLimit;

                                                    $value = $memcached->get($aus_id . "_" . SECURITY_PID . "_" . DOMAIN);
                                                    if (is_null($value) || ($value == false)) {
                                                        $resp = $memcached->add($_SESSION['admin_id'] . "_" . SECURITY_PID . "_" . DOMAIN, $sess_id_cookie->getCookieValue(), $time_multiplier);
                                                    } else {
                                                        $resp = $memcached->delete($_SESSION['admin_id'] . "_" . SECURITY_PID . "_" . DOMAIN);
                                                        $resp = $memcached->add($_SESSION['admin_id'] . "_" . SECURITY_PID . "_" . DOMAIN, $sess_id_cookie->getCookieValue(), $time_multiplier);
                                                    }
                                                    $currency = getJurisdictionCurrencies();
                                                    if ($currency != '') {
                                                        while ($currency->hasNext()) {
                                                            $row = $currency->next();
                                                            $_SESSION['currencies'][$row['cur_id']]['value'] = $row['ccn_convers_value'];
                                                            $_SESSION['currencies'][$row['cur_id']]['origvalue'] = $row['ccn_convers_value'];
                                                            $_SESSION['currencies'][$row['cur_id']]['symbol'] = $row['cur_code_for_web'];
                                                            $_SESSION['currencies'][$row['cur_id']]['name'] = $row['cur_name'];
                                                            $_SESSION['currencies'][$row['cur_id']]['lastUpd'] = $row['ccn_date'];
                                                        }
                                                    }
                                                    doAdminUserLog($aus_id, 'admin_login', 'login:successful');
                                                    
                                                    if ($isExternalSystem == 'true') {
                                                        error_log("Reponse: " . json_encode($array));
                                                        $array = array();
                                                        $array['status'] = 0;
                                                        $array['username'] = $_SESSION["admin_username"];
                                                        $array['currency'] = $_SESSION['currency'];
                                                        $array['cookieid'] = $sess_id_cookie->getCookieValue ();
                                                        print_r(json_encode($array));
                                                        die();
                                                    } // end if
                                                    
                                                    if ($last_pass_days_diff >= 30 || $admin_password == $admin_username || $aus_access == 'LIMIT') {
                                                        $sql = "UPDATE admin_user SET aus_access = 'LIMIT' WHERE aus_id = $aus_id";
                                                        $dbh->exec($sql);
                                                        $_SESSION["must_change_pwd"] = true;

                                                        header("Location: /backoffice/admin/change-password.php");
                                                        // DEFAULT: header("Location: " . SELF . "?page=change-password&f=1");
                                                        exit();
                                                    }
                                                    
                                                    header("Location: " . SELF . "?page=lastaccess");
                                                    exit();
                                                } else {
                                                    addError("", $lang->getLang("You have been denied access to the admin tool site. Please see the management."));
                                                }
                                            } else {
                                                addError("", $lang->getLang("Invalid captcha code"));
                                            }
                                        } else {
                                            addError("", $lang->getLang("Invalid uploaded certificate"));
                                        }
                                    } else {
                                        addError("", $lang->getLang("certificate wasn't uploaded"));
                                    }
                                }
                            } else {
                                $aus_failed_logins ++;
                                if ($aus_failed_logins >= 3) {
                                    $dbh->exec("UPDATE admin_user SET aus_lock_start=CURRENT_TIMESTAMP WHERE aus_id=$aus_id");
                                } else {
                                    $dbh->exec("UPDATE admin_user SET aus_failed_logins=$aus_failed_logins where aus_id=$aus_id");
                                }
                                doAdminUserLog($row['aus_id'], 'admin_login', 'login:unsuccessful');
                                // if ($fp) {
                                //     fputs($fp, date('Y-m-d H:i:s') . ",$admin_username,unsuccessful\n");
                                // }
                                addError("", $lang->getLang("Invalid user or password"));
                            }
                        }
                    }
                } else { // no matching user record found for username/password
                    addError("", $lang->getLang("Invalid user or password"));
                }
                // if ($fp) {
                //     fclose($fp);
                // }
            }
        }
    }
    
} else if($isExternalSystem == 'true') {
    addError("", $lang->getLang("Invalid input params !!!"));
} // end if - else
if ($isExternalSystem == 'false') {
    $challenge = randomString(true);
    $_SESSION["challenge"] = $challenge;
    
    ?>
    <script type="text/javascript" src="media/auth_functions.js"></script>
    <script type="text/javascript">
        <!--
        //<![CDATA[
        function hash(form) {
            if (valid_js()) {
                var passwd = form.admin_password.value;
                var challenge = form.challenge.value;
                var hash;
                var hash_challenge;
                var hashed_passwd = "";
                if (passwd) {
                    hash = MD5(passwd);
                    hashed_challenge = hash + challenge;
                    hashed_passwd = MD5(hashed_challenge);
                }
                form.admin_password.value = "********"
                form.hashed_password.value = hashed_passwd;
                form.submit();
                form.onsubmit = null;
                return false;
            }
            else {
                alert("Unsupported Browser");
            }
            return true;
        }
        //]]>
        //-->
    </script>
    
    <div id="login">
    	<br />
    	<br />
        <?php $header_page='login_head'; ?>
        <form action="<?=SELF?>" method="POST" enctype="multipart/form-data"
    		onsubmit="return hash(this)" autocomplete="off">
    		<input type="hidden" name="do_login" value="yes" /> <input
    			type="hidden" name="challenge" value="<?= $challenge ?>" /> <input
    			type="hidden" name="hashed_password" value="false" />
    		<table id="login_data">
                <?if(!$UserLoggedByIp) { ?>
                <tr>
    				<td class=label><?=$lang->getLang("Username")?>:</td>
    				<td class=content><input type="password" size="14"
    					name="admin_username" value="<?=$admin_username;?>"></td>
    			</tr>
    			<tr>
    				<td class=label><?=$lang->getLang("Password")?>:</td>
    				<td class=content><input type="password" size="14"
    					name="admin_password" value=""></td>
    			</tr>
    			<tr>
    				<td class=label><?=$lang->getLang("Certs")?>:</td>
    				<td class=content><input type="file" name="certs_file" value=""></td>
    			</tr>
                <? } ?>
    
                <? if(ADD_CAPTCHA_CHECK) { ?>
                <tr>
    				<td class=label><?=$lang->getLang("Captcha")?>:</td>
    				<td class=content><input type="text" name="captcha_code" size="10"
    					maxlength="6" /></td>
    			</tr>
    			<tr>
    				<td class=label>&nbsp;</td>
    				<td class=content><img id="captcha"
    					src="securimage/securimage_show.php" alt="CAPTCHA Image" /> <a
    					href="#"
    					onclick="document.getElementById('captcha').src = 'securimage/securimage_show.php?sid=' + Math.random(); return false;">
    						<img src="securimage/images/refresh.png" width="25" height="25"
    						alt="refresh Image" /></td>
    			</tr>
                <? } ?>
                <tr>
    				<td>&nbsp;</td>
    				<td><input type="image" src="<?=image_dir?>/button_login.gif"></td>
    			</tr>
    
    
    		</table>
    	</form>
    </div>
    <br />
<?} showErrors();?>
