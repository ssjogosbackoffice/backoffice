<?php

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
global $lang;

define('RGX_EMAIL',    '#[a-z0-9!\#$%&\'*+/=?^_`{|}~-]+(?:\.[a-z0-9!\#$%&\'*+/=?^_`{|}~-]+)*@(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?#');
define('RGX_IPADD',    '#\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\b#');
define('RGX_NUMER',    '#^\d+$#');
define('RGX_NIN_CODE', '/^[a-z]{6}[0-9]{2}[a-z]{1}[0-9]{2}[a-z]{1}[0-9]{3}[a-z]{1}$/i');
define('RGX_NAME',     '/^.*,.*/i');

define('SEARCH_TYPE_NONE',         0);
define('SEARCH_TYPE_EMPTY',        1);
define('SEARCH_TYPE_USERNAME',     2);
define('SEARCH_TYPE_USERID',       3);
define('SEARCH_TYPE_CUSTNUM',      4);
define('SEARCH_TYPE_EMAIL',        5);
define('SEARCH_TYPE_IPADDRESS',    6);
define('SEARCH_TYPE_PARTNERUSER',  7);
define('SEARCH_TYPE_NIN_CODE',     8);
define('SEARCH_TYPE_NAME',         9);



$search_string      = trim($search_string);
$exact              = ($_POST ['exact'] == "on" || !$doQuery) ? true : false;

$serialize_filename = "cust_srch_q.srlz.".session_id();
$filter             = "search_string=$search_string&form_submitted=yes";

$search_type        = SEARCH_TYPE_NONE;
//$search_string      = $dbh->escape($search_string);


