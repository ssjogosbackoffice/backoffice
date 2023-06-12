<?php
class Cookie {
	private $_valueCookie;
	private $_nameCookie;
	function __construct($name) {
        self::setCookieName ( $name );
		self::setCookieValue ();
	}
	
	public function setCookie($value = '', $expire = 0, $path = '', $domain = '', $secure = false, $httponly = false) {
		$this->_valueCookie = $value;
		return setcookie ( $this->_nameCookie, $value, $expire, $path, $domain, $secure, $httponly );
	}
	
	public function setCookieName($name) {
		$this->_nameCookie = $name;
	}
	
	public function deleteCookie() {
		$this->_valueCookie = NULL;
		return setcookie ( $this->_nameCookie, NULL, - 1 );
	}
	
	public function getCookieValue() {
		return $this->_valueCookie;
	}
	
	public function updateCookie($expire = 0, $path = '', $domain = '', $secure = false, $httponly = false) {
		return setcookie ( $this->_nameCookie, $this->_valueCookie, $expire, $path, $domain, $secure, $httponly );
	}

    public function wasSentByClient()
    {
        return isset($_COOKIE["$this->_nameCookie"]);
    }

    public function setCookieValue() {
		if (isset($_COOKIE[$this->_nameCookie])) {
			$this->_valueCookie = $_COOKIE[$this->_nameCookie];
		} else {
			$this->_valueCookie = NULL;
		}
	}	
}

?>