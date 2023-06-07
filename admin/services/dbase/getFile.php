<?php

//check_access("database_slave_check", true);
 if(isset($_GET["file"]))
 {
	die("<pre>". file_get_contents($_GET["file"]) . "</pre>");
 }


?>
