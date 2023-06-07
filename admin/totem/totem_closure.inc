<?php check_access('totem_closure_manage',$redirect=true);
$date_end= (isset($_POST['date_start'])) ? $_POST['date_end'] : date("Y-m-d", time()-3600*24);
?>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap-responsive.css" title="core_style" />
<script src="media/jquery.alerts.js" type="text/javascript"></script>
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
            maxDate:-1,
            numberOfMonths:1
        });

        $( "#searchdatefrom" ).datepicker({
            changeMonth: true,
            maxDate:-1,
            numberOfMonths:1
        });

        $("form").submit( function(event, skip) {
            if (skip) {
                return true;
            }
            event.preventDefault();
            var self = this;
            if($('#date_start').children().length > 0){
                jAlert('<?=$lang->getlang("There is no date available")?>');
            }
            else if($('#percentage').children().length > 0 ){
                jAlert('<?=$lang->getlang("Before please configure the percentuals!")?>');
            }
            else if($('#date_end').val()== '<?=$lang->getLang("No data available")?>' ){
                jAlert('<?=$lang->getlang("No end date available!")?>');
            }
            else{
                $(self).trigger('submit', true);
            }
        });
    });


    function toggleSearchDate(){
        $('#searchDate').toggleClass('hide show');
    }


    Date.prototype.yyyymmdd = function() {
        var yyyy = this.getFullYear().toString();
        var mm = (this.getMonth()+1).toString(); // getMonth() is zero-based
        var dd  = this.getDate().toString();
        return yyyy +"-"+(mm[1]?mm:"0"+mm[0]) +"-"+ (dd[1]?dd:"0"+dd[0]); // padding
    };


    function confirmClosure(date_start,date_end,bet,win,rake,fin,perc,note,id,ca,n,re,d,c){
        jConfirm('Are you sure?','Please Confirm', function(r) {
            if (r){
                $.post("totem/totem_closure_service.inc",{action:'3',date_start:date_start,date_end:date_end,bet:bet,win:win,rake:rake,fin:fin,perc:perc,note:note,id:id,ca:ca,n:n,re:re,d:d,c:c}, function(data){
                    if(data=='1'){
                        $('#confirmClosure').empty().append( '<h3 class="alert alert-success"><strong><?=$lang->getLang("Partial Closure successfully made!")?></strong></h3>');
                        var currentTime = new Date()
                        var month = ('0' + (currentTime.getMonth()+1)).slice(-2);
                        var day = ('0' + currentTime.getDate()).slice(-2) ;
                        var year = currentTime.getFullYear();
                        today=(year+ "-" + month + "-" + day);
                        var myDate = new Date(date_end);
                        myDate.setDate(myDate.getDate() + 1);
                        $('#date_start').empty().append(myDate.yyyymmdd());
                        $('#ds').val(myDate.yyyymmdd());
                        if(myDate.yyyymmdd()==today){
                            $('#date_end').val('<?=$lang->getLang("No data available")?>');
                        }
                        else {
                            newdate=new Date(date_end);
                            newdate.setDate(newdate.getDate()+7)
                            $("#date_end").datepicker( "setDate" ,newdate);
                        }
                    }
                    else  {
                        $('#confirmClosure').empty().append('<h3 class="alert alert-error"><?=$lang->getLang("Partial closure already made!")?></h3>');
                    }
                });
            }
        });
    }

    function getPercent(parent){
        $.post("totem/totem_closure_service.inc",{action:'1',parent:parent,child:$('select#jurisdiction option:selected').val() }, function(data){
            if(data==''){
                $('#percentage').empty().append('<div class="errorbold"><?=$lang->getLang("Please set the percentuals!")?></div>');
                $("#percentagelabel").attr('class', 'label label-important');
            }
            else if ($.isNumeric(data)) {
                $('#percentage').empty().append( data +'%');
                $('#perc').val(data);
                $("#percentagelabel").attr('class', 'label label-inverse');
            }
            else{
                $('#percentage').empty().append('<div class="errorbold"><?=$lang->getLang("Please configure the following percentuals:")?><br></div><div class="error">'+data+'</div>');
                $("#percentagelabel").attr('class', 'label label-important');
            }
        });
    }
    /*    function getJurDate(){
     $('#results').hide();
     $('#confirmClosure').hide();
     $.post("totem/totem_closure_service.inc",{action:'2',thisclass:'<?=getSubJurisdiction($_SESSION['jurisdiction_class'])?>',id:$('select#jurisdiction option:selected').val()}, function(data){
     $('#date_start').empty().append(data);

     var currentTime = new Date()
     var month = ('0' + (currentTime.getMonth()+1)).slice(-2);
     var day = ('0' + currentTime.getDate()).slice(-2) ;
     var year = currentTime.getFullYear();
     today=(year+ "-" + month + "-" + day);

     if(today==data){
     $('#date_end').val('No data available');
     }
     else
     {
     $('#date_end').val('<?=$date_end?>');
     }

     $('#ds').val(data);

     if(data=="<span class='errorbold'>No data available</span>")
     {
     $("#datestartlabel").attr('class', 'label label-important');
     }
     else{
     $("#datestartlabel").attr('class', 'label label-inverse');
     }

     });
     }*/

    function getClosure(parent,parentclass){
        if ($('#searchBox').is(':checked')) {
            datetosend=$('#searchdatefrom').val();
        } else {
            datetosend='';
        }
        $.post("totem/totem_closure_service.inc",{action:'4',parent:parent,parentclass:parentclass,date_start:datetosend }, function(data){
            $('#closureResults').empty().append( data);

        });
    }

