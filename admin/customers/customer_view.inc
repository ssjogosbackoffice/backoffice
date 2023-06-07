<script type="text/javascript">
    $(document).ready(function() {
        var e = document.getElementById("profile");
        var strUser = e.options[e.selectedIndex].value;

        showProfileDetails(strUser);
        $( "#dob" ).datepicker({
            changeMonth: true,
            changeYear: true,
            numberOfMonths:1

        });
    });
    function showProfileDetails(sel)
    {
        $('.hidden').hide();
        $('#profile'+sel).show();
        return false;
    }
</script>
<?php check_access('customers_show',$doredirect=true);?>
<?php $usercanchangedaily=check_access('change_daily_user_limit',$redirect=false);
$userCanModify   = check_access('modify_customer_details');
$skin = getSkinDetailsByJurisdiction($_SESSION["jurisdiction_id"], $_SESSION["jurisdiction_class"]);

/**
 * @return boolean
 * @param int $dob_day
 * @param int $dob_month
 * @param int $dob_year
 * @param string $username
 * @param string $first_name
 * @param string $last_name
 * @param string $email
 * @param string $address1
 * @param string $city_suburb
 * @param string $country_code
 * @param string $identified
 * @param string $notes
 * @desc Checks the validity of the passed argumens.  Returns true if arguments ar valid, false otherwise
 */
define('RGX_EMAIL',    '#[a-z0-9!\#$%&\'*+/=?^_`{|}~-]+(?:\.[a-z0-9!\#$%&\'*+/=?^_`{|}~-]+)*@(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?#');
define('RGX_NAME',     '#^(?:[a-z]+[ ]?)+$#i');
define('RGX_USERNAME', '#^[a-z0-9]{1}(?:[a-z0-9]+[\. _-]?){1,10}[a-z0-9-_]{1}$#i');
define('RGX_PASSWORD', '#^[a-zA-Z0-9_\-\*\^\&\#\@\!\%\$]{6,66}$#');

function formValidated($dob,$username,$first_name,$last_name,$email,$address1,$city_suburb,
                       $country_code,$identified,$notes, $nin_code) {
    global $lang;
    $all_valid = true;
    $log = "";

    if ($_SESSION["jurisdiction_class"] != "casino" && $identified==1) {
        return $all_valid;
    }

    if($email && check_access('customers_modify_email')){
        return $all_valid;
    }
    else{
        if ( isBlank($dob)) {
            addError("dob", $lang->getLang("Invalid birthday"));
            $all_valid=false;
        }
        if ( isBlank($username) || (!preg_match(RGX_USERNAME, $username) && !preg_match(RGX_EMAIL, $username))) {
            addError("", $lang->getLang("A username must be entered"));
            $all_valid=false;
        }

        if ( isBlank($first_name) || !preg_match(RGX_NAME, $first_name) ) {
            addError("first_name", $lang->getLang("A first name must be entered"));
            $all_valid=false;
        }
        if ( isBlank($last_name) || !preg_match(RGX_NAME, $last_name) ) {
            addError("last_name", $lang->getLang("A last name must be entered"));
            $all_valid=false;
        }
        if ( isBlank($email) || !preg_match(RGX_EMAIL, $email) ) {
            addError("last_name", $lang->getLang("An email address must be entered"));
            $all_valid=false;
        }
        if ( isBlank($address1) ) {
            addError("address", $lang->getLang("An address (line 1) must be entered"));
            $all_valid=false;
        }
        if ( isBlank($city_suburb) ) {
            addError("suburb", $lang->getLang("A city or suburb must be entered"));
            $all_valid=false;
        }
        if ( isBlank($country_code) ) {
            addError("country_code", $lang->getLang("A country must be selected"));
            $all_valid=false;
        }

        if(DOT_IT){
            if(!checkNinCode($nin_code)){
                addError("nin_country_code", $lang->getLang("Invalid National Insurance Number."));
                $all_valid=false;
            }
        }

        /*
        if ( ($identified) ) {
          if ( isBlank($notes) ) {
            addError("", "Since this customer has been identified, notes must be entered ");
            $all_valid=false;
          }
        }
        */
    }
    return $all_valid;
}

/**
 * @return void
 * @param int    $customer_id
 * @param string $username
 * @param string $first_name
 * @param string $middle_name
 * @param string $last_name
 * @param string $email
 * @param string $address1
 * @param string $address2
 * @param string $address2
 * @param string $city_suburb
 * @param string $postcode_zip
 * @param string $state_province
 * @param string $country_code
 * @param string $phone_home
 * @param string $phone_business
 * @param string $phone_mobile
 * @param string $fax
 * @param string $notes
 * @param string $gender
 * @param string $dob
 * @param string $identifified
 * @param string $groups
 * @param string $orig_groups
 * @param string $access
 * @param string $investigate
 * @desc Updates a specific customer record and logs the changes to fields
 */

function isUserPresent($username,$customer_id,$email, $nin_code){
    global $dbh,$lang;
    $username = strtolower($username);
    $email    = strtolower($email);
    $nin_code = strtolower($nin_code);
    $q="SELECT pun_id, pun_username, pun_email, pun_nin_code FROM punter WHERE pun_id <> $customer_id AND (pun_username='$username' OR pun_email = '$email'";
    if($nin_code!=''){
        $q.=" OR pun_nin_code = '$nin_code')";
    }
    else{
        $q.=" )";
    }
    $rs = $dbh->exec($q);
    while($rs->hasNext()){
        $row = $rs->next();
        if($row["pun_username"] == $username){
            return $lang->getLang("The username entered already exists, please enter a different username");
        }elseif ($row["pun_email"] == $email){
            return $lang->getLang("That email address is already being used. Please use a different one.");
        }elseif (DOT_IT && strtolower($row["pun_nin_code"]) == $nin_code){
            return $lang->getLang("National Insurance number already used.");
        }
    }
    return;
}

function getLimits($limit){
    global $lang;
    $limits_str1='<table>';
    $limits_str='';
    global $userCanModify;
    if($limit!=''){
        $userlimit = explode('=', $limit );
        $limits=explode('~',$userlimit[1]);
        $usercanmodifylimit=$limits[0];
        for($i=1;$i<=3;$i++){
            list($limit_type,$max_limit,$available,$chosen,$sevenmod)=explode(":",$limits[$i]);

            if($limit_type=='d'){
                $limits_str .="<tr><td><b>".$lang->getLang("Daily limits").":</b></td> ";
                if($max_limit>0){
                    $limits_str.="<td><input type='text' name='dlimit' value=$max_limit /></td>";
                }
                else{
                    $limits_str.="<td><span style='color:red'>".$lang->getLang("Not configured yet")."</span></td>";
                }
                $limits_str.='</tr>';
            }
            elseif($limit_type=='w'){
                $limits_str .="<tr><td><b>".$lang->getLang('Weekly limits').":</b></td>";
                if($max_limit>0){
                    $limits_str.="<td><input type='text' name='wlimit' value=$max_limit /></td>";
                }
                else{
                    $limits_str.="<td><span style='color:red'>".$lang->getLang("Not configured yet")."</span></td>";
                }
                $limits_str.='</tr>';
            }
            elseif($limit_type=='m'){
                $limits_str .="<tr><td><b>".$lang->getLang("Monthly limits").":</b></td>";
                if($max_limit>0){
                    $limits_str.="<td><input type='text' name='mlimit' value=$max_limit /></td>";
                }
                else{
                    $limits_str.="<td><span style='color:red'>".$lang->getLang("Not configured yet")."</span></td>";
                }
                $limits_str.="</tr>";
            }
        }
    }

    $limits_str2="<tr><td>".$lang->getLang("User can update limit?")."</td><td>";
    if($userCanModify){
        $limits_str2.="<select name='user_limit'>
                        <option  value='0'>".$lang->getLang("YES")."</option>
                                <option value='1'";
        if($usercanmodifylimit==1) {
            $limits_str2.=" selected='selected' ";
        }
        $limits_str2 .=" >".$lang->getLang("NO")."</option>
                                </select> ";
    }
    else{
        if($usercanmodifylimit==1) {
            $limits_str2 .="No";
        }
        else{
            $limits_str2 .="Yes";
        }
    }
    $limits_str2 .= "</td></tr>";
    $limits_str1.=$limits_str2;
    $limits_str1.=$limits_str;
    $limits_str1.='</table>';
    return $limits_str1;
}


