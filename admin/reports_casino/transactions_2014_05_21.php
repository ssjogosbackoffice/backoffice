<?include('header.inc'); ?>
<script type="text/javascript">
    $(function() {
        $("#date_start").datepicker({
            numberOfMonths: 1,
            dateFormat: 'yy-mm-dd',
            onSelect: function (selectedDate) {
                $("#date_end").datepicker("option", "minDate", selectedDate);
                if($("#date_end").val()!==''){
                    diff = 0;

                    diff = Math.floor(($("#date_end").datepicker('getDate').getTime() - $("#date_start").datepicker('getDate').getTime()) / 86400000); // ms per day

                  if(diff>7){
                      jAlert('Please choose an interval of maximum 7 days ');
                  }
                }
            }
        });
        $("#date_end").datepicker({
            numberOfMonths: 1,
            dateFormat: 'yy-mm-dd',
            onSelect: function (selectedDate) {
                if($("#date_start").val()!==''){
                    diff = 0;

                    diff = Math.floor(($("#date_end").datepicker('getDate').getTime() - $("#date_start").datepicker('getDate').getTime()) / 86400000); // ms per day

                    if(diff>7){
                        jAlert('Please choose an interval of maximum 7 days ');
                    }
                }
            }
        });

    });
    function duration(duration,divId)
    {
        $('#'+divId).empty().append('<?=$lang->getLang("Time took to execute your request")?>: '+duration);
    }
</script>
<?php

