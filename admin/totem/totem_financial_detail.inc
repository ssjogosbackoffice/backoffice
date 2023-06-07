<?php
include_once 'JurisdictionsList.class.inc';
require_once 'RedisClient.php';

global $lang;

if(isset($_GET['listTotemConnected'])) {

    // get the data from reddis client
    $redisClient=new RedisClient(REDISSERVER,REDISPORT,$dbLog);
    $reddisTotemKey = "TTM_INFO_*";
    $reddisData = $redisClient->getKeys($reddisTotemKey);

    // Extract the codes from the reddis data
    $keywordArray = [];
    foreach($reddisData as $key => $value) {
        $keyword = str_replace("TTM_INFO_", "", $value);
        array_push($keywordArray, $keyword);
    }

    $jur_id =  $_GET['jurId'];
    $jur_class = $_GET['jurClass'];
    $jur_class_id = "";
    switch ($jur_class) {
        case 'club': $jur_class_id = 5; break;
        case 'district': $jur_class_id = 4; break;
        case 'region': $jur_class_id = 3; break;
        case 'nation': $jur_class_id = 2; break;
        case 'casino' : $jur_class_id = 1; break;
    }

    //extract the codes for the db
    $sql = "call extract_totem_jur_sub_level_code($jur_id, $jur_class_id, '";
    foreach ($keywordArray as $key => $value) {
        $sql .= $value . ",";
    }
    $sql = substr($sql, 0, -1);
    $sql .= "');";

    $totemData = $dbh->doCachedQuery($sql, 0);
    $totals = $totemData->getNumRows();
    $tableArr = array();

    while ($totemData->hasNext()){

        $row = $totemData->next();
        $section = array();

        $totemCode = $row['ttm_code'];
        $totemAccess = $row['ttm_access'];
        $totemAccess =  strtolower($totemAccess);
        if($totemAccess == "allow") {
            $section[1] = "<img title='Allowed' src=" . image_dir . "/icon_lock_open.gif />";
        }
        else {
            $section[1] = "<img title='Denied' src=" . image_dir . "/icon_lock_close.gif'";
        }

        $totemAccStatus = $row['ttm_act_status'];
        if($totemAccStatus == 1) {
            $section[2] = "<span class ='text-danger'> Not Active</span>";
        } else {
            $section[2] = "<span class = 'text-success'> Active</span>";
        }

        $section[0] = "<span class = 'totemCode'>$totemCode</span>";
        $section[3] =  "<img title='Delete from Cache' class='dltTtmFromCache' src=" . image_dir . "/deleteBtn.png />";
        array_push($tableArr, $section);
    }

    $output= array(
        "draw"            => intval($_GET['draw']),
        "recordsTotal"    => intval($totals),
        "recordsFiltered" => intval($totals),
        "data"            => $tableArr
    );
    die(json_encode($output));
}

$checkactivity=false; //check_access('ttm_activity_result',$redirect=false);
$checklist=check_access('ttm_totem_list',$redirect=false);
$checktranstrack=check_access('ttm_totem_daily_transtrack');
$checkclosure=check_access('totem_closure_manage');
$checkvirtual=check_access('ttm_virtual_games', $redirect=false);
$sql = "Select jur_name , jur_class,jur_code,jps_perc_totem,jur_available_credit from jurisdiction
   		   left join jurisdiction_payment_settings on jps_jur_id=jur_id 
   		   where jur_id = " . $_REQUEST['jurid'];
$rs = $dbh->queryRow($sql);

$jur_id = $_REQUEST['jurid'];
$jur_name = $rs['jur_name'];
$jur_class = $rs['jur_class'];
$jur_credit = $rs['jur_available_credit'];


?>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<script src="<?= secure_host ?>/media/jquery.dataTables.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/js/sweetalert/sweetalert.css"  />
<script src="<?= secure_host ?>/media/js/sweetalert/sweetalert.min.js" type="text/javascript"></script>

<!-- Test -->
<style>
    #totemConnected tbody td {
        background-color: #FFFFFF;
        font-size: 11px;
        color: black;
        text-align: center;
    }

    span.ui-icon-closethick.ui-icon.ui-button-icon-primary {
        top: auto;
        left: 0%;
        right: 0%;
        bottom: 0%;
    }
    #totemName {
        min-width: 195px;
    }
    .input-style {
        min-width: 195px;
    }

    .text-success{
        color: green;
    }
    .text-danger {
        color: red;
    }

</style>

