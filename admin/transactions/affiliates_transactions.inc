<?php
check_access("transactions_affiliates", true);
$depositWithdraw=check_access("transactions_affiliates_transfer");
$overdraftDepositWithdraw=check_access("transactions_affiliates_overdraft");
$_POST['totOvd']=0;
?>
<?require_once "account_functions.php";?>
<div class="bodyHD"><?=$lang->getLang("Affiliate Credit Management")?></div><br />
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

     /*   if($('#' + $("#jurisdiction").find('option:selected').val()+'Amnt').val()< 1 ){
            $("#operation option[value='withdraw']").hide();
            if($('#operation').find('option:selected').val()=='withdraw'){
                $("#operation").val($("#operation option:first").val());
            }
        }*/
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
            if(event.which != 8 && event.which != 46 && isNaN(String.fromCharCode(event.which))){
                event.preventDefault();
            }});


        $('#operation').change(function() {
            if($(this).find('option:selected').val()=='reset'){
                $('#fromTr,#ovResetTr').show();
                $('#amountTr').hide();
            }
            else if($(this).find('option:selected').val()=='owithdraw')
            {
                $('#fromTr,#amountTr,#ovResetTr,#totOvd').show();
            }
            else if($(this).find('option:selected').val()=='odeposit' ){
                $('#fromTr,#amountTr,#totOvd').show();
                $('#ovResetTr').hide();
                $('#ovReset').prop('checked',false);
            }
            else{
                $('#fromTr,#ovResetTr,#totOvd').hide();
                $('#ovReset').prop('checked',false);
                $('#amountTr').show();
            }
        }).trigger('change');
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
    $affclub=$_POST[$_POST['jurisdiction']."_cid"];
    $aff=$_POST['jurisdiction'];
    $trnsId= strtoupper('ADM'.$dbh->nextSequence('CTR_TRAN_NUM_SEQ').rand_str(4));
    $sql = "SELECT jur_id, jur_code, jur_code, jur_available_credit,jur_overdraft,jur_tot_overdraft_received,jur_overdraft_start_time FROM jurisdiction " .
        " WHERE jur_id = " . $_SESSION['jurisdiction_id'];
    $rs       = $dbh->exec($sql,false,true);
    $row = $rs->next();
        $super_entity_id            = $_SESSION['jurisdiction_id'];
        $super_acc_code             = $row['jur_code'];
        $super_available_credit     = $row['jur_available_credit'];
        $super_available_overdraft  = $row['jur_overdraft'];
        $super_total_overdraft      = $row['jur_tot_overdraft_received'];
        $super_overdraft_start      = $row['jur_overdraft_start_time'];
        $amount                     = $_POST['amount'];
        $sub_entity_avail_credit=$dbh->queryOne("SELECT act_credits from affiliate_credit where act_aff_id=".$dbh->escape($_POST['jurisdiction']),true);
        $sub_entity_avail_overdraft=$dbh->queryOne("SELECT act_overdraft from affiliate_credit where act_aff_id=".$dbh->escape($_POST['jurisdiction']),true);

    switch ($_POST['operation']){
        case 'reset':
            if(changeOverdraftDate($_POST['jurisdiction'],((isset($_POST['ovReset']))? 'NULL': $_POST['from']))){
                if(isset($_POST['ovReset']))
                {
                    addResult('',$lang->getLang('Successfully reset the overdraft date'));
                    doAdminUserLog($_SESSION['admin_id'],'overdraft modify','Reset overdraft time for affiliate '.$_POST['jurisdiction'],'NULL',getIpAddress());
                }
                else{
                    addResult('',$lang->getLang('Successfully updated the overdraft date to')." ".$_POST['from']);
                    doAdminUserLog($_SESSION['admin_id'],'overdraft modify','Changed overdraft time to '.$dbh->escape($_POST['from']).' for affiliate '.$_POST['jurisdiction'],'NULL',getIpAddress());
                }
                showResult();
            }
            break;

        case 'deposit':
            $dbh->begin();
            $supresetov=0;

            if($_POST['amount']!=''){
                if (  $super_entity_id != 1 && $_POST['amount'] > ($super_available_credit+$super_available_overdraft) && $_POST['amount'] > ($super_available_credit) && $_POST['amount'] > ($super_available_overdraft)){
                    addError("", $lang->getLang("Amount to high. You have insufficient available credit.")); //invalid jurisdiction
                    showErrors();
                }
                elseif((strtotime('now')-strtotime($super_overdraft_start)> (3600*24*7)) && $_SESSION['jurisdiction_class']!='casino' && $super_available_credit < $_POST['amount'] && $super_overdraft_start!=''){
                        addError("", $lang->getLang("Your overdraft has expired"));
                        showErrors();
                    }
                else{
                    $jursdictionClass=$_SESSION['jurisdiction_class'];
                    $jurisdictionPostClass='club';
                    $alljurisdictions=getAllEntitiesBetween($jursdictionClass,$jurisdictionPostClass,$affclub);
                    $transfer_type=19;
                    $keys = array_keys($alljurisdictions);
                    for ($i = 0; $i < count($keys); $i++) {
                        $cur = $alljurisdictions[$keys[$i]];
                        $next = $alljurisdictions[$keys[$i+1]];
                        if($next!=''){
                            if($next==$affclub){

                                $distcredit=$dbh->queryOne('select jur_available_credit from jurisdiction where jur_id='.$cur,true);
                                $distov=$dbh->queryOne('select jur_overdraft from jurisdiction where jur_id='.$cur,true);
                                if($_SESSION['jurisdiction_class']!='district'){
                                    $distcredit+=$amount;
                                    $last=true;
                                }
                                if(!is_int(adminUserCreditLog($_POST['note'],$transfer_type,$_SESSION['admin_id'],$cur,$affclub,$_POST['amount'], $_POST['jurisdiction'],$last))){
                                    addError('','An error has occurred when adding the transaction record');
                                }
                                if(!affiliateCreditLog($_POST['note'],$_POST['amount'],0,$sub_entity_avail_credit,$sub_entity_avail_overdraft,$distcredit,$distov,$_POST['jurisdiction'],$_SESSION['admin_id'],19)){
                                    addError('','An error has occurred when adding the affiliate transaction record');
                                }
                            }
                            else{
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
                    }


                if(areErrors()){
                    $dbh->rollback();
                    showErrors();
                }
                else{
                    if($super_available_credit<$amount){
                        if($super_available_credit<0){
                            $super_available_overdraft=($super_available_overdraft-$amount);
                        }else{
                            $super_available_overdraft=($super_available_overdraft+($super_available_credit-$amount));
                            $supresetov=1;
                        }
                        $super_available_credit= ($super_available_credit-$amount);
                    }
                    else{
                        $super_available_credit= $super_available_credit-$amount;
                    }
                    $sub_entity_total_overdraft = $dbh->queryOne('SELECT act_tot_overdraft_received FROM affiliate_credit
                                                WHERE act_aff_id = ' .$dbh->escape($_POST['jurisdiction']),true);
                    if($sub_entity_avail_credit < 0){
                        $max_overdraft=$sub_entity_total_overdraft-$sub_entity_avail_overdraft;
                        if($max_overdraft > 0){
                            if($sub_entity_avail_credit+$amount<0){
                                if($max_overdraft>$amount){
                                    $sub_entity_avail_overdraft+=$amount;
                                }
                                else{
                                    $sub_entity_avail_overdraft += $max_overdraft;
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

                    if(do_credit_send($_SESSION['jurisdiction_id'],$affclub,$_POST['amount'],$aff,$supresetov)){
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
                }
            }else{
                addError('',$lang->getLang('An amount must be entered'));
                showErrors();
            }
            break;

        case 'withdraw':
            $dbh->begin();
            if($_POST['amount']!=''){
            $maxCreditToWithdraw=($sub_entity_avail_credit>0?$sub_entity_avail_credit+$sub_entity_avail_overdraft : $sub_entity_avail_overdraft);
                if($_POST['amount']>$maxCreditToWithdraw){
                    addError('','Please choose a value smaller than '.getInDollars($maxCreditToWithdraw));
                    showErrors();
                }
                else{
                    $jursdictionClass=$_SESSION['jurisdiction_class'];
                    $jurisdictionPostClass='club';
                    $alljurisdictions=getAllEntitiesBetween($jursdictionClass,$jurisdictionPostClass,$affclub);
                    $transfer_type=20;
                    $alljurisdictions=array_reverse($alljurisdictions);
                    $keys = array_keys($alljurisdictions);
                    for ($i = 0; $i < count($keys); $i++) {
                        $cur = $alljurisdictions[$keys[$i]];
                        $next = $alljurisdictions[$keys[$i+1]];
                        if($next!=''){
                            if($cur==$affclub){
                                $distcredit=$dbh->queryOne('select jur_available_credit from jurisdiction where jur_id='.$next,true);
                                $distov=$dbh->queryOne('select jur_overdraft from jurisdiction where jur_id='.$next,true);
                                if(!is_int(adminUserCreditLog($_POST['note'],$transfer_type,$_SESSION['admin_id'],$next,$affclub,-$_POST['amount'],$_POST['jurisdiction']))){
                                    addError('','An error has occurred when adding the transaction record');
                                }
                                if(!affiliateCreditLog($_POST['note'],-$_POST['amount'],0,$sub_entity_avail_credit,$sub_entity_avail_overdraft,$distcredit,$distov,$_POST['jurisdiction'],$_SESSION['admin_id'],20)){
                                    addError('','An error has occurred when adding the affiliate transaction record');
                                }
                            }
                            elsE{
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
                    }


                    if(areErrors()){
                        $dbh->rollback();
                        showErrors();
                    }
                    else{
                          if($sub_entity_avail_credit<$amount){
                            if($sub_entity_avail_credit<0){
                                $sub_entity_avail_overdraft=($sub_entity_avail_overdraft-$amount);
                            }else{
                                $sub_entity_avail_overdraft=($sub_entity_avail_overdraft+($sub_entity_avail_credit-$amount));
                                $supresetov=1;
                            }
                        }
                         $sub_entity_avail_credit= $sub_entity_avail_credit-$amount;


                        if($super_available_credit<0){
                            /*if(($amount+$super_available_credit)>=0){
                                $max_overdraft=-$super_available_credit;
                            }
                            else{
                                $max_overdraft=$amount;
                            }*/
                            $max_overdraft=$amount;
                            if($super_available_overdraft!=$super_total_overdraft){
                                if(($super_available_overdraft-$super_available_credit) > $super_total_overdraft){
                                    $super_available_overdraft=$super_total_overdraft;
                                }
                                else{
                                    $super_available_overdraft -= $super_available_credit;
                                }
                            }
                        }
                        $super_available_credit +=$amount;

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
                addError('',$lang->getLang('An amount must be entered'));
                showErrors();
            }
            break;

        case 'odeposit':
            $dbh->begin();

            if($_POST['amount']!=''){
                if ( $super_entity_id != 1 && ( $_POST['amount'] > ($super_available_overdraft) )){
                    addError("", $lang->getLang("Amount to high. You have insufficient available overdraft.")); //invalid jurisdiction
                    showErrors();
                }
                else{
                    if((strtotime('now')-strtotime($super_overdraft_start)> (3600*24*7)) && $_SESSION['jurisdiction_class']!='casino' && $super_overdraft_start!=''){
                        addError("", $lang->getLang("Your overdraft has expired")); //invalid jurisdiction
                    }
                    else{
                    $transfer_type=21;
                        if(!is_int(adminUserCreditLog($_POST['note'],$transfer_type,$_SESSION['admin_id'],$_SESSION['jurisdiction_id'],$affclub,$_POST['amount'], $_POST['jurisdiction']))){
                            addError('',$lang->getLang('An error has occurred when adding the transaction record'));
                        }
                            if(!affiliateCreditLog($_POST['note'],0,$_POST['amount'],$sub_entity_avail_credit,$sub_entity_avail_overdraft,$super_available_credit,$super_available_overdraft,$_POST['jurisdiction'],$_SESSION['admin_id'],21)){
                                   addError('',$lang->getLang('An error has occurred when adding the affiliate transaction record'));
                            }
                }
                if(areErrors()){
                    $dbh->rollback();
                    showErrors();
                }
                else{
                    $totOvd = ($_POST['totOvd']==1? 1:0);
                    if(do_overdraft_send($_SESSION['jurisdiction_id'],$_POST[$_POST['jurisdiction']."_cid"],$_POST['amount'],$_POST['jurisdiction'],$totOvd)){
                        if($_POST['from']!=''){
                            if(changeOverdraftDate($_POST['jurisdiction'],$_POST['from'])){
                                addResult('',$lang->getLang('Successfully updated the overdraft date to %', $_POST['from']));
                                doAdminUserLog($_SESSION['admin_id'],'overdraft modify','Changed overdraft time to '.$dbh->escape($_POST['from']).' for affiliate '.$_POST['jurisdiction'],'NULL',getIpAddress());
                            }
                            else{
                                showErrors();
                            }
                        }
                        $_SESSION['jurisdiction_credit']=$_SESSION['jurisdiction_credit']-$_POST['amount'];
                        $dbh->commit();
                        addResult('',$lang->getLang('Successfully deposited overdraft %', getInDollars($_POST['amount'])));
                        showResult();
                    }
                    else{
                        $dbh->rollback();
                        showErrors();
                    }
                }
                }
            }else{
                addError('',$lang->getLang('An amount must be entered'));
                showErrors();
            }
            break;

        case 'owithdraw':
            $dbh->begin();

            $totOvd = ($_POST['totOvd']==1? 1:0);
            if($_POST['amount']!=''){
                if($_POST['amount']>$sub_entity_avail_overdraft){
                    addError('','Please choose a value smaller than '.getInDollars($sub_entity_avail_overdraft));
                    showErrors();
                }
                if($_POST['amount']>$super_total_overdraft-$super_available_overdraft){
                    addError('','Please choose a value smaller than '.getInDollars($super_total_overdraft-$super_available_overdraft));
                    showErrors();
                }
                else{
                    $transfer_type=22;
                            if(!is_int(adminUserCreditLog($_POST['note'],$transfer_type,$_SESSION['admin_id'],$_SESSION['jurisdiction_id'],$affclub,-$_POST['amount'], $_POST['jurisdiction']))){
                                addError('','An error has occurred when adding the transaction record');
                            }
                                if(!affiliateCreditLog($_POST['note'],0,-$_POST['amount'],$sub_entity_avail_credit,$sub_entity_avail_overdraft,$super_available_credit,$super_available_overdraft,$_POST['jurisdiction'],$_SESSION['admin_id'],22)){
                                    addError('','An error has occurred when adding the affiliate transaction record');
                        }
                    if(areErrors()){
                        $dbh->rollback();
                        showErrors();
                    }
                    else{
                        $reset = ($sub_entity_avail_overdraft-$_POST['amount']==0? 1:0);
                        if(do_overdraft_return($_SESSION['jurisdiction_id'],$_POST[$_POST['jurisdiction']."_cid"],$_POST['amount'],$_POST['jurisdiction'],$reset,$totOvd)){
                            $_SESSION['jurisdiction_credit']=$_SESSION['jurisdiction_credit']+$_POST['amount'];
                            if($_POST['from']!='' || $_POST['ovReset']){
                                if(changeOverdraftDate($_POST['jurisdiction'],((isset($_POST['ovReset'])) ? 'NULL': $_POST['from']))){
                                    if(isset($_POST['ovReset']))
                                    {
                                        addResult('',$lang->getLang('Successfully reset the overdraft date'));
                                        doAdminUserLog($_SESSION['admin_id'],'overdraft modify','Reset overdraft time for affiliate '.$_POST['jurisdiction'],'NULL',getIpAddress());
                                    }
                                    else{
                                        addResult('',$lang->getLang('Successfully updated the overdraft date to %', $_POST['from']));
                                        doAdminUserLog($_SESSION['admin_id'],'overdraft modify','Changed overdraft time to '.$dbh->escape($_POST['from']).' for affiliate '.$_POST['jurisdiction'],'NULL',getIpAddress());
                                    }
                                }
                                else{
                                    showErrors();
                                }
                            }
                            $dbh->commit();
                            addResult('',$lang->getLang('Successfully withdrawn overdraft %' , getInDollars($_POST['amount'])));
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
                addError('',$lang->getLang('An amount must be entered'));
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
    <div class="tip"><?=$lang->getLang("Time taken to execute your request")?>:<?=$duration?></div>
<? }
$onlyAff=(isset($_GET['jurisdiction']) ? $_GET['jurisdiction'] : $_POST['onlyAff']);

?>
    <input type='hidden' name='onlyAff' value='<?=$onlyAff?>' >
    <table><tr><td class="label"><?=$lang->getLang("Select Affiliate")?></td><td>
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
                    "overdraft_time"=>$row['act_overdraft_start_time'],
                    "tot_overdraft_received"=>$row['act_tot_overdraft_received']
                );
            }
            ?>
            <select name='jurisdiction' id='jurisdiction'>
                <?
                foreach($clubs as $k=>$v){
                    ?>
                    <optgroup label="<?=$k?>">
                        <? foreach ($v as $k2=>$v2){
                            if($onlyAff!=''){
                                if($onlyAff==$k2){
                                    ?>
                                    <option value='<?=$k2?>'  selected="selected" ><?=$v2['username']?></option>
                                    <?
                                }
                            }
                            else{ ?>
                            <option value='<?=$k2?>' <?if($_POST['jurisdiction']==$k2 ){?> selected="selected" <? } ?>><?=$v2['username']?></option>
                        <?
                            }
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
                    <table id="<?=$k2?>Info" class="hidden" style="margin-left: 200px;">
                        <input type="hidden" id="<?=$k2?>OAmnt" name="<?=$k2?>OAmnt" value="<?=$v2['overdraft']?>" class='affInfo' disabled="disabled" />
                        <input type="hidden" name="<?=$k2?>_cid" value="<?=$v2['club_id']?>" class='affInfo' disabled="disabled" />
                        <input type="hidden" id="<?=$k2?>Amnt" name="<?=$k2?>Amnt" value="<?=$v2['availableCredit']?>" class='affInfo' disabled="disabled" />
                        <tr><td class="label"><?=$lang->getLang("Club name")?></td><td><?=$v2['club']?></td></tr>
                        <? if($depositWithdraw) { ?>
                        <tr><td class="label"><?=$lang->getLang("Credit")?></td><td><?=getInDollars($v2['availableCredit'])?></td></tr>
                        <? } if($overdraftDepositWithdraw) { ?>
                        <tr><td class="label"><?=$lang->getLang("Overdraft")?></td><td><?=getInDollars($v2['overdraft'],2,false,true)?>
                               </td></tr>
                        <tr><td class="label"><?=$lang->getLang("Total Overdraft Received")?></td><td><?=getInDollars($v2['tot_overdraft_received'],2,false,true)?> </td></tr>
                        <tr><td class="label"><?=$lang->getLang("Overdraft start time")?></td><td><?=($v2['overdraft_time']=='0000-00-00 00:00:00') ? 'none': $v2['overdraft_time'] ?></td></tr>
                        <? } ?>
                    </table>
                <? }
            } ?>
        </td>
    </tr>
    <tr>
        <td class="label"><?=$lang->getLang("Select operation")?></td>
        <td>
            <select name='operation' id='operation'>
                <? if($depositWithdraw) {?>
                <option value='deposit' <?=($_POST['operation']=='deposit'? 'selected="selected"' : '' )?>><?=$lang->getLang("Deposit")?></option>
                <option value='withdraw' <?=($_POST['operation']=='withdraw'? 'selected="selected"' : '' )?>><?=$lang->getLang("Withdraw")?></option>
                 <? } if($overdraftDepositWithdraw) { ?>
                <option value='odeposit' <?=($_POST['operation']=='odeposit'? 'selected="selected"' : '' )?>><?=$lang->getLang("Deposit Overdraft")?></option>
                <option value='owithdraw' <?=($_POST['operation']=='owithdraw'? 'selected="selected"' : '' )?>><?=$lang->getLang("Withdraw Overdraft")?></option>
                <option value='reset' <?=($_POST['operation']=='reset'? 'selected="selected"' : '' )?>><?=$lang->getLang("Reset overdraft")?></option>
                <? } ?>
            </select>
        </td>
    </tr>
    <tr id='amountTr'>
        <td class="label"><?=$lang->getLang("Amount")?> ( <?=((isset($_SESSION['currency_html']) && $_SESSION['currency_html']!='') ? $_SESSION['currency_html'] : "&euro;" )?> )</td>
        <td>
            <input type="number" name='amount' id='amount' min="0" step="0.01" value="<?=$_POST['amount']?>" autocomplete="off" />
          <!--  <span id='totOvd'><input type="checkbox" name='totOvd' value='1' > <?=$lang->getLang("Do not modify total overdraft")?></span> -->
        </td>
    </tr>
    <tr id='fromTr'>
        <td class="label"><?=$lang->getLang("Overdraft new start day")?></td>
        <td>
            <input type="text" name='from' id='from' autocomplete="off" />
        </td>
    </tr>

<!--    <tr id='ovResetTr'>
        <td class="label"><?/*=$lang->getLang("Remove overdraft time")*/?> </td>
        <td>
            <input type="checkbox" name='ovReset' id='ovReset' />
        </td>
    </tr>-->
    <tr>
        <td class=label><?=$lang->getLang("Note")?></td>
        <td class="content"><textarea  name="note"></textarea></td>
    </tr>

    <tr>
        <td colspan="2"><input type="submit" value="<?=$lang->getLang("Submit")?>" /> </td>
    </tr>
    <? if($onlyAff!=''){ ?>
    <tr>
        <td colspan="2">
      <?
        $affInfo=$dbh->queryRow('Select pun_username,pun_id from punter where pun_aff_id='.$dbh->escape($onlyAff)." AND pun_aus_id IS NOT NULL");
        $affname=$affInfo['pun_username']; ?>
        <?=getClassLink(refPage('customers/customer_view') . "&customer_id=".$affInfo['pun_id'], $lang->getLang("Go back to this $affname 's profile "))?>
        </td>
    </tr>
        <? } ?>
</table>
</form>
<? }
    else { ?>
    <div class="error"><?=$lang->getLang("You're not allowed to see this page")?></div>
<? } ?>
<?php

function changeOverdraftDate($affiliate,$newdate){
    global $dbh,$lang;
    if (DateTime::createFromFormat('Y-m-d', $newdate) !== FALSE || $newdate=='NULL') {
    $sql="Update affiliate_credit set act_overdraft_start_time=".$dbh->prepareString($newdate)." Where act_aff_id=".$dbh->escape($affiliate);
        try{
        $result=$dbh->exec($sql);
        if($result!=1){
            throw new Exception('Query error');
        }
    }
    catch(Exception $e){
        addError('',$lang->getLang("The following errors have occurred:").$e->getMessage().'. The database was not updated.');
        return false;
    }
    }
    else{
        addError('',$lang->getLang("Invalid date!"));
        return false;
    }
    return true;
}

function getJurisdictionsAffiliates($jur_id) {
    global $dbh;
    $jur_class=$_SESSION['jurisdiction_class'];
    $sql = "select c.jur_name club_name,c.jur_id club_id,d.jur_name district_name,aus_username,act_credits,act_overdraft,aff_id,act_overdraft_start_time,act_tot_overdraft_received
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
    $result = $dbh->exec( $sql, false,true );
    return $result;

}

function do_credit_send($super_ent_id, $aff_club_id, $amount, $aff_id,$supresetov=0){
    global $dbh,$super_available_credit,$super_available_overdraft ,$super_total_overdraft,$super_overdraft_start,$sub_entity_avail_overdraft;

    $super_credit_updated= update_jurisdiction_credits($super_ent_id, $super_available_credit,$super_available_overdraft,$super_total_overdraft,$supresetov);
    $sub_credit_updated   = add_affiliate_available_credit($aff_id, $amount,$sub_entity_avail_overdraft);
    if ( ! ( TRUE == $super_credit_updated &&  TRUE == $sub_credit_updated) ){
        return false;
    }
    return TRUE;
}

function do_credit_return($super_ent_id, $aff_club_id, $amount, $aff_id){
    global $dbh,$super_available_credit,$super_available_overdraft ,$super_total_overdraft,$super_overdraft_start,$sub_entity_avail_overdraft;
    $super_credit_updated = subtract_affiliate_credit($aff_id, $amount,$sub_entity_avail_overdraft);
    $sub_credit_updated   = update_jurisdiction_credits($super_ent_id, $super_available_credit,$super_available_overdraft,$super_total_overdraft,0);
    if ( ! ( TRUE == $super_credit_updated &&  TRUE == $sub_credit_updated) ){
        return false;
    }
    return TRUE;
}

function do_overdraft_send($super_ent_id, $aff_club_id, $amount, $aff_id,$totOvd){
    global $dbh;
    $super_credit_updated = subtract_available_overdraft($super_ent_id, $amount);
    $sub_credit_updated   = add_affiliate_available_overdraft($aff_id, $amount,$totOvd);
    if ( ! ( TRUE == $super_credit_updated &&  TRUE == $sub_credit_updated) ){
        return false;
    }
    return TRUE;
}

function do_overdraft_return($super_ent_id, $aff_club_id, $amount, $aff_id,$reset=0,$totOvd=0){
    global $dbh;

   $sub_credit_updated = subtract_affiliate_overdraft($aff_id, $amount,$reset,$totOvd);
    $super_credit_updated  = add_available_overdraft($super_ent_id, $amount);
    if ( ! ( TRUE == $super_credit_updated &&  TRUE == $sub_credit_updated) ){
        return false;
    }
    return TRUE;
}



function adminUserCreditLog($note='',$transactionType,$admId,$admJurId,$admFinalJurId,$admAmount,$aff,$last=false)
{
    $juravailov=0;
    $juravailcred=0;
    if($transactionType=='19' || $transactionType=='20'){
        $amount=$admAmount;
        $overdraft=0;

        if($last&&$transactionType=='19'){
            $juravailcred +=$admAmount;
        }
    }
    else{
        $amount=0;
        $overdraft=$admAmount;

        if($last){
            $juravailov +=$admAmount;
        }
    }
    global $dbh,$trnsId;
    $sql="Insert into admin_user_transaction (att_note,
                                              att_tty_id,
                                              att_aus_id,
                                              att_aff_id,
                                              att_transaction_id,
                                              att_jur_id,
                                              att_final_jur_id,
                                              att_amount,
                                              att_overdraft,
                                              att_jur_avail_credit,
                                              att_jur_avail_overdraft,
                                              att_jur_final_avail_credit,
                                              att_jur_final_avail_overdraft )
                                        Values (
                                                ".$dbh->prepareString($note).",
                                                ".$dbh->prepareString($transactionType).",
                                                ".$dbh->prepareString($admId).",
                                                ".$dbh->escape($aff).",
                                                ".$dbh->prepareString($trnsId).",
                                                ".$dbh->prepareString($admJurId).",
                                                ".$dbh->prepareString($admFinalJurId).",
                                                ".$dbh->prepareString($amount).",
                                                ".$dbh->prepareString($overdraft).",
                                                (SELECT jur_available_credit from jurisdiction where jur_id =".$dbh->prepareString($admJurId).")+$juravailcred,
                                                (SELECT jur_overdraft from jurisdiction where jur_id =".$dbh->prepareString($admJurId).")+$juravailov,
                                                (SELECT act_credits from affiliate_credit where act_aff_id =".$dbh->escape($aff)."),
                                                (SELECT act_overdraft from affiliate_credit where act_aff_id =".$dbh->escape($aff).")";
                                            $sql.=")";
    $result=$dbh->exec($sql);

    return  $result;
}



function affiliateCreditLog($note='',$amount=0,$overdraft=0,$availableCredit,$availableoverdraft,$otheravaiablecredit,$otheravailableoverdraft,$aff_id,$aus_id,$transactionType){
    global $dbh,$trnsId;
    $sql="Insert into affiliate_transaction ( atr_notes,";
    if($amount!=0){ $sql.="atr_amount,"; }
    if($overdraft!=0){ $sql.="atr_overdraft,"; }
    $sql.="atr_available_credit,
           atr_available_overdraft,
           atr_other_available_credit,
           atr_other_available_overdraft,
           atr_aff_id,
           atr_aus_id,
           atr_transaction_id,
           atr_tty_id)
           Values (".$dbh->prepareString($note).",";
    if($amount!=0){ $sql.=$dbh->prepareString($amount).","; }
    if($overdraft!=0){ $sql.=$dbh->prepareString($overdraft).","; }
    $sql.=$dbh->prepareString($availableCredit).",
        ".$dbh->prepareString($availableoverdraft).",
        ".$dbh->prepareString($otheravaiablecredit).",
        ".$dbh->prepareString($otheravailableoverdraft).",
        ".$dbh->prepareString($aff_id).",
        ".$dbh->prepareString($aus_id).",
        ".$dbh->prepareString($trnsId).",".$transactionType.")";
    return $dbh->exec($sql);

}
?>