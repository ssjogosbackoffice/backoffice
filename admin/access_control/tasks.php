<?check_access('manage_tasks',$doredirect=true);?>
<?php

$title = $lang->getLang("Administrator Task Definitions");
include('header.inc');

function get_task_list(){
    global $dbh;
    $ret = '';
    $sql = "SELECT task_code, task_description FROM admin_task  ORDER BY task_code";
    $rs  = $dbh->exec($sql);
    while ($rs->hasNext()){
        $row = $rs->next();
        $task_code = $row['task_code'];
        $task_description = $row['task_description'];
        $user_list = '';
        $sql = "SELECT aty_name FROM admin_user_type" .
            "    WHERE aty_id IN" .
            "        ( SELECT rule_aty_id FROM admin_access_rule" .
            "          WHERE rule_task_code = " . $dbh->prepareString($task_code) .')';
        $rs2 = $dbh->exec($sql);
        while ( $rs2->hasNext() ){
            $row2        = $rs2->next();
            $user_list .= $row2['aty_name'];
            if ( $row2 )
                $user_list .= ', ';
        }

        $task_code_link = '<a href="/access_control/tasks.php?op=Change&task_code=' . $task_code . '">' . $task_code . '</a>';
        $ret .= '<tr valign="top"><td class="content">' . $task_code_link . '</td>' .
            '    <td class="content">' . $task_description . '</td>' .
            '    <td width="300" class="content">' . $user_list . '</td></tr>';
    }

    return $ret;
}

function saveTask($code="",$description="", $user_types=""){
    global $dbh, $lang;

    if ( !$code ){
        addError('task_code',$lang->getLang("A unique task code must be entered"));
    }else{
        $count = $dbh->queryOne("SELECT count(*) FROM admin_task WHERE task_code = '$code'");
        if ( $count > 0 ){
            addError('task_code', $lang->getLang('The task code you entered already exists'));
        }
    }
    if ( !$description ){
        addError('task_description',$lang->getLang( 'A task description must be entered'));
    }

    if ( ! areErrors() ){
        $dbh->begin();
        if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
            betting_begin();
        }
        $sql = "INSERT INTO admin_task (task_code, task_description)" .
            " VALUES ( " . $dbh->prepareString($code) . ", " .
            $dbh->prepareString($description) . ")";
        $dbh->exec($sql);
        if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
           $result= betting_insert_task(escapeSingleQuotes($code),escapeSingleQuotes($description));
            if(!$result){
                addError("", "Database error");
                $dbh->rollback();
                betting_rollback();
                return;
            }
        }

        //delete access rules for this taks first
        $sql = "DELETE FROM admin_access_rule WHERE rule_task_code = " . $dbh->prepareString($code);
        $dbh->exec($sql);
        if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
            betting_delete_task($code);
        }

        //go through allowed admin users, and insert a rule record for each one (for this task)
        if ( is_array($user_types) ){
            foreach ( $user_types as $id =>$val){
                $sql  = "INSERT INTO admin_access_rule (rule_aty_id, rule_task_code)" .
                    " VALUES (" . $id . ", ". $dbh->prepareString($code) .")";
                $dbh->exec($sql);

                if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
                    $result2=betting_add_user_task($id,$code);
                    if(!$result2){
                        addError("", "Database error");
                        $dbh->rollback();
                        betting_rollback();
                        return;
                    }
                }
            }
        }
        $dbh->commit();
        if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
            betting_commit();
        }
        return TRUE;
    }
    return FALSE;
}

function updateTask($code="",$description="", $user_types=array()){
    global $dbh, $lang;
    if ( !$code ){
        addError('task_code', $lang->getLang('Missing task code must'));
        return FALSE;
    }

    if ( !$description ){
        addError('task_description', $lang->getLang('A task description must be entered'));
    }else {

        $dbh->begin();
        if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
            betting_begin();
        }
        $sql = "UPDATE admin_task" .
            " SET task_description = " . $dbh->prepareString($description) .
            " WHERE task_code = " . $dbh->prepareString($code);
        $dbh->exec($sql);

        if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
            $result=betting_update_tasks($code,$description);
        }
        //delete access rules for this taks first
        $sql = "DELETE FROM admin_access_rule WHERE rule_task_code = " . $dbh->prepareString($code);
        $dbh->exec($sql);
        if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
            betting_delete_task($code);
        }
        //go through allowed admin users, and insert a rule record for each one (for this task)
        if ( is_array($user_types) ){
            foreach ( $user_types as $id =>$val){
                $sql  = "INSERT INTO admin_access_rule (rule_aty_id, rule_task_code)" .
                    " VALUES (" . $id . ", ". $dbh->prepareString($code) .")";

                $dbh->exec($sql);
                if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
                    $result2=betting_add_user_task($id,$code);
                    if(!$result2){
                        addError("", "Database error");
                        $dbh->rollback();
                        betting_rollback();
                        return;
                    }
                }
            }
        }
        $dbh->commit();
        if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
            betting_commit();
        }
        return TRUE;
    }
    return FALSE;
}

