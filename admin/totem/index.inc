<?php
check_access('ttm_totem_admin', true);
$jurisdiction_id    = $_SESSION["jurisdiction_id"];
$jurisdiction_class = $_SESSION["jurisdiction_class"];

?>
<style>
  a.red{
    color:Red;
  }
  a.green{
    color: Green;
  }
  code{
    font-size: 14px;
  }
  .online{
    color: Green;
  }
  .offline{
    color: Red;
  }
</style>
<div class="bodyHD"><?=ucwords($jurisdiction_class)?> <?=$lang->getLang("Totem Manager")?></div>

<?php
$action = globalize("action");

if(empty($action)){
  $action = "show";
}

switch ($action){
  case "assign":
    $ttm_id = $_GET["id"];
    settype($ttm_id, "integer");

    $_POST["ttm_status"] = 'P';

    $valid_props = array(
      array("name" => "ttm_address",          "type" => "string",  "beautyname" => "Address", "nullable" => true),
      array("name" => "ttm_city",             "type" => "string",  "beautyname" => "City",    "nullable" => true),
      array("name" => "ttm_note",             "type" => "string",  "beautyname" => "Note",    "nullable" => true),
      array("name" => "ttm_club_id",          "type" => "integer", "beautyname" => "Club",    "nullable" => false),
      array("name" => "ttm_status",           "type" => "string",  "beautyname" => "",        "nullable" => false),
      array("name" => "ttm_locked",           "type" => "string",  "beautyname" => "Lock",    "nullable" => false)
    );

    $properties = array();

    foreach ($valid_props as $prop){
      if($_POST[$prop["name"]] == ""){
        addError($prop["name"], "Missing property " . $prop["beautyname"]);
      }
      if(checkType($_POST[$prop["name"]], $prop["type"], $prop["rules"], $prop["nullable"])){
        $properties[$prop["name"]] = escapeAndQuote($_POST[$prop["name"]], $prop["type"]);//($prop["type"] == "string" || $prop["type"] == "date")?($dbh->quote($dbh->escape($_POST[$prop["name"]]))):($dbh->escape($_POST[$prop["name"]]));
      }else{
        addError($prop["name"], $prop["beautyname"] . " '" . $_POST[$prop["name"]] . "' must be a " . $prop["type"]);
      }
    }

    $sql = "UPDATE totems SET ";

    $update_props = array();

    foreach ($properties as $key => $prop){
      array_push($update_props, $key . " = " . $prop);
    }

    $sql .= implode(", ", $update_props);
    $sql .= " WHERE ttm_id = $ttm_id"; 

    $res = $dbh->exec($sql);

    if($res !== 1){
      addError("", getLang("Error in sql, contact webmaster <!--'%'-->",$sql));
      showErrors();
    }else{
      printInfoMessage(getLang("Totem data updated correctly!"));
      showTotemSearchForm();
      showTotemDetails($ttm_id, true);
      printLegend();
    }

    if(areErrors()){
      showErrors();
      break;
    }
    break;
  case "details":
    showTotemSearchForm();
    ?>
    <div class="bodyHD3">
      <?=getClassLink(refFormPage($_GET["page"]), "Return club List")?>
    </div>
    <br/><br/>
    <?php
    $jur_id = $_GET["id"];
    settype($jur_id, "integer");
    showTotemsListByJurId($jur_id);
    printLegend();
    break;
  case "totemDetails":
    showTotemSearchForm();
    $totem_code = trim($_REQUEST["code"]);

    if(ereg("^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$", $totem_code)){
	    $sql = "SELECT ttm_code FROM totems WHERE ttm_ip_address LIKE " . escapeAndQuote("%$totem_code", "string");
	    $rs  = $dbh->exec($sql);
	
	    if(!$rs->hasNext()){
		    addError("", "Ip not found");
		    showErrors();
		    showTotemsList();
		    exit();
	    }
	
	    $ttm = $rs->next();
	    $totem_code = $ttm["ttm_code"];
    }

    showTotemDetails($totem_code);
    printLegend();
    break;
  case "clean":
    $totem_code = trim($_GET["code"]);
    showClearTotemPage($totem_code);
    break;
  case "show":
    showTotemSearchForm();
    showTotemsList();
    break;
  case "update":
    $ttm_id = $_GET["id"];
    settype($ttm_id, "integer");

    $sql = "SELECT ttm_club_id, jur_name, ttm_status FROM totems LEFT JOIN jurisdiction ON jur_id = ttm_club_id WHERE ttm_id = $ttm_id";

    $rs  = $dbh->exec($sql);

    $totem_status;
    $totem;
    if($rs->hasNext()){
      $totem        = $rs->next();
      $totem_status = $totem["ttm_status"];
    }else{
      addError("", $lang->getLang("Totem doesn't exists"));
    }

    $options = shrinkOptions($_POST["options"]);

    $valid_props = array(
      array("name" => "ttm_address",          "type" => "string",  "beautyname" => "Address", "nullable" => true),
      array("name" => "ttm_city",             "type" => "string",  "beautyname" => "City",    "nullable" => true),
      array("name" => "ttm_note",             "type" => "string",  "beautyname" => "Notes",   "nullable" => true),
      array("name" => "ttm_club_id",          "type" => "integer", "beautyname" => "Betting Club"),
      array("name" => "ttm_locked",           "type" => "string",  "beautyname" => "Lock",    "nullable" => false)
    );

    $properties = array();
    foreach ($valid_props as $prop){
      if($_POST[$prop["name"]] == "" && !$prop["nullable"]){
        addError($prop["name"], "Missing property " . $prop["beautyname"]);
      }
      if(checkType($_POST[$prop["name"]], $prop["type"], $prop["rules"], $prop["nullable"])){
        $properties[$prop["name"]] = escapeAndQuote($_POST[$prop["name"]], $prop["type"]);//($prop["type"] == "string" || $prop["type"] == "date")?($dbh->quote($dbh->escape($_POST[$prop["name"]]))):($dbh->escape($_POST[$prop["name"]]));
      }else{
        addError($prop["name"], $prop["beautyname"] . " '" . $_POST[$prop["name"]] . "' must be a " . $prop["type"]);
      }
    }

    if(areErrors()){
      showErrors();
      showTotemDetails($ttm_id, true);
      break;
    }

    $sql = "UPDATE totems SET ";

    $update_props = array();

    $properties["ttm_status"] = "'A'";
    $properties["ttm_options"] = escapeAndQuote($options, "string");

    foreach ($properties as $key => $prop){
      array_push($update_props, $key . " = " . $prop);
    }

    $sql .= implode(", ", $update_props);

    $sql .= " WHERE ttm_id = $ttm_id";
    //die($sql);
    $rs = $dbh->exec($sql);

    if($rs == 1){
      //$msg  = "Totem properties changed by #" . $_SESSION["admin_id"] . " (" . $_SESSION["admin_username"] . ") ";
      $msg  = "";
      if($totem["ttm_club_id"] != $properties["ttm_club_id"]){
        if(empty($totem["ttm_club_id"])){
          $msg = "Assigned to " . getClubName($properties["ttm_club_id"]) . " (" . $properties["ttm_club_id"] . ")";
        }else{
          $msg = "Changed Jur from " . $totem["jur_name"] . "(" . $totem["ttm_club_id"] . ") to " . getClubName($properties["ttm_club_id"]) . "(" . $properties["ttm_club_id"] . ")";
        }
        $msg .= " by " . $_SESSION["admin_username"] . " (" . $_SESSION["admin_id"] . ")";
        saveTotemLog($ttm_id, $msg);
      }

      printInfoMessage($lang->getLang("Totem assigned with success"));
      showTotemDetails($ttm_id, true);
    }else{
      addError("", $lang->getLang("Error saving totem status"));
      showErrors();
    }

    break;
}

