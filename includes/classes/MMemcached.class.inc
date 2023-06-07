<?php

class MMemcached {

    static $mc = null;
    static $lifetime = 360;

    public function __construct($pid, $servername, $port) {

        if(empty($pid) || empty($servername) || empty($port)) {
            throw new Exception("Memcached::No paramaters set");
        }
        self::$mc = new Memcached($pid);
        //self::$mc->setOption(Memcached::OPT_LIBKETAMA_COMPATIBLE, true);
        $pool = count(self::$mc->getServerList());
        if($pool < 1) {
            self::$mc->addServer($servername, $port);
        } elseif($pool > 1) {
            self::$mc->resetServerList();
            self::$mc->addServer($servername, $port);
        }
    }

    public function set($key, $value, $expire = null) {
        if(empty($key) || empty($value)) {
            throw new Exception("Memcached::No values set");
        }
        $result = self::$mc->set( $key, $value, ($expire == null ? time() + self::$lifetime : ($expire == 0 ? 0 : time() + $expire)) );
        $time =date('Y-m-d H:i:s',($expire == null ? time() + self::$lifetime : ($expire == 0 ? 0 : time() + $expire)));
        error_log("################ SET MEMCACHE ################: for key " . $key . " value: " . $value ." time: ".$time);
        return $result;
    }

    public function get($key) {
        $result = self::$mc->get($key);
        error_log("Message After get : " . self::$mc->getResultMessage());
        return $result;
    }

    public function add($key, $value, $expire = 0) {
        if(empty($key) || empty($value)) {
            throw new Exception("Memcached::No values set");
        }
        $result = self::$mc->add($key, $value, ($expire <= 0 ? self::$lifetime : $expire));
        $time =date('Y-m-d H:i:s',($expire == null ? time() + self::$lifetime : ($expire == 0 ? 0 : time() + $expire)));
        error_log("################ ADD MEMCACHE ################: for key " . $key . " value: " . $value ." time: ".$time);
        return $result;
    }

    public function delete($key) {
        if(empty($key)) {
            throw new Exception("Memcached::No values set");
        }
        $result = self::$mc->delete($key);
        error_log("Message After delete : " . self::$mc->getResultMessage());
        return $result;
    }

    public function close(){
        self::$mc->quit();
    }
}

?>
