<?php
check_access("transactions_affiliates", true);
$depositWithdraw=check_access("transactions_affiliates_transfer");
$overdraftDepositWithdraw=check_access("transactions_affiliates_overdraft");
?>
<?require_once "account_functions.php";?>
<div class="bodyHD">Affiliate Credit Management</div><br />
    <link rel="stylesheet" type="text/css" href="/media/jquery.multiselect.css" title="core_style">
<script src="/media/jquery.multiselect.js" type="text/javascript"></script>
<script type="text/javascript">

    $(function(){
        $("#jurisdiction").multiselect({
            multiple: false,
            header: "Select an option",
            noneSelectedText: "Select an Option",
            selectedList: 1
        }).multiselectfilter();
        $( "#from" ).datepicker({
            changeMonth: true,
            numberOfMonths:1
        });

        $('#' + $("#jurisdiction").find('option:selected').val()+'Info').show();
        $('#' + $("#jurisdiction").find('option:selected').val()+'Info .affInfo').prop('disabled',false);

        if($('#' + $("#jurisdiction").find('option:selected').val()+'Amnt').val()< 1 ){
            $("#operation option[value='withdraw']").hide();
            if($('#operation').find('option:selected').val()=='withdraw'){
                $("#operation").val($("#operation option:first").val());
            }
        }
        if($('#' + $("#jurisdiction").find('option:selected').val()+'OAmnt').val()< 1 ){
            $("#operation option[value='owithdraw']").hide();
            if($('#operation').find('option:selected').val()=='owithdraw'){
                $("#operation").val($("#operation option:first").val());
            }
        }

        $('#jurisdiction').change(function() {
            $('.hidden').hide();
            $('#' + $(this).find('option:selected').val()+'Info').show();
            $('.affInfo').prop('disabled',true);
            $('#' + $(this).find('option:selected').val()+'Info .affInfo').prop('disabled',false);
            if($('#' + $(this).find('option:selected').val()+'Amnt').val()<1){
                $("#operation option[value='withdraw']").hide();
                if($('#operation').find('option:selected').val()=='withdraw'){
                    $("#operation").val($("#operation option:first").val());
                }
            }
            else{
                $("#operation option[value='withdraw']").show();
            }

            if($('#' + $("#jurisdiction").find('option:selected').val()+'OAmnt').val()< 1 ){
                $("#operation option[value='owithdraw']").hide();
                $("#fromTr").hide();
                if($('#operation').find('option:selected').val()=='owithdraw'){
                    $("#operation").val($("#operation option:first").val());
                }
            }
            else{
                $("#operation option[value='owithdraw']").show();
            }
        });


        $('#amount').keypress(function(event){
            if(event.which != 8 && isNaN(String.fromCharCode(event.which))){
                event.preventDefault();
            }});


        if($("#operation").find('option:selected').val()=='reset'){
            $('#fromTr,#ovResetTr').show();
            $('#amountTr').hide();
        }
        else if($("#operation").find('option:selected').val()=='owithdraw')
        {
            $('#fromTr,#amountTr,#ovResetTr').show();
        }
         else if($("#operation").find('option:selected').val()=='odeposit' ){
            $('#fromTr,#amountTr').show();
            $('#ovResetTr').hide();
            $('#ovReset').prop('checked',false);

        }
        else{
            $('#fromTr,#ovResetTr').hide();
            $('#ovReset').prop('checked',false);
            $('#amountTr').show();
        }

        $('#operation').change(function() {
            if($(this).find('option:selected').val()=='reset'){
                $('#fromTr,#ovResetTr').show();
                $('#amountTr').hide();
            }
            else if($(this).find('option:selected').val()=='owithdraw')
            {
                $('#fromTr,#amountTr,#ovResetTr').show();
            }
            else if($(this).find('option:selected').val()=='odeposit' ){
                $('#fromTr,#amountTr').show();
                $('#ovResetTr').hide();
                $('#ovReset').prop('checked',false);

            }
            else{
                $('#fromTr,#ovResetTr').hide();
                $('#ovReset').prop('checked',false);
                $('#amountTr').show();
            }
        });
        $('#ovReset').change(function() {
            if($(this).is(":checked")) {
                $('#fromTr').fadeOut('slow');
            }
            else{
                $('#fromTr').fadeIn('slow');
            }
        });
    });