function showClearTotemPage($code){
  global $dbh,$lang;
  $sql   = "SELECT ttm_id, ttm_code, ttm_credit FROM totems WHERE ttm_code = " . $dbh->prepareString($code);
  $totem = $dbh->queryRow($sql);
  ?>
  <div class="bodyHD2">Totem <?=$totem["ttm_code"]?><?=$lang->getLang("Clear Page")?></div><br/>
  <?php if($totem["ttm_credit"] > 0) : ?>
        <?=$lang->getLang("We are sorry, there are still % inside.",getInDollars($totem["ttm_credit"]))?><br/>
    <?=$lang->getLang("You must provide to withdrawal the totem with the procedure.")?><br/><br/>
    <?=getLink(refFormPage($_GET["page"]) . "&action=totemDetails&code=" . $totem["ttm_code"], "Back to totem " . $totem["ttm_code"])?>
  <?php else : ?>
    <?=$lang->getLang("Click on the link below to clear this totem.")?><br/><?=$lang->getLang("All the stored info will be deleted.")?><br/><br/>
    Are you sure?<br/><br/>
    <?=getLink(refFormPage($_GET["page"]) . "&action=cleanForce&code=" . $totem["ttm_code"], "Yes, clear totem "  . $totem["ttm_code"])?>&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;
    <?=getLink(refFormPage($_GET["page"]) . "&action=totemDetails&code=" . $totem["ttm_code"], "No, back to totem " . $totem["ttm_code"])?>
  <?php endif; ?>
  <?php
}

function showActivationPage($rs = null){
    global $lang;
  ?>
  <div class="bodyHD2"><?=$lang->getLang("Totem Activation")?></div>
  <?php
}

