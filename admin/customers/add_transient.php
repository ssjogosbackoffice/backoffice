<?php
check_access("create_transient_customer", true);
if(DOT_IT){
    check_access("whatthehell", true);
}

// checks the fields submitting using the customer details form
function checkDetails($club_id, $amount) {
    global $dbh;

    if(isBlank($amount))
        addError("", "An amount must be entered");
    elseif(!isValidMoney($amount))
        addError("", "Invalid amount entered - must be a Euro amount (cents optional)");
    else {
        //get cash limit
        $sql =  "SELECT jur_available_credit" .
            " FROM  jurisdiction " .
            " WHERE jur_id = " . $club_id;
        $rs = $dbh->exec($sql);
        $num_rows = $rs->getNumRows();

        if ( $num_rows > 0 ) {
            $row = $rs->next();
            $available_credit = $row['jur_available_credit'];

            if ( $available_credit - $amount < 0 ) {
                $max_deposit = getInDollars($available_credit);
                addError("", "Cannot create customer - cannot process deposit. Your club does not have enough available credit. Maximum deposit allowed is $max_deposit");
                return false;
            }
        }
        else {
            addError("", "Critical error: Missing account!");
        }
    }
    return !areErrors();
}

// inserts a customer record into the punter table
function createTransientAccount($initial_deposit) {
    global $dbh;


    require_once('account_functions.php');

    $initial_deposit = number_format(floatval($initial_deposit), 2, '.', '');

    // begin db transaction
    $dbh->begin();

    // get club code
    $sql = 'SELECT jur_code FROM jurisdiction WHERE jur_id = ' . $_SESSION['jurisdiction_id'];
    $rs  = $dbh->exec($sql);
    $row = $rs->next();

    $club_code = $row['jur_code'];

    // generate username
    $username = generate_username($club_code);

    // generate password
    mt_srand((double)microtime()*1000000);
    $str             = md5 (uniqid (rand(100, 900), true));
    $password        = substr($str, 0, 6);
    $md5_password    = md5($password);

    // create punter record
    $id              = $dbh->nextSequence('pun_id_seq');
    $admin_user_rec  = getAdminUser($_SESSION['admin_id']);
    $customer_number = generateCustomerNumber($username);

    $cou_id          = (isset($_SESSION['aus_cou_id'])) ? $_SESSION['aus_cou_id'] : 110;
    $cou_code        = getCountryCode2Chars($cou_id);


    // Insert punter record

    $sql  = "INSERT INTO " .
        "punter (pun_id, pun_customer_number, pun_username, pun_password, pun_member_type, pun_last_name," .
        "pun_access, pun_registration_status, pun_betting_club, pun_cou_id, pun_cou_code, " .
        "pun_reg_date) " .
        "VALUES ($id, $customer_number, ".db_quote($username).", '$md5_password', 'FINANCIAL', ".db_quote($username)."," .
        "'allow', 'activated', " . $_SESSION['jurisdiction_id'] . ", $cou_id, '$cou_code', current_timestamp)";
    //die($sql);
    $dbh->exec($sql);

    // check for insert errors
    if($dbh->getAffectedRows() <= 0) {
        $dbh->rollback();
        return false;
    }


    // create punter_credit record
    /*
    $sql = "insert into punter_credit (pcr_pun_id, pcr_credits) values ($id, 0)";
    $dbh->exec($sql);

    if($dbh->getAffectedRows() <= 0)
    {
       $dbh->rollback();
       return false;
    }
    */

    // log customer balance update
    $ctr_id             = $dbh->nextSequence('ctr_id_seq');
    $transaction_number = nextTxnNumberCashdesk();

    $sql  = "INSERT INTO customer_transaction (ctr_id, ctr_pun_id, ctr_amount, ctr_tty_id, ctr_pme_code, ctr_status, " .
        "ctr_aus_id, ctr_mngr_id, ctr_time, ctr_transaction_num, ctr_settle_track_num) " .
        "VALUES ($ctr_id, $id, $initial_deposit, " . T_TYPE_DEPOSIT . ", 'CSH', 'completed', " .
        $_SESSION['admin_id'] . ", NULL, CURRENT_TIMESTAMP, '$transaction_number', '$username')";
    $dbh->exec($sql);

    // check for insert errors
    if($dbh->getAffectedRows() <= 0) {
        $dbh->rollback();
        return false;
    }

    // add punter to cashdesk group
    addToGroup('CASHDESK', $id);

    ////
    // adjust customer record
    ////

    // lock credit row
    $dbh->lockRow('punter_credit', array('pcr_pun_id'=>$id));

    // do initial customer deposit
    $was_deposited = doDeposit($id, $initial_deposit);

    // check that customer balance was updated
    if($dbh->getAffectedRows() <= 0) {
        $dbh->rollback();
        return false;
    }

    ////
    // adjust jurisdiction records
    ////

    $sql = "SELECT jur_id from jurisdiction WHERE jur_code = 'CUS'";
    $rs  = $dbh->exec($sql);

    $row    = $rs->next();
    $cus_jur_id = $row['jur_id'];

    // update the club account's balance
    $club_rec_updated = subtract_available_credit($_SESSION['jurisdiction_id'], $initial_deposit);

    // update the dummy custoemr account's balance
    $cus_rec_updated  = add_available_credit($cus_jur_id, $initial_deposit);

    // check that the club and customer dummy jurisdiction records were Sinserted
    if ( FALSE == $club_rec_updated && FALSE == $cus_rec_updated ) {
        $dbh->rollback();
        return false;
    }

    ////
    // log the credit transfer
    ////

    $log_entity_trans = log_credit_transfer ($_SESSION['jurisdiction_id'], $cus_jur_id, $initial_deposit, 'OUT', $info=$id);
    $log_club_trans   = log_credit_transfer ($cus_jur_id, $_SESSION['jurisdiction_id'], $initial_deposit, 'IN', $info=$id);

    // check that the credit_transfer records were inserted

    if ( FALSE == $log_entity_trans &&  FALSE == $log_club_trans ) {
        $dbh->rollback();
        return false;
    }
    $dbh->commit();

    // LOG
    // punter creation log
    $log  = "id:$id|customer_number:$customer_number|username:$username|password:$password|" .
        "member_type:NONFINANCIAL|access:allow|reg_date:" . databaseTimestampToday() . "|";
    "access:allow|registration_status:active|betting_club:" . $_SESSION['jurisdiction_id'];

    doAdminUserLog($_SESSION['admin_id'], 'register_transient_customer', escapeSingleQuotes($log), $id);

    // return user details
    return array('id'=>$id, 'username'=>$username, 'password'=>$password, 'customer_number'=>$customer_number, 'amount'=> $initial_deposit);
}