function prepareLimits($limit){
    $limitstosave='';
    if($limit!=''){
        list($lmt,$currentlimits)=explode('=',$limit);
        $limits=explode('~',$currentlimits);
        if(isset($_POST['user_limit'])){
            $limits[0]=$_POST['user_limit'];
        }

        for($i=1;$i<=3;$i++){
            //0~d:30:10:40:1~w:400:390:0:0~m:700:690:0:0
            $currlimit=explode(":",$limits[$i]);
            if($currlimit[0]=='d'){
                if(isset($_POST['dlimit']) && ($_POST['dlimit']!=$currlimit[1])){
                    $currlimit[1]=$_POST['dlimit'];
                    unset($currlimit[2],$currlimit[3],$currlimit[4],$currlimit[5]);
                }
                if(isset($_POST['dlimit'])&&$_POST['dlimit']==''){
                    unset($limits[$i]);
                }
                else{
                    $limits[$i]=implode(':',$currlimit);
                }
            }
            elseif($currlimit[0]=='w'){
                if(isset($_POST['wlimit']) && ($_POST['wlimit']!=$currlimit[1])){
                    $currlimit[1]=$_POST['wlimit'];
                    unset($currlimit[2],$currlimit[3],$currlimit[4],$currlimit[5]);
                }
                if(isset($_POST['wlimit'])&&$_POST['wlimit']==''){
                    unset($limits[$i]);
                }
                else{
                    $limits[$i]=implode(':',$currlimit);
                }
            }
            elseif($currlimit[0]=='m'){
                if(isset($_POST['mlimit']) && ($_POST['mlimit']!=$currlimit[1])){
                    $currlimit[1]=$_POST['mlimit'];
                    unset($currlimit[2],$currlimit[3],$currlimit[4],$currlimit[5]);
                }
                if(isset($_POST['mlimit'])&&$_POST['mlimit']==''){
                    unset($limits[$i]);
                }
                else{
                    $limits[$i]=implode(':',$currlimit);
                }
            }
        }
        $limitstosave=$lmt."=".implode('~',$limits);
    }
    return $limitstosave;

}
function updateCustomer($customer_id,$username,$first_name,$middle_name,$last_name,$email,$address1,$address2,
                        $address2,$city_suburb,$postcode_zip,$state_province,$country_code,$phone_home,$phone_business,
                        $phone_mobile,$fax,$profile,$daily_limit,$notes,$gender,$dob,$identified, $groups, $orig_groups, $access="allow",
                        $investigate="NULL", $nin_code,$user_limit) {
    global $dbh,$lang,$userCanModify;  //global database object,
    $prev_vals = getCustomer($customer_id);  //get values of previous fields for customer record


    if ($_SESSION["jurisdiction_class"] != "casino") {
        if (strtolower($prev_vals["access"]) != "allow" || ($prev_vals["identified"] > 0)) {
            if(strtolower($prev_vals["access"]) != "allow") {
                if ($email  && ($email!=$prev_vals['email'])) {
                    $log = "Email limits changed from ".$prev_vals['email']." to $email ";
                    doAdminUserLog($_SESSION['admin_id'], 'modify_email', escapeSingleQuotes($log), $customer_id);
                    return $dbh->exec("Update punter set pun_email=" . db_quote($email) . " WHERE pun_id = $customer_id");
                } elsE {
                    printInfoMessage($lang->getLang("Cannot modify,this user is blocked."));
                }
            }
            else{
                $dbh->begin();
                $sql = "UPDATE punter" .
                    " SET     pun_betting_type           = ".db_quote($profile)          .
                    ",   pun_daily_deposits_limit   = ".db_quote($daily_limit) 		.
                    ",   pun_notes                  = ".db_quote($notes) 	.
                    ",   pun_preferences            = ".db_quote($user_limit).
                    "  WHERE pun_id = $customer_id";

                $rs = $dbh->exec($sql);
                if($daily_limit !=$prev_vals['daily_deposits_limit']){
                    $log .= "Daily limits changed from ".$prev_vals['daily_deposits_limit']." to $daily_limit <br>";
                }
                if ( $user_limit != $prev_vals['pun_preferences']){
                    $limitschanged='true';
                    $log .= "Limits changed from ".$prev_vals['pun_preferences']." to $user_limit <br>";
                }
                if ( $log ) {
                    if ( substr($log, -1) == '|' )
                        $log = substr($log, 0, strlen($log)-1); //get rid of last delimiter
                    //write log string
                    doAdminUserLog($_SESSION['admin_id'], 'modify_customer', escapeSingleQuotes($log), $customer_id);
                }
                $dbh->commit();
                if(isset($limitschanged)){
                    $datatoSend="operationcode=2003&uid=$customer_id&glimit=$user_limit";
                    sendToWebApp($datatoSend,WEB_APP_PATH);
                }
                return $rs;
//                printInfoMessage($lang->getLang("Cannot modify user, he was already identified."));
            }
            return false;
        }
    }

    $dbh->begin();  //begin update/log transaction
    if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
        betting_begin();
    }
    //define sql for record update
    if($email && check_access('customers_modify_email') && !$userCanModify){
        $sql="Update punter set pun_email=" .db_quote($email)." WHERE pun_id = $customer_id";
    }
    else{

        $sql = "UPDATE punter" .
            " SET pun_username              = ".db_quote($username) 		.
            ",   pun_first_name             = ".db_quote($first_name) 		.
            ",   pun_middle_name            = ".db_quote($middle_name) 		.
            ",   pun_last_name              = ".db_quote($last_name) 		.
            ",   pun_email                  = ".db_quote($email) 			.
            ",   pun_address_line1          = ".db_quote($address1) 		.
            ",   pun_address_line2          = ".db_quote($address2) 		.
            ",   pun_city_suburb            = ".db_quote($city_suburb) 		.
            ",   pun_postcode_zip           = ".db_quote($postcode_zip) 	.
            ",   pun_state_province         = ".db_quote($state_province) 	.
            ",   pun_cou_code               = ".db_quote($country_code) 	.
            ",   pun_phone_home             = ".db_quote($phone_home) 		.
            ",   pun_phone_business         = ".db_quote($phone_business) 	.
            ",   pun_phone_mobile           = ".db_quote($phone_mobile) 	.
            ",   pun_fax                    = ".db_quote($fax) .
            ",   pun_betting_type           = ".db_quote($profile)          .
            ",   pun_daily_deposits_limit   = ".db_quote($daily_limit) 		.
            ",   pun_notes                  = ".db_quote($notes) 			.
            ",   pun_gender                 = ".( $gender? db_quote($gender) : 'NULL') .
            ",   pun_dob                    = ".( $dob? db_quote($dob) : 'NULL') .
            ",   pun_identified             = $identified" .
            ",   pun_preferences            = ".db_quote($user_limit) 	.
            ",   pun_nin_code               = " . db_quote($nin_code);

        if ( 'allow' == strtolower($access) ) {
            $sql .= ", pun_num_failed_logins = 0";
        }

        $sql .= ",   pun_investigate    = $investigate" .
            "  WHERE pun_id = $customer_id";
    }

    $rs = $dbh->exec($sql); //execute sql update
    if($rs !== 1){
        addError("", "No modify done");
        $dbh->rollback();
        if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
            betting_rollback();
        }
        return;
    }

    if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
        $result = betting_update_user($profile, $customer_id);
        if(!$result){
            addError("", "Database error");
            $dbh->rollback();
            betting_rollback();
            return;
        }
    }

    ////START SECTION: Do Logging

    ///If fields have changed, ensure the changes are loggged
    $log='';
    if ( $username != $prev_vals['username'] )
        $log .= "username:".$prev_vals['username']."=".$_POST['username'].'|';

    if ( $do_password_reset )
        if ( md5($_POST['password']) != $prev_vals['password'] )
            $log .= 'password:set|';

    if ( $first_name != $prev_vals['first_name'] )
        $log .= " First_name changed from ".$prev_vals['first_name']." to $first_name <br>";
    if ( $middle_name != $prev_vals['middle_name'] )
        $log .= " Middle name changed from ".$prev_vals['middle_name']." to $middle_name <br>";
    if ( $last_name != $prev_vals['last_name'] )
        $log .= " Last name changed from ".$prev_vals['last_name']." to $last_name <br>";
    if ( $email != $prev_vals['email'] )
        $log .= " Email changed from ".$prev_vals['email']." to $email <br>";
    if ( $address1 != $prev_vals['address1'] )
        $log .= " Address1 changed from ".$prev_vals['address1']." to $address1 <br>";
    if ( $address2 != $prev_vals['address2'] )
        $log .= " Address2 changed from ".$prev_vals['address2']." to $address2 <br>";
    if ( $city_suburb != $prev_vals['city_suburb'] )
        $log .= " City suburb changed from ".$prev_vals['city_suburb']." to $city_suburb <br>";
    if ( $postcode_zip != $prev_vals['postcode_zip'] )
        $log .= " Postcode zip changed from ".$prev_vals['postcode_zip']." to $postcode_zip <br>";
    if ( $state_province != $prev_vals['state_province'] )
        $log .= " State province changed from ".$prev_vals['state_province']." to $state_province <br>";
    if ( $country_code != $prev_vals['country_code'] )
        $log .= " Country changed from ".$prev_vals['country_code']." to $country_code <br>";
    if ( $phone_home != $prev_vals['phone_home'] )
        $log .= " Phone home changed from ".$prev_vals['phone_home']." to $phone_home <br>";
    if ( $phone_business != $prev_vals['phone_business'] )
        $log .= "Phone business changed from ".$prev_vals['phone_business']." to $phone_business <br>";
    if ( $phone_mobile != $prev_vals['phone_mobile'] )
        $log .= "Phone mobile changed from ".$prev_vals['phone_mobile']." to $phone_mobile <br>";
    if ( $fax != $prev_vals['fax'] )
        $log .= "Fax changed from ".$prev_vals['fax']." to $fax <br>";
    if ( $dob != $prev_vals['dob'] )
        $log .= "Birthday changed from ".$prev_vals['dob']." to ".$dob." <br>";
    if ( $identified != $prev_vals['identified'] )
        $log .= "Identified changed from ".$prev_vals['identified']." to $identified <br>";
    if ( $notes != $prev_vals['notes'] )
        $log .= "Notes changed from ".$prev_vals['notes']." to $notes <br>";
    if ( $gender != $prev_vals['gender'] )
        $log .= "Gender changed from ".$prev_vals['gender']." to $gender <br>";
    if ( $investigate != $prev_vals['investigate'] )
        $log .= "Investigate changed from ".$prev_vals['investigate']." to $investigate <br>";
    if ( $access != $prev_vals['access'] )
        $log .= "Access changed from ".$prev_vals['access']." to $access <br>";

    $admins=$dbh->exec("SELECT aus_id FROM admin_user where aus_jurisdiction_id=1 ");
    if($daily_limit !=$prev_vals['daily_deposits_limit']){
        $log .= "Daily limits changed from ".$prev_vals['daily_deposits_limit']." to $daily_limit <br>";

        $sql="INSERT INTO admin_user_mailbox VALUES ";
        while($admins->hasNext()) {
            $admin = $admins->next();
            $msg_id = $dbh->nextSequence("AUM_ID_SEQ");

            $sql.="($msg_id, ".$_SESSION['admin_id'].", ".$admin['aus_id'].", 'User limits changed', 'Daily limits for user ".$prev_vals['username']." changed from ".$prev_vals['daily_deposits_limit']." to $daily_limit ', '" . $dbh->escape(getIpAddress()) . "', '0', CURRENT_TIMESTAMP, NULL,0),";
        }
        $sql=substr($sql,0,-1);
        $dbh->exec($sql);
    }
    if($profile!=$prev_vals['betting_type']){

        $profiles=getCustomerProfiles();
        $saveString='Betting profile for user '.$prev_vals['username'].' changed from ';
        while ($profiles->hasNext()) {
            $p = $profiles->next();
            if($prev_vals['betting_type']==$p["pft_id"]){
                $saveStringPrev=$p['pft_name']. " to ";
            }
            if($profile==$p["pft_id"]){
                $saveStringAfter=$p['pft_name'];

            }
        }
        $saveString.=$saveStringPrev.=$saveStringAfter;
        $admins->resetRecPtr();
        $sql="INSERT INTO admin_user_mailbox VALUES ";
        while($admins->hasNext()) {
            $admin = $admins->next();
            $msg_id = $dbh->nextSequence("AUM_ID_SEQ");

            $sql.="($msg_id, ".$_SESSION['admin_id'].", ".$admin['aus_id'].", 'User betting profile changed', ".$dbh->prepareString($saveString).", '" . $dbh->escape(getIpAddress()) . "', '0', CURRENT_TIMESTAMP, NULL,0),";
        }
        $sql=substr($sql,0,-1);
        $dbh->exec($sql);


    }

    if ( $user_limit != $prev_vals['pun_preferences']){
        $limitschanged='true';
        $log .= "Limits changed from ".$prev_vals['pun_preferences']." to $user_limit <br>";
    }

    if ( $log ) {
        if ( substr($log, -1) == '|' )
            $log = substr($log, 0, strlen($log)-1); //get rid of last delimiter
        //write log string
        doAdminUserLog($_SESSION['admin_id'], 'modify_customer', escapeSingleQuotes($log), $customer_id);
    }
    ////END SECTION: Do Logging
    //////////////////////////////////////////////////////////
    $dbh->commit();  //commit customer record update
    if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
        betting_commit();
    }

    if(isset($limitschanged)){
        $datatoSend="operationcode=2003&uid=$customer_id&glimit=$user_limit";
        sendToWebApp($datatoSend,WEB_APP_PATH);
    }
    return $rs;
}