function showTotemsList(){
  global $dbh, $jurisdiction_class, $jurisdiction_id,$lang;
  $sql  = "SELECT club.jur_id AS club_id, SUM(tot_totems.ttm_credit) AS money, COUNT(tot_totems.ttm_id) AS totems, COUNT(online_totem.ttm_id) AS o_totems, club.jur_name AS club_name, district.jur_name AS district_name, 
	         region.jur_name AS region_name, nation.jur_name AS nation_name
	         FROM totems tot_totems
	         LEFT JOIN totems online_totem ON tot_totems.ttm_id = online_totem.ttm_id AND online_totem.ttm_online_status = 'online'
	         JOIN jurisdiction club ON club.jur_id = tot_totems.ttm_club_id 
	         JOIN jurisdiction district ON district.jur_id = club.jur_parent_id 
	         JOIN jurisdiction region ON region.jur_id = district.jur_parent_id 
	         JOIN jurisdiction nation ON nation.jur_id = region.jur_parent_id
           WHERE 1 = 1";

  switch ($jurisdiction_class){
    case "club":
      $sql .= " AND club.jur_id = $jurisdiction_id";
      break;
    case "district":
      $sql .= " AND district.jur_id = $jurisdiction_id";
      break;
    case "region":
      $sql .= " AND region.jur_id = $jurisdiction_id";
      break;
    case "nation":
      $sql .= " AND nation.jur_id = $jurisdiction_id";
      break;
  }

  $sql .= " GROUP BY club.jur_id, club.jur_name, district.jur_name, region.jur_name, nation.jur_name
            ORDER BY LOWER(club.jur_name) ASC";

  $rs = $dbh->exec($sql);

  //$cols_arr    = array("Club", "Totems", "Money", "Download Configuration");
  $cols_arr    = array("Club", "District", "Region", "Nation", "Totems", "Download Configuration");

  echo "<br/><div class='bodyHD2'>Assigned Totems</div>";

  $val_fmt_arr = array(
  'Club'                   => 'return getLink(refFormPage($_GET["page"] . "&action=details&id=" . $rec["club_id"]), $rec["club_name"]);',
  'District'               => 'return $rec["district_name"];',
  'Region'                 => 'return $rec["region_name"];',
  'Nation'                 => 'return $rec["nation_name"];',
  'Totems'                 => 'return (($rec["o_totems"] > 0)?("<img width=\"16\" height=\"16\" src=\"" . image_dir . "/icon_online.gif\"/>"):("<img width=\"16\" height=\"16\" src=\"" . image_dir . "/icon_offline.gif\"/>")) . " " . $rec["o_totems"] . " of " . $rec["totems"];',
  'Money'                  => 'return getInDollars($rec["money"]);',
  'Download Configuration' => 'return getLink(refFormPage("totem/configuration&action=download&jur_id=" . $rec["jur_id"]), "Download " . $rec["jur_name"] . " Config");'
  );


  $numrow = $rs->getNumRows();
  if($numrow > 0){
    $qp = new QueryPrinter($rs);
    $qp->printTable($rs, $cols_arr, "", $val_fmt_arr, null, 0, 0, "", $width="100%", 1, $numrow, false);
  }else{
    addError("", $lang->getLang("No totems found"));
    showErrors();
  }

  if($jurisdiction_class == "casino"){
    $cols_arr    = array("Code", "Status", "Club", "Date");

    $val_fmt_arr = array(
     'Date'                   => 'return $rec["ttm_activation_date"];',
     'Status'                 => 'return getTotemStatusIcons($rec);',
     'Club'                   => 'return getLink(refFormPage($_GET["page"] . "&action=details&id=" . $rec["jur_id"]), $rec["jur_name"]);',
     'Code'                   => 'return getLink(refFormPage($_GET["page"] . "&action=totemDetails&code=" . $rec["ttm_code"]), "<code>" . $rec["ttm_code"] . "</code>");'
    );


    $sql = "SELECT ttm_id, UPPER(ttm_code) AS ttm_code, ttm_activation_date, ttm_status, ttm_online_status,ttm_locked,  jur_id, jur_name FROM totems LEFT JOIN jurisdiction ON jur_id = ttm_club_id WHERE ttm_club_id IS NULL OR ttm_status != 'A' ORDER BY ttm_activation_date";
    $rs2 = $dbh->exec($sql);

    $numrow = $rs2->getNumRows();
    if($numrow > 0){
      echo "<br/><div class='bodyHD2'>".$lang->getLang("Pending Totems")."</div>";
      $qp = new QueryPrinter($rs2);
      $qp->printTable($rs2, $cols_arr, "", $val_fmt_arr, null, 0, 0, "", $width="100%", 1, $numrow, false);
    }
  }

}

