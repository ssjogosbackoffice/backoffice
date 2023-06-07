<?php

if ( ! class_exists('CreditCards')) {

include_once("ITCCard.class.inc");
include_once("WMCCard.class.inc");
include_once("MBMCard.class.inc");


class CreditCards {
  function & getInstance($user, $card_number) {
    $ccv = new CreditCardValidator($card_number);
    if (!$ccv->isValid()) {
      return NULL;
    }
    if ($ccv->getType() == CCTYPE_WMC) {
      return new WMCCard($user, $card_number);
    } else if ($ccv->getType() == CCTYPE_MBM) {
      return new MBMCard($user, $card_number);
    } else if ($ccv->getType() == CCTYPE_ITC) {
      return new ITCCard($user, $card_number);
    } else {
      return NULL;
    }
  }
  function isValidMoney($money) {
    $pattern = "/^[0-9]*(\.{1,1}[0-9]{1,2}){0,1}$/";
    return preg_match($pattern, $money);
  }
  function isBlank($field) {
    return mb_ereg_match("^\s*$", $field);
  }
}


}

?>
