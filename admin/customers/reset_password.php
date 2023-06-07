<?php
$customer_id = (int)$_GET['customer_id'];
$customer_row = getCustomer($customer_id);

function generate_html_email($body) {
    ob_start();
    ?>
    <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
    <html>
    <head>
    </head>
    <body bgcolor="#fff" link="#00f" vlink="#00d" alink="#f00" leftmargin="0" topmargin="0" rightmargin="0" bottommargin="0" marginwidth="0">
    <table width="637" border="0" cellspacing="0" cellpadding="0" align="center">
        <tr><td style="height: 40px">&nbsp;</td></tr>
        <tr>
            <td bgcolor="#5D0100">
                <table width="619" border="0" cellspacing="6" cellpadding="16" align="center" bgcolor="#fff">
                    <tr>
                        <td>
                            <div style="position:relative;font-family:Arial;font-size:10pt;color:black">
                                <?php echo $body?>
                            </div>
                        </td>
                    </tr>
                </table>
            </td>
        </tr>
        <tr>
            <td style="height: 40px">&nbsp;</td>
        </tr>
    </table>
    </body>
    </html>
    <?php
    return ob_get_clean();
}
?>

<?php include ('header.inc');?>

<div  class=bodyHD>View Customers - <?=( !isBlank($customer_row['full_name']) ? $customer_row['full_name'] : $customer_row['username']);?> - <?=$customer_row['customer_number']?> (<?=$customer_row['member_type']?>)</div>
<div class=bodyHD2>Reset Password</div>
<br /><br />

