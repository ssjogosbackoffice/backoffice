<?
function getMultiPartRegMessage($reg_code, $first_name, $email, $sms_sent)
{  global $HTTP_SERVER_VARS;
   	
/*************************/
/***** get text part *****/
/*************************/
   ob_start();
?>
Hi <?=$first_name?>, 

Thank you for registering with <?=CASINO_FULL_NAME?> Online.

You should have received a registration code from our representative.  
<? if ( $sms_sent ) { ?>
The code was also sent to the mobile phone number you registered with.
<? } ?>

If you have not already activated your account and logged in, go to
https://<?=secure_host?>/register//preg.html and click on the "pre-registrations" at the top of the page.

We look forward to with the most exciting experience in
online gaming today. 

<?=CASINO_FULL_NAME?>
<?
   $mail_body['text'] = ereg_replace("\r\n", "\n", ob_get_contents());
   ob_end_clean();

/*************************/
/***** get html part *****/
/*************************/
   ob_start();
     include 'templates/email_head.inc';
?>
        <img src="<?=secure_host;?>/register/email_read.php?reg_code=<?=$reg_code?>&email=<?=urlencode($email)?>" border=0>
        <P>
        Hi <?=$first_name?>,
        <P>
        Thank you for registering with <?=CASINO_FULL_NAME?> Online.
        <P>
		You should have received a registration code from our representative.  
		<? if ( $sms_sent ) { ?>
		The code was also sent to the mobile phone number you registered with.
		<? } ?>
		<P>		
		If you have not already activated your account and logged in, go to
		<a href="<?=secure_host?>/register/preg.html" target="_new"><?=secure_host?>/register/preg.html</a> and click on the "pre-registrations" at the top of the page.
		<P>
		We look forward to with the most exciting experience in online gaming today. 
        <P>
        <?=CASINO_FULL_NAME;?>
        <br><br><br>
<?   include 'templates/email_tail.inc';
	 $mail_body['html'] = ereg_replace("\r\n", "\n", ob_get_contents());
   ob_end_clean();

   return $mail_body;
}
?>