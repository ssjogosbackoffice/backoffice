<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/dataTablesExtra/TableTools-2.2.0/css/dataTables.tableTools.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<script src="/media/jquery.dataTables.js" type="text/javascript"></script>
<script src="/media/jquery.alerts.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/highcharts.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/modules/exporting.js" type="text/javascript"></script>
<script src="/media/dataTablesExtra/TableTools-2.2.0/js/dataTables.tableTools.js" type="text/javascript"></script>


<div class="container" style="position: absolute;left:0;right: 0;width: 100%" >
<div id="customers">
<div id="cr_container"></div>
<div id="cr"><button type="button" class="close" style="color:darkblue" onclick="hideCustomerDetails();">X</button><div id="cr_body"></div></div><?php
require_once "financial_casino/financial_functions.inc";
$jurisdiction_level = (isset($_POST['jurisdiction_level'])) ? $_POST['jurisdiction_level'] : getStartJurisdictionLevel();
$jur_groups_level = (isset($_POST['jur_level'])) ? $_POST['jur_level'] : getStartJurisdictionLevel();
$date_start = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d", time() - (60 * 60 * 24));
$date_end = (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d", time() - (60 * 60 * 24));
$row_to_page = (isset($_POST['resforpage'])) ? $_POST['resforpage'] : 20;
$defaultmonth= (isset($_POST['monthHelper'])) ? $_POST['monthHelper'] : false;
$defaultweek= (isset($_POST['weekHelper'])) ? $_POST['weekHelper'] : false;
$casino= (!isset($_POST['form_submitted']) || $_POST['casino']==1) ? 1 : 0;
$casino_live= (!isset($_POST['form_submitted']) || $_POST['casino_live']==1) ? 1 : 0;
$virtual= (!isset($_POST['form_submitted']) || $_POST['virtual']==1) ? 1 : 0;
$pokerl= (!isset($_POST['form_submitted']) || $_POST['pokerl']==1) ? 1 : 0;
$poker2d= (!isset($_POST['form_submitted']) || $_POST['poker2d']==1) ? 1 : 0;
$betting= (!isset($_POST['form_submitted']) || $_POST['betting']==1) ? 1 : 0;
$bettingl= $betting;
$betting_jurisdictions=array('Betting<br>cost casino','Betting<br>cost nation','Betting<br>cost region','Betting<br>cost district');
$betting_jurisdictions_live=array('Betting live<br>cost casino','Betting live<br>cost nation','Betting live<br>cost region','Betting live<br>cost district');
function month_select_box($defaultmonth=false)
{
    global $lang;
    $month_options = '<option value="" >'.$lang->getLang("Select a month").'</option>';

    $curryear = date("Y");
    $firstyear=$curryear-3;
    for ($j = $firstyear; $j <= $curryear; $j++) {
        for ($i = 1; $i <= 12; $i++) {
            $month_num = str_pad($i, 2, 0, STR_PAD_LEFT);
            $month_name = date('F', mktime(0, 0, 0, $i + 1, 0, 0, 0));
            if((date('Y-m-d')) >= date('Y-m-t',strtotime($j.'-'.$month_num))) {
                $month_options .= '<option value="' . $j . '-' . ($month_num) . '" '.($defaultmonth==($j.'-'.$month_num)? 'selected':'').'  >' . $month_name . ' ' . $j . '</option>';
            }
        }

    }
    return '<select id="monthHelper" name="monthHelper">' . $month_options . '</select>';
}

function week_select_box($defaultweek=false)
{

    global $lang;
    $year           = date('Y');
    $firstDayOfYear = mktime(0, 0, 0, 1, 1, $year);
    $nextMonday     = strtotime('monday', $firstDayOfYear);
    $nextSunday     = strtotime('sunday', $nextMonday);
    $week_options = '<option value="" disabled>'.$lang->getLang("Select a week").'</option>';
    $i=1;
    while ( $nextMonday <= strtotime('now') ) {
        $week_options .= '<option value="'.date('Y-m-d', $nextMonday). '~'. date('Y-m-d', $nextSunday). '" '.($defaultweek==(date('Y-m-d', $nextMonday).'~'. date('Y-m-d', $nextSunday))? 'selected':'').' >'.$year.' '.$lang->getLang('Week').' # '.$i.', '.date('d M', $nextMonday).' - '. date('d M', $nextSunday).'</option>';
        $nextMonday = strtotime('+1 week', $nextMonday);
        $nextSunday = strtotime('+1 week', $nextSunday);
        $i++;
    }
    return '<select id="weekHelper" name="weekHelper" size="3">' . $week_options . '</select>';
}
?>
<script type="text/javascript">
    $(function() {

        $(".display").dataTable({'sPaginationType':"full_numbers","sDom": 'T<"clear">lfrtip',"oTableTools": {
            "aButtons": [
                "copy",
                "csv",
                "xls",
                "pdf"
            ]
        }
        });
        $( "#date_start" ).datepicker({
            changeMonth: true,
            changeYear: true,
            numberOfMonths:1,
            onSelect: function( selectedDate ) {
                $( "#date_end" ).datepicker( "option", "minDate", selectedDate );
                $( "#monthHelper, #weekHelper" ).val('');
            }
        });
        $( "#date_end" ).datepicker({
            changeMonth: true,
            changeYear: true,
            numberOfMonths:1,
            onSelect: function( selectedDate ) {
                $( "#date_start" ).datepicker( "option", "maxDate", selectedDate );
                $( "#monthHelper, #weekHelper" ).val('');
            }
        });

        $(".alignment .btn").click(function() {
            // whenever a button is clicked, set the hidden helper

            if(!$(this).hasClass('active')){
                $('#'+$(this).attr('data-send')).prop('disabled', false);
            }
            else{
                $('#'+$(this).attr('data-send')).prop('disabled', true);
            }
        });

    });

    $(document).on('change', '#monthHelper', function() {
        var d = new Date($(this).val());
        createMonthHelper(d);
    });
    $(document).on('change', '#weekHelper', function() {
        createWeekHelper($(this).val());
    });


    $(document).on('click', '.showYours', function() {
        var tr = $(this).closest('tr');
        $(tr).find('td .hidden').toggle();
        if ($(tr).find('td .hidden').is(":hidden")) {
            var src = $(tr).find('img').attr("src").replace("branch_collapse.gif", "branch_expand.gif");
            $(tr).find('img').attr("src", src);
        }
        else {
            var src = $(tr).find('img').attr("src").replace("branch_expand.gif", "branch_collapse.gif");
            $(tr).find('img').attr("src", src);
        }
    });

    function createWeekHelper(dateobj){
        dates=dateobj.split('~');
        firstDay=dates[0];
        lastDay=dates[1];
        $( "#date_start" ).datepicker('setDate', firstDay);
        $( "#date_end" ).datepicker('setDate', lastDay);
        $('#monthHelper').val('');
    }

    function createMonthHelper(dateobj){
        var firstDay = new Date(dateobj.getFullYear(), dateobj.getMonth(), 1);
        $( "#date_start" ).datepicker('setDate', firstDay);
        var lastDay = new Date(dateobj.getFullYear(), dateobj.getMonth() + 1, 0);
        $( "#date_end" ).datepicker('setDate', lastDay);
        $('#weekHelper').val('');
    }

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
        $.post("financial_casino/financial_customer_details.inc",{action:"1",club:c,date_start:ds,date_end:de,betwin:bw}, function(data){
            $("#cr_body").empty().append( data );
        });
    }


