<?check_access('transactions_transfer',$doredirect=true);
include('header.inc');
global $lang;
$transferToAll=check_access('admin_transfer_to_sub_level');
require "account_functions.php";?>
<style>
    #td_jurisdiction td {
        vertical-align: top;
    }
    #jurisdiction_select{
        float:left;
    }
    #jurisdiction_info{
        float:right;
    }
</style>
<div class="bodyHD"><?=$lang->getLang("Entity Credit Transfer")?></div>
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
    $(function() {
        <?if($transferToAll && (('send_credits' == $_POST['transfer_type'] ||  'take_credits' == $_POST['transfer_type'])||(strtolower($_SESSION['aty_code'])=='casinomanager'))){ ?>
        $("<div id='jurisdiction_info'></div>").insertAfter("#jurisdiction_select");
        var jur_id= $("select[name='jurisdiction']").find('option:selected').val();
        if(jur_id!='' && typeof jur_id !== 'undefined'){
        $.post("/services/jurisdiction_services.inc", {"action":5,"jur_id":jur_id}, function(data){
            $('#jurisdiction_info').empty().append(data);
        });
        }
        $(document.body).on('change', "select[name='jurisdiction'], #jurisdiction_level" ,function() {
            var jur_id= $("select[name='jurisdiction']").find('option:selected').val();
            $.post("/services/jurisdiction_services.inc", {"action":5,"jur_id":jur_id}, function(data){
                $('#jurisdiction_info').empty().append(data);
            });
        });
        <? } else{ ?>

        $('#jurisdiction').change(function() {
            $('.hidden').hide();
            $('#' + $(this).find('option:selected').val()+'Info').show();
        });
        <? } ?>
    });

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
        if($_SESSION['jurisdiction_class']=='district'){
            $sql="
                SELECT Distinct jur_id, jur_name,jur_available_credit,jur_overdraft,jur_overdraft_start_time,jur_tot_overdraft_received, cur_code_for_web
                FROM  jurisdiction
                JOIN currency on jur_currency=cur_id
                WHERE jur_parent_id = " . $_SESSION['jurisdiction_id']."
                AND  jur_id not in (
                                select aus_jurisdiction_id from affiliate, admin_user
                                where aff_aus_id = aus_id
                            )";
        }
        else{
            $sql = "SELECT jur_id, jur_name, jur_available_credit,jur_overdraft,jur_overdraft_start_time,jur_tot_overdraft_received,cur_code_for_web FROM jurisdiction
            JOIN currency on cur_id=jur_currency WHERE jur_id IN (" . implode(",", $list) . ") ORDER BY LOWER(TRIM(jur_name))";
        }
        $rs = $dbh->exec($sql,false,true);
        $num_rows = $rs->getNumRows();
    }


    if ( $num_rows > 0){
        $summary='';
        ?>
        <select name="jurisdiction" id='jurisdiction' style="float:left">
            <option value="">- <?=$lang->getLang("Please select")?> -</option>
            <?php
            while($rs->hasNext()){
                $row  = $rs->next();
                $id   = $row['jur_id'];
                $name = $row['jur_name'];

                $available_credit = getInDollars($row['jur_available_credit'],2,$row['cur_code_for_web']);
                $selected = ($_POST['jurisdiction'] == $id) ? 'selected="selected"' :  '';
                $summary.="<table id='".$id."Info' class='hidden' style='padding-left: 200px;'>
                    <tr><td class='label'>Credit</td><td>".$available_credit."</td></tr>
                    <tr><td class='label'>Overdraft</td><td>".getInDollars($row['jur_overdraft'],2,$row['cur_code_for_web'])."</td></tr>
                    <tr><td class='label'>Total Overdraft Received</td><td class='unwrappable'>".getInDollars($row['jur_tot_overdraft_received'],2,$row['cur_code_for_web'])."</td></tr>
                <tr><td class='label'>Overdraft start time</td><td>";
                $summary.=($row['jur_overdraft_start_time']=='') ? 'none': $row['jur_overdraft_start_time'];
                $summary.=" </td></tr></table>";
                ?>
                <option value="<?=$id?>" <?=$selected?>>
                    <?=$name?>
                </option>
            <?php
            }
            ?>  </select>
        <?=$summary?>

    <?php
    }else
        echo '<span style="color:red">'.$lang->getLang("ERROR: subordinate entities not found").'</span>';
}

