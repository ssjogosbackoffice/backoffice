<?php
define("CONNECT", 1);
define("ERR_DBSQL", 2);
define("ERR_DBNOCONNECTION", 3);

require_once 'MDB2.php';

if ( ! class_exists('CacheFileManager' ))
    include 'CacheFileManager.class.inc';

class DatabaseHandler {
    var $Connection;
    var $Host;
    var $Database;
    var $Username;
    var $Password;
    var $AllowSqlExec = true;
    var $NumRows = 0;
    var $AffectedRows = 0;
    var $startQuery = 0;
    var $DSN           = null;

    function DatabaseHandler($conn_str = "") {
        if (empty($conn_str) && defined('DSN')) {
            $this->DSN    = DSN;
        }
        else {
            $this->DSN    = $conn_str;
        }
        $this->Connection = null;
    }

    /* Connect to the database */
    function connect() {
        //$this->DSN="mysql://developer:TEAMDEVELOPER@10.15.0.12/casinogames_malta";
        //            mysql://egamesystem:qhqwqqwhdsq!@10.10.0.103/egamessystem

        $url = parse_url($this->DSN);
        
        if (isset($url["path"]) && $url["path"] !== '' && $url["path"]{0} == '/') {
            $url["path"] = substr($url["path"], 1);
        }
        $dsn = array('phptype'   => $url['scheme'],
            'hostspec'  => $url['host'],
            'username'  => $url['user'],
            'password'  => $url['pass'],
            'database'  => $url['path'],
            'port'      => $url['port']
        );
        $options = array(
            'debug' => 2,
            'result_buffering' => true,
//                    'emulate_database' => true
        );
//        var_dump($dsn);

        //echo "<pre>";
        //var_dump($dsn);
        // var_dump($options);
        //echo "</pre>";
        //$this->Connection  = &MDB2::factory($dsn,$options);

        $this->Connection  = &MDB2::connect($dsn, $options);
       

        if (PEAR::isError($this->Connection)) {
            $this->handleDatabaseError(ERR_DBCONNECT);
        }
        $this->Connection->loadModule('Extended', null, false);
        $this->Connection->setOption('persistent', false);
//        $this->Connection->setOption('multi_query', true);
        //$this->Connection->setOption('portability', MDB2_PORTABILITY_ALL ^ MDB2_PORTABILITY_FIX_CASE);
        $this->Connection->setOption('seqname_format', "%s");
//        $this->Connection->setOption('autofree', true);
        $this->Connection->setFetchMode(MDB2_FETCHMODE_ASSOC);
        $this->Connection->setCharset('utf8');
        return $this->Connection;
    }

    /* Disconnect from the database */
    function disconnect() {
        if ( $this->Connection ){
            $this->Connection=null;
        }
        //$this->Connection->disconnect();
    }

    /* Check if database is connected */
    function isConnected() {
        return is_a($this->getConnection(), "MDB2_Driver_Common");
    }

    /* Begin a transaction */
    function begin() {
        if ($this->getConnection()->supports('transactions')) {
            $this->getConnection()->beginTransaction();
        }
    }

    /* Commit a transaction */
    function commit() {
        if ($this->getConnection()->in_transaction)
            $this->getConnection()->commit();
    }

    function rollback() {
        if ($this->getConnection()->in_transaction)
            $this->getConnection()->rollback();
    }

    /* Return connection resource */
    function getConnection() {
        return $this->Connection;
    }

    function escape($str) {
        return $this->Connection->escape($str);
    }

    function quote($str) {
        return $this->Connection->quote($str);
    }

    function printHeaderLog($sql){
        global $dbLog;
        $dbLog->info("");
        $dbLog->info("--------QUERY INFO---------");
        $dbLog->info("Page in execution: " . $_SERVER["PHP_SELF"]);
        $dbLog->info("Query to execute: ". $sql);
        $this->startQuery = microtime(true);
    }
    function printFooterLog(){
        global $dbLog;
        $stop = microtime(true);
        $executeTime = $stop - $this->startQuery;
        $dbLog->info("Query executed in ".$executeTime . " milliseconds");
        $dbLog->info("------QUERY END INFO---------");
    }