function getTotemStatusIcons($rec){
  $ret = "";
  if($rec["ttm_online_status"] == "online"){
   $ret .= "<img width=\"16\" height=\"16\" src=\"" . image_dir . "/icon_online.gif\" title=\"Online\"/> ";
  }else{
   $ret .= "<img width=\"16\" height=\"16\" src=\"" . image_dir . "/icon_offline.gif\" title=\"Offline\"/> ";
  };
  if($rec["ttm_status"] == "A"){
   $ret .= "<img width=\"16\" height=\"16\" src=\"" . image_dir . "/icon_active.gif\" title=\"Active\"/> ";
  }elseif($rec["ttm_status"] == "N"){
   $ret .= "<img width=\"16\" height=\"16\" src=\"" . image_dir . "/icon_new.gif\" title=\"New\"/> ";
  }elseif($rec["ttm_status"] == "P"){
   $ret .= "<img width=\"16\" height=\"16\" src=\"" . image_dir . "/icon_travel.gif\" title=\"Pending\"/> ";
  };
  if($rec["ttm_locked"] == "0"){
   $ret .= "<img width=\"16\" height=\"16\" src=\"" . image_dir . "/icon_lock_open.gif\" title=\"Unlocked\"/> ";
  }else{
   $ret .= "<img width=\"16\" height=\"16\" src=\"" . image_dir . "/icon_lock_close.gif\" title=\"Locked\"/> ";
  };
  return $ret;
}

function showTotemSearchForm(){
    global $lang;
  ?>
  <form name="totem_search" action="<?=refFormPage($_GET["page"] . "&action=totemDetails")?>" method="POST">
    <table cellpadding="4" cellspacing="1" border="0">
      <tr>
        <td colspan="2" class="label"><?=$lang->getLang("Totem Search")?></td>
      </tr>
      <tr>
        <td class="label"><?=$lang->getLang("Totem Code")?></td>
        <td class="content"><input type="text" value="" name="code"/></td>
      </tr>
    </table>
  </form>
  <?php
}

