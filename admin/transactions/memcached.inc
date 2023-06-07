<?php
include_once("queue_func.php");

$token = "TDPL-123123S1";

if(queue_begin($token, 10)){
  echo $lang->getLang('The transaction can go')."<br/>";
}else{
  echo $lang->getLang("Another transaction is already in progress")."<br/>";
}

?>