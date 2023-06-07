<?php
include_once '../php/Logger.php';
define('WEB_CONFIG',     realpath(dirname(__FILE__).'/../config'));
Logger::configure(WEB_CONFIG . "/log4php.xml");

$dbLog = Logger::getLogger("DbLogger");
$month = isset($_SERVER['argv'][1]) ? $_SERVER['argv'][1] : date('Y-m-d',time() - 60 * 60 * 24);
if(file_exists("../../cache/$month.xls")){
    return "File already exists";
}
else{
    include_once("../classes/DatabaseHandler.class.inc");
    $dbh = new DatabaseHandler("mysql://developer:TEAMDEVELOPER@10.15.0.12/casinogames");
    $dbh->connect();
    $sql="SELECT  pun_username,
                  pun_id,
                  pun_first_name,
                  pun_last_name,
                  pun_email,
                  cms_available_credit,
            cms_tot_bet,
            cur_name,
            cou_name,
            pun_dob,
            pun_last_logged_in
            FROM    punter,
            customer_monthly_summary,
            country,
            currency
            WHERE   cms_pun_id = pun_id
            AND     cou_id = pun_cou_id
            AND     cms_cur_code_id = cur_code_id
            AND     cms_day ='".$month."'
            GROUP BY pun_id";
    $result=$dbh->exec($sql);

    ?>

<?
    if($result->getNumRows()>0){
        $plm='<table><tr><th>Username</th><th>User code</th><th>Name</td><th>Last Name</th><th>Email</th><th>Amount</th><th>Currency</th><th>Country</th><th>Birthday</th><th>Last Login</th><th>Total bets</th></tr>';

      /*  require_once '../PHPExcel.php';
        require_once '../PHPExcel/Writer/Excel2007.php';
        require_once '../PHPExcel/Style/Fill.php';
        require_once '../PHPExcel/Style/Color.php';
	require_once '../PHPExcel/IOFactory.php';

        $cacheMethod = PHPExcel_CachedObjectStorageFactory::cache_in_memory_gzip;
        if (!PHPExcel_Settings::setCacheStorageMethod($cacheMethod)) {
            die($cacheMethod . " caching method is not available" . EOL);
        }
        $objPHPExcel = new PHPExcel();
	$objPHPExcel->setActiveSheetIndex(0);
	$rowId = 1;

        $objPHPExcel->getProperties()->setCreator("CsLiveGames");
        $objPHPExcel->getProperties()->setLastModifiedBy("CsLiveGames");
        $objPHPExcel->getProperties()->setTitle("Monthly Reports for " . $month);
        $objPHPExcel->setActiveSheetIndex(0);
        $objPHPExcel->getActiveSheet()->SetCellValue("A".$rowId, "Username:");
        $objPHPExcel->getActiveSheet()->SetCellValue("B".$rowId, "User code");
        $objPHPExcel->getActiveSheet()->SetCellValue("C".$rowId, "Name ");
        $objPHPExcel->getActiveSheet()->getStyle("A".$rowId.":K".$rowId)->getFont()->getColor()->setARGB(PHPExcel_Style_Color::COLOR_WHITE);
        $objPHPExcel->getActiveSheet()->getStyle("A".$rowId.":K".$rowId)->getFill()->setFillType(PHPExcel_Style_Fill::FILL_SOLID)->getStartColor()->setARGB('FF0000DD');
        $objPHPExcel->getActiveSheet()->SetCellValue("D".$rowId, "Last Name");
        $objPHPExcel->getActiveSheet()->SetCellValue("E".$rowId, "Email");
        $objPHPExcel->getActiveSheet()->SetCellValue("F".$rowId, "Amount");
        $objPHPExcel->getActiveSheet()->SetCellValue("G".$rowId, "Currency");
        $objPHPExcel->getActiveSheet()->SetCellValue("H".$rowId, "Country ");
        $objPHPExcel->getActiveSheet()->SetCellValue("I".$rowId, "Birthday");
        $objPHPExcel->getActiveSheet()->SetCellValue("J".$rowId, "Last Login");
        $objPHPExcel->getActiveSheet()->SetCellValue("K".$rowId, "Total bets");
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
        $time = microtime(true);
	$objWriter = new PHPExcel_Writer_Excel2007($objPHPExcel);
	$objWriter->save("../../cache/lga.xlsx");

	$objPHPExcel = PHPExcel_IOFactory::load("../../cache/lga.xlsx");
	$objPHPExcel->setActiveSheetIndex(0);
	$rowId = $objPHPExcel->getActiveSheet()->getHighestRow()+1; */

	while($result->hasNext())
        {
            $row=$result->next();
            echo $result->getCurrentIndex() . " \n";
            $plm .="<tr><td>".$row["pun_username"]."</td>
                    <td>".$row["pun_id"]."</td>
                    <td>".$row["pun_first_name"]."</td>
                    <td>".$row["pun_last_name"]."</td>
                    <td>".$row["pun_email"]."</td>
                    <td>".$row["cms_available_credit"]."</td>
                    <td>".$row["cur_name"]."</td>
                    <td>".$row["cou_name"]."</td>
                    <td>".$row["pun_dob"]."</td>
                    <td>".$row["pun_last_logged_in"]."</td>
                    <td>".$row["cms_tot_bet"]."</td></tr>";
            if($result->getCurrentIndex()>1000){break;}
         /*   $row = $result->next();
	    echo $result->getCurrentIndex() . " \n"; 
	    $rowId++;
	    $objPHPExcel->getActiveSheet()->SetCellValue("A".$rowId, $row["pun_username"]);
            $objPHPExcel->getActiveSheet()->SetCellValue("B".$rowId, $row["pun_id"]);
            $objPHPExcel->getActiveSheet()->SetCellValue("C".$rowId, $row["pun_first_name"]);
            $objPHPExcel->getActiveSheet()->SetCellValue("D".$rowId, $row["pun_last_name"]);
            $objPHPExcel->getActiveSheet()->SetCellValue("E".$rowId, $row["pun_email"]);
            $objPHPExcel->getActiveSheet()->SetCellValue("F".$rowId, $row["cms_available_credit"]);
            $objPHPExcel->getActiveSheet()->SetCellValue("G".$rowId, $row["cur_name"]);
            $objPHPExcel->getActiveSheet()->SetCellValue("H".$rowId, $row["cou_name"]);
            $objPHPExcel->getActiveSheet()->SetCellValue("I".$rowId, $row["pun_dob"]);
            $objPHPExcel->getActiveSheet()->SetCellValue("J".$rowId, $row["pun_last_logged_in"]);
            $objPHPExcel->getActiveSheet()->SetCellValue("K".$rowId, $row["cms_tot_bet"]);*/
	}
        $plm .="</table>";
        file_put_contents("../../cache/$month.xls",$plm);
	/*$objWriter = new PHPExcel_Writer_Excel2007($objPHPExcel);
        $objWriter->save("../../cache/lga.xlsx");

        $end_time = microtime(true);
        $res = $end_time - $time;*/
	echo "Succesfully";
    }
    else{
        echo 'No result found for '.$month;
    }
}
?>