if ($doQuery) {

    settype($jurisdiction,       'integer');
    settype($jurisdiction_level, 'string');
    $jurisdiction_level = addslashes($jurisdiction_level);

    $safe_str  = null;

    $sql       = "";
    if ($_SESSION["jurisdiction_class"] == "club") {
        $jurisdiction_level = "club";
        $jurisdiction       = $_SESSION["jurisdiction_id"];
    }
    elseif (!checkValidJurisdiction($jurisdiction_level, $jurisdiction)) {
        addError('', "Invalid jurisdiction selected");
    }

    if (!areErrors()) {
        $safe_checked = 1;
        if (empty($search_string) &&
            ($_SESSION["jurisdiction_class"] == "club" || $jurisdiction_level == "club")) {
            $search_type = SEARCH_TYPE_EMPTY;
        } elseif($field == 0) {
            if (preg_match(RGX_NUMER, $search_string)) {
                $search_type =  SEARCH_TYPE_CUSTNUM;
                settype($search_string, 'integer');
            }
            elseif (preg_match(RGX_EMAIL, $search_string)) {
                $search_type = SEARCH_TYPE_EMAIL;
                $safe_str    = strtolower(str_replace('%', '%%', $search_string));
            }
            elseif (preg_match(RGX_IPADD, $search_string)) {
                $search_type = SEARCH_TYPE_IPADDRESS;
            }elseif (preg_match(RGX_NIN_CODE, $search_string)){
                $search_type = SEARCH_TYPE_NIN_CODE;
            }elseif (preg_match(RGX_NAME, $search_string)){
                $search_type = SEARCH_TYPE_NAME;
            }
            else {
                $search_type = SEARCH_TYPE_USERNAME;
                $safe_str    = $search_string; //Modificata Manuel Rinaldi
                if ($safe_str == null) {
                    addError('', $lang->getLang('Invalid Search String'));
                }
            }
        }
        elseif ($field == 1) {
            $search_type = SEARCH_TYPE_USERNAME;
            $safe_str    = $search_string; //Modificata Manuel Rinaldi
            if ($safe_str == null) {
                addError('', $lang->getLang('Invalid Search String'));
            }
        }
        elseif ($field == 2) {
            $search_type = SEARCH_TYPE_CUSTNUM ;
            $search_string= (int)$search_string;

        }
        elseif ($field == 3) {
            $search_type = SEARCH_TYPE_EMAIL;
            $safe_str    = strtolower(str_replace('%', '%%', $search_string));
            if (!$exact && strlen($search_string) < 4) {
                addError('', $lang->getLang('Please supply more details for email address'));
            }
        }
        elseif ($field == 4) {
            $search_type = SEARCH_TYPE_IPADDRESS;
        }
        elseif ($field == 5){
            $search_type = SEARCH_TYPE_NIN_CODE;
        }
        elseif ($field == 6){
            $search_type = SEARCH_TYPE_NAME;
        }

        elseif ($field == 7){
            $search_type = SEARCH_TYPE_PARTNERUSER;
            $search_string = (int)$search_string;


        }
        else {
            addError('', $lang->getLang('Invalid Search String'));
        }
    }

    include_once("cust_list_sql_head.inc");
    if (!areErrors()) {
        if ($search_type == SEARCH_TYPE_USERNAME) {
            if ($exact)
                $sql .= " AND pun_username=".$dbh->quote($search_string);
            else
                $sql .= " AND pun_username LIKE '%$safe_str%'"; //Modificata query Manuel Rinaldi
        }
        elseif ($search_type == SEARCH_TYPE_EMAIL) {
            if ($exact)
                $sql .= " AND pun_email=" . $dbh->quote($search_string);
            else
                $sql .= " AND pun_email LIKE ('%$safe_str%')";
        }
        elseif ($search_type == SEARCH_TYPE_PARTNERUSER) {

            $sql .= " AND (";
            $sql .= " pun_id IN (SELECT pnu_local_id FROM partner_users WHERE pnu_id = $search_string) ";
            $sql .= ")";
        }
        elseif ($search_type == SEARCH_TYPE_CUSTNUM) {
            $sql .= " AND (";
            $sql .= " (pun_customer_number = $search_string OR pun_id = $search_string ) ";

            /* if($_SERVER["HTTP_HOST"] == "poker.leonardogames.it"){
             $sql .= " OR REGEXP_LIKE(pun_first_name, '[a-z0-9]{3}_$search_string', 'i')";
            $sql .= " OR pun_id IN (SELECT pnu_local_id FROM partner_users WHERE pnu_username = 'itp_$search_string')";
            }*/ //Modificata query Manuel Rinaldi

            $sql .= ")";


            //$sql .= " AND (pun_customer_number = $search_string OR pun_id = $search_string OR REGEXP_LIKE(pun_first_name, '[a-z0-9]{3}_$search_string', 'i') OR pun_id IN (SELECT pnu_local_id FROM partner_users WHERE pnu_id = $search_string))";
        }

        elseif ($search_type == SEARCH_TYPE_IPADDRESS) {
            $sql .= " AND pun_id  = clg_punid AND clg_ipaddress = " . $dbh->quote($search_string) ." GROUP BY pun_id ";
            $ip =$search_string;
            require_once 'getIp.php';
        }
        elseif ($search_type == SEARCH_TYPE_NIN_CODE){
            $sql .= " AND pun_nin_code = " . $dbh->quote($search_string) . " ";
        }elseif ($search_type == SEARCH_TYPE_NAME) {
            list($last_name, $first_name) = explode(",", $search_string);

            $sql .= " AND (TRIM(pun_last_name) = " . $dbh->quote(trim(strtolower($last_name )));

            if(!is_null($first_name)){
                $sql .= " AND TRIM(pun_first_name) = " . $dbh->quote(trim(strtolower($first_name)));
            }
            $sql .= ")";
        }
        elseif ($search_type != SEARCH_TYPE_EMPTY) {
            $sql .= " AND 1 = 0 ";
            addError('', 'Search Error');
        }
        include('customers/cust_search_exec.inc');
        //echo "<div>$sql</div>";
    }
}

if (empty($max_rows)) { $max_rows = 30; }

