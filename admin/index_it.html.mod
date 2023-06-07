<?php
// query string (minus language argument)
$qs = mb_ereg_replace('[&]*set_language=[^&]*', '', $_SERVER['QUERY_STRING']);

ob_start();
globalise('page');
@include $page."_head.inc";
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
  <title>Amministrazione</title>
  <link rel="stylesheet" type="text/css" href="media/style.css" title="core_style" />
  <script src="media/jfunctions.js" type="text/javascript"></script>
  <script src="media/stm31.js" type="text/javascript"></script>
</head>

<body style="background-color:white" leftmargin="0" topmargin="0" rightmargin="0" bottommargin="0" marginwidth="0" marginheight="0" 
<?php
if ( $_SESSION['restrict_access_msg'] ) {
   unset ($_SESSION['restrict_access_msg']);
   $jsmsg = mb_ereg_replace("'", "\'", "Mancano le caratteristiche necessarie per accedere alla sezione richiesta");
?>
onload="alert('<?=$jsmsg?>')"
<?php
}
?>>
  <div>
    <table cellpadding="0" cellspacing="0" border="0" width="100%">
    <tr>
      <td colspan="3" bgcolor="#000000"><img src="<?=image_dir;?>/clear.gif" width=1 height=10></td>
    </tr>
    <tr height="60" valign="top">
      <td width="171"><img src="<?=image_dir?>/header_logo_seclev.gif" width="171" height="60" alt="" border="0"></a></td>
      <td bgcolor=black height=2><table cellpadding=0 cellspacing=0 border=0>
        <tr>
          <td bgcolor="#000000"><img src="<?=image_dir;?>/clear.gif" width=1 height=14></td>
        </tr>
        <tr> 
          <td height=34 bgcolor=black valign=bottom><div style="color:yellow;background-color:black;font-size:14pt">&nbsp;&nbsp;&nbsp;&nbsp;Amministrazione</div></td>
        </tr>
        </table></td>
     <td width="100%"><table cellpadding=0 cellspacing=0 border=0 width="100%">
        <tr>
	  <td bgcolor="#000000"><img src="<?=image_dir;?>/clear.gif" width=1 height=14></td>
	</tr>
	<tr> 
	  <td height=46 align="right" bgcolor="black" valign="middle">
	    <div style="background-color:black;color:white;font-size:8pt;font-weight:bold">
	      &nbsp;<strong>LANGUAGES:</strong>&nbsp;
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
     </table></td>
   <tr>
   </table>
  </div>
  <table cellpadding="0" cellspacing="0" border="0" width="100%" bgcolor="#cccccc">
  <tr>
    <td>
      <? include(include_content('menubar.inc')); ?>
    </td>
    <td>
      <?
        function controlMessage(){
          global $dbh;
          $_SESSION['last_request_message']=time();
          $sql="Select count(aum_id) from admin_user_mailbox where aum_to={$_SESSION['admin_id']}  and aum_read=0";
          return $dbh->countQuery($sql);
        }
        
        if (isLoggedIn() ) {
          $number_message=0;
          $ts = time() - strtotime(str_replace("-","/",$_SESSION['last_request_message'])); 
          if(($ts-$_SESSION['last_request_message'])>60){
            $number_message=controlMessage(); 
            $_SESSION['last_request_message']=time();
          }
          
          $string="<span style='font-style:italic'> Message : ";
          $string.= "<span style='font-style:normal'> <a  style='".(($number_message=="0")?'color:green;':'color:red').
                    "' href='".refPage('message/listMessage')."'>You Have ({$number_message}) to read</a> </span>";
          $string.="</span>";
          echo $string;
        }
      ?>
   </td>
    <td style="font-size:8pt">
      <span style="font-style:italic">Server Time:</span> 
      <span style="color:blue"><? echo date('j/m/Y H:i:s');?></span>
      &nbsp;&nbsp;|&nbsp;&nbsp;					        
      <? echo gmdate('j/m/Y H:i:s');?> UTC</span>
    </td>  
    <td align=right>
      <?include(include_content('login_logout.inc'));?>
    </td>
  </tr>
  </table>
  <br /><br />
  <table align="center">
  <tr>
    <td>
<?php
if ( isLoggedIn() ) {
   include ($page  ? "$page.inc" : "default.inc");
} else {
   include('login.inc');
}

echo ob_get_clean();
?>
<img src="<?=image_dir?>/clear.gif" width=0 height=0 <? 
   if ( $_SESSION['alert_message'] )  {
?> onLoad="alert('<?=ereg_replace("'", "\'", $_SESSION['alert_message']);?>');<?
   if ($code_action) 
      echo $code_action;
?>"<? 
      unset($_SESSION['alert_message']);
   }
?>>
           </td>
       </tr>
   </table>

</body>
</html>
