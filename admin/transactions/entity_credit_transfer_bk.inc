<?php include('header.inc'); ?>
<?check_access('transactions_transfer',$doredirect=true);?>
<?require "account_functions.php";?>

<div class="bodyHD"><?=$lang->getLang("Credit Transfer")?></div>
<script language="javascript">

    function submit_form(form){
        var trans_type_sel = document.getElementById('transfer_type');
        var trans_type = trans_type_sel.options[trans_type_sel.selectedIndex].value;

        if ( trans_type != '' )
            document.forms['phase_1_form'].submit();
    }

    function go_to_phase (form,phase){
        form.phase.value = phase;
        form.submit();
    }
</script>
<?php

function print_sub_entity_select ($entity_id){
    global $dbh,$lang;

    $select_class = "club";
    switch($_SESSION["jurisdiction_class"]){
        case "casino":
            $select_class = "nation";
            break;
        case "nation":
            $select_class = "region";
            break;
        case "region":
            $select_class = "district";
            break;
        case "district":
            $select_class = "club";
            break;
    }

    include "JurisdictionsList.class.inc";
    $jur_list = JurisdictionsList::getInstance($dbh);
    $list     = $jur_list->getChilJurisdictionsIdList($entity_id, $select_class);

    if(count($list) > 0){
        $sql = "SELECT jur_id, jur_name, jur_available_credit FROM jurisdiction WHERE jur_id IN (" . implode(",", $list) . ") ORDER BY LOWER(TRIM(jur_name))";
        $rs = $dbh->exec($sql);
        $num_rows = $rs->getNumRows();
    }

    if ( $num_rows > 0){
        ?>
        <select name="jurisdiction">
            <option value="">- please select -</option>
            <?php
            while($rs->hasNext()){
                $row  = $rs->next();
                $id   = $row['jur_id'];
                $name = $row['jur_name'];
                $available_credit = getInDollars($row['jur_available_credit']);
                $selected = ($_POST['jurisdiction'] == $id) ? 'selected="selected"' :  '';
                ?>
                <option value="<?=$id?>" <?=$selected?>>
                    <?=$name?> [<?=$available_credit?> available credit.]
                </option>
            <?php
            }
            ?>  </select>
    <?php
    }else
        echo '<span style="color:red">'.$lang->getLang("ERROR: subordinate entities not found").'</span>';
}

function get_entity ($jurisdiction){
    global $dbh;

    $sql = "SELECT jur_name, jur_code, jur_available_credit,jur_overdraft,jur_tot_overdraft_received  FROM jurisdiction WHERE jur_id = $jurisdiction";
    $rs  = $dbh->exec($sql);
    $num_rows = $rs->getNumRows();
    if ( $num_rows != 1){
        addError("", "Invalid Jurisdiction"); //invalid jurisdiction
        return FALSE;
    }
    $row = $rs->next();
    return array('ent_name'=>$row['jur_name'], 'ent_available_credit'=>$row['jur_available_credit'], 'ent_available_overdraft'=>$row['jur_overdraft'],'ent_tot_overdraft'=>$row['jur_tot_overdraft_received']);

}
function getAllEntitiesBetween(){
    global $dbh;
    $jurisdictionClass=$_SESSION['jurisdiction_class'];
    $jurisdictionPostClass=$_POST['jurisdiction_level'];

    switch ($jurisdictionClass)
    {
        case 'casino':
            $params =" ca.jur_id as casino,n.jur_id as nation,r.jur_id as region,d.jur_id as district,c.jur_id as club ";
            $from = " FROM jurisdiction ca
                        LEFT JOIN jurisdiction n on ca.jur_id = n.jur_parent_id
                        LEFT JOIN jurisdiction r ON n.jur_id = r.jur_parent_id
                        LEFT JOIN jurisdiction d on r.jur_id = d.jur_parent_id
                        LEFT JOIN jurisdiction c on d .jur_id =c.jur_parent_id  ";
            break;
        case 'nation':
            $params =" n.jur_id as nation,r.jur_id as region,d.jur_id as district,c.jur_id as club ";
            $from = "FROM
                        jurisdiction n
                        LEFT JOIN jurisdiction r ON n.jur_id = r.jur_parent_id
                        LEFT JOIN jurisdiction d on r.jur_id = d.jur_parent_id
                        LEFT JOIN jurisdiction c on d .jur_id =c.jur_parent_id  ";
            break;
        case 'region':
            $params =" r.jur_id as region,d.jur_id as district,c.jur_id as club ";
            $from = " FROM
                        jurisdiction r
                        LEFT JOIN jurisdiction d on r.jur_id = d.jur_parent_id
                        LEFT JOIN jurisdiction c on d .jur_id =c.jur_parent_id  ";
            break;
        case 'district':
            $params =" d.jur_id as district,c.jur_id as club ";
            $from = " FROM  jurisdiction d
                        LEFT JOIN jurisdiction c on d .jur_id =c.jur_parent_id  ";
            break;
        default:
            $params =" c.jur_id as club ";
            $from = " FROM jurisdiction c";
    }

    $params=explode(',',$params);
    switch ($jurisdictionPostClass ){
        case 'nation':
            $params=array_slice($params,0,count($params)-3);
            $where=" Where n.jur_id=".$_POST['jurisdiction'];
            $group=" Group by n.jur_id";
            break;
        case 'region':
            $params=array_slice($params,0,count($params)-2);
            $where=" Where r.jur_id=".$_POST['jurisdiction'];
            $group=" Group by r.jur_id";
            break;
        case 'district':
            $params=array_slice($params,0,count($params)-1);
            $where=" Where d.jur_id=".$_POST['jurisdiction'];
            $group=" Group by d.jur_id";
            break;
        default:
            $where=" Where c.jur_id=".$_POST['jurisdiction'];
            $group=" Group by c.jur_id";

    }
    $params=implode(',',$params);
    $sql ="SELECT $params $from $where $group";
    $result=$dbh->queryRow($sql);
    return $result;

}


