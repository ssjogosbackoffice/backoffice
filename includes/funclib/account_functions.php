<?php
// add $amount to the available credit of an entity (and decrease the cash-in-hand by $amount)
function add_available_credit ($ent_id, $amount){
    global $dbh;
    $sql = "UPDATE jurisdiction " .
        "SET jur_cash_in_hand = jur_cash_in_hand - ($amount)," .
        "    jur_available_credit = jur_available_credit + ($amount)" .
        " WHERE jur_id = $ent_id";
    $rs=$dbh->exec($sql);
    return ( 1 == count($rs) );
}

// remove $amount from the available credit of an entity (and increase the cash-in-hand by $amount)
function subtract_available_credit ($ent_id, $amount){
    global $dbh;
    $sql = "UPDATE jurisdiction " .
        "SET jur_cash_in_hand = jur_cash_in_hand + $amount," .
        "    jur_available_credit = jur_available_credit - $amount" .
        " WHERE jur_id = $ent_id";
    $rs=$dbh->exec($sql);
    return ( 1 == count($rs) );
}

function add_available_overdraft($ent_id, $amount){
    global $dbh;
    $sql = "UPDATE jurisdiction " .
        "SET jur_overdraft = jur_overdraft + $amount" .
        " WHERE jur_id = $ent_id";
    $rs=$dbh->exec($sql);
    return ( 1 == count($rs) );
}

function subtract_available_overdraft ($ent_id, $amount){
    global $dbh;
    $sql = "UPDATE jurisdiction " .
        "SET jur_overdraft = jur_overdraft - $amount" .
        " WHERE jur_id = $ent_id";
    $rs=$dbh->exec($sql);
    return ( 1 == count($rs) );
}

// insert a credit transfer log record

function log_credit_transfer ($ent_id, $other_ent_id, $amount, $direction, $info=''){
    global $dbh;
    $sql = "INSERT INTO credit_transfer " .
        "(cre_entity_id, cre_other_entity_id, cre_amount, cre_time, cre_direction, cre_description,cre_id) " .
        "VALUES ($ent_id, $other_ent_id, $amount, CURRENT_TIMESTAMP, '$direction', '$info',null) ";
    $rs=$dbh->exec($sql);
    return ( 1 == count($rs) );
}


function add_affiliate_available_credit ($aff_id, $amount,$avail_overdraft){
    global $dbh;
    $sql = "UPDATE affiliate_credit " .
        "SET act_credits = act_credits + ($amount), act_total_deposit=act_total_deposit+($amount), act_overdraft=$avail_overdraft, act_last_deposit=NOW()  " .
        " WHERE act_aff_id = $aff_id";
    $rs=$dbh->exec($sql);
    return ( 1 == count($rs) );

}
function subtract_affiliate_credit ($aff_id, $amount,$avail_overdraft){
    global $dbh;
    $sql = "UPDATE affiliate_credit " .
        "SET act_credits = act_credits - ($amount), act_total_withdraw=act_total_withdraw+($amount),act_overdraft=$avail_overdraft, act_last_withdraw=NOW() " .
        " WHERE act_aff_id = $aff_id";
    $rs=$dbh->exec($sql);
    return ( 1 == count($rs) );

}

function add_affiliate_available_overdraft ($aff_id, $amount,$totOvd=0){
    global $dbh;
    $sql = "UPDATE affiliate_credit " .
        "SET act_overdraft = act_overdraft + ($amount)";
    if($totOvd==0){

        $sql.=" ,act_tot_overdraft_received=act_tot_overdraft_received+$amount ";

        $oldSql="Select act_tot_overdraft_received from affiliate_credit where act_aff_id = $aff_id";
        $oldJur=$dbh->queryRow($oldSql);
        doAdminUserLog($_SESSION['admin_id'],'overdraft modify','Increased total overdraft received  from '.$oldJur['act_tot_overdraft_received'].' to '.($oldJur['act_tot_overdraft_received']+$amount).' for affiliate '.$aff_id,'NULL',getIpAddress());
    }
    $sql.=  " WHERE act_aff_id = $aff_id";
    $rs=$dbh->exec($sql);
    return ( 1 == count($rs) );
}

