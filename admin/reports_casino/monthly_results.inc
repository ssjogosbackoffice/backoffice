<div class="modal" id="myModal" style="display: none;margin-top:15%">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-body centered" >
                <?=$lang->getLang("Loading ...")?><img src="media/images/ajax-loader.gif">
            </div>
        </div>
    </div>
</div>
<?php
$active=(isset($_POST["rt"])) ? $_POST["rt"] : 's' ;
$time=(isset($_POST["time"])) ? $_POST["time"] : '1' ;
$date_end= (isset($_POST['date_end'])) ? $_POST['date_end'] : '';
$day_end= (isset($_POST['day_end'])) ? $_POST['day_end'] : '';
$day_start2= (isset($_POST['day_start2'])) ? $_POST['day_start2'] :  date("Y-m-d", time()-3600*24);
$day_end2= (isset($_POST['day_end2'])) ? $_POST['day_end2'] :  date("Y-m-d", time());
$today=date("Y-m-d", time()-3600*24);
?>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap-responsive.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<script src="/media/jquery.dataTables.js" type="text/javascript"></script>
<script type="text/javascript">
    $('#myModal').modal('show');
    $(window).load(function(){
        //hide loader div here
        $("#myModal").modal('hide');
    });
    $(function() {
       <? if($day_end!='' &&($_POST['date_end']==date("Y-m"))){ ?>
            $("#monthdetails").show();
        <?
        }?>
        $( "#date_end" ).datepicker({
            changeMonth: true,
            changeYear: true,
            showButtonPanel: true,
            dateFormat: 'yy-mm',
            onClose: function(dateText, inst) {
                var month = $("#ui-datepicker-div .ui-datepicker-month :selected").val();
                var year = $("#ui-datepicker-div .ui-datepicker-year :selected").val();
                var currentDate = new Date();
                var currentMonth=currentDate.getMonth();
                var currentYear=currentDate.getFullYear();
                $(this).datepicker('setDate', new Date(year, month, 1));
                if(year==currentYear && month==currentMonth){
                    $("#monthdetails").show();
                }
                else{
                    $("#monthdetails").hide();
                }
            }
        });
        $( "#day_end" ).datepicker({
            changeMonth: true,
            changeYear: true,
            minDate: $('#ds').val(),
            maxDate:-1
        });
        $( "#day_start2" ).datepicker({
            changeMonth: true,
            changeYear: true,
            maxDate:-1
        });

        $( "#day_end2" ).datepicker({
            changeMonth: true,
            changeYear: true,
            maxDate:-1
        });

        $("#date_end").focus(function () {
            $(".ui-datepicker-calendar").hide();
            $("#ui-datepicker-div").position({
                my: "center top",
                at: "center bottom",
                of: $(this)
            });
        });

        if($('input[name=time]:checked').val()=='2'){
            $('#month').show();
        }
        else{
            $('#month,#monthdetails').hide();
        }

        $('input[name=time]').on('change',function(){
            if($(this).val()=='1'){
                $('#month,#monthdetails').hide();
            }
            else{
                $('#month').show();
            }


        });

    });
    function duration(duration,divId)
    {
        $('#'+divId).empty().append('<?=$lang->getLang("Time taken to execute your request")?>: '+duration);
    }

    function generateMonthlyReport(time,date){

        $.post("/services/financial_lga.inc",{action:"0",time:time,date_end:date});
        }

    function generateMonthlyReportBets(startDate,endDate,totalBet){
        $.post("/services/financial_services.inc",{action:"generateMonthlyReportBets",startDate:startDate,endDate:endDate,totalBet:totalBet});
    }


</script>
<table class="table table-bordered">
<tr><td class='navbar-inner'><h3 class="text-center"><?=$lang->getLang("Monthly Report")?></h3></td></tr>
<tr>
<td>
<ul class="nav nav-tabs" id='myTab'>
    <li id='amount_li' <?=($active=='s') ? "class='active'":""?>><a href="#amount" data-toggle="tab"><button class="btn btn-success"><?=$lang->getLang("Amount")?></button></a></li>
    <li id='bets_li' <?=($active=='r') ? "class='active'":""?>><a href="#bets" data-toggle="tab"><button class="btn btn-success"><?=$lang->getLang("Bets")?></button> </a></li>
