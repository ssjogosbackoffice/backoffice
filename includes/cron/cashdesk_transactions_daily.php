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

   $sql = "INSERT INTO customer_cashdesk_daily (cus_csh_day, cus_csh_pun_id, cus_csh_amount)" .
          " SELECT ctr_time as Day, ctr_pun_id as Customer, coalesce(sum(ctr_amount), 0) as Financial " .
          "    FROM customer_transaction " .
	  "    WHERE ctr_time BETWEEN '$date 00:00:00' AND '$date 23:59:59' " .
          "      AND ctr_tty_id IN(8 ,7)" .
          "    GROUP BY  ctr_pun_id" .
          "    ORDER BY ctr_time";
          
          
   $db->exec($sql);
   $db->disconnect();
   
   if($DEBUG)
      echo $db->getAffectedRows() . " records updated.\n";
?>