/*function getAllEntitiesBetween($sup,$sub){
    global $dbh,$jurisdictionsArray;
    $sql="Select jur_id,jur_parent_id from jurisdiction where jur_id=".$sub." AND jur_parent_id IS NOT NULL";
    $result=$dbh->queryRow($sql);
    if($result['jur_parent_id']==$sup)
    {
        array_push($jurisdictionsArray,$sub);
        array_push($jurisdictionsArray,$sup);
    }
    else
    {
        array_push($jurisdictionsArray,$result['jur_id']);
        getAllEntitiesBetween($sup,$result['jur_parent_id']);
    }
    return  $jurisdictionsArray;
}*/

function do_credit_send($super_ent_id, $sub_ent_id, $sup_amount,$sup_overdraft,$sup_tot_overdraft,$sub_amount,$sub_overdraft,$sub_tot_overdraft, $aus_id){
    global $dbh;
    $super_credit_updated = update_jurisdiction_credits($super_ent_id, $sup_amount,$sup_overdraft,$sup_tot_overdraft);
    $sub_credit_updated   = update_jurisdiction_credits($sub_ent_id, $sub_amount,$sub_overdraft,$sub_tot_overdraft);
    if ( ! ( TRUE == $super_credit_updated &&  TRUE == $sub_credit_updated) ){
        $dbh->rollback();
        return false;
    }
    return TRUE;
}

function do_credit_return($super_ent_id, $sub_ent_id, $sup_amount,$sup_overdraft,$sup_tot_overdraft,$sub_amount,$sub_overdraft,$sub_tot_overdraft, $aus_id){
    global $dbh;

    $dbh->begin();

    $super_credit_updated = update_jurisdiction_credits($super_ent_id, $sup_amount,$sup_overdraft,$sup_tot_overdraft);
    $sub_credit_updated   = update_jurisdiction_credits($sub_ent_id, $sub_amount,$sub_overdraft,$sub_tot_overdraft);

    if ( ! ( TRUE == $super_credit_updated &&  TRUE == $sub_credit_updated) ){
        $dbh->rollback();
        return false;
    }

    $dbh->commit();
    return TRUE;
}

