<?php
check_access ( 'ttm_totem_log', $redirect = true );

if (! isset ( $_GET ['ttcode'] )) {
    addError ( "", $lang->getLang("Totem code don't found. Check the get parameters" ));
    showErrors ();
    die ();
}

globalise ( 'ttldate' );
setupDateInputJs ( "ttldate" );

$totem_code = $_GET ['ttcode'];
$log_date = date ( 'Y-m-d', mktime ( 0, 0, 0, date ( "m" ), date ( "d" ), date ( "Y" ) ) );
$row_to_page = 10;
if (isset ( $_POST ['ttldate'] )) {
    $log_date = $_POST ['ttldate'];
}
if (isset ( $_POST ['resforpage'] )) {
    $row_to_page = $_POST ['resforpage'];
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
				   		WHERE ttm_code = " . $dbh->quote ( $totem_code ) . " and ttl_date > date_format(" . $dbh->quote ( $log_date ) . ", '%Y%m%d000000') ";

if(empty($_POST)){
    $sql .= " ORDER BY ttl_date DESC LIMIT 10 ";
}

$res = $dbh->exec ( $sql );

$cols_arr = array ('Date', 'Totem Code', 'Totem IP', 'Note', 'Club Name', 'Username', 'Log Type' );

$val_fmt_arr = array ('Date' => 'return $rec["ttl_date"];', 'Totem Code' => 'return strtoupper($rec["ttm_code"]);', 'Totem IP' => 'return $rec["ttm_ip"];', 'Note' => 'return $rec["ttl_note"];', 'Club Name' => 'return $rec["jur_name"];', 'Log Type' => 'return $rec["tlt_description"];' );

$post_vars = array ('ttldate' => $log_date, 'resforpage' => $row_to_page , 'do_search' => $_POST['do_search']);

$cell_fmt_arr = array ();
$start_row = (isset ( $_POST ['start_row'] )) ? $_POST ['start_row'] : 1;
$qp = new QueryPrinter ( $res );
$excel_file_name = 'totemLog_' . $_SESSION ['admin_username'] . '.xls';
$qp->dumpToExcelFile ( $res, $cols_arr, $val_fmt_arr, $excel_file_name );
$qp->printTable ( $res, $cols_arr, $post_vars, $val_fmt_arr, $cell_fmt_arr, 0, $excel_file_name, $header = "", $width = "100%", $start_row, $row_to_page, false );

?>
<hr />
<center>
    <div class="bodyHD">Totem <?=strtoupper($totem_code)?> Log:</div>
    <form action="<?=$_SERVER['PHP_SELF']."?".$_SERVER['QUERY_STRING']?>"
          method="POST" name="search">
        <input type="hidden" name="do_search" value="1" />
        <table>
            <tr>
                <td class="label"><?=$lang->getLang("Registration Date")?></td>
                <td class="content">
                    <?=createDateInput("ttldate", $log_date, false, "search", false)?></td>
            </tr>

            <tr>
                <td class="label"><?=$lang->getLang("Records per page")?></td>
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