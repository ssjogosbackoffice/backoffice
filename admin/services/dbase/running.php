<?php
$ini_array = parse_ini_file('config.ini',true);
$command = $ini_array['default']['command'];
$path = $ini_array['default']['path'];
$ext = $ini_array['default']['ext'];
$settings = $ini_array['database']['settings'];


foreach($settings as $k => $v){
    $data = explode("$",$v);
    $tempCommand = $command;
    for($i=0; $i<count($data) ;$i++){
        $tempCommand = preg_replace('/\?/',$data[$i], $tempCommand, 1);
    }
    $output=shell_exec($tempCommand);
    $filename=$path.$data[count($data)-1].$ext;
    $name=$data[count($data)-1].$ext;

    $content = file_put_contents($filename,$output);
}

?>