function get_entity ($jurisdiction){
    global $dbh;
    $sql = "SELECT jur_name, jur_code, jur_available_credit,jur_overdraft,jur_tot_overdraft_received,jur_overdraft_start_time,cur_code_for_web
    FROM jurisdiction JOIN currency on jur_currency=cur_id WHERE jur_id = $jurisdiction";
    $rs  = $dbh->exec($sql,false,true);
    $num_rows = $rs->getNumRows();
    if ( $num_rows != 1){
        addError("", "Invalid Jurisdiction"); //invalid jurisdiction
        return FALSE;
    }
    $row = $rs->next();
    return array('ent_name'=>$row['jur_name'], 'ent_available_credit'=>$row['jur_available_credit'], 'ent_available_overdraft'=>$row['jur_overdraft'],'ent_tot_overdraft'=>$row['jur_tot_overdraft_received'],'ent_overdraft_start'=>$row['jur_overdraft_start_time'],'cur_code_for_web'=>$row['cur_code_for_web']);

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

function do_credit_send($super_ent_id, $sub_ent_id, $sup_amount,$sup_overdraft,$sup_tot_overdraft,$supresetov=0,$sub_amount,$sub_overdraft,$sub_tot_overdraft,$subresetov=0, $aus_id){
    global $dbh;
    $super_credit_updated = update_jurisdiction_credits($super_ent_id, $sup_amount,$sup_overdraft,$sup_tot_overdraft,$supresetov);
    $sub_credit_updated   = update_jurisdiction_credits($sub_ent_id, $sub_amount,$sub_overdraft,$sub_tot_overdraft,$subresetov);

    if ( ! ( TRUE == $super_credit_updated &&  TRUE == $sub_credit_updated) ){
        $dbh->rollback();
        return false;
    }
    return TRUE;
}





function adminUserCreditLog($note='',$transactionType,$admId,$admJurId,$admFinalJurId,$amount,$overdraft,$admCredit,$admOverdraft,$admFinalCredit,$admFinalOverdraft)
{
    global $dbh,$trnsId;
    $sql="Insert into admin_user_transaction (att_note,
                                                att_tty_id,
                                                att_aus_id,
                                                att_transaction_id,
                                                att_jur_id,
                                                att_final_jur_id,
                                                att_amount,
                                                att_overdraft,
                                                att_jur_avail_credit,
                                                att_jur_avail_overdraft,
                                                att_jur_final_avail_credit,
                                                att_jur_final_avail_overdraft)
                                        Values (
                                                ".$dbh->prepareString($note).",
                                                ".$dbh->prepareString($transactionType).",
                                                ".$dbh->prepareString($admId).",
                                                ".$dbh->prepareString($trnsId).",
                                                ".$dbh->prepareString($admJurId).",
                                                ".$dbh->prepareString($admFinalJurId).",
                                                ".$dbh->prepareString($amount).",
                                                ".$dbh->prepareString($overdraft).",
                                                ".$dbh->prepareString($admCredit).",
                                                ".$dbh->prepareString($admOverdraft).",
                                                ".$dbh->prepareString($admFinalCredit).",
                                                ".$dbh->prepareString($admFinalOverdraft);
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
$_POST['amount']=str_replace(',',".",$_POST['amount']);
//get account details for logged in  admin user (the seller of the credit)

$sql = "SELECT jur_id, jur_code, jur_code, jur_available_credit,jur_overdraft,jur_tot_overdraft_received,jur_overdraft_start_time,cur_code_for_web FROM jurisdiction  join currency on jur_currency=cur_id" .
    " WHERE jur_id = " . $_SESSION['jurisdiction_id'];
$rs       = $dbh->exec($sql,false,true);
$num_rows = $rs->getNumRows();

if ( 1 == $num_rows ){
    $row = $rs->next();
    $super_entity_id            = $_SESSION['jurisdiction_id'];
    $super_acc_code             = $row['jur_code'];
    $super_available_credit     = $row['jur_available_credit'];
    $super_available_overdraft  = $row['jur_overdraft'];
    $super_total_overdraft      = $row['jur_tot_overdraft_received'];
    $super_overdraft_start      = $row['jur_overdraft_start_time'];
    $superCurrency              = $row["cur_code_for_web"]; }
else{
    die($lang->getLang("Cannot find your account record"));
}

if ( 'process' == $_POST['phase'] ){

    // validate sale amount


    $amount_valid = TRUE;
    if ( isBlank($_POST['amount']) ){
        if($_POST['transfer_type']=='reset_overdraft'){
            if(($_POST['ovReset']!='on') && $_POST['ovday']==''){
                addError("", $lang->getLang("Choose remove overdraft time or add a new start day!")); //invalid jurisdiction
            }
        }
        else{
            addError("", $lang->getLang("A collection amount must be entered")); //invalid jurisdiction
            $amount_valid = FALSE;
        }
    }elseif ( ! isValidMoney($_POST['amount'])  ){
        addError("", $lang->getLang("Invalid collection amount")); //invalid jurisdiction
        $amount_valid = FALSE;
    }
    if ( 'send_credits' != $_POST['transfer_type'] && 'take_credits' != $_POST['transfer_type'] &&
        'send_overdraft' != $_POST['transfer_type'] && 'take_overdraft' != $_POST['transfer_type'] && 'reset_overdraft' != $_POST['transfer_type']
    ){
        addError("", $lang->getLang("Invalid or missing transfer type"));
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
            $sub_entity_overdraft_start = $sub_arr['ent_overdraft_start'];
            $sub_avail_credit_fmt    = getInDollars($sub_entity_avail_credit,2,$sub_arr["cur_code_for_web"]);
            $sub_entity_currency =$sub_arr["cur_code_for_web"];
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
                elseif  ( 'send_overdraft' == $_POST['transfer_type'] ){

                    if ( $super_entity_id != 1 && ( $_POST['amount'] > ($super_available_overdraft) )){

                        addError("", $lang->getLang("Amount to high. You have insufficient available overdraft.")); //invalid jurisdiction
                        $amount_valid = FALSE;
                    }
                }
                elseif  ( 'take_overdraft' == $_POST['transfer_type'] ){
                    if (  $_POST['amount'] > ($sub_entity_avail_overdraft) ){

                        addError("", $lang->getLang("Amount to high. The sub entity doesn't have insufficient available overdraft.")); //invalid jurisdiction
                        $amount_valid = FALSE;
                    }
                    if ( $super_entity_id != 1 &&($_POST['amount'] > ($super_total_overdraft-$super_available_overdraft)) ){
                        addError("", $lang->getLang("Amount exceeds your overdraft received.")); //invalid jurisdiction
                        $amount_valid = FALSE;
                    }
                }
                elseif  ( 'reset_overdraft' == $_POST['transfer_type'] ){
                    if(($_POST['ovReset']!='on') && $_POST['ovday']==''){
                        addError("", $lang->getLang("Choose remove overdraft time or add a ne start day!")); //invalid jurisdiction
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

        $trnsId= 'ADM'.$dbh->nextSequence('CTR_TRAN_NUM_SEQ').rand_str(4);

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
        elseif  ( 'send_overdraft' == $_POST['transfer_type'] ){

            if ( $super_entity_id != 1 && ( $_POST['amount'] > ($super_available_overdraft) )){
                addError("", $lang->getLang("Amount to high. You have insufficient available overdraft.")); //invalid jurisdiction
                $amount_valid = FALSE;
            }
        }
        elseif  ( 'take_overdraft' == $_POST['transfer_type'] ){
            if (  $_POST['amount'] > ($sub_entity_avail_overdraft) ){

                addError("", $lang->getLang("Amount to high. The sub entity doesn't have insufficient available overdraft.")); //invalid jurisdiction
                $amount_valid = FALSE;
            }
            if ( $super_entity_id != 1 &&($_POST['amount'] > ($super_total_overdraft-$super_available_overdraft)) ){
                addError("", $lang->getLang("Amount exceeds your overdraft received.")); //invalid jurisdiction
                $amount_valid = FALSE;
            }
        }
        elseif  ( 'reset_overdraft' == $_POST['transfer_type'] ){
            if(($_POST['ovReset']!='on') && $_POST['ovday']==''){
                addError("", $lang->getLang("Choose remove overdraft time or add a ne start day!")); //invalid jurisdiction
            }
        }

        if(areErrors()){
            $dbh->rollback();
            $show_form = 'entry';
        }
        else
        {  $starttime = microtime(true);

            $jursdictionClass=$_SESSION['jurisdiction_class'];
            $jurisdictionPostClass=$_POST['jurisdiction_level'];
            $jurisdictionsArray=array();
            $alljurisdictions=getAllEntitiesBetween($jursdictionClass,$jurisdictionPostClass,$_POST['jurisdiction']);
            $amount=$_POST['amount'];


            if ( 'send_credits' == $_POST['transfer_type'] )
            {
                $supresetov=0;
                if((strtotime('now')-strtotime($super_overdraft_start)> (3600*24*7)) && $_SESSION['jurisdiction_class']!='casino' && $super_available_credit<=0 && $super_overdraft_start!=''){
                    $dbh->rollback();
                    addError("", $lang->getLang("Your overdraft is expired"));
                    $show_form = 'entry';
                }else{
                    $transfer_type=19;
                    $keys = array_keys($alljurisdictions);
                    for ($i = 0; $i < count($keys); $i++) {
                        $cur = $alljurisdictions[$keys[$i]];
                        $next = $alljurisdictions[$keys[$i+1]];
                        if($next!=''){
                            if(!is_int(adminUserCreditLogMultiple(
                                    $_POST['note'],
                                    $transfer_type,
                                    $_SESSION['admin_id'],
                                    $cur,
                                    $next,
                                    $_POST['amount'],
                                    0,
                                    $i)
                            )){
                                addError('','An error has occurred when adding the transaction record');
                            };
                        }
                    }

                    if($super_available_credit<$amount){
                        if($super_available_credit<0){
                            $super_available_overdraft=($super_available_overdraft-$amount);

                        }
                        else{
                            if((strtotime('now')-strtotime($super_overdraft_start)> (3600*24*7)) && $_SESSION['jurisdiction_class']!='casino'){
                                $supresetov=1;
                            }
                            $super_available_overdraft=($super_available_overdraft+($super_available_credit-$amount));
                        }
                        $super_available_credit= ($super_available_credit-$amount);
                    }
                    else{
                        $super_available_credit= $super_available_credit-$amount;
                    }
                    if($sub_entity_avail_credit<0){
                        $max_overdraft=$sub_entity_total_overdraft-$sub_entity_avail_overdraft;
                        if($max_overdraft>0){
                            if($sub_entity_avail_credit+$amount<0){
                                if($max_overdraft>$amount){
                                    $sub_entity_avail_overdraft+=$amount;
                                }
                                else{
                                    $sub_entity_avail_overdraft+=$max_overdraft;
                                }
                            }
                            else{
                                if($max_overdraft>(-$sub_entity_avail_credit)){
                                    $sub_entity_avail_overdraft+=(-$sub_entity_avail_credit);
                                }
                                else{
                                    $sub_entity_avail_overdraft+=$max_overdraft;
                                }
                            }
                        }
                    }

                    $sub_entity_avail_credit +=$amount;
                    $was_sent = do_credit_send ($super_entity_id, $jurisdiction, $super_available_credit,$super_available_overdraft,$super_total_overdraft,$supresetov,$sub_entity_avail_credit,$sub_entity_avail_overdraft,$sub_entity_total_overdraft,0, $_SESSION['admin_id']);
                }
            }
            elseif ( 'take_credits' == $_POST['transfer_type'] )
            {
                $transfer_type=20;
                $alljurisdictions=array_reverse($alljurisdictions);
                $keys = array_keys($alljurisdictions);
                for ($i = 0; $i < count($keys); $i++) {
                    $cur = $alljurisdictions[$keys[$i]];
                    $next = $alljurisdictions[$keys[$i+1]];
                    if($next!=''){
                        if(!is_int(adminUserCreditLogMultiple(
                                $_POST['note'],
                                $transfer_type,
                                $_SESSION['admin_id'],
                                $next,
                                $cur,
                                -$_POST['amount'],
                                0,
                                $i)
                        )){
                            addError('','An error has occurred when adding the transaction record');
                        };
                    }
                }

                $sub_entity_avail_credit= $sub_entity_avail_credit-$amount;
                if($super_available_credit < 0){
                    /*if(($amount+$super_available_credit)>=0){
                        $max_overdraft=-$super_available_credit;
                    }
                    else{
                        $max_overdraft=$amount;
                    }*/
                    $max_overdraft=$amount;
                    if($super_available_overdraft!=$super_total_overdraft){
                        if(($super_available_overdraft+$max_overdraft) > $super_total_overdraft){
                            $super_available_overdraft=$super_total_overdraft;
                        }
                        else{
                            $super_available_overdraft += $max_overdraft;
                        }
                    }
                }
                $super_available_credit +=$amount;
                $was_sent = do_credit_send ($super_entity_id, $jurisdiction, $super_available_credit,$super_available_overdraft,$super_total_overdraft,0,$sub_entity_avail_credit,$sub_entity_avail_overdraft,$sub_entity_total_overdraft,0, $_SESSION['admin_id']);
            }
            elseif ( 'send_overdraft' == $_POST['transfer_type'] )
            {
                if((strtotime('now')-strtotime($super_overdraft_start)> (3600*24*7)) && $_SESSION['jurisdiction_class']!='casino' && $super_available_credit<=0 && $super_overdraft_start!=''){
                    $dbh->rollback();
                    addError("", $lang->getLang("Your overdraft is expired"));
                    $show_form = 'entry';
                }
                else{
                    $transfer_type=21;
                    if(!is_int(adminUserCreditLog($_POST['note'],$transfer_type,$_SESSION['admin_id'],$super_entity_id,$jurisdiction,0,$_POST['amount'],$super_available_credit,$super_available_overdraft,$sub_entity_avail_credit,$sub_entity_total_overdraft))){
                        addError('','An error has occurred when adding the transaction record');
                    };
                    $reset = ($sub_entity_avail_overdraft==0? 1:0);

                    $sub_entity_avail_overdraft+=$amount;
                    if($_POST['totOvd']!='1'){
                        $sub_entity_total_overdraft +=$amount;
                    }
                    $super_available_overdraft -=$amount;
                    $was_sent = do_credit_send ($super_entity_id, $jurisdiction, $super_available_credit,$super_available_overdraft,$super_total_overdraft,0,$sub_entity_avail_credit,$sub_entity_avail_overdraft,$sub_entity_total_overdraft,$reset, $_SESSION['admin_id']);
                }
            }
            elseif ( 'take_overdraft' == $_POST['transfer_type'] )
            {
                $transfer_type=22;
                if(!is_int(adminUserCreditLog($_POST['note'],$transfer_type,$_SESSION['admin_id'],$super_entity_id,$jurisdiction,0,-$_POST['amount'],$super_available_credit,$super_available_overdraft,$sub_entity_avail_credit,$sub_entity_total_overdraft))){
                    addError('','An error has occurred when adding the transaction record');
                };
                $amount=$_POST['amount'];
                $sub_entity_avail_overdraft -= $amount;
                if($_POST['totOvd']!='1'){
                    $sub_entity_total_overdraft -= $amount;
                }
                $super_available_overdraft  += $amount;
                $reset = ($sub_entity_avail_overdraft==0 ? 2:0);
                $was_sent = do_credit_send ($super_entity_id, $jurisdiction, $super_available_credit,$super_available_overdraft,$super_total_overdraft,0,$sub_entity_avail_credit,$sub_entity_avail_overdraft,$sub_entity_total_overdraft,$reset, $_SESSION['admin_id']);
            }
            elseif ( 'reset_overdraft' == $_POST['transfer_type'] )
            {

                $time = ($_POST['ovReset']!='on'? $_POST['ovday']:0);
                $was_sent = setOverdraftTime($jurisdiction,$time);
            }

            if ( $was_sent && !areErrors() ) { // transaction successful
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
        if($_POST['transfer_type']=='reset_overdraft'){
            if(($_POST['ovReset']!='on') && $_POST['ovday']==''){
                addError("", $lang->getLang("Choose remove overdraft time or add a new start day!")); //invalid jurisdiction
            }
        }
        else{
            addError("", $lang->getLang("A collection amount must be entered")); //invalid jurisdiction
            $amount_valid = FALSE;
        }
    }elseif ( ! isValidMoney($_POST['amount'])  ){
        addError("", $lang->getLang("Invalid collection amount")); //invalid jurisdiction
        $amount_valid = FALSE;
    }

    if ( 'send_credits' != $_POST['transfer_type'] && 'take_credits' != $_POST['transfer_type'] &&
        'send_overdraft' != $_POST['transfer_type'] && 'take_overdraft' != $_POST['transfer_type'] && 'reset_overdraft' != $_POST['transfer_type'] ){
        addError("", $lang->getLang("Invalid or missing transfer type"));
    }
    // validate buyer entity id, and get account details of buyer
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
            $sub_entity_overdraft_start = $sub_arr['ent_overdraft_start'];
            $sub_avail_credit_fmt    = getInDollars($sub_entity_avail_credit,2,$sub_arr['cur_code_for_web']);

            if ( TRUE == $amount_valid ){
                // check if seller (entity current user an administrator of)
                if ( 'send_credits' == $_POST['transfer_type'] ){
                    // if superior is not the casino, and the avail credit is less than the credit transfer amount
                    if (  $super_entity_id != 1 && $_POST['amount'] > ($super_available_credit+$super_available_overdraft) && $_POST['amount'] > ($super_available_credit) && $_POST['amount'] > ($super_available_overdraft)){
                        addError("", $lang->getLang("Amount to high. You have insufficient available credit.")); //invalid jurisdiction
                        $amount_valid = FALSE;
                    }
                    if((strtotime('now')-strtotime($super_overdraft_start)> (3600*24*7)) && $_SESSION['jurisdiction_class']!='casino' && $super_available_credit<=0){
                        addError("", $lang->getLang("Your overdraft has expired")); //invalid jurisdiction
                        $amount_valid = FALSE;
                    }
                }
                elseif  ( 'take_credits' == $_POST['transfer_type'] ){
                    if (  $_POST['amount'] > ($sub_entity_avail_credit) ){
                        addError("", $lang->getLang("Amount to high. You have insufficient available credit.")); //invalid jurisdiction
                        $amount_valid = FALSE;
                    }
                }
                elseif  ( 'send_overdraft' == $_POST['transfer_type'] ){

                    if ( $super_entity_id != 1 && ( $_POST['amount'] > ($super_available_overdraft) )){
                        addError("", $lang->getLang("Amount to high. You have insufficient available overdraft.")); //invalid jurisdiction
                        $amount_valid = FALSE;
                    }
                    if((strtotime('now')-strtotime($super_overdraft_start)> (3600*24*7)) && $_SESSION['jurisdiction_class']!='casino' && $super_available_credit<=0){
                        addError("", $lang->getLang("Your overdraft has expired")); //invalid jurisdiction
                        $amount_valid = FALSE;
                    }

                }
                elseif  ( 'take_overdraft' == $_POST['transfer_type'] ){
                    if (  $_POST['amount'] > ($sub_entity_avail_overdraft) ){
                        addError("", $lang->getLang("Amount to high. The sub entity doesn't have insufficient available overdraft.")); //invalid jurisdiction
                        $amount_valid = FALSE;
                    }
                    if ( $super_entity_id != 1 &&($_POST['amount'] > ($super_total_overdraft-$super_available_overdraft)) ){
                        addError("", $lang->getLang("Amount exceeds your overdraft received.")); //invalid jurisdiction
                        $amount_valid = FALSE;
                    }
                }
                elseif  ( 'reset_overdraft' == $_POST['transfer_type'] ){
                    if(($_POST['ovReset']!='on') && $_POST['ovday']==''){
                        addError("", $lang->getLang("Choose remove overdraft time or add a ne start day!")); //invalid jurisdiction
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
        'send_overdraft' != $_POST['transfer_type'] && 'take_overdraft' != $_POST['transfer_type'] && 'reset_overdraft' != $_POST['transfer_type']){
        addError("", $lang->getLang("Invalid or missing transfer type"));
        unset($show_form);
    }else{
        $transfer_type = $_POST['transfer_type'];
        $show_form = 'entry';
    }
}

if ( 'completed' == $show_form ){
    $sub_arr = get_entity($_POST['jurisdiction']);
    $jurisdiction               = $_POST['jurisdiction'];
    $sub_entity_name            = $sub_arr['ent_name'];
    $sub_entity_avail_credit    = $sub_arr['ent_available_credit'];
    $sub_entity_avail_overdraft = $sub_arr['ent_available_overdraft'];
    $sub_entity_total_overdraft = $sub_arr['ent_tot_overdraft'];
    $sub_entity_overdraft_start = $sub_arr['ent_overdraft_start'];
    $sub_avail_credit_fmt    = getInDollars($sub_entity_avail_credit,2,$sub_arr['cur_code_for_web']);

    ?>      <div class=bodyHD2><?=$lang->getLang("Transaction Successful")?></div><br />
    <div class="tip fleft"><?=$lang->getLang("Time taken to execute your request")?>: <?=$duration?></div>
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
                    echo $lang->getLang("Extend credit");
                elseif ( 'take_credits' == $_POST['transfer_type'] )
                    echo $lang->getLang("Revoke credit");
                elseif ( 'send_overdraft' == $_POST['transfer_type'] )
                    echo $lang->getLang("Extend overdraft");
                elseif ( 'take_overdraft' == $_POST['transfer_type'] )
                    echo $lang->getLang("Revoke overdraft");
                elseif ( 'reset_overdraft' == $_POST['transfer_type'] )
                    echo $lang->getLang("Reset overdraft time");
                ?>
            </td>
        </tr>
        <tr>
            <td class=label><?=$lang->getLang("Your available credit")?></td>
            <td class="content"><?php if ($_SESSION["jurisdiction_id"] == 1 ) echo $lang->getLang("Unlimited"); else echo getInDollars($super_available_credit,2,$superCurrency);?></td>
        </tr>
        <tr>
            <td class=label><?=$lang->getLang("Your available overdraft")?></td>
            <td class="content"><?php if ($_SESSION["jurisdiction_id"] == 1 ) echo $lang->getLang("Unlimited"); else echo getInDollars($super_available_overdraft,2,$superCurrency);?></td>
        </tr>

        <tr>
            <td class=label><?=$label;?></td>
            <td class="content">
                <span style='float:left;margin-right: 10px'><?=$sub_entity_name?></span>
                <table  style='padding-left: 200px;'>
                    <tr><td class='label'><?=$lang->getLang("Credit")?></td><td class="unwrappable"><?=getInDollars($sub_entity_avail_credit,2,$sub_entity_currency)?></td></tr>
                    <tr><td class='label'><?=$lang->getLang("Overdraft")?></td><td><?=getInDollars($sub_entity_avail_overdraft,2,$sub_entity_currency)?></td></tr>
                    <tr><td class='label'><?=$lang->getLang("Total Overdraft Received")?></td><td><?=getInDollars($sub_entity_total_overdraft,2,$sub_entity_currency)?></td></tr>
                    <tr><td class='label'><?=$lang->getLang("Overdraft start time")?></td><td>
                            <?if( 'reset_overdraft' == $_POST['transfer_type'] ) { ?>
                                <?if($_POST['ovReset']=='on'){?> None
                                <?}else {?>  <?=$_POST['ovday']?>
                                <? }
                            } else { ?>
                                <?=($sub_entity_overdraft_start=='') ? 'none': $sub_entity_overdraft_start;?>
                            <? } ?>
                        </td></tr>
                </table>
            </td>
        </tr>
        <tr>
            <td class=label><?if($_POST['transfer_type']=='take_overdraft' || $_POST['transfer_type']=='send_overdraft') { ?><?=$lang->getLang("Overdraft Amount")?> <? } else { ?><?=$lang->getLang("Credit Amount")?><? } ?></td>
            <td class="content"><?=getInDollars($_POST['amount'],2,$sub_entity_currency)?></td>
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
        <input type="hidden" name="ovReset" value="<?=$_POST['ovReset'];?>">
        <input type="hidden" name="ovday" value="<?=$_POST['ovday'];?>">
        <input type="hidden" name="note" value="<?=$_POST['note'];?>">
        <input type="hidden" name="totOvd" value="<?=$_POST['totOvd'];?>">
        <table cellpadding=4 cellspacing=1 border=0>
            <tr>
                <td class=label><?=$lang->getLang("Transfer Type")?></td>
                <td class="content">
                    <?php
                    if ( 'send_credits' == $_POST['transfer_type'] )
                        echo $lang->getLang("Extend credit");
                    elseif ( 'take_credits' == $_POST['transfer_type'] )
                        echo $lang->getLang("Revoke credit");
                    elseif ( 'send_overdraft' == $_POST['transfer_type'] )
                        echo $lang->getLang("Extend overdraft");
                    elseif ( 'take_overdraft' == $_POST['transfer_type'] )
                        echo $lang->getLang("Revoke overdraft");
                    ?>
                </td>
            </tr>
            <tr>
                <td class=label><?=$lang->getLang("Your available credit")?></td>
                <td class="content"><?php if ($_SESSION["jurisdiction_id"] == 1 ) echo $lang->getLang("Unlimited"); else echo getInDollars($super_available_credit,2,$superCurrency);?></td>
            </tr>
            <tr>
                <td class=label><?=$lang->getLang("Your available overdraft")?></td>
                <td class="content"><?php if ($_SESSION["jurisdiction_id"] == 1 ) echo $lang->getLang("Unlimited"); else echo getInDollars($super_available_overdraft,2,$superCurrency);?></td>
            </tr>
            <tr>
                <td class=label><?=$label;?></td>
                <td class="content"><span style='float:left;margin-right: 10px'><?=$sub_entity_name?></span>
                    <table style='padding-left: 200px;'>
                        <tr><td class='label'><?=$lang->getLang("Credit")?></td><td class="unwrappable"><?=getInDollars($sub_entity_avail_credit,2,$sub_entity_currency)?></td></tr>
                        <tr><td class='label'><?=$lang->getLang("Overdraft")?></td><td><?=getInDollars($sub_entity_avail_overdraft,2,$sub_entity_currency)?></td></tr>
                        <tr><td class='label'><?=$lang->getLang("Total Overdraft Received")?></td><td><?=getInDollars($sub_entity_total_overdraft,2,$sub_entity_currency)?></td></tr>
                        <tr><td class='label'><?=$lang->getLang("Overdraft start time")?></td><td><?=($sub_entity_overdraft_start=='') ? 'none': $sub_entity_overdraft_start;?></td></tr>
                    </table>
                </td>
            </tr>
            <tr>
                <td class=label><?if($_POST['transfer_type']=='take_overdraft' || $_POST['transfer_type']=='send_overdraft') { ?>
                        <?=$lang->getLang("Overdraft Amount")?> <? }
                    elseif($_POST['transfer_type']=='reset_overdraft') { ?>
                        <?=$lang->getLang("Change overdraft time")?> <?
                    }
                    else { ?><?=$lang->getLang("Credit Amount")?><? } ?></td>
                <td class="content">
                    <?if($_POST['transfer_type']=='reset_overdraft') { ?>
                        <?if($_POST['ovReset']=='on'){?>Reset Overdraft time
                        <?}else {?> Set overdraft time to <?=$_POST['ovday']?>
                        <? } } else { ?>
                        <?=getInDollars($_POST['amount'],2,$sub_entity_currency)?>
                    <? }?>
                </td>
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
        <table><tr><td>
                    <table id="transfer_form" cellpadding=4 cellspacing=1 border=0>
                        <tr>
                            <td class=label><?=$lang->getLang("Transfer Type")?></td>
                            <td class="content">
                                <?php
                                if ( 'send_credits' == $_POST['transfer_type'] )
                                    echo $lang->getLang("Extend credit");
                                elseif ( 'take_credits' == $_POST['transfer_type'] )
                                    echo $lang->getLang("Revoke credit");
                                elseif ( 'send_overdraft' == $_POST['transfer_type'] )
                                    echo $lang->getLang("Extend overdraft");
                                elseif ( 'take_overdraft' == $_POST['transfer_type'] )
                                    echo $lang->getLang("Revoke overdraft");
                                elseif ( 'reset_overdraft' == $_POST['transfer_type'] )
                                    echo $lang->getLang("Reset overdraft time");
                                ?>
                            </td>
                        </tr>
                        <tr>
                            <td class=label><?=$lang->getLang("Your available credit")?></td>
                            <td class="content"><?php if ($_SESSION["jurisdiction_id"] == 1 ) echo $lang->getLang("Unlimited"); else echo getInDollars($super_available_credit,2,$superCurrency);?></td>
                        </tr>
                        <tr>
                            <td class=label><?=$lang->getLang("Your available overdraft")?></td>
                            <td class="content"><?php if ($_SESSION["jurisdiction_id"] == 1 ) echo $lang->getLang("Unlimited"); else echo getInDollars($super_available_overdraft,2,$superCurrency);?></td>
                        </tr>
                        <?php
                        if($transferToAll && (('send_credits' == $_POST['transfer_type'] ||  'take_credits' == $_POST['transfer_type'])||(strtolower($_SESSION['aty_code'])=='casinomanager'))){
                            $hide_option_one=true;
                            $noAffiliates=true;
                            include_once 'jurisdiction_filters.php';
                            include_once 'jurisdiction_filter_tabrow.php';
                        }
                        else {
                            ?>
                            <tr>
                                <td class=label><?=$lang->getLang("Choose jurisdiction")?></td>
                                <td class="content"> <? print_sub_entity_select($_SESSION['jurisdiction_id']);?></td>
                            </tr>
                        <? } if ( 'reset_overdraft' == $_POST['transfer_type'] ) { ?>
                            <tr>
                                <td class=label><?=$lang->getLang("Remove overdraft time")?></td>
                                <td class="content"> <input type='checkbox' name='ovReset' id='ovReset' /></td>
                            </tr>
                            <tr id='ovdayTr'>
                                <td class=label><?=$lang->getLang("Overdraft new start day")?></td>
                                <td class="content"> <input type='text' name='ovday' id='ovday' /></td>
                            </tr>
                            <script type="text/javascript">
                                $(function(){
                                    $( "#ovday" ).datepicker({
                                        changeMonth: true,
                                        numberOfMonths:1
                                    });
                                    $('#ovReset').change(function() {
                                        if($(this).is(":checked")) {
                                            $('#ovdayTr').fadeOut('slow');
                                        }
                                        else{
                                            $('#ovdayTr').fadeIn('slow');
                                        }
                                    });
                                });
                            </script>
                        <? } else { ?>
                            <tr>
                                <td class=label><?if($_POST['transfer_type']=='take_overdraft' || $_POST['transfer_type']=='send_overdraft') { ?><?=$lang->getLang("Overdraft Amount")?> <? } else { ?><?=$lang->getLang("Credit Amount")?><? } ?>
                                </td>
                                <td class="content"><input type="text" name="amount" value="<?=$_POST['amount']?>" size="10">
                                    <?if($_POST['transfer_type']=='send_overdraft' || $_POST['transfer_type']=='take_overdraft') { ?>
                                        <input type="checkbox" value='1' name='totOvd' ><?=$lang->getLang("Do not modify total overdraft")?>
                                    <? }?>
                                </td>
                            </tr>
                            <tr>
                                <td class=label><?=$lang->getLang("Note")?></td>
                                <td class="content"><textarea  name="note"><?=$_POST['note']?></textarea></td>
                            </tr>
                        <? } ?>

                        <tr>
                            <td></td>
                            <td>
                                <input type="button" name="change" value="<?=$lang->getLang("Back")?>" onClick="go_to_phase(this.form,'')" />
                                <input type="submit" name="confirm" value="<?=$lang->getLang("Next")?>" border=0 />
                            </td>
                        </tr>
                    </table>
                </td></tr></table>
    </form>

<?}elseif ( ! $show_form ){?>

    <?showErrors();?>

    <P>
    <form name="phase_1_form" action="<?=$_SERVER['PHP_SELF']?>" method="POST">
        <input type="hidden" name="phase" value="entry">
        <table  cellpadding=4 cellspacing=1 border=0>
            <tr>
                <td class=label><?=$lang->getLang("Your available credit")?></td>
                <td class="content"><?php if ( $_SESSION["jurisdiction_id"] == 1 ) echo $lang->getLang("Unlimited"); else echo getInDollars($super_available_credit,2,$superCurrency);?></td>
            </tr>
            <tr>
                <td class=label><?=$lang->getLang("Your available overdraft")?></td>
                <td class="content"><?php if ($_SESSION["jurisdiction_id"] == 1 ) echo $lang->getLang("Unlimited"); else echo getInDollars($super_available_overdraft,2,$superCurrency);?></td>
            </tr>
            <tr>
                <td class=label><?=$lang->getLang("Transfer Type")?></td>
                <td class="content">
                    <select name="transfer_type" id="transfer_type">
                        <option value="">- <?=$lang->getLang("Please select")?> -</option>
                        <option value="send_credits" <?if ('send_credits' == $_POST['transfer_type'] ) echo ' selected="selected"'?>><?=$lang->getLang("Extend credit")?></option>
                        <option value="take_credits" <?if ('take_credits' == $_POST['transfer_type'] ) echo ' selected="selected"'?>><?=$lang->getLang("Revoke credit")?></option>
                        <option value="send_overdraft" <?if ('send_overdraft' == $_POST['transfer_type'] ) echo ' selected="selected"'?>><?=$lang->getLang("Extend overdraft")?></option>
                        <option value="take_overdraft" <?if ('take_overdraft' == $_POST['transfer_type'] ) echo ' selected="selected"'?>><?=$lang->getLang("Revoke overdraft")?></option>
                        <option value="reset_overdraft" <?if ('reset_overdraft' == $_POST['transfer_type'] ) echo ' selected="selected"'?>><?=$lang->getLang("Change overdraft time")?></option>
                    </select>
                </td>
            </tr>
            <tr>
                <td></td>
                <td>
                    <input type="submit" name="confirm" value="<?=$lang->getLang("Next")?> &gt;">
                </td>
            </tr>
        </table>
    </form>
<?
}
?>
<?include 'footer.inc';?>
