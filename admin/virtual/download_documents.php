<?php
if(isset($_GET['file']) && isset($_SESSION["admin_id"])){
    $path=VIRTUAL_DOCUMENTS;
    $file = $_GET['file'];
        $filelocation=($path."/".$file);

    if(!file_exists($filelocation)) die ("The file you choose does not exist. ");
    header('Content-Type: application/pdf');
    header("Content-Disposition: attachment;filename=$file");
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