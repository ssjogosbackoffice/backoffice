<?php
define('CC_ERR_SUCCESS',                     0);
define('CC_ERR_MISSING_CARD_NUMBER',         1);
define('CC_ERR_MISSING_AMOUNT',              2);
define('CC_ERR_BAD_FMT_AMOUNT',              3);
define('CC_ERR_MISSING_PASSCODE',            4);
define('CC_ERR_CURL_ERROR',                  5);
define('CC_ERR_INVALID_CARD_NUMBER',         6);
define('CC_ERR_GENERAL',                     7);
define('CC_ERR_INSUFFICIENT_FUNDS',          8);
define('CC_ERR_UNDER_MIN_TRANS_DEPOSIT',     9);
define('CC_ERR_EXCEED_MAX_TRANS_DEPOSIT',    10);
define('CC_ERR_UNDER_MIN_TRANS_WITHDRAWAL',  11);
define('CC_ERR_EXCEED_MAX_TRANS_WITHDRAWAL', 12);
define('CC_ERR_INVALID_USER',                 13);

define('CC_NUM_ERRORS',                      13);

define('CC_MIN_DEPOSIT', 30);
define('CC_MAX_DEPOSIT', 2000);
define('CC_MIN_WITHDRAWAL', 30);
define('CC_MAX_WITHDRAWAL', 1000);


$min_trans_deposit      = getInDollars(CC_MIN_DEPOSIT);
$max_trans_deposit      = getInDollars(CC_MAX_DEPOSIT);
$min_trans_withdrawal   = getInDollars(CC_MIN_WITHDRAWAL);
$max_trans_withdrawal   = getInDollars(CC_MAX_WITHDRAWAL);


$cc_errors[CC_ERR_SUCCESS] = "Success";
$cc_errors[CC_ERR_MISSING_CARD_NUMBER] = "<content id=875 type=php>";
$cc_errors[CC_ERR_MISSING_AMOUNT] = "<content id=876 type=php>";
$cc_errors[CC_ERR_BAD_FMT_AMOUNT] = "<content id=877 type=php>";
$cc_errors[CC_ERR_MISSING_PASSCODE] = "<content id=878 type=php>";
$cc_errors[CC_ERR_CURL_ERROR] = "<content id=879 type=php>";
$cc_errors[CC_ERR_INVALID_CARD_NUMBER] = "<content id=895 type=php>";
$cc_errors[CC_ERR_GENERAL] = "<content id=894 type=php>";
$cc_errors[CC_ERR_INSUFFICIENT_FUNDS] = "<content id=896 type=php>";
$cc_errors[CC_ERR_UNDER_MIN_TRANS_DEPOSIT] = "<content id=2920 type=php min=$min_trans_deposit>";
$cc_errors[CC_ERR_EXCEED_MAX_TRANS_DEPOSIT] = "<content id=1094 type=php max=$max_trans_deposit>";
$cc_errors[CC_ERR_UNDER_MIN_TRANS_WITHDRAWAL] = "<content id=2920 type=php min=$min_trans_withdrawal>";
$cc_errors[CC_ERR_EXCEED_MAX_TRANS_WITHDRAWAL] = "<content id=1094 type=php max=$max_trans_withdrawal>";
$cc_errors[CC_ERR_INVALID_USER] = "<content id=2921 type=php>";

?>
