<?php
$log_totem=check_access ( 'ttm_totem_log');
if($log_totem==true){
if (! isset ( $_GET ['ttcode'] )) {
	addError ( "", $lang->getLang("Totem code don't found. Check the get parameters" ));
	showErrors ();
	die ();
}
$totem_code = $_GET ['ttcode'];
	$log_date_start = date('Y-m-d H:i', mktime(0,0,0,date('m'),date('d')-1,date('Y')));
	$log_date_end =  date('Y-m-d H:i', mktime(23,59,0,date('m'),date('d'),date('Y')));
$row_to_page = 1000;
if (isset ( $_GET ["ttldate_start$totem_code"] )) {
	$log_date_start = $_GET["ttldate_start$totem_code"];	
	$log_date_end = $_GET["ttldate_end$totem_code"];
}
if (isset ( $_GET["resforpage$totem_code"] )) {
	$row_to_page = $_GET["resforpage$totem_code"];
}
$sql = "SELECT ttl_ip, 
				   ttl_note, 
				   ttl_date, 
				   jur_name, 
				   pun_username, 
				   tlt_description 
				   	FROM `totem_log`  
				   	LEFT JOIN totem_log_type on tlt_id = ttl_tlt_id 
				   	LEFT JOIN punter on pun_id = ttl_pun_id 
				   	LEFT JOIN jurisdiction on jur_id = ttl_club_id 
				   	LEFT JOIN totem on ttl_ttm_id = ttm_id  
				   		WHERE ttm_code = " . $dbh->quote ( $totem_code ) . " and ttl_date > date_format(" . $dbh->quote ( $log_date_start ) . ", '%Y%m%d000000') AND ttl_date < date_format(" . $dbh->quote ( $log_date_end ) . ", '%Y%m%d000000')  ";
if(empty($_POST)){
	$sql .= " ORDER BY ttl_date DESC LIMIT $row_to_page ";
}
$res = $dbh->exec ( $sql );
$cols_arr = array ('Date', 'Totem Code', 'Totem IP', 'Note', 'Club Name', 'Username', 'Log Type' );
$val_fmt_arr = array ('Date' => 'return $rec["ttl_date"];', 'Totem Code' => 'return strtoupper($rec["ttm_code"]);', 'Totem IP' => 'return $rec["ttm_ip"];', 'Note' => 'return $rec["ttl_note"];', 'Club Name' => 'return $rec["jur_name"];', 'Log Type' => 'return $rec["tlt_description"];' );
$post_vars = array ('do_search' => $_GET['do_search']);
$cell_fmt_arr = array ();
$start_row = (isset ( $_POST ['start_row'] )) ? $_POST ['start_row'] : 1;

?>
 <script>
    $(function() {
        $( "#datepicker_start<?=$totem_code?>" ).datepicker();   
        $( "#datepicker_end<?=$totem_code?>" ).datepicker();       
    });
    </script>
<hr />
<center>
	<div class="bodyHD"><?=$lang->getLang("Totem")?> <?=strtoupper($totem_code)?> Log:</div>
		<table>
			<tr>
			<form action="<?=$_SERVER['HTTP_REFERER']?>"
		method="get" name="search">
		<input type="hidden" name="do_search" value="1" />
				<td class="label"><?=$lang->getLang("From")?></td>
				<td class="content"><input type="text" id="datepicker_start<?=$totem_code?>" value="<?=$log_date_start?>" name="ttldate_start<?=$totem_code?>" /></td>
			</tr>
		<tr>
			<td class="label"><?=$lang->getLang("Until")?></td>
				<td class="content"><input type="text" id="datepicker_end<?=$totem_code?>" value="<?=$log_date_end?>" name="ttldate_end<?=$totem_code?>" /></td>
			</tr>
			<tr>
				<td>&nbsp;</td>
			</form>
			<td><button id="submit<?=$totem_code?>">Submit
			<script>
						 $(function() {
						$( '#submit<?=$totem_code?>')
					            .button()
					            .click(function() {
					            	$.get("totem/totem_log_individual.inc",{ttcode:'<?=$totem_code?>',ttldate_start<?=$totem_code?>:$('input:text[name=ttldate_start<?=$totem_code?>]').val(),ttldate_end<?=$totem_code?>:$('input:text[name=ttldate_end<?=$totem_code?>]').val()}, function(data){			
					            	$('#submit<?=$totem_code?>').parent().closest('div').empty().append( data );
						            	});
						            });
							return false;
							});
						</script>			
			    </button>
				</td>					
			</tr>
		</table>	
</center>

<?
$qp = new QueryPrinter ( $res );
$excel_file_name = 'totemLog_' . $_SESSION ['admin_username'] . '.xls';
$qp->dumpToExcelFile ( $res, $cols_arr, $val_fmt_arr, $excel_file_name );
$qp->printTable ( $res, $cols_arr, $post_vars, $val_fmt_arr, $cell_fmt_arr, 0, $excel_file_name, $header = "", $width = "100%", $start_row, $row_to_page, false );

}else{?>
<div><?=$lang->getLang("You are not allowed to see this page!")?></div>
<?}?>