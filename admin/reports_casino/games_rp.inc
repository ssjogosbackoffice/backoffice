<?php
check_access("casino_view_games_reports", true);
?>
    <link rel="stylesheet" type="text/css" href="/media/jquery.multiselect.css" title="core_style">
    <script src="/media/jquery.multiselect.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/highcharts.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/highcharts-3d.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/modules/exporting.js" type="text/javascript"></script>

<div class="bodyHD" style="text-align: center"><?=$lang->getLang('Games Reports')?></div>
<br/>
<?php form_head() ?>
<?php printHiddenInput('doQuery', 'yes'); ?>
<?php printHiddenInput('search_type', 'date_range'); ?>
<?php
require_once 'JurisdictionsList.class.inc';
require_once 'PHPExcel.php';
require_once 'PHPExcel/Writer/Excel2007.php';
require_once 'PHPExcel/Style/Fill.php';
require_once 'PHPExcel/Style/Color.php';
$doQuery = $_POST['doQuery'];
$date_start = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d", time() - (60 * 60 * 24));
$date_end = (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d", time());
$jurClass = JurisdictionsList::getInstance($dbh);
$clubList = $jurClass->getChilJurisdictionsDataList($_SESSION ['jurisdiction_id'], 'club');
$whichClub = "*";
if (isset($_POST['clubname'])) {
    $whichClub = $_POST['clubname'];
}
?>
<script type="text/javascript">
    $(function () {
        $('#clubname').select2({
            width: "158px"
        });

        $('#games').multiselect({
            selectedList: 4,
            noneSelectedText:'<?=$lang->getLang("Select a game")?>'
        }).multiselectfilter();

        $("#date_start").datepicker({
            changeMonth: true,
            numberOfMonths: 1,
            onSelect: function (selectedDate) {
                $("#date_end").datepicker("option", "minDate", selectedDate);
                dateMax = $("#date_start").datepicker("getDate");
                var rangeMax = new Date(dateMax.getFullYear(), dateMax.getMonth() + 1, dateMax.getDate());
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

        $("input[name='cclub']").change(function () {
            $("#clubselect").toggle();
        });
        $("input[name='cclub']:checked").change();
        $("input[name='cgames']").change(function () {
            $("#gamesselect").toggle();
            if ($("input[name='ccategory']").attr("disabled")) {
                $("input[name='ccategory']").removeAttr("disabled");
                $('#gametochoose').find(':checkbox').each(function() {
                    this.checked = false;
                });
            } else {
                $("input[name='ccategory']").removeAttr("checked");
                $("input[name='category']").removeAttr("checked");
                $("#categoryselect").hide();
                $("input[name='ccategory']").attr("disabled", "disabled");
            }
        });
        $("input[name='cgames']:checked").change();

        $("input[name='ccategory']").change(function () {
            $("#categoryselect").toggle();
            if ($("input[name='cgames']").attr("disabled")) {
                $("input[name='cgames']").removeAttr("disabled");
                $("input[type='radio']").removeAttr("checked");
            } else {
                $("input[name='cgames']").removeAttr("checked");
                $("input[name='games[]']").removeAttr("checked");
                $("#gamesselect").hide();
                $("input[name='cgames']").attr("disabled", "disabled");
            }
        });
        $("input[name='ccategory']:checked").change();

        $("input[name='cclub']").change(function(){
            $('#clubname option[value="*"]').attr('selected','selected');
        });
    });

    function getGamesCategory(category)
    {
        $.post("reports_casino/retreive_category_games.php",{category:category}, function(data){
            $("#gametochoose").empty().append( data );
        });
    }
</script>


<table align="center" style='padding:4px;border-spacing:1px;' border="0">
    <tr valign='top'>
        <td class='label' style="padding: 5px"><?=$lang->getLang('From')?></td>
        <td class='content'>
            <input type='text' name='date_start' id='date_start' value="<?= $date_start ?>">
        </td>
    </tr>
    <tr valign='top'>
        <td class='label' style="padding: 5px"><?=$lang->getLang('Until')?></td>
        <td class='content'>
            <input type='text' name='date_end' id='date_end' value="<?= $date_end ?>">
        </td>
    </tr>
    <tr>
        <td class="label" style="padding: 5px"><?=$lang->getLang('Search by')?>:</td>
        <td class="content">
            <input type='checkbox' name='cclub' <?php if(isset($_POST['cclub'])) {  ?>checked="checked"<?php } ?> ><?=$lang->getLang("Club")?>
            <input type='checkbox' name='ccategory' <?php if(isset($_POST['ccategory'])) {  ?>checked="checked"<?php } ?> ><?=$lang->getLang("Category")?>
            <input type='checkbox' name='cgames' <?php if(isset($_POST['cgames'])) {  ?>checked="checked" <?php } ?> > <?=$lang->getLang("Games")?>
            <input type='checkbox' name='date' <?php if(isset($_POST['date'])) {  ?>checked="checked" <?php } ?> > <?=$lang->getLang("Date")?>
        </td>
    </tr>
    <tr id='clubselect'>
        <td class="label" style="padding: 5px"><?=$lang->getLang('Select the club')?></td>
        <td class="content">
            <select name="clubname" id='clubname'>
                <option <?php if ($whichClub == "*") { ?> selected <?php }?> value="*"><?=$lang->getLang('All clubs')?></option>
                <?php
                    foreach ($clubList as $k => $v) {
                 ?>
                    <option <?php if ($whichClub == $v[0]) { ?> selected <?php }?>
                        value="<?= $v[0] ?>"><?=$v[1]?></option>    <?php
                }
                ?>
            </select>
        </td>
    </tr>
    <tr id='categoryselect'>
        <td class="label"><?=$lang->getLang('Select category')?></td>
        <td>
            <p style="height: 80px; width:125px; overflow: auto; border: 5px solid #eee; background: #eee; color: #000; margin: 0px;">
                <label><input type="radio" name="category" <?php if(isset($_POST['category'])&&($_POST['category']=='provider')) {  ?>checked="checked"<?php } ?>  value="provider">provider</label><br>
                <label><input type="radio" name="category" <?php if(isset($_POST['category'])&&($_POST['category']=='casino')) {  ?>checked="checked"<?php } ?> value="casino">casino </label><br>
                <label><input type="radio" name="category" <?php if(isset($_POST['category'])&&($_POST['category']=='casino_live')) {  ?>checked="checked"<?php } ?> value="casino_live">casino_live </label><br>
                <label><input type="radio" name="category" <?php if(isset($_POST['category'])&&($_POST['category']=='casino_agency')) {  ?>checked="checked"<?php } ?> value="casino_agency">casino_agency </label><br>
            </p>
        </td>
    </tr>

    <tr id='gamesselect'>
        <td class="label"><?=$lang->getLang('Select the games')?></td>
        <td>
            <!--<div id='gametochoose' style="height: 150px; width:1100px; overflow: auto; border: 5px solid #eee; background: #eee; color: #000; margin: 0px;">
                <ul style='float:left;margin-right: 5px;'>-->
           <select id='games' name="games[]"  multiple="multiple" >

            <?php
                /*$sql = "select gam_id, gam_name,gam_category from game where gam_category in ('casino', 'casino_live', 'casino_agency', 'provider_poker_live', 'provider_gaminator','provider_poker','provider_betting','provider_live_betting')  order by gam_category";*/
                 $sql="select gam_id, gam_name,gam_category from game JOIN game_category on GAM_CATEGORY=gct_name order by gam_category";
                $resGames = $dbh->exec($sql);
                $gameList = array();
                $category='';
                $activatedGames = $_POST['games'];

                while ($resGames->hasNext()) {
                   $row = $resGames->next();
                    if($row['gam_category']!=$category){ ?>
                        <optgroup label="<?= str_replace('provider_','',$row['gam_category'])?>" value="<?= str_replace('provider_','',$row['gam_category'])?>"> <? }
                            $category=$row['gam_category'];
                    ?>
                          <option <?=(in_array( $row['gam_id'],$activatedGames))  ? 'selected' : ''?> value="<?= $row['gam_id'] ?>" /><?= $row['gam_name']?> </option>
                    <?
                    $gameList[$row['gam_id']] = $row['gam_name'];
                }
                ?>
              <!--  </ul>
            </div>-->
                            </optgroup>
        </td>
    </tr>
    <tr>
        <td class=content></td>
        <td class=content>
            <input type="submit" class="entitybtn" value="<?=$lang->getLang('Calculate')?>"/>
        </td>
    </tr>
</table>
<?php
if ($doQuery) {
    /*$sql = "SELECT pun_betting_club AS clubid,
  	          cus_res_gam_id as gameid , jur_name as clubname,
  	          date_format(cus_res_day, '%Y-%m-%d') as cus_day,
  	          sum(cus_res_bet) as totalbet,
  	          sum(cus_res_win) as totalwin,
  	          count(*) as players,
  	          gam_name
          		FROM customer_result_daily
          		JOIN punter ON cus_res_pun_id = pun_id
          		JOIN jurisdiction ON jur_id = pun_betting_club
                JOIN game on cus_res_gam_id = gam_id
          		 WHERE cus_res_day BETWEEN date_format('$date_start 00:00:00', '%Y-%m-%d %H:%i:%s') AND date_format('$date_end 23:59:59', '%Y-%m-%d %H:%i:%s')
          		 AND cus_res_gam_id in (" . implode(",", $activatedGames) . ")
          		 AND pun_betting_club in (" . implode(",", $idList) . ")
                 GROUP BY pun_betting_club,date_format(cus_res_day, '%Y-%m-%d') , cus_res_gam_id
  	            ORDER BY  cus_res_gam_id, date_format( cus_res_day, '%Y-%m-%d' )";
    */

   $params ="c.jur_name as clubname,gct_id,  sum(cus_res_bet+cus_res_tournament_bet) totalbet, sum(cus_res_win+cus_res_tournament_win) totalwin,IF( (gct_id=7 OR gct_id=9),(sum(cus_res_bet)-sum(cus_res_win)),  sum(cus_res_rake+cus_res_tournament_rake)) tot_rake, count(cus_res_pun_id) players, (sum(cus_res_win) / sum(cus_res_bet) ) * 100 perc ,DATE_FORMAT(cus_res_day, '%Y-%m-%d') cus_day,gam_name,cur_code_for_web, cur_id as currency, cus_res_played ";
    if(isset($_POST['day'])){
        $params .= "   ";

    }
    if(isset ($_POST['clubname'])){
        if($_POST['clubname']!='*'){
            $where .=" AND c.jur_id =".$dbh->escape($whichClub);
        }
    }

    if(isset($_POST['category'])){
        if($_POST['category']=='provider'){
            $where.=" and gam_category LIKE('%".$_POST['category']."%') ";
        }
        elsE {
            $where .= " and gam_category IN ('" . $_POST['category'] . "') ";
        }
    }

    elseif(isset($_POST['cgames']) && $activatedGames!=''){

        $where.= " and cus_res_gam_id in (" . implode(",", $activatedGames) . ") ";
        /*$where.=" and gam_category in ('casino', 'casino_live', 'casino_agency', 'provider_poker_live', 'provider_gaminator','provider_poker','provider_betting','provider_live_betting') ";*/
    }

    $starttime = microtime(true);
    $sql="select $params
          from jurisdiction c,currency,";
    if($_SESSION['jurisdiction_class']=='nation'){
     $sql.="jurisdiction n,jurisdiction r,jurisdiction d,";
    }
    if($_SESSION['jurisdiction_class']=='region'){
        $sql.=   "jurisdiction r,jurisdiction d,";
    }
    if($_SESSION['jurisdiction_class']=='district'){
        $sql.="jurisdiction d,";
    }
    $sql.=" game JOIN game_category on GAM_CATEGORY=gct_name
        LEFT JOIN customer_result_daily ON gam_id = cus_res_gam_id
          where  cus_res_club_id = c.jur_id
          AND cur_id=c.jur_currency ";

    if($_SESSION['jurisdiction_class']=='nation'){
        $sql.=" AND c.jur_parent_id=d.jur_id
          AND d.jur_parent_id=r.jur_id
          AND r.jur_parent_id=n.jur_id
          And n.jur_id=". $_SESSION['jurisdiction_id'];
    }
    if($_SESSION['jurisdiction_class']=='region'){
        $sql.=" AND c.jur_parent_id=d.jur_id
           AND d.jur_parent_id=r.jur_id
           AND r.jur_id=". $_SESSION['jurisdiction_id'];
    }
    if($_SESSION['jurisdiction_class']=='district'){
        $sql.="AND c.jur_parent_id=d.jur_id
           AND d.jur_id=". $_SESSION['jurisdiction_id'];
    }
    if($_SESSION['jurisdiction_class']=='club'){
        $sql.="  AND c.jur_id=". $_SESSION['jurisdiction_id'];
    }

     $sql .=" AND cus_res_day Between '$date_start' and '$date_end'  $where
          group by gam_name,date_format(cus_res_day, '%Y-%m-%d'),c.jur_currency
          order by gam_name,date_format(cus_res_day, '%Y-%m-%d'),c.jur_currency";
    $resultGames = $dbh->exec($sql);
    $endtime = microtime(true);
    $duration = $endtime - $starttime;
    $duration=number_format($duration, 4, ',', '')."s";


    if($resultGames->getNumRows()>0){
    $gamesList = array();
    $lastGame = "";
    $totalBet = 0;
    $totalWin = 0;
    $playerTot = 0;
    $playedTot = 0;
    $totalRake = 0;

    $bettotal=0;
    $wintotal=0;
    $raketotal=0;
    $totalplayers=0;
    $totalPlayed=0;
    while ($resultGames->hasNext()) {
        $row = $resultGames->next();
        if ($lastGame != $row['gam_name']) {
            if ($lastGame != "") {
                $gamesList[$lastGame][$lastGame . "_total"] = array($totalBet, $totalWin, $playerTot,$totalRake,$playedTot);
                $lastGame = "";
                $totalBet = 0;
                $totalWin = 0;
                $totalRake = 0;
                $playerTot = 0;
                $playedTot = 0;
            }
            $lastGame = $row['gam_name'];
        }

        if($row["currency"]!=$_SESSION["currency_id"]){
            if($_SESSION["currency_id"]==1){
                $row['totalbet']=bcdiv($row['totalbet'],$_SESSION["currencies"][$row["currency"]]["value"],3);
                $row['totalwin']=bcdiv( $row['totalwin'],$_SESSION["currencies"][$row["currency"]]["value"],3);
                $row['tot_rake']=bcdiv( $row['tot_rake'],$_SESSION["currencies"][$row["currency"]]["value"],3);

            }
            else{
                $row['totalbet']=bcmul($row['totalbet'],$_SESSION["currencies"][$_SESSION["currency_id"]]["value"],3);
                $row['totalwin']=bcmul($row['totalwin'],$_SESSION["currencies"][$_SESSION["currency_id"]]["value"],3);
                $row['tot_rake']=bcmul($row['tot_rake'],$_SESSION["currencies"][$_SESSION["currency_id"]]["value"],3);
            }
        }

        $totalBet += $row['totalbet'];
        $totalWin += $row['totalwin'];
        $totalRake+= $row['tot_rake'];

        $bettotal += $row['totalbet'];
        $wintotal  += $row['totalwin'];
        $raketotal +=$row['tot_rake'];
        $playerTot += $row['players'];
        $playedTot += $row['cus_res_played'];
        $totalplayers += $row['players'];
        $totalPlayed += $row['cus_res_played'];
        if(!isset($gamesList[$row['gam_name']][$row['cus_day']])){
            $gamesList[$row['gam_name']][$row['cus_day']]=array();
        }
        $gamesList[$row['gam_name']][$row['cus_day']][0]+= $row['totalbet'];
        $gamesList[$row['gam_name']][$row['cus_day']][1]+= $row['totalwin'];
        $gamesList[$row['gam_name']][$row['cus_day']][2]+= $row['players'];
        $gamesList[$row['gam_name']][$row['cus_day']][3]+= $row['tot_rake'];
        $gamesList[$row['gam_name']][$row['cus_day']][4]+= $row['cus_res_played'];

        if (!$resultGames->hasNext()) {
            $gamesList[$lastGame][$lastGame . "_total"] = array($totalBet, $totalWin, $playerTot,$totalRake,$playedTot);
            $lastGame = "";
            $totalBet = 0;
            $totalWin = 0;
            $totalRake = 0;
            $playerTot = 0;
            $playedTot = 0;
        }
    }

    if ($gamesList != array()) {
        $objPHPExcel = new PHPExcel();
        $objPHPExcel->getProperties()->setCreator("CsLiveGames");
        $objPHPExcel->getProperties()->setLastModifiedBy("CsLiveGames");
        $objPHPExcel->getProperties()->setTitle("Games Reports to " . $date_start . " from " . $date_end);
        ?>
        <style>
            .odd td {
                background-color: #FFF;
            }
            .even td {
                background-color: #EEE;
            }
        </style>
    <div class="tip fleft"><?=$lang->getLang("Time taken to execute your request")?> : <?=$duration?></div>
        <div style="text-align:right;margin-right:150px;margin-bottom:10px;"><a
                href='<?= "../cache/" . $_SESSION["jurisdiction_class"] . "_" . $date_start . "_" . $date_end . "_games.xlsx" ?>'><img
                    src="<?= secure_host ?>/media/images/icon_download.jpg" width="16" height="16"/>
                Download excel file</a></div>
    <div>
        <table align="center" bgcolor="#a99a9" cellpadding="4" cellspacing="1" border="0" width="100%x">
            <tr valign="top" align="center">
                <th class="label"><?=$lang->getLang('Total bet')?></th>
                <th class="label"><?=$lang->getLang('Total win')?></th>
                <th class="label"><?=$lang->getLang('Rake')?></th>
                <th class="label"><?=$lang->getLang('Played')?></th>
                <th class="label"><?=$lang->getLang('Players')?></th>
                <th class="label">%</th>
            </tr>
            <tr class="content centered">
                <td><?=getInDollars($bettotal)?></td>
                <td><?=getInDollars($wintotal)?></td>
                <td><?=getInDollars($raketotal)?></td>
                <td><?=$totalPlayed?></td>
                <td><?=$totalplayers?></td>
                <td><?= round(($wintotal/ $bettotal*100), 2);?></td>
            </tr>


        </table>

    </div>
<br>


        <?php
        $row = 1;

        foreach ($gamesList as $key => $value) {
            $objPHPExcel->setActiveSheetIndex(0);
            $objPHPExcel->getActiveSheet()->SetCellValue('A' . $row, $key);
            $objPHPExcel->getActiveSheet()->getStyle('A' . $row)->getFont()->getColor()->setARGB(PHPExcel_Style_Color::COLOR_WHITE);
            $objPHPExcel->getActiveSheet()->getStyle('A' . $row)->getFill()->setFillType(PHPExcel_Style_Fill::FILL_SOLID)->getStartColor()->setARGB('FFFF0000');
            $row++;
            $objPHPExcel->getActiveSheet()->SetCellValue('A' . $row, 'DATE');
            $objPHPExcel->getActiveSheet()->SetCellValue('B' . $row, 'TOTAL BET');
            $objPHPExcel->getActiveSheet()->SetCellValue('C' . $row, 'TOTAL WIN');
            $objPHPExcel->getActiveSheet()->SetCellValue('G' . $row, '%');
            $objPHPExcel->getActiveSheet()->SetCellValue('E' . $row, 'RAKE');
            $objPHPExcel->getActiveSheet()->SetCellValue('D' . $row, 'PLAYED');
            $objPHPExcel->getActiveSheet()->SetCellValue('F' . $row, 'PLAYERS');
            $objPHPExcel->getActiveSheet()->getColumnDimension('A')->setAutoSize(true);
            $objPHPExcel->getActiveSheet()->getColumnDimension('B')->setAutoSize(true);
            $objPHPExcel->getActiveSheet()->getColumnDimension('C')->setAutoSize(true);
            $objPHPExcel->getActiveSheet()->getColumnDimension('D')->setAutoSize(true);
            $objPHPExcel->getActiveSheet()->getColumnDimension('G')->setAutoSize(true);
            $objPHPExcel->getActiveSheet()->getColumnDimension('E')->setAutoSize(true);
            $objPHPExcel->getActiveSheet()->getColumnDimension('F')->setAutoSize(true);
            $objPHPExcel->getActiveSheet()->getStyle('A' . $row . ':G' . $row)->getFont()->getColor()->setARGB(PHPExcel_Style_Color::COLOR_WHITE);
            $objPHPExcel->getActiveSheet()->getStyle('A' . $row . ':G' . $row)->getFill()->setFillType(PHPExcel_Style_Fill::FILL_SOLID)->getStartColor()->setARGB('FF0000DD');
            //$objPHPExcel->getActiveSheet()->getStyle('A1:G1')->getFill()->setFillStyle(PHPExcel_Style_Fill::FILL_SOLID);
            ?>

            <div class="bodyHD"><?=$key?></div>
            <table align="center" bgcolor="#a99a9" cellpadding="4" cellspacing="1" border="0" width="100%">
                <thead>
                <tr valign="top" align="center">
                    <th class="label"><?=$lang->getLang('DATE')?></th>
                    <th class="label"><?=$lang->getLang('TOTAL BET')?></th>
                    <th class="label"><?=$lang->getLang('TOTAL WIN')?></th>
                    <th class="label"><?=$lang->getLang('RAKE')?></th>
                    <th class="label"><?=$lang->getLang('PLAYED')?></th>
                    <th class="label"><?=$lang->getLang('PLAYERS')?></th>
                    <th class="label">%</th>
                </tr>
                <thead>
                <tbody>
                <?php
                foreach ($value as $date => $data) {
                    $row++;
                    $class = ($row % 2 == 0) ? "odd" : "even";
                    if (!strpos($date, 'total')) {
                        if(isset($_POST['date'])){

                        $objPHPExcel->getActiveSheet()->SetCellValue('A' . $row, $date);
                        $objPHPExcel->getActiveSheet()->SetCellValue('B' . $row, $data[0]);
                        $objPHPExcel->getActiveSheet()->SetCellValue('C' . $row, $data[1]);
                        $objPHPExcel->getActiveSheet()->SetCellValue('F' . $row, round($data[1] / $data[0], 2));
                        $objPHPExcel->getActiveSheet()->SetCellValue('D' . $row, ($data[4]));
                        $objPHPExcel->getActiveSheet()->SetCellValue('G' . $row, ($data[3]));
                        $objPHPExcel->getActiveSheet()->SetCellValue('E' . $row, $data[2]);
                        ?>
                        <tr class="<?= $class ?> centered">
                            <td><?=$date?></td>
                            <td><?=getInDollars($data[0])?></td>
                            <td><?=getInDollars($data[1])?></td>
                            <td><?=getInDollars($data[3])?></td>
                            <td><?=$data[4]?></td>
                            <td><?=$data[2]?></td>
                            <td><?= round($data[1] / $data[0]*100, 2);?></td>
                        </tr>
                    <?php
                    }
                    }
                        else {
                        $objPHPExcel->getActiveSheet()->SetCellValue('A' . $row, 'TOTALS');
                        $objPHPExcel->getActiveSheet()->getStyle('A' . $row)->getFont()->getColor()->setARGB(PHPExcel_Style_Color::COLOR_WHITE);
                        $objPHPExcel->getActiveSheet()->getStyle('A' . $row)->getFill()->setFillType(PHPExcel_Style_Fill::FILL_SOLID)->getStartColor()->setARGB('FF0000DD');
                        $objPHPExcel->getActiveSheet()->SetCellValue('B' . $row, $data[0]);
                        $objPHPExcel->getActiveSheet()->SetCellValue('C' . $row, $data[1]);
                        $objPHPExcel->getActiveSheet()->SetCellValue('F' . $row, round($data[1] / $data[0], 2));
                        $objPHPExcel->getActiveSheet()->SetCellValue('D' . $row, ($data[4]));
                        $objPHPExcel->getActiveSheet()->SetCellValue('G' . $row, ($data[3]));
                        $objPHPExcel->getActiveSheet()->SetCellValue('E' . $row, $data[2]);
                        ?>
                        <tr class="centered">
                            <td class="label">TOTALS</td>
                            <td class="content"><?=getInDollars($data[0])?></td>
                            <td class="content"><?=getInDollars($data[1])?></td>
                            <td class="content"><?=getInDollars($data[3])?></td>
                            <td class="content"><?=$data[4]?></td>
                            <td class="content"><?=$data[2]?></td>
                            <td class='content'><?=round(($data[1] / $data[0]*100), 2);?></td>
                        </tr>
                    <?php
                    }
                }
                ?>
                </tbody>
            </table>
            <br/>
            <div id="container_<?= $key ?>" style="min-width: 1100px; height: 450px; margin: 0 auto;width: 100%"></div>
            <script type="text/javascript">
                chart = new Highcharts.Chart({
                    chart: {
                        renderTo: 'container_<?=$key?>',
                        //type: 'spline',
                        zoomType: 'x'
                    },
                    title: {
                        text: '<?=$key?> Daily Average Rake',
                        x: -20 //center
                    },
                    subtitle: {
                        text: '',
                        x: -20
                    },
                    credits: {
                        text: ''
                    },
                    xAxis: {
                        //categories: days,
                        type: 'datetime',
                        dateTimeLabelFormats: { // don't display the dummy year
                            month: '%e. %b',
                            year: '%b'
                        }
                    },
                    yAxis: {
                        title: {
                            text: 'Rake'
                        },
                        plotLines: [
                            {
                                value: 0,
                                width: 1,
                                color: '#808080'
                            }
                        ]
                    },
                    tooltip: {
                        formatter: function () {
                            var s;
                            if (this.point.name) { // the pie chart
                                s = '' +
                                    this.point.name + ': ' + this.y;
                            } else {
                                s = '' +
                                    Highcharts.dateFormat('%e. %b', this.x) + ': ' + this.y;
                            }
                            return s;
                        }

                    },

                    series: [
                        {
                            name: "<?=$key?>",
                            type: 'spline',
                            data: [
                                <?php
                                   foreach($value as $d => $data){
                                        if(!strpos($d,'total')){
                                         list($year, $month, $day) = explode("-",$d);
                                        $month = $month - 1;
                                      ?>
                                [ Date.UTC(<?=$year?>, <?=$month?>, <?=$day?>), <?=($data[0]-$data[1])?>],
                                <?php
                              }
                            }
                        ?>
                            ]
                        },
                        {
                            name: "<?=$key?> Total",
                            type: 'pie',
                            data: [
                                <?php
                                  foreach($value as $d => $data){
                                       if(strpos($d,'total')){
                                       ?>
                                {
                                    name: 'Bet',
                                    y: <?=$data[0]?>,
                                    color: '#4572A7'
                                },
                                {
                                    name: 'Win',
                                    y: <?=$data[1]?>,
                                    color: '#AA4643'
                                },
                                <?php
                               }
                             }
                         ?>
                            ],
                            center: [50, 50],
                            size: 100,
                            showInLegend: false,
                            dataLabels: {
                                enabled: false
                            }
                        }

                    ]
                });
            </script>
            <br/>
            <?php
            $row += 2;
        }
        ?>
        <div class="bodyHD"><?=$lang->getLang("TOTAL GAMES COMPARE")?></div>
        <div id="container" style="min-width: 800px; height: 450px; margin: 0 auto"></div>
        <script type="text/javascript">
            chart = new Highcharts.Chart({
                chart: {
                    renderTo: 'container',
                    type: 'spline',
                    zoomType: 'x'
                },
                credits: {
                    text: ''
                },
                title: {
                    text: 'Games Daily Average Rake',
                    x: -20 //center
                },
                subtitle: {
                    text: '',
                    x: -20
                },
                xAxis: {
                    //categories: days,
                    type: 'datetime',
                    dateTimeLabelFormats: { // don't display the dummy year
                        month: '%e. %b',
                        year: '%b'
                    }
                },
                yAxis: {
                    title: {
                        text: 'Rake'
                    },
                    plotLines: [
                        {
                            value: 0,
                            width: 1,
                            color: '#808080'
                        }
                    ]
                },
                tooltip: {
                    formatter: function () {
                        return '<b>' + this.series.name + '</b><br/>' +
                            Highcharts.dateFormat('%e. %b', this.x) + ': ' + this.y;
                    }
                },

                series: [
                    <?php
                      foreach($gamesList as $name => $value){
                       ?>
                    {
                        name: "<?=$name?>",
                        data: [
                            <?php
                                 foreach($value as $date => $data){
                                     if(!strpos($date,'total')){
                                     list($year, $month, $day) = explode("-",$date);
                                     $month = $month - 1;
                                     $rake = $data[3];
                                 ?>
                            [ Date.UTC(<?=$year?>, <?=$month?>, <?=$day?>), <?=$rake?>],
                            <?php
                               }
                             }
                              ?>
                        ]
                    },
                    <?php
                   }
                   ?>
                ]
            });

        </script>
        <?php
        $rakes = array();
        foreach ($gamesList as $name => $value) {
            foreach ($value as $date => $data) {
                if (strpos($date, 'total')) {
                    $rakes[$name] = $data[3];
                }
            }
        }
        ?>
        <div id="cont_pie" style="min-width: 800px; height: 450px; margin: 0 auto"></div>
        <script type="text/javascript">
            chart = new Highcharts.Chart({
                chart: {
                    renderTo: 'cont_pie',
                    options3d: {
                        enabled: true,
                        alpha: 45,
                        beta: 0
                    }
                },

                credits: {
                    text: ''
                },
                title: {
                    text: 'Total Games Rake'
                },
                tooltip: {
                    formatter: function () {
                        return '<b>' + this.point.name + '</b>: ' + Math.round(this.percentage) + ' %';
                    }
                },
                plotOptions: {
                    pie: {
                        allowPointSelect: true,
                        cursor: 'pointer',
                        depth: 15,
                        dataLabels: {
                            enabled: true,
                            color: '#000000',
                            connectorColor: '#000000',
                            formatter: function () {
                                return '<b>' + this.point.name + '</b>: ' + Math.round(this.percentage) + ' %';
                            }
                        }


                    }
                },
                series: [
                    {
                        type: 'pie',
                        name: 'Rake',
                        pointWidth:15,
                        data: [
                            <?php
                             foreach($rakes as $name => $rake){
                                 if($rake < 0){
                                     $rake = 0;
                                 }
                          ?>
                            ['<?=$name?>', <?=$rake?>],
                            <?php
                            }
                           ?>

                        ]
                    }
                ]
            });
        </script>
        <?php
        $objPHPExcel->getActiveSheet()->setTitle('Reports');
        $objWriter = new PHPExcel_Writer_Excel2007($objPHPExcel);
        $objWriter->save("../cache/" . $_SESSION["jurisdiction_class"] . "_" . $date_start . "_" . $date_end . "_games.xlsx");
    }
}
else{
    addError("",$lang->getLang("No result found"));
    showErrors();
}
}
?>
