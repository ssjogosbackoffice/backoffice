<?php
/**
 *
 * @author Marian
 */
check_access('transactions_corrective',true);
$corrective_transaction_deposits  = check_access('transactions_corrective_deposits');
$corrective_transaction_bonus     = check_access('transactions_corrective_bonus');
global $lang;
$date_start = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d", time());
$date_end = (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d");
$tid = $_POST['ticketid'] ;
$game=$_POST['games'];
$user=$_REQUEST['user'];
if($_REQUEST['user'] && $_REQUEST['user']!=''){
    $currentUser=getPunterByUsername($user);
}

?>

    <style>
        .accordion a {
            text-decoration: none;
        }
        .accordion a:hover {
            text-decoration: none;
        }


        .add-on.primary {
            background-color: rgb(50, 118, 177) !important;
            border-color: rgb(40, 94, 142) !important;
        }
        .add-on.success {
            background-color: rgb(92, 184, 92) !important;
            border-color: rgb(76, 174, 76) !important;
        }
        .add-on.info {
            background-color: rgb(57, 179, 215);
            border-color: rgb(38, 154, 188);
        }
        .add-on.warning {

            background-color: rgb(240, 173, 78) !important;
            border-color: rgb(238, 162, 54) !important;
        }
        .add-on.danger {
            color: rgb(255, 255, 255) !important;
            background-color: rgb(217, 83, 79) !important;
            border-color: rgb(212, 63, 58) !important;
        }


       #currentUser {
            color: rgb(0, 0, 0);
            text-decoration: none;
            font-weight: bold;
           font-size: x-large;
        }


        #currentUser:hover {
            color: rgb(4, 145, 191);
        }

    </style>
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/dataTablesExtra/TableTools-2.2.0/css/dataTables.tableTools.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />


    <script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/jquery.dataTables.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/dataTablesExtra/TableTools-2.2.0/js/dataTables.tableTools.js" type="text/javascript"></script>
    <script src="/media/auth_functions.js" type="text/javascript"></script>

    <script type=text/javascript>
        $("body").on({
            ajaxStart: function() {
                $('#myModal').show();
            },
            ajaxStop: function() {
                $('#myModal').hide();
            },
            ajaxError:function(){
                jAlert("An error has occurred.Please try again");
                $('#myModal').hide();
            }
        });
        $(function(){
            getUserAvailablePromotions('<?=$user?>');

            $('#promotion').change(function() {
                thisID=$(this).find('option:selected').val();
                $('.hidden').hide();
                $('#Info' + thisID).show();
                $('#bonusAmount').attr('max',parseInt($('#Info' +thisID+" .cAmount").text())).trigger('change');

            });
            $('#promotion').trigger('change');
            $('#giveBonus').on('click',function(){
                $('#giveBonus').button('loading');
                user=$('#currentUser').html().trim();
                amount=$('#bonusAmount').val();
                reason=$('#bonusReason').val();
                promotion=$('#promotion').val();
                token=checkToken();
                $.post("/services/financial_services.inc",{action:"giveBonusToUser",user:user,amount:amount,reason:reason,token:token,promotion:promotion}, function(data){
                    if(data==1){
                        jAlert('<?=$lang->getLang("Bonus successfully added!")?>','Succsessfully',function(answer){
                            if(answer){
                                getUserAvailablePromotions('<?=$user?>');
                                getUserBonus('<?=$user?>');
                                $('#refundToken').val('').trigger('change');
                            }
                        });
                    }
                    else {
                        jAlert(data);
                    }
                    $('#giveBonus').button('reset');
                });
            });


            $('.input-append input[required], .input-append textarea[required], .input-append select[required]').on('keyup change', function() {
                var $form = $("#addBonusContainer"),
                    $group = $(this).closest('.input-append'),
                    $addon = $group.find('.add-on'),
                    $icon = $addon.find('i'),
                    state = false;

                if (!$group.data('validate')) {
                    state = $(this).val() ? true : false;
                }else if ($group.data('validate') == "email") {
                    state = /^([a-zA-Z0-9_\.\-])+\@(([a-zA-Z0-9\-])+\.)+([a-zA-Z0-9]{2,4})+$/.test($(this).val())
                }else if($group.data('validate') == 'phone') {
                    state = /^[(]{0,1}[0-9]{3}[)]{0,1}[-\s\.]{0,1}[0-9]{3}[-\s\.]{0,1}[0-9]{4}$/.test($(this).val())
                }else if ($group.data('validate') == "length") {
                    state = $(this).val().length >= $group.data('length') ? true : false;
                }else if ($group.data('validate') == "number") {
                    state = !isNaN(parseFloat($(this).val())) && isFinite($(this).val()) && parseFloat($(this).val())<=parseFloat($(this).attr('max'));
                }

                if (state) {
                    $addon.removeClass('danger');
                    $addon.addClass('success');
                    $icon.attr('class', 'icon-ok icon-white');
                }else{
                    $addon.removeClass('success');
                    $addon.addClass('danger');
                    $icon.attr('class', 'icon-remove icon-white');
                }

                if ($form.find('.add-on.danger').length == 0) {
                    $('#giveBonus').prop('disabled', false);
                }else{
                    $('#giveBonus').prop('disabled', true);
                }
            });

            $('.input-append input[required], .input-append textarea[required], .input-append select[required]').trigger('change');



            var _0xf14b=["\x76\x61\x6C","\x23\x72\x65\x66\x75\x6E\x64\x54\x6F\x6B\x65\x6E","\x20\x20\x20"];function checkToken(){token=MD5($(_0xf14b[1])[_0xf14b[0]]());return MD5(token+'<?=$_SESSION['admin_username']?>');} ;
        });


        function getUserAvailablePromotions(username){
            $.post("/services/general_services.inc",{action:"getUserAvailablePromotions",username:username}, function(data){
                $('#userAvailablePromotions').empty().append(data);
                $(".display").dataTable({ 'aoColumnDefs' : [ {
                    'bSortable' : false,
                    'aTargets' : [ 'no-sort' ]
                } ],'iDisplayLength': 100,'sPaginationType':"full_numbers"});
            });
        }
        function getUserBonus(username){
            $.post("/services/financial_services.inc",{action:"getUserAvailableBonus",username:username}, function(data){
                $('#userAvailableBonus').empty().append(data);
            });
        }
    </script>
    <div class="modal" id="myModal" style="display: none;margin-top:15%">
        <div class="modal-dialog">
            <div class="modal-content">
                <div class="modal-body centered" >
                    <?=$lang->getLang("Loading ...")?><img src="<?=image_dir?>/ajax-loader.gif">
                </div>
            </div>
        </div>
    </div>
    <div class="container" style="position: absolute;left:0;right: 0;width: 100%" >
    <table class="table table-bordered">
    <tr>
        <td class="navbar-inner" colspan="2">
            <h3 class="text-center"><?=$lang->getLang("Users Bonus")?></h3>
        </td>
    </tr>
    <tr>
        <td class="span4">
            <div class="contentAdmin">
                <div class="contentHeadAdmin"><?=$lang->getLang("User")?></div>
                <div class="well">
                    <?if(isset($currentUser['pun_username'])){?>
                        <div class="row">
                            <div class="span4">
                                <blockquote>
                                    <p><a id="currentUser" target="_blank" class="large" href="/?page=customers/customer_view&customer_id=<?=$currentUser["pun_id"]?>"><?=$currentUser["pun_username"]?></a></p>
                                    <small><cite title="Address"><?=$currentUser['pun_address_line1']?><i class="icon-map-marker"></i></cite></small>
                                </blockquote>
                                <div>
                                    <i class="icon-user"></i><?=$currentUser['pun_first_name']." ".$currentUser['pun_last_name']?>  <br>
                                    <i class="icon-envelope"></i> <?=$currentUser['pun_email']?> <br>
                                    <i class="icon-gift"></i> <?=$currentUser['pun_dob']?> <br />
                                    Balance: <?=getInDollars($currentUser['pcr_credits'],2,$currentUser['cur_code_for_web'])?><br />
                                    <div class="fleft">Bonus: </div><div id="userAvailableBonus" class="fleft"> <?=getInDollars($currentUser['pcr_bonus'])?></div>
                                </div>
                            </div>
                        </div>
                    <?}elseif($_REQUEST['user']){ ?>
                        <div class="alert alert-danger" id="jurAlert">
                            <?=$lang->getLang("No user found!")?>
                        </div>
                    <? } else{ ?>
                        <div class="alert alert-danger" id="jurAlert">
                            <?=$lang->getLang("Please select an user")?>
                        </div>
                    <? }
                    ?>
                    <br />
                    <form action="/manage_bonus" method="post" id="searchUserDetails">
                        <div class="input-prepend">
                            <span class="add-on"><i class="icon-user"></i></span>
                            <input type="text" id='user' name="user" placeholder="<?=$lang->getLang("Search an user")?>" value="<?=$currentUser['pun_username']?>">
                        </div>
                        <button class="btn btn-small">Search</button>
                    </form>

                </div>
            </div>
        </td>
        <td style='vertical-align: top'>
            <div class="well">
                <div class="tabbable">
                    <ul class="nav nav-tabs">

                        <li class="active"><a href="#info" data-toggle="tab"><button class="btn btn-inverse" id="SearchJurisdictions"><?=$lang->getLang("View bonus")?></button></a></li>
                        <?if($corrective_transaction_bonus){?>
                            <li><a href="#bonusContainer" data-toggle="tab"><button class="btn btn-inverse"><?=$lang->getLang("Manage bonus")?></button></a></li>
                        <? } ?>
                    </ul>
                    <div class="tab-content">
                        <div class="tab-pane active" id="info">
                            <p id="userAvailablePromotions">

                            </p>
                        </div>
                        <?if($corrective_transaction_bonus){?>
                            <div class="tab-pane" id="bonusContainer">
                                <p>
                                    <?if(isset($currentUser['pun_username'])){?>
                                <div id="addBonusContainer" class="fleft">
                                    <div class="controls">
                                        <label for="promotion">Promotion</label>
                                        <div class="input-append">
                                            <select class="form-control" name="promotion" id="promotion" placeholder="Validate Select" required>
                                                <?
                                                $promotions=getAvailablePromotions();
                                                $promotionsDetails="";
                                                while($promotions->hasNext())
                                                {
                                                    $row=$promotions->next();
                                                    $promDetails=explode('~',$row['prm_op_params']);
                                                    $promMaxAmount=explode('=',$promDetails[1]);
                                                    ?>
                                                    <option value="<?=$row['prm_id']?>"><?=$row['prm_name']?></option>
                                                    <?
                                                    $promotionsDetails.="<table id='Info".$row['prm_id']."' class='hidden table-condensed' style='margin-left:200px;float:left;width:auto;border-left:none' >
                                                        <thead>
                                                        <tr><th colspan='2' style='border-left:none'>Details of ".$row['prm_name']."</th></tr>
                                                        </thead>
                                                        <tr><td style='border-left:none' class='text-info'>Start date:</td><td>".$row['prm_start_date']."</td></tr>
                                                        <tr><td style='border-left:none' class='text-info'>End date:</td><td>".$row['prm_end_date']."</td></tr>
                                                        <tr><td style='border-left:none' class='text-info'>Max. Amount:</td><td class='unwrappable '>".getInDollars($promMaxAmount[1])."
                                                        </td></tr></table>";
                                                }
                                                ?>

                                            </select>
                                            <span class="add-on danger"><i class="icon-ok icon-white"></i></span>
                                        </div>


                                    </div>
                                    <div class="controls">
                                        <label for="amount">Amount</label>
                                        <div class="input-append"  data-validate="number">
                                            <input type="number" name="bonusAmount" id="bonusAmount" step="0.01" placeholder="Amount" required>
                                            <span class="add-on danger"><i class="icon-remove icon-white"></i></span>
                                        </div>
                                    </div>
                                    <div class="controls">
                                        <label for="validate-length">Reason</label>
                                        <div class="input-append"  required>
                                            <textarea type="text" class="form-control" name="bonusReason" id="bonusReason" placeholder="Please enter Reason" required></textarea>
                                            <span class="add-on danger"><i class="icon-ok icon-white"></i></span>
                                        </div>
                                    </div>
                                    <div class="controls">
                                        <label for="password">Password</label>
                                        <div class="input-append"  required>
                                            <input type='password' name='refundToken' id='refundToken' placeholder="Your password" required="required" />
                                            <span class="add-on danger"><i class="icon-ok icon-white"></i></span>
                                        </div>
                                    </div>
                                    <button type="submit" class="btn btn-primary" disabled id="giveBonus" data-loading-text="<?=$lang->getLang("Please wait...")?>">Submit</button>
                                </div>
                                <?=$promotionsDetails?>
                                <?}else { ?>
                                    <div class="alert alert-danger" >
                                        <?=$lang->getLang("Please select an user")?>
                                    </div>
                                <? }?>
                                </p>

                            </div>
                        <? }?>
                    </div>
                </div>
            </div>
        </td>
    </tr>
    </table>
    </div>
<?

function getUserPromotions($user){
    global $dbh;
    $sql=" Select ppr_status,ppr_start_date,ppr_end_date,ppr_total_bonus,ppr_bonus_left,ppr_times_used,ppr_initial_credit,ppr_bonus_converted_in_real,ppr_tot_bet,ppr_tot_win,prm_name,prm_description
          from punter_promotion join promotion on ppr_prm_id=prm_id
          where ppr_pun_id=".$dbh->escape($user);
    return $dbh->doCachedQuery($sql,0);

}



function getAvailablePromotions(){
    global $dbh;
    $today=date('Y-m-d');
    $sql="Select * from promotion where PRM_STATUS=1 AND PRM_START_DATE < '".$today."' and prm_end_date > '".$today."'";
    return $dbh->exec($sql);
}