// generates a unique transient username
function generate_username($club_code) {
    global $dbh;

    // get the username sequence
    $seq_username = $dbh->nextSequence('seq_username');

    // if sequence string length is less than 5, pad with zeros at beginnning
    $len = strlen($seq_username);

    if ($len < 5) {
        $pads_needed = 5 - $len;

        for ( $i=0; $i<$pads_needed; $i++ )
            $pad .= '0';

        $seq_username  = $pad.$seq_username;
    }

    // username is admin users's club code concatenated with the (padded) sequence
    $username = mb_strtolower($club_code) . $seq_username;

    $sql = "select pun_id from punter " .
        "where pun_username = " . $dbh->prepareString($username) . " " .
        "and pun_registration_status != 'activation expired' " .
        "and pun_registration_status != 'email bounced'";

    $exist_count = $dbh->countQuery($sql);

    // if this username already exists, choose another
    if($exist_count > 0)
        $username = generate_username();

    return $username;
}


if($_POST['phase'] == 'generate') {
    globalizeFloat('amount');
    $initial_deposit = $amount;

    // create transient account
    if(checkDetails($_SESSION['jurisdiction_id'],$initial_deposit))
        $auth_array = createTransientAccount($initial_deposit);
}
?>
<?php include ('header.inc'); ?>
<div class=bodyHD>Register a Transient Customer</div>
<P>
    <? showErrors(); ?>
</P>
<?php
if(isset($auth_array)) {
    if(is_array($auth_array)) {
        printInfoMessage("Transient customer account created successfully");
        ?>
        <P>
            <span style="font-weight:bold">Username</span>: <?=$auth_array['username'];?><br>
            <span style="font-weight:bold">Password</span>: <?=$auth_array['password'];?>
        </P>
        <P>
            <span style="font-weight:bold">Amount Deposited</span>: <?=printInDollars($auth_array['amount']);?><br>
            <span style="font-weight:bold">Customer Available Balance</span>: <?=printInDollars($auth_array['amount']);?>
        </P>
    <?php
    }
    else {
        printInfoMessage('There was an error creating this user');
    }
}
else {
    ?>
    <form action="<?=$_SERVER['PHP_SELF']?>" method="POST">
        <input type=hidden name=phase value="generate">
        <table cellpadding=4 cellspacing=1 border=0>
            <tr>
                <td class=label>Amount to Deposit </td>
                <td class=content><input type="text" name="amount" value="<?=replace_quotes($initial_deposit)?>"></td>
            </tr>
            <tr>
                <td></td>
                <td class=content><input type="image" src="<?=image_dir?>/button_next.gif" border=0></td>
            </tr>
        </table>

    </form>
<?php
}
?>
<?php include 'footer.inc';?>
