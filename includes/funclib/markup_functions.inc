<?
/******
markup_functions.inc
These functions are for marking up html, including provisions for maintaining state
******/

function printOption($select_name, $option_value, $option_text, $is_default=false){
  ?>
<option value="<?=$option_value?>"<?
  if($is_default) echo " selected";?>><?=$option_text?>
  </option>
<?
}

function printBr($number){
  for ( $i=0; $i<$number; $i++ ){
  	 echo "<BR>";
  }
}

function printCheckbox($name, $text="", $onClick=""){?>
 <input type="checkbox" name="<?=$name?>"<? if ( $_POST[$name] || $_GET[$name] ) echo " checked";?><? if ( $onClick ) echo " onClick=\"$onClick\"";?>> <?=$text?>
<?}

function printHiddenInput($name, $value=""){  ?>
<input type="hidden" name="<?=$name?>" value="<? if ( $value ) echo $value; else echo getPostGetVar($name);?>">
<?}

function printImageLink($ref, $image_name, $width="", $height="", $target="_self", $imgattr=""){
	if ( $width )
	   $width_string =  "width=\"$width\"";
	if ( $height )
	   $height_string =  "height=\"$height\"";
   ?><a href="<?=$ref;?>" target="<?=$target;?>"><img src="<?=image_dir."/$image_name";?>" <?=$width_string;?> <?=$height_string;?> border=0 <?=$imgattr;?>></a><?
}

function dateSelector($select_name, $default="", $onChange=""){
  if(empty($default)){
    $default = time();
  }
  daySelector($select_name . "[day]",           date("d", $default));
  monthSelectorNumber($select_name . "[month]", date("m", $default));
  yearSelector($select_name . "[year]",         date("Y") - 1, date("Y") + 1, date("Y", $default), $onChange="");
  echo "-";
  hourSelector($select_name . "[hour]",         date("H", $default));
  echo ":";
  minuteSelector($select_name . "[minute]",     date("i", $default));
}

function minuteSelector($select_name, $default="", $onChange=""){?>
  <select name="<?=$select_name;?>">
  <? for ( $i=0; $i<60; $i++ )
        printOption($select_name, $i, addleadingZero($i), ($i == $default));
   ?>
  </select>
<?}


function hourSelector($select_name, $default="", $onChange=""){?>
  <select name="<?=$select_name;?>">
  <? for ( $i=0; $i<24; $i++ )
        printOption($select_name, $i, addleadingZero($i), ($i == $default));
   ?>
  </select>
<?}

function daySelector($select_name, $default="", $onChange=""){?>
  <select name="<?=$select_name;?>">
  <? for ( $i=1; $i<=31; $i++ )
        printOption($select_name, $i, addleadingZero($i), ($i == $default));
   ?>
  </select>
<?}

function monthSelectorNumber($select_name, $default="", $onChange=""){
  ?>
<select name="<?=$select_name;?>">
<? for ( $i=1; $i<13; $i++ )
      printOption($select_name, $i, addleadingZero($i), ($i == $default));
 ?>
</select>
<?}

function yearSelector($select_name, $min_year, $max_year, $default="", $onChange=""){?>
<select name="<?=$select_name;?>">
<? for ($i=$min_year; $i<=$max_year; $i++ )
      printOption($select_name, $i, $i, ($i == $default));
 ?>
</select>
<?}



function getRandomImageMarkup ($image_array, $options="border=0"){
   $num_images = count($image_array);
   $pick = rand(0,$num_images-1);
   return "<img src=\"".image_dir."/$image_array[$pick]\" $options>";
}

/**
  * @return void
  * @param string $message
  * @desc Prints markup for a javascript alert
  */
function jAlert( $message ){
  echo "<script language=\"javascript\">jAlert(\"$message\",\"info\");</script>";
}

/**
 * @return void
 * @param string $ref
 * @param string $text
 * @param string $class
 * @param string $target
 * @param string $name
 * @desc Prints a Hyperlink using CSS
*/
function printClassLink($ref, $text, $class="", $target="_self", $name=""){
	if ( $class ){
   	?><a href="<?=$ref?>" target="<?=$target?>" class="<?=$class?>"><?=$text?></a><?
   }else{
   	?><a href="<?=$ref?>" target="<?=$target?>"><?=$text?></a><?
   }
}

function getGamesName($name){
  $nameSplit = split("_", $name);
  return ucfirst($nameSplit[0]);
}

