<?php
check_access("casino_games_activity",true);
$casino_stats_playing_users_access = check_access("casino_stats_players_playing");
/**
 * @remade Marian, @maintained Alexandra
 */
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

$jur =(isset($_GET['jur'])?$_GET['jur']: (isset($_REQUEST['jur']) ? substr($_REQUEST['jur'],1) : $_SESSION['jurisdiction_id'] ));
$cls = (isset($_REQUEST['cls']) ? $_REQUEST['cls'] : $_SESSION['jurisdiction_class'] );
$subSection=(isset($_POST['subsection']) ? $_POST['subsection'] :'all');
$active = (isset($_POST['active']) ? $_POST['active'] : 'general' );
$statType=(isset($_POST['statType']) ? $_POST['statType'] : 'graph' );
?>

    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/dataTablesExtra/TableTools-2.2.0/css/dataTables.tableTools.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree3/themes/default/style.css"  />
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
    <script src="<?= secure_host ?>/media/jstree3/jstree.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/jquery.dataTables.js" type="text/javascript"></script>
    <script type="text/javascript">
    $(function () {

        // Initializing the datatable
        playingUsrsTable = $('#userStats').DataTable({
            "processing": true,
            "serverSide": true,
            "ajax": {
                "url": "/services/general_services.inc",
                "data": function (d) {
                    d.action = "displayPlayingUsers";
                }
            },
            'sPaginationType': "full_numbers",
            "columnDefs": [
                {
                    "orderable": false, "targets": [ 3 ]
                }
            ]
        });


        /*        $(".toggleGraph").click(function(){
         var id=$(this).attr("id").substr(4);
         $("#"+id+",#"+id+"M").toggle("slow");
         $(this).toggleClass("btn-inverse");
         });
         $(".toggleAll").click(function(){
         $(".toggleGraph").trigger('click');
         });*/

        $("input:radio[name=type]").change(function(){
            $('.dateStartHelper').val('<?=$date_start?>');
            $('.dateEndHelper').val('<?=$date_end?>');
            if($(this).val()=='m'){
                $(this).closest('table').find('.dly').hide();
                $(this).closest('table').find('.mnt').show();
            }
            else{
                $(this).closest('table').find('.mnt').hide();
                $(this).closest('table').find('.dly').show();
            }

        });
        if($("input:radio[name=type]:checked").val()=='m'){
            $('.dly').hide();
            $('.mnt').show();
        }
        else{
            $('.mnt').hide();
            $('.dly').show();
        }


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

        $(document).on('change', '.monthHelper', function() {
            var d = new Date($(this).val());
            createMonthHelper(d);
        });
        $(document).on('change', '.weekHelper', function() {
            createWeekHelper($(this).val());
        });

        function createWeekHelper(dateobj){
            if (typeof dateobj !== 'undefined' || dateobj!=''){
                dates=dateobj.split('~');
                firstDay=dates[0];
                lastDay=dates[1];
                $( ".dateStartHelper" ).datepicker('setDate', firstDay);
                $( ".dateEndHelper" ).datepicker('setDate', lastDay);
                $('.monthHelper').val('');
            }
        }

        function createMonthHelper(dateobj){
            if (typeof dateobj !== 'undefined' || dateobj!=''){
                var firstDay = new Date(dateobj.getFullYear(), dateobj.getMonth(), 1);
                $( ".dateStartHelper" ).datepicker('setDate', firstDay);
                var lastDay = new Date(dateobj.getFullYear(), dateobj.getMonth() + 1, 0);
                $( ".dateEndHelper" ).datepicker('setDate', lastDay);
                $('.weekHelper').val('');
            }
        }

        $(document).on('click', '#deleteSelectedFromCache', function() {

            //console.log("clicked");
            var idList = [];
            if ($('input.deleteFromCache').is(':checked')) {

                //console.log("Is Checked");
                $('input:checkbox.deleteFromCache:checked').each(function () {
                    var id = $(this).attr('data-id');
                    // Add the id to the id list
                    idList.push(id);
                });
            }
            if(idList.length > 0) {
                deleteSelectedFromCache(idList);
                $('#printError').empty();
            } else {
                // Print error
                $('#printError').empty().html("Please select one or more users to delete");
            }

        });

        function deleteSelectedFromCache( idList) {
            $.ajax({
                url: "/services/general_services.inc",
                type: "POST",
                data: {
                    action: "deleteFromCache",
                    selectedId: idList
                }
            }).done(function (data){
              // reload the table
                playingUsrsTable.ajax.reload();
            })
        }

    $('#jurisdictionTreeSearch').keyup(function () {
            if(this.value.length>3){
                var start        = new Date().getTime();
                $.ajax({
                    url: "/services/jurisdiction_services.inc",
                    xhrFields: {
                        withCredentials: true
                    },
                    type: "GET",
                    dataType: "json",
                    data: {
                         action: "getJurisdictionsByName",
                         str: $('#jurisdictionTreeSearch').val()
                    }
                }).done(function (data) {
                        var res = "<ul>";
                        for (var jurisdictions in data) {
                            res += "<li><span class='title " + jurisdictions + "'>" + jurisdictions + "</span>";
                            res += "<ol class='" + jurisdictions + "'>";
                            for (var j = 0; j < data[jurisdictions].length; j++) {
                                res += "<li><span class='category'>" + jurisdictions + "</span> <a href='/games_stats?jur=" + data[jurisdictions][j]['id'] + "&cls="+jurisdictions+"'>" + data[jurisdictions][j]['name'] + "</a> ";
                                res += "</li>";
                            }
                            res += "</ol>";
                            res += "</li>";
                        }
                        res += "</ul>";
                        var end = new Date().getTime();

                        res = "<span class='tip'><?=$lang->getLang("Time taken to execute your request")?> " + ((end-start)/1000) + " s</span><br/>" + res;
                        $('#searchResult').html(res);
                    })
                    .always(function () {
                    });
            }
            else if  (this.value.length==0){
                $('#searchResult').empty();
            }
        });
  /*      $("#jurisdictionTree")
            .jstree({
                core:{"multiple":false

                },
                "plugins" : [
                    "search",
                    "types",
                    "state"
                ],
                /!*         "checkbox" : {
                 "three_state":false
                 },*!/
                "search":{
                    "fuzzy":false,
                    "show_only_matches":true
                },
                "types" : {
                    "casino" : {
                        "icon" : "../media/images/ca.png"
                    },
                    "nation" : {
                        "icon" : "../media/images/n.png"
                    },
                    "region" : {
                        "icon" : "../media/images/r.png"
                    },
                    "district" : {
                        "icon" :  "../media/images/d.png"
                    },
                    "club" : {
                        "icon": "../media/images/c.png"
                    }
                }
            }).bind("select_node.jstree",  function (e, data) {
                $('.jur').val(data.node.id);
                $('.cls').val(data.node.type);
            });
     */


        var $tree=$('#jurisdictionTree').jstree({

            "plugins" : [
                "search",
                "types",
                "unique"
            ],
            "types": {
                "casino": {
                    "icon": "../media/images/ca.png"
                }, "nation": {
                    "icon": "../media/images/n.png"
                },
                "region": {
                    "icon": "../media/images/r.png"
                },
                "district": {
                    "icon": "../media/images/d.png"
                },
                "club": {
                    "icon": "../media/images/c.png"
                }
            }
            ,'core' : {
                'data' : {
                    'url' : function (node) {
                        return '/services/jurisdiction_services.inc';
                    },
                    'data' : function (node) {
                        return {'id': node.id, 'action': 'getJurisdictions',parent:true};
                    },
                    'dataType':'json'
                }
            }
        }).bind("select_node.jstree",  function (e, data) {
            $('.jur').val(data.node.id);
            $('.cls').val(data.node.li_attr.class);
        }).on('ready.jstree', function (e, data) {
                <?
                 if($cls!='casino') {
                     $jurSelected = getJurisdictionPathByIdAndClass($jur, $cls);
                 }
            else{
                $jurSelected=json_encode(array());
            }
                ?>
                jurToSelect = <?=$jurSelected;?>;
                jurToOpen = <?=$jurSelected;?>;
                jurToOpen.shift();
                data.instance.set_state({ core : {
                    'selected':  [jurToSelect[0]],
                    open : jurToOpen

                } });
            });


    });

    Highcharts.theme = {
        colors: ["#DDDF0D", "#7798BF", "#55BF3B", "#DF5353", "#aaeeee", "#ff0066", "#eeaaee",
            "#55BF3B", "#DF5353", "#7798BF", "#aaeeee"],
        chart: {
            backgroundColor: {
                linearGradient: { x1: 0, y1: 0, x2: 0, y2: 1 },
                stops: [
                    [0, 'rgb(96, 96, 96)'],
                    [1, 'rgb(16, 16, 16)']
                ]
            },
            borderWidth: 0,
            borderRadius: 15,
            plotBackgroundColor: null,
            plotShadow: false,
            plotBorderWidth: 0
        },
        title: {
            style: {
                color: '#FFF',
                font: '16px Lucida Grande, Lucida Sans Unicode, Verdana, Arial, Helvetica, sans-serif'
            }
        },
        subtitle: {
            style: {
                color: '#DDD',
                font: '12px Lucida Grande, Lucida Sans Unicode, Verdana, Arial, Helvetica, sans-serif'
            }
        },
        xAxis: {
            gridLineWidth: 0,
            lineColor: '#999',
            tickColor: '#999',
            labels: {
                style: {
                    color: '#999',
                    fontWeight: 'bold'
                }
            },
            title: {
                style: {
                    color: '#AAA',
                    font: 'bold 12px Lucida Grande, Lucida Sans Unicode, Verdana, Arial, Helvetica, sans-serif'
                }
            }
        },
        yAxis: {
            alternateGridColor: null,
            minorTickInterval: null,
            gridLineColor: 'rgba(255, 255, 255, .1)',
            minorGridLineColor: 'rgba(255,255,255,0.07)',
            lineWidth: 0,
            tickWidth: 0,
            labels: {
                style: {
                    color: '#999',
                    fontWeight: 'bold'
                }
            },
            title: {
                style: {
                    color: '#AAA',
                    font: 'bold 12px Lucida Grande, Lucida Sans Unicode, Verdana, Arial, Helvetica, sans-serif'
                }
            }
        },
        legend: {
            itemStyle: {
                color: '#CCC'
            },
            itemHoverStyle: {
                color: '#FFF'
            },
            itemHiddenStyle: {
                color: '#333'
            }
        },
        labels: {
            style: {
                color: '#CCC'
            }
        },
        tooltip: {
            backgroundColor: {
                linearGradient: { x1: 0, y1: 0, x2: 0, y2: 1 },
                stops: [
                    [0, 'rgba(96, 96, 96, .8)'],
                    [1, 'rgba(16, 16, 16, .8)']
                ]
            },
            borderWidth: 0,
            style: {
                color: '#FFF'
            }
        },

        plotOptions: {
            series: {
                shadow: true
            },
            line: {
                dataLabels: {
                    color: '#CCC'
                },
                marker: {
                    lineColor: '#333'
                }
            },
            spline: {
                marker: {
                    lineColor: '#333'
                }
            },
            scatter: {
                marker: {
                    lineColor: '#333'
                }
            },
            candlestick: {
                lineColor: 'white'
            }
        },

        toolbar: {
            itemStyle: {
                color: '#CCC'
            }
        },

        navigation: {
            buttonOptions: {
                symbolStroke: '#DDDDDD',
                hoverSymbolStroke: '#FFFFFF',
                theme: {
                    fill: {
                        linearGradient: { x1: 0, y1: 0, x2: 0, y2: 1 },
                        stops: [
                            [0.4, '#606060'],
                            [0.6, '#333333']
                        ]
                    },
                    stroke: '#000000'
                }
            }
        },

        // scroll charts
        rangeSelector: {
            buttonTheme: {
                fill: {
                    linearGradient: { x1: 0, y1: 0, x2: 0, y2: 1 },
                    stops: [
                        [0.4, '#888'],
                        [0.6, '#555']
                    ]
                },
                stroke: '#000000',
                style: {
                    color: '#CCC',
                    fontWeight: 'bold'
                },
                states: {
                    hover: {
                        fill: {
                            linearGradient: { x1: 0, y1: 0, x2: 0, y2: 1 },
                            stops: [
                                [0.4, '#BBB'],
                                [0.6, '#888']
                            ]
                        },
                        stroke: '#000000',
                        style: {
                            color: 'white'
                        }
                    },
                    select: {
                        fill: {
                            linearGradient: { x1: 0, y1: 0, x2: 0, y2: 1 },
                            stops: [
                                [0.1, '#000'],
                                [0.3, '#333']
                            ]
                        },
                        stroke: '#000000',
                        style: {
                            color: 'yellow'
                        }
                    }
                }
            },
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
            maskFill: 'rgba(16, 16, 16, 0.5)',
            series: {
                color: '#7798BF',
                lineColor: '#A6C7ED'
            }
        },

        scrollbar: {
            barBackgroundColor: {
                linearGradient: { x1: 0, y1: 0, x2: 0, y2: 1 },
                stops: [
                    [0.4, '#888'],
                    [0.6, '#555']
                ]
            },
            barBorderColor: '#CCC',
            buttonArrowColor: '#CCC',
            buttonBackgroundColor: {
                linearGradient: { x1: 0, y1: 0, x2: 0, y2: 1
                },
                stops: [
                    [0.4, '#888'],
                    [0.6, '#555']
                ]
            },
            buttonBorderColor: '#CCC',
            rifleColor: '#FFF',
            trackBackgroundColor: {
                linearGradient: { x1: 0, y1: 0, x2: 0, y2: 1 },
                stops: [
                    [0, '#000'],
                    [1, '#333']
                ]
            },
            trackBorderColor: '#666'
        },

        // special colors for some of the demo examples
        legendBackgroundColor: 'rgba(48, 48, 48, 0.8)',
        legendBackgroundColorSolid: 'rgb(70, 70, 70)',
        dataLabelsColor: '#444',
        textColor: '#E0E0E0',
        maskColor: 'rgba(255,255,255,0.3)'
    };

    // Apply the theme
    var highchartsOptions = Highcharts.setOptions(Highcharts.theme);

    </script>


    <div class="container" style="position: absolute;left:0;right: 0;width: 100%" >
        <table class="table table-bordered">
            <tr>
                <td class="navbar-inner" colspan="2">
                    <h3 class="text-center"><?=$lang->getLang("Stats")?></h3>
                </td>
            </tr>
            <tr>
                <td class="span4">
                    <!--      <div class="contentAdmin">
            <div class="contentHeadAdmin"><?/*=$lang->getLang("toggle")*/?></div>
            <div class="well">
                <div class="btn-group btn-group-vertical">
                    <button type="button" style="width: 220px" class="btn btn-small btn-inverse toggleGraph" id="btn_graphplayer"><?/*=$lang->getLang("Players per day")*/?></button>
                    <button type="button" style="width: 220px" class="btn btn-small btn-inverse toggleGraph" id="btn_graphnet"><?/*=$lang->getLang("Real net per day")*/?></button>
                    <button type="button" style="width: 220px" class="btn btn-small btn-inverse toggleGraph" id="btn_graphperc"><?/*=$lang->getLang("Percent of return")*/?></button>
                    <button type="button" style="width: 220px" class="btn btn-small btn-inverse toggleGraph" id="btn_graphregplayers"><?/*=$lang->getLang("Registered players per day")*/?></button>
                    <button type="button" style="width: 220px" class="btn btn-small btn-inverse toggleGraph" id="btn_graphdeposit"><?/*=$lang->getLang("Deposit and withdraw")*/?></button>
                    <button type="button" style="width: 220px" class="btn btn-small btn-info toggleAll" ><?/*=$lang->getLang("All")*/?></button>
                </div>
            </div>
        </div>-->
                    <div class="contentAdmin">
                        <div class="contentHeadAdmin"><?=$lang->getLang("Jurisdiction chooser")?></div>
                        <div class="well">
                           <input type="text" id='jurisdictionTreeSearch' style="float:left;padding: 5px;width: 94%" placeholder="<?=$lang->getLang("Search (min. 4 char)")?>">
                            <div id="searchResult"></div>
                            <div class="clearfix"></div>
                             <br/>
                            <div id="jurisdictionTree">
                            </div>
                        </div>
                    </div>
                </td>
                <td style='vertical-align: top'>
                    <div class="well">
                        <div class="tabbable">
                            <ul class="nav nav-tabs">
                                <li class="<?=($active=='general'? 'active':'')?>"><a href="#generalStats" data-toggle="tab"><button class="btn btn-inverse" id="general"><?=$lang->getLang("Game stats")?></button></a></li>
                                <li <?=($active=='system'? 'class="active"':'')?>><a href="#systemStats" data-toggle="tab"><button class="btn btn-inverse" id="system"><?=$lang->getLang("System stats")?></button></a></li>
                                <li <?=($active=='totem'? 'class="active"':'')?>><a href="#totemStats" data-toggle="tab"><button class="btn btn-inverse" id="totemBtn"><?=$lang->getLang("Totem stats")?></button></a></li>
                                <?php
                                if ($casino_stats_playing_users_access) { ?>
                                    <li <?=($active=='playingUsers'? 'class="active"':'')?>><a href="#playingUsers" data-toggle="tab"><button class="btn btn-inverse" id="playingUsersBtn"><?=$lang->getLang("Playing users")?></button></a></li>
                                 <?php }?>

                            </ul>
                            <div class="tab-content">
                                <div class="tab-pane <?=($active=='general'? 'active':'')?>" id="generalStats">
                                    <div class="tabbable  tabs-left">
                                        <ul class="nav nav-tabs">
                                            <li <?=($subSection=='all'?'class="active"':'')?> <?=(($subSection=='all' || ($subSection!='ppday' && $subSection!='net' && $subSection!='procentofreturn'))?'class="active"':'')?> ><a href="#all" data-toggle="tab"><?=$lang->getLang("All")?></a></li>
                                            <li <?=($subSection=='ppday'?'class="active"':'')?>><a href="#ppday" data-toggle="tab"><?=$lang->getLang("Players Per Day")?></a></li>
                                            <li <?=($subSection=='net'?'class="active"':'')?>><a href="#net" data-toggle="tab"><?=$lang->getLang("Net")?></a></li>
                                            <li <?=($subSection=='procentofreturn'?'class="active"':'')?>><a href="#procentofreturn" data-toggle="tab"><?=$lang->getLang("Percent of return")?></a></li>
                                        </ul>
                                        <div class="tab-content">
                                            <div class="tab-pane <?=(($subSection=='all' || ($subSection!='ppday' && $subSection!='net' && $subSection!='procentofreturn'))? 'active':'')?> " id="all">
                                                <?createOptionsSelector('general','all')?>
                                                <?php
                                                if($interval->days > 93 && $type=='m'  &&  $_POST['subsection']=='ppday'){
                                                    addError('',$lang->getLang('Please choose an interval of maximum 3 months'));
                                                    showErrors();
                                                }
                                                elseif($interval->days > 31 && $type=='d' &&  $_POST['subsection']=='ppday'){
                                                    addError('',$lang->getLang('Please choose an interval of maximum 31 days'));
                                                    showErrors();
                                                }
                                                elseif($active=='general' && $_POST['subsection']=='all') {
                                                    getGeneralInfoStats();
                                                    getGeneralInfoStats('net');
                                                    getGeneralInfoStats('procentofreturn');
                                                }?>

                                             </div>
                                            <div class="tab-pane <?=$subSection=='ppday'?'active':''?>" id="ppday">
                                                <?createOptionsSelector('general','ppday')?>
                                                <?php
                                                if($interval->days > 93 && $type=='m'  &&  $_POST['subsection']=='ppday'){
                                                    addError('',$lang->getLang('Please choose an interval of maximum 3 months'));
                                                    showErrors();
                                                }
                                                elseif($interval->days > 31 && $type=='d' &&  $_POST['subsection']=='ppday'){
                                                    addError('',$lang->getLang('Please choose an interval of maximum 31 days'));
                                                    showErrors();
                                                }
                                                elseif($active=='general' && $_POST['subsection']=='ppday') {
                                                    getGeneralInfoStats();
                                                }?>
                                            </div>
                                            <div class="tab-pane <?=$subSection=='net'?'active':''?>" id="net">
                                                <?createOptionsSelector('general','net')?>
                                                <?php
                                                if($interval->days > 93 && $type=='m'  && $_POST['subsection']=='net'){
                                                    addError('',$lang->getLang('Please choose an interval of maximum 3 months'));
                                                    showErrors();
                                                }
                                                elseif($interval->days > 31 && $type=='d' && $_POST['subsection']=='net'){
                                                    addError('',$lang->getLang('Please choose an interval of maximum 31 days'));
                                                    showErrors();
                                                }
                                                elseif($active=='general'&& $_POST['subsection']=='net') {
                                                    getGeneralInfoStats('net');
                                                }?>
                                            </div>
                                            <div class="tab-pane <?=$subSection=='procentofreturn'?'active':''?>" id="procentofreturn">
                                                <?createOptionsSelector('general','procentofreturn')?>
                                                <?php

                                                if($interval->days > 93 && $type=='m'&& $_POST['subsection']=='procentofreturn'){
                                                    addError('',$lang->getLang('Please choose an interval of maximum 3 months'));
                                                    showErrors();
                                                }
                                                elseif($interval->days > 31 && $type=='d' && $_POST['subsection']=='procentofreturn'){
                                                    addError('',$lang->getLang('Please choose an interval of maximum 31 days'));
                                                    showErrors();
                                                }
                                                elseif($active=='general' && $_POST['subsection']=='procentofreturn') {
                                                    getGeneralInfoStats('procentofreturn');
                                                }?>
                                            </div>
                                        </div>
                                    </div>



                                </div>
                                <div class="tab-pane <?=($active=='system'? 'active':'')?>" id="systemStats">
                                    <div class="tabbable  tabs-left">
                                        <ul class="nav nav-tabs">
                                            <li <?=(($subSection=='allDR' || ($subSection!='registered' && $subSection!='deposits'))?'class="active"':'')?>><a href="#allDR" data-toggle="tab"><?=$lang->getLang("All")?></a></li>
                                            <li <?=($subSection=='registered'?'class="active"':'')?>><a href="#registered" data-toggle="tab"><?=$lang->getLang("Registered players per day")?></a></li>
                                            <li <?=($subSection=='deposits'?'class="active"':'')?>><a href="#deposits" data-toggle="tab"><?=$lang->getLang("Deposit and withdraw")?></a></li>
                                        </ul>
                                        <div class="tab-content">
                                            <div class="tab-pane <?=(($subSection=='allDR' || ($subSection!='registered' && $subSection!='deposits'))?'active':'')?>" id="allDR">
                                                <?
                                                createOptionsSelector('system','allDR');
                                                if($interval->y > 0 && $active=='system' && $_POST['subsection']=='allDR'){
                                                    addError('',$lang->getLang('Please choose an interval of maximum 1 year'));
                                                    showErrors();
                                                }

                                                else{
                                                    if(($_POST['subsection']=='allDR')){
                                                        getRegisteredPlayersPerDay();
                                                        getDepositsAndWithdraws();
                                                    }
                                                }
                                                ?>
                                            </div>
                                            <div class="tab-pane  <?=($subSection=='registered'?'active':'')?>" id="registered">
                                                <?createOptionsSelector('system','registered',true,true);
                                                if($interval->y > 0  && $active=='system' && $_POST['subsection']=='registered' ){
                                                    addError('',$lang->getLang('Please choose an interval of maximum 1 year'));
                                                    showErrors();
                                                }
                                                else{
                                                    if(($_POST['subsection']=='registered')){
                                                        getRegisteredPlayersPerDay('graphregplayers',$statType);
                                                    }
                                                }
                                                ?>
                                            </div>
                                            <div class="tab-pane <?=($subSection=='deposits'?'active':'')?>" id="deposits">
                                                <?createOptionsSelector('system','deposits',false,true);
                                                if($interval->y > 0  && $active=='system' && $_POST['subsection']=='deposits' && $statType=='detailed'){
                                                    addError('',$lang->getLang('Please choose an interval of maximum 1 year'));
                                                    showErrors();
                                                }
                                                elseif($interval->y > 0   && $active=='system' && $_POST['subsection']=='deposits' && $statType=='graph'){
                                                    addError('',$lang->getLang('Please choose an interval of maximum 1 year'));
                                                    showErrors();
                                                }
                                                elseif(($_POST['subsection']=='deposits')){

                                                    getDepositsAndWithdraws('graphdeposit',$statType);
                                                }
                                                ?>
                                            </div>
                                        </div>
                                    </div>

                                </div>
                                <div class="tab-pane <?=($active=='totem'? 'active':'')?>" id="totemStats">
                                    <div class="tabbable  tabs-left">
                                        <ul class="nav nav-tabs">
                                            <li  <?=(($subSection=='allTotem' || ($subSection!='ppdayTotem' && $subSection!='netTotem' && $subSection!='procentofreturnTotem'))?'class="active"':'')?>><a href="#allTotem" data-toggle="tab"><?=$lang->getLang("All")?></a></li>
                                            <li <?=($subSection=='ppdayTotem'?'class="active"':'')?>><a href="#ppdayTotem" data-toggle="tab"><?=$lang->getLang("Totems Per Day")?></a></li>
                                            <li <?=($subSection=='netTotem'?'class="active"':'')?>><a href="#netTotem" data-toggle="tab"><?=$lang->getLang("Net")?></a></li>
                                            <li <?=($subSection=='procentofreturnTotem'?'class="active"':'')?>><a href="#procentofreturnTotem" data-toggle="tab"><?=$lang->getLang("Percent of return")?></a></li>
                                        </ul>
                                        <div class="tab-content">
                                            <div class="tab-pane <?=(($subSection=='allTotem' || ($subSection!='ppdayTotem' && $subSection!='netTotem' && $subSection!='procentofreturnTotem'))?'active':'')?>" id="allTotem">
                                                <?createOptionsSelector('totem','allTotem')?>
                                                <?php
                                                if($interval->days > 93 && $type=='m'  &&  $_POST['subsection']=='allTotem'){
                                                    addError('',$lang->getLang('Please choose an interval of maximum 3 months'));
                                                    showErrors();
                                                }
                                                elseif($interval->days > 31 && $type=='d' &&  $_POST['subsection']=='allTotem'){
                                                    addError('',$lang->getLang('Please choose an interval of maximum 31 days'));
                                                    showErrors();
                                                }
                                                elseif($active=='totem' && $_POST['subsection']=='allTotem') {
                                                    getGeneralInfoStatsTotem();
                                                    getGeneralInfoStatsTotem('netTotem');
                                                    getGeneralInfoStatsTotem('procentofreturnTotem');
                                                }?>

                                            </div>
                                            <div class="tab-pane <?=$subSection=='ppdayTotem'?'active':''?>" id="ppdayTotem">
                                                <?createOptionsSelector('totem','ppdayTotem')?>
                                                <?php
                                                if($interval->m > 6 && $type=='m'  &&  $_POST['subsection']=='ppdayTotem'){
                                                    addError('',$lang->getLang('Please choose an interval of maximum 6 months'));
                                                    showErrors();
                                                }
                                                elseif($interval->m > 6 && $type=='d' &&  $_POST['subsection']=='ppdayTotem'){
                                                    addError('',$lang->getLang('Please choose an interval of maximum 6 months'));
                                                    showErrors();
                                                }
                                                elseif($active=='totem' && $_POST['subsection']=='ppdayTotem') {
                                                    getGeneralInfoStatsTotem();
                                                }?>
                                            </div>
                                            <div class="tab-pane <?=$subSection=='netTotem'?'active':''?>" id="netTotem">
                                                <?createOptionsSelector('totem','netTotem')?>
                                                <?php
                                                if($interval->m > 6 && $type=='m'  && $_POST['subsection']=='netTotem'){
                                                    addError('',$lang->getLang('Please choose an interval of maximum 6 months'));
                                                    showErrors();
                                                }
                                                elseif($interval->m > 6 && $type=='d' && $_POST['subsection']=='netTotem'){
                                                    addError('',$lang->getLang('Please choose an interval of maximum 6 months'));
                                                    showErrors();
                                                }
                                                elseif($active=='totem'&& $_POST['subsection']=='netTotem') {
                                                    getGeneralInfoStatsTotem('netTotem');
                                                }?>
                                            </div>
                                            <div class="tab-pane <?=$subSection=='procentofreturnTotem'?'active':''?>" id="procentofreturnTotem">
                                                <?createOptionsSelector('totem','procentofreturnTotem')?>
                                                <?php
                                                if($interval->m > 6 && $type=='m'  && $_POST['subsection']=='procentofreturnTotem'){
                                                    addError('',$lang->getLang('Please choose an interval of maximum 6 months'));
                                                    showErrors();
                                                }
                                                elseif($interval->m > 6 && $type=='d' && $_POST['subsection']=='procentofreturnTotem'){
                                                    addError('',$lang->getLang('Please choose an interval of maximum 6 months'));
                                                    showErrors();
                                                }
                                                elseif($active=='totem'&& $_POST['subsection']=='procentofreturnTotem') {
                                                    getGeneralInfoStatsTotem('procentofreturnTotem');
                                                }
                                                ?>
                                            </div>
                                        </div>
                                     </div>
                                </div>

                                <div class="tab-pane <?=($active=='playingUsers'? 'active':'')?>" id="playingUsers">
                                        <div class="tab" style="width: 80%">
                                            <div class="tab-pane" id="allPlayingUsers" >
                                                <div style="width: 100%">
                                                    <span class="error" id="printError"></span>
                                                    <button class="btn btn-danger fright" type="button" id="deleteSelectedFromCache" ><i class="icon-trash"></i> Delete All Selected</button>
                                                    <br/>
                                                    <br/>
                                                </div>


                                                <table id="userStats" class='display table table-striped table-hover table-bordered table-condensed'>
                                                    <thead>
                                                        <tr>
                                                            <th><?=$lang->getLang("Username")?></th>
                                                            <th><?=$lang->getLang("Jurisdiction")?></th>
                                                            <th><?=$lang->getLang("User Credit")?></th>
                                                            <th><?=$lang->getLang("Operations")?></th>
                                                        </tr>
                                                    </thead>
                                                </table>
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
/**
 * @param $childArray
 * @param bool $onlyChild
 * @param bool $agencyCode
 */
