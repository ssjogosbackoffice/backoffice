<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head> 
  <title><?=$title?></title>
  <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/style.css" title="core_style" />
  <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jquery-ui.css" title="ui-core" />
  <script src="<?= secure_host ?>/media/jfunctions.js" type="text/javascript"></script>
  <script src="<?= secure_host ?>/media/stm31.js" type="text/javascript"></script>
  <script src="<?= secure_host ?>/media/jquery.js" type="text/javascript"></script>
  <script src="<?= secure_host ?>/media/jquery-ui-min.js" type="text/javascript"></script>
  <?php
    if (!empty($more_headers)) {
      print $more_headers;
    } 
    // query string (minus language argument)
    $qs = mb_ereg_replace('[&]*set_language=[^&]*', '', $_SERVER['QUERY_STRING']);
    if ( $head_include ) {
      require_once ($head_include);
    }
  ?>
</head>

<body onload="initFunction()" style="background-color:white" leftmargin="0" topmargin="0" rightmargin="0" bottommargin="0" marginwidth="0" marginheight="0"
<?php
    if ( !empty($onload) ) {
        $onload_code = "$onload;";
    }
    if ( $_SESSION['restrict_access_msg'] ){	
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
    
    if ( !empty($onload_code) )
    {   echo 'onload ="'.$onload_code.'"';
    }
?>>
  <div>
  <div id="header">
	  <a href="/?page=lastaccess"><img src="<?=image_dir?>/header_cards.gif"/></a>
	  <div id="aams"><img src="<?=image_dir?>/header_aams.gif"/></div>
	  <div id="contacts">
	    <a href="skype:italypokerassistenza1?chat"><img src="<?=image_dir?>/skype.png"/> italypokerassistenza1</a><br/>
	    <a href="skype:italypokerassistenza2?chat"><img src="<?=image_dir?>/skype.png"/> italypokerassistenza2</a>
	  </div>
  </div>
  <div id="main_menu">
  <table cellpadding="0" cellspacing="0" border="0" width="100%" id="menubar">
  <tr>
    <td>
    <td>
      <? include(include_content('menubar.inc')); ?>
    </td>
    <td align="right">
      <?
        if(!empty($_SESSION['admin_id'])){
         $messages = checkMessages();
         $found    = 0;
         $found_id = 0;

         foreach($messages as $message){
           if($message["aum_level"] >= 1 && $message["aum_level"] > $found){
             $found    = $message["aum_level"];
             $found_id = $message["aum_id"];
           }
         }
         
         if($_REQUEST["page"] != "message/manager"){
           if($found == 1){
             if(!$_SESSION["just_logged"]){
               jalert("Controlla la lista messaggi, ci sono dei messaggi importanti per te!");
               $_SESSION["just_logged"] = true;
             }
           }elseif($found == 2){
            ?>
            <script type="text/javascript">
            alert("Devi leggere il seguente messaggio per poter continuare.");
            location.href = "<?=refPage('message/manager&action=read&id=' . $found_id)?>";
            </script>
            <?php
           }
         }
         
         if(count($messages) > 0){
           ?>
           <span id="nav">
           <a href="<?=refPage('message/manager')?>">Messaggi (<?=count($messages)?>) <img src="<?=image_dir?>/email.gif" />&nbsp;</a>
           </span>
           <?
         }else{
           ?>
           <span id="nav">
           <a href="<?=refPage('message/manager')?>">Messaggi (<?=count($messages)?>) <img src="<?=image_dir?>/email_read.gif" />&nbsp;</a>
           </span>
           <?
         }
        }
      ?>
   </td>    
    <td style="font-size:8pt">
      <span><? echo date('j/m/Y H:i:s');?></span>
    </td>
    <td align=right>
      <?include(include_content('login_logout.inc'));?>
    </td>
</tr>
</table>
</div>
</div>		
<table cellpadding=0 cellspacing=0 border=0 width="100%">
<tr>
  <td align="center">
    <table cellpadding=10 cellspacing=0 border=0>
    <tr>
      <td align="left"><br /><br />
