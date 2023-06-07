<?php
  
  if(isset($_GET['id']) && $_SESSION["admin_id"] == $_GET["id"]){
    $fullPath = KEYS_DIR."/privateKey_".$_GET['id'].".pem";
    header("Pragma: public"); // required
    header("Expires: 0");
    header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
    header("Cache-Control: private",false); // required for certain browsers
    header("Content-Type: application/force-download");
    header("Content-Disposition: attachment; filename=backoffice.pem;");
    ob_clean();
    flush();
    readfile($fullPath);
  }else{
    echo "<html><head><title>".$lang->getLang("Forbidden")."</title><head><body><h3>".$lang->getLang("Unauthorized access prohibited");
  }
?>
