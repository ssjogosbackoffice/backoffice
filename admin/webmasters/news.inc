<?php
PutEnv("NLS_LANG=AMERICAN_AMERICA.WE8ISO8859P1");
//include_once 'include/sites.inc.php';
check_access('manage_news', true);
global $lang;

$sql = "SELECT skn_id, skn_name FROM skins ORDER BY skn_name";
$rs  = $dbh->exec($sql);

$skins_list = array();

while ($rs->hasNext()) {
    $skin = $rs->next();
    $skins_list[$skin["skn_id"]] = $skin["skn_name"];
}

if($_SESSION["aty_code"] == "SKINMANAGER"){
    $skin_id = getSkinId($_SESSION["jurisdiction_id"]);
}



?>
<div class="bodyHD">
    <?php if (!empty($skin_id)) : ?>
        <?=getSkinName($skin_id) . " "?>
    <?php endif; ?>
    <?=$lang->getLang("News Manager")?></div>
<div class="bodyHD3"><?=getClassLink(refFormPage($_GET["page"] . "&action=new"), $lang->getLang("Create a news"))?> | <?=getClassLink(refFormPage($_GET["page"]), $lang->getLang("Return to List"))?></div>
<script type="text/javascript" src="<?=secure_host?>/media/nic/nicEdit.js"></script>
<script type="text/javascript">
    bkLib.onDomLoaded(function(){ nicEditors.allTextAreas() });
</script>
<style>
    a.red{
        color:Red;
    }
    a.green{
        color:Green;
    }
</style>
<?php


$action   = $_REQUEST["action"];
$id       = $_REQUEST["nws_id"];
settype($id, "integer");

