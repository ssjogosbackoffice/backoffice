<?php
/**
 * Created by CSLIVEGAMES.
 * User: Manuel
 * Date: 19/06/13
 * Time: 12.15
 * File: testlelio.php
 */

//echo $betting_db->isConnected();
//$betting_db->begin();
//$sql = "INSERT INTO ProfiliCommissioni (IDProfiloCommissioneTipo,jur_id,Raggruppamento,Commissione) VALUES (1,300,32,30)";
//$betting_db->exec($sql);
//$betting_db->rollback();
//echo "Rollback effettuata";
include "Db.php";
define ("DB_HOST", "193.242.104.14");
define ("DB_DBASE", "betting_lga");
define ("DB_USER", "betting");
define ("DB_PWD", "qazwsx");
Db::setConnectionInfo(DB_DBASE,DB_USER,DB_PWD,"mysql",DB_HOST);

if(Db::beginTransaction()){

    Db::execute("INSERT INTO ProfiliCommissioni (IDProfiloCommissioneTipo,jur_id,Raggruppamento,Commissione) VALUES (1,300,32,30);");
    echo "Query eseguita<br/>";
    Db::commitTransaction();
    echo "Query rollback";
}

?>