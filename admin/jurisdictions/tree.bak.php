<?php
// fake a jurisdiction id for top level (casino) admin user
$jur_id = ($_SESSION['jurisdiction_id']) ? $_SESSION['jurisdiction_id'] : 1;
// retrieve the appropriate section of the jurisdiction tree
$sql  = 'SELECT jur_id, jur_name, jur_code, jur_class, jur_vat_code,  coalesce(jur_parent_id, 0) as parent_id ' .
'FROM jurisdiction ' .
"WHERE jur_name != 'Internet' AND (" .
// current level (district/region/nation/casino)
'jur_id =  '. $jur_id . ' ' .
// parent
'OR (jur_id = (SELECT jur_parent_id FROM jurisdiction WHERE jur_id = ' . $jur_id . ')) ' .
// grandparent
'OR (jur_id = (SELECT jur_parent_id FROM jurisdiction WHERE jur_id = (SELECT jur_parent_id FROM jurisdiction WHERE jur_id = ' . $jur_id . '))) ' .
// children
'OR (jur_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = ' . $jur_id . ')) ' .
// grandchildren
'OR (jur_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = ' . $jur_id . '))) ' .
// great grandchildren
'OR (jur_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = ' . $jur_id . '))))' .
// superfluous 7th comment
'OR ( jur_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = ' . $jur_id . ')))))' .
") " .
'ORDER BY jur_class, LOWER(jur_name)';
//var_dump($sql);
$rs  = $dbh->exec($sql);
$tree= array(); 
$root=1;

if ( $rs ){
  while ( $rs->hasNext()){
	$row = $rs->next();
    $jur_id = $row['jur_id'];
    $tree[$jur_id] = $row;
  }
}

foreach ( $tree as $id => $value ){
  $par_id = $tree[$id]['parent_id'];
  if ( $par_id  ){
    $tree[$par_id]['children'][] = $id;
  }else{
    $root = $id;
  }
}
//var_dump($tree);
//echo $root;
$class_order = array("club" => 0, "district" => 1, "region" => 2, "nation" => 3, "casino" => 4);
    ?>
    <script type="text/javascript">
    var jurisdictions = new Array();
    <?php
    $counter = 0;
    foreach ($tree as $jur){
      $my_level  = $class_order[$_SESSION['jurisdiction_class']];
      $cur_level = $class_order[$jur["jur_class"]];
      if(!empty($jur["jur_name"]) && $my_level > $cur_level){
        $clean_name = ereg_replace("[^0-9a-z]", "", strtolower($jur["jur_name"]));
        echo "jurisdictions[$counter] = new Array();\n";
        echo "jurisdictions[$counter][\"jur_id\"]    = \"" . $jur["jur_id"] . "\";\n";
        echo "jurisdictions[$counter][\"jur_name\"]  = \"" . str_replace('"', '\"', $jur["jur_name"]) . "\";\n";
        echo "jurisdictions[$counter][\"jur_class\"] = \"" . $jur["jur_class"] . "\";\n";
        echo "jurisdictions[$counter][\"jur_code\"]  = \"" . strtolower($jur["jur_code"]) . "\";\n";
        echo "jurisdictions[$counter][\"jur_vat_code\"]  = \"" . strtolower($jur["jur_vat_code"]) . "\";\n";
        $counter++;
      }
    }
    ?>
    </script>

    <input id="searchBox" type="text" name="searchUser" value="Search jurisdiction..." onfocus="focusSearch()" onkeyup="refreshSearch()" onblur="lostFocus()" size="40"/><br/>
    <div id="searchResult"></div>
    <br/>
    <ul class="legend">
      <li class="casino"><span class="icon">&nbsp;</span>Casino</li>
      <li class="nation"><span class="icon">&nbsp;</span>Nation</li>
      <li class="region"><span class="icon">&nbsp;</span>Region</li>
      <li class="district"><span class="icon">&nbsp;</span>District</li>
      <li class="club"><span class="icon">&nbsp;</span>Club</li>
    </ul>
    <br/>
    <?php
    print_tree($tree, $root, $level=0);
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
  if(searchBox.value == "Search jurisdiction..."){
    searchBox.value = "";
  }
}

function lostFocus(){
  var searchBox = document.getElementById("searchBox");
  if(searchBox.value == ""){
    searchBox.value = "Search jurisdiction...";
  }
}

