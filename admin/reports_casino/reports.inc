<?php 
check_access('view_payment_transactions',true);

?>

<script src="<?= secure_host ?>/media/highcharts.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/modules/exporting.js" type="text/javascript"></script>

<div class="bodyHD" align="center">Financial Reports</div>
<br/>
<?php form_head()?>
<?php printHiddenInput('doQuery', 'yes');?>
<?php printHiddenInput('search_type', 'date_range');?>

<?php  
    require_once 'JurisdictionsList.class.inc';
    require_once 'PHPExcel.php';
    require_once 'PHPExcel/Writer/Excel2007.php';
    require_once 'PHPExcel/Style/Fill.php';
    require_once 'PHPExcel/Style/Color.php';
	$doQuery = $_POST['doQuery'];
	$date_start = dbDateToday();
	$date_end = dbDateToday();

	if($doQuery){
		$date_start = $_POST['start_year'] . "-" . $_POST['start_month'] . "-" . $_POST['start_day'];
		$date_end = $_POST['end_year'] . "-" . $_POST['end_month'] . "-" . $_POST['end_day'];
	}

	$jurClass = JurisdictionsList::getInstance ( $dbh );
	$clubList = $jurClass->getChilJurisdictionsDataList ( $_SESSION ['jurisdiction_id'], 'club' );
	$whichClub = "*";
	if(isset($_POST['clubname'])){
		$whichClub = $_POST['clubname'];
	}
?>

<table align="center" cellpadding="4" cellspacing="1" border="0">
  <tr valign=top>
    <td class=label>Start Date</td>
    <td class=content>
	<?php date_selector('start_day', 'start_month', 'start_year', 2006, date('Y'),  $date_start); ?>
    </td>
  </tr>
  <tr valign=top>
    <td class=label><?=$lang->getLang("End Date")?></td>
    <td class=content>
	<?php date_selector('end_day', 'end_month', 'end_year', 2006,  date('Y'), $date_end);?>
    </td>
  </tr>
  <tr>
       <td class="label"><?=$lang->getLang("Select the club")?></td>
       <td class="content">
  				<select name="clubname">
  				<option <?php if($whichClub == "*"){?> selected <?php }?> value="*"><?=$lang->getLang("All clubs")?></option>
  				<?php 
  				foreach($clubList as $k=>$v)
  				{
  				  ?> <option <?php if($whichClub == $v[0]){?> selected <?php }?> value="<?=$v[0]?>"><?=$v[1]?></option>	<?php 
  				}
  				?>
  				</select>     
       </td>
     </tr>
  <tr>
    <td class=content>&nbsp;</td>
    <td class=content><input type="submit" value="Calculate" /></td>
  </tr>
  </table>
