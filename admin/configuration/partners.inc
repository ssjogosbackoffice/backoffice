<?php check_access('manage_partners', true);
$allClubs=getAllClubsUnderJurisdiction($_SESSION['jurisdiction_id'],true);
$clubs="<select name='addClub' id='addClub'>";
$maxValue = (isset($_POST["maxValue"])) ? $_POST["maxValue"] : 10000 ;
$enableMaxValue=(isset($_POST["enableMaxValue"])) ? $_POST["enableMaxValue"] :2 ;
$status = (isset($_POST["status"])) ? $_POST["status"] : 2 ;
while($allClubs->hasNext()){
    $row=$allClubs->next();
    $clubs.="<option value='".$row['club_id']."'".($row['club_id']==$partner['jur_id']?'selected':'')." >".$row['club_name']."</option>";
}
$clubs.="</select>";

require_once 'WebRequest.class.inc';

?>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
<script src="/media/jquery.dataTables.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media//select2/select2.css" >
<script type="text/javascript" src="https://maps.googleapis.com/maps/api/js?libraries=places&sensor=false&&language=<?=$_SESSION['language']?>&region=<?=$_SESSION['language']?>"></script>

<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<script src="/media/bootstrap/js/bootstrap-multiselect.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/select2/select2.js" type="text/javascript"></script>
<script src="/media/auth_functions.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="/media/bootstrap/css/bootstrap-multiselect.css" >
<style>

    .entry {
        display:block;
    }

    #addNewClubContainer{
        display:none;
    }
    #addNewClubContainer input, #addNewClubContainer select{
        margin-bottom: auto;
        width: auto;
    }
    .multiselect-clear-filter{
        display: none;
    }
    a{
        text-decoration: none !important;
    }
    #loadingDiv {
        display: none;
        background-color: rgba(255, 255, 255, 0.47);
        font-size: 12px;
        z-index: 1010;
        position: fixed;
        top: 0;
        bottom: 0;
        left: 0;
        right: 0;
        margin: auto;
        padding-top: 200px;
    }
    .partnerDetailsTd{
        max-width: 150px;
        overflow-x: auto;
        display: block;
    }
    .formheading{
        background: rgb(51, 51, 51);
    }
    .pac-container {
        z-index: 1051 !important;
    }
