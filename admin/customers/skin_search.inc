<?php

check_access("customers_show_skin", true);

globalize('doQuery');
globalizeSql('search_string');
globalise('start_row');
globalise('num_rows');
globalise('max_rows');
globalise('show_search_results');
globalize('num_rows_displayed');
globalize('form_submitted');
globalize('field');
globalize('jurisdiction');
globalize('jurisdiction_level');

define('RGX_EMAIL', '#[a-z0-9!\#$%&\'*+/=?^_`{|}~-]+(?:\.[a-z0-9!\#$%&\'*+/=?^_`{|}~-]+)*@(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?#');
define('RGX_IPADD', '#\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\b#');
define('RGX_NUMER', '#^\d+$#');

define('SEARCH_TYPE_NONE',      0x00);
define('SEARCH_TYPE_EMPTY',     0x01);
define('SEARCH_TYPE_USERNAME',  0x02);
define('SEARCH_TYPE_USERID',    0x04);
define('SEARCH_TYPE_CUSTNUM',   0x08);
define('SEARCH_TYPE_EMAIL',     0x10);
define('SEARCH_TYPE_IPADDRESS', 0x20);

$search_string      = trim($search_string);
$exact              = ($_POST['exact'] == "on")?(true):(false);

$serialize_filename = "cust_srch_q.srlz.".session_id();
$filter             = "search_string=$search_string&form_submitted=yes";

$search_type        = SEARCH_TYPE_NONE;

$jurisdiction_level = "club";
$jurisdiction       = getSkinClubId($_SESSION["jurisdiction_id"]);


if($jurisdiction == -1){
  $doQuery = false;
  addError("", "Invalid skin settings, contact assistance");
}

if($doQuery) {  
  settype($jurisdiction,       'integer');
  settype($jurisdiction_level, 'string');
  
  $jurisdiction_level = addslashes($jurisdiction_level);
  $safe_str           = null;
  $sql                = "";
  
  if (!areErrors()) {
    $safe_checked = 1;
    include_once("cust_list_sql_head.inc");

    if (empty($search_string) &&
        ($SESSION["jurisdiction_class"] == "club" || $jurisdiction_level == "club")) {
      $search_type = SEARCH_TYPE_EMPTY;
    }
    elseif ($field == 0) {
      if (preg_match(RGX_NUMER, $search_string)) {
        $search_type = SEARCH_TYPE_USERID | SEARCH_TYPE_CUSTNUM;
        settype($search_string, 'integer');
      }
      elseif (preg_match(RGX_EMAIL, $search_string)) {
        $search_type = SEARCH_TYPE_EMAIL;
        $safe_str    = strtolower(str_replace('%', '%%', $search_string));
      }
      elseif (preg_match(RGX_IPADD, $search_string)) {
        $search_type = SEARCH_TYPE_IPADDRESS;
      }
      else {
        $search_type = SEARCH_TYPE_USERNAME;
        $safe_str    = string2regexp($search_string);
        if ($safe_str == null) {
          addError('', 'Invalid Search String');
        }
      }
    }
    elseif ($field == 1) { 
      $search_type = SEARCH_TYPE_USERNAME;
      $safe_str    = string2regexp($search_string);
      if ($safe_str == null) {
        addError('', 'Invalid Search String');
      }
    }
    elseif ($field == 2) {
      $search_type = SEARCH_TYPE_USERID | SEARCH_TYPE_CUSTNUM;
      settype($search_string, 'numeric');
    }
    elseif ($field == 3) {
      $search_type = SEARCH_TYPE_EMAIL;
      $safe_str    = strtolower(str_replace('%', '%%', $search_string));
      if (!$exact && strlen($search_string < 4)) {
        addError('', 'Please supply more details for email address');
      }
    }
    elseif ($field == 4) {
      $search_type = SEARCH_TYPE_IPADDRESS;
    }
    else {
      addError('', 'Invalid Search String');
    }
  }
  if (!areErrors()) {
    if ($search_type == SEARCH_TYPE_USERNAME) {
      if ($exact)
        $sql .= " AND lower(pun_username)=".$dbh->quote($search_string);
      else
        $sql .= " AND REGEXP_LIKE(pun_username, '$safe_str', 'i')";
    }
    elseif ($search_type == SEARCH_TYPE_EMAIL) {
      if ($exact)
        $sql .= " AND lower(pun_email)=".$dbh->quote($search_string);
      else
        $sql .= " AND lower(pun_email) LIKE ('%$safe_str%')";
    }
    elseif ($search_type == (SEARCH_TYPE_USERID | SEARCH_TYPE_CUSTNUM)) {
      $sql .= " AND (punter.pun_customer_number = $search_string OR pun_id = $search_string)";
    }
    elseif ($search_type == SEARCH_TYPE_IPADDRESS) {
      $sql .= " AND pun_id IN " ;
      $sql .= " (SELECT pun_id FROM punter JOIN customer_log ON clg_punid = pun_id " .
                        "  WHERE clg_clt_code = 1 AND clg_ipaddress = " . $dbh->quote($search_string) .
                        "  GROUP BY pun_id)";
    }
    elseif ($search_type != SEARCH_TYPE_EMPTY) {
      $sql .= " AND 1 = 0 ";
      addError('', 'Search Error');
    }
    include(include_content('customers/cust_search_exec.inc'));
  }
}