<?php
  if($doQuery){

  	$datetime1 = new DateTime($date_start);
  	$datetime2 = new DateTime($date_end);
  	$interval = $datetime1->diff($datetime2);
  	$diff = $interval->format('%a');
  	if($diff > 120){
  		?>
  		<div ><?=$lang->getLang("You select more then 120 days of difference")?></div>
  		<?php
  		return;
  	}
  	 $generalData = array();
  	 
  	 $idList = array();
  	 if($whichClub == "*"){
  	 	for($i = 0 ; $i < count($clubList); $i++){
  	 		array_push($idList, $clubList[$i][0]);
  	 	}
  	 }else{
  	 	for($i = 0 ; $i < count($clubList); $i++){
  	 		if($whichClub == $clubList[$i][0]){
  	 			array_push($idList, $whichClub);
  	 		}
  	 	}
  	 }
  	 
  	 if(count($idList) <= 0){
  	 	for($i = 0 ; $i < count($clubList); $i++){
  	 		array_push($idList, $clubList[$i][0]);
  	 	}
  	 }
  	 
  	 
  	 $sql = "SELECT pun_betting_club AS clubid, jur_name as clubname,
  	 			    cus_csh_day, 
  	 			    sum( cus_csh_amount ) AS financial
					  FROM customer_cashdesk_daily
					   JOIN punter ON cus_csh_pun_id = pun_id
					   JOIN jurisdiction ON jur_id = pun_betting_club
						WHERE cus_csh_day BETWEEN date_format( '$date_start 00:00:00', '%Y-%m-%d %H:%i:%s' )
							  AND date_format( '$date_end 23:59:59', '%Y-%m-%d %H:%i:%s' )
							  AND pun_betting_club in (". implode(",", $idList) .")
							GROUP BY pun_betting_club, cus_csh_day
  	 					   ORDER BY pun_betting_club, cus_csh_day";
  	 
  	 $resultFinancial = $dbh->exec($sql);
  	 $totalFinancial = 0;
  	 $lastClub = "";
  	 while($resultFinancial->hasNext()){
  	 	$rec = $resultFinancial->next();
  	 	if($lastClub != $rec['clubid']){
  	 		if($lastClub != ""){
  	 			$generalData[$lastClub."_total"] = array('clubid' => $lastClub,
  	 														  'totalFinancial' => $totalFinancial);
  	 			$totalFinancial  = 0;
  	 		}
  	 		$lastClub = $rec['clubid'];
  	 	}else{
  	 		if(!$resultFinancial->hasNext()){
  	 			$generalData[$lastClub."_total"] = array('clubid' => $lastClub,
  	 														   'totalFinancial' => $totalFinancial);
  	 			$totalFinancial = 0;
  	 		}
  	 	}
  	 	$totalFinancial += $rec['financial'];
  	 	$generalData[$rec['clubid']."_".$rec['cus_csh_day']] = array('clubid' => $rec['clubname'],
  	 										 'date' => $rec['cus_csh_day'],
  	 										 'financial' => $rec['financial']);
  	 }
  	 $sql = "SELECT pun_betting_club AS clubid, 
  	          cus_res_gam_id as gameid , jur_name as clubname,
  	          date_format(cus_res_day, '%Y-%m-%d') as cus_day, 
  	          sum(cus_res_bet) as totalbet, 
  	          sum(cus_res_win) as totalwin
          		FROM customer_result_daily
          		JOIN punter ON cus_res_pun_id = pun_id
          		JOIN jurisdiction ON jur_id = pun_betting_club
          		 WHERE cus_res_day BETWEEN date_format('$date_start 00:00:00', '%Y-%m-%d %H:%i:%s') AND date_format('$date_end 23:59:59', '%Y-%m-%d %H:%i:%s')
          		 AND pun_betting_club in (". implode(",", $idList) .")
                 GROUP BY pun_betting_club, 
  	                      date_format(cus_res_day, '%Y-%m-%d') , 
  	                      cus_res_gam_id
  	            ORDER BY pun_betting_club, date_format( cus_res_day, '%Y-%m-%d' ) , cus_res_gam_id ";
  	 $resultGames = $dbh->exec($sql);
  	 $clubsReport = array();
  	 $lastClub = "";
  	 $totalBetLive = 0;
  	 $totalWinLive = 0;
  	 $totalBetCasino = 0;
  	 $totalWinCasino = 0;
  	 $totalBet = 0;
  	 $totalWin = 0;
  	 while($resultGames->hasNext()){
  	 	$rec = $resultGames->next();
  	 	if($lastClub != $rec['clubid']){
  	 		if($lastClub != ""){
  	 			$totalBet = $totalBetLive + $totalBetCasino;
  	 			$totalWin = $totalWinLive + $totalWinCasino;
  	 			$clubsReport[$lastClub."_total"] = array('clubid' => $lastClub,
  	 					'ttbet_live' => $totalBetLive,
  	 					'ttwin_live' => $totalWinLive,
  	 					'ttbet_cs' => $totalBetCasino,
  	 					'ttwin_cs' => $totalWinCasino,
  	 					'totalBet' => $totalBet,
  	 					'totalWin' => $totalWin);
  	 			
  	 			$totalBetLive = 0;
  	 			$totalWinLive = 0;
  	 			$totalBetCasino = 0;
  	 			$totalWinCasino = 0;
  	 			$totalBet = 0;
  	 			$totalWin = 0;
  	 		}
  	 		$lastClub = $rec['clubid'];
  	 	}else{
  	 		if(!$resultGames->hasNext()){
  	 			$totalBet = $totalBetLive + $totalBetCasino;
  	 			$totalWin = $totalWinLive + $totalWinCasino;
  	 			$clubsReport[$lastClub."_total"] = array('clubid' => $lastClub,
  	 					'ttbet_live' => $totalBetLive,
  	 					'ttwin_live' => $totalWinLive,
  	 					'ttbet_cs' => $totalBetCasino,
  	 					'ttwin_cs' => $totalWinCasino,
  	 					'totalBet' => $totalBet,
  	 					'totalWin' => $totalWin);
  	 			 
  	 			$totalBetLive = 0;
  	 			$totalWinLive = 0;
  	 			$totalBetCasino = 0;
  	 			$totalWinCasino = 0;
  	 			$totalBet = 0;
  	 		}
  	 	}
  	 	if(is_null($clubsReport[$rec['clubid']."_".$rec['cus_day']])){
  	 		
  	 		$clubsReport[$rec['clubid']."_".$rec['cus_day']] = array('clubid' => $rec['clubname'],
  	 										     				 'date' => $rec['cus_day']);
  	 	}
  	 	$gameModule = ($rec['gameid'] - ($rec['gameid'] % 1000));
  	 	if($gameModule >= 6000 && $gameModule <= 8000){
  	 		$totalBetLive += $rec['totalbet'];
  	 		$totalWinLive += $rec['totalwin'];
  	 		$clubsReport[$rec['clubid']."_".$rec['cus_day']]['live_ttbet'] += $rec['totalbet'];
  	 		$clubsReport[$rec['clubid']."_".$rec['cus_day']]['live_ttwin'] += $rec['totalwin'];
  	 	}else{
  	 		$totalBetCasino += $rec['totalbet'];
  	 		$totalWinCasino += $rec['totalwin'];
  	 		$clubsReport[$rec['clubid']."_".$rec['cus_day']]['cs_ttbet'] += $rec['totalbet'];
  	 		$clubsReport[$rec['clubid']."_".$rec['cus_day']]['cs_ttwin'] += $rec['totalwin'];
  	 	}
  	 }
  	 
  	 foreach ($clubsReport as $key => $value){
  	 	if(strpos($key,'total') > 0){
  	 		$clubsReport[$key]['totalFinancial'] = $generalData[$key]['totalFinancial'];
  	 	}else{
  	 		$clubsReport[$key]['financial'] = $generalData[$key]['financial'];
  	 	}
  	 }
  	// echo "<pre>";
  	// var_dump($clubsReport);
  	// echo "</pre>";
  	 if($clubsReport != array()){

  	 	$objPHPExcel = new PHPExcel();
  	 	$objPHPExcel->getProperties()->setCreator("CsLiveGames");
		$objPHPExcel->getProperties()->setLastModifiedBy("CsLiveGames");
		$objPHPExcel->getProperties()->setTitle("Financial Reports to " . $date_start . " from " . $date_end);
		
  	 	
		$objPHPExcel->setActiveSheetIndex(0);
		$objPHPExcel->getActiveSheet()->SetCellValue('A1', 'CLUB NAME');
		$objPHPExcel->getActiveSheet()->SetCellValue('B1', 'DATE');
		$objPHPExcel->getActiveSheet()->SetCellValue('C1', 'FINANCIAL');
		$objPHPExcel->getActiveSheet()->SetCellValue('D1', 'LIVE BET');
		$objPHPExcel->getActiveSheet()->SetCellValue('E1', 'LIVE WIN');
		$objPHPExcel->getActiveSheet()->SetCellValue('F1', 'CASINO BET');
		$objPHPExcel->getActiveSheet()->SetCellValue('G1', 'CASINO WIN');
		$objPHPExcel->getActiveSheet()->getColumnDimension('A')->setAutoSize(true);
		$objPHPExcel->getActiveSheet()->getColumnDimension('B')->setAutoSize(true);
		$objPHPExcel->getActiveSheet()->getColumnDimension('C')->setAutoSize(true);
		$objPHPExcel->getActiveSheet()->getColumnDimension('D')->setAutoSize(true);
		$objPHPExcel->getActiveSheet()->getColumnDimension('E')->setAutoSize(true);
		$objPHPExcel->getActiveSheet()->getColumnDimension('F')->setAutoSize(true);
		$objPHPExcel->getActiveSheet()->getColumnDimension('G')->setAutoSize(true);
		$objPHPExcel->getActiveSheet()->getStyle("A1:G1")->getFont()->getColor()->setARGB(PHPExcel_Style_Color::COLOR_WHITE);
		$objPHPExcel->getActiveSheet()->getStyle("A1:G1")->getFill()->setFillType(PHPExcel_Style_Fill::FILL_SOLID)->getStartColor()->setARGB('FF0000DD');
		//$objPHPExcel->getActiveSheet()->getStyle('A1:G1')->getFill()->setFillStyle(PHPExcel_Style_Fill::FILL_SOLID);
		?>
  	 	 <style>
          .odd td{
    		background-color:#FFF;
 		  }
  		  .even td{
    		background-color:#EEE;
  		  }
		</style>
		  <div style="text-align:right;margin-right:150px;margin-bottom:10px;"><a href='<?="../cache/".$_SESSION["jurisdiction_class"]."_".$date_start."_".$date_end.".xlsx"?>'><img src="<?= secure_host ?>/media/images/icon_download.jpg" width="16" height="16"/> Download excel file</a></div>
  	 	  <table align="center" bgcolor="#a99a9" cellpadding="4" cellspacing="1" border="0">
  	 	     <HEAD>
  	 	      <tr valign="top" align="center"><th class="label"><?=$lang->getLang("Club name")?></th><th class="label"><?=$lang->getLang("DATE")?></th><th class="label"><?=$lang->getLang("Financial")?></th><th class="label"><?=$lang->getLang("Live betting")?></th><th class="label"><?=$lang->getLang("Live win")?></th><th class="label"><?$lang->getLang("Casino Bet")?></th><th class="label"><?=$lang->getLang("Casino win")?></th></tr>
  	 	     </HEAD>
  	 	     <BODY>
  	 	     <?php 
  	 	        $row = 2;
  	 	        $days = array();
  	 	        $clubs = array();
  	 	        foreach ($clubsReport as $key => $value){
  	 	            $class = ($row % 2 == 0) ? "odd" : "even"; 
  	 	            if(!strpos($key,'total')){
  	 	            	$days[$value['date']] = $value['date'];
  	 	            	if($whichClub != "*"){
  	 	            	  $clubs[$value['clubid']."_Casino"][$value['date']] = ($value['cs_ttbet'] -  $value['cs_ttwin']);
  	 	            	  $clubs[$value['clubid']."_Live"][$value['date']] = ($value['live_ttbet']  - $value['live_ttwin']);
  	 	            	} 
  	 	            	$clubs[$value['clubid']."_Total"][$value['date']] = (($value['live_ttbet'] + $value['cs_ttbet']) - ($value['live_ttwin'] + $value['cs_ttwin']));
  	 	            	
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("A".$row, $value['clubid']);
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("B".$row, $value['date']);
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("C".$row, $value['financial']);
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("D".$row, $value['live_ttbet']);
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("E".$row, $value['live_ttwin']);
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("F".$row, $value['cs_ttbet']);
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("G".$row, $value['cs_ttwin']);
  	 	            	
  	 	            	
  	 	            ?>
  	 	        	  <tr class="<?=$class?>">
  	 	        	    <td><?=$value['clubid']?></td>
  	 	        	    <td><?=$value['date']?></td>
  	 	        	    <td><?=getInDollars($value['financial'])?></td>
  	 	        	    <td><?=getInDollars($value['live_ttbet'])?></td>
  	 	        	    <td><?=getInDollars($value['live_ttwin'])?></td>
  	 	        	    <td><?=getInDollars($value['cs_ttbet'])?></td>
  	 	        	    <td><?=getInDollars($value['cs_ttwin'])?></td>
  	 	        	   </tr>
  	 	              <?php
  	 	            }else{
  	 	            	
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("A".$row, "");
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("B".$row, "");
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("C".$row, "TOTAL FINANCIAL");
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("D".$row, "TOTAL LIVE BET");
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("E".$row, "TOTAL LIVE WIN");
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("F".$row, "TOTAL CASINO BET");
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("G".$row, "TOTAL CASINO WIN");
  	 	            	$objPHPExcel->getActiveSheet()->getStyle("C".$row.":G".$row)->getFont()->getColor()->setARGB(PHPExcel_Style_Color::COLOR_WHITE);
  	 	            	$objPHPExcel->getActiveSheet()->getStyle("C".$row.":G".$row)->getFill()->setFillType(PHPExcel_Style_Fill::FILL_SOLID)->getStartColor()->setARGB('FF0000DD');
  	 	            	$row++;
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("A".$row, "");
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("B".$row, "");
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("C".$row, $value['totalFinancial']);
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("D".$row, $value['ttbet_live']);
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("E".$row, $value['ttwin_live']);
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("F".$row, $value['ttbet_cs']);
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("G".$row, $value['ttwin_cs']);
  	 	            	$row++;
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("A".$row, "");
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("B".$row, "");
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("C".$row, "");
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("D".$row, "TOTAL BET");
  	 	            	$objPHPExcel->getActiveSheet()->getStyle("D".$row)->getFont()->getColor()->setARGB(PHPExcel_Style_Color::COLOR_WHITE);
  	 	            	$objPHPExcel->getActiveSheet()->getStyle("D".$row)->getFill()->setFillType(PHPExcel_Style_Fill::FILL_SOLID)->getStartColor()->setARGB('FF0000DD');
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("E".$row, $value['totalBet']);
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("F".$row, "TOTAL WIN");
  	 	            	$objPHPExcel->getActiveSheet()->getStyle("F".$row)->getFont()->getColor()->setARGB(PHPExcel_Style_Color::COLOR_WHITE);
  	 	            	$objPHPExcel->getActiveSheet()->getStyle("F".$row)->getFill()->setFillType(PHPExcel_Style_Fill::FILL_SOLID)->getStartColor()->setARGB('FF0000DD');
  	 	            	$objPHPExcel->getActiveSheet()->SetCellValue("G".$row, $value['totalWin']);
  	 	            	$row++;
  	 	            	?>
  	 	            	 <tr>
  	 	            	    <td class="content">&nbsp;</td>
  	 	            	    <td class="content">&nbsp;</td>
  	 	            	    <td class="label">TOTAL FINANCIAL</td>
  	 	            	    <td class="label">TOTAL LIVE BET</td>
  	 	            	    <td class="label">TOTAL LIVE WIN</td>
  	 	            	    <td class="label">TOTAL CASINO BET</td>
  	 	            	    <td class="label">TOTAL CASINO WIN</td>
  	 	            	 </tr>
  	 	            	 <tr class="<?=$class?>">
  	 	            	    <td>&nbsp;</td>
  	 	            	    <td>&nbsp;</td>
  	 	            	    <td><?=getInDollars($value['totalFinancial'])?></td>
  	 	            	    <td><?=getInDollars($value['ttbet_live'])?></td>
  	 	            	    <td><?=getInDollars($value['ttwin_live'])?></td>
  	 	            	    <td><?=getInDollars($value['ttbet_cs'])?></td>
  	 	            	    <td><?=getInDollars($value['ttwin_cs'])?></td>
  	 	            	 </tr>
  	 	            	 <tr>
  	 	            	    <td class="content">&nbsp;</td>
  	 	            	    <td class="content">&nbsp;</td>
  	 	            	    <td class="content">&nbsp;</td>
  	 	            	    <td class="label">TOTAL BET</td>
  	 	            	    <td class="content"><?=getInDollars($value['totalBet'])?></td>
  	 	            	    <td class="label">TOTAL WIN</td>
  	 	            	    <td class="content"><?=getInDollars($value['totalWin'])?></td>
  	 	            	 </tr>
  	 	            	 <tr class="<?=$class?>">
  	 	            	    <td colspan="7">&nbsp;</td>
  	 	            	    
  	 	            	 </tr>
  	 	            	<?php
  	 	            }
  	 	            $row++;
  	 	        }
  	 	        sort($days, SORT_ASC);
  	 	        //var_dump($clubs);
  	 	      ?>
  	 	     </BODY>
  	 	  </table>
  	 	  <div id="container"  style="min-width: 1000px; height: 450px; margin: 0 auto"></div>
  	 	  <script type="text/javascript">
             var chart;
             var daysSt = "<?=implode(",",$days)?>";
             var days = daysSt.split(",");
            chart = new Highcharts.Chart({
		    chart: {
		 	   renderTo: 'container',
			   type: 'spline',
			   zoomType:'x'
		    },
		    credits:{
          		text: ''
      		},
		    title: {
			  text: 'Daily Average Rake',
			  x: -20 //center
		    },
		    subtitle: {
			   text: '',
			   x: -20
		    },
		    xAxis: {
			    //categories: days,
			    type: 'datetime',
			    dateTimeLabelFormats: { // don't display the dummy year
					month: '%e. %b',
					year: '%b'
				}
		    },
		    yAxis: {
			    title: {
				text: 'Rake'
			},
			plotLines: [{
				value: 0,
				width: 1,
				color: '#808080'
			}]
		   },
		   tooltip: {
			formatter: function() {
					return '<b>'+ this.series.name +'</b><br/>'+
					Highcharts.dateFormat('%e. %b', this.x) +': '+ this.y ;
			}
		   },
		  
		   series: [
			 <?php 
			    foreach($clubs as $name => $value){
			       ?>
			       {
				       name: "<?=$name?>",
				       data: [
			           <?php 
				         foreach($value as $d => $rake){
				         	list($year, $month, $day) = explode("-",$d);
				         	$month = $month - 1;
				            ?>
				         	 [ Date.UTC(<?=$year?>,<?=$month?>,<?=$day?>), <?=$rake?>],
         	               <?php 
				         }
				       ?>
				       ]
					   },
			       <?php 
			        
			    }
			 ?>		   
            ]	
	      });
          </script>
          
  	 	<?php 
  	 }
  	 $objPHPExcel->getActiveSheet()->setTitle('Reports');
  	 
  	 $objWriter = new PHPExcel_Writer_Excel2007($objPHPExcel);
  	 $objWriter->save("../cache/".$_SESSION["jurisdiction_class"]."_".$date_start."_".$date_end.".xlsx");
  	 	
  }
?>