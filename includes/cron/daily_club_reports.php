<?php
// dependancies
ini_set('include_path', realpath(dirname(__FILE__).'/../classes').PATH_SEPARATOR.ini_get('include_path'));
require_once("DatabaseHandler.class.inc");
require_once($_SERVER['argv'][1]);

// make sure the required database related definitions have been defined in the config file
if(!defined('DSN'))
  error("The config file `" . $_SERVER['argv'][1] . "` does not contain the required database information.");

if(empty($_SERVER['argv'][1])){
  error("usage: php " . $_SERVER['argv'][1] . " config_file [YY-WW]\n");
}

$dbh  = new DatabaseHandler;
$dbh->connect();


function getFinancialByClub($start_date, $end_date) {
  global $dbh;

  $archive_date = (($search_today) ? $yesterday_date : $end_date);

  $sql  = "SELECT pun_betting_club AS clubid, coalesce(sum(cus_csh_amount), 0) as financial, 0 as totalbet, 0 as totalwin, 0 as rake, 0 as tourney_bet, 0 as tourney_win, 0 as tourney_rake , 0 as casino
           FROM customer_cashdesk_daily
           JOIN punter ON cus_csh_pun_id=pun_id
           WHERE cus_csh_day BETWEEN to_date('$start_date', 'YYYY-mm-dd hh24:mi:ss') AND to_date('$end_date', 'YYYY-mm-dd hh24:mi:ss')
           GROUP BY pun_betting_club";

  $rs = $dbh->exec($sql);
  return $rs;
}

/***********************
 * RAKE
 ***********************/
function getRakeByClub($start_date, $end_date) {
  global $dbh;

  $sql  = "SELECT pun_betting_club AS clubid, 0 as financial, coalesce(sum(cus_res_bet), 0) as totalbet, coalesce(sum(cus_res_win), 0) as totalwin,
           coalesce(sum(cus_res_rake), 0) AS rake, 0 as tourney_bet,0 as tourney_win,0 as tourney_rake, 0 as casino
           FROM customer_result_daily
           JOIN punter on cus_res_pun_id=pun_id
           WHERE cus_res_day BETWEEN to_date('$start_date','YYYY-MM-DD HH24:MI:SS') AND to_date('$end_date','YYYY-MM-DD HH24:MI:SS')
           GROUP BY pun_betting_club";

  $rs = $dbh->exec($sql);
  return $rs;
}

/***********************
 * TOURNAMENTS
 ***********************/
function getTournamentByClub($start_date, $end_date) {
  global $dbh;
  $sql  = "SELECT pun_betting_club AS clubid, 0 as financial, 0 as totalbet, 0 as totalwin, 0 AS rake,
           SUM(tnt_buyin) AS tourney_bet, SUM(tnt_fee) AS tourney_rake, SUM(tnt_wins) AS tourney_win
            FROM tournament_entries
            JOIN punter ON pun_id = tnt_punid
            WHERE tnt_date BETWEEN to_date('$start_date','YYYY-MM-DD HH24:MI:SS') AND to_date('$end_date','YYYY-MM-DD HH24:MI:SS')
            GROUP BY pun_betting_club";
  $rs = $dbh->exec($sql);
  return $rs;
}

/******************
* CASINO
******************/
function getCasinoByClub($partnerid, $start_date, $end_date) {
  global $dbh, $jurisdiction;
  global $start_date, $end_date, $today_date, $yesterday_date, $search_today;

  $archive_date = (($search_today) ? $yesterday_date : $end_date);
  $sql = "SELECT pun_betting_club AS clubid, 0 as financial, coalesce(sum(cus_cas_amount), 0) as casino, 0 as totalbet, 0 as totalwin, 0 as rake, 0 as tourney_bet, 0 as tourney_win, 0 as tourney_rake
          FROM customer_casino_daily
          JOIN punter ON cus_cas_pun_id = pun_id
          WHERE cus_cas_day BETWEEN to_date('$start_date', 'YYYY-mm-dd hh24:mi:ss') AND to_date('$end_date', 'YYYY-mm-dd hh24:mi:ss')
          AND cus_cas_ptn_id = $partnerid
          GROUP BY pun_betting_club";

  $rs = $dbh->exec($sql);
  return $rs;
}


function error($msg) {
  w($msg);
  exit(0);
}

function w($msg){
  echo date("Y-m-d H:s") . " - " . $msg . "\n";
}

