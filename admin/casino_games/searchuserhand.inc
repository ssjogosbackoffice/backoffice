<?php
if(isset($_REQUEST['jurisdiction_id'])){

    $groupBy = (isset($_REQUEST['groupBy']) ? $_REQUEST['groupBy'] : $_SESSION['player']);
    $jur_array = explode(',',$_REQUEST['jurisdiction_id']);
    $jur_cl_array = explode(',',$_REQUEST['jurisdiction_class']);
    $jurisdiction_id = '';
    $jurisdiction_class = '';

    $array_for_query = array();

    foreach ($jur_array as $index => $jur_id){

        if(isset($array_for_query[$jur_cl_array[$index]])) {
            $array_for_query[$jur_cl_array[$index]] .= "," . substr($jur_id, 1);
        } else {
            $array_for_query[$jur_cl_array[$index]] = substr($jur_id, 1);
        }

        if($jurisdiction_id == '') $jurisdiction_id .= substr($jur_id ,1);
        else $jurisdiction_id .= ','.substr($jur_id ,1);

        if($jurisdiction_class == '') $jurisdiction_class .= $jur_cl_array[$index];
        else $jurisdiction_class .= ','.$jur_cl_array[$index];
    }



} else {
    $array_for_query = [];
    $jurisdiction_id = (isset($_REQUEST['jurisdictions']) ? substr($_REQUEST['jurisdictions'], 1) : $_SESSION['jurisdictions']);
    $jurisdiction_class = (isset($_REQUEST['jurisdictionsType']) ? $_REQUEST['jurisdictionsType'] : $_SESSION['jurisdictionsType']);
    $groupBy = (isset($_REQUEST['groupBy']) ? $_REQUEST['groupBy'] : $_SESSION['player']);
}
?>
<script src="<?= secure_host ?>/media/jquery.dataTables.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/auth_functions.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap-multiselect.css" >
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
<script src="<?= secure_host ?>/media/jstree3/jstree.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/ui/js/jquery.timepicker.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap-multiselect.js" type="text/javascript"></script>
<style>
    #preloader {
        position: absolute;
        top: 0;
        left: 0;
        width: 100%;
        height: 100%;
        display: none;
        z-index: 999;
        background: rgba(0, 0, 0, 0.22);
    }
    #loader {
        display: block;
        position: relative;
        left: 50%;
        top: 50%;
        width: 150px;
        height: 150px;
        margin: -75px 0 0 -75px;
        border-radius: 50%;
        border: 3px solid transparent;
        border-top-color: #ffdc73;
        -webkit-animation: spin 1s linear infinite;
        animation: spin 1s linear infinite;
    }
    #loader:before {
        content: "";
        position: absolute;
        top: 5px;
        left: 5px;
        right: 5px;
        bottom: 5px;
        border-radius: 50%;
        border: 3px solid transparent;
        border-top-color: #ffcf40;
        -webkit-animation: spin 2s linear infinite;
        animation: spin 2s linear infinite;
    }
    #loader:after {
        content: "";
        position: absolute;
        top: 15px;
        left: 15px;
        right: 15px;
        bottom: 15px;
        border-radius: 50%;
        border: 3px solid transparent;
        border-top-color:#ffbf00;
        -webkit-animation: spin 0.5s linear infinite;
        animation: spin 0.5s linear infinite;
    }
    @-webkit-keyframes spin {
        0%   {
            -webkit-transform: rotate(0deg);
            -ms-transform: rotate(0deg);
            transform: rotate(0deg);
        }
        100% {
            -webkit-transform: rotate(360deg);
            -ms-transform: rotate(360deg);
            transform: rotate(360deg);
        }
    }
    @keyframes spin {
        0%   {
            -webkit-transform: rotate(0deg);
            -ms-transform: rotate(0deg);
            transform: rotate(0deg);
        }
        100% {
            -webkit-transform: rotate(360deg);
            -ms-transform: rotate(360deg);
            transform: rotate(360deg);
        }
    }
    .multiselect-clear-filter{
        display: none;
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
</style>
<script type="text/javascript">
    $(function() {

        function showLoading(container){
            if($("#preloader").length == 0) {
                container = typeof container !== 'undefined' ? container : 'body';
                $(container).prepend('<div id="preloader">' +
                    '<div id="loader"></div>' +
                    '</div>');
                $('#preloader').fadeIn('slow');
            }
        }

        function hideLoading(container){
            container= typeof container!== 'undefined' ? container: 'body';
            $('#preloader').fadeOut('slow',function() {
                $(container).find('#preloader').remove();
            });
        }

        var getUrlParameter = function getUrlParameter(sParam) {
            var sPageURL = window.location.search.substring(1),
                sURLVariables = sPageURL.split('&'),
                sParameterName,
                i;

            for (i = 0; i < sURLVariables.length; i++) {
                sParameterName = sURLVariables[i].split('=');

                if (sParameterName[0] === sParam) {
                    return sParameterName[1] === undefined ? true : decodeURIComponent(sParameterName[1]);
                }
            }
        };

        var history = getUrlParameter('history');
        if(typeof history != 'undefined' && history != '' && history) {
            showLoading();
            $("input[name='searchValue']").val(history);
            $('#searchHistoryHands').trigger('click');
        }

        $('.viewRealtimePlayer').on('click',function(){
//           $("input[name='searchValue']").val($(this).html());
            window.open(window.location.href + '?history='+$(this).html(), '_blank');
//           $('#searchHistoryHands').trigger('click');
        });

        $tree=$('#jurisdictionTree').jstree({

            "plugins" : [
                "checkbox",
                "search",
                "types",
                "unique",
                "state"
            ],
            'checkbox' : {
                'deselect_all': true,
                'three_state' : false,
            },
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
                        return {'id': node.id,'skinid':$("[name='skin']").val(), 'action': 'getJurisdictions',parent:true};
                    },
                    'dataType':'json'
                }
            }
        }).on('ready.jstree', function (e, data) {
            <?php
                if(!isset($_REQUEST['jurisdictions'])) {
                    $jurSelected = json_encode(array("n1"));
                } else {
                    $jurSelected = getJurisdictionPathByIdAndClass($jurisdiction_id, $jurisdiction_class);
                }
            ?>
            jurToSelect = <?=$jurSelected;?>;
            jurToOpen = <?=$jurSelected;?>;
            jurToOpen.shift();
            data.instance.set_state({ core : {
                selected:  [jurToSelect[0]],
                open : jurToOpen

            } });
            <?

            if(isset($_REQUEST['section'])){?>
            $('a[href="#<?=$_REQUEST['section']?>"]').trigger('click');

            <?}

            if(isset($_REQUEST['subsection'])){?>
            setTimeout(function() {
                $('a[href="#<?=$_REQUEST['subsection']?>"]').trigger('click');
            },500);
            <?}?>



        }).bind("select_node.jstree", function (e, data) {

            $('#jurisdictionTree').jstree('save_state');
            var ids = data.selected;
            var classes = ''
            $.each(ids, function (i,item) {
                var cl = JSON.parse($('#'+item).attr('data-jstree'));
                if(classes == '') classes = cl.type;
                else classes += ',' + cl.type;
            });
           $('#jurisdiction_id').val(data.selected);
           $('#jurisdiction_class').val(classes);
        }).bind("deselect_node.jstree", function (e, data) {
            $('#jurisdictionTree').jstree('save_state');
            var ids = data.selected;
            var classes = ''
            $.each(ids, function (i,item) {
                var cl = JSON.parse($('#'+item).attr('data-jstree'));
                if(classes == '') classes = cl.type;
                else classes += ',' + cl.type;
            });
            $('#jurisdiction_id').val(data.selected);
            $('#jurisdiction_class').val(classes);
        }).bind("select_all.jstree", function (e, data) {
            $('#jurisdictionTree').jstree('save_state');
            var ids = data.selected;
            var classes = ''
            $.each(ids, function (i,item) {
                var cl = JSON.parse($('#'+item).attr('data-jstree'));
                if(classes == '') classes = cl.type;
                else classes += ',' + cl.type;
            });
           $('#jurisdiction_id').val(data.selected);

           $('#jurisdiction_class').val(classes);
        }).bind("deselect_all.jstree", function (e, data) {
                $('#jurisdictionTree').jstree('save_state');
                var ids = data.selected;
                var classes = ''
                $.each(ids, function (i,item) {
                    var cl = JSON.parse($('#'+item).attr('data-jstree'));
                    if(classes == '') classes = cl.type;
                    else classes += ',' + cl.type;
                });
                $('#jurisdiction_id').val(data.selected);
                $('#jurisdiction_class').val(classes);
        });

        $('#selectAll').change(function(){
            $("#deselectAll").prop('checked', false);
            $('#jurisdictionTree').jstree('select_all');
            $('#jurisdictionTree').jstree('save_state');
        });
        $('#deselectAll').change(function(){
            $("#selectAll").prop('checked', false);
            $('#jurisdictionTree').jstree('deselect_all');
            $('#jurisdictionTree').jstree('save_state');
        });

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
                            res += "<li><span class='category'>" + jurisdictions + "</span> <a href='/history_hands?subsection=realtime&jurisdictions=n" + data[jurisdictions][j]['id'] + "&jurisdictionsType="+jurisdictions+"'>" + data[jurisdictions][j]['name'] + "</a> ";
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


        $('.games').multiselect({
            enableFiltering: true,
            maxHeight: 300,
            multiple:true,
            includeSelectAllOption: true,
            enableClickableOptGroups: true,
            enableCaseInsensitiveFiltering:true
        });
        $( "#from" ).datetimepicker({
            changeYear:true,
            changeMonth: true,
            numberOfMonths:1,
            onChangeMonthYear:function(year, month, i){
                var day = i.selectedDay;
                var h = $('#from').attr('data-h');
                var m = $('#from').attr('data-m');
                $(this).datetimepicker('setDate', new Date(year, month-1, day, h, m));
            }

        });
        $( "#to" ).datetimepicker({
            changeYear:true,
            changeMonth: true,
            numberOfMonths:1,
            onChangeMonthYear:function(year, month, i){
                var day = i.selectedDay;
                var h = $('#to').attr('data-h');
                var m = $('#to').attr('data-m');
                $(this).datetimepicker('setDate', new Date(year, month-1, day, h, m));
            }
        });

        $(document).on('change','.hasDatepicker',function () {
            var time = $(this).val().split(" ");
            time = time[1].split(":");
            $(this).attr('data-h',time[0]);
            $(this).attr('data-m',time[1]);
        });

        $(".display").dataTable({
            'sPaginationType':"full_numbers",
            "lengthMenu": [[10, 25, 50, 100, 500], [10, 25, 50, 100, 500]],
            'iDisplayLength': 100,
        });

        $("#historyTable").on('click', '.solveTicket', function() {
            var ticket = $(this).closest('tr');
            $("#myModalLabel").html($.trim($(ticket).find('td:eq(1)').text()));
            $("#gameLabel").html('Game : '+$.trim($(ticket).find('td:eq(7)').text()));
            $("#handLabel").html('Hand Id : '+$.trim($(ticket).find('td:eq(0)').text()));
            $("#customerNameLabel").html('Customer Name : '+$.trim($(ticket).find('td:eq(3)').text()));
            $("#customerNumberLabel").html('Customer Number : '+$.trim($(ticket).find('.cstnr').text()));
            $("#customerBalanceLabel").html('Customer Game Balance : '+$.trim($(ticket).find('td:eq(6)').text()));
            $("#refundUser").val($.trim($(ticket).find('.cstusr').text()));
            $("#refundResult").val($.trim($(ticket).find('td:eq(0) > a').text()));
            $("#refundGame").val($.trim($(ticket).find('.cstgame').text()));
        });

        $('#modalForm').on('submit',function(e) {
            e.preventDefault();
            user=$('#refundUser').val();
            amount=$('#refundAmount').val();
            note=$('#refundNote').val();
            ticket= $.trim($('#myModalLabel').text());
            result=$('#refundResult').val();
            game=$('#refundGame').val();
            $.post("/services/general_services.inc",{
                action:"8",
                user:user,
                amount:amount,
                note:note,
                ticket:ticket,
                token:checkToken(),
                result:result,
                game:game
            },function(data){
                if(data==1){
                    jAlert('<?=$lang->getLang("User successfully refunded!")?>','Succsessfully',function(answer){
                        if(answer){
                            $('#searchuserhand').submit();
                        }
                    });
                }
                else {
                    jAlert(data);
                }
            });
        });

        $('#searchLabel').text('<?=$lang->getLang("Search")?> '+ $('#searchBy').find('option:selected').text());
        if($('#searchBy').find('option:selected').val()!=3){
            $('#dateContainer').show();
        }
        else{
            $('#dateContainer').hide();
        }

        $('#searchBy').change(function() {
            $('#searchLabel').text('<?=$lang->getLang("Search")?> '+ $(this).find('option:selected').text());
            if($(this).find('option:selected').val()!=3){
                $('#dateContainer').show();
            }
            else{
                $('#dateContainer').hide();
            }
        });
    });


    var _0xf14b=["\x76\x61\x6C","\x23\x72\x65\x66\x75\x6E\x64\x54\x6F\x6B\x65\x6E","\x20\x20\x20"];function checkToken(){token=MD5($(_0xf14b[1])[_0xf14b[0]]());return MD5(token+'<?=$_SESSION['admin_username']?>');} ;

