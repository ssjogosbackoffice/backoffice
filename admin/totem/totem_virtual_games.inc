<?php 
$checkvirtual = check_access('ttm_virtual_games' , $redirect = true);
include_once 'JurisdictionsList.class.inc';
$jurClass = JurisdictionsList::getInstance ( $dbh );
$jur_class = $_POST['jurclass'];
$jur_id=$_POST['jurid'];
$clubsid=$jurClass->getChilJurisdictionsIdList($jur_id, 'club' );
function multi_implode($array, $glue) {
	$ret = '';
	foreach ($array as $item) {
		if (is_array($item)) {
			$ret .= multi_implode($item, $glue) . $glue;
		} else {
			$ret .= $item . $glue;
		}
	}
	$ret = substr($ret, 0, 0-strlen($glue));
	return $ret;
}
$clubs=multi_implode($clubsid,",");

?>

<script type="text/javascript">
$(document).ready(function(){
	getAgencyGames();
});
    
function getAgencyGames()
{	
	$.post("totem/totem_query_service.inc",{action:"9",clubs:"<?=$clubs?>",jurid:<?=$jur_id?>,jurclass:'<?=$jur_class?>'}, function(data){			
	 	 $("#virtualgames").empty().append( data );
		});
};
</script>

<div style="text-align:center;"><h3><?=$lang->getLang("Virtual Games")?> </h3>
<div class="tip"id="duration_virtual"></div>
</div>
<div style="text-align:center;">
	<div id="virtualgames"></div>
</div>