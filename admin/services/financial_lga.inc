<?php
    $action = $_REQUEST['action'];
    switch ($action)
    {
        case "0":
        {
            $time=$_REQUEST['time'];
            $date_end=$_REQUEST['date_end'];
            $file="query.txt";
            if(!file_exists($file)){
            $file=file_put_contents($file,' ');
            shell_exec("/usr/bin/php /works/gr_projects/backoffice/admin/services/generatemonthlyresults.php $time $date_end >/dev/null &");
            }
           // generateMonthlyReportXLS($time,$date_end);
            break;
        }

            case'1':
        {
            $time=$_REQUEST['time'];
            $date_end=$_REQUEST['date_end'];
            $start = $_REQUEST["iDisplayStart"];
            $length = $_REQUEST["iDisplayLength"];
            $totals=$_REQUEST['totals'];
            echo getMonthlyReport($time,$date_end,$totals,$start,$length);
           break;
        }
    }


function getMonthlyReport($time,$date_end,$totresult,$start,$length){
global $lang,$dbh;
    $today=date("Y-m-d", time()-3600*24);
    $sql="SELECT  pun_username,
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
$sql.=" cms_tot_bet, ";

if(date("n", strtotime($date_end)) != date("n")) {
    $day=date("Y-m-t", strtotime($date_end));
}
else{
    $day=$_POST['day_end'];
}
if($time==1){
    $day=$today;
}
$sql.=" cur_name,
        cou_name,
        pun_dob,
        pun_last_logged_in
        FROM  punter";
if($time=='1'){
    $sql.=" LEFT JOIN punter_credit on pcr_pun_id=pun_id ";
}

$sql.=" LEFT JOIN customer_monthly_summary on  cms_pun_id = pun_id AND cms_day='$day'
        LEFT JOIN country on cou_id = pun_cou_id
        LEFT JOIN currency on cms_cur_code_id = cur_code_id
        GROUP BY pun_id limit $start,$length ";
$rs=$dbh->exec($sql);

    $users=array();
    while ( $rs->hasNext () ) {
        $section=array();
        $row = $rs->next ();
        $section[0]=$row['pun_username'];
        $section[1]=$row['pun_id'];
        $section[2]=$row['pun_first_name'];
        $section[3]=$row['pun_last_name'];
        $section[4]=$row['pun_email'];
        $section[5]="<p class='text-right'>";
        $section[5] .=getInDollars($row["cms_available_credit"]);
        $section[5].="</p>";
        $section[6]=$row['cur_name'];
        $section[7]=$row['cou_name'];
        $section[8]=$row['pun_dob'];
        $section[9]=$row['pun_last_logged_in'];
        $section[10]=getInDollars($row['cms_tot_bet']);
        array_push($users,$section);
    }
   return toJsonTable($totresult,$users);


}

function generateMonthlyReportXLS($time,$date_end){
    global $dbh;

    $today=date("Y-m-d", time()-3600*24);
    if(date("n", strtotime($date_end)) != date("n")) {
        $day=date("Y-m-t", strtotime($date_end));
    }
    else{
        $day=$_POST['day_end'];
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
            GROUP BY pun_id limit 0,3000 ";
    $rs=$dbh->exec($sql);
    if($rs->getNumRows()>1){

        require_once 'PHPExcel.php';
        require_once 'PHPExcel/Writer/Excel2007.php';
        require_once 'PHPExcel/Style/Fill.php';
        require_once 'PHPExcel/Style/Color.php';

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
            $objWriter->save("../documents/$today.xlsx");
        }
    }

}


function toJsonTable($i,$children){
    $output = array(
        "sEcho" => intval($_REQUEST['sEcho']),
        "iTotalRecords" => intval($i),
        "iTotalDisplayRecords" => intval($i),
        "aaData" => $children
    );
    return json_encode($output);
}