<?php
global $dbh;
check_access("manage_jurisdictions", true);
$dbh->exec("SET character_set_results = 'utf8', character_set_client = 'utf8', character_set_connection = 'utf8', character_set_database = 'utf8', character_set_server = 'utf8'",false,true);
mysql_set_charset('utf8');
function save_nation($name, $code,$currency,$country,$city,$skinId,$time,$accessType, $childsLimit,$s_processors) {

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

    if (!$name) {
        addError('jurisdiction_name', $lang->getLang("A Nation name must be entered"));
    }
    else {
        $sql = "SELECT count(*) FROM jurisdiction WHERE LOWER(jur_name) = LOWER('$name')";
        $count = $dbh->countQuery($sql);
        if ($count > 0) {
            addError('jurisdiction_name', $lang->getLang("The Nation name you entered already exists as the name of a region, district or club"));
            return FALSE;
        }
    }

    if (isBlank($code)) {
        addError("account_code", $lang->getLang("An account code must be entered"));
    }
    elseif (mb_strlen($code) != 6 || !mb_ereg_match("^[A-Za-z]*$", $code)) {
        addError("account_code", $lang->getLang("The account code has to contain only 6 alphabetical characters"));
    } else {
        $sql = "SELECT count(*) FROM jurisdiction WHERE LOWER(jur_code) = " . $dbh->prepareString(strtolower($code));
        $count = $dbh->countQuery($sql);
        if ($count > 0) {
            addError("account_code", $lang->getLang("The account code you entered already exists for another jurisdiction."));
        }
    }

    require_once "funclib/percentual.inc.php";
    if(!checkPercentualsCasino()){
        addError("percetage", $lang->getLang("You have to set the percentage fields to save the new %", "nation"));
    }
    if(!checkPercentualsFields()){
        addError("percetage", $lang->getLang("You have to set the percentage fields to save the new %", "nation"));
    }
    if (!areErrors()) {
        $dbh->begin();
        if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting") betting_begin();
        //$currency=($currency=="")?1:$currency;
        $next_id = $dbh->nextSequence('JUR_ID_SEQ');
        $sql = "INSERT INTO jurisdiction (jur_id, jur_name, jur_class, jur_parent_id, jur_code, jur_currency,jur_cou_id,jur_city,jur_skn_id,jur_time_utc,jur_access_type, jur_childs_limit, jur_processor_enabled)" .
            "  VALUES (" . $next_id . ", " . $dbh->prepareString($name) . ", 'nation', 1, " . $dbh->prepareString($code) . ",".$dbh->escape($currency)."," . $dbh->prepareString($country) . "," . $dbh->prepareString($city) . ",".$dbh->escape($skinId).",".$dbh->escape($time).",".$dbh->prepareString($accessType).",".$dbh->prepareString($childsLimit).",".$dbh->prepareString($s_processors).")";
        $ret = $dbh->exec($sql);
        $casino_db_trans = false;
        $betting_db_trans = false;
        if ( !$ret ) {
            addError("", $lang->getLang('Database Error - Could not create jurisdiction record'));
            $casino_db_trans = false;
        }else{
            if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
                $result = save_betting_jurisdiction($next_id,$name,1,"nation");
                if($result){
                    $perc_res = savePercentuals($_POST["percentuals"], $next_id, 1,$_POST["betting"],$_POST['betting_live']);
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
                $perc_res = savePercentuals($_POST["percentuals"], $next_id, 1,$_POST["betting"],$_POST['betting_live']);
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
           $menu_info='';
            $skin_properties='';
            $games_info='';
      /*      $menu_info=file_get_contents('../../config/menu.ini');
            $skin_properties=file_get_contents('../../config/skinsproperties.ini');
            $dbh->exec('SET SESSION group_concat_max_len = 4000000');
            $games_info=$dbh->queryOne("select concat ('[slots]\n\n', 
            coalesce((select GROUP_CONCAT('\tgames[] = ', ggi_button_image_name, '@', ggi_game_id, '@', ggi_width, '@', ggi_height, '@', ggi_pes_group, '@', ggi_has_forfun, '@', ggi_web_name SEPARATOR '\n')
            from game_global_info
            where ggi_category = 'slots' and (ggi_pes_group is null or ggi_pes_group = '0' or ggi_pes_group = '')), ''),
            '\n\n[video poker]\n\n',
            coalesce((select GROUP_CONCAT('\tgames[] = ', ggi_button_image_name, '@', ggi_game_id, '@', ggi_width, '@', ggi_height, '@', ggi_pes_group, '@', ggi_has_forfun, '@', ggi_web_name SEPARATOR '\n')
            from game_global_info
            where ggi_category = 'video poker' and (ggi_pes_group is null or ggi_pes_group = '0' or ggi_pes_group = '')), ''),
            '\n\n[table games]\n\n',
            coalesce((select GROUP_CONCAT('\tgames[] = ', ggi_button_image_name, '@', ggi_game_id, '@', ggi_width, '@', ggi_height, '@', ggi_pes_group, '@', ggi_has_forfun, '@', ggi_web_name SEPARATOR '\n')
            from game_global_info
            where ggi_category = 'table games' and (ggi_pes_group is null or ggi_pes_group = '0' or ggi_pes_group = '')),''),
            '\n\n[live casino]\n\n',
            coalesce((select GROUP_CONCAT('\tgames[] = ', ggi_button_image_name, '@', ggi_game_id, '@', ggi_width, '@', ggi_height, '@', ggi_pes_group, '@', ggi_has_forfun, '@', ggi_web_name SEPARATOR '\n')
            from game_global_info
            where ggi_category = 'live casino' and (ggi_pes_group is null or ggi_pes_group = '0' or ggi_pes_group = '')), ''),
            '\n\n[common draw]\n\n',
            coalesce((select GROUP_CONCAT('\tgames[] = ', ggi_button_image_name, '@', ggi_game_id, '@', ggi_width, '@', ggi_height, '@', ggi_pes_group, '@', ggi_has_forfun, '@', ggi_web_name SEPARATOR '\n')
            from game_global_info
            where ggi_category = 'common draw' and (ggi_pes_group is null or ggi_pes_group = '0' or ggi_pes_group = '')), ''),
            '\n\n[dice games]\n\n',
            coalesce((select GROUP_CONCAT('\tgames[] = ', ggi_button_image_name, '@', ggi_game_id, '@', ggi_width, '@', ggi_height, '@', ggi_pes_group, '@', ggi_has_forfun, '@', ggi_web_name SEPARATOR '\n')
            from game_global_info
            where ggi_category = 'dice games' and (ggi_pes_group is null or ggi_pes_group = '0' or ggi_pes_group = '')), ''),
            '\n\n[virtual]\n\n',
            coalesce((select GROUP_CONCAT('\tgames[] = ', ggi_button_image_name, '@', ggi_game_id, '@', ggi_width, '@', ggi_height, '@', ggi_pes_group, '@', ggi_has_forfun, '@', ggi_web_name SEPARATOR '\n')
            from game_global_info
            where ggi_category = 'virtual' and (ggi_pes_group is null or ggi_pes_group = '0' or ggi_pes_group = '')), '')
            ) as game_info");*/

            $generalInfo=$dbh->exec('INSERT INTO jurisdiction_configuration (SCN_JUR_ID, SCN_GAMES_INFO, SCN_MENU_INFO, SCN_SKIN_PROPERTIES)
                              VALUES('.$next_id.','.$dbh->prepareString($games_info).','.$dbh->prepareString($menu_info).','.$dbh->prepareString($skin_properties).')');


          /*  var_dump('INSERT INTO jurisdiction_configuration (SCN_JUR_ID, SCN_GAMES_INFO, SCN_MENU_INFO, SCN_SKIN_PROPERTIES)
                              VALUES('.$next_id.','.$dbh->prepareString($games_info).','.$dbh->prepareString($menu_info).','.$dbh->prepareString($skin_properties).')');
       */

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

function update_nation($id, $name, $currency,$country,$city,$accessType, $childsLimit,$skinId=false,$time=false,$u_processors) {

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
    $sql = "SELECT * FROM jurisdiction j WHERE jur_id = " . $id . " ";
    $rs  = $dbh->exec($sql);
    $current_nation = $rs->next();
    if (!$name) {
        addError('jurisdiction_name', $lang->getLang('A Nation name must be entered'));
        return FALSE;
    } else {
        $sql = "SELECT count(*) FROM jurisdiction WHERE LOWER(jur_name) = LOWER('$name') and jur_id != $id";
        $count = $dbh->countQuery($sql);
        if ($count > 0) {
            addError('jurisdiction_name', $lang->getLang("The Nation name you entered already exists as the name of a region, district or club"));
        }
    }

    require_once "funclib/percentual.inc.php";
    if(!checkPercentualsCasino()){
        addError("percetage", $lang->getLang("You have to set the percentage fields to save the new %", "nation"));
    }
    if(!checkPercentualsFields()){
        addError("percetage", $lang->getLang("You have to set the percentage fields to save the new %", "nation"));
    }
    if (!areErrors()) {
        $dbh->begin();
        if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting") betting_begin();
        $sql = "UPDATE jurisdiction  set jur_name = " . $dbh->prepareString($name) .
            ", jur_currency=".$dbh->escape($currency)."
            , jur_cou_id = " . $country .
            ", jur_access_type = " . $dbh->prepareString(strtoupper($accessType)) .
            ", jur_childs_limit = " . $childsLimit .
            ", jur_processor_enabled = " . $dbh->prepareString($u_processors) .
            ", jur_city =".$dbh->prepareString($city);
        if($_SESSION['aty_code'] == "SUPERUSER"){
            $sql.=" ,jur_time_utc =".$dbh->escape($time);
        }
        $update=array('jur_currency'=>intval($currency),
            'jur_time_utc'=>intval($time));

        if(isset($skinId) && $skinId!==$current_nation['jur_skn_id']){
            $sql.=",jur_skn_id=".$dbh->escape($skinId);
            $update['jur_skn_id']=(int)$skinId;
            updateAllHierarchy($id,'nation',false,'pun_site_id',(int)$skinId);
        }

        $sql.=" WHERE jur_id = $id";
        $ret = $dbh->exec($sql);
// 		$betting_db_trans = false;
        $casino_db_trans = false;


        $hierarchyUpdated=updateAllHierarchy($id,'nation',$update);
        if ( !$ret || !$hierarchyUpdated) {
            addError("", $lang->getLang('No update was made'));
            $casino_db_trans = false;
        }else{


            if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
                $result = update_betting_jurisdiction($id,$name,1,"nation");
                if($result){
                    $perc_res = savePercentuals($_POST["percentuals"], $id, 1,$_POST["betting"],$_POST['betting_live']);
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

                $perc_res = savePercentuals($_POST["percentuals"], $id, 1,$_POST["betting"],$_POST['betting_live']);
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

$op = (!empty ($_POST['op']) ? $_POST['op'] : $_GET['op']);
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

include "JurisdictionsList.class.inc";
$jur_list = JurisdictionsList::getInstance($dbh);


switch ($op) {
    case "update_subjur_processors":
        $u_processors = implode("~",$_POST['processors']);
        $hierarchyUpdated=updateAllProcSubjur($node,'nation',$u_processors);
        ob_end_clean();
        echo "success";
        exit;
        break;
    case "Add" : //display add new task screen
        $display = "nation_add_form.php";
        break;
    case "Save" : //save a new task
        $s_processors = implode("~",$_POST['processors']);
        $node = save_nation($_POST['nation_name'], $_POST['account_code'],$_POST['currency'],$_POST['country'],$_POST['city'],$_POST['skin'],$_POST['time'],strtoupper($_POST['accessType']),$_POST['childs_limit'],$s_processors);
        
        if (!empty ($node)) {
            update_user_limit_childs($_SESSION['jurisdiction_id']);
            $display = 'nation_display_form.php'; //display new record
            $nation = $_POST['nation_name'];
            $alert = $lang->getLang("Nation '%' saved",$nation);
            $delete_cache = true;
        } else {
            $display = "nation_add_form.php"; //if error, display add form again
        }
        break;
    case "Update" : //update an existing task
        $u_processors = implode("~",$_POST['processors']);
        $updated = update_nation($node, $_POST['nation_name'],$_POST['currency'],$_POST['country'],$_POST['city'],$_POST['accessType'],$_POST['childs_limit'],$_POST['skin'],$_POST['time'],$u_processors);
        if ($updated) {
            $jur_list->destroyParentsJurisdictionsCache($node);
            $nation = $_POST['nation_name'];
            $alert = $lang->getLang("Nation '%' updated",$nation);
        }

        $sql = "SELECT * FROM jurisdiction WHERE jur_id = " . $node;

        $rs = $dbh->exec($sql);
        $num_rows = $rs->getNumRows();
        if (1 == $num_rows) {
            $row = $rs->next();
            $nation_name  = $row['jur_name'];
            $account_code = $row['jur_code'];
            $currency     = $row['jur_currency'];
            $time         = $row['jur_time_utc'];
            $jurChildsLimit     = $row['jur_childs_limit'];
            $u_processors = explode('~',$row['jur_processor_enabled']);

        }

        $delete_cache = true;
        $display = 'nation_display_form.php';
        break;
    /*addedd cache*/
    case "Delete":
        $sql = "UPDATE jurisdiction SET jur_status = 0 WHERE jur_id = $node";
        $rs  = $dbh->exec($sql);
        if($rs == 1){
            $delete_cache = true;
        }
        $alert = $lang->getLang("% blocked","nation ".$node)."!</br>";

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
        $sql = "UPDATE jurisdiction SET jur_status = 4 WHERE jur_status = 1 AND (" . implode(" OR ", $jur_arr) . ") AND jur_id != $node";
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
            $alert .= $lang->getLang("and % locked: %",$lang->getLang("administrators"), $locked_users)."!</br>";
        }
        if($_GET["lock_punters"] == "true"){
            $sql = "UPDATE punter SET pun_access = 'deny', pun_notes = 'Locked by admin'";
            if(count($pun_arr) > 0){
                $sql .= " WHERE (" . implode(" OR ", $pun_arr) . ")";
                $rs  = $dbh->exec($sql);
                $lock_punters += $dbh->getAffectedRows();
            }
            $alert .= $lang->getLang("and % locked: %",$lang->getLang("players"), $lock_punters);
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
        $alert = $lang->getLang("% restored","nation ".$node)."!</br>";

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
        $sql = "UPDATE jurisdiction SET jur_status = 1 WHERE jur_status = 4 AND (" . implode(" OR ", $jur_arr) . ") AND jur_id != $node";
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
            $alert .= $lang->getLang("and % unlocked: %",$lang->getLang("administrators"), $locked_users)."!</br>";
        }
        if($_GET["unlock_punters"] == "true"){
            $sql = "UPDATE punter SET pun_access = 'allow', pun_notes = 'Unocked by admin'";
            if(count($pun_arr) > 0){
                $sql .= " WHERE (" . implode(" OR ", $pun_arr) . ")";
                $rs  = $dbh->exec($sql);
                $lock_punters += $dbh->getAffectedRows();
            }
            $alert .= $lang->getLang("and % unlocked: %",$lang->getLang("players"), $lock_punters);
        }
        $alert .= "!";
        doAdminUserLog($_SESSION['admin_id'], 'unblock_jurisdiction', escapeSingleQuotes($alert));
        break;
    /*addedd cache*/
    default :
        $display = 'nation_display_form.php';
}

if($delete_cache){
    $jur_list->destroyParentsJurisdictionsCache($node);
}

if ('nation_display_form.php' == $display) {
    if ($_POST['op']) {
        $nation_name = $_POST['nation_name'];
        $country      = $_POST['country'];
        $city         = $_POST['city'];
        $currency     = $_POST['currency'];
        $time         = $_POST['time'];
        $accessType   = $_POST['accessType'];
        $skinId = $dbh->queryOne('SELECT jur_skn_id from jurisdiction where jur_id='.$dbh->escape($node));
        if ('update' != strtolower($op)) {
            $account_code = $_POST['account_code'];

        }

        $sql = "SELECT * FROM jurisdiction WHERE jur_id = " . $node;
        $rs = $dbh->exec($sql);
        $num_rows = $rs->getNumRows();
        if (1 == $num_rows) {
            $row = $rs->next();
            $jur_status   = $row['jur_status'];
            $jurChildsLimit     = $row['jur_childs_limit'];
        }
    } else {

        // don't query twice
        if (!$delete_failed) {
            $sql = "SELECT * FROM jurisdiction WHERE jur_id = " . $node;
            $rs = $dbh->exec($sql);
            $num_rows = $rs->getNumRows();
            if (1 == $num_rows) {
                $row = $rs->next();
                $nation_name  = $row['jur_name'];
                $account_code = $row['jur_code'];
                $currency     = $row['jur_currency'];
                $jur_status   = $row['jur_status'];
                $creationTime = $row['jur_creation_date'];
                $country      = getCountryCode2Chars($row['jur_cou_id']);
                $city         = $row['jur_city'];
                $time         = $row['jur_time_utc'];
                $accessType   = $row['jur_access_type'];
                $skinId       = $row['jur_skn_id'];
                $jurChildsLimit     = $row['jur_childs_limit'];
                $u_processors = explode('~',$row['jur_processor_enabled']);
            }
        }
    }
}

if ($display) {
    require_once ($display);
} else {
    echo $lang->getLang("Please select from the menu on the left");
}

if (isset ($alert))
    jAlert(mb_ereg_replace('"', '\"', $alert));

?>