/**
 * @return string
 * @param string $ref
 * @param string $text
 * @param string $class
 * @param string $target
 * @param string $name
 * @desc Returns a Hyperlink using CSS
*/

function getClassLink($ref, $text, $class="", $target="_self", $size=10){
   ob_start();
   printClassLink($ref, $text, $class, $target,  $name="");
   return ob_get_clean();
}

/**
 * @return string
 * @param string $ref
 * @param string $text
 * @param string $class
 * @param string $target
 * @param string $name
 * @desc Alias for printClassLink
*/
function printLink ($ref, $text, $class="", $target="_self", $name=""){
	printClassLink($ref, $text, $class, $target="_self", $name="");
}

/**
 * @return string
 * @param string $ref
 * @param string $text
 * @param string $class
 * @param string $target
 * @param string $name
 * @desc Alias for getClassLink
*/
function getLink ($ref, $text, $class="", $target="_self", $name=""){
	ob_start();
   	printClassLink($ref, $text, $class, $target,  $name="");
   	return ob_get_clean();
}

/**
 * @return void
 * @param string $ref
 * @param string $text
 * @param string $class
 * @param string $target
 * @param string $name
 * @desc Prints a horizopntal table row
*/
function sideTableRow($label, $content, $align="left", $class_arr=''){
   if ( ! $class_arr )
      $class_arr=array("label", "content")
?>
  <tr valign=top>
    <td class=<?=$class_arr[0]?>><?=$label;?></td>
    <td class=<?=$class_arr[1]?> align=<?=$align?>><?=$content;?></td>
  </tr>
<?
}

/**
 * @return void
 * @param string $ref
 * @param string $text
 * @param string $class
 * @param string $target
 * @param string $name
 * @desc Prints the heading row of a vertically labelled table
*/
function topTableheading($label_array, $width="", $cellpadding=4, $cellspacing=1, $border=0){
?>
<table cellpadding="<?=$cellpadding?>" cellspacing="<?=$cellspacing?>" border="<?=$border?>"<? if ($width) echo " width=\"$width\"";?>>
   <tr>
<? foreach ( $label_array as $key => $val){
?>    <td class=label><?=$val;?></td>
<? }
?>
   </tr>
<?
}

/**
 * @return void
 * @param string $content_array
 * @param string $align_right_arr
 * @desc Prints a vertical table row
*/
function topTableRow($content_array, $align_right_arr=array()){
?>
<tr valign=top>
<? foreach ( $content_array as $key => $val ){
     $align = ( in_array($key, $align_right_arr) ? "right" : "left" );
?>    <td class=content align="<?=$align?>"><?=$val;?></td>
<? }
?>
</tr>
<?
}

/**
 * @return void
 * @param string $message
 * @param string fourth_cell
 * @param string font_color
 * @desc Prints an Information message for customers
*/
function printInfoMessage($message, $fourth_cell=""){
?>
<div style="padding:5">
   <table cellpadding=0 cellspacing=0 border=0>
      <tr valign=top>
         <td ><img src="<?=image_dir?>/icon_info.gif"></td>
         <td width=6><img src="<?=image_dir?>/clear.gif"></td>
         <td valign=middle><div class="mesg"><?=$message?></div></td>
     <? if ( $fourth_cell )
         {
      ?>   <td width=6><img src="<?=image_dir?>/clear.gif"></td>
           <td class=normal><?=$fourth_cell?></td>
      <? }
      ?>
      </tr>
   </table>
</div>
<?
}


/**
 * @return void
 * @param string $message
 * @desc Prints an Error message for customers
*/
function printErrorMessage($message){
?>
   <table cellpadding=0 cellspacing=0 border=0>
      <tr valign=top>
         <td><img src="<?=image_dir?>/icon_error.gif" height=16 width=16></td>
         <td width=6><img src="<?=image_dir?>/clear.gif"></td>
         <td class=message><font color=red><b><?=$message?></td>
      </tr>
   </table>
<?
}

function jscriptStart() { ?><script language="JavaScript" type="text/javascript"><?}

function jscriptEnd() { ?></script> <?}

function jscriptBlock($code_block) {
	jscriptStart();
   echo $code_block;
   jscriptEnd();
}

function jscriptLine($code_line){
	echo $code_line;
}

