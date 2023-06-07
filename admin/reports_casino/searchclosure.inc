<?php check_access('financial_closure_search',$redirect=true);
$type=(isset($_POST["type"])) ? $_POST["type"] : '1' ;
?>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap-responsive.css" title="core_style" />
<script src="media/jquery.alerts.js" type="text/javascript"></script>
<script src="media/jquery.editable.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<script type="text/javascript">
    $(function() {
        $( "#date_start" ).datepicker({
            changeMonth: true,
            changeYear: true,
            maxDate:-1,
            numberOfMonths:1
        });
        $( "#date_end" ).datepicker({
            changeMonth: true,
            changeYear: true,
            maxDate:-1,
            numberOfMonths:1
        });
    });



    function switchToEdit(whichid,editterid,img)
    {
        var which = document.getElementById(''+whichid);
        which.style.display = 'none';
        img.style.display = 'none';
        var editter = document.getElementById(''+editterid);
        editter.firstChild.value = which.innerHTML;
        editter.style.display = 'block';
    }
    function switchBack(containerid,save)
    {
        var viewer = document.getElementById(containerid+'_text');
        var editter = document.getElementById(containerid+'_edit');

        if(save){
            viewer.innerHTML = editter.firstChild.value;
            $.post("reports_casino/partial_closure_service.inc",{action:'5',pid:containerid,pval:editter.firstChild.value}, function(data){
            });
        }
        editter.style.display = 'none';
        viewer.style.display = 'block';
        $('#'+containerid+'_img').show();
    }


    function toggleId(idtotoggle){
        if ($('#'+idtotoggle).is(":hidden")) {
            $('#'+idtotoggle).show('slow');
        } else {
            $('#'+idtotoggle).hide('slow');
        }
        return false;
    }
</script>
<?
/**
 * @author Marian
 */
//$date_start=$_POST['date_start'];
//$date_end=$_POST['date_end'];
?>

