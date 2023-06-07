<?php
define('NAMESDB_FILE', realpath(dirname(__FILE__))."/../names.db");
$pasctab = array(
     0x0,  0x1,  0x2,  0x3,  0x4,  0x5,  0x6,  0x7,  0x8,  0x9,
     0xa,  0xb,  0xc,  0xd,  0xe,  0xf, 0x10, 0x11, 0x12, 0x13,
    0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
    0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x4f, 0x49,
    0x32, 0x45, 0x41, 0x53, 0x36, 0x54, 0x38, 0x39, 0x3a, 0x3b,
    0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45,
    0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
    0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x41, 0x42, 0x43,
    0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d,
    0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
    0x58, 0x59, 0x5a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81,
    0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b,
    0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95,
    0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9,
    0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3,
    0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd,
    0xbe, 0xbf, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x43,
    0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49, 0x45, 0x4e,
    0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0xd7, 0x4f, 0x55, 0x55, 0x55,
    0x55, 0x59, 0x54, 0x73, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
    0x61, 0x63, 0x65, 0x65, 0x65, 0x65, 0x69, 0x69, 0x69, 0x69,
    0x65, 0x6e, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0xf7, 0x6f, 0x75,
    0x75, 0x75, 0x75, 0x79, 0x74, 0x79
);


$sctab = array(
    'À' => 'A', 'Â' => 'A', 'Ä' => 'A',  'Æ' => 'AE', 'È' => 'E', 'Ê' => 'E', 'Ì' => 'I', 'Î' => 'I',
    'Ð' => 'D', 'Ò' => 'O', 'Ô' => 'O',  'Ö' => 'O',  'Ø' => 'O', 'Ú' => 'U', 'Ü' => 'U', 'à' => 'a',
    'â' => 'a', 'ä' => 'a', 'æ' => 'ae', 'è' => 'e',  'ê' => 'e', 'ì' => 'i', 'î' => 'i', 'ð' => 'o',
    'ò' => 'o', 'ô' => 'o', 'ö' => 'o',  'ø' => 'o',  'ú' => 'u', 'ü' => 'u', 'Á' => 'A', 'Ã' => 'A',
    'Å' => 'A', 'Ç' => 'C', 'É' => 'E',  'Ë' => 'E',  'Í' => 'I', 'Ï' => 'I', 'Ñ' => 'N', 'Ó' => 'O',
    'Õ' => 'O', 'Ù' => 'U', 'Û' => 'U',  'Ý' => 'Y',  'ß' => 'B', 'á' => 'a', 'ã' => 'a', 'å' => 'a',
    'ç' => 'c', 'é' => 'e', 'ë' => 'e',  'í' => 'i',  'ï' => 'i', 'ñ' => 'n', 'ó' => 'o', 'õ' => 'o',
    'ù' => 'u', 'û' => 'u', 'ý' => 'y',  'ÿ' => 'y',  '@' => '@', ' ' => '_', '[' => '_', ']' => '_',
    '(' => '_', ')' => '_', '&' => '_',  '+' => '_',  '=' => '_'
);

$CONV_SOURCE = "`1234567890-=\\qwertyuiop[]asdfghjkl;'zxcvbnm,./~!@#$%^&*()_+|QWERTYUIOP{}ASDFGHJKL:\"ZXCVBNM<>?";
$CONV_SHIFT  = "~!@#$%^&*()_+|QWERTYUIOP{}ASDFGHJKL:\"ZXCVBNM<>?`1234567890-=\\qwertyuiop[]asdfghjkl;'zxcvbnm,./";
$CONV_INVERT = "`1234567890-=\\QWERTYUIOP[]ASDFGHJKL;'ZXCVBNM,./~!@#$%^&*()_+|qwertyuiop{}asdfghjkl:\"zxcvbnm<>?";
$CONV_VOWELS = "`1234567890-=\\QWeRTYuioP[]aSDFGHJKL;'ZXCVBNM,./~!@#$%^&*()_+|QWeRTYuioP{}aSDFGHJKL:\"ZXCVBNM<>?";
$CONV_RIGHT  = "1234567890-=\\\\wertyuiop[]]sdfghjkl;''xcvbnm,./\\!@#$%^&*()_+||WERTYUIOP{}}SDFGHJKL:\"\"XCVBNM<>?|";
$CONV_LEFT   = "``1234567890-=qqwertyuiop[aasdfghjkl;zzxcvbnm,.~~!@#$%^&*()_+QQWERTYUIOP{AASDFGHJKL:ZZXCVBNM<>";
$CHARS_LOWER = "abcdefghijklmnopqrstuvwxyz";
$CHARS_UPPER = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";



