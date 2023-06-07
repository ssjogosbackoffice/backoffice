<?php
$dbh->exec("SET character_set_results = 'utf8', character_set_client = 'utf8', character_set_connection = 'utf8', character_set_database = 'utf8', character_set_server = 'utf8'",false,true);
mysql_set_charset('utf8');
function save_district($name, $region_id, $code,$country,$city,$accessType, $childsLimit,$u_processors) {
    global $dbh,$lang;
    if ( isBlank($name) ) {
        addError("", $lang->getLang("Please enter a valid name"));
    }
    $name=trim($name);
    if ( isBlank($country) ) {
        addError("", $lang->getLang("Please select a country"));
    }
    else{
        $country=getCountry($country);
    }

    if ( isBlank($city) ) {
        addError("", $lang->getLang("Please enter a city"));
    }
    else{
        $city=explode(',',$city);
        $city=$city[0];
    }
    if ($childsLimit <=0) {
        $childsLimit = 20;
    }
    if ( !$name ) {
        addError('district_name', $lang->getLang("A district name must be entered."));
    }else {
        $sql = "SELECT count(*) FROM jurisdiction WHERE LOWER(jur_name) = LOWER(" . $dbh->prepareString($name) . ") AND lower(jur_name) != 'internet'";
        $count = $dbh->countQuery($sql);
        if ( $count > 0 ) {
            addError('district_name', $lang->getLang("The district name you entered already exists as the name of a region, district or club"));
        }
    }

    if ( isBlank($code) ) {
        addError("account_code", $lang->getLang("An account code must be entered"));
    }
    elseif ( mb_strlen($code) != 6 || ! mb_ereg_match("^[A-Za-z]*$", $code) ) {
        addError("account_code", $lang->getLang("The account code has to contain only 3 alphabetical characters"));
    }else {
        $sql = "SELECT count(*) FROM jurisdiction WHERE LOWER(jur_code) = " . $dbh->prepareString(strtolower($code));
        $count = $dbh->countQuery($sql);

        if ( $count > 0 ) {
            addError("account_code", $lang->getLang("The account code you entered already exists for another jurisdiction"));
        }
    }

    $region_id = (int) $region_id;

    if ( ! $region_id ) {
        addError('region_id', $lang->getLang("A Region must be selected"));
    }else {
        $sql = "SELECT count(*) FROM jurisdiction WHERE jur_id = $region_id AND jur_class = 'region'";
        $valid_region_count = $dbh->countQuery($sql);

        if ( $valid_region_count != 1  ) {
            addError('region_id', $lang->getLang('Invalid Region ID'));
        }
    }

    $sql="SELECT * FROM jurisdiction_payment_settings WHERE JPS_JUR_ID='$region_id' ";
    $parentResult = $dbh->queryRow($sql);
    if(!$parentResult){
        addError("percetage", $lang->getLang("You have to set the percentage of the district before to go forward"));
    }
    require_once "funclib/percentual.inc.php";
    if(!checkPercentualsCasino()){
        addError("percetage", $lang->getLang("You have to set the percentage fields to save the new %", "district"));
    }
    if(!checkPercentualsFields()){
        addError("percetage", $lang->getLang("You have to set the percentage fields to save the new %", "district"));
    }

    if ( ! areErrors() ) {
        $next_id  = $dbh->nextSequence('JUR_ID_SEQ');
        $currency = $dbh->queryOne('SELECT jur_currency from jurisdiction where jur_id='.$dbh->escape($region_id));
        if(!is_numeric($currency) || $currency==''){
            $currency=1;
        }
        //$currency=($currency=="")?1:$currency;
        $skinId = $dbh->queryOne('SELECT jur_skn_id from jurisdiction where jur_id='.$dbh->escape($region_id));
        if(!is_numeric($skinId)){
            $skinId='NULL';
        }
        $timeZone= $dbh->queryOne('SELECT jur_time_utc from jurisdiction where jur_id='.$dbh->escape($region_id));
        $dbh->begin();
        if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting") betting_begin();
        $sql = "INSERT INTO jurisdiction (jur_id, jur_name, jur_class, jur_parent_id, jur_code, jur_currency,jur_cou_id,jur_city,jur_skn_id,jur_time_utc,jur_access_type, jur_childs_limit,jur_processor_enabled)" .
            " VALUES (" . $next_id . ", " . $dbh->prepareString($name) . ", 'district', " .
            $region_id . ", '" . $code ."',$currency,".$dbh->prepareString($country).",".$dbh->prepareString($city).",$skinId,$timeZone,".$dbh->prepareString(strtoupper($accessType)).",".$dbh->prepareString($childsLimit).",".$dbh->prepareString($u_processors).")";
        $ret=$dbh->exec($sql);

        $casino_db_trans = false;
        $betting_db_trans = false;
        if ( !$ret ) {
            addError("", $lang->getLang('Database Error - Could not create jurisdiction record'));
            $casino_db_trans = false;
        }else{
            if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
                $result = save_betting_jurisdiction($next_id,$name,$region_id,"district");
                if($result){
                    $perc_res = savePercentuals($_POST["percentuals"], $next_id, $region_id,$_POST["betting"],$_POST['betting_live']);
                    if($perc_res){
                        $betting_result = saveBettingPercentuals($_POST["betting"], $next_id);
                        if($betting_result){
                            $casino_db_trans = true;
                            $betting_db_trans = true;
                        }else{
                            $casino_db_trans = false;
                            $betting_db_trans = false;
                            addError("", $lang->getLang('Database Error - Could not create betting percentages record'));
                        }
                    }else{
                        $casino_db_trans = false;
                        $betting_db_trans = false;
                        addError("", $lang->getLang('Database Error - Could not create percentages record'));
                    }
                }else{
                    $casino_db_trans = false;
                    $betting_db_trans = false;
                    addError("", $lang->getLang('Database Error - Could not create betting jurisdiction record'));
                }
            }else{
                $perc_res = savePercentuals($_POST["percentuals"], $next_id, $region_id,$_POST["betting"],$_POST['betting_live']);
                if($perc_res){
                    $casino_db_trans = true;
                    $betting_db_trans = true;
                }else{
                    $casino_db_trans = false;
                    $betting_db_trans = false;
                    addError("", $lang->getLang('Database Error - Could not create percentage jurisdiction record'));
                }
            }
        }
        if($casino_db_trans && $betting_db_trans){
            if($_POST['username']!=''){
                $saved = save_admin_user ($_POST['username'], $_POST['email'], $_POST['full_name'], $_POST['access'], $_POST['user_type'], $next_id, $_POST['user_country'], $_POST['password'], $_POST['password_confirm']);
                if (  TRUE != $saved ){
                    addError('',"Administrator not saved");
                    $dbh->rollback();
                    if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting") betting_rollback();
                    return FALSE;
                }
            }

            $structureConfiguration=$dbh->exec('INSERT INTO jurisdiction_configuration (SCN_JUR_ID, SCN_GAMES_INFO, SCN_MENU_INFO, SCN_SKIN_PROPERTIES,
                                              SCN_MOBILE_GAMES_INFO, SCN_MOBILE_MENU_INFO,SCN_MOBILE_SKIN_PROPERTIES)
                                        SELECT '.$next_id.', SCN_GAMES_INFO, SCN_MENU_INFO, SCN_SKIN_PROPERTIES ,SCN_MOBILE_GAMES_INFO, SCN_MOBILE_MENU_INFO,SCN_MOBILE_SKIN_PROPERTIES
                                        FROM jurisdiction_configuration
                                        WHERE SCN_JUR_ID = '.$region_id);

            $dbh->commit();
            if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting") betting_commit();
            return $next_id;
        }else{
            $dbh->rollback();
            if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting") betting_rollback();
            return FALSE;
        }

    }
    return FALSE;
}

