<?php
   
   // customer_result_daily cron job

   // usage: php customer_result_daily.php dbconf.inc [date]
   // 
   //        dbconf.inc
   //           path to database configuration file (required to define _DBHOST, _DBNAME, _USERNAME, _PASSWORD)
   //
   //        date (optional)
   //           insert data for the specified date (YYYY-MM-DD format) as opposed to yesterday's data (default)
   
   
   $DEBUG = true;
   
   // usage
   function usage($msg = null) {
      error("usage: php " . $_SERVER['argv'][0] . " config_file [YYYY-MM-DD]\n" . $msg);
   }   
    
   function error($msg) {
      echo $msg . "\n";
      exit;
   }
   
   ///////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////
   
   
   // make sure we get passed a config file
   if(empty($_SERVER['argv'][1]))
      usage();

   // dependancies
   ini_set('include_path', realpath(dirname(__FILE__).'/../classes').PATH_SEPARATOR.ini_get('include_path'));
   require_once("DatabaseHandler.class.inc");
   require_once($_SERVER['argv'][1]);   
   
   // make sure the required database related definitions have been defined in the config file
   if(!defined('DSN'))
      error("The config file `" . $_SERVER['argv'][1] . "` does not contain the required database information.");
   
   // assign a day to generate the report for (default to yesterday)
   $strdate = (empty($_SERVER['argv'][2])) ? 'yesterday' : $_SERVER['argv'][2];
   $time    = strtotime($strdate);
   $date    = date('Y-m-d', $time);
     
   // if we were passed a bogus date, error out
   if(!empty($_SERVER['argv'][2]))
   {
      if($date != $_SERVER['argv'][2] || $time === -1)
         error("Date mismatch.");
   }
      
      
   $db  = new DatabaseHandler;
   $db->connect();

   // error handling is included in DatabaseHandler::connect() / exec() so we dont need to
   // explicitly handle connection / query errors here
  /* 
   $vql = "FROM punter_result, result WHERE " .
          "pre_void > 0 AND " .
          "pre_res_id = res_id  AND " .
          "res_date between '" . $date . " 00:00:00' AND '" . $date . " 23:59:59' AND " .
          "pre_member_type = 'FINANCIAL' AND " .
          "pre_pun_id = x.pre_pun_id AND " .
          "res_gam_id = y.res_gam_id";
   
   $sql = "INSERT INTO customer_result_daily SELECT to_date(res_date, 'YYYY-MM-DD') as \"Day\", res_gam_id, pre_pun_id,   " .
          "sum(pre_bet) - (SELECT coalesce(sum(pre_bet),0) " . $vql . ") as \"pre_bet\", " .
          "sum(pre_win) - (SELECT coalesce(sum(pre_win),0) " . $vql . ") as \"pre_win\" FROM punter_result x, result y WHERE " .
          "pre_res_id = res_id  AND res_date between '" . $date . " 00:00:00' AND '" . $date . " 23:59:59' AND " .
          "pre_member_type = 'FINANCIAL' GROUP BY \"Day\", res_gam_id, pre_pun_id";
  */
   $sql = "INSERT INTO customer_result_daily SELECT res_date AS Day " .
          "           ,  res_gam_id, pre_pun_id " .
          "           ,  sum(pre_bet), sum(pre_win) " .
          "           ,  sum(pre_rake), pcr_credits " .
          "             FROM punter_result, result, punter_credit " .
          "             WHERE res_date BETWEEN '$date 00:00:00' AND '$date 23:59:59' " .
          "             AND res_id = pre_res_id " .
          "             AND pcr_pun_id = pre_pun_id " . 
          "             GROUP BY res_gam_id, pre_pun_id " .
          "             ORDER BY res_date ";
   $db->exec($sql);
   $db->disconnect();
   
   if($DEBUG)
      echo $db->getAffectedRows() . " records updated.\n";
?>
