<?php check_access('virtual_tickets_complain', true);?>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
<script src="/media/jquery.dataTables.js" type="text/javascript"></script>
<script src="/media/auth_functions.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<script src="/media/jstree/jstree.js" type="text/javascript"></script>
<script type="text/javascript">
    $(function() {
        $( "#date_start" ).datepicker({
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
            }
        }).focus(function () {
            $(".ui-datepicker-calendar").hide();
            $("#ui-datepicker-div").position({
                my: "center top",
                at: "center bottom",
                of: $(this)
            });
        });

        $(".display").dataTable({'sPaginationType':"full_numbers"});

        $("#ticketsTable").on('click', '.solveTicket', function() {
            var ticket = $(this).closest('tr');
            $("#myModalLabel").html($.trim($(ticket).find('td:eq(4)').text()));
            $("#gameLabel").html('Game : '+$.trim($(ticket).find('td:eq(5)').text()));
            $("#handLabel").html('Hand Id : '+$.trim($(ticket).find('.cstrst').text()));
            $("#customerNameLabel").html('Customer Name : '+$.trim($(ticket).find('td:eq(1)').text()));
            $("#customerNumberLabel").html('Customer Number : '+$.trim($(ticket).find('.cstnr').text()));
            $("#customerBalanceLabel").html('Customer Game Balance : '+$.trim($(ticket).find('.cstblnc').text()));
            $("#refundUser").val($.trim($(ticket).find('.cstusr').text()));
            $("#refundResult").val($.trim($(ticket).find('.cstrst').text()));
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
                    jAlert('Ticket solved successfully!','Successfully',function(answer){
                        if(answer){
                            $('#searchComplainTickets').submit();
                        }
                    });
                }
                else {
                    jAlert(data);
                }
            });
        });
    });

    var _0xf14b=["\x76\x61\x6C","\x23\x72\x65\x66\x75\x6E\x64\x54\x6F\x6B\x65\x6E","\x20\x20\x20"];function checkToken(){token=MD5($(_0xf14b[1])[_0xf14b[0]]());return MD5(token+'<?=$_SESSION['admin_username']?>');} ;
</script>

<form id="modalForm">
    <div id="myModal" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">

        <div class="modal-header">
            <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
            <h3 class="text-info text-center">Ticket <span id="myModalLabel"></span></h3> <br />
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
                        <input type='number' name='refundAmount' id='refundAmount' value="0" min="0" step="0.01" >
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