</script>
<?php
if(isset($_POST['jurisdiction'])){
    $starttime = microtime(true);
    switch ($_POST['operation']){
        case 'reset':
            if(changeOverdraftDate($_POST['jurisdiction'],((isset($_POST['ovReset']))? 'NULL': $_POST['from']))){
                if(isset($_POST['ovReset']))
                {
                    addResult('','Successfully reset the overdraft date ');

                }
                else{
                    addResult('','Successfully updated the overdraft date to '.$_POST['from']);
                }
                showResult();
            }
            break;

        case 'deposit':
            $dbh->begin();
            if($_POST['amount']!=''){
                $alljurisdictions=getAllEntitiesBetween($_SESSION['jurisdiction_class'],$_POST[$_POST['jurisdiction']."_cid"]);
                $trnsId= strtoupper('ADM'.$dbh->nextSequence('CTR_TRAN_NUM_SEQ').rand_str(4));
                $transfer_type=19;
                $keys = array_keys($alljurisdictions);
                for ($i = 0; $i < count($keys); $i++) {
                    $prev=$alljurisdictions[$keys[$i-1]];
                    $cur = $alljurisdictions[$keys[$i]];
                    $next = $alljurisdictions[$keys[$i+1]];
                    if($next!='')
                    {
                        if(!is_int(adminUserCreditLog($_POST['note'],$transfer_type,$_SESSION['admin_id'],$cur,$next,$_POST['amount'],$i,($aff=($keys[$i]=='district'? $_POST['jurisdiction']:0))))){

                            addError('','An error has occurred when adding the transaction record');

                        }
                        if($keys[$i]=='district'){
                            if(!affiliateCreditLog($_POST['note'],$_POST['amount'],0,$_POST[$_POST['jurisdiction'].'Amnt'],$_POST[$_POST['jurisdiction']."OAmnt"],$_SESSION['jurisdiction_credit'],$_POST['jurisdiction'],$_SESSION['admin_id'],19)){
                                addError('','An error has occurred when adding the affiliate transaction record');
                            }
                        }
                    }
                }
                if(areErrors()){
                    $dbh->rollback();
                    showErrors();
                }
                else{

                    if(do_credit_send($_SESSION['jurisdiction_id'],$_POST[$_POST['jurisdiction']."_cid"],$_POST['amount'],$_POST['jurisdiction'])){
                        $_SESSION['jurisdiction_credit']=$_SESSION['jurisdiction_credit']-$_POST['amount'];
                        $dbh->commit();
                        addResult('','Successfully deposited '.getInDollars($_POST['amount']));
                        showResult();
                    }
                    else{
                        $dbh->rollback();
                        showErrors();
                    }
                }
            }else{
                addError('','Please insert amount');
                showErrors();
            }
            break;

        case 'withdraw':
            $dbh->begin();
            if($_POST['amount']!=''){
                if($_POST['amount']>$_POST[$_POST['jurisdiction'].'Amnt']){
                    addError('','Please choose a value smaller than '.getInDollars($_POST[$_POST['jurisdiction'].'Amnt']));
                    showErrors();
                }
                else{
                    $alljurisdictions=array_reverse(getAllEntitiesBetween($_SESSION['jurisdiction_class'],$_POST[$_POST['jurisdiction']."_cid"]));
                    $trnsId= strtoupper('ADM'.$dbh->nextSequence('CTR_TRAN_NUM_SEQ').rand_str(4));
                    $transfer_type=20;
                    $keys = array_keys($alljurisdictions);
                    for ($i = 0; $i < count($keys); $i++) {
                        $prev=$alljurisdictions[$keys[$i-1]];
                        $cur = $alljurisdictions[$keys[$i]];
                        $next = $alljurisdictions[$keys[$i+1]];
                        if($next!='')
                        {
                            if(!is_int(adminUserCreditLog($_POST['note'],$transfer_type,$_SESSION['admin_id'],$next,$cur,-$_POST['amount'],$i,($aff=($keys[$i]=='club'? $_POST['jurisdiction']:0))))){
                                addError('','An error has occurred when adding the transaction record');
                            }
                            if($keys[$i]=='district'){
                                if(!affiliateCreditLog($_POST['note'],-$_POST['amount'],0,$_POST[$_POST['jurisdiction'].'Amnt'],$_POST[$_POST['jurisdiction']."OAmnt"],$_SESSION['jurisdiction_credit'],$_POST['jurisdiction'],$_SESSION['admin_id'],20)){
                                    addError('','An error has occurred when adding the affiliate transaction record');
                                }
                            }
                        }
                    }
                    if(areErrors()){
                        $dbh->rollback();
                        showErrors();
                    }
                    else{
                        if(do_credit_return($_SESSION['jurisdiction_id'],$_POST[$_POST['jurisdiction']."_cid"],$_POST['amount'],$_POST['jurisdiction'])){
                            $_SESSION['jurisdiction_credit']=$_SESSION['jurisdiction_credit']+$_POST['amount'];
                            $dbh->commit();
                            addResult('','Successfully withdrawn '.getInDollars($_POST['amount']));
                            showResult();
                        }
                        else{
                            $dbh->rollback();
                            showErrors();
                        }
                    }
                }
            }
            else{
                addError('','Please insert amount');
                showErrors();
            }
            break;

        case 'odeposit':
            $dbh->begin();
            if($_POST['amount']!=''){
                $alljurisdictions=getAllEntitiesBetween($_SESSION['jurisdiction_class'],$_POST[$_POST['jurisdiction']."_cid"]);
                $trnsId= strtoupper('ADM'.$dbh->nextSequence('CTR_TRAN_NUM_SEQ').rand_str(4));
                $transfer_type=19;
                $keys = array_keys($alljurisdictions);
                for ($i = 0; $i < count($keys); $i++) {
                    $prev=$alljurisdictions[$keys[$i-1]];
                    $cur = $alljurisdictions[$keys[$i]];
                    $next = $alljurisdictions[$keys[$i+1]];
                    if($next!='')
                    {
                        if(!is_int(adminUserCreditLog($_POST['note'],$transfer_type,$_SESSION['admin_id'],$cur,$next,$_POST['amount'],$i,($aff=($keys[$i]=='district'? $_POST['jurisdiction']:0))))){
                            addError('','An error has occurred when adding the transaction record');
                        }
                        if($keys[$i]=='district'){
                            if(!affiliateCreditLog($_POST['note'],0,$_POST['amount'],$_POST[$_POST['jurisdiction'].'Amnt'],$_POST[$_POST['jurisdiction']."OAmnt"],$_SESSION['jurisdiction_credit'],$_POST['jurisdiction'],$_SESSION['admin_id'],21)){
                                addError('','An error has occurred when adding the affiliate transaction record');
                            }
                        }
                    }
                }
                if(areErrors()){
                    $dbh->rollback();
                    showErrors();
                }
                else{

                    if(do_overdraft_send($_SESSION['jurisdiction_id'],$_POST[$_POST['jurisdiction']."_cid"],$_POST['amount'],$_POST['jurisdiction'])){
                        if($_POST['from']!=''){
                            if(changeOverdraftDate($_POST['jurisdiction'],$_POST['from'])){
                                addResult('','Successfully updated the overdraft date to '.$_POST['from']);
                            }
                            else{
                                showErrors();
                            }
                        }
                        $_SESSION['jurisdiction_credit']=$_SESSION['jurisdiction_credit']-$_POST['amount'];
                        $dbh->commit();
                        addResult('','Successfully deposited overdraft '.getInDollars($_POST['amount']));
                        showResult();
                    }
                    else{
                        $dbh->rollback();
                        showErrors();
                    }
                }
            }else{
                addError('','Please insert amount');
                showErrors();
            }
            break;

        case 'owithdraw':
            $dbh->begin();
            if($_POST['amount']!=''){
                if($_POST['amount']>$_POST[$_POST['jurisdiction'].'OAmnt']){
                    addError('','Please choose a value smaller than '.getInDollars($_POST[$_POST['jurisdiction'].'OAmnt']));
                    showErrors();
                }
                else{
                    $alljurisdictions=array_reverse(getAllEntitiesBetween($_SESSION['jurisdiction_class'],$_POST[$_POST['jurisdiction']."_cid"]));
                    $trnsId= strtoupper('ADM'.$dbh->nextSequence('CTR_TRAN_NUM_SEQ').rand_str(4));
                    $transfer_type=20;
                    $keys = array_keys($alljurisdictions);
                    for ($i = 0; $i < count($keys); $i++) {
                        $prev=$alljurisdictions[$keys[$i-1]];
                        $cur = $alljurisdictions[$keys[$i]];
                        $next = $alljurisdictions[$keys[$i+1]];
                        if($next!='')
                        {
                            if(!is_int(adminUserCreditLog($_POST['note'],$transfer_type,$_SESSION['admin_id'],$next,$cur,-$_POST['amount'],$i,($aff=($keys[$i]=='club'? $_POST['jurisdiction']:0))))){
                                addError('','An error has occurred when adding the transaction record');
                            }
                            if($keys[$i]=='district'){
                                if(!affiliateCreditLog($_POST['note'],0,-$_POST['amount'],$_POST[$_POST['jurisdiction'].'Amnt'],$_POST[$_POST['jurisdiction']."OAmnt"],$_SESSION['jurisdiction_credit'],$_POST['jurisdiction'],$_SESSION['admin_id'],22)){
                                    addError('','An error has occurred when adding the affiliate transaction record');
                                }
                            }
                        }
                    }
                    if(areErrors()){
                        $dbh->rollback();
                        showErrors();
                    }
                    else{
                        if(do_overdraft_return($_SESSION['jurisdiction_id'],$_POST[$_POST['jurisdiction']."_cid"],$_POST['amount'],$_POST['jurisdiction'])){
                            $_SESSION['jurisdiction_credit']=$_SESSION['jurisdiction_credit']+$_POST['amount'];
                            if($_POST['from']!='' || $_POST['ovReset']){
                                if(changeOverdraftDate($_POST['jurisdiction'],((isset($_POST['ovReset'])) ? 'NULL': $_POST['from']))){
                                    if(isset($_POST['ovReset']))
                                    {
                                        addResult('','Successfully reset the overdraft date ');
                                    }
                                    else{
                                        addResult('','Successfully updated the overdraft date to '.$_POST['from']);
                                    }
                                }
                                else{
                                    showErrors();
                                }
                            }
                            $dbh->commit();
                            addResult('','Successfully withdrawn overdraft '.getInDollars($_POST['amount']));
                            showResult();
                        }
                        else{
                            $dbh->rollback();
                            showErrors();
                        }
                    }
                }
            }
            else{
                addError('','Please insert amount');
                showErrors();
            }
            break;
    }
    $endtime = microtime(true);
    $duration = $endtime - $starttime;
    $duration=number_format($duration, 4, ',', '')."s";
}
?>
<?if ($depositWithdraw || $overdraftDepositWithdraw){ ?>

<?form_head()?>
<?if(isset($_POST['jurisdiction'])){?>
    <div class="tip"><?=$lang->getLang("Time took to execute your request")?>:<?=$duration?></div>
<? }?>
<table><tr><td class="label">Select Affiliate</td><td>
            <?php
            $result=getJurisdictionsAffiliates($_SESSION['jurisdiction_id']);
            $clubs=array();
            while($result->hasNext()){
                $row=$result->next();
                if(!is_array($clubs[$row['district_name']])){
                    $clubs[$row['district_name']]=array();
                }
                $clubs[$row['district_name']][$row['aff_id']]=array("club"=>$row['club_name'],
                    "club_id"=>$row['club_id'],
                    "username"=>$row['aus_username'],
                    "availableCredit"=>$row['act_credits'],
                    "overdraft"=>$row['act_overdraft'],
                    "overdraft_time"=>$row['act_overdraft_start_time']
                );
            }
            ?>
            <select name='jurisdiction' id='jurisdiction'>
                <?
                foreach($clubs as $k=>$v){
                    ?>
                    <optgroup label="<?=$k?>">
                        <? foreach ($v as $k2=>$v2){
                            ?>
                            <option value='<?=$k2?>' <?if($_POST['jurisdiction']==$k2){?> selected="selected" <? } ?>><?=$v2['username']?></option>
                        <?
                        }
                        ?>
                    </optgroup>
                <?
                }
                ?>
            </select>
        </td>
        <td>
            <? foreach($clubs as $k=>$v){
                foreach ($v as $k2=>$v2){
                    ?>


                    <table id="<?=$k2?>Info" class="hidden" style="margin-left: 20px;">
                        <input type="hidden" id="<?=$k2?>OAmnt" name="<?=$k2?>OAmnt" value="<?=$v2['overdraft']?>" class='affInfo' disabled="disabled" />
                        <input type="hidden" name="<?=$k2?>_cid" value="<?=$v2['club_id']?>" class='affInfo' disabled="disabled" />
                        <input type="hidden" id="<?=$k2?>Amnt" name="<?=$k2?>Amnt" value="<?=$v2['availableCredit']?>" class='affInfo' disabled="disabled" />
                        <tr><td class="label"><?=$lang->getLang("Club name")?></td><td><?=$v2['club']?></td></tr>
                    <? if($depositWithdraw) { ?>
                        <tr><td class="label">Credit</td><td><?=getInDollars($v2['availableCredit'])?>
                            </td></tr>
                        <? } if($overdraftDepositWithdraw) { ?>
                        <tr><td class="label">Overdraft</td><td><?=getInDollars($v2['overdraft'])?>
                               </td></tr>
                        <tr><td class="label">Overdraft start time</td><td><?=($v2['overdraft_time']=='0000-00-00 00:00:00') ? 'none': $v2['overdraft_time'] ?></td></tr>
                        <? } ?>
                    </table>
                <? }
            } ?>
        </td>
    </tr>
    <tr>
        <td class="label">Select operation</td>
        <td>
            <select name='operation' id='operation'>
                <? if($depositWithdraw) {?>
                <option value='deposit' <?=($_POST['operation']=='deposit'? 'selected="selected"' : '' )?>>Deposit</option>
                <option value='withdraw' <?=($_POST['operation']=='withdraw'? 'selected="selected"' : '' )?>>Withdraw</option>
                 <? } if($overdraftDepositWithdraw) { ?>
                <option value='odeposit' <?=($_POST['operation']=='odeposit'? 'selected="selected"' : '' )?>>Deposit Overdraft</option>
                <option value='owithdraw' <?=($_POST['operation']=='owithdraw'? 'selected="selected"' : '' )?>>Withdraw Overdraft</option>
                <option value='reset' <?=($_POST['operation']=='reset'? 'selected="selected"' : '' )?>>Reset overdraft</option>
                <? } ?>
            </select>
        </td>
    </tr>
    <tr id='amountTr'>
        <td class="label">Amount ( &euro; )</td>
        <td>
            <input type="number" name='amount' id='amount' min="1" value="<?=$_POST['amount']?>" autocomplete="off" />
        </td>
    </tr>
    <tr id='fromTr'>
        <td class="label">Overdraft new start day</td>
        <td>
            <input type="text" name='from' id='from' autocomplete="off" />
        </td>
    </tr>

    <tr id='ovResetTr'>
        <td class="label">Remove overdraft time </td>
        <td>
            <input type="checkbox" name='ovReset' id='ovReset' />
        </td>
    </tr>
    <tr>
        <td class=label><?=$lang->getLang("Note")?></td>
        <td class="content"><textarea  name="note"></textarea></td>
    </tr>

    <tr>
        <td colspan="2"><input type="submit" value="Submit" /> </td>
    </tr>
</table>
</form>
<? }
    else {
    ?>
    <div class="error">You're not allowed to see this page</div>
<? } ?>
<?php

