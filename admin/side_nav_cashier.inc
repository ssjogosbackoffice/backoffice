<table cellpadding="0" cellspacing="0" border="0" width="239" background="<?=image_dir?>/clear.gif">  
	<tr>
		<td  colspan="2" valign="BOTTOM"  bgcolor="#0E240E">&nbsp;&nbsp;&nbsp;<font face="Verdana" size="1" color="#FFFFFF"><b><img src="<?=image_dir?>/side_nav_ball.gif" width="10" height="10" alt="" border="0"> &nbsp;<b>MENU</b></font><br><img src="<?=image_dir?>/clear.gif" width=100% height=1 alt="" border=0 vspace=0 align="bottom"></td>
	</tr>
	<tr bgcolor="white">
	   <td width=10 bgcolor="#FCF7D8"><img src="<?=image_dir;?>/clear_dot.gif" width=10 height=1></td>
		 <td bgcolor="#FCF7D8"><br>
         <? printNavLink(refPage('default'), "Back to Home");?>
         <? printNavLink(refPage('cashier_desk/cashdesk_deposit'), "Cashdesk Deposit");?>
         <? printNavLink(refPage('cashier_desk/cashdesk_withdrawal'), "Cashdesk Withdrawal");?>
         </td>
	   </tr>
   </table>
