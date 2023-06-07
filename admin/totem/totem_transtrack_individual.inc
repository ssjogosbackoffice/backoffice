<?php
$trans_totem=check_access ( 'ttm_totem_trns',true);
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
						alert("<?=$lang->getLang("The request was successfully resolved.The totem is now available")?>");
				  }else{
					  	alert("<?=$lang->getLang("The request was accepted, but the totem can't be reached")?>");
				  }			     				 
			  },
			  error: function(){
				  alert("<?=$lang->getLang("Communication error")?>");
			  }
			});
 }

//-->
</script>
<?php

$chooseFor = "club";

function getAmount($amount,$symbol=false) {
	if ($amount > 0) {
		return '<FONT COLOR="#000000">' . getInDollars ( $amount,2,$symbol ) . '</FONT>';
	} else {
		return '<FONT COLOR="#FF0000">' . getInDollars ( $amount ,2,$symbol) . '</FONT>';
	}
}

function getStatus($status, $tttID, $tCode){
    global $lang;
	if($status == "C"){
		return "<FONT COLOR='green'>".$lang->getLang("Completed")."</FONT>";
	}else if($status == "P"){
		return "<FONT COLOR='orange'>".$lang->getLang("Pending")."</FONT><button type='button' onClick='acceptPending(\"$tttID\", \"$tCode\");refreshTotem();' name='pAccept' value='Accept'>".$lang->getLang('Accept')."</button>";
	}
}

function getExcelStatu($status, $tttID, $tCode){
	if($status == "C"){
		return "Completed";
	}else if($status == "P"){
		return "Pending";
	}
}

if (isset ( $_GET ['ttcode'] )) {
	$totemCode = $_GET ['ttcode'];
	$date_start = date('Y-m-d H:i:s', mktime(0,0,0,date('m'),date('d')-1,date('Y')));
	$date_end =  date('Y-m-d H:i:s', mktime(23,59,59,date('m'),date('d'),date('Y')));
} else if (isset ( $_POST ['ttcode'] )) {
	$totemCode = $_POST ['ttcode'];
	$date_start = $_POST["date_start$totemCode"];
	$date_end = $_POST["date_end$totemCode"];
} else {
	$totemCode = '';
	$date_start = date('Y-m-d H:i:s', mktime(0,0,0,date('m'),date('d')-1,date('Y')));
	$date_end =  date('Y-m-d H:i:s', mktime(23,59,59,date('m'),date('d'),date('Y')));
}



?>

<script type="text/javascript">
  $(function() {
        $( "#date_start<?=$totemCode?>" ).datepicker({            
            changeMonth: true,
            numberOfMonths:1,
            onSelect: function( selectedDate ) {
                $( "#date_end<?=$totemCode?>" ).datepicker( "option", "minDate", selectedDate );
                $( "#date_end<?=$totemCode?>" ).datepicker( "option", "maxDate", "+2M");
            }
        });
        $( "#date_end<?=$totemCode?>" ).datepicker({
            changeMonth: true,
            numberOfMonths:1,
            onSelect: function( selectedDate ) {
                $( "#date_start<?=$totemCode?>" ).datepicker( "option", "maxDate", selectedDate );
            }
        });
    });
    </script>
<center>
<table>	
	<p class="bodyHD"><?=$lang->getLang("Totem Transaction")?>:</p>
	<form action="<?=$_SERVER['HTTP_REFERER']?>"
		method="POST" name="search">
		<input type="hidden" name="do_search" value="1" />
			 
		    <tr>
				<td class="label"><?=$lang->getLang("From")?>:</td>
				<td class="content"><input type="text" id="date_start<?=$totemCode?>" name="date_start<?=$totemCode?>"  value="<?=date('Y-m-d',strtotime($date_start))?>" /></td>
			</tr>
		    <tr>
				<td class="label"><?=$lang->getLang("Until")?>:</td>
				<td class="content">
 				<input type="text" id="date_end<?=$totemCode?>" name="date_end<?=$totemCode?>"  value="<?=date('Y-m-d',strtotime($date_end))?>" /></td>
			</tr>	    
	
			<tr>
				<td>&nbsp;</td>
				</form>
				<td><button id="trans<?=$totemCode?>"><?=$lang->getLang("Submit")?>
			<script>
						 $(function() {
						$( '#trans<?=$totemCode?>')
					            .button()
					            .click(function() {
					            	$.post("totem/totem_transtrack_individual.inc",{do_search:'1',ttcode:'<?=$totemCode?>','date_start<?=$totemCode?>':$('input:text[name=date_start<?=$totemCode?>]').val(),'date_end<?=$totemCode?>':$('input:text[name=date_end<?=$totemCode?>]').val()}, function(data){
					            	$('#trans<?=$totemCode?>').parent().closest('div').empty().append( data );
						            	});
						            });
							return false;
							});
						</script>
			</button></td>
			</tr>
		</table>
