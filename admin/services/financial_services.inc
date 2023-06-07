<?php
require_once "financial_casino/financial_functions.inc";
require_once "Error.class.inc";
$action = $_REQUEST['action'];
if (isset($action)){
    switch ($action)
    {
        case "giveBonusToUser":
        {
            if(!(checkHashedPassword($_SESSION['admin_username'],$_POST['token']))) {
                die("Invalid password!");
            }
            die(giveUserBonus($_POST['user'],$_POST['amount'],$_POST['promotion'],$_POST['reason']));
            break;
        }

        case "getUserAvailableBonus":{
            if($_POST['username'] && $_POST['username']!=''){
                $currentUser=getPunterByUsername($_REQUEST['username']);
                die(getInDollars($currentUser['pcr_bonus'],2,$currentUser['cur_code_for_web']));
            }
                return false;
            break;
        }

        case "getMonthlyReportBets":{
            getMonthlyReportBets($_REQUEST);
            break;
        }

        case "generateMonthlyReportBets":{
            generateMonthlyReportBets($_POST['startDate'],$_POST['endDate'],$_POST['totalBet']);
            break;
        }
        case "getBettingFinancialDataJson":
            {
                error_log(json_encode($_REQUEST));
                if (substr(getIpAddress(), 0, strlen(BETTING_COMMISSIONS_SERVER_IP))==BETTING_COMMISSIONS_SERVER_IP) {
                    $jur_sel_id = (isset($_REQUEST['jurisdiction'])) ? $_REQUEST['jurisdiction'] : false;
                    $jurisdiction_level = (isset($_REQUEST['jurisdiction_level'])) ? $_REQUEST['jurisdiction_level'] : 'nation';
                    $jur_groups_level = (isset($_REQUEST['jurisdiction_group'])) ? $_REQUEST['jurisdiction_group'] : 'club';
                    $date_start = (isset($_REQUEST['sdate'])) ? $_REQUEST['sdate'] : date("Y-m-d", time() - (60 * 60 * 24));
                    $date_end = (isset($_REQUEST['edate'])) ? $_REQUEST['edate'] : date("Y-m-d", time() - (60 * 60 * 24));
                    $betwin = (isset($_REQUEST['betwin']) && $_REQUEST['betwin'] == true) ? true : false;
                    $financial = (isset($_REQUEST['financial']) && $_REQUEST['financial'] == true) ? true : false;
                    $result=json_encode(returnBettingFinancialDataJson($date_start, $date_end, $jur_groups_level, $betwin, $financial, $jurisdiction_level, $jur_sel_id));
                    error_log($result);
                    die($result);
                }
                else{
                    die(json_encode([-16,Error::$errors[-16]]));
                }

                }
            break;

            case "getBettingPeriod": {
                 $monthlyPeriods=month_select_box(false,true,'array');
                 $date=$_REQUEST['sdate'];
                 $return=['month'=>null,'week'=>null];
                 foreach ($monthlyPeriods as $k=>$v){
                    if($date==$v[1]){
                        $return['month']=$v[0].",".$v[1];
                        break;
                    }
                 }

                $weeklyPeriods=week_select_box(false,true,'array');
                foreach ($weeklyPeriods as $k=>$v){
                    if($date==$v[1]){
                        $return['week']=$v[0].",".$v[1];
                        break;
                    }
                }

                die(json_encode($return));
                break;
            }

        default:{
            die('Invalid operation');
        break;
        }
    }
}
function giveUserBonus($username,$amount,$promotion,$reason){
    global $dbh;
    $dbh->begin();
    $amount=floatval($amount);
    $customer=getPunterByUsername($username);
    $return='1';
    $customer_id=$customer['pun_id'];
    $trans_type = T_TYPE_BONUS;
    $refund_type = "BON";
    $transaction_number = nextTxnBonusAdjustment();
    $promotionInfo=getPromotionInfo((int)$promotion);
    if(!updateCustomerTransBonus($customer_id,$trans_type,getCustomerBalance($customer_id),$promotionInfo['prm_name'],$refund_type,$amount,$reason,$_SESSION['admin_id'])){
        $dbh->rollback();
        $return='Can not save customer transaction';
        $transaction_number=false;
    };

    if(!doBonusDeposit($customer_id, $amount)){
        $dbh->rollback();
        $return='Can not add money to customer';
        $transaction_number=false;
    }
    addPunterNote($customer_id,$_SESSION['admin_id'], 'BONUS DEPOSIT for promotion:'.$promotionInfo['prm_name']."...".$reason);

    if(!addPromotionToUser($customer_id,$promotion,$reason,0,$promotionInfo['prm_end_date'],$amount,$amount)){
        $dbh->rollback();
        $return='Can not assign promotion to customer.User already has a bonus of this promotion.';
    }
    $dbh->commit();
    return $return;
}

