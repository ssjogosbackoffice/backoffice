<?php check_access('casino_games_limit', true);
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


$active=(isset($_POST["active"])) ? $_POST["active"] : 'casino';
$periodType=(isset($_POST["periodType"])) ? $_POST["periodType"] : '0';
require_once "financial_casino/financial_functions.inc";
include_once ('jurisdiction_filters.php');
global $lang;
$jurisdiction_level = (isset($_POST['jurisdiction_level'])) ? $_POST['jurisdiction_level'] : getStartJurisdictionLevel();
$jur_groups_level = (isset($_POST['jur_level'])) ? $_POST['jur_level'] : getStartJurisdictionLevel();
$date_start = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d 00:00:00", time() - (60 * 60 * 24));
$date_end = (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d 23:59:59", time() - (60 * 60 * 24));
?>

<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/font-awesome.min.css"  />
<script src="/media/jquery.dataTables.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="/media/bootstrap/css/bootstrap-multiselect.css" >

<script type="text/javascript" src="https://maps.googleapis.com/maps/api/js?libraries=places&sensor=false&&language=<?=$_SESSION['language']?>&region=<?=$_SESSION['language']?>"></script>

<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<script src="/media/bootstrap/js/bootstrap-multiselect.js" type="text/javascript"></script>
<script src="/media/auth_functions.js" type="text/javascript"></script>

<style>

    .deleteUserGameLimit {
        height: 26px;
        color: #fff !important;
    }

    .error_message, error_message_pending {
        color: #ac0000;
        height: 20px;
        line-height: 20px;
        font-size: 14px;
        padding-left: 30px;
        padding-bottom: 10px;
    }
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
    .date_size {
        min-width: 90px;
    }
    .trans_actions {
        width: 160px;
    }

    #pendingTableContainer td, #partnersTableContainer_wrapper td {
        vertical-align: middle;
    }
    #exact_user, #exact_user_search {
        margin: 7px;
    }
