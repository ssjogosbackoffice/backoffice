
<tr valign=top  class="by_jurisdiction" style="display:none;">
    <td class='blackTd'><?=$lang->getLang("Jurisdiction")?></td>
    <td <?form_td_style ('jurisdiction')?>>
        <table cellpadding="0" cellspacing="0" border="0">
            <tr>
                <td>
                    <select id="jurisdiction_level_1" name="jurisdiction_level_1" onChange="change_jurisdiction(this, 'jurisdiction_select_1');">
                        <option value="club"<?if ( 'club' == $jurisdiction_level ) echo 'selected="selected"'; ?>><?=$lang->getLang("Club")?> </option>
                        <?php if ( 'casino' == $_SESSION['jurisdiction_class'] || 'nation' == $_SESSION['jurisdiction_class'] || 'region' == $_SESSION['jurisdiction_class'] ) { ?>
                            <option value="district"<?if ( 'district' == $jurisdiction_level ) echo 'selected="selected"'; ?>><?=$lang->getLang("District")?></option>
                        <?php } ?>
                        <?php if ( 'casino' == $_SESSION['jurisdiction_class'] || 'nation' == $_SESSION['jurisdiction_class'] ){ //show region option ?>
                            <option value="region"<?if ( 'region' == $jurisdiction_level || ! $jurisdiction_level ) echo 'selected="selected"'; ?>><?=$lang->getLang("Region")?></option>
                        <?php } ?>
                        <?php if ( 'casino' == $_SESSION['jurisdiction_class'] ){ //show region option ?>
                            <option value="nation"<?if ( 'nation' == $jurisdiction_level || ! $jurisdiction_level ) echo 'selected="selected"'; ?>><?=$lang->getLang("Nation")?></option>
                        <?php } ?>
                    </select>
                </td>
                <td>
                    <div id="jurisdiction_select_1" class="content">
                        <?php echo get_jurisdiction_select($jurisdiction_level, $_SESSION['jurisdiction_id']);
                        //getPostGetVar('jurisdiction_level')
                        ?>
                    </div>
                </td>
            </tr>
        </table>
        <?php echo err_field('jurisdiction', $no_br=true); ?>
    </td>
</tr>
