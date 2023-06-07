<?php
if($_SESSION["jurisdiction_class"] != 'club'){
  check_access("noenter", true);
}

define("WEBSITE_ROOT", realpath(WEB_ROOT . "/../"));
define("SKINS_PATH",   realpath(WEBSITE_ROOT . "/skins/"));

$skin = getSkinDetailsByJurisdiction($_SESSION["jurisdiction_id"], $_SESSION["jurisdiction_class"]);

$skinDirName = SKINS_PATH . "/" . $skin["skn_foldername"] . "/banners";
$webDir      = $skin["skn_url"] . "/skins/" . $skin["skn_foldername"] . "/banners";


$sql = "SELECT jur_code, jur_name FROM jurisdiction WHERE jur_id = " . $_SESSION["jurisdiction_id"];

$row  = $dbh->queryRow($sql);
$code = strtolower($row["jur_code"]);
$name = $row["jur_name"];

//$code        =

if(file_exists($skinDirName)){
  ?>
  <h1>Banner di <?=$skin["skn_name"]?></h1>
  <div>I clienti che arriveranno al sito usando questo banner<br/>diverranno clienti del tuo club <b><?=$name?></b></div>
  <br/>
  <br/>
  Se vuoi creare un banner in autonomia, ricordati di linkarlo a:<br/>
  <textarea  cols="70" rows="2" readonly="readonly"><?=$skin["skn_url"] . "/www/register.php?ref=$code"?></textarea><br/>
  <?php
  $skinDir = opendir($skinDirName);
  while($file = readdir($skinDir)){
    if(!ereg("^[:\.]", $file)){
      ?>
      <h2>Banner <?=$file?></h2>
      <img src="<?=$webDir . "/" . $file?>"/><br/><br/>
      <div class="bodyHD3">Per aggiungere il banner <?=$file?> al proprio sito web<br/>copiare ed incollare il seguente codice all'interno della pagina</div><br/>
      <textarea cols="70" rows="4" readonly="readonly"><a href="<?=$skin["skn_url"] . "/www/register.php?ref=$code"?>"><?="\n  "?><img src="<?=$webDir . "/" . $file?>"/><?="\n"?></a></textarea><br/><br/><br/><br/>
      <?php
    }
  }
}else{
  printInfoMessage("No banners for " . $skin["skn_name"]);
}
?>