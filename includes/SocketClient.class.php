<?php

class SocketClient {
	
	private $host;
	private $port;
	private $message;
	private $socket;
	private $connected;
	
	public function __construct($domain = AF_INET, $type = SOCK_STREAM, $protocol = SOL_TCP) {
		$this->socket = socket_create ( $domain, $type, $protocol );
		if ($this->socket === false) {
			echo "socket_create() failed: reason: " . socket_strerror ( socket_last_error () ) . "\n";
		}
	}
	
	public function connect($host, $port) {
		$this->host = $host;
		$this->port = $port;
		$this->connected = socket_connect ( $this->socket, $this->host, $this->port );
	
	}
	
	public function close() {
		if (! $this->connected)
			return;
		socket_shutdown ( $this->socket, 2 );
		socket_close ( $this->socket );
	}
	public function isConnected() {
		return $this->connected;
	}
	
	public function disconnect() {
		return $this->close ();
	}
	
	public function write($message) {
		$this->buff = $message . "\n";
		$bytes = socket_write ( $this->socket, $this->buff, strlen ( $this->buff ) );
		if ($bytes == false) {
			return FALSE;
		}
		return TRUE;
	}
	
	public function read() {
		$res = socket_read ( $this->socket, 4096 );
		return $res;
	}
}
?>