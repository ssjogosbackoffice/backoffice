<?
check_access('lotto_financial_reports',true);
global $lang;
$canViewErrors=check_access('lotto_casino_show_errors');
$code=$_POST['lotteries'];
$periodType=(isset($_POST["periodType"])) ? $_POST["periodType"] : '0';
$date_start = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d 00:00", time() );
$date_end = (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d 23:59", time());
$active = (isset($_POST['active']) ? $_POST['active'] : 'lottoResults' );
$missing = (isset($_POST['missingType']) ? $_POST['missingType'] : '0' );

$sql="SELECT gls_code,gls_cou_code,gls_name from game_lotto where gls_status=1 group by gls_code";
$results=$dbh->doCachedQuery($sql,60);
$lottoCountries=array();
$allCountries=$dbh->doCachedQuery("SELECT cou_code,cou_name from country",0);
$allCountries=$allCountries->toArray();
$allCountries=formatCountry($allCountries);
while($results->hasNext()){
    $row=$results->next();
    if(!is_array($lottoCountries[$allCountries[$row['gls_cou_code']]])){
        $lottoCountries[$allCountries[$row['gls_cou_code']]]=array();
    }
    $lottoCountries[$allCountries[$row['gls_cou_code']]][$row['gls_code']]=$row['gls_name'];
}
ksort($lottoCountries);
?>
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="/media/bootstrap/css/bootstrap-multiselect.css" >
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/FlipClock/compiled/flipclock.css" >

    <script src="/media/jquery.dataTables.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
    <script src="/media/bootstrap/js/bootstrap-multiselect.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/jstree/jstree.js" type="text/javascript"></script>
    <script src="/media/auth_functions.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/FlipClock/compiled/flipclock.js" type="text/javascript"></script>
    <style>
        .jstree-anchor {
            display: inline;
        }

        .multiselect-clear-filter{
            display: none;
        }
        a{
            text-decoration: none !important;
        }
        .top, .bottom  {
            padding: 1px;
            background-color: rgb(255, 255, 255);
            border: 1px solid rgb(204, 204, 204);
        }
        #loadingDiv {
            display: none;
            background-color: rgba(255, 255, 255, 0.47);
            font-size: 12px;
            z-index: 1010;
            position: fixed;
            top: 0;
            bottom: 0;
            left: 0;
            right: 0;
            margin: auto;
            padding-top: 200px;
        }

        .multiselect-search{
            width: 93% !important;
        }
        button.multiselect {
            border-top-right-radius:5px !important;
            border-top-left-radius:0 !important;
            border-bottom-right-radius:5px !important;
            border-bottom-left-radius:0 !important;
        }
        .dataTables_wrapper {
            position: inherit;
        }

        .searchTable {
            background-color: rgba(0, 0, 0, 0) !important;
            width: 440px !important;
            margin:auto;
            margin-bottom: 30px;
        }
        #lottosResultTable{
            cursor: pointer;
        }
    </style>
    <script>
        var servicesUrl="/services/lotto_services.php";
        var _0xf14b=["\x76\x61\x6C","\x23\x72\x65\x66\x75\x6E\x64\x54\x6F\x6B\x65\x6E","\x20\x20\x20"];
        function checkToken(){token=MD5($(_0xf14b[1])[_0xf14b[0]]());return MD5(token+'<?=$_SESSION['admin_username']?>');} ;

        var lottoObj=function(code,date,extractionCode){
            this.code=code;
            this.date=date;
            this.extractionCode=extractionCode;
            this.setCodeAndDate=function(code,date,extractionCode){
                this.code=code;
                this.date=date;
                this.extractionCode=extractionCode;
            };
            this.updateDuration=function(duration){
                    $("#duration").html('<?=$lang->getLang("Time taken to execute your request")?>:'+ duration/1000+" s");
            };
        };

        lottoObj.prototype.prettyResult=function(dataHtml){
            var prettyData="<div>" +
                "<table class='table table-bordered'><tr>" +
                "<td class='blackTd'><?=$lang->getLang('Lottery')?></td><td>"+dataHtml["0"]+"</td></tr>"+
                "<td class='blackTd'><?=$lang->getLang('Extraction')?></td><td>"+dataHtml["5"]+"</td></tr>"+
                "<tr><td class='blackTd'><?=$lang->getLang('Result')?></td><td><textarea id='newLottoValue'>"+(dataHtml[1]!=null?dataHtml[1]:'')+"</textarea></td></tr>"+
                "<tr><td class='blackTd'><?=$lang->getLang('Extra result')?></td><td><textarea id='newExtraResultLottoValue'>"+(dataHtml[2]!=null?dataHtml[2]:'')+"</textarea></td></tr>"+
                "<tr><td class='blackTd'><?=$lang->getLang('Time')?></td><td>"+dataHtml[3]+"</td></tr>"+
                "<tr><td class='blackTd'><?=$lang->getLang('Time of extraction')?></td><td><input type='text' class='datepickerinput' id='newTimeOfExtraction' value='"+dataHtml[4]+"'></td></tr>" +
                "<tr><td class='blackTd'><?=$lang->getLang('Your password')?></td><td> <input type='password'  name='refundToken' id='refundToken' required='required' /></td></tr>"+
                "<tr><td colspan='2'><button class='btn btn-small btn-danger fright updateLotteryResultButton' disabled><?=$lang->getLang('Update')?></button></td></tr>"+
                "<table>";
            return prettyData;
        };


        lottoObj.prototype.getDetails=function(updateResultInfo){
            var $self = this;
            startTime=new Date;
            $.post(servicesUrl,{
                pk:"getLottoExtractionDetails",
                code:this.code,
                date:this.date,
                extractionCode:this.extractionCode
            },function(data){
                endTime = new Date;
                totalTime = endTime - startTime;
                $self.updateDuration(totalTime);
                data=JSON.parse(data);
                $('#delResult').empty();
                $("#userDetails").empty().append( $self.prettyResult(data));
                $('.datepickerinput').datetimepicker({
                    showSecond: true,
                    timeFormat: 'HH:mm:ss',
                    onChangeMonthYear:function(year, month, i){
                        var day = i.selectedDay;
                        $(this).datepicker('setDate', new Date(year, month-1, day));
                    }
                });
                if(updateResultInfo) {
                    updateResultInfo();
                }
            });
            return false;
        };
        lottoObj.prototype.update=function(){
            var $self = this;
            $.post(servicesUrl,{
                pk:"updateLottoExtraction",
                code:this.code,
                date:this.date,
                extractionCode:this.extractionCode,
                resultValue:$('#newLottoValue').val(),
                extraResultValue:$('#newExtraResultLottoValue').val(),
                timeofExtraction:$('#newTimeOfExtraction').val(),
                token:checkToken()
            },function(data){
                if(data==1){
                    $('#refundToken').val('');
                    $self.getDetails(function(){
                     $('#delResult').empty().append("<h4 class='alert alert-success fade in'><button type='button' class='close' data-dismiss='alert'>&times;</button><?=$lang->getLang('You have successfully updated the result')?>!</h4><br />");
                    });
                }
                else if(data==-2){
                    $("#delResult").empty().append("<h4 class='alert alert-error fade in'><button type='button' class='close' data-dismiss='alert'>&times;</button><?=$lang->getLang("Invalid password")?></h4><br />");
                }
                else{
                    $('#refundToken').val('');
                    $('#delResult').empty().append("<h4 class='alert alert-error fade in'><button type='button' class='close' data-dismiss='alert'>&times;</button><?=$lang->getLang("An error occurred!Result not updated!")?></h4><br />");
                }
            });
            return false;
        };

        $("body").on({
            ajaxStart: function() {
                //$('#loadingDiv').show();
                startTime=new Date;
            },
            ajaxStop: function() {
                $('#loadingDiv').hide();
                endTime = new Date;
                totalTime = endTime - startTime;
                $("#duration").html('<?=$lang->getLang("Time taken to execute your request")?>:'+ totalTime/1000+" s");
            },
            ajaxError:function(){
                jAlert("An error has occurred.Please try again");
                $('#loadingDiv').hide();
                endTime = new Date;
                totalTime = endTime - startTime;
                $("#duration").html('<?=$lang->getLang("Time taken to execute your request")?>:'+ totalTime/1000+" s");
            }
        });

        $(document).on('keyup','#refundToken',function(){
                $('.updateLotteryResultButton').prop('disabled', this.value == "" ? true : false);
        });

        $(document).ready(function() {
          var   checkForResultsInt=false;
            $('#jurisdictionTreeSearch').keyup(function () {
                $("#jurisdictionTree").jstree('search',$('#jurisdictionTreeSearch').val());
            });
            $("#jurisdictionTree")
                .jstree({
                    core:{"multiple":false
                    },
                    "plugins" : [
                        "search",
                        "types",
                        "state"
                    ],
                    "search":{
                        "fuzzy":false,
                        "show_only_matches":true
                    },
                    "types" : {
                        "district" : {
                            "icon" :  "../media/images/c.png"
                        },
                        "club" : {
                            "icon":false
                        }
                    }
                });
            $('.periodType').trigger('change');
            <?if(check_access('lotto_casino_result_manage')){?>
            $('#lottosResultTable').on('click','td',function(){
                var lottoCode=$(this).parent().find('.lottoCode').text();
                var lottoDate=$(this).parent().find('.lottoTime').text();
                var lottoExtractionCode=$(this).parent().find('.lottoExtractionCode').text();

                lottery=new lottoObj(lottoCode,lottoDate,lottoExtractionCode);
                lottery.getDetails();
            });
            <? } ?>
            $('.lotteries').multiselect({
                enableFiltering: true,
                maxHeight: 300,
                multiple:true,
                includeSelectAllOption: true,
                enableClickableOptGroups: true,
                enableCaseInsensitiveFiltering:true
            });
            /*  table of results */
           var resultsTable= $('#lottosResultTable').DataTable( {
                "processing": true,
                "serverSide": true,
               "order": [[ 7, "asc" ]],
                columnDefs: [
                    {
                        targets: 6 ,
                        className:'lottoTime'
                    },
                    { "targets": 4, "orderable": false },
                    { "targets": 5, "orderable": false }
                ],
                "ajax":{
                    "url": servicesUrl,
                    "data": function ( d ) {
                        d.pk = "getLottoResults";
                        d.code=JSON.stringify($("#lotteries").val());
                        d.startDate=$("#date_start").val();
                        d.endDate=$("#date_end").val();
                        d.type=$("#missingType").val();
                    }
                }, 'iDisplayLength': 10,'sPaginationType':"full_numbers"
            });


            /* table of errors */
            <?if($canViewErrors) { ?>
            $('#lottosErrorsTable').DataTable( {
                "processing": true,
                "serverSide": true,

                columnDefs: [
                    {
                        targets: 2 ,
                        className:'unwrappable'
                    }

                ],
                "ajax":{
                    "url": servicesUrl,
                    "data": function ( d ) {
                        d.pk = "getLottoErrors";
                        d.code=JSON.stringify($("#lotteriesErr").val());
                        d.startDate=$("#date_start_err").val();
                        d.endDate=$("#date_end_err").val();
                    }
                }, 'iDisplayLength': 10,'sPaginationType':"full_numbers"
            });

             <? } ?>

            $( ".dateStartHelper" ).datetimepicker({
                changeMonth: true,
                changeYear: true,
                numberOfMonths:1,
                onSelect: function( selectedDate ) {
                    $( ".monthHelper, .weekHelper" ).val('');
                },
                onChangeMonthYear:function(year, month, i){
                    var day = i.selectedDay;
                    $(this).datepicker('setDate', new Date(year, month-1, day));
                }
            });
            $( ".dateEndHelper" ).datetimepicker({
                changeMonth: true,
                changeYear: true,
                numberOfMonths:1,
                onSelect: function( selectedDate ) {
                    $( ".monthHelper, .weekHelper" ).val('');
                },
                onChangeMonthYear:function(year, month, i){
                    var day = i.selectedDay;
                    $(this).datepicker('setDate', new Date(year, month-1, day));
                }
            });


            table = $('#lottosIncomingTable').DataTable({
                "order": [[ 4, "asc" ]],
                 columnDefs: [
                    {
                        targets: 1 ,
                        className:'lotteryCode'
                    },
                    {
                        "targets": 4,
                        "className":'unwrappable' }
                ],
                "processing": true,
                "serverSide": true,
                "ajax":{
                    "url": servicesUrl,
                    "data": function ( d ) {
                        d.pk = "getLotteriesIncomingResults";
                        d.lottos= JSON.stringify($("#lotteriesInc").val());
                    }
                },
                "fnDrawCallback": function (nRow, aData, iDisplayIndex, iDisplayIndexFull) {
                    counter=0;
                    $clock=[];
                    clearInterval(checkForResultsInt);

                    $('.countdownContainer').each(function(){
                        var $_self=$(this);
                        var code=$_self.closest('tr').find('.lotteryCode').html();
                        var time=$(this).html();
                        var $_endDate=$(this).attr('data-end');
                        if(time!='') {
                            var clockFace=(time<=24*3600 ? 'HourlyCounter' : 'DailyCounter');
                            $clock[counter] = $(this).FlipClock(time, {
                                countdown: true,
                                clockFace: clockFace,
                                callbacks:{
                                    stop:function() {
                                        $_self.fadeOut(function() {
                                            $(this).html('<span class="text-success dots"><?=$lang->getLang('Waiting for result')?><span>.</span><span>.</span><span>.</span> </span>')
                                        }).fadeIn();
                                        pendingResults.addLotteriesId(code,$_endDate);
                                    }
                                }
                            });
                        }
                    });
                },
                'iDisplayLength': 10,'sPaginationType':"full_numbers",
                "lengthMenu": [ 10, 25]
            });

            pendingResults={
                pendingLotteriesResults:{},
                addLotteriesId:function(key,value){
                    this.pendingLotteriesResults[key]=value;
                    if(!checkForResultsInt) {
                        checkForResultsInt = window.setInterval("pendingResults.checkForResults()", 25000);
                    }
                },
                removeLotteriesId:function(id){
                    delete(this.pendingLotteriesResults[id]);
                    if(jQuery.isEmptyObject(this.pendingLotteriesResults)){
                        clearInterval(checkForResultsInt);
                        checkForResultsInt = false;
                        table.ajax.reload();
                    }
                },
                checkForResults:function(){
                    $.post(servicesUrl,{
                        pk:"checkForResults",
                        lotteries:JSON.stringify(this.pendingLotteriesResults)
                    },function(data){

                        data=JSON.parse(data);
                        if(data!='') {
                            resultsTable.ajax.reload();
                        }
                        for (var key in data) {
                            $('td.lotteryCode:contains("'+key+'")').closest('tr').fadeOut();
                            pendingResults.removeLotteriesId(key);
                        }
                    });
                }
            };
        });

        $(document).on('click', ".updateLotteryResultButton", function() {
            lottery.update();

        });
        $(document).on('change', ".periodType", function() {
            var currVal=$(this).val();
            $(this).closest('table').find('.timeHelpers').hide();
            $(this).closest('table').find('.period'+currVal).show();
        });

        $(document).on('change', '.monthHelper', function() {
            var d = new Date($(this).val());
            createMonthHelper(d,$(this));
        });
        $(document).on('change', '.weekHelper', function() {
            createWeekHelper($(this).val(),$(this));
        });

        function createWeekHelper(dateobj,container){
            if (typeof dateobj !== 'undefined' || dateobj!=''){
                dates=dateobj.split('~');
                firstDay=dates[0];
                lastDay=dates[1];
                container.closest('table').find( ".dateStartHelper" ).datepicker('setDate', firstDay);
                container.closest('table').find( ".dateEndHelper" ).datepicker('setDate', lastDay);
                container.closest('table').find('.monthHelper').val('');
            }
        }

        function createMonthHelper(dateobj,container){
            if (typeof dateobj !== 'undefined' || dateobj!=''){
                var firstDay = new Date(dateobj.getFullYear(), dateobj.getMonth(), 1);
                container.closest('table').find( ".dateStartHelper" ).datepicker('setDate', firstDay);
                var lastDay = new Date(dateobj.getFullYear(), dateobj.getMonth() + 1, 0);
                container.closest('table').find( ".dateEndHelper" ).datepicker('setDate', lastDay);
                container.closest('table').find( ".weekHelper").val('');
            }
        }
    </script>

