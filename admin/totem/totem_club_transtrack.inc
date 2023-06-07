<?php
$trans_totem=check_access ( 'ttm_totem_daily_transtrack');
if($trans_totem==true){
include_once 'JurisdictionsList.class.inc';
require_once 'PHPExcel.php';
require_once 'PHPExcel/Writer/Excel2007.php';
require_once 'PHPExcel/Style/Fill.php';
require_once 'PHPExcel/Style/Color.php';
?>
<script type="text/javascript">
<!--
 function acceptPending(ttID, tCode){
		$.ajax({  type: 'POST',
			  url:  'totem/transack_accept.php',
			  data: {pAccept:1,tr_id:ttID,tcode:tCode},
			  success: function(data){
				  if(data == '-100'){
						alert("<?=$lang->getLang("The request wasn't accepted")?>");
				  }else if(data == "0"){
						alert('<?=$lang->getLang("The request was successfully resolved.The totem is now available")?>');
				  }else{
					  	alert("<?=$lang->getLang("The totem can't be reached at this moment")?>");
				  }
				  getClubDetails('<?=$_POST['totemClub']?>','<?=$_POST['jur_class']?>');			     				 
			  },
			  error: function(){
				  alert("<?=$lang->getLang('Communication Error')?>");
			  }			  
			});
 }

//-->
</script>
<?php

$chooseFor = "club";

function getAmount($amount) {
	if ($amount >= 0) {
		return '<FONT COLOR="#000000">' . getInDollars ( $amount ) . '</FONT>';
	} else {
		return '<FONT COLOR="#FF0000">' . getInDollars ( $amount ) . '</FONT>';
	}
}

function getStatus($status, $tttID, $tCode){
	global $lang;
    if($status == "C"){
		return "<FONT COLOR='green'>".$lang->getLang("Completed")."</FONT>";
	}else if($status == "P"){			
		return "<FONT COLOR='orange'>".$lang->getLang("Pending")."</FONT><button type='button' onClick='acceptPending(\"$tttID\", \"$tCode\");' name='pAccept' value='Accept'>".$lang->getLang("Accept")."</button>";
	}
}

function getExcelStatu($status, $tttID, $tCode){
	global $lang;
    if($status == "C"){
		return $lang->getLang("Completed");
	}else if($status == "P"){
		return $lang->getLang("Pending");
	}
}

if (isset ( $_GET ['ttcode'] )) {
	$totemClub = $_GET ['ttcode'];
	$date_start = date('Y-m-d H:i:s', mktime(0,0,0,date('m'),date('d')-1,date('Y')));
	$date_end =  date('Y-m-d H:i:s', mktime(23,59,59,date('m'),date('d'),date('Y')));
} else if (isset ( $_POST ['totemClub'] )) {
	$totemClub = $_POST ['totemClub'];
	$date_start = $_POST["date_club"]." 00:00:00";
	$date_end = $_POST["date_club"]. " 23:59:59";
} else {
	$totemClub = '';
	$date_start = date('Y-m-d H:i:s', mktime(0,0,0,date('m'),date('d')-1,date('Y')));
	$date_end =  date('Y-m-d H:i:s', mktime(23,59,59,date('m'),date('d'),date('Y')));
}
if (isset ( $_POST ['do_search'] )) {
	$starttime = microtime(true);
	$totemClub=$_POST['totemClub'];
	$sql = "SELECT SUM(IF(ttt_amount > 0 ,ttt_amount ,0)) as plus, SUM(IF(ttt_amount < 0 ,ttt_amount ,0)) as minus, sum(ttt_tax) as tax
				       FROM totem_transtrack
								LEFT JOIN totem on ttm_id = ttt_ttm_id
								LEFT JOIN punter on ttt_pun_id = pun_id
		 						WHERE ttm_club_id = " . $totemClub . " AND
								ttt_time BETWEEN ".$dbh->quote($date_start)."
								AND  " .$dbh->quote($date_end) . " order by ttt_id, ttt_time ASC ";
$sumRes = $dbh->queryRow($sql);

$sql = "Select  ttt_id,ttm_code, ttm_access,
				                ttt_ttm_id,
								ttt_time,
								ttt_pun_id,
								ttt_transferid,
								ttt_amount,
								ttt_note,
								ttt_status,
								ttt_tax,
								ttt_tax_perc,
								pun_id,
								pun_username ,
								ttm_code
								FROM totem_transtrack
								LEFT JOIN totem on ttm_id = ttt_ttm_id
								LEFT JOIN punter on ttt_pun_id = pun_id
		 						WHERE ttm_club_id = " . $totemClub . " AND
								ttt_time BETWEEN ".$dbh->quote($date_start)."
								AND  " .$dbh->quote($date_end) . "  order by ttm_code, ttt_id, ttt_time ASC";

$endtime = microtime(true);
$duration = $endtime - $starttime;
$duration=number_format($duration, 4, ',', '')."s";
?>
 <script>duration('<?=$duration?>','duration_transtrack');</script>
<?
$totems = array();
$actualTotem = null;
$plusAmountTotem = 0;
$minusAmountTotem = 0;
$tax = 0;
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
				$totems[$actualTotem]["tax"] = ($tax );
				$plusAmountTotem = 0;
				$minusAmountTotem = 0;
				$tax = 0;
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
		$tax+=$row['ttt_tax'];
	}
	$totems[$actualTotem]["deposit"] = $plusAmountTotem;
	$totems[$actualTotem]["withdrawal"] = $minusAmountTotem;
	$totems[$actualTotem]["total"] = ($plusAmountTotem + $minusAmountTotem);
	$totems[$actualTotem]["tax"] = ($tax);
	$plusAmountTotem = 0;
	$minusAmountTotem = 0;
	$tax = 0;
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
					<div style="text-align:right;margin-right:10px;margin-bottom:10px;"><a href='<?="../cache/".$_SESSION["jurisdiction_class"]."_".$date_start."_".$date_end."_totem.xlsx"?>'><img src="<?= secure_host ?>/media/images/icon_download.jpg" width="16" height="16"/> <?=$lang->getLang("Download excel file")?> </a></div>
					<h2>Totals Period: <?=$date_start?> To <?=$date_end?></h2>
					<table>
					  <tr>
					    <td style="padding:5px;" class="label"><?=$lang->getLang("Total Deposit")?>:</td>
					    <td style="padding:5px;" class="label"><?=$lang->getLang("Total Withdrawal")?>:</td>
					    <td style="padding:5px;" class="label"><?=$lang->getLang("Totals")?>:</td>
					    <td style="padding:5px;" class="label"><?=$lang->getLang("Tax")?>:</td>
					  </tr>
					  <tr>
					    <td style="text-align:right;vertical-align:middle;padding:5px; class="content"><?=getAmount($sumRes['plus'])?></td>
					    <td style="text-align:right;vertical-align:middle;padding:5px; class="content"><?=getAmount($sumRes['minus'])?></td>
					    <td style="text-align:right;vertical-align:middle;padding:5px; class="content"><?=getAmount(($sumRes['plus'] + $sumRes['minus']))?></td>
					    <td style="text-align:right;vertical-align:middle;padding:5px; class="content"><?=getAmount(($sumRes['tax']))?></td>
					  </tr>
					</table>
					<?php 	
					$row = 1;
					foreach ($totems as $key => $value){
						
						$objPHPExcel->getActiveSheet()->SetCellValue('B'.$row, "Report Totem Code: " . $key);
						$objPHPExcel->getActiveSheet()->mergeCells('B'.$row.':G'.$row);
						$objPHPExcel->getActiveSheet()->getStyle('B'.$row)->getFont()->getColor()->setARGB(PHPExcel_Style_Color::COLOR_WHITE);
						$objPHPExcel->getActiveSheet()->getStyle('B'.$row)->getFill()->setFillType(PHPExcel_Style_Fill::FILL_SOLID)->getStartColor()->setARGB('FFFF0000');
						$row++;
						$objPHPExcel->getActiveSheet()->SetCellValue('B'.$row, 'DATE');
						$objPHPExcel->getActiveSheet()->SetCellValue('C'.$row, 'USERNAME');
						$objPHPExcel->getActiveSheet()->SetCellValue('D'.$row, 'TRANSFER CODE');
						$objPHPExcel->getActiveSheet()->SetCellValue('E'.$row, 'AMOUNT');
						$objPHPExcel->getActiveSheet()->SetCellValue('F'.$row, 'NOTE');
						$objPHPExcel->getActiveSheet()->SetCellValue('G'.$row, 'STATUS');
						$objPHPExcel->getActiveSheet()->getColumnDimension('B')->setAutoSize(true);
						$objPHPExcel->getActiveSheet()->getColumnDimension('C')->setAutoSize(true);
						$objPHPExcel->getActiveSheet()->getColumnDimension('D')->setAutoSize(true);
						$objPHPExcel->getActiveSheet()->getColumnDimension('E')->setAutoSize(true);
						$objPHPExcel->getActiveSheet()->getColumnDimension('F')->setAutoSize(true);
						$objPHPExcel->getActiveSheet()->getColumnDimension('G')->setAutoSize(true);
						$objPHPExcel->getActiveSheet()->getStyle('B'.$row.':G'.$row)->getFont()->getColor()->setARGB(PHPExcel_Style_Color::COLOR_WHITE);
						$objPHPExcel->getActiveSheet()->getStyle('B'.$row.':G'.$row)->getFill()->setFillType(PHPExcel_Style_Fill::FILL_SOLID)->getStartColor()->setARGB('FF0000DD');
						$row++
						?>
						 <hr>
						 <h4>Report Totem Code <?=$key?></h4>
						 <table cellpadding="0" cellspacing="1"  border="0" width="100%">
						     <tr valign=top>
			                   <td style="padding:5px;" class="label"><?=$lang->getLang("Total Deposit")?>:</td>
			                   <td class="content" style="text-align:right;vertical-align:middle;padding:5px;"><?=getAmount($value["deposit"])?></td>
			                   <td style="padding:5px;vertical-align:middle;" class="label"><?=$lang->getLang("Total Withdrawal")?>:</td>
			                   <td class="content" style="text-align:right;vertical-align:middle;padding:5px;"><?=getAmount($value["withdrawal"])?></td>
			                   <td style="padding:5px;vertical-align:middle;" class="label"><?=$lang->getLang("Totals")?>:</td>
			                   <td class="content"  style="text-align:right;vertical-align:middle;padding:5px;"><?=getAmount($value["total"])?></td>
                                 <td style="padding:5px;vertical-align:middle;" class="label"><?=$lang->getLang("Tax")?>:</td>
			                   <td class="content"  style="text-align:right;vertical-align:middle;padding:5px;white-space: nowrap"><?=getAmount($value["tax"])?></td>
			               </tr>
						  <tr><td colspan="8">&nbsp;</tr>
						  <tr height="20px">
						  <td class="label" style="padding:5px;"><?=$lang->getLang("Date")?></td><td class="label" style="padding:5px;"><?=$lang->getLang("Totem")?></td><td class="label" style="padding:5px;"><?=$lang->getLang("Transfer Code")?></td><td class="label" style="padding:5px;"><?=$lang->getLang("Amount")?></td><td class="label" style="padding:5px;"><?=$lang->getLang("Note")?></td><td class="label" style="padding:5px;"><?=$lang->getLang("Status")?></td><td class="label" style="padding:5px;" colspan="2"><?=$lang->getLang("Tax")?>(%)</td>
						  </tr>
						  <?php 
						     $i = 0;
			                 foreach ($value as $vKey => $vValue){
			                 	if(is_int($vKey)){
			                 		$rec = $vValue["data"];
			                 		$objPHPExcel->getActiveSheet()->SetCellValue('B'.$row, $rec["ttt_time"]);
			                 		$objPHPExcel->getActiveSheet()->SetCellValue('C'.$row, $rec["ttm_code"]);
			                 		$objPHPExcel->getActiveSheet()->SetCellValue('D'.$row, $rec["ttt_transferid"]);
			                 		$objPHPExcel->getActiveSheet()->SetCellValue('E'.$row, $rec["ttt_amount"]);
			                 		$objPHPExcel->getActiveSheet()->getStyle('E'.$row)->getNumberFormat()->setFormatCode(PHPExcel_Style_NumberFormat::FORMAT_CURRENCY_EUR_SIMPLE);
			                 		$objPHPExcel->getActiveSheet()->SetCellValue('F'.$row, $rec["ttt_note"]);
			                 		$objPHPExcel->getActiveSheet()->SetCellValue('G'.$row, getExcelStatu($rec["ttt_status"],$rec["ttt_id"],$rec["ttm_code"]));
			                 		$row++
			                 		?>
			                 		<tr height="20px" class="<?php if(($i % 2) == 0) {?> odd <?php }else{ ?> even <?php } ?>">
			                 			<td width="150px" style="text-align:center;" class="content"><?=$rec["ttt_time"]?></td>
			                 			<td class="content" style="color:<?if(strtoupper($rec['ttm_access'])=='ALLOW'){?>green<?}else{?>red<?}?>"><?=$rec["ttm_code"]?></td>
			                 			<td  width="150px" class="content"><?=$rec["ttt_transferid"]?></td>
			                 			<td  width="80px" class="content"><?=getAmount($rec["ttt_amount"])?></td>
			                 			<td  width="200px" class="content"><?=$rec["ttt_note"]?></td>
			                 			<td class="content"><?=getStatus($rec["ttt_status"],$rec["ttt_id"],$rec["ttm_code"])?></td>
			                 			<td colspan="2" class="content"><?=$rec["ttt_tax"]."(".$rec["ttt_tax_perc"].")"?></td>
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
					<h4><font color="#DD0000" ><?=$lang->getLang("No Result!")?></font></h4>
					<?php 
				}				
			}
}else{?>
<div><?=$lang->getLang("You are not allowed to see this page!")?></div>
<?}?>
</div>
<?=showErrors();?>