function subtract_affiliate_overdraft ($aff_id, $amount,$reset=0,$totOvd=0){
    global $dbh;
    $oldSql="Select act_tot_overdraft_received,act_overdraft_start_time from affiliate_credit where act_aff_id = $aff_id";
    $oldJur=$dbh->queryRow($oldSql);

    $sql = "UPDATE affiliate_credit " .
        "SET act_overdraft = act_overdraft - ($amount) ";
          if($totOvd==0){

        $sql.=" ,act_tot_overdraft_received=act_tot_overdraft_received - $amount ";

        doAdminUserLog($_SESSION['admin_id'],'overdraft modify','Decreased total overdraft received  from '.$oldJur['act_tot_overdraft_received'].' to '.($oldJur['act_tot_overdraft_received']-$amount).' for affiliate '.$aff_id,'NULL',getIpAddress());
    }

    if($reset!=0){
        $sql.=",act_overdraft_start_time= NULL ";
        doAdminUserLog($_SESSION['admin_id'],'overdraft modify','Reset overdraft time for affiliate '.$aff_id,'NULL',getIpAddress());
    }
    $sql.= " WHERE act_aff_id = $aff_id";
    $rs=$dbh->exec($sql);
    return ( 1 == count($rs) );
}

function update_jurisdiction_credits ($ent_id, $amount,$overdraft,$total_overdraft,$resetovd=0){
    global $dbh;
    if(!$amount || $amount==''){
        $amount=0;
    }
    $oldSql="Select jur_overdraft_start_time,jur_tot_overdraft_received,jur_name from jurisdiction where jur_id = ".$dbh->escape($ent_id);
    $oldJur=$dbh->queryRow($oldSql);

    $sql = "UPDATE jurisdiction " .
        "SET jur_cash_in_hand = $amount," .
        "    jur_available_credit = $amount " ;
        if($overdraft || $overdraft==0){
        $sql.=" ,jur_overdraft = $overdraft ";
        }
    if($total_overdraft || $total_overdraft==0){
     $sql.= " ,jur_tot_overdraft_received = $total_overdraft ";
      if($total_overdraft!=$oldJur['jur_tot_overdraft_received']){
        doAdminUserLog($_SESSION['admin_id'],'overdraft modify','Changed total overdraft received  from '.$oldJur['jur_tot_overdraft_received'].' to '.$total_overdraft.' for jurisdiction '.$oldJur['jur_name'],'NULL',getIpAddress());
      }
    }

    if($resetovd!=0){
        if($resetovd==1){
            $sql.=",jur_overdraft_start_time= now() ";
            doAdminUserLog($_SESSION['admin_id'],'overdraft modify','Changed overdraft time from '.$oldJur['jur_overdraft_start_time'].' to NOW() for jurisdiction '.$oldJur['jur_name'],'NULL',getIpAddress());
        }
        else{
            $sql.=",jur_overdraft_start_time= NULL ";
            doAdminUserLog($_SESSION['admin_id'],'overdraft modify','Reset overdraft time for jurisdiction '.$oldJur['jur_name'],'NULL',getIpAddress());
        }

    }
    $sql .=" WHERE jur_id = $ent_id";
    $rs=$dbh->exec($sql);
    return ( 1 == count($rs) );
}

function setOverdraftTime($jurisdiction,$time){
    global $dbh;
    $oldSql="Select jur_overdraft_start_time,jur_name from jurisdiction where jur_id = ".$dbh->escape($jurisdiction);
    $oldJur=$dbh->queryRow($oldSql);
    $sql = "UPDATE jurisdiction " .
        "SET jur_overdraft_start_time = ".($time==0 ? 'NULL' :$dbh->prepareString($time)) ." WHERE jur_id = ".$dbh->escape($jurisdiction);
    $rs=$dbh->exec($sql);
    doAdminUserLog($_SESSION['admin_id'],'overdraft modify','Changed overdraft time from '.$oldJur['jur_overdraft_start_time'].' to '.($time==0 ? 'NULL' :$dbh->escape($time)).' for jurisdiction '.$oldJur['jur_name'],'NULL',getIpAddress());
    return ( 1 == count($rs) );
}


