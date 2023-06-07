
<?php
include_once "funclib/percentual.inc.php";
$manageAffilliates=check_access('manage_affiliates');
$acceptregistration=check_access('manage_accept_affiliate_registration');
$active=(isset($_POST['active'])? $_POST['active']:'register');
$ip=getIpAddress();
$country = (isset($_POST ['countries'])? $_POST ['countries'] : geoip_country_code_by_name($ip));
$gender = $_POST ['gender'];
$address = $_POST ['street'];
$username = $_POST ['username'];
$rules = 'yes';
$password=md5($_POST['password']);
$name = $_POST ['contactlastname'];
$firstname = $_POST ['contactname'];
$dob = $_POST ['birthday'];
$email = $_POST ['email'];
$city = $_POST ['city'];
$zcode = $_POST ['zip'];
$phone = $_POST ['phone'];
$_county=$_POST['province'];
$_vat= (empty($_POST['vat']) ? "" : $_POST['vat']);
$encoded=urlencode($username);
$age='yes';
$_commplan=$_POST['betting'];
$_note=$_POST['note'];
$_agencyname=$_POST['name'];

include_once 'admin_users/jurisdiction_changer_js.php';
global $lang;
?>

<style>
    #affiliatesInfoTable{
        white-space: nowrap;
    }
    #affiliatesInfoTree{
        width: 200px;
    }
    .error-popover{
        background-color: rgb(242, 222, 222) !important;
        border-color: rgb(238, 211, 215) !important;
        box-shadow: none;
        color: rgb(185, 74, 72);
        cursor: pointer;
        max-width: none;
        z-index: 1099;
        padding:1px !important;
    }
    .popover.error-popover .arrow {
        border-top-color: rgb(238, 211, 215) !important;
        left: 30px;
    }
    .popover.top .arrow:after {
        bottom: 1px;
        margin-left: -10px;
        border-top-color: rgb(240, 228, 228) !important;
        border-bottom-width: 0;
        padding: 0px;
    }
    .container{
        width: 1480px !important;
    }
    #betting_live_perc input,#betting_perc input{
        width: auto;
    }

</style>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap-responsive.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap-multiselect.css" title="core_style">

<script src="/media/jquery.dataTables.js" type="text/javascript"></script>
<script src="/media/affiliatesJs/ListTable.js" type="text/javascript"></script>
<script src="/media/affiliatesJs/resultFunctions.js" type="text/javascript"></script>
<script src="/media/jstree/jstree.js" type="text/javascript"></script>
<script src="/media/bootstrap/js/bootstrap-multiselect.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/jquery.validate.js" type="text/javascript"></script>
<script type="text/javascript" src="https://maps.googleapis.com/maps/api/js?libraries=places&sensor=false&&language=en&region=US"></script>
<script type="text/javascript">

function checkDistrict(form){

    if($('#dstr').val()!='0'){
        form.submit();
        return true;
    }

    jAlert('<?=$lang->getLang("Choose a district please!")?>');
    return false;
}

