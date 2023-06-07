<?php
$dbh->exec("SET character_set_results = 'utf8', character_set_client = 'utf8', character_set_connection = 'utf8', character_set_database = 'utf8', character_set_server = 'utf8'",false,true);
mysql_set_charset('utf8');
function save_region($name, $code,$nation,$country,$city,$accessType, $childsLimit,$skinId=false,$s_processors){
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
    if ( !$name ){
        addError('jurisdiction_name', $lang->getLang("A region name must be entered"));
    }
    else{
        $sql = "SELECT count(*) FROM jurisdiction WHERE LOWER(jur_name) = LOWER('$name')";
        $count = $dbh->countQuery($sql);

        if ( $count > 0 ){
            addError('jurisdiction_name', $lang->getLang("The Region name you entered already exists as the name of a region, district or club"));
            return FALSE;
        }
    }

    if ( isBlank($code) ){
        addError("account_code",$lang->getLang("An account code must be entered"));
    }
    elseif ( mb_strlen($code) != 6 || ! mb_ereg_match("^[A-Za-z]*$", $code) ){
        addError("account_code",  $lang->getLang("The account code has to contain only 3 alphabetical characters"));
    }
    else{
        $sql = "SELECT count(*) FROM jurisdiction WHERE LOWER(jur_code) = " . $dbh->prepareString(strtolower($code));
        $count = $dbh->countQuery($sql);
        if ( $count > 0 ){
            addError("account_code",  $lang->getLang("The account code you entered already exists for another jurisdiction."));
        }
    }

    $sql="SELECT * FROM jurisdiction_payment_settings WHERE JPS_JUR_ID='$nation' ";
    $parentResult = $dbh->queryRow($sql);
    if(!$parentResult){
        addError("percetage", $lang->getLang("You have to set the percentage of the district before to go forward"));
    }
    require_once "funclib/percentual.inc.php";
    if(!checkPercentualsCasino()){
        addError("percetage", $lang->getLang("You have to set the percentage fields to save the new %", "region"));
    }
    if(!checkPercentualsFields()){
        addError("percetage", $lang->getLang("You have to set the percentage fields to save the new %", "region"));
    }

    if ( ! areErrors() ){
        $next_id  = $dbh->nextSequence('JUR_ID_SEQ');
        $dbh->begin();
        $currency = $dbh->queryOne('SELECT jur_currency from jurisdiction where jur_id='.$dbh->escape($nation));
        if(!is_numeric($currency)){
            $currency=1;
        }
        if(!$skinId || $skinId==''){
        $skinId = $dbh->queryOne('SELECT jur_skn_id from jurisdiction where jur_id='.$dbh->escape($nation));
        }
        else{
            $skinId-=53;
        }
        if(!is_numeric($skinId)){
            $skinId='NULL';
        }
        $time= $dbh->queryOne('SELECT jur_time_utc from jurisdiction where jur_id='.$dbh->escape($nation));
        if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting") betting_begin();
        $sql = "INSERT INTO jurisdiction (jur_id, jur_name, jur_class, jur_parent_id, jur_code,jur_cou_id,jur_city,jur_currency,jur_skn_id,jur_time_utc,jur_access_type, jur_childs_limit,jur_processor_enabled)" .
            " VALUES (" . $next_id . ", " . $dbh->prepareString($name) . ", 'region', $nation , " . $dbh->prepareString($code) . "," . $dbh->prepareString($country) . "," . $dbh->prepareString($city) . ",$currency,$skinId,$time,".$dbh->prepareString($accessType).",".$dbh->prepareString($childsLimit).",".$dbh->prepareString($s_processors).")";
        $ret=$dbh->exec($sql);

        $casino_db_trans = false;
        $betting_db_trans = false;
        if ( !$ret ) {
            addError("", $lang->getLang('Database Error - Could not create jurisdiction record'));
            $casino_db_trans = false;
        }else{
            if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
                $result = save_betting_jurisdiction($next_id,$name,$nation,"region");
                if($result){
                    $perc_res = savePercentuals($_POST["percentuals"], $next_id, $nation,$_POST["betting"],$_POST['betting_live']);
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
                $perc_res = savePercentuals($_POST["percentuals"], $next_id, $nation,$_POST["betting"],$_POST['betting_live']);
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
                                        SELECT '.$next_id.', SCN_GAMES_INFO, SCN_MENU_INFO, SCN_SKIN_PROPERTIES,SCN_MOBILE_GAMES_INFO, SCN_MOBILE_MENU_INFO,SCN_MOBILE_SKIN_PROPERTIES 
                                        FROM jurisdiction_configuration
                                        WHERE SCN_JUR_ID = '.$nation);
            $dbh->commit();
            if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting") betting_commit();
            return $next_id;
        }
        else{
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


function update_region($id, $name,$nation,$currency,$country,$city, $childsLimit,$accessType,$u_processors){
    global $dbh,$lang;
    // get current record
    if ( isBlank($name) ) {
        addError("", $lang->getLang("Please enter a valid name"));
    }
    $name=trim($name);
    global $dbh,$lang;
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
    if ($childsLimit <= 0) {
        $sql = "SELECT * FROM jurisdiction j WHERE jur_id = " . $id . " ";
        $rs  = $dbh->exec($sql);
        $row = $rs->next();
        $childsLimit = $row['jur_childs_limit'];
    }
    if ( !$name ){
        addError('jurisdiction_name',  $lang->getLang('A region name must be entered'));
        return FALSE;
    }else{
        $sql = "SELECT count(*) FROM jurisdiction WHERE LOWER(jur_name) = LOWER('$name') and jur_id != $id";
        $count = $dbh->countQuery($sql);
        if ( $count > 0 ){
            addError('jurisdiction_name', $lang->getLang("The region name you entered already exists as the name of a region, district or club"));
        }
    }
    $sql="SELECT * FROM jurisdiction_payment_settings WHERE JPS_JUR_ID='$nation' ";
    $parentResult = $dbh->queryRow($sql);
    if(!$parentResult){
        addError("percetage", $lang->getLang("You have to set the percentage of the district before to go forward"));
    }
    require_once "funclib/percentual.inc.php";
    if(!checkPercentualsCasino()){
        addError("percetage", $lang->getLang("You have to set the percentage fields to save the new %", "region"));
    }
    if(!checkPercentualsFields()){
        addError("percetage", $lang->getLang("You have to set the percentage fields to save the new %", "region"));
    }
    if ( ! areErrors() ){
        $dbh->begin();
        if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
            betting_begin();
        }

        $currency = $dbh->queryOne('SELECT jur_currency from jurisdiction where jur_id='.$dbh->escape($nation));

        if(!is_numeric($currency)){
            $currency=1;
        }
        $skinId = $dbh->queryOne('SELECT jur_skn_id from jurisdiction where jur_id='.$dbh->escape($nation));
        if(!is_numeric($skinId)){
            $skinId='NULL';
        }
        
        $time= $dbh->queryOne('SELECT jur_time_utc from jurisdiction where jur_id='.$dbh->escape($nation));
        $sql = " UPDATE jurisdiction set jur_name = " . $dbh->prepareString($name) . "
        , jur_parent_id= $nation
        , jur_cou_id = " . $country .
        ", jur_city =".$dbh->prepareString($city).
        ", jur_currency =$currency".
        ",jur_time_utc =".$time.
        ", jur_childs_limit = " . $childsLimit .
        ",jur_skn_id =".$dbh->prepareString($skinId).
        ",jur_processor_enabled =".$dbh->prepareString($u_processors).
        ",jur_access_type =".$dbh->prepareString(strtoupper($accessType)).
        " WHERE jur_id = $id";
        
        $ret = $dbh->exec($sql);
        $betting_db_trans = false;
        $casino_db_trans = false;
        $update=array('jur_currency'=>intval($currency),
                      'jur_time_utc'=>intval($time),
                      'jur_skn_id'=>  intval($skinId)
            );
        
        $hierarchyUpdated=updateAllHierarchy($id,'region',$update);
        
        $hierarchyUpdatedPunterSkin=updateAllHierarchy($id,'region','','pun_site_id',$skinId);
        
        if ( !$ret || !$hierarchyUpdated ) {
            addError("", $lang->getLang('Database Error - Could not create jurisdiction record'));
            $casino_db_trans = false;
        }else{
            if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
                $result = update_betting_jurisdiction($id,$name,$nation,"region");
                if($result){
                    $perc_res = savePercentuals($_POST["percentuals"], $id, $nation,$_POST["betting"],$_POST['betting_live']);
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
                $perc_res = savePercentuals($_POST["percentuals"], $id, $nation,$_POST["betting"],$_POST['betting_live']);
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
        dieWithError("Jurisdiction not found");
    }
}
$delete_cache = false;
/*addedd cache*/
include "JurisdictionsList.class.inc";
$jur_list = JurisdictionsList::getInstance($dbh);

switch ( $op ){
    case "update_subjur_processors":
        $u_processors = implode("~",$_POST['processors']);
        $hierarchyUpdated=updateAllProcSubjur($node,'region',$u_processors);
        echo "success";
        exit;
        break;
    case "Add": //display add new task screen
        $display =  "region_add_form.php";
        break;
    case "Save":  //save a new task
        $s_processors = implode("~",$_POST['processors']);
        update_user_limit_childs($_SESSION['jurisdiction_id']);
        $node = save_region($_POST['region_name'], $_POST['account_code'],$_POST['nation_id'],$_POST['country'],$_POST['city'],strtoupper($_POST['accessType']),$_POST['childs_limit'],$_POST['skin'],$s_processors);
        if ( ! empty($node) ){
            $display = 'region_display_form.php'; //display new record
            $region  = $_POST['region_name'];
            $alert   = "Region \"$region\" saved";
            $delete_cache = true;

        }
        else{
            $display = "region_add_form.php"; //if error, display add form again
        }
        break;
    case "Update":  //update an existing task
        $u_processors = implode("~",$_POST['processors']);
        $updated = update_region($node, $_POST['region_name'],$_POST['nation_id'],$_POST['currency'],$_POST['country'],$_POST['city'],$_POST['childs_limit'],strtoupper($_POST['accessType']),$u_processors);

        if ( $updated ){
            $jur_list->destroyParentsJurisdictionsCache($node);
            $region  = $_POST['region_name'];
            $alert   = $lang->getLang("Region '%' updated",$region);
        }
        $sql = "SELECT * FROM jurisdiction  WHERE jur_id = " . $node;

        $rs = $dbh->exec($sql);
        $num_rows = $rs->getNumRows();

        if ( 1 == $num_rows ){
            $row = $rs->next();
            $account_code      = $row['jur_code'];
            $nation            = $row['jur_parent_id'];
            $jurChildsLimit     = $row['jur_childs_limit'];
            $u_processors = explode('~',$row['jur_processor_enabled']);
        }

        $delete_cache = true;


        $display =  'region_display_form.php';
        break;
    /*addedd cache*/


    case "Delete":
        $sql = "UPDATE jurisdiction SET jur_status = 0 WHERE jur_id = $node";
        $rs  = $dbh->exec($sql);
        if($rs == 1){
            $delete_cache = true;
        }
        $alert = $lang->getLang("% blocked","region ".$node)."!</br>";

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
        $sql = "UPDATE jurisdiction SET jur_status = 3 WHERE jur_status = 1 AND (" . implode(" OR ", $jur_arr) . ") AND jur_id != $node";
        //die($sql);
        $rs  = $dbh->exec($sql);
        $locked_jurs = 1 + $dbh->getAffectedRows();
        //die($dbh->getAffectedRows());
        $alert .= $lang->getLang("% blocked children jurisdictions", $locked_jurs)."!</br>";
        //-------------------------


        if($_GET["lock_users"] == "true"){
            $sql = "UPDATE admin_user SET aus_access = 'LOCK', aus_notes = 'Locked by admin'";
            if(count($aus_arr) > 0){
                $sql .= " WHERE aus_access = 'ALLOW' AND (" . implode(" OR ", $aus_arr) . ")";
                $rs  = $dbh->exec($sql);
                $locked_users += $dbh->getAffectedRows();
            }
            $alert .= " " . $lang->getLang("and % locked: %",$lang->getLang("administrators"), $locked_users)."!</br>";
        }
        if($_GET["lock_punters"] == "true"){
            $sql = "UPDATE punter SET pun_access = 'deny', pun_notes = 'Locked by admin'";
            if(count($pun_arr) > 0){
                $sql .= " WHERE (" . implode(" OR ", $pun_arr) . ")";
                $rs  = $dbh->exec($sql);
                $lock_punters += $dbh->getAffectedRows();
            }
            $alert .= " " . $lang->getLang("and % locked: %",$lang->getLang("players"), $lock_punters);
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
        $alert = $lang->getLang("% restored","region ".$node)."!</br>";

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
        $sql = "UPDATE jurisdiction SET jur_status = 1 WHERE jur_status = 3 AND (" . implode(" OR ", $jur_arr) . ") AND jur_id != $node";
        $rs  = $dbh->exec($sql);
        $locked_jurs = 1 + $dbh->getAffectedRows();
        $alert .= $lang->getLang("% unblocked children jurisdictions", $locked_jurs)."!</br>";
        //-------------------------


        if($_GET["unlock_users"] == "true"){
            $sql = "UPDATE admin_user SET aus_access = 'ALLOW', aus_notes = 'Unlocked by admin'";
            if(count($aus_arr) > 0){
                $sql .= " WHERE (" . implode(" OR ", $aus_arr) . ")";
                $rs  = $dbh->exec($sql);
                $locked_users += $dbh->getAffectedRows();
            }
            $alert .= " " . $lang->getLang("and % unlocked: %",$lang->getLang("administrators"), $locked_users)."!</br>";
        }
        if($_GET["unlock_punters"] == "true"){
            $sql = "UPDATE punter SET pun_access = 'allow', pun_notes = 'Unocked by admin'";
            if(count($pun_arr) > 0){
                $sql .= " WHERE (" . implode(" OR ", $pun_arr) . ")";
                $rs  = $dbh->exec($sql);
                $lock_punters += $dbh->getAffectedRows();
            }
            $alert .= " " . $lang->getLang("and % unlocked: %",$lang->getLang("players"), $lock_punters);
        }
        $alert .= "!";
        doAdminUserLog($_SESSION['admin_id'], 'unblock_jurisdiction', escapeSingleQuotes($alert));
        break;
    /*addedd cache*/
    default:
        $display =  'region_display_form.php';
}

if($delete_cache){
    $jur_list->destroyParentsJurisdictionsCache($node);
}

if ( 'region_display_form.php' == $display ){
    if ( $_POST['op'] ){
        if ('Update' != $op ){
            $account_code = $_POST['account_code'];
        }
        $region_name  = $_POST['region_name'];
        $parent_id    = $_POST['nation_id'];
        $country      = $_POST['country'];
        $city         = $_POST['city'];
        $accessType   = $_POST['accessType'];
        $skinId = $dbh->queryOne('SELECT jur_skn_id from jurisdiction where jur_id='.$dbh->escape($node));
        $sql = "SELECT jur_name, jur_code,jur_status, jur_parent_id,jur_cou_id,jur_city,jur_creation_date,jur_time_utc,jur_available_credit,jur_childs_limit,jur_processor_enabled FROM jurisdiction WHERE jur_id = " . $node;
        $rs  = $dbh->exec($sql);
        $num_rows = $rs->getNumRows();
        if ( 1 == $num_rows ){
            $row = $rs->next();
            $jur_status   = $row['jur_status'];
            $jurChildsLimit     = $row['jur_childs_limit'];
            $u_processors = explode('~',$row['jur_processor_enabled']);
        }
    }
    else{
        // don't query twice
        if ( ! $delete_failed ){
            $sql = "SELECT jur_name, jur_code,jur_status, jur_parent_id,jur_cou_id,jur_city,jur_creation_date,jur_time_utc,jur_available_credit,jur_access_type,jur_childs_limit,jur_processor_enabled FROM jurisdiction WHERE jur_id = " . $node;
            $rs  = $dbh->exec($sql);
            $num_rows = $rs->getNumRows();
            if ( 1 == $num_rows ){
                $row = $rs->next();
                $region_name  = $row['jur_name'];
                $account_code = $row['jur_code'];
                $parent_id    = $row['jur_parent_id'];
                $jur_status   = $row['jur_status'];
                $creationTime = $row['jur_creation_date'];
                $country      = getCountryCode2Chars($row['jur_cou_id']);
                $city         = $row['jur_city'];
                $accessType   = $row['jur_access_type'];
                $skinId       = $row['jur_skn_id'];
                $jurChildsLimit     = $row['jur_childs_limit'];
                $u_processors = explode('~',$row['jur_processor_enabled']);
            }
        }
    }
}

if ( $display ){
    require_once($display);
}
else{
    echo $lang->getLang("Please select from the menu on the left");
}

if ( isset($alert) )
    jAlert(mb_ereg_replace('"', '\"', $alert));
?>
