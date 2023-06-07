<?php
stream_wrapper_register('cache', 'stream_memcache');
/*
*/

class stream_memcache { 
  private $_id    = ''; 
  private $_file  = ''; 
  private $_open  = false; 
  private $_pos   = 0; 
  private $_mode  = 0; // 1 = read, 2 = write, 3 = both 
  private $_data  = null; 
  private $_isdir = false; 
  private $_time  = 0; 
  private $_size  = 0; 
  private $_path  = ''; 
  private $_src   = 'mem'; 
  private $_mc    = null;
        
  private function exists_data($path, $store = false) {
    $res = parse_url($path);
    $ret = false;
    if (empty($res["host"]))
      return false;
    if (empty($res["port"]))
      $res["port"] = 11211;
    $file = $res["path"];
    $id   = $file;
    $mc   = memcache_connect($res["host"], $res["port"]);
    if ($mc == false)
      return false;
    $data = memcache_get($mc, $id);
    if ($data == false) { 
      if (file_exists($file)) { 
        $data = array('time' => filemtime($file), 'size' => filesize($file)); 
        if ($store) { 
          memcache_set($mc, $id, array('time' => $data['time'], 'size' => $data['size'], 'content' => file_get_contents($file))); 
        } 
        $ret = &$data;
      }
    }
    else { 
      $ret = array('time' => $data['time'], 'size' => $data['size']); 
    }
    memcache_close($mc);
    return $ret;
  } 

  private function load_data() { 
    $data = memcache_get($this->_mc, $this->_id); 
    if ($data != false) { 
      $this->_time = $data['time']; 
      $this->_size = $data['size']; 
      $this->_data = $data['content']; 
      $this->_src  = 'mem'; 
      return true;                   
    }
    else { 
      if (file_exists($this->_file)) { 
        $this->_time = filemtime($this->_file); 
        $this->_size = filesize($this->_file); 
        $this->_data = file_get_contents($this->_file); 
        $this->_src  = 'disk'; 
        return true; 
      }
      else { 
        $this->_data = ''; 
        $this->_time = time(); 
        $this->_size = 0; 
        return false; 
      } 
    } 
  } 
    
  private function save_data() { 
    if (($this->_mode & 2) || ($this->_src == 'disk')) { 
      memcache_set($this->_mc, $this->_id, array('time' => $this->_time, 'size' => $this->_size, 'content' => $this->_data)); 
    } 
  } 
    
  function stream_open($path, $mode, $options, &$opened_path) {
    $res = parse_url($path);
    $ret = false;
    if (empty($res["host"]))
      return false;
    if (empty($res["port"]))
      $res["port"] = 11211;
    $file = $res["path"];
    $id   = $file;
    $mc   = memcache_connect($res["host"], $res["port"]);
    if ($mc == false)
      return false;

    $this->_mc  = &$mc;
    $this->_id  = $file;
    $this->file = $file;
    $this->path = $file;
    $fmode      = $this->_mode; 
        
    $ok = false; 
    $mode = strtolower($mode); 
    switch ($mode{0}) { 
      case    "r"  :  $ok = $this->load_data(); 
                      $fmode = $fmode | 1; 
                      break; 
      case    "w"  : 
      case    "a"  :  $this->load_data(); 
                      $ok = true; 
                      $fmode = $fmode | 2; 
                      break; 
      case    "x"  :  $this->load_data(); 
                      $ok = true; 
                      $fmode = $fmode | 2; 
                      break; 
    } 
        
    $opened_path = $this->_file; 

    if (!$ok) { return false; } 

    $this->_mode  = $fmode; 
    $this->_pos   = 0; 
    $this->_open  = true; 
    $this->_isdir = false; 

    if ($mode{0} == 'a') { 
      $this->stream_seek(0, SEEK_END); 
    } 

    if (strlen($mode) > 1 && $mode{1} == '+') { 
      $this->_mode = $this->_mode | 1 | 2; 
    } 

    return true; 
  } 

  function stream_eof() { 
    return ($this->_pos >= $this->_size); 
  } 

  function stream_tell() {
    return $this->_pos; 
  } 

  function stream_close() {
    $this->save_data(); 
    $this->_pos  = 0; 
    $this->_open = false;
    memcache_close($this->_mc);
  } 

