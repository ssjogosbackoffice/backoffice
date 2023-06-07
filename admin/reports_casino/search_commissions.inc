<?php check_access('commissions_search',$redirect=true);?>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap-responsive.css" title="core_style" />
<script src="media/jquery.alerts.js" type="text/javascript"></script>
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
</script>
<?
/**
 * @author Marian
 */
$date_start=$_POST['date_start'];
$date_end=$_POST['date_end'];
$jurisdiction_level=$_POST['jurisdiction_level'];
?>

<table class="table">
    <tr><td class='navbar-inner'><h3 class="text-center"><?=$lang->getLang("Search Jurisdiction Commissions")?></h3></td></tr>
    <tr><td><div class="well">
                <?php
                /**
                 * @author Marian
                 */
                form_head()?>
                <?php printHiddenInput('doQuery', 'yes');?>
                <?php printHiddenInput('search_type', 'date_range');?>
               <div class='well'>
                   <div class='row'>
                       <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Jurisdiction")?></div>
                           <br>
                           <div class="controls">
                               <div class="input-prepend">
                                   <span class="add-on"><i class="icon-random"></i></span>
                                   <select id="jurisdiction_level" name="jurisdiction_level">
                                       <option value="club"<?if ( 'club' == $jurisdiction_level ) echo 'selected="selected"'; ?>> <?=$lang->getLang("Club")?> </option>
                                       <?php if ( 'casino' == $_SESSION['jurisdiction_class'] || 'nation' == $_SESSION['jurisdiction_class'] || 'region' == $_SESSION['jurisdiction_class'] ) { ?>
                                           <option value="district"<?if ( 'district' == $jurisdiction_level ) echo 'selected="selected"'; ?>> <?=$lang->getLang("District")?></option>
                                       <?php } ?>
                                       <?php if ( 'casino' == $_SESSION['jurisdiction_class'] || 'nation' == $_SESSION['jurisdiction_class'] ){ //show region option ?>
                                           <option value="region"<?if ( 'region' == $jurisdiction_level || ! $jurisdiction_level ) echo 'selected="selected"'; ?>><?=$lang->getLang("Region")?></option>
                                       <?php } ?>
                                       <?php if ( 'casino' == $_SESSION['jurisdiction_class'] ){ //show region option ?>
                                           <option value="nation"<?if ( 'nation' == $jurisdiction_level || ! $jurisdiction_level ) echo 'selected="selected"'; ?>><?=$lang->getLang("Nation")?></option>
                                       <?php } ?>
                                   </select>
                               </div>
                           </div>
                       </div>
                   </div>
                   <div class='row'>
                       <div class="span3"><div class='label label-inverse'><?=$lang->getLang("From")?></div>
                           <br>
                           <div class="controls">
                               <div class="input-prepend">
                                   <span class="add-on"><i class="icon-calendar"></i></span>
                                   <input type='text'  name='date_start' id='date_start' value="<?=$date_start?>">
                               </div>
                           </div>

                       </div>

                       <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Until")?></div>
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
               </div>
       </td>
       </tr>
    <?php
    if(isset($_POST['doQuery'])){
        if(!isset($_POST['date_start'])&&!isset($_POST['date_end'])){
            addError('',$lang->getLang('Please complete the interval!'));
            showErrors();
        }
        else{
            getCommissions($_POST['date_start'],$_POST['date_end']);
        }
    }
    ?>

