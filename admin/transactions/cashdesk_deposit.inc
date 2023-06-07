<?check_access('transactions_deposits', $redirect=true);?>
<div  class="bodyHD"><?=$lang->getLang("Cashier Desk Deposit")?></div>
<?php
function checkFields($club_id, $amount, $payment_method, $password) {
    global $dbh,$lang;

    if ( isBlank($amount) || $amount <= 0 ) {
        addError("", $lang->getLang("An amount must be entered"));
    }
    elseif ( ! isValidMoney($amount) ) {
        addError("", $lang->getLang("Invalid amount entered - must be a Euro amount (cents optional)"));
    }
    elseif ( defined('MAX_DEPOSIT_LIMIT') &&MAX_DEPOSIT_LIMIT == true &&
        $amount > MAX_DEPOSIT_AMOUNT) {
        $max_deposit = getInDollars(MAX_DEPOSIT_AMOUNT);
        addError("", $lang->getLang("Maximum deposit allowed is %", $max_deposit));
    }
    else {
        $sql =  "SELECT jur_available_credit" .
            "   FROM  jurisdiction " .
            "   WHERE jur_id = " . $club_id;
        //echo $sql;
        $rs       = $dbh->exec($sql);
        $num_rows = $rs->getNumRows();

        if ( $num_rows > 0 ) {
            $row = $rs->next();
            $available_credit = $row['jur_available_credit'];

            if ( $available_credit - $amount < 0 ){
                $max_deposit = getInDollars($available_credit);
                addError("", $lang->getLang("Cannot create customer - cannot process deposit. Your club does not have enough available credit. Maximum deposit allowed is %",$max_deposit));
                return false;
            }
        }
        else {
            addError("", $lang->getLang("Critical error: Missing account!"));
        }
    }

    if ( isBlank($payment_method) ) {
        addError("", $lang->getLang("A payment method must be selected"));
    }
    else {
        if ( !('cashier cheque' == $payment_method || 'cashier cash' == $payment_method ||
            'cashier credit card' == $payment_method || 'cashier bonus' == $payment_method) ) {
            addError("",  $lang->getLang("Invalid payment method"));
        }
    }
    /*
     if ( isBlank($password) )
        addError("", "Your cashier password must be entered");
     elseif ( ! adminUserAuthenticated($_SESSION['admin_username'], $password) )
        addError("", "Your cashier password is incorrect");
    */
    return ! areErrors();
}


function do_cash_deposit ($amount, $customer_id, $aus_id,  $club_id) {
    global $dbh, $console_port;

    require 'account_functions.php';

    $dbh->begin();

    // perform credit to customer account

    $deposited = doDeposit($customer_id, $amount);

    if ( FALSE == $deposited ) {
        $dbh->rollback();
        return false;
    }

    $customer_row = getCustomer($customer_id,false,true);
    $betting_club = $customer_row['betting_club'];

    // tie customer to this betting club if is not tied already

    if ( $betting_club != $_SESSION['jurisdiction_id'] ) {
        $dbh->exec("update punter set pun_betting_club = ".$club_id." where pun_id = $customer_id");

        if ( $dbh->AffectedRows != 1 ) {
            $dbh->rollback();
            return false;
        }

        //remove customer from original groups by deleteing his/her records from punter_cgroup
        //$dbh->exec("DELETE FROM punter_cgroup WHERE pcg_pun_id = $customer_id");

        //$dbh->exec("INSERT INTO punter_cgroup (pcg_cgr_code, pcg_pun_id) VALUES('CASHDESK', $customer_id)");

    }


    // insert customer transaction record

    $ctr_id = $dbh->nextSequence('ctr_id_seq');
    $transaction_number = nextTxnNumberCashdesk();

    $sql  = "INSERT INTO customer_transaction" .
        " ( ctr_id, ctr_pun_id, ctr_amount" .
        ", ctr_tty_id, ctr_pme_code, ctr_status" .
        ", ctr_aus_id, ctr_time, ctr_transaction_num)" .
        "  VALUES ($ctr_id, $customer_id, $amount" .
        ", ".T_TYPE_DEPOSIT.", 'CSH', 'completed'" .
        ", $aus_id,  CURRENT_TIMESTAMP, '$transaction_number')";

    $dbh->exec($sql);

    if ( $dbh->AffectedRows != 1 ) {
        $dbh->rollback();
        return false;
    }


    ////
    // adjust jurisdiction records
    ////

    $sql = "SELECT jur_id from jurisdiction WHERE jur_code = 'CUS'";
    $rs  = $dbh->exec($sql);

    $row = $rs->next();
    $cus_jur_id = $row['jur_id'];

    // update the club account's balance
    $club_rec_updated = subtract_available_credit($club_id, $amount);

    // update the dummy custoemr account's balance
    $cus_rec_updated  = add_available_credit($cus_jur_id, $amount);


    // check that the club and customer dummy jurisdiction records were Sinserted

    if ( FALSE == $club_rec_updated && FALSE == $cus_rec_updated ) {
        $dbh->rollback();
        return false;
    }


    ////
    // log the credit transfer
    ////

    $log_entity_trans = log_credit_transfer ($club_id, $cus_jur_id, $amount, 'OUT', $info=$id);
    $log_club_trans   = log_credit_transfer ($cus_jur_id, $club_id, $amount, 'IN', $info=$id);

    // check that the credit_transfer records were inserted

    if ( FALSE == $log_entity_trans &&  FALSE == $log_club_trans ) {
        $dbh->rollback();
        return false;
    }

    //this is only reached if all inserts and updates were successful
    $dbh->commit();

    //sendPunterCredits($customer_id, $console_port);

    return $transaction_number;
}