function do_cash_deposit ($amount, $customer_id, $aus_id,  $club_id,$trnsId,$type=false) {
    global $dbh, $console_port;
    $dbh->begin();
    // perform credit to customer account
    $deposited = doDeposit($customer_id, $amount);

    if ( FALSE == $deposited ) {
        $dbh->rollback();
        return false;
    }

    $customer_row = getCustomer($customer_id,false,true);
    $betting_club = $customer_row['betting_club'];
    $customer_credits=$customer_row['credits'];
    // tie customer to this betting club if is not tied already

    // insert customer transaction record

    $ctr_id = $dbh->nextSequence('ctr_id_seq');
    //$transaction_number = nextTxnNumberCashdesk();
    $transaction_number=$trnsId;

    $sql  = "INSERT INTO customer_transaction" .
        " ( ctr_id, ctr_pun_id, ctr_amount,ctr_balance_available" .
        ", ctr_tty_id, ctr_pme_code, ctr_status" .
        ", ctr_aus_id, ctr_time, ctr_transaction_num,ctr_notes)" .
        "  VALUES ($ctr_id, $customer_id, $amount,$customer_credits" .
        ", ".($type ? $type:T_TYPE_DEPOSIT).", 'CSH', 'completed'" .
        ", $aus_id,  CURRENT_TIMESTAMP, '$transaction_number','".$dbh->escape($_POST['note'])."')";

    $dbh->exec($sql);

    if ( $dbh->AffectedRows != 1 ) {
        $dbh->rollback();
        return false;
    }

    ////
    // adjust jurisdiction records
    ////

    $sql = "SELECT jur_id from jurisdiction WHERE jur_code = 'CUS'";
    $rs  = $dbh->exec($sql);

    $row = $rs->next();
    $cus_jur_id = $row['jur_id'];

    // update the club account's balance
    $club_rec_updated = subtract_available_credit($club_id, $amount);

    // update the dummy custoemr account's balance
    $cus_rec_updated  = add_available_credit($cus_jur_id, $amount);

    // check that the club and customer dummy jurisdiction records were Sinserted

    if ( FALSE == $club_rec_updated && FALSE == $cus_rec_updated ) {
        $dbh->rollback();
        return false;
    }

    ////
    // log the credit transfer
    ////

    //this is only reached if all inserts and updates were successful
    $dbh->commit();

    //sendPunterCredits($customer_id, $console_port);

    return $transaction_number;
}


function do_cash_withdrawal($amount, $customer_id,  $aus_id, $club_id,$trnsId,$type=false){
    global $dbh, $console_port;
    $dbh->begin();

    settype($customer_id, "integer");
    $sql    = "SELECT pun_access FROM punter WHERE pun_id=$customer_id";
    $access = $dbh->queryOne($sql);
    if (strtoupper($access) != "ALLOW") {
        $dbh->rollback();
        return false;
    }

    settype($amount, "float");
    // perform credit to customer account
    $withdrawn = doWithdrawal($customer_id, $amount);
    if ( FALSE === $withdrawn ) {
        $dbh->rollback();
        return false;
    }

    $customer_row = getCustomer($customer_id);
    $betting_club = $customer_row['betting_club'];
    // tie customer to this betting club if is not tied already
    $customer_credits=$customer_row['credits'];
    if ( ! $betting_club ) {
        $dbh->exec("update punter set pun_betting_club = ".$club_id." where pun_id = $customer_id");

        if ( $dbh->AffectedRows != 1 ) {
            $dbh->rollback();
            return false;
        }
    }

    // insert customer transaction record

    $ctr_id = $dbh->nextSequence('ctr_id_seq');
    //$transaction_number = nextTxnNumberCashdesk();
    $transaction_number=$trnsId;

    $neg_amount = $amount * -1;

    $sql  = "INSERT INTO customer_transaction" .
        " ( ctr_id, ctr_pun_id, ctr_amount,ctr_balance_available" .
        ", ctr_tty_id, ctr_pme_code, ctr_status" .
        ", ctr_aus_id, ctr_time, ctr_transaction_num,ctr_notes)" .
        "  VALUES ($ctr_id, $customer_id, $neg_amount,$customer_credits" .
        ", ".($type?$type:T_TYPE_WITHDRAWAL).", 'CSH', 'completed'" .
        ", $aus_id,  CURRENT_TIMESTAMP, '$transaction_number','".$dbh->escape($_POST['note'])."')";
    $dbh->exec($sql);

    if ( $dbh->AffectedRows != 1 )
    {
        $dbh->rollback();
        return false;
    }

    $sql = "SELECT jur_id from jurisdiction WHERE jur_code = 'CUS'";
    $rs  = $dbh->exec($sql);

    $row    = $rs->next();
    $cus_jur_id = $row['jur_id'];


    $club_rec_updated = add_available_credit($club_id, $amount);

    $cus_rec_updated  = subtract_available_credit($cus_jur_id, $amount);

    $row    = $rs->next();


    if ( FALSE == $club_rec_updated && FALSE == $cus_rec_updated ) {
        $dbh->rollback();
        return false;
    }

    $dbh->commit();

    return $transaction_number;
}