</script>
<center>
    <table class="table table-bordered">
        <tr>
            <td class="navbar-inner" colspan="2"><h3 class="text-center"><?=$lang->getLang("Financial Activity Summary")?></h3></td>
        </tr>
        <tr><td>
                <div class="well">
                    <br/>
                    <p>
                        <?php
                        /**
                         * @author Marian
                         */
                        form_head()?>
                        <?php printHiddenInput('doQuery', 'yes');?>
                        <?php printHiddenInput('search_type', 'date_range');?>
                    <table class="table" style="background-color: transparent">
                        <tr>
                            <td style="border: 0px;">
                                <div><div class='label label-inverse'><?=$lang->getLang("Choose month")?></div>
                                    <br>
                                    <div class="controls">
                                        <div class="input-prepend">
                                            <span class="add-on"><i class="icon-calendar"></i></span>
                                            <?=month_select_box($defaultmonth)?>
                                        </div>
                                    </div>
                                </div><br />
                                <div><div class='label label-inverse'><?=$lang->getLang("Choose week")?></div>
                                    <br>
                                    <div class="controls">
                                        <div class="input-prepend">
                                            <span class="add-on"><i class="icon-calendar"></i></span>
                                            <?=week_select_box($defaultweek)?>
                                        </div>
                                    </div>
                                </div>

                            </td>
                            <td style="border: 0px;">
                                <div><div class='label label-inverse'><?=$lang->getLang("From")?></div>
                                    <br>
                                    <div class="controls">
                                        <div class="input-prepend">
                                            <span class="add-on"><i class="icon-calendar"></i></span>
                                            <input type='text' name='date_start' id='date_start'  value="<?=$date_start?>" >
                                        </div>
                                    </div>
                                </div><br />
                                <div><div class='label label-inverse'><?=$lang->getLang("Until")?></div>
                                    <br>
                                    <div class="controls">
                                        <div class="input-prepend">
                                            <span class="add-on"><i class="icon-calendar"></i></span>
                                            <input type='text' name='date_end' id='date_end' value="<?=$date_end?>">
                                        </div>
                                    </div>
                                </div>
                            </td>
                            <td style="border: 0px;">

                                <?php
                                if ( $_SESSION['jurisdiction_class'] != 'club' ) {
                                    include ('jurisdiction_filters.php');
                                    include ("financial_casino/jurisdiction_filter.php");
                                }
                                else{
                                    $jur_groups_level='club';
                                }
                                ?>
                            </td>
                            <td style="border: 0px;">
                                <div><div class='label label-inverse'><?=$lang->getLang("Details")?></div>
                                    <br>
                                    <div class="btn-group alignment" data-toggle="buttons-checkbox">

                                        <button type="button" data-send="betwin" class="btn btn-primary  <?php if(isset($_POST['betwin'])) echo "active"; ?> " ><?=$lang->getLang("Bet & Win")?></button>
                                        <input type='hidden' name='betwin' id='betwin' <?=(isset($_POST['betwin'])? "" : "disabled") ?>   />
                                        <input type='hidden' name='financial' id='financial' <?=(isset($_POST['financial'])? "" : "disabled") ?> />
                                        <button type="button" data-send="financial" class="btn btn-primary " <?php if(isset($_POST['financial'])) echo "active"; ?> ><?=$lang->getLang("Financial")?></button>

                                    </div>
                                </div><br />

                                <div><div class='label label-inverse'><?=$lang->getLang("Financial contains")?></div>
                                    <br>
                                    <div class="btn-group alignment" data-toggle="buttons-checkbox">
                                        <button type="button" data-send="casino"  class="btn btn-primary <?php if($casino==1) echo "active"; ?>"  ><?=$lang->getLang("Casino")?></button>
                                        <input type='hidden'  name='casino' value="1"  id="casino" <?=($casino==1 ? "" : "disabled") ?>  />

                                        <button type="button" data-send="casino_live" class="btn btn-primary <?php if($casino_live==1) echo "active"; ?>" ><?=$lang->getLang("Casino Live")?></button>
                                        <input type='hidden' name='casino_live' value="1" id="casino_live" <?=($casino_live==1 ? "" : "disabled") ?>  />

                                        <button type="button" data-send="virtual" class="btn btn-primary <?php if($virtual==1) echo "active"; ?>" ><?=$lang->getLang("Virtual")?></button>
                                        <input type='hidden' name='virtual' value="1" id="virtual" <?=($virtual==1 ? "" : "disabled") ?>  />

                                        <button type="button" data-send="pokerl" class="btn btn-primary <?php if($pokerl==1) echo "active"; ?>" ><?=$lang->getLang("Poker Live")?></button>
                                        <input type='hidden' name='pokerl' value="1" id="pokerl" <?=($pokerl==1 ? "" : "disabled") ?> />

                                        <button type="button" data-send="poker2d" class="btn btn-primary  <?php if($poker2d==1) echo "active"; ?>" ><?=$lang->getLang("Poker 2d")?></button>
                                        <input type='hidden' name='poker2d' value="1"  id="poker2d" <?=($poker2d==1 ? "" : "disabled") ?> />

                                        <input type='hidden' name='betting' value="1" id="betting" <?=($betting==1 ? "" : "disabled") ?> />
                                        <button type="button" data-send="betting" class="btn btn-primary <?php if($betting==1) echo "active"; ?>" ><?=$lang->getLang("Betting & Betting Live ")?></button>

                                    </div>
                                </div>

                            </td>
                        </tr>
                    </table>
                    <input type="submit" class="btn btn-primary" value="Submit" >
                    <div class="clearfix"></div>
                </div>
</center>

<?php

