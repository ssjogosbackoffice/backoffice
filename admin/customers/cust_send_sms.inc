<?
$punter_row = getPunter($customer_id);
$first_name = $punter_row['first_name'];
$last_name  = $punter_row['last_name'];
$pun_full_name  = "$first_name $last_name"; 
$pun_email  = $punter_row['email'];
?>
<div class=level2heading>Customers</div>
<div class=subnavmenu>
<?printLink(refpage("customers/customers"), "Main menu");?>
&nbsp;&gt;&nbsp;
<?printLink(refpage("customers/customer_view&customer_id=$customer_id", "customers/customer_view_head"), "Details of $pun_full_name");?>
&nbsp;&gt;&nbsp;
<b>Send an email to <?=$pun_full_name?></b>
</div><br>

<?
//Validates fields submitted by email form 
function validatePostVars($subject, $message)
{  if ( isBlank($subject) )
      addError ("", "A  subject must be entered");

   if ( isBlank($message) )
      addError ("", "A message must to be entered");

   return !areErrors();
}



$form_submitted = $_POST['form_submitted'];
$subject 		= $_POST['subject'];
$message 		= $_POST['message'];

if ( $form_submitted )
{  if ( 'casinosalesleader' == $_SESSION['aty_code'] ) 
   {  $from = EMAIL_SALES;
      $from_name = CASINO_FULL_NAME.' Sales';
   }
   else
   {  $from = EMAIL_SUPPORT;
      $from_name = CASINO_FULL_NAME.' Support';
   }

   if ( validatePostVars($subject, $message) )
   {   
	   //set sender and recipient
	   									
	   $email_sender    = new EmailFrom($from, $from_name, $_SESSION['admin_id'], 'admin staff', $from, $from_name);
	   $email_recipient = new EmailRecipient($pun_email, $pun_full_name, $pun_id, 'customer');
	
	   $mail_sent = SendSingleEmail ( $email_sender, $email_recipient, $subject, $message, $html_body="", $priority);
	
	   if ( $mail_sent )
	   {  printInfoMessage("Your message has been sent to $pun_email");
	   	  doAdminUserLog($_SESSION['admin_id'], 'modify_customer', "subject=$subject|message=$message", $customer_id);
	   }
	   else
	      addError("", "Your message was not be sent.  Please try again.");
   }
 }



if ( !$form_submitted || areErrors() )
{  ?>
<?showErrors()?>
<? form_head() ?>
   <input type="hidden" name="customer_id" value="<?=$customer_id?>">
   <input type="hidden" name="to" value="<?=$pun_email?>">
   <table width=460 border="0" cellspacing="1" cellpadding="4">
   <tr>
       <td valign="top" class=label>To</td>
      <td class=content>
            <b><?=$pun_full_name?> &lt;<?=$pun_email;?>&gt;</b>
      </td>
   </tr>   
   <tr valign=top>
      <td  class=label width=120>From</td>
      <td class=content width=340>
<? if ( 'CASINOSALESLEADER' == $HTTP_SESSION_VARS['aty_code'] ) 
      echo EMAIL_SALES;
   else            
   	  echo EMAIL_SUPPORT;

?>    </td>
   </tr>
   <tr>
       <td valign="top" class=label>Subject</td>
       <td class=content><input type="Text" name="subject" size="50" value="<?=stripslashes($subject);?>"></td>
   </tr>
   <tr>
       <td valign="TOP" class=label>Message cl</td>
       <td class=content><textarea name="message" cols="50" rows="14" wrap=hard><?=stripslashes($message); ?></textarea></td>
   </tr>
   <tr align="center"><td colspan="2"><input type="Image" name="send" src="<?=image_dir?>/button_send.gif" border="0"></td></tr>
   </table>
   </form>
<?  
}   
?>