<script>
    function showClosure(parentid,parentclass,childid,childclass){
        $('#DoClosure').empty().hide();
        $('#ShowClosure').show();
    }
    function DoClosure(){
        $('#ShowClosure').hide();
        $('#DoClosure').empty().load('totem/totem_partial_closure.inc',{jurid:<?=$jur_id?>,jurclass:'<?=$jur_class?>'});
        $('#DoClosure').show();
    }


    // script that shows the decive info details in a dialog box
    $(document).on('click', '.opener', function() {
        var dialogBox = $( "#dialog");
        dialogBox.dialog("open");
        var textMessage = $(this).attr('data-info');
        dialogBox.empty().html(textMessage);
    });

    //
    $(document).on('click', '#addDeviceInfo', function() {
        var clubCode = $('#jur_code').html();
        //console.log(clubCode);
        $.ajax({
            url: '/services/totem_services.php',
            method: "POST",
            data: {
                pk: 'getTotemCodeList',
                jurCode: clubCode
            }
        }).done(function (data) {
            var totemSelect = data;
            var form = "<table>" +
                "<tr><td class='label'>Totem Code</td><td class='content'>" + totemSelect + "</td></tr> " +
                "<tr> <td class='label'>Totem Info</td> <td class='content'><textarea rows='4' cols='30' name='comment' id='totemInfo'>Enter text here...</textarea></td> </tr> " +
                "<tr> <td class='label'>Club Id</td> <td class='content'><input name='clubId' id='clubId' class='input-style' value='" +jurId +  "' disabled/></td> </tr> " +
                "<tr> <td> </td> <td> <button class='fright btn ui-state-error' id='cancelAdd'>Cancel</button><button class='fright btn ui-state-active'  id='addDevice'>Save</button> </td> </tr> " +
                "</table>";
            $('#addNewDevice').empty().html(form);
        });
    });


    // Add a new device functionality
    $(document).on('click', '#addDevice', function() {
        //get the values from the input fields
        var totemCode = $( "#ttm_code option:selected" ).val();
        var totemInfo = $('textarea#totemInfo').val();

        // send the data with an ajax request
        $.ajax({
            url: '/services/totem_services.php',
            method: "POST",
            data: {
                pk: 'addNewTotemDevice',
                ttmCode: totemCode,
                ttmInfo: totemInfo,
                ttmClubId: jurId
            }
        }).done(function (data) {
            // Reload the table
            drawDeviceInfoTable();

            //show confirmation message
            swal({
                title: "Success",
                text: "Succesfully added the new device",
                type: "success"
            });
        });
    });

    // Deletes a totem
    $(document).on('click', '.deleteTotem', function() {
        var id = $(this).attr('data-id');

        // ask for a confirmation before actually performing the delete

        $.ajax({
            url: '/services/totem_services.php',
            method: "POST",
            data: {
                pk: 'deleteDevice',
                entryId: id,
            }
        }).done(function (data) {
            // Reload the table
            drawDeviceInfoTable();
            //show confirmation message
            swal({
                title: "Success",
                text: "Succesfully delete selected device",
                type: "success"
            });
        });
    });

    // Edit device info
    $(document).on('click', '.editTotem', function () {

        totemId = $(this).attr('data-tti-id');
        // Get the values of the fields and set them accordingly
        var id = $(this).attr('data-id');
        //console.log(id);
        var row = $(this).closest('tr');
        var totemCode = row.find('td:eq(0)').text();
        var dialogValue = $(this).attr('data-info');
        if(totemCode == "-") {
            totemCode = "";
        }
        var clubCode = $('#jur_code').html();
        $.ajax({
            url: '/services/totem_services.php',
            method: "POST",
            data: {
                pk: 'getTotemCodeList',
                jurCode: clubCode
            }
        }).done(function (data) {

            var totemSelect = data;
            var form = "<table>" +
                "<tr><td class='label'>Totem Code</td><td class='content'>" + totemSelect + "</td></tr> " +
                "<tr> <td class='label'>Totem Info</td> <td class='content'><textarea rows='4' cols='30' name='comment' id='totemInfo'>Enter text here...</textarea></td> </tr> " +
                "<tr> <td class='label'>Club Id</td> <td class='content'><input name='clubId' id='clubId' class='input-style' value='" +jurId +  "' disabled/></td> </tr> " +
                "<tr> <td> </td> <td> <button class='fright btn ui-state-error' id='cancelAdd'>Cancel</button><button class='fright btn ui-state-active'  id='edtDevice'>Save</button> </td> </tr> " +
                "</table>";
            $('#addNewDevice').empty().html(form);

            // set the correct select option
            $('#ttm_code').val(id).prop('selected', true);

            // if there is no totem code, set the select to "-"
            if(id == "" ){
                $('#ttm_code').val(0).prop('selected', true);
            }

            // set the totem info
            $('#totemInfo').html(dialogValue);
        });

        $(document).off('click', '#edtDevice');
        $(document).on('click', '#edtDevice', function() {

            //console.log(totemId);
            var totemCode = $( "#ttm_code option:selected" ).val();
            var totemInfo = $('textarea#totemInfo').val();

            // Perform an ajax request to edit the data
            $.ajax({
                url: '/services/totem_services.php',
                method: "POST",
                data: {
                    pk: 'editDeviceInfo',
                    ttmCode : totemCode,
                    ttmInfo : totemInfo,
                    ttmID : totemId
                }
            }).done(function (data) {
                // Redraw the table
                drawDeviceInfoTable();
                // Display a message upon success or fail
                swal({
                    title: "Success",
                    text: "Succesfully edited the selected device",
                    type: "success"
                });
            });

        });
    });


    // Cancels the add new device functionality
    $(document).off('click', '#cancelAdd');
    $(document).on('click', '#cancelAdd', function() {
        $('#addNewDevice').empty();
    });

    //document ready actions
    $(function() {

        //initialize jquery dialog box
        dialog = $( "#dialog" ).dialog({ autoOpen: false });
        // initialize jquery tabs
        $( "#tabs" ).tabs();
        // initialize datepickers
        $('.startDate').datepicker();
        $('.endDate').datepicker();
        // ajax request on clicking pending tickets which display the table view
        $('#viewPendingTickets').on('click',function(){
            $.ajax({
                url: '/services/totem_services.php',
                method: "POST",
                data: {
                    pk: 'getPendingTickets',
                    startDate: ($('#pendingTicketsStartDate').val()),
                    endDate: $('#pendingTicketsEndDate').val(),
                    status: ($('#pendingTicketsStatus').val())
                }
            }).done(function (data) {
                $('#pendingTicketsResultContainer').empty().append(data);
            }).fail(function (error) {
                console.log(error);
                return false;
            });
        });

        jurId = <?php echo $jur_id ?>;
        jurClass = <?php echo json_encode($jur_class) ?>;

        $('#deviceInfoTab').on('click',function(){
            drawDeviceInfoTable();
        });

        // Initializing the datatable
        totemConnected = $('#totemConnected').DataTable({
            "processing": true,
            "serverSide": true,
            "ajax": {
                "url": "totem/totem_financial_detail.inc",
                "data": function (d) {
                    d.listTotemConnected = "1";
                    d.jurId = jurId;
                    d.jurClass = jurClass;
                }
            },
            'sPaginationType': "full_numbers",
            "searching": false,
            "order": [],
            "paging": false,
            "columnDefs": [{
                "orderable": false,
                "targets": [0, 1, 2, 3]
            }]
        });

    });

    $('#liveTotemSearchBox').off('keyup');
    $('#liveTotemSearchBox').on( 'keyup', function () {
        var txt = $(this).val();
        var filter, table, tr, td, i;

        filter = txt.toUpperCase();
        table = $("#totemConnected");
        tr = table.find("tr");

        for (i = 0; i < tr.length; i++) {
            var selectedTr = tr[i];
            if ($(selectedTr).hasClass('even') || $(selectedTr).hasClass('odd')) {
//                                        console.log(selectedTr);
                td = selectedTr.cells[0];
                var td2 = selectedTr.cells[2];

                if ((td.innerHTML.toUpperCase().indexOf(filter) > -1) || (td2.innerHTML.toUpperCase().indexOf(filter) > -1)) {
                    tr[i].style.display = "";
                } else {
                    tr[i].style.display = "none";
                }
            }
        }
    });


    //delete totem from cache
    $(document).off("click", ".dltTtmFromCache");
    $(document).on("click", ".dltTtmFromCache", function() {
        var totemCode = $(this).attr('data-code');
        console.log(totemCode);
        // put some sort of allert
        swal({
            title: "Delete from cache?",
            text: "Are you sure you want to delete this from cache?",
            type: "warning",
            showCancelButton: true,
        }, function(isConfirm) {
            if(isConfirm) {
                $.ajax({
                    url: '/services/totem_services.php',
                    method: "POST",
                    data: {
                        pk: 'deleteEntryFromCache',
                        ttmCode : totemCode,
                    }
                });
                swal({ title: "Deleted!",  text: "The record has been deleted.", type: "success"});
                totemConnected.ajax.reload();
            }
        });

    })

    function drawDeviceInfoTable() {
        // ajax request which generates the device info table
        $('#device-info-table-content').empty();

        $.ajax({
            url: '/services/totem_services.php',
            method: "POST",
            data: {
                pk: 'displayDeviceInfo',
                jurId: jurId
            }
        }).done(function (data) {
            //console.log(data);
            $('#device-info-table-content').empty().append(data);
        }).fail(function (error) {
            console.log(error);
            return false;
        });
    }



    $(document).ready(function(){
        <?if($checkactivity==true){ ?>
        //$('#tabs-1').load('totem/totem_financial_detail_monthly.inc',{jurid:<?=$jur_id?>,jurclass:'<?=$jur_class?>'});
        <?}if($checklist==true){?>
        $('#tabs-2').load('totem/totem_financial_detail_totemlist.inc',{jurid:<?=$jur_id?>,jurclass:'<?=$jur_class?>'});
        <?}if($checktranstrack==true){?>
        $('#tabs-3').load('totem/totem_financial_detail_clublist.inc',{jurid:<?=$jur_id?>,jurclass:'<?=$jur_class?>'});
        <? } ?>
        //$('#tabs-4').load('totem/totem_deposit.inc',{jurid:<?=$jur_id?>,jurclass:'<?=$jur_class?>'});
        /* <?if($checkvirtual==true){?>
         $('#tabs-5').load('totem/totem_virtual_games.inc',{jurid:<?=$jur_id?>,jurclass:'<?=$jur_class?>'});
        <? } ?> */
        $('#totemnr').load('totem/totem_query_service.inc',{action:'4',jurid:<?=$jur_id?>,jurclass:'<?=$jur_class?>'});
    });