function changeOverdraftDate($affiliate,$newdate){
    global $dbh;
    if (DateTime::createFromFormat('Y-m-d', $newdate) !== FALSE || $newdate=='NULL') {
    $sql="Update affiliate_credit set act_overdraft_start_time=".$dbh->prepareString($newdate)." Where act_aff_id=".$dbh->escape($affiliate);
        try{
        $result=$dbh->exec($sql);
        if($result!=1){
            throw new Exception('Query error');
        }
    }
    catch(Exception $e){
        addError('','The following errors have occurred: '.$e->getMessage().'. The database was not updated.');
        return false;
    }
    }
    else{
        addError('','Invalid date!');
        return false;
    }
    return true;
}

function getJurisdictionsAffiliates($jur_id) {
    global $dbh;
    $jur_class=$_SESSION['jurisdiction_class'];
    $sql = "select c.jur_name club_name,c.jur_id club_id,d.jur_name district_name,aus_username,act_credits,act_overdraft,aff_id,act_overdraft_start_time
            FROM jurisdiction c
            JOIN jurisdiction d on d.jur_id = c.jur_parent_id
            JOIN admin_user on c.jur_id=aus_jurisdiction_id
            JOIN affiliate on aff_aus_id=aus_id
            JOIN affiliate_credit on act_aff_id=aff_id
    ";
    if($jur_class == "club"){
        $sql .= " WHERE c.jur_id = " . $jur_id;
    }
    elseif($jur_class == "district"){
        $sql .= "
				  WHERE d.jur_id = " . $jur_id;
    }
    elseif($jur_class=="region"){        $sql .="
				  JOIN jurisdiction r on r.jur_id = d.jur_parent_id
				  WHERE r.jur_id=".$jur_id;
    }
    elseif ($jur_class=="nation"){
        $sql .=" JOIN jurisdiction r on r.jur_id = d.jur_parent_id
				 JOIN jurisdiction n on n.jur_id = r.jur_parent_id
				WHERE n.jur_id=".$jur_id;
    }
    else{
        $sql .=" WHERE 1=1";
    }
    $sql.=" AND c.jur_class='club'
            AND aff_is_club=1
    ";
    $result = $dbh->doCachedQuery ( $sql, 0 );
    return $result;

}

