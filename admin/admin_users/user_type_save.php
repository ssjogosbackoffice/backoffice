<?php
function save_type($code, $name, $class, $level){
    global $dbh, $err_msgs,$lang;
    $err_msgs = array();
    if ( !$code ){
        $err_msgs[] = "A type code must be entered";
    }else{
        $sql = "SELECT count(*) FROM admin_user_type WHERE aty_code = ".$dbh->prepareString($code);
        $count = $dbh->countQuery($sql);
        if ( $count > 0 ){
            $err_msgs[] =  $lang->getLang("The code you entered already exists for another admin user type");
        }
    }

    if ( !$name )
        $err_msgs[] =  $lang->getLang("A type name must be entered");

    if ( ! $class )
        $err_msgs[] =  $lang->getLang("An entity type must be selected");

    $level = (int)$level;

    if ( count($err_msgs) == 0 ){
        $next_id  = $dbh->nextSequence('ATY_ID_SEQ');
        $dbh->begin();
       /*     if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
                betting_begin();
            }*/
        $sql = "INSERT INTO admin_user_type (aty_id, aty_code, aty_name, aty_level, aty_jurisdiction_class)" .
            " VALUES (" . $next_id . ", " . $dbh->prepareString($code) . ", " . $dbh->prepareString($name) .
            ", " . $level . ", " . $dbh->prepareString($class) . ")";
        $ret=$dbh->exec($sql);
 /*       if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
            $result2=betting_insert_admin_user($next_id,$code,$name,$level,$class,$code);
            if(!$result2){
                addError("", $lang->getLang("Database error"));
                $dbh->rollback();
                betting_rollback();
                return;
            }
        }*/
        if ( 1 == $ret ){
            $dbh->commit();
        /*    if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
                betting_commit();
            }*/
            return TRUE;

        }else{
            $err_msgs[] =  $lang->getLang("Database Error");
            $dbh->rollback();
/*            if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
                betting_rollback();
            }*/

        }


    }
    return FALSE;
}

$saved = save_type($_GET['code'], $_GET['name'], $_GET['juris_class'], $_GET['level']);
$name = $_GET['name'];

if ( TRUE == $saved ){
    echo  "1,".$lang->getLang("Administrator user % saved",$name);  // successful status
}else{
    echo '-1';  // failed status
    if ( count($err_msgs) > 0  )      // append error messages
        echo ',' .  $lang->getLang("The following errors occurred:") . implode("\n - ",$err_msgs);
}
?>