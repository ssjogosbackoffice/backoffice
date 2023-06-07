<?php 
check_access('ttm_totem_list' , $redirect = true);
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
	$.post("totem/totem_query_service.inc",{action:"1",clubs:"<?php echo $clubs?>",jurid:<?=$jur_id?>,jurclass:'<?=$jur_class?>',type:'details'}, function(data){			
	 	   //alert(data);
                   $("#totems").fadeOut("slow").fadeIn("slow").empty().append( data );
		});
};
//-->
</script>
<div style="text-align:center;"><h3><?=$lang->getLang("Totem Manage List")?></h3></div>
<div style="text-align:center;">
<div style="display: none" id="reset-confirm" title="Reset?">
    		<p><span class="ui-icon ui-icon-alert" style="float: left; margin: 0 7px 20px 0;"></span><?=$lang->getLang("You are attempting to reset this totem.Are you sure?")?></p>
		</div>
<div id="totems"></div>
</div>
