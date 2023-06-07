<script type="text/javascript" src="media/jscharts.js"></script>
<script type="text/javascript">
    function searchCroupier(){
        var valueToSearch = $("input#searchfield").val();
        $.ajax({
            url: "live/croupier_func.inc.php",
            data: {reqCode:1, value:valueToSearch},
            success: function(data){
                if(data != ""){
                    var valueData = "<select name='croupier'>";
                    var listCroupier = data.split(";");
                    for(var i = 0; i < listCroupier.length; i++){
                        var croupier = listCroupier[i].split(":");
                        valueData += "<option value='"+croupier[0]+"'>"+croupier[1]+"</option>";
                    }
                    valueData += "</select>";
                    $("div#crpsearch").html(valueData);
                }else{
                    $("div#crpsearch").html("");
                }
            }
        });
    }

    $(function () {
        $("#date_start").datepicker({
            changeMonth: true,
            numberOfMonths: 1,
            onSelect: function (selectedDate) {
                $("#date_end").datepicker("option", "minDate", selectedDate);
                dateMax = $("#date_start").datepicker("getDate");
                var rangeMax = new Date(dateMax.getFullYear(), dateMax.getMonth() , dateMax.getDate()+7);
                $("#date_end").datepicker("option", "maxDate", rangeMax);
            }
        });
        $("#date_end").datepicker({
            changeMonth: true,
            numberOfMonths: 1,
            onSelect: function (selectedDate) {
                $("#date_start").datepicker("option", "maxDate", selectedDate);
            }
        });
    });
</script>
<?php
$numbers = array();

check_access('live_croupier_number', $redirect=true);
include_once("funclive.inc.php");