function adminUserCreditLog($note='',$transactionType,$admId,$admJurId,$admFinalJurId,$admAmount,$mult=0)
{
    global $dbh,$trnsId;
    $sql="Insert into admin_user_transaction (att_note,
                                                att_tty_id,
                                                att_aus_id,
                                                att_transaction_id,
                                                att_jur_id,
                                                att_final_jur_id,
                                                att_jur_avail_credit,
                                                att_amount,
                                                att_jur_final_avail_credit
                                                )
                                        Values (
                                                ".$dbh->prepareString($note).",
                                                ".$dbh->prepareString($transactionType).",
                                                ".$dbh->prepareString($admId).",
                                                ".$dbh->prepareString($trnsId).",
                                                ".$dbh->prepareString($admJurId).",
                                                ".$dbh->prepareString($admFinalJurId).",
                                                (SELECT jur_available_credit from jurisdiction where jur_id =".$dbh->prepareString($admJurId).")";

                                        if($mult!=0 && ($transactionType==19 || $transactionType==21)){
                                            $sql.="+".$dbh->prepareString($admAmount);
                                            }
                                         $sql.=",".$dbh->prepareString($admAmount).",
                                                (SELECT jur_available_credit from jurisdiction where jur_id =".$dbh->prepareString($admFinalJurId).")";
                                          if($mult!=0 && $transactionType==20){
                                              $sql.="-".$dbh->prepareString($admAmount);
                                          }
                                        $sql.=")";
    $result=$dbh->exec($sql);
    return  $result;
}
// get label for subordinate jurisdiction
switch ( $_SESSION['jurisdiction_class'] ){
    case 'district':
        if ( 'take_credits' == $_POST['transfer_type'] )
            $label = "";  // entity receiving credits
        else
            $label = "Jurisdiction";   // entity returning credits
        $missing_sub_msg = $lang->getLang("A jurisdiction must be selected");
        break;

    case 'region':
        if ( 'take_credits' == $_POST['transfer_type'] )
            $label = "Jurisdiction";  // entity receiving credits
        else
            $label = "Jurisdiction";   // entity returning credits
        $missing_sub_msg = $lang->getLang("A jurisdiction must be selected");
        break;

    case 'casino':
        if ( 'take_credits' == $_POST['transfer_type'] )
            $label = "";  // entity receiving credits national
        else
            $label = "Jurisdiction";   // entity returning credits national
        $missing_sub_msg = $lang->getLang("A jurisdiction must be selected");
        break;

    case 'nation':
        if ( 'take_credits' == $_POST['transfer_type'] )
            $label = "Jurisdiction";  // entity receiving credits
        else
            $label = "Jurisdiction";   // entity returning credits
        $missing_sub_msg = $lang->getLang("A jurisdiction must be selected");
        break;
    default:
        die("Invalid access");
}

//get account details for logged in  admin user (the seller of the credit)

$sql = "SELECT jur_id, jur_code, jur_code, jur_available_credit,jur_overdraft,jur_tot_overdraft_received FROM jurisdiction " .
    " WHERE jur_id = " . $_SESSION['jurisdiction_id'];
$rs       = $dbh->exec($sql);
$num_rows = $rs->getNumRows();

if ( 1 == $num_rows ){
    $row = $rs->next();
    $super_entity_id            = $_SESSION['jurisdiction_id'];
    $super_acc_code             = $row['jur_code'];
    $super_available_credit     = $row['jur_available_credit'];
    $super_available_overdraft  = $row['jur_overdraft'];
    $super_total_overdraft      = $row['jur_tot_overdraft_received'];
}
else{
    die($lang->getLang("Cannot find your account record"));
}

