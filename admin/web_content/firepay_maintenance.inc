<div  class=level2heading>Firepay Maintenance Schedule</div>
<P>
<?

function checkFields ($day, $month, $year, $hour, $min, $hr_duration, $min_duration)
{  if ( ! $day || ! $month || ! $year || strlen($hour) < 1 || strlen($min) < 1 )
      addError("", "All the fields of the start date and time must be selected");
   elseif ( ! checkDate($month, $day, $year) ) 
      addError("", "The maintenance start date you selected is not a valid calendar date");

	if ( $hr_duration && $min_duration )
	{  if ( !isInteger($hr_duration) || $hr_duration < 1 || $hr_duration > 24 )
		  addError("", "The maintenance duration (hours) must be a whole number between 1 and 24");
	   if ( !isInteger($min_duration) || $min_duration < 1 || $min_duration > 59 )
		  addError("", "The maintenance duration (minutes) must be a whole number between 1 and 59");
	}
	elseif ( $hr_duration )
	{  	if ( !isInteger($hr_duration) || $hr_duration < 1 || $hr_duration > 24 )
		  addError("", "The maintenance duration (hours) must be a whole number between 1 and 24");		
	}
	elseif ( $min_duration )
	{	if ( !isInteger($min_duration) || $min_duration < 1 || $min_duration > 59 )
		   addError("", "The maintenance duration (minutes) must be a whole number between 1 and 59");
	}
	else
	  addError("", "The maintenance duration in hours and/or minutes must be entered");
   
   return ! areErrors();
}

$cfg_filename = CONFIG_DIR."/firepay_maintenance.cfg";
$cfg = getCfgFile($cfg_filename);

globalize('day');
globalize('month');
globalize('year');
globalize('hour');
globalize('min');
globalize('min_duration');
globalize('hr_duration');
globalize('mode');

if ( 'save' == $mode )
{  if ( checkFields($day, $month, $year, $hour, $min, $hr_duration, $min_duration) )
   {  $hour = addLeadingZero($hour);
      $min = addLeadingZero($min);
      $sec = "00";
      $time = "$hour:$min:$sec";  
      $cfg['start_time'] = toDatabaseTimestamp ($day, $month, $year, $time); 
      $cfg['hr_duration'] = $hr_duration;
      $cfg['min_duration'] = $min_duration;      
      
      if ( !$auto_clear )
         $auto_clear = 'off';
      $cfg['auto_clear'] = $auto_clear;

      $cfg_w_handle = fopen($cfg_filename, 'w');

      if ( $cfg_w_handle )
      {  foreach ( $cfg as $key => $val )
         {  $str = "$key=$val\n";
            fputs($cfg_w_handle, $str);
         }
         fclose ($cfg_w_handle);
         $HTTP_SESSION_VARS['alert_message']="Firepay Maintenance scheduled successfully";
      }
      else
         dieWithError("Could not open config file for writing");
   }
   else 
   {   $mode = 'modify';
   		$last_mode = 'save';
   }
}
elseif ( 'clear' ==  $mode )  //clear cfg file
{  $cfg_w_handle = fopen($cfg_filename, 'w');
   if ( $cfg_w_handle )
      fclose ($cfg_w_handle);
   $HTTP_SESSION_VARS['alert_message']="Firepay Maintenance schedule cleared successfully";
   unset($cfg);
}

if ('modify' == $mode )
{  if ( $cfg && $last_mode != 'save')
   {  $start_time = $cfg['start_time'];
      $hr_duration = $cfg['hr_duration'];
      $min_duration = $cfg['min_duration'];
      $auto_clear = $cfg['auto_clear'];
   
      $date_arr = splitDate($start_time);
      $day   = $date_arr['day'];
      $month = $date_arr['month'];
      $year  = $date_arr['year'];
      $hour  = $date_arr['hour'];
      $min   = $date_arr['minute'];
   }   
   //$end_mktime = mktime($hour+$duration, $min, 0, $month, $day, $year);
   //$end_time = phpTsToDbts($end_mktime, true);
?> 
   <P>
   <?showErrors()?>
   <P>
   <?form_head()?>
   <?printHiddenInput("mode", "save");?>
   <table cellpadding=4 cellspacing=1 border=0>
      <tr>
         <td class=formheading colspan=2>
            Scheduled Firepay Maintenance Period
         </td>
      </tr>
      <tr valign=top>
         <td class=label>Maintenance start date and time<br><span class=labelnote>(EST)</span></td>
         <td class=content><?date_selector ('day', 'month', 'year', date('Y'), date('Y')+1, $default="", $options="")?> &nbsp;<?time_selector ('hour', 'min')?></td>
      </tr>
      <tr valign=top>
         <td class=label>Maximum duration</td>
         <td class=content>
         	<input type="text" name="hr_duration" size=2 value="<?=$hr_duration?>"> hours
         	&nbsp;&nbsp;&nbsp;&nbsp;
         	<input type="text" name="min_duration" size=2 value="<?=$min_duration?>"> minutes
         </td>
      </tr>
      <tr valign=top>
         <td class=label>Auto clear schedule</span></td>
         <td class=content><input type="checkbox" name="auto_clear" <?if ( 'on' == $auto_clear ) echo 'checked';?>></td>
      </tr>
      <tr valign=top>
         <td></td>
         <td>
<? if ( $cfg ) { ?>
            <input type=image src="<?=image_dir;?>/button_update.gif" border=0>
<? } else {  ?>            
            <input type=image src="<?=image_dir;?>/button_save.gif" border=0>
<? } ?>            
            
            
            <a href="<?=refPage($page);?>"><img src="<?=image_dir;?>/button_cancel.gif" border=0></a> 
         </td>
      </tr>
   </table>
<?
}

