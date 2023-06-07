<?php
if (!defined('_FINANCIAL_FUNCTIONS_INC')) {
 define('_FINANCIAL_FUNCTIONS_INC', 1);

 define("T_TYPE_CORRECTION",6);
 define("T_TYPE_DEPOSIT",7);
 define("T_TYPE_WITHDRAWAL",8);
 define("T_TYPE_GAME_RESULT",4);
 define("T_TYPE_BONUS",9);

// define("T_TYPE_DEPOSIT_NOTFN",4);
// define("T_TYPE_WDWL_RSVN",5);
// define("T_TYPE_DEPOSIT_NOTFN_REJ",8);
// define("T_TYPE_WDWL_RSVN_RETN",9);
 define("T_TYPE_TIP",9);

/********************************************************/
/*********** GENERAL WITHDRAWAL FUNCTIONS ***************/
/********************************************************/

function getCustomerTotalSum($customer_id){
  global $dbh;
  $sql = "SELECT SUM(cus_res_rake) AS cus_res_rake, SUM(cus_res_bet) AS cus_res_bet, SUM(cus_res_win) AS cus_res_win FROM customer_result_daily WHERE cus_res_pun_id = $customer_id";
  $rs  = $dbh->exec($sql);
  $rec = $rs->next();
  return $rec;
}

function getCustomerBalance($customer_id){
	global $dbh;

   $rs       = $dbh->exec("select pcr_credits as balance from punter_credit where pcr_pun_id=$customer_id");
   $num_rows = $rs->getNumRows();

   if ($num_rows != 1 )
      die("Could not get punter's balance - punter credit record does not exist");

   $rec = $rs->next();
   return $rec['balance'];
}


function getCustomerAvailableBalance($customer_id){
	global $dbh;

   $rs = $dbh->exec("select (pcr_credits-pcr_reserved_funds) as balance from punter_credit where pcr_pun_id=$customer_id");
   $num_rows = $rs->getNumRows();

   if ($num_rows != 1 )
      die("Could not get punter's balance - punter credit record does not exist");

   $rec = $rs->next();
   return $rec['balance'];
}

function getCustomerBonusBalance($customer_id){
        global $dbh;

        $rs = $dbh->exec("select (pcr_bonus) as balance from punter_credit where pcr_pun_id=$customer_id");
        $num_rows = $rs->getNumRows();

        if ($num_rows != 1 )
            die("Could not get punter's balance - punter credit record does not exist");

        $rec = $rs->next();
        return $rec['balance'];
    }


function getJurAvailableCredit($jur_id) {
  global $dbh;
  $q = "SELECT jur_available_credit AS avail_balance  from jurisdiction where jur_id=$jur_id ";
  $rs_data = $dbh->queryOne($q);
  return $rs_data;
}

function getJurCredit($jur_id) {
  global $dbh;
  $q = "SELECT get_jur_balance($jur_id) AS balance";
  $rs = $dbh->exec($q);
  $rs_data = $rs->next();
  return $rs_data["balance"];
}

function getJurReservedFunds($jur_id) {
  global $dbh;
  $q = "SELECT jur_reserved_fund FROM jurisdiction WHERE jur_id=$jur_id";
  $rs = $dbh->exec($q);
  $rs_data = $rs->next();
  return $rs_data["jur_reserved_fund"];
}


function getLastDepositTime ( $pun_id ) {
  global $dbh;

  $sql  = "select ctr_time from customer_transaction";
  $sql .= " where ctr_amount > 0";
  $sql .= " and ctr_tty_id = ".T_TYPE_DEPOSIT;
  $sql .= " and ctr_pun_id = $pun_id";
  $sql .= " order by ctr_time desc limit 1";

  $rs = $dbh->exec($sql);
  $num_rows = $rs->getNumRows();

  if ( $num_rows > 0 ) {
    $row = $rs->next();
    $time = $row['ctr_time'];
  }
  return $time;
}


function getWithdrawalRequest($wre_number) {
  global $dbh;

  $sql  = "SELECT wre_id, wre_pun_id, wre_time, wre_amount, wre_status";
  $sql .= ", wre_internal_number, wre_amount_credit_card, wre_amount_bank_transfer, wre_bac_id";
  $sql .= " FROM withdrawal_request";
  $sql .= " where wre_internal_number = $wre_number";
  $rs = $dbh->exec($sql);
  $num_rows = $rs->getNumRows();
  //withdrawal request record found

  if ( 1 == $num_rows ) {
    $row  = $rs->next();
    $id                   = $row['wre_id'];
    $pun_id               = $row['wre_pun_id'];
    $time                 = $row['wre_time'];
    $amount               = $row['wre_amount'];
    $status               = $row['wre_status'];
    $reference_number     = $row['wre_internal_number'];
    $amount_credit_card   = $row['wre_amount_credit_card'];
    $amount_bank_transfer = $row['wre_amount_bank_transfer'];
    $bac_id               = $row['bac_id'];

    $withdrawal_request = array('id'=>$id,'pun_id'=>$pun_id, 'time'=>$time,'amount'=>$amount, 'status'=>$status, 'internal_number'=>$reference_number, 'amount_credit_card'=>$amount_credit_card,
         'amount_bank_transfer'=>$amount_bank_transfer, 'bac_id' => $bac_id, 'transaction_num'=>$reference_number);
  }
  return $withdrawal_request;
}


function hasFailedWithdrawal ($pun_id)
{  global $dbh;

   $sql  = "select count(*) from withdrawal_request";
   $sql .= " where wre_pun_id = $pun_id";
 $sql .= " and wre_status = 'check'";

   $num_failed_wre = $dbh->countQuery($sql);

   return ( $num_failed_wre > 0 );
}


function getWithdrawalTransferCharge($amount)
{  if ( $amount < 1 )
      return 0;

   $cfg = getCfgFile(CONFIG_DIR."/charges.cfg");

   if ( $amount < 2000 )
   {  $flat = $cfg['out_flat_under_2000'];
      $percentage = $cfg['out_percentage_under_2000'] / 100 * $amount;
      $transaction_charge = $flat + $percentage;

   }
   else
   {  $flat = $cfg['out_flat_2000_plus'];
      $percentage = $cfg['out_percentage_2000_plus'] / 100 * $amount;
      $transaction_charge = $flat + $percentage;
   }

   return $transaction_charge;
}


function getDepositTransferCharge($amount)
{  if ( $amount < 1 )
      return 0;

   $cfg = getCfgFile(CONFIG_DIR."/charges.cfg");

   $flat = $cfg['in_flat'];
   $percentage = $cfg['in_percentage'] / 100 * $amount;
   $transaction_charge = $flat + $percentage;

   return $transaction_charge;
}


function bankTransferWithdrawalAvailable($punter_row, $max_to_bank_account)
{  global $dbh;

   if ( $max_to_bank_account > 0 )
   {  if ( $punter_row['identified'] )
         if ( hasIdentifiedBankAccounts($punter_row['id']) )
            return 1;
         else
            return -2;
      else
         return -1;
   }
   else
      return 0;
}


function getWithdrawalAmount($amount)
{  return  $amount + getWithdrawalTransferCharge($amount);
}


function getAmountWithdrawnBTReserved($wre_number)
{  global $conn;

   $amount = 0;
   $sql  = "select ctr_reserve_amount from customer_transaction";
   $sql .= " where ctr_wre_number = $wre_number";
   $res = sqlExec($conn, $sql);
   $num_rows = sqlNumRows($res);

   if ( 1 == $num_rows )
      $row = sqlResult($res);
   $amount = $row[0];

   return $amount;
}


function isFraudError($subError)
{  return  ( 1081 == $subError || 1082 == $subError || 1083 == $subError);
}


function printAccountDetailsFormatted($bac_id, $return=false)
{  global $dbh;

   $details = getAccountDetails($bac_id);
ob_start();
?><table cellpadding=1 cellspacing=1 border=0>
      <tr>
         <td class=small><b>Acct Name:</b></td>
         <td class=small><?=$details['name']?></td>
      </tr>
      <tr>
         <td class=small><b>Acct Sort Code:</b></td>
         <td class=small><?=$details['sort_code']?></td>
      </tr>
      <tr>
         <td class=small><b>Acct Number:</b></td>
         <td class=small><?=$details['number']?></td>
      </tr>
      <tr>
         <td class=small><b>Acct Swift Code:</b></td>
         <td class=small><?=$details['swift_code']?></td>
      </tr>
   </table>
<?
   $str = ob_get_contents();
ob_end_clean();
   if ($return)
      return $str;
   else
      echo $str;
}



function accountSelector($customer_id)
{  global $dbh;

   if ( ! $customer_id )
      return;
   $sql  = "select bac_id, bac_holder_name, bac_sort_code, bac_number";
   $sql .= " from bank_account";
   $sql .= " where bac_pun_id = $customer_id and bac_identified = 1";
   $rs   = $dbh->exec($sql);
   $num_rows = $rs->getNumRows();

   if ( $num_rows > 0 )
   {
?> <select name="bank_account">
      <option value=""> - Choose bank account -
<?    while ( $rs->hasNext() )
      {  $row = $rs->next();
         $bac_id          = $row['bac_id'];
         $bac_holder_name = $row['bac_holder_name'];
         $bac_sort_code   = $row['bac_sort_code'];
         $bac_number      = $row['bac_number'];

         $text_str = "$bac_sort_code $bac_number $bac_holder_name";
?>    <option value="<?=$bac_id?>" <? if ( $GLOBALS['bank_account'] == $bac_id || 1 == $num_rows)  echo " selected"; ?>><?=$text_str?>
<?    }
?> </select><br>
<? }
}


function hasBankAccounts($pun_id)
{  global $dbh;

   return $dbh->countQuery("select count(*) from bank_account where bac_pun_id = $pun_id");
}


function bankAccountIdentified($bac_id)
{  global $dbh;

   $identified = 0;

   $sql = "select bac_identified from bank_account where bac_id = $bac_id";
   $rs = $dbh->exec($sql);
   $num_rows = $rs->getNumRows();

   if ( 1 == $num_rows )
   {  $row = $rs->next();
      $identified = $row['bac_identified'];
   }
   return $identified;
}


function hasIdentifiedBankAccounts($pun_id)
{  global $dbh;

   $num_identified = 0;

   $sql = "select count(*) from bank_account where bac_pun_id = $pun_id";
   $sql .= "and bac_identified = 1";

   $num_identified = $dbh->countQuery($sql);

   return $num_identified;
}


function bankAccountBelongsTo($bac_id, $pun_id)
{  global $dbh;

   $num_identified = 0;

   $sql = "select count(*) from bank_account where bac_pun_id = $pun_id";
   $sql .= "and bac_id = $bac_id";

   $count = $dbh->countQuery($sql);

   return ($count == 1);
}


function getAccountDetails($bac_id)
{  global $dbh;

   $details = array();

   $sql   = "select bac_id, bac_holder_name, bac_sort_code, bac_number";
   $sql  .= ", bac_swift_code, bac_branch_address, bac_identified, bac_pun_id";
   $sql .= " from bank_account";
   $sql .= " where bac_id = $bac_id";
   $rs  = $dbh->exec($sql);
   $num_rows = $rs->getNumRows();

   if ( $num_rows > 0 )
   {  $row = $rs->next();
      $details['id']             = $row['bac_id'];
      $details['name']           = $row['bac_holder_name'];
      $details['sort_code']      = $row['bac_sort_code'];
      $details['number']         = $row['bac_number'];
      $details['swift_code']     = $row['bac_swift_code'];
      $details['branch_address'] = $row['bac_branch_addres'];
      $details['identified']     = $row['bac_identified'];
      $details['pun_id']         = $row['bac_pun_id'];
   }
   return $details;
}


function voidCreditCardPurchase($dcc_id)
{  global $conn;

   $sql = "update deposit_credit_card set dcc_void = 1 where dcc_firepay_trans_num = $dcc_id";
   sqlExec($conn, $sql);
}


function hasVoidCreditCardPurchase($wre_number)
{  global $conn;

   $sql  = "select count(*) from deposit_credit_card where dcc_void = 1";
   $sql .= " and dcc_firepay_trans_num in";
   $sql .= " ( select ccr_purch_txn_num from credit_to_card";
   $sql .= " where ccr_status = 'failed' and ccr_wre_number = $wre_number )";

   $num = countQuery($sql);
   return ($num > 0);
}


function transactionNumberExists($settle_number)
{  global $dbh;

   $sql = "select count(*) from customer_transaction where ctr_transaction_num = '$settle_number'";
   $ret = $dbh->countQuery($sql);
   if ( $ret > 0 )
      return true;
   return false;
}





function hasCreditsAfter($purch_txn_num, $time){
   global $dbh;
   $sql  = "select count(*) from credit_to_card";
   $sql .= " where ccr_purch_txn_num = $purch_txn_num";
   $sql .= " and ccr_time > '$time'";
   $count = countQuery($sql);

   return ($count > 0);
}

function nextTxnNumberTip(){
   global $dbh;
   return 'tip'.$dbh->nextSequence('SEQ_TIP_TXN_NUM');
}

function nextTxnNumberDepositNotRej(){
	global $dbh;
   return 'dnr'.$dbh->nextSequence('SEQ_DNOREJ_ID');
}


function nextTxnNumberAdjustment(){
   global $dbh;
   return 'crn'.$dbh->nextSequence('SEQ_ADJ_TXN_NUM');
}

function nextTxnBonusAdjustment(){
   global $dbh;
   return 'bon' . $dbh->nextSequence('SEQ_BON_TXN_NUM');
}

function nextTxnNumberDepositNot(){
   global $dbh;
   return 'dno'.$dbh->nextSequence('SEQ_DNO_NUM');
}

function nextTxnNumberBTSettle()
{  global $dbh;
   return 'bts'.$dbh->nextSequence('SEQ_BT_SETTLE_TXN_NUM');
}


function toTxnNumberCC($merchant_txn_number)
{  global $dbh;
   return 'crd'.$merchant_txn_number;
}


function getMerchantTxnNumber()
{  global $dbh;
   return sqlNextSequence('SEQ_MERCHANT_TXN_NUM');
}

function nextTxnNumberCashdesk()
{  global $dbh;
   return 'csh'.$dbh->nextSequence('SEQ_CASHDESK_TXN_NUM');
}


function nextTxnNumberReservedRtn()
{  global $dbh;
   return "rrt".$dbh->nextSequence('SEQ_RESADJ_TXN_NUM');
}

function nextTxnNumberBTReserve()
{  global $dbh;
   return "btr".$dbh->nextSequence('SEQ_BT_AUTH_TXN_NUM');
}


function getTotalBets($customer_id)
{	global $dbh;

    $sql = "select pcr_total_bets from punter_credit";
	$sql .= "where pcr_pun_id = $customer_id";
	$rs = $dbh->exec($sql);
	$num_rows = $rs->getNumRows();

	if ( $num_rows == 1 )
    {	$row = $rs->next();
		$total_bets = $row[0];
		return $total_bets;
	}
	return 0;
}

function metBettingRequirements($customer_id, $amount)
{	return getTotalBets($customer_id) >= $amount;
}

function amountToMeetRequirements($customer_id, $amount)
{	return ( $amount - getTotalBets($customer_id) );

}

function numPendingWithdrawals($customer_id)
{	global $dbh;

	$sql  = "select count(*) from customer_transaction";
	$sql .= " where ctr_pun_id = $customer_id";
	$sql .= " and ctr_tty_id = ".T_TYPE_WDWL_RSVN;
	$sql .= " and ctr_status = 'pending'";
	$num  = $dbh->countQuery($sql);

	return $num;
}

function numPendingWithdrawalsToAccount($customer_id, $bank_account_id)
{	global $dbh;

	$sql  = "select count(*) from customer_transaction";
	$sql .= " where ctr_pun_id = $customer_id";
	$sql .= " and ctr_tty_id = ".T_TYPE_WDWL_RSVN;
	$sql .= " and ctr_status = 'pending'";
	$sql .= " and ctr_bac_id = $bank_account_id";
	$sql .= " and ctr_pme_code = 'BAT'";
	$num  = $dbh->countQuery($sql);

	return $num;
}


function printDepositNotificationDetails($notfn_transaction_rec, $cust_version=false)
{  global $conn;


   if ( T_TYPE_DEPOSIT_NOTFN == $notfn_transaction_rec['tty_id']  )
   {	if ( 'completed' == $notfn_transaction_rec['status'] )
			$message = "<font color=blue>COMPLETED</font>";//<br>Transferred funds received and credited";
		elseif ( 'rejected' == $notfn_transaction_rec['status'] )
			$message = "<font color=red>CANCELLED</font>";//<br>Transferred funds never received";
		elseif ( 'pending' == $notfn_transaction_rec['status'] )
			$message = "<font color=green>PENDING</font>";//<br>Transferred funds not yet received";


		if ( $notfn_transaction_rec['settle_transaction_num'] )
		{	$settle_transaction_rec = getTransaction($notfn_transaction_rec['settle_transaction_num']);
			if ( ! $cust_version )
			{	if ( defined("ADMIN_SITE") )
					$admin_user_arr = getAdminUser($settle_transaction_rec['aus_id']);
				$admin_username = $admin_user_arr['username'];
				$admin_name     = $admin_user_arr['name'];
			}
		}
?>
<P>
<table cellpadding=4 cellspacing=1 border=0 width=350>
	<tr>
		<td class="formheading" colspan=2>Deposit Notification </td>
	</tr>
<? sideTableRow('Status', $message);
   sideTableRow('Notification Time', shorterDate($notfn_transaction_rec['time'], true));
   sideTableRow('Amount Notified', getinDollars($notfn_transaction_rec['notification_amount']), $align='right');
   sideTableRow('Source Bank', $notfn_transaction_rec['deposit_src_bank']);
   sideTableRow('Advised Transfer Date', shorterDate($notfn_transaction_rec['deposit_trans_date']));
   if ( $notfn_transaction_rec['deposit_trans_ref'] )
      sideTableRow('Transfer Reference Number', $notfn_transaction_rec['deposit_trans_ref'] );

?>
</table>
<? }
}


function getTransaction($transaction_num,$customer_id="")
{  global $dbh;

   $sql  = "select ctr_id as id, ctr_time as time, ctr_tty_id as tty_id, ctr_status as status";
   $sql .= ", ctr_pme_code as pme_code, ctr_amount as amount, ctr_gam_id as gam_id, ctr_dealer_id as dealer_id";
   $sql .= ", ctr_transaction_num as transaction_num, ctr_settle_track_num as settle_track_num";
   $sql .= ", ctr_settle_transaction_num as settle_transaction_num";
   $sql .= ", ctr_aus_id as aus_id, ctr_manager_id as manager_id, ctr_notes as notes, ctr_dcc_id as dcc_id";
   $sql .= ",  ctr_bon_id as bon_id, ctr_res_id as res_id";
   $sql .= ", ctr_cashier_desk_ref_num as cashier_desk_ref_num, ctr_bac_id as bac_id, ctr_deposit_src_bank as deposit_src_bank";
   $sql .= ", ctr_deposit_trans_ref as deposit_trans_ref";
   $sql .= ", ctr_deposit_trans_date as deposit_trans_date, ctr_notification_amount as notification_amount";
   $sql .= ", ctr_reserve_amount as reserve_amount";
   $sql .= ", ctr_transaction_charge as transaction_charge, ctr_other_charges as other_charges, ctr_wre_number as wre_number";
   $sql .= ", ctr_pun_id as pun_id, pun_customer_number as customer_number, pun_first_name || ' ' ||  pun_last_name as \"customer_name\"";
   $sql .= ", tty_name, pme_name";
   $sql .= " from customer_transaction, punter, transaction_type, payment_method";
   $sql .= " where ctr_pun_id = pun_id";
   $sql .= " and ctr_tty_id = tty_id";
   $sql .= " and ctr_pme_code = pme_code";
   $sql .= " and ctr_transaction_num = '$transaction_num'";

   if ( $customer_id )
   	$sql .= " and ctr_pun_id = $customer_id";

   $rs = $dbh->exec($sql);
   $num_rows = $rs->getNumRows();

   if ( $num_rows > 0 )
   {
      	return $rs->next();
   }
   else
   	 return;
}


function topupCredits($punter_row)
{  	global $dbh;

	$pun_id = $punter_row['id'];
	$member_type = $punter_row['member_type'];

   	if ( $member_type == 'NONFINANCIAL' )
   	{	$sql  = "update punter_credit set pcr_credits = 1000";
   		$sql .= " where pcr_pun_id  = $pun_id and pcr_credits < 1000";
   		$dbh->exec($sql);
   	}
}


function printPercentage($num)
{  	echo getPercentage($num);
}

function getPercentage($num)
{	if ( $num - strtok($num,'.') > 0 )
   			return number_format($num,2).'%';
   		return	number_format($num,0).'%';
}

function printInCurrency($num, $currency)
{  if ( 'FUNTOKENS' == $currency )
   	 $symbol = '#';
   elseif ( 'USD' ==  $currency )
   	 $symbol = '$';

   if ( 0 == $num )
      echo $symbol."0";
   elseif ( $num < 0 )
      echo "<font color=red>($symbol".number_format(abs($num),  2).")</font>";
   else
      echo "$symbol".number_format($num,  2);
}


function getPaymentMethodName($pme_code)
{	global $dbh;

	$rs = $dbh->exec("select pme_name from payment_method where pme_code = '$pme_code'");

	if ( $rs->getNumRows() == 1  )
	{	$rec = $rs->next();
		return $rec['pme_name'];
	}
}


function printWithdrawalRequestDetails($settle_wdwl_transaction, $cust_version=false)
{  global $dbh;

   $settle_transaction_num = $settle_wdwl_transaction['settle_transaction_num'];
?>
   	<P>
	<table cellpadding=4 cellspacing=1 border=0 width=350>
		<tr>
			<td class=formheading colspan=2>Withdrawal Request</td>
		</tr>
<?      if ( ! $cust_version )
			sideTableRow('Customer', $settle_wdwl_transaction['customer_name']);
		sideTableRow('Status', ucwords($settle_wdwl_transaction['status']));

		if ( 'completed' == $settle_wdwl_transaction['status'] )
		{	$sql  = "select ctr_reserve_amount, ctr_transaction_charge, ctr_transaction_num";
			$sql .= " from customer_transaction";
			$sql .= " where ctr_settle_transaction_num = '$settle_transaction_num'";
			$sql .= " and ctr_tty_id = ".T_TYPE_WDWL_RSVN;
			$rs = $dbh->exec($sql);
			$num_rows = $rs->NumRows;

			if ( $num_rows > 1 )
			{	$str = "<table cellpadding=0 cellspacing=1 border=0 bgcolor=gray>";
				$str .= "<tr class=body valign=top align=center><td><b>Transaction #</b></td><td><b>Request Amount</b></td><td><b>Transfer charge</b></td><td><b>Subtotal reserved</b></td>";

				$total_reserve_amount  = 0;
				$total_transaction_charge = 0;

				for ( $i=0; $i<$num_rows; $i++ )
				{	$row = $rs->next();
					$reserve_amount  = $row['ctr_reserve_amount'];
					$transaction_charge = $row['ctr_transaction_charge'];
					$transaction_num = $row['ctr_transaction_num'];

					$total_reserve_amount  += $reserve_amount;
					$total_transaction_charge += $transaction_charge;

					if ( $highlight ==  $transaction_num )
						$str .= "<tr bgcolor=lightblue>";
					else
						$str .= "<tr class=content>";

					$str .= "<td>$transaction_num</td><td align=right>".getInDollars($reserve_amount-$transaction_charge)."</td>";
					$str .= "<td align=right>".getInDollars($transaction_charge)."</td><td align=right>".getInDollars($reserve_amount)."</td></tr>";
				}
				$str .= "<tr  class=content align=right><td><b>Totals</b></td><td align=right><b>".getInDollars($total_reserve_amount-$total_transaction_charge)."</td>";
				$str .= "<td><b>".getInDollars($total_transaction_charge)."</b></td><td><b>".getInDollars($total_reserve_amount)."</td></tr>";
				$str .= "</table>";
				sideTableRow('<font color=maroon>LINKED</font> Bank Transfer withdrawal requests<br>', $str);
            	//sideTableRow('Transfer charges deducted<br>(discounted)' , getInDollars($settle_wdwl_transaction), $align='right');

				if ( $highlight ==  $settle_rec['transaction_num'] )
					$class_arr = array('label', 'highlight');
			}
			else
			{	sideTableRow('Bank Transfer request #', $settle_wdwl_transaction['transaction_num']);

				if ( $highlight ==  $settle_wdwl_transaction['transaction_num'] )
					$class_arr = array('label', 'highlight');

				sideTableRow('Amount Requested', getInDollars($settle_wdwl_transaction['reserve_amount']-$settle_wdwl_transaction['transaction_charge']), $align='right', $class_arr);
				sideTableRow('Transfer Charge Added', "<u>".getInDollars($settle_wdwl_transaction['transaction_charge'])."</u>", $align='right', $class_arr);
				sideTableRow('Amount Reserved', getInDollars($settle_wdwl_transaction['reserve_amount']), $align='right', $class_arr);


				if ( $highlight ==  $settle_rec['transaction_num'] )
					$class_arr = array('label', 'highlight');

			}
		}
		else
		{	if ( $highlight ==  $settle_rec['transaction_num'] )
					$class_arr = array('label', 'highlight');

			sideTableRow('Bank Transfer request #', $settle_wdwl_transaction['transaction_num'], $align='left', $class_arr);
			sideTableRow('Amount Requested', getInDollars($settle_wdwl_transaction['reserve_amount']-$settle_wdwl_transaction['transaction_charge']), $align='right', $class_arr);
			sideTableRow('Transfer Charge Added', "<u>".getInDollars($settle_wdwl_transaction['transaction_charge'])."</u>", $align='right', $class_arr);
			sideTableRow('Amount Reserved', getInDollars($settle_wdwl_transaction['reserve_amount']), $align='right', $class_arr);
   		}

?>    </table>
<?
}


//Customer transaction functions

/*********************************************/
/*********** DEPOSIT FUNCTIONS ***************/
/*********************************************/

/**************************************************************
Name: doDeposit(int, double or int)
Purpose: Increases credit balance of customer specified in argument
Preconditions: Punter_credit row with the pcr_pun_id = $customer_id
               should be locked to get an accurate balance returned
Returns: Balance after credit update
**************************************************************/

function doDeposit($customer_id, $amount){
   global $dbh, $cookie;
   $cus_rec = getCustomer($customer_id, false, true);
   // to financial membership

   if ( 'NONFINANCIAL' == $cus_rec['member_type'] ){
       //set to financial, reset all currency fields
       $sql  = "update punter_credit" .
               " set pcr_credits=0, pcr_total_bets=0, pcr_total_wins=0, pcr_reserved_funds=0" .
               ", pcr_bonus_credits=0, pcr_total_deposits=0,pcr_total_withdrawals=0" .
               " where pcr_pun_id = $customer_id";
       $rs = $dbh->exec($sql);

       if ( 1 == count($rs)){ //reset successful
           $sql = "update punter set pun_member_type = 'FINANCIAL'" .
                   " where pun_id = $customer_id";
           $rs2=$dbh->exec($sql);

           if ( 1 == count($rs2)) { //to financial membership successful
                // reset member type cookie
                $cookie->setFieldValue('member_type', 'FINANCIAL');
                $cookie->set();
           }else{
               return false;
           }
       }else{
           return false;
       }
   }
   // update punter's balance with deposit amount
   $sql  = "update punter_credit" .
           " set pcr_credits=pcr_credits+'$amount'" .
           ", pcr_total_deposits=pcr_total_deposits+'$amount'" .
           " where pcr_pun_id = $customer_id";

   $rs3=$dbh->exec($sql);

   return ( 1 == count($rs3) );
}

function doBonusDeposit($customer_id, $amount){
  global $dbh, $cookie;
  settype($amount, "float");
  settype($customer_id, "integer");

  $sql  = "update punter_credit" .
         " set pcr_bonus=pcr_bonus + $amount" .
         //", pcr_total_deposits=pcr_total_deposits+'$amount'" .
         " where pcr_pun_id = $customer_id";

  $rs3 = $dbh->exec($sql);

  return ( 1 == count($rs3) );
}


/**************************************************************
Name: doWithdrawal
Purpose: Reduces customer credit balance by subtracting $amount
         from the credit column
Preconditions: Punter_credit row with the pcr_pun_id = $customer_id
               should be locked to ensure the customer balance
               does not change before updating this record
Postconditions: New record in customer_transaction table
Returns: Nothing
**************************************************************/

function doWithdrawal( $customer_id, $amount){
   global $dbh;
   
   $punter_credit = getCustomerBalance($customer_id);
   
   $success = false;
   if($punter_credit >= $amount){   
     //subtract amount from punter credit balance
     $sql  = "update punter_credit";
     $sql .= " set pcr_credits = pcr_credits - $amount";
     $sql .= ", pcr_total_withdrawals = pcr_total_withdrawals + $amount";
     $sql .= " where pcr_pun_id = $customer_id";
     $dbh->exec($sql);
     $success = $punter_credit;
   }else{
     $success = false;
   }

   return $success;
}



/**************************************************************
Name: doReserveFunds
Purpose:  increments pcr_reserved_funds column

Preconditions: Punter_credit row with the pcr_pun_id = $customer_id
               should be locked to ensure the customer balance
               does not change before inserting this record
Postconditions: New record in customer_transaction table
Returns: Nothing
**************************************************************/

function doReservefunds( $customer_id, $amount){
   global $dbh;
   //add amount to punter reserved funds balance
   $sql  = "update punter_credit";
   $sql .= " set pcr_reserved_funds = pcr_reserved_funds + '$amount'";
   $sql .= " where pcr_pun_id = $customer_id";
   $dbh->exec($sql);
}


function setWdwlRequestStatus($wre_number, $status){
   global $dbh;
   $sql  = "update withdrawal_request";
   $sql .= "   set wre_status = '$status'";
   $sql .= " where wre_internal_number = '$wre_number'";
   $dbh->exec($sql);
   return $status;
}


function doBankTransferDeposit ($amount, $aus_id, $settle_tracking_number, $notfn_transaction_rec){
	global $dbh;
   	if ( T_TYPE_DEPOSIT_NOTFN == $notfn_transaction_rec['tty_id'] && 'pending' == $notfn_transaction_rec['status'] ){
   		$customer_id 				= $notfn_transaction_rec['pun_id'];
   		$notification_amount		= $notfn_transaction_rec['notification_amount'];
   		$transfer_source_bank_name	= $notfn_transaction_rec['deposit_src_bank'];
   		$transfer_date 				= $notfn_transaction_rec['deposit_trans_date'];
   		$transfer_reference_numu	= $notfn_transaction_rec['deposit_trans_ref'];

   		doDeposit($customer_id, $amount);


	    $transaction_num = nextTxnNumberBTSettle();

	   	$sql  = "INSERT INTO customer_transaction";
	   	$sql .= " ( ctr_id, ctr_pun_id, ctr_amount, ctr_pme_code, ctr_tty_id, ctr_status";
	   	$sql .= ",  ctr_aus_id, ctr_time, ctr_transaction_num, ctr_settle_track_num, ctr_deposit_src_bank";
	   	$sql .= ",  ctr_deposit_trans_date, ctr_deposit_trans_ref, ctr_notification_amount)";
	   	$sql .= "   VALUES (nextval('seq_ctr_id'), $customer_id, $amount";
	   	$sql .= ",  'BAT', ".T_TYPE_DEPOSIT.", 'completed', $aus_id, CURRENT_TIMESTAMP, '$transaction_num', '$settle_tracking_number'";
	   	$sql .= ", '$transfer_source_bank_name', '$transfer_date', '$transfer_reference_num'";
	   	$sql .= ", $notification_amount)";

	  	$dbh->exec($sql);

	  	updateDepositNotification($notfn_transaction_rec['transaction_num'], 'completed', $transaction_num);

	  	return true;
   	}
   	return false;
}



function doBankTransferWdwlReserveFunds ($reserve_amount, $transaction_charge, $customer_id,  $bac_id, $wdwl_request_num="", $notes, $aus_id="NULL", $mngr_id="NULL") {
   global $dbh;

   doReserveFunds($customer_id, $reserve_amount);

   $transaction_number = nextTxnNumberBTReserve();

   $sql  = "INSERT INTO customer_transaction";
   $sql .= " ( ctr_id, ctr_pun_id, ctr_reserve_amount, ctr_transaction_charge";
   $sql .= ",  ctr_tty_id, ctr_pme_code, ctr_status,  ctr_aus_id, ctr_mngr_id";
   $sql .= ",  ctr_time, ctr_transaction_num, ctr_bac_id, ctr_wre_number, ctr_notes)";
   $sql .= "   VALUES (nextval('seq_ctr_id'), $customer_id, $reserve_amount, $transaction_charge";
   $sql .= ", ".T_TYPE_WDWL_RSVN.",  'BAT', 'pending'";
   $sql .= ", $aus_id, $mngr_id, CURRENT_TIMESTAMP, '$transaction_number'";
   $sql .= ", $bac_id, '$wdwl_request_num', '$notes')";

   $dbh->exec($sql);

   return $transaction_number;
}


function doBankTransferWithdrawal ($amount,  $transaction_charge, $customer_id,  $bac_id, $settle_track_num, $aus_id="NULL", $mngr_id="NULL") {
   global $dbh;

   doWithdrawalSettlement($customer_id, $amount);

   $transaction_number = nextTxnNumberBTSettle();

   $sql  = "INSERT INTO customer_transaction";
   $sql .= " ( ctr_id, ctr_pun_id, ctr_amount,  ctr_transaction_charge";
   $sql .= ",  ctr_tty_id, ctr_pme_code, ctr_status,  ctr_aus_id, ctr_mngr_id";
   $sql .= ",  ctr_time, ctr_transaction_num, ctr_bac_id, ctr_settle_track_num)";
   $sql .= "   VALUES (nextval('seq_ctr_id'), $customer_id, -$amount,  $transaction_charge";
   $sql .= ", ".T_TYPE_WITHDRAWAL.",  'BAT', 'completed'";
   $sql .= ", $aus_id, $mngr_id, CURRENT_TIMESTAMP, '$transaction_number'";
   $sql .= ", $bac_id, '$settle_track_num')";

   $dbh->exec($sql);

   return $transaction_number;
}


function createWithdrawalRequest($pun_id, $amount_cc, $amount_bt, $bac_id="", $admin_id="") {
   global $dbh;

   if ( ! $bac_id )
      $bac_id = 'NULL';

   if ( ! $admin_id )
      $admin_id = 'NULL';

   $request_number = $dbh->nextSequence('seq_wre_internal_number'); //reference number
   $request_amount = $amount_cc + $amount_bt;

   //define sql statement
   $sql  = "insert into withdrawal_request";
   $sql .= " ( wre_id, wre_pun_id, wre_time, wre_aus_id";
   $sql .= ", wre_amount, wre_amount_credit_card, wre_amount_bank_transfer";
   $sql .= ", wre_internal_number, wre_bac_id, wre_status)";
   $sql .= " VALUES ( nextval('seq_wre_id'), $pun_id, CURRENT_TIMESTAMP, $admin_id";
   $sql .= ", $request_amount, $amount_cc, $amount_bt";
   $sql .= ", '$request_number', $bac_id, 'check')"; //start in check status

   $dbh->exec($sql);

   return $request_number;
}


function updateDepositNotification($notfn_transaction_num, $status, $settlement_transaction_num, $notes="") {
   global $dbh;

   $sql  = "update customer_transaction";
   $sql .= " set ctr_status = '$status'";
   $sql .= ", ctr_settle_transaction_num = '$settlement_transaction_num'";
   $sql .= ", ctr_notes = '$notes'";
   $sql .= " where ctr_transaction_num = '$notfn_transaction_num'";
   $sql .= " and ctr_tty_id = ".T_TYPE_DEPOSIT_NOTFN;
   $sql .= " and ctr_status = 'pending'";

   $dbh->exec($sql);
}


function doRejectDepositNotification ($notfn_transaction_rec, $aus_id) {
  global $dbh;

  if ( 'pending' == $notfn_transaction_rec['status'] && T_TYPE_DEPOSIT_NOTFN == $notfn_transaction_rec['tty_id']  ) {
    $customer_id = $notfn_transaction_rec['pun_id'];
    $notification_amount = $notfn_transaction_rec['notification_amount'];
    $transfer_reference_num = $notfn_transaction_rec['deposit_trans_date'];
    $transaction_number = nextTxnNumberDepositNotRej();

    $sql  = "INSERT INTO customer_transaction";
    $sql .= " ( ctr_id, ctr_pun_id, ctr_amount,  ctr_tty_id, ctr_pme_code, ctr_status";
    $sql .= ",  ctr_aus_id, ctr_time, ctr_transaction_num, ctr_notification_amount)";
    $sql .= "   VALUES (nextval('seq_ctr_id'), $customer_id, 0, ".T_TYPE_DEPOSIT_NOTFN_REJ;
    $sql .= ",  'BAT', 'completed', $aus_id, CURRENT_TIMESTAMP";
    $sql .= ", '$settle_tracking_number', $notification_amount)";

    $dbh->exec($sql);

    updateDepositNotification($notfn_transaction_rec['transaction_num'], 'rejected', $settle_tracking_number);
  }
}

function getReservedFunds($pun_id) {
   $punter_details = getPunter($pun_id);
   return $punter_details['reserved_funds'];
}

function doCancelWithdrawalReservation ($transaction_rec, $cancel_reason="", $aus_id="NULL", $mngr_id="NULL") {
   global $dbh;

   $customer_id = $transaction_rec['pun_id'];
   $bank_account_id = $transaction_rec['bac_id'];
   $reserve_amount = $transaction_rec['reserve_amount'];
   $transaction_charge = $transaction_rec['transaction_charge'];

   $wre_rec = getWithdrawalRequest($transaction_rec['wre_number']);
   $wre_number = $wre_rec['wre_number'];
   $customer_reserved_funds = getReservedFunds($customer_id);

   if ( numPendingWithdrawalsToAccount($customer_id, $bank_account_id) > 1 )
   {	$sql  = "SELECT ctr_id, ctr_time, ctr_tty_id, ctr_reserve_amount";
		$sql .= ", ctr_pun_id, pun_customer_number, pun_first_name";
		$sql .= ", pun_last_name, ctr_wre_number, ctr_transaction_num";
		$sql .= ", ctr_bac_id, ctr_transaction_charge";
		$sql .= " FROM customer_transaction, punter";
		$sql .= " WHERE ( ctr_tty_id = '".T_TYPE_WDWL_RSVN."' )";
		$sql .= " AND ctr_status = 'pending'";
		$sql .= " AND ctr_pme_code = 'BAT'";
		$sql .= " and ctr_pun_id = pun_id";
		$sql .= " and ctr_bac_id = $bank_account_id";
		$sql .= " and pun_id = $customer_id";
		$sql .= " order by ctr_time asc";

		$rs2 = $dbh->exec($sql);
		$num_rows2 = $rs2->getNumRows();

		while ( $rs2->hasNext() )
		{	$row2 = $rs2->next();
			$ctr_id           		= $row2['ctr_id'];
			$time             		= $row2['ctr_time'];
			$tty_id            		= $row2['ctr_tty_id'];
			$reserve_amount        	= $row2['ctr_reserve_amount'];
			$customer_id           	= $row2['ctr_pun_id'];
			$customer_number  		= $row2['pun_customer_number'];
			$punter_fname      		= $row2['punter_first_name'];
			$punter_lname      		= $row2['punter_last_name'];
			$wre_number      		= $row2['ctr_wre_number'];
			$transaction_num 		= $row2['ctr_transaction_num'];
			$bank_account_id		= $row2['ctr_bac_id'];
			$transaction_charge	    = $row2['ctr_transaction_charge'];

			$reserve_transactions[$transaction_num] = array('wre_number'=>$wre_number,'reserve_amount'=>$reserve_amount);
			$sum_reserve_amount += $reserve_amount;
		}
		$notes = $all_notes;

		$linked = true;
   }
   else
   {
   	  $reserve_transactions[$transaction_rec['transaction_num']] = array('wre_number'=>$wre_number,'reserve_amount'=>$reserve_amount);
      $sum_reserve_amount = $reserve_amount;
   }

   if ( $customer_reserved_funds < $sum_reserve_amount )  //will only occur if $reserve-amount is negative
   {	dieWithError("CRITICAL ERROR: 'cancelWithdrawalAuthorization (amount=$reserve_amount, customer_id=$customer_id) ..': insufficient reserved funds! ($reserved_funds)" );
   }

   $dbh->begin();
	   doReserveFunds($customer_id, -$sum_reserve_amount); //decrement reserved funds by reserve amount


	   $rec_transaction_number = nextTxnNumberReservedRtn();

	   //insert settlement recrd
	   $sql  = "INSERT INTO customer_transaction";
	   $sql .= " ( ctr_id, ctr_pun_id, ctr_reserve_amount, ctr_transaction_charge";
	   $sql .= ",  ctr_tty_id, ctr_pme_code, ctr_status";
	   $sql .= ",  ctr_aus_id, ctr_mngr_id, ctr_time, ctr_transaction_num, ctr_notes, ctr_settle_transaction_num, ctr_bac_id)";
	   $sql .= "   VALUES (nextval('seq_ctr_id'), $customer_id, -$sum_reserve_amount, '$transaction_charge'";
	   $sql .= ", ".T_TYPE_WDWL_RSVN_RETN.",  'BAT', 'completed'";
	   $sql .= ", $aus_id, $mngr_id, CURRENT_TIMESTAMP, '$rec_transaction_number', 'Cancellation reason: $cancel_reason'";
	   $sql .= ", '".$transaction_rec['transaction_num']."',".$transaction_rec['bac_id'].")";

	   $dbh->exec($sql);

	   foreach ( $reserve_transactions as $reserve_transaction_num => $details )
	   {	//set withdrawal reservsation transaction status
			updateWithdrawalReservation($reserve_transaction_num, 'cancelled', $rec_transaction_number);

			if ( $wre_rec['allocated_credit_card'] > 0 )
				setWdwlRequestStatus($details['wre_number'], 'closed');
			else
				setWdwlRequestStatus($details['wre_number'], 'cancelled');
	   }
   $dbh->commit();
}


function logDepositBankTransferNotification ($amount, $customer_id, $transfer_source_bank_name, $transfer_date, $transfer_reference_number, $notes, $aus_id="NULL", $mngr_id="NULL") {
   global $dbh;

   $transaction_number = nextTxnNumberDepositNot();

   $sql  = "INSERT INTO customer_transaction";
   $sql .= " ( ctr_id, ctr_pun_id, ctr_notification_amount";
   $sql .= ",  ctr_tty_id, ctr_pme_code, ctr_status,  ctr_aus_id, ctr_mngr_id";
   $sql .= ",  ctr_time, ctr_transaction_num, ctr_deposit_src_bank";
   $sql .= ",  ctr_deposit_trans_date, ctr_deposit_trans_ref, ctr_notes)";
   $sql .= "   VALUES (nextval('seq_ctr_id'), $customer_id, $amount";
   $sql .= ", ".T_TYPE_DEPOSIT_NOTFN.", 'BAT', 'pending'";
   $sql .= ", $aus_id, $mngr_id, CURRENT_TIMESTAMP, '$transaction_number'";
   $sql .= ", '$transfer_source_bank_name', '$transfer_date', '$transfer_reference_number', '$notes')";

   $dbh->exec($sql);
   return $transaction_number;
}

function getAllCreditCardDetails ($customer_id)
{  global $dbh;

   $sql = "select crd_reg_amount, crd_registered, crd_card_type";
   $sql .= ", crd_last_five_digits, crd_holder_name, crd_max_total_deposit";
   $sql .= ", crd_max_per_deposit, crd_reg_date from credit_card";
   $sql .= " where crd_pun_id = $customer_id";
   $rs = $dbh->exec($sql);
   $num_rows = $rs->getNumRows();


   if ( $num_rows > 0 )
   {  for ( $i=0; $i<$num_rows; $i++ )
      {  $row = $rs->next();

         $arr[$i]['reg_amount']         = $row['crd_reg_amount'];
         $arr[$i]['registered']         = $row['crd_registered'];
         $arr[$i]['last_five_digits']   = $row['crd_last_five_digits'];
         $arr[$i]['holder_name']        = $row['crd_holder_name'];
         $arr[$i]['max_total_deposits'] = $row['crd_max_total_deposits'];
         $arr[$i]['max_per_deposit']    = $row['crd_max_per_deposit'];
         $arr[$i]['reg_date']           = $row['crd_reg_date'];

         $arr[$i]['card_type']          = ($row['crd_card_type'] == 'VISA' || $row['crd_card_type'] == 'VI') ? 'Visa' : 'Mastercard';
      }

      return $arr;
   }
   return '';
}


function doWithdrawalSettlement($customer_id, $amount)
{  global $dbh;

   $amount = number_format($amount,2);
   $sql  = "SELECT pcr_credits, pcr_reserved_funds from punter_credit";
   $sql .= " WHERE pcr_pun_id = $customer_id";
   $rs = $dbh->exec($sql);
   $row =  $rs->next();
   $credits 	   = $row['pcr_credits'];
   $reserved_funds = number_format($row['pcr_reserved_funds'],2);

   if ( $reserved_funds < $amount )
   {  $err_message  = "<br><br>Insufficient reserved funds:<br>The current reserved funds in this customer's account is less than the amount required by this reservation transaction (".getInDollars($amount)." required, ".getInDollars($reserved_funds)." reserved)";
      $err_message .= " for a withdrawal settlement of ".getInDollars($amount);

   }


   if ( $credits < $amount )
   {  $err_message  = "<br><br>Insufficient funds funds (".getInDollars($credits).")";
      $err_message .= " for a withdrawal settlement of ".getInDollars($amount);
   }

   if ( $err_message )
   {   printErrorMessage($err_message);
      ?><P><?
      dieWithError("This is a critical error, please contact technical support");
   }
   else
   {  //perform settlement
      $sql  = "update punter_credit";
      $sql .= " set pcr_reserved_funds=pcr_reserved_funds-'$amount'";
      $sql .= ", pcr_credits = pcr_credits-'$amount'";
      $sql .= ", pcr_total_withdrawals = pcr_total_withdrawals + '$amount'";
      $sql .= " where pcr_pun_id = $customer_id";
      $dbh->exec($sql);
   }
}


function firepayMaintenanceScheduled ()
{  $cfg_filename = CONFIG_DIR."/firepay_maintenance.cfg";
   $cfg = getCfgFile(CONFIG_DIR."/firepay_maintenance.cfg");

   if ( ! $cfg )
      return false;

  $start_time = $cfg['start_time'];
   $hr_duration = $cfg['hr_duration'];
   $min_duration = $cfg['min_duration'];
   $auto_clear = $cfg['auto_clear'];

   $date_arr = splitDate($start_time);
   $day    = $date_arr['day'];
   $month  = $date_arr['month'];
   $year   = $date_arr['year'];
   $hour   = $date_arr['hour'];
   $minute = $date_arr['minute'];

   $start_mktime = mktime($hour, $minute, 0, $month, $day, $year);
   $end_mktime = mktime($hour+$hr_duration, $minute+$min_duration, 0, $month, $day, $year);
   $time = mktime();

   if ( $time >= $start_mktime && $time <= $end_mktime ) //if within scheduled period
   	  return true;

   if ( $time > $end_mktime ) //if past scheduled period
   {
      if ( 'on' == $auto_clear )   //if restriction can be cleared automatically
      {  $f = fopen ($cfg_filename, 'w'); //clear file
         fclose($f);
         return false;  //restriction cleared
      }
      return true; //can't be cleared, restriction remains
   }
   return $false;  //before scheduled period
}


function cashierDeskRefNumExists($reference_number) {
   global $dbh;

   $sql  = "select count(*) from customer_transaction";
   $sql .= " where ctr_cashier_desk_ref_num = '$reference_number'";
   $ret = $dbh->countQuery($sql);
   if ( $ret > 0 )
      return true;
   return false;
}


function getTransactionById($id) {
   global $dbh;

   $sql  = "select ctr_id as id, ctr_time as time, ctr_tty_id as tty_id, ctr_status as status";
   $sql .= ", ctr_pme_code as pme_code, ctr_amount as amount, ctr_gam_id as gam_id, ctr_dealer_id as dealer_id";
   $sql .= ", ctr_transaction_num as transaction_num, ctr_settle_track_num as settle_track_num";
   $sql .= ", ctr_settle_transaction_num as settle_transaction_num";
   $sql .= ", ctr_aus_id as aus_id, ctr_manager_id as manager_id, ctr_notes as notes, ctr_dcc_id as dcc_id";
   $sql .= ",  ctr_bon_id as bon_id, ctr_res_id as res_id";
   $sql .= ", ctr_cashier_desk_ref_num as cashier_desk_ref_num, ctr_bac_id as bac_id, ctr_deposit_src_bank as deposit_src_bank";
   $sql .= ", ctr_deposit_trans_ref as deposit_trans_ref";
   $sql .= ", ctr_deposit_trans_date as deposit_trans_date, ctr_notification_amount as notification_amount";
   $sql .= ", ctr_reserve_amount as reserve_amount";
   $sql .= ", ctr_transaction_charge as transaction_charge, ctr_other_charges as other_charges, ctr_wre_number as wre_number";
   $sql .= ", ctr_pun_id as pun_id, pun_customer_number as customer_number, pun_first_name || ' ' ||  pun_last_name as \"customer_name\"";
   $sql .= ", tty_name, pme_name";
   $sql .= " from customer_transaction, punter, transaction_type, payment_method";
   $sql .= " where ctr_pun_id = pun_id";
   $sql .= " and ctr_tty_id = tty_id";
   $sql .= " and ctr_pme_code = pme_code";
   $sql .= " and ctr_id = " . $id;

   if ( $customer_id )
   	$sql .= " and ctr_pun_id = $customer_id";

   $rs = $dbh->exec($sql);
   $num_rows = $rs->getNumRows();

   if ( $num_rows > 0 ) {
      	return $rs->next();
   }
   else {
        return ' ';
   }
}


/**
 * @return string
 * @param double amount
 * @param integer cents
 * @desc Returns display formatted currency
*/
function translateProcessorStatus($code) {
        $str="";
  switch($code){
        case "I": $str="<b style='color:orange'> Inizialized</b>"; break;
        case "P": $str="<b style='color:blue'> Pending</b>"; break;
        case "A": $str="<b style='color:green'> Accepted</b>"; break;
        case "D": $str="<b style='color:red'> Declined</b>"; break;
  }
  return $str;
}

function printLastAdminTransactions($jur_id = null, $target_jur_id = null, $limit = 15){
	global $dbh;
	if(empty($jur_id)){
	  $jur_id = $_SESSION["jurisdiction_id"];
	}
	
	$sql  = "SELECT a.jur_name AS jur, a.jur_class AS jur_class, b.jur_class AS other_jur_class, b.jur_name AS other_jur_name, TO_CHAR(cre_time, 'DD-MM-YYYY HH24:MI') time, cre_amount, cre_direction, cre_id FROM credit_transfer JOIN jurisdiction a ON a.jur_id = cre_entity_id JOIN jurisdiction b ON b.jur_id = cre_other_entity_id WHERE cre_other_entity_id != 2 AND cre_entity_id = $jur_id";
	
	if(!empty($target_jur_id)){
	  if($target_jur_id == "u"){
  	  $sql .= " AND b.jur_id = a.jur_parent_id"; 
	  }else{
	    $sql .= " AND cre_other_entity_id = $target_jur_id";
	  }
	}
	
	$sql .= " ORDER BY cre_time DESC";
	
	$dbh->Connection->setLimit($limit);
	//$dbh->setLimit($limit);
	$rs  = $dbh->exec($sql);
	
	$rs->Records = array_reverse($rs->Records);
	
	$num_rows = $rs->getNumRows();
	?>
	  <div class="bodyHD">Last <?=$num_rows?> money transfer</div>
	<table>
	  <thead>
	    <tr>
	      <th>Date</th>
	      <th colspan="2">Your jurisdiction</th>
	      <th>&nbsp;</th>
	      <th colspan="2">Other jurisdiction</th>
	      <th>Amount</th>
	    </tr>
	  </thead>
	  <tbody>
	<?php
	$sum = 0;
	while($rs->hasNext()){
		$row  = $rs->next();
		$sum += $row["cre_amount"];
		?>
	    <tr>
	      <td><i><?=$row["time"]?></i></td>
	      <td><b><?=$row["jur_class"]?></b></td>
	      <td><?=$row["jur"]?></td>
	      <td><img src="<?=image_dir . "/" . (($row["cre_direction"] == "IN")?("in"):("out"))?>.gif"/></td>
	      <td><b><?=$row["other_jur_class"]?></b></td>
	      <td><?=$row["other_jur_name"]?></td>
	      <td><?=getInDollars($row["cre_amount"])?></td>
	    </tr>
		<?php
	}
	?>
	    <tr>
	      <td colspan="6"></td>
	      <td style="border-top:1px solid #999"><?=getInDollars($sum)?></td>
	    </tr>
	  </tbody>
	</table>
	<?php
}

}
?>