</style>
<script type="text/javascript">
    $.fn.serializeObject = function()
    {
        var o = {};
        var a = this.serializeArray();
        $.each(a, function() {
            if (o[this.name] !== undefined) {
                if (!o[this.name].push) {
                    o[this.name] = [o[this.name]];
                }
                o[this.name].push(this.value || '');
            } else {
                o[this.name] = this.value || '';
            }
        });
        return o;
    };
    var startTime=null;
    var endTime=null;
    function disableHttps()
    {
        document.getElementById("http").checked=true;
        document.getElementById("https").disabled=true;
    }

    function enableHttps()
    {
        document.getElementById("https").disabled=false;
    }
    function getUserDetails(partner,type){
        type = typeof type !== 'undefined' ? type : 'display';
        $.post("/services/general_services.inc",{action:'grabPartnerDetails',partner:partner,type:type}, function(data){
            $("#userDetails").empty().append(data);
            $("body").animate({scrollTop: 300}, 800);

            if(type!='display'){
         /*       $('#currentClub').multiselect({
                    enableFiltering: true,
                    maxHeight: 300,
                    buttonWidth:'220px',
                    enableCaseInsensitiveFiltering:true
                });*/


                $('.currentPartnerProviders, #currentClub').select2({
                    width: '206px'
                }).on("change", function (e) {
                    if($(".currentPartnerProviders option[value='']:selected").length > 0){
                        $(this).siblings('.alert').show();
                    }
                    else{
                        $(this).siblings('.alert').hide();
                    }
                });
            }
        });
    }
    var _0xf14b = ["\x76\x61\x6C", "\x23\x72\x65\x66\x75\x6E\x64\x54\x6F\x6B\x65\x6E", "\x20\x20\x20"];
    function checkToken() {
        token = MD5($(_0xf14b[1])[_0xf14b[0]]());
        return MD5(token + '<?=$_SESSION['admin_username']?>');
    }
    $("body").on({
        ajaxStart: function() {
            $('#loadingDiv').show();
            startTime=new Date;
        },
        ajaxStop: function() {
            $('#loadingDiv').hide();
            endTime = new Date;
            totalTime = endTime - startTime;
            $("#duration").html('<?=$lang->getLang("Time taken to execute your request")?>:'+ totalTime/1000+" s");
        },
        ajaxError:function(){
            jAlert("An error has occurred.Please try again");
            $('#loadingDiv').hide();
            endTime = new Date;
            totalTime = endTime - startTime;
            $("#duration").html('<?=$lang->getLang("Time taken to execute your request")?>:'+ totalTime/1000+" s");
        }
    });
    $(document).on('click',".depositWithdraw", function() {
        var current=$(this);
        var club = $(this).parent().attr('data-club');
        var partnerName=$(this).parent().data('name');
        var type=current.data('type');
        $('#partnerToDepositWithdraw').val(club);
        $('#partnerTypeToDepositWithdraw').val(type);
        $("#myModalLabel").html(partnerName+" - "+type.substr(0,1).toUpperCase()+type.substr(1));
    });
    $(function(){

        $('.addPartner').on('click',function(){
            $('#field').empty().append(inputIp);
            $('#addPartnerWebsite').val("");
            $('#addPartnerSubdomain').val("");
            $('#addPartnerStatus').val(1);
            $('#addPartnerKey').val(MD5(Date.now().toString()).substring(0,8));

        });


        var inputIp='<div class="entry input-append">' +
            '<input class="form-control" name="ips[]" type="text" placeholder="Allowed Ips" required="required" />' +
            '<button class="btn btn-success btn-add" type="button">' +
            '<span class="glyphicon glyphicon-plus">+</span>' +
            '</button>' +
            '</div>';

        $(document).on('click', '.btn-add', function(e)
        {
            e.preventDefault();
            var controlForm = $('#field'),
                currentEntry = $(inputIp),
                newEntry = $(currentEntry.clone()).appendTo(controlForm);

            newEntry.find('input').val('');
            controlForm.find('.entry:last-child .btn-add')
                .removeClass('btn-add').addClass('btn-remove')
                .removeClass('btn-success').addClass('btn-danger')
                .html('<span class="glyphicon glyphicon-minus">-</span>');
        }).on('click', '.btn-remove', function(e)
        {
            $(this).parents('.entry:first').remove();

            e.preventDefault();
            return false;
        });

        var editInputIp='<div class="entry input-append">' +
            '<input class="form-control serializeIp addPartnerIp" name="ips[]" type="text" id="addPartnerIp" placeholder="Allowed Ips" required="required"/>' +
            '<button class="btn btn-success edt-btn-add" type="button">' +
            '<span class="glyphicon glyphicon-plus">+</span>' +
            '</button>' +
            '</div>';


        $(document).on('click', '.edt-btn-add', function(e)
        {
            e.preventDefault();
            var controlForm1 = $('#ip-field'),
                currentEntry = $(editInputIp),
                newEntry = $(currentEntry.clone()).appendTo(controlForm1 );

            newEntry.find('input').val('');
            controlForm1.find('.entry:last-child .edt-btn-add')
                .removeClass('edt-btn-add').addClass('btn-remove')
                .removeClass('btn-success').addClass('btn-danger')
                .html('<span class="glyphicon glyphicon-minus">-</span>');
        }).
        on('click', '.btn-remove', function(e)
        {
            $(this).parents('.entry:first').remove();

            e.preventDefault();
            return false;
        });

        $(document).on('click', '.clonePartner', function() {
            $('#field').empty();
            $('#addPartnerKey').val(MD5(Date.now().toString()).substring(0,8));

            var id = $(this).attr('data-id');
            //console.log(id);
            $.post("/services/general_services.inc",{
                    action:"getPartnerData",
                    id:id
                },
                function(data){
                    var addPartnerProviders=$('#addPartnerProviders').select2({
                        width: "206px"
                    });
                    var myArray = JSON.parse(data);



                    var websiteFormat = myArray[0];
                    var website = (websiteFormat != null) ? websiteFormat.substring(13, websiteFormat.length) : "";
                    $('#addPartnerWebsite').val(website);
                    var ipFormat = myArray[1];
                    //console.log(ipFormat);
                    var inputIp='<div class="entry input-append">' +
                        '<input class="form-control" name="ips[]" id="addPartnerIp" type="text" placeholder="Allowed Ips" required="required"/>' +
                        '<button class="btn btn-success btn-add" type="button">' +
                        '<span class="glyphicon glyphicon-plus">+</span>' +
                        '</button>' +
                        '</div>';
                    var ipList = ipFormat.split("|");
                    if (inputIp == null) {
                        $('#field').empty().append(inputIp);
                    } else {
                        var $xs = 0;
                        ipList.forEach(function(ip) {
                            var controlForm = $('#field'),
                                currentEntry = $(inputIp),
                                newEntry = $(currentEntry.clone()).appendTo(controlForm);
                                newEntry.find('input').val(ip);
                            if($xs > 0){
                                newEntry.find('input').val(ip);
                                controlForm.find('.entry:last-child .btn-add')
                                    .removeClass('btn-add').addClass('btn-remove')
                                    .removeClass('btn-success').addClass('btn-danger')
                                    .html('<span class="glyphicon glyphicon-minus">-</span>');
                            }
                            $xs++;
                        });


                    }

                    var subdomainFormat = myArray[2];
                    subdomainFormat = (subdomainFormat != null) ? subdomainFormat : "";
                    $('#addPartnerSubdomain').val(subdomainFormat);

                    var statusFormat = myArray[3];
                    $('#addPartnerStatus').val(statusFormat);

                    var noteFormat = myArray[7];
                    $('#addPartnerNotes').val(noteFormat);

                    var extidFormat = myArray[5];
                    $('#addPartnerEId').val(extidFormat);

                    var forcewithdrawFormat = myArray[8];
                    $('#addPartnerForceWithdraw').val(forcewithdrawFormat);

                    var paramsFormat = myArray[4];
                    $('#addPartnerParams').val(paramsFormat);

                    if(myArray[6] != null) {
                        var providersFormat = myArray[6].split(",");
                        $("#addPartnerProviders").select2("val", providersFormat);
                    } else {
                        $("#addPartnerProviders").select2("val", '');
                    }

                    var paramsIsSeamless = myArray[9];
                    $('#addPartnerIsSeamless').val(paramsIsSeamless);
                    var paramsClient = myArray[10];
                    $('#addPartnerClient').val(paramsClient);
                });
        });


        $('#depositWithdraw').on('submit',function(e) {
            e.preventDefault();
            club=$('#partnerToDepositWithdraw').val();
            amount=$('#refundAmount').val();
            note=$('#refundNote').val();
            type=$('#partnerTypeToDepositWithdraw').val();
            $.post("/services/general_services.inc",{
                action:"sendJurisdictionCredits",
                amount:amount,
                note:note,
                token:checkToken(),
                jurisdiction:club,
                type:type
            },
            function(data){
                if($.isNumeric(data)){
                    $('#'+club+'_credit>.cAmount').html(data);
                    jAlert('Successfully '+type+'!','Successfully');
                    $("#depositWithdraw")[0].reset();
                    partnersTable.ajax.reload();
                    $('#depositWithdrawDiv').modal('hide');
                }
                else {
                    jAlert(data,"Error");
                }
            });
        });


        var startTimeTextBox = $('#hoursStart');
        var endTimeTextBox = $('#hoursEnd');

        $.timepicker.timeRange(
            startTimeTextBox,
            endTimeTextBox,
            {
                minInterval: (1000*60*30), // 1hr
                timeFormat: 'HH:mm',
                start: {}, // start picker options
                end: {} // end picker options
            }
        );

        $('#addNewClub').on('change',function(){
            $('#addNewClubContainer').toggle('fade');
            $('#currentClubsContainer').toggle('fade');
        });
        $('#country, #district, #addClub').select2({
            width: "180px"
        });
        var addPartnerProviders=$('#addPartnerProviders').select2({
            width: "206px"
        });
        addPartnerProviders.on("change", function (e) {
            if($("#addPartnerProviders option[value='']:selected").length > 0){
                $(this).siblings('.alert').show();
            }
            else{
                $(this).siblings('.alert').hide();
            }
        });
        geocoder = new google.maps.Geocoder();
        $('#city').on('focus',function(){
            input = this,
                options = {
                    types: ['(cities)'],
                    componentRestrictions: {
                        country: $('#country option:selected').val()
                    }
                };
            city = new google.maps.places.Autocomplete(input, options);
            google.maps.event.addListener(city, 'place_changed', function() {
                place = city.getPlace();
                $('#city').val(place.address_components[0].long_name);
            })
        });
        $('#address, #city, #country').on('blur',function(){
            var country=$('#country option:selected').text();
            var address=$('#address').val();
            var city=$('#city').val();

            if( country!='' && address!='' && city!='') {
                currAddress = address + ',' + city + ',' + country;
                geocoder.geocode({'address': currAddress}, function (results, status) {
                    $('#longitudeS,#longitude').val(results[0].geometry.location.lng());
                    $('#latitudeS,#latitude').val(results[0].geometry.location.lat());
                });
            }
        });


        $('#enableMaxValue').on('click',function(){
            if($(this).is(':checked')){
                $('#maxValue').attr('disabled',false);

            }
            else{
                $('#maxValue').attr('disabled','disabled');
            }
        });
      /*  $('#addClub').multiselect({
            enableFiltering: true,
            maxHeight: 300,
            buttonWidth:'220px',
            enableCaseInsensitiveFiltering:true
        });*/
       partnersTable= $("#partnersTableContainer").DataTable({
            "processing": true,
            "serverSide": true,
            "ajax": {
                "url": "/services/general_services.inc",
                "data": function ( d ) {
                    d.action = "getPartners";
                    d.status = $('#status').val();
                    if ($("#enableMaxValue").is(':checked')) {
                        d.maxValue=$("#maxValue").val();
                    }
                }
            },
           'iDisplayLength': 200,'sPaginationType':"full_numbers",
           "lengthMenu": [ 10, 25, 50, 100, 200, 500],
        });
        //.on('click', '.deletePartner', function() {
        //    var partner = $(this).parent().attr('data-id');
        //    thistr=$(this).closest('tr');
        //    jConfirm("<?//=$lang->getLang("Are you sure you want to delete this partner?")?>//","",function (answer) {
        //        if(answer){
        //            $.post("/services/general_services.inc",{action:"deletePartner",partner:partner}, function(data){
        //                if(data==1){
        //                    $('#delResult').empty().append("<h4 class='alert alert-success fade in'><button type='button' class='close' data-dismiss='alert'>&times;</button><?//=$lang->getLang("You have successfully deleted the the partner!")?>//</h4><br />");
        //                    thistr.hide();
        //                }
        //                else
        //                {
        //                    $('#delResult').empty().append("<h4 class='alert alert-error fade in'><button type='button' class='close' data-dismiss='alert'>&times;</button><?//=$lang->getLang("An error occurred!Partner not deleted!")?>//</h4><br />");
        //                }
        //            });
        //        }
        //    });
        //})
        $(document).on('click','.showPartnerDetails',function(){
                $(".alert").alert('close');
                getUserDetails($(this).attr('data-id'));

            })

            .on('click', '.blockPartner', function() {
                var current=$(this);
                var partner = $(this).parent().attr('data-id');
                var type = $(this).parent().attr('data-status');
                var textToDisplay=(type==1?'disable':'enable');
                jConfirm("Are you sure you want to "+textToDisplay+" this partner?"," ",function (answer) {
                    if(answer){
                        $.post("/services/general_services.inc",{action:"blockPartners",partner:partner,type:type}, function(data){
                            if(data==1){
                                $('#delResult').empty().append("<h4 class='alert alert-success fade in'><button type='button' class='close' data-dismiss='alert'>&times;</button>You have successfully "+textToDisplay+"d  the partner!</h4><br />");
                                if(type==1){
                                    current.parent().attr('data-status',0);
                                    current.find('a').html('<i class="icon-ok"></i> <?=($lang->getLang("Enable"))?>');
                                    current.closest('tr').find('.partnerStatus').removeClass('text-success').addClass('text-error').html('<?=$lang->getLang('Disabled')?>');
                                    $('#userDetails').empty();
                                }else{
                                    current.parent().attr('data-status',1);
                                    current.find('a').html('<i class="icon-ban-circle"></i> <?=($lang->getLang("Disable"))?>');
                                    current.closest('tr').find('.partnerStatus').removeClass('text-error').addClass('text-success').html('<?=$lang->getLang('Enabled')?>');
                                    $('#userDetails').empty();
                                }
                            }
                            else
                            {
                                $('#delResult').empty().append("<h4 class='alert alert-error fade in'><button type='button' class='close' data-dismiss='alert'>&times;</button>An error occurred!Partner not "+textToDisplay+"ed!</h4><br />");
                            }
                        });
                    }
                });
            }).on('click', '.modifyPartner', function() {
            thisPartner=$(this).parent();
            var partner = $(this).parent().attr('data-id');
            $(".alert").alert('close');
            getUserDetails(partner,'modify');
        }).on('click', '.updateCurrentPartner', function() {
            $(".alert").alert('close');
            var partner = $('#currentPartnerId').text();
            var partnerName = $('#currentPartnerName').val();
            var partnerClub = $('#currentClub').val();
            var partnerWebsite = $('#currentPartnerWebsite').val();

            //var partnerIp = $('#currentPartnerIp').val();
            var partnerIp = decodeURIComponent($('.serializeIp').serialize());

            var partnerKey = $('#currentPartnerKey').val();
            var partnerSubdomain = $('#currentPartnerSubdomain').val();
            var partnerEId = $('#currentPartnerEId').val();
            var partnerForceWithdraw = $('#currentPartnerForceWithdraw').val();
            var partnerNote = $('#currentPartnerNotes').val();
            var partnerProviders=($('#currentPartnerProviders').val()!=null? $('#currentPartnerProviders').val().join() : '');
            var partnerParams=$('#currentPartnerParams').val();
            var partnerIsSeamless=$('#currentPartnerIsSeamless').val();
            var partnerClient=$('#currentPartnerClient').val();
            var flag = true;
            $('.serializeIp').each(function(i, obj) {
                if ($(this).val() == ''){
                    $('#delResult').empty().append("<h4 class='alert alert-error fade in'><button type='button' class='close' data-dismiss='alert'>&times;</button>Ip error!Partner not updated</h4><br />");
                    flag = false;
                }
            });
            if(flag) {
                $.post("/services/general_services.inc",{
                    action:"updatePartner",
                    partner:partner,
                    partnerName:partnerName,
                    partnerClub:partnerClub,
                    partnerWebsite:partnerWebsite,
                    partnerIp:partnerIp,
                    partnerKey:partnerKey,
                    partnerSubdomain:partnerSubdomain,
                    partnerEId:partnerEId,
                    partnerForceWithdraw:partnerForceWithdraw,
                    partnerNote:partnerNote,
                    partnerProviders:partnerProviders,
                    partnerParams:partnerParams,
                    partnerIsSeamless:partnerIsSeamless,
                    partnerClient:partnerClient,
                }, function(data){
                    if(data==1){
                        $('#delResult').empty().append("<h4 class='alert alert-success fade in'><button type='button' class='close' data-dismiss='alert'>&times;</button>You have successfully updated the partner!</h4><br />");
                        thisPartner.closest('tr').find('.partnerName').html(partnerName);
                    }
                    else{
                        $('#delResult').empty().append("<h4 class='alert alert-error fade in'><button type='button' class='close' data-dismiss='alert'>&times;</button>An error has occured!Partner not updated</h4><br />");
                    }
                });
            }



    });




        $('#modalForm').on('submit',function(e) {
            e.preventDefault();
            var values=$(this).serializeObject();
            $.ajax({
                url: "/services/general_services.inc",
                type: "POST",
                data:values
            }).done(function (data) {
                if(data==2){
                    jAlert('Successfully added partner!','Success');
                    $('#modalForm')[0].reset();
                    partnersTable.ajax.reload();
                    $('#myModal').modal('hide');
                }
                else{
                    jAlert(data,'Error');
                }
                }).always(function () {
                });



            // var partnerName = $('#addPartnerName').val();
            // var partnerClub = $('#addClub').val();
            // var partnerWebtype=$('input:radio[name=webtype]:checked').val();
            // var partnerHttp=$('input:radio[name=http]:checked').val();
            // var partnerWebsite = $('#addPartnerWebsite').val();
            // var partnerIp = $('#addPartnerIp').val();
            // var partnerKey = $('#addPartnerKey').val();
            // var partnerSubdomain = $('#addPartnerSubdomain').val();
            // var partnerEId = $('#addPartnerEId').val();
            // var partnerForceWithdraw = $('#addPartnerForceWithdraw').val();
            // var partnerStatus = $('#addPartnerStatus').val();
            // var partnerNote = $('#addPartnerNotes').val();

            //$.post("/services/general_services.inc",{
            //    action:"addPartner",
            //    partnerName:partnerName,
            //    partnerClub:partnerClub,
            //    partnerWebtype:partnerWebtype,
            //    partnerHttp:partnerHttp,
            //    partnerWebsite:partnerWebsite,
            //    partnerIp:partnerIp,
            //    partnerKey:partnerKey,
            //    partnerSubdomain:partnerSubdomain,
            //    partnerEId:partnerEId,
            //    partnerForceWithdraw:partnerForceWithdraw,
            //    partnerStatus:partnerStatus,
            //    partnerNote:partnerNote,
            //    form:fd
            //},function(data){
            //    if(data==2){
            //        jAlert('<?//=$lang->getLang("Partner successfully added")?>//','Successfully',function(answer){
            //            if(answer){
            //                window.location.reload();
            //            }
            //        });
            //    }
            //    else if(data==1){
            //        jAlert('<?//=$lang->getLang("Partner already exists")?>//','Try again');
            //    }
            //    else {
            //        jAlert(data);
            //    }
            //});
        });
    });

    function getSelectedValue(){
        $val = $('#district').val();
        $.post("/services/jurisdiction_services.inc", {"action":1,"jur_id":$val,"parent":"b"}, function(data){
            if(!data){
                $('#error_mess').show();
                $('#betting_perc').hide();
            }else{
                $('#betting_perc').show();
                $('#error_mess').hide();
            }
        });
        $.post("/services/jurisdiction_services.inc", {"action":3,"jur_id":$val,"parent":"b"}, function(data){
            if(data == -1){
                $('#error_mess_payments').show();
                $('#casino_perc').hide();
            }
            else{
                $('#casino_perc').show();
                var result = data.split("~");
                var details = result[1].split(";");
                for (var i = 0; i < details.length; i++){
                    var percDetail = details[i].split(":");
                    percDetail[1] = parseInt(percDetail[1]) - 15;
                    if(parseInt(percDetail[1]) < 0) percDetail[1] = 0;
                    $("#"+percDetail[0]).val(percDetail[1]);
                    $("#sel_"+percDetail[0]).val(percDetail[1]);
                }
                $('#error_mess_payments').hide();
            }
        });
        $.post("/services/jurisdiction_services.inc", {"action":6,"jur_id":$val,"parent":"b"}, function(data){
            if(!data){
                $('#live_error_mess').show();
                $('#betting_live_perc').hide();
            }else{
                $('#betting_live_perc').show();
                $('#live_error_mess').hide();
            }
        });
    }