$(function() {
    <?=($active=='confirm'? 'intvID = window.setInterval("updateCategories()", 15000);':"")?>
    $('a[data-toggle="tab"]').on('show.bs.tab', function (e) {
        if(e.target.id=='confirmAffiliateTab' ){
            intvID = window.setInterval("updateCategories()", 15000);
        }
        else{
            clearInterval(intvID);
        }
    });
    $( "#birthday" ).datepicker({
        changeMonth: true,
        changeYear:true,
        yearRange: "-80:-18",
        dateFormat: 'yy-mm-dd',
        defaultDate:'-80y',
        onSelect: function(dateStr) {
            $(this).closest("form").validate().element(this);
        }
    });

    $("input[name='city']").on('focus',function(){
        input = this;
            if($('#countries option:selected').val()!=''){
                options = {
                    types: ['(cities)'],
                    componentRestrictions: {
                        country: $('#countries option:selected').val()
                    }
                }
            }
            else{
                options = {
                    types: ['(cities)']
                }
            }

        city = new google.maps.places.Autocomplete(input, options);
        google.maps.event.addListener(city, 'place_changed', function() {
            place = city.getPlace();
            $("input[name='city']").val(place.address_components[0].long_name);
            $("input[name='province']").val(place.address_components[2].short_name).valid();
        })
    });


/*    jQuery.validator.defaults.onfocusout = function (element, event) {
        // detectect if is the element you dont want validate
        // the element has to have the attribute 'data-control="mycitycontrol"'
        // you can also identify your element as you please
        if ($(element).data("control") === "control"){
        if (!this.checkable(element) && (element.name in this.submitted || !this.optional(element))) {
            this.element(element);
        }}
        else{
            return;
        }
    }
    jQuery.validator.defaults.onkeyup = function (element, event) {
        // detectect if is the element you dont want validate
        // the element has to have the attribute 'data-control="mycitycontrol"'
        // you can also identify your element as you please
        if ($(element).data("control") === "control") {
            if (event.which === 9 && this.elementValue(element) === "") {
                return;
            }
            else if (element.name in this.submitted || element === this.lastElement) {
                this.element(element);
            }
        }
        else{
            return;
        }
    }*/


    $.validator.addMethod("alphanumeric", function(value, element) {
        return this.optional(element) || /^[a-z 0-9\_, ]+$/i.test(value);
    }, "Use only letters, numbers, or underscore.");

    $.validator.addMethod("notEqual", function(value, element, param) {
        return this.optional(element) || value != param;
    }, "Please specify value different of 'Username'");
    $.validator.addMethod('ContainsAtLeastOneCapitalLetter',   function (value) {
            return /[A-Z]/.test(value);
        },
        'Your password must contain at least one capital letter.'
    );
    $.validator.addMethod('ContainsAtLeastOneSpecialChar',   function (value) {
            return /[^\w\s]/.test(value);
        },
        'Your password must contain at least one capital letter.'
    );
    $.validator.addMethod('ContainsAtLeastOneNumber',   function (value) {
            return /[0-9]/.test(value);
        },
        'Your password must contain at least one number.'
    );

    $.validator.addMethod("nowhitespace", function(value, element) {
        return this.optional(element) || /^\S+$/i.test(value);
    }, "No white space please");

    var validator3 = $("#webform").validate({
        submitHandler: function(form) {
            checkDistrict(form);
        },
        rules: {
            jurisdiction:"required",
            name: {
                required: true,
                minlength: 2
            },
            vat: {
                required: false
            },
            contactname: {
                required: true,
                minlength: 2
            },
            contactlastname: {
                required: true,
                minlength: 2
            },
            email: {
                required: true,
                email: true,
                remote: {
                    url: "/services/general_services.inc",
                    type: "post",
                    data: {
                        username: function() {
                            return $( "#username" ).val();
                        },
                        action:'checkEmailAlreadyExists'
                    }
                }
            },
            phone: {
                required: true,
                digits: true
            },
            birthday:"required",
            street: "required",
            city:"required",
            countries:"required",
            zip: {
                required: true,
                minlength: 2,
                digits:true
            },
            province:"required",
            username: {
                required: true,
                minlength: 2,
                nowhitespace: true,
                remote: {
                    url: "/services/general_services.inc",
                    type: "post",
                    data: {
                        username: function() {
                            return $( "#username" ).val();
                        },
                        action:'checkAffiliateAlreadyExists'
                    }
                }
            },
            password: {
                required: true,
                minlength: 8,
                ContainsAtLeastOneCapitalLetter:true,
                ContainsAtLeastOneSpecialChar:true,
                ContainsAtLeastOneNumber:true
            },
            password_confirm: {
                required: true,
                minlength: 5,
                equalTo: "#password"
            },
            betting: {
                required: true,
                digits: true
            },
            terms:"required",
            captcha_code:{
                required:true,
                minlength:6,
                maxlength:6
            }
        },
        messages: {
            name: {
                required:"<?=$lang->getLang("Please enter a name")?>",
                minlength:"<?=$lang->getLang("The name has to be at least 2 characters long")?>"
            },
            vat: "<?=$lang->getLang("Please enter your vat number")?>",
            contactname: "<?=$lang->getLang("Please enter your contact name")?>",
            contactlastname: "<?=$lang->getLang("Please enter your contact last name")?>",
            email: {
                required: "<?=$lang->getLang("Please enter your email")?>",
                email:"<?=$lang->getLang("Please enter a valid email address")?>"
            },
            phone:{
                required: "<?=$lang->getLang("Please enter your phone number")?>",
                digits: " <?=$lang->getLang("Only digits are allowed")?>!"
            },
            birthday:" <?=$lang->getLang("Please choose the birthday")?>",
            street:" <?=$lang->getLang("Please enter your address")?>",
            city:" <?=$lang->getLang("Please enter your city")?>",
            zip: {
                required: " <?=$lang->getLang("Please enter a zip")?>",
                minlength: " <?=$lang->getLang("The zip has to be at least 2 numbers long")?>",
                digits:" <?=$lang->getLang("Only digits are allowed")?>!"
            },
            province:" <?=$lang->getLang("Please enter your province")?>",
            countries:" <?=$lang->getLang("Please enter your country")?>",
            username: {
                required: ' <?=$lang->getLang("Your username must be entered")?> ',
                minlength: jQuery.format(' <?=$lang->getLang("The username has to be at least 2 characters long")?>')
            },
            password: {
                required: ' <?=$lang->getLang("Please enter a password")?>',
                minlength: ' <?=$lang->getLang("The password has to be at least 8 characters long")?> ',
                ContainsAtLeastOneCapitalLetter:' <?=$lang->getLang("Your password must contain at least one capital letter!")?>',
                ContainsAtLeastOneSpecialChar:' <?=$lang->getLang("Your password must contain at least one special character(ex:@,#,etc)")?> ',
                ContainsAtLeastOneNumber:' <?=$lang->getLang("Your password must contain at least one number!")?>'
            },
            password_confirm: {
                required: ' <?=$lang->getLang("This field is required")?>',
                minlength: ' <?=$lang->getLang("It has to be at least 8 characters")?>',
                equalTo: " <?=$lang->getLang("The passwords don't match")?>"
            },
            beetting:{
                required: "<?=$lang->getLang("Please enter your the commission")?>",
                digits: " <?=$lang->getLang("Only digits are allowed")?>!"
            },
            terms: " <?=$lang->getLang("You have to accept our terms in order to play on this site!")?>",
            captcha_code: {
                required: " <?=$lang->getLang("Please enter the code above")?>",
                minlength:" <?=$lang->getLang("Invalid code")?>",
                equalTo:" <?=$lang->getLang("Invalid code")?>"
            }
        },
        showErrors: function(errorMap, errorList) {
            $.each( this.successList , function(index, value) {
                $(value).popover('hide');
                $(value).closest('.control-group').removeClass('error').addClass('success');
                $(value).closest('.controls').prev().removeClass('label-inverse label-important').addClass('label-success');
            });
            $.each( errorList , function(index, value) {
                var _popover = $(value.element).popover({
                    trigger: 'manual',
                    placement: 'top',
                    content: value.message,
                    template: '<div class="popover error-popover">' +
                    '       <div class="arrow">' +
                    '       </div>' +
                    '<div class="popover-inner  alert-error">' +
                    '<div class="popover-content  alert-error"><p></p></div></div></div>'
                });
                _popover.data('popover').options.content = value.message;
                $(value.element).popover('show');
                $(value.element).closest('.control-group').removeClass('success').addClass('error');
                $(value.element).closest('.controls').prev().removeClass('label-inverse label-success').addClass('label-important');
            });
        }
    });

    $('[name="jurisdiction"]').multiselect({
        onChange: function(option, checked) {
            getSelectedValue();
            if(option.val()!=''){
                $(".multiselect.btn-danger").removeClass('btn-danger').addClass('btn-success');
                $('#dstr').val('1');
            }else{
                $(".multiselect.btn-success").removeClass('btn-success').addClass('btn-danger');
                $('#dstr').val('0');
            }
        },
        includeSelectAllOption: true,
        enableFiltering: true,
        maxHeight: 300,
        buttonClass: 'btn btn-danger',
        buttonWidth:'250px'
    });
    getSelectedValue();
    <?if(isset($_POST['jurisdiction']) ){
?>
    $('#dstr').val('1');
    $(".multiselect.btn-danger").removeClass('btn-danger').addClass('btn-success');
    <?
    }
    ?>

    if($('[name="jurisdiction"]').val()!=''){
        $('#dstr').val('1');
        $(".multiselect.btn-danger").removeClass('btn-danger').addClass('btn-success');
    }
});

