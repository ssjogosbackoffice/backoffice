<?php
require_once('MDB2.php');

define("CONNECT", 1);
define("ERR_DBSQL", 2);
define("ERR_DBNOCONNECTION", 3);


class DbConnect {
  public  $hands;
  private $result;
  public  $mdb2;
  
  public function DbConnect(){
    $this->Connect();
  }
  
  /**
   * Metodo per connettersi al database
   *
   * @return connection success
   */
  public function Connect(){
    $options = array(
                    'debug' => 2,
                    'result_buffering' => true,
                    'emulate_database' => true
                    );
 
    
    $this->mdb2 =& MDB2::connect(DBTYPE .'://'. DBUSER .':'. DBPASS .'@//'. DBHOST .'/'. DBSERVICE, $options);
    if (PEAR::isError($this->mdb2)) {
      die("Connection Error (" . $this->mdb2->getMessage() . "<br/>" . $this->mdb2->getUserInfo() . ")");
      return false;
    }else{
      $this->mdb2->setFetchMode(MDB2_FETCHMODE_ASSOC);
      $connection = $this->mdb2;      
      return true;
    }
    
  }
  public function Disconnect(){
  	$this->mdb2->disconnect();
  }
  /*
   * Metodo che restituisce i risultati trovati dalla funzione doQuery
   */
  public function getResults(){
    return $this->result;
  }
  
  /**
   * Metodo per fare una query, restituisce un valore booleano, 
   * e imposta la variabile result per essere restituita dalla Funzione getResult
   * @param string $sql
   */
  public function doQuery($sql){
    
    $res = $this->mdb2->query($sql);
    if (PEAR::isError($res)) {
      echo ("Query error (" . $res->getMessage() . "<br/>" . $res->getMessage() . ")<br/>Query = " . $sql . "<br/>" . $_SERVER["SCRIPT_NAME"]);
      exit; 
      return false;		    
    }
    
    $this->result = $res;
    return true;
  }  
  
  function execs($sql, $returnOnError = false) {
  	   $this->doQuery($sql);
	   $resultset = $this->getResults(); 

	   if($resultset) {
	     $this->AffectedRows = $this->result->numRows();
             if($this->result->numRows() > 0){
             	$rs = new RecordSet($this->result->fetchAll());
             }else{
             	$rs = new RecordSet(array());
             }
	     return $rs;   	   			
	   }
	   
	   if($returnOnError)
	     return $resultset;
	   else
	     $this->handleDatabaseError('', $sql);
	}

  public function escape($val,$type='text'){
    return $this->mdb2->quote($val,$type);
  }

  public function queryOne($query){
  	$res =  $this->mdb2->queryOne($query);
    if (PEAR::isError($res)) {
      echo ("Query error (" . $res->getMessage() . "<br/>" . $res->getMessage() . ")<br/>Query = " . $query . "<br/>" . $_SERVER["SCRIPT_NAME"]);
      exit; 
      return false;
    }
    return $res;
  }
  
  public function queryRow($query){
  	$res = $this->mdb2->queryRow($query);
    if (PEAR::isError($res)) {
      echo ("Query error (" . $res->getMessage() . "<br/>" . $res->getMessage() . ")<br/>Query = " . $query . "<br/>" . $_SERVER["SCRIPT_NAME"]);
      exit; 
      return false;
    }
    return $res;
  }
    
  public function exec($query){
  	$res = $this->mdb2->exec($query);
  	if (PEAR::isError($res)) {
      echo ("Query error (" . $res->getMessage() . "<br/>" . $res->getMessage() . ")<br/>Query = " . $query . "<br/>" . $_SERVER["SCRIPT_NAME"]);
      //exit; 
      return false;  		
  	}
  	return $res;
  }
  
  public function nextSequence ( $seq_name ) {
  	//echo ("next seq: " . $seq_name . "<br>");
  	$res = $this->queryOne( "select ". $seq_name .".nextval from dual" );
    if (PEAR::isError($res)) {
      //echo ("Query error (" . $res->getMessage() . "<br/>" . $res->getMessage() . ")<br/>Query = " . $query . "<br/>" . $_SERVER["SCRIPT_NAME"]);
      exit; 
      return -1;  		
    }
    return $res;
  }
  
  function lockRow($table_name, $field_filters){  
   	$sql = "select * from $table_name";
      $num_fields = count($field_filters);
      if ( $num_fields > 0 ) {  
      	$sql .= " where 1 = 1";
         foreach ( $field_filters as $field => $val ){  
         	$sql .= " and $field = $val";
         }
      }
      $sql .= " for update of punter_credit";
      $this->mdb2->exec($sql);
   }
   
/* Begin a transaction */
   function begin(){  
     return $this->mdb2->exec('begin');
   }
   
   /* Commit a transaction */
   function commit(){  
     return $this->mdb2->exec('commit');
   }
   
   
   function rollback(){
      return $this->mdb2->exec('rollback');
   }
 
