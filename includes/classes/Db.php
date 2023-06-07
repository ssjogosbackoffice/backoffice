<?php
class Db {
    private static $_pdoObject = null;
    protected static $_fetchMode = PDO::FETCH_ASSOC;
    protected static $_connectionStr = null;
    protected static $_driverOptions = array();
    private static $_username = null;
    private static $_password = null;
    private static $_return = null;

    /**
     * Set connection information
     *
     * @example    Db::setConnectionInfo('basecamp','dbuser', 'password', 'mysql', 'http://mysql.abcd.com');
     */
    public static function setConnectionInfo($schema, $username = null, $password = null, $database = 'mysql', $hostname = 'localhost',$port='3306') {
        if(self::$_pdoObject == null) {
            if($database == 'mysql') {
                self::$_connectionStr = "mysql:dbname=$schema;host=$hostname;port=$port";
                self::$_username = $username;
                self::$_password = $password;
            } else if($database == 'sqlite') {
                // For sqlite, $schema is the file path
                self::$_connectionStr = "sqlite:$schema";
            }
            // Making the connection blank
            // Will connect with provided info on next query execution
            //self::$_pdoObject = null;
        }
    }


    /**
     * Esegue stored procedure che restituisce pi� dataset
     * @param string $sp Stored procedure
     */
    public static function execStoredProcedureMultiDs($sp) {
        if(self::$_pdoObject == null) {
            self::_connect();
        }
        $stmt = self::$_pdoObject->prepare($sp);
        $stmt->execute();
        $ds = array();
        do {
            $rows = $stmt->fetchAll(self::$_fetchMode);
            if($rows) {
                $ds[] = $rows;
            }
        } while($stmt->nextRowset());

        return $ds;
    }


    /**
     * Execute a statement and returns number of effected rows
     *
     * Should be used for query which doesn't return resultset
     *
     * @param   string $sql    SQL statement
     * @param   array $params A single value or an array of values
     * @return  integer number of effected rows
     */
    public static function execute($sql, $params = array()) {
        $statement = self::_query($sql, $params);
        return $statement->rowCount();
    }

    /**
     * Execute a statement and returns a single value
     *
     * @param   string $sql    SQL statement
     * @param   array $params A single value or an array of values
     * @return  mixed
     */
    public static function getValue($sql, $params = array()) {
        $statement = self::_query($sql, $params);
        return $statement->fetchColumn(0);
    }

    /**
     * Execute a statement and returns the first row
     *
     * @param   string $sql    SQL statement
     * @param   array $params A single value or an array of values
     * @return  array   A result row
     */
    public static function getRow($sql, $params = array()) {
        $statement = self::_query($sql, $params);
        return $statement->fetch(self::$_fetchMode);
    }


    public static function getNumRows() {
        if(!is_null(self::$_return)) {
            return count(self::$_return);
        } else {
            throw new PDOException('Db::getNumRows: No result found');
        }
    }


    /**
     * Execute a statement and returns row(s) as 2D array
     *
     * @param   string $sql    SQL statement
     * @param   array $params A single value or an array of values
     * @return  array   Result rows
     */
    public static function getResult($sql, $params = array()) {
        $statement = self::_query($sql, $params);
        self::$_return = $statement->fetchAll(self::$_fetchMode);
        return self::$_return;
    }

    public static function getLastInsertId($sequenceName = "") {
        return self::$_pdoObject->lastInsertId($sequenceName);
    }


    public static function setFetchMode($fetchMode) {
        self::_connect();
        self::$_fetchMode = $fetchMode;
    }

    public static function getPDOObject() {
        self::_connect();
        return self::$_pdoObject;
    }

    public static function beginTransaction() {
        self::_connect();
        return self::$_pdoObject->beginTransaction();
    }

    public static function commitTransaction() {
        self::$_pdoObject->commit();
    }

    public static function rollbackTransaction() {
        self::$_pdoObject->rollBack();
    }

