<?php
check_access ( 'ttm_totem_daily_transtrack', $redirect = true );
include_once 'JurisdictionsList.class.inc';
require_once 'PHPExcel.php';
require_once 'PHPExcel/Writer/Excel2007.php';
require_once 'PHPExcel/Style/Fill.php';
require_once 'PHPExcel/Style/Color.php';
?>
    <script type="text/javascript">
        <!--
        function acceptPending(ttID, tCode){
            $.ajax({
                type: 'POST',
                url:  'totem/transack_accept.php',
                data: {pAccept:1,tr_id:ttID,tcode:tCode},
                success: function(data){
                    if(data == '-100'){
                        alert("La richiesta non � stata accettata");
                    }else if(data == "0"){
                        alert("La richiesta si e' conclusa correttamente. Il Totem e' ora disponibile");
                    }else{
                        alert("La richiesta e' stata accetta, ma il totem al momento non e' stato contattato");
                    }
                    $query = "<?=$_SERVER['QUERY_STRING']?>";
                    if($query.indexOf('ttcode') == -1){
                        $query += "&ttcode="+tCode;
                    }
                    location.href = "<?=$_SERVER['PHP_SELF']?>?"+$query;
                },
                error: function(){
                    alert("Errore di comunicazione");
                }
            });
        }
        function chooseClub(){
            $("#choosefortotem").hide();
            $("#chooseforclub").show();
            $("#resinpage").hide();
        }
        function chooseTotem(){
            $("#chooseforclub").hide();
            $("#choosefortotem").show();
            $("#resinpage").show();
        }
        $(document).ready(function(){
            $('#ajaxProgress').hide();
        });

        $(document).ajaxStart(function(){
            $('#ajaxProgress').show();
        });
        $(document).ajaxStop(function(){
            $('#ajaxProgress').hide();
        });
        //-->
    </script>
    <center>
        <div id="ajaxProgress">
            Request in progress.. wait please. ... <img src="<?=image_dir?>/ajax-loader.gif" />
        </div>
    </center>
<?php

$chooseFor = "club";

function getAmount($amount) {
    if ($amount > 0) {
        return '<FONT COLOR="#000000">' . getInDollars ( $amount ) . '</FONT>';
    } else {
        return '<FONT COLOR="#FF0000">' . getInDollars ( $amount ) . '</FONT>';
    }
}

function getStatus($status, $tttID, $tCode){
    if($status == "C"){
        return "<FONT COLOR='green'>Completed</FONT>";
    }else if($status == "P"){
        return "<FONT COLOR='orange'>Pending</FONT><input type='button' onClick='acceptPending(\"$tttID\", \"$tCode\");' name='pAccept' value='Accept'  />";
    }
}

function getExcelStatu($status, $tttID, $tCode){
    if($status == "C"){
        return "Completed";
    }else if($status == "P"){
        return "Pending";
    }
}

globalise ( 'ttldate' );
globalise ('date_end');
setupDateInputJs ( "date_start" , "date_end" );

if (isset ( $_GET ['ttcode'] )) {
    $totemCode = $_GET ['ttcode'];
} else if (isset ( $_POST ['ttcode'] )) {
    $totemCode = $_GET ['ttcode'];
} else {
    $totemCode = '';
}


$date_start = date('Y-m-d H:i', mktime(0,0,0,date('m'),date('d')-1,date('Y')));
$date_end =  date('Y-m-d H:i', mktime(23,59,0,date('m'),date('d'),date('Y')));

