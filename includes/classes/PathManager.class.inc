<?php
if (!class_exists('PathManager')) {

class PathManager {
  private static $__pathman = null;
  protected $path;
  function __construct() {
    $this->path = explode(PATH_SEPARATOR, ini_get("include_path"));
  }
  function addPath($path, $pos) {
    if ($pos >= 0) {
      if (!isset($this->path[$pos]) || empty($this->path[$pos])) {
        $this->path[$pos] = $path;
      } else {
        if ($this->path[$pos] != $path) {
          $oldpos = array_search($path, $this->path);
          if ($oldpos !== FALSE) {
             unset($this->path[$oldpos]);
          }
          $tmp = $this->path[$pos];
          $this->path[$pos]  = $path;
          $this->path[]      = $tmp;
        }
      }
    } else {
      if (!in_array($path, $this->path)) {
        $this->path[] = $path;
      }
    }
  }
  function setPath() {
    ini_set("include_path", implode(PATH_SEPARATOR, $this->path));
  }
  static function singleton() {
    if (self::$__pathman == null) {
      $c = __CLASS__;
      self::$__pathman = new $c;
    }
    return self::$__pathman;
  }
  static function add($path, $pos = -1) {
    PathManager::singleton()->addPath($path, $pos);
  }
  static function set() {
    PathManager::singleton()->setPath();
  }
}

}
?>