    /* Execute a sql statement, return resultset */
    function exec($sql, $returnOnError = false) {
        global $dbLog;
        if(!$this->isConnected())
            $this->handleDatabaseError(ERR_DBNOCONNECTION, $sql);
        if (preg_match("#^\s*(INSERT|UPDATE|DELETE|REPLACE) #i", $sql)) {
            //$this->printHeaderLog($sql);
            $this->AffectedRows = $this->Connection->exec($sql);
            if(PEAR::isError($this->AffectedRows)){

               // $dbLog->info($this->AffectedRows->getUserInfo());
                error_log('Query error:'.$this->AffectedRows->getUserInfo());
                //error_log($this->AffectedRows->getDebugInfo());

                return false;
            }
            //$this->printFooterLog();
            return ($this->AffectedRows === 0) ? true : $this->AffectedRows;
        }
        else {
            //$this->printHeaderLog($sql);
            $rows = $this->Connection->queryAll($sql);
            
            //$this->printFooterLog();
            if (PEAR::isError($rows)) {
                if ($returnOnError)
                    return $rows;
                else
                    error_log('Query error:'.$rows->getUserInfo());
                    $this->handleDatabaseError('', $sql);
            }
            else if (is_array($rows)) {
                $rs = new RecordSet($rows);
                return $rs;
            }
        }
    }


    /* Prints error details of a database error */
    function handleDatabaseError($error_code="", $sql="") {
        global $lang,$dbLog;
        $this->disconnect();
        //	ob_end_clean();

        define('DEBUG', true);
        if (DEBUG) {
            if ( $error_code == ERR_DBNOCONNECTION ) {
                echo "No Database Connection! Connect first!<P>";
            }
            elseif ( $error_code == ERR_DBCONNECT ) {
                echo "Could not connect to Database<P>";
            }
            else {
                echo "<div class='errorbold'>".$lang->getLang("Query error! Please contact assistance.Thank you!")."</div>";

                $dbLog->info("SQL ERROR:"."'".$sql."'");
            }
        }
        else {
            header ('Location: ' . secure_host .'/system_error.php');
        }
        exit;
    }

    function findWithFilter($table_name, $fields_arr, $all_req=true, $like_arr="") {
        global $dbh;

        $like_arg = is_array($like_arr);
        $sql  = "SELECT count(*) FROM $table_name";
        if ( $all_req ) {
            $sql .= ' WHERE 1=1';
            $op = 'AND';
        }
        else {
            $sql .= ' WHERE 1=0';
            $op = 'OR';
        }

        foreach ($fields_arr as $col_name=>$value ) {
            if ( $like_arg ) {
                if ( in_array($col_name, $like_arg) )
                    $sql .= "$op $col_name LIKE ('%$value%')";
            }
            else {
                $sql .= "$op $col_name = '".addslashes($value)."'";
            }
        }
        return $this->countQuery($sql);
    }

    function getAffectedRows() {
        return $this->AffectedRows;
    }


    function lockRow($table_name, $field_filters = array()) {
        /* PGSQL + MSSQL */
        $sql = "SELECT * FROM $table_name";
        $num_fields = count($field_filters);

        if ( $num_fields > 0 ) {
            $sql .= " WHERE 1 = 1";

            foreach ( $field_filters as $field => $val ) {
                $sql .= " AND $field = $val";
            }
        }

        /* PGSQL */
        //$sql .= " FOR update OF punter_credit";
        /* MYSQL */
        //$sql = "LOCK TABLES $table_name WRITE";

        $this->exec($sql);
    }



    function nextSequence ( $sequence_name ) {
        $res = $this->Connection->queryOne("SELECT last_seq_id(UPPER(".$this->prepareString($sequence_name)."))");
        return $res;
        //return $this->Connection->nextId($sequence_name);
    }

    function getBeforeId( $table ) {
        return $this->Connection->extended->getBeforeID($table);
    }

    function getAfterId ( $id, $table ) {
        return $this->Connection->extended->getAfterID($id, $table);
    }

