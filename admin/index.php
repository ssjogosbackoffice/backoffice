<?php
require_once '../includes/funclib/general_functions.inc';
require_once '../config/backoffice.conf.inc';
// query string (minus language argument)
$qs = mb_ereg_replace('[&]*set_language=[^&]*', '', $_SERVER['QUERY_STRING']);

ob_start();
globalise('page');
@include $page."_head.inc";
require_once "../includes/templates/header.inc";
?>

<table align="center">  
    <tr>
        <td>
            <?php
if ( isLoggedIn() ) {
   require_once $page  ? "$page.inc" : "default.inc";
} else { 
   require_once 'login.inc';
}

echo ob_get_clean();
?>
            <img src="<?=image_dir?>/clear.gif" width=0 height=0 <?
   if ( isset($_SESSION['alert_message']) )  {

?> onLoad="alert('<?=ereg_replace("'", "\'", $_SESSION['alert_message']);?>');<?
   if ($code_action)
      echo $code_action;
?>"<?
      unset($_SESSION['alert_message']);
   }
?>>
            <!-- Administration -->
        </td>
    </tr>
</table>

</body>
</html> 