function showTotemDetails($totem_code, $isTotemId = false){
  global $dbh, $jurisdiction_class, $jurisdiction_id,$lang;
  
  $sql  = "SELECT ttm_id, ttm_code, ttm_key, ttm_ip_address, ttm_mac_address, ttm_credit, ttm_city, ttm_address, ttm_status, ttm_online_status, ttm_ip_address, ttm_note, ttm_locked, ttm_club_id, ttm_options, ttm_password, ttm_last_logged_in, ttm_version, club.jur_name AS jur_name FROM totems";
  $sql  = "SELECT ttm_id, ttm_code, ttm_key, ttm_ip_address, ttm_mac_address, ttm_credit, ttm_city, ttm_address, ttm_status, ttm_online_status, ttm_ip_address, ttm_note, ttm_locked, ttm_club_id, ttm_options, ttm_password, ttm_last_logged_in, club.jur_name AS club_name FROM totems";
 
  $sql .= " LEFT JOIN jurisdiction club ON club.jur_id = ttm_club_id";

  switch ($jurisdiction_class) {
  	case "club":
      $sql .= " AND ttm_club_id = $jurisdiction_id";
  		break;
  	case "district":
      $sql .= " JOIN jurisdiction district ON district.jur_id = club.jur_parent_id AND district.jur_id = $jurisdiction_id";
      break;
  	case "region":
      $sql .= " JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
                JOIN jurisdiction region   ON region.jur_id   = district.jur_parent_id AND region.jur_id = $jurisdiction_id";
  		break;
  	case "nation":
      $sql .= " JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
                JOIN jurisdiction region   ON region.jur_id   = district.jur_parent_id
                JOIN jurisdiction nation   ON nation.jur_id   = region.jur_parent_id AND nation.jur_id = $jurisdiction_id";
      break;
  }

  if($isTotemId){
    $sql .= " WHERE ttm_id = $totem_code";
  }else{
    $sql .= " WHERE LOWER(ttm_code) = LOWER(" . $dbh->prepareString($totem_code) . ")";
  }

  $totem = $dbh->queryRow($sql);

  if(empty($totem)){
    addError("", $lang->getLang("You are not authorized to view this totem"));
    showErrors();
    return false;
  }

  $totem_options = expandOptions($totem["ttm_options"]);

  $isNew      = ($totem["ttm_status"] == "N");
  $isPending  = ($totem["ttm_status"] == "P");
  $isActive   = ($totem["ttm_status"] == "A");

  if(empty($totem)){
    printInfoMessage($lang->getLang("No totem found for code %",$totem_code));
    return;
  }

  $jurisdiction_id    = $_SESSION['jurisdiction_id'];
  $jurisdiction_class = $_SESSION['jurisdiction_class'];

  if ( 'club' != $jurisdiction_class) {
    include_once ('jurisdiction_filters.php');
  }

  $jurisdiction_id    = $_SESSION["jurisdiction_id"];
  $jurisdiction_level = $_SESSION["jurisdiction_class"];
  ?>
  <div class="bodyHD3">
      <?=getLink(refFormPage($_GET["page"]) . "&action=totemDetails&code=" . urlencode($totem["ttm_code"]), "Refresh")?> |
      <?php if(!empty($totem["ttm_club_id"])) : ?>
      <?=getClassLink(refFormPage($_GET["page"]) . "&action=details&id=" . $totem["ttm_club_id"], "Return to '" . $totem["jur_name"] . "' totem list")?>
      <?php else : ?>
      <?=getClassLink(refFormPage($_GET["page"]), $lang->getLang("Return to club list"))?>
      <?php endif; ?>
    </div>
    <br/><br/>

  <?php
  if($totem["ttm_status"] == "N"){
    printInfoMessage($lang->getLang("This totem need to be activated")."<br>".$lang->getLang("fill the totem details and save to activate.")."<br/><br/>");
  }
  ?>
  <style>
  #table{
   background-image:url(<?=image_dir?>/table.jpg);
  }
  </style>
  <table cellpadding="4" cellspacing="1" border="0">
    <tr>
      <td class="label" colspan="2"><?=$lang->getLang("Totem")?> <code><?=$totem["ttm_code"]?></code></td>
    </tr>
    <tr>
      <td class="label"><?=$lang->getLang("Code")?></td>
      <td class="content"><code><?=$totem["ttm_code"]?></code></td>
    </tr>
    <?php if($jurisdiction_class == "casino"): ?>
    <tr>
      <td class="label"><?=$lang->getlang("Totem Password")?></td>
      <td class="content"><code><?=$totem["ttm_key"]?></code></td>
    </tr>
	  <tr>
	    <td class="label"><?=$lang->getlang("Version")?></td>
	    <td class="content"><code><?=((!empty($totem["ttm_version"]))?("ver. " . $totem["ttm_version"]):("Unknown"))?></code></td>
	  </tr>
    <tr>
      <td class="label"><?=$lang->getlang("Totem Win Password")?></td>
      <td class="content"><code><?=$totem["ttm_password"]?></code></td>
    </tr>

    <?php endif; ?>
    <form name="totem_save" action="<?=refFormPage($_GET["page"] . "&action=update&id=" . $totem["ttm_id"])?>" method="POST">
      <tr>
        <td class="label"><?=$lang->getlang("Address")?></td>
        <td class="content"><input type="text" name="ttm_address" value="<?=$totem["ttm_address"]?>"/></td>
      </tr>
      <tr>
        <td class="label"><?=$lang->getlang("City")?></td>
        <td class="content"><input type="text" name="ttm_city" value="<?=$totem["ttm_city"]?>"/></td>
      </tr>
      <tr>
        <td class="label"><?=$lang->getlang("Notes")?></td>
        <td class="content"><textarea cols="25" rows="4" name="ttm_note"><?=$totem["ttm_note"]?></textarea></td>
      </tr>
      <?php if($jurisdiction_class == "casino" && (empty($totem["ttm_club_id"]) || strtolower($_SESSION["admin_username"]) == "totem" || strtolower($_SESSION["admin_username"]) == "admin")) : ?>
      <tr>
        <td class="label"><?=$lang->getlang("Club")?></td>
        <td>
          <select name="ttm_club_id">
          <?php
          $sql = "SELECT
                    club.jur_id club_id, club.jur_name club_name, district.jur_name district_name, region.jur_name region_name, nation.jur_name nation_name
                  FROM jurisdiction club
                  JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
                  JOIN jurisdiction region   ON region.jur_id   = district.jur_parent_id
                  JOIN jurisdiction nation   ON nation.jur_id   = region.jur_parent_id";

          switch ($jurisdiction_class){
            case "club":
              $sql .= " WHERE club.jur_id = $jurisdiction_id";
              break;
            case "district":
              $sql .= " WHERE district.jur_id = $jurisdiction_id";
              break;
            case "region":
              $sql .= " WHERE region.jur_id = $jurisdiction_id";
              break;
            case "nation":
              $sql .= " WHERE nation.jur_id = $jurisdiction_id";
              break;
          }

          $sql .= " ORDER BY LOWER(TRIM(club.jur_name))";

          $rs = $dbh->exec($sql);
          printOption("jurisdiction_id", "", "Select a Club", ("" == $totem["ttm_club_id"]));
          while($rs->hasNext()){
            $row = $rs->next();
            printOption("jurisdiction_id", $row["club_id"], $row["club_name"] . " (" . $row["district_name"] . ")", ($row["club_id"] == $totem["ttm_club_id"]));
          }
          ?>
          </select>
        </td>
      </tr>
      <?php else : ?>
      <tr>
        <td class="label"><?=$lang->getLang("Club")?></td>
        <td class="label"><?=$totem["jur_name"]?></td>
      </tr>
      <input type="hidden" name="ttm_club_id" value="<?=$totem["ttm_club_id"]?>"/>
      <?php endif; ?>
      <tr>
        <td class="label"><?=$lang->getLang("Locked")?></td>
        <td class="content">
          <input type="radio" name="ttm_locked" value="0" <?=(($totem["ttm_locked"] == "0")?("checked"):(""))?>><?="<img src=" . image_dir . "/icon_lock_open.gif />"?> - Open</input><br/>
          <input type="radio" name="ttm_locked" value="1" <?=(($totem["ttm_locked"] == "1")?("checked"):(""))?>><?="<img src=" . image_dir . "/icon_lock_close.gif />"?> - Locked</input>
        </td>
      </tr>
      <tr>
        <td class="label" colspan="2"><?=$lang->getLang("Totem Status")?></td>
      </tr>
      <tr>
        <td class="label"><?=$lang->getLang("Network Status")?></td>
        <td class="content">
        <?php
        $status = "offline";
        if($totem["ttm_online_status"] == "online"){
          $status = "online";
          echo "<img src=\"" . image_dir . "/icon_online.gif\" title=\"Online\"/>";
        }else{
          echo "<img src=\"" . image_dir . "/icon_offline.gif\" title=\"Offline\"/>";
        };
        ?>
        <span class="<?=$status?>"><?=ucwords($status)?>
        <?php if($jurisdiction_class == "casino") : ?>
        <?=" - " . $totem["ttm_ip_address"]?></code>
        <?php endif; ?>
        </span>
        </td>
      </tr>
      <tr>
        <td class="label"><?=$lang->getLang("Last Connection")?></td>
        <td class="content"><?=$totem["ttm_last_logged_in"]?></td>
      </tr>
      <?php if($jurisdiction_class == "casino") : ?>
      <tr>
        <td class="label"><?=$lang->getLang("MAC Address")?></td>
        <td class="content"><code><?=$totem["ttm_mac_address"]?></code></td>
      </tr>
      <?php endif; ?>
      <tr>
        <td class="label"><?=$lang->getLang("Status")?></td>
        <td class="content">
        <?php
        if($isActive){
          echo "<img src=\"" . image_dir . "/icon_active.gif\" title=\"Active\"/> Active";
        }elseif($isNew){
          echo "<img src=\"" . image_dir . "/icon_new.gif\" title=\"New\"/> New";
        }elseif($isPending){
          echo "<img src=\"" . image_dir . "/icon_travel.gif\" title=\"Pending\"/> Pending";
        };
        ?>
        </td>
      </tr>
      <!--tr>
        <td class="label">Money</td>
        <td class="content"><?=getInDollars($totem["ttm_credit"])?></td>
      </tr-->
      <tr>
        <td class="label" colspan="2"><?=$lang->getLang("Options")?></td>
      </tr>
      <tr>
        <td class="label"><?=$lang->getLang("Tavolo all'interno della sala")?></td>
        <td class="content">
        <select name="options[t]">
        <?php
        printOption("options[t]", "$i", "- Nessun tavolo -", (empty($totem_options["t"])));
        for($i = 1 ; $i <= 15 ; $i++){
	        printOption("options[t]", "$i", "Tav. " . $totem["jur_name"] . " $i", ($i == $totem_options["t"]));
        }
        ?>
        </select>
        </td>
      </tr>
      <tr>
        <td class="label">Numero sedie tavolo</td>
        <td class="content">
        <select name="options[c]">
        <?php
        printOption("options[c]", "$i", "- Nessun tavolo -", (empty($totem_options["c"])));
        for($i = 2 ; $i <= 10 ; $i++){
	        printOption("options[c]", "$i", "$i sedie", ($i == $totem_options["c"]));
        }
        ?>
        </select>
        </td>
      </tr>
      <?php if(false) : ?>
      <tr>
        <td class="label" colspan="2">Enada Settings</td>
      </tr>
      <tr>
        <td class="label">Table ID</td>
        <td class="content">
          <input name="options[t]" value="<?=$totem_options["t"]?>"/>
        </td>
      </tr>
      <tr>
        <td class="label">Username</td>
        <td class="content">
          <input name="options[u]" value="<?=$totem_options["u"]?>"/>
        </td>
      </tr>
      <tr>
        <td class="label">Password</td>
        <td class="content">
          <input name="options[w]" value="<?=$totem_options["w"]?>"/>
        </td>
      </tr>
      <tr>
        <td class="label">Totem Type</td>
        <td class="content">
        <select name="options[l]">
        <?php
        printOption("options[l]", "totem", "Modalità Totem",  ("totem" == $totem_options["l"]));
        printOption("options[l]", "table", "Modalità Tavolo", ("table" == $totem_options["l"]));
        ?>
        </select>
        </td>
      </tr>
    <?php endif; ?>
      <tr>
        <td class="label" valign="top">Seat</td>
        <td>
          <table width="120" height="90" id="table">
            <tr>
              <td><input type="radio" name="options[s]" value="8" <?=(($totem_options["s"] == 8)?("checked"):(""))?>/></td><td>&nbsp;</td><td><input type="radio" name="options[s]" value="9" <?=(($totem_options["s"] == 9)?("checked"):(""))?>/></td><td>&nbsp;</td><td><input type="radio" name="options[s]" value="0" <?=(($totem_options["s"] == 0)?("checked"):(""))?>/></td><td>&nbsp;</td><td align="right"><input type="radio" name="options[s]" value="1" <?=(($totem_options["s"] == 1)?("checked"):(""))?>/></td>
            </tr>
            <tr>
              <td colspan="7"></td>
            </tr>
            <tr>
              <td><input type="radio" name="options[s]" value="7" <?=(($totem_options["s"] == 7)?("checked"):(""))?>/></td><td>&nbsp;</td><td align="center" colspan="3"><input type="radio" name="options[s]" value="c" <?=(($totem_options["s"] == "c")?("checked"):(""))?>/></td><td>&nbsp;</td><td align="right"><input type="radio" name="options[s]" value="2" <?=(($totem_options["s"] == 2)?("checked"):(""))?>/></td>
            </tr>
            <tr>
              <td colspan="7"></td>
            </tr>
            <tr>
              <td><input type="radio" name="options[s]" value="6" <?=(($totem_options["s"] == 6)?("checked"):(""))?>/></td><td>&nbsp;</td><td><input type="radio" name="options[s]" value="5" <?=(($totem_options["s"] == 5)?("checked"):(""))?>/></td><td>&nbsp;</td><td><input type="radio" name="options[s]" value="4" <?=(($totem_options["s"] == 4)?("checked"):(""))?>/></td><td>&nbsp;</td><td align="right"><input type="radio" name="options[s]" value="3" <?=(($totem_options["s"] == 3)?("checked"):(""))?>/></td>
            </tr>
          </table>
        </td>
      </tr>
      <tr>
        <td class="label">Lobby Type</td>
        <td class="content">
        <select name="options[f]">
        <?php
        printOption("options[f]", "0", "Lobby Classica",     ("0" == $totem_options["f"]));
        printOption("options[f]", "1", "Lobby Semplificata", ("1" == $totem_options["f"]));
        ?>
        </select>
        </td>
      </tr>
      <tr>
        <td colspan="2" align="center" colspan="2"><input type="submit" value="Save" size="50"/></td>
      </tr>
      <?php if($jurisdiction_class == "casino") : ?>
      <tr>
        <td colspan="2" align="center" colspan="2">
          <?php
          $dbh->setLimit(10);

          $sql = "SELECT ttl_date, ttl_ip_address, ttl_note FROM totem_logs WHERE ttl_ttm_id = " . $totem["ttm_id"] . " ORDER BY ttl_date DESC";
          $rs  = $dbh->exec($sql);

          $rows = $rs->getNumRows();

          $cols_arr    = array("Date", "Note", "Ip");

          $val_fmt_arr = array(
            'Date' =>'return $rec["ttl_date"];',
            'Note' =>'return $rec["ttl_note"];',
            'Ip'   =>'return $rec["ttl_ip_address"];'
          );

          $qp = new QueryPrinter($rs);
          $qp->printTable($rs, $cols_arr, "", $val_fmt_arr, $cell_fmt_arr, 0, 0, "", $width="100%", 1, $rows, false);
          ?>
        </td>
      </tr>
      <?php endif; ?>
    </form>
  </table>
  <?php
}

