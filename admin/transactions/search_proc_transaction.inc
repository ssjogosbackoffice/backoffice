<?php
/**
 * Created by CSLIVEGAMES.
 * User: Manuel
 * Date: 03/06/13
 * Time: 18.09
 */
check_access('transactions_processor_search', true);

define('PRC_WITHDRAWAL', 10);
define('PRC_DEPOSITS', 3);
define('PRC_REFUND', 16);
define('PRC_DECLINED', 15);

$selectProcessor = "Select prc_id, prc_name from processor where prc_active = 1";
$result = $dbh->doCachedQuery($selectProcessor, 3600);
$start_date = (isset($_POST['date_start'])) ? $_POST['date_start'] : date('Y-m-d H:i', mktime(0,0,0,date('m',time()),date('d',time()),date('Y',time())));
$end_date = (isset($_POST['date_end'])) ? $_POST['date_end'] : date('Y-m-d H:i', time());
$trans_code = (isset($_POST['trans_code'])) ? $_POST['trans_code'] : "";
$procID = (isset($_POST['proclist'])) ? $_POST['proclist'] : '*';
$row_to_page = (isset($_POST['rowforpages'])) ? $_POST['rowforpages'] : 20;
$status = (isset($_POST['status'])) ? $_POST['status'] : array();
$username = (isset($_POST['username'])) ? $_POST['username'] : '';
$result = $dbh->doCachedQuery($selectProcessor, 0);
?>
<script src="<?= secure_host ?>/media/ui/js/jquery.timepicker.js" type="text/javascript"></script>
<script type="text/javascript">
    $(document).ready(function(){
        $( "#date_start" ).datetimepicker({
            showAnim:"slide",
            buttonImage: "/media/images/cal.gif",
            buttonImageOnly: true,
            showOn:"both",
            showButtonPanel: true,
            changeYear:true,
            changeMonth: true,
            numberOfMonths:1,
            onChangeMonthYear:function(year, month, i){
                var day = i.selectedDay;
                $(this).datepicker('setDate', new Date(year, month-1, day));
            }
        });
        $( "#date_end").datetimepicker({
            buttonImage: "/media/images/cal.gif",
            buttonImageOnly: true,
            showOn:"both",
            showButtonPanel: true,
            showAnim:"slide",
            changeYear:true,
            changeMonth: true,
            numberOfMonths:1,
            onSelect: function( selectedDate ) {
                $( "#date_start" ).datetimepicker( "option", "minDate", -50);
                $( "#date_start" ).datetimepicker( "option", "maxDate", selectedDate );
            },
            onChangeMonthYear:function(year, month, i){
                var day = i.selectedDay;
                $(this).datepicker('setDate', new Date(year, month-1, day));
            }
        });
    });
</script>

<div class="bodyHD centered" ><?=$lang->getLang("Processor Transaction")?></div>

</br>

<div style="margin:auto;width:280px;">
    <table>
        <?php form_head()?>
        <?php printHiddenInput('doQuery', 'yes');?>
        <?php printHiddenInput('search_type', 'date_range');?>
        <tr><td class="label"><?=$lang->getLang("Username")?>:</td>
            <td class="content"><input id="username" name="username" type="input" value="<?=$username?>" /></td></tr>
        <tr>
        <tr><td class="label"><?=$lang->getLang("Transaction Code")?>:</td>
            <td class="content"><input id="trans_code" name="trans_code" type="input" value="<?=$trans_code?>" /></td></tr>
        <tr>
        <tr><td class="label"><?=$lang->getLang("Select Processor")?>:</td>
            <td class="content">
                <select name="proclist">
                    <option value="*" <?php if($row['prc_id'] == $procID){?> selected <?php }?>><?=$lang->getLang("All")?></option>
                    <?php
                    while($result->hasNext()){
                        $row = $result->next();
                        ?>
                        <option value="<?=$row['prc_id']?>" <?php if($row['prc_id'] == $procID){?> selected <?php }?>><?=$row['prc_name']?></option>
                    <?php
                    }
                    ?>
                </select>
            </td></tr>
        <tr><td class="label"><?=$lang->getLang("From")?></td>
            <td class="content"><input id="date_start" name="date_start" type="input" value="<?=$start_date?>" /></td></tr>
        <tr><td class="label"><?=$lang->getLang("Until")?></td>
            <td class="content"><input id="date_end" name="date_end" type="input" value="<?=$end_date?>" /></td></tr>

        <tr><td class="label"><?=$lang->getLang("Select Status")?>:</td>
            <td class="content">
                <select multiple name="status[]" style="width:80px;height:95px;">
                    <option value="*" <?php if(checkStatusIsAll()){ ?> selected <?php } ?>><?=$lang->getLang("All")?></option>
                    <option value="A" <?php if(checkStatusSelected('A')){ ?> selected <?php } ?>><?=$lang->getLang("Accepted")?></option>
                    <option value="P" <?php if(checkStatusSelected('P')){ ?> selected <?php } ?>><?=$lang->getLang("Pending")?></option>
                    <option value="D" <?php if(checkStatusSelected('D')){ ?> selected <?php } ?>><?=$lang->getLang("Declined")?></option>
                    <option value="I" <?php if(checkStatusSelected('I')){ ?> selected <?php } ?>><?=$lang->getLang("Initialized")?></option>
                    <option value="T" <?php if(checkStatusSelected('T')){ ?> selected <?php } ?>><?=$lang->getLang("Timeout")?></option>
                    <option value="GP" <?php if(checkStatusSelected('GP')){ ?> selected <?php } ?>><?=$lang->getLang("Gateway pending")?></option>
                </select>
            </td></tr>
        <tr><td class="label"><?=$lang->getLang("Records per page")?>:</td>
            <td class="content">
                <select name="rowforpages">
                    <option value="20" <?php if($row_to_page == 20){ ?> selected <?php } ?> >20</option>
                    <option value="50" <?php if($row_to_page == 50){ ?> selected <?php } ?>>50</option>
                    <option value="100" <?php if($row_to_page == 100){ ?> selected <?php } ?>>100</option>
                </select>
            </td></tr>
        <td>&nbsp;</td>
        <td><input type =image src="<?=image_dir?>/button_submit.gif" name="submit"></td>
        </tr>
    </table>
