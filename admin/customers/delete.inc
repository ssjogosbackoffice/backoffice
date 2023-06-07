<?php
check_access("delete_punter", true);

$action = $_REQUEST["action"];

switch($action){
  case "delete":
    $punter_id = $_GET["id"];
    settype($punter_id, "integer");
    if(deleteUser($punter_id)){
      printInfoMessage("User deleted correctly");
    }
    
    echo getLink(refFormPage("customers/customer_view&customer_id=$punter_id"), "Torna all'utente");
    
    break;
  default:
    showPunterList();
    break;	  
}

if(areErrors()){
  showErrors();
}
    
function showPunterList(){
  global $dbh;
  $sql = "SELECT jur_id FROM jurisdiction WHERE jur_code ='666' AND jur_class = 'club'";
  $rs  = $dbh->exec($sql);

  if(!$rs->hasNext()){
    addError("", "Special jurisdiction not found");
    return false;
  }
  
  
  $row      = $rs->next();
  $grave_id = $row["jur_id"];
  
    
  $sql = "SELECT pun_id, pun_customer_number, pun_username, pun_email FROM punter WHERE pun_betting_club = $grave_id ORDER BY LOWER(pun_username)";
  $rs  = $dbh->exec($sql);
  
  if(!$rs->hasNext()){
    addError("", "No deleted users found");
    return false;
  }
  

  $cols_arr = array(
    'C. Number',
    'Username',
    'Email'
  );

  $val_fmt_arr = array (
    'C. Number'        => 'return $rec["pun_customer_number"];',
    'Username'         => 'return getClassLink(refFormPage("customers/customer_view&customer_id="   . $rec["pun_id"]), $rec["pun_username"]);',
    'Email'            => 'return $rec["pun_email"];'
  );

  $qp = new QueryPrinter($rs);
  $qp->printTable($rs, $cols_arr, array(), $val_fmt_arr, $cell_fmt_arr, $sum_fmt_arr = "", $excel_filename = "", "", "", 1, 20000, false, "themesearch");
  
}

function deleteUser($punter_id){
  global $dbh;
  $sql = "SELECT pcr_credits FROM punter_credit WHERE pcr_pun_id = $punter_id";
  
  $rs  = $dbh->exec($sql);
  
  if(!$rs->hasNext()){
    addError("", "Punter not found");
    return false;
  }
  
  $row = $rs->next();
  $credits = $row["pcr_credits"];
  
  if($row["pcr_credits"] > 0){
    addError("", "Can't delete user, has " . getInDollars($row["pcr_credits"]) . " on his account");
    return false;
  }
  
  $sql = "SELECT jur_id FROM jurisdiction WHERE jur_code ='666' AND jur_class = 'club'";
  $rs  = $dbh->exec($sql);

  if(!$rs->hasNext()){
    addError("", "Special jurisdiction not found");
    return false;
  }
  
  $row      = $rs->next();
  $grave_id = $row["jur_id"];
  $prefix   = "_ex_";
  $sql = "UPDATE punter SET pun_betting_club = $grave_id, pun_username = CONCAT('$prefix' , pun_username), pun_access = 'deny', pun_email = '$prefix' || pun_email, pun_nin_code = CONCAT('$prefix' ,pun_nin_code) WHERE pun_id = $punter_id AND pun_betting_club != $grave_id";

  $rs = $dbh->exec($sql);
  return ($rs == 1);
}


?>