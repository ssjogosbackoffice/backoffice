<?php

function dtderrors($errno, $errstr, $errfile, $errline, $errcontext, $ret = false)
{
   static $errs = array(); 
   if ($ret === true) {
       return $errs; 
   }
   $tag = 'DOMDocument::validate(): '; 
   $errs[] = str_replace($tag, '', $errstr); 
}


function parseGame($gameId) {
  $result = array("errno" => 0, "errdesc" => "Success");
  try {
    $gameserver_conf = "/srv/gameserver/config";

    $dom = new DomDocument('1.0', 'UTF-8');
    $dom->preserveWhiteSpace = false;
    if ($dom->load($gameserver_conf."/gameserver.xml")) {
      set_error_handler('dtderrors');
      $old = ini_set('html_errors', false);
      $dom->validate();
      ini_set('html_errors', $old);
      restore_error_handler(); 
      $errs = dtderrors(null, null, null, null, null, true);
      //print_r($errs);

      $xdom = new DOMXPath($dom);
      $xdom->registerNamespace("gameserver", "urn:schemas-dollarobet-com:gameserver");
      $gameNode = $xdom->query("/gameserver:configuration/game[@gameId=$gameId]")->item(0);

      if ($gameNode ) {
        if ($gameNode->hasAttributes()) {
          $result["name"]  = $gameNode->attributes->getNamedItem("name")->textContent;
          $result["class"] = $gameNode->attributes->getNamedItem("class")->textContent;
        } else {
          return -2;
        }
        if ($gameNode->hasChildNodes()) {
          $params = $gameNode->getElementsByTagName("param");
          foreach ($params as $param) {
             $paramName = trim($param->attributes->getNamedItem("name")->textContent);
             $paramValue = trim($param->attributes->getNamedItem("value")->textContent);
             if (!empty($paramName)) {
               if ($paramName == "configFile") {
                 $confData   = file_get_contents($gameserver_conf . "/" . $paramValue);
                 $properties = explode("\n", $confData);
                 $prop_array  = array();
                 foreach ($properties as $property) {
                   list($name, $val) = split('=', $property);
                   $name  = trim($name);
                   $val   = trim($val);
                   if (!empty($name) && !empty($val)) {
                     $prop_array[$name] = $val;
                   }
                 }
                 $result["configFile"]["name"]     = $paramValue;
                 $result["configFile"]["contents"] = $prop_array;
               } else {
                 $result[$paramName] = $paramValue;
               }
            }
          }
        }
      } else {
        $result["errno"] = 2;
        $result["errdesc"] = "Cannot find game with id ".$gameId;
      }
    } else {
      $result["errno"] = 1;
      $result["errdesc"] = "Cannot load xml file";
    }
  } catch (Exception $exception) {
    $result["errno"]   = -1;
    $result["errdesc"] = "Unhandled Exception";
  }
  return $result;
}

?>
