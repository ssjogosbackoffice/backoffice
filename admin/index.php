<?php
// query string (minus language argument)
$qs = mb_ereg_replace('[&]*set_language=[^&]*', '', $_SERVER['QUERY_STRING']);

ob_start();
globalise('page');
@include $page."_head.inc";
include "header.inc";
?>


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
            <!-- Administration -->
        </td>
    </tr>
</table>

</body>
</html> 
