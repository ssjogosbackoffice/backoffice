<script type="text/javascript">
    <!--
    function openModifyTotem(totem_code){
        location.href = "<?=$_SERVER['PHP_SELF']?>"+"?page=totem/totem_modify&header_page=&ttcode="+totem_code;
    }
    function openConnectTotem(totem_code){
        location.href = "<?=$_SERVER['PHP_SELF']?>"+"?page=totem/totem_connection&header_page=&ttcode="+totem_code;
    }
    function openLogTotem(totem_code){
        location.href = "<?=$_SERVER['PHP_SELF']?>"+"?page=totem/totem_log&header_page=&ttcode="+totem_code;
    }
    function openTransactionTotem(totem_code){
        location.href = "<?=$_SERVER['PHP_SELF']?>"+"?page=totem/totem_transtrack&header_page=&ttcode="+totem_code;
    }
    function openConfigurationTotem(totem_code){
        location.href = "<?=$_SERVER['PHP_SELF']?>"+"?page=totem/totem_configure&header_page=&ttcode="+totem_code;
    }
    //-->
</script>
<?php
check_access ( 'totem_configure', $redirect = true );
include_once 'JurisdictionsList.class.inc';
global $lang;

globalise ( 'ttmregdate' );
setupDateInputJs ( "ttmregdate" );
/**
 * Settings variable
 *
 * @author Manuel, Marian
 */
define ( 'ROWS_TO_PAGES', 10 );
$totemCode = '*';
$registration_date = '*';
$ttmregsymb = "=";
$ttmcodesymb = "=";
$ttmactiveon = 1;
$ttmactiveoff = 1;
$ttmaccesson = 1;
$ttmaccessoff = 1;
$ttmJurisdiction = '*';
$row_to_page = ROWS_TO_PAGES;

function showActivation($activation) {
    global $lang;
    if ($activation == 1) {
        return '<FONT COLOR="red">'.$lang->getLang("Not Active").'</FONT>';
    } else {
        return '<FONT COLOR="green">'.$lang->getLang("Active").'</FONT>';
    }
}

function showAccess($access) {
    if (trim(strtolower($access)) == "deny") {
        return '<img src="' . image_dir . '/icon_lock_close.gif" />';
    } else {
        return '<img src="' . image_dir . '/icon_lock_open.gif" />';
    }
}

function getTotemAction($TotemCode) {

    global $dbh,$lang;
    $modify_totem = check_access ( 'totem_configure' );
    $connect_to_totem = check_access ( 'ttm_totem_admin' );
    $transack_totem = check_access('ttm_totem_daily_transtrack');
    $htmlAction = "";
    $htmlAction = '<img width="20" height="20" src="' . image_dir . '/icon_log.gif" onclick="openLogTotem(' . $dbh->quote ( $TotemCode ) . ');" />';
    $htmlAction .= ($transack_totem) ? '<img width="20" height="20" src="' . image_dir . '/icon_financial.gif" onclick="openTransactionTotem(' . $dbh->quote ( $TotemCode ) . ');" />' : '';
    $htmlAction .= ($modify_totem) ? '<img width="20" height="20" src="' . image_dir . '/icon_edit_on.gif" onclick="openModifyTotem(' . $dbh->quote ( $TotemCode ) . ');" />' : '';
    $htmlAction .= ($connect_to_totem) ? '<img width="20" height="20" src="' . image_dir . '/icon_server_connect.gif" onclick="openConnectTotem(' . $dbh->quote ( $TotemCode ) . ');" />' : '';

    if ($htmlAction == "") {
        $htmlAction = $lang->getLang("You don't have privileges for any operation");
    }
    return $htmlAction;
}