    //Modifica Temporanea per vedere se l'oggetto è stato istanziato
    public static function inTransaction() {

        if(self::$_pdoObject != null) {
            return self::$_pdoObject->inTransaction();
        } else {
            return false;
        }
    }

    public static function setDriverOptions(array $options) {
        self::$_driverOptions = $options;
    }

    private static function _connect() {
        if(self::$_pdoObject != null) {
            return;
        }

        if(self::$_connectionStr == null) {
            throw new PDOException('Connection information is empty. Use Db::setConnectionInfo to set them.');
        }

        self::$_pdoObject = new PDO(self::$_connectionStr, self::$_username, self::$_password, self::$_driverOptions);
    }

    private static function _disconnect() {
        self::$_pdoObject = null;
    }

    /**
     * Prepare and returns a PDOStatement
     *
     * @param   string $sql SQL statement
     * @param   array $params Parameters. A single value or an array of values
     * @return  PDOStatement
     */
    private static function _query($sql, $params = array()) {

        if(self::$_pdoObject == null) {
            self::_connect();
        }

        $statement = self::$_pdoObject->prepare($sql, self::$_driverOptions);

        if(!$statement) {
            $errorInfo = self::$_pdoObject->errorInfo();
            throw new PDOException("Database error [{$errorInfo[0]}]: {$errorInfo[2]}, driver error code is $errorInfo[1]", $errorInfo[1]);
        }

        $paramsConverted = (is_array($params) ? ($params) : (array( $params )));

        if((!$statement->execute($paramsConverted)) || ($statement->errorCode() != '00000')) {
            $errorInfo = $statement->errorInfo();
            throw new PDOException("Database error [{$errorInfo[0]}]: {$errorInfo[2]}, driver error code is $errorInfo[1]", $errorInfo[1]);
        }
        return $statement;
    }

    public static function nextSequence() {

    }

    public static function doCachedQueryRs($sql, $life_seconds) {
        $file_name = md5($sql);
        $file_path = SQL_CACHE_DIR . '/' . $file_name . ".cache";

        if(file_exists($file_path)) {
            if((time() - filemtime($file_path)) < $life_seconds) {
                $file = fopen($file_path, "r");
                $content = null;
                if(filesize($file_path) > 0) {
                    $content = fread($file, filesize($file_path));
                }
                return unserialize($content);
            }
        }

        $rs = self::getResult($sql);

        if(count($rs)) {
            $file = fopen($file_path, "w+");
            fwrite($file, serialize($rs));
            fclose($file);
        }
        return $rs;
    }

    public static function doCachedQueryRsMultiDs($sql, $life_seconds) {
        $file_name = md5($sql);
        $file_path = SQL_CACHE_DIR . '/' . $file_name . ".cache";

        if(file_exists($file_path)) {
            if((time() - filemtime($file_path)) < $life_seconds) {
                $file = fopen($file_path, "r");
                $content = null;
                if(filesize($file_path) > 0) {
                    $content = fread($file, filesize($file_path));
                }
                return unserialize($content);
            }
        }

        $rs = self::execStoredProcedureMultiDs($sql);

        if(count($rs)) {
            $file = fopen($file_path, "w+");
            fwrite($file, serialize($rs));
            fclose($file);
        }
        return $rs;
    }

    /** Prepares the string for database operations
     * @param string $str -  the string to be prepared
     * @return string - the string prepared
     */
    public static function prepareString($str) {
        return "'".self::escapeQuotesAndSlashes($str)."'"; //escape single quotes and slashes,
        //and add quotes around entire string	}
    }


    /**
     * Function to escape slashes and single quotes of a string
     * @return string The string escaped
     */
    private static function escapeQuotesAndSlashes($str) {
        $str = mb_ereg_replace("\\\\", "\\\\", $str); //escape slashes
        return mb_ereg_replace("'", "''", $str); //escape single quotes
    }

}
