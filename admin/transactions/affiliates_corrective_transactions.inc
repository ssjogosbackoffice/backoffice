<?php
check_access("transactions_affiliates_corrective", true);

$_POST['totOvd']=0;
?>
<style>
    .refresh{
        vertical-align: middle;
        margin-left: 5px;
        cursor: pointer;
    }
</style>
<?require_once "account_functions.php";?>
    <div class="bodyHD"><?=$lang->getLang("Affiliate credit modify")?></div><br />
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

            $('#jurisdiction').change(function() {
                afid= $(this).val();
                $.post("/services/general_services.inc",{action:"11",afid:afid}, function(data){
                    var aff=data.split('~');
                    $('#club2').empty().append(aff[0]);

                    $('#amount').val(aff[1]);
                    $('#amount2').empty().append(aff[1]);

                    $('#overdraft').val(aff[2]);
                    $('#overdraft2').empty().append(aff[2]);

                    $('#totOverdraft').val(aff[3]);
                    $('#totOverdraft2').empty().append(aff[3]);

                    $('#from').val(aff[4]);
                    $('#from2').empty().append(aff[4]);
                });
            }).trigger('change');


            $('#amount').keypress(function(event){
                if(event.which != 8 && event.which != 46 && event.which != 45  && isNaN(String.fromCharCode(event.which))){
                    event.preventDefault();
                }});

            $('.refresh').on('click',function() {
                var previous=$(this).prev();
                prev_id=previous.attr("id")

                previous.val(parseFloat($("#"+prev_id+"2").find(".cAmount").text()));
            });
            $('.clear').on('click',function() {
                var previous=$(this).parent().find("input");
                previous.val('');
                //prev_id=previous.attr("id");
                //previous.val($("#"+prev_id+"2").text());
            });
        });
    </script>
<?php
if(isset($_POST['jurisdiction'])){
    $starttime = microtime(true);
        $endtime = microtime(true);
    $result=doAffAdjustment($_POST['jurisdiction'],$_POST['amount'],$_POST['overdraft'],$_POST['totOverdraft'],$_POST["from"]);
   if($result==1){
       addResult('',$lang->getLang('Affiliate updated successfully!'));
       showResult();
   }
    $duration = $endtime - $starttime;
    $duration=number_format($duration, 4, ',', '')."s";
}
?>

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
                <table  style="margin-left: 200px;">
                     <tr><td class="label"><?=$lang->getLang("Club name")?></td><td id="club2"></td></tr>
                     <tr><td class="label"><?=$lang->getLang("Credit")?> </td><td id="amount2"></td></tr>
                     <tr><td class="label"><?=$lang->getLang("Overdraft")?></td><td id="overdraft2"></td></tr>
                     <tr><td class="label"><?=$lang->getLang("Total Overdraft Received")?></td><td id="totOverdraft2"></td></tr>
                     <tr><td class="label"><?=$lang->getLang("Overdraft start time")?></td><td id="from2"></td></tr>
                </table>
            </td>
        </tr>
        <tr id='amountTr'>
            <td class="label"><?=$lang->getLang("Amount")?></td>
            <td>
                <input type="number" name='amount' id='amount'  step="0.01" value="<?=$_POST['amount']?>" autocomplete="off" /><img src="/media/images/refresh.png" width="20px" height="20px" class="refresh" ><button type="button" class="clear entitybtn"><?=$lang->getLang("Clear")?></button>
                <!--  <span id='totOvd'><input type="checkbox" name='totOvd' value='1' > <?=$lang->getLang("Do not modify total overdraft")?></span> -->
            </td>
        </tr>
        <tr>
            <td class="label"><?=$lang->getLang("Overdraft")?></td>
            <td>
                <input type="number" name='overdraft' id='overdraft' min="0" step="0.01" value="<?=$_POST['amount']?>" autocomplete="off" /><img src="/media/images/refresh.png" width="20px" height="20px" class="refresh" ><button type="button" class="clear entitybtn"><?=$lang->getLang("Clear")?></button>
            </td>
        </tr>
        <tr>
            <td class="label"><?=$lang->getLang("Total Overdraft")?> </td>
            <td>
                <input type="number" name='totOverdraft' id='totOverdraft' min="0" step="0.01" value="<?=$_POST['amount']?>" autocomplete="off" /><img src="/media/images/refresh.png" width="20px" height="20px" class="refresh" ><button type="button" class="clear entitybtn"><?=$lang->getLang("Clear")?></button>
            </td>
        </tr>
        <tr id='fromTr'>
            <td class="label"><?=$lang->getLang("Overdraft start time")?></td>
            <td>
                <input type="text" name='from' id='from' autocomplete="off" /><img src="/media/images/refresh.png" width="20px" height="20px" class="refresh" ><button type="button" class="clear entitybtn"><?=$lang->getLang("Clear")?></button>
            </td>
        </tr>

        <!--    <tr id='ovResetTr'>
        <td class="label"><?/*=$lang->getLang("Remove overdraft time")*/?> </td>
        <td>
            <input type="checkbox" name='ovReset' id='ovReset' />
        </td>
    </tr>-->
   <!--     <tr>
            <td class=label><?/*=$lang->getLang("Note")*/?></td>
            <td class="content"><textarea  name="note"></textarea></td>
        </tr>-->

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