function getDataByClub($start_date, $end_date){

  $num_rows = 0;

  w("Getting data from $start_date to $end_date");

  w("Getting financial info");
  $rs = getFinancialByClub($start_date, $end_date);
  w("Found " . $rs->getNumRows() . " results");

  w("Getting rake info");
  $rs2 = getRakeByClub($start_date, $end_date);
  w("Found " . $rs2->getNumRows() . " results");

  w("Getting tournaments info");
  $rs3 = getTournamentByClub($start_date, $end_date);
  w("Found " . $rs3->getNumRows() . " results");

  w("Getting casino info");
  $rs4 = getCasinoByClub(4, $start_date, $end_date);
  w("Found " . $rs4->getNumRows() . " results");


  while ( $rs2->hasNext() ) {  //append other columns to records
    $rec2  = $rs2->next();
    $found = false;
    $rs->resetRecPtr();  //to start searching for same game
    while ( $rs->next() ) {
      $rec = & $rs->currentRef();
      if ( $rec['clubid'] == $rec2['clubid'] ) { //match found
        $rec['totalbet'] = $rec2['totalbet'];
        $rec['totalwin'] = $rec2['totalwin'];
        $rec['rake']     = $rec2['rake'];
        $found = true;
        break; //since found
      }
    }
    if ( ! $found ) {
      $rec2['financial'] = 0;
      $rs->appendRecords(array($rec2));
    }
  }

  while ( $rs3->hasNext() ) {
    $rec3  = $rs3->next();
    $found = false;
    $rs->resetRecPtr();  //to start searching for same game
    while ( $rs->next() ) {
      $rec = & $rs->currentRef();
      if ( $rec['clubid'] == $rec3['clubid'] ) { //match found
        $rec['tournament'] = $rec3['tournament'];
        $found = true;
        break; //since found
      }
    }
    if ( ! $found ) {
      $rs->appendRecords(array($rec3));
    }
  }


  while ( $rs4->hasNext() ) {
    $rec4  = $rs4->next();
    $found = false;
    $rs->resetRecPtr();  //to start searching for same game
    while ( $rs->next() ) {
      $rec = & $rs->currentRef();
      if ( $rec['clubid'] == $rec4['clubid'] ) { //match found
        $rec['casino'] = $rec4['casino'];
        $found = true;
        break; //since found
      }
    }
    if ( ! $found ) {
      $rs->appendRecords(array($rec4));
    }
  }

  return $rs;
}

define("MAX_MULTIPLE_INSERT", 50);

function getMultiInsertSql($insert_sql){
    $sql  = "INSERT ALL\n";
    $sql .= $insert_sql;
    $sql .= "SELECT * FROM dual\n";
    return $sql;
}

$start_time = strtotime($_SERVER['argv'][2]);

if(!$start_time){
  $start_time = strtotime("yesterday");
}

$end_time   = $start_time;

$start_date = date("Y-m-d H:i:s", $start_time);
$end_date   = date("Y-m-d H:i:s", $end_time);

$res = getDataByClub($start_date, $end_date);
$res->resetRecPtr();
//print "Club\t\tBet\t\tWin\t\tRake\t\tT Bet\t\tT Win\t\tT Fee\t\tCasino";

$counter = 0;
$insert_cache = "";
while($res->hasNext()){
  $result = $res->next();
  //w($result["clubid"]);
  $insert_cache .= "INTO club_daily_summary VALUES(TO_DATE('" . date("Y-m-d", $start_time) . "', 'YYYY-MM-DD'), {$result["clubid"]}, {$result["financial"]}, {$result["totalbet"]}, {$result["totalwin"]}, {$result["rake"]}, {$result["casino"]}, {$result["tourney_bet"]}, {$result["tourney_win"]}, {$result["tourney_rake"]})\n";
  if(++$counter%MAX_MULTIPLE_INSERT == 0){
    $sql = getMultiInsertSql($insert_cache);
    w("Inserting " . MAX_MULTIPLE_INSERT . " rows...");
    $rs  = $dbh->exec($sql);
    if(is_numeric($rs)){
      w("$rs rows insert");
    }else{
      w("ERROR - Problem inserting " . MAX_MULTIPLE_INSERT . " rows:\n$rs\n$sql");
    }
    $insert_cache = "";
  }
}

w("Inserting last rows...");
$sql = getMultiInsertSql($insert_cache);
$rs  = $dbh->exec($sql);
if(is_numeric($rs)){
  w("Last $rs rows insert");
}else{
  w("ERROR - Problem inserting last rows:\n$rs\n$sql");
}
?>