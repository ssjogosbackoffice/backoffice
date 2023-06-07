<script type="text/javascript">
    function change_radio_level(select){
        var juris_class = select.options[select.selectedIndex].value;
        switch ( juris_class ){
            <?php if ( 'casino' == $_SESSION['jurisdiction_class'])  { ?>
            case 'nation' :
                var str = '<?php echo getRadioGroupsLevel('nation','nation');?>';
                document.getElementById('jur_radio_level').innerHTML = str;
                break;
            <?php } ?>
            <?php if ( 'casino' == $_SESSION['jurisdiction_class'] || 'nation' == $_SESSION['jurisdiction_class'] ||
                       'region' == $_SESSION['jurisdiction_class'] || 'district' == $_SESSION['jurisdiction_class'])  { ?>
            case 'club' :
                var str = '<?php echo getRadioGroupsLevel('club','club');?>';
                document.getElementById('jur_radio_level').innerHTML = str;
                break;
            <?php } ?>
            <?php if ( 'casino' == $_SESSION['jurisdiction_class'] ||'nation' == $_SESSION['jurisdiction_class'] || 'region' == $_SESSION['jurisdiction_class'])  { ?>
            case 'district' :
                var str = '<?php echo getRadioGroupsLevel('district','district');?>';
                document.getElementById('jur_radio_level').innerHTML = str;
                break;
            <?php } ?>
            <?php if ( 'casino' == $_SESSION['jurisdiction_class'] ||  'nation' == $_SESSION['jurisdiction_class'] )  { ?>
            case 'region' :
                var str = '<?php echo getRadioGroupsLevel('region','region');?>';
                document.getElementById('jur_radio_level').innerHTML = str;
                break;
            <?php } ?>
        }
    }
</script>

<div><div class='label label-inverse'><?=$lang->getLang("Jurisdiction")?></div>
    <br>
    <div class="controls">

            <select id="jurisdiction_level" name="jurisdiction_level" onChange="change_jurisdiction(this);change_radio_level(this);" style="float:left">
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
</div><br />



<div><div class='label label-inverse'><?=$lang->getLang("Jurisdiction Totals")?></div>
    <br>
    <div class="controls">

            <div id="jur_radio_level">
                <?php echo getRadioGroupsLevel($jurisdiction_level, $jur_groups_level) ?>
            </div>
    </div>
</div><br />