if ( 'process' == $_POST['phase'] ){

    // validate sale amount
    $amount_valid = TRUE;
    if ( isBlank($_POST['amount']) ){
        addError("", $lang->getLang("A collection amount must be entered")); //invalid jurisdiction
        $amount_valid = FALSE;
    }elseif ( ! isValidMoney($_POST['amount'])  ){
        addError("", $lang->getLang("Invalid collection amount")); //invalid jurisdiction
        $amount_valid = FALSE;
    }
    if ( 'send_credits' != $_POST['transfer_type'] && 'take_credits' != $_POST['transfer_type'] &&
    'send_overdraft' != $_POST['transfer_type'] && 'take_overdraft' != $_POST['transfer_type']
    ){
        addError("", $lang->getLang("Invalid  or missing transfer type"));
    }
    // validate buyer entity
    if ( empty($_POST['jurisdiction']) || ! isInteger($_POST['jurisdiction']) ){
        addError("", $missing_sub_msg); // buyer entity not selected
    }else{
        $sub_arr = get_entity($_POST['jurisdiction']);
        if ( empty($sub_arr) )
            addError("", $missing_sub_msg); //invalid entity
        else{
            $jurisdiction           = $_POST['jurisdiction'];
            $sub_entity_name         = $sub_arr['ent_name'];
            $sub_entity_avail_credit = $sub_arr['ent_available_credit'];
            $sub_entity_avail_overdraft = $sub_arr['ent_available_overdraft'];
            $sub_entity_total_overdraft = $sub_arr['ent_tot_overdraft'];
            $sub_avail_credit_fmt    = getInDollars($sub_entity_avail_credit);
            if ( TRUE == $amount_valid ){
                // check if seller (entity current user an administrator of)
                if ( 'send_credits' == $_POST['transfer_type'] ){
                    // if superior is not the casino, and the avail credit is less than the credit transfer amount
                    if (  $super_entity_id != 1 && $_POST['amount'] > ($super_available_credit+$super_available_overdraft) && $_POST['amount'] > ($super_available_credit) && $_POST['amount'] > ($super_available_overdraft)){
                        addError("", $lang->getLang("Amount to high. You have insufficient available credit.")); //invalid jurisdiction
                        $amount_valid = FALSE;
                    }
                }
                elseif  ( 'take_credits' == $_POST['transfer_type'] ){
                    if (  $_POST['amount'] > ($sub_entity_avail_credit) ){
                        addError("", $lang->getLang("Amount to high. You have insufficient available credit.")); //invalid jurisdiction
                        $amount_valid = FALSE;
                    }
                }
            }
        }
    }

    if ( areErrors() )      // show original form if there are errors
    $show_form = 'entry';
    else
    {
        $dbh->begin();
        // perform transaction
        $jurisdictionsArray=array();
        $alljurisdictions=getAllEntitiesBetween();
        $trnsId= 'ADM'.$dbh->nextSequence('CTR_TRAN_NUM_SEQ').rand_str(4);
        if ( 'send_credits' == $_POST['transfer_type'] ){
            $transfer_type=19;
        }
        else{
            $transfer_type=20;
            $alljurisdictions=array_reverse($alljurisdictions);
        }
        $keys = array_keys($alljurisdictions);
        for ($i = 0; $i < count($keys); $i++) {
            $cur = $alljurisdictions[$keys[$i]];
            $next = $alljurisdictions[$keys[$i+1]];
            if($next!=''){
                if ( 'send_credits' == $_POST['transfer_type'] )
                {
                    if(!is_int(adminUserCreditLog($_POST['note'],$transfer_type,$_SESSION['admin_id'],$cur,$next,$_POST['amount'],$i))){
                        addError('','An error has occurred when adding the transaction record');
                    };
                }
                else
                {
                    if(!is_int(adminUserCreditLog($_POST['note'],$transfer_type,$_SESSION['admin_id'],$next,$cur,-$_POST['amount'],$i))){
                        addError('','An error has occurred when adding the transaction record');
                    };
                }
            }
        }
        if(areErrors()){
            $dbh->rollback();
            $show_form = 'entry';
        }
        else
        {  $starttime = microtime(true);
            if ( 'send_credits' == $_POST['transfer_type'] )
            {
                $amount=$_POST['amount'];
                    if($super_available_credit<$amount){
                        if($super_available_credit<0){
                            $super_available_overdraft=($super_available_overdraft-$amount);
                        }else{
                            $super_available_overdraft=($super_available_overdraft+($super_available_credit-$amount));
                        }
                            $super_available_credit= ($super_available_credit-$amount);
                    }
                else{
                    $super_available_credit= $super_available_credit-$amount;
                }
                $sub_entity_avail_credit +=$amount;
                $was_sent = do_credit_send ($super_entity_id, $jurisdiction, $super_available_credit,$super_available_overdraft,$super_total_overdraft,$sub_entity_avail_credit,$sub_entity_avail_overdraft,$sub_entity_total_overdraft, $_SESSION['admin_id']);
            }
            elseif ( 'take_credits' == $_POST['transfer_type'] )
            {
                $amount=$_POST['amount'];
                $sub_entity_avail_credit= $sub_entity_avail_credit-$amount;
                $super_available_credit +=$amount;
                if($super_available_overdraft!=$super_total_overdraft){
                    if(($super_available_overdraft+$amount) > $super_total_overdraft){
                        $super_available_overdraft=$super_total_overdraft;
                    }
                    else{
                        $super_available_overdraft += $amount;
                    }
                }
                $was_sent = do_credit_send ($super_entity_id, $jurisdiction, $super_available_credit,$super_available_overdraft,$super_total_overdraft,$sub_entity_avail_credit,$sub_entity_avail_overdraft,$sub_entity_total_overdraft, $_SESSION['admin_id']);
            }
                if ( $was_sent ) { // transaction successful
                $dbh->commit();
                $show_form = 'completed';
                $endtime = microtime(true);
                $duration = $endtime - $starttime;
                $duration=number_format($duration, 4, ',', '')."s";
            }else{
                $dbh->rollback();
                addError("", $lang->getLang("Database Error"));
                $show_form = 'entry';
            }
        }
    }
}
elseif ( 'confirm' == $_POST['phase'] ){
    // validate sale amount
    $amount_valid = TRUE;
    if ( isBlank($_POST['amount']) ){
        addError("", $lang->getLang("A collection amount must be entered")); //invalid jurisdiction
        $amount_valid = FALSE;
    }elseif ( ! isValidMoney($_POST['amount'])  ){
        addError("", $lang-getLang("Invalid collection amount")); //invalid jurisdiction
        $amount_valid = FALSE;
    }

    if ( 'send_credits' != $_POST['transfer_type'] && 'take_credits' != $_POST['transfer_type'] &&
        'send_overdraft' != $_POST['transfer_type'] && 'take_overdraft' != $_POST['transfer_type'] ){
        addError("", $lang->getLang("Invalid  or missing transfer type"));
    }
    // validate buyer entity id, and get account details of buyer
    if ( empty($_POST['jurisdiction']) || ! isInteger($_POST['jurisdiction']) ){
        addError("", $missing_sub_msg); // buyer entity not selected
    }else{
        $sub_arr = get_entity($_POST['jurisdiction']);
        if ( empty($sub_arr) )
            addError("", $missing_sub_msg); //invalid entity
        else{
            $sub_entity_name         = $sub_arr['ent_name'];
            $sub_entity_avail_credit = $sub_arr['ent_available_credit'];
            $sub_avail_credit_fmt      = getInDollars($sub_entity_avail_credit);

            if ( TRUE == $amount_valid ){
                // check if seller (entity current user an administrator of)
                if ( 'send_credits' == $_POST['transfer_type'] ){
                    // if superior is not the casino, and the avail credit is less than the credit transfer amount
                    if (  $super_entity_id != 1 && $_POST['amount'] > ($super_available_credit+$super_available_overdraft) && $_POST['amount'] > ($super_available_credit) && $_POST['amount'] > ($super_available_overdraft)){
                        addError("", $lang->getLang("Amount to high. You have insufficient available credit.")); //invalid jurisdiction
                        $amount_valid = FALSE;
                    }
                }elseif  ( 'take_credits' == $_POST['transfer_type'] ){
                    if (  $_POST['amount'] > $sub_entity_avail_credit ){
                        addError("", $lang->getLang("Amount to high. You have insufficient available credit.")); //invalid jurisdiction
                        $amount_valid = FALSE;
                    }
                }
            }
        }
    }
    if ( areErrors() )      // show default form if there are errors
    $show_form = 'entry';
    else
        $show_form = 'confirm';  //show confirmation form

}
elseif ('entry' == $_POST['phase'] ){
    if ( 'send_credits' != $_POST['transfer_type'] && 'take_credits' != $_POST['transfer_type'] &&
        'send_overdraft' != $_POST['transfer_type'] && 'take_overdraft' != $_POST['transfer_type'] ){
        addError("", "Invalid  or missing transfer type");
        unset($show_form);
    }else{
        $transfer_type = $_POST['transfer_type'];
        $show_form = 'entry';
    }
}

