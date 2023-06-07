<?php check_access('casino_view_totem_ticket', true);?>
    <script src="<?= secure_host ?>/media/ui/js/jquery.timepicker.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/jquery.multiselect.js" type="text/javascript"></script>

    <style type="text/css"></style>
    <script type="text/javascript">
        $(function() {
            $("select").multiselect({selectedList: 4}).multiselectfilter();
            var startDateTextBox = $('#date_start');
            var endDateTextBox = $('#date_end');

            startDateTextBox.datetimepicker({
                changeYear:true,
                onClose: function(dateText, inst) {
                    if (endDateTextBox.val() != '') {
                        var testStartDate = startDateTextBox.datetimepicker('getDate');
                        var testEndDate = endDateTextBox.datetimepicker('getDate');
                        endDateTextBox.datetimepicker('setDate', testStartDate);
                    }
                    else {
                        endDateTextBox.val(dateText);
                    }
                },
                onSelect: function (selectedDateTime){
                    endDateTextBox.datetimepicker('option', 'minDate', startDateTextBox.datetimepicker('getDate') );
                    endDateTextBox.datetimepicker('option', 'maxDate', startDateTextBox.datetimepicker('getDate') );
                }
            });
            endDateTextBox.datetimepicker({
                onClose: function(dateText, inst) {
                    if (startDateTextBox.val() != '') {
                        var testStartDate = startDateTextBox.datetimepicker('getDate');
                        var testEndDate = endDateTextBox.datetimepicker('getDate');
                        if (testStartDate > testEndDate)
                            startDateTextBox.datetimepicker('setDate', testEndDate);
                    }
                    else {
                        startDateTextBox.val(dateText);
                    }
                }
            });
        });
    </script>
    <link rel="stylesheet" type="text/css" href="/media/jquery.multiselect.css" title="core_style">