function updateCustomerTransBonus($customer_id,$trans_type,$punter_amount,$promotionName,$refund_type,$amount, $reason, $aus_id){
    global $dbh;
    $ctr_id = $dbh->nextSequence('ctr_id_seq');
    $sql  = " INSERT INTO customer_transaction (ctr_id,ctr_amount, ctr_time, ctr_tty_id, ctr_notes, ctr_balance_available, ctr_pun_id, ctr_pme_code,
                                                ctr_transaction_num, ctr_status, ctr_bonus_available, ctr_aus_id)
               VALUES ($ctr_id,$amount, CURRENT_TIMESTAMP, $trans_type, 'FIRST BONUS DEPOSIT for promotion: ".$promotionName."...".$dbh->escape($reason)."',$punter_amount, $customer_id, '$refund_type',
                                                    concat('BON-', last_seq_id('BONUS_TRAN_SEQ')), 'complete',".getCustomerBonusBalance($customer_id).", $aus_id) ";

    return $dbh->exec($sql);
}

function addPromotionToUser($pun_id,$prm_id,$note,$status=0,$end_date,$total_bonus,$initial_credit){
    global $dbh;
    $sql="INSERT INTO punter_promotion (ppr_pun_id, ppr_prm_id, ppr_note, ppr_status, ppr_start_date, ppr_end_date, ppr_total_bonus,ppr_bonus_left, ppr_initial_credit)
     VALUES (".$dbh->escape($pun_id).",
              ".$dbh->escape($prm_id).",
              ".$dbh->prepareString($note).",
              ".$dbh->escape($status).",
              CURRENT_TIMESTAMP,
              ".$dbh->prepareString($end_date).",
              ".$dbh->prepareString($total_bonus).",
              ".$dbh->prepareString($total_bonus).",
              ".$dbh->prepareString($initial_credit)."
              )";
    return $dbh->exec($sql);
}

function getPromotionInfo($promotionId){
    global $dbh;
    return $dbh->queryRow("Select * from promotion where prm_id=".$dbh->escape($promotionId));
}


function getMonthlyReportBets($request){
    global $dbh;
    $columns = array('pun_username','ctr_time','ABS(ctr_amount)', 'ctr_notes' );
    $sTable = "  customer_transaction, punter  " ;
    $limit = '';
    if ( isset($request['start']) && $request['length'] != -1 ) {
        $limit = "LIMIT ".intval($request['start']).", ".intval($request['length']);
    }
    $order = '';
    if ( isset($request['order']) && count($request['order']) ) {
        $orderBy = array();
        for ( $i=0, $ien=count($request['order']) ; $i<$ien ; $i++ ) {
            $columnIdx = intval($request['order'][$i]['column']);
            $requestColumn = $request['columns'][$columnIdx];
            $column = $columns[ $columnIdx ];

            if ( $requestColumn['orderable'] == 'true' ) {
                $dir = $request['order'][$i]['dir'] === 'asc' ?
                    'ASC' :
                    'DESC';
                $orderBy[] = ''.$column.' '.$dir;
            }
        }
        $order = 'ORDER BY '.implode(', ', $orderBy);
    }

    $sWhere="WHERE   ctr_time BETWEEN '".$dbh->escape($request['startDate'])." 00:00:00' AND '".$dbh->escape($request['endDate'])." 23:59:59'
                             and ctr_pun_id = pun_id
                             and ctr_tty_id = 8
                                  and ctr_gam_id = 210
                             and ctr_pme_code = 'CSH'  ";


    $str = $request['search']['value'];
    if ( isset($request['search']) && $request['search']['value'] != '' ) {
        $extrasWhere = " AND ";
        $extrasWhere .=" (pun_username  LIKE '%".mysql_real_escape_string( $str )."%' OR ctr_notes LIKE '%".$dbh->escape($str)."%' )";
    }
    $sQuery = "  SELECT SQL_CALC_FOUND_ROWS ".str_replace(" , ", " ", implode(", ", $columns))."
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
        $row['ctr_notes']=substr($row['ctr_notes'],0,strpos($row['ctr_notes'],'Bet performed')-1);
        $section=array();
        $section[0]=$row['pun_username'];
        $section[1]=$row['ctr_time'];
        $section[2]=$row["abs(ctr_amount)"];
        $section[3]=$row["ctr_notes"];
        array_push($tableArray,$section);
    }

    $output= array(
        "draw"            => intval( $request['draw'] ),
        "recordsTotal"    => intval( $rResultFilterTotal ),
        "recordsFiltered" => intval( $iFilteredTotal ),
        "data"            => $tableArray
    );
    echo json_encode( $output);
}


