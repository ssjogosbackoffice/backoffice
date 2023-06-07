<?
include_once 'DatabaseHandler.class.inc';
class DatabaseWrapper{

    public $_dbInUse;
    public $_dbMaster;
    public $_dbSlave;
    private $_slaveInUse=1;
    private $_slaveConnections=false;

    function __construct(){
            $this->_dbMaster = new DatabaseHandler();
            $this->_dbMaster->connect();
            $this->_dbInUse=$this->_dbMaster;
            if($this->checkSlaves()){
                $this->_dbSlave  = $this->getSlaveInUse();
                $this->_dbSlave->connect();
                $this->_dbInUse=$this->_dbSlave;
            }
    }

    private function getSlaveInUse(){
        if(isset($_SESSION['slaveInUse'])){
            $this->_slaveInUse=$_SESSION['slaveInUse'];
        }
        $slave=new DatabaseHandler($_SERVER["SLAVEDBS_".$this->_slaveInUse]);
        $this->_slaveInUse++;
        $_SESSION['slaveInUse']=$this->_slaveInUse;
        if($this->_slaveInUse > $this->_slaveConnections){
            $_SESSION['slaveInUse']=1;
        }
        return $slave;
    }

    public function __get($value){
        return $this->_dbInUse->$value;
    }

    private function checkSlaves(){
        $this->_slaveConnections=$_SERVER['SLAVEDBS_NUMBER'];
        if(!$this->_slaveConnections){
           $this->_slaveConnections=false;
        }
        return $this->_slaveConnections;
    }

    public function checkQueryType($query,$forcedMaster=false){
        if($forcedMaster || $this->checkSlaves()==false ){
            $this->_dbInUse=$this->_dbMaster;
        }
        else{
            if (preg_match ( "#^\s*(INSERT|UPDATE|DELETE|REPLACE|SET) #i", $query )) {
                $this->_dbInUse=$this->_dbMaster;
            }
            else{
                $this->_dbInUse=$this->_dbSlave;
            }
        }
    }

    public function isConnected(){
        return $this->_dbInUse->isConnected();
    }

    public function getConnection(){
        return $this->_dbInUse->getConnection();
    }

    public function escape($str){
        return $this->_dbInUse->escape($str);
    }

    public function quote($str){
        return $this->_dbInUse->quote($str);
    }

    public function handleDatabaseError($error_code="", $sql=""){
        $this->_dbInUse->handleDatabaseError($error_code, $sql);
    }

    public function findWithFilter($table_name, $fields_arr, $all_req=true, $like_arr="") {
        return $this->_dbInUse->findWithFilter($table_name, $fields_arr, $all_req, $like_arr);
    }

    public function getAffectedRows(){
        return $this->_dbInUse->getAffectedRows();
    }

    public function  nextSequence ( $sequence_name ) {
        return $this->_dbMaster-> nextSequence ( $sequence_name ) ;
    }

    public function getBeforeId( $table ){
        return $this->_dbInUse->getBeforeId( $table );
    }

    public function getAfterId ( $id, $table ){
        return $this->_dbInUse->getAfterId ( $id, $table );
    }

    public function  lastInsertId( $table, $id ){
        return $this->_dbInUse-> lastInsertId( $table, $id );
    }

    public function setLimit($num, $srow = 0){
        return $this->_dbInUse->setLimit($num, $srow);
    }

    public function escapeQuotesAndSlashes($str){
        return $this->_dbInUse->escapeQuotesAndSlashes($str);
    }

    public function  prepareString($str){
        return $this->_dbInUse-> prepareString($str);
    }

    public function sql_extract_date($col_name){
        return $this->_dbInUse->sql_extract_date($col_name);
    }

    public function date_today(){
        return $this->_dbInUse->date_today();
    }

    public function timestamp_now(){
        return $this->_dbInUse->timestamp_now();
    }

    public function timestamp_today(){
        return $this->_dbInUse->timestamp_today();
    }

    public function time_now(){
        return $this->_dbInUse->time_now();
    }

    public function seconds_to_timestamp($seconds){
        return $this->_dbInUse->seconds_to_timestamp($seconds);
    }

    public function connect() {
        return $this->_dbInUse->connect();
    }

    public function disconnect() {
        return $this->_dbInUse->disconnect();
    }

    public function countQuery($sql){
        return $this->_dbInUse->countQuery($sql);
    }

    public function getNumRows(){
        return $this->_dbInUse->getNumRows();
    }

    public function getResults(){
        return $this->_dbInUse->getResults();
    }

    public function execs($a, $b = false){
        return $this->_dbInUse->execs($a, $b );
    }

    public function doQuery($sql){
        return $this->_dbInUse->doQuery($sql );
    }

    public function doCachedQuery($query, $life_seconds,$forcedMaster=false){
        self::checkQueryType($query,$forcedMaster);
        return $this->_dbInUse->doCachedQuery($query, $life_seconds);
    }

    public function exec($query, $returnOnError = false,$forcedMaster=false){
        self::checkQueryType($query,$forcedMaster);
        return $this->_dbInUse->exec($query, $returnOnError);
    }

    public function queryOne($query,$forcedMaster=false){
        self::checkQueryType($query,$forcedMaster);
        return $this->_dbInUse->queryOne($query);
    }

    public function queryRow($query,$forcedMaster=false){
        self::checkQueryType($query,$forcedMaster);
        return $this->_dbInUse->queryRow($query);
    }

    public function begin(){
        $this->_dbInUse=$this->_dbMaster;
        $this->_dbMaster->begin();
    }

    public function commit(){
        $this->_dbInUse=$this->_dbMaster;
        $this->_dbMaster->commit();
    }

    public function rollback(){
        $this->_dbInUse=$this->_dbMaster;
        $this->_dbMaster->rollback();
    }

    public function destroyCache($sql){
       $this->_dbInUse->destroyCache($sql);
    }
}