<?php
if ( -1 == $customer_row['identified'] )
{
    // generate password

    mt_srand((double)microtime()*1000000);
    $str = md5 (uniqid (rand(100, 900), true));

    $password = substr($str, 0, 6);

    $md5_password = md5($password); //hash for database storage

    $sql = "UPDATE punter SET pun_password = '$md5_password' WHERE pun_id = $customer_id";
    $dbh->exec($sql);

    printInfoMessage("Customer password reset successfully");
    ?>
    <P>
        <span style="font-weight:bold">Customer Number</span>: <?=$customer_row['customer_number'];?><br />
    </P>
    <span style="font-weight:bold">Username</span>: <?=$customer_row['username'];?><br />
    <span style="font-weight:bold">Password</span>: <?=$password;?>
    </P>
<?php
}
else {
    $new_password = randomString($short=true); //generate random password
    $md5_password = md5($new_password); //hash

    //update customer record with new (hashed) password
    $sql = "UPDATE punter SET pun_password = '$md5_password'" .
        ", pun_temp_password = 't' WHERE pun_id = ".$customer_row['id'];
    $rs = $dbh->exec($sql);

    $email      = $customer_row['email'];
    $first_name = $customer_row['first_name'];
    $last_name  = $customer_row['last_name'];
    $full_name  = $customer_row['full_name'];

    $casino_full_name = CASINO_FULL_NAME;

    // Text part of multipart email

    ob_start();
    ?>
    Hi <?=$first_name?>,<br/><br/>Your password has been reset to a temporary one.<br/>Your new (temporary) password is: <?=$new_password?><br/><br/>For security reasons, your username is not included in this email.<br/><br/>For your own account security, on your next login you will be required to change your password.<br/><br/>We strongly recommend that you do this immediately.<br/><br/>Regards,<br/><br/><?=$casino_full_name?> Support
    <?php
    $text_part = ob_get_clean();

    $text_part = str_replace(array("<br/>", "<br />"),"\n",$text_part);
    // HTML part of multipart email
    //include (include_content('email_html_template.php'));
    ob_start();
    ?>
    Hi <?=$first_name?>,<br/><br/>Your password has been reset to a temporary one.<br/>Your new (temporary) password is: <?=$new_password?><br/><br/>For security reasons, your username is not included in this email.<br/><br/>For your own account security, on your next login you will be required to change your password.<br/><br/>We strongly recommend that you do this immediately.<br/><br/>Regards,<br/><br/><?=$casino_full_name?> Support
    <?php
    $html_part = ob_get_clean();
    $html_part = generate_html_email($html_part);

    // send the email
/*    if ( ! class_exists('phpmailer') )
        require 'phpmailer/class.phpmailer.php';

    $mailer = new phpmailer();
    $mailer->Host = "mail.dollarobet.com";
    $mailer->IsSMTP(true);

    // set sender fields
    $mailer->From = EMAIL_SUPPORT;
    $mailer->FromName = CASINO_FULL_NAME.' Support';

    //$customer_name = $first_name . ($middle_name ? ' ' . $middle_name . ' ' : ' ' ) . $last_name;
    $mailer->AddAddress($email, "$first_name $last_name");
    $mailer->AddReplyTo(EMAIL_SUPPORT, CASINO_FULL_NAME.' Support');
    $mailer->ConfirmReadingTo = EMAIL_RETURN;

    //set recipient fields
    $mailer->Subject  = "Your temporary password";
    $mailer->Priority = 1;
    $mailer->WordWrap = 80;

    //$mailer->IsHTML(true);
    //$mailer->Body    = $html_part;
    //$mailer->AltBody = $text_part;
    $mailer->Body    = $text_part;

    $mail_sent = $mailer->Send();

    if ( $mail_sent ) {
        $msg = "The password of $full_name has been reset to a temporary one.<br/>The temporary password has been emailed to the customer's registered email address";
    }
    else {
        $msg = "The attempt to email $full_name with a temporary password has failed.\nThe customer's password has been updated successfully, however, so it will be necessary to verify the customer's email address, and perform this reset again (so that the customer can know his/her password)";
    }*/


     $sql="SELECT * from skins where skn_id=".SKINID;

        $skinInformation=$dbh->doCachedQuery($sql,0);
        $skinInformationArr=$skinInformation->Records[0];
        if(count($skinInformation->Records)!=1){
            addError('','Unable to get skin details');
        }

        /** Configuration of the email  ****/
        $skinName=$skinInformationArr['skn_name'];
        $userActivationUrl=$skinInformationArr['skn_reg_activation_url'];
        $userActivationMedia=$skinInformationArr['skn_media_url'].'/main/site/media/images';
        $skinmaildetails=explode('#',$skinInformationArr['skn_email']);
        $userMailSender=$skinmaildetails[0];
        $userMailSenderPsswd=$skinmaildetails[1];
        $mailSmtpHost=$skinInformationArr['skn_smtp'];
        /***** End configuration of the email ***/

        if($skinName=='' || is_null($skinName)){
            addError('','The skin name is not set');
        }
        if(is_null($userMailSender) || $userMailSender=='' ){
            addError('','The user mail sender is not configured');
        }
        if(is_null($mailSmtpHost) || $mailSmtpHost==''){
            addError('','The smtp host is not configured');
        }
        if(!areErrors()){
        $body=$text_part;
         require_once 'Mail.php';
        require_once 'Mail/mime.php' ;
        $from = $userMailSender;
        $to = $email;
        $subject=  "Your temporary password";
        $host = explode(':',$mailSmtpHost);
        $crlf = "\n";
        $headers = array('From' => $from, 'To' => $to, 'Subject' => $subject);
        $mime = new Mail_mime($crlf);
        $mime->setHTMLBody($body);
        $body = $mime->get();
        $headers = $mime->headers($headers);
            $smtpArray=array();
            $smtpArray['host']=$host[0];
            if(isset($skinmaildetails[2])){
                $smtpArray['host']='ssl://'.$smtpArray['host'];
            }
        if($userMailSenderPsswd!=''){
            $smtpArray['auth'] = true;
            $smtpArray['username'] = $userMailSender;
            $smtpArray['password'] = $userMailSenderPsswd;
            if(isset($host[1])){
                $smtpArray['port'] = $host[1];
            }
        }
            $smtp = Mail::factory('smtp',$smtpArray);
    $mail = $smtp->send($to, $headers, $body);
        if ( PEAR::isError($mail) )
        {
              $msg = "The attempt to email $full_name with a temporary password has failed.\nThe customer's password has been updated successfully, however, so it will be necessary to verify the customer's email address, and perform this reset again (so that the customer can know his/her password)";

          }
            else {
                    $msg = "The password of $full_name has been reset to a temporary one.<br/>The temporary password has been emailed to the customer's registered email address";

            }
    }
    else{
        showErrors();
        }

    printInfoMessage($msg);
}
?>
<br /><br />
<input type="button" style="width:120px" value="Back" onClick="window.location='<?=refPage('customers/customer_view&customer_id='.$customer_id);?>'">
<? include 'footer.inc';?>