globalizeStr('phase');
globalizeInt('customer_id');
globalizeFloat('amount');
//globalize('password');
$password="";
globalize('payment_method');
globalize('scode');
globalize('customer_str');


if ( 'process_transaction' == $phase ) {
//   $sql = "select jur_id from jurisdiction where jur_class = 'club' and jur_name = 'Internet'";
//   $rs = $dbh->exec($sql);
//   if ( 1 == $rs->getNumRows() ){
//     $row = $rs->next();
//     $internet_jur_id = $row['jur_id'];
//   }


    if ( $scode && $_SESSION['scode'] == $scode  ) {
        addError("", $lang->getLang("Cannot resubmit/refresh deposit transaction"));
        $phase = 'enter_details';
    }
    else {
        if ( $customer_id ) {
            $customer_id = (int)$customer_id;
            $customer_row = getCustomer($customer_id, false, true);

            if ( $customer_row )
                $phase = 'select_payment_method';
            else {
                dieWithError($lang->getLang("Invalid customer ID: %",$customer_id));
                unset($phase);
            }
        }
        else {
            dieWithError($lang->getLang("Missing customer ID"));
        }

        if ( checkFields($_SESSION['jurisdiction_id'],$amount, $payment_method, $password) ) {
            //if ( punterIsPlaying($customer_id) ) {
            //   addError("", "Cannot perform a transaction because this customer is currently logged in to a game");
            //   $phase = 'enter_details';
            //}  //if had made a deposit (tied to a betting club) and this admin person;s club is different, he can't make a deposit
            //elseif ( $customer_row['betting club'] != 'Internet' && $customer_row['betting_club'] != $_SESSION['jurisdiction_id'] ) {
            if ( $customer_row['betting_club'] != $_SESSION['jurisdiction_id'] ) {
                $club_name = getClubName($customer_row['betting_club']);
                addError("",$lang->getLang("Transactions cannot be performed for this customer at this club because the customer's account is held at another club (%)",$club_name));
                $phase = 'enter_details';
            }
            else {
                if($customer_row['betting_club'] == 'Internet' && $_SESSION['jurisdiction_class'] != 'club' ) {
                    addError("", $lang->getlang("Transactions cannot be performed on this customer's account until they have made their initial deposit via the cashdesk."));
                    $phase = 'enter_details';
                }
                else {
                    $betting_club_prev = $customer_row['betting_club'];

                    //perform deposit
                    //$dbh->lockRow('punter_credit', array('pcr_pun_id'=>$customer_id));//lock punter_credit row
                    require_once("queue_func.php");
                    if(queue_begin("TDPL-$customer_id", 120)){
                        $transaction_num = do_cash_deposit ($amount, $customer_id, $_SESSION['admin_id'], $_SESSION['jurisdiction_id']);
                    }else{
                        dieWithError($lang->getLang("Another transaction for this customer is running, wait a few for a new transaction, thanks."));
                    }


                    if ( $transaction_num ) {
                        printInfoMessage($lang->getLang("Deposit successful"));
                        if ( !$betting_club_prev ) {
                            ?>
                            <br/>
                            <div class=highlight>
                                <?=$lang->getLang("Please advise this customer that from this point forward, they may deposit or withdraw money from their betting account at this club only.")?>
                            </div>
                        <?
                        }

                        if ( -1 == $customer_row['identified'] )
                        {
                            $icon = "<img src=\"".image_dir."/icon_transient.gif\" alt=\"Transient customer\">";
                        }


                        registerSubmitVar('scode');
                        ?>
                        <p>
                        <table cellpadding=4 cellspacing=1 border=0>
                            <? sideTableRow($lang->getLang("Amount Deposited"), getInDollars($amount), 'right'); ?>
                            <? sideTableRow($lang->getLang("Customer"), $customer_row['customer_number'] .': ' . ( isBlank($customer_row['full_name']) ? $customer_row['username'] : $customer_row['full_name']). '&nbsp;'.$icon, 'right'); ?>
                            <? sideTableRow($lang->getLang("New Customer Balance"), getInDollars(getCustomerBalance($customer_id)), 'right'); ?>
                            <? sideTableRow($lang->getLang("New Customer Available Balance"), getInDollars(getCustomerAvailableBalance($customer_id)), 'right'); ?>
                            <? sideTableRow($lang->getLang("Transaction Reference Number"), $transaction_num, 'right'); ?>
                        </table>
                        </p>
                        <p>
                            <?printlink(refPage(""), $lang->getLang("Click Here"));?> <?=$lang->getLang("to return to the main page.")?>
                        </p>
                    <?           }
                    else {
                        addError("", $lang->getLang("Database Error"));
                        $phase = 'enter_details';
                    }
                }
            }
        }
        else
            $phase = 'enter_details';
    }
}


