<?php
if($selector_unique_id == '' || $selector_unique_id == null) {
    $selector_unique_id = '';
}
?>
<tr valign=top id="jurisdictionSelector<?=$selector_unique_id?>">
  <td class=label><?=$lang->getLang("Jurisdiction")?></td>
  <td <?form_td_style ('jurisdiction')?>>
    <table cellpadding="0" cellspacing="0" border="0">
    <tr>
      <td>
        <select id="jurisdiction_level<?=$selector_unique_id?>" name="jurisdiction_level" onChange="change_jurisdiction(this,'jurisdiction_select<?=$selector_unique_id?>')">
             <option value="club"<?if ( 'club' == $jurisdiction_level ) echo 'selected="selected"'; ?>>Betting Club </option>
<?php if ( 'casino' == $_SESSION['jurisdiction_class'] || 'nation' == $_SESSION['jurisdiction_class'] || 'region' == $_SESSION['jurisdiction_class'] ) { ?>
        <option value="district"<?if ( 'district' == $jurisdiction_level ) echo 'selected="selected"'; ?>>District</option>
<?php } ?>
<?php if ( 'casino' == $_SESSION['jurisdiction_class'] || 'nation' == $_SESSION['jurisdiction_class'] ){ //show region option ?>
         <option value="region"<?if ( 'region' == $jurisdiction_level || ! $jurisdiction_level ) echo 'selected="selected"'; ?>>Region</option>
<?php } ?>
<?php if ( 'casino' == $_SESSION['jurisdiction_class'] ){ //show region option ?>
         <option value="nation"<?if ( 'nation' == $jurisdiction_level || ! $jurisdiction_level ) echo 'selected="selected"'; ?>>Nation</option>
<?php } ?>
         </select>
       </td>
       <td>
         <div id="jurisdiction_select<?=$selector_unique_id?>" class="content">
             <script type="text/javascript">
                 var select = document.getElementById("jurisdiction_level<?=$selector_unique_id?>");
                 change_jurisdiction(select,'jurisdiction_select<?=$selector_unique_id?>');
             </script>
         </div>
       </td>
     </tr>
     </table>
     <?php echo err_field('jurisdiction', $no_br=true); ?>
   </td>
</tr>

