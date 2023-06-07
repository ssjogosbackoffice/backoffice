<?php

if(isset($_GET['banner']) && isset($_SESSION["admin_id"])){
    $file = $_GET['banner'];
    if(!defined("BANNERS_SUBDOMAIN")){
        $filelocation=(BANNERS."/".$file);
    }else{
        $filelocation=(BANNERS_SUBDOMAIN."/".$file);
    }
    if(!file_exists($filelocation)) die("I'm sorry, the file doesn't seem to exist.");
    header('Content-Type: application/octet-stream');
    header("Content-Disposition: attachment;filename=$file");
    header("Content-Transfer-Encoding: binary");
    header('Pragma: no-cache');
    header('Expires: 0');
    set_time_limit(0);
    readfile($filelocation);
}
else{
    header("Location: /?err=expired");
    die();
}
?>
