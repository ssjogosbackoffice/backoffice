<?php   
  if ( 'club' == $_SESSION['jurisdiction_class'] ) {
?>
   <div id="jurisdiction_select" class="content">
     <?php echo get_jurisdiction_select('club', $jurisdiction_id);?>
   </div>
<?php
  }
  else{
?>
  <table cellpadding="0" cellspacing="0" border="0">
    <tr>
      <td>
<?php
  // if ( 'region' == $report_type || 'district' == $report_type || 'club' == $report_type ){   
       	//$disabled = 'disabled="disabled"';
  // } 
  if(isset($_POST["jurisdiction_level"])) {
    $jurisdiction_level = $_POST["jurisdiction_level"];
  }
?>

<select id="jurisdiction_level" name="jurisdiction_level" onChange="change_jurisdiction(this)" <?=$disabled?>>

<?php if ('casino' == $_SESSION['jurisdiction_class'] || 'nation' == $_SESSION['jurisdiction_class'] ) : ?>
<?php ?>//if ('casino' == $_SESSION['jurisdiction_class']) : ?>
        <option value=""> - All Jurisdictions - </option>
<?php endif; ?>

<?php if ('casino' == $_SESSION['jurisdiction_class']) : ?>
        <option value="casino"<?if ( isset($jurisdiction_level) && 'casino' == $jurisdiction_level ) echo 'selected="selected"'; ?>>Casino</option>
<?php endif; ?>

<?php if ( 'casino' == $_SESSION['jurisdiction_class'] || 'nation' == $_SESSION['jurisdiction_class']) : ?>
<?php ?>//if ( 'casino' == $_SESSION['jurisdiction_class']) : ?>
        <option value="nation"<?if ( isset($jurisdiction_level) && 'nation' == $jurisdiction_level ) echo 'selected="selected"'; ?>>National </option>
<?php endif; ?>

<?php if ('casino' == $_SESSION['jurisdiction_class'] || 'nation' == $_SESSION['jurisdiction_class'] || 'region' == $_SESSION['jurisdiction_class'] ) : ?>
<option value="region" <?php if ( isset($jurisdiction_level) && 'region' == $jurisdiction_level ) echo 'selected="selected"'; ?>>Region</option>
<?php endif; ?>

<?php if ('casino' == $_SESSION['jurisdiction_class'] || 'nation' == $_SESSION['jurisdiction_class'] || 'region' == $_SESSION['jurisdiction_class'] || 'district' == $_SESSION['jurisdiction_class']) : ?>
<option value="district" <?php if ( isset($jurisdiction_level) && 'district' == $jurisdiction_level ) echo 'selected="selected"'; ?>>District</option>
<?php endif; ?>

<?php if ( 'district' == $_SESSION['jurisdiction_class'] ) : ?>
        <option value=""> - All clubs - </option>		
<?php endif; ?>
        <option value="club"<?if ( isset($jurisdiction_level) && 'club' == $jurisdiction_level ) echo 'selected="selected"'; ?>>Club</option>
  </select>
      </td>
	  <td>
	    <div id="jurisdiction_select" class="content">
          <?= get_jurisdiction_select(getPostGetVar('jurisdiction_level'), $jurisdiction_id);?>
        </div>
      </td>
    </tr>
  </table>
<?php
  }
?>
