<?php
check_access ( 'live_recovery', $redirect = true );
require_once 'funclive.inc.php';

function getActions($recoveryID, $status){
    if($status == 0){
        return "<a href='javascript:processRecovery($recoveryID)' >Processa</a>";
    }else{
        return "";
    }
}
function getIcon($status){
    if($status == 0){
        return "<img src='live/liveimage/open.png' width='16' height='16' />";
    }else{
        return "<img src='live/liveimage/close.png' width='16' height='16' />";
    }
}
?>
<script type="text/javascript">
    function processRecovery(recoveryID){
        location.href = "<?=$_SERVER['PHP_SELF']?>"+"?page=live/process&rid="+recoveryID+"&header_page=";
    }
</script>
<div class="bodyHD"><?=$lang->getLang('Recovery Hands')?></div>
<table border="0" cellspacing="0" cellpadding="0">
    <tr valing="top">
        <td>
            <?php
            $sql = "SELECT res_string,
					   rte_id, 
					   rte_res_id, 
					   rte_recovery_result, 
					   crp_first_name, 
					   crp_last_name, 
					   rte_note, 
					   rte_status, 
					   rte_close_date,
					   res_date 
					   	FROM recovery_result 
					   	JOIN result on res_id = rte_res_id 
					   	JOIN croupier on crp_id = rte_crp_id";
            $rs = $dbh->exec ( $sql );
            $numrow = $rs->getNumRows();
            if ($numrow > 0) {
                $cols_arr    = array('Hands ID', 'Result', 'Recovery Result', 'Croupier' ,'Status','Note','Chiusa', 'Azioni');

                $val_fmt_arr = array (
                    'Hands ID'        => 'return getClassLink("' . refPage('casino_games/searchuserhand') . '&handid=" . $rec["rte_res_id"] . "&from=" . $rec["res_date"] . "&to=" . date("Y-m-d H:i", (strtotime($rec["res_date"]) + 60 * 60 * 3)), "View Hands", "", "_blank");',
                    'Result'          => 'return printExtractNumber($rec["res_string"]);',
                    'Recovery Result' => 'return printExtractNumber($rec["rte_recovery_result"]);',
                    'Croupier'        => 'return $rec["crp_last_name"] . " " . $rec["crp_first_name"];',
                    'Status'          => 'return getIcon($rec["rte_status"]);',
                    'Note'            => 'return $rec["rte_note"];',
                    'Chiusa'          => 'return $rec["rte_close_date"];',
                    'Azioni'          => 'return getActions($rec["rte_id"],$rec["rte_status"]);'
                );

                $cell_fmt_arr = array ();

                $qp = new QueryPrinter($rs);
                $qp->printTable($rs, $cols_arr, "", $val_fmt_arr, $cell_fmt_arr, 0, 0, "", $width="100%", 1, $numrow, false);
            } else {
                addError ( "", $lang->getLang("No result found"));
                showErrors ();
            }
            ?>
        </td>
    </tr>
</table>
