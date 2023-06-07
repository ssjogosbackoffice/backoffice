<?php

//check_access('manage_jackpot', true);
check_access('manage_jackpot', true);
require_once 'RedisClient.php';

$redisClient=new RedisClient(REDISSERVER,REDISPORT,$dbLog);

if(isset($_REQUEST['refresh'])){
    $reddisKeyword = "GAMECASHIER-".$_REQUEST['refresh'];
    $reddisData = $redisClient->get($reddisKeyword);
    die($reddisData);
}
if(isset($_REQUEST['pk'])){
    $name = $_REQUEST['name'];
    $id = $_REQUEST['pk'];
    $value = $_REQUEST['value'];

    $reddisKeyword = "GAMECASHIER-".$id;
    $reddisData = $redisClient->get($reddisKeyword);
    if($reddisData != NULL) {
        $result = json_decode($reddisData, true);

        if ($name == 'amount') {
            $result['amount'] = $value;
        }
        if ($name == 'perc') {
            $result['perc'] = $value;
        }

        $dataJson = json_encode($result);
        $redisResponse = $redisClient->set($reddisKeyword, $dataJson);
        die('success');
    } else {
        die('error');
    }
}
// lists the table in datatables format
if(isset($_GET['listGameCashier'])){

    $sql = "SELECT gc.*, g.gam_name, g.gam_category, j.jur_name, j.jur_class
            FROM egamessystem.game_cashier gc, egamessystem.game g, egamessystem.jurisdiction j
            WHERE gc.gch_gam_id = g.gam_id
            AND gc.gch_jur_id = j.jur_id";

    $dbh = new DatabaseHandler();
    $dbh->connect();

    $results = $dbh->doCachedQuery($sql, 0);
    $totals = $results->NumRows;
    $tableArr = array();

    while ($results->hasNext()){
        $i=0;
        $row = $results->next();
        $section = array();

        $section[$i++] = $row['gch_gam_id'];
        $section[$i++] = $row['gch_name'];
        $section[$i++] = "<span class='creation'>".$row['gch_creation']."</span> / <span class='last'>".$row['gch_lastupdate']."</span>";
        $section[$i++] = "<span class='editable'><a href='#' data-name='amount' data-pk='".$row['gch_jur_id']."-".$row['gch_gam_id']."' data-type='text'>".$row['gch_amount']."</a></span>";
        $section[$i++] = "<span class='editable'><a href='#' data-name='perc' data-pk='".$row['gch_jur_id']."-".$row['gch_gam_id']."' data-type='text'>".$row['gch_perc']."</a></span>";
        $section[$i++] = $row['jur_name'];
        $section[$i++] = $row['jur_class'];
        $section[$i++] = "<button type='button' class='btn btn-small refresh' data-id='".$row['gch_jur_id']."-".$row['gch_gam_id']."'><i style='padding-right: 5px;' class='icon-refresh'></i> Refresh</button>";
        array_push($tableArr,$section);
    }

    $output= array(
        "draw"            => intval($_GET['draw']),
        "recordsTotal"    => intval($totals),
        "recordsFiltered" => intval($totals),
        "data"            => $tableArr
    );
    die(json_encode($output));
}
?>

<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/js/sweetalert/sweetalert.css"  />
<script src="<?= secure_host ?>/media/js/sweetalert/sweetalert.min.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
<script src="../media/jquery.dataTables.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<link href="//cdnjs.cloudflare.com/ajax/libs/x-editable/1.5.0/bootstrap-editable/css/bootstrap-editable.css" rel="stylesheet"/>
<script src="//cdnjs.cloudflare.com/ajax/libs/x-editable/1.5.0/bootstrap-editable/js/bootstrap-editable.min.js"></script>
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<style>
    .paginate_button.current {
        background-color: #5dc2f1 !important;
    }
    .error_msg {
        width: 100%;
        background-color: #c8211e;
        color: #fff;
        height: 35px;
        border-radius: 5px;
        text-align: center;
        font-size: 14px;
        font-weight: bold;
        line-height: 35px;
    }
    .error_row{
        display: none;
    }
    .editable-buttons .btn {
        padding: 6px 8px !important;
    }
    .editable-buttons {
        margin-left: 3px !important;
    }
    .editable-buttons .editable-cancel{
        margin-left: 3px !important;
    }
