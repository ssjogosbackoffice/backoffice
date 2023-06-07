<?php check_access('lotto_casino_view_stats', true);
/**
 * @author Marian
 */
include_once 'JurisdictionsList.class.inc';
$date_start = (isset($_POST['date_start']) &&$_POST['date_start']!='-1') ? $_POST['date_start'] : date("Y-m-d", time() - (60 * 60 * 24));
$date_end = (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d", time());
$type = (isset($_POST['type']) ? $_POST['type'] : 'd' );
if($type=='m'){
    $date_start=$_POST['monthHelperStart']."-01";
    $date_end=date('Y-m-t',strtotime($_POST['monthHelperEnd']));
}
$datetime1 = new DateTime($date_start);
$datetime2 = new DateTime($date_end);
$interval = $datetime1->diff($datetime2);
$lotteries=getAllLotteries();
$active = (isset($_POST['active']) ? $_POST['active'] : 'general' );
$sortyBy = (isset($_POST['sortBy']) ? $_POST['sortBy'] : '0' );



?>

    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
    <style>
        .searchTable {
            background-color: rgba(0, 0, 0, 0) !important;
            width: 440px !important;
            margin:auto;
            margin-bottom: 30px;
        }
    </style>
    <script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/highcharts.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/modules/exporting.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/jstree/jstree.js" type="text/javascript"></script>
    <script type="text/javascript">

    $(function () {
        $('.periodType').trigger('change');
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
                /*         "checkbox" : {
                 "three_state":false
                 },*/
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

        var lottery=function(code){
            this.code=code;
            this.setCode=function(code){
                this.code=code;
            };
            this.updateDuration=function(duration){
                $("#duration").html('<?=$lang->getLang("Time taken to execute your request")?>:'+ duration/1000+" s");
            };
            this.getLotteryStats=function(){
                startTime=new Date;
                $this=this;
                $.post("/services/lotto_services.php",{
                    pk:"getLotteryDetails",
                    code:this.code,
                    startDate:$(".dateStartHelper").val(),
                    endDate:$(".dateEndHelper").val()
                },function(resp){
                    var formatteddata=[];
                    var categories=[];
                    endTime = new Date;
                    totalTime = endTime - startTime;
                    $this.updateDuration(totalTime);
                    resp=JSON.parse(resp);
                    if(jQuery.isEmptyObject(resp)){
                        $('#result').html("<div class='alert alert-error'><strong><?=$lang->getLang("No result found")?>!</strong></div>");
                    }
                    else {
                        var dataSum = 0;
                        dataSorted=[];
                        $.each(resp, function (k, v) {
                            dataSum += v;
                            dataSorted.push([k, v])
                        });
                        dataSorted.sort(function(a, b){
                            return a[$('#sortBy').val()] - b[$('#sortBy').val()]
                        });
                        for (var key in dataSorted) {
                            formatteddata.push(dataSorted[key][1]);
                            categories.push(dataSorted[key][0]);

                        }

                        $('#result').empty().highcharts({
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
                                categories:categories
                            },
                            yAxis: {
                                min: 0,
                                title: {
                                    text: 'Percent Extracted'
                                },
                                labels: {
                                    formatter: function () {
                                        var pcnt = (this.value / dataSum) * 100;
                                        return Highcharts.numberFormat(pcnt, 2, ',') + '%';
                                    }
                                }
                            },
                            tooltip: {
                                pointFormat: '<span style="color:{series.color};padding:0">' +
                                '<b>{point.y} times </b></span>',
                                useHTML: true
                            },
                            plotOptions: {
                                bar: {
                                    pointPadding:0,
                                    pointWidth:12,
                                    dataLabels: {
                                        enabled: true,
                                        formatter: function () {
                                            var pcnt = (this.y / dataSum) * 100;
                                            return this.x + " : "+this.y+' times';
                                            /*return Highcharts.numberFormat(pcnt) + '%';*/
                                        }
                                    }
                                }
                            },
                            legend: {
                                layout: 'vertical',
                                align: 'right',
                                verticalAlign: 'top',
                                x: -20,
                                y: 60,
                                floating: true,
                                borderWidth: 1,
                                backgroundColor: ((Highcharts.theme && Highcharts.theme.legendBackgroundColor) || '#FFFFFF'),
                                shadow: true,
                                reversed: true,
                                title:"Extracted numbers"
                            },
                            series: [{ data: formatteddata}]
                        });
                    }
                });
            };
        };

        lotto=new lottery($('#jurisdictionTree').jstree("get_selected"));
          $('#getLotteryStats').on('click',function(){
             lotto.setCode($('#jurisdictionTree').jstree("get_selected"));
             lotto.getLotteryStats();
          });


        $( ".dateStartHelper" ).datepicker({
            changeMonth: true,
            changeYear: true,
            numberOfMonths:1,
            onSelect: function( selectedDate ) {
                $( ".monthHelper, .weekHelper" ).val('');
            }
        });
        $( ".dateEndHelper" ).datepicker({
            changeMonth: true,
            changeYear: true,
            numberOfMonths:1,
            onSelect: function( selectedDate ) {
                $( ".monthHelper, .weekHelper" ).val('');
            }
        });

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
    /**
     * Dark theme for Highcharts JS
     * @author Torstein Honsi
     */

// Load the fonts
    Highcharts.createElement('link', {
        href: '//fonts.googleapis.com/css?family=Unica+One',
        rel: 'stylesheet',
        type: 'text/css'
    }, null, document.getElementsByTagName('head')[0]);

    Highcharts.theme = {
        colors: ["#2b908f", "#90ee7e", "#f45b5b", "#7798BF", "#aaeeee", "#ff0066", "#eeaaee",
            "#55BF3B", "#DF5353", "#7798BF", "#aaeeee"],
        chart: {
            backgroundColor: {
                linearGradient: { x1: 0, y1: 0, x2: 1, y2: 1 },
                stops: [
                    [0, '#2a2a2b'],
                    [1, '#3e3e40']
                ]
            },
            borderRadius:5,
            style: {
                fontFamily: "'Unica One', sans-serif"
            },
            plotBorderColor: '#606063'
        },
        title: {
            style: {
                color: '#E0E0E3',
                textTransform: 'uppercase',
                fontSize: '20px'
            }
        },
        subtitle: {
            style: {
                color: '#E0E0E3',
                textTransform: 'uppercase'
            }
        },
        xAxis: {
            gridLineColor: '#707073',
            labels: {
                style: {
                    color: '#E0E0E3'
                }
            },
            lineColor: '#707073',
            minorGridLineColor: '#505053',
            tickColor: '#707073',
            title: {
                style: {
                    color: '#A0A0A3'

                }
            }
        },
        yAxis: {
            gridLineColor: '#707073',
            labels: {
                style: {
                    color: '#E0E0E3'
                }
            },
            lineColor: '#707073',
            minorGridLineColor: '#505053',
            tickColor: '#707073',
            tickWidth: 1,
            title: {
                style: {
                    color: '#A0A0A3'
                }
            }
        },
        tooltip: {
            backgroundColor: 'rgba(0, 0, 0, 0.85)',
            style: {
                color: '#F0F0F0'
            }
        },
        plotOptions: {
            series: {
                dataLabels: {
                    color: '#B0B0B3'
                },
                marker: {
                    lineColor: '#333'
                }
            },
            boxplot: {
                fillColor: '#505053'
            },
            candlestick: {
                lineColor: 'white'
            },
            errorbar: {
                color: 'white'
            }
        },
        legend: {
            itemStyle: {
                color: '#E0E0E3'
            },
            itemHoverStyle: {
                color: '#FFF'
            },
            itemHiddenStyle: {
                color: '#606063'
            }
        },
        credits: {
            style: {
                color: '#666'
            }
        },
        labels: {
            style: {
                color: '#707073'
            }
        },

        drilldown: {
            activeAxisLabelStyle: {
                color: '#F0F0F3'
            },
            activeDataLabelStyle: {
                color: '#F0F0F3'
            }
        },

        navigation: {
            buttonOptions: {
                symbolStroke: '#DDDDDD',
                theme: {
                    fill: '#505053'
                }
            }
        },

        // scroll charts
        rangeSelector: {
            buttonTheme: {
                fill: '#505053',
                stroke: '#000000',
                style: {
                    color: '#CCC'
                },
                states: {
                    hover: {
                        fill: '#707073',
                        stroke: '#000000',
                        style: {
                            color: 'white'
                        }
                    },
                    select: {
                        fill: '#000003',
                        stroke: '#000000',
                        style: {
                            color: 'white'
                        }
                    }
                }
            },
            inputBoxBorderColor: '#505053',
            inputStyle: {
                backgroundColor: '#333',
                color: 'silver'
            },
            labelStyle: {
                color: 'silver'
            }
        },

        navigator: {
            handles: {
                backgroundColor: '#666',
                borderColor: '#AAA'
            },
            outlineColor: '#CCC',
            maskFill: 'rgba(255,255,255,0.1)',
            series: {
                color: '#7798BF',
                lineColor: '#A6C7ED'
            },
            xAxis: {
                gridLineColor: '#505053'
            }
        },

        scrollbar: {
            barBackgroundColor: '#808083',
            barBorderColor: '#808083',
            buttonArrowColor: '#CCC',
            buttonBackgroundColor: '#606063',
            buttonBorderColor: '#606063',
            rifleColor: '#FFF',
            trackBackgroundColor: '#404043',
            trackBorderColor: '#404043'
        },

        // special colors for some of the
        legendBackgroundColor: 'rgba(0, 0, 0, 0.5)',
        background2: '#505053',
        dataLabelsColor: '#B0B0B3',
        textColor: '#C0C0C0',
        contrastTextColor: '#F0F0F3',
        maskColor: 'rgba(255,255,255,0.3)'
    };

    // Apply the theme
    Highcharts.setOptions(Highcharts.theme);
    // Apply the theme


    </script>


    <div class="container" style="position: absolute;left:0;right: 0;width: 100%" >
        <table class="table table-bordered">
            <tr>
                <td class="navbar-inner" colspan="2">
                    <h3 class="text-center"><?=$lang->getLang("Lottery Statistics")?></h3>
                </td>
            </tr>
            <tr>
                <td class="span4">
                    <div class="contentAdmin">
                        <div class="contentHeadAdmin"><?=$lang->getLang("Lottery chooser")?></div>
                        <div class="well">
                            <input type="text" id='jurisdictionTreeSearch' placeholder="<?=$lang->getLang("Search a lottery")?>">
                            <div id="jurisdictionTree">
                                <ul>
                                    <?=createLotteriesTree($lotteries);?>
                                </ul>
                            </div>
                        </div>
                    </div>
                </td>
                <td style='vertical-align: top'>
                    <div class="well">
                        <div class="tabbable">
                            <ul class="nav nav-tabs">
                                <li class="<?=($active=='general'? 'active':'')?>"><a href="#generalStats" data-toggle="tab"><button class="btn btn-inverse" id="general"><?=$lang->getLang("Lotto stats")?></button></a></li>
                            </ul>
                            <div class="tab-content">
                                <div class="tab-pane <?=($active=='general'? 'active':'')?>" id="generalStats">
                                    <div class="tabbable  tabs-left">
                                    <div class="row"><div class="span7">
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
                                                <tr>
                                                    <td><div class='label label-inverse'><?=$lang->getLang("Sort by")?></div></td>
                                                    <td> <div class="controls">
                                                            <div class="input-prepend">
                                                                <span class="add-on"><i class="icon-random"></i></span>
                                                                <select class="form-control" name="sortBy" id="sortBy" >
                                                                    <option <?=($sortyBy==0 ? "selected":"")?>  value="0" ><?=$lang->getLang("Numbers")?></option>
                                                                    <option <?=($missing==1 ? "selected":"")?>  value="1" ><?=$lang->getLang("Most Extracted")?></option>
                                                                </select>
                                                            </div>
                                                        </div>
                                                    </td>
                                                </tr>
                                                </tbody>
                                            </table>
                                            <button class="btn btn-danger" id="getLotteryStats">Show Stats</button>
                                            <br />
                                            <br />
                                            <div id='duration' class="tip fleft"></div>
                                            <br />
                                        </div></div>
                                        <div class="container-fluid">
                                        <div class="row">
                                            <br/>
                                            <div id="ballsContainer">



                                            </div>
                                            <div id="result" style="max-width: 100%;height: 1400px"></div>
                                        </div>
                                            </div>
                                    </div>
                                </div>
                            </div>
                        </div>
                </td>
            </tr>
        </table>
    </div>
<?
function getAllLotteries(){
    global $dbh;
    $sql="SELECT gls_code,gls_cou_code,gls_name from game_lotto group by gls_code";
    $results=$dbh->doCachedQuery($sql,60);
    $lottoCountries=array();

    $allCountries=$dbh->doCachedQuery("SELECT cou_code,cou_name from country",24*3600*365);
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

    return $lottoCountries;
}


function createLotteriesTree($lotteries){
    foreach($lotteries as $key => $value){
        if(is_array($value)){ ?>
                <li data-jstree='{ "type" : "district" }' class="jstree-closed district" id="<?=$key?>"><?=strtoupper($key)?><ul><?php
            createLotteriesTree($value); ?>
                </ul></li><?php
        }else{ ?>
                <li data-jstree='{ "type" : "club" }' class="club" id="<?=$key?>"><?=$value?>-<?=($key)?></li><?php }

    }
}