    function lastInsertId( $table, $id ) {
        return $this->Connection->lastInsertID( $table, $id );
    }

    function setLimit($num, $srow = 0) {
        return $this->Connection->setLimit($num, $srow);
    }

    /**
     * @return string
     * @param string $str
     * @desc Prepares a string for an sql statement, escaping quotes and slashes where necessary
     */
    function escapeQuotesAndSlashes($str) {
        $str = mb_ereg_replace("\\\\", "\\\\", $str); //escape slashes
        return mb_ereg_replace("'", "''", $str); //escape single quotes
    }


    /**
     * @return string
     * @param string $str
     * @desc Prepares a string for an sql statement, escaping quotes and slashes where necessary
     */
    function prepareString($str) {
        return "'".$this->escapeQuotesAndSlashes($str)."'"; //escape single quotes and slashes,
        //and add quotes around entire string	}
    }

    /**
     * @return string
     * @param string $str
     * @desc Define the sql segment that extracts the date part from a column
     */
    function sql_extract_date($col_name) {
        return 'substr(' . $col_name . ', 0, 10)';
    }


    /**
     * @return string
     * @param string $str
     * @desc Returns a postgresql format specific date
     */
    function date_today() {
        return date('Y-m-d');
    }


    /**
     * @return string
     * @param string $str
     * @desc Returns a postgres format specific timestamp
     */
    function timestamp_now() {
        return date('Y-m-d H:i:s');
    }

    function timestamp_today() {
        return date('Y-m-d') . ' 00:00:00';
    }

    function time_now() {
        return date('H:i:s');
    }

    function seconds_to_timestamp ($seconds) {
        return date('Y-m-d H:i:s', $seconds);
    }

    function queryOne($sql) {
        if (!$this->isConnected()) {
            $this->handleDatabaseError(ERR_DBNOCONNECTION, $sql);
        }
        //$this->printHeaderLog($sql);
        $rows = $this->Connection->queryOne($sql);
        //$this->printFooterLog();
        if (PEAR::isError($rows)) {
            if (isset($returnOnError) && !$returnOnError)
                $this->handleDatabaseError('', $sql);
        }
        return $rows;
    }

    function countQuery($sql) {
        return $this->queryOne($sql);
    }


    function getNumRows() {
        return $this->result->numRows();
    }

    function destroyCache($sql){
        $file_name = md5($sql);
        $file_path = SQL_CACHE_DIR . '/' . $file_name . ".cache";

        if(file_exists($file_path)){
            unlink($file_path);
        }
    }

    function doCachedQuery($sql, $life_seconds){
        $file_name = md5($sql);
        $file_path = SQL_CACHE_DIR . '/' . $file_name . ".cache";
        //$this->destroyCache($sql);
        if(file_exists($file_path)){
            //Il file di cache esiste
            if(time() - filemtime($file_path) < $life_seconds){
                $file    = fopen($file_path, "r");
                $content = null;
                if(filesize($file_path)>0){
                    $content = fread($file, filesize($file_path));
                }
                return new RecordSet(unserialize($content));
            }
        }
        //Do query
        $rs = $this->exec($sql);
        file_put_contents($file_path, serialize($rs->Records));
        return $rs;
    }


    //DbConnect compats
    var $result;
//    function Connect()    { return ($this->connect() != null); }
//    function Disconnect() { $this->disconnect(); }
    function getResults() { return $this->result; }
    function execs($a, $b = false){ return $this->exec($a, $b); }
    function doQuery($sql) {
        global $lang;
        if(!$this->isConnected())
            $this->handleDatabaseError(ERR_DBNOCONNECTION, $sql);
        $res = $this->Connection->query($sql);

    
	if (PEAR::isError($res)) {
	    $this->handleDatabaseError('', $sql);
            //echo ("Query error (" . $res->getMessage() . "<br/>" . $res->getMessage() . ")<br/>Query = " . $sql . "<br/>" . $_SERVER["SCRIPT_NAME"]);
            exit;
            return false;
        }
        $this->result = $res;
        return true;
    }
    