switch ($action){
    case "save":
        $save_action = "save_new";
        if(!empty($id)){
            $save_action = "edit";
        }


        $_POST["nws_start_date"] = date("Y-m-d H:i", strtotime($_POST["nws_start_date"]));

        if(!empty($_POST["nws_end_date"])){
            $_POST["nws_end_date"]   = date("Y-m-d H:i", strtotime($_POST["nws_end_date"]));
        }

        $valid_props = array(
            array("name" => "nws_title",            "type" => "string",  "beautyname" => "Title",      "nullable" => false),
            array("name" => "nws_summary",          "type" => "string",  "beautyname" => "Summary",    "nullable" => true),
            array("name" => "nws_body",             "type" => "string",  "beautyname" => "Body",       "nullable" => false),
            array("name" => "nws_skin_id",          "type" => "integer", "beautyname" => "Skin",       "nullable" => true),
            array("name" => "nws_language",         "type" => "string",  "beautyname" => "Language",   "nullable" => false),
            array("name" => "nws_start_date",       "type" => "date",    "beautyname" => "Start Date", "nullable" => false),
            array("name" => "nws_end_date",         "type" => "date",    "beautyname" => "End Date",   "nullable" => true)
        );

        $properties = array();

        foreach ($valid_props as $prop){
            if(checkType($_POST[$prop["name"]], $prop["type"], $prop["rules"], $prop["nullable"])){
                $properties[$prop["name"]] = escapeAndQuote($_POST[$prop["name"]], $prop["type"]);
            }else{
                addError($prop["name"], $prop["beautyname"] . " '" . $_POST[$prop["name"]] . "' must be a " . $prop["type"]);
            }
        }

        $properties["nws_aus_id"] = $_SESSION["admin_id"];

        if(areErrors()){
            showErrors();
            showEditPage($_POST);
            break;
        }

        $sql = "";
        if($save_action == "save_new"){
            $insert_columns = array();
            $insert_values  = array();

//      array_push($insert_columns, "nws_id");
//      array_push($insert_values,  "nws_id_seq.nextval");

            foreach ($properties as $key => $prop){
                array_push($insert_columns, $key);
                array_push($insert_values,  $prop);
            }

            $sql = "INSERT INTO news (" . implode(",\n", $insert_columns) . ") VALUES (" . implode(",\n", $insert_values) . ")";
//      die($sql);
        }elseif($save_action == "edit"){

            $sql = "UPDATE news SET ";

            $update_props = array();

            foreach ($properties as $key => $prop){
                array_push($update_props, $key . " = " . $prop);
            }

            $sql .= implode(", ", $update_props);
            $sql .= " WHERE nws_id = $id";
        }

        //echo "<code>$sql</code>";

        $res = $dbh->exec($sql);

        if($res !== 1){
            addError("", "Error in sql, contact webmaster <!--'$sql'-->");
            showErrors();
        }else{
            if($save_action == "save_new"){
                printInfoMessage("News " . $properties["nws_title"] . " inserted!");
            }else{
                printInfoMessage("News " . $properties["nws_title"] . " edited!");
            }
        }

        clearErrors();
        showNews();
        break;
    case "delete":
        $sql = "DELETE FROM news WHERE nws_id = $id";
        if(isset($skin_id)){
            $sql .= " AND nws_skin_id = $skin_id ";
        }
        $rs     = $dbh->exec($sql);
        printInfoMessage("News deleted!");
        showNews();
        break;
    case "enable":
        $sql = "UPDATE news SET nws_active = '1' WHERE nws_id = $id";
        if(isset($skin_id)){
            $sql .= " AND nws_skin_id = $skin_id ";
        }

        $rs     = $dbh->exec($sql);
        showNews();
        break;
    case "disable":
        $sql = "UPDATE news SET nws_active = '0' WHERE nws_id = $id";
        if(isset($skin_id)){
            $sql .= " AND nws_skin_id = $skin_id ";
        }

        $rs     = $dbh->exec($sql);
        showNews();
        break;
    case "copy":
        $sql = "SELECT nws_title, nws_summary, nws_body, nws_skin_id, nws_language, nws_start_date, nws_end_date, nws_active FROM news WHERE nws_id = $id";
        if(isset($skin_id)){
            $sql .= " AND nws_skin_id = $skin_id ";
        }

        $rs     = $dbh->exec($sql);
        if(!$rs->hasNext()){
            addError("", $lang->getLang("Invalid news id"));
            showErrors();
        }else{
            $row = $rs->next();
            showEditPage($row);
        }
        break;
    case "edit":
        $sql = "SELECT nws_id, nws_title, nws_summary, nws_body, nws_skin_id, nws_language, nws_start_date, nws_end_date, nws_active FROM news WHERE nws_id = $id";
        if(isset($skin_id)){
            $sql .= " AND nws_skin_id = $skin_id ";
        }

        $rs     = $dbh->exec($sql);
        if(!$rs->hasNext()){
            addError("", $lang->getLang("Invalid news id"));
            showErrors();
        }else{
            $row = $rs->next();
            showEditPage($row);
        }
        break;
    case "new":
        $default = array(
            "nws_start_date" => date("Y-m-d H:00",  strtotime("now + 1 hour")),
            "nws_end_date"   => date("Y-m-d 07:00", strtotime("tomorrow + 7 days")),
        );
        showEditPage($default);
        break;
    default:
        showNews();
        break;
}

function getStatus($rec){
    if($rec["nws_active"] == '0'){
        return "<span>" . getClassLink(refFormPage($_GET["page"] . "&action=enable&nws_id="   . $rec["nws_id"]), "Disabled", "red") . "</span>";
    }else{
        return "<span>" . getClassLink(refFormPage($_GET["page"] . "&action=disable&nws_id="   . $rec["nws_id"]), "Enabled", "green") . "</span>";
    }
}

function getSkinName($skinid){
    global $skins_list;

    $ret = "All";
    if($skinid != -1){
        $ret = ucwords($skins_list[$skinid]);
    }

    if(empty($ret)){
        $ret = "Not found skin $skinid";
    }

    return $ret;
}

