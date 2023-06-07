<?php
check_access('totem_administration' , $redirect = true);
include_once 'JurisdictionsList.class.inc';
$jurClass = JurisdictionsList::getInstance ( $dbh );
$hierarchy = $jurClass->getJurisdictionTree($_SESSION['jurisdiction_id']);

?>
<script type=text/javascript>
    var jurisdictions=[];
</script>
<?
function showChild($childArray){
    $counter = 0;

    $now = time();
    foreach($childArray as $key => $value){
        $short_class = substr($value["class"], 0, 1);
        if($value['class'] == 'casino'){
            $class_type = "casino";
        }elseif($value['class'] == 'nation'){
            $class_type = "nation";
        }elseif($value['class'] == 'region'){
            $class_type = "region";
        }elseif($value['class'] == 'district'){
            $class_type = "district";
        }elseif($value['class'] == 'club'){
            $class_type = "club";
        }
        $str = "[" . $value["id"] . ",\"" . str_replace('"', '\"', $value["name"]) . "\",\"" . $short_class . "\",\"" . strtoupper($value["code"]) . "\"," . $value["parent_id"] . ",\"" . ((($now - $value["date"]) <= NEW_ELAPSED_SECONDS)?("1"):("0")) . "\"]";
        echo "
    <script>
    jurisdictions.push(".$str.");
    </script>

    ";
        if(!is_null($value['child'])){
            ?><li><span class="folder <?=$class_type?>" id="tree_<?=$value['id']?>"><a href="javascript:openDetail('<?=$value['id']?>')"><?=isset($value["code"])? "[".strtoupper($value["code"])."]" : ''?><?=strtoupper($value['name'])?></a></span><ul><?php
                showChild($value['child']);
                ?></ul></li>
        <?php
        }else{
            ?><li><span class="file <?=$class_type?>" id="tree_<?=$value['id']?>"><a href="javascript:openDetail('<?=$value['id']?>')"><?=isset($value["code"])? "[".strtoupper($value["code"])."]" : ''?> <?=strtoupper($value['name'])?></a></span></li><?php
        }

    }

}
?>
    <script language="javascript">
    var lastSearch   = "";

    function openChildren(father_id, target){
        var list = document.getElementById(father_id);
        if(list == null){
            return;
        }

        if(list.style.display == "none"){
            list.style.display = "block";
            target.innerHTML = "<img src=\"<?=image_dir . '/branch_collapse.gif'?>\"/>";
        }else{
            list.style.display = "none";
            target.innerHTML = "<img src=\"<?=image_dir . '/branch_expand.gif'?>\"/>";
        }

    }

    function focusSearch(){
        var searchBox = document.getElementById("searchBox");
        if(searchBox.value == "<?=$lang->getLang("Search jurisdiction")?>..."){
            searchBox.value = "";
        }
    }

    function lostFocus(){
        var searchBox = document.getElementById("searchBox");
        if(searchBox.value == ""){
            searchBox.value = "<?=$lang->getLang("Search jurisdiction")?>...";
        }
    }

    function findParent(jur_id){
        var ret = ["", "", "", "", "", ""];
        for(var i in jurisdictions){
            if(jurisdictions[i][0] == jur_id){
                ret = jurisdictions[i];
                break;
            }
        }
        return ret;
    }

    var cached_parent = new Array();

    function refreshSearch(){
        var start        = new Date().getTime();

        var searchBox    = document.getElementById("searchBox");
        var searchResult = document.getElementById("searchResult");
        var searchString = searchBox.value;

        if(searchString == lastSearch){
            return;
        }

        lastSearch = searchString;

        searchResult.innerHTML = "";

        if(searchString.length <= 1){
            return;
        }

        var res = "<ul>";
        var cnt = 0;
        var ext = 0;
        var res_r = new Array();
        var res_exact = "<li><span class='title exact'><?=$lang->getLang("Jurisdiction Code")?></span><ol class='exact'>";
        res_r["casino"]   = new Array();
        res_r["nation"]   = new Array();
        res_r["region"]   = new Array();
        res_r["district"] = new Array();
        res_r["club"]     = new Array();

        counter = 0;

        var search_str_lower = searchString.toLowerCase();
        var jur_class_conversion = new Array();
        jur_class_conversion["z"] = "casino";
        jur_class_conversion["n"] = "nation";
        jur_class_conversion["r"] = "region";
        jur_class_conversion["d"] = "district";
        jur_class_conversion["c"] = "club";

        for(var i in jurisdictions){
            if(searchString.toUpperCase() == jurisdictions[i][3]){
                res_exact += "<li><span class='category'>Jur Code</span><a href='javascript:void(0)' onclick='openMyOwnBranchInTree("+jurisdictions[i][0]+");openDetail("+jurisdictions[i][0]+")'>" + jurisdictions[i][3] + ", " + jurisdictions[i][1] + "</a></li>";
                ext++;
            }

            if(search_str_lower == "new" && jurisdictions[i][6] == "1" || jurisdictions[i][1].toLowerCase().indexOf(search_str_lower) != -1 || (jurisdictions[i][5] != undefined && jurisdictions[i][5].indexOf(search_str_lower) != -1)){
                res_r[jur_class_conversion[jurisdictions[i][2]]].push(jurisdictions[i]);
                cnt++;
            }
        }

        res_exact += "</ol></li>";

        if(ext > 0){
            res += res_exact;
        }

        for(var i in res_r){
            if(res_r[i].length > 0){
                res += "<li><span class='title " + i + "'>" + i + "</span>";
                res += "<ol class='" + i + "'>";
                for(var j = 0 ; j < res_r[i].length ; j++){
                    res += "<li><span class='category'>" + i + "</span> <a href='javascript:void(0)' onclick='openMyOwnBranchInTree("+res_r[i][j][0]+");openDetail("+res_r[i][j][0]+")'>" + res_r[i][j][1] + "</a> ";

                    if(res_r[i][j][6] == "1"){
                        res += "<img src='<?=image_dir . '/icon_new.gif'?>'/>";
                    }

                    res += "</li>";
                }
                res += "</ol>";
                res += "</li>";
            }
        }

        res += "</ul>";

        var end = new Date().getTime();

        res = "<b>" + (cnt+ext) + "</b> results found in " + ((end-start)/1000) + " sec.<br/>" + res;

        searchResult.innerHTML = res;
    }


