<?php
function update_club  ($id, $name, $address1, $address2, $district_id, $postc, $phone, $notes, $has_live,$currency,$country,$city,$accessType,$longitude,$latitude,$startHours,$endHours,$params,$jurUsersLimit,$u_processors){
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
    $sql = "SELECT * FROM jurisdiction WHERE jur_id = " . $id;
    $current_club = $dbh->queryRow($sql);
    unset($rs);

    if ( isBlank($name) ) {
        addError("", $lang->getLang("A Betting Club name must be entered"));
    } else {
        $sql = "SELECT count(*) FROM jurisdiction WHERE jur_name = " . $dbh->prepareString($name) . " and jur_id != $id AND lower(jur_name) != 'internet'";
        $count = $dbh->countQuery($sql);
        if ( $count > 0 ) {
            addError('club_name',$lang->getLang('The Betting Club name you entered already exists as the name of a region, district or club'));
        }
    }

    if ( isBlank($address1) )
        addError("", $lang->getLang("An address (line 1) must be entered"));
    if ( isBlank($district_id) ) {
        addError("", $lang->getLang("A district must be selected"));
    } else {
        $sql = "SELECT count(*) FROM jurisdiction WHERE jur_id = $district_id AND jur_class = 'district'";
        $valid_district_count = $dbh->countQuery($sql);
        if ( $valid_district_count != 1  ) {
            addError('district', $lang->getLang("Invalid District"));
        }
    }

    if ( isBlank($postc) )
        addError("", $lang->getLang("Blank Postcode, please insert one"));

    if ( isBlank($phone) )
        addError("", $lang->getLang('A phone number must be entered'));

    $has_live = (($has_live == true) ? "1" : "0");
    if ( isBlank($startHours) )
        $startHours="00:00";

    if ( isBlank($endHours) )
        $endHours="00-00";

    $hours=$startHours."-".$endHours;
    if($latitude&$longitude){
        $localization=$latitude.';'.$longitude;
    }else{
        $localization='';
    }
    $currency = 1;

    $sql="SELECT * FROM jurisdiction_payment_settings WHERE JPS_JUR_ID='$district_id' ";
    $parentResult = $dbh->queryRow($sql);
    if(!$parentResult){
        addError("percetage", $lang->getLang("You have to set the percentage of the district before to go forward"));
    }
    require_once "funclib/percentual.inc.php";
    if(!checkPercentualsCasino()){
        addError("percetage", $lang->getLang("You have to set the percentage fields to save the new %", "club"));
    }
    if(!checkPercentualsFields()){
        addError("percetage", $lang->getLang("You have to set the percentage fields to save the new %", "club"));
    }
    if ( ! areErrors() ){
        $dbh->begin();
        if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting") betting_begin();
        // Update club with new details
        $currency = $dbh->queryOne('SELECT jur_currency from jurisdiction where jur_id='.$dbh->escape($district_id));
        if(!is_numeric($currency) || $currency==''){
            $currency=1;
        }
        $skinId = $dbh->queryOne('SELECT jur_skn_id from jurisdiction where jur_id='.$dbh->escape($district_id));
        if(!is_numeric($skinId)){
            $skinId='NULL';
        }
        $timeZone= $dbh->queryOne('SELECT jur_time_utc from jurisdiction where jur_id='.$dbh->escape($district_id));

        $sql = "UPDATE jurisdiction" .
            " SET jur_name  = " . db_quote($name) .
            ", jur_address1 = " . db_quote($address1) .
            (!empty($address2) ? ", jur_address2 = " . db_quote($address2)  : ", jur_address2 = NULL") .
            ", jur_parent_id = " . $district_id .
            ", jur_postcode_zip = " . db_quote($postc) .
            ", jur_phone = " . db_quote($phone) .
            ", jur_notes = " . db_quote($notes) .
            ", jur_has_live_games = " . $has_live .
            ", jur_currency =".$currency.
            ", jur_cou_id = " . $country .
            ", jur_city =".$dbh->prepareString($city).
            ", jur_skn_id =".$skinId.
            ", jur_users_limit =" . $jurUsersLimit .
            ", jur_time_utc =".$dbh->prepareString($timeZone).
            ", jur_access_type =".$dbh->prepareString(strtoupper($accessType)).
            ", jur_agency_hour =".$dbh->prepareString(strtoupper($hours)).
            ", jur_google_map_info =".$dbh->prepareString(strtoupper($localization)).
            ", jur_processor_enabled =".$dbh->prepareString($u_processors).
            ", jur_params =".$dbh->prepareString($params).
            " WHERE jur_id = " . $id;
        $ret=$dbh->exec($sql);
        $betting_db_trans = false;
        $casino_db_trans = false;
        $update=array('jur_currency'=>intval($currency),
            'jur_time_utc'=>intval($timeZone),
            'jur_skn_id'=>  intval($skinId)
        );

        $hierarchyUpdated=updateAllHierarchy($id,'club',$update);
        updateAllHierarchy($id,'club',false,'pun_site_id',$skinId);
        if ( !$ret ) {
            addError("", $lang->getLang('Database Error - Could not create jurisdiction record'));
            $casino_db_trans = false;
        }else{
            if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
                $result = update_betting_jurisdiction($id,$name,$district_id,"club");
                if($result){
                    $perc_res = savePercentuals($_POST["percentuals"], $id, $district_id,$_POST["betting"],$_POST['betting_live']);
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
                $perc_res = savePercentuals($_POST["percentuals"], $id, $district_id,$_POST["betting"],$_POST['betting_live']);
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
            if( $current_club['jur_name'] != $name)
                $log = 'jur_name:' . $current_club['jur_name'] . '=' . $name . '|';
            if( $current_club['jur_address1'] != $address1)
                $log .= 'jur_address1:' . $current_club['jur_address1'] . '=' . $address1 . '|';
            if( $current_club['jur_address2'] != $address2)
                $log .= 'jur_address2:' . $current_club['jur_address2'] . '=' . $address2 . '|';
            if( $current_club['jur_parent_id'] != $city)
                $log .= 'jur_parent_id:' . $current_club['jur_parent_id'] . '=' . $district_id . '|';
            if( $current_club['jur_postcode_zip'] != $postc)
                $log .= 'jur_postcode_zip:' . $current_club['jur_postcode_zip'] . '=' . $postc . '|';
            if( $current_club['jur_phone'] != $phone)
                $log .= 'jur_phone:' . $current_club['jur_phone'] . '=' . $phone . '|';
            if( $current_club['jur_notes'] != $notes)
                $log .= 'jur_notes:' . $current_club['jur_notes'] . '=' . $notes . '|';
            if( $current_club['jur_has_live_games'] != $has_live)
                $log .= 'jur_has_live_games:'. $current_club['jur_has_live_games'] . '=' . $has_live . '|';

            if($log) {
                $log = substr($log, 0, -1);
                doAdminUserLog($_SESSION['admin_id'], 'modify_betting_club', escapeSingleQuotes($log), $pun_id="NULL");
            }



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

function save_club  ($name, $code, $address1, $address2, $district_id, $postc, $phone, $notes, $has_live,$currency, $vatCode=null,$country,$city,$accessType,$longitude,$latitude,$startHours,$endHours,$params, $jurUsersLimit,$u_processors){
    global $dbh , $lang;
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
    if ( isBlank($name) ) {
        addError("", $lang->getLang("A Betting Club name must be entered"));
    } else {
        $sql = "SELECT count(*) FROM jurisdiction WHERE LOWER(jur_name) = LOWER(" . $dbh->prepareString($name) . ") AND lower(jur_name) != 'internet'";
        $count = $dbh->countQuery($sql);
        if ( $count > 0 ) {
            addError('club_name', $lang->getLang("The Betting Club name you entered already exists as the name of a region, district or club"));
        }
    }
    if ($jurUsersLimit <= 0) {
        $jurUsersLimit = 30;
    }
    if ($code == "AUTO"){
        $files_path = realpath(WEB_ROOT . "/../cache/clubcodes");
        if(file_exists("$files_path/free_codes.txt")){
            $free_codes = unserialize(file_get_contents("$files_path/free_codes.txt"));
            $idx = 0;
            if(file_exists("$files_path/free_codes.idx")){
                $idx        = file_get_contents("$files_path/free_codes.idx");
            }

            if($idx > count($free_codes)){
                addError("account_code", $lang->getLang("Free code problem - Contact assistance for help"));
            }

            $code  = $free_codes[$idx];
            $_POST['account_code'] = $code;

            $sql   = "SELECT count(*) FROM jurisdiction WHERE LOWER(jur_code) = " . $dbh->prepareString(strtolower($code));
            $count = $dbh->countQuery($sql);
            if ( $count > 0 ) {
                addError("account_code", $lang->getLang("The account code you entered already exists for another jurisdiction"));
            }

            file_put_contents("$files_path/free_codes.idx", ++$idx);

        }else{
            addError("account_code", $lang->getLang("Not found new free codes"));
        }
    }else{
        if ( isBlank($code) ) {
            addError("account_code", $lang->getLang("You have to insert at least one account code"));
        } elseif ( mb_strlen($code) != 6 || ! mb_ereg_match("^[a-zA-Z]*$", $code) ) {
            addError("account_code", $lang->getLang("The account code has to contain only 3 alphabetical characters"));
        } else {
            $sql = "SELECT count(*) FROM jurisdiction WHERE LOWER(jur_code) = " . $dbh->prepareString(strtolower($code));
            $count = $dbh->countQuery($sql);
            if ( $count > 0 ) {
                addError("account_code", $lang->getLang("The Betting Club name you entered already exists as the name of a region, district or club"));
            }
        }
    }

    if ( isBlank($address1) )
        addError("", $lang->getLang("An address (line 1) must be entered"));

    if ( isBlank($district_id) ) {
        addError("", $lang->getLang("A district must be selected"));
    } else {
        $district_id = (int)$district_id;
        $sql = "SELECT count(*) FROM jurisdiction WHERE jur_id = $district_id AND jur_class = 'district'";
        $valid_district_count = $dbh->countQuery($sql);

        if ( $valid_district_count != 1  ) {
            addError('district', $lang->getLang("Invalid District"));
        }
    }

    if ( isBlank($postc) )
        addError("", $lang->getLang("Blank Postcode, please insert one"));

    if ( isBlank($startHours) )
        $startHours="00:00";

    if ( isBlank($endHours) )
        $endHours="00-00";

    $hours=$startHours."-".$endHours;
     if($latitude&$longitude){
         $localization=$latitude.';'.$longitude;
     }else{
         $localization='';
     }
    $has_live = (($has_live == true) ? "1" : "0");

    //------------------
    if(DOT_IT){
        if(isBlank($vatCode)){
            addError("vat_code", $lang->getLang("Insert the VAT"));
        }

        if(!is_numeric($vatCode)){
            addError("vat_code",  $lang->getLang("The VAT has to contain only numbers"));
        }

        if(!areErrors()){
            $sql = "SELECT COUNT(*) FROM jurisdiction WHERE jur_vat_code = " . $dbh->prepareString($vatCode);
            $rs  = $dbh->queryOne($sql);
            $vatCodeExists = ($rs > 1);

            if($vatCodeExists){
                addError("vat_code",  $lang->getLang("Chose another jurisdiction VAT code, code '%' is associated to a club",$vatCode));
            }
        }
        /*
        if(!areErrors()){
          try{
            $url  = 'http://ec.europa.eu/taxation_customs/vies/api/checkVatPort?wsdl';
            $vies = new SoapClient($url, array('connection_timeout'=>10));
            $a    = array(
            countryCode => "IT",
            vatNumber => $vatCode
            );

            $ret  = $vies->checkVat($a);

            if($ret->valid != "1"){
              addError("jur_vat_code", "Invalid VAT code");
            }
          }catch (Exception $e){
            addError("jur_vat_code", "Problem contacting central database of VAT code, retry in a minute, <!--" . $e->getMessage() . "-->");
          }
        }
        */
    }
    //------------------
    $sql="SELECT * FROM jurisdiction_payment_settings WHERE JPS_JUR_ID='$district_id' ";
    $parentResult = $dbh->queryRow($sql);
    if(!$parentResult){
        addError("percetage", $lang->getLang("You have to set the percentage of the district before to go forward"));
    }
    require_once "funclib/percentual.inc.php";
    if(!checkPercentualsCasino()){
        addError("percetage", $lang->getLang("You have to set the percentage fields to save the new %", "club"));
    }
    if(!checkPercentualsFields()){
        addError("percetage", $lang->getLang("You have to set the percentage fields to save the new %", "club"));
    }

    if ( !areErrors() ) {

        $next_id  = $dbh->nextSequence('JUR_ID_SEQ');

        $dbh->begin();
        if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting") betting_begin();
        //$currency=(empty($currency) ? 1 : $currency);
        $currency = $dbh->queryOne('SELECT jur_currency from jurisdiction where jur_id='.$dbh->escape($district_id));
        if(!is_numeric($currency) || $currency==''){
            $currency=1;
        }
        $skinId = $dbh->queryOne('SELECT jur_skn_id from jurisdiction where jur_id='.$dbh->escape($district_id));

        if(!is_numeric($skinId)){
            $skinId='NULL';
        }

        $timeZone= $dbh->queryOne('SELECT jur_time_utc from jurisdiction where jur_id='.$dbh->escape($district_id));
        $sql = "INSERT INTO jurisdiction (jur_id, jur_name, jur_code, jur_class, jur_address1, jur_address2" .
            ", jur_postcode_zip, jur_phone, jur_notes, jur_parent_id, jur_has_live_games,jur_currency, jur_vat_code,jur_cou_id,jur_city,jur_skn_id,jur_time_utc,jur_access_type,jur_google_map_info,jur_agency_hour,jur_params, jur_users_limit,jur_processor_enabled)" .
            " VALUES (" . $next_id . ", " . $dbh->prepareString($name) . ", " . $dbh->prepareString($code) . ", 'club'" .
            ", " . $dbh->prepareString($address1) . ", " . $dbh->prepareString($address2) .
            ", " . $dbh->prepareString($postc) . ", " . $dbh->prepareString($phone) .
            ", " . $dbh->prepareString($notes) . ", " . $district_id . ", " . $has_live . ",$currency, '$vatCode','$country','$city',$skinId,$timeZone,".$dbh->prepareString(strtoupper($accessType)).",".$dbh->prepareString($localization).",".$dbh->prepareString($hours).",".$dbh->prepareString($params)."," . $jurUsersLimit . "," . $dbh->prepareString($u_processors) . ")";
        $ret=$dbh->exec($sql);
        $betting_db_trans = false;
        $casino_db_trans = false;
        if ( !$ret ) {
            addError("", $lang->getLang('Database Error - Could not create jurisdiction record'));
            $casino_db_trans = false;
        }else{
            if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
                $result = save_betting_jurisdiction($next_id,$name,$district_id,"club");
                if($result){
                    $perc_res = savePercentuals($_POST["percentuals"], $next_id, $district_id,$_POST["betting"],$_POST['betting_live']);
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
            }
            else{
                $perc_res = savePercentuals($_POST["percentuals"], $next_id, $district_id,$_POST["betting"],$_POST['betting_live']);
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
                                        SELECT '.$next_id.', SCN_GAMES_INFO, SCN_MENU_INFO, SCN_SKIN_PROPERTIES ,SCN_MOBILE_GAMES_INFO,
                                                        SCN_MOBILE_MENU_INFO, SCN_MOBILE_SKIN_PROPERTIES
                                        FROM jurisdiction_configuration
                                        WHERE SCN_JUR_ID = '.$district_id);



            $dbh->commit();
            if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting") betting_commit();
            return $next_id;
        }else{
            $dbh->rollback();
            if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting") betting_rollback();
            return FALSE;
        }
    }
}

//var_dump($_POST);
$op = ( !empty($_POST['op']) ? $_POST['op'] :$_GET['op'] );
$has_live = true;

/*addedd cache*/

include "JurisdictionsList.class.inc";
$jur_list = JurisdictionsList::getInstance($dbh);

if($op != "Add" && $op != "Save"){
    $hierarchy = $jur_list->getJurisdictionHierarchy($node);

    $can_modify_jurisdiction = false;
    foreach($hierarchy as $parent){
        if($_SESSION["jurisdiction_id"] == $parent){
            $can_modify_jurisdiction = true;
        }
    }
    if(!$can_modify_jurisdiction){
        dieWithError($lang->getLang('Jurisdiction not found'));
    }
}
$delete_cache = false;
/*addedd cache*/

include "JurisdictionsList.class.inc";
$jur_list = JurisdictionsList::getInstance($dbh);

switch ( $op ){
    case "Add": //display add new task screen
        $display =  "club_add_form.php";
        break;
    /*addedd cache*/
    case "Delete":
        $sql = "UPDATE jurisdiction SET jur_status = 0 WHERE jur_id = $node";
        $rs  = $dbh->exec($sql);
        if($rs == 1){
            $delete_cache = true;
        }
        $alert = $lang->getLang("% blocked","club ".$node)."!</br>";

        $child_id_list   = $jur_list->getChilJurisdictionsIdList($node);
        //$child_id_list[] = $node;
        $chunked_lists   = array_chunk($child_id_list, 1000);
        $locked_users  = 0;

        $aus_arr = array();
        $jur_arr = array();
        $pun_arr = array();
        foreach($chunked_lists as $list){
            $aus_arr[] = "aus_jurisdiction_id IN (" . implode(",", $list) . ")";
            $jur_arr[] = "jur_id IN (" . implode(",", $list) . ")";
            $pun_arr[] = "pun_betting_club IN (" . implode(",", $list) . ")";
        }


        //Nei club non è necessario
        /*
        $sql = "UPDATE jurisdiction SET jur_status = 2 WHERE jur_status = 1 AND (" . implode(" OR ", $jur_arr) . ") AND jur_id != $node";
        $rs  = $dbh->exec($sql);
        $locked_jurs = 1 + $rs;
        $alert .= ", $locked_jurs deleted children jurisdictions";
        */
        //-------------------------


        if($_GET["lock_users"] == "true"){
            $sql = "UPDATE admin_user SET aus_access = 'LOCK', aus_notes = 'Locked by admin'";
            if(count($aus_arr) > 0){
                $sql .= " WHERE aus_access = 'ALLOW' AND (" . implode(" OR ", $aus_arr) . ")";
                $rs  = $dbh->exec($sql);
                $locked_users += $dbh->getAffectedRows();
            }
            $alert .= $lang->getLang("and % locked: %",$lang->getLang("administrators"), $locked_users) ."!</br>";
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
        $display =  "club_add_form.php";
        $sql = "UPDATE jurisdiction SET jur_status = 1 WHERE jur_id = $node";
        $rs  = $dbh->exec($sql);
        if($rs == 1){
            $delete_cache = true;
        }
        $alert = $lang->getLang("% restored","club ".$node)."!</br>";

        $child_id_list   = $jur_list->getChilJurisdictionsIdList($node);
        //$child_id_list[] = $node;
        $chunked_lists   = array_chunk($child_id_list, 1000);
        $locked_users  = 0;

        $aus_arr = array();
        $jur_arr = array();
        $pun_arr = array();
        foreach($chunked_lists as $list){
            $aus_arr[] = "aus_jurisdiction_id IN (" . implode(",", $list) . ")";
            $jur_arr[] = "jur_id IN (" . implode(",", $list) . ")";
            $pun_arr[] = "pun_betting_club IN (" . implode(",", $list) . ")";
        }


        //Nei club non è necessario
        /*
        $sql = "UPDATE jurisdiction SET jur_status = 2 WHERE jur_status = 1 AND (" . implode(" OR ", $jur_arr) . ") AND jur_id != $node";
        $rs  = $dbh->exec($sql);
        $locked_jurs = 1 + $rs;
        $alert .= ", $locked_jurs deleted children jurisdictions";
        */
        //-------------------------


        if($_GET["unlock_users"] == "true"){
            $sql = "UPDATE admin_user SET aus_access = 'ALLOW', aus_notes = 'Unlocked by admin'";
            if(count($aus_arr) > 0){
                $sql .= " WHERE (" . implode(" OR ", $aus_arr) . ")";
                $rs  = $dbh->exec($sql);
                $locked_users += $dbh->getAffectedRows();            }
            $alert .=  $lang->getLang("and % unlocked: %",$lang->getLang("administrators"), $locked_users) ."!</br>";
        }
        if($_GET["unlock_punters"] == "true"){
            $sql = "UPDATE punter SET pun_access = 'allow', pun_notes = 'Unlocked by admin'";
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
    /*addedd cache*/
    case "Save":  //save a new task
        if($_POST["autocode"] == "1"){
            $_POST['account_code'] = "AUTO";
        }
        $params='';
        $taxValue='';
        if(isset($_POST['tax'])&& $_POST['tax']!=''){
            $taxValue="tax:";
            foreach($_POST['tax'] as $k=>$v ){
                $taxValue.=$k."=".$v.";";
            }
            $taxValue=substr($taxValue, 0, -1);
        }
        $params=$taxValue;
        $virtualJackpot='';
        if(isset($_POST['virtual_jackpot'])&& $_POST['virtual_jackpot']!=''){
            $virtualJackpot="conf:virtual_jackpot=ON";
            if($params!=''){
                $params.="~".$virtualJackpot;
            }
            else{
                $params=$virtualJackpot;
            }
        }

        $u_processors = implode("~",$_POST['processors']);
        $node = save_club($_POST['club_name'],  $_POST['account_code'], $_POST['address1'], $_POST['address2'],$_POST['district'], $_POST['postc'], $_POST['phone'], $_POST['notes'], ($_POST['has_live'] == "ALLOW"),$_POST['currency'], $_POST["vat_code"],$_POST['country'],$_POST['city'],$_POST['accessType'],$_POST['latitude'],$_POST['longitude'],$_POST['hoursStart'],$_POST['hoursEnd'],$params,$_POST['users_limit'],$u_processors);
        if ( ! empty($node) ) {            
            update_user_limit_childs($_SESSION['jurisdiction_id']);
            $display =  "club_display_form.php"; //display new record
            $club     = $_POST['club_name'];
            $vatCode  = $_POST['vat_code'];
            $alert   = "club $club successful created! ";
            $delete_cache = true;
           error_log('created club');

        } else {
            $display =  "club_add_form.php"; //if error, display add form again
        }
        break;
    case "Update":  //update an existing club
        $params='';
        $taxValue='';
        if(isset($_POST['tax'])&& $_POST['tax']!=''){
            $taxValue="tax:";
            foreach($_POST['tax'] as $k=>$v ){
                $taxValue.=$k."=".$v.";";
            }
            $taxValue=substr($taxValue, 0, -1);
        }
        $params=$taxValue;
        $virtualJackpot='';
        if(isset($_POST['virtual_jackpot'])&& $_POST['virtual_jackpot']!=''){
            $virtualJackpot="conf:virtual_jackpot=ON";
        }else{
            $virtualJackpot="conf:virtual_jackpot=OFF";
        }
        if($params!=''){
            $params.="~".$virtualJackpot;
        }
        else{
            $params=$virtualJackpot;
        }

        $u_processors = implode("~",$_POST['processors']);

        $updated = update_club($node, $_POST['club_name'], $_POST['address1'], $_POST['address2'],
            $_POST['district'], $_POST['postc'], $_POST['phone'], $_POST['notes'], ($_POST['has_live'] == "ALLOW"),$_POST['currency'],$_POST['country'],$_POST['city'],$_POST['accessType'],$_POST['latitude'],$_POST['longitude'],$_POST['hoursStart'],$_POST['hoursEnd'],$params,$_POST['users_limit'],$u_processors);
        if ( $updated ) {
            $jur_list->destroyParentsJurisdictionsCache($node);
            $club = $_POST['club_name'];
            $alert   = $lang->getLang("Club % successful updated!",$club);
        }
        $sql = "SELECT * FROM jurisdiction WHERE jur_id = " . $node;
        $rs = $dbh->exec($sql);
        $num_rows = $rs->getNumRows();
        if ( 1 == $num_rows ) {
            $row = $rs->next();
            $account_code  = $row['jur_code'];
            $vatCode       = $row['jur_vat_code'];
            $u_processors = explode('~',$row['jur_processor_enabled']);
        }

        $delete_cache = true;
        $display =  'club_display_form.php';
        break;
    default:
        $display =  'club_display_form.php';
}

if($delete_cache){
    $jur_list->destroyParentsJurisdictionsCache($node);
}

if ( 'club_display_form.php' == $display ){
    if ( ! $_POST['op'] ){
        $sql = "SELECT DISTINCT jur_id, jur_name, jur_code, jur_class, jur_address1, jur_address2,jur_access_type,jur_skn_id,
                jur_postcode_zip, jur_phone, jur_notes, jur_parent_id, jur_has_live_games, jur_creation_date, jur_status,jur_cou_id,jur_city,
                jur_time_utc,jur_available_credit,jur_google_map_info,jur_agency_hour, coalesce(pun_username, NULL) affiliate_username , coalesce(pun_id, NULL) affiliate_pun_id,jur_params,jur_users_limit,jur_processor_enabled
                FROM jurisdiction
                LEFT JOIN admin_user ON jur_id = aus_jurisdiction_id
                LEFT JOIN affiliate ON aus_id = aff_aus_id
                LEFT JOIN punter ON pun_betting_club = jur_id AND aff_aus_id = pun_aus_id AND aff_id = pun_aff_id
                WHERE jur_id =" . $dbh->escape($node);
        $rs = $dbh->exec($sql);
        $num_rows = $rs->getNumRows();
        if ( 1 == $num_rows ){
            $row = $rs->next();
            $club_name    = $row['jur_name'];
            $account_code = $row['jur_code'];
            $address1     = $row['jur_address1'];
            $address2     = $row['jur_address2'];
            $district     = $row['jur_parent_id'];
            $postc        = $row['jur_postcode_zip'];
            $phone        = $row['jur_phone'];
            $notes        = $row['jur_notes'];
            $jur_status   = $row['jur_status'];
            $country      = getCountryCode2Chars($row['jur_cou_id']);
            $city         = $row['jur_city'];
            $has_live     = ($row["jur_has_live_games"] == 1);
            $creationTime = $row['jur_creation_date'];
            $accessType   = $row['jur_access_type'];
            $skinId       = $row['jur_skn_id'];
            $hours        = explode('-',$row['jur_agency_hour']);
            $params       = $row['jur_params'];
            $hoursStart=$hours[0];
            $hoursEnd=$hours[1];
            $localization = explode(';',$row['jur_google_map_info']);
            $longitude=$localization[0];
            $latitude=$localization[1];
            $affiliateUsername=$row['affiliate_username'];
            $affiliateId=$row['affiliate_pun_id'];
            $jurUsersLimit=$row['jur_users_limit'];
            $u_processors = explode('~',$row['jur_processor_enabled']);

        }
    } else {
        error_log('showing display');
        if ('Update' != $op ) {
            $account_code = $_POST['account_code'];
        }
        $club_name    = $_POST['club_name'];
        $address1     = $_POST['address1'];
        $address2     = $_POST['address2'];
        $district     = $_POST['district'];
        $postc        = $_POST['postc'];
        $phone        = $_POST['phone'];
        $notes        = $_POST['notes'];
        $jur_status   = $_POST['jur_status'];
        $country      = $_POST['country'];
        $city         = $_POST['city'];
        $hoursStart   = $_POST['hoursStart'];
        $hoursEnd     = $_POST['hoursEnd'];
        $longitude    = $_POST['longitude'];
        $latitude     = $_POST['latitude'];
        $accessType   = $_POST['accessType'];
        $u_processors = $_POST['processors'];

        $has_live     = ((empty($_POST['has_live']) ? true : (($_POST['has_live'] == "ALLOW") ? true : false)));
        $skinId = $dbh->queryOne('SELECT jur_skn_id from jurisdiction where jur_id='.$dbh->escape($node));
        $sql = " SELECT DISTINCT jur_id, jur_name, jur_code, jur_class, jur_address1, jur_address2,jur_access_type,jur_skn_id,
                 jur_postcode_zip, jur_phone, jur_notes, jur_parent_id, jur_has_live_games, jur_creation_date, jur_status,jur_cou_id,jur_city,
                 jur_time_utc,jur_available_credit,jur_google_map_info,jur_agency_hour, coalesce(pun_username, NULL) affiliate_username,  coalesce(pun_id, NULL) affiliate_pun_id,jur_params,jur_users_limit,jur_processor_enabled
                 FROM jurisdiction
                 LEFT JOIN admin_user ON jur_id = aus_jurisdiction_id
                 LEFT JOIN affiliate ON aus_id = aff_aus_id
                 LEFT JOIN punter ON pun_betting_club = jur_id AND aff_aus_id = pun_aus_id AND aff_id = pun_aff_id
                 WHERE jur_id =" .$dbh->escape($node);
        $rs = $dbh->exec($sql);
        $num_rows = $rs->getNumRows();
        if ( 1 == $num_rows ){
            $row = $rs->next();
            $jur_status   = $row['jur_status'];
            $affiliateUsername=$row['affiliate_username'];
            $affiliateId=$row['affiliate_pun_id'];
            $params       = $row['jur_params'];
            $jurUsersLimit=$row['jur_users_limit'];
            $u_processors = explode('~',$row['jur_processor_enabled']);
        }
    }
}
if ( $display ) {
    require($display);
} else {
    echo $lang->getLang("Please select from the menu on the left");
}

if ( isset($alert) )
    jAlert(mb_ereg_replace('"', '\"', $alert));
?>
