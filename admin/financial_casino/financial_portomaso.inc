<?$active=(isset($_POST["active"])) ? $_POST["active"] : 'casino';
require_once "financial_casino/financial_portomaso_functions.inc";
include_once ('jurisdiction_filters.php');
global $lang;
$date_start = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d", time() - (60 * 60 * 24));
$date_end = (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d", time() - (60 * 60 * 24));
$defaultmonth= (isset($_POST['monthHelper'])) ? $_POST['monthHelper'] : false;
$defaultweek= (isset($_POST['weekHelper'])) ? $_POST['weekHelper'] : false;

$portomaso= 1;
$infinity= 0;
$canCangeCurency=check_access('view_admin_currencies',false);
$betwin = true;

?>
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jquery.dataTables.css" />
    <script src="<?= secure_host ?>/media/jquery.dataTables.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/highcharts.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/highcharts-3d.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/modules/exporting.js" type="text/javascript"></script>

    <script type="text/javascript">
        $(function() {
            $( "#tabs" ).tabs();
            $('#periodType').trigger('change');
            $('.jurisdictionSelector').trigger('change');
            $('.dateStartPicker,.dateEndPicker').keyup(function() {
                $( ".monthHelper, .weekHelper,#periodType" ).val('');
            });
            $( ".dateStartPicker" ).datepicker({
                changeMonth: true,
                changeYear: true,
                numberOfMonths:1,
                onSelect: function( selectedDate ) {
                    // $( "input[name='date_end']" ).datepicker( "option", "minDate", selectedDate );
                    $( ".monthHelper, .weekHelper,#periodType" ).val('');
                }
            });
            $( ".dateEndPicker" ).datepicker({
                changeMonth: true,
                changeYear: true,
                numberOfMonths:1,
                onSelect: function( selectedDate ) {
                    //$( "input[name='date_start']" ).datepicker( "option", "maxDate", selectedDate );
                    $( ".monthHelper, .weekHelper,#periodType" ).val('');
                }
            });

            $("#toggleAllCurrency").on("click",function(){
                $('.changeCurr').trigger('click');
            });
        });

        $(document).on('change', ".monthHelper", function() {
            createMonthHelper($(this).val());
            $('.monthHelper').not(this).val('');
        });
        $(document).on('change', "[name='weekHelper']", function() {
            createWeekHelper($(this).val());
        });
        $(document).on('change', "#periodType", function() {
            var currVal=$(this).val();
            $('.timeHelpers').hide();
            $('#period'+currVal).show();
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

        $(document).on('click', '.changeCurr', function() {
            var iSymbol = $(this).closest('tr').find('.cSymbol');
            var iAmount = $(this).closest('tr').find('.cAmount');
            var curr = $(this).attr("data-curr");
            var mult=parseFloat($( "input[name='"+curr+"']").val());
            var symb=$(this).html();
            if( symb=='<?=$_SESSION['currency_html']?>' || symb==$('#jurisdiction_credit .cSymbol:first-child').html().trim()){
                $(this).html(iSymbol.html());
                iSymbol.each(function() {
                    $( this ).html( "<?=$_SESSION['currency_html']?>" );
                });
                iAmount.each(function() {
                    if($(this).html().indexOf(',')!='-1'){
                        newValue=parseFloat($(this).html().replace('.','').replace(',','.'));
                    }
                    else{
                        newValue=parseFloat($(this).html().replace(',','.'));
                    }
                    $( this ).html((newValue/mult).toFixed(2));
                });
            }
            else{
                iSymbol.each(function() {
                    $( this ).html(symb+" ");
                });
                iAmount.each(function() {
                    if($(this).html().indexOf(',')!='-1'){
                        newValue=parseFloat($(this).html().replace('.','').replace(',','.'));
                    }
                    else{
                        newValue=parseFloat($(this).html().replace(',','.'));
                    }
                    $( this ).html((newValue*mult).toFixed(2));
                });
                $(this).html("<?=$_SESSION['currency_html']?>");
            }

        });

        function createWeekHelper(dateobj){
            dates=dateobj.split('~');
            firstDay=dates[0];
            lastDay=dates[1];
            $( ".dateStartPicker" ).datepicker('setDate', firstDay);
            $( ".dateEndPicker" ).datepicker('setDate', lastDay);
            $('.monthHelper').val('');
        }

        function createMonthHelper(dateobj){
            dates=dateobj.split('~');
            if(dates.length<2){
                var day=new Date(dates[0]);
                var firstDay = new Date(day.getFullYear(), day.getMonth(), 1);
                var lastDay = new Date(day.getFullYear(), day.getMonth() + 1,0);
            }
            else{
                firstDay=dates[0];
                lastDay=dates[1];
            }

            $(".dateStartPicker" ).datepicker('setDate', firstDay);
            $(".dateEndPicker" ).datepicker('setDate', lastDay);
            $('.weekHelper').val('');
        }

        function hideCustomerDetails()
        {
            $("#cr_container").hide('slow');
            $("#cr").hide('slow');
            $("#cr_body").html("");
        }

        function getCustomerDetails(c,ds,de,bw,f,ca,cl,v,p2d,pl,b,rpk,csx,csb,infinity,portomaso,ttm,forex,bankr,zecc,gree)
        {
            $("#cr_container").show('slow');
            $("#cr").show('slow');
            $("#cr_body").empty().append(' <b><?=$lang->getLang("Loading")?>...   </b> <img src="media/images/ajax-loader.gif">');
            $.post("financial_casino/financial_portomaso_details.inc",{action:"1",
                club:c,
                date_start:ds,
                date_end:de,
                betwin:bw,                
                portomaso:portomaso
            }, function(data){
                $("#cr_body").empty().append( data );
                $('html, body').animate({
                    scrollTop: $("#cr_body").offset().top - 50
                }, 1000);

            $('#userDetails').DataTable();




            });
        }
    </script>
<body>
<?if (!check_access('financial_portomaso') ) {
    ?>
    <div class="error">You don't have any option enabled</div>
<?}else{ ?>
    <div id="tabs">
    <ul>
        <?if(check_access('financial_portomaso')){?>
            <li><a href="#casino">Portomaso</a></li>
        <? } ?>
    </ul>

    <?if(check_access('financial_portomaso')){?>
        <div id="casino">
        <h2 class="centered"><?=$lang->getLang("Portomaso Financial Activity Summary")?></h2>
        <p>
        <div id="customers">
            <div id="cr_container"></div>
            <div id="cr"><button type="button" class="close" style="color:darkblue" onclick="hideCustomerDetails();">X</button><div id="cr_body"></div></div></div><?php

        ?>
        <!-- The functions for initialising the date input -->
        <center>
            <br/>
            <p>
                <?php
                /**
                 * @author Daniel Miut
                 */
                ?>
            <form action="/financial_portomaso" method="post" class="resubmitForm">
                <?php printHiddenInput('doQuery', 'yes');?>
                <?php printHiddenInput('search_type', 'date_range');?>
                <input type="hidden" name="active" value="casino">
                <table cellpadding="4" cellspacing="1" border="0">
                    <tr valign=top>
                        <td class=label><?=$lang->getLang("Choose month")?></td>
                        <td class=content>
                            <?=month_select_box($defaultmonth)?>
                        </td>
                    </tr>

                    <tr valign=top>
                        <td class=label><?=$lang->getLang("Choose week")?></td>
                        <td class=content>
                            <?=week_select_box($defaultweek)?>
                        </td>
                    </tr>


                    <tr valign=top>
                        <td class=label><?=$lang->getLang("From")?></td>
                        <td class=content>
                            <input type='text' name='date_start' id='date_start' class="dateStartPicker" value="<?=$date_start?>" >
                        </td>
                    </tr>
                    <tr valign=top>
                        <td class=label><?=$lang->getLang("Until")?></td>
                        <td class=content>
                            <input type='date_end' name='date_end' id='date_end' class="dateEndPicker" value="<?=$date_end?>">
                        </td>
                    </tr>
                    
                    </tr>
                        <tr valign=top>
                            <td class=label><?=$lang->getLang("Details")?></td>
                            <td class=content>
                                <input type='checkbox' name='betwin' id='betwin' disabled="disabled" checked="checked" /><?=$lang->getLang("Bet & Win")?>
                            </td>
                        </tr>
                    
                    <tr>
                        <td class="label"><?=$lang->getLang("Result in Page")?></td>
                        <td class="content"><select name="resforpage">
                                <option value="100">100</option>
                            </select></td>
                    </tr>
                    <tr>
                        <td></td>
                        <td><input type =image src="<?=image_dir?>/button_submit.gif" name="submit"></td>
                    </tr>
                </table>
            </form>
        </center>
        <?php

        if(isset($_POST['doQuery']) && $active=='casino'){

            ?>
            <script>
                $(function() {
                    $( "#tabs" ).tabs( "option", "active", 0 );
                });

            </script>
            <?
            $date_start = $_POST["date_start"];
            $date_end   = $_POST["date_end"];

            $d1 = strtotime("$date_start+1 year");
            $d2 = strtotime("$date_end");

            if($d2-$d1>0){
                addError("",$lang->getLang("Please choose an interval less than 1 year"));
                showErrors();
            }
            if ( ! areErrors() )
            {
                $betwin = (isset($_POST['betwin'])&&$_POST['betwin']==true) ? true : false;
                $financial = (isset($_POST['financial'])&&$_POST['financial']==true) ? true : false;
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
 

                $diffCurr=false;
                $isDiffCur=false;
                $prevCur='';
                
                $totalsNetSum=0;
                $totalsBetSum=0;
                $totalsWinSum=0;
                $totalUsers = 0;
                $elemens = array();
                $index = 0;
                while($res->hasNext()){
                    $row = $res->next();
                    $elemens[$index++] = $row;
                    if($row['currency']!=$prevCur && $prevCur!==''){
                        $isDiffCur=true;
                    }
                    $prevCur = $row['currency'];

                    if($row["currency"]!=$_SESSION["currency_id"]){
                        $diffCurr=true;
                        foreach($row as $n=>$nv){
                            if((strpos($n,'tot_bet') !== false) || (strpos($n,'tot_win') !== false) || (strpos($n,'tot_net') !== false)){
                                if($nv!=0){
                                    if($_SESSION["currency_id"]==1){
                                        $res->Records[$res->CurrentIndex][$n]=$nv/$_SESSION["currencies"][$row["currency"]]["value"];
                                        $row[$n]=$nv/$_SESSION["currencies"][$row["currency"]]["value"];
                                    }
                                    else{
                                        $res->Records[$res->CurrentIndex][$n]=$nv*$_SESSION["currencies"][$_SESSION["currency_id"]]["value"];
                                        $row[$n]=$nv*$_SESSION["currencies"][$_SESSION["currency_id"]]["value"];
                                    }
                                }
                            }
                        }
                    }
                    $totalsNetSum += $row['tot_net'];
                    $totalsBetSum += $row['tot_bet'];
                    $totalsWinSum += $row['tot_win'];
                    $totalUsers += $row['users'];
                }
                
                ?>
                <div class="tip fleft"><?=$lang->getLang("Time taken to execute your request")?>: <?=$duration?></div>
                <center>
                <br><br><br>
                <div class="bodyHD"><?=$lang->getLang("Total")?></div>
                <table bgcolor="#a99a9" cellpadding="4" cellspacing="1">
                    <tr>
                        <td class="label centered"><?=$lang->getLang($_SESSION["currencies"][$_SESSION['currency_id']]["symbol"])?></td>
                        <td class="label centered"><?=$lang->getLang("Tot Bet")?></td>
                        <td class="label centered"><?=$lang->getLang("Tot Win")?></td>
                        <td class="label centered"><?=$lang->getLang("Tot Net")?></td>
                        <td class="label centered"><?=$lang->getLang("Tot Users")?></td>
                    </tr>
                    <tr>
                        <?if($elemens[$x]["currency"] != $_SESSION['currency_id'] && $_SESSION['currency_id']!=$prevCu){ ?>
                            <td class="content centered"><button class="entitybtn" id="toggleAllCurrency"><?=$lang->getLang("Toggle All")?></button></td>
                        <? } ?>
                        <td class="content" style="text-align: right"><?=getInDollars($totalsBetSum)?></td>
                        <td class="content" style="text-align: right"><?=getInDollars($totalsWinSum)?></td>
                        <td class="content" style="text-align: right"><?=getInDollars($totalsNetSum)?></td>
                        <td class="content" style="text-align: right"><?=$totalUsers?><br>
                        </td>
                    </tr>
                </table>
<!-- partial totals -->
                <br>
                <div class="bodyHD"><?=$lang->getLang("Partial Totals")?></div>
                <table bgcolor="#a99a9" cellpadding="4" cellspacing="1">
                <tr>
                    	<td class="label centered"><?=$_SESSION["currencies"][$_SESSION['currency_id']]["symbol"]?> </td>     
                        <td class="label centered"><br><?=$lang->getLang("Game")?></td>
                        <td class="label centered"><br><?=$lang->getLang("Bet")?></td>
                        <td class="label centered"><br><?=$lang->getLang("Win")?></td>
                        <td class="label centered"><br><?=$lang->getLang("Net")?></td>
                        <td class="label centered"><br><?=$lang->getLang("Users")?></td>
                        <td class="label centered"><?=$lang->getLang("Currency")?></td>
                </tr>
                <?php 
                    $arrlength = count($elemens);
                    for($x = 0; $x < $arrlength; $x++) { ?>
                <tr class='odd' style="vertical-align: top">
                    <?php if($elemens[$x]["currency"] != $_SESSION['currency_id'] && $_SESSION['currency_id']!=$prevCur){ ?>
                    <td class="content dark">
                        <span class=changeCurr style="font-weight:bold;color:red;cursor:pointer;" data-curr="<?=$prevCur?>" ><?=$_SESSION["currencies"][$prevCur]["symbol"]?></span></td>
                <? } else {?>                    
                    <td class="content dark"><?=$_SESSION["currencies"][$elemens[$x]["currency"]]["value"]?> </td>   
                <? }?>
                   <td class="content dark" style="text-align: right"><?= $elemens[$x]['gam_name']?></td> 
                   <?
                   $totBet = $elemens[$x]['tot_bet'];
                   $totWin = $elemens[$x]['tot_win'];
                   $totNet = $elemens[$x]['tot_net'];
                   
                   if ($elemens[$x]["currency"] != $_SESSION["currency_id"]) {
                       $totBet = $totBet / $_SESSION["currencies"][$elemens[$x]["currency"]]["value"];
                       $totWin = $totWin / $_SESSION["currencies"][$elemens[$x]["currency"]]["value"];
                       $totNet = $totNet / $_SESSION["currencies"][$elemens[$x]["currency"]]["value"];
                   }?>                    
                   <td class="content dark" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totBet))?></td>
                   <td class="content dark" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totWin))?></td>
                   <td class="content dark" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totNet))?></td>
                   <td class="content dark" style="text-align: right"><?= isset($elemens[$x]['users']) ? $elemens[$x]['users'] : 0 ?></td>  
                   <td class="content dark" style="text-align: right"><?= $elemens[$x]['cur_name'] ?></td>                                     
                </tr>
                
                    <? }?>
                </table>
                <br>

                <!-- end partial totals  -->

                </center>
                <?php
            }
        } ?>
        </p>
        </div>

    <? }?>

    </div>
<? }


?>