globalize('op'); //operation to be performed

$customer_id = (int)globalise('customer_id'); //current customer
if ( ! $customer_id ) //if customer_id is 0, kill script (invalid customer id)
    dieWithError($lang->getLang("Customer not found under your jurisdiction!"));

$customer_row	= getPunter($customer_id); //get all fields of customer record
$symbol=$customer_row['code_for_web'];
if($customer_row['cur_id']!='1'){
    $currencyConversion=($_SESSION['currencies'][$customer_row['cur_id']]['value']);
}
else{
    $currencyConversion=1;
}


if ( ! $customer_row )
    dieWithError($lang->getLang("Customer not found under your jurisdiction!"));

//get original group membership of current customer
$orig_groups = array();
/*
$sql  = "select pcg_cgr_code from punter_cgroup";
$sql .= " where pcg_pun_id = $customer_id";
$rs  = $dbh->exec($sql);
$num_rows = $rs->getNumRows();

$i=0;
if ( $num_rows >  0 )
{   while ( $rs->hasNext() )
{  $row = $rs->next();
$grp_code = $row['pcg_cgr_code'];
$orig_groups[$i++] = $grp_code;
}
}
*/


globalise('form_submitted');
globalise('action');
globalise('doc');

if($action == "load"){
    if(empty($_FILES[$doc]["name"])){
        addError("", $lang->getLang("File not received"));
    }
    if(!areErrors()){
        $file_list = glob(CUSTOMERS_DOCS_PATH . "/" . getUploadedDocFileName($customer_id, $doc) . "*");
        foreach ($file_list as $file_to_remove){
            unlink($file_to_remove);
        }

        if(eregi("image", $_FILES[$doc]["type"]) && filesize($_FILES[$doc]["tmp_name"]) < 1024 * 1024){
            saveDocumentImage($customer_id, $_FILES[$doc]["tmp_name"], $doc);
        }else{
            eregi(".*(\.[a-zA-Z0-9]{3,4})", $_FILES[$doc]["name"], $match);
            move_uploaded_file($_FILES[$doc]["tmp_name"], CUSTOMERS_DOCS_PATH . "/" . getUploadedDocFileName($customer_id, $doc . $match[1]));
        }
    }
}


$userCanIdentify = (($customer_row["identified"] != 1 && $_SESSION["jurisdiction_class"] == "club")   || $_SESSION["jurisdiction_class"] == "casino");
$user_limit     = $customer_row['preferences'];

if ( $form_submitted ) {
    $user_limit     = prepareLimits($customer_row['preferences']);

    $identified     = (($userCanIdentify && $_POST['identified'])? 1:-1);
    if ( 'reset_password' == $op ) {
        $username       = $_POST['username'];
        $first_name     = $_POST['first_name'];
        $middle_name    = $_POST['middle_name'];
        $last_name      = $_POST['last_name'];
        $email          = $_POST['email'];
        $dob            = $_POST['dob'];
        $gender         = $_POST['gender'];
        $address1      	= $_POST['address1'];
        $address2      	= $_POST['address2'];
        $city_suburb    = $_POST['city_suburb'];
        $state_province = $_POST['state_province'];
        $postcode_zip   = $_POST['postcode_zip'];
        $country_code  	= $_POST['country_code'];
        $phone_business = $_POST['phone_business'];
        $phone_home     = $_POST['phone_home'];
        $phone_mobile   = $_POST['phone_mobile'];
        $fax            = $_POST['fax'];
        $profile        = $_POST['profile'];
        $daily_limit    = round($_POST['daily_limit']/$currencyConversion);
        $notes          = $_POST['notes'];
        $investigate    = ( $_POST['investigate'] ? 1 : 0 );
        $access         = ( $_POST['deny_access']  ? 'deny' : 'allow' );
        $nin_code       = strtoupper(ereg_replace("[^A-Za-z0-9]", "", $_POST['nin_code']));
        $partnerId      = $_POST['partner_user_id'];
    }
    elseif ( 'update' == $op ) {  //If update is the operation, start processing
        if($_POST['email'] && check_access('customers_modify_email') && !$userCanModify){
            $username       = $_POST['username'];
            $email          = $_POST['email'];
            $first_name     = $customer_row['first_name'];
            $middle_name    = $customer_row['middle_name'];
            $last_name      = $customer_row['last_name'];
            $gender         = $customer_row['gender'];
            $address1       = $customer_row['address1'];
            $address2       = $customer_row['address2'];
            $city_suburb    = $customer_row['city_suburb'];
            $state_province = $customer_row['state_province'];
            $postcode_zip   = $customer_row['postcode_zip'];
            $country_code   = $customer_row['country_code'];
            $phone_business = $customer_row['phone_business'];
            $phone_home     = $customer_row['phone_home'];
            $phone_mobile   = $customer_row['phone_mobile'];
            $fax            = $customer_row['fax'];
            $identified     = $customer_row['identified'];
            $profile        = $customer_row['betting_type'];
            $daily_limit    = round($customer_row['daily_deposits_limit']);
            $notes          = $customer_row['notes'];
            $investigate    = $customer_row['investigate'];
            $access         = $customer_row['access'];
            $notes          = $customer_row['notes'];
            $dob            = $customer_row['dob'];
            $nin_code       = $customer_row['nin_code'];
            $partnerId      = $customer_row['partner_user_id'];




            //split date of birth into comonents

            $full_name  = $customer_row['full_name'];
        }
        else{
            $username       = $_POST['username'];
            $first_name     = $_POST['first_name'];
            $middle_name    = $_POST['middle_name'];
            $last_name      = $_POST['last_name'];
            $email          = $_POST['email'];
            $dob            = $_POST['dob'];
            $gender         = $_POST['gender'];
            $address1      	= $_POST['address1'];
            $address2  	    = $_POST['address2'];
            $city_suburb    = $_POST['city_suburb'];
            $state_province = $_POST['state_province'];
            $postcode_zip   = $_POST['postcode_zip'];
            $country_code	= $_POST['country_code'];
            $phone_business = $_POST['phone_business'];
            $phone_home     = $_POST['phone_home'];
            $phone_mobile   = $_POST['phone_mobile'];
            $fax            = $_POST['fax'];
            $profile        = $_POST['profile'];
            $daily_limit    = round($_POST['daily_limit']/$currencyConversion);
            $notes          = $_POST['notes'];
            $investigate    = ( $_POST['investigate'] ? 1 : 0 );
            $access         = ( $_POST['deny_access']  ? 'deny' : 'allow' );
            $partnerId      = $_POST['partner_user_id'];


            //get database date string from date components

            $nin_code       = strtoupper(ereg_replace("[^A-Za-z0-9]", "", $_POST['nin_code']));

            $full_name      = concatNames($first_name, $middle_name, $last_name);
            $groups         = $_POST['groups'];
        }
        if ( ! isRepeatSubmit('scode') ) { //if submit code has a value and is the same as the session submit code
            //we don't perform an operation (because this suggest a resend/refresh)

            if ( formValidated($dob,$username,$first_name,$last_name,$email,$address1,$city_suburb,$country_code,$customer_row['identified'],$notes,$nin_code)) {
                $userPresent = isUserPresent($username, $customer_id, $email, $nin_code);

                if(empty($userPresent)) {
                    $upd=updateCustomer( $customer_id,$username,$first_name,
                        $middle_name,$last_name,$email,
                        $address1,$address2, $address2,
                        $city_suburb,$postcode_zip,
                        $state_province,$country_code,$phone_home,
                        $phone_business, $phone_mobile,$fax,$profile,$daily_limit,
                        $notes,$gender,$dob,$identified,
                        $groups, $orig_groups, $access,$investigate, $nin_code,$user_limit);
                    if(!areErrors()&& $upd){
                        $msg = $lang->getLang("Details updated successfully");
                    }else{
                        $msg = $lang->getLang("No modify was done!");
                    }
                }
                else {
                    $msg = $userPresent;
                }
                //we don't reset the password of unactivated pre-registered accounts (i.e. customer ahs to choose password)
                if ( $_POST['set_pwd']  && !( $customer_row['pre_registered'] && $customer_row['registration_status'] != 'activated') ) {
                    $msg .= resetPassword($customer_row);
                }
                jalert($msg);
                unset($_POST['set_pwd']);

                $customer_row = getCustomer($customer_id); //get details of customer after update
            }
        }
    }
}
else { //if no operation, get customer fields from database
    $username       = $customer_row['username'];
    $first_name     = $customer_row['first_name'];
    $middle_name    = $customer_row['middle_name'];
    $last_name      = $customer_row['last_name'];
    $email          = $customer_row['email'];
    $gender         = $customer_row['gender'];
    $address1       = $customer_row['address1'];
    $address2       = $customer_row['address2'];
    $city_suburb    = $customer_row['city_suburb'];
    $state_province = $customer_row['state_province'];
    $postcode_zip   = $customer_row['postcode_zip'];
    $country_code   = $customer_row['country_code'];
    $phone_business = $customer_row['phone_business'];
    $phone_home     = $customer_row['phone_home'];
    $phone_mobile   = $customer_row['phone_mobile'];
    $fax            = $customer_row['fax'];
    $identified     = $customer_row['identified'];
    $profile        = $customer_row['betting_type'];
    $daily_limit    = $customer_row['daily_deposits_limit'];
    $notes          = $customer_row['notes'];
    $investigate    = $customer_row['investigate'];
    $access         = $customer_row['access'];
    $notes          = $customer_row['notes'];
    $dob            = $customer_row['dob'];
    $nin_code       = $customer_row['nin_code'];
    $partnerId      = $customer_row['partner_user_id'];

    //split date of birth into comonents

    $full_name  = $customer_row['full_name'];

}