function update_user_limit_childs($jurisdictionId) {
    
    global $dbh,$lang;
    
    $sql = "UPDATE jurisdiction SET jur_childs_limit = jur_childs_limit - 1 WHERE jur_id = " . $jurisdictionId . " ";
    $rs  = $dbh->exec($sql);
    if (!$rs) {
        addError('generic_error', $lang->getLang('Unable to update Childs limit'));
        return FALSE;
    } else {
        $_SESSION['jur_childs_limit'] = $_SESSION['jur_childs_limit'] - 1;
        return TRUE;
    }
}


function update_district($id, $name, $region_id,$country,$city,$accessType, $childsLimit,$u_processors) {
    global $dbh,$lang;
    if ( isBlank($name) ) {
        addError("", $lang->getLang("Please enter a valid name"));
    }
    $name=trim($name);
    if ( isBlank($country) ) {
        addError("", $lang->getLang("Please select a country"));
    }
    else{
        $country=getCountry($country);
    }

    if ( isBlank($city) ) {
        addError("", $lang->getLang("Please enter a city"));
    }
    else{
        $city=explode(',',$city);
        $city=$city[0];
    }
    // get current record columns
    
    if ($childsLimit <= 0) {
        $sql = "SELECT * FROM jurisdiction j WHERE jur_id = " . $id . " ";
        $rs  = $dbh->exec($sql);
        $row = $rs->next();
        $childsLimit = $row['jur_childs_limit'];
    }
    if ( !$name ) {
        addError('district_name', $lang->getLang("A district name must be entered."));
    }else {
        $sql = "SELECT count(*) FROM jurisdiction WHERE LOWER(jur_name) = LOWER(" . $dbh->prepareString($name) . ")  AND jur_id != $id AND lower(jur_name) != 'internet'";
        $count = $dbh->countQuery($sql);
        if ( $count > 0 ) {
            addError('district_name', $lang->getLang("The district name you entered already exists as the name of a region, district or club"));
        }
    }
    $region_id = (int) $region_id;
    if ( ! $region_id ) {
        addError('region_id', $lang->getLang("A Region must be selected"));
    }else {
        $sql = "SELECT count(*) FROM jurisdiction WHERE jur_id = $region_id AND jur_class = 'region'";
        $valid_region_count = $dbh->countQuery($sql);
        if ( $valid_region_count != 1  ) {
            addError('region_id', $lang->getLang('Invalid Region ID'));
        }
    }

    $sql="SELECT * FROM jurisdiction_payment_settings WHERE JPS_JUR_ID='$region_id' ";
    $parentResult = $dbh->queryRow($sql);
    if(!$parentResult){
        addError("percetage", $lang->getLang("You have to set the percentage of the district before to go forward"));
    }
    require_once "funclib/percentual.inc.php";
    if(!checkPercentualsCasino()){
        addError("percetage", $lang->getLang("You have to set the percentage fields to save the new %", "district"));
    }
    if(!checkPercentualsFields()){
        addError("percetage", $lang->getLang("You have to set the percentage fields to save the new %", "district"));
    }

    if ( ! areErrors() ) {
        $dbh->begin();
        if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting") betting_begin();
        $currency = $dbh->queryOne('SELECT jur_currency from jurisdiction where jur_id='.$dbh->escape($region_id));
        if(!is_numeric($currency) || $currency==''){
            $currency=1;
        }
        $skinId = $dbh->queryOne('SELECT jur_skn_id from jurisdiction where jur_id='.$dbh->escape($region_id));
        if(!is_numeric($skinId)){
            $skinId='NULL';
        }
        $timeZone= $dbh->queryOne('SELECT jur_time_utc from jurisdiction where jur_id='.$dbh->escape($region_id));
        $sql = "UPDATE jurisdiction" .
            " SET jur_name = " . $dbh->prepareString($name) .
            "  , jur_parent_id = " . $region_id .
            "  , jur_currency=$currency
            , jur_cou_id = " . $country .
            ", jur_city =".$dbh->prepareString($city).
            ", jur_childs_limit = " . $childsLimit .
            ", jur_time_utc =".$dbh->prepareString($timeZone).
            ",jur_skn_id =".$dbh->prepareString($skinId).
            ",jur_processor_enabled =".$dbh->prepareString($u_processors).
            ",jur_access_type =".$dbh->prepareString(strtoupper($accessType)).
            " WHERE jur_id = $id";
        $ret=$dbh->exec($sql);
        $betting_db_trans = false;
        $casino_db_trans = false;
        $update=array('jur_currency'=>intval($currency),
            'jur_time_utc'=>intval($timeZone),
            'jur_skn_id'=>  intval($skinId)
        );

        $hierarchyUpdated=updateAllHierarchy($id,'district',$update);
        $hierarchyUpdatedPunterSkin=updateAllHierarchy($id,'district',false,'pun_site_id',$skinId);
        if ( !$ret ) {
            addError("", $lang->getLang('Database Error - Could not create jurisdiction record'));
            $casino_db_trans = false;
        }else{
            if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
                $result = update_betting_jurisdiction($id,$name,$region_id,"club");
                if($result){
                    $perc_res = savePercentuals($_POST["percentuals"], $id, $region_id,$_POST["betting"],$_POST['betting_live']);
                    if($perc_res){
                        $betting_result = saveBettingPercentuals($_POST["betting"], $id);
                        if($betting_result){
                            $casino_db_trans = true;
                            $betting_db_trans = true;
                        }else{
                            $casino_db_trans = false;
                            $betting_db_trans = false;
                            addError("", $lang->getLang('Database Error - Could not create betting percentages record'));
                        }
                    }else{
                        $casino_db_trans = false;
                        $betting_db_trans = false;
                        addError("", $lang->getLang('Database Error - Could not create percentages record'));
                    }
                }else{
                    $casino_db_trans = false;
                    $betting_db_trans = false;
                    addError("", $lang->getLang('Database Error - Could not create betting jurisdiction record'));
                }
            }else{
                $perc_res = savePercentuals($_POST["percentuals"], $id, $region_id,$_POST["betting"],$_POST['betting_live']);
                if($perc_res){
                    $casino_db_trans = true;
                    $betting_db_trans = true;
                }else{
                    $casino_db_trans = false;
                    $betting_db_trans = false;
                    addError("", $lang->getLang('Database Error - Could not create percentage jurisdiction record'));
                }
            }
        }
        if($casino_db_trans && $betting_db_trans){
            $dbh->commit();
            if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting") betting_commit();
            return true;
        }else{
            $dbh->rollback();
            if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting") betting_rollback();
            return FALSE;
        }

    }
    return FALSE;
}

