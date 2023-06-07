<?php
check_access('handle_processor_deposit', $doredirect=true);
global $lang;
?>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/dataTablesExtra/TableTools-2.2.0/css/dataTables.tableTools.css" title="core_style" />
<script src="<?= secure_host ?>/media/jquery.dataTables.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/dataTablesExtra/TableTools-2.2.0/js/dataTables.tableTools.js" type="text/javascript"></script>
<?php
//require_once realpath("./../config")."/site.config.php";
//require_once "commons.inc.php";
if(!extension_loaded('curl')){
    dl("curl.so");
}
//echo "<pre>";
//print_r($_SESSION);
//exit;

$jur_id = $_SESSION['jurisdiction_id'];
$jur_class = $_SESSION['jurisdiction_class'];

$class = '';
switch ($jur_class) {
    case 'nation':
        $class = 'AND n.jur_id = '.$jur_id;
        break;
    case 'region':
        $class = 'AND r.jur_id = '.$jur_id;
        break;
    case 'district':
        $class = 'AND d.jur_id = '.$jur_id;
        break;
    case 'club':
        $class = 'AND c.jur_id = '.$jur_id;
        break;
}


define("T_TYPE_REFUND", 6);
define("T_TYPE_PROCESSOR_WITHDRAWAL", 10);
define("T_TYPE_PROCESSOR_DEPOSIT",    3);
define('LOG_FILE_NAME', '/var/log/httpd/gatewaytransaction.log');
define('HEADER_FILE_LOG', 'gatewaytransaction.php');
$action = "SHOW";
$thispage = SELF . "?page=" . $_GET['page'];

// Il puntatore al file di log
$log = null;

if(isset($_GET['action'])){
    $action = strtoupper(trim($_GET['action']));
}

$id = 0;
$starttime = microtime(true);
if(isset($_GET['id'])){
    $id = (int)$_GET['id'];
}

if($action != "SHOW"){
    $sql = "SELECT prc_id, prc_name, ptr_uniqueid, ptr_status, ptr_amount, ptr_pun_id,ptr_admin_user_id FROM processor_transaction JOIN processor ON ptr_prc_id = prc_id WHERE ptr_id = $id";

    $rs       = $dbh->exec($sql);
    $num_rows = $rs->getNumRows();


    $dbh->begin();

    if ($num_rows != 1 ){
        addError('', $lang->getLang("Transaction error..."));
        $dbh->rollback();
        break;
    }

    $rec    = $rs->next();
    $amount = $rec['ptr_amount'];
    $pun_id = $rec['ptr_pun_id'];
    $aus_id = $_SESSION["admin_id"];
    $status = $rec['ptr_status'];
    $procId = $rec['prc_id'];
    $tranId = $rec['ptr_uniqueid'];
    // Viene inviato il messaggio per la gestione della transazione in pending dell'utente
    $result = send($tranId, strtolower($action), $pun_id, $procId, $aus_id, $_SESSION["admin_username"],$rec['ptr_admin_user_id']);

    // Viene chiusa la connessione al DataBase
    $dbh->rollback();


    if($result != '0'){
        addError("", $lang->getLang("Impossible to save the status of the transaction."));
    } else {
        printInfoMessage($lang->getLang("Transaction of % successfully processed, the money are the money has been returned to the customer in case of request for payment",getInDollars($amount)));
    } // end if - else

}
showErrors();
$sql = "select
          ptr_id,
          pun_id,
          pun_username,
          ptr_amount,
          ptr_start,
          ptr_user,
          ptr_uniqueid,
          ptr_note,
          ptr_punip,
          ptr_merchant_account,
          ptr_processor_note,
          ptr_ppc_code,
          prc_name,
          aus_username,
          ptr_admin_user_id,
          ccs_conversion_rate,
          ptr_amount_converted,
          curr1.cur_code_for_web cur_code_for_web,
          curr2.cur_code_for_web converted_currency_sym
    FROM jurisdiction n
    JOIN jurisdiction r ON r.jur_parent_id = n.jur_id AND n.jur_parent_id = 1
    JOIN jurisdiction d ON d.jur_parent_id = r.jur_id
    JOIN jurisdiction c ON c.jur_parent_id = d.jur_id
    JOIN punter ON pun_betting_club = c.jur_id
    JOIN processor_transaction ON pun_id = ptr_pun_id
    JOIN processor ON ptr_prc_id = prc_id
    LEFT JOIN admin_user ON ptr_admin_user_id = aus_id
    LEFT JOIN currencies_conversion_session ON ptr_ccs_id = ccs_id
    LEFT JOIN currency curr1 ON curr1.cur_id = ccs_cur_id_from
    LEFT JOIN currency curr2 ON ccs_cur_id_to = ccs_cur_id_to
        WHERE
          ptr_status = 'P' $class
        GROUP BY ptr_uniqueid
        ORDER BY
          ptr_merchant_account ASC,
          pun_username ASC,
          prc_name ASC";

