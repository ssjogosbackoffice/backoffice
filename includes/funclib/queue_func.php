<?php
function getMemcached(){
  global $mem;
  if(defined("MEMCACHED_LOCK_SERVER")){  
    if(empty($mem)){
      list($addr, $port) = split(":", MEMCACHED_LOCK_SERVER);
      $m   = new Memcache();
      $r   = $m->connect($addr, $port);
      if(!$r){
        dieWithError("Check lock server <!--" . MEMCACHED_LOCK_SERVER . "-->");
      }
      $mem = $m;
    }
  }else{
    dieWithError("Please set MEMCACHED_LOCK_SERVER");    
  }
  return $mem;
}

function queue_begin($myprocess = "", $mytimeout = null) {
  return true;
  $keepalive  = 2; 
  $starttime  = time();
  $mytimeout  = (empty($mytimeout))?(20):($mytimeout);
  $m          = getMemcached();
  do {($valid = ($m->get($myprocess) == ""))?(null):(sleep(1));} while (!$valid && (time() - $starttime) < $keepalive);
  if($valid)$m->set($myprocess, "0", 0, $mytimeout);
  return $valid;
}

function queue_end($myprocess){
  return true;
  return getMemcached()->delete($myprocess);
}
?>
