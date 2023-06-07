<?php
//Modificate tutte le query Manuel Rinaldi tolto TO_CHAR
function checkSingleUserLogged($user_id, $sessionID){
    global $memcached;
    if(is_null($user_id)) return false;
    $value = $memcached->get($user_id);
    if($value != $sessionID){
        session_destroy();
        return false;
    }
    return true;
}

function generateSessionId(){
    if (isset($_POST["extsys"]) && $_POST["extsys"] == 'true' && isset($_POST['cookieid']) && $_POST['cookieid'] != '') {
        return $_POST['cookieid'];
    } else {
        return md5(uniqid(microtime()) . $_SERVER['REMOTE_ADDR'] . $_SERVER['HTTP_USER_AGENT']);
    }
}
function browserName() {
    if ( strstr(strtoupper($_SERVER["HTTP_USER_AGENT"]), "MSIE") )
        return "ie";
    else
        return "netscape";
}

function getMagic() {
    $user_agent = $_SERVER["HTTP_USER_AGENT"];
    $accept     = $_SERVER["HTTP_ACCEPT"];
    $accept_lang= $_SERVER["HTTP_ACCEPT_LANGUAGE"];
    $accept_enc = $_SERVER["HTTP_ACCEPT_ENCODING"];
    $accept_chs = $_SERVER["HTTP_ACCEPT_CHARSET"];
    $ipaddress  = getIpAddress();
    return md5($user_agent.$accept.$accept_lang.$accept_enc.$accept_chs.$ipaddress);
}

function getGamesReturn($win, $bet){
    return number_format(($win / $bet) * 100 , 2);
}


function returnRealGamesName($tbl_name){
    $table = explode("_", $tbl_name);
    if(strpos($tbl_name,'Mega_Lotto_Agency')!==false){
        $table=str_replace('_',' ',$tbl_name);
        return $table;
    }
    return $table[0];
}

/**
 * @return void
 * @param string $var
 * @param string $relative
 * @desc Prints the value of a variable, inside a div (layer).  If $var is an array, will print key=>value pairs
 */
function showVal($var, $position='relative') {
    if ( is_array($var) ) {
        ob_start();
        print_r($var);
        $var = ob_get_clean();
    }
    if ( 'relative' == $position ) {
        ?>  <div style="position:relative;background-color:white;color:black;border:solid; border-width:1;border-color:blue"><?=$var?></div>
        <?php
    }
    else {
        ?>  <div style="position:absolute; top:50; left:50; background-color:white; color:black; border:solid; border-width:1; border-color:blue; height: 11px;"><?=$var?></div>
        <?php
    }
}

/**
 * @return void
 * @param string $var
 * @param string $relative
 * @desc aliases for $showval
 */
function printval($var, $position='relative') { return showVal($var, $position='relative'); }
function printvar($var, $position='relative') { return showVal($var, $position='relative'); }
function showvar($var, $position='relative')  { return showVal($var, $position='relative'); }

/**
 * @return boolean
 * @param string $str
 * @desc Returns true if a variable contains white space only
 */
function isBlank($str) {
    return mb_ereg_match("^\s*$", $str);
}

/**
 * @return boolean
 * @desc Returns true string contains numbers only
 */
function isInteger($var) {
    return  preg_match("/^[0-9]+$/", $var);
}

/**
 * @return string
 * @param string $short
 * @desc Returns a string of random charcters
 */
function randomString($short=false) {
    mt_srand((double)microtime()*1000000);
    $str = md5 (uniqid (rand(100, 900), true));
    return ( $short ? substr($str, 0, 8) : $str );
}

/**
 * @return void
 * @param string $first_name
 * @param string $middle_name
 * @param string $last_name
 * @desc Returns a display-formatted name string
 */
function concatNames($first_name, $middle_name, $last_name, $indexed=false) {
    if ( $middle_name )
        $middle_name = " ".$middle_name." ";
    else
        $middle_name = " ";

    if ( $indexed )
        return "$last_name, $first_name$middle_name";
    else
        return "$first_name$middle_name$last_name";
}


/**
 * @return boolean
 * @desc Returns true if the customer is logged in (Session variables/cookies are set)
 */
function isLoggedIn()  {
    return isset($_SESSION["admin_id"]);
}


/**
 * @return void
 * @param string $num
 * @param string $decimal_places
 * @desc prints an amount in dollar format
 */
function printInDollars($amount, $decimal_places=2, $return=false,$sym=false,$greaterThanZero=false) {
    global $symbol;
    if(!isset($sym) || $sym=='' || is_null($sym)){
        if(!isset($symbol) || $symbol=='' || is_null($symbol)){
            if(isset($_SESSION['currency_html']) && $_SESSION['currency_html']!=''){
                $symbol = $_SESSION['currency_html']." ";
            }
            elseif (defined('CURRENCY_SYMBOL')) {
                $symbol = CURRENCY_SYMBOL;
            }
            else {
                $symbol = ' NaN ';
            }
        }
        else{
            $symbol=trim($symbol)." ";
        }
    }
    else{
        $symbol=trim($sym)." ";
    }
    if ( $return )
        ob_start();

    if ( 0 == $amount && !$greaterThanZero ) {

        ?><span class="cSymbol"><?=$symbol?></span><span class="cAmount"> 0 </span><?
    } else {
        $val_out = number_format($amount,  $decimal_places, ',' , '.'); //get abs value
        if ( $amount <= 0 ) {
            ?><span style="color:red"><span class="cSymbol"><?=$symbol?></span><span class="cAmount"><?=$val_out?></span></span><?php
        } else {
            ?><span class="cSymbol"><?=$symbol?></span><span class="cAmount"> <?=$val_out?></span><?php
        }
    }

    if ( $return )
        return ob_get_clean();
}

/**
 * @return string
 * @param double amount
 * @param integer cents
 * @desc Returns display formatted currency
 */
function getInDollars($amount, $cents=2,$sym=false,$greaterThanZero=false) {
    return printInDollars($amount, $cents, $return=true,$sym,$greaterThanZero);
}

/**
 * @return string
 * @param string $num
 * @param string $decimal_places
 * @desc returns an amount in 'fun token' format
 */
function getInFunTokens($amount, $cents=2) {
    if ( 0 == $amount ) {
        return '#0';
    }
    elseif ( $amount < 0 ) {
        if ( $cache_file ) {
            return "(#".number_format(abs($amount),  $cents).")";
        }
        else {
            return "<font color=red>(#".number_format(abs($amount),  $cents).")</font>";
        }
    }
    else {
        return "#   ".number_format($amount,  $cents);
    }
}

/**
 * @return void
 * @param string $num
 * @param string $decimal_places
 * @desc prints an amount in 'fun token' format
 */
function printInFunTokens($amount, $decimal_places=2) {
    print getInFunTokens($amount, $decimal_places);
}

/**
 * @return void
 * @param string $err_str
 * @desc Die and print formatted error message
 */
function dieWithError($err_str) {
    die("<font color=\"red\"><strong>$err_str</strong></font>");
}

/**
 * @return String
 * @param string $str
 * @desc Removes non-numeric characters froma string
 */
function toNumbersOnly($str) {
    return preg_replace("/[^0-9]/", "", $str);
}

/**
 * @return String
 * @param string $str
 * @desc Alias for toNumbersOnly
 */
function toDigitsOnly($str) {
    return toNumbersOnly($str);
}

/**
 * @return int
 * @param string $customer_number
 * @desc Returns id (primary key) of customer record with matching number
 */
function getPunterIDByCustNum($customer_number)  {
    global $dbh;

    $pun_id =0;
    $sql = "select pun_id from punter where pun_customer_number = $customer_number";
    $rs=$dbh->exec($sql);
    $num_rows = $rs->getNumRows();

    if ( 1 == $num_rows ) {
        $rec = $rs->next();
        $pun_id = $rec['pun_id'];
    }
    return $pun_id;
}



/**
 * @return array
 * @param string $punter_id
 * @desc Returns associative array of punter
 */
function getPunter($punter_id, $is_customer_num=false, $include_internet_users = false,$forceMaster=false) {
    global $dbh;

    //Select punter details record using punter id
    $sql  = "select pun_id, pun_first_name, pun_middle_name, pun_last_name,pun_betting_type,pun_daily_deposits_limit,pun_login_lock_start,pun_aff_id " .
        ", pun_member_type, pcr_credits, pcr_bonus AS bonus_credits, pun_access, pcr_total_bets" .
        ", pcr_total_wins, pun_email, pun_username, pun_reg_date" .
        ", pun_customer_number, pun_identified, pun_connection_type" .
        ", pun_notes, pun_cou_code, pun_investigate" .
        ", pun_dob, pun_address_line1, pun_address_line2, pun_city_suburb" .
        ", pun_state_province, pun_postcode_zip, pun_phone_business" .
        ", pun_phone_home, pun_phone_mobile, pun_fax, pun_daily_allowance" .
        ", pun_gender, pun_temp_password, pun_num_logins, pun_last_logged_in" .
        ", pcr_reserved_funds, pun_lock_reason, pun_id_documents, pun_preg_code" .
        ", pun_login_lock_start, pun_email_reg_code, pun_sms_reg_code" .
        ", pun_registration_status, pun_pre_registered, pcr_total_deposits" .
        ", pun_password, pcr_total_withdrawals, pun_password_set_time, pun_sre_code" .
        ", pun_last_request_time,pun_sess_id,pun_site_id, pun_num_failed_logins, pun_betting_club, pun_lang, pun_nin_code,pun_preferences,cur_code_for_web,cur_id as cur_cur_id,skn_url, " .


        " j1.jur_name as 'pun_betting club',
          j2.jur_name as 'pun_district',
          j3.jur_name as 'pun_region',
          j4.jur_name as 'pun_nation',
          j1.jur_id as 'pun_betting_club_id',
          j2.jur_id as 'pun_district_id',
          j3.jur_id as 'pun_region_id',
          j4.jur_id as 'pun_nation_id',
          (select pnu_id from partner_users where pnu_local_id = pun_id) partner_user_id
          from punter
              LEFT JOIN punter_credit on pun_id = pcr_pun_id
              LEFT JOIN jurisdiction j1 on pun_betting_club = j1.jur_id
              LEFT JOIN jurisdiction j2 on j1.jur_parent_id = j2.jur_id
              LEFT JOIN jurisdiction j3 on j2.jur_parent_id = j3.jur_id
              LEFT JOIN jurisdiction j4 on j3.jur_parent_id = j4.jur_id
              LEFT JOIN currency on j1.jur_currency=cur_id
              LEFT JOIN skins on pun_site_id=skn_id
         where ";

    if ( $is_customer_num )
        $sql .= " pun_customer_number = $punter_id";
    else
        $sql .= " pun_id = $punter_id";

    if ( ! empty($_SESSION['admin_id']) ) {
        //Filter punters according to their jurisdiction and the admin user's jurisdiction

        if ( 'club' == $_SESSION['jurisdiction_class'] ) { //If a club admin user
            // only show punters in same club as admin user, unless we're also including
            // internet users (nonfinancial only)
            if($include_internet_users)
                $sql .= " AND (pun_betting_club = " . $_SESSION['jurisdiction_id'] . " OR " .
                    " (pun_betting_club = (SELECT jur_id FROM jurisdiction WHERE jur_name = 'Internet' AND jur_class = 'club') AND pun_member_type = 'NONFINANCIAL')) ";
            else
                $sql .= " and pun_betting_club = " . $_SESSION['jurisdiction_id'];
        }
        elseif ( 'district' == $_SESSION['jurisdiction_class'] ) { //If a district manager admin user
            // only select punters where their betting club (bottom level jurisdiction)
            // is under the admin user's district
            $sql .= ' AND pun_betting_club IN' .
                '       (   SELECT jur_id FROM jurisdiction' .
                '           WHERE jur_parent_id = ' . $_SESSION['jurisdiction_id'] .
                '       )';
        }
        elseif ( 'region' == $_SESSION['jurisdiction_class'] )  { //If a region manager admin user
            // only select punters where their betting club (bottom level jurisdiction)
            // is under the admin user's district
            // select only  records within admin user's jurisdiction
            $sql .= ' AND (pun_betting_club IN ' .
                '     (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
                '         (  SELECT jur_id FROM jurisdiction ' .
                '             WHERE jur_parent_id = ' .$_SESSION['jurisdiction_id'] .
                '         )' .
                '     )';

            if($_SESSION["aty_code"] == "SKINMANAGER"){
                $skin_id = getSkinClubId($_SESSION["jurisdiction_id"]);
                $sql .= " OR pun_betting_club = $skin_id";
            }

            $sql .= ")";
        }
        elseif ( 'nation' == $_SESSION['jurisdiction_class'] ) { // If a nation manager admin user
            $sql .= ' AND pun_betting_club IN ' .
                '     (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
                '        (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
                '            (  SELECT jur_id FROM jurisdiction ' .
                '                WHERE jur_parent_id = ' .$_SESSION['jurisdiction_id'] .
                '            )' .
                '        ) ' .
                '     )  ';
        }
        else {
            if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
                dieWithError("Invalid user Jurisdiction Class!");
            }
        }
    }
    if($forceMaster){
        $rs = $dbh->exec($sql,false,true);

    }else{
        $rs = $dbh->exec($sql);
    }
    $num_rows = $rs->getNumRows();

    $new_row = null;
    if ( $num_rows > 0 ){
        $rec = $rs->next();
        $new_row = array();
        foreach ( $rec as $colname => $val )
            $new_row[removeTablePrefix($colname)] = $val;

        $new_row['balance'] = $new_row['credits'];
        $new_row['available_balance'] = $new_row['credits'] - $new_row['reserved_funds'];
        $new_row['address1'] = $new_row['address_line1'];
        $new_row['address2'] = $new_row['address_line2'];
        $new_row['country_code'] = $new_row['cou_code'];
        $new_row['country'] = $new_row['cou_code'];
        $new_row['full_name'] = concatNames($new_row['first_name'],$new_row['middle_name'],$new_row['last_name']);
        $new_row['bonus_credits'] = $rec["bonus_credits"];

        $timeout_end_mt = dbTsToPhpTs($new_row['last_request_time']) + 3600;
        $new_row['sess_timeout'] = mktime() > $timeout_end_mt;
        $new_row['bonus_credits'] = $rec["bonus_credits"];
        $new_row['partner_user_id'] = $rec["partner_user_id"];
    }
    return $new_row;
}


function getCustomerByAuth($username, $password) {
    global $dbh;

    $password = md5($password);

    //Select punter details record using punter id
    $sql  = "select pun_id, pun_first_name, pun_middle_name, pun_last_name";
    $sql .= ", pun_member_type, pcr_credits, pcr_bonus AS bonus_credits, pun_access, pcr_total_bets";
    $sql .= ", pcr_total_wins, pun_email, pun_username,  pun_reg_date";
    $sql .= ", pun_customer_number, pun_identified, pun_connection_type";
    $sql .= ", pun_notes, pun_cou_code, pun_investigate";
    $sql .= ", pun_dob, pun_address_line1, pun_address_line2, pun_city_suburb";
    $sql .= ", pun_state_province, pun_postcode_zip, pun_phone_business";
    $sql .= ", pun_phone_home, pun_phone_mobile, pun_fax, pun_daily_allowance";
    $sql .= ", pun_gender, pun_temp_password, pun_num_logins, pun_last_logged_in";
    $sql .= ", pcr_reserved_funds, pun_lock_reason, pun_id_documents, pun_preg_code";
    $sql .= ", pun_login_lock_start, pun_email_reg_code, pun_sms_reg_code";
    $sql .= ", pun_registration_status, pun_pre_registered, pcr_total_deposits";
    $sql .= ", pun_password, pcr_total_withdrawals, pun_password_set_time, pun_sre_code";
    $sql .= ", pun_last_request_time";
    $sql .= "  FROM punter, punter_credit WHERE";
    $sql .= "  pun_username = '$username' and pun_password = '$password'";
    $sql .= "  AND pun_id = pcr_pun_id";

    $rs = $dbh->exec($sql);
    $num_rows = $rs->getNumRows();

    if ( $num_rows > 0 ) {
        $rec = $rs->next();

        foreach ( $rec as $colname => $val )
            $new_row[removeTablePrefix($colname)] = $val;

        $new_row['balance'] = $new_row['credits'];
        $new_row['address1'] = $new_row['address_line1'];
        $new_row['address2'] = $new_row['address_line2'];
        $new_row['available_balance'] = $new_row['credits'] - $new_row['reserved_funds'];
        $new_row['country_code'] = $new_row['cou_code'];
        $new_row['country'] = $new_row['cou_code'];
        $new_row['full_name'] = concatNames($new_row['first_name'],$new_row['middle_name'],$new_row['last_name']);
        $new_row['available_balance'] = $new_row['credits'] - $new_row['reserved_funds'];

        $timeout_end_mt = dbTsToPhpTs($new_row['last_request_time']) + 3600;
        $new_row['sess_timeout'] = mktime() > $timeout_end_mt;
        $new_row['bonus_credits'] = $rec["bonus_credits"];

    }
    return $new_row;
}

/**
 * @return array
 * @param string $punter_id
 * @desc alias for getPunter
 */
function getCustomer ($customer_id, $is_customer_num=false, $include_internet_users = false) {
    return getPunter($customer_id, $is_customer_num, $include_internet_users);
}

/**
 * @return array
 * @param string $punter_id
 * @desc Returns associative array of punter
 */
function removeTablePrefix($column_name, $prefix='') {
    if ( $prefix && substr($column_name, 0, 3) != $prefix ) {  //if prefix specified and column doesn't have prefix
        return $column_name;                                  //then return the column as-is
    }
    return substr($column_name, 4, strlen($column_name)-4);
}

function sendToWebApp($DataToSend,$WebAppAddr='http://wsap.cslivegames.net:8080/services/InternalService') {
    error_log('Sending data to webapp: '.$DataToSend);
    $ch = curl_init ($WebAppAddr);
    curl_setopt ( $ch, CURLOPT_SSL_VERIFYPEER, FALSE );
    curl_setopt ( $ch, CURLOPT_POST, 1 );
    curl_setopt ( $ch, CURLOPT_POSTFIELDS, $DataToSend );
    curl_setopt ( $ch, CURLOPT_RETURNTRANSFER, 1 );
    $result = curl_exec ( $ch );
    error_log('Received: '.$result);
    return $result;
}


/**
 * @return array
 * @desc Returns array indexed by game_id punter, containing array of punters
 */