if ( 'modify' != $mode )
{  if ( $cfg )
   {  $start_time = $cfg['start_time'];
      $hr_duration = $cfg['hr_duration'];
      $min_duration = $cfg['min_duration'];
      $auto_clear = $cfg['auto_clear'];

      $date_arr = splitDate($start_time);
      $day   = $date_arr['day'];
      $month = $date_arr['month'];
      $year  = $date_arr['year'];
      $hour  = $date_arr['hour'];
      $min   = $date_arr['minute'];
      
      $end_mktime = mktime($hour+$hr_duration, $min+$min_duration, 0, $month, $day, $year);
      $end_time = phpTsToDbts($end_mktime, true);

      ?>
      <table cellpadding=4 cellspacing=1 border=0>
         <tr>
            <td class=formheading colspan=2>
               Scheduled Firepay Maintenance Period
            </td>
         </tr>
         <tr valign=top>
            <td class=label>Start date and time</td>
            <td class=content><?=shortDate($start_time, true);?></td>
         </tr>
         <tr valign=top>
            <td class=label>End date and time</td>
            <td class=content><?=shortDate($end_time, true);?></td>
         </tr>
	     <tr valign=top>
	     	<td class=label>Maximum duration</td>
	        <td class=content>
	        	<? if ( $hr_duration ) { ?>
	         		<?=$hr_duration?> hours      	
	         	<? } ?>
	         	<? if ( $hr_duration && $min_duration ) echo " and "; ?>
	         	<? if ( $min_duration ) { ?>
	         		<?=$min_duration?> minutes      	
	         	<? } ?>
	        </td>
	     </tr>
         <tr valign=top>
            <td class=label>Auto clear schedule</td>
            <td class=content><?=$auto_clear?></td>
         </tr>
         <tr valign=top>
            <td colspan=2 align=center>
               <br>
               <a href="<?=refPage("$page&mode=modify");?>"><img src="<?=image_dir;?>/button_change.gif" border=0></a>  
			   <a href="<?=refPage("$page&mode=clear");?>"><img src="<?=image_dir;?>/button_clear.gif" border=0></a>
            </td>
         </tr>
      </table>
      <P>
<? }
   else
   {   printInfoMessage("No Firepay Maintenance currently scheduled");
?>
      <P>
      <?showErrors()?>
      <P>
      <?form_head()?>
      <?printHiddenInput("mode", "save");?>
      <table cellpadding=4 cellspacing=1 border=0>
         <tr>
            <td class=formheading colspan=2>
               Schedule a Firepay Maintenance Period 
            </td>
         </tr>
         <tr valign=top>
            <td class=label>Maintenance start date and time<br><span class=labelnote>(EST)</span></td>
            <td class=content><?date_selector ('day', 'month', 'year', date('Y'), date('Y')+1, $default="", $options="")?> &nbsp;<?time_selector ('hour', 'min')?></td>
         </tr>
      <tr valign=top>
         <td class=label>Maximum duration</td>
         <td class=content>
         	<input type="text" name="hr_duration" size=2 value="<?=$hr_duration?>"> hours
         	&nbsp;&nbsp;&nbsp;&nbsp;
         	<input type="text" name="min_duration" size=2 value="<?=$min_duration?>"> minutes
         </td>
      </tr>
         <tr valign=top>
            <td class=label>Auto clear schedule</span></td>
            <td class=content><input type="checkbox" name="auto_clear" <?if ( 'on' == $auto_clear ) echo 'checked';?>></td>
         </tr>
         <tr valign=top>
            <td>&nbsp;</td>
            <td>
               <input type="image" src="<?=image_dir?>/button_submit.gif">
            </td>
         </tr>
      </table>
<? }
}
?>