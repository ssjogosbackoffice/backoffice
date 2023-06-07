<?php
require_once 'RedisClient.php';
if(isset($_REQUEST['pk'])){
    global $lang;
    $code=$_POST['code'];
    if(is_array($_POST['code'])){
        $code=$_POST['code'][0];
    }
    switch($_REQUEST['pk'])
    {
        case "getTotemTickets":
            try{
                if(!$_REQUEST['startDate']){
                    throw new Exception($lang->getLang('Invalid Start Date'));
                }
                else{
                    $return= getTotemsTickets($_REQUEST['totems'],$_REQUEST['startDate'],$_REQUEST['endDate'],$_REQUEST['ticketCode'],$_REQUEST['status']);

                }
            }
            catch(Exception $e){
                error_log($e->getMessage());
                $return=-1;
            }
            die($return);
            break;

        case "payTicket":
            try{
                if(!$_REQUEST['ticket']){
                    throw new Exception($lang->getLang('Invalid Ticket Code'));
                }
                else{
                    $return= payTotemTicket($_REQUEST['ticket']);
                    if($return){
                        $return="1";
                    }
                }
            }
            catch(Exception $e){
                error_log($e->getMessage());
                $return=-1;
            }
            die($return);
            break;

        case "getPendingTickets":
        {

           die(getPendingTickets($_REQUEST['startDate']." 00:00:00",$_REQUEST['endDate']." 23:59:59",$_REQUEST['status']));
            break;
        }
        case "displayDeviceInfo":
        {
            die(displayDeviceInfoTable($_POST['jurId']));
            break;
        }
        case "getTotemCodeList":
        {
            die(getTotemCodeList($_POST['jurCode']));
            break;
        }
        case "addNewTotemDevice":
        {
            die(addNewTotem($_POST['ttmCode'], $_POST['ttmInfo'], $_POST['ttmClubId']));
            break;
        }
        case "deleteDevice":
        {
            die(deleteDevice($_POST['entryId']));
            break;
        }
        case "editDeviceInfo":
        {
            die(editDevice($_POST['ttmCode'], $_POST['ttmInfo'], $_POST['ttmID']));
            break;
        }
        case "deleteEntryFromCache":
        {
            die(deleteItemFromCache($_POST['ttmCode']));
            break;
        }


        default:
            die('Invalid request');
            break;
    }
}


//deletes a totem from cache
function deleteItemFromCache($totemCode) {
    // create the key to update redis
    $dbLog = Logger::getLogger("DbLogger");
    $redisClient=new RedisClient(REDISSERVER,REDISPORT,$dbLog);
    $keyword = "TTM_INFO_" . $totemCode;

    // deletes from redis
    $redisClient->delete($keyword);
}

//edits the device Info
function editDevice($totemCode, $totemInfo, $totemId) {
    global $dbh;

    $totemCode = ($totemCode == 0) ? 'NULL' : $totemCode;
    $date = new DateTime();
    $formattedDateWithTimestamp = $date->format('Y-m-d H:i:s');

    $sql = "Update totem_info set tti_ttm_id = $totemCode, tti_last_update = '$formattedDateWithTimestamp' , tti_info = '$totemInfo' where tti_id = $totemId";
    $response = $dbh->exec($sql);
    $response = "".$response;
    return $response;
}

//function that formats objects and arrays, used mainly to view formatted data
function preprint($obj){
    print "<pre>". print_r($obj, true) ."</pre>";
}

function deleteDevice($deviceId) {
    global $dbh;

    $sql = "DELETE FROM totem_info WHERE TTI_ID = '$deviceId'";

    $response = $dbh->exec($sql);
    $response = "".$response;
    return $response;
}

function addNewTotem($totemCode, $totemInfo, $totemClubId) {
    global $dbh;

    // Set the variables to null if they are empty
    $totemCode = !empty($totemCode) ?  $totemCode : "NULL";
    $totemCode = ($totemCode == 0) ? "NULL" : $totemCode;
    $totemInfo = !empty($totemInfo) ?  $totemInfo : "NULL";

    //timestamped date
    $date = new DateTime();
    $formattedDateWithTimestamp = $date->format('Y-m-d H:i:s');

    $sql = "INSERT INTO totem_info (TTI_CLUB_ID, TTI_TTM_ID, TTI_REGISTRATION, TTI_LAST_UPDATE, TTI_INFO) VALUES ($totemClubId, $totemCode, '$formattedDateWithTimestamp', '$formattedDateWithTimestamp', '$totemInfo' )";
    $result = $dbh->exec($sql);
    return $result;
}

