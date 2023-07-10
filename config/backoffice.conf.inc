<?php
require_once '../includes/prepend.php';
require_once 'database.conf.inc';

if (empty($_SERVER['CASINO_NAME'])) {
    // die("Please configure CASINO_NAME in VirtualHost");
    $_SERVER['CASINO_NAME'] = "Backoffice";
}
$casino_name = $_SERVER['CASINO_NAME'];
define('CASINO_NAME',      $casino_name);
define('CASINO_FULL_NAME', 'Casino');
define('EMAIL_SUPPORT',    'info@test.com');
define('MENU_FILE',    'menu.ini');
#  preg_match('/[^.]+\.[^.]+$/', HOST, $m);
//  preg_match('/([^.]+\.[^.:]+)[:0-9]*$/', HOST, $m);
//  define(DOMAIN, $m[1]);
//  unset($m);

define('DASHBOARD_SHOW_IP', true);
define('DOT_IT', false);
define('CUSTOMERS_UPLOAD_PATH',      realpath(WEB_ROOT . "/uploads"));
define('CUSTOMERS_DOCS_PATH',        CUSTOMERS_UPLOAD_PATH . "/documents");
define('CUSTOMERS_UPLOAD_DOCS_DIR',  "/uploads/documents");
define('MEMCACHED_SERVERS', realpath(WEB_ROOT . "/../config/memcached.servers"));
define('TRANSACTION_DATE',"2014-07-04"); 

define('BANNERS',WEB_ROOT.'/admin/media/banners');
define('BANNERS_WIDTH',700);
define('BANNERS_HEIGHT',300);
define('BANNERS_WIDTH_BETTING',669);
define('BANNERS_HEIGHT_BETTING',250); 
define('temp_dir', WEB_ROOT.'/admin/temp');
define("CACHING_DRIVER", "FileSystem");

//Enable or disable the captcha check from here. Can have only TRUE or FALSE VALUES
define("ADD_CAPTCHA_CHECK", false);

define("SQL_CACHE_DIR", WEB_ROOT . "/cache/sqlcache");
//aggiunte per unificazione finanziario .it e .com
define('RAKE_TAX_RATE',       0);

//granbet
define('BETTING_PROVIDER_ID', 7);
define('BETTING_TAX_RATE',    0);

//gratta e vinci
define('SCRATCH_PROVIDER_ID', 6);
define('SCRATCH_TAX_RATE',    0);

//royalpk
define('CASINO1_PROVIDER_ID', 8);
define('CASINO1_TAX_RATE',    0);

//medialive
define('CASINO2_PROVIDER_ID', 4);
define('CASINO2_TAX_RATE',    0);

define('MENU_MESSAGES','1');
define('MENU_HELP','1');

if (!defined('KEYS_DIR')) {
   define("KEYS_DIR", WEB_ROOT.'/admin/keys');
}

define("SITE_LOG", WEB_ROOT . "/logs/");
define("SKINID", "58");
define("RTYPE",  "0");
define('WEBAPP_KEY', "1bc44594");
define('ONLINEPLRS','http://testsrv.egaming.net/services_new/cachemonitor.jsp?type=1');

define('WEB_APP_TOTEM' , "http://testsrv.egaming.net/totemmanagement/TotemServiceManager");
define('WEB_APP_PATH', "http://testsrv.egaming.net/services/InternalService");
define('WEB_APP_WEBSITES', "http://testsrv.egaming.net/services/WebSiteServiceManager");

define('BETTING_URL',"https://backend.egaming.net");
define('BETTING_WCODE','378605');
define('BETTING_HISTORY_URL', 'https://sports-itainment.uat.biahosted.com/generic/betlist.aspx');
define('BETTING_COMMISSIONS_SERVER_IP', '82.20');

//$_SERVER["ADMIN_DSN"] = "jconnpool://127.0.0.1:9087";

//fine aggiunte
if (defined('DBCONF')) {
    $url = parse_url(DBCONF);
    if ($url["path"]{0} == '/')
        $url["path"] = substr($url["path"], 1);
    if ($url["scheme"] == "mysql") {
        define('DSN', DBCONF);
    }
}

$gameserver_host = $_SERVER['HTTP_HOST'];
$console_port    = 3032;

if (isset($_SERVER['GAMESERVER_HOST'])) {
    $gameserver_host = $_SERVER['GAMESERVER_HOST'];
}
if (isset($_SERVER['CONSOLE_PORT'])) {
    $console_port    = intval($_SERVER['CONSOLE_PORT']);
}
$casino_console  =  "$gameserver_host:$console_port";

if (isset($_SERVER['CURRENCY_SYMBOL'])) {
    define('CURRENCY_SYMBOL', $_SERVER['CURRENCY_SYMBOL']);
}
else {
    define('CURRENCY_SYMBOL','&euro;&nbsp;');
}

define("ADMIN_SITE", 58);

define('DYNAMIC_RECHARGE',  false);
//  define("SEARCH_REALTIME", false);
define('MAX_DEPOSIT_LIMIT',  true);
define('MAX_DEPOSIT_AMOUNT', 500);

define('log_dir',        WEB_LOG);
define('cache_file_dir', WEB_ROOT.'/cache');
define('temp_download_url','/temp');

// DEFAULT is https://, i changed because i was having ssl errors
define('nonsecure_host',    "http://".HOST);
define('secure_host',     nonsecure_host."/backoffice/admin");

define ( 'SECURITY_PID', 'SECURITY_PID_ADMIN' );
define("MEMSERVERCACHE", '127.0.0.1');
define("MEMPORTCACHE", '11211');
//DEFAULT: define ( "MEMSERVERCACHE", "10.10.30.13");
//DEFAULT:  define ( "MEMPORTCACHE", 11211);

