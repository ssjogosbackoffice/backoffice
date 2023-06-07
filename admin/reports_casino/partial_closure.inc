<?php

require_once 'partial_closure_service.inc';
global $lang;
check_access('financial_closure_do',$redirect=true);
$date_end= (isset($_POST['date_start'])) ? $_POST['date_end'] : date("Y-m-d", time()-3600*24);

$active=(isset($_POST["operation"])) ? $_POST["operation"] : 'casino' ;
$periodType=(isset($_POST["periodType"])) ? $_POST["periodType"] : '1';
$defaultmonth= (isset($_POST['monthHelper'])) ? $_POST['monthHelper'] : false;
$defaultmonth2= (isset($_POST['monthHelper2'])) ? $_POST['monthHelper2'] : false;
$defaultmonth4= (isset($_POST['monthHelper4'])) ? $_POST['monthHelper4'] : false;
$defaultweek= (isset($_POST['weekHelper'])) ? $_POST['weekHelper'] : false;
?>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap-responsive.css" title="core_style" />

<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<script type="text/javascript">
var dataTypes=[];
$(function() {
    getJurDate();
    getJurDateBetting();

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


    $(document).on('change', '#jurisdiction', function() {
        getPercent($(this).val());
        getJurDate();
    });
    $(document).on('change', '#jurisdiction_betting', function() {
        getPercentBetting($(this).val());
        getJurDateBetting();
    });


    $("#casino_form").submit( function(event, skip) {
        if (skip) {
            return true;
        }
        event.preventDefault();
        var self = this;
        if($('#date_start').children().length > 0){
            jAlert("<?=$lang->getLang("There is no date available")?>");
        }
        else if($('#percentagelabel').hasClass("label-important") ){
            jAlert("<?=$lang->getLang("Before please configure the percentuals!")?>");
        }
        else if($('#date_end').val()== '<?=$lang->getLang("No data available")?>' ){
            jAlert("<?=$lang->getLang("No end date available!")?>");
        }
        else{
            $(self).trigger('submit', true);
        }
    });
    $("#betting_form").submit( function(event, skip) {
        if (skip) {
            return true;
        }
        event.preventDefault();
        var self = this;
        if($('#ds_betting').val()=='' || $('#ds_betting').val()=='undefined' ){
            jAlert("<?=$lang->getLang("There is no date available")?>");
        }
        else if($('#percentagelabel_betting').hasClass("label-important") ){
            jAlert("<?=$lang->getLang("Before please configure the percentuals!")?>");
        }
        else if($('#date_end_betting').html()=='<?=$lang->getLang("No data available")?>' || $('#date_end_betting').html()=='' ){
            jAlert("<?=$lang->getLang("No end date available!")?>");
        }
        else{
            $(self).trigger('submit', true);
        }
    });
    $(document).on('change', ".monthHelper", function() {
        createMonthHelper($(this).val());
        $('.monthHelper').not(this).val('');
    });
    $(document).on('change', "[name='weekHelper']", function() {
        createWeekHelper($(this).val());
    });
    $(document).on('change', "#periodType", function() {
        var currVal=$(this).val();
        $("#de_betting" ).val(dataTypes[currVal]);
        $("#date_end_betting").html(dataTypes[currVal])
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

function confirmClosure(date_start,date_end,bet,win,rake,cash,parent,child,fin,perc,note,ca,n,re,d,c){
    jConfirm('<?=$lang->getLang('Are you sure?')?>',
        '<?=$lang->getLang('Please confirm')?>', function(r) {
            if (r){
                $.post("reports_casino/partial_closure_service.inc",{action:'3',date_start:date_start,date_end:date_end,bet:bet,win:win,rake:rake,cash:cash,parent:parent,child:child,fin:fin,perc:perc,note:note,ca:ca,n:n,re:re,d:d,c:c}, function(data){
                    if(data=='1'){
                        $('#confirmClosure').empty().append( "<h3 class='alert alert-success'><strong><?=$lang->getLang('Partial Closure successfully made!')?></strong></h3>");
                       getJurDate();
                        $('#results,#confirmClosure').show();
                    }
                    else
                    {
                        $('#confirmClosure').empty().append("<h3 class='alert alert-error'><?=$lang->getLang('Partial closure already made!')?></h3>");
                    }
                });
            }
        });
}

function confirmBettingClosure(date_start,date_end,bet,win,rake,cash,parent,child,fin,perc,note,ca,n,re,d,c){
    jConfirm('<?=$lang->getLang('Are you sure?')?>',
        '<?=$lang->getLang('Please confirm')?>', function(r) {
            if (r){
                $.post("reports_casino/partial_closure_service.inc",{action:'7',date_start:date_start,date_end:date_end,bet:bet,win:win,rake:rake,cash:cash,parent:parent,child:child,fin:fin,perc:perc,note:note,ca:ca,n:n,re:re,d:d,c:c}, function(data){
                    if(data=='1'){
                        $('#confirmClosureBetting').empty().append( "<h3 class='alert alert-success'><strong><?=$lang->getLang('Partial Closure successfully made!')?></strong></h3>").show();
                        getJurDateBetting();
                        $('#results_betting,#confirmClosureBetting').show();
                    }
                    else  {
                        $('#confirmClosureBetting').empty().append("<h3 class='alert alert-error'><?=$lang->getLang('Partial closure already made!')?></h3>").show();

                    }
                });
            }
        });
}

function getPercent(parent){
    $.post("reports_casino/partial_closure_service.inc",{action:'1',parent:parent,child:$('select#jurisdiction option:selected').val() }, function(data){
        $('#percentagelabel').empty().append($('select#jurisdiction option:selected').text()+' percentage');
        if(data==''){
            $('#percentage').empty().append("<div class='errorbold'><?=$lang->getLang('Please set the percentuals!')?></div>");
            $("#percentagelabel").attr('class', 'label label-important');
        }
        else if (data[0]=='C') {

            percents=data.split('###');
            $('#percentage').empty().append( percents[0] );
            $('#perc').val(percents[1]);
            $("#percentagelabel").attr('class', 'label label-inverse');
        }
        else{
            $('#percentage').empty().append("<div class='errorbold'><?=$lang->getLang("Please configure the following percentuals")?> :<br></div><div class='error'>"+data+"</div>");
            $("#percentagelabel").attr('class', 'label label-important');
        }
    });
}

function getPercentBetting(parent){
    $.post("reports_casino/partial_closure_service.inc",{action:'6',parent:parent,child:$('select#jurisdiction_betting option:selected').val() }, function(data){
        if(data==''){
            $('#percentage_betting').empty().append("<div class='errorbold'><?=$lang->getLang('Please set the percentuals!')?></div>");
            $("#percentagelabel_betting").attr('class', 'label label-important');
        }
        else if (data.search('#')>0) {
            percs=data.split('#');
            $('#percentage_betting').empty().append("<b> <?=$lang->getLang("BETTING")?>: </b> "+percs[0]  + " <br />"+"<b><?=$lang->getLang("Live betting")?>: </b> "+percs[1] );
            $('#perc_betting').val(data);
            $("#percentagelabel_betting").attr('class', 'label label-inverse');
        }
        else{
            $('#percentage_betting').empty().append("<div class='errorbold'><?=$lang->getLang("Please configure the following percentuals")?> :<br></div><div class='error'>"+data+"</div>");
            $("#percentagelabel_betting").attr('class', 'label label-important');
        }
    });
}

function getJurDate(){
    $('#results').hide();
    $('#confirmClosure').hide();
    $.post("reports_casino/partial_closure_service.inc",{action:'2',thisclass:'<?=getSubJurisdiction($_SESSION['jurisdiction_class'])?>',id:$('select#jurisdiction option:selected').val()}, function(data){
        $('#date_start').empty().append(data);
        var currentTime = new Date()
        var month = ('0' + (currentTime.getMonth()+1)).slice(-2);
        var day = ('0' + currentTime.getDate()).slice(-2) ;
        var year = currentTime.getFullYear();
        today=(year+ "-" + month + "-" + day);
        if(today==data){
            $('#date_end').val('<?=$lang->getLang("No data available")?>');
        }
        else
        {
            $('#date_end').val('<?=$date_end?>');
        }

        $('#ds').val(data);
        if(data=="<span class='errorbold'><?=$lang->getLang("No data available")?></span>")
        {
            $("#datestartlabel").attr('class', 'label label-important');
        }
        else{
            $("#datestartlabel").attr('class', 'label label-inverse');
        }
    });
}

function getJurDateBetting(){
    $('#results_betting').hide();
    $('#confirmClosureBetting').hide();

    $.post("reports_casino/partial_closure_service.inc",{action:'2',thisclass:'<?=getSubJurisdiction($_SESSION['jurisdiction_class'])?>',id:$('select#jurisdiction_betting option:selected').val(),betting:1}, function(data){
        $('#ds_betting,#de_betting').val('');
        $('#date_start_betting,#date_end_betting').empty();
        if(data=="<span class='errorbold'><?=$lang->getLang("No data available")?></span>")
        {
            $("#datestartlabel_betting").attr('class', 'label label-important');
            $("#periodContainer").hide();
        }
        else{
            dates=data.split(';');
            dataTypes=dates;
            if(dataTypes[1]=="0")
            {
                $("#datestartlabel_betting").attr('class', 'label label-important');
                $("#periodContainer").hide();
            }else{
                $( "#periodType option" ).each(function( index ) {
                    return ( dataTypes[index+1]==0?  $(this).prop('disabled',true)  : $(this).prop('disabled',false));
                });
                $("#ds_betting" ).val(dataTypes[0])
                $("#date_start_betting").html(dataTypes[0]);

                $("#de_betting" ).val(dataTypes[$("#periodType").val()]);
                $("#date_end_betting").html(dataTypes[$("#periodType").val()]);

                $("#datestartlabel_betting").attr('class', 'label label-inverse');
                $("#periodContainer").show();
                if(dataTypes[$('#periodType').val()]=='0'|| typeof dataTypes[$('#periodType').val()]=='undefined'){
                    $('#periodType').val(1).trigger('change');
                }
            }
        }
    });
}

function getClosure(parent,parentclass){
    if ($('#searchBox').is(':checked')) {
        datetosend=$('#searchdatefrom').val();
    } else {
        datetosend='';
    }
    $.post("reports_casino/partial_closure_service.inc",{action:'4',parent:parent,parentclass:parentclass,date_start:datetosend }, function(data){
        $('#closureResults').empty().append( data);

    });
}
function toggleClass(classtotoggle){
    if ($('.'+classtotoggle).is(":hidden")) {
        $('#showMyProfit').html('<?=$lang->getLang('Hide my profit')?>');
        $('.'+classtotoggle).show('slow');
    } else {
        $('.'+classtotoggle).hide('slow');
        $('#showMyProfit').html('<?=$lang->getLang('Show my profit')?>');
    }
    return false;
}

</script>

<div class="container" style="position: absolute;left:0;right: 0;width: 100%" >
<div class="container-fluid">
<table class="table table-bordered">
<tr><td class='navbar-inner'><h3 class="text-center"><?=$lang->getLang("Partial Closure")?></h3></td></tr>
<tr>
<td>
<div class="tabbable tabs-left">
<ul class="nav nav-tabs">

    <li <?=($active=='casino') ? "class='active'":""?>><a href="#casino" data-toggle="tab"><?=$lang->getLang("Casino & Poker")?></a></li>
    <li <?=($active=='betting') ? "class='active'":""?>><a href="#betting" data-toggle="tab"><?=$lang->getLang("Betting")?></a></li>
</ul>
<div class="tab-content">
<div class="tab-pane fade <?=($active=='casino') ? "in active":""?>" id="casino">


<?php
/**
 * @author Marian
 */
if($_SESSION['jurisdiction_class']=='club'){
    ?>

    <div class='well'>
        <h2 class="text-error"><?=$lang->getLang("This function is not available for you!")?></h2>
    </div>
<?
}
else{
?>

        <div class='well'>
        <form action="/partial_closure" method="post" id="casino_form">
            <?php printHiddenInput('doQuery', 'yes');?>
            <?php printHiddenInput('search_type', 'date_range');?>
            <?php printHiddenInput('operation', 'casino');?>
            <div class='row'>
                <div class='span3'><div class='label label-inverse'> <?= $lang->getLang(ucfirst(getSubJurisdiction($_SESSION['jurisdiction_class'])));?></div>
                    <br><div>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-user"></i></span>
                                <?php echo getjurisdictionChild( $_SESSION['jurisdiction_id'],'jurisdiction',$_POST['jurisdiction']);
                                ?>
                            </div>
                        </div>
                    </div>
                </div>

                <div class="span3"><div class='label label-inverse' id='datestartlabel'><?=$lang->getLang("From")?></div> <br>
                        <span class="add-on"><i class="icon-calendar"></i>
                        <span id='date_start'></span></span>
                    <input type='hidden' name='date_start' id='ds'>
                </div>
                <div class='span3'><div id='percentagelabel' class='label label-inverse'><?=$lang->getLang("Percentage")?></div><div id='percentage'><script>getPercent(<?=$_SESSION['jurisdiction_id']?>)</script></div>
                    <input type='hidden' name='perc' id='perc'>
                </div>

            </div>
            <div class='row'>&nbsp;</div>
            <div class='row'>
                <div class='span3'><div class='label label-inverse'><?=$lang->getLang("User")?></div> <br><div>
                        <span class="add-on"><i class="icon-user"></i> <?=$_SESSION['admin_username']?></span>
                    </div>
                </div>

                <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Until")?></div>
                    <br>

                    <div class="controls">
                        <div class="input-prepend">
                            <span class="add-on"><i class="icon-calendar"></i></span>

                            <input type='text'  name='date_end' id='date_end' autocomplete="off" value="<?=$date_end?>">
                        </div>
                    </div>

                </div>

                <div class='span5'><div class='label label-inverse'><?=$lang->getLang("Note")?></div> <br>
                    <div><textarea  style="width: 100%;height: 130px" name='note' ><?=$_POST['note']?></textarea> </div>
                </div>
            </div>
            <button class="btn btn-primary" ><?=$lang->getLang("Calculate")?></button>
    </form>
<br>
<br>
    <div id='confirmClosure'></div>


<?php
if(isset($_POST['doQuery']) && $active=='casino'){
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

            $percentuals=calculatePercent($parent,$child);
            $child_details=get_jurisdiction($child);
            $parent_details=get_jurisdiction($_SESSION['jurisdiction_id']);
            $partialClosure=calculatePartialClosure($parent,$child,$date_start,$date_end);
            if($partialClosure==0){
                addError("",$lang->getLang("No result found"));
                showErrors();
            }
            else
            {
                ?>
                <table class='table table-bordered' >
                <tr>
                    <td colspan="19" class="badge-inverse"><div class='label label-inverse'><?=$lang->getLang("Results for % from % until %",$child_details['jur_name'],$date_start,$date_end)?></div></td>
                </tr>
                <tr><td class='alert alert-info'><strong>Casino NET</strong></td>
                    <td class='alert alert-info'><strong>Casino FIN</strong></td>
                    <td class='alert alert-success'><strong>Casino Live NET</strong></td>
                    <td class='alert alert-success'><strong>Casino Live FIN</strong></td>
                    <td class='alert alert-info'><strong>Virtual NET</strong></td>
                    <td class='alert alert-info'><strong>Virtual FIN</strong></td>
<!--                    <td class='alert alert-info'><strong>RoyalPk NET</strong></td>-->
<!--                    <td class='alert alert-info'><strong>RoyalPk FIN</strong></td>-->
<!--                    <td class='alert alert-info'><strong>CasinoExt NET</strong></td>-->
<!--                    <td class='alert alert-info'><strong>CasinoExt FIN</strong></td>-->
                    <td class='alert alert-success'><strong>Poker Live RAKE</strong></td>
                    <td class='alert alert-success'><strong>Poker Live FIN</strong></td>
                    <td class='alert alert-info'><strong>Poker 2d RAKE</strong></td>
                    <td class='alert alert-info'><strong>Poker 2d FIN</strong></td>
                    <td class='alert alert-success'><strong><?=$lang->getLang("Total Fin")?></strong></td>
                    <td class='alert alert-success'><strong><?=$lang->getLang("Total Rake")?></strong></td>
                    <td class='alert alert-success'><strong><?=$lang->getLang("Total Net")?></strong></td>
                    <td class='alert alert-danger'><strong><?=$parent_details['jur_name']?></strong></td>
                    <td class='alert alert-danger'><strong><?=$child_details['jur_name']?></strong></td>
                </tr>
                <?
                $perc=$_POST['perc'];
                $note=$_POST['note'];
                $totalPerc=array();
                $casino=$casino_live=$virtual=$pokerl=$poker2d=$royalpk=$casinoext=1;
                while($partialClosure->hasNext())
                {
                    $row=$partialClosure->next();
                        $totalsPerc['casino_rake']+=(($row["casino_rake"]/100)* ($row["jps_perc_casino"]));
                        $totalsPerc['casino_live_rake']+=$row["casino_live_rake"]/100* ($row["jps_perc_casino_live"]);
                        $totalsPerc['virtual_rake']+=$row["virtual_rake"]/100* ($row["jps_perc_virtual"]);
//                        $totalsPerc['royalpk_rake']+=$row["royalpk_rake"]/100* ($row["jps_perc_royalpk"]);
//                        $totalsPerc['casinoext_rake']+=$row["casinoext_rake"]/100* ($row["jps_perc_casinoext"]);
                        $totalsPerc['poker_live_rake']+=$row["poker_live_rake"]/100* ($row["jps_perc_poker_live"]);
                        $totalsPerc['poker_2d_rake']+=$row["poker_2d_rake"]/100* ($row["jps_perc_poker_2d"]);
                        $nation=$row['nation_id'];
                        $region=$row['region_id'];
                        $district=$row['district_id'];
                        $club=$row['club_id'];
                    foreach($row as $key => $val){
                        if(is_numeric($val)){
                            $totals[$key] += $val;
                        }
                    }

                }

                $totalsRakeSum=0;
                $totalsBetSum=0;
                $totalsWinSum=0;
                $totalsFinSum=0;
                if($casino==1){
                    $totalsRakeSum +=$totals['casino_rake'];
                    $totalsBetSum +=$totals['casino_bet'];
                    $totalsWinSum +=$totals['casino_win'];
                    $totalsFinSum +=$totals['casino_fin'];
                }
                if($casino_live==1){
                    $totalsRakeSum +=$totals['casino_live_rake'];
                    $totalsBetSum  +=$totals['casino_live_bet'];
                    $totalsWinSum  +=$totals['casino_live_win'];
                    $totalsFinSum  +=$totals['casino_live_fin'];

                }
                if($virtual==1){
                    $totalsRakeSum +=$totals['virtual_rake'];
                    $totalsBetSum  +=$totals['virtual_bet'];
                    $totalsWinSum  +=$totals['virtual_win'];
                    $totalsFinSum  +=$totals['virtual_fin'];
                }
//                if($royalpk==1){
//                    $totalsRakeSum +=$totals['royalpk_rake'];
//                    $totalsBetSum  +=$totals['royalpk_bet'];
//                    $totalsWinSum  +=$totals['royalpk_win'];
//                    $totalsFinSum  +=$totals['royalpk_fin'];
//                }
//                if($casinoext==1){
//                    $totalsRakeSum +=$totals['casinoext_rake'];
//                    $totalsBetSum  +=$totals['casinoext_bet'];
//                    $totalsWinSum  +=$totals['casinoext_win'];
//                    $totalsFinSum  +=$totals['casinoext_fin'];
//                }
                if($pokerl==1){
                    $totalsRakeSum +=$totals['poker_live_rake'];
                    $totalsBetSum  +=$totals['poker_live_bet'];
                    $totalsWinSum  +=$totals['poker_live_win'];
                    $totalsFinSum  +=$totals['poker_live_fin'];
                }
                if($poker2d==1){
                    $totalsRakeSum +=$totals['poker_2d_rake'];
                    $totalsBetSum +=$totals['poker_2d_bet'];
                    $totalsWinSum +=$totals['poker_2d_win'];
                    $totalsFinSum +=$totals['poker_2d_fin'];;
                }
                $cash=$_SESSION['jurisdiction_credit'];
                    ?>
                    <tr>
                        <td class="text-success unwrappable"><p class='text-right'><?=getInDollars($totals['casino_rake'],2,$row['cur_code_for_web'])?><br>
                            <p class='closureall'>
                                <?=(100-$_SESSION['casino2'])." % => ".preg_replace("/\&nbsp; /", "",getInDollars($totals['casino_rake']-$totalsPerc["casino_rake"]))?>
                            </p>

                        </td>
                        <td class="text-success unwrappable"><p class='text-right'><?=getInDollars($totals['casino_fin'])?></p></td>

                        <td class="text-info unwrappable"><p class='text-right'><?=getInDollars($totals['casino_live_rake'])?><br>
                            <p class='closureall'>
                                <?=(100-$_SESSION['casino_live2'])." % => ".getInDollars($totals['casino_live_rake']-$totalsPerc["casino_live_rake"])?>
                            </p>
                        </td>
                        <td class="text-info unwrappable"><p class='text-right'><?=getInDollars($totals['casino_live_fin'])?></p></td>

                        <td class="text-info unwrappable">
                            <p class='text-right'><?=getInDollars($totals['virtual_rake'])?><br>
                            <p class='closureall'>
                                <?=(100-$_SESSION['virtual2'])." % => ".getInDollars($totals['virtual_rake']-$totalsPerc['virtual_rake'])?>
                            </p>
                        </td>
                        <td class="text-info unwrappable"><p class='text-right'><?=getInDollars($totals['virtual_fin'])?></p></td>

<!--                        <td class="text-info unwrappable">-->
<!--                            <p class='text-right'>--><?//=getInDollars($totals['royalpk_rake'])?><!--<br>-->
<!--                            <p class='closureall'>-->
<!--                                --><?//=(100-$_SESSION['royalpk2'])." % => ".getInDollars($totals['royalpk_rake']-$totalsPerc['royalpk_rake'])?>
<!--                            </p>-->
<!--                        </td>-->
<!--                        <td class="text-info unwrappable"><p class='text-right'>--><?//=getInDollars($totals['royalpk_fin'])?><!--</p></td>-->
<!---->
<!--                        <td class="text-info unwrappable">-->
<!--                            <p class='text-right'>--><?//=getInDollars($totals['casinoext_rake'])?><!--<br>-->
<!--                            <p class='closureall'>-->
<!--                                --><?//=(100-$_SESSION['casinoext2'])." % => ".getInDollars($totals['casinoext_rake']-$totalsPerc['casinoext_rake'])?>
<!--                            </p>-->
<!--                        </td>-->
<!--                        <td class="text-info unwrappable"><p class='text-right'>--><?//=getInDollars($totals['casinoext_fin'])?><!--</p></td>-->


                        <td class="text-success unwrappable"><p class='text-right'><?=getInDollars($totals['poker_live_rake'])?><br>
                            <p class='closureall'>
                                <?=(100-$_SESSION['poker2'])." % => ".getInDollars($totals['poker_live_rake']-$totalsPerc['poker_live_rake'])?>
                            </p>
                        </td>
                        <td class="text-success unwrappable"><p class='text-right'><?=getInDollars($totals['poker_live_fin'])?></p></td>

                        <td class="text-success unwrappable"><p class='text-right'><?=getInDollars($totals['poker_2d_rake'])?><br>
                            <p class='closureall'>
                                <?=(100-$_SESSION['poker_2d_2'])." % => ".getInDollars($totals['poker_2d_rake']-$totalsPerc['poker_2d_net'])?>
                            </p>
                        </td>
                        <td class="text-success unwrappable"><p class='text-right'><?=getInDollars($totals['poker_2d_fin'])?></p></td>

                        <td class="unwrappable"><?=getInDollars($totalsFinSum)?></td>
                        <td class="unwrappable"><strong><?=getInDollars($totalsRakeSum)?><br>
                                Total Perc = <?=getInDollars(array_sum($totalsPerc))?>
                        </td>
                        <td class="unwrappable"><strong><?=getInDollars($totalsFinSum-(array_sum($totalsPerc)) )?></td>
                        <td class="unwrappable"><?=getInDollars($totalsFinSum-(array_sum($totalsPerc)))?></td>
                        <td class="unwrappable"><?=getInDollars(array_sum($totalsPerc))?></td>

                    </tr>
                    <?
                    if($_SESSION['jurisdiction_class']=='casino'){
                        $params=",'1','".$nation."'";

                    }
                    elseif($_SESSION['jurisdiction_class']=='nation')
                    {
                        $params=",1,".$nation.",".$region;
                    }
                    elseif($_SESSION['jurisdiction_class']=='region')
                    {
                        $params=",1,".$nation.",".$region.",".$district;
                    }
                    elseif($_SESSION['jurisdiction_class']=='district')
                    {
                        $params=",1,".$nation.",".$region.",".$district.",".$club;
                    }

                    $perc=urlencode($perc);
                    ?>
                    </table>
                    <button class="btn btn-danger" id="showMyProfit" onclick="toggleClass('closureall')"><?=$lang->getLang("Show my profit")?></button>
                <button class="btn btn-primary" onclick="confirmClosure('<?=$date_start?>',
                        '<?=$date_end?>',
                        '<?=$totalsBetSum?>',
                        '<?=$totalsWinSum?>',
                        '<?=$totalsRakeSum?>',
                        '<?=$cash?>',
                        '<?=$totalsFinSum-(array_sum($totalsPerc))?>',
                        '<?=array_sum($totalsPerc)?>',
                        '<?=$totalsFinSum?>',
                        '<?=$perc?> ',
                        '<?=$note?>'<?=$params?>)"><?=$lang->getLang("Confirm")?></button><br>
                <?
            }
        }
        else{
            showErrors();
        }
        ?>
    </div>
    <script>$(function(){
            $('#results').show();
        });</script>
<?
}
}
?>
</div>
</div>

<div class="tab-pane fade <?=($active=='betting') ? "in active":""?>" id="betting">

<?php
/**
 * @author Marian
 */
if($_SESSION['jurisdiction_class']=='club'){
    ?>

    <div class='well'>
        <h2 class="text-error"><?=$lang->getLang("This function is not available for you!")?></h2>
    </div>
<?
}
else{ ?>
<div class='well'>


<table>
    <tr>
        <td  style="width: 55%;border: 0px">
            <form action="/partial_closure" method="post" id="betting_form">
                <?php printHiddenInput('doQuery', 'yes');?>
                <?php printHiddenInput('operation', 'betting');?>
                <div class='row'>
                    <div class='span3'><div class='label label-inverse'> <?=ucfirst(getSubJurisdiction($_SESSION['jurisdiction_class']));?></div>
                        <br><div>
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-user"></i></span>
                                    <?php echo getjurisdictionChild($_SESSION['jurisdiction_id'],'jurisdiction_betting',$_POST['jurisdiction_betting']);
                                    ?>
                                </div>
                            </div>
                        </div>
                    </div>
                    <div class='span3'><div class='label label-inverse'><?=$lang->getLang("User")?></div> <br><div>
                            <span class="add-on"><i class="icon-user"></i> <?=$_SESSION['admin_username']?></span>
                        </div>
                    </div>
                </div>
                <div class='row'>&nbsp;</div>
                <div class='row' id="periodContainer">
                    <div class="span3"><div class='label label-inverse' ><?=$lang->getLang("Type")?></div> <br>
                                     <span class="add-on"><i class="icon-calendar"></i>
                                        <span>
                                          <select name="periodType" id="periodType">
                                              <option value="1" <?=($periodType==1?'selected': '')?>>Weekly</option>
                                              <option value="2" <?=($periodType==2?'selected': '')?>>Monthly</option>
                                              <option value="3" <?=($periodType==3?'selected': '')?>>2 Months</option>
                                              <option value="4" <?=($periodType==4?'selected': '')?>>4 Months</option>
                                          </select>
                                        </span>
                                     </span>
                    </div>
                </div>
                <div class='row'>&nbsp;</div>
                <div class='row'>
                    <div class="span3"><div class='label label-inverse' id='datestartlabel_betting'><?=$lang->getLang("From")?></div> <br>
                                     <span class="add-on"><i class="icon-calendar"></i>
                                        <span id='date_start_betting' class="dateStartPicker">

                                        </span>
                                     </span>
                        <input type='hidden' name='date_start_betting' id='ds_betting'>
                    </div>

                    <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Until")?></div> <br>
                                     <span class="add-on"><i class="icon-calendar"></i>
                                        <span id='date_end_betting' class="dateEndPicker">
                                        </span>
                                     </span>
                        <input type='hidden'  name='date_end_betting' id='de_betting' class="dateEndPicker" value="<?=$date_end_betting?>">
                    </div>



                </div>
                <div class='row'>&nbsp;</div>
                <div class="row">
                    <div class='span3'><div id='percentagelabel_betting' class='label label-inverse'>
                            <?=$lang->getLang("Percentage")?></div><div id='percentage_betting'>
                            <script>getPercentBetting(<?=$_SESSION['jurisdiction_id']?>)</script></div>
                        <input type='hidden' name='perc_betting' id='perc_betting'>
                    </div>
                    <div class='span5'><div class='label label-inverse'><?=$lang->getLang("Note")?></div> <br>
                        <div><textarea style="width: 100%;height: 130px" name='note_betting' ><?=$_POST['note_betting']?></textarea> </div>
                    </div>
                </div>
                <br /><br />

                <button class="btn btn-primary" ><?=$lang->getLang("Calculate")?></button>
            </form>
            <br><br>

            <div id='confirmClosureBetting'></div>

        </td>
        <td style="border: 0px">
            <div id="legend">
                <table style="border: 0px;border-collapse: collapse">
                    <tr><td colspan="2"><span class="label label-inverse"><?=$lang->getLang("Legend")?></span> </td></tr>
                    <tr><td>
                            <table style="border: 0px;border-collapse: collapse">
                                <tr><td style="border: 0px"> <span class="label label-info"><?=$lang->getLang("Total Bet")?></span> </td><td><span class="tip"><?=$lang->getLang("Sum of betting and live betting bet")?> </span> </td></tr>
                                <tr><td style="border: 0px"><span class="label label-info"><?=$lang->getLang("Total Win")?></span></td><td><span class="tip"><?=$lang->getLang("Sum of betting and live betting win")?> </span> </td></tr>
                                <tr><td style="border: 0px"><span class="label label-info"><?=$lang->getLang("Utile")?></span></td><td><span class="tip"><?=$lang->getLang("Total Bet - Total Win")?> </span> </td></tr>
                                <tr><td style="border: 0px"><span class="label label-info"><?=$lang->getLang("Club rake")?></span></td><td><span class="tip"><?=$lang->getLang("Total cost of the club for betting and live betting")?></span> </td></tr>
                                <tr><td style="border: 0px"> <span class="label label-info"><?=$lang->getLang("Club tot profit")?></span></td><td><span class="tip"><?=$lang->getLang("Club rake + Club bonus")?> </span> </td></tr>
                                <tr><td style="border: 0px"><span class="label label-info"><?=$lang->getLang("Netto bet")?></span></td><td><span class="tip"><?=$lang->getLang("Utile - Club rake")?> </span> </td></tr>
                            </table>
                        </td>
                        <td>
                            <table style="border: 0px;border-collapse: collapse">
                                <tr><td style="border: 0px"> <span class="label label-success"><?=$lang->getLang("Prematch Bet")?></span> </td><td><span class="tip"><?=$lang->getLang("Sum of betting bet")?> </span> </td></tr>
                                <tr><td style="border: 0px"><span class="label label-success"><?=$lang->getLang("Prematch Win")?></span></td><td><span class="tip"><?=$lang->getLang("Sum of betting win")?> </span> </td></tr>
                                <tr><td style="border: 0px"> <span class="label label-warning"><?=$lang->getLang("Live Bet")?></span> </td><td><span class="tip"><?=$lang->getLang("Sum of live betting bet")?> </span> </td></tr>
                                <tr><td style="border: 0px"><span class="label label-warning"><?=$lang->getLang("Live Win")?></span></td><td><span class="tip"><?=$lang->getLang("Sum of live betting win")?> </span> </td></tr>
                            </table>
                        </td>
                    </tr>
                </table>
            </div>
        </td>
    </tr>
</table>


<?php
if(isset($_POST['doQuery']) && $active=='betting'){
    ?>

    <div id='results_betting'>
        <?
        $parent=$_SESSION['jurisdiction_id'];
        $child=$_POST['jurisdiction_betting'];
        $date_start_betting=$_POST['date_start_betting'];
        $date_end_betting= $_POST['date_end_betting'];
        $d1 = strtotime("$date_start_betting");
        $d2 = strtotime("$date_end_betting");
        if($d2-$d1<0){
            addError("",$lang->getLang("Please choose an end date greater than start date"));
        }
        $now=strtotime(date("Y-m-d 23:59:59"));
        if($now-$d2<'86400'){
            addError("",$lang->getLang("The end date can't be greater than yesterday"));
        }
        if(!areErrors())
        {
            $child_details=get_jurisdiction($child);

            $parent_details=get_jurisdiction($_SESSION['jurisdiction_id']);
            $res=calculateBettingPartialClosure($parent,$child,$date_start_betting,$date_end_betting);
            if($res==0){
                addError("",$lang->getLang("No result found"));
                showErrors();
            }
            else
            {
                $totals = array();
                $totalsPerc = array();
                while($res->hasNext()){
                    $row = $res->next();
                    $totals['casino']+=calculateBettingRake($row,'casino',1);
                    $totals['nation']+=calculateBettingRake($row,'nation',1);
                    $totals['region']+=calculateBettingRake($row,'region',1);
                    $totals['district']+=calculateBettingRake($row,'district',1);
                    $totals['bonus']+=calculateBonus($row);

                    $totals['betting_rake']+=$row['betting_rake'];
                    $totals['betting_bet']+=$row['betting_bet'];
                    $totals['betting_win']+=$row['betting_win'];

                    $totals['betting_rake_live']+=$row['betting_rake_live'];
                    $totals['betting_bet_live']+=$row['betting_bet_live'];
                    $totals['betting_win_live']+=$row['betting_win_live'];

                    $nation=$row['nation_id'];
                    $region=$row['region_id'];
                    $district=$row['district_id'];
                    $club=$row['club_id'];

                    $nationPercent=$row['jps_perc_betting_n'];
                    $nationPercentLive=$row['jps_perc_betting_live_n'];

                    $regionPercent=$row['jps_perc_betting_r'];
                    $regionPercentLive=$row['jps_perc_betting_live_r'];

                    $districtPercent=$row['jps_perc_betting_d'];
                    $districtPercentLive=$row['jps_perc_betting_live_n'];

                    $clubPercent=$row['jps_perc_betting_c'];
                    $clubPercentLive=$row['jps_perc_betting_live_c'];

                }
                $totalsRakeSum=0;
                $totalsBetSum=0;
                $totalsWinSum=0;
                $totalBonus=0;

                $totalsRakeSum +=($totals['betting_rake']);
                $totalsBetSum  +=$totals['betting_bet'];
                $totalsWinSum  +=$totals['betting_win'];
                $totalBonus    +=$totals['bonus'];

                $totalsRakeSum +=($totals['betting_rake_live']);
                $totalsBetSum  +=$totals['betting_bet_live'];
                $totalsWinSum  +=$totals['betting_win_live'];

                $cash=$_SESSION['jurisdiction_credit'];
                $parentRake=$totals[$_SESSION['jurisdiction_class']];
                $childRake=$totals[$child_details['jur_class']];


                $perc_betting=$_POST['perc_betting'];
                $note_betting=$_POST['note_betting'];

                ?>
                <table class='table table-bordered' >
                    <tr><td colspan="19" class="badge-inverse"><div class='label label-inverse'><?=$lang->getLang("Betting")?> <?=$lang->getLang("Results for % from % until %",$child_details['jur_name'],$date_start_betting,$date_end_betting)?> </div></td> </tr>
                    <tr>
                        <td class='alert alert-info'><strong><?=$lang->getLang("Prematch bet")?></strong></td>
                        <td class='alert alert-info'><strong><?=$lang->getLang("Prematch win")?></strong></td>
                        <td class='alert alert-info'><strong><?=$lang->getLang("Live bet")?></strong></td>
                        <td class='alert alert-info'><strong><?=$lang->getLang("Live win")?></strong></td>
                        <td class='alert alert-success'><strong><?=$lang->getLang("Total bet")?></strong></td>
                        <td class='alert alert-success'><strong><?=$lang->getLang("Total Win")?></strong></td>
                        <td class='alert alert-success'><strong><?=$lang->getLang("Utile")?></strong></td>
                        <td class='alert alert-success'><strong><?=$lang->getLang("Club rake")?></strong></td>
                        <td class='alert alert-success'><strong><?=$lang->getLang("Club bonus")?></strong></td>
                        <td class='alert alert-success'><strong><?=$lang->getLang("Club tot profit")?></strong></td>
                        <td class='alert alert-success'><strong><?=$lang->getLang("Netto bet")?></strong></td>
                        <td class='alert alert-danger'><strong><?=$parent_details['jur_name']?></strong></td>
                        <td class='alert alert-danger'><strong><?=$child_details['jur_name']?></strong></td>
                    </tr>
                    <tr>
                        <td style="white-space: nowrap;"><?=getInDollars($totals["betting_bet"],2,$row['cur_code_for_web'])?></td>
                        <td style="white-space: nowrap;"><?=getInDollars($totals["betting_win"],2,$row['cur_code_for_web'])?></td>
                        <td style="white-space: nowrap;"><?=getInDollars($totals["betting_bet_live"],2,$row['cur_code_for_web'])?></td>
                        <td style="white-space: nowrap;"><?=getInDollars($totals["betting_win_live"],2,$row['cur_code_for_web'])?></td>
                        <td style="white-space: nowrap;"><?=getInDollars($totalsBetSum,2,$row['cur_code_for_web'])?></td>
                        <td style="white-space: nowrap;"><?=getInDollars($totalsWinSum,2,$row['cur_code_for_web'])?></td>
                        <td style="white-space: nowrap;"><p class='text-right'><?=getInDollars($totalsBetSum-$totalsWinSum,2,$row['cur_code_for_web'])?></td>
                        <td style="white-space: nowrap;"><p class='text-right'><?=getInDollars($totalsRakeSum,2,$row['cur_code_for_web'])?></td>
                        <td style="white-space: nowrap;"><p class='text-right'><?=getInDollars($totalBonus,2,$row['cur_code_for_web'])?></td>
                        <td style="white-space: nowrap;"><p class='text-right'><?=getInDollars($totalsRakeSum+$totalBonus,2,$row['cur_code_for_web'])?></td>
                        <td style="white-space: nowrap;"><p class='text-right'><?=getInDollars($totalsBetSum-$totalsWinSum-$totalsRakeSum-$totalBonus,2,$row['cur_code_for_web'])?></td>
                        <td style="white-space: nowrap;"><?=getInDollars($parentRake,2,$row['cur_code_for_web'])?></td>
                        <td style="white-space: nowrap;"><?=getInDollars($childRake,2,$row['cur_code_for_web'])?></td>
                    </tr>
                </table>
                <?
                if($_SESSION['jurisdiction_class']=='casino'){
                    $params=",'1','".$nation."'";
                    $_SESSION['betting_perc']=$nationPercent;
                    $_SESSION['betting_live_perc']=$nationPercentLive;

                }
                elseif($_SESSION['jurisdiction_class']=='nation')
                {
                    $params=",1,".$nation.",".$region;
                    $_SESSION['betting_perc']=$regionPercent;
                    $_SESSION['betting_live_perc']=$regionPercentLive;
                }
                elseif($_SESSION['jurisdiction_class']=='region')
                {
                    $params=",1,".$nation.",".$region.",".$district;
                    $_SESSION['betting_perc']=$districtPercent;
                    $_SESSION['betting_live_perc']=$districtPercentLive;
                }
                elseif($_SESSION['jurisdiction_class']=='district')
                {
                    $params=",1,".$nation.",".$region.",".$district.",".$club;
                    $_SESSION['betting_perc']=$clubPercent;
                    $_SESSION['betting_live_perc']=$clubPercentLive;
                }
                ?>
                <button class="btn btn-primary" onclick="confirmBettingClosure('<?=$date_start_betting?>','<?=$date_end_betting?>','<?=$totalsBetSum?>','<?=$totalsWinSum?>','<?=$totalsRakeSum+$totalBonus?>','<?=$cash?>','<?=($parentRake)?>','<?=$childRake?>','<?=$totalsRakeSum?>','<?=$perc_betting?>','<?=$note_betting?>'<?=$params?>)"><?=$lang->getLang("Confirm")?></button><br>
            <?

            }
        }
        else{
            showErrors();
        }
        ?>
    </div>
    <script>$(function(){
            $('#results_betting').show();
        });</script>
<?
}
}
?>
</div>
</div>
</div>
</td>
</tr>
</table>
</div>
</div>
