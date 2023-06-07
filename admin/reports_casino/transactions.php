<?if(isset($_SESSION['admin_id'])) {
    include('header.inc');
    $defaultmonth= (isset($_POST['monthHelper'])) ? $_POST['monthHelper'] : false;
    $defaultweek= (isset($_POST['weekHelper'])) ? $_POST['weekHelper'] : false;
    $type= (isset($_POST['type'])) ? $_POST['type'] : 0;
    $with= (isset($_POST['with'])) ? $_POST['with'] : 0;
    $jur_id = getPostGetVar('jur_id');
    $jur_id = (int)$jur_id;
    $date_start = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d 00:00", time() - (60 * 60 * 24));
    $date_end = (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d 23:59", time());
    $sess_jur_id = $_SESSION['jurisdiction_id'];
    $datetime1 = new DateTime($date_start);
    $datetime2 = new DateTime($date_end);
    $interval = $datetime1->diff($datetime2);
    $max_rows= (isset($_POST['max_rows'])) ? $_POST['max_rows'] : 100;
    ?>
    <script type="text/javascript">
        $(function() {
            $("#date_start").datetimepicker({
                numberOfMonths: 1,
                changeMonth: true,
                changeYear: true,
                showWeek: true,
                firstDay: 1,
                onSelect: function (selectedDate) {
                    $("#date_end").datetimepicker("option", "minDate", selectedDate);
                    $( "#monthHelper, #weekHelper" ).val('');
                    if($("#date_end").val()!==''){
                        diff = 0;
                        diff = Math.floor(($("#date_end").datetimepicker('getDate').getTime() - $("#date_start").datetimepicker('getDate').getTime()) / 86400000); // ms per day
                        if(diff>31){
                            jAlert('Please choose an interval of maximum 31 days ');
                        }
                    }
                }
            });
            $("#date_end").datetimepicker({
                numberOfMonths: 1,
                changeMonth: true,
                changeYear: true,
                showWeek: true,
                firstDay: 1,
                onSelect: function (selectedDate) {
                    $( "#monthHelper, #weekHelper" ).val('');
                    if($("#date_start").val()!==''){
                        diff = 0;
                        diff = Math.floor(($("#date_end").datetimepicker('getDate').getTime() - $("#date_start").datetimepicker('getDate').getTime()) / 86400000); // ms per day
                        if(diff>31){
                            jAlert('Please choose an interval of maximum 31 days ');
                        }
                    }
                }
            });

         $('#monthHelper').select2({
            width: "158px"
        });

        });

        $(document).on('change', '#monthHelper', function() {
            var d = new Date($(this).val());
            createMonthHelper(d);
        });
        $(document).on('change', '#weekHelper', function() {
            createWeekHelper($(this).val());
        });
        function createWeekHelper(dateobj){
            dates=dateobj.split('~');
            firstDay=dates[0];
            lastDay=dates[1];
            $( "#date_start" ).datetimepicker('setDate', firstDay);
            $( "#date_end" ).datetimepicker('setDate', lastDay);
            $('#monthHelper').val('');
        }

        function createMonthHelper(dateobj){
            var firstDay = new Date(dateobj.getFullYear(), dateobj.getMonth(), 1);
            $( "#date_start" ).datetimepicker('setDate', firstDay);
            var lastDay = new Date(dateobj.getFullYear(), dateobj.getMonth() + 1, 0 , 23,59);
            $( "#date_end" ).datetimepicker('setDate', lastDay);
            $('#weekHelper').val('');
        }
        function duration(duration,divId)
        {

            $('#'+divId).empty().append('<?=$lang->getLang("Time taken to execute your request")?>: '+duration);
        }

    </script>
    <?php


    $sql =  "SELECT jur_code, jur_name, jur_available_credit,jur_class,jur_overdraft,jur_tot_overdraft_received,jur_overdraft_start_time,cur_code_for_web,
             @affiliateId := IF(jur_class = 'club', (SELECT aff_id FROM admin_user, affiliate WHERE aus_jurisdiction_id = jur_id AND aus_id = aff_aus_id AND aff_is_club = 1), 0) as affiliateid " .
        "FROM jurisdiction LEFT JOIN currency ON jur_currency=cur_id " .
        "WHERE jur_id = $jur_id ";
    $rs = $dbh->exec($sql);
    $num_rows = $rs->getNumRows();

    if ( 1 == $num_rows ){
        $row = $rs->next();
        $jur_name               = $row['jur_name'];
        $entity_name            = $jur_name;
        $jur_available_credit   = $row['jur_available_credit'];
        $jur_overdraft          = $row['jur_overdraft'];
        $jur_overdraft_received = $row['jur_tot_overdraft_received'];
        $jur_overdraft_time     = $row['jur_overdraft_start_time'];
        $jur_class=$row['jur_class'];
        $aff_id=$row['affiliateid'];
        $symbol=$row['cur_code_for_web'];
        if(!is_null($aff_id) && $aff_id > 0){
            $affInfo=$dbh->queryRow('Select * from affiliate_credit where act_aff_id='.$dbh->escape($aff_id));
            $actCredits=$affInfo['act_credits'];
            $actOverdraft=$affInfo['act_overdraft'];
            $actOverdraftReceived=$affInfo['act_tot_overdraft_received'];
            $actOverdraftTime=$affInfo['act_overdraft_start_time'];
        }
        ob_start();
        ?>
        <div style="font-size:8pt;color:#7f7f7f">
            <?=$lang->getLang("Available Credit")?>: <?=getInDollars(((!is_null($aff_id)&& $aff_id>0) ? $actCredits : $jur_available_credit))?>
            <br /><?=$lang->getLang("Available Overdraft")?>:<?=getInDollars(((!is_null($aff_id)&& $aff_id>0) ? $actOverdraft : $jur_overdraft))?>
            <br /><?=$lang->getLang("Total overdraft received")?>:<?=getInDollars(((!is_null($aff_id)&& $aff_id>0) ? $actOverdraftReceived : $jur_overdraft_received))?>
            <br /><?=$lang->getLang("Overdraft start time")?>:<?=(((!is_null($aff_id) && $aff_id>0) ? $actOverdraftTime : $jur_overdraft_time))?>
        </div>
        <?
        $acc_details = ob_get_clean();
    }
    else
    {
        dieWithError($lang->getLang("Account not found or you do not have the proper privileges to view the account"));
    }
    ?>
    <div class="bodyHD">
        <?=$lang->getLang("Credit Transactions Report")?><br />
        <span style="color:#BF0E38;font-size:10pt"><?=$jur_name?></span>
    </div><br>
    <?=$acc_details;?>
    <br />
    <?form_head()?>
    <input type="hidden" name="form_submitted" value="yes" />
    <input type="hidden" name="doQuery" value="yes" />
    <input type="hidden" name="search_type"  value="date_range" />
    <input type="hidden" name="jur_id"  value="<?=$jur_id;?>" />
    <table cellpadding=4 cellspacing=1 border=0>
        <tr>
            <td colspan="2" class="formheading" >
                <?
                if ( isset($_POST['doQuery']) )
                {
                    echo $lang->getLang("Search Again");  // Search Again
                }
                else
                {
                    echo $lang->getLang("Transaction Search");  //Transaction Search
                }
                ?>
            </td>
        </tr>
        <tr valign=top>
            <td class=label><?=$lang->getLang("Choose month")?></td>
            <td class=content>
                <?=month_select_box($defaultmonth)?>
            </td>
        </tr>
        <tr valign=top>
            <td class=label><?=$lang->getLang("Choose week")?></td>
            <td class=content>
                <?=week_select_box($defaultweek)?>
            </td>
        </tr>
        <tr valign=top>
            <td class=label><?=$lang->getLang("From")?></td>
            <td <? form_td_style('start_date');?>>
                <input type='text' name='date_start' id='date_start'  value="<?=$date_start?>" >
                <?=err_field('start_date');?>
            </td>
        </tr>
        <tr valign=top>
            <td class=label> <?=$lang->getLang("Until")?> </td>
            <td <? form_td_style('end_date');?>>
                <input type='text' name='date_end' id='date_end' value="<?=$date_end?>">
                <?=err_field('end_date');?>
            </td>
        </tr>
        <tr valign=top>
            <td class=label> <?=$lang->getLang("Admin user")?> </td>
            <td>
                <input type='text' name='admin_user'  value="<?=$_POST['admin_user']?>" placeholder="( Optional )">
                <?=err_field('end_date');?>
            </td>
        </tr>
        <? if($jur_class=='club') { ?>
            <tr valign=top>
                <td class=label><?=$lang->getLang("Type")?></td>
                <td>
                    <select name='type' id='type'>
                        <option value="0" <?=$type==0 ? 'selected': '' ?> ><?=$lang->getLang("All")?></option>
                        <option value="1" <?=$type==1 ? 'selected': '' ?> ><?=$lang->getLang("Players")?></option>
                        <option value="2" <?=$type==2 ? 'selected': '' ?> ><?=$lang->getLang("Jurisdictions")?></option>
                        <option value="3" <?=$type==3 ? 'selected': '' ?> ><?=$lang->getLang("Virtual")?></option>
                        <option value="4" <?=$type==4 ? 'selected': '' ?> ><?=$lang->getLang("Totem")?></option>
                    </select>
                </td>
            </tr>
        <? } ?>
        <tr valign=top>
            <td class=label><?=$lang->getlang("Max records per page")?></td>
            <td class=content>
                <select name="max_rows">
                    <option <?php if($max_rows == 100){?> selected <?php }?> value="100" >100</option>
                    <option <?php if($max_rows == 200){?> selected <?php }?> value="200" >200</option>
                    <option <?php if($max_rows == 500){?> selected <?php }?> value="500" >500</option>
                    <option <?php if($max_rows == 1000){?> selected <?php }?> value="1000" >1000</option>
                </select>
            </td>
        </tr>
        <? if(check_access('search_entity_transactions_filters')) { ?>
        <tr valign=top>
            <td class=label><?=$lang->getLang("Transactions made with")?></td>
            <td class=content>
                <select name="with">
                    <option value="0" <?=($with==0 ? 'selected' : '') ?> ><?=$lang->getLang('Under')?></option>
                    <option value="1" <?=($with==1 ? 'selected' : '') ?> ><?=$lang->getLang('Up')?></option>
                    <option value="2" <?=($with==2 ? 'selected' : '') ?> ><?=$lang->getLang('All')?></option>
                </select>
            </td>
        </tr>
        <? } ?>
        <tr>
            <td>&nbsp;</td>
            <td><button  class="entitybtn"><?=$lang->getLang('Submit')?></td>
        </tr>
    </table>
    </form>
    <br />
    <a href="<?=refPage('reports_casino/entity_report')?>"> <?=$lang->getlang("Back to Entities Credit Report")?></a>
    <? if ( isset($_POST['doQuery']) )   // do the search
    {
   /*  $date_start.=":00";
     $date_end.=":59";*/
        if($interval->days > 31){
            addError('',$lang->getLang('Please choose an interval of maximum 31 days'));
            showErrors();
        }
        else{
            $start_date = $date_start;
            $end_date   = $date_end;
            if($aff_id!='' &&  $aff_id > 0 ){
                searchAffiliateTransaction();
            }
            else{
                searchAdminTransaction();
            }
        }
    }
    ?>

    <?include('footer.inc'); ?>
<? }
else{
    header("Location: /?err=expired");
    die();
}
function searchAdminTransaction(){
    global $dbh,
           $lang,
           $date_start,
           $date_end,
           $jur_id,
           $jur_class,
           $type,
           $with,
           $max_rows;


    //TODO to eleminate in future this date;
    if(defined('TRANSACTION_DATE')){
        $update=TRANSACTION_DATE;
    }
    else{
        $update=date('Y-m-d');
    }
    $starttime = microtime(true);
    $sql1="Select att_id,
                att_time,
                att_aus_id,
                att_note,
                att_transaction_id,
                IF (att_tty_id IS NOT NULL, (select tty_description from transaction_type where tty_id = att_tty_id), 'N') tty_description,
                IF (att_aus_id IS NOT NULL, (select aus_username from admin_user where aus_id = att_aus_id), 'N') aus_username,
                IF (att_aus_final_id IS NOT NULL, (select aus_username from admin_user where aus_id = att_aus_final_id), 'N') aus_final_username,
                IF (att_pun_id IS NOT NULL, (select pun_username from punter where pun_id = att_pun_id), 'N') pun_username,
                IF (att_jur_id IS NOT NULL, (select jur_name from jurisdiction where jur_id=att_jur_id), 'N') first_jurisdiction,
                IF (att_final_jur_id IS NOT NULL, (select jur_name from jurisdiction where jur_id=att_final_jur_id), 'N') second_jurisdiction,
                IF (att_final_jur_id IS NOT NULL, (select jur_class from jurisdiction where jur_id=att_final_jur_id), 'N') second_jurisdiction_class,
                IF (att_ttm_id IS NOT NULL, (select ttm_code from totem where ttm_id=att_ttm_id), 'N') ttm_code,
                att_amount,
                att_overdraft,
                att_jur_avail_credit,
                att_jur_final_avail_credit, ";
    if($jur_class=='club'){
        $sql1.=" IF ((att_jur_id = ".$jur_id." AND att_amount < 0 AND att_tty_id <> 3) OR
                    (att_jur_id = ".$jur_id." AND att_overdraft < 0 AND att_tty_id <> 22)  OR
                    (att_final_jur_id = ".$jur_id." AND att_amount < 0) OR (att_tty_id = 10), 'CASH-OUT', 'CASH-IN') type
                FROM admin_user_transaction ";
        if($_POST['admin_user']!=''){
            $sql1.=" join admin_user on aus_id=att_aus_id and aus_username=".$dbh->prepareString($_POST['admin_user']);
        }

    }
    else{
        $sql1.=" IF ((att_jur_id = ".$jur_id." AND att_amount > 0 AND att_tty_id <> 3) OR
                    (att_jur_id = ".$jur_id." AND att_overdraft > 0 AND att_tty_id <> 22)  OR
                    (att_final_jur_id = ".$jur_id." AND att_amount < 0) OR (att_tty_id = 10), 'CASH-OUT', 'CASH-IN') type
                FROM admin_user_transaction ";
        if($_POST['admin_user']!=''){
            $sql1.=" join admin_user on aus_id=att_aus_id and aus_username=".$dbh->prepareString($_POST['admin_user']);
        }
    }
    $sql1.=" WHERE  1=1 ";
    if($with==0){
        $sql1.="AND att_jur_id=".$jur_id;
    }
    elseif($with==1){
        $sql1.=" AND att_final_jur_id=".$jur_id;
    }else{
        $sql1.=" AND (att_jur_id=".$jur_id."
               OR att_final_jur_id=".$jur_id.")";
    }

    if($type==1 && strtotime($date_start.":00") > strtotime($update)){
        $sql1.=" AND att_pun_id IS NOT NULL AND att_tty_id not in(25,26) ";
    }
    if($type==2 && strtotime($date_start.":00") > strtotime($update)){
        $sql1.=" AND att_pun_id IS NULL ";
    }
    if($type==3){
        $sql1.=" AND att_tty_id in(25,26)";
    }
    if($type==4){
        $sql1.=" AND att_ttm_id >0 ";
    }
    $sql1 .= " AND att_time BETWEEN '$date_start:00' AND '$date_end:59' ";

    if((strtotime($date_start)<=strtotime($update)) ||(($type==0 || $type==2) && strtotime($date_start) > strtotime($update))){
        $sql1.=" UNION ALL
               SELECT ptr_id att_id,
               ptr_start att_time,
               ptr_admin_user_id att_aus_id,
               ptr_note att_note,
               ptr_uniqueid att_transaction_id,
               (select tty_description from transaction_type where tty_id = ptr_tty_id) tty_description,
               IF (ptr_admin_user_id IS NOT NULL, (select aus_username from admin_user where aus_id = ptr_admin_user_id), 'N') aus_username,
               'N' aus_final_username,
               'N' pun_username,
               'N' first_jurisdiction,
               'N' second_jurisdiction,
               'N' second_jurisdiction_class,
               'N' ttm_code,
               ptr_amount att_amount,
               0 att_overdraft,
               ptr_available_credit att_jur_avail_credit,
               0 att_jur_final_avail_credit,
               'N' type
               FROM processor_transaction, admin_user  WHERE  1=1
               AND (ptr_status !='A')
               AND ptr_admin_user_id = aus_id  ";
        $sql1 .= " AND ptr_admin_user_id  IN (SELECT aus_id FROM admin_user where aus_jurisdiction_id=".$jur_id.")";
        $sql1 .= " AND ptr_start >= '$date_start:00' AND ptr_end <= '$date_end:59'";
        if($_POST['admin_user']!=''){
            $sql1.=" and aus_username=".$dbh->prepareString($_POST['admin_user']);
        }
    }

    if($jur_class=='club'){
        $sql2=" SELECT ctr_id att_id,
                    ctr_time att_time,
                    ctr_aus_id att_aus_id,
                    ctr_notes att_note,
                    ctr_transaction_num att_transaction_id,
                    IF (ctr_tty_id IS NOT NULL, (select tty_description from transaction_type where tty_id = ctr_tty_id), 'N') tty_description,
                    IF (ctr_aus_id IS NOT NULL, (select aus_username from admin_user where aus_id = ctr_aus_id), 'N') aus_username,
                    'N' aus_final_username,
                    IF (ctr_pun_id IS NOT NULL, (select pun_username from punter where pun_id = ctr_pun_id), 'N') pun_username,
                    IF (ctr_aus_id IS NOT NULL, (select jur_name from jurisdiction, admin_user where jur_id = aus_jurisdiction_id AND aus_id = ctr_aus_id), 'N') first_jurisdiction,
                    'N' second_jurisdiction,
                    'N' second_jurisdiction_class,
                    'N' ttm_code,
                    ctr_amount att_amount,
                    0 att_overdraft,
                    0 att_jur_avail_credit,
                    ctr_balance_available att_jur_final_avail_credit,
                    IF (ctr_tty_id in (19, 7,25), 'CASH-IN', 'CASH-OUT') type
                    FROM customer_transaction,admin_user

                    WHERE 1=1 AND aus_jurisdiction_id=".$jur_id;
        if($type==3){
            $sql2.=" AND ctr_tty_id in(25,26) ";
        }
        if($type==4){
            $sql2.=" AND ctr_ttm_id > 0 ";
        }
        $sql2.=" AND  aus_id = ctr_aus_id
        AND ctr_time BETWEEN '$date_start:00' AND '$date_end:59' ";
        if($_POST['admin_user']!=''){
            $sql2.=" and aus_username=".$dbh->prepareString($_POST['admin_user']);
        }
    }

    if(strtotime($date_start)<= strtotime($update)){
        if(($type==0 || $type==4) && $jur_class=='club'){
            $sql=$sql1." UNION ALL ".$sql2;
        }
        elseif($type==1 || $type==3){
            $sql=$sql2;
        }
        elsE{
            $sql=$sql1;
        }
    }
    else{
        $sql=$sql1;
    }
    if($type!=1){
        $sql .= " ORDER BY att_id, att_time";
    }

    if(!areErrors()){
        $rs=$dbh->doCachedQuery($sql,0);
        $cols_arr = array("Time","Transaction Type","Admin","Transaction Id","In","Out","From","Available Credit","To","Available Credit ","Note");
        $val_fmt_arr = array (
            "Time"                          =>  'return $rec["att_time"];',
            "Note"                          =>  'return $rec["att_note"];',
            "Transaction Type"              =>  'return $rec["tty_description"];',
            "Admin"                         =>  'return getClassLink("/admin_users/admin_user_edit.php?id={$rec["att_aus_id"]}", $rec["aus_username"]);',
            "Transaction Id"                =>  'return $rec["att_transaction_id"];',
            "From"                          =>  'return ($rec["first_jurisdiction"]);',
            "To"                            =>  'return returnWithAdmin($rec["second_jurisdiction"],$rec["pun_username"],$rec["second_jurisdiction_class"],$rec["ttm_code"]);',
            "In"                            =>  'return ($rec["type"]=="CASH-IN"? ($rec["att_amount"]!=0? getInDollars(abs($rec["att_amount"])) : getInDollars(abs($rec["att_overdraft"]))) : 0 ) ;',
            "Out"                           =>  'return ($rec["type"]=="CASH-OUT"? ($rec["att_amount"]!=0? getInDollars(abs($rec["att_amount"])) : getInDollars(abs($rec["att_overdraft"]))) : 0 );',
            "Available Credit"              =>  'return "Before:".(($rec["first_jurisdiction"]=="casino" ) ? " Unlimited" : getInDollars($rec["att_jur_avail_credit"]))."<br / >After:&nbsp;&nbsp;&nbsp;".(($rec["first_jurisdiction"]=="casino" ) ? " Unlimited" :getInDollars($rec["att_jur_avail_credit"] - $rec["att_amount"] )) ;',
            "Available Credit "             =>  'return "Before:".(($rec["second_jurisdiction"]=="casino" ) ? " Unlimited" : getInDollars($rec["att_jur_final_avail_credit"]))."<br / >After:&nbsp;&nbsp;&nbsp;".(($rec["second_jurisdiction"]=="casino" ) ? " Unlimited" : getInDollars($rec["att_jur_final_avail_credit"] + $rec["att_amount"] )) ;'
        );

        $cell_fmt_arr = array ();
        $numrow = $rs->getNumRows();
        $cashin=0;
        $cashout=0;
        $cashinOv=0;
        $cashoutOv=0;

        $totemCashin=0;
        $totemCashout=0;
        $totemCashinOv=0;
        $totemCashoutOv=0;

        while($rs->hasNext()){
            $row = $rs->next();
            if($row['type']=='CASH-IN'){
                if($row['ttm_code']!='N'){
                    $totemCashin     +=abs($row['att_amount']);
                    $totemCashinOv   +=abs($row['att_overdraft']);
                }
                elsE{
                    $cashin     +=abs($row['att_amount']);
                    $cashinOv   +=abs($row['att_overdraft']);
                }
            }
            else{
                if($row['ttm_code']!='N'){
                    $totemCashout    +=abs($row['att_amount']);
                    $totemCashoutOv  +=abs($row['att_overdraft']);
                }
                else{
                    $cashout    +=abs($row['att_amount']);
                    $cashoutOv  +=abs($row['att_overdraft']);
                }
            }
        }
        $endtime  = microtime(true);
        $duration = $endtime - $starttime;
        $duration = number_format($duration, 4, ',', '')."s";
        ?>
        <br /><div class="tip"><?=$lang->getLang("Time taken to execute your request")?>:<?=$duration?></div>

        <? if($numrow!=0){
            ?>
            <br/>
            <table style="width: 100%"><tr><td style="width: 30%">
            <table class="fleft table table-bordered">
                <tr><td class="label" style="font-size: 14px;padding: 5px;border-radius:3px">Cash-In</td><td class="label" style="padding: 5px; font-size: 14px;border-radius:3px">Cash-Out</td><td class="label centered" style="padding: 5px;border-radius:3px; font-size: 14px">Diff</td></tr>
                <tr><td style="font-size: 14px"><?=getInDollars($cashin-($type==0? $totemCashin:-$totemCashin))?></td><td style="font-size: 14px"><?=getInDollars($cashout-($type==0?$totemCashout:-$totemCashout))?></td><td style="font-size: 14px"><?=getInDollars($cashin-$cashout-($type==0?($totemCashin-$totemCashout):-($totemCashin-$totemCashout)))?></td></tr>
            </table>
            </td><td style="width: 36%">

            <? if ($type==0 && $jur_class=='club') { ?>
            <table class="fleft table table-bordered" >
                <tr><td class="label" style="font-size: 14px;padding: 5px;border-radius:3px">Totem Cash-In</td><td class="label"  style="font-size: 14px;padding: 5px;border-radius:3px">Totem Cash-Out</td><td class="label"  style="font-size: 14px;padding: 5px;border-radius:3px">Diff</td></tr>
                <tr><td style="font-size: 14px"><?=getInDollars($totemCashin)?></td><td style="font-size: 14px"><?=getInDollars($totemCashout)?></td><td style="font-size: 14px"><?=getInDollars($totemCashin-$totemCashout)?></td></tr>
            </table>
            </td><td style="width: 33%">
            <? } ?>

            <table class="fright">
                <tr><td class="label" style="font-size: 14px;padding: 5px;border-radius:3px">Overdraft-In</td><td class="label" style="font-size: 14px;padding: 5px;border-radius:3px">Overdraft-Out</td><td class="label centered" style="font-size: 14px;padding: 5px;border-radius:3px">Diff</td></tr>
                <tr><td style="font-size: 14px"><?=getInDollars($cashinOv)?></td><td style="font-size: 14px"><?=getInDollars($cashoutOv)?></td><td style="font-size: 14px"><?=getInDollars($cashinOv-$cashoutOv)?></td></tr>
            </table>
            </td></tr></table>
            <br />
            <div class="clearfix"></div>
            <?
            $post_vars = array('jur_id'=>$jur_id, 'date_start'=>$date_start,'date_end'=>$date_end,'doQuery'=>$_POST['doQuery'],'type'=>$type,'with'=>$with,'max_rows'=>$max_rows );
            $qp = new QueryPrinter($rs);
            $qp->printTable($rs, $cols_arr, $post_vars, $val_fmt_arr, $cell_fmt_arr, 0, 0,"",$width="100%", $start_row = ((isset ( $_POST ['start_row'] )) ? $_POST ['start_row'] : 1), $_POST['max_rows'], false);
        }
        else
        {
            addError("", $lang->getLang("No result found"));
            showErrors();
        }
    }
    else{
        showErrors();
    }
}