</script>
<?php
/**
 *
 * @author Marian
 */


function get_jurisdiction_options($jurisdiction_class,$id=''){
    global $dbh;
    $options = '<option>No ' . $jurisdiction_class .' found</option>';

    require_once("JurisdictionsList.class.inc");
    $jur_list = JurisdictionsList::getInstance($dbh);
    $list = $jur_list->getChildJurisdictions($_SESSION['jurisdiction_id']);
    if(!empty($list[$jurisdiction_class])){
        $options = "";
        foreach($list[$jurisdiction_class] as $jur){
            $selected = (($id == $jur["id"])?("selected='selected'"):(""));
            $options .= '<option value="' . $jur["id"] . '"' . $selected . '>' . $jur["name"];
        }
    }

    return $options;
}


?>
<div id='loadingDiv' class="centered"><img src="<?=image_dir?>/ajax-loader.gif" /></div>



<form id="depositWithdraw">
    <div id="depositWithdrawDiv" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="depositWithdrawLabel" aria-hidden="true">
        <div class="modal-header">
            <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
            <h3 class="text-info text-center" id="myModalLabel"></h3>
            <input type="hidden" name="partnerToDepositWithdraw" id="partnerToDepositWithdraw" />
            <input type="hidden" name="partnerTypeToDepositWithdraw" id="partnerTypeToDepositWithdraw" />
        </div>
        <div class="modal-body">
            <p>
            <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Amount")?></div>
                <br>
                <div class="controls">
                    <div class="input-append">
                        <input title="Amount" type='number' name='refundAmount' id='refundAmount' value="0" min="0" step="0.01" >
                        <span class="add-on"><?=((isset($_SESSION['currency_html']) && $_SESSION['currency_html']!='') ? $_SESSION['currency_html'] : "&euro;" )?></span>
                    </div>
                </div>
            </div>
            <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Note")?></div>
                <br>
                <div class="controls">
                    <div class="input-prepend">
                        <textarea rows="3" name="refundNote" id="refundNote" placeholder="Your note" required="required" ></textarea>
                    </div>
                </div>
            </div>
            <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Your Password")?></div>
                <br>
                <div class="controls">
                    <div class="input-prepend">
                        <input title="Password" type='password' name='refundToken' id='refundToken' required="required" />
                    </div>
                </div>
            </div>
            </p>
        </div>
        <div class="modal-footer">
            <button class="btn" data-dismiss="modal" aria-hidden="true"><?=$lang->getLang("Cancel")?></button>
            <button class="btn btn-primary" id="sendModal" name="save"><?=$lang->getLang("Proceed")?></button>
        </div>
    </div>
