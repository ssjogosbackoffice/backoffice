<?php
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
    $(document).ready(function(){
        getTotemList();
    });

    function getTotemList()
    {
        $.post("totem/totem_query_service.inc",{action:"1",clubs:"<?php echo $clubs?>",jurid:<?=$jur_id?>,jurclass:'<?=$jur_class?>',type:'closure'}, function(data){
            $("#totempartial").fadeOut("slow").fadeIn("slow").empty().append( data );
        });
    };

    function hideClosure()
    {
        $("#part").hide('slow');
        $("#add_part").hide('slow');

    }
    //-->
</script>

<div style="text-align:center;"><h3><?=$lang->getLang("Totem Partial Closure")?></h3></div>

<div style="text-align:center;">
    <div id="totempartial"></div>
</div>