////////////////////////////////////////////////////////////
////START SECTION: get unmodifiable fields
$customer_number   = $customer_row['customer_number'];
$reg_date  	       = $customer_row['reg_date'];
$access            = $customer_row['access'];
$last_logged_in    = $customer_row['last_logged_in'];
$num_logins        = ( $customer_row['num_logins'] ? $customer_row['num_logins'] : 0 );
$reg_status        = $customer_row['registration_status'];
$credits           = $customer_row['credits'];

$total_bets        = $customer_row['total_bets'];
$total_wins 	     = $customer_row['total_wins'];

$total_deposits    = $customer_row['total_deposits'];
$total_withdrawals = $customer_row['total_withdrawals'];
$aff_id            = $customer_row['aff_id'];
$member_type       = $customer_row['member_type'];
$pre_registered    = $customer_row['pre_registered'];
$preg_code         = $customer_row['preg_code'];
$betting_club      = $customer_row['betting club'];
$district          = $customer_row['district'];
$region            = $customer_row['region'];
$nation            = $customer_row['nation'];
$dplimit           = $customer_row['daily_deposits_limit'];
$login_lock        = $customer_row['login_lock_start'];
$url               = $customer_row['url'];
$connectionType    = $customer_row['connection_type'];
if((!$url) || ($url=='0')) {
    if(!$url && $customer_row['site_id']=='-1'){
        $url= $lang->getLang('None');
    }
    else {
        $url = $lang->getLang('All');
    }
}
if($aff_id!=''){
    $affiliateCredits=$dbh->queryRow('SELECT * from affiliate_credit where act_aff_id='.$dbh->escape($aff_id));
}

$rake_sql = "SELECT SUM(cus_res_rake) cus_res_rake FROM customer_result_daily WHERE cus_res_pun_id = $customer_id";
$rs       = $dbh->exec($rake_sql);
$row      = $rs->next();
$cus_res_rake = $row["cus_res_rake"];
unset($rake_sql, $rs, $row);

if ($customer_row['full_name'] == " ")
    $customer_row['full_name'] = $customer_row['username'];

//get and format login fields
$ts_last_login   = dbtstophpts($last_logged_in); //last login as php timestamp
$ts_reg_date     = dbtstophpts($reg_date);  //registration date as php timestamp
if ( ! $ts_last_login || $ts_last_login < $ts_reg_date )  //if never logged in or last login is less than reg date (default for last login is the epoch when registering)
    $last_login = "(Never)";
else
    $last_login = shortDate($customer_row['last_logged_in'],true);

//get betting club details
//$betting_club_name = ( $customer_row['betting_club'] ? getClubName($customer_row['betting_club']) : "(Not Set)" );

//get membership details and display accordingly
if ( 'FINANCIAL' == $member_type )
    $member_type_disp = "<img src=\"".image_dir."/icon_financial.gif\" alt=\"Financial\"> ".$lang->getLang('FINANCIAL');
elseif ( 'TOTEM' == $member_type )
    $member_type_disp = "<img src=\"".image_dir."/icon_server.gif\" alt=\"Totem\">".$lang->getLang('TOTEM');
elseif ( 'INTERNET' == $member_type )
    $member_type_disp = "<img src=\"".image_dir."/icon_financial.gif\" alt=\"Internet\">".$lang->getLang('INTERNET');
else
    $member_type_disp = "<img src=\"".image_dir."/icon_nonfinancial.gif\" alt=\"Nonfinancial\">".$lang->getLang('NON FINANCIAL');

if ( -1 == $identified )
    $member_type_disp .= "<br /><img src=\"".image_dir."/icon_transient.gif\" alt=\"Clienti Transitori\"> Clienti Transitori";

if ( 'pending' == $customer_row['registration_status'] )
    $pun_reg_status = "<img src=\"".image_dir."/icon_waiting.gif\" alt=\"Activation Pending\"> <span style=\"color:red\">".$lang->getLang('Waiting confirmation')."</span>";
elseif ( 'activated' == $customer_row['registration_status'] )
    $pun_reg_status = "<img src=\"".image_dir."/icon_activated.gif\" alt=\"Account activated\"> <span style=\"color:blue\">".$lang->getLang('Active')."</span>";

////END SECTION: Get unmodifiable fields
///////////////////////////////////////////////////////////

?>

<script type="text/javascript">
    function refreshForm() {
        if ( window.confirm("Refreshing will cause any unsaved changes to be lost.Are you sure you wish to continue?") )
            window.location = '<?=refPage("$page&customer_id=$customer_id");?>';
    }

    function saveChanges() {
        if(window.confirm("<?=$lang->getLang('Save changes?')?>")){
            document.forms.customer_admin.submit();
        }
    }

    function setPasswordEnabled (setpwd) {
        var form = document.forms.customer_admin;

        if ( setpwd ) {
            form.password.disabled = false;
            form.password_confirm.disabled = false;
        }
        else {
            form.password.disabled = true;
            form.password_confirm.disabled = true;
        }
    }
</script>


<div class=bodyHD><?=$lang->getLang("View Customers")?> <b><?=$customer_row['username'];?></b></div>
<div class="bodyHD2"><?=$customer_row['full_name'];?> - <?=$customer_row['customer_number']?> (<?=$customer_row['member_type']?>)</div>
<div class="bodyHD2"><?=$lang->getLang("Player Details")?></div>
<div><?=(($userCanModify)?(printLink(refpage("customers/search"), $lang->getLang("Back to Main menu"))):(""))?>

    <!--
<strong>|</strong>
<a href="<?='/customers/reports/account_statement.php?customer_id='.$customer_id?>">Account Statement</a>
-->
    <!--
<strong>|</strong>

<a href="<?=refPage("games/stats&customer_number=$customer_number")?>">Ricerca nell'attività di Gioco </a>
-->
    <?if($userCanModify){ ?>
        <?php if ( ! ($pre_registered && $reg_status != 'activated') ) {
            if(check_access('customers_resend_password')){
                ?>
                <strong>|</strong>

                <a href="javascript: if(confirm('<?=$lang->getLang("Reset Password")?>?')) { document.location = '<?="/customers/reset_password.php?customer_id=" . $customer_id;?>';}"><?=$lang->getLang("Reset Password")?></a>
            <?php } } ?>


        <?php
        if ( 'allow' == strtolower($customer_row['access'] )) {
            if(check_access('customer_lock')){

                ?>
                <strong>|</strong>
                <a href="javascript: jConfirm('<?=$lang->getLang("Lock Account")?>?','Please Confirm', function(r) {
            if (r){
            jPrompt('Please enter the note:','','Note', function(rsp) {
            if(rsp)
                {
                    document.location = '<?="/customers/account_status.php?op=lock&customer_id=" . $customer_id."&note='+rsp;";?>
                }
            else{
                    jAlert('<?=$lang->getLang('Insert note')?>');
                }
                });
            }

            });
             "><?=$lang->getLang("Lock Account")?></a>
                <?php
            }
        }
        else {
            if(check_access('customer_unlock')){
                ?>
                <strong>|</strong>

                <a href="javascript: jConfirm('<?=$lang->getLang("Unlock Account")?>?','Please Confirm', function(r) {
            if (r){
            jPrompt('Please enter the note:','','Note', function(rsp) {
            if(rsp)
                {
                    document.location = '<?="/customers/account_status.php?op=unlock&customer_id=" . $customer_id."&note='+rsp;";?>
                }
            else{
                jAlert('<?=$lang->getLang('Insert note')?>');
                }
                });
            }

            });
             ">
                    <?=$lang->getLang("Unlock Account")?></a>
                <?php
            }
        }
        ?>
        <strong>|</strong>
        <?php if ( $customer_row['investigate'] )  { ?>
            <a href="javascript: if(confirm('<?=$lang->getLang("Clear Investigate Flag")?>?')) { document.location = '<?="/customers/account_status.php?op=investigate_off&customer_id=" . $customer_id;?>';}"><?=$lang->getLang("Clear Investigate Flag")?></a>
        <?php } else { ?>
            <a href="javascript: if(confirm('<?=$lang->getLang("Set Investigate Flag")?>?')) { document.location = '<?="/customers/account_status.php?op=investigate_on&customer_id=" . $customer_id;?>';}"><?=$lang->getLang("Set Investigate Flag")?></a>
        <?php }

        if( $customer_row['email_reg_code'] ){
            ?>
            <strong>|</strong>
            <a href="javascript: if(confirm('<?=$lang->getLang("Resend activation email")?>?')) { document.location = '<?="/customers/account_status.php?op=resend_email&customer_id=" . $customer_id;?>';}"><?=$lang->getLang("Resend email")?></a>
            <?php
        }

    } ?>

</div>
<?showErrors()?>
<?form_head($action=SELF, $name="customer_admin", $method="POST")?>
<input type="hidden" name="curr_rec_index" value="<? echo $curr_rec_index;?>">
<input type="hidden" name="op" value="update">
<input type="hidden" name="customer_id" value="<?=$customer_id; ?>">
<input type="hidden" name="reg_form_submitted" value="yes">
<input type="hidden" name="scode" value="<?=mktime().session_id()?>">