function checkAdmin($admin,$name,$user_id){
    if($admin!=""){
        $name=getClassLink("admin_users/admin_user_edit.php?id={$admin}",$name).'<br /><span class="tip">( Admin )</span>';
    }
    else{
        $name=getClassLink(refPage("customers/customer_view&customer_id=" . $user_id),$name);
    }
    return $name;
}

$res = $dbh->exec($sql);

if($res) {
    $cols_arr = array($lang->getLang('Date'),$lang->getLang('Account'), $lang->getLang('External ID'), $lang->getLang('Processor / Type'), $lang->getLang('Username'),$lang->getLang('Amount'),$lang->getLang('Amount Converted'),$lang->getLang('Conversion Rate'),$lang->getLang('Processor User'), $lang->getLang('Notes'), $lang->getLang('Action'));
    $val_fmt_arr = array (
        $lang->getLang('Date')             =>'return shortFormatDate($rec["ptr_start"]);',
        $lang->getLang('Account')          =>'return $rec["ptr_merchant_account"]."<br /><span class=tip>".$rec["ptr_processor_note"]."</span>";',
        $lang->getLang('External ID')      =>'return getClassLink(refPage("poker/searchprocessordeposit") . "&search=watch&form_submitted=yes&field=6&value=" . $rec["ptr_uniqueid"], $rec["ptr_uniqueid"]);',
        $lang->getLang('Processor / Type') =>'return $rec["prc_name"]."<br /><span class=tip>(".$rec["ptr_ppc_code"].")</span>";',
        $lang->getLang('Username')         =>'return checkAdmin($rec["ptr_admin_user_id"],$rec["pun_username"],$rec["pun_id"]); ',
        $lang->getLang('Amount')           =>'return getInDollars($rec["ptr_amount"],2,$rec["cur_code_for_web"]);',
        $lang->getLang('Amount Converted') =>'return ($rec["ptr_amount_converted"] !=0 ? getInDollars($rec["ptr_amount_converted"],2,$rec["converted_currency_sym"]):"");',
        $lang->getLang('Conversion Rate')  =>'return $rec["ccs_conversion_rate"];',
        $lang->getLang('Processor User')   =>'return getClassLink(refPage(\'poker/searchprocessordeposit&action=search&field=5&value=\' . urlencode($rec["ptr_user"])), $rec["ptr_user"]);',
        $lang->getLang('Notes')            =>'return getClassLink(refPage(\'customers/search&field=4&search_string=\' . urlencode(ereg_replace("[^0-9\.]", "", $rec["ptr_punip"]))), $rec["ptr_punip"]) . " " . $rec["ptr_note"];',
        $lang->getLang('Action')           =>'return "<a href=\'javascript:if(confirm(\"Sicuro di voler accettare la transazione di per " . $rec["pun_username"] . "?\")){document.location = \"' . $thispage . '&action=accepted&id=" . $rec["ptr_id"] . "\";};\'><font color=\"green\">Accetta</font></a> |
                                          <a href=\'javascript:if(confirm(\"Sicuro di voler declinare la transazione di per  " . $rec["pun_username"] . "?\")){document.location = \"' . $thispage . '&action=declined&id=" . $rec["ptr_id"] . "\";};\'><font color=\"red\">Declina </font></a> ";'
    );
    $deposits = array();
    $withdraw = array();
    $total_with=0;
    $total_dep=0;
    foreach ($res->Records as $transaction){
        if($transaction['ptr_amount']>0){
            $total_dep += $transaction['ptr_amount'];
            array_push($deposits, $transaction);
        }else{
            array_push($withdraw, $transaction);
            $total_with += $transaction['ptr_amount'];
        }
    }
    $endtime = microtime(true);
    $duration = $endtime - $starttime;
    $duration=number_format($duration, 4, ',', '')."s";


    ?>
    <script type='text/javascript'>
    var seconds_left = 21;
    var interval = setInterval(function() {
    document.getElementById('timer_div').innerHTML = "<?=$lang->getLang("The page will automatically refresh in")?>: "+--seconds_left +" seconds" ;

    if (seconds_left <= 0)
    {
    document.getElementById('timer_div').innerHTML = 'The page will now refresh..';
    clearInterval(interval);
        window.location.replace("/search_pending_transactions")
    }
    }, 1000);
    </script>

    <div class="centered">
      <div class="tip" id="timer_div">&nbsp;</div><br>
        <div class="tip"><?=$lang->getLang("Time taken to execute your request")?>:<?=$duration?></div>
        <h2 class="centered"><?=$lang->getLang("Total pending amount")?></h2>
    <table bgcolor="#a99a9" cellpadding="4" cellspacing="1" border="0" style='margin:auto'>
        <tr>
            <td class='label'><?=$lang->getLang("Total deposit requests")?></td><td class="label"><?=$lang->getLang("Total withdrawal requests")?></td>
        </tr>
        <tr>
            <td class="content"><?=getInDollars($total_dep)?></td><td class='content'><?=getInDollars($total_with)?></td>
        </tr>
    </table>
    </div>
    <?

    $rs_deposits = new RecordSet($deposits);
    $rs_withdraw = new RecordSet($withdraw);
    echo "<h2>".$lang->getLang("Pending Deposits")."</h2>";
    $qp = new QueryPrinter(null);
    $qp->printTable($rs_deposits, $cols_arr, $post_vars, $val_fmt_arr, $cell_fmt_arr, $sum_fmt_arr, $excel_filename, $header="", $width="", $start_row, count($deposits));
    echo "<h2>".$lang->getLang("Withdrawal Requests")."</h2>";
    $qp = new QueryPrinter(null);
    $qp->printTable($rs_withdraw, $cols_arr, $post_vars, $val_fmt_arr, $cell_fmt_arr, $sum_fmt_arr, $excel_filename, $header="", $width="", $start_row, count($withdraw));
}