    function queryRow($query) {
        global $lang;
        //$this->printHeaderLog($query);
        $res = $this->Connection->queryRow($query);
        //$this->printFooterLog();
        if (PEAR::isError($res)) {
            $this->handleDatabaseError('', $query);
            //echo ("Query error (" . $res->getMessage() . "<br/>" . $res->getMessage() . ")<br/>Query = " . $query . "<br/>" . $_SERVER["SCRIPT_NAME"]);
            exit;
            return false;
        }
        return $res;
    }
}

class RecordSet
{
    var $Records;
    var $NumRows =0;
    var $CurrentIndex;
    var $RecPtr;
    var $ColSums;

    function RecordSet($arr) {
        $this->Records = $arr;
        $this->NumRows = count($arr);
        $this->RecPtr = null;
        $this->CurrentIndex = -1;
    }

    function resetRecPtr() {
        $this->RecPtr = null;
        $this->CurrentIndex = -1;
    }

    function toLastRec() {
        $this->CurrentIndex = $this->getNumRows()-1;
        $this->RecPtr = $this->Records[$this->getCurrentIndex()];
        return $this->current();
    }

    /* return current record */
    function current() {
        if ( $this->CurrentIndex < 0 )
            return $this->next();
        return $this->Records[$this->getCurrentIndex()];
    }

    /* Return a reference to the current record */
    function & currentRef() {
        if ( $this->CurrentIndex < 0 )
            return $this->next();
        return $this->Records[$this->CurrentIndex];
    }

    /* returm previous record */
    function previous() {
        if ( $this->hasPrevious() ) {
            $this->RecPtr = $this->Records[(--$this->CurrentIndex)];
            return $this->current();
        }
    }

    /* return next record */
    function next() {
        if ( $this->hasNext() ) {
            $this->RecPtr = $this->Records[(++$this->CurrentIndex)];
            return $this->current();
        }
    }

    /* return true if there is a record  after the current one */
    function hasNext() {
        return( $this->getNumRows() > 0 && $this->getCurrentIndex() < ($this->getNumRows()-1) );
    }

    /* return true if there is a record before the current one */
    function hasPrevious() {
        return( $this->CurrentIndex > 0 );
    }

    /* return the record at th position in sequence of records  */
    function getRecordAt($index) {
        return $this->Records[$index];
    }

    /* return the number of records in this recordse  */
    function getNumRows() {
        return $this->NumRows;
    }

    function appendRecords($arr) {
        $this->Records = array_merge($this->Records,$arr);
        $this->NumRows = $this->getNumRows() + count($arr);
    }


    function getKeys() {
        if ( $this->NumRows > 0 )
            return array_keys($this->Records[0]);
    }


    function serialize($serialize_filename) {
        $byte_stream = serialize($this);
        $f = fopen(cache_file_dir."/$serialize_filename",'w');

        fputs($f,$byte_stream);
        fclose($f);
    }


    function unserialize($serialize_filename) {
        $filepath = cache_file_dir."/$serialize_filename";
        $byte_stream = file_get_contents($filepath);
        return unserialize($byte_stream);
    }

    function toRecordAt($index) {
        $this->CurrentIndex = $index;
    }

    function getCurrentIndex() {
        return $this->CurrentIndex;
    }

    function delete_records() {
        unset ($this->Records);
    }

    function toArray() {
        return $this->Records;
    }
}

class QueryPrinter{
    var $RecordSet;
    var $OutputType;


    function QueryPrinter($recordset){
        $this->RecordSet = $recordset;
    }

    function formatFields($fmt_arr)
    {  $this->RecordSet->resetRecPtr();

        while ( $this->RecordSet->next() )
        {  $rec = & $this->RecordSet->currentRef();
            reset($fmt_arr);
            foreach ($fmt_arr as $col_name => $function_code )
            {  $func = create_function($rec, $function_code);
                $rec[$col_name] = $func;
            }
        }
    }