</form>



<form id="modalForm">
    <div id="myModal" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
        <input type="hidden" name="action" value="addPartner" >
        <div class="modal-header">
            <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
            <h3 class="text-info text-center"><?=$lang->getLang("Add a partner")?> <span id="myModalLabel"></span></h3> <br />
        </div>
        <div class="modal-body">
            <p>
            <table class='table table-condensed '>
                <tr><td class='blackTd'><?=$lang->getLang("Name")?></td>
                    <td><input type="text" name='addPartnerName' id='addPartnerName' value='' required="required"></td></tr>
                <tr><td class='blackTd'><?=$lang->getLang("Club")?></td><td><div id="currentClubsContainer"><?=$clubs?></div>
                        <br /> <label class="checkbox"> <input type="checkbox" id="addNewClub" name="addNewClub" >Add a new club</label><br />
                        <div id="addNewClubContainer">
                            <table class="table table-condensed table-bordered" >
                                <tr>
                                    <td class="formheading" colspan="2"><?=$lang->getLang("Add New Club")?></td>
                                </tr>
                                <tr>
                                    <td><?=$lang->getLang("Club Name")?><br /><div class="labelnote"></div></td>
                                    <td><input type="text" class="lettersAndNumbers" name="club_name" value="<?php if ( areErrors() ) { echo replace_quotes($_POST['club_name']); } ?>"  /></td>
                                </tr>
                                <?php
                                if(empty($_POST['account_code'])){
                                    $_POST['account_code'] = strtoupper(get_free_jurcode());
                                }
                                ?>
                                <tr>
                                    <td><?=$lang->getLang("Account Code")?></td>
                                    <td><input type="text" name="account_code" value="<?=replace_quotes($_POST['account_code'])?>"   maxlength="6" placeholder="6 characters"/></td>
                                </tr>
                                <tr>
                                    <td><?=$lang->getLang("Access Type")?><div id="accessType"></div><br /></td>
                                    <td><select name="accessType">
                                            <option value="BACKOFFICE">Backoffice</option>
                                            <option value="WEBSITE">Website</option>
                                        </select>
                                    </td>
                                </tr>
                                <tr>
                                    <td><?=$lang->getLang("District")?></td>
                                    <td>
                                        <select name="district" id="district" onchange="getSelectedValue();">
                                            <?php
                                            if($_SESSION["jurisdiction_class"] == "district"){
                                                echo  get_jurisdiction_options('district', $id=$_SESSION['jurisdiction_id'], $hide_internet=true);
                                            }else{
                                                echo  get_jurisdiction_options('district', $_POST["district"], $hide_internet=true);
                                            }
                                            ?>
                                        </select>
                                        <input type="hidden" name="currency" value="<?=$currency?>" />
                                    </td>
                                </tr>
                                <tr>
                                    <td><?=$lang->getLang("Country")?></td>
                                    <td <? form_td_style('country'); ?>>
                                        <?countrySelect('country', "", $_POST['user_country'],false,'country'); ?>
                                        <?php echo err_field('user_country'); ?>
                                    </td>
                                </tr>


                                <tr>
                                    <td><?=$lang->getLang("City")?></td>
                                    <td><input type="text" name="city" id="city" value="" /></td>
                                </tr>

                                <tr>
                                    <td><?=$lang->getLang("Address Line 1")?></td>
                                    <td><input type="text" name="address1" id="address"  value="<?php if ( areErrors() ) { echo replace_quotes($_POST['address1']); } ?>"  /></td>
                                </tr>
                                <tr>
                                    <td><?=$lang->getLang("Address Line 2")?></td>
                                    <td><input type="text" name="address2" value="<?php if ( areErrors() ) { echo replace_quotes($_POST['address2']); } ?>" /></td>
                                </tr>
                                <tr>
                                    <td><?=$lang->getLang("Postcode or Zipcode")?></td>
                                    <td><input type="text" name="postc" value="<?php if ( areErrors() ) { echo replace_quotes($_POST['postc']); } ?>" /></td>
                                </tr>
                                <tr>
                                    <td><?=$lang->getLang("Longitude")?></td>
                                    <td>
                                        <input type="text" id="longitudeS" disabled="disabled" value="<?php if ( areErrors() ) { echo replace_quotes($_POST['langitude']); } ?>"  />
                                        <input type="hidden" name="longitude" id="longitude" value="<?php if ( areErrors() ) { echo replace_quotes($_POST['langitude']); } ?>" />
                                    </td>
                                </tr>
                                <tr>
                                    <td><?=$lang->getLang("Latitude")?></td>
                                    <td>
                                        <input type="text"  id="latitudeS" disabled="disabled" value="<?php if ( areErrors() ) { echo replace_quotes($_POST['latitude']); } ?>"  />
                                        <input type="hidden" name="latitude" id="latitude"  value="<?php if ( areErrors() ) { echo replace_quotes($_POST['latitude']); } ?>"  />
                                    </td>
                                </tr>
                                <tr>
                                    <td><?=$lang->getLang("Working Hours")?> </td>
                                    <td>
                                        <input type="text" name="hoursStart" id="hoursStart" style="width: 50px" value="<?php if ( areErrors() ) { echo replace_quotes($_POST['hoursStart']); } ?>"  > -
                                        <input type="text" name="hoursEnd" id="hoursEnd" style="width: 50px" value="<?php if ( areErrors() ) { echo replace_quotes($_POST['hoursEnd']); } ?>" >
                                    </td>
                                </tr>
                                <tr>
                                    <td><?=$lang->getLang("Contact Number")?> </td>
                                    <td><input type="text" name="phone" value="<?php if ( areErrors() ) { echo replace_quotes($_POST['phone']); } ?>"  /></td>
                                </tr>


                                <tr>
                                    <td valign="top"><?=$lang->getLang("Notes")?></td>
                                    <td ><textarea name="notes" rows="4" cols="4" wrap="hard"><?php if ( areErrors() ) { echo replace_quotes($_POST['notes']); } ?></textarea></td>
                                </tr>
                            </table>
                            <table id="casino_perc" width="90%">
                                <?php
                                include_once "funclib/percentual.inc.php";
                                showPercentage($_SESSION["jurisdiction_id"],$_SESSION["jurisdiction_class"],-1,"club",'navbar-inner');
                                ?>
                            </table>
                            <table id="error_mess_payments" width="90%" style="display:none;">
                                <tr>
                                    <td class="formheading" colspan='2'><?=$lang->getLang("Payments Percentage");?></td>
                                </tr>
                                <tr>
                                    <td class='content' colspan='2'><?=$lang->getLang("Before to insert percentage to this new % your up level has to set your percentage!","nation")?></td>
                                </tr>
                            </table>

                            <table id="betting_perc" width="90%">
                                <?php
                                showBettingPercentage($_SESSION["jurisdiction_id"],$_SESSION["jurisdiction_class"], "club",-1,-1,'navbar-inner');
                                ?>
                            </table>
                            <table id="error_mess" width="90%" style="display:none;">
                                <tr>
                                    <td class="formheading" colspan='2'><?=$lang->getLang("Betting Percentage");?></td>
                                </tr>
                                <tr>
                                    <td class='content' colspan='2'><?=$lang->getLang("Before to insert percentage to this new % your up level has to set your percentage!","club")?></td>
                                </tr>
                            </table>

                            <table id="betting_live_perc" width="90%">
                                <?php
                                showLiveBettingPercentage($_SESSION["jurisdiction_id"],$_SESSION["jurisdiction_class"], "club",-1, -1,'navbar-inner');
                                ?>
                            </table>
                            <table id="live_error_mess" width="90%" style="display:none;">
                                <tr>
                                    <td class="formheading" colspan='2'><?=$lang->getLang("Live Betting Percentage");?></td>
                                </tr>
                                <tr>
                                    <td class='content' colspan='2'><?=$lang->getLang("Before to insert percentage your up level has to set your percentage!")?></td>
                                </tr>
                            </table>
                        </div>


                    </td>
                </tr>
                <tr><td class="blackTd"><?=$lang->getLang("Type")?></td>
                    <td><input type="radio" id="webtype" name='webtype' selected checked value='web'  >Web <br>

                    </td></tr>
                <tr><td class="blackTd">Http/Https</td>
                    <td><input type="radio" id="http" name='http' selected="selected" checked value='http'>Http <br>
                        <input type="radio" id="https" name='http' value='https'>Https
                    </td>
                </tr>
                <tr><td class='blackTd'><?=$lang->getLang("Website")?></td>
                    <td><input type="text" name='addPartnerWebsite' id='addPartnerWebsite' value='' required="required"></td>
                </tr>
                <tr><td class='blackTd'><?=$lang->getLang("Allowed ip")?></td>
                    <td>
                        <div id="field">
                            <div class="entry input-append">
                                <input class="form-control" name="ips[]" id="addPartnerIp" type="text" placeholder="Allowed Ips" required="required"/>
                                <button class="btn btn-success btn-add" type="button">
                                    <span class="glyphicon glyphicon-plus">+</span>
                                </button>
                            </div>
                        </div>

                    <!--    <input type="text" name='addPartnerIp' id='addPartnerIp' value='' required="required">
                    --></td>
                </tr>
                <tr><td class='blackTd'><?=$lang->getLang("Key")?></td>
                    <td><input type="text" name='addPartnerKey' id='addPartnerKey' value='' required="required"></td>
                </tr>
                <tr><td class='blackTd'><?=$lang->getLang("Subdomain")?></td>
                    <td><input type="text" name='addPartnerSubdomain' id='addPartnerSubdomain' value='' ></td>
                </tr>
                <tr><td class='blackTd'><?=$lang->getLang("External Id")?></td>
                    <td><input type="text" name='addPartnerEId' id='addPartnerEId' value="" placeholder="Same as partner Id" ></td>
                </tr>
                <tr><td class='blackTd'><?=$lang->getLang("Force withdraw")?></td><td><select name='addPartnerForceWithdraw' id='addPartnerForceWithdraw'>
                            <option value='0' class='text-error' ><?=$lang->getLang("Disabled")?></option>
                            <option value='1' class="text-success"><?=$lang->getLang("Enabled")?></option>
                        </select></td>
                </tr>
                <tr><td class='blackTd'><?=$lang->getLang("Status")?></td><td>
                        <select name='addPartnerStatus' id='addPartnerStatus'>
                            <option value='0' class='text-error' ><?=$lang->getLang("Disabled")?></option>
                            <option value='1' class="text-success"><?=$lang->getLang("Enabled")?></option>
                        </select>

                    </td>
                </tr>
                <tr><td class='blackTd'><?=$lang->getLang("Providers enabled")?></td>
                    <td><select name='addPartnerProviders[]'  id='addPartnerProviders' multiple='multiple' >
                        <?
                        $providers=$dbh->doCachedQuery("Select * from providers");
                            while($providers->hasNext()){
                                $row=$providers->next();
                            ?>
                        <option value='<?=$row['pes_game_code']?>'  ><?=$row['pes_name']?></option>
                        <? } ?>
                        </select><br /><br />
                        <div class="alert alert-warning hidden" style="width: 80%"><?=$lang->getLang("Partner has a null value")?></div>
                    </td>
                </tr>
                <tr><td class='blackTd'><?=$lang->getLang("Params")?></td>
                    <td><input type="text" name='addPartnerParams' id='addPartnerParams' value="" placeholder="Params" ></td>
                </tr>

                <tr><td class='blackTd'><?=$lang->getLang("Is seamless")?></td><td>
                        <select name='addPartnerIsSeamless' id='addPartnerIsSeamless'>
                            <option value='0' class='text-error' ><?=$lang->getLang("False")?></option>
                            <option value='1' class="text-success"><?=$lang->getLang("True")?></option>
                        </select>

                    </td>
                </tr>

                <tr><td class='blackTd'><?=$lang->getLang("Client type")?></td>
                    <td><input type="text" name='addPartnerClient' id='addPartnerClient' value="generic" placeholder="<?=$lang->getLang("Client type")?>" ></td>
                </tr>
                <tr><td class='blackTd'><?=$lang->getLang("Notes")?></td><td><textarea name='addPartnerNotes' id='addPartnerNotes' ></textarea></td></tr>
            </table>
            </p>
        </div>
        <div class="modal-footer">
            <button class="btn" data-dismiss="modal" aria-hidden="true"><?=$lang->getLang("Cancel")?></button>
            <button class="btn btn-primary" id="sendModal" name="save"><?=$lang->getLang("Add")?></button>
        </div>
    </div>