function showTotemsListByJurId($jur_id){
  global $dbh, $jurisdiction_class, $jurisdiction_id;
  $sql = "SELECT ttm_id, ttm_code, ttm_city, ttm_address, ttm_status, ttm_online_status, ttm_locked, ttm_credit, ttm_options, ttm_note, ttm_version,
          club.jur_name AS club_name, district.jur_name AS district_name, region.jur_name AS region_name, nation.jur_name AS nation_name
          FROM totems
          JOIN jurisdiction club     ON club.jur_id       = ttm_club_id
          JOIN jurisdiction district ON district.jur_id   = club.jur_parent_id
          JOIN jurisdiction region   ON region.jur_id     = district.jur_parent_id
          JOIN jurisdiction nation   ON nation.jur_id     = region.jur_parent_id
          WHERE club.jur_id = $jur_id";

  $sql = "SELECT ttm_id, ttm_code, ttm_city, ttm_address, ttm_status, ttm_online_status, ttm_locked, ttm_credit, ttm_options, ttm_note,
          club.jur_name AS club_name, district.jur_name AS district_name, region.jur_name AS region_name, nation.jur_name AS nation_name
          FROM totems
          JOIN jurisdiction club     ON club.jur_id       = ttm_club_id
          JOIN jurisdiction district ON district.jur_id   = club.jur_parent_id
          JOIN jurisdiction region   ON region.jur_id     = district.jur_parent_id
          JOIN jurisdiction nation   ON nation.jur_id     = region.jur_parent_id
          WHERE club.jur_id = $jur_id";



  switch ($jurisdiction_class){
    case "club":
      $sql .= " AND club.jur_id = $jurisdiction_id";
      break;
    case "district":
      $sql .= " AND district.jur_id = $jurisdiction_id";
      break;
    case "region":
      $sql .= " AND region.jur_id = $jurisdiction_id";
      break;
    case "nation":
      $sql .= " AND nation.jur_id = $jurisdiction_id";
      break;
  }

  $sql .= " ORDER BY ttm_online_status DESC, ttm_options";
  $rs = $dbh->exec($sql);

  //$cols_arr = array("Code", "Club", "Address", "Money", "Status");
  if($jurisdiction_class == "casino"){
	    $cols_arr = array("Code", "Club", "District", "Region", "Nation", "Address", "Notes", "Table", "Pos.", "Status");
	}else{
		  $cols_arr = array("Code", "Club", "District", "Region", "Nation", "Address", "Notes", "Table", "Pos.", "Status");
	}

  $val_fmt_arr = array(
    'Code'     => 'return getLink(refFormPage($_GET["page"]) . "&action=totemDetails&code=" . $rec["ttm_code"] , "<span class=\'code\'>" . $rec["ttm_code"] . "</pre>");',
    'Club'     => 'return $rec["club_name"];',
    'District' => 'return $rec["district_name"];',
    'Region'   => 'return $rec["region_name"];',
    'Nation'   => 'return $rec["nation_name"];',
    'Address'  => 'return $rec["ttm_address"] . " " . (($rec["ttm_city"])?(" (" . $rec["ttm_city"] . ")"):(""));',
    'Version'  => 'return ((!empty($rec["ttm_version"]))?("ver. " . $rec["ttm_version"]):("Unknown"));',
    'Notes'    => 'return substr($rec["ttm_note"], 0, 20);',
    'Table'    => 'return "Tav. " . $rec["club_name"] . " " . getOptions($rec["ttm_options"], "t");',
    'Pos.'     => 'return getOptions($rec["ttm_options"]);',
    'Status'   => 'return getTotemStatusIcons($rec);'
  );

  $numrow = $rs->getNumRows();
  if($numrow > 0){
    $qp = new QueryPrinter($rs);
    $qp->printTable($rs, $cols_arr, "", $val_fmt_arr, null, 0, 0, "", $width="100%", 1, 2000, false);
  }else{
    addError("", "No totem found");
    showErrors();
  }

  ?>
  <br/>
  <?php
  print getLink(refFormPage("totem/configuration&action=download&jur_id=$jur_id"), "Download Config");

}

