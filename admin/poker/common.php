<?php


function language()
{
    # function returns longhand form of the language. 
    # function expects longhand form of the language in the 
    # $_GET variable to switch languages (?language=english) 

    // Set variables 
    $default_language = 'english';
    $supported_languages = array(
        'en' => 'english',
        'es' => 'spanish',
        'fr' => 'french',
        'it' => 'italian',
        'de' => 'german',
        'nl' => 'dutch'
    );

    // Client is setting language with query string '?language=english             
    if((!empty($_GET['language'])) and (in_array($_GET['language'], $supported_languages)))
    {
        setcookie ('language', $_GET['language'], time()+100000000, '/', '', 0);
        return $_GET['language'];
    }

    // Check for preset language in $_COOKIE['language'] 
    if((!empty($_COOKIE['language'])) and (in_array($_COOKIE['language'], $supported_languages)))
    {
        return $_COOKIE['language'];
    }

    // If none of the above the script finds the first 
    // matching browser language or returns the language 
    // in the $default_language variable on no match 
    if(empty($_SERVER['HTTP_ACCEPT_LANGUAGE']))
    {
        return $default_language;
    }

    $bar = ''; # to use in regex pattern
    if (!isset($pattern)) $pattern="";
    foreach($supported_languages as $k => $v){
        $pattern .= $bar.$k;
        $bar = '|';
    }

    if(preg_match('/'.$pattern.'/', $_SERVER['HTTP_ACCEPT_LANGUAGE'], $matches))
    {
        return $supported_languages[$matches['0']];
    }
    return $default_language;
}


function GetClientIp()
{
    if (getenv("HTTP_CLIENT_IP") && strcasecmp(getenv("HTTP_CLIENT_IP"), "unknown"))
        $ip = getenv("HTTP_CLIENT_IP");
    else if (getenv("HTTP_X_FORWARDED_FOR") && strcasecmp(getenv("HTTP_X_FORWARDED_FOR"), "unknown"))
        $ip = getenv("HTTP_X_FORWARDED_FOR");
    else if (getenv("REMOTE_ADDR") && strcasecmp(getenv("REMOTE_ADDR"), "unknown"))
        $ip = getenv("REMOTE_ADDR");
    else if (isset($_SERVER['REMOTE_ADDR']) && $_SERVER['REMOTE_ADDR'] && strcasecmp($_SERVER['REMOTE_ADDR'], "unknown"))
        $ip = $_SERVER['REMOTE_ADDR'];
    else
        $ip = "0.0.0.0";

    return $ip;//gethostbyaddr($ip);
}


function curPageName()
{
    return substr($_SERVER["SCRIPT_NAME"],strrpos($_SERVER["SCRIPT_NAME"],"/")+1);
}


function FormatMoney($cur, $amount)
{
    if ( $cur == "?" || $cur == chr(128) )
        return "&euro; ".number_format($amount/100, 2, ",", ".");
    else if ( $cur > "" )
        return $cur." ".number_format($amount/100, 2, ",", ".");

    return $amount;
}

function ShowCards($cards) {
    $htmlText = "";
    $count = strlen($cards) / 3;

    for ($i=0;$i<$count;$i++)
    {
        $htmlText .= "<img src='/poker/cardimg.php?c=".substr($cards, $i*3, 2)."' border='0' width='20' height='28'>";
    }
    return $htmlText;
}

function BlockUserLevel($level) {

    if(strstr($level, ",")) {
        $lvs = explode(",", $level);
    } else {
        $lvs = array($level);
    }

    foreach($lvs as $str) {
        if(!isset($_SESSION["muser_level"]) || $_SESSION["muser_level"] == (int)$str) {
            MessageBox("This account doesn't have rights to access this page!", "warning");
            exit;
        }
    }
}

function LoggedIn() {
    if(!isset($_SESSION["muser_id"]) || $_SESSION["muser_id"] == "") {
        MessageBox("You are not loggedin!", "warning");
        exit;
    }
}

function AllowedToChangeUserField($level, $man_id, $reg_id, $dis_id) {
    $allowed = FALSE;

    if ( $level <= $_SESSION["muser_level"] )
        return $allowed;
    else if ( $_SESSION["muser_level"] < 10 )
        $allowed = TRUE;
    else if ( $_SESSION["muser_level"] == 11 && $_SESSION["muser_manager_id"] == $man_id )
        $allowed = TRUE;
    else if ( $_SESSION["muser_level"] == 12 && $_SESSION["muser_region_id"] == $reg_id )
        $allowed = TRUE;
    else if ( $_SESSION["muser_level"] == 13 && $_SESSION["muser_district_id"] == $dis_id )
        $allowed = TRUE;

    return $allowed;
}

function MakeDateDB($value)
{
    $datearr = explode("-",$value);

    return $datearr[2]."-".$datearr[1]."-".$datearr[0];
}

function file_extension($filename)
{
    return end(explode(".", $filename));
}

function AddTextField($value, $first="")
{
    if ( $first>"")
        return '"'.$value.'"';
    else
        return ',"'.$value.'"';
}

function AddNumField($value, $first="")
{
    if ($value=="")
        $value="0";

    if ( $first>"")
        return $value;
    else
        return ",".$value;
}

function WriteErrorLog($value)
{
    $fp = fopen("./log/error.log","a");
    if ( $fp )
    {
        $value = $value."\n";
        fputs($fp, $value);
    }
    fclose($fp);
}

function WriteInfoLog($value)
{
    $fp = fopen("./log/error.log","a");
    if ( $fp )
    {
        $value = $value."\n";
        fputs($fp, $value);
    }
    fclose($fp);
}

function ConvertIp($value)
{
    $value = $value+0;
    $ip4 = $value & 0xFF;
    $ip3 = ($value>>8) & 0xFF;
    $ip2 = ($value>>16) & 0xFF;
    $ip1 = ($value>>24) & 0xFF;

    return $ip1.".".$ip2.".".$ip3.".".$ip4;
}

function convertIpToString($ip) {
    $long = 4294967295 - ($ip - 1);
    return long2ip(-$long);
}

function convertIpToLong($ip) {
    return sprintf("%u", ip2long($ip));
}

?>