  function stream_read($count)  {
    if (!$this->_open) { 
      return false; 
    } 

    if (!($this->_mode & 1)) { 
      return false; 
    } 

    $data = substr($this->_data, $this->_pos, $count); 
    $this->_pos = $this->_pos + strlen($data); 
    return $data; 
  } 

  function stream_write($data) {
    if (!$this->_open) { 
      return false; 
    } 
        
    if (!($this->_mode & 2)) { 
      return false; 
    } 
        
    $datalen = strlen($data); 
        
    $this->_data = substr($this->_data, 0, $this->_pos) . $data . substr($this->_data, $this->_pos+$datalen); 
    $this->_pos  = $this->_pos + $datalen; 
    if ($this->_pos > $this->_size) { 
      $this->_size = $this->_pos; 
    }
    return $datalen; 
  } 

  function stream_seek($offset, $whence) {
    switch ($whence) { 
      case SEEK_SET: 
        if (($offset < $this->_size) && ($offset >= 0)) { 
          $this->_pos = $offset; 
          return true; 
        }
        else { 
          return false; 
        } 
        break; 
      case SEEK_CUR: 
        if ($offset >= 0) { 
          $this->_pos += $offset; 
          return true; 
        }
        else { 
          return false; 
        } 
        break; 
      case SEEK_END: 
        if ($this->_size + $offset >= 0) { 
          $this->_pos = $this->_size + $offset; 
          return true; 
        }
        else { 
          return false; 
        } 
        break; 
      default: 
        return false; 
    } 
  } 

  function stream_flush() {
    return true; 
  } 

  function stream_stat() {
    if ($this->_isdir) { 
      $mode = 0040000; 
      $size = 0; 
      $time = time(); 
    }
    else { 
      $mode = 0100000; 
      $size = $this->_size; 
      $time = $this->_time; 
    } 
    $mode = $mode | 0777; 

    $keys = array( 
      'dev'     => 0, 
      'ino'     => 0, 
      'mode'    => $mode, 
      'nlink'   => 0, 
      'uid'     => 0, 
      'gid'     => 0, 
      'rdev'    => 0, 
      'size'    => $size, 
      'atime'   => $time, 
      'mtime'   => $time, 
      'ctime'   => $time, 
      'blksize' => 0, 
      'blocks'  => 0 
    ); 

    return array_merge(array_values($keys), $keys); 
  } 

  function url_stat($path, $flags) { 
//    if (strpos($path, '.tpl') === false) { 
//      $mode = 0040000; 
//      $size = 0; 
//      $time = time(); 
//    }
//    else { 
      $mode = 0100000; 
      $data = $this->exists_data($path, true); 
      if ($data === false) { return false; } 
        $time = $data['time']; 
        $size = $data['size']; 
//    } 
    $mode = $mode | 0777; 

    $keys = array( 
        'dev'     => 0, 
        'ino'     => 0, 
        'mode'    => $mode, 
        'nlink'   => 0, 
        'uid'     => 0, 
        'gid'     => 0, 
        'rdev'    => 0, 
        'size'    => $size, 
        'atime'   => $time, 
        'mtime'   => $time, 
        'ctime'   => $time, 
        'blksize' => 0, 
        'blocks'  => 0 
    ); 
        
    return array_merge(array_values($keys), $keys); 
  } 
    
  function unlink($path) { 
    $res = parse_url($path);
    $ret = false;
    if (empty($res["host"]))
      return false;
    if (empty($res["port"]))
      $res["port"] = 11211;
    $file = $res["path"];
    $mc   = memcache_connect($res["host"], $res["port"]);
    if ($mc != false) {
      memcache_delete($mc, $file, 0);
      memcache_close($mc);
      clearstatcache();
      $ret = true;
    }
    return $ret; 
  } 
    
  function rename($from, $to) { 
    return true; 
  } 
    
  function mkdir($path, $mode, $options) { 
    return true; 
  } 
    
  function rmdir($path, $options) { 
    return true; 
  } 

  function dir_opendir($path, $options) {
    $res  = parse_url($path);
    $file = $res["path"];
    $this->_id    = $file;
    $this->_file  = $file;
    $this->_path  = $file;
    $this->_isdir = true;

    $this->_open = true; 
    return true; 
  } 

  function dir_closedir() {
    $this->_open = false; 
    return true; 
  } 