</script>

<!--    select r.jur_id, coalesce(ttc_bet, 0), coalesce(ttc_win, 0), coalesce(ttc_rake, 0), coalesce(ttc_perc,0) ttc_aus_id
    FROM jurisdiction n LEFT JOIN jurisdiction r ON n.jur_id = r.jur_parent_id LEFT JOIN
    totem_closure ON r.jur_id = ttc_jur_region
    WHERE n.jur_id = 7
    [16:39:46] Alessio5.5:



    select n.jur_id, coalesce(ttc_bet, 0), coalesce(ttc_win, 0), coalesce(ttc_rake, 0), coalesce(ttc_perc,0) ttc_aus_id
    FROM jurisdiction n LEFT JOIN totem_closure ON n.jur_id = ttc_jur_nation
    WHERE n.jur_parent_id  = 1
-->
<table class="table">
<tr><td class='navbar-inner'><h3 class="text-center"><?=$lang->getLang("Totem Closure")?></h3></td></tr>
<tr><td>
<?php
/**
 * @author Marian
 */
form_head()?>
<?php printHiddenInput('doQuery', 'yes');?>
<?php printHiddenInput('search_type', 'date_range');?>
<div class='well'>
    <div class='row'>
        <div class='span3'><div class='label label-inverse'><?=$lang->getLang("User")?></div> <br><div>
                <span class="add-on"><i class="icon-user"></i> <?=$_SESSION['admin_username']?></span>
            </div>
        </div>

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

        <div class='span3'><div class='label label-inverse'><?=$lang->getLang("Note")?></div> <br><div><textarea rows="3" name='note' ><?=$_POST['note']?></textarea> </div>
        </div>
    </div>
    <button class="btn btn-primary" ><?=$lang->getLang("Calculate")?></button>
    </form><br><br>
    <div id='confirmClosure'></div>
</div>


