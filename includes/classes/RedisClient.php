<?php

/**
 * Created by PhpStorm.
 * User: Coletti Alessio
 * Date: 07.07.2016
 * Time: 16:55
 */

include_once 'Logger.php';
require_once 'predis/src/Autoloader.php';


class RedisClient
{
    /**
     * @var Logger used to print the application message
     */
    private $logger = null;

    /**
     * Instance of the object needed to manage the connection with the redis server
     */
    private $redisClient = null;

    /**
     * class constructor
     *
     * @param string $host the host where is running memcache
     * @param string|int $port the port where is listening memcache
     * @param Logger $logger instance of the logger class used to print the application messages
     */
    public function __construct($host, $port, $logger)
    {
        $this->logger = $logger;
        Predis\Autoloader::register();
        $this->redisClient = new Predis\Client(array("scheme" => "tcp", "host" => $host, "port" => $port));
    } //end constructor


    /**
     * save the information key - value in the server
     *
     * @param $key
     * @param $value
     * @param $expire
     * @throws Exception
     * @return bool
     */
    public function set($key, $value, $expire = null)
    {
        if (empty($key) || empty($value) || empty($this->redisClient)) {
            throw new Exception("Redis::No values set or connection object not initialized");
        } // end if
        $status = $this->redisClient->set($key, $value);
        if (isset($expire)) {
            $this->redisClient->expire($key, $expire);
        } // end if
        return $status;
    } // end method set (String, String, int)

    /**
     * delete key from memcache server
     *
     * @param $key
     * @throws Exception
     * @return bool
     */
    public function delete($key)
    {
        if (empty($key) || empty($this->redisClient)) {
            throw new Exception("Redis::No values set or connection object not initialized");
        } // end if
        return $this->redisClient->del($key);
    } // end method delete(String)


    /**
     * get value from key
     *
     * @param $key
     * @return array|string
     */
    public function get($key)
    {
        $status = $this->redisClient->get($key);
        error_log("GET REDIS:" . $status . " for key " . $key);
        return $status;
    } // end method get(String)

    public function getKeys($key)
    {
        $status = $this->redisClient->keys($key);
        error_log("GET REDIS:" . $status . " for key " . $key);
        return $status;
    } // end method get(String)
}