<?php
$states = array(
    array(id => "13" , name => "ABRUZZO"),
    array(id => "17" , name => "BASILICATA"),
    array(id => "18" , name => "CALABRIA"),
    array(id => "15" , name => "CAMPANIA"),
    array(id => "8"  , name => "EMILIA-ROMAGNA"),
    array(id => "6"  , name => "FRIULI-VENEZIA GIULIA"),
    array(id => "12" , name => "LAZIO"),
    array(id => "7"  , name => "LIGURIA"),
    array(id => "3"  , name => "LOMBARDIA"),
    array(id => "11" , name => "MARCHE"),
    array(id => "14" , name => "MOLISE"),
    array(id => "1"  , name => "PIEMONTE"),
    array(id => "21" , name => "PR AU BOLZANO"),
    array(id => "22" , name => "PR AU DI TRENTO"),
    array(id => "16" , name => "PUGLIA"),
    array(id => "20" , name => "SARDEGNA"),
    array(id => "19" , name => "SICILIA"),
    array(id => "9"  , name => "TOSCANA"),
    array(id => "21" , name => "TRENTINO"),
    array(id => "10" , name => "UMBRIA"),
    array(id => "2"  , name => "VALLE D'AOSTA"),
    array(id => "5"  , name => "VENETO")
);
$profiles=getCustomerProfiles();
?>