/********* INITIALIZATION ******/
function rules_init($str, $convert_table) {
  $conv = array();
  for ($i = 0; $i < 0x100; $i++) {
    $conv[$i] = chr($i);
  }
  for ($i = 0; $i < strlen($str); $i++) {
    $pos = ord($str{$i});
    $conv[$pos] = $convert_table{$i};
  }
  return $conv;
}

function rules_apply($str, &$table) {
  $out = "";
  for ($i = 0; $i < strlen($str) ;$i++) {
    $pos  = ord($str{$i});
    $out .= $table[$pos];
  }
  return $out;
}


/******** UTIL FUNCS ********/
function startsWith( $string, $substring, $case_sensitive=true ) {
  if ( ! $case_sensitive ) {
    $string = strtolower( $string );
    $substring = strtolower( $substring );
  }
  return ( substr( $string, 0, strlen( $substring ) ) == $substring );
}

function endsWith( $string, $substring, $case_sensitive=true ) {
  if ( ! $case_sensitive ) {
    $string = strtolower( $string );
    $substring = strtolower( $substring );
  }
  return ( substr( $string, -strlen( $substring ) ) == $substring );
}

function strtolower_keepaccents($txt) {
  return strtolower(strtr($txt, "ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝÞ", "àáâãäåæçèéêëìíîïðñòóôõöøùúûüýþ"));
}

function strip_accents($pass) {
  global $sctab;
  return str_replace(array_keys($sctab), $sctab, $pass);
}

function decode_word($word) {
  global $pasctab;
  $ret = "";
  for ($i = 0; $i < strlen($word); $i++) {
    $ret .= chr($pasctab[ord($word{$i})]);
  }
  return $ret;
}

function similar_word($word, $against) {
  similar_text(strtolower($word), strtolower($against), $percent);
  return $percent;
}

$current_try  = "";
$tried_passwd = array();

function setCurrentTry($pass) {
  global $current_try;
  global $tried_passwd;
  $current_try = $pass;
  array_push($tried_passwd, $pass);
}

function dumpTriedPasswords() {
  global $tried_passwd;
  $i = 1;
  $buf = "";
  foreach ($tried_passwd as $password) {
    $buf .= "$password, ";
    if (($i % 5) == 0) {
      $i = 0;
      $buf .= "\n";
    }
    $i++;
  }
  $buf .= "\n";
}

function abort($msg) {
  global $current_try;
  print "$msg\n";
  print "Current try: $current_try\n";
  die();
}



/*****************
 *  TESTS CALLBACK
 */
function check_length($user, $pass) {
  $passlen = strlen($pass);
  if ($passlen < 6) {
      $ret = "Your password must be at least 6 characters long"; }
  elseif ($passlen >= 9) { $ret = 1; }
  else {
    $ret = 1;
  }
  return $ret;
}


function check_strength($user, $pass) {
  $ret = 1;
  if (preg_match('/[$!@#%\*-]/', $pass)) {
    $ret += 2;
  }
  if (preg_match('/^\w*(?=\w*\d)(?=\w*[a-z])(?=\w*[A-Z])\w*$/', $pass)) {
    $ret += 2;
  }
  $nm = preg_match_all('#[0-9]#', $pass, $m);
  if ($nm > 1) {
    $ret += 1;
  }
  return $ret;

/*
  if (!preg_match('/[$!@#%\*-]/', $pass) &&
      !preg_match('/^\w*(?=\w*\d)(?=\w*[a-z])(?=\w*[A-Z])\w*$/', $pass)) {
  }
*/
}

$cached_strings = array();