  function escapeQuotesAndSlashes($str) {
    $str = mb_ereg_replace("\\\\", "\\\\", $str); //escape slashes
    return mb_ereg_replace("'", "\'", $str); //escape single quotes
  }

/**
 * @return string
 * @param string $str
 * @desc Prepares a string for an sql statement, escaping quotes and slashes where necessary
 */
function prepareString($str) {
  return "'".$this->escapeQuotesAndSlashes($str)."'"; //escape single quotes and slashes,
  //and add quotes around entire string   }
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

function getNumRows() {
  return $this->result->numRows();
}

/* Returns a count from a count type query */
    function countQuery ($sql) {
	  return $this->queryOne($sql);
	/*
        $rs = $this->exec($sql);
        if ( $rs->getNumRows() > 0 ) {
            $rec = $rs->next();
            if (isset($rec['count(*)'])) { //MySQL
              $rec['count'] = $rec['count(*)'];
            }
            elseif (isset($rec['computed'])) { //MSSQL
              $rec['count'] = $rec['computed'];
            }
            return $rec['count'];
        }
        return 0;
		*/
    }



}

class RecordSet 
{	var $Records;
	var $NumRows =0;
	var $CurrentIndex;
	var $RecPtr;
	var $ColSums;
	
	function RecordSet($arr){
		$this->Records = $arr;
		$this->NumRows = count($arr);
		$this->RecPtr = null;
		$this->CurrentIndex = -1;
	}
	
	function resetRecPtr(){	
		$this->RecPtr = null;
		$this->CurrentIndex = -1;				
	}
	