function getAllEntitiesBetween($jurisdictionClass,$jurisdictionPostClass,$jurisdiction=false,$customer_id=false){
    global $dbh;
    if(!$jurisdiction){
        $customer_row = getCustomer($customer_id,false,true);
        $jurisdiction = $customer_row['betting_club'];
    }
    switch ($jurisdictionClass)
    {
        case 'casino':
            $params =" ca.jur_id as casino,n.jur_id as nation,r.jur_id as region,d.jur_id as district,c.jur_id as club ";
            $from = " FROM jurisdiction ca
                        LEFT JOIN jurisdiction n on ca.jur_id = n.jur_parent_id
                        LEFT JOIN jurisdiction r ON n.jur_id = r.jur_parent_id
                        LEFT JOIN jurisdiction d on r.jur_id = d.jur_parent_id
                        LEFT JOIN jurisdiction c on d .jur_id =c.jur_parent_id  ";
            break;
        case 'nation':
            $params =" n.jur_id as nation,r.jur_id as region,d.jur_id as district,c.jur_id as club ";
            $from = "FROM
                        jurisdiction n
                        LEFT JOIN jurisdiction r ON n.jur_id = r.jur_parent_id
                        LEFT JOIN jurisdiction d on r.jur_id = d.jur_parent_id
                        LEFT JOIN jurisdiction c on d .jur_id =c.jur_parent_id  ";
            break;
        case 'region':
            $params =" r.jur_id as region,d.jur_id as district,c.jur_id as club ";
            $from = " FROM
                        jurisdiction r
                        LEFT JOIN jurisdiction d on r.jur_id = d.jur_parent_id
                        LEFT JOIN jurisdiction c on d .jur_id =c.jur_parent_id  ";
            break;
        case 'district':
            $params =" d.jur_id as district,c.jur_id as club ";
            $from = " FROM  jurisdiction d
                        LEFT JOIN jurisdiction c on d .jur_id =c.jur_parent_id  ";
            break;
        default:
            $params =" c.jur_id as club ";
            $from = " FROM jurisdiction c";
    }

    $params=explode(',',$params);
    switch ($jurisdictionPostClass ){
        case 'nation':
            $params=array_slice($params,0,count($params)-3);
            $where=" Where n.jur_id=".$jurisdiction;
            $group=" Group by n.jur_id";
            break;
        case 'region':
            $params=array_slice($params,0,count($params)-2);
            $where=" Where r.jur_id=".$jurisdiction;
            $group=" Group by r.jur_id";
            break;
        case 'district':
            $params=array_slice($params,0,count($params)-1);
            $where=" Where d.jur_id=".$jurisdiction;
            $group=" Group by d.jur_id";
            break;
        default:
            $where=" Where c.jur_id=".$jurisdiction;
            $group=" Group by c.jur_id";

    }

    $params=implode(',',$params);
    $sql ="SELECT $params $from $where $group";
    $result=$dbh->queryRow($sql);

    return $result;

}