  function dir_rewinddir() {
    if (!$this->_open) { 
      return false; 
    } 
    return true; 
  } 

  function dir_readdir() {
    return false; 
  } 
} 

/*
$file = "cache://127.0.0.1/p";
if (file_exists($file)) {
  $fp = fopen($file, "r");
  if ($fp != null) {
    $x  = fgets($fp);
    print $x;
    fclose($fp);
    unlink($file);
  }
}
else {
  $fp = fopen($file, "w");
  fputs($fp, "Hello World\n");
  fclose($fp);
  print "Data Set\n";
}
*/


/*
$rx = "^(?#Protocol)(?:(?:ht|f)tp(?:s?)\:\/\/|~\/|\/)?(?#Username:Password)(?:\w+:\w+@)?(?#Subdomains)(?:(?:[-\w]+\.)+(?#TopLevelDomains)(?:com|org|net|gov|mil|biz|info|mobi|name|aero|jobs|edu|co.uk|ac.uk|museum|travel|[a-z]{2}))(?#Port)(?::[\d]{1,5})?(?#Directories)(?:(?:(?:\/(?:[-\w~!$+|.,=]|%[a-f\d]{2})+)+|\/)+|\?|#)?(?#Query)(?:(?:\?(?:[-\w~!$+|.,*:]|%[a-f\d{2}])+=?(?:[-\w~!$+|.,*:=]|%[a-f\d]{2})*)(?:&(?:[-\w~!$+|.,*:]|%[a-f\d{2}])+=?(?:[-\w~!$+|.,*:=]|%[a-f\d]{2})*)*)*(?#Anchor)(?:#(?:[-\w~!$+|.,*:=]|%[a-f\d]{2})*)?$";

//preg_match("/$regex/", "http://www.softzone.it/pippo.html?suck-my&cock=pippo", $m);
//var_dump($m);

//print_r(j_parseUrl("http://www.softzone.it/pippo"));

function j_parseUrl($url) {
  $r  = "(?:([a-z0-9+-._]+)://)?";
  $r .= "(?:";
  $r .=   "(?:((?:[a-z0-9-._~!$&'()*+,;=:]|%[0-9a-f]{2})*)@)?";
  $r .=   "(?:\[((?:[a-z0-9:])*)\])?";
  $r .=   "((?:[a-z0-9-._~!$&'()*+,;=]|%[0-9a-f]{2})*)";
  $r .=   "(?::(\d*))?";
  $r .=   "(/(?:[a-z0-9-._~!$&'()*+,;=:@/]|%[0-9a-f]{2})*)?";
  $r .=   "|";
  $r .=   "(/?";
  $r .=     "(?:[a-z0-9-._~!$&'()*+,;=:@]|%[0-9a-f]{2})+";
  $r .=     "(?:[a-z0-9-._~!$&'()*+,;=:@\/]|%[0-9a-f]{2})*";
  $r .=    ")?";
  $r .= ")";
  $r .= "(?:\?((?:[a-z0-9-._~!$&'()*+,;=:\/?@]|%[0-9a-f]{2})*))?";
  $r .= "(?:#((?:[a-z0-9-._~!$&'()*+,;=:\/?@]|%[0-9a-f]{2})*))?";
  preg_match("`$r`i", $url, $match);
  $parts = array(
            "scheme"=>'',
            "userinfo"=>'',
            "authority"=>'',
            "host"=> '',
            "port"=>'',
            "path"=>'',
            "query"=>'',
            "fragment"=>'');
  switch (count ($match)) {
    case 10: $parts['fragment'] = $match[9];
    case 9: $parts['query'] = $match[8];
    case 8: $parts['path'] =  $match[7];
    case 7: $parts['path'] =  $match[6] . $parts['path'];
    case 6: $parts['port'] =  $match[5];
    case 5: $parts['host'] =  $match[3]?"[".$match[3]."]":$match[4];
    case 4: $parts['userinfo'] =  $match[2];
    case 3: $parts['scheme'] =  $match[1];
  }
  $parts['authority'] = ($parts['userinfo']?$parts['userinfo']."@":"").
                         $parts['host'].
                        ($parts['port']?":".$parts['port']:"");
  return $parts;
}

//print_r(parse_url("cache://10.10.3.100:8080/srv/www/hthome/dollarobaba/pippo.cache"));
*/
?>
