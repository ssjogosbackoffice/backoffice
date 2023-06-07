<?
check_access('transactions_user_transfer',true);
require 'account_functions.php';
global $lang;
$witdrawAccess=check_access('transactions_withdrawals');
$depositAccess=check_access('transactions_deposits');
if(!$witdrawAccess && !$depositAccess){
    dieWithError($lang->getLang("You're not allowed to see this page"));
}
?>

<script language="javascript" type="text/javascript">
    function toggle_wmc_number_input(select_obj) {
        if ( 'wmc' == select_obj.options[select_obj.selectedIndex].value ) {
            document.getElementById('wmc_number_div').innerHTML = '<span style="font-size:8pt">card number</span> <input type="text" name="wmc_card_number" size="20">';
        }
        else {
            document.getElementById('wmc_number_div').innerHTML = '';
        }
    }
</script>

<div class="bodyHD"><?=$lang->getLang("Cashier Desk")?></div>
<?php
include_once 'JurisdictionsList.class.inc';
$thisJurClass = JurisdictionsList::getInstance ( $dbh );
globalizeStr('phase');
globalizeInt('customer_id');
globalizeInt('reference_number');
globalizeFloat('amount');
//globalise('password');
$password="";
globalize('payment_method');
globalize('scode');
globalize('customer_str');
globalize('wmc_card_number');

function checkFields($club_id, $amount, $payment_method, $password,$type) {
    global $dbh,$lang,$customer_id;
    if ( isBlank($amount) || $amount <= 0)
        addError("", $lang->getLang("An amount must be entered"));
    elseif ( ! isValidMoney($amount) )
        addError("", "Invalid amount entered - must be a ".((isset($_SESSION['currency_html']) && $_SESSION['currency_html']!='') ? $_SESSION['currency_html'] : "&euro;" )." amount (cents optional)");

    if ( isBlank($payment_method) )
        addError("", "A payment method must be selected");
    elseif ( !('cashier cheque' == $payment_method || 'cashier cash' == $payment_method || 'cashier credit card' == $payment_method || 'cashier bonus' == $payment_method || 'wmc' == $payment_method) )
    {
        addError("", $lang->getLang("Invalid payment method"));
    }

    if($type=='deposit'){
        $sql =  "SELECT jur_available_credit" .
            "   FROM  jurisdiction " .
            "   WHERE jur_id = " . $club_id;
        //echo $sql;
        $rs       = $dbh->exec($sql,false,true);
        $num_rows = $rs->getNumRows();

        if ( $num_rows > 0 ) {
            $row = $rs->next();
            $available_credit = $row['jur_available_credit'];

            if ( $available_credit - $amount < 0 && $_SESSION['jurisdiction_class']!='casino' ){
                $max_deposit = getInDollars($available_credit);
                addError("", $lang->getLang("Cannot create customer - cannot process deposit. Your club does not have enough available credit. Maximum deposit allowed is %",$max_deposit));
                return false;
            }
        }
        else {
            addError("", $lang->getLang("Critical error: Missing account!"));
        }
        $sql="Select pun_daily_deposits_limit,cur_id, cur_code from punter left join jurisdiction on pun_betting_club=jur_id left join currency on cur_id=jur_currency where pun_id = ".$dbh->escape($customer_id);
        $currLimits=$dbh->queryRow($sql);
        $max_deposit=$currLimits["pun_daily_deposits_limit"];
        if($currLimits["cur_id"]!=1){
            $currMult=$dbh->queryOne("Select ccn_convers_value from currencies_conversion where ccn_cur_secondary=".$dbh->prepareString(strtoupper($currLimits["cur_code"])));
           $max_deposit *=$currMult;
        }
        if (  $amount > $max_deposit) {
            addError("", $lang->getLang("Maximum deposit allowed is %", $max_deposit));
        }
    }

    return ! areErrors();
}