    function dumpToExcelFile($recordset,$cols_arr, $val_fmt_arr, $excel_filename){
        $f = fopen(temp_dir."/$excel_filename", 'w');
        $recordset->resetRecPtr();
        fputs($f, "<table>");
        fputs($f, "<tr><th>".implode('</th><th>', $cols_arr)."</th></tr>\n");
        while ( $rec = $recordset->next() )
        {	//Apply formatting functions to column values, if present
            reset($cols_arr);
            foreach ( $cols_arr as $col_index ){
                if ( $val_fmt_arr[$col_index] ){ //if formatting code exists for current row,column
                    $function = create_function('$rec,$index', $val_fmt_arr[$col_index]); //create anonymous function
                    $val = $function($rec,$col_index);	//apply function to current record
                } else {
                    $val = $rec[$col_index];
                }
                $out_line_arr[$col_index] = $val; //grab value of column we want to output
            }
            // highlight voided entries
            if(!empty($rec['pre_void']) || !empty($rec['ctr_void'])  )
                fputs($f, "<tr><td style=\"background-color: #FF9999;\">".implode('</td><td style="background-color: #FF9999;">', $out_line_arr)."</td></tr>\n");
            else
                fputs($f, "<tr><td>".implode('</td><td>', $out_line_arr)."</td></tr>\n");
        }
        fputs($f, "</table>");
        fclose($f);
    }


    function printPdf($recordset,$cols_arr,$val_fmt_arr,$pdf_filename,$header=false,$footer=false){
        $recordset->resetRecPtr();
        include  "FPDF/mpdf.php";
        $pdf = new mPdf('c','A4-L',2);
        $pdf->useSubstitutions = false;
        $pdf->simpleTables = true;
        $stylesheet= file_get_contents("media/table.css"); // external css
        $stylesheet.= file_get_contents("media/pdfStyle.css");
        $pdf->WriteHTML($stylesheet,1);
        $data="";
        if($header){
            $data.=$header;
        }
        $data.= "<table class='display table table-bordered table-condensed' cellpadding='0' cellspacing='1'  border='0'>".
            "<thead><tr><th>".implode('</th><th>', $cols_arr)."</th></tr></thead>";
        while ( $rec = $recordset->next() )
        {
            reset($cols_arr);
            foreach ( $cols_arr as $col_index ){
                if ( $val_fmt_arr[$col_index] ){
                    $function = create_function('$rec,$index', $val_fmt_arr[$col_index]);
                    $val = $function($rec,$col_index);	//apply function to current record
                } else {
                    $val = $rec[$col_index];
                }
                $out_line_arr[$col_index] = $val;
            }
            $data.= "<tr><td>".implode('</td><td>', $out_line_arr)."</td></tr>\n";
        }
        $data.="</table>";
        if($footer){
            $data.=$footer;
        }
        $data= preg_replace('/<a href=\"(.*?)\">(.*?)<\/a>/', "\\2", $data);
        $pdf->WriteHTML($data,2);
        $pdf->Output(temp_dir."/$pdf_filename",'F');
    }

