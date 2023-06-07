<?php
/**
 *
 * @author Marian
 */
check_access('virtual_reports',true);
global $lang;
require_once('virtual_functions.inc');
$date_start = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d", time());
$date_end = (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d");
$tid = $_POST['ticketid'] ;
$game=$_POST['games'];
$virtualHierarchy = getVirtualHierarchy();
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
    .displayCashier td {
        vertical-align: middle !important;
    }

</style>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/dataTablesExtra/TableTools-2.2.0/css/dataTables.tableTools.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="/media/bootstrap/css/bootstrap-multiselect.css" title="core_style">


<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap-multiselect.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/jquery.dataTables.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/jstree/jstree.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/jquery.alerts.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/highcharts.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/modules/exporting.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/dataTablesExtra/TableTools-2.2.0/js/dataTables.tableTools.js" type="text/javascript"></script>

<script src="<?= secure_host ?>/media/htmlTableExport/tableExport.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/htmlTableExport/jquery.base64.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/htmlTableExport/html2canvas.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/htmlTableExport/jspdf/libs/base64.js" type="text/javascript"></script>
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

    $(function() {

        var isActiveInfoNavTab = $('.info').hasClass('active');

        $('#userTypeContainer').hide();
        var to = false;

        $( "body" ).on( "click", ".cancel", function() {
            $(this).closest('td').find('.tableClass').popover('hide');
        }).on( "click", ".confirmdeposit", function() {
            thisButton=$(this);
            var tr=thisButton.closest('tr')
            customer=tr.find('input[name="customer"]').val();
            amount=$(this).prev().val();
            if(!$.isNumeric(amount)){
                jAlert('Please insert a correct amount');
                thisButton.siblings('.depositResultDiv').empty();
            }else{
                $.post("/virtual/virtual_functions.inc",{action:"5",type:'deposit',amount:amount,customer:customer}, function(data){
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
                $.post("/virtual/virtual_functions.inc",{action:"5",type:'withdraw',amount:amount,customer:customer}, function(data){
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

        $(".timeHelper").datepicker({
            changeMonth: true,
            changeYear:true,
            numberOfMonths:1
        });

        $('#selectAll').change(function(){
            $("#deselectAll").prop('checked', false);
            $('#jurisdictionTree').jstree('select_all');
        });
        $('#deselectAll').change(function(){
            $("#selectAll").prop('checked', false);
            $('#jurisdictionTree').jstree('deselect_all');
        });
        $('#jurisdictionTreeSearch').keyup(function () {
            if(this.value.length>2){
                $("#jurisdictionTree").jstree('search',$('#jurisdictionTreeSearch').val());
            }
            else{
                $("#jurisdictionTree").jstree("clear_search");
                $('#jurAlert').fadeOut();
            }
        });
        $("#jurisdictionTree")
            .jstree({
                "plugins" : [
                    "search",
                    "checkbox" ,
                    "types"
                ],
                "checkbox" : {
                    "three_state":true,
                    "whole_node":true
                },
                "search":{
                    "fuzzy":false,
                    "show_only_matches":true
                },
                "types" : {
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
            $("#deselectAll").prop('checked', false);
            var allselected=true;
            $.each( data.instance.get_node('#').children, function( index, value ){
                if(!data.instance.is_selected(value)){
                    allselected=false;
                }
                if(allselected==true){
                    $("#selectAll").prop('checked', true);
                }
                else{
                    $("#selectAll").prop('checked', false);
                }
            });

        }).bind("deselect_node.jstree",  function (e, data) {
            $("#deselectAll").prop('checked', false);
            return $("#selectAll").prop('checked', false);
        }).bind("loaded.jstree", function (event, data) {
            $('#selectAll').trigger('click');
            if($.trim(data.instance.element.context.innerText)=='-1'){
                $('#jurisdictionTree').empty().append("<div class='alert alert-error'><strong><?=$lang->getLang("No result found")?>!</strong></div>");
            }
        }).bind("select_all.jstree",  function (e, data) {
            $("#selectAll").prop('checked', true);
            return $("#selectAll").prop('checked', true);
        }).bind("open_all.jstree",  function (e, data) {
            var allselected=true;
            $.each( data.instance.get_node('#').children, function( index, value ){
                if(!data.instance.is_selected(value)){
                    allselected=false;
                }
                if(allselected==true){
                    $("#selectAll").prop('checked', true);
                }
                else{
                    $("#selectAll").prop('checked', false);
                }
            });
            if($('#jurisdictionTreeSearch').val()!=''){
                $("#jurisdictionTree").jstree('search',$('#jurisdictionTreeSearch').val());
            }
        }).bind("search.jstree", function (e, data) {
            if (data.nodes.length == 0){
                $('#jurAlert').fadeIn();
            }
            else{
                $('#jurAlert').fadeOut();
            }
        });

        $("#SearchTicket").click(function() {
            jurisdictions=$('#jurisdictionTree').jstree().get_selected();
            if(jurisdictions==''){
                jAlert('<?=$lang->getLang("Please select at least one jurisdiction!")?>');
            }
            else{
                ticket=$('input[name="ticketid"]').val();
                date_start=$('input[name="date_start"]').val();
                games=  $('#games').find('option:selected').val();
                selectAll=$('#selectAll').is(":checked");
                $.post("/virtual/virtual_functions.inc",{action:"1",
                    jurisdictions:JSON.stringify(jurisdictions),
                    section:'ticket',
                    ticket:ticket,
                    date_start:date_start,
                    games:games,
                    allSelected:selectAll
                }, function(data){
                    data=JSON.parse(data);
                    var pdfData="<h2 class='red'>Virtual Played Tickets <?=$lang->getLang("in")?> "+ date_start + "</h2><br />" +data[1];
                    $("#ticket_result").empty().append(data);
                    $(".displayTicket").dataTable({'sPaginationType':"full_numbers","sDom": 'T<"clear">lfrtip',"oTableTools": {
                        "aButtons": [
                            "copy",
                            "csv",
                            "xls",
                            {
                                "sExtends": "serverPdf",
                                "sPdfOrientation": "landscape",
                                "generatePdfFunction":function(){
                                    var url="services/printPdf.php";
                                    var mapForm = document.createElement("form");
                                    mapForm.target = "Map";
                                    mapForm.method = "POST"; // or "post" if appropriate
                                    mapForm.action = url;

                                    var mapInput = document.createElement("input");
                                    mapInput.type = "hidden";
                                    mapInput.name = "pdfData";
                                    mapInput.value = pdfData;
                                    mapForm.appendChild(mapInput);

                                    document.body.appendChild(mapForm);

                                    map = window.open("", "Map", "status=0,title=0,height=600,width=800,scrollbars=1");

                                    if (map) {
                                        mapForm.submit();
                                    } else {
                                        alert('You must allow popups to see the pdf report.');
                                    }

                                    return true;
                                }
                            },
                            {
                                "sExtends":"png",
                                "classOfTable":"displayTicket"
                            }

                        ]
                    }
                    });
                });
            }
        });


        $('#SearchTicketPaid').click(function() {
            $('#SearchTicketPaid').button('loading');
            jurisdictions=$('#jurisdictionTree').jstree().get_selected();
            if(jurisdictions==''){
                jAlert('<?=$lang->getLang("Please select at least one jurisdiction!")?>');
                $('#SearchTicketPaid').button('reset');
            }
            else{
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
                        jurisdictions:JSON.stringify(jurisdictions),
                        games:JSON.stringify(selectedGames),
                        allSelected:$('#selectAll').is(":checked"),
                        winmajor:$('#winmajor').is(":checked"),
                        ticketInfo:$('#ticketInfo').is(":checked"),
                        ticket:ticket
                    }, function(data){
                        try {
                            data=JSON.parse(data);
                            var pdfData="<h2 class='red'>Virtual Paid Tickets <?=$lang->getLang("in")?> "+ date_start + "</h2><br />" +data[1]+data[2];
                            $("#ticket_result_paid").empty().append(data);
                            //$('.displayTicketPaid').tableExport({type:'pdf',escape:'false'});
                            $(".displayTicketPaid").dataTable({'iDisplayLength': 100,'sPaginationType':"full_numbers","sDom": 'T<"clear">lfrtip',"oTableTools": {
                                "aButtons": [
                                    "xls",
                                    {
                                        "sExtends": "serverPdf",
                                        "sPdfOrientation": "landscape",
                                        "generatePdfFunction":function(){
                                            var url="services/printPdf.php";
                                            var mapForm = document.createElement("form");
                                            mapForm.target = "Map";
                                            mapForm.method = "POST"; // or "post" if appropriate
                                            mapForm.action = url;

                                            var mapInput = document.createElement("input");
                                            mapInput.type = "hidden";
                                            mapInput.name = "pdfData";
                                            mapInput.value = pdfData;
                                            mapForm.appendChild(mapInput);

                                            document.body.appendChild(mapForm);

                                            map = window.open("", "Map", "status=0,title=0,height=600,width=800,scrollbars=1");

                                            if (map) {
                                                mapForm.submit();
                                            } else {
                                                alert('You must allow popups to see the pdf report.');
                                            }

                                            return true;
                                        }
                                    },
                                    {
                                        "sExtends":"png",
                                        "classOfTable":"displayTicketPaid"
                                    }
                                ]
                            }
                            });
                        }
                        catch(err) {
                            jAlert('<?=$lang->getLang("An error has occured. Please try again later")?>');
                        }

                        $('#SearchTicketPaid').button('reset');
                    });
                }
            }
        });


        $('#SearchTicketNotPaid').click(function() {
            $('#SearchTicketNotPaid').button('loading');
            jurisdictions=$('#jurisdictionTree').jstree().get_selected();
            if(jurisdictions==''){
                jAlert('<?=$lang->getLang("Please select at least one jurisdiction!")?>');
                $('#SearchTicketNotPaid').button('reset');
            }
            else{
                selectedGames=$("#gamesNotPaid").val();
                date_start=$('input[name="date_start_not_paid"]').val();
                ticket=$('input[name="ticketidNotPaid"]').val();
                if(selectedGames=='' || typeof selectedGames=='undefined' || selectedGames==null){
                    jAlert('<?=$lang->getLang("Please select at least one game!")?>');
                    $('#SearchTicketNotPaid').button('reset');
                }else{
                    $.post("/virtual/virtual_functions.inc",{
                        action:"1",
                        date_start:date_start,
                        section:'ticketNotPaid',
                        jurisdictions:JSON.stringify(jurisdictions),
                        games:JSON.stringify(selectedGames),
                        allSelected:$('#selectAll').is(":checked"),
                        ticket:ticket
                    }, function(data){
                        try {
                            data=JSON.parse(data);
                            var pdfData="<h2 class='red'>Virtual Paid Tickets <?=$lang->getLang("in")?> "+ date_start + "</h2><br />" +data[1]+data[2];
                            $("#ticket_result_not_paid").empty().append(data);
                            //$('.displayTicketPaid').tableExport({type:'pdf',escape:'false'});
                            $(".displayTicketNotPaid").dataTable({'iDisplayLength': 100,'sPaginationType':"full_numbers","sDom": 'T<"clear">lfrtip',"oTableTools": {
                                "aButtons": [
                                    "xls",
                                    {
                                        "sExtends": "serverPdf",
                                        "sPdfOrientation": "landscape",
                                        "generatePdfFunction":function(){
                                            var url="services/printPdf.php";
                                            var mapForm = document.createElement("form");
                                            mapForm.target = "Map";
                                            mapForm.method = "POST"; // or "post" if appropriate
                                            mapForm.action = url;

                                            var mapInput = document.createElement("input");
                                            mapInput.type = "hidden";
                                            mapInput.name = "pdfData";
                                            mapInput.value = pdfData;
                                            mapForm.appendChild(mapInput);
                                            document.body.appendChild(mapForm);

                                            map = window.open("", "Map", "status=0,title=0,height=600,width=800,scrollbars=1");

                                            if (map) {
                                                mapForm.submit();
                                            } else {
                                                alert('You must allow popups to see the pdf report.');
                                            }

                                            return true;
                                        }
                                    },
                                    {
                                        "sExtends":"png",
                                        "classOfTable":"displayTicketNotPaid"
                                    }
                                ]
                            }
                            });
                        }
                        catch(err) {
                            jAlert('<?=$lang->getLang("An error has occured.Please try again later")?>');
                        }
                        $('#SearchTicketNotPaid').button('reset');
                    });
                }
            }
        });

        $('#SearchCashier').click(function(){
            $('#SearchCashier').button('loading');
            jurisdictions=$('#jurisdictionTree').jstree().get_selected();
            var date_start=$('input[name="date_start_cash"]').val();
            var date_end=$('input[name="date_end_cash"]').val();
            var users= $('#userType').find('option:selected').val();
            var group= $('#groupByCash').val();
            var diff = Math.floor(($('input[name="date_end_cash"]').datepicker('getDate').getTime() - $('input[name="date_start_cash"]').datepicker('getDate').getTime()) / 86400000);
            if(jurisdictions==''){
                $('#SearchCashier').button('reset');
                jAlert("<?=$lang->getLang('Please select at least one jurisdiction!')?>");
            }
            else{
                if(diff>93){
                    jAlert('Please choose an interval of maximum 3 months');
                    $('#SearchCashier').button('reset');
                }
                else{
                    $.post("/virtual/virtual_functions.inc",{
                        action:"1",
                        date_start:date_start,
                        date_end:date_end,
                        jurisdictions:JSON.stringify(jurisdictions),
                        group:group,
                        section:'cashier',
                        allSelected:$('#selectAll').is(":checked")
                    }, function(data){
                        data=JSON.parse(data);
                        var pdfData="<h2 class='red'>Cashier Closure <?=$lang->getLang("From")?> "+ date_start +" <?=$lang->getLang("Until")?> "+date_end+ "</h2><br />" +data[1]+data[2];
                        $('#SearchCashier').button('reset');
                        $("#cashier_result").empty().append(data);
                        $(".displayCashier").dataTable({'iDisplayLength': 100,'sPaginationType':"full_numbers","sDom": 'T<"clear">lfrtip',"oTableTools": {
                            "aButtons": [
                                "copy",
                                "csv",
                                "xls",
                                {
                                    "sExtends": "serverPdf",
                                    "generatePdfFunction":function(){

                                        var url="services/printPdf.php";
                                        var pdfForm = document.createElement("form");
                                        pdfForm.target = "pdf";
                                        pdfForm.method = "POST"; // or "post" if appropriate
                                        pdfForm.action = url;

                                        var pdfInput = document.createElement("input");
                                        pdfInput.type = "hidden";
                                        pdfInput.name = "pdfData";
                                        pdfInput.value = pdfData;
                                        pdfForm.appendChild(pdfInput);

                                        document.body.appendChild(pdfForm);

                                        pdf = window.open("", "pdf", "status=0,title=0,height=600,width=800,scrollbars=1");

                                        if (pdf) {
                                            pdfForm.submit();
                                        } else {
                                            alert('You must allow popups to see the pdf report.');
                                        }
                                        return true;

                                    }
                                },
                                {
                                    "sExtends":"png",
                                    "classOfTable":"displayCashier"
                                }
                            ]
                        }
                        });
                    });
                }
            }
        });

        $('#SearchJurisdictions').click(function() {

            $('.toHide').hide();
            var selected =  $("#jurisdictionTree").jstree("get_selected", true);
            var countClubs = 0;
            for (var i = 0; i < selected.length; i++) {
                //console.log(selected[i]);
                var data = selected[i].data.jstree.type;
                if (data == "club") {
                    countClubs++;
                }
            }
            if(countClubs >= 2){
                $('#jurisdictionTree').jstree('deselect_all');
            }
            $('#jurisdictionTree').jstree(true).settings.core.multiple = false;
            $('#jurisdictionTree').jstree(true).refresh();

            jurisdictions = $('#jurisdictionTree').jstree().get_selected();
            //console.log(jurisdictions);

            if(jurisdictions == ''){
                jAlert('<?=$lang->getLang("Please select at least one jurisdiction!")?>');
                if(countClubs > 2){
                    jAlert('<?=$lang->getLang("Only one jurisdiction can be selected!")?>');
                }
            }
            else {
                $.post("/virtual/virtual_functions.inc",{
                    action:"1",
                    jurisdictions:JSON.stringify(jurisdictions),
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
            }

        });

        $('#SearchFinancial').click(function() {
            $('#SearchFinancial').button('loading');
            jurisdictions=$('#jurisdictionTree').jstree().get_selected();
            date_start=$('input[name="date_start_fin"]').val();
            date_end=$('input[name="date_end_fin"]').val();
            diff = Math.floor(($('input[name="date_end_fin"]').datepicker('getDate').getTime() - $('input[name="date_start_fin"]').datepicker('getDate').getTime()) / 86400000);
            group= $('#groupByFin').find('option:selected').val();
            if(jurisdictions==''){
                jAlert("<?=$lang->getLang('Please select at least one jurisdiction!')?>");
                $('#SearchFinancial').button('reset');
            }
            else{
                if(diff>93){
                    jAlert('Please choose an interval of maximum 3 months');
                    $('#SearchFinancial').button('reset');
                }
                else{
                    $.post("/virtual/virtual_functions.inc",{
                        action:"1",
                        jurisdictions:JSON.stringify(jurisdictions),
                        date_start:date_start,
                        date_end:date_end,
                        section:'financial',
                        group:group,
                        allSelected:$('#selectAll').is(":checked"),
                        type:$('input[name=ur]:checked').val()
                    }, function(data){
                        $('#SearchFinancial').button('reset');
                        data=JSON.parse(data);
                        var pdfData="<h2 class='red'>Virtual Financial <?=$lang->getLang("From")?> "+ date_start +" <?=$lang->getLang("Until")?> "+date_end+ "</h2><br />" +data[1]+data[2];
                        $("#financial_result").empty().append(data);
                        $(".displayFinancial").dataTable({'iDisplayLength': 100,'sPaginationType':"full_numbers","sDom": 'T<"clear">lfrtip',"oTableTools": {
                            "aButtons": [
                                "csv",
                                "xls",
                                {
                                    "sExtends": "serverPdf",
                                    "generatePdfFunction":function(){
                                        var url="services/printPdf.php";
                                        var pdfForm = document.createElement("form");
                                        pdfForm.target = "pdf";
                                        pdfForm.method = "POST"; // or "post" if appropriate
                                        pdfForm.action = url;

                                        var pdfInput = document.createElement("input");
                                        pdfInput.type = "hidden";
                                        pdfInput.name = "pdfData";
                                        pdfInput.value = pdfData;
                                        pdfForm.appendChild(pdfInput);

                                        document.body.appendChild(pdfForm);

                                        pdf = window.open("", "pdf", "status=0,title=0,height=600,width=800,scrollbars=1");

                                        if (pdf) {
                                            pdfForm.submit();
                                        } else {
                                            alert('You must allow popups to see the pdf report.');
                                        }
                                        return true;
                                    }
                                },
                                {
                                    "sExtends":"png",
                                    "classOfTable":"displayFinancial"
                                }

                            ]
                        }
                        });
                        $(".totalFinancial").dataTable({
                            "sDom": 'T<"clear">lrtip',"oTableTools": {
                                "aButtons": [
                                    {
                                        "sExtends":    "csv",
                                        "fnCellRender": function ( sValue, iColumn, nTr, iDataIndex ) {
                                            // Append the text 'TableTools' to column 5
                                            if ( iColumn === 4 ) {
                                                return sValue.replace(/[\r\n]/g," ").replace( /<.*?>/g, "").replace(/special.*/g,"");
                                            }
                                            if ( iColumn === 1 ) {
                                                return sValue.replace(/[\r\n]/g," ").replace( /<.*?>/g, "").replace(/^.+Tax :/g,"");
                                            }
                                            return sValue.replace(/[\r\n]/g," ").replace( /<.*?>/g, "" );
                                        }
                                    }
                                ]
                            },
                            'bPaginate':false
                        });
                    });
                }
            }
        });

        $('#SearchOverall').click(function() {
            $('#SearchOverall').button('loading');
            jurisdictions=$('#jurisdictionTree').jstree().get_selected();
            date_start=$('input[name="date_start_ov"]').val();
            date_end=$('input[name="date_end_ov"]').val();
            users= $('#userType').find('option:selected').val();
            group= $('#groupBy').find('option:selected').val();
            diff = Math.floor(($('input[name="date_end_ov"]').datepicker('getDate').getTime() - $('input[name="date_start_ov"]').datepicker('getDate').getTime()) / 86400000);
            if(jurisdictions==''){
                $('#SearchOverall').button('reset');
                jAlert("<?=$lang->getLang('Please select at least one jurisdiction!')?>");
            }
            else{
                if(diff>93){
                    jAlert('Please choose an interval of maximum 3 months');
                    $('#SearchOverall').button('reset');
                }
                else{
                    $.post("/virtual/virtual_functions.inc",{
                        action:"1",
                        jurisdictions:JSON.stringify(jurisdictions),
                        date_start:date_start,
                        date_end:date_end,
                        users:users,
                        group:group,
                        section:'overall',
                        allSelected:$('#selectAll').is(":checked")
                    }, function(data){
                        data=JSON.parse(data);
                        var pdfData="<h2 class='red'>Virtual Overall <?=$lang->getLang("From")?> "+ date_start +" <?=$lang->getLang("Until")?> "+date_end+ "</h2><br />" +data[1]+data[2];
                        $('#SearchOverall').button('reset');
                        $("#overall_result").empty().append(data);
                        $(".displayOverall").dataTable({'iDisplayLength': 100,'sPaginationType':"full_numbers","sDom": 'T<"clear">lfrtip',"oTableTools": {
                            "aButtons": [
                                "copy",
                                "csv",
                                "xls",
                                {
                                    "sExtends": "serverPdf",
                                    "generatePdfFunction":function(){

                                        var url="services/printPdf.php";
                                        var pdfForm = document.createElement("form");
                                        pdfForm.target = "pdf";
                                        pdfForm.method = "POST"; // or "post" if appropriate
                                        pdfForm.action = url;

                                        var pdfInput = document.createElement("input");
                                        pdfInput.type = "hidden";
                                        pdfInput.name = "pdfData";
                                        pdfInput.value = pdfData;
                                        pdfForm.appendChild(pdfInput);

                                        document.body.appendChild(pdfForm);

                                        pdf = window.open("", "pdf", "status=0,title=0,height=600,width=800,scrollbars=1");

                                        if (pdf) {
                                            pdfForm.submit();
                                        } else {
                                            alert('You must allow popups to see the pdf report.');
                                        }
                                        return true;

                                    }
                                },
                                {
                                    "sExtends":"png",
                                    "classOfTable":"displayOverall"
                                }
                            ]
                        }
                        });
                    });
                }
            }
        });

        $('#SearchCreditBtn').click(function() {
            jurisdictions=$('#jurisdictionTree').jstree().get_selected();
            if(jurisdictions==''){
                jAlert('<?=$lang->getLang("Please select at least one jurisdiction!")?>');
            }
            else{
                $.post("/virtual/virtual_functions.inc",{action:"4",jurisdictions:JSON.stringify(jurisdictions),allSelected:$('#selectAll').is(":checked")}, function(data){
                    $("#transferCredit").empty().append(data);
                    $(".displayCredit").dataTable({'iDisplayLength': 100,'sPaginationType':"full_numbers","sDom": 'T<"clear">lfrtip'
                    });
                    $('.tableClass').popover().on('click', function (e) {
                        $('.tableClass').not(this).popover('hide');
                    });
                });
            }
        });

        $('#StatsBtn').click(function() {
            jurisdictions=$('#jurisdictionTree').jstree().get_selected();
            if(jurisdictions==''){
                jAlert('<?=$lang->getLang("Please select at least one jurisdiction!")?>');
            }
            else{
                $.post("/virtual/virtual_functions.inc",{
                        action:"1",
                        jurisdictions:JSON.stringify(jurisdictions),
                        section:'stats',
                        allSelected:$('#selectAll').is(":checked"),
                        period:$('#stsPeriod').val(),
                        datefrom:$('#date_start_sts').val(),
                        inactivity:$('#inactivity').val()
                    },
                    function(data){
                        $("#stats_result").empty().append(data);
                        $(".displayStats").dataTable({'sPaginationType':"full_numbers","sDom": 'T<"clear">lfrtip',"oTableTools": {
                            "aButtons": [
                                "copy",
                                "csv",
                                "xls",
                                "pdf"
                            ]
                        }
                        });
                    });
            }

        });


        $(document).on('click', '.showHiddenSelect', function() {
            $(".toHide").show();
            $('#jurisdictionTree').jstree(true).settings.core.multiple = true;
            $('#jurisdictionTree').jstree(true).refresh();
        });

        $('#stsPeriod').change(function() {
            if($(this).val()==0){
                $('#dateStartStsContainer').show();
            }
            else{
                $('#dateStartStsContainer').hide();
            }
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
        }).trigger('change');
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
        $('#jurisdictionTree').jstree('select_all');
        $('#gamesPaid').multiselect({
            maxHeight: 300,
            includeSelectAllOption: true,
            buttonWidth:'270px'
        });
        $('#gamesNotPaid').multiselect({
            maxHeight: 300,
            includeSelectAllOption: true,
            buttonWidth:'270px'
        });

    });

    function getConnectedUsers(container,club){
        $.post("/virtual/virtual_functions.inc",{action:"2",club:club}, function(data){
            $(container).parent().empty().append(data);
        });
    }

    $(document).on('click', '.changeCurr', function() {
        var iSymbol = $(this).closest('tr').find('.cSymbol');
        var iAmount = $(this).closest('tr').find('.cAmount');
        var curr = $(this).attr("data-curr");
        var mult=parseFloat($( "input[name='"+curr+"']").val());
        var symb=$(this).html();
        if( symb=='&#8364;' || symb=='€'){
            $(this).html(iSymbol.html());
            iSymbol.each(function() {
                $( this ).html( "&#8364; " );
            });
            iAmount.each(function() {
                if($(this).html().indexOf(',')!='-1'){
                    newValue=parseFloat($(this).html().replace('.','').replace(',','.'));
                }
                else{
                    newValue=parseFloat($(this).html().replace(',','.'));
                }
                $( this ).html((newValue/mult).toFixed(2));
            });
        }
        else{
            iSymbol.each(function() {
                $( this ).html(symb+" ");
            });
            iAmount.each(function() {
                if($(this).html().indexOf(',')!='-1'){
                    newValue=parseFloat($(this).html().replace('.','').replace(',','.'));
                }
                else{
                    newValue=parseFloat($(this).html().replace(',','.'));
                }
                $( this ).html((newValue*mult).toFixed(2));
            });
            $(this).html('&#8364;');
        }

    });


</script>
<div class="modal" id="myModal" style="display: none;margin-top:15%">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-body centered" >
                <?=$lang->getLang("Loading ...")?><img src="media/images/ajax-loader.gif">
            </div>
        </div>
    </div>
</div>
<div class="container" style="position: absolute;left:0;right: 0;width: 100%" >
    <table class="table table-bordered">
        <tr>
            <td class="navbar-inner" colspan="2">
                <h3 class="text-center"><?=$lang->getLang("Virtual Management")?></h3>
            </td>
        </tr>
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
                        <input type="text" id='jurisdictionTreeSearch' placeholder="<?=$lang->getLang("Search a jurisdiction")?>">
                        <div id="jurisdictionTree">
                            <ul><?=showChild($virtualHierarchy,true,true);?></ul>
                        </div>
                    </div>
                </div>
            </td>
            <td style='vertical-align: top'>
                <div class="well">
                    <div class="tabbable">
                        <ul class="nav nav-tabs">
                            <?if(check_access('virtual_management_info')) {?>
                                <li class=""><a href="#info" data-toggle="tab"><button class="btn btn-inverse" id="SearchJurisdictions"><?=$lang->getLang("Info")?></button></a></li>
                            <? } if(check_access('virtual_management_played')) {?>
                                <li class="showHiddenSelect active"><a href="#ticket" data-toggle="tab"><button class="btn btn-inverse showHiddenSelect" id="plydStatus"><?=$lang->getLang("Played Status")?></button></a></li>
                            <? } if(check_access('virtual_management_financial')) {?>
                                <li class="showHiddenSelect"><a href="#financial" data-toggle="tab"><button class="btn btn-inverse showHiddenSelect"><?=$lang->getLang("Financial")?></button></a></li>
                            <? } if(check_access('virtual_management_overall')) {?>
                                <li class="showHiddenSelect"><a href="#overall" data-toggle="tab"><button class="btn btn-inverse showHiddenSelect"><?=$lang->getLang("Overall")?></button></a></li>
                            <? } if(check_access('virtual_manage_credit')) {?>
                                <li class="showHiddenSelect"><a href="#transferCredit" data-toggle="tab"><button class="btn btn-inverse showHiddenSelect" id="SearchCreditBtn"><?=$lang->getLang("Manage credit")?></button></a></li>
                            <? } if(check_access('virtual_management_stats')) {?>
                                <li class="showHiddenSelect"><a href="#stats" data-toggle="tab"><button class="btn btn-inverse showHiddenSelect"><?=$lang->getLang("Stats")?></button></a></li>
                            <? } ?>
                            <li class="showHiddenSelect"><a href="#docs" data-toggle="tab"><button class="btn btn-inverse showHiddenSelect"><?=$lang->getLang("Documents")?></button></a></li>
                            <li class="showHiddenSelect"><a href="#cashierClosure" data-toggle="tab"><button class="btn btn-inverse showHiddenSelect"><?=$lang->getLang("Cashier Closure")?></button></a></li>
                        </ul>
                        <div class="tab-content">
                            <?if(check_access('virtual_management_info')) {?>
                                <div class="tab-pane" id="info">
                                    <p><?=$lang->getLang("Select a jurisdiction from the left menu and press on 'Info' button")?></p>
                                </div>
                            <? } if(check_access('virtual_management_played')) {?>
                                <div class="tab-pane active" id="ticket">
                                    <p>
                                    <div class="tabbable  tabs-left">
                                        <ul class="nav nav-tabs">
                                            <li class="active"><a href="#playedP" data-toggle="tab"><?=$lang->getLang("Played")?></a></li>
                                            <li><a href="#playedPaid" data-toggle="tab"><?=$lang->getLang("Paid")?></a></li>
                                            <li><a href="#playedNotPaid" data-toggle="tab"><?=$lang->getLang("Not Paid")?></a></li>
                                        </ul>
                                        <div class="tab-content">
                                            <div class="tab-pane active" id="playedP" >
                                                <div class='row'>
                                                    <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Day")?></div>
                                                        <br>
                                                        <div class="controls">
                                                            <div class="input-prepend">
                                                                <span class="add-on"><i class="icon-calendar"></i></span>
                                                                <input type='text' id="date_start"  name='date_start' class="timeHelper" placeholder="Date Start" value="<?=$date_start?>">
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
                                                    <span class="span2"> <button class="btn btn-primary" id="SearchTicket"><?=$lang->getLang("Search")?></button></span>
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
                                                    <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Ticket Info")?></div>
                                                        <br>
                                                        <label class="checkbox">
                                                            <input type="checkbox"  id="ticketInfo" > <?=$lang->getLang("Ticket Info")?>
                                                        </label>
                                                    </div>
                                                </div>
                                                <div class="row">
                                                    <span class="span2"> <button class="btn btn-primary" data-loading-text="<?=$lang->getLang("Please wait")?>..." id="SearchTicketPaid"><?=$lang->getLang("Search")?></button></span>
                                                </div><br /><br />
                                                <div id="ticket_result_paid"></div>
                                                <br />
                                            </div>

                                            <div class="tab-pane" id="playedNotPaid" style="min-height: 355px">
                                                <div class='row'>
                                                    <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Day")?></div>
                                                        <br>
                                                        <div class="controls">
                                                            <div class="input-prepend">
                                                                <span class="add-on"><i class="icon-calendar"></i></span>
                                                                <input type='text' id="date_start_not_paid"  name='date_start_not_paid' class="timeHelper" placeholder="Date Start" value="<?=$date_start?>">
                                                            </div>
                                                        </div>
                                                    </div>
                                                    <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Games")?></div>
                                                        <br>
                                                        <div class="controls">
                                                            <select name='gamesNotPaid[]' id='gamesNotPaid' multiple="multiple">
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
                                                                <input type='text'  name='ticketidNotPaid' id='ticketidNotPaid' placeholder="<?=$lang->getLang("Ticket Id")?> ( Optional )" maxlength="32" value="">
                                                            </div>
                                                        </div>
                                                    </div>
                                                </div>
                                                <div class="row">
                                                    <span class="span2"> <button class="btn btn-primary" data-loading-text="<?=$lang->getLang("Please wait")?>..." id="SearchTicketNotPaid"><?=$lang->getLang("Search")?></button></span>
                                                </div><br /><br />
                                                <div id="ticket_result_not_paid"></div>
                                                <br />
                                            </div>

                                        </div>
                                    </div>
                                    </p>
                                </div>

                            <? } if(check_access('virtual_management_financial')) {?>
                                <div class="tab-pane" id="financial">
                                    <p>
                                    <div class='row'>
                                        <table  class="searchTable table table-bordered">
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
                                                            <input type='text' name='date_start_fin' id='date_start_fin' class="dateStartHelper"  value="<?=$date_start?>" >
                                                        </div>
                                                    </div></td>
                                            </tr>
                                            <tr>
                                                <td><div class='label label-inverse'><?=$lang->getLang("Until")?></div></td>
                                                <td><div class="controls">
                                                        <div class="input-prepend">
                                                            <span class="add-on"><i class="icon-calendar"></i></span>
                                                            <input type='text' name='date_end_fin' id='date_end_fin' class="dateEndHelper" value="<?=$date_end?>">
                                                        </div>
                                                    </div></td>
                                            </tr>

                                            </tbody>
                                        </table>
                                        <div class="span4 fleft"><div class='label label-inverse'><?=$lang->getLang("Group by")?></div>
                                            <br>
                                            <div class="controls">
                                                <div class="input-prepend">
                                                    <span class="add-on"><i class="icon-align-justify"></i></span>
                                                    <select name="groupBy" id="groupByFin">
                                                        <!--   <option value="2"><?/*=$lang->getLang("Financial")*/?></option>-->
                                                        <option value="0"><?=$lang->getLang("Club")?></option>
                                                        <option value="1"><?=$lang->getLang("Cashier")?></option>
                                                        <option value="3"><?=$lang->getLang("Game")?></option>
                                                    </select>
                                                </div>
                                            </div>
                                        </div>
                                        <div class="span4 fleft"><div class='label label-inverse'><?=$lang->getLang("Financial contains")?></div>
                                            <br />
                                            <input type="radio" name="ur" class="yes" id="yes" checked value="0" />
                                            <input type="radio" name="ur" class="alternative" id="no"  value="1" />
                                            <div class="switch grey">
                                                <label class="labelYes" for="yes"><?=$lang->getLang("Rake")?></label>
                                                <label class="labelNo" for="no"><?=$lang->getLang("Cashier")?></label>
                                                <span></span>
                                            </div>
                                        </div>
                                    </div>

                                    <div class="row">
                                        <span class="span2"> <button class="btn btn-primary" id="SearchFinancial" data-loading-text="<?=$lang->getLang("Please wait")?>..."><?=$lang->getLang("Search")?></button></span>
                                    </div><br /><br />
                                    <div id="financial_result">
                                    </div>
                                    </p>
                                </div>
                            <? } if(check_access('virtual_management_overall')) {?>
                                <div class="tab-pane" id="overall">
                                    <p>
                                    <div class='row'>
                                        <table class="searchTable table table-bordered">
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
                                                            <input type='text' name='date_start_ov' id='date_start_ov' class="dateStartHelper" placeholder="Date Start" value="<?=$date_start?>" >
                                                        </div>
                                                    </div></td>
                                            </tr>
                                            <tr>
                                                <td><div class='label label-inverse'><?=$lang->getLang("Until")?></div></td>
                                                <td><div class="controls">
                                                        <div class="input-prepend">
                                                            <span class="add-on"><i class="icon-calendar"></i></span>
                                                            <input type='text' name='date_end_ov' id='date_end_ov' class="dateEndHelper"  placeholder="Date End" value="<?=$date_end?>">
                                                        </div>
                                                    </div></td>
                                            </tr>
                                            </tbody>
                                        </table>

                                        <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Group by")?></div>
                                            <br>
                                            <div class="controls">
                                                <div class="input-prepend">
                                                    <span class="add-on"><i class="icon-align-justify"></i></span>
                                                    <select name="groupBy" id="groupBy">
                                                        <option value="2"><?=$lang->getLang("Financial")?></option>
                                                        <option value="0"><?=$lang->getLang("Club")?></option>
                                                        <option value="1"><?=$lang->getLang("Cashier")?></option>
                                                        <option value="3"><?=$lang->getLang("Game")?></option>
                                                    </select>
                                                </div>
                                            </div>
                                        </div>

                                        <div class="span3" id="userTypeContainer"><div class='label label-inverse'><?=$lang->getLang("Users")?></div>
                                            <br>
                                            <div class="controls">
                                                <div class="input-prepend">
                                                    <span class="add-on"><i class="icon-align-justify"></i></span>
                                                    <select name="userType" id="userType">
                                                        <option value="0"><?=$lang->getLang("All users")?></option>
                                                        <option value="1"><?=$lang->getLang("Connected")?></option>
                                                        <option value="2"><?=$lang->getLang("Not connected")?></option>
                                                    </select>
                                                </div>
                                            </div>
                                        </div>
                                    </div>
                                    <div class="row">
                                        <span class="span2"> <button class="btn btn-primary" id="SearchOverall"  data-loading-text="<?=$lang->getLang("Please wait")?>..."><?=$lang->getLang("Search")?></button></span>
                                    </div><br /><br />
                                    <div id="overall_result">
                                    </div>
                                    </p>
                                </div>
                            <? } if(check_access('virtual_manage_credit')) {?>
                                <div class="tab-pane" id="transferCredit"></div>
                            <? } if(check_access('virtual_management_stats')) {?>
                                <div class="tab-pane" id="stats">
                                    <p>
                                    <div class='row'>
                                        <div class="span4"><div class='label label-inverse'><?=$lang->getLang("For")?></div>
                                            <br>
                                            <div class="controls">
                                                <div class="input-prepend">
                                                    <span class="add-on"><i class="icon-random"></i></span>
                                                    <select name='stsPeriod' id='stsPeriod' >
                                                        <option  value="1w"><?=$lang->getLang("One week")?></option>
                                                        <option  value="2w"><?=$lang->getLang("Two weeks")?></option>
                                                        <option  value="1m"><?=$lang->getLang("One month")?></option>
                                                        <option  value="2m"><?=$lang->getLang("Two months")?></option>
                                                        <option  value="3m"><?=$lang->getLang("Three months")?></option>
                                                        <option  value="6m"><?=$lang->getLang("Six months")?></option>
                                                        <option  value="0"><?=$lang->getLang("Other")?></option>
                                                    </select>
                                                </div>
                                            </div>
                                        </div>
                                        <div class="span4" id="dateStartStsContainer"><div class='label label-inverse'><?=$lang->getLang("Date from")?></div>
                                            <br>
                                            <div class="controls">
                                                <div class="input-prepend">
                                                    <span class="add-on"><i class="icon-align-justify"></i></span>
                                                    <input type='text' class="timeHelper" id="date_start_sts"  name='date_start_sts'   placeholder="Date Start" value="<?=$date_start?>">
                                                </div>
                                            </div>
                                        </div>
                                        <div class="span4" ><div class='label label-inverse'><?=$lang->getLang("Inactivity days")?></div>
                                            <br>
                                            <div class="controls">
                                                <div class="input-prepend">
                                                    <span class="add-on"><i class="icon-align-justify"></i></span>
                                                    <input type='number' id="inactivity" name='inactivity' step="1"  placeholder="<?=$lang->getLang("Inactivity days")?>"  value="30">
                                                </div>
                                            </div>
                                        </div>
                                    </div>

                                    <div class="row">
                                        <span class="span2"> <button class="btn btn-primary" id="StatsBtn"><?=$lang->getLang("Search")?></button></span>
                                    </div><br /><br />
                                    <div id="stats_result">

                                    </div>
                                    </p>
                                </div>
                            <? } ?>
                            <div class="tab-pane" id="docs">
                                <h3>Software [ <span style="color:blue"><?=(substr(DOMAIN,0,1)=='.'?substr(DOMAIN,1) : DOMAIN)?></span> ]</h3>
                                <table class="table display table-hover table-striped" style="width: 500px;margin: auto;margin-left:0">
                                    <thead> <tr><th>Nr</th><th><?=$lang->getLang("File")?></th><th><?=$lang->getLang("Download")?></th></tr></thead>
                                    <tbody>
                                    <tr><td>1</td><td><?=$lang->getLang("Virtual Installer")?></td>
                                        <td>
                                            <a  class="btn btn-medium" href="<?= "/documents/virtual_software/Virtual_Installer.exe" ?>" ><i class="icon-download"></i> <?=$lang->getLang("Download")?></a>
                                        </td>
                                    </tr>
                                    <tr><td>2</td><td><?=$lang->getLang("Virtual Totem Setup")?></td>
                                        <td>
                                            <a  class="btn btn-medium" href="<?= "/documents/virtual_software/Virtual_totem_Setup.exe" ?>"><i class="icon-download"></i> <?=$lang->getLang("Download")?></a>
                                        </td>
                                    </tr>
                                    </tbody>
                                </table><br /><br />

                                <h3>Technical documents</h3>
                                <table class="table display table-hover table-striped" style="width: 500px;margin: auto;margin-left:0">
                                    <thead> <tr><th>Nr</th><th><?=$lang->getLang("Document")?></th><th><?=$lang->getLang("Download")?></th></tr></thead>
                                    <tbody>
                                    <tr><td>1</td><td><?=$lang->getLang("Virtual Games - Users's Guide")?></td>
                                        <td><?$file=WEB_ROOT."/admin/documents/virtual_games_manual/".strtolower($_SESSION["language"])."/VirtualGamesManual.pdf"?>
                                            <a  class="btn btn-medium" href="<?=file_exists($file) ? "/documents/virtual_games_manual/".strtolower($_SESSION["language"])."/VirtualGamesManual.pdf" : "/documents/virtual_games_manual/en/VirtualGamesManual.pdf"?>" target="_blank"><i class="icon-zoom-in"></i><?=$lang->getLang("View")?></a>
                                        </td>
                                    </tr>
                                    <tr><td>2</td><td><?=$lang->getLang("Virtual Monitor - Users's Guide")?></td>
                                        <td><?$file=WEB_ROOT."/admin/documents/virtual_monitor/".strtolower($_SESSION["language"])."/VirtualMonitor.pdf"?>
                                            <a  class="btn btn-medium" href="<?=file_exists($file) ? "/documents/virtual_monitor/".strtolower($_SESSION["language"])."/VirtualMonitor.pdf" : "/documents/virtual_monitor/en/VirtualMonitor.pdf"?>" target="_blank"><i class="icon-zoom-in"></i><?=$lang->getLang("View")?></a>
                                        </td>
                                    </tr>
                                    <tr><td>3</td><td>Agency hardware specifications</td>
                                        <td><?$file=WEB_ROOT."/admin/documents/hardware_specifications/HardwareSpecifications_".strtolower($_SESSION["language"]).".pdf"?>
                                            <a  class="btn btn-medium" href="<?=file_exists($file) ? "/documents/hardware_specifications/HardwareSpecifications_".strtolower($_SESSION["language"]).".pdf" : "/documents/hardware_specifications/HardwareSpecifications_en.pdf"?>" target="_self"><i class="icon-zoom-in"></i><?=$lang->getLang("View")?></a>
                                        </td>
                                    </tr>
                                    </tbody>
                                </table><br /><br />
                                <h3>Game rules</h3>
                                <table class="table display table-hover table-striped" style="width: 500px;margin: auto;margin-left:0">
                                    <thead> <tr><th>Nr</th><th><?=$lang->getLang("Game")?></th><th><?=$lang->getLang("Download")?></th></tr></thead>
                                    <tbody> <tr><td>1</td><td><?=$lang->getLang("Ballomatic")?></td>
                                        <td>
                                            <?$file=WEB_ROOT."/admin/documents/game_rules/".strtolower($_SESSION["language"])."/ballomatic.pdf"?>
                                            <a  class="btn btn-medium" href="<?=file_exists($file) ? "/documents/game_rules/".strtolower($_SESSION["language"])."/ballomatic.pdf" : "documents/game_rules/en/ballomatic.pdf"?>" target="_blank"><i class="icon-zoom-in"></i><?=$lang->getLang("View")?></a>
                                        </td>
                                    </tr>
                                    <tr><td>2</td><td><?=$lang->getLang("Bet 6")?></td>
                                        <td>
                                            <?$file=WEB_ROOT."/admin/documents/game_rules/".strtolower($_SESSION["language"])."/bet6.pdf"?>
                                            <a  class="btn btn-medium" href="<?=file_exists($file) ? "/documents/game_rules/".strtolower($_SESSION["language"])."/bet6.pdf" : "documents/game_rules/en/bet6.pdf"?>" target="_blank"><i class="icon-zoom-in"></i><?=$lang->getLang("View")?></a>
                                        </td>
                                    </tr>
                                    <tr><td>3</td><td><?=$lang->getLang("Cash Roulette")?></td>
                                        <td>
                                            <?$file=WEB_ROOT."/admin/documents/game_rules/".strtolower($_SESSION["language"])."/cashroulette.pdf"?>
                                            <a  class="btn btn-medium" href="<?=file_exists($file) ? "/documents/game_rules/".strtolower($_SESSION["language"])."/cashroulette.pdf" : "documents/game_rules/en/cashroulette.pdf"?>" target="_blank"><i class="icon-zoom-in"></i><?=$lang->getLang("View")?></a>
                                        </td>
                                    </tr>
                                    <tr><td>4</td><td><?=$lang->getLang("Virtual Poker Odds")?></td>
                                        <td>
                                            <?$file=WEB_ROOT."/admin/documents/game_rules/".strtolower($_SESSION["language"])."/vpokerodds.pdf"?>
                                            <a  class="btn btn-medium" href="<?=file_exists($file) ? "/documents/game_rules/".strtolower($_SESSION["language"])."/vpokerodds.pdf" : "documents/game_rules/en/vpokerodds.pdf"?>" target="_blank"><i class="icon-zoom-in"></i><?=$lang->getLang("View")?></a>
                                        </td>
                                    </tr>
                                    <tr><td>5</td><td><?=$lang->getLang("Dog Racer")?></td>
                                        <td>
                                            <?$file=WEB_ROOT."/admin/documents/game_rules/".strtolower($_SESSION["language"])."/dogracer.pdf"?>
                                            <a  class="btn btn-medium" href="<?=file_exists($file) ? "/documents/game_rules/".strtolower($_SESSION["language"])."/dogracer.pdf" : "documents/game_rules/en/dogracer.pdf"?>" target="_blank"><i class="icon-zoom-in"></i><?=$lang->getLang("View")?></a>
                                        </td>
                                    </tr>
                                    <tr><td>6</td><td><?=$lang->getLang("Virtual Soccer")?></td>
                                        <td>
                                            <?$file=WEB_ROOT."/admin/documents/game_rules/".strtolower($_SESSION["language"])."/vsoccer.pdf"?>
                                            <a  class="btn btn-medium" href="<?=file_exists($file) ? "/documents/game_rules/".strtolower($_SESSION["language"])."/vsoccer.pdf" : "documents/game_rules/en/vsoccer.pdf"?>" target="_blank"><i class="icon-zoom-in"></i><?=$lang->getLang("View")?></a>
                                        </td>
                                    </tr>
                                    <tr><td>7</td><td><?=$lang->getLang("Virtula Soccer CD Multimatch")?></td>
                                        <td>
                                            <?$file=WEB_ROOT."/admin/documents/game_rules/".strtolower($_SESSION["language"])."/vsoccercdmultimatch.pdf"?>
                                            <a  class="btn btn-medium" href="<?=file_exists($file) ? "/documents/game_rules/".strtolower($_SESSION["language"])."/vsoccercdmultimatch.pdf" : "documents/game_rules/en/vsoccercdmultimatch.pdf"?>" target="_blank"><i class="icon-zoom-in"></i><?=$lang->getLang("View")?></a>
                                        </td>
                                    </tr>

                                    </tbody>
                                </table>
                            </div>
                            <div class="tab-pane" id="cashierClosure">
                                <p>
                                <div class='row'>
                                    <table  class="searchTable table table-bordered">
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
                                                        <input type='text' name='date_start_cash' id='date_start_cash' class="dateStartHelper"  value="<?=$date_start?>" >
                                                    </div>
                                                </div></td>
                                        </tr>
                                        <tr>
                                            <td><div class='label label-inverse'><?=$lang->getLang("Until")?></div></td>
                                            <td><div class="controls">
                                                    <div class="input-prepend">
                                                        <span class="add-on"><i class="icon-calendar"></i></span>
                                                        <input type='text' name='date_end_cash' id='date_end_cash' class="dateEndHelper" value="<?=$date_end?>">
                                                    </div>
                                                </div></td>
                                        </tr>

                                        </tbody>
                                    </table>
                                    <div class="span4 fleft"><div class='label label-inverse'><?=$lang->getLang("Group by")?></div>
                                        <br>
                                        <div class="controls">
                                            <div class="input-prepend">
                                                <span class="add-on"><i class="icon-align-justify"></i></span>
                                                <select name="groupByCash" id="groupByCash">
                                                    <option value="-1"><?=$lang->getLang("None")?></option>
                                                    <option value="0"><?=$lang->getLang("Club")?></option>
                                                </select>
                                            </div>
                                        </div>
                                    </div>
                                </div>

                                <div class="row">
                                    <span class="span2"> <button class="btn btn-primary" id="SearchCashier" data-loading-text="<?=$lang->getLang("Please wait")?>..."><?=$lang->getLang("Search")?></button></span>
                                </div><br /><br />
                                <div id="cashier_result">
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