function searchAdminTransaction(){
    global $dbh,$lang,$date_start, $date_end,$jur_id;
    $starttime = microtime(true);

    $sql="Select att_id,
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
                att_amount,
                att_overdraft,
                att_jur_avail_credit,
                att_jur_final_avail_credit
                FROM admin_user_transaction
                WHERE  1=1 ";
    $sql.=" AND (att_jur_id=".$jur_id."
                OR att_final_jur_id=".$jur_id.")";
    $sql .= " AND att_time BETWEEN '$date_start 00:00:00' AND '$date_end 23:59:59' ";


    $sql.=" UNION ALL
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
                    ptr_amount att_amount,
                    0 att_overdraft,
                    ptr_available_credit att_jur_avail_credit,
                    0 att_jur_final_avail_credit
                            FROM processor_transaction, admin_user  WHERE  1=1
                            AND (ptr_status !='A')
                            AND ptr_admin_user_id = aus_id ";
                            $sql .= " AND ptr_admin_user_id  IN (SELECT aus_id FROM admin_user where aus_jurisdiction_id=".$jur_id.")";
                             $sql .= " AND ptr_start >= '$date_start 00:00:00' AND ptr_end <= '$date_end 23:59:59'";
    $sql .= " ORDER BY att_id, att_time";
    if(!areErrors()){
        $rs=$dbh->doCachedQuery($sql,0);
        $cols_arr = array("Time","Transaction Type","Admin","Transaction Id","Amount","Overdraft","From","Available Credit","To","Available Credit ","Note");
        $val_fmt_arr = array (
            "Time"                          =>  'return $rec["att_time"];',
            "Note"                          =>  'return $rec["att_note"];',
            "Transaction Type"              =>  'return $rec["tty_description"];',
            "Admin"                         =>  'return getClassLink("admin_users/admin_user_edit.php?id={$rec["att_aus_id"]}", $rec["aus_username"]);',
            "Transaction Id"                =>  'return $rec["att_transaction_id"];',
            "From"                          =>  'return ($rec["first_jurisdiction"]);',
            "To"                            =>  'return $rec["second_jurisdiction"];',
            "Amount"                        =>  'return getInDollars($rec["att_amount"]);',
            "Overdraft"                     =>  'return getInDollars($rec["att_overdraft"]);',
            "Available Credit"              =>  'return "Before:".(($rec["first_jurisdiction"]=="casino" ) ? " Unlimited" : getInDollars($rec["att_jur_avail_credit"]))."<br / >After:&nbsp;&nbsp;&nbsp;".(($rec["first_jurisdiction"]=="casino" ) ? " Unlimited" :getInDollars($rec["att_jur_avail_credit"] - $rec["att_amount"] )) ;',
            "Available Credit "             =>  'return "Before:".(($rec["second_jurisdiction"]=="casino" ) ? " Unlimited" : getInDollars($rec["att_jur_final_avail_credit"]))."<br / >After:&nbsp;&nbsp;&nbsp;".(($rec["second_jurisdiction"]=="casino" ) ? " Unlimited" : getInDollars($rec["att_jur_final_avail_credit"] + $rec["att_amount"] )) ;'
        );

        $cell_fmt_arr = array ();
        $numrow = $rs->getNumRows();
        $endtime = microtime(true);
        $duration = $endtime - $starttime;
        $duration=number_format($duration, 4, ',', '')."s";
        ?>
        <br /><div class="tip"><?=$lang->getLang("Time took to execute your request")?>:<?=$duration?></div>
        <? if($numrow!=0){
            $post_vars = array('jur_id'=>$jur_id, 'date_start'=>$date_start,'date_end'=>$date_end,'doQuery'=>$_POST['doQuery'] );
            $qp = new QueryPrinter($rs);
            $qp->printTable($rs, $cols_arr, $post_vars, $val_fmt_arr, $cell_fmt_arr, 0, 0,"",$width="100%", $start_row = ((isset ( $_POST ['start_row'] )) ? $_POST ['start_row'] : 1), $_POST['max_rows'], false);
        }else
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


function searchAffiliateTransaction(){
    global $dbh,$lang,$date_start, $date_end,$aff_id,$jur_id;
    $starttime = microtime(true);
    $sql="SELECT atr_time,
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
			             'A' status
                        FROM affiliate_transaction
                        WHERE atr_time BETWEEN  '$date_start 00:00:00' AND '$date_end 23:59:59'
                        AND (atr_aff_id = ".$dbh->escape($aff_id)." OR atr_other_aff_id = ".$dbh->escape($aff_id).")";
    $sql.=" UNION ALL
                    SELECT ptr_start atr_time,
                            ptr_amount atr_amount,
                            0 atr_overdraft,
                            ptr_available_credit atr_available_credit,
                            ptr_available_overdraft atr_available_overdraft,
                            0 atr_other_available_credit,
                            0 atr_other_available_overdraft,
                            IF (ptr_pun_id IS NOT NULL, (select pun_username FROM punter WHERE pun_id = ptr_pun_id), 'N') atr_aff_username,
                            'N' atr_aus_username, 'N' atr_pun_username, 'N' atr_other_aff_username, coalesce(ptr_note, 'NONE') atr_notes, ptr_uniqueid atr_transaction_id,
                            IF (ptr_tty_id IS NOT NULL, (select tty_description from transaction_type where tty_id = ptr_tty_id), 'N') tty_description, ptr_ppc_code, ptr_status status
                            FROM processor_transaction, punter
                        WHERE ptr_start BETWEEN  '$date_start 00:00:00' AND '$date_end 23:59:59'
                        AND pun_aff_id = ".$dbh->escape($aff_id)."
                        AND pun_aus_id IS NOT NULL
                        AND ptr_pun_id = pun_id
                        AND (ptr_status != 'A')
                        order by atr_time DESC ";

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

        $cell_fmt_arr = array ();
        $numrow = $rs->getNumRows();
        $endtime = microtime(true);
        $duration = $endtime - $starttime;
        $duration=number_format($duration, 4, ',', '')."s";
        ?>
        <br /><div class="tip"><?=$lang->getLang("Time took to execute your request")?>:<?=$duration?></div>
        <? if($numrow!=0){
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


$jur_id = getPostGetVar('jur_id');
$jur_id = (int)$jur_id;
$date_start = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d", time() - (60 * 60 * 24));
$date_end = (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d", time());
$sess_jur_id = $_SESSION['jurisdiction_id'];

$datetime1 = new DateTime($date_start);
$datetime2 = new DateTime($date_end);
$interval = $datetime1->diff($datetime2);


$sql =  "SELECT jur_code, jur_name, jur_available_credit,jur_class,jur_overdraft,jur_tot_overdraft_received,jur_overdraft_start_time,
 @affiliateId := IF(jur_class = 'club', (SELECT aff_id FROM admin_user, affiliate WHERE aus_jurisdiction_id = jur_id AND aus_id = aff_aus_id AND aff_is_club = 1), 0) as affiliateId " .
    "FROM jurisdiction " .
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
        <br /><?=$lang->getLang("Overdraft start time")?>:<?=(((!is_null($aff_id)&& $aff_id>0) ? $actOverdraftTime : $jur_overdraft_time))?>
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
            ?></td>
    </tr>
    <tr valign=top>
        <td class=label>
            <?=$lang->getLang("From")?>
        </td>
        <td <? form_td_style('start_date');?>>
            <input type='text' name='date_start' id='date_start'  value="<?=$date_start?>" >
            <?=err_field('start_date');?>
        </td>
    </tr>
    <tr valign=top>
        <td class=label>
            <?=$lang->getLang("Until")?>
        </td>
        <td <? form_td_style('end_date');?>>
            <input type='text' name='date_end' id='date_end' value="<?=$date_end?>">
            <?=err_field('end_date');?>
        </td>
    </tr>
    <tr valign=top>
        <td class=label>
            <?=$lang->getlang("Max records per page")?>
        </td>
        <td class=content>
            <select name="max_rows">
                <option <?php if($max_rows == 100){?> selected <?php }?>
                    value="100">100</option>
                <option <?php if($max_rows == 200){?> selected <?php }?>
                    value="200">200</option>
                <option <?php if($max_rows== 500){?> selected <?php }?>
                    value="500">500</option>
                <option <?php if($max_rows == 1000){?> selected <?php }?>
                    value="1000">1000</option>
            </SELECT>
        </td>
    </tr>
    <tr>
        <td>&nbsp;</td>
        <td><input type =image src="<?=image_dir?>/button_submit.gif"></td>
    </tr>
</table>
</form>
<a href="<?=refPage('reports_casino/entity_report')?>"> <?=$lang->getlang("Back to Entities Credit Report")?></a>
<?
if ( isset($_POST['doQuery']) )   // do the search
{
if($interval->days > 7){
addError('',$lang->getLang('Please choose an interval of maximum 7 days'));
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
    /*    $max_rows    = isset($_POST['max_rows'])?$_POST['max_rows']:300;

        if ( $max_rows < 100 )
        {
            $max_rows = 100;
        }

        if(isset($_POST['startrow']))
        {
            $startrow=$_POST['startrow'];
        }
        else
        {
            $startrow='0';
        }
        $d1 = strtotime("$start_date +2 months");
        $d2 = strtotime("$end_date");

        if($d2-$d1>0){
            addError("",$lang->getLang("Please choose an interval less than 2 months"));
            showErrors();
        }
        if ( ! areErrors() )
        {
            $starttime = microtime(true);
            $sql = "SELECT *, (select jur_name FROM jurisdiction WHERE jur_id = cre_entity_id) jur_entity_name,
                                      (select jur_name FROM jurisdiction WHERE jur_id = cre_other_entity_id) jur_other_entity_name
                     FROM credit_transfer
                     WHERE DATE(cre_time)
                      BETWEEN '$start_date 00:00:00' AND '$end_date 23:59:59'
                      AND cre_entity_id = '$jur_id'
                      order by cre_id desc  LIMIT $startrow, $max_rows";


            $sql2 = "SELECT count(1)
                     FROM credit_transfer
                      WHERE cre_entity_id = '$jur_id'
                      AND  DATE(cre_time) BETWEEN '$start_date 00:00:00' AND '$end_date 23:59:59'
                      order by cre_id desc
                      ";
            $rs2=$dbh->queryOne($sql2);
            $rs = $dbh->exec($sql);
            $num_rows = $rs2;
            $totals=$num_rows;
            $endtime = microtime(true);
            $duration = $endtime - $starttime;
            $duration=number_format($duration, 4, ',', '')."s";
            ?>
            <div class="tip" id='duration'><?=$lang->getLang("Time took to execute your request")?>: <?=$duration?></div>
            <?
            echo '<br />';
            printInfoMessage("$num_rows rows returned");
            echo '<br />';

            if ( $num_rows > 0 )
            {
                ?>
                <div style="margin-left:20px" id="paginator">
                    <table cellpadding="5" cellspacing="1" border="0" id="paginator" style='min-width: 700px;'>
                        <tr>
                            <th class="label" style='padding: 5px'>ID</th>
                            <th class="label" style='padding: 5px'><?=$lang->getLang("Time")?></th>
                            <th class="label" style='padding: 5px'><?=$lang->getLang("Entity")?></th>
                            <th class="label" style='padding: 5px'><?=$lang->getLang("Other Entity")?></th>
                            <th class="label" style='padding: 5px'><?=$lang->getLang("Credit Received")?></th>
                            <th class="label" style='padding: 5px'><?=$lang->getLang("Credit Transferred")?></th>
                        </tr>
                        <?
                        while ($rs->hasNext()) {
                            $row               = $rs->next();
                            $id                = $row['cre_id'];
                            $time              = $row['cre_time'];
                            $amount            = $row['cre_amount'];
                            $entity = $row['jur_entity_name'];
                            $other_entity_name = $row['jur_other_entity_name'];
                            $direction         = $row['cre_direction'];
                            $bg = ('IN' == $direction) ? 'lightgreen' : '#F3C3C7';
                            $lastid=$row['cre_id'];
                            ?>
                            <tr>
                                <td class="content" style="background-color:<?=$bg?>"><?=$id?></td>
                                <td class="content" style="background-color:<?=$bg?>"><?=$time?></td>
                                <td class="content" style="background-color:<?=$bg?>"><?=$entity?></td>
                                <td class="content" style="background-color:<?=$bg?>"><?=$other_entity_name?></td>
                                <td class="content" style="background-color:<?=$bg?>" align="right">
                                    <?php
                                    if ( 'IN' == $direction)
                                        echo getInDollars($amount);
                                    else
                                        echo '&nbsp;';
                                    ?>
                                </td>
                                <td class="content" style="background-color:<?=$bg?>" align="right">
                                    <?php
                                    if ( 'OUT' == $direction)
                                        echo getInDollars($amount);
                                    else
                                        echo '&nbsp;';
                                    ?>
                                </td>
                            </tr>
                            <?         // $row = $rs->next();
                        }
                        ?>
                    </table>
                </div>

                <?php
                if ($totals> $max_rows){?>
                    <div id="paginator_control" style="margin-left:20px;min-width: 700px;">
                        <link href="../media/smartpaginator/smartpaginator.css" rel="stylesheet" type="text/css" />
                        <script src="../media/smartpaginator/smartpaginator.js" type="text/javascript"></script>
                        <script>

                            $('#paginator_control').smartpaginator({ totalrecords: <?=$totals?>,
                                recordsperpage: <?=$max_rows?>,
                                theme: 'black',
                                controlsalways:true,
                                onchange: function(recordStartIndex) {
                                    $.post("reports_casino/entity_report_functions.inc",{action:"4",jur_id:<?=$jur_id?>,results:"<?=$totals?>",startrow:recordStartIndex,start_date:$('input:text[name=date_start]').val(),end_date:$('input:text[name=date_end]').val(),recperpage:<?=$max_rows?>}, function(data){
                                        $('#paginator').empty().append( data);
                                    });
                                }
                            });
                        </script>
                    </div>
                <?
                }
                ?>
                <br /><br /><br />
            <?

            }
        } */
}
}
?>

<?include('footer.inc'); ?>
