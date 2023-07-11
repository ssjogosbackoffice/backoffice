<?php
// define('MEMCACHED_SERVERS', "/srv/web/backoffice/config/memcached.servers");
//define('MEMCACHED_SERVERS', "/srv/casino/htdocs/backoffice/config/memcached.servers");
require_once(dirname(__FILE__).'/cache_wrapper_memcache.inc.php');
//echo implode("<br/>", $_SESSION["access"]);
class SiteCache {
  static private $continuum = null;

  static private function ensureConnected() {
    if (self::$continuum == null || !is_resource(self::$continuum) ) {
      if (!defined('MEMCACHED_SERVERS')) {
        throw new Exception("No MEMCACHED_SERVERS defined");
      }
      //self::$continuum = ketama_roll(MEMCACHED_SERVERS);
    }
  }

  static protected function getResourceAddress($key) {
      return "file:///srv/web/backoffice/cache/$key.cache";
    //return "file:///srv/casino/htdocs/backoffice/cache/$key.cache";
    if ($key{0} != "/") { $key = "/$key"; }
    $server = ketama_get_server($key, self::$continuum);
    return "cache://" . $server["ip"] . "$key";
  }

  protected function __destruct() { ketama_destroy(self::$continuum); }

  public static function store($key, $val) {
    
    self::ensureConnected();
    $file   = self::getResourceAddress($key);

    if (file_exists($file)){
     unlink($file);
    }
    
    $fd = fopen($file, "w");
    fwrite($fd, $val);
    fclose($fd);
  }

  public function fetch($key) {
    self::ensureConnected();
    
    $file  = self::getResourceAddress($key);
    
    $val   = null;
    if (file_exists($file)) {
      $fd  = fopen($file, "r");
      while (!feof($fd)) {
        $val .= fread($fd, 8192);
      }
      fclose($fd);
    }
    return $val;
  }

  public function delete($key) {
    self::ensureConnected();
    $file = self::getResourceAddress($key);
    if (file_exists($file)) unlink($file);
  }
}
?>
