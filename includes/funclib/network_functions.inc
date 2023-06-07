<?
function getIpDetails($ip_address){
  global $dbh;

  $ip_to_long = sprintf("%u", ip2long($ip_address));

  $sql  = "SELECT ipt_id, ipt_from, ipt_to, ipt_country_code2, ipt_country_code3, ipt_country_name";
  $sql .= " FROM ip_to_country";
  $sql .= " WHERE ipt_from <= $ip_to_long AND ipt_to >= $ip_to_long";

  $rs  = $dbh->exec($sql);

  if ( 1 == $num_rows  )
  {  $row = $rs->next();
  $id = $row['ipt_id'];
  $from = $row['ipt_from'];
  $to = $row['ipt_to'];
  $country_code2 = $row['ipt_country_code2'];
  $country_code3 = $row['ipt_country_code3'];
  $country_name = $row['ipt_country_name'];

  $arr = array('id'=>$id, 'from'=>$from, 'to'=>$to,
  'country_code2'=>$country_code2,
  'country_code2'=>$country_code3,
  'country_name'=>$country_name);


  return $arr;
  }
  return array();
}


function ipToCountryName($ip_address){
  $ip_details = getIpDetails($ip_address);
  return $ip_details['country_name'];
}

function isValidIp($ip){
  if ( preg_match("/[^0-9\.]/", $ip) )
  {
    return false;

  }

  $arr = explode('.', $ip);

  if ( count($arr) != 4 )
  return false;

  return true;
}

function sendSms($phone_number, $message){
  $regexp = "/^(0039)?3(2(0|3|7|8|9)|3(0|3|4|5|6|7|8|9)|4(0|3|6|7|8|9)|6(0|3|6|8)|77(3|7)|8(0|3|8|9)|9(0|1|2|3))[0-9]{5,7}$/";
  
	$phone_number = trim($phone_number);
  $phone_number = ereg_replace("[^0-9\+]+", "", $phone_number);
  $phone_number = str_replace("+", "00");

  if(!preg_match($regexp, $phone_number)){
    return false;
  }
  
  if(!preg_match("/^\+39/", $phone_number)){
  	$phone_number = "0039" . $phone_number;
  }
  
  die("Invio a $phone_number:<br/>$message");
  
  return true;
}

/*
function sendSms($numbers, $text)
{  include 'bigfoot.cfg';

//defining post arguments
$vars   = array ("username"=>$username, "password"=>$password, "test_mode"=>$test_mode, "type"=>$type,
"text"=>$text, "numbers"=>$numbers, "originator"=>$originator, "version"=>$version);

if(($response = http_post($server, $port, $url, $vars)) == false)
return false;

$response_msg = getResponseBody($response);

return wasSent($response_msg);
}
*/

function http_post($server, $port, $url, $vars)
{
  $user_agent = "Mozilla/4.0 (compatible; MSIE 5.5; Windows 98)";
  $urlencoded = "";

  while (list($key,$value) = each($vars))
  $urlencoded.= urlencode($key) . "=" . urlencode($value) . "&";
  $urlencoded = substr($urlencoded,0,-1);

  $content_length = strlen($urlencoded);

  $headers  = "POST /$url HTTP/1.1\n";
  $headers .= "Accept: */*\n";
  $headers .= "Accept-Language: en-au\n";
  $headers .= "Content-Type: application/x-www-form-urlencoded\n";
  $headers .= "User-Agent: $user_agent\n";
  $headers .= "Host: $server\n";
  $headers .= "Connection: Keep-Alive\n";
  $headers .= "Cache-Control: no-cache\n";
  $headers .= "Content-Length: $content_length\n";

  $fp = fsockopen($server, $port, $errno, $errstr);
  // $fp = fsockopen($server, $port, $errno, $errstr) or die("$errno, $errstr");

  if (!$fp)
  return false;

  fputs($fp, "$headers\n$urlencoded");

  $ret = "";
  while (!feof($fp))
  $ret.= stripslashes(fgets($fp, 1024));

  fclose($fp);

  return $ret;
}




function getResponseBody($response)
{  $response_arr = explode("\n",$response);
$len = count($response_arr);
return $response_arr[$len-2];
}

function wasSent($response_message)
{  $message_array = explode(",",$response_message);
$len = count($message_array);

$status = $message_array[1];
if ($status == 0)
return true;
else
return false;
}


function https_post($url, $vars, $timeout=60, $vfy_host=false, $vfy_peer=false, $username="", $password="", $debug=false)
{
  $user_agent = "Mozilla/4.0 (compatible; MSIE 5.5; Windows 98)";
  $urlencoded = "";
  if (is_array($vars)) {
    foreach ($vars as $key => $value)
    $urlencoded.= urlencode($key) . "=" . urlencode($value) . "&";
    $urlencoded = substr($urlencoded,0,-1);
  } else {
    $urlencoded = $vars;
  }
  $ch = curl_init($url);
  curl_setopt($ch, CURLOPT_USERAGENT, $user_agent); // pretend to be a mozilla browser
  curl_setopt($ch, CURLOPT_POST, 1);   // HTTP POST
  curl_setopt($ch, CURLOPT_POSTFIELDS, $urlencoded); // set postfield as attribute query str

  curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, 0);

  if ( $username && $password )
  {
    curl_setopt($ch, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_setopt($ch, CURLOPT_USERPWD, $username.':'.$password);
  }




  // ssl host certificate verification?

  if ( $vfy_host )
  {
    curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 1);  //make sure the certificate is correct
  }
  else
  {
    curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 0);
  }

  // ssl client certificate verification?

  if ( $vfy_peer )
  {
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, 1); //no client-side certificate verification at this time
  }
  else
  {
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, 0);
  }

  curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1); // make sure curl_exec RETURNS,  not prints the response
  curl_setopt($ch, CURLOPT_TIMEOUT, $timeout);

  // send the request, store response
  $response = curl_exec($ch);

  // store error number (will be 0 if no error )
  $curl_errno = curl_errno($ch);
  $http_code = curl_getinfo($ch, CURLINFO_HTTP_CODE);
  curl_close($ch);

  // return false on CURL error

  if ( 0 != $curl_errno  )
  {
    if ( $debug )
    echo "CURL ERROR:" . $curl_errno . '< br/>';

    return -1;
  }
  elseif ( 200 != $http_code )
  {
    if ( $debug )
    echo "HTTP CODE:" . $http_code . '< br/>';
    return -2;
  }

  return $response;
}
?>