<?

function getJurisdictionsAffiliates($jur_id) {
    global $dbh;
    $jur_class=$_SESSION['jurisdiction_class'];
    $sql = "select c.jur_name club_name,c.jur_id club_id,d.jur_name district_name,aus_username,aff_id
            FROM jurisdiction c
            JOIN jurisdiction d on d.jur_id = c.jur_parent_id
            JOIN admin_user on c.jur_id=aus_jurisdiction_id
            JOIN affiliate on aff_aus_id=aus_id
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
            AND aff_is_club=1 ";
    $result = $dbh->exec( $sql, false,true );
    return $result;

}

function doAffAdjustment($aff,$amount,$overdraft,$totOverdraft,$ovStart){
    global $dbh;
    $sql="select act_credits,act_overdraft,act_tot_overdraft_received,act_overdraft_start_time from affiliate_credit where act_aff_id=".$dbh->escape($aff);
    $affDetails=$dbh->queryRow($sql);
    $sql="Update affiliate_credit set act_credits=".$dbh->prepareString($amount).",
                                      act_overdraft=".$dbh->prepareString($overdraft).",
                                      act_tot_overdraft_received=".$dbh->prepareString($totOverdraft).",
                                      act_overdraft_start_time=".($ovStart!=''? $dbh->prepareString($ovStart) : 'NULL')."
           WHERE act_aff_id=".$dbh->escape($aff);
    $log_data='';
    if($affDetails['act_credits']!=$amount){
        if($amount==''){
            $amount=0;
        }
        $log_data = " Credit from ".$affDetails['act_credits']." to ".$dbh->escape($amount);
    }
    if($affDetails['act_overdraft']!=$overdraft){
        if($log_data!=''){
            $log_data .=" ,";
        }
        if($overdraft==''){
            $overdraft=0;
        }
        $log_data.= "Overdraft from ".$affDetails['act_overdraft']." to ".$dbh->escape($overdraft);
    }
    if($affDetails['act_tot_overdraft_received']!=$totOverdraft){
        if($log_data!=''){
            $log_data .=" ,";
        }
        if($totOverdraft==''){
            $totOverdraft=0;
        }
        $log_data.= "Tot Overdraft from ".$affDetails['act_tot_overdraft_received']." to ".$dbh->escape($totOverdraft);
    }
    if($affDetails['act_overdraft_start_time']!=$ovStart){
        if($log_data!=''){
            $log_data .=" ,";
        }
        if($ovStart==''){
            $ovStart='NULL';
        }
        $log_data.= "Ov start time from ".$affDetails['act_overdraft_start_time']." to ".($ovStart!=''? $dbh->escape($ovStart) : 'NULL');
    }
    $pun="COALESCE((SELECT pun_id from punter where pun_aff_id=".$dbh->escape($aff)." AND pun_aus_id IS NOT NULL and pun_aff_id >0), 0)";
    doAdminUserLog($_SESSION['admin_id'],'update affiliate',"$log_data",$pun);
    $result=$dbh->exec($sql);
    return $result;

}


?>