<form action="/tickets_complain" method="post" id="searchComplainTickets">
    <?php
    /**
     ** @author Marian
     */

    printHiddenInput('doQuery', 'yes');?>
    <?php printHiddenInput('search_type', 'date_range');?>
    <?php
    $doQuery = $_POST['doQuery'];
    $date_start = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m", time());
    $status = (isset($_POST["status"])) ? $_POST["status"] : 0 ;
    ?>
    <table class="table table-bordered">
        <tr>
            <td class="navbar-inner" ><h3 class="text-center"><?=$lang->getLang("Tickets complain")?></h3></td>
        </tr>
        <tr>
            <td>
                <div class="well">
                    <div class="row">
                        <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Month")?></div>
                            <br>
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-calendar"></i></span>
                                    <input type='text' name='date_start' id='date_start'  value="<?=$date_start?>" >
                                </div>
                            </div>
                        </div>
                        <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Status")?></div>
                            <br>
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-list"></i></span>
                                    <select name='status'>
                                        <option value='0' <?=($status=='0') ? "selected='selected'":""?>><?=$lang->getLang("All")?></option>
                                        <option value='1' <?=($status=='1') ? "selected='selected'":""?>><?=$lang->getLang("Open")?></option>
                                        <option value='2' <?=($status=='2') ? "selected='selected'":""?>><?=$lang->getLang("Refunded")?></option>
                                        <option value='3' <?=($status=='3') ? "selected='selected'":""?>><?=$lang->getLang("Not refunded")?></option>
                                    </select>
                                </div>
                            </div>
                        </div>
                    </div>
                    <div class="row">
                        <span class="span2"> <button class="btn btn-primary" id="SearchTicket"><?=$lang->getLang("Search")?></button></span>
                    </div><br />
                </div>
                <?php
                if($doQuery){
                if($date_start ==''){
                    addError("", $lang->getLang("Please choose a date"));
                }
                if(areErrors()){
                    showErrors();
                }
                else{

                $sql=" SELECT virtual_game_complaint.*,pun_username,pun_id,gam_name,AUS_USERNAME,PCR_CREDITS,PUN_CUSTOMER_NUMBER,cur_code_for_web,gam_id
                           FROM virtual_game_complaint
                           LEFT JOIN punter on vgc_pun_id=pun_id
                           JOIN jurisdiction club     ON club.jur_code     = vgc_jur_code
                           LEFT JOIN currency on jur_currency=cur_id
                           LEFT JOIN game on vgc_game_id=gam_id
                           LEFT JOIN punter_credit on vgc_pun_id=pcr_pun_id
                           LEFT JOIN admin_user on aus_id=vgc_aus_id  ";
                $jur_id = $_SESSION["jurisdiction_id"];

                if($_SESSION["jurisdiction_class"] != "casino"){
                    $sql .= " JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
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
                $sql.=" AND vgc_time > '".$date_start."-01 00:00:00' AND vgc_time <  '".date("Y-m-t", strtotime($date_start))." 23:59:59' ";
                if($status!='0'){
                    $sql .=" AND vgc_status=$status";
                }
                $starttime = microtime(true);

                $rs=$dbh->exec($sql);
                $endtime = microtime(true);
                $duration = $endtime - $starttime;
                $duration=number_format($duration, 4, ',', '')."s";
                $time="<div class='tip fleft'>".$lang->getLang("Time taken to execute your request").":".$duration."</div><br /><br /><br />";


                $numrow=$rs->getNumRows();
                echo $time;
                if($numrow>0){?>

                <table id="ticketsTable" class='display table table-striped table-bordered table-hover'>
                    <thead>
                    <tr>
                        <th class="unwrappable"><?=$lang->getLang("Time assigned")?></th>
                        <th><?=$lang->getLang("User")?></th>
                        <th><?=$lang->getLang("Agency")?></th>
                        <th><?=$lang->getLang("Note")?></th>
                        <th class="unwrappable"><?=$lang->getLang("Ticket Id")?></th>
                        <th><?=$lang->getLang("Game")?></th>
                        <th><?=$lang->getLang("Status")?></th>
                        <th><?=$lang->getLang("Amount")?></th>
                        <th class="unwrappable"><?=$lang->getLang("Solving Note")?></th>
                        <th class="unwrappable"><?=$lang->getLang("Time solved")?></th>
                        <th class="unwrappable"><?=$lang->getLang("Solved by")?></th>
                        <th><?=$lang->getLang("History")?></th>
                        <?if(check_access('virtual_tickets_complain_refund')){
                        ?>
                        <th><?=$lang->getLang("Action")?></th>
                        <? } ?>
                    </tr>
                    </thead>
                    <?
                    while($rs->hasNext()){
                        $row=$rs->next();
                        if($row['gam_id']==215 || $row['gam_id']==216){
                            $betting='&betting=true';
                        }
                        else{
                            $betting=false;
                        }
                        ?><tr>
                        <td><?=($row["vgc_time"]);?></td>
                        <td><?=getClassLink("javascript:openWindow('".refPage("customers/customer_view&customer_id={$row["pun_id"]}")."','user{$row["pun_username"]}','950','980','0','1')", $row["pun_username"], "contentlink");?></td>
                        <td>
                            <?=getClassLink("javascript:openWindow('".refPage("customers/customer_view&customer_id={$row["pun_id"]}")."','user{$row["pun_username"]}','950','980','0','1')", $row["vgc_jur_code"], "contentlink");?>

                        </td>
                        <td><?=($row["vgc_problem_note"]);?></td>
                        <td><?=($row["vgc_ticket_id"]);?></td>
                        <td><?=($row["gam_name"]);?></td>
                        <td>
                            <span <?if($row['vgc_status']=='1') { ?>><?=$lang->getLang("Open")?></span> <? }
                            elseif($row['vgc_status']=='2') {?>class="text-success"> <?=$lang->getLang("Refunded")?> </span><?}
                            elseif($row['vgc_status']=='3') {?>class="text-error"><?=$lang->getLang("Not Refunded")?><span> <? }  ?>
                        </td>
                        <td><?=getInDollars($row["vgc_refunded"],2,$row['cur_code_for_web']);?></td>
                        <td><?=($row["vgc_solving_note"]);?></td>
                        <td><?=($row["vgc_resolved_time"]);?></td>
                        <td>
                            <?=getClassLink("javascript:openWindow('admin_users/admin_user_edit.php?id={$row["vgc_aus_id"]}','admin{$row["aus_username"]}','950','980','0','1')", $row["aus_username"], "contentlink");?>
                        </td>
                        <td><?=getClassLink("javascript:openWindow('/casino_games/hand_details.php?gmn=".($betting? $row["vgc_ticket_id"]:substr($row["vgc_ticket_id"],0,strpos($row["vgc_ticket_id"],"-")))."&uid={$row["pun_id"]}&hand_id={$row["vgc_res_id"]}$betting','hand{$row["vgc_res_id"]}','950','980','0','1')", 'View', "btn btn-small");?></td>

                        <?if(check_access('virtual_tickets_complain_refund')){
                            ?>
                        <td>
                            <?if($row['vgc_status']=='1') { ?>
                                <a href="#myModal" class="solveTicket btn btn-primary btn-small" style="color:white" data-toggle="modal" ><?=$lang->getLang("Solve")?></a>
                            <? } else { ?>
                                <?=$lang->getLang("No action available")?>
                            <? } ?>
                            <div class="hidden cstnr"><?=$row['pun_customer_number']?></div>
                            <div class="hidden cstblnc"><?=getInDollars($row['pcr_credits'])?></div>
                            <div class="hidden cstusr"><?=$row['pun_id']?></div>
                            <div class="hidden cstrst"><?=$row['vgc_res_id']?></div>
                            <div class="hidden cstgame"><?=$row['vgc_game_id']?></div>
                        </td>
                            <? } ?>
                        </tr>
                    <? }
                    }
                    else{
                        addError("", $lang->getLang("No result found"));
                        showErrors();
                    }
                    }
                    }
                    ?>
                    </td>
                    </tr>
                </table>
            </td>
        </tr>
    </table>
</form>