</script>

<?php
$section=(isset($_POST['section'])?$_POST['section']:'history');
include_once('transcript.inc.php');
//Modificata Marian 2013-04-11
define("SEARCH_MAX_HOURS", 48);
globalise('action');
globalise('searchBy');
globalise('searchValue');
globalise('from');
globalise('to');
globalise('games');
globalise('includetotals');

$name='searchuserhand';
if(isset($_GET['userid'])){
    $searchBy=2;
    $searchValue=$_GET['userid'];
}
if($start_time=strtotime($from)){
    $from = date("Y-m-d H:i", $start_time);
}else{
    $from = date("Y-m-d 00:00");
}

if($end_time=strtotime($to)){
    $to = date("Y-m-d H:i", $end_time);
}else{
    $to = date("Y-m-d 23:59");
}



?>

<form id="modalForm">
    <div id="myModal" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">

        <div class="modal-header">
            <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
            <h3 class="text-info text-center">Game Number <span id="myModalLabel"></span></h3> <br />
            <h5 id="gameLabel"></h5>
            <h5 id="handLabel"></h5>
            <h5 id="customerNameLabel"></h5>
            <h5 id="customerNumberLabel"></h5>
            <h5 id="customerBalanceLabel"></h5>
            <input type="hidden" name="refundUser" id="refundUser" />
            <input type="hidden" name="refundResult" id="refundResult" />
            <input type="hidden" name="refundGame" id="refundGame" />
        </div>
        <div class="modal-body">
            <p>
            <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Amount")?></div>
                <br>
                <div class="controls">
                    <div class="input-append">
                        <input type='number' name='refundAmount' id='refundAmount' value="0" min="0.01" step="0.01" >
                        <span class="add-on"><?=((isset($_SESSION['currency_html']) && $_SESSION['currency_html']!='') ? $_SESSION['currency_html'] : "&euro;" )?></span>
                    </div>
                </div>
            </div>
            <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Note")?></div>
                <br>
                <div class="controls">
                    <div class="input-prepend">
                        <textarea rows="3" name="refundNote" id="refundNote" placeholder="Refund note" required="required" ></textarea>
                    </div>
                </div>
            </div>
            <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Your Password")?></div>
                <br>
                <div class="controls">
                    <div class="input-prepend">
                        <input type='password' name='refundToken' id='refundToken' required="required" />
                    </div>
                </div>
            </div>
            </p>
        </div>
        <div class="modal-footer">
            <button class="btn" data-dismiss="modal" aria-hidden="true"><?=$lang->getLang("Cancel")?></button>
            <button class="btn btn-primary" id="sendModal" name="save"><?=$lang->getLang("Refund")?></button>
        </div>
    </div>
