<?php
include('ip2locationlite.class.php');
 
//Load the class
$ipLite = new ip2location_lite;
$ipLite->setKey('82d986bcdaebc971dcb4fc03f58b766c2c33e0b0e231975d9385af41b1d21b0d');
//Get errors and locations
$locations = $ipLite->getCity($ip);
$errors = $ipLite->getError();
 
//Getting the result
?>
<center><div><table><tr><td style="vertical-align: top"><table><tr><td class="formheading" colspan="2">Details</td></tr>

<? if (!empty($locations) && is_array($locations)) {
  foreach ($locations as $field => $val) {
  ?>
  <tr><td class="label"><?=$field?><td class="content"><?=$val?></td></tr>
    
    <? if($field=='latitude')
    {
    	$latitude=$val;
    }
    if($field=="longitude")
    {
    	$longitude=$val;
    }
  }
}
?>
</table></td><td>
<iframe width="425" height="350" frameborder="0" scrolling="no" marginheight="0" marginwidth="0" src="https://maps.google.ro/?q=<?=$latitude?>,<?=$longitude?>&amp;ie=UTF8&amp;t=m&amp;z=14&amp;output=embed"></iframe><br /><small><a href="https://maps.google.ro/?q=<?=$latitude?>,<?=$longitude?>&amp;ie=UTF8&amp;t=m&amp;z=14&amp;source=embed" style="color:#0000FF;text-align:left" target="_blank">View Larger Map</a></small></td></tr></table></div></center>
