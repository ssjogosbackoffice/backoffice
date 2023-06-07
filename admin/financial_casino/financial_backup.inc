<script src="<?= secure_host ?>/media/highcharts.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/modules/exporting.js" type="text/javascript"></script>
<div id="customers">
<div id="cr_container">
</div><div id="cr"><button type="button" class="close" style="color:darkblue" onclick="hideCustomerDetails();">X</button><div id="cr_body"></div></div><?php
require_once "financial_casino/financial_functions.inc";
$jurisdiction_level = (isset($_POST['jurisdiction_level'])) ? $_POST['jurisdiction_level'] : getStartJurisdictionLevel();
$jur_groups_level = (isset($_POST['jur_level'])) ? $_POST['jur_level'] : getStartJurisdictionLevel();
$date_start = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d", time() - (60 * 60 * 24));
$date_end = (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d", time() - (60 * 60 * 24));
$row_to_page = (isset($_POST['resforpage'])) ? $_POST['resforpage'] : 20;
?>
<!-- Le funzioni javascript inizializzano il datapicker per la scelta della data di partenza e data di fine settando a massimo 2 mesi la ricerca-->
<script type="text/javascript">
    $(function() {
        $( "#date_start" ).datepicker({
            changeMonth: true,
            numberOfMonths:1,
            onSelect: function( selectedDate ) {
                $( "#date_end" ).datepicker( "option", "minDate", selectedDate );
            }
        });
        $( "#date_end" ).datepicker({
            changeMonth: true,
            numberOfMonths:1,
            onSelect: function( selectedDate ) {
                $( "#date_start" ).datepicker( "option", "maxDate", selectedDate );
            }
        });
    });

    function hideCustomerDetails()
    {
        $("#cr_container").hide('slow');
        $("#cr").hide('slow');
        $("#cr_body").html("");
    }

    function getCustomerDetails(c,ds,de,bw,f)
    {
        $("#cr_container").show('slow');
        $("#cr").show('slow');
        $("#cr_body").empty().append(' <b>Loading...   </b> <img src="media/images/ajax-loader.gif">');
        $.post("financial_casino/financial_customer_details.inc",{action:"1",club:c,date_start:ds,date_end:de,betwin:bw,financial:f}, function(data){
            $("#cr_body").empty().append( data );

        });
    }
</script>
<center>
    <div class="bodyHD">Financial Activity Summary</div>
    <br/>
    <p>
        <?php form_head()?>
        <?php printHiddenInput('doQuery', 'yes');?>
        <?php printHiddenInput('search_type', 'date_range');?>
    <table cellpadding="4" cellspacing="1" border="0">
        <tr valign=top>
            <td class=label>Start Date</td>
            <td class=content>
                <input type='text' name='date_start' id='date_start'  value="<?=$date_start?>" >
            </td>
        </tr>
        <tr valign=top>
            <td class=label>End Date</td>
            <td class=content>
                <input type='date_end' name='date_end' id='date_end' value="<?=$date_end?>">
            </td>
        </tr>
        <?php
        if ( $_SESSION['jurisdiction_class'] != 'club' ) {
            include (include_content('jurisdiction_filters.php'));
            include (include_content("financial_casino/jurisdiction_filter.php"));
        }
        else{
            $jur_groups_level='club';
        }
        ?>
        </tr>
        <tr valign=top>
            <td class=label>Details</td>
            <td class=content>
                <input type='checkbox' name='betwin' id='betwin' <?php if(isset($_POST['betwin'])) echo "checked"; ?> >Bet & Win</input>
                <input type='checkbox' name='financial' id='financial' <?php if(isset($_POST['financial'])) echo "checked"; ?> >Financial</input>
            </td>
        </tr>
        <tr>
            <td class="label">Result in Page</td>
            <td class="content"><select name="resforpage">
                    <option <?php if($row_to_page == 20){?> selected <?php }?>
                        value="20">20</option>
                    <option <?php if($row_to_page == 50){?> selected <?php }?>
                        value="50">50</option>
                    <option <?php if($row_to_page == 100){?> selected <?php }?>
                        value="100">100</option>
                </select></td>
        </tr>
        <tr>
            <td>&nbsp;</td>
            <td><input type =image src="<?=image_dir?>/button_submit.gif" name="submit"></td>
        </tr>
    </table>

</center>
<?php

if(isset($_POST['doQuery'])){

$date_start = $_POST["date_start"];
$date_end   = $_POST["date_end"];

$d1 = strtotime("$date_start+2 months");
$d2 = strtotime("$date_end");

if($d2-$d1>0){
    addError("","Please choose an interval less than 2 months");
    showErrors();
}
    if ( ! areErrors() )
{


$betwin = (isset($_POST['betwin'])) ? true : false;
$financial = (isset($_POST['financial'])) ? true : false;
$jur_sel_id = $_POST['jurisdiction'];
if(isset($_POST['submit_x'])) {
    $start_row = 1;
    unset($_POST['start_row']);
}
$starttime = microtime(true);
$res = getFinancialData($date_start, $date_end,$jur_groups_level, $betwin, $financial,$jurisdiction_level,$jur_sel_id); //Ritorna i dati del financial nel range delle date passate
$endtime = microtime(true);
$duration = $endtime - $starttime;
$duration=number_format($duration, 4, ',', '')."s";
//var_dump($res);
$cell_fmt_arr = array (
    'date'  => 'style="text-align:center;min-width:70px; "',
    'casino<br>bet'  => 'style="text-align:right; " class="content light"',
    'casino<br>win'   => 'style="text-align:right" class="content light"',
    'casino<br>rake'   => 'style="text-align:right" class="content light"',
    'casino<br>financial' => 'style="text-align:right" class="content light"',
    'casino<br>live bet' => 'style="text-align:right" class="content dark"',
    'casino<br>live win'     => 'style="text-align:right" class="content dark"',
    'casino<br>live<br>financial'      => 'style="text-align:right" class="content dark"',
    'casino<br>live rake' => 'style="text-align:right" class="content dark"',
    'virtual<br>bet'     => 'style="text-align:right" class="content light"',
    'virtual<br>financial'      => 'style="text-align:right" class="content light"',
    'virtual<br>rake' => 'style="text-align:right" class="content light"',
    'virtual<br>win'     => 'style="text-align:right" class="content light"',
    'poker<br>bet' => 'style="text-align:right" class="content dark"',
    'poker<br>win'     => 'style="text-align:right" class="content dark"',
    'poker<br>financial'      => 'style="text-align:right" class="content dark"',
    'poker<br>rake' => 'style="text-align:right" class="content dark"',
    'gaminator<br>bet' => 'style="text-align:right" class="content light"',
    'gaminator<br>win'     => 'style="text-align:right" class="content light"',
    'gaminator<br>financial'      => 'style="text-align:right" class="content light"',
    'gaminator<br>rake' => 'style="text-align:right" class="content light"',
    ' % ' => 'style="text-align:right" class="content dark"'
);
$val_fmt_arr = array (
    'date'        => 'return "<strong>" . date("Y-m-d",strtotime($rec["cus_res_day"])) . "<br>%</strong>";',
    'casino<br>bet'  => 'return preg_replace("/\&nbsp; /", "", getInDollars($rec["casino_bet"]));',
    'casino<br>win'  => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_win"]));',
    'casino<br>rake' => 'return "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_rake"]))."<br>".($rec["jps_perc_casino"]-$rec["jps_perc_casino2"])."% = ".preg_replace("/\&nbsp; /","",getInDollars($rec["casino_rake"]/100* ($rec["jps_perc_casino"]-$rec["jps_perc_casino2"])));',
    'casino<br>financial'  => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_fin"]));',
    'casino<br>live bet'   => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_live_bet"]));',
    'casino<br>live win'       => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_live_win"]));',
    'casino<br>live rake'      => 'return "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_live_rake"]))."<br>".($rec["jps_perc_casino_live"]-$rec["jps_perc_casino_live2"])."% = ".preg_replace("/\&nbsp; /","",getInDollars($rec["casino_live_rake"]/100* ($rec["jps_perc_casino_live"]-$rec["jps_perc_casino_live2"])));',
    'casino<br>live<br>financial'        => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_live_fin"]));',
    'virtual<br>bet'  => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["virtual_bet"]));',
    'virtual<br>win'  => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["virtual_win"]));',
    'virtual<br>rake'     => 'return "<strong>". preg_replace("/\&nbsp; /", "",getInDollars($rec["virtual_rake"]))."<br>".($rec["jps_perc_virtual"]-$rec["jps_perc_virtual2"])."% = ".preg_replace("/\&nbsp; /","",getInDollars($rec["virtual_rake"]/100* ($rec["jps_perc_virtual"]-$rec["jps_perc_virtual2"])));',
    'virtual<br>financial' => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["virtual_fin"]));',
    'poker<br>bet'     => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["poker_live_bet"]));',
    'poker<br>win'     => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["poker_live_win"]));',
    'poker<br>rake'     => 'return "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($rec["poker_live_rake"]))."<br>".($rec["jps_perc_poker_live"]-$rec["jps_perc_poker_live2"])."% = ".preg_replace("/\&nbsp; /","",getInDollars($rec["poker_live_rake"]/100* ($rec["jps_perc_poker_live"]-$rec["jps_perc_poker_live2"])));',
    'poker<br>financial'     => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["poker_live_fin"]));',
    'gaminator<br>bet'     => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["gaminator_bet"]));',
    'gaminator<br>win'     => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["gaminator_win"]));',
    'gaminator<br>rake'     => 'return "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($rec["gaminator_rake"]))."<br>".($rec["jps_perc_gaminator"]-$rec["jps_perc_gaminator2"])."% = ".preg_replace("/\&nbsp; /","",getInDollars($rec["gaminator_rake"]/100* ($rec["jps_perc_gaminator"]-$rec["jps_perc_gaminator2"])));',
    'gaminator<br>financial' => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["gaminator_fin"]));',
    ' % ' =>'return "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_rake"]+$rec["casino_live_rake"]+$rec["virtual_rake"]+$rec["poker_live_rake"]+$rec["gaminator_rake"]))."<br>% = ".preg_replace("/\&nbsp; /","",getInDollars($rec["casino_rake"]/100* ($rec["jps_perc_casino"]-$rec["jps_perc_casino2"])+ $rec["casino_live_rake"]/100* ($rec["jps_perc_casino_live"]-$rec["jps_perc_casino_live2"])+$rec["virtual_rake"]/100* ($rec["jps_perc_virtual"]-$rec["jps_perc_virtual2"])+$rec["poker_live_rake"]/100* ($rec["jps_perc_poker_live"]-$rec["jps_perc_poker_live2"])+$rec["gaminator_rake"]/100* ($rec["jps_perc_gaminator"]-$rec["jps_perc_gaminator2"])));'

);