<?php
if(isset($_POST['doQuery'])){
    ?>
    <div id='results'>
        <?
        $parent=$_SESSION['jurisdiction_id'];
        $child=$_POST['jurisdiction'];
        $date_start=$_POST['date_start'];
        $d1 = strtotime("$date_start");
        $d2 = strtotime("$date_end");
        if($d2-$d1<0){
            addError("",$lang->getLang("Please choose an end date greater than start date"));
        }
        $now=strtotime(date("Y-m-d 23:59:59"));
        if($now-$d2<'86400'){
            addError("",$lang->getLang("The end date can't be greater than yesterday"));
        }
        if(!areErrors())
        {
            // $child_details=get_jurisdiction($child);
            $partialClosure=calculatePartialClosure($date_start,$date_end);
            if($partialClosure==0){
                addError("","No results found!");
                showErrors();
            }
            else
            {

                ?>

                <table class='table table-bordered' >
                    <tr><td colspan="15" class="badge-inverse"><div class='label label-inverse'><?=$lang->getLang("Results  from % until %",$date_start,$date_end)?></div></td> </tr>
                    <tr>

                        <? if($_SESSION['jurisdiction_class']=='casino'){
                            ?>
                            <td class='alert alert-error'><strong><?=$lang->getLang("Nation")?></strong></td>
                            <td class='alert alert-warning'><strong><?=$lang->getLang("Region")?></strong></td>
                            <td class='alert alert-info'><strong><?=$lang->getLang("District")?></strong></td>
                            <td class='alert alert-success'><strong><?=$lang->getLang("Club")?></strong></td>
                            <td class='label-inverse' style='border-top: 0px'><strong class='label label-inverse'><?=$lang->getLang("Totem Code")?></strong></td>
                        <?
                        }
                        elseif($_SESSION['jurisdiction_class']=='nation'){
                            ?>
                            <td class='alert alert-warning'><strong><?=$lang->getLang("Region")?></strong></td>
                            <td class='alert alert-info'><strong><?=$lang->getLang("District")?></strong></td>
                            <td class='alert alert-success'><strong><?=$lang->getLang("Club")?></strong></td>
                            <td class='alert alert-success'><strong><?=$lang->getLang("Totem Code")?></strong></td>
                        <?
                        }
                        elseif ($_SESSION['jurisdiction_class']=='region'){
                            ?>
                            <td class='alert alert-info'><strong><?=$lang->getLang("District")?></strong></td>
                            <td class='alert alert-success'><strong><?=$lang->getLang("Club")?></strong></td>
                            <td class='alert alert-success'><strong><?=$lang->getLang("Totem Code")?></strong></td>
                        <?
                        }
                        elseif ($_SESSION['jurisdiction_class']=='district'){
                            ?>
                            <td class='alert alert-success'><strong><?=$lang->getLang("Club")?></strong></td>
                            <td class='alert alert-success'><strong><?=$lang->getLang("Totem Code")?></strong></td>
                        <?
                        }
                        elseif ($_SESSION['jurisdiction_class']=='region'){
                            ?>
                            <td class='alert alert-success'><strong><?=$lang->getLang("Totem Code")?></strong></td>
                        <?
                        }
                        ?>

                        <td class='alert alert-info'><strong><?=$lang->getLang("Deposit")?></strong></td>
                        <td class='alert alert-info'><strong><?=$lang->getLang("Withdraw")?></strong></td>
                        <td class='alert alert-info'><strong><?=$lang->getLang("Total rake")?></strong></td>
                    </tr>
                    <?
                    while($partialClosure->hasNext())
                    {
                        $row=$partialClosure->next();
                        ?>
                        <tr>
                            <? if($_SESSION['jurisdiction_class']=='casino'){
                                ?>
                                <td><strong><?=$row['nation_name']?></strong></td>
                                <td><strong><?=$row['region_name']?></strong></td>
                                <td><strong><?=$row['district_name']?></strong></td>
                                <td><strong><?=$row['club_name']?></strong></td>
                                <td><strong><?=$row['ttm_code']?></strong></td>
                            <?
                            }
                            elseif($_SESSION['jurisdiction_class']=='nation'){
                                ?>
                                <td><strong><?=$row['region_name']?></strong></td>
                                <td><strong><?=$row['district_name']?></strong></td>
                                <td><strong><?=$row['club_name']?></strong></td>
                                <td><strong><?=$row['ttm_code']?></strong></td>
                            <?
                            }
                            elseif ($_SESSION['jurisdiction_class']=='region'){
                                ?>
                                <td><strong><?=$row['district_name']?></strong></td>
                                <td><strong><?=$row['club_name']?></strong></td>
                                <td><strong><?=$row['ttm_code']?></strong></td>
                            <?
                            }
                            elseif ($_SESSION['jurisdiction_class']=='district'){
                                ?>
                                <td><strong><?=$row['club_name']?></strong></td>
                                <td><strong><?=$row['ttm_code']?></strong></td>
                            <?
                            }
                            elseif ($_SESSION['jurisdiction_class']=='club'){
                                ?>
                                <td  class='alert alert-success'><strong><?=$row['ttm_code']?></strong></td>
                            <?
                            }
                            ?>
                            <td><?=getInDollars($row['deposit'])?></td>
                            <td><?=getInDollars($row['withdraw'])?></td>
                            <td><p class='text-right'><?=getInDollars($row['tot_rake'])?><br>
                                </p>

                            </td>
                        </tr>
                        <?
                        $bet=$row['deposit'];
                        $win=$row['withdraw'];
                        $note=$_POST['note'];
                        if($_SESSION['jurisdiction_class']=='casino'){
                            $params=",'1','".$row['nation']."'";
                        }
                        elseif($_SESSION['jurisdiction_class']=='nation')
                        {
                            $params=",1,".$row['nation'].",".$row['region'];
                        }
                        elseif($_SESSION['jurisdiction_class']=='region')
                        {
                            $params=",1,".$row['nation'].",".$row['region'].",".$row['district'];
                        }
                        elseif($_SESSION['jurisdiction_class']=='district')
                        {
                            $params=",1,".$row['nation'].",".$row['region'].",".$row['district'].",".$row['club'];
                        }
                    }

                    ?>

                </table>
                <button class="btn btn-primary" onclick="confirmClosure('<?=$date_start?>','<?=$date_end?>','<?=$bet?>','<?=$win?>','<?=$rake?>','<?=$fin?>','<?=$perc?>','<?=$note?>','<?=$_SESSION['admin_id']?>'<?=$params?>)"><?=$lang->getLang("Confirm")?></button><br>
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
</td></tr></table>
<?
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

function getjurisdictionChild ($id,$default=null){
    global $dbh;
    $sql= "SELECT jur_id,jur_name from jurisdiction where jur_parent_id='$id'";
    $result=$dbh->exec($sql);
    $str  = "<select name='jurisdiction' id='jurisdiction'  onchange=getPercent($id);getJurDate();>";
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


function calculatePartialClosure($date_start,$date_end){
    global $dbh;
    //  $sql="SELECT jps_perc_totem FROM jurisdiction_payment_settings WHERE jps_jur_id=$jurid";
    /*SELECT ttm_code, ttm_id, c.jur_name club_name, c.jur_id club_id, d.jur_name district_name, d.jur_id district_id, r.jur_name region_name, r.jur_id region_id, n.jur_name nation_name, n.jur_id nation_id,
        COALESCE( jps1.jps_perc_totem, 0 ) AS perc_casino,
        COALESCE( jps2.jps_perc_totem, 0 ) AS perc_nation,
        COALESCE( jps3.jps_perc_totem, 0 ) AS perc_region,
        COALESCE( jps4.jps_perc_totem, 0 ) AS perc_district,
        COALESCE( jps5.jps_perc_totem, 0 ) AS perc_club,
        SUM( IF( ttt_amount > 0 and ttt_time BETWEEN '2011-08-07 00:00:00' and '2013-05-01 23:59:59', ttt_amount, 0 ) ) AS deposit,
        SUM( IF( ttt_amount <0 and ttt_time BETWEEN '2011-08-07 00:00:00' and '2013-05-01 23:59:59', ttt_amount, 0 ) ) AS withdraw
        FROM totem, totem_transtrack,punter,jurisdiction c, jurisdiction d, jurisdiction r, jurisdiction n, jurisdiction_payment_settings jps1, jurisdiction_payment_settings jps2, jurisdiction_payment_settings jps3, jurisdiction_payment_settings jps4, jurisdiction_payment_settings jps5
        WHERE n.jur_parent_id = 1
        and r.jur_parent_id = n.jur_id
          and d.jur_parent_id = r.jur_id
                and c.jur_parent_id = d.jur_id
                    and jps1.jps_jur_id = 1
                        and jps2.jps_jur_id = n.jur_id
                            and jps3.jps_jur_id = r.jur_id
                                and jps4.jps_jur_id = d.jur_id
                                    and jps5.jps_jur_id = c.jur_id
                                        and d.jur_id = 74
                                            and c.jur_id = pun_betting_club
                                                and pun_id = ttt_pun_id
                                                    and ttt_ttm_id = ttm_id
                                                        and ttt_time BETWEEN '2011-08-07 00:00:00' and '2013-05-01 23:59:59'
       GROUP BY c.jur_id, ttm_id */
    $sql=" SELECT ttm_code,
              ttm_id,
              c.jur_name club_name,
              c.jur_id club_id,
              d.jur_name district_name,
              d.jur_id district_id,
              r.jur_name region_name,
              r.jur_id region_id,
              n.jur_name nation_name,
              n.jur_id nation_id,
        COALESCE( jps1.jps_perc_totem, 0 ) AS perc_casino,
        COALESCE( jps2.jps_perc_totem, 0 ) AS perc_nation,
        COALESCE( jps3.jps_perc_totem, 0 ) AS perc_region,
        COALESCE( jps4.jps_perc_totem, 0 ) AS perc_district,
        COALESCE( jps5.jps_perc_totem, 0 ) AS perc_club,
        SUM( IF( ttt_amount > 0 and ttt_time BETWEEN '$date_start 00:00:00' and '$date_end 23:59:59', ttt_amount, 0 ) ) AS deposit,
        SUM( IF( ttt_amount <0 and ttt_time BETWEEN '$date_start 00:00:00' and '$date_end 23:59:59', ttt_amount, 0 ) ) AS withdraw
        FROM totem,
             totem_transtrack,
             punter,
             jurisdiction c,
             jurisdiction d,
             jurisdiction r,
             jurisdiction n,
             jurisdiction_payment_settings jps1,
             jurisdiction_payment_settings jps2,
             jurisdiction_payment_settings jps3,
             jurisdiction_payment_settings jps4,
             jurisdiction_payment_settings jps5
        WHERE n.jur_parent_id = 1
        and r.jur_parent_id = n.jur_id
        and d.jur_parent_id = r.jur_id
        and c.jur_parent_id = d.jur_id
        and jps1.jps_jur_id = 1
        and jps2.jps_jur_id = n.jur_id
        and jps3.jps_jur_id = r.jur_id
        and jps4.jps_jur_id = d.jur_id
        and jps5.jps_jur_id = c.jur_id
        and c.jur_id = pun_betting_club
        and pun_id = ttt_pun_id
        and ttt_ttm_id = ttm_id
        and ttt_time BETWEEN '$date_start 00:00:00' and '$date_end 23:59:59'";
    if($_SESSION['jurisdiction_class']=='nation'){
        $sql .=' AND  n.jur_id='.$_SESSION['jurisdiction_id'];
    }
    if($_SESSION['jurisdiction_class']=='region'){
        $sql .=' AND r.jur_id='.$_SESSION['jurisdiction_id'];
    }
    if($_SESSION['jurisdiction_class']=='district'){
        $sql .=' AND d.jur_id='.$_SESSION['jurisdiction_id'];
    }
    if($_SESSION['jurisdiction_class']=='club'){
        $sql .=' AND c.jur_id='.$_SESSION['jurisdiction_id'];
    }
    $sql .="  GROUP BY c.jur_id, ttm_id";
    $result = $dbh->doCachedQuery($sql, 0);

    if($result->getNumRows()>0){
        return $result;
    }
    else{
        return '0';
    }
}
?>
