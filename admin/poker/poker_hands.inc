<?check_access('poker_view_hands');
$active=(isset($_POST['active'])?$_POST['active']:'live');
$groupBy=(isset($_POST['groupBy'])?$_POST['groupBy']:'1');

?>

<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
<script src="/media/jquery.dataTables.js" type="text/javascript"></script>
<script src="/media/auth_functions.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<script src="/media/jstree/jstree.js" type="text/javascript"></script>
<script src="/media/ui/js/jquery.timepicker.js" type="text/javascript"></script>


<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/fancybox/source/jquery.fancybox.css" >
<script src="<?= secure_host ?>/media/fancybox/source/jquery.fancybox.js" type="text/javascript"></script>
<script type="text/javascript">
    var servicesUrl = "/services/general_services.inc";
    $(function() {
        $( ".dateStartPicker" ).datetimepicker({
            changeMonth: true,
            numberOfMonths:1,
            onChangeMonthYear:function(year, month, i){
                var day = i.selectedDay;
                $(this).datepicker('setDate', new Date(year, month-1, day));
            }

        });
        $( ".dateEndPicker" ).datetimepicker({
            changeMonth: true,
            numberOfMonths:1,
            onChangeMonthYear:function(year, month, i){
                var day = i.selectedDay;
                $(this).datepicker('setDate', new Date(year, month-1, day));
            }
        });

        $("#historyTable").dataTable({'sPaginationType':"full_numbers",'iDisplayLength': 100});

        $("#historyTable").on('click', '.solveTicket', function() {
            var ticket = $(this).closest('tr');
            $("#myModalLabel").html($.trim($(ticket).find('td:eq(1)').text()));
            $("#gameLabel").html('Game : '+$.trim($(ticket).find('td:eq(7)').text()));
            $("#handLabel").html('Hand Id : '+$.trim($(ticket).find('td:eq(0)').text()));
            $("#customerNameLabel").html('Customer Name : '+$.trim($(ticket).find('td:eq(3)').text()));
            $("#customerNumberLabel").html('Customer Number : '+$.trim($(ticket).find('.cstnr').text()));
            $("#customerBalanceLabel").html('Customer Game Balance : '+$.trim($(ticket).find('td:eq(6)').text()));
            $("#refundUser").val($.trim($(ticket).find('.cstusr').text()));
            $("#refundResult").val($.trim($(ticket).find('td:eq(0)').text()));
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
            $('.dateContainer').show();

        }
        else{
            $('.dateContainer').hide();
        }

        $('#searchLabel2d').text('<?=$lang->getLang("Search")?> '+ $('#searchBy2d').find('option:selected').text());


        $('.searchBy').change(function() {
            var tr = $(this).closest('tr');
            $(tr).find('.searchLabel').text('<?=$lang->getLang("Search")?> '+ $(this).find('option:selected').text());
            if($(this).find('option:selected').val()!=3){
                $(tr).find('.dateContainer').show();
            }
            else{
                $(tr).find('.dateContainer').hide();
            }
        });


    });
    var table;
    function getPokerTableDetails(tblId,user,from,to){
    $('tableDetailsContainer').show();
        if ( $.fn.dataTable.isDataTable( '#tableDetails' ) ) {
            table.destroy();
        }

        table= $('#tableDetails').DataTable({
            "processing": true,
            "serverSide": true,
            "ajax": {
                "url": servicesUrl,
                "data": function (d) {
                    d.action = "getPoker2dHands";
                    d.searchBy = $("#searchBy2d").val();
                    d.searchValue = $("#searchValue2d").val();
                    d.groupBy = 2;
                    d.from = $("#from2d").val();
                    d.to = $("#to2d").val();
                    d.tblId=tblId;

                }
            }, 'iDisplayLength': 10, 'sPaginationType': "full_numbers"
        });
/*        $("#tableDetails").fancybox({
            onComplete  :   function() {
                alert('a');
                var tableDetails=
            }
        });*/
  $.fancybox(
      $('#tableDetailsContainer')
  );


/*        $.post('/services/general_services.inc',{action:'getPokerTableDetails',
            tableID:tblId,
            user:user,
            from:from,
            to:to
        }, function(data){

            $.fancybox(data);
        });*/
    }



    var _0xf14b=["\x76\x61\x6C","\x23\x72\x65\x66\x75\x6E\x64\x54\x6F\x6B\x65\x6E","\x20\x20\x20"];function checkToken(){token=MD5($(_0xf14b[1])[_0xf14b[0]]());return MD5(token+'<?=$_SESSION['admin_username']?>');} ;


</script>

<?php

include_once('transcript.inc.php');
include_once('common.php');
/* @author Marian */
define("SEARCH_MAX_HOURS", 40);
globalise('action');
globalise('searchBy');
globalise('searchBy2d');
globalise('searchValue');
globalise('from');
globalise('to');
globalise('games');
$name='searchuserhand';
$name2d='searchuserhand2d';
if(isset($_GET['userid'])){
    $searchBy=2;
    $searchValue=$_GET['userid'];
}
if($start_time=strtotime($from)){
    $from = date("Y-m-d H:i", $start_time);
}else{
    $from = date("Y-m-d H:i", strtotime("now - 2 hours"));
}

if($end_time=strtotime($to)){
    $to = date("Y-m-d H:i", $end_time);
}else{
    $to = date("Y-m-d H:i", strtotime("now"));
}


?>

<table class="table table-bordered" >
    <tr>
        <td class="navbar-inner" ><h3 class="text-center"><?=$lang->getLang("Poker Hands")?></h3></td>
    </tr>
    <tr>
        <td>
            <div class="well">
                <div class="tabbable">
                    <ul class="nav nav-tabs">
                        <li <?=($active=='live')? 'class="active"':''?>><a href="#live" data-toggle="tab"><button class="btn btn-inverse"><?=$lang->getLang("Live")?></button></a></li>
                        <li <?=($active=='2d')? 'class="active"':''?>> <a href="#2d" data-toggle="tab"><button class="btn btn-inverse"><?=$lang->getLang("2D")?></button></a></li>
                    </ul>
                    <div class="tab-content">
                        <div class="tab-pane <?=($active=='live')? 'active':''?>" id="live">
                            <form action="/poker_hands" method="post" id="searchuserhand" name="searchuserhand">
                                <input type="hidden" name="active" value="live">
                                <table>
                                    <tr><td style="border: 0px;">
                                            <div class="span5">
                                                <div class='label label-inverse'><?=$lang->getLang("Search by")?></div>
                                                <br>
                                                <div class="controls">
                                                    <div class="input-prepend">
                                                        <span class="add-on"><i class="icon-user"></i></span>
                                                        <select name="searchBy" id="searchBy" class="searchBy">
                                                            <option <?=($searchBy==1 ? 'selected="selected"': '')?> value="1"  ><?=$lang->getLang("Username")?></option>
                                                            <option <?=($searchBy==2 ? 'selected="selected"': '')?> value="2"  ><?=$lang->getLang("User Id")?></option>
                                                        </select>
                                                    </div>
                                                </div>
                                                <div><div class='label label-inverse searchLabel' id="searchLabel" content="searchLabel"><?=$lang->getLang("Search Username")?></div>
                                                    <br />
                                                    <div class="controls">
                                                        <div class="input-prepend">
                                                            <span class="add-on"><i class="icon-edit"></i></span>
                                                            <input type='text'  name='searchValue'  placeholder="<?=$lang->getLang('Search')?>" required="required" value="<?=$searchValue?>">
                                                        </div>
                                                    </div>
                                                </div>
                                            </div>
                                        </td>

                                        <td style="border: 0px;" id="dateContainer" class="dateContainer">
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
                                                        <input type='text' name='from' id='from' class="dateStartPicker"  value="<?=$from?>" >
                                                    </div>
                                                </div>
                                            </div>
                                            <div><div class='label label-inverse'><?=$lang->getLang("Until")?></div>
                                                <br>
                                                <div class="controls">
                                                    <div class="input-prepend">
                                                        <span class="add-on"><i class="icon-calendar"></i></span>
                                                        <input type='text' name='to' id='to' class="dateEndPicker"  value="<?=$to?>" >
                                                    </div>
                                                </div>
                                            </div>
                                        </td>
                                    </tr>
                                </table>
                                <br />
                                <br />
                                <input type="hidden" value="searchuserhands" name="action" />
                                <button class="btn btn-primary" ><?=$lang->getLang("Search")?></button>
                                <br>
                            </form>
                            <?
                            if(isset($_POST['searchValue'])&& $active=='live'){
                                if(($end_time - $start_time) / 60 / 60 > SEARCH_MAX_HOURS){ ?>
                                    <div class="alert alert-error fade in"><button type='button' class='close fade in' data-dismiss='alert'>&times;</button><?=$lang->getLang('You have selected an interval too long');?></div>
                                <? }
                                else{
                                    searchupokerhands($searchBy,$searchValue,$from,$to);
                                }
                            }
                            ?>
                        </div>
                        <div class="tab-pane <?=($active=='2d')? 'active':''?>" id="2d">
                            <form action="/poker_hands" method="post" id="searchuserhand2d" name="searchuserhand2d" >
                                <input type="hidden" name="active" value="2d">
                                <table>
                                    <tr><td style="border: 0px;">
                                            <div class="span5">
                                                <div class='label label-inverse'><?=$lang->getLang("Search by")?></div>
                                                <br>
                                                <div class="controls">
                                                    <div class="input-prepend">
                                                        <span class="add-on"><i class="icon-user"></i></span>
                                                        <select name="searchBy2d" id="searchBy2d" class="searchBy">
                                                            <option <?=($searchBy2d==1 ? 'selected="selected"': '' )?> value="1"  ><?=$lang->getLang("Username")?></option>
                                                            <option <?=($searchBy2d==2 ? 'selected="selected"': '')?> value="2"  ><?=$lang->getLang("User Id")?></option>
                                                        </select>
                                                    </div>
                                                </div>
                                                <div><div class='label label-inverse searchLabel' id="searchLabel2d"><?=$lang->getLang("Search Username")?></div>
                                                    <br />
                                                    <div class="controls">
                                                        <div class="input-prepend">
                                                            <span class="add-on"><i class="icon-edit"></i></span>
                                                            <input type='text' id="searchValue2d"  name='searchValue'  placeholder="<?=$lang->getLang('Search')?>" required="required" value="<?=$searchValue?>">
                                                        </div>
                                                    </div>
                                                </div>
                                                <div class='label label-inverse'><?=$lang->getLang("Group by")?></div>
                                                <br>
                                                <div class="controls">
                                                    <div class="input-prepend">
                                                        <span class="add-on"><i class="icon-list"></i></span>
                                                        <select name="groupBy" id="groupBy" >
                                                            <option <?=($groupBy==1 ? 'selected="selected"': '')?> value="1"  ><?=$lang->getLang("Table")?></option>
                                                            <option <?=($groupBy==2 ? 'selected="selected"': '')?> value="2"  ><?=$lang->getLang("Hand")?></option>
                                                            <option <?=($groupBy==3 ? 'selected="selected"': '')?> value="3"  ><?=$lang->getLang("Tournament")?></option>
                                                        </select>
                                                    </div>
                                                </div>
                                            </div>
                                        </td>

                                        <td style="border: 0px;" id="dateContainer" class="dateContainer">
                                            <div><div class='label label-inverse'><?=$lang->getLang("Date")?></div>
                                                <br>
                                                <div class="controls">
                                                    <div class="input-prepend">
                                                        <span class="add-on"><i class="icon-calendar"></i></span>
                                                        <?createDateHelper("date_helper2d", SEARCH_MAX_HOURS, "from", "to", $name2d)?>
                                                    </div>
                                                </div>
                                            </div>
                                            <div><div class='label label-inverse'><?=$lang->getLang("From")?></div>
                                                <br>
                                                <div class="controls">
                                                    <div class="input-prepend">
                                                        <span class="add-on"><i class="icon-calendar"></i></span>
                                                        <input type='text' name='from' id='from2d' class="dateStartPicker"  value="<?=$from?>" >
                                                    </div>
                                                </div>
                                            </div>
                                            <div><div class='label label-inverse'><?=$lang->getLang("Until")?></div>
                                                <br>
                                                <div class="controls">
                                                    <div class="input-prepend">
                                                        <span class="add-on"><i class="icon-calendar"></i></span>
                                                        <input type='text' name='to' id='to2d' class="dateEndPicker"  value="<?=$to?>" >
                                                    </div>
                                                </div>
                                            </div>
                                        </td>
                                    </tr>
                                </table>
                                <br />
                                <br />
                                <button class="btn btn-primary" ><?=$lang->getLang("Search")?></button>
                                <br>
                            </form>
                            <?
                            if(isset($_POST['searchValue']) && $active=='2d' ){

                            if($groupBy==3) { ?>
                                <table class='table display table-hover table-striped table-bordered' id="historyTableTournament">
                                    <thead>
                                    <tr>
                                        <th class="unwrappable"><?= $lang->getLang("Tournament") ?></th>
                                        <th class="unwrappable"><?= $lang->getLang("Date") ?></th>
                                        <th class="unwrappable"><?= $lang->getLang("User Name") ?></th>
                                        <th class="unwrappable"><?= $lang->getLang("Win") ?></th>
                                        <th class="unwrappable"><?= $lang->getLang("Buy In") ?></th>
                                        <th class="unwrappable"><?= $lang->getLang("Fee") ?></th>
                                        <th class="unwrappable"><?= $lang->getLang("Rebuy") ?></th>
                                        <th class="unwrappable"><?= $lang->getLang("Rebuy Fee") ?></th>
                                        <th class="unwrappable"><?= $lang->getLang("Addon") ?></th>
                                        <th class="unwrappable"><?= $lang->getLang("Addon Fee") ?></th>
                                        <th class="unwrappable"><?= $lang->getLang("Finish Place") ?></th>
                                        <th class="unwrappable"><?= $lang->getLang("Ip") ?></th>

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
                                        <td></td>
                                        <td></td>
                                        <td></td>
                                        <td></td>
                                    </tr>
                                    </tbody>
                                </table>
                                <script>

                                    $(function () {
                                        var history2dTournament=$('#historyTableTournament').DataTable({
                                            "processing": true,
                                            "serverSide": true,
                                            "ajax": {
                                                "url": servicesUrl,
                                                "data": function (d) {
                                                    d.action = "getPoker2dHands";
                                                    d.searchBy = $("#searchBy2d").val();
                                                    d.searchValue = $("#searchValue2d").val();
                                                    d.groupBy = $("#groupBy").val();
                                                    d.from = $("#from2d").val();
                                                    d.to = $("#to2d").val();

                                                }
                                            }, 'iDisplayLength': 10, 'sPaginationType': "full_numbers"
                                        });


                                    });

                                </script>
                            <?
                            }
                            else{

                                if($groupBy==2) { ?>
                                    <table class='table display table-hover table-striped table-bordered'
                                           id="historyTable2D">
                                        <thead>
                                        <tr>
                                            <th class="unwrappable"><?= $lang->getLang("Hand ID") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Date") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("User Name") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Bet") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Win") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Table Name") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Ip") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Cards") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Common Cards") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Nr. Players") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Total Bets") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Total Rake") ?></th>
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
                                            <td></td>
                                            <td></td>
                                            <td></td>
                                            <td></td>
                                        </tr>
                                        </tbody>
                                    </table>
                                    <script>

                                        $(function () {
                                            var history2d=$('#historyTable2D').DataTable({
                                                "processing": true,
                                                "serverSide": true,
                                                "ajax": {
                                                    "url": servicesUrl,
                                                    "data": function (d) {
                                                        d.action = "getPoker2dHands";
                                                        d.searchBy = $("#searchBy2d").val();
                                                        d.searchValue = $("#searchValue2d").val();
                                                        d.groupBy = $("#groupBy").val();
                                                        d.from = $("#from2d").val();
                                                        d.to = $("#to2d").val();

                                                    }
                                                }, 'iDisplayLength': 10, 'sPaginationType': "full_numbers"
                                            });


                                        });

                                    </script>
                                    <?
                                }
                                else {
                                    searchPokerHands2d($searchBy, $searchValue, $from, $to);
                                    ?>
                                <div id="tableDetailsContainer" class="hidden">
                                    <table class='table display table-hover table-striped table-bordered compact nowrap'
                                           id="tableDetails">
                                        <thead>
                                        <tr>
                                            <th class="unwrappable"><?= $lang->getLang("Hand ID") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Date") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("User Name") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Bet") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Win") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Table Name") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Ip") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Cards") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Common Cards") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Nr. Players") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Total Bets") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Total Rake") ?></th>
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
                                            <td></td>
                                            <td></td>
                                            <td></td>
                                            <td></td>
                                        </tr>
                                        </tbody>
                                    </table>
                                </div>
                                <?
                            }
                                }
                            }
                            ?>
                        </div>
                    </div>
                </div>
            </div>

        </td>
    </tr>
