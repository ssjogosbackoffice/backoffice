<?
/**********************************************************************************/
/***			date_functions.inc	--> all functions applying to dates!		***/
/**********************************************************************************/

function removeLeadingZero ($number_string)
{	if ( strlen($number_string) > 1 )
if ( substr($number_string, 0, 1) == "0" )
$number_string = substr($number_string, 1, 1);
return $number_string;
}


function getDBDate( $db_date )
{	$arr = explode(" ", $db_date);
$date = $arr[0];
return $date;
}


function getDBTime( $db_date )
{	$arr = explode(" ", $db_date);
$time = $arr[1];
return $time;
}


function m2month ( $mon_num )
{  removeLeadingZero($mon_num);

switch( $mon_num )
{  case "1" :  return "January";
  case "2" :  return "February";
  case "3" :  return "March";
  case "4" :  return "April";
  case "5" :  return "May";
  case "6" :  return "June";
  case "7" :  return "July";
  case "8" :  return "August";
  case "9" :  return "September";
  case "10" :  return "October";
  case "11" :  return "November";
  case "12" :  return "December";
  default:	return;
}
}


function month2m ( $mon_num )
{  switch( $mon_num )
{  case "January" :  return 1;
  case "February" :  return 2;
  case "March" :  return 3;
  case "April" :  return 4;
  case "May" :  return 5;
  case "June" :  return 6;
  case "July" :  return 7;
  case "August" :  return 8;
  case "September" :  return 9;
  case "October" :  return 10;
  case "November" :  return 11;
  case "December" :  return 12;
  default:	return;
}
}


//formats a hyphen-delimited or forward-slash-delimited date into user-friendly display
function formatDate( $date, $show_hour=false, $newline=false )
{  //$just_date	= getDBDate($date);
  //$hour		= getDBTime($date);
  $date_arr = splitDate($date);
  $year  = $date_arr["year"];
  $month = removeLeadingZero($date_arr["month"]);
  $day   = removeLeadingZero($date_arr["day"]);
  $time  = $date_arr["time"];

  if ( $newline )
  $nl = "<br>";

  if ( $show_hour )
  return  "$day ".m2month($month)." $year, $nl" . formatTime($time)." hours";
  else
  return "$day ".m2month($month)." $year";
}


function shortDate ( $date, $show_time=false, $newline=false )
{  $date_arr = splitDate($date);

settype($date_arr["hour"], "integer");
settype($date_arr["minute"], "integer");
settype($date_arr["second"], "integer");
settype($date_arr["month"], "integer");
settype($date_arr["day"], "integer");
settype($date_arr["year"], "integer");
$mktime = mktime($date_arr["hour"],$date_arr["minute"],$date_arr["second"],$date_arr["month"],$date_arr["day"],$date_arr["year"]);

if ( $newline )
$nl = "<br>";
else
$nl = " ";

if ( $show_time )
$date_formatted = date('j/n/Y',$mktime).'&nbsp;<nobr>'.date('H:i:s', $mktime);
else
$date_formatted = date('j/n/Y', $mktime);

return  $date_formatted;
}


function formatTime($time)
{
  if ( strstr($time, ":") )
  return substr($time, 0, 5);

  $hr   = substr($time, 0, 2);
  $min   = substr($time, 2, 2);
  return "$hr:$min";
}

/*function toDatabaseDate ($day, $month, $year, $time="00:00:00")
{  return "$year-".addLeadingZero($month)."-".addLeadingZero($day);
}


function toDatabaseDateTime ($day, $month, $year, $time="00:00:00")
{  return "$year-$month-$day $time";
}

function toDatabaseTimestamp ($day, $month, $year, $time="00:00:00")
{
return toDatabaseDateTime ($day, $month, $year, $time);
}
*/
function phpTsToDbTs ($php_timestamp, $showtime=false)
{  if ( $showtime )
return date('Y-m-d H:i:s',$php_timestamp);
else
return date('Y-m-d',$php_timestamp);
}



function toMySQLTimestamp ($day, $month, $year, $time="00:00:00")
{  return toPGTimestamp ($day, $month, $year, $time);
}