function generateMonthlyReportBets($startDate,$endDate,$totalBet){
    ini_set('memory_limit', '1000M');
    global $dbh;
    $sql="select pun_username username,
                             ctr_time date,
                             ABS(ctr_amount) bet_amount,
                             ctr_notes note from
                             customer_transaction, punter
                             where ctr_time BETWEEN ".$dbh->prepareString("$startDate 00:00:00" ) ." AND ".$dbh->prepareString("$endDate 23:59:59'").
                             " and ctr_pun_id = pun_id
                             and ctr_tty_id = 8
                                  and ctr_gam_id = 210
                             and ctr_pme_code = 'CSH'
                             order by ctr_time asc ";

    $result=$dbh->exec($sql);

               require_once 'PHPExcel.php';
                require_once 'PHPExcel/Writer/Excel2007.php';
                require_once 'PHPExcel/Style/Fill.php';
                require_once 'PHPExcel/Style/Color.php';
                $rowId = 1;
                $objPHPExcel = new PHPExcel();
                $sheet=$objPHPExcel->getActiveSheet();
                $objPHPExcel->getProperties()->setCreator("CsLiveGames");
                $objPHPExcel->getProperties()->setLastModifiedBy("CsLiveGames");
                $objPHPExcel->getProperties()->setTitle("Monthly Played from " .$startDate ." untill ".$endDate );
                $objPHPExcel->setActiveSheetIndex(0);
                $sheet->SetCellValue("A".$rowId, "Username:");
                $sheet->SetCellValue("B".$rowId, "Date");
                $sheet->SetCellValue("C".$rowId, "Bet Amount");
                $sheet->getStyle("A".$rowId.":D".$rowId)->getFont()->getColor()->setARGB(PHPExcel_Style_Color::COLOR_WHITE);
                $sheet->getStyle("A".$rowId.":D".$rowId)->getFill()->setFillType(PHPExcel_Style_Fill::FILL_SOLID)->getStartColor()->setARGB('FF0000DD');
                $sheet->SetCellValue("D".$rowId, "Note");
            while($result->hasNext()) {
                $row = $result->next();
                $rowId++;
                $row['note']=substr($row['note'],0,strpos($row['note'],'Bet performed')-1);
                $row['note'] = str_ireplace('AdminUser', '', $row['note']);
                $sheet->getColumnDimension('A')->setAutoSize(true);
                $sheet->getColumnDimension('B')->setAutoSize(true);
                $sheet->getColumnDimension('C')->setAutoSize(true);
                $sheet->getColumnDimension('D')->setAutoSize(true);

                $sheet->SetCellValue("A" . $rowId, $row['username']);
                $sheet->SetCellValue("B" . $rowId, $row["date"]);
                $sheet->SetCellValue("C" . $rowId, $row["bet_amount"]);
                $sheet->SetCellValue("D" . $rowId, $row["note"]);

            }
                    $rowId++;
                 $sheet->SetCellValue("A".$rowId, 'Total Bet');
                 $sheet->SetCellValue("C".$rowId, $totalBet);
                    $rowId++;
                $sheet->SetCellValue("A".$rowId, 'Total LGA Tax');
                 $sheet->SetCellValue("C".$rowId, $totalBet*0.005);
                    $objWriter = new PHPExcel_Writer_Excel2007($objPHPExcel);
                    $objWriter->save("../documents/Games played from $startDate until $endDate.xlsx");
    return true;



}