</div>


<?php
if(isset($_POST['doQuery'])){
    $jur_id = $_SESSION['jurisdiction_id'];
    $jur_class = $_SESSION['jurisdiction_class'];

    $class = '';
    switch ($jur_class) {
        case 'nation':
            $class = 'AND n.jur_id = '.$jur_id;
            break;
        case 'region':
            $class = 'AND r.jur_id = '.$jur_id;
            break;
        case 'district':
            $class = 'AND d.jur_id = '.$jur_id;
            break;
        case 'club':
            $class = 'AND c.jur_id = '.$jur_id;
            break;
    }

    $sql = "SELECT prc_name, ptr_merchant_account, ptr_procref, pun_username, ptr_amount, ptr_note, ptr_processor_note,
                   ptr_status, ptr_uniqueid, ptr_punip, ptr_start, ptr_tty_id,
                   IF (ptr_admin_user_id IS NOT NULL, aus_username, pun_username) user,ptr_admin_user_id,
                   pun_id,ccs_conversion_rate,ptr_amount_converted,
                   curr1.cur_code_for_web cur_code_for_web,
                   curr2.cur_code_for_web converted_currency_sym

            FROM jurisdiction n
            JOIN jurisdiction r ON r.jur_parent_id = n.jur_id AND n.jur_parent_id = 1
            JOIN jurisdiction d ON d.jur_parent_id = r.jur_id
            JOIN jurisdiction c ON c.jur_parent_id = d.jur_id
            JOIN punter ON pun_betting_club = c.jur_id
            JOIN processor_transaction ON pun_id = ptr_pun_id
            LEFT JOIN admin_user on aus_id=ptr_admin_user_id
            LEFT JOIN processor ON prc_id = ptr_prc_id
            LEFT JOIN currencies_conversion_session on ptr_ccs_id=ccs_id
            LEFT join currency curr1 on  curr1.cur_id=ccs_cur_id_from
            LEFT join currency curr2 on  ccs_cur_id_to=ccs_cur_id_to ";
//    if($_SESSION["jurisdiction_class"] != "casino"){
//        $sql .= "
//  		     JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
//             JOIN jurisdiction regional ON regional.jur_id = district.jur_parent_id
//             JOIN jurisdiction nation   ON nation.jur_id   = regional.jur_parent_id";
//    }

    $sql .=  " WHERE ptr_start between '$start_date' and '$end_date' $class ";
    if(isset($_POST['proclist']) && $_POST['proclist'] != "*"){
        $sql .= " AND ptr_prc_id = $procID";
    }
    if(isset($_POST['trans_code']) && $_POST['trans_code'] != ""){
        $sql .= " AND ptr_uniqueid = '$trans_code'";
    }
    if(isset($_POST['status']) && $_POST['status'] != array() && !checkStatusIsAll()){
        $sql .= " AND ptr_status in ('" . implode($status,"','") . "')";
    }
    if(isset($_POST['username']) && $_POST['username'] != ""){
        $sql .= " AND pun_username LIKE '%$username%' ";

//        switch($_SESSION["jurisdiction_class"]){
//            case "club":
//                $sql .= " AND club.jur_id = ". $_SESSION['jurisdiction_id'];
//                break;
//            case "district":
//                $sql .= " AND district.jur_id = " . $_SESSION['jurisdiction_id'];
//                break;
//            case "region":
//                $sql .= " AND regional.jur_id = ". $_SESSION['jurisdiction_id'];
//                break;
//            case "nation":
//                $sql .= " AND nation.jur_id = ". $_SESSION['jurisdiction_id'];
//                break;
//        }
    }
    $sql .= " GROUP BY ptr_uniqueid

     ORDER BY ptr_start DESC";

//    echo "<pre>";
//    print_r($sql);
//    exit;

    $starttime = microtime(true);
    $res = $dbh->doCachedQuery($sql,0);
    $endtime = microtime(true);
    $duration = round(($endtime - $starttime),4);
    $totalDeposits = 0;
    $totalPendingDep = 0;
    $totalWithdrawal = 0;
    $totalPendingWith = 0;
    if($res->NumRows > 0){
        ?>
        <div class="tip"><?=$lang->getLang("Time taken to execute your request")?>: <?=$duration?></div>
        </br>
        <div class="tip"><?=$lang->getLang("Number of rows")?>: <?=$res->NumRows?></div>
        <table style="width:60%;margin:auto;">
            <tr>
                <th class="label"><?=$lang->getLang("TOTAL ACCEPTED DEPOSIT")?></th>
                <th class="label"><?=$lang->getLang("TOTAL ACCEPTED WITHDRAWAL")?></th>
                <th class="label"><?=$lang->getLang("TOTAL PENDING DEPOSIT")?></th>
                <th class="label"><?=$lang->getLang("TOTAL PENDING WITHDRAWAL")?></th>
            </tr>
            <tr>
                <td style="text-align:right;"><div id="totdep" ><?=getInDollars(0)?></div></td>
                <td style="text-align:right;"><div id="totwith"><?=getInDollars(0)?></div></td>
                <td style="text-align:right;"><div id="totpedep"><?=getInDollars(0)?></div></td>
                <td style="text-align:right;"><div id="totpewith"><?=getInDollars(0)?></div></td>
            </tr>
        </table>
        <br/>
        <br/>
        <br/>
        <div id="paginatorproc">
            <table style="width:100%;border-spacing:0px;" id="paginator">
                <tr class="header">
                    <th class="label centered"><?=$lang->getLang("Processor")?></th>
                    <th class="label centered"><?=$lang->getLang("Account")?></th>
                    <th class="label centered"><?=$lang->getLang("External Id")?></th>
                    <th class="label centered"><?=$lang->getLang("Username")?></th>
                    <th class="label centered"><?=$lang->getLang("Amount")?></th>
                    <th class="label centered"><?=$lang->getLang("Amount converted")?></th>
                    <th class="label centered"><?=$lang->getLang("Conversion rate")?></th>
                    <th class="label centered"><?=$lang->getLang("Notes")?></th>
                    <th class="label centered"><?=$lang->getLang("Processor Notes")?></th>
                    <th class="label centered"><?=$lang->getLang("Processor TR Id")?></th>
                    <th class="label centered"><?=$lang->getLang("Ip")?></th>
                    <th class="label centered"><?=$lang->getLang("Trans. Type")?></th>
                    <th class="label centered"><?=$lang->getLang("Status")?></th>
                    <th class="label centered"><?=$lang->getLang("Date")?></th>
                </tr>
                <?php
                $i = 0;
                while($res->hasNext()){
                    $i++;
                    $row = $res->next();
                    switch($row['ptr_tty_id']){
                        case PRC_WITHDRAWAL:
                            if($row['ptr_status'] == 'A'){$totalWithdrawal = $totalWithdrawal+$row['ptr_amount'];}elseif($row['ptr_status'] == 'P'){$totalPendingWith = $totalPendingWith+$row['ptr_amount'];}
                            break;
                        case PRC_DEPOSITS:
                            if($row['ptr_status'] == 'A'){$totalDeposits = $totalDeposits+$row['ptr_amount'];}elseif($row['ptr_status'] == 'P'){$totalPendingDep = $totalPendingDep+$row['ptr_amount'];}
                            break;
                    }
                    ?>
                    <tr style="<?php if(($i % 2) == 0) { ?> background-color:#d0d0d0; <?php } ?>">
                        <td><?=$row["prc_name"]?></td>
                        <td class="centered" style="padding:20px;"><?=$row["ptr_merchant_account"]?></td>
                        <td class="centered" ><?=$row["ptr_procref"]?></td>
                        <td class="centered" style="width:100px;" >
                            <?=checkAdmin($row["ptr_admin_user_id"],$row["user"],$row["pun_id"]);?>
                        </td>
                        <td class="centered" style="width:100px;"><?=getInDollars($row["ptr_amount"],2,$row['cur_code_for_web'])?></td>
                        <td class="centered" style="width:100px;"><?=($row["ptr_amount_converted"] !="0"  ? getInDollars($row["ptr_amount_converted"],2,$row["converted_currency_sym"]) : "");?></td>
                        <td class="centered" style="width:100px;"><?=$row['ccs_conversion_rate']?></td>
                        <td style="text-align:left;width:200px;height:50px"><?=$row["ptr_note"]?></td>
                        <td style="text-align:left;width:200px;height:50px"><?=$row["ptr_processor_note"]?></td>
                        <?php
                        if($row['ptr_status'] == 'P'){
                            ?>
                            <td class="centered" ><?=getClassLink(refPage("transactions/deposit_pendings&header_page="),$row["ptr_uniqueid"])?></td>
                        <?php
                        }else{
                            ?>
                            <td class="centered" ><?=$row["ptr_uniqueid"]?></td>
                        <?php
                        }
                        ?>
                        <td class="centered" style="width:120px;"><?=getClassLink( refPage("customers/search&field=4&search_string={$row["ptr_punip"]}") , $row["ptr_punip"]);?></td>
                        <td class="centered" ><?=getTransactionType($row['ptr_tty_id'])?></td>
                        <td class="centered" style="width:100px;"><?=writeStatus($row["ptr_status"])?></td>
                        <td class="centered" ><?=$row["ptr_start"]?></td>
                    </tr>
                <? } ?>
                <? if ($res->NumRows>$row_to_page){?>
                    <div id="paginator_control" style="width:100%">
                        <link href="media/smartpaginator/smartpaginator.css" rel="stylesheet" type="text/css" />
                        <script src="media/smartpaginator/smartpaginator.js" type="text/javascript"></script>
                        <script>
                            $('#paginator_control').smartpaginator({ totalrecords:<?=$res->NumRows?>,
                                recordsperpage: <?=$row_to_page?>,
                                datacontainer: 'paginator',
                                dataelement: 'tr',
                                theme: 'black'
                            });
                            show('paginatorproc');
                        </script>
                    </div>

                <?
                }
                else{
                    ?>
                    <script>
                        show('paginatorproc');
                    </script>
                <?
                }
                ?>
            </table>
        </div>
        <script type="text/javascript">
            $("#totdep").html('<?=getInDollars($totalDeposits)?>');
            $("#totwith").html('<?=getInDollars($totalWithdrawal)?>');
            $("#totpedep").html('<?=getInDollars($totalPendingDep)?>');
            $("#totpewith").html('<?=getInDollars($totalPendingWith)?>');
        </script>
    <?php
    }else{
        addError("",$lang->getLang("No result was found"));
        showErrors();
    }
    ?>

    </br>
    </br>

<?php
}
?>

