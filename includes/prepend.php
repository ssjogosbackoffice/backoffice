<?php
header("P3P: CP=\"CAO PSA OUR\"");
define('WEB_ROOT',     realpath(dirname(__FILE__).'/../'));
define('WEB_CONFIG',   WEB_ROOT . '/config');
define('WEB_INCLUDES', WEB_ROOT . '/includes');
define('LANGUAGES_PATH', WEB_ROOT . '/languages');
define('WEB_LOG',      WEB_ROOT . '/logs');
define('HOST',         $_SERVER["HTTP_HOST"]);
define('SELF',         $_SERVER["PHP_SELF"]);
define('PHOTOS',       '../cache/documents/');
define('WEB_ADMIN',      WEB_ROOT.'/admin');
include_once(WEB_INCLUDES.'/classes/PathManager.class.inc');
//ini_set("display_errors", E_ALL);
PathManager::add('.',     0);
PathManager::add('..',    1);
PathManager::add('../..', 2);
PathManager::add(WEB_INCLUDES, 3);
PathManager::add(WEB_INCLUDES.'/PEAR', 4);
PathManager::add(WEB_INCLUDES.'/templates');
PathManager::add(WEB_INCLUDES.'/i18n');
PathManager::add(WEB_INCLUDES.'/funclib');
PathManager::add(WEB_INCLUDES.'/classes');
PathManager::add(WEB_INCLUDES.'/jpgraph');
PathManager::add(WEB_INCLUDES.'/nusoap');
PathManager::add(WEB_INCLUDES.'/Smarty');
PathManager::add(WEB_INCLUDES.'/php');
PathManager::add(WEB_INCLUDES.'/libs');
PathManager::add(WEB_ROOT.'/../shared/include');
PathManager::add(WEB_ROOT.'/../shared/class');
PathManager::add(WEB_ROOT.'/../shared/funclib');
PathManager::add(WEB_CONFIG);
PathManager::set();
//var_dump(ini_get("include_path"));
include_once("classes/Object.class.inc");
include_once("classes/Exception.class.inc");
//die(ini_get("include_path"));
//include_once("");

function clean($arr) {
  return is_array($arr) ? array_map('clean', $arr) : ereg_replace('\.\./*', '', $arr);
}

$_GET     = clean($_GET);
$_POST    = clean($_POST);
$_REQUEST = clean($_REQUEST);

//determine subdir
//$subdir=strtok($_SERVER["REQUEST_URI"], "/");
//GLOBAL Server specific configuration
//if (isset($_SERVER["SITE_CONFIG"]) && file_exists($_SERVER["SITE_CONFIG"])) {
//  include_once($_SERVER["SITE_CONFIG"]);
//}

//elseif (!empty($subdir) && file_exists(WEB_CONFIG .'/'. $subdir .'.config.inc')) {
//  include_once("$subdir.config.inc");
//}
//elseif ($subdir == 'translate') {
//  include_once("admin.config.inc");
//}
if (isset($_SERVER["USE_HTTPS"]) && $_SERVER["USE_HTTPS"] == "false") {
        define('HTTP_PROTO',  'http://');
}else{
        define('HTTP_PROTO',  'https://');
}
include_once($_SERVER['CONFIG_FILE']);
?>
