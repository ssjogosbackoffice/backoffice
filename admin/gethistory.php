<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/style.css"  />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap-responsive.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<script src="<?= secure_host ?>/media/jquery.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<?php
$date_start= date("Y-m-d",time()-3600*24);
$date_end=  date("Y-m-d");
function searchAdminTransaction(){
    $starttime = microtime(true);
    global $dbh,$date_start, $date_end;
    $searchType = $_POST["field"];
    $sql="Select att_time,
                att_note,
                att_transaction_id,
                IF (att_tty_id IS NOT NULL, (select tty_description from transaction_type where tty_id = att_tty_id), 'N') tty_description,
                IF (att_aus_id IS NOT NULL, (select aus_username from admin_user where aus_id = att_aus_id), 'N') aus_username,
                IF (att_aus_final_id IS NOT NULL, (select aus_username from admin_user where aus_id = att_aus_final_id), 'N') aus_final_username,
                IF (att_pun_id IS NOT NULL, (select pun_username from punter where pun_id = att_pun_id), 'N') pun_username,
                IF (att_jur_id IS NOT NULL, (select jur_name from jurisdiction where jur_id=att_jur_id), 'N') first_jurisdiction,
                IF (att_final_jur_id IS NOT NULL, (select jur_name from jurisdiction where jur_id=att_final_jur_id), 'N') second_jurisdiction,
                att_amount,
                att_overdraft,
                'A' status,
                att_jur_avail_credit,
                att_jur_final_avail_credit,
                att_jur_avail_overdraft,
                att_jur_final_avail_overdraft
                FROM admin_user_transaction
                WHERE  1=1 ";

    $sql .= " AND att_aus_id  IN (SELECT aus_id FROM admin_user where aus_username=".$dbh->prepareString($_SESSION['admin_username']).")";
    if($searchType != 3){
        $sql .= " AND att_time BETWEEN '$date_start 00:00:00' AND '$date_end 23:59:59'";
    }

    $sql.=" UNION ALL
                    SELECT ptr_start att_time,
                            ptr_note att_note,
                            ptr_uniqueid att_transaction_id,
                            (select tty_description from transaction_type where tty_id = ptr_tty_id) tty_description,
                            IF (ptr_admin_user_id IS NOT NULL, (select aus_username from admin_user where aus_id = ptr_admin_user_id), 'N') aus_username,
                            'N' aus_final_username,
                            'N' pun_username,
                            ptr_ppc_code first_jurisdiction,
                            'N' second_jurisdiction,
                            ptr_amount att_amount,
                            0 att_overdraft,
                            ptr_status status,
                            ptr_available_credit att_jur_avail_credit,
                            0 att_jur_final_avail_credit,
                             ptr_available_overdraft att_jur_avail_overdraft,
                             0 att_jur_final_avail_overdraft
                            FROM processor_transaction
                             WHERE  1=1 ";

    $sql .= " AND ptr_admin_user_id  IN (SELECT aus_id FROM admin_user where aus_username=".$dbh->prepareString($_SESSION['admin_username']).")";
    if($searchType != 3){
        $sql .= " AND ptr_start >= '$date_start 00:00:00' AND ptr_end <= '$date_end 23:59:59'";
    }
    if(!areErrors()){
        $return="<table class='display table table-striped table-bordered table-hover table-condensed' id='historyTable'><thead>";
        $return.="<tr><th style='white-space: nowrap;'>Time</th>
                        <th style='white-space: nowrap; '>Transaction Type</th>
                        <th style='white-space: nowrap;'>Admin</th>
                        <th style='white-space: nowrap;'>Transaction Id</th>
                        <th style='white-space: nowrap;'>Amount</th>
                        <th style='white-space: nowrap;'>Overdraft</th>
                        <th style='white-space: nowrap;'>From</th>
                        <th style='white-space: nowrap;'>Available Credit</th>
                        <th style='white-space: nowrap;'>Available Overdraft</th>
                        <th style='white-space: nowrap;'>To</th>
                        <th style='white-space: nowrap;'>Available Credit</th>
                        <th style='white-space: nowrap;'>Available Overdraft</th>
                        <th>Note</th>
                  </tr></thead>";
        $rs=$dbh->doCachedQuery($sql,0);
        while ($rs->hasNext()){
            $row=$rs->next();
            $return.="<tr><td style='white-space: nowrap;'>".$row['att_time']."</td>
                        <td>".$row['tty_description']."</td>
                        <td>".$row['aus_username']."</td>
                        <td>".$row['att_transaction_id']."</td>
                       <td>".getInDollars($row['att_amount']);
                        $return.="<br> <span ";
                        if($row['status']=='A'){
                            $return .='class="result" >Accepted ';
                        }
                        elseif ($row['status']=='D'){
                            $return .='class="error">Declined ';
                        }
                        elseif($row['status']=='P'){
                            $return.= 'class="pending">Pending ';
                        }
                        $return.="</span></td>
                        <td>".getInDollars($row['att_overdraft'])."</td>
                        <td>".$row['first_jurisdiction']."</td>
                        <td>Before:".(($row["first_jurisdiction"]=="casino" ) ? " Unlimited" : getInDollars($row["att_jur_avail_credit"]))."<br / >After:&nbsp;&nbsp;&nbsp;".(($row["first_jurisdiction"]=="casino" ) ? " Unlimited" :getInDollars($row["att_jur_avail_credit"] - $row["att_amount"] ))."</td>";
            $return.="<td>Before:".(($row["first_jurisdiction"]=="casino" ) ? " Unlimited" : getInDollars($row["att_jur_avail_overdraft"]))."<br / >After:&nbsp;&nbsp;&nbsp;".(($row["first_jurisdiction"]=="casino" ) ? " Unlimited" :getInDollars($row["att_jur_avail_overdraft"] - $row["att_overdraft"] ))."</td>";
            $return.="<td>".$row['second_jurisdiction']."</td>";
            $return.="<td>Before:".(($row["second_jurisdiction"]=="casino" ) ? " Unlimited" : getInDollars($row["att_jur_final_avail_credit"]))."<br / >After:&nbsp;&nbsp;&nbsp;".(($row["second_jurisdiction"]=="casino" ) ? " Unlimited" :getInDollars($row["att_jur_final_avail_credit"] + $row["att_amount"] ))."</td>";
            $return.="<td>Before:".(($row["second_jurisdiction"]=="casino" ) ? " Unlimited" : getInDollars($row["att_jur_final_avail_overdraft"]))."<br / >After:&nbsp;&nbsp;&nbsp;".(($row["second_jurisdiction"]=="casino" ) ? " Unlimited" :getInDollars($row["att_jur_final_avail_overdraft"] + $row["att_overdraft"] ))."</td>";
            $return.="<td>".$row['att_note']."</td>
                    </tr>";

        }

        $return.="</table>




        ";

        die($return);
    }
    else{
        showErrors();
    }
}
 searchAdminTransaction();

;?>