function showChild($childArray,$onlyChild=false,$agencyCode=false){

    foreach($childArray as $key => $value){
        if($value['class'] == 'casino'){
            $class_type = "casino";
        }elseif($value['class'] == 'nation'){
            $class_type = "nation";
        }elseif($value['class'] == 'region'){
            $class_type = "region";
        }elseif($value['class'] == 'district'){
            $class_type = "district";
        }elseif($value['class'] == 'club'){
            $class_type = "club";
        }
        if(!is_null($value['child'])){
            if(!$onlyChild ||($onlyChild && strpos(getAllSubJurisdictions($_SESSION['jurisdiction_class']), strtolower($class_type))!==false )){ ?>
                <li data-jstree='{ "type" : "<?=$class_type?>" }' class="jstree-closed <?=$class_type?>" id="<?=$value['id']?>"><?=(($agencyCode && $class_type!='casino')? "[".strtoupper($value['code'])."] " :'' )?><?=strtoupper($value['name'])?><ul><?php
            }
            showChild($value['child'],$onlyChild,$agencyCode);

            if(!$onlyChild ||($onlyChild && strpos(getAllSubJurisdictions($_SESSION['jurisdiction_class']), strtolower($class_type))!==false )){ ?>
                </ul></li><?php }
        }else{
            if(!$onlyChild ||($onlyChild && strpos(getAllSubJurisdictions($_SESSION['jurisdiction_class']), strtolower($class_type))!==false ) || $class_type=='club'){ ?>
                <li data-jstree='{ "type" : "<?=$class_type?>" }' class="<?=$class_type?>" id="<?=$value['id']?>"><?=($agencyCode? "[".strtoupper($value['code'])."] " :'' )?><?=strtoupper($value['name'])?></li><?php }
        }
    }
}