</ul>
<div class="tab-content" style="overflow: visible">
<div class="tab-pane fade <?=($active=='s') ? "in active":""?>" id="amount">
    <?
    /**
     * @author Marian
     */
    form_head()?>
    <?php printHiddenInput('doQuery', 'yes');?>
    <?php printHiddenInput('search_type', 'date_range');?>
    <div class='well'>
        <div class='row'>
            <input class='span' type='hidden' name='rt' value='s' />
            <div class="span3">
                <label class="radio">
                    <input type="radio" name='time' value='1' <?=($time=='1'? 'checked="checked"' : "") ?> > <?=$lang->getLang("Today")?>
                </label>
                <label class="radio">
                    <input type="radio" name='time' value='2' <?=($time=='2'? 'checked="checked"' : "") ?> > <?=$lang->getLang("Specific period")?>
                </label>
            </div><br /><br /><br />
            <div class="span3" id="month"><div class='label label-inverse'><?=$lang->getLang("Month")?></div>
                <br>
                <div class="controls">
                    <div class="input-prepend">
                        <span class="add-on"><i class="icon-calendar"></i></span>
                        <input type='text'  name='date_end' id='date_end' value="<?=$date_end?>">
                    </div>
                </div>
            </div>
        </div><br>
        <div class='row'>
            <div id='monthdetails' style="display: none" class="span3">
                <div class="span2"><div class='label label-inverse' id='datestartlabel'><?=$lang->getLang("From")?></div> <br>
                        <span class="add-on"><i class="icon-calendar"></i>
                        <span  id='day_start'><?=date('Y-m-01');?></span></span>
                    <input type='hidden' name='day_start' id='ds' value="<?=date('Y-m-01')?>">
                </div><br>
                <div class="span2"><div class='label label-inverse'><?=$lang->getLang("Until")?></div>
                    <br><br>
                    <div class="controls">
                        <div class="input-prepend">
                            <span class="add-on"><i class="icon-calendar"></i></span>
                            <input type='text'  name='day_end' id='day_end' value="<?=$day_end?>">
                        </div>
                    </div>
                </div>
            </div>
        </div><br>
        <div class='row'>
            <div class='span4'>
                <button class="btn btn-primary" ><?=$lang->getLang("View")?></button>
            </div>
            <?php
            if(isset($_POST['doQuery']) &&($_POST['rt']=='s')){
                if(date("n", strtotime($date_end)) != date("n")) {
                    $day=date("Y-m-t", strtotime($date_end));
                }
                else{
                    $day=$day_end;
                }
                if($time==1){
                    $day=$today;
                }
                ?>
                <div id='excelContainer' class='span3'><img src='/media/images/excel.png'style="width:30px; height:30px" >
                    <?
                    if($time==1){
                        if(file_exists('documents/today.xlsx')) {
                            unlink('documents/today.xlsx');
                        }
                            ?>
                            <span id="generatingExcel">
                                <?=$lang->getLang("Generating report..")?> <img src="media/images/ajax-loader.gif">
                            </span>
                                <script>
                                    var start_time = new Date().getTime();
                                    var interval = null;
                                    file='documents/today.xlsx';
                                    function checkFile(){
                                            var http = new XMLHttpRequest();
                                            http.open('HEAD', file, false);
                                            http.send();
                                            if(http.status==200){
                                                var request_time = new Date().getTime() - start_time;
                                                $('#generatingExcel').empty().append("<a href='documents/today.xlsx'><?=$lang->getLang("Download as Excel")?></a>");
                                                clearInterval(interval);
                                            }
                                    }
                                    $(function(){
                                        generateMonthlyReport('<?=$time?>','<?=$day?>');
                                        interval=setInterval(checkFile,1000);
                                    });
                                    </script>
                            <?
                    }
                    else {
                    if(file_exists("documents/".$day.".xlsx") ){?>
                        <a href='documents/<?=$day?>.xlsx'><?=$lang->getLang("Download as Excel")?></a>
                    <? }
                    else { ?>
                        <span><?=$lang->getLang("There is no report generated")?>!</span>
                    <?
                        }
                    }
                    ?>
                </div>
            <?
            } ?>
        </div>
        </form><br><br>
    </div>
    <?php
    if(isset($_POST['doQuery']) && $_POST['rt']=='s'){
    $action='1';
        if($time==1){
            $sql2="Select sum(pcr_credits) total_bet,count(*) as totusers,cur_code_for_web
                            FROM punter
                            LEFT JOIN punter_credit on pcr_pun_id=pun_id
                            LEFT JOIN jurisdiction on jur_id=pun_betting_club
                            LEFT JOIN customer_monthly_summary on  cms_pun_id = pun_id AND cms_day='$day'
                            LEFT JOIN country on cou_id = pun_cou_id
                            LEFT JOIN currency on jur_currency=cur_id ";
        }
        else{
            $sql2="Select sum(cms_available_credit) total_bet,count(*) as totusers,cur_code_for_web
                            FROM punter
                            LEFT JOIN customer_monthly_summary on  cms_pun_id = pun_id AND cms_day='$day'
                            LEFT JOIN country on cou_id = pun_cou_id
                            LEFT JOIN jurisdiction on jur_id=pun_betting_club
                            LEFT JOIN currency on jur_currency=cur_id  ";
        }
        $total_amount=$dbh->queryRow($sql2);
        $totals=$total_amount['totusers'];
        $total_amount=$total_amount['total_bet'];
    ?>
    <script>
        $(document).ready( function() {
        $('#paginator').dataTable( {
        "bProcessing": true,
        "bServerSide": true,
        "bSort":false,
        "sPaginationType":"full_numbers",
        "bFilter":false,
        "sAjaxSource": "services/financial_lga.inc",
            "fnServerParams": function ( aoData ) {
                aoData.push( { "name": "action", "value": '1' },
                             { "name": "time", "value":"<?=$time?>" },
                             { "name": "totals", "value": '<?=$totals?>' },
                             { "name": "date_end", "value":"<?=$day?>" });
            }
        });
        });
    </script>
   <!-- <div id='duration' class="tip fleft"><?/*=$lang->getLang("Time taken to execute your request")*/?>: <?/*=$duration*/?></div>-->
            <div id='results'>
                    <table class='display table table-striped table-condensed'>
                        <thead>
                        <tr>
                            <th><?=$lang->getLang("Total Player Balance");?>  <?=getInDollars($total_amount)?></th>
                        </tr>
                        </thead>
                    </table><br />
                    <table class='display table table-striped table-bordered table-hover table-condensed'  id='paginator'>
                        <thead>
                        <tr><th style='white-space: nowrap;'><strong><?=$lang->getLang("Username")?></strong></th>
                            <th style='white-space: nowrap;'><strong><?=$lang->getLang("User code")?></strong></th>
                            <th style='white-space: nowrap;'><strong><?=$lang->getLang("Name")?></strong></th>
                            <th style='white-space: nowrap;'><strong><?=$lang->getLang("Last Name")?></strong></th>
                            <th style='white-space: nowrap;'><strong><?=$lang->getLang("Email")?></strong></th>
                            <th style='white-space: nowrap;'><strong><?=$lang->getLang("Amount")?></strong></th>
                            <th style='white-space: nowrap;'><strong><?=$lang->getLang("Currency")?></strong></th>
                            <th style='white-space: nowrap;'><strong><?=$lang->getLang("Country")?></strong></th>
                            <th style='white-space: nowrap;'><strong><?=$lang->getLang("Birthday")?></strong></th>
                            <th style='white-space: nowrap;'><strong><?=$lang->getLang("Last Login")?></strong></th>
                            <th style='white-space: nowrap;'><strong><?=$lang->getLang('Total bets')?></strong></th>
                        </tr>
                        </thead>
                    </table>
                    <br />
                    <table class='display table table-striped table-condensed'>
                        <thead>
                        <tr>
                            <th><?=$lang->getLang("Total Player Balance");?>  <?=getInDollars($total_amount)?></th>
                        </tr>
                        </thead>
                    </table>
            </div>
                <?
                }
    ?>