    //todo(Here!)
    function printTable($recordset,$cols_arr, $post_vars="", $val_fmt_arr="", $cell_fmt_arr="", $sum_fmt_arr="", $excel_filename="", $header="", $width="", $start_row=1, $max_rows="20", $hide_msg=false, $form_name="",$pdf_filename=''){
        if ( ! $max_rows )
            $max_rows = 20;
        if ( empty($start_row) )
            $start_row = 1;

        $num_cols       = count($cols_arr);//number of columns
        $num_to_display = min($recordset->getNumRows()-$start_row+1, $max_rows); //num rows to display this page
        $rows_displayed = $start_row + $num_to_display - 1; //total rows displayed up to & including current page
        $next_display   = min($recordset->getNumRows() - $rows_displayed, $max_rows); //number of rows to display next
        //$rows_remaining = $this->RecordSet->getNumRows() - $start_row + $max_rows;

        if ( ! $header ) //if no heading supplied use the column field names by default
            $header = "<tr valign=top><td class=label>".implode('</td><td class=label>', $cols_arr)."</td></tr>";
        $form_name = 'form'.randomString();
        ?>
        <p>
        <form <? if ( $form_name ) echo "name=\"$form_name\"";?> action="<?=SELF.'?'.$_SERVER['QUERY_STRING'];?>" method="POST">
        <?    if ( is_array($post_vars) ) {
            foreach( $post_vars as $key => $val ){
                if ( is_array($val) ){
                    foreach( $val as $key2 => $val2 ){
                        ?><input type=hidden name="<?=$key."[".$key2."]";?>" value="<?=$val2;?>">
                    <?php       }
                }
                else{
                    ?><input type=hidden name="<?=$key;?>" value="<?=$val;?>">
                <?        }
            }
        }
        ?>
        <?ob_start()?>
        <style>
            .odd td{
                background-color:#FFF;
            }
            .even td{
                background-color:#EEE;
            }
        </style>
        <table cellpadding="0" cellspacing="1"  border="0" <?if ( $width) echo "width=$width"?>>
            <tr align="top" height="40">
                <td align="left">
                    <?php
                    if ( ! $hide_msg )
                        if (isset($num_records) && 1 == $num_records  )
                            printInfoMessage(isset($num_rows) ? $num_rows : 0 . " record found");
                        else
                            printInfoMessage($recordset->getNumRows()." records found. Displaying $start_row - ".($start_row + $num_to_display-1));
                    ?>
                </td>
                <td align="right">
                    <? if ( $pdf_filename ) { ?>
                        <a href="<?=temp_download_url."/$pdf_filename"?>" target="_blank">
                            <img alt="Download Pdf" src="<?=image_dir?>/icon_download.jpg" border=0></a>
                        <?printClassLink(temp_download_url."/$pdf_filename", "Download Pdf","","_blank");?><br />
                    <? } else {?>
                        &nbsp;
                    <? } ?>

                    <? if ( $excel_filename ) { ?>
                        <a href="<?=temp_download_url."/$excel_filename?".mktime()?>">
                            <img alt="Download as Excel File" src="<?=image_dir?>/icon_download.jpg" border=0></a>
                        <?printClassLink(temp_download_url."/$excel_filename?".mktime(), "Download Excel File");?>
                    <? } else {?>
                        &nbsp;
                    <? } ?>
                </td>
            </tr>
            <tr>
                <td colspan="2">
                    <table bgcolor="#a99a9" cellpadding="4" cellspacing="1" border="0" <?if ( $width) echo "width=$width"?>>
                        <?=$header?>
                        <?php
                        $i=0;
                        $recordset->toRecordAt($start_row-2);
                        $num_rows_displayed = 0;

                        while ( $recordset->hasNext() && $num_rows_displayed < $max_rows  ) //loop through records
                        {
                            $rec = $recordset->next();
                            ?> <tr valign=top class="<?=(($i++%2==0)?("odd"):("even"))?>">
                            <?php reset($cols_arr);

                            foreach ( $cols_arr as $key => $col_name )  //loop through columns
                            {
                                $cell_fmt = ( $cell_fmt_arr[$col_name] ? $cell_fmt_arr[$col_name] : 'class="content"' );

                                if ( $val_fmt_arr[$col_name] ){ //if formatting code exists for current row,column
                                    $function = create_function('$rec', $val_fmt_arr[$col_name]); //create anonymous function
                                    $val = $function($rec);	//apply function to current record
                                }
                                else
                                    $val = $rec[$col_name];

                                if(!empty($rec['pre_void']) || !empty($rec['ctr_void']))
                                    $cell_fmt .= ' style="background-color: #FF9999;"';
                                ?>
                                <td <?=$cell_fmt?>><?=$val?></td>
                            <?php }
                            ?> </tr>
                            <?php $num_rows_displayed++;
                        }
                        $out = ob_get_clean();
                        if ( $sum_fmt_arr ) {			//if column to be included in sum
                        $recordset->resetRecPtr();
                        while ( $recordset->hasNext()  ){ //loop through records
                            $rec = $recordset->next();

                            foreach ( $cols_arr as $key => $col_name ){  //loop through columns
                                if ( $sum_fmt_arr[$col_name] )
                                    if (empty($rec['pre_void']) && empty($rec['ctr_void']) && $rec['do_not_total'] != true){
                                        if (isset($totals))
                                            $totals[$col_name] += $rec[$col_name];
                                        //echo "Adding " . $rec[$col_name] . " to " . $col_name . "<br/>\n";
                                    }
                            }
                        }
                        //exit();
                        if ( $sum_fmt_arr['total'] ) {
                            $totals['total'] = '';
                        }

                        if( is_array($totals) )
                            reset($totals);
                        else
                            $totals = array();
                        $total_hd_arr = array_keys($totals);

                        foreach( $total_hd_arr as $key => $val ){
                            if ( strtolower(substr($val, 0, 6)) != 'total' )
                                $total_hd_arr[$key] = "Total $val";
                            else
                                $total_hd_arr[$key] = $val;
                        }
                        ?>
                        <table cellpadding="4" cellspacing="1" border="0">
                            <tr>
                                <td class="label"><?=implode('</td><td class="label">', $total_hd_arr)?></td>
                            </tr>
                            <?php
                            foreach ( $totals as $col_name => $total_val ){
                                if ( $sum_fmt_arr[$col_name] ){
                                    $function = create_function('$totals', $sum_fmt_arr[$col_name]);
                                    $ntotals[$col_name] = $function($totals);	//apply function to current record
                                }
                            }

                            if($ntotals){
                            ?> <tr align="right"><td class="content"><span style="font-weight:bold"><?=implode('</span></td><td class="content"><span style="font-style:bold">', $ntotals) ?></td>
                            </tr>
                        </table>
                            <?php
                            }
                            if ( $recordset->getNumRows() <= $max_rows ){  //if table broken up among pages
                                ob_start();
                                ?>
                                <tr>
                                    <?php
                                    foreach ( $cols_arr as $key => $col_name ) {
                                        $cell_fmt = ( $cell_fmt_arr[$col_name] ? $cell_fmt_arr[$col_name] : 'class="content"' );
                                        $val = '';
                                        if ( $sum_fmt_arr[$col_name]  ) { //if contains function code
                                            $function = create_function('$totals', $sum_fmt_arr[$col_name]); //create function using code in sum_fmt_arr
                                            $val = $function($totals);	//apply function to current record
                                        }
                                        else {
                                            $val = '&nbsp;';
                                        }

                                        ?><td <?=$cell_fmt;?>><span style="font-weight:bold"><?=$val?></span></td>
                                    <?    }?>
                                </tr>
                                <?php
                                $out .= ob_get_clean();
                            }
                            }
                            echo $out;
                            ?>
                    </table>
                </td>
            </tr>
        </table>
        <?php
        if ( $max_rows < $recordset->getNumRows()) {
            ?>
            <input type="hidden" name="start_row" value="<?=$start_row+$max_rows;?>">
            <input type="hidden" name="num_rows" value="<?=$recordset->getNumRows();?>">
            <input type="hidden" name="max_rows" value="<?=$max_rows;?>">
            <input type="hidden" name="show_search_results" value="true">
            <input type="hidden" name="member_type" value="<?=$GLOBALS['member_type'];?>">
            <table cellpadding="4" cellspacing="1" border="0">
                <tr>
                    <td class=body colspan=<?=$num_fields;?>>
                        <?php
                        if ( $start_row > 1 ) {
                            ?>
                            <input type=button value="Previous <?=$max_rows;?> rows" onClick="this.form.start_row.value=<?=($start_row-$max_rows);?>;this.form.submit()">
                        <?php
                        }

                        if ( $next_display > 0 ){
                            if ( 1 == $next_display ){
                                ?>
                                <input type="submit" value="Last row">
                            <?php
                            }
                            else {
                                ?>
                                <input type="submit" value="Next <?=min($max_rows, $next_display);?> rows">
                            <?php
                            }
                        }
                        ?>
                    </td>
                </tr>
            </table>
        <? } ?>
        </form>
        <?php
        return $form_name;
    }
}
?>
