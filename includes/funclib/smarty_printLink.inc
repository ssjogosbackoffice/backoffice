<?php

function tpl_printLink($params) {                                                                                                   
  printLink($params["ref"], $params["text"]);                                                                                       
}                                                                                                                                   

$template->register_function("printLink", "tpl_printLink"); 

?>
