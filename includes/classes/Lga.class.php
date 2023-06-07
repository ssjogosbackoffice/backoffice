<?php
   class Lga{
       function __construct($day){
           if(file_exists("../cache/$day.xlsx")){
             return "File already exists";
           }
           else{
           global $dbh;
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
                          WHERE   cms_pun_id=pun_id
                          AND     cou_id=pun_cou_id
                          AND     cms_currency=cur_code_id
                          AND     cms_day ='".$day."'
                          GROUP BY pun_id";
           $result=$dbh->exec($sql);
               if($result->getNumRows()>0){
                   require_once 'PHPExcel.php';
                   require_once 'PHPExcel/Writer/Excel2007.php';
                   require_once 'PHPExcel/Style/Fill.php';
                   require_once 'PHPExcel/Style/Color.php';
                   $rowId = 1;
                   $objPHPExcel = new PHPExcel();
                   $objPHPExcel->getProperties()->setCreator("CsLiveGames");
                   $objPHPExcel->getProperties()->setLastModifiedBy("CsLiveGames");
                   $objPHPExcel->getProperties()->setTitle("Monthly Reports for " . $day);
                   $objPHPExcel->setActiveSheetIndex(0);
                   $objPHPExcel->getActiveSheet()->SetCellValue("A".$rowId, "Username:");
                   $objPHPExcel->getActiveSheet()->SetCellValue("B".$rowId, "User code");
                   $objPHPExcel->getActiveSheet()->SetCellValue("C".$rowId, "Name ");
                   $objPHPExcel->getActiveSheet()->getStyle("A".$rowId.":J".$rowId)->getFont()->getColor()->setARGB(PHPExcel_Style_Color::COLOR_WHITE);
                   $objPHPExcel->getActiveSheet()->getStyle("A".$rowId.":J".$rowId)->getFill()->setFillType(PHPExcel_Style_Fill::FILL_SOLID)->getStartColor()->setARGB('FF0000DD');
                   $objPHPExcel->getActiveSheet()->SetCellValue("D".$rowId, "Lastname");
                   $objPHPExcel->getActiveSheet()->SetCellValue("E".$rowId, "Email");
                   $objPHPExcel->getActiveSheet()->SetCellValue("F".$rowId, "Amount");
                   $objPHPExcel->getActiveSheet()->SetCellValue("F".$rowId, "Currency");
                   $objPHPExcel->getActiveSheet()->SetCellValue("G".$rowId, "Country ");
                   $objPHPExcel->getActiveSheet()->SetCellValue("H".$rowId, "Birthday");
                   $objPHPExcel->getActiveSheet()->SetCellValue("I".$rowId, "Last Login");
                   $objPHPExcel->getActiveSheet()->SetCellValue("J".$rowId, "Total bets");

                    while($result->hasNext())
                    {
                    $rowId++;
                    $row=$result->next();
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

                    $objPHPExcel->getActiveSheet()->SetCellValue("A".$rowId, $row['pun_username']);
                    $objPHPExcel->getActiveSheet()->SetCellValue("B".$rowId, $row["pun_id"]);
                    $objPHPExcel->getActiveSheet()->SetCellValue("C".$rowId, $row["pun_first_name"]);
                    $objPHPExcel->getActiveSheet()->SetCellValue("D".$rowId, $row["pun_last_name"]);
                    $objPHPExcel->getActiveSheet()->SetCellValue("E".$rowId, $row["pun_email"]);
                    $objPHPExcel->getActiveSheet()->SetCellValue("F".$rowId, $row["cms_available_credit"]);
                    $objPHPExcel->getActiveSheet()->SetCellValue("G".$rowId, $row["cur_name"]);
                    $objPHPExcel->getActiveSheet()->SetCellValue("H".$rowId, $row["pun_dob"]);
                    $objPHPExcel->getActiveSheet()->SetCellValue("I".$rowId, $row["pun_last_logged_in"]);
                    $objPHPExcel->getActiveSheet()->SetCellValue("J".$rowId, $row["cms_tot_bet"]);
                    }
                   $objWriter = new PHPExcel_Writer_Excel2007($objPHPExcel);
                   $objWriter->save("../cache/$day.xlsx");
               }
               else{
                   error_log('No result found for'.$day);
               }
            }
       }
   }
?>