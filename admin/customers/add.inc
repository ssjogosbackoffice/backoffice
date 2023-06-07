<?php
check_access("customers_register", true);

if ( $_SESSION['jur_users_limit'] <= 0 && $_SESSION['jurisdiction_class'] == 'club') {       
    die("<strong style='color: red;'>You reached the users creation limit!!</strong>");    
    exit;
}
$jurisdiction_id    = $_SESSION["jurisdiction_id"];
$jurisdiction_class = $_SESSION["jurisdiction_class"];
$skin = getSkinDetailsByJurisdiction($jurisdiction_id, $jurisdiction_class);
$section=(isset($_POST['section'])?$_POST['section']:'0');
define("WEBSITE_ROOT", realpath(WEB_ROOT . "/../"));
$fastRegisterAccess=check_access("customers_register_fast");
if(isset($_POST['searchUsername'])) {
    $error = '';
    $usernameInput = $_POST['username'];

    if ($usernameInput != '') {
        $sql = "SELECT count( * ) FROM punter where pun_username = '$usernameInput' ";
        $total = $dbh->queryOne($sql);
        if ($total != 0 ) {
            $error = "Username exists!";
        }
    } else {
        $error = "Empty username";
    }
    echo $error;
    die();
}
?>

<script src="/media/jquery.multiselect.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="/media/jquery.multiselect.css" title="core_style">
<script type="text/javascript">
    $(function() {

        $(".usrSrch").focusout(function() {
            var usrnme = $(this).val();
            $.post("customers/add.inc", {
                searchUsername : "yes",
                username : usrnme
            }, function(data) {
                //change the html of the error div to display this new error
                //console.log(data);
                var error = $('.errorDisplay');
                if (data != '') {

                    error.empty().html("<span class='error'>"+ data + "</span>");
                }
                else {
                    error.empty();
                }
                console.log("done");
            })
        })

        $(".emptyError").click( function() {
            $('.errorDisplay').empty();
        })
        $(function() {
            $( "#tabs" ).tabs({'active' : <?=$section?>});
        });
        var $skin = $("#skin");

        $("#jurisdiction").multiselect({
            multiple: false,
            header: "Select an option",
            noneSelectedText: "Select an Option",
            selectedList: 1,
            click: function(event, ui){
                $.post("/services/jurisdiction_services.inc",{action:"7",
                    jur_id:ui.value
                }, function(data){
                    $skin.text(data);
                });

            }
        }).multiselectfilter();

        $("#jurisdictionFast").multiselect({
            multiple: false,
            header: "Select an option",
            noneSelectedText: "Select an Option",
            selectedList: 1,
            click: function(event, ui){
                $.post("/services/jurisdiction_services.inc",{action:"7",
                    jur_id:ui.value
                }, function(data){
                    $('#skinFast').text(data);
                });

            }
        }).multiselectfilter();
        $( "#dob" ).datepicker({
            changeMonth: true,
            changeYear: true,
            numberOfMonths:1,
            yearRange: "-99:-18",
            defaultDate:'-99y'
        });

        $.post("/services/jurisdiction_services.inc",{action:"7",
            jur_id:$("#jurisdiction").val()
        }, function(data){
            $skin.text(data);
        });
       $.post("/services/jurisdiction_services.inc",{action:"7",
            jur_id:$("#jurisdictionFast").val()
        }, function(data){
           $('#skinFast').text(data);
        });

    });
  /*  function openIdWindow(form){
        var str = form.id_documents.value
        openWindow('/customers/idtypes_pop.html?str='+str, 'idtypes', 360,50,'no','no')
    }

    function changeDialCode (countryselect){
        var form  = countryselect.form;
        var index = countryselect.selectedIndex;
        var id    = countryselect.options[index].value;
        var x     = phone_codes[id];

        if ( phone_codes[id] != null)
        {	form.cou_code_business.value = phone_codes[id];
            form.cou_code_home.value = phone_codes[id];
            form.cou_code_mobile.value = phone_codes[id];
            form.cou_code_fax.value = phone_codes[id];
        }
        else
        {	form.cou_code_business.value = '??';
            form.cou_code_home.value = '??';
            form.cou_code_mobile.value = '??';
            form.cou_code_fax.value = '??';
        }
    } */
</script>
<style>
    .tips{
        color:#CCC;
    }
    .error {
        font-size:10px;
    }
</style>
<?


function update_user_limit_creation($jurisdictionId) {
    
    global $dbh,$lang;
    if ($_SESSION['jurisdiction_class'] != 'club') {
        return TRUE;
    }
    $sql = "UPDATE jurisdiction SET jur_users_limit = jur_users_limit - 1 WHERE jur_id = " . $jurisdictionId . " ";
    $rs  = $dbh->exec($sql);
    if (!$rs) {
        addError('generic_error', $lang->getLang('Unable to update Users creation limit'));
        return FALSE;
    } else {
        $_SESSION['jur_users_limit'] = $_SESSION['jur_users_limit'] - 1;
        return TRUE;
    }
}