function getSelectedValue(){

    $val = $('[name="jurisdiction"]').val();
    if($val!=''){
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
        }else{
            $('#casino_perc').show();
            var result = data.split("~");
            var details = result[1].split(";");
            for(var i = 0; i < details.length; i++){
                var percDetail = details[i].split(":");
                percDetail[1] = parseInt(percDetail[1]) - 15;
                if(parseInt(percDetail[1]) < 0) percDetail[1] = 0;
                $("#"+percDetail[0]).val(percDetail[1]);
                $("#sel_"+percDetail[0]).val(percDetail[1]);
            }
            $('#error_mess_payments').hide();
        }
    });
    }
}


</script>

<div class="container">
<div class="tabbable tabs-left">
<ul class="nav nav-tabs">
    <?if($manageAffilliates){?>
        <li <?=($active=='register') ? 'class="active"': '' ?>><a href='#registerAffiliate' id="registerAffiliateTab" data-toggle="tab"><?=$lang->getLang("Register Affiliate")?></a></li>
    <? } ?>
    <?if($acceptregistration){ ?>
        <li <?=($active=='confirm') ? 'class="active"': '' ?>><a href='#confirmAffiliate' id="confirmAffiliateTab"  data-toggle="tab"><?=$lang->getLang("Confirm Affiliate")?></a></li>
    <? }?>
