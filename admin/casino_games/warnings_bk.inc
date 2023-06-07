<style>
  a.deny{
    color:Red;
  }
</style>
<?php
check_access('customers_dangerous', $redirect=true);
define("WARNING_RAKE_PERC", 3);
define("WARNING_MIN_WINS",  100);
define("HISTORY_DAYS",      90);

$action = $_GET["action"];

$date = date("d-m-Y", strtotime("yesterday"));

switch ($action){
  case "details":
    $pun_id       = $_GET["id"];
    $pun_username = $_GET["username"];
    
    $date = date("d-m-Y", strtotime("$date - " . HISTORY_DAYS . " days"));
    
    settype($pun_id, "integer");
    
    ?>
    <a href="<?=refFormPage($_GET["page"])?>">Return to List</a><br/><br/>
    <div class="bodyHD"><?=$lang->getLang("Details for last % days of % since %",HISTORY_DAYS,$pun_username,$date)?></div>
    <?
    $date = date("Y-m-d", strtotime($date));
    $sql = "SELECT cus_res_day, SUM(cus_res_bet) bets, SUM(cus_res_win) wins, SUM(cus_res_rake) rake 
            FROM customer_result_daily 
            WHERE cus_res_day >= '$date' AND cus_res_pun_id = " . $pun_id . " GROUP BY cus_res_day";
    
    echo $sql;
    die();
    $rs = $dbh->exec($sql);
    
    $cols_arr    = array("Day", "Bet", "Win", "Net", "Rake", "% Rake");

    $val_fmt_arr = array (
      'Day'         =>'return date("d-m-Y", strtotime($rec["cus_res_day"]));',
      'Bet'         =>'return getInDollars($rec["bets"]);',
      'Win'         =>'return getInDollars($rec["wins"]);',
      'Rake'        =>'return getInDollars($rec["rake"]);',
      'Net'         =>'return getInDollars($rec["wins"] - $rec["bets"]);',
      '% Rake'      =>'return number_format($rec["rake"]/$rec["bets"]*100, 2) . "%";'
    );
    
    $cell_fmt_arr = array ();
    
    $numrow = $rs->getNumRows();
    if($numrow > 0){
      $qp = new QueryPrinter($rs);
      $qp->printTable($rs, $cols_arr, "", $val_fmt_arr, $cell_fmt_arr, 0, 0, "", $width="100%", 1, $numrow, false);
    }else{
      addError("", "No warnings today :)");
      showErrors();
    }    
    
    break;
  default:
    
    
    ?>
    <div class="bodyHD"><?=$lang->getLang("Warning for players with less then % of rake for %",WARNING_RAKE_PERC,$date)?></div>
    <?
    $date = date("Y-m-d", strtotime($date));
    $sql = "SELECT 
              cus_res_pun_id, pun_username, pun_access, club.jur_id club_id , club.jur_name club, 
              district.jur_id district_id, district.jur_name district, pcr_credits, SUM(cus_res_bet) AS bets, 
              SUM(cus_res_win) AS wins, SUM(cus_res_rake) AS rake FROM customer_result_daily 
              JOIN punter on cus_res_pun_id = pun_id 
              JOIN punter_credit ON cus_res_pun_id = pcr_pun_id 
              JOIN jurisdiction club ON jur_id = pun_betting_club
              JOIN jurisdiction district ON club.jur_parent_id = district.jur_id
              WHERE cus_res_day = '$date' 
              AND cus_res_win != 0 
              AND ((cus_res_rake / cus_res_bet) < " . WARNING_RAKE_PERC / 100 . ") 
              AND ABS(cus_res_win - cus_res_bet) > " . WARNING_MIN_WINS;
    
    $jur_id = $_SESSION["jurisdiction_id"];
  
    if($_SESSION["jurisdiction_class"] != "casino"){
      $sql .= "JOIN jurisdiction club     ON club.jur_id     = pun_betting_club
    		       JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
               JOIN jurisdiction regional ON regional.jur_id = district.jur_parent_id
               JOIN jurisdiction nation   ON nation.jur_id   = regional.jur_parent_id";
    }
    
    switch($_SESSION["jurisdiction_class"]){
      case "club":
        $sql .= " AND club.jur_id = $jur_id";
        break;
      case "district":
        $sql .= " AND district.jur_id = $jur_id";
        break;
      case "region":
        $sql .= " AND regional.jur_id = $jur_id";
        break;
      case "nation":
        $sql .= " AND nation.jur_id = $jur_id";
        break;
      case "casino":
        $sql .= " AND 1=1";
        break;
      default:
        $sql .= " AND 1=0";
        break;
    }
    
    $sql .= " GROUP BY 
                cus_res_pun_id, pun_username, pun_access, club.jur_id, club.jur_name, 
                district.jur_id, district.jur_name, pcr_credits";
    
    $sql .= " ORDER BY SUM(ABS(cus_res_win - cus_res_bet)) DESC, SUM(cus_res_win) DESC";

    //die("<pre>$sql</pre>");
    $rs = $dbh->exec($sql);
    
    
    $cols_arr    = array("Username", "Balance", "Club", "District", "Bet", "Win", "Net", "Rake", "% Rake", "Action");
    
    $val_fmt_arr = array (
      'Username'    =>'return "<a class=\'" . $rec["pun_access"] . "\' href=\'" . refFormPage("customers/customer_view&customer_id=" . $rec["cus_res_pun_id"] . "&header_page=") . "\'>" . $rec["pun_username"] . (($rec["pun_access"] == "deny")?(" (Locked)"):("")) . "</a>";',
      'Balance'     =>'return getInDollars($rec["pcr_credits"]);',
      'Club'        =>'return getClassLink("/jurisdictions/?node=" . $rec["club_id"], $rec["club"]);',
      'District'    =>'return $rec["district"];',
      'Bet'         =>'return getInDollars($rec["bets"]);',
      'Win'         =>'return getInDollars($rec["wins"]);',
      'Rake'        =>'return getInDollars($rec["rake"]);',
      'Net'         =>'return getInDollars($rec["wins"] - $rec["bets"]);',
      '% Rake'      =>'return number_format($rec["rake"]/$rec["bets"]*100, 2) . "%";',
      'Action'      =>'return "<a href=\'" . refFormPage($_GET["page"] . "&action=details&id=" . $rec["cus_res_pun_id"]) . "&username=" . urlencode($rec["pun_username"]) . "\'>View last ' . HISTORY_DAYS . ' days</a>";'
    );
    
    $cell_fmt_arr = array ();
    $numrow = $rs->getNumRows();
    if($numrow > 0){
      $qp = new QueryPrinter($rs);
      $qp->printTable($rs, $cols_arr, "", $val_fmt_arr, $cell_fmt_arr, 0, 0, "", $width="100%", 1, $numrow, false);
    }else{
      addError("", $lang->getLang("No warnings today :)"));
      showErrors();
    }    
        
    break;
}




?>
