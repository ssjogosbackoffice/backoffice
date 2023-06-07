<?php
check_access('transactions_corrective', $doredirect=true);
globalize('bonus');
?>
<div  class=bodyHD><?=$lang->getLang("Corrective Transaction")?> <?=ucwords($bonus)?></div>
<?php

function updateCustomerTrans($customer_id,$trans_type,$refund_type,$transaction_number, $amount, $reason, $aus_id){
    global $dbh;
    $ctr_id = $dbh->nextSequence('ctr_id_seq');
    $sql  = "INSERT INTO customer_transaction";
    $sql .= " ( ctr_id, ctr_pun_id,ctr_balance_available, ctr_amount,  ctr_tty_id, ctr_status, ctr_pme_code";
    $sql .= ",  ctr_aus_id,   ctr_time, ctr_transaction_num, ctr_notes)";
    $sql .= "   VALUES ($ctr_id, $customer_id,(SELECT pcr_credits from punter_credit where pcr_pun_id=$customer_id), $amount, $trans_type";
    $sql .= ", 'completed', '$refund_type', $aus_id, CURRENT_TIMESTAMP";
    $sql .= ", '$transaction_number', '$reason')";
    $dbh->exec($sql);
}

function doAdjustment ($customer_id, $type, $amount, $reason, $aus_id, $mngr_id="NULL") {
    global $dbh, $console_port,$lang;
    $dbh->begin();
    //$dbh->lockRow('punter_credit', array('pcr_pun_id'=>$customer_id));
    switch ($type) {
        case "deposit":
            $trans_type = T_TYPE_CORRECTION;
            $refund_type = "CRN";
            $transaction_number = nextTxnNumberAdjustment();
            updateCustomerTrans($customer_id,$trans_type,$refund_type,$transaction_number, $amount, $reason, $aus_id);
            doDeposit($customer_id, $amount);
            break;
        case "withdrawl":

            $trans_type = T_TYPE_CORRECTION;
            $refund_type = "CRN";
            $transaction_number = nextTxnNumberAdjustment();
            updateCustomerTrans($customer_id,$trans_type,$refund_type,$transaction_number, -$amount, $reason, $aus_id);
            doWithdrawal($customer_id, $amount);
            break;
        case "bonus":

            $trans_type = T_TYPE_CORRECTION;
            $refund_type = "BON";
            $transaction_number = nextTxnBonusAdjustment();
            updateCustomerTrans($customer_id,$trans_type,$refund_type,$transaction_number, $amount, $reason, $aus_id);
            doBonusDeposit($customer_id, $amount);
            break;
        default:
            addError("", $lang->getLang("Invalid type"));
            showErrors();
            exit();
            break;
    }

    addPunterNote($customer_id, $aus_id, "Corrective transaction $type #$transaction_number: $reason " . getInDollars($amount));

    $dbh->commit();


    return $transaction_number;
}

function checkFields($amount, $correction_type, $password, $notes){
    global $HTTP_SESSION_VARS, $bonus,$lang;

    if ( isBlank($amount) )
        addError("", $lang->getLang("An amount must be entered"));
    else
        if ( !isValidMoney($amount) && $bonus != "bonus")
            addError("", $lang->getLang("Invalid amount entered - must be a ".((isset($_SESSION['currency_html']) && $_SESSION['currency_html']!='') ? $_SESSION['currency_html'] : "&euro;" )." amount (cents optional)"));

    if ( isBlank($correction_type) )
    {
        addError("", $lang->getLang("A payment method must be selected"));
    }
    else
        if ( !('deposit' == $correction_type || 'withdrawl' == $correction_type))
            addError("", $lang->getLang("Invalid Payment Method"));

    if ( isBlank($password) )
        addError("", $lang->getLang("Your cashier password must be entered"));
    elseif ( ! adminUserAuthenticated($_SESSION['admin_username'], $password) )
        addError("", $lang->getLang("Your password must be entered"));

    if(isBlank($notes))
        addError("", $lang->getLang("A reason for this adjustment must be entered"));

    return ! areErrors();
}

globalise('phase');
globalise('customer_id');
globalise('amount');
globalise('password');
globalise('notes');
globalise('correction_type');
globalise('scode');
globalise('customer_str');