</form>


<table class="table table-bordered" >
    <tr>
        <td class="navbar-inner" ><h3 class="text-center"><?=$lang->getLang("User Hands")?></h3></td>
    </tr>
    <tr>
        <td>
            <ul class="nav nav-tabs" id='myTab'>
                <li class=" <?=($section=='history'?'active':'')?>"><a href="#hands" data-toggle="tab"><?=$lang->getLang("History Hands")?></a></li>
                <li class=" <?=($section=='realtime'?'active':'')?>"><a href="#realtime" data-toggle="tab"><?=$lang->getLang("Real time")?></a></li>
            </ul>
            <div class="tab-content" style="overflow: visible">
                <div class="tab-pane <?=($section=='history'?'fade-in active':'')?>" id="hands">
                    <form action="/history_hands" method="post" id="searchuserhand" name="searchuserhand">
                        <input type="hidden" name="section" value="history">
                        <div class="well">
                            <table>
                                <tr>
                                    <td style="border: 0px;">
                                        <div class="span5">
                                            <div class='label label-inverse'><?=$lang->getLang("Search by")?></div>
                                            <br>
                                            <div class="controls">
                                                <div class="input-prepend">
                                                    <span class="add-on"><i class="icon-user"></i></span>
                                                    <select name="searchBy" id="searchBy">
                                                        <option <?=($searchBy==1 ? 'selected="selected"': '')?> value="1"  ><?=$lang->getLang("Username")?></option>
                                                        <option <?=($searchBy==2 ? 'selected="selected"': '')?> value="2"  ><?=$lang->getLang("User Id")?></option>
                                                        <option <?=($searchBy==3 ? 'selected="selected"': '')?> value="3"  ><?=$lang->getLang("Hand Id")?></option>
                                                        <option <?=($searchBy==4 ? 'selected="selected"': '')?> value="4"  ><?=$lang->getLang("Ip")?></option>
                                                        <option <?=($searchBy==7 ? 'selected="selected"': '')?> value="7"  ><?=$lang->getLang("Partner User Id")?></option>
                                                        <option <?=($searchBy==8 ? 'selected="selected"': '')?> value="8"  ><?=$lang->getLang("Game Session Id")?></option>
                                                    </select>
                                                </div>
                                            </div>
                                            <div><div class='label label-inverse' id="searchLabel"><?=$lang->getLang("Search Username")?></div>
                                                <br />
                                                <div class="controls">
                                                    <div class="input-prepend">
                                                        <span class="add-on"><i class="icon-edit"></i></span>
                                                        <input type='text'  name='searchValue'  placeholder="<?=$lang->getLang('Search')?>" required="required" value="<?=$searchValue?>">
                                                    </div>
                                                </div>
                                            </div>
                                            <div><div class='label label-inverse'><?=$lang->getLang("Game")?></div>
                                                <br>
                                                <div class="controls">
                                                    <div class="input-prepend">
                                                        <span class="add-on"><i class="icon-globe"></i></span>
                                                        <select name="games" id="games" class="games">
                                                            <option <? if($games == 0){ ?> selected <? } ?> value="0"><?=$lang->getLang("All");?></option>
                                                            <?php
                                                            $sql = "Select gam_id , gam_name from game";
                                                            $rs=$dbh->exec($sql);
                                                            while($rs->hasNext()){
                                                                $row = $rs->next();
                                                                ?> <option <? if($games == $row["gam_id"]){ ?> selected <? } ?>  value='<?=$row["gam_id"]?>' ><?=$row["gam_name"]?></option><?php
                                                            }
                                                            ?>
                                                        </select>
                                                    </div>
                                                </div>
                                            </div>
                                            <div><div class='label label-inverse'><?=$lang->getLang("Include Totals")?></div>
                                                <br />
                                                <input type="radio" name="includetotals" <?=($includetotals=="2"? "checked" :"" )?> class="yes" id="yes"  value="2" />
                                                <input type="radio" name="includetotals" class="no" id="no" <?=($includetotals!="2"? "checked" :"" )?>  value="1" />
                                                <div class="switch">
                                                    <label class="labelYes" for="yes"><?=$lang->getLang("Yes")?></label>
                                                    <label class="labelNo" for="no"><?=$lang->getLang("No")?></label>
                                                    <span></span>
                                                </div>
                                            </div>
                                        </div>

                                    </td>
                                    <td style="border: 0;" id="dateContainer">
                                        <div><div class='label label-inverse'><?=$lang->getLang("Date")?></div>
                                            <br>
                                            <div class="controls">
                                                <div class="input-prepend">
                                                    <span class="add-on"><i class="icon-calendar"></i></span>
                                                    <?createDateHelper("date_helper", SEARCH_MAX_HOURS, "from", "to", $name)?>
                                                </div>
                                            </div>
                                        </div>
                                        <div><div class='label label-inverse'><?=$lang->getLang("From")?></div>
                                            <br>
                                            <div class="controls">
                                                <div class="input-prepend">
                                                    <span class="add-on"><i class="icon-calendar"></i></span>
                                                    <input type='text' name='from' id='from'  value="<?=$from?>" >
                                                </div>
                                            </div>
                                        </div>
                                        <div><div class='label label-inverse'><?=$lang->getLang("Until")?></div>
                                            <br>
                                            <div class="controls">
                                                <div class="input-prepend">
                                                    <span class="add-on"><i class="icon-calendar"></i></span>
                                                    <input type='text' name='to' id='to'  value="<?=$to?>" >
                                                </div>
                                            </div>
                                        </div>
                                    </td>
                                </tr>
                            </table>
                        </div>
                        <br />
                        <br />
                        <input type="hidden" value="searchuserhands" name="action" />
                        <button class="btn btn-primary" id="searchHistoryHands" ><?=$lang->getLang("Search")?></button>
                        <br>
                    </form>
                    <?
                    if($section=='history') {
                        if (($end_time - $start_time) / 60 / 60 > SEARCH_MAX_HOURS) {
                            ?>
                            <h4 class="alert alert-error fade in">
                                <button type='button' class='close fade in' data-dismiss='alert'>&times;
                                </button><?= $lang->getLang('You have selected an interval too long'); ?></h4>
                            <?
                        } else {

                            switch ($action) {
                                case "searchuserhands" :
                                    if ($games == "5002") {
                                        searchuserhandsRoulette();
                                    } else {
                                        searchuserhands();
                                    }
                                    break;
                            }
                        }
                    }
                    ?>
                </div>
                <div class="tab-pane <?=($section=='realtime'?'fade-in active':'')?>" id="realtime" >
                    <form action="/history_hands" method="post" id="searchuserhand" name="searchuserhand">
                        <input type="hidden" name="section" value="realtime">
                        <input type="hidden" name="jurisdiction_id" id="jurisdiction_id" value="n<?=$jurisdiction_id?>">
                        <input type="hidden" name="jurisdiction_class" id="jurisdiction_class" value="<?=$jurisdiction_class?>">
                        <table>
                            <tr>
                                <td class="span4">
                                    <div class="contentAdmin">
                                        <div class="contentHeadAdmin"><?=$lang->getLang("Jurisdiction chooser")?></div>
                                        <div class="well">
                                            <div class="alert hidden alert-danger" id="jurAlert">
                                                <?=$lang->getLang("No jurisdiction found")?>
                                            </div>
                                            <label class="checkbox fleft toHide">
                                                <input type="checkbox" id="selectAll" value="1"><?=$lang->getLang("Select All")?>
                                            </label>
                                            <label class="checkbox fright toHide">
                                                <input type="checkbox" id="deselectAll" value="1"><?=$lang->getLang("Deselect All")?>
                                            </label>
                                            <input type="text" id='jurisdictionTreeSearch' style="float:left;padding: 5px;width: 94%" placeholder="<?=$lang->getLang("Search (min. 4 char)")?>">
                                            <div id="searchResult"></div>
                                            <div class="clearfix"></div>
                                            <br/>
                                            <div id="jurisdictionTree"></div>
                                        </div>
                                    </div>
                                </td>
                                <td>
                                    <div class="well">

                                        <div class="span9">

                                            <div class="span4"><div class='label label-inverse' ><?=$lang->getLang("Search Username")?></div>
                                                <br />
                                                <div class="controls">
                                                    <div class="input-prepend">
                                                        <span class="add-on"><i class="icon-edit"></i></span>
                                                        <input type='text'  name='username'  placeholder="<?=$lang->getLang('Search')?> (Optional)"  value="<?=$_POST['username']?>">
                                                    </div>
                                                </div>
                                            </div>
                                            <div  class="span2"><div class='label label-inverse'><?=$lang->getLang("Game")?></div>
                                                <br>
                                                <div class="controls">
                                                    <div class="input-prepend">
                                                        <span class="add-on"><i class="icon-globe"></i></span>
                                                        <select name="games" id="games" class="games">
                                                            <option <? if($games == 0){ ?> selected <? } ?> value="0"><?=$lang->getLang("All");?></option>
                                                            <?php
                                                            $sql = "Select gam_id , gam_name from game";
                                                            $rs=$dbh->exec($sql);
                                                            while($rs->hasNext()){
                                                                $row = $rs->next();
                                                                ?> <option <? if($games == $row["gam_id"]){ ?> selected <? } ?>  value='<?=$row["gam_id"]?>' ><?=$row["gam_name"]?></option><?php
                                                            }
                                                            ?>
                                                        </select>
                                                    </div>
                                                </div>
                                            </div>
                                            <div  class="span2"><div class='label label-inverse'><?=$lang->getLang("Group By")?></div>
                                                <br>
                                                <div class="controls">
                                                    <div class="input-prepend">
                                                        <span class="add-on"><i class="icon-globe"></i></span>
                                                        <select name="groupBy" >
                                                            <!--<option value="">None</option>-->
                                                            <option value="player" <?=($groupBy=='player'? 'selected' : '')?>  >Player</option>
                                                            <option value="game" <?=($groupBy=='game'? 'selected' : '')?> >Game</option>
                                                        </select>
                                                    </div>
                                                </div>
                                            </div>

                                        </div>
                                        <div class="clearfix"></div>
                                        <button class="btn btn-primary" ><?=$lang->getLang("Search")?></button>
                                    </div>
                                    <?if($section=='realtime'){?>
                                    <?=getRealTime($jurisdiction_id,$jurisdiction_class,$groupBy,$_POST['username'],$_POST['games'],$array_for_query)?>
                                    <? } ?>
                                </td>
                            </tr>
                        </table>

                    </form>

                </div>
            </div>

        </td>

    </tr>
