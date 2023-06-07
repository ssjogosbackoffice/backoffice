<?php

if ( ! class_exists('HTTPRequest')) {

class HTTPRequest extends Properties {
  private $errors;

  function __construct() {
    $this->errors = array();
    $this->buildVars();
  }
  
  function __destruct() {
  }

  static function getInstance() {
    return new HTTPRequest();
  }

  function addError($text, $fatal = 0) {
    if ($fatal) {
      die($text);
    } else {
      $this->errors[] = $text;
    }
  }

  function hasErrors() { return (count($this->errors) != 0); }

  function buildVars() {
    if (isset($_GET) && is_array($_GET)) {
      foreach ($_GET as $key => $value) {
/*
        if ((preg_match('#[^a-zA-Z0-9\.\-/=\_]+#i', $value)) ||
            (preg_match('#\.\.?/#', $value)) ||
              (eregi("\"", $value))) {
                  $this->addError("Invalid characters in GET REQUEST", true);
        } else {
*/
          $this->set($key, $value);
//        }
      }
    }
    $uservars = array();
    if (isset($_POST) && is_array($_POST)) {
      $uservars = $_POST;
    }
    if (isset($_COOKIE) && is_array($_COOKIE)) {
      $uservars = array_merge($uservars, $_COOKIE);
    }
    if (basename($_SERVER["SCRIPT_NAME"]) != "process.html") {
      foreach ($uservars as $seckey => $secvalue) {
        if ((eregi("<[^>][*script*|*object*|*iframe*|*applet*|*meta*|*style*|*form*|*img*]\"?[^>]*>", $secvalue)) ||
            (preg_match('#<[\/\!]*?[^<>]*?>#si', $secvalue)) || 
            (eregi("\([^>]*\"?[^)]*\)", $secvalue))) { 
          $this->addError("Invalid characters in POST REQUEST", true);
        } else {
          $uservars[$seckey] = addslashes($uservars[$seckey]);
          $this->set($seckey,$uservars[$seckey]);
        }
      }
    }
    if (isset($_FILES) && is_array($_FILES)) {
      foreach ($_FILES as $seckey => $secvalue) {
        $_FILES[$seckey]["name"] = preg_replace("#[^A-Za-z0-9\-\.]#", "-", $_FILES[$seckey]["name"]);
        $this->set($seckey, $_FILES[$seckey]);
      }
    }

    if ( ereg("php\.exe", $_SERVER["PHP_SELF"]) || ereg("php3\.cgi", $_SERVER["PHP_SELF"]) || 
         ereg("phpts\.exe", $_SERVER["PHP_SELF"]) ) {
         $docLocation = getenv("PATH_INFO");
    } else {
         $docLocation = $_SERVER["PHP_SELF"];
    }
  }
  
  function getPageName($url = "") {
    $page = ( $url ? $url : $_SERVER["SCRIPT_NAME"]); //defaults to current script	 
    $arr = explode("/", $page);
    $pg_with_query_str = $arr[count($arr)-1];
    $page_name = strtok($pg_with_query_str, '?');
    return $page_name;
  }
  
  function getBrowserName() {
    if ( strstr(strtoupper($_SERVER["HTTP_USER_AGENT"]), "MSIE") )
      return "ie";
    else
      return "mozilla";
  }
  
  function getRequestMethod() {
    if ( count($_POST) > 0 )
      return "post";
    elseif ( count($_GET) > 0 )
      return "get";
    return '';
  }

  function thisDirName () {
    $arr = explode("/", $_SERVER["SCRIPT_NAME"]);
    if ( count($arr) < 3 ) //means no parent dir below root
      return;
    return $arr[1];
  }

}

}
?>
