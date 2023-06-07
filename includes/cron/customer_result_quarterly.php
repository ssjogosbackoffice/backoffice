<?php
 /**
  * usage: php customer_result_quarterly.php <dbconf.inc> <outfile>
  *        dbconf.inc
  *           path to database configuration file (required to define _DBHOST, _DBNAME, _USERNAME, _PASSWORD)
  *        outdir
  *           output dir
  */
   
  $DEBUG = true;

  function error($msg) {
    printf("%s\n", $msg);
    exit;
  }

  // usage
  function usage($msg = null) {
    error("usage: php " . $_SERVER['argv'][0] . " <db_config_file>\n" . $msg);
  }   

  ///////////////////////////////////////////////////////////////
  if(empty($_SERVER['argv'][1]))
     usage();
  ///////////////////////////////////////////////////////////////

  // dependancies
  $mydir = dirname(__FILE__);
  $incdir = realpath($mydir.'/../');
  $classdir = $incdir.'/classes';
  ini_set('include_path', $incdir.PATH_SEPARATOR.$classdir.PATH_SEPARATOR.ini_get('include_path'));
  require_once("DatabaseHandler.class.inc");
  require_once($_SERVER['argv'][1]);   

  // make sure the required database related definitions have been defined in the config file
  if(!defined('_DBHOST'))
     error("The config file `" . $_SERVER['argv'][1] . "` does not contain the required database information.");

  $date    = date('Y-m-d');

  $db  = new DatabaseHandler();
  $db->connect();

  $db->begin();  
  $db->exec("TRUNCATE TABLE customer_result_quarterly_tmp");


  // error handling is included in DatabaseHandler::connect() / exec() so we dont need to
  // explicitly handle connection / query errors here
  $vql = "FROM punter_result, result WHERE " .
         "pre_void > 0 AND " .
         "pre_res_id = res_id  AND " .
         "res_date between '" . $date . " 00:00:00' AND '" . $date . " 23:59:59' AND " .
         "pre_member_type = 'FINANCIAL' AND " .
         "pre_pun_id = x.pre_pun_id AND " .
         "res_gam_id = y.res_gam_id";
   
  $sql = "INSERT INTO customer_result_quarterly_tmp " .
         "  SELECT to_date(res_date, 'YYYY-MM-DD') as \"Day\",".
         "  res_gam_id, pre_pun_id," .
         "  sum(pre_bet) - (SELECT coalesce(sum(pre_bet),0) " . $vql . ") as \"pre_bet\", " .
         "  sum(pre_win) - (SELECT coalesce(sum(pre_win),0) " . $vql . ") as \"pre_win\" " .
         "   FROM punter_result x, result y " .
         "    WHERE pre_res_id = res_id  " .
         "      AND res_date between '" . $date . " 00:00:00' AND '" . $date . " 23:59:59' " .
         "      AND pre_member_type = 'FINANCIAL' GROUP BY \"Day\", res_gam_id, pre_pun_id";

  $db->exec($sql);
   
  $db->exec("TRUNCATE TABLE customer_result_quarterly");

  $rs = $db->exec("SELECT * FROM customer_result_quarterly");

  $sql = "INSERT INTO customer_result_quarterly " .
         " SELECT cus_res_day, cus_res_gam_id, cus_res_pun_id, cus_res_bet, cus_res_win " .
         " FROM customer_result_quarterly_tmp ";

  $db->exec($sql);

  $db->commit(); 

  $db->disconnect();
?>