function showNews(){
    global $dbh, $skin_id , $lang;
    $status_id = $_GET["status"];


    if(!isset($status_id)){
        $status_id = 2;
        if(isset($_SESSION["news"]["status"])){
            $status_id = $_SESSION["news"]["status"];
        }
    }

    settype($status_id, "integer");

    $_SESSION["news"]["status"] = $status_id;

    ?>

    <?form_head(refFormPage($_GET["page"]), "search", "GET");?>
    <table cellpadding=4 cellspacing=1 border=0>
        <tr>
            <td class="label"><?=$lang->getLang("News Status");?></td>
            <td class="content">
                <?php
                $status = array(0 => "All News", 1 => "Past News", 2 => "Active News");
                echo "<select name='status'>";
                foreach($status as $key => $s){
                    printOption("status", $key, $s, ($status_id == $key));
                }
                echo "</select>";
                ?>
                <input type="submit" name="<?=$lang->getLang("Submit");?>"/>
            </td>
        </tr>
        <tr>
            <td colspan="2">
            </td>
        </tr>
    </table>
    </form>

    <?php



    $sql = "SELECT
            nws_id, nws_title, nws_summary, nws_skin_id, nws_language, 
            nws_start_date, nws_end_date, nws_active 
          FROM 
            news";

    switch ($status_id){
        case 1:
            $sql .= " WHERE nws_end_date <= CURRENT_TIMESTAMP ";
            break;
        case 2:
            $sql .= " WHERE nws_end_date >= CURRENT_TIMESTAMP ";
            break;
        default:
            $sql .= " WHERE 1 = 1 ";
            break;
    }

    if(isset($skin_id)){
        $sql .= " AND nws_skin_id = $skin_id ";
    }

    $sql .= "ORDER BY
            nws_start_date DESC, nws_creation_date DESC";
    $rs = $dbh->exec($sql);



    $cols_arr = array("Title", "Summary", "Skin", "Language", "Start", "End", "Active", "Edit", "Copy" , "Delete");
    $val_fmt_arr = array(
        'ID'        =>'return $rec["nws_id"];',
        'Title'     =>'return $rec["nws_title"];',
        'Summary'   =>'return strip_tags($rec["nws_summary"], "<br/>");',
        'Skin'      =>'return getSkinName($rec["nws_skin_id"]);',
        'Language'  =>'return ((!empty($rec["nws_language"]))?(strtoupper($rec["nws_language"])):("All"));',
        'Start'     =>'return getDateStatus($rec["nws_start_date"]);',
        'End'       =>'return getDateStatus($rec["nws_end_date"]);',
        'Active'    =>'return getStatus($rec);',
        'Edit'      =>'return getClassLink(refFormPage($_GET["page"] . "&action=edit&nws_id="   . $rec["nws_id"]), "Edit");',
        'Copy'      =>'return getClassLink(refFormPage($_GET["page"] . "&action=copy&nws_id="   . $rec["nws_id"]), "Copy");',
        'Delete'    =>'return getClassLink(refFormPage($_GET["page"] . "&action=delete&nws_id=" . $rec["nws_id"]), "Delete");',
    );

    $numrow = $rs->getNumRows();
    if($numrow > 0){
        $qp = new QueryPrinter($rs);
        $qp->printTable($rs, $cols_arr, "", $val_fmt_arr, null, 0, 0, "", $width="100%", 1, 2000, false);
    }else{
        addError("", "No news found");
        showErrors();
    }

}