<?php form_head()?>
<?php printHiddenInput('doQuery', 'yes');?>
<?php printHiddenInput('search_type', 'date_range');?>
<?php
global $dbh;
$doQuery = $_POST['doQuery'];
$row_to_page = (isset($_POST['resforpage'])) ? $_POST['resforpage'] : 20;
$date_start = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d 00:00", time());
$date_end = (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d 23:59", time());
$tid = $_POST['alltotems'] ;
$tid=implode("','",$tid);
$sql="select jur_name, ttm_code, ttm_id
        from jurisdiction, totem
        where jur_id = ttm_club_id ";
$result=$dbh->doCachedQuery($sql,0);

$clubs=array();
while($result->hasNext()){
    $row=$result->next();
    if(!is_array($clubs[$row['jur_name']])){
        $clubs[$row['jur_name']]=array();
    }
    array_push($clubs[$row['jur_name']],$row['ttm_code']);
}
?>
    <div class="centered bodyHD"><?=$lang->getLang("Search totem games")?></div><br>
    <table align="center" cellpadding="4" cellspacing="1" border="0">
        <tr valign=middle>
            <td class=label><?=$lang->getLang("From")?></td>
            <td class=content>
                <input type='text' name='date_start' id='date_start'  value="<?=$date_start?>" >
            </td>
        </tr>
        <tr valign=middle>
            <td class=label><?=$lang->getLang("Until")?></td>
            <td class=content>
                <input type='text' name='date_end' id='date_end'  value="<?=$date_end?>" >
            </td>
        </tr>

        <!-- <tr>
            <td class="label">Totem Code</td>
            <td class="content"> <input type='text' name='tid' id='tid'  value="<?/*=$tid*/?>" ></td>
        </tr>-->
        <tr>
            <td class='label'><?=$lang->getLang("Choose totem")?></td>
            <td>
                <select name='alltotems[]' multiple='multiple'>
                    <?
                    foreach($clubs as $k=>$v){
                        ?>
                        <optgroup label="<?=$k?>">
                            <? foreach ($v as $k2=>$v2){
                                ?>
                                <option value='<?=$v2?>' <?$pos = strpos($tid, $v2);if($pos===false){}else{ ?>selected="selected" <?}?>  ><?=$v2?></option>
                            <?
                            }
                            ?>
                        </optgroup>
                    <?
                    }
                    ?>
                </select>
            </td>
        </tr>
        <tr>
            <td class='label'><?=$lang->getLang("Show details")?></td>
            <td class=content><input type="checkbox" name='details' <?if(isset($_POST['details'])){?> checked="checked" <?}?>/><?=$lang->getLang("Show details")?></td>
        </tr>
        <tr>
            <td class=content></td>
            <td class=content><input type="submit" value="<?=$lang->getLang("View")?>"/></td>
        </tr>
    </table>

<?php
if($doQuery){
    if(isset($_POST['details'])){
        if($tid!=''){
            $sql2=" select jur_name,pun_username,pun_id,ttm_code from  totem,punter,jurisdiction
                WHERE pun_betting_club=jur_id
                AND ttm_club_id=pun_betting_club
                AND ttm_pun_id=pun_id
              and ttm_code IN ('$tid')  ORDER BY ttm_code";
            $starttime = microtime(true);
            $rs2=$dbh->exec($sql2);
            $endtime = microtime(true);
            $duration = $endtime - $starttime;
            $duration=number_format($duration, 4, ',', '')."s";
            ?>
            <div class="tip fleft"><?=$lang->getLang("Time took to execute your request")?>:<?=$duration?></div>
            <?
            $result2=array();
            $numrow2=$rs2->getNumRows();
            while($rs2->hasNext()){
                $row2=$rs2->next();
                array_push($result2,$row2);
            }
            $rss2= new Recordset($result2);
            $cols_arr2 = array('Club','Username','Totem Code');
            $val_fmt_arr2 = array (
                'Club'          =>'return ($rec["jur_name"]);',
                'Username'      =>'return getClassLink("' . refPage('customers/customer_view&customer_id={$rec["pun_id"]}') . '", $rec["pun_username"]);',
                'Totem Code'    =>'return ($rec["ttm_code"]);',
            );


            if($numrow2!=0){
                $qp2 = new QueryPrinter($rss2);
                $qp2->printTable($rss2,$cols_arr2,"",$val_fmt_arr2,$cell_fmt_arr2, 0, 0,"",$width="100%", 1, $numrow2,false);
                ?>
                <br>
            <?
            }else{
                addError("", $lang->getLang("No result found"));
                showErrors();
            }
        }
    }
    if($date_start ==''){
        addError("",  $lang->getLang("Please choose a start date"));
    }
    if($tid==''){
        addError("",  $lang->getLang("Please choose at least one totem"));
    }
    if($date_end==''){
        addError("", $lang->getLang("Please choose a end date"));
    }
    /* if(strtotime($date_end)-strtotime($date_start)>3600*24){
         addError("", "Please choose a max 1 day interval !");
     }*/

    if(areErrors()){
        showErrors();
    }
    else{
        $starttime = microtime(true);
        $sql=" select jur_name,pun_username,ttm_code,gam_name,pre_bet_string, pre_game_num,pre_pun_id,res_id,pre_res_id,pre_time,pre_bet,pre_win,pre_balance,pre_pun_ip from result, punter_result, totem,game,punter,jurisdiction
where pre_res_id = res_id
and res_date between '$date_start:00' AND '$date_end:59'
and pre_pun_id = ttm_pun_id
and res_gam_id = gam_id
and pre_pun_id=pun_id
and pun_betting_club=jur_id
and ttm_code IN ('$tid') ORDER BY ttm_code,res_date";
        $rs=$dbh->exec($sql);
        $endtime = microtime(true);
        $duration = $endtime - $starttime;
        $duration=number_format($duration, 4, ',', '')."s";
        $numrow=$rs->getNumRows();
        $result=array();
        while($rs->hasNext()){

            $row=$rs->next();
            $detHandUser     = explode(";", $row['pre_bet_string']);

            array_push($result,$row);
        }

        $rss= new Recordset($result);
        $cols_arr = array('Hand ID','Game Nr','Date','Club','Username','Totem Code', 'Ip', 'Game', 'Bet', 'Win', 'Balance');
        $val_fmt_arr = array (
            'Hand ID'       =>'return getClassLink("javascript:openWindow(\'/casino_games/hand_details.php?gmn={$rec["pre_game_num"]}&uid={$rec["pre_pun_id"]}&hand_id={$rec["res_id"]}\',\'hand{$rec["pre_res_id"]}\',\'950\',\'980\',\'0\',\'1\')", $rec["res_id"], "contentlink");',
            'Game Nr'       =>'return getClassLink("javascript:openWindow(\'/casino_games/hand_details.php?gmn={$rec["pre_game_num"]}&uid={$rec["pre_pun_id"]}&hand_id={$rec["res_id"]}\',\'hand{$rec["pre_res_id"]}\',\'950\',\'980\',\'0\',\'1\')", $rec["pre_game_num"], "contentlink");',
            'Date'          =>'return ($rec["pre_time"]);',
            'Club'          =>'return ($rec["jur_name"]);',
            'Username'      =>'return getClassLink("' . refPage('customers/customer_view&customer_id={$rec["pre_pun_id"]}') . '", $rec["pun_username"]);',
            'Totem Code'    =>'return ($rec["ttm_code"]);',
            'Bet'           =>'return getInDollars($rec["pre_bet"]);',
            'Win'           =>'return getInDollars($rec["pre_win"]);',
            'Balance'       =>'return getInDollars($rec["pre_balance"]);',
            'Game'          =>'return getGamesName($rec["gam_name"]);',
            'Ip'            =>'return getClassLink(refPage(\'customers/search&field=4&search_string=\' . urlencode(ereg_replace("[^0-9\.]", "", $rec["pre_pun_ip"]))), $rec["pre_pun_ip"]);'
        );


        if($numrow!=0){
            ?><div class="tip fleft"><?=$lang->getLang("Time took to execute your request")?>:<?=$duration?></div><?
            $qp = new QueryPrinter($rss);
            $qp->printTable($rss,$cols_arr,"",$val_fmt_arr,$cell_fmt_arr, 0, 0,"",$width="100%", 1, $numrow,false);
        }else{
            addError("",  $lang->getLang("No result found"));
            showErrors();
        }
    }
}