function getUserClubJurisdictionTree($userClub){
    global $thisJurClass;
    $strToRet='<table>';
    $thistree=$thisJurClass->getJurisdictionHierarchy($userClub,true);
    foreach($thistree as $k=>$v){
        $strToRet.="<tr><td><b>".$k.":</b><td> ".$v."</td><tr>";
        if($k==$_SESSION['jurisdiction_class']){
            $strToRet.="</table>";
            return $strToRet;
        }
    }

}

if ( 'process_transaction' == $phase ) {

    if ( $scode && $_SESSION['scode'] == $scode  ) {
        addError("", $lang->getLang("Cannot resubmit/refresh deposit transaction"));
        $phase = 'enter_details';
        }
     else {
        if ( $customer_id ) {
            $customer_id = (int)$customer_id;
            $customer_row = getCustomer($customer_id,false,false,true);
        if ( $customer_row ) {
            $phase = 'select_payment_method';
        }
    else {
        dieWithError($lang->getLang("Invalid customer ID"));
        unset($phase);
    }
}
else {
    dieWithError("Missing customer ID");
}




if ( $customer_row['betting_club'] && !in_array($customer_row['betting_club'], $thisJurClass->getChilJurisdictionsIdList($_SESSION['jurisdiction_id']))) {

    $club_name = getClubName($customer_row['betting_club']);

    addError("",$lang->getLang("Transactions cannot be performed for this customer at this club because the customer's account is held at another club (%)",$club_name));
    $phase = 'enter_details';
}
else {
if ( checkFields($_SESSION['jurisdiction_id'], $amount, $payment_method, $password,$_POST['operation'] ) ) {
//$dbh->lockRow('punter_credit', array('pcr_pun_id'=>$customer_id));  //lock punter_credit row
$dbh->begin();
// check that the customer has enough funds in account
if($_POST['operation']=='withdraw' &&  $customer_row['credits'] < $amount ){//available_balance
    $balance = getInDollars($customer_row['available_balance'],2,$customer_row['code_for_web']);
    addError("", $lang->getLang("Insufficient funds % for withdrawal", $balance) );
    $phase = 'enter_details';
}
else{
$jurisdictionsArray=array();
$alljurisdictions=getAllEntitiesBetween($_SESSION['jurisdiction_class'],'club',false,$customer_id);
    $trnsId= 'ADM'.$dbh->nextSequence('CTR_TRAN_NUM_SEQ').rand_str(4);
if ( 'deposit' == $_POST['operation'] ){
    $transfer_type=19;
}
else{
    $transfer_type=20;
    $alljurisdictions=array_reverse($alljurisdictions);
}
    $mult=0;
$keys = array_keys($alljurisdictions);
   if($_POST['operation']=='withdraw'){
        if(!is_int(adminCreditLog($_POST['note'],$transfer_type,$_SESSION['admin_id'],$customer_row['betting_club'],false,$customer_id, -$_POST['amount'],1,$trnsId))){
            addError('',$lang->getLang('An error has occurred when adding the transaction record'));
        }
    }

for ($i = 0; $i < count($keys); $i++) {

    $cur = $alljurisdictions[$keys[$i]];
    $next = $alljurisdictions[$keys[$i+1]];

    if($next!=''){
        $mult++;
        if ( 'deposit' == $_POST['operation'] )
        {
            if(!is_int(adminCreditLog($_POST['note'],$transfer_type,$_SESSION['admin_id'],$cur,$next,false,$_POST['amount'],$i,$trnsId))){
                addError('',$lang->getLang('An error has occurred when adding the transaction record'));
            };
        }
        else
        {
            if(!is_int(adminCreditLog($_POST['note'],$transfer_type,$_SESSION['admin_id'],$next,$cur,false,-$_POST['amount'],$i,$trnsId))){
                addError('',$lang->getLang('An error has occurred when adding the transaction record'));
            };
        }
    }
}
    if($_POST['operation']=='deposit'){
        if(!is_int(adminCreditLog($_POST['note'],$transfer_type,$_SESSION['admin_id'],$customer_row['betting_club'],false,$customer_id, $_POST['amount'],$mult,$trnsId))){
            addError('',$lang->getLang('An error has occurred when adding the transaction record'));
        }
}
if(areErrors()){
    $dbh->rollback();
    $phase = 'enter_details';
}
else
{
    if ( 'cashier cash' == $_POST['payment_method'] ){
        if($_POST['operation']=='withdraw' ){
                if(DOT_IT && $customer_row["identified"] != 1){
                    $dbh->rollback();
                    $skin = getSkinDetailsByJurisdiction($_SESSION["jurisdiction_id"], $_SESSION["jurisdiction_class"]);
                    addError("", "Impossible to withdrawal required sum, user doesn't send the signed contract.<br/>" . getLink($skin["skn_url"] . "/accounts/Contract?uid=" . $customer_row["customer_number"], "Download contract"));
                    $phase = 'enter_details';
                }
                else{
                $transaction_number = do_cash_withdrawal ($amount, $customer_id, $_SESSION['admin_id'], $_SESSION['jurisdiction_id'],$trnsId);
                }
            }
        else{
            require_once("queue_func.php");
            if(queue_begin("TDPL-$customer_id", 120)){
                $transaction_number = do_cash_deposit ($amount, $customer_id, $_SESSION['admin_id'], $_SESSION['jurisdiction_id'],$trnsId);
            }else{
                dieWithError($lang->getLang("Another transaction for this customer is running, wait a few for a new transaction, thanks."));
            }

        }
            if ( $transaction_number ){
                $success = true;
            }else{
                addError("", "Database Error");
                $phase = 'enter_details';
            }

    }
    elseif ( 'wmc' == $payment_method ){
        require 'wmc_classes.php';
        $wmc_w = new wmc_withdrawal ($customer_id, $wmc_card_number, $amount, $_SESSION['admin_id']);

        if ( $wmc_w->validate_fields() ){
            $success = $wmc_w->exec();
            if ( $success ){
                $transaction_number = $wmc_w->response_fields['rtn'];
            }else{
                $error_code = $wmc_w->response_fields['error_code'];
                if ( !empty($wmc_srv_errors[$error_code]) ){
                    $error_msg = $wmc_srv_errors[$error_code]['msg'];
                }else{   // if we don't have the error map, use the error returned by wmc gateway

                    $error_msg = $wmc_w->response_fields['error_code'] . ': ' . $wmc_w->response_fields['error_details'];
                }

                addError("", $error_msg);
                $phase = 'enter_details';
            }
        }
        else {
            // display local validation errors
            foreach ( $wmc_w->errors as $key => $error_code ) {
                addError($key, $wmc_local_errors[$error_code]);
            }
            $phase = 'enter_details';
        }
    }
    else {
        addError($key, $lang->getLang("Invalid Payment Method"));
    }
}
}

if ( true == $success ) {
registerSubmitVar('scode');
$customer_row = getCustomer($customer_id,false,false,true);; // refresh record
$wdwl_amount_disp = getInDollars($amount,2,$customer_row['code_for_web']);
?>
    <P>
        <?php   if($_POST['operation']=="withdraw"){
            printInfoMessage($lang->getLang("Withdrawal successful. Please pay this customer"). " ".$wdwl_amount_disp);
        }
        else{
            printInfoMessage($lang->getLang("Deposit successful"));
        }
        registerSubmitVar('scode');

        if ( -1 == $customer_row['identified'] ) {
            $icon = "<img src=\"".image_dir."/icon_transient.gif\" alt=\"Transient customer\">";
        }
        ?>
    <p>
    <table cellpadding=4 cellspacing=1 border=0>
        <tr>
            <?php
            $amount_in_dollars = getInDollars($amount,2,$customer_row['code_for_web']);

            if($_POST['operation']=="withdraw"){
                ?>
                <td class=formheading colspan=2><?=ucwords($payment_method);?> <?=$lang->getLang("Withdrawal successful. Please pay this customer")." ". $amount_in_dollars?></td>
            <?php
            }
            else{
                ?>
                <td class=formheading colspan=2><?=ucwords($payment_method);?> <?=$lang->getLang("Deposit successful. Amount deposited ").": ". $amount_in_dollars?></td>
            <?php
            }
            ?>
        </tr>

        <? sideTableRow($lang->getLang('Amount'), getInDollars($amount), 'right'); ?>
        <? sideTableRow($lang->getLang('Customer'), $customer_row['customer_number'] .': ' . ( isBlank($customer_row['full_name']) ? $customer_row['username'] : $customer_row['full_name']). '&nbsp;'.$icon, 'right'); ?>
  <!--      <?/* sideTableRow($lang->getLang('New Customer Balance'), getInDollars($customer_row['credits']+($_POST['operation']=="withdraw"?$_POST['amount']*-1 :$_POST['amount'])), 'right'); */?>
  -->      <? sideTableRow($lang->getLang('New Customer Balance'), getInDollars($customer_row['credits']), 'right'); ?>
        <? sideTableRow($lang->getLang('Transaction Reference Number'), $transaction_number, 'right'); ?>
    </table>
    </p>
<p>
    <?printlink('user_credit_transfer', $lang->getLang("Click Here"));?> <?=$lang->getLang("to return to the main page.")?>
    <?php
    }
    }
    else {
        $phase = 'enter_details';
    }
    }
    }
    }


    if ( 'enter_details' == $phase || ! $phase && $customer_id ) {
    if ( $customer_id ) {
    $customer_id = (int)$customer_id;
    $customer_row = getCustomer($customer_id,false,false,true);;

    if ( $customer_row ) {
    ?>
    <div class="bodyHD2"><strong><?=$lang->getLang("Step 2:")?></strong><?=$lang->getLang("Enter operation details")?></div>
    <P>
        <?showErrors()?>
    <p>
        <?form_head()?>
        <input type="hidden" name="phase" value="process_transaction">
        <input type="hidden" name="customer_id" value="<?=$customer_id?>">
        <input type="hidden" name="scode" value="<?=mktime().session_id()?>">
    <table cellpadding=4 cellspacing=1 border=0>
        <tr>
            <td class=label><?=$lang->getLang("Customer Number")?> </td>
            <td class=content>
                <?=$customer_row['customer_number'];?>
                <?php
                if ( -1 == $customer_row['identified'] ) {
                    echo "<img src=\"".image_dir."/icon_transient.gif\" alt=\"Transient customer\">";

                }
                ?>
            </td>
        </tr>
        <tr>
            <td class=label><?=(!isBlank($customer_row['full_name'])) ? $lang->getLang("Customer Name") : $lang->getLang("Customer Username")?></td>
            <td class=content><?=(!isBlank($customer_row['full_name'])) ? $customer_row['full_name'] : $customer_row['username']?></td>
        </tr>
        <tr>
            <td class=label><?=$lang->getLang("Username")?></td>
            <td class=content><?=$customer_row['username']?></td>
        </tr>
        <tr>
            <td class=label><?=$lang->getLang("Operation")?></td>
            <td class=content><select name="operation">
                    <? if($depositAccess){?>
                    <option value="deposit"><?=$lang->getLang("Deposit")?></option>
                    <?}if($witdrawAccess){?>
                    <option value="withdraw"><?=$lang->getLang("Withdraw")?></option>
                    <? }?>
                </select>
            </td>
        </tr>
        <tr>
            <td class=label><?=$lang->getLang("Customer available Balance")?></td>
            <td class=content>
                <?=printInDollars($customer_row['balance'],2,false,$customer_row['code_for_web']);?>
            </td>
        </tr>
        <tr>
            <td class=label><?=$lang->getLang("Customer reserved funds")?></td>
            <td class=content>
                <?printInDollars($customer_row['reserved_funds']);?>
            </td>
        </tr>
        <tr>
            <td class=label><?=$lang->getLang("Amount")?></td>
            <td class=content>
                <input type="text" name="amount" value="<?=replace_quotes($amount)?>">
            </td>
        </tr>
        <tr valign="top">
            <td class=label><?=$lang->getLang("Payment Method")?></td>
            <td class=content>
                <select name="payment_method" onChange="toggle_wmc_number_input(this)">
                    <? printOption('payment_method', 'cashier cash', "Cash")?>
                    <? //printOption('payment_method', 'wmc', "WMC card")?>
                </select>
                <div id="wmc_number_div" style="padding:4">
                    <?php
                    if ( 'wmc' == $_POST['payment_method'] ) {
                        ?>
                        card number <input type="text" name="wmc_card_number" value="<?=$_POST['wmc_card_number'];?>" size="20">&nbsp;
                    <?php
                    }
                    ?>
                </div>
            </td>
        </tr>
        <tr>
            <td class=label><?=$lang->getLang("Note")?></td>
            <td class="content"><textarea  name="note"></textarea></td>
        </tr>

        <tr>
            <td></td>
            <td><a href="<?=refPage("$page&phase=search&customer_str=$customer_str")?>"><img src="<?=image_dir?>/button_back.gif" border=0></a> <a href="<?=refPage("$page&customer_id=$customer_id")?><input type="image" src="<?=image_dir?>/button_back.gif" border=0></a> <input type="image" src="<?=image_dir?>/button_submit.gif" border=0></td>
        </tr>
    </table>
    </form>
    </p>
<?php  }
else {
    addError("", $lang->getLang("Invalid customer ID"));
    $phase = 'search';
}
}
else {
    addError("",  $lang->getLang("A customer must be selected"));
    $phase = 'search';
}
}
if ( 'search' == $phase ) {
globalize('customer_str');


if ( isBlank($customer_str) || (!is_numeric($customer_str) && $_POST['searchBy']=='3') ) {
    if(!is_numeric($customer_str) && $_POST['searchBy']=='3'){
        addError("search_string", $lang->getLang("Please enter a number"));
    }
    else{
    addError("search_string", $lang->getLang("Please enter a customer number or search string"));
    }
    $phase = '';
}
else {
$sql  = "select pun_id as \"id\",pun_betting_club, pun_username,pun_email, pun_customer_number as \"customer number\",  pun_last_name || ', ' ||  pun_first_name || ' ' || pun_middle_name as \"customer name\", pun_identified as \"identified\"";
$sql .= " from punter ";
$jurCheck=doJurisdictionCheck();
$sql .=$jurCheck;
$sql.=" AND pun_member_type  IN ('FINANCIAL','LIMITED') ";


if ( $_POST['searchBy']=='3' ) { //assume customer number

    $sql .= " AND pun_customer_number = $customer_str";

}
elseif ( $_POST['searchBy']=='2' ) {
    if($_POST['exactlyResult']==1){
        $sql .= " AND pun_email =".$dbh->prepareString($customer_str);
    }
    else{
        $sql .= " AND pun_email like ('%".escapeSingleQuotes($customer_str)."%')";
    }

}
else {
    if($_POST['exactlyResult']==1){
        $sql .= " AND pun_username =".$dbh->prepareString($customer_str);
    }
    else{
        $sql .= " AND pun_username like ('%".escapeSingleQuotes($customer_str)."%')";
    }
}

$sql .= " order by pun_last_name, pun_first_name, pun_middle_name";
    $starttime = microtime(true);
$rs  = $dbh->exec($sql); //execute search query
$num_rows = $rs->getNumRows();
    $endtime = microtime(true);
    $duration = $endtime - $starttime;
    $duration=number_format($duration, 4, ',', '')."s";
if ( $num_rows > 0 ) {
?>
    <div class="bodyHD2"><?=$lang->getLang("Step 2: Select a customer")?></div>
<br/>
    <div class="tip fleft"><?=$lang->getLang("Time taken to execute your request")?>: <?=$duration?></div>
    <br />
<?php showErrors()?>
<?php
while ( $rs->hasNext() ) {
    $rs->next();
    $rec = & $rs->currentRef();
    $rec[$lang->getLang("select")] = '<input type="radio" name="customer_id" value="'.$rec['id'].'">';

    //if ( ',' ==  mb_ereg_replace(' ', '', ($rec['customer name'])) )
    $rec[$lang->getLang("Customer name")] = $rec['pun_username'];
    $rec[$lang->getLang("Email")] = $rec['pun_email'];
    $rec[$lang->getLang("Jurisdiction")] = getUserClubJurisdictionTree($rec['pun_betting_club']);

    if ( -1 == $rec['identified'] )
        $rec[$lang->getLang("Customer number")] = "<img src=\"".image_dir."/icon_transient.gif\" alt=\"Transient customer\">";
    $rec[$lang->getLang("Customer number")].=$rec['customer number'];
}

$post_vars = array('customer_str'=>$customer_str, 'max_rows'=>$max_rows, 'phase'=>'enter_details', 'page'=>$page);
$qp = new QueryPrinter($rs);
$form_name = $qp->printTable($rs,array($lang->getLang("Customer number"),$lang->getLang("Customer name"),$lang->getLang("Email"),$lang->getLang("Jurisdiction"), $lang->getLang("select")), $post_vars, $val_fmt_arr, $cell_fmt_arr, $sum_fmt_arr, $excel_filename="", $header="", $width="", $start_row=1, $max_rows=1000);
?>
    <a href="<?=refPage($page)?>"><img src="<?=image_dir?>/button_back.gif" border=0></a>  <a href="javascript:document.forms.<?=$form_name?>.submit()"><img src="<?=image_dir?>/button_next.gif" border=0></a>
<br/>
<?php }
        else {
            addError("customer_str", $lang->getLang("No matching customers found"));
            unset($phase);
        }
    }
}

