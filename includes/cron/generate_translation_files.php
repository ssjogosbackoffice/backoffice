<?php
include_once("../classes/DatabaseHandler.class.inc");
define('LANGUAGES_PATH', "../../languages");
define('FILENAME', LANGUAGES_PATH . "/lang_tpl.inc");
//$dbh = new DatabaseHandler("mysql://developer:TEAMDEVELOPER@10.15.0.12/translator");

include_once '../php/Logger.php';
Logger::configure(WEB_CONFIG . "/log4php.xml");

$dbLog = Logger::getLogger("DbLogger");

$dbh = new DatabaseHandler("mysql://einstain:0r0l0g1a10@10.15.0.4/translator");
$dbh->connect();
$sql = "desc words";
$result = $dbh->exec($sql);
$languages_available = array();
while($result->hasNext()){
    $row = $result->next();
    if($row['field'] == 'wds_id') continue;
    list($prefix, $language) = explode("_", $row['field']);
    array_push($languages_available, $language);
}
$fileContent = file(FILENAME);
$transaltions = array();
foreach($fileContent as $line){
    if(preg_match("<<content id='(.*)' />>", $line, $matches, PREG_OFFSET_CAPTURE)){
        $wordID = $matches[1][0];
        $sql = "Select * from words where wds_id = $wordID";
        $translation = $dbh->queryRow($sql);
        $transaltions[$wordID] =  $translation;
    }
}
foreach($languages_available as $lang){
    $currentContent = array();
    unlink(LANGUAGES_PATH . "/lang_".$lang.".inc");
    foreach($fileContent as $line){
        preg_match("<<content id='(.*)' />>", $line, $matches, PREG_OFFSET_CAPTURE);
        $wordID = $matches[1][0];
        $line = preg_replace("<<content id='(.*)' />>", $transaltions[$wordID]["wds_".$lang], $line);
        file_put_contents(LANGUAGES_PATH . "/lang_".$lang.".inc", strtolower($line), FILE_APPEND);
    }
    echo "File for language ". $lang . " successfully created  </br>";
}
?>