<table border="0" cellspacing="0" cellpadding="0">
    <tr valign="top">
        <td>
            <table border="0" cellspacing="1" cellpadding="4" >
                <tr valign="middle" align="left">
                    <td colspan=2 class="formheading"><?=$lang->getLang("Personal Details")?></td>
                </tr>
                <tr valign="top" align="left">
                    <td width=160 class="label"><?=$lang->getLang("Customer number")?></td>
                    <td class="content"><?=replace_quotes($customer_number); ?></td>
                </tr>
                <tr valign="middle" align="left">
                    <td width=140 class="label"><?=$lang->getLang("Partner user id")?></strong></td>
                    <td class="content"><?=($partnerId!=''? $partnerId : 'Internal')?></td>
                    <input type="hidden" name="partner_user_id" size=40 maxlength=100 value="<?=replace_quotes($partnerId); ?>" <?=(($userCanModify||check_access('customers_modify_email'))?(""):("disabled"))?>>
                </tr>
                <tr valign="top" align="left">
                    <td width=160 class="label"><?=$lang->getLang("Jurisdictions")?></td>
                    <td class="content">
                        <table border="0" cellspacing="1" cellpadding="4">
                            <tr>
                                <td class="content" align="right"><b><?=$lang->getLang("Club")?>:</b></td><td class="content" align="left"><?=replace_quotes($betting_club);?></td>
                            </tr>
                            <tr>
                                <td class="content" align="right"><b><?=$lang->getLang("District")?>:</b></td><td class="content" align="left"><?=replace_quotes($district);?></td>
                            </tr>
                            <tr>
                                <td class="content" align="right"><b><?=$lang->getLang("Region")?>:</b></td><td class="content" align="left"><?=replace_quotes($region);?></td>
                            </tr>
                            <tr>
                                <td class="content" align="right"><b><?=$lang->getLang("Country")?>:</b></td><td class="content" align="left"><?=replace_quotes($nation);?></td>
                            </tr>
                            <tr>
                                <td class="content" align="right"><b><?=$lang->getLang("Skin Url")?>:</b></td><td class="content" align="left"><?=replace_quotes($url);?></td>
                            </tr>
                        </table>
                    </td>
                </tr>
                <tr valign="middle" align="left">
                    <td width=140 class="label"><?=$lang->getLang("Username")?></strong></td>
                    <td class="content"><?=$username?>
                        <input type="hidden" name="username" size=40 maxlength=100 value="<?=replace_quotes($username); ?>" <?=(($userCanModify||check_access('customers_modify_email'))?(""):("disabled"))?>>
                    </td>
                </tr>



                <tr valign="middle" align="left">
                    <td class="label"><?=$lang->getLang("Email")?>: </td>
                    <td class="content">
                        <input type="text" name="email" size=40 maxlength=100 value="<?=replace_quotes($email); ?>" <?=(($userCanModify||check_access('customers_modify_email')) ?(""):("disabled"))?>>
                    </td>
                </tr>
                <tr valign="middle" align="left">
                    <td class="label"><?=$lang->getLang("Gender")?></td>
                    <td class="content">
                        <input type=radio name="gender" value="male" <?if ( 'male' == strtolower($gender) || 'm'==strtolower($gender)) echo 'checked'?> <?=((($userCanModify)&&($customer_row["identified"] != 1 || $_SESSION['jurisdiction_class']=='casino'))?(""):("disabled"))?>>Male
                        &nbsp;&nbsp;&nbsp;<input type=radio name="gender" value="female" <?if ( 'female' == strtolower($gender)) echo 'checked'?> <?=((($userCanModify)&&($customer_row["identified"] != 1 || $_SESSION['jurisdiction_class']=='casino'))?(""):("disabled"))?>>Female
                    </td>
                </tr>
                <tr valign="middle" align="left">
                    <td class="label"><?=$lang->getLang("Name")?>:</td>
                    <td class="content"><input type="text" name="first_name" size=40 maxlength=100 value="<?=replace_quotes($first_name); ?>" <?=((($userCanModify)&&($customer_row["identified"] != 1 || $_SESSION['jurisdiction_class']=='casino'))?(""):("disabled"))?>></td>
                </tr>
                <tr valign="middle" align="left">
                    <td class="label"><?=$lang->getLang("Middle name")?>: </td>
                    <td class="content"><input type="text" name="middle_name" size=40 maxlength=30 value="<?=replace_quotes($middle_name); ?>" <?=((($userCanModify)&&($customer_row["identified"] != 1 || $_SESSION['jurisdiction_class']=='casino'))?(""):("disabled"))?>></td>
                </tr>
                <tr valign="middle" align="left">
                    <td class="label"><?=$lang->getLang("Last Name")?>: </td>
                    <td class="content"><input type="text" name="last_name" size=40 maxlength=30 value="<?=replace_quotes($last_name); ?>" <?=((($userCanModify)&&($customer_row["identified"] != 1 || $_SESSION['jurisdiction_class']=='casino'))?(""):("disabled"))?>></td>
                </tr>
                <tr valign="middle" align="left">
                    <td class="label"><?=$lang->getLang("Birthday")?>: </strong> <br/>(dd/mm/yyyy)</td>
                    <td class="content">
                        <input type="text" name="dob" id="dob" size=40 maxlength=30 value="<?=replace_quotes($dob); ?>" <?=((($userCanModify)&&($customer_row["identified"] != 1 || $_SESSION['jurisdiction_class']=='casino'))?(""):("disabled"))?>>
                    </td>
                </tr>
                <tr valign="middle" align="left">
                    <td class="label"><?=$lang->getLang("National Insurance number")?> </td>
                    <td class="content"><input type="text" value="<?=strtoupper($customer_row["nin_code"])?>" name="nin_code" size="40" <?=((($userCanModify)&&($customer_row["identified"] != 1 || $_SESSION['jurisdiction_class']=='casino'))?(""):("disabled"))?>/></td>
                </tr>
                <tr valign="middle" align="left">
                    <td class="label"><?=$lang->getLang("Address Line 1")?>:</td>
                    <td class="content"><input type="text" name="address1" size=40 maxlength=30 value="<?=replace_quotes($address1); ?>" <?=((($userCanModify)&&($customer_row["identified"] != 1 || $_SESSION['jurisdiction_class']=='casino'))?(""):("disabled"))?>></td>
                </tr>
                <tr valign="middle" align="left">
                    <td class="label"><?=$lang->getLang("Address Line 2")?>:</td>
                    <td class="content"><input type="text" name="address2" size=40 maxlength=30 value="<?=replace_quotes($address2); ?>" <?=((($userCanModify)&&($customer_row["identified"] != 1 || $_SESSION['jurisdiction_class']=='casino'))?(""):("disabled"))?>></td>
                </tr>
                <tr valign="middle" align="left">
                    <td class="label"><?=$lang->getLang("City")?></td>
                    <td class="content"><input type="text" name="city_suburb" size=40 maxlength=30 value="<?=replace_quotes($city_suburb); ?>" <?=((($userCanModify)&&($customer_row["identified"] != 1 || $_SESSION['jurisdiction_class']=='casino'))?(""):("disabled"))?>></td>
                </tr>
                <tr valign="middle" align="left">
                    <td class="label"><?=$lang->getLang("State or province")?></td>
                    <td class="content">
                        <?php if(DOT_IT) : ?>
                            <select name="state_province" <?=((($userCanModify)&&($customer_row["identified"] != 1 || $_SESSION['jurisdiction_class']=='casino'))?(""):("disabled"))?>>
                                <?php
                                foreach ($states as $province){
                                    printOption("state_province", $province["id"], ucwords(strtolower($province["name"])), $state_province == $province["id"]);
                                }
                                ?>
                            </select>
                        <?php else : ?>
                            <input type=text name="state_province" size=40  value="<?=replace_quotes($state_province); ?>" <?=((($userCanModify)&&($customer_row["identified"] != 1 || $_SESSION['jurisdiction_class']=='casino'))?(""):("disabled"))?>>
                        <?php endif; ?>
                    </td>
                </tr>
                <tr valign="middle" align="left">
                    <td class="label"><?=$lang->getLang("Postcode or Zipcode")?></td>
                    <td class="content"><input type="text" name="postcode_zip" size=40 maxlength=30 value="<?=replace_quotes($postcode_zip); ?>" <?=((($userCanModify)&&($customer_row["identified"] != 1 || $_SESSION['jurisdiction_class']=='casino'))?(""):("disabled"))?>></td>
                </tr>
                <tr valign="middle" align="left">
                    <td class="label"><?=$lang->getLang("Country")?>:</td>
                    <td class="content">
                        <?if($userCanModify){ ?>
                            <?countrySelect('country_code',$onchange='',$default=$country_code,!$userCanModify);?>
                        <? } else {?>
                            <?=getCountryName($country_code)?>
                        <? } ?>
                    </td>
                </tr>
                <tr valign="middle" align="left">
                    <td class="label"><?=$lang->getLang("Home Phone")?>:</td>
                    <td class="content"><input type="text" name="phone_home" size=40 maxlength=30 value="<?=replace_quotes($phone_home); ?>" <?=((($userCanModify)&&($customer_row["identified"] != 1 || $_SESSION['jurisdiction_class']=='casino'))?(""):("disabled"))?>></td>
                </tr>
                <tr valign="middle" align="left">
                    <td class="label"><?=$lang->getLang("Business Phone")?>:</td>
                    <td class="content"><input type="text" name="phone_business" size=40 maxlength=30 value="<?=replace_quotes($phone_business); ?>" <?=((($userCanModify)&&($customer_row["identified"] != 1 || $_SESSION['jurisdiction_class']=='casino'))?(""):("disabled"))?>></td>
                </tr>
                <tr valign="middle" align="left">
                    <td class="label"><?=$lang->getLang("Mobile Phone")?>:</td>
                    <td class="content"><input type="text" name="phone_mobile" size=40 maxlength=30 value="<?=replace_quotes($phone_mobile); ?>" <?=((($userCanModify)&&($customer_row["identified"] != 1 || $_SESSION['jurisdiction_class']=='casino'))?(""):("disabled"))?>></td>
                </tr>
                <tr valign="middle" align="left">
                    <td class="label"><?=$lang->getLang("Fax Number")?>:</td>
                    <td class="content"><input type="text" name="fax" size=40 maxlength=30 value="<?=replace_quotes($fax); ?>" <?=((($userCanModify)&&($customer_row["identified"] != 1 || $_SESSION['jurisdiction_class']=='casino'))?(""):("disabled"))?>></td>
                </tr>

                <tr valign="middle" align="left">
                    <td class="label"><?=$lang->getLang("Betting profile")?></td>
                    <td class="content">
                        <select name="profile"  onchange="showProfileDetails(this.value)" id='profile' <?=(($userCanModify)?(""):("style='display:none'"))?> >
                            <?php
                            while ($profiles->hasNext()) {
                                $p = $profiles->next();
                                printOption("profile", $p["pft_id"], ucwords(strtolower($p["pft_name"])), $profile == $p["pft_id"]);
                                $profile_details .="<table border='0' cellspacing='1' cellpadding='4' id='profile".$p["pft_id"]."' class='hidden'>
                                    <tr>
                                         <td class='content' align='right'><b>".$lang->getLang('Min Bet').":</b></td><td class='content' align='left'>".($p["pft_min_bet"]*$currencyConversion)." ( ".$symbol." )</td>
                                     </tr>
                                    <tr>
                                        <td class='content' align='right'><b>".$lang->getLang('Max Single Bet').":</b></td><td class='content' align='left'>".($p["pft_max_single_bet"]*$currencyConversion)." ( ".$symbol." )</td>
                                    </tr>
                                    <tr>
                                        <td class='content' align='right'><b>".$lang->getLang('Max Singe Win').":</b></td><td class='content' align='left'>".($p["pft_max_single_win"]*$currencyConversion)." ( ".$symbol." )</td>
                                    </tr>
                                    <tr>
                                        <td class='content' align='right'><b>".$lang->getLang('Max Multiple Bet').":</b></td><td class='content' align='left'>".($p["pft_max_multiple_bet"]*$currencyConversion)." ( ".$symbol." )</td>
                                    </tr>
                                    <tr>
                                        <td class='content' align='right'><b>".$lang->getLang('Max Multiple Win').":</b></td><td class='content' align='left'>".($p["pft_max_multiple_win"]*$currencyConversion)." ( ".$symbol." )</td>
                                    </tr>
                                    <tr>
                                        <td class='content' align='right'><b>".$lang->getLang('Limit Single').":</b></td><td class='content' align='left'>".($p["pft_limit_single"]*$currencyConversion)." ( ".$symbol." )</td>
                                    </tr>
                                    <tr>
                                        <td class='content' align='right'><b>".$lang->getLang('Limit Multiple').":</b></td><td class='content' align='left'>".($p["pft_limit_multiple"]*$currencyConversion)." ( ".$symbol." )</td>
                                    </tr>
                                    <tr>
                                        <td class='content' align='right'><b>".$lang->getLang('Limit Double Integral').":</b></td><td class='content' align='left'>".($p["pft_limit_double_integral"]*$currencyConversion)." ( ".$symbol." )</td>
                                    </tr>
                                    <tr>
                                        <td class='content' align='right'><b>".$lang->getLang('Limit System').":</b></td><td class='content' align='left'>".($p["pft_limit_system"]*$currencyConversion)." ( ".$symbol." )</td>
                                    </tr>
                                 </table>";
                            }
                            ?>
                        </select>
                        <?=$profile_details?>

                    </td>
                </tr>


                <tr valign="middle" align="left">
                    <td class="label"><?=$lang->getLang("Daily deposit limit")." ( ".$symbol." )" ?>:</td>
                    <td class="content">
                        <?if($usercanchangedaily){?>
                            <input type="text" name="daily_limit" size=40 maxlength=30 value="<?=replace_quotes($daily_limit*$currencyConversion); ?>" <?=(($userCanModify)?(""):("disabled"))?>>
                        <?} else{?>
                            <input type="hidden" name="daily_limit" size=40 maxlength=30 value="<?=replace_quotes($daily_limit*$currencyConversion); ?>" <?=(($userCanModify)?(""):("disabled"))?>><?=replace_quotes($daily_limit); ?>
                        <? } ?>
                    </td>
                </tr>
                <tr valign="middle" align="left">
                    <td class="label"><?=$lang->getLang("User limits")?>:</td>
                    <td class="content">

                        <?=getLimits($user_limit)?>
                    </td>
                </tr>
                <?php if(!DOT_IT || $userCanIdentify) : ?>
                    <tr valign="middle" align="left">
                        <td class="label"><?=$lang->getLang("Identified")?>:</td>
                        <td class="content">
                            <?php if($_SESSION["jurisdiction_class"] == "casino") {  ?>

                            <input type="checkbox"
                                <? if ( 1 == $identified ) { ?>
                                    checked="checked" disabled='disabled'
                                <? } else { ?>
                                    name='identified' <?=(($userCanModify)?(""):("disabled")); } ?>
                            >
                                <?if ( 1 == $identified ) { ?> <input type="hidden" name="identified" value='1'> <? }?>
                            <?php }else { ?>
                                <script type="text/javascript">
                                    text = "<?=$lang->getLang("Once the user has been identified you won't be able to submit any further changes.");?>";
                                </script>
                            <input type="checkbox" name="identified" onchange='if(this.checked){alert(text);}' <? if ( 1 == $customer_row["identified"] ) echo " checked";?> <?=(($userCanIdentify)?(""):("disabled"))?>>
                            <?php } ?>
                        </td>
                    </tr>
                <?php endif; ?>
                <tr valign="top" align="left">
                    <td class="label"><?=$lang->getLang("Notes")?></td>
                    <td class="content">
                        <textarea name="notes" rows=14 cols=40 wrap=hard <?=(($userCanModify)?(""):("disabled"))?>><?=replace_quotes($notes);?></textarea></td>
                </tr>
                </form>
                <?php
                /*
                <tr valign=top>
                   <td class="label">Iscrizione di gruppo</td>
                   <td class="content">
                //display all group membership checkboxes; check ones that customer is already a member
                //of, or where user has selected in last submit
                $sql  = "select cgr_code, cgr_description from cgroup order by cgr_code";
                $rs  = $dbh->exec($sql);
                $num_rows = $rs->getNumRows();

                if ( $num_rows > 0 )
                {  while ( $rs->hasNext() )
                {  $checked ='';
                $row = $rs->next();
                $group_code = $row['cgr_code'];

                if ( $form_submitted )
                {
                if ( $_POST['groups'][$group_code] )
                $checked = " CHECKED";
                }
                elseif ( in_array($group_code, $orig_groups) )
                $checked = " CHECKED";
                if ($_SESSION["jurisdiction_class"] == "casino") {
                ?>        <input type="checkbox" name="groups[<?=$group_code;?>]"<?=$checked?>/><?=$group_code?><br/>
                <?php
                } else {
                if (!empty($checked)) {
                ?>
                <input type="hidden" name="groups[<?=$group_code;?>]" value="1"/><strong><?= $group_code ?></strong><br/>
                <?php
                } else {
                ?>
                <?= $group_code ?><br/>
                <?php
                }
                }
                }
                }
                    </td>
                   </tr>
                */
                ?>
            </table>
        </td>
        <td width=30 valign=top align=left>&nbsp;</td>
        <td valign=top>
            <?php
            $format_func = ( 'FINANCIAL' == $member_type ? 'printInDollars' : 'printInFunTokens');  ?>
            <table cellpadding=4 cellspacing=1 border=0>
                <tr valign="middle" align="left">
                    <td colspan=2 class="formheading"><?=$lang->getLang("Account Activity Summary")?></strong></td>
                </tr>
                <tr>
                    <td class="label"><strong><?=$lang->getLang("Membership Type")?></strong></td>
                    <td  class="content"><?=$member_type;?> <?if(strtolower($member_type)=='affiliate' || strtolower($member_type)=='subaffiliate'){?>
                            <span class='tip'>( ID : <?=$aff_id?> )</span>
                        <? } ?>
                    </td>
                </tr>
                <?php if ( $pre_registered ) : ?>
                    <tr valign=top>
                        <td class="label"><?=$lang->getLang("Pre-registration code")?></td>
                        <td class="content">
                            <table cellpadding=0 cellspacing=0 border=0>
                                <tr valign=bottom>
                                    <td><img src="<?=image_dir;?>/icon_preg.gif"></td>
                                    <td class="content">&nbsp;<?=$preg_code;?></td>
                                </tr>
                            </table>
                        </td>
                    </tr>
                <?php endif; ?>
                <tr>
                    <td class="label"><strong><?=$lang->getLang("Account Access")?></strong></td>
                    <td class="content">
                        <?if('deny' == $access ){
                            echo  "<img src=\"".image_dir."/icon_lock_close.gif\" alt=\"Financial\"> <span style='color:red'>".strtoupper($lang->getLang("Locked"))."</span>";
                            if( $customer_row['email_reg_code'] ){
                                echo  "<br /><span style='color:red'>".strtoupper($lang->getLang("Email not confirmed!"))."</span>";
                            }
                        }else{
                            if(!is_null($login_lock)){
                                echo  "<img src=\"".image_dir."/icon_lock_close.gif\" alt=\"Financial\"> <span style='color:red'>".strtoupper($lang->getLang("Locked for wrong login"))."</span><br><span class='tip'>(The account will be unlocked in a few minutes)</span>";
                            }
                            else{
                                echo "<img src=\"".image_dir."/icon_lock_open.gif\" alt=\"Financial\"> <span style='color:rgb(8, 139, 8)'>".strtoupper($lang->getLang("Open"))."</span>";
                            }
                        }
                        if( $investigate ) echo '<br/><span style="color:red" class="large">'.$lang->getLang("Under Investigation").'</span>'; ?><br />
                    </td>
                </tr>
                <tr>
                    <td class="label"><strong><?=$lang->getLang("Account Status")?></strong></td>
                    <td class="content">
                        <?if ($customer_row["identified"]==-1){
                            echo  "<img src=\"".image_dir."/icon_disabled.gif\" alt=\"Financial\"> <span style='color:red'>".strtoupper($lang->getLang("Not Identified"))."</span>";
                        }
                        else {
                            echo "<img src=\"".image_dir."/icon_active.gif\" alt=\"Financial\"> <span style='color:rgb(8, 139, 8)'>".strtoupper($lang->getLang("Identified"))."</span>";
                        } ?>
                    </td>
                </tr>
                <tr>
                    <td class="label"><strong><?=$lang->getLang("Registration Status")?></strong></td>
                    <td  class="content"><?=ucwords($pun_reg_status);?></td>
                </tr>
                <tr>
                    <td class="label"><strong><?=$lang->getLang("User connection type")?></strong></td>
                    <td  class="content"><?=strtoupper($connectionType);?></td>
                </tr>
                <tr>
                    <td class="label"><strong><?=$lang->getLang("Registration Date")?></strong></td>
                    <td align="right" class="content"><?=shortDate($customer_row['reg_date'],true);?></td>
                </tr>
                <tr>
                    <td class="label"><strong><?=$lang->getLang("Last Login")?></strong></td>
                    <td align="right" class="content"><?=$last_login;?></td>
                </tr>
                <tr>
                    <td class="label"><strong><?=$lang->getLang("Num. Logins")?></strong></td>
                    <td align="right" class="content"><?=$num_logins;?></td>
                </tr>
                <tr>
                    <td class="label"><strong><?=$lang->getLang("Available Balance")?>:</strong></td>
                    <td align="right" class="content">
        <span style="color:blue">
          <?if(strtolower($member_type)=='affiliate' || strtolower($member_type)=='subaffiliate'){?>
              <?=getInDollars($affiliateCredits['act_credits'])?>
          <? }else{ ?>
              <?=getInDollars($customer_row['credits']);?>
          <? } ?>
        </span></td>
                </tr>
                <tr>
                    <td class="label"><strong><?=$lang->getlang("Bonus")?>:</strong></td>
                    <td align="right" class="content"><span style="color:blue">
            <?=getInDollars($customer_row['bonus_credits']);?>
        </span></td>
                </tr>
                <tr>
                    <td class="label"><strong><?=$lang->getLang("Reserved Funds")?>:</strong></td>
                    <td align="right" class="content"><span style="color:blue">
                  <?if(strtolower($member_type)=='affiliate' || strtolower($member_type)=='subaffiliate'){?>
                      <?=getInDollars($affiliateCredits['act_reserved_funds'])?>
                  <? }else{ ?>
                      <?=getInDollars($customer_row['reserved_funds']);?>
                  <? } ?>
                </tr>
                <?if(strtolower($member_type)=='affiliate' || strtolower($member_type)=='subaffiliate'){?>
                    <tr>
                        <td class="label"><strong><?=$lang->getLang("Overdraft")?></strong></td>
                        <td align="right" class="content">
                            <?=getInDollars($affiliateCredits['act_overdraft'])?>
                        </td>
                    </tr>
                <? } ?>
                <tr>

                    <td class="label"><strong><?=$lang->getLang("Total Deposit")?>:</strong></td>
                    <td align="right" class="content">
                        <?if(strtolower($member_type)=='affiliate' || strtolower($member_type)=='subaffiliate'){?>
                            <?=getInDollars($affiliateCredits['act_total_deposit'])?>
                        <? }else{ ?>
                            <?=getInDollars($customer_row['total_deposits']);?>
                        <? } ?>
                    </td>
                </tr>
                <tr>
                    <td class="label"><strong><?=$lang->getLang("Total Withdrawal")?></strong></td>
                    <td align="right" class="content">
                        <?if(strtolower($member_type)=='affiliate' || strtolower($member_type)=='subaffiliate'){?>
                            <?=getInDollars($affiliateCredits['act_total_withdraw'])?>
                        <? }else{ ?>
                            <?=getInDollars($customer_row['total_withdrawals']);?>
                        <? } ?>
                    </td>
                </tr>
                <?php if ($_SESSION["jurisdiction_class"] == "casino") : ?>
                    <tr>
                        <td class="label"><?=$lang->getLang("Total bets to date")?>:</td>
                        <td align="right" class="content">
                            <?=getInDollars($customer_row['total_bets']);?>
                        </td>
                    </tr>
                    <tr>
                        <td class="label"><?=$lang->getLang("Total returned to date")?>:</td>
                        <td align="right" class="content"><?=getInDollars($customer_row['total_wins']);?></td>
                    </tr>
                    <tr>
                        <td class="label"><?=$lang->getLang("Rake")?>:</td>
                        <td align="right" class="content"><?=getInDollars($cus_res_rake)?></td>
                    </tr>

                    <?if(strtolower($member_type)=='affiliate' || strtolower($member_type)=='subaffiliate'){?>
                        <tr>
                            <td class="label"><strong><?=$lang->getLang("Last  Deposit")?></strong></td>
                            <td align="right" class="content">
                                <?=$affiliateCredits['act_last_deposit']?>
                            </td>
                        </tr>
                    <? } ?>

                    <?if(strtolower($member_type)=='affiliate' || strtolower($member_type)=='subaffiliate'){?>
                        <tr>
                            <td class="label"><strong><?=$lang->getLang("Last withdraw")?></strong></td>
                            <td align="right" class="content">
                                <?=$affiliateCredits['act_last_withdraw']?>
                            </td>
                        </tr>
                    <? } ?>


                    <?if(strtolower($member_type)=='affiliate' || strtolower($member_type)=='subaffiliate'){?>
                        <tr>
                            <td class="label"><strong><?=$lang->getLang("Total overdraft")?></strong></td>
                            <td align="right" class="content">
                                <?=getInDollars($affiliateCredits['act_tot_overdraft_received'])?>
                            </td>
                        </tr>
                    <? } ?>

                    <?if(strtolower($member_type)=='affiliate' || strtolower($member_type)=='subaffiliate'){?>
                        <tr>
                            <td class="label"><strong><?=$lang->getLang("Overdraft start time")?></strong></td>
                            <td align="right" class="content">
                                <?=$affiliateCredits['act_overdraft_start_time']?>
                            </td>
                        </tr>
                    <? } ?>

                    <tr>
                        <td width=30 valign=top align=left>&nbsp;</td>
                    </tr>
                <?php endif; ?>
                <tr>
                    <td colspan="2" style="margin:0px; padding:0px;">
                        <?php
                        $view_user_transactions    = check_access('transactions_search');
                        $poker_view_user_hands     = check_access('casino_view_hands');
                        $view_processor_deposits   = check_access('transactions_processor_search');
                        $handle_processor_deposits = check_access('transactions_processor_search');
                        $corrective_transaction    = check_access('transactions_corrective');
                        ?>
                        <?php if ($view_user_transactions || $poker_view_user_hands || $view_processor_deposits || $handle_processor_deposits || $corrective_transaction) : ?>
                            <table cellpadding=4 cellspacing=1 border=0 width="300px">
                                <tr valign="middle" align="left">
                                    <td colspan=2 class="formheading"><strong><?=$lang->getLang("Related Tasks")?></strong></td>
                                </tr>
                                <?php if ($view_user_transactions) : ?>
                                    <?if((strtolower($member_type)=='affiliate' && check_access("affiliate_transaction_search")  ) || (strtolower($member_type)!='affiliate' && check_access("user_transaction_search")) )  { ?>
                                        <tr>
                                            <td class="label"><?=$lang->getLang("Transactions")?></td>
                                            <td align="right" class="content">
                                                <?if(strtolower($member_type)=='affiliate' || strtolower($member_type)=='subaffiliate' ){?>
                                                    <?=getClassLink(refPage('casino_games/searchtransaction') . "&field=2&value=$customer_number&srctype=3&aff=$aff_id", $lang->getLang("Transaction Search"))?>
                                                <? } else { ?>
                                                    <?=getClassLink(refPage('casino_games/searchtransaction') . "&field=2&value=$customer_number&srctype=1", $lang->getLang("Transaction Search"))?>
                                                <? } ?>
                                            </td>

                                        </tr>
                                    <?php } endif; ?>
                                <?php if ($poker_view_user_hands) : ?>
                                    <tr>
                                        <td class="label"><?=$lang->getLang("Hands History")?></td>
                                        <td  align="right" class="content"><?=getClassLink(refPage('casino_games/searchuserhand') . "&userid=$customer_number", $lang->getLang("Hands History"))?></td>
                                    </tr>
                                <?php endif; ?>
                                <?php if ($corrective_transaction) : ?>
                                    <tr>
                                        <td class="label"><?=$lang->getLang("Refund")?></td>
                                        <td align="right" class="content">
                                            <?if(strtolower($member_type)=='affiliate' || strtolower($member_type)=='subaffiliate'){?>
                                                <?=getClassLink('/affiliates_credit_transfer/' . "$aff_id", $lang->getLang("Refund"))?>
                                            <? } else{ ?>
                                                <?=getClassLink(refPage('transactions/correction') . "&customer_id=$customer_id&customer_str=$username&phase=enter_details", $lang->getLang("Refund"))?>
                                            <? } ?>


                                        </td>

                                    </tr>
                                    <?if(strtolower($member_type)!='affiliate' && strtolower($member_type)!='subaffiliate'){?>
                                        <tr>
                                            <td class="label"><?=$lang->getLang("Refund Bonus")?></td>
                                            <td align="right" class="content"><a href="/manage_bonus/<?=$username?>"><?=$lang->getLang("Refund Bonus")?></a></td>
                                        </tr>

                                    <?php } endif; ?>
                                <?php if($_SESSION["jurisdiction_class"] == "casino") : //TODO eliminare?>
                                    <?php if ($view_user_transactions) : ?>
                                        <tr>
                                            <td class="label"><?=$lang->getLang("Payment Processors")?></td>
                                            <td  align="right" class="content"><?=getClassLink(refPage('transactions/search_proc_transaction') . "&value=$customer_number&field=2", $lang->getLang("Deposits History"))?></td>
                                        </tr>
                                    <?php endif; ?>
                                <?php endif; ?>
                                <tr>
                                    <td class="label"><?=$lang->getLang("Withdraw status")?></td>
                                    <td  align="right" class="content"><?=getClassLink(refPage('customers/withdraw_status') . "&searchValue=$username", $lang->getLang("Withdraw Status check"))?></td>
                                </tr>
                                <?php if (DOT_IT) : ?>
                                    <?php if($identified != 1) :?>
                                        <tr>
                                            <td class="label"><?=$lang->getLang("Contract")?></td>
                                            <td  align="right" class="content"><?=getClassLink($skin["skn_url"] . "/accounts/Contract?uid=$customer_number",  $lang->getLang("Download contract"))?></td>
                                        </tr>
                                        <tr>
                                            <td class="label"><?=$lang->getLang("Resend confirm mail")?></td>
                                            <td  align="right" class="content"><?=getClassLink(refPage('customers/sendPrereg') . "&id=$customer_number",  $lang->getLang("Resend confirm mail"))?></td>
                                        </tr>
                                    <?php endif; ?>
                                <?php endif; ?>

                                <tr valign="middle" align="left">
                                    <td colspan=2 class="formheading"><strong><?=$lang->getLang("ID Documents")?></strong></td>
                                </tr>
                                <?php
                                $documents = getPunterDocuments($customer_id);
                                foreach($documents as $key => $doc){
                                    $name = "";
                                    if($key == "nin_code_front"){
                                        $name = $lang->getLang("National Insurance number Front");
                                    }elseif($key == "nin_code_back"){
                                        $name = $lang->getLang("National Insurance number Rear");
                                    }elseif($key == "idcard_front"){
                                        $name = $lang->getLang("Id Card Front");
                                    }elseif($key == "idcard_back"){
                                        $name = $lang->getLang("Id Card Rear");
                                    }elseif($key == "extra"){
                                        $name = $lang->getLang("Extra file");
                                    }elseif($key == "creditcard"){
                                        $name = $lang->getLang("Credit Card Front");
                                    }elseif($key == "photo"){
                                        $name = $lang->getLang("Photo");
                                    }else{
                                        $name = $key;
                                    }
                                    ?>
                                    <tr>
                                        <td colspan="2" class="content">
                                            <div class=bodyHD2><?=$name?></div>
                                            <? if($doc != "") :?>
                                                <?php
                                                $filetypes = explode("|", $doc);

                                                foreach ($filetypes as $ft){
                                                    if($ft == ".jpg"){
                                                        ?>
                                                        <a target="_blank" href="/customers/documents.php?uid=<?=$customer_id?>&type=<?=$key?>&ext=<?=$ft?>&cnb=<?=$customer_number?>&first_name=<?=$first_name?>&last_name=<?=$last_name?>">
                                                            <img border="none" src="/customers/documents.php?uid=<?=$customer_id?>&type=<?=$key?>&ext=<?=$ft?>&thumb=true" alt="<?=$name?>&cnb=<?=$customer_number?>&first_name=<?=$first_name?>&last_name=<?=$last_name?>"/>
                                                        </a>
                                                        <?php
                                                    }else{
                                                        ?>
                                                        <a href="/customers/documents.php?uid=<?=$customer_id?>&type=<?=$key?>&ext=<?=$ft?>&cnb=<?=$customer_number?>&first_name=<?=$first_name?>&last_name=<?=$last_name?>">Download -&gt; <?=$ft?></a>
                                                        <?php
                                                    }
                                                    echo "<br/>";
                                                }
                                                ?>
                                                <br/>
                                            <?php endif; ?>
                                            <?if($userCanModify){?>
                                                <?php if($identified != 1 || $_SESSION["jurisdiction_class"] == "casino") : ?>
                                                    <form action="<?=refFormPage($_REQUEST["page"]) . "&action=load&doc=$key&customer_id=" . $customer_id;?>" method="POST" name="<?=$key?>" enctype="multipart/form-data">
                                                        <input type="file" name="<?=$key?>" value="<=$lang->getLang("$name")" />
                                                        <input type="submit" value="Upload <?=$lang->getLang("$name")?>"/>
                                                    </form>
                                                <?php endif; } ?>
                                        </td>
                                    </tr>
                                    <?php
                                }
                                ?>
                            </table>
                        <?php endif; ?>
                    </td>
                </tr>

                <!--     <tr>
            <td colspan="2" width=30 valign=top align=left>&nbsp;</td>
        </tr>
        <tr>
            <td colspan="2" style="margin:0px; padding:0px;">
                <table cellpadding=4 cellspacing=1 border=0 width="300px">
                    <tr valign="middle" align="left">
                        <td colspan=2 class="formheading"><strong><?/*=$lang->getLang("Limit")*/?></strong> <span style="float:right;"> <a href="javascript:openInfoWindow('/customers/customer_update_limit.php?customer_id=<?/*=$customer_id*/?>&customer_name=<?/*=$username*/?>')" style="color:#cccccc;"><?/*=$lang->getLang("Edit limit")*/?></a></td>
                    </tr>
                    <tr>
                        <td class="label"><?/*=$lang->getLang("Limit")*/?></td><td  align="right" class="content"><?/*=getInDollars($customer_row['daily_deposits_limit'])*/?></td>
                    </tr>
                </table>
            </td>
        </tr>-->
            </table>
        </td>
        <td width=30 valign=top align=left>&nbsp;</td>
        <td width=30 valign=top align=left>
            <?php if($userCanModify || check_access('customers_modify_email')){ ?>
                <input type="button" value="<?=$lang->getLang('Save')?>" onClick="saveChanges()"><br/>
                <input type="button" value="<?=$lang->getLang("Refresh")?> " onClick="refreshForm()">
            <? }?>
        </td>
    </tr>