</table>
<?
function searchupokerhands($searchBy,$searchValue,$from,$to,$type='live'){
    global $dbh, $handid, $games,$lang;
    if(empty($searchValue)){ ?>
        <h4 class="alert alert-error fade in"><button type='button' class='close fade in' data-dismiss='alert'>&times;</button><?=$lang->getLang('Username or user id or hand id required');?></h4>
        <?
    }
    if($type=='live') {
        $sql = " select pa.*,p.user_id from casinogames.punter pa,poker.rep_player_account p
           WHERE pa.pun_customer_number = p.partner_user_id ";
        if ($searchBy == 1) {
            $sql .= " AND pun_username=" . $dbh->prepareString($searchValue);
        } elseif ($searchBy == 2) {
            $sql .= " AND pun_customer_number=$searchValue ";
        }
        $customer = $dbh->queryRow($sql, true);
        if ($customer) {
            $_SESSION['poker_user_id'] = $customer['user_id'];
            $_SESSION['poker_username'] = $customer['pun_username'];
        } else {
            unset($_SESSION['poker_user_id']);
            unset($_SESSION['poker_username']);
            ?>
            <h4 class="alert alert-error fade in">
                <button type='button' class='close fade in'
                        data-dismiss='alert'>&times;</button><?= $lang->getLang('Player not found'); ?></h4>
            <?
            return false;
        }
        $query = "select
        poker.rep_games.`active_table_id` AS `active_table_id`,
        poker.rep_seats.`player_id` AS `player_id`,
        poker.rep_seats.`game_id` AS `hand_id`,
        poker.rep_seats.`ip` AS `ip`,
        poker.rep_seats.`holecards` AS `holecards`,
        poker.rep_games.`board_cards` AS `board_cards`,
        poker.rep_games.`tablename` AS `tablename`,
        poker.rep_games.`total_pot` AS `total_pot`,
        poker.rep_games.`total_rake` AS `total_rake`,
        poker.rep_games.`sb_value` AS `sb_value`,
        poker.rep_games.`bb_value` AS `bb_value`,
        min(poker.rep_games.`hand_start`) AS `hand_start`,
        max(poker.rep_games.`hand_start`) AS `hand_end`,
        min(poker.rep_games.`game_id`) AS `first_game_id`,
        max(poker.rep_games.`game_id`) AS `last_game_id`,
        count(poker.rep_games.`game_id`) AS `hands_played`,
        poker.rep_seats.`bets` AS `bets`,
        poker.rep_seats.`won` AS `won`,
        (select count(*) from poker.rep_seats prs where prs.`game_id`=poker.rep_games.`game_id`) AS nr_players
    from
        (poker.rep_games
        join poker.rep_seats ON ((poker.rep_seats.`game_id` = poker.rep_games.`game_id`)))
    where
        ((poker.rep_games.`tour_id` = 0)
           and (poker.rep_seats.`holecards` > _utf8''))
           and player_id =" . $_SESSION['poker_user_id'] . "
           and poker.rep_games.`hand_start` > '$from:00'
           and poker.rep_games.`hand_start` < '$to:59'
    group by poker.rep_games.`active_table_id` , poker.rep_seats.`player_id`
    order by min(poker.rep_games.`hand_start`) desc ";

        $starttime = microtime(true);
        $rs = $dbh->exec($query, false, true);
        $endtime = microtime(true);
        $duration = $endtime - $starttime;
        $duration = number_format($duration, 4, ',', '') . "s";
    }

    $cards_arr = array (
        "Ac" => "C|1",
        "Ad" => "D|1",
        "As" => "S|1",
        "Ah" => "H|1",
        "2c" => "C|2",
        "2d" => "D|2",
        "2s" => "S|2",
        "2h" => "H|2",
        "3c" => "C|3",
        "3d" => "D|3",
        "3s" => "S|3",
        "3h" => "H|3",
        "4c" => "C|4",
        "4d" => "D|4",
        "4s" => "S|4",
        "4h" => "H|4",
        "5c" => "C|5",
        "5d" => "D|5",
        "5s" => "S|5",
        "5h" => "H|5",
        "6c" => "C|6",
        "6d" => "D|6",
        "6s" => "S|6",
        "6h" => "H|6",
        "7c" => "C|7",
        "7d" => "D|7",
        "7s" => "S|7",
        "7h" => "H|7",
        "8c" => "C|8",
        "8d" => "D|8",
        "8s" => "S|8",
        "8h" => "H|8",
        "9c" => "C|9",
        "9d" => "D|9",
        "9s" => "S|9",
        "9h" => "H|9",
        "Tc" => "C|10",
        "Td" => "D|10",
        "Ts" => "S|10",
        "Th" => "H|10",
        "Jc" => "C|11",
        "Jd" => "D|11",
        "Js" => "S|11",
        "Jh" => "H|11",
        "Qc" => "C|12",
        "Qd" => "D|12",
        "Qs" => "S|12",
        "Qh" => "H|12",
        "Kc" => "C|0",
        "Kd" => "D|0",
        "Ks" => "S|0",
        "Kh" => "H|0"
    );



    if ( $rs->getNumRows()>0 )
    {
        ?>
        <div class="tip fleft"><?=$lang->getLang("Time taken to execute your request")?>: <?=$duration?></div><br /> <br />
        <table class='table display table-hover table-striped table-bordered' id="historyTable" >
            <thead>
            <tr>
                <th class="unwrappable"><?=$lang->getLang("Hand ID")?></th>
                <th class="unwrappable"><?=$lang->getLang("Date")?></th>
                <th class="unwrappable"><?=$lang->getLang("User Name")?></th>
                <th class="unwrappable"><?=$lang->getLang("Bet")?></th>
                <th class="unwrappable"><?=$lang->getLang("Win")?></th>
                <th class="unwrappable"><?=$lang->getLang("Table Name")?></th>
                <th class="unwrappable"><?=$lang->getLang("Ip")?></th>
                <th class="unwrappable"><?=$lang->getLang("Cards")?></th>
                <th class="unwrappable"><?=$lang->getLang("Common Cards")?></th>
                <th class="unwrappable"><?=$lang->getLang("Nr. Players")?></th>
                <th class="unwrappable"><?=$lang->getLang("Total Bets")?></th>
                <th class="unwrappable"><?=$lang->getLang("Total Rake")?></th>
            </tr>
            </thead>
            <tbody>
            <?
            while ( $rs->hasNext() )
            {
                $row=$rs->next();
                if($type=='2d'){
                    $row['holecards']=chunk_split($row['holecards'], 2, ' ');
                    $row['board_cards']=chunk_split($row['board_cards'], 2, ' ');
                }
                /*$curcards=trim($row['holecards'])." ".trim($row['board_cards']);
                $curcards=explode(" ",trim($curcards));
                $currentHandValue=0;
                foreach($curcards as $k=>$v){
                    list ( $cardType, $cardValue ) = split ( "\|", $cards_arr[$v] );
                    $currentValue = getCardValue ( strtolower ( trim ( $cardValue ) . "" . trim ( $cardType )));
                    $currentHandValue*=$currentValue[2];
                }*/
                ?>
                <tr>
                    <td><?=getClassLink("javascript:openWindow('/poker/hand_details.php?hand_id={$row["hand_id"]}',
                                                        'hand{$row["hand_id"]}',
                                                        '900','650','0','1')", $row["hand_id"], "contentlink");?></td>
                    <td><?=($row["hand_start"])?></td>
                    <td> <?=getClassLink("javascript:openWindow('".refPage("customers/customer_view&customer_id={$customer["pun_id"]}")."','user{$customer["pun_username"]}','950','980','0','1')", $customer["pun_username"], "contentlink");?></td>
                    <td><?=getInDollars($row["bets"]/100);?></td>
                    <td><?=getInDollars($row["won"]/100);?></td>
                    <td><?=$row["tablename"];?></td>
                    <td><?=long2ip($row["ip"]);?></td>
                    <td><?=ShowCards($row['holecards'])?></td>
                    <td><?=ShowCards($row['board_cards']);?></td>
                    <td><?=$row["nr_players"];?></td>
                    <td><?=getInDollars($row["total_pot"]/100);?></td>
                    <td><?=getInDollars($row["total_rake"]/100);?></td>
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
    return false;
}

function searchPokerHands2d($searchBy,$searchValue,$from,$to,$groupBy){
    global $dbh, $lang;
    if(empty($searchValue)){ ?>
        <h4 class="alert alert-error fade in"><button type='button' class='close fade in' data-dismiss='alert'>&times;</button><?=$lang->getLang('Username or user id or hand id required');?></h4>
        <?
    }
    else{


        $sql = " select pa.*,p.pct_pun_id from casinogames.punter pa,poker.punter_cash_table p
           WHERE pa.pun_id = p.pct_pun_id";
        if ($searchBy == 1) {
            $sql .= " AND pun_username=" . $dbh->prepareString($searchValue);
        } elseif ($searchBy == 2) {
            $sql .= " AND pun_customer_number=$searchValue ";
        }
        $customer = $dbh->queryRow($sql, true);
        if ($customer) {
            $_SESSION['poker_user_id'] = $customer['pct_pun_id'];
            $_SESSION['poker_username'] = $customer['pun_username'];
        } else {
            unset($_SESSION['poker_user_id']);
            unset($_SESSION['poker_username']);
            ?>
            <h4 class="alert alert-error fade in">
                <button type='button' class='close fade in'
                        data-dismiss='alert'>&times;</button><?= $lang->getLang('Player not found'); ?></h4>
            <?
            return false;
        }


        if($groupBy=='2') {
            $query = "select
                poker.punter_cash_table.`pct_pun_id` AS `player_id`,
                poker.punter_cash_table.`pct_id` AS `hand_id`,
                '' AS `ip`,
                poker.punter_cash_table_hands.`pch_user_cards` AS `holecards`,
                poker.punter_cash_table_hands.`pch_common_cards` AS `board_cards`,
                poker.punter_cash_table.`pct_table_id` AS `tablename`,
                poker.punter_cash_table.`pct_total_bet`*100 AS `total_pot`,
                poker.punter_cash_table.`pct_total_rake`*100 AS `total_rake`,
                min(poker.punter_cash_table_hands.`pch_deal_date`) AS `hand_start`,
                max(poker.punter_cash_table_hands.`pch_deal_date`) AS `hand_end`,
                min(poker.punter_cash_table_hands.`pch_deal_id`) AS `first_game_id`,
                max(poker.punter_cash_table_hands.`pch_deal_id`) AS `last_game_id`,
                poker.punter_cash_table.`pct_total_hands` AS `hands_played`,
                poker.punter_cash_table_hands.`pch_bet`*100 AS `bets`,
               poker.punter_cash_table_hands.`pch_win`*100 AS `won`,
                (select count(*) from poker.punter_cash_table_hands prs where prs.`pch_deal_id`=poker.punter_cash_table_hands.`pch_deal_id`) AS nr_players
            from
                (poker.punter_cash_table
                join poker.punter_cash_table_hands ON poker.punter_cash_table.`pct_table_id` = poker.punter_cash_table_hands.`pch_pct_id` )
            where
            pct_pun_id =" . $_SESSION['poker_user_id'] . "
            and poker.punter_cash_table.`pct_start_date` > '$from'
            and poker.punter_cash_table.`pct_end_date` < '$to'
            group by poker.punter_cash_table.pct_id
            order by min(poker.punter_cash_table.`pct_start_date`) desc ";
            $starttime = microtime(true);
            $rs = $dbh->exec($query, false, true);
            $endtime = microtime(true);
            $duration = $endtime - $starttime;
            $duration = number_format($duration, 4, ',', '') . "s";







            if ( $rs->getNumRows()>0 )
            {
                ?>
                <div class="tip fleft"><?=$lang->getLang("Time taken to execute your request")?>: <?=$duration?></div><br /> <br />
                <table class='table display table-hover table-striped table-bordered' id="historyTable" >
                    <thead>
                    <tr>
                        <th class="unwrappable"><?=$lang->getLang("Hand ID")?></th>
                        <th class="unwrappable"><?=$lang->getLang("Date")?></th>
                        <th class="unwrappable"><?=$lang->getLang("User Name")?></th>
                        <th class="unwrappable"><?=$lang->getLang("Bet")?></th>
                        <th class="unwrappable"><?=$lang->getLang("Win")?></th>
                        <th class="unwrappable"><?=$lang->getLang("Table Name")?></th>
                        <th class="unwrappable"><?=$lang->getLang("Ip")?></th>
                        <th class="unwrappable"><?=$lang->getLang("Cards")?></th>
                        <th class="unwrappable"><?=$lang->getLang("Common Cards")?></th>
                        <th class="unwrappable"><?=$lang->getLang("Nr. Players")?></th>
                        <th class="unwrappable"><?=$lang->getLang("Total Bets")?></th>
                        <th class="unwrappable"><?=$lang->getLang("Total Rake")?></th>
                    </tr>
                    </thead>
                    <tbody>
                    <?
                    while ( $rs->hasNext() )
                    {
                        $row=$rs->next();

                        $row['holecards']=chunk_split($row['holecards'], 2, ' ');
                        $row['board_cards']=chunk_split($row['board_cards'], 2, ' ');


                        ?>
                        <tr>
                            <td><?=getClassLink("javascript:openWindow('/poker/hand_details.php?hand_id={$row["hand_id"]}',
                                                        'hand{$row["hand_id"]}',
                                                        '900','650','0','1')", $row["hand_id"], "contentlink");?></td>
                            <td><?=($row["hand_start"])?></td>
                            <td> <?=getClassLink("javascript:openWindow('".refPage("customers/customer_view&customer_id={$customer["pun_id"]}")."','user{$customer["pun_username"]}','950','980','0','1')", $customer["pun_username"], "contentlink");?></td>
                            <td><?=getInDollars($row["bets"]/100);?></td>
                            <td><?=getInDollars($row["won"]/100);?></td>
                            <td><?=$row["tablename"];?></td>
                            <td><?=long2ip($row["ip"]);?></td>
                            <td><?=ShowCards($row['holecards'])?></td>
                            <td><?=ShowCards($row['board_cards']);?></td>
                            <td><?=$row["nr_players"];?></td>
                            <td><?=getInDollars($row["total_pot"]/100);?></td>
                            <td><?=getInDollars($row["total_rake"]/100);?></td>
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
        else{
            $query = "select pct_table_id, pct_start_date,
            sum(pct_total_hands) pct_total_hands,
            sum(pct_win_hands) pct_win_hands,
            sum(pct_total_bet) pct_total_bet,
            sum(pct_total_win) pct_total_win,
            sum(pct_total_rake) pct_total_rake from
            poker.punter_cash_table where
            pct_pun_id =" . $_SESSION['poker_user_id'] . "
            and poker.punter_cash_table.`pct_start_date` > '$from'
            and poker.punter_cash_table.`pct_end_date` < '$to'
            group by pct_table_id";

            $starttime = microtime(true);
            $rs = $dbh->exec($query, false, true);
            $endtime = microtime(true);
            $duration = $endtime - $starttime;
            $duration = number_format($duration, 4, ',', '') . "s";


            if ( $rs->getNumRows()>0 )
            {
                ?>
                <div class="tip fleft"><?=$lang->getLang("Time taken to execute your request")?>: <?=$duration?></div><br /> <br />
                <table class='table display table-hover table-striped table-bordered' id="historyTable2dTable" >
                    <thead>
                    <tr>
                        <th class="unwrappable"><?=$lang->getLang("Table")?></th>
                        <th class="unwrappable"><?=$lang->getLang("Date")?></th>
                        <th class="unwrappable"><?=$lang->getLang("User Name")?></th>
                        <th class="unwrappable"><?=$lang->getLang("Total Hands")?></th>
                        <th class="unwrappable"><?=$lang->getLang("Win Hands")?></th>
                        <th class="unwrappable"><?=$lang->getLang("Total bet")?></th>
                        <th class="unwrappable"><?=$lang->getLang("Total win")?></th>
                        <th class="unwrappable"><?=$lang->getLang("Total rake")?></th>
                    </tr>
                    </thead>
                    <tbody>
                    <?
                    while ( $rs->hasNext() )
                    {
                        $row=$rs->next();
                        ?>
                        <tr>
                            <td><a href="javascript:void(0);" onclick="getPokerTableDetails('<?=$row['pct_table_id'];?>','<?=$customer["pun_id"]?>','<?=$from?>','<?=$to?>')" ><?=$row['pct_table_id'];?></a></td>
                            <td><?=($row["pct_start_date"])?></td>
                            <td> <?=getClassLink("javascript:openWindow('".refPage("customers/customer_view&customer_id={$customer["pun_id"]}")."','user{$customer["pun_username"]}','950','980','0','1')", $customer["pun_username"], "contentlink");?></td>
                            <td><?=$row["pct_total_hands"];?></td>
                            <td><?=$row["pct_win_hands"];?></td>
                            <td><?=getInDollars($row["pct_total_bet"]);?></td>
                            <td><?=getInDollars($row["pct_total_win"]);?></td>
                            <td><?=getInDollars($row["pct_total_rake"]);?></td>
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
    }
}


setupDateInputJs("from", "to");

?>


