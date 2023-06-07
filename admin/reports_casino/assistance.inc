<?php
check_access('transactions_refunds',true);
?>

    <link rel="Stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/js/dataTables/1.10.15/datatables.css"  />
    <script src="/media/js/dataTables/1.10.15/datatables.js" type="text/javascript"></script>
    <script type="text/javascript">
        $(function() {
            $( ".dateStartHelper" ).datepicker({
                changeMonth: true,
                changeYear: true,
                numberOfMonths:1,
                onSelect: function( selectedDate ) {
                    $( ".monthHelper, .weekHelper" ).val('');
                }
            });
            $( ".dateEndHelper" ).datepicker({
                changeMonth: true,
                changeYear: true,
                numberOfMonths:1,
                onSelect: function( selectedDate ) {
                    $( ".monthHelper, .weekHelper" ).val('');
                }
            });

            $(document).on('change', '.monthHelper', function() {
                var d = new Date($(this).val());
                createMonthHelper(d);
            });
            $(document).on('change', '.weekHelper', function() {
                createWeekHelper($(this).val());
            });
            function createWeekHelper(dateobj){
                if (typeof dateobj !== 'undefined' || dateobj!=''){
                    dates=dateobj.split('~');
                    firstDay=dates[0];
                    lastDay=dates[1];
                    $( ".dateStartHelper" ).datepicker('setDate', firstDay);
                    $( ".dateEndHelper" ).datepicker('setDate', lastDay);
                    $('.monthHelper').val('');
                }
            }

            function createMonthHelper(dateobj){
                if (typeof dateobj !== 'undefined' || dateobj!=''){
                    var firstDay = new Date(dateobj.getFullYear(), dateobj.getMonth(), 1);
                    $( ".dateStartHelper" ).datepicker('setDate', firstDay);
                    var lastDay = new Date(dateobj.getFullYear(), dateobj.getMonth() + 1, 0);
                    $( ".dateEndHelper" ).datepicker('setDate', lastDay);
                    $('.weekHelper').val('');
                }
            }
            $('#resultsTable').DataTable({'sPaginationType': "full_numbers"});
        });

    </script>

    <style>
        .paging_full_numbers {
            height: auto;
            line-height:normal;
            width: auto;
        }
    </style>
<div class="bodyHD" align="center"><?=$lang->getLang("Assistance Refund Reports")?></div>
<br/>
<?php form_head()?>
<?php printHiddenInput('doQuery', 'yes');?>
<?php printHiddenInput('search_type', 'date_range');?>

