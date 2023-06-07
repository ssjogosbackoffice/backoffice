<?php
check_access("manage_admin_users", true);
function show_link ($label, $col_orderby){
    global $orderby, $direction;
    echo $label;

    $link_srt_asc  = "javascript:sortTable('$col_orderby', 'asc')";
    $link_srt_desc = "javascript:sortTable('$col_orderby', 'desc')";

    if ( $orderby == $col_orderby ){
        if ( 'desc' == $direction ){
            ?>  <a href="<?=$link_srt_asc?>"><img src="<?=image_dir;?>/arrow_white_up.gif" border="0" alt="Sort ascending"></a>
            <img src="<?=image_dir;?>/arrow_red_down.gif">
        <?
        }
        else{
            ?>  <img src="<?=image_dir;?>/arrow_red_up.gif">
            <a href="<?=$link_srt_desc?>"><img src="<?=image_dir;?>/arrow_white_down.gif" border="0" alt="Sort descending"></a>
        <?
        }
    }
    else{
        ?>  <a href="<?=$link_srt_asc?>"><img src="<?=image_dir;?>/arrow_white_up.gif" border="0" alt="Sort ascending"></a>
        <a href="<?=$link_srt_desc?>"><img src="<?=image_dir;?>/arrow_white_down.gif" border="0" alt="Sort descending"></a>
    <?
    }
}

function cleanStringForJs($string){
    $string = eregi_replace("[^a-z0-9@ .]+", "", $string);
    $string = substr($string, 0, 30);
    return "\"" . $string . "\"";
}

$search_string = $_REQUEST["search_string"];
$search_string = trim($search_string);

$jurisdiction = $_REQUEST["jurisdiction"];
settype($jurisdiction, "integer");

$jurisdiction_level = $_REQUEST["jurisdiction_level"];

$subusers = !is_null($_REQUEST["sub-users"]);

$orderby_arr = array ('name'=>'aus_name', 'username'=>'aus_username', 'email'=>'aus_email', 'access'=>'aus_access',
    'email', 'aus_email', 'usertype'=>'aty_name', 'entitytype'=>'aty_jurisdiction_class',
    'entity'=>'jur_name');

$orderby = $_GET['orderby'];
if ( empty($orderby) || ! array_key_exists($orderby,$orderby_arr) )
    $orderby = 'name';

$db_orderby = $orderby_arr[$orderby];

$direction = $_GET['direction'];

if ( $direction != 'desc' )
    $direction = 'asc';
$sql  = 'SELECT aus_id,  aus_username, aus_name, aus_email, aus_access, aus_jurisdiction_id, ' .
    'aty_name, aty_code, aus_last_logged_in, aty_jurisdiction_class, jur_parent_id, jur_name, j1.jur_code AS jurisdiction_code ' .
    'FROM admin_user JOIN admin_user_type ON aus_aty_id = aty_id LEFT OUTER JOIN  jurisdiction j1 ' .
    'ON aus_jurisdiction_id = j1.jur_id ' .
    'WHERE ( aus_jurisdiction_id IN ' . // get child admin users
    '        ( SELECT j2.jur_id FROM jurisdiction j2 WHERE j2.jur_parent_id = ' . $_SESSION['jurisdiction_id'] . ')' .
    '       OR ' .
    '       aus_jurisdiction_id IN ' . // get grandchild admin users
    '       (  SELECT j2.jur_id FROM jurisdiction j2 WHERE j2.jur_parent_id IN ' .
    '          ( SELECT j3.jur_id FROM jurisdiction j3 WHERE j3.jur_parent_id = ' . $_SESSION["jurisdiction_id"] . ')' .
    '       )' .
    '       OR ' .
    '       aus_jurisdiction_id IN ' . // get great-grandchild admin users
    '       (  SELECT j2.jur_id FROM jurisdiction j2 WHERE j2.jur_parent_id IN ' .
    '          ( SELECT j3.jur_id FROM jurisdiction j3 WHERE j3.jur_parent_id IN ' .
    '            ( SELECT j4.jur_id FROM jurisdiction j4 ' .
    '              WHERE j4.jur_parent_id  = ' . $_SESSION['jurisdiction_id'] .
    '            ) ' .
    '          ) ' .
    '       )' .
    '       OR ' .
    '       aus_jurisdiction_id IN ' .
    '       (  SELECT j2.jur_id FROM jurisdiction j2 WHERE j2.jur_parent_id IN ( '.
    '               SELECT j3.jur_id FROM jurisdiction j3 WHERE j3.jur_parent_id IN ( '.
    '                    SELECT j4.jur_id FROM jurisdiction j4 WHERE j4.jur_parent_id IN ( '.
    '                         SELECT j5.jur_id FROM jurisdiction j5 WHERE j5.jur_parent_id = ' . $_SESSION["jurisdiction_id"] .
    '                    )'.
    '               )'.
    '          )'.
    '       )'.
    '       OR ' .  // same jurisdiction, higher aty_level level
    '       (  aus_jurisdiction_id = ' . $_SESSION['jurisdiction_id'] .
    '          AND aty_jurisdiction_class = \'' . $_SESSION['jurisdiction_class']. '\'' .
    '          AND aty_level  >' .($_SESSION['aty_level']=='2'?'=':''). $_SESSION['aty_level'] .
    '       )' .
    '     )';
