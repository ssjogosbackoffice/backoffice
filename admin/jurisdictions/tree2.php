 <?php
include "JurisdictionsList.class.inc";
$jur_list = JurisdictionsList::getInstance($dbh);



$list = $jur_list->getChildJurisdictions($_SESSION['jurisdiction_id'],0);
$tree = $jur_list->getJurisdictionTree($_SESSION['jurisdiction_id'], 0);
$class_order = array("club" => 0, "district" => 1, "region" => 2, "nation" => 3, "casino" => 4);
define("NEW_ELAPSED_SECONDS", 60 * 60 * 48);
?>

    <script type="text/javascript">
        <?php
        $counter = 0;
        $js_jur_array = array();
        $now = time();
        foreach($list as $key => $jur_class){
          foreach($jur_class as $jur){
            $short_class = substr($jur["class"], 0, 1);
            //0 = jur_id
            //1 = name
            //2 = class
            //3 = jur_code
            //4 = parent_id
            //5 = IT - partita IVA
            //6 - new
            $str = "[" . $jur["id"] . ",\"" . str_replace('"', '\"', $jur["name"]) . "\",\"" . $short_class . "\",\"" . strtoupper($jur["code"]) . "\"," . $jur["parent_id"] . ",\"" . $jur["vat_code"] . "\",\"" . ((($now - $jur["date"]) <= NEW_ELAPSED_SECONDS)?("1"):("0")) . "\"]";
            $js_jur_array[] = $str;
          }
        }
        echo "var jurisdictions = [" . implode(",", $js_jur_array) . "];";
        ?>
    </script>

    <input id="searchBox" type="text" name="searchUser" value="<?=$lang->getLang("Search jurisdiction")?>..." onfocus="focusSearch()" onkeyup="refreshSearch()" onblur="lostFocus()" size="40"/><br/>
    <div id="searchResult"></div>
    <br/>
    <ul class="legend">
        <li class="casino"><span class="tip"><span class="icon">C</span></span>Casino</li>
        <li class="nation"><span class="tip"><span class="icon">N</span></span>Nation</li>
        <li class="region"><span class="tip"><span class="icon">R</span></span>Region</li>
        <li class="district"><span class="tip"><span class="icon">D</span></span>District</li>
        <li class="club"><span class="tip"><span class="icon">C</span></span>Club</li>
    </ul>
    <br/>
<?php

if(empty($node)){
    $node = $_REQUEST["node"];
    settype($node, "integer");
}

if($node > 0){
    $open_hierarchy = $jur_list->getJurisdictionHierarchy($node);
}
print_tree($tree);
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
                    res_exact += "<li><span class='category'>Jur Code</span> <a href='" + "<?=$_SERVER['PHP_SELF'] . "?node="?>" + jurisdictions[i][0] +  "'>" + jurisdictions[i][3] + ", " + jurisdictions[i][1] + "</a></li>";
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
                        res += "<li><span class='category'>" + i + "</span> <a href='" + "<?=$_SERVER['PHP_SELF'] . "?node="?>" + res_r[i][j][0] +  "'>" + res_r[i][j][1] + "</a> ";

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
<?php
function print_tree($tree, $father_id = null, $father_class = null){
    global $class_order, $open_hierarchy, $node, $lang;
    $my_level     = $class_order[$_SESSION['jurisdiction_class']];
    $cur_level    = $class_order[$father_class];

    $open_root = ($open_hierarchy[$father_class] == $father_id);

    ?>
    <ul id="jur_<?=$father_id?>" style="<?=(($cur_level >= $my_level || $open_root || empty($cur_level))?(""):("display:none"))?>">
        <?php

        $now = time();

        foreach($tree as $branch){
            $has_child = !empty($branch["child"]);

            $open_branch = false;
            if(!empty($open_hierarchy)){
                foreach($open_hierarchy as $open){
                    if($open == $branch["id"]){
                        $open_branch = true;
                        break;
                    }
                }
            }
            ?>
            <li class="jurisdiction <?=$branch["class"]?> <?=(($open_branch)?("selected"):(""))?>">
                <?php if($open_branch && $has_child && $branch["class"] != "club") : ?>
                    <a href="javascript:void(0);" onclick="openChildren('jur_<?=$branch['id']?>', this);"><img src="<?=image_dir . '/branch_collapse.gif'?>"/></a>
                <?php elseif($has_child) : ?>
                    <a href="javascript:void(0);" onclick="openChildren('jur_<?=$branch['id']?>', this);"><img src="<?=image_dir . '/branch_expand.gif'?>"/></a>
                <?php else : ?>
                    <img src="<?=image_dir . '/arrow_tree.gif'?>"/>
                <?php endif;

                $child_class = "";
                switch($father_class){
                    case "casino":
                        $child_class = "region";
                        break;
                    case "nation":
                        $child_class = "district";
                        break;
                    case "region":
                        $child_class = "club";
                        break;
                }
                ?>
                <span class="tip"><span class="icon"><?=strtoupper(substr($branch["class"], 0, 1))?></span></span>
                <?php
                if($node == $branch['id']){
                    printClassLink($_SERVER['PHP_SELF'] . "?node=" . $branch['id'], $branch['name'], "current_node");
                }elseif($branch['id'] == $_SESSION['jurisdiction_id']){
                    echo $branch['name'];
                }else{
                    printClassLink($_SERVER['PHP_SELF'] . "?node=" . $branch['id'], $branch['name'], "name");
                }

                if($now - $branch["date"] <= NEW_ELAPSED_SECONDS){
                    ?>
                    <img src="<?=image_dir . '/icon_new.gif'?>"/>
                    <?php
                    if(function_exists("getReadableDate")){
                        echo  getReadableDate($branch["date"]);
                    }
                    ?>
                <?php
                }
                if(!empty($child_class)){
                    ?>
                    <span class="tip"><?=((count($branch["child"]) > 0)?(" (" . count($branch["child"]) . " $child_class)"):(""));?></span>
                <?php
                }
                if($branch["jur_status"] == '0'){
                    ?>
                    <span class="tip" style="color:red">(<?=$lang->getLang("blocked")?>)</span>
                <?php
                }
                if($branch["jur_status"] == '2'){
                    ?>
                    <span class="tip" style="color:#ff6600">(<?=$lang->getLang("blocked")?>)</span>
                <?php
                }
                if($branch["jur_status"] == '3'){
                    ?>
                    <span class="tip" style="color:#660099">(<?=$lang->getLang("blocked")?>)</span>
                <?php
                }
                if($branch["jur_status"] == '4'){
                    ?>
                    <span class="tip" style="color:#b22222">(<?=$lang->getLang("blocked")?>)</span>
                <?php
                }
                if($has_child){
                    print_tree($branch["child"], $branch['id'], $branch['class']);
                }
                ?>
            </li>
        <?php
        }
        ?>
    </ul>
    <?php
    return;
}

?>