if (isset ( $_POST ['do_search'] )) {
    $totemCode = $_POST ['ttmcode'];
    $registration_date = $_POST ['ttmregdate'];
    $ttmregsymb = $_POST ['ttmregsymb'];
    $ttmcodesymb = $_POST ['ttmcodesymb'];
    $ttmActiveStatus = $_POST ['ttmactive'];
    $ttmAccessStatus = $_POST ['ttmaccess'];
    $ttmJurisdiction = $_POST ['jurisdiction'];

    $ttmactiveon = 0;
    $ttmactiveoff = 0;
    for($countActive = 0; $countActive < count ( $ttmActiveStatus ); $countActive ++) {
        if ($ttmActiveStatus [$countActive] == 1) {
            $ttmactiveon = 1;
        }
        if ($ttmActiveStatus [$countActive] == 0) {
            $ttmactiveoff = 1;
        }
    }
    $ttmaccesson = 0;
    $ttmaccessoff = 0;
    for($countAccess = 0; $countAccess < count ( $ttmAccessStatus ); $countAccess ++) {
        if ($ttmAccessStatus [$countAccess] == "allow") {
            $ttmaccesson = 1;
        }
        if ($ttmAccessStatus [$countAccess] == "deny") {
            $ttmaccessoff = 1;
        }
    }

    $row_to_page = $_POST ['resforpage'];

    $sql_totem_list = "SELECT ttm_code,
			                          ttm_ip,
									  ttm_totem_md5,
									  ttm_access,
									  ttm_reg_date,
									  ttm_last_login,
									  ttm_club_id ,
									  ttm_act_status ,
									  ttm_note,
									  club.jur_name jur_name
									   FROM `totem` 
			  							LEFT JOIN jurisdiction  club on club.jur_id = ttm_club_id ";


    if($_SESSION["jurisdiction_class"] != "casino"){
        $sql_totem_list .= " JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
                JOIN jurisdiction region ON region.jur_id = district.jur_parent_id
                JOIN jurisdiction nation   ON nation.jur_id   = region.jur_parent_id";
    }
    switch($_SESSION["jurisdiction_class"]){
        case "club":
            $sql_totem_list .= " WHERE club.jur_id = ".$_SESSION['jurisdiction_id'];
            break;
        case "district":
            $sql_totem_list .= " WHERE district.jur_id = ".$_SESSION['jurisdiction_id'];
            break;
        case "region":
            $sql_totem_list .= " WHERE region.jur_id = ".$_SESSION['jurisdiction_id'];
            break;
        case "nation":
            $sql_totem_list .= " WHERE nation.jur_id = ".$_SESSION['jurisdiction_id'];
            break;

        case "casino":
            $sql_totem_list .= " WHERE 1=1";
            break;
    }

    if ($totemCode != '' && $totemCode != "*") {
        switch ($ttmcodesymb) {
            case "=" :
                $sql_totem_list .= " AND ttm_code = " . $dbh->quote ( $_POST ['ttmcode'] ) . " ";
                break;
            case "%LIKE" :
                $sql_totem_list .= " AND ttm_code LIKE " . $dbh->quote ( "%" . $_POST ['ttmcode'] ) . " ";
                break;
            case "LIKE%" :
                $sql_totem_list .= " AND ttm_code LIKE " . $dbh->quote ( $_POST ['ttmcode'] . "%" ) . " ";
                break;
            case "%LIKE%" :
                $sql_totem_list .= " AND ttm_code LIKE " . $dbh->quote ( "%" . $_POST ['ttmcode'] . "%" ) . " ";
                break;
        }

    }
    if ($registration_date != '' && $registration_date != '*') {
        $sql_totem_list .= " AND date_format(ttm_reg_date,'%Y%m%d') " . $ttmregsymb . " date_format(" . $dbh->quote ( $registration_date ) . ",'%Y%m%d') ";
    }

    if ($ttmactiveon == 1 && $ttmactiveoff == 1) {
        $sql_totem_list .= " AND ttm_act_status in (1,2) ";
    } else if ($ttmactiveon == 0 && $ttmactiveoff == 0) {
        $sql_totem_list .= " AND ttm_act_status = 0 ";
    } else if ($ttmactiveon == 1) {
        $sql_totem_list .= " AND ttm_act_status = 2 ";
    } else if ($ttmactiveoff == 1) {
        $sql_totem_list .= " AND ttm_act_status = 1 ";
    }

    if ($ttmaccesson == 1 && $ttmaccessoff == 1) {
        $sql_totem_list .= " AND ttm_access in ('allow','deny') ";
    } else if ($ttmaccesson == 0 && $ttmaccessoff == 0) {
        $sql_totem_list .= " AND ttm_access = NULL ";
    } else if ($ttmaccesson == 1) {
        $sql_totem_list .= " AND ttm_access = 'allow' ";
    } else if ($ttmaccessoff == 1) {
        $sql_totem_list .= " AND ttm_access = 'deny' ";
    }

    if ($ttmJurisdiction != '' && $ttmJurisdiction != '*') {
        $sql_totem_list .= " AND ttm_club_id = $ttmJurisdiction";
    }
    $res_totem = $dbh->doCachedQuery ( $sql_totem_list, 0 );
    $count = 0;
    $totem = array ();

    $cols_arr = array ($lang->getLang('Totem Code'), $lang->getLang('Totem IP'), $lang->getLang('Activation Status'), $lang->getLang('Hardware key'),$lang->getLang('Access'),$lang->getLang('Registration Date') , $lang->getLang('Last Login'),$lang->getLang('Club Name'), $lang->getLang('Totem Note'), $lang->getLang('Action') );

    $val_fmt_arr = array ($lang->getLang('Totem Code') => 'return strtoupper($rec["ttm_code"]);', $lang->getLang('Totem IP') => 'return $rec["ttm_ip"];', $lang->getLang('Activation Status') => 'return showActivation($rec["ttm_act_status"]);', $lang->getLang('Hardware key') => 'return $rec["ttm_totem_md5"];', $lang->getLang('Access') => 'return showAccess($rec["ttm_access"]);', $lang->getLang('Registration Date') => 'return $rec["ttm_reg_date"];', $lang->getLang('Last Login') => 'return $rec["ttm_last_login"];', $lang->getLang('Club Name') => 'return $rec["jur_name"];', $lang->getLang('Totem Note') => 'return $rec["ttm_note"];', $lang->getLang('Action') => 'return getTotemAction($rec["ttm_code"]);' );

    $cell_fmt_arr = array ();

    $post_vars = array('ttmcode' => $totemCode,
        'ttmregdate' => $registration_date,
        'ttmregsymb' => $ttmregsymb,
        'ttmcodesymb' => $ttmcodesymb,
        'ttmactive' => $ttmActiveStatus,
        'ttmaccess' => $ttmAccessStatus,
        'jurisdiction' => $ttmJurisdiction,
        'do_search' => '1',
        'resforpage' => $row_to_page

    );

    $start_row = (isset ( $_POST ['start_row'] )) ? $_POST ['start_row'] : 1;

    $qp = new QueryPrinter ( $res_totem );
    $excel_file_name = 'totemList_' . $_SESSION ['admin_username'] . '.xls';
    $qp->dumpToExcelFile ( $res_totem, $cols_arr, $val_fmt_arr, $excel_file_name );
    $qp->printTable ( $res_totem, $cols_arr, $post_vars, $val_fmt_arr, $cell_fmt_arr, 0, $excel_file_name, $header = "", $width = "100%", $start_row, $row_to_page, false );

}
$jurClass = JurisdictionsList::getInstance ( $dbh );
$clubList = $jurClass->getChilJurisdictionsDataList ( $_SESSION ['jurisdiction_id'], 'club' );
?>
<style>
    <!--
    #legend {
        padding-top: 2px;
        width: 300px;
        background: #EEEEEE;
        border: 1px solid #FF0000;
        margin-left: 5px;
        display: none;
    }

    .titleLegend {
        font-size: 14;
        color: #220022;
    }
    -->
