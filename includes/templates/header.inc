<?php
if (isset($_SESSION["pda"]) && $_SESSION["pda"] == true) {
  include('pda/header.inc');
} else {
  if(DOT_IT == true){
    include('web/header_it.inc'); 
  }else{
    include('web/header_com.inc'); 
  }
}
?>
