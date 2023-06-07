<?php
/**
 *
 * @author Marian
 */
check_access('virtual_netbet',true);
global $lang;
require_once('virtual_functions.inc');
$date_start = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d", time());
$date_end = (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d", time() + (60 * 60 * 24));
$tid = $_POST['ticketid'] ;
$game=$_POST['games'];
?>
<style>
    .accordion a {
        text-decoration: none;
    }
    .accordion a:hover {
        text-decoration: none;
    }
    .ui-multiselect-checkboxes label input {
        vertical-align: inherit;
        margin: 5px;
    }
    .ui-multiselect{
        max-width: none;
    }
    .searchTable{
        background-color: transparent !important;
        width:440px !important;
        margin-left: 20px;
    }

    .dropdown-menu > .active > a, .dropdown-menu > .active > a:hover, .dropdown-menu > .active > a:focus {
        color: rgb(255, 255, 255);
        text-decoration: none;
        background-color: rgb(218, 218, 218) !important;
        background-image: -moz-linear-gradient(top, rgb(208, 208, 208), rgb(111, 111, 111)) !important;
        background-image: -webkit-gradient(linear, 0 0, 0 100%, from(rgb(208, 208, 208)), to(rgb(111, 111, 111))) !important;
        background-image: -webkit-linear-gradient(top, rgb(208, 208, 208), rgb(111, 111, 111)) !important;
        background-image: -o-linear-gradient(top, rgb(208, 208, 208),rgb(111, 111, 111)) !important;
        background-image: linear-gradient(to bottom, rgb(208, 208, 208), rgb(111, 111, 111)) !important;
    }

    .dropdown-menu > li > a:focus, .dropdown-submenu:hover > a, .dropdown-submenu:focus > a {
        background-color: rgb(218, 218, 218) !important;
        background-image: -moz-linear-gradient(top, rgb(208, 208, 208), rgb(111, 111, 111)) !important;
        background-image: -webkit-gradient(linear, 0 0, 0 100%, from(rgb(208, 208, 208)), to(rgb(111, 111, 111))) !important;
        background-image: -webkit-linear-gradient(top, rgb(208, 208, 208), rgb(111, 111, 111)) !important;
        background-image: -o-linear-gradient(top, rgb(208, 208, 208),rgb(111, 111, 111)) !important;
        background-image: linear-gradient(to bottom, rgb(208, 208, 208), rgb(111, 111, 111)) !important;
        filter: progid:DXImageTransform.Microsoft.gradient(startColorstr='#ff0088cc', endColorstr='#ff0077b3', GradientType=0);
    }
    .dropdown-menu > li > a:hover, .dropdown-menu > li > a:focus, .dropdown-submenu:hover > a, .dropdown-submenu:focus > a {
        color: rgb(255, 255, 255);
        text-decoration: none;
        background-color: rgb(218, 218, 218) !important;
        background-image: -moz-linear-gradient(top, rgb(208, 208, 208), rgb(111, 111, 111)) !important;
        background-image: -webkit-gradient(linear, 0 0, 0 100%, from(rgb(208, 208, 208)), to(rgb(111, 111, 111))) !important;
        background-image: -webkit-linear-gradient(top, rgb(208, 208, 208), rgb(111, 111, 111)) !important;
        background-image: -o-linear-gradient(top, rgb(208, 208, 208),rgb(111, 111, 111)) !important;
        background-image: linear-gradient(to bottom, rgb(208, 208, 208), rgb(111, 111, 111)) !important;
        background-repeat: repeat-x;
        filter: progid:DXImageTransform.Microsoft.gradient(startColorstr='#ff0088cc', endColorstr='#ff0077b3', GradientType=0);
    }

</style>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/dataTablesExtra/TableTools-2.2.0/css/dataTables.tableTools.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="/media/bootstrap/css/bootstrap-multiselect.css" title="core_style">


<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap-multiselect.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/jquery.dataTables.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/jstree/jstree.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/highcharts.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/modules/exporting.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/dataTablesExtra/TableTools-2.2.0/js/dataTables.tableTools.js" type="text/javascript"></script>
<script type=text/javascript>
    $("body").on({
        ajaxStart: function() {
            $('#myModal').show();
        },
        ajaxStop: function() {
            $('#myModal').hide();
        }
    });
    $(function() {
        var jurisdictions=<?=$_SESSION['jurisdiction_id']?>;
        $( "body" ).on( "click", ".cancel", function() {
            $(this).closest('td').find('.plm').popover('hide');
        });


        $( "body" ).on( "click", ".confirmdeposit", function() {
            thisButton=$(this);
            var tr=thisButton.closest('tr')
            customer=tr.find('input[name="customer"]').val();
            amount=$(this).prev().val();
            if(!$.isNumeric(amount)){
              jAlert('Please insert a correct amount');
              thisButton.siblings('.depositResultDiv').empty();
            }else{
                $.post("/virtual/netbet/virtual_functions.inc",{action:"5",type:'deposit',amount:amount,customer:customer}, function(data){
                    if(data[0]!=1){
                     jAlert(data);
                        thisButton.siblings('.depositResultDiv').empty();
                 }
                    else{
                       thisButton.siblings('.depositResultDiv').empty().append( "<h6 class='text-success'><?=$lang->getLang("Deposit successful")?></h6>" );
                      tr.find('td:eq(4)').html(data.substr(1));
                        getCredits();
                    }
                });
            }
        });

        $( "body" ).on( "click", ".confirmwithdraw", function() {
            thisButton=$(this);
            var tr=thisButton.closest('tr')
            customer=tr.find('input[name="customer"]').val();
            amount=$(this).prev().val();
            if(!$.isNumeric(amount)){
                jAlert('Please insert a correct amount');
                thisButton.siblings('.depositResultDiv').empty();
            }else{
                $.post("/virtual/netbet/virtual_functions.inc",{action:"5",type:'withdraw',amount:amount,customer:customer}, function(data){
                    if(data[0]!=1){
                        jAlert(data);
                        thisButton.siblings('.depositResultDiv').empty();
                    }
                    else{
                        thisButton.siblings('.depositResultDiv').empty().append( "<h6 class='text-success'><?=$lang->getLang("Withdraw successful")?></h6>" );
                        tr.find('td:eq(4)').html(data.substr(1));
                        getCredits();
                    }

                });
            }

        });
        $('#userTypeContainer').hide();


        var to = false;
        $( "#date_start" ).datepicker({
            changeMonth: true,
            changeYear:true,
            numberOfMonths:1

        });
        $( "#date_start_fin" ).datepicker({
            changeMonth: true,
            changeYear:true,
            numberOfMonths:1

        });
        $( "#date_start_ov" ).datepicker({
            changeMonth: true,
            changeYear:true,
            numberOfMonths:1

        });
        $( "#date_start_paid" ).datepicker({
            changeMonth: true,
            changeYear:true,
            numberOfMonths:1

        });
        $("#SearchTicket").click(function() {
            $('#SearchTicket').button('loading');


            ticket=$('input[name="ticketid"]').val();
            /* if(!/(^-?\d| )/.test(ticket)){
             $('#ticketid').focus();
             jAlert('Please enter a valid ticket id!');
             return false;
             }*/

            date_start=$('input[name="date_start"]').val();
            games=  $('#games').find('option:selected').val()
            $.post("/virtual/netbet/virtual_functions.inc",{action:"1",
                section:'ticket',
                ticket:ticket,
                date_start:date_start,
                games:games
            }, function(data){
                $("#ticket_result").empty().append(data);
                $(".displayTicket").dataTable({'sPaginationType':"full_numbers","sDom": 'T<"clear">lfrtip',"oTableTools": {
                    "aButtons": [
                        "copy",
                        "csv",
                        "xls",
                        "pdf"
                    ]
                }
                });
                $('#SearchTicket').button('reset');
            });
        });

        $('#SearchJurisdictions').click(function() {
            jurisdictions=<?=$_SESSION['jurisdiction_id']?>;
                $.post("/virtual/virtual_functions.inc",{
                    action:"1",
                    jurisdictions:jurisdictions,
                    section:'jurisdiction',
                    allSelected:$('#selectAll').is(":checked")
                }, function(data){
                    $("#info").empty().append(data);
                    $(".displayInfo").dataTable({'iDisplayLength': 100,'sPaginationType':"full_numbers","sDom": 'T<"clear">lfrtip',"oTableTools": {
                        "aButtons": [
                            "copy",
                            "csv",
                            "xls",
                            "pdf"
                        ]
                    }
                    });
                });
        });

        $('#SearchTicketPaid').click(function() {
                $('#SearchTicketPaid').button('loading');
                selectedGames=$("#gamesPaid").val();
                date_start=$('input[name="date_start_paid"]').val();
                ticket=$('input[name="ticketidPaid"]').val();
                if(selectedGames=='' || typeof selectedGames=='undefined' || selectedGames==null){
                    jAlert('<?=$lang->getLang("Please select at least one game!")?>');
                    $('#SearchTicketPaid').button('reset');
                }else{
                    $.post("/virtual/virtual_functions.inc",{
                        action:"1",
                        date_start:date_start,
                        section:'ticketPaid',
                        games:JSON.stringify(selectedGames),
                        allSelected:$('#selectAll').is(":checked"),
                        winmajor:$('#winmajor').is(":checked"),
                        ticket:ticket
                    }, function(data){
                        data=JSON.parse(data);
                        $("#ticket_result_paid").empty().append(data);
                        $(".displayTicketPaid").dataTable({'iDisplayLength': 100,'sPaginationType':"full_numbers","sDom": 'T<"clear">lfrtip',"oTableTools": {
                            "aButtons": [
                                "xls",
                                {
                                    "sExtends": "pdf",
                                    "sPdfOrientation": "landscape"
                                }
                            ]
                        }
                        });
                        $('#SearchTicketPaid').button('reset');
                    });
            }
        });




        $('#SearchFinancial').click(function() {
            $('#SearchFinancial').button('loading');
            date_start=$('input[name="date_start_fin"]').val();
            group= $('#groupByFin').find('option:selected').val();

                $.post("/virtual/netbet/virtual_functions.inc",{action:"1",jurisdictions:jurisdictions,group:group,date_start:date_start,section:'financial'}, function(data){
                    $("#financial_result").empty().append(data);
                    $(".displayFinancial").dataTable({'iDisplayLength': 10,'sPaginationType':"full_numbers","sDom": 'T<"clear">lfrtip',"oTableTools": {
                        "aButtons": [
                            "copy",
                            "csv",
                            "xls",
                            "pdf"
                        ]
                    }
                    });
                    $('#SearchFinancial').button('reset');
                });

        });


        $('#groupBy').change(function() {
            var group_sel=$(this).find('option:selected').val();
            if(group_sel=='1' ){
                $('#userTypeContainer').show();
            }
            else{
                $('#userType').val('0');
                $('#userTypeContainer').hide();
            }
        });

        $('#gamesPaid').multiselect({
            maxHeight: 300,
            includeSelectAllOption: true,
            buttonWidth:'270px'
        });
    });


    function getConnectedUsers(container,club){
        $.post("/virtual/netbet/virtual_functions.inc",{action:"2",club:club}, function(data){
            $(container).parent().empty().append(data);
        });

    }
</script>
<div class="modal" id="myModal" style="display: none;margin-top:15%">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-body centered" >
                Loading... <img src="media/images/ajax-loader.gif">
            </div>
        </div>
    </div>
</div>
<div class="container" style="position: absolute;left:0;right: 0;width: 100%" >
    <table class="table table-bordered">
        <tr>
            <td class="navbar-inner" colspan="2"><h3 class="text-center"><?=$lang->getLang("Virtual Management")?></h3></td>
        </tr>
        <tr>
            <td style='vertical-align: top'>
                <div class="well">
                    <div class="tabbable">
                        <ul class="nav nav-tabs">
                            <?  if(check_access('virtual_agency_info')) {?>
                                <li class="active"><a href="#info" data-toggle="tab"><button class="btn btn-inverse" id="SearchJurisdictions"><?=$lang->getLang("Info")?></button></a></li>
                            <? } ?>
                            <?  if(check_access('virtual_management_played')) {?>
                            <li><a href="#ticket" data-toggle="tab"><button class="btn btn-inverse"><?=$lang->getLang("Played Status")?></button></a></li>
                            <? } if(check_access('virtual_management_financial')) {?>
                            <li><a href="#financial" data-toggle="tab"><button class="btn btn-inverse"><?=$lang->getLang("Financial")?></button></a></li>
                            <? } ?>

                        </ul>
                        <div class="tab-content">
                            <?if(check_access('virtual_agency_info')) {?>
                            <div class="tab-pane active" id="info">
                                <p><?=$lang->getLang("Press on 'Info' button")?></p>
                            </div>
                            <? } ?>



                               <div class="tab-pane" id="ticket">
                                <p>
                                   <div class="tabbable  tabs-left">
                                       <ul class="nav nav-tabs">
                                           <li class="active"><a href="#playedP" data-toggle="tab"><?=$lang->getLang("Played")?></a></li>
                                           <li><a href="#playedPaid" data-toggle="tab"><?=$lang->getLang("Paid")?></a></li>
                                       </ul>
                                       <div class="tab-content">
                                           <div class="tab-pane active" id="playedP" >

                                               <div class='row'>
                                                   <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Day")?></div>
                                                       <br>
                                                       <div class="controls">
                                                           <div class="input-prepend">
                                                               <span class="add-on"><i class="icon-calendar"></i></span>
                                                               <input type='text' id="date_start"  name='date_start'  placeholder="Date Start" value="<?=$date_start?>">
                                                           </div>
                                                       </div>
                                                   </div>
                                                   <?php
                                                   $sql = "select gam_id, gam_name from game where gam_category='casino_agency'";
                                                   $resGames = $dbh->doCachedQuery($sql,0);
                                                   $gameList = array();
                                                   while($resGames->hasNext()){
                                                       $row = $resGames->next();
                                                       $gameList[$row['gam_id']] = $row['gam_name'];
                                                   }
                                                   $activatedGames = $_POST['games'];
                                                   ?>
                                                   <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Games")?></div>
                                                       <br>
                                                       <div class="controls">
                                                           <div class="input-prepend">
                                                               <span class="add-on"><i class="icon-random"></i></span>
                                                               <select name='games' id='games' >
                                                                   <?php foreach($gameList as $key => $value){
                                                                       ?>
                                                                       <option  value="<?=$key?>"<?php if($activatedGames==$key){?>selected="selected" <?php  }?>  ><?=$value?> </option><?php
                                                                   } ?>
                                                               </select>
                                                           </div>
                                                       </div>
                                                   </div>
                                                   <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Ticket Id")?></div>
                                                       <br>
                                                       <div class="controls">
                                                           <div class="input-prepend">
                                                               <span class="add-on"><i class="icon-list-alt"></i></span>
                                                               <input type='text'  name='ticketid' id='ticketid' placeholder="Ticket Id ( Optional )" maxlength="32" value="">
                                                           </div>
                                                       </div>
                                                   </div>
                                               </div>
                                               <div class="row">
                                                   <span class="span2"> <button class="btn btn-primary" id="SearchTicket" data-loading-text="<?=$lang->getLang("Loading. Please wait...")?>"><?=$lang->getLang("Search")?></button></span>
                                               </div><br /><br />
                                               <div id="ticket_result"></div>
                                           </div>
                                           <div class="tab-pane" id="playedPaid" style="min-height: 355px">
                                               <div class='row'>
                                                   <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Day")?></div>
                                                       <br>
                                                       <div class="controls">
                                                           <div class="input-prepend">
                                                               <span class="add-on"><i class="icon-calendar"></i></span>
                                                               <input type='text' id="date_start_paid"  name='date_start_paid' class="timeHelper" placeholder="Date Start" value="<?=$date_start?>">
                                                           </div>
                                                       </div>
                                                   </div>
                                                   <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Games")?></div>
                                                       <br>
                                                       <div class="controls">
                                                           <select name='gamesPaid[]' id='gamesPaid' multiple="multiple">
                                                               <?php foreach($gameList as $key => $value){
                                                                   ?>
                                                                   <option  value="<?=$key?>" selected ><?=$value?> </option><?php
                                                               } ?>
                                                           </select>
                                                       </div>
                                                   </div>
                                                   <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Ticket Id")?></div>
                                                       <br>
                                                       <div class="controls">
                                                           <div class="input-prepend">
                                                               <span class="add-on"><i class="icon-list-alt"></i></span>
                                                               <input type='text'  name='ticketidPaid' id='ticketidPaid' placeholder="<?=$lang->getLang("Ticket Id")?> ( Optional )" maxlength="32" value="">
                                                           </div>
                                                       </div>
                                                   </div>
                                                   <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Win")?></div>
                                                       <br>
                                                       <label class="checkbox">
                                                           <input type="checkbox"  id="winmajor" checked> Win > 0
                                                       </label>
                                                   </div>
                                               </div>
                                               <div class="row">
                                                   <span class="span2"> <button class="btn btn-primary" id="SearchTicketPaid"  data-loading-text="<?=$lang->getLang("Loading. Please wait...")?>" ><?=$lang->getLang("Search")?></button></span>
                                               </div><br /><br />
                                               <div id="ticket_result_paid"></div>
                                               <br />
                                           </div>
                                       </div>
                                   </div>
                                </p>

                            </div>
                            <div class="tab-pane" id="financial">
                                <p>
                                <div class='row'>
                                    <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Day")?></div>
                                        <br>
                                        <div class="controls">
                                            <div class="input-prepend">
                                                <span class="add-on"><i class="icon-calendar"></i></span>
                                                <input type='text' id="date_start_fin"  name='date_start_fin'  placeholder="Date Start" value="<?=$date_start?>">
                                            </div>
                                        </div>
                                    </div>
                                    <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Group by")?></div>
                                        <br>
                                        <div class="controls">
                                            <div class="input-prepend">
                                                <span class="add-on"><i class="icon-align-justify"></i></span>
                                                <select name="groupBy" id="groupByFin">
                                                    <!--   <option value="2"><?/*=$lang->getLang("Financial")*/?></option>
                                                    <option value="0"><?=$lang->getLang("Club")?></option>-->
                                                    <option value="1"><?=$lang->getLang("Cashier")?></option>
                                                    <option value="3"><?=$lang->getLang("Game")?></option>
                                                </select>
                                            </div>
                                        </div>
                                    </div>
                                </div>
                                <div class="row">
                                    <span class="span2"> <button class="btn btn-primary" id="SearchFinancial" data-loading-text="<?=$lang->getLang("Loading. Please wait...")?>"><?=$lang->getLang("Search")?></button></span>
                                </div><br /><br />
                                <div id="financial_result">
                                </div>
                                </p>
                            </div>
                        </div>
                    </div>
                </div>
            </td>
        </tr>
    </table>
</div>