</style>
<script type="text/javascript">
    $(document).ready(function () {

        (function($) {
            $.fn.inputFilter = function(inputFilter,min,max) {
                return this.on("input keydown keyup mousedown mouseup select contextmenu drop", function() {
                    if (inputFilter(this.value)) {
                        if(max){
                            if(this.value > max){
                                this.value = max;

                                $.jAlert({
                                    'title': 'Error',
                                    'content': "Maximum value is"+max,
                                    'theme': 'red',
                                    'btns': {'text':'Ok', 'theme': 'red'}
                                });
                            }
                            if(min.toString().length == this.selectionEnd){
                                if(this.value < min){
                                    this.value = min;
                                    $.jAlert({
                                    'title': 'Error',
                                    'content': "Minimum value is"+min,
                                    'theme': 'red',
                                    'btns': {'text':'Ok', 'theme': 'red'}
                                });
                                }
                            }
                        }
                        this.oldValue = this.value;
                        this.oldSelectionStart = this.selectionStart;
                        this.oldSelectionEnd = this.selectionEnd;
                    } else if (this.hasOwnProperty("oldValue")) {
                        this.value = this.oldValue;
                        this.setSelectionRange(this.oldSelectionStart, this.oldSelectionEnd);

                    }
                });
            };
        }(jQuery));

        //        Integer values (both positive and negative):
        //        /^-?\d*$/.test(value)
        //        Integer values (positive only):
        //        /^\d*$/.test(value)
        //        Integer values (positive and up to a particular limit):
        //        /^\d*$/.test(value) && (value === "" || parseInt(value) <= 500)
        //        Floating point values (allowing both . and , as decimal separator):
        //        /^-?\d*[.,]?\d*$/.test(value)
        //        Currency values (i.e. at most two decimal places):
        //        /^-?\d*[.,]?\d{0,2}$/.test(value)
        //        Currency values positive only(i.e. at most two decimal places):
        //        /^\d*[.,]?\d{0,2}$/.test(value)
        //        Hexadecimal values:
        //        /^[0-9a-f]*$/i.test(value)
        //        Alphanumeric no injectino:
        //        /^[0-9a-zA-Za-åa-ö-w-я-`/() ?&%:#=+]*$/i.test(value)




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
        function getUserDetails(user,username){
            $.post("/services/general_services.inc",{action:'grabGameLimitDetails',user:user,username:username}, function(data){
                $("#userDetails").empty().append(data);
                $("body").animate({scrollTop: 300}, 800);
                $('.gameLimitDetails #currentProvider').select2({
                    width: '206px'
                });
                $('.inputnumber').inputFilter(function (value) {
                    return /^\d*[.]?\d{0,2}$/.test(value);
                });
            });
        }
        function deleteUserGameLimit(limit_id){
            $.post("/services/general_services.inc",{action:'deleteUserGameLimit',limit_id:limit_id}, function(data){

            });
        }
        // var _0xf14b = ["\x76\x61\x6C", "\x23\x72\x65\x66\x75\x6E\x64\x54\x6F\x6B\x65\x6E", "\x20\x20\x20"];
        //function checkToken() {
        //    token = MD5($(_0xf14b[1])[_0xf14b[0]]());
        //    return MD5(token + '<?//=$_SESSION['admin_username']?>//');
        //}
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
            //ajaxError:function(){
            //    jAlert("An error has occurred.Please try again");
            //    $('#loadingDiv').hide();
            //    endTime = new Date;
            //    totalTime = endTime - startTime;
            //    $("#duration").html('<?//=$lang->getLang("Time taken to execute your request")?>//:'+ totalTime/1000+" s");
            //}
        });

        $(function(){

            $('.addPartner').on('click',function(){
                $('#field').empty().append(inputIp);
                $('#addPartnerWebsite').val("");
                $('#addPartnerSubdomain').val("");
                $('#addPartnerStatus').val(1);
                $('#addPartnerKey').val(MD5(Date.now().toString()).substring(0,8));

            });


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

            $('#trans_status, #trans_provider').select2({
                minimumResultsForSearch: -1,
                width: "200px",
            });
            $('#trans_provider').select2({
                width: "200px",
            });

            $( "#date_start" ).datetimepicker({
                changeMonth: true,
                changeYear: true,
                numberOfMonths:1,
                // format: 'YYYY-MM-DD H:mm:ss',
                timeFormat: 'H:mm:ss',
                onSelect: function( selectedDate ) {
                    $( "#date_end" ).datetimepicker( "option", "minDate", selectedDate );
                    $( "#date_end" ).datetimepicker( "option", "maxDate", "+1M");
                }
            });
            $( "#date_end" ).datetimepicker({
                changeMonth: true,
                changeYear: true,
                numberOfMonths:1,
                // format: 'DD-MM-YYYY H:mm:ss',
                timeFormat: 'H:mm:ss',
                onSelect: function( selectedDate ) {
                    $( "#date_start" ).datetimepicker( "option", "maxDate", selectedDate );
                }
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



            limitsTable= $("#partnersTableContainer").DataTable({
                "processing": true,
                "serverSide": true,
                "deferLoading": false,
                "searching": false,
                "ajax": {
                    "url": "/services/general_services.inc",
                    "data": function ( d ) {
                        d.action = "getGamesLimit";
                        d.searchBy = $('#searchBy').val();
                        d.username = $('#searchUser').val();
                        d.jur_level = $('#jurisdiction_level').val();
                        d.jur_select = $('#jurisdiction_select select[name="jurisdiction"]').val();

                        // if ($("#enableMaxValue").is(':checked')) {
                        //     d.maxValue=$("#maxValue").val();
                        // }
                    }
                },
                "columnDefs": [
                    { className: "date_size", "targets": [ 5 ] },
                    { className: "un", "targets": [ 4 ] },
                ],
                'iDisplayLength': 200,'sPaginationType':"full_numbers",
                "lengthMenu": [ 10, 25, 50, 100, 200, 500],
            });


            pendingTable= $("#pendingTableContainer").DataTable({
                "processing": true,
                "serverSide": true,
                "deferLoading": false,
                "searching": false,
                "ajax": {
                    "url": "/services/general_services.inc",
                    "data": function ( d ) {
                        d.action = "getPendingTrans";
                        d.status = $('#trans_status').val();
                        d.username = $('#trans_user').val();
                        d.provider = $('#trans_provider').val();
                        d.from = $('#date_start').val();
                        d.until = $('#date_end').val();

                        // if ($("#enableMaxValue").is(':checked')) {
                        //     d.maxValue=$("#maxValue").val();
                        // }
                    }
                },
                "columnDefs": [
                    { className: "date_size", "targets": [ 5 ] },
                    { className: "un", "targets": [ 4 ] },
                ],
                'iDisplayLength': 200,'sPaginationType':"full_numbers",
                "lengthMenu": [ 10, 25, 50, 100, 200, 500],
            });

            $(document).on('click','.showPartnerDetails',function(){
                $(".alert").alert('close');
                getUserDetails($(this).attr('data-id'));

            }).on('click', '.modifyGameLimit', function() {
                thisGameLimit=$(this);
                var user = $(this).attr('data-id');
                var username = $(this).closest('tr').find('.un').text();
                $(".alert").alert('close');
                getUserDetails(user,username);
            }).on('click', '.deleteUserGameLimit', function() {
                thisGameLimit=$(this);
                var limit_id = $(this).attr('data-id');
                $(this).closest('tr').remove();
                deleteUserGameLimit(limit_id);
            }).on('click', '.updateCurrentUserGameLimit', function() {
                $(".alert").alert('close');
                var userGameLimit = $('#gamelimit_id').val();
                var currentDepositLimit = $('#currentDepositLimit').val();
                var currentWithdrawLimit = $('#currentWithdrawLimit').val();
                var currentBetLimit = $('#currentBetLimit').val();
                var currentWinLimit = $('#currentWinLimit').val();
                var currentProvider = $('#currentProvider').val();
                var currentLock = $('#currentLock').val();


                $.post("/services/general_services.inc",{
                    action:"updateUserGameLimit",
                    type:1,
                    userGameLimit:userGameLimit,
                    currentDepositLimit:currentDepositLimit,
                    currentWithdrawLimit:currentWithdrawLimit,
                    currentBetLimit:currentBetLimit,
                    currentWinLimit:currentWinLimit,
                    currentProvider:currentProvider,
                    currentLock:currentLock,

                }, function(data){
                    if(data==1){
                        $('#delResult').empty().append("<h4 class='alert alert-success fade in'><button type='button' class='close' data-dismiss='alert'>&times;</button>You have successfully updated the game limit!</h4><br />");
                        // thisGameLimit.closest('tr').find('.partnerName').html(partnerName);
                        limitsTable.ajax.reload();
                    }
                    else{
                        $('#delResult').empty().append("<h4 class='alert alert-error fade in'><button type='button' class='close' data-dismiss='alert'>&times;</button>An error has occured!User game limit not updated</h4><br />");
                    }
                });

            });
            $('#modalFormAddLimit').on('submit',function(e) {
                e.preventDefault();
                var values=$(this).serializeObject();

                if($('#jurisdiction_select_1').val() == '' && $('#searchByadd').val() == '2') {
                    jAlert('No jurisdiction selected','Error');
                    return false;
                }
                if($('#searchUseradd').val() == '' && $('#searchByadd').val() == '1') {
                    jAlert('No user selected','Error');
                    return false;
                }
                $.ajax({
                    url: "/services/general_services.inc",
                    type: "POST",
                    data:values
                }).done(function (data) {
                    if(data==2){
                        jAlert('Successfully added game limit !','Success');
                        $('#modalFormAddLimit')[0].reset();

                        $('#myModal').modal('hide');
                    }
                    else{
                        jAlert('Duplicate entry ','Error');
                    }
                }).always(function () {
                });
            });
            $(document).off('click','.do_trans');
            $(document).on('click','.do_trans',function(e) {
                e.preventDefault();
                var btn_container = $(this).closest('div');
                $.ajax({
                    url: "/services/general_services.inc",
                    type: "POST",
                    data: {
                        action:"pendingTransaction",
                        uid:$(this).data('uid'),
                        tr:$(this).data('tr'),
                        do:$(this).data('do'),
                    }
                }).done(function (data) {
                    if(data == 0) {
                        btn_container.remove();
                    } else if(data == -168) {
                        jAlert('<?=$lang->getLang("Transaction already closed")?>','Warrning');
                    } else if(data == -139) {
                        jAlert('<?=$lang->getLang("Invalid admin user id")?>','Warrning');
                    } else if(data == -6) {
                        jAlert('<?=$lang->getLang("Invalid transaction")?>','Warrning');
                    }
                }).always(function () {
                });
            });
            $('#modalFormUpdateGameLimit').on('submit',function(e) {
                e.preventDefault();
                var values=$(this).serializeObject();

                $.ajax({
                    url: "/services/general_services.inc",
                    type: "POST",
                    data:values
                }).done(function (data) {
                    if(data==2){
                        jAlert('Successfully updateed game limit !','Success');
                        $('#modalFormUpdateGameLimit')[0].reset();
                        $('#updateGameLimit').modal('hide');
                        limitsTable.ajax.reload();
                    }
                    else{
                        jAlert(data,'Error');
                    }
                }).always(function () {
                });
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


        $(function() {
            $('#SearchTicket').off('click');
            $('#SearchTicket').on('click', function () {
                var searchby = $('#searchBy').val();
                if(searchby == 1) {
                    var searchValue = $('#searchUser').val();
                    if(searchValue == '') {
                        $('.error_message').html('Please select a username !');
                        return false;
                    } else {
                        $('.error_message').html();
                    }
                    $('#update_jur_type').val('');
                    $('#update_jur_id').val('');
                    $('a.updateGameLimit').hide();
                    $('a.deleteGameLimit').hide();
                } else if(searchby == 2) {
                    var searchValue = $('#jurisdiction_select select').val();
                    var jur_name = $('#jurisdiction_select select option[value="'+searchValue+'"]').text();
                    var jur_type = $('#jurisdiction_level').val();
                    if(searchValue == '') {
                        $('.error_message').html('Please select juristriction !');
                        $('#update_jur_type').val('');
                        $('#update_jur_id').val('');
                        return false;
                    } else {
                        $('a.updateGameLimit').show();
                        $('a.deleteGameLimit').show();
                        $('#update_jur_type').val(jur_type);
                        $('#update_jur_id').val(searchValue);
                        $('#jur_title_update').html(jur_type+' - '+jur_name);
                        $('.error_message').html();
                    }
                }

                limitsTable.ajax.reload();
                return false;
            });
            $('#searchPending').off('click');
            $('#searchPending').on('click', function () {
                pendingTable.ajax.reload();
                return false;
            });

            $('#searchUser').change(function () {
                var searchValue = $('#searchUser').val();
                if(searchValue != '') {
                    $('.error_message').html('');
                }
            });

            $('.deleteGameLimit').on('click', function () {
                var jur_type = $('#jurisdiction_level').val();
                var jur_id = $('#jurisdiction_select select').val();


                $.post("/services/general_services.inc",{action:'deleteGameLimit',jur_type:jur_type,jur_id:jur_id}, function(data) {

                });
            });
            $('#jurisdiction_select select').change(function () {
                var searchValue = $('#jurisdiction_select select').val();
                if(searchValue != '') {
                    $('.error_message').html('');
                }
            });


            var servicesUrl="/services/general_services.inc";

            $("#searchUser").select2({
                placeholder: 'Type to search',
                ajax: {
                    url: servicesUrl,
                    dataType: 'json',
                    delay: 250,
                    data: function (params,page) {
                        return {
                            page: page,
                            start:page*10,
                            value: params ,
                            action:'getUsersId' ,
                            exact:($('#exact_user_search').is(':checked')?1:0),
                            type:1,
                            exactResult:1
                        };
                    },
                    processResults: function (data, params) {
                        params.page = params.page || 1;
                        return {
                            results: $.map(data, function (item) {
                                return {
                                    text: item.username,
                                    id: item.id
                                }
                            }),
                            pagination: {
                                more: (params.page * 30) < data.recordsTotal
                            }
                        };
                    },
                    cache: true
                },
                escapeMarkup: function (markup) { return markup; }, // let our custom formatter work
                minimumInputLength: 3
            });

            $("#trans_user").select2({
                placeholder: 'Type to search',
                ajax: {
                    url: servicesUrl,
                    dataType: 'json',
                    delay: 250,
                    data: function (params,page) {
                        return {
                            page: page,
                            start:page*10,
                            value: params ,
                            action:'getUsersId' ,
                            exact:($('#exact_user').is(':checked')?1:0),
                            type:1,
                            exactResult:1
                        };
                    },
                    processResults: function (data, params) {
                        params.page = params.page || 1;
                        return {
                            results: $.map(data, function (item) {
                                return {
                                    text: item.username,
                                    id: item.id
                                }
                            }),
                            pagination: {
                                more: (params.page * 30) < data.recordsTotal
                            }
                        };
                    },
                    cache: true
                },
                escapeMarkup: function (markup) { return markup; }, // let our custom formatter work
                minimumInputLength: 3
            });

            $('#searchUser').change(function () {
                $('#insert_name').html(' User: '+$('#s2id_searchUser .select2-chosen').text());
                // $('#searchUseradd').select2('data', {id: $('#searchUser').val(), text: $('#s2id_searchUser .select2-chosen').text()});
                // $('#searchUseradd').attr('value',$('#searchUser').val());
                $('#searchUseradd').val($('#searchUser').val()).change();
            })


            $('#searchBy').change(function () {
                if($(this).val() == 1) {
                    $('#filter_jurisdiction').hide();
                    $('#filter_username').show();
                    $('#insert_name').html(' User: '+$('#s2id_searchUser .select2-chosen').text());
                    $('#searchByadd').val(1).change();
                    //$('#searchByadd').html('<option value="1" selected ><?//=$lang->getLang("Username")?>//</option><option value="2"  ><?//=$lang->getLang("Jurisdiction")?>//</option>');
                    return false;
                }
                if($(this).val() == 2) {
                    $('#filter_jurisdiction').show();
                    $('#filter_username').hide();
                    $('#insert_name').html(' Jurisdiction: '+$('#jurisdiction_select option:selected').text());
                    $('#searchByadd').val(2).change();
                    //$('#searchByadd').html('<option value="1" ><?//=$lang->getLang("Username")?>//</option><option value="2" selected ><?//=$lang->getLang("Jurisdiction")?>//</option>');
                    return false;
                }
            });

            $('#jurisdiction_level').change(function () {
                $('#jurisdiction_level_1').val($(this).val()).change();
            });

            $('#jurisdiction_level').change(function () {
                // $('#jurisdiction_level_1').change();
            });

            $(document).on('change','#jurisdiction_select select[name="jurisdiction"]',function(){
                $('#jurisdiction_select_1').val($(this).val()).change();
                $('#insert_name').html(' Jurisdiction: '+$('#jurisdiction_select option:selected').text());
            });

            $('#searchByadd').change(function () {
                if($(this).val() == 1) {
                    $('.by_jurisdiction').hide();
                    $('.by_user').show();
                    return false;
                }
                if($(this).val() == 2) {
                    $('.by_jurisdiction').show();
                    $('.by_user').hide();
                    return false;
                }
            });
        });

        $('.inputnumber').inputFilter(function (value) {
            return /^\d*[.]?\d{0,2}$/.test(value);
        });
    });

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

global $dbh,$lang;
$providers=$dbh->doCachedQuery("Select * from providers");
$providers_list ='';
$providers_list2 ='';
while($providers->hasNext()){
    $row=$providers->next();

    $providers_list .= "<option value='".$row['pes_game_code']."'>".$row['pes_name']."</option>";
    $providers_list2 .= "<option value='".$row['pes_id']."'>".$row['pes_name']."</option>";

}
?>
<div id='loadingDiv' class="centered"><img src="<?=image_dir?>/ajax-loader.gif" /></div>

<form id="modalFormAddLimit" novalidate>
    <div id="myModal" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
        <input type="hidden" name="action" value="addGameLimit" >
        <div class="modal-header">
            <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
            <h3 class="text-info text-center"><?=$lang->getLang("Add game limit")?> <span id="myModalLabel"></span></h3> <br />
        </div>
        <div class="modal-body">
            <p>
            <table class='table table-condensed '>
                <tr>
                    <td class='blackTd'><?=$lang->getLang("Insert limit for")?></td>
                    <td id="insert_name">
<!--                        <select name="searchByadd" id="searchByadd" class="searchByadd">-->
<!--                            <option value="1"  >--><?//=$lang->getLang("Username")?><!--</option>-->
<!--                            <option value="2"  >--><?//=$lang->getLang("Jurisdiction")?><!--</option>-->
<!--                        </select>-->
                    </td>
                    <input type="hidden" id="searchByadd" name="searchByadd" value="1">
                    <input type="hidden" id="searchUseradd" name="searchUseradd">
                    <input type="hidden" id="jurisdiction_level_1" name="jurisdiction_level_1">
                    <input type="hidden" id="jurisdiction_select_1" name="jurisdiction">
                </tr>
<!--                <tr class="by_user">-->
<!--                    <td class='blackTd'>--><?//=$lang->getLang("Search Username")?><!--</td>-->
<!--                    <td>-->
<!--                        <input type='text'  id='searchUseradd' name='searchUseradd' style="width: 200px" value='0'   placeholder="--><?//=$lang->getLang('Search')?><!--" required="required" >-->
<!--                    </td>-->
<!--                </tr>-->
<!--                --><?php
//
//                include ("configuration/jurisdiction_filter_black.php");
//
//                ?>

                <tr>
                    <td class='blackTd'><?=$lang->getLang("Deposit Limit")?></td>
                    <td>
                        <input type="text" class="inputnumber" id="deposit_limit_add" name="deposit_limit_add" value="0.00" size="13">
                    </
                </tr>
                <tr>
                    <td class='blackTd'><?=$lang->getLang("Withdraw Limit")?></td>
                    <td>
                        <input type="text" class="inputnumber" id="withdraw_limit_add" name="withdraw_limit_add" value="0.00" size="13">
                    </td>
                </tr>
                <tr>
                    <td class='blackTd'><?=$lang->getLang("Bet Limit")?></td>
                    <td>
                        <input type="text" class="inputnumber" id="bet_limit_add" name="bet_limit_add" value="0.00" size="13">
                    </td>
                </tr>
                <tr>
                    <td class='blackTd'><?=$lang->getLang("Win Limit")?></td>
                    <td>
                        <input type="text" class="inputnumber" id="win_limit_add" name="win_limit_add" value="0.00" size="13">
                    </td>
                </tr>
                <tr >
                    <td class='blackTd'><?=$lang->getLang("Provider")?></td>
                    <td>
                        <select  name='searchProvideradd' id='searchProvideradd' class='' >
                            <option value='ALL' >ALL</option>
                            <?=$providers_list?>
                        </select>
                    </td>
                </tr>

                <tr>
                    <td class='blackTd'><?=$lang->getLang("Is lock")?></td>
                    <td>
                        <select name="Lockadd" id="Lockadd" class="searchByadd">
                            <option value='0' class='text-error' ><?=$lang->getLang("False")?></option>
                            <option value='1' class="text-success"><?=$lang->getLang("True")?></option>
                        </select>
                    </td>
                </tr>
            </table>
            </p>
        </div>
        <div class="modal-footer">
            <button class="btn" data-dismiss="modal" aria-hidden="true"><?=$lang->getLang("Cancel")?></button>
            <button class="btn btn-primary" id="sendModal" name="save"><?=$lang->getLang("Add")?></button>
        </div>
    </div>
</form>

<form id="modalFormUpdateGameLimit" novalidate>
    <div id="updateGameLimit" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
        <input type="hidden" name="action" value="updateGameLimit" >
        <input type="hidden" name="update_jur_type" id="update_jur_type" value="">
        <input type="hidden" name="update_jur_id" id="update_jur_id" value="">
        <div class="modal-header">
            <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
            <h3 class="text-info text-center"><?=$lang->getLang("Update game limit")?> <span id="myModalLabel"></span></h3> <br />
            <h3 class="text-info text-center" id="jur_title_update"></h3> <br />
        </div>
        <div class="modal-body">
            <p>
            <table class='table table-condensed '>



                <tr>
                    <td class='blackTd'><?=$lang->getLang("Deposit Limit")?></td>
                    <td>
                        <input type="text" class="inputnumber" id="deposit_limit_update" name="deposit_limit_update" value="0.00" size="13">
                    </
                </tr>
                <tr>
                    <td class='blackTd'><?=$lang->getLang("Withdraw Limit")?></td>
                    <td>
                        <input type="text" class="inputnumber" id="withdraw_limit_update" name="withdraw_limit_update" value="0.00" size="13">
                    </td>
                </tr>
                <tr>
                    <td class='blackTd'><?=$lang->getLang("Bet Limit")?></td>
                    <td>
                        <input type="text" class="inputnumber" id="bet_limit_update" name="bet_limit_update" value="0.00" size="13">
                    </td>
                </tr>
                <tr>
                    <td class='blackTd'><?=$lang->getLang("Win Limit")?></td>
                    <td>
                        <input type="text" class="inputnumber" id="win_limit_update" name="win_limit_update" value="0.00" size="13">
                    </td>
                </tr>

                <tr >
                    <td class='blackTd'><?=$lang->getLang("Provider")?></td>
                    <td>
                        <select  name='searchProviderupdate' id='searchProviderupdate' class='' >
                            <option value='ALL'>ALL</option>
                            <?=$providers_list?>
                        </select>
                    </td>
                </tr>

                <tr>
                    <td class='blackTd'><?=$lang->getLang("Is lock")?></td>
                    <td>
                        <select name="Lockupdate" id="Lockupdate" class="searchByupdate">
                            <option value='0' class='text-error' ><?=$lang->getLang("False")?></option>
                            <option value='1' class="text-success"><?=$lang->getLang("True")?></option>
                        </select>
                    </td>
                </tr>
            </table>
            </p>
        </div>
        <div class="modal-footer">
            <button class="btn" data-dismiss="modal" aria-hidden="true"><?=$lang->getLang("Cancel")?></button>
            <button class="btn btn-primary" id="sendModalupdate" name="save"><?=$lang->getLang("Update")?></button>
        </div>
    </div>
</form>
<div class="container-fluid ">
    <ul class="nav nav-tabs" id="navTabList">
        <li class="active"><a data-toggle="tab" href="#games_limit"><?=$lang->getLang("Games limit")?></a></li>
        <?php if(check_access('transactions_provider_pending_manage')) { ?>
        <li class=""><a data-toggle="tab" href="#pending_trans"><?=$lang->getLang("Pending Transactions")?></a></li>
        <?php } ?>
    </ul>

    <div class="tab-content" id="navTabContent">
        <div id="games_limit" class="tab-pane fade in active">
            <table class="table table-bordered">
                <tr>
                    <td class="navbar-inner" colspan="2"  ><h3 class="text-center"><?=$lang->getLang("Games Limit Management")?></h3></td>
                </tr>
                <tr>
                    <td colspan="2">
                        <div class="well">
                            <form action="/gameslimit_management" method="post">
                                <?php printHiddenInput('doQuery', 'yes');?>
                                <div class="row">
                                    <div class="span5">
                                        <div class='label label-inverse'><?=$lang->getLang("Search by")?></div>
                                        <br>
                                        <div class="controls">
                                            <div class="input-prepend">

                                                <select name="searchBy" id="searchBy" class="searchBy">
                                                    <option <?=($searchBy==1 ? 'selected="selected"': '')?> value="1"  ><?=$lang->getLang("Username")?></option>
                                                    <option <?=($searchBy==2 ? 'selected="selected"': '')?> value="2"  ><?=$lang->getLang("Jurisdiction")?></option>
                                                </select>
                                            </div>
                                        </div>
                                        <div id="filter_username"><div class='label label-inverse searchLabel' id="searchLabel" content="searchLabel"><?=$lang->getLang("Search Username")?></div>
                                            <br />
                                            <div class="controls">
                                                <div class="input-prepend">

                                                    <input type='text'  id='searchUser' name='searchUser' style="width: 200px"   placeholder="<?=$lang->getLang('Search')?>" required="required" >
                                                    <input id="exact_user_search" type="checkbox" title="Exact search">
                                                </div>
                                            </div>
                                        </div>
                                    </div>

                                </div>
                                <div class="row"  id="filter_jurisdiction"  style="display:none;">
                                    <div class="span5">
                                        <?php
                                        include ("configuration/jurisdiction_filter_new.php");
                                        ?>
                                    </div>
                                </div>

                                <div class="row error_message">
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
                            <!--                --><?php
                            //                $starttime = microtime(true);
                            //                $endtime = microtime(true);
                            //                $duration = $endtime - $starttime;
                            //                $duration=number_format($duration, 4, ',', '')."s";
                            //                $time="<br /><div class='tip fleft' id='duration'>".$lang->getLang("Time taken to execute your request").":".$duration."</div>";
                            //                echo $time;
                            //                ?>
<!--                            <a href="#updateGameLimit" class="updateGameLimit fleft btn btn-info btn-small" style="color:white; display:none;" data-toggle="modal" >--><?//=$lang->getLang("Update game limit")?><!--</a>-->
<!--                            <a href="#" class="deleteGameLimit fleft btn btn-danger btn-small" style="color:white; display:none;" data-toggle="modal" >--><?//=$lang->getLang("Delete")?><!--</a>-->
                            <a href="#myModal" class="addGameLimit fright btn btn-info btn-small" style="color:white" data-toggle="modal" ><?=$lang->getLang("Add game limit")?></a>
                            <br /><br /><br />
                            <table id="partnersTableContainer" class='display table table-striped table-hover table-bordered table-condensed' style="width: 90%">
                                <thead>
                                <tr>
                                    <th><?=$lang->getLang("Nation")?></th>
                                    <th><?=$lang->getLang("Region")?></th>
                                    <th><?=$lang->getLang("District")?></th>
                                    <th><?=$lang->getLang("Club")?></th>
                                    <th><?=$lang->getLang("Username")?></th>
                                    <th><?=$lang->getLang("Creation")?><br><?=$lang->getLang("Last update")?></th>
                                    <th><?=$lang->getLang("Deposit Limit")?></th>
                                    <th><?=$lang->getLang("Withdraw Limit")?></th>
                                    <th><?=$lang->getLang("Bet Limit")?></th>
                                    <th><?=$lang->getLang("Win Limit")?></th>
                                    <th><?=$lang->getLang("Provider Name")?></th>
                                    <th><?=$lang->getLang("Action")?></th>
                                </tr>
                                </thead>
                                <tbody>
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
        </div>

        <div id="pending_trans" class="tab-pane fade in">
            <table class="table table-bordered" >
                <tr>
                    <td class="navbar-inner" colspan="2"  ><h3 class="text-center"><?=$lang->getLang("Pending transactions")?></h3></td>
                </tr>
                <tr>
                    <td colspan="2">
                        <div class="well">
                            <form action="/gameslimit_management" method="post">
                                <?php printHiddenInput('doQuery', 'yes');?>
                                <div class="row">
                                    <div class="span3">
                                        <div id="filter_username"><div class='label label-inverse searchLabel' id="searchLabel" content="searchLabel"><?=$lang->getLang("Search Username")?></div>
                                            <br />
                                            <div class="controls">
                                                <div class="input-prepend">
                                                    <input type='text'  id='trans_user' name='trans_user' style="width: 200px"   placeholder="<?=$lang->getLang('Search')?>" required="required" >
                                                    <input id="exact_user" type="checkbox" title="<?=$lang->getLang("Exact search")?>">
                                                </div>
                                            </div>
                                        </div>
                                    </div>
                                    <div class="span3">
                                        <div class='label label-inverse'><?=$lang->getLang("Status")?></div>
                                        <div class="controls">
                                            <select name="trans_status" id="trans_status" class="searchBy">
                                                <option value="all" ><?=$lang->getLang("All")?></option>
                                                <option value="1" ><?=$lang->getLang("Open")?></option>
                                                <option value="0" ><?=$lang->getLang("Close")?></option>
                                            </select>
                                        </div>
                                    </div>
                                    <div class="span3">
                                        <div class='label label-inverse'><?=$lang->getLang("Providers")?></div>
                                        <div class="controls">
                                            <select  name='trans_provider' id='trans_provider' class='' >
                                                <option value='ALL' >ALL</option>
                                                <?=$providers_list2?>
                                            </select>
                                        </div>
                                    </div>
                                </div>
                                <div class="row">
                                    <div class="span3">
                                        <div class="controls">
                                            <div class='label label-inverse'><?=$lang->getLang("From date")?></div>
                                            <input type="text" id="date_start" name="date_start" value="<?=$date_start?>" />
                                        </div>
                                    </div>
                                    <div class="span3">
                                        <div class="controls">
                                            <div class='label label-inverse'><?=$lang->getLang("To date")?></div>
                                            <input type="text" id="date_end" name="date_end" value="<?=$date_end?>" />
                                        </div>
                                    </div>
                                </div>


                                <div class="row error_message_pending">
                                </div>

                                <div class="row">
                                    <span class="span2"> <button class="btn btn-primary" id="searchPending"><?=$lang->getLang("Search")?></button></span>
                                </div><br/>
                            </form>
                        </div>
                    </td>
                </tr>
                <tr>
                    <td>

                        <div style="width: 100%">
                            <br/><br/><br/>
                            <table id="pendingTableContainer" class='display table table-striped table-hover table-bordered table-condensed' style="width: 90%">
                                <thead>
                                <tr>
                                    <th><?=$lang->getLang("Username")?></th>
                                    <th><?=$lang->getLang("Admin")?></th>
                                    <th><?=$lang->getLang("Provider")?></th>
                                    <th><?=$lang->getLang("Creation")?></th>
                                    <th><?=$lang->getLang("Last update")?></th>
                                    <th><?=$lang->getLang("Transaction no.")?></th>
                                    <th><?=$lang->getLang("Amount")?></th>
                                    <th><?=$lang->getLang("Balance")?></th>
                                    <th><?=$lang->getLang("Type")?></th>
                                    <th><?=$lang->getLang("Status")?></th>
                                    <th><?=$lang->getLang("Note")?></th>
                                    <th><?=$lang->getLang("Action")?></th>
                                </tr>
                                </thead>
                                <tbody>
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
        </div>
    </div>
</div>