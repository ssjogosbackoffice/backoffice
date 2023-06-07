<table cellpadding="0" cellspacing="0" border="0" width="239" background="<?=image_dir?>/clear.gif">
	   <tr>
		   <td  colspan="2" valign="BOTTOM"  bgcolor="#0E240E">&nbsp;&nbsp;&nbsp;<font face="Verdana" size="1" color="#FFFFFF"><b><img src="<?=image_dir?>/side_nav_ball.gif" width="10" height="10" alt="" border="0"> &nbsp;<b>GAMES</b></font><br><img src="<?=image_dir?>/clear.gif" width=100% height=1 alt="" border=0 vspace=0 align="bottom"></td>
	   </tr>
    <tr bgcolor="white">
	   <td width=10 bgcolor="#FCF7D8"><img src="<?=image_dir;?>/clear_dot.gif" width=10 height=1></td>
		 <td bgcolor="#FCF7D8"><br>
         <? printNavLink(refPage('games/stats'), "Game Activity Reports");?>
         <? printNavLink(refPage('games/game_num_search'), "Find a Game (Number)");?>
         <? printNavLink(refPage('games/config'), "Game Configuration");?>
         <? printNavLink("JavaScript:openWindow('".secure_host."/admin/game_monitor/casino_monitor.html', 'colonyrealtimemonitor', 800, 600, 'no', 'no')", "Realtime Game Monitor");?></b><br>
         </td>
	   </tr>
      <tr>
		 <td colspan="2" valign="BOTTOM" bgcolor="#0E240E">&nbsp;&nbsp;&nbsp;<font face="Verdana" size="1" color="#FFFFFF"><b><img src="<?=image_dir?>/side_nav_ball.gif" width="10" height="10" alt="" border="0"> &nbsp;<b>CUSTOMERS</b></font><br><img src="<?=image_dir?>/clear.gif" width=100% height=1 alt="" border=0 vspace=0 align="bottom"></td>
	   </tr>
      <tr bgcolor="white">
	   <td width=10 bgcolor="#FCF7D8"><img src="<?=image_dir;?>/clear_dot.gif" width=10 height=1></td>
		 <td bgcolor="#FCF7D8"><br>
         <? printNavLink(refPage('customers/customers'), "View Customers");?>
         <? printNavLink(refPage('customers/cust_group_admin'), "Manage Groups");?>
         <? printNavLink(refPage('customers/add'), "Register a Customer");?>
         