</style>
<script>
    $( document ).ready(function() {

        // Initializing the datatable
        cashiertable = $('#cashiertable').DataTable({
            "processing": true,
            "serverSide": false,
            "bPaginate":false,
            "ajax": {
                "url": "casino_games/manage_cashier.inc",
                "data": function (d) {
                    d.listGameCashier = 1;
                }
            },
            "columnDefs": [
                { className: "date_r", "targets": [ 2 ] },
                { className: "amount_r", "targets": [ 3 ] },
                { className: "rtp_r", "targets": [ 4 ] }
            ],
            "initComplete": function (data) {
                $.fn.editable.defaults.mode = 'inline';
                init_editable();
            },
        });

        function init_editable(){
            $('.editable a').each(function () {
                var name = $(this).attr('data-name');
                $(this).editable({
                    url: "casino_games/manage_cashier.inc",
                    type: 'inline',
                    name: name,
                    inputclass: 'input-mini',
                    success: function(data) {
                        if(data == 'error') {
                            $('tr .error_row').show();
                            setTimeout(function(){ $('tr .error_row').fadeOut(500) }, 3000);
                        }
                    }
                });
            });
        }

        init_editable();
        $(document).off('click','.refresh');
        $(document).on('click','.refresh', function () {
            var btn = $(this);
            $.ajax({
                type: 'POST',
                url: "casino_games/manage_cashier.inc",
                data: {
                    'refresh' : $(this).attr('data-id')
                },
                success: function (data) {
                    if(data) {
                        console.log(data);
                        var data = $.parseJSON(data);
                        var last_update = format_date(data.lastUpdate);
                        btn.closest('tr').find('.last').text(last_update);
                        btn.closest('tr').find('.amount_r').html("<span class='editable'><a href='#' data-name='amount' data-pk='"+data.jurId+'-'+data.gameId+"' data-type='text'>"+data.amount+"</a></span>");
                        btn.closest('tr').find('.rtp_r').html("<span class='editable'><a href='#' data-name='perc' data-pk='"+data.jurId+'-'+data.gameId+"' data-type='text'>"+data.perc+"</a></span>");
                        init_editable();
                    } else {
                        $('tr .error_row').show();
                        setTimeout(function(){ $('tr .error_row').fadeOut(500) }, 3000);
                    }
                }
            });
        });

        function format_date(timestamp) {
            var date = new Date(timestamp);
            month=date.getMonth();
            month=month+1; //javascript date goes from 0 to 11
            if (month<10) month="0"+month; //adding the prefix

            year=date.getFullYear();
            day=date.getDate();
            hour=(date.getHours()<10?'0':'')+date.getHours();
            minutes=(date.getMinutes()<10?'0':'')+date.getMinutes();
            seconds=(date.getSeconds()<10?'0':'')+date.getSeconds();

            return year+"-"+month+"-"+day+" "+hour+":"+minutes+":"+seconds;

        }

        $(document).off('click','.tabs');
        $(document).on('click','.tabs',function () {
            var link = $(this).attr('href');
            if(link == '#user_c' && !$('#userDetails tbody').length){
                userDetails = $('#userDetails').DataTable({
                    "processing": true,
                    "serverSide": true,
                    "ajax": {
                        "url": "casino_games/userConnected.inc",
                        "data": function (d) {
                            d.listDetails = "1";
                        }
                    },
                    'sPaginationType': "full_numbers"
                });
            }
            if(link == '#jackpot_m' && !$('#jackpotTable tbody').length){
                jackpotTable = $('#jackpotTable').DataTable({
                    "processing": true,
                    "serverSide": true,
                    "ajax": {
                        "url": "casino_games/jackpotManagement.inc",
                        "data": function (d) {
                            d.listJackpots = "1";
                        }
                    },
                    'sPaginationType': "full_numbers",
                    "columnDefs": [
                        { "orderable": false, "targets": 6 }
                    ]
                });
            }
        });
    });

</script>

<ul class="nav nav-tabs" id='myTab'>
    <li class="show active"><a href="#game_cashier" class="tabs" data-toggle="tab"><?=$lang->getLang("Game Cashier")?></a></li>
    <li class=""><a href="#jackpot_m" class="tabs" data-toggle="tab"><?=$lang->getLang("Jackpot Management")?></a></li>
    <li class=""><a href="#user_c" class="tabs" data-toggle="tab"><?=$lang->getLang("User Connected")?></a></li>
</ul>
<div class="tab-content" style="width:100%;">
    <div class="tab-pane show active" id="game_cashier">
        <table class="table table-bordered" style="width: calc(100vw - 200px);"">
        <tr>
            <td class="navbar-inner" colspan="2"><h3 class="text-center"><?=$lang->getLang("Game Cashier")?></h3></td>
        </tr>
        <tr class="error_row">
            <td>
                <div class="error_msg">
                    ERROR
                </div>
            </td>
        </tr>
        <tr>
            <td>
                <table id="cashiertable" class='display table table-striped table-hover table-bordered table-condensed dataTable no-footer' style="width: 100%">
                    <thead>
                    <tr>
                        <th><?=$lang->getLang("Game id")?></th>
                        <th><?=$lang->getLang("Game name")?></th>
                        <th><?=$lang->getLang("Creation / Last Update")?></th>
                        <th><?=$lang->getLang("Amount")?></th>
                        <th><?=$lang->getLang("RTP %")?></th>
                        <th><?=$lang->getLang("Jurisdiction")?></th>
                        <th><?=$lang->getLang("Jur Class")?></th>
                        <th><?=$lang->getLang("Action")?></th>
                    </tr>
                    </thead>
                </table>
            </td>
        </tr>
        </table>
    </div>
    <div class="tab-pane" id="jackpot_m">
        <? include('jackpotManagement.inc'); ?>
    </div>
    <div class="tab-pane" id="user_c">
        <? include('userConnected.inc'); ?>
    </div>
</div>
