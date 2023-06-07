<!-- @Author Marian -->
<?php
$chech_entity=check_access('financial_entity_credits', true);
global $lang;

?>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<script type="text/javascript">
    $(document).ready(function(){
        $.post("reports_casino/entity_report_functions.inc",{action:"0",jurid:"<?=$_SESSION['jurisdiction_id']?>"}, function(data){
            $("#root").empty().append( data );
        });
    });

    function getSubEntityTotals(entityid,entityclass,container,fclub){
            $('#'+container).empty().append('<?=$lang->getLang("Loading ...")?><img src="/media/images/loader.gif" >');


        $.post("reports_casino/entity_report_functions.inc",{action:"5",jurid:entityid,jurclass:entityclass,fclub:fclub}, function(data){
            $("#"+container).empty().append( data );
        });

    }
    function getSubEntity(entityId,results,cscls)
    {
        $('.overlay').show();
        $('#'+entityId).empty().append('<?=$lang->getLang("Loading ...")?><img src="/media/images/loader.gif" >');
        toggle(entityId,cscls);
        $.post("reports_casino/entity_report_functions.inc",{action:"0",jurid:entityId}, function(data){
            $("#"+entityId).empty().append( data );
            $('.overlay').hide();
        }).fail(function() {
            $('#'+entityId).empty().append('Please try again');
            $('.overlay').hide();
        });
    }

    function getClubEntity(entityId,results,cscls)
    {
        $('.overlay').show();
        $('#'+entityId).empty().append('<?=$lang->getLang("Loading ...")?><img src="/media/images/loader.gif" >');
        toggle(entityId,cscls);
        $.post("reports_casino/entity_report_functions.inc",{action:"1",jurid:entityId,results:results}, function(data){
            $("#"+entityId).empty().append( data );
            $('.overlay').hide();
        }).fail(function() {
            $('#'+entityId).empty().append('<?=$lang->getLang("Please try again")?>');
            $('.overlay').hide();
        });
    }

    function toggle(id,cscls)
    {
        if ($('#'+id).is(":visible")) {
            hide(cscls);
            changeIcon(id);
            $('#'+id).slideUp('fast');
        } else {
            hide(cscls);
            changeIcon(id);
            $('#'+id).slideDown('fast');

        }
        return false;
    }
    function showtooltiptable(id){
        $('.tableshown').removeClass('tableshown');
        $('#'+id).addClass('tableshown');
    }

    function hide(details)
    {
        $('.'+details).slideUp('fast');
        return false;
    }


    function changeIcon(id)
    {
        if ($('#'+id).is(":hidden")) {
            var src = $('#img_'+id).attr("src").replace("branch_expand.gif", "branch_collapse.gif");
            $('#img_'+id).attr("src", src);
            $('#img_'+id).parents().closest('div').siblings().find("img:first").attr('src', 'media/images/branch_expand.gif');
        }
        else {
            var src = $('#img_'+id).attr("src").replace("branch_collapse.gif", "branch_expand.gif");
            $('#img_'+id).attr("src", src);
        }
        return false;
    }
</script>
<div class="overlay"></div>

<div class="bodyHD" style="min-width:1000px;"><?=$lang->getlang("Entities Credit Reports")?></div><br>
<div id="jurisdictionTotal">
    <button class='entitybtn' onclick="getSubEntityTotals('<?=$_SESSION['jurisdiction_id']?>','<?=$_SESSION['jurisdiction_class']?>','jurisdictionTotal',0)"><?=$lang->getLang("Show totals")?></button>
</div>
<a href="/reports_casino/transactions.php?jur_id=<?=$_SESSION['jurisdiction_id'];?>"><button class='entitybtn' ><?=$lang->getLang("My transactions")?></button></a>
<br /><br />
<table style="width:100%;border:none;border-collapse:collapse;min-width: 1000px ">
    <tr style="background-color:#E9FCC7;"><td style="color:blue;font-weight:bold;"><?=$lang->getLang("Jurisdiction")?></td><td colspan='2' style="color:blue;text-align:right; font-weight:bold;"><?=$lang->getLang("Available Credit")?></td></tr>
    <tr><td colspan="3"><div id="root"></div></td></tr>
</table>