</table>





<?
function searchuserhandsRoulette(){

    global $dbh, $searchBy, $searchValue, $from, $to,  $handid, $games,$lang;
    $paramCount  = 0;

    $fieldFromDate = $from;
    $fieldToDate   = $to;


    if(empty($searchValue)){
        ?>
        <h4 class="alert alert-error fade in"><button type='button' class='close fade in' data-dismiss='alert'>&times;</button><?=$lang->getLang('Username or user id or hand id required');?></h4>
        <?
    }


    $sql = "SELECT res_id, ctr_id, ctr_pun_id, pun_username, pre_time, ctr_amount, ctr_balance_available, gam_name, ctr_tty_id, ctr_notes
      FROM customer_transaction
      JOIN
        result on res_id = ctr_res_id
      JOIN
        punter on pun_id = ctr_pun_id
      JOIN
        game on gam_id = res_gam_id
      WHERE
        ctr_pun_id = (SELECT pun_id from punter where pun_id=$searchValue OR pun_customer_number = $searchValue)
      AND res_id = ctr_res_id
      AND res_gam_id = 5002
      AND (ctr_tty_id = 12 OR ctr_tty_id = 13)
      AND ctr_time > DATE_FORMAT(  '$fieldFromDate', '%Y-%m-%d %H%i' )
      AND ctr_time < DATE_FORMAT(  '$fieldToDate', '%Y-%m-%d %H%i' )";


    $starttime = microtime(true);
    $rs=$dbh->exec($sql);
    $endtime = microtime(true);
    $duration = $endtime - $starttime;
    $duration=number_format($duration, 4, ',', '')."s";
    $numrow=$rs->getNumRows();
    if($numrow!=0){
        ?>
        <div class="tip fleft"><?=$lang->getLang("Time taken to execute your request")?>: <?=$duration?></div><br /> <br />
        <table class='table display table-hover table-striped table-bordered' id="historyTable" >
            <thead>
            <tr>
                <th style="white-space: nowrap"><?=$lang->getLang("Hand ID")?></th>
                <th><?=$lang->getLang("Date")?></th>
                <th style="white-space: nowrap"><?=$lang->getLang("User Name")?></th>
                <th><?=$lang->getLang("Amount")?></th>
                <th><?=$lang->getLang("Balance")?></th>
                <th><?=$lang->getLang("Game")?></th>

            </tr>
            </thead>
            <tbody>
            <?
            while($rs->hasNext()){
                $rec=$rs->next();
                ?>

                <tr>
                    <td>

                        <?=getClassLink("javascript:openWindow('/casino_games/hand_details_roulette.php?gmn={$rec["ctr_id"]}&uid={$rec["ctr_pun_id"]}&hand_id={$rec["res_id"]}','hand{$rec["res_id"]}','900','980','0','1')", $rec["res_id"], "contentlink");?>
                    </td>
                    <td><?=($rec["pre_time"])?></td>
                    <td><?=getClassLink(refPage('customers/customer_view&customer_id='.$rec["ctr_pun_id"]), $rec["pun_username"])?></td>
                    <td><?=getInDollars($rec["ctr_amount"]);?></td>
                    <td><?=getInDollars($rec["ctr_balance_available"]);?></td>
                    <td><?=getInDollars($rec["gam_name"]);?></td>
                </tr>
                <?
            }
            ?>
            </tbody>
        </table>
        <?
    }
    else{
        ?>
        <h4 class="alert alert-error fade in"><button type='button' class='close fade in' data-dismiss='alert'>&times;</button><?=$lang->getLang('No result found');?></h4>
        <?
    }

}