if ( 'completed' == $show_form ){
    ?>      <div class=bodyHD2><?=$lang->getLang("Transaction Successful")?></div><br />
    <div class="tip fleft"><?=$lang->getLang("Time took to execute your request")?>: <?=$duration?></div>
    <br />
    <?php
    $time = date('d/m/Y H:i:s');
    printInfoMessage($lang->getLang("Transaction Completed at"). $time);
    ?>
    <br />
    <table cellpadding=4 cellspacing=1 border=0>
        <tr>
            <td class=label><?=$lang->getLang("Transfer Type")?></td>
            <td class="content">
                <?php
                if ( 'send_credits' == $_POST['transfer_type'] )
                    echo "Extend credit";
                elseif ( 'take_credits' == $_POST['transfer_type'] )
                    echo "Revoke credit";
                elseif ( 'send_overdraft' == $_POST['transfer_type'] )
                    echo "Extend overdraft";
                elseif ( 'take_overdraft' == $_POST['transfer_type'] )
                    echo "Revoke overdraft";
                ?>
            </td>
        </tr>
        <tr>
            <td class=label><?=$lang->getLang("Your available credit")?></td>
            <td class="content"><?php if ($_SESSION["jurisdiction_id"] == 1 ) echo "Unlimited"; else echo getInDollars($super_available_credit);?></td>
        </tr>
        <tr>
            <td class=label><?=$lang->getLang("Your available overdraft")?></td>
            <td class="content"><?php if ($_SESSION["jurisdiction_id"] == 1 ) echo "Unlimited"; else echo getInDollars($super_available_overdraft);?></td>
        </tr>

        <tr>
            <td class=label><?=$label;?></td>
            <td class="content"><?=$sub_entity_name;?> [<?=getInDollars($sub_entity_avail_credit)?> available credit.]</td>
        </tr>
        <tr>
            <td class=label><?=$lang->getLang("Credit Amount")?></td>
            <td class="content"><?=getInDollars($_POST['amount'])?></td>
        </tr>
    </table>
    <br><br>
    <a href="<?=$_SERVER['PHP_SELF'];?>"><?=$lang->getLang("Perform another credit transfer")?></a>
<?  }elseif ( 'confirm' == $show_form ){  ?>
    <div class=bodyHD2><?=$lang->getLang("Confirm Details")?></div>
    <br>
    <?showErrors()?>
    <P>
    <form action="<?=$_SERVER['PHP_SELF']?>" method="POST">
        <input type="hidden" name="amount" value="<?=$_POST['amount'];?>">
        <input type="hidden" name="phase" value="process">
        <input type="hidden" name="transfer_type" value="<?=$_POST['transfer_type']?>">
        <input type="hidden" name="jurisdiction_level" value="<?=$_POST['jurisdiction_level']?>">
        <input type="hidden" name="jurisdiction" value="<?=$_POST['jurisdiction'];?>">
        <input type="hidden" name="note" value="<?=$_POST['note'];?>">
        <table cellpadding=4 cellspacing=1 border=0>
            <tr>
                <td class=label><?=$lang->getLang("Transfer Type")?></td>
                <td class="content">

                    <?php
                    if ( 'send_credits' == $_POST['transfer_type'] )
                        echo "Extend credit";
                    elseif ( 'take_credits' == $_POST['transfer_type'] )
                        echo "Revoke credit";
                    elseif ( 'send_overdraft' == $_POST['transfer_type'] )
                        echo "Extend overdraft";
                    elseif ( 'take_overdraft' == $_POST['transfer_type'] )
                        echo "Revoke overdraft";
                    ?>
                </td>
            </tr>
            <tr>
                <td class=label><?=$lang->getLang("Your available credit")?></td>
                <td class="content"><?php if ($_SESSION["jurisdiction_id"] == 1 ) echo "Unlimited"; else echo getInDollars($super_available_credit);?></td>
            </tr>
            <tr>
                <td class=label><?=$lang->getLang("Your available overdraft")?></td>
                <td class="content"><?php if ($_SESSION["jurisdiction_id"] == 1 ) echo "Unlimited"; else echo getInDollars($super_available_overdraft);?></td>
            </tr>
            <tr>
                <td class=label><?=$label;?></td>
                <td class="content"><?=$sub_entity_name?>[<?=getInDollars($sub_entity_avail_credit)?> available credit.]</td>
            </tr>
            <tr>
                <td class=label><?=$lang->getLang("Credit Amount")?></td>
                <td class="content"><?=getInDollars($_POST['amount'])?></td>
            </tr>
            <tr>
                <td></td>
                <td><input type="button" value="&lt; Back"  onClick="go_to_phase(this.form,'entry')"> <input type="submit" name="process" value="Submit"></td>
            </tr>
        </table>
    </form>

<? } elseif ( 'entry' == $show_form ){?>

    <div class=bodyHD2><?$lang->getLang("Enter Details")?></div>
    <br />
    <?showErrors()?>
    <P>
    <form action="<?=$_SERVER['PHP_SELF']?>" method="POST">
        <input type="hidden" name="phase" value="confirm">
        <input type="hidden" name="transfer_type" value="<?=$_POST['transfer_type']?>">
        <table id="transfer_form" cellpadding=4 cellspacing=1 border=0>
            <tr>
                <td class=label><?=$lang->getLang("Transfer Type")?></td>
                <td class="content">
                    <?php
                    if ( 'send_credits' == $_POST['transfer_type'] )
                        echo "Extend credit";
                    elseif ( 'take_credits' == $_POST['transfer_type'] )
                        echo "Revoke credit";
                    elseif ( 'send_overdraft' == $_POST['transfer_type'] )
                        echo "Extend overdraft";
                    elseif ( 'take_overdraft' == $_POST['transfer_type'] )
                        echo "Revoke overdraft";
                    ?>
                </td>
            </tr>
            <tr>
                <td class=label><?=$lang->getLang("Your available credit")?></td>
                <td class="content"><?php if ($_SESSION["jurisdiction_id"] == 1 ) echo "Unlimited"; else echo getInDollars($super_available_credit);?></td>
            </tr>
            <tr>
                <td class=label><?=$lang->getLang("Your available overdraft")?></td>
                <td class="content"><?php if ($_SESSION["jurisdiction_id"] == 1 ) echo "Unlimited"; else echo getInDollars($super_available_overdraft);?></td>
            </tr>
            <?php

            include('jurisdiction_filters.php');
            include('jurisdiction_filter_tabrow.php');
            ?>
            <tr>
                <td class=label><?=$lang->getLang("Credit Amount")?></td>
                <td class="content"><input type="text" name="amount" value="<?=$_POST['amount']?>" size="10"></td>
            </tr>
            <tr>
                <td class=label><?=$lang->getLang("Note")?></td>
                <td class="content"><textarea  name="note"> <?=$_POST['note']?> </textarea></td>
            </tr>

            <tr>
                <td></td>
                <td>
                    <input type="button" name="change" value="Back" onClick="go_to_phase(this.form,'')" />
                    <input type="submit" name="confirm" value="Next" border=0 />
                </td>
            </tr>

        </table>
    </form>

<?}elseif ( ! $show_form ){?>

    <?showErrors();?>

    <P>
    <form name="phase_1_form" action="<?=$_SERVER['PHP_SELF']?>" method="POST">
        <input type="hidden" name="phase" value="entry">
        <table  cellpadding=4 cellspacing=1 border=0>
            <tr>
                <td class=label><?=$lang->getLang("Your available credit")?></td>
                <td class="content"><?php if ( $_SESSION["jurisdiction_id"] == 1 ) echo "Unlimited"; else echo getInDollars($super_available_credit);?></td>
            </tr>
            <tr>
                <td class=label><?=$lang->getLang("Your available overdraft")?></td>
                <td class="content"><?php if ($_SESSION["jurisdiction_id"] == 1 ) echo "Unlimited"; else echo getInDollars($super_available_overdraft);?></td>
            </tr>
            <tr>
                <td class=label><?=$lang->getLang("Transfer Type")?></td>
                <td class="content">
                    <select name="transfer_type" id="transfer_type">
                        <option value="">- please select -</option>
                        <option value="send_credits"<?if ('send_credits' == $_POST['transfer_type'] ) echo ' selected="selected"'?>><?=$lang->getLang("Extend credit")?></option>
                        <option value="take_credits"<?if ('take_credits' == $_POST['transfer_type'] ) echo ' selected="selected"'?>><?=$lang->getLang("Revoke credit")?></option>
                        <option value="send_overdraft"<?if ('send_overdraft' == $_POST['transfer_type'] ) echo ' selected="selected"'?>><?=$lang->getLang("Extend overdraft")?></option>
                        <option value="take_overdraft"<?if ('take_overdraft' == $_POST['transfer_type'] ) echo ' selected="selected"'?>><?=$lang->getLang("Revoke overdraft")?></option>
                    </select>
                </td>
            </tr>
            <tr>
                <td></td>
                <td>
                    <input type="submit" name="confirm" value="Next &gt;">
                </td>
            </tr>
        </table>
    </form>
<?
}
?>
<?include 'footer.inc';?>