function refreshSearch(){
  var start          = new Date().getTime();

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
  var res_exact = "<li><span class='title exact'>Jurisdiction Code</span><ol class='exact'>";
  res_r["casino"]   = new Array();
  res_r["nation"]   = new Array();
  res_r["region"]   = new Array();
  res_r["district"] = new Array();
  res_r["club"]     = new Array();

  for(var i in jurisdictions){
    if(searchString.toLowerCase() == jurisdictions[i]["jur_code"]){
      res_exact += "<li><span class='category'>Jur Code</span> <a href='" + "<?=$_SERVER['PHP_SELF'] . "?node="?>" + jurisdictions[i]["jur_id"] +  "'>" + jurisdictions[i]["jur_code"].toUpperCase() + ", " + jurisdictions[i]["jur_name"] + "</a></li>";
      ext++;
    }
    if(jurisdictions[i]["jur_name"].toLowerCase().indexOf(searchString.toLowerCase()) != -1 || searchString.toLowerCase() == jurisdictions[i]["jur_class"] || searchString.toLowerCase() == jurisdictions[i]["jur_vat_code"]){
      res_r[jurisdictions[i]["jur_class"]].push(new Array(jurisdictions[i]["jur_id"], jurisdictions[i]["jur_name"]));
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
         res += "<li><span class='category'>" + i + "</span> <a href='" + "<?=$_SERVER['PHP_SELF'] . "?node="?>" + res_r[i][j][0] +  "'>" + res_r[i][j][1] + "</a></li>";
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
    ul{
      list-style:none;
	    margin:0;
	    padding:0;
    }
    ul a img{
      border:none;
      padding-top:2px
    }
    .exact, .exact a{
      color:#333;
    }
    .casino, .casino a{
      color:#116062;
    }
    .nation, .nation a{
      color:#B22222;
    }
    .region, .region a{
      color:#660099;
    }
    .district, .district a{
      color:#FF6600;
    }
    .club, .club a{
      color:#116062;
    }
    .icon{
      display:inline-block;
      width:16px;
      height:16px;
      clear:both;
      margin:2px;
    }
    .casino .icon{
      background-color:#116062;
    }
    .nation .icon{
      background-color:#B22222;
    }
    .region .icon{
      background-color:#660099;
    }
    .district .icon{
      background-color:#FF6600;
    }
    .club .icon{
      background-color:#116062;
    }

    li.jurisdiction{
      background-image: url(<?=image_dir . '/arrow_tree.gif'?>) no-repeat left 3px;
      padding-top:2px;
      padding-left:15px;
    }

    .title{
      text-align:Center;
      font-weight:Bold;
      font-size:14px;
      display:block;
    	font-variant: small-caps;
    	border-bottom: 1px dashed #CCC;
    	padding-top:10px;
    	margin-bottom:5px;
    }
    .category{
      color:#999;
    }
    ol{
      list-style:none;
      margin:2px;
      padding:5px;
    }
    ol.exact{
      border: 1px solid #333;
      background-color: #CCC;
    }
    ol.nation{
      border: 1px solid #B22222;
      background-color: #FFECE5;
    }
    ol.region{
      border: 1px solid #660099;
      background-color: #E9DDFC;
    }
    ol.district{
      border: 1px solid #FF6600;
      background-color: #FFE4D8;
    }
    ol.club{
      border: 1px solid #116062;
      background-color: #DBF6EA;
    }
    </style>
    <div id="region_popup" style="position:absolute;left:0px;top:0px;visibility:hidden;background-color:gray;font-size:8pt;z-index:57" onmouseout="document.getElementBy='hidden'" onmouseout="this.style.visibility='hidden'">
        Edit<br>
        Add a District<br>
    </div>
    <?php
    function print_tree($tree, $curr_node, $level=0){
      global $node, $dbh, $class_order;
      static $height_offset = HEIGHT_OFFSET;
      if($level == 0){
        echo "<ul class='root'>";
      }
      $children = $tree[$curr_node]['children'];


      $my_level  = $class_order[$_SESSION['jurisdiction_class']];
      $cur_level = $class_order[$tree[$curr_node]["jur_class"]];

      ?>
      <li class="jurisdiction <?=$tree[$curr_node]["jur_class"]?>">
      <?php if($tree[$curr_node]['jur_class'] != "club" && count($children) > 0 && $my_level > $cur_level) : ?>
      <a href="javascript:void(0);" onclick="openChildren('jur_<?=$tree[$curr_node]['jur_id']?>', this);"><img src="<?=image_dir . '/branch_expand.gif'?>"/></a>
      <?php else : ?>
      <img src="<?=image_dir . '/arrow_tree.gif'?>"/>
      <?php endif; ?>

      <?php
      if(empty($tree[$curr_node]['jur_name'])){
        $tree[$curr_node]['jur_name']  = "casino";
        $tree[$curr_node]['jur_class'] = "casino";
      }

      if($cur_level < $my_level){
        echo printLink($_SERVER['PHP_SELF'] . "?node=" . $tree[$curr_node]['jur_id'], $tree[$curr_node]['jur_name'] . ((count($children) > 0)?(" (" . count($children) . ")"):("")));
      }else{
        echo $tree[$curr_node]['jur_name'];
      }
      ?>
      <?php

      if ( $children ){
        ?>

        <ul id="jur_<?=$tree[$curr_node]['jur_id']?>" style="<?=(($cur_level >= $my_level || empty($cur_level))?(""):("display:none"))?>">

        <?php
        foreach ( $children as $key => $child_node ){
          print_tree($tree, $child_node, $level+1);
        }
        ?>

        </ul>

        <?php

      }
      if($level == 0){
        echo "</ul>";
      }

      return;

      $label =  $tree[$curr_node]['jur_name'];

      if ( 0 == $level ) {
        echo '<div style="position:relative;background-color:#ffffff;font-size:8pt" class="tree_menu_item">';
        echo '<b>' . mb_strtoupper($label) . '</b>';
        echo "</div>";
      }else{
        if ( 'nation' == $tree[$curr_node]['jur_class'] || 'region' == $tree[$curr_node]['jur_class'] ){
          echo "<b>".mb_strtoupper($label)."<b>";
        }else{
          echo $label;
        }
        echo '</div></td>';
        if(!$tree[$curr_node]['children']){
          // make sure that the node is not part of the internet branch, as it is
          // the default assignment for new customers
          if(strtolower($tree[$curr_node]['jur_name']) != 'internet'){
            // only create delete node links to those node's below our jurisdiction class
            //
            // this stops a region/district manager from deleting their own region/district,
            //	that should be left up to their parent admin (casino and region users respectively)
            $display_delete_node = false;

            switch($_SESSION['jurisdiction_class']){
              case 'casino':
                $display_delete_node = true;
                break;
              case 'nation':
                if($tree[$curr_node]['jur_class'] == 'region'  || $tree[$curr_node]['jur_class'] == 'district' || $tree[$curr_node]['jur_class'] == 'club')
                $display_delete_node = true;
                break;
              case 'region':
                if($tree[$curr_node]['jur_class'] == 'district' || $tree[$curr_node]['jur_class'] == 'club')
                $display_delete_node = true;
                break;
              case 'district':
                if($tree[$curr_node]['jur_class'] == 'club')
                $display_delete_node = true;
                break;
            }

            if($display_delete_node){
              // make sure that this node has no users attached to it
              // $sql = 'SELECT pun_id FROM punter WHERE pun_betting_club = ' . $tree[$curr_node]['jur_id'] . ' ' .
              //        'UNION ALL (SELECT aus_id FROM admin_user WHERE aus_jurisdiction_id = ' . $tree[$curr_node]['jur_id'] . ')';
              // $rs  = $dbh->exec($sql);
              // if there are no users attached, display the delete icon
              // if($rs->getNumRows() <= 0)
              if(0 == 1){
                echo '<td>';
                $link = "javascript:deleteNode(" . $curr_node . ", '" . $label . "', '" . $tree[$curr_node]['jur_class'] ."');";
                $alt  = 'delete ' . $label . ' ' . $tree[$curr_node]['jur_class'];
                echo '<a href="' . $link . '" >';
                echo '<img src="' . image_dir . '/jurisdiction_del.gif" alt="' . $alt . '" border="0"/>';
                echo '</a>';
                echo '</td>';
              }
            }
          }
        }
        echo '</tr></table>';
      }
      $children = $tree[$curr_node]['children'];

      if ( $children ){
        foreach ( $children as $child_node ){
          $height_offset += 24;
          $left = $par_div_left + 20 * $level+1;
          $width = mb_strlen($tree[$child_node]['jur_name']) * 6;

          if($child_node == $node){
            $bgcolor     = 'white';
            $color       = 'red';
            $font_weight = 'bold';
          }else{
            $bgcolor     = 'white';
            $color       = '#122970';
            $font_weight = 'normal';
          }
          // only create links to those node's below our jurisdiction class
          $display_node_as_link = false;

          switch($_SESSION['jurisdiction_class']){
            case 'casino':
              $display_node_as_link = true;
              break;
            case 'nation':
              if($tree[$child_node]['jur_class'] == 'region' || $tree[$child_node]['jur_class'] == 'district' || $tree[$child_node]['jur_class'] == 'club')
              $display_node_as_link = true;
              break;
            case 'region':
              if($tree[$child_node]['jur_class'] == 'district' || $tree[$child_node]['jur_class'] == 'club')
              $display_node_as_link = true;
              break;
            case 'district':
              if($tree[$child_node]['jur_class'] == 'club')
              $display_node_as_link = true;
              break;
          }

          // if the current node is above or equal to our jurisdiction class
          // make it black, this season's new ... black.

          if($display_node_as_link == false){
            $color       = "#000000";
            $font_weight = "bold";
          }

          echo '<table cellpadding="0" cellspacing="0">';
          echo '<tr><td>';
          echo '<img src="' . image_dir . '/clear.gif" height="13" width="' . ($level*13) . '" />';
          echo '<img src="' . image_dir . '/arrow_tree.gif" width="13" height="13" />';
          echo "</td>";
          echo '<td>';

          echo '<div id="node'.$child_node.'" style="border:none;position:relative;padding:2px;color:'.$color.';background-color:'.$bgcolor.';font-size:8pt;font-weight:'.$font_weight.'"';

          if($child_node != $node && $display_node_as_link == true)
          {
            echo ' onmouseover="node_msover(this, \''.$tree[$child_node]['jur_class'].'\')"
    	    	           onmouseout="node_msout(this, \''.$tree[$child_node]['jur_class'].'\')"
    	    	           onClick="window.location=\'' . $_SERVER['PHP_SELF'] . "?node=" . $child_node . '\'"';
          }
          echo  '>';
          print_tree($tree, $child_node, $level+1);
        }
      }
    }



?>