<table class="table">
    <tr><td class='navbar-inner'><h3 class="text-center"><?=$lang->getLang("Search Closure")?></h3></td></tr>
    <tr><td><div class="well">
                <?php
                /**
                 * @author Marian
                 */
                form_head()?>
                <?php printHiddenInput('doQuery', 'yes');?>
                <?php printHiddenInput('search_type', 'date_range');?>
                <div class="row">

                    <div class='span2'>
                        <div class="label label-inverse"><?=$lang->getLang("View cash-in")?></div>
                        <label class="checkbox"><i class="icon-arrow-right"></i>
                            <input type="checkbox" name='cashin' <? if($_SESSION['jurisdiction_class']=='club') {?> disabled="disabled"<? } else{ if(isset($_POST['doQuery']) && !isset($_POST['cashin'])){}else{?>checked="checked"<? } } ?> > <?=$lang->getLang("Cash-In")?>
                        </label>
                    </div>

                    <div class='span2'>
                        <div class="label label-inverse"><?=$lang->getLang("View cash-out")?></div>
                        <label class="checkbox"><i class="icon-arrow-left"></i>
                            <input type="checkbox" name='cashout'  <? if($_SESSION['jurisdiction_class']=='casino') {?> disabled="disabled"<? } else{ if(isset($_POST['doQuery']) && !isset($_POST['cashout'])){}else{?>checked="checked"<? } } ?> > <?=$lang->getLang("Cash-Out")?>
                        </label>
                    </div>

                    <div class='span3'><div class='label label-inverse'><?=$lang->getLang('Type')?></div>
                        <br><div>
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-adjust"></i></span>
                                    <select name="type">
                                        <option value="1" <?=($type==1 ?  'selected' : '')?> ><?=$lang->getLang('All')?></option>
                                        <option value="0" <?=($type==0 ?  'selected' : '')?> ><?=$lang->getLang('Casino&Poker')?></option>
                                        <option value="7" <?=($type==7 ?  'selected' : '')?> ><?=$lang->getLang('Betting')?></option>
                                    </select>

                                </div>
                            </div>
                        </div>
                    </div>

                    <? if($_SESSION['jurisdiction_class']!='club'){?>
                        <div class='span3'><div class='label label-inverse'> <?=ucfirst(getSubJurisdiction($_SESSION['jurisdiction_class']));?></div>
                            <br><div>
                                <div class="controls">
                                    <div class="input-prepend">
                                        <span class="add-on"><i class="icon-user"></i></span>
                                        <?php echo getjurisdictionChild( $_SESSION['jurisdiction_id'],$_POST['jurisdiction']);
                                        ?>
                                    </div>
                                </div>
                            </div>
                        </div>
                    <? }?>

                </div><br>
                <button class="btn btn-primary" ><?=$lang->getLang("Search")?></button>
                </form>
            </div>
        </td></tr>
    <!--  <tr><td>
               <div class='well'>
                   <div class='row'>

                       <div class="span3"><div class='label label-inverse'>From</div>
                           <br>

                           <div class="controls">
                               <div class="input-prepend">
                                   <span class="add-on"><i class="icon-calendar"></i></span>
                                   <input type='text'  name='date_start' id='date_start' value="">
                               </div>
                           </div>

                       </div>

                       <div class="span3"><div class='label label-inverse'>To</div>
                           <br>

                           <div class="controls">
                               <div class="input-prepend">
                                   <span class="add-on"><i class="icon-calendar"></i></span>
                                   <input type='text'  name='date_end' id='date_end' value="">
                               </div>
                           </div>
                       </div>
                   </div>
                   <button class="btn btn-primary" >Search</button>
                   </form><br><br>
               </div>
       </td>
       </tr> -->
    <?php
    if(isset($_POST['doQuery'])){
        if(!isset($_POST['cashin'])&&!isset($_POST['cashout'])){
            addError('',$lang->getLang('Please choose at least one option!'));
            showErrors();
        }
        if(isset($_POST['cashin'])){
            getClosureIn();
        }
        if(isset($_POST['cashout'])){
            getClosureOut();
        }
    }
    elseif(isset($_GET['startdate'])){
        if($_SESSION['jurisdiction_class']!='club'){
            getClosureIn($_GET['startdate'],$_GET['enddate']);
        }
        if($_SESSION['jurisdiction_class']!='casino'){
            getClosureOut($_GET['startdate'],$_GET['enddate']);
        }
    }
    ?>

