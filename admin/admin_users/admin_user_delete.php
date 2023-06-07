<?php
loginOnlyPage('/admin/');

check_access("manage_admin_users", true);

function delete_admin_user ($id){
    global $dbh;
    $id      = (int)$id;
    $aus_row = getAdminUser($id);

    if ( $aus_row ){
        $name 		     = $aus_row['name'];
        $last_logged_in  = $aus_row['last_logged_in'];
        $username        = $aus_row['username'];
        $user_type 		  = $aus_row['aty_id'];

        if($last_logged_in){
            $sql = 'UPDATE admin_user SET aus_num_logins = -1 WHERE aus_id = ' . $id;
            $sql .= ' AND ' .
                '((aus_jurisdiction_id =  '. $_SESSION['jurisdiction_id'] .
                ' AND (SELECT aty_level FROM admin_user_type WHERE aty_id = aus_aty_id) > ' . $_SESSION['aty_level'] .' )' .
                ' OR   (aus_jurisdiction_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = ' . $_SESSION['jurisdiction_id'] . '))' .
                ' OR   (aus_jurisdiction_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = ' . $_SESSION['jurisdiction_id'] . ')))' .
                ' OR   (aus_jurisdiction_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = ' . $_SESSION['jurisdiction_id'] . ')))))';

            $dbh->begin();
            $ret=$dbh->exec($sql);

            if($ret){
                // update succeeded
                doAdminUserLog($_SESSION['admin_id'], escapeSingleQuotes('disabled_admin_user'), escapeSingleQuotes("aus_id:$id|aus_username:$username|aus_name:$name|aus_aty_id:$user_type"), $pun_id='NULL');
                $ret = TRUE;
                $dbh->commit();
            }else{
                // update failed
                $ret = FALSE;
                $dbh->rollback();
            }
            return $ret;
        }else{
            // admin user can be physically deleted
            $dbh->begin();
            Db::beginTransaction();
            $sql = 'DELETE FROM admin_user WHERE aus_id = ' . $id;
            // jurisdictions below the casino jurisdiction have an ID
            if ( $_SESSION['jurisdiction_id'] != 1 ){
                $sql .= ' AND ' .
                    '((aus_jurisdiction_id =  '. $_SESSION['jurisdiction_id'] .
                    ' AND (SELECT aty_level FROM admin_user_type WHERE aty_id = aus_aty_id) > ' . $_SESSION['aty_level'] .' )' .
                    ' OR   (aus_jurisdiction_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = ' . $_SESSION['jurisdiction_id'] . '))' .
                    ' OR   (aus_jurisdiction_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = ' . $_SESSION['jurisdiction_id'] . ')))' .
                    ' OR   (aus_jurisdiction_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = ' . $_SESSION['jurisdiction_id'] . ')))))';
            }
            $res=$dbh->exec($sql);

            if($res){
                // delete succeeded
                doAdminUserLog($_SESSION['admin_id'], escapeSingleQuotes('delete_admin_user'), escapeSingleQuotes("aus_id:$id|aus_username:$username|aus_name:$name|aus_aty_id:$user_type"), $pun_id='NULL');
                $dbh->commit();
                Db::commitTransaction();
                $ret = TRUE;
            }else{
                // delete failed
                $ret = FALSE;
                $dbh->rollback();
                Db::rollbackTransaction();
            }
            return $ret;
        }
    }
    return FALSE;
}

$id  = (int)$_GET['id'];

$sql = "select aus_name from admin_user where aus_id = ". $id;

$rs  = $dbh->exec($sql);
$num_rows = $rs->getNumRows();

if ( 1 == $num_rows ){
    $name = $rs['aus_name'];
    $deleted = delete_admin_user($id);
    if ( $deleted ){
        echo $lang->getLang("Administrator user % has been deleted", $name);
    }
    else{
        echo $lang->getLang("Failed to delete admin user %", $name);
    }
}
?>
