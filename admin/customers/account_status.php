<?php include ('header.inc'); ?>
<?php include ('mail_functions.inc'); ?>
<?php include ('Curl.class.php'); ?>
<?php

$customer_id  = (int)$_GET['customer_id'];
$customer_row = getCustomer($customer_id);

if ( $customer_row )
{
    ?>
    <div  class="bodyHD">View Customers - <?=( !isBlank($customer_row['full_name']) ? $customer_row['full_name'] : $customer_row['username']);?> - <?=$customer_row['customer_number']?> (<?=$customer_row['member_type']?>)</div>
    <div class=bodyHD2>Account Status</div>
    <br /><br />
    <?php
    $aus_id   = $_SESSION["admin_id"];
    $curlRequest = new Curl();
    if ( 'unlock'  == $_GET['op'] )
    {
        $sql = "update punter set pun_access = 'allow'".

            " where pun_id = $customer_id";
        $dbh->exec($sql);

        $admin_user_id = $_SESSION['admin_id'];
        $to=$dbh->exec("SELECT  aus_id  from admin_user
         where aus_jurisdiction_id=".$customer_row['betting_club_id']."
         OR aus_jurisdiction_id=".$customer_row['district_id']."
         OR aus_jurisdiction_id=".$customer_row['region_id']."
         OR aus_jurisdiction_id=".$customer_row['nation_id']."
         OR aus_jurisdiction_id=1 ");

        $msgsql=" INSERT INTO admin_user_mailbox VALUES ";
        while($to->hasNext()){
            $rec = $to->next();
            $msg_id = $dbh->nextSequence("AUM_ID_SEQ");
            if($rec['aus_id']!=$admin_user_id){
                $msgsql.="(
                $msg_id,
                $admin_user_id,".
                    $rec['aus_id'].",
                'User unlocked',
                'User ".$customer_row['username']." was unlocked. Note:   " . $dbh->escape($_GET["note"]) . "' ,
                '" . $dbh->escape(getIpAddress()) . "', '0', CURRENT_TIMESTAMP, NULL, 0),";
            }
        }
        $msgsql=substr($msgsql, 0, -1);
        $dbh->exec($msgsql);

        $msg = "Customer account UNLOCKED";
        addPunterNote($customer_id, $aus_id, "User account <span class='unlocked'>unlocked</span>.Reason: ".$dbh->escape($_GET["note"]));
        $curlRequest->post(WEB_APP_PATH,"operationcode=2001&t=0&uid=$customer_id");
    }
    elseif ( 'lock' == $_GET['op'] )
    {
        $sql = "update punter set pun_access = 'deny' where pun_id = $customer_id";
        $dbh->exec($sql);

        $admin_user_id = $_SESSION['admin_id'];
         $to=$dbh->exec("SELECT  aus_id  from admin_user
         where aus_jurisdiction_id=".$customer_row['betting_club_id']."
         OR aus_jurisdiction_id=".$customer_row['district_id']."
         OR aus_jurisdiction_id=".$customer_row['region_id']."
         OR aus_jurisdiction_id=".$customer_row['nation_id']."
         OR aus_jurisdiction_id=1 ");

        $msgsql=" INSERT INTO admin_user_mailbox VALUES ";
        while($to->hasNext()){

            $rec = $to->next();
            $msg_id = $dbh->nextSequence("AUM_ID_SEQ");
            if($rec['aus_id']!=$admin_user_id){
            $msgsql.="(
                $msg_id,
                $admin_user_id,".
                $rec['aus_id'].",
                'User blocked',
                'User ".$customer_row['username']." was blocked. Note:   " . $dbh->escape($_GET["note"]) . "' ,
                '" . $dbh->escape(getIpAddress()) . "', '0', CURRENT_TIMESTAMP, NULL, 0),";
            }
        }
        $msgsql=substr($msgsql, 0, -1);
        $dbh->exec($msgsql);
        $msg = "Customer account LOCKED";
        addPunterNote($customer_id, $aus_id, "User account <span class='locked'>locked</span>.Reason: ".$dbh->escape($_GET["note"]));
        $curlRequest->post(WEB_APP_PATH,"operationcode=2001&t=1&uid=$customer_id");
    }
    elseif ( 'investigate_on' == $_GET['op'] )
    {
        $sql = "update punter set pun_investigate = 1" .
            " where pun_id = $customer_id";
        $dbh->exec($sql);
        $msg = "Customer account INVESTIGATION FLAG SET";
        addPunterNote($customer_id, $aus_id, "User under investigation");
    }
    elseif ( 'investigate_off' == $_GET['op'] )
    {
        $sql = "update punter set pun_investigate = NULL" .
            " where pun_id = $customer_id";
        $dbh->exec($sql);
        $msg = "Customer account INVESTIGATION FLAG CLEARED";
        addPunterNote($customer_id, $aus_id, "User under investigation removed");
    }
    elseif ( 'resend_email' == $_GET['op'] )
    {
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

       if(is_null($skinInformationArr['skn_reg_activation_url']) || $skinInformationArr['skn_reg_activation_url']==''){
           addError('','The user activation url is not defined');
       }
        if(is_null($skinInformationArr['skn_media_url']) || $skinInformationArr['skn_media_url']==''){
            addError('','The user activation media url is not defined');
        }
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
        $link=$userActivationUrl.'?code='.$customer_row['email_reg_code'].'&se='.$customer_row['sess_id'].'&sk='.$customer_row['site_id'];
        $body="<div style='width:500px;margin:10px auto;margin:0;padding:0;font-family: Verdana, sans-serif;font-size:12px;'>
                        <div>
                            <img src='".$userActivationMedia."/logo.png' width='237px' height='60px'/>
                        </div>
                        <div style='width:100%;min-height:400px;-webkit-border-radius: 5px;-moz-border-radius: 5px;border-radius: 5px;color:#333;padding:5px;font-size:13px;font-weight:bold;'>
                        <p style='line-height:25px;'>Dear ".$customer_row['first_name']." ".$customer_row['last_name'].",<br/>Welcome to ".$skinName."! A world of games awaits you!<br/><br/>Click <a style='font-size:15px;' target='_blank' href='".$link."'>HERE</a> to activate your account. Thank you!<br/><br/>If you have any questions, contact us and we'll do our best to help you.<br/><br/><br/>Have Fun,<br/> ".$skinName." Team";
        require_once 'Mail.php';
        require_once 'Mail/mime.php' ;
        $from = $userMailSender;
        $to = $customer_row['email'];
        $subject=  'Confirm registration';
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
            $msg="An error has occurred during mail sending.Customer registration email not sent";
        }
        else
        {
            $msg = "Customer registration email resent successfully";
        }
    }
    else{
        showErrors();
        }
    }

    if ( $msg )
    {
        printInfoMessage($msg);
    }
}
else
    die('Invalid Customer ID');
?>
<br /><br />
<input type="button" style="width:120px" value="Back" onClick="window.location='<?=refPage('customers/customer_view&customer_id='.$customer_id);?>'">
<?php include 'footer.inc'; ?>