<!--     <? printNavLink("/admin/customers/cust_account_check.html", "Customer Account Check");?>
        
         -->
         <br>
		 </td>
	   </tr>
	   <tr>
		 <td  colspan="2" valign="BOTTOM" bgcolor="#0E240E">&nbsp;&nbsp;&nbsp;<font face="Verdana" size="1" color="#FFFFFF"><b><img src="<?=image_dir?>/side_nav_ball.gif" width="10" height="10" alt="" border="0"> &nbsp;<b>TRANSACTION TOOLS</b></font><br><img src="<?=image_dir?>/clear.gif" width=100% height=1 alt="" border=0 vspace=0 align="bottom"></td>
	   </tr>
	   <tr bgcolor="white">
	   <td width=10 bgcolor="#FCF7D8"><img src="<?=image_dir;?>/clear_dot.gif" width=10 height=1></td>
		 <td bgcolor="#FCF7D8"><br>
		 <? printNavLink(refPage('cashier_desk/cashdesk_deposit'), "Cashdesk Deposit");?>
         <? printNavLink(refPage('cashier_desk/cashdesk_withdrawal'), "Cashdesk Withdrawal");?>
         <? printNavLink("javascript:openWindow('/admin/notification-window_pop.php', 'Notifications', 700, 400,'yes','yes')", "Popup Pending Transfers"); ?><br>
         </td>
	   </tr>

	   <tr>
		 <td  colspan="2" valign="BOTTOM" bgcolor="#0E240E">&nbsp;&nbsp;&nbsp;<font face="Verdana" size="1" color="#FFFFFF"><b><img src="<?=image_dir?>/side_nav_ball.gif" width="10" height="10" alt="" border="0"> &nbsp;<b>FINANCIAL REPORTS</b></font><br><img src="<?=image_dir?>/clear.gif" width=100% height=1 alt="" border=0 vspace=0 align="bottom"></td>
	   </tr>
	   <tr bgcolor="white">
	   <td width=10 bgcolor="#FCF7D8"><img src="<?=image_dir;?>/clear_dot.gif" width=10 height=1></td>
		 <td bgcolor="#FCF7D8"><br>
         <? printNavLink(refPage('financial_reports/financial_transactions'), "Financial Transactions Report");?>
         <? printNavLink(refPage('financial_reports/cclog'), "Credit Card Transactions Report");?>
         </td>
	   </tr>
	   <tr>
		 <td  colspan="2" valign="BOTTOM" bgcolor="#0E240E">&nbsp;&nbsp;&nbsp;<font face="Verdana" size="1" color="#FFFFFF"><b><img src="<?=image_dir?>/side_nav_ball.gif" width="10" height="10" alt="" border="0"> &nbsp;<b>SECURITY CHECK</b></font><br><img src="<?=image_dir?>/clear.gif" width=100% height=1 alt="" border=0 vspace=0 align="bottom"></td>
	   </tr>
	   <tr bgcolor="white">
	   <td width=10 bgcolor="#FCF7D8"><img src="<?=image_dir;?>/clear_dot.gif" width=10 height=1></td>
		 <td bgcolor="#FCF7D8"><br>
         <? printNavLink(refPage('security-check/large_transactions'), "Large Deposits or Withdrawals");?>
         <? printNavLink(refPage('security-check/frequent_transactions'), "Frequent Transactions");?>
         <? //printNavLink(refPage('security-check/ip_check'), "Different Accounts, same IP");?>
         </td>
	   </tr>
	   <tr>
		 <td  colspan="2" valign="BOTTOM"  bgcolor="#0E240E">&nbsp;&nbsp;&nbsp;<font face="Verdana" size="1" color="#FFFFFF"><b><img src="<?=image_dir?>/side_nav_ball.gif" width="10" height="10" alt="" border="0"> &nbsp;<b>SALES & MARKETING</b></font><br><img src="<?=image_dir?>/clear.gif" width=100% height=1 alt="" border=0 vspace=0 align="bottom"></td>
	   </tr>
	   <tr  bgcolor="#FCF7D8">
	   <td width=10 bgcolor="#FCF7D8"><img src="<?=image_dir;?>/clear_dot.gif" width=10 height=1></td>
		 <td><br>
         <?printNavLink(refPage("sales_reps/sales_reps"), "Sales Representatives");?><br>
<!--		   &nbsp;<?printNavLink("/adminadvert_compare.html", "Advertising Success Statistics");?><br>         
		   &nbsp;<?printNavLink("/adminreferrals_admin_index.html", "Customer Referrals");?><br><br>-->
		 </td>
	   </tr>
		<tr>
		 <td  colspan="2" valign="BOTTOM" bgcolor="#0E240E">&nbsp;&nbsp;&nbsp;<font face="Verdana" size="1" color="#FFFFFF"><b><img src="<?=image_dir?>/side_nav_ball.gif" width="10" height="10" alt="" border="0"> &nbsp;<b>WEBSITE ADMINISTRATION</b></font><br><img src="<?=image_dir?>/clear.gif" width=100% height=1 alt="" border=0 vspace=0 align="bottom"></td>
	   </tr>
	   <tr  bgcolor="#FCF7D8">
		   <td width=10 bgcolor="#FCF7D8"><img src="<?=image_dir;?>/clear_dot.gif" width=10 height=1></td>
		   <td><br>
            <? printNavLink(secure_host.'/admin/webalizer/', 'Webalizer Usage Statistics', '_new');?>
		    <? printNavLink(refPage("admin_users/admin_users","admin_users/admin_users_head"), "Administrator Users"); ?>
			<? printNavLink(refPage("web_content/news_admin"), "Website News");?>
            <? printNavLink(refPage("web_content/transfer_charges"), "Bank Transfer Charges");?>
            <? printNavLink(refPage("web_content/firepay_maintenance"), "Firepay Maintenance Schedule");?>
            <? //printNavLink(refPage("web_content/promotions"), "Promotions");?><br><br><br>            
         </td>
	   </tr>
   </table>
