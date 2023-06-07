<style type="text/css">
.errbox { 
  background-color: #d1d1d1;
  font-size:  8pt;
  color:  red;
  
}
</style>
<script type="text/javascript">
	is_opera = (navigator.userAgent.toLowerCase().indexOf("opera") != -1);
	var curObj = null; /* current object wich be drag */
	var mouseY = 0;	/* mouse X */
	var moousX = 0;	/* mouse Y */
	var dx = 0;
	var dy = 0;


    function dragThis(what){
         var tmp;
	 //alert(what.id);
	 if (document.all) tmp = document.all[what].style;
	 else if (document.getElementById) tmp = document.getElementById(what).style;
	 else if (document.layers) tmp=document.layers[what];
	 curObj = tmp;
	 var str = "";
	 var i=0;
	 str = tmp.left;	/* in ie style.left="12px" and in opere style.left="12" */
	 i= (!is_opera) ? str.substr(0,str.length-2) : str;
	 dx = mouseX - i;
	 str = tmp.top;
	 i= (!is_opera) ? str.substr(0,str.length-2) : str;
	 dy = mouseY - i;
   }


    function mouseMove(e)
    { 	mouseX = (document.all)? event.clientX : e.x;
    	mouseY = (document.all)? event.clientY : e.y;


    	if (curObj){
        	curObj.left= mouseX - dx;
       		curObj.top = mouseY - dy;
       	}
    }


    function mouseUp() {
      	curObj = null;
    }
    
    if (document.layers)
    	document.captureEvents(Event.MOUSEMOVE);
    document.onmousemove=mouseMove;
    document.onmouseup=mouseUp;
</script>

<style type="text/css">
td.errbox {color:red;font-size:10px;font-family:arial;font-weight:bold}
</style>

<script language="javascript">

var displayedID = null;

function closeInputBox(layerID)
{ 	var form = document.forms['f'+layerID];

	var numElements = form.elements.length;
	
	for ( var i=0; i<numElements; i++ )
	{	if  ( form.elements[i].type == 'text' )
			form.elements[i].value = '';
	}

	document.all[layerID].style.visibility = 'hidden';
	document.all[layerID+'inr'].style.visibility = 'hidden';
	
	document.all['td'+layerID.replace('_c','')].style.backgroundColor='white'; 
}

function doTransactionProcess(transaction_num)
{	var div = document.all[transaction_num];
    var subdiv = document.all[transaction_num+'inr'];

	document.all['td'+transaction_num].style.backgroundColor='lightgreen'; 
	
	div_height = (div.style.height.split("px"))[0];
	div_width = (div.style.width.split("px"))[0];
	var top = document.body.clientHeight/2-100 - div_height/2;
	var left = document.body.clientWidth/2 - div_width/2;
	
	div.style.top = top;
	div.style.left = left;
	div.style.visibility='visible';
	subdiv.style.visibility='visible';
	
	
	if ( displayedID != null && displayedID != transaction_num)
		closeInputBox(displayedID);
	displayedID = transaction_num;
	
	var form_name = "f"+transaction_num;
	
//	document.forms[form_name].top.value = top;
//	document.forms[form_name].left.value = left;
	document.forms[form_name].elements[0].focus();
}


function doDepositReject(transaction_num, amount, customer_details)
{	document.all['td'+transaction_num].style.backgroundColor='lightgreen'; 
	
	var mesg  = "Are you sure you want to reject the "+amount+" deposit notification from the customer "+customer_details+"?\n\n";
   	    mesg += "Click OK to Reject this notification or CANCEL to go back";
          
    if ( window.confirm(mesg) )
    {  window.location = '<?=SELF;?>?op=reject_deposit&transaction_num='+transaction_num;
    }
    else
    	document.all['td'+transaction_num].style.backgroundColor='white'; 
   
}


function doWithdrawalCancel(transaction_num)
{	var div_name = transaction_num + "_c";
	var div = document.all[div_name];
	
	
	var subdiv = document.all[div_name+'inr'];

	document.all['td'+transaction_num].style.backgroundColor='lightgreen'; 
	div_height = (div.style.height.split("px"))[0];
	div_width = (div.style.width.split("px"))[0];
	var top = document.body.clientHeight/2-100 - div_height/2;
	var left = document.body.clientWidth/2 - div_width/2;
	
	div.style.top = top;
	div.style.left = left;
	div.style.visibility='visible';
	subdiv.style.visibility='visible';
	
	if ( displayedID != null && displayedID != div_name )
		closeInputBox(displayedID);
	displayedID = div_name;
	
	var form_name = "f"+transaction_num+"_c";
	
	document.forms[form_name].elements[0].focus();
}


</script>

<META HTTP-EQUIV="Refresh" CONTENT="120;URL=<?=$HTTP_SERVER_VARS['REQUEST_URI'];?>">