if('process_transaction' == $phase){

if ( $scode && $_SESSION['scode'] == $scode  ){
    addError("", $lang->getLang("Cannot resubmit/refresh deposit transaction"));
    $phase = 'enter_details';
}
else{
if ( $customer_id ){
    $customer_id = (int)$customer_id;
    $customer_row = getCustomer($customer_id);
    if ( $customer_row )
        $phase = 'select_payment_method';
    else{
        dieWithError($lang->getLang("Invalid customer ID:"). $customer_id);
        unset($phase);
    }
}
else{
    dieWithError($lang->getLang("Missing customer ID"));
}

if ( checkFields($amount, $correction_type, $password, $notes) ){
/* if ( punterIsPlaying($customer_id) ){
 addError("", $lang->getLang("Cannot perform a corrective transaction because this customer is currently logged in to a game"));
 $phase = 'enter_details';
}
else{   //if had made a deposit (tied to a betting club) and this admin person;s club is different, he can't make a deposit */
// if($customer_row['Betting Club'] == 'Internet' && $_SESSION['jurisdiction_class'] != 'club')

//$customer_row

if($customer_row["total_deposits"] <= 0){
    addError("", $lang->getLang("Transactions cannot be performed on this customer's account until they have made their initial deposit via the cashdesk."));
    $phase = 'enter_details';
}
else{
if($correction_type == 'withdrawl' && $amount > getCustomerAvailableBalance($customer_id)){
    addError("", $lang->getLang("Cannot perform a transaction because this customer's available balance is less than the requested amount"));
    $phase = 'enter_details';
}
$betting_club_prev = $customer_row['betting_club'];
// do transaction
//var_dump($_SESSION);

if($bonus == "bonus"){
    $correction_type = "bonus";
}

$reference_num = doAdjustment($customer_id, $correction_type, $amount, $notes, $_SESSION['admin_id'], "NULL");
$_SESSION['scode'] = $scode;

?>
    <p>
    <table cellpadding=4 cellspacing=1 border=0>
        <? sideTableRow("Correction Type", ucwords($correction_type), 'right'); ?>
        <? sideTableRow("Correction Amount", getInDollars($amount), 'right'); ?>
        <? sideTableRow("Customer", $customer_row['customer_number'] .': ' . ( isBlank($customer_row['full_name']) ? $customer_row['username'] : $customer_row['full_name']). '&nbsp;'.$icon, 'right');?>
        <? sideTableRow("New Customer Balance", getInDollars(getCustomerBalance($customer_id)), 'right'); ?>
        <? sideTableRow("New Customer Available Balance", getInDollars(getCustomerAvailableBalance($customer_id)), 'right'); ?>
        <? sideTableRow("Transaction Reference Number", $reference_num, 'right'); ?>
    </table>
    </p>
<p>
    <? printClassLink(refPage("customers/customer_view&customer_id=" . $customer_row["id"]), $lang->getLang("Click here to return to  ") . $customer_row["username"]); ?>
    <?php    }
    // }
    }
    else
        $phase = 'enter_details';
    }
    }

    if ( 'enter_details' == $phase || ! $phase && $customer_id ){
    if ( $customer_id ){
    $customer_id = (int)$customer_id;
    $customer_row = getCustomer($customer_id);

    if ( $customer_row ){
    ?>
    <div class=bodyHD2><?=$lang->getLang("Step 3: Enter transaction details")?></div>
    <P>
        <?showErrors()?>
    <P>
        <?form_head()?>
        <input type="hidden" name="phase" value="process_transaction">
        <input type="hidden" name="customer_id" value="<?=$customer_id?>">
        <input type="hidden" name="scode" value="<?=mktime().session_id()?>">
        <input type="hidden" name="bonus" value="<?=$bonus?>">
    <table cellpadding=4 cellspacing=1 border=0>
        <tr>
            <td class=label><?=$lang->getLang("Customer Number")?></td>
            <td class=content>

                <?=$customer_row['customer_number'];?>
                <?php
                if ( -1 == $customer_row['identified'] ){
                    echo "<img src=\"".image_dir."/icon_transient.gif\" alt=\"Transient customer\">";
                }
                ?>
            </td>
        </tr>
        <tr>
            <td class=label><?=(!isBlank($customer_row['full_name'])) ? "Customer Name" : "Customer Username"?></td>
            <td class=content><?=(!isBlank($customer_row['full_name'])) ? $customer_row['full_name'] : $customer_row['username']?></td>
        </tr>
        <tr>
            <td class=label><?=$lang->getLang("Customer Account Balance")?></td>
            <td class=content style="font-weight: bold;">
                <?=getInDollars(getCustomerBalance($customer_id))?>
            </td>
        </tr>
        <tr>
            <td class=label><?=$lang->getLang("Customer Available Balance")?></td>
            <td class=content style="font-weight: bold;">
                <?=getInDollars(getCustomerAvailableBalance($customer_id))?>
            </td>
        </tr>
        <?php if($bonus == "bonus"): ?>
            <input type="hidden" name="correction_type" value="deposit">
        <?php else: ?>
            <tr>
                <td class=label><?=$lang->getLang("Correction Type")?></td>
                <td class=content>
                    <select name="correction_type" style="width: 200px;">
                        <? printOption('correction_type', 'deposit', $lang->getLang('Deposit'))?>
                        <? printOption('correction_type', 'withdrawl', $lang->getLang('Withdrawal'))?>
                    </select>
                </td>
            </tr>
        <?php endif; ?>
        <tr>
            <td class=label><?=$symbol='&#8364;';?></td>
            <td class=content>
                <input type="text" name="amount" value="<?=replace_quotes($amount)?>" style="width: 200px;">
            </td>
        </tr>
        <tr valign=top>
            <td class=label><?=$lang->getLang("Reason for Adjustment")?></td>
            <td class=content>
                <textarea name="notes" wrap="hard" rows=10 cols=35 style="width: 200px;"><?=$notes?></textarea>
            </td>
        </tr>
        <tr>
            <td class=label><?=$lang->getLang("Your password")?></td>
            <td class=content>
                <input type="password" name="password" value="<?=replace_quotes($_POST['password'])?>" style="width: 200px;">
            </td>
        </tr>
        <tr>
            <td></td>
            <td><a href="<?=refPage("$page&phase=search&customer_str=$customer_str")?>"><img src="<?=image_dir?>/button_back.gif" border=0></a> <a href="<?=refPage("$page&customer_id=$customer_id")?><input type="image" src="<?=image_dir?>/button_back.gif" border=0></a> <input type="image" src="<?=image_dir?>/button_submit.gif" border=0></td>
        </tr>
    </table>
    </form>
<?php }
else{
    addError("", $lang->getLang("Invalid customer ID"));
    $phase;
}
}
else{
    addError("",  $lang->getLang("A customer must be selected"));
    $phase = 'search';
}
}