$op = ( !empty($_POST['op']) ? $_POST['op'] :$_GET['op'] );

/*addedd cache*/

include "JurisdictionsList.class.inc";
$jur_list = JurisdictionsList::getInstance($dbh);

if($op != "Add" && $op != "Save" && $op != "update_subjur_processors"){
    $hierarchy = $jur_list->getJurisdictionHierarchy($node);

    $can_modify_jurisdiction = false;
    foreach($hierarchy as $parent){
        if($_SESSION["jurisdiction_id"] == $parent){
            $can_modify_jurisdiction = true;
        }
    }
    if(!$can_modify_jurisdiction){
        dieWithError($lang->getLang("Jurisdiction not found"));
    }
}
$delete_cache = false;
/*addedd cache*/

switch ( $op ) {
    case "update_subjur_processors":
        $u_processors = implode("~",$_POST['processors']);
        $hierarchyUpdated=updateAllProcSubjur($node,'district',$u_processors);
        ob_end_clean();
        echo "success";
        exit;
        break;
    case "Add": //display add new task screen
        $display =  "district_add_form.php";
        break;
    case "Delete":
        $sql = "UPDATE jurisdiction SET jur_status = 0 WHERE jur_id = $node";
        $rs  = $dbh->exec($sql);
        if($rs == 1){
            $delete_cache = true;
        }
        $alert = $lang->getLang("% blocked","district ".$node)."!</br>";

        $child_id_list   = $jur_list->getChilJurisdictionsIdList($node,null, 0);
        //$child_id_list[] = $node;
        $chunked_lists   = array_chunk($child_id_list, 1000);
        $locked_users  = 0;

        $aus_arr = array();
        $jur_arr = array();

        foreach($chunked_lists as $list){
            $aus_arr[] = "aus_jurisdiction_id IN (" . implode(",", $list) . ")";
            $jur_arr[] = "jur_id IN (" . implode(",", $list) . ")";
            $pun_arr[] = "pun_betting_club IN (" . implode(",", $list) . ")";
        }

        //Nei club non è necessario
        $sql = "UPDATE jurisdiction SET jur_status = 2 WHERE jur_status = 1 AND (" . implode(" OR ", $jur_arr) . ") AND jur_id != $node";
        //die($sql);
        $rs  = $dbh->exec($sql);
        $locked_jurs = 1 + $dbh->getAffectedRows();
        //die($dbh->getAffectedRows());
        $alert .=  $lang->getLang("% blocked children jurisdictions", $locked_jurs)."!</br>";
        //-------------------------


        if($_GET["lock_users"] == "true"){
            $sql = "UPDATE admin_user SET aus_access = 'LOCK', aus_notes = 'Locked by admin'";
            if(count($aus_arr) > 0){
                $sql .= " WHERE aus_access = 'ALLOW' AND (" . implode(" OR ", $aus_arr) . ")";
                $rs  = $dbh->exec($sql);
                $locked_users += $dbh->getAffectedRows();
            }
            $alert .=  $lang->getLang("and % locked: %",$lang->getLang("administrators"), $locked_users)."!</br>";
        }
        if($_GET["lock_punters"] == "true"){
            $sql = "UPDATE punter SET pun_access = 'deny', pun_notes = 'Locked by admin'";
            if(count($pun_arr) > 0){
                $sql .= " WHERE (" . implode(" OR ", $pun_arr) . ")";
                $rs  = $dbh->exec($sql);
                $lock_punters += $dbh->getAffectedRows();
            }
            $alert .=  $lang->getLang("and % locked: %",$lang->getLang("players"), $lock_punters);
        }
        $alert .= "!";
        doAdminUserLog($_SESSION['admin_id'], 'block_jurisdiction', escapeSingleQuotes($alert));
        break;
    case "Restore":
        $sql = "UPDATE jurisdiction SET jur_status = 1 WHERE jur_id = $node";
        $rs  = $dbh->exec($sql);
        if($rs == 1){
            $delete_cache = true;
        }
        $alert = $lang->getLang("% restored","district ".$node)."!</br>";

        $child_id_list   = $jur_list->getChilJurisdictionsIdList($node,null, 0);
        //$child_id_list[] = $node;
        $chunked_lists   = array_chunk($child_id_list, 1000);
        $locked_users  = 0;

        $aus_arr = array();
        $jur_arr = array();

        foreach($chunked_lists as $list){
            $aus_arr[] = "aus_jurisdiction_id IN (" . implode(",", $list) . ")";
            $jur_arr[] = "jur_id IN (" . implode(",", $list) . ")";
            $pun_arr[] = "pun_betting_club IN (" . implode(",", $list) . ")";
        }

        //Nei club non è necessario
        $sql = "UPDATE jurisdiction SET jur_status = 1 WHERE jur_status = 2 AND (" . implode(" OR ", $jur_arr) . ") AND jur_id != $node";
        $rs  = $dbh->exec($sql);
        $locked_jurs = 1 + $dbh->getAffectedRows();
        $alert .=  $lang->getLang("% unblocked children jurisdictions", $locked_jurs)."!</br>";
        //-------------------------


        if($_GET["unlock_users"] == "true"){
            $sql = "UPDATE admin_user SET aus_access = 'ALLOW', aus_notes = 'Unlocked by admin'";
            if(count($aus_arr) > 0){
                $sql .= " WHERE (" . implode(" OR ", $aus_arr) . ")";
                $rs  = $dbh->exec($sql);
                $locked_users += $dbh->getAffectedRows();
            }
            $alert .=  $lang->getLang("and % unlocked: %",$lang->getLang("administrators"), $locked_users)."!</br>";
        }
        if($_GET["unlock_punters"] == "true"){
            $sql = "UPDATE punter SET pun_access = 'allow', pun_notes = 'Unocked by admin'";
            if(count($pun_arr) > 0){
                $sql .= " WHERE (" . implode(" OR ", $pun_arr) . ")";
                $rs  = $dbh->exec($sql);
                $lock_punters += $dbh->getAffectedRows();
            }
            $alert .=  $lang->getLang("and % unlocked: %",$lang->getLang("players"), $lock_punters);
        }
        $alert .= "!";
        doAdminUserLog($_SESSION['admin_id'], 'unblock_jurisdiction', escapeSingleQuotes($alert));
        break;
    case "Save":  //save a new task
        $u_processors = implode("~",$_POST['processors']);
        $node = save_district($_POST['district_name'], $_POST['region_id'], $_POST['account_code'],$_POST['country'],$_POST['city'],$_POST['accessType'],$_POST['childs_limit'],$u_processors);
        if (  ! empty($node) ) {
            update_user_limit_childs($_SESSION['jurisdiction_id']);
            $display = 'district_display_form.php'; //display new record
            $district_name = $_POST['district_name'];
            $alert = "District \"$district_name\" saved";
            $delete_cache = true;

        }else {
            $display = "district_add_form.php"; //if error, display add form again
        }
        break;
    case "Update":  //update an existing task
        $u_processors = implode("~",$_POST['processors']);
        $updated = update_district($node,  $_POST['district_name'], $_POST['region_id'],$_POST['country'],$_POST['city'],$_POST['accessType'],$_POST['childs_limit'],$u_processors);
        $delete_cache = true;
        if ( $updated ) {
            $jur_list->destroyParentsJurisdictionsCache($node);
            $district_name = $_POST['district_name'];
            $alert = "District \"$district_name\" updated";
        }
        $sql = "SELECT jur_code, jur_childs_limit FROM jurisdiction  WHERE jur_id = " . $node;

        $rs = $dbh->exec($sql);
        $num_rows = $rs->getNumRows();

        if ( 1 == $num_rows ) {
            $row = $rs->next();
            $account_code  = $row['jur_code'];
            $jurChildsLimit     = $row['jur_childs_limit'];
            $u_processors = explode('~',$row['jur_processor_enabled']);
        }

        $delete_cache = true;


        $display =  'district_display_form.php';
        break;
    default:
        $display =  'district_display_form.php';
}

