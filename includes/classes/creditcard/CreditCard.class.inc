<?php

if ( ! class_exists('CreditCard')) {

include("Punter.class.inc");
include("CreditCardValidator.class.inc");
include_once(include_content("CreditCardErrors.inc"));
//include_once("CreditCardErrors.inc.tpl");

define(MIN_WDW_DEPOSIT, 30.00);
define(MIN_WDW_GAMES, 50);

abstract class CreditCard extends Properties {
  protected $errno;
  protected $user;
  function __construct($user, $cardnumber) {
    $this->user = $user;
    $this->set("pun_id", $user->get("id"));
    $this->set("cardnumber", $cardnumber);
    $this->set("amount", 0.00);
    $this->set("pun_betting_club", $this->user->get("betting_club"));
    $this->set("pun_betting_club_name", $this->user->get("Betting Club"));
    $this->errno = 0;
  }

  protected function validateDeposit(&$amount) {
    if (!CreditCards::isValidMoney($amount)) {
      $this->setError(CC_ERR_BAD_FMT_AMOUNT);
      return false;
    }
    if ($amount < CC_MIN_DEPOSIT) {
      $this->setError(CC_ERR_UNDER_MIN_TRANS_DEPOSIT);
      return false;
    } else if ($amount > CC_MAX_DEPOSIT) {
      $this->setError(CC_ERR_EXCEED_MAX_TRANS_DEPOSIT);
      return false;
    }
    if ($this->user->get("Betting Club") != "Internet") {
      $this->setError(CC_ERR_INVALID_USER);
      return false;
    }
    return true;
  }
  
  protected function validateWithdrawal(&$amount) {
    global $max_trans_withdrawal;
    if (!CreditCards::isValidMoney($amount)) {
      $this->setError(CC_ERR_BAD_FMT_AMOUNT);
      return false;
    }
    if ($amount < CC_MIN_WITHDRAWAL) {
      $this->setError(CC_ERR_UNDER_MIN_TRANS_WITHDRAWAL);
      return false;
    }
    if ($amount > CC_MAX_WITHDRAWAL) {
      $this->setError(CC_ERR_EXCEED_MAX_TRANS_WITHDRAWAL);
      return false;
    }
    
    if ($this->user->get("Betting Club") != "Internet") {
      $this->setError(CC_ERR_INVALID_USER);
      return false;
    }

    $avail_balance = $this->user->getAvailableBalance();    
    if ($amount > $avail_balance) {
      $this->setError(CC_ERR_INSUFFICIENT_FUNDS);
      return false;
    }

    $last_deposit = $this->user->getLastDeposit();

    if ($last_deposit["pme_code"] != $this->getTypeAsString()) {
      $this->setError(CC_ERR_INVALID_USER);
      return false;
    }

    $credit_limit = MIN_WDW_DEPOSIT;
    $num_games = $this->user->numGames($last_deposit["time"]);
    if ($num_games > MIN_WDW_GAMES) {
      $credit_limit = 0.00;
    }
    $max_trans_withdrawal = getInDollars(($avail_balance - $credit_limit));

    if (($avail_balance - $amount) < $credit_limit) {
      $this->setError(CC_ERR_EXCEED_MAX_TRANS_WITHDRAWAL);
      return false;
    }
    return true;
  }

  public function doDeposit($amount) {
    $success = false;
    if (!$this->setLock()) {
      $this->text_log("Unable to set LockFile for Deposit Transaction");
      $this->setError(CC_ERR_GENERAL);
      return false;
    }
    if ($this->validateDeposit($amount)) {
      if ($this->hasErrors()) {
        $this->setError(CC_ERR_GENERAL);
      } else {
        $exchrt = $this->getExchangeRate();       //FIXME:: Only valid for EURO/OTHER conversion
        $this->set("amount", $amount); 
        $this->set("ex_amount", ($amount * $exchrt));
        $this->logPendingDeposit();
        try {
          $this->processDeposit();
          $this->logCompleted("completed");
          $success = true;
        } catch (Exception $e) {
          $this->setError(CC_ERR_GENERAL);
          $this->logCompleted("failed");
        }
      }
    }
    $this->unlock();
    return $success;
  }
  public function doWithdrawal($amount) {
    $success = false;

    if (!$this->setLock()) {
      $this->text_log("Unable to set LockFile for Withdrawal Transaction");
      $this->setError(CC_ERR_GENERAL);
      return false;
    }
    if (!$this->validateWithdrawal($amount)) {
      if (!$this->hasErrors()) {
        $this->setError(CC_ERR_GENERAL);
      }
    } else {
      $exchrt = (1 / $this->getExchangeRate());  //FIXME:: Only valid for EURO/OTHER conversion
      $this->set("amount", $amount);
      $this->set("ex_amount", ($amount * $exchrt));
      $this->logPendingWithdrawal();
      try {
        $this->processWithdrawal();
        $this->logCompleted("completed");
        $success = true;
      } catch (Exception $e) {
        $this->setError(CC_ERR_GENERAL);
        $this->logCompleted("failed");
      }
    }
    $this->unlock();
    return $success;
  }

  function hasErrors() { return ($this->errno != 0); }
  function setError($num) { 
    if ($num >= 0 && $num <= CC_NUM_ERRORS)
      $this->errno = $num;
  }
  function getErrorDescription() {
    global $cc_errors;
    return $cc_errors[$this->errno];
  }
  protected function logPendingDeposit()
  {
    global $dbh;
    $this->set("rtn", $dbh->nextSequence('seq_wmc_deposit_txn_num'));
    $this->set("tid", $dbh->nextSequence('seq_ctr_id'));
    $amount = $this->get("ex_amount");

    $sql    = "INSERT INTO customer_transaction (ctr_id, ctr_time, ctr_amount, ctr_pun_id" .
              ",ctr_status, ctr_transaction_num, ctr_pme_code, ctr_tty_id, ctr_wre_number)" .
              " VALUES (" . $this->get("tid") . ", CURRENT_TIMESTAMP," . $amount .", ".$this->get("pun_id"). "," .
              " 'pending', ".$this->get("rtn").", '".$this->getTypeAsString()."', 1, '" . $this->get("cardnumber"). "')";
    $dbh->exec($sql);
  }

  protected function logPendingWithdrawal()
  {
    global $dbh;
    $this->set("rtn", $dbh->nextSequence('seq_wmc_wdwl_txn_num'));
    $this->set("tid", $dbh->nextSequence('seq_ctr_id'));
    $amount = $this->get("amount");
    $sql    = "INSERT INTO customer_transaction (ctr_id,ctr_time,ctr_amount,ctr_pun_id" .
              ",ctr_status, ctr_transaction_num, ctr_pme_code, ctr_tty_id, ctr_wre_number)" .
              " VALUES (" . $this->get("tid") . ", CURRENT_TIMESTAMP," . -$amount .", " . $this->get("pun_id") . "," .
              " 'pending', ".$this->get("rtn").", '".$this->getTypeAsString()."', 2, '".$this->get("cardnumber")."')";
    $dbh->exec($sql);
  }

  protected function logCompleted($status)
  {
    global $dbh;
    $sql  = "UPDATE customer_transaction " .
            " SET ctr_status = " . $dbh->prepareString($status) .
            ",     ctr_settle_track_num =  " . $dbh->prepareString($this->get("ecro")) .
            ",     ctr_notes =  " . $dbh->prepareString($this->get("response_log")) .
            " WHERE ctr_id = " . $this->get("tid");
    $dbh->exec($sql);
  }
  function setLock() {
    $this->lockfile = LOG_DIR .'/cc_lockfile';
    $this->lock = fopen($this->lockfile, "w+");
    if (!$this->lock || !flock($this->lock, LOCK_EX|LOCK_NB,$eWouldBlock) || $eWouldBlock) {
        fclose($this->lock);
        unlink($this->lockfile);
        $this->lock = NULL;
        return FALSE;
    }
    ignore_user_abort(true);
    return TRUE;
  }
  function unLock() {
    if ($this->lock) {
      flock($this->lock, LOCK_UN);
      fclose($this->lock);
      unlink($this->lockfile);
      $this->lock = NULL;
      $this->lockfile= NULL;
      ignore_user_abort(false);
    }
  }
  function text_log($log_str)
  {
    $f = fopen(LOG_DIR.'/cc_log', 'a');
    fputs($f, date('Y-m-d H:i:s').' (' .$this->getTypeAsString().') '. ereg_replace("\r", "", $log_str) . "\n");
    fclose($f);
  }
  abstract function getTypeAsString();
  abstract function processDeposit();
  abstract function processWithdrawal();
  abstract function getExchangeRate();
}


}

?>
