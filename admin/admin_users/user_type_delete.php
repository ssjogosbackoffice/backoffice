<?php loginOnlyPage('/admin/'); ?>
<?php
// delete an admin user record
function delete_type ($id, $name){
    global $dbh,$err_msg,$lang;
    $num_users = $dbh->queryOne("select count(*) from admin_user where aus_aty_id = " . $id);
    if ( $num_users > 0 ){
        $err_msg = "";
    }else {
        $dbh->begin();
        $sql = "DELETE FROM admin_user_type WHERE aty_id = " . $id;
        $ret=$dbh->exec($sql);
        if ( 1 == $ret )
        {
            if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
                $result=betting_delete_admin_user($id);
                if(1==$result){
                    $dbh->commit();
                    return true;
                }
                else{
                    betting_rollback();
                    $dbh->rollback();
                    return;
                }
            }
            $dbh->commit();
            return true;

        }
        else
        {

            $err_msg = $lang->getLang("Database Error");
        }
    }
    return false;
}

$id = (int)$_GET['id'];

$sql = "select aty_name from admin_user_type where aty_id = ". $id;
$dbh->doQuery($sql);
$rs  = $dbh->getResults();
$num_rows =$dbh->getNumRows();

if ( 1 == $num_rows ){
    $row  = $rs->fetchRow();
    $name = $row['aty_name'];
    $deleted = delete_type($id, $name);
    if ( $deleted ){
        echo "1,".$lang->getLang("Administrator user type '%' deleted",$name);
    }else{
        echo $lang->getLang("$err_msg");
    }
}else{
    echo $lang->getLang("User type with id % not found",$id);
}
?>
