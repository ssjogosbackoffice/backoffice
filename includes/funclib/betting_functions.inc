<?php
function betting_update_tasks($code,$description){
    $sql = "UPDATE admin_task" .
        " SET task_description = '" . $description .
        "' WHERE task_code = '". $code."'";
    $result = Db::execute($sql);
    return $result;
}


function betting_delete_task($code){
    $sql = "DELETE FROM admin_access_rule WHERE rule_task_code = '" . $code."'";
    $result = Db::execute($sql);
    return $result;
}

function betting_add_user_task($user, $code){
    $sql  = "INSERT INTO admin_access_rule (rule_aty_id, rule_task_code)" .
        " VALUES (" . $user . ", '". $code ."')";
    $result = Db::execute($sql);
    return $result;
}

function betting_insert_task($code, $description){
    $sql = "INSERT INTO admin_task (task_code, task_description)" .
        " VALUES ( '" . $code . "', '" .
        $description . "')";
    $result = Db::execute($sql);
    return $result;
}

function betting_insert_admin_user($next_id,$code,$name,$level,$class,$code){
    $sql = "INSERT INTO admin_user_type (aty_id, aty_code, aty_name, aty_level, aty_jurisdiction_class)" .
        " VALUES (" . $next_id . ", '" . $code . "', '" . $name .
        "', '" . $level . "', '" . $class . "')";
    $result = Db::execute($sql);
    return $result;
}

function betting_delete_admin_user($id){
    $sql = " DELETE FROM admin_user_type WHERE aty_id = " . $id;
    $result = Db::execute($sql);
    return $result;
}

function betting_update_admin_user($id, $code, $name, $jurisdiction_class, $level){
    $sql = "UPDATE admin_user_type " .
        "SET aty_code = '" . $code .
        "', aty_name = '" . $name .
        "', aty_jurisdiction_class = '" .$jurisdiction_class .
        "', aty_level = '" . $level .
        "' WHERE aty_id = " . $id;
    $result = Db::execute($sql);
    return $result;
}


function betting_update_user($profile, $customer_id){
    $sql = "Update Utenti set IDUtenteGruppo=$profile, TStampLogin = NOW() where IDUtente= $customer_id";
    $result = Db::execute($sql);
    return $result;
}


function save_betting_jurisdiction($jur_id, $jur_name,$jur_parent_id, $jur_class){
    global $dbh;
    $sql="INSERT INTO jurisdiction (jur_id,jur_name,Username,Password,jur_parent_id,jur_class,Attivo,jur_data_creation) VALUES
                        ($jur_id,'$jur_name','$jur_name','".md5($jur_name.strtotime('now'))."',$jur_parent_id,'$jur_class',1,NOW());";
    $result =  Db::execute($sql);
    if($result){
        $sql = "INSERT INTO jurisdiction_services (id_jurisdiction,abilita_bet) VALUES ($jur_id,1);";

        $result =  Db::execute($sql);


        if($result){
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
    return false;
}
function update_betting_jurisdiction($jur_id, $jur_name,$jur_parent_id, $jur_class){
    $sql = "UPDATE jurisdiction SET jur_name='$jur_name',Username='$jur_name',jur_parent_id=$jur_parent_id, Attivo = 1, LastUpdate = NOW() WHERE jur_id=$jur_id";
    return  Db::execute($sql);
}
function save_betting_percentuals($type, $node){
    global $dbh;
    $sql="DELETE FROM ProfiliCommissioni WHERE jur_id= $node";
    $result = Db::execute($sql);
    switch($type){
        case 1:
            $sql  = "INSERT INTO ProfiliCommissioni (IDProfiloCommissioneTipo,jur_id,Raggruppamento,Commissione) VALUES (1,$node,0,".$_POST["commisionbanco"].")";
            break;
        case 3:
            $sql  = "INSERT INTO ProfiliCommissioni (IDProfiloCommissioneTipo,jur_id,Raggruppamento,Commissione) VALUES (3,$node,0,".$_POST["commisionbancoplus"].")";
            break;
        case 2:
            $sql  = "INSERT INTO ProfiliCommissioni (IDProfiloCommissioneTipo,jur_id,Raggruppamento,Commissione) VALUES ";
            $y = 0;
            for($i=0; $i < count($_POST["commissions"]); $i++){
                $y++;
                $sql .= " (2,$node,". $y .",".$_POST["commissions"][$i]."),";
            }
            $sql  = substr($sql, 0 ,strlen($sql)-1);
            break;
        case 4:
            $sql = "INSERT INTO ProfiliCommissioni (IDProfiloCommissioneTipo,jur_id,Raggruppamento,Commissione) VALUES (4,$node,0,".$_POST["bonusmensile"]."),";
            $y = 0;
            for($i=0; $i < count($_POST["monthly"]); $i++){
                $y++;
                $sql .= "  (4,$node,".$y.",".$_POST["monthly"][$i]."),";
            }
            $sql = substr($sql, 0 ,strlen($sql)-1);
            break;
    }
    $result = false;
    try{
        $result = Db::execute($sql);
    }catch(PDOException $e){
        $$result = false;
    }
    return $result;

}
function betting_begin(){
    Db::beginTransaction();
}
function betting_commit(){
    Db::commitTransaction();
}
function betting_rollback(){
    Db::rollbackTransaction();
}

?>