function fastRegisterCustomer($username,$password,$pun_club_id){
    global $dbh,$lang;
    $id  = $dbh->nextSequence('pun_id_seq');
    $customer_number = generateCustomerNumber($username.$username);
    $_site = getSkinId($pun_club_id);
    $sql="SELECT * from punter where pun_username=".$dbh->prepareString($username);
    $rs=$dbh->queryOne($sql);
    if($rs>0){
        $rs=-11;
        return  $rs;
    }

    $sql  = "INSERT INTO punter
            (  pun_id,  pun_customer_number, pun_username, pun_password
            ,  pun_first_name,  pun_last_name, pun_dob,  pun_member_type, pun_reg_date, pun_access
            ,  pun_betting_club, pun_site_id)
            VALUES ($id, $customer_number, ".db_quote($username).", ".$dbh->prepareString($password).
            ", " . db_quote($username).", " . db_quote($username) .", '1970-01-01', 'LIMITED', CURRENT_TIMESTAMP
            , 'ALLOW',  " . $pun_club_id.", " . $dbh->escape($_site).")";


    $rs = $dbh->exec($sql);

    if($rs!=1){
        $rs=-1;
        return $rs;
    }

    $log  = "id:$id|customer_number:$customer_number|username:$username|" .
        "password:$password|first_name:$username|" .
        "last_name:$username|member_type:LIMITED|reg_date:".databaseTimestampToday()."|" .
        "access:ALLOW|site_id:".$_site;
    doAdminUserLog($_SESSION['admin_id'], 'pre-register_customer', escapeSingleQuotes($log), $id);
    update_user_limit_creation($_SESSION['jurisdiction_id']);
    return $id;

}


function createCustomer( $username,
                         $password,
                         $pun_club_id,
                         $first_name,
                         $last_name,
                         $dob,
                         $email,
                         $address_line1,
                         $city_suburb,
                         $postcode_zip,
                         $country_code,
                         $phone_home,
                         $nin_code,
                         $address_line2,
                         $gender,$_site=0
){
    global $dbh,$lang;
    $id  = $dbh->nextSequence('pun_id_seq');
    $admin_user_rec = getAdminUser($_SESSION['admin_id']);
    $customer_number = generateCustomerNumber($first_name.$last_name);
    //temporary placeholders
    $country_row   = getCountry($country_code);
    $cou_id        = (int)$country_row['cou_id'];
    $confirmation_list = "";
    $first_name    = ucwords($first_name);
    $last_name     = ucwords($last_name);
    $sql="SELECT * from punter where pun_username='$username'";
    $rs=$dbh->queryOne($sql);
    if($rs>0){
        $rs=-11;
        return  $rs;
    }
    $sql="SELECT * from punter where  pun_email='$email'";
    $rs=$dbh->queryOne($sql);
    if($rs>0){
        $rs=-123;
        return  $rs;
    }
    $sql  = "INSERT INTO punter
            (  pun_id,  pun_customer_number, pun_username, pun_password
            ,  pun_first_name,  pun_last_name, pun_email
            ,  pun_address_line1, pun_address_line2
            ,  pun_city_suburb,  pun_cou_code, pun_cou_id
            ,  pun_postcode_zip, pun_phone_home, pun_dob, pun_gender
            ,  pun_member_type, pun_reg_date, pun_access
            , pun_internal_customer
            ,  pun_confirmation_list
            ,  pun_betting_club, pun_nin_code, pun_nonfinancial_reg_date,pun_site_id)
            VALUES ($id, $customer_number, ".db_quote($username).", '$password'
            , " . db_quote($first_name).", " . db_quote($last_name) .", ".db_quote(strtolower($email)) . "
            , " . db_quote($address_line1).", ".db_quote($address_line2).", ".db_quote($city_suburb) . "
            , ".db_quote($country_code).", $cou_id
            , '$postcode_zip', '$phone_home'
            , '$dob', '$gender', 'FINANCIAL', CURRENT_TIMESTAMP
            , 'allow', 1, '$confirmation_list'
            , " . db_quote($pun_club_id).", " . db_quote($nin_code)." ,  current_timestamp," . $dbh->escape($_site).")";
        //  pun_pre_registered removed from insert with value '1'
$rs = $dbh->exec($sql);
    if($rs!=1){
        $rs=-1;
        return $rs;
    }
    $log  = "id:$id|customer_number:$customer_number|username:$username|" .
        "password:$password|first_name:$first_name|" .
        "last_name:$last_name|email:$email|address_line1:$address_line1|" .
        "address_line2:$address_line2|city_suburb:$city_suburb|" .
        "|country_code:$country_code|" .
        "cou_id:$cou_id|" .
        "phone_home:$phone_home|" .
        "|dob:$dob|gender:$gender" .
        "member_type:FINANCIAL|reg_date:".databaseTimestampToday()."|" .
        "access:limit|pre_registered:1|internal_customer:1|" .
        "confirmation_list:$confirmation_list|" .
        "sre_code:".$admin_user_rec['sales_code'].
        "site_id:".$_site;
        doAdminUserLog($_SESSION['admin_id'], 'pre-register_customer', escapeSingleQuotes($log), $id);        
        update_user_limit_creation($_SESSION['jurisdiction_id']);
return $id;
}

