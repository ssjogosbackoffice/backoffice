<?php

if ( ! class_exists('Customer')) {

include_once("financial_functions.inc");

define('GAMESERVER_SERVICE_ADDR', 'localhost');
define('GAMESERVER_SERVICE_PORT', 3032);

class Customer extends Properties {
  protected $db;
  protected function DBInstance() {
    if (!$this->db || !is_a($this->db, "DatabaseHandler") || !$this->db->isConnected()) {
      $this->db = new DatabaseHandler();
      $this->db->connect();
    }
    return $this->db;
  }
  function getBalance() {
    $db = $this->DBInstance();
    $customer_id = $this->get("id");
    $rs = $db->exec("select get_cust_balance($customer_id) as balance");
    $num_rows = $rs->getNumRows();

    if ($num_rows != 1 )
       die("Could not get punter's balance - punter credit record does not exist");

    $rec = $rs->next();
    return $rec['balance'];
  }
  
  function getAvailableBalance() {
    $db = $this->DBInstance();
    $customer_id = $this->get("id");
    $rs = $db->exec("select get_cust_avail_balance($customer_id) as balance");
    $num_rows = $rs->getNumRows();

    if ($num_rows != 1 )
      die("Could not get punter's balance - punter credit record does not exist");

    $rec = $rs->next();
    return $rec['balance'];
  }

  function getLastDepositTime ()
  {
    $db = $this->DBInstance();
    $customer_id = $this->get("id");
    $time = NULL;
    $sql  = "select ctr_time from customer_transaction";
    $sql .= " where ctr_amount > 0";
    $sql .= " and ctr_tty_id = ".T_TYPE_DEPOSIT;
    $sql .= " and ctr_pun_id = $customer_id";
    $sql .= " and ctr_status = 'completed'";
    $sql .= " order by ctr_time desc limit 1";

    $rs = $db->exec($sql);
    $num_rows = $rs->getNumRows();

    if ( $num_rows > 0 )
    {
      $row = $rs->next();
      $time = $row['ctr_time'];
    }
    return $time;
  }

  function getLastDepositType ()
  {
    $db = $this->DBInstance();
    $customer_id = $this->get("id");
    $type = NULL;
    $sql  = "select ctr_pme_code from customer_transaction";
    $sql .= " where ctr_amount > 0";
    $sql .= " and ctr_tty_id = ".T_TYPE_DEPOSIT;
    $sql .= " and ctr_pun_id = $customer_id";
    $sql .= " and ctr_status = 'completed'";
    $sql .= " order by ctr_time desc limit 1";
    
    $rs = $db->exec($sql);
    $num_rows = $rs->getNumRows();

    if ( $num_rows > 0 )
    {
      $row = $rs->next();
      $type = $row['ctr_pme_code'];
    }
    return $type;
  }
  
  function getLastDeposit() {
    $db = $this->DBInstance();
    $customer_id = $this->get("id");
    $deposit = array();
    $sql  = "select * from customer_transaction";
    $sql .= " where ctr_amount > 0";
    $sql .= " and ctr_tty_id = ".T_TYPE_DEPOSIT;
    $sql .= " and ctr_pun_id = $customer_id";
    $sql .= " and ctr_status = 'completed'";
    $sql .= " order by ctr_time desc limit 1";
    $rs = $db->exec($sql);
    $num_rows = $rs->getNumRows();

    if ( $num_rows > 0 )
    {
      $row = $rs->next();
      foreach ( $row as $colname => $val )
        $deposit[removeTablePrefix($colname)] = $val;
    }
    return $deposit;
  }
  
  function numGames($date) {
    $db = $this->DBInstance();
    $customer_id = $this->get("id");
    $sql  = "select count(ctr_amount) from customer_transaction where ";
    $sql .= "ctr_pun_id=$customer_id and ctr_time > '$date'";
    return $db->countQuery($sql);
  }
  
  function isPlaying($sessions = "") {
    $customer_id = $this->get("id");
    if ( !is_array($sessions) )
      $sessions = Punter::getPunterSessions();

    if ( ! $sessions )  //gameserver is not running
      return false;

    foreach ( $sessions as $game_id =>  $punter_sessions_arr )
      if ( $punter_sessions_arr[$customer_id]  )
        return true;
    return false;
  }

  function isFinancial() {
    return ($this->get("member_type") == 'FINANCIAL');
  }
  
  function isNonFinancial() {
    return ($this->get("member_type") == 'NONFINANCIAL');
  }
  
  function isTransient() {
    return ($this->get("identified") == -1);
  }
  
  function setFinancial() {
    global $cookie;
    if ($this->isNonFinancial()) {
      // reset all currency fields
      $db = $this->DBInstance();
      $db->begin();
      $customer_id = $this->get("id");
      $sql  = "update punter_credit" .
              " set pcr_credits=0, pcr_total_bets=0, pcr_total_wins=0, pcr_reserved_funds=0" .
              ", pcr_bonus_credits=0, pcr_total_deposits=0,pcr_total_withdrawals=0" .
              " where pcr_pun_id = $customer_id";
      $rs = $db->exec($sql);
      if ( 1 == $db->getAffectedRows() ) { //reset successful
        $sql = "update punter set pun_member_type = 'FINANCIAL'" .
               " where pun_id = $customer_id";
        $db->exec($sql);  
        if ( 1 == $db->getAffectedRows() ) { //to financial membership successful
          // reset member type cookie
          $cookie->setFieldValue('member_type', 'FINANCIAL');
          $cookie->set();   
        } else {
          $db->rollback();
          return false;
        }
      } else {
        $db->rollback();
        return false;
      }
      $db->commit();
    }
    $this->set("member_type", 'FINANCIAL');
    return true;
  }
  
  function doDeposit($amount)
  {
    $db = $this->DBInstance();
    $customer_id = $this->get("id");
    
    // to financial membership
    if ($this->isNonFinancial()) {
      if (!$this->setFinancial()) {
        return false;
      }
    }
    // update punter's balance with deposit amount
    $sql  = "update punter_credit" .
            " set pcr_credits=pcr_credits+'$amount'" .
            ", pcr_total_deposits=pcr_total_deposits+'$amount'" .
            " where pcr_pun_id = $customer_id";
    $db->exec($sql);
    return ( 1 == $db->getAffectedRows() );
  }

  function doWithdrawal($amount) {
    $db = $this->DBInstance();
    $customer_id = $this->get("id");
    //subtract amount from punter credit balance
    $sql  = "update punter_credit";
    $sql .= " set pcr_credits = pcr_credits - '$amount'";
    $sql .= ", pcr_total_withdrawals = pcr_total_withdrawals + '$amount'";
    $sql .= " where pcr_pun_id = $customer_id";
    $db->exec($sql);   

    return getCustomerBalance($customer_id);
  }

  function doReservefunds($amount) {
    $db = $this->DBInstance();
    $customer_id = $this->get("id");

    //add amount to punter reserved funds balance
    $sql  = "update punter_credit";
    $sql .= " set pcr_reserved_funds = pcr_reserved_funds + '$amount'";
    $sql .= " where pcr_pun_id = $customer_id";
    $db->exec($sql);
  }
  
  function log($log_type, $log_data='') {
    $db = $this->DBInstance();
    $customer_id = $this->get("id");
    $ip_address = getIPAddress();
    $sql  = "insert into customer_log";
    $sql .= " (clg_id, clg_time, clg_pun_id, clg_clt_code, clg_data, clg_ip_address)";
    $sql .= " values(nextval('seq_clg_id'), CURRENT_TIMESTAMP, $customer_id, '$log_type', '$log_data', '$ip_address')";
    $db->exec($sql);  
  }
}

class Punter {
  static function authenticate($username, $password) {
    $db = new DatabaseHandler();
    $db->connect();
    $password = md5($password);
    //Select punter details record using punter id
    $sql  = "select pun_id, pun_first_name, pun_middle_name, pun_last_name";
    $sql .= ", pun_member_type, pcr_credits, pun_access, pcr_total_bets";
    $sql .= ", pcr_total_wins, pun_email, pun_username, pun_reg_date";
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
    $rs = $db->exec($sql);
    $num_rows = $rs->getNumRows();  


    if ( $num_rows > 0 )
    {  $rec = $rs->next();

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

    }
    $user = new Customer();
    $user->raw($new_row);
    return $user;
  }
  
  static function getInstance($punter_id, $is_customer_num=false, $include_internet_users = false) {
    $db = new DatabaseHandler();
    $db->connect();

    //Select punter details record using punter id
    $sql  = "select pun_id, pun_first_name, pun_middle_name, pun_last_name" .
            ", pun_member_type, pcr_credits, pun_access, pcr_total_bets" .
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
            ", pun_last_request_time, pun_num_failed_logins, pun_betting_club, pun_lang" .
            ',(SELECT jur_name FROM jurisdiction WHERE jur_id = pun_betting_club)  AS "pun_Betting Club"' .
                    ',(SELECT jur_name FROM jurisdiction WHERE jur_id =
                        (SELECT jur_parent_id FROM jurisdiction WHERE jur_id = pun_betting_club)
                      ) AS "pun_District"' .
                    ',(SELECT jur_name FROM jurisdiction WHERE jur_id =
                             (SELECT jur_parent_id FROM jurisdiction WHERE jur_id =
                                (SELECT jur_parent_id FROM jurisdiction WHERE jur_id = pun_betting_club)
                             )
                      ) AS "pun_Region"' .
            " from punter, punter_credit WHERE";
    if ( $is_customer_num )
      $sql .= " pun_customer_number = $punter_id and pun_id = pcr_pun_id";
    else
      $sql .= " pun_id = $punter_id and pun_id = pcr_pun_id";
    if ( ! empty($_SESSION['admin_id']) ) {
      //Filter punters according to their jurisdiction and the admin user's jurisdiction
      if ( 'club' == $_SESSION['jurisdiction_class'] ) { //If a club admin user
        // only show punters in same club as admin user, unless we're also including
        // internet users (nonfinancial only)
        if($include_internet_users)
          $sql .= " and (pun_betting_club = " . $_SESSION['jurisdiction_id'] . " OR (pun_betting_club = (SELECT jur_id FROM jurisdiction WHERE jur_name = 'Internet' AND jur_class = 'club') AND pun_member_type = 'NONFINANCIAL')) ";
        else
          $sql .= " and pun_betting_club = " . $_SESSION['jurisdiction_id'];  
      } elseif ( 'district' == $_SESSION['jurisdiction_class'] ) { //If a district manager admin user
        // only select punters where their betting club (bottom level jurisdiction)
        // is under the admin user's district
        $sql .= ' AND pun_betting_club IN' .
                '       (   SELECT jur_id FROM jurisdiction' .
                '           WHERE jur_parent_id = ' . $_SESSION['jurisdiction_id'] .
                '       )';
      } elseif ( 'region' == $_SESSION['jurisdiction_class'] ) { //If a district manager admin user 
        // only select punters where their betting club (bottom level jurisdiction)
        // is under the admin user's district
        // select only  records within admin user's jurisdiction
        $sql .= ' AND pun_betting_club IN ' .
                '     (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
                '         (  SELECT jur_id FROM jurisdiction ' .
                '             WHERE jur_parent_id = ' .$_SESSION['jurisdiction_id'] .
                '         )' .
                '     ) ';
      } else {
        if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
          dieWithError("Invalid user Jurisdiction Class!");
        }
      }
    }

    $rs = $db->exec($sql);
    $num_rows = $rs->getNumRows();

    if ( $num_rows > 0 )
    {  $rec = $rs->next();

      foreach ( $rec as $colname => $val )
        $new_row[removeTablePrefix($colname)] = $val;

      $new_row['balance'] = $new_row['credits'];
      $new_row['available_balance'] = $new_row['credits'] - $new_row['reserved_funds'];
      $new_row['address1'] = $new_row['address_line1'];
      $new_row['address2'] = $new_row['address_line2'];
      $new_row['country_code'] = $new_row['cou_code'];
      $new_row['country'] = $new_row['cou_code'];
      $new_row['full_name'] = concatNames($new_row['first_name'],$new_row['middle_name'],$new_row['last_name']);

      $timeout_end_mt = dbTsToPhpTs($new_row['last_request_time']) + 3600;
      $new_row['sess_timeout'] = mktime() > $timeout_end_mt;
    }
    $user = new Customer();
    $user->raw($new_row);
    return $user;
  }
  
  static function getPunterSessions() {
    $punter_sessions = array();

    //send request to gameserver to obtain list of punters currently playing
    if ($sock = fsockopen(GAMESERVER_SERVICE_ADDR, GAMESERVER_SERVICE_PORT, $errno, $errstr, 30))   //attempt to open socket to localhost:3032
    {  $request = "clients\n";
       fputs($sock, $request); //send request 

       while ( $line = fgets($sock, 1024) )
       {  $arr = explode(";", $line);  //split comma-delimited string into array elements
          $game_id       = $arr[0];   //first token
          $total_players = $arr[1];   //second token 
          $punters       = $arr[2];  //list of punter session strings "pun_id:session_start_time:member_type"

          //explode into punter session strings such that each sell contains pun_id,session_start,member_type
          $punters_sessions_arr   = explode(',', $arr[2]);  
          array_pop($punters_sessions_arr); //since comma at the end of punter list, remove last cell

          //Iterate through array of punter session strings
          foreach ( $punters_sessions_arr as $key => $val )
          {
            $punter_session = explode(':', $val);
            $id = $punter_session[0];
            $session_start = $punter_session[1];
            $member_type = $punter_session[2];

            //stor customer sessiosn for each game
            $punter_sessions[$game_id][$id] = array('session_start'=>$session_start, 'member_type'=>$member_type);
          }
       }
       fclose($sock);
    } else {
      return;
    }
    return $punter_sessions;
  }
}

}

?>