function searchuserhands(){
    global $dbh, $searchBy, $searchValue, $from, $to,  $handid, $games,$lang,$includetotals;
    $paramCount  = 0;

    $fieldFromDate = $from;
    $fieldToDate   = $to;


    if(empty($searchValue)){
        ?>
        <h4 class="alert alert-error fade in"><button type='button' class='close fade in' data-dismiss='alert'>&times;</button><?=$lang->getLang('Username or user id or hand id required');?></h4>
        <?
    }
    $jur_id = $_SESSION["jurisdiction_id"];
    //, tbl_id, tbl_name, tth_maxplayersnum

    if(EXT_PARTNER_ID>0){


        $sql="SELECT pun_username, PRE_RES_ID, pre_game_num, PRE_PUN_ID, PRE_BET, PRE_WIN, PRE_RAKE, PRE_BALANCE, PRE_TBL_ID, PRE_PUN_IP, pre_bet_string ,res_date, res_id, pre_time,  pre_balance, (select gam_name from game where res_gam_id = gam_id) gam_name ";
        if(TAX_CALCULATION_ACTIVE=='ON'){
            $sql.=" ,pre_bet_tax ";
        }
        $sql.="    FROM ext_punter_result, punter  ";
        if($_SESSION["jurisdiction_class"] != "casino"){
            $sql .= " LEFT JOIN jurisdiction club     ON club.jur_id     = pun_betting_club
  		       LEFT JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
             LEFT JOIN jurisdiction regional ON regional.jur_id = district.jur_parent_id
             LEFT JOIN jurisdiction nation   ON nation.jur_id   = regional.jur_parent_id";
        }
        if($searchBy==7){
            $sql.=" JOIN partner_users on pnu_local_id=pun_id";
        }
        $sql.="WHERE pre_time BETWEEN '$fieldFromDate' AND '$fieldToDate' ";



        switch($_SESSION["jurisdiction_class"]){
            case "club":
                $sql .= " AND club.jur_id = $jur_id";
                break;
            case "district":
                $sql .= " AND district.jur_id = $jur_id";
                break;
            case "region":
                $sql .= " AND regional.jur_id = $jur_id";
                break;
            case "nation":
                $sql .= " AND  nation.jur_id = $jur_id";
                break;
            case "casino":
                $sql .= " ";
                break;
            default:
                $sql .= " WHERE 1=0";
                break;
        }

        $sql.=" AND pun_customer_number=pre_pun_id ";

        if($searchBy==1){
            $sql .= " AND pun_username=".$dbh->prepareString($searchValue);
        }
        elseif($searchBy==2){
            $sql .= " AND pun_customer_number=$searchValue ";
        }
        elseif($searchBy==3){
            $sql .= " AND res_id= $searchValue ";
        }
        elseif($searchBy==4){
            $sql .= " AND pre_pun_ip= ".$dbh->prepareString($searchValue);
        }
        elseif($searchBy==7){
            $sql .= " AND pnu_id= ".$dbh->escape($searchValue);
        }
        elseif($searchBy==8){
            $sql .= " AND pre_external_id= ".$dbh->prepareString($searchValue);
        }
        $sql.="  ORDER BY pre_time , pre_game_num ASC";
    }
    else{

        $sql = "SELECT pun_username, PRE_RES_ID, pre_game_num, PRE_PUN_ID, PRE_BET,PUN_CUSTOMER_NUMBER,PUN_ID,res_date,
      	    PRE_WIN, PRE_RAKE, PRE_BALANCE, PRE_TBL_ID, PRE_PUN_IP, pre_bet_string , res_id,PRE_VOID_DETAILS,
      	pre_time,gam_id,
      	    gam_name, pre_balance, cur_code_for_web  ";
        if(TAX_CALCULATION_ACTIVE=='ON'){
            $sql.=" ,pre_bet_tax ";
        }
        $sql.="      	 FROM
      	   punter_result ";

        if($searchBy!=3){
            $sql .=" force index (pre_time) ";
        }
        $sql .=" JOIN
      	   punter on pun_id = pre_pun_id
      	 JOIN
      	   result on res_id = pre_res_id
      	 JOIN
      	   game on gam_id = res_gam_id
      	   JOIN jurisdiction club     ON club.jur_id     = pun_betting_club
      	   JOIN currency on club.jur_currency=cur_id

      	   ";
        if($searchBy==7){
            $sql.=" JOIN partner_users on pnu_local_id=pun_id";
        }
        if($_SESSION["jurisdiction_class"] != "casino"){
            $sql .= "
  		       JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
             JOIN jurisdiction regional ON regional.jur_id = district.jur_parent_id
             JOIN jurisdiction nation   ON nation.jur_id   = regional.jur_parent_id ";
        }

        switch($_SESSION["jurisdiction_class"]){
            case "club":
                $sql .= " WHERE club.jur_id = $jur_id";
                break;
            case "district":
                $sql .= " WHERE district.jur_id = $jur_id";
                break;
            case "region":
                $sql .= " WHERE regional.jur_id = $jur_id";
                break;
            case "nation":
                $sql .= " WHERE nation.jur_id = $jur_id";
                break;
            case "casino":
                $sql .= " WHERE 1=1";
                break;
            default:
                $sql .= " WHERE 1=0";
                break;
        }
        if($searchBy!=3){
            if($searchBy==1){
                $sql .= " AND pun_username=".$dbh->prepareString($searchValue);
            }
            if($searchBy==2){
                if(!is_numeric($searchValue)){

                    die("<h4 class='alert alert-error fade in'><button type='button' class='close fade in' data-dismiss='alert'>&times;</button>".$lang->getLang('Invalid user id')."</h4>");
                }
                $sql .= "  AND pun_customer_number=$searchValue ";
            }
            if($searchBy==4){
                $sql .= " AND pre_pun_ip= ".$dbh->prepareString($searchValue);
            }
            if($searchBy==7){
                $sql .= " AND pnu_id= ".$dbh->escape($searchValue);
            }
            if($searchBy==8){
                $sql .= " AND pre_external_id= ".$dbh->prepareString($searchValue);
            }

            $sql .= " AND pre_time BETWEEN '$fieldFromDate' AND '$fieldToDate' ";
        }else{

            if(!is_numeric($searchValue)){

                die("<h4 class='alert alert-error fade in'><button type='button' class='close fade in' data-dismiss='alert'>&times;</button>".$lang->getLang('Invalid hand id')."</h4>");
            }
            $sql .= " AND result.res_id= $searchValue";
        }

        if($games != 0){
            $sql .= " AND res_gam_id = $games";
        }

        $sql .= " ORDER BY pre_time ASC";
    }
//die($sql);
    $starttime = microtime(true);
    $rs=$dbh->exec($sql);
    $endtime = microtime(true);
    $duration = $endtime - $starttime;
    $duration=number_format($duration, 4, ',', '')."s";
    $numrow=$rs->getNumRows();
    if($numrow!=0){ ?>

        <div class="tip fleft"><?=$lang->getLang("Time taken to execute your request")?>: <?=$duration?></div><br /> <br />
        <?
        $return="<table class='table display table-hover table-striped table-bordered' id='historyTable' >
    <thead>
    <tr>
        <th style='white-space: nowrap'>".$lang->getLang("Hand ID")."</th>
        <th style='white-space: nowrap'>".$lang->getLang("Game Nr")."</th>
        <th>".$lang->getLang("Date")."</th>
        <th style='white-space: nowrap'>".$lang->getLang("User Name")."</th>
        <th>".$lang->getLang("Bet")."</th>";

        if(TAX_CALCULATION_ACTIVE=='ON'){
            $return.='<th style="white-space: nowrap">'.$lang->getLang("Bet Tax").'</th>';
        }

        $return.="<th>".$lang->getLang("Win")."</th>
        <th>".$lang->getLang("Net")."</th> 
        <th>".$lang->getLang("Rake")."</th>       
        <th>".$lang->getLang("Balance")."</th>   
        <th>".$lang->getLang("Balance Modifed")."</th>
        <th>".$lang->getLang("Game")."</th>
        <th>".$lang->getLang("Ip")."</th>
        <th>".$lang->getLang("Refund")."</th>
    </tr>
    </thead>
    <tbody>";

        
        $cashIn=0;
        $cashOut=0;
        $cashRake=0;
        while($rs->hasNext()){
            $rec=$rs->next();
            $cashOut += $rec['pre_win'];
            $cashIn += $rec['pre_bet'];
            $cashRake += $rec['pre_rake'];

            $return .="<tr>
            <td>";
            if (strpos($rec['pre_bet_string'],'Refund') !== false) {
                $return.=$rec["res_id"];
            }
            else{
                $return.=getClassLink("javascript:openWindow('/casino_games/hand_details.php?gmn={$rec["pre_game_num"]}&uid={$rec["pre_pun_id"]}&hand_id={$rec["res_id"]}&curr={$rec["cur_code_for_web"]}','hand{$rec["pre_res_id"]}','950','980','0','1')", $rec["res_id"], "contentlink");
            }
            $return.='<br /><span class="tip unwrappable">('.$rec["res_date"].')</span>
    </td>
    <td>';
            if (strpos($rec['pre_bet_string'],'Refund') !== false) {
                $return .=$rec['pre_game_num'];
            }
            else{

                $return .=getClassLink("javascript:openWindow('/casino_games/hand_details.php?gmn={$rec["pre_game_num"]}&uid={$rec["pre_pun_id"]}&hand_id={$rec["res_id"]}&curr={$rec["cur_code_for_web"]}','hand{$rec["pre_res_id"]}','950','980','0','1')", $rec["pre_game_num"], "contentlink");
            }
            $return.="</td>
           <td class='unwrappable'>".($rec["pre_time"])."</td>
    <td>".getClassLink("javascript:openWindow('".refPage("customers/customer_view&customer_id={$rec["pre_pun_id"]}")."','user{$rec["pun_username"]}','950','980','0','1')", $rec["pun_username"], "contentlink").
                "<td>".getInDollars($rec["pre_bet"],2,$rec['cur_code_for_web'])."</td>";

            if(TAX_CALCULATION_ACTIVE=='ON'){
                $return.=" <td>".($rec['pre_bet_tax'] !='' ? getInDollars(($rec['pre_bet']- $rec['pre_bet_tax']),3)."<br />
                <span class='tip'>Tax : ".getInDollars($rec['pre_bet_tax'],3)."</span>" : '--' )."</td>";
            }

            $return.="<td>".getInDollars($rec["pre_win"])."</td>";
            $return.="<td>".getInDollars($rec["pre_bet"]-$rec["pre_win"])."</td>";
            $return.="<td>".getInDollars($rec["pre_rake"])."</td>";
            $return.="<td>".getInDollars($rec["pre_balance"])."</td>";
            $return.="<td>".getInDollars($rec["pre_balance"] + ($rec["pre_win"] - $rec["pre_bet"]))."</td>";
            $return.="<td class='unwrappable'>".getGamesName($rec["gam_name"])."</td>
        <td>".getClassLink("javascript:openWindow('".refPage('customers/search&field=4&search_string=' . urlencode(ereg_replace("[^0-9\.]", "", $rec["pre_pun_ip"])), $rec["pre_pun_ip"])."','ip{$rec["pre_pun_ip"]}','950','980','0','1')", $rec["pre_pun_ip"], "contentlink").
                "</td>
    <td>";
            if ($rec["pre_void_details"]==''){
                if(check_access('transactions_game_refund')){

                    $return.= '<a href="#myModal" class="solveTicket btn btn-info btn-small" style="color:white" data-toggle="modal" >'.$lang->getLang("Refund").'</a>';
                }
                else{
                    $return.=$lang->getLang('No action available');
                }
            }
            else {
                $return.=$rec["pre_void_details"];
            }
            $return.='<div class="hidden cstnr">'.$rec['pun_customer_number'].'</div>
              <div class="hidden cstusr">'.$rec['pun_id'].'</div>
              <div class="hidden cstgame">'.$rec['gam_id'].'</div>
              </td>
             </tr>';
        }
        $return.='    </tbody>
    </table>';

    }
    else{
        $return="<h4 class='alert alert-error fade in'><button type='button' class='close fade in' data-dismiss='alert'>&times;</button>".$lang->getLang('No result found')."</h4>";
    }
    if($includetotals=='2') {
        ?>

        <table class="fleft table  table-bordered">
            <thead>
            <tr>
                <th style="font-size: 15px;border-radius:3px">Total Bet</th>
                <th style="font-size: 15px;border-radius:3px">Total Win</th>
                <th style="border-radius:3px; font-size: 15px">Net</th>
                <th style="font-size: 15px;border-radius:3px">Total Rake</th>
            </tr>
            </thead>
            <tr>
                <td style="font-size: 15px"><?= getInDollars($cashIn) ?></td>
                <td style="font-size: 15px"><?= getInDollars($cashOut) ?></td>
                <td style="font-size: 15px"><?= getInDollars($cashIn - $cashOut) ?></td>
                <td style="font-size: 15px"><?= getInDollars($cashRake) ?></td>
            </tr>
        </table>
        <?
    }
    echo $return;
}

setupDateInputJs("from", "to");


function getRealTime($jurisdiction_id,$jurisdiction_class,$groupBy,$player=false,$game=false,$array_for_query){
    global $dbh,$lang;
    $sql="SELECT sum(pre_win) pre_win,sum(pre_bet) pre_bet, SUM(pre_rake) pre_rake, pre_bet_string,res_id,pre_game_num,pre_pun_id,pre_res_id,res_date,pre_pun_ip,pun_username,pre_time, gam_name 
        FROM result 
        JOIN punter_result on res_id = pre_res_id
        JOIN punter ON pre_pun_id=pun_id
        JOIN game on res_gam_id=gam_id 
         JOIN jurisdiction club ON club.jur_id  = pun_betting_club
  		        JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
                JOIN jurisdiction region ON region.jur_id = district.jur_parent_id
                JOIN jurisdiction nation   ON nation.jur_id   = region.jur_parent_id ";

    switch($_SESSION["jurisdiction_class"]){
        case "club":
            $sql .= " WHERE club.jur_id = ".$_SESSION['jurisdiction_id'];
            break;
        case "district":
            $sql .= " WHERE district.jur_id = ".$_SESSION['jurisdiction_id'];
            break;
        case "region":
            $sql .= " WHERE region.jur_id = ".$_SESSION['jurisdiction_id'];
            break;
        case "nation":
            $sql .= " WHERE nation.jur_id = ".$_SESSION['jurisdiction_id'];
            break;
        case "casino":
            $sql .= " WHERE 1=1";
            break;
        default:
            $sql .= " WHERE 1=0";
            break;
    }

    $where_jur_class = '';

    if(!empty($array_for_query)){
        foreach ($array_for_query as $class => $ids){
            switch($class){
                case "club":
                    if($where_jur_class == '') {
                        $where_jur_class = " AND (";
                        $where_jur_class .= " club.jur_id in (" . $ids . ")";
                    } else {
                        $where_jur_class .= " OR club.jur_id in (" . $ids . ")";
                    }
                    break;
                case "district":
                    if($where_jur_class == '') {
                        $where_jur_class = " AND (";
                        $where_jur_class .= " district.jur_id in (" . $ids . ")";
                    } else {
                        $where_jur_class .= " OR district.jur_id in (" . $ids . ")";
                    }
                    break;
                case "region":
                    if($where_jur_class == '') {
                        $where_jur_class = " AND (";
                        $where_jur_class .= " region.jur_id in (".$ids.")";
                    } else {
                        $where_jur_class .= " OR region.jur_id in (" . $ids . ")";
                    }
                    break;
                case "nation":
                    if($where_jur_class == '') {
                        $where_jur_class = " AND (";
                        $where_jur_class .= " nation.jur_id in (" . $ids . ")";
                    } else {
                        $where_jur_class .= " OR nation.jur_id in (" . $ids . ")";
                    }
                    break;
            }
        }
        if($where_jur_class != '') $where_jur_class .= " )";
    }

    $sql .= $where_jur_class;

    switch($jurisdiction_class){
        case "club":
            $sql .= " AND club.jur_id = ".$jurisdiction_id;
            break;
        case "district":
            $sql .= " AND district.jur_id = ".$jurisdiction_id;
            break;
        case "region":
            $sql .= " AND region.jur_id = ".$jurisdiction_id;
            break;
        case "nation":
            $sql .= " AND nation.jur_id = ".$jurisdiction_id;
            break;
    }
   $sql.=" AND res_date between concat(DATE_FORMAT(now(), '%Y-%m-%d'), ' 00:00:00') AND now() " ;

    if($player){
        $sql.=" AND pun_username=".$dbh->prepareString($player);
    }
    if($game && $game!='0' ){
        $sql.=" AND res_gam_id=".$dbh->escape($game);
    }
    if($groupBy=='player'){
        $sql.=" Group by pun_id";
    }
    else if($groupBy=='game'){
        $sql.=" Group by gam_id";
    }
       /* AND res_gam_id = ?
        AND pre_pun_id = ?");*/
    
    $starttime = microtime(true);
    $result=$dbh->doCachedQuery($sql,0);
    $endtime = microtime(true);
    $duration = $endtime - $starttime;
    $duration=number_format($duration, 4, ',', '')."s";


    if($result->getNumRows()!=0){ ?>

        <div class="tip fleft"><?=$lang->getLang("Time taken to execute your request")?>: <?=$duration?></div><br /> <br />
        <?
        $return="<table class='table display table-hover table-striped table-bordered'>
    <thead>
    <tr>
       
        <th>".$lang->getLang("Date")."</th>";
        if($groupBy=='player') {
            $return.="<th style='white-space: nowrap'>".$lang->getLang("Player")."</th>";
            }
        $return.="<th>".$lang->getLang("Bet")."</th>
                 <th>".$lang->getLang("Win")."</th>
                <th>".$lang->getLang("Net")."</th>
                <th>".$lang->getLang("Rake")."</th>";
        if($groupBy=='game') {
          $return.="  <th> ".$lang->getLang("Game")." </th >";
            }
        if($groupBy=='player') {
            $return.="<th>".$lang->getLang("Ip")."</th>";
        }
        $return.="</tr>
    </thead>
    <tbody>";

        $cashIn=0;
        $cashOut=0;
        $cashRake=0;
        while($result->hasNext()){
            $rec=$result->next();
            $cashOut += $rec['pre_win'];
            $cashIn += $rec['pre_bet'];
            $cashRake += $rec['pre_rake'];

            $return .="<tr>
                    <td class='unwrappable'>".($rec["pre_time"])."</td> ";

            if($groupBy=='player') {
                $return.="<td><button type='button' class='btn btn-small btn-default viewRealtimePlayer'>". $rec["pun_username"]."</button></td>";

            }
                $return.= "<td>".getInDollars($rec["pre_bet"],2,$rec['cur_code_for_web'])."</td>";

            $return.="<td>".getInDollars($rec["pre_win"])."</td>";
            $return.="<td>".getInDollars($rec["pre_bet"]-$rec["pre_win"])."</td>";
            $return.="<td>".getInDollars($rec["pre_rake"])."</td>";
            if($groupBy=='game') {
                $return.="<td class='unwrappable'>".getGamesName($rec["gam_name"])."</td>";
            }
            if($groupBy=='player') {
                $return.=" <td>".getClassLink("javascript:openWindow('".refPage('customers/search&field=4&search_string=' . urlencode(ereg_replace("[^0-9\.]", "", $rec["pre_pun_ip"])), $rec["pre_pun_ip"])."','ip{$rec["pre_pun_ip"]}','950','980','0','1')", $rec["pre_pun_ip"], "contentlink").
                    "</td>";
            }
             $return.="</tr>";
        }
        $return.='    </tbody>
    </table>';

    }
    else{
        $return="<h4 class='alert alert-error fade in'><button type='button' class='close fade in' data-dismiss='alert'>&times;</button>".$lang->getLang('No result found')."</h4>";
    }
        ?>

        <table class="fleft table  table-bordered">
            <thead>
            <tr>
                <th style="font-size: 15px;border-radius:3px">Total Bet</th>
                <th style="font-size: 15px;border-radius:3px">Total Win</th>
                <th style="border-radius:3px; font-size: 15px">Net</th>
                <th style="font-size: 15px;border-radius:3px">Total Rake</th>
            </tr>
            </thead>
            <tr>
                <td style="font-size: 15px"><?= getInDollars($cashIn) ?></td>
                <td style="font-size: 15px"><?= getInDollars($cashOut) ?></td>
                <td style="font-size: 15px"><?= getInDollars($cashIn - $cashOut) ?></td>
                <td style="font-size: 15px"><?= getInDollars($cashRake) ?></td>
            </tr>
        </table>
        <?

    echo $return;



}

?>
<!--
<script type="text/javascript">
    function openLink(link){
        document.all.videoframe.src = link;
    }
</script>-->