</div>



<div class="tab-pane fade <?=($active=='r') ? "in active":""?>" id="bets">
<? /**
 * @author Marian
 */ form_head()?>
<?php printHiddenInput('doQuery', 'yes');?>
<?php printHiddenInput('search_type', 'date_range');?>

<div class='well'>
    <div class='row'>
        <input type="hidden" name='rt' value='r'>
        <div class="span3"><div class='label label-inverse'><?=$lang->getLang("From")?></div>
            <br>
            <div class="controls">
                <div class="input-prepend">
                    <span class="add-on"><i class="icon-calendar"></i></span>
                    <input type='text'  name='day_start2' id='day_start2' value="<?=$day_start2?>">
                </div>
            </div>
        </div>
        <br />
    </div>
    <br />
    <div class='row'>
        <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Until")?></div>
            <br>
            <div class="controls">
                <div class="input-prepend">
                    <span class="add-on"><i class="icon-calendar"></i></span>
                    <input type='text'  name='day_end2' id='day_end2' value="<?=$day_end2?>">
                </div>
            </div>
        </div>
    </div><br>
    <div class='row'>
        <div class='span4'>
            <button class="btn btn-primary" ><?=$lang->getLang("View")?></button>
        </div>
        <?php
        if(isset($_POST['doQuery']) && $_POST['rt']=='r'){
            ?>
            <?
            if(file_exists('documents/today.xlsx')) {
                unlink('documents/today.xlsx');
            }
            ?>
            <span id="generatingExcelBets">
                                <?=$lang->getLang("Generating report..")?> <img src="media/images/ajax-loader.gif">
                            </span>
            <script>
                var start_time = new Date().getTime();
                var interval = null;
                file='documents/Games played from <?=$day_start2?> until <?=$day_end2?>.xlsx';
                function checkFile(){
                    var http = new XMLHttpRequest();
                    http.open('HEAD', file, false);
                    http.send();
                    if(http.status==200){
                        var request_time = new Date().getTime() - start_time;
                        $('#generatingExcelBets').empty().append("<a href='"+file+"'><?=$lang->getLang("Download as Excel")?></a>");
                        clearInterval(interval);
                    }
                }
            </script>
        <?
        }
        ?>
    </div>
    <?php
    if(isset($_POST['doQuery']) && $_POST['rt']=='r'){?>
        <div id='duration2' class="tip fleft"><?=$lang->getLang("Time taken to execute your request")?>: <?=$duration?></div>
    <? }
    ?>
    </form><br><br>
