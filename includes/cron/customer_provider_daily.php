<?php
   
   // customer_provider_daily cron job

   // usage: php customer_provider_daily.php dbconf.inc [date]
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

   $sql = "INSERT INTO customer_provider_daily (cus_pro_day, cus_pro_pun_id, cus_pro_bet_type, cus_pro_provider_id, cus_pro_bet, cus_pro_win, cus_pro_num_bets)
             SELECT 
               TRUNC(ctr_time, 'DDD'), ctr_pun_id, pes_game_code, ctr_pesid, 
               COALESCE(SUM(DECODE(ctr_tty_id, 8, -(ctr_amount))), 0),
               COALESCE(SUM(DECODE(ctr_tty_id, 7, ctr_amount)), 0),
               COUNT(ctr_id)
             FROM 
               customer_transaction
             JOIN
               providers ON pes_id = ctr_pesid
             WHERE 
               ctr_tty_id IN (7, 8) AND ctr_pme_code = 'PRV' AND ctr_pesid IS NOT NULL
               AND ctr_time BETWEEN TO_DATE('$date 00:00:00', 'YYYY-MM-DD HH24:MI:SS') AND TO_DATE('$date 23:59:59', 'YYYY-MM-DD HH24:MI:SS') AND ctr_pesid != 2
             GROUP BY 
               TRUNC(ctr_time, 'DDD'), ctr_pun_id, pes_game_code, ctr_pesid";
    
   $db->exec($sql);
   $db->disconnect();
   
   if($DEBUG)
      echo $db->getAffectedRows() . " records updated.\n";
?>