function deleteTask($code="", $force=""){
    global $dbh,$lang;
    if ( !$code ){
        addError('task_code', $lang->getLang('Missing task code must'));
        return FALSE;
    }

    if ( ! $force ){
        //Check if this admin task is referenced in the admin_access_rule table
        $sql = "SELECT count(*) FROM admin_access_rule WHERE rule_task_code = " .
            $dbh->prepareString($code);
        $count = $dbh->queryOne($sql);
        if ( $count > 0 ){
            $msg = $lang->getLang('The task you are attempting delete has access rules referencing it.' .
                ' Please remove these references before deleting this task, or check "force"');
            addError('', $msg);
            return FALSE;
        }
    }
   $dbh->begin();
    if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
        betting_begin();
    }
    $sql = "DELETE FROM admin_task WHERE task_code = " . $dbh->prepareString($code);
    if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
        $result1=betting_delete_task($code);
        if(!$result1){
            addError("", "Database error");
            $dbh->rollback();
            betting_rollback();
            return;
        }
    }
    $dbh->exec($sql);
    $dbh->commit();
    if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting"){
        betting_commit();
    }
    return TRUE;
}

$op = ( !empty($_POST['op']) ? $_POST['op'] :$_GET['op'] );

switch ( $op ){
    case "Add": //display add new task screen
        $display =  "task_add_form.php";
        break;
    case "Change": //display add new task screen
        $task_code = getPostGetVar('task_code');
        if ( $task_code ){
            $sql = "SELECT task_code, task_description FROM admin_task" .
                " WHERE task_code = " . $dbh->prepareString($task_code);
            $rs = $dbh->exec($sql);
            $num_rows = $rs->getNumRows();

            if ( $num_rows > 0 ){
                $row = $rs->next();
                $task_code =  $row['task_code'];
                $task_description =  $row['task_description'];

                $sql = "SELECT aty_id, aty_name FROM admin_user_type" .
                    "    WHERE aty_id IN" .
                    "        ( SELECT rule_aty_id FROM admin_access_rule" .
                    "          WHERE rule_task_code = " . $dbh->prepareString($task_code) .')';

                $rs = $dbh->exec($sql);
                $num_rows = $rs->getNumRows();

                if ( $num_rows > 0 ){
                    while ( $row = $rs->next() ){
                        $id = $row['aty_id'];
                        $name = $row['aty_name'];
                        $user_types[$id] = $name;
                    }
                }
            }
        }
        $display =  "task_edit_form.php";
        break;

    case "Save":  //save a new task
        $added = saveTask($_POST['task_code'],  $_POST['task_description'], $_POST['user_types']);
        if ( TRUE == $added ){
            $display = 'task_display_form.php'; //display new record
            $alert = $lang->getLang("Task saved");
        }else{
            $display = "task_add_form.php"; //if error, display add form again
        }
        break;
    case "Update":  //update an existing task
        $updated = updateTask ($_POST['task_code'],  $_POST['task_description'], $_POST['user_types']);
        if ( $updated ){
            $alert = $lang->getLang("Task updated");
        }
        $task_code        =  $_POST['task_code'];
        $task_description =  $_POST['task_description'];
        $display =  'task_edit_form.php';
        break;
    case "Delete":  //delet an existing task
        $deleted = deleteTask($_POST['task_code'],$_POST['force']);
        if ( $deleted ){
            $alert = $lang->getLang("Task deleted");
        }
        $display =  'task_display_form.php';
        break;
    default:
        $display =  'task_display_form.php';
}
require_once($display);

if ( isset($alert) )
    jAlert($alert);
?>
<?php include 'footer.inc'; ?>