function do_credit_send($super_ent_id, $aff_club_id, $amount, $aff_id){
    global $dbh;
    $super_credit_updated = subtract_available_credit($super_ent_id, $amount);
    $sub_credit_updated   = add_affiliate_available_credit($aff_id, $amount);
    if ( ! ( TRUE == $super_credit_updated &&  TRUE == $sub_credit_updated) ){
        return false;
    }
    return TRUE;
}

function do_credit_return($super_ent_id, $aff_club_id, $amount, $aff_id){
    global $dbh;
    $super_credit_updated = subtract_affiliate_credit($aff_id, $amount);
    $sub_credit_updated   = add_available_credit($super_ent_id, $amount);
    if ( ! ( TRUE == $super_credit_updated &&  TRUE == $sub_credit_updated) ){
        return false;
    }
    return TRUE;
}

function do_overdraft_send($super_ent_id, $aff_club_id, $amount, $aff_id){
    global $dbh;
    $super_credit_updated = subtract_available_credit($super_ent_id, $amount);
    $sub_credit_updated   = add_affiliate_available_overdraft($aff_id, $amount);
    if ( ! ( TRUE == $super_credit_updated &&  TRUE == $sub_credit_updated) ){
        return false;
    }
    return TRUE;

}

function do_overdraft_return($super_ent_id, $aff_club_id, $amount, $aff_id){
    global $dbh;
    $super_credit_updated = subtract_affiliate_overdraft($aff_id, $amount);
    $sub_credit_updated   = add_available_credit($super_ent_id, $amount);
    if ( ! ( TRUE == $super_credit_updated &&  TRUE == $sub_credit_updated) ){
        return false;
    }
    return TRUE;
}


