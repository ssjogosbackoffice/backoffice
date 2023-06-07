<script type="text/javascript">
    function change_radio_level(select,id){
        id = typeof id !== 'undefined' ? id : 'jur_radio_level';
        var juris_class = select.options[select.selectedIndex].value;
        switch ( juris_class ){
            <?php if ( 'casino' == $_SESSION['jurisdiction_class'])  { ?>
            case 'nation' :
                var str = '<?php echo getRadioGroupsLevel('nation','nation');?>';
                document.getElementById(id).innerHTML = str;
                break;
            <?php } ?>
            <?php if ( 'casino' == $_SESSION['jurisdiction_class'] || 'nation' == $_SESSION['jurisdiction_class'] ||
                       'region' == $_SESSION['jurisdiction_class'] || 'district' == $_SESSION['jurisdiction_class'])  { ?>
            case 'club' :
                var str = '<?php echo getRadioGroupsLevel('club','club');?>';
                document.getElementById(id).innerHTML = str;
                break;
            <?php } ?>
            <?php if ( 'casino' == $_SESSION['jurisdiction_class'] ||'nation' == $_SESSION['jurisdiction_class'] || 'region' == $_SESSION['jurisdiction_class'])  { ?>
            case 'district' :
                var str = '<?php echo getRadioGroupsLevel('district','district');?>';
                document.getElementById(id).innerHTML = str;
                break;
            <?php } ?>
            <?php if ( 'casino' == $_SESSION['jurisdiction_class'] ||  'nation' == $_SESSION['jurisdiction_class'] )  { ?>
            case 'region' :
                var str = '<?php echo getRadioGroupsLevel('region','region');?>';
                document.getElementById(id).innerHTML = str;
                break;
            <?php } ?>
        }
    }
</script>

<tr valign=top>
    <td class=label><?=$lang->getLang("Jurisdiction")?></td>
    <td <?form_td_style ('jurisdiction')?>>
        <table cellpadding="0" cellspacing="0" border="0">
            <tr>
                <td>
                    <select id="jurisdiction_level" name="jurisdiction_level" onChange="change_jurisdiction(this);change_radio_level(this);">
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
                    <div id="jurisdiction_select" class="content">
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

<tr valign=top>
    <td class=label><?=$lang->getLang("Jurisdiction Totals")?></td>
    <td <?form_td_style ('jurisdiction')?> ><div id="jur_radio_level">
        <?php echo getRadioGroupsLevel($jurisdiction_level, $jur_groups_level) ?>
        </div>
    </td>
</tr>