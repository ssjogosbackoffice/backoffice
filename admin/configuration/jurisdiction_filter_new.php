
<div><div class='label label-inverse'><?=$lang->getLang("Jurisdiction")?></div>
    <br>
    <div class="controls">

            <select id="jurisdiction_level" name="jurisdiction_level" onChange="change_jurisdiction(this);" style="float:left">
                <option value="club"<?if ( 'club' == $jurisdiction_level ) echo 'selected="selected"'; ?>>Club </option>
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
            <div id="jurisdiction_select" >
                <?php echo get_jurisdiction_select($jurisdiction_level, $_SESSION['jurisdiction_id']);
                //getPostGetVar('jurisdiction_level')
                ?>
            </div>

    </div>
    <?php echo err_field('jurisdiction', $no_br=true); ?>
</div>

