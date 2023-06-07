<div  class=level2heading>Bank Transfer Charges Configuration</div>
<P>
Edit the fields, and click update to save the configuration.
<P>
<?

function checkFields ($in_flat, $in_percentage, $out_flat_under_2000, $out_percentage_under_2000, $out_flat_2000_plus, $out_percentage_2000_plus)
{  if ( ! isValidMoney($in_flat) ) 
      addError("", "The <u>Incoming transfer charge - Flat</u> amount must be a valid money number (whole or decimal)");

   if ( ! isValidMoney($in_percentage) || $in_percentage >= 100  ) 
      addError("", "The <u>Incoming transfer charge - Percentage</u> must be a valid number under 100");


   if ( ! isValidMoney($out_flat_under_2000) ) 
      addError("", "The <u>Incoming transfer charge - under $2000 - Flat</u> amount must be a valid number (whole or decimal)");

   if ( ! isValidMoney($out_percentage_under_2000) || $out_percentage_under_2000 >= 100  ) 
      addError("", "The <u>Outgoing transfer charge - under $2000 - Percentage</u> must be a valid number under 100");


   if ( ! isValidMoney($out_flat_2000_plus) ) 
      addError("", "The <u>Incoming transfer charge - $2000 and over - Flat</u> amount must be a valid number (whole or decimal)");

   if ( ! isValidMoney($out_percentage_2000_plus) || $out_percentage_2000_plus >= 100  ) 
      addError("", "The <u>Outgoing transfer charge - $2000 and over - Percentage</b> must be a valid number under 100");
   return ! areErrors();
}


$cfg_filename = CONFIG_DIR."/charges.cfg";
$cfg =  getCfgFile($cfg_filename);

if ( 'update' == $mode )
{  
   if ( checkFields($in_flat, $in_percentage, $out_flat_under_2000, $out_percentage_under_2000, $out_flat_2000_plus, $out_percentage_2000_plus) )
   {  
      $cfg['in_flat']                   = $in_flat; 
      $cfg['in_percentage']             = $in_percentage;
      $cfg['out_flat_under_2000']       = $out_flat_under_2000; 
      $cfg['out_percentage_under_2000'] = $out_percentage_under_2000;
      $cfg['out_flat_2000_plus']        = $out_flat_2000_plus;
      $cfg['out_percentage_2000_plus']  = $out_percentage_2000_plus;

      $cfg_w_handle = fopen($cfg_filename, 'w');

      if ( $cfg_w_handle )
      {  foreach ( $cfg as $key => $val )
         {  $str = "$key=$val\n";
            fputs($cfg_w_handle, $str);
         }
         fclose ($cfg_w_handle);

         $alert_message = "Configuration saved successfully";
      }
      else
         dieWithError("Could not open config file for writing");
   }
}
else
{  $in_flat = $cfg['in_flat'];
   $in_percentage = $cfg['in_percentage'];
   $out_flat_under_2000 = $cfg['out_flat_under_2000']; 
   $out_percentage_under_2000 = $cfg['out_percentage_under_2000'];
   $out_flat_2000_plus = $cfg['out_flat_2000_plus'];
   $out_percentage_2000_plus = $cfg['out_percentage_2000_plus'];
}
?>
<P>
<?showErrors()?>
<P>
<?form_head()?>
<?printHiddenInput("mode", "update");?>
<table cellpadding=4 cellspacing=1 border=0>
   <tr valign=top>
      <td class=label>Incoming Transfer Charge</td>
      <td class=content>Flat $<input type="text" name="in_flat" size=17 value="<?=$in_flat?>">&nbsp;&nbsp;+&nbsp;&nbsp;Percentage<input type="text" name="in_percentage" size=5 value="<?=$in_percentage?>">%</td>
   </tr>
   <tr valign=top>
      <td class=label>Outgoing Transfer Charge - under $2000</td>
      <td class=content>Flat $<input type="text" name="out_flat_under_2000" size=17 value="<?=$out_flat_under_2000?>">&nbsp;&nbsp;+&nbsp;&nbsp;Percentage 
         <input type="text" name="out_percentage_under_2000" size="5" value="<?=$out_percentage_under_2000?>">%
      </td>
   </tr>
   <tr valign=top>
      <td class=label>Outgoing Transfer Charge - $2000 and over</td>
      <td class=content>
           Flat  $<input type="text" name="out_flat_2000_plus" size="17" value="<?=$out_flat_2000_plus?>">&nbsp;&nbsp;+&nbsp;&nbsp;Percentage 
           <input type="text" name="out_percentage_2000_plus" size="5" value="<?=$out_percentage_2000_plus?>">%
      </td>
   </tr>
   <tr valign=top>
      <td class=content></td>
      <td class=content>
         <input type="submit" value="Update">
         <input type="button" value="Cancel" onClick="window.location='<?=refPage($page);?>'">
      </td>
   </tr>
</table>
