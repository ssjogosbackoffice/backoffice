<?php

if (!function_exists("get_directory")) {
 function get_directory($dir) {
   if (!file_exists($dir)) {
     if (!mkdir($dir, 0700)) {
       die("unable to create directory $dir");
     }
   }
   elseif (!is_dir($dir)) {
     die("$dir exists but is not a directory!");
   }
   return $dir;
 }
}


function queue_begin($myprocess = "", $mytimeout = 0) {
  $keepalive = 60;
  $starttime = time();
  $begin_return = false;
  if (!$mytimeout) { $mytimeout = 20; }
  $lock_dir    = get_directory(cache_file_dir . "/locks");
  $lock_file   = $lock_dir . "/$myprocess.dat";
  if (file_exists($lock_file)) {
    while ((file_exists($lock_file)) && ((time() - $starttime) < $mytimeout)) {
      if (filemtime($lock_file) < (time() - $keepalive)) {
        // delete old process
        unlink($lock_file);
      } else {
        //usleep(100000);
        sleep(1);
      }
    }
  }
  if (!file_exists($lock_file)) {
    // create process
    $pid = fopen($lock_file, "w");
    fclose($pid);
    $begin_return = true;
  }
  return $begin_return;
}

function queue_end($myprocess = "") {
  $lock_dir    = get_directory(cache_file_dir . "/locks");
  $lock_file   = $lock_dir . "/$myprocess.dat";
  if (file_exists($lock_file)) {
    // delete process
    unlink($lock_file);
  }
}

?>
