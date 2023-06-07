<?
//used to store details of an Email recipient
class EmailRecipient 
{  var $email;
   var $name;
   var $userId;
   var $userType;
   var $mxString;
 
   function EmailRecipient ( $email, $name, $user_id=0, $user_type="" )
   {  $this->email = $email;
      $this->name = $name;
      $this->userId = $user_id;  //system user's id in database
      $this->userType = $user_type;  //customer or admin
   }

   function getEmail() { return $this->email; }
   function getName() { return $this->name; }
   function getUserId() { return $this->userId; }
   function getUserType() { return $this->userType; }
   function getMxString() { return $this->mxString; }
 
   
   function setMxHosts() 
   {  
   	  list($mailbox, $host) = split("@", $this->email);
      
      if ( getmxrr($host, $mx_records) )
      {  $this->mxString = implode(';', $mx_records);
         return true;
      }
      return false;
   }
}

//used to store details of an Email sender
class EmailFrom
{  //recipient
   var $email;
   var $name;
   var $replyTo;
   var $replyToName;
   var $userId;
   var $userType;

   function EmailFrom ( $email, $name, $user_id=0, $user_type="", $reply_to="", $reply_to_name="" )
   {  $this->email = $email;
      $this->name = $name;

      if ( $reply_to )   //reply to specified
      {  $this->replyTo = $reply_to;
         $this->replyToName = $reply_to_name;
      }
      else //reply to not sepecified, default to sender email &name
      {  $this->replyTo = $email;
         $this->replyToName = $name;
      }

      $this->userId = $user_id;
      $this->userType = $user_type;
   }
   
   function getEmail() { return $this->email; }
   function getName() { return $this->name; }
   function getReplyTo() { return $this->replyTo; }
   function getReplyToName() { return $this->replyToName; }
   function getUserId() { return $this->userId; }
   function getUserType() { return $this->userType; }
}

/*** 
Sends an email to a single recipient.

Uses EmailRecipient and EmailFrom Objects in argument list
****/

function SendSingleEmail ( $from_ob, $recipient_ob, $subject, $text_body, $html_body="", $priority=3,  $BCC=EMAIL_BCC, $RETURN_RECEIPT_TO=EMAIL_RETURN) 
{
   if(!class_exists('phpmailer'))
	   include 'phpmailer/class.phpmailer.php';

   $mailer = new phpmailer();

   $mailer->IsSendmail();
   
   // set sender fields
   $mailer->From     = $from_ob->getEmail();
   $mailer->FromName = $from_ob->getName();
   $mailer->AddReplyTo($from_ob->getReplyTo(), $from_ob->getReplyToName());

   if ( $BCC )
   	 $mailer->AddBCC($BCC, $BCC);
   	 
   if ( $RETURN_RECEIPT_TO )
      $mailer->ConfirmReadingTo = $RETURN_RECEIPT_TO;
   	 
   //set recipient fields
   $mailer->Subject  = $subject;
   $mailer->Priority = $priority;
   $mailer->WordWrap = 80;                                
   
   $mx_hosts_found   = $recipient_ob->setMxHosts();
  
   
   if ( ! $mx_hosts_found )
   {  
	  return false;
   }

   //extract mailbox and hostname from sender address      
   $mailer->Host  =  $recipient_ob->getMxString();  // SMTP servers for recipient
   $mailer->AddAddress($recipient_ob->getEmail(), $recipient_ob->getName()); 
   
   //determine if a multipart html/alt email or single part (text only)
   if($html_body)
   {  
      $mailer->Body    =  $html_body;
      $mailer->AltBody =  $text_body;
      $mailer->IsHTML(true);
   }
   else
      $mailer->Body    =  $text_body;

      
   $mail_sent = $mailer->Send();
   
   if (!$mail_sent)
     $GLOBALS['mailer_error'] = $mailer->ErrorInfo;
   
   return $mail_sent;  
}


function getMultiPartMessage($html_part, $text_part)
{  global $HTTP_SERVER_VARS;
   
   $mail_body['text'] = $text_part;

   if ( $html_part )
   { ob_start();
	  include 'templates/email_head.inc';
      echo $html_part;
      include 'templates/email_tail.inc';
     
     $mail_body['html'] = ereg_replace("\r\n", "\n", ob_get_contents());
     ob_end_clean();   
   }      

   return $mail_body;
}
	?>
