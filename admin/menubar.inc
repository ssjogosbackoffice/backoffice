<script type="text/javascript">
    sfHover = function() {
        var sfEls = document.getElementById("nav").getElementsByTagName("LI");
        for (var i=0; i<sfEls.length; i++) {
            sfEls[i].onmouseover=function() {
                this.className+=" sfhover";
            }
            sfEls[i].onmouseout=function() {
                this.className=this.className.replace(new RegExp(" sfhover\\b"), "");
            }
        }
    }
    if (window.attachEvent) window.attachEvent("onload", sfHover);
</script>
<?php
function writeSubMenu($topMenu, $topKey, $submenu, $http_site){
    global $lang,$menu_parsed;
    $http_site = ($http_site == "") ? HTTP_PROTO . $_SERVER['HTTP_HOST'] : $http_site;
    foreach($submenu as $subKey => $subValue){
//echo $subKey . " " . $subValue;
        list($label, $href, $checkValue) = explode("@",$subValue);
        if(strpos($href,'#') === false){
            if(check_access($checkValue)){
                if(strtolower($http_site)=='href'){
                  $hrefParts=explode('%',$href);?>
                    <li> <a href="<?=$menu_parsed['URL'][$hrefParts[0]].$hrefParts[1]?>"><?=$lang->getLang($label)?></a></li>
                <?
                }
                else{ ?>
                <li> <a href="<?=$http_site . $href?>"><?=$lang->getLang($label)?></a></li>
            <?php
                }
            }
        }else{
            if(check_access($checkValue)){
                $href = str_replace('#','',$href);
                ?>
                <li><a href="#" class="daddy"><?=$lang->getLang($href)?></a><ul>
                        <?php
                        $thirdlevelmenu = $topMenu[$topKey][$href];
                        writeSubMenu($topMenu, $topKey , $thirdlevelmenu, $http_site);
                        ?>
                    </ul></li>
            <?php
            }
        }
    }
}
if (isLoggedIn()) {
?>
<ul id="nav">
    <?php
    $menu_parsed = parse_ini_file(MENU_FILE,true);

    foreach ($menu_parsed as $key => $value){
        if(strtolower($key)!='url'){
        $submenu = $menu_parsed[$key]["submenu"];
        $access_tasks = $menu_parsed[$key]["access"];
        $http_site = (isset($menu_parsed[$key]["site"])) ? $menu_parsed[$key]["site"] : "";
        // DEFAULT: $http_site = (!is_null($menu_parsed[$key]["site"])) ? $menu_parsed[$key]["site"] : "";
        if(check_access($access_tasks)){
            ?>
            <li class="principal" ><a href="#"><?=$lang->getLang($key)?></a>
                <ul>
                    <?php writeSubMenu($menu_parsed, $key, $submenu, $http_site); ?>
                </ul>
            </li>
        <?php
        }
    }
    }

    ?>
    <ul>
        <?php
        }
        ?>




