    <div class=bodyHD><?=$lang->getLang("Administrator Task Definitions")?></div>
    <br />
    <table cellpadding=4 cellspacing=1 border="0">
        <tr valign="top">
            <td>

                <table cellpadding=4 cellspacing=1 border="0">
                    <tr>
                        <td class="label"><?=$lang->getLang("Task Code")?></td>
                        <td class="label"><?=$lang->getLang("Task Description")?></td>
                        <td class="label"><?=$lang->getLang("Administrator Users with Access")?></td>
                    </tr>
                    <?php echo get_task_list(); ?>
                </table>
            </td>
            <td>
                <input type="button" style="width:80px;margin-bottom:2px" value="<?=$lang->getLang('Add task')?>" onClick="window.location='<?=$_SERVER['PHP_SELF'];?>?op=Add'"/>
            </td>
        </tr>
    </table>