if ( 'enter_details' == $phase || ! $phase && $customer_id )
{
    if ( $customer_id )
    {
        $customer_id = (int)$customer_id;
        $customer_row = getCustomer($customer_id, false, true);

        if ( $customer_row )
        {

            ?>
            <div class="bodyHD2"><?=$lang->getLang("Step 3: Enter deposit details")?></div>
            <p>
                <?showErrors()?>
            </p>
            <p>
                <?form_head()?>
                <input type="hidden" name="phase" value="process_transaction">
                <input type="hidden" name="customer_id" value="<?=$customer_id?>">
                <input type="hidden" name="scode" value="<?=mktime().session_id()?>">
            <table cellpadding=4 cellspacing=1 border=0>
                <tr>
                    <td class=label><?=$lang->getLang("Customer Number")?></td>
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
                    <td class=label><?=$lang->getLang("Amount to Deposit")?> </td>
                    <td class=content>
                        <input type="text" name="amount" value="<?=replace_quotes($amount)?>">
                    </td>
                </tr>
                <tr>
                    <td class=label><?=$lang->getLang("Payment Method")?></td>
                    <td class=content>
                        <select name="payment_method">
                            <? //printOption('payment_method', '', '- select payment method -')?>
                            <? printOption('payment_method', 'cashier cash', "Cash")?>
                            <? //printOption('payment_method', 'cashier cheque', 'Cheque')?>
                            <? //printOption('payment_method', 'cashier credit card', 'Credit Card')?>
                            <? //printOption('payment_method', 'cashier bonus', 'Bonus')?>
                        </select>
                    </td>
                </tr>
                <!--
               <tr>
                  <td class=label>Your password</td>
                  <td class=content>
                     <input type="password" name="password" value="<?=replace_quotes($_POST['password'])?>">  
                  </td>
               </tr>
-->
                <tr>
                    <td>&nbsp;</td>
                    <td><a href="<?=refPage("$page&phase=search&customer_str=$customer_str")?>"><img src="<?=image_dir?>/button_back.gif" border=0></a> <a href="<?=refPage("$page&customer_id=$customer_id")?><input type="image" src="<?=image_dir?>/button_back.gif" border=0></a> <input type="image" src="<?=image_dir?>/button_submit.gif" border=0></td>
                </tr>
            </table>
            </form>
            </p>
        <?
        }
        else
        {  addError("", $lang->getLang("Invalid customer ID"));
            $phase;
        }
    }
    else
    {  addError("",  $lang->getLang("A customer must be selected"));
        $phase = 'search';
    }
}