function adminCreditLog($note='',$transactionType,$admId,$admJurId,$admFinalJurId=false,$pun_id=false,$admAmount,$mult=0,$trnsId)
{
    global $dbh;
    $sql="Insert into admin_user_transaction (att_note,att_tty_id,att_aus_id,";
    if($pun_id){
        $sql.=" att_pun_id,";
    }
    $sql.=" att_transaction_id, att_jur_id,";
    if($admFinalJurId){
        $sql.=" att_final_jur_id,";
    }
    $sql.=" att_amount,att_jur_avail_credit,att_jur_avail_overdraft,att_jur_final_avail_credit ";
    if($admFinalJurId){
        $sql.=" ,att_jur_final_avail_overdraft ";
    }
    $sql.=" )
                                        Values (
                                                ".$dbh->prepareString($note).",
                                                ".$dbh->escape($transactionType).",
                                                ".$dbh->escape($admId).",";
    if($pun_id){
        $sql.=$dbh->escape($pun_id).",";
    }
    $sql.= $dbh->prepareString($trnsId).",
                                                ".$dbh->escape($admJurId).",";
    if($admFinalJurId){
        $sql.=$dbh->escape($admFinalJurId).",";
    }
    $sql.=$dbh->prepareString($admAmount).",
                                           coalesce((SELECT jur_available_credit from jurisdiction where jur_id =".$dbh->escape($admJurId)."),0)";
    if($mult!=0 && ($transactionType==19 ||$transactionType==25)){
        $sql.="+".$dbh->prepareString($admAmount);
    }
    $sql.=" , coalesce((SELECT jur_overdraft from jurisdiction where jur_id =".$dbh->escape($admJurId)."),0),";
    if($admFinalJurId){
        $sql.="coalesce((SELECT jur_available_credit from jurisdiction where jur_id =".$dbh->escape($admFinalJurId)."),0)";
        if($transactionType==20 || $transactionType==26){
            $sql.="-".$dbh->prepareString($admAmount);
        }
    }
    else{
        $sql.=" (SELECT pcr_credits from punter_credit where pcr_pun_id =".$dbh->escape($pun_id).")";
    }
    if($admFinalJurId){
        $sql.=" ,coalesce((SELECT jur_overdraft from jurisdiction where jur_id =".$dbh->escape($admFinalJurId)."),0)";
    }

    $sql.=")";
    $result=$dbh->exec($sql);
    return  $result;
}

function adminUserCreditLogMultiple($note='',$transactionType,$admId,$admJurId,$admFinalJurId,$amount,$overdraft,$mult=0)
{
    global $dbh,$trnsId;
    $sql="Insert into admin_user_transaction (att_note,
                                                att_tty_id,
                                                att_aus_id,
                                                att_transaction_id,
                                                att_jur_id,
                                                att_final_jur_id,
                                                att_amount,
                                                att_overdraft,
                                                att_jur_avail_credit,
                                                att_jur_avail_overdraft,
                                                att_jur_final_avail_credit,
                                                att_jur_final_avail_overdraft )
                                        Values (
                                                ".$dbh->prepareString($note).",
                                                ".$dbh->prepareString($transactionType).",
                                                ".$dbh->prepareString($admId).",
                                                ".$dbh->prepareString($trnsId).",
                                                ".$dbh->prepareString($admJurId).",
                                                ".$dbh->prepareString($admFinalJurId).",
                                                ".$dbh->prepareString($amount).",
                                                ".$dbh->escape($overdraft).",
                                                    (SELECT jur_available_credit from jurisdiction where jur_id =".$dbh->prepareString($admJurId).")";
    if($mult!=0 && ($transactionType==19 || $transactionType==21)){
        $sql.="+".$dbh->prepareString($amount);
    }
    $sql .=", (SELECT jur_overdraft from jurisdiction where jur_id =".$dbh->prepareString($admJurId)."),";
    $sql.="(SELECT jur_available_credit from jurisdiction where jur_id =".$dbh->prepareString($admFinalJurId).")";
    if($mult!=0 && $transactionType==20){
        $sql.="-".$dbh->prepareString($amount);
    }
    $sql .=", (SELECT jur_overdraft from jurisdiction where jur_id =".$dbh->prepareString($admFinalJurId)."))";
    $result=$dbh->exec($sql);
    return  $result;
}



?>