<?php
ini_set('memory_limit', '999M' );
include_once '/works/gr_projects/backoffice/includes/php/Logger.php';
//define('WEB_CONFIG',     realpath(dirname(__FILE__).'/../config'));
Logger::configure("/works/gr_projects/backoffice/config/logger.xml");

$dbLog = Logger::getLogger("DbLogger");
include_once("/works/gr_projects/backoffice/includes/classes/DatabaseHandler.class.inc");
$dbh = new DatabaseHandler("mysql://developer:TEAMDEVELOPER@10.15.0.12/casinogames");
$dbh->connect();

$time=$_SERVER['argv'][1];
$date_end = $_SERVER['argv'][2];
$today=date("Y-m-d", time()-3600*24);


if(date("n", strtotime($date_end)) != date("n")) {
    $day=date("Y-m-t", strtotime($date_end));
}
if($time==1){
    $day=$today;
}

$sql="SELECT
            pun_username,
            pun_id,
            pun_first_name,
            pun_last_name,
            pun_email,";
if($time=='1'){
    $sql.=" pcr_credits as  cms_available_credit, ";
}
else{
    $sql .=" cms_available_credit,";
}
$sql.="cms_tot_bet,
            cur_name,
            cou_name,
            pun_dob,
            pun_last_logged_in
            FROM    punter";
if($time=='1'){
    $sql.=" LEFT JOIN punter_credit on pcr_pun_id=pun_id ";
}
$sql.=" LEFT JOIN customer_monthly_summary on  cms_pun_id = pun_id AND cms_day='$day'
        LEFT JOIN country on cou_id = pun_cou_id
        LEFT JOIN currency on cms_cur_code_id = cur_code_id
        GROUP BY pun_id ";
$rs=$dbh->exec($sql);
if($rs->getNumRows()>1){
    require_once '/works/gr_projects/backoffice/includes/PHPExcel.php';
    require_once '/works/gr_projects/backoffice/includes/PHPExcel/Writer/Excel2007.php';
    require_once '/works/gr_projects/backoffice/includes/PHPExcel/Style/Fill.php';
    require_once '/works/gr_projects/backoffice/includes/PHPExcel/Style/Color.php';

    $rowId = 1;
    $objPHPExcel = new PHPExcel();
    $objPHPExcel->getProperties()->setCreator("CsLiveGames");
    $objPHPExcel->getProperties()->setLastModifiedBy("CsLiveGames");
    $objPHPExcel->getProperties()->setTitle("Monthly Played for " . $day);
    $objPHPExcel->setActiveSheetIndex(0);
    $objPHPExcel->getActiveSheet()->SetCellValue("A".$rowId, "Username:");
    $objPHPExcel->getActiveSheet()->SetCellValue("B".$rowId, "User code");
    $objPHPExcel->getActiveSheet()->SetCellValue("C".$rowId, "Name");
    $objPHPExcel->getActiveSheet()->SetCellValue("D".$rowId, "Last Name");
    $objPHPExcel->getActiveSheet()->SetCellValue("E".$rowId, "Email");
    $objPHPExcel->getActiveSheet()->SetCellValue("F".$rowId, "Amount");
    $objPHPExcel->getActiveSheet()->SetCellValue("G".$rowId, "Currency");
    $objPHPExcel->getActiveSheet()->SetCellValue("H".$rowId, "Country");
    $objPHPExcel->getActiveSheet()->SetCellValue("I".$rowId, "Birthday");
    $objPHPExcel->getActiveSheet()->SetCellValue("J".$rowId, "Last Login");
    $objPHPExcel->getActiveSheet()->SetCellValue("K".$rowId, "Total bets");
    $objPHPExcel->getActiveSheet()->getStyle("A".$rowId.":K".$rowId)->getFont()->getColor()->setARGB(PHPExcel_Style_Color::COLOR_WHITE);
    $objPHPExcel->getActiveSheet()->getStyle("A".$rowId.":K".$rowId)->getFill()->setFillType(PHPExcel_Style_Fill::FILL_SOLID)->getStartColor()->setARGB('FF0000DD');

    while($rs->hasNext())
    {
        $row=$rs->next();

        if($time==1){
            $rowId++;
            $objPHPExcel->getActiveSheet()->getColumnDimension('A')->setAutoSize(true);
            $objPHPExcel->getActiveSheet()->getColumnDimension('B')->setAutoSize(true);
            $objPHPExcel->getActiveSheet()->getColumnDimension('C')->setAutoSize(true);
            $objPHPExcel->getActiveSheet()->getColumnDimension('D')->setAutoSize(true);
            $objPHPExcel->getActiveSheet()->getColumnDimension('E')->setAutoSize(true);
            $objPHPExcel->getActiveSheet()->getColumnDimension('F')->setAutoSize(true);
            $objPHPExcel->getActiveSheet()->getColumnDimension('G')->setAutoSize(true);
            $objPHPExcel->getActiveSheet()->getColumnDimension('H')->setAutoSize(true);
            $objPHPExcel->getActiveSheet()->getColumnDimension('I')->setAutoSize(true);
            $objPHPExcel->getActiveSheet()->getColumnDimension('J')->setAutoSize(true);
            $objPHPExcel->getActiveSheet()->getColumnDimension('K')->setAutoSize(true);

            $objPHPExcel->getActiveSheet()->SetCellValue("A".$rowId, $row['pun_username']);
            $objPHPExcel->getActiveSheet()->SetCellValue("B".$rowId, $row["pun_id"]);
            $objPHPExcel->getActiveSheet()->SetCellValue("C".$rowId, $row["pun_first_name"]);
            $objPHPExcel->getActiveSheet()->SetCellValue("D".$rowId, $row["pun_last_name"]);
            $objPHPExcel->getActiveSheet()->SetCellValue("E".$rowId, $row['pun_email']);
            $objPHPExcel->getActiveSheet()->SetCellValue("F".$rowId, $row["cms_available_credit"]);
            $objPHPExcel->getActiveSheet()->SetCellValue("G".$rowId, $row["cur_name"]);
            $objPHPExcel->getActiveSheet()->SetCellValue("H".$rowId, $row["cou_name"]);
            $objPHPExcel->getActiveSheet()->SetCellValue("I".$rowId, $row['pun_dob']);
            $objPHPExcel->getActiveSheet()->SetCellValue("J".$rowId, $row["pun_last_logged_in"]);
            $objPHPExcel->getActiveSheet()->SetCellValue("K".$rowId, $row["cms_tot_bet"]);
        }
    }

    if($time==1){
        $objWriter = new PHPExcel_Writer_Excel2007($objPHPExcel);
        $objWriter->save("/works/gr_projects/backoffice/admin/documents/today.xlsx");
        unlink("/works/gr_projects/backoffice/admin/services/query.txt");
    echo 'generated';
    }
}
?>
