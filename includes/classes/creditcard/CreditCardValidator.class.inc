<?php

if ( ! class_exists('CreditCardValidator')) {


/*
 * Credit Card MINLEN and MAXLEN
 */
define('CC_MINLEN', 12);
define('CC_MAXLEN', CC_MINLEN + 16);
/*
 * Credit Card Types Costants
 */
define('CCTYPE_UNKNOWN', 0);
define('CCTYPE_WMC', 1);
define('CCTYPE_ITC', 2);
define('CCTYPE_MBM', 3);

/*
 * Error Costants
 */
define('CCERROR_NOERROR', 0);
define('CCERROR_UNKNOWN', 1);
define('CCERROR_WRONGFORMAT', 2);
define('CCERROR_WRONGLENGTH', 3);
define('CCERROR_CHECKSUM', 4);

/**
 * Credit Card Validator Class.
 * Can validate a credit card number, support most of the
 * existing credit card types.
 * Will check for format, type, expected length and base 10 checksum
 *
 * @since PHP 4.0.0 (3.0?)
 * @author Giuseppe Caulo <giusc@softzone.it>
 * @see http://www.softzone.it
 */
class CreditCardValidator {

  // {{{ properties
  /**
   * The actual type of passed credit card,
   * is one of following defined costants:
   * CCTYPE_UNKNOWN - Unknown type
   * CCTYPE_WMC - WMC Card
   * CCTYPE_ITC - ITC Card
   * CCTYPE_MBM - MBM Card
   *
   * @var int
   * @access private
   */
  var $type;

  /**
   * The actual error number,
   * is one of following defined costants:
   * CCERROR_NOERROR - No error
   * CCERROR_UNKNOWN - Unknown Card type
   * CCERROR_WRONGFORMAT - Format of passed Card is wrong
   * CCERROR_WRONGLENGTH - length doesn't match expected
   * CCERROR_CHECKSUM - Checksum is invalid
   *
   * @var int
   * @access private
   */
  var $err;

  // }}}
  // {{{ constructor

  /**
   * Constructor, register and initialize variables.
   * if the credit card number is passed in arguments performs
   * immediately the check for that number
   *
   * @param string optional credit card number to check
   * @return void
   * @access public
   */
  function CreditCardValidator($ccnum = null) {
    $this->errors = array (
         CCERROR_NOERROR => "No error",
         CCERROR_UNKNOWN => "Unknown Card Type",
         CCERROR_WRONGFORMAT => "Card number has a wrong format",
         CCERROR_WRONGLENGTH => "Card number has a wrong length",
         CCERROR_CHECKSUM => "Checksum is incorrect");

    $this->supportedTypes = &$this->getSupportedTypes();
    if (!empty($ccnum)) {
      $this->validateCard($ccnum);
    } else {
      $this->ccnum = $ccnum;
      $this->type = CCTYPE_UNKNOWN;
      $this->err = CCERROR_UNKNOWN;
    }
  }

  // }}}
  // {{{ L()

  /**
   * Compute and return a bitwise operator
   *
   * @return int the operator
   * @access private
   */
  function L($n) {
    return (1 << (($n) - CC_MINLEN));
  }

  // }}}
  // {{{ getSupportedTypes()

  /**
   * Returns a valid structured list of supported credit card
   *
   * @return array the list of supported credit card
   * @access private
   */
  function getSupportedTypes() {
    return array (
       CCTYPE_ITC => array (
                        "name" => "Intercash",
                        "len" => (CreditCardValidator::L(19)),
                        "start" => array("6220983125"),
                        "checksum" => true),
       CCTYPE_WMC => array (
                        "name" => "WMC",
                        "len" => (CreditCardValidator::L(16)),
                        "start" => array("40808963"),
                        "checksum" => true),
       CCTYPE_MBM => array (
                        "name" => "MBM",
                        "len" => (CreditCardValidator::L(16)),
                        "start" => array("4080896401"),
                        "checksum" => true)
       );
  }

  // }}}
  // {{{ validChecksum()

  /**
   * Compute the checksum in base 10 of the Credit Card
   *
   * @param string $ccnum the credit card number
   * @return bool true if valid, false otherwise
   * @access private
   */
  function validChecksum(&$ccnum) {
    $checksum = 0;
    $dbl = 0;
    $index = strlen($ccnum);
    while ($index--)
    {
      $c = ord($ccnum{$index}) - ord('0');
      if ($dbl) {
        $c *= 2;
        if ($c >= 10)
          $c -= 9;
      }
      $checksum += $c;
      $dbl = !$dbl;
    }
    return (($checksum % 10) == 0);
  }

  // }}}
  // {{{ validFormat()

  /**
   * Strip spaces and separators from the passed Credit Card
   * then check for minimum and maximum length and for
   * numeric input
   *
   * @param string $ccnum the credit card number
   * @return bool true if valid, false otherwise
   * @access private
   */
  function validFormat(&$ccnum) {
    $ccnum = preg_replace('#[ -]#', '', $ccnum);
    return ((preg_match('#^[\d]+$#', $ccnum)) &&
            (strlen($ccnum) > CC_MINLEN) &&
            (strlen($ccnum) < CC_MAXLEN));
  }

  // }}}
  // {{{ _validLength()

  /**
   * Check the Credit Card Number against expectedLength for
   * that type.
   *
   * @param string $ccnum the credit card number
   * @param int $expectedLength the -bitwised- expected length
   * @return bool true if valid, false otherwise
   * @access private
   * @see CreditCardValidator:L
   */
  function _validLength(&$ccnum, &$expectedLength) {
    return ((CreditCardValidator::L(strlen($ccnum)) & $expectedLength) != 0);
  }

  // }}}
  // {{{ _getType()

  /**
   * Get the type for passed credit card number
   *
   * @param string $ccnum the credit card number
   * @param array  $validTypes a valid array containing
   *               a structured list of credit card types
   * @return int the type of the credit card or CCTYPE_UNKNOWN
   * @access private
   */
  function _getType(&$ccnum, &$validTypes) {
    foreach ($validTypes as $type => $prop) {
      foreach ($prop["start"] as $startnum) {
        $startlen = strlen($startnum);
        if (substr($ccnum, 0, $startlen) == $startnum) {
          return $type;
        }
      }
    }
    return CCTYPE_UNKNOWN;
  }

  // }}}
  // {{{ validate()

  /**
   * Validate passed Credit Card Number.
   * Performs all tests step by step and set the correct
   * Object Error Code.
   *
   * @return void
   * @access private
   */
  function _validate() {
    if ($this->validFormat($this->ccnum)) {
      if (($type = $this->_getType($this->ccnum, $this->supportedTypes)) != CCTYPE_UNKNOWN) {
        $this->type = $type;
        if ($this->_validLength($this->ccnum, $this->supportedTypes[$type]["len"])) {
          if ($this->supportedTypes[$type]["checksum"]) {
            if ($this->validChecksum($this->ccnum)) {
              $this->err = CCERROR_NOERROR;
            } else {
              $this->err = CCERROR_CHECKSUM;
            }
          } else {
            $this->err = CCERROR_NOERROR;
          }
        } else {
          $this->err = CCERROR_WRONGLENGTH;
        }
      } else {
        $this->err = CCERROR_UNKNOWN;
      }
    } else {
      $this->err = CCERROR_WRONGFORMAT;
    }
  }

  // }}}
  // {{{ validateCard()

  /**
   * This function actually do the work of validating
   * a credit card.
   * Resets errors and types then performs full checks
   *
   * Usage example:
   *
   * $ccvalidator = new CreditCardValidator();
   * $ccvalidator->validateCard($creditCardNumber)
   * if ($ccvalidator->isValid())
   *   print "Valid";
   * else
   *   print "Invalid";
   *
   * @param string $ccnum the credit card number to verify
   * @return void
   * @access public
   */
  function validateCard($ccnum) {
    $this->ccnum = $ccnum;
    $this->type = CCTYPE_UNKNOWN;
    $this->err = CCERROR_UNKNOWN;
    $this->_validate();
  }

  // }}}
  // {{{ isValid()

  /**
   * Return wether the processed CC resulted valid
   *
   * Usage example:
   *
   * $ccvalidator = new CreditCardValidator($creditCardNumber);
   * if ($ccvalidator->isValid())
   *   print "Valid";
   * else
   *   print "Invalid";
   *
   * @return bool true if processed CC is valid false otherwise
   * @access public
   * @see CreditCardValidator::validate
   */
  function isValid() {
    return (($this->type != CCTYPE_UNKNOWN) &&
            ($this->err == CCERROR_NOERROR));
  }

  // }}}
  // {{{ getType()

  /**
   * Returns the type number of the current CC
   *
   * @return int
   * @access public
   */
  function getType() {
    return $this->type;
  }

  // }}}
  // {{{ getTypeName()

  /**
   * Returns the type name of the current CC
   *
   * @return string
   * @access public
   */
  function getTypeName() {
    return $this->supportedTypes[$this->type]["name"];
  }

  // }}}
  // {{{ getErr()

  /**
   * Returns error number for the current CC
   *
   * @return int
   * @access public
   */
  function getErr() {
    return $this->err;
  }

  /**
   * Returns error string for the current CC
   *
   * @return string
   * @access public
   */
  function getErrName() {
    return $this->errors[$this->err];
  }

  // }}}
  // {{{ toString()

  /**
   * Renders object with a string representation
   *
   * @return string
   * @access public
   */
  function toString() {
    return '
    Credit Card Number: '. $this->ccnum . '
    Credit Card Type: '. $this->getTypeName() .'
    Credit Card isValid: '. (($this->isValid()) ? "Yes" : "No") . '
    Error Code : ' . $this->getErr() . ' ('.$this->getErrName().')
    ';
  }

  // }}}
  // {{{ validCC()

  /**
   * Static Method to check CC Validity.
   * Doesn't return anything about CC properties.
   *
   * Usage Example:
   * if (CreditCardValidator::validCC($creditCardNumber)) {
   *   print "Valid";
   * else
   *   print "Not Valid";
   *
   * @param string $ccnum The Credit Card number to verify
   * @return bool true if $ccnum is a valid CC number false otherwise
   * @access public
   */
  function validCC($ccnum) {
    $supportedTypes = &CreditCardValidator::getSupportedTypes();
    if (CreditCardValidator::validFormat($ccnum)) {
      if (($type = CreditCardValidator::_getType($ccnum, $supportedTypes)) != CCTYPE_UNKNOWN) {
        if (CreditCardValidator::_validLength($ccnum, $supportedTypes[$type]["len"])) {
          if ($supportedTypes[$type]["checksum"])
            return CreditCardValidator::validChecksum($ccnum);
          else
            return true;
        }
      }
    }
    return false;
  }
  // }}}
}


}

/*
 * Local Variables:
 * mode: php
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
?>