function adminUserCreditLog($note='',$transactionType,$admId,$admJurId,$admFinalJurId,$admAmount,$mult=0,$aff=0)
{
    global $dbh,$trnsId;
    $sql="Insert into admin_user_transaction (att_note,
                                              att_tty_id,
                                              att_aus_id, ";

    if($aff!=0){
        $sql.="att_aff_id,";
    }
    $sql.=" att_transaction_id,
                                                att_jur_id,
                                                att_final_jur_id,
                                                att_jur_avail_credit,
                                                att_amount,
                                                att_jur_final_avail_credit
                                                )
                                        Values (
                                                ".$dbh->prepareString($note).",
                                                ".$dbh->prepareString($transactionType).",
                                                ".$dbh->prepareString($admId).",";
    if($aff!=0){
        $sql.=$dbh->escape($aff).",";
    }
    $sql.=$dbh->prepareString($trnsId).",
                                                ".$dbh->prepareString($admJurId).",
                                                ".$dbh->prepareString($admFinalJurId).",
                                                (SELECT jur_available_credit from jurisdiction where jur_id =".$dbh->prepareString($admJurId).")";

    if($mult!=0 && ($transactionType==19 || $transactionType==21)){
        $sql.="+".$dbh->prepareString($admAmount);
    }

    $sql.=",".$dbh->prepareString($admAmount).",";
    if($aff==0){
        $sql.="(SELECT jur_available_credit from jurisdiction where jur_id =".$dbh->prepareString($admFinalJurId).")";
    }
    else{
        $sql.=" (SELECT act_credits from affiliate_credit where act_aff_id =".$dbh->escape($aff).")";
    }

    if($mult!=0 && ($transactionType==20 || $transactionType==22)){
        $sql.="-".$dbh->prepareString($admAmount);
    }
    $sql.=")";
    $result=$dbh->exec($sql);
    return  $result;
}

