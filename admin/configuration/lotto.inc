<?php check_access('lotto_manage', true);
$allClubs=getAllClubsUnderJurisdiction($_SESSION['jurisdiction_id']);
$status = (isset($_POST["status"])) ? $_POST["status"] : 2 ;
$verified = (isset($_POST["verified"])) ? $_POST["verified"] : "0" ;
$type = (isset($_POST["type"])) ? $_POST["type"] : 2 ;
$allCountries=$dbh->doCachedQuery("SELECT cou_code,cou_name from country",24*3600*365);
$allCountries=$allCountries->toArray();
$allCountries=formatCountry($allCountries);
$days=array(1=>"Monday",
    2=>"Tuesday",
    3=>"Wednesday",
    4=>"Thursday",
    5=>"Friday",
    6=>"Saturday",
    7=>"Sunday");

$userCanModify=check_access('lotto_games_management');
?>
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />

    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="/media/bootstrap/css/bootstrap-multiselect.css" >
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/bootstrap-editable/bootstrap-editable/css/bootstrap-editable.css" >
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media//select2/select2.css" >
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/FlipClock/compiled/flipclock.css" >

    <script src="/media/jquery.dataTables.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/bootstrap/bootstrap-editable/bootstrap-editable/js/bootstrap-editable.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/select2/select2.js" type="text/javascript"></script>
    <script src="/media/bootstrap/js/bootstrap-multiselect.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/highcharts.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/highcharts-3d.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/modules/exporting.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/FlipClock/compiled/flipclock.js" type="text/javascript"></script>

    <style>
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
    </style>
    <script type="text/javascript">
    var startTime=null;
    var endTime=null;
  /*  $(document).on('click','.showLotteryDetails',function(){
        getLotteryDetails($(this).closest('tr').find('.lotteryCode').html());

    });*/
    $("body").on({
        ajaxStart: function() {
            $('#loadingDiv').show();
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
    var servicesUrl="/services/lotto_services.php";
    $(function(){

        $('#addLotteryConfigBalls').on('change',function(){
           var maximumQuotes=$(this).val();
            if(maximumQuotes>10){
                maximumQuotes=10;
            }
            $('#addLotteryOdds').empty();
            for (var i=0;i < maximumQuotes;i++) {
               var html=$("<input type='text' class='quoteInput' style='width:55px;float:left;margin-right: 5px'>");
                $('#addLotteryOdds').append(html);

            }

        });
        $('#country,#addCountry').multiselect({
            enableFiltering: true,
            maxHeight: 300,
            multiple:true,
            includeSelectAllOption: true,
            enableCaseInsensitiveFiltering:true
        });
        availableCountries=<?=json_encode($allCountries)?>;
        countries=[];
        $.each(availableCountries, function(k, v) {
            countries.push({
                id: k,
                text: v
            });
        });
        $('#eDays,#addExtractionDays').multiselect({
            maxHeight: 300,
            multiple:true,
            includeSelectAllOption: true,
            allSelectedText: 'All week'
        });
        var table = $('.display').DataTable({
            columnDefs: [
                {
                    targets: 1 ,
                    className:'lotteryCode'
                },
            {
                targets: 9 ,
                className:'unwrappable'
            }

                
            //  ,  { "targets": 9, "className":'unwrappable' }
            ],
            "processing": true,
            "serverSide": true,
            "ajax":{
                "url": servicesUrl,
                "data": function ( d ) {
                    d.pk = "getLotteries";
                    d.country= JSON.stringify($("#country").val());
                    d.extractionDays=JSON.stringify($("#eDays").val());
                    d.status=$("#status").val();
                    d.type=$("#type").val();
                    d.verified=$("#verified").val();
                }
            },
            "fnDrawCallback": function (nRow, aData, iDisplayIndex, iDisplayIndexFull) {
                <?if ($userCanModify) { ?>
                $('.editableSiteClass').editable({
                    url:'/services/lotto_services.php',
                    send:'always',
                    pk:'updateSiteUrl',
                    params: function(params) {
                        params.code = $(this).closest('tr').find('.lotteryCode').html();
                        return params;
                    },
                    success: function(response, newValue) {
                    },
                    error: function(response, newValue) {
                        jAlert('An error has occured');
                    }
                });
                $('.editableExtractionType').editable({
                    url:'/services/lotto_services.php',
                    send:'always',
                    pk:'updateExtractionType',
                    params: function(params) {
                        params.code = $(this).closest('tr').find('.lotteryCode').html();
                        return params;
                    },
                    success: function(response, newValue) {
                    },
                    error: function(response, newValue) {
                        jAlert('An error has occured');
                    }
                });
                $('.country').editable({
                    source: countries,
                    url:'/services/lotto_services.php',
                    send:'always',
                    pk:'updateCountries',
                    select2: {
                        width: 200,
                        placeholder: 'Select country',
                        allowClear: true
                    },
                    params: function(params) {
                        params.code = $(this).closest('tr').find('.lotteryCode').html();
                        return params;
                    },
                    success: function(response, newValue) {
                    },
                    error: function(response, newValue) {
                        jAlert('An error has occured');
                    }
                });
                $('.daysSelect').editable({
                    url:'/services/lotto_services.php',
                    send:'always',
                    pk:'updateDays',
                    separator:",",
                    source: [{
                        value: 1,
                        text: 'Monday'
                    }, {
                        value: 2,
                        text: 'Tuesday'
                    }, {
                        value: 3,
                        text: 'Wednesday'
                    }, {
                        value: 4,
                        text: 'Thursday'
                    }, {
                        value: 5,
                        text: 'Friday'
                    },
                        {
                            value: 6,
                            text: 'Saturday'
                        },
                        {
                            value: 7,
                            text: 'Sunday'
                        }],
                    params: function(params) {
                        params.code = $(this).closest('tr').find('.lotteryCode').html();
                        return params;
                    },
                    success: function(response, newValue) {
                    },
                    error: function(response, newValue) {
                        jAlert('An error has occured');
                    }
                });
                $('.simpleTypeSelect').editable({
                    url:'/services/lotto_services.php',
                    send:'always',
                    pk:'updateType',
                    source: [
                        {value: 0, text: "Lotto"}, {value: 2, text: "Keno"}
                    ],
                    params: function(params) {
                        params.code = $(this).closest('tr').find('.lotteryCode').html();
                        return params;
                    },
                    success: function(response, newValue) {
                    },
                    error: function(response, newValue) {
                        jAlert('An error has occured');
                    }
                });
                $('.statusEditableSelect').editable({
                    url:'/services/lotto_services.php',
                    send:'always',
                    pk:'updateStatus',
                    escape:false,
                    source: [
                        {value: 1, text: "Enabled"},{value: 0, text: "Disabled"}
                    ],
                    params: function(params) {
                        params.code = $(this).closest('tr').find('.lotteryCode').html();
                        return params;
                    },
                    success: function(response, newValue) {
                        if(newValue==1){
                            $(this).addClass('text-success').removeClass('text-error');
                        }
                        else{

                            $(this).addClass('text-error').removeClass('text-success');
                        }
                    },
                    error: function(response, newValue) {
                        jAlert('An error has occured');
                    }
                });
                $('.editableOddsClass').editable({
                    selector: 'a',
                    url:'/services/lotto_services.php',
                    send:'always',
                    pk:'updateOdds',
                    params: function(params) {
                        params.code = $(this).closest('tr').find('.lotteryCode').html();
                        return params;
                    },
                    success: function(response, newValue) {
                    },
                    error: function(response, newValue) {
                        jAlert('An error has occured');
                    }
                });
                $('.editableConfigClass').editable({
                    selector: 'a',
                    url:'/services/lotto_services.php',
                    send:'always',
               /*     value:function(){
                        var quotes=$(this).closest('td').find('.editableConfigClass').html();
                        quotes=quotes.split(";");
                        var  newValues={};
                        for (i = 0; i < quotes.length; i++) {
                            newValues[i]=quotes[i];
                        }
                            return [newValues];
                    },*/
                   /* value:{
                        plm:"15",
                        plm2:"30",
                        plm3:"ss"
                    },
                    display: function(value) {
                        if (!value) {
                            $(this).empty();
                            return;
                        }
                        var html = '<b>' + $('<div>').text(value.plm).html() + '</b>, ' + $('<div>').text(value.plm).html() + ' st., bld. ' + $('<div>').text(value.plm).html();
                        $(this).html(html);
                    },*/

                    pk:'updateConfiguration',
                    params: function(params) {
                        params.code = $(this).closest('tr').find('.lotteryCode').html();
                        return params;
                    },
                    success: function(response, newValue) {
                    },
                    error: function(response, newValue) {
                        jAlert('An error has occured');
                    }
                });
                <? } else{ ?>
                $('.country').each(function(){
                    $(this).html($(this).data('value'));
                });
                <? } ?>

       /*         counter=0;
                $clock=[];
                $('.countdownContainer').each(function(){
                    var $_self=$(this);
                    var time=$(this).html();
                    if(time!='') {
                    /!*    var date = new Date(time);
                        var now = new Date();
                        var diff = (date.getTime() / 1000) - (now.getTime() / 1000);
                        console.log(diff)*!/
                        var clockFace=(time<=24*3600 ? 'HourlyCounter' : 'DailyCounter');
                        $clock[counter] = $(this).FlipClock(time, {
                            countdown: true,
                            clockFace: clockFace,
                            callbacks:{
                                stop:function() {
                                    $_self.fadeOut(function() {
                                        $(this).html('<span class="text-success">Waiting for result...</span>')
                                    }).fadeIn();
                                }
                            }
                        });
                    }
                });*/
            },
            'iDisplayLength': 10,'sPaginationType':"full_numbers"
        });


        $('#modalForm').on('submit',function(e) {
            e.preventDefault();
            var lottoName = $('#addLotteryName').val();
            var lottoCode = $('#addLotteryCode').val();
            var lottoCountry=$('#addCountry').val();
            var lottoSiteUrl=$('#addLotterySite').val();
            var lottoExtractionDays = $('#addExtractionDays').val();
            var lottoExtractionNumbers=$('#addExtractionNumbers').val();
            var lottoType = $('#addLotteryType').val();
            var lottoConfig = $('#addLotteryConfigBalls').val()+";"+$('#addLotteryConfigBallsChosen').val()+";"+$('#addLotteryConfigMinMax').val();
            var lottoOdds='';
            $('.quoteInput').each(function(i, el) {
                if(lottoOdds==''){
                    lottoOdds+=$(el).val();
                }
                else{
                    lottoOdds +=";"+ $(el).val();
                }
            });

            var lottoStatus = $('#addLotteryStatus').val();
            $.post("/services/lotto_services.php",{
                pk:"addNewLotto",
                lottoName:lottoName,
                lottoCode:lottoCode,
                lottoCountry:lottoCountry,
                lottoSiteUrl:lottoSiteUrl,
                lottoExtractionDays:lottoExtractionDays,
                lottoType:lottoType,
                lottoOdds:lottoOdds,
                lottoConfig:lottoConfig,
                lottoExtractionNumbers:lottoExtractionNumbers,
                lottoStatus:lottoStatus
            },function(data){
                if(data==1){
                    jAlert('<?=$lang->getLang("Lottery successfully added")?>','Success');
                }
                else {
                    jAlert(data);
                }
            });
        });
    });


    function getLotteryDetails(lottoCode){
        $.post("/services/lotto_services.php",{
            pk:"getLotteryDetails",
            code:lottoCode
        },function(data){
            var formatteddata=[];
            data=JSON.parse(data);
            var dataSum = 0;
            $.each(data, function(k, v) {
                dataSum += v;
                formatteddata.push({
                    name: k,
                    data: [v]
                })
            });
            $('#delResult').empty().highcharts({
                chart: {
                    type: 'bar'
                },
                title: {
                    text: 'Most extracted numbers'
                },
                subtitle: {
                    text: ''
                },
                credits: {
                    enabled: false
                },
                xAxis: {
                    categories: [
                        'Extracted Numbers'
                    ],
                    crosshair: true

                },
                yAxis: {
                    min: 0,
                    title: {
                        text: 'Times Extracted'
                    },
                    labels: {
                        formatter:function() {
                            var pcnt = (this.value / dataSum) * 100;
                            return Highcharts.numberFormat(pcnt,0,',') + '%';
                        }
                    }
                },
                tooltip: {
                    pointFormat: '<span style="color:{series.color};padding:0">{series.name}: ' +
                        '<b>{point.y} times </b></span>',
                    useHTML: true
                },
                plotOptions: {
                    bar: {
                        dataLabels: {
                            enabled: true,
                            formatter:function() {
                                var pcnt = (this.y / dataSum) * 100;
                                return Highcharts.numberFormat(pcnt) + '%';
                            }

                        }
                    }
                },
                legend: {
                    layout: 'vertical',
                    align: 'right',
                    verticalAlign: 'top',
                    x: -40,
                    y: 80,
                    floating: true,
                    borderWidth: 1,
                    backgroundColor: ((Highcharts.theme && Highcharts.theme.legendBackgroundColor) || '#FFFFFF'),
                    shadow: true
                },
                series: formatteddata
            });
        });
    }
    </script>
<?php
/**
 *
 * @author Marian
 */

?>
    <div id='loadingDiv' class="centered"><img src="<?=image_dir?>/ajax-loader.gif" /></div>
    <form id="modalForm">
        <div id="myModal" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
                <h3 class="text-info text-center">Add a lottery<span id="myModalLabel"></span></h3> <br />
            </div>
            <div class="modal-body">
                <p>
                <table class='table table-condensed '>
                    <tr><td class='blackTd'>Name</td><td><input type="text" name='addLotteryName' id='addLotteryName' value='' required="required"></td></tr>
                    <tr><td class='blackTd'>Code</td><td><input type="text" name='addLotteryCode' id='addLotteryCode' value='' required="required"></td></tr>
                    <tr><td class="blackTd"><?=$lang->getLang("Country")?></td>
                        <td>
                            <?=countrySelect('addCountry','',$_POST['addCountry'],false,'addCountry',true,false)?>
                        </td>
                    </tr>
                    <tr><td class="blackTd">Site</td>
                        <td><input type="text" id="addLotterySite" name='addLotterySite' value=''>
                        </td>
                    </tr>
                    <tr><td class='blackTd'><?=$lang->getLang("Extraction days")?></td><td>
                            <select name='addExtractionDays[]' id="addExtractionDays" multiple="multiple">
                                <option value='1'><?=$lang->getLang("Monday")?></option>
                                <option value='2'><?=$lang->getLang("Tuesday")?></option>
                                <option value='3'><?=$lang->getLang("Wednesday")?></option>
                                <option value='4'><?=$lang->getLang("Thursday")?></option>
                                <option value='5'><?=$lang->getLang("Friday")?></option>
                                <option value='6'><?=$lang->getLang("Saturday")?></option>
                                <option value='7'><?=$lang->getLang("Sunday")?></option>
                            </select>
                        </td>
                    </tr>
                    <tr><td class="blackTd">Extraction numbers</td>
                        <td><input type="text" id="addExtractionNumbers" name='addExtractionNumbers' value='' placeholder="">
                        </td>
                    </tr>
                    <tr><td class='blackTd'><?=$lang->getLang("Type")?></td><td>
                            <select name='addLotteryType' id="addLotteryType">
                                <option value='0'><?=$lang->getLang("Lotto")?></option>
                                <option value='1'><?=$lang->getLang("Keno")?></option>
                            </select>
                        </td></tr>
                    <tr><td class="blackTd">Config</td>
                        <td>
                            <table>
                                <tr><td>Balls extracted</td><td><input required type="number" max="20" maxlength="2" id="addLotteryConfigBalls" style="width: 50px;margin-bottom: 0" name='addLotteryConfig' value=''  ></td></tr>
                                <tr><td>Balls chosen</td><td><input required type="text" id="addLotteryConfigBallsChosen" style="width: 60px;margin-bottom: 0" name='addLotteryConfig' value='' placeholder="Min-max" ></td></tr>
                                <tr><td>Min-max balls</td><td><input required type="text" id="addLotteryConfigMinMax" style="width: 60px;margin-bottom: 0" name='addLotteryConfig' value='' placeholder="Min-max"></td></tr>
                            </table>
                        </td>
                    </tr>
                    <tr><td class="blackTd">Odds</td>
                        <td  id="addLotteryOdds">Please fill the field "Balls extracted" first </td>
                    </tr>


                    <tr><td class='blackTd'><?=$lang->getLang("Status")?></td><td>
                            <select name='addLotteryStatus' id="addLotteryStatus">
                                <option value='1'><?=$lang->getLang("Enabled")?></option>
                                <option value='0'><?=$lang->getLang("Disabled")?></option>
                            </select>
                        </td></tr>
                </table>
                </p>
            </div>
            <div class="modal-footer">
                <button class="btn" data-dismiss="modal" aria-hidden="true"><?=$lang->getLang("Cancel")?></button>
                <button class="btn btn-primary" id="sendModal" name="save"><?=$lang->getLang("Add")?></button>
            </div>
        </div>
    </form>
    <table class="table table-bordered">
        <tr>
            <td class="navbar-inner" colspan="2"  ><h3 class="text-center"><?=$lang->getLang("Lotteries management")?></h3></td>
        </tr>
        <tr>
            <td colspan="2">
                <div class="well">
                    <form action="/lotto_management" method="post">
                        <div class="row">
                            <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Country")?></div>
                                <br />
                                <div class="controls">
                                    <div class="input-prepend">
                                        <span class="add-on"><i class="icon-calendar"></i> </span>
                                        <?=countrySelect('country','',$_POST['country'],false,'country',true,true)?>
                                    </div>
                                </div>
                            </div>
                            <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Extraction days")?></div>
                                <br />
                                <div class="controls">
                                    <div class="input-prepend">
                                        <span class="add-on"><i class="icon-calendar"></i> </span>
                                        <select name='eDays[]' id="eDays" multiple="multiple">
                                            <option value='1' <?=(in_array('1',$_POST['eDays'])|| !isset($_POST['eDays']) ? "selected='selected'":"")?>><?=$lang->getLang("Monday")?></option>
                                            <option value='2' <?=(in_array('2',$_POST['eDays'])|| !isset($_POST['eDays']) ? "selected='selected'":"")?>><?=$lang->getLang("Tuesday")?></option>
                                            <option value='3' <?=(in_array('3',$_POST['eDays'])|| !isset($_POST['eDays'])? "selected='selected'":"")?>><?=$lang->getLang("Wednesday")?></option>
                                            <option value='4' <?=(in_array('4',$_POST['eDays'])|| !isset($_POST['eDays']) ? "selected='selected'":"")?>><?=$lang->getLang("Thursday")?></option>
                                            <option value='5' <?=(in_array('5',$_POST['eDays'])|| !isset($_POST['eDays']) ? "selected='selected'":"")?>><?=$lang->getLang("Friday")?></option>
                                            <option value='6' <?=(in_array('6',$_POST['eDays'])|| !isset($_POST['eDays']) ? "selected='selected'":"")?>><?=$lang->getLang("Saturday")?></option>
                                            <option value='7' <?=(in_array('7',$_POST['eDays']) || !isset($_POST['eDays'])? "selected='selected'":"")?>><?=$lang->getLang("Sunday")?></option>
                                        </select>
                                    </div>
                                </div>
                            </div>
                            <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Type")?></div>
                                <br />
                                <div class="controls">
                                    <div class="input-prepend">
                                        <span class="add-on"><i class="icon-random"></i></span>
                                        <select name='type' id="type">
                                            <option value='0' <?=($type=='0') ? "selected='selected'":""?>><?=$lang->getLang("Lotto")?></option>
                                            <option value='1' <?=($type=='1') ? "selected='selected'":""?>><?=$lang->getLang("Keno")?></option>
                                            <option value='2' <?=($type=='2') ? "selected='selected'":""?>><?=$lang->getLang("All")?></option>
                                        </select>
                                    </div>
                                </div>
                            </div>
                            <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Status")?></div>
                                <br>
                                <div class="controls">
                                    <div class="input-prepend">
                                        <span class="add-on"><i class="icon-random"></i></span>
                                        <select name='status' id="status">
                                            <option value='2' <?=($status=='2') ? "selected='selected'":""?> ><?=$lang->getLang("All")?></option>
                                            <option value='1' <?=($status=='1') ? "selected='selected'":""?>><?=$lang->getLang("Enabled")?></option>
                                            <option value='0' <?=($status=='0') ? "selected='selected'":""?>><?=$lang->getLang("Disabled")?></option>
                                        </select>
                                    </div>
                                </div>
                            </div>
                            <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Verified")?></div>
                                <br>
                                <div class="controls">
                                    <div class="input-prepend">
                                        <span class="add-on"><i class="icon-random"></i></span>
                                        <select name='verified' id="verified">
                                            <option value='0' <?=($verified=='0') ? "selected='selected'":""?>><?=$lang->getLang("All")?></option>
                                            <option value='1' <?=($verified=='1') ? "selected='selected'":""?> class="text-success"><?=$lang->getLang("Verified")?></option>
                                            <option value='2' <?=($verified=='2') ? "selected='selected'":""?> class="text-error"><?=$lang->getLang("Failed")?></option>
                                        </select>
                                    </div>
                                </div>
                            </div>
                        </div>
                        <div class="row">
                            <br />
                            <span class="span2"> <button class="btn btn-primary"><?=$lang->getLang("Search")?></button></span>
                        </div><br />
                    </form>
                </div>
            </td>
        </tr>
        <tr>
            <td>
                <div style="width: 100%">

                    <a href="#myModal" class="addPartner fright btn btn-info btn-small" style="color:white" data-toggle="modal" ><?=$lang->getLang("Add a lottery")?></a> <br /><br /><br />
                    <table class='display table table-striped table-hover table-bordered table-condensed'>
                        <thead>
                        <tr>
                            <th><?=$lang->getLang("Name")?></th>
                            <th><?=$lang->getLang("Code")?></th>
                            <th><?=$lang->getLang("Country")?></th>
                            <th><?=$lang->getLang("Site")?></th>
                            <th><?=$lang->getLang("Extraction days")?></th>
                            <th><?=$lang->getLang("Extraction type")?></th>
                            <th><?=$lang->getLang("Type")?></th>
                            <th><?=$lang->getLang("Status")?></th>
                            <th><?=$lang->getLang("Odds")?></th>
                            <th><?=$lang->getLang("Configuration")?></th>
                            <th><?=$lang->getLang("Details")?></th>
                          <!--  <th><?/*=$lang->getLang("Time until next extraction")*/?></th>-->

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
                                <td></td>
                                <td></td>
<!--                                <td></td>-->

                               <!-- <td><?/*=$row['gls_name']*/?></td>
                                <td  class="lotteryCode"><?/*=$row['gls_code']*/?></td>
                                <td><a href="#" class="country" data-type="select2"  data-value="<?/*=$row['gls_cou_code']*/?>" data-title="Select country"></a></td>
                                <td><div class="editableSiteClass" data-title="Enter extraction site"><?/*=$row['gls_result_site_url']*/?></div></td>
                                <td><a href="#" data-id="<?/*=$row['gls_code']*/?>" class="daysSelect" data-type="checklist" data-value="<?/*=$row['gls_extraction_days']*/?>" data-title="Select days"  data-original-title="" title="" style="background-color: rgba(0, 0, 0, 0);"><?/*=daysReadable($row['gls_extraction_days'])*/?></a></td>
                                <td><a href="#" class="simpleTypeSelect" data-type="select"  data-title="Type"><?/*=($row['gls_type']==0? "Lotto":"Keno")*/?></a></td>
                                <td><div data-type="select"   data-title="Enable/disable" class='statusEditableSelect <?/*=($row['gls_status']==0? " text-error'> Disabled": " text-success'> Enabled")*/?></div></td>
    <td><a href="<?/*=$row['gls_result_site_url']*/?>" target="_blank"><button class='btn btn-small showLotteryDetails'>View site</button></a></td>
                                <td>
                                    <div id="clock<?/*=$count*/?>">
                                        <?/*=calculateTimeUntilExtraction($row['gls_extraction_days'],$row['gls_extraction_numbers'])*/?>
                                    </div>
                                     </td>
                                    -->

                                    <!--    <script>
                                    var time='<?/*=calculateTimeUntilExtraction($row['gls_extraction_numbers'])*/?>';
                                    if(time!='') {
                                        var date = new Date(time);
                                        var now = new Date();
                                        var diff = (date.getTime() / 1000) - (now.getTime() / 1000);
                                        var clock<?/*=$count*/?> = $('#clock<?/*=$count*/?>').FlipClock(diff, {
                                            countdown: true
                                        });
                                    }
                                </script>-->




                            </tr>

                        </tbody>
                    </table>
                </div>
            </td>
            <td style="vertical-align:top;">
                <div id="delResult" style="height:600px"></div>
            </td>
        </tr>
</table>