function check_strings_combs($user, $pass) {
  global  $cached_strings;
//  global $pasctab;


  if (array_key_exists($pass, $cached_strings)) {
    return $cached_strings[$pass];
  }

  $ulen = strlen($user);
  $plen = strlen($pass);
  $ret  = 5;

/*
  $usr  = "";
  $pwd  = "";

  for ($i = 0; $i < strlen($user); $i++) {
    $usr .= chr($pasctab[ord($user{$i})]);
  }

  for ($tmp = 0, $x = 0; $x < strlen($pass); $x++) {
    $ordchar = ord($pass{$x});
    if ((($ordchar > 33)  && ($ordchar < 65)) ||
        (($ordchar > 90)  && ($ordchar < 97)) ||
        (($ordchar > 122) && ($ordchar < 127))) {
      $tmp++;
    }
    $pwd .= chr($pasctab[$ordchar]);
  }
*/

  $usr = decode_word($user);
  $pwd = decode_word($pass);

  setCurrentTry($pwd);

/*
  if ($tmp == 0) {

    $ret--;
  }
*/

  if ((($ulen >  $plen) && (strstr($usr, $pwd) != NULL)) ||
      (($plen >  $ulen) && (strstr($pwd, $usr) != NULL)) ||
      (($plen == $ulen) && (strcmp($usr, $pwd) == 0))) {
    $ret = 1;
  }

  if (similar_word($usr, $pwd) > 75) {
    $ret--;
  }

  if (soundex($usr) == soundex($pwd)) {
    $ret--;
  }

  if (levenshtein($usr, $pwd) < ($ulen / 2)) {
    $ret--;
  }

  if (levenshtein(metaphone($usr), metaphone($pwd)) < 2) {
    $ret--;
  }

  $rusr = strrev($usr);
  if ((($ulen >  $plen) && (strstr($rusr, $pwd) != NULL)) ||
      (($plen >  $ulen) && (strstr($pwd,  $rusr) != NULL)) ||
      (($plen == $ulen) && (strcmp($rusr, $pwd) == 0))) {
    $ret -= 3;
  }
  if (similar_word($rusr, $pwd) > 75) {
    $ret--;
  }

  $toCheck = $usr . $rusr;
  if (strstr($toCheck, $pwd)) {
    $ret -= 3;
  }
  $toCheck = $usr . $usr;
  if (strstr($toCheck, $pwd)) {
    $ret -= 3;
  }
  $ret = (($ret <= 0) ? 1 : $ret);
  $cached_strings[$pass] = $ret;
  return $ret;
}

function check_dict($user, $pass) {
   if(defined('CHECK_SQL_LITE_PASS')){
    $ret = 3;


  $pwlen = strlen($pass);

  if     ($pwlen < 5) { return 1; }
  elseif ($pwlen > 9) { return 4; }
  elseif (preg_match('#[0-9]#', $pass)) { return 1; }

  setCurrentTry($pass);
  $db = sqlite_open(NAMESDB_FILE, 444, $err);
  if ($err) {
    die("ERROR: $err\n");
  }

  sqlite_create_function($db, "similar_word", 'similar_word', 2);

  $q  = "SELECT psn_name FROM person_names WHERE similar_word(psn_name, '".sqlite_escape_string($pass)."') > 80";
  $rs = sqlite_query($db, $q);
  if (sqlite_num_rows($rs) > 2) {
    $ret = 1;
  }
  else {
    $ret++;
    $q  = "SELECT psn_surname FROM person_surnames WHERE similar_word(psn_surname, '".sqlite_escape_string($pass)."') > 80";
    $rs = sqlite_query($db, $q);
    if (sqlite_num_rows($rs) > 2) {
      $ret--;
    }
    else {
      $ret++;
    }
  }

/*
  $rows = sqlite_array_query($db, $q);
  foreach ($rows as $res) {
    print $res["psn_surname"] . "\n";
  } */

  sqlite_close($db);
  return $ret;
   }
    else{
        return 4;
    }

}

function check_known_passwd($user, $pass) {
  $ret = 3;

  $pwlen = strlen($pass);

  if ($pwlen < 5)      { return 1; }
  elseif ($pwlen >= 9) { return 4; }

  setCurrentTry($pass);
  $db = sqlite_open(NAMESDB_FILE, 444, $err);
  if ($err) {
    die("ERROR: $err\n");
  }

  $q  = "SELECT str_passwd FROM known_passwords WHERE str_passwd='".sqlite_escape_string($pass)."'";
  $rs = sqlite_query($db, $q);
  if (sqlite_num_rows($rs) > 0) {
    $ret = 1;
  }
  else {
    $ret = 3;
  }
  sqlite_close($db);
  return $ret;
}



