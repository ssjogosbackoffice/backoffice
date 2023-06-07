<?php

check_access('manage_admin_users',$redirect=true);
//die("ciao");
?>
<?php
$title = $lang->getLang("Administrator Users");
$more_headers = '<script src="' . secure_host . '/media/admin_users.js.php" type="text/javascript"></script>';
$access= (isset($_POST['access'])) ? $_POST['access'] : '0';
include('header.inc');
?>
<div class="bodyHD"><?=$lang->getLang("Administrator Users")?> <a href="/admin_users/admin_user_add.php"><img align="right" border="0" src="<?=image_dir?>/button_add.gif"></a></div>

<?php if($_SESSION["jurisdiction_class"] != "club" &&  $_SESSION["jurisdiction_class"] != "district") : ?>

    <form action="<?=$_SERVER["PHP_SELF"]?>" method="POST">
        <table>
            <tr>
                <td class="label"><?=$lang->getLang("Jurisdictions")?></td>
                <td class="content">
                    <?php
                    include('jurisdiction_filters.php');
                    include('jurisdiction_selector.php');
                    ?>
                </td>
            </tr>
            <tr>
                <td class="label"><?=$lang->getLang("Search")?></td>
                <td class="content">
                    <input type="text" name="search_string" value="<?=$_POST["search_string"]?>"/>
                </td>
            </tr>

            <tr>
                <td class="label"><?=$lang->getLang("Include users of a sub-level")?></td>
                <td class="content">
                    <input type="checkbox" name="sub-users" <?=($_POST["sub-users"] == "on")?("checked"):("")?>/>
                </td>
            </tr>
            <tr>
                <td class="label"><?=$lang->getLang("Access")?></td>
                <td class="content">
                    <select name="access">
                        <option value="0" <?=($access=="0"? "selected" : "") ?> ><?=$lang->getLang("All")?></option>
                        <option value="1" <?=($access=="1"? "selected" : "") ?> ><?=$lang->getLang("Allow")?></option>
                        <option value="2" <?=($access=="2"? "selected" : "") ?> ><?=$lang->getLang("Deny")?></option>
                    </select>
                </td>
            </tr>
            <tr>
                <td colspan="2"><input type="submit" value="<?=$lang->getLang("Search")?>"/></td>
            </tr>
        </table>
        <input type="hidden" name="submit" value="1"/>
    </form>
<?php else : ?>
    <?php
    $_POST["submit"] = "1";
    ?>
<?php endif; ?>

<?php if($_POST["submit"] == "1") : ?>
    <table cellpadding="0" cellspacing="0">
        <tr valign="top">
            <td>
                <div id="urlContent">
                    <?php include ("admin_user_list.php"); ?>
                </div>
            </td>
            <td>&nbsp;</td>
        </tr>
    </table>
<?php endif; ?>
<div id="disable_pane" style="visibility:hidden;position:absolute;left:0px;top:0px;width:100%;height:100%;z-index:99;filter:alpha(Opacity=10);-moz-opacity:0.1;">
    <input type="hidden" id="tabtaker" />
</div>
<form>
    <input type="hidden" id="orderby" value="name">
    <input type="hidden" id="direction" value="asc">
</form>

<?php
include 'loading_bar_inc.php';
include 'footer.inc';
?>
