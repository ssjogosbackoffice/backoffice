<?php
check_access('ttm_totem_admin', true);

$sql = "SELECT lic_key, lic_times_used, lic_max_use FROM totem_licences WHERE lic_times_used < lic_max_use ORDER BY lic_id";
$rs = $dbh->exec($sql);

$cols_arr    = array("Code", "Uses");
$val_fmt_arr = array(
'Code'  => 'return $rec["lic_key"];',
'Uses'  => 'return (($rec["lic_max_use"] > 1)?($rec["lic_times_used"] . "/" . $rec["lic_max_use"]):("Single"));'
);

$numrow = $rs->getNumRows();
if($numrow > 0){
  $qp = new QueryPrinter($rs);
  $qp->printTable($rs, $cols_arr, "", $val_fmt_arr, null, 0, 0, "", $width="100%", 1, $numrow, false);
}else{
  addError("", $lang->getLang("No skins found"));
  showErrors();
}

?>