if ( 'search' == $phase )
{
    globalize('customer_str');

    if ( isBlank($customer_str) )
    {   addError("search_string", $lang->getLang("Please enter a customer number or search string"));
        $phase = '';
    }
    else
    {
        $sql  = "select pun_id as \"id\", pun_username, pun_customer_number as \"Customer Number\", " .
            " pun_last_name || ', ' ||  pun_first_name || ' ' || pun_middle_name as \"Customer Name\"," .
            " pun_identified as \"Identified\", pun_member_type as \"Member Type\"" .
            " from punter where pun_member_type = 'FINANCIAL' ";

        if ( is_numeric($customer_str) ) { //assume customer number
            $sql .= " AND pun_customer_number = $customer_str";
        }
        else {
            //** modified to only allow club customers to do cash deposits

//         $sql .= " and ( pun_first_name like ('%".escapeSingleQuotes($customer_str)."%')" .
//                 "       or pun_middle_name like ('%".escapeSingleQuotes($customer_str)."%')" .
//                 "       or pun_last_name like ('%".escapeSingleQuotes($customer_str)."%')" .
//                 "       or pun_email like ('%".escapeSingleQuotes($customer_str)."%')" .
//                 "       or pun_username like ('%".escapeSingleQuotes($customer_str)."%')" .
//                 "      )";
            $sql .= " AND ( pun_username like ('%".escapeSingleQuotes($customer_str)."%')" .
                "      )";
        }

//      $sql .= " AND ((pun_betting_club = " . $_SESSION['jurisdiction_id'] . 
//              "  AND (SELECT pcg_cgr_code FROM punter_cgroup WHERE pcg_pun_id = pun_id) = 'CASHDESK') " .
//              " OR  (pun_member_type = 'NONFINANCIAL' AND pun_betting_club = " .
//              "        (SELECT jur_id FROM jurisdiction WHERE jur_class = 'club' AND jur_name = 'Internet') )) " .
//              " ORDER by pun_last_name, pun_first_name, pun_middle_name";

//      $sql .= " AND (pun_betting_club = " . $_SESSION['jurisdiction_id'] . 
//              "  OR  (pun_member_type = 'NONFINANCIAL' AND pun_betting_club = " .
//              "        (SELECT jur_id FROM jurisdiction WHERE jur_class = 'club' AND jur_name = 'Internet') )) " .
//              " ORDER by pun_last_name, pun_first_name, pun_middle_name";
        $sql .= " AND pun_betting_club = " . $_SESSION['jurisdiction_id'] .
            " ORDER by pun_username";


        $rs =$dbh->exec($sql); //execute search query
        $num_rows = $rs->getNumRows();

        if ( $num_rows > 0 )
        {
            ?>
            <div class=bodyHD2><?=$lang->getLang("Step 2: Select a customer")?></div>
            <br/>
            <?showErrors()?>
            <?
            while ( $rs->hasNext() )
            {
                $rs->next();
                $rec = & $rs->currentRef();

                $rec[$lang->getLang('select')] = '<input type="radio" name="customer_id" value="'.$rec['id'].'">';

//                /if ( ',' ==  mb_ereg_replace(' ', '', ($rec['pun_username'])) )
                    $rec[$lang->getLang("Customer name")] = $rec['pun_username'];
                if ( -1 == $rec['identified'] )
                    $rec[$lang->getLang("Customer number")] = "<img src=\"".image_dir."/icon_transient.gif\" alt=\"Transient customer\">" . $rec['customer number'];

                $rec[$lang->getLang("Membership type")] = $rec["member type"];
            }


            $post_vars = array('customer_str'=>$customer_str, 'max_rows'=>$max_rows, 'phase'=>'enter_details', 'page'=>$page);
            $qp = new QueryPrinter($rs);
            $form_name = $qp->printTable($rs,array($lang->getLang("Customer number"),$lang->getLang("Customer name"), $lang->getLang("Membership type"), $lang->getLang('select')), $post_vars, $val_fmt_arr, $cell_fmt_arr, $sum_fmt_arr, $excel_filename="", $header="", $width="", $start_row=1, $max_rows=1000);
            ?>
            <a href="<?=refPage($page)?>"><img src="<?=image_dir?>/button_back.gif" border=0></a>  <a href="javascript:document.forms.<?=$form_name?>.submit()"><img src="<?=image_dir?>/button_next.gif" border=0></a>
            <br/>
        <?    }
        else
        {  addError("customer_str", $lang->getLang("No matching customers found"));
            unset($phase);
        }

    }
}