</table>
<?php
function getClosureIn($startdate=NULL,$enddate=NULL){
    global $dbh,$lang,$type;
    $jurisdictionClass=$_SESSION['jurisdiction_class'];
    $jurisdictionId=$_SESSION['jurisdiction_id'];
    $sql="select pce_start_day,pce_end_day,pce_time,pce_gam_category,cur_code_for_web, ";

    if($jurisdictionClass=='casino'){
        $sql .="n.jur_name jurisdiction_name,n.jur_id jurisdiction_id ";
    }
    elseif($jurisdictionClass=='nation')
    {
        $sql .="r.jur_name jurisdiction_name,r.jur_id jurisdiction_id ";
    }
    elseif($jurisdictionClass=='region')
    {
        $sql .="d.jur_name jurisdiction_name,d.jur_id jurisdiction_id ";
    }
    elseif($jurisdictionClass=='district'){
        $sql .="c.jur_name jurisdiction_name,c.jur_id jurisdiction_id ";
    }
    $sql.=",coalesce(pce_bet, 0) bet,
            coalesce(pce_win, 0) win,
            coalesce(pce_rake, 0) rake,
            coalesce(pce_perc,0) perc,
            aus_username,
            pce_note,
            pce_cash_in_hand,
            pce_id,
            pce_profit,
            pce_profit_child
    FROM jurisdiction n ";

    if($jurisdictionClass=='casino')
    {
        $sql.=" LEFT JOIN jurisdiction_partial_closure ON n.jur_id = pce_jur_nation
                LEFT JOIN admin_user on aus_id=pce_aus_id
                LEFT  JOIN currency on cur_id=n.jur_currency
                WHERE n.jur_parent_id  = 1 AND pce_jur_region = 0";
        if(isset($_POST['jurisdiction'])&&($_POST['jurisdiction']!='*')){
            $sql .=" AND n.jur_id=".$_POST['jurisdiction'];
        }
        if(isset($startdate)){
            $sql.= " AND pce_start_day >='$startdate'";
        }
        if(isset($enddate)){
            $sql .=" AND pce_end_day <='$enddate'";
        }
        if($type!=1){
            $sql .=" AND pce_gam_category ='".$dbh->escape($type)."'";
        }
        $sql .=" ORDER BY n.jur_id,pce_start_day DESC";

    }
    elseif($jurisdictionClass=='nation')
    {
        $sql .="LEFT JOIN jurisdiction r ON n.jur_id = r.jur_parent_id
                LEFT  JOIN currency on cur_id=r.jur_currency
                LEFT JOIN jurisdiction_partial_closure ON r.jur_id = pce_jur_region
                LEFT JOIN admin_user on aus_id=pce_aus_id
                WHERE n.jur_id = $jurisdictionId AND pce_jur_district = 0";
        if(isset($_POST['jurisdiction'])&&($_POST['jurisdiction']!='*')){
            $sql .=" AND r.jur_id=".$_POST['jurisdiction'];
        }
        if(isset($startdate)){
            $sql.= " AND pce_start_day >='$startdate'";
        }
        if(isset($enddate)){
            $sql .=" AND pce_end_day <='$enddate'";
        }
        if($type!=1){
            $sql .=" AND pce_gam_category ='".$dbh->escape($type)."'";
        }
        $sql .=" ORDER BY r.jur_id,pce_start_day DESC";
    }
    elseif($jurisdictionClass=='region')
    {
        $sql .="LEFT JOIN jurisdiction r ON n.jur_id = r.jur_parent_id
                LEFT JOIN jurisdiction d ON r.jur_id = d.jur_parent_id
                LEFT  JOIN currency on cur_id=d.jur_currency
                LEFT JOIN jurisdiction_partial_closure ON d.jur_id = pce_jur_district
                LEFT JOIN admin_user on aus_id=pce_aus_id
                WHERE r.jur_id = $jurisdictionId AND pce_jur_club = 0";
        if(isset($_POST['jurisdiction'])&&($_POST['jurisdiction']!='*')){
            $sql .=" AND d.jur_id=".$_POST['jurisdiction'];
        }
        if(isset($startdate)){
            $sql.= " AND pce_start_day >='$startdate'";
        }
        if(isset($enddate)){
            $sql .=" AND pce_end_day <='$enddate'";
        }
        if($type!=1){
            $sql .=" AND pce_gam_category ='".$dbh->escape($type)."'";
        }
        $sql .=" ORDER BY d.jur_id,pce_start_day DESC";
    }
    elseif($jurisdictionClass=='district'){
        $sql .="LEFT JOIN jurisdiction r ON n.jur_id = r.jur_parent_id
                LEFT JOIN jurisdiction d ON r.jur_id = d.jur_parent_id
                LEFT JOIN jurisdiction c ON d.jur_id = c.jur_parent_id
                LEFT  JOIN currency on cur_id=c.jur_currency
                LEFT JOIN jurisdiction_partial_closure ON c.jur_id = pce_jur_club
                LEFT JOIN admin_user on aus_id=pce_aus_id
                WHERE d.jur_id = $jurisdictionId";
        if(isset($_POST['jurisdiction'])&&($_POST['jurisdiction']!='*')){
            $sql .=" AND c.jur_id=".$_POST['jurisdiction'];
        }
        if(isset($startdate)){
            $sql.= " AND pce_start_day >='$startdate'";
        }
        if(isset($enddate)){
            $sql .=" AND pce_end_day <='$enddate'";
        }
        if($type!=1){
            $sql .=" AND pce_gam_category ='".$dbh->escape($type)."'";
        }
        $sql .=" ORDER BY c.jur_id,pce_start_day DESC";
    }
    /*if($startdate==''){
        $startdate=date('Y-01-01');
     }
     $sql .=" AND pce_start_day BETWEEN '$startdate' AND ";
     if($enddate==''){
         $enddate=date('Y-m-d');
     }
     $sql .="  '$enddate'";*/
    $result = $dbh->exec($sql);
    if($result->getNumRows()==0){
        addError('',$lang->getLang('No result found'));
        showErrors();
    }
    else{
        $parent_details=get_jurisdiction($_SESSION['jurisdiction_id']);
        ?>

        <table class='table table-bordered table-striped table-hover'  >
            <thead>
            <tr>
                <th class='label-inverse' colspan='20'><strong class='label label-inverse'><?=$lang->getLang("Cash-In")?></strong></th>

            </tr>
            <tr>
                <th class='alert alert-success'><strong><?=ucfirst(getSubJurisdiction($_SESSION['jurisdiction_class']))?></strong></th>
                <th class='alert alert-success'><strong><?=$lang->getLang("From")?></strong></th>
                <th class='alert alert-success'><strong><?=$lang->getLang("Until")?></strong></th>
                <th class='alert alert-success'><strong><?=$lang->getLang("Type")?></strong></th>
                <th class='alert alert-info'><strong><?=$lang->getLang("Total bet")?></strong></th>
                <th class='alert alert-info'><strong><?=$lang->getLang("Total win")?></strong></th>
                <th class='alert alert-info'><strong><?=$lang->getLang("Total rake")?></strong></th>
                <th class='alert alert-success'><strong><?=ucfirst($parent_details['jur_name'])?> <?=$lang->getLang("Rake")?></strong></th>
                <th class='alert alert-success'><strong><?=ucfirst(getSubJurisdiction($_SESSION['jurisdiction_class']))?> <?=$lang->getLang("Rake")?></strong></th>
                <!-- <th class='alert alert-info'><strong>Perc</strong></th> -->
                <th class='alert alert-success'><strong><?=$lang->getLang("Note")?></strong></th>
                <th class='alert alert-success'><strong><?=$lang->getLang("User")?></strong></th>
                <th class='alert alert-success'><strong><?=$lang->getLang("Closure time")?></strong></th>
                <th class='alert alert-success'><strong><?=$lang->getLang("Action")?></strong></th>
            </tr>
            </thead>
            <tbody>
            <?
            $entity='';
            while($result->hasNext()){
                $row=$result->next();
                if($row['win']==0 && $row['bet']==0 && $row['rake']== 0){
                    ?>
                    <tr>
                        <td class='text-error'><strong><?=$row['jurisdiction_name']?></strong></td>
                        <td colspan="11" class="text-error"><form action="<?=refFormPage('reports_casino/partial_closure')?>" method='post'><input type='hidden' name='jurisdiction' value='<?=$row['jurisdiction_id']?>'> <strong><?=$lang->getLang("Partial Closure never made!")?></strong> </td>
                        <td><button class="btn btn-danger"><?=$lang->getLang("Do first closure")?></button></form> </td>
                    </tr>
                <?
                }else{ list($usernote,$detailsnote)=explode('^^^',$row['pce_note']);
                    $totalparentrake=0;
                    $detailsnote=explode('#',$detailsnote);
                    foreach($detailsnote as $k=>$v){
                        list($type,$typeamount)=explode(";",$v);
                        $totalparentrake +=$typeamount;
                    }
                    ?>
                    <tr>
                        <td><strong><?=$row['jurisdiction_name']?></strong></td>
                        <td><strong><?=$row['pce_start_day']?></strong></td>
                        <td><strong><?=$row['pce_end_day']?></strong></td>
                        <td><strong><?=($row['pce_gam_category']==7? 'Betting' :'Casino & Poker')?></strong></td>
                        <td><strong><?=getInDollars($row['bet'],2,$row['cur_code_for_web'])?></strong></td>
                        <td><strong><?=getInDollars($row['win'])?></strong></td>
                        <td><strong><?=getInDollars($row['rake'])?></strong></td>
                        <td><strong><?=getInDollars($row['pce_profit'])?></strong>
                        <td><strong><?=getInDollars($row['pce_profit_child'])?></strong></td>
                        <td><strong style="width: 90%;" class="fleft">
                                <div id="<?=$row['pce_id']?>_text" > <?
                                    if($usernote!=''){
                                        echo $usernote;
                                    }
                                    ?></div>
                                <div id="<?=$row['pce_id']?>_edit" style="display: none;"><textarea style="width:90%;"></textarea><br /><input type="button" class="btn btn-info btn-small" value="save" style='padding:0px 3px;font-size: 10px;' onclick="switchBack('<?=$row['pce_id']?>',true);" />&nbsp;<input type="button" class='btn btn-danger' style='padding:0px 3px;font-size: 10px;' value="cancel" onclick="switchBack('<?=$row['pce_id']?>',false);" /></div>
                            </strong>
                            <img  src="live/liveimage/edit.png" id='<?=$row['pce_id']?>_img' style='width: 20px;float:right' onclick="switchToEdit('<?=$row['pce_id']?>_text','<?=$row['pce_id']?>_edit',this);">
                        </td>
                        <td><strong><?=$row['aus_username']?></strong></td>
                        <td><strong><?=$row['pce_time']?></strong></td><td>
                            <?
                            if($row['jurisdiction_name']==$entity){
                                ?>
                            <?
                            }
                            else if($row['jurisdiction_name']!=$entity){
                                $entity=$row['jurisdiction_name'];
                                $now=strtotime(date("Y-m-d 23:59:59"));
                                $d2 = strtotime($row['pce_end_day']);
                                if(($now-$d2) >(3600*24*8) ){
                                    ?>
                                    <form action="<?=refFormPage('reports_casino/partial_closure')?>" method='post'><input type='hidden' name='jurisdiction' value='<?=$row['jurisdiction_id']?>'>
                                        <button class="btn btn-primary" ><?=$lang->getLang("Do a closure")?></button>
                                    </form>
                                <?
                                }
                                else{
                                    ?>
                                    <span></span> <?=$lang->getLang("Up to date!No action available!")?>
                                <?
                                }
                            }
                            ?>
                        </td>
                    </tr>

                <?
                }
            }
            ?>
            </tbody>
        </table>
    <?
    }
}


