<?php
globalise('form_submitted');
globalise('action');
globalise('doc');

if($action == "load"){
  if(empty($_FILES[$doc]["name"])){
    addError("", "File not received");
  }
  if(!areErrors()){
    $file_list = glob(CUSTOMERS_DOCS_PATH . "/" . getUploadedDocFileName($customer_id, $doc) . "*");
    foreach ($file_list as $file_to_remove){
      unlink($file_to_remove);
    }

    if(eregi("image", $_FILES[$doc]["type"]) && filesize($doc_file_ref) < 1024 * 1024){
      saveDocumentImage($customer_id, $_FILES[$doc]["tmp_name"], $doc);
    }else{
      eregi(".*(\.[a-zA-Z0-9]{3,4})", $_FILES[$doc]["name"], $match);
      move_uploaded_file($_FILES[$doc]["tmp_name"], CUSTOMERS_DOCS_PATH . "/" . getUploadedDocFileName($customer_id, $doc . $match[1]));
    }
  }
}
?>