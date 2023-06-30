<br/><br/>
<?php
if(defined('DASHBOARD_SHOW_IP')) {
    $showIP = DASHBOARD_SHOW_IP;
}else {
    $showIP = true;
}
?>
<style>
    .error td{
        color:Red;
        font-weight:Bold;
    }
    #loadingDiv {
        display:block;
        position: fixed;
        left: 0;
        top: 0;
        width: 100%;
        height: 100%;
        z-index: 9999;
        background: url(<?=image_dir?>/ajax-loader.gif?>) center no-repeat  rgba(255, 255, 255, 0.47);
    }
    .label{
        display: table-cell !important;
        border-radius:0;
        background: rgb(51, 51, 51);
        background: -moz-linear-gradient(top, #333333 0%, #222222 91%, #1f1f1f 100%);
        background: -webkit-gradient(linear, left top, left bottom, color-stop(0%,rgb(51, 51, 51)), color-stop(91%,rgb(34, 34, 34)), color-stop(100%,rgb(31, 31, 31)));
        background: -webkit-linear-gradient(top, rgb(51, 51, 51) 0%,rgb(34, 34, 34) 91%,rgb(31, 31, 31) 100%);
        background: -o-linear-gradient(top, #333333 0%,#222222 91%,#1f1f1f 100%);
        background: -ms-linear-gradient(top, #333333 0%,#222222 91%,#1f1f1f 100%);
        background: linear-gradient(top, #333333 0%,#222222 91%,#1f1f1f 100%);
        filter: progid:DXImageTransform.Microsoft.gradient( startColorstr='#333333', endColorstr='#1f1f1f',GradientType=0 ); !important;
    }
</style>
<link rel="stylesheet" type="text/css" href="media/table.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="media/bootstrap/css/bootstrap.css" title="core_style" />
<script src="media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<script>
    $(window).load(function() {
        $("#loadingDiv").fadeOut("slow");
    });
    function showHideCredits(){
        if ($(".showHideCredits").is(":hidden")){
            $(".showHideCredits").who("slow");
            $("#showHideCredits span").html("&#9660;");
        }
        else{
            $(".showHideCredits").fadeOut("slow");
            $("#showHideCredits span").html("&#9650;");
        }
        return false;
    }
    $(function(){
        $('#showHideCredits').on('click',function(){
            showHideCredits();
        });
        $('#getDetailedFinanancialData').on('click',function(){

            if($('#detailedFinancial').html()=='') {
                $("#loadingDiv").show();
                startTime=new Date;
                $.ajax({
                    url: "/services/general_services.inc",
                    xhrFields: {
                        withCredentials: true
                    },
                    type: "POST",
                    data: {action: "getAccountFinnancialSummary"}
                }).done(function (data) {
                        $('#detailedFinancial').empty().append(data);
                        $('#getDetailedFinanancialData').html("<?=$lang->getLang("Hide financials")?>")
                    })
                    .fail(function () {
                        $("#loadingDiv").fadeOut();
                    })
                    .always(function () {
                        $("#loadingDiv").hide();
                        endTime = new Date;
                        totalTime = endTime - startTime;
                        $("#duration").html('<?=$lang->getLang("Time taken to execute your request")?>:'+ totalTime/1000+" s");
                    });
            }
            else{
                if( $('#detailedFinancial').is(":visible")) {
                    $('#detailedFinancial').hide();
                    $(this).html("<?=$lang->getLang("Show financials")?>")
                }
                else{
                    $('#detailedFinancial').show();
                    $(this).html("<?=$lang->getLang("Hide financials")?>")
                }
            }


        })
    });
</script>
<h3><?=$lang->getLang("Account summary")?></h3>
<?echo "<div id='loadingDiv' class='centered'></div>";
if(check_access('financial_home_stats')) { ?>

<?php
$starttime = microtime(true);



//$today = date('Y-m-d');


?>

    <div class="tip fleft" id="duration"><?= $lang->getLang("Time taken to execute your request") ?>:</div><br />
    <button class="btn btn-small btn-primary" id="getDetailedFinanancialData"><?=$lang->getLang("Show financials")?></button>
    <div class="clearfix"></div>
<table>
    <tr>
        <td>
            <table class='table display table-bordered '>
                <thead>
                <tr>
                    <th><?= $lang->getLang("Last access") ?></th>

                </tr>
                </thead>
                <tbody>
                <tr>
                    <td>
                        <? } ?>
                        <table class='table display table-hover table-bordered '>
                            <tr>
                                <th style="border-left: 0"><?= $lang->getLang("Date") ?></th>
                                <th><?= $lang->getLang("Username") ?></th>
                                <?php if($showIP) { ?><th><?= $lang->getLang("IP") ?></th><?php } ?>
                                <th><?= $lang->getLang("Actions") ?></th>
                                <?php



                                //@TODO: Utilizzare limitquery
                                $sql = "SELECT alg_time AS Time, aus_username AS Username, alg_ipaddr as IPAddress, alg_data AS Actions " .
                                    "            FROM admin_user_log JOIN admin_user ON alg_aus_id=aus_id " .
                                    "            WHERE alg_aus_id=".$_SESSION['admin_id']." AND alg_type='admin_login' " .
                                    "              ORDER BY alg_time DESC LIMIT 0,10";

                                $rs  = $dbh->exec($sql);
                                while ($rs->hasNext()) {
                                $r = $rs->next();

                                ?>
                                <?php if(strstr($r["actions"], "unsuccessful") !== false) : ?>
                            <tr class="error">
                                <?php else : ?>
                            <tr>
                                <?php endif; ?>
                                <td style="border-left: 0" align="center"><?= date("d M Y - H:i", strtotime($r["time"])) ?></td>
                                <td align="center"><?=$r["username"]?></td>
                                <?php if($showIP) { ?><td align="center"><?=$r["ipaddress"]?></td> <?php } ?>
                                <td align="center"><?=$r["actions"]?></td>
                            </tr>
                            <?php
                            }
                            ?>
                        </table>
                        <?php if(check_access('financial_home_stats')) { ?>
                    </td>



                </tr>
                </tbody>
            </table>
        </td>
        <td style="vertical-align: top;" id="detailedFinancial"></td>
        </td></tr>
</table>
<?
    $endtime = microtime(true);
    $duration = $endtime - $starttime;
    $duration = number_format($duration, 4, ',', '') . "s";

?>
<script>
    $(function(){
        $('#duration').append(' <?= $duration ?>');
    })
        </script>
<br/>
<?php
}
if($_SESSION['aty_code'] == "SUPERUSER"){
    $date_from = date("Y-m-d", strtotime("-5 day"));
    $date = date("Y-m-d", strtotime("today"));
    $row_to_page = 100;
    $sql = "Select ctr_aus_id, ctr_notes, coalesce(SUM(CASE WHEN ctr_tty_id = 6 and ctr_amount > 0 THEN ctr_amount END ),0) deposit,
                coalesce(SUM(  CASE WHEN ctr_tty_id =6 and ctr_amount < 0 THEN ctr_amount END ), 0) withdraw, ctr_pun_id, ctr_time time, date_format(ctr_time,'%Y-%m-%d'),
                aus_username, pun_username
                from customer_transaction
                LEFT JOIN admin_user on aus_id = ctr_aus_id
                LEFT JOIN punter on pun_id = ctr_pun_id
                where ctr_time >= date_format('$date_from','%Y%m%d000000')
                and ctr_time <= date_format('$date','%Y%m%d235959')
                AND ctr_tty_id = 6
                AND aus_id <> 0
                group by ctr_aus_id, date_format(ctr_time,'%Y%m%d000000'), ctr_id";
    $res= $dbh->exec($sql);

    $cols_arr = array ($lang->getLang('Operator'),$lang->getLang('Notes'), $lang->getLang('Deposit'), $lang->getLang('Withdrawal'), $lang->getLang('Username'), $lang->getLang('Refund DateTime'));

    $val_fmt_arr = array ($lang->getLang('Operator') => 'return strtoupper($rec["aus_username"]);',
        $lang->getLang('Notes') => 'return $rec["ctr_notes"];',
        $lang->getLang('Deposit') => 'return getInDollars($rec["deposit"]);',
        $lang->getLang('Withdrawal') => 'return getInDollars($rec["withdraw"]);',
        $lang->getLang('Username') => 'return $rec["pun_username"];',
        $lang->getLang('Refund DateTime') => 'return $rec["time"];');

    $cell_fmt_arr = array ();

    $post_vars = array('resforpage' =>$row_to_page );

    $start_row = (isset ( $_POST ['start_row'] )) ? $_POST ['start_row'] : 1;
    ?>
    <?
    $qp = new QueryPrinter ( $res );
    $qp->printTable ( $res, $cols_arr, $post_vars, $val_fmt_arr, $cell_fmt_arr, 0, "" , $header = "", $width = "100%", $start_row, $row_to_page, false,false,false,'table display table-bordered' );
}

?>
