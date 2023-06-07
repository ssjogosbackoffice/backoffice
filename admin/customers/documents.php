<?php
globalise("uid");
globalise("type");
globalise("ext");
globalise("cnb");
globalise("first_name");
globalise("last_name");
settype($uid, "integer");

//$customer_row = getPunter($uid);

//var_dump($customer_row);

$filename = "noimage.jpg";

if($uid > 0){
  $filename = getUploadedDocFileName($uid, $type, isset($_REQUEST["thumb"]));
}


$filepath = CUSTOMERS_DOCS_PATH . "/" . $filename . $ext;
$file     = file_get_contents($filepath);
ob_start();
$length = strlen($file);

//$beauty_name = "";
$beauty_name = $cnb . "_" . $first_name . "_" . $last_name . "_" . $type . $ext;

header('Last-Modified: ' . date('r'));
header('Accept-Ranges: bytes');
header('Content-Length: ' . $length);
if($ext == ".jpg"){
  header('Content-Type: image/jpeg');
}elseif ($ext == ".pdf"){
  header('Content-Type: Content-Type: application/pdf');
  header("Content-Disposition: attachment; filename=$beauty_name");
  header("Content-Transfer-Encoding: binary");
}elseif ($ext == ".doc" || $ext == ".docx"){
  header('Content-Type: application/msword');
  header("Content-Disposition: attachment; filename=$beauty_name");
  header("Content-Transfer-Encoding: binary");
}else{
  header('Content-Type: file');
  header("Content-Disposition: attachment; filename=$beauty_name");
  header("Content-Transfer-Encoding: binary");
}


print($file);
ob_end_flush();
?>