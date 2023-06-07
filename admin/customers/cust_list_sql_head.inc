<?php
$sql  = "select pun_id, pun_first_name, pun_middle_name, pun_last_name" .
        ", pun_member_type, pcr_credits, pun_access, pcr_total_bets" .
        ", pcr_total_wins, pun_email, pun_username, pun_reg_date" .
        ", pun_customer_number, pun_identified" .
        ", pun_notes, cou_name, pun_investigate" .
        ", pun_dob, pun_address_line1, pun_address_line2, pun_city_suburb" .
        ", pun_state_province, pun_postcode_zip, pun_phone_business" .
        ", pun_phone_home, pun_phone_mobile, pun_fax, pun_daily_allowance" .
        ", pun_gender, pun_temp_password, pun_num_logins, pun_last_logged_in" .
        ", pcr_reserved_funds, pun_lock_reason, pun_id_documents, pun_preg_code" .
        ", pun_login_lock_start, pun_email_reg_code, pun_sms_reg_code" .
        ", pun_registration_status, pun_pre_registered, pcr_total_deposits,pcr_fees_retained" .
        ", pcr_total_withdrawals, pun_password_set_time, pun_sre_code, pun_confirmation_list" .
        ", c.jur_name as \"Betting Club\"".
        ", d.jur_name as \"District\"".
        ", r.jur_name as \"Region\"".
        ", n.jur_name as \"Nation\"".
        ", cur_code_for_web as pun_currency ".
        " FROM punter " .
        " LEFT JOIN punter_credit on pun_id=pcr_pun_id " .
        " LEFT JOIN jurisdiction c ON pun_betting_club=c.jur_id" .
        " LEFT JOIN jurisdiction d ON c.jur_parent_id=d.jur_id" .
        " LEFT JOIN jurisdiction r ON d.jur_parent_id=r.jur_id" .
        " LEFT JOIN jurisdiction n ON r.jur_parent_id=n.jur_id" .
        " LEFT JOIN currency ON c.jur_currency=cur_id " ;
        if($search_type == SEARCH_TYPE_IPADDRESS ){
        	$sql .=" JOIN customer_log  ON clg_punid=pun_id";
        }          
        $sql .=" LEFT OUTER JOIN country ON pun_cou_code = cou_code" .
        " WHERE pcr_pun_id = pun_id "; //Modificata query Manuel Rinaldi

if (isset($safe_checked) && !empty($jurisdiction_level) && !empty($jurisdiction)) {
  if ($jurisdiction_level == "club"){
    $sql .= " AND c.jur_id =".$jurisdiction;
  }
  elseif ($jurisdiction_level == "district"){
    $sql .= " AND d.jur_id =".$jurisdiction;	
  }
  elseif ($jurisdiction_level == "region"){
    $sql .= " AND r.jur_id =".$jurisdiction;	
  }
  elseif ($jurisdiction_level == "nation"){
    $sql .= " AND n.jur_id =".$jurisdiction;		
  }
  else {
    $sql .= " AND 1 = 0 ";
  }
}
else {
  if ( 'club' == $_SESSION['jurisdiction_class'] ) { //If a club admin user
    //only show punters in same club as admin user
    $sql .= " and c.jur_id = ".$_SESSION['jurisdiction_id'];  
  }
  elseif ( 'district' == $_SESSION['jurisdiction_class'] )  { //If a district manager admin user
    $sql .= " and d.jur_id = ".$_SESSION['jurisdiction_id'];
  }
  elseif ( 'region' == $_SESSION['jurisdiction_class'] ) { //If a district manager admin user
    // only select punters where their betting club (bottom level jurisdiction)
    // is under the admin user's district
    // select only  records within admin user's jurisdiction
    $sql .= " and r.jur_id = ".$_SESSION['jurisdiction_id'];          
  }
  elseif ( 'nation' == $_SESSION['jurisdiction_class'] )  { //If a district manager admin user
    // only select punters where their betting club (bottom level jurisdiction)
    // is under the admin user's district
    // select only  records within admin user's jurisdiction
    $sql .= " and n.jur_id = ".$_SESSION['jurisdiction_id']; 
  }
  elseif ( 'casino' != $_SESSION["jurisdiction_class"] ) {
    $sql .= " and 1 = 0 ";
  }
}
?>
