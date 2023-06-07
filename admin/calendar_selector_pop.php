<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<html>
<head>
   <title>Calendar</title>
   <meta name="Generator" content="EditPlus">
   <meta name="Author" content="">
   <meta name="Keywords" content="">
   <meta name="Description" content="">
   <script language=javascript type="text/javascript">
     <?include 'jfunctions.js';?>
   </script>
   <?include 'style.css';?>
</head>

<body>
<script language="javascript">
function clearDates()
{  var numDivs = document.all.length;
   var div;
   
   for ( var i=0; i<numDivs; i++ )
   {  div = document.all[i];
      if ( div.id.substr(0,7) == 'caldate' )
      {  if ( 'blue' == div.style.backgroundColor )
         {  div.style.backgroundColor = '#efefef';
            div.style.color = 'black';
         }
      }
   }
}

function doSelect(divObject)
{  divObject.style.backgroundColor='blue';
   divObject.style.color='#efefef'
   document.forms["calendar"].day.value=divObject.id.substr(7, divObject.id.length);
}


function doHighlight(divObject)
{    
   if ( divObject.style.backgroundColor != 'blue' )
   {  divObject.style.backgroundColor='yellow';
      divObject.style.color='black';
   }
}


function doUnHighlight(divObject)
{  if ( divObject.style.backgroundColor != 'blue' )                            
   {  divObject.style.backgroundColor='#efefef';
      divObject.style.color='black'
   }
}


function seedTargetForm(formObject)
{  targetForm = window.opener.document.forms['<?=$targetForm;?>'];
   targetForm.<?=$day_name?>.selectedIndex=formObject.day.value-1;
   targetForm.<?=$month_name?>.selectedIndex=formObject.month.selectedIndex;

   //number of year options
   num_year_opts = targetForm.<?=$year_name?>.options.length;
   selectedYear = formObject.year.options[formObject.year.selectedIndex].value;
   var i=0;
   while ( i < num_year_opts )
   {  if ( targetForm.<?=$year_name?>.options[i].value == selectedYear )
         break;
      i++;
   }
   targetForm.<?=$year_name?>.selectedIndex=i;
   window.close();
}


</script>
<?
//default to today
if ( ! $day )
{  $day = date('d');
   $month = date('m');
   $year = date('Y');
}


//current timestamp 
while ( !checkDate($month, $day, $year) )
   $day--;
$curr_mktime = mktime(0,0,0,$month,$day,$year);
$day_text = date('D', $curr_mktime); //current day as text (e.g. 'Fri')

$curr_mon_first_day_mktime = mktime(0, 0, 0, $month, 1, $year); //timestamp for the 1st day of the current month/year
$curr_mon_first_day = 1; //first day of any month
$curr_mon_first_day_text = date('D', $curr_mon_first_day_mktime); //fist day of current month/year as text

$prev_mktime = mktime(0,0,0,$month-1,$day,$year);
$prev_day   = date('d', $prev_mktime);
$prev_month = date('m', $prev_mktime);
$prev_year  = date('Y', $prev_mktime);

$day_name_arr = array('Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat');
?> 
<form name=calendar cellpadding=0 cellspacing=0 border=0 method=POST>
   <input type=hidden name=day value="<?=$day?>">
<? 
$num_get_vars = count($HTTP_GET_VARS);
if ( $num_get_vars >0)
   foreach ($HTTP_GET_VARS as $key => $val)
   {  if ( 'day' != $key && 'month' != $key && 'year' != $key && $key != 'form_submitted') 
      {
?> <input type="hidden" name="<?=$key;?>" value="<?=$val?>">
<?    }
   }
?>
   <table cellpadding=2 cellspacing=0 border=0 bgcolor=#2761A4>
      <tr>
         <td bgcolor=#2761A4 colspan=7>
            <select name="month" onChange="this.form.submit()">
<?             for ( $i=1; $i<13; $i++ )
               {        
?>                <option value=<?=$i?> <? if ($i == $month) echo " selected";?>><?=m2month($i);?>
<?             }
?>
            </select>            
            <select name="year" onChange="this.form.submit()">
<?             $min_date = date('Y') - 2;   
               $max_date = date('Y');   

               for ( $i=$min_date; $i<=$max_date; $i++ )
               {        
?>             <option value="<?=$i?>" <? if ($i == $year) echo " selected"; ?>><?=$i;?>
<?             }
?>          </select><br/>
         </td>
      </tr>
      <tr>
<? foreach ( $day_name_arr as $key => $val )
   {
?>       <td bgcolor=#2761A4 width=25><div  align=center style="color:white;position: relative; background-color:#2761A4; width:28; height:20;"><?=$val?></td>
<? }  ?>
      </tr>
      <tr>
<? $key = 0;
   if ( $curr_mon_first_day_text != $day_name_arr[0] ) //if first day of curr month is not sunday
   {  $day_name_arr = array('Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat');
      $key = array_search($curr_mon_first_day_text, $day_name_arr); //find key of matching weekday name 
?>       <td bgcolor="#efefef" colspan="<?=$key?>">&nbsp;</td>
<? }   
      
   for ( $i=1; $i<=7-$key; $i++ )
   {  if ( $day == $i )
         $colors = "background-color: blue; color: white;";   
      else
         $colors = "background-color: #efefef; color: black;";

      
?>       <td bgcolor=#efefef align=center width=28><div id="caldate<?=$i;?>" align=center style="position: relative; <?=$colors?> width:20; height:20;" onmouseover="doHighlight(this)" onmouseout="doUnHighlight(this)" onClick="clearDates();doSelect(this)" ><?=$i;?></div></td>
<? }      

?>    </tr>
      <tr>
<? $filled=1;
   for ( ; $i<32 ; $i++ )
   {  if ( !checkDate($month, $i, $year) )
      {  
?>       <td bgcolor=#efefef colspan="<?=(7-$filled+1)?>"></td>
<?
         break;                                 
      }
      
      if ( $filled > 7 )
      {  $filled=1;   
?>    </tr>
      <tr>
<?    }
   
      if ( $day == $i )
         $colors = "background-color: blue; color: #efefef;";   
      else
         $colors = "background-color: #efefef; color: black;";
         
      
?>          <td align=center bgcolor=#efefef width=28><div id="caldate<?=$i;?>" align=center style="position: relative; <?=$colors?> width:20; height:20;" onmouseover="doHighlight(this)" onmouseout="doUnHighlight(this)" onClick="clearDates();doSelect(this)" ><?=$i;?></div></td>
<?    
      if ( 31 == $i )
      {
?><td bgcolor=#efefef colspan="<?=(7-$filled)?>"></td>
<?    
      }
      $filled++;
   } 
?>
      </tr>
      <tr>
         <td colspan=7 align=center>
            <input type="button" value="OK" onclick="seedTargetForm(this.form)">
         </td>
      </tr>
   </table>
   <br/>
</form>
</body>
</html>