//checks the fields (personal details form) submitted by the registering customer
function checkPersonalForm ($username,
                            $password,
                            $confirm_password,
                            $pun_club_id,
                            $first_name,
                            $last_name,
                            $dob,
                            $email,
                            $address_line1,
                            $city_suburb,
                            $postcode_zip,
                            $country_code,
                            $phone_home,
                            $nin_code,
                            $nationality=null,
                            $phone_mobile=null,
                            $state_province=null,
                            $cob=null,
                            $bc=null
                            ){
    global $dbh,$lang;
    if ( isBlank($username ) )
        addError("",$lang->getLang("A username must be entered"));
    if ( isBlank($password ) )
        addError("",$lang->getLang("Insert a password for user"));
    if ( isBlank($confirm_password ) )
        addError("",$lang->getLang("Insert the password confirm"));
    if ( $confirm_password !=$password )
        addError("",$lang->getLang("The password and password confirmation do not match"));
    if ( isBlank($pun_club_id) )
        addError("",$lang->getLang("A club must be selected"));
    if ( isBlank($first_name ) )
        addError("",$lang->getLang("A first name must be entered"));
    if ( isBlank($last_name ) )
        addError("",$lang->getLang("Empty Last Name"));
    if ( ! isBlank($email ) || DOT_ID)
        if ( !isValidEmail($email) )
            addError("",$lang->getLang("The email address you entered appears to be invalid"));
        else
        {  if ( emailIsRegistered($email) )
            addError("", $lang->getLang("That email address is already being used. Please use a different one."));
        }
    if ( isBlank($dob) )
        addError("", $lang->getLang("Invalid birthday"));

    if(is_null($phone_mobile)){
        if ( !isValidPhoneNumber($phone_home) )
                addError("", $lang->getLang("Please enter a valid phone number"));
    }
    if ( isBlank($address_line1) )
        addError("", $lang->getLang("An address (line 1) must be entered"));
    if ( isBlank($city_suburb) )
        addError("", $lang->getLang("A city or suburb must be entered"));
    if ( isBlank($postcode_zip) )
    addError("", $lang->getLang("Please enter a zip"));
    if ( isBlank($country_code) )
        addError("", $lang->getLang("A country must be selected"));
    elseif ( ! getCountry($country_code) )
        addError("", $lang->getLang("Invalid country"));
        if(isBlank($nin_code)){
            addError("", $lang->getLang("Invalid National Insurance Number."));
        }else{
            if(!checkNinCode($nin_code)){
                addError("", $lang->getLang("Invalid National Insurance Number."));
            }
            if(ninCodeIsRegistered($nin_code)){
                addError("", $lang->getLang("National Insurance number already used."));
            }
        }
    if(!is_null($nationality)){
        if ( isBlank($nationality) )
            addError("", $lang->getLang("A nationality must be entered"));
    }
    if(!is_null($bc)){
        if ( isBlank($bc) )
            addError("", $lang->getLang("A country of birth must be selected"));
        elseif ( ! getCountry($bc) )
            addError("", $lang->getLang("Invalid country of birth"));
    }
    if(!is_null($phone_mobile)){
        if ( !isValidPhoneNumber($phone_mobile) )
            addError("", $lang->getLang("Please enter a valid phone number"));
    }
    if(!is_null($state_province)){
        if ( isBlank($state_province) )
            addError("", $lang->getLang("A state/province must be entered"));
    }
    if(!is_null($cob)){

        if ( isBlank($cob) )
            addError("", $lang->getLang("A city of birth must be entered"));
    }
    return !areErrors();
}