function getClosureOut($startdate=NULL,$enddate=NULL){
    global $dbh,$lang,$type;
    $jurisdictionClass=$_SESSION['jurisdiction_class'];
    $jurisdictionId=$_SESSION['jurisdiction_id'];
    $sql="select pce_start_day,pce_end_day,pce_time,cur_code_for_web,";
    if($jurisdictionClass=='nation')
    {
        $sql .="ca.jur_name jurisdiction_name,ca.jur_id jurisdiction_id ";
    }
    elseif($jurisdictionClass=='region')
    {
        $sql .="n.jur_name jurisdiction_name,n.jur_id jurisdiction_id ";
    }
    elseif($jurisdictionClass=='district'){
        $sql .="r.jur_name jurisdiction_name,r.jur_id jurisdiction_id ";
    }
    elseif($jurisdictionClass=='club'){
        $sql .="d.jur_name jurisdiction_name,d.jur_id jurisdiction_id ";
    }
    $sql.=",coalesce(pce_bet, 0) bet,
            coalesce(pce_win, 0) win,
            coalesce(pce_rake, 0) rake,
            coalesce(pce_perc,0) perc,
            coalesce(pce_profit, 0) pce_profit,
            coalesce(pce_profit_child, 0) pce_profit_child,
            aus_username,
            pce_note,
            pce_cash_in_hand,
            pce_id,
            pce_gam_category
     ";

    if($jurisdictionClass=='nation')
    {
        $sql .="FROM jurisdiction ca, jurisdiction n, jurisdiction_partial_closure, admin_user,currency
                WHERE n.jur_id = $jurisdictionId
                AND n.jur_parent_id = ca.jur_id
                AND n.jur_currency=cur_id
                AND aus_id = pce_aus_id
                AND pce_jur_nation = n.jur_id
                AND pce_jur_region = 0 ";
    }
    elseif($jurisdictionClass=='region')
    {
        $sql .=" FROM jurisdiction r
                LEFT JOIN currency  ON r.jur_currency = cur_id
                LEFT JOIN jurisdiction n ON n.jur_id = r.jur_parent_id
                LEFT JOIN jurisdiction_partial_closure ON n.jur_id = pce_jur_nation
                LEFT JOIN admin_user on aus_id=pce_aus_id
                WHERE r.jur_id = $jurisdictionId AND pce_jur_district = 0 AND pce_jur_region = r.jur_id";
    }
    elseif($jurisdictionClass=='district'){
        $sql .="FROM jurisdiction d
                LEFT JOIN currency  ON d.jur_currency = cur_id
                LEFT JOIN jurisdiction r ON r.jur_id = d.jur_parent_id
                LEFT JOIN jurisdiction n ON n.jur_id = r.jur_parent_id
                LEFT JOIN jurisdiction_partial_closure ON n.jur_id = pce_jur_nation
                LEFT JOIN admin_user on aus_id=pce_aus_id
                WHERE d.jur_id = $jurisdictionId AND pce_jur_club = 0 AND pce_jur_district = d.jur_id";
    }
    elseif($jurisdictionClass=='club'){
        $sql .="FROM jurisdiction c
                LEFT JOIN currency  ON c.jur_currency = cur_id
                LEFT JOIN jurisdiction d ON d.jur_id = c.jur_parent_id
                LEFT JOIN jurisdiction r ON r.jur_id = d.jur_parent_id
                LEFT JOIN jurisdiction n ON n.jur_id = r.jur_parent_id
                LEFT JOIN jurisdiction_partial_closure ON n.jur_id = pce_jur_nation
                LEFT JOIN admin_user on aus_id=pce_aus_id
                WHERE c.jur_id = $jurisdictionId AND pce_jur_club = c.jur_id ";
    }
    if(isset($startdate)){
        $sql.= " AND pce_start_day >='$startdate'";
    }
    if(isset($enddate)){
        $sql .=" AND pce_end_day <='$enddate'";
    }
    if($type!=1){
        $sql .=" AND pce_gam_category ='".$dbh->escape($type)."'";
    }

    /*if($startdate==''){
        $startdate=date('Y-01-01');
     }
     $sql .=" AND pce_start_day BETWEEN '$startdate' AND ";
     if($enddate==''){
         $enddate=date('Y-m-d');
     }
     $sql .="  '$enddate'";*/
    $result = $dbh->exec($sql);
    if($result->getNumRows()==0){
        addError('','No result found');
        showErrors();
    }
    else{
        ?>
        <table class='table table-bordered table-striped table-hover'  >
            <thead>
            <tr>
                <th class='label-inverse' colspan='15'><strong class='label label-inverse'><?=$lang->getLang("Cash-Out")?></strong></th>
            </tr>
            <tr>
                <th class='alert alert-success'><strong><?=ucfirst(getSuperJurisdiction($_SESSION['jurisdiction_class']))?></strong></th>
                <th class='alert alert-success'><strong><?=$lang->getLang("Start Date")?></strong></th>
                <th class='alert alert-success'><strong><?=$lang->getLang("End Date")?></strong></th>
                <th class='alert alert-success'><strong><?=$lang->getLang("Type")?></strong></th>
                <th class='alert alert-info'><strong><?=$lang->getLang("Total bet")?></strong></th>
                <th class='alert alert-info'><strong><?=$lang->getLang("Total win")?></strong></th>
                <th class='alert alert-info'><strong><?=$lang->getLang("Total rake")?></strong></th>
                <th class='alert alert-success'><strong><?=ucfirst(getSuperJurisdiction($_SESSION['jurisdiction_class']))?> <?=$lang->getLang("Rake")?></strong></th>
                <th class='alert alert-success'><strong><?=ucfirst($_SESSION['jurisdiction_class'])?> <?=$lang->getLang("Rake")?></strong></th>
                <!-- <th class='alert alert-info'><strong>Perc</strong></th>-->
                <th class='alert alert-success'><strong><?=$lang->getLang("Note")?></strong></th>
                <th class='alert alert-success'><strong><?=$lang->getLang("User")?></strong></th>
                <th class='alert alert-success'><strong><?=$lang->getLang("Closure time")?></strong></th>
            </tr>
            </thead>
            <tbody>
            <?  while($result->hasNext()){
                $row=$result->next();
                if($row['win']==0 && $row['bet']==0 && $row['rake']==0){
                    ?>
                    <tr>
                        <td><strong><?=$row['jurisdiction_name']?></strong></td>
                        <td colspan="11" class="text-error"><form action="<?=refFormPage('reports_casino/partial_closure')?>" method='post'>
                                <input type='hidden' name='jurisdiction' value='<?=$row['jurisdiction_id']?>'> <strong><?=$lang->getLang("Partial Closure never made!")?><?=$lang->getLang("Please")?> </strong> <button class="btn btn-danger" ><?=$lang->getLang("Make first closure")?></button>
                            </form>
                        </td>
                    </tr>
                <?
                }else{list($usernote,$detailsnote)=explode('^^^',$row['pce_note']);
                    ?>
                    <tr>
                        <td><strong><?=$row['jurisdiction_name']?></strong></td>
                        <td><strong><?=$row['pce_start_day']?></strong></td>
                        <td><strong><?=$row['pce_end_day']?></strong></td>
                        <td><strong><?=($row['pce_gam_category']==7? 'Betting' :'Casino & Poker')?></strong></td>
                        <td><strong><?=getInDollars($row['bet'])?></strong></td>
                        <td><strong><?=getInDollars($row['win'])?></strong></td>
                        <td><strong><?=getInDollars($row['rake'])?></strong></td>
                        <td><strong><?=getInDollars($row['pce_profit'])?></strong> </td>
                        <td><strong><?=getInDollars($row['pce_profit_child'])?></strong></td>
                        <!-- <td><strong><?/*=$row['perc']*/?> %</strong></td>-->
                        <td><strong><?=$usernote?></strong></td>
                        <td><strong><?=$row['aus_username']?></strong></td>
                        <td><strong><?=$row['pce_time']?></strong></td>
                    </tr>

                <?
                }
            }
            ?>
            </tbody>
        </table>
    <?
    }
}