<?php
    require_once 'PHPExcel.php';
    require_once 'PHPExcel/Writer/Excel2007.php';
    require_once 'PHPExcel/Style/Fill.php';
    require_once 'PHPExcel/Style/Color.php';
	$doQuery = $_POST['doQuery'];
    $date_start = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d", time() - (60 * 60 * 24));
    $date_end = (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d", time());
    $username = (isset($_POST['username'])) ? $_POST['username'] : '';
    $exact = (isset($_POST['exact'])) ? true : false;

?>
<table  align="center" cellpadding="4" cellspacing="1" border="0">

    <tr valign=top>
        <td class=label><?=$lang->getLang("Choose month")?></td>
        <td class=content>
            <?=month_select_box()?>
        </td>
    </tr>

    <tr valign=top>
        <td class=label><?=$lang->getLang("Choose week")?></td>
        <td class=content>
            <?=week_select_box()?>
        </td>
    </tr>




  <tr valign=top>
    <td class=label><?=$lang->getLang("From")?></td>
    <td class=content>
        <input type='text' name='date_start' id='date_start' class="dateStartHelper"  value="<?=$date_start?>" >
    </td>
  </tr>
  <tr valign=top>
    <td class=label><?=$lang->getLang("Until")?></td>
    <td class=content>
        <input type='text' name='date_end' id='date_end' class="dateEndHelper" value="<?=$date_end?>">
    </td>
  </tr>
    <tr valign=top>
        <td class=label><?=$lang->getLang("Username")?></td>
        <td class=content>
            <input type='text' name='username' id='username' value="<?=$username?>" placeholder="Optional">
        </td>
    </tr>    <tr valign=top>
        <td class=label><?=$lang->getLang("Exact match")?></td>
        <td class=content>
            <input type='checkbox' name='exact' id='exact' <?=($exact?'checked':'')?>>
        </td>
    </tr>
<tr>
    <td class=content>&nbsp;</td>
    <td class=content><input type="submit" value="<?=$lang->getLang("Calculate")?>" /></td>
  </tr>
  </table>

  <?php
   if($doQuery){
        $start_time = microtime(true);
        $sql = "Select count(distinct aus_username) As countUsers, ctr_aus_id, ctr_notes, coalesce(SUM(CASE WHEN ctr_tty_id = 6 and ctr_amount > 0 THEN ctr_amount END ),0) deposit,
                coalesce(SUM(  CASE WHEN ctr_tty_id =6 and ctr_amount < 0 THEN ctr_amount END ), 0) withdraw, ctr_pun_id, ctr_time time, date_format(ctr_time,'%Y-%m-%d'),
                aus_username, pun_username,cur_code_for_web
                from customer_transaction
                LEFT JOIN admin_user on aus_id = ctr_aus_id
                LEFT JOIN punter on pun_id = ctr_pun_id
                LEFT JOIN jurisdiction on pun_betting_club=jur_id
                LEFT JOIN currency on jur_currency=cur_id
                where ctr_time >= date_format('$date_start','%Y%m%d000000')
                and ctr_time <= date_format('$date_end','%Y%m%d235959')
                AND ctr_tty_id = 6 ";
        if($username!=''){
            if($exact){
                $sql.=" AND pun_username =".$dbh->prepareString($username);
            }
            else{
                $sql.=" AND pun_username like ".$dbh->prepareString("%".$username."%");
            }
        }
        $sql.=" group by ctr_aus_id, date_format(ctr_time,'%Y%m%d000000'), ctr_id";

       $res= $dbh->exec($sql);
       $end_time = microtime(true);
       $duration = $end_time - $start_time;
       $duration=number_format($duration, 4, ',', '')."s";

        $totalDeposit=0;
        $totalWithdraw=0;
        $totalRefunds=0;
        $totalUsers=0;
       if($res->NumRows!=0){

           ?>
           <div class="tip" id='duration'><?=$lang->getLang("Time taken to execute your request")?>: <?=$duration?></div><br>
            <?
         //   printInfoMessage("$res->NumRows records found.");
            ?>
            <br>

         <?$result='  <table class="table display table-striped" id="resultsTable" >
           <thead>
               <tr>
               <th>'.$lang->getLang("Operator").'</th>
               <th>'.$lang->getLang("Notes").'</th>
               <th>'.$lang->getLang("Deposit").'</th>
               <th>'.$lang->getLang("Withdrawal").'</th>
               <th>'.$lang->getLang("Username").'</th>
               <th>'.$lang->getLang("Refund DateTime").'</th>
           </tr>
           </thead>
               <tbody>';

         while($res->hasNext())
            {
                $row=$res->next();


                $totalDeposit+=$row['deposit'];
                $totalWithdraw+=$row['withdraw'];
                $totalRefunds++;



               $result.=" <tr>
                    <td>". strtoupper($row['aus_username'])."</td>
                    <td>".$row['ctr_notes']."</td>
                    <td>".getInDollars($row['deposit'],2,$row['cur_code_for_web'])."</td>
                    <td>".getInDollars($row['withdraw'],2,$row['cur_code_for_web'])."</td>
                    <td>".$row['pun_username']."</td>
                    <td>".$row['time']."</td>
                </tr>";

            }

               $result.="</tbody>
      </table>";

         $totalsTable="<table class=' table-striped display table'>

    <thead>
    <tr>
        <th>".$lang->getLang("Total deposit")."</th>
        <th>".$lang->getLang("Total withdraw")."</th>
        <th>".$lang->getLang("Difference")."</th>
        <th>".$lang->getLang("Refunds")."</th>
        <th>".$lang->getLang("Users")."</th>
        </tr>
    </thead>
    <tbody>
    <tr>
        <td>".getInDollars($totalDeposit)."</td>
        <td>".getInDollars($totalWithdraw)."</td>
        <td>".getInDollars($totalDeposit+$totalWithdraw)."</td>
        <td>".$totalRefunds."</td>
        <td>".$row['countusers']."</td>
    </tr>
    </tbody>
    </table>
        </table>";
       echo "<br />". $totalsTable."<br /><br />".$result;

       }
       else{
           addError("", $lang->getLang("No result found"));
           showErrors();
        }


       /* $cols_arr = array ('Operator', 'Notes', 'Deposit', 'Withdrawal', 'Username', 'Refund DateTime' );

        $val_fmt_arr = array ('Operator' => 'return strtoupper($rec["aus_username"]);',
        					  'Notes' => 'return $rec["ctr_notes"];',
        					  'Deposit' => 'return getInDollars($rec["deposit"]);',
        					  'Withdrawal' => 'return getInDollars($rec["withdraw"]);',
        					  'Username' => 'return $rec["pun_username"];',
        			          'Refund DateTime' => 'return $rec["time"];'
        		             );

        $cell_fmt_arr = array ();

        $post_vars = array('resforpage' =>$row_to_page );

        $start_row = (isset ( $_POST ['start_row'] )) ? $_POST ['start_row'] : 1;
        ?>

        <?
        $qp = new QueryPrinter ( $res );
        $qp->printTable ( $res, $cols_arr, $post_vars, $val_fmt_arr, $cell_fmt_arr, 0, "" , $header = "", $width = "100%", $start_row, $row_to_page, false );
       */

   }
?>