if ( $show_search_results ) {
    $post_vars = array('page'=>$page,'search_string'=>$search_string, 'filter'=>$filter);
    include('customers/cust_search_show_results.inc');
}
?>
<div class="bodyHD"><?=$lang->getLang("View Customers")?></div>
<div class="subnavmenu"><strong><?=$lang->getLang("Main Menu")?></strong></div>
<br />
<?php showErrors(); ?>
<form action="<?=SELF;?>" method="POST">
    <table cellpadding="4" cellspacing="0" border="0">
        <tr>
            <th colspan=2 class="heading"><?=$lang->getLang("Quick Search")?> &nbsp;&nbsp;|&nbsp;&nbsp;<a href="<?=refFormPage('customers/search_advanced')?>"><?=$lang->getLang("Advanced Search")?></a></th>
        </tr>
        <tr>
            <td colspan=2>&nbsp;</td>
        </tr>
        <tr valign=top>
            <td colspan=2 class=formheading><b><?=$lang->getLang("Quick search")?></td>
        </tr>
        <tr>
            <td class="label" ><?=$lang->getLang("Search type")?></td>
            <td>
                <select name="field">
                    <option value="0"<?= ($field == 0) ? ' selected' : '' ?>><?=$lang->getLang("Smart")?></option>
                    <option value="1"<?= ($field == 1) ? ' selected' : '' ?>><?=$lang->getLang("User")?></option>
                    <option value="2"<?= ($field == 2) ? ' selected' : '' ?>><?=$lang->getLang("Customer number")?></option>
                    <option value="3"<?= ($field == 3) ? ' selected' : '' ?>><?=$lang->getLang("Mail address")?></option>
                    <option value="4"<?= ($field == 4) ? ' selected' : '' ?>><?=$lang->getLang("Ip address")?></option>
                    <option value="6"<?= ($field == 6) ? ' selected' : '' ?>><?=$lang->getLang("Surname, Name")?></option>
                    <option value="7"<?= ($field == 7) ? ' selected' : '' ?>><?=$lang->getLang("Partner user id")?></option>
                    <?php if(DOT_IT) : ?>
                        <option value="5"<?= ($field == 5) ? ' selected' : '' ?>> <?=$lang->getLang("National Insurance number")?></option>
                    <?php endif; ?>
                </select>
            </td>
        </tr>
        <tr>
            <td class="label"><?=$lang->getLang("String Search")?>: </td>
            <td class="content"> <input name="search_string" type="text" value="<?=$search_string?>" /></td>
        </tr>
        <tr>
            <td class="label"><?=$lang->getLang("Exactly Result")?>: </td>
            <td class="content"> <input type="checkbox" name="exact" <?=(($exact)? "checked='true'" : "")?> ></td>
        </tr>
        <tr>
            <td class="label"><?=$lang->getLang("Records per page")?>: </td>
        <?php
        if ($_SESSION["jurisdiction_class"] != "club") {
            ?>
            <td class="content">
                <select name="max_rows">
                    <option value="10" <? if ( 10 == $max_rows ) echo 'selected'; ?>>10</option>
                    <option value="30" <? if ( 30 == $max_rows ) echo 'selected'; ?>>30</option>
                    <option value="60" <? if ( 60 == $max_rows ) echo 'selected'; ?>>60</option>
                    <option value="100" <? if ( 100 == $max_rows ) echo 'selected'; ?>>100</option>
                </select>
            </td>
            </tr>

                    <?php
                    include('jurisdiction_filters.php');
                    include('jurisdiction_filter_tabrow.php');
                    ?>

        <?php
        }
        ?>
        <tr>
            <td colspan="2">
                <input type="hidden" name="page" value="customers/search">
                <input type="hidden" name="start_search" value="yes">
                <input type="hidden" name="doQuery" value="yes">
                <input type="image" src="<?=image_dir?>/button_submit.gif">
            </td>
        </tr>
    </table>
</form>
<br/>
<?php


if($_SESSION['jur_id']==''){
    loginOnlyPage('index.html');
}
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
    if (!in_array($jur_id, $valid_jur_id) && $jur_id!='') {
        return false;
    }
    return true;
}
?>
