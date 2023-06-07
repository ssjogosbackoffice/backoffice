<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" 
     "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
  <title><?=$title?></title>
<?php
  include_once('admin/javascript_css.inc');
  include_once('admin/style.css');
  // query string (minus language argument)
  $qs = mb_ereg_replace('[&]*set_language=[^&]*', '', $_SERVER['QUERY_STRING']);
  if ( $head_include ) {
    require_once ($head_include);
  }
?>
</head>

<body style="background-color:white" leftmargin="0" topmargin="0" rightmargin="0" bottommargin="0" marginwidth="0" marginheight="0"
<?php

    if ( !empty($onload) ) {
        $onload_code = "$onload;";
    }
    if ( $_SESSION['restrict_access_msg'] )
    {	
        unset ($_SESSION['restrict_access_msg']);
        $jsmsg = mb_ereg_replace("'", "\'", "You do not have the proper privileges to access the section you requested");
        $onload_code .= "alert('".$jsmsg."')";
    } else {
        $jalert  = empty($alert) ? $_SESSION['alert_message'] : $alert;
        
        if ( !empty($jalert) ) {
            $onload_code .= "alert('".$jalert."')";
            unset ($_SESSION['alert_message']);
        }
    }
    
    
    if ( !empty($onload_code) ) {
      echo 'onload ="'.$onload_code.'"';
    }
?>>
  <div>
    <table cellpadding=0 cellspacing=0 border=0	 width="100%">
    <tr>
      <td colspan=3 bgcolor=#AC0906><img src="<?=image_dir;?>/clear.gif" width=1 height=10></td>
    </tr>
    <tr height="60" valign="top">
      <td width="171"><img src="<?=image_dir?>/header_logo_seclev.gif" width="171" height="60" alt="" border="0"></a></td>
      <td bgcolor="black" height=2><table cellpadding=0 cellspacing=0 border=0>
        <tr>
	  <td bgcolor=#AC0906><img src="<?=image_dir;?>/clear.gif" width=1 height=14></td>
	</tr>
        <tr> 
          <td height=34 bgcolor=black valign=bottom><div style="color:yellow;background-color:black;font-size:14pt">&nbsp;&nbsp;&nbsp;&nbsp;Administration </div></td>
        </tr>
	</table>
      </td>
      <td width="100%"><table cellpadding=0 cellspacing=0 border=0 width="100%">
        <tr>
	  <td bgcolor="#AC0906"><img src="<?=image_dir;?>/clear.gif" width=1 height=14></td>
	</tr>
        <tr> 
          <td height=46 align=right bgcolor=black valign=middle>
            <div style="background-color:black;color:white;font-size:8pt;font-weight:bold">&nbsp;LANGUAGES:&nbsp;
              <?php
                if(is_array($site_avail_langs)) {
                  foreach($site_avail_langs as $lang_name => $lang_code) {
              ?>
                    <a href="<?=SELF;?>?<?=$qs?>&set_language=<?=$lang_code?>"><img src="<?=image_dir?>/flag_<?=strtolower($lang_code)?>.gif" alt="<?=replace_quotes($lang_name)?>" border="0" align="absmiddle"></a>
              <?php
                  }
                }
              ?>&nbsp;
            </div></td>
	</tr>
     </table>
     </td>
     <tr>
   </table>
 </div>
 <div>
  <table cellpadding=0 cellspacing=0 border=0 width="100%" bgcolor="#cccccc">
   <tr>
    <td>&nbsp;</td>
    <td style="font-size:8pt">
      <span style="font-style:italic">Server Time:</span> <span style="color:blue"><? echo date('j/m/Y H:i:s');?></span>
      &nbsp;&nbsp;|&nbsp;&nbsp;					        
      <? echo gmdate('j/m/Y H:i:s');?> UTC</span>
    </td>
    <td align=right>
      <?include(include_content('login_logout.inc'));?>
    </td>
   </tr>
  </table>
 </div>		
 <table cellpadding=0 cellspacing=0 border=0 width="100%">
 <tr>
  <td align="left" valign="top">
    <?php include('pda/menubar.inc'); ?>
  </td>
  <td align="center" valign="top">
    <table cellpadding=10 cellspacing=0 border=0 width="100%">
    <tr>
      <td align="left"><br /><br />