<?php
function writeStatus($statusID){
    global $lang;
    switch($statusID){
        case 'A':
            return '<span style="color:#2a9722">'.$lang->getLang("Accepted").'</span>';
        case 'D':
            return '<span style="color:#EE0000">'.$lang->getLang("Declined").'</span>';
        case 'I':
            return '<span style="color:#0000DD">'.$lang->getLang("Initialized").'</span>';
        case 'P':
            return '<span style="color:#A2773A">'.$lang->getLang("Pending").'</span>';
        case 'T':
            return '<span style="color:#AA0000">'.$lang->getLang("Timeout").'</span>';
        case 'GP':
            return '<span style="color:#ff8000">' .$lang->getLang("Gateway pending").'</span>';
    }
}
function getTransactionType($tty){
    global $lang;
    switch($tty){
        case PRC_DEPOSITS:
            return $lang->getLang('Deposit');
        case PRC_WITHDRAWAL:
            return $lang->getLang('Withdrawal');
        case PRC_REFUND:
            return $lang->getLang('Refund');
    }
}
function checkStatusSelected($statusID){
    global $status;
    if(checkStatusIsAll()) return false;
    foreach($status as $key => $val){
        if($val == $statusID){
            return true;
        }
    }
    return false;
}
function checkStatusIsAll(){
    global $status;
    foreach($status as $key => $val){
        if($val == '*'){
            return true;
        }
    }
    return false;
}

function checkAdmin($admin,$name,$user_id){

    if($admin!=""){
        $name=getClassLink("admin_users/admin_user_edit.php?id={$admin}",$name).'<br /><span class="tip">( Admin )</span>';
    }
    else{
        $name=getClassLink(refPage("customers/customer_view&customer_id=" . $user_id),$name);
    }
    return $name;
}


?>