if(isset($_POST['doQuery'])){


    if($jur_groups_level=='club'){
        $betting_jurisdictions=array_slice($betting_jurisdictions,array_search('Betting<br>cost '.$_SESSION['jurisdiction_class'],$betting_jurisdictions)+1);
        $betting_jurisdictions_live=array_slice($betting_jurisdictions_live,array_search('Betting live<br>cost '.$_SESSION['jurisdiction_class'],$betting_jurisdictions_live)+1);
    }
    else{
        $betting_jurisdictions=array_slice($betting_jurisdictions,array_search('Betting<br>cost '.$_SESSION['jurisdiction_class'],$betting_jurisdictions)+1,array_search('Betting<br>cost '.$jur_groups_level,$betting_jurisdictions));
        $betting_jurisdictions_live=array_slice($betting_jurisdictions_live,array_search('Betting live<br>cost '.$_SESSION['jurisdiction_class'],$betting_jurisdictions_live)+1,array_search('Betting live<br>cost '.$jur_groups_level,$betting_jurisdictions_live));
    }

    if($_SESSION['jurisdiction_class']=="club"){
        $betting_jurisdictions=array();
        $betting_jurisdictions_live=array();
    }

    $date_start = $_POST["date_start"];
    $date_end   = $_POST["date_end"];

    $d1 = strtotime("$date_start+2 months");
    $d2 = strtotime("$date_end");

    if($d2-$d1<0){
        addError("",$lang->getLang("Please choose an interval less than 2 months"));
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
        $res = getFinancialData($date_start, $date_end,$jur_groups_level, $betwin, $financial,$jurisdiction_level,$jur_sel_id); //Returns the financial details for the period passed
        $endtime = microtime(true);
        $duration = $endtime - $starttime;
        $duration=number_format($duration, 4, ',', '')."s";

        $totals = array();
        $totalsPerc = array();
        $totalsPercBetting = array();
        while($res->hasNext()){
            $row = $res->next();
            if($casino==1){
                $totalsPerc['casino_rake']+=(($row["casino_rake"]/100)* ($row["jps_perc_casino"]));
            }
            if($casino_live==1){
                $totalsPerc['casino_live_rake']+=$row["casino_live_rake"]/100* ($row["jps_perc_casino_live"]);
            }
            if($virtual==1){
                $totalsPerc['virtual_rake']+=$row["virtual_rake"]/100* ($row["jps_perc_virtual"]);
            }
            if($pokerl){
                $totalsPerc['poker_live_rake']+=$row["poker_live_rake"]/100* ($row["jps_perc_poker_live"]);
            }
            if($poker2d){
                $totalsPerc['poker_2d_rake']+=$row["poker_2d_rake"]/100* ($row["jps_perc_poker_2d"]);
            }
            if($betting){
                $totalsPercBetting['betting_rake']+=calculateRowBetting($row);
            }
            if($bettingl){
                $totalsPercBetting['betting_rake_live']+=calculateRowBettingLive($row);
            }
            $totals['nation']+=calculateBettingRake($row,'nation',1);
            $totals['region']+=calculateBettingRake($row,'region',1);
            $totals['district']+=calculateBettingRake($row,'district',1);
            $totals['nation_live']+=calculateLiveBettingRake($row,'nation',1);
            $totals['region_live']+=calculateLiveBettingRake($row,'region',1);
            $totals['district_live']+=calculateLiveBettingRake($row,'district',1);
            foreach($row as $key => $val){
                if(is_numeric($val)){
                    $totals[$key] += $val;
                }
            }

        }
        $totalsRakeSum=0;
        $totalsBetSum=0;
        $totalsWinSum=0;
        $totalsRakeSumBetting=0;
        $totalsBetSumBetting=0;
        $totalsWinSumBetting=0;
        if($casino==1){
            $totalsRakeSum +=$totals['casino_rake'];
            $totalsBetSum +=$totals['casino_bet'];
            $totalsWinSum +=$totals['casino_win'];
        }
        if($casino_live==1){
            $totalsRakeSum +=$totals['casino_live_rake'];
            $totalsBetSum +=$totals['casino_live_bet_bet'];
            $totalsWinSum +=$totals['casino_live_bet_win'];
        }
        if($virtual==1){
            $totalsRakeSum +=$totals['virtual_rake'];
            $totalsBetSum +=$totals['virtual_bet'];
            $totalsWinSum +=$totals['virtual_win'];
        }
        if($pokerl==1){
            $totalsRakeSum +=$totals['poker_live_rake'];
            $totalsBetSum +=$totals['poker_live_bet'];
            $totalsWinSum +=$totals['poker_live_win'];
        }
        if($poker2d==1){
            $totalsRakeSum +=$totals['poker_2d_rake'];
            $totalsBetSum +=$totals['poker_2d_bet'];
            $totalsWinSum +=$totals['poker_2d_win'];
        }
        if($betting==1){
            $totalsRakeSumBetting +=($totals['betting_bet']-$totals['betting_win']);
            $totalsBetSumBetting +=$totals['betting_bet'];
            $totalsWinSumBetting +=$totals['betting_win'];
        }
        if($bettingl==1){
            $totalsRakeSumBetting +=($totals['betting_bet_live']-$totals['betting_win_live']);
            $totalsBetSumBetting +=$totals['betting_bet_live'];
            $totalsWinSumBetting +=$totals['betting_win_live'];
        }
        ?>
        <div class="tip fleft"><?=$lang->getLang("Time took to execute your request")?>: <?=$duration?></div>
        <center>
        <br /><br />
        <table class="table table-bordered" style="margin:auto;width: auto;">
            <tr><td colspan="19" class="badge-inverse"><div class='label label-inverse'><?=$lang->getLang("Total")?></div></td> </tr>
            <tr>
                <td class='centered alert alert-success'><?=$lang->getLang("Closure")?></td>
                <td class='centered alert alert-success'>Financial</td>
                <?php
                if($betwin)
                {
                    ?>
                    <td class='centered alert alert-success'>Bet</td>
                    <td class='centered alert alert-success'>Win</td>
                <?php
                }
                ?>

                <td class='centered alert alert-success'>Total Perc.</td>
                <td class='centered alert alert-success'>Net</td>
            </tr>
            <tr>
                <td><a href="?page=reports_casino/searchclosure&startdate=<?=$date_start?>&enddate=<?=$date_end?>">View closure</a></td>
                <td style="text-align: right"><?=getInDollars($totals["fin"])?></td>
                <?php
                if($betwin)
                {
                    ?>
                    <td style="text-align: right">
                        <?=getInDollars($totalsBetSum)?></td>
                    <td style="text-align: right">
                        <?=getInDollars($totalsWinSum)?></td>
                <?php
                }
                ?>

                <td style="text-align: right">
                    <?=getInDollars($totalsRakeSum)?><br>
                    % = <?=getInDollars(array_sum($totalsPerc))?>
                </td>
                <td style="text-align: right">
                    <?=getInDollars($totals["fin"]-(abs(array_sum($totalsPerc))) )?><br>
                </td>
            </tr>
        </table>

        <br /><br /><br /><br />
        <table class="table table-bordered" style="margin:auto;width: auto;">
        <tr><td colspan="25" class="badge-inverse"><div class='label label-inverse'><?=$lang->getLang("Partial Totals")?></div></td> </tr>
        <tr>
            <td class="centered alert alert-info">Financial </td>
            <?php
            if($casino==1){
                if($betwin)
                {?>
                    <td class="centered alert alert-success">Casino<br>Bet
                    </td>
                    <td class="centered alert alert-success">Casino<br>Win
                    </td>
                <?php }  ?>
                <td class="centered alert alert-success">Casino<br>Net
                </td>
                <?if($financial)
                {?>
                    <td class="centered alert alert-success">Casino<br>Fin</td>
                <?php } } ?>

            <?php if($casino_live==1){
                if($betwin)
                {?>
                    <td class="centered alert-info">Casino<br>Live Bet
                    </td>
                    <td class="centered alert-info">Casino<br>Live Win
                    </td>
                <?php }
                ?>
                <td class="centered alert-info">Casino<br>Live Net
                </td>
                <?if($financial)
                {?>
                    <td class="centered alert alert-info">Casino<br>Live Fin</td>
                <?php } }  ?>
            <?php if($virtual==1){
                if( $betwin)
                {?>
                    <td class="centered alert alert-success">Virtual<br>Bet
                    </td>
                    <td class="centered alert alert-success">Virtual<br>Win
                    </td>
                <?php }
                ?>
                <td class="centered alert alert-success">Virtual<br>Net
                </td>
                <?if($financial)
                { ?>
                    <td class="centered alert alert-success">Virtual<br>Fin</td>
                <?php } } ?>
            <?php if($pokerl==1){
                if($betwin)
                { ?>
                    <td class="centered alert alert-info">Poker Live<br>Bet
                    </td>
                    <td class="centered alert alert-info">Poker Live<br>Win
                    </td>
                <?php }
                ?>
                <td class="centered alert alert-info">Poker Live<br>Rake
                </td>
                <?if($financial)
                { ?>
                    <td class="centered alert alert-info">Poker Live<br>Fin</td>
                <?php
                } }
            if($poker2d==1){
                if($betwin)
                {?>
                    <td class="centered alert alert-success">Poker 2d<br>Bet
                    </td>
                    <td class="centered alert alert-success">Poker 2d<br>Win
                    </td>
                <?php }
                ?>
                <td class="centered alert alert-success">Poker 2d<br>Rake
                </td>
                <?if($financial)
                { ?>
                    <td class="centered alert alert-success">Poker 2d<br>Fin</td>
                <?php
                }
            }
            if($betting==1){
                if($betwin)
                { ?>
                    <td class="centered alert alert-info">Betting<br>Bet
                    </td>
                    <td class="centered alert alert-info">Betting<br>Win
                    </td>
                <?php }

                ?>
                <td class="centered alert alert-info">Betting<br>Net</td>
                <td class="centered alert alert-info">Betting<br>Cost Club</td>
                <?if($jur_groups_level=='nation') {?>
                    <td class="centered alert alert-info">Betting<br>Cost Nation</td>
                <? } elseif($jur_groups_level=='region') {?>
                    <?if($_SESSION['jurisdiction_class']=='casino'){ ?>
                        <td class="centered alert alert-info">Betting<br>Cost Nation</td>
                    <? } ?>
                    <td class="centered alert alert-info">Betting<br>Cost Region</td>
                <? } elseif($jur_groups_level=='district') {?>
                    <?if($_SESSION['jurisdiction_class']=='casino'){ ?>
                        <td class="centered alert alert-info">Betting<br>Cost Nation</td>
                    <? } ?>
                    <?if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction_class']=='nation'){ ?>
                        <td class="centered alert alert-info">Betting<br>Cost Region</td>
                    <? } ?>
                    <td class="centered alert alert-info">Betting<br>Cost District</td>
                <? } elseif($jur_groups_level=='club') {?>
                    <?if($_SESSION['jurisdiction_class']=='casino'){ ?>
                        <td class="centered alert alert-info">Betting<br>Cost Nation</td>
                    <? } ?>
                    <?if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction_class']=='nation'){ ?>
                        <td class="centered alert alert-info">Betting<br>Cost Region</td>
                    <? } if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction_class']=='nation' || $_SESSION['jurisdiction_class']=='region'  ){ ?>
                        <td class="centered alert alert-info">Betting<br>Cost District</td>
                    <? } } ?>

                <?if($financial)
                { ?>
                    <td class="centered alert alert-info">Betting<br>Fin</td>
                <?php }
            } ?>

            <?
            if($bettingl==1){
                if($betwin)
                {?>
                    <td class="centered alert alert-success">Betting live<br>Bet
                    </td>
                    <td class="centered alert alert-success">Betting live<br>Win
                    </td>
                <?php }
                ?>
                <td class="centered alert alert-success">Betting live<br>Club Net</td>
                <td class="centered alert alert-success">Betting live<br>Cost Club</td>
                <?if($jur_groups_level=='nation') {?>
                    <td class="centered alert alert-success">Betting live<br>Cost Nation</td>
                <? } elseif($jur_groups_level=='region') {?>
                    <?if($_SESSION['jurisdiction_class']=='casino'){ ?>
                        <td class="centered alert alert-success">Betting live<br>Cost Nation</td>
                    <? } ?>
                    <td class="centered alert alert-success">Betting live<br>Cost Region</td>
                <? } elseif($jur_groups_level=='district') {?>
                    <?if($_SESSION['jurisdiction_class']=='casino'){ ?>
                        <td class="centered alert alert-success">Betting live<br>Cost Nation</td>
                    <? } ?>
                    <?if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction_class']=='nation'){ ?>
                        <td class="centered alert alert-success">Betting live<br>Cost Region</td>
                    <? } ?>
                    <td class="centered alert alert-success">Betting<br>Cost District</td>
                <? } elseif($jur_groups_level=='club') {?>
                    <?if($_SESSION['jurisdiction_class']=='casino'){ ?>
                        <td class="centered alert alert-success">Betting live<br>Cost Nation</td>
                    <? } ?>
                    <?if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction_class']=='nation'){ ?>
                        <td class="centered alert alert-success">Betting live<br>Cost Region</td>
                    <? } if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction_class']=='nation' || $_SESSION['jurisdiction_class']=='region'  ){ ?>
                        <td class="centered alert alert-success">Betting live<br>Cost District</td>
                    <? } } ?>
                <?if($financial)
                { ?>
                    <td class="centered alert alert-success">Betting live<br>Fin</td>
                <?php }  } ?>
            <td class="centered alert alert-danger">Total Perc.</td>
            <td class="centered alert alert-danger">Net</td>
            <? if($bettingl==1){ ?>
                <td class="centered alert alert-danger">Total Net Betting</td>
            <? } ?>
        </tr>
        <tr class='odd' style="vertical-align: top">
            <td  style="text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["fin"]))?></td>
            <?php
            if($casino==1){
                if($betwin)
                {?>
                    <td  style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["casino_bet"]))?></strong></td>
                    <td  style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["casino_win"]))?></strong></td>
                <?php }?>
                <td  style="text-align: right">
                    <?=($casino!=1? "<span class=tip>Excluded</span>" : "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($totals["casino_rake"])) ."<br>
                % =".preg_replace("/\&nbsp; /", "",getInDollars($totalsPerc["casino_rake"])))?>
                </td>
                <?if($financial)
                {?>
                    <td  style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["casino_fin"]))?></td>
                <?php } } ?>
            <?php if($casino_live==1){
                if($betwin)
                {?>
                    <td  style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["casino_live_bet"]))?></td>
                    <td  style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["casino_live_win"]))?></td>
                <?php }?>
                <td  style="text-align: right">
                    <?=($casino_live!=1? "<span class=tip>Excluded</span>" : "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($totals["casino_live_rake"]))."<br>
                % =".preg_replace("/\&nbsp; /", "",getInDollars($totalsPerc["casino_live_rake"])))?>
                </td>
                <?if($financial)
                {?>
                    <td  style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["casino_live_fin"]))?></td>
                <?php } } ?>
            <?php if($virtual==1){
                if($betwin)
                {?>
                    <td  style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["virtual_bet"]))?></td>
                    <td  style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["virtual_win"]))?></td>
                <?php }?>
                <td  style="text-align: right">
                    <?=($virtual!=1? "<span class=tip>Excluded</span>" : "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($totals["virtual_rake"]))."<br>
                % =".preg_replace("/\&nbsp; /", "",getInDollars($totalsPerc["virtual_rake"])))?>
                </td>
                <?if( $financial)
                {?>
                    <td style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["virtual_fin"]))?></td>
                <?php } }?>
            <?php if($pokerl==1){
                if($betwin)
                { ?>
                    <td style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["poker_live_bet"]))?></td>
                    <td style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["poker_live_win"]))?></td>
                <?php }?>
                <td style="text-align: right">
                    <?=($pokerl!=1? "<span class=tip>Excluded</span>" : "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($totals["poker_live_rake"]))."<br>
                % = ".preg_replace("/\&nbsp; /", "",getInDollars($totalsPerc["poker_live_rake"])))?>
                </td>
                <?if($financial)
                {?>
                    <td style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["poker_live_fin"]))?></td>
                <?php } }
            if($poker2d==1){

                if($betwin)
                {?>
                    <td style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["poker_2d_bet"]))?></td>
                    <td style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["poker_2d_win"]))?></td>
                <?php }?>
                <td style="text-align: right">
                    <?=($poker2d!=1? "<span class=tip>Excluded</span>" : "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($totals["poker_2d_rake"]))."<br>
                % = ".preg_replace("/\&nbsp; /", "",getInDollars($totalsPerc["poker_2d_rake"])))?>
                </td>
                <?if($financial)
                {?>
                    <td style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["poker_2d_fin"]))?></td>
                <?php } }
            if($betting==1){
                if($betwin)
                {?>
                    <td style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["betting_bet"]))?></td>
                    <td style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["betting_win"]))?></td>
                <?php }
                ?>
                <td style="text-align: right"><strong>
                    <?=preg_replace("/\&nbsp; /", "",getInDollars($totals["betting_bet"]- $totals["betting_win"]))?></td>
                <td style="text-align: right"><strong><br />
                        % = <?=preg_replace("/\&nbsp; /", "",getInDollars($totals["betting_rake"]))?></td>
                <?if($jur_groups_level=='nation') {?>
                    <td style="text-align: right"><strong><br />% = <?=getInDollars($totals['nation'])?></td>
                <? } elseif($jur_groups_level=='region') {?>
                    <?if($_SESSION['jurisdiction_class']=='casino') { ?>
                        <td class="tip" style="text-align: right"><br />% = <?=getInDollars($totals['nation'])?></td>
                    <? } ?>
                    <td  style="text-align: right"><strong><br />% = <?=getInDollars($totals['region'])?></td>
                <? } elseif($jur_groups_level=='district') {?>
                    <?if($_SESSION['jurisdiction_class']=='casino') { ?>
                        <td class="tip" style="text-align: right"><br />% = <?=getInDollars($totals['nation'])?></td>
                    <? } ?>
                    <?if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction']=='nation') { ?>
                        <td class="tip" style="text-align: right"><br />% = <?=getInDollars($totals['region'])?></td>
                    <? } ?>
                    <td  style="text-align: right"><strong><br />% = <?=getInDollars($totals['district'])?></td>
                <? } elseif($jur_groups_level=='club') {?>
                    <?if($_SESSION['jurisdiction_class']=='casino') { ?>
                        <td class="tip" style="text-align: right"><br />% = <?=getInDollars($totals['nation'])?></td>
                    <? } ?>
                    <?if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction']=='nation') { ?>
                        <td class="tip" style="text-align: right"><br />% = <?=getInDollars($totals['region'])?></td>
                    <? } if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction']=='nation' || $_SESSION['jurisdiction']=='region') {?>
                        <td style="text-align: right"><strong><br />% = <?=getInDollars($totals['district'])?></td>
                    <? } }?>

                <?if($financial)
                { ?>
                    <td style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["betting_fin"]))?></td>
                <?php } } ?>
            <?
            if($bettingl==1){
                if($betwin)
                {?>
                    <td  style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["betting_bet_live"]))?></td>
                    <td  style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["betting_win_live"]))?></td>
                <?php }
                ?>
                <td  style="text-align: right"><strong>
                    <?=preg_replace("/\&nbsp; /", "",getInDollars($totals["betting_bet_live"]- $totals["betting_win_live"]))?></td>
                <td  style="text-align: right"><strong><br />% =
                    <?=preg_replace("/\&nbsp; /", "",getInDollars($totals["betting_rake_live"]))?></td>
                <?if($jur_groups_level=='nation') {?>
                    <td  style="text-align: right"><strong><br />% = <?=getInDollars($totals['nation_live'])?></td>
                <? } elseif($jur_groups_level=='region') {?>
                    <?if($_SESSION['jurisdiction_class']=='casino') { ?>
                        <td class="tip" style="text-align: right"><br />% = <?=getInDollars($totals['nation_live'])?></td>
                    <? } ?>
                    <td style="text-align: right"><strong><br />% = <?=getInDollars($totals['region_live'])?></td>
                <? } elseif($jur_groups_level=='district') {?>
                    <?if($_SESSION['jurisdiction_class']=='casino') { ?>
                        <td class="tip" style="text-align: right"><br />% = <?=getInDollars($totals['nation_live'])?></td>
                    <? } ?>
                    <?if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction']=='nation') { ?>
                        <td class="tip" style="text-align: right"><br />% = <?=getInDollars($totals['region_live'])?></td>
                    <? } ?>
                    <td  style="text-align: right"><strong><br />% = <?=getInDollars($totals['district_live'])?></td>
                <? } elseif($jur_groups_level=='club') {?>
                    <?if($_SESSION['jurisdiction_class']=='casino') { ?>
                        <td class="tip" style="text-align: right"><br />% = <?=getInDollars($totals['nation_live'])?></td>
                    <? } ?>
                    <?if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction']=='nation') { ?>
                        <td class="tip" style="text-align: right"><br />% = <?=getInDollars($totals['region_live'])?></td>
                    <? } if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction']=='nation' || $_SESSION['jurisdiction']=='region_live') {?>
                        <td style="text-align: right"><strong><br />% = <?=getInDollars($totals['district_live'])?></td>
                    <? } }?>
                <?if($financial)
                {?>
                    <td style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["betting_fin_live"]))?></td>
                <?php } } ?>
            <td style="text-align: right" ><strong><?=getInDollars($totalsRakeSum)?><br>
                    % = <?=getInDollars(array_sum($totalsPerc))?>
            </td>
            <td  style="text-align: right" ><strong><?=getInDollars($totals['fin']-abs(array_sum($totalsPerc)) )?></td>
            <?if($betting==1){ ?>
                <td class="centered"><strong><?=getInDollars((array_sum($totalsPercBetting)))?></strong></td>
            <? } ?>
        </tr>
        </table>
        <br /><br /><br /> </center>
        <table class="table display table-bordered">
        <thead>
        <tr>
            <th style="white-space: nowrap;">Nation</th>
            <?php
            $chartGroup='n_jur_name';
            if($jur_groups_level=='region')
            {
                ?>
                <th style="white-space: nowrap;">Region</th>
                <?
                $chartGroup='r_jur_name';
            }
            if($jur_groups_level=='district')
            {
                ?>
                <th style="white-space: nowrap;">Region</th>
                <th style="white-space: nowrap;">District</th>
                <?
                $chartGroup='d_jur_name';
            }
            if($jur_groups_level=='club')
            {
                ?>
                <th style="white-space: nowrap;">Region</th>
                <th style="white-space: nowrap;">District</th>
                <th style="white-space: nowrap;">Club</th>
                <?
                $chartGroup='c_jur_name';
            }

            ?>
            <th style="white-space: nowrap;">Financial</th>
            <?

            if($casino==1){
                if($betwin)
                {
                    ?>
                    <th style="white-space: nowrap;">Casino<br>bet</th>
                    <th style="white-space: nowrap;">Casino<br>win</th>
                <?
                }
                ?>
                <th style="white-space: nowrap;">Casino<br>net</th>
                <?
                if($financial)
                {
                    ?>
                    <th style="white-space: nowrap;">Casino<br>fin</th>
                <?
                }
            }
            if($casino_live==1){
                if($betwin)
                {
                    ?>
                    <th style="white-space: nowrap;">Casino<br>live bet</th>
                    <th style="white-space: nowrap;">Casino<br>live win</th>
                <?
                }
                ?>
                <th style="white-space: nowrap;">Casino<br>live net</th>
                <?
                if($financial)
                {
                    ?>
                    <th style="white-space: nowrap;">Casino<br>live fin</th>
                <?
                }
            }

            if($virtual==1){
                if($betwin)
                {
                    ?>
                    <th style="white-space: nowrap;">Virtual<br>bet</th>
                    <th style="white-space: nowrap;">Virtual<br>win</th>
                <?
                }
                ?>
                <th style="white-space: nowrap;">Virtual<br>net</th>
                <?
                if($financial)
                {
                    ?>
                    <th style="white-space: nowrap;">Virtual<br>fin</th>
                <?
                }
            }
            if($pokerl==1){
                if($betwin)
                {
                    ?>
                    <th style="white-space: nowrap;">Poker live<br>bet</th>
                    <th style="white-space: nowrap;">Poker live<br>win</th>
                <?
                }
                ?>
                <th style="white-space: nowrap;">Poker live<br>rake</th>
                <?
                if($financial)
                {
                    ?>
                    <th style="white-space: nowrap;">Poker live<br>fin</th>
                <?
                }
            }

            if($poker2d==1){
                if($betwin)
                {
                    ?>
                    <th style="white-space: nowrap;">Poker 2d<br>bet</th>
                    <th style="white-space: nowrap;">Poker 2d<br>win</th>
                <?
                }
                ?>
                <th style="white-space: nowrap;">Poker 2d<br>rake</th>
                <?
                if($financial)
                {
                    ?>
                    <th style="white-space: nowrap;">Poker 2d<br>fin</th>
                <?
                }
            }

            if($betting==1){
                if($betwin){
                    ?>
                    <th style="white-space: nowrap;">Betting<br>bet</th>
                    <th style="white-space: nowrap;">Betting<br>win</th>
                <?
                }
                ?>
                <th style="white-space: nowrap;">Betting<br>net</th>
                <th style="white-space: nowrap;">Betting<br>cost club</th>
                <th style="white-space: nowrap;">Betting<br>gross profit</th>
                <th style="white-space: nowrap;">Betting<br>benefit</th>
                <?
                foreach ($betting_jurisdictions as $k=>$v){
                    ?>
                    <th style="white-space: nowrap;"><?=$v?></th>
                <?
                }
                if($financial){
                    ?>
                    <th style="white-space: nowrap;">Betting<br>fin</th>
                <?
                }
                if($betwin){
                    ?>
                    <th style="white-space: nowrap;">Betting live<br>bet</th>
                    <th style="white-space: nowrap;">Betting live<br>win</th>
                <?
                }
                ?>
                <th style="white-space: nowrap;">Betting live<br>net</th>
                <th style="white-space: nowrap;">Betting live<br>cost club</th>
                <th style="white-space: nowrap;">Betting live<br>gross profit</th>
                <th style="white-space: nowrap;">Betting live<br>benefit</th>
                <?
                foreach ($betting_jurisdictions_live as $k=>$v){
                    ?>
                    <th style="white-space: nowrap;"><?=$v?></th>
                <?
                }
                if($financial){
                    ?>
                    <th style="white-space: nowrap;">Betting live<br>fin</th>
                <?
                }
            }
            ?>
            <th style="white-space: nowrap;">Total Perc</th>
            <th>Net</th>
            <?
            if($betting==1){
                ?>
                <th style="white-space: nowrap;">Total Net Betting</th>
            <?
            }

            ?>
        </tr></thead>
        <tbody>
        <?
        $res->resetRecPtr();
        while($res->hasNext()) {
            $rec = $res->next();
            ?>
            <tr><td style="white-space: nowrap;"><strong><?=$rec["n_jur_name"]?></strong></td>
                <?
                if($jur_groups_level=='region')
                {
                    ?>
                    <td style="white-space: nowrap;"><strong><?=$rec["r_jur_name"]?></strong><br /> <span class=\"tip\"> % </span></td>
                <?
                }
                if($jur_groups_level=='district')
                {
                    ?>
                    <td style="white-space: nowrap;"><strong><?=$rec["r_jur_name"]?></strong></td>
                    <td style="white-space: nowrap;"><strong><?=$rec["d_jur_name"]?></strong><br /><span class=\"tip\"> % </span></td>
                <?
                }
                if($jur_groups_level=='club')
                {
                    ?>
                    <td style="white-space: nowrap;"><strong><?=$rec["d_jur_name"] ?></strong></td>
                    <td style="white-space: nowrap;"><strong><?=$rec["r_jur_name"] ?></strong></td>
                    <td style="white-space: nowrap;"><strong><a href=javascript:getCustomerDetails('<?=$rec['jds_jur_club']?>','<?=$_POST['date_start']?>','<?=$_POST['date_end']?>','<?=$_POST['betwin']?>','<?=$_POST['financial']?>');><?=$rec['c_jur_name']?> </a></strong><br /><span class=\"tip\"> % </span></td>
                <?
                }

                ?>
                <td style="white-space: nowrap;text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["fin"]))?></td>
                <?

                if($casino==1){
                    if($betwin)
                    {
                        ?>
                        <td style="white-space: nowrap;text-align: right"><?=preg_replace("/\&nbsp; /", "", getInDollars($rec["casino_bet"]))?></td>
                        <td style="white-space: nowrap;text-align: right"><?=preg_replace("/\&nbsp; /", "", getInDollars($rec["casino_win"]))?></td>
                    <?
                    }
                    ?>
                    <td style="white-space: nowrap;text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_rake"]))."<br>".($rec["jps_perc_casino"])."% = ".preg_replace("/\&nbsp; /","",getInDollars($rec["casino_rake"]/100* ($rec["jps_perc_casino"])))?></strong></td>
                    <?
                    if($financial)
                    {
                        ?>
                        <td style="white-space: nowrap;text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_fin"]))?></td>
                    <?
                    }
                }
                if($casino_live==1){
                    if($betwin)
                    {
                        ?>
                        <td style="white-space: nowrap;text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_live_bet"]))?></td>
                        <td style="white-space: nowrap;text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_live_win"]))?></td>
                    <?
                    }
                    ?>
                    <td style="white-space: nowrap;text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_live_rake"]))."<br>".($rec["jps_perc_casino_live"])."% = ".preg_replace("/\&nbsp; /","",getInDollars($rec["casino_live_rake"]/100* ($rec["jps_perc_casino_live"])))?></strong></td>
                    <?
                    if($financial)
                    {
                        ?>
                        <td style="white-space: nowrap;text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_live_fin"]))?></td>
                    <?
                    }
                }

                if($virtual==1){
                    if($betwin)
                    {
                        ?>
                        <td style="white-space: nowrap;text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["virtual_bet"]))?></td>
                        <td style="white-space: nowrap;text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["virtual_win"]))?></td>
                    <?
                    }
                    ?>
                    <td style="white-space: nowrap;text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["virtual_rake"]))."<br>".($rec["jps_perc_virtual"])."% = ".preg_replace("/\&nbsp; /","",getInDollars($rec["virtual_rake"]/100* ($rec["jps_perc_virtual"])))?></strong></td>
                    <?
                    if($financial)
                    {
                        ?>
                        <td style="white-space: nowrap;text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["virtual_fin"]))?></td>
                    <?
                    }
                }
                if($pokerl==1){
                    if($betwin)
                    {
                        ?>
                        <td style="white-space: nowrap;text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["poker_live_bet"]))?></td>
                        <td style="white-space: nowrap;text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["poker_live_win"]))?></td>
                    <?
                    }
                    ?>
                    <td style="white-space: nowrap;text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["poker_live_rake"]))."<br>".($rec["jps_perc_poker_live"])."% = ".preg_replace("/\&nbsp; /","",getInDollars($rec["poker_live_rake"]/100* ($rec["jps_perc_poker_live"])))?></strong></td>
                    <?
                    if($financial)
                    {
                        ?>
                        <td style="white-space: nowrap;text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["poker_live_fin"]))?></td>
                    <?
                    }
                }

                if($poker2d==1){
                    if($betwin)
                    {
                        ?>
                        <td style="white-space: nowrap;text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["poker_2d_bet"]))?></td>
                        <td style="white-space: nowrap;text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["poker_2d_win"]))?></td>
                    <?
                    }
                    ?>
                    <td style="white-space: nowrap;text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["poker_2d_rake"]))."<br>".($rec["jps_perc_poker_2d"])."% = ".preg_replace("/\&nbsp; /","",getInDollars($rec["poker_2d_rake"]/100* ($rec["jps_perc_poker_2d"])))?></strong></td>
                    <?
                    if($financial)
                    {
                        ?>
                        <td style="white-space: nowrap;text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["poker_2d_fin"]))?></td>
                    <?
                    }
                }

                if($betting==1){
                    if($betwin){
                        ?>
                        <td style="white-space: nowrap;text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_bet"]))?></td>
                        <td style="white-space: nowrap;text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_win"]))?></td>
                    <?
                    }
                    ?>
                    <td style="white-space: nowrap;text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_bet"]-$rec["betting_win"]))?></strong></td>
                    <td style="white-space: nowrap;text-align: right"><strong><?=($GLOBALS["jur_groups_level"]=="club" ?  getBettingPercentage($rec["jps_perc_betting_c"],"type") : "" )."<br> % = ".preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_rake"]))?></strong></td></td>
                    <td style="white-space: nowrap;text-align: right"><strong><br /><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_bet"]-$rec["betting_win"]-$rec["betting_rake"]))?></strong></td>
                    <td style="white-space: nowrap;text-align: right"><strong><br /><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_bet"]-$rec["betting_win"]-$rec["betting_rake"]-calculateBettingRake($rec,$GLOBALS["jur_groups_level"],1)))?></strong></td>
                    <?
                        if(array_key_exists('Betting <br>cost nation',$betting_jurisdictions)){
                            ?>
                                <td style="white-space: nowrap;text-align: right"><?=($GLOBALS["jur_groups_level"]=="nation" ? "<strong>" : "<span class=tip>" ).getBettingPercentage($rec["jps_perc_betting_n"],"type")."<br>".calculateBettingRake($rec,"nation")?></td>
                            <?

                        }
                    if(array_key_exists('Betting <br>cost region',$betting_jurisdictions)){
                        ?>
                        <td style="white-space: nowrap;text-align: right"><?=($GLOBALS["jur_groups_level"]=="region" ? "<strong>" : "<span class=tip>" ).getBettingPercentage($rec["jps_perc_betting_r"],"type")."<br>".calculateBettingRake($rec,"region")?></td>
                    <?

                    }
                    if(array_key_exists('Betting <br>cost district',$betting_jurisdictions)){
                        ?>
                        <td style="white-space: nowrap;text-align: right"><?=($GLOBALS["jur_groups_level"]=="district" ? "<strong>" : "<span class=tip>" ).getBettingPercentage($rec["jps_perc_betting_d"],"type")."<br>".calculateBettingRake($rec,"district")?></td>
                    <?

                    }

                    if($financial){
                        ?>
                        <td style="white-space: nowrap;text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_fin"]))?></td>
                    <?
                    }
                    if($betwin){
                        ?>
                        <td style="white-space: nowrap;text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_bet_live"]))?></td>
                        <td style="white-space: nowrap;text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_win_live"]))?></td>
                    <?
                    }
                    ?>
                    <td style="white-space: nowrap;text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_bet_live"]-$rec["betting_win_live"]))?></strong></td>
                    <td style="white-space: nowrap;text-align: right"><strong><?=($GLOBALS["jur_groups_level"]=="club" ?  getBettingPercentage($rec["jps_perc_betting_live_c"],"type"): "")."<br> % = ".preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_rake_live"]))?></strong></td>
                    <td style="white-space: nowrap;text-align: right"><strong><br /><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_bet_live"]-$rec["betting_win_live"]-$rec["betting_rake_live"]))?></strong></td>
                    <td style="white-space: nowrap;text-align: right"><strong><br /><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_bet_live"]-$rec["betting_win_live"]-$rec["betting_rake_live"]-calculateLiveBettingRake($rec,$GLOBALS["jur_groups_level"],1)))?></td>
                    <?
                    if(array_key_exists('Betting live<br>cost nation',$betting_jurisdictions_live)){
                        ?>
                        <td style="white-space: nowrap;text-align: right"><?=($GLOBALS["jur_groups_level"]=="nation" ? "<strong>" : "<span class=tip>" ).getBettingPercentage($rec["jps_perc_betting_live_n"],"type")."<br>".calculateLiveBettingRake($rec,"nation")?></td>
                    <?
                    }
                    if(array_key_exists('Betting live<br>cost region',$betting_jurisdictions_live)){
                        ?>
                        <td style="white-space: nowrap;text-align: right"><?=($GLOBALS["jur_groups_level"]=="region" ? "<strong>" : "<span class=tip>" ). getBettingPercentage($rec["jps_perc_betting_live_r"],"type")."<br>".calculateLiveBettingRake($rec,"region")?></td>
                    <?

                    }
                    if(array_key_exists('Betting live<br>cost district',$betting_jurisdictions_live)){
                        ?>
                        <td style="white-space: nowrap;text-align: right"><?=($GLOBALS["jur_groups_level"]=="district" ? "<strong>" : "<span class=tip>" ).getBettingPercentage($rec["jps_perc_betting_live_d"],"type")."<br>".calculateLiveBettingRake($rec,"district")?></td>
                    <?
                    }

                    if($financial){
                        ?>
                        <td style="white-space: nowrap;text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_fin_live"]))?></td>
                    <?
                    }
                }
                ?>
                <td style="white-space: nowrap;text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars(calculatePerc($rec)))."<br>% = ".preg_replace("/\&nbsp; /","",getInDollars(calculateCurrentSubPerc($rec)))?></strong></td>
                <td style="white-space: nowrap;text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($rec["fin"]-(abs(calculateCurrentSubPerc($rec)))))?> </td>
                <?
                if($betting==1){
                    ?>
                    <td style="white-space: nowrap;text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars(calculatePercBetting($rec)))."<br>% = ".preg_replace("/\&nbsp; /","",getInDollars(calculateCurrentSubPercBetting($rec)))?></strong></td>
                <?
                }
                ?>
            </tr>
        <?
        }
    }

    ?>