$cols_arr = array('date');
array_push($cols_arr, 'nation');
$val_fmt_arr['nation']= 'return "<strong>" . $rec["n_jur_name"] . "</strong>";';
$chartGroup='n_jur_name';

if($jur_groups_level=='region')
{
    array_push($cols_arr, 'region');
    $val_fmt_arr['region']= 'return "<strong>" . $rec["r_jur_name"] . "</strong>";';
    $chartGroup='r_jur_name';
}
if($jur_groups_level=='district')
{
    array_push($cols_arr, 'region');
    array_push($cols_arr, 'district');
    $val_fmt_arr['region']= 'return "<strong>" . $rec["r_jur_name"] . "</strong>";';
    $val_fmt_arr['district']='return "<strong>" . $rec["d_jur_name"] . "</strong>";';
    $chartGroup='d_jur_name';
}
if($jur_groups_level=='club')
{
    array_push($cols_arr, 'region');
    array_push($cols_arr, 'district');
    array_push($cols_arr, 'club');
    $val_fmt_arr['district'] ='return "<strong>" . $rec["d_jur_name"] . "</strong>";';
    $val_fmt_arr['region'] = 'return "<strong>" . $rec["r_jur_name"] . "</strong>";';
    $val_fmt_arr['club'] = 'return "<strong><a href=javascript:getCustomerDetails(\'$rec[cus_res_club_id]\',\'$_POST[date_start]\',\'$_POST[date_end]\',\'$_POST[betwin]\',\'$_POST[financial]\');> $rec[c_jur_name] </a></strong>";';
    $chartGroup='c_jur_name';
}