function getTotemCodeList($jurCode) {
    global $dbh;
    $sql = "select ttm_id, ttm_code  from totem, jurisdiction where jur_id = ttm_club_id and jur_code = '$jurCode'";
    $result = $dbh->doCachedQuery($sql, 0);
    if($result->getNumRows() > 0) {

        $resultHtml = "<select id='ttm_code'><option value='0'>-</option>";
        while ($result->hasNext()) {
            $row = $result->next();
            $totemId = $row['ttm_id'];
            $totemCode = $row['ttm_code'];
            $resultHtml .= "<option value='$totemId'>$totemCode</option>";
        }
        $resultHtml .= "</select>";
    } else {
        $resultHtml = "<select><option>None</option></select>";
    }
    return $resultHtml;
}

function displayDeviceInfoTable($jurId) {
    global $dbh;
    $sql = "select tti_id, tti_club_id, tti_ttm_id, TTI_REGISTRATION, TTI_LAST_UPDATE, TTI_INFO, 
            coalesce(ttm_code, '-'), ttm_id from totem_info 
            left join totem ON tti_ttm_id = ttm_id
            where tti_club_id = $jurId";
    $result = $dbh->doCachedQuery($sql, 0);
    if($result->getNumRows() > 0) {
        $resultHtml = "<table class='table display'><thead><tr>
                         <th>Totem Code</th>
                         <th>Registration</th>
                         <th>Last update</th>
                         <th>Info</th>
                         <th>Operation</th>
                      </tr></thead>";
        while($result->hasNext()) {

            $row = $result->next();
            $totemInfoId = $row['tti_id'];
            $totemId = $row['ttm_id'];
            //$totemCode = !empty($row['ttm_code']) ? $row['ttm_code'] : "-";
            $totemCode = $row["coalesce(ttm_code, '-')"];
            $resultHtml .= "<tr>";
            $resultHtml .= "<td>".$totemCode."</td>";
            $resultHtml .= "<td>".$row['tti_registration']."</td>";
            $resultHtml .= "<td>".$row['tti_last_update']."</td>";
            //$resultHtml .= "<td>".$row['tti_info']."</td>";
            $info  = $row['tti_info'];
            $resultHtml .= "<td><button class ='opener ui-state-default' data-info ='$info'>View Info</button></td>";
            $resultHtml .= "<td> <button class=' ui-state-default editTotem' data-info ='$info' data-id='$totemId' data-tti-id='$totemInfoId'>Edit</button><button class=' ui-state-error deleteTotem' data-id='$totemInfoId'>Delete</button></td>";
        }
        $resultHtml .= "</table>";
    } else {
      $resultHtml = "<div class='err'>No result found</div>";
    }
    return $resultHtml;
}

function getPendingTickets($startDate,$endDate,$status){
    global $dbh,$lang;
    $sql="Select * from totem_credit_account_reg 
          where ((tca_last_update BETWEEN  ".$dbh->prepareString($startDate)."  AND ".$dbh->prepareString($endDate)." )  
           OR     (tca_time BETWEEN  ".$dbh->prepareString($startDate)."  AND ".$dbh->prepareString($endDate)." ))";
       if($status>=0){ $sql.=   "AND tca_status=".$dbh->escape($status); }
    $result=$dbh->doCachedQuery($sql,0);
   if($result->getNumRows()>0){
      $resultHtml="<table class='table display'><thead><tr>
                    <th>Creation</th>
                    <th>Last Modify</th>
                    <th>TicketID</th>
                    <th>Credits</th>
                    <th>Points</th>
                    <th>Status</th>
                    <th>Type</th>
                    </tr></thead>";
       while($result->hasNext()) {
           $row = $result->next();
           $resultHtml.="<tr>";
           $resultHtml.="<td>".$row['tca_time']."</td>";
           $resultHtml.="<td>".($row['tca_last_update']!=''?$row['tca_last_update']:$lang->getLang('N'))."</td>";
           $resultHtml.="<td>".$row['tca_code']."</td>";
           $resultHtml.="<td>".$row['tca_credits']."</td>";
           $resultHtml.="<td>".$row['tca_points']."</td>";
           $resultHtml.="<td>".($row['tca_status']==0 ? "<span class='result'>Open</span>" :($row['tca_status']==1? "<span class='pending'>Reused</span>":"<span class='error'>Closed</span>" ))."</td>";
           $resultHtml.="<td>".($row['tca_type']==1?"Virtual cashout ticket":"Internet fee")."</td>";
           $resultHtml.="</tr>";
       }
       $resultHtml.="</table>";
   }
   else{
       $resultHtml="<div class='err'>No result found</div>";
   }

    return $resultHtml;

}