function returnStatus($status,$amount){
    $strToRet="<span>".getInDollars($amount)."</span><br>";
    $strToRet.="<span ";
    if ($status=='A'){
        $strToRet.=' class="result">Accepted';
    }
    elseif ($status=='D'){
        $strToRet.=' class="error">Declined';
    }
    elseif ($status=='P')
    {
        $strToRet .='class ="pending">Pending';
    }
    return $strToRet;
}


function returnWith($aus='N',$username='N',$otheraff='N',$proc='N',$otherAvailableCredit,$otherAvailableOverdraft){

    $strToRet='';
    if($aus!='N'){
        $strToRet .=" Admin ".$aus."<br />
                          Available credit: ".getInDollars($otherAvailableCredit)."<br />
                          Available overdraft: ".getInDollars($otherAvailableOverdraft);
    }
    elseif($username!='N'){
        $strToRet .=" User ".$username."<br />
                          Available credit: ".getInDollars($otherAvailableCredit)."<br />";
    }
    elseif($otheraff!='N'){
        $strToRet .=" Subaffiliate ".$otheraff."<br />
                          Available credit: ".getInDollars($otherAvailableCredit)."<br />
                          Available overdraft: ".getInDollars($otherAvailableOverdraft);
    }
    else{
        $strToRet.=$proc;
    }

    return $strToRet;
}

