<?//sleep(3)?>
<?php
//save an admin user record
function save_type($code="", $name="", $class="", $level=""){
    global $dbh,$err_msg,$lang;
    $err_msg = '';
    if ( !$code ){
        $err_msg = "- $lang->getLang('A type code must be entered')";
    }else{
        $sql = "SELECT count(*) FROM admin_user_type" .
            " WHERE aty_code = ".$dbh->prepareString($code);
        $count = $dbh->countQuery($sql);
        if ( $count > 0 ){
            $err_msg = '\n-' . $lang->getLang("The code you entered already  exists for another admin user type");
        }
    }

    if ( !$name ){
        $err_msg .= '\n-' . $lang->getLang("A type name must be entered");
    }

    if ( ! $class ){
        $err_msg .= '\n-' .$lang->getLang("An entity type must be selected");
    }

    $level = (int)$level;

    if ( ! $err_msg ){
        $next_id  = $dbh->nextSequence('seq_aty_id');
        $sql = "INSERT INTO admin_user_type (aty_id, aty_code, aty_name, aty_level, aty_jurisdiction_class)" .
            " VALUES (" . $next_id . ", " . $dbh->prepareString($code) . ", " . $dbh->prepareString($name) .
            ", " . $level . ", " . $dbh->prepareString($class) . ")";

        $dbh->exec($sql);
        return TRUE;
    }
    return FALSE;
}

// perform operation on admin user, if specified
$op = getPostGetVar('op');
switch ( $op ){
    case "Save":  //save a new task
        $saved = save_type($_POST['code'], $_POST['name'], $_POST['jurisdiction_class'], $_POST['level']);
        if (  'TRUE' == $saved ){
            $name  = $_POST['name'];
            $alert = "";
        }else{
            if ( $err_msg ){
                $alert = $err_msg;
            }
            $addbox_visibility = 'visible';
        }
        break;
    case "Update":  //update an existing task
        break;
}

function show_link ($label, $col_orderby){
    global $orderby, $direction , $lang;

    echo $label;

    $link_srt_asc  = "javascript:sortTable('$col_orderby', 'asc')";
    $link_srt_desc = "javascript:sortTable('$col_orderby', 'desc')";

    if ( $orderby == $col_orderby ){
        if ( 'desc' == $direction ){
            ?>  <a href="<?=$link_srt_asc?>"><img src="<?=image_dir;?>/arrow_white_up.gif" border="0" alt="Sort ascending"></a>
            <img src="<?=image_dir;?>/arrow_red_down.gif">
        <?
        }else{
            ?>  <img src="<?=image_dir;?>/arrow_red_up.gif">
            <a href="<?=$link_srt_desc?>"><img src="<?=image_dir;?>/arrow_white_down.gif" border="0" alt="Sort descending"></a>
        <?
        }
    }else{
        ?>  <a href="<?=$link_srt_asc?>"><img src="<?=image_dir;?>/arrow_white_up.gif" border="0" alt="Sort ascending"></a>
        <a href="<?=$link_srt_desc?>"><img src="<?=image_dir;?>/arrow_white_down.gif" border="0" alt="Sort descending"></a>
    <?
    }
}

$orderby_arr = array ('name'=>'aty_name', 'code'=>'aty_code', 'level'=>'aty_level', 'entity_type'=>'aty_jurisdiction_class');

$orderby = getPostGetVar('orderby');
if ( empty($orderby) || ! array_key_exists($orderby,$orderby_arr) )
    $orderby = 'name';

$db_orderby = $orderby_arr[$orderby];

$direction = getPostGetVar('direction');

if ( $direction != 'desc' )
    $direction = 'asc';

$sql = "SELECT aty_id, aty_name, aty_code, aty_level, aty_jurisdiction_class" .
    "  FROM admin_user_type" .
    " ORDER BY $db_orderby $direction";

$dbh->doQuery($sql);

$rs  = $dbh->getResults();
$num_rows = count($rs);
if ( $num_rows > 0 ){
    global $lang;
    ?>
    <table cellpadding=4 cellspacing=1 border="0" bgcolor="#E9E9E9">
        <tr>
            <td class="label"><?show_link($lang->getLang("Name"), "name")?></td>
            <td class="label"><?show_link($lang->getLang("Code"), "code")?></td>
            <td class="label"><?show_link($lang->getLang("Entity Type"), "entity_type")?></td>
            <td class="label"><?show_link($lang->getLang("Level"), "level")?></td>
            <td class="label"><?show_link($lang->getLang("Action"),"action")?></td>
        </tr>
        <?
        //$rs->resetRecPtr();
        //$row = $rs->next();
        $i=0;
        while ( $row=$rs->fetchRow() )
        {
            $id          =  $row['aty_id'];
            $name        =  $row['aty_name'];
            $code        =  $row['aty_code'];
            $juris_class =  $row['aty_jurisdiction_class'];
            $level       =  $row['aty_level'];
            ?>
            <tr>
                <td <?form_td_style("names[$id]")?>>
                    <?=$name;?>
                </td>
                <td <?form_td_style("codes[$id]")?>>
                    <?=$code;?>
                </td>
                <td <?form_td_style("jurisdiction_classes[$id]")?>>
                    <?=$juris_class?>
                </td>
                <td <?form_td_style("levels[$id]")?>>
                    <?=$level?>
                </td>
                <td <?form_td_style("buttons[$id]")?>>
                    <a href="javascript:deleteUserType(<?=$id?>, '<?=$name?>', '<?=$orderby?>', '<?=$direction?>')"><img src="<?=image_dir;?>/icon_delete.gif" border="0" /></a>&nbsp;&nbsp;  <a href="javascript:showEditBox(<?=$id?>, '<?=$name?>', '<?=$code?>', '<?=$juris_class?>', <?=$level?>)"><img src="<?=image_dir;?>/icon_edit_off.gif" border="0" /></a>
                </td>
            </tr>
            <?php      // $row = $rs->next();
            //$i++;
        }
        ?>
    </table>
<?  }

if ( isset($alert) )
    jAlert($alert);
?>