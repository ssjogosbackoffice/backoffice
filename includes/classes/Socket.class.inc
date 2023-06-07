<?php

define(SOCKSTATUS_DISCONNECTED, 0);
define(SOCKSTATUS_CONNECTED, 1);
define(SOCKSTATUS_ERROR, 2);

class Socket extends Object {
  var $host;
  var $port;
  var $sock;
  var $timeout;
  var $status;
  var $binary;
  var $error_string;

  function __construct($host, $port, $timeout = 5, $binary = TRUE)
  {
    $this->host = $host;
    $this->port = $port;
    $this->timeout = $timeout;
    $this->status = SOCKSTATUS_DISCONNECTED;
    $this->sock = NULL;
    $this->binary = $binary;
    $this->error_string = "";
    $this->connect();
  }
  function __destruct() {
    $this->disconnect();
  }
  function connect() {
    $sock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
    if (!$sock) {
      $this->status = SOCKSTATUS_ERROR;
      $this->error_string = socket_strerror(socket_last_error());
      return FALSE;
    }
    socket_set_nonblock($sock);
    $old_er = error_reporting(E_ERROR);
    if (!socket_connect($sock, $this->host, $this->port)) {
      $err = socket_last_error($sock);
      if ($err != 114 && $err != 115) {
        $this->status = SOCKSTATUS_ERROR;
        $this->error_string = socket_strerror(socket_last_error($this->sock));
        error_reporting($old_er);
        return FALSE;
      }
    }
    error_reporting($old_er);
    $changed_sockets = socket_select($r = NULL, $w = array($sock), $e = NULL, $this->timeout);
    if ($changed_sockets === false) {
      $this->status = SOCKSTATUS_ERROR;
      $this->error_string = socket_strerror(socket_last_error($this->sock));
      return FALSE;
    } else if ($changed_sockets == 0) {
      $this->status = SOCKSTATUS_ERROR;
      $this->error_string = "Connection Timed Out";
      return FALSE;
    }
    $errstatus = socket_get_option($sock, SOL_SOCKET, SO_ERROR);
    if ($errstatus != 0) {
      $this->status = SOCKSTATUS_ERROR;
      $this->error_string = socket_strerror($errstatus);
      return FALSE;
    }
    socket_clear_error($sock);
    $this->status = SOCKSTATUS_CONNECTED;
    $this->sock = $sock;
    $this->error_string = "";
    return TRUE;
  }
  function disconnect() {
    if ($this->isConnected()) {
      socket_close($this->sock);
      $this->sock = NULL;
      $this->status = SOCKSTATUS_DISCONNECTED;
    }
  }
  function read($len) {
    if (!$this->isConnected())
      return "";
    $buffer = "";
    $buffer_len = 0;
    $mode = $this->isBinary() ? PHP_BINARY_READ : PHP_NORMAL_READ;
    while ($buffer_len < $len) {
      $temp_buf = NULL;
      $changed_sockets = socket_select($r = array($this->sock), $w = NULL, $e = NULL, $this->timeout);
      if ($changed_sockets === false) {
        $this->status = SOCKSTATUS_ERROR;
        $this->error_string = socket_strerror(socket_last_error($this->sock));
        return "";
      } else if ($changed_sockets == 0) {
        if ($buffer_len > 0) {
          break;
        } else {
          $this->status = SOCKSTATUS_ERROR;
          $this->error_string = "Read Timeout";
          return "";
        }
      }
      $errstatus = socket_get_option($this->sock, SOL_SOCKET, SO_ERROR);
      if ($errstatus != 0) {
        $this->status = SOCKSTATUS_ERROR;
        $this->error_string = socket_strerror($errstatus);
        return "";
      }
      $temp_buf = socket_read($this->sock, $len, $mode);
      $buffer_len += strlen($temp_buf);
      $buffer .= $temp_buf;
    }
    return $buffer;
  }
  function write($data, $len = -1) {
    if (!$this->isConnected() || empty($data)) {
      return 0;
    }
    $buffer_len = 0;
    if ($len == -1) {
      $len = strlen($data);
    } 
    while ($buffer_len < $len) {
      $changed_sockets = socket_select($r = NULL, $w = array($this->sock), $e = NULL, $this->timeout);
      if ($changed_sockets === false) {
        $this->status = SOCKSTATUS_ERROR;
        $this->error_string = socket_strerror(socket_last_error($this->sock));
      } else if ($changed_sockets == 0) {
        $this->status = SOCKSTATUS_ERROR;
        $this->error_string = "Write Timeout";
        return 0;
      }
      $errstatus = socket_get_option($this->sock, SOL_SOCKET, SO_ERROR);
      if ($errstatus != 0) {
        $this->status = SOCKSTATUS_ERROR;
        $this->error_string = socket_strerror($errstatus);
        return 0;
      }
      $tosend = substr($data, $buffer_len);
      $bytes_sent = socket_write($this->sock, $tosend, ($len - $buffer_len));
      $buffer_len += $bytes_sent;
    }
    return $buffer_len;
  }
  function setBinary($val = TRUE) {
    $this->binary = $val;
  }
  function isBinary() { return ($this->binary != FALSE); }
  function isConnected() { return ($this->status == SOCKSTATUS_CONNECTED); }
  function isError() { return ($this->status == SOCKSTATUS_ERROR); }
  function getErrorMessage() { return $this->error_string; }
}

?>