if( $betwin)
{
    array_push($cols_arr, 'casino<br>bet');
    array_push($cols_arr, 'casino<br>win');
}
array_push($cols_arr,'casino<br>rake');
if($financial)
{
    array_push($cols_arr, 'casino<br>financial');
}
if( $betwin)
{
    array_push($cols_arr, 'casino<br>live bet');
    array_push($cols_arr, 'casino<br>live win');
}
array_push($cols_arr,'casino<br>live rake');
if( $financial)
{
    array_push($cols_arr, 'casino<br>live<br>financial');
}
if( $betwin)
{
    array_push($cols_arr, 'virtual<br>bet');
    array_push($cols_arr, 'virtual<br>win');
}
array_push($cols_arr,'virtual<br>rake');
if( $financial)
{
    array_push($cols_arr, 'virtual<br>financial');
}
if( $betwin)
{
    array_push($cols_arr, 'poker<br>bet');
    array_push($cols_arr, 'poker<br>win');
}
array_push($cols_arr,'poker<br>rake');
if( $financial)
{
    array_push($cols_arr,'poker<br>financial');
}

if( $betwin)
{
    array_push($cols_arr, 'gaminator<br>bet');
    array_push($cols_arr, 'gaminator<br>win');
}
array_push($cols_arr,'gaminator<br>rake');
if( isset($_POST['financial'])& $_POST['financial']==1)
{
    array_push($cols_arr,'gaminator<br>financial');
}
array_push($cols_arr,' % ');
$totals = array();
$totalsPerc = array();
while($res->hasNext()){
    $row = $res->next();
    $totalsPerc['casino_rake']+=(($row["casino_rake"]/100)* ($row["jps_perc_casino"]-$row["jps_perc_casino2"]));
    $totalsPerc['casino_live_rake']+=$row["casino_live_rake"]/100* ($row["jps_perc_casino_live"]-$row["jps_perc_casino_live2"]);
    $totalsPerc['virtual_rake']+=$row["virtual_rake"]/100* ($row["jps_perc_virtual"]-$row["jps_perc_virtual2"]);
    $totalsPerc['poker_live_rake']+=$row["poker_live_rake"]/100* ($row["jps_perc_poker_live"]-$row["jps_perc_poker_live2"]);
    $totalsPerc['gaminator_rake']+=$row["gaminator_rake"]/100* ($row["jps_perc_gaminator"]-$row["jps_perc_gaminator2"]);
    foreach($row as $key => $val){
        if(is_numeric($val)){
            $totals[$key] += $val;
        }
    }
}
?>
    <div class="tip fleft">Time took to execute your request: <?=$duration?></div>
