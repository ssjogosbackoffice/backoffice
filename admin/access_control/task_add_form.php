<?php if ( areErrors() ) { showErrors(); echo "<br /><br />";}?>
<script type="text/javascript">
    function sendForm(operation){
        $('#sub_save').val(operation);
    }
</script>
<form action="<?=$_SERVER['PHP_SELF']?>" id="task_form" method="POST">
    <input type="hidden" name="op" value="add">
    <input type="hidden" name="do_process" value="yes">
    <table cellpadding=4 cellspacing=1 border="0">
        <tr valign="top">
            <td class="label"><?=$lang->getLang("Task Description")?></td>
            <td class="content"><input type=text id="task_code" name="task_code" size="26" value="<?php if ( areErrors() ) { echo $_POST['task_code']; }?>"></td>
            <td rowspan="3">
                &nbsp;
            </td>
            <td rowspan="3" width="160">
                <input type="submit" id="sub_save" name="op" onclick="sendForm('Save');" style="width:60px;margin-bottom:2px" value="<?=$lang->getLang("Save")?>" />
                <br />
                <input type="submit" id="sub_cancel" name="op" style="width:60px" value="Cancel" />
            </td>
        </tr>
        <tr valign="top">
            <td class="label"><?=$lang->getLang("Description")?></td>
            <td class="content"><textarea id="task_description" name="task_description" cols="30" rows="10"><?php if ( areErrors() ) { echo $_POST['task_description']; }?></textarea></td>
        </tr>
        <tr valign="top">
            <td class=label><?=$lang->getLang("Administrator Users with Access")?></td>
            <td class=content>
                <?php require_once 'user_type_check_list.php'?>
            </td>
            <td>&nbsp;</td>
        </tr>
    </table>
</form>