function limited_date_selector($day_name, $month_name, $year_name, $options="",$with_id=false ) {
  $arr = ( count($_POST) > 0 ? $_POST : $_GET );
  if ( ! ( $arr[$day_name] || $arr[$month_name] || $arr[$year_name] ) )
  $arr = $GLOBALS;

  $prev_day = $arr[$day_name];
  $prev_month = $arr[$month_name];
  $prev_year = $arr[$year_name];
  $today_day = date('j');
  $today_month = date('n');
  $today_year = date('Y');
  $def_day = $today_day;

  //day select
  if ( $day_name ) {
    $str = "<select ".(($with_id)?"id=$day_name":"")." name=$day_name $options>";
    if ( !$def_day && !$prev_day )
    $str .= "<option value=\"\">day";
    for ( $i = $today_day-2; $i <= $today_day; $i++) {
      $disp = ( $i < 10 ? "0$i" : $i );
      $str .= "\n  <option value=$disp";
      if ( $prev_day == $i || (!$prev_day && $def_day == $i) )
      $str .= " SELECTED";
      $str .= ">$i";
    }
    $str .= "\n</select> / ";
  }
  //month select
  $str .= "<select ".(($with_id)?"id=$month_name":"")." name=$month_name $options>";
  $disp = ( $today_month < 10 ? "0$today_month" : $today_month );
  $str .= "\n  <option value=$disp SELECTED>".m2month($today_month)."</select> /";

  //year select
  $str .= "<select ".(($with_id)?"id=$year_name":"")."  name=$year_name $options>";
  $str .= "\n  <option value=$today_year SELECTED>".$today_year."</select> /";
  echo $str;
}



function date_selector($day_name, $month_name, $year_name, $min_year, $max_year, $default="", $options="",$with_id=false){
  $prev_day   = $GLOBALS[$day_name];
  $prev_month = $GLOBALS[$month_name];
  $prev_year  = $GLOBALS[$year_name];
  $today_year = date('Y');

  if ($default)
  if ( $default == "today" ){
    $def_day    = date("j");
    $def_month  = date("n");
    $def_year   = date("Y");
  } elseif ( $default == "todaynextmonth" ) {
    $def_day    = date("j");
    $def_month  = date("n")+1;
    $def_year   = date("Y");

    if ( $def_month > 12 ){
      $def_year++;
      $def_month = 1;
    }

    while ( !checkdate($def_month, $def_day, $def_year) )
    $def_day--;
  }   elseif ( $default == "todaylastmonth" ) {
    $def_day    = date("j");
    $def_month  = date("n")-1;
    $def_year   = date("Y");

    if ( $def_month < 1 ){
      $def_year--;
      $def_month = 12;
    }

    while ( !checkdate($def_month, $def_day, $def_year) )
    $def_day--;
  }   elseif ( $default == "todaynextyear" ) {
    $def_day    = date("j");
    $def_month  = date("n");
    $def_year   = date("Y")+1;

    while ( !checkdate($def_month, $def_day, $def_year) )
    $def_day--;

  }elseif ( $default == "todaylastyear" ){
    $def_day    = date("j");
    $def_month  = date("n");
    $def_year   = date("Y")-1;

    while ( !checkdate($def_month, $def_day, $def_year) )
    $def_day--;

  }elseif ( $default == "todaylastweek" ){
    $def_day    = date("j");
    $def_month  = date("n");
    $def_year   = date("Y");
    $mktime = mktime(0,0,0,$def_month, $def_day-7, $def_year);
    $def_day    = date("j", $mktime);
    $def_month  = date("n", $mktime);
    $def_year   = date("Y", $mktime);

  }else{
    $def_day   = "";
    $def_month = "";
    $def_year  = "";
    if($default['day']!=""){
      if( is_array($default)){
        $def_day   = $default['day'];
        $def_month = $default['month'];
        $def_year  = $default['year'];
      }else{
        $default_arr = splitDate($default);
        $def_day   = $default_arr['day'];
        $def_month = $default_arr['month'];
        $def_year  = $default_arr['year'];
      }
      if ( !checkdate($def_month, $def_day, $def_year) ){
        echo "<font color=red size=2><b>Invalid default date paramater: \"$default\"</b></font> ";
        return;
      }
    }

  }

  //day select
  if ( $day_name ){
    $str = "<select ".(($with_id)?"id=$day_name":"")." name=$day_name $options>";
    if ( !$def_day && !$prev_day )
    $str .= "<option value=\"\">day";
    for ( $i=1; $i<=31; $i++ ){
      $disp = ( $i < 10 ? "0$i" : $i );

      $str .= "\n  <option value=$disp";
      if ( $prev_day == $i || (!$prev_day && $def_day == $i) )
      $str .= " SELECTED";
      $str .= ">$i";
    }
    $str .= "\n</select> / ";
  }

  //month select
  $str .= "<select ".(($with_id)?"id=$month_name":"")." name=$month_name $options>";
  if ( !$def_month && !$prev_month )
  $str .= "<option value=\"\">month";
  for ( $i=1; $i<=12; $i++ )
  {

    $disp = ( $i < 10 ? "0$i" : $i );
    $str .= "\n  <option value=$disp";
    if ( $prev_month == $i || (!$prev_month && $def_month == $i) )
    $str .= " SELECTED";
    $str .= ">".m2month($i);
  }
  $str .= "\n</select> / ";

  //year select
  $str .= "<select ".(($with_id)?"id=$year_name":"")." name=$year_name $options>";
  if ( !$def_year && ! $prev_year)
  $str .= "<option value=\"\">year";
  if ( $min_year == "today")
  $min_year = $today_year;
  elseif ( !$min_year )
  $min_year = $today_year - 2;

  if ( $max_year == "today" || !$max_year)
  $max_year = $today_year;

  for ( $i=$min_year; $i<=$max_year; $i++)
  {	$str .= "\n<option value=$i";
  if ( $i == $prev_year || (!$prev_year && $def_year == $i)  )
  $str .= " SELECTED";
  $str .= ">$i";
  }
  $str .= "\n</select>";

  echo $str;
}