<center>
<br>
<div class="bodyHD">Totals</div>
<table bgcolor="#a99a9" cellpadding="4" cellspacing="1">
    <tr>
        <?php
        if($betwin)
        {?>
            <td class="label centered">Bet</td>
            <td class="label centered">Win</td>
        <?php }
        ?>
        <td class="label centered">Rake</td>
        <?php
        if( $financial)
        {
            ?>
            <td class="label centered">Financial</td>
        <?php }?>
    </tr>
    <tr>
        <?php
        if( $betwin)
        {?>
            <td class="content" style="text-align: right"><?=getInDollars($totals["casino_bet"]+$totals["casino_live_bet"]+$totals["virtual_bet"]+$totals["poker_live_bet"]+$totals["gaminator_bet"])?></td>
            <td class="content" style="text-align: right"><?=getInDollars($totals["casino_win"]+$totals["casino_live_win"]+$totals["virtual_win"]+$totals["poker_live_win"]+$totals["gaminator_win"])?></td>
        <?php }
        ?>
        <td class="content" style="text-align: right"><?=getInDollars($totals["casino_rake"]+$totals["casino_live_rake"]+$totals["virtual_rake"]+$totals["poker_live_rake"]+$totals["gaminator_rake"])?><br>
            % = <?=getInDollars(array_sum($totalsPerc))?>

        </td>
        <?php
        if( $financial)
        {
            ?>
            <td class="content" style="text-align: right"><?=getInDollars($totals["casino_fin"]+$totals["casino_live_fin"]+$totals["virtual_fin"]+$totals["poker_live_fin"]+$totals["gaminator_fin"])?></td>
        <?php }?>
    </tr>