function check_date($user, $pass) {
  $ret = 5;
  if (strtotime($pass) !== FALSE) {
    $ret = 0;
  }
  else {
    $rx = '\b(0?[1-9]|[12][0-9]|3[01])[- /.]?(0?[1-9]|1[012])[- /.]?(19|20)?[0-9]{2}\b';
    if (preg_match("#$rx#", $pass)) {
      $ret = 0;
    }
    else {
      $rx = '\b(19|20)?[0-9]{2}[- /.]?(0?[1-9]|1[012])[- /.]?(0?[1-9]|[12][0-9]|3[01])\b';
      if (preg_match("#$rx#", $pass)) {
        $ret = 0;
      }
      else {
        $rx = '(0?[1-9]|[12][0-9]|3[01])[- /.]?(0?[1-9]|1[012])[- /.]?(19|20)?[0-9]{2}';
        if (preg_match("#$rx#", $pass)) {
          $ret = 0;
        }
        else {
          $rx = '(19|20)?[0-9]{2}[- /.]?(0?[1-9]|1[012])[- /.]?(0?[1-9]|[12][0-9]|3[01])';
          if (preg_match("#$rx#", $pass)) {
            $ret = 0;
          }
        }
      }
    }
  }
  return (($ret <= 0) ? 0 : $ret);
}

function check_creditcard($user, $pass) {
  $rx = '^(?:4[0-9]{12}(?:[0-9]{3})?|5[1-5][0-9]{14}|6011[0-9]{12}|3(?:0[0-5]|[68][0-9])[0-9]{11}|3[47][0-9]{13})$';
  if (preg_match("#$rx#", $pass)) {
    return 2;
  }
  return 5;
}

function check_email($user, $pass) {
  $rx = '^[A-Z0-9._%-]+@[A-Z0-9.-]+\.[A-Z]{2,4}$';
  if (preg_match("#$rx#i", $pass)) {
      return "The password should not be an email address";
  }
    return 4;
}


function check_number($user,$pass){
    if( preg_match('/\d/',          $pass)) {
        return 1;
    }
    return "Your password must contain at least one number.";

}

function check_capitalize($user,$pass){
    if (preg_match("/[A-Z]/", $pass)) {
        return 1;
    }
    return "Your password must contain at least one capital letter.";

}

function check_special($user,$pass){
    if (preg_match('/[^a-zA-Z\d]/',$pass)) {
        return 1;
    }
    return "Your password must contain at least one special character(ex:@,#,etc)";

}






$user = $_SERVER["argv"][1];
$pass = $_SERVER["argv"][2];


/****************
 * CORE
 */

function checkpass_init() {
  global $CONV_SOURCE, $CONV_SHIFT, $CONV_INVERT;
  global $CONV_VOWELS, $CONV_RIGHT, $CONV_LEFT;
  global $CONV_LOWER,  $CONV_UPPER;
  global $tbl_source,  $tbl_shift, $tbl_invert;
  global $tbl_vowels,  $tbl_right, $tbl_left;
  global $tbl_lower,   $tbl_upper;
  $tbl_source  = rules_init($CONV_SOURCE, $CONV_SOURCE);
  $tbl_shift   = rules_init($CONV_SOURCE, $CONV_SHIFT);
  $tbl_invert  = rules_init($CONV_SOURCE, $CONV_INVERT);
  $tbl_vowels  = rules_init($CONV_SOURCE, $CONV_VOWELS);
  $tbl_right   = rules_init($CONV_SOURCE, $CONV_RIGHT);
  $tbl_left    = rules_init($CONV_SOURCE, $CONV_LEFT);
  $tbl_lower   = rules_init($CHARS_UPPER, $CHARS_LOWER);
  $tbl_upper   = rules_init($CHARS_LOWER, $CHARS_UPPER);
}



$tests = array(
  "Length"      => array("cb" => "check_length"),
  "Email"       => array("cb" => "check_email"),
  "Date"        => array("cb" => "check_date"),
  "Number"      => array("cb" => "check_number"),
  "Capitalize"  => array("cb" => "check_capitalize"),
  "Special"     => array("cb" => "check_special"),
);


function checkpass_run($user, $pass) {
  global $tests, $results,$lang;
  $error = false;

  $pass = strip_accents($pass);
  foreach ($tests as $name => $data) {
     $xpass = $pass;
      $ret            = call_user_func($data["cb"], $user, $xpass);
      if (!is_numeric($ret)) {
           $error = true;
           addError('',$lang->getLang($ret));
       }
    }
    return ($error==false? true:false);
}

function checkpass_dispose()  {
  dumpTriedPasswords();
}

?>