function getRegisteredPlayersPerDay($container='graphregplayers',$statType='graph'){
    global $dbh,$type,$cls,$jur,$date_start,$date_end,$lang;
    $starttime = microtime(true);
    if($statType=='graph'){
        $categoriesRegDaily=array();
        $daysReg=array();
        $dailyRegSql=" SELECT count(*) as reg_daily , ";
        if($type=='m'){
            $dailyRegSql.=" EXTRACT( YEAR_MONTH FROM  `pun_reg_date` )  pun_reg_date ";
        }
        else{
            $dailyRegSql.="   date(pun_reg_date) pun_reg_date ";
        }
        $dailyRegSql.=" from punter ";

        $jur_id = $_SESSION["jurisdiction_id"];
        $dailyRegSql .= "JOIN jurisdiction club ON club.jur_id  = pun_betting_club
  		        JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
                JOIN jurisdiction regional ON regional.jur_id = district.jur_parent_id
                JOIN jurisdiction nation   ON nation.jur_id   = regional.jur_parent_id";

        switch($_SESSION["jurisdiction_class"]){
            case "club":
                $dailyRegSql .= " WHERE club.jur_id = $jur_id";
                break;
            case "district":
                $dailyRegSql .= " WHERE district.jur_id = $jur_id";
                break;
            case "region":
                $dailyRegSql .= " WHERE regional.jur_id = $jur_id";
                break;
            case "nation":
                $dailyRegSql .= " WHERE nation.jur_id = $jur_id";
                break;

            case "casino":
                $dailyRegSql .= " WHERE 1=1";
                break;
        }
        if($cls=='nation'){
            $dailyRegSql.=" AND nation.jur_id=".$jur;
        }
        if($cls=='region'){
            $dailyRegSql.=" AND regional.jur_id=".$jur;
        }
        if($cls=='district'){
            $dailyRegSql.=" AND district.jur_id=".$jur;
        }
        if($cls=='club'){
            $dailyRegSql.=" AND club.jur_id=".$jur;
        }

        $dailyRegSql.=" AND  pun_reg_date BETWEEN  '$date_start 00:00:00' AND '$date_end 23:59:59' ";
        if($type=='m'){
            $dailyRegSql.=" GROUP BY EXTRACT( YEAR_MONTH FROM  `pun_reg_date` ) ";
        }
        else{
            $dailyRegSql.="  GROUP BY date(pun_reg_date) ";
        }

        $dailyRegRes = $dbh->exec($dailyRegSql);
        $categoriesRegDaily['registered']=array();

        while($dailyRegRes->hasNext()){
            $row = $dailyRegRes->next();
            if(!in_array($row['pun_reg_date'],$daysReg)){
                array_push($daysReg,$row['pun_reg_date']);
            }
            array_push($categoriesRegDaily['registered'],array($row['pun_reg_date']=>$row['reg_daily']));
        }
        $endtime = microtime(true);
        $duration = $endtime - $starttime;
        $duration=number_format($duration, 4, ',', '')."s";
        ?>
        <div class="daily" style="max-width: 90%">
            <div class="tip"><?=$lang->getLang("Time taken to execute your request")?>: <?=$duration?></div><br>
            <div id="<?=$container?>"></div><br>
        </div>
        <script>
            $(function(){
                $('#<?=$container?>').highcharts({
                    chart: {
                        zoomType:'x'
                    },
                    credits: {
                        text: ''
                    },
                    title: {
                        text: '<?=$lang->getLang("Registered players per day")?>'
                    },
                    xAxis: {
                        categories: [<?php foreach($daysReg as $k=>$v){ echo "'".($type=='m'?substr_replace($v, "-", 4, 0):$v)."',";} ?>],
                        labels: {
                            rotation: -45,
                            align: 'right',
                            style: {
                                fontSize: '13px',
                                fontFamily: 'Verdana, sans-serif'
                            }
                        }
                    },
                    tooltip: {
                        formatter: function() {
                            var s;
                            if (this.point.name) {
                                s = ''+
                                    this.point.name +': '+ this.y +' players';
                            } else {
                                s = ''+
                                    this.x  +': '+ this.y +' players';
                            }
                            return s;
                        }
                    },
                    series: [
                        <?php
                         $totals=array();
                         $daytotals=array();
                         foreach($categoriesRegDaily as $k=>$v){
                         $thistotal=0;

                    ?>
                        {
                            type: 'column',
                            name: '<?=$k?>',
                            data: [<?php
                foreach($daysReg as $kd=>$kv){
                $value='0,';
                $catplayed=0;
                    foreach($v as $catk=>$catv){
                        if(isset($catv[$kv])){
                           $value= round($catv[$kv],2);
                           $catplayed=1;
                          }
                      }
                 echo $value.",";
                 $daytotals[$kd] +=$value;
                 $thistotal +=$value;
                 }
                 $totals["$k"]=$thistotal;

             ?>]
                        },
                        <? } ?>
                        {
                            type: 'pie',
                            allowPointSelect: true,
                            name: 'Total net',
                            data: [
                                <?php
                               $i=0;
                                   foreach ($totals as $tk=>$tv){
                                   ?>
                                {
                                    name:'<?=$tk?>',
                                    y:<?=$tv?>,
                                    color: Highcharts.getOptions().colors[<?=$i?>]
                                },
                                <?
                                $i++;
                            }?>
                            ],
                            center: [30, 30],
                            size: 60,
                            showInLegend: false,
                            dataLabels: {
                                enabled: false
                            }
                        }]
                });

            });
        </script>
    <?
    }
    else{
        $dailyRegSql="SELECT pun_username,
                            pun_first_name,
                            pun_last_name,
                            pun_access,
                            pun_email,
                            pun_reg_date,
                            pun_member_type,
                            skn_name,
                            club.jur_name as jurisdiction_name from punter ";

        $dailyRegSql .= "JOIN jurisdiction club ON club.jur_id  = pun_betting_club
  		        LEFT JOIN skins on skn_id=club.jur_skn_id
  		        JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
                JOIN jurisdiction regional ON regional.jur_id = district.jur_parent_id
                JOIN jurisdiction nation   ON nation.jur_id   = regional.jur_parent_id
                ";
        $jur_id = $_SESSION["jurisdiction_id"];
        switch($_SESSION["jurisdiction_class"]){
            case "club":
                $dailyRegSql .= " WHERE club.jur_id = $jur_id";
                break;
            case "district":
                $dailyRegSql .= " WHERE district.jur_id = $jur_id";
                break;
            case "region":
                $dailyRegSql .= " WHERE regional.jur_id = $jur_id";
                break;
            case "nation":
                $dailyRegSql .= " WHERE nation.jur_id = $jur_id";
                break;

            case "casino":
                $dailyRegSql .= " WHERE 1=1";
                break;
        }
        if($cls=='nation'){
            $dailyRegSql.=" AND nation.jur_id=".$jur;
        }
        if($cls=='region'){
            $dailyRegSql.=" AND regional.jur_id=".$jur;
        }
        if($cls=='district'){
            $dailyRegSql.=" AND district.jur_id=".$jur;
        }
        if($cls=='club'){
            $dailyRegSql.=" AND club.jur_id=".$jur;
        }

        $dailyRegSql.=" AND  pun_reg_date BETWEEN  '$date_start 00:00:00' AND '$date_end 23:59:59' ";
        $dailyRegRes = $dbh->exec($dailyRegSql);
        $endtime = microtime(true);
        $duration = $endtime - $starttime;
        $duration=number_format($duration, 4, ',', '')."s";
        ?>
        <div style="max-width: 100%">
        <div class="tip"><?=$lang->getLang("Time taken to execute your request")?>: <?=$duration?></div><br>
        <table class="display table table-striped table-bordered table-condensed">
            <thead>
            <tr>
                <th><?=$lang->getLang("Username")?></th>
                <th><?=$lang->getLang("Name")?>,<br /><?=$lang->getLang("Last Name")?></th>
                <th><?=$lang->getLang("Type")?></th>
                <th class='no-sort'><?=$lang->getLang("Status")?></th>
                <th><?=$lang->getLang("Email")?></th>
                <th><?=$lang->getLang("Registration Date")?></th>
                <th class='no-sort'><?=$lang->getLang("Skin")?>,<br /><?=$lang->getLang("Jurisdiction")?></th>
            </tr>
            </thead>
            <tbody>
            <?
            while($dailyRegRes->hasNext()){
                $row = $dailyRegRes->next();
                ?>
                <tr>
                    <td><?=$row['pun_username']?></td>
                    <td><?=$row['pun_first_name']?><br /><?=$row['pun_last_name']?></td>
                    <td><?=$row['pun_member_type']?></td>
                    <td><span class="<?=(strtolower($row['pun_access'])=='allow'? "text-success" :"text-error")?>"><?=strtoupper($row['pun_access'])?></span></td>
                    <td><?=$row['pun_email']?></td>
                    <td><?=$row['pun_reg_date']?></td>
                    <td><?=$row['skn_name']?> <br /> <?=$row['jurisdiction_name']?></td>
                </tr>
            <?
            }
            ?>
            </tbody>
        </table>
        </div>
        <script>
            $('.display').dataTable({'iDisplayLength': 100,'sPaginationType':"full_numbers","sDom": 'T<"clear">lfrtip','aoColumnDefs' : [ {
                'bSortable' : false,
                'aTargets' : [ 'no-sort' ]
            } ]});
        </script>
    <?
    }
}