if ( !$phase )
{
    ?>
    <div class="bodyHD2"><?=$lang->getLang("Step 1: Find a customer")?></div>
    <br>
    <?showErrors()?>
    <p>
        <?form_head()?>
        <input type="hidden" name="phase" value="search">
    <table cellpadding=4 cellspacing=1 border=0>
        <tr>
            <td class=label><?=$lang->getLang("Customer Search Text")?></td>
            <td class=content><input type="text" name="customer_str" value="<?=replace_quotes($customer_str)?>"></td>
        </tr>
    <tr>
        <td class=label><?=$lang->getLang("Search by")?></td>
        <td class=content>
             <select name="searchBy" id="searchBy">
                <option value="1" <?=$_POST['searchBy']=='1'? 'selected' :''  ?>><?=$lang->getLang("Username")?></option>
                <option value="2" <?=$_POST['searchBy']=='2'? 'selected' :''  ?>><?=$lang->getLang("Email")?></option>
                <option value="3" <?=$_POST['searchBy']=='3'? 'selected' :''  ?>><?=$lang->getLang("Customer number")?></option>
            </select>
        </td>
    </tr>
    <tr>
            <td class=label><?=$lang->getLang("Exactly result")?></td>
            <td class=content><input type="checkbox" name="exactlyResult" value="1"></td>
        </tr>
        <tr>
            <td>&nbsp;</td>
            <td><input type="image" src="<?=image_dir?>/button_next.gif" border=0></td>
        </tr>
    </table>
    </form>
    </p>
    <script type="text/javascript">
        $(function(){
            $("input[name='customer_str']").bind('keypress',function(e){
                    if($('#searchBy').val()=='3'){
                    if (e.which < 48 ||e.which > 57 ) {
                        e.preventDefault();
                    }
                    }
                }
            );
        });
    </script>
<?
}
?>