if($delete_cache){
    $jur_list->destroyParentsJurisdictionsCache($node);
}

if ( 'district_display_form.php' == $display ){
    if ( $_POST['op'] ) {
        if ('Update' != $op ) {
            $account_code = $_POST['account_code'];
        }
        $district_name  = $_POST['district_name'];
        $region_id      = $_POST['region_id'];
        $jur_status     = $_POST['jur_status'];
        $country        = $_POST['country'];
        $city           = $_POST['city'];
        $accessType   = $_POST['accessType'];
        $skinId = $dbh->queryOne('SELECT jur_skn_id from jurisdiction where jur_id='.$dbh->escape($node));
        $sql = "SELECT jur_name,jur_parent_id, jur_code, jur_status,jur_cou_id,jur_city,jur_creation_date,jur_time_utc,jur_available_credit,jur_childs_limit,jur_processor_enabled " .
            " FROM jurisdiction " .
            " WHERE jur_id = " . $node;
        $rs = $dbh->exec($sql);
        $num_rows = $rs->getNumRows();
        if ( 1 == $num_rows ) {
            $row = $rs->next();
            $jur_status     = $row['jur_status'];
            $jurChildsLimit     = $row['jur_childs_limit'];
            $u_processors = explode('~',$row['jur_processor_enabled']);
        }
    }else{
        if (  $delete_failed ) {
            $district_name = $row['jur_name'];
            $region_id = $row['jur_parent_id'];
        }else{
            $sql = "SELECT jur_name,jur_parent_id,jur_access_type, jur_code, jur_status,jur_cou_id,jur_city,jur_creation_date,jur_time_utc,jur_available_credit,jur_childs_limit,jur_processor_enabled " .
                " FROM jurisdiction " .
                " WHERE jur_id = " . $node;
            $rs = $dbh->exec($sql);
            $num_rows = $rs->getNumRows();
            if ( 1 == $num_rows ) {
                $row = $rs->next();
                $district_name = $row['jur_name'];
                $region_id     = $row['jur_parent_id'];
                $account_code  = $row['jur_code'];
                $jur_status    = $row['jur_status'];
                $country       = getCountryCode2Chars($row['jur_cou_id']);
                $city          = $row['jur_city'];
                $creationTime = $row['jur_creation_date'];
                $accessType   = $row['jur_access_type'];
                $skinId       = $row['jur_skn_id'];
                $jurChildsLimit     = $row['jur_childs_limit'];
                $u_processors = explode('~',$row['jur_processor_enabled']);
            }
        }
    }
}

if ( $display ) {
    require_once($display);
}
else {
    echo $lang->getLang("Please select from the menu on the left");
}
if ( isset($alert) )
    jAlert(mb_ereg_replace('"', '\"', $alert));
?>