<?php
check_access('poker_manage_themes', $redirect=true);
$action        = globalize("action");
$tth_type      = $_REQUEST["tth_type"];

$limit         = $_REQUEST["tth_bettinglimit"];
$min_players   = $_REQUEST["min_players"];
$max_players   = $_REQUEST["max_players"];
$theme_id      = $_REQUEST["tth_id"];
$poker_variant = $_REQUEST["tth_variant"];
$selected_host = $_REQUEST["tth_hstid"];

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
settype($selected_host, "integer");


if(empty($min_players)){
    $min_players = 2;
}

if(empty($max_players)){
    $max_players = 10;
}

$variants = array(1 => "Texas Hold'Em", 2 => "Omaha");

switch ($action){
    case "save":
        $away_status = $_POST["away_status"];

        $save_action = (($theme_id > 0)?("edit"):("save_new"));

        $_POST["tth_maxawayhands"]  = "0";
        $_POST["tth_maxawayrounds"] = "0";

        if($_POST["away_status"] == "hands"){
            $_POST["tth_maxawayhands"]  = $_POST["tth_away_int"];
        }else{
            $_POST["tth_maxawayrounds"] = $_POST["tth_away_int"];
        }

        $mult = (($_POST["tth_type"] == 1)?(100):(1));

        $_POST["tth_minstake"]   = $_POST["tth_minstake"] / $mult;
        $_POST["tth_maxstake"]   = $_POST["tth_maxstake"] / $mult;
        $_POST["tth_smallblind"] = $_POST["tth_smallblind"] / $mult;
        $_POST["tth_bigblind"]   = $_POST["tth_bigblind"] / $mult;
        $_POST["tth_minbuyin"]   = $_POST["tth_minbuyin"] / $mult;
        $_POST["tth_maxbuyin"]   = $_POST["tth_maxbuyin"] / $mult;

        $valid_props = array(
            array("name" => "tth_name",             "type" => "string",  "beautyname" => "Name",              "nullable" => true),
            array("name" => "tth_bettinglimit",     "type" => "integer", "beautyname" => "Betting Limit",     "nullable" => true),
            array("name" => "tth_maxawayhands",     "type" => "integer", "beautyname" => "Max Away Hands",    "nullable" => true),
            array("name" => "tth_maxawayrounds",    "type" => "integer", "beautyname" => "Max Away Rounds",   "nullable" => true),
            array("name" => "tth_minplayersnum",    "type" => "integer", "beautyname" => "Min Players",       "nullable" => false),
            array("name" => "tth_maxplayersnum",    "type" => "integer", "beautyname" => "Max Players",       "nullable" => false),
            array("name" => "tth_uniqueip",         "type" => "integer", "beautyname" => "Unique IP",         "nullable" => false),
            array("name" => "tth_webcammode",       "type" => "integer", "beautyname" => "Webcam Mode",       "nullable" => false),
            array("name" => "tth_hstid",            "type" => "integer", "beautyname" => "Game Host",         "nullable" => false),

            array("name" => "tth_variant",          "type" => "integer", "beautyname" => "Poker Variant",     "nullable" => false),
            array("name" => "tth_bettinglimit",     "type" => "integer", "beautyname" => "Betting Limit",     "nullable" => false),

            array("name" => "tth_minstake",         "type" => "decimal", "beautyname" => "Minimum Stake",     "nullable" => true),
            array("name" => "tth_maxstake",         "type" => "decimal", "beautyname" => "Maximum Stake",     "nullable" => true),
            array("name" => "tth_maxraisenum",      "type" => "integer", "beautyname" => "Max Raise Allowed", "nullable" => true),
            array("name" => "tth_rakepercent",      "type" => "integer", "beautyname" => "Rake Percentual",   "nullable" => true),
            array("name" => "tth_rakelimit",        "type" => "decimal", "beautyname" => "Rake Limit",        "nullable" => true),

            array("name" => "tth_smallblind",       "type" => "decimal", "beautyname" => "Small Blind",       "nullable" => true),
            array("name" => "tth_bigblind",         "type" => "decimal", "beautyname" => "Big Blind",         "nullable" => true),

            array("name" => "tth_minbuyin",         "type" => "decimal", "beautyname" => "Min Buyin",         "nullable" => true),
            array("name" => "tth_maxbuyin",         "type" => "decimal", "beautyname" => "Max Buyin",         "nullable" => true),

            array("name" => "tth_movetimeout",      "type" => "integer", "beautyname" => "Move Timeout",      "nullable" => false),
            array("name" => "tth_timebank",         "type" => "integer", "beautyname" => "Timebank",          "nullable" => false),
            array("name" => "tth_type",             "type" => "integer", "beautyname" => "Theme Type",        "nullable" => false)
        );

        $properties = array();


        foreach ($valid_props as $prop){
            if($_POST[$prop["name"]] == "" && $_POST[$prop["name"]] != "0"){
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
            showThemesEditor($_POST);
            break;
        }

        $sql = "";

        if($save_action == "save_new"){
            $insert_columns = array();
            $insert_values  = array();

            $theme_id = $dbh->nextSequence("tth_id_seq");

            array_push($insert_columns, "tth_id");
            array_push($insert_values,  $theme_id);

            foreach ($properties as $key => $prop){
                array_push($insert_columns, $key);
                array_push($insert_values,  $prop);
            }

            $sql = "INSERT INTO tables_themes (" . implode(",\n", $insert_columns) . ") VALUES (" . implode(",\n", $insert_values) . ")";
        }elseif($save_action == "edit"){

            $sql = "UPDATE tables_themes SET ";

            $update_props = array();

            foreach ($properties as $key => $prop){
                array_push($update_props, $key . " = " . $prop);
            }

            $sql .= implode(", ", $update_props);

            $sql .= " WHERE tth_id = " . $theme_id;

        }


        $rs = $dbh->exec($sql);

        if($rs === 1){
            printInfoMessage("Theme updated successfull!");
        }else{
            addError("", "Some error occurred updating theme <!--$sql-->");
            showErrors();
            exit();
        }

        header("Location: " . refFormPage($_GET["page"] . "&action=edit&tth_id=" . $theme_id));
        exit();
        break;
    case "new":
        showThemesEditor();
        break;
    case "copy":
        $sql = "SELECT * FROM tables_themes WHERE tth_id = $theme_id";
        $rs = $dbh->exec($sql);
        $theme = $rs->next();
        showThemesEditor($theme);
        break;
    case "edit":
        $sql = "SELECT * FROM tables_themes WHERE tth_id = $theme_id";
        $rs = $dbh->exec($sql);
        $theme = $rs->next();
        $theme["saved"] = true;
        showThemesEditor($theme);
        break;
    default:
        printSearchBox();
        getThemeList();
        break;
}



function getThemeList(){
    global $dbh, $tth_type, $limit, $min_players, $max_players, $poker_variant, $selected_host;

    $sql = "SELECT
            tth_name, ttt_name, tth_minplayersnum, tth_maxplayersnum, tth_bettinglimit, tth_smallblind, tth_status,
            tth_bigblind, tth_minbuyin, tth_maxbuyin, tth_id, hst_id, hst_name, tth_type, tth_variant, COUNT(tbl_id) AS tables
          FROM
            tables_themes
          JOIN tables_themes_type ON ttt_id = tth_type
          JOIN game_host ON hst_id = tth_hstid
          LEFT JOIN tables ON (tbl_themeid = tth_id)
          WHERE 1 = 1";

    if($tth_type > -1){
        $sql .= " AND tth_type = $tth_type";
    }

    if($limit > -1){
        $sql .= " AND tth_bettinglimit = $limit";
    }

    if($poker_variant > -1){
        $sql .= " AND tth_variant = " . $poker_variant;
    }

    if($selected_host > 0){
        $sql .= " AND tth_hstid = $selected_host";
    }

    $sql .= " AND (tth_maxplayersnum >= $min_players AND tth_maxplayersnum <= $max_players)";

    $sql .= " GROUP BY tth_name, ttt_name, tth_minplayersnum, tth_maxplayersnum, tth_bettinglimit, tth_smallblind, tth_status,
            tth_bigblind, tth_minbuyin, tth_maxbuyin, tth_id, hst_id, hst_name, tth_type, tth_variant";

    $sql .= " ORDER BY ttt_name, tth_bettinglimit ASC, tth_smallblind ASC, tth_minplayersnum ASC";

    $rs       = $dbh->exec($sql);
    $num_rows = $rs->getNumRows();

    if($num_rows!=0){

        $cols_arr = array(
            'ID',
            'Name',
            'Active Tables',
            'Type',
            'Players',
            'Limit',
            'Blinds',
            'Buy In',
            'Host',
            'Active',
            'Edit',
            'Copy'
        );

        $val_fmt_arr = array (
            'ID'               => 'return $rec["tth_id"];',
            'Name'             => 'return getClassLink(refFormPage($_GET["page"] . "&action=edit&tth_id="   . $rec["tth_id"]), $rec["tth_name"]);',
            'Active Tables'    => 'return (($rec["tables"] > 0)?(getClassLink(refFormPage("poker/tables&tth_id=" . $rec["tth_id"]), "View " . $rec["tables"] . " tables")):("<span style=\"color:#CCC\">No cash tables</span>"));',
            'Type'             => 'return getVariantName($rec["tth_variant"]) . " - " . $rec["ttt_name"];',
            'Players'          => 'return $rec["tth_maxplayersnum"];',
            'Limit'            => 'return getLimitName($rec["tth_bettinglimit"]);',
            'Blinds'           => 'return getAutoMoney($rec["tth_smallblind"], $rec["tth_type"]) . " / " . getAutoMoney($rec["tth_bigblind"], $rec["tth_type"]);',
            'Buy In'           => 'return getAutoMoney($rec["tth_minbuyin"], $rec["tth_type"]) . " / " . getAutoMoney($rec["tth_maxbuyin"], $rec["tth_type"]);',
            'Host'             => 'return "<span class=\"host_" . $rec["hst_id"] . "\">" . $rec["hst_name"] . "</span>";',
            'Active'           => 'return (($rec["tth_status"] == "1")?("<span style=\'color:Green;\'>Enabled</span>"):("<span style=\'color:Red;\'>Disabled</span>"));',
            'Edit'             => 'return getClassLink(refFormPage($_GET["page"] . "&action=edit&tth_id="   . $rec["tth_id"]), "Edit");',
            'Copy'             => 'return getClassLink(refFormPage($_GET["page"] . "&action=copy&tth_id="   . $rec["tth_id"]), "Copy");'
        );

        $qp = new QueryPrinter($rs);
        $qp->printTable($rs, $cols_arr, array(), $val_fmt_arr, $cell_fmt_arr, $sum_fmt_arr = "", $excel_filename = "", "", "", 1, $num_rows, false, "themesearch");
    }else{
        addError("", "Nessun Risultato Trovato ");
        showErrors();
        clearErrors();
        return false;
    }
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
    if(eregi("^([0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}:[0-9]{,5}).*;?", $host, $match)){
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
    $sql = "SELECT ttt_id, ttt_name FROM tables_themes_type ORDER BY ttt_name" ;
    $dbh->doQuery($sql);
    $rs   = $dbh->getResults();
    ?>
    <select name="tth_type" id="tth_type" <?=(($disabled)?("disabled"):(""))?>>
        <?php
        if($accept_all){
            printOption("tth_type", -1, "All", ($default == -1));
        }
        while($row = $rs->fetchRow()) {
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

function printHostSelect($default, $all=false){
    global $dbh;
    $sql = "SELECT hst_id, hst_name, hst_string FROM game_host ORDER BY hst_id";
    $rs  = $dbh->exec($sql);
    if($all){
        ?>
        <input type="radio" name="tth_hstid" value="0" <?=((empty($default))?("checked"):(""))?>>All</br>
    <?php
    }

    if(empty($default)){
        $default = 1;
    }

    while ($rs->hasNext()){
        $host = $rs->next();
        ?>
        <input type="radio" name="tth_hstid" value="<?=$host["hst_id"]?>" <?=(($host["hst_id"] == $default)?("checked"):(""))?>><span class="host_<?=$host["hst_id"]?>"><?=$host["hst_name"]?></span></br>
    <?php
    }
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

function printSearchBox(){
    global $dbh, $tth_type, $limit, $min_players, $max_players, $poker_variant, $selected_host;
    ?>
    <div class="bodyHD">Themes Manager</div>
    <div>
        <a href="<?=refFormPage($_GET["page"]) . "&action=new"?>">New Theme</a>
    </div>
    <br/><br/>
    <table cellpadding=4 cellspacing=1 border=0 width="100%">
        <?php form_head(refFormPage($_GET["page"]) . "&action=search", "search", "GET"); ?>
        <tr>
            <td class="formheading" colspan="4">Search</td>
        </tr>
        <tr>
            <td class="label">Type:</td><td class="content"><?= createThemeTypeSelect($tth_type); ?></td>
            <td class="label">Min Players:</td><td class="content"><?php printPlayersSelect("min_players", $min_players) ?></td>
        </tr>
        <tr>
            <td class="label">Limit:</td><td class="content"><? printLimitTypeSelect($limit); ?></td>
            <td class="label">Max Players:</td><td class="content"><?php printPlayersSelect("max_players", $max_players) ?></td>
        </tr>
        <tr>
            <td class="label">Variant:</td><td class="content"><?php printVariantSelect($poker_variant); ?></td>
            <td class="label" valign="top">Host:</td><td class="content"><?php printHostSelect($selected_host, true); ?></td>
        </tr>
        <tr>
            <td><input type="submit" name="submit" value="search"/></td>
        </tr>
        </form>
    </table>
    <?php
    include(include_content("poker/hosts.inc"));
}

function showThemesEditor($rs=null){
    global $dbh,$rebuy_options;

    $lock      = (($rs["saved"])?("disabled"):(""));
    $awaymode  = (($rs["tth_maxawayhands"] > 0)?("hands"):("rounds"));
    $rs["tth_away_int"] = (($awaymode == "hands")?($rs["tth_maxawayhands"]):($rs["tth_maxawayrounds"]));
    ?>
    <a href="<?=refFormPage($_GET["page"])?>">Return to list</a>
    <div class=bodyHD>Edit Themes Details</div>
    <br/>
    <form action="<?=refFormPage($_GET["page"] . "&action=save")?>" method="POST">
    <?php if($rs["saved"]) : ?>
        <input type="hidden" name="tth_id" value="<?=$rs["tth_id"]?>">
    <?php endif; ?>
    <table cellpadding=0 cellspacing="1" border=0 width="100%">
    <tr>
    <td>
    <table cellpadding="4" cellspacing="1" border=0>
    <tr cellpadding="4">
        <th colspan="4" class="formheading">Info <?=(($rs["saved"])?(" - #" . $rs["tth_id"] . " " . $rs["tth_name"]):(""))?></th>
    </tr>
    <tr valign="top">
        <td class="label">Name</td>
        <td colspan="3" <? form_td_style('tth_name'); ?>><input size="80" type="text" name="tth_name" value="<?=replace_quotes($rs["tth_name"])?>">
            <?php echo err_field('tth_name'); ?>
        </td>
    </tr>

    <tr valign="top">
        <td class="label">Type</td>
        <td <? form_td_style('tth_type'); ?>><?php createThemeTypeSelect($rs["tth_type"], false, $lock) ?>
            <?php echo err_field('tth_type'); ?>
            <?php if($lock): ?>
                <input type="hidden" name="tth_type" value="<?=$rs["tth_type"]?>">
            <?php endif; ?>
        </td>
        <td class="label">Betting Limit</td>
        <td <? form_td_style('tth_bettinglimit'); ?>><?php printLimitTypeSelect($rs["tth_bettinglimit"], false) ?>
            <?php echo err_field('tth_bettinglimit'); ?>
        </td>
    </tr>

    <tr valign="top">
        <td class="label">Poker Variant</td>
        <td <? form_td_style('tth_maxplayersnum'); ?>>
            <?php
            printVariantSelect($rs["tth_variant"], false);
            ?>
            <?php echo err_field('tth_variant'); ?>
        </td>
        <td class="label">Away Rules</td>
        <td class="content" <? form_td_style('away_status'); ?>>
            <select name="away_status">
                <?php printOption("away_status", "hands",  "Max Away Hands",  ($awaymode == "hands"))?>
                <?php printOption("away_status", "rounds", "Max Away Rounds", ($awaymode == "rounds"))?>
            </select>
            <?php echo err_field('away_status'); ?>
            -
            <select name="tth_away_int">
                <?php
                for($i = 1 ; $i <= 10 ; $i++){
                    printOption("tth_away_int", $i, $i, ($i == $rs["tth_away_int"]));
                }
                ?>
            </select>
            <?php echo err_field('tth_away_int'); ?>
        </td>
    </tr>

    <tr valign="top">
        <td class="label">Min Players</td>
        <td <? form_td_style('tth_minplayersnum'); ?>><?php printPlayersSelect("tth_minplayersnum", $rs["tth_minplayersnum"]); ?>
            <?php echo err_field('tth_minplayersnum'); ?>
        </td>
        <td class="label">Max Players</td>
        <td <? form_td_style('tth_maxplayersnum'); ?>><?php printPlayersSelect("tth_maxplayersnum", $rs["tth_maxplayersnum"]); ?>
            <?php echo err_field('tth_maxplayersnum'); ?>
        </td>
    </tr>

    <tr valign="top">
        <td class="label">Unique IP</td>
        <td <? form_td_style('tth_uniqueip'); ?>>
            <select name="tth_uniqueip">
                <?php
                printOption("tth_uniqueip", 1, "Yes", ($rs["tth_uniqueip"] == "1"));
                printOption("tth_uniqueip", 0, "No",  ($rs["tth_uniqueip"] == "0"));
                ?>
            </select>
            <?php echo err_field('tth_uniqueip'); ?>
        </td>
        <td class="label">Webcam</td>
        <td <? form_td_style('tth_webcammode'); ?>>
            <select name="tth_webcammode">
                <?php
                printOption("tth_webcammode", 1, "Yes", ($rs["tth_webcammode"] == 1));
                printOption("tth_webcammode", 0, "No",  ($rs["tth_webcammode"] == 0));
                ?>
            </select>
            <?php echo err_field('tth_webcammode'); ?>
        </td>
    </tr>

    <tr valign="top">
        <td class="label">Host</td>
        <td <? form_td_style('tth_hstid'); ?>>
            <?printHostSelect($rs["tth_hstid"])?>
        </td>
    </tr>

    <tr cellpadding="4">
        <th colspan="4" class="formheading">Limit Info - Leave blank for no-limit themes</th>
    </tr>

    <tr valign="top">
        <td class="label">Min Stake</td>
        <td <? form_td_style('tth_minstake'); ?>>
            <input type="text" name="tth_minstake" value="<?=getAutoNumber(replace_quotes($rs["tth_minstake"]), $rs["tth_type"])?>">
            <?php echo err_field('tth_minstake'); ?>
        </td>
        <td class="label">Max Stake</td>
        <td <? form_td_style('tth_maxstake'); ?>>
            <input type="text" name="tth_maxstake" value="<?=getAutoNumber(replace_quotes($rs["tth_maxstake"]), $rs["tth_type"])?>">
            <?php echo err_field('tth_maxstake'); ?>
        </td>
    </tr>

    <tr valign="top">
        <td class="label">Max Raise Allowed</td>
        <td <? form_td_style('tth_maxraisenum'); ?>>
            <select name="tth_maxraisenum">
                <?php
                for($i = 1 ; $i <= 10 ; $i++){
                    printOption("tth_maxraisenum", $i, $i, ($i == $rs["tth_maxraisenum"]));
                }
                ?>
            </select>
            <?php echo err_field('tth_maxraisenum'); ?>
        </td>
    </tr>

    <tr cellpadding="4">
        <th colspan="4" class="formheading">Rake</th>
    </tr>

    <tr valign="top">
        <td class="label">Rake Percentual</td>
        <td <? form_td_style('tth_rakepercent'); ?>>
            <select name="tth_rakepercent">
                <?php
                for($i = 0 ; $i <= 20 ; $i++){
                    printOption("tth_rakepercent", $i, $i . "%", ($i == $rs["tth_rakepercent"]));
                }
                ?>
            </select>
            <?php echo err_field('tth_rakepercent'); ?>
        </td>
        <td class="label">Rake Cap</td>
        <td <? form_td_style('tth_rakelimit'); ?>>
            <select name="tth_rakelimit">
                <?php
                for($i = 0 ; $i <= 10 ; $i++){
                    printOption("tth_rakelimit", $i, getInDollars($i), ($i == $rs["tth_rakelimit"]));
                }
                for($i = 12 ; $i <= 50 ; $i += 2){
                    printOption("tth_rakelimit", $i, getInDollars($i), ($i == $rs["tth_rakelimit"]));
                }
                for($i = 60 ; $i <= 200 ; $i += 10){
                    printOption("tth_rakelimit", $i, getInDollars($i), ($i == $rs["tth_rakelimit"]));
                }
                ?>
            </select>
            <?php echo err_field('tth_rakelimit'); ?>
        </td>
    </tr>

    <tr cellpadding="4">
        <th colspan="4" class="formheading">Game Config</th>
    </tr>

    <tr valign="top">
        <td class="label">Small Blind</td>
        <td <? form_td_style('tth_smallblind'); ?>><input type="text" name="tth_smallblind" value="<?=getAutoNumber(replace_quotes($rs["tth_smallblind"]), $rs["tth_type"])?>">
            <?php echo err_field('tth_smallblind'); ?>
        </td>
        <td class="label">Big Blind</td>
        <td <? form_td_style('tth_bigblind'); ?>><input type="text" name="tth_bigblind" value="<?=getAutoNumber(replace_quotes($rs["tth_bigblind"]), $rs["tth_type"])?>">
            <?php echo err_field('tth_bigblind'); ?>
        </td>
    </tr>

    <tr valign="top">
        <td class="label">Minimum Buyin</td>
        <td <? form_td_style('tth_minbuyin'); ?>><input type="text" name="tth_minbuyin" value="<?=getAutoNumber(($rs["tth_minbuyin"]), $rs["tth_type"])?>">
            <?php echo err_field('tth_minbuyin'); ?>
        </td>
        <td class="label">Maximum Buyin</td>
        <td <? form_td_style('tth_maxbuyin'); ?>><input type="text" name="tth_maxbuyin" value="<?=getAutoNumber(replace_quotes($rs["tth_maxbuyin"]), $rs["tth_type"])?>">
            <?php echo err_field('tth_maxbuyin'); ?>
        </td>
    </tr>

    <tr cellpadding="4">
        <th colspan="4" class="formheading">Times</th>
    </tr>

    <tr valign="top">
        <td class="label">Move Timeout</td>
        <td <? form_td_style('tth_movetimeout'); ?>>
            <select name="tth_movetimeout">
                <?php
                for($i = 2 ; $i <= 60 ; $i +=2){
                    printOption("tth_movetimeout", $i * 1000, $i . " sec.", ($i == $rs["tth_movetimeout"]/1000));
                }
                ?>
            </select>
            <?php echo err_field('tth_movetimeout'); ?>
        </td>
        <td class="label">Timebank</td>
        <td <? form_td_style('tth_timebank'); ?>>
            <select name="tth_timebank">
                <?php
                for($i = 0 ; $i <= 120 ; $i += 10){
                    printOption("tth_timebank", $i * 1000, $i . " sec.", ($i == $rs["tth_timebank"]/1000));
                }
                ?>
            </select>
            <?php echo err_field('tth_timebank'); ?>
        </td>
    </tr>
    <tr valign="top">
        <td colspan="2" align="center"><input type="reset" <?if($ended){echo"disabled";}?>/>&nbsp;<input type="submit" <?if($ended){echo"disabled";}?>/></td>
    </tr>
    </table>
    </td>
    </tr>
    </table>
    </form>
<?php
}
?>