function getPunterSessions() {
    $punter_sessions = array();

    //send request to gameserver to obtain list of punters currently playing
    if ($sock = fsockopen("10.15.0.7", 60001, $errno, $errstr, 30)) {  //attempt to open socket to localhost:3032
        $request = "4\n";
        fwrite($sock, $request); //send request

        while ( $line = fgets($sock, 1024) ) {
            $arr = explode(";", $line);  //split comma-delimited string into array elements
            $game_id       = $arr[0];   //first token
            $total_players = $arr[1];   //second token
            $punters       = $arr[2];  //list of punter session strings "pun_id:session_start_time:member_type"

            //explode into punter session strings such that each sell contains pun_id,session_start,member_type
            $punters_sessions_arr   = explode(',', $arr[2]);
            array_pop($punters_sessions_arr); //since comma at the end of punter list, remove last cell

            //Iterate through array of punter session strings
            foreach ( $punters_sessions_arr as $key => $val ) {
                $punter_session = explode(':', $val);
                $id = $punter_session[0];
                $session_start = $punter_session[1];
                $member_type = $punter_session[2];

                //store customer sessions for each game
                $punter_sessions[$game_id][$id] = array('session_start'=>$session_start, 'member_type'=>$member_type);
            }
        }
        fclose($sock);
    }
    else
        return;

    return $punter_sessions;
}

function getConnectedClients() {
    $connected_clients = array();
    if ($sock = fsockopen("localhost", 50000, $errno, $errstr, 30)) {
        $request = "lusers\n";
        fputs($sock, $request);
        $buf = "";
        while ($line = fgets($sock, 1024)) {
            $buf .= $line;
        }
        $clients_arr = explode(',', $buf);
        array_pop($clients_arr);
        foreach ($clients_arr as $client) {
            list($pun_id, $pun_username) = split(":", $client);
            $connected_clients[$pun_id] = $pun_username;
        }
        fclose($sock);
    }
    return $connected_clients;
}

function sendPunterCredits($pun_id, $console_port) {
    return true;
    if (!is_numeric($pun_id))
        return false;
    if ($sock = fsockopen("10.10.0.2", $console_port , $errno, $errstr, 30)) {
        $request = "sendCredits,$pun_id\n";
        fputs($sock, $request);
        while ($line = fgets($sock, 1024)) {
            list ($errCode, $errStr) = explode('/', $line);
            if ($errCode == 1) {
                return true;
            }
        }
        fclose($sock);
    }
    return false;
}


function disconnectClient($pun_id) {
    if (!is_numeric($pun_id))
        return false;
    if ($sock = fsockopen("localhost", 50000, $errno, $errstr, 30)) {
        $request = "kick,$pun_id\n";
        fputs($sock, $request);
        $buf = "";
        while ($line = fgets($sock, 1024)) {
            $buf .= $line;
        }
        fclose($sock);
        if (substr($buf, 0, 1) == "1") {
            return true;
        }
    }
    return false;
}

/**
 * @return array
 * @param pun_id
 * @param sessions
 * @desc Checks if a punter is logged in and playing a game; $sessions can be supplied to reduce overhead if it contains punter sessions already
 */
function punterIsPlaying( $pun_id, $sessions="" ) {
    global $conn;

    if (defined('DYNAMIC_RECHARGE') && DYNAMIC_RECHARGE == true)
        return false;

    if ( !is_array($sessions) )
        $sessions = getPunterSessions();

    if ( ! $sessions )  //gameserver is not running
        return false;

    foreach ( $sessions as $game_id =>  $punter_sessions_arr )
        if ( $punter_sessions_arr[$pun_id]  )
            return true;
    return false;
}

/**
 * @return int
 * @param string $customer_number
 * @desc Returns associative array of punter
 */
function customerNumberExists($customer_number) {
    global $dbh;
    return $dbh->countQuery("select count(*) from punter where pun_customer_number = $customer_number");
}

/**
 * @return int
 * @param string name
 * @desc Returns a customer number
 */
function generateCustomerNumber($name) {
    global $dbh;

    $num_chars = strlen($name);


    //get array representation of string (a character to a cell)
    for ($i=0; $i<$num_chars; $i++)
        $char_array[$i] = substr($name, $i, 1);

    $cust_number = array(0,0,0,0,0,0);

    for ($i=0; $i<$num_chars; $i+=6 ) {
        $cust_number[0] = substr($cust_number[0] + substr(ord($char_array[$i]),-1),-1);
        $cust_number[1] = substr($cust_number[1] + substr(ord($char_array[$i+1]),-1),-1);
        $cust_number[2] = substr($cust_number[2] + substr(ord($char_array[$i+2]),-1),-1);
        $cust_number[3] = substr($cust_number[3] + substr(ord($char_array[$i+3]),-1),-1);
        $cust_number[4] = substr($cust_number[4] + substr(ord($char_array[$i+4]),-1),-1);
        $cust_number[5] = substr($cust_number[5] + substr(ord($char_array[$i+5]),-1),-1);
    }
    $customer_number_string = implode("", $cust_number);

    if ( strlen($customer_number_string) < 6 )
        return  generateCustomerNumber($name.preg_replace("/[^a-zA-Z]/", "", randomString()));

    if ( customerNumberExists($customer_number_string) ) {
        $str = preg_replace("/[^a-zA-Z]/", "", randomString().randomString());
        return generateCustomerNumber($str);
    }
    return $customer_number_string;
}

/**
 * @return int
 * @param string name
 * @desc Returns the name of the country with id $country_id
 */
function getCountryName($country_code) {
    global $dbh;

    $sql = "SELECT cou_name from country where cou_code = '$country_code'";
    $rs = $dbh->exec($sql);

    if ( $rs->getNumRows() > 0 ) {
        $rec = $rs->next();
        $country_name = $rec['cou_name'];
    }
    return $country_name;
}


function getCustomerProfiles() {
    global $dbh;
    $sql = "SELECT * from punter_profiles";
    $rs = $dbh->exec($sql);
    return $rs;
}
/**
 * @return int
 * @param int country_id
 * @desc Returns a three-character country code
 */
function getCountryCode($country_id) {
    global $dbh;

    $sql = "select cou_code_three_chars from country where cou_id = $country_id";
    $rs = $dbh->exec($sql);

    if($rs->getNumRows() > 0) {
        $rec = $rs->next();
        $country_code = $rec['cou_code_three_chars'];
    }
    return $country_code;
}

function getCountryCode2Chars($country_id){
    global $dbh;
    $country_code = "US";
    $sql = "select cou_code_two_chars from country where cou_id = $country_id";
    try {
        $rs = $dbh->queryRow($sql);
        if ( count($rs) > 0 ) {
            $country_code = $rs['cou_code_two_chars'];
        }
    } catch (Exception $e) {}

    return $country_code;
}

function getCountry($country_code){
    global $dbh;
    $sql = "select cou_id, cou_code, cou_code_three_chars, cou_dns_code, cou_phone_code, cou_name from country where cou_code = ".$dbh->prepareString($country_code);
    $country_rec = $dbh->queryOne($sql);

    return $country_rec;
}

/**
 * @return boolean
 * @param int country_id
 * @desc Returns true if the customer has credit card deposit records
 */
function hasDepositedByCreditCard($pun_id) {
    global $dbh;

    $cnt = $dbh->countQuery("select count(*) from customer_transaction where ctr_pun_id = $pun_id and ctr_pme_code = 'CRD'");
    return $cnt > 0;
}

/**
 * @return string
 * @desc Returns IP address of client
 */
function getIpAddress() {
    return ( isset($_SERVER['HTTP_X_FORWARDED_FOR']) ? $_SERVER['HTTP_X_FORWARDED_FOR'] : $_SERVER['REMOTE_ADDR']);
}

/**
 * @return void
 * @desc Recordsactivity of a customer
 */
function doCustomerLog($pun_id, $log_type,$log_data='') {
    global $dbh;

    $ip_address = getIpAddress();
    $sql  = "insert into customer_log";
    $sql .= " (clg_id, clg_time, clg_pun_id, clg_clt_code, clg_data, clg_ip_address)";
    $sql .= " values(nextval('seq_clg_id'), CURRENT_TIMESTAMP, $pun_id, '$log_type', '$log_data', '$ip_address')";

    $dbh->exec($sql);
}

function getPunterGroups($pun_id) {
    global $dbh;
    $pun_groups = array();
    $sql  = "select pcg_cgr_code FROM punter_cgroup WHERE pcg_pun_id=$pun_id";
    $res = $dbh->exec($sql);
    while($res->hasNext()) {
        $res_data = $res->next();
        $pun_groups[] = $res_data["pcg_cgr_code"];
    }
    return $pun_groups;
}

/**
 * @return boolean
 * @param string $group_code
 * @param int $pun_id
 * @desc Returns true if the customer $pun_id is in the group $group_code
 */
function punterInGroup($group_code, $pun_id) {
    global $dbh;

    $sql  = "select count(*) from punter_cgroup";
    $sql .= " where pcg_cgr_code = '$group_code' and pcg_pun_id = $pun_id";
    $count = $dbh->countQuery($sql);
    return ($count > 0 );
}

function punterInCCGroups($pun_id) {
    global $dbh;
    $sql  = "SELECT count(*) FROM punter_cgroup";
    $sql .= " WHERE pcg_pun_id=$pun_id AND (pcg_cgr_code='WMC' OR pcg_cgr_code='ITC')";
    $count = $dbh->countQuery($sql);
    return ($count > 0);
}

/**
 * @return void
 * @param string $group_code
 * @param int $pun_id
 * @desc Adds the customer with id $pun_id to the group $group_code
 */
function addToGroup($group_code, $pun_id) {
    global $dbh;

    $sql = " insert into punter_cgroup (pcg_pun_id,  pcg_cgr_code)";
    $sql .= " values ($pun_id,'$group_code')";
    $dbh->exec($sql);
}

/**
 * @return void
 * @param string $group_code
 * @param int $pun_id
 * @desc Removes the customer with id $pun_id to the group $group_code
 */
function removeFromGroup($group_code, $pun_id) {
    global $dbh;

    $sql  = "DELETE FROM punter_cgroup";
    $sql .= " WHERE pcg_pun_id = $pun_id and pcg_cgr_code = '$group_code'";
    $dbh->exec($sql);
}

/**
 * @return boolean
 * @param string $url
 * @desc returns true if $url begins with 'https'
 */
function isSecureUrl ($url) {
    return substr($url, 0, 5) == 'https';
}

/**
 * @return boolean
 * @param string $var
 * @desc Used to prevent multiple submissions; registers $var that was sent in the request
 */
function registerSubmitVar($var_name) {
    $var_val = ( isset($_POST[$var_name])  ? $_POST[$var_name] : $_GET[$var_name] );
    $_SESSION[$var_name] = $var_val;
}

/**
 * @return boolean
 * @param string $var
 * @desc Used to prevent multiple submissions; returns true if a POST/GET variable has been recorded in the session
 */
function isRepeatSubmit($var_name) {
    $val = getPostGetVar($var_name);
    return ( isset($_SESSION[$var_name]) && $_SESSION[$var_name] == $val);
}

/**
 * @return boolean
 * @param string $var
 * @desc Used to prevent multiple submissions; unregisters a submit variable so a request can be repeated
 */
function unregisterSubmitVar($var_name) {
    unset($_SESSION[$var_name]);
}

/**
 * @return boolean
 * @param string $number
 * @desc checks if string is in a correct currency format
 */
function isValidMoney($number) {
    $pattern = "/^[0-9]*(\.{1,1}[0-9]{1,2}){0,1}$/";
    return preg_match($pattern, $number);
}


/**
 * @return boolean
 * @param string $number
 * @desc alias for isValidMoney
 */
function isMoney($number) {
    return isValidMoney($number);
}


/**
 * @return boolean
 * @param string $number
 * @desc Returns true if $number looks like a phone number
 */
function isValidPhoneNumber ( $number )
{	$PHONE_PATTERN = "^[0-9\-\+\(\) ]+$";

    return  mb_ereg_match ($PHONE_PATTERN, $number);
}


/**
 * @return array
 * @param string $number
 * @desc Returns array [config key]=>value by reading in the config file
 *       specified in $file_path
 */
function getCfgFile($file_path)
{  if ( file_exists($file_path) )
{  if ( $f = fopen($file_path, 'r') )
{  while ( $line = fgets($f, 2048) )
{  $arr = explode("=", $line);
    $directive = $arr[0];
    $value = ereg_replace("\n", '', ereg_replace("\r", "", $arr[1]));
    $cfg[$directive] = $value;
}
    fclose($f);
}
else
    dieWithError("cannot open: CFG".( DEBUG ? $file_path : ''));
}
else
    dieWithError("does not exist: CFG".( DEBUG ? $file_path : ''));

    return $cfg;
}



/**
 * @return int
 * @param string $sre_code
 * @param boolean $surname
 * @desc Returns name in proper case
 *
 */
function salesCodeExists ( $sre_code )
{  global $dbh;

    $sql  = "select count(*) from admin_user";
    $sql .= " where lower(aus_sales_code) = '".strtolower($sre_code)."'";

    return $dbh->countQuery($sql);
}


/**
 * @return string
 * @param int $number
 * @desc Adds leading zero to a single-digit number
 *
 */
function addLeadingZero ($number)
{  if ( $number < 10 )
    if ( strlen($number) < 2 )
        return "0$number";
    return $number;
}


/**
 * @return string
 * @param string $url
 * @desc returns array with directory names in conexutive order
 *
 */
function getDirs($url="")
{  if ( ! $url ) //if no argument, use current script path
    $url = $_SERVER['SCRIPT_NAME'];

    $dir_arr = explode('/', $url);
    array_shift($dir_arr); //blank before first '/'
    array_pop($dir_arr);//don't want filename or blank after last '/'
    return $dir_arr;
}

/**
 * @return boolean
 * @param string $page_name
 * @desc returns true if page has pop or popup at end of filename
 *
 */
function isPopupPage($page_name) {
    $arr = explode ('.', $page_name);
    $filename = $arr[0];
    return ( substr ($filename, -4) == '_pop' || substr ($filename, -6) == '_popup' );
}


/**
 * @return string
 * @param string $varname
 * @desc returns contents of post or get variable (if exists)
 *
 */
function getPostGetVar($var_name) {
    return ( isset($_POST[$var_name]) ? $_POST[$var_name] : $_GET[$var_name] ) ;
}

/**
 * @return void
 * @param string $var_name
 * @desc Makes a POST or GET variable global
 *
 */
function globalize($var_name,$func="") {
    return globalise($var_name,$func);
}

function globalizeFloat($var_name) {
    globalise($var_name, "floatval");
}

function globalizeInt($var_name) {
    globalise($var_name, "intval");
}

function globalizeStr($var_name) {
    globalise($var_name, "strval");
}

function globalizeSql($var_name) {
    globalise($var_name, "addslashes");
}

function globalisePost($var_name) {
    if ( $_POST[$var_name] )
        $GLOBALS[$var_name] = $_POST[$var_name];
}

function globalise($var_name, $func="") {
    $value = ( $_POST[$var_name] ? $_POST[$var_name] : $_GET[$var_name]);

    if ( $func )
        @$value = $func($value);

    $GLOBALS[$var_name] = $value;
    return $value;
}

/**
 * @return boolean
 * @param string $varname
 * @desc returns true if a contact medium for a customer has been confirmed .e.g mobile, email
 *
 */
function isConfirmed($customer_id, $comm_medium) {
    global $dbh;

    $sql  = "select pun_confirmation_list from punter";
    $sql .= " where pun_id = $customer_id";
    $rs   = $dbh->exec($sql);

    if ( $rs->getNumRows() == 1 ) {
        $rec = $rs->next();
        $confirmation_list = $rec['pun_confirmation_list'];

        if ( $confirmation_list ) {
            $arr = explode(',', $confirmation_list);
            return in_array(strtolower($medium), $arr);
        }
    }
    return false;
}


/**
 * @return array
 * @param int $id
 * @desc returns associative array of columns for admin user with id == $id
 *
 */
function getAdminUser($id) {
    global $dbh;

    $sql  = "SELECT aus_id, aus_name, aus_username, aus_password, aus_aty_id, aus_access";
    $sql .= ", aus_last_logged_in, aus_num_logins, aus_email, aus_address1, aus_address2, aty_name";
    $sql .= ", aus_city_suburb, aus_state_province, aus_postcode_zip, aus_phone_home, aus_phone_work";
    $sql .= ", aus_phone_mobile, aus_cou_id, aus_notes,  aus_sales_code ";
    $sql .= " FROM admin_user, admin_user_type";
    $sql .= " where aus_id = $id and aus_aty_id = aty_id";

    $rs = $dbh->queryRow($sql);

    if ( count($rs)!=0) {
        return removeTablePrefixFromKeys($rs,'aus');
    }
    return;
}


/**
 * @return array
 * @param int $id
 * @desc returns associative array of columns for admin user with id == $id
 *
 */
function removeTablePrefixFromKeys($arr,$prefix=""){
    foreach( $arr as $key => $val )
        $arr2[removeTablePrefix($key,$prefix)] = $val;
    return $arr2;
}


/**
 * @return string
 * @param int $id
 * @desc returns name of a game with gam_id == $id
 *
 */
function getGameName($id){
    global $dbh;
    $rs=$dbh->queryOne("select gam_name from game where gam_id= $id");
    $game_name = $rs;
    return $game_name;
}

/**
 * @return void
 * @param string $path
 * @param bollean $has_ext
 * @param string $session_id
 * @param int $min_atime_diff
 * @desc Cleans the temporary download files created for the current session
 */
function cleanTempFiles ($path, $has_ext=false, $session_id='', $min_atime_diff=0) {
    $time = mktime();
    clearstatcache();

    if ( $dir = opendir($path) ) {
        while ( $file_name = readdir($dir) ) {
            $file_path = "$path/$file_name";

            $nam_arr  = explode ('.', $file_name);
            $ext = $nam_arr[count($nam_arr)-1];

            if ( 'html' != $ext  && 'log' != $ext && 'lg' != $ext && 'inc' != $ext && 'cfg' != $ext && 'js' != $ext ) {
                if ( is_file($file_path) ) {
                    $atime = fileatime($file_path);
                    $atime_diff = $time - $atime;
                    //echo "$file_path ".date('d/m/Y H:i:s', fileatime($file_path))."<br>";

                    if ( $atime_diff >  $min_atime_diff ) {
                        $arr = explode('.', $file_name);

                        if ( $has_ext )
                            $file_sid = $arr[count($arr)-2]; //filename.sid.extension
                        else
                            $file_sid = $arr[count($arr)-1]; //filename.sid

                        if ( $session_id ) {
                            if ( $file_sid == $session_id )
                                unlink($file_path);
                        }
                        else {
                            unlink($file_path);
                        }
                    }
                }
            }
        }
    }
}


/**
 * @return boolean
 * @param string $str
 * @desc Returns true is a string looks like it is utf8 encoded
 */
function seems_utf8($str) {
    for ($i = 0; $i < strlen($str); $i++)  {
        if ( ord($str[$i]) < 0x80)  continue;  // 0bbbbbbb
        else if ((ord($str[$i]) & 0xE0) == 0xC0)  $n = 1;    // 110bbbbb
        else if ((ord($str[$i]) & 0xF0) == 0xE0)  $n = 2;    // 1110bbbb
        else if ((ord($str[$i]) & 0xF8) == 0xF0)  $n = 3;    // 11110bbb
        else if ((ord($str[$i]) & 0xFC) == 0xF8)  $n = 4;    // 111110bb
        else if ((ord($str[$i]) & 0xFE) == 0xFC)  $n = 5;    // 1111110b
        else  return false;                                  // Does not match any model

        for ($j = 0; $j < $n; $j++) {                        // n bytes matching 10bbbbbb follow?
            if ((++$i == strlen($str)) || ((ord($str[$i]) & 0xC0) != 0x80))
                return false;
        }
    }
    return true;
}