</table>
<br/>
<?if(check_access('customer_show_note')) { ?>
    <table  style="max-width:930px" width="100%" >
        <tr>
            <td valign="top" style="min-width:465px;max-width;">
                &nbsp;
                <?php
                $sql="SELECT clg_time, clg_ipaddress, clt_description, clg_data FROM customer_log JOIN customer_log_type ON clt_id=clg_clt_code WHERE clg_punid=$customer_id ORDER BY clg_time DESC";
                $limit = 15;
                $dbh->Connection->setLimit($limit);
                $rs = $dbh->exec($sql);

                $cols_arr = array("Last $limit log");
                $val_fmt_arr = array(
                    "Last $limit log"       => 'return shortFormatDate($rec["clg_time"]) . " " . getClassLink(refPage(\'customers/search&field=4&search_string=\' . urlencode(ereg_replace("[^0-9\.]", "", $rec["clg_ipaddress"]))), $rec["clg_ipaddress"]) . "<br/><b>" . $rec["clt_description"] . "</b> <i>" . $rec["clg_data"] . "</i>";'
                );
                $cell_fmt_arr = array(
                    'Date' => 'class="date" nowrap'
                );

                $numrow = $rs->getNumRows();
                if($numrow > 0){
                    $qp = new QueryPrinter($rs);
                    $qp->printTable($rs, $cols_arr, "", $val_fmt_arr, $cell_fmt_arr, 0, 0, "", $width="100%", 1, $numrow, true);
                }else{
                    addError("", $lang->getLang("0 Records found"));
                    showErrors();
                }
                ?>
            </td>

            <style>
                .date{
                    font-size:10px;
                }
                .locked{
                    color:Red;
                }
                .unlocked{
                    color:Green;
                }
            </style>

            <td  valign="top" style='max-width:465px;min-width:465px'>
                <?if($userCanModify){?>
                    <a href="javascript:openInfoWindow(' <?='/customers/customer_add_note.php?customer_id='.$customer_id.'&customer_name='.$username ?>')"><?=$lang->getLang('Insert note')?></a>
                <? } ?>
                <?
                $sql="SELECT aun_aus_id, aun_date, aun_note, aus_username FROM admin_user_note JOIN admin_user ON aus_id = aun_aus_id WHERE aun_pun_id = $customer_id ORDER BY aun_date DESC"; //Modificato query Manuel Rinaldi
                $rs = $dbh->exec($sql);

                $cols_arr = array("Note");
                $val_fmt_arr = array(
                    'Note'       => 'return shortFormatDate($rec["aun_date"]) . "<br/><i>" . getClassLink("/admin_users/admin_user_edit.php?id=" . $rec["aun_aus_id"], $rec["aus_username"]) . "</i>" . ":\n" . $rec["aun_note"];'
                );

                $cell_fmt_arr = array(
                    'Date' => 'class="date" nowrap'
                );

                $numrow = $rs->getNumRows();
                if($numrow > 0){
                    $qp = new QueryPrinter($rs);
                    $qp->printTable($rs, $cols_arr, "", $val_fmt_arr, $cell_fmt_arr, 0, 0, "", $width="100%", 1, $numrow, true);
                }else{
                    addError("", $lang->getLang("0 Records found"));
                    ?> <br /> <br /><br /> <br /> <?
                    showErrors();
                }
                ?>

                <?
                $sql="SELECT alg_data, alg_time, aus_username, alg_ipaddr FROM admin_user_log JOIN admin_user ON aus_id = alg_aus_id WHERE alg_pun_id = $customer_id ORDER BY alg_time DESC"; //Modificato query Manuel Rinaldi
                $rs = $dbh->exec($sql);

                $cols_arr = array("Operation Logs");
                $val_fmt_arr = array(
                    'Operation Logs'       => 'return "<p style=word-break:break-all;>".shortFormatDate($rec["alg_time"]) . " " . getClassLink(refPage(\'customers/search&field=4&search_string=\' . urlencode(ereg_replace("[^0-9\.]", "", $rec["alg_ipaddr"]))), $rec["alg_ipaddr"]) ."<br/><i>" . getClassLink("/admin_users/admin_user_edit.php?id=" . $rec["alg_aus_id"], $rec["aus_username"]) . "</i>" . ":\n" . $rec["alg_data"];'
                );

                $cell_fmt_arr = array(
                    'Date' => 'class="date" nowrap'
                );

                $numrow = $rs->getNumRows();
                if($numrow > 0){
                    $qp = new QueryPrinter($rs);
                    $qp->printTable($rs, $cols_arr, "", $val_fmt_arr, $cell_fmt_arr, 0, 0, "", $width="100%", 1, $numrow, true);
                }else{
                    addError("", $lang->getLang("0 Records found"));
                    showErrors();
                }
                ?>
            </td>

        </tr>
    </table>
<? } ?>