</div>
<?php
if(isset($_POST['doQuery'])  && $_POST['rt']=='r'){
    ?>
        <div id='results'>
            <?

            $starttime = microtime(true);

            $sql2="SELECT  sum(ABS(ctr_amount)) as total_bet
                             FROM
                             customer_transaction, punter
                             where ctr_time BETWEEN '$day_start2 00:00:00' AND '$day_end2 23:59:59'
                             and ctr_pun_id = pun_id
                             and ctr_tty_id = 8
                             and ctr_gam_id = 210
                             and ctr_pme_code = 'CSH'
                             order by ctr_time asc ";
            $totalbet=$dbh->queryOne($sql2);
            $endtime = microtime(true);
            $duration = $endtime - $starttime;
            $duration=number_format($duration, 4, ',', '')."s";
            ?>
            <script>duration('<?=$duration?>','duration2');
                $(function(){
                        $('#paginator2').DataTable( {
                            "processing": true,
                            "serverSide": true,
                            "ajax":{
                                "url": "services/financial_services.inc",
                                "data": function ( d ) {
                                    d.action = "getMonthlyReportBets";
                                    d.startDate="<?=$day_start2?>";
                                    d.endDate="<?=$day_end2?>" ;
                                }
                            }, 'iDisplayLength': 10,'sPaginationType':"full_numbers"
                        });
                    <?if($totalbet >0) { ?>
                    generateMonthlyReportBets('<?=$day_start2?>','<?=$day_end2?>','<?=$totalbet?>');
                    interval=setInterval(checkFile,1000);
                    <? }
                    else{ ?>
                    $('#generatingExcelBets').empty();
                    <? }
                    ?>
                });</script>


                <table class='display table table-striped table-bordered table-hover table-condensed'>
                    <thead><tr><th style="font-weight: bold"><?=$lang->getLang('Total bet')?>: <?=getInDollars($totalbet);?><br />
                            <?=$lang->getLang('Total Tax %','LGA')?>: <?=getInDollars($totalbet*0.005)?>
                        </th>
                    </tr>
                    </thead>
                </table><br />

                <table class='display table table-striped table-bordered table-hover table-condensed'  id='paginator2'>
                    <thead>
                    <tr><th style='white-space: nowrap;'><strong><?=$lang->getLang("Username")?></strong></th>
                        <th style='white-space: nowrap;'><strong><?=$lang->getLang("Date")?></strong></th>
                        <th style='white-space: nowrap;'><strong><?=$lang->getLang("Bet Amount")?></strong></th>
                        <th style='white-space: nowrap;'><strong><?=$lang->getLang('Note')?></strong></th>
                    </tr>
                    </thead><tbody id='tablebody2'>
                    <tr>
                        <td></td>
                        <td></td>
                        <td></td>
                        <td></td>
                        </tr>
                    </tbody>
                </table>
                <br />
                <table class='display table table-striped table-bordered table-hover table-condensed'>
                    <thead><tr><th style="font-weight: bold"><?=$lang->getLang('Total bet')?>: <?=getInDollars($totalbet);?><br />
                            <?=$lang->getLang('Total Tax %','LGA')?>: <?=getInDollars($totalbet*0.005)?>
                        </th>
                    </tr>
                    </thead>
                </table>

        </div>
    <?

}
?>
</div>
</div>
</td>
</tr>
</table>