function getOptions($options, $type=null){
  $opt_r = explode(";", $options);
  $ret   = "";

  foreach ($opt_r as $o){
    list($prop, $value) = explode(":", $o);
    if(empty($type)){
      switch ($prop){
        case "s":
          if($value !== ""){
            $ret .= "<img src='" . image_dir . "/seat_$value.gif'/>";
          }
          break;
      }
    }elseif($prop == $type){
	    $ret .= $value;
    }
  }
  return $ret;
}

function printLegend(){
  ?>
  <br/><br/>
  <table>
    <tr>
      <td><?="<img src=" . image_dir . "/icon_online.gif />"?></td>
      <td>Online</td>
      <td>&nbsp;</td>
      <td><?="<img src=" . image_dir . "/icon_offline.gif />"?></td>
      <td>Offline</td>
    </tr>
    <tr>
      <td><?="<img src=" . image_dir . "/icon_new.gif />"?></td>
      <td>Never used</td>
      <td>&nbsp;</td>
      <td><?="<img src=" . image_dir . "/icon_travel.gif />"?></td>
      <td>Pending..</td>
    </tr>
    <tr>
      <td><?="<img src=" . image_dir . "/icon_lock_open.gif />"?></td>
      <td>Open</td>
      <td>&nbsp;</td>
      <td><?="<img src=" . image_dir . "/icon_lock_close.gif />"?></td>
      <td>Locked</td>
    </tr>
  </table>
  <?php
}