</form>

<table class="table table-bordered" style="padding: 20px">
    <tr>
        <td class="navbar-inner" colspan="2"  ><h3 class="text-center"><?=$lang->getLang("Manage Partners")?></h3></td>
    </tr>
    <tr>
        <td colspan="2">
            <div class="well">
                <form action="/partners_management" method="post">
                    <?php printHiddenInput('doQuery', 'yes');?>
                    <div class="row">
                        <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Credit smaller than")?></div>
                            <br>
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"> <input type="checkbox" name="enableMaxValue" id="enableMaxValue" <?=($enableMaxValue==1? 'checked':'')?> value="1"></span>
                                    <input type="number" name="maxValue" id="maxValue" <?=($enableMaxValue!=1? 'disabled':'')?> value="<?=$maxValue?>">
                                </div>
                            </div>
                        </div>
                        <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Status")?></div>
                            <br>
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-random"></i></span>
                                    <select name='status' id="status">
                                        <option value='2' <?=($status=='2') ? "selected='selected'":""?> ><?=$lang->getLang("All")?></option>
                                        <option value='0' <?=($status=='1') ? "selected='selected'":""?>><?=$lang->getLang("Enabled")?></option>
                                        <option value='1' <?=($status=='0') ? "selected='selected'":""?>><?=$lang->getLang("Disabled")?></option>
                                    </select>
                                </div>
                            </div>
                        </div>
                    </div>
                    <div class="row">
                        <span class="span2"> <button class="btn btn-primary" id="SearchTicket"><?=$lang->getLang("Search")?></button></span>
                    </div><br />
                </form>
            </div>
        </td>
    </tr>
    <tr>
        <td>

            <div style="width: 100%">
                <?php
                $starttime = microtime(true);
                $endtime = microtime(true);
                $duration = $endtime - $starttime;
                $duration=number_format($duration, 4, ',', '')."s";
                $time="<br /><div class='tip fleft' id='duration'>".$lang->getLang("Time taken to execute your request").":".$duration."</div>";
                echo $time;
                ?>
                <a href="#myModal" class="addPartner fright btn btn-info btn-small" style="color:white" data-toggle="modal" ><?=$lang->getLang("Add a partner")?></a> <br /><br /><br />
                <table id="partnersTableContainer" class='display table table-striped table-hover table-bordered table-condensed' style="width: 90%">
                    <thead>
                    <tr>
                        <th><?=$lang->getLang("ID")?></th>
                        <th><?=$lang->getLang("Club")?><br /><?=$lang->getLang("Status")?></th>
                        <th><?=$lang->getLang("Available Credit")?></th>
                        <th><?=$lang->getLang("Partner")?><br /><?=$lang->getLang("Status")?></th>
                        <th><?=$lang->getLang("Details")?></th>
                        <th><?=$lang->getLang("Operation")?></th>
                    </tr>
                    </thead>
                    <tbody>
                    <td></td>
                    <td></td>
                    <td></td>
                    <td></td>
                    <td></td>

                    </tbody>
                </table>
            </div>
        </td>
        <td style="vertical-align:top;">
            <div id="delResult" ></div>
            <div id="userDetails"></div>
        </td>
    </tr>
</table>

