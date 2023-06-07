<?php
$prova = check_access('ttm_totem_daily_transtrack' , $redirect = true);
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
<!--
    var actualMonth = new Date().getMonth();
    var monthNames = [ "<?=$lang->getLang('January')?>", "<?=$lang->getLang('February')?>", "<?=$lang->getLang('March')?>", "<?=$lang->getLang('April')?>", "<?=$lang->getLang('May')?>", "<?=$lang->getLang('June')?>", "<?=$lang->getLang('July')?>", "<?=$lang->getLang('August')?>", "<?=$lang->getLang('September')?>", "<?=$lang->getLang('October')?>", "<?=$lang->getLang('November')?>", "<?=$lang->getLang('December')?>" ];

$(document).ready(function(){
	getTotemList();
});

function getTotemList()
{
	$.post("totem/totem_query_service.inc",{action:"6",clubs:"<?php echo $clubs?>",jurid:<?=$jur_id?>,jurclass:'<?=$jur_class?>'}, function(data){
	 	 $("#clubfinancial").fadeOut("slow").fadeIn("slow").empty().append( data );
		});
};
//-->
</script>
<div style="text-align:center;"><h3><?=$lang->getLang("Totem Transactions")?></h3>
<div class="tip"id="duration_transtrack"></div>
</div>
<div style="text-align:center;"><div id="clubfinancial"></div></div>