/*if ("owncustomers" == $phase) {

    $sql  = "SELECT pun_id AS \"id\", pun_username, pun_customer_number as \"Customer Number\", " .
        " pun_last_name || ', ' ||  pun_first_name || ' ' || pun_middle_name as \"Customer Name\"," .
        " pun_identified as \"Identified\", pun_member_type as \"Member Type\", " .
        " pcr_credits as \"Credit\" ".
        " FROM punter,punter_credit " .
        " WHERE pun_member_type = 'FINANCIAL' AND pun_betting_club=" . $_SESSION['jurisdiction_id'] .
        //   "  AND ((SELECT pcg_cgr_code FROM punter_cgroup WHERE pcg_pun_id = pun_id) = 'CASHDESK')" .
        "  AND pcr_pun_id=pun_id " .
        "order by pun_last_name, pun_first_name, pun_middle_name";
    //echo $sql;

    $rs =$dbh->exec($sql); //execute search query
    $num_rows =$rs->getNumRows();

    if ( $num_rows > 0 ) {
        */?><!--
        <div class="bodyHD2">Step 2: Select a customer</div>
        <br/>
        <?/*showErrors()*/?>
        <?/*
        while ( $rs->hasNext()){
            $rs->next();
            $rec = & $rs->currentRef();
            $rec['select'] = '<input type="radio" name="customer_id" value="'.$rec['id'].'">';

            if ( ',' ==  mb_ereg_replace(' ', '', ($rec['customer name'])) )
                $rec['customer name'] =$rec['pun_username'];


            if ( -1 == $res['Identified'] )
                $rec['Customer Number'] = "<img src=\"".image_dir."/icon_transient.gif\" alt=\"Transient customer\">" . $rec['customer number'];
        }

        $post_vars = array('customer_str'=>$customer_str, 'max_rows'=>$max_rows, 'phase'=>'enter_details', 'page'=>$page);
        $qp = new QueryPrinter($rs);
        $func = 'if ($rec["member type"] == "FINANCIAL") return getInDollars($rec["credit"]+0); else return getInFunTokens($rec["credit"]+0);';
        $val_fmt_arr = array (
            'Credit' => $func
        );

        //var_dump($rs);
        $form_name = $qp->printTable($rs,array('customer number','customer name', 'member type', 'credit', 'select'), $post_vars, $val_fmt_arr, $cell_fmt_arr, $sum_fmt_arr, $excel_filename="", $header="", $width="", $start_row=1, $max_rows=1000);
        */?>
        <a href="<?/*=refPage($page)*/?>"><img src="<?/*=image_dir*/?>/button_back.gif" border=0></a>  <a href="javascript:document.forms.<?/*=$form_name*/?>.submit()"><img src="<?/*=image_dir*/?>/button_next.gif" border=0></a>
        <br/>
    --><?/*    }else{
        addError("customer_str", $lang->getLang("No matching customers found"));
        unset($phase);
    }
}*/

if ( !$phase )
{
    ?>
    <div class="bodyHD2"><?=$lang->getLang("Step 1: Find a customer")?></div>
    <br/>
    <?showErrors()?>
    <p>
        <?form_head()?>
        <input type="hidden" name="phase" value="search">
    <table cellpadding="4" cellspacing="1" border="0">
        <tr>
            <td class="label"><?=$lang->getLang("Customer Search Text")?></td>
            <td class="content">
                <input type="text" name="customer_str" value="<?=replace_quotes($customer_str)?>">
            </td>
        </tr>
        <!--<tr>
            <td colspan="2"><a href="<?= SELF."?page=$page&header_page=$header_page&phase=owncustomers" ?>"><?=$lang->getLang("Go to customers list")?></a></td>
        </tr>-->
        <tr>
            <td>&nbsp;</td>
            <td><input type="image" src="<?=image_dir?>/button_next.gif" border=0></td>
        </tr>
    </table>
    </form>
    </p>
<?
}
?>
