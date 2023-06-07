<script type='text/javascript'>
    function openModifyCroupier(croupier_id){
        location.href = "<?=$_SERVER['PHP_SELF']?>"+"?page=live/croupier_edit&header_page=&cid="+croupier_id;
    }
    function openLogin(croupier_id){
        location.href = "<?=$_SERVER['PHP_SELF']?>"+"?page=live/croupier_login&header_page=&cid="+croupier_id;
    }
    function openNumber(croupier_id){
        location.href = "<?=$_SERVER['PHP_SELF']?>"+"?page=live/croupier_number&header_page=&cid="+croupier_id;
    }
</script>
<?php
check_access('live_manage_croupier', $redirect=true);
include_once("funclive.inc.php");

/*
    Funzione di controllo sugli accessi alle operazioni di gestione della croupier
*/
function createCroupierAction($crpid){
    global $lang;
    $croupier_login  = check_access('live_croupier_login');
    $croupier_edit   = check_access('live_croupier_edit');
    $croupier_number = check_access('live_croupier_number');
    $htmlAction = "";
    $htmlAction .= ($croupier_edit)   ? "<img width='20' height='20' src='live/liveimage/edit.png' onclick='openModifyCroupier($crpid);' />"   : "";
    $htmlAction .= ($croupier_login)  ? "<img width='20' height='20' src='live/liveimage/key.png' onclick='openLogin($crpid);' />"    : "";
    $htmlAction .= ($croupier_number) ? "<img width='20' height='20' src='live/liveimage/casino.png' onclick='openNumber($crpid);' />" : "";

    if($htmlAction == ""){
        $htmlAction = $lang->getLang("You don't have privileges for any operation");
    }
    return $htmlAction;
}

/*
    Funzione di controllo sull'abilitazione della croupier al gioco
*/


function checkFill($name,$lastname,$zodiac,$photo){
    global $lang;
    $missing="<div class='errorbold'>".$lang->getLang("Missing").":</div>";
    $error='';
    if($name==''){
        $error.="<div class='error'> - ".$lang->getLang("Name")."</div>";
    }
    if($lastname==''){
        $error.= "<div class='error'> -".$lang->getLang("Last Name")."</div>";
    }
    if($zodiac==''){
        $error.= "<div class='error'> - ".$lang->getLang("Zodiac")."</div>";
    }
    $photo=substr($photo,4);
    if(!file_exists(PHOTOS.$photo.".jpeg")&&!file_exists(PHOTOS.$photo.".png")){
        $error.= "<div class='error'> - ".$lang->getLang("Photo")."</div>";
    }
    if($error==''){
        $error='<p class="result">'.$lang->getLang("All details are filled!").'</p>';
    }
    else{
        $error=$missing.$error;
    }
    return $error;
}

function checkCroupierPermission($access){
    if($access == "allow"){
        return "<img width='20' height='20' src='live/liveimage/allow.png' />";
    }else if($access == "deny"){
        return "<img width='20' height='20' src='live/liveimage/deny.png' />";
    }
}

$sql = "Select * from croupier";
$rs = $dbh->exec($sql);

$cols_arr    = array('Croupier Code','Name', 'Username' ,'Croupier Type','Access','Action','Status');

$val_fmt_arr = array (
    'Croupier Code' => 'return substr($rec["crp_code"] ,4);',
    'Name'          => 'return $rec["crp_last_name"]. " " . $rec["crp_first_name"];',
    'Username'      => 'return $rec["crp_username"];',
    'Croupier Type' => 'return checkTypeCroupier($rec["crp_type"]);',
    'Access'        => 'return checkCroupierPermission($rec["crp_access"]);',
    'Action'        => 'return createCroupierAction($rec["crp_id"]);',
    'Status'        => 'return checkFill($rec["crp_first_name"],$rec["crp_last_name"],$rec["crp_zodiac"],$rec["crp_code"]);'
);

$cell_fmt_arr = array ();

$numrow = $rs->getNumRows();
if($numrow > 0){
    $qp = new QueryPrinter($rs);
    $qp->printTable($rs, $cols_arr, "", $val_fmt_arr, $cell_fmt_arr, 0, 0, "", $width="100%", 1, $numrow, false);
}else{
    addError("", "No croupier found :)");
    showErrors();
}

?>