define ( "REDISSERVER", "10.10.30.13");
define ( "REDISPORT", 6379);
/*if (isset($_SERVER["USE_HTTPS"]) && $_SERVER["USE_HTTPS"] == "false") {
  define('secure_host',     nonsecure_host);
}
else {
  if (isset($_SERVER['HTTPS_PORT'])) {
    define('secure_host',   "https://".HOST.":".$_SERVER["HTTPS_PORT"]);
  }
  else {
    define('secure_host',   "https://".HOST);
  }
}*/
define('common_image_dir',  "/media/images");
// DEFAULT: define('image_dir',         "/media/images");
define('image_dir',         "/backoffice/admin/media/images");

$site_avail_langs = array('English'  => 'EN',   // Italiano
    'Italiano' => 'IT',   // English
    'Romanian' => 'RO',
    'Turkish' => 'TR',
    'Spanish' => 'ES',
    'Portuguese' => 'PT',
    'French'   => 'FR',
    'Dutch' => 'NL',
    'German'   => 'DE');
//                            'Portuguese' => 'PT', // Portuguese
//                            'Spanish'  => 'ES',   // Spanish
//                            'German'   => 'DE',   // Spanish
//                            'Romanian' => 'RO',   // Romanian
//                            'Bulgarian' => 'BG',
//                            'Swedish' => 'SE',
//                            'Norvegian' => 'NO',
//                            'Chinese' => 'CN',
//                            'French'   => 'FR',   // French
//                            'Russian'  => 'RU',   // Russian
//                            'Greek'    => 'GR');


#require_once('DatabaseHandler.class.inc');

require_once('../includes/classes/DatabaseWrapper.class.inc');

include_once("Cookie.class.inc");

//include_once("funclib/cookie_handling_adm.inc");
include_once("funclib/general_functions.inc");
include_once("funclib/date_functions_adm.inc");
include_once("funclib/markup_functions.inc");
include_once("funclib/network_functions.inc");
include_once("funclib/financial_functions.inc");
include "Db.php";
require_once '../includes/classes/PathManager.class.inc';
require_once '../includes/classes/Cookie.class.inc';
require_once '../includes/php/Logger.php';

define('WEB_I18N',     realpath(WEB_ROOT.'/../localization/'));
// DEFAULT: define('DOMAIN', ".egaming.net");
define('DOMAIN', "localhost");
define('MMEM_LIFETIME', 7200);
$mmemLifetime = 7200;


PathManager::add(WEB_I18N);
PathManager::set();
ini_set('session.cookie_domain',DOMAIN);


$sess_id_cookie = new Cookie ( 'bsid' );
Logger::configure(WEB_CONFIG . "/log4php.xml");

$dbLog = Logger::getLogger("DbLogger");
if(isset($_SESSION['jurisdiction_class'])) {
    if($_SESSION['jurisdiction_class'] == 'club') {
        $mmemLifetime = 36000;
    }
}
if (is_null ( $sess_id_cookie->getCookieValue () )) {
    if (session_status() === PHP_SESSION_ACTIVE) {
        session_destroy();
    }
    $bsid = generateSessionId ();
    $sess_id_cookie->setCookie ( $bsid, time () + $mmemLifetime, '/', DOMAIN );
} else {
    $sess_id_cookie->updateCookie ( time () + $mmemLifetime, '/', DOMAIN );
}

session_name("asid");
session_id ( $sess_id_cookie->getCookieValue () );
session_start ();
$bsid = $sess_id_cookie->getCookieValue();
error_log("Will be created user session: bsid: " . $bsid . ' ---  SessCookie: ' . $sess_id_cookie->getCookieValue ());



# New database for Slave managing 2014-04-23
#$dbh = new DatabaseHandler();
#$dbh->connect();

$dbh = new DatabaseWrapper();
if(isset($_SERVER['THIRD_PART_BACKOFFICE']) && $_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
    list($user_betting, $pass_betting, $host_betting, $db_betting) = explode(":",$_SERVER["BETTING_DSN"]);
    Db::setConnectionInfo($db_betting,$user_betting,$pass_betting,"mysql",$host_betting);
    include_once("funclib/betting_functions.inc");
}

if(isset($_SESSION['jurisdiction_class'])) {
    if($_SESSION['jurisdiction_class'] == 'club') {
        $mmemLifetime = 36000;
    }
}

if(!isset($_SESSION['memcached_value']) && count($_SESSION) > 0)
{
    include_once('MMemcache.class.inc');
    $memcached = new MMemcache(SECURITY_PID,MEMSERVERCACHE,MEMPORTCACHE);
} else {
    $memcached = false;
}

if(isset($_SESSION['admin_id'])){
    if(!checkSingleUserLogged($_SESSION['admin_id']. "_" . SECURITY_PID . "_" . DOMAIN, $sess_id_cookie->getCookieValue(), $memcached)){
        addError("","Someone Logged with your account");
        $bsid = generateSessionId ();
        $sess_id_cookie->setCookie ( $bsid, time () + $mmemLifetime, '/', DOMAIN );
        session_id ( $sess_id_cookie->getCookieValue () );
        session_start ();
    }
}


if (isset($_GET['set_language']) && in_array($_GET['set_language'],$site_avail_langs) ) {
    $_SESSION['language'] = $_GET['set_language'];
}
require_once('Language.class.inc');
$lang = new Language( $site_avail_langs);

if (isset($_SESSION['must_change_pwd']) && $_SESSION['must_change_pwd'] == true && $_REQUEST['page'] != 'change-password'){
    header("Location: /?page=change-password&f=1");
}


?>

