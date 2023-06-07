<?php
check_access('ttm_totem_admin', true);
include_once("Crypt/Rc4.php");
define("CONFIGURATION_FILE", dirname(realpath(__FILE__)) . "/config/configuration.txt");

$action = $_REQUEST["action"];
switch($action){
  case "download":
    $jur_id = $_REQUEST["jur_id"];
    settype($jur_id, "integer");
    downloadConfiguration($jur_id);
    break;
  default:
    showWelcomePage();
    break;
}


function encryptConfiguration($file_content){
  $thumper_keyfile = "file://" . realpath(WEB_ROOT . "/../config/certs") . "/thumper-pub.pem";
  $config_key      = rand_str(8);
  //$config_key      = "skYNq";
  $encrypted_key   = "";
  $cryptkey        = openssl_public_encrypt($config_key, $encrypted_key, $thumper_keyfile);
  //file_put_contents("/tmp/dump.dmp", $encrypted_key);
  $cryptkey_enc    = base64_encode($encrypted_key);
  $RC4             = new Crypt_RC4($config_key);
  $RC4->crypt($file_content);
  $encoded_message = base64_encode($file_content);
  $final_message   = "key=$cryptkey_enc\r\nmsg=$encoded_message\r\n";
  //print "plain_key=$config_key\r\n";
  return $final_message;
}

function downloadConfiguration($jur_id){
  ob_clean();
  $file_content = file_get_contents(CONFIGURATION_FILE);

  header('Content-Type: text/plain');
  $file_content = str_replace("%clubid%", $jur_id, $file_content);
  $file_content = encryptConfiguration($file_content);

  $length       = strlen($file_content);
  header('Last-Modified: '  . date('r'));
  header('Content-Length: ' . $length);
  header('Content-Type: file');
  header("Content-Disposition: attachment; filename=Thumper.setup");
  header("Content-Transfer-Encoding: binary");
  echo $file_content;
  exit();

}

function showWelcomePage(){
  global $dbh,$lang;
  ?>
  <div class="bodyHD"><?=$lang->getLang("Totem configuration download page")?></div>
  <?php
  if(file_exists(CONFIGURATION_FILE)){
    $mod_time = filemtime(CONFIGURATION_FILE);
  }
  ?>
  <br/>
  <div class="bodyHD3"><?=$lang->getLang("Last update")?> <b><?=date("d M Y H:i", $mod_time)?></b></div>
  <br/>
  <? form_head(); ?>
  <input type="hidden" name="action" value="download"/>
  <select name="jur_id">
  <?php
  $jurisdiction_class = $_SESSION["jurisdiction_class"];
  $jurisdiction_id    = $_SESSION["jurisdiction_id"];

  $sql = "SELECT
            club.jur_id, club.jur_name AS jur_name
          FROM
            jurisdiction club
          JOIN
            jurisdiction district ON district.jur_id = club.jur_parent_id
          JOIN
            jurisdiction region   ON region.jur_id   = district.jur_parent_id
          JOIN
            jurisdiction nation   ON nation.jur_id   = region.jur_parent_id
          WHERE
            club.jur_class = 'club'";

  switch ($jurisdiction_class){
    case "club":
      $sql .= " AND club.jur_id = $jurisdiction_id";
      break;
    case "district":
      $sql .= " AND district.jur_id = $jurisdiction_id";
      break;
    case "region":
      $sql .= " AND region.jur_id = $jurisdiction_id";
      break;
    case "nation":
      $sql .= " AND nation.jur_id = $jurisdiction_id";
      break;
  }

  $sql .= " ORDER BY
            LOWER(club.jur_name)";
  $rs = $dbh->exec($sql);

  while ($rs->hasNext()) {
  	$jurisdiction = $rs->next();
  	printOption("jur_id", $jurisdiction["jur_id"], $jurisdiction["jur_name"]);
  }

  ?>
  </select>
  <br/>
  <input type="submit" value="<?=$lang->getLang("Download Configuration")?>"/>
  </form>
  <?php
}

?>
