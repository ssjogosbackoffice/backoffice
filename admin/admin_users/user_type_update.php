<?php
function save_type($code="", $name="", $class="", $level=""){
    global $dbh,$err_msg,$lang;
    $err_msg = '';
    if ( !$code ){
        $err_msgs[] = $lang->getLang("A type code must be entered");
    }else{
        $sql = "SELECT count(*) FROM admin_user_type WHERE aty_code = ".$dbh->prepareString($code);
        $count = $dbh->queryOne($sql);
        if ( $count > 0 ){
            $err_msgs[] = $lang->getLang("The code you entered already  exists for another admin user type");
        }
    }

    if ( !$name ){
        $err_msgs[] = $lang->getLang("A type name must be entered");
    }

    if ( ! $class ){
        $err_msgs[] = $lang->getLang("An entity type must be selected");
    }

    $level = (int)$level;


    if ( ! $err_msg ){
        $next_id  = $dbh->nextSequence('aty_id_seq');

        $sql = "INSERT INTO admin_user_type (aty_id, aty_code, aty_name, aty_level, aty_jurisdiction_class)" .
            " VALUES (" . $next_id . ", " . $dbh->prepareString($code) . ", " . $dbh->prepareString($name) .
            ", " . $level . ", " . $dbh->prepareString($class) . ")";
        $ret=$dbh->exec($sql);
        if ( 1 == $ret )
            return TRUE;
        else
            $err_msgs[] = "Database Error";
    }
    return FALSE;
}


function update_type ($id, $code, $name, $jurisdiction_class, $level){
    global  $dbh, $err_msgs,$lang;
    $err_msg = array();
    $level = (int) $level; // default level is 0
    // check code field
    if ( !$code ){
        $err_msgs[] = $lang->getLang("A type code must be entered");  //missing code
    }else{
        //check if any of the other types have the same aty_code
        $sql = 'SELECT count(*) FROM admin_user_type WHERE lower(aty_code) = ' . $dbh->prepareString($code) .
            ' AND aty_id != ' . $id;
        $exists = $dbh->queryOne($sql);
        if ( $exists > 0 ){
            $err_msgs[] = $lang->getLang("The code you entered already  exists for another admin user type");
        }
    }

    if ( isBlank($name) )
        $err_msgs[] = $lang->getLang("A type name must be entered");

    if ( isBlank($jurisdiction_class) )
        $err_msgs[] = $lang->getLang("An entity type must be selected");
    else if ( 'casino' != $jurisdiction_class && 'club' != $jurisdiction_class
        && 'district' != $jurisdiction_class && 'region' != $jurisdiction_class  && 'nation' != $jurisdiction_class )
        $err_msgs[] = $lang->getLang('Invalid jurisdiction class');

    if ( count($err_msgs) == 0 ){
        $dbh->begin();
        $sql = "UPDATE admin_user_type " .
            "SET aty_code = " . $dbh->prepareString($code) .
            ", aty_name = " . $dbh->prepareString($name) .
            ", aty_jurisdiction_class = " . $dbh->prepareString($jurisdiction_class) .
            ", aty_level = " . $level .
            " WHERE aty_id = " . $id;
        $ret=$dbh->exec($sql);

        if ( 1 == $ret )
        {
    /*        if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
                $result=betting_update_admin_user($id, $code, $name, $jurisdiction_class, $level);
                if(1==$result){
                    $dbh->commit();
                    return true;
                }
                else{
                    betting_rollback();
                    $dbh->rollback();
                    return;
                }
            }*/
            $dbh->commit();
            return true;

        }
        else
        {

            $err_msg = $lang->getLang("Database Error");
        }
    }
    return FALSE;
}

$updated = update_type($_GET['id'], $_GET['code'], $_GET['name'], $_GET['juris_class'], $_GET['level']);

if ( TRUE == $updated ){
    echo "1,".$lang->getLang("Details updated successfully");  // successful status
}else{
    echo '-1';  // failed status
    if ( count($err_msgs) > 0  )
         global $lang;
         // append error messages
        echo ',' . $lang->getLang("The following errors occurred:") . implode("\n - ",$err_msgs);
}
?>