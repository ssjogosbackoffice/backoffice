<?php

check_access ( 'corrective_transaction', true );
if ($_SESSION ['aty_code'] != "SUPERUSER")
    header ( "Location: /?err=expired" );
$separator = '';
$selectedDate = true;
$selectedUser = false;
$dateStart = date('Y-m-d');
if (isset ( $_POST ['do_search'] )) {

    $groupBy = $_POST['groupby'];
    $selectedDate = false;
    $selectedUser = false;
    for($i = 0; $i < count($groupBy); $i++){
        if($groupBy[$i] == 'date'){
            $selectedDate = true;
        }
        if($groupBy[$i] == 'users'){
            $selectedUser = true;
        }
    }

    $separator = $_POST['separator'];
    $provider = $_POST['providers'];
    $dateStart = $_POST['date_start'];
    if ($_FILES['uploaded_file']['error'] == UPLOAD_ERR_OK               //checks for errors
        && is_uploaded_file($_FILES['uploaded_file']['tmp_name'])) { //checks that file is uploaded
        $file_content =  file_get_contents($_FILES['uploaded_file']['tmp_name']);

        $users = split($separator, $file_content);
        $count_users = count($users);
        $sql = "Select date_format(ctr_time,'%Y-%m-%d') date, ABS(SUM(CASE WHEN ctr_tty_id =8 THEN ctr_amount END )) bets,
					SUM(CASE WHEN ctr_tty_id =7 THEN ctr_amount END ) wins, ctr_time, ctr_gam_id gameid, ctr_pesid ,pnu_id
					from customer_transaction, partner_users 
					WHERE ctr_time > date_format(".$dbh->quote($dateStart).",'%Y%m%d000000') 
					AND ctr_pesid = $provider 
					AND ctr_tty_id in (7,8)
					AND pnu_id in (".implode(',',$users).")
					AND pnu_local_id = ctr_pun_id";

        if($selectedDate && $selectedUser){
            $sql .= " group by date_format(ctr_time,'%Y%m%d000000'), pnu_id";
        }else if($selectedDate){
            $sql .= " group by date_format(ctr_time,'%Y%m%d000000') ";
        }else if($selectedUser){
            $sql .= " group by pnu_id";
        }



        $res = $dbh->exec($sql);
        $countRow = $res->getNumRows();
        if($countRow > 0){

            if(!$selectedDate && !$selectedUser){

                $cols_arr    = array('Bets','Wins');

                $val_fmt_arr = array (
                    'Bets'      => 'return getInDollars($rec["bets"]);',
                    'Wins' 		=> 'return getInDollars($rec["wins"]);'
                );

            }else if($selectedDate && $selectedUser){
                $cols_arr    = array('Date','Users','Bets','Wins');

                $val_fmt_arr = array (
                    'Date'  	=> 'return $rec["date"];',
                    'Users'  	=> 'return $rec["pnu_id"];',
                    'Bets'      => 'return getInDollars($rec["bets"]);',
                    'Wins' 		=> 'return getInDollars($rec["wins"]);'
                );
            }else if($selectedDate){
                $cols_arr    = array('Date','Bets','Wins');

                $val_fmt_arr = array (
                    'Date'  	=> 'return $rec["date"];',
                    'Bets'      => 'return getInDollars($rec["bets"]);',
                    'Wins' 		=> 'return getInDollars($rec["wins"]);'
                );
            }else if($selectedUser){
                $cols_arr    = array('Users','Bets','Wins');

                $val_fmt_arr = array (
                    'Users'  	=> 'return $rec["pnu_id"];',
                    'Bets'      => 'return getInDollars($rec["bets"]);',
                    'Wins' 		=> 'return getInDollars($rec["wins"]);'
                );
            }



            $cell_fmt_arr = array ();

            if($countRow > 0){
                $qp = new QueryPrinter($res);
                $qp->dumpToExcelFile($res, $cols_arr, $val_fmt_arr, 'provResult.xls');
                $qp->printTable($res, $cols_arr, "", $val_fmt_arr, $cell_fmt_arr, 0, 'provResult.xls', $header="", $width="100%", 1, $countRow, false);
            }else{
                addError('', $lang->getLang('No result found'));
            }
        }else{
            addError('', $lang->getLang('No result found'));
        }
    }else{
        addError('', $lang->getLang('File not found or unreadable'));
    }
}
if($separator == ''){
    $separator = '\n';
}
?>
<?php //form_head($action=$_SERVER['PHP_SELF'], $name="", $method="POST", $onsubmit="return hash(this)", $noautocomplete=true)?>
    <form action=<?=$_SERVER['PHP_SELF'].'?page=transactions/extract_data&header_page='?> method="POST" enctype="multipart/form-data" >
<input type="hidden" name="do_search" value="yes" />
<input type="hidden" name="hashed_password" value="false" />
<table>
    <tr>
        <td class=label><?=$lang->getLang("Choose file:")?></td>
        <td class=content><input type="file" size="40" name="uploaded_file" ></td>
    </tr>
    <tr>
        <td class=label><?=$lang->getLang('Date')?>:</td>
        <td class=content><input type="text" size="11" name="date_start" value="<?=$dateStart?>" ></td>
    </tr>
    <tr>
        <td class=label><?=$lang->getLang("Provider ID")?>:</td>
        <td class=content><select name='providers'>
                <?php
                $sql_provider = 'SELECT PES_ID, PES_NAME from providers';
                $res_provider = $dbh->exec ( $sql_provider );

                while ( $res_provider->hasNext () ) {
                    $row = $res_provider->next ();
                    ?>
                    <option <?php if($_POST['providers'] == $row["pes_id"]){ ?> selected <?php }?> value='<?=$row["pes_id"]?>'><?=$row["pes_name"]?></option>
                <?php
                }
                ?>
            </select></td>
    </tr>
    <tr>
        <td class=label><?=$lang->getLang("Separator")?>:</td>
        <td class=content><input type="text" size="2" maxlenght="2" name="separator" value="<?=$separator?>" ></td>
    </tr>
    <tr>
        <td class="label"><?=$lang->getLang("Group by")?></td>
        <td>
            <input <?php if($selectedDate) {?> checked <?php }?> type="checkbox" name="groupby[]" value="date" /><?=$lang->getLang("Date")?> <br />
            <input <?php if($selectedUser) {?> checked <?php }?> type="checkbox" name="groupby[]" value="users" /><?=$lang->getLang("Users")?>
        </td>
    </tr>
    <tr>
        <td>&nbsp;</td>
        <td>
            <input type="image" src="<?=image_dir?>/button_submit.gif">
        </td>
    </tr>
</table>
</form>
<?showErrors();?>