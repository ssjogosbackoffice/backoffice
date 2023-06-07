<?php

if ( ! class_exists('Object')) {

interface Application {
  function equals($obj);
  function className();
  function toString();
}

abstract class Object implements Application {
  function equals($obj) {
    if (is_object($obj)) {
      if (is_a($obj, $this->className())) {
        return true;
      }
    }
    return false;
  }
  function className() {
    return get_class($this);
  }
  static function getInstance($class = __CLASS__) {
    $obj = new $class();
    return $obj;
  }
  function toString() {
    return "[".get_class($this)."]";
  }
}

abstract class ObjectList extends Object {
  private $CurrentIndex;
  private $ObjectArray;
  private $ObjPtr;
  function __construct() {
    $this->ObjectArray = array();
    $this->reset();
  }
  function __destruct() {
    $this->clear();
  }
  function add($obj) {
    $this->ObjectArray[] = $obj;
  }
  function setData(&$data) {
    $this->ObjectArray = $data;
    $this->reset();
  }
  function clear() {
    $this->ObjectArray = NULL;
    $this->reset();
  }
  function reset() {
    $this->ObjPtr = NULL; 
    $this->CurrentIndex = -1;
  }
  function count() {
    return (count($this->ObjectArray));
  }
  /* return current record */
  function current() {
    if ( $this->CurrentIndex < 0 )
       return $this->next();
    return $this->ObjectArray[$this->getCurrentIndex()];
  }

  /* Return a reference to the current record */
  function & currentRef() {
    if ( $this->CurrentIndex < 0 )
       return $this->next();
    return $this->ObjectArray[$this->getCurrentIndex()];
  }
  function hasNext() {
    return (($this->count() > 0) && $this->getCurrentIndex() < ($this->count()-1) );  
  }
  function hasPrevious() {
    return ($this->CurrentIndex > 0);
  }
  function previous() {
    if ( $this->hasPrevious() ) {
      $this->ObjPtr = $this->ObjectArray[(--$this->CurrentIndex)];
      return $this->current();
    }
  }
  /* return next record */
  function next() {
    if ( $this->hasNext() ) {
      $this->RecPtr = $this->Records[(++$this->CurrentIndex)];
      return $this->current();
    }
  }
  function getCurrentIndex() { return $this->CurrentIndex; }
}

if (!defined(PROP_DEF_SEPARATOR)) {
  define(PROP_DEF_SEPARATOR, "\n");
}

class Properties extends Object {
  protected $data;
  protected $separator;
  function __construct($separator = PROP_DEF_SEPARATOR) {
    $this->data = array();
    $this->separator = $separator;
  }
  function __destruct() {
    $this->data = NULL;
  }
  function setSeparator($separator) {
    $this->separator = $separator;
  }
  function set($name, $val, $overwrite = true) {
    $ret = "";
    if (isset($this->data[$name])) {
      if ($overwrite) {
        $ret = $this->data[$name];
        $this->data[$name] = $val;
      } else {
        $ret = FALSE;
      }
    } else {
      $this->data[$name] = $val;
    }
    return $ret;
  }
  function get($name) {
    if (!isset($this->data[$name])) {
      return FALSE;
    }
    return $this->data[$name];
  }
  function raw(&$vars) {
    if (!is_array($vars)) {
      return FALSE;
    }
    $this->data = $vars;
  }
  function length() {
    return count($this->data);
  }
/*
  function unset($name) {
    if (isset($this->data[$name])) {
      unset($this->data[$name]);
    }
  }*/
  function reset() {
    if (count($this->data) > 0) {
      unset($this->data);
    }
  }
  function serialize() {
    $ret = array();
    foreach ($this->data AS $key => $val) {
      $ret[] = $key."=".$val;
    } 
    return implode($this->separator, $ret);
  }
  static function & unserialize($data, $separator = PROP_DEF_SEPARATOR) {
    $ret = FALSE;
    if (is_array($data)) {
      return $ret;
    }
    $ret  = new Properties();
    $props = explode($separator, $data);
    foreach ($props as $prop) {
      list($name, $val) = split('=', $prop);
      $name = trim($name);
      if (!empty($name)) {
        $val = preg_replace('#^"([^"]*)"$#', "$1", trim($val));
        if (is_numeric($val)) {
          settype($val, "integer");
        } else if (strtolower($val) == "true") {
          $val = true;
        } else if (strtolower($val) == "false") {
          $val = false;
        } else if (strtolower($val) == "null") {
          $val = NULL;
        } else {
          settype($val, "string");
        }
        $ret->set($name, $val);
      }
    }
    return $ret;
  }
  function toFile($filename) {
    $fd = fopen($filename, "w+");
    if (!$fd) {
      return FALSE;
    }
    fwrite($fd, $this->serialize());
    fclose($fd);
  }
  static function & fromFile($filename, $separator = PROP_DEF_SEPARATOR) {
    $ret = FALSE;
    $data = file_get_contents($filename);
    if ($data === FALSE) {
      return $ret;
    }
    $cc = '#/\*[^*]*\*+([^/*][^*]*\*+)*/|//[^\n]*|("(\\.|[^"\\\])*"|\'(\\.|[^\'\\\])*\'|.[^/"\'\\\]*)#s';
    $ws = '#^[ \t\n]*#ms';
    $perl = '#^\#.*$#m';
    $data = preg_replace($cc, "$2", $data);
    $data = preg_replace($perl, "", $data);
    $data = preg_replace($ws, "", $data);
    return Properties::unserialize($data, $separator);
  }

  function & getData() {
    return $this->data;
  }
  
  function toString() {
    return $this->serialize();
  }
}

class PHPUtil extends Object {
  static function checkVersion($currentversion, $requiredversion) {
    list($majorC, $minorC, $editC) = split('[/.-]', $currentversion);
    list($majorR, $minorR, $editR) = split('[/.-]', $requiredversion);
   
    if ($majorC > $majorR) return true;
    if ($majorC < $majorR) return false;
    // same major - check minor
    if ($minorC > $minorR) return true;
    if ($minorC < $minorR) return false;
    // and same minor - check edit
    if ($editC  >= $editR)  return true;
    return true;
  }
}


}
?>