function showEditPage($rs=null){
    global $dbh, $skins_list, $site_avail_langs, $skin_id , $lang;
    ?>

    <form name="news_editor" action="<?=refFormPage($_GET["page"] . "&action=save")?>" method="POST">
        <input type="hidden" name="nws_id" value="<?=$rs["nws_id"]?>">

        <table cellpadding=0 cellspacing="1" border=0>
            <tr>
                <td>
                    <table cellpadding="4" cellspacing="1" border=0>

                        <tr cellpadding="4">
                            <th colspan="2" class="formheading"><?=$lang->getLang("News Editor")?></th>
                        </tr>

                        <tr valign="top">
                            <td class="label"><?=$lang->getLang("Title")?></td>
                            <td <? form_td_style('nws_title'); ?>><input type="text" name="nws_title" value="<?=eregi_replace("''", "", $rs["nws_title"])?>" size="50">
                                <?php echo err_field('nws_title'); ?>
                            </td>
                        </tr>
                        <tr valign="top">
                            <td class="label"><?=$lang->getLang("Summary")?></td>
                            <td <? form_td_style('nws_summary'); ?>>
                                <textarea name="nws_summary" cols="80" rows="10" wrap="hard"><?=eregi_replace("''", "", $rs["nws_summary"])?></textarea>
                                <?php echo err_field('nws_summary'); ?>
                            </td>
                        </tr>
                        <tr valign="top">
                            <td class="label"><?=$lang->getLang("Body")?></td>
                            <td <? form_td_style('nws_body'); ?>>
                                <textarea name="nws_body" cols="80" rows="25" wrap="hard"><?=eregi_replace("''", "'", $rs["nws_body"])?></textarea>
                                <?php echo err_field('nws_body'); ?>
                            </td>
                        </tr>

                        <tr cellpadding="4">
                            <th colspan="2" class="formheading"><?=$lang->getLang("DATE")?></th>
                            <?php
                            setupDateInputJs("nws_start_date", "nws_end_date");
                            ?>
                        </tr>
                        <tr>
                            <td class="label"><?=$lang->getLang("From")?></td>
                            <td><?=createDateInput("nws_start_date", $rs["nws_start_date"], false, "news_editor")?></td>
                        </tr>
                        <tr>
                            <td class="label"><?=$lang->getLang("Until")?></td>
                            <td><?=createDateInput("nws_end_date",  $rs["nws_end_date"], false, "news_editor")?></td>
                        </tr>

                        <tr cellpadding="4">
                            <th colspan="2" class="formheading"><?=$lang->getLang("SETTINGS")?></th>
                        </tr>

                        <?php if(isset($skin_id)) : ?>
                            <input type="hidden" name="nws_skin_id" value="<?=$skin_id?>"/>
                        <?php else :  ?>
                            <tr valign="top">
                                <td class="label">Skin</td>
                                <td <? form_td_style('nws_skin_id'); ?>>
                                    <select name="nws_skin_id" <?if($ended){echo"disabled";}?>>
                                        <?php
                                        asort($skins_list);
                                        printOption("nws_skin_id", "-1", "All");
                                        foreach($skins_list as $skin_id => $skin_name){
                                            printOption("none", $skin_id, ucwords($skin_name), (($skin_id == $rs["nws_skin_id"]) && !is_null($skin_id)));
                                        }
                                        ?>
                                    </select>
                                </td>
                            </tr>
                        <?php endif; ?>

                        <tr valign="top">
                            <td class="label"><?=$lang->getLang("Language")?></td>
                            <td <? form_td_style('nws_language'); ?>>
                                <select name="nws_language" <?if($ended){echo"disabled";}?>>
                                    <?php
                                    asort($site_avail_langs);
                                    foreach($site_avail_langs as $lang){
                                        printOption("nws_language", $lang, strtoupper($lang), ($lang == $rs["nws_language"]));
                                    }
                                    ?>
                                </select>
                            </td>
                        </tr>

                        <tr valign="top">
                            <td colspan="2" align="center"><input type="hidden" name="trn_id" value="<?=$rs["trn_id"]?>"/><input type="reset" <?if($ended){echo"disabled";}?>/>&nbsp;<input type="submit" <?if($ended){echo"disabled";}?>/></td>
                        </tr>

                    </table>
                </td>
            </tr>
        </table>
    </form>

<?php
}
?>