function expandOptions($options){
  $ret = array();
  $params = explode(";", $options);
  foreach ($params as $param){
    $app = explode(":", $param);
    $ret[$app[0]] = $app[1];
  }
  return $ret;
}

function shrinkOptions($options){
  $ret = array();
  foreach ($options as $key => $opt){
    array_push($ret, "$key:$opt");
  }
  return implode(";", $ret);
}

function getStatus($rec){
  if($rec["skn_status"] == '0'){
    return "<span>" . getClassLink(refFormPage($_GET["page"] . "&action=enable&skn_id="   . $rec["skn_id"]), "Disabled", "red")   . "</span>";
  }else{
    return "<span>" . getClassLink(refFormPage($_GET["page"] . "&action=disable&skn_id="  . $rec["skn_id"]), "Enabled",  "green") . "</span>";
  }
}

function getSkinsSelect($default=null){
  ?>
  <select name="skn_foldername">
  <?php
  $skinDir = opendir(SKINS_PATH);
  $skinsArray = array();

  while($file = readdir($skinDir)){
    if(!ereg("^[:\.]", $file)){
      array_push($skinsArray, $file);
    }
  }
  sort($skinsArray);
  foreach ($skinsArray as $skin) {
    printOption("skn_foldername", $skin, ucfirst($skin), ($default == $skin));
  }
  ?>
  </select>
  <?php
}

function saveTotemLog($totem_id, $message, $ip_address=null){
  global $dbh;

  if(empty($ip_address)){
    $ip_address = getIpAddress();
  }

  $sql = "INSERT INTO totem_logs (ttl_ttm_id, ttl_ip_address, ttl_date, ttl_note)
          VALUES ($totem_id, '" . $dbh->escape($ip_address) . "', CURRENT_TIMESTAMP, '" . $dbh->escape($message) . "')";
  $rs  = $dbh->exec($sql);
}

?>
                                                                                                           