function getAllEntitiesBetween($superClass,$sub){
    global $dbh;
    switch ($superClass)
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
            $from = "FROM jurisdiction n
         LEFT JOIN jurisdiction r ON n.jur_id = r.jur_parent_id
         LEFT JOIN jurisdiction d on r.jur_id = d.jur_parent_id
         LEFT JOIN jurisdiction c on d .jur_id =c.jur_parent_id  ";
            break;
        case 'region':
            $params =" r.jur_id as region,d.jur_id as district,c.jur_id as club ";
            $from = " FROM jurisdiction r
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
    $where=" Where c.jur_id=".$sub;
    $group=" Group by c.jur_id";

    $params=implode(',',$params);
    $sql ="SELECT $params $from $where $group";
    $result=$dbh->queryRow($sql);
    return $result;
}


function affiliateCreditLog($note='',$amount=0,$overdraft=0,$availableCredit,$availableoverdraft,$otheravaiablecredit,$aff_id,$aus_id,$transactionType){
    global $dbh,$trnsId;
    $sql="Insert into affiliate_transaction ( atr_notes,";
    if($amount!=0){$sql.="atr_amount,";}
    if($overdraft!=0){$sql.="atr_overdraft,";}
    $sql.="atr_available_credit,
           atr_available_overdraft,
           atr_other_available_credit,
           atr_aff_id,
           atr_aus_id,
           atr_transaction_id,
           atr_tty_id)
           Values (".$dbh->prepareString($note).",";
    if($amount!=0){$sql.=$dbh->prepareString($amount).",";}
    if($overdraft!=0){$sql.=$dbh->prepareString($overdraft).",";}
    $sql.=$dbh->prepareString($availableCredit).",
        ".$dbh->prepareString($availableoverdraft).",
        ".$dbh->prepareString($otheravaiablecredit).",
        ".$dbh->prepareString($aff_id).",
        ".$dbh->prepareString($aus_id).",
        ".$dbh->prepareString($trnsId).",".$transactionType.")";
    return $dbh->exec($sql);

}
?>