</center>
<hr/>
<script>
function refreshTotem()
{
	$.post("totem/totem_transtrack_individual.inc",{do_search:'1',ttcode:'<?=$totemCode?>','date_start<?=$totemCode?>':'<?=$date_start?>','date_end<?=$totemCode?>':'<?=$date_end?>'}, function(data){
    	$('#date_start<?=$totemCode?>').parent().closest('div').empty().append( data );
	});
}
</script>
<?
if (isset ( $_POST ['do_search'] )) {
	$totemCode = $_POST['ttcode'];
	$date_start = date("Y-m-d 00:00:00", strtotime($_POST["date_start$totemCode"]));
	$date_end = $new_time = date("Y-m-d 23:59:59", strtotime($_POST["date_end$totemCode"]));
	$row_to_page = '1000';
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
							   ttm_code,
							   ttt_tax,
							   ttt_tax_perc,
							   cur_code_for_web
								 FROM totem_transtrack 
								  LEFT JOIN totem on ttm_id = ttt_ttm_id
								  LEFT JOIN punter on ttt_pun_id = pun_id
								  JOIN jurisdiction on jur_id=pun_betting_club
								  JOIN currency on cur_id=jur_currency
								  WHERE UPPER(ttm_code) = UPPER(" . $dbh->quote ( $totemCode ) . ") 
								 AND ttt_time BETWEEN date_format(".$dbh->quote($date_start).",'%Y-%m-%d %H:%i:%s')
			 					 AND  date_format(" .$dbh->quote($date_end) . " ,'%Y-%m-%d %H:%i:%s') order by ttt_id ASC";

						$res = $dbh->exec( $sql );
						$total=0;
						$totaldeposit=0;
						$totalwithdraw=0;
						$totalTax=0;
						$cols_arr = array ('Totem Code', 'Transaction ID', 'Date', 'Username', 'Transfer ID', 'Amount', 'Note', 'Status','Tax(%)' );
						$val_fmt_arr = array ('Totem Code' => 'return strtoupper($rec["ttm_code"]);', 'Transaction ID' => 'return $rec["ttt_id"];', 'Date' => 'return $rec["ttt_time"];', 'Username' => 'return getClassLink(refPage("customers/customer_view&customer_id=" . $rec["pun_id"]), $rec["pun_username"]);', 'Transfer ID' => 'return $rec["ttt_transferid"];', 'Amount' => 'return getAmount($rec["ttt_amount"],$rec["cur_code_for_web"]);', 'Note' => 'return $rec["ttt_note"];', 'Status' => 'return getStatus($rec["ttt_status"],$rec["ttt_id"],$rec["ttm_code"]);','Tax(%)'=>'return $rec["ttt_tax"]." (".$rec["ttt_tax_perc"]."%)";');
						$cell_fmt_arr = array ();
						 $post_vars = array('ttcode' => $totemCode,
						 	'date_start' => $date_start,
						 	'date_end' => $date_end,						 	
						 	'do_search' => '1'
						 );
						// 'ttmactive' => $ttmActiveStatus,
						// 'ttmaccess' => $ttmAccessStatus,
						// 'jurisdiction' => $ttmJurisdiction,
						// 'do_search' => '1',
						// 'resforpagetrans' => $row_to_page
						
						// );
						
						$start_row = (isset ( $_POST ['start_row'] )) ? $_POST ['start_row'] : 1;
						$qp = new QueryPrinter ( $res );
						while($res->hasNext()){
							$row=$res->next();
							if($row['ttt_amount']<=0)
							{
								$totalwithdraw=$totalwithdraw+$row['ttt_amount'];
							}
							else
							{
								$totaldeposit=$totaldeposit+$row['ttt_amount'];
							}
							$total=$total+$row['ttt_amount'];
							$totalTax+=$row['ttt_tax'];
						}
						
						$excel_file_name = 'totemTransaction_' . $totemCode . '.xls';
						$qp->dumpToExcelFile ( $res, $cols_arr, $val_fmt_arr, $excel_file_name );
						
						?>						
						<table style="width:100%" bgcolor="#a99a9" cellpadding="4" cellspacing="1" border="0"><tr><td class="formheading" colspan="8"><?=$lang->getLang("Stats for  period:")?> <?=$date_start." - ".$date_end?></td></tr>
						 		<tr><td class="label"><?=$lang->getLang("Total Deposit")?></td><td class="content" style="text-align:right"><?=getInDollars($totaldeposit)?></td><td class="label"><?=$lang->getLang("Total Withdraw")?></td> <td class="content" style="text-align:right"><?=getInDollars($totalwithdraw)?></td><td class="label" >Totals:</td> <td class="content" style="text-align:right"><?=getInDollars($total)?></td><td class="label" >Tax:</td> <td class="content" style="text-align:right"><?=getInDollars($totalTax)?></td></tr>
						</table>
						<hr />
						<?php 
						
						$qp->printTable ( $res, $cols_arr, $post_vars, $val_fmt_arr, $cell_fmt_arr, 0, $excel_file_name, $header = "", $width = "100%", $start_row, $row_to_page, false );
				
			}

?>
</div>
<?=showErrors();?>