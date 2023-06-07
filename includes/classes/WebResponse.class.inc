<?php
class WebResponse {
	public $response;
	function __construct($resp) {
		$this->response = $resp;
	}
	function checkResponse($redirect = true) {
		if ($this->response < 0) {
			if($redirect) {
				header ( "Location: /error/$this->response" );
			}else{
				include_once 'Error.class.inc';
				$err = new Error($this->response);
				return $err->checkError();
			}
			die ();
		} else {
			return $this->response;
		}
	}
}
?>