</table>
<?php
function getCommissions($startdate,$enddate){
    global $dbh,$lang;
    $jurisdictionClass=$_SESSION['jurisdiction_class'];
    $jurisdictionId=$_SESSION['jurisdiction_id'];
    $sql="select pce_start_day,pce_end_day,pce_time,pce_profit,pce_aff_id,";
    if($_POST['jurisdiction_level']=='nation'){
        $sql.="n.jur_name nation_name,n.jur_id nation_id, ";
    }
    elseif($_POST['jurisdiction_level']=='region'){
        $sql.="n.jur_name nation_name,n.jur_id nation_id,
         r.jur_name region_name,r.jur_id region_id, ";
    }
    elseif($_POST['jurisdiction_level']=='district'){
        $sql.="n.jur_name nation_name,n.jur_id nation_id,
         r.jur_name region_name,r.jur_id region_id,
         d.jur_name district_name,d.jur_id district_id,";
    }
    elseif($_POST['jurisdiction_level']=='club'){
        $sql.="n.jur_name nation_name,n.jur_id nation_id,
         r.jur_name region_name,r.jur_id region_id,
         d.jur_name district_name,d.jur_id district_id,
         c.jur_name club_name,c.jur_id club_id, ";
    }
    $sql.=" coalesce(pce_bet, 0) bet, coalesce(pce_win, 0) win, coalesce(pce_rake, 0) rake, coalesce(pce_perc,0) perc, aus_username,pce_note,pce_cash_in_hand,pce_id
    FROM jurisdiction n ";

    if($_POST['jurisdiction_level']=='nation'){
        $sql.=" LEFT JOIN jurisdiction_partial_closure ON n.jur_id = pce_jur_nation ";
        $and=" AND pce_jur_region=0 ";
        $group="
                ORDER BY n.jur_id,pce_start_day DESC";
    }
    elseif($_POST['jurisdiction_level']=='region'){
        $sql.=" LEFT JOIN jurisdiction r ON n.jur_id = r.jur_parent_id
                LEFT JOIN jurisdiction_partial_closure ON r.jur_id = pce_jur_region ";
        $and=" AND pce_jur_district=0 ";
        $group="
                ORDER BY r.jur_id,pce_start_day DESC";
    }
    elseif($_POST['jurisdiction_level']=='district'){
        $sql.=" LEFT JOIN jurisdiction r ON n.jur_id = r.jur_parent_id
                LEFT JOIN jurisdiction d ON r.jur_id = d.jur_parent_id
                LEFT JOIN jurisdiction_partial_closure ON d.jur_id = pce_jur_district ";
        $and=" AND pce_jur_club=0 ";
        $group="
                ORDER BY d.jur_id,pce_start_day DESC";
    }
    elseif($_POST['jurisdiction_level']=='club'){
        $sql.=" LEFT JOIN jurisdiction r ON n.jur_id = r.jur_parent_id
                LEFT JOIN jurisdiction d ON r.jur_id = d.jur_parent_id
                LEFT JOIN jurisdiction c ON d.jur_id = c.jur_parent_id
                LEFT JOIN jurisdiction_partial_closure ON c.jur_id = pce_jur_club ";
        $and='';
        $group="
                 ORDER BY c.jur_id,pce_start_day DESC";
    }

    if($_SESSION['jurisdiction_class']=="casino"){
        $where= " WHERE n.jur_parent_id  = 1";
    }
    elseif($_SESSION['jurisdiction_class']=="nation"){
        $where= " WHERE n.jur_id = ".$_SESSION['jurisdiction_id'];
    }
    elseif($_SESSION['jurisdiction_class']=="region"){
        $where= " WHERE r.jur_id = ".$_SESSION['jurisdiction_id'];
    }
    elseif($_SESSION['jurisdiction_class']=="district"){
        $where= " WHERE d.jur_id = ".$_SESSION['jurisdiction_id'];
    }
    elseif($_SESSION['jurisdiction_class']=="club"){
        $where= " WHERE c.jur_id = ".$_SESSION['jurisdiction_id'];
    }

        $sql.=" LEFT JOIN admin_user on aus_id=pce_aus_id ";
        $sql.=$where;
        $sql.=$and;
        $sql.= " AND ( ( pce_start_day >='$startdate' AND  pce_start_day <='$enddate') ";
        $sql .=" OR (pce_end_day >='$startdate' AND pce_end_day <='$enddate')) ";
        $sql .= $group;
    $result = $dbh->exec($sql);
    if($result->getNumRows()==0){
        addError('',$lang->getLang('No result found'));
        showErrors();
    }
    else{
        $totalbet=0;
        $totalwin=0;
        $totalrake=0;
        $totcashinhand=0;
        $totalprofit=0;
        $results="<table class='table table-bordered table-striped table-hover'  >
            <thead>
            <tr>
                <th class='label-inverse' colspan='20'><strong class='label label-inverse'>".$lang->getLang('Cash-In / Cash-Out')."</strong></th>
            </tr>
            <tr>
                <th class='alert alert-success'><strong>".$lang->getLang('From')."</strong></th>
                <th class='alert alert-success'><strong>".$lang->getLang('Until')."</strong></th>";
      $results.="<th class='alert alert-error'><strong>".$lang->getLang('Nation')."</strong></th>";

        if($_POST['jurisdiction_level']=='region'){

            $results.="<th class='alert alert-error'><strong>".$lang->getLang("Region")."</strong></th>";
        }
        if($_POST['jurisdiction_level']=='district'){

            $results.="<th class='alert alert-error'><strong>".$lang->getLang("Region")."</strong></th>
            <th class='alert alert-error'><strong>".$lang->getLang("District")."</strong></th>";
        }
        if($_POST['jurisdiction_level']=='club'){

            $results.="<th class='alert alert-error'><strong>".$lang->getLang("Region")."</strong></th>
            <th class='alert alert-error'><strong>".$lang->getLang("District")."</strong></th>
            <th class='alert alert-error'><strong>".$lang->getLang("Club")."</strong></th>";
        }
    $results.=" <th class='alert alert-info'><strong>".$lang->getLang("Total bet")."</strong></th>
                <th class='alert alert-info'><strong>".$lang->getLang("Total win")."</strong></th>
                <th class='alert alert-info'><strong>".$lang->getLang("Total net/rake")."</strong></th>
                <th class='alert alert-info'><strong>".$lang->getLang("Cash in hand")."</strong></th>
                <th class='alert alert-info'><strong>".$lang->getLang("Profit")."</strong></th>
                <th class='alert alert-info'><strong>".$lang->getLang("Perc")."</strong></th>
                <th class='alert alert-success'><strong>".$lang->getLang("Note")."</strong></th>
                <th class='alert alert-success'><strong>".$lang->getLang("User")."</strong></th>
                <th class='alert alert-success'><strong>".$lang->getLang("Closure time")."</strong></th>
            </tr>
            </thead>
            <tbody>";
            $commissions=array(1=>'Banco',2=>'Provvigione',3=>"Banco Plus", 4=>"Misto");
            while($result->hasNext()){
                $row=$result->next();
                $row['perc']=explode(';',$row['perc']);
                if($row['perc']['1']!=null){
                    $type=explode(':',$row['perc'][0]);
                    $row['perc'][0]=$commissions[$type[1]];
                }
                else{
                    $row['perc'][0]=$row['perc'][0]." %";
                }
                $totalbet+=$row['bet'];
                $totalwin+=$row['win'];
                $totalrake+=$row['rake'];
                $totcashinhand+=$row['pce_cash_in_hand'];
                $totalprofit+=$row['pce_profit'];
                $results.="  <tr>
                        <td><strong>".$row['pce_start_day']."</strong></td>
                        <td><strong>".$row['pce_end_day']."</strong></td>
                        <td><strong>".$row['nation_name']."</strong></td>";

                if($_POST['jurisdiction_level']=='region'){

                    $results.="
                               <td><strong>".$row['region_name']."</strong></td>";
                }
                if($_POST['jurisdiction_level']=='district'){

                    $results.="
                               <td><strong>".$row['region_name']."</strong></td>
                               <td><strong>".$row['district_name']."</strong></td>";
                }
                if($_POST['jurisdiction_level']=='club'){

                    $results.="
                               <td><strong>".$row['region_name']."</strong></td>
                               <td><strong>".$row['district_name']."</strong></td>
                               <td><strong>".$row['club_name'];
                    if($row['pce_aff_id']>0){
                        $results.="<br /><span class='tip'> ( Affiliate ) </span> ";
                    }
                    $results.="</strong></td>";
                }
                       $results.=" <td><strong>".getInDollars($row['bet'])."</strong></td>
                        <td><strong>".getInDollars($row['win'])."</strong></td>
                        <td><strong>Net:".getInDollars($row['bet']-$row['win'])."<br />Rake:".getInDollars($row['rake'])."</strong></td>
                        <td><strong>".getInDollars($row['pce_cash_in_hand'])."</strong></td>
                        <td><strong>".getInDollars($row['pce_profit'])."</strong></td>
                        <td><strong>".($row['perc'][0])."</strong></td>
                        <td><strong style='width: 90%;' class='fleft'>
                                <div id='".$row['pce_id']."_text' >";
                                    if($row['pce_note']!=''){
                                        $results.=$row['pce_note'];
                                    }
                    $results.="</div>
                            </strong>
                        </td>
                        <td><strong>".$row['aus_username']."</strong></td>
                        <td><strong>".$row['pce_time']."</strong></td>
                    </tr>";
                }
            $results.="</tbody>
        </table>";

        $totals="<table class='table table-bordered table-striped table-hover' >
            <thead>
            <tr>
                <th class='label-inverse' colspan='20'><strong class='label label-inverse'>".$lang->getLang('Totals')."</strong></th>
            </tr>
            <tr>
                <th class='alert alert-success'><strong>".$lang->getLang('Total bet')."</strong></th>
                <th class='alert alert-success'><strong>".$lang->getLang('Total win')."</strong></th>
                <th class='alert alert-success'><strong>".$lang->getLang('Total rake')."</strong></th>
                <th class='alert alert-success'><strong>".$lang->getLang("Total Cash in hand")."</strong></th>
                <th class='alert alert-success'><strong>".$lang->getLang("Total profit")."</strong></th>
            </tr>
            </thead>
            <tbody>
            <tr>
                        <td><strong>".getInDollars($totalbet)."</strong></td>
                        <td><strong>".getInDollars($totalwin)."</strong></td>
                        <td><strong>".getInDollars($totalrake)."</strong></td>
                        <td><strong>".getInDollars($totcashinhand)."</strong></td>
                        <td><strong>".getInDollars($totalprofit)."</strong></td>
                        </tr></table>
            ";
        echo $totals."<br />".$results;
    }
}
?>