function countrySelect($select_name='country_code', $onChange='', $default="",$disabled=false,$id=false,$hideOptionOne=false,$multiple=false){
	global $dbh,$lang;
   if ( $onChange )
     $onchangestr = "onChange=\"$onChange\"";
     $country_post_val = array_key_exists('country', $_POST);

     $ip_arr = getIpDetails(getIpAddress());
     $sql  = 'select cou_code, cou_name from country';
     $sql .= ' order by cou_name asc';
     $rs = $dbh->exec($sql);
     if ( count($rs) > 0 ){
?>   <select name="<?=$select_name.($multiple?"[]":'');?>" <?=$onchangestr;?> <?=(($disabled)?("disabled"):(""))?> <?=(($id)?("id=$id"):(""))?> <?=($multiple?"multiple='multiple'":'')?> >


       <? if(!$hideOptionOne){ ?>
             <option value="">- <?=$lang->getLang('Please select a country') ?> - <? }
     while ( $rs->hasNext() ){
     	$rec = $rs->next();
        unset($selected);
        //use value of equivalent Post or get var as default country
        //echo trim($rec['cou_code'])."==".$default;
        if(is_array(getPostGetVar($select_name)) && in_array($rec['cou_code'],getPostGetVar($select_name))){
            $selected = 'selected';
        }
        elseif($multiple && !getPostGetVar($select_name)){
            $selected = 'selected';
        }
        elseif ( strtolower(getPostGetVar($select_name)) == strtolower(trim($rec['cou_code'])) || trim(strtolower($rec['cou_code'])) == trim(strtolower($default)))
          $selected = 'selected';
        else{
        	if ( $country_post_val ){  //use the country that the IP-to-country table returns
            	if ( $ip_arr['country_code2'] == trim($rec['cou_code']) )
                 $selected = 'selected';
            }
        }
?>      <option value="<?=trim($rec['cou_code'])?>" <?=$selected?>><?=$rec['cou_name']?>
<?   }
?> </select>
<? }
}

function countryCodeSelector($select_name='Ecom_BillTo_Postal_CountryCode', $onChange=''){
   global $dbh;
   $sql  = 'select cou_name, cou_code_two_chars from country' .
           ' order by cou_name asc';
   $dbh->doQuery($sql);
   $rs = $dbh->getResults();
   $num_rows = $dbh->getNumRows();
   if ( $num_rows < 1 ){
   	  echo '<div style="color:red">Error Could not get country details</span>';
       return;
   }
   if ( $onChange ){
   	$onchangestr = "onChange=\"$onChange\"";
   }
   $ip_arr = getIpDetails(getIpAddress());
   //Print the select
   echo '<select name="' . $select_name . '" ' . $onchangestr . '>';
   echo '<option value="">- select a country -';
   //$rec = $rs->next();
   //loop throught countryr ecords
   while ( $rec=$rs->fetchRow() ){
   	  $selected = '';
      $curr_cou_code = trim($rec['cou_code_two_chars']); //country code
      $curr_cou_name = $rec['cou_name'];  //country name

      if ( $_POST[$select_name] == $curr_cou_code ){
      	$default_cou_code =  $_POST[$select_name];
      }elseif ( $_GET[$select_name] == $curr_cou_code ){
      	$default_cou_code =  $_GET[$select_name];
      }elseif ( $_GLOBALS[$select_name] == $curr_cou_code ){
      	$default_cou_code =  $_GLOBALS[$select_name];
      }else{
      	  //try to get client geographical location details using IP address
       	   $ip_cou_code =  $ip_arr['country_code2']; //country code  determined from IP address
      	   if ( $ip_cou_code == $curr_cou_code )
               $selected = 'selected';
      }
      echo '<option value="' . $curr_cou_code . '" ' . $selected . '>' . $curr_cou_name;
      //$rec = $rs->next();
   }
   echo '</select>';
}

/**
 * @return void
 * @param string $punter_id
 * @desc Prints "back" link for pages
*/
function backLink($link, $text){
?>
<a href="<?=$link?>"><img src="<?=image_dir?>/button_back_arrow.gif" alt="" width="10" height="10" vspace="2" border="0" align="absmiddle"></a> <a href="<?=$link?>" class=medium><b><?=$text?></b></a>
<?
}

function contentClass ($field_id){
	global $ERRORS;
    if ( fieldInError($field_id) )
       return "contenterror";
    else
       return "content";
}

function fieldErr($field_id){
	global $ERROR_MESSAGES;
	if ( $msg = $ERROR_MESSAGES[$field_id] )
		return "<div style=\"color:white;font-weight:bold;font-size:8pt\">$msg</div>";
}