function getTotemsTickets($totems,$startDate,$endDate=false,$code=false,$status=3){
    global $dbh,$lang;
    if(!$endDate){
        $endDate=$startDate;
    }
    $totems=json_decode($totems);
    if(is_array($totems)){
        $totems=implode("','",$totems);
    }


    $columns = array('ttm_code','pun_username','tca_code', 'tca_time','tca_last_update', 'tca_credits', 'tca_points' ,'tca_status','pun_id');
    $sTable = " casinogames.totem_credit_account_reg, casinogames.punter, casinogames.jurisdiction c, casinogames.jurisdiction d,
          casinogames.jurisdiction r, casinogames.jurisdiction n, casinogames.totem ";

    $limit = '';
    if ( isset($_REQUEST['start']) && $_REQUEST['length'] != -1 ) {
        $limit = "LIMIT ".intval($_REQUEST['start']).", ".intval($_REQUEST['length']);
    }
    $order = '';
    
    if ( isset($_REQUEST['order']) && count($_REQUEST['order']) ) {
        $orderBy = array();
        for ( $i=0, $ien=count($_REQUEST['order']) ; $i<$ien ; $i++ ) {
            $columnIdx = intval($_REQUEST['order'][$i]['column']);
            $_REQUESTColumn = $_REQUEST['columns'][$columnIdx];
            $column = $columns[ $columnIdx ];

            if ( $_REQUESTColumn['orderable'] == 'true' ) {
                $dir = $_REQUEST['order'][$i]['dir'] === 'asc' ?
                    'ASC' :
                    'DESC';
                $orderBy[] = '`'.$column.'` '.$dir;
            }
        }
        $order = 'ORDER BY '.implode(', ', $orderBy);
    }

    $sWhere=" where tca_ttm_pun_id = pun_id
          and ttm_id = tca_ttm_id
          and pun_betting_club = c.jur_id
          and c.jur_parent_id = d.jur_id
          and d.jur_parent_id = r.jur_id
          and r.jur_parent_id = n.jur_id
          and n.jur_parent_id = 1
          and ttm_code  IN ('$totems')
          and tca_time between ".$dbh->prepareString($startDate." 00:00:00")." and ".$dbh->prepareString($endDate." 23:29:59");
    if($code){
        $sWhere.=" and tca_code = ".$dbh->prepareString($code);
    }
    if($status!=3){
        $sWhere.=" and tca_status = ".$dbh->escape($status);
    }

    switch($_SESSION["jurisdiction_class"]){
        case "club":
            $sWhere .= " and c.jur_id =". $_SESSION["jurisdiction_id"];
            break;
        case "district":
            $sWhere .= " and d.jur_id =". $_SESSION["jurisdiction_id"];
            break;
        case "region":
            $sWhere .= " and r.jur_id =". $_SESSION["jurisdiction_id"];
            break;
        case "nation":
            $sWhere .= " and n.jur_id =". $_SESSION["jurisdiction_id"];
            break;
    }



    $str = $_REQUEST['search']['value'];
    if ( isset($_REQUEST['search']) && $_REQUEST['search']['value'] != '' ) {
        $extrasWhere = " AND ";
        $extrasWhere .=" ticket code  LIKE '%".mysql_real_escape_string( $str )."%' ";
    }
    $sQuery = "  SELECT SQL_CALC_FOUND_ROWS `".str_replace(" , ", " ", implode("`, `", $columns))."`
    FROM   $sTable
    $sWhere
    $extrasWhere
    $order
    $limit";

    $rResult = $dbh->exec($sQuery)  ;

    $sQuery = " SELECT FOUND_ROWS() ";
    $iFilteredTotal = $dbh->queryOne($sQuery) ;
    $rResultFilterTotal = $dbh->queryOne("SELECT count(*) from $sTable $sWhere");

    $tableArray=array();
    while($rResult->hasNext()){
        $row=$rResult->next();
        $section=array();
        if($row["tca_status"]==0){
            $status= "<span class='text-warning'>".$lang->getLang('Open')."</span>";
        }
        elseif($row["tca_status"]==1){
            $status= "<span class='text-success'>".$lang->getLang('Reused')."</span>";
        }
        else{
            $status= "<span class='text-info'>".$lang->getLang('Withdrawn')."</span>";
        }
        $section[0]=$row['ttm_code'];
        $section[1]="<a target='_blank' href='/?page=customers/customer_view&customer_id=".$row['pun_id']."'>".$row["pun_username"]."</a>";
        $section[2]=$row['tca_code'];
        $section[3]=$row['tca_time'];
        $section[4]=$row['tca_last_update'];
        $section[5]=getInDollars($row["tca_credits"]);
        $section[6]=$row["tca_points"];
        $section[7]=$status;
        $section[8]=($row['tca_status']==0)? "<button class='btn btn-mini btn-success payTicket'>".$lang->getLang('Pay')." ".getInDollars($row["tca_credits"]+$row["tca_points"])."</button>" : "<span class='muted'>No action</span>";
        array_push($tableArray,$section);
    }

    $output= array(
        "draw"            => intval( $_REQUEST['draw'] ),
        "recordsTotal"    => intval( $rResultFilterTotal ),
        "recordsFiltered" => intval( $iFilteredTotal ),
        "data"            => $tableArray
    );
    echo json_encode( $output);




}