</table>

<br>
<div class="bodyHD">Partial Totals</div>
<table bgcolor="#a99a9" cellpadding="4" cellspacing="1">
    <tr>
        <?php
        if( $betwin)
        {?>
            <td class="label centered">Casino<br>Bet
            </td>
            <td class="label centered">Casino<br>Win
            </td>
        <?php }  ?>
        <td class="label centered">Casino<br>Rake
        </td>
        <?php
        if( $financial)
        {
            ?>
            <td class="label centered">Casino<br>Financial
            </td>
        <?php }
        if( $betwin)
        {?>
            <td class="label centered">Casino<br>Live Bet
            </td>
            <td class="label centered">Casino<br>Live Win
            </td>
        <?php }
        ?>
        <td class="label centered">Casino<br>Live Rake
        </td>
        <?php
        if( $financial)
        {
            ?>
            <td class="label centered">Casino<br>Live<br>Financial
            </td>
        <?php }
        if( $betwin)
        {?>
            <td class="label centered">Virtual<br>Bet
            </td>
            <td class="label centered">Virtual<br>Win
            </td>
        <?php }
        ?>
        <td class="label centered">Virtual<br>Rake
        </td>
        <?php
        if( $financial)
        {
            ?>
            <td class="label centered">Virtual<br>Financial
            </td>
        <?php }
        if( $betwin)
        {?>
            <td class="label centered">Poker<br>Bet
            </td>
            <td class="label centered">Poker<br>Win
            </td>
        <?php }
        ?>
        <td class="label centered">Poker<br>Rake
        </td>
        <?php
        if( $financial)
        {
            ?>
            <td class="label centered">Poker<br>Financial
            </td>
        <?php }
        if( $betwin)
        {?>
            <td class="label centered">Gaminator<br>Bet
            </td>
            <td class="label centered">Gaminator<br>Win
            </td>
        <?php }
        ?>
        <td class="label centered">Gaminator<br>Rake
        </td>
        <?php
        if( $financial)
        {
            ?>
            <td class="label centered">Gaminator<br>Financial
            </td>
        <?php }?>
    </tr>
    <tr class='odd'>
        <?php
        if( $betwin)
        {?>
            <td class="content light" style="text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["casino_bet"]))?></td>
            <td class="content light" style="text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["casino_win"]))?></td>
        <?php }?>
        <td class="content light" style="text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["casino_rake"]))?><br>
            % = <?=preg_replace("/\&nbsp; /", "",getInDollars($totalsPerc["casino_rake"]))?>
        </td>
        <?php
        if( $financial)
        {
            ?>
            <td class="content light" style="text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["casino_fin"]))?></td>
        <?php }
        if($betwin)
        {?>
            <td class="content dark" style="text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["casino_live_bet"]))?></td>
            <td class="content dark" style="text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["casino_live_win"]))?></td>
        <?php }?>
        <td class="content dark" style="text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["casino_live_rake"]))?><br>
            % = <?=preg_replace("/\&nbsp; /", "",getInDollars($totalsPerc["casino_live_rake"]))?>
        </td>
        <?php
        if($financial)
        {
            ?>
            <td class="content dark" style="text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["casino_live_fin"]))?></td>
        <?php }
        if($betwin)
        {?>
            <td class="content light" style="text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["virtual_bet"]))?></td>
            <td class="content light" style="text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["virtual_win"]))?></td>
        <?php }?>
        <td class="content light" style="text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["virtual_rake"]))?><br>
            % = <?=preg_replace("/\&nbsp; /", "",getInDollars($totalsPerc["virtual_rake"]))?>
        </td>
        <?php
        if($financial)
        {
            ?>
            <td class="content light" style="text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["virtual_fin"]))?></td>
        <?php }
        if($betwin)
        {?>
            <td class="content dark" style="text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["poker_live_bet"]))?></td>
            <td class="content dark" style="text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["poker_live_win"]))?></td>
        <?php }?>
        <td class="content" style="text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["poker_live_rake"]))?><br>
            % = <?=preg_replace("/\&nbsp; /", "",getInDollars($totalsPerc["poker_live_rake"]))?>

        </td>
        <?php
        if($financial)
        {
            ?>
            <td class="content dark" style="text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["poker_live_fin"]))?></td>
        <?php }
        if($betwin)
        {?>
            <td class="content light" style="text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["gaminator_bet"]))?></td>
            <td class="content light" style="text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["gaminator_win"]))?></td>
        <?php }?>
        <td class="content light" style="text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["gaminator_rake"]))?><br>
            % = <?=preg_replace("/\&nbsp; /", "",getInDollars($totalsPerc["gaminator_rake"]))?>
        </td>
        <?php
        if( $financial)
        {
            ?>
            <td class="content light" style="text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["gaminator_fin"]))?></td>
        <?php }?>
    </tr>
