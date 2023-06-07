<?php
$card = $_REQUEST["c"];
$src  = "cards/".substr($card,1,1).substr($card,0,1).".png";

$img = imagecreatefrompng($src); 
$dest = imagecreatetruecolor(20, 28);

imagealphablending($dest, false);
imagecopyresampled($dest, $img, 0, 0, 0, 0, 20, 28, 51, 73);
imagesavealpha($dest, true); 

header('Content-type: image/png');
imagepng($dest);
imagedestroy($dest);

?>
