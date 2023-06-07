<?php

if (!class_exists("StringTokenizer")) {

class StringTokenizer {
 var $pos = 0;
 var $tokens = array();

 function __construct ($string, $separator) {
   
   if (!is_string($string))
     trigger_error('StringTokenizer->__construct: param is not a string');
   $this->tokens = explode($separator, $string);
 }

 function __destruct() {}

 function countTokens() {
   return sizeof($this->tokens);
 }

 function hasMoreElements() {}

 function hasMoreTokens() {
   return ($this->pos < sizeof($this->tokens));
 }

 function nextElement() {}

 function nextToken() {
   if ($this->pos > sizeof($this->tokens))
     trigger_error('token??');
   return $this->tokens[$this->pos++];
 }
}

}
?>