</table>
<br>


<?php

$start_row = (isset ( $_POST ['start_row'] )) ? $_POST ['start_row'] : 1;
$qp = new QueryPrinter($res);
$qp->dumpToExcelFile($res,$cols_arr,$val_fmt_arr,$excel_filename);
$qp->printTable($res,$cols_arr, $post_vars, $val_fmt_arr, $cell_fmt_arr, $sum_fmt_arr, $excel_filename, $header, $width="", $start_row, $row_to_page);
}
}
?>
</center>
<div id="graphs" style="width:100%; height:400px;"></div><br>
<?php
if($res->getNumRows() > 0){
include_once("classes/highcharts.class.php");
$totalRake=new highcharts('chart1', 'pie', 'Total Rakes', 'graphs');
$totalRake->prepareData($totals, "$chartGroup",array('casino_rake','casino_live_rake','poker_live_rake','virtual_rake','gaminator_rake'));
?>
<?=$totalRake->showChart();?>
<div style="width:100%;min-width:1400px;height:300px;">
    <div id="graphs_casino" style='float:left;width:30%;margin-left:5%;'></div>
    <div id="graphs_casino_live" style='float:left;width:30%;'></div>
    <div id="graphs_poker" style='float:left;width:30%;'></div>
</div>
<div>
    <div id="graphs_virtual" style='float:left;width:35%; margin-left:15%;'></div>
    <div id="graphs_gaminator" style='float:left;width:35%;'></div>
</div>
<?php
$casinoRake=new highcharts('chart2', 'pie', 'Casino Rakes', 'graphs_casino');
$casinoRake->prepareData($res->Records, "$chartGroup",'casino_rake');
?>
<?=$casinoRake->showChart('80');?>
<?php
$casinoLiveRake=new highcharts('chart3', 'pie', 'Casino Live Rakes', 'graphs_casino_live');
$casinoLiveRake->prepareData($res->Records, "$chartGroup",'casino_live_rake');
?>
<?=$casinoLiveRake->showChart('80');?>
<?php
$pokerLiveRake=new highcharts('chart4', 'pie', 'Poker Live Rakes', 'graphs_poker');
$pokerLiveRake->prepareData($res->Records, "$chartGroup",'poker_live_rake');
?>
<?=$pokerLiveRake->showChart('80');?>

<?php
$virtualRake=new highcharts('chart5', 'pie', 'Virtual Rakes', 'graphs_virtual');
$virtualRake->prepareData($res->Records, "$chartGroup",'virtual_rake');
?>
<?=$virtualRake->showChart('80');?>

<?php
$gaminatorRake=new highcharts('chart6', 'pie', 'Gaminator Rakes', 'graphs_gaminator');
$gaminatorRake->prepareData($res->Records, "$chartGroup",'gaminator_rake');
?>
<?=$gaminatorRake->showChart('80');?>
<?php
}
?>