if ( 'search' == $phase ){
    globalize('customer_str');
    if ( isBlank($customer_str) ){
        addError("search_string", $lang->getLang("Please enter a customer number or search string"));
        $phase = '';
    }
    else{
        $sql  = "SELECT pun_id, pun_customer_number, pun_username, pun_last_name, pun_first_name, pun_middle_name, pun_identified FROM punter where 1=1";

        if(is_numeric($customer_str) ){
            settype($customer_str, "integer");
            $sql .= " AND pun_customer_number = $customer_str";
        }else{

            $sql .= " AND pun_username LIKE '%".$dbh->escape($customer_str)."%'";

        }
        $sql .= " ORDER BY pun_last_logged_in ASC";

        $rs = $dbh->exec($sql);

        $num_rows = $rs->getNumRows();



        if ($num_rows > 0 ){
            ?>
            <div class="bodyHD2"><?=$lang->getLang("Step 2: Select a Customer")?></div>
            <br/>

            <?php
            showErrors();

            $cols_arr    = array("Customer Number", "Username", "Select");
            $val_fmt_arr = array(
                "Customer Number" => 'return (($rec["pun_identified"] == -1)?("<img src=\'" . image_dir . "/icon_transient.gif\'/> "):("")) . $rec["pun_customer_number"];',
                "Username"        => 'return getClassLink(refPage("customers/customer_view&customer_id=" . $rec["pun_id"]), $rec["pun_username"]) . " (" . $rec["pun_last_name"] . ", " . $rec["pun_first_name"] . ")";',
                "Select"          => 'return "<input type=\"radio\" name=\"customer_id\" value=\"" . $rec["pun_id"] . "\"/>";'
            );
            $post_vars   = array(
                'customer_str'    => $customer_str,
                'max_rows'        => $max_rows,
                'phase'           => 'enter_details',
                'page'            => $page,
                'bonus'           => $bonus
            );

            $qp = new QueryPrinter($rs);

            $form_name = $qp->printTable($rs, $cols_arr, $post_vars, $val_fmt_arr, $cell_fmt_arr, $sum_fmt_arr, $excel_filename="", $header="", $width="", $start_row=1, $max_rows=1000);
            ?>

            <a href="<?=refPage($page)?>"><img src="<?=image_dir?>/button_back.gif" border="0" /></a>
            <a href="javascript:void(0);" onclick="document.forms.<?=$form_name?>.submit();"><img src="<?=image_dir?>/button_next.gif" border="0" /></a>
            <br/>

        <?php }
        else {
            adderror("customer_str", $lang->getLang("No matching customers found"));
            unset($phase);
        }
    }
}

if ( !$phase ){
    ?>
    <div class="bodyHD2"><?=$lang->getLang("Step 1: Find a Customer")?></div>
    <br>
    <?showErrors()?>
    <P>
        <?form_head()?>
        <input type="hidden" name="phase" value="search">
        <input type="hidden" name="bonus" value="<?=$bonus?>"/>
    <table cellpadding=4 cellspacing=1 border=0>
        <tr>
            <td class=label><?=$lang->getLang("Customer search text")?> </td>
            <td class=content><input type="text" name="customer_str" value="<?=replace_quotes($customer_str)?>"></td>
        </tr>
        <tr>
            <td></td>
            <td><input type="image" src="<?=image_dir?>/button_next.gif" border=0></td>
        </tr>
    </table>
    </form>
<?
}
?>