if($section=='1'){
    $username = $_POST['username'];
    $password = $_POST['password'];
    $confirm_password = $_POST['confirm_password'];
    if ( isBlank($username ) )
        addError("",$lang->getLang("A username must be entered"));
    if ( isBlank($password ) )
        addError("",$lang->getLang("Insert a password for user"));
    if ( isBlank($confirm_password ) )
        addError("",$lang->getLang("Insert the password confirm"));
    if ( $confirm_password !=$password )
        addError("",$lang->getLang("The password and password confirmation do not match"));

    $password = md5($password);
    if(!areErrors()){
       $response=fastRegisterCustomer($username,$password,$_POST['jurisdiction']);
        if ($response < 0) {
            include_once 'Error.class.inc';
            $error = new Error($response);
            addError('', $error->checkError());

        } else {
            if (!areErrors()) {
                if ($response == '0') {
                    $sql = "select pun_id from punter where pun_username='$username' and pun_email='$email' AND pun_dob='" . $dob . "'";
                    $response = $dbh->queryOne($sql);
                    if (!$response || $response == NULL) {
                        sleep(1);
                        $response = $dbh->queryOne($sql);
                    }
                }
                header("Location: " . refPage("customers/customer_view&customer_id=$response"));
                exit();
            }
        }
    }
}
else {
    if ($_POST['mode'] == 'save') {
        $first_name = $_POST['first_name'];
        $last_name = $_POST['last_name'];
        $address_line1 = $_POST['address_line1'];
        $address_line2 = $_POST['address_line2'];
        $city_suburb = $_POST['city_suburb'];
        $state_province = $_POST['state_province'];
        $postcode_zip = $_POST['postcode_zip'];
        $country_code = $_POST['country_code'];
        $phone_home = $_POST['phone_home'];
        $phone_mobile = $_POST['phone_mobile'];
        $dob = $_POST['dob'];
        $gender = $_POST['gender'];
        $email = $_POST['email'];
        $sre_code = $_POST['sre_code'];
        $id_documents = $_POST['id_documents'];
        $nin_code = strtoupper(ereg_replace("[^A-Za-z0-9]", "", $_POST['nin_code']));
        $language = $_POST['language'];
        $nationality = $_POST['nationality'];
        $username = $_POST['username'];
        $password = $_POST['password'];
        $confirm_password = $_POST['confirm_password'];
        $pun_club_id = $_POST['jurisdiction'];
        $bc = $_POST['bc'];
        $profession = $_POST['profession'];
        $cob = $_POST['cob'];

        if (checkPersonalForm($username,
            $password,
            $confirm_password,
            $pun_club_id,
            $first_name,
            $last_name,
            $dob,
            $email,
            $address_line1,
            $city_suburb,
            $postcode_zip,
            $country_code,
            $phone_home,
            $nin_code,
            $nationality,
            $phone_mobile,
            $state_province,
            $cob,
            $bc
        ))
        {
            $password = md5($password);
            $sre_code = strtolower($sre_code);
            if ($phone_mobile) {
                if (substr($phone_mobile, 0, 1) == 0)
                    $phone_mobile = $cou_code_mobile . substr($phone_mobile, 1, strlen($phone_mobile));
                else
                    $phone_mobile = "$cou_code_mobile$phone_mobile";
            }
            $phone_mobile;

            if ($phone_home) {
                if ($area_code_home) {
                    if (substr($area_code_home, 0, 1) == 0)
                        $area_code_home = substr($area_code_home, 1, strlen($area_code_home));
                } else {
                    if (substr($phone_home, 0, 1) == 0)
                        $phone_home = substr($phone_home, 1, strlen($phone_home));
                }
                $phone_home = $cou_code_home . $area_code_home . $phone_home;
            }
            /*$dbh->begin();*/
            $preg_code = randomString(true);
            $email_reg_code = randomString(true);
            $glimit = '';
            $operationtype = 3015;
            $rtype = RTYPE;
            $skinid = SKINID;
            $_site = getSkinId($pun_club_id);
            $md5 = md5($skinid . $username . $first_name . $last_name . $dob . WEBAPP_KEY);
            if (RTYPE == 1) {
                $dataToSend = "ninec=$nin_code&psw=$password&county=$state_province&phone2=$phone_mobile&nationality=$nationality&username=$username&rules=yes&first=$first_name&last=$last_name&gender=$gender&date=$dob&address=$address_line1&city=$city_suburb&country=$country_code&phone=$phone_home&email=$email&18yrs=yes&zcode=$postcode_zip&md5=$md5&operationtype=$operationtype&skinid=$skinid&rtype=$rtype&ln=$language&glimit=$glimit&pun_club_id=$pun_club_id&fsite=$_site&activation=false";
                $response = sendToWebApp($dataToSend, WEB_APP_WEBSITES);
            } elseif (RTYPE == 2) {
                $dataToSend = "ninec=$nin_code&psw=$password&username=$username&rules=yes&first=$first_name&last=$last_name&gender=$gender&date=$dob&address=$address_line1&city=$city_suburb&country=$country_code&phone=$phone_home&email=$email&18yrs=yes&zcode=$postcode_zip&md5=$md5&operationtype=$operationtype&skinid=$skinid&rtype=$rtype&ln=$language&glimit=$glimit&pun_club_id=$pun_club_id&cob=$cob&bc=$bc&profession=$profession&address2=$address_line2&activation=false&fsite=$_site";
                $response = sendToWebApp($dataToSend, WEB_APP_WEBSITES);
            } else {
                $response = createCustomer($username,
                    $password,
                    $pun_club_id,
                    $first_name,
                    $last_name,
                    $dob,
                    $email,
                    $address_line1,
                    $city_suburb,
                    $postcode_zip,
                    $country_code,
                    $phone_home,
                    $nin_code,
                    $address_line2,
                    $gender,
                    $_site
                );
            }
            if ($response < 0) {
                include_once 'Error.class.inc';
                $error = new Error($response);
                addError('', $error->checkError());

            } else {
                if (!areErrors()) {
                    if ($response == '0') {
                        $sql = "select pun_id from punter where pun_username='$username' and pun_email='$email' AND pun_dob='" . $dob . "'";
                        $response = $dbh->queryOne($sql);
                        if (!$response || $response == NULL) {
                            sleep(1);
                            $response = $dbh->queryOne($sql);
                        }
                    }
                    header("Location: " . refPage("customers/customer_view&customer_id=$response"));
                    exit();
                }
            }


            /*  $id = createCustomer($username, $password, $first_name, $middle_name, $last_name,
                  $email, $address_line1, $address_line2, $city_suburb, $state_province, $country_code,
                  $postcode_zip, $phone_home, $phone_business, $phone_mobile, $fax,
                  $dob, $gender, $id_documents, $notes, $email_reg_code, $preg_code, $nin_code);

              createCustomerCreditRecord($id); */
            addPunterNote($id, $_SESSION['admin_id'], "User created");

            /*
            if ( !isBlank($phone_mobile)  )
            {  $text  = "Thanks for joining ".CASINO_FULL_NAME;
            $text .= ". Please go to ".$_SERVER['HTTP_HOST']."/preg.html";
            $text .= " and enter the code: $preg_code";
            $text .= "\n-- ".CASINO_FULL_NAME;


            $sms_sent = sendSms($phone_mobile, $text);

            if ( $sms_sent )
            {	$sql = "update punter set pun_confirmation_list = 'sms sent'";
            $sql .= " where pun_id = $id";
            $dbh->exec($sql);
            }
            }

            include 'pre_reg_message.inc';
            $message =  "Benvenuto su XXX il tuo"; //getMultiPartRegMessage($email_reg_code, $first_name, $email, $sms_sent);

            $email_sender    = new EmailFrom(EMAIL_REGISTRAR, CASINO_FULL_NAME." Registrar", 1, 'admin', EMAIL_NOREPLY, EMAIL_NOREPLY);
            $email_recipient = new EmailRecipient($email, "$first_name $last_name", $id, 'customer');

            $mail_sent = SendSingleEmail ( $email_sender, $email_recipient, CASINO_FULL_NAME.' - Registration Confirmation', $message['text'], $message['html'], $priority=2);

            if ( $mail_sent )
            {  if ( $sms_sent )
            $sql = "update punter set pun_confirmation_list = 'sms sent,email sent'";
            else
            $sql = "update punter set pun_confirmation_list = 'email sent'";
            $sql .= " where pun_id = $id";
            $dbh->exec($sql);
            }

            $dbh->commit();


            //refresh cache file
            $cache_filename = "internal_customers.".session_id();

            $_SESSION['message'] = "";
            //go to view mode for this newly created customer
            */

        }
    }
}
global $lang;
?>