function time_selector ( $hour_name, $min_name, $options="", $default="")
{	  if ( array_key_exists($hour_name, $_GET) || array_key_exists($hour_name, $_POST ) )
$prev_hour = $GLOBALS[$hour_name];

if ( array_key_exists($min_name, $_GET) || array_key_exists($min_name, $_POST ) )
$prev_min = getPostGetVar($min_name);
?>    <select name="<?=$hour_name;?>">
         <option value="">hour
<?
for ( $i=0; $i<=23; $i++ ){
  $selected = "";
  if ( is_array($default) || (isset($prev_hour) && $prev_hour == $i) )
  if ( $i == $default[0] && $default[0]!="")
  $selected = 'selected';
  $val = ( $i < 10 ? "0$i" : $i );
?>          <option value="<?=$val;?>" <?=$selected;?>><?=$val;?>
<?       }
?>       </select>
         :
         <select name="<?=$min_name;?>">
            <option value="">min
<?       
for ( $i=0; $i<=59; $i++ ){
  $selected = "";
  if ( is_array($default) || (isset($prev_hour) && $prev_hour == $i) )
  if ( $i == $default[1] && $default[1]!="")
  $selected = 'selected';


  $val = ( $i < 10 ? "0$i" : $i );
?>          <option value="<?=$val;?>" <?=$selected;?>><?=$val;?>
<?       }
?>       </select>
<?
}



function splitDate( $pg_timestamp )
{
  if ( strstr($pg_timestamp, "/") )
  $delim = "/";
  elseif ( strstr($pg_timestamp, "-") )
  $delim = "-";

  if ( $delim )
  {	//get date part
    $date = strtok($pg_timestamp, " ");
    $d_arr = explode($delim, $date);
    $year  = $d_arr[0];
    $month = $d_arr[1];
    $day   = $d_arr[2];

    //get time part
    $time = strtok(" ");
    $t_arr = explode(':', $time);
    $hour  = $t_arr[0];

    $minute = $t_arr[1];

    $second = strtok($t_arr[2], '.');
    $second = strtok($second, '+');
  }
  else
  {	//mysql-type timetamp
    $year  = substr($pg_timestamp, 0, 4);
    $month = substr($pg_timestamp, 4, 2);
    $day   = substr($pg_timestamp, 6, 2);
    $hour   = substr($pg_timestamp, 8, 2);
    $minute   = substr($pg_timestamp, 10, 2);
    $second   = substr($pg_timestamp, 12, 2);
  }
  $arr = array("year"=>$year, "month"=>$month, "day"=>$day,
  "hour"=>$hour, "minute"=>$minute, "second"=>$second);

  return $arr;
}

/*
function dbTsToPhpTs ( $db_timestamp )
{  $ts = splitDate($db_timestamp);
$timestamp = mktime($ts['hour'], $ts['minute'], $ts['second'], $ts['month'], $ts['day'], $ts['year']);
return $timestamp;
}*/

function getTimeOnly ( $dbTimestamp)
{  $ts = splitDate($dbTimestamp);
$time = $ts['hour'].":".$ts['minute'].":".$ts['second'];
return $time;
}

/*
function dbDateToday()
{  return date('Y-m-d'); //postresql database format YYYY-M-D
}*/

function isValidDateString ($date_str)
{  $db_date_arr = explode('-',$date_str);

$year = (int)$db_date_arr[0];
$month = (int)$db_date_arr[1];
$day = (int)$db_date_arr[2];

return checkDate($month, $day, $year);
}

function extractTime ($date)
{  $date_arr = splitDate($date);
$hour  = $date_arr["hour"];
$minute  = $date_arr["minute"];
$second  = $date_arr["second"];

return  "$hour:$minute:$second";
}


