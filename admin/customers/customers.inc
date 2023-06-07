<?check_access('view_customers', $redirect=true);?>
<div class=bodyHD>View Customers</div>
<div class=subnavmenu>
    <b>Main Menu </b>
</div>
<br />
<table cellpadding=0 cellspacing=0 border=0>
    <tr valign=top>
        <td>
            <?php
            // if ($_SESSION["admin_id"] != 1320) {
            ?>
            <table cellpadding=4 cellspacing=1 border=0>
                <tr valign=top>
                    <td colspan=3 class=label><strong>Browse customers</strong></td>
                </tr>
                <tr valign=top >
                    <td  class=content><a href="<?=refPage('customers/cust_browse')?>&doQuery=yes">All Customers</a></td>
                </tr>
                <tr valign=top >
                    <td  class=content><a href="<?=refPage('customers/cust_browse')?>&doQuery=yes&member_type=financial">Financial Customers</a></td>
                </tr>
                <tr valign=top>
                    <td  class=content><a href="<?=refPage('customers/cust_browse')?>&doQuery=yes&member_type=nonfinancial">Nonfinancial Customers</a></td>
                </tr>
                <tr valign=top>
                    <td class=content><a href="<?=refPage('customers/cust_browse')?>&doQuery=yes&account_activated=no">Unactivated Accounts</a></td>
                </tr>
                <tr valign=top>
                    <td class=content><a href="<?=refPage('customers/cust_browse')?>&doQuery=yes&account_locked=true">Locked Accounts</a></td>
                </tr>
                <tr valign=top>
                    <td class=content><a href="<?=refPage('customers/cust_browse')?>&doQuery=yes&account_investigated=true">Accounts under Investigation</a></td>
                </tr>
                <tr valign=top>
                    <td class=content><a href="<?=refPage('customers/cust_browse')?>&doQuery=yes&transient=true">Transient Customers</a></td>
                </tr>
                <!--
<?	//	$num_domains = $dbh->countQuery("select count(distinct substring(pun_email from position('@' in pun_email)+1 for char_length(pun_email))) from punter ".( $_SESSION['betting_club'] ? " where pun_betting_club = ".$_SESSION['betting_club'] : '')); ?>
            <tr valign=top>
               <td class=label>Unique Email Domains</font></td>
               <td class=content align=right><?=$num_domains?></td>
               <td class=content>
<? //            if ( $num_domains > 0 )
   //               printLink(refPage('customers/browse_domains').'&doQuery=yes', "[Browse]", 'contentlink');
   //            else
   //               echo '&nbsp;';
?>
               </td>
             </tr>-->
            </table>
            <?php
            //}
            ?>
        </td>
        <td width=50>
            &nbsp;
        </td>
        <td>
            <? showErrors(); ?>
            <table cellpadding=4 cellspacing=1 border=0>
                <form action="<?=SELF;?>" method="POST">
                    <input type="hidden" name="page" value="customers/cust_search_quick">
                    <input type="hidden" name="start_search" value="yes">
                    <input type="hidden" name="doQuery" value="yes">
                    <input type="hidden" name="max_rows" value="30">
                    <tr>
                        <td width=100 class=label>Quick Search</td>
                        <td class=content><input type="text" name="search_string" value="<?=$search_string?>">&nbsp;</span></td>
                    </tr>
                    <tr>
                        <td width=100>&nbsp;</td>
                        <td>
                            <input type="image" src="<?=image_dir?>/button_submit.gif">
                        </td>
                    </tr>
                </form>
            </table>
            <br><br><br>
            <!--<a href="<?=refPage("customers/modify_account/add")?>"><img src="<?=image_dir?>/button_add.gif" border=0></a>-->
        </td>
        <?php
        //if ($_SESSION["admin_id"] != 1320) {
        ?>
        <td>&nbsp;<? printLink(refPage('customers/cust_search_advanced'), "Advanced Search.", "_self",8);?></td>
        <?php
        //}
        ?>
    </tr>
</table>
