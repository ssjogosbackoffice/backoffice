<?php
if ( ! class_exists('WMCCard')) {

include_once("CreditCard.class.inc");

define('WMC_GATEWAY_USERNAME', 'egc');
define('WMC_GATEWAY_PASSWORD', '12345');
define('WMC_GATEWAY_TRANSACTION_URL', 'https://secure.cinquepuntocinque.com/paygate/partners/egc/transaction.php');
define('WMC_DTD_URL', 'https://secure.cinquepuntocinque.com/paygate/partners/Partners.dtd');
define('WMC_DTD_NAME', 'transaction');

define('WMC_TRANS_STATUS_SUSPENDED', 1);
define('WMC_TRANS_STATUS_FAILED',    2);
define('WMC_TRANS_STATUS_VALIDATED', 3);
define('WMC_TRANS_STATUS_PENDING',   4);

class WMCCard extends CreditCard {
  protected function &parse_response(&$response)
  {
  }
  protected function sendRequest($url, &$dom, $callback) {
    $data = $dom->saveXML();
    $response = https_post(WMC_GATEWAY_TRANSACTION_URL, $data, $timeout=60, $vfy_host=false, $vfy_peer=false,
                              WMC_GATEWAY_USERNAME, WMC_GATEWAY_PASSWORD, $debug = true);
    if ($response < 0) {
      return false;
    } else {
      return call_user_func($callback, $response);
    }
  }

  private function processBankTransaction(&$xml_data) {
    $dom = new DomDocument('1.0', 'UTF-8');
    if (!$dom->loadXML(trim($xml_data)))
      return false;
    $xdom = new DOMXPath($dom);
    $status = $xdom->query("/transaction/Response/Status/StatusId")->item(0)->textContent;
    $statusdesc = $xdom->query("/transaction/Response/Status/StatusDesc")->item(0)->textContent;
    if ($status != 0)
      return false;
    $ecro = $xdom->query("/transaction/Response/Status/RefUID")->item(0)->textContent;
    $this->set("ecro", $ecro);
    return true;
  }

  private function createDeposit(&$dom, &$n_servicesrq) {
    $amount = $this->get("amount");
    settype($amount, "float");
    $n_debitaddrq= $dom->createElement('DebitAddRq');
    $n_debitaddrq = $n_servicesrq->appendChild($n_debitaddrq);
    $n_rquid = $dom->createElement('RqUID', $this->get("rtn"));
    $n_rquid = $n_debitaddrq->appendChild($n_rquid);
    $n_creditcard = $dom->createElement('CreditCard', $this->get("cardnumber"));
    $n_creditcard = $n_debitaddrq->appendChild($n_creditcard);
    $n_passcode = $dom->createElement('PassCode', $this->get("passcode"));
    $n_passcode = $n_debitaddrq->appendChild($n_passcode);
    $n_cardtype = $dom->createElement('CardType', 'WMC');
    $n_cardtype = $n_debitaddrq->appendChild($n_cardtype);
    $n_amount = $dom->createElement('Amount', sprintf("%.02f", $amount));
    $n_amount = $n_debitaddrq->appendChild($n_amount);
  }
  private function createWithdrawal(&$dom, &$n_servicesrq) {
    $amount = $this->get("amount");
    settype($amount, "float");
    $n_creditaddrq= $dom->createElement('CreditAddRq');
    $n_creditaddrq = $n_servicesrq->appendChild($n_creditaddrq);
    $n_rquid = $dom->createElement('RqUID', $this->get("rtn"));
    $n_rquid = $n_creditaddrq->appendChild($n_rquid);
    $n_creditcard = $dom->createElement('CreditCard', $this->get("cardnumber"));
    $n_creditcard = $n_creditaddrq->appendChild($n_creditcard);
    $n_passcode = $dom->createElement('PassCode', $this->get("passcode"));
    $n_passcode = $n_creditaddrq->appendChild($n_passcode);
    $n_cardtype = $dom->createElement('CardType', 'WMC');
    $n_cardtype = $n_creditaddrq->appendChild($n_cardtype);
    $n_amount = $dom->createElement('Amount', sprintf("%.02f", $amount));
    $n_amount = $n_creditaddrq->appendChild($n_amount);
  }
  private function createListTransaction(&$dom, &$n_servicesrq) {
  }

  function & createDocument($callback) {
    $imp = new DOMImplementation();
    $dtd = $imp->createDocumentType(ITC_DTD_NAME, '', ITC_DTD_URL);
    $dom = $imp->createDocument('','',$dtd);
    $dom->version = '1.0';
    $dom->encoding = 'UTF-8';
    $dom->standalone = false;
    //$dom = new DomDocument('1.0', 'UTF-8');
    $dom->preserveWhiteSpace = false;
    $dom->formatOutput = true;
    $n_transaction = $dom->createElement('transaction');
    $n_transaction = $dom->appendChild($n_transaction);
    $n_request = $dom->createElement('Request');
    $n_request = $n_transaction->appendChild($n_request);
    $n_signonrq = $dom->createElement('SignonRq');
    $n_signonrq = $n_request->appendChild($n_signonrq);
    $n_partnerid = $dom->createElement('PartnerId', WMC_GATEWAY_USERNAME);
    $n_partnerid = $n_signonrq->appendChild($n_partnerid);
    $n_partnerpwd = $dom->createElement('PartnerPwd', WMC_GATEWAY_PASSWORD);
    $n_partnerpwd = $n_signonrq->appendChild($n_partnerpwd);
    $n_partnerdt = $dom->createElement('PartnerDt', strftime("%Y%m%d%H%M%S"));
    $n_partnerdt = $n_signonrq->appendChild($n_partnerdt);
    $n_crypttype = $dom->createElement('CryptType', "None");
    $n_crypttype = $n_signonrq->appendChild($n_crypttype);
    $n_servicesrq = $dom->createElement('ServicesRq');
    $n_servicesrq = $n_request->appendChild($n_servicesrq);
    call_user_func($callback, $dom, $n_servicesrq);
    $n_signoffrq = $dom->createElement('SignoffRq');
    $n_signoffrq = $n_request->appendChild($n_signoffrq);
    $n_spartnerid = $dom->createElement('PartnerId', WMC_GATEWAY_USERNAME);
    $n_spartnerid = $n_signoffrq->appendChild($n_spartnerid);
    return $dom;
  }
  function processDeposit() {
    $dom = $this->createDocument(array(&$this, "createDeposit"));
    if ($this->sendRequest(WMC_GATEWAY_TRANSACTION_URL, $dom, array(&$this, "processBankTransaction"))) {
      $this->user->doDeposit($this->get("amount"));
    } else {
      throw new Exception("Transaction Error");
    }
  }
  function processWithdrawal() {
    $dom = $this->createDocument(array(&$this, "createWithdrawal"));
    if ($this->sendRequest(WMC_CREDIT_TRANSACTION_URL, $dom, array(&$this, "processBankTransaction"))) {
      $this->user->doWithdrawal($this->get("cur_amount"));
    } else {
      throw new Exception("Transaction Error");
    }
  }
  function getTypeAsString() { return "WMC"; }
  function getExchangeRate() {
    return 1.00;
  }
  function validateDeposit(&$amount) {
    $passcode = $this->get("passcode");
    if (empty($passcode)) {
      $this->setError(CC_ERR_MISSING_PASSCODE);
      return false;
    }
    return parent::validateDeposit($amount);
  }
  function validateWithdrawal(&$amount) {
    $passcode = $this->get("passcode");
    if (empty($passcode)) {
      $this->setError(CC_ERR_MISSING_PASSCODE);
      return false;
    }
    return parent::validateWithdrawal($amount);
  }
}

}

?>