if (isset ( $_POST ['do_search'] )) {
    $date_start = $_POST['date_start'];
    $date_end = $_POST['date_end'];
    $totemCode = $_POST['ttcode'];
    $chooseFor = $_POST['choosefor'];
    $totemClub = $_POST['ttclub'];

    $interval = date_diff(date_create($date_start), date_create($date_end));
    $out = $interval->format("%M");
    settype($out, 'integer');

    if($out < 2){

        if($chooseFor == "totem"){

            if($totemCode  == ''){
                addError('', "Totem Code non puo' essere nullo");
            }else{
                $row_to_page = $_POST['resforpage'];
                $sql = "Select ttt_id,
									   ttt_ttm_id, 
									   ttt_time,
									   ttt_pun_id, 
									   ttt_transferid, 
									   ttt_amount, 
									   ttt_note, 
									   ttt_status,
									   pun_id, 
									   pun_username ,
									   ttm_code
										 FROM totem_transtrack 
										  LEFT JOIN totem on ttm_id = ttt_ttm_id
										  LEFT JOIN punter on ttt_pun_id = pun_id
										  WHERE UPPER(ttm_code) = UPPER(" . $dbh->quote ( $totemCode ) . ") 
											AND ttt_time BETWEEN date_format(".$dbh->quote($date_start).",'%Y-%m-%d %H:%i') 
											AND  date_format(" .$dbh->quote($date_end) . " ,'%Y-%m-%d %H:%i') order by ttt_id ASC";

                $res = $dbh->doCachedQuery ( $sql, 0 );

                $cols_arr = array ('Totem Code', 'Transaction ID', 'Date', 'Username', 'Transfer ID', 'Amount', 'Note', 'Status' );

                $val_fmt_arr = array ('Totem Code' => 'return strtoupper($rec["ttm_code"]);', 'Transaction ID' => 'return $rec["ttt_id"];', 'Date' => 'return $rec["ttt_time"];', 'Username' => 'return getClassLink(refPage("customers/customer_view&customer_id=" . $rec["pun_id"]), $rec["pun_username"]);', 'Transfer ID' => 'return $rec["ttt_transferid"];', 'Amount' => 'return getAmount($rec["ttt_amount"]);', 'Note' => 'return $rec["ttt_note"];', 'Status' => 'return getStatus($rec["ttt_status"],$rec["ttt_id"],$rec["ttm_code"]);' );

                $cell_fmt_arr = array ();

                $post_vars = array('ttcode' => $totemCode,
                    'date_start' => $date_start,
                    'date_end' => $date_end,
                    'choosefor' => $chooseFor,
                    'resforpage' => $row_to_page,
                    'do_search' => '1'
                );
                // 'ttmactive' => $ttmActiveStatus,
                // 'ttmaccess' => $ttmAccessStatus,
                // 'jurisdiction' => $ttmJurisdiction,
                // 'do_search' => '1',
                // 'resforpage' => $row_to_page

                // );

                $start_row = (isset ( $_POST ['start_row'] )) ? $_POST ['start_row'] : 1;
                $qp = new QueryPrinter ( $res );
                $excel_file_name = 'totemTransaction_' . $totemCode . '.xls';
                $qp->dumpToExcelFile ( $res, $cols_arr, $val_fmt_arr, $excel_file_name );

                ?>
                <hr>
                <h4>Report Totem Code <?=$totemCode?></h4>
                <?php

                $qp->printTable ( $res, $cols_arr, $post_vars, $val_fmt_arr, $cell_fmt_arr, 0, $excel_file_name, $header = "", $width = "100%", $start_row, $row_to_page, false );
            }
        }else{
            $sql = "SELECT SUM(IF(ttt_amount > 0 ,ttt_amount ,0)) as plus, SUM(IF(ttt_amount < 0 ,ttt_amount ,0)) as minus
				       FROM totem_transtrack
								LEFT JOIN totem on ttm_id = ttt_ttm_id
								LEFT JOIN punter on ttt_pun_id = pun_id
		 						WHERE ttm_club_id = " . $totemClub . " AND  
								ttt_time BETWEEN date_format(".$dbh->quote($date_start).",'%Y-%m-%d %H:%i')
								AND  date_format(" .$dbh->quote($date_end) . " ,'%Y-%m-%d %H:%i') order by ttt_id, ttt_time ASC ";

            $sumRes = $dbh->queryRow($sql);


            $sql = "Select  ttt_id,
				                ttt_ttm_id,
								ttt_time,
								ttt_pun_id,
								ttt_transferid,
								ttt_amount,
								ttt_note,
								ttt_status,
								pun_id,
								pun_username ,
								ttm_code
								FROM totem_transtrack
								LEFT JOIN totem on ttm_id = ttt_ttm_id
								LEFT JOIN punter on ttt_pun_id = pun_id
		 						WHERE ttm_club_id = " . $totemClub . " AND  
								ttt_time BETWEEN date_format(".$dbh->quote($date_start).",'%Y-%m-%d %H:%i')
								AND  date_format(" .$dbh->quote($date_end) . " ,'%Y-%m-%d %H:%i') order by ttm_code, ttt_id, ttt_time ASC";

            $totems = array();
            $actualTotem = null;
            $plusAmountTotem = 0;
            $minusAmountTotem = 0;
            $res = $dbh->doCachedQuery ( $sql, 0 );
            //'Totem Code', 'Transaction ID', 'Date', 'Username', 'Transfer ID', 'Amount', 'Note', 'Status'
            if($res->NumRows > 0){
                $objPHPExcel = new PHPExcel();
                $objPHPExcel->getProperties()->setCreator("CsLiveGames");
                $objPHPExcel->getProperties()->setLastModifiedBy("CsLiveGames");
                $objPHPExcel->getProperties()->setTitle("Totem Reports to " . $date_start . " from " . $date_end);
                $objPHPExcel->setActiveSheetIndex(0);

                while($res->hasNext()){
                    $row = $res->next();
                    if($row['ttm_code'] != $actualTotem){
                        if($actualTotem != null){
                            $totems[$actualTotem]["deposit"] = $plusAmountTotem;
                            $totems[$actualTotem]["withdrawal"] = $minusAmountTotem;
                            $totems[$actualTotem]["total"] = ($plusAmountTotem + $minusAmountTotem);
                            $plusAmountTotem = 0;
                            $minusAmountTotem = 0;
                        }
                        $totems[$row['ttm_code']] = array();
                        $actualTotem = $row['ttm_code'];
                    }
                    $totems[$row['ttm_code']][$row["ttt_id"]] = array();
                    $totems[$row['ttm_code']][$row["ttt_id"]]["data"] = $row;
                    if($row["ttt_amount"] > 0){
                        $plusAmountTotem = $plusAmountTotem + $row['ttt_amount'];
                    }else{
                        $minusAmountTotem = $minusAmountTotem + $row['ttt_amount'];
                    }
                }
                $totems[$actualTotem]["deposit"] = $plusAmountTotem;
                $totems[$actualTotem]["withdrawal"] = $minusAmountTotem;
                $totems[$actualTotem]["total"] = ($plusAmountTotem + $minusAmountTotem);
                $plusAmountTotem = 0;
                $minusAmountTotem = 0;
            }else{
                $totems = null;
            }
            ?>
            <style>
                .odd td{
                    background-color:#FFF;
                }
                .even td{
                    background-color:#EEE;
                }
            </style>
            <?php
            if(!is_null($totems)){
                ?>
                <div style="text-align:right;margin-right:10px;margin-bottom:10px;"><a href='<?="../cache/".$_SESSION["jurisdiction_class"]."_".$date_start."_".$date_end."_totem.xlsx"?>'><img src="<?= secure_host ?>/media/images/icon_download.jpg" width="16" height="16"/> Download excel file</a></div>
                <h2>Totals Period: <?=$date_start?> To <?=$date_end?></h2>
                <table>
                    <tr>
                        <td style="padding:5px;" class="label">Total Deposit:</td>
                        <td style="padding:5px;" class="label">Total Withdrawal:</td>
                        <td style="padding:5px;" class="label">Totals:</td>
                    </tr>
                    <tr>
                        <td style="text-align:right;vertical-align:middle;padding:5px; class="content"><?=getAmount($sumRes['plus'])?></td>
                        <td style="text-align:right;vertical-align:middle;padding:5px; class="content"><?=getAmount($sumRes['minus'])?></td>
                        <td style="text-align:right;vertical-align:middle;padding:5px; class="content"><?=getAmount(($sumRes['plus'] + $sumRes['minus']))?></td>
                    </tr>
                </table>
                <?php
                $row = 1;
                foreach ($totems as $key => $value){

                    $objPHPExcel->getActiveSheet()->SetCellValue('A'.$row, "Report Totem Code: " . $key);
                    $objPHPExcel->getActiveSheet()->mergeCells('A'.$row.':G'.$row);
                    $objPHPExcel->getActiveSheet()->getStyle('A'.$row)->getFont()->getColor()->setARGB(PHPExcel_Style_Color::COLOR_WHITE);
                    $objPHPExcel->getActiveSheet()->getStyle('A'.$row)->getFill()->setFillType(PHPExcel_Style_Fill::FILL_SOLID)->getStartColor()->setARGB('FFFF0000');
                    $row++;
                    $objPHPExcel->getActiveSheet()->SetCellValue('A'.$row, 'TRANS.. ID');
                    $objPHPExcel->getActiveSheet()->SetCellValue('B'.$row, 'DATE');
                    $objPHPExcel->getActiveSheet()->SetCellValue('C'.$row, 'USERNAME');
                    $objPHPExcel->getActiveSheet()->SetCellValue('D'.$row, 'TRANSFER CODE');
                    $objPHPExcel->getActiveSheet()->SetCellValue('E'.$row, 'AMOUNT');
                    $objPHPExcel->getActiveSheet()->SetCellValue('F'.$row, 'NOTE');
                    $objPHPExcel->getActiveSheet()->SetCellValue('G'.$row, 'STATUS');
                    $objPHPExcel->getActiveSheet()->getColumnDimension('A')->setAutoSize(true);
                    $objPHPExcel->getActiveSheet()->getColumnDimension('B')->setAutoSize(true);
                    $objPHPExcel->getActiveSheet()->getColumnDimension('C')->setAutoSize(true);
                    $objPHPExcel->getActiveSheet()->getColumnDimension('D')->setAutoSize(true);
                    $objPHPExcel->getActiveSheet()->getColumnDimension('E')->setAutoSize(true);
                    $objPHPExcel->getActiveSheet()->getColumnDimension('F')->setAutoSize(true);
                    $objPHPExcel->getActiveSheet()->getColumnDimension('G')->setAutoSize(true);
                    $objPHPExcel->getActiveSheet()->getStyle('A'.$row.':G'.$row)->getFont()->getColor()->setARGB(PHPExcel_Style_Color::COLOR_WHITE);
                    $objPHPExcel->getActiveSheet()->getStyle('A'.$row.':G'.$row)->getFill()->setFillType(PHPExcel_Style_Fill::FILL_SOLID)->getStartColor()->setARGB('FF0000DD');
                    $row++
                    ?>
                    <hr>
                    <h4>Report Totem Code <?=$key?></h4>
                    <table cellpadding="0" cellspacing="1"  border="0" width="100%">
                        <tr valign=top>
                            <td style="padding:5px;" class="label">Total Deposit:</td>
                            <td class="content" style="text-align:right;vertical-align:middle;padding:5px;"><?=getAmount($value["deposit"])?></td>
                            <td style="padding:5px;vertical-align:middle;" class="label">Total Withdrawal:</td>
                            <td class="content" style="text-align:right;vertical-align:middle;padding:5px;"><?=getAmount($value["withdrawal"])?></td>
                            <td style="padding:5px;vertical-align:middle;" class="label">Totals:</td>
                            <td class="content" colspan="2" style="text-align:right;vertical-align:middle;padding:5px;"><?=getAmount($value["total"])?></td>
                        </tr>
                        <tr><td colspan="7">&nbsp;</tr>
                        <tr height="20px">
                            <td class="label" style="padding:5px;">Trans.. ID</td><td class="label" style="padding:5px;">Date</td><td class="label" style="padding:5px;">Username</td><td class="label" style="padding:5px;">Transfer Code</td><td class="label" style="padding:5px;">Amount</td><td class="label" style="padding:5px;">Note</td><td class="label" style="padding:5px;">Status</td>
                        </tr>
                        <?php
                        $i = 0;
                        foreach ($value as $vKey => $vValue){
                            if(is_int($vKey)){
                                $rec = $vValue["data"];
                                $objPHPExcel->getActiveSheet()->SetCellValue('A'.$row, $rec["ttt_id"]);
                                $objPHPExcel->getActiveSheet()->SetCellValue('B'.$row, $rec["ttt_time"]);
                                $objPHPExcel->getActiveSheet()->SetCellValue('C'.$row, $rec["pun_username"]);
                                $objPHPExcel->getActiveSheet()->SetCellValue('D'.$row, $rec["ttt_transferid"]);
                                $objPHPExcel->getActiveSheet()->SetCellValue('E'.$row, $rec["ttt_amount"]);
                                $objPHPExcel->getActiveSheet()->getStyle('E'.$row)->getNumberFormat()->setFormatCode(PHPExcel_Style_NumberFormat::FORMAT_CURRENCY_EUR_SIMPLE);
                                $objPHPExcel->getActiveSheet()->SetCellValue('F'.$row, $rec["ttt_note"]);
                                $objPHPExcel->getActiveSheet()->SetCellValue('G'.$row, getExcelStatu($rec["ttt_status"],$rec["ttt_id"],$rec["ttm_code"]));
                                $row++
                                ?>
                                <tr height="20px" class="<?php if(($i % 2) == 0) {?> odd <?php }else{ ?> even <?php } ?>">
                                    <td width="80px" class="content"><?=$rec["ttt_id"]?></td>
                                    <td width="150px" style="text-align:center;" class="content"><?=$rec["ttt_time"]?></td>
                                    <td class="content"><?=getClassLink(refPage("customers/customer_view&customer_id=" . $rec["pun_id"]), $rec["pun_username"])?></td>
                                    <td  width="150px" class="content"><?=$rec["ttt_transferid"]?></td>
                                    <td  width="80px" class="content"><?=getAmount($rec["ttt_amount"])?></td>
                                    <td  width="200px" class="content"><?=$rec["ttt_note"]?></td>
                                    <td class="content"><?=getStatus($rec["ttt_status"],$rec["ttt_id"],$rec["ttm_code"])?></td>
                                </tr>
                                <?php
                                $i++;
                            }
                        }
                        $objPHPExcel->getActiveSheet()->SetCellValue('A'.$row, 'Total Deposit:');
                        $objPHPExcel->getActiveSheet()->getStyle('A'.$row)->getFont()->getColor()->setARGB(PHPExcel_Style_Color::COLOR_WHITE);
                        $objPHPExcel->getActiveSheet()->getStyle('A'.$row)->getFill()->setFillType(PHPExcel_Style_Fill::FILL_SOLID)->getStartColor()->setARGB('FF0000DD');
                        $objPHPExcel->getActiveSheet()->SetCellValue('B'.$row, $value["deposit"]);
                        $objPHPExcel->getActiveSheet()->getStyle('B'.$row)->getNumberFormat()->setFormatCode(PHPExcel_Style_NumberFormat::FORMAT_CURRENCY_EUR_SIMPLE);
                        $objPHPExcel->getActiveSheet()->SetCellValue('C'.$row, 'Total Withdrawal:');
                        $objPHPExcel->getActiveSheet()->getStyle('C'.$row)->getFont()->getColor()->setARGB(PHPExcel_Style_Color::COLOR_WHITE);
                        $objPHPExcel->getActiveSheet()->getStyle('C'.$row)->getFill()->setFillType(PHPExcel_Style_Fill::FILL_SOLID)->getStartColor()->setARGB('FF0000DD');
                        $objPHPExcel->getActiveSheet()->SetCellValue('D'.$row, $value["withdrawal"]);
                        $objPHPExcel->getActiveSheet()->getStyle('D'.$row)->getNumberFormat()->setFormatCode(PHPExcel_Style_NumberFormat::FORMAT_CURRENCY_EUR_SIMPLE);
                        $objPHPExcel->getActiveSheet()->SetCellValue('E'.$row, 'Totals:');
                        $objPHPExcel->getActiveSheet()->getStyle('E'.$row)->getFont()->getColor()->setARGB(PHPExcel_Style_Color::COLOR_WHITE);
                        $objPHPExcel->getActiveSheet()->getStyle('E'.$row)->getFill()->setFillType(PHPExcel_Style_Fill::FILL_SOLID)->getStartColor()->setARGB('FF0000DD');
                        $objPHPExcel->getActiveSheet()->SetCellValue('G'.$row, $value["total"]);
                        $objPHPExcel->getActiveSheet()->getStyle('G'.$row)->getNumberFormat()->setFormatCode(PHPExcel_Style_NumberFormat::FORMAT_CURRENCY_EUR_SIMPLE);
                        $row++;
                        $row++;
                        ?>


                    </table>
                <?php
                }
                $objPHPExcel->getActiveSheet()->setTitle('Reports Totem');

                $objWriter = new PHPExcel_Writer_Excel2007($objPHPExcel);
                $objWriter->save("../cache/".$_SESSION["jurisdiction_class"]."_".$date_start."_".$date_end."_totem.xlsx");
            }else{
                ?>
                <h4><font color="#DD0000" >No Result!!</font></h4>
            <?php
            }


        }
    }else{

        addError("", "Sorry! To long period. Maximum two months");
    }
}