function getDepositsAndWithdraws($container='graphdeposit',$statType='graph'){
    global $dbh,$type,$cls,$jur,$date_start,$date_end,$lang;
    $starttime = microtime(true);
    if($statType=='graph'){
        $categoriesDepositsDaily=array();
        $daysDep=array();
        if($type=='m'){
            $dailyDepSql=" SELECT EXTRACT( YEAR_MONTH FROM  `ptr_start` ) ptr_start ";
        }
        else{
            $dailyDepSql="   SELECT date(ptr_start) ptr_start ";
        }
        $dailyDepSql.=" ,sum(if(ptr_tty_id=7 or ptr_tty_id=3, ptr_amount, 0)) deposit,
                   sum(if(ptr_tty_id=8 or ptr_tty_id=10 or ptr_tty_id=16, abs(ptr_amount), 0)) withdraw,
                   club.jur_currency currency
                   from processor_transaction
                   join punter on ptr_pun_id=pun_id ";

        $dailyDepSql .= "JOIN jurisdiction club ON club.jur_id  = pun_betting_club
  		        JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
                JOIN jurisdiction regional ON regional.jur_id = district.jur_parent_id
                JOIN jurisdiction nation   ON nation.jur_id   = regional.jur_parent_id";
        $jur_id = $_SESSION["jurisdiction_id"];
        switch($_SESSION["jurisdiction_class"]){
            case "club":
                $dailyDepSql .= " WHERE club.jur_id = $jur_id";
                break;
            case "district":
                $dailyDepSql .= " WHERE district.jur_id = $jur_id";
                break;
            case "region":
                $dailyDepSql .= " WHERE regional.jur_id = $jur_id";
                break;
            case "nation":
                $dailyDepSql .= " WHERE nation.jur_id = $jur_id";
                break;

            case "casino":
                $dailyDepSql .= " WHERE 1=1";
                break;
        }

        if($cls=='nation'){
            $dailyDepSql.=" AND nation.jur_id=".$jur;
        }
        if($cls=='region'){
            $dailyDepSql.=" AND regional.jur_id=".$jur;
        }
        if($cls=='district'){
            $dailyDepSql.=" AND district.jur_id=".$jur;
        }
        if($cls=='club'){
            $dailyDepSql.=" AND club.jur_id=".$jur;
        }

        $dailyDepSql.=" AND ptr_status='A'
                         AND ptr_start BETWEEN  '$date_start 00:00:00' AND '$date_end 23:59:59'  ";
        if($type=='m'){
            $dailyDepSql.="  GROUP BY EXTRACT( YEAR_MONTH FROM  `ptr_start` )";
        }
        else{
            $dailyDepSql.="   GROUP BY date(ptr_start) ";
        }

        $dailyDepRes = $dbh->exec($dailyDepSql);
        $categoriesDepositsDaily['deposit']=array();
        $categoriesDepositsDaily['withdraw']=array();
        while($dailyDepRes->hasNext()){
            $row = $dailyDepRes->next();
            if($row["currency"]!=$_SESSION["currency_id"]){
                if($_SESSION["currency_id"]==1){
                    $row["deposit"]=bcdiv($row["deposit"],$_SESSION["currencies"][$row["currency"]]["value"],3);
                    $row["withdraw"]=bcdiv($row["withdraw"],$_SESSION["currencies"][$row["currency"]]["value"],3);
                }
                else{
                    $row["deposit"]=bcmul($row["deposit"],$_SESSION["currencies"][$_SESSION["currency_id"]]["value"],3);
                    $row["withdraw"]=bcmul($row["withdraw"],$_SESSION["currencies"][$_SESSION["currency_id"]]["value"],3);
                }
            }
            if(!in_array($row['ptr_start'],$daysDep)){
                array_push($daysDep,$row['ptr_start']);
            }

            array_push($categoriesDepositsDaily['deposit'],array($row['ptr_start']=>$row['deposit']));
            array_push($categoriesDepositsDaily['withdraw'],array($row['ptr_start']=>$row['withdraw']));
        }

        $endtime = microtime(true);
        $duration = $endtime - $starttime;
        $duration=number_format($duration, 4, ',', '')."s";
        ?>
        <div class="daily" style="max-width: 90%">
            <div class="tip"><?=$lang->getLang("Time taken to execute your request")?>: <?=$duration?></div><br>
            <div id="<?=$container?>"></div><br>
        </div>
        <script>
            $(function(){
                $('#<?=$container?>').highcharts({
                    chart: {
                        zoomType:'x'
                    },
                    credits: {
                        text: ''
                    },
                    title: {
                        text: '<?=$lang->getLang("Deposit and withdraw")?>'
                    },
                    xAxis: {
                        categories: [<?php foreach($daysDep as $k=>$v){ echo "'".($type=='m'?substr_replace($v, "-", 4, 0):$v)."',";} ?>],
                        labels: {
                            rotation: -45,
                            align: 'right',
                            style: {
                                fontSize: '13px',
                                fontFamily: 'Verdana, sans-serif'
                            }
                        }
                    },
                    tooltip: {
                        useHTML: true,
                        formatter: function() {
                            var s;
                            if (this.point.name) {
                                s = ''+
                                    this.point.name +': '+ this.y ;
                            } else {
                                s = ''+
                                    this.x  +': '+ this.y + " <span><?=$_SESSION["currency_html"]?></span>";
                            }
                            return s;
                        }
                    },
                    series: [
                        <?php
                         $totals=array();
                         $daytotals=array();
                         foreach($categoriesDepositsDaily as $k=>$v){
                         $thistotal=0;
                    ?>
                        {
                            type: 'column',
                            name: '<?=$k?>',
                            data: [<?php
                foreach($daysDep as $kd=>$kv){
                $value='0,';
                $catplayed=0;
                    foreach($v as $catk=>$catv){
                        if(isset($catv[$kv])){
                           $value= round($catv[$kv],2);
                           $catplayed=1;
                          }
                      }
                 echo $value.",";
                 $daytotals[$kd] +=$value;
                 $thistotal +=$value;
                 }
                 $totals["$k"]=$thistotal;
             ?>]
                        },
                        <? } ?>
                        {
                            type: 'pie',
                            allowPointSelect: true,
                            name: 'Total net',
                            data: [
                                <?php
                                $i=0;
                                   foreach ($totals as $tk=>$tv){
                                   ?>
                                {
                                    name:'<?=$tk?>',
                                    y:<?=$tv?>,
                                    color: Highcharts.getOptions().colors[<?=$i?>]
                                },
                                <?
                                $i++;
                            }?>
                            ],
                            center: [30, 30],
                            size: 60,
                            showInLegend: false,
                            dataLabels: {
                                enabled: false
                            }
                        }]
                });

            });

        </script>
    <?
    }
    else{
        $dailyDepSql="SELECT pun_username,
                            pun_email,
                            skn_name,
                           sum(if(ptr_tty_id=7 or ptr_tty_id=3, ptr_amount, 0)) deposit,
                           sum(if(ptr_tty_id=8 or ptr_tty_id=10 or ptr_tty_id=16, abs(ptr_amount), 0)) withdraw,
                           club.jur_name as jurisdiction_name ,
                           club.jur_currency currency,
                           prc_name
                   from processor_transaction
                   join processor on prc_id=ptr_prc_id
                   join punter on ptr_pun_id=pun_id ";

        $dailyDepSql .= "JOIN jurisdiction club ON club.jur_id  = pun_betting_club
  		        LEFT JOIN skins on club.jur_skn_id=skn_id
  		        JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
                JOIN jurisdiction regional ON regional.jur_id = district.jur_parent_id
                JOIN jurisdiction nation   ON nation.jur_id   = regional.jur_parent_id
                ";
        $jur_id = $_SESSION["jurisdiction_id"];
        switch($_SESSION["jurisdiction_class"]){
            case "club":
                $dailyDepSql .= " WHERE club.jur_id = $jur_id";
                break;
            case "district":
                $dailyDepSql .= " WHERE district.jur_id = $jur_id";
                break;
            case "region":
                $dailyDepSql .= " WHERE regional.jur_id = $jur_id";
                break;
            case "nation":
                $dailyDepSql .= " WHERE nation.jur_id = $jur_id";
                break;

            case "casino":
                $dailyDepSql .= " WHERE 1=1";
                break;
        }
        if($cls=='nation'){
            $dailyDepSql.=" AND nation.jur_id=".$jur;
        }
        if($cls=='region'){
            $dailyDepSql.=" AND regional.jur_id=".$jur;
        }
        if($cls=='district'){
            $dailyDepSql.=" AND district.jur_id=".$jur;
        }
        if($cls=='club'){
            $dailyDepSql.=" AND club.jur_id=".$jur;
        }

        $dailyDepSql.="
        AND ptr_status='A'
        AND ptr_start BETWEEN  '$date_start 00:00:00' AND '$date_end 23:59:59'
        GROUP BY pun_id ";

        $dailyDepRes = $dbh->exec($dailyDepSql);
        $totals=array();
        $totalsProc=array();
        $endtime = microtime(true);
        $duration = $endtime - $starttime;
        $duration=number_format($duration, 4, ',', '')."s";

        $depositsResult="";
        while($dailyDepRes->hasNext()){
            $row = $dailyDepRes->next();
            if($row["currency"]!=$_SESSION["currency_id"]){
                if($_SESSION["currency_id"]==1){
                    $row["deposit"]=bcdiv($row["deposit"],$_SESSION["currencies"][$row["currency"]]["value"],3);
                    $row["withdraw"]=bcdiv($row["withdraw"],$_SESSION["currencies"][$row["currency"]]["value"],3);
                }
                else{
                    $row["deposit"]=bcmul($row["deposit"],$_SESSION["currencies"][$_SESSION["currency_id"]]["value"],3);
                    $row["withdraw"]=bcmul($row["withdraw"],$_SESSION["currencies"][$_SESSION["currency_id"]]["value"],3);
                }
            }
            $totals['deposit']+=$row['deposit'];
            $totals['withdraw']+=$row['withdraw'];

            if(!isset($totalsProc[$row['prc_name']]['deposit'])){
                $totalsProc[$row['prc_name']]['deposit']=0;
            }
            if(!isset($totalsProc[$row['prc_name']]['withdraw'])){
                $totalsProc[$row['prc_name']]['withdraw']=0;
            }
            $totalsProc[$row['prc_name']]['deposit']+=$row['deposit'];
            $totalsProc[$row['prc_name']]['withdraw']+=$row['withdraw'];


            $depositsResult.="<tr>
                <td>".$row['pun_username']."</td>
                <td>".$row['skn_name']."<br />".$row['jurisdiction_name']."</td>
                <td>".$row['pun_email']."</td>
                <td>".$row['prc_name']."</td>
                <td>".getInDollars($row['deposit'])."</td>
                <td>".getInDollars($row['withdraw'])."</td>
            </tr>";

        }


        ?>
        <div class="tip"><?=$lang->getLang("Time taken to execute your request")?>: <?=$duration?></div><br>
        <div style="width: 100%">
            <div style="float: left">
                <table class="display  table table-striped table-bordered table-condensed">
                    <thead>
                    <tr>
                        <th><?=$lang->getLang("Total deposit")?></th>
                        <th><?=$lang->getLang("Total withdraw")?></th>
                        <th><?=$lang->getLang("Total users")?></th>
                        <th><?=$lang->getLang("Net")?></th>
                    </tr>
                    </thead>
                    <tbody>
                    <tr><td><?=getInDollars($totals['deposit'])?></td>
                        <td><?=getInDollars($totals['withdraw'])?></td>
                        <td><?=$dailyDepRes->getNumRows()?></td>
                         <td><?=getInDollars($totals['deposit']-$totals['withdraw'])?></td></tr>
                    </tbody>
                    </table>
            </div>
            <div style="float: right;">
                <table class="display  table table-striped table-bordered table-condensed">
                    <thead>
                    <tr>
                        <th><?=$lang->getLang("Processor")?></th>
                        <th><?=$lang->getLang("Total deposit")?></th>
                        <th><?=$lang->getLang("Total withdraw")?></th>
                        <th><?=$lang->getLang("Net")?></th>
                    </tr>
                    </thead>
                    <tbody>
                    <?foreach($totalsProc as $k=>$v){ ?>
                    <tr>
                        <td><?=$k?></td>
                        <td><?=getInDollars($v['deposit'])?></td>
                        <td><?=getInDollars($v['withdraw'])?></td>
                        <td><?=getInDollars($v['deposit']-$v['withdraw'])?></td>
                    </tr>
                    <? }?>
                    </tbody>
                </table>
            </div>
            <br /><br /><br /><br /><br /><br />
        </div>
        <div style="max-width: 100%">

        <table class="display displayDeposits table table-striped table-bordered table-condensed">
            <thead>
            <tr>
                <th><?=$lang->getLang("Username")?></th>
                <th class='no-sort'><?=$lang->getLang("Skin")?>,<br /><?=$lang->getLang("Jurisdiction")?></th>
                <th><?=$lang->getLang("Email")?></th>
                <th><?=$lang->getLang("Processor")?></th>
                <th><?=$lang->getLang("Total deposit")?></th>
                <th><?=$lang->getLang("Total withdraw")?></th>
            </tr>
            </thead>
            <tbody>
                 <?=$depositsResult?>
            </tbody>
        </table>
        </div>
        <script>
            $('.displayDeposits').dataTable({'iDisplayLength': 100,'sPaginationType':"full_numbers","sDom": 'T<"clear">lfrtip','aoColumnDefs' : [ {
                'bSortable' : false,
                'aTargets' : [ 'no-sort' ]
            } ]});
        </script>
    <?
    }

}