</tbody></table>
    </center>
    <div id="graphs" style="width:100%; height:400px;"></div><br>
    <?php
    if($res->getNumRows() > 0){
        include_once("classes/highcharts.class.php");
        $totalRake=new highcharts('chart1', 'pie', 'Total Rakes', 'graphs');

        $totalChartArray=array();
        if($casino==1){
            array_push($totalChartArray,'casino_rake');
        }
        if($casino_live==1){
            array_push($totalChartArray,'casino_live_rake');
        }
        if($virtual==1){
            array_push($totalChartArray,'virtual_rake');
        }
        if($pokerl==1){
            array_push($totalChartArray,'poker_live_rake');
        }
        if($poker2d==1){
            array_push($totalChartArray,'poker_2d_rake');
        }
        if($betting==1){
            array_push($totalChartArray,'betting_rake');
        }
        if($bettingl==1){
            array_push($totalChartArray,'betting_rake_live');
        }
        $totalRake->prepareData($totals, "$chartGroup",$totalChartArray);
        ?>
        <?=$totalRake->showChart();?>
        <div style="width:100%;width:1400px;height:300px;">
            <div id="graphs_casino" style='float:left;width:30%;margin-left:5%;'></div>
            <div id="graphs_casino_live" style='float:left;width:30%;'></div>
            <div id="graphs_virtual" style='float:left;width:30%;'></div>
            <div id="graphs_poker" style='float:left;width:30%;'></div>
            <div id="graphs_poker_2d" style='float:left;width:30%; margin-left:5%;'></div>
            <div id="graphs_betting" style='float:left;width:30%;'></div>
            <div id="graphs_betting_live" style='float:left;width:30%; margin-left:5%;'></div>
        </div>

        <?php
        if($casino==1){
            $casinoRake=new highcharts('chart2', 'pie', 'Casino Rakes', 'graphs_casino');
            $casinoRake->prepareData($res->Records, "$chartGroup",'casino_rake');
            ?>
            <?=$casinoRake->showChart('80');
        }?>



        <?php
        if($casino_live==1){
            $casinoLiveRake=new highcharts('chart3', 'pie', 'Casino Live Rakes', 'graphs_casino_live');
            $casinoLiveRake->prepareData($res->Records, "$chartGroup",'casino_live_rake');
            ?>
            <?=$casinoLiveRake->showChart('80'); } ?>


        <?php
        if($pokerl==1){
            $pokerLiveRake=new highcharts('chart4', 'pie', 'Poker Live Rakes', 'graphs_poker');
            $pokerLiveRake->prepareData($res->Records, "$chartGroup",'poker_live_rake');
            ?>
            <?=$pokerLiveRake->showChart('80');
        }?>

        <?php
        if($poker2d==1){
            $poker2dRake=new highcharts('chart7', 'pie', 'Poker 2d Rakes', 'graphs_poker_2d');
            $poker2dRake->prepareData($res->Records, "$chartGroup",'poker_2d_rake');
            ?>
            <?=$poker2dRake->showChart('80'); }?>

        <?php
        if($virtual==1){
            $virtualRake=new highcharts('chart5', 'pie', 'Virtual Rakes', 'graphs_virtual');
            $virtualRake->prepareData($res->Records, "$chartGroup",'virtual_rake');
            ?>
            <?=$virtualRake->showChart('80'); } ?>

        <?php
        if($betting==1){
            $bettingRake=new highcharts('chart6', 'pie', 'Betting Rakes', 'graphs_betting');
            $bettingRake->prepareData($res->Records, "$chartGroup",'betting_rake');
            ?>
            <?=$bettingRake->showChart('80'); } ?>

        <?php
        if($bettingl==1){
            $bettingRakeLive=new highcharts('chart8', 'pie', 'Live Betting Rakes', 'graphs_betting_live');
            $bettingRakeLive->prepareData($res->Records, "$chartGroup",'betting_rake_live');
            ?>
            <?=$bettingRakeLive->showChart('80'); } ?>

    <?php
    }
}
?>
</div>
</td></tr></table>
</div>