function form_head($action=SELF, $name="", $method="POST", $onsubmit="", $noautocomplete=false, $encType = false) {
global $page, $header_page, $operation;

if ( ! $action )
    $action = SELF;
if ( $name )
    $name_str = " name=\"$name\"";
if ( $onsubmit )
    $onsubmit = " onSubmit=\"$onsubmit\"";
if ($encType)
    $encType=" enctype=\"$encType\"";

if ( $noautocomplete )
    $noautocomplete = " autocomplete=\"off\"";
else
    $noautocomplete = "";

?>
<form action="<?=$action?>" method="<?=$method?>"<?=$name_str?><?=$onsubmit?><?=$noautocomplete?><?=$encType?>>
    <input type="hidden" name="form_submitted" value="yes">
    <input type="hidden" name="page" value="<?=$page?>">
    <input type="hidden" name="header_page" value="<?=$header_page?>">
    <input type="hidden" name="operation" value="<?=$operation;?>">
    <?php
    }


    function refPage($page_name="", $header_page=""){
        return "/?page=".$page_name."&header_page=".$header_page;
    }
    function refFormPage($page_name="", $header_page=""){
        return "/?page=".$page_name."&header_page=".$header_page;
    }

    function pdaRefPage($page_name="", $header_page="") {
        return secure_host."/pda/?page=".$page_name."&header_page=".$header_page;
    }

    function getPhoneContactString($cust_row) {
        $phone_home = $cust_row['phone_home'];
        $phone_business = $cust_row['phone_business'];
        $phone_mobile = $cust_row['phone_mobile'];
        $fax = $cust_row['fax'];
        $pun_id = $cust_row['id'];

        if ( $phone_home ) {
            $contact_str .= "Home: $phone_home";
            if ( isConfirmed($pun_id, 'homephone') )
                $contact_str .= ' <img src="'.image_dir.'/icon_activated_sm.gif">';
            $contact_str .="<br/>";
        }
        if ( $phone_business ) {
            $contact_str .= "Work: $phone_business";
            if ( isConfirmed($pun_id, 'businessphone') )
                $contact_str .= ' <img src="'.image_dir.'/icon_activated_sm.gif">';
            $contact_str .="<br>";
        }
        if ( $phone_mobile ) {
            $contact_str .= "Mobile: $phone_mobile";
            if ( isConfirmed($pun_id, 'mobilephone') )
                $contact_str .= ' <img src="'.image_dir.'/icon_activated_sm.gif">';
            $contact_str .="<br>";
        }
        if ( $fax ) {
            $contact_str .= "Fax: $fax<br>";

            if ( isConfirmed($pun_id, 'fax') )
                $contact_str .= ' <img src="'.image_dir.'/icon_activated_sm.gif">';
            $contact_str .="<br>";
        }
        return $contact_str;
    }

    function numFailedLogins($pun_id) {
        global $dbh;

        $sql  = "select pun_num_failed_logins";
        $sql .= " from punter where pun_id = $pun_id";
        $rs  = $dbh->exec($sql);
        $row  = $rs->next();
        return $row['num_failed_logins'];
    }

    function totalBets($customer_id) {
        global $dbh;

        $sql  = "select pcr_total_bets from punter_credit";
        $sql .= " where pcr_pun_id = $customer_id";
        $rs  = $dbh->exec($sql);
        $row  = $rs->next();
        return $row['total_bets'];
    }

    function deleteCustomer($customer_id) {
        global $dbh;

        $customer = getCustomer($customer_id);

        if ( $customer ) {
            if ( ! $customer['num_logins']) {
                $dbh->begin();

                //delete customer group record if exists
                $sql  = "delete from customer_log";
                $sql .= " where clg_pun_id = $customer_id";
                $dbh->exec($sql);

                //delete customer group record if exists
                $sql  = "delete from punter_cgroup";
                $sql .= " where pcg_pun_id = $customer_id";
                $dbh->exec($sql);


                //delete customer credit record
                $sql  = "delete from punter_credit";
                $sql .= " where pcr_pun_id = $customer_id";
                $dbh->exec($sql);

                //delete customer record
                $sql  = "delete from punter ";
                $sql .= " where pun_id = $customer_id";
                $dbh->exec($sql);
                $dbh->commit();

                $_SESSION['message'] =  "Customer ".$customer['full_name']." has been deleted successfully";
            }
        }
        else {
            $_SESSION['message'] =  "The customer record for ".$customer['full_name']." cannot be deleted, because the customer is a financial member";
        }
    }


    /****** ADMIN FUNCTIONS *****/
    function adminUserAuthenticated($username, $password){
        global $dbh;
        $md5_pwd = md5($password);
        $sql = "select count(*) from admin_user";
        $sql .= " where aus_username = '$username'";
        $sql .= " and aus_password = '$md5_pwd'";
        $num = $dbh->queryOne($sql);
        return (1 == $num);
    }


    function clearInvestigation($pun_id) {
        global $conn;

        $sql  = "update punter set pun_investigate = NULL";
        $sql .= " where pun_id = $pun_id";
        sqlExec($conn, $sql);
    }

    function adminAlert($msg) {
        global $_SESSION;
        $_SESSION['alert_message'] = $msg;
    }

    function allChecked($checkboxArray) {
        if ( ! is_array($checkboxArray) )
            return false;

        foreach($checkboxArray as $key => $val )
            if ( ! $val )
                return false;
        return true;
    }


    function arrayHasValueSet ($array) {
        foreach ( $array as $key => $val )
            if ( $val )
                return true;
        return false;
    }

    function doAdminUserLog($admin_id, $log_type, $log_data, $pun_id='NULL', $ipaddr = null ) {
        global $dbh;

        //  $next_id  = $dbh->nextSequence('ALG_ID_SEQ');
        //  $sql  = "insert into admin_user_log (alg_id, alg_time, alg_type, alg_data, alg_aus_id, alg_pun_id)";
        //  $sql .= " values ($next_id, CURRENT_TIMESTAMP, '$log_type', '$log_data', $admin_id, $pun_id)";
        $alg_id = $dbh->nextSequence("alg_id_seq");
        if ($ipaddr == null)
            $ipaddr = getIpAddress();
        $sql  = "INSERT INTO admin_user_log (alg_id, alg_type, alg_time, alg_data, alg_aus_id, alg_pun_id, alg_ipaddr)";
        $sql .= "  VALUES ($alg_id, '$log_type', CURRENT_TIMESTAMP, '$log_data', $admin_id, $pun_id, '$ipaddr')";
        $dbh->exec($sql);
    }


    function numMembers($group_code) {
        global $dbh;
        $sql  = "select count(*) from punter_cgroup where pcg_cgr_code = '$group_code'";
        $num_members  = $dbh->countQuery($sql);
        return $num_members;
    }
    function getAllGames() {
        global $dbh;
        $sql = "Select * from game";
        $result = $dbh->doCachedQuery ( $sql, 0 );
        return $result;
    }



    function getGames () {
        global $dbh;

        if ( ! $dbh )
            $dbh = new DatabaseHandler;

        if ( ! $dbh->isConnected() )
            $dbh->connect();

        $sql  = "select gameid, gamename";//, gty_name, gam_max_bet, gam_min_bet, gam_max_pos_bet, gam_min_pos_bet
        $sql .= " from game ";//, game_type where gam_gty_id = gty_id

        return $dbh->exec($sql);
    }


    function getAddressString($cust_row) {
        $address_line1 = $cust_row['address_line1'];
        $address_line2 = $cust_row['address_line2'];
        $city_suburb = $cust_row['city_suburb'];
        $state_province = $cust_row['state_province'];
        $postcode_zipcode = $cust_row['postcode_zip'];
        $cou_code = $cust_row['cou_code'];
        $pun_id = $cust_row['id'];

        $country_arr  = getCountryName($cou_code);


        $addr_str = $address_line1;
        if ( $address_line2 )
            $addr_str .= "<br>$address_line2";
        $addr_str .= "<br>$city_suburb";

        if ( $state_province && 'NA' != $state_province && 'N/A' != $state_province)
            $addr_str .= "<br>$state_province";

        if ( $postcode_zipcode && 'NA' != $postcode_zipcode && 'N/A' != $postcode_zipcode)
            $addr_str .= "<br>$postcode_zipcode";

        $addr_str .= "<br>$country";

        if ( isConfirmed($pun_id, 'address') ) {
            ob_start();
            ?>
            <table cellpadding=0 cellspacing=0 border=0>
                <tr valign=top><td><?=$addr_str;?></td><td>&nbsp;&nbsp;&nbsp;<img src="<?=image_dir?>/icon_activated_sm.gif"></td></tr>
            </table>
            <?php
            $addr_str = ob_get_contents();
            ob_end_clean();
        }

        return $addr_str;
    }

    function checkHashedPassword($username,$password){
        global $dbh;
        $sql = "select AUS_PASSWORD from admin_user";
        $sql .= " where aus_username = '$username'";
        $password_against =md5($dbh->queryOne($sql).$username);
        return ($password == $password_against);
    }


    /****** END ADMIN FUNCTIONS *****/

    function setEditable() {
        global $mode;

        if ( $mode == "view" )
            echo " onFocus='this.blur()' ";
        else
            echo " onChange='ChangesMade=true'";
    }

    function emailIsRegistered($email) {
        global $dbh;
        $sql = "select pun_id from punter";
        $sql .= " where (pun_email = '$email' OR pun_username = '$email') AND pun_access = 'allow'";
        $sql .= " and pun_registration_status != 'activation expired' ";
        $rs = $dbh->exec($sql);
        return ($rs->getNumRows() > 0);
    }

    function ninCodeIsRegistered($nin_code) {
        global $dbh;
        $sql = "select pun_id from punter";
        $sql .= " where pun_nin_code = '$nin_code' AND pun_access = 'allow'";
        $sql .= " and pun_registration_status != 'activation expired' ";
        $rs = $dbh->exec($sql);
        return ($rs->getNumRows() > 0);
    }


    function isValidEmail ( $email ) {
        $EMAIL_PATTERN = "^[a-zA-Z0-9_\-\+\.]+@{1,1}([a-zA-Z0-9_\-]+\.{1,1}([a-zA-Z0-9_\-\+])+)+$";
        return mb_ereg_match($EMAIL_PATTERN,$email);
    }



    function getErrors(){
        global $ERRORS;
        return $ERRORS;

    }
    
    function showErrors($only_global = false, $return = false) {
        global $ERRORS, $lang, $isExternalSystem;
    
        $num_errors = count($ERRORS);
        $s = $num_errors > 1 ? 's' : '';
    
        $num_printed = 0;
        $out = "";
        $array = array();
        if ($num_errors > 0) {
            $out .= "<div class=error><table>";
            $out .= "  <tr>";
            $out .= "    <td class=error colspan=2>";
            $out .= "     <div class=\"errorbold\">" . $lang->getLang("The following error$s occurred:") . "</div>";
            $out .= "      <table cellpadding=2 cellspacing=0 border=0>";
    
            for ($i = 0; $i < $num_errors; $i ++) {
                if ($only_global) {
                    if (! empty($ERRORS[$i]['field']))
                        continue;
                }
                $num_printed ++;
                $out .= "  <tr valign=top>";
                $out .= "    <td class=error>-</td>";
                $out .= "    <td class=error>";
                $out .= $ERRORS[$i]["message"];
                $out .= "    </td>";
                $out .= "  </tr>";
                $array['error' . $i] = $ERRORS[$i]["message"];
            }
            
            $out .= "</table>";
            $out .= "      </td>";
            $out .= "   </tr>";
            $out .= "</table></div>";
            if ($return) {
                return $out;
            }
            if ($num_printed > 0) {
                echo $out;
            }
/**
            if ($isExternalSystem == 'false') {
                if ($return) {
                    return $out;
                }
                if ($num_printed > 0) {
                    echo $out;
                }
            } else {
                $array['status'] = -100;
                error_log("Reponse: " . json_encode($array));
                return print_r(json_encode($array));
            }
**/

        }
    }

    function showResult ($only_global=false) {
        global $RESULT;

        $num_result = count($RESULT);
        $s = $num_result > 1 ? 's' : '';

        $num_printed = 0;

        if ($num_result > 0 ) {
            $out .= "<div class='result'><table>";
            $out .= "  <tr>";
            $out .= "    <td class='result' colspan=2>";
            $out .= "     <div class=\"resultbold\">The following message$s for you:</div>";
            $out .= "      <table cellpadding=2 cellspacing=0 border=0>";

            for ( $i=0; $i<$num_result; $i++ ) {
                if ( $only_global ) {
                    if ( ! empty($RESULT[$i]['field']) )
                        continue;
                }
                $num_printed++;

                $out .= "  <tr valign=top>";
                $out .= "    <td class='result'>-</td>";
                $out .= "    <td class='result'>";
                $out .= $RESULT[$i]["message"];
                $out .= "    </td>";
                $out .= "  </tr>";
            }

            $out .= "</table>";
            $out .= "      </td>";
            $out .= "   </tr>";
            $out .= "</table></div>";

            if ( $num_printed > 0 ) {
                echo $out;
            }
        }
    }


    function printError ( $message ) {
        ?><span style="font-family:verdana; font-size:10pt; color:red">
        <? echo "<br><b>$message</b><br>" ?>
        </span><?
    }


    function areErrors () {
        global $ERRORS;
        return (count($ERRORS) >= 1);
    }

    function clearErrors() {
        global $ERRORS;
        unset($ERRORS);
    }

    function addError($field_name, $msg) {
        global $ERRORS, $ERROR_MESSAGES;

        $num_errors = count($ERRORS);

        $ERRORS[$num_errors]["field"] = $field_name;
        $ERRORS[$num_errors]["message"] = $msg;
        $ERROR_MESSAGES[$field_name] = $msg;
    }

    function addResult($field_name, $msg) {
        global $RESULT, $RESULT_MESSAGES;

        $num_result = count($RESULT);

        $RESULT[$num_result]["field"] = $field_name;
        $RESULT[$num_result]["message"] = $msg;
        $RESULT_MESSAGES[$field_name] = $msg;
    }

    function clearResult() {
        global $RESULT;
        unset($RESULT);
    }


    function destroySession() {
        session_destroy();
        session_set_cookie_params(-1000000);
        $_SESSION = array();
        /*global $cookie;

        //cleanTempFiles (cache_file_dir, $has_ext=false, session_id(), $min_atime_diff=0);
        //cleanTempFiles (temp_dir, $has_ext=true, session_id(), $min_atime_diff=0);

        if ( count($_SESSION) > 0 ) {
          foreach ( $_SESSION as $key => $val ) {
            unset($_SESSION[$key]);
          }
          session_destroy();
        }
        $cookie->delete();*/
    }

    function sessionTimeout() {
        global $dbh, $dirs, $page_name;

        if ( $_SESSION['customer_number'] ) {
            $sql = "update punter set pun_last_request_time = NULL";
            $sql .= " where pun_customer_number = ".$_SESSION['customer_number'];
            $dbh->exec($sql);
        }

        destroySession();

        $dirs = getDirs();

        if ( 'applets' == $dirs[0] || isPopupPage($page_name) ) {
            jscriptStart();
            jscriptLine("window.opener.location = '".secure_host."/auth/session_timeout.html';");
            jscriptLine("window.close()");
            jscriptEnd();
        }
        else {
            header("Location: ".secure_host.'/auth/session_timeout.html');
        }
        exit;
    }

    function revokeUserAccess($arg_username="") {
        global $_SESSION;

        $conn = casinoConnect();
        $user = ( $arg_username ? $arg_username : $_SESSION["username"] );
        $sql = "update user set usr_access = 'deny' where usr_username = $user";
        if ( $user ) {
            $sql = "update user set usr_access = 'deny' where usr_username = '$user'";
            sqlExec($conn, $sql);
        }
    }

    function flagPunterForInvestigation($pun_id) {
        global $conn;

        $sql  = "update punter set pun_investigate = 1";
        $sql .= " where pun_id = $pun_id";
        sqlExec($conn, $sql);
    }

    function lockAccount($pun_id, $reason="") {
        global $dbh;

        $sql  = "update punter";
        $sql .= " set pun_access = 'deny'";
        $sql .= ", pun_lock_reason = '$reason'";
        $sql .= "where pun_id = $pun_id";
        $dbh->exec($sql);
    }

    function checkSessionTimeout($page_name) {
        global $dbh, $cookie, $customer_row;

        if ( 'login.html' == $page_name || 'logout.html' == $page_name ||
            'session_timeout.html' == $page_name || 'applet.html' == $page_name ) {
            return false;
        }

        $customer_number = toNumbersOnly($cookie->getFieldValue('customer_number'));

        if ( isBlank($customer_number) )
            return true;

        if ( $customer_row ) {
            return $customer_row['sess_timeout'];
        }
        else {
            $sql = "select pun_last_request_time from punter" .
                " where pun_customer_number = $customer_number";
            $rs  =  $dbh->exec($sql);

            if ( $rs->getNumRows() == 1 ) {
                $row = $rs->next();
                $timeout_end_mt = dbTsToPhpTs($row['pun_last_request_time']) + 3600; //convert to seconds ts
                return mktime() > $timeout_end_mt;  //return timeout if now is later than lat request time + 1 hour
            }
        }
        return false;
    }


    function updateRequestTime($customer_number="") {
        global $dbh, $cookie;

        if ( ! $customer_number )
            $customer_number = toNumbersOnly($cookie->getFieldValue('customer_number'));

        if ( isBlank($customer_number) )
            return;

        $sql  = "update punter set pun_last_request_time = CURRENT_TIMESTAMP";
        $sql .= " where pun_customer_number = $customer_number";
        $dbh->exec($sql);
    }


    function loginOnlyPage($redir_page) {
        if ( !isLoggedIn() ) {
            header("Location: $redir_page");
            exit;
        }
    }

    function financialMemberOnlyPage($redir_page) {
        if ( isLoggedIn() ) {
            if ( $_SESSION['member_type'] != 'FINANCIAL' ) {
                header("Location: $redir_page");
                exit;
            }
        }
        else {
            header("Location: $redir_page");
            exit;
        }
    }


    function isSecurePage () {
        return (isset($_SERVER["HTTPS"]) && $_SERVER["HTTPS"] == "on");
        //  return $_SERVER['SERVER_PORT'] == SSL_PORT;
    }

    function fieldInError($field_name) {
        global $ERRORS;

        $num_errors = count($ERRORS);

        for ( $i=0; $i<$num_errors; $i++ )
            if ( $ERRORS[$i]['field'] == $field_name )
                return $ERRORS[$i]['message'];
        return 0;
    }

    function set_login_sess_vars ($punter_row) {
        global $cookie;

        //register session variables
        $_SESSION["username"]        = $punter_row['username'];
        $_SESSION["pun_id"]          = $punter_row['id'];
        $_SESSION["customer_id"]     = $punter_row['id'];
        $_SESSION["member_type"]     = $punter_row['member_type'];
        $_SESSION["customer_number"] = $punter_row['customer_number'];
        $_SESSION["ip_address"]      = getIpAddress();


        if ( $punter_row['temp_password'] ) {  //to flag that the customer must change their password
            setCookie("chp", 'yes',0,'/');
            $_SESSION["chp"] = 'yes';
        }

        $cookie->setFieldValue('customer_number', $punter_row['customer_number']);
        $cookie->setFieldValue('member_type', $punter_row['member_type']);

        $cookie->set();
    }


    function doLogin($punter_row, $referrer="") {
        doLoginNoRedirect($punter_row);

        $referrer_t = trim(strtok($referrer, '?'));

        if ( !$referrer_t
            || secure_host.'/auth/logout.html' == $referrer_t
            || nonsecure_host.'/index.html' == $referrer_t
            // || secure_host.'/auth/forgot_password.html' == $referrer_t
            || secure_host.'/auth/session_timeout.html' == $referrer_t
            || secure_host.'/cookies_blocked.html' == $referrer_t
            || ! isSecureUrl($referrer_t)
        )
            $referrer = secure_host."/myaccount/";
        else {
            $ref_arr = parse_url($referrer);

            if ( $ref_arr['host'] != $_SERVER['HTTP_HOST'] )
                $referrer = secure_host."/myaccount/";
        }

        header("Location: $referrer");
        exit;
    }


    function doLoginNoRedirect($punter_row) {
        global $dbh;

        $sql  = "UPDATE punter " .
            "SET pun_login_lock_start = NULL, " .
            " pun_num_logins = pun_num_logins+1, " .
            " pun_last_logged_in = CURRENT_TIMESTAMP, " .
            " pun_num_failed_logins=0, " .
            " pun_last_request_time = CURRENT_TIMESTAMP, " .
            " pun_sess_id = '" . session_id() . "' " .
            "WHERE pun_id = ".$punter_row['id'];

        $dbh->exec($sql);

        //    if ( "NONFINANCIAL" == $member_type )
        //    {  $sql  = "update punter_credit set pcr_credits = 1000";
        //   	   $sql .= " where pcr_pun_id = ".$punter_row['pun_id']." and pcr_credits < 1000";
        //   	   $dbh->exec($sql);
        //    }

        /* topup user who just logged in) credits where the user has less
        than $1000 and is a non-financial memeber */
        // session_regenerate_id();

        //register session variables
        set_login_sess_vars ($punter_row);
    }



    function numLogins($pun_id) {
        global $dbh;

        $sql  = "select pun_num_logins";
        $sql .= " from punter";
        $sql .= " where pun_id = $pun_id";
        $rs   = $dbh->exec($sql);

        $row = $rs->next();
        return $row['pun_num_logins'];
    }


    //Querys user table for matching user record
    /**
     * @return unknown
     * @param unknown $username
     * @param unknown $password
     * @desc Enter description here...
     */
    function authenticate( $username, $password )
    {  global $dbh;

        $password = md5($password);

        $sql  = "select pun_id, pun_member_type, pun_access, pun_first_name" .
            ", pun_customer_number, pun_reg_date, pun_email_reg_code" .
            ", pun_sms_reg_code, pun_temp_password, pun_registration_status" .
            ", pun_confirmation_list, pun_login_lock_start, pun_lang" .
            " from punter where pun_username = '$username'" .
            " and pun_password = '$password' and  pun_registration_status != 'activation expired'";



        $rs   = $dbh->exec($sql);

        if ( $rs->getNumRows() == 1 )
            return $rs->next();
    }

    function check_swf_login($sid) {
        global $dbh;
        $sanity_chk =  preg_match('/^[A-Za-z0-9]{32,32}$/', $sid);
        if ($sanity_chk) {
            $query = "SELECT pun_id FROM punter WHERE pun_sess_id='$sid'";
            $rs = $dbh->exec($query);
            if ($rs->getNumRows() == 1) {
                $cus_row = $rs->next();
                $cus_id = $cus_row["pun_id"];
                $cus_row = getCustomer($cus_id);
                doLoginNoRedirect($cus_row);
                return $cus_id;
            }
        }
        return 0;
    }


    function doLoginLock ($customer_id) {
        global $conn;

        $sql  = "update punter set pun_login_lock_start = CURRENT_TIMESTAMP";
        $sql .= " where pun_id = $customer_id";
        sqlExec($conn, $sql);
    }


    function getLoginLockStart($customer_id) {
        global $conn;

        $sql  = "select pun_login_lock_start from punter";
        $sql .= " where pun_id = $customer_id";
        $res = sqlExec($conn, $sql);
        $num_rows =sqlNumRows($res);
        if ( $num_rows > 0 ) {
            $row = sqlResult($res);
            sqlFreeResult($res);
            $login_lock_start = $row[0];
            return  $login_lock_start;
        }
        return;
    }


    function removeLoginLock ($customer_id) {
        global $dbh;

        $sql  = "update punter set pun_login_lock_start = NULL";
        $sql .= " where pun_id = $customer_id";
        $dbh->exec($sql);
    }


    function getMsg($id, $replace_arr="", $lang="") {
        global $dbh;

        if ( !$lang )
            if ( ! $lang = $_SESSION['lang'] )
                $lang = 'EN';

        $sql = "select mes_text from message where mes_id = $id and mes_lang_code = '$lang'";
        $rs  = $dbh->exec($sql);

        if ( $rs->getNumRows() == 1 ) {
            $rec = $rs->next();
            $mes_text = $rec['mes_text'];
            if ( $replace_arr ) {
                foreach ( $replace_arr as $key => $val )
                    $mes_text = ereg_replace("<val$key>", $val, $mes_text);
            }
        }
        return $mes_text;
    }



    function getPunterByUsername($username) {
        global $dbh;
        include_once 'JurisdictionsList.class.inc';
        $jurisdiction = JurisdictionsList::getInstance ( $dbh );
        $clubs=implode(",",$jurisdiction->getChilJurisdictionsIdList($_SESSION['jurisdiction_id'],'club'));
        //Select punter details record using punter id
        $sql  = "select pun_id, pun_first_name, pun_middle_name, pun_last_name";
        $sql .= ", pun_member_type, pcr_credits, pcr_bonus , pun_access, pcr_total_bets";
        $sql .= ", pcr_total_wins, pun_email, pun_username,  pun_reg_date";
        $sql .= ", pun_customer_number, pun_identified";
        $sql .= ", pun_notes, pun_cou_code, pun_investigate";
        $sql .= ", pun_dob, pun_address_line1, pun_address_line2, pun_city_suburb";
        $sql .= ", pun_state_province, pun_postcode_zip, pun_phone_business";
        $sql .= ", pun_phone_home, pun_phone_mobile, pun_fax, pun_daily_allowance";
        $sql .= ", pun_gender, pun_temp_password, pun_num_logins, pun_last_logged_in";
        $sql .= ", pcr_reserved_funds, pun_lock_reason, pun_id_documents, pun_preg_code";
        $sql .= ", pun_login_lock_start, pun_email_reg_code, pun_sms_reg_code";
        $sql .= ", pun_registration_status, pun_pre_registered, pcr_total_deposits";
        $sql .= ", pcr_total_withdrawals, pun_password_set_time, pun_sre_code";
        $sql .= ", pun_last_request_time, pun_num_failed_logins, pun_lang,cur_code,cur_code_for_web ";
        $sql .= " from punter, punter_credit,jurisdiction, currency where";
        $sql .= " pun_username = '".$dbh->escape($username)."' and pun_id = pcr_pun_id and jur_id=pun_betting_club and jur_currency=cur_id";
        $sql .= " AND pun_betting_club in (".$dbh->escape($clubs).") and pun_id = pcr_pun_id";
        $rs = $dbh->exec($sql);
        if ( $rs->getNumRows() > 0 ) {
            $new_row = $rs->next();
        }
        return $new_row;
    }

    function breakPoint($displayval="") {
        showval($displayval);
        showval("<font color=red>(BREAKPOINT)</font>");
        exit;
    }

    function setNav($include_filename) {
        $GLOBALS['side_nav'] = "$include_filename.inc";
    }


    function hasCustomerEditPriv ($aty_code) {
        return in_array($aty_code, array('SUPPORT','SUPERUSER', 'ACCOUNTSADMINISTRATOR', 'CLUBMANAGER','CASINOMANAGER'));
    }

    function hasCustomerDeletePriv ($aty_code) {
        return in_array($aty_code, array('SUPPORT','SUPERUSER','CASINOMANAGER'));
    }


    function hasCustomerTransactionPriv ($aty_code) {
        return in_array($aty_code, array('SUPPORT','SUPERUSER','CASHIER'));
    }


    function hasCustomerAdjustmentTransactionPriv ($aty_code) {
        return in_array($aty_code, array('SUPPORT','SUPERUSER','FINANCIALMANGER'));
    }


    function hasCustomerViewTransactionPriv ($aty_code) {
        return in_array($aty_code, array('SUPPORT','SUPERUSER','CASINOMANAGER', 'CLUBMANAGER', 'ACCOUNTSADMINISTRATOR', 'FINANCIALMANAGER'));
    }

    function isManager ($aty_code) {
        return in_array($aty_code, array('SUPPORT','SUPERUSER','CASINOMANGER'));
    }



    function dec2hex2 ( $textString ) {
        $hexequiv = Array ("0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F");
        return $$hexequiv[($textString >> 4) & 0xF] + $hexequiv[$textString & 0xF];
    }

    function dec2hex4 ( $textString ) {
        $hexequiv = Array ("0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F");
        return $hexequiv[($textString >> 12) & 0xF] + $hexequiv[($textString >> 8) & 0xF] + $hexequiv[($textString >> 4) & 0xF] + $hexequiv[$textString & 0xF];
    }

    function convertCP2UTF16 ( $textString )
    {
        $outputString = "";
        $textString = preg_replace('/^\s+/', '',$textString, $limit=1);
        if (strlen($textString) == 0) { return ""; }
        $textString = preg_replace('/^\s+/', '',$textString);

        $listArray = explode(' ',$textString);
        $listArrayLen = count($listArray);
        for ( $i=0; $i<$listArrayLen; $i++ )
        {
            $n = hexdec($listArray[$i]);
            if ($i > 0) { $outputString .= ' ';}
            if ($n <= 0xFFFF)
            {
                $outputString .= dec2hex4($n);
            }
            else if ($n <= 0x10FFFF)
            {
                $n -= 0x10000;
                $outputString .= dec2hex4(0xD800 | ($n >> 10)) . ' ' . dec2hex4(0xDC00 | ($n & 0x3FF));
            }
            else
            {
                $outputString .= '!errorr ' . dechex($n) .'!';
            }
        }
        return $outputString;
    }



    function convertUTF82CP ( $textString ) {
        require 'utf8.inc';
        $outputString = "";
        $compte = 0;
        $n = 0;

        $textString = preg_replace("/^\s+/", '',$textString, $limit=1);
        if (strlen($textString) == 0) { return ""; }
        $textString = preg_replace("/^\s+/", '',$textString);

        //$listArray = explode(' ',$textString);
        $listArray =  & utf8ToUnicode($textString);
        $listArrayLen = count($listArray);
        //breakpoint($listArray);
        for ( $i=0; $i<$listArrayLen; $i++ )  {
            //$b = hexdec($listArray[$i]); // alert('b:'+dec2hex(b));
            $b = $listArray[$i];
            switch ($compte) {
                case 0:
                    if (0 <= $b && $b <= 0x7F) {  // 0xxxxxxx
                        $outputString .= dechex($b) . ' ';
                    } else if (0xC0 <= $b && $b <= 0xDF) {  // 110xxxxx
                        $compte = 1;
                        $n = $b & 0x1F;
                    } else if (0xE0 <= $b && $b <= 0xEF) {  // 1110xxxx
                        $compte = 2;
                        $n = $b & 0xF;
                    } else if (0xF0 <= $b && $b <= 0xF7) {  // 11110xxx
                        $compte = 3;
                        $n = $b & 0x7;
                    } else {
                        $outputString .= '!error ' . dechex($b) . '! ';
                    }
                    break;
                case 1:
                    if ($b < 0x80 || $b > 0xBF) {
                        $outputString .= '!error ' . dechex($b) . '! ';
                    }
                    $compte--;
                    $outputString .= dechex(($n << 6) | ($b-0x80)) . ' ';
                    $n = 0;
                    break;
                case 2: case 3:
                if ($b < 0x80 || $b > 0xBF) {
                    $outputString .= '!error ' . dechex($b) . '! ';
                }
                $n = ($n << 6) | ($b-0x80);
                $compte--;
                break;
            }
        }
        return preg_replace('/ $/', '', $outputString);
    }


    function convertUTF8toUTF16($string) {
        $cp_str = convertUTF82CP($string);
        return convertCP2UTF16 ( $cp_str);
    }


    /*
    function convertChar2CP ( $textString ) {
    var $outputString = "";
    var $haut = 0;
    var $n = 0;

    for ( $i=0; $i<$textString.length; i++) {
    var b = textString.charCodeAt(i);
    if (b < 0 || b > 0xFFFF) {
    outputString += '!erreur ' + dec2hex(b) + '!';
    }
    if (haut != 0) {
    if (0xDC00 <= b && b <= 0xDFFF) {
    outputString += dec2hex(0x10000 + ((haut - 0xD800) << 10) + (b - 0xDC00)) + ' ';
    haut = 0;
    continue;
    } else {
    outputString += '!erreur ' + dec2hex(haut) + '!';
    haut = 0;
    }
    }
    if (0xD800 <= b && b <= 0xDBFF) {
    haut = b;
    } else {
    outputString += dec2hex(b) + ' ';
    }
    }
    return( outputString.replace(/ $/, '') );
    }
    */

    /*function include_content($file) {
      global $cookie_perm,   // permanent cookie
      $cookie,       // temporary cookie
      $customer_row,  // customer record (will be set if the customer is logged in)
      $site_avail_langs; // available site languages

      // if the customer is logged in, but the permanent cookie containing
      // the language preference was not sent with the last request.
      // then we default to the punter's preferred language in their db record.
      // the permanent cookie is resent to the customer when a set_language GET arg
      // added to a requested

      if ( $cookie_perm->wasSentByClient() ) {  // check permanent cookie first
        // use value from cookie
        $language_code = substr($cookie_perm->getFieldValue('language'), 0, 2);
      }
      elseif ( $cookie->wasSentByClient() ) {  // else check temporary cookie
        // use value from cookie
        $language_code = substr($cookie->getFieldValue('language'), 0, 2);
      }
      else { // no cookies sent by browser? use browser accept language
        if ( !empty($_SERVER['HTTP_ACCEPT_LANGUAGE']) ) {
          $accept_lang =  strtoupper(substr($_SERVER['HTTP_ACCEPT_LANGUAGE'], 0, 2));

          if ( in_array($accept_lang, $site_avail_langs) ) {
            $language_code = $accept_lang;
          }
        }
      }

      if ( ! in_array($language_code, $site_avail_langs) ) {
        unset($language_code);
      }

      $default_code  = 'EN';

      if( empty($language_code) )
      $language_code = $default_code;

      if ($language_code == "BR") {
        $language_code = "PT";
      }

      //
      $rpos          = strrpos($file, '.');
      $paths         = explode(':', ini_get('include_path'));

      $inc_file_l    = substr($file, 0, $rpos) . '_' . strtolower($language_code) . substr($file, $rpos);
      $inc_file_d    = substr($file, 0, $rpos) . '_' . strtolower($default_code)  . substr($file, $rpos);
      //echo "$inc_file_l<br>";
      //echo "$inc_file_d<br>";
      foreach($paths as $path) {
        if(file_exists($path . DIRECTORY_SEPARATOR . $inc_file_l)) {
          return $inc_file_l;
        }
        else {
          if($default_code != $language_code) {
            if(file_exists($path . DIRECTORY_SEPARATOR . $inc_file_d)) {
              return $inc_file_d;
            }
          }
        }
      }
      return '';
    }*/

    function _T($file) {
        return include_content($file);
    }

    function getClubName($id) {
        global $dbh;
        $sql = "select jur_name from jurisdiction where jur_id=$id";
        $rs = $dbh->exec($sql);
        if ( $rs->getNumRows() == 1 ) {
            $row =$rs->next();
            $name = $row['jur_name'];
        }
        return $name;
    }

    /**
     * @return string
     * @param string $str
     * @desc Escapes single quotes (changes ' to \');
     */
    function escapeSingleQuotes($str) {
        $str = mb_ereg_replace("\\\\", "\\\\", $str); //escape slashes
        return mb_ereg_replace("'", "\'", $str); //escape single quotes
    }

    /**
     * @return string
     * @param string $str
     * @desc converts double quotes (") to html entity (&amp;quot);
     */
    function replace_quotes($str) { return mb_ereg_replace('"', '&quot;', $str);  }

    /**
     * @return string
     * @param string $str
     * @desc returns a string that can be used as a column insert for quoted column (text/varchar/char/date) type
     */
    function db_quote($str) {
        global $dbh;

        return $dbh->prepareString($str);
        //return "'".escapeSingleQuotes($str)."'";
    }

    function form_td_style ($field_name) {
        $message = fieldInError($field_name);

        echo 'id="td_'.$field_name.'"';

        if ( $message ) {
            echo ' class="contenterror" ';
        }
        else {
            echo ' class="content" ';
        }
    }


    function err_field($field_name, $no_br=false) {
        $message = fieldInError($field_name);

        if ( $message ) {
            if ( $no_br ) { return $message; }
            else {
                return '<br /><span style="font-weight:bold;color:yellow">[ERROR: '. $message.']</span>';
            }
        }
        return '';
    }


    function check_access ($task_code, $do_redirect=false, $popup=false)  {
        global $dbh;
        if ( !isLoggedIn() ) {
            header("Location: /?err=expired");
            exit;
        }

        if (empty($_SESSION["access"])) {
            $sql = "SELECT rule_task_code FROM admin_access_rule " .
                "  WHERE rule_aty_id = " . $_SESSION['aty_id'];
            $rs  = $dbh->exec($sql);
            $access = array();
            while ($rs->hasNext()) {
                $r = $rs->next();
                $access[] = $r["rule_task_code"];
            }
            $_SESSION["access"] = $access;
        }
        else {
            $access = $_SESSION["access"];
        }

        //  $sql = "SELECT count(*) from admin_access_rule WHERE rule_aty_id = " . $_SESSION['aty_id'].
        //           "   AND rule_task_code = " . $dbh->prepareString($task_code);
        //  $count =$dbh->queryOne($sql);

        $count = 0;
        if (in_array($task_code, $access)) {
            $count = 1;
        }

        if ( 1 == $count ) {
            return true;
        }
        else {
            if ( $do_redirect ) {
                $_SESSION['restrict_access_msg'] = 'You do not have the proper privileges to access the section that you requested';
                if ( $popup ) {
                    jscriptBlock("window.opener.location='".secure_host."/'; window.close()");
                }
                else{
                    header('Location:'.secure_host.'/');
                }
                exit;
            }
            return false;
        }
    }


    function get_jurisdiction($id) {
        global $dbh;
        $sql = "SELECT jur_id, jur_name, jur_class, jur_parent_id, jur_address1, jur_address2, " .
            "jur_postcode_zip, jur_notes, jur_phone FROM jurisdiction " .
            "WHERE jur_id = $id";
        $rs = $dbh->exec($sql);
        $num_rows = $rs->getNumRows();

        if ( 1 == $num_rows ) {
            $row = $rs->next();
            return $row;
        }
    }


    function new_session_id ($session_id) {
        if ( preg_match('/^[A-Za-z0-9]{32,32}$/', $session_id) ) {
            foreach ( $_SESSION as $key => $val ) {
                $session_vars[$key] = $val;
            }
            session_id($session_id); // set new session id
            session_start(); // start new session
            foreach ( $_SESSION as $key => $val ) {
                $_SESSION['key'] = $val;
            }
        }
        return session_id();
    }


    function currency($number){
        $symbol = defined('CURRENCY_SYMBOL')?CURRENCY_SYMBOL:'&#8364;';
        $decimal = number_format($number,2);

        $string = $symbol . $decimal;

        return $string;
    }

    function removeUnsafeAttributesAndGivenTags($input, $validTags = ''){
        $regex = '#\s*<(/?\w+)\s+(?:on\w+\s*=\s*(["\'\s])?.+?\(\1?.+?\1?\);?\1?|style=["\'].+?["\'])\s*>#is';
        return preg_replace($regex, '<${1}>',strip_tags($input, $validTags));
    }

    function escapeAndQuote($string, $type){
        global $dbh;

        $string = trim($string);

        if(is_null($string) || $string == ""){
            return "NULL";
        }

        if($type == "string"){
            $string = str_ireplace("�", "&euro;", $string);

            //$string = str_replace("�", "&euro;", $string);
        }

        $string = $dbh->escape($string);

        if($type == "string" || $type == "date"){
            $string = "'$string'";
        }

        if($type == "date"){
            $string = "DATE_FORMAT($string, '%Y-%m-%d %H:%i')";
        }

        if($type == "boolean"){
            $string = (($string >= 1)?(1):(0));
        }

        if($type == "integer"){
            $string = (int)$string;
        }

        return $string;
    }

    function checkType($data, $type = "string", $rules = null, $nullable){
        if(is_null($data) || $data == ""){
            return $nullable;
        }

        switch ($type){
            case "string":
                return is_string($data);
                break;
            case "integer":
                if(is_numeric($data)){
                    return is_int((int)$data);
                }
                return false;
                break;
            case "decimal":
                if(is_numeric($data)){
                    return is_float((float)$data);
                }
                return false;
                break;
            case "boolean":
                return is_bool((boolean)$data);
                break;
            case "date":
                if(is_array($data)){
                    $data = $data["year"] . "-" . $data["month"] . "-" . $data["day"] . " " . $data["hour"] . ":" . $data["minute"];
                }
                return strtotime($data) > 0;
                break;
        }

    }

    function string2regexp($str){
        $str = preg_replace('#\W#', '.', $str);
        $charcount = preg_match_all('#\w#', $str, $m);
        if ($charcount < 3) {
            return null;
        }
        return strtolower("^$str");
    }

    function randomCode($length = 8){
        $key = '';
        for($i = 0 ; $i < $length ; $i++){
            $key .= rand(0,9);
        }
        return $key;
    }

    function generateCouponCode(){
        $consonants = "wrtpsdfghjklzxcvbnmq";
        return strtoupper(
            escapeAndQuote(
                $consonants[rand(0, strlen($consonants) - 1)] .
                $consonants[rand(0, strlen($consonants) - 1)] .
                $consonants[rand(0, strlen($consonants) - 1)] .
                randomCode(4) .
                $consonants[rand(0, strlen($consonants) - 1)] .
                $consonants[rand(0, strlen($consonants) - 1)] .
                randomCode(3)
                , "string")
        );
    }

    function addPunterNote($pun_id, $aus_id, $message){
        global $dbh;
        if(empty($pun_id) || empty($aus_id) || empty($message)){
            return false;
        }
        $sql = "INSERT INTO admin_user_note VALUES ("  . $dbh->nextSequence('AUN_ID_SEQ').", " . escapeAndQuote($pun_id, "integer") . ", " . escapeAndQuote($aus_id, "integer") . ", CURRENT_TIMESTAMP, " . escapeAndQuote($message, "string") . ")";
        return ($dbh->exec($sql) === 1);
    }

    function checkNinCode($cf){
        return true;
        if( $cf == '' )  return false;
        if( strlen($cf) != 16 )
            return false;
        $cf = strtoupper($cf);
        if( ! ereg("^[A-Z0-9]+$", $cf) ){
            return false;
        }
        $s = 0;
        for( $i = 1; $i <= 13; $i += 2 ){
            $c = $cf[$i];
            if( '0' <= $c && $c <= '9' )
                $s += ord($c) - ord('0');
            else
                $s += ord($c) - ord('A');
        }
        for( $i = 0; $i <= 14; $i += 2 ){
            $c = $cf[$i];
            switch( $c ){
                case '0':  $s += 1;  break;
                case '1':  $s += 0;  break;
                case '2':  $s += 5;  break;
                case '3':  $s += 7;  break;
                case '4':  $s += 9;  break;
                case '5':  $s += 13;  break;
                case '6':  $s += 15;  break;
                case '7':  $s += 17;  break;
                case '8':  $s += 19;  break;
                case '9':  $s += 21;  break;
                case 'A':  $s += 1;  break;
                case 'B':  $s += 0;  break;
                case 'C':  $s += 5;  break;
                case 'D':  $s += 7;  break;
                case 'E':  $s += 9;  break;
                case 'F':  $s += 13;  break;
                case 'G':  $s += 15;  break;
                case 'H':  $s += 17;  break;
                case 'I':  $s += 19;  break;
                case 'J':  $s += 21;  break;
                case 'K':  $s += 2;  break;
                case 'L':  $s += 4;  break;
                case 'M':  $s += 18;  break;
                case 'N':  $s += 20;  break;
                case 'O':  $s += 11;  break;
                case 'P':  $s += 3;  break;
                case 'Q':  $s += 6;  break;
                case 'R':  $s += 8;  break;
                case 'S':  $s += 12;  break;
                case 'T':  $s += 14;  break;
                case 'U':  $s += 16;  break;
                case 'V':  $s += 10;  break;
                case 'W':  $s += 22;  break;
                case 'X':  $s += 25;  break;
                case 'Y':  $s += 24;  break;
                case 'Z':  $s += 23;  break;
            }
        }

        if( chr($s%26 + ord('A')) != $cf[15] )
            return false;

        return true;
    }

    function getSkinId($jur_id){
        global $dbh;

        $sql = "SELECT jur_skn_id FROM jurisdiction WHERE jur_id = ".$dbh->escape($jur_id);
        $skin_id = $dbh->queryOne($sql);

        if(is_null($skin_id)){
            $skin_id = -1;
        }

        return $skin_id;
    }

    function getSkinClubId($jur_id){
        global $dbh;
        $club_id = $_SESSION["skin_club_id"];
        if(empty($club_id) && $club_id != -1){
            $sql = "SELECT skn_internet_club_id FROM skins WHERE skn_jur_id = $jur_id";
            $club_id = $dbh->queryOne($sql);

            if(is_null($club_id)){
                $club_id = -1;
            }

            $_SESSION["skin_club_id"] = $club_id;
        }
        return $club_id;
    }

    function getSkinDetailsByJurisdiction($jur_id, $jur_class){
        $ret = null;

        if(!isset($_SESSION["skin_details"]) || 1 == 1){
            global $dbh;

            $sql = "SELECT
              skins.*, (SELECT jur_class FROM jurisdiction WHERE jur_id = skins.skn_jur_id) as jur_class
            FROM
              jurisdiction club
            LEFT JOIN
              jurisdiction district ON district.jur_id = club.jur_parent_id
            LEFT JOIN
              jurisdiction region   ON region.jur_id = district.jur_parent_id
            LEFT JOIN
              jurisdiction nation   ON nation.jur_id = region.jur_parent_id
            LEFT JOIN
              skins ON (skn_jur_id = club.jur_id OR skn_jur_id = district.jur_id OR skn_jur_id = region.jur_id OR skn_jur_id = nation.jur_id) AND skn_status = '1' ";


            switch ($jur_class){
                case "club":
                    $sql .= "WHERE club.jur_id = $jur_id";
                    break;
                case "district":
                    $sql .= "WHERE district.jur_id = $jur_id";
                    break;
                case "region":
                    $sql .= "WHERE region.jur_id = $jur_id";
                    break;
                case "casino":
                    $sql .= "WHERE skn_id = 0";
                    break;
                default:
                    $sql .= "WHERE 1 = 0";
                    break;
            }

            $sql .= " OR skn_id = 0";

            $rs = $dbh->exec($sql);

            $jur_r = array();

            while($rs->hasNext()){
                $row = $rs->next();
                $jur_r[$row["jur_class"]] = $row;
            }

            $ret           = $jur_r["nation"];

            /*
            echo "<pre>";
            var_dump($jur_r);
            echo "</pre>";
            */

            $order = array(
                "club"     => 0,
                "district" => 1,
                "region"   => 2,
                "nation"   => 3,
                "casino"   => 4
            );

            if(!empty($jur_r["club"])           && ($order[$jur_class] <= $order["club"])){
                $ret = $jur_r["club"];
            }elseif (!empty($jur_r["district"]) && ($order[$jur_class] <= $order["district"])){
                $ret = $jur_r["district"];
            }elseif (!empty($jur_r["region"])   && ($order[$jur_class] <= $order["region"])){
                $ret = $jur_r["region"];
            }elseif (!empty($jur_r["nation"])   && ($order[$jur_class] <= $order["nation"])){
                $ret = $jur_r["nation"];
            }elseif (!empty($jur_r["casino"])   && ($order[$jur_class] <= $order["casino"])){
                $ret = $jur_r["casino"];
            }

            $_SESSION["skin_details"] = $ret;
        }else{
            $ret = $_SESSION["skin_details"];
        }
        return $ret;
    }

    function isolateDomainName($url){
        $ret = preg_replace("/^https?:\/\//", "", $url);
        $ret = strtolower($ret);
        if (!preg_match("#\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\b#", $ret)) {
            preg_match('/([^.]+\.[^.:]+)[:0-9]*$/', $ret, $m);
            $ret = $m[1];
        }
        $ret = preg_replace("/\//", "", $ret);

        return $ret;
    }

    //return array
    function getPunterDocuments($pun_id){
        $documents = array("idcard_front", "idcard_back");


        array_push($documents, "nin_code_front");
        array_push($documents, "photo");
        array_push($documents, "firma");
        array_push($documents, "creditcard");
        array_push($documents, "extra");

        $ret = array();
        foreach ($documents as $doc){
            $filename = getUploadedDocFileName($pun_id, $doc);
            $file_path = CUSTOMERS_DOCS_PATH . "/" . $filename;
            $file_list = glob(CUSTOMERS_DOCS_PATH . "/" . $filename . "*");

            foreach ($file_list as $key => $fname){
                if(eregi("$filename(\.[a-zA-Z0-9]{3,4})$", $fname, $match)){
                    $file_list[$key] = $match[1];
                }else{
                    unset($file_list[$key]);
                }
            }


            $ret[$doc] = implode("|", $file_list);
        }
        return $ret;
    }

    function getUploadedDocFileName($pun_id, $doc_type, $isThumb=false){
        return floor($pun_id / 1000) . "000/" . floor($pun_id / 100) . "00/" . $pun_id . "_" . $doc_type . (($isThumb)?("_t"):(""));
    }

    function saveDocumentImage($pun_id, $doc_file_ref, $doc_type){
        if(!file_exists($doc_file_ref)){
            return false;
        }

        $file_name       = getUploadedDocFileName($pun_id, $doc_type, false);
        $thumb_file_name = getUploadedDocFileName($pun_id, $doc_type, true);
        $file_path       = CUSTOMERS_DOCS_PATH . "/" . $file_name;
        $thumb_file_path = CUSTOMERS_DOCS_PATH . "/" . $thumb_file_name;

        if(!is_dir(dirname($file_path))){

            mk_dir(dirname($file_path));
        }


        require_once 'Image/Transform.php';
        $it = Image_Transform::factory('GD');

        $it->load($doc_file_ref);
        $it->scaleByX(800);
        $it->save($file_path . ".jpg");

        $it->free();

        addPunterNote($pun_id,$_SESSION['admin_id'],"Admin user uploaded ".$doc_type);

        $it->load($file_path . ".jpg");
        $it->scaleByX(120);
        $it->save($thumb_file_path . ".jpg");
        $it->free();
    }

    //recursively creates a folder.
    function mk_dir($path, $rights = 0777) {
        if (!@is_dir($path)) {
            $folder_path = array($path);
        } else {
            return;
        }

        while(!@is_dir(dirname(end($folder_path)))
            && dirname(end($folder_path)) != '/'
            && dirname(end($folder_path)) != '.'
            && dirname(end($folder_path)) != '')
        {
            array_push($folder_path, dirname(end($folder_path)));
        }

        while($parent_folder_path = array_pop($folder_path)) {
            if(!@mkdir($parent_folder_path, $rights)) {
                user_error("Can't create folder \"$parent_folder_path\".\n");
            }
        }
    }

    function checkMessages(){
        global $dbh;
        $now = time();

        $admin_user_id = $_SESSION['admin_id'];
        if(!empty($admin_user_id)){
            if(empty($_SESSION["messages"]) || $now - $_SESSION["messages"]["last_check"] >= 60){

                $sql = "SELECT aum_id, aum_level
            FROM admin_user_mailbox 
            JOIN admin_user f ON f.aus_id = aum_from 
            JOIN jurisdiction fj ON fj.jur_id = f.aus_jurisdiction_id
            WHERE aum_status = 0 AND aum_to = $admin_user_id ORDER BY aum_send_date";
                $rs = $dbh->exec($sql);
                $_SESSION["messages"]["last_check"] = $now;
                $_SESSION["messages"]["inbox"]      = $rs->Records;
            }
        }
        return $_SESSION["messages"]["inbox"];
    }

    function get_free_jurcode(){
        global $dbh;
        $ret = null;

        $rs = $dbh->exec("SELECT get_free_jurcode() as get_free_jurcode");
        if($rs->hasNext()){
            $row = $rs->next();
            $jur_code = $row["get_free_jurcode"];

            if(!empty($jur_code)){
                $ret = $jur_code;
            }
        }

        return $ret;
    }
    /*
    function serverCmd($host, $port, $cmd){
       $http = fsockopen($host, $port);

       // Send a request to the server
       $req  = $cmd;
       fputs($http, $req);

       // Output the request results
       while(!feof($http)) {
          echo fgets($http, 1024);
       }

       // Close the connection
       fclose($http);
    }
    */


    function serverCmd($host, $port, $cmd) {
        $sock = fsockopen( $host, $port );
        if ($sock){
            fputs($sock, $cmd);
            //stream_set_timeout($sock, 0);
            $info = stream_get_meta_data($sock);
            $s="";

            while(!feof($sock) && !$info["timed_out"]){
                $s    .= trim(fgets($sock, 4096));
                $info  = stream_get_meta_data($sock);
            }

            fclose($sock);
            return $s;
        }
    }

    function mb_unserialize($string) {
        $string = preg_replace('!s:(\d+):"(.*?)";!se', "'s:'.strlen('$2').':\"$2\";'", $string);
        return unserialize($string);
    }

    /*
    function serverCmd($host, $port, $cmd) {
      $sock = @fsockopen( $host, $port );
      if ($sock){

        //echo $host . ":" . $port . "\n";
        //echo $cmd . "\n";

        stream_set_timeout($sock, 60);
        fputs( $sock, $cmd ."\n");
        $info = stream_get_meta_data($sock);
        $s="";
        while(!feof($sock) && !$info["timed_out"]){
          $res   = trim(fgets($sock, 4096));
          if($res != ""){
            $s    .= $res . "\n";
          }
          $info  = stream_get_meta_data($sock);
        }
        $info = stream_get_meta_data($sock);
        fclose($sock);
        return $s;
      }else{
        throw new Exception("Error connecting to $host:$port");
        return null;
      }
    }
    */
    function getAllClubsUnderJurisdiction($jur_id,$recent=false) {
        global $dbh;
        $jur_class=$_SESSION['jurisdiction_class'];
        $sql = "select c.jur_name club_name,c.jur_id club_id,d.jur_name district_name
            FROM jurisdiction c
            JOIN jurisdiction d on d.jur_id = c.jur_parent_id
    ";
        if($jur_class == "club"){
            $sql .= " WHERE c.jur_id = " . $jur_id;
        }
        elseif($jur_class == "district"){
            $sql .= "
				  WHERE d.jur_id = " . $jur_id;
        }
        elseif($jur_class=="region"){        $sql .="
				  JOIN jurisdiction r on r.jur_id = d.jur_parent_id
				  WHERE r.jur_id=".$jur_id;
        }
        elseif ($jur_class=="nation"){
            $sql .=" JOIN jurisdiction r on r.jur_id = d.jur_parent_id
				 JOIN jurisdiction n on n.jur_id = r.jur_parent_id
				WHERE n.jur_id=".$jur_id;
        }
        else{
            $sql .=" WHERE 1=1";
        }
        $sql.=" AND c.jur_class='club'
            AND c.jur_status=1
    ";
        if($recent){
            $sql.=" AND c.JUR_CREATION_DATE > now() - interval 120 day";
        }
        $result = $dbh->doCachedQuery ( $sql, 0 );
        return $result;
    }


    function rand_str($length = 32, $chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890') {
        // Length of character list
        $chars_length = (strlen($chars) - 1);
        // Start our string
        $string = $chars{rand(0, $chars_length)};
        // Generate random string
        for ($i = 1; $i < $length; $i = strlen($string)) {
            // Grab a random character from our list
            $r = $chars{rand(0, $chars_length)};
            // Make sure the same two characters don't appear next to each other
            if ($r != $string{$i - 1}) $string .=  $r;
        }
        // Return the string
        return $string;
    }

    function month_select_box($defaultmonth=false,$betting=false,$type='select')
    {
        global $lang;
        if($type=='select') {
            $month_options = '<option value="" >' . $lang->getLang("Select a month") . '</option>';
        }
        else{
            $month_options=[];
        }
        $curryear = date("Y");
        $firstyear=$curryear-3;
        for ($j = $firstyear; $j <= $curryear; $j++) {
            for ($i = 1; $i <= 12; $i++) {
                if($betting){
                    $currMonth=$j."-".($i<10? "0".$i:$i);
                    if(date('D',strtotime($currMonth))=='Tue'){
                        $firstmonth=date('Y-m',strtotime($currMonth));
                        $month_num = date('Y-m-d',strtotime($firstmonth));
                        $month_name = date('F', mktime(0, 0, 0, $i + 1, 0, 0, 0));
                        $month_num2 = date('Y-m-d',strtotime("Last Monday of ".$firstmonth));
                    }
                    else {
                        $firstmonth = ($i == 1 ? ($j - 1) . "-12" : $j . "-" . ($i - 1));
                        $month_num = date('Y-m-d',strtotime("Last Tuesday of ".$firstmonth));
                        $month_name = date('F', mktime(0, 0, 0, $i + 1, 0, 0, 0));
                        $month_num2 = date('Y-m-d',strtotime("Last Monday of".date('Y-m',strtotime("$firstmonth+1 month"))));
                    }


                    if((date('Y-m')) >= date('Y-m',strtotime($j."-".$i))) {
                        if($type=='select') {
                            $month_options .= '<option value="' . ($month_num) . '~' . ($month_num2) . '" ' . ($defaultmonth == ($month_num . "~" . $month_num2) ? 'selected' : '') . '  >' . $lang->getLang($month_name) . ' ' . $j . '</option>';
                        }
                        else{
                            array_push($month_options,[$month_num,$month_num2]);
                        }
                    }


                }
                else{
                    $month_num = str_pad($i, 2, 0, STR_PAD_LEFT);
                    $month_name = date('F', mktime(0, 0, 0, $i + 1, 0, 0, 0));
                    if((date('Y-m')) >= date('Y-m',strtotime($j.'-'.$month_num))) {
                        $month_options .= '<option value="' . $j . '-' . ($month_num) . '" '.($defaultmonth==($j.'-'.$month_num)? 'selected':'').'  >' . $lang->getLang($month_name) . ' ' . $j . '</option>';
                    }
                }
            }

        }
        if($type=='select') {
            return '<select id="monthHelper" name="monthHelper" class="monthHelper">' . $month_options . '</select>';
        }
        else{
            return $month_options;

        }
    }

    function month_select_box2($defaultmonth=false,$monthsNumber=1)
    {
        global $lang;
        $month_options = '<option value="" >'.$lang->getLang("Select months").'</option>';
        $months=$monthsNumber-1;
        $curryear = date("Y");
        $firstyear=$curryear-3;
        for ($j = $firstyear; $j <= $curryear; $j++) {
            for ($i = 1; $i <= 12; $i++) {
                $currMonth=$j."-".($i<10? "0".$i:$i);
                if(date('D',strtotime($currMonth))=='Tue'){
                    $firstmonth=date('Y-m',strtotime($currMonth));
                    $month_num1 = date('Y-m-d',strtotime($firstmonth));
                    $month_name1 = date('F', strtotime($j."-".$i))." ".$j;
                    $month_num2 = date('Y-m-d',strtotime("Last Monday of".date('Y-m',strtotime("$month_num1+".($months)." months"))));
                    $month_name2 = date('F',strtotime("$month_num1++".($months)." months"))." ".date('Y',strtotime("$month_num1+".($months)." months"));
                }
                else {
                    $month_num1 = date('Y-m-d',strtotime("Last Tuesday of ".($i==1? ($j-1)."-12" : $j."-".($i-1))));
                    $month_name1 = date('F', strtotime($j."-".$i))." ".$j;
                    $month_num2 = date('Y-m-d',strtotime("Last Monday of".date('Y-m',strtotime("$month_num1+".($months+1)." months"))));
                    $month_name2 = date('F',strtotime("$month_num1++".($months+1)." months"))." ".date('Y',strtotime("$month_num1+".($months+1)." months"));
                }

                if((date('Y-m')) >= date('Y-m',strtotime($j."-".$i))) {
                    $month_options .= '<option value="'. ($month_num1) . '~'.($month_num2)  .'" '.($defaultmonth==($month_num1."~".$month_num2)? 'selected':'').'  >' . $month_name1.' untill '.$month_name2. '</option>';
                }
            }
        }
        return '<select id="monthHelper'.$monthsNumber.'" name="monthHelper'.$monthsNumber.'" class="monthHelper">' . $month_options . '</select>';
    }

    function week_select_box($defaultweek=false,$betting=false,$type='select')
    {
        global $lang;
        $year           = date('Y', strtotime('-1 year'));
        $firstDayOfYear = mktime(0, 0, 0, 1, 1, $year);
        $nextMonday     = strtotime('monday', $firstDayOfYear);
        $nextSunday     = strtotime('sunday', $nextMonday);
        if($betting){
            $nextMonday     = strtotime('Tuesday', $firstDayOfYear);
            $nextSunday     = strtotime('Monday', $nextMonday);
        }
        if($type=='select') {
            $week_options = '<option value="" disabled>'.$lang->getLang("Select a week").'</option>';
        }
        else{
            $week_options=[];
        }

        $i=1;
        while ( $nextMonday <= strtotime('now') ) {
            $prevYear=$year;
            $year=date('Y', ($nextMonday));
            if($prevYear!=$year){
                $i=1;
            }
            if($type=='select') {
                $week_options .= '<option value="'.date('Y-m-d', $nextMonday). '~'. date('Y-m-d', $nextSunday). '" '.($defaultweek==(date('Y-m-d', $nextMonday).'~'. date('Y-m-d', $nextSunday))? 'selected':'').' >'.$year.' '.$lang->getLang('Week').' # '.$i.', '.date('d M', $nextMonday).' - '. date('d M', $nextSunday).'</option>';
            }
            else{
                array_push($week_options,[date('Y-m-d', $nextMonday),date('Y-m-d', $nextSunday)]);
            }
             $nextMonday = strtotime('+1 week', $nextMonday);
            $nextSunday = strtotime('+1 week', $nextSunday);
            $i++;
        }
        if($type=='select') {
            return '<select id="weekHelper" name="weekHelper" class="weekHelper" size="3">' . $week_options . '</select>';
        }
        else{
            return  $week_options ;
        }

    }

    function getBettingInterval($month,$type='weekly'){
        $interval=array();
        if(date('D',strtotime($month))=='Tue'){
            $firstDay=date('Y-m-d',strtotime($month));
        }
        else {
            $firstDay=date('Y-m-d',strtotime("Last Tuesday of ".date('Y-m',strtotime($month.' -1 month'))));
        }
        $lastDay = date('Y-m-d',strtotime("Last Monday of $month"));
        if($type!='weekly'){
            $interval['start']=$firstDay;
            $interval['end']= $lastDay;
            return $interval;
        }

        $prev=0;
        $last=0;
        While(strtotime($lastDay)>strtotime($last) && strtotime('now')>strtotime($last)){
            $section=array();

            if($prev==0){
                $prev=$firstDay;


            }
            elsE{
                $prev=date('Y-m-d',strtotime($prev." +7 days" ));
            }
            $last= date('Y-m-d',strtotime( $prev." +6 days"));
            $section['start']=$prev;
            $section['end']=$last;
            array_push($interval,$section);
        }
        return $interval;
    }

    function toJsonTable($i,$children){
        $output = array(
            "sEcho" => intval($_REQUEST['sEcho']),
            "iTotalRecords" => intval($i),
            "iTotalDisplayRecords" => intval($i),
            "aaData" => $children
        );
        return json_encode($output);
    }


    function calculateTaxValue($amount,$tax){
        if($tax<100){
            return  $amount*$tax/100;
        }
        else{
            return ($tax-100)*$amount/$tax;
        }
    }


    /**
     * @param $jurisdictionClass
     * @return string
     */
    function getAllSubJurisdictions($jurisdictionClass){
        $subjurisdiction='';
        $jurisdictionClass=strtolower($jurisdictionClass);
        if($jurisdictionClass=='casino')
        {
            $subjurisdiction='nation,region,district,club';
        }
        elseif($jurisdictionClass=='nation')
        {
            $subjurisdiction='region,district,club';
        }
        elseif($jurisdictionClass=='region')
        {
            $subjurisdiction='district,club';
        }
        elseif($jurisdictionClass=='district'){
            $subjurisdiction='club';
        }
        return $subjurisdiction;
    }


    function containsLetter($string){
        $pattern = '/[A-Za-z]/';
        preg_match($pattern, $string, $matches);
        if (!$matches)
        {
            return false;
        }
        return true;
    }
    function containsNumber($string){
        if( preg_match('/\d/',          $string)) {
            return true;
        }
        return false;
    }

    function doJurisdictionCheck(){
        $jur_id = $_SESSION["jurisdiction_id"];
        $return='';
        if($_SESSION["jurisdiction_class"] != "casino"){
            $return .= " JOIN jurisdiction club ON club.jur_id  = pun_betting_club
  		        JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
                JOIN jurisdiction region ON region.jur_id = district.jur_parent_id
                JOIN jurisdiction nation   ON nation.jur_id   = region.jur_parent_id";
        }
        switch($_SESSION["jurisdiction_class"]){
            case "club":
                $return .= " WHERE club.jur_id = $jur_id";
                break;
            case "district":
                $return .= " WHERE district.jur_id = $jur_id";
                break;
            case "region":
                $return .= " WHERE region.jur_id = $jur_id";
                break;
            case "nation":
                $return .= " WHERE nation.jur_id = $jur_id";
                break;

            case "casino":
                $return .= " WHERE 1=1";
                break;
        }
        return $return;
    }

    function getJurisdictionCurrencies(){
        global $dbh;
        $return='';
        if($_SESSION['jurisdiction_class']=='casino'){
            $sql='select cur_id,ccn_date, cur_name, cur_code, cur_code_for_web, ccn_cur_master, ccn_cur_secondary, ccn_convers_value
              from currency, currencies_conversion,
              ( select max(ccn_date) max_date from currencies_conversion) t
              where cur_code_for_web is not null
              and cur_code = ccn_cur_secondary
              and ccn_date = max_date ';
            $return=$dbh->doCachedQuery($sql,3600*5);
            if($return->getNumRows()<1){
                error_log("Currency extraction error.Query results : ".$return->getNumRows());
                $return=$dbh->doCachedQuery($sql,0);
            }
        }
        elseif(strtolower($_SESSION['currency_id'])!='1'){
            $sql='select cur_id,ccn_date, cur_name, cur_code, cur_code_for_web, ccn_cur_master, ccn_cur_secondary, ccn_convers_value
              from currency, currencies_conversion,
              ( select max(ccn_date) max_date from currencies_conversion) t
              where cur_code_for_web is not null
              and cur_code = ccn_cur_secondary
              and ccn_date = max_date
              and cur_name='.$dbh->prepareString($_SESSION['currency']);
            $return=$dbh->doCachedQuery($sql,3600*5);
            if($return->getNumRows()<1){
                error_log("Currency extraction error.Query results : ".$return->getNumRows());
                $return=$dbh->doCachedQuery($sql,3600*5);
            }
        }
        return $return;

    }

    function getAllCurrencies(){
        global $dbh;
        return $dbh->doCachedQuery('Select * from currency where cur_code_for_web is not null',3600);
    }

    function getAllCurrency($currencys) {
        // !start query removed
        /* $str  = '<select name="currency">';
        $str .= '<option value="1">Euro</option>';
        $str .= '</select>';
        return $str; */
        // !end query removed

        global $dbh;
        $sql = "SELECT * FROM currency where cur_code_for_web is not null ";
        $rs = $dbh->exec($sql);
        $selected = '';
        $str  = '<select name="currency">';
        while ($rs->hasNext()) {
            $row = $rs->next();
            $selected = '';
            if($row['cur_id']==$currencys){
                $selected = 'selected';
            }
            $str .= '<option value="' . $row['cur_id'] . '"' . $selected . '>' . $row['cur_name'] . '</option>';
        }
        $str .= '</select>';
        return $str;
    }

    function updateAllHierarchy($jur,$jurClass,$field,$punterfield=false,$punterFieldValue=false){
        global $dbh;
        $jurisdiction = JurisdictionsList::getInstance ( $dbh );
        $allChildren=implode(",",$jurisdiction->getChilJurisdictionsIdList($jur,( $punterfield ? 'club' : null )));
        if ($allChildren == '') {
            return true;
        }
        $sql= " update ";
        if($punterfield){
            $sql.=" punter ";
            $sql.=" SET $punterfield =".(is_numeric($punterFieldValue)? $dbh->escape($punterFieldValue) : $dbh->prepareString($punterFieldValue));
            $sql.=" WHERE pun_betting_club in (".$allChildren.")";
        }
        else{
            $sql.=" jurisdiction SET ";
            foreach($field as $name=>$value){
                $sql.= " $name = ".(is_int($value)? $dbh->escape($value) : $dbh->prepareString($value)).",";
            }
            $sql=rtrim($sql,',');
            $sql.=" WHERE jur_id in (".$allChildren.")";
        }
        return $dbh->exec($sql);
    }

    function updateAllProcSubjur($jur,$jurClass,$processors){
        global $dbh;
        $jurisdiction = JurisdictionsList::getInstance ( $dbh );
        $allChildren=implode(",",$jurisdiction->getChilJurisdictionsIdList($jur, null));
        if ($allChildren == '') {
            return true;
        }
        $sql= " update ";

            $sql.=" jurisdiction SET ";
            $sql.= " jur_processor_enabled = ".$dbh->prepareString($processors) ;

            $sql.=" WHERE jur_id in (".$allChildren.")";


        return $dbh->exec($sql);
    }



    function getAllClubsAndDistricts(){
        global $dbh;
        $today=date('Y-m-d');
        $sql=" SELECT d.jur_id dis_jur_id,
           d.jur_name dis_jur_name, c.jur_id cl_jur_id, c.jur_name cl_jur_name, c.jur_status cl_jur_status
           FROM jurisdiction ca
           JOIN jurisdiction n ON n.jur_parent_id = ca.jur_id
           JOIN jurisdiction r ON r.jur_parent_id = n.jur_id
           JOIN jurisdiction d ON d.jur_parent_id = r.jur_id
           JOIN jurisdiction c ON c.jur_parent_id = d.jur_id
           ";
        if($_SESSION['jurisdiction_class']=='nation'){
            $sql .=" WHERE n.jur_id=". $_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class']=='region'){
            $sql.=" WHERE r.jur_id=". $_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class']=='district'){
            $sql.=" WHERE d.jur_id=". $_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class']=='club'){
            $sql.="  WHERE c.jur_id=". $_SESSION['jurisdiction_id'];
        }

        $sql.=" GROUP BY d.jur_id, c.jur_id
           ORDER BY d.jur_id, c.jur_id ";
        return $dbh->exec($sql);
    }


    /**
     * @param $country
     * @return array
     */
    function formatCountry($country){
        $formattedCountryArr=array();
        foreach($country as $k=>$v){
            $formattedCountryArr[$v['cou_code']]=$v['cou_name'];
        }
        return $formattedCountryArr;
    }

    function checkAffiliateIsUnderJurisdiction($affID,$jurID){
        global $dbh;
        $result=$dbh->queryOne('Select jur_id from affiliate join admin_user on aff_aus_id=aus_id join jurisdiction on aus_jur_id=jur_id  where aff_id='.$dbh->escape($affID));
        return ($result==$jurID);

    }


    function getPartnersByJurisdiction(){
        global $dbh;
        $jur_id = $_SESSION["jurisdiction_id"];
        $sql=" Select * from partners";
        if($_SESSION["jurisdiction_class"] != "casino"){
            $sql .= "   join jurisdiction club on club.jur_id=ptn_jur_id
  		        JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
                JOIN jurisdiction region ON region.jur_id = district.jur_parent_id
                JOIN jurisdiction nation   ON nation.jur_id   = region.jur_parent_id";
        }
        switch($_SESSION["jurisdiction_class"]){
            case "club":
                $sql .= " WHERE club.jur_id = $jur_id";
                break;
            case "district":
                $sql .= " WHERE district.jur_id = $jur_id";
                break;
            case "region":
                $sql .= " WHERE region.jur_id = $jur_id";
                break;
            case "nation":
                $sql .= " WHERE nation.jur_id = $jur_id";
                break;

            case "casino":
                $sql .= " WHERE 1=1";
                break;
        }
        return $dbh->exec($sql);
    }

    function getNumberOfNewPlayers($start, $until)
    {
        global $dbh;
        $sql = "SELECT count(*) as players from punter";
        $sql .= doJurisdictionCheck();
        $sql .= " AND pun_reg_date BETWEEN " . $dbh->prepareString($start) . " AND " . $dbh->prepareString($until);
        return $dbh->queryOne($sql);
    }

    function getSubjurisdictionClass($jurisdictionClass)
    {

        $subjurisdiction='';

        if($jurisdictionClass=='casino')
        {
            $subjurisdiction='nation';
        }
        elseif($jurisdictionClass=='nation')
        {
            $subjurisdiction='region';
        }
        elseif($jurisdictionClass=='region')
        {
            $subjurisdiction='district';
        }
        elseif($jurisdictionClass=='district'){
            $subjurisdiction='club';
        }
        elseif($jurisdictionClass=='club'){
            $subjurisdiction='club';
        }
        return $subjurisdiction;
    }


    /**
     * Return the name of the jurisdiciton type
     *
     * @param $jurisdictionType the unique id of the jurisdiction.
     * Can have one of the following value:
     * <ul>
     *      <li>1 - Owner</li>
     *      <li>2 - Seller</li>
     *      <li>3 - Re-seller</li>
     *      <li>4 - Agent</li>
     *      <li>5 - Star</li>
     * </ul>
     * @return string the name of the jurisdiction type. can have of the following value:
     * <ul>
     *      <li>Owner</li>
     *      <li>Seller</li>
     *      <li>Re-seller</li>
     *      <li>Agent</li>
     *      <li>Star</li>
     * </ul>
     * @throws ServiceException exception returned in case the jurisdictionType is not valid
     */
    function getJurisdictionClassName($jurisdictionType)
    {
        $msg = "[general_function.inc]";
        $jurTypeName='';
        switch($jurisdictionType) {
            case 1: $jurTypeName = "owner"; break;
            case 2: $jurTypeName = "seller"; break;
            case 3: $jurTypeName = "re-seller"; break;
            case 4: $jurTypeName = "agent"; break;
            case 5: $jurTypeName = "star"; break;
            default: throw new ServiceException(SystemErrorMapper::$ERROR_INVALID_STRUCTURE_TYPE, "");
        } // end switch
        return $jurTypeName;
    } // end method getJurisdictionClassName()


    function buildJurisdictionsTree($childArray,$onlyChild=false,$agencyCode=false,$class=false){
        global $lang;
        foreach($childArray as $key => $value){
            if(!$class) {
                if ($value['class'] == 'casino') {
                    $class_type = "casino";
                } elseif ($value['class'] == 'nation') {
                    $class_type = "nation";
                } elseif ($value['class'] == 'region') {
                    $class_type = "region";
                } elseif ($value['class'] == 'district') {
                    $class_type = "district";
                } elseif ($value['class'] == 'club') {
                    $class_type = "club";
                }
            }
            $blocked='';
            if($value['jur_status']!='1' && $value['class']!='casino'){
                $blocked=' <span class="tip" style="color:red">('.$lang->getLang("blocked").')</span>';
            }
            if(!is_null($value['child'])){
                if(!$onlyChild ||($onlyChild && strpos(getAllSubJurisdictions($_SESSION['jurisdiction_class']), strtolower($class_type))!==false )){ ?>
                    <li data-jstree='{ "type" : "<?=$class_type?>" }' class="jstree-closed <?=$class_type?>" id="<?=$value['id']?>">
                    <a href='/jurisdictions/index.php?node=<?=$value['id']?>'><?=strtoupper($value['name'])?><span class='tip'>(<?=(count($value['child'])." ".getSubjurisdictionClass($value['class']).(count($value['child'])>1?'s':''))?>)</span><?=$blocked?></a><ul><?php
                }
                buildJurisdictionsTree($value['child'],$onlyChild,$agencyCode);

                if(!$onlyChild ||($onlyChild && strpos(getAllSubJurisdictions($_SESSION['jurisdiction_class']), strtolower($class_type))!==false )){ ?>
                    </ul></li><?php }
            }else{
                if(!$onlyChild ||($onlyChild && strpos(getAllSubJurisdictions($_SESSION['jurisdiction_class']), strtolower($class_type))!==false ) || $class_type=='club'){ ?>
                    <li data-jstree='{ "type" : "<?=$class_type?>" }' class="<?=$class_type?>" id="<?=$value['id']?>"><?=($agencyCode? "[".$value['code']."] " :'' )?><a href='/jurisdictions/index.php?node=<?=$value['id']?>'><?=strtoupper($value['name'])?><?=$blocked?></a></li><?php }
            }
        }
    }

    function getJurisdictionPathByIdAndClass($id,$class='nation'){
        global $dbh;
        $select=" n.jur_id nation_id ";
        $from=" jurisdiction n ";
        $where= "   n.jur_parent_id=1
        AND n.jur_id =".$dbh->escape($id);
        if($class=='region'){
            $select=" n.jur_id nation_id,r.jur_id region_id ";
            $from=" jurisdiction n,jurisdiction r";
            $where= "   r.jur_parent_id=n.jur_id
                   AND n.jur_parent_id=1
                   AND r.jur_id =".$dbh->escape($id);
        }
        if($class=='district'){
            $select=" n.jur_id nation_id,r.jur_id region_id,d.jur_id district_id ";
            $from=" jurisdiction n, jurisdiction r,jurisdiction d ";
            $where= "      d.jur_parent_id=r.jur_id
                   AND r.jur_parent_id=n.jur_id
                   AND n.jur_parent_id=1
                   AND d.jur_id =".$dbh->escape($id);
        }
        if($class=='club'){
            $select="n.jur_id nation_id,r.jur_id region_id,d.jur_id district_id,c.jur_id club_id";
            $from=" jurisdiction n, jurisdiction r, jurisdiction d,jurisdiction c ";
            $where= "  c.jur_parent_id=d.jur_id
                    AND d.jur_parent_id=r.jur_id
                    AND r.jur_parent_id=n.jur_id
                    AND n.jur_parent_id=1
                    AND c.jur_id =".$dbh->escape($id);
        }

        $sql="SELECT $select FROM $from WHERE $where";

        $jurisdicitons=  $dbh->doCachedQuery($sql, 0);
        $return=array();
        while ($jurisdicitons->hasNext()) {
            $row = $jurisdicitons->next();
            if ($row['club_id']== $id) {
                array_push($return, "n" . $row['club_id']);
                array_push($return, "n" . $row['district_id']);
                array_push($return, "n" . $row['region_id']);
                array_push($return, "n" . $row['nation_id']);
            }
            elseif($row['district_id']== $id){
                array_push($return, "n" . $row['district_id']);
                array_push($return, "n" . $row['region_id']);
                array_push($return, "n" . $row['nation_id']);
            }
            elseif($row['region_id']== $id) {
                array_push($return, "n" . $row['region_id']);
                array_push($return, "n" . $row['nation_id']);
            }
            elseif($row['nation_id']== $id) {
                array_push($return, "n" . $row['nation_id']);
            }
        }
        return json_encode(array_unique($return));
    }
    
    function getPartners($request){
        global $dbh,$lang;
        $jur_id = $_SESSION["jurisdiction_id"];
        $columns = array('ptn_id','jur_name','jur_available_credit',"ptn_name",'jur_id','jur_status', 'jur_currency','cur_code_for_web','ptn_id','ptn_status' );
        $sTable = " partners ptn
            JOIN jurisdiction club on  ptn_jur_id = club.jur_id
            JOIN currency ON  club.jur_currency=cur_id    " ;
        if($_SESSION["jurisdiction_class"] != "casino"){
            $sTable .= "
  		        JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
                JOIN jurisdiction region ON region.jur_id = district.jur_parent_id
                JOIN jurisdiction nation   ON nation.jur_id   = region.jur_parent_id";
        }

        $limit = '';
        if ( isset($request['start']) && $request['length'] != -1 ) {
            $limit = "LIMIT ".intval($request['start']).", ".intval($request['length']);
        }
        $order = '';
        if ( isset($request['order']) && count($request['order']) ) {
            $orderBy = array();
            for ( $i=0, $ien=count($request['order']) ; $i<$ien ; $i++ ) {
                $columnIdx = intval($request['order'][$i]['column']);
                $requestColumn = $request['columns'][$columnIdx];
                $column = $columns[ $columnIdx ];
                if ( $requestColumn['orderable'] == 'true' ) {
                    $dir = $request['order'][$i]['dir'] === 'asc' ?
                        'ASC' :
                        'DESC';
                    $orderBy[] = ''.$column.' '.$dir;
                }
            }
            $order = 'ORDER BY '.implode(', ', $orderBy);
        }

        switch($_SESSION["jurisdiction_class"]){
            case "club":
                $sWhere = " WHERE club.jur_id = $jur_id";
                break;
            case "district":
                $sWhere = " WHERE district.jur_id = $jur_id";
                break;
            case "region":
                $sWhere = " WHERE region.jur_id = $jur_id";
                break;
            case "nation":
                $sWhere= " WHERE nation.jur_id = $jur_id";
                break;

            case "casino":
                $sWhere= " WHERE 1=1";
                break;
        }
        if($request['status']!=2){
            if($request['status']==0){
                $sWhere.=" And ptn_status=0";
            }
            else{
                $sWhere.=" And ptn_status!=0";
            }
        }

        if($request["maxValue"]){
            $sWhere.=" AND club.jur_available_credit < ".$dbh->escape($request["maxValue"]);
        }

        $str = $request['search']['value'];
        if ( isset($request['search']) && $request['search']['value'] != '' ) {
            $extrasWhere = " AND ";
            $extrasWhere .=" (club.jur_name  LIKE '%".mysql_real_escape_string( $str )."%' OR ptn_name LIKE '%".$dbh->escape($str)."%' )";
        }
        $sQuery = "  SELECT SQL_CALC_FOUND_ROWS ".str_replace(" , ", " ", implode(", ", $columns))."
    FROM   $sTable
    $sWhere
    $extrasWhere
    $order  
    $limit";
        $rResult = $dbh->exec($sQuery)  ;

        $sQuery = " SELECT FOUND_ROWS() ";
        $iFilteredTotal = $dbh->queryOne($sQuery) ;

        $rResultFilterTotal = $dbh->queryOne("SELECT count(*) from $sTable $sWhere");
        $tableArray=array();

        while ($rResult->hasNext()){
            $row=$rResult->next();
            $section=array();
            $i=0;
            $section[$i++]=$row['ptn_id'];
            $section[$i++]=$row['jur_name']."<br />".($row['jur_status']!=1? "<span class='text-error'>".$lang->getLang('Blocked')."</span>" :"<span class='text-success'>".$lang->getLang('Active')."</span>");
            $section[$i++]="<span id='".$row['jur_id']."_credit'>".getInDollars($row["jur_available_credit"],2,$row["cur_code_for_web"])."</span>";
            $section[$i++]="<span class='partnerName'>".$row['ptn_name']."</span><br />".
                ($row['ptn_status']==0? "<span class='text-error partnerStatus'>".$lang->getLang('Disabled')."</span>" :
                    ($row['ptn_status']==2 ? "<span class='text-success partnerStatus'>".$lang->getLang('Enabled -Force rollback ')."</span>" :
                        "<span class='text-success partnerStatus'>".$lang->getLang('Enabled')."</span>"));
            $section[$i++]="<button class='btn btn-small showPartnerDetails' data-id='".$row['ptn_id']."'>".$lang->getLang("Show details")."</button>";
            $section[$i]="";
            if(check_access('manage_partners_modification')) {
                //dropdown menu
                $section[$i].='<div class="btn-group">
                    <button class="btn btn-small dropdown-toggle" data-toggle="dropdown">'.$lang->getLang("Action").'<span class="caret"></span></button>
                    <ul class="dropdown-menu" style="min-width:80px "  data-id="'.$row['ptn_id'].'" data-status="'.$row['ptn_status'].'" data-name="'.$row['ptn_name'].'" data-club="'.$row['jur_id'].'">
                        <li class="modifyPartner"><a href="javascript:void(0)" ><i class="icon-pencil"> </i> '.$lang->getLang("Modify").'</a></li>
                        <li class="blockPartner" ><a href="javascript:void(0)" >'.($row['ptn_status']==1? '<i class="icon-ban-circle"></i>'. $lang->getLang("Disable") : '<i class="icon-ok"></i>'. $lang->getLang("Enable")).'</a></li>
                        <li class="depositWithdraw" data-type="deposit" ><a href="#depositWithdrawDiv"  data-toggle="modal" ><i class="icon-hand-left"> </i> '.$lang->getLang("Deposit").'</a></li>
                        <li class="depositWithdraw" data-type="withdraw"><a href="#depositWithdrawDiv"  data-toggle="modal" ><i class="icon-hand-right"> </i> '. $lang->getLang("Withdraw").'</a></li>
                        <li class="clonePartner" data-id="'.$row['ptn_id'].'"><a href="#myModal"  data-toggle="modal" ><i class="icon-file"> </i> '. $lang->getLang("Clone").'</a></li>
                    </ul>
                </div>';
            } else {
                $section[$i]=$lang->getLang("No action available");
            }
            array_push($tableArray,$section);
        }

        $output= array(
            "draw"            => intval( $request['draw'] ),
            "recordsTotal"    => intval( $rResultFilterTotal ),
            "recordsFiltered" => intval( $iFilteredTotal ),
            "data"            => $tableArray
        );
        echo json_encode( $output);

    }

    function deleteGameLimit($request) {
        global $dbh,$lang;
        $jur_id = $_POST["jur_id"];
        $jur_class = $_POST["jur_type"];



            $username = $request['username'];

            $jurisdiction = '';
            if ($jur_class == 'casino') {
                $jurisdiction = 'n.jur_id';
            } else if ($jur_class == 'nation') {
                $jurisdiction = 'n.jur_id';
            } else if ($jur_class == 'region') {
                $jurisdiction = 'r.jur_id';
            } else if ($jur_class == 'district') {
                $jurisdiction = 'd.jur_id';
            } else if ($jur_class == 'club') {
                $jurisdiction = 'c.jur_id';
            }

        $sTable = " egamessystem.jurisdiction n
            JOIN egamessystem.jurisdiction r ON n.jur_id = r.jur_parent_id
            JOIN egamessystem.jurisdiction d ON r.jur_id = d.jur_parent_id
            JOIN egamessystem.jurisdiction c ON d.jur_id = c.jur_parent_id
            JOIN egamessystem.punter ON pun_betting_club = c.jur_id
            JOIN egamessystem.punter_games_limit gl ON pun_id = pgl_pun_id
            LEFT JOIN egamessystem.providers ON pes_game_code = pgl_pes_game_code ";
            $sWhere = " WHERE $jurisdiction = $jur_id";

            $sQuery = "DELETE gl.*
                from 
                $sTable
                $sWhere";


        $rResult = $dbh->exec($sQuery);

        return '2';
    }

    function getGamesLimit($request){
        global $dbh,$lang;
        $jur_id = $_SESSION["jurisdiction_id"];
        $jur_class = $_SESSION['jurisdiction_class'];

        $sWhere = '';

        if($request['searchBy'] == 1) {
            $username = $request['username'];

            $jurisdiction = '';
            if ($jur_class == 'casino') {
                $jurisdiction = 'n.jur_parent_id';
            } else if ($jur_class == 'nation') {
                $jurisdiction = 'n.jur_parent_id';
            } else if ($jur_class == 'region') {
                $jurisdiction = 'r.jur_parent_id';
            } else if ($jur_class == 'district') {
                $jurisdiction = 'd.jur_parent_id';
            } else if ($jur_class == 'club') {
                $jurisdiction = 'c.jur_parent_id';
            }

            $sTable = " egamessystem.punter_games_limit pgl
                    JOIN egamessystem.jurisdiction c ON c.jur_id = pgl_jur_id
                    JOIN egamessystem.jurisdiction d ON c.jur_parent_id = d.jur_id
                    JOIN egamessystem.jurisdiction r ON d.jur_parent_id = r.jur_id
                    JOIN egamessystem.jurisdiction n ON r.jur_parent_id = n.jur_id
                    JOIN egamessystem.punter ON pun_betting_club = c.jur_id AND pun_id = pgl_pun_id
                    LEFT JOIN egamessystem.providers ON pes_game_code = pgl_pes_game_code";
            $sWhere = " WHERE pun_id = $username ";
            $sQuery = "SELECT 6 jur_type, n.jur_id as nation_id, n.jur_name as nation, r.jur_id as region_id, r.jur_name as region, d.jur_id as district_id, d.jur_name as district, c.jur_id as club_id, c.jur_name as club, pgl.*, COALESCE(pes_name, 'ALL') provider_name, pun_username
        from 
        $sTable
        $sWhere GROUP BY pgl_jur_id, pgl_pun_id ";

        }

        if($request['searchBy'] == 2) {

            $jur_id = $request["jur_select"];
            $jur_class = $request['jur_level'];

            $jurisdiction = '';
            if ($jur_class == 'casino') {
                $jurisdiction = 'n.jur_id';
            } else if ($jur_class == 'nation') {
                $jurisdiction = 'n.jur_id';
            } else if ($jur_class == 'region') {
                $jurisdiction = 'r.jur_id';
            } else if ($jur_class == 'district') {
                $jurisdiction = 'd.jur_id';
            } else if ($jur_class == 'club') {
                $jurisdiction = 'c.jur_id';
            }

            $sTable = " egamessystem.punter_games_limit pgl
                JOIN egamessystem.jurisdiction c ON c.jur_id = pgl_jur_id
                JOIN egamessystem.jurisdiction d ON c.jur_parent_id = d.jur_id
                JOIN egamessystem.jurisdiction r ON d.jur_parent_id = r.jur_id
                JOIN egamessystem.jurisdiction n ON r.jur_parent_id = n.jur_id
                JOIN egamessystem.punter ON pun_betting_club = c.jur_id
                LEFT JOIN egamessystem.providers ON pes_game_code = pgl_pes_game_code ";
            $sWhere = " WHERE $jurisdiction = $jur_id";
            $sQuery = "SELECT 6 jur_type, n.jur_id as nation_id, n.jur_name as nation, r.jur_id as region_id, r.jur_name as region, d.jur_id as district_id, d.jur_name as district, c.jur_id as club_id, c.jur_name as club, pgl.*, COALESCE(pes_name, 'ALL') provider_name, coalesce(pun_username, '-') as pun_username
            FROM 
            $sTable
            $sWhere  AND pgl_pun_id = pun_id GROUP BY pgl_jur_id, pgl_pun_id, pgl_pes_game_code ";
        }

        // query Club
        $unionall = " UNION ALL
                    SELECT 5 jur_type, n.jur_id as nation_id, n.jur_name as nation, r.jur_id as region_id, r.jur_name as region, d.jur_id as district_id, d.jur_name as district, c.jur_id as club_id, c.jur_name as club, pgl.*, COALESCE(pes_name, 'ALL') provider_name, '-' as pun_username
                    FROM egamessystem.punter_games_limit pgl
                    JOIN egamessystem.jurisdiction c ON pgl.pgl_jur_id = c.jur_id AND pgl_pun_id = 0
                    JOIN egamessystem.jurisdiction d ON c.jur_parent_id = d.jur_id
                    JOIN egamessystem.jurisdiction r ON d.jur_parent_id = r.jur_id
                    JOIN egamessystem.jurisdiction n ON r.jur_parent_id = n.jur_id ".
                    ($request['searchBy'] == 1?'JOIN egamessystem.punter ON pun_betting_club = c.jur_id':'').
                    " LEFT JOIN egamessystem.providers ON pes_game_code = pgl_pes_game_code
                    $sWhere GROUP BY pgl_jur_id, pgl_pun_id, pgl_pes_game_code ";

        // query District
        $unionall .= " UNION ALL
                    SELECT 4 jur_type, n.jur_id as nation_id, n.jur_name as nation, r.jur_id as region_id, r.jur_name as region, d.jur_id as district_id, d.jur_name as district, -1 as club_id, '-' as club, pgl.*, COALESCE(pes_name, 'ALL') provider_name, '-' as pun_username
                    FROM egamessystem.jurisdiction c 
                    JOIN egamessystem.jurisdiction d ON c.jur_parent_id = d.jur_id
                    JOIN egamessystem.jurisdiction r ON d.jur_parent_id = r.jur_id
                    JOIN egamessystem.jurisdiction n ON r.jur_parent_id = n.jur_id
                    JOIN egamessystem.punter_games_limit pgl ON pgl_jur_id = d.jur_id AND pgl_pun_id = 0 ".
                    ($request['searchBy'] == 1?'JOIN egamessystem.punter ON pun_betting_club = c.jur_id':'').
                    " LEFT JOIN egamessystem.providers ON pes_game_code = pgl_pes_game_code
                    $sWhere GROUP BY pgl_jur_id, pgl_pun_id, pgl_pes_game_code ";

        // query Regional
        $unionall .= " UNION ALL
                    SELECT 3 jur_type, n.jur_id as nation_id, n.jur_name as nation, r.jur_id as region_id, r.jur_name as region, -1 as district_id, '-' as district, -1 as club_id, '-' as club, pgl.*, COALESCE(pes_name, 'ALL') provider_name, '-' as pun_username
                    FROM egamessystem.jurisdiction c 
                    JOIN egamessystem.jurisdiction d ON c.jur_parent_id = d.jur_id
                    JOIN egamessystem.jurisdiction r ON d.jur_parent_id = r.jur_id
                    JOIN egamessystem.jurisdiction n ON r.jur_parent_id = n.jur_id
                    JOIN egamessystem.punter_games_limit pgl ON pgl_jur_id = r.jur_id AND pgl_pun_id = 0 ".
                    ($request['searchBy'] == 1?'JOIN egamessystem.punter ON pun_betting_club = c.jur_id':'').
                    " LEFT JOIN egamessystem.providers ON pes_game_code = pgl_pes_game_code
                    $sWhere GROUP BY pgl_jur_id, pgl_pun_id, pgl_pes_game_code ";

        // query National
        $unionall .= " UNION ALL
                    SELECT 2 jur_type, n.jur_id as nation_id, n.jur_name as nation, -1 as region_id, '-' as region, -1 as district_id, '-' as district, -1 as club_id, '-' as club, pgl.*, COALESCE(pes_name, 'ALL') provider_name, '-' as pun_username
                    FROM egamessystem.jurisdiction c 
                    JOIN egamessystem.jurisdiction d ON c.jur_parent_id = d.jur_id
                    JOIN egamessystem.jurisdiction r ON d.jur_parent_id = r.jur_id
                    JOIN egamessystem.jurisdiction n ON r.jur_parent_id = n.jur_id
                    JOIN egamessystem.punter_games_limit pgl ON pgl_jur_id = n.jur_id AND pgl_pun_id = 0 ".
                    ($request['searchBy'] == 1?'JOIN egamessystem.punter ON pun_betting_club = c.jur_id':'').
                    " LEFT JOIN egamessystem.providers ON pes_game_code = pgl_pes_game_code
                     $sWhere  GROUP BY pgl_jur_id, pgl_pun_id, pgl_pes_game_code  ";


        $sQuery .= $unionall . " ORDER BY jur_type ".($request['searchBy'] == 1?'DESC':'');

//        die($sQuery);
        $rResult = $dbh->exec($sQuery) ;


        $sQuery = " SELECT FOUND_ROWS() ";
        $iFilteredTotal = $dbh->queryOne($sQuery);

        $rResultFilterTotal = $iFilteredTotal;

//        die($iFilteredTotal);
//        exit;

        $tableArray=array();

        while ($rResult->hasNext()){
            $row=$rResult->next();
            $section=array();
            $i=0;
            $section[$i++]="<span data-id='".$row['nation_id']."' class='nation'>".$row['nation']."</span>";
            $section[$i++]="<span data-id='".$row['region_id']."' class='region'>".$row['region']."</span>";
            $section[$i++]="<span data-id='".$row['district_id']."' class='district'>".$row['district']."</span>";
            $section[$i++]="<span data-id='".$row['club_id']."' class='club'>".$row['club']."</span>";
            $section[$i++]=$row['pun_username'];
            $section[$i++]=$row['pgl_creation'].'<br>'.$row['pgl_last_update'];
            $section[$i++]=$row['pgl_deposit_limit'];
            $section[$i++]=$row['pgl_withdraw_limit'];
            $section[$i++]=$row['pgl_bet_limit'];
            $section[$i++]=$row['pgl_win_limit'];
            $section[$i++]=$row['provider_name'];

            $section[$i]="";
            if(check_access('casino_games_limit_modify')) {
                //dropdown menu
                $section[$i] .= '<div class="btn-group">
                    <button class="btn btn-small modifyGameLimit" data-id="' . $row['pgl_id'] . '" ><i class="icon-pencil"> </i>' . $lang->getLang("Modify") . '</button>
                    <button class="btn btn-danger btn-small deleteUserGameLimit" data-id="' . $row['pgl_id'] . '" ><i class="icon-trash"> </i> </button>
                </div>';

            } else {
                $section[$i]=$lang->getLang("No action available");
            }
            array_push($tableArray,$section);
        }

        $output= array(
            "draw"            => intval( $request['draw'] ),
            "recordsTotal"    => intval( $rResultFilterTotal ),
            "recordsFiltered" => intval( $iFilteredTotal ),
            "data"            => $tableArray
        );
        echo json_encode( $output);

    }


    function getPendingTrans($request){
        global $dbh,$lang;
        $jur_id = $_SESSION["jurisdiction_id"];
        $jur_class = $_SESSION['jurisdiction_class'];


            $username = $request['username'];

            $jurisdiction = '';
            if ($jur_class == 'casino') {
                $jurisdiction = 'n.jur_parent_id';
            } else if ($jur_class == 'nation') {
                $jurisdiction = 'n.jur_parent_id';
            } else if ($jur_class == 'region') {
                $jurisdiction = 'r.jur_parent_id';
            } else if ($jur_class == 'district') {
                $jurisdiction = 'd.jur_parent_id';
            } else if ($jur_class == 'club') {
                $jurisdiction = 'c.jur_parent_id';
            }

            $sTable = " FROM egamessystem.transaction_pending p
                        JOIN egamessystem.punter ON pun_id = tpg_pun_id
                        LEFT JOIN egamessystem.admin_user ON aus_id = tpg_aus_id 
                        LEFT JOIN egamessystem.providers ON pes_id = tpg_pes_id ";
            $sWhere = " WHERE tpg_creation BETWEEN '".$request['from']."' AND '".$request['until']."'";
            if($request['status'] != 'all' ) {
                $sWhere .= " AND tpg_status = ".$request['status'];
            }
            if($request['username']) {
                $sWhere .= " AND tpg_pun_id = ".$request['username'];
            }
            if($request['provider']!='ALL') {
                $sWhere .= " AND tpg_pes_id = ".$request['provider'];
            }

            $sQuery = "SELECT p.*, coalesce(aus_username, '-') aus_username , pun_username username, coalesce(pes_name, '-') pes_name
                        $sTable
                        $sWhere";


        $rResult = $dbh->exec($sQuery) ;

        $sQuery = " SELECT FOUND_ROWS() ";
        $iFilteredTotal = $dbh->queryOne($sQuery);

        $rResultFilterTotal = $dbh->queryOne("SELECT count(*) from $sTable $sWhere");
        $tableArray=array();

        while ($rResult->hasNext()){
            $row=$rResult->next();
            $section=array();
            $i=0;
            $section[$i++]=$row['username'];
            $section[$i++]=$row['aus_username'];
            $section[$i++]=$row['pes_name'];
            $section[$i++]=$row['tpg_creation'];
            $section[$i++]=$row['tpg_last_update'];
            $section[$i++]=$row['tpg_ctr_transaction_num'];
            $section[$i++]=$row['tpg_amount'];
            $section[$i++]=$row['tpg_balance_available'];
            $section[$i++]=$row['tpg_type']==1?"<span class='text-success'>".$lang->getLang("Deposit")." / ".$lang->getLang("Win")."</span>":"<span class='text-error'>".$lang->getLang("Withdraw")." / ".$lang->getLang("Bet")."</span>";
            $section[$i++]=$row['tpg_status']?$lang->getLang("Open"):$lang->getLang("Closed");
            $section[$i++]=$row['tpg_note'];
            $section[$i++]= $row['tpg_status']?("<div class='btn-group trans_actions'>
                            <button class='btn btn-success do_trans' data-uid='".$row['tpg_pun_id']."' data-tr='".$row['tpg_ctr_transaction_num']."' data-do='1'><i class='fa fa-check'></i></button>
                            <button class='btn btn-danger do_trans' data-uid='".$row['tpg_pun_id']."' data-tr='".$row['tpg_ctr_transaction_num']."' data-do='2'><i class='fa fa-times'></i></button>
                            </div>"):"";

            array_push($tableArray,$section);
        }

        $output= array(
            "draw"            => intval( $request['draw'] ),
            "recordsTotal"    => intval( $rResultFilterTotal ),
            "recordsFiltered" => intval( $iFilteredTotal ),
            "data"            => $tableArray
        );
        echo json_encode( $output);

    }

    function getAllClubs() {
        global $dbh;
        $sql = "select jur_name,jur_id from jurisdiction where jur_class='club'";

        $result = $dbh->doCachedQuery ( $sql, 0 );
        return $result;
    }

    function getProviders($poker=false)
    {
        global $dbh;
        $sql="Select * from providers";
        if($poker){
            $sql.=" where pes_name like '%poker%'";
        }
        $result= $dbh->doCachedQuery($sql,0);
//        echo "<pre>";
//        print_r($result);
//        die();
        return $result;
    }
    function getProvidersList($search=false)
    {

        global $dbh;
        $sql="Select * from egamessystem.providers";
        if($search){
            $sql.=" where pes_name like '%".$search."%'";
        }
//        die($sql);
        $result= $dbh->doCachedQuery($sql,0);
        return $result;
    }
    function checkGames($games)
    {
        $gid=strpos($games, 'gid:');
        if($gid!==false){
            $games=substr($games, 4);
        }
        return $games;
    }



    function addProvider($props)
    {
        global $dbh;
        if($props['name']!=''){
            $sql="SELECT * FROM providers where pes_name=".$dbh->prepareString($props['name']);
            if($result=$dbh->queryRow($sql))
            {
                return '2';
            }
            $sql="SELECT pes_id from providers where pes_remote_key=".$dbh->prepareString($props['key']);
            if($result=$dbh->queryRow($sql))
            {
                return '4';
            }
            $sql = "SELECT pes_id from providers where pes_id =" .$dbh->prepareString($props['providerNewId']);
            $result=$dbh->queryRow($sql);
            //var_dump($result);
            if( $result != null) {
                return '5';
            }

            if($props['providerNewId'] == "") {
                $sql="INSERT INTO providers 
            (
            pes_name,
            pes_allowip,
            pes_remote_id,
            pes_game_code,
            pes_gam_id,
            pes_remote_key,
            pes_remote_url,
            pes_status,
            pes_transfer_type,
            pes_params,
            pes_master_currency,
            pes_other_currency,
            pes_has_multy_cur_param,
             pes_group)
			          VALUES(
			          ".$dbh->prepareString($props['name']).",
			           ".$dbh->prepareString(implode("|",$props['ips'])).",
			          ".$dbh->prepareString($props['remoteId']).",
			          ".$dbh->prepareString($props['games']).",
			          ".$dbh->prepareString($props['gameId']).",
			          ".$dbh->prepareString($props['key']).",
			          ".$dbh->prepareString($props['websiteURL']).",
			          ".$dbh->prepareString($props['status']).",
			          ".$dbh->prepareString($props['transfer_type']).",
			          ".$dbh->prepareString($props['params']).",
			          ".$dbh->prepareString($props['mcurrency']).",
			          ".$dbh->prepareString(implode(";",$props['ocurrency'])).",
			          ".$dbh->prepareString($props['hmcurrency']).",
			          ".$dbh->prepareString($props['group']).")";
            }
            else {
                $sql="INSERT INTO providers 
            (
            pes_id,
            pes_name,
            pes_allowip,
            pes_remote_id,
            pes_game_code,
            pes_gam_id,
            pes_remote_key,
            pes_remote_url,
            pes_status,
            pes_transfer_type,
            pes_params,
            pes_master_currency,
            pes_other_currency,
            pes_has_multy_cur_param,
             pes_group)
			          VALUES(
			           ".$dbh->prepareString($props['providerNewId']).",
			          ".$dbh->prepareString($props['name']).",
			           ".$dbh->prepareString(implode("|",$props['ips'])).",
			          ".$dbh->prepareString($props['remoteId']).",
			          ".$dbh->prepareString($props['games']).",
			          ".$dbh->prepareString($props['gameId']).",
			          ".$dbh->prepareString($props['key']).",
			          ".$dbh->prepareString($props['websiteURL']).",
			          ".$dbh->prepareString($props['status']).",
			          ".$dbh->prepareString($props['transfer_type']).",
			          ".$dbh->prepareString($props['params']).",
			          ".$dbh->prepareString($props['mcurrency']).",
			          ".$dbh->prepareString(implode(";",$props['ocurrency'])).",
			          ".$dbh->prepareString($props['hmcurrency']).",
			          ".$dbh->prepareString($props['group']).")";
            }

//            print_r($sql);die();
            $result = $dbh->exec($sql);
            if($result)
            {
//                $id = mysql_insert_id();
                return '1';
            }
            else{
                return '3';
            }
        }
        else{
            return '3';
        }
    }
    function updateProvider($props) {
        global $dbh;
        $sql = "SELECT pes_id from providers where pes_id =" .$dbh->prepareString($props['providerNewId']);
        $result = $dbh->queryRow($sql);
        if($result != null && $props['providerNewId'] != $props['providerid']) {
            return '5';
        } else {
            $sql = "Update providers
        SET
        pes_id=".$dbh->prepareString($props['providerNewId']).",
        pes_name=".$dbh->prepareString($props['name']).",
        pes_allowip=".$dbh->prepareString(implode("|",$props['ips'])).",
        pes_remote_id=".$dbh->prepareString($props['remoteId']).",
        pes_game_code=".$dbh->prepareString($props['games']).",
        pes_gam_id=".$dbh->prepareString($props['gameId']).",
        pes_remote_key=".$dbh->prepareString($props['key']).",
        pes_remote_url= ".$dbh->prepareString($props['websiteURL']).",
        pes_status= ".$dbh->prepareString($props['status']).",
        pes_transfer_type= ".$dbh->prepareString($props['transfer_type']).",
        pes_params=  ".$dbh->prepareString($props['params']).",
        pes_master_currency=".$dbh->prepareString($props['mcurrency']).",
        pes_other_currency= ".$dbh->prepareString(implode(";",$props['ocurrency'])).",
        pes_has_multy_cur_param= ".$dbh->prepareString($props['hmcurrency']).",
        pes_group= ".$dbh->prepareString($props['group']) .
                "  WHERE pes_id=".$dbh->escape($_REQUEST['providerid']);
            $result = $dbh->exec ($sql);
            if($result){
                return '1';
            }
        }


    }

    function getJurisdictionSkins($jur_id=false,$class=false){
        global $dbh;
        if(!$jur_id){
            $jur_id = $_SESSION["jurisdiction_id"];
        }

        if(!$class){
            $class=$_SESSION['jurisdiction_class'];
        }
        $sql="select skn_id,  skn_name";
        if($class!='casino'){
            $sql.=" from
                        (
                       select n2.jur_id nation_jur_id
                        " ;



            switch($class){
                case "club":
                    $sql .= "
                        from jurisdiction c2, jurisdiction d2, jurisdiction r2, jurisdiction n2
                        where c2.jur_id = $jur_id
                        and c2.jur_parent_id = d2.jur_id
                        and d2.jur_parent_id = r2.jur_id
                        and r2.jur_parent_id = n2.jur_id
                        and n2.jur_parent_id = 1 ";
                    break;
                case "district":
                    $sql .= "
                        from  jurisdiction d2, jurisdiction r2, jurisdiction n2
                        where d2.jur_id = $jur_id
                        and d2.jur_parent_id = r2.jur_id
                        and r2.jur_parent_id = n2.jur_id
                        and n2.jur_parent_id = 1 ";
                    break;
                case "region":
                    $sql .= "
                        from  jurisdiction r2, jurisdiction n2
                        where r2.jur_id = $jur_id
                        and r2.jur_parent_id = n2.jur_id
                        and n2.jur_parent_id = 1 ";
                    break;
                case "nation":
                    $sql .= "
                        from   jurisdiction n2
                        where n2.jur_id = $jur_id
                        and n2.jur_parent_id = 1 ";
                    break;
            }
            $sql.=") t, skins, jurisdiction c, jurisdiction d, jurisdiction r, jurisdiction n
                        where c.jur_id = skn_jur_id
                        and c.jur_parent_id = d.jur_id
                        and d.jur_parent_id = r.jur_id
                        and r.jur_parent_id = n.jur_id
                        and n.jur_id = nation_jur_id ";
        }

        else{
            $sql .= "
                        from   skins ";
        }
        return $dbh->exec($sql);
    }
    
    
    function createSkinSelect($availableSkins,$defaultSkin=-1,$optionNone=false,$optionAll=false) {
        global $dbh;

        $selected = '';
        $str  = '<select name="skin">';
        if($optionNone){
            $str.= '<option value="'.$optionNone.'"' . $selected . '>Not configured</option>';
        }
        if($optionAll){
            $str.= '<option value="">All</option>';
        }
        while ($availableSkins->hasNext()) {
            $row = $availableSkins->next();
            $selected = '';
            if($row['skn_id']==$defaultSkin){
                $selected = 'selected';
            }


            $str .= '<option value="' . $row['skn_id'] . '"' . $selected . '>' . $row['skn_name'] . '</option>';
        }
        $str .= '</select>';
        return $str;
    }
    
    
    
    function getProvidersListEnabledForSkin($jur_id){
        global $dbh;
        $sql = "SELECT pes_id, pes_name, pes_game_code , if (COALESCE(scn_jur_id, 0) > 0, 1, 0) is_enabled
                    FROM egamessystem.jurisdiction
                    JOIN egamessystem.skins ON jur_id = " . $jur_id . " AND skn_id = jur_skn_id
                    JOIN egamessystem.providers ON skn_pes_code_enabled like concat('%', pes_game_code, '%') AND (jur_currency = pes_master_currency OR pes_other_currency like concat('%', jur_currency, '%')) 
                    LEFT JOIN egamessystem.jurisdiction_configuration ON jur_id = scn_jur_id AND scn_games_info like concat('%', pes_game_code, '%')";

//        die($sql);
        $result =  $dbh->exec($sql);

        $str = '';
        while ($result->hasNext()) {
            $row = $result->next();

            $str .= '<label class="nice_checkbox">'.$row['pes_name'].'
                        <input type="checkbox" class="check_provider" data-id="'.$row['pes_id'].'" data-code="'.$row['pes_game_code'].'" '.($row['is_enabled']==1?'checked':'').'>
                        <span class="checkmark"></span>
                    </label>';
        }

        return $str;
    }
    
    
    
    function createProvidersSelect($availableProvider,$defaultProvider=-1,$optionNone=false,$optionAll=false) {
        
        $selected = '';
        $str  = '<select name="providers">';
        if($optionNone){
            $str.= '<option value="'.$optionNone.'"' . $selected . '>Not configured</option>';
        }
        if($optionAll){
            $str.= '<option value="">All</option>';
        }
        while ($availableProvider->hasNext()) {
            $row = $availableProvider->next();
            $selected = '';
            if($row['pes_id']==$defaultProvider){
                $selected = 'selected';
            }
            
            
            $str .= '<option value="' . $row['pes_game_code'] . '"' . $selected . '>' . $row['pes_name'] . '</option>';
        }
        $str .= '</select>';
        return $str;
    }


    function getAvailableCountries(){
        global $dbh,$skinsettings;
        $sql="SELECT COU_CODE,COU_NAME FROM country";
        $result = $dbh->doCachedQuery($sql, 3600*24*30);

        return $result;
    }

    function getAllPaymentProcessors(){
        global $dbh,$skinsettings;
        $sql="SELECT * FROM processor_payment";
        $result = $dbh->doCachedQuery($sql, 3600*24*30);
        return $result;
    }
    /**
     * @param $jur_id
     * @return bool|int|RecordSet|string
     */
    function getSkinByJurisdiction($jur_id,$full=false){
        global $dbh;
        if($full){
            $sql="Select * from skins,jurisdiction where jur_skn_id=skn_id AND jur_id=".$jur_id;
            $skin=$dbh->queryRow($sql);
        }
        else{
            $sql="Select skn_name from skins,jurisdiction  where jur_skn_id=skn_id AND jur_id=".$jur_id;
            $skin=$dbh->queryOne($sql);
        }

        if($skin==''){
            $skin="None";
        }
        return $skin;
    }
    ?>


    