function createOptionsSelector($active='general',$subsection=false,$hideGroup=false,$includedetails=false){
    global $jur,$cls,$lang,$type,$date_start,$date_end,$statType;
    ?>
    <?form_head() ?>
    <?printHiddenInput('doQuery', 'yes'); ?>
    <?printHiddenInput('search_type', 'date_range'); ?>
    <input type='hidden' name='jur' class='jur' value="<?=$jur?>" >
    <input type='hidden' name='cls' class='cls' value="<?=$cls?>" >
    <table class="searchTable table table-bordered">
        <tbody>
        <?if($hideGroup==false){?>
            <tr>
                <td><div class='label label-inverse'><?=$lang->getLang("Group")?></div></td>
                <td><div class="controls">
                        <label class="radio">
                            <input class='span' type='radio' name='type' value='d' <?if($type=='d'){?> checked="checked"<?}?> ><?=$lang->getLang("Daily")?>
                        </label>
                        <label class="radio">
                            <input class='span' type='radio' name='type' value='m' <?if($type=='m'){?> checked="checked"<?}?>><?=$lang->getLang("Monthly")?>
                        </label>
                    </div></td>
            </tr>
            <tr class="dly">
                <td><div class='label label-inverse'><?=$lang->getLang("Choose month")?></div></td>
                <td><div class="controls">
                        <div class="input-prepend">
                            <span class="add-on"><i class="icon-calendar"></i></span>
                            <?=month_select_box()?>
                        </div>
                    </div></td>
            </tr>
        <? }?>

        <tr<?if($hideGroup==false){?> class="dly" <?} ?>>
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
        <tr<?if($hideGroup==false){?> class="dly" <?} ?>>
            <td><div class='label label-inverse'><?=$lang->getLang("From")?></div></td>
            <td> <div class="controls">
                    <div class="input-prepend">
                        <span class="add-on"><i class="icon-calendar"></i></span>
                        <input type='text' name='date_start'  class="dateStartHelper" placeholder="Date Start" value="<?=$date_start?>" >
                    </div>
                </div></td>
        </tr>
        <tr<?if($hideGroup==false){?> class="dly" <?} ?>>
            <td><div class='label label-inverse'><?=$lang->getLang("Until")?></div></td>
            <td><div class="controls">
                    <div class="input-prepend">
                        <span class="add-on"><i class="icon-calendar"></i></span>
                        <input type='text' name='date_end'  class="dateEndHelper"  placeholder="Date End" value="<?=$date_end?>">
                    </div>
                </div></td>
        </tr>
        <?if($hideGroup==false){?>
        <tr class="mnt">
            <td><div class='label label-inverse'><?=$lang->getLang("From")?></div></td>
            <td> <div class="controls">
                    <div class="input-prepend">
                        <span class="add-on"><i class="icon-calendar"></i></span>
                        <?
                        $month_options = '';
                        $curryear = date("Y");
                        $firstyear=$curryear-3;
                        for ($j = $firstyear; $j <= $curryear; $j++) {
                            for ($i = 1; $i <= 12; $i++) {
                                $month_num = str_pad($i, 2, 0, STR_PAD_LEFT);
                                $month_name = date('F', mktime(0, 0, 0, $i + 1, 0, 0, 0));
                                if((date('Y-m-d')) >= date('Y-m-t',strtotime($j.'-'.$month_num))) {
                                    $month_options .= '<option value="' . $j . '-' . ($month_num) . '" '.($_POST['monthHelperStart']==($j.'-'.$month_num)? 'selected':'').'  >' . $lang->getLang($month_name) . ' ' . $j . '</option>';
                                }
                            }

                        }
                        echo '<select id="monthHelperStart" name="monthHelperStart" class="monthSelecter">' . $month_options . '</select>';
                        ?>
                    </div>
                </div></td>
        </tr>
        <tr class="mnt">
            <td><div class='label label-inverse'><?=$lang->getLang("Until")?></div></td>
            <td><div class="controls">
                    <div class="input-prepend">
                        <span class="add-on"><i class="icon-calendar"></i></span>
                        <?
                        $month_options = '';
                        $curryear = date("Y");
                        $firstyear=$curryear-3;
                        for ($j = $firstyear; $j <= $curryear; $j++) {
                            for ($i = 1; $i <= 12; $i++) {
                                $month_num = str_pad($i, 2, 0, STR_PAD_LEFT);
                                $month_name = date('F', mktime(0, 0, 0, $i + 1, 0, 0, 0));
                                if((date('Y-m-d')) >= date('Y-m-t',strtotime($j.'-'.$month_num))) {
                                    $month_options .= '<option value="' . $j . '-' . ($month_num) . '" '.($_POST['monthHelperEnd']==($j.'-'.$month_num)? 'selected':'').'  >' . $lang->getLang($month_name) . ' ' . $j . '</option>';
                                }
                            }
                        }
                        echo '<select id="monthHelperEnd" name="monthHelperEnd" class="monthSelecter">' . $month_options . '</select>';
                        ?>
                    </div>
                </div></td>
        </tr>
        <? }?>

        <?if($includedetails){ ?>
            <tr>
                <td><div class='label label-inverse'><?=$lang->getLang("Stat type")?></div></td>
                <td><div class="controls">
                        <div class="input-prepend">
                            <span class="add-on"><i class="icon-calendar"></i></span>
                            <select name="statType">
                                <option <?=($statType=='graph'? "selected":"")?> value="graph">Graph</option>
                                <option  <?=($statType=='detailed'? "selected":"")?> value="detailed">Detailed</option>
                            </select>
                        </div>
                    </div></td>
            </tr>
        <? }?>
        <tr>
            <td></td>
            <td>
                <span class="span2"> <button class="btn btn-primary" ><?=$lang->getLang("Calculate")?></button></span>
            </td>
        </tr>
        </tbody>
    </table>
        <input type="hidden" name="active" value="<?=$active?>">
        <input type="hidden" name="subsection" value="<?=$subsection?>">

    </form>
<?
}


