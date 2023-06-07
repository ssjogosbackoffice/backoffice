<?php check_access('manage_promotions',$redirect=true);
$date_start= (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d",time()-3600*24);
$date_end= (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d");
$date_start_new= (isset($_POST['date_start_new'])) ? $_POST['date_start_new'] : date("Y-m-d");
$date_end_new= (isset($_POST['date_end_new'])) ? $_POST['date_end_new'] : date("Y-m-d",time()+3600*24*30);
$status = (isset($_POST["status"])) ? $_POST["status"] : 2 ;
$period = (isset($_POST["period"])) ? $_POST["period"] : 0 ;
$active=(isset($_POST["operation"])) ? $_POST["operation"] : 'list' ;
if(isset($_POST['doQuery'])&& $_POST['operation']=='add'){
    $name=$_POST['name_new'];
    $description=$_POST['description_new'];
    $code_new=$_POST['code_new'];
    $start_date=$_POST['date_start_new'];
    $end_date=$_POST['date_end_new'];
    $status_new=$_POST['status_new'];
    $op_code=$_POST['op_code_new'];
    $op_params=$_POST['op_params_new'];
    $jur_id=$_POST['jurisdiction_new'];
    $public=$_POST['public_new'];
    if($_POST['games_new'][0]=='multiselect-all'){
        array_shift($_POST['games_new']);
    }
    $game_code=implode(',',$_POST['games_new']);
    $ptn_id=$_POST['partners_new'];
    $pes_id=$_POST['providers_new'];
}

?>
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap-responsive.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="/media/bootstrap/css/bootstrap-multiselect.css" title="core_style">
    <script src="/media/bootstrap/js/bootstrap-multiselect.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
    <script type="text/javascript">
        $(function() {
            $( "#date_start" ).datepicker({
                changeMonth: true,
                changeYear:true,
                maxDate:-1,
                numberOfMonths:1

            });
            $( "#date_end" ).datepicker({
                changeMonth: true,
                changeYear:true,
                numberOfMonths:1
            });
            $( "#date_start_new" ).datepicker({
                changeMonth: true,
                changeYear:true,
                minDate:0,
                numberOfMonths:1

            });
            $( "#date_end_new" ).datepicker({
                changeMonth: true,
                changeYear:true,
                numberOfMonths:1,
                minDate:'+1m'
            });
            $(".alignment .btn").click(function() {
                // whenever a button is clicked, set the hidden helper
                $("#period").val($(this).val());
                if($(this).val()==0){
                    $(".specific_date").hide("fast");
                }
                else{
                    $(".specific_date").show("fast");
                }
            });
             $('#games_new').multiselect({
                 includeSelectAllOption: true,
                 enableFiltering: true,
                 maxHeight: 300,
                 buttonClass: 'btn btn-primary disabled',
                 buttonWidth:'220px'
             });
        });
    </script>
    <table class="table table-bordered">
    <tr><td class='navbar-inner'><h3 class="text-center"><?=$lang->getLang("Promotions")?></h3></td></tr>
    <tr>
    <td>
    <ul class="nav nav-tabs" id='myTab'>
        <li <?=($active=='list') ? "class='active'":""?>><a href="#list" data-toggle="tab"><button class="btn btn-success"><?=$lang->getLang("List")?></button></a></li>
        <li <?=($active=='add') ? "class='active'":""?>><a href="#insert" data-toggle="tab"><button class="btn btn-success"><?=$lang->getLang("Add")?></button> </a></li>
    </ul>
    <div class="tab-content" style="overflow: visible">
    <div class="tab-pane fade <?=($active=='list') ? "in active":""?>" id="list">
        <?php
        /**
         * @author Marian
         */
        form_head()?>
        <?php printHiddenInput('doQuery', 'yes');?>
        <?php printHiddenInput('operation', 'list');?>
        <div class='well'>
            <div class='row'>
                <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Period")?></div>
                    <br>
                    <div class="btn-group alignment" data-toggle="buttons-radio">
                        <button type="button" class="btn btn-primary <?=($period=='0') ? "active":""?>" value="0"><?=$lang->getLang("Any Time")?></button>
                        <button type="button" class="btn btn-primary <?=($period=='1') ? "active":""?>" value="1"><?=$lang->getLang("Specific period")?></button>
                    </div>
                    <input type="hidden" id="period" name='period' value="<?=$period?>" />
                </div>
                <div class="span3 specific_date" <?=($period=='0') ? "style='display:none'":""?>><div class='label label-inverse'><?=$lang->getLang("From")?></div>
                    <br>
                    <div class="controls">
                        <div class="input-prepend">
                            <span class="add-on"><i class="icon-calendar"></i></span>
                            <input type='text'  name='date_start' id='date_start' value="<?=$date_start?>">
                        </div>
                    </div>
                </div>
            </div>
            <div class='row'>
                <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Status")?></div>
                    <br>
                    <div class="controls">
                        <div class="input-prepend">
                            <span class="add-on"><i class="icon-random"></i></span>
                            <select name='status'>
                                <option value='2' <?=($status=='2') ? "selected='selected'":""?> ><?=$lang->getLang("All")?></option>
                                <option value='1' <?=($status=='1') ? "selected='selected'":""?>><?=$lang->getLang("Active")?></option>
                                <option value='0' <?=($status=='0') ? "selected='selected'":""?>><?=$lang->getLang("Inactive")?></option>
                            </select>
                        </div>
                    </div>
                </div>
                <div class="span3 specific_date" <?=($period=='0') ? "style='display:none'":""?>><div class='label label-inverse'><?=$lang->getLang("Until")?></div>
                    <br>
                    <div class="controls">
                        <div class="input-prepend">
                            <span class="add-on"><i class="icon-calendar"></i></span>
                            <input type='text'  name='date_end' id='date_end' value="<?=$date_end?>">
                        </div>
                    </div>
                </div>
            </div>
            <button class="btn btn-primary" ><?=$lang->getLang("Search")?></button>
            </form><br><br>
            <?php
            if(isset($_POST['doQuery']) && $_POST['operation']=='list'){
                ?>
                <div id='results'>
                    <?
                    $date_start=$_POST['date_start'];
                    $date_end=$_POST['date_end'];
                    $d1 = strtotime("$date_start");
                    $d2 = strtotime("$date_end");
                    if($d2-$d1<0){
                        addError("",$lang->getLang("Please choose an end date greater than start date"));
                    }
                    if(!areErrors())
                    {

                        $sql="Select * from promotion where 1=1 ";
                        if($period=='1'){
                        $sql.=" AND  prm_start_date >'$date_start' AND prm_end_date<'$date_end'";
                        }
                        if($status!='2'){
                        $sql .=" AND prm_status=$status";
                        }
                        $result = $dbh->exec($sql);

                        if($result->getNumRows()==0){
                            addError("","No results found!");
                            showErrors();
                        }
                        else
                        {
                            ?>
                            <table class='table table-hover table-striped table-bordered' >
                                <thead>
                                <tr  class='alert alert-info'><th><?=$lang->getLang("Username")?></th>
                                    <th><?=$lang->getLang("Description")?></th>
                                    <th><strong><?=$lang->getLang("Code")?></strong></th>
                                    <th><strong><?=$lang->getLang("From")?></strong></th>
                                    <th><strong><?=$lang->getLang("Until")?></strong></th>
                                    <th><strong><?=$lang->getLang("Status")?></strong></th>
                                    <th><strong><?=$lang->getLang("Op Code")?></strong></th>
                                    <th><strong><?=$lang->getLang("Params")?></strong></th></tr>
                                </thead>
                                <tbody>
                                <?
                                while($result->hasNext())
                                {
                                    $row=$result->next();
                                    ?>
                                    <tr>
                                        <td>
                                            <?=$row['prm_name']?>
                                        </td>
                                        <td>
                                            <?=$row['prm_description']?>
                                        </td>
                                        <td>
                                            <?=$row['prm_code']?>
                                        </td>
                                        <td>
                                            <?=$row['prm_start_date']?>
                                        </td>
                                        <td>
                                            <?=$row['prm_end_date']?>
                                        </td>
                                        <td class="<?=($row['prm_status']=='0') ? "text-error":"text-success"?>">
                                            <?if($row['prm_status']=='0'){
                                                ?><?=$lang->getLang("expired")?>
                                                <?
                                            }
                                            else{
                                                ?>
                                                <?=$lang->getLang("Active")?>
                                                <?
                                            }
                                            ?>
                                        </td>
                                        <td>
                                            <?=$row['prm_op_code']?>
                                        </td>
                                        <td>
                                            <?=$row['prm_op_params']?>
                                        </td>

                                    </tr>
                                <?
                                }
                                ?>
                                </tbody>
                            </table>
                        <?
                        }
                    }
                    else{
                        showErrors();
                    }
                    ?>
                </div>

            <?
            }
            ?>
        </div>
    </div>

    <div class="tab-pane fade <?=($active=='add') ? "in active":""?>" id="insert">
        <?php form_head()?>
        <?php printHiddenInput('doQuery', 'yes');?>
        <?php printHiddenInput('operation', 'add');?>
        <div class='well'>
            <div class='row'>
                <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Name")?></div>
                    <br>
                    <div class="controls">
                        <div class="input-prepend">
                            <span class="add-on"><i class="icon-align-justify"></i></span>
                            <input type='text'  name='name_new'  placeholder="<?=$lang->getLang("New promotion name")?>" value="<?=$name?>">
                        </div>
                    </div>
                </div>
                <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Available From")?></div>
                    <br>
                    <div class="controls">
                        <div class="input-prepend">
                            <span class="add-on"><i class="icon-calendar"></i></span>
                            <input type='text'  name='date_start_new' id='date_start_new' value="<?=$date_start_new?>">
                        </div>
                    </div>
                </div>
                <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Code")?></div>
                    <br>
                    <div class="controls">
                        <div class="input-prepend">
                            <span class="add-on"><i class="icon-lock"></i></span>
                            <input type='text'  name='code_new'  placeholder="<?=$lang->getLang("Promotion code(max. 32 char)")?>" maxlength="32" value="<?=$code_new?>">
                        </div>
                    </div>
                </div>
                <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Op Code")?></div>
                    <br>
                    <div class="controls">
                        <div class="input-prepend">
                            <span class="add-on"><i class="icon-lock"></i></span>
                            <?=printOpCode($op_code)?>
                        </div>
                    </div>
                </div>

            </div>

            <div class='row'>
                <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Description")?></div>
                    <br>
                    <div class="controls">
                        <textarea  name='description_new' id='description_new'class="row-fluid" rows="3" placeholder="<?=$lang->getLang("Insert promotion description")?>"><?=$description?></textarea>
                    </div>
                </div>
                <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Available Until")?></div>
                    <br>
                    <div class="controls">
                        <div class="input-prepend">
                            <span class="add-on"><i class="icon-calendar"></i></span>
                            <input type='text'  name='date_end_new' id='date_end_new' value="<?=$date_end_new?>">
                        </div>
                    </div>
                </div>
                <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Status")?></div>
                    <br>
                    <div class="controls">
                        <div class="input-prepend">
                            <span class="add-on"><i class="icon-random"></i></span>
                            <select name='status_new'>
                                <option value='0'><?=$lang->getLang("Active")?></option>
                                <option value='1'><?=$lang->getLang("Inactive")?></option>
                            </select>
                        </div>
                    </div>
                </div>
                <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Op params")?></div>
                    <br>
                    <div class="controls">
                        <textarea  name='op_params_new' id='op_params_new'class="row-fluid" rows="3" placeholder="<?=$lang->getLang("Insert promotion params")?>"><?=$op_params?></textarea>
                    </div>
                </div>
            </div>

            <div class='row'>
                <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Jurisdiction")?></div>
                    <br>

                        <div class="input-prepend">
                            <span class="add-on"><i class="icon-user"></i></span>
                            <select name='jurisdiction_new' id='jurisdiction_new'>
                              <option value='*'>All</option>
                               <?$allclubs= getAllClubs();
                                while($allclubs->hasNext())
                                {
                                $row=$allclubs->next(); ?>
                                <option value='<?=$row["jur_id"]?>'><?=$row['jur_name']?></option>
                                <? }?>
                                </select>
                        </div>
                </div>
                <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Public")?></div>
                    <br>
                    <div class="controls">
                        <div class="input-prepend">
                            <span class="add-on"><i class="icon-lock"></i></span>
                            <input type='text'  name='public_new'  placeholder="<?=$lang->getLang("Promotion public")?>" value="<?=$public?>">
                        </div>
                    </div>
                </div>
                <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Partner")?></div>
                    <br>
                    <div class="controls">
                        <div class="input-prepend">
                            <span class="add-on"><i class="icon-user"></i></span>
                           <?=printPartners($ptn_id)?>
                        </div>
                    </div>
                </div>
                <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Provider")?></div>
                    <br>
                    <div class="controls">
                        <div class="input-prepend">
                            <span class="add-on"><i class="icon-user"></i></span>
                           <?=printProviders($pes_id)?>
                        </div>
                    </div>
                </div>
                </div>
            <div class="row">
                <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Game")?></div>
                    <br>
                    <div class="controls">
                        <div class="input-prepend input-append">
                            <span class="add-on"><b class="icon-list-alt"></b></span>
                            <?=printGamesList()?>
                        </div>
                    </div>
                </div>
                </div>
            <button class="btn btn-primary" ><?=$lang->getLang("Add")?></button>
            <br>
            <div class="row">
            <?
            if(isset($_POST['doQuery'])&& $_POST['operation']=='add'){
                addPromotion($name,$description,$code_new,$start_date,$end_date,$status,$op_code,$op_params,$jur_id,$public,$game_code,$ptn_id,$pes_id);
            }
            ?>
            </div>
        </div>
    </div>
    </td>
    </tr>
    </table>
<?php

/*function getAllClubs (){
    global $dbh;
    $jur_class=$_SESSION['jurisdiction_class'];
    $jur_id=$_SESSION['jurisdiction_id'];
    $sql= "SELECT c.jur_name,c.jur_id from jurisdiction c ";
    if($jur_class == "club"){
        $sql .= " WHERE c.jur_id = " . $jur_id;
    }
    elseif($jur_class == "district"){
        $sql .= " JOIN jurisdiction d on d.jur_id = c.jur_parent_id
				  WHERE d.jur_id = " . $jur_id;
    }
    elseif($jur_class=="region"){
        $sql .=" JOIN jurisdiction d on d.jur_id = c.jur_parent_id
				JOIN jurisdiction r on r.jur_id = d.jur_parent_id
				WHERE r.jur_id=".$jur_id;
    }
    elseif ($jur_class=="nation"){
        $sql .=" JOIN jurisdiction d on d.jur_id = c.jur_parent_id
				JOIN jurisdiction r on r.jur_id = d.jur_parent_id
				JOIN jurisdiction n on n.jur_id = r.jur_parent_id
				WHERE n.jur_id=".$jur_id;
    }
    else{
        $sql.=' WHERE 1=1 ';
    }
    $sql .=" AND c.jur_class='club' group by c.jur_id";
    $result=$dbh->exec($sql);
    $str  = "<select name='jurisdiction_new' id='jurisdiction_new'>";
    if($jur_class!="club"){
            $str.="<option value='*'>All</option> ";
            }
    while($result->hasNext())
    {
        $row=$result->next();
        echo $row['jur_id']."=>".$row['jur_name'];
        $str .="<option value='".$row["jur_id"]."'>".$row['jur_name']."</option>";
    }
    $str .=  '</select>';
    return $str;
}*/

function printGamesList(){
    $rs=getAllGames();
    $return =" <select name='games_new[]' id='games_new' multiple='multiple' class='multiselect'>";
        while ($rs->hasNext()){
            $row = $rs->next();
            $return.="<option value='".$row["gam_id"]."'>".$row["gam_name"]."</option>";
        }
    $return.="</select>";
    return $return;
}

function addPromotion($name,$description,$code_new,$start_date,$end_date,$status_new,$op_code,$op_params,$jur_id,$public,$game_code,$ptn_id,$pes_id){
    global $dbh,$lang;
    if($name==''){
        addError('',$lang->getLang('Please enter a name'));
    }
    if($description==''){
        addError('',$lang->getLang('Please enter a description'));
    }
    if($code_new==''){
        addError('',$lang->getLang('Please enter a code'));
    }
    if($start_date==''){
        addError('',$lang->getLang('Please enter a start date'));
    }
    if($end_date==''){
        addError('',$lang->getLang('Please choose a end date'));
    }
    if(count($code_new)>32){
        addError('',$lang->getLang('Please enter a code smaller than 32 characters'));
    }
    if(!areErrors()){
    $sql="Insert into promotion (prm_id,prm_name,prm_description,prm_code,prm_start_date,prm_end_date,prm_status,prm_op_code,prm_op_params ";
     if($jur_id!='*'){
         $sql.=' ,prm_jur_id';
     }
    if($ptn_id!='*'){
        $sql.=' ,prm_ptn_id';
    }
    if($pes_id!='*'){
        $sql.=' ,prm_pes_id';
    }
    $sql.=",prm_public,prm_game_code) VALUES ('".$dbh->nextSequence('PRM_ID_SEQ')."','$name','$description','$code_new','$start_date','$end_date','$status_new','$op_code','$op_params'";
    if($jur_id!='*'){
        $sql.=",'$jur_id'";
    }
    if($ptn_id!='*'){
        $sql.=" ,'$ptn_id'";
    }
    if($pes_id!='*'){
        $sql.=" ,'$pes_id'";
    }
    $sql.=",'$public','$game_code')";
     $res=$dbh->exec($sql);

    if($res>0){
        ?>
        <h2 class="span10 text-success"><?=$lang->getLang('You have successfully added a promotion');?></h2>
    <?
    }
    elseif(areErrors())
    {
        ?>
        <h2 class="span10 error"><?=showErrors();?></h2>
        <?
    }
    else{
        ?>
        <h2 class="span10 error"><?=$lang->getLang('An error has occured.Promotion was not inserted');?></h2>
        <?
    }
    }else{
        ?>
       <span class="span-10">
           <?
        showErrors();
        ?>
           </span>
        <?
    }
    return;

}
function printOpCode($default=''){
    global $dbh;

    $sql='Select * from promotion_operations';
    $res=$dbh->doCachedQuery($sql,0);
    $return =" <select name='op_code_new' id='op_code_new'>";
    while ($res->hasNext()){
        $row = $res->next();
        $return.="<option value='".$row["pmt_code"]."'";
        if($default==$row['pmt_code']){
            $return .=" selected='selected' ";
        }
        $return.=">".$row["pmt_code"]."</option>";
    }
    $return.="</select>";
    return $return;
}

function printPartners($default='') {
    global $dbh, $lang ;
    $sql = "Select * from partners";
    $result = $dbh->doCachedQuery ( $sql, 0 );
    $return =" <select name='partners_new' id='partners_new'>";
    $return .="<option value='-1'>".$lang->getLang('Deny')."</option>";
    $return .="<option value='*'>".$lang->getLang('All')."</option>";
    while ($result->hasNext()){
        $row = $result->next();
        $return.="<option value='".$row["ptn_id"]."'";
        if($default==$row['ptn_id']){
            $return .=" selected='selected' ";
        }
        $return.=">".$row["ptn_name"]."</option>";
    }
    $return.="</select>";
    return $return;
}

function printProviders($default='') {
    global $dbh,$lang;
    $sql = "Select * from providers";
    $result = $dbh->doCachedQuery ( $sql, 0 );
    $return =" <select name='providers_new' id='providers_new'>";
    $return .="<option value='-1'>".$lang->getLang('Deny')."</option>";
    $return .="<option value='*'>".$lang->getLang('All')."</option>";
    while ($result->hasNext()){
        $row = $result->next();
        $return.="<option value='".$row["pes_id"]."'";
            if($default==$row['pes_id']){
                $return .=" selected='selected' ";
            }
        $return.=">".$row["pes_name"]."</option>";
    }
    $return.="</select>";
    return $return;
}
?>