if(!empty($search_string)){
    $search_string = eregi_replace("[^a-z0-9]", ".", $search_string);
    if(strlen($search_string) < 3 && (empty($jurisdiction) && empty($jurisdiction_level))){
        addError("", $lang->getLang("Search string too short, min. 3 chars."));
    }
    $sql .= " AND ((aus_username LIKE '%" . $dbh->escape($search_string) . "%') OR  (aus_name LIKE '%" . $dbh->escape($search_string) . "%') OR (jur_name LIKE '%" . $dbh->escape($search_string) . "%'))";
}

if($jurisdiction_level == "casino"){
    $jurisdiction = 1;
}

if($subusers && !empty($jurisdiction)){
    $sql .= " AND (jur_id = $jurisdiction OR jur_parent_id = $jurisdiction)";
}else{
    if(!empty($jurisdiction_level)){
        $sql .= " AND jur_class = '" . $dbh->escape($jurisdiction_level) . "'";

    }elseif(($_SESSION['jurisdiction_class'] != "club" && $_SESSION['jurisdiction_class'] != "district") && empty($jurisdiction_level) && empty($search_string)){
        addError("", $lang->getLang("Please select jurisdiction level, or a search string."));
        $sql .= " AND 1 = 0 ";
    }
    if(!empty($jurisdiction)){
        $sql .= " AND jur_id = $jurisdiction";
    }
}

if(($_SESSION['jurisdiction_class'] == "casino" || $_SESSION['jurisdiction_class'] == "nation") && $jurisdiction_level == "club" && empty($jurisdiction) && empty($search_string)){
    addError("", $lang->getLang("Please select a club or search by name"));
    $sql .= " AND 1 = 0 ";
}

if($access=="1"){
    $sql.=" AND aus_access='ALLOW' ";
}
if($access=="2"){
    $sql.=" AND aus_access='DENY' ";
}


$sql .= " ORDER BY ";

$sql .= " LOWER(TRIM(jur_name)), LOWER(TRIM(aus_username))";
if(areErrors()){
    showErrors();
    exit();
}
$rs = $dbh->exec($sql);
$num_rows = intval($rs->getNumRows());
?>

