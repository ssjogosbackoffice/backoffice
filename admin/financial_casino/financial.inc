<?$active=(isset($_POST["active"])) ? $_POST["active"] : 'casino';
$periodType=(isset($_POST["periodType"])) ? $_POST["periodType"] : '0';
require_once "financial_casino/financial_functions.inc";
include_once ('jurisdiction_filters.php');
global $lang;
$jurisdiction_level = (isset($_POST['jurisdiction_level'])) ? $_POST['jurisdiction_level'] : getStartJurisdictionLevel();
$jur_groups_level = (isset($_POST['jur_level'])) ? $_POST['jur_level'] : getStartJurisdictionLevel();
$date_start = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d", time() - (60 * 60 * 24));
$date_end = (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d", time() - (60 * 60 * 24));
$row_to_page = (isset($_POST['resforpage'])) ? $_POST['resforpage'] : 20;
$defaultmonth= (isset($_POST['monthHelper'])) ? $_POST['monthHelper'] : false;
$defaultmonth2= (isset($_POST['monthHelper2'])) ? $_POST['monthHelper2'] : false;
$defaultmonth4= (isset($_POST['monthHelper4'])) ? $_POST['monthHelper4'] : false;
$defaultweek= (isset($_POST['weekHelper'])) ? $_POST['weekHelper'] : false;

$casino= (!isset($_POST['doQuery']) || $_POST['casino']==1 || ($active=='betting')) ? 1 : 0;
$casino_live= (!isset($_POST['doQuery']) || $_POST['casino_live']==1 || ($active=='betting')) ? 1 : 0;
$bankroulette= (!isset($_POST['doQuery']) || $_POST['bankroulette']==1 ||($active=='betting')) ? 1 : 0;
$portomaso= (!isset($_POST['doQuery']) || $_POST['portomaso']==1 ||($active=='betting')) ? 1 : 0;
$royalpk= (!isset($_POST['doQuery']) || $_POST['royalpk']==1 || ($active=='betting')) ? 1 : 0;
$poker2d= (!isset($_POST['doQuery']) || $_POST['poker2d']==1 ||($active=='betting')) ? 1 : 0;
$evolution= (!isset($_POST['doQuery']) || $_POST['evolution']==1 ||($active=='betting')) ? 1 : 0;
$infinity= (!isset($_POST['doQuery']) || $_POST['infinity']==1 ||($active=='betting')) ? 1 : 0;
$zeus= (!isset($_POST['doQuery']) || $_POST['zeus']==1 ||($active=='betting')) ? 1 : 0;

$forex= 0; //(!isset($_POST['doQuery']) || $_POST['forex']==1 ||($active=='betting')) ? 1 : 0;
$pokerl= 0; //(!isset($_POST['doQuery']) || $_POST['pokerl']==1 || ($active=='betting')) ? 1 : 0;
$totem= 0; //(!isset($_POST['doQuery']) || $_POST['totem']==1 ||($active=='betting')) ? 1 : 0;
$playtech= 0; //(!isset($_POST['doQuery']) || $_POST['playtech']==1 ||($active=='betting')) ? 1 : 0;
$virtual= 0;//(!isset($_POST['doQuery']) || $_POST['virtual']==1 ||($active=='betting')) ? 1 : 0;
$greek= 0; //(!isset($_POST['doQuery']) || $_POST['greek']==1 ||($active=='betting')) ? 1 : 0;
$zecchinetta= 0;//(!isset($_POST['doQuery']) || $_POST['zecchinetta']==1 ||($active=='betting')) ? 1 : 0;

$bettingl=$betting= 0;
$title = $lang->getLang("Financial Summary");
$partner= (isset($_POST['partner'])) ? $_POST['partner'] : false;
$canCangeCurency=check_access('view_admin_currencies',false);
$betwin = (isset($_POST['betwin'])&&$_POST['betwin']==true) ? true : false;

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

        function getCustomerDetails(c,ds,de,bw,f,ca,cl,v,p2d,pl,rpk,playtech,evolution,infinity,portomaso,ttm,forex,bankr,zecc,gree,zeus)
        {
            $("#cr_container").show('slow');
            $("#cr").show('slow');
            $("#cr_body").empty().append(' <b><?=$lang->getLang("Loading")?>...   </b> <img src="media/images/ajax-loader.gif">');
            $.post("financial_casino/financial_customer_details.inc",{action:"1",
                club:c,
                date_start:ds,
                date_end:de,
                betwin:bw,
                financial:f,
                ca:ca,
                cl:cl,
                v:v,
                p2d:p2d,
                pl:pl,
                rpk:rpk,
                playtech:playtech,
                evolution:evolution,
                infinity:infinity,
                portomaso:portomaso,
                ttm:ttm,
                forex:forex,
                bankr:bankr,
                zecc:zecc,
                gree:gree,
                zeus:zeus
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
<?if (!check_access('financial_summary_casino') && !check_access('financial_summary_betting') && !check_access('financial_summary_virtual') && !check_access('financial_summary_user_details') && !check_access('financial_summary_partner')) {
    ?>
    <div class="error">You don't have any option enabled</div>
<?}else{ ?>
    <div id="tabs">
    <ul>
        <?if(check_access('financial_summary_casino')){?>
            <li><a href="#casino">Casino & Poker</a></li>
        <? } if(check_access('financial_summary_betting')){?>
            <li><a href="#betting">Betting & Betting live</a></li>
        <? } if(check_access('financial_summary_virtual')){?>
            <li><a href="#virtual">Virtual</a></li>
        <? } if(check_access('financial_summary_user_details')){?>
            <li><a href="#usersFinancial">Users Financial</a></li>
        <? } if(check_access('financial_summary_partner')){?>
            <li><a href="#partnersFinancial">Partners</a></li>
        <? }?>
    </ul>

    <?if(check_access('financial_summary_casino')){?>
        <div id="casino">
        <h2 class="centered"><?=$lang->getLang("Casino & Poker Financial Activity Summary")?></h2>
        <p>
        <div id="customers">
            <div id="cr_container"></div>
            <div id="cr"><button type="button" class="close" style="color:darkblue" onclick="hideCustomerDetails();">X</button><div id="cr_body"></div></div></div><?php

        /*$betting_jurisdictions=array('betting<br>cost casino','betting<br>cost nation','betting<br>cost region','betting<br>cost district');
        $betting_jurisdictions_live=array('betting live<br>cost casino','betting live<br>cost nation','betting live<br>cost region','betting live<br>cost district');*/

        ?>
        <!-- The functions for initialising the date input -->
        <center>
            <br/>
            <p>
                <?php
                /**
                 * @author Marian
                 */
                ?>
            <form action="/financial_summary" method="post" class="resubmitForm">
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
                    <?php
                    if ( $_SESSION['jurisdiction_class'] != 'club' ) {
                        include ("financial_casino/jurisdiction_filter.php");
                    }
                    else{
                        $jur_groups_level='club';
                    }
                    ?>
                    </tr>
                    <?if(check_access('financial_summary_details')) {?>
                        <tr valign=top>
                            <td class=label><?=$lang->getLang("Details")?></td>
                            <td class=content>
                                <?if(check_access("financial_summary_details_betwin")){ ?>
                                    <input type='checkbox' name='betwin' id='betwin' <?php if(isset($_POST['betwin'])&& $_POST['betwin']==true) echo "checked"; ?> /><?=$lang->getLang("Bet & Win")?>
                                <? } ?>
                                <?if(check_access("financial_summary_details_fin")){ ?>
                                    <input type='checkbox' name='financial' id='financial' <?php if(isset($_POST['financial']) && $_POST['financial']==true) echo "checked"; ?> /><?=$lang->getLang("Financial")?>
                                <? }?>
                                <?if(check_access("financial_summary_details_fin")){ ?>
                                    <!--<input type='checkbox' name='bonus_financial' id='bonus_financial' <?php /*if(isset($_POST['bonus_financial']) && $_POST['bonus_financial']==true) echo "checked"; */?> />--><?/*=$lang->getLang("Bonus Financial")*/?>
                                <? }?>
                            </td>
                        </tr>
                    <? } ?>
                    <tr valign=top>
                        <td class=label><?=$lang->getLang("Financial contains")?></td>
                        <td class=content>
                            <label><input type='checkbox' name='casino' value="1" <?php if($casino==1) echo "checked"; ?> /><?=$lang->getLang("Casino")?></label>
                            <label><input type='checkbox' name='casino_live' value="1" <?php if($casino_live==1) echo "checked"; ?> /><?=$lang->getLang("Casino Live")?></label>
                            <!--<label><input type='checkbox' name='virtual' value="1" <?php if($virtual==1) echo "checked"; ?> /><?=$lang->getLang("Virtual")?></label>-->
                            <!--<label><input type='checkbox' name='royalpk' value="1" <?php /*if($royalpk==1) echo "checked"; */?> /><?/*=$lang->getLang("RoyalPk")*/?></label>-->
                            <label><input type='checkbox' name='bankroulette' value="1" <?php if($bankroulette==1) echo "checked"; ?> /><?=$lang->getLang("Bankroulette")?>
                            <!--<label><input type='checkbox' name='zecchinetta' value="1" <?php if($zecchinetta==1) echo "checked"; ?> /><?=$lang->getLang("Zecchinetta")?>-->
                            <!--<label><input type='checkbox' name='greek' value="1" <?php if($greek==1) echo "checked"; ?> /><?=$lang->getLang("Greek")?>-->
                            <label><input type='checkbox' name='zeus' value="1" <?php if($zeus==1) echo "checked"; ?> /><?=$lang->getLang("Zeus")?>
                            <!--<label><input type='checkbox' name='playtech' value="1" <?php if($playtech==1) echo "checked"; ?> /><?=$lang->getLang("Playt./SkyWind")?></label>-->
                            <label><input type='checkbox' name='evolution' value="1" <?php if($evolution==1) echo "checked"; ?> /><?=$lang->getLang("Evolution")?></label>
                            <label><input type='checkbox' name='infinity' value="1" <?php if($infinity==1) echo "checked"; ?> /><?=$lang->getLang("Infinity")?></label>
                            <label><input type='checkbox' name='portomaso' value="1" <?php if($portomaso==1) echo "checked"; ?> /><?=$lang->getLang("Portomaso")?>
                            <!--<label><input type='checkbox' name='pokerl' value="1" <?php if($pokerl==1) echo "checked"; ?> /><?=$lang->getLang("Poker Live")?></label>-->
                            <label><input type='checkbox' name='poker2d' value="1" <?php if($poker2d==1) echo "checked"; ?> /><?=$lang->getLang("Poker 2d")?></label>
                            <!--<label><input type='checkbox' name='totem' value="1" <?php if($totem==1) echo "checked"; ?> /><?=$lang->getLang("Totem")?></label>-->
                            <!--<label><input type='checkbox' name='forex' value="1" <?php /*if($forex==1) echo "checked"; */?> /><?/*=$lang->getLang("Forex")*/?></label>
-->
                        </td>
                    </tr>
                    <tr>
                        <td class="label"><?=$lang->getLang("Result in Page")?></td>
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
                $bonus_financial = (isset($_POST['bonus_financial'])&&$_POST['bonus_financial']==true) ? true : false;
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
                $cell_fmt_arr = array (
                    //'date'  => 'style="text-align:center;min-width:70px; "',
                    'casino<br>bet'                 => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'casino<br>win'                 => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'casino<br>net'                 => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'casino<br>fin'                 => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'casino<br>live bet'            => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'casino<br>live win'            => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'casino<br>live net'            => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'casino<br>live fin'            => 'style="text-align:right;white-space: nowrap;" class="content dark"',

                    'bankroulette<br>bet'                => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'bankroulette<br>win'                => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'bankroulette<br>rake'                => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'bankroulette<br>fin'                => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    
                    'zeus<br>bet'                => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'zeus<br>win'                => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'zeus<br>net'                => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'zeus<br>fin'                => 'style="text-align:right;white-space: nowrap;" class="content light"',

                    'evolution<br>bet'                => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'evolution<br>win'                => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'evolution<br>net'                => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'evolution<br>fin'                => 'style="text-align:right;white-space: nowrap;" class="content dark"',

                    'infinity<br>bet'                => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'infinity<br>win'                => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'infinity<br>net'                => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'infinity<br>fin'                => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    
                    'portomaso<br>bet'                => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'portomaso<br>win'                => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'portomaso<br>net'                => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'portomaso<br>fin'                => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    
                    'poker 2d<br>bet'               => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'poker 2d<br>win'               => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'poker 2d<br>net'               => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'poker 2d<br>rake'              => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'poker 2d<br>fin'               => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'betting<br>bet'                => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'betting<br>win'                => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'betting<br>net'                => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'betting<br>gross profit'       => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'betting<br>benefit'            => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'betting<br>cost club'          => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'betting<br>cost nation'        => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'betting<br>cost region'        => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'betting<br>cost district'      => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'betting<br>fin'                => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'betting live<br>bet'           => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'betting live<br>win'           => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'betting live<br>net'           => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'betting live<br>gross profit'  => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'betting live<br>benefit'       => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'betting live<br>cost club'     => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'betting live<br>cost nation'   => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'betting live<br>cost region'   => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'betting live<br>cost district' => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'betting live<br>fin'           => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'financial'                     => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    // ' Total Rake '                 => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'Total Net Betting'             => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'Net'                           => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'nation'                        => 'style="white-space: nowrap;" ',
                    'region'                        => 'style="white-space: nowrap;" ',
                    'district'                      => 'style="white-space: nowrap;" ',
                    'club'                          => 'style="white-space: nowrap;" '
                );
                $val_fmt_arr = array (
                    //'date'        => 'return "<strong>" . date("Y-m-d",strtotime($rec["jds_day"])) . "<br>%</strong>";',
                    'casino<br>bet'                 => 'return preg_replace("/\&nbsp; /", "", getInDollars($rec["casino_bet"]));',
                    'casino<br>win'                 => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_win"]));',
                    'casino<br>net'                 => 'return "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_rake"]))."<br>".($rec["jps_perc_casino"])."% = ".preg_replace("/\&nbsp; /","",getInDollars($rec["casino_rake"]/100* ($rec["jps_perc_casino"])));',
                    'casino<br>fin'                 => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_fin"]));',

                    'casino<br>live bet'            => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_live_bet"]));',
                    'casino<br>live win'            => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_live_win"]));',
                    'casino<br>live net'            => 'return "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_live_rake"]))."<br>".($rec["jps_perc_casino_live"])."% = ".preg_replace("/\&nbsp; /","",getInDollars($rec["casino_live_rake"]/100* ($rec["jps_perc_casino_live"])));',
                    'casino<br>live fin'            => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_live_fin"]));',

                    'bankroulette<br>bet'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_bankroulette_bet"]));',
                    'bankroulette<br>win'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_bankroulette_win"]));',
                    'bankroulette<br>rake'                => 'return  "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_bankroulette_rake"]))."<br>".($rec["jps_perc_bankroulette"])."% = ".preg_replace("/\&nbsp; /","",getInDollars($rec["casino_bankroulette_rake"]/100* ($rec["jps_perc_bankroulette"])));',
                    'bankroulette<br>fin'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_bankroulette_fin"]));',
                    
                    'zeus<br>bet'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["zeus_bet"]));',
                    'zeus<br>win'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["zeus_win"]));',
                    'zeus<br>net'                => 'return  "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($rec["zeus_rake"]))."<br>".($rec["jps_perc_zeus"])."% = ".preg_replace("/\&nbsp; /","",getInDollars($rec["zeus_rake"]/100* ($rec["jps_perc_zeus"])));',
                    'zeus<br>fin'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["zeus_fin"]));',

                    'evolution<br>bet'                  => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["evolution_bet"]));',
                    'evolution<br>win'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["evolution_win"]));',
                    'evolution<br>net'                => 'return  "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($rec["evolution_rake"]))."<br>".($rec["jps_perc_evolution"])."% = ".preg_replace("/\&nbsp; /","",getInDollars($rec["evolution_rake"]/100* ($rec["jps_perc_evolution"])));',
                    'evolution<br>fin'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["evolution_fin"]));',

                    'infinity<br>bet'                  => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["infinity_bet"]));',
                    'infinity<br>win'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["infinity_win"]));',
                    'infinity<br>net'                => 'return  "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($rec["infinity_rake"]))."<br>".($rec["jps_perc_infinity"])."% = ".preg_replace("/\&nbsp; /","",getInDollars($rec["infinity_rake"]/100* ($rec["jps_perc_infinity"])));',
                    'infinity<br>fin'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["infinity_fin"]));',
                    
                    'portomaso<br>bet'                  => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["portomaso_bet"]));',
                    'portomaso<br>win'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["portomaso_win"]));',
                    'portomaso<br>net'                => 'return  "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($rec["portomaso_rake"]))."<br>".($rec["jps_perc_infinity"])."% = ".preg_replace("/\&nbsp; /","",getInDollars($rec["portomaso_rake"]/100* ($rec["jps_perc_infinity"])));',
                    'portomaso<br>fin'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["portomaso_fin"]));',

                    'poker 2d<br>bet'               => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["poker_2d_bet"]));',
                    'poker 2d<br>win'               => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["poker_2d_win"]));',
                    'poker 2d<br>net'               => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["poker_2d_net"]));',
                    'poker 2d<br>rake'              => 'return "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($rec["poker_2d_rake"]))."<br>".($rec["jps_perc_poker_2d"])."% = ".preg_replace("/\&nbsp; /","",getInDollars($rec["poker_2d_rake"]/100* ($rec["jps_perc_poker_2d"])));',
                    'poker 2d<br>fin'               => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["poker_2d_fin"]));',

                    'betting<br>bet'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_bet"]));',
                    'betting<br>win'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_win"]));',
                    'betting<br>cost club'          => 'return "<strong>".($GLOBALS["jur_groups_level"]=="club" ?  getBettingPercentage($rec["jps_perc_betting_c"],"type") : "" )."<br> % = ".preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_rake"]));',
                    'betting<br>net'                => 'return "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_bet"]-$rec["betting_win"]));',
                    'betting<br>gross profit'       => 'return "<strong><br />".preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_bet"]-$rec["betting_win"]-$rec["betting_rake"]));',
                    'betting<br>benefit'            => 'return "<strong><br />".preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_bet"]-$rec["betting_win"]-$rec["betting_rake"]-calculateBettingRake($rec,$GLOBALS["jur_groups_level"],1)));',
                    'betting<br>cost nation'        => 'return ($GLOBALS["jur_groups_level"]=="nation" ? "<strong>" : "<span class=tip>" ).getBettingPercentage($rec["jps_perc_betting_n"],"type")."<br>".calculateBettingRake($rec,"nation");',
                    'betting<br>cost region'        => 'return ($GLOBALS["jur_groups_level"]=="region" ? "<strong>" : "<span class=tip>" ).getBettingPercentage($rec["jps_perc_betting_r"],"type")."<br>".calculateBettingRake($rec,"region");',
                    'betting<br>cost district'      => 'return ($GLOBALS["jur_groups_level"]=="district" ? "<strong>" : "<span class=tip>" ).getBettingPercentage($rec["jps_perc_betting_d"],"type")."<br>".calculateBettingRake($rec,"district");',
                    'betting<br>fin'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_fin"]));',
                    'betting live<br>bet'           => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_bet_live"]));',
                    'betting live<br>win'           => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_win_live"]));',
                    'betting live<br>cost club'     => 'return "<strong>".($GLOBALS["jur_groups_level"]=="club" ?  getBettingPercentage($rec["jps_perc_betting_c"],"type"): "")."<br> % = ".preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_rake_live"]));',
                    'betting live<br>net'           => 'return "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_bet_live"]-$rec["betting_win_live"]));',
                    'betting live<br>gross profit'  => 'return "<strong><br />".preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_bet_live"]-$rec["betting_win_live"]-$rec["betting_rake_live"]));',
                    'betting live<br>benefit'       => 'return "<strong><br />".preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_bet_live"]-$rec["betting_win_live"]-$rec["betting_rake_live"]-calculateLiveBettingRake($rec,$GLOBALS["jur_groups_level"],1)));',
                    'betting live<br>cost nation'   => 'return ($GLOBALS["jur_groups_level"]=="nation" ? "<strong>" : "<span class=tip>" ).getBettingPercentage($rec["jps_perc_betting_n"],"type")."<br>".calculateLiveBettingRake($rec,"nation");',
                    'betting live<br>cost region'   => 'return ($GLOBALS["jur_groups_level"]=="region" ? "<strong>" : "<span class=tip>" ). getBettingPercentage($rec["jps_perc_betting_r"],"type")."<br>".calculateLiveBettingRake($rec,"region");',
                    'betting live<br>cost district' => 'return ($GLOBALS["jur_groups_level"]=="district" ? "<strong>" : "<span class=tip>" ).getBettingPercentage($rec["jps_perc_betting_d"],"type")."<br>".calculateLiveBettingRake($rec,"district");',
                    'betting live<br>fin'           => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_fin_live"]));',
                    'financial'                     => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["fin"]));',
                    'Total Fin'                     => 'return "<strong>".preg_replace("/\&nbsp; /", "",getInDollars(calculateFin($rec)));',
                    ' Total Rake '                  => 'return "<strong>".preg_replace("/\&nbsp; /", "",getInDollars(calculatePerc($rec)))."<br>Total Perc = ".preg_replace("/\&nbsp; /","",getInDollars(calculateCurrentSubPerc($rec)));',
                    'Total Net Betting'             => 'return "<strong>".preg_replace("/\&nbsp; /", "",getInDollars(calculatePercBetting($rec)))."<br>% = ".preg_replace("/\&nbsp; /","",getInDollars(calculateCurrentSubPercBetting($rec)));',
                    'Net<br><span class=tipWhite>(Total Fin-Total Perc)'                           => 'return preg_replace("/\&nbsp; /", "",getInDollars(calculateFin($rec)-((calculateCurrentSubPerc($rec)))));',
                    'casino net'                   => 'return preg_replace("/\&nbsp; /", "",getInDollars(calculateRowNet($rec,"casino")));',
                    'nation net'                   => 'return preg_replace("/\&nbsp; /", "",getInDollars(calculateRowNet($rec,"nation")));',
                    'region net'                   => 'return preg_replace("/\&nbsp; /", "",getInDollars(calculateRowNet($rec,"region")));',
                    'district net'                 => 'return preg_replace("/\&nbsp; /", "",getInDollars(calculateRowNet($rec,"district")));'
                );


                $val_fmt_arr_excel = array (
                    //'date'        => 'return "<strong>" . date("Y-m-d",strtotime($rec["jds_day"])) . "<br>%</strong>";',
                    'casino<br>bet'                 => 'return preg_replace("/\&nbsp; /", "", $rec["casino_bet"]);',
                    'casino<br>win'                 => 'return preg_replace("/\&nbsp; /", "",$rec["casino_win"]);',
                    'casino<br>net'                 => 'return preg_replace("/\&nbsp; /", "",$rec["casino_rake"])."<br>".($rec["jps_perc_casino"])."% = ".preg_replace("/\&nbsp; /","",$rec["casino_rake"]/100* ($rec["jps_perc_casino"]));',
                    'casino<br>fin'                 => 'return preg_replace("/\&nbsp; /", "",$rec["casino_fin"]);',
                   
                    'casino<br>live bet'            => 'return preg_replace("/\&nbsp; /", "",$rec["casino_live_bet"]);',
                    'casino<br>live win'            => 'return preg_replace("/\&nbsp; /", "",$rec["casino_live_win"]);',
                    'casino<br>live net'            => 'return preg_replace("/\&nbsp; /", "",$rec["casino_live_rake"])."<br>".($rec["jps_perc_casino_live"])."% = ".preg_replace("/\&nbsp; /","",$rec["casino_live_rake"]/100* ($rec["jps_perc_casino_live"]));',
                    'casino<br>live fin'            => 'return preg_replace("/\&nbsp; /", "",$rec["casino_live_fin"]);',
                    
                    'bankroulette<br>bet'                => 'return preg_replace("/\&nbsp; /", "",$rec["casino_bankroulette_bet"]);',
                    'bankroulette<br>win'                => 'return preg_replace("/\&nbsp; /", "",$rec["casino_bankroulette_win"]);',
                    'bankroulette<br>rake'                => 'return  preg_replace("/\&nbsp; /", "",$rec["casino_bankroulette_rake"])."<br>".($rec["jps_perc_bankroulette"])."% = ".preg_replace("/\&nbsp; /","",$rec["casino_bankroulette_rake"]/100* ($rec["jps_perc_bankroulette"]));',
                    'bankroulette<br>fin'                => 'return preg_replace("/\&nbsp; /", "",$rec["casino_bankroulette_fin"]);',
                    
                    'zeus<br>bet'                => 'return preg_replace("/\&nbsp; /", "",$rec["zeus_bet"]);',
                    'zeus<br>win'                => 'return preg_replace("/\&nbsp; /", "",$rec["zeus_win"]);',
                    'zeus<br>net'                => 'return  preg_replace("/\&nbsp; /", "",$rec["zeus_rake"])."<br>".($rec["jps_perc_zeus"])."% = ".preg_replace("/\&nbsp; /","",$rec["zeus_rake"]/100* ($rec["jps_perc_zeus"]));',
                    'zeus<br>fin'                => 'return preg_replace("/\&nbsp; /", "",$rec["zeus_fin"]);',
                    
                    'evolution<br>bet'                => 'return preg_replace("/\&nbsp; /", "",$rec["evolution_bet"]);',
                    'evolution<br>win'                => 'return preg_replace("/\&nbsp; /", "",$rec["evolution_win"]);',
                    'evolution<br>net'                => 'return preg_replace("/\&nbsp; /", "",$rec["evolution_rake"])."<br>".($rec["jps_perc_evolution"])."% = ".preg_replace("/\&nbsp; /","",$rec["evolution_rake"]/100* ($rec["jps_perc_evolution"]));',
                    'evolution<br>fin'                => 'return preg_replace("/\&nbsp; /", "",$rec["evolution_fin"]);',

                    'infinity<br>bet'                => 'return preg_replace("/\&nbsp; /", "",$rec["infinity_bet"]);',
                    'infinity<br>win'                => 'return preg_replace("/\&nbsp; /", "",$rec["infinity_win"]);',
                    'infinity<br>net'                => 'return preg_replace("/\&nbsp; /", "",$rec["infinity_rake"])."<br>".($rec["jps_perc_infinity"])."% = ".preg_replace("/\&nbsp; /","",$rec["infinity_rake"]/100* ($rec["jps_perc_infinity"]));',
                    'infinity<br>fin'                => 'return preg_replace("/\&nbsp; /", "",$rec["infinity_fin"]);',
                    
                    'portomaso<br>bet'                => 'return preg_replace("/\&nbsp; /", "",$rec["portomaso_bet"]);',
                    'portomaso<br>win'                => 'return preg_replace("/\&nbsp; /", "",$rec["portomaso_win"]);',
                    'portomaso<br>net'                => 'return preg_replace("/\&nbsp; /", "",$rec["portomaso_rake"])."<br>".($rec["jps_perc_infinity"])."% = ".preg_replace("/\&nbsp; /","",$rec["portomaso_rake"]/100* ($rec["jps_perc_infinity"]));',
                    'portomaso<br>fin'                => 'return preg_replace("/\&nbsp; /", "",$rec["portomaso_fin"]);',
                    
                    'poker 2d<br>bet'               => 'return preg_replace("/\&nbsp; /", "",$rec["poker_2d_bet"]);',
                    'poker 2d<br>win'               => 'return preg_replace("/\&nbsp; /", "",$rec["poker_2d_win"]);',
                    'poker 2d<br>net'               => 'return preg_replace("/\&nbsp; /", "",$rec["poker_2d_net"]);',
                    'poker 2d<br>rake'              => 'return preg_replace("/\&nbsp; /", "",$rec["poker_2d_rake"])."<br>".($rec["jps_perc_poker_2d"])."% = ".preg_replace("/\&nbsp; /","",$rec["poker_2d_rake"]/100* ($rec["jps_perc_poker_2d"]));',
                    'poker 2d<br>fin'               => 'return preg_replace("/\&nbsp; /", "",$rec["poker_2d_fin"]);',
                    'betting<br>bet'                => 'return preg_replace("/\&nbsp; /", "",$rec["betting_bet"]);',
                    'betting<br>win'                => 'return preg_replace("/\&nbsp; /", "",$rec["betting_win"]);',
                    'betting<br>cost club'          => 'return ($GLOBALS["jur_groups_level"]=="club" ?  getBettingPercentage($rec["jps_perc_betting_c"],"type") : "" )."<br> % = ".preg_replace("/\&nbsp; /", "",$rec["betting_rake"]);',
                    'betting<br>net'                => 'return preg_replace("/\&nbsp; /", "",$rec["betting_bet"]-$rec["betting_win"]);',
                    'betting<br>gross profit'       => 'return preg_replace("/\&nbsp; /", "",$rec["betting_bet"]-$rec["betting_win"]-$rec["betting_rake"]);',
                    'betting<br>benefit'            => 'return preg_replace("/\&nbsp; /", "",$rec["betting_bet"]-$rec["betting_win"]-$rec["betting_rake"]-calculateBettingRake($rec,$GLOBALS["jur_groups_level"],1));',
                    'betting<br>cost nation'        => 'return ($GLOBALS["jur_groups_level"]=="nation" ? "<strong>" : "<span class=tip>" ).getBettingPercentage($rec["jps_perc_betting_n"],"type")."<br>".calculateBettingRake($rec,"nation",2);',
                    'betting<br>cost region'        => 'return ($GLOBALS["jur_groups_level"]=="region" ? "<strong>" : "<span class=tip>" ).getBettingPercentage($rec["jps_perc_betting_r"],"type")."<br>".calculateBettingRake($rec,"region",2);',
                    'betting<br>cost district'      => 'return ($GLOBALS["jur_groups_level"]=="district" ? "<strong>" : "<span class=tip>" ).getBettingPercentage($rec["jps_perc_betting_d"],"type")."<br>".calculateBettingRake($rec,"district",2);',
                    'betting<br>fin'                => 'return preg_replace("/\&nbsp; /", "",$rec["betting_fin"]);',
                    'betting live<br>bet'           => 'return preg_replace("/\&nbsp; /", "",$rec["betting_bet_live"]);',
                    'betting live<br>win'           => 'return preg_replace("/\&nbsp; /", "",$rec["betting_win_live"]);',
                    'betting live<br>cost club'     => 'return "<strong>".($GLOBALS["jur_groups_level"]=="club" ?  getBettingPercentage($rec["jps_perc_betting_c"],"type"): "")."<br> % = ".preg_replace("/\&nbsp; /", "",$rec["betting_rake_live"]);',
                    'betting live<br>net'           => 'return "<strong>".preg_replace("/\&nbsp; /", "",$rec["betting_bet_live"]-$rec["betting_win_live"]);',
                    'betting live<br>gross profit'  => 'return "<strong><br />".preg_replace("/\&nbsp; /", "",$rec["betting_bet_live"]-$rec["betting_win_live"]-$rec["betting_rake_live"]);',
                    'betting live<br>benefit'       => 'return "<strong><br />".preg_replace("/\&nbsp; /", "",$rec["betting_bet_live"]-$rec["betting_win_live"]-$rec["betting_rake_live"]-calculateLiveBettingRake($rec,$GLOBALS["jur_groups_level"],1));',
                    'betting live<br>cost nation'   => 'return ($GLOBALS["jur_groups_level"]=="nation" ? "<strong>" : "<span class=tip>" ).getBettingPercentage($rec["jps_perc_betting_n"],"type")."<br>".calculateLiveBettingRake($rec,"nation",2);',
                    'betting live<br>cost region'   => 'return ($GLOBALS["jur_groups_level"]=="region" ? "<strong>" : "<span class=tip>" ). getBettingPercentage($rec["jps_perc_betting_r"],"type")."<br>".calculateLiveBettingRake($rec,"region",2);',
                    'betting live<br>cost district' => 'return ($GLOBALS["jur_groups_level"]=="district" ? "<strong>" : "<span class=tip>" ).getBettingPercentage($rec["jps_perc_betting_d"],"type")."<br>".calculateLiveBettingRake($rec,"district",2);',
                    'betting live<br>fin'           => 'return preg_replace("/\&nbsp; /", "",$rec["betting_fin_live"]);',
                    'financial'                     => 'return preg_replace("/\&nbsp; /", "",$rec["fin"]);',
                    'Total Fin'                     => 'return "<strong>".preg_replace("/\&nbsp; /", "",calculateFin($rec));',
                    ' Total Rake '                  => 'return "<strong>".preg_replace("/\&nbsp; /", "",calculatePerc($rec))."<br>Total Perc = ".preg_replace("/\&nbsp; /","",calculateCurrentSubPerc($rec));',
                    'Total Net Betting'             => 'return "<strong>".preg_replace("/\&nbsp; /", "",calculatePercBetting($rec))."<br>% = ".preg_replace("/\&nbsp; /","",calculateCurrentSubPercBetting($rec));',
                    'Net<br><span class=tipWhite>(Total Fin-Total Perc)'                           => 'return preg_replace("/\&nbsp; /", "",calculateFin($rec)-((calculateCurrentSubPerc($rec))));',
                    'casino net'                   => 'return preg_replace("/\&nbsp; /", "",calculateRowNet($rec,"casino"));',
                    'nation net'                   => 'return preg_replace("/\&nbsp; /", "",calculateRowNet($rec,"nation"));',
                    'region net'                   => 'return preg_replace("/\&nbsp; /", "",calculateRowNet($rec,"region"));',
                    'district net'                 => 'return preg_replace("/\&nbsp; /", "",calculateRowNet($rec,"district"));'
                );

                $cols_arr = array();
                array_push($cols_arr, 'nation');
                $val_fmt_arr['nation']= 'return "<strong>" . $rec["n_jur_name"] . "</strong>"';
                if($canCangeCurency){
                    $val_fmt_arr['nation'].='.(($rec["currency"]!=$_SESSION["currency_id"] && $rec["currency"]!="")? "<br><span class=changeCurr style=\"font-weight:bold;color:red;cursor:pointer;\" data-curr=".$rec["currency"].">".$_SESSION["currencies"][$rec["currency"]]["symbol"]."</span>" :
                                            (($_SESSION["currency_id"]!=1)? "<br><span class=changeCurr style=\"font-weight:bold;color:red;cursor:pointer;\" data-curr=".$rec["currency"].">'.$_SESSION['currency_html'].'</span>" :"" ) );';
                }
                else{
                    $val_fmt_arr['nation'].=';';
                }
                $val_fmt_arr_excel['nation']= 'return "<strong>" . $rec["n_jur_name"] . "</strong>";';
                $chartGroup='n_jur_name';
                if($jur_groups_level=='region')
                {
                    array_push($cols_arr, 'region');
                    $val_fmt_arr['region']= 'return "<strong>" . $rec["r_jur_name"] . "</strong><br /> <span class=\"tip\"> % </span>";';
                    $val_fmt_arr_excel['region']= 'return "<strong>" . $rec["r_jur_name"] . "</strong><br /> <span class=\"tip\"> % </span>";';
                    $chartGroup='r_jur_name';
                }
                if($jur_groups_level=='district')
                {
                    array_push($cols_arr, 'region');
                    array_push($cols_arr, 'district');
                    $val_fmt_arr['region']= 'return "<strong>" . $rec["r_jur_name"] . "</strong>";';
                    $val_fmt_arr['district']='return "<strong>" . $rec["d_jur_name"] . "</strong><br /><span class=\"tip\"> % </span>";';
                    $val_fmt_arr_excel['region']= 'return "<strong>" . $rec["r_jur_name"] . "</strong>";';
                    $val_fmt_arr_excel['district']='return "<strong>" . $rec["d_jur_name"] . "</strong><br /><span class=\"tip\"> % </span>";';
                    $chartGroup='d_jur_name';
                }
                if($jur_groups_level=='club')
                {
                    array_push($cols_arr, 'region');
                    array_push($cols_arr, 'district');
                    array_push($cols_arr, 'club');
                    $val_fmt_arr['district'] ='return "<strong>" . $rec["d_jur_name"] . "</strong>";';
                    $val_fmt_arr['region'] = 'return "<strong>" . $rec["r_jur_name"] . "</strong>";';
                    $val_fmt_arr['club'] = 'return "<strong><a href=javascript:getCustomerDetails(\'$rec[jds_jur_club]\',\'$_POST[date_start]\',\'$_POST[date_end]\',\'$_POST[betwin]\',\'$_POST[financial]\','.$casino.','.$casino_live.','.$virtual.','.$poker2d.','.$pokerl.','.$royalpk.','.$playtech.','.$evolution.','.$infinity.','.$portomaso.','.$totem.','.$forex.','.$bankroulette.','.$zecchinetta.','.$greek.','.$zeus.');> $rec[c_jur_name] </a></strong><br /><span class=\"tip\"> % </span>";';
                    $val_fmt_arr_excel['district'] ='return "<strong>" . $rec["d_jur_name"] . "</strong>";';
                    $val_fmt_arr_excel['region'] = 'return "<strong>" . $rec["r_jur_name"] . "</strong>";';
                    $val_fmt_arr_excel['club'] = 'return "<strong>$rec[c_jur_name] </strong><br /><span class=\"tip\"> % </span>";';
                    $chartGroup='c_jur_name';
                }
                if($financial) {
                    array_push($cols_arr,'financial');
                }
                if($casino==1){
                    if($betwin)
                    {
                        array_push($cols_arr, 'casino<br>bet');
                        array_push($cols_arr, 'casino<br>win');
                    }
                    array_push($cols_arr,'casino<br>net');

                    //  array_push($cols_arr, 'casino<br>fin');
                }
                if($casino_live==1){
                    if($betwin)
                    {
                        array_push($cols_arr, 'casino<br>live bet');
                        array_push($cols_arr, 'casino<br>live win');
                    }
                    array_push($cols_arr,'casino<br>live net');
                }
                if($bankroulette==1){
                    if($betwin)
                    {
                        array_push($cols_arr, 'bankroulette<br>bet');
                        array_push($cols_arr, 'bankroulette<br>win');
                    }
                    array_push($cols_arr,'bankroulette<br>rake');
                    array_push($cols_arr, 'bankroulette<br>fin');
                }
                
                if($zeus==1){
                    if($betwin)
                    {
                        array_push($cols_arr, 'zeus<br>bet');
                        array_push($cols_arr, 'zeus<br>win');
                    }
                    array_push($cols_arr,'zeus<br>net');
                }

                if($evolution==1){
                    if($betwin)
                    {
                        array_push($cols_arr, 'evolution<br>bet');
                        array_push($cols_arr, 'evolution<br>win');
                    }
                    array_push($cols_arr,'evolution<br>net');

                }
                if($infinity==1){
                    if($betwin)
                    {
                        array_push($cols_arr, 'infinity<br>bet');
                        array_push($cols_arr, 'infinity<br>win');
                    }
                    array_push($cols_arr,'infinity<br>net');

                }
                if($portomaso==1){
                    if($betwin)
                    {
                        array_push($cols_arr, 'portomaso<br>bet');
                        array_push($cols_arr, 'portomaso<br>win');
                    }
                    array_push($cols_arr,'portomaso<br>net');
                    
                }


                if($poker2d==1){
                    if($betwin)
                    {
                        array_push($cols_arr, 'poker 2d<br>bet');
                        array_push($cols_arr, 'poker 2d<br>win');
                        array_push($cols_arr, 'poker 2d<br>net');
                    }
                    array_push($cols_arr,'poker 2d<br>rake');
                    array_push($cols_arr, 'poker 2d<br>fin');
                }

                if($betting==1){
                    if($betwin){
                        array_push($cols_arr,'betting<br>bet');
                        array_push($cols_arr,'betting<br>win');
                    }
                    array_push($cols_arr,'betting<br>net');
                    array_push($cols_arr,'betting<br>cost club');
                    array_push($cols_arr,'betting<br>gross profit');
                    array_push($cols_arr,'betting<br>benefit');
                    $cols_arr=array_merge($cols_arr,$betting_jurisdictions);
                    if($financial){
                        array_push($cols_arr,'betting<br>fin');
                    }
                    if($betwin){
                        array_push($cols_arr,'betting live<br>bet');
                        array_push($cols_arr,'betting live<br>win');
                    }
                    array_push($cols_arr,'betting live<br>net');
                    array_push($cols_arr,'betting live<br>cost club');
                    array_push($cols_arr,'betting live<br>gross profit');
                    array_push($cols_arr,'betting live<br>benefit');
                    $cols_arr=array_merge($cols_arr,$betting_jurisdictions_live);
                    if($financial){
                        array_push($cols_arr,'betting live<br>fin');
                    }
                }
                array_push($cols_arr,'Total Fin');
                array_push($cols_arr,' Total Rake ');
                array_push($cols_arr,'Net<br><span class=tipWhite>(Total Fin-Total Perc)');
                if($betting==1){
                    array_push($cols_arr,'Total Net Betting');
                }

                $totals = array();
                $totalsPerc = array();
                $totalsPercBetting = array();
                $diffCurr=false;
                $isDiffCur=false;
                $prevCur='';
                while($res->hasNext()){
                    $row = $res->next();
                    if($row['currency']!=$prevCur && $prevCur!==''){
                        $isDiffCur=true;
                    }
                    $prevCur = $row['currency'];

                    if($row["currency"]!=$_SESSION["currency_id"]){
                        $diffCurr=true;
                        foreach($row as $n=>$nv){
                            if(((strpos($n,'bet') !== false) || (strpos($n,'win') !== false) || (strpos($n,'rake') !== false) || (strpos($n,'fin') !== false))&&(strpos($n,'betting') === false) && (strpos($n,'jps_perc') === false)){
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
                    if($casino==1){
                        $totalsPerc['casino_rake']+=(($row["casino_rake"]/100)* ($row["jps_perc_casino"]));
                        $rake=$row['casino_rake']-$row["casino_rake"]/100*($row["jps_perc_casino"]);
                        $net=($row['casino_fin']-($row["casino_rake"]/100)* ($row["jps_perc_casino"]));
                        $casinoPerc=100-$row['jps_perc_casino_n'];
                        $nationPerc=$row['jps_perc_casino_n'];
                        $regionPerc=$row['jps_perc_casino_r'];
                        $districtPerc=$row['jps_perc_casino_d'];
                        $clubPerc=$row['jps_perc_casino_c'];
                        $totals['casino net']+=$net-($rake*(($nationPerc-$regionPerc)*100/(100-$regionPerc)))/100;
                        if($_SESSION['jurisdiction_class']=='nation'){
                            $totals['nation net']+=($rake*(($nationPerc-$regionPerc)*100/(100-$regionPerc)))/100;
                        }
                        else{
                            $totals['nation net']+=$net-($rake*(($regionPerc-$districtPerc)*100/(100-$districtPerc)))/100;
                        }
                        if($_SESSION['jurisdiction_class']=='region'){
                            $totals['region net']+=($rake*(($regionPerc-$districtPerc)*100/(100-$districtPerc)))/100;
                        }
                        else{
                            $totals['region net']+=$net-($rake*(($districtPerc-$clubPerc)*100/(100-$clubPerc)))/100;
                        }
                        $totals['district net']+=($rake*(($districtPerc-$clubPerc)*100/(100-$clubPerc)))/100;
                    }
                    if($casino_live==1){
                        $totalsPerc['casino_live_rake']+=$row["casino_live_rake"]/100* ($row["jps_perc_casino_live"]);
                        $net=($row['casino_live_fin']-($row["casino_live_rake"]/100)* ($row["jps_perc_casino_live"]));
                        $rake=$row['casino_live_rake']-$row["casino_live_rake"]/100*($row["jps_perc_casino_live"]);
                        $casinoPerc=100-$row['jps_perc_casino_live_n'];
                        $nationPerc=$row['jps_perc_casino_live_n'];
                        $regionPerc=$row['jps_perc_casino_live_r'];
                        $districtPerc=$row['jps_perc_casino_live_d'];
                        $clubPerc=$row['jps_perc_casino_live_c'];
                        $totals['casino net']+=$net-($rake*(($nationPerc-$regionPerc)*100/(100-$regionPerc)))/100;
                        if($_SESSION['jurisdiction_class']=='nation'){
                            $totals['nation net']+=($rake*(($nationPerc-$regionPerc)*100/(100-$regionPerc)))/100;
                        }
                        else{
                            $totals['nation net']+=$net-($rake*(($regionPerc-$districtPerc)*100/(100-$districtPerc)))/100;
                        }
                        if($_SESSION['jurisdiction_class']=='region'){
                            $totals['region net']+=($rake*(($regionPerc-$districtPerc)*100/(100-$districtPerc)))/100;
                        }
                        else{
                            $totals['region net']+=$net-($rake*(($districtPerc-$clubPerc)*100/(100-$clubPerc)))/100;
                        }
                        $totals['district net']+=($rake*(($districtPerc-$clubPerc)*100/(100-$clubPerc)))/100;
                    }
                    if($bankroulette==1){
                        $totalsPerc['casino_bankroulette_rake']+=(($row["casino_bankroulette_rake"]/100)* ($row["jps_perc_bankroulette"]));
                        $rake=$row['casino_bankroulette_rake']-$row["casino_bankroulette_rake"]/100*($row["jps_perc_bankroulette"]);
                        $net=($row['casino_bankroulette_fin']-($row["casino_bankroulette_rake"]/100)* ($row["jps_perc_bankroulette"]));
                        $casinoPerc=100-$row['jps_perc_bankroulette_n'];
                        $nationPerc=$row['jps_perc_bankroulette_n'];
                        $regionPerc=$row['jps_perc_bankroulette_r'];
                        $districtPerc=$row['jps_perc_bankroulette_d'];
                        $clubPerc=$row['jps_perc_bankroulette_c'];
                        $totals['casino net']+=$net-($rake*(($nationPerc-$regionPerc)*100/(100-$regionPerc)))/100;
                        if($_SESSION['jurisdiction_class']=='nation'){
                            $totals['nation net']+=($rake*(($nationPerc-$regionPerc)*100/(100-$regionPerc)))/100;
                        }
                        else{
                            $totals['nation net']+=$net-($rake*(($regionPerc-$districtPerc)*100/(100-$districtPerc)))/100;
                        }
                        if($_SESSION['jurisdiction_class']=='region'){
                            $totals['region net']+=($rake*(($regionPerc-$districtPerc)*100/(100-$districtPerc)))/100;
                        }
                        else{
                            $totals['region net']+=$net-($rake*(($districtPerc-$clubPerc)*100/(100-$clubPerc)))/100;
                        }
                        $totals['district net']+=($rake*(($districtPerc-$clubPerc)*100/(100-$clubPerc)))/100;
                    }

                    if($zeus==1){
                        $totalsPerc['zeus_rake']+=(($row["zeus_rake"]/100)* ($row["jps_perc_zeus"]));
                        $rake=$row['zeus_rake']-$row["zeus_rake"]/100*($row["jps_perc_zeus"]);
                        $net=($row['zeus_fin']-($row["zeus_rake"]/100)* ($row["jps_perc_zeus"]));
                        $casinoPerc=100-$row['jps_perc_zeus_n'];
                        $nationPerc=$row['jps_perc_zeus_n'];
                        $regionPerc=$row['jps_perc_zeus_r'];
                        $districtPerc=$row['jps_perc_zeus_d'];
                        $clubPerc=$row['jps_perc_zeus_c'];
                        $totals['casino net']+=$net-($rake*(($nationPerc-$regionPerc)*100/(100-$regionPerc)))/100;
                        if($_SESSION['jurisdiction_class']=='nation'){
                            $totals['nation net']+=($rake*(($nationPerc-$regionPerc)*100/(100-$regionPerc)))/100;
                        }
                        else{
                            $totals['nation net']+=$net-($rake*(($regionPerc-$districtPerc)*100/(100-$districtPerc)))/100;
                        }
                        if($_SESSION['jurisdiction_class']=='region'){
                            $totals['region net']+=($rake*(($regionPerc-$districtPerc)*100/(100-$districtPerc)))/100;
                        }
                        else{
                            $totals['region net']+=$net-($rake*(($districtPerc-$clubPerc)*100/(100-$clubPerc)))/100;
                        }
                        $totals['district net']+=($rake*(($districtPerc-$clubPerc)*100/(100-$clubPerc)))/100;
                    }
                    
                    if($evolution==1){
                        
                        $totalsPerc['evolution_rake']+=$row["evolution_rake"]/100* ($row["jps_perc_evolution"]);
                        $net=($row['evolution_fin']-($row["evolution_rake"]/100)* ($row["jps_perc_evolution"]));
                        $rake=$row['evolution_rake']-$row["evolution_rake"]/100*($row["jps_perc_evolution"]);
                        $casinoPerc=100-$row['jps_perc_evolution_n'];
                        $nationPerc=$row['jps_perc_evolution_n'];
                        $regionPerc=$row['jps_perc_evolution_r'];
                        $districtPerc=$row['jps_perc_evolution_d'];
                        $clubPerc=$row['jps_perc_evolution_c'];
                        $totals['casino net']+=$net-($rake*(($nationPerc-$regionPerc)*100/(100-$regionPerc)))/100;
                        if($_SESSION['jurisdiction_class']=='nation'){
                            $totals['nation net']+=($rake*(($nationPerc-$regionPerc)*100/(100-$regionPerc)))/100;
                        }
                        else{
                            $totals['nation net']+=$net-($rake*(($regionPerc-$districtPerc)*100/(100-$districtPerc)))/100;
                        }
                        if($_SESSION['jurisdiction_class']=='region'){
                            $totals['region net']+=($rake*(($regionPerc-$districtPerc)*100/(100-$districtPerc)))/100;
                        }
                        else{
                            $totals['region net']+=$net-($rake*(($districtPerc-$clubPerc)*100/(100-$clubPerc)))/100;
                        }
                        $totals['district net']+=($rake*(($districtPerc-$clubPerc)*100/(100-$clubPerc)))/100;
                        
                    }

                    if($infinity==1){

                        $totalsPerc['infinity_rake']+=$row["infinity_rake"]/100* ($row["jps_perc_infinity"]);
                        $net=($row['infinity_fin']-($row["infinity_rake"]/100)* ($row["jps_perc_infinity"]));
                        $rake=$row['infinity_rake']-$row["infinity_rake"]/100*($row["jps_perc_infinity"]);
                        $casinoPerc=100-$row['jps_perc_infinity_n'];
                        $nationPerc=$row['jps_perc_infinity_n'];
                        $regionPerc=$row['jps_perc_infinity_r'];
                        $districtPerc=$row['jps_perc_infinity_d'];
                        $clubPerc=$row['jps_perc_infinity_c'];
                        $totals['casino net']+=$net-($rake*(($nationPerc-$regionPerc)*100/(100-$regionPerc)))/100;
                        if($_SESSION['jurisdiction_class']=='nation'){
                            $totals['nation net']+=($rake*(($nationPerc-$regionPerc)*100/(100-$regionPerc)))/100;
                        }
                        else{
                            $totals['nation net']+=$net-($rake*(($regionPerc-$districtPerc)*100/(100-$districtPerc)))/100;
                        }
                        if($_SESSION['jurisdiction_class']=='region'){
                            $totals['region net']+=($rake*(($regionPerc-$districtPerc)*100/(100-$districtPerc)))/100;
                        }
                        else{
                            $totals['region net']+=$net-($rake*(($districtPerc-$clubPerc)*100/(100-$clubPerc)))/100;
                        }
                        $totals['district net']+=($rake*(($districtPerc-$clubPerc)*100/(100-$clubPerc)))/100;

                    }
                    
                    if($portomaso==1){
                        
                        $totalsPerc['portomaso_rake']+=$row["portomaso_rake"]/100* ($row["jps_perc_infinity"]);
                        $net=($row['portomaso_fin']-($row["portomaso_rake"]/100)* ($row["jps_perc_infinity"]));
                        $rake=$row['portomaso_rake']-$row["portomaso_rake"]/100*($row["jps_perc_infinity"]);
                        $casinoPerc=100-$row['jps_perc_infinity_n'];
                        $nationPerc=$row['jps_perc_infinity_n'];
                        $regionPerc=$row['jps_perc_infinity_r'];
                        $districtPerc=$row['jps_perc_infinity_d'];
                        $clubPerc=$row['jps_perc_infinity_c'];
                        $totals['casino net']+=$net-($rake*(($nationPerc-$regionPerc)*100/(100-$regionPerc)))/100;
                        if($_SESSION['jurisdiction_class']=='nation'){
                            $totals['nation net']+=($rake*(($nationPerc-$regionPerc)*100/(100-$regionPerc)))/100;
                        }
                        else{
                            $totals['nation net']+=$net-($rake*(($regionPerc-$districtPerc)*100/(100-$districtPerc)))/100;
                        }
                        if($_SESSION['jurisdiction_class']=='region'){
                            $totals['region net']+=($rake*(($regionPerc-$districtPerc)*100/(100-$districtPerc)))/100;
                        }
                        else{
                            $totals['region net']+=$net-($rake*(($districtPerc-$clubPerc)*100/(100-$clubPerc)))/100;
                        }
                        $totals['district net']+=($rake*(($districtPerc-$clubPerc)*100/(100-$clubPerc)))/100;
                        
                    }
                    
                    if($poker2d==1){
                        $totalsPerc['poker_2d_rake']+=$row["poker_2d_rake"]/100* ($row["jps_perc_poker_2d"]);
                        $net=($row['poker_2d_fin']-($row["poker_2d_rake"]/100)* ($row["jps_perc_poker_2d"]));
                        $rake=$row['poker_2d_rake']-$row["poker_2d_rake"]/100*($row["jps_perc_poker_2d"]);
                        $casinoPerc=100-$row['jps_perc_poker_2d_n'];
                        $nationPerc=$row['jps_perc_poker_2d_n'];
                        $regionPerc=$row['jps_perc_poker_2d_r'];
                        $districtPerc=$row['jps_perc_poker_2d_d'];
                        $clubPerc=$row['jps_perc_poker_2d_c'];
                        $totals['casino net']+=$net-($rake*(($nationPerc-$regionPerc)*100/(100-$regionPerc)))/100;
                        if($_SESSION['jurisdiction_class']=='nation'){
                            $totals['nation net']+=($rake*(($nationPerc-$regionPerc)*100/(100-$regionPerc)))/100;
                        }
                        else{
                            $totals['nation net']+=$net-($rake*(($regionPerc-$districtPerc)*100/(100-$districtPerc)))/100;
                        }
                        if($_SESSION['jurisdiction_class']=='region'){
                            $totals['region net']+=($rake*(($regionPerc-$districtPerc)*100/(100-$districtPerc)))/100;
                        }
                        else{
                            $totals['region net']+=$net-($rake*(($districtPerc-$clubPerc)*100/(100-$clubPerc)))/100;
                        }
                        $totals['district net']+=($rake*(($districtPerc-$clubPerc)*100/(100-$clubPerc)))/100;

                    }


                    foreach($row as $key => $val){
                        if(is_numeric($val)){
                            $totals[$key] += $val;
                        }
                    }
                }

                $totalsRakeSum=0;
                $totalsBetSum=0;
                $totalsWinSum=0;
                $totalsFinSum=0;
                $totalsBonusFin=0;
                if($casino==1){
                    $totalsRakeSum +=$totals['casino_rake'];
                    $totalsBetSum +=$totals['casino_bet'];
                    $totalsWinSum +=$totals['casino_win'];
                    $totalsFinSum +=$totals['casino_fin'];
                    $totalsBonusFin+=$totals['casino_fin_bonus'];
                }
                if($casino_live==1){
                    $totalsRakeSum +=$totals['casino_live_rake'];
                    $totalsBetSum  +=$totals['casino_live_bet'];
                    $totalsWinSum  +=$totals['casino_live_win'];
                    $totalsFinSum  +=$totals['casino_live_fin'];
                    $totalsBonusFin+=$totals['casino_live_fin_bonus'];

                }
                if($bankroulette==1){
                    $totalsRakeSum +=$totals['casino_bankroulette_rake'];
                    $totalsBetSum  +=$totals['casino_bankroulette_bet'];
                    $totalsWinSum  +=$totals['casino_bankroulette_win'];
                    $totalsFinSum  +=$totals['casino_bankroulette_fin'];
                    $totalsBonusFin+=$totals['casino_bankroulette_fin_bonus'];
                }
                if($zeus==1){
                    $totalsRakeSum +=$totals['zeus_rake'];
                    $totalsBetSum  +=$totals['zeus_bet'];
                    $totalsWinSum  +=$totals['zeus_win'];
                    $totalsFinSum  +=$totals['zeus_fin'];
                }
                if($evolution==1){
                    $totalsRakeSum +=$totals['evolution_rake'];
                    $totalsBetSum  +=$totals['evolution_bet'];
                    $totalsWinSum  +=$totals['evolution_win'];
                    $totalsFinSum  +=$totals['evolution_fin'];
                    $totalsBonusFin+=$totals['evolution_fin_bonus'];
                }

                if($infinity==1){
                    $totalsRakeSum +=$totals['infinity_rake'];
                    $totalsBetSum  +=$totals['infinity_bet'];
                    $totalsWinSum  +=$totals['infinity_win'];
                    $totalsFinSum  +=$totals['infinity_fin'];
                    $totalsBonusFin+=$totals['infinity_fin_bonus'];
                }
                if($portomaso==1){
                    $totalsRakeSum +=$totals['portomaso_rake'];
                    $totalsBetSum  +=$totals['portomaso_bet'];
                    $totalsWinSum  +=$totals['portomaso_win'];
                    $totalsFinSum  +=$totals['portomaso_fin'];
                    $totalsBonusFin+=$totals['portomaso_fin_bonus'];
                }
                if($poker2d==1){
                    $totalsRakeSum +=$totals['poker_2d_rake'];
                    $totalsBetSum +=$totals['poker_2d_bet'];
                    $totalsWinSum +=$totals['poker_2d_win'];
                    $totalsNetSum +=$totals['poker_2d_net'];
                    $totalsFinSum +=$totals['poker_2d_fin'];
                    $totalsBonusFin+=$totals['poker_2d_fin_bonus'];
                }
                if($betting==1){
                    $totalsRakeSum +=($totals['betting_bet']-$totals['betting_win']);
                    $totalsBetSum +=$totals['betting_bet'];
                    $totalsWinSum +=$totals['betting_win'];
                }
                if($bettingl==1){
                    $totalsRakeSum +=($totals['betting_bet_live']-$totals['betting_win_live']);
                    $totalsBetSum +=$totals['betting_bet_live'];
                    $totalsWinSum +=$totals['betting_win_live'];
                }

                ?>
                <div class="tip fleft"><?=$lang->getLang("Time taken to execute your request")?>: <?=$duration?></div>
                <center>
                <br><br><br>
                <div class="bodyHD"><?=$lang->getLang("Total")?></div>
                <table bgcolor="#a99a9" cellpadding="4" cellspacing="1">
                    <tr>
                        <?if($diffCurr){ ?>
                            <td class="label centered"><?=$lang->getLang("Currency")?></td>
                        <? } ?>
                        <td class='label centered'><?=$lang->getLang("Closure")?></td>
                        <?if($financial){ ?>
                            <td class="label centered"><?=$lang->getLang("Financial")?></td>
                        <? } ?>
                        <?php
                        if($betwin)
                        {
                            ?>
                            <td class="label centered"><?=$lang->getLang("Bet")?></td>
                            <td class="label centered"><?=$lang->getLang("Win")?></td>
                        <?php
                        }
                        ?>
                        <?if($bonus_financial) { ?>
                            <td class="label centered"><?=$lang->getLang("Total bonus")?></td>
                        <?}?>
                        <td class="label centered"><?=$lang->getLang("Total Fin")?></td>
                        <td class="label centered"><?=$lang->getLang("Total Rake")?></td>
                        <td class="label centered"><?=$lang->getLang("Net")?><br><span class="tipWhite">(Total Fin-Total Perc)</span></td>
                    </tr>
                    <tr>
                        <?if($diffCurr){ ?>
                            <td class="content centered"><button class="entitybtn" id="toggleAllCurrency"><?=$lang->getLang("Toggle All")?></button></td>
                        <? } ?>
                        <td class='content'><a href="/search_closure/<?=$date_start?>/<?=$date_end?>"><?=$lang->getLang("View closure")?></a></td>
                        <?if($financial){?>
                            <td class="content" style="text-align: right"><?=getInDollars($totals["fin"])?></td>
                        <? } ?>
                        <?php
                        if($betwin)
                        {
                            ?>
                            <td class="content" style="text-align: right">
                                <?=getInDollars($totalsBetSum)?></td>
                            <td class="content" style="text-align: right">
                                <?=getInDollars($totalsWinSum)?></td>
                        <?php
                        }
                        ?>
                        <?if($bonus_financial) { ?>
                            <td class="content" style="text-align: right"><?=getInDollars($totalsBonusFin )?><br></td>
                        <?}?>
                        <td class="content" style="text-align: right">
                            <?=getInDollars($totalsFinSum )?><br>
                        </td>
                        <td class="content" style="text-align: right">
                            <?=getInDollars($totalsRakeSum)?><br>
                            Total Perc = <?=getInDollars(array_sum($totalsPerc))?>
                        </td>

                        <td class="content" style="text-align: right">
                            <?=getInDollars($totalsFinSum-((array_sum($totalsPerc))) )?><br>
                        </td>

                    </tr>
                </table>
<!-- partial totals -->
                <br>
                <div class="bodyHD"><?=$lang->getLang("Partial Totals")?></div>
                <table bgcolor="#a99a9" cellpadding="4" cellspacing="1">
                <tr>
                <?php if(!$isDiffCur && $_SESSION['currency_id']!=$prevCur){ ?>
                    <td class="label centered"><?=$lang->getLang("Currency")?> </td>
                <? } ?>
                    <?if($financial) { ?>
                        <td class="label centered"><?=$lang->getLang("Financial")?> </td>
                    <? } ?>
                    <?php if($casino==1){
                        if($betwin)
                        {?>
                            <td class="label centered">Casino<br><?=$lang->getLang("Bet")?></td>
                            <td class="label centered">Casino<br><?=$lang->getLang("Win")?></td>
                        <?php }  ?>
                        <td class="label centered">Casino<br><?=$lang->getLang("Net")?></td>

                        <?if($bonus_financial) { ?>
                            <td class="label centered">Casino<br><?=$lang->getLang("Bonus Fin")?></td>
                        <?}?>
                    <?php  } ?>

                    <?php if($casino_live==1){
                        if($betwin)
                        {?>
                            <td class="label centered">Casino<br>Live <?=$lang->getLang("Bet")?></td>
                            <td class="label centered">Casino<br>Live <?=$lang->getLang("Win")?></td>
                        <?php }
                        ?>
                        <td class="label centered">Casino<br>Live <?=$lang->getLang("Net")?></td>
                        <?if($bonus_financial) { ?>
                            <td class="label centered">Casino Live<br><?=$lang->getLang("Bonus Fin")?></td>
                        <?}?>

                    <?php }   ?>
                    <?php if($virtual==1){
                        if( $betwin)
                        {?>
                            <td class="label centered">Virtual<br><?=$lang->getLang("Bet")?></td>
                            <td class="label centered">Virtual<br><?=$lang->getLang("Win")?></td>
                        <?php }
                        ?>
                        <td class="label centered">Virtual<br><?=$lang->getLang("Net")?></td>
                        <?if($bonus_financial) { ?>
                            <td class="label centered">Virtual <br><?=$lang->getLang("Bonus Fin")?></td>
                        <?}?>
                    <?php }  ?>

                    <?php if($bankroulette==1){
                        if($betwin)
                        {?>
                            <td class="label centered">Bankroulette<br><?=$lang->getLang("Bet")?></td>
                            <td class="label centered">Bankroulette<br><?=$lang->getLang("Win")?></td>
                        <?php }  ?>
                        <td class="label centered">Bankroulette<br><?=$lang->getLang("Rake")?></td>
                        <td class="label centered">Bankroulette<br>Fin</td>

                        <?if($bonus_financial) { ?>
                            <td class="label centered">Bankroulette<br><?=$lang->getLang("Bonus Fin")?></td>
                        <?}?>
                    <?php  } ?>

                    <?php if($zecchinetta==1){
                        if($betwin)
                        {?>
                            <td class="label centered">Zecchinetta<br><?=$lang->getLang("Bet")?></td>
                            <td class="label centered">Zecchinetta<br><?=$lang->getLang("Win")?></td>
                        <?php }  ?>
                        <td class="label centered">Zecchinetta<br><?=$lang->getLang("Rake")?></td>
                        <td class="label centered">Zecchinetta<br>Fin</td>

                        <?if($bonus_financial) { ?>
                            <td class="label centered">Zecchinetta<br><?=$lang->getLang("Bonus Fin")?></td>
                        <?}?>
                    <?php  } ?>

                    <?php if($greek==1){
                        if($betwin)
                        {?>
                            <td class="label centered">Greek<br><?=$lang->getLang("Bet")?></td>
                            <td class="label centered">Greek<br><?=$lang->getLang("Win")?></td>
                        <?php }  ?>
                        <td class="label centered">Greek<br><?=$lang->getLang("Net")?></td>

                        <?if($bonus_financial) { ?>
                            <td class="label centered">Greek<br><?=$lang->getLang("Bonus Fin")?></td>
                        <?}?>
                    <?php  } ?>

                    <?php if($zeus==1){
                        if($betwin)
                        {?>
                            <td class="label centered">Zeus<br><?=$lang->getLang("Bet")?></td>
                            <td class="label centered">Zeus<br><?=$lang->getLang("Win")?></td>
                        <?php }  ?>
                        <td class="label centered">Zeus<br><?=$lang->getLang("Net")?></td>

                        <?if($bonus_financial) { ?>
                            <td class="label centered">Zeus<br><?=$lang->getLang("Bonus Fin")?></td>
                        <?}?>
                    <?php  } ?>
                    
                    <?php if($royalpk==1) {
                        if( $betwin)
                        {?>
                            <td class="label centered">RoyalPk<br><?=$lang->getLang("Bet")?></td>
                            <td class="label centered">RoyalPk<br><?=$lang->getLang("Win")?></td>
                        <?php }
                        ?>
                        <td class="label centered">Royalpk<br><?=$lang->getLang("Net")?></td>
                        <?if($bonus_financial) { ?>
                            <td class="label centered">Royalpk <br><?=$lang->getLang("Bonus Fin")?></td>
                        <?}?>
                    <?php }  ?>

                    <?php if($playtech==1){
                        if( $betwin)
                        {?>
                            <td class="label centered">Playt./SkyWind<br><?=$lang->getLang("Bet")?></td>
                            <td class="label centered">Playt./SkyWind<br><?=$lang->getLang("Win")?></td>
                        <?php }
                        ?>
                        <td class="label centered">Playt./SkyWind<br><?=$lang->getLang("Net")?></td>
                        <?if($bonus_financial) { ?>
                            <td class="label centered">Playt./SkyWind <br><?=$lang->getLang("Bonus Fin")?></td>
                        <?}?>
                    <?php }  ?>

                    <?php if($evolution==1){
                        if( $betwin)
                        {?>
                            <td class="label centered">Evolution<br><?=$lang->getLang("Bet")?></td>
                            <td class="label centered">Evolution<br><?=$lang->getLang("Win")?></td>
                        <?php }
                        ?>
                        <td class="label centered">Evolution<br><?=$lang->getLang("Net")?></td>
                        <?if($bonus_financial) { ?>
                            <td class="label centered">Evolution <br><?=$lang->getLang("Bonus Fin")?></td>
                        <?}?>
                    <?php }  ?>

                    <?php if($infinity==1){
                        if( $betwin)
                        {?>
                            <td class="label centered">Infinity<br><?=$lang->getLang("Bet")?></td>
                            <td class="label centered">Infinity<br><?=$lang->getLang("Win")?></td>
                        <?php }
                        ?>
                        <td class="label centered">Infinity<br><?=$lang->getLang("Net")?></td>
                        <?if($bonus_financial) { ?>
                            <td class="label centered">Infinity<br><?=$lang->getLang("Bonus Fin")?></td>
                        <?}?>
                    <?php }  ?>
                    <?php if($portomaso==1){
                        if( $betwin)
                        {?>
                            <td class="label centered">Portomaso<br><?=$lang->getLang("Bet")?></td>
                            <td class="label centered">Portomaso<br><?=$lang->getLang("Win")?></td>
                        <?php }
                        ?>
                        <td class="label centered">Portomaso<br><?=$lang->getLang("Net")?></td>
                        <?if($bonus_financial) { ?>
                            <td class="label centered">Portomaso<br><?=$lang->getLang("Bonus Fin")?></td>
                        <?}?>
                    <?php }  ?>
                    <?php if($forex==1){
                        if( $betwin)
                        {?>
                            <td class="label centered">Forex<br><?=$lang->getLang("Bet")?></td>
                            <td class="label centered">Forex<br><?=$lang->getLang("Win")?></td>
                        <?php }
                        ?>
                        <td class="label centered">Forex<br><?=$lang->getLang("Net")?></td>
                        <?if($bonus_financial) { ?>
                            <td class="label centered">Forex <br><?=$lang->getLang("Bonus Fin")?></td>
                        <?}?>
                    <?php }  ?>

                    <?php if($totem==1){
                        if( $betwin)
                        {?>
                            <td class="label centered">Totem<br><?=$lang->getLang("Bet")?></td>
                            <td class="label centered">Totem<br><?=$lang->getLang("Win")?></td>
                        <?php }
                        ?>
                        <td class="label centered">Totem<br><?=$lang->getLang("Net")?></td>
                        <?if($bonus_financial) { ?>
                            <td class="label centered">Totem <br><?=$lang->getLang("Bonus Fin")?></td>
                        <?}?>
                    <?php }  ?>

                    <?php if($pokerl==1){
                        if($betwin)
                        { ?>
                            <td class="label centered">Poker Live<br><?=$lang->getLang("Bet")?></td>
                            <td class="label centered">Poker Live<br><?=$lang->getLang("Win")?></td>
                        <?php }
                        ?>
                        <td class="label centered">Poker Live<br><?=$lang->getLang("Rake")?></td>
                        <td class="label centered">Poker Live<br>Fin</td>
                        <?if($bonus_financial) { ?>
                            <td class="label centered">Poker Live <br><?=$lang->getLang("Bonus Fin")?></td>
                        <?}?>
                    <?php
                    }
                    if($poker2d==1){
                        if($betwin)
                        {?>
                            <td class="label centered">Poker 2d<br><?=$lang->getLang("Bet")?></td>
                            <td class="label centered">Poker 2d<br><?=$lang->getLang("Win")?></td>
                            <td class="label centered">Poker 2d<br><?=$lang->getLang("Net")?></td>
                        <?php }
                        ?>
                        <td class="label centered">Poker 2d<br><?=$lang->getLang("Rake")?></td>
                        <td class="label centered">Poker 2d<br>Fin</td>
                        <?if($bonus_financial) { ?>
                            <td class="label centered">Poker 2d <br><?=$lang->getLang("Bonus Fin")?></td>
                        <?}?>
                    <?php
                    }
                    ?>
                    <td class="label centered">Total Fin</td>
                    <td class="label centered">Total Rake</td>
                    <td class="label centered"><?=$lang->getLang("Net")?> <br><span class="tipWhite">(Total Fin-Total Perc)</span></td>
                    <? if($bettingl==1){ ?>
                        <td class="label centered"><?=$lang->getLang("Total Net Betting")?> </td>
                    <? } ?>

                    <?
                    if($_SESSION['jurisdiction_class']=='nation' && $jur_groups_level=='region')
                    {?>
                        <td class="label centered" style="text-align: right">
                            <?=$lang->getLang('Casino net')?><br>
                        </td>
                        <td class="label centered" style="text-align: right">
                            <?=$lang->getLang('Nation net')?><br>
                        </td>
                    <?
                    }
                    if($_SESSION['jurisdiction_class']=='region'&& $jur_groups_level=='district')
                    { ?>
                        <td class="label centered" style="text-align: right">
                            <?=$lang->getLang('Nation net')?><br>
                        </td>
                        <td class="label centered" style="text-align: right">
                            <?=$lang->getLang('Region net')?><br>
                        </td>
                    <?
                    }
                    if($_SESSION['jurisdiction_class']=='district')
                    { ?>
                        <td class="label centered" style="text-align: right">
                            <?=$lang->getLang('Region net')?><br>
                        </td>
                        <td class="label centered" style="text-align: right">
                            <?=$lang->getLang('District net')?><br>
                        </td>
                    <?
                    }
                    ?>

                </tr>
                <tr class='odd' style="vertical-align: top">
                    <?php if(!$isDiffCur && $_SESSION['currency_id']!=$prevCur){ ?>
                    <td class="centered">
                        <span class=changeCurr style="font-weight:bold;color:red;cursor:pointer;" data-curr="<?=$prevCur?>" ><?=$_SESSION["currencies"][$prevCur]["symbol"]?></span></td>
                <? }?>
                <?if($financial) { ?>
                        <td class="content dark" style="text-align: right"><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["fin"]))?></td>
                    <? } ?>
                    <?php if($casino==1){
                        if($betwin)
                        {?>
                            <td class="content light" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["casino_bet"]))?></strong></td>
                            <td class="content light" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["casino_win"]))?></strong></td>
                        <?php }?>
                        <td class="content light" style="text-align: right">
                            <?=($casino!=1? "<span class=tip>".$lang->getLang('betting club')."</span>" : "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($totals["casino_rake"])) ."<br>
                % =".preg_replace("/\&nbsp; /", "",getInDollars($totalsPerc["casino_rake"])))?>
                        </td>
                        <?if($bonus_financial) { ?>
                            <td class="content light"><?=getInDollars($totalsPerc["casino_fin_bonus"])?></td>
                        <?}?>
                    <?php }  ?>
                    <?php if($casino_live==1){
                        if($betwin)
                        {?>
                            <td class="content dark" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["casino_live_bet"]))?></td>
                            <td class="content dark" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["casino_live_win"]))?></td>
                        <?php }?>
                        <td class="content dark" style="text-align: right">
                            <?=($casino_live!=1? "<span class=tip>".$lang->getLang('Excluded')."</span>" : "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($totals["casino_live_rake"]))."<br>
                % =".preg_replace("/\&nbsp; /", "",getInDollars($totalsPerc["casino_live_rake"])))?>
                        </td>
                        <?if($bonus_financial) { ?>
                            <td class="content dark"><?=getInDollars($totalsPerc["casino_live_fin_bonus"])?></td>
                        <?}?>
                    <?php }  ?>
                    <?php if($virtual==1){
                        if($betwin)
                        {?>
                            <td class="content light" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["virtual_bet"]))?></td>
                            <td class="content light" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["virtual_win"]))?></td>
                        <?php }?>
                        <td class="content light" style="text-align: right">
                            <?=($virtual!=1? "<span class=tip>".$lang->getLang('Excluded')."</span>" : "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($totals["virtual_rake"]))."<br>
                % =".preg_replace("/\&nbsp; /", "",getInDollars($totalsPerc["virtual_rake"])))?>
                        </td>
                        <?if($bonus_financial) { ?>
                            <td class="content light"><?=getInDollars($totalsPerc["casino_fin_bonus"])?></td>
                        <?}?>
                    <?php } ?>

                    <?php if($bankroulette==1){
                        if($betwin)
                        {?>
                            <td class="content light" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["casino_bankroulette_bet"]))?></strong></td>
                            <td class="content light" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["casino_bankroulette_win"]))?></strong></td>
                        <?php }?>
                        <td class="content light" style="text-align: right">
                            <?=($bankroulette!=1? "<span class=tip>".$lang->getLang('Excluded')."</span>" : "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($totals["casino_bankroulette_rake"])) ."<br>
                % =".preg_replace("/\&nbsp; /", "",getInDollars($totalsPerc["casino_bankroulette_rake"])))?>
                        </td>
                        <td class="content light" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["casino_bankroulette_fin"]))?></td>
                        <?if($bonus_financial) { ?>
                            <td class="content light"><?=getInDollars($totalsPerc["casino_bankroulette_fin_bonus"])?></td>
                        <?}?>
                    <?php }  ?>

                    <?php if($zeus==1){
                        if($betwin)
                        {?>
                            <td class="content light" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["zeus_bet"]))?></strong></td>
                            <td class="content light" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["zeus_win"]))?></strong></td>
                        <?php }?>
                        <td class="content light" style="text-align: right">
                            <?=($zeus!=1? "<span class=tip>".$lang->getLang('Excluded')."</span>" : "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($totals["zeus_rake"])) ."<br>
                % =".preg_replace("/\&nbsp; /", "",getInDollars($totalsPerc["zeus_rake"])))?>
                        </td>
                    <?php }  ?>


                    <?php if($evolution==1){
                        if($betwin)
                        {?>
                            <td class="content dark" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["evolution_bet"]))?></td>
                            <td class="content dark" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["evolution_win"]))?></td>
                        <?php }?>
                        <td class="content dark" style="text-align: right">
                            <?=($evolution!=1? "<span class=tip>".$lang->getLang('Excluded')."</span>" : "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($totals["evolution_rake"]))."<br>
                % =".preg_replace("/\&nbsp; /", "",getInDollars($totalsPerc["evolution_rake"])))?>
                        </td>
                        <?if($bonus_financial) { ?>
                            <td class="content dark"><?=getInDollars($totalsPerc["evolution_fin_bonus"])?></td>
                        <?}?>
                    <?php } ?>

                    <?php if($infinity==1){
                        if($betwin)
                        {?>
                            <td class="content dark" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["infinity_bet"]))?></td>
                            <td class="content dark" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["infinity_win"]))?></td>
                        <?php }?>
                        <td class="content dark" style="text-align: right">
                            <?=($infinity!=1? "<span class=tip>".$lang->getLang('Excluded')."</span>" : "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($totals["infinity_rake"]))."<br>
                % =".preg_replace("/\&nbsp; /", "",getInDollars($totalsPerc["infinity_rake"])))?>
                        </td>
                        <?if($bonus_financial) { ?>
                            <td class="content dark"><?=getInDollars($totalsPerc["infinity_fin_bonus"])?></td>
                        <?}?>
                    <?php } ?>
                    <?php if($portomaso==1){
                        if($betwin)
                        {?>
                            <td class="content dark" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["portomaso_bet"]))?></td>
                            <td class="content dark" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["portomaso_win"]))?></td>
                        <?php }?>
                        <td class="content dark" style="text-align: right">
                            <?=($portomaso!=1? "<span class=tip>".$lang->getLang('Excluded')."</span>" : "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($totals["portomaso_rake"]))."<br>
                % =".preg_replace("/\&nbsp; /", "",getInDollars($totalsPerc["portomaso_rake"])))?>
                        </td>
                        <?if($bonus_financial) { ?>
                            <td class="content dark"><?=getInDollars($totalsPerc["portomaso_fin_bonus"])?></td>
                        <?}?>
                    <?php } ?>
                    <?php if($forex==1){
                        if($betwin)
                        {?>
                            <td class="content dark" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["forex_bet"]))?></td>
                            <td class="content dark" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["forex_win"]))?></td>
                        <?php }?>
                        <td class="content dark" style="text-align: right">
                            <?=($forex!=1? "<span class=tip>".$lang->getLang('Excluded')."</span>" : "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($totals["forex_rake"]))."<br>
                % =".preg_replace("/\&nbsp; /", "",getInDollars($totalsPerc["forex_rake"])))?>
                        </td>
                        <?if($bonus_financial) { ?>
                            <td class="content dark"><?=getInDollars($totalsPerc["forex_fin_bonus"])?></td>
                        <?}?>
                    <?php } ?>

                    <?php if($totem==1){
                        if($betwin)
                        {?>
                            <td class="content dark" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["totem_bet"]))?></td>
                            <td class="content dark" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["totem_win"]))?></td>
                        <?php }?>
                        <td class="content dark" style="text-align: right">
                            <?=($totem!=1? "<span class=tip>".$lang->getLang('Excluded')."</span>" : "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($totals["totem_rake"]))."<br>
                % =".preg_replace("/\&nbsp; /", "",getInDollars($totalsPerc["totem_rake"])))?>
                        </td>
                        <?if($bonus_financial) { ?>
                            <td class="content dark"><?=getInDollars($totalsPerc["totem_fin_bonus"])?></td>
                        <?}?>
                    <?php } ?>


                    <?php if($pokerl==1){
                        if($betwin)
                        { ?>
                            <td class="content light" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["poker_live_bet"]))?></td>
                            <td class="content light" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["poker_live_win"]))?></td>
                        <?php }?>
                        <td class="content light" style="text-align: right">
                            <?=($pokerl!=1? "<span class=tip>".$lang->getLang('Excluded')."</span>" : "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($totals["poker_live_rake"]))."<br>
                % = ".preg_replace("/\&nbsp; /", "",getInDollars($totalsPerc["poker_live_rake"])))?>
                        </td>
                        <td class="content light" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["poker_live_fin"]))?></td>
                        <?if($bonus_financial) { ?>
                            <td class="content light"><?=getInDollars($totalsPerc["poker_live_fin_bonus"])?></td>
                        <?}?>
                    <?php }
                    if($poker2d==1){

                        if($betwin)
                        {?>
                            <td class="content light" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["poker_2d_bet"]))?></td>
                            <td class="content light" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["poker_2d_win"]))?></td>
                            <td class="content light" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["poker_2d_net"]))?></td>
                        <?php }?>
                        <td class="content light" style="text-align: right">
                            <?=($poker2d!=1? "<span class=tip>".$lang->getLang('Excluded')."</span>" : "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($totals["poker_2d_rake"]))."<br>
                % = ".preg_replace("/\&nbsp; /", "",getInDollars($totalsPerc["poker_2d_rake"])))?>
                        </td>
                        <td class="content light" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["poker_2d_fin"]))?></td>
                        <?if($bonus_financial) { ?>
                            <td class="content light"><?=getInDollars($totalsPerc["poker_2d_fin_bonus"])?></td>
                        <?}?>
                    <?php } ?>

                    <td class="content dark" style="text-align: right" ><strong><?=getInDollars($totalsFinSum)?><br>
                    </td>
                    <td class="content dark" style="text-align: right" ><strong><?=getInDollars($totalsRakeSum)?><br>
                            Total Perc = <?=getInDollars(array_sum($totalsPerc))?>
                    </td>
                    <td class="content light" style="text-align: right" ><strong><?=getInDollars($totalsFinSum-(array_sum($totalsPerc)) )?><br />


                    </td>
                    <?if($betting==1){ ?>
                        <td class="content centered"><strong><?=getInDollars((array_sum($totalsPercBetting)))?> </strong></td>
                    <? } ?>

                    <?
                    if($_SESSION['jurisdiction_class']=='nation'&& $jur_groups_level=='region')
                    {?>
                        <td class="content centered" style="text-align: right">
                            <?=getInDollars($totals['casino net'])?><br>
                        </td>
                        <td class="content centered" style="text-align: right">
                            <?=getInDollars($totals['nation net'])?><br>
                        </td>

                    <?
                    }
                    if($_SESSION['jurisdiction_class']=='region'&& $jur_groups_level=='district')
                    { ?>
                        <td class="content centered" style="text-align: right">
                            <?=getInDollars($totals['nation net'])?><br>
                        </td>
                        <td class="content centered" style="text-align: right">
                            <?=getInDollars($totals['region net'])?><br>
                        </td>

                    <?
                    }
                    if($_SESSION['jurisdiction_class']=='district')
                    { ?>
                        <td class="content centered" style="text-align: right">
                            <?=getInDollars($totals['region net'])?><br>
                        </td>
                        <td class="content centered" style="text-align: right">
                            <?=getInDollars($totals['district net'])?><br>
                        </td>
                    <?
                    }
                    ?>
                </tr>
                </table>
                <br>
				
                <!-- end partial totals  -->
                <?php
                $casino= (!isset($_POST['doQuery']) || $_POST['casino']==1 || $active!='casino') ? 1 : 0;
                $casino_live= (!isset($_POST['doQuery']) || $_POST['casino_live']==1 || $active!='casino') ? 1 : 0;
                $virtual= (!isset($_POST['doQuery']) || $_POST['virtual']==1 || $active!='casino') ? 1 : 0;
                $bankroulette= (!isset($_POST['doQuery']) || $_POST['bankroulette']==1 || $active!='casino') ? 1 : 0;
                $zeus= (!isset($_POST['doQuery']) || $_POST['zeus']==1 || $active!='casino') ? 1 : 0;
                $evolutionh= (!isset($_POST['doQuery']) || $_POST['evolution']==1 || $active!='casino') ? 1 : 0;
                $infinity= (!isset($_POST['doQuery']) || $_POST['infinity']==1 || $active!='casino') ? 1 : 0;
                $portomaso= (!isset($_POST['doQuery']) || $_POST['portomaso']==1 || $active!='casino') ? 1 : 0;
                $poker2d= (!isset($_POST['doQuery']) || $_POST['poker2d']==1 || $active!='casino') ? 1 : 0;
                
                
                $zecchinetta= 0;//(!isset($_POST['doQuery']) || $_POST['zecchinetta']==1 || $active!='casino') ? 1 : 0;
                $greek= 0;//(!isset($_POST['doQuery']) || $_POST['greek']==1 || $active!='casino') ? 1 : 0;
                $royalpk= 0;//(!isset($_POST['doQuery']) || $_POST['royalpk']==1 || $active!='casino') ? 1 : 0;
                $playtech= 0;//(!isset($_POST['doQuery']) || $_POST['playtech']==1 || $active!='casino') ? 1 : 0;
                $forex= 0;//(!isset($_POST['doQuery']) || $_POST['forex']==1 || $active!='casino') ? 1 : 0;
                $pokerl= 0;//(!isset($_POST['doQuery']) || $_POST['pokerl']==1 || $active!='casino') ? 1 : 0;
                $totem= 0;//(!isset($_POST['doQuery']) || $_POST['totem']==1 || $active!='casino') ? 1 : 0;
                $post_vars = array('active'=>'casino',
                                    'doQuery'=>'yes',
                                    'date_start'=>$date_start,
                                    'date_end'=>$date_end,
                                    'jurisdiction_level'=>$jurisdiction_level,
                                    'jurisdiction'=>$jur_sel_id,
                                    'jur_level'=>$jur_groups_level,
                                    'casino'=>$casino,
                                    'casino_live'=>$casino_live,
                                    'bankroulette'=>$bankroulette,
                                    'zecchinetta'=>$zecchinetta,
                                    'greek'=>$greek,
                                    'virtual'=>$virtual,
                                    'royalpk'=>$royalpk,
                                    'playtech'=>$playtech,
                                    'evolution'=>$evolution,
                                    'infinity'=>$infinity,
                                    'portomaso'=>$portomaso,
                                    'forex'=>$forex,
                                    'totem' =>$totem,
                                    'pokerl'=>$pokerl,
                                    'poker2d'=>$poker2d,
                                    'betwin'=>$betwin,
                                    'financial'=>$financial,
                                    'resforpage'=>$row_to_page
                );
                $excel_filename = 'Financial for '.$date_start.' - '.$date_end.'.xls';
                $start_row = (isset ( $_POST ['start_row'] )) ? $_POST ['start_row'] : 1;
               // $pdf_filename=md5($_SESSION['aus_username']).".pdf";
               // $pdfHeader='<h2 class="red">'.$lang->getLang("Financial summary").' '.$lang->getLang("for").' '.$date_start.' - '.$date_end."   </h2>";
                $qp = new QueryPrinter($res);
                var_dump($sum_fmt_arr);
                $qp->printTable($res,$cols_arr, $post_vars, $val_fmt_arr, $cell_fmt_arr, $sum_fmt_arr, $excel_filename, $header, $width="", $start_row, $row_to_page,false,"",$pdf_filename);
                $qp->dumpToExcelFile($res,$cols_arr,$val_fmt_arr_excel,$excel_filename);
                //$qp->printPdf($res,$cols_arr,$val_fmt_arr,$pdf_filename,$pdfHeader);

                ?>
                </center>
                <div class="clearfix"></div>
                <div id="graphs" ></div><br>
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
                    if($bankroulette==1){
                        array_push($totalChartArray,'casino_bankroulette_rake');
                    }
                    if($zeus==1){
                        array_push($totalChartArray,'zeus_rake');
                    }
                    if($evolution==1){
                        array_push($totalChartArray,'evolution_rake');
                    }
                    if($infinity==1){
                        array_push($totalChartArray,'infinity_rake');
                    }
                    if($portomaso==1){
                        array_push($totalChartArray,'portomaso_rake');
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
                    <div class="clearfix"></div>
                    <div style="width:1400px;">
                        <div id="graphs_casino" style='float:left;width:30%;margin-left:5%;'></div>
                        <div id="graphs_casino_live" style='float:left;width:30%;'></div>
                        <div id="graphs_virtual" style='float:left;width:30%;'></div>
                        <div id="graphs_royalpk" style='float:left;width:30%; margin-left:5%;'></div>
                        <div id="graphs_playtech" style='float:left;width:30%; margin-left:5%;'></div>
                        <div id="graphs_evolution" style='float:left;width:30%; margin-left:5%;'></div>
                        <div id="graphs_infinity" style='float:left;width:30%; margin-left:5%;'></div>
                        <div id="graphs_zeus" style='float:left;width:30%; margin-left:5%;'></div>
                        <div id="graphs_portomaso" style='float:left;width:30%; margin-left:5%;'></div>
                        <div id="graphs_forex" style='float:left;width:30%; margin-left:5%;'></div>
                        <div id="graphs_totem" style='float:left;width:30%; margin-left:5%;'></div>
                        <div id="graphs_poker" style='float:left;width:30%;'></div>
                        <div id="graphs_poker_2d" style='float:left;width:30%; margin-left:5%;'></div>
                        <div id="graphs_betting" style='float:left;width:30%;'></div>
                        <div id="graphs_betting_live" style='float:left;width:30%; margin-left:5%;'></div>

                    </div>
                    <div class="clearfix"></div>
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
                    if($royalpk==1){
                        $royalpkRake=new highcharts('chart10', 'pie', 'RoyalPk Rakes', 'graphs_royalpk');
                        $royalpkRake->prepareData($res->Records, "$chartGroup",'royalpk_rake');
                        ?>
                        <?=$royalpkRake->showChart('80'); } ?>

                    <?php
                    if($playtech==1){
                        $playtechRake=new highcharts('chart10', 'pie', 'Playtech/SkyWind Rakes', 'graphs_playtech');
                        $playtechRake->prepareData($res->Records, "$chartGroup",'playtech_rake');
                        ?>
                        <?=$playtechRake->showChart('80'); } ?>

                    <?php
                    if($zeus==1){
                        $zeusRake=new highcharts('chart10', 'pie', 'Zeus Rakes', 'graphs_playtech');
                        $zeusRake->prepareData($res->Records, "$chartGroup",'zeus_rake');
                        ?>
                        <?=$zeusRake->showChart('80'); } ?>
                        
                    <?php  if($evolution==1){
                        $evolutionRake=new highcharts('chart10', 'pie', 'Evolution Rakes', 'graphs_evolution');
                        $evolutionRake->prepareData($res->Records, "$chartGroup",'evolution_rake');
                        ?>
                        <?=$evolutionRake->showChart('80'); } ?>

                    <?php

                    if($infinity==1){
                        $infinityRake=new highcharts('chart10', 'pie', 'Infinity Rakes', 'graphs_infinity');
                        $infinityRake->prepareData($res->Records, "$chartGroup",'infinity_rake');
                        ?>
                        <?=$infinityRake->showChart('80'); } ?>

                    <?php

                    if($portomaso==1){
                        $portomasoRake=new highcharts('chart10', 'pie', 'Portomaso Rakes', 'graphs_portomaso');
                        $portomasoRake->prepareData($res->Records, "$chartGroup",'portomaso_rake');
                        ?>
                        <?=$portomasoRake->showChart('80'); } ?>

                    <?php
                    if($forex==1){
                        $forexRake=new highcharts('chart10', 'pie', 'Forex Rakes', 'graphs_forex');
                        $forexRake->prepareData($res->Records, "$chartGroup",'forex_rake');
                        ?>
                        <?=$forexRake->showChart('80'); } ?>


                    <?php
                    if($totem==1){
                        $totemRake=new highcharts('chart10', 'pie', 'Totem Rakes', 'graphs_totem');
                        $totemRake->prepareData($res->Records, "$chartGroup",'totem_rake');
                        ?>
                        <?=$totemRake->showChart('80'); } ?>

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
        } ?>
        </p>
        </div>

    <? }?>

    <? if(check_access('financial_summary_betting')){?>

        <div id="betting">
        <center>
        <?
        $betting= $bettingl=  1;
        $betting_jurisdictions=array('Casino profit','Nation profit','Region profit','District profit');
        ?>
        <form action="/financial_summary" method="post" class="resubmitForm">
            <?php printHiddenInput('doQuery', 'yes');?>
            <h2><?=$lang->getLang("Betting Financial Activity")?></h2>
            <p>

                <input type="hidden" name="active" value="betting">
            <table cellpadding="4" cellspacing="1" border="0">
                <tr valign=top>
                    <td class=label><?=$lang->getLang("Choose period type")?></td>
                    <td class=content>
                        <select name="periodType" id="periodType" class="fleft">
                            <option value="0" <?=($periodType==0?'selected': '')?>><?=$lang->getLang("Custom")?></option>
                            <option value="1" <?=($periodType==1?'selected': '')?>><?=$lang->getLang("Weekly")?></option>
                            <option value="2" <?=($periodType==2?'selected': '')?>><?=$lang->getLang("Monthly")?></option>
                            <option value="3" <?=($periodType==3?'selected': '')?>>2 <?=$lang->getLang("Months")?></option>
                            <option value="4" <?=($periodType==4?'selected': '')?>>4 <?=$lang->getLang("Months")?></option>
                        </select>
                        <div style="margin-left: 300px">
                            <?$file=WEB_ROOT."/documents/commissions_".strtolower($_SESSION["language"]).".doc"?>
                            <a href="<?=file_exists($file) ? "/documents/commissions_".strtolower($_SESSION["language"]).".doc" : "/documents/commissions_en.doc"?>" target="_self"><img alt="Download Comissions documnetations" src="/media/images/icon_download.jpg" border="0"> <?=$lang->getLang("Download Commissions documentation")?></a>
                        </div>
                    </td>
                </tr>
                <tr valign=top id="period2" class="timeHelpers hidden">
                    <td class=label><?=$lang->getLang("Choose month")?></td>
                    <td class=content>
                        <?=month_select_box($defaultmonth,true)?>
                    </td>
                </tr>

                <tr valign=top id="period1" class="timeHelpers hidden">
                    <td class=label><?=$lang->getLang("Choose week")?></td>
                    <td class=content>
                        <?=week_select_box($defaultweek,true)?>
                    </td>
                </tr>
                <tr valign=top id="period3" class="timeHelpers hidden">
                    <td class=label>2 <?=$lang->getLang("Months")?></td>
                    <td class=content>
                        <?=month_select_box2($defaultmonth2,2)?>
                    </td>
                </tr>
                <tr valign=top id="period4" class="timeHelpers hidden">
                    <td class=label>4 <?=$lang->getLang("Months")?></td>
                    <td class=content>
                        <?=month_select_box2($defaultmonth4,4)?>
                    </td>
                </tr>
                <tr valign=top>
                    <td class=label><?=$lang->getLang("From")?></td>
                    <td class=content>
                        <input type='text' name='date_start' id='date_start_betting' class="dateStartPicker" value="<?=$date_start?>" >
                    </td>
                </tr>
                <tr valign=top>
                    <td class=label><?=$lang->getLang("Until")?></td>
                    <td class=content>
                        <input type='date_end' name='date_end' id='date_end_betting' class="dateEndPicker" value="<?=$date_end?>">
                    </td>
                </tr>
                <?php
                if ( $_SESSION['jurisdiction_class'] != 'club' ) {
                    ?>
                    <tr valign=top>
                        <td class=label><?=$lang->getLang("Jurisdiction")?></td>
                        <td class=content>
                            <?php
                            if ( 'club' == $_SESSION['jurisdiction_class'] ) {
                                ?>
                                <div id="jurisdiction_select2" class="content">
                                    <?php echo get_jurisdiction_select('club', $jurisdiction_id);?>
                                </div>
                            <?php
                            }
                            else{
                                ?>
                                <table cellpadding="0" cellspacing="0" border="0">
                                    <tr>
                                        <td>
                                            <?php
                                            if ( 'region' == $report_type || 'district' == $report_type || 'club' == $report_type ){
                                                //$disabled = 'disabled="disabled"';
                                            }
                                            $jurisdiction_level = $_POST["jurisdiction_level"];
                                            ?>
                                            <select id="jurisdiction_level" name="jurisdiction_level" class="jurisdictionSelector" onChange="change_jurisdiction(this,'jurisdiction_select2')" <?=$disabled?>>
                                                <?php if ( 'casino' == $_SESSION['jurisdiction_class'] ) : ?>
                                                    <?php ?>//if ( 'casino' == $_SESSION['jurisdiction_class']) : ?>
                                                    <option value="nation"<?if ( 'nation' == $jurisdiction_level ) echo 'selected="selected"'; ?>>Nation </option>
                                                <?php endif; ?>

                                                <?php if ('casino' == $_SESSION['jurisdiction_class'] || 'nation' == $_SESSION['jurisdiction_class']  ) : ?>
                                                    <option value="region" <?php if ('region' == $jurisdiction_level) echo 'selected="selected"'; ?>>Region</option>
                                                <?php endif; ?>

                                                <?php if ('casino' == $_SESSION['jurisdiction_class'] || 'nation' == $_SESSION['jurisdiction_class'] || 'region' == $_SESSION['jurisdiction_class'] ) : ?>
                                                    <option value="district" <?php if ('district' == $jurisdiction_level) echo 'selected="selected"'; ?>>District</option>
                                                <?php endif; ?>

                                                <?php if ('casino' == $_SESSION['jurisdiction_class'] || 'nation' == $_SESSION['jurisdiction_class'] || 'region' == $_SESSION['jurisdiction_class'] || 'district' == $_SESSION['jurisdiction_class'] ) : ?>

                                                <?php endif; ?>
                                                <option value="club"<?if ( 'club' == $jurisdiction_level ) echo 'selected="selected"'; ?>>Club</option>
                                            </select>
                                        </td>
                                        <td>
                                            <div id="jurisdiction_select2" class="content">
                                                <?= get_jurisdiction_select(getPostGetVar('jurisdiction_level'), $jurisdiction_id);?>
                                            </div>
                                        </td>
                                    </tr>
                                </table>
                            <?php
                            }
                            ?>
                        </td>
                    </tr>
                <?
                }
                $jur_groups_level='club';
                ?>
                </tr>
                <?if(check_access('financial_summary_details_fin') || check_access("financial_summary_details_betwin")) {?>
                    <tr valign=top>
                        <td class=label><?=$lang->getLang("Details")?></td>
                        <td class=content>
                            <?if(check_access("financial_summary_details_fin")){ ?>
                            <input type='checkbox' name='financial' id='financial' <?php if(isset($_POST['financial'])&& $_POST['financial']==true) echo "checked"; ?> /><?=$lang->getLang("Financial")?>
                            <? } if(check_access("financial_summary_details_betwin")){ ?>
                                <input type='checkbox' name='betwin' id='betwin_betting' <?php if(isset($_POST['betwin'])&& $_POST['betwin']==true) echo "checked"; ?> /><?=$lang->getLang("Bet & Win")?>
                            <? } ?>
                        </td>
                    </tr>
                <? } ?>
                <tr>
                    <td class="label"><?=$lang->getLang("Result in Page")?></td>
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
                    <td></td>
                    <td><input type =image src="<?=image_dir?>/button_submit.gif" name="submit"></td>
                </tr>
            </table>
        </form>
        <?if(isset($_POST['doQuery']) && $active=='betting'){
            $casino= $casino_live=  $virtual=  $royalpk= $playtech=$evolution=$infinity=$portomaso=$forex= $pokerl= $poker2d=$totem=  0;
            ?>
            <script>
                $(function() {
                    var index = $('#tabs a[href="#betting"]').parent().index();
                    $( "#tabs" ).tabs( "option", "active", index );
                });
            </script>
            <?
            if($jur_groups_level=='club'){
                $betting_jurisdictions=array_slice($betting_jurisdictions,array_search(ucfirst($_SESSION['jurisdiction_class'])." profit",$betting_jurisdictions));
            }
            else{
                $betting_jurisdictions=array_slice($betting_jurisdictions,array_search(ucfirst($_SESSION['jurisdiction_class'])." profit",$betting_jurisdictions),array_search(ucfirst($jur_groups_level)." profit",$betting_jurisdictions));
            }

            $betting_jurisdictions=array_reverse($betting_jurisdictions);
            if($_SESSION['jurisdiction_class']=="club"){
                $betting_jurisdictions=array();
                $betting_jurisdictions_live=array();
            }
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
                $cell_fmt_arr = array (
                    'Prematch<br>bet'               => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'Prematch<br>win'               => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'Prematch<br>net'               => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'Live<br>bet'                   => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'Live<br>win'                   => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'Live<br>net'                   => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'Total<br>bet'                  => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'Total<br>win'                  => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'Utile'                         => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'Club<br>rake'                  => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'Club<br>tot profit'            => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'Netto<br>bet'                  => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'betting live<br>bet'           => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'betting live<br>win'           => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'betting live<br>net'           => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'Club<br>bonus'                 => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    'Nation profit'                 => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'Region profit'                 => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'District profit'               => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'betting live<br>cost region'   => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'betting live<br>cost district' => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'betting live<br>fin'           => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'financial'                     => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                    ' Total Perc. '                 => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'Total Net Betting'             => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'Net'                           => 'style="text-align:right;white-space: nowrap;" class="content light"',
                    'nation'                        => 'style="white-space: nowrap;" ',
                    'region'                        => 'style="white-space: nowrap;" ',
                    'district'                      => 'style="white-space: nowrap;" ',
                    'club'                          => 'style="white-space: nowrap;" '
                );
                $val_fmt_arr = array (
                    //'count'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_rake"]));',
                    'Prematch<br>bet'             => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_bet"]));',
                    'Live<br>bet'                 => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_bet_live"]));',
                    'Prematch<br>win'             => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_win"]));',
                    'Live<br>win'                 => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_win_live"]));',
                    'Total<br>bet'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_bet"]+$rec["betting_bet_live"]));',
                    'Total<br>win'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_win"]+$rec["betting_win_live"]));',
                    'Utile'                       => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_bet"]+$rec["betting_bet_live"]- $rec["betting_win"]-$rec["betting_win_live"]));',
                    'Club<br>rake'                => 'return "<strong>".getBettingPercentage($rec["jps_perc_betting_c"],"type")."<br>".preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_rake"]+$rec["betting_rake_live"]));',
                    'Club<br>bonus'               => 'return getInDollars(calculateBonus($rec));',
                    'Club<br>tot profit'          => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_rake"]+$rec["betting_rake_live"]+calculateBonus($rec)));',
                    'Netto<br>bet'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_bet"]+$rec["betting_bet_live"]- $rec["betting_win"]-$rec["betting_win_live"]-$rec["betting_rake"]-$rec["betting_rake_live"]-calculateBonus($rec)));',
                    'Profile'                     => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_rake"]));',
                    'Casino profit'               => 'return "<strong>".(getBettingPercentage($rec["jps_perc_betting_n"],"type")=="FeedTax" ? getInDollars($rec["betting_bet"]) : getInDollars($rec["betting_bet"]+$rec["betting_bet_live"]-$rec["betting_win"]-$rec["betting_win_live"]-$rec["betting_rake"]-$rec["betting_rake_live"]-calculateBonus($rec)))
                                                     ."<br><span class=tip>".getInDollars(calculateBettingRake($rec,"casino",1));',
                    'Nation profit'               => 'return (getBettingPercentage($rec["jps_perc_betting_n"],"type")=="FeedTax" ? getInDollars(calculateBettingRake($rec,"nation",1)) :"<strong>".getBettingPercentage($rec["jps_perc_betting_n"],"type")."<br>".getBettingPercentage($rec["jps_perc_betting_n"])."% ="
                                                     .($_SESSION["jurisdiction_class"]=="nation" ? getInDollars(calculateBettingRake($rec,"nation",1)+calculateBettingRake($rec,"region",1)+calculateBettingRake($rec,"district",1))."<br><span class=tip>" : "")
                                                     .getInDollars(calculateBettingRake($rec,"nation",1)));',
                    'Region profit'               => 'return "<strong>".getBettingPercentage($rec["jps_perc_betting_r"],"type")."<br>".getBettingPercentage($rec["jps_perc_betting_r"])."% ="
                                                     .($_SESSION["jurisdiction_class"]=="region" ? getInDollars(calculateBettingRake($rec,"region",1)+calculateBettingRake($rec,"district",1))."<br><span class=tip>" : "")
                                                     .getInDollars(calculateBettingRake($rec,"region",1));',
                    'District profit'             => 'return "<strong>".getBettingPercentage($rec["jps_perc_betting_d"],"type")."<br>".getBettingPercentage($rec["jps_perc_betting_d"])."% =".getInDollars(calculateBettingRake($rec,"district",1));',
                    'Total Net'                   => 'return preg_replace("/\&nbsp; /", "",getInDollars(calculateTotalNet($rec)));',
                    'Prematch<br>fin'             => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_fin"]));',
                    'Prematch<br>net'             => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_bet"]-$rec["betting_win"]));',
                    'Live<br>fin'                 => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_fin_live"]));',
                    'Live<br>net'                 => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_bet_live"]-$rec["betting_win_live"]));',
                );


                $val_fmt_arr_excel = array (
                    //'count'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_rake"]));',
                    'Prematch<br>bet'             => 'return preg_replace("/\&nbsp; /", "",$rec["betting_bet"]);',
                    'Live<br>bet'                 => 'return preg_replace("/\&nbsp; /", "",$rec["betting_bet_live"]);',
                    'Prematch<br>win'             => 'return preg_replace("/\&nbsp; /", "",$rec["betting_win"]);',
                    'Live<br>win'                 => 'return preg_replace("/\&nbsp; /", "",$rec["betting_win_live"]);',
                    'Total<br>bet'                => 'return preg_replace("/\&nbsp; /", "",$rec["betting_bet"]+$rec["betting_bet_live"]);',
                    'Total<br>win'                => 'return preg_replace("/\&nbsp; /", "",$rec["betting_win"]+$rec["betting_win_live"]);',
                    'Utile'                       => 'return preg_replace("/\&nbsp; /", "",$rec["betting_bet"]+$rec["betting_bet_live"]- $rec["betting_win"]-$rec["betting_win_live"]);',
                    'Club<br>rake'                => 'return "<strong>".getBettingPercentage($rec["jps_perc_betting_c"],"type")."<br>".preg_replace("/\&nbsp; /", "",$rec["betting_rake"]+$rec["betting_rake_live"]);',
                    'Club<br>bonus'               => 'return getInDollars(calculateBonus($rec));',
                    'Club<br>tot profit'          => 'return preg_replace("/\&nbsp; /", "",$rec["betting_rake"]+$rec["betting_rake_live"]+calculateBonus($rec));',
                    'Netto<br>bet'                => 'return preg_replace("/\&nbsp; /", "",$rec["betting_bet"]+$rec["betting_bet_live"]- $rec["betting_win"]-$rec["betting_win_live"]-$rec["betting_rake"]-$rec["betting_rake_live"]-calculateBonus($rec));',
                    'Profile'                     => 'return preg_replace("/\&nbsp; /", "",$rec["betting_rake"]);',
                    'Casino profit'               => 'return "<strong>".$rec["betting_bet"]+$rec["betting_bet_live"]-$rec["betting_win"]-$rec["betting_win_live"]-$rec["betting_rake"]-$rec["betting_rake_live"]-calculateBonus($rec)
                                              ."<br><span class=tip>".calculateBettingRake($rec,"casino",1);',
                    'Nation profit'               => 'return "<strong>".getBettingPercentage($rec["jps_perc_betting_n"],"type")."<br>".getBettingPercentage($rec["jps_perc_betting_n"])."% ="
                                             .($_SESSION["jurisdiction_class"]=="nation" ? calculateBettingRake($rec,"nation",1)+calculateBettingRake($rec,"region",1)+calculateBettingRake($rec,"district",1)."<br><span class=tip>" : "")
                                             .calculateBettingRake($rec,"nation",1);',
                    'Region profit'               => 'return "<strong>".getBettingPercentage($rec["jps_perc_betting_r"],"type")."<br>".getBettingPercentage($rec["jps_perc_betting_r"])."% ="
                                            .($_SESSION["jurisdiction_class"]=="region" ? calculateBettingRake($rec,"region",1)+calculateBettingRake($rec,"district",1)."<br><span class=tip>" : "")
                                            .calculateBettingRake($rec,"region",1);',
                    'District profit'             => 'return "<strong>".getBettingPercentage($rec["jps_perc_betting_d"],"type")."<br>".getBettingPercentage($rec["jps_perc_betting_d"])."% =".calculateBettingRake($rec,"district",1);',
                    'Total Net'                   => 'return preg_replace("/\&nbsp; /", "",calculateTotalNet($rec));',
                    'Prematch<br>fin'             => 'return preg_replace("/\&nbsp; /", "",$rec["betting_fin"]);',
                    'Prematch<br>net'             => 'return preg_replace("/\&nbsp; /", "",$rec["betting_bet"]-$rec["betting_win"]);',
                    'Live<br>fin'                 => 'return preg_replace("/\&nbsp; /", "",$rec["betting_fin_live"]);',
                    'Live<br>net'                 => 'return preg_replace("/\&nbsp; /", "",$rec["betting_bet_live"]-$rec["betting_win_live"]);',
                );
                $cols_arr = array();
                array_push($cols_arr, 'nation');
                $val_fmt_arr['nation']= 'return "<strong>" . $rec["n_jur_name"] . "</strong>"';
                if($canCangeCurency){
                    $val_fmt_arr['nation'].='.(($rec["currency"]!=$_SESSION["currency_id"] && $rec["currency"]!="")? "<br><span class=changeCurr style=\"font-weight:bold;color:red;cursor:pointer;\" data-curr=".$rec["currency"].">".$_SESSION["currencies"][$rec["currency"]]["symbol"]."</span>" :
                                            (($_SESSION["currency_id"]!=1)? "<br><span class=changeCurr style=\"font-weight:bold;color:red;cursor:pointer;\" data-curr=".$rec["currency"].">'.$_SESSION['currency_html'].'</span>" :"" ) );';
                }
                else{
                    $val_fmt_arr['nation'].=';';
                }
                $val_fmt_arr_excel['nation']= 'return "<strong>" . $rec["n_jur_name"] . "</strong>";';
                $chartGroup='n_jur_name';
                if($jur_groups_level=='region')
                {
                    array_push($cols_arr, 'region');
                    $val_fmt_arr['region']= 'return "<strong>" . $rec["r_jur_name"] . "</strong><br /> <span class=\"tip\"> % </span>";';
                    $val_fmt_arr_excel['region']= 'return "<strong>" . $rec["r_jur_name"] . "</strong><br /> <span class=\"tip\"> % </span>";';
                    $chartGroup='r_jur_name';
                }
                if($jur_groups_level=='district')
                {
                    array_push($cols_arr, 'region');
                    array_push($cols_arr, 'district');
                    $val_fmt_arr['region']= 'return "<strong>" . $rec["r_jur_name"] . "</strong>";';
                    $val_fmt_arr['district']='return "<strong>" . $rec["d_jur_name"] . "</strong><br /><span class=\"tip\"> % </span>";';
                    $val_fmt_arr_excel['region']= 'return "<strong>" . $rec["r_jur_name"] . "</strong>";';
                    $val_fmt_arr_excel['district']='return "<strong>" . $rec["d_jur_name"] . "</strong><br /><span class=\"tip\"> % </span>";';
                    $chartGroup='d_jur_name';
                }
                if($jur_groups_level=='club')
                {
                    array_push($cols_arr, 'region');
                    array_push($cols_arr, 'district');
                    array_push($cols_arr, 'club');
                    $val_fmt_arr['district'] ='return "<strong>" . $rec["d_jur_name"] . "</strong>";';
                    $val_fmt_arr['region'] = 'return "<strong>" . $rec["r_jur_name"] . "</strong>";';
                    $val_fmt_arr['club'] = 'return "<strong>". $rec[c_jur_name] ."</strong><br /><span class=\"tip\"> % </span>";';
                    $val_fmt_arr_excel['district'] ='return "<strong>" . $rec["d_jur_name"] . "</strong>";';
                    $val_fmt_arr_excel['region'] = 'return "<strong>" . $rec["r_jur_name"] . "</strong>";';
                    $val_fmt_arr_excel['club'] = 'return "<strong>". $rec[c_jur_name] ."</strong><br /><span class=\"tip\"> % </span>";';
                    $chartGroup='c_jur_name';
                }
                if($betwin) {
                    array_push($cols_arr, 'Prematch<br>bet');
                    array_push($cols_arr, 'Prematch<br>win');
                }
                array_push($cols_arr,'Prematch<br>net');
                if($financial){
                    array_push($cols_arr,'Prematch<br>fin');
                }
                if($betwin) {
                    array_push($cols_arr, 'Live<br>bet');
                    array_push($cols_arr, 'Live<br>win');
                }
                array_push($cols_arr,'Live<br>net');
                if($financial){

                    array_push($cols_arr,'Live<br>fin');
                }
                array_push($cols_arr,'Total<br>bet');
                array_push($cols_arr,'Total<br>win');
                array_push($cols_arr,'Utile');
                array_push($cols_arr,'Club<br>rake');
                array_push($cols_arr,'Club<br>bonus');
                array_push($cols_arr,'Club<br>tot profit');
                array_push($cols_arr,'Netto<br>bet');
                $cols_arr=array_merge($cols_arr,$betting_jurisdictions);
                array_push($cols_arr,'Total Net');
                $totals = array();
                $totalsPerc = array();

                $caTot=0;

                $prevNTotR=0;
                $prevN='';
                $prevNProfile='';

                $prevRTotR=0;
                $prevR='';
                $prevRProfile='';

                $prevDTotR=0;
                $prevD='';
                $prevDProfile='';
                $totFeedTax=0;
                $totFeedTaxLive=0;

                while($res->hasNext()){
                    $row = $res->next();
                    if($row["currency"]!=$_SESSION["currency_id"]){
                        foreach($row as $n=>$nv){
                            if(((strpos($n,'bet') !== false) || (strpos($n,'win') !== false) || (strpos($n,'rake') !== false) || (strpos($n,'fin') !== false))&&(strpos($n,'jps_perc') === false)){
                                if($nv!=0 && isset($_SESSION["currencies"][$row["currency"]]["value"])){
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

                    if(getBettingPercentage($row["jps_perc_betting_c"],"number")=="1" || getBettingPercentage($row["jps_perc_betting_c"],"number")=="3" ){
                        $row['betting_rake']= $res->Records[$res->CurrentIndex]['betting_rake']=($row["betting_bet"]-$row["betting_win"])*getBettingPercentage($row["jps_perc_betting_c"])/100;
                        $row['betting_rake_live']= $res->Records[$res->CurrentIndex]['betting_rake_live']=($row["betting_bet_live"]-$row["betting_win_live"])*getBettingPercentage($row["jps_perc_betting_live_c"])/100;

                    }
                    $netTot=$row["betting_bet"]+$row["betting_bet_live"]- $row["betting_win"]-$row["betting_win_live"]-$row["betting_rake"]-$row["betting_rake_live"]-calculateBonus($row);
                    $curN=$row['n_jur_name'];
                    $curNProfile=getBettingPercentage($row['jps_perc_betting_n'],'numeric');
                    $curNProc=getBettingPercentage($row['jps_perc_betting_n']);

                    $curR=$row['r_jur_name'];
                    $curRProfile=getBettingPercentage($row['jps_perc_betting_r'],'numeric');
                    $curRProc=getBettingPercentage($row['jps_perc_betting_r']);

                    $curD=$row['d_jur_name'];
                    $curDProfile=getBettingPercentage($row['jps_perc_betting_d'],'numeric');
                    $curDProc=getBettingPercentage($row['jps_perc_betting_d']);


                    if($prevD!=$curD && $prevD!=''){
                        if($prevDTotR<0 && $prevDProfile==3){
                            $prevRTotR+=$prevDTotR;

                        }
                        else{
                            $totals['district']+=$prevDTotR;
                        }

                        $prevDTotR=0;
                    }
                    if($prevR!=$curR && $prevR!=''){

                        if($prevRTotR<0 && $prevRProfile==3){
                            $prevNTotR+=$prevDTotR;
                        }
                        else{
                            $totals['region']+=$prevRTotR;
                        }
                        $prevRTotR=0;

                    }
                    if($prevN!=$curN && $prevN!=''){
                        if($prevNTotR<0 && $prevNProfile==3){
                            $caTot+=$prevNTotR;
                        }
                        else{
                            $totals['nation']+=$prevNTotR;
                        }
                        $prevNTotR=0;
                    }
                    if($netTot<0){
                        if($curNProfile==3){
                            $row['jps_perc_betting_n']=changeBettingType($row['jps_perc_betting_n'],1);
                        }
                        $prevNTotR+=calculateBettingRake($row,'nation',1,1);

                        if($curRProfile==3){
                            $row['jps_perc_betting_r']=changeBettingType($row['jps_perc_betting_r'],1);

                        }
                         $prevRTotR+=calculateBettingRake($row,'region',1,1);

                        if($curDProfile==3){
                            $row['jps_perc_betting_d']=changeBettingType($row['jps_perc_betting_d'],1);
                        }
                        $prevDTotR+=calculateBettingRake($row,'district',1,1);
                    }
                    else{
                        $prevNTotR+=calculateBettingRake($row,'nation',1);
                        $prevRTotR+=calculateBettingRake($row,'region',1);
                        $prevDTotR+=calculateBettingRake($row,'district',1);
                    }
                    $caTot+=calculateBettingRake($row,'casino',1);
                    $totals['bonus']+=calculateBonus($row);
                    $prevD=$curD;
                    $prevDProfile=$curDProfile;
                    $prevR=$curR;
                    $prevRProfile=$curRProfile;
                    $prevN=$curN;
                    $prevNProfile=$curNProfile;


                    if(getBettingPercentage($row["jps_perc_betting_n"],"type")=="FeedTax") {
                        $totFeedTax+=$row["betting_bet"]/100*getBettingPercentage($row["jps_perc_betting_n"]);
                        $totFeedTaxLive+=calculateLiveBettingRake($row,'nation',1);
                    }


                    foreach($row as $key => $val){
                        if(is_numeric($val)){
                            $totals[$key] += $val;
                        }
                    }
                }

                    if($prevDTotR<0 && $prevDProfile==3){
                        $prevRTotR+=$prevDTotR;

                    }
                    else{
                        $totals['district']+=$prevDTotR;
                    }

                    if($prevRTotR<0 && $prevRProfile==3){
                        $prevNTotR+=$prevRTotR;
                    }
                    else{
                        $totals['region']+=$prevRTotR;
                    }
                    if($prevNTotR<0 && $prevNProfile==3){
                        $totals['casino']+=$prevNTotR;
                    }
                    else{
                        $totals['nation']+=$prevNTotR;
                    }

                $totals["casino"]+=$caTot;


                $totalsRakeSum=0;
                $totalsBetSum=0;
                $totalsWinSum=0;
                $totalBonus=0;

                $totalsRakeSum +=($totals['betting_rake']);
                $totalsBetSum  +=$totals['betting_bet'];
                $totalsWinSum  +=$totals['betting_win'];
                $totalBonus    +=$totals['bonus'];

                $totalsRakeSum +=($totals['betting_rake_live']);
                $totalsBetSum  +=$totals['betting_bet_live'];
                $totalsWinSum  +=$totals['betting_win_live'];


                /* if(is_numeric($jur_sel_id)){
                     $nation_profile=getBettingPercentage($row['jps_perc_betting_n'],'numeric');
                     $nation_proc=getBettingPercentage($row['jps_perc_betting_n']);

                     $region_profile=getBettingPercentage($row['jps_perc_betting_r'],'numeric');
                     $region_proc=getBettingPercentage($row['jps_perc_betting_r']);

                     $district_profile=getBettingPercentage($row['jps_perc_betting_d'],'numeric');
                     $district_proc=getBettingPercentage($row['jps_perc_betting_d']);


                     $netBet=$totalsBetSum-$totalsWinSum-$totalsRakeSum-$totalBonus;
                     if(($totalsBetSum-$totalsWinSum)<0)
                     {
                         if($jurisdiction_level=='nation'){
                             if($nation_profile==3){
                                 $totals['casino']=$netBet;
                                 $totals['nation']=0;
                             }
                         }

                         if($jurisdiction_level=='region'){
                             if($region_profile==3){
                                 if($nation_profile==3){
                                     $totals['casino']+=$totals['nation'];
                                     $totals['nation']=0;
                                 }
                                 else{
                                     $totals['nation']=$netBet/100*$nation_proc;
                                 }
                                 $totals['region']=0;
                             }
                          }

                         if($jurisdiction_level=='district'){
                             if($district_profile==3){
                                 if($region_profile==3){
                                     if($nation_profile==3){
                                         $totals['casino']+=$totals['nation'];
                                         $totals['nation']=0;
                                     }
                                     else{
                                         $totals['nation']=$netBet/100*$nation_proc;
                                     }
                                     $totals['region']=0;
                                 }
                                 else{
                                     $totals["region"]=$netBet/100*$region_proc;
                                 }
                                 $totals['district']=0;
                           }
                         }

                     }
                 }*/
                ?>
                <div class="tip fleft"><?=$lang->getLang("Time taken to execute your request")?>: <?=$duration?></div>
                <center>
                <br>

                <br>
                <div class="bodyHD"><?=$lang->getLang("Totals")?></div>
                <table bgcolor="#a99a9" cellpadding="4" cellspacing="1">
                    <tr><td class="label centered">Closure</td>
                        <?if($betwin){ ?>
                        <td class="label centered"><?=$lang->getLang("Prematch")?><br><?=$lang->getLang("Bet")?></td>
                        <td class="label centered"><?=$lang->getLang("Prematch")?><br><?=$lang->getLang("Win")?></td>
                        <? } ?>
                        <td class="label centered"><?=$lang->getLang("Prematch")?><br><?=$lang->getLang("Net")?></td>
                        <?if($financial){ ?>
                            <td class="label centered"><?=$lang->getLang("Prematch")?><br><?=$lang->getLang("Fin")?></td>
                        <? } ?>
                        <?if($betwin){ ?>
                        <td class="label centered"><?=$lang->getLang("Live")?><br><?=$lang->getLang("Bet")?></td>
                        <td class="label centered"><?=$lang->getLang("Live")?><br><?=$lang->getLang("Win")?></td>
                        <? } ?>
                        <td class="label centered"><?=$lang->getLang("Live")?><br><?=$lang->getLang("Net")?></td>
                        <?if($financial){ ?>
                            <td class="label centered"><?=$lang->getLang("Live")?><br><?=$lang->getLang("Fin")?></td>
                        <? } ?>
                        <td class="label centered"><?=$lang->getLang("Total")?><br><?=$lang->getLang("Bet")?></td>
                        <td class="label centered"><?=$lang->getLang("Total")?><br><?=$lang->getLang("Win")?></td>
                        <td class="label centered"><?=$lang->getLang("Utile")?></td>
                        <td class="label centered"><?=$lang->getLang("Club")?><br><?=$lang->getLang("rake")?></td>
                        <td class="label centered"><?=$lang->getLang("Club")?><br><?=$lang->getLang("bonus")?></td>
                        <td class="label centered"><?=$lang->getLang("Club")?><br><?=$lang->getLang("tot profit")?></td>
                        <td class="label centered"><?=$lang->getLang("Netto")?><br><?=$lang->getLang("bet")?></td>
                        <?if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction_class']=='nation'){ ?>
                            <td class="label centered"><?=$lang->getLang("Feed")?><br><?=$lang->getLang("Tax")?></td>
                        <? } ?>

                        <?  if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction_class']=='nation' || $_SESSION['jurisdiction_class']=='region' ||  $_SESSION['jurisdiction_class']=='district') {?>
                            <td class="label centered"><?=$lang->getLang("District")?><br><?=$lang->getLang(" Profit")?></td>
                        <? } if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction_class']=='nation' || $_SESSION['jurisdiction_class']=='region'  ){ ?>
                        <td class="label centered"><?=$lang->getLang("Region")?><br><?=$lang->getLang(" profit")?></td>
                        <? } if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction_class']=='nation'){ ?>
                            <td class="label centered"><?=$lang->getLang("Nation")?><br><?=$lang->getLang(" profit")?></td>
                        <? } if($_SESSION['jurisdiction_class']=='casino'){ ?>
                            <td class="label centered"><?=$lang->getLang("Casino")?><br><?=$lang->getLang(" profit")?></td>
                        <?} if($_SESSION['jurisdiction_class']!='club'){?>
                            <td class="label centered"><?=$lang->getLang("Total Net")?></td>
                        <? } ?>
                    </tr>
                    <tr class='odd' style="vertical-align: top">
                        <td class='content'><a href="/search_closure/<?=$date_start?>/<?=$date_end?>"><?=$lang->getLang("View closure")?></a></td>
                        <?if($betwin){ ?>
                        <td class="content dark" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["betting_bet"]))?> </td>
                        <td class="content dark" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["betting_win"]))?></td>
                        <? } ?>
                        <td class="content dark" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["betting_bet"]-$totals["betting_win"]))?></td>
                        <?if($financial){ ?>
                            <td class="content dark" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["betting_fin"]))?></td>
                        <? } ?>
                        <?if($betwin){ ?>
                        <td class="content dark" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["betting_bet_live"]))?></td>
                        <td class="content dark" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["betting_win_live"]))?></td>
                        <? } ?>
                        <td class="content dark" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["betting_bet_live"]-$totals["betting_win_live"]))?></td>
                        <?if($financial){ ?>
                            <td class="content dark" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totals["betting_fin_live"]))?></td>
                        <? } ?>
                        <td class="content light" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totalsBetSum))?></td>
                        <td class="content light" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totalsWinSum))?></td>
                        <td class="content light" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totalsBetSum-$totalsWinSum))?></td>
                        <td class="content light" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totalsRakeSum))?></td>
                        <td class="content light" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totalBonus))?></td>
                        <td class="content light" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totalsRakeSum+$totalBonus))?></td>
                        <td class="content light" style="text-align: right"><strong><?=preg_replace("/\&nbsp; /", "",getInDollars($totalsBetSum-$totalsWinSum-$totalsRakeSum-$totalBonus))?></td>
                        <?if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction_class']=='nation'){ ?>
                        <td class="content light" style="text-align: right"><?=getInDollars($totFeedTax)?><br />
                            <?=getInDollars($totFeedTaxLive)?>
                        </td>
                         <? } ?>

                        <? if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction_class']=='nation' || $_SESSION['jurisdiction_class']=='region' ||  $_SESSION['jurisdiction_class']=='district') {?>
                            <td class="content dark tip" style="text-align: right"><br />
                                % = <?=getInDollars($totals['district'])?></td>
                        <? } if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction_class']=='nation' || $_SESSION['jurisdiction_class']=='region') {?>
                        <td class="content dark tip" style="text-align: right"><br />
                            <?if($_SESSION['jurisdiction_class']=='region'){ ?>
                                <?=getInDollars($totals['region']+$totals['district'])?><br />
                            <?}?>
                            % = <?=getInDollars($totals['region'])?></td>



                        <? } if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction_class']=='nation') { ?>
                            <td class="content dark tip" style="text-align: right"><br />
                                <?/*if($_SESSION['jurisdiction_class']=='nation'){ */?><!--
                                    <?/*=getInDollars($totals['nation']+$totals['region']+$totals['district'])*/?><br />
                                --><?/*}*/?>
                                <?if($_SESSION['jurisdiction_class']=='nation'){ ?>
                                    <?=getInDollars($totals['nation']+$totals['casino'])?><br />
                                <?}?>

                                % = <?=getInDollars($totals['nation'])?></td>

                        <?} if($_SESSION['jurisdiction_class']=='casino') { ?>
                            <td class="content dark tip" style="text-align: right"><br />% = <?=getInDollars(($totals['casino']))?></td>


                        <? } if($_SESSION['jurisdiction_class']=='casino') { ?>
                            <td class="content dark tip" style="text-align: right"><br />% = <?=getInDollars(($totals['casino']))?></td>
                        <? } ?>
                        <?if($_SESSION['jurisdiction_class']=='nation') { ?>
                            <td class="content dark tip" style="text-align: right"><br />% = <?=getInDollars($totals['nation']+$totals['casino'])?></td>
                        <? } if($_SESSION['jurisdiction_class']=='region') {?>
                            <td class="content dark tip" style="text-align: right"><br />% = <?=getInDollars($totals['region']+$totals['nation']+$totals['casino'])?></td>
                        <? } if($_SESSION['jurisdiction_class']=='district') {?>
                            <td class="content dark tip" style="text-align: right"><br />% = <?=getInDollars($totals['region']+$totals['nation']+$totals['casino']+$totals['casino'])?></td>
                        <? } ?>
                    </tr>
                </table>
                <br>
                <?php
                $post_vars = array('active'=>'betting','doQuery'=>'yes','date_start'=>$date_start,'date_end'=>$date_end,'jurisdiction_level'=>$jurisdiction_level,'jurisdiction'=>$jur_sel_id,'resforpage'=>$row_to_page,'betwin'=>$_POST['betwin'] );
                $start_row = (isset ( $_POST ['start_row'] )) ? $_POST ['start_row'] : 1;
                $excel_filename = 'Financial betting for '.$date_start.' - '.$date_end.'.xls';
                //$pdf_filename=md5($_SESSION['aus_username']).".pdf";
               // $pdfHeader='<h2 class="red">'.$lang->getLang("Financial summary for betting ").' '.$lang->getLang("for").' '.$date_start.' - '.$date_end."   </h2>";
                $qp = new QueryPrinter($res);
                $qp->printTable($res,$cols_arr, $post_vars, $val_fmt_arr, $cell_fmt_arr, $sum_fmt_arr, $excel_filename, $header, $width="", $start_row, $row_to_page,false,"",$pdf_filename);
                $qp->dumpToExcelFile($res,$cols_arr,$val_fmt_arr_excel,$excel_filename);
               // $qp->printPdf($res,$cols_arr,$val_fmt_arr,$pdf_filename,$pdfHeader);
                //var_dump(returnFinancialDataJson($res,$cols_arr,$val_fmt_arr));
            }

            ?>
            </center>
            <div class="clearfix"></div>
            <div id="graphs" style="width:1200px; "></div><br>
            <?php
            if($res->getNumRows() > 0){
                include_once("classes/highcharts.class.php");
                $totalRake=new highcharts('chart1', 'pie', 'Total Rakes', 'graphs');
                $totalChartArray=array();
                if($betting==1){
                    array_push($totalChartArray,'betting_rake');
                }
                if($bettingl==1){
                    array_push($totalChartArray,'betting_rake_live');
                }
                $totalRake->prepareData($totals, "$chartGroup",$totalChartArray);
                ?>
                <?=$totalRake->showChart();?>
                <div class="clearfix"></div>
                <div style="width:1400px;">
                    <div id="graphs_betting" style='float:left;width:50%;'></div>
                    <div id="graphs_betting_live" style='float:left;width:45%; margin-left:5%;'></div>

                </div>
                <div class="clearfix"></div>

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
        } ?>
        </center>
        </p>
        </div>

    <? } ?>


    <?if(check_access('financial_summary_virtual')){?>
        <div id="virtual">
        <?


        ?>
        <script type="text/javascript">
            function change_radio_level(select,id){
                id = typeof id !== 'undefined' ? id : 'jur_radio_level';
                var juris_class = select.options[select.selectedIndex].value;
                switch ( juris_class ){
                    <?php if ( 'casino' == $_SESSION['jurisdiction_class'])  { ?>
                    case 'nation' :
                        var str = '<?php echo getRadioGroupsLevel('nation','nation');?>';
                        document.getElementById(id).innerHTML = str;
                        break;
                    <?php } ?>
                    <?php if ( 'casino' == $_SESSION['jurisdiction_class'] || 'nation' == $_SESSION['jurisdiction_class'] ||
                               'region' == $_SESSION['jurisdiction_class'] || 'district' == $_SESSION['jurisdiction_class'])  { ?>
                    case 'club' :
                        var str = '<?php echo getRadioGroupsLevel('club','club');?>';
                        document.getElementById(id).innerHTML = str;
                        break;
                    <?php } ?>
                    <?php if ( 'casino' == $_SESSION['jurisdiction_class'] ||'nation' == $_SESSION['jurisdiction_class'] || 'region' == $_SESSION['jurisdiction_class'])  { ?>
                    case 'district' :
                        var str = '<?php echo getRadioGroupsLevel('district','district');?>';
                        document.getElementById(id).innerHTML = str;
                        break;
                    <?php } ?>
                    <?php if ( 'casino' == $_SESSION['jurisdiction_class'] ||  'nation' == $_SESSION['jurisdiction_class'] )  { ?>
                    case 'region' :
                        var str = '<?php echo getRadioGroupsLevel('region','region');?>';
                        document.getElementById(id).innerHTML = str;
                        break;
                    <?php } ?>
                }
            }
        </script>
        <?

        $jur_groups_level = (isset($_POST['jur_level'])) ? $_POST['jur_level'] : getStartJurisdictionLevel();?>
        <center>
        <form action="/financial_summary" method="post">
        <?php printHiddenInput('doQuery', 'yes');?>
        <h2>Virtual Financial Activity</h2>
        <p>
            <input type="hidden" name="active" value="virtual">
        <table cellpadding="4" cellspacing="1" border="0">
        <form action="/financial_summary" method="post">
            <?php printHiddenInput('doQuery', 'yes');?>
            <?php printHiddenInput('search_type', 'date_range');?>
            <input type="hidden" name="active" value="virtual">
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
                        <input type='text' name='date_start' id='date_start3' class="dateStartPicker" value="<?=$date_start?>" >
                    </td>
                </tr>
                <tr valign=top>
                    <td class=label><?=$lang->getLang("Until")?></td>
                    <td class=content>
                        <input type='date_end' name='date_end' id='date_end3' class="dateEndPicker" value="<?=$date_end?>">
                    </td>
                </tr>
                <?php
                if ( $_SESSION['jurisdiction_class'] != 'club' ) {
                    ?>
                    <tr valign=top>
                        <td class=label><?=$lang->getLang("Jurisdiction")?></td>
                        <td class=content>
                            <?php
                            if ( 'club' == $_SESSION['jurisdiction_class'] ) {
                                ?>
                                <div id="jurisdiction_select3" class="content">
                                    <?php echo get_jurisdiction_select('club', $jurisdiction_id);?>
                                </div>
                            <?php
                            }
                            else{
                                ?>
                                <table cellpadding="0" cellspacing="0" border="0">
                                    <tr>
                                        <td>
                                            <?php
                                            if ( 'region' == $report_type || 'district' == $report_type || 'club' == $report_type ){
                                                //$disabled = 'disabled="disabled"';
                                            }
                                            $jurisdiction_level = $_POST["jurisdiction_level"];
                                            ?>
                                            <select id="jurisdiction_level" name="jurisdiction_level" class="jurisdictionSelector" onChange="change_jurisdiction(this,'jurisdiction_select3'),change_radio_level(this,'jur_radio_level2');" <?=$disabled?>>
                                                <?php if ( 'casino' == $_SESSION['jurisdiction_class'] || 'nation' == $_SESSION['jurisdiction_class']) : ?>
                                                    <?php ?>//if ( 'casino' == $_SESSION['jurisdiction_class']) : ?>
                                                    <option value="nation"<?if ( 'nation' == $jurisdiction_level ) echo 'selected="selected"'; ?>> <?=$lang->getLang("Nation")?> </option>
                                                <?php endif; ?>

                                                <?php if ('casino' == $_SESSION['jurisdiction_class'] || 'nation' == $_SESSION['jurisdiction_class'] || 'region' == $_SESSION['jurisdiction_class'] ) : ?>
                                                    <option value="region" <?php if ('region' == $jurisdiction_level) echo 'selected="selected"'; ?>><?=$lang->getLang("Region")?></option>
                                                <?php endif; ?>

                                                <?php if ('casino' == $_SESSION['jurisdiction_class'] || 'nation' == $_SESSION['jurisdiction_class'] || 'region' == $_SESSION['jurisdiction_class'] || 'district' == $_SESSION['jurisdiction_class']) : ?>
                                                    <option value="district" <?php if ('district' == $jurisdiction_level) echo 'selected="selected"'; ?>><?=$lang->getLang("District")?></option>
                                                <?php endif; ?>

                                                <?php if ( 'district' == $_SESSION['jurisdiction_class'] ) : ?>
                                                    <option value=""> - <?=$lang->getLang("All clubs")?> - </option>
                                                <?php endif; ?>
                                                <option value="club"<?if ( 'club' == $jurisdiction_level ) echo 'selected="selected"'; ?>>Club</option>
                                            </select>
                                        </td>
                                        <td>
                                            <div id="jurisdiction_select3" class="content">
                                                <?= get_jurisdiction_select(getPostGetVar('jurisdiction_level'), $jurisdiction_id);?>
                                            </div>
                                        </td>
                                    </tr>
                                </table>
                            <?php
                            }
                            ?>
                        </td>
                    </tr>
                <?
                }
                ?>
                <tr valign=top>
                    <td class=label><?=$lang->getLang("Jurisdiction Totals")?></td>
                    <td <?form_td_style ('jurisdiction')?> ><div id="jur_radio_level2">
                            <?php echo getRadioGroupsLevel($jurisdiction_level, $jur_groups_level) ?>
                        </div>
                    </td>
                </tr>
                <?if(check_access('financial_summary_details')) {?>
                    <tr valign=top>
                        <td class=label><?=$lang->getLang("Details")?></td>
                        <td class=content>
                            <?if(check_access("financial_summary_details_betwin")){ ?>
                                <input type='checkbox' name='betwin' id='betwin' <?php if(isset($_POST['betwin'])&& $_POST['betwin']==true) echo "checked"; ?> /><?=$lang->getLang("Bet & Win")?>
                            <? } ?>
                            <?if(check_access("financial_summary_details_fin")){ ?>
                                <input type='checkbox' name='financial' id='financial' <?php if(isset($_POST['financial']) && $_POST['financial']==true) echo "checked"; ?> /><?=$lang->getLang("Financial")?>
                            <? }?>
                        </td>
                    </tr>
                <? } ?>
                <tr>
                    <td class="label"><?=$lang->getLang("Result in Page")?></td>
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
                    <td></td>
                    <td><input type =image src="<?=image_dir?>/button_submit.gif" name="submit"></td>
                </tr>
            </table>
        </form>
        <?
        if(isset($_POST['doQuery']) && $active=='virtual'){
        $virtual=1;
        ?>
        <script>
            $(function() {
                var index = $('#tabs a[href="#virtual"]').parent().index();
                $( "#tabs" ).tabs( "option", "active", index );
            });
        </script>
        <?
        $date_start = $_POST["date_start"];
        $date_end   = $_POST["date_end"];

        $d1 = strtotime("$date_start+1 year");
        $d2 = strtotime("$date_end");

        if($jur_groups_level=='club'){
            $betting_jurisdictions=array_slice($betting_jurisdictions,array_search(ucfirst($_SESSION['jurisdiction_class'])." profit",$betting_jurisdictions));
        }
        else{
            $betting_jurisdictions=array_slice($betting_jurisdictions,array_search(ucfirst($_SESSION['jurisdiction_class'])." profit",$betting_jurisdictions),array_search(ucfirst($jur_groups_level)." profit",$betting_jurisdictions));
        }

        if($_SESSION['jurisdiction_class']=="club"){
            $betting_jurisdictions=array();
            $betting_jurisdictions_live=array();
        }
        if($d2-$d1>0){
            addError("",$lang->getLang("Please choose an interval less than 1 year"));
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
        $res = getVirtualFinancialData($date_start, $date_end,$jur_groups_level, $betwin, $financial,$jurisdiction_level,$jur_sel_id); //Returns the financial details for the period passed
        $endtime = microtime(true);
        $duration = $endtime - $starttime;
        $duration=number_format($duration, 4, ',', '')."s";




        $totals = array();
        $totalsPerc = array();
        $totalsPercBetting = array();
        while($res->hasNext()){
            $row = $res->next();
            $totals['nation']+=($row['virtual_rake']/100*$row['jps_perc_virtual_n']);
            $totals['region']+=($row['virtual_rake']/100*$row['jps_perc_virtual_r']);
            $totals['district']+=($row['virtual_rake']/100*$row['jps_perc_virtual_d']);
            $totals['club']+=($row['virtual_rake']/100*$row['jps_perc_virtual_c']);
            foreach($row as $key => $val){
                if(is_numeric($val)){
                    $totals[$key] += $val;
                }
            }

        }
        ?>
        <div class="tip fleft"><?=$lang->getLang("Time taken to execute your request")?>: <?=$duration?></div>
        <center>
            <br><br><br>
            <div class="bodyHD"><?=$lang->getLang("Total")?></div>
            <table bgcolor="#a99a9" cellpadding="4" cellspacing="1">
                <tr>
                    <?php
                    if($betwin)
                    {
                        ?>
                        <td class="label centered"><?=$lang->getLang("Bet")?></td>
                        <td class="label centered"><?=$lang->getLang("Win")?></td>
                    <?php
                    }
                    ?>
                    <td class="label centered"><?=$lang->getLang("Net")?></td>
                    <?
                    if($jur_groups_level=='nation') {?>
                        <td class="label centered"><?=$lang->getLang("Casino Profit")?></td>
                        <td class="label centered"><?=$lang->getLang("Nation Profit")?></td>
                    <? } elseif($jur_groups_level=='region') {?>
                        <?if($_SESSION['jurisdiction_class']=='casino'){ ?>
                            <td class="label centered"><?=$lang->getLang("Casino Profit")?></td>

                        <? } ?>
                        <td class="label centered"><?=$lang->getLang("Nation Profit")?></td>
                        <td class="label centered"><?=$lang->getLang("Region Profit")?></td>
                    <? } elseif($jur_groups_level=='district') {?>
                        <?if($_SESSION['jurisdiction_class']=='casino'){ ?>
                            <td class="label centered"><?=$lang->getLang("Casino Profit")?></td>
                        <? } ?>
                        <?if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction_class']=='nation'){ ?>
                            <td class="label centered"><?=$lang->getLang("Nation Profit")?></td>
                        <? } ?>
                        <?if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction_class']=='nation' || $_SESSION['jurisdiction_class']=='region' ){ ?>
                            <td class="label centered"><?=$lang->getLang("Region Profit")?></td>
                        <? } ?>
                        <td class="label centered"><?=$lang->getLang("District Profit")?></td>
                    <? } elseif($jur_groups_level=='club') {?>
                        <?if($_SESSION['jurisdiction_class']=='casino'){ ?>
                            <td class="label centered"><?=$lang->getLang("Casino Profit")?></td>
                        <? } ?>
                        <?if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction_class']=='nation'){ ?>
                            <td class="label centered"><?=$lang->getLang("Nation Profit")?></td>
                        <? } if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction_class']=='nation' || $_SESSION['jurisdiction_class']=='region'  ){ ?>
                            <td class="label centered"><?=$lang->getLang("Region Profit")?></td>
                        <? } if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction_class']=='nation' || $_SESSION['jurisdiction_class']=='region' || $_SESSION['jurisdiction_class']=='district'  ){ ?>
                            <td class="label centered"><?=$lang->getLang("District Profit")?></td>
                        <? }
                        ?>
                        <td class="label centered"><?=$lang->getLang("Club Profit")?></td>
                    <?
                    } ?>
                </tr>
                <tr>
                    <?php
                    if($betwin)
                    {
                        ?>
                        <td class="content" style="text-align: right">
                            <?=getInDollars($totals['virtual_bet'])?></td>
                        <td class="content" style="text-align: right">
                            <?=getInDollars($totals['virtual_win'])?></td>
                    <?php
                    }
                    ?>
                    <td class="content" style="text-align: right">
                        <?=getInDollars($totals["virtual_rake"])?><br>
                    </td>
                    <?
                    if($jur_groups_level=='nation') {?>
                        <td class="content centered"><?=getInDollars($totals["virtual_rake"])?><br><?=getInDollars($totals["virtual_rake"]-$totals['nation'])?></td>
                        <td class="content centered"><?=getInDollars($totals['nation'])?></td>
                    <? } elseif($jur_groups_level=='region') {?>
                        <?if($_SESSION['jurisdiction_class']=='casino'){ ?>
                            <td class="content centered"><?=getInDollars($totals["virtual_bet"]-$totals['virtual_win'])?><br><?=getInDollars($totals["virtual_bet"]-$totals['virtual_win']-$totals['nation'])?></td>
                        <? } ?>
                        <td class="content centered"><?=getInDollars($totals['nation'])?></td>
                        <td class="content centered"><?=getInDollars($totals['region'])?></td>
                    <? } elseif($jur_groups_level=='district') {?>
                        <?if($_SESSION['jurisdiction_class']=='casino'){ ?>
                            <td class="content centered"><?=getInDollars($totals["virtual_bet"]-$totals['virtual_win'])?><br><?=getInDollars($totals["virtual_bet"]-$totals['virtual_win']-$totals['nation'])?></td>
                        <? } ?>
                        <?if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction_class']=='nation'){ ?>
                            <td class="content centered"><?=getInDollars($totals['nation'])?></td>
                        <? } ?>
                        <?if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction_class']=='nation' || $_SESSION['jurisdiction_class']=='region' ){ ?>
                            <td class="content centered"><?=getInDollars($totals['region'])?></td>
                        <? } ?>
                        <td class="content centered"><?=getInDollars($totals['district'])?></td>
                    <? } elseif($jur_groups_level=='club') {?>
                        <?if($_SESSION['jurisdiction_class']=='casino'){ ?>
                            <td class="content centered"><?=getInDollars($totals["virtual_bet"]-$totals['virtual_win'])?><br><?=getInDollars($totals["virtual_bet"]-$totals['virtual_win']-$totals['nation'])?></td>
                        <? } ?>
                        <?if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction_class']=='nation'){ ?>
                            <td class="content centered"><?=getInDollars($totals['nation'])?></td>
                        <? } if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction_class']=='nation' || $_SESSION['jurisdiction_class']=='region'  ){ ?>
                            <td class="content centered"><?=getInDollars($totals['region'])?></td>
                        <? } if($_SESSION['jurisdiction_class']=='casino' || $_SESSION['jurisdiction_class']=='nation' || $_SESSION['jurisdiction_class']=='region' || $_SESSION['jurisdiction_class']=='district'  ){ ?>
                            <td class="content centered"><?=getInDollars($totals['district'])?></td>
                        <? }
                        ?>
                        <td class="content centered"><?=getInDollars($totals['club'])?></td>
                    <?
                    } ?>

                </tr>
            </table>
            <br>
            <?
            $cell_fmt_arr = array (
                'virtual<br>bet'                => 'style="text-align:right;white-space: nowrap;" class="content light"',
                'virtual<br>net'                => 'style="text-align:right;white-space: nowrap;" class="content light"',
                'virtual<br>win'                => 'style="text-align:right;white-space: nowrap;" class="content light"',
                'virtual<br>fin'                => 'style="text-align:right;white-space: nowrap;" class="content light"',
                'financial'                     => 'style="text-align:right;white-space: nowrap;" class="content dark"',
                'Total Net Betting'             => 'style="text-align:right;white-space: nowrap;" class="content light"',
                'Net'                           => 'style="text-align:right;white-space: nowrap;" class="content light"',
                'nation'                        => 'style="white-space: nowrap;" ',
                'region'                        => 'style="white-space: nowrap;" ',
                'district'                      => 'style="white-space: nowrap;" ',
                'club'                          => 'style="white-space: nowrap;" '
            );
            $val_fmt_arr = array (
                'virtual<br>bet'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["virtual_bet"]));',
                'virtual<br>win'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["virtual_win"]));',
                'virtual<br>net'                => 'return  "<strong>".preg_replace("/\&nbsp; /", "",getInDollars($rec["virtual_rake"]))."<br>".($rec["jps_perc_virtual"])."% = ".preg_replace("/\&nbsp; /","",getInDollars($rec["virtual_rake"]/100* ($rec["jps_perc_virtual"])));',
                'virtual<br>fin'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["virtual_fin"]));',
                'financial'                     => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["fin"]));',
                'Casino profit'                 => 'return "<strong>".getInDollars($rec["virtual_rake"])."<br><span class=tip>".getInDollars($rec["virtual_rake"]/100*(100-$rec["jps_perc_virtual_n"]));',
                'Nation profit'                 => 'return "<strong>".$rec["jps_perc_virtual_n"]."% ="
                                            .($_SESSION["jurisdiction_class"]=="nation" ? getInDollars($rec["virtual_rake"]/100*$rec["jps_perc_virtual_n"])."<br><span class=tip>" : "")
                                            .getInDollars($rec["virtual_rake"]/100*($rec["jps_perc_virtual_n"]-$rec["jps_perc_virtual_r"]));',
                'Region profit'                 => 'return "<strong>".$rec["jps_perc_virtual_r"]."% ="
                                            .($_SESSION["jurisdiction_class"]=="region" ? getInDollars($rec["virtual_rake"]/100*$rec["jps_perc_virtual_r"])."<br><span class=tip>" : "")
                                            .getInDollars($rec["virtual_rake"]/100*($rec["jps_perc_virtual_r"]-$rec["jps_perc_virtual_d"]));',
                'District profit'               => 'return "<strong>".$rec["jps_perc_virtual_d"]."% ="
                                            .($_SESSION["jurisdiction_class"]=="district" ? getInDollars($rec["virtual_rake"]/100*$rec["jps_perc_virtual_d"])."<br><span class=tip>" : "")
                                            .getInDollars($rec["virtual_rake"]/100*($rec["jps_perc_virtual_d"]-$rec["jps_perc_virtual_c"]));',
            );
            $cols_arr = array();
            array_push($cols_arr, 'nation');
            $val_fmt_arr['nation']= 'return "<strong>" . $rec["n_jur_name"] . "</strong>";';
            $chartGroup='n_jur_name';
            if($jur_groups_level=='region')
            {
                array_push($cols_arr, 'region');
                $val_fmt_arr['region']= 'return "<strong>" . $rec["r_jur_name"] . "</strong><br /> <span class=\"tip\"> % </span>";';
                $chartGroup='r_jur_name';
            }
            if($jur_groups_level=='district')
            {
                array_push($cols_arr, 'region');
                array_push($cols_arr, 'district');
                $val_fmt_arr['region']= 'return "<strong>" . $rec["r_jur_name"] . "</strong>";';
                $val_fmt_arr['district']='return "<strong>" . $rec["d_jur_name"] . "</strong><br /><span class=\"tip\"> % </span>";';
                $chartGroup='d_jur_name';
            }
            if($jur_groups_level=='club')
            {
                array_push($cols_arr, 'region');
                array_push($cols_arr, 'district');
                array_push($cols_arr, 'club');
                $val_fmt_arr['district'] ='return "<strong>" . $rec["d_jur_name"] . "</strong>";';
                $val_fmt_arr['region'] = 'return "<strong>" . $rec["r_jur_name"] . "</strong>";';
                $val_fmt_arr['club'] = 'return "<strong>". $rec[c_jur_name]." </strong><br /><span class=\"tip\"> % </span>";';
                $chartGroup='c_jur_name';
            }

            if($virtual==1){
                if($betwin)
                {
                    array_push($cols_arr, 'virtual<br>bet');
                    array_push($cols_arr, 'virtual<br>win');
                }
                array_push($cols_arr,'virtual<br>net');
                if($financial)
                {
                    array_push($cols_arr, 'virtual<br>fin');
                }
            }
            $cols_arr=array_merge($cols_arr,$betting_jurisdictions);
            $post_vars = array('active'=>'virtual','doQuery'=>'yes','date_start'=>$date_start,'date_end'=>$date_end,'jurisdiction_level'=>$jurisdiction_level,'jurisdiction'=>$jur_sel_id,'jur_level'=>$jur_groups_level );
            $start_row = (isset ( $_POST ['start_row'] )) ? $_POST ['start_row'] : 1;
            $qp = new QueryPrinter($res);
            $qp->printTable($res,$cols_arr, $post_vars, $val_fmt_arr, $cell_fmt_arr, $sum_fmt_arr, $excel_filename, $header, $width="", $start_row, $row_to_page);
            }
            }?>
        </div>
    <? }

    if(check_access('financial_summary_user_details')){?>
        <div id="usersFinancial">
            <link rel="stylesheet" type="text/css" href="/media/jquery.multiselect.css" title="core_style">
            <script src="/media/jquery.multiselect.js" type="text/javascript"></script>
            <script type="text/javascript">

                $(function(){
                    $("#jurisdiction").multiselect({
                        multiple: false,
                        header: "Select an option",
                        noneSelectedText: "Select an Option",
                        selectedList: 1
                    }).multiselectfilter();
                });
                </script>
        <h2 class="centered"><?=$lang->getLang("Users Financial")?></h2>
        <p>
        <center>
            <br/>
            <p>
            <form action="/financial_summary" method="post" class="resubmitForm">
                <?php printHiddenInput('doQuery', 'yes');?>
                <?php printHiddenInput('search_type', 'date_range');?>
                <input type="hidden" name="active" value="usersFinancial">
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
                            <input type='text' name='date_start' id='date_start_uf' class="dateStartPicker" value="<?=$date_start?>" >
                        </td>
                    </tr>
                    <tr valign=top>
                        <td class=label><?=$lang->getLang("Until")?></td>
                        <td class=content>
                            <input type='date_end' name='date_end' id='date_end_uf' class="dateEndPicker" value="<?=$date_end?>">
                        </td>
                    </tr>
                    <tr valign=top>
                        <td class=label><?=$lang->getLang("Club")?></td>
                        <td class=content>
                            <?php
                            $result=getAllClubsAndDistricts();
                            $districts=array();
                            $jur_sel_id = $_POST['jurisdiction'];

                            while($result->hasNext()){
                                $row=$result->next();
                                if(!is_array($districts[$row['dis_jur_id']])){
                                    $districts[$row['dis_jur_id']]=array();
                                    $districts[$row['dis_jur_id']]['name']=$row['dis_jur_name'];
                                }
                                $districts[$row['dis_jur_id']]['clubs'][$row['cl_jur_id']]=$row['cl_jur_name'] ;
                            }
                            ?>
                            <select name="jurisdiction" id="jurisdiction">
                                <?foreach($districts as $district=>$clubs){ ?>
                                <?if($_SESSION['jurisdiction_class']!='club') { ?>
                                <optgroup label="<?=$clubs['name']?>" value="<?=$district?>"> <? }
                                    foreach($clubs['clubs'] as $club=>$v){ ?>
                                        <option value="<?=$club?>"   <?=($club==$jur_sel_id? 'selected' : '')?>><?=$v?></option>
                                    <?
                                    }
                                    } ?>
                            </select>
                        </td>
                    </tr>
                    <tr valign=top>
                        <td class=label><?=$lang->getLang("Financial contains")?></td>
                        <td class=content>
                            <label><input type='checkbox' name='casino' value="1" <?php if($casino==1) echo "checked"; ?> /><?=$lang->getLang("Casino")?></label>
                            <label><input type='checkbox' name='casino_live' value="1" <?php if($casino_live==1) echo "checked"; ?> /><?=$lang->getLang("Casino Live")?></label>
                            <label><input type='checkbox' name='virtual' value="1" <?php if($virtual==1) echo "checked"; ?> /><?=$lang->getLang("Virtual")?></label>
                            <label><input type='checkbox' name='bankroulette' value="1" <?php if($bankroulette==1) echo "checked"; ?> /><?=$lang->getLang("BankRoulette")?></label>
                            <label><input type='checkbox' name='zecchinetta' value="1" <?php if($zecchinetta==1) echo "checked"; ?> /><?=$lang->getLang("Zecchinetta")?></label>
                            <label><input type='checkbox' name='greek' value="1" <?php if($greek==1) echo "checked"; ?> /><?=$lang->getLang("Greek")?></label>
                            <label><input type='checkbox' name='zeus' value="1" <?php if($zeus==1) echo "checked"; ?> /><?=$lang->getLang("Zeus")?></label>
                            <label><input type='checkbox' name='royalpk' value="1" <?php if($royalpk==1) echo "checked"; ?> /><?=$lang->getLang("RoyalPk")?></label>
                            <label><input type='checkbox' name='playtech' value="1" <?php if($playtech==1) echo "checked"; ?> /><?=$lang->getLang("playtech")?></label>
                            <label><input type='checkbox' name='evolution' value="1" <?php if($evolution==1) echo "checked"; ?> /><?=$lang->getLang("evolution")?></label>
                            <label><input type='checkbox' name='infinity' value="1" <?php if($infinity==1) echo "checked"; ?> /><?=$lang->getLang("infinity")?></label>
                            <label><input type='checkbox' name='portomaso' value="1" <?php if($portomaso==1) echo "checked"; ?> /><?=$lang->getLang("portomaso")?></label>
                            <label><input type='checkbox' name='forex' value="1" <?php if($forex==1) echo "checked"; ?> /><?=$lang->getLang("Forex")?></label>
                            <label><input type='checkbox' name='totem' value="1" <?php if($totem==1) echo "checked"; ?> /><?=$lang->getLang("Totem")?></label>
                            <label><input type='checkbox' name='pokerl' value="1" <?php if($pokerl==1) echo "checked"; ?> /><?=$lang->getLang("Poker Live")?></label>
                            <label><input type='checkbox' name='poker2d' value="1" <?php if($poker2d==1) echo "checked"; ?> /><?=$lang->getLang("Poker 2d")?></label>
                        </td>
                    </tr>
                        <td class="label"><?=$lang->getLang("Result in Page")?></td>
                        <td class="content">
                            <select name="resforpage">
                                <option <?php if($row_to_page == 20){?> selected <?php }?>
                                    value="20">20</option>
                                <option <?php if($row_to_page == 50){?> selected <?php }?>
                                    value="50">50</option>
                                <option <?php if($row_to_page == 100){?> selected <?php }?>
                                    value="100">100</option>
                            </select>
                        </td>
                    </tr>
                    <tr>
                        <td></td>
                        <td><input type=image src="<?=image_dir?>/button_submit.gif" name="submit"></td>
                    </tr>
                </table>
            </form>
        </center>
          <?
        if(isset($_POST['doQuery']) && $active=='usersFinancial'){ ?>
            <script>
                $(function() {
                    var index = $('#tabs a[href="#usersFinancial"]').parent().index();
                        $( "#tabs" ).tabs( "option", "active", index );
                });
            </script>
            <div>
            </div>
            <?
            $datetime1 = new DateTime($date_start);
            $datetime2 = new DateTime($date_end);
            $interval = $datetime1->diff($datetime2);
            if($interval->days > 7){
                addError('',$lang->getLang('Please choose an interval of maximum one week'));
                showErrors();
            }
            else{
                getUsersFinancialDetailsByClub($jur_sel_id,$date_start,$date_end,$casino,$casino_live,$virtual,$poker2d,$pokerl,$royalpk,$playtech,$evolution,$infinity,$totem,$forex,$bankroulette,$zecchinetta,$greek,$zeus);
            }
        } ?>
        </p>
        </div>
    <? }

    if(check_access('financial_summary_partner')){
        $betting= (!isset($_POST['doQuery']) || $_POST['betting']==1 ||($active=='betting')) ? 1 : 0;
        ?>

        <div id="partnersFinancial">
            <link rel="stylesheet" type="text/css" href="/media/jquery.multiselect.css" title="core_style">
            <script src="/media/jquery.multiselect.js" type="text/javascript"></script>
            <script type="text/javascript">

                $(function(){
                    $("#partner").multiselect({
                        noneSelectedText: "Select an partner"
                    }).multiselectfilter();
                });
            </script>
            <h2 class="centered"><?=$lang->getLang("Partners Financial")?></h2>
            <p>
            <center>
                <br/>
                <p>
                <form action="/financial_summary" method="post" class="resubmitForm">
                    <?php printHiddenInput('doQuery', 'yes');?>
                    <?php printHiddenInput('search_type', 'date_range');?>
                    <input type="hidden" name="active" value="partnersFinancial">
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
                                <input type='text' name='date_start' id='date_start_pf' class="dateStartPicker" value="<?=$date_start?>" >
                            </td>
                        </tr>
                        <tr valign=top>
                            <td class=label><?=$lang->getLang("Until")?></td>
                            <td class=content>
                                <input type='date_end' name='date_end' id='date_end_pf' class="dateEndPicker" value="<?=$date_end?>">
                            </td>
                        </tr>
                        <tr valign=top>
                            <td class=label><?=$lang->getLang("Partners")?></td>
                            <td class=content>
                                <?php
                                $thisJurisdiction = JurisdictionsList::getInstance ( $dbh );
                                $allChildren=implode(",",$thisJurisdiction->getChilJurisdictionsIdList($_SESSION['jurisdiction_id']));
                                $partners=$dbh->exec("SELECT ptn_id,ptn_name from partners where ptn_jur_id in (".$allChildren.")");
                                    ?>
                                <select name="partner[]" id="partner" multiple="multiple">
                                <?
                                while($partners->hasNext()){
                                    $row=$partners->next();
                                    ?>
                                    <option value="<?=$row['ptn_id']?>" <?=(is_array($partner)&&in_array($row['ptn_id'],$partner))  ? 'selected' : ''?>  ><?=$row['ptn_name']?></option>
                                    <?
                                }
                                ?>
                                </select>
                            </td>
                        </tr>
                        <?if(check_access("financial_summary_details_betwin")) {?>
                            <tr valign=top>
                                <td class=label><?=$lang->getLang("Details")?></td>
                                <td class=content>
                                    <?if(check_access("financial_summary_details_betwin")){ ?>
                                        <input type='checkbox' name='betwin' id='betwin_betting' <?php if(isset($_POST['betwin'])&& $_POST['betwin']==true) echo "checked"; ?> /><?=$lang->getLang("Bet & Win")?>
                                    <? } ?>
                                </td>
                            </tr>
                        <? } ?>
                        <tr valign=top>
                            <td class=label><?=$lang->getLang("Financial contains")?></td>
                            <td class=content>
                                <label><input type='checkbox' name='casino' value="1" <?php if($casino==1) echo "checked"; ?> /><?=$lang->getLang("Casino")?></label>
                                <label><input type='checkbox' name='royalpk' value="1" <?php if($royalpk==1) echo "checked"; ?> /><?=$lang->getLang("Royal Pk")?></label>
                                <label><input type='checkbox' name='pokerl' value="1" <?php if($pokerl==1) echo "checked"; ?> /><?=$lang->getLang("Poker Live")?></label>
                                <label><input type='checkbox' name='poker2d' value="1" <?php if($poker2d==1) echo "checked"; ?> /><?=$lang->getLang("Poker 2d")?></label>
                                <label><input type='checkbox' name='forex' value="1" <?php if($forex==1) echo "checked"; ?> /><?=$lang->getLang("Forex")?></label>
                                <label><input type='checkbox' name='betting' value="1" <?php if($betting==1) echo "checked"; ?> /><?=$lang->getLang("Betting")?></label>
                            </td>
                        </tr>
                        <tr><td class="label"><?=$lang->getLang("Result in Page")?></td>
                        <td class="content">
                            <select name="resforpage">
                                <option <?php if($row_to_page == 20){?> selected <?php }?>
                                    value="20">20</option>
                                <option <?php if($row_to_page == 50){?> selected <?php }?>
                                    value="50">50</option>
                                <option <?php if($row_to_page == 100){?> selected <?php }?>
                                    value="100">100</option>
                            </select>
                        </td>
                        </tr>
                        <tr>
                            <td></td>
                            <td><input type=image src="<?=image_dir?>/button_submit.gif" name="submit"></td>
                        </tr>
                    </table>
                </form>
            </center>
            <?
            if(isset($_POST['doQuery']) && $active=='partnersFinancial'){ ?>
                <script>
                    $(function() {
                        var index = $('#tabs a[href="#partnersFinancial"]').parent().index();
                        $( "#tabs" ).tabs( "option", "active", index );

                        $("#sendReportByEmail").on('click',function(){
                            email=$('#emailToSend').val();
                            $.post("services/general_services.inc",{action:"sendFinancialReport",email:email}, function(data){
                               if(data==1){
                                   jAlert('<?=$lang->getLang("Mail sent successfully")?>',"Success");
                               }
                                else{
                                   jAlert('<?=$lang->getLang("Mail not sent")?>')
                               }
                            });
                        });
                    });
                </script>
                <div>
                </div>
                <?
                $datetime1 = new DateTime($date_start);
                $datetime2 = new DateTime($date_end);
                $interval = $datetime1->diff($datetime2);
                if($interval->y> 1){
                    addError('',$lang->getLang('Please choose an interval of maximum one year'));
                    showErrors();
                }
                elseif(empty($partner)){
                    addError('',$lang->getLang('Please choose at least one partner'));
                    showErrors();
                }
                elseif(count(array_diff_key ( $partner , $partners->Records ))>0){
                    addError('',$lang->getLang('Invalid partners'));
                    showErrors();
                }
                elsE{
                    $starttime = microtime(true);
                    $result=getPartnersFinancial($partner,$date_start,$date_end,$casino,$pokerl,$royalpk);
                    $endtime = microtime(true);
                    $duration = $endtime - $starttime;
                    $duration=number_format($duration, 4, ',', '')."s";

                    ?>
                    <br /><div class="tip fleft"><?=$lang->getLang("Time taken to execute your request")?>: <?=$duration?></div><br />
                    <?

                    $datetime1 =  date("Y-m",strtotime($date_start));
                    $datetime2 =  date("Y-m",strtotime($date_end));
                    $sameMonth = ($datetime1==$datetime2);

                    if($sameMonth){
                        $cols_arr=array();
                        $val_fmt_arr['date'] = 'return ($rec["month"]);';
                        array_push($cols_arr,'date');
                    }
                    else{
                        $cols_arr=$cell_fmt_arr=$val_fmt_arr=array();
                    }

                    $cell_fmt_arr ['name'] = 'style="text-align:right;white-space: nowrap;" class="content dark"';

                    if($casino){
                        $cell_fmt_arr['casino bet'] = 'style="text-align:right;white-space: nowrap;" class="content light"';
                        $cell_fmt_arr['casino win'] = 'style="text-align:right;white-space: nowrap;" class="content dark"';
                        $cell_fmt_arr['casino net'] = 'style="text-align:right;white-space: nowrap;" class="content light"';
                    }
                    if($royalpk){
                        $cell_fmt_arr['royalpk bet'] = 'style="text-align:right;white-space: nowrap;" class="content light"';
                        $cell_fmt_arr['royalpk win'] = 'style="text-align:right;white-space: nowrap;" class="content dark"';
                        $cell_fmt_arr['royalpk net'] = 'style="text-align:right;white-space: nowrap;" class="content light"';
                    }
                    if($forex){
                        $cell_fmt_arr['forex bet'] = 'style="text-align:right;white-space: nowrap;" class="content light"';
                        $cell_fmt_arr['forex win'] = 'style="text-align:right;white-space: nowrap;" class="content dark"';
                        $cell_fmt_arr['forex net'] = 'style="text-align:right;white-space: nowrap;" class="content light"';
                    }

                    if($pokerl){
                        $cell_fmt_arr['poker live bet']     = 'style="text-align:right;white-space: nowrap;" class="content light" ';
                        $cell_fmt_arr['poker live win']     = 'style="text-align:right;white-space: nowrap;" class="content dark"';
                        $cell_fmt_arr['poker live diff']    = 'style="text-align:right;white-space: nowrap;" class="content dark"';
                        $cell_fmt_arr['poker live rake']    = 'style="text-align:right;white-space: nowrap;" class="content light"';
                    }

                    if($poker2d){
                        $cell_fmt_arr['poker 2d bet']     = 'style="text-align:right;white-space: nowrap;" class="content light" ';
                        $cell_fmt_arr['poker 2d win']     = 'style="text-align:right;white-space: nowrap;" class="content dark"';
                        $cell_fmt_arr['poker 2d net']     = 'style="text-align:right;white-space: nowrap;" class="content dark"';
                        $cell_fmt_arr['poker 2d diff']    = 'style="text-align:right;white-space: nowrap;" class="content dark"';
                        $cell_fmt_arr['poker 2d rake']    = 'style="text-align:right;white-space: nowrap;" class="content light"';
                    }

                        $val_fmt_arr ['name']     = 'return ($rec["name"]);';
                        $val_fmt_arr['conversion']= 'return (($rec["currency"]==$_SESSION["currency_id"])?"<span class=tip>none</span>":$rec["cur_code_for_web"]."(".$_SESSION["currencies"][$rec["currency"]]["value"].")");';

                    if($casino){
                        $val_fmt_arr['casino bet'] = 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_bet"]));';
                        $val_fmt_arr['casino win'] = 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_win"]));';
                        $val_fmt_arr['casino net'] = 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_net"]));';
                    }

                    if($royalpk){
                        $val_fmt_arr['royalpk bet'] = 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["royalpk_bet"]));';
                        $val_fmt_arr['royalpk win'] = 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["royalpk_win"]));';
                        $val_fmt_arr['royalpk net']= 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["royalpk_rake"]));';
                    }
                   if($forex){
                        $val_fmt_arr['forex bet'] = 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["forex_bet"]));';
                        $val_fmt_arr['forex win'] = 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["forex_win"]));';
                        $val_fmt_arr['forex net']= 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["forex_rake"]));';
                    }
                   if($betting){
                        $val_fmt_arr['betting bet'] = 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_bet"]));';
                        $val_fmt_arr['betting win'] = 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_win"]));';
                        $val_fmt_arr['betting net']= 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_rake"]));';

                        $val_fmt_arr['betting live bet'] = 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_live_bet"]));';
                        $val_fmt_arr['betting live win'] = 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_live_win"]));';
                        $val_fmt_arr['betting live net']= 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["betting_live_rake"]));';
                    }
                    if($pokerl){
                        $val_fmt_arr['poker live bet'] = 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["poker_live_bet"]));';
                        $val_fmt_arr['poker live win'] = 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["poker_live_win"]));';
                        $val_fmt_arr['poker live diff']= 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["poker_live_diff"]));';
                        $val_fmt_arr['poker live rake']= 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["poker_live_rake"]));';
                    }
                    if($poker2d){
                        $val_fmt_arr['poker 2d bet'] = 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["poker_bet"]));';
                        $val_fmt_arr['poker 2d win'] = 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["poker_win"]));';
                        $val_fmt_arr['poker 2d diff']= 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["poker_diff"]));';
                        $val_fmt_arr['poker 2d rake']= 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["poker_rake"]));';
                    }




                    array_push($cols_arr,'name');
                    array_push($cols_arr,'conversion');

                    if($casino){
                        if($betwin) {
                            array_push($cols_arr, 'casino bet');
                            array_push($cols_arr, 'casino win');
                        }
                        array_push($cols_arr,'casino net');
                    }
                    if($royalpk){
                         if($betwin) {
                             array_push($cols_arr, 'royalpk bet');
                             array_push($cols_arr, 'royalpk win');
                         }
                        array_push($cols_arr,'royalpk net');
                    }
                    if($forex){
                         if($betwin) {
                             array_push($cols_arr, 'forex bet');
                             array_push($cols_arr, 'forex win');
                         }
                        array_push($cols_arr,'forex net');
                    }
                if($betting){
                     if($betwin) {
                         array_push($cols_arr, 'betting bet');
                         array_push($cols_arr, 'betting win');
                     }
                        array_push($cols_arr,'betting net');
                     if($betwin) {
                         array_push($cols_arr, 'betting live bet');
                         array_push($cols_arr, 'betting live win');
                     }
                        array_push($cols_arr,'betting live net');
                    }
                if($pokerl){
                     if($betwin) {
                         array_push($cols_arr, 'poker live bet');
                         array_push($cols_arr, 'poker live win');
                     }
                        array_push($cols_arr,'poker live diff');
                        array_push($cols_arr,'poker live rake');
                    }

                    if($poker2d){
                         if($betwin) {
                             array_push($cols_arr, 'poker 2d bet');
                             array_push($cols_arr, 'poker 2d win');
                         }
                        array_push($cols_arr,'poker 2d diff');
                        array_push($cols_arr,'poker 2d rake');
                    }
                    if($result->getNumRows()>0){
                        ?>
                        <br /><table><tr><td class="label">Email to send to:</td><td>
                                    <input name="emailToSend" type="text" id="emailToSend" value="" >
                                </td><td><button id="sendReportByEmail" class='entitybtn'>Send</button></td></tr></table>
                        <?
                    }

                    $filename=md5($_SESSION['admin_username']."financial_partners").".csv";

                    $fp = fopen("documents/".$filename, 'w');
                    fputcsv($fp, array("DATE START: ".$date_start,"DATE END: ".$date_end,"MAIN CURRENCY:".$_SESSION['currency']));
                    fputcsv($fp, array(" "," "," "));
                    $headers=array();
                    if($sameMonth){
                        array_push($headers,"DATE");
                    }
                    array_push($headers,"NAME");
                    array_push($headers,"CONVERSION RATE");
                 if($betwin) {
                     array_push($headers, "CASINO BET");
                     array_push($headers, "CASINO WIN");
                 }
                    array_push($headers,"CASINO NET");
                 if($betwin) {
                     array_push($headers, "ROYALPK BET");
                     array_push($headers, "ROYALPK WIN");
                 }
                    array_push($headers,"ROYALPK NET");
                 if($betwin) {
                     array_push($headers, "FOREX BET");
                     array_push($headers, "FOREX WIN");
                 }
                    array_push($headers,"FOREX NET");
                if($betwin) {
                    array_push($headers, "BETTING BET");
                    array_push($headers, "BETTING WIN");
                }
                    array_push($headers,"BETTING NET");
                 if($betwin) {
                     array_push($headers, "BETTING LIVE BET");
                     array_push($headers, "BETTING LIVE WIN");
                 }
                    array_push($headers,"BETTING LIVE NET");
                   if($betwin) {
                       array_push($headers, "POKER LIVE BET");
                       array_push($headers, "POKER LIVE WIN");
                   }
                    array_push($headers,"POKER LIVE DIFF");
                    array_push($headers,"POKER LIVE RAKE");
                 if($betwin) {
                     array_push($headers, "POKER 2D BET");
                     array_push($headers, "POKER 2D WIN");
                 }
                    array_push($headers,"POKER 2D DIFF");
                    array_push($headers,"POKER 2D RAKE");
                    fputcsv($fp,$headers);
                    fputcsv($fp, array(" "," "," "));
                    fputcsv($fp, array(" "," "," "));
                    $totals=array();

                    while ( $result->hasNext () ) {
                        $row = $result->next ();
                        if($row["currency"]!=$_SESSION["currency_id"]){
                            foreach($row as $n=>$nv){
                                if(((strpos($n,'bet') !== false) || (strpos($n,'win') !== false) || (strpos($n,'net') !== false) || (strpos($n,'rake') !== false) || (strpos($n,'fin') !== false))){
                                    if($nv!=0){
                                        if($_SESSION["currency_id"]==1){
                                            $result->Records[$result->CurrentIndex][$n]=$nv/$_SESSION["currencies"][$row["currency"]]["value"];
                                            $row[$n]=$nv/$_SESSION["currencies"][$row["currency"]]["value"];
                                        }
                                        else{
                                            $result->Records[$result->CurrentIndex][$n]=$nv*$_SESSION["currencies"][$_SESSION["currency_id"]]["value"];
                                            $row[$n]=$nv*$_SESSION["currencies"][$_SESSION["currency_id"]]["value"];
                                        }
                                    }
                                }
                            }
                        }
                            $section=array();
                        if($sameMonth){
                            $section['date']=$row['month'];
                        }

                            $section['name']=$row['name'];
                            $section['conversion']=(($row["currency"]==$_SESSION["currency_id"])? "none":$row["cur_code"]."(".$_SESSION["currencies"][$row["currency"]]["value"].")");
                            if($casino){

                                if($betwin) {
                                    $section['casino_bet']=number_format($row['casino_bet'],2);
                                    $section['casino_win']=number_format($row['casino_win'],2);
                                    $totals['casino bet'] += $row['casino_bet'];
                                    $totals['casino win'] += $row['casino_win'];
                                }
                                $section['casino_net']=number_format($row['casino_net'],2);
                                $totals['casino net']+=$row['casino_net'];
                            }
                        if($royalpk){

                             if($betwin) {
                                 $section['royalpk_bet']=number_format($row['royalpk_bet'],2);
                                 $section['royalpk_win']=number_format($row['royalpk_win'],2);
                                 $totals['royalpk bet'] += $row['royalpk_bet'];
                                 $totals['royalpk win'] += $row['royalpk_win'];
                             }
                            $section['royalpk_rake']=number_format($row['royalpk_rake'],2);
                            $totals['royalpk net']+=$row['royalpk_rake'];
                        }
                        if($forex){

                             if($betwin) {
                                 $section['forex_bet']=number_format($row['forex_bet'],2);
                                 $section['forex_win']=number_format($row['forex_win'],2);
                                 $totals['forex bet'] += $row['forex_bet'];
                                 $totals['forex win'] += $row['forex_win'];
                             }
                            $section['forex_rake']=number_format($row['forex_rake'],2);
                            $totals['forex net']+=$row['forex_rake'];
                        }
                        if($betting){


                           if($betwin) {
                               $section['betting_bet']=number_format($row['betting_bet'],2);
                               $section['betting_win']=number_format($row['betting_win'],2);
                               $totals['betting bet'] += $row['betting_bet'];
                               $totals['betting win'] += $row['betting_win'];
                           }
                            $section['betting_rake']=number_format($row['betting_rake'],2);
                            $totals['betting net']+=$row['betting_rake'];
                            if($betwin) {
                              $section['betting_live_bet']=number_format($row['betting_live_bet'],2);
                              $section['betting_live_win']=number_format($row['betting_live_win'],2);
                               $totals['betting live bet']+=$row['betting_live_bet'];
                               $totals['betting live win']+=$row['betting_live_win'];
                           }
                            $section['betting_live_rake']=number_format($row['betting_live_rake'],2);
                            $totals['betting live net']+=$row['betting_live_rake'];
                        }

                            if($pokerl){
                                if($betwin) {
                                    $section['poker_live_bet']=number_format($row['poker_live_bet'],2);
                                    $section['poker_live_win']=number_format($row['poker_live_win'],2);
                                    $totals['poker live bet'] += $row['poker_live_bet'];
                                    $totals['poker live win'] += $row['poker_live_win'];
                                }
                                $section['poker_live_diff']=number_format($row['poker_live_diff'],2);
                                $section['poker_live_rake']=number_format($row['poker_live_rake'],2);
                                $totals['poker live diff']+=$row['poker_live_diff'];
                                $totals['poker live rake']+=$row['poker_live_rake'];
                            }

                        if($poker2d){

                            if($betwin) {
                                $section['poker_bet']=number_format($row['poker_bet'],2);
                                $section['poker_win']=number_format($row['poker_win'],2);
                                $totals['poker bet'] += $row['poker_bet'];
                                $totals['poker win'] += $row['poker_win'];
                            }
                            $section['poker_diff']=number_format($row['poker_diff'],2);
                            $section['poker_rake']=number_format($row['poker_rake'],2);
                            $totals['poker diff']+=$row['poker_diff'];
                            $totals['poker rake']+=$row['poker_rake'];
                        }
                        fputcsv($fp, $section);

                    }
                    fclose($fp);
                $columns=array_keys($totals);

                ?>
                <br /><table style="width:100%" bgcolor="#a99a9" cellpadding="4" cellspacing="1" border="0"><thead><tr>
                <?
               foreach($columns as $k=>$v) {
                   ?>
                   <th class="label"><?=$v?></th>
                   <?
               }
                ?>
                </tr></thead><tr>
                    <?
                    foreach($totals as $k=>$v) {
                        ?>
                        <td class="content"><?=getInDollars($v)?></td>
                        <?
                    }
                    ?>
                </tr>
                </table>
                    <?
                    $qp = new QueryPrinter($result);
                   $qp->printTable($result,$cols_arr, $post_vars, $val_fmt_arr, $cell_fmt_arr, '', '', '', $width="100%", $start_row, $row_to_page,false,"",false);
                    //$qp->dumpToExcelFile($result,$cols_arr,$val_fmt_arr,$excel_filename,$header);
                }
            } ?>
            </p>
        </div>
    <? }?>

    </div>
<? }


?>