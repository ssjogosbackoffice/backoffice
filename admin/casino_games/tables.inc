<?php
check_access('poker_manage_tables', true);
globalize('action');

$tth_type      = $_REQUEST["tth_type"];
$limit         = $_REQUEST["tth_bettinglimit"];
$min_players   = $_REQUEST["min_players"];
$max_players   = $_REQUEST["max_players"];
$theme_id      = $_REQUEST["tth_id"];
$poker_variant = $_REQUEST["tth_variant"];
$tbl_id        = $_REQUEST["tbl_id"];
$game_id       = $_REQUEST["gam_id"];
$min_buyin     = $_REQUEST["min_buyin"];
$max_buyin     = $_REQUEST["max_buyin"];

if($tth_type == ""){
    $tth_type = -1;
}

if($limit == ""){
    $limit = -1;
}

if(empty($poker_variant)){
    $poker_variant = -1;
}

settype($tth_type,      "integer");
settype($limit,         "integer");
settype($min_players,   "integer");
settype($max_players,   "integer");
settype($theme_id,      "integer");
settype($poker_variant, "integer");
settype($tbl_id,        "integer");

if(empty($min_players)){
    $min_players = 2;
}

if(empty($max_players)){
    $max_players = 10;
}

$variants = array(1 => "Texas Hold'Em", 2 => "Omaha");

