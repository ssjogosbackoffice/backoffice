<?php

if (!class_exists('Database')) {

include("MDB2.php");
//include_shared("classes/DatabaseHandler.class.inc");
include_once("classes/DatabaseHandler.class.inc");

class Database {

  static public $db = null;

  static function ensureConnected() {
    if (!is_a(self::$db, "DatabaseHandler") ) {
      self::$db = new DatabaseHandler();
      self::$db->connect();
    }
  }

  static function & getConnection() {
    self::ensureConnected();
    return self::$db;
  }

  static function escape($str) {
    self::ensureConnected();
    return self::$db->escape($str);
  }

  static function query($sql) {
    self::ensureConnected();
    return self::$db->exec($sql);
  }

  static function countQuery($sql) {
    self::ensureConnected();
    return self::$db->countQuery($sql);
  }

  static function queryOne($sql) {
    self::ensureConnected();
    return self::$db->queryOne($sql);
  }

  static function nextId($seq) {
    self::ensureConnected();
    return self::$db->nextSequence($sql);
  }
}

}
?>