function getGeneralInfoStats($category='ppday'){
    global $dbh,$type,$cls,$jur,$date_start,$date_end,$lang;

    $sql=" SELECT gct_name,
            ((sum(jds_win) / sum(jds_bet)*100)) as perc,
            sum(jds_bet) as bet,
            sum(jds_win) as win,
            IF((jds_gam_category=7 OR JDS_GAM_CATEGORY=9) ,(sum(jds_bet) - sum(jds_win)), sum(jds_rake)) as net,
            sum(jds_players_num) as player, ";
    if($type=='m'){
        $sql.=" EXTRACT( YEAR_MONTH FROM  `jds_day` ) jds_day ,";
    }
    else{
        $sql.="       jds_day,";
    }
    $sql.=" jur_currency currency
            FROM jurisdiction_daily_summary , game_category,jurisdiction
            WHERE jds_gam_category=gct_id
            AND jur_id=jds_jur_club
            ";
    if($_SESSION['jurisdiction_class']=='nation'){
        $sql.=" AND jds_jur_nation=".$_SESSION['jurisdiction_id'];
    }
    if($_SESSION['jurisdiction_class']=='region'){
        $sql.=" AND jds_jur_region=".$_SESSION['jurisdiction_id'];
    }
    if($_SESSION['jurisdiction_class']=='district'){
        $sql.=" AND jds_jur_district=".$_SESSION['jurisdiction_id'];
    }
    if($_SESSION['jurisdiction_class']=='club'){
        $sql.=" AND jds_jur_club=".$_SESSION['jurisdiction_id'];
    }
    if($cls=='nation'){
        $sql.=" AND jds_jur_nation=".$jur;
    }
    if($cls=='region'){
        $sql.=" AND jds_jur_region=".$jur;
    }
    if($cls=='district'){
        $sql.=" AND jds_jur_district=".$jur;
    }
    if($cls=='club'){
        $sql.=" AND jds_jur_club=".$jur;
    }
    $sql.=" AND jds_day >=  '$date_start' AND jds_day <= '$date_end' ";

    if($type=='m'){
        $sql.="  GROUP BY EXTRACT( YEAR_MONTH FROM  `jds_day` ) , gct_name";
    }
    else{
        $sql.="  GROUP BY jds_day,gct_name";
    }


    $starttime = microtime(true);
    $rs = $dbh->doCachedQuery($sql,30);

    $days=array();
    $players=array();
    $categories=array();
    $categoriesNet=array();
    $categoriesPerc=array();
    while($rs->hasNext()){
        $row = $rs->next();
        if($row["currency"]!=$_SESSION["currency_id"]  && $cls!='casino'){
            if($_SESSION["currency_id"]==1){
                $row["net"]=bcdiv($row["net"],$_SESSION["currencies"][$row["currency"]]["value"],3);
            }
            else{
                $row["net"]=bcmul($row["net"],$_SESSION["currencies"][$_SESSION["currency_id"]]["value"],3);
            }

        }
        if(!in_array($row['jds_day'],$days)){
            array_push($days,$row['jds_day']);
        }
        if(!isset($categories[$row['gct_name']])){
            $categories[$row['gct_name']]=array();
        }
        if(!isset($categoriesNet[$row['gct_name']])){
            $categoriesNet[$row['gct_name']]=array();
        }
        if(!isset($categoriesPerc[$row['gct_name']])){
            $categoriesPerc[$row['gct_name']]=array();
        }
        array_push($categories[$row['gct_name']],array($row['jds_day']=>$row['player']));
        array_push($categoriesPerc[$row['gct_name']],array($row['jds_day']=>$row['perc']));
        if($cls!='casino'){
            $categoriesNet[$row['gct_name']][$row['jds_day']]+=$row['net'];
        }
    }
    if($cls=='casino'){
        $netSql=" SELECT gct_name,
            IF((jds_gam_category=7 OR JDS_GAM_CATEGORY=9) ,(sum(jds_bet) - sum(jds_win)), sum(jds_rake)) as net,";
        if($type=='m'){
            $netSql.=" EXTRACT( YEAR_MONTH FROM  `jds_day` ) jds_day ,";
        }
        else{
            $netSql.="       jds_day,";
        }

        $netSql.=" jur_currency currency
            FROM jurisdiction_daily_summary , game_category,jurisdiction
            WHERE jds_gam_category=gct_id
            AND jur_id=jds_jur_club
            ";
        $netSql.=" AND jds_day >=  '$date_start' AND jds_day <= '$date_end' ";
        if($type=='m'){
            $netSql.="  GROUP BY jur_id,EXTRACT( YEAR_MONTH FROM  `jds_day` ) , gct_name";
        }
        else{
            $netSql.="  GROUP BY jur_id,jds_day,gct_name";
        }



        $rs = $dbh->exec($netSql);

        while($rs->hasNext()){
            $row = $rs->next();
            if($row["currency"]!=$_SESSION["currency_id"]){
                if($_SESSION["currency_id"]==1){
                    $row["net"]=bcdiv($row["net"],$_SESSION["currencies"][$row["currency"]]["value"],3);
                }
                else{
                    $row["net"]=bcmul($row["net"],$_SESSION["currencies"][$_SESSION["currency_id"]]["value"],3);
                }

            }
            $categoriesNet[$row['gct_name']][$row['jds_day']]+=$row['net'];
        }
    }

    $endtime = microtime(true);
    $duration = $endtime - $starttime;
    $duration=number_format($duration, 4, ',', '')."s";
    ?>
    <div class="daily" style="max-width: 90%">
    <div class="tip"><?=$lang->getLang("Time taken to execute your request")?>: <?=$duration?></div><br>
    <?if($category=='ppday'){ ?>
        <div id="graphplayer"></div><br>
        <script type="text/javascript">
            $(function () {
                $('#graphplayer').highcharts({
                    chart: {
                        zoomType:'x'
                    },
                    credits: {
                        text: ''
                    },
                    title: {
                        text: '<?=$lang->getLang("Players per day")?>'
                    },
                    xAxis: {
                        categories: [<?php foreach($days as $k=>$v){ echo "'".($type=='m'?substr_replace($v, "-", 4, 0):substr($v,5))."',";} ?>],
                        labels: {
                            rotation: -45,
                            align: 'right',
                            style: {
                                fontSize: '13px',
                                fontFamily: 'Verdana, sans-serif'
                            }
                        }
                    },
                    tooltip: {
                        formatter: function() {
                            var s;
                            if (this.point.name) { // the pie chart
                                s = ''+ this.point.name +': '+ this.y +' players';
                            } else {
                                s = ''+ this.x  +': '+ this.y +' players';
                            }
                            return s;
                        }
                    },
                    series: [
                        <?php
                         $totals=array();
                         $daytotals=array();
                         foreach($categories as $k=>$v){
                         $thistotal=0;
                    ?>
                        {
                            type: 'column',
                            name: '<?=$k?>',
                            data: [<?php
                foreach($days as $kd=>$kv){
                $value='0,';
                $catplayed=0;
                    foreach($v as $catk=>$catv){
                        if(isset($catv[$kv])){
                           $value= $catv[$kv];
                           $catplayed=1;
                          }
                      }
                 echo $value.",";
                 $daytotals[$kd] +=$value;
                 $daylycateg[$kd]+=$catplayed;
                 $thistotal +=$value;
                 }
                 $totals["$k"]=$thistotal;
             ?>]
                        },
                        <? } ?>
                        {
                            type: 'spline',
                            name: 'Average',
                            data: [
                                <?php
                                foreach($daytotals as $key => $value) {
                                    $daytotals[$key] = $value / $daylycateg[$key];
                                }
                                $average=implode(",",$daytotals);
                                echo $average;
                                ?>
                            ],
                            marker: {
                                lineWidth: 2,
                                lineColor: Highcharts.getOptions().colors[3],
                                fillColor: 'white'
                            }
                        }, {
                            type: 'pie',
                            allowPointSelect: true,
                            name: 'Total net',
                            data: [
                                <?php
                               $i=0;
                                   foreach ($totals as $tk=>$tv){
                                   ?>
                                {
                                    name:'<?=$tk?>',
                                    y:<?=$tv?>,
                                    color: Highcharts.getOptions().colors[<?=$i?>]
                                },
                                <?
                                $i++;
                            }?>
                            ],
                            center: [30, 30],
                            size: 60,
                            showInLegend: false,
                            dataLabels: {
                                enabled: false
                            }
                        }]
                });
            });

        </script>
    <?}elseif($category=='net'){ ?>
        <div id="graphnet"></div><br>
        <script>
            $(function(){
                $('#graphnet').highcharts({
                    chart: {
                        zoomType:'x'
                    },
                    credits: {
                        text: ''
                    },
                    title: {
                        text: '<?=$lang->getLang("Real Net per day")?>'
                    },
                    xAxis: {
                        categories: [<?php foreach($days as $k=>$v){ echo "'".($type=='m'?substr_replace($v, "-", 4, 0):substr($v,5))."',";} ?>],
                        labels: {
                            rotation: -45,
                            align: 'right',
                            style: {
                                fontSize: '13px',
                                fontFamily: 'Verdana, sans-serif'
                            }
                        }
                    },
                    tooltip: {
                        useHTML: true,
                        formatter: function() {
                            var s;
                            if (this.point.name) { // the pie chart
                                s = ''+
                                    this.point.name +': '+ this.y +' net';
                            } else {
                                s = ' '+
                                    this.x  +': '+ this.y + " <span><?=$_SESSION["currency_html"]?></span>";
                            }
                            return s;
                        }
                    },
                    series: [
                        <?php
                         $totals=array();
                         $daytotals=array();
                         $daylycateg=array();
                         foreach($categoriesNet as $k=>$v){
                         $thistotal=0;
                    ?>
                        {
                            type: 'column',
                            name: '<?=$k?>',
                            data: [<?php
                foreach($days as $kd=>$kv){
                $value='0,';
                $catplayed=0;

                        if(isset($v[$kv])){
                           $value= round($v[$kv],2);
                           $catplayed=1;
                          }

                 echo $value.",";
                 $daytotals[$kd] +=$value;
                 $daylycateg[$kd]+=$catplayed;
                 $thistotal +=$value;
                 }
                 $totals["$k"]=$thistotal;
             ?>]
                        },
                        <? } ?>
                        {
                            type: 'spline',
                            name: 'Average',
                            data: [
                                <?php
                                foreach($daytotals as $key => $value) {
                                    $daytotals[$key] = round($value / $daylycateg[$key],2);
                                }
                                $average=implode(",",$daytotals);
                                echo $average;
                                ?>
                            ],
                            marker: {
                                lineWidth: 2,
                                lineColor: Highcharts.getOptions().colors[3],
                                fillColor: 'white'
                            }
                        }, {
                            type: 'pie',
                            allowPointSelect: true,
                            name: 'Total consumption',
                            data: [
                                <?php
                               $i=0;
                                   foreach ($totals as $tk=>$tv){
                                   ?>
                                {
                                    name:'<?=$tk?>',
                                    y:<?=$tv?>,
                                    color: Highcharts.getOptions().colors[<?=$i?>]
                                },
                                <?
                                $i++;
                            }?>
                            ],
                            center: [30, 30],
                            size: 60,
                            showInLegend: false,
                            dataLabels: {
                                enabled: false
                            }
                        }]

                });
            });

        </script>
    <? }else { ?>
        <div id="graphperc"></div><br>
        <script>
            $(function(){
                $('#graphperc').highcharts({
                    chart: {
                        zoomType:'x'
                    },
                    credits: {
                        text: ''
                    },
                    title: {
                        text: ' <?=$lang->getLang("Percent of Return")?>'
                    },
                    xAxis: {
                        categories: [<?php foreach($days as $k=>$v){ echo "'".($type=='m'?substr_replace($v, "-", 4, 0):substr($v,5))."',";} ?>],
                        labels: {
                            rotation: -45,
                            align: 'right',
                            style: {
                                fontSize: '13px',
                                fontFamily: 'Verdana, sans-serif'
                            }
                        }
                    },
                    tooltip: {
                        formatter: function() {
                            var s;
                            if (this.point.name) { // the pie chart
                                s = ''+
                                    this.point.name +': '+ this.y +' %';
                            } else {
                                s = ''+
                                    this.x  +': '+ this.y +' %';
                            }
                            return s;
                        }
                    },
                    series: [
                        <?php
                        $totalsperc=array();
                        $daytotalsperc=array();
                        $daylycateg=array();
                        foreach($categoriesPerc as $k=>$v){
                            $thistotal=0;
                            ?>
                        {
                            type: 'column',
                            name: '<?=$k?>',
                            data: [<?php
    foreach($days as $kd=>$kv){
        $value='0';
        $catplayed=0;
        foreach($v as $catk=>$catv){
            if(isset($catv[$kv])){
                $value= round($catv[$kv],2);
                $catplayed=1;
            }
        }
        echo $value.",";
        $daytotalsperc[$kd] +=$value;
        $daylycateg[$kd]+=$catplayed;
        $thistotal +=$value;
    }
    $totalsperc["$k"]=$thistotal;

    ?>]
                        },
                        <? } ?>
                        {
                            type: 'spline',
                            name: 'Average',
                            data: [
                                <?php
                                foreach($daytotalsperc as $key => $value) {
                                    $daytotalsperc[$key] = round($value / $daylycateg[$key],2);
                                }
                                $average=implode(",",$daytotalsperc);
                                echo $average;
                                ?>
                            ],
                            marker: {
                                lineWidth: 2,
                                lineColor: Highcharts.getOptions().colors[3],
                                fillColor: 'white'
                            }
                        }, {
                            type: 'pie',
                            name: 'Total consumption',
                            allowPointSelect: true,
                            data: [
                                <?php
                                $i=0;
                                foreach ($totalsperc as $tk=>$tv){
                                    ?>
                                {
                                    name:'<?=$tk?>',
                                    y:<?=$tv?>,
                                    color: Highcharts.getOptions().colors[<?=$i?>]
                                },
                                <?
                                $i++;
                            }?>
                            ],
                            center: [30, 30],
                            size: 60,
                            showInLegend: false,
                            dataLabels: {
                                enabled: false
                            }
                        }]
                });


            });
        </script>
    <? }?>
    </div>
<?
}