switch ($action){
    case "save":
        $save_action = (($tbl_id > 0)?("edit"):("save_new"));

        $_POST["tbl_themeid"] = $_POST["tth_id"];

        $valid_props = array(
            array("name" => "tbl_name",             "type" => "string",  "beautyname" => "Table Name",        "nullable" => false),
            array("name" => "tbl_password",         "type" => "string",  "beautyname" => "Table Password",    "nullable" => true),
            array("name" => "tbl_themeid",          "type" => "integer", "beautyname" => "Table Theme",       "nullable" => false),
            array("name" => "tbl_gam_id",           "type" => "string", "beautyname" => "Table Game",       "nullable" => false)
        );

        $properties = array();

        foreach ($valid_props as $prop){
            if($_POST[$prop["name"]] == "" && $_POST[$prop["name"]] != "0" && !$prop["nullable"]){
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
            showTableEdit($_POST);
            break;
        }

        $sql = "";

        if($save_action == "save_new"){
            $insert_columns = array();
            $insert_values  = array();

            $tbl_id = $dbh->nextSequence("tbl_id_seq");

            array_push($insert_columns, "tbl_id");
            array_push($insert_values,  $tbl_id);

            foreach ($properties as $key => $prop){
                array_push($insert_columns, $key);
                array_push($insert_values,  $prop);
            }

            $sql = "INSERT INTO tables (" . implode(",\n", $insert_columns) . ") VALUES (" . implode(",\n", $insert_values) . ")";
        }elseif($save_action == "edit"){

            $sql = "UPDATE tables SET ";

            $update_props = array();

            foreach ($properties as $key => $prop){
                array_push($update_props, $key . " = " . $prop);
            }

            $sql .= implode(", ", $update_props);

            $sql .= " WHERE tbl_id = " . $tbl_id;

        }

        $rs = $dbh->exec($sql);

        if($rs === 1){
            printInfoMessage($lang->getLang("Table successfully updated"));
        }else{
            addError("", $lang->getLang("Some error occurred updating theme").$sql);
            showErrors();
            exit();
        }

        header("Location: " . refFormPage($_GET["page"] . "&action=edit&tbl_id=" . $tbl_id));
        exit();
        break;
    case "new":
        showSearchBox();
        showTableEdit($table);
        showTableList();
        break;
    case "disable":
        $sql = "UPDATE tables SET tbl_stop = 1 WHERE tbl_id = $tbl_id";
        $rs = $dbh->exec($sql);
        if($rs == 1){
            printInfoMessage($lang->getLang("Table disabled"));
        }else{
            printInfoMessage($lang->getLang("Problem in disabling table"));
        }
        showSearchBox();
        showTableList();
        break;
    case "enable":
        $sql = "UPDATE tables SET tbl_stop = 0 WHERE tbl_id = $tbl_id";
        $rs = $dbh->exec($sql);
        if($rs == 1){
            printInfoMessage($lang->getLang("Table enabled"));
        }else{
            printInfoMessage($lang->getLang("Problem in enabling table"));
        }
        showSearchBox();
        showTableList();
        break;

    case "copy":
        $sql = "SELECT * FROM tables WHERE tbl_id = $tbl_id";
        $rs = $dbh->exec($sql);

        $table = $rs->next();
        $table["tbl_name"] = "Copy of " . $table["tbl_name"];

        showSearchBox();
        showTableEdit($table);
        showTableList();
        break;
        break;
    case "edit":
        $sql = "SELECT * FROM tables WHERE tbl_id = $tbl_id";
        $rs = $dbh->exec($sql);

        $table = $rs->next();
        $table["saved"] = true;

        showSearchBox();
        showTableEdit($table);
        //showTableList();
        break;
    default:
        showSearchBox();
        showTableList();
        break;
}

function showTableEdit($rs=null){
    global $lang;
    ?>
    <a href="<?=refFormPage($_GET["page"])?>"><?=$lang->getLang("Return to list")?></a>
    <div class=bodyHD>Edit Themes Details</div>
    <br/>
    <form action="<?=refFormPage($_GET["page"] . "&action=save")?>" method="POST">
    <?php if($rs["saved"]) : ?>
        <input type="hidden" name="tbl_id" value="<?=$rs["tbl_id"]?>" />
    <?php endif; ?>
    <table cellpadding=0 cellspacing="1" border=0 width="100%">
        <tr>
            <td>
                <table cellpadding="4" cellspacing="1" border=0 width="100%">
                    <tr>
                        <th colspan="2" class="formheading"><?=$lang->getLang("Table")?> <?=(($rs["saved"])?(" - #" . $rs["tbl_id"] . " " . $rs["tbl_name"]):(""))?></th>
                    </tr>
                    <tr valign="top">
                        <td class="label"><?=$lang->getLang("Name")?></td>
                        <td class="content" <? form_td_style('tbl_name'); ?>><input type="text" name="tbl_name" size="60" value="<?=replace_quotes($rs["tbl_name"])?>">
                            <?php echo err_field('tbl_name'); ?>
                        </td>
                    </tr>
                    <?php if(empty($rs["tbl_themeid"])) : ?>
                        <tr valign="top">
                            <td class="label"><?=$lang->getLang("Theme")?></td>
                            <td class="content" <? form_td_style('tbl_themeid'); ?>>
                                <?php printThemesSelect($rs["tbl_themeid"], false) ?>
                                <?php echo err_field('tbl_themeid'); ?>
                            </td>
                        </tr>
                    <?php else : ?>
                        <input type="hidden" name="tth_id" value="<?=$rs["tbl_themeid"]?>"/>
                    <?php endif; ?>
                    <tr valign="top">
                        <td class="label"><?=$lang->getLang("Game")?></td>
                        <td class="content" <? form_td_style('tbl_gam_id'); ?>>
                            <?php printGameSelect($rs["tbl_gam_id"], false) ?>
                            <?php echo err_field('tbl_gam_id'); ?>
                        </td>
                    </tr>
                    <tr valign="top">
                        <td class="label"><?=$lang->getLang("Password")?></td>
                        <td <? form_td_style('tbl_password'); ?>><input type="text" name="tbl_password" value="<?=replace_quotes($rs["tbl_password"])?>">
                            <?php echo err_field('tbl_password'); ?>
                            <?php if($lock): ?>
                                <input type="hidden" name="tth_type" value="<?=$rs["tth_type"]?>">
                            <?php endif; ?>
                        </td>
                    </tr>
                    <tr valign="top">
                        <td class="label"><?=$lang->getLang("Status")?></td>
                        <td class="content"><?=(($rs["tbl_stop"] == "0")?("<span style='color:Green;'>" . getLink(refFormPage($_GET["page"] . "&action=disable"), "Enabled") . "</span>"):("<span style='color:Red;'>" . getLink(refFormPage($_GET["page"] . "&action=enabled"), "Stopped") . "</span>"))?></td>
                    </tr>

                    <tr valign="top">
                        <td colspan="2" align="center"><input type="reset" <?if($ended){echo"disabled";}?>/>&nbsp;<input type="submit" <?if($ended){echo"disabled";}?>/></td>
                    </tr>

                </table>
            </td>
        </tr>
    </table>
<?php
}

function showSearchBox(){
    global $dbh, $tth_type, $limit, $min_players, $max_players, $poker_variant, $theme_id, $min_buyin, $max_buyin,$lang;
    ?>
    <div class="bodyHD"><?=$lang->getLang("Tables Manager")?></div>
    <div>
        <a href="<?=refFormPage($_GET["page"]) . "&action=new"?>"><?=$lang->getLang("New Table")?></a>
    </div>
    <br/><br/>
    <table cellpadding=4 cellspacing=1 border=0 width="100%">
        <?php form_head(refFormPage($_GET["page"]) . "&action=search", "search", "GET"); ?>
        <tr>
            <td class="formheading" colspan="4"><?=$lang->getLang("Search")?></td>
        </tr>
        <tr>
            <td class="label"><?=$lang->getLang("Type")?>:</td><td class="content"><?= createThemeTypeSelect($tth_type); ?></td>
            <td class="label"><?=$lang->getLang("Min Players")?>:</td><td class="content"><?php printPlayersSelect("min_players", $min_players) ?></td>
        </tr>
        <tr>
            <td class="label"><?=$lang->getLang("Limit")?>:</td><td class="content"><? printLimitTypeSelect($limit); ?></td>
            <td class="label"><?=$lang->getLang("Max Players")?>:</td><td class="content"><?php printPlayersSelect("max_players", $max_players) ?></td>
        </tr>
        <tr>
            <td class="label"><?=$lang->getLang("Variant")?>:</td><td class="content"><?php printVariantSelect($poker_variant); ?></td>
            <td class="label"><?=$lang->getLang("Theme")?>:</td><td class="content"><?php printThemesSelect($theme_id) ?></td>
        </tr>
        <tr>
            <td class="label"><?=$lang->getLang("Min Buyin")?>:</td>
            <td class="content">
                <select name="min_buyin">
                    <?php
                    printOption("", "-1", "All", (-1 == $min_buyin));
                    $i = 5;
                    while($i <= 50000){
                        $add = 5;
                        if($i >= 50){
                            $add = 10;
                        }
                        if($i >= 100){
                            $add = 50;
                        }
                        if($i >= 1000){
                            $add = 500;
                        }
                        if($i >= 10000){
                            $add = 2000;
                        }
                        printOption("", $i, getInDollars($i / 100) . " / " . getInDollars($i / 100 * 2), ($min_buyin == $i));
                        $i += $add;
                    }
                    ?>
                </select>
            </td>
            <td class="label"><?=$lang->getLang("Max Buyin")?>:</td>
            <td class="content">
                <select name="max_buyin">
                    <?php
                    printOption("", "-1", "All", (-1 == $max_buyin));
                    $i = 5;
                    while($i <= 50000){
                        $add = 5;
                        if($i >= 50){
                            $add = 10;
                        }
                        if($i >= 100){
                            $add = 50;
                        }
                        if($i >= 1000){
                            $add = 500;
                        }
                        if($i >= 10000){
                            $add = 2000;
                        }
                        printOption("", $i, getInDollars($i / 100) . " / " . getInDollars($i / 100 * 2), ($max_buyin == $i));
                        $i += $add;
                    }
                    ?>
                </select>
            </td>
        </tr>
        <tr>
            <td><input type="submit" name="submit" value="<?=$lang->getLang("Search")?>"/></td>
        </tr>
    </form>
  </table>
  <?php
  /* include(include_content("poker/hosts.inc")); */
}

function showTableList(){
    global $dbh, $limit, $min_players, $max_players, $theme_id, $poker_variant, $tth_type, $min_buyin, $max_buyin;


    $sql = "SELECT
            tbl_id, tbl_name, tbl_password, tth_maxplayersnum,gam_name,
            tth_id, tth_type, tth_name, tth_bettinglimit, tth_smallblind, tth_bigblind, tth_variant, ttt_name, tbl_stop
          FROM tables
          JOIN tables_themes ON tth_id = tbl_themeid
          JOIN game ON gam_id = tbl_gam_id
          JOIN tables_themes_type ON ttt_id = tth_type
         ";

    $sql .= " WHERE 1 = 1 ";

    if($limit > -1){
        $sql .= " AND tth_bettinglimit = $limit";
    }

    if($tth_type > -1){
        $sql .= " AND tth_type = $tth_type";
    }

    if($poker_variant > -1){
        $sql .= " AND tth_variant = " . $poker_variant;
    }

    if($theme_id > 0){
        $sql .= " AND tth_id = " . $theme_id;
    }

    if($min_buyin > -1){
        $sql .= " AND tth_smallblind >= " . ($min_buyin / 100);
    }

    if($max_buyin > -1){
        $sql .= " AND tth_smallblind <= " . ($max_buyin / 100);
    }

    if($min_buyin > $max_buyin){
        addError("", "Min buyin too big");
    }

    $sql .= " AND (tth_maxplayersnum >= $min_players AND tth_maxplayersnum <= $max_players) ";

    $sql .= " ORDER BY tbl_password DESC, ttt_name, tth_bettinglimit ASC, tth_smallblind ASC, tth_maxplayersnum ASC, tbl_name";

    //print $sql;

    if(!areErrors()){

        $rs = $dbh->exec($sql);

        $num_rows = $rs->getNumRows();

        if($num_rows != 0){
            $cols_arr = array(
                'ID',
                'Name',
                'Type',
                'Theme',
                'Game',
                'Players',
                'Limit',
                //'Host',
                'Password',
                'Active',
                'Edit',
                'Copy'
            );

            $val_fmt_arr = array (
                'ID'               => 'return $rec["tbl_id"];',
                'Name'             => 'return getClassLink(refFormPage($_GET["page"] . "&action=edit&tbl_id="   . $rec["tbl_id"]), $rec["tbl_name"]);',
                'Type'             => 'return getVariantName($rec["tth_variant"]) . " - " . $rec["ttt_name"];',
                'Theme'            => 'return getLink(refFormPage("casino_games/themes&action=edit&tth_id=" . $rec["tth_id"]), str_pad($rec["tth_id"], 3, "0", STR_PAD_LEFT) . " - " . $rec["tth_name"], "", "_blank");',
                'Game'             => 'return $rec["gam_name"];',
                'Players'          => 'return $rec["tth_maxplayersnum"];',
                'Limit'            => 'return getLimitName($rec["tth_bettinglimit"]);',

                //'Buy In'           => 'return getAutoMoney($rec["tth_minbuyin"], $rec["tth_type"]) . " / " . getAutoMoney($rec["tth_maxbuyin"], $rec["tth_type"]);',
                ////getHostIp($rec["hst_string"]);
                //'Host'             => 'return "<span class=\"host_" . $rec["hst_id"] . "\">" . $rec["hst_name"] . "</span>";',
                'Password'         => 'return getPassword($rec["tbl_password"]);',
                'Active'           => 'return getIsActive($rec["tbl_stop"], $rec["tbl_id"]);',
                'Edit'             => 'return getClassLink(refFormPage($_GET["page"] . "&action=edit&tbl_id="   . $rec["tbl_id"]), "Edit");',
                'Copy'             => 'return getClassLink(refFormPage($_GET["page"] . "&action=copy&tbl_id="   . $rec["tbl_id"]), "Copy");'
            );

            ?>
            <style>
                a.red{
                    color:Red;
                }
                a.green{
                    color:Green;
                }
            </style>
            <?php

            $qp = new QueryPrinter($rs);
            $qp->printTable($rs, $cols_arr, array(), $val_fmt_arr, $cell_fmt_arr, $sum_fmt_arr = "", $excel_filename = "", "", "", 1, $num_rows, false, "themesearch");
        }else{
            addError("", $lang->getLang("No result found"));
            showErrors();
            clearErrors();
            return false;
        }

    }

}

function getPassword($password){
    global $lang;
    if(empty($password)){
        $password = "<span style='color:Green;'>".$lang->getLang("Free")."</span>";
    }else{
        $password = "<span style='color:Red;'>$password</span>";
    }
    return $password;
}

function getIsActive($active, $tbl_id){
    $status = array(
        1 => getClassLink(refFormPage($_GET["page"] . "&action=enable&tbl_id=$tbl_id"),  "Disabled", "red"),  //"<span style='color:Green;'>Running</span>",
        0 => getClassLink(refFormPage($_GET["page"] . "&action=disable&tbl_id=$tbl_id"), "Enabled" , "green") //"<span style='color:Red;'>Stopped</span>"
    );
    return $status[$active];
}

function getAutoNumber($number, $theme_type){
    $ret;
    if($theme_type == 1){
        $ret = number_format($number * 100, 0, ".", "");
    }else{
        $ret = number_format($number, 2, ".", "");
    }
    return $ret;
}

function getAutoMoney($money, $theme_type){
    $ret;
    if($theme_type == 1){
        $ret = getInFunTokens($money * 100, 0, ".", "");
    }else{
        $ret = getInDollars($money);
    }
    return $ret;
}

function getHostIp($host){
    $ret = $host;
    if(eregi("^([0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}:[0-9]{0,5}).*?$", $host, $match)){
        $ret = $match[1];
    }
    return $ret;
}

function getVariantName($variant){
    global $variants;
    return $variants[$variant];
}

function getLimitName($limit){
    $limits = array(
        1 => "Limit",
        2 => "Pot Limit",
        3 => "No Limit",
        4 => "Fixed Limit",
        5 => "Spread Limit"
    );
    return $limits[$limit];
}

function createThemeTypeSelect($default = null, $accept_all=true, $disabled=false) {
    global $dbh, $tth_type;
    $sql = "SELECT ttt_id, ttt_name FROM tables_themes_type WHERE ttt_id IN (0, 1, 7) ORDER BY ttt_name" ;
    $rs   = $dbh->exec($sql);
    ?>
    <select name="tth_type" id="tth_type" <?=(($disabled)?("disabled"):(""))?>>
        <?php
        if($accept_all){
            printOption("tth_type", -1, "All", ($default == -1));
        }

        while($rs->hasNext()) {
            $row = $rs->next();
            printOption("tth_type", $row['ttt_id'], $row['ttt_name'], ($default == $row['ttt_id']));
        }
        ?>
    </select>
<?php
}

function printLimitTypeSelect($defult = null, $accept_all = true, $disabled=false){
    ?>
    <select name="tth_bettinglimit" <?=(($disabled)?("disabled"):(""))?>>
        <?php
        $limits;
        if($accept_all){
            $limits = array(-1 => "All", 1 => "Limit", 2 => "Pot Limit", 3 => "No Limit", 4 => "Fixed Limit", 5 => "Spread Limit");
        }else{
            $limits = array(1 => "Limit", 2 => "Pot Limit", 3 => "No Limit", 4 => "Fixed Limit", 5 => "Spread Limit");
        }


        foreach ($limits as $key => $limit){
            printOption("tth_bettinglimit", $key, $limit, ($defult == $key));
        }
        ?>
    </select>
<?php
}

function printPlayersSelect($name, $default = null){
    ?>
    <select name="<?=$name?>">
        <?php
        $players = array(2, 3, 4, 5, 6, 7, 8, 9, 10);
        foreach ($players as $i){
            printOption($name, $i, "$i players", ($i == $default));
        }
        ?>
    </select>
<?php
}

function printHostSelect($default){
    global $dbh;
    $sql = "SELECT gam_id, gam_name, gam_host_string FROM game ORDER BY gam_id";
    $rs  = $dbh->exec($sql);
    while ($rs->hasNext()){
        $host = $rs->next();
        ?>
        <input type="radio" name="tth_roomid" value="<?=$host["gam_id"]?>" <?=(($host["gam_id"] == $default)?("checked"):(""))?>><?=$host["gam_name"]?></br>
    <?php
    }
}

function printThemesSelect($default, $showAll=true){
    global $dbh;
    $sql = "SELECT tth_id, tth_name FROM tables_themes WHERE tth_type IN (0, 1, 7) ORDER BY LOWER(tth_name)";
    $rs  = $dbh->exec($sql);
    ?>
    <select name="tth_id">
        <?php
        if($showAll){
            printOption("tth_id", "-1", "All", ($default == -1));
        }
        while ($rs->hasNext()){
            $row = $rs->next();
            printOption("tth_id", $row["tth_id"], str_pad($row["tth_id"], 3, "0", STR_PAD_LEFT) . " - " . $row["tth_name"], ($row["tth_id"] == $default));
        }
        ?>
    </select>
<?php
}


function printGameSelect($default, $showAll=true){
    global $dbh;
    $sql = "SELECT gam_id, gam_name FROM game WHERE gam_category IN ('casino','casino_live' ,'provide_poker_live','provider_gaminator','casino_agency') AND gam_type='multi_table'";
    $rs  = $dbh->exec($sql);
    ?>
    <select name="tbl_gam_id">
        <?php
        while ($rs->hasNext()){
            $row = $rs->next();
            printOption("gam_id", $row["gam_id"], str_pad($row["gam_name"], 3, "0", STR_PAD_LEFT) , ($row["gam_id"] == $default));
        }
        ?>
    </select>
<?php
}



function printVariantSelect($default, $accept_all = true){
    global $variants;
    ?>
    <select name="tth_variant">
        <?php
        if($accept_all){
            printOption("tth_variant", -1, "All", ($default == $key));
        }
        foreach ($variants as $key => $variant){
            printOption("tth_variant", $key, $variant, ($default == $key));
        }
        ?>
    </select>
<?php
}
?>