<div class="container-fluid">
    <div id='loadingDiv' class="centered"><img src="<?=image_dir?>/ajax-loader.gif" /></div>

    <table class="table table-bordered">
        <tr>
            <td class="navbar-inner" colspan="3"  ><h3 class="text-center"><?=$lang->getLang("Lotto results")?></h3></td>
        </tr>
        <tr>
            <td class="span4">
                <div class="contentAdmin">
                    <div class="contentHeadAdmin"><?=$lang->getLang("Lottery helper")?><br /><span class="tip" style="text-transform:lowercase">(<?=$lang->getLang('Help only')?>  )</span> </div>
                    <div class="well">
                        <input type="text" id='jurisdictionTreeSearch' placeholder="<?=$lang->getLang("Search a lottery")?>">
                        <div id="jurisdictionTree">
                            <ul>
                                <?=createLotteriesTree($lottoCountries);?>
                            </ul>
                        </div>
                    </div>
                </div>
            </td>
            <td>
                <div class="tabbable">
                    <ul class="nav nav-tabs">
                        <li class="<?=($active=='lottoResults'? 'active':'')?>"><a href="#lottoResults" data-toggle="tab"><button class="btn btn-inverse" ><?=$lang->getLang("Results")?></button></a></li>
                        <?if($canViewErrors) { ?>
                        <li class="<?=($active=='lottoErrors'? 'active':'')?>"><a href="#lottoErrors" data-toggle="tab"><button class="btn btn-inverse" ><?=$lang->getLang("Errors")?></button></a></li>
                        <? } ?>
                        <li class="<?=($active=='lottoIncoming'? 'active':'')?>"><a href="#lottoIncoming" data-toggle="tab"><button class="btn btn-inverse" ><?=$lang->getLang("Incoming results")?></button></a></li>
                    </ul>
                    <div class="tab-content">
                        <div class="tab-pane <?=($active=='lottoResults'? 'active':'')?>" id="lottoResults">
                            <table>
                                <tr>
                                    <td colspan="2">
                                        <div class="well">
                                            <form action="/lotto_results" method="post">
                                                <input name="active" type="hidden" value="lottoResults">
                                                <div class="row">
                                                    <div class="span5"><div class='label label-inverse'><?=$lang->getLang("Lottery")?></div>
                                                        <br>
                                                        <div class="controls">
                                                            <div class="input-prepend">
                                                                <span class="add-on"><i class="icon-random"></i></span>
                                                                <select class="form-control lotteries" name="lotteries[]" id="lotteries" multiple="multiple" >
                                                                    <?foreach($lottoCountries as $countries=>$lotto){ ?>
                                                                    <optgroup label="<?=$countries?>" value="<?=$countries?>"> <?
                                                                        foreach($lotto as $k=>$v){ ?>
                                                                            <option <?=((in_array($k,$_POST['lotteries']) || empty($_POST['lotteries']))?"selected" :"")?>  value="<?=$k?>" ><?=$v?>-<?=$k?></option>
                                                                            <?
                                                                        }?>

                                                                       </optgroup>
                                                                     <?   } ?>
                                                                </select>
                                                            </div>
                                                        </div><br /><br />

                                                        <div class='label label-inverse'><?=$lang->getLang("Type")?></div>
                                                        <br>
                                                        <div class="controls">
                                                            <div class="input-prepend">
                                                                <span class="add-on"><i class="icon-random"></i></span>
                                                                <select class="form-control" name="missingType" id="missingType"  >
                                                                    <option <?=($missing==0 ? "selected":"")?>  value="0" ><?=$lang->getLang('All')?></option>
                                                                    <option <?=($missing==1 ? "selected":"")?>  value="1" ><?=$lang->getLang('Waiting for result')?></option>
                                                                    <option <?=($missing==2 ? "selected":"")?> value="2" ><?=$lang->getLang('Already has result')?></option>
                                                                </select>
                                                            </div>
                                                        </div>
                                                    </div>
                                                    <div class="span7">
                                                        <table  class="searchTable table table-bordered">
                                                            <tbody>
                                                            <tr><td>
                                                                    <div class='label label-inverse'><?=$lang->getLang("Choose period type")?></div></td><td>
                                                                    <select name="periodType"  class="periodType fleft">
                                                                        <option value="0" <?=($periodType==0?'selected': '')?>><?=$lang->getLang("Custom")?></option>
                                                                        <option value="1" <?=($periodType==1?'selected': '')?>><?=$lang->getLang("Weekly")?></option>
                                                                        <option value="2" <?=($periodType==2?'selected': '')?>><?=$lang->getLang("Monthly")?></option>
                                                                    </select>
                                                                </td>
                                                            </tr>
                                                            <tr  class="period2 timeHelpers">
                                                                <td><div class='label label-inverse'><?=$lang->getLang("Choose month")?></div></td>
                                                                <td>
                                                                    <div class="controls">
                                                                        <div class="input-prepend">
                                                                            <span class="add-on"><i class="icon-calendar"></i></span>
                                                                            <?=month_select_box()?>
                                                                        </div>
                                                                    </div></td>
                                                            </tr>
                                                            <tr  class="period1 timeHelpers">
                                                                <td><div class='label label-inverse'><?=$lang->getLang("Choose week")?></div></td>
                                                                <td>
                                                                    <div class="controls">
                                                                        <div class="input-prepend">
                                                                            <span class="add-on"><i class="icon-calendar"></i></span>
                                                                            <?=week_select_box()?>
                                                                        </div>
                                                                    </div>
                                                                </td>
                                                            </tr>

                                                            <tr>
                                                                <td><div class='label label-inverse'><?=$lang->getLang("From")?></div></td>
                                                                <td><div class="controls">
                                                                        <div class="input-prepend">
                                                                            <span class="add-on"><i class="icon-calendar"></i></span>
                                                                            <input type='text' name='date_start' id='date_start' class="dateStartHelper"  value="<?=$date_start?>" >
                                                                        </div>
                                                                    </div>
                                                                </td>
                                                            </tr>
                                                            <tr>
                                                                <td><div class='label label-inverse'><?=$lang->getLang("Until")?></div></td>
                                                                <td><div class="controls">
                                                                        <div class="input-prepend">
                                                                            <span class="add-on"><i class="icon-calendar"></i></span>
                                                                            <input type='text' name='date_end' id='date_end' class="dateEndHelper" value="<?=$date_end?>">
                                                                        </div>
                                                                    </div>
                                                                </td>
                                                            </tr>
                                                            </tbody>
                                                        </table>
                                                    </div>
                                                </div>
                                                <div class="row">
                                                    <span class="span2"> <button class="btn btn-primary" id="SearchTicket"><?=$lang->getLang("Search")?></button></span>
                                                </div><br />
                                            </form>
                                            <br />
                                            <div id='duration' class="tip fleft"></div>
                                            <br />
                                        </div>
                                    </td>
                                </tr>
                                <tr>
                                    <td>
                                        <table id="lottosResultTable" class="display table table-striped table-hover table-bordered table-condensed" >
                                            <thead>
                                            <tr>
                                                <th><?=$lang->getLang('Lottery name')?></th>
                                                <th><?=$lang->getLang('Country')?></th>
                                                <th><?=$lang->getLang('Extraction code')?></th>
                                                <th><?=$lang->getLang('Extraction id')?></th>
                                                <th><?=$lang->getLang('Lottery result')?></th>
                                                <th><?=$lang->getLang('Lottery extra result')?></th>
                                                <th><?=$lang->getLang('Lottery time')?></th>
                                                <th><?=$lang->getLang('Lottery time of extraction')?></th>
                                            </tr>
                                            </thead>
                                            <tbody>
                                            <tr>
                                                <td></td>
                                                <td></td>
                                                <td></td>
                                                <td></td>
                                                <td></td>
                                                <td></td>
                                                <td class="lottoTime"> </td>
                                                <td></td>
                                            </tr>
                                            </tbody>
                                        </table>
                                    </td>
                                    <td>
                                        <div id="delResult"  style="padding: 15px;"></div>
                                        <div id="userDetails"></div>
                                    </td>
                                </tr>
                            </table>
                        </div>
                        <?if($canViewErrors) { ?>
                        <div class="tab-pane <?=($active=='lottoErrors'? 'active':'')?>" id="lottoErrors">
                            <table>
                                <tr>
                                    <td>
                                        <div class="well">
                                            <form action="/lotto_results" method="post">
                                                <input name="active" type='hidden' value="lottoErrors">
                                                <div class="row">
                                                    <div class="span5"><div class='label label-inverse'><?=$lang->getLang("Lottery")?></div>
                                                        <br>
                                                        <div class="controls">
                                                            <div class="input-prepend">
                                                                <span class="add-on"><i class="icon-random"></i></span>
                                                                <select class="lotteries form-control" name="lotteries[]" id="lotteriesErr" multiple="multiple" >
                                                                    <?foreach($lottoCountries as $countries=>$lotto){ ?>
                                                                    <optgroup label="<?=$countries?>" value="<?=$countries?>"> <?
                                                                        foreach($lotto as $k=>$v){ ?>
                                                                            <option <?=((in_array($k,$_POST['lotteries']) || empty($_POST['lotteries']))?"selected" :"")?>  value="<?=$k?>" ><?=$v?></option>
                                                                            <?
                                                                        }
                                                                        } ?>
                                                                </select>
                                                            </div>
                                                        </div>
                                                    </div>
                                                    <div class="span7">
                                                        <table  class="searchTable table table-bordered">
                                                            <tbody>
                                                            <tr><td>
                                                                    <div class='label label-inverse'><?=$lang->getLang("Choose period type")?></div></td><td>
                                                                    <select name="periodType" class="periodType fleft">
                                                                        <option value="0" <?=($periodType==0?'selected': '')?>><?=$lang->getLang("Custom")?></option>
                                                                        <option value="1" <?=($periodType==1?'selected': '')?>><?=$lang->getLang("Weekly")?></option>
                                                                        <option value="2" <?=($periodType==2?'selected': '')?>><?=$lang->getLang("Monthly")?></option>
                                                                    </select>
                                                                </td>
                                                            </tr>
                                                            <tr  class="timeHelpers period2">
                                                                <td><div class='label label-inverse'><?=$lang->getLang("Choose month")?></div></td>
                                                                <td>
                                                                    <div class="controls">
                                                                        <div class="input-prepend">
                                                                            <span class="add-on"><i class="icon-calendar"></i></span>
                                                                            <?=month_select_box()?>
                                                                        </div>
                                                                    </div></td>
                                                            </tr>
                                                            <tr class="timeHelpers period1">
                                                                <td><div class='label label-inverse'><?=$lang->getLang("Choose week")?></div></td>
                                                                <td>
                                                                    <div class="controls">
                                                                        <div class="input-prepend">
                                                                            <span class="add-on"><i class="icon-calendar"></i></span>
                                                                            <?=week_select_box()?>
                                                                        </div>
                                                                    </div>
                                                                </td>
                                                            </tr>

                                                            <tr>
                                                                <td><div class='label label-inverse'><?=$lang->getLang("From")?></div></td>
                                                                <td><div class="controls">
                                                                        <div class="input-prepend">
                                                                            <span class="add-on"><i class="icon-calendar"></i></span>
                                                                            <input type='text' name='date_start' id='date_start_err' class="dateStartHelper"  value="<?=$date_start?>" >
                                                                        </div>
                                                                    </div>
                                                                </td>
                                                            </tr>
                                                            <tr>
                                                                <td><div class='label label-inverse'><?=$lang->getLang("Until")?></div></td>
                                                                <td><div class="controls">
                                                                        <div class="input-prepend">
                                                                            <span class="add-on"><i class="icon-calendar"></i></span>
                                                                            <input type='text' name='date_end' id='date_end_err' class="dateEndHelper" value="<?=$date_end?>">
                                                                        </div>
                                                                    </div>
                                                                </td>
                                                            </tr>
                                                            </tbody>
                                                        </table>
                                                    </div>
                                                </div>
                                                <div class="row">
                                                    <span class="span2"> <button class="btn btn-primary" ><?=$lang->getLang("View")?></button></span>
                                                </div><br />
                                            </form>
                                            <br />
                                            <div id='duration' class="tip fleft"></div>
                                            <br />
                                        </div>
                                    </td>
                                </tr>
                                <tr>
                                    <td>
                                        <table id="lottosErrorsTable" class="display table table-striped table-hover table-bordered table-condensed" >
                                            <thead>
                                            <tr>
                                                <th><?=$lang->getLang('Lottery name')?></th>
                                                <th><?=$lang->getLang('Country')?></th>
                                                <th><?=$lang->getLang('Time')?></th>
                                                <th><?=$lang->getLang('Error Description')?></th>
                                            </tr>
                                            </thead>
                                            <tbody>
                                            <tr>
                                                <td></td>
                                                <td></td>
                                                <td class="unwrappable"></td>
                                                <td></td>
                                            </tr>
                                            </tbody>
                                        </table>
                                    </td>

                                </tr>
                            </table>

                        </div>
                        <? } ?>
                        <div class="tab-pane <?=($active=='lottoIncoming'? 'active':'')?>" id="lottoIncoming">
                            <table>
                                <tr>
                                    <td>
                                        <div class="well">
                                            <form action="/lotto_results" method="post">
                                                <input name="active" type='hidden' value="lottoIncoming">
                                                <div class="row">
                                                    <div class="span5"><div class='label label-inverse'><?=$lang->getLang("Lottery")?></div>
                                                        <br>
                                                        <div class="controls">
                                                            <div class="input-prepend">
                                                                <span class="add-on"><i class="icon-random"></i></span>
                                                                <select class="lotteries form-control" name="lotteries[]" id="lotteriesInc" multiple="multiple" >
                                                                    <?foreach($lottoCountries as $countries=>$lotto){ ?>
                                                                    <optgroup label="<?=$countries?>" value="<?=$countries?>"> <?
                                                                        foreach($lotto as $k=>$v){ ?>
                                                                            <option <?=((in_array($k,$_POST['lotteries']) || empty($_POST['lotteries']))?"selected" :"")?>  value="<?=$k?>" ><?=$v?></option>
                                                                            <?
                                                                        }
                                                                        } ?>
                                                                </select>
                                                            </div>
                                                        </div>
                                                    </div>
                                                </div>
                                                <div class="row"><br /><br />
                                                    <span class="span2"> <button class="btn btn-primary" ><?=$lang->getLang("View")?></button></span>
                                                </div><br />
                                            </form>
                                            <br />
                                            <div id='duration' class="tip fleft"></div>
                                            <br />
                                        </div>
                                    </td>
                                </tr>
                                <tr>
                                    <td>
                                        <table id="lottosIncomingTable" class="display table table-striped table-hover table-bordered table-condensed" >
                                            <thead>
                                            <tr>
                                                <th><?=$lang->getLang("Name")?></th>
                                                <th><?=$lang->getLang("Code")?></th>
                                                <th><?=$lang->getLang("Country")?></th>
                                                <th><?=$lang->getLang("Site")?></th>
                                                <th><?=$lang->getLang("Time until next extraction")?></th>
                                            </tr>
                                            </thead>
                                            <tbody>
                                            <tr>
                                                <td></td>
                                                <td></td>
                                                <td></td>
                                                <td></td>
                                                <td></td>
                                            </tr>
                                            </tbody>
                                        </table>
                                    </td>

                                </tr>
                            </table>

                            </div>
                    </div>
                </div>
            </td>
        </tr>
    </table>
</div>
<?
function createLotteriesTree($lotteries){
    foreach($lotteries as $key => $value){
        if(is_array($value)){ ?>
            <li data-jstree='{ "type" : "district" }' class="jstree-closed disabled district" id="<?=$key?>"><?=strtoupper($key)." <small>( ".count($value)." ) </small>"?><ul><?php
            createLotteriesTree($value); ?>
            </ul></li><?php
        }else{ ?>
            <li data-jstree='{ "type" : "club" }' class="club" id="<?=$key?>"><?=$value?> - <?=$key?></li><?php }

    }
}