function returnWithAdmin($firstJur='N',$username='N',$firstJurClass,$ttm_code){
    $strToRet='';
    if($firstJur!='N'){
        $strToRet .= $firstJur ."<br />
                     <span class='tip'>(".$firstJurClass.")</span>";
    }
    elseif($username!='N'){
        $strToRet .=$username."<br />
                     <span class='tip'>(User) </span>";
    }
    elseif($ttm_code!='N'){
        $strToRet .=$ttm_code."<br />
                     <span class='tip'>(Totem) </span>";
    }
    return $strToRet;
}

function searchAffiliateTransaction(){
    global $dbh,$lang,$date_start, $date_end,$aff_id,$jur_id,$type,$with;
    $starttime = microtime(true);
    $sql1="SELECT atr_time,
                         atr_amount,
                         atr_overdraft,
                         atr_available_credit,
                         atr_available_overdraft,
                         atr_other_available_credit,
                         atr_other_available_overdraft,
                         IF (atr_aff_id IS NOT NULL, (select pun_username FROM punter WHERE pun_aff_id = atr_aff_id AND pun_aus_id IS NOT NULL), 'N') atr_aff_username,
                         IF (atr_aus_id IS NOT NULL, (select aus_username FROM admin_user WHERE aus_id = atr_aus_id), 'N') atr_aus_username,
                         IF (atr_other_pun_id IS NOT NULL, (select pun_username FROM punter WHERE pun_id = atr_other_pun_id), 'N') atr_pun_username,
                         IF (atr_other_aff_id IS NOT NULL, (select pun_username FROM punter WHERE pun_aff_id = atr_other_aff_id AND pun_aus_id IS NOT NULL), 'N') atr_other_aff_username,
                         coalesce(atr_notes, 'NONE') atr_notes,
                         atr_transaction_id,
                         IF (atr_tty_id IS NOT NULL, (select tty_description from transaction_type where atr_tty_id = tty_id), 'N') tty_description,
			             IF (atr_ptr_id IS NOT NULL, (select ptr_ppc_code FROM processor_transaction WHERE atr_ptr_id = ptr_id), 'N') ptr_ppc_code,
			             'A' status,
			             IF ((atr_tty_id = 19 AND atr_other_pun_id > 0) OR (atr_tty_id = 20 AND atr_aus_id > 0) OR (atr_tty_id = 10 OR atr_tty_id = 24), 'CASH-OUT', 'CASH-IN') type
                        FROM affiliate_transaction
                        WHERE atr_time BETWEEN  '$date_start:00' AND '$date_end:59'
                         AND (atr_aff_id=".$aff_id."
               OR atr_other_aff_id=".$aff_id.")";
    if($with==0){
        $sql1.=" AND atr_other_pun_id IS NOT NULL";
    }
    elseif($with==1){
        $sql1.=" AND  atr_other_pun_id IS NULL";
    }
    $sql2="         SELECT ptr_start atr_time,
                            ptr_amount atr_amount,
                            0 atr_overdraft,
                            ptr_available_credit atr_available_credit,
                            ptr_available_overdraft atr_available_overdraft,
                            0 atr_other_available_credit,
                            0 atr_other_available_overdraft,
                            IF (ptr_pun_id IS NOT NULL, (select pun_username FROM punter WHERE pun_id = ptr_pun_id), 'N') atr_aff_username,
                            'N' atr_aus_username, 'N' atr_pun_username, 'N' atr_other_aff_username, coalesce(ptr_note, 'NONE') atr_notes, ptr_uniqueid atr_transaction_id,
                            IF (ptr_tty_id IS NOT NULL, (select tty_description from transaction_type where tty_id = ptr_tty_id), 'N') tty_description, ptr_ppc_code, ptr_status status,
                            'N' type
                            FROM processor_transaction, punter
                        WHERE ptr_start BETWEEN  '$date_start:00' AND '$date_end:59'
                        AND pun_aff_id = ".$dbh->escape($aff_id)."
                        AND pun_aus_id IS NOT NULL
                        AND ptr_pun_id = pun_id
                        AND (ptr_status != 'A')
                        order by atr_time DESC ";
    if($type==0 && $with!=1){
        $sql=$sql1." UNION ALL ".$sql2;
    }
    elseif($type==1 && $with!=0){
        $sql=$sql2;
    }
    else{
        $sql=$sql1;
    }
    if(!areErrors()){
        $rs=$dbh->doCachedQuery($sql,0);

        $cols_arr = array("Date","Operation","Amount","Overdraft","Available Credit","Available Overdraft","With","Note","Transaction Id");

        $val_fmt_arr = array (
            "Date"                          =>  'return $rec["atr_time"];',
            "Operation"                     =>  'return $rec["tty_description"];',
            "Amount"                        =>  'return returnStatus($rec["status"],$rec["atr_amount"]);',
            "Overdraft"                     =>  'return getInDollars($rec["atr_overdraft"]);',
            "Available Credit"              =>  'return getInDollars($rec["atr_available_credit"]);',
            "Available Overdraft"           =>  'return getInDollars($rec["atr_available_overdraft"]);',
            "With"                          =>  'return returnWith($rec["atr_aus_username"],$rec["atr_pun_username"],$rec["atr_other_aff_username"],$rec["ptr_ppc_code"],$rec["atr_other_available_credit"],$rec["atr_other_available_overdraft"]);',
            "Note"                          =>  'return ($rec["atr_notes"]=="" ? "--" : $rec["atr_notes"]);',
            "Transaction Id"                =>  'return $rec["atr_transaction_id"];'
        );
        $cashin=0;
        $cashout=0;
        while($rs->hasNext()){
            $row = $rs->next();
            if($row['type']=='CASH-IN'){
                $cashin +=abs($row['atr_amount']);
            }
            else{
                $cashout +=abs($row['atr_amount']);
            }
        }
        $cell_fmt_arr = array ();
        $numrow = $rs->getNumRows();
        $endtime = microtime(true);
        $duration = $endtime - $starttime;
        $duration=number_format($duration, 4, ',', '')."s";
        ?>
        <br /><div class="tip"><?=$lang->getLang("Time taken to execute your request")?>:<?=$duration?></div>

        <? if($numrow!=0){
            ?>
            <br/>
            <table>
                <tr><td class="label" style="font-size: 14px">Cash-In</td><td class="label" style="font-size: 14px">Cash-Out</td><td class="label" style="font-size: 14px">Diff</td></tr>
                <tr><td style="font-size: 14px"><?=getInDollars($cashin)?></td><td style="font-size: 14px"><?=getInDollars($cashout)?></td><td style="font-size: 14px"><?=getInDollars($cashin-$cashout)?></td></tr>
            </table>
            <?
            $post_vars = array('jur_id'=>$jur_id, 'date_start'=>$date_start,'date_end'=>$date_end,'doQuery'=>$_POST['doQuery'] );
            $qp = new QueryPrinter($rs);
            $qp->printTable($rs, $cols_arr, $post_vars, $val_fmt_arr, $cell_fmt_arr, 0, 0,"",$width="100%", $start_row = ((isset ( $_POST ['start_row'] )) ? $_POST ['start_row'] : 1), $_POST['max_rows'], false);
        }else{
            addError("", $lang->getLang("No result found"));
            showErrors();
        }

    }
    else{
        showErrors();
    }
}