function getjurisdictionChild ($id,$default=null){
    global $dbh;
    $sql= "SELECT jur_id,jur_name from jurisdiction where jur_parent_id='$id'";
    $result=$dbh->exec($sql);
    $str  = "<select name='jurisdiction' id='jurisdiction'>";
    $str .='<option value="*"> All';
    while($result->hasNext())
    {
        $row=$result->next();
        $str .=  '<option value="' . $row['jur_id'].'"';
        if(isset($default)&&$default==$row['jur_id']){
            $str .=" selected=selected ";
        }
        $str.= ">". $row['jur_name'] . '</option>';
    }
    $str .=  '</select>';
    return $str;
}

function getSubJurisdiction($jurisdictionClass){
    $subjurisdiction='';

    if($jurisdictionClass=='casino')
    {
        $subjurisdiction='nation';
    }
    elseif($jurisdictionClass=='nation')
    {
        $subjurisdiction='region';
    }
    elseif($jurisdictionClass=='region')
    {
        $subjurisdiction='district';
    }
    elseif($jurisdictionClass=='district'){
        $subjurisdiction='club';
    }
    return $subjurisdiction;
}

function getSuperJurisdiction($jurisdictionClass){
    $subjurisdiction='';

    if($jurisdictionClass=='region')
    {
        $subjurisdiction='nation';
    }
    elseif($jurisdictionClass=='nation')
    {
        $subjurisdiction='casino';
    }
    elseif($jurisdictionClass=='district')
    {
        $subjurisdiction='nation';
    }
    elseif($jurisdictionClass=='club'){
        $subjurisdiction='district';
    }
    return $subjurisdiction;
}
?>