function setupDateInputJs(){
  ?>
	 <script type="text/javascript" src="media/calendar.js"></script>
	 <link type="text/css" rel="stylesheet" media="screen" href="media/calendar.css"/>
	 <script type="text/javascript">
	 pathToImages = "/media/images/";
	 </script>
  <?
}

function createDateInput($name="date_from", $value, $readonly=true, $form_name="search", $hour_selector=true, $date_helper_name="date_helper", $start_year=null){
  if(empty($start_year)){
    $start_year = date('Y') - 90;
  }
  ?>
  <input type="text" name="<?=$name?>" value="<?=$value?>"<?=(($readonly)?(" readonly='true'"):(""))?>"  onchange="refreshDateSelect_<?=$date_helper_name?>();"/> <a href="javascript:void(0);" onclick="displayCalendar(document.forms['<?=$form_name?>'].<?=$name?>,'yyyy-mm-dd<?=(($hour_selector)?(" hh:ii"):(""))?>',this,<?=(($hour_selector)?("true"):("false"))?>)"><img src="media/images/cal.gif" width="16" height="16" border="0" alt="Click Here to Pick up the date"></a>
  <?php
}

function createDateHelper($name, $max_hours=168, $from_name, $to_name, $form_name="search", $hour_selector=true){
  $now = time();
  ?>
   <script type="text/javascript">
   function updateDate_<?=$name?>(){
     var date = document.<?=$form_name?>.<?=$name?>.value;
     if(date == ""){
       return;
     }

     document.<?=$form_name?>.<?=$from_name?>.value = date;
     document.<?=$form_name?>.<?=$to_name?>.value   = "<?=date("Y-m-d" . (($hour_selector)?(" H:i"):("")), strtotime("tomorrow - 1 minutes"))?>";
   }

   function refreshDateSelect_<?=$name?>(){
     var a = document.getElementById("<?=$name?>");
     //a.selectedIndex = a.options.length - 1;
     a.selectedIndex = 0;
   }
   </script>

  <select name="<?=$name?>" id="<?=$name?>" onChange="updateDate_<?=$name?>();">
    <option value="">Custom</option>
    <?php
    $opt_arr = array();

    if($hour_selector){
      $opt_arr[] = array("name" => "Last 2 hours",   "value" => strtotime("now - 2 hours"));
      $opt_arr[] = array("name" => "Last 4 hours",   "value" => strtotime("now - 4 hours"));
      $opt_arr[] = array("name" => "Last 12 hours",  "value" => strtotime("now - 12 hours"));
      $opt_arr[] = array("name" => "Last 24 hours",  "value" => strtotime("now - 24 hours"));
      $opt_arr[] = array("name" => "Last 48 hours",  "value" => strtotime("now - 48 hours"));
    }

    $opt_arr[]  = array("name" => "Last 3 days",     "value" => strtotime("tomorrow - 3 days"));
    $opt_arr[]  = array("name" => "Last week",       "value" => strtotime("tomorrow - 1 weeks"));
    $opt_arr[]  = array("name" => "Last 2 weeks",    "value" => strtotime("tomorrow - 14 days"));
    $opt_arr[]  = array("name" => "Last 3 weeks",    "value" => strtotime("tomorrow - 21 days"));
    $opt_arr[] = array("name" => "Last month",      "value" => strtotime("tomorrow - 1 months + 1 days"));
    $opt_arr[] = array("name" => "Last 2 month",    "value" => strtotime("tomorrow - 2 months + 1 days"));
    $opt_arr[] = array("name" => "Last 3 month",    "value" => strtotime("tomorrow - 3 months + 1 days"));
    $opt_arr[] = array("name" => "Last 6 month",    "value" => strtotime("tomorrow - 6 months + 1 days"));
    $opt_arr[] = array("name" => "Last year",       "value" => strtotime("tomorrow - 12 months + 1 days"));

    foreach ($opt_arr as $opt){
      if(($now - $opt["value"]) / 60 / 60 <= $max_hours){
        $date = date("Y-m-d" . (($hour_selector)?(" H:i"):("")), $opt["value"]);
        printOption($name, $date, $opt["name"]);
      }
    }
    ?>
  </select>
  <?php
}

function getDateStatus($date){
  if(empty($date)){
    return "Forever";
  }

  $date_time = strtotime($date);
  $now       = time();

  $ret = "%s";

  if($date_time < $now){
    $ret = "<span style='color:#CCC'>%s</span>";
  }

  return sprintf($ret, date("d M Y - H:i", $date_time));
}

function shortFormatDate($date){
  return date("d M Y H:i", strtotime($date));
}

?>