</style>
<hr />
<center>
    <div class="bodyHD"><?=$lang->getLang("Totem Search")?>:</div>
    <form action="<?=$_SERVER['PHP_SELF']."?".$_SERVER['QUERY_STRING']?>"
          method="POST" name="search">
        <input type="hidden" name="do_search" value="1" />
        <table>
            <tr>
                <td class="label"><?=$lang->getLang("Totem Code")?></td>
                <td class="content"><input type="text" name="ttmcode" size="6"
                                           maxlength="6" value="<?=$totemCode?>" /> <input
                        <?php if($ttmcodesymb == "="){?> checked <?php }?> type="radio"
                                                                           name="ttmcodesymb" value="=" /> = <input
                        <?php if($ttmcodesymb == "%LIKE"){?> checked <?php }?> type="radio"
                                                                               name="ttmcodesymb" value="%LIKE" /> *%s <input
                        <?php if($ttmcodesymb == "LIKE%"){?> checked <?php }?> type="radio"
                                                                               name="ttmcodesymb" value="LIKE%" /> %s* <input
                        <?php if($ttmcodesymb == "%LIKE%"){?> checked <?php }?>
                        type="radio" name="ttmcodesymb" value="%LIKE%" /> *%s*</td>
            </tr>
            <tr>
                <td class="label"><?=$lang->getLang("Registration Date")?></td>
                <td class="content">
                    <?=createDateInput("ttmregdate", $registration_date, false, "search", false)?>
                    <input <?php if($ttmregsymb == ">"){?> checked <?php }?> type="radio"
                                                                             name="ttmregsymb" value=">" /> > <input
                        <?php if($ttmregsymb == "<"){?> checked <?php }?> type="radio"
                                                                          name="ttmregsymb" value="<" /> <
                    <input
                        <?php if($ttmregsymb == "="){?> checked <?php }?> type="radio"
                                                                          name="ttmregsymb" value="=" /> =
                </td>
            </tr>
            <tr>
                <td class="label"><?=$lang->getLang("Records per page")?></td>
                <td class="content"><select name="resforpage">
                        <option <?php if($row_to_page == 2){?> selected <?php }?>
                            value="2">2</option>
                        <option <?php if($row_to_page == 5){?> selected <?php }?>
                            value="5">5</option>
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
                <td class="label"><?=$lang->getLang('Activation status')?></td>
                <td class="content"><input <?php if($ttmactiveon == 1){ ?> checked
                    <?php }?> type="checkbox" name="ttmactive[]" value="1" /> <?=$lang->getLang('Active')?><input
                        <?php if($ttmactiveoff == 1){ ?> checked <?php }?> type="checkbox"
                                                                           name="ttmactive[]" value="0" /> <?=$lang->getLang('Not Active')?></td>
            </tr>
            <tr>
                <td class="label"><?=$lang->getlang("Access")?></td>
                <td class="content"><input <?php if($ttmaccesson == 1){ ?> checked
                    <?php }?> type="checkbox" name="ttmaccess[]" value="allow" /> <?=$lang->getlang("Allow")?>
                    <input <?php if($ttmaccessoff == 1){ ?> checked <?php }?>
                        type="checkbox" name="ttmaccess[]" value="deny" /> <?=$lang->getlang("Deny")?></td>
            </tr>
            <tr>


            <tr>
                <td class="label"><?=$lang->getLang('Club')?></td>
                <td class="content"><select name="jurisdiction">
                        <?php  if($_SESSION['jurisdiction_class'] != 'club'){?>
                            <option <?php if($ttmJurisdiction == '*'){ ?> selected <?php } ?>
                                value="*"><?=$lang->getLang("All clubs")?></option>
                        <?php }

                        for($y = 0; $y < count ( $clubList ); $y ++) {
                            ?> <option <?php if($ttmJurisdiction ==  $clubList[$y][0]) {?>
                                selected <?php }?> value="<?=$clubList[$y][0]?>"><?=$clubList[$y][1]?></option> <?php
                        }
                        ?>
                    </select></td>
            </tr>
            <td colspan="2">
                <div id="legend">

                    <h3><?=$lang->getLang("Legend")?></h3>
                    <span class="titleLegend">%s - </span><?=$lang->getLang("indicates the contents of the text box")?> <br /> <span class="titleLegend"> * - </span><?=$lang->getlang("enter an asterisk in the field if you want to exclude from the search")?>
                </div>
            </td>
            </tr>
            <tr>
                <td>&nbsp;</td>
                <td><input type="submit" value="<?=$lang->getLang("Submit")?>" /> <input type="button"
                                                                                         value="<?=$lang->getLang("Legend")?>" onclick="toogleLegend();" /></td>
            </tr>
        </table>
        <script type="text/javascript">
            function toogleLegend(){
                if($('#legend').css("display") == "block"){
                    $('#legend').hide('slow');
                }else{
                    $('#legend').show('slow');
                }
            }
        </script>
    </form>
</center>