function shorterDate ( $date, $show_time=false, $newline=false, $show_seconds=false)
{  $date_arr = splitDate($date);
$year  = $date_arr["year"];
$month = removeLeadingZero($date_arr["month"]);
$day   = removeLeadingZero($date_arr["day"]);
$hour  = $date_arr["hour"];
$minute  = $date_arr["minute"];
$second  = $date_arr["second"];

$mktime = mktime($hour, $minute, $second, $month, $day, $year);

$time_format = ( $show_seconds ?  'H:i:s' : 'H:i' );

if ( $show_time )
if ( $newline )
$date = date('j/n/y', $mktime)."<br>".date($time_format, $mktime);
else
$date = date("j/n/y $time_format", $mktime);
else
$date = date('j/n/y', $mktime);

return  $date;
}

function checkTime($hour, $min, $sec)
{  if ( $hour < 0 || $hour > 23 )
return false;

if ( $min < 0 || $min > 59 )
return false;

if ( $sec < 0 || $sec > 59 )
return false;

return true;
}



function secondsToDisplay ($php_timestamp, $showtime=true)
{   return shortdate(date('Y-m-d H:i:s',$php_timestamp),$showtime);
}


function sqlTimestampToY_M_D ( $field_name )
{  return "to_char($field_name, 'YYYY-MM-DD')";
}

function sqlTimestampToY_M ( $field_name)
{  return "to_char($field_name, 'YYYY-MM')";
}

function sqlTimestampToY_M_D_H ( $field_name)
{  return "to_char($field_name, 'YYYY-MM-DD HH24')";
}

function sqlTimestampToY_M_D_H_I ( $field_name)
{  return "to_timestamp($field_name, 'YYYY-MM-DD HH24:MI')";
}

function sqlTimestampToY_M_D_H_I_S ( $field_name)
{  return "to_timestamp($field_name, 'YYYY-MM-DD HH24:MI:SS')";
}

function sqlTimestamp ( $field_name, $timezone=false)
{  if ( $timezone )
return $field_name;
else
{  return "to_timestamp($field_name, 'YYYY-MM-DD HH24:MI:SS')";
}

return $str;
}

function toDatabaseDate ($day, $month, $year, $time="00:00:00")
{  return "$year-".addLeadingZero($month)."-".addLeadingZero($day);
}


function toDatabaseDateTime ($day, $month, $year, $time="00:00:00")
{  return "$year-".addLeadingZero($month)."-".addLeadingZero($day)." $time";
}

function toDatabaseTimestamp ($day, $month, $year, $time="00:00:00")
{  return toDatabaseDateTime ($day, $month, $year, $time);
}

function toSQLTimestamp ($day, $month, $year, $time="00:00:00")
{  $month = addLeadingZero($month);
$day = addLeadingZero($day);

return "$year-$month-$day $time";
}

function toYYYY_M_D_H ($day, $month, $year, $time="00")
{  $month = addLeadingZero($month);
$day = addLeadingZero($day);

return "$year-$month-$day $time";
}


function toYYYY_M_D ($day, $month, $year)
{  $month = addLeadingZero($month);
$day = addLeadingZero($day);

return "$year-$month-$day";
}


function toYYYY_M ($month, $year)
{	$month = addLeadingZero($month);

return "$year-$month";
}


function phpTsToDb ($php_timestamp)
{  return date('Y-m-d H:i:s',$php_timestamp);

}


function dbTsToPhpTs ( $db_timestamp )
{
  return strtotime($db_timestamp);
  /*
    $ts = splitDate($db_timestamp);
$timestamp = mktime($ts['hour'], $ts['minute'], $ts['second'], $ts['month'], $ts['day'], $ts['year']);

return $timestamp;
*/
}

function dbDateToday($showtime=false)
{  return date('Y-m-d'); //postresql database format YYYY-MM-DD
}


function databaseTimestampToday()
{	return date('Y-m-d H:i:s');
}

function getReadableDate($timestamp){
  $now = time();
  //test
  //$timestamp = $now - (60 * 60 * 24 * 31);
  
  $time_diff = $now - $timestamp;
  $daydiff   = floor($time_diff / (60 * 60 * 24));  
  
  $ret = "";

  if($time_diff < 60){
    $ret = $time_diff . "s ago";
  }elseif($time_diff < 60 * 5){
    $ret = floor($time_diff / 60) . "m ago";
  }elseif($time_diff < 60 * 30){
    $ret = (floor($time_diff / (60 * 5)) * 5) . "m ago";
  }elseif($time_diff < 60 * 60){
    $ret = (floor($time_diff / (60 * 10)) * 10) . "m ago";
  }elseif($daydiff == 0){
    $ret = floor($time_diff / (60 * 60)) . "h ago";
  }elseif($daydiff == 1){
    $ret = "yesterday";
  }elseif($daydiff <= 30){
    $ret = $daydiff . " days ago";
  }elseif($daydiff <= 365){
    $ret = floor($daydiff / 30) . " months ago";
  }else{
    $ret = floor($daydiff / 365) . " years ago";
  }

  return "<span class='tip'>$ret</span>";
}


?>
