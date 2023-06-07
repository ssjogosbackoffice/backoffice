<?php
if ( ! class_exists('CacheFileManager') )
{
    class CacheFileManager
    {
        var $C_NAME;
        var $filePtr;
        var $filePath;
        var $mode;
        var $path = cache_file_dir;
        var $FileName;

        //constructor opens cache file - the name of which is passed as an argument
        function CacheFileManager ($cacheFileName)
        {
        }

        function setMode($mode)
        {  $this->mode=$mode;
        }

        function getMode()
        {  return $this->mode;
        }

        function setCName($cname)
        {  $this->C_NAME=$cname;
        }

        function getCName()
        {  return $this->C_NAME;
        }

        //open file: file name should have been set by constructor
        function open() {
            $file_path = $this->getFilePath();
            if ( !$file_path )
            {
                $this->showError("File path not set");
                return false;
            }

            if ( $fp = fopen($file_path, $this->getMode()) )
              return $this->filePtr = $fp;
            else
              $this->showError("Could not open \"$file_path\"",true);
        }

        //close file pointer
        function close ($unlink=false)
        { 
            if ( $fp = $this->getFilePtr() ) {
                fclose($fp);
                if ( $unlink )
                  $this->unlinkFile();
            }
        }

        //set the file path class variable
        function setFilePath($cacheFileName) {
            if ( isBlank($cacheFileName) )
            {
                $this->showError("Cannot set cache file path. blank file name argument supplied",true);
                return;
            }
            $this->filePath = $this->path."/$cacheFileName";
        }


        //returns the file pointer
        function getFilePtr()
        {  return $this->filePtr; }

        function getFilePath()
        {  return $this->filePath; }

        function showError ($error_str, $die=false) {
            if ( $die )
              die("<font color=\"red\">".$this->getCName().": $error_str</font>");
            echo("<font color=\"red\">".$this->getCName().": $error_str</font>");
        }

        function isEof()
        {  return feof($this->getFilePtr()); }

        function resetFile()
        {  return rewind($this->getFilePtr()); }


        function unlinkFile()
        {  unlink($this->getFilepath()); }

        function isOpen()
        {  return $this->getFilePtr(); }

        function exists()
        {  return file_exists($this->filePath); }
}


class CacheFileReader extends CacheFileManager
{
    var $Delimiter;
    var $HeadingsArr;
    var $NumLines;

    //constructor opens cache file - the name of which is passed as an argument
    function CacheFileReader ($cache_filename, $delimiter='|') {
        $this->setCName("Cache File Reader");
        $this->setFilepath($cache_filename);
        $this->setMode('r');
        $this->Delimiter = $delimiter;
    }

    function readLine() {
        $fp = $this->getFilePtr();
        if ( !$fp ) {
            $this->showError("File Pointer not set");
            return;
        }
        if ( $this->isEof($fp) ) {
            return;
        }
        $line = substr(fgets($fp, 4096), 0, -1);
        return $line;
    }

    function countLines() {
        $count = 0;
        $this->resetFile();
        while ( $this->readLine() )
          $count++;
        $this->resetFile();
        $this->NumLines = $count;
        return $count;
    }


    function readLineToArray() {
        $line = $this->readLine();
        return explode($this->Delimiter, $line);
    }

   
    function cr_advance($num_lines) {
        for ( $i=0; $i<$num_lines; $i++ )
        $this->readLine();
    }

    /** assume reading marker at BOF **/
    function cr_getHeadings() {
        if ( ! $this->NumLines )
        $this->countLines();
        if ( $this->NumLines > 0 )
        $this->Headings = $this->readLineToArray();
    }

    /** assume reading marker at BOF **/
    /*function getSerializedHeadings($headings_arr) {
      $filename_arr = explode('$th');
      if ( file_exists($this-> )
        $this->Headings = $headings_arr;
    }*/

   
   function readLineToArrayAssoc() {
      $arr = $this->readLineToArray();
      foreach ( $arr as $index => $value ) {
          $assoc_arr[$this->Headings[$index]] = $value; 	
      }
      return $assoc_arr;
   }

   
   function printFromCacheFile($exclude_cols="", $post_vars="", $excel_filename="", $max_rows=60, $num_rows_displayed=0, $row_format_arr="", $heading_format_arr="", $td_format_arr, $sum_arr="")
   {
       global $totals_aux;
       $this->open();
       $num_rows = $this->countLines()-1;

       if ( $num_rows == 0 )
         printInfoMessage("No Records found");
       else {
         if ( $num_rows_displayed == $num_rows )
           die ("No (more) records to display");
         if ( ! $this->HeadingsArr )
           $this->cr_getHeadings(); //first line in cache file

         $this->cr_advance($num_rows_displayed);  //advance past records that were already displayed
         $last_display = $num_rows_displayed+$max_rows;
         $num_cols_displayed = count($this->Headings) - count($exclude_cols);
         printInfoMessage("$num_rows records found");
         ob_start();
?>
<p>
<form name='recorddisplayform' action="<?=SELF;?>" method="POST">
<?php
   if ( is_array($post_vars) ) {
     foreach( $post_vars as $key => $val ) {
?>
       <input type="hidden" name="<?=$key;?>" value="<?=$val;?>">
<?php
     }
   }
?>
<table cellpadding="4" cellspacing="1"  border="0">
  <tr align="top" height="40"> 
    <td class=body colspan=<?=($x=ceil($num_cols_displayed/2))?>>
      <div class=small>
        <b>Displaying rows <?=$num_rows_displayed+1;?> - <?=min($last_display, $num_rows);?> of <?=$num_rows;?></b>
      </div>
    </td>
    <td align=right class=body colspan=<?=count($this->Headings)-$x?>>
<? if ( $excel_filename ) { ?>
      <a href="<?=temp_download_url."/$excel_filename?".mktime()?>"><img alt="Download as Excel File" src="<?=image_dir?>/icon_download.jpg" border=0></a> <?printClassLink(temp_download_url."/$excel_filename?".mktime(), "Download Excel File", "medium");?>
<? } ?>
    </td>
  </tr>
  <tr valign=top>
<? foreach ( $this->Headings as $key => $heading ) // print headings for table
   {
     if ( !in_array($heading, $exclude_cols) ) {
       if ( $hfunc = $heading_format_arr[$heading] )
         $heading = $hfunc($heading);
?>    
       <td class=label><?=$heading;?></td>
<?   }
   }
?> </tr>
<? 
  $i=0;
  $num_cols = count($this->Headings);	
  //$row = $this->readLineToArrayAssoc();
  while ( $i < $max_rows && $num_rows_displayed < $num_rows ) {
    $row = $this->readLineToArrayAssoc();
?>
    <tr valign=top>
<?
      $j=0;
      foreach ( $row as $field_name => $field_value ) {
        $attr_str = 'class=content';
        if ( ! in_array($field_name, $exclude_cols) ) //if field not excluded form output
	{
          $cols_displayed++;
          if ( $sum_arr[$field_name] ) {
	    $totals[$j]['field_name'] = $field_name;
	    $totals[$j]['sum'] += $field_value;	
	    $totals_aux[$field_name] += $field_value;
	  } else {
	    $totals[$j] = "";
          }
          if  ( $attr_spec = $td_format_arr[$field_name] )
	    $attr_str = $td_format_arr[$field_name];
	  if ( $func = $row_format_arr[$field_name] ) {
	    $arg_vals = array();
	    if ( is_array($func) ) {
   	      $realfunc = $func['func'];
	      $args = explode(',', $func['args']);							
	
	      foreach ($args as $_key => $_val ) {
	        $arg_vals[] = $row[$_val];
	      }						
	      $field_value = $realfunc($arg_vals);
	    } else {
	      $field_value = $func($field_value);
            }
    	  }
?>
          <td <?=$attr_str;?>><?=$field_value?></td>
<?				
	   $j++;				 
	}
      }
?> </tr>
<?    $num_rows_displayed++;
      $i++;	
   }
   $contents = ob_get_clean();
   
if ( $sum_arr )
{
	if ( $num_rows <= $max_rows )
	{	
	    echo $contents;
	?>

<tr>
<?		$num_cols_displayed = count($row) - count($exclude_cols);
		foreach ( $totals as $index => $arr)
		{	if ( $arr )
			{	$func = $sum_arr[($arr['field_name'])];
	?>
				<td class=content align=right><b><?=$func($arr['sum']);?></b></td>	
	<?		}
			else
			{
	?>
				<td class=content align=right>&nbsp;</td>	
	<?		
			}
		}
	}
	else
	{	unset($totals);
		$this->resetFile();
		$row = $this->readLine(); //heading line
		for ( $i=0; $i<$num_rows; $i++ ) 
   		{	$j=0;
   			$row = $this->readLineToArrayAssoc();
	   		
   			foreach ( $row as $field_name => $field_value )
			{	if ( $sum_arr[$field_name] )			
				{	$totals[$j]['field_name'] = $field_name;
					$totals[$j]['sum'] += $field_value;	
					$totals_aux[$field_name] += $field_value;
				}
				else 
					$totals[$j] = "";
				$j++;
	   		}
	   		
   		}

?>
<p>

		<table cellpadding=4 cellspacing=1 border=0>
<?		foreach ( $sum_arr as $key => $sumfunc )
		{	reset($totals);
			foreach ( $totals as $index => $sum )
			{	if ( $key == $sum['field_name'] )
				{ 	$thds[]	= "Total $key";
					$tval[] =  $sumfunc($sum['sum']);
				}								
			}
		}		
		echo "<tr><td class=label>".implode('</td><td class=label>', $thds)."</td></tr>";
		echo "<tr align=right><td class=content>".implode('</td><td class=content>', $tval)."</td></tr>";
?>		</table>

<?=$contents?>
<?php
	}
}
else
	echo $contents;
?>

</table>

<?php  if ( $max_rows < $num_rows ) { ?>
    <input type=hidden name=num_rows_displayed value=<?=$num_rows_displayed;?>>
    <input type=hidden name=num_rows value=<?=$num_rows;?>>
    <input type=hidden name=max_rows value=<?=$max_rows;?>>
    <input type=hidden name=show_search_results value=yes>
    <input type=hidden name=member_type value=<?=$GLOBALS['member_type'];?>>
    <input type="hidden" name="show_search_results"	value="yes">
<table cellpadding=4 cellspacing=1 border=0>
   <tr>
      <td class=body colspan=<?=$num_fields;?>>
<?php    if ( $num_rows_displayed - $max_rows  > 0 ) { ?>
     <input type=button value="Previous <?=$max_rows;?> rows" onClick="this.form.num_rows_displayed.value=<?=($num_rows_displayed-$max_rows-$i);?>;this.form.submit()">
<?       }
         
         if ( $num_rows_displayed < $num_rows )
         {  $rows_remaining = $num_rows - $num_rows_displayed;
            if ( $rows_remaining == 1 )
            {
?>
<input type=submit value="Next row">
<?          }
            else
            {
?>
<input type=submit value="Next <?=min($max_rows, $rows_remaining);?> rows">
<?          }
         }
?>
      </td>
   </tr>
</table>
<? } ?>
</form>
<?    }
      $this->close();  //close cache file  
    }   
}

class CacheFileWriter extends CacheFileManager
{
    //constructor opens cache file - the name of which is passed as an argument
    function CacheFileWriter ($cache_filename)
    {
        $this->setCName("CacheFileWriter");
        $this->setFilePath($cache_filename);
        $this->setMode('w');
    }

    function writeLine($string)
    {
        if ( !$fp = $this->getFilePtr() )
        {
            $this->showError($this->getCName().":Cannot Write: File Pointer not set");
            return;
        }

        if ( substr($string, -2) != "\n" )
        $string .= "\n";
        fputs($fp, $string);
    }

    function writeAllRecords( $res )
    {
        if ( !$res )
        $this->showError($this->getCName().": Invalid resultset");

        $num_rows = sqlNumRows($res);

        for ( $i=0; $i<$num_rows; $i++ )
        {
            $row = sqlResult($res, $i);
            $str = implode(",", $row);

            $this->writeLine($str);
        }
    }
}


class CSVFileWriter extends CacheFileWriter
{
    var $path = temp_dir;

    function CacheFileWriter ($csvFileName)
    {
        $this->setCName("CSVFileWriter");
        $this->setFilePath($csvFileName);
        $this->setMode('w');
    }

    //set the file path class variable
    function setFilePath($csvFileName)
    {
        if ( isBlank($csvFileName) )
        {
            $this->showError("Cannot set CSV file path. blank file name argument supplied",true);
            return;
        }
        $this->filePath = $this->path."/$csvFileName";
    }
}




class CacheFileDisplayManager extends CacheFileReader
{   var $max_rows = 0;
    var $num_rows = 0;
    var $num_rows_displayed = 0;
    var $_DELIMITER = "|";

    function CacheFileDisplayManager ($cache_filename, $num_rows, $max_rows, $num_rows_displayed)
    {   
    	$this->setCName("Cache File Reader");
        $this->setFilepath($cache_filename);
        $this->setNumRows($num_rows);
        if ( $max_rows )
        	$this->setMaxRows($max_rows);
        else
        	$this->setMaxRows(20);
        	
        $this->setNumRowsDisplayed($num_rows_displayed);
        $this->setMode('r');      
    }
    
    function setMaxRows($max_rows)
    {  $this->max_rows = $max_rows;
    }
    
    function getMaxRows()
    {  return $this->max_rows;
    }
    
    function setNumRows($num_rows)
    {  $this->num_rows = $num_rows;

    }

    function getNumRows()
    {  return $this->num_rows;
    }

    function setNumRowsDisplayed($num_rows_displayed)
    {  $this->num_rows_displayed = $num_rows_displayed;
    }
    
    function getNumRowsDisplayed()
    {  return $this->num_rows_displayed;
    }
    

    function writeTable( $col_headings, $field_disp, $financial="", $head_bgcolor, $bgcolor,
                                  $h_font_color='white', $b_font_color='black', $post_vars="")
    { $num_rows = ( $this->getNumRows() ? $this->getNumRows() : $this->countLines());
      $max_rows = $this->getMaxRows();
      $num_rows_displayed = $GLOBALS['num_rows_displayed'];//$this->getNumRowsDisplayed();
      $this->resetFile();
	//  $table_headings = explode('|', $this->readLine()); //first line in cache file	            
       
      if ( $num_rows == 0 )
         printInfoMessage("No Records found");
      else
      {  if ( $num_rows_displayed == $num_rows )
            die ("No (more) records to display");

         if ( $num_rows_displayed > 0 )
         {  for ( $k=0; $k<$num_rows_displayed; $k++ )
               $this->readLine();
         }
?>     
<? printInfoMessage("$num_rows records found");?>
<P class=mesg>
<?
          $last_display = $num_rows_displayed+$max_rows;
?>     
<P>
<b>Displaying rows <?=$num_rows_displayed+1;?> - <?=min($last_display, $num_rows);?> of <?=$num_rows;?></b>
</P>
<P>
<form name='recorddisplayform' action="<?=SELF;?>" method="POST">
<? if ( is_array($post_vars) )
   {  foreach( $post_vars as $key => $val )
      {  
?><input type=hidden name="<?=$key;?>" value="<?=$val;?>">
<?    }
   }
?>    
<table cellpadding=4 cellspacing=1 bgcolor="gray" border=0>
   <tr valign=top>
<? foreach ( $col_headings as $key => $val )
   {  
?>    <td class=label><?=$val;?></span></td>
<? }
?> </tr>
<? $i=0;

   while ( $i < $max_rows && $num_rows_displayed < $num_rows )
   {  //read until last row displayed (inclusive)
      $row = $this->readLine();
      $arr = explode($this->_DELIMITER, $row);
      $num_fields = count($arr);
?> <tr valign=top>
<?    foreach ( $field_disp as $key => $val  )
      {  if ( is_array($val) )
         {  foreach (  $val as $k => $v )
               if ( $arr[$v] )
               {  unset($field);
                  $field = $arr[$v];
                  break;
               }
         }
         else
            $field = $arr[$val];

         $field = ereg_replace("  "," ", $field);
         $first_char  = substr($field, 0,1);
         $second_char = substr($field, 1,1);

         if ( $first_char == '$' || $first_char == '#' ||  $first_char == '%' || ( $first_char == '(' && $second_char == '$' )  )
         {
?>          <td align="right" class=content>					   
<?             
               if ( $first_char == '$' )
               {  if ( $financial )
                     if ( 'NONFINANCIAL' == $arr[$financial] )
                        if ( substr($field, 1, strlen($field)-1) > 0 )
                           echo '#'.number_format(substr($field, 1, strlen($field)-1), 2);
                        else
                           echo '#0';
                     else
                     {  
?>                      <font color=blue>
                           <?printInDollars(substr($field, 1, strlen($field)-1),2);?>
                        </font>
<?                   }
                  else
                  {  
?>                   <font color=blue>
                        <?printInDollars(substr($field, 1, strlen($field)-1),2);?>
                     </font>
<?                }
               }
               elseif ( $first_char == '%' )
                  echo number_format(substr($field, 1, strlen($field)-1), 2)."%";
               elseif ( $first_char == '#' )
                  if ( substr($field, 1, strlen($field)-1) > 0 )
                     echo '#'.number_format(substr($field, 1, strlen($field)-1), 2);
                  else
                     echo '#0';
               else
                  printInDollars(substr($field, 2, strlen($field)-1)*-1)."%";
?>          </td>
<?       }
         else
         {  $qualifier=substr($field, 0,6);
            if ( 'imgref' == $qualifier )
            {  
            	
?>             <td class=content align=center>
<?                           	
            	$imgs_arr = explode(';',$field);
               $num_imgs = count($imgs_arr);
               
               for ($i=0; $i<$num_imgs; $i++ )
               {  $img_pair = $imgs_arr[$i];
               	  $ref_arr = explode('=', $img_pair);
                  $img_name = $ref_arr[1];
?>                    
                  <img src="<?=image_dir."/$img_name"?>">
<?				}
?>			  </td>	          
<?          }
            else
            {  
?>             <td class=content>
                  <?=stripslashes($field);?>
               </td>
<?          }
         }
      }
?> </tr>
<?    $num_rows_displayed++;
      $i++;	
   }
?>    
</table>


<?  if ( $max_rows < $num_rows ) { ?>
<input type=hidden name=num_rows_displayed value=<?=$num_rows_displayed;?>>
<input type=hidden name=num_rows value=<?=$num_rows;?>>
<input type=hidden name=max_rows value=<?=$max_rows;?>>
<input type=hidden name=show_search_results value=yes>
<input type=hidden name=member_type value=<?=$GLOBALS['member_type'];?>>
<input type="hidden" name="show_search_results"	value="yes">
<table cellpadding=4 cellspacing=1 border=0>
   <tr>
      <td class=body colspan=<?=$num_fields;?>>
<?       if ( $num_rows_displayed - $max_rows  > 0 )
         {  
?>
<input type=button value="Previous <?=$max_rows;?> rows" onClick="this.form.num_rows_displayed.value=<?=($num_rows_displayed-$max_rows-$i);?>;this.form.submit()">
<?       }
         
         if ( $num_rows_displayed < $num_rows )
         {  $rows_remaining = $num_rows - $num_rows_displayed;
            if ( $rows_remaining == 1 )
            {
?>
<input type=submit value="Next row">
<?          }
            else
            {
?>
<input type=submit value="Next <?=min($max_rows, $rows_remaining);?> rows">
<?          }
         }
?>
      </td>
   </tr>
</table>
<? } ?>
</form>
<?    }
      //$this->close();  //close cache file  
    }
    
    
    
    function printTable($exclude_cols="", $post_vars="", $excel_filename="", $row_format_array="", $heading_format_array="", $sum_array="")
    { $num_rows = ( $this->getNumRows() ? $this->getNumRows() : $this->countLines());
      $max_rows = $this->getMaxRows();
      $num_rows_displayed = $GLOBALS['num_rows_displayed'];//$this->getNumRowsDisplayed();
      $this->resetFile();
      $table_headings = explode('|', $this->readLine()); //first line in cache file	    

     
      if ( $num_rows == 0 )
         printInfoMessage("No Records found");
      else
      {  if ( $num_rows_displayed == $num_rows )
            die ("No (more) records to display");

         if ( $num_rows_displayed > 0 )
         {  for ( $k=0; $k<$num_rows_displayed; $k++ )
               $this->readLine();
         }

         $last_display = $num_rows_displayed+$max_rows;
         
         printInfoMessage("$num_rows records found");
ob_start(); 
?>   
<P>
<form name='recorddisplayform' action="<?=SELF;?>" method="POST">
<? if ( is_array($post_vars) )
   {  foreach( $post_vars as $key => $val )
      {  
?><input type=hidden name="<?=$key;?>" value="<?=$val;?>">
<?    }
   }
?>    

<table cellpadding=4 cellspacing=1  border=0>
	<tr align=top height=40> 
		<td class=body colspan=<?=($x=ceil(count($table_headings)/2))?>>
			<div class=small>
			<b>Displaying rows <?=$num_rows_displayed+1;?> - <?=min($last_display, $num_rows);?> of <?=$num_rows;?></b>
			</div>
		</td>
		<td align=right class=body colspan=<?=count($table_headings)-$x?>>
<? if ( $excel_filename ) { ?>
		<a href="<?=secure_host."/admin/temp/$excel_filename?".mktime()?>"><img alt="Download as Excel File" src="<?=image_dir?>/icon_download.jpg" border=0></a> <?printClassLink(secure_host."/admin/temp/$excel_filename?".mktime(), "Download Excel File", "medium");?>
<?	} ?>
		</td>
	</tr>

<tr valign=top>
<? foreach ( $table_headings as $key => $val ) // print headings for table
   {  	$rval = strtok($val,';');
   		if ( !in_array($rval, $exclude_cols) )
   		{	
?>    <td class=label><?=$rval;?></td>
<?		}
   		$table_headings_indexed[] = $rval;
   		$table_headings_indexed_fmt[] = $val;
   }
?> </tr>
<? $i=0;
   $num_cols = count($table_headings);	
   
   while ( $i < $max_rows && $num_rows_displayed < $num_rows )
   {  $row = explode($this->_DELIMITER, $this->readLine());
   	  $num_fields = count($arr);
   	  $col_offset = 0;
   	  $cols_displayed = 0;

?> <tr valign=top>
<?    	$pos = 0;
		for ( $j=0; $j<$num_cols; $j++ )
		{	if ( ! in_array($table_headings_indexed[$j], $exclude_cols) )
			{	$align='left';
				$cols_displayed++;
	   			if ( strstr($table_headings_indexed_fmt[$j], ';') )
				{	$hd = strtok($table_headings_indexed_fmt[$j], ';');
					$_str = strtok(';');
					strtok($_str, '=');
					$align = strtok('=');					
				}
				
				if ( in_array($table_headings_indexed[$j], $sum_array ) )
				{	$totals[$cols_displayed] += $row[$j];
				}
				else
					unset($totals[$cols_displayed]);

					
								
?>				<td class=content align=<?=$align?>><?=$row[$j]?></td>
<?				$pos++;
			}
		}
?> </tr>
<?    $num_rows_displayed++;
      $i++;	
   }

$contents = ob_get_clean();
   
  // showval($num_rows."!".$num_rows_displayed);
?> 
<?
$contents = ob_get_clean();
   
if ( $totals )
{	if ( $num_rows <= $max_rows )
	{	echo $contents;

?>
<tr>
<?php	$num_cols_displayed = count($row) - count($exclude_cols);
		foreach ( $totals as $index => $arr)
		{
		    if ( $arr ) {
		        $func = $sum_arr[($arr['field_name'])];
?>
				<td class=content align=right><b><?=$func($arr['sum']);?></b></td>	
<?php		}
			else {
?>
				<td class=content align=right>&nbsp;</td>	
<?php
			}
		}
	}
	else {
?>
<p>
<table cellpadding="4" cellspacing="1" border="0">
<?php		foreach ( $sum_arr as $key => $sumfunc )
		{	reset($totals);
			foreach ( $totals as $index => $sum )
			{	if ( $key == $sum['field_name'] )
				{ 	$thds[]	= "Total $key";
					$tval[] =  $sumfunc($sum['sum']);
				}								
			}
		}
		
		echo "<tr><td class=label>".implode('</td><td class=label>', $thds)."</td></tr>";
		echo "<tr align=right><td class=content>".implode('</td><td class=content>', $tval)."</td></tr>";

?>
</table>
<?=$contents?>
<?php
	}
}
?>
</table>


<?php  if ( $max_rows < $num_rows ) { ?>
    <input type="hidden" name="num_rows_displayed" value="<?=$num_rows_displayed;?>">
    <input type="hidden" name="num_rows" value="<?=$num_rows;?>">
    <input type="hidden" name="max_rows" value="<?=$max_rows;?>">
    <input type="hidden" name="show_search_results" value="yes">
    <input type="hidden" name="member_type" value="<?=$GLOBALS['member_type'];?>">
    <input type="hidden" name="show_search_results"	value="yes">
<table cellpadding=4 cellspacing=1 border=0>
   <tr>
      <td class=body colspan=<?=$num_fields;?>>
<?php   if ( $num_rows_displayed - $max_rows  > 0 ) {  ?>
              <input type="button" value="Previous <?=$max_rows;?> rows" onClick="this.form.num_rows_displayed.value=<?=($num_rows_displayed-$max_rows-$i);?>;this.form.submit()">
<?php   }
         
         if ( $num_rows_displayed < $num_rows ) {
            $rows_remaining = $num_rows - $num_rows_displayed;
            if ( $rows_remaining == 1 ) {
?>
              <input type=submit value="Next row">
<?php       }
            else {
?>
               <input type=submit value="Next <?=min($max_rows, $rows_remaining);?> rows">
<?php       }
         }
?>
      </td>
   </tr>
</table>
<?php } ?>
</form>
<?php
      }
      //$this->close();  //close cache file  
    }
       
}

} //endif class_exists
?>
