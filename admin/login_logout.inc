<?php
if ( isLoggedIn() && $page != 'logout' ) {
    ?><div>
    <table border="0" cellspacing="0" cellpadding="0" bgcolor=#cccccc>
        <tr valign=top>
            <td><table cellpadding=0 cellspacing=0 border=0>
                    <tr>
                        <td style="font-size:8pt;"><b><?=$lang->getLang("Username")?>:</b>&nbsp;</td>
                        <td style="font-size:8pt;"><?=$_SESSION['admin_username']?></td>
                    </tr>
                    <tr>
                        <td style="font-size:8pt;"><b><?=$lang->getLang("User Type")?>:</b>&nbsp;</td>
                        <td style="font-size:8pt;"><?=ucwords(strtolower($_SESSION['aty_code']))?></td>
                    </tr>
                </table>
            </td>
            <td>&nbsp;&nbsp;&nbsp;</td>
            <td><a class="small" href="<?=refPage('logout');?>">Logout</a> | <a class="small" href="<?=refPage('change-password');?>"><?=$lang->getLang("Change Password")?> </a> </td>
        </tr>
    </table>
    </div>
<?php
}
?>