$jurClass = JurisdictionsList::getInstance ( $dbh );
$clubList = $jurClass->getChilJurisdictionsDataList ( $_SESSION ['jurisdiction_id'], 'club' );
$clubIdList = array();
foreach($clubList as $key => $value){
    array_push($clubIdList, $value[0]);
}
$sql = "SELECT ttm_code from totem where ttm_club_id in (".implode(',',$clubIdList ).")";
$totemList = $dbh->exec($sql);


?>

    <script type="text/javascript">
        $(document).ready(function(){
            <?php

            if($chooseFor == "club"){
                 ?>
            chooseClub();
            <?php
       }else{
           ?>
            chooseTotem();
            <?php
        }

       ?>
        });
    </script>

    <hr />
    <center>
        <div class="bodyHD">Totem Transaction:</div>
        <form action="<?=$_SERVER['PHP_SELF']."?".$_SERVER['QUERY_STRING']?>"
              method="POST" name="search">
            <input type="hidden" name="do_search" value="1" />
            <table>
                <tr>
                    <td class="label">Search For:</td>
                    <td class="content">
                        <input type="radio" <?php if($chooseFor == "club") { ?> checked <?php } ?>  onclick="chooseClub();" name="choosefor" value="club" >Club</input>
                        <input type="radio" <?php if($chooseFor == "totem") { ?> checked <?php } ?>  onclick="chooseTotem();" name="choosefor" value="totem" >Totem</input>
                    </td>
                </tr>
                <tr id="chooseforclub">
                    <td class="label">Club:</td>
                    <td class="content">
                        <select name="ttclub">
                            <?php
                            foreach($clubList as $key => $value){
                                ?><option <?php if($totemClub == $value[0]) {?> selected <?php }?> value="<?=$value[0]?>" ><?=$value[1]?></option><?php
                            }
                            ?>
                        </select>
                    </td>
                </tr>
                <tr id="choosefortotem">
                    <td class="label">TotemCode:</td>
                    <td class="content">
                        <select name="ttcode">
                            <?php
                            while($totemList->hasNext()){
                                $row = $totemList->next();
                                ?><option <?php if($totemCode == $row['ttm_code']) {?> selected <?php }?> value="<?=$row['ttm_code']?>" ><?=$row['ttm_code']?></option><?php
                            }
                            ?>
                        </select>
                    </td>
                </tr>
                <tr>
                    <td class="label">Start Date:</td>
                    <td class="content">
                        <?=createDateInput("date_start", $date_start, false, "search", true)?></td>
                </tr>
                <tr>
                    <td class="label">End Date:</td>
                    <td class="content">
                        <?=createDateInput("date_end", $date_end, false, "search", true)?></td>
                </tr>

                <tr id="resinpage">
                    <td class="label">Result in Page</td>
                    <td class="content"><select name="resforpage">
                            <option <?php if($row_to_page == 10){?> selected <?php }?>
                                value="10">10</option>
                            <option <?php if($row_to_page == 30){?> selected <?php }?>
                                value="30">30</option>
                            <option <?php if($row_to_page == 50){?> selected <?php }?>
                                value="50">50</option>
                            <option <?php if($row_to_page == 80){?> selected <?php }?>
                                value="80">80</option>
                            <option <?php if($row_to_page == 100){?> selected <?php }?>
                                value="100">100</option>
                        </select></td>
                </tr>
                <tr>
                    <td>&nbsp;</td>
                    <td><input type="submit" value="Submit" /></td>
                </tr>
            </table>
        </form>
    </center>
<?=showErrors();?>