<?php include 'xmlhttprequest_inc.php'; ?>
<script language="JavaScript">
    function retrieveURL(url, resultFunction) {
        if (window.XMLHttpRequest) { // Non-IE browsers
            req = new XMLHttpRequest();
            req.onreadystatechange = eval(resultFunction);
            try {
                req.open("GET", url, true);
            } catch (e) {
                alert(e);
            }
            req.send(null);
        } else if (window.ActiveXObject) { // IE
            req = new ActiveXObject("Microsoft.XMLHTTP");
            if (req) {
                req.onreadystatechange = eval(resultFunction);
                req.open("GET", url, true);
                req.send();
            }
        }
    }

    function updateTable(){
        if (req.readyState == 4){
            // Complete
            if (req.status == 200){
                // OK response
                document.getElementById(contentArea).innerHTML = req.responseText;
            }else {
                alert("Status: " + req.status + " Problem: updateTable" + req.statusText);
            }
        }
    }


    function updateTableAndAlert(msg){
        if (req.readyState == 4){
            // Complete
            if (req.status == 200){
                // OK response
                document.getElementById(contentArea).innerHTML = req.responseText;
                alert(msg);
            }else{
                alert("Status: " + req.status + " Problem: updateTableAndAlert " + req.statusText);
            }
        }
    }

    function alertResponse(){
        if (req.readyState == 4){
            // Complete
            if (req.status == 200){
                // OK response
                var msg = req.responseText;
                func =  function () { updateTableAndAlert(msg); }
                retrieveURL('/admin_users/admin_user_list.php?orderby=<?=$orderby?>&direction=<?=$direction?>',func);
            }else {
                alert("Status: " + req.status + " Problem: alertResponse " + req.statusText);
            }
        }
    }

    function deleteUser(name, id, orderby, direction){
        deleteuser = confirm("<?=$lang->getLang('Delete admin user')?> " + name + " ?");
        if(deleteuser){
            retrieveURL('/admin_users/admin_user_delete.php?id=' + id, 'alertResponse');
        }
    }
    var contentArea = 'urlContent';
</script>
<?php
$view_logs = check_access('view_admin_user_log');
if ( $num_rows < 1 ) {
    printInfoMessage($lang->getLang("There are currently no system users"));
} else {
    ?>
    <table cellpadding="4" cellspacing="1" border="0" bgcolor="#E9E9E9">
        <tr valign=top height="18">
            <td class=label><?=$lang->getLang("Username")?></td>
            <td class=label><?=$lang->getLang("User Type")?></td>
            <td class=label><?=$lang->getLang("Full Name")?></td>
            <td class=label><?=$lang->getLang("Level")?></td>
            <td class=label><?=$lang->getLang("Access")?></td>
            <td class=label></td>
        </tr>
        <?php
        $i = 0;
        $last_letter = "-";
        while ( $row = $rs->next()){
            if ( 'DENY' ==$row['aus_access'] )
                $access_disp = "<span style='color:red'>".$lang->getLang('Denied')."</span>";
            else
                $access_disp = "Allowed ";
            ?>
            <?php
            $letter = $row['jur_name'];
            if($last_letter != $letter){
                $last_letter = $letter;
                ?>
                <tr class="table_title">
                    <td colspan="3"><strong><?=$last_letter?></strong></td>
                    <td colspan="5"><strong><span class="<?=$row['aty_jurisdiction_class']?>"><?=$row['aty_jurisdiction_class']?></span></strong></td>
                </tr>
            <?php
            }
            ?>
            <tr class="content <?=(('DENY' ==$row['aus_access'])?("deny"):(""))?>">
                <td class=content><?=$row['aus_username']?></td>
                <td class=content><?=$row['aty_name']?></td>
                <td class=content><?=$row['aus_name']?></td>
                <td class="<?=$row['aty_jurisdiction_class']?>"><?=$row['aty_jurisdiction_class']?></td>

                <td class=content><?=$access_disp?></td>
                <td class=content>
                    <?php if ($view_logs): ?>
                        <a href="/?page=admin_users/log&curr_aus_id=<?=$row['aus_id'];?>" class="contentlink"><img src="<?=image_dir;?>/icon_log.gif" border="0" alt="Log"></a>
                    <?php endif; ?>
                    <?php if ( $row['aty_code'] != 'SUPERUSER' ): ?>
                        <a href="/admin_users/admin_user_edit.php?id=<?=$row['aus_id'];?>" class="contentlink"><img src="<?=image_dir;?>/icon_edit_off.gif" border="0" alt="Edit"></a>
                        <a href="javascript:deleteUser('<?=$row['aus_name']?>', <?=$row['aus_id']?>, '<?=$orderby?>', '<?=$direction;?>')" class="contentlink"><img src="<?=image_dir;?>/icon_delete.gif" border="0" alt="Delete" /></a>
                    <?php endif; ?>
                </td>
            </tr>
            <?php
            $i++;
        }
        ?>
    </table>
<?php
}
?>