</script>
<style>
    .folder a{
        text-decoration: none;
    }
    .file a{
        text-decoration: none;
    }
</style>
<link rel="Stylesheet" type="text/css" href="<?= secure_host ?>/media/treeview/jquery.treeview.css" />
<link rel="Stylesheet" type="text/css" href="<?= secure_host ?>/media/ui/css/cupertino/jquery-ui-1.8.24.css" />

<script src="<?= secure_host ?>/media/treeview/jquery.treeview.js" type="text/javascript"></script>
<script type="text/javascript">
    var progressStep = 0;
    var progressInterval;
    function ask()
    {
        progressStep = progressStep + 2;
        $( "#progressbar").progressbar({
            value: progressStep
        });
    }
    $(document).ready(function(){
        $('#progressbar').ajaxStart(function(){
            $(this).show();
            progressStep = 0;
            progressInterval = setInterval("ask();", 500);
        });
        $('#progressbar').ajaxStop(function(){
            clearInterval(progressInterval);
            $( "#progressbar").progressbar({
                value: 0
            });
            $(this).hide();
        });
    })
    $(document).ready(function(){
        $("#jurtree").treeview({animated: "fast", speed: "fast", collapsed: true,unique: true });
    });
    function loadData(data, textStatus, jqXHR){
        $('#management').html(data);
    }
    function openDetail(divID){
        $('#management').load('totem/totem_financial_detail.inc',{jurid:divID});
        $('#detailcontent').animate({scrollTop: 0}, 800);
    }
    function openMyOwnBranchInTree(idLeaf) {
        $('#tree_' + idLeaf).parents('ul').show();
        var searchBox = document.getElementById("searchBox");
        searchBox.value ="";
        refreshSearch();
        lostFocus();
            searchBox.value = "<?=$lang->getLang("Search jurisdiction")?>...";
    }

</script>
<style>
    .ui-progressbar .ui-progressbar-value { background-image: url(media/images/progressbar.gif); }
</style>

<div style="border:1px solid black;width:1000px;height:900px;overflow-y:auto;overflow-x:hidden;" id="detailcontent">
    <div id="head" style="background-color:#0085FF;height:50px;margin-top:-15px;text-align:center;color:#FFFFFF;">
        <h2 style="padding-top: 10px;"><?=$lang->getLang("Totem Management Panel")?></h2>
    </div>

    <div id="content-left" style="float:left;width:300px;padding:15px;">
        <h4 style="margin-bottom: 0"><?=$lang->getLang("Search jurisdiction")?></h4>
        <input id="searchBox" type="text" name="searchUser" value="<?=$lang->getLang("Search jurisdiction")?>..." onfocus="focusSearch()" onkeyup="refreshSearch()" onblur="lostFocus()" size="25"/><br/>
        <div id="searchResult"></div>
        <br/>
        <ul id="jurtree" style="float:left;width:250px;">


            <?php
            showChild($hierarchy);
            ?>
        </ul>
    </div>
    <div id="detailed" style="float:left;width:650px;">
        <div id="management"></div>
        <div id="progressbar" style="width:220px;height:25px;position:absolute;top:50%;left:50%;z-index:10;"></div>
    </div>
</div>
