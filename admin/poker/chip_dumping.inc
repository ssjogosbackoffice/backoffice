<?
/* @author Marian */
check_access('poker_view_chip_dumping');
$active=(isset($_POST['active'])?$_POST['active']:'clients');
$groupBy=(isset($_POST['groupBy'])?$_POST['groupBy']:'1');
$date_start = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d", time());
$date_end = (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d");
$users=$dbh->exec('Select * from poker.clients');
$users=$users->Records;
?>

<script src="<?= secure_host ?>/media/js/dataTables/1.10.15/datatables.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/auth_functions.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<script src="<?= secure_host ?>/media/ui/js/jquery.timepicker.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media//select2/select2.css" >
<script src="<?= secure_host ?>/media/select2/select2.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
<script src="<?= secure_host ?>/media/jstree3/jstree.js" type="text/javascript"></script>

<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/js/sweetalert/sweetalert.css"  />
<script src="<?= secure_host ?>/media/js/sweetalert/sweetalert.min.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/js/moment.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/js/dataTables/1.10.15/datatables.css"  />
<style>
    .paging_full_numbers {
        height: auto;
        line-height:auto;
    }
</style>

<script type="text/javascript">
    var servicesUrl = "/services/general_services.inc";
    $(function() {
        $('#jurisdictionTreeSearch').keyup(function () {
            if(this.value.length>2){
                $("#jurisdictionTree").jstree('search',$('#jurisdictionTreeSearch').val());
            }
            else{
                $("#jurisdictionTree").jstree("clear_search");
                $('#jurAlert').fadeOut();
            }
        });


        $tree=  $("#jurisdictionTree")
            .jstree({
                "plugins" : [
                    "search",
                    "types",
                    "state"
                ],
                "search":{
                    "fuzzy":false,
                    "show_only_matches":true
                },
                "types" : {
                    "partner" : {
                        "icon" : "../media/images/p.png"
                    },
                    "nation" : {
                        "icon" : "../media/images/n.png"
                    },
                    "district" : {
                        "icon" :  "../media/images/c.png"
                    },
                    "club" : {
                        "icon": "../media/images/a.png"
                    }
                }
            }).bind("search.jstree", function (e, data) {
                if (data.nodes.length == 0){
                    $('#jurAlert').fadeIn();
                }
                else{
                    $('#jurAlert').fadeOut();
                }
            });
        $('.usernameSelect').select2();
        $( ".dateStartPicker" ).datetimepicker({
            changeMonth: true,
            numberOfMonths:1
        });

        $( "#reportDate" ).datepicker({
            changeMonth: true,
            changeYear: true,
            showButtonPanel: true,
            dateFormat: 'yy-mm',
            onClose: function(dateText, inst) {
                var month = $("#ui-datepicker-div .ui-datepicker-month :selected").val();
                var year = $("#ui-datepicker-div .ui-datepicker-year :selected").val();
                $(this).datepicker('setDate', new Date(year, month, 1));
            }
        });

        $("#reportDate").focus(function () {
            $(".ui-datepicker-calendar").hide();
            $("#ui-datepicker-div").position({
                my: "center top",
                at: "center bottom",
                of: $(this)
            });
        });


        // clients
        var clientsTable=$('#clientsTable').DataTable({
            "processing": true,
            "serverSide": true,
            "ajax": {
                "url": servicesUrl,
                "data": function (d) {
                    d.action = "getChipDumpingClients";
                }
            }, 'iDisplayLength': 10, 'sPaginationType': "full_numbers"
        });
        $('#btnSaveClients').on('click',function () {
            $.ajax({
                url: servicesUrl,
                method: "POST",
                data:{
                    action:'saveChipDumpingClient',
                    name:$('#clientName').val(),
                    type:$('#clientType').val(),
                    id:$('#chipDumpingClientId').val()
                },
                dataType:'json'

            }) .done(function(data) {

                if(data[0]=='1'){
                    swal("Success", "Successfully saved", "success");
                    clientsTable.ajax.reload();
                    return false;

                }


                swal({
                    title: "Error!",
                    text: data[1],
                    type: "error"
                });

                return false;

            })
                .fail(function(error) {
                    swal({
                        title: "Error!",
                        text: "Something went wrong, chip dumping not deleted",
                        type: "error"
                    });
                });
        });
        $(document).on('click','.modifyClients',function(){
            $('a[href="#manageclient"]').trigger('click');
            $('#clientName').val($(this).closest('tr').find('td:nth-child(2)').html());
            $('#chipDumpingClientId').val($(this).closest('tr').find('td:nth-child(1)').html())

        });
        $('a[href="#manageclient"]').on('click',function(){
            $('#chipDumpingClientId').val('');
        });


         // chip dumping

        function getTotals(){
            $.ajax({
                url: servicesUrl,
                method: "POST",
                data:{
                    action:'getChipDumpingTotals',
                    jur : $('#clubClients').val(),
                    type : $('#chipDumpingClientType').val(),
                    startDate : $('#chipDumpingStartDate').val(),
                     endDate : $('#chipDumpingEndDate').val()
                }

            }) .done(function(data) {
                  $('#chipDumpingTotals').empty().append(data);


            })
                .fail(function(error) {

                });
        }
        getTotals();
        var chipDumpingTable=$('#chipDumpingTable').DataTable({
            "processing": true,
            "serverSide": true,
            "ajax": {
                "url": servicesUrl,
                "data": function (d) {
                    d.action = "getChipDumping";
                    d.jur = $('#clubClients').val();
                    d.type = $('#chipDumpingClientType').val();
                    d.startDate = $('#chipDumpingStartDate').val();
                    d.endDate = $('#chipDumpingEndDate').val();
                }
            }, 'iDisplayLength': 10, 'sPaginationType': "full_numbers"
        });
        $('#btnSaveChipDumping').on('click',function () {
            var jurisdiction=($tree.jstree(true).get_selected(true));
            $.ajax({
                url: servicesUrl,
                method: "POST",
                data:{
                    action:'saveChipDumping',
                    username:$('#username').val(),
                    club:$('#club').val(),
                    from:$('#from').val(),
                    receivefrom:$('#receivefrom').val(),
                    giveto:$('#giveto').val(),
                    to:$('#to').val(),
                    type:$('#type').val(),
                    amount:$('#amount').val(),
                    date:$('#date').val(),
                    jur:jurisdiction[0].id,
                    chipdumpingid:$('#chipDumpingId').val()


                },
                dataType:'json'

            }) .done(function(data) {

                if(data[0]=='1'){

                    swal("Success", "Successfully saved", "success");

                    chipDumpingTable.ajax.reload();
                    return false;

                }

                swal({
                    title: "Error!",
                    text: data[1],
                    type: "error"
                });
                return false;

            })
                .fail(function(error) {
                    swal({
                        title: "Error!",
                        text: "Something went wrong, chip dumping not deleted",
                        type: "error"
                    });
                });
        });

        $(document).on('click','.btnDelete',function(){
            var id=$(this).closest('tr').find('td:nth-child(1)').html();



            swal({
                    title: "Are you sure?",
                    text: "Are you sure you want to delete this dumping?",
                    type: "warning",
                    showCancelButton: true,
                    confirmButtonColor: "#DD6B55",
                    confirmButtonText: "Yes, delete it!",
                    closeOnConfirm: false
                },
                function(){
                    $.ajax({
                        url: servicesUrl,
                        method: "POST",
                        data:{
                            action:'deleteChipDumping',
                            id:id
                        }

                    }) .done(function(data) {

                        if(data=='1'){
                            chipDumpingTable.ajax.reload();
                            swal("Success", "Successfully deleted chip dumping", "success");
                            return false;

                        }
                        swal({
                            title: "Error!",
                            text: "Something went wrong, chip dumping not deleted",
                            type: "error"
                        });
                        return false;

                    })
                        .fail(function(error) {
                            swal({
                                title: "Error!",
                                text: "Something went wrong",
                                type: "error"
                            });
                        });
                });

        });

        $(document).on('click','.btnModify',function(){
            $('a[href="#managechipdumping"]').trigger('click');
            $('#jurisdictionSelector').hide();
            var id=$(this).closest('tr').find('td:nth-child(1)').html();
            $('#date').val($(this).closest('tr').find('td:nth-child(2)').html());
            var club=$(this).closest('tr').find('td:nth-child(3)').html();
            $("#club option").filter(function() {
                return $(this).text()==club;
            }).prop('selected', true);
            $("#club").trigger('change.select2');
            $('#username').val($(this).closest('tr').find('td:nth-child(4)').html());
            $('#type').val($(this).closest('tr').find('td:nth-child(5)').html()=='Give'?'give':'receive').trigger('change'),
            $('#amount').val($(this).closest('tr').find('td:nth-child(6)').html());

            var clubfrom=$(this).closest('tr').find('td:nth-child(7)').html();
            $("#from option").filter(function() {
                return $(this).text()==clubfrom;
            }).prop('selected', true);
            $("#from").trigger('change.select2');
            $('#receivefrom').val($(this).closest('tr').find('td:nth-child(8)').html());
            var clubto=$(this).closest('tr').find('td:nth-child(9)').html();
                $("#to option").filter(function() {
                    return $(this).text()==clubto;
                }).prop('selected', true);
            $("#to").trigger('change.select2');
            $('#giveto').val($(this).closest('tr').find('td:nth-child(10)').html());
                $('#chipDumpingId').val(id);
        });

        $(document).on('click','#getChipDumping',function(){
            chipDumpingTable.ajax.reload();
            getTotals();
        });
        $('a[href="#managechipdumping"]').on('click',function(){
            $('#jurisdictionSelector').show();
            $('#chipDumpingForm')[0].reset();
            $("#club").trigger('change.select2');
            $("#from").trigger('change.select2');
            $("#to").trigger('change.select2');

            $('#chipDumpingId').val('');
        });




        // reports

        var reportsTable=$('#reportsTable').DataTable({
            "processing": true,
            "serverSide": true,
            ordering: false,
            "dom": 'B<"clear">t',

            "footerCallback": function ( row, data, start, end, display ) {
                var api = this.api(), data;

                // Remove the formatting to get integer data for summation
                var intVal = function ( i ) {
                    return typeof i === 'string' ?
                        i.replace(/[\$,]/g, '')*1 :
                        typeof i === 'number' ?
                            i : 0;
                };

                // Total over all pages
                totalPasa = api
                    .column( 2 )
                    .data()
                    .reduce( function (a, b) {
                        return intVal(a) + intVal(b);
                    }, 0 );

                totalReceive = api
                    .column( 3 )
                    .data()
                    .reduce( function (a, b) {
                        return intVal(a) + intVal(b);
                    }, 0 );

                totalDiff = api
                    .column( 4 )
                    .data()
                    .reduce( function (a, b) {
                        return intVal(a) + intVal(b);
                    }, 0 );

                // Total over this page

                $( api.column( 2 ).footer() ).html(
                    '$'+totalPasa
                );
                $( api.column( 3 ).footer() ).html(
                    '$'+totalReceive
                );
                $( api.column( 4 ).footer() ).html(
                    '$'+totalDiff
                );
            },
            "buttons": [
                {
                    extend: 'excel',
                    text: 'Export data',
                    title:function(){return moment().format('YYYY-MM-DD')+$("#reportClients").select2('data').text} ,
                    extension: '.xlsx',
                    footer:true,
                    sheetName:function(){return $("#reportClients").select2('data').text},
                    customize: function ( xlsx ){
                        var sheet = xlsx.xl.worksheets['sheet1.xml'];


                        $('row c[r*="1"]', sheet).attr( 's', '15' );
                        $('c[r=G1]', sheet).attr( 's', '16' );
                        $('c[r=H1]', sheet).attr( 's', '16' );
                        $('c[r=I1]', sheet).attr( 's', '16' );
                        $('c[r=J1]', sheet).attr( 's', '16' );
                    }
                }
            ],
            "ajax": {
                "url": servicesUrl,
                "data": function (d) {
                    d.action = "generateChipDumpingReport";
                    d.jur = $('#reportClients').val();
                    d.month = $('#reportDate').val();
                }
            }, 'iDisplayLength': 10, 'sPaginationType': "full_numbers"
        });

        $(document).on('click','#reportButton',function(){
            reportsTable.ajax.reload();
        });



        $(document).on('change', '.monthHelper', function() {
            var d = new Date($(this).val());
            createMonthHelper(d);
        });
        $(document).on('change', '.weekHelper', function() {
            createWeekHelper($(this).val());
        });

        $(".timeHelper").datetimepicker({
            changeMonth: true,
            changeYear:true,
            numberOfMonths:1,
            onChangeMonthYear:function(year, month, i){
                var day = i.selectedDay;
                $(this).datepicker('setDate', new Date(year, month-1, day));
            }
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



    });


</script>

<?php




if($start_time=strtotime($from)){
    $from = date("Y-m-d H:i", $start_time);
}else{
    $from = date("Y-m-d H:i", strtotime("now - 2 hours"));
}



?>

<table class="table table-bordered" >
    <tr>
        <td class="navbar-inner" ><h3 class="text-center"><?=$lang->getLang("Chip dumping")?></h3></td>
    </tr>
    <tr>
        <td>
            <div class="well">
                <div class="tabbable">
                    <ul class="nav nav-tabs">
                        <li <?=($active=='clients')? 'class="active"':''?>><a href="#clients" data-toggle="tab"><button class="btn btn-inverse"><?=$lang->getLang("Clients")?></button></a></li>
                        <li <?=($active=='cd')? 'class="active"':''?>> <a href="#cd" data-toggle="tab"><button class="btn btn-inverse"><?=$lang->getLang("Chip dumping")?></button></a></li>
                        <? if (check_access('poker_chip_dumping_reports')) { ?>
                        <li <?=($active=='mr')? 'class="active"':''?>> <a href="#mr" data-toggle="tab"><button class="btn btn-inverse"><?=$lang->getLang("Monthly reports")?></button></a></li>
                        <? } ?>
                    </ul>
                    <div class="tab-content">
                        <div class="tab-pane <?=($active=='clients')? 'active':''?>" id="clients">
                            <ul class="nav nav-tabs">
                                <li class="active"><a href="#allclients" data-toggle="tab"><?=$lang->getLang("All clients")?></a></li>
                                <li><a href="#manageclient" data-toggle="tab"><?=$lang->getLang("Manage clients")?></a></li>
                            </ul>
                            <div class="tab-content">
                                <div class="tab-pane active" id="allclients">
                                    <table class='table display table-hover table-striped table-bordered' id="clientsTable">
                                        <thead>
                                        <tr>
                                            <th class="unwrappable"><?= $lang->getLang("Id") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Name") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Type") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Registration") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Action") ?></th>


                                        </tr>
                                        </thead>
                                        <tbody>
                                        <tr>
                                            <td></td>
                                            <td></td>
                                            <td></td>
                                            <td></td>
                                        </tr>
                                        </tbody>
                                    </table>
                                </div>
                                <div class="tab-pane" id="manageclient">
                                    <input type="hidden" id="chipDumpingClientId" value="">
                                    <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Name")?></div>
                                        <br>
                                        <div class="controls">
                                            <div class="input-prepend">
                                                <span class="add-on"><i class="icon-user"></i></span>
                                                <input type='text'  name='clientName' id="clientName"  placeholder="<?=$lang->getLang("Name")?>" value='' required="required">
                                            </div>
                                        </div>
                                    </div>
                                    <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Type")?></div>
                                        <br />
                                        <div class="controls">
                                            <div class="input-prepend">
                                                <span class="add-on"><i class="icon-globe"></i></span>
                                                <select name="clientType" id="clientType">
                                                    <option  value="1"  ><?=$lang->getLang('Partner')?></option>
                                                    <option  value="2"  ><?=$lang->getLang('Club')?></option>
                                                    <option  value="3"  ><?=$lang->getLang('Nation')?></option>
                                                </select>
                                            </div>
                                        </div>
                                    </div>
                                    <div class="clearfix"></div>
                                    <div class="span4">
                                        <button class="btn btn-primary" id="btnSaveClients" ><?=$lang->getLang("Save")?></button>
                                    </div>
                                </div>
                            </div>
                        </div>
                        <div class="tab-pane <?=($active=='cd')? 'active':''?>" id="cd">
                            <ul class="nav nav-tabs">
                                <li class="active"><a href="#allchipdumping" data-toggle="tab"><?=$lang->getLang("All chip dumping")?></a></li>
                                <li><a href="#managechipdumping" data-toggle="tab"><?=$lang->getLang("Manage chip dumping")?></a></li>
                            </ul>
                            <div class="tab-content">
                                <div class="tab-pane active" id="allchipdumping">
                                    <div class="row">
                                        <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Jurisdiction")?></div>
                                            <br>
                                            <div class="controls">
                                                <select name="clubClients" id="clubClients" class="usernameSelect" style="width: 248px">
                                                    <option  value=""  >All</option>
                                                    <?php foreach ($users as $k=>$v) { if($v['clt_type']==3){ ?>
                                                        <option  value="<?=$v['clt_id']?>"  ><?=$v['clt_name']?></option>
                                                    <? }} ?>
                                                </select>
                                            </div>
                                            <div class="clearfix"></div>
                                        </div>
                                        <div class="span2"><div class='label label-inverse'><?=$lang->getLang("Type")?></div>
                                            <br />
                                            <div class="controls">
                                                <div class="input-prepend">
                                                    <span class="add-on"><i class="icon-globe"></i></span>
                                                    <select id="chipDumpingClientType">
                                                        <option  value=""  ><?=$lang->getLang('All')?></option>
                                                        <option  value="1"  ><?=$lang->getLang('Partner')?></option>
                                                        <option  value="2"  ><?=$lang->getLang('Club')?></option>
                                                        <option  value="3"  ><?=$lang->getLang('Nation')?></option>
                                                    </select>
                                                </div>
                                            </div>
                                        </div>
                                        <div class="span6">
                                        <table  class="table-bordered">
                                            <tbody>
                                            <tr>
                                                <td><div class='label label-inverse'><?=$lang->getLang("Choose month")?></div></td>
                                                <td><div class="controls">
                                                        <div class="input-prepend">
                                                            <span class="add-on"><i class="icon-calendar"></i></span>
                                                            <?=month_select_box()?>
                                                        </div>
                                                    </div></td>
                                            </tr>
                                            <tr>
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
                                                <td> <div class="controls">
                                                        <div class="input-prepend">
                                                            <span class="add-on"><i class="icon-calendar"></i></span>
                                                            <input type='text' id="chipDumpingStartDate" class="timeHelper dateStartHelper"  value="<?=$date_start.' 00:00'?>" >
                                                        </div>
                                                    </div></td>
                                            </tr>
                                            <tr>
                                                <td><div class='label label-inverse'><?=$lang->getLang("Until")?></div></td>
                                                <td><div class="controls">
                                                        <div class="input-prepend">
                                                            <span class="add-on"><i class="icon-calendar"></i></span>
                                                            <input type='text' id="chipDumpingEndDate" class="timeHelper dateEndHelper" value="<?=$date_end.' 23:59'?>">
                                                        </div>
                                                    </div></td>
                                            </tr>

                                            </tbody>
                                        </table>
                                        </div>
                                        <div class="clearfix"></div>
                                        <br />
                                        <div class="span2"><button type="button" id="getChipDumping" class="btn btn-info">View</button></div>
                                    </div>
                                    <br />
                                    <div id="chipDumpingTotals"></div>
                                    <br />
                                    <table class='table display table-hover table-striped table-bordered' id="chipDumpingTable">
                                        <thead>
                                        <tr>
                                            <th class="unwrappable"><?= $lang->getLang("Id") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Date") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Club") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Username") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Type") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Amount") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("From jurisdiction") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("From username") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("To jurisdiction") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("To username") ?></th>
                                            <th class="unwrappable"><?= $lang->getLang("Action") ?></th>
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
                                        </tr>
                                        </tbody>
                                    </table>
                                </div>
                                <div class="tab-pane" id="managechipdumping">
                                    <input type="hidden" id="chipDumpingId" value="">
                                    <table><tr><td class="span4" id="jurisdictionSelector">
                                                <div class="contentAdmin">
                                                    <div class="contentHeadAdmin"><?=$lang->getLang("Choose MAIN jurisdiction ")?></div>

                                                    <div class="alert hidden alert-danger" id="jurAlert">
                                                        <?=$lang->getLang("No jurisdiction found")?>
                                                    </div>

                                                    <div class="well">
                                                        <input type="text" id='jurisdictionTreeSearch' placeholder="<?=$lang->getLang("Search a jurisdiction")?>">

                                                        <div class="clearfix"></div>
                                                        <br/>
                                                        <div id="jurisdictionTree">
                                                            <ul>
                                                                <?  foreach($users as $key => $v){ if($v['clt_type']==3){
                                                                    ?>
                                                                    <li data-jstree='{ "type" : "nation" }' class="nation" id="<?=$v['clt_id']?>"><?=strtoupper($v['clt_name'])?></li><?php
                                                                } }?>
                                                            </ul>
                                                        </div>
                                                    </div>
                                                </div>
                                            </td><td>
                                                <form id="chipDumpingForm">
                                                    <div class="row">
                                                        <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Type")?></div>
                                                            <br>
                                                            <div class="controls">
                                                                <select name="type" id="type" >
                                                                    <option  value="give"  >Give</option>
                                                                    <option  value="receive"  >Receive</option>
                                                                </select>
                                                            </div>
                                                        </div>
                                                        <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Date")?></div>
                                                            <br>
                                                            <div class="controls">
                                                                <div class="input-prepend">
                                                                    <span class="add-on"><i class="icon-calendar"></i></span>
                                                                    <input type='text' name='date' id='date' class="dateStartPicker"  value="<?=$from?>" >
                                                                </div>
                                                            </div>
                                                        </div>
                                                        <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Amount")?></div>
                                                            <br />
                                                            <div class="controls">
                                                                <div class="input-prepend">
                                                                    <span class="add-on"><i class="icon-user"></i></span>
                                                                    <input type='number' style="width: 70px"  name='amount' id="amount"  placeholder="<?=$lang->getLang("Amount")?>" value='1' required="required">
                                                                </div>
                                                            </div>
                                                        </div>
                                                    </div>
                                                    <br/>
                                                    <div class="row">
                                                        <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Club")?> (B)</div>
                                                            <br>
                                                            <div class="controls">
                                                                <select name="club" id="club" class="usernameSelect" style="width: 248px">
                                                                    <?php foreach ($users as $k=>$v) { if($v['clt_type']!=3){ ?>
                                                                        <option  value="<?=$v['clt_id']?>"  ><?=$v['clt_name']?></option>
                                                                    <? } } ?>
                                                                </select>
                                                            </div>
                                                            <div class="clearfix"></div>
                                                        </div>


                                                        <div class="span4"><div class='label label-inverse'><?=$lang->getLang("From")?> (H)</div>
                                                            <br>
                                                            <div class="controls">
                                                                <select name="from" id="from" class="usernameSelect" style="width: 248px">
                                                                    <option  value=""  >None</option>
                                                                    <?php foreach ($users as $k=>$v) { if($v['clt_type']!=3){ ?>
                                                                        <option  value="<?=$v['clt_id']?>"  ><?=$v['clt_name']?></option>
                                                                    <? } } ?>
                                                                </select>
                                                            </div>
                                                        </div>
                                                        <div class="span4"><div class='label label-inverse'><?=$lang->getLang("To")?> (K)</div>
                                                            <br>
                                                            <div class="controls">
                                                                <select name="to" id="to" class="usernameSelect" style="width: 248px">
                                                                    <option  value="" >None</option>
                                                                    <?php foreach ($users as $k=>$v) { if($v['clt_type']!=3){ ?>
                                                                        <option  value="<?=$v['clt_id']?>"  ><?=$v['clt_name']?></option>
                                                                    <? } } ?>
                                                                </select>
                                                            </div>
                                                        </div>


                                                    </div>

                                                    <div class="row">
                                                        <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Username")?> (A)</div>
                                                            <br>
                                                            <div class="controls">
                                                                <div class="input-prepend">
                                                                    <span class="add-on"><i class="icon-user"></i></span>
                                                                    <input type='text'  name='username' id="username"  placeholder="<?=$lang->getLang("Username")?>" value='' required="required">
                                                                </div>
                                                            </div>
                                                        </div>
                                                        <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Username from")?> (G)</div>
                                                            <br>
                                                            <div class="controls">
                                                                <div class="input-prepend">
                                                                    <span class="add-on"><i class="icon-user"></i></span>
                                                                    <input type='text'  name='receivefrom' id="receivefrom"  placeholder="<?=$lang->getLang("Receive From")?>" value='' required="required">
                                                                </div>
                                                            </div>
                                                        </div>
                                                        <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Username To")?> (J)</div>
                                                            <br>
                                                            <div class="controls">
                                                                <div class="input-prepend">
                                                                    <span class="add-on"><i class="icon-user"></i></span>
                                                                    <input type='text'  name='giveto' id="giveto"  placeholder="<?=$lang->getLang("Give To")?>" value='' required="required">
                                                                </div>
                                                            </div>
                                                        </div>
                                                    </div>


                                                    <div class="clearfix"></div>
                                                    <div class="span4">
                                                        <button type="button" class="btn btn-primary" id="btnSaveChipDumping" ><?=$lang->getLang("Save")?></button>
                                                    </div>
                                                </form>
                                            </td></tr></table>
                                </div>
                            </div>
                        </div>
                        <div class="tab-pane <?=($active=='mr')? 'active':''?>" id="mr">
                            <div class="row">
                                <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Jurisdiction")?></div>
                                    <br>
                                    <div class="controls">
                                        <select name="reportClients" id="reportClients" class="usernameSelect" style="width: 248px">
                                            <option  value=""  >All</option>
                                            <?php foreach ($users as $k=>$v) {if($v['clt_type']==3){ ?>
                                                <option  value="<?=$v['clt_id']?>"  ><?=$v['clt_name']?></option>
                                            <? } } ?>
                                        </select>
                                    </div>
                                    <div class="clearfix"></div>
                                </div>
                                <br />
                                <div class="clearfix"></div>
                            <br />
                            <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Month")?></div>
                                <br>
                                <div class="controls">
                                    <div class="input-prepend">
                                        <span class="add-on"><i class="icon-calendar"></i></span>
                                        <input type='text' name='reportDate' id='reportDate'   value="<?=date('Y-m')?>" >
                                    </div>
                                </div>
                            </div>
                            <br />
                            </div>
                            <button type="button" class="btn btn-small btn-primary" id="reportButton">View</button>
                            <div clas="clearfix"></div><br />

                            <table class='table table-hover table-striped table-bordered' id="reportsTable">
                                <thead>
                                <tr>

                                    <th class="unwrappable" style="background-color: greenyellow;color:black"><?= $lang->getLang("Username") ?></th>
                                    <th class="unwrappable" style="background-color: greenyellow;color:black"><?= $lang->getLang("Club") ?></th>
                                    <th class="unwrappable" style="background-color: greenyellow;color:black"><?= $lang->getLang("Passa") ?></th>
                                    <th class="unwrappable" style="background-color: greenyellow;color:black"><?= $lang->getLang("Recive") ?></th>
                                    <th class="unwrappable" style="background-color: greenyellow;color:black"><?= $lang->getLang("Differenza") ?></th>
                                    <th class="unwrappable" style="background-color: greenyellow;color:black"><?= $lang->getLang("Data") ?></th>
                                    <th class="unwrappable" style="background-color: yellow;color:black"><?= $lang->getLang("Recive da") ?></th>
                                    <th class="unwrappable" style="background-color: yellow;color:black"><?= $lang->getLang("Club nazione") ?></th>
                                    <th class="unwrappable" style="background-color: yellow;color:black"><?= $lang->getLang("Passa a") ?></th>
                                    <th class="unwrappable" style="background-color: yellow;color:black"><?= $lang->getLang("Club nazione") ?></th>
                                </tr>
                                </thead>
                                <tfoot>
                                <tr>
                                    <th colspan="2" style="text-align:right"></th>
                                    <th></th>
                                    <th></th>
                                    <th></th>
                                </tr>
                                </tfoot>
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
                                </tr>
                                </tbody>
                            </table>
                        </div>
                    </div>
                </div>
            </div>

        </td>
    </tr>
</table>
<?




?>