</ul>
<div class="tab-content">
<?if($manageAffilliates){?>
    <div class='tab-pane <?=($active=='register') ? 'active': '' ?>' id='registerAffiliate' >
    <input type='hidden' value='0' id='dstr' />
  <form action='/affiliates' method='post' id='webform'>
    <table class="table table-bordered" style="margin: auto;width: auto">
    <tr><td colspan='4' class='navbar-inner'><h3 class="text-center"><?=$lang->getLang("REGISTER AFFILIATE")?></h3></td></tr>
    <tr>
        <td colspan='4' class='centered'><h4 class="text-center"><?=$lang->getLang("Choose District")?></h4>
            <div id="jurisdiction_select" class="centered">
                <?php echo get_jurisdiction_select('district', (isset($_POST['jurisdiction'])?$_POST['jurisdiction']: $_SESSION['jurisdiction_id']));?>
            </div>
        </td>
    </tr>
    <tr>
    <tr>
    <td style="vertical-align: top;padding:0px">
        <table><tr><td>
        <table style="width: 100%">
            <tr><td><h3 class="centered text-success"><?=$lang->getLang("Affiliate Information")?></h3> </td> </tr>
            <tr>
                <td>
                    <div class="span3 control-group control-group"><div class='label label-inverse'><?=$lang->getLang("Affiliate Name")?></div>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-align-justify"></i></span>
                                <input type='text'  name='name'  placeholder="<?=$lang->getLang("New Affiliate Name")?>" value="<?=$_agencyname?>">
                            </div>
                        </div>
                    </div>
                </td>
            </tr>
            <tr>
                <td>
                    <div class="span3 control-group"><div class='label label-inverse'><?=$lang->getLang("Contact Name")?></div>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-align-justify"></i></span>
                                <input type='text'  name='contactname'  placeholder="<?=$lang->getLang("Contact Name")?>" value="<?=$firstname?>">
                            </div>
                        </div>
                    </div>
                </td>
            </tr>
            <tr>
                <td>
                    <div class="span3 control-group"><div class='label label-inverse'><?=$lang->getLang("Contact Last Name")?></div>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-align-justify"></i></span>
                                <input type='text'  name='contactlastname'  placeholder="<?=$lang->getLang("Contact Last Name")?>" value="<?=$name?>">
                            </div>
                        </div>
                    </div>
                </td>
            </tr>
            <tr>
                <td>
                    <div class="span3 control-group"><div class='label label-inverse'><?=$lang->getLang("Email")?></div>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-align-justify"></i></span>
                                <input type='text' data-control="control"  name='email'  placeholder="<?=$lang->getLang("Email")?>" value="<?=$email?>">
                            </div>
                        </div>
                    </div>
                </td>
            </tr>
            <tr>
                <td>
                    <div class="span3 control-group"><div class='label label-inverse'><?=$lang->getLang("Gender")?></div>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-align-justify"></i></span>
                                <select id="gender" name="gender">
                                    <option value='M'><?=$lang->getLang("Male")?></option>
                                    <option value="F"><?=$lang->getLang("Female")?></option>
                                </select>
                            </div>
                        </div>
                    </div>
                </td>
            </tr>
            <tr>
                <td>
                    <div class="span3 control-group"><div class='label label-inverse'><?=$lang->getLang("Birthday")?></div>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-align-justify"></i></span>
                                <input type='text' id='birthday'  name='birthday'  placeholder="<?=$lang->getLang("Birthday")?>" value="<?=$dob?>">
                            </div>
                        </div>
                    </div>
                </td>
            </tr>
        </table>
    </td>
    <td style="padding:0;vertical-align: top">
        <table style="width: 100%">
            <tr><td><h3 class="centered text-info"><?=$lang->getLang("Address")?></h3> </td> </tr>
            <tr>
                <td>
                    <div class="span3 control-group"><div class='label label-inverse'><?=$lang->getLang("Country")?></div>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-align-justify"></i></span>
                                <? countrySelect('countries', "","$country",false,'countries'); ?>
                            </div>
                        </div>
                    </div>
                </td>
            </tr>
            <tr>
                <td>
                    <div class="span3 control-group"><div class='label label-inverse'><?=$lang->getLang("Province")?></div>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-align-justify"></i></span>
                                <input type='text'  name='province'  placeholder="<?=$lang->getLang("Province")?>" value="<?=$_county?>">
                            </div>
                        </div>
                    </div>
                </td>
            </tr>
            <tr>
                <td>
                    <div class="span3 control-group"><div class='label label-inverse'><?=$lang->getLang("City")?></div>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-align-justify"></i></span>
                                <input type='text'  name='city'  placeholder="<?=$lang->getLang("City")?>" value="<?=$city?>">
                            </div>
                        </div>
                    </div>
                </td>
            </tr>
            <tr>
                <td>
                    <div class="span3 control-group"><div class='label label-inverse'><?=$lang->getLang("Street")?></div>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-align-justify"></i></span>
                                <input type='text'  name='street'  placeholder="<?=$lang->getLang("Street")?>" value="<?=$address?>">
                            </div>
                        </div>
                    </div>
                </td>
            </tr>
            <tr>
                <td>
                    <div class="span3 control-group"><div class='label label-inverse'><?=$lang->getLang("Zip/Postal Code")?></div>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-align-justify"></i></span>
                                <input type='text'  name='zip'  placeholder="<?=$lang->getLang("Zip/Postal Code")?>" value="<?=$zcode?>">
                            </div>
                        </div>
                    </div>
                </td>
            </tr>
            <tr>
                <td>
                    <div class="span3 control-group"><div class='label label-inverse'><?=$lang->getLang("Phone")?></div>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-align-justify"></i></span>
                                <input type='text'  name='phone'  placeholder="<?=$lang->getLang("Phone")?>" value="<?=$phone?>">
                            </div>
                        </div>
                    </div>
                </td>
            </tr>
        </table>
    </td>
    <td style="padding:0;vertical-align: top">
        <table style="width: 100%">
            <tr>
                <td><h3 class="centered text-error"><?=$lang->getLang("User Account")?></h3> </td>
            </tr>
            <tr>
                <td>
                    <div class="span3 control-group"><div class='label label-inverse'><?=$lang->getLang("Username")?></div>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-align-justify"></i></span>
                                <input type='text' data-control="control"  name='username' id="username" placeholder="<?=$lang->getLang("Username")?>" value="<?=$username?>">
                            </div>
                        </div>
                    </div>
                </td>
            </tr>

            <tr>
                <td>
                    <div class="span3 control-group"><div class='label label-inverse'><?=$lang->getLang("Password")?></div>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-align-justify"></i></span>
                                <input type='password' id='password'  name='password'  placeholder="<?=$lang->getLang("Password")?>" >
                            </div>
                        </div>
                    </div>
                </td>
            </tr>
            <tr>
                <td>
                    <div class="span3 control-group"><div class='label label-inverse'><?=$lang->getLang("Confirm Password")?></div>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-align-justify"></i></span>
                                <input type='password'  name='password_confirm'  placeholder="<?=$lang->getLang("Confirm password")?>" >
                            </div>
                        </div>
                    </div>
                </td>
            </tr>
            <tr>
                <td>
                    <div class="span3 control-group"><div class='label label-inverse'><?=$lang->getLang("VAT Code")?></div>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-align-justify"></i></span>
                                <input type='text'  name='vat'  placeholder="<?=$lang->getLang("VAT Code")?>" value="<?=$_vat?>">
                            </div>
                        </div>
                    </div>
                </td>
            </tr>
            <!--<tr>
                <td>
                    <div class="span3 control-group"><div class='label label-inverse'><?/*=$lang->getLang("Currency")*/?></div>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-align-justify"></i></span>
                                <select id="currency" name="currency">
                                    <option value="eur">EUR</option>
                                </select>
                            </div>
                        </div>
                    </div>
                </td>
            </tr>-->
        </table>
    </td>
        </tr>
        <tr>
            <td style="text-align: left;vertical-align: top" colspan="3">
                <h3 class="jebetting"><?=$lang->getLang("Note")?></h3>
                <textarea id="note" name="note" rows="5" style="width: 98%"><?=$_POST['note']?></textarea>
            </td>
        </tr>
        <tr>
            <td colspan="3"><button class="btn btn-primary"><?=$lang->getLang("Submit")?></button> </td>
        </tr>


        </table></td>
    <td>
    <table>
      <tr>
    <td style="padding:0;vertical-align: top">
        <table>
            <tr>
                <td>
                    <h3 class="centered text-warning"><?=$lang->getLang("Payments Settings")?></h3>
                </td>
            </tr>
            <tr>
                <td>
                    <table id="casino_perc">
                        <?php
                        showPercentage($_SESSION["jurisdiction_id"],$_SESSION["jurisdiction_class"],-1,"club",'navbar-inner');
                        ?>
                    </table>
                    <table id="error_mess_payments"  style="display:none;width:90%">
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
                        showLiveBettingPercentage($_SESSION["jurisdiction_id"],$_SESSION["jurisdiction_class"], "club",-1, true,'navbar-inner');
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
                </td>
            </tr>
        </table>
    </td>
    </tr>
    </table>
    </td>
    </tr>

    </table>
    <input type="hidden" value='register' name="active" >
    </form>
    </div>
<? } ?>
<?if($acceptregistration){?>
    <div class='tab-pane <?=($active=='confirm') ? 'active': '' ?>' id='confirmAffiliate' >
        <?
        $date_start = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d", time() - (60 * 60 * 24));
        $date_end = (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d", time() + (60 * 60 * 24));
        ?>

        <div id="testresult"></div>
        <table class="table table-bordered">
            <tr>
                <td class="navbar-inner" colspan="2"><h3 class="text-center"><?=$lang->getLang("Confirm Affiliate")?></h3></td>
            </tr>
            <tr>
                <td>
                    <div class="well">
                        <div id="affiliatesInfoContainer">
                            <form name="affiliatesInfoTable_formFilter" id="affiliatesInfoTable_formFilter" method="post" action='affiliates_add'>
                                <div>
                                    <div class="input-prepend">
                                        <span  class="add-on"><?=$lang->getLang("From")?></span>
                                        <input class="span2" type='text'  name='date_start' id='date_start' value="<?=$date_start?>">
                                    </div>
                                </div>
                                <div>
                                    <div class="input-prepend">
                                        <span class="add-on">&nbsp;<?=$lang->getLang("Until")?>&nbsp;</span>
                                        <input type='text' class="span2"  name='date_end' id='date_end' value="<?=$date_end?>">
                                    </div>
                                </div>
                                <button class="btn btn-primary"><?=$lang->getLang("Get Data")?></button>
                                <fieldset>
                                    <legend><?=$lang->getLang("Affiliates")?></legend>
                                    <div class="control-group">
                                        <input type="hidden" name="code" value="<?=$_POST['code']?>" />
                                        <input type="hidden" name="action" value="listAffiliates" />
                                        <input type="hidden" name="active" value="confirm" />
                                    </div>
                                </fieldset>
                            </form>
                            <table id="affiliatesInfoTable" class="display table table-striped table-bordered table-condensed">
                                <thead>
                                <tr>
                                    <th>Pun id</th>
                                    <th><?=$lang->getLang("Affiliate")?></th>
                                    <th><?=$lang->getLang("First Name")?></th>
                                    <th><?=$lang->getLang("Last Name")?></th>
                                    <th><?=$lang->getLang("Registration Date")?></th>
                                    <th><?=$lang->getLang("Jurisdiction Name")?></th>
                                    <th><?=$lang->getLang("Affiliate Type")?></th>
                                    <th><?=$lang->getLang("Operation")?></th>
                                    <th>Reg</th>
                                </tr>
                                </thead>
                                <tbody>
                                </tbody>
                            </table>
                        </div>
                </td>
                <td style='vertical-align: top'>
                    <div id="affiliatesInfoTree"><?=$lang->getLang("Affiliate information content")?></div>
                </td>
            </tr>
        </table>
    </div>
<? }?>
</div>
</div>
</div>
<?
if(isset($_POST['username'])){
global $lang;
$distcode=$dbh->queryOne('Select jur_code from jurisdiction where jur_id='.$dbh->escape($_POST['jurisdiction']));
$skinid = SKINID;
$key=WEBAPP_KEY;
$operationtype = 3030;
$name = $_POST ['contactlastname'];
if(isBlank($name)){
addError('',$lang->getLang('Please enter the last name'));
}
$firstname = $_POST ['contactname'];
if(isBlank($firstname)){
addError('',$lang->getLang('Please enter the first name'));
}

if(isBlank($gender)){
addError('',$lang->getLang('Please choose the last gender'));
}
$dob = $_POST ['birthday'];
if(isBlank($dob)){
addError('',$lang->getLang('Please enter the last name'));
}
$email = $_POST ['email'];
if ( ! isBlank($email ) || DOT_ID){
if ( !isValidEmail($email) )
addError("",$lang->getLang("The email address you entered appears to be invalid"));
}
$city = $_POST ['city'];
if(isBlank($city)){
addError('',$lang->getLang('Please enter the last city'));
}
$zcode = $_POST ['zip'];
if(isBlank($zcode)){
addError('',$lang->getLang('Please enter the postal code'));
}
$phone = $_POST ['phone'];
if(isBlank($phone)){
addError('',$lang->getLang('Please enter the phone number'));
}

if(isBlank($password)){
addError('',$lang->getLang('Please enter the password'));
}

if(isBlank($username)){
addError('',$lang->getLang('Please enter the username'));
}

if(isBlank($address)){
addError('',$lang->getLang('Please enter the street'));
}



$admin=1;
$settings="c:".$_POST['percentuals']['JPS_PERC_CASINO'].";cl:".$_POST['percentuals']['JPS_PERC_CASINO_LIVE'].";p:".$_POST['percentuals']['JPS_PERC_POKER'].";pl:".$_POST['percentuals']['JPS_PERC_POKER_LIVE'].";t:".$_POST['percentuals']['JPS_PERC_TOTEM'].";v:".$_POST['percentuals']['JPS_PERC_VIRTUAL'].";pt:".$_POST['percentuals']['JPS_PERC_PLAYTECH'].";ev:".$_POST['percentuals']['JPS_PERC_EVOLUTION'].";art:".$_POST['percentuals']['JPS_PERC_ARTBET'];
$b_settings=getBettingString($_commplan);
$lb_settings=getLiveBettingString($_POST['betting_live']);

$md5 = md5 ( $skinid . $username . $firstname . $name . $dob . WEBAPP_KEY );
if(!areErrors()){

$dataToSend="psw=$password&date=$dob&first=$firstname&last=$name&county=$_county&gender=$gender&address=$address&city=$city&country=$country&phone=$phone&email=$email&zcode=$zcode&vat=$_vat&commplan=$_commplan&agencyname=$_agencyname&note=$_note&md5=$md5&operationtype=$operationtype&skinid=$skinid&18yrs=$age&rules=$rules&sessionid=$key&distcode=$distcode&username=$encoded&admin=$admin&sett=$settings&bsett=$b_settings&lbsett=$lb_settings";
var_dump($dataToSend);
$response=sendToWebApp($dataToSend,WEB_APP_WEBSITES);

if($response!='0' || $response==''){
 error_log("QUERRY ERROR:Affiliates add request error.Response from web app:".$response);
include_once 'Error.class.inc';
$error=new Error($response);
?>
<script type="text/javascript">
    jAlert('<?=$error->checkError()?>');
</script>
<?
}
else{
    if(!areErrors()){
        ?>
        <script type="text/javascript">
            $(function(){
            jAlert('<?=$lang->getLang("You have successfully registered an affiliate!")?>');
                $('#webform').find("input[type=text], textarea, select").val("");
            })
        </script>
    <?
    }
}

}
else
{
    showErrors();
}
}
?>
