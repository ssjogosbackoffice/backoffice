<?php
class WebRequest {
	private $curl;
	private $options;
	private $response;
    private $_url;

	function __construct($url = WEB_APP_PATH) {
        $this->_url = $url;
		$this->curl = curl_init ( $url );
	}
	
	function setOptions($optArray, $get = false) {
        if($get)
        {
            $this->options = array(CURLOPT_URL => $this->_url . $optArray, CURLOPT_RETURNTRANSFER => 1);
        }else
        {
            $this->options = array (CURLOPT_POSTFIELDS => $optArray, CURLOPT_RETURNTRANSFER => 1 );
        }
		curl_setopt_array ( $this->curl, $this->options );

    }
	
	function sendRequest() {
		$this->response = curl_exec ( $this->curl );
	}
	function getResponse() {
		return $this->response;
	}
    function setParameter($option,$value) {
       $this->options[$option]=$value;
        curl_setopt_array ( $this->curl, $this->options );
    }
    function setEncoding(){
        curl_setopt( $this->curl, CURLOPT_HTTPHEADER, array('Content-Type: application/x-www-form-urlencoded; charset=UTF-8'));
    }
}
?>