$from = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d", time() - (60 * 60 * 24));
$to = (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d", time());
?>
<div class="bodyHD"><?=$lang->getLang("Numbers Stats")?></div>
<?form_head(refFormPage($_GET["page"]) . "&cid=".$_GET['cid']."&action=show", "search", "POST");?>
<?php
if($_POST['form_submitted'] == "yes"){

    $from = $_POST['date_start'];
    $to = $_POST['date_end'];

    $start_time = strtotime($from);
    $end_time = strtotime($to);

    if($end_time - $start_time < 60 * 60 * 24 * 7){

        $sql = "SELECT res_id,
					   res_string, 
					   gam_name, 
					   res_date, 
					   crp_code,
					   crp_first_name,
					   crp_last_name,
					   res_total_bets, 
					   res_total_payouts, 
					   res_num_players, 
					   res_num_betters 
					   	FROM `result` 
					   	LEFT JOIN croupier on crp_id = res_dealer_id 
					   	LEFT JOIN game on gam_id = res_gam_id 
					   		WHERE res_dealer_id = ".$_GET['cid']." 
					   		AND res_date > '$from 00:00:00'
					   		AND res_date < '$to 23:59:59'
					   		AND res_gam_id in (6001,6002,6003);";
        $rs = $dbh->exec($sql);
        while($rs->hasNext()){
            $row = $rs->next();
            $extract = $row['res_string'];
            list($number, $color) = split("\|",$extract);
            array_push($numbers, array('number' => $number, 'color' => $color, 'sector' => getSector($number)));
        }
        $colors     = array('#FA5E1F', '#FDCB3F', '#71D743', '#D23333', '#BAE73F', '#AB7B55', '#B381C9');

        $sectorsGraphs = array();
        $sectorsTitle = "Settori";
        $colors_exp_sector = array();

        $orph = getCountInSector($numbers,ORPHELINS);
        $vois = getCountInSector($numbers,VOISINS);
        $tier = getCountInSector($numbers,TIER);
        array_push($sectorsGraphs, "['ORPHELINS', $orph]");
        array_push($sectorsGraphs, "['VOISINS', $vois]");
        array_push($sectorsGraphs, "['TIER', $tier]");

        for($i = 0 ; $i < count($sectorsGraphs) ; $i++){
            array_push($colors_exp_sector, "'" . $colors[$i%count($colors)] . "'");
        }

        $numbersGraphs = array();
        $numbersTitle = "Numeri";
        $colors_exp_number = array();

        for($i = 0; $i < 37; $i++){
            $numb = getCountNumber($numbers,$i);
            array_push($numbersGraphs, "['$i', $numb]");
        }
        for($i = 0 ; $i < count($numbersGraphs) ; $i++){
            array_push($colors_exp_number, "'" . $colors[$i%count($colors)] . "'");
        }
        $colorsGraphs = array();
        $colorsTitle = "Red & Black";
        $colors_exp_color = array();
        $colors = array('#FF0000', '#000000');
        $red = getCountColor($numbers,2);
        $black = getCountColor($numbers,1);
        array_push($colorsGraphs, "['red', $red]");
        array_push($colorsGraphs, "['black', $black]");

        for($i = 0 ; $i < count($colorsGraphs) ; $i++){
            array_push($colors_exp_color, "'" . $colors[$i%count($colors)] . "'");
        }
        ?>

        <table border="0" cellpadding="0" cellspacing="0">
            <tr valign="top">
                <td>
                    <?php
                    $croupier_bets = check_access('croupier_bets');

                    if($croupier_bets){
                        $cols_arr    = array('Hands Id','Croupier Name','Number','Sector','Date','Players Betters', 'Total Bet', 'Total Win','Game');

                        $val_fmt_arr = array (
                            'Hands Id'  => 'return getClassLink("' . refPage('casino_games/searchuserhand') . '&handid=" . $rec["res_id"] . "&from=" . $rec["res_date"] . "&to=" . date("Y-m-d H:i", (strtotime($rec["res_date"]) + 60 * 60 * 3)), "View Hands", "", "_blank");',
                            'Croupier Name' => 'return $rec["crp_last_name"]. " " . $rec["crp_first_name"];',
                            'Number'          => 'return printExtractNumber($rec["res_string"]);',
                            'Sector' => 'return printSector($rec["res_string"]);',
                            'Date'  => 'return $rec["res_date"];',
                            'Players Betters' => 'return $rec["res_num_players"] ." / " . $rec["res_num_betters"] ;',
                            'Total Bet' => 'return getInDollars($rec["res_total_bets"]);',
                            'Total Win' => 'return getInDollars($rec["res_total_payouts"]);',
                            'Game'      => 'return $rec["gam_name"];'
                        );
                    }else{
                        $cols_arr    = array('Croupier Name','Number','Sector','Date','Game');

                        $val_fmt_arr = array (
                            'Croupier Name' => 'return $rec["crp_last_name"]. " " . $rec["crp_first_name"];',
                            'Number'          => 'return printExtractNumber($rec["res_string"]);',
                            'Sector' => 'return printSector($rec["res_string"]);',
                            'Date'  => 'return $rec["res_date"];',
                            'Game'      => 'return $rec["gam_name"];'
                        );

                    }
                    $cell_fmt_arr = array ();

                    $numrow = $rs->getNumRows();
                    if($numrow > 0){
                    $qp = new QueryPrinter($rs);
                    $qp->printTable($rs, $cols_arr, "", $val_fmt_arr, $cell_fmt_arr, 0, 0, "", $width="100%", 1, $numrow, false);

                    ?>
                </td>
                <td width="30"></td>
                <td>
                    <table border="0" cellpadding="4" cellspacing="1">
                        <tr valign="top">
                            <td><div id="sectorchart"></div></td>
                            <td><div id="colorchart"></div></td>
                        </tr>
                        <tr>
                            <td colspan="2"><div id="numberchart"></div></td>
                        </tr>
                    </table>
                </td>
            </tr>
        </table>
        <script type="text/javascript">

            var myData  = new Array(<?=implode(",", $sectorsGraphs)?>);
            var myChart = new JSChart('sectorchart', 'bar');
            var colors = [<?=implode(",", $colors_exp_sector)?>];
            myChart.setDataArray(myData,"data");
            myChart.colorizeBars(colors);
            myChart.setTitle('<?=$sectorsTitle?>');
            myChart.setTitleColor('#8E8E8E');
            myChart.setAxisNameX('Sector');
            myChart.setAxisNameY('Amount');
            myChart.setAxisColor('#c6c6c6');
            myChart.setAxisWidth(1);
            myChart.setAxisNameColor('#9a9a9a');
            myChart.setAxisValuesColor('#939393');
            myChart.setAxisPaddingTop(60);
            myChart.setAxisPaddingLeft(50);
            myChart.setAxisPaddingBottom(60);
            myChart.setTextPaddingBottom(20);
            myChart.setTextPaddingLeft(15);
            myChart.setTitleFontSize(11);
            myChart.setBarBorderWidth(2);
            myChart.setBarSpacingRatio(50);
            myChart.setBarValuesColor('#737373');
            myChart.setGrid(true);
            myChart.setFlagOpacity(0.1);

            myChart.setSize(250, 250);
            myChart.draw();

            var myData  = new Array(<?=implode(",", $numbersGraphs)?>);
            var myChart = new JSChart('numberchart', 'bar');
            var colors = [<?=implode(",", $colors_exp_number)?>];
            myChart.setDataArray(myData,"data");
            myChart.colorizeBars(colors);
            myChart.setTitle('<?=$numbersTitle?>');
            myChart.setTitleColor('#8E8E8E');
            myChart.setAxisNameX('Numbers');
            myChart.setAxisNameY('Amount');
            myChart.setAxisColor('#c6c6c6');
            myChart.setAxisWidth(1);
            myChart.setAxisNameColor('#9a9a9a');
            myChart.setAxisValuesColor('#939393');
            myChart.setAxisPaddingTop(60);
            myChart.setAxisPaddingLeft(50);
            myChart.setAxisPaddingBottom(60);
            myChart.setTextPaddingBottom(20);
            myChart.setTextPaddingLeft(15);
            myChart.setTitleFontSize(11);
            myChart.setBarBorderWidth(2);
            myChart.setBarSpacingRatio(50);
            myChart.setBarValuesColor('#737373');
            myChart.setGrid(true);
            myChart.setFlagOpacity(0.1);

            myChart.setSize(600, 250);
            myChart.draw();

            var myData  = new Array(<?=implode(",", $colorsGraphs)?>);
            var myChart = new JSChart('colorchart', 'pie');
            var colors = [<?=implode(",", $colors_exp_color)?>];
            myChart.setDataArray(myData,"data");
            myChart.colorize(colors);
            myChart.setTitle('<?=$colorsTitle?>');
            myChart.setSize(250, 250);
            myChart.draw();
        </script>
    <?php
    }else{
        addError("", "No number found :)");
        showErrors();
    }
    }else{
        addError("","Too long period");
        showErrors();
    }
}
?>
<table border="0" cellpadding="0" cellspacing="0">
    <tr valign="top">
        <td>
            <table border="0" cellpadding="0" cellspacing="0">
                <tr>
                    <td colspan="3" class="formheading"><?=$lang->getLang("Date")?></td>
                </tr>
                <?php if(!isset($_GET['cid'])){ ?>
                    <tr>
                        <td width="150" class="label"><?=$lang->getLang("Choose a croupier")?></td>
                        <td class="content">
                            <input type="text" value="" id="searchfield" onchange="searchCroupier();" />
                        </td>
                        <td class="content">
                            <div id="crpsearch"></div>
                        </td>
                    </tr>
                <?php } ?>
                <tr>
                    <td width="150" class="label"><?=$lang->getLang("From")?></td>
                    <td class="content"><input type='text' name='date_start' id='date_start' value="<?= $from ?>"></td>
                </tr>
                <tr>
                    <td width="150" class="label"><?=$lang->getLang("Until")?></td>
                    <td class="content"> <input type='date_end' name='date_end' id='date_end' value="<?= $to ?>"></td>
                </tr>
                <tr>
                    <td><input type="submit" value="<?=$lang->getLang("Search")?>" /></td>
                </tr>
            </table>
        </td>
    </tr>
</table>