function payTotemTicket($ticketId){
    check_access('totem_ticket', true);
    global $dbh;
    $dbh->begin();
    $totemID=$dbh->queryOne("Select tca_ttm_id from totem_credit_account_reg where tca_code=".$dbh->prepareString($ticketId));
    $amount=$dbh->queryOne("Select sum(tca_credits+tca_points) from totem_credit_account_reg where tca_code=".$dbh->prepareString($ticketId));
    if(!$totemID){
        error_log('Invalid totem Id');
        return -1;
    }

    $result=adminUserTotemCreditLog('totem ticket pay',20,$_SESSION['admin_id'],$totemID,$_SESSION['jurisdiction_id'],$_SESSION['jurisdiction_id'],$amount);
    if($result) {
        $sql = "Update totem_credit_account_reg set tca_status=2 where tca_code=" . $dbh->prepareString($ticketId);
        $result = $dbh->exec($sql);
        if ($result) {
            $dbh->commit();
            doAdminUserLog($_SESSION['admin_id'], 'totem_ticket_pay', "Ticket code: $ticketId");
        }
        else {
            $dbh->rollback();
        }
    }
    else {
        $dbh->rollback();
    }
    return $result;
}


function adminUserTotemCreditLog($note='',$transactionType,$admId,$totemId,$admJurId,$admFinalJurId,$amount)
{
    global $dbh;
    $sql="Insert into admin_user_transaction (att_note,
                                                att_tty_id,
                                                att_aus_id,
                                                att_ttm_id,
                                                att_transaction_id,
                                                att_jur_id,
                                                att_amount,
                                                att_jur_avail_credit,
                                                att_jur_avail_overdraft,
                                                att_jur_final_avail_credit,
                                                att_jur_final_avail_overdraft )
                                        Values (
                                                ".$dbh->prepareString($note).",
                                                ".$dbh->prepareString($transactionType).",
                                                ".$dbh->prepareString($admId).",
                                                ".$dbh->prepareString($totemId).",
                                                ".$dbh->prepareString('ADM'.$dbh->nextSequence('CTR_TRAN_NUM_SEQ').rand_str(4)).",
                                                ".$dbh->prepareString($admJurId).",
                                                ".$dbh->prepareString($amount).",
                                               (SELECT jur_available_credit from jurisdiction where jur_id =".$dbh->prepareString($admJurId)."),
                                               (SELECT jur_overdraft from jurisdiction where jur_id =".$dbh->prepareString($admJurId)."),
                                               (SELECT jur_available_credit from jurisdiction where jur_id =".$dbh->prepareString($admFinalJurId)."),
                                               (SELECT jur_overdraft from jurisdiction where jur_id =".$dbh->prepareString($admFinalJurId)."))";
    $result=$dbh->exec($sql);
    return  $result;
}