</script>
<style>
    .fwhite {
        color: floralwhite;
    }
</style>
<div id="add_part"></div>
<div id='part' >
    <button type="button" class="close" style="color: darkblue;cursor:pointer" onclick="hideClosure()">X</button>
    <div id='partialclosure' style='max-height: 650px;overflow: auto'></div>
    <div id='closureresult'></div>
</div>

<div class="demo" style="margin-top:20px;">
    <h2><?=strtoupper($jur_class)?>: <span class="<?=$jur_class?>"><?=strtoupper($jur_name)?> </span>-  &nbsp; <?=$lang->getLang("Percentage")?>:  <span class="<?=$jur_class?>"><?=strtoupper($rs['jps_perc_totem'])?>% </span>-  &nbsp; <?=$lang->getLang("Credits")?>:    <span class="<?=$jur_class?>"> <?=getInDollars($jur_credit)?></span><?if($checklist==true){ ?> <span id="totemnr"></span><?}?>  -  &nbsp; <?=$lang->getLang("Agency Code")?>:  <span class="<?=$jur_class?>" id="jur_code"><?=strtoupper($rs['jur_code'])?> </span> </h2>
    <div style="margin-top:30px">
        <div id="tabs">
            <ul>
                <?if($checkactivity==true){ ?>	<li><a href="#tabs-1"><?=$lang->getLang("Totem Activity Result")?></a></li>
                <?}if($checklist==true){?><li><a href="#tabs-2"><?=$lang->getLang("Totem List")?></a></li>
                <?}if($checktranstrack==true){?><li><a href="#tabs-3"><?=$lang->getLang("Totem Financial")?></a></li><?}?>
                <!-- 	<li><a href="#tabs-4">Totem Deposit</a></li> -->
                <!--<?if($checkvirtual==true){?><li><a href="#tabs-5"><?=$lang->getLang("Virtual Games")?></a></li><?}?> -->
                <?if($checkclosure==true){ ?>
                    <li><a href="#tabs-6"><?=$lang->getLang("Partial Closure")?></a></li>
                <?}?>
                <li><a href="#totemPendingTicketsContainer"><?=$lang->getLang("Pending ticket")?></a></li>
                <li><a href="#totemConnectedContainer"><?=$lang->getLang("Totem connected")?></a></li>
                <?php if($jur_class == 'club')  {?>
                    <li><a href="#deviceInfo" id="deviceInfoTab"><?=$lang->getLang("Device info")?></a></li>
                <?php } ?>
            </ul>
            <?if($checkactivity==true){ ?>	<div id="tabs-1"></div>
            <?}if($checklist==true){?><div id="tabs-2"></div>
            <?}if($checktranstrack==true){?><div id="tabs-3"></div><?}?>
            <!-- 	<div id="tabs-4"></div> -->
            <!--<?if($checkvirtual==true){?><div id="tabs-5"></div><?}?> -->
            <?if($checkclosure==true){ ?>
                <div id="tabs-6">
                    <div style="padding: 20px;margin-bottom: 10px;" class='ui-widget-header'><div style="float:left;margin-top: -10px;" class="ui-state-active" ><input type="radio" name='closuretype' onclick="showClosure()"><?=$lang->getLang("Search Closure")?></div>
                        <div style="float:right;margin-top: -10px;" class="ui-state-active" ><input type='radio' name='closuretype' onclick="DoClosure()" ><?=$lang->getLang("Do closure")?></div>
                    </div>
                    <div id='DoClosure' class='hidden'>
                    </div>
                    <div id='ShowClosure' class='hidden'>
                        <br><div id='ShowClosureSearch' >
                            <input type="checkbox" name='cashin'<?if($_SESSION['jurisdiction_class']=='club'){?>disabled="disabled"<?}?>><?=$lang->getLang("Cash-In")?>
                            <input type="checkbox" name='cashout'<?if($_SESSION['jurisdiction_class']=='casino'){?>disabled="disabled"<?}?>><?=$lang->getLang("Cash-Out")?>
                            <button id='ShowClosureButton'><?=$lang->getLang("Search")?></button>
                            <script>
                                $(function() {
                                    $( '#ShowClosureButton')
                                        .button()
                                        .click(function() {
                                            $.post("totem/totem_query_service.inc",{action:'16',cashin:$("input[name=cashin]:checked").val(), cashout:$("input[name=cashout]:checked").val()}, function(data){
                                                $('#ShowClosureResult').empty().append(data);
                                            });
                                        });
                                    return false;
                                });
                            </script>
                        </div>
                        <br><div id='ShowClosureResult'>
                        </div>
                    </div>
                </div>
            <? }?>
            <div id="totemPendingTicketsContainer">
                <p class="bodyHD"><?=$lang->getLang("Pending tickets")?>:</p>
                <div class="fleft" style="width: 50%">
                    <h4>Period</h4>
                    <table>
                        <tr>
                            <td class="label"><?=$lang->getLang("From")?>:</td>
                            <td class="content"><input type="text"  class="startDate" id="pendingTicketsStartDate" value="<?=date('Y-m-d 00:00:00')?>" /></td>
                        </tr>
                        <tr>
                            <td class="label"><?=$lang->getLang("Until")?>:</td>
                            <td class="content">
                                <input type="text"  name="date_end" class="endDate" id="pendingTicketsEndDate" value="<?=date('Y-m-d 23:59:59')?>" /></td>
                        </tr>
                    </table>
                    <div class="clearfix"></div>
                </div>
                <div class="fright" style="width: 50%">
                    <h4>Status</h4>
                    <select id="pendingTicketsStatus">
                        <option value="-1">All</option>
                        <option value="0">Open</option>
                        <option value="1">Reused</option>
                        <option value="2">Closed</option>
                    </select>
                    <div class="clearfix"></div>
                </div>

                <div class="clearfix"></div><br />
                <button id="viewPendingTickets" class="btn btn-primary"><?=$lang->getLang("View")?></button>
                <div class="clearfix"></div>
                <br /><div id="pendingTicketsResultContainer"></div>
            </div>

            <div id="totemConnectedContainer">
                <table id="totemConnected" style="width: 100%">
                    <input type="text" id="liveTotemSearchBox" class="" placeholder="Search totem code">
                    <thead>
                    <tr>
                        <th class="ttm_label"><?=$lang->getLang("Totem code")?></th>
                        <th class="ttm_label"><?=$lang->getLang("Access")?></th>
                        <th class="ttm_label"><?=$lang->getLang("Status")?></th>
                        <th class="ttm_label"><?=$lang->getLang("Management")?></th>
                    </tr>
                    </thead>
                </table>
            </div>

            <div id="deviceInfo">
                <div class="" style="width:50%; display:inline;">
                    <button class="btn ui-state-active" id="addDeviceInfo">Add</button>
                </div>
                <br />
                <br />
                <div id="addNewDevice"></div>
                <br />
                <br />
                <div id="device-info-table-content"></div>
                <div id="dialog" title="Details"></div>

            </div>

        </div>
    </div>
    <div style="display: none" id="resetClub" title="Reset all totems?">
        <p><span class="ui-icon ui-icon-alert" style="float: left; margin: 0 7px 20px 0;"></span><?=$lang->getLang("You are attempting to reset all the totems under this club.Are you sure?")?></p>
    </div>
    <div style="display: none;" class="demo-description">
        <p><?=$lang->getLang("Click tabs to swap between content that is broken into logical sections")?>.</p>
    </div><!-- End demo-description -->
</div>