function getGeneralInfoStatsTotem($category='ppdayTotem'){
    global $dbh,$type,$cls,$jur,$date_start,$date_end,$lang;
    $sql=" SELECT gam_category gct_name,
            ((sum(cus_res_win + cus_res_tournament_win) / sum(cus_res_bet+cus_res_tournament_bet)*100)) as perc,
            sum(cus_res_bet + cus_res_tournament_bet) as bet,
            sum(cus_res_win + cus_res_tournament_win) as win,
            (sum(cus_res_bet+cus_res_tournament_bet) - sum(cus_res_win+cus_res_tournament_win)) as net,
            count(distinct cus_res_pun_id)  as player, ";
    if($type=='m'){
        $sql.=" EXTRACT( YEAR_MONTH FROM  `cus_res_day` ) jds_day ,";
    }
    else{
        $sql.=" cus_res_day  jds_day,";
    }
    $sql.=" c.jur_currency currency
            FROM customer_result_daily , game,jurisdiction n, jurisdiction r,jurisdiction d, jurisdiction c,totem
            WHERE cus_res_gam_id=gam_id
             AND ttm_pun_id = cus_res_pun_id
            AND c.jur_id=cus_res_club_id
            AND c.jur_id=ttm_club_id
            AND n.jur_id=r.jur_parent_id
            AND r.jur_id=d.jur_parent_id
            AND d.jur_id=c.jur_parent_id ";
    if($_SESSION['jurisdiction_class']=='nation'){
        $sql.=" AND n.jur_id=".$_SESSION['jurisdiction_id'];
    }
    if($_SESSION['jurisdiction_class']=='region'){
        $sql.=" AND r.jur_id=".$_SESSION['jurisdiction_id'];
    }
    if($_SESSION['jurisdiction_class']=='district'){
        $sql.=" AND d.jur_id=".$_SESSION['jurisdiction_id'];
    }
    if($_SESSION['jurisdiction_class']=='club'){
        $sql.=" AND c.jur_id=".$_SESSION['jurisdiction_id'];
    }
    if($cls=='nation'){
        $sql.=" AND n.jur_id=".$jur;
    }
    if($cls=='region'){
        $sql.=" AND r.jur_id=".$jur;
    }
    if($cls=='district'){
        $sql.=" d.jur_id=".$jur;
    }
    if($cls=='club'){
        $sql.=" AND c.jur_id=".$jur;
    }
    $sql.=" AND cus_res_day>=  '$date_start' AND cus_res_day <= '$date_end' ";

    if($type=='m'){
        $sql.="  GROUP BY EXTRACT( YEAR_MONTH FROM  `jds_day` ) , gct_name";
    }
    else{
        $sql.="  GROUP BY jds_day,gct_name";
    }
    $starttime = microtime(true);
    $rs = $dbh->doCachedQuery($sql,30);
    $days=array();
    $categories=array();
    $categoriesNet=array();
    $categoriesPerc=array();
    while($rs->hasNext()){
        $row = $rs->next();
        if($row["currency"]!=$_SESSION["currency_id"]  && $cls!='casino'){
            if($_SESSION["currency_id"]==1){
                $row["net"]=bcdiv($row["net"],$_SESSION["currencies"][$row["currency"]]["value"],3);
            }
            else{
                $row["net"]=bcmul($row["net"],$_SESSION["currencies"][$_SESSION["currency_id"]]["value"],3);
            }
        }
        if(!in_array($row['jds_day'],$days)){
            array_push($days,$row['jds_day']);
        }
        if(!isset($categories[$row['gct_name']])){
            $categories[$row['gct_name']]=array();
        }
        if(!isset($categoriesNet[$row['gct_name']])){
            $categoriesNet[$row['gct_name']]=array();
        }
        if(!isset($categoriesPerc[$row['gct_name']])){
            $categoriesPerc[$row['gct_name']]=array();
        }
        array_push($categories[$row['gct_name']],array($row['jds_day']=>$row['player']));
        array_push($categoriesPerc[$row['gct_name']],array($row['jds_day']=>$row['perc']));
        if($cls!='casino'){
            $categoriesNet[$row['gct_name']][$row['jds_day']]+=$row['net'];
        }
    }
    if($cls=='casino'){
        $netSql=" SELECT gam_category gct_name,
            (sum(cus_res_bet+cus_res_tournament_bet) - sum(cus_res_win+cus_res_tournament_win)) as net,";
        if($type=='m'){
            $netSql.=" EXTRACT( YEAR_MONTH FROM  `cus_res_day` ) jds_day ,";
        }
        else{
            $netSql.=" cus_res_day jds_day,";
        }
        $netSql.=" jur_currency currency
            FROM customer_result_daily , game,jurisdiction,totem
            WHERE cus_res_gam_id=gam_id
             AND ttm_pun_id = cus_res_pun_id
            AND jur_id=cus_res_club_id

            ";
        $netSql.=" AND cus_res_day >=  '$date_start' AND cus_res_day <= '$date_end' ";
        if($type=='m'){
            $netSql.="  GROUP BY jur_id,EXTRACT( YEAR_MONTH FROM  `jds_day` ) , gct_name";
        }
        else{
            $netSql.="  GROUP BY jur_id,jds_day,gct_name";
        }
        $rs = $dbh->exec($netSql);
        while($rs->hasNext()){
            $row = $rs->next();
            if($row["currency"]!=$_SESSION["currency_id"]){
                if($_SESSION["currency_id"]==1){
                    $row["net"]=bcdiv($row["net"],$_SESSION["currencies"][$row["currency"]]["value"],3);
                }
                else{
                    $row["net"]=bcmul($row["net"],$_SESSION["currencies"][$_SESSION["currency_id"]]["value"],3);
                }
            }
            $categoriesNet[$row['gct_name']][$row['jds_day']]+=$row['net'];
        }
    }
    $endtime = microtime(true);
    $duration = $endtime - $starttime;
    $duration=number_format($duration, 4, ',', '')."s";
    ?>
    <div class="daily" style="max-width: 90%">
    <div class="tip"><?=$lang->getLang("Time taken to execute your request")?>: <?=$duration?></div><br>
    <?if($category=='ppdayTotem'){ ?>
        <div id="graphplayerTotem"></div><br>
        <script type="text/javascript">
            $(function () {
                $('#graphplayerTotem').highcharts({
                    chart: {
                        zoomType:'x'
                    },
                    credits: {
                        text: ''
                    },
                    title: {
                        text: '<?=$lang->getLang("Totems per day")?>'
                    },
                    xAxis: {
                        categories: [<?php foreach($days as $k=>$v){ echo "'".($type=='m'?substr_replace($v, "-", 4, 0):substr($v,5))."',";} ?>],
                        labels: {
                            rotation: -45,
                            align: 'right',
                            style: {
                                fontSize: '13px',
                                fontFamily: 'Verdana, sans-serif'
                            }
                        }
                    },
                    tooltip: {
                        formatter: function() {
                            var s;
                            if (this.point.name) { // the pie chart
                                s = ''+
                                    this.point.name +': '+ this.y +' totems';
                            } else {
                                s = ''+
                                    this.x  +': '+ this.y +' totems';
                            }
                            return s;
                        }
                    },
                    series: [
                        <?php
                         $totals=array();
                         $daytotals=array();
                         foreach($categories as $k=>$v){
                         $thistotal=0;
                    ?>
                        {
                            type: 'column',
                            name: '<?=$k?>',
                            data: [<?php
                foreach($days as $kd=>$kv){
                $value='0,';
                $catplayed=0;
                    foreach($v as $catk=>$catv){
                        if(isset($catv[$kv])){
                           $value= $catv[$kv];
                           $catplayed=1;
                          }
                      }
                 echo $value.",";
                 $daytotals[$kd] +=$value;
                 $daylycateg[$kd]+=$catplayed;
                 $thistotal +=$value;
                 }
                 $totals["$k"]=$thistotal;
             ?>]
                        },
                        <? } ?>
                        {
                            type: 'spline',
                            name: 'Average',
                            data: [
                                <?php
                                foreach($daytotals as $key => $value) {
                                    $daytotals[$key] = $value / $daylycateg[$key];
                                }
                                $average=implode(",",$daytotals);
                                echo $average;
                                ?>
                            ],
                            marker: {
                                lineWidth: 2,
                                lineColor: Highcharts.getOptions().colors[3],
                                fillColor: 'white'
                            }
                        }, {
                            type: 'pie',
                            allowPointSelect: true,
                            name: 'Total net',
                            data: [
                                <?php
                               $i=0;
                                   foreach ($totals as $tk=>$tv){
                                   ?>
                                {
                                    name:'<?=$tk?>',
                                    y:<?=$tv?>,
                                    color: Highcharts.getOptions().colors[<?=$i?>]
                                },
                                <?
                                $i++;
                            }?>
                            ],
                            center: [30, 30],
                            size: 60,
                            showInLegend: false,
                            dataLabels: {
                                enabled: false
                            }
                        }]
                });
            });

        </script>
    <?}elseif($category=='netTotem'){ ?>
        <div id="graphnetTotem"></div><br>
        <script>
            $(function(){
                $('#graphnetTotem').highcharts({
                    chart: {
                        zoomType:'x'
                    },
                    credits: {
                        text: ''
                    },
                    title: {
                        text: '<?=$lang->getLang("Real Net per day")?>'
                    },
                    xAxis: {
                        categories: [<?php foreach($days as $k=>$v){ echo "'".($type=='m'?substr_replace($v, "-", 4, 0):substr($v,5))."',";} ?>],
                        labels: {
                            rotation: -45,
                            align: 'right',
                            style: {
                                fontSize: '13px',
                                fontFamily: 'Verdana, sans-serif'
                            }
                        }
                    },
                    tooltip: {
                        useHTML: true,
                        formatter: function() {
                            var s;
                            if (this.point.name) { // the pie chart
                                s = ''+
                                    this.point.name +': '+ this.y +' net';
                            } else {
                                s = ' '+
                                    this.x  +': '+ this.y + " <span><?=$_SESSION["currency_html"]?></span>";
                            }
                            return s;
                        }
                    },
                    series: [
                        <?php
                         $totals=array();
                         $daytotals=array();
                         $daylycateg=array();
                         foreach($categoriesNet as $k=>$v){
                         $thistotal=0;
                    ?>
                        {
                            type: 'column',
                            name: '<?=$k?>',
                            data: [<?php
                foreach($days as $kd=>$kv){
                $value='0,';
                $catplayed=0;

                        if(isset($v[$kv])){
                           $value= round($v[$kv],2);
                           $catplayed=1;
                          }

                 echo $value.",";
                 $daytotals[$kd] +=$value;
                 $daylycateg[$kd]+=$catplayed;
                 $thistotal +=$value;
                 }
                 $totals["$k"]=$thistotal;
             ?>]
                        },
                        <? } ?>
                        {
                            type: 'spline',
                            name: 'Average',
                            data: [
                                <?php
                                foreach($daytotals as $key => $value) {
                                    $daytotals[$key] = round($value / $daylycateg[$key],2);
                                }
                                $average=implode(",",$daytotals);
                                echo $average;
                                ?>
                            ],
                            marker: {
                                lineWidth: 2,
                                lineColor: Highcharts.getOptions().colors[3],
                                fillColor: 'white'
                            }
                        }, {
                            type: 'pie',
                            allowPointSelect: true,
                            name: 'Total consumption',
                            data: [
                                <?php
                               $i=0;
                                   foreach ($totals as $tk=>$tv){
                                   ?>
                                {
                                    name:'<?=$tk?>',
                                    y:<?=$tv?>,
                                    color: Highcharts.getOptions().colors[<?=$i?>]
                                },
                                <?
                                $i++;
                            }?>
                            ],
                            center: [30, 30],
                            size: 60,
                            showInLegend: false,
                            dataLabels: {
                                enabled: false
                            }
                        }]

                });
            });

        </script>
    <? }else { ?>
        <div id="graphpercTotem"></div><br>
        <script>
            $(function(){
                $('#graphpercTotem').highcharts({
                    chart: {
                        zoomType:'x'
                    },
                    credits: {
                        text: ''
                    },
                    title: {
                        text: ' <?=$lang->getLang("Percent of Return")?>'
                    },
                    xAxis: {
                        categories: [<?php foreach($days as $k=>$v){ echo "'".($type=='m'?substr_replace($v, "-", 4, 0):substr($v,5))."',";} ?>],
                        labels: {
                            rotation: -45,
                            align: 'right',
                            style: {
                                fontSize: '13px',
                                fontFamily: 'Verdana, sans-serif'
                            }
                        }
                    },
                    tooltip: {
                        formatter: function() {
                            var s;
                            if (this.point.name) { // the pie chart
                                s = ''+
                                    this.point.name +': '+ this.y +' %';
                            } else {
                                s = ''+
                                    this.x  +': '+ this.y +' %';
                            }
                            return s;
                        }
                    },
                    series: [
                        <?php
                        $totalsperc=array();
                        $daytotalsperc=array();
                        $daylycateg=array();
                        foreach($categoriesPerc as $k=>$v){
                            $thistotal=0;
                            ?>
                        {
                            type: 'column',
                            name: '<?=$k?>',
                            data: [<?php
    foreach($days as $kd=>$kv){
        $value='0';
        $catplayed=0;
        foreach($v as $catk=>$catv){
            if(isset($catv[$kv])){
                $value= round($catv[$kv],2);
                $catplayed=1;
            }
        }
        echo $value.",";
        $daytotalsperc[$kd] +=$value;
        $daylycateg[$kd]+=$catplayed;
        $thistotal +=$value;
    }
    $totalsperc["$k"]=$thistotal;

    ?>]
                        },
                        <? } ?>
                        {
                            type: 'spline',
                            name: 'Average',
                            data: [
                                <?php
                                foreach($daytotalsperc as $key => $value) {
                                    $daytotalsperc[$key] = round($value / $daylycateg[$key],2);
                                }
                                $average=implode(",",$daytotalsperc);
                                echo $average;
                                ?>
                            ],
                            marker: {
                                lineWidth: 2,
                                lineColor: Highcharts.getOptions().colors[3],
                                fillColor: 'white'
                            }
                        }, {
                            type: 'pie',
                            name: 'Total consumption',
                            allowPointSelect: true,
                            data: [
                                <?php
                                $i=0;
                                foreach ($totalsperc as $tk=>$tv){
                                    ?>
                                {
                                    name:'<?=$tk?>',
                                    y:<?=$tv?>,
                                    color: Highcharts.getOptions().colors[<?=$i?>]
                                },
                                <?
                                $i++;
                            }?>
                            ],
                            center: [30, 30],
                            size: 60,
                            showInLegend: false,
                            dataLabels: {
                                enabled: false
                            }
                        }]
                });


            });
        </script>
    <? }?>
    </div>


<?
}