<div align=center class=bodyHD>Pending Bank Transfers</div>
<P align=center>
This list will refresh automatically every 2 minutes.<br><? printLink(SELF."?show_type=<?=$show_type?>", "Refresh now", 'medium')?>
<P>
<?
//require 'financial_functions.inc';

function sendDepositNotificationSettledEmail($notfn_transaction_num)
{  	$notification_rec = getTransaction($notfn_transaction_num);   
	$settlement_rec   = getTransaction($notification_rec['settle_transaction_num']);
	
	$customer_row     = getCustomer($notification_rec['pun_id']);
	
   ob_start();
   //define html part
?> Hi <?=$customer_row['first_name'];?>,
   <P>
   Your account has been credited with <b><?printInDollars(abs($settlement_rec['amount']));?></b>.
   <P>
   This was the amount
   we received from your funds transfer. You notified us of the transfer on <?=shortDate($notification_rec['time'],true);?> (deposit notification # <?=$notification_rec['transaction_num']?>).
   <P>
   Your reference number for this deposit transaction is <b><?=$settlement_rec['transaction_num']?></b>.  Please
   quote this number if you have any queries about this transaction.
  
<? if ( abs($settlement_rec['amount'])  < $notification_rec['amount'] ) { ?>
		<P>
		The amount we received is less than the amount you notified us of (<?printInDollars($notification_rec['amount']);?>).
		This is possibly due to the deduction of transfer charges or exchange rate losses.
<?	} ?>
   <P>
   Thankyou for choosing to bet with <?=CASINO_FULL_NAME;?>.
   <P>
   <?=CASINO_FULL_NAME;?> Cashier<br>
   <a href="mailto:<?=EMAIL_CASHIER?>" target="_new">&lt;<?=EMAIL_CASHIER?>&gt;</a>
<? 
   $message['html'] = ob_get_clean();
   

   ob_start();

   //define text part
?> Hi <?=$customer_row['first_name'];?>,

   Your account has been credited with <?printInDollars($settlement_rec['amount']);?>.  
   
   This was the amount
   we received from your funds transfer. You notified us of the transfer on <?=shortDate($notification_rec['time']);?> (deposit notification # <?=$notification_rec['transaction_num']?>).

   	Your reference number for this deposit transaction is <?=$settlement_rec['transaction_num']?>.  Please
    quote this number if you have any queries about this transaction.

    <? if ( $settlement_rec['amount']  < $notification_rec['amount'] ) { ?>
	The amount we received is less than the amount you notified us of (<?printInDollars($notification_rec['amount']);?>).
	This is possibly due to the deduction of transfer charges or exchange rate losses.
<?	} ?>



   Thankyou for choosing to bet with <?=CASINO_FULL_NAME;?>.

   <?=CASINO_FULL_NAME;?> Cashier
   <<?=EMAIL_CASHIER?>>
<? 
   $message['text'] = ob_get_clean();
   
   $message = getMultiPartMessage($message['html'], $message['text']);

   $from = EMAIL_CASHIER;
   $from_name = CASINO_FULL_NAME." Cashier";
   $subject   = "Your account has been credited with  ".getInDollars(abs($settlement_rec['amount']));
     
   $email_sender    = new EmailFrom($from, $from_name, 0, 'web', $from, $from_name);
   $email_recipient = new EmailRecipient($customer_row['email'], $customer_row['full_name'], $customer_row['id'], $recipient_type='customer');

   //send notification email
   $mail_sent = SendSingleEmail ($email_sender, $email_recipient, $subject, $message['text'], $message['html']);   
   
   return $mail_sent;
}


function updateWithdrawalReservation($transaction_num, $status, $settlement_transaction_num, $notes="")
{  global $dbh;

   $sql  = "update customer_transaction";
   $sql .= " set ctr_status = '$status'";
   $sql .= ", ctr_settle_transaction_num = '$settlement_transaction_num'";
   $sql .= ", ctr_notes = '$notes'";   
   $sql .= " where ctr_transaction_num   = '$transaction_num'";
   $sql .= " and ctr_tty_id   = ".T_TYPE_WDWL_RSVN;
   $sql .= " and ctr_status = 'pending'";
      
   $dbh->exec($sql);
}



function checkWithdrawal($settle_transaction_number, $transaction_rec)
{  global $dbh, $errors, $fatal_error;
   
   if ( $transaction_rec['tty_id'] != T_TYPE_WDWL_RSVN )
   {  $errors[] = "Invalid transaction";
      $fatal_error = $transaction_rec['transaction_num'];
      return;
   }

   if ( punterIsPlaying($transaction_rec['customer_id']) ) 
   {  $pun_row = getPunter($transaction_rec['customer_id']);
      $name = $transaction_rec['customer_name'];
      $errors[] = "Cannot Apply Withdrawal for the customer $name, because the customer is currently playing a game";
      $fatal_error = $transaction_rec['tranasaction_num'];
      return;
   }                     

   if ( isBlank($settle_transaction_number) )
      $errors[] = "A tracking number must be entered";
   elseif ( transactionNumberExists($settle_transaction_number) )
      $errors[] = "There is already a transaction with the tracking number $settle_transaction_number";

   return count($errors) == 0;
}

function checkDepositFields($settle_track_num, $transaction_rec, $amount_received)
{  global $conn, $errors, $fatal_error;

   if ( $transaction_rec['tty_id'] != T_TYPE_DEPOSIT_NOTFN )
   {  $errors[] = "Invalid transaction";
      $fatal_error = $transaction_rec['tranasaction_num'];
      return;
   }

   if ( isBlank($settle_track_num) )
      $errors[] = "A tracking number must be entered";
   elseif ( transactionNumberExists($settle_track_num) )
      $errors[] = "There is already a transaction with the tracking number $settle_track_num";
      
 
   if ( isBlank($amount_received) )
      $errors[] =  "The actual amount transfer must be entered";
   elseif ( !isValidMoney($amount_received)) 
      $errors[] =  "The actual amount received must be a monetary amount";
   elseif ( $amount_received < 1 )
      $errors[] =  "The amount received must be greater than 0";
   elseif ( $amount_received > $transaction_rec['notification_amount'] )
   	   $errors[] =  "The amount received cannot be greater than the amount notified";
   
   	  

   return count($errors) == 0;
}

globalise('op');



if ( 'process_withdrawal' == $op )
{	if ( $transaction_num = $_POST['transaction_num'] )
	{	if ( $transaction_rec = getTransaction($transaction_num)) 
		{	$settle_transaction_num = $_POST['settle_track_num'];
			$customer_id = $transaction_rec['pun_id'];
			$bac_id = $transaction_rec['bac_id'];
			
			if ( checkWithdrawal($settle_transaction_num, $transaction_rec) )
            {	$linked = false;
            	$reserve_amount = 0;
                                
                if ( numPendingWithdrawalsToAccount($customer_id, $bac_id) > 1 )
                {	$linked = true;
                
                	$sql  = "SELECT ctr_transaction_num, ctr_reserve_amount";
					$sql .= ", ctr_transaction_charge, ctr_wre_number";
					$sql .= " FROM customer_transaction";
					$sql .= " WHERE ( ctr_tty_id = ".T_TYPE_WDWL_RSVN." )";
					$sql .= " AND ctr_status = 'pending'";
					$sql .= " AND ctr_pme_code = 'BAT'";
					$sql .= " and ctr_bac_id = $bac_id";
					$sql .= " and ctr_pun_id = $customer_id";
					$sql .= " order by ctr_time asc";
							
					$rs2 = $dbh->exec($sql);
					$num_rows2 = $rs2->getNumRows();         		       			
										
					for ( $j=0; $j<$num_rows2; $j++ )
					{	$row2 = $rs2->next();
						$tr_num            = $row2['ctr_transaction_num'];
						$tr_reserve_amount = $row2['ctr_reserve_amount'];
						$tr_charge	       = $row2['ctr_transaction_charge'];
						$wre_number 	   = $row2['ctr_wre_number'];

						$linked_array[]['tr_num'] 	  = $tr_num;
						$linked_array[]['wre_number'] = $wre_number;
						        						        					        
						//$wdw_amount += $tr_reserve_amount - $tr_charge;											        
						$reserve_amount += $tr_reserve_amount;
					}					
					$transaction_charge = getWithdrawalTransferCharge($reserve_amount); 
					$linked_str = " <font color=darkorange>[Linked]</font>";
		         }
		         else 	
		         {	$reserve_amount  = $transaction_rec['reserve_amount'];
		         	$transaction_charge = $transaction_rec['transaction_charge'];
		         }
                                     	                   	                     	
                 //apply the deposit to the customer's account
                 $dbh->begin();
//                    $dbh->lockRow('punter_credit', array('pcr_pun_id'=>$customer_id));

                    $settlement_number = 
                     doBankTransferWithdrawal($reserve_amount, $transaction_charge, $customer_id,  $bac_id, $settle_number, $aus_id=$_SESSION['admin_id'], $mngr_id="NULL");							   
                                            
					if ($linked) //update all linked withdrawal reservations
					{	foreach ( $linked_array as $key => $val )
						{	updateWithdrawalReservation($val['tr_num'], 'completed', $settlement_number);
							setWdwlRequestStatus($val['wre_number'], 'completed');
						}
             		}
                    else //update single withdrawal reservation
                    {	updateWithdrawalReservation($transaction_num, 'completed', $settlement_number);
                     	setWdwlRequestStatus($wre_number, 'completed');
                    }
                 $dbh->commit();
			}
			else
				$div_display = $transaction_num;
		}
	}
}
elseif ( 'process_deposit' == $op )
{	if ( $transaction_num = $_POST['transaction_num'] )
	{	
		if ( $transaction_rec = getTransaction($transaction_num)) 
		{	$settle_track_num = $_POST['settle_track_num'];
			$amount_received = $_POST['amount_received'];
			$customer_id = $transaction_rec['pun_id'];
			$customer_row = getCustomer($customer_id);
			
			if ( punterIsPlaying($customer_id) && 'NONFINANCIAL' == $customer_row['member_type'] )
			{	$msg  = "The customer ".$customer_row['full_name']." is currently playing a game\n";
				$msg .= " and is a Non Financial member.  A deposit cannot be performed until";
				$msg .= " the customer is no longer playing.\n\nPlease try again later.";
			
				jalert($msg);
		
			}
			else
			{	if ( checkDepositFields($settle_track_num, $transaction_rec, $amount_received) )
	            {	//apply the deposit to the customer's account
	               	//$dbh->lockRow('punter_credit', array('pcr_pun_id'=>$customer_id));
	                  
	               	$dbh->begin();
	                	doBankTransferDeposit ($amount_received, $aus_id=$_SESSION['admin_id'], $settle_track_num, $transaction_rec);

	                $dbh->commit();  
					sendDepositNotificationSettledEmail($transaction_num);
	            }
	            else
	            	$div_display = $transaction_num;
			}
		}
	}
}
elseif ( 'reject_deposit' == $op )
{	
	if ( $transaction_num = $_GET['transaction_num'] )
	{	if ( $transaction_rec = getTransaction($transaction_num) ) 
		{	if ( T_TYPE_DEPOSIT_NOTFN == $transaction_rec['tty_id'] && 'pending' == $transaction_rec['status'])
			{	$dbh->begin();
				doRejectDepositNotification($transaction_rec, $_SESSION['admin_id']);
				$dbh->commit();
			}
		}
	}	
}
elseif ( 'cancel_withdrawal' == $op )
{	
	if ( $transaction_num = $_POST['transaction_num'] )
	{	$transaction_rec = getTransaction($transaction_num);
		if ( T_TYPE_WDWL_RSVN == $transaction_rec['tty_id'] && $transaction_rec['status'] == 'pending' ) 
		{	if ( isBlank($_POST['cancel_reason']) )
			{	$errors[] = "A Reason for this cancellation must be entered";
				$div_display = $transaction_num."_c";
			}
			else
				$settle_transaction_num = doCancelWithdrawalReservation($transaction_rec, $_POST['cancel_reason'], $aus_id=$_SESSION['admin_id'], $mngr_id="NULL");
		}
	}
}
        

$linked_array = array();

/***** Get withdrawal authorizations *****/
//get all withdrawal authorization
$sql  = "SELECT ctr_id, ctr_time, ctr_tty_id, pme_name, ctr_notification_amount";
$sql .= ", ctr_reserve_amount, ctr_pun_id, ctr_deposit_src_bank, ctr_deposit_trans_ref";
$sql .= ", ctr_deposit_trans_date, ctr_bac_id, ctr_transaction_num, ctr_bac_id";
$sql .= ", ctr_transaction_charge, ctr_notes";
$sql .= " FROM customer_transaction, payment_method ";
$sql .= " where ctr_pme_code = pme_code";
$sql .= " AND ctr_status = 'pending'";

if ( 'incoming' == getPostGetVar('show_type') )  
	$sql .= " AND ( ctr_tty_id = ".T_TYPE_DEPOSIT_NOTFN.")";
elseif ( 'outgoing' == getPostGetVar('show_type') ) 
	$sql .= " AND ( ctr_tty_id = ".T_TYPE_WDWL_RSVN.")";	
else
	$sql .= " AND ( ctr_tty_id = ".T_TYPE_DEPOSIT_NOTFN." OR ctr_tty_id = ".T_TYPE_WDWL_RSVN.")";
$sql .= " order by ctr_time asc";


$rs = $dbh->exec($sql);
$num_rows = $rs->getNumRows();
globalise('show_type');
?>
<P align=center class=info>
<form action="<?=SELF;?>" method=POST>
   <input type=hidden name=page value="<?=$page;?>">
   <input type=hidden name=dir value="<?=$dir;?>">
   <b>Show:</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type=radio name=show_type value="" <? if ( !$show_type ) echo 'checked'; ?> onClick="this.form.submit()">All
   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
   <input type=radio name=show_type value=incoming <? if ( 'incoming' == $show_type ) echo 'checked'; ?> onClick="this.form.submit()">Incoming Transfers
   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
   <input type=radio name=show_type value=outgoing <? if ( 'outgoing' ==  $show_type ) echo 'checked'; ?> onClick="this.form.submit()">Outgoing Transfers
</form>
<?


if ( $num_rows > 0 )
{ //filter by withdrawal or deposit
?>

<div align=center class=background2>
   	<table cellpadding="4" cellspacing="1" border=0 border="0" bgcolor=gray>
      <tr valign=top>
         <td class=label>Time</td>         
         <td class=label>Type</td>
         <td class=label>Transfer Amount</td>
         <td class=label>Details</td>
         <td class=label align=center>Customer</td>
         <td class=label>Notes</td>
         <td class=label align=center>&nbsp;</td>
      </tr>
<?	
      while($rs->hasNext()) 
      {	 $row = $rs->next();
         
         $ctr_id           		= $row['ctr_id'];
         $time             		= $row['ctr_time'];
	     $tty_id             	= $row['ctr_tty_id'];
         $payment_method   		= $row['pme_name'];
         $notification_amount 	= $row['ctr_notification_amount'];
         $reserve_amount        = $row['ctr_reserve_amount'];
         $customer_id           = $row['ctr_pun_id'];
         $source_bank_name 		= $row['ctr_deposit_src_bank'];
         $bank_trans_ref   		= $row['ctr_deposit_trans_ref'];
         $bank_trans_date  		= $row['ctr_deposit_trans_date'];
         $bank_account_id 	    = $row['ctr_bac_id']; 
      	 $transaction_num 		= $row['ctr_transaction_num'];
         $transaction_charge	= $row['ctr_transaction_charge'];
         $notes 				= $row['ctr_notes'];

      	 $customer_row = getCustomer($customer_id);
      	 $customer_name = $customer_row['full_name'];		
         $customer_number = $customer_row['customer_number'];		
      	 
      	 //showval($customer_row);
         $address = '<div style="color:green">'.getAddressString($customer_row).'</div>';
         $customer = '<div style="color:green;font-weight:bold">'.$customer_number.'&nbsp;'.$customer_row['full_name'].'</div>';
         
         
         
         if ( $customer_row['phone_home'] )
            $phone = '<i>Home ph</i>: '.$customer_row['phone_home'].'<br>';
         		
         if ( $customer_row['phone_business'] )
            $phone .= '<i>Business ph</i>: '.$customer_row['phone_business'].'<br>';
         		   
         if ( $customer_row['phone_mobile'] )
            $phone .= '<i>Mobile ph</i>: '.$customer_row['phone_mobile'].'<br>';
        
           
         $customer .= $address;
         
         if ($phone )
         	$customer .= "$phone<br>"; 
         
         
                  
         //$<br>phone_number";         		
         
      	 $transfer_amount = $reserve_amount - $transaction_charge;
      	       	 
      	 if ( T_TYPE_DEPOSIT_NOTFN == $tty_id ) 
         {	$transactions[$transaction_num] = array('tty_id'=>$tty_id,'amount'=>$notification_amount,'customer_name'=>$customer_name, 'phase'=>'');
		        	
?>       <tr valign=top>
		    <td class=contentwhitebg ><?=shorterDate($time, true);?></span></td>
            <td class=contentwhitebg><font color=blue>Incoming</font></font></td>
            <td class=contentwhitebg align=right><font color=blue><?printInDollars($notification_amount);?></font></td>
            <td class=contentwhitebg>
               <table cellpadding=1 cellspacing=1 border=0 >
                  <tr valign=top>
                     <td class=small><b>Trans. Date:</b></font></td> 
                     <td class=small><?=shorterDate($bank_trans_date)?></font></td>
                  </tr>
                  <tr valign=top>
                     <td class=small><b>Source Bank:</b></font></td>
                     <td class=small><?=$source_bank_name?></font></td>
                  </tr>
			<? if ( !isBlank($bank_trans_ref) ) { ?>
                  <tr valign=top>
                     <td class=small><b>Trans. Ref:</b></font></td>
                     <td class=small><?=$bank_trans_ref?></font></td>
                  </tr>
			<? } ?>     
		  		 </table>               
            </td>
            <td class=contentwhitebg>
                <?=$customer?><br>
            </td>
            <td class=contentwhitebg>
            	
             <?=ereg_replace("\n", "<br>",stripslashes($notes));?>&nbsp;</td>
         <form action="<?=SELF;?>" method="POST">
         	<?printHiddenInput("transaction_num");?>
         	<?printHiddenInput("transaction_num");?>
            <td class=contentwhitebg id="td<?=$transaction_num?>">
<?  
	if ( hasCustomerTransactionPriv($_SESSION['aty_code']) )
	{
?>	             
               <input type="button" value="Process" onClick="javascript:doTransactionProcess ('<?=$transaction_num;?>')"> 
               <input type="button" value="  Reject " onClick="javascript:doDepositReject('<?=$transaction_num;?>', '<?printInDollars($notification_amount)?>', '<?="$customer_number/$customer_name"?>')">
<?}?>
               </td>
         </form>
<?      
         } 
         elseif ( T_TYPE_WDWL_RSVN == $tty_id ) 
         {  	
         	//get withdrawal request details
         		//destination bank account
         		if ( in_array($ctr_id, $linked_array) )
         			continue;
         		
         		$linked = false;
         		

         		if ( numPendingWithdrawalsToAccount($customer_id, $bank_account_id) > 1 )
         		{	//linked withdrawals
         			$sql  = "SELECT ctr_id, ctr_time, ctr_reserve_amount";
					$sql .= ", ctr_pun_id, pun_customer_number, pun_first_name";
					$sql .= ", pun_last_name, ctr_bac_id, ctr_transaction_num";
					$sql .= ", ctr_bac_id, ctr_transaction_charge, ctr_notes";
					$sql .= " FROM customer_transaction, punter";
					$sql .= " WHERE  ctr_tty_id = ".T_TYPE_WDWL_RSVN;
					$sql .= " AND ctr_status = 'pending'";
					$sql .= " AND ctr_pme_code = 'BAT'";
					$sql .= " and ctr_pun_id = pun_id";
					$sql .= " and ctr_bac_id = $bank_account_id";
					$sql .= " and pun_id = $customer_id";
					$sql .= " order by ctr_time asc";
					
					$rs2 = $dbh->exec($sql);
					$num_rows2 = $rs2->getNumRows();         		       			
					
					$wdw_amount =0;
					for ( $j=0; $j<$num_rows2; $j++ )
					{	$row2 = $rs2->next();
						$ctr_id           		= $row2['ctr_id'];
				        $time             		= $row2['ctr_time'];
				        $reserve_amount        	= $row2['ctr_reserve_amount'];
				        $customer_id           	= $row2['ctr_pun_id'];
				        $customer_number  		= $row2['ctr_customer_numbner'];
				        $punter_fname      		= $row2['pun_first_name'];
				        $punter_lname      		= $row2['pun_last_name'];
				        $wre_number      		= $row2['pun_wre_number'];
				        $transaction_num 		= $row2['ctr_transaction_num'];
				        $bank_account_id		= $row2['ctr_bac_id'];
				        $transaction_charge	    = $row2['ctr_transaction_charge'];
				        $notes	    			= $row2['ctr_notes'];
    
				        $linked_array[] = $ctr_id;
				        $sum_reserve_amount += $reserve_amount;
				        			
						if ( $notes )
							$all_notes .= "<br>$transaction_num:</b> $notes<br>";
					}			
					$transaction_charge = getWithdrawalTransferCharge($sum_reserve_amount);
					$transfer_amount = $sum_reserve_amount - $transaction_charge;
					$notes = $all_notes;
					
					$linked = true;
         		}
         		      
            	$transactions[$transaction_num] = array('tty_id'=>$tty_id,'amount'=>$transfer_amount,'customer_name'=>$customer_name, 'transaction_charge'=>$transaction_charge);
            	
?>       	<tr bgcolor=white valign=top>
				<td class=contentwhitebg><?=shorterDate($time, true);?><?if ($linked) echo "<br><font color=darkorange>[linked]</font>";?></td>
	            <td class=contentwhitebg><font color=red>Outgoing</font></td>
	            <td align=right class=contentwhitebg><font color=red><?printInDollars($transfer_amount);?></font></td>  
	            <td class=contentwhitebg><? printAccountDetailsFormatted($bank_account_id); ?></td>
	            <td class=contentwhitebg><?=$customer?></td>
	            <td class=contentwhitebg><?=ereg_replace("\n", "<br>",stripslashes($notes));?>&nbsp;</td>
	         <form action="<?=SELF;?>" method=POST onSubmit="return setInternalNumber(this, 'withdrawal')">
	            <td  class=contentwhitebg id="td<?=$transaction_num?>">
	               <!--<input type="button" value="Process" onClick="openWindow('/admin/withdrawal_process.html?transaction_num=<?=$transaction_num?>', 'withdrawal_process', 480, 450, 'yes', 'yes')"><br>-->
<?  
	if ( hasCustomerTransactionPriv($_SESSION['aty_code']) )
	{
?>		<input type="button" value="Process" onClick="javascript:doTransactionProcess('<?=$transaction_num?>')">
	    <input type="button" value="  Cancel " onClick="javascript:doWithdrawalCancel('<?=$transaction_num;?>')">
<?	} 
?>
	            </td>
	         </form>
	         </tr>
<?       	}
	  }  
?>
	</table>
<?

$i=1;
foreach ( $transactions as $transaction_num => $details)
{	if ( T_TYPE_DEPOSIT_NOTFN == $details['tty_id'] )
	{	if ( $div_display == $transaction_num )	
		{	$visibility = 'visible';
			jscriptBlock("document.all['tdd$transaction_num'].style.backgroundColor='lightgreen'");
			$top = $_POST['top'];
			$left = $_POST['left'];	
		}
		else
		{	$visibility = 'hidden';
			$top  = 0;
			$left = 0;
		}		
?>
<div id="<?=$transaction_num?>" align="center" valign=middle style="position:absolute; top:<?=$top?>; left:<?=$left?>; width:300; height:30; z-index:<?=$i;?>; border:outset;border-width:2; border-left-color:white;border-top-color:white;border-right-color:silver;border-bottom-color:silver; padding:0;visibility:<?=$visibility?>">
<div onmousedown="dragThis('<?=$transaction_num?>')" style="vertical-align:middle;width:299;background-color:green;height:22;padding:2;font-family:arial;color:white;font-weight:bold;font-size:8pt">
Process withdrawal
</div>
<div align="center" id="<?=$transaction_num?>inr" valign=middle style="position:relative; width:300; height:30; z-index:<?=$i+1;?>; background-color:#d1d1d1; border:none; border-color:silver; padding:4;visibility:<?=$visibility?>;font-size:8pt;font-family:arial">
<form name="f<?=$transaction_num?>" action="<?=SELF;?>" method="POST" onSubmit="this.left.value=document.all['<?=$transaction_num?>'].style.left;this.top.value=document.all['<?=$transaction_num?>'].style.top;return true;">
<? 		if ( $fatal_error == $transaction_num )
		{				
?>			<P class=errbox><?=$errors[0];?></P>
			<P>
			<input type="button" value="OK" onClick="javascript:window.location='<?=SELF;?>?show_type=<?=$show_type?>'">
<?		}
		else
		{	 
?>
	Enter details for transfer deposit notification
	of <?printInDollars($details['amount'])?> from  <?=$details['customer_name']?>
	<P align=left class=errbox>
<?
		if ( is_array($errors) )
		{
?>
<table cellpadding=2 cellspacing=0 border=0>
<?
			foreach ( $errors as $key=>$val )
				echo "<tr valign=top><td class=errbox>-</td><td class=errbox>$val</td></tr>";	
?>
</table>
<?		}
?>	</P>
<table cellpadding=1 cellspacing=1>
	<tr>
		<td class=small><b>Actual Amount Received</td>
		<td class=small><input type="text" name="amount_received" value="<?=$amount_received?>" size=15></td>
	</tr>
	<tr>
		<td class=small><b>Transaction Reference #</td>
		<td class=small><input type="text" name="settle_track_num" value="<?=$settle_track_num?>" size=15></td>
	</tr>	
</table><br>
<input type="submit" value="Submit"> 
<? if ( is_array($errors) ) {?>
<input type="button" value="Cancel" onClick="javascript:window.location='<?=SELF;?>?show_type=<?=$show_type?>'">
<?	} else { ?>
<input type="button" value="Cancel" onClick="closeInputBox('<?=$transaction_num?>')">
<?	} ?>
<?printHiddenInput("top", $top);?>
<?printHiddenInput("left", $left);?>
<?printHiddenInput("transaction_num", $transaction_num);?>
<?printHiddenInput("op", "process_deposit");?>	
</form>
</div>
</div>
<?		}
	}
	elseif ( T_TYPE_WDWL_RSVN == $details['tty_id'] )
	{	if ( $div_display == $transaction_num )	
		{	$visibility = 'visible';
			jscriptBlock("document.all['td$transaction_num'].style.backgroundColor='lightgreen'");
			$top = $_POST['top'];
			$left = $_POST['left'];	
		}
		else
		{	$visibility = 'hidden';
			$top  = 0;
			$left = 0;
		}
		
		//if ( $visibility == 'visible' )
		//	jscriptBlock("document.all['tdw$transaction_num'].style.backgroundColor='lightgreen'");
?>
<div  id="<?=$transaction_num?>" align="center" valign=middle style="position:absolute; top:<?=$top?>; left:<?=$left?>; width:250; height:30; z-index:<?=$i;?>;  border:outset; border-width:2; border-left-color:white;border-top-color:white;border-right-color:silver;border-bottom-color:silver; padding:0;visibility:<?=$visibility?>">
<div onmousedown="dragThis('<?=$transaction_num?>')" style="width:249;background-color:green;height:20;padding:2;font-family:arial;color:white;font-weight:bold;font-size:8pt">
Process withdrawal
</div>
<div align="center" id="<?=$transaction_num?>inr" valign=middle style="position:relative; width:250; height:30; z-index:<?=$i+1;?>; background-color:#d1d1d1; border:none; border-color:silver; padding:4;visibility:<?=$visibility?>;font-size:8pt;font-family:arial">
<form name="f<?=$transaction_num?>" action="<?=SELF;?>" method="POST" onSubmit="this.left.value=document.all['<?=$transaction_num?>'].style.left;this.top.value=document.all['<?=$transaction_num?>'].style.top;return true;">
<? 	if ( $fatal_error == $transaction_num )
	{	
?>		<P align=left class=errbox><?=$errors[0];?></P>
		<P>
		<input type="button" value="OK" onClick="javascript:window.location='<?=SELF;?>?show_type=<?=$show_type?>'">
<?	}
	else 
	{
?>		Enter Transaction Reference Number for outgoing transfer of <?printInDollars($details['amount'])?> to <?=$details['customer_name']?>
		<P class=errbox>
<?
		if ( is_array($errors) )
		{
?>
<table cellpadding=2 cellspacing=0 border=0>
<?
			foreach ( $errors as $key=>$val )
				echo "<tr valign=top><td class=errbox>-</td><td class=errbox>$val</td></tr>";	
?>
</table>
<?		}
?>		</P>
<?			
	}
?>
<input type="text" name="settle_track_num" value="<?=$trans_ref_num?>" size=15><br><br>
<input type="submit" value="Submit"> 
<? if ( is_array($errors) ) {?>
<input type="button" value="Cancel" onClick="javascript:window.location='<?=SELF;?>?show_type=<?=$show_type?>'">
<?	} else { ?>
<input type="button" value="Cancel" onClick="closeInputBox('<?=$transaction_num?>')">
<?	} ?>
<?printHiddenInput("top", $top);?>
<?printHiddenInput("left", $left);?>
<?printHiddenInput("transaction_num", $transaction_num);?>
<?printHiddenInput("op", "process_withdrawal");?>
</form>
</div>
</div>
<?
if ( $div_display == $transaction_num."_c" )	
{	$visibility = 'visible';
	jscriptBlock("document.all['td$transaction_num'].style.backgroundColor='lightgreen'");
	$top = $_POST['top'];
	$left = $_POST['left'];	
}
else
{	$visibility = 'hidden';
	$top  = 0;
	$left = 0;
}



?>
<div id="<?=$transaction_num?>_c" align="center" valign=middle style="position:absolute; top:<?=$top?>; left:<?=$left?>; width:250; height:30; z-index:<?=$i;?>; border:outset; border-width:2; border-left-color:white;border-top-color:white;border-right-color:silver;border-bottom-color:silver; padding:0; visibility:<?=$visibility?>">
<div onmousedown="dragThis('<?=$transaction_num?>_c')" style="width:249;background-color:green;height:20;padding:2;font-family:arial;color:white;font-weight:bold;font-size:8pt">
Process withdrawal
</div>
<div align="center" id="<?=$transaction_num?>_cinr" valign=middle style="position:relative; width:250; height:30; z-index:<?=$i+1;?>; background-color:#d1d1d1; border:none; border-color:silver; padding:4;visibility:<?=$visibility?>;font-size:8pt;font-family:arial">
<form name="f<?=$transaction_num?>_c" action="<?=SELF;?>" method="POST" onSubmit="this.left.value=document.all['<?=$transaction_num?>_c'].style.left;this.top.value=document.all['<?=$transaction_num?>_c'].style.top;return true;">
Enter the reason for cancelling the outgoing transfer of <?printInDollars($details['amount'])?> plus <?printInDollars($details['transaction_charge'])?> transfer charge to <?=$details['customer_name']?>
<P class=errbox>
<?
if ( is_array($errors) )
{
?>
<table cellpadding=2 cellspacing=0 border=0>
<?	foreach ( $errors as $key=>$val )
		echo "<tr valign=top><td class=errbox>-</td><td class=errbox>$val</td></tr>";	
?>
</table>
<?		
}
?>		
</P>
<input type="text" name="cancel_reason" value="" size=30><br><br>
<input type="submit" value="Submit"> 
<? if ( is_array($errors) ) {?>
<input type="button" value="Cancel" onClick="window.location='<?=SELF;?>?show_type=<?=$show_type?>'">
<?	} else { ?>
<input type="button" value="Cancel" onClick="closeInputBox('<?=$transaction_num?>_c')">
<?	} ?>
<?printHiddenInput("top", $top);?>
<?printHiddenInput("left", $left);?>
<?printHiddenInput("transaction_num", $transaction_num);?>
<?printHiddenInput("op", "cancel_withdrawal");?>
</form>
</div>
</div>
<?	}
	$i++;
}  
}
else
{ 
?><br><br><br><div align=center>   
<?   printInfoMessage("No records found");?>
</div>
<?
}
?>  
</div>