<div  class=bodyHD2><?=$skin["skn_name"]?>: <?=$lang->getLang('Register a customer')?> 
        <?php if ($_SESSION['jurisdiction_class'] == 'club') {?>
        	(Limit: <?=$_SESSION['jur_users_limit'] ?>)
        <?}?></div>
<P >
    <? showErrors(); ?>
</P>

<div id="tabs">
    <ul>
        <li class="emptyError"><a href="#normal" <?($section=='0'?'active':'')?>>Normal</a></li>
        <?if($fastRegisterAccess) { ?>
        <li class="emptyError"><a href="#fast" <?($section=='1'?'active':'')?>>Fast</a></li>
        <? }?>
    </ul>
    <div id="normal">
        <?form_head(SELF, 'preg_form')?>
        <input type=hidden name=mode value="save">
        <input type=hidden name=was_viewing value="<?=getPostGetVar('was_viewing');?>">

        <input type="hidden" name="section" value="0">
        <p>
        <table border="0" cellspacing="0" cellpadding="0" width="630">
            <tr>
                <td>
                    <table border="0" cellspacing="1" cellpadding="4" width="460">
                        <tr valign="middle" align="left">
                            <td colspan=2 class="formheading"><?=$lang->getLang("Add a Customer")?></b></td>
                        </tr>
                        <tr valign="middle" align="left">
                            <td class=label><?=$lang->getLang("First Name")?>: <?php if(DOT_ID) : ?> <span style="color:black;font-size:16px">*</span> <?php endif; ?></td>
                            <td class=content><input type="text" name="first_name" size=40 maxlength=100 value="<?=replace_quotes($first_name); ?>"></td>
                        </tr>
                        <tr valign="middle" align="left">
                            <td class=label><?=$lang->getLang("Last Name")?>: <?php if(DOT_ID) : ?> <span style="color:black;font-size:16px">*</span> <?php endif; ?></td>
                            <td class=content><input type="text" name="last_name" size=40  value="<?=replace_quotes($last_name); ?>"></td>
                        </tr>
                        <tr valign="middle" align="left">
                            <td class=label><?=$lang->getLang("Gender")?> <?php if(DOT_ID) : ?> <span style="color:black;font-size:16px">*</span> <?php endif; ?></td>
                            <td class=content>
                                <select name="gender">
                                    <option value="male"><?=$lang->getLang("Male")?></option>
                                    <option value="female"><?=$lang->getLang("Female")?></option>
                                </select>
                            </td>
                        </tr>
                        <tr valign="middle" align="left">
                            <td class=label><?=$lang->getLang("Date of Birth (must be of legal age)")?>: </b>  <?php if(DOT_ID) : ?> <span style="color:black;font-size:16px">*</span> <?php endif; ?></td>
                            <td class=content>
                                <input type='text'  name='dob' id='dob' value='<?=$dob?>' />
                            </td>
                        </tr>
                        <?if (RTYPE==1) { ?>
                            <tr valign="middle" align="left">
                                <td class=label><?=$lang->getLang("Nationality")?>: </b>  <?php if(DOT_ID) :  ?> <span style="color:black;font-size:16px">*</span> <?php endif; ?></td>
                                <td class=content>
                                    <input name='nationality' value="<?=$nationality?>" />
                                </td>
                            </tr>
                        <? }?>
                        <tr valign="middle" align="left">
                            <TD class=label><?=$lang->getLang("Language")?>: <?php if(DOT_ID) : ?> <span style="color:black;font-size:16px">*</span> <?php endif; ?></td>
                            <td class=content>
                                <select name="language">
                                    <option value="en">En</option>
                                    <option value="it">It</option>
                                    <option value="ro">Ro</option>
                                </select>
                            </td>
                        </tr>
                        <tr valign="middle" align="left">
                            <td class=label><?=$lang->getLang("Email")?>: <?php if(DOT_ID) : ?> <span style="color:black;font-size:16px">*</span> <?php endif; ?></td>
                            <td class=content><input type="text" name="email" size=40 maxlength=100 value="<?=replace_quotes($email); ?>"></td>
                        </tr>
                        <tr valign="top" align="left">
                            <td class=label><?=$lang->getLang("Phone")?> <?if (RTYPE!=1) { ?> <span style="color:black;font-size:16px">*</span><? } ?></td>
                            <td class=content>
                                <input type="text" name="phone_home" size="20" maxlength=255 value="<?=replace_quotes($phone_home); ?>">
                            </td>
                        </tr>
                        <?if (RTYPE==1) { ?>
                            <tr valign="top" align="left">
                                <td class=label><?=$lang->getLang("Mobile Phone")?> <span style="color:black;font-size:16px">*</span></td>
                                <td class=content>
                                    <input type="text" name="phone_mobile" size="20" maxlength=255 value="<?=replace_quotes($phone_mobile); ?>">
                                </td>
                            </tr>

                            <tr valign="middle" align="left">
                                <TD class=label><?=$lang->getLang("Currency")?>: <?php if(DOT_ID) : ?> <span style="color:black;font-size:16px">*</span> <?php endif; ?></td>
                                <td class=content>
                                    <select name="language">
                                        <option value="en">Eur</option>
                                        <option value="it">Usd</option>
                                    </select>
                                </td>
                            </tr>
                        <? } ?>

                        <tr valign="middle" align="left">
                            <TD class=label><?=$lang->getLang("Address Line 1")?>: <?php if(DOT_ID) : ?> <span style="color:black;font-size:16px">*</span> <?php endif; ?></td>
                            <td class=content><input type="text" name="address_line1" size=40  value="<?=replace_quotes($address_line1); ?>"></td>
                        </tr>
                        <? if(RTYPE!=1){ ?>
                            <tr valign="middle" align="left">
                                <TD class=label><?=$lang->getLang("Address Line 2")?>:</td>
                                <td class=content><input type="text" name="address_line2" size=40  value="<?=replace_quotes($address_line2); ?>"></td>
                            </tr>
                        <? } ?>
                        <tr valign="middle" align="left">
                            <TD class=label><?=$lang->getLang("City or suburb")?> <?php if(DOT_ID) : ?> <span style="color:black;font-size:16px">*</span> <?php endif; ?></td>
                            <td class=content><input type="text" name="city_suburb" size=40  value="<?=replace_quotes($city_suburb); ?>"></td>
                        </tr>
                        <tr valign="middle" align="left">
                            <TD class=label><?=$lang->getLang("Postcode or Zipcode")?> <?php if(DOT_ID) : ?> <span style="color:black;font-size:16px">*</span> <?php endif; ?></td>
                            <td class=content><input type="text" name="postcode_zip" size=40  value="<?=replace_quotes($postcode_zip); ?>"></td>
                        </tr>
                        <?if (RTYPE==1) { ?>
                            <tr valign="middle" align="left">
                                <TD class=label><?=$lang->getLang("State or province")?> <?php if(DOT_ID) : ?> <span style="color:black;font-size:16px">*</span> <?php endif; ?></td>
                                <td class=content>
                                    <?php if(DOT_ID) : ?>
                                        <input type=text name="state_province" size=40  value="<?=replace_quotes($state_province); ?>">
                                    <?php else : ?>
                                        <input type=text name="state_province" size=40  value="<?=replace_quotes($state_province); ?>">
                                    <?php endif; ?>
                                </td>
                            </tr>
                        <? } ?>
                        <?if (RTYPE==2) { ?>
                            <tr valign="middle" align="left">
                                <TD class=label><?=$lang->getLang("Profession")?> <?php if(DOT_ID) : ?> <span style="color:black;font-size:16px">*</span> <?php endif; ?></td>
                                <td class=content>
                                    <select name="profession">
                                        <option value="Employee"><?=$lang->getLang("Employee")?></option>
                                        <option value="Laborer"><?=$lang->getLang("Laborer")?></option>
                                        <option value="Unemployed"><?=$lang->getLang("Unemployed")?></option>
                                        <option value="Self-employed"><?=$lang->getLang("Self-employed")?></option>
                                    </select>
                                    <input type='hidden' name='country_code' value='be' />
                                </td>
                            </tr>
                            <tr valign="middle" align="left">
                                <TD class=label><?=$lang->getLang("Country of birth")?> <?php if(!DOT_ID) : ?> <span style="color:black;font-size:16px">*</span> <?php endif; ?>
                                <td class=content>
                                    <? countrySelect('bc','' ,"$bc"); ?>
                                </td>
                            </tr>
                            <tr valign="middle" align="left">
                                <TD class=label><?=$lang->getLang("City of birth")?> <?php if(DOT_ID) : ?> <span style="color:black;font-size:16px">*</span> <?php endif; ?></td>
                                <td class=content><input type="text" name="cob" size=40  value="<?=replace_quotes($cob); ?>"></td>
                            </tr>


                        <? } ?>
                        <?if (RTYPE!=2) { ?>
                            <tr valign="middle" align="left">
                                <TD class=label><?=$lang->getLang("Country")?> <?php if(!DOT_ID) : ?> <span style="color:black;font-size:16px">*</span> <?php endif; ?>
                                <td class=content>
                                    <? countrySelect('country_code', "","$country_code"); ?>
                                </td>
                            </tr>
                        <? } ?>
                        <tr valign="middle" align="left">
                            <TD class=label><?=$lang->getLang("Club")?> <?php if(DOT_ID) : ?> <span style="color:black;font-size:16px">*</span> <?php endif; ?></td>
                            <td class=content> <?php
                                $result=getAllClubsUnderJurisdiction($_SESSION['jurisdiction_id']);
                                $clubs=array();
                                while($result->hasNext()){
                                    $row=$result->next();
                                    if(isset($row['district_name']) && !is_array($clubs[$row['district_name']])){
                                        $clubs[$row['district_name']]=array();
                                    }
                                    $clubs[$row['district_name']][$row['club_id']]=$row['club_name'];
                                }
                                ?>
                                <select name='jurisdiction' id='jurisdiction'>
                                    <?
                                    foreach($clubs as $k=>$v){
                                        ?>
                                        <optgroup label="<?=$k?>">
                                            <? foreach ($v as $k2=>$v2){
                                                ?>
                                                <option value='<?=$k2?>' <?if(isset($pun_club_id) && $pun_club_id==$k2){?> selected="selected" <? } ?>><?=$v2?></option>
                                                <?
                                            }
                                            ?>
                                        </optgroup>
                                        <?
                                    }
                                    ?>
                                </select>
                            </td>
                        </tr>
                        <tr valign="middle" align="left">
                            <td class=label><?=$lang->getLang("Skin")?>  <span style="color:black;font-size:16px"></span>
                            <td class=content>
                                <div id="skin">
                                </div>

                            </td>
                        </tr>
                        <?php if(DOT_ID) : ?>
                            <tr valign="middle" align="left">
                                <td class=label><?=$lang->getLang("National Insurance number")?> : <span style="color:black;font-size:16px">*</span></td>
                                <td class=content><input type="text" name="nin_code" size=40 maxlength=100 value="<?=replace_quotes($nin_code); ?>"></td>
                            </tr>
                        <?php endif; ?>
                        <tr valign="middle" align="left">
                            <TD class=label><?=$lang->getLang("Username")?> <?php if(DOT_ID) : ?> <span style="color:black;font-size:16px">*</span> <?php endif; ?></td>
                            <td class=content><input type="text" name="username" size=40 class="usrSrch"  value="<?=replace_quotes($username); ?>" ><span class="errorDisplay"></span></td>
                        </tr>
                        <tr valign="middle" align="left">
                            <TD class=label><?=$lang->getLang("Password")?> <?php if(DOT_ID) : ?> <span style="color:black;font-size:16px">*</span> <?php endif; ?></td>
                            <td class=content><input type="password" name="password" size=40  value=""></td>
                        </tr>
                        <tr valign="middle" align="left">
                            <TD class=label><?=$lang->getLang("Confirm password")?> <?php if(DOT_ID) : ?> <span style="color:black;font-size:16px">*</span> <?php endif; ?></td>
                            <td class=content><input type="password" name="confirm_password" size=40  value=""></td>
                        </tr>
                        <script language="javascript">
                            var phone_codes = new Array();
                            <?
                            global $lang;
                            $sql    = "SELECT cou_phone_code, cou_code FROM country WHERE cou_phone_code IS NOT NULL";
                            $rs     = $dbh->exec($sql);
                            $ip_arr = getIpDetails(getIpAddress());

                            while ( $rs->hasNext() ){
                              $row = $rs->next();
                              $cou_phone_code   = $row['cou_phone_code'];
                              $cou_code  = trim($row['cou_code']);

                              if (isset($cou_code_2_chars) && strtolower($cou_code_2_chars) && strtolower($cou_code_2_chars) == strtolower($ip_arr['country_code2']) ){
                                if(!array_key_exists('country_code', $_POST)){
                                  $cou_code_home     = $cou_phone_code;
                                  $cou_code_business = $cou_phone_code;
                                  $cou_code_mobile   = $cou_phone_code;
                                  $cou_code_fax      = $cou_phone_code;
                                }
                              }
                            }
                            ?>
                        </script>
                        <!--tr valign="middle" align="left">
        <TD class=label>ID documents:</td>
        <td class=content>
        	<input type="text" name="id_documents" value="<?=$id_documents;?>" size=40">
        	<input type="button" value="..." onClick="openIdWindow(this.form)">
        </td>
      </tr>
      <tr valign="top" align="left">
        <TD class=label>Notes</td>
        <td class=content>
          <textarea name="notes" rows=14 cols=44 wrap=hard><?=replace_quotes($notes);?></textarea>
        </td>
      </tr-->

                    </table>
                </td>
                <td width=30 valign=top align=left></td>
                <td width=200 valign=top align=left>
                    <input type="image" src="<?=image_dir?>/button_save.gif"><br>
                    <a href="<?=refpage('');?>">
                        <img src="<?=image_dir?>/button_cancel.gif" border=0>
                    </a>
                </td>
            </tr>
        </table>
        </p>
        </form>
    </div>
    <?if($fastRegisterAccess) { ?>
    <div id="fast">
        <?form_head(SELF, 'preg_form')?>
        <input type=hidden name=mode value="save">
        <input type=hidden name=was_viewing value="<?=getPostGetVar('was_viewing');?>">

        <input type="hidden" name="section" value="1">
        <p>
        <table border="0" cellspacing="0" cellpadding="0" width="630">
            <tr>
                <td>
                    <table border="0" cellspacing="1" cellpadding="4" width="460">
                        <tr valign="middle" align="left">
                            <td colspan=2 class="formheading"><?=$lang->getLang("Fast register")?></b></td>
                        </tr>
                        <tr valign="middle" align="left">
                            <TD class=label><?=$lang->getLang("Username")?> <?php if(DOT_ID) : ?> <span style="color:black;font-size:16px">*</span> <?php endif; ?></td>
                            <td class=content><input type="text" name="username" class="usrSrch" size=40  value="<?=replace_quotes($username); ?>" ><span class="errorDisplay"></span></td>
                        </tr>
                        <tr valign="middle" align="left">
                            <TD class=label><?=$lang->getLang("Password")?> <?php if(DOT_ID) : ?> <span style="color:black;font-size:16px">*</span> <?php endif; ?></td>
                            <td class=content><input type="password" name="password" size=40  value=""></td>
                        </tr>
                        <tr valign="middle" align="left">
                            <TD class=label><?=$lang->getLang("Confirm password")?> <?php if(DOT_ID) : ?> <span style="color:black;font-size:16px">*</span> <?php endif; ?></td>
                            <td class=content><input type="password" name="confirm_password" size=40  value=""></td>
                        </tr>
                        <tr valign="middle" align="left">
                            <TD class=label><?=$lang->getLang("Club")?> <?php if(DOT_ID) : ?> <span style="color:black;font-size:16px">*</span> <?php endif; ?></td>
                            <td class=content>
                                <select name='jurisdiction' id='jurisdictionFast'>
                                    <?
                                    foreach($clubs as $k=>$v){
                                        ?>
                                        <optgroup label="<?=$k?>">
                                            <? foreach ($v as $k2=>$v2){
                                                ?>
                                                <option value='<?=$k2?>' <?if(isset($pun_club_id) && $pun_club_id==$k2){?> selected="selected" <? } ?>><?=$v2?></option>
                                                <?
                                            }
                                            ?>
                                        </optgroup>
                                        <?
                                    }
                                    ?>
                                </select>
                            </td>
                        </tr>
                        <tr valign="middle" align="left">
                            <td class=label><?=$lang->getLang("Skin")?>  <span style="color:black;font-size:16px"></span>
                            <td class=content>
                                <div id="skinFast">
                                </div>

                            </td>
                        </tr>
                     </table>
                </td>
                <td width=30 valign=top align=left></td>
                <td width=200 valign=top align=left>
                    <input type="image" src="<?=image_dir?>/button_save.gif"><br>
                    <a href="<?=refpage('');?>">
                        <img src="<?=image_dir?>/button_cancel.gif" border=0>
                    </a>
                </td>
            </tr>
        </table>
        </p>
        </form>
    </div>
    <? } ?>

</div>