/**
 * Effettua l'invio della richiesta al server del partner per l'estrazione dell'id locale dell'utente.
 *
 * @param $sessionId il sessionid locale associato all'utente
 * @param $tableId l'id del tavolo
 * @param $tableThemeId l'id del tema del tavolo
 * @param $hostName il nome dell'host al quale connettersi
 * @param $md5 l'md5 di sicurezza generato per il partner
 * @return la risposta del partner
 */
function send($transactionId, $status, $userId, $processorId, $agentId, $ausName,$auid=null) {

    // Viene generata la chiave per l'autenticitˆ del messaggio
    $DataToSend ="operationtype=3011&agid=" . $agentId . "&procid=" . $processorId . "&tranid=" . $transactionId . "&status=" . $status .
        "&userid=" . $userId . "&ausname=" . $ausName."&auid=".$auid;
    log_data("   POST request parameter to Send: " . $DataToSend);

    // Apro la connessione
    $ch=curl_init(WEB_APP_WEBSITES);

    // Imposto gli headers HTTP
    // imposto curl per protocollo https
    curl_setopt($ch,CURLOPT_SSL_VERIFYPEER, FALSE);
    curl_setopt($ch,CURLOPT_POST,1);

    // Invio i dati
    curl_setopt($ch,CURLOPT_POSTFIELDS, $DataToSend);

    //imposta la variabile PHP
    curl_setopt($ch,CURLOPT_RETURNTRANSFER, 1);

    //Ricevo la risposta dal server
    $result=curl_exec($ch);
    return $result;
} // end function send()

/**
 * Effettua la scrittura dei messaggi di log
 *
 * @param $msg il messaggio da scrivere nei log
 */
function log_data($msg){
    global $log;
    if(empty($log)) {
        $log = fopen(LOG_FILE_NAME,'a');
    } // end if
    fwrite($log, date(" H:i:s") . " - " . HEADER_FILE_LOG . " - " . $msg . "\n");
} // end function log_data()


/**
 * Effettua la chiusura del file di log
 */
function close() {
    global $log;
    fclose($log);
} // end function close()


?>
