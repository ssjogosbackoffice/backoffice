<?php
/**
 * Created by marian.
 * User: marian
 * Date: 8/11/15
 * Time: 12:07 AM
 */

class MMemcache {

    /**
     * Instance of the object needed to manage the connection with the server memcached
     */
    // private $mem_conn = null;

    /**
     * the time expressed in seconds about how much the information will be in the server
     */
    // private $lifetime = 600;

    /**
     * class constructor
     */
    // public function __construct($pid, $host, $port) {
        // $this->mem_conn = new Memcache();
        // $this->mem_conn->addServer($host, $port, false);
    // }//end function


    /**
     * save the information key - value in the server
     *
     * @param $key
     * @param $value
     * @param $expire
     * @throws Exception
     * @return bool
     */
    // public function set($key, $value, $expire = null) {
        // if(empty($key) || empty($value) || empty($this->mem_conn)) {
            // throw new Exception("Memcached::No values set or connection object not initialized");
        // } // end if

        // $status = $this->mem_conn->set( $key, $value,false, ($expire == null ? time() + $this->lifetime : ($expire == 0 ? $this->lifetime : time() + $expire)) );
        // $time =date('Y-m-d H:i:s',($expire == null ? time() + $this->lifetime : ($expire == 0 ? $this->lifetime : time() + $expire)));
        // error_log("################ SET MEMCACHE ################:" . $status. " for key " . $key . " value: " . $value ." time: ".$time);
	// return $status;
    // } // end method set (String, String, int)


    /**
     * save the information key - value only if the key doesn't actually exist
     *
     * @param $key
     * @param $value
     * @param int $expire
     * @throws Exception
     * @return bool
     */
    // public function add($key, $value, $expire = 0) {
        // if(empty($key) || empty($value) || empty($this->mem_conn)) {
            // throw new Exception("Memcached::No values set or connection object not initialized");
        // } // end if

        // $status = $this->mem_conn->add($key, $value, false, ($expire <= 0 ? $this->lifetime : $expire));
        // $time = date('Y-m-d H:i:s',($expire <= 0 ? (time() + $this->lifetime ): (time() + $expire)));
        // error_log("################ ADD MEMCACHE ################:" . $status. " for key " . $key . " value: " . $value ." time: ".$time);
        // return $status;
    // } // end method add(String, String, int)


    /**
     * delete key from memcache server
     *
     * @param $key
     * @throws Exception
     * @return bool
     */
    // public function delete($key) {
        // if(empty($key) || empty($this->mem_conn)) {
            // throw new Exception("Memcached::No values set or connection object not initialized");
        // } // end if
        // return $this->mem_conn->delete($key);
    // } // end method delete(String)


    /**
     * get value from key
     *
     * @param $key
     * @return array|string
     */
    // public function get($key) {
        // $status = $this->mem_conn->get($key);
        // error_log("GET MEMCACHE:" . $status . " for key " . $key);
        // return $status;
    // } // end method get(String)


    /**
     * initialize the connection with the server
     *
     * @return bool
     */
    // private function init_connection() {
        // $msg = "[MMemcache.init_connection]";
        // $isConnected = false;
        // if (empty($this->mem_conn)) {
            // return $isConnected;
        // } // end if
        // $isConnected = $this->mem_conn->connect();
        // return $isConnected;
    // } // end method init_connection()


    /**
     * close the connection with the server memcache
     *
     * @return bool|void
     */
    // private function close_connection() {
        // $msg = "[MMemcache.init_connection]";
        // $isConnected = false;
        // if (empty($this->mem_conn)) {
            // return $isConnected;
        // } // end if
        // $isConnected = $this->mem_conn->close();
    // } // end method close_connection()


    /**
     * function called automatic by php
     */
    // public function __destruct() {
        // $this->close_connection();
    // } // end method __destruct()
}