	function toLastRec(){	
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
	function & currentRef(){  
		if ( $this->CurrentIndex < 0 )
			return $this->next();
		return $this->Records[$this->CurrentIndex];
	}	
	
	/* returm previous record */
	function previous(){  	
		if ( $this->hasPrevious() ){	
			$this->RecPtr = $this->Records[(--$this->CurrentIndex)];
			return $this->current();
		}				
	}	
	
	/* return next record */
	function next(){
		if ( $this->hasNext() ){ 
			$this->RecPtr = $this->Records[(++$this->CurrentIndex)];
			return $this->current();
		}					
	}	
	
	/* return true if there is a record  after the current one */
	function hasNext(){  	
		return( $this->getNumRows() > 0 && $this->getCurrentIndex() < ($this->getNumRows()-1) );		
	}	

	/* return true if there is a record before the current one */
	function hasPrevious(){  	
		return( $this->CurrentIndex > 0 );
	}	
	
	/* return the record at th position in sequence of records  */
	function getRecordAt($index){	
		return $this->Records[$index];	
	}
	
	/* return the number of records in this recordse  */
	function getNumRows(){  
		return $this->NumRows; 
	}
			
	function appendRecords($arr){
	    //var_dump($arr);
		$this->Records = array_merge($this->Records,$arr);
		$this->NumRows = $this->getNumRows() + count($arr);	
	}
   
	function getKeys(){	
		if ( $this->NumRows > 0 )
			return array_keys($this->Records[0]);		
	}
	
	
	function serialize($serialize_filename){ 
   	  $byte_stream = serialize($this);
   	  file_put_contents  (cache_file_dir."/$serialize_filename"  , $byte_stream);
      //$f = fopen(,'w');
      //fputs($f,);
      //fclose($f);
   }
    
   
   function unserialize($serialize_filename){  
   	$filepath = cache_file_dir."/$serialize_filename";
      $byte_stream = file_get_contents($filepath);
      return unserialize($byte_stream);
   }
   
   function toRecordAt($index){   
   	$this->CurrentIndex = $index;        
   }
   
   function getCurrentIndex(){  
   	return $this->CurrentIndex;
   }
   
   function delete_records(){
        unset ($this->Records);       
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
      
   function printTable($recordset,$cols_arr, $post_vars="", $val_fmt_arr="", $cell_fmt_arr="", $sum_fmt_arr="", $excel_filename="", $header="", $width="", $start_row=1, $max_rows="20", $hide_msg=false, $form_name=""){
   if ( ! $max_rows )
	   $max_rows = 20;
	if ( empty($start_row) )
	   $start_row = 1;
    //sdfasdfdasgasdfasfd
	$num_cols = count($cols_arr);//number of columns
    $num_to_display = min($recordset->getNumRows()-$start_row+1, $max_rows); //num rows to display this page
	$rows_displayed = $start_row + $num_to_display - 1; //total rows displayed up to & including current page
	$next_display   = min($recordset->getNumRows() - $rows_displayed, $max_rows); //number of rows to display next
	//$rows_remaining = $this->RecordSet->getNumRows() - $start_row + $max_rows;

	if ( ! $header ) //if no heading supplied use the column field names by default	
        $header = "<tr valign=top><td class=label>".implode('</td><td class=label>', $cols_arr)."</td></tr>";
    $form_name = 'form'.randomString();
?>   
<p>
<form <? if ( $form_name ) echo "name=\"$form_name\"";?> action="<?=SELF;?>" method="POST">
<?    if ( is_array($post_vars) ) {
      	foreach( $post_vars as $key => $val ){
          if ( is_array($val) ){
            foreach( $val as $key2 => $val2 ){
              ?><input type=hidden name="<?=$key."[".$key2."]";?>" value="<?=$val2;?>">
<?          }
          }else{
	    ?><input type=hidden name="<?=$key;?>" value="<?=$val;?>">
<?        }
        }
      }    
?>    
<?ob_start()?>

<table cellpadding="0" cellspacing="1"  border="0" <?if ( $width) echo "width=$width"?>>
  <tr align="top" height="40"> 
    <td align="left">
<?		
if ( ! $hide_msg )
   if ( 1 == $num_records  )
     printInfoMessage("$num_rows record found");
   else
     printInfoMessage($recordset->getNumRows()." records found. Displaying $start_row - ".($start_row + $num_to_display-1));
?>
    </td>
    <td align="right">
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
<? $i=0;
   $recordset->toRecordAt($start_row-2);
   $num_rows_displayed = 0;
   
   while ( $recordset->hasNext() && $num_rows_displayed < $max_rows  ) //loop through records
   {	
       $rec = $recordset->next();
?> <tr valign=top>
<?    reset($cols_arr); 

      foreach ( $cols_arr as $key => $col_name )  //loop through columns
		{	$cell_fmt = ( $cell_fmt_arr[$col_name] ? $cell_fmt_arr[$col_name] : 'class="content"' );
		   
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
<?		}
?> </tr>
<?	  $num_rows_displayed++;  
   }
   $out = ob_get_clean();
if ( $sum_fmt_arr ){			//if column to be included in sum
   $recordset->resetRecPtr();
   while ( $recordset->hasNext()  ){ //loop through records
   	    $rec = $recordset->next();
        foreach ( $cols_arr as $key => $col_name ){  //loop through columns	
		   if ( $sum_fmt_arr[$col_name] )
		      if (empty($rec['pre_void']) && empty($rec['ctr_void']) && $rec['do_not_total'] != true)
                  $totals[$col_name] += $rec[$col_name];
		}
   }
   
   if ( $sum_fmt_arr['Total'] ){       
       
	   $totals['Total'] = '';
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
<?		
   foreach ( $totals as $col_name => $total_val ){  
     if ( $sum_fmt_arr[$col_name] ){	
       $function = create_function('$totals', $sum_fmt_arr[$col_name]);
       $ntotals[$col_name] = $function($totals);	//apply function to current record		                   
     }
   }
   
if($ntotals){?> <tr align="right"><td class="content"><span style="font-weight:bold"><?=implode('</span></td><td class="content"><span style="font-style:bold">', $ntotals) ?></td>

</tr>
</table>
<p>
<? 
}
   if ( $recordset->getNumRows() <= $max_rows ){  //if table broken up among pages
       ob_start();
?>
<tr>
<?    foreach ( $cols_arr as $key => $col_name ) {
        $cell_fmt = ( $cell_fmt_arr[$col_name] ? $cell_fmt_arr[$col_name] : 'class="content"' );
        $val = '';
	if ( $sum_fmt_arr[$col_name]  ) //if contains function code
        {
          $function = create_function('$totals', $sum_fmt_arr[$col_name]); //create function using code in sum_fmt_arr
	  $val = $function($totals);	//apply function to current record		                   
        } else {
   	   $val = '&nbsp;';
        }
   		 
?><td <?=$cell_fmt;?>><span style="font-weight:bold"><?=$val?></span></td>   
<?    }?>
</tr>
<?
      $out .= ob_get_clean();
    }
}
    echo $out;
?>
        </table>
     </td>
   </tr>
</table>
<?  if ( $max_rows < $recordset->getNumRows()) { ?>
<input type="hidden" name="start_row" value="<?=$start_row+$max_rows;?>">
<input type="hidden" name="num_rows" value="<?=$recordset->getNumRows();?>">
<input type="hidden" name="max_rows" value="<?=$max_rows;?>">
<input type="hidden" name="show_search_results" value="true">
<input type="hidden" name="member_type" value="<?=$GLOBALS['member_type'];?>">
<table cellpadding="4" cellspacing="1" border="0">
<tr>
  <td class=body colspan=<?=$num_fields;?>>
<?  if ( $start_row > 1 )
    {  
?>
<input type=button value="Previous <?=$max_rows;?> rows" onClick="this.form.start_row.value=<?=($start_row-$max_rows);?>;this.form.submit()">
<?  }
         
    if ( $next_display > 0 ){
      if ( 1 == $next_display ){
?>
<input type="submit" value="Last row">
<?    } else {?>
<input type="submit" value="Next <?=min($max_rows, $next_display);?> rows">
<?    }
    }?>
      </td>
   </tr>
</table>
<? } ?>
</form>
<?
      return $form_name;
   }
}
?>