if (empty($max_rows)) { $max_rows = 30; }

if ( $show_search_results ) {
  $post_vars = array('page'=>$page,'search_string'=>$search_string, 'filter'=>$filter);
  include(include_content('customers/cust_search_show_results.inc'));
}
?>
<div class="bodyHD"><?=$lang->getLang("View Customers")?></div>
<div class="subnavmenu"><strong><?=$lang->getLang("Main Menu")?></strong></div>
<br />
<?php showErrors(); ?>
<form action="<?=SELF?>" method="POST">
<table cellpadding="4" cellspacing="0" border="0">
<tr>
  <th colspan=2 class="heading"><?=$lang->getLang("Skin Customers Search")?></th>
</tr>
<tr>
  <td colspan=2>&nbsp;</th>
</tr>
<tr valign=top>
  <td colspan=2 class=formheading><b><?=$lang->getLang("Quick Search")?></td>
</tr>
<tr>
  <td class="label" ><?=$lang->getLang("Search Type")?> : </td>
  <td> 
    <select name="field">
      <option value="0"<?= ($field == 0) ? ' selected' : '' ?>> Smart</option>
      <option value="1"<?= ($field == 1) ? ' selected' : '' ?>> User</option>
      <option value="2"<?= ($field == 2) ? ' selected' : '' ?>> Customer Number</option>
      <option value="3"<?= ($field == 3) ? ' selected' : '' ?>> Email Address</option>
      <option value="4"<?= ($field == 4) ? ' selected' : '' ?>> IP Address</option>
    </select> 
  </td>
</tr>
<tr>
  <td class="label"><?=$lang->getLang("Search String")?>: </td>
  <td class="content"> <input name="search_string" type="text" value="<?=$search_string?>" /></td>
</tr>
<tr>
  <td class="label"><?=$lang->getLang("Exact match")?>: </td>
  <td class="content"> <input type="checkbox" name="exact"/></td>
</tr>
<tr>
  <td class="label"><?=$lang->getLang("Number of  Records per page")?>: </td>
  <td class="content">
    <select name="max_rows">
      <option value="10"  <? if ( 10  == $max_rows ) echo 'selected'; ?>>10</option>
      <option value="30"  <? if ( 30  == $max_rows ) echo 'selected'; ?>>30</option>
      <option value="60"  <? if ( 60  == $max_rows ) echo 'selected'; ?>>60</option>
      <option value="100" <? if ( 100 == $max_rows ) echo 'selected'; ?>>100</option>
    </select>
  </td>
</tr>
<tr>
  <td colspan="2">
    <input type="hidden" name="page" value="customers/skin_search">
    <input type="hidden" name="start_search" value="yes">
    <input type="hidden" name="doQuery" value="yes">
    <input type="image" src="<?=image_dir?>/button_submit.gif">
  </td> 
</tr>
</table>
</form>
<br/>	
<?php
function checkValidJurisdiction($jur_class, $jur_id) {
  global $dbh;
  
  $valid_jur_class = array();
  $valid_jur_id    = array();
  $sql             = "";

  if ($_SESSION["jurisdiction_class"] == 'club') {
    $valid_jur_id = array($_SESSION["jurisdiction_id"]);
  }
  elseif ($_SESSION["jurisdiction_class"] == 'district') {
    $valid_jur_class = array("club");
    $sql = "SELECT jur_id FROM jurisdiction WHERE jur_parent_id=".$_SESSION["jurisdiction_id"] .
           " OR (jur_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = ".$_SESSION["jurisdiction_id"]."))";

  }
  elseif($_SESSION["jurisdiction_class"] == 'region') {
    $valid_jur_class = array("district", "club");
    $sql = "SELECT jur_id FROM jurisdiction " .
           " WHERE jur_id = " . $_SESSION["jurisdiction_id"] .
           "  OR (jur_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " . $_SESSION["jurisdiction_id"] . ")) " .
           "  OR (jur_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = ".$_SESSION["jurisdiction_id"].")))";
  }
  elseif($_SESSION["jurisdiction_class"] == 'nation') {
    $valid_jur_class = array("region", "district", "club");
    $sql = "SELECT jur_id FROM jurisdiction " .
           " WHERE jur_id = " . $_SESSION["jurisdiction_id"] .
           "  OR (jur_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " . $_SESSION["jurisdiction_id"] . ")) " .
           "  OR (jur_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = ".$_SESSION["jurisdiction_id"].")))";
           "  OR (jur_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN " .
           "           (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = ".$_SESSION["jurisdiction_id"]."))))";
  }
  elseif ($_SESSION["jurisdiction_class"] == 'casino' || $jur_id==0) { 
    return true;
  }
  else {
    return false;
  }
  if (!in_array($jur_class, $valid_jur_class)) {
    return false;
  }
  
  if (!empty($sql)) {
    $rs = $dbh->exec($sql);
    if ($rs->getNumRows() == 0) {
      return false;
    }
    while ($rs->hasNext()) {
      $r = $rs->next();
      array_push($valid_jur_id, $r["jur_id"]);
    }
  }
  
  var_dump($valid_jur_id);
  
  if (!in_array($jur_id, $valid_jur_id)) {
    return false;
  }
  return true;
}
?>
