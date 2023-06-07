<table width="208" height="100%" border="0" cellspacing="0" cellpadding=0 valign="TOP" >
   <tr>
      <td valign="TOP" >
      <img src="<?=image_dir?>/clear.gif" width=239 height=1>
<? include "login_logout.inc"; ?>
<br>
<?  

  $page_name = pageName();

function printNavLink($href, $text, $target="_self") 
{  global $page_name;
   $href_comp = strtok($href,'?');

   if ( $page_name == $href_comp)
   {	echo $text;
   }
   else
   {	
?>		<A HREF="<?=$href?>" target="<?=$target?>" class=sidenav><?=$text?></A>
<? }
   ?>

<img src="<?=image_dir?>/side_nav_line.gif" width="217" height="6" alt="" border="0">

<?
}
if( isLoggedIn() )
{  if ( $page != 'logout' ) 
	   switch ( $_SESSION['aty_code'] )
	   {  case 'SUPERUSER':
	         include 'side_nav_superuser.inc';   
	         break;
	      case 'SUPPORT':
	         include 'side_nav_support.inc';   
	         break;
	      case 'CASINODIRECTOR':
	         include 'side_nav_director.inc';
	         break;
	      case 'CASINOMANAGER':
	         include 'side_nav_manager.inc';
	         break;
	      case 'FINANCIALMANAGER':
	         include 'side_nav_financial_manager.inc';
	         break;
	      case 'ACCOUNTSADMINISTRATOR':
	         include 'side_nav_accts_admin.inc';
	         break;
	      case 'CASINOSALESLEADER':
	         include 'side_nav_salesleader.inc';
	         break;
	      case 'CASHIER':
	         include 'side_nav_cashier.inc';
	         break;
	      case 'EXTERNALSALESREP':
	         include 'side_nav_salesrep.inc';
	         break;
      	         
	   }  
}



?>
      </td>
   </tr>
</table>