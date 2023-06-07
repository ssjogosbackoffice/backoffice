<?
check_access('virtual_manage_closure',true);
$virtualCommissionsMonthly=check_access('virtual_monthly_closure');
$virtualCommissionsWeekly=check_access('virtual_daily_closure');
require_once('virtual_functions.inc');
$class=getSubJurisdiction($_SESSION['jurisdiction_class']);
$jurisdiction=$_SESSION['jurisdiction_id'];
$club=(isset($_POST['clubs'])? $_POST['clubs']:$jurisdiction);
$virtualHierarchy=getVirtualHierarchy();
$virtualClubs=getVirtualJurisdiction($jurisdiction,$class);
$date_start= (isset($_POST['date_start'])) ? $_POST['date_start'] : date('Y-m');
$type= (isset($_POST['type'])) ? $_POST['type'] : 'A';
$catre= (isset($_POST['catre'])) ? $_POST['catre'] : '0';
$lostP= (isset($_POST['lostP'])) ? $_POST['lostP'] : '0';
$manageTax= (isset($_POST['manageTax'])) ? $_POST['manageTax'] : '0';
$brutOrNet= (isset($_POST['brutornet'])) ? $_POST['brutornet'] : '0';
$active = (isset($_POST['active'])) ? $_POST['active'] : 'monthly';
?>
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/bootstrap-datepicker/css/bootstrap-datepicker.css" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
    <script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/bootstrap/bootstrap-datepicker/js/bootstrap-datepicker.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/jstree/jstree.js" type="text/javascript"></script>
    <script src="/media/jquery.dataTables.js" type="text/javascript"></script>
    <script>

        $(function() {

            $('.nav a:first').tab('show');
            $( "#date_start" ).datepicker({
                format: 'yyyy-mm',
                minViewMode: 1
            });

            $( ".dateStartHelper" ).datepicker({
                format: 'yyyy-mm-dd',
                numberOfMonths:1,
                daysOfWeekHighlighted: "1",
                todayHighlight: true
            });
            $('#clubs').select2({
                width: "158px"
            }).on('change',function(){
                $('#jurisdictionTree').jstree('deselect_all').jstree('select_node', $(this).val());
            });

            $('#SearchClosure').click(function() {
                jurisdictions=$('#clubs').val();
                if(typeof jurisdictions=='undefined'){
                    jurisdictions= $('#jurisdictionTree').jstree().get_selected();
                }
                if(jurisdictions==''){
                    jAlert('<?=$lang->getLang("Please select at least one jurisdiction!")?>');
                }
                else{
                    $.post("/virtual/virtual_functions.inc",{
                        action:"searchClosure",
                        date:$('#date_start_closure').val(),
                        jurisdictions:JSON.stringify(jurisdictions)
                    }, function(data){
                        $("#closureResult").empty().append(data);
                        $('#saveCommissionsClosure').on('click',  function(){
                            raportz=$('#raportz').val();
                            plati=$('#plati').val();
                            tcatre=$('#tcatre').val();
                            tdin=$('#tdin').val();
                            tdsediu=$('#tdsediu').val();
                            tdiffprofit=$('#tdiffprofit').val();
                            tdiffpierdere=$('#tdiffpierdere').val();
                            anulate=$('#anulate').val();
                            raportfiscal=$('#raportfiscal').val();
                            time=$('#timeOfClosure').val();
                            club=$('#clubCodeOfClosure').val();

                            $.post("/virtual/virtual_functions.inc",{
                                action:"saveVirtualCommissions",
                                raportz:raportz,
                                plati:plati,
                                tcatre:tcatre,
                                tdin:tdin,
                                tdsediu:tdsediu,
                                tdiffprofit:tdiffprofit,
                                tdiffpierdere:tdiffpierdere,
                                anulate:anulate,
                                raportfiscal:raportfiscal,
                                club:club,
                                time:time
                            },function(data){
                               if(data==1){
                                   $('#SearchClosure').trigger('click');
                               }
                               else if(data==-2){
                                   jAlert('<?=$lang->getLang("Your session has expired.Please login again")?>');
                               }
                                else{
                                   jAlert('<?=$lang->getLang("Unable to save modifies")?>');
                               }
                            });
                        });
                    });
                }
            });
            $('#jurisdictionTreeSearch').keyup(function () {
                if(this.value.length>2){
                    $("#jurisdictionTree").jstree('search',$('#jurisdictionTreeSearch').val());
                }
                else{
                    $("#jurisdictionTree").jstree("clear_search");
                    $('#jurAlert').fadeOut();
                }
            });
            $("#jurisdictionTree").jstree({
                "plugins":[
                    "search",
                    "types"
                ],
                "search": {
                    "fuzzy": false,
                    "show_only_matches": true
                },
                "types": {
                    "nation": {
                        "icon": "../media/images/n.png"
                    },
                    "region": {
                        "icon": "../media/images/r.png"
                    },
                    "district": {
                        "icon": "../media/images/d.png"
                    },
                    "club": {
                        "icon": "../media/images/c.png"
                    }
                }
            }).bind("select_node.jstree", function (e, data) {
                $("#clubs").select2("val", data.node.id);

            }).bind("search.jstree", function (e, data) {
                if (data.nodes.length == 0) {
                    $('#jurAlert').fadeIn();
                }
                else {
                    $('#jurAlert').fadeOut();
                }
            }).jstree('deselect_all').jstree('select_node', '<?=$club?>');;

        });
    </script>
    <div class="container" style="position: absolute;left:0;right: 0;margin:20px;width:98%" >
        <table class="table table-bordered">
            <tr>
                <td class="navbar-inner" colspan="2" ><h3 class="text-center"><?=$lang->getLang("Virtual Closure")?></h3></td>
            </tr>
            <tr>
                <td class="span4">
                    <div class="contentAdmin">
                        <div class="contentHeadAdmin"><?=$lang->getLang("Jurisdiction chooser")?></div>
                        <div class="well">
                            <div class="alert hidden alert-danger" id="jurAlert">
                                <?=$lang->getLang("No club found")?>
                            </div>
                            <input type="text" id='jurisdictionTreeSearch' placeholder="<?=$lang->getLang("Search a jurisdiction")?>">
                            <div id="jurisdictionTree">
                                <ul>

                                    <?=showChild($virtualHierarchy,true,true);?>
                                   <!-- <?php
/*                                    while($virtualHierarchy->hasNext())
                                    {
                                        $row=$virtualHierarchy->next(); */?>
                                    <li  data-jstree='{ "type" : "club" }'  id="<?/*=$row['club_id']*/?>" <?/*=($row['club_id']==$club?"selected=selected":'')*/?> >[<?/*=$row['club_code']*/?>]<?/*=$row['club']*/?> </li>--><?php
/*                                    }
                                    $virtualHierarchy->resetRecPtr();
                                    */?>
                                </ul>
                            </div>
                        </div>
                    </div>
                </td>
                <td>
                    <div class="well">
                        <div class="tabbable">
                            <ul class="nav nav-tabs">
                                <?if($virtualCommissionsMonthly) { ?>
                                    <li><a href="#monthly" data-toggle="tab"><button class="btn btn-inverse"><?=$lang->getLang("Monthly Closure")?></button></a></li>
                                <? } if($virtualCommissionsWeekly) { ?>
                                    <li><a href="#weekly" data-toggle="tab" ><button class="btn btn-inverse" ><?=$lang->getLang("Daily closure")?></button></a></li>
                                <? } ?>
                            </ul>
                            <div class="tab-content">
                                <?if($virtualCommissionsMonthly) { ?>
                                    <div class="tab-pane active" id="monthly">
                                        <form action="/virtual_commissions" method="post">
                                            <div class="row">
                                                <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Date")?></div>
                                                    <br>
                                                    <div class="controls">
                                                        <div class="input-prepend">
                                                            <span class="add-on"><i class="icon-random"></i></span>
                                                            <input type='text' name='date_start' id='date_start'  value="<?=$date_start?>" >
                                                        </div>
                                                    </div>
                                                </div>
                                                <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Club")?></div>
                                                    <br>
                                                    <div class="controls">
                                                        <div class="input-prepend">
                                                            <select name='clubs' id="clubs">
                                                                <?php
                                                                while($virtualClubs->hasNext())
                                                                {
                                                                    $row=$virtualClubs->next(); ?>
                                                                <option  value="<?=$row['club_id']?>" <?=($row['club_id']==$club?"selected=selected":'')?> >[<?=$row['club_code']?>]<?=$row['club']?> </option><?php
                                                                }
                                                                ?>
                                                            </select>
                                                        </div>
                                                    </div>
                                                </div>
                                                <div class="span2"><div class='label label-inverse'><?=$lang->getLang("Type")?></div>
                                                    <br>
                                                    <div class="controls">
                                                        <div class="input-prepend">
                                                            <span class="add-on"><i class="icon-random"></i></span>
                                                            <select name='type' id="type">
                                                                <option  value="a" <?=($type=='a' ? "selected=selected":'')?> >50-50</option>
                                                                <option  value="b" <?=($type=='b' ? "selected=selected":'')?> >B</option>
                                                                <option  value="c" <?=($type=='c' ? "selected=selected":'')?> >C</option>
                                                            </select>
                                                        </div>
                                                    </div>
                                                </div>
                                                <div class="span3"><div class='label label-inverse'><?=$lang->getLang("To")?></div>
                                                    <br>
                                                    <div class="controls">
                                                        <div class="input-prepend">
                                                            <span class="add-on"><i class="icon-random"></i></span>
                                                            <select name='catre' id="catre">
                                                                <option  value="0" <?=($catre=='0' ? "selected=selected":'')?> ><?=$lang->getLang("Supplier")?></option>
                                                                <option  value="1" <?=($catre=='1' ? "selected=selected":'')?> ><?=$lang->getLang("Associate participant")?></option>
                                                            </select>
                                                        </div>
                                                    </div>
                                                </div>
                                                <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Participa la pierdere")?></div>
                                                    <br>
                                                    <div class="controls">
                                                        <div class="input-prepend">
                                                            <span class="add-on"><i class="icon-random"></i></span>
                                                            <select name='lostP' id="lostP">
                                                                <option  value="0" <?=($lostP=='0' ? "selected=selected":'')?> ><?=$lang->getLang("Nu Participa")?></option>
                                                                <option  value="1" <?=($lostP=='1' ? "selected=selected":'')?> ><?=$lang->getLang("Participa")?></option>
                                                            </select>
                                                        </div>
                                                    </div>
                                                </div>

                                                <!--  <div class="span3"><div class='label label-inverse'><?/*=$lang->getLang("Manage 5% tax")*/?></div>
                            <br>
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-random"></i></span>
                                    <select name='manageTax' id="lostP">
                                        <option  value="0" <?/*=($manageTax=='0' ? "selected=selected":'')*/?> ><?/*=$lang->getLang("Manage")*/?></option>
                                        <option  value="1" <?/*=($manageTax=='1' ? "selected=selected":'')*/?> ><?/*=$lang->getLang("Don't Manage")*/?></option>
                                    </select>
                                </div>
                            </div>
                        </div>-->
                                                <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Procent based on")?></div>
                                                    <br>
                                                    <div class="controls">
                                                        <div class="input-prepend">
                                                            <span class="add-on"><i class="icon-random"></i></span>
                                                            <select name='brutornet' id="brutornet">
                                                                <option  value="0" <?=($brutOrNet=='0' ? "selected=selected":'')?> ><?=$lang->getLang("Gross income")?></option>
                                                                <option  value="1" <?=($brutOrNet=='1' ? "selected=selected":'')?> ><?=$lang->getLang("Gross net")?></option>
                                                            </select>
                                                        </div>
                                                    </div>
                                                </div>
                                            </div>
                                            <div class="row">
                                                <span class="span2"> <button class="btn btn-primary" id="SearchTicket"><?=$lang->getLang("Search")?></button></span>
                                            </div><br />

                                        </form>


                                        <?
                                        $starttime = microtime(true);
                                        if(isset($_POST['date_start'])) {
                                            if($type=='a') {
                                                $results = (calculateAffiliateCommissions($club, $date_start,50,50));
                                                $resultsBetting=calculateAffiliateCommissionsBetting($club,$date_start,50);
                                                $resultsNumere=calculateAffiliateCommissionsNumbers($club,$date_start,50);
                                            }
                                            else{
                                                $results = (calculateAffiliateCommissions($club, $date_start));
                                                $resultsBetting=calculateAffiliateCommissionsBetting($club,$date_start);
                                                $resultsNumere=calculateAffiliateCommissionsNumbers($club,$date_start);
                                            }
                                            if($type=='c') {
                                                $results = (calculateAffiliateCommissions($club, $date_start));
                                                $resultsNumere = (calculateAffiliateCommissionsNumbers($club, $date_start));
                                                $detailedResults=(calculateAffiliateCommissionsDetailed($club,$date_start));
                                                $resultsBetting=calculateAffiliateCommissionsBetting($club,$date_start);
                                            }

                                            $endtime = microtime(true);
                                            $duration = $endtime - $starttime;
                                            $duration=number_format($duration, 4, ',', '')."s";

                                            $datesInterval=getBettingInterval($date_start,'monthly');
                                            $date_start_interval=$datesInterval['start'];
                                            $date_end_interval=$datesInterval['end'];

                                            ?>
                                            <div class="tip fleft"><?=$lang->getLang("Time taken to execute your request")?>: <?=$duration?></div>  <br />  <br />
                                        <?$agencyInfo=getAgencyDetails($club)?>
                                            <button id="printButton" class="entitybtn">Print</button>
                                            <script>
                                                $(document).on('click','#printButton',function() {
                                                    var prtContent = document.getElementById("printDiv");
                                                    var WinPrint = window.open('', '', 'left=0,top=0,width=800,height=900,toolbar=0,scrollbars=0,status=0');
                                                    WinPrint.document.write('<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/style.css" title="core_style" />');
                                                    WinPrint.document.write('<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />');
                                                    WinPrint.document.write(' <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />');



                                                    WinPrint.document.write("<table class='table table-bordered'><tr></tr><tr><td><div>" );
                                                    WinPrint.document.write(prtContent.innerHTML);
                                                    WinPrint.document.close();
                                                    WinPrint.focus();
                                                    WinPrint.print();
                                                    WinPrint.close();
                                                });
                                            </script>

                                            <div id="printDiv">
                                                <div>SC RG FAIRPLAY SRL</div><br />
                                                <div>Agentie <?=$agencyInfo['club_address']?></div>
                                                <div>Codificare Agentie <?=$agencyInfo['club']?></div>
                                                <div>Decont luna <?=$lang->getLang(date("F",strtotime($date_start)))?> <?=date("Y",strtotime($date_start))?> </div>
                                                <div>Interval: <?=$date_start_interval ?> - <?=$date_end_interval?> </div><br /><br />

                                                <table>
                                                    <tr><td><?=$lang->getLang('Category')?></td><td><?=$lang->getLang('Gross income')?></td><td><?=$lang->getLang('Tax')?> 5%</td><td><?=$lang->getLang('Win paid')?></td><td><?=$lang->getLang('Win paid tax')?></td><td><?=$lang->getLang('Gross Net')?></td></tr>
                                                    <tr><td><?=$lang->getLang('sport')?></td> <td class="financial_td"><?=getInDollars($resultsBetting['totalBrutSport'],2,$resultsBetting['cur_code_for_web'])?> <?=$resultsBetting['cur_code_for_web']?></td><td class="financial_td"><?=getInDollars($resultsBetting['totalTaxSport'])?></td><td class="financial_td"><?=getInDollars($resultsBetting['totalPlataJucatoriSport'])?></td><td class="financial_td"><?=getInDollars($resultsBetting['totalPlataJucatoriTaxaSport'])?></td><td class="financial_td"><?=getInDollars($resultsBetting['totalNetSport'])?></td></tr>
                                                    <tr><td><?=$lang->getLang('nonsport')?></td><td class="financial_td"><?=getInDollars($results['totalBrutNonSport'])?></td> <td class="financial_td"><?=getInDollars($results['totalTaxNonSport'])?></td><td class="financial_td"><?=getInDollars($results['totalPlataJucatoriNonSport'])?></td><td class="financial_td"><?=getInDollars($results['totalPlataJucatoriTaxaNonSport'])?></td><td class="financial_td"><?=getInDollars($results['totalNetNonSport'])?></td></tr>
                                                    <tr><td><?=$lang->getLang('numbers')?></td><td class="financial_td"><?=getInDollars($resultsNumere['totalBrutNumere'])?></td> <td class="financial_td"><?=getInDollars($resultsNumere['totalTaxNumere'])?></td><td class="financial_td"><?=getInDollars($resultsNumere['totalPlataJucatoriNumere'])?></td><td class="financial_td"><?=getInDollars($resultsNumere['totalPlataJucatoriTaxaNumere'])?></td><td class="financial_td"><?=getInDollars($resultsNumere['totalNetNumere'])?></td></tr>
                                                    <tr><td><?=$lang->getLang('Other income/other players payments')?></td><td class="financial_td"></td> <td class="financial_td"></td><td class="financial_td"></td><td class="financial_td"></td><td class="financial_td"></td></tr>
                                                    <tr><td>Total</td>
                                                        <td class="financial_td"><?=(getInDollars($resultsBetting['totalBrutSport']+$results['totalBrutNonSport']+$resultsNumere['totalBrutNumere']))?></td>
                                                        <td class="financial_td"><?=(getInDollars($resultsBetting['totalTaxSport']+$results['totalTaxNonSport']+$resultsNumere['totalTaxNumere']))?></td>
                                                        <td class="financial_td"><?=(getInDollars($resultsBetting['totalPlataJucatoriSport']+$results['totalPlataJucatoriNonSport']+$resultsNumere['totalPlataJucatoriNumere']))?></td>
                                                        <td class="financial_td"><?=(getInDollars($resultsBetting['totalPlataJucatoriTaxaSport']+$results['totalPlataJucatoriTaxaNonSport']+$resultsNumere['totalPlataJucatoriTaxaNumere']))?></td>
                                                        <td class="financial_td"><?=(getInDollars($resultsBetting['totalNetSport']+$results['totalNetNonSport']+$resultsNumere['totalNetNumere']))?></td>
                                                    </tr>
                                                </table>
                                                <br />
                                                <h5 style="text-transform: uppercase"><?=$lang->getLang('Calculation of percentage')?></h5>
                                                <table>
                                                    <tr><td style="text-transform: uppercase"><?=$lang->getLang('Supplier')?></td><td><?=$lang->getLang('Gross Net')?></td></tr>
                                                    <tr><td> <?=$lang->getLang('sport')?> <?=($type!='c'?100-$results['sportPercent'] ."%":'')?></td><td class="financial_td">
                                                            <?if($type=='c') {
                                                                $bonus=($resultsBetting['totalNetSport']/10-$detailedResults['total']<0? 0 : $resultsBetting['totalNetSport']/10-$detailedResults['total']);
                                                                /*$totalSportParticipant = $detailedResults['total']+$bonus-$resultsBetting['totalPlataJucatoriTaxaSport'];*/
                                                                $totalSportParticipant = $detailedResults['total']+$bonus;
                                                                if($lostP==0 && $totalSportParticipant<0 ){
                                                                    /*$totalSportParticipant=0-$resultsBetting['totalPlataJucatoriTaxaSport'];*/
                                                                    $totalSportParticipant=0;
                                                                }
                                                                $totalSportTitular = $resultsBetting['totalNetSport']-$totalSportParticipant;


                                                            }
                                                            else{
                                                                /*$totalSportParticipant =$resultsBetting['totalNetSport']/100*$resultsBetting['sportPercent']-$resultsBetting['totalPlataJucatoriTaxaSport'];*/
                                                                $totalSportParticipant =$resultsBetting['totalNetSport']/100*$resultsBetting['sportPercent'];
                                                                if($lostP==0 && $resultsBetting['totalNetSport'] <0 ){
                                                                    /*$totalSportParticipant=0-$resultsBetting['totalPlataJucatoriTaxaSport'];*/
                                                                    $totalSportParticipant=0;
                                                                }
                                                                $totalSportTitular =$resultsBetting['totalNetSport']-$totalSportParticipant;
                                                            }
                                                            if($results['totalNetNonSport']<0 && $lostP==0){
                                                                $results['taxPercent']=0;
                                                            }

                                                            $totalNumereParticipant=$resultsNumere['totalNetNumere']/100*$resultsNumere['taxPercentNumere'];
                                                            if($lostP==0 && $totalNumereParticipant<0 ){
                                                                /*$totalSportParticipant=0-$resultsBetting['totalPlataJucatoriTaxaSport'];*/
                                                                $totalNumereParticipant=0;
                                                            }
                                                            $totalNumereTitular = $resultsNumere['totalNetNumere']-$totalNumereParticipant;
                                                            ?>
                                                            <?=getInDollars($totalSportTitular)?>
                                                        </td></tr>

                                                    <tr><td> (nonsport) <?=$lang->getLang('virtual')?> <?=100-$results['taxPercent']?>%</td><td class="financial_td"><?=getInDollars($results['totalNetNonSport']/100*(100-$results['taxPercent']) )?></td></tr>
                                                    <tr><td> <?=$lang->getLang('numbers')?> <?=100-$resultsNumere['taxPercentNumere']?>%</td><td class="financial_td"><?=getInDollars($totalNumereTitular )?></td></tr>
                                                    <?if($manageTax=='0') {
                                                        $tax5=$resultsBetting['totalTaxSport']+$results['totalTaxNonSport']+$resultsNumere['totalTaxNumere'];
                                                        ?>
                                                        <tr><td> Tax 5% </td><td class="financial_td"><?=getInDollars($resultsBetting['totalTaxSport']+$results['totalTaxNonSport']+$resultsNumere['totalTaxNumere'])?></td></tr>
                                                    <? }
                                                    else{
                                                        $tax5=0;
                                                    }

                                                    ?>
                                                    <tr><td> <?=$lang->getLang('Win paid tax')?> </td><td class="financial_td"><?=getInDollars($resultsBetting['totalPlataJucatoriTaxaSport']+$results['totalPlataJucatoriTaxaNonSport']+$resultsNumere['totalPlataJucatoriTaxaNumere'])?></td></tr>
                                                    <tr><td>Total <span style="text-transform: uppercase"><?=$lang->getLang('Supplier')?></span></td><td class="financial_td"><?=getInDollars($totalSportTitular+ $results['totalNetNonSport']/100*(100-$results['taxPercent'])+$results['totalPlataJucatoriTaxaNonSport']+$totalNumereTitular+$resultsNumere['totalPlataJucatoriTaxaNumere'] +$resultsBetting['totalPlataJucatoriTaxaSport']  +  $tax5 )?></td></tr>

                                                    <tr><td colspan="2"><br /><br /></td></tr>


                                                    <!--//* PARTICIPANT *//-->
                                                    <tr><td style="text-transform: uppercase"><?=$lang->getLang('Associate participant')?></td><td><?=$lang->getLang('Gross Net')?></td></tr>
                                                    <tr><td> <?=$lang->getLang('sport')?> <?=($type!='c'?$results['sportPercent'] ."%":'')?></td><td class="financial_td"><?=getInDollars($totalSportParticipant)?></td></tr>
                                                    <tr><td> (nonsport) <?=$lang->getLang('virtual')?> <?=$results['taxPercent']?>%</td><td class="financial_td"><?=getInDollars($results['totalNetNonSport']/100*$results['taxPercent'])?></td></tr>
                                                    <tr><td><?=$lang->getLang('numbers') ?>  <?=$resultsNumere['taxPercentNumere']?>%</td><td class="financial_td"><?=getInDollars($totalNumereParticipant)?></td></tr>
                                                    <tr><td>Total <span style="text-transform: uppercase"><?=$lang->getLang('ASSOCIATE PARTICIPANT')?></span></td><td class="financial_td"><?=getInDollars($totalSportParticipant + ($results['totalNetNonSport']/100*$results['taxPercent']) + $totalNumereParticipant)?></td></tr>
                                                    <tr><td colspan="2"><br /><br /></td></tr>
                                                    <!--// PARTICIPANT  //-->


                                                    <?if ($type!='a'){ ?>
                                                        <tr><td class="centered" colspan="2"> <?=$lang->getLang('Virtual - If has profit')?><br /><?=$lang->getLang('Associat participant')?>-35%</td></tr>
                                                        <tr><td><?=$lang->getLang('Between % % - % % - % of profit', 0,$results['cur_code_for_web'],5000,$results['cur_code_for_web'],'40%')?></td><td class="financial_td"><?=($results['taxPercent']==40?getInDollars($results['totalNetNonSport']/100*$results['taxPercent']):0)?></td></tr>
                                                        <tr><td><?=$lang->getLang('Between % % - % % - % of profit', 5000,$results['cur_code_for_web'],10000,$results['cur_code_for_web'],'45%')?></td><td class="financial_td"><?=($results['taxPercent']==45 ? getInDollars($results['totalNetNonSport']/100*$results['taxPercent']):0)?></td></tr>
                                                        <tr><td> <?=$lang->getLang('Over % % - %  of profit', 10000,$results['cur_code_for_web'],'50%')?></td><td class="financial_td"><?=($results['taxPercent']==50? getInDollars($results['totalNetNonSport']/100*$results['taxPercent']):0)?></td></tr>
                                                        <tr><td>Total</td><td class="financial_td"><?=getInDollars($results['totalNetNonSport']/100*$results['taxPercent'])?></td></tr>
                                                    <? } ?>
                                                </table>
                                                <br />

                                                <?if($type=='c') {?>
                                                    <table>
                                                        <? foreach ($detailedResults as $k=>$v){

                                                            if($k!=='total'){
                                                                ?>
                                                                <tr><td colspan="4"><?=$lang->getLang('Week')?> <?=$v['start']." - ".$v['end']?></td></tr>
                                                                <tr><td colspan="2"><span class="bold"><?=$lang->getLang('Events played on each ticket')?></span></td><td><?=$lang->getLang('Amount')?></td><td><?=$lang->getLang('commission')?></td></tr>
                                                                <tr><td><?=$lang->getLang('Column has 1 event(single)')?></td><td><?=$lang->getLang('commission')?> 1%</td><td><?=getInDollars($v['1_comm'])?></td><td><?=getInDollars($v['1_comm']/100)?></td></tr>
                                                                <tr><td><?=$lang->getLang('Column has 2 events(double)')?></td><td><?=$lang->getLang('commission')?> 3%</td><td><?=getInDollars($v['3_comm'])?></td><td><?=getInDollars($v['3_comm']/100*3) ?></td></tr>
                                                                <tr><td><?=$lang->getLang('Column has % events', '3,4,5')?></td><td><?=$lang->getLang('commission')?> 5%</td><td><?=getInDollars($v['5_comm'])?></td><td><?=getInDollars($v['5_comm']/100*5)?></td></tr>
                                                                <tr><td><?=$lang->getLang('Column has % events', '6,7,8')?></td><td><?=$lang->getLang('commission')?> 7%</td><td><?=getInDollars($v['7_comm'])?></td><td><?=getInDollars($v['7_comm']/100*7)?></td></tr>
                                                                <tr><td><?=$lang->getLang('Column has % events', '9,10,11')?></td><td><?=$lang->getLang('commission')?> 10%</td><td><?=getInDollars($v['10_comm'])?></td><td><?=getInDollars($v['10_comm']/100*10)?></td></tr>
                                                                <tr><td><?=$lang->getLang('Column has % or more  events',12)?></td><td><?=$lang->getLang('commission')?> 13%</td><td><?=getInDollars($v['13_comm'])?></td><td><?=getInDollars($v['13_comm']/100*13)?></td></tr>
                                                                <tr><td colspan="2"><span class="centered">Total</span></td><td><?=getInDollars($v['total'])?></td><td><?=getInDollars($v['totalComm'])?></td></tr>
                                                                <tr><td colspan="4"><br /><br /></td></tr>
                                                            <?} }?>
                                                        <tr><td><?=$lang->getLang('Monthly % bonus taken from commission','10%')?></td><td colspan="3"><?=getInDollars($bonus)?></td> </tr>
                                                    </table>
                                                <? }?>
                                                <?if($catre==0){
                                                    ?>
                                                    <h5 style="text-transform: uppercase">total <?=$lang->getLang('Supplier')?> :  <?=getInDollars($totalSportTitular+ $results['totalNetNonSport']/100*(100-$results['taxPercent'])+$results['totalPlataJucatoriTaxaNonSport']   +$totalNumereTitular+$resultsNumere['totalPlataJucatoriTaxaNumere']   +$resultsBetting['totalPlataJucatoriTaxaSport']  +  $tax5)?></h5>
                                                    <?
                                                }
                                                else{ ?>
                                                    <h5 style="text-transform: uppercase">total <?=$lang->getLang('Associate participant')?> :  <?=getInDollars($totalSportParticipant + ($results['totalNetNonSport']/100*$results['taxPercent']) +$totalNumereParticipant)?></h5>
                                                <?}
                                                ?>
                                            </div>
                                            <?
                                        }?>

                                    </div>
                                <? } if($virtualCommissionsWeekly) { ?>
                                    <div class="tab-pane" id="weekly">
                                        <p>
                                        <div class='row'>
                                            <div class="span4 fleft"><div class='label label-inverse'><?=$lang->getLang("Day")?></div>
                                                <br>
                                                <div class="controls">
                                                    <div class="input-prepend">
                                                        <span class="add-on"><i class="icon-calendar"></i></span>
                                                        <input type='text' name='date_start_closure' id='date_start_closure' class="dateStartHelper"  value="<?=date('Y-m-d')?>" >
                                                    </div>
                                                </div>
                                            </div><br />
                                        </div>
                                        <div class="row">
                                            <span class="span2"> <button class="btn btn-primary" id="SearchClosure" data-loading-text="<?=$lang->getLang("Please wait")?>..."><?=$lang->getLang("Search")?></button></span>
                                        </div><br /><br />
                                        <div id="closureResult">
                                        </div>

                                        </p>
                                    </div>
                                <? } ?>
                            </div>
                        </div>
                </td>
            </tr>
        </table>
    </div>
<?

function calculateAffiliateCommissions($club,$date,$sportPercent=false,$virtualPercent=false){
    global $dbh,$brutOrNet;
    $dates=getBettingInterval($date,'monthly');
    $date_start=$dates['start'];
    $date_end=$dates['end'];
    $sql="SELECT cur_code_for_web, DATE_FORMAT(vds_date, '%Y-%m') month_, jur_name agency_name, sum(vds_bet) venit_brut, sum(vds_bet_tax) taxa_5,
          sum(VDS_TICKET_PAID_AMOUNT) - sum(VDS_TICKET_PAID_AMOUNT_TAX) plati_jucatori , sum(VDS_TICKET_PAID_AMOUNT_TAX) plati_jucatori_taxa, sum(vds_bet) - sum(vds_bet_tax) - sum(VDS_TICKET_PAID_AMOUNT)  venit_net
   /*     (case vds_gam_id
          when 215 THEN 'sport'
          else 'nonsport'
          end
        ) gam_type*/
        FROM casinogames.virtual_daily_summary, jurisdiction,currency
        where vds_club_id = jur_id
        AND  (vds_gam_id <>215 AND vds_gam_id <> 9026)
        and jur_currency=cur_id
        and jur_id = ".$dbh->escape($club)."
        and vds_date BETWEEN '".$dbh->escape($date_start)."' AND '".$dbh->escape($date_end)."'
        group by DATE_FORMAT(vds_date, '%Y-%m'), jur_id";
    $result= $dbh->exec($sql);
    $results=array();
    /*    $results['totalBrutSport']=0;
        $results['totalTaxSport']=0;
        $results['totalPlataJucatoriSport']=0;
        $results['totalPlataJucatoriTaxaSport']=0;
        $results['totalNetSport']=0;*/

    $results['totalBrutNonSport']=0;
    $results['totalTaxNonSport']=0;
    $results['totalPlataJucatoriNonSport']=0;
    $results['totalPlataJucatoriTaxaNonSport']=0;
    $results['totalNetNonSport']=0;


    while($result->hasNext()){
        $row=$result->next();
        $results['cur_code_for_web']=$row['cur_code_for_web'];

        /*     if($row['gam_type']=='sport'){
                 $results['totalBrutSport']+=$row['venit_brut'];
                 $results['totalTaxSport']+=$row['taxa_5'];
                 $results['totalPlataJucatoriSport']+=$row['plati_jucatori'];
                 $results['totalPlataJucatoriTaxaSport']+=$row['plati_jucatori_taxa'];
                 $results['totalNetSport']+=$row['venit_net'];
             }
             else{*/
        $results['totalBrutNonSport']+=$row['venit_brut'];
        $results['totalTaxNonSport']+=$row['taxa_5'];
        $results['totalPlataJucatoriNonSport']+=$row['plati_jucatori'];
        $results['totalPlataJucatoriTaxaNonSport']+=$row['plati_jucatori_taxa'];
        $results['totalNetNonSport']+=$row['venit_net'];
    }
    /*}
    if(!$sportPercent) {
        $results['sportPercent']=35;
    }
    else{
        $results['sportPercent']=$sportPercent;
    } */

    if(!$virtualPercent) {
        if($brutOrNet==1) {
            $totalToCompare =$results['totalNetNonSport'];
        }
        else{
            $totalToCompare=$results['totalBrutNonSport']- $results['totalTaxNonSport'];
        }

        if ( $totalToCompare<= 5000) {
            $results['taxPercent'] = 40;
        } elseif ($totalToCompare > 5000 && $totalToCompare <= 10000) {
            $results['taxPercent'] = 45;
        } elseif ($totalToCompare > 10000) {
            $results['taxPercent'] = 50;
        }
    }else{
        $results['taxPercent']=$virtualPercent;
    }
    return $results;
}
function calculateAffiliateCommissionsBetting($club,$date,$sportPercent=false){
    global $dbh;
    $dates=getBettingInterval($date,'monthly');

    $date_start=$dates['start'];
    $date_end=$dates['end'];
    $sql="SELECT cur_code_for_web, DATE_FORMAT(vds_date, '%Y-%m') month_, jur_name agency_name, sum(vds_bet) venit_brut, sum(vds_bet_tax) taxa_5,
          sum(VDS_TICKET_PAID_AMOUNT) - sum(VDS_TICKET_PAID_AMOUNT_TAX) plati_jucatori , sum(VDS_TICKET_PAID_AMOUNT_TAX) plati_jucatori_taxa, sum(vds_bet) - sum(vds_bet_tax) - sum(VDS_TICKET_PAID_AMOUNT)  venit_net

        FROM casinogames.virtual_daily_summary, jurisdiction,currency
        where vds_club_id = jur_id
        AND vds_gam_id=215
        and jur_currency=cur_id
        and jur_id = ".$dbh->escape($club)."
        and vds_date BETWEEN '".$dbh->escape($date_start)."' AND '".$dbh->escape($date_end)."'
        group by DATE_FORMAT(vds_date, '%Y-%m'), jur_id";
    $result= $dbh->exec($sql);
    $results=array();
    $results['totalBrutSport']=0;
    $results['totalTaxSport']=0;
    $results['totalPlataJucatoriSport']=0;
    $results['totalPlataJucatoriTaxaSport']=0;
    $results['totalNetSport']=0;


    while($result->hasNext()){
        $row=$result->next();
        $results['cur_code_for_web']=$row['cur_code_for_web'];

        $results['totalBrutSport']+=$row['venit_brut'];
        $results['totalTaxSport']+=$row['taxa_5'];
        $results['totalPlataJucatoriSport']+=$row['plati_jucatori'];
        $results['totalPlataJucatoriTaxaSport']+=$row['plati_jucatori_taxa'];
        $results['totalNetSport']+=$row['venit_net'];

    }
    if(!$sportPercent) {
        $results['sportPercent']=35;
    }
    else{
        $results['sportPercent']=$sportPercent;
    }

    return $results;
}

function calculateAffiliateCommissionsNumbers($club,$date,$numerePercent=false){
    global $dbh,$brutOrNet;
    $dates=getBettingInterval($date,'monthly');

    $date_start=$dates['start'];
    $date_end=$dates['end'];
    $sql="SELECT cur_code_for_web, DATE_FORMAT(vds_date, '%Y-%m') month_, jur_name agency_name, sum(vds_bet) venit_brut, sum(vds_bet_tax) taxa_5,
          sum(VDS_TICKET_PAID_AMOUNT) - sum(VDS_TICKET_PAID_AMOUNT_TAX) plati_jucatori , sum(VDS_TICKET_PAID_AMOUNT_TAX) plati_jucatori_taxa, sum(vds_bet) - sum(vds_bet_tax) - sum(VDS_TICKET_PAID_AMOUNT)  venit_net
        FROM casinogames.virtual_daily_summary, jurisdiction,currency
        where vds_club_id = jur_id
        AND vds_gam_id=9026
        and jur_currency=cur_id
        and jur_id = ".$dbh->escape($club)."
        and vds_date BETWEEN '".$dbh->escape($date_start)."' AND '".$dbh->escape($date_end)."'
        group by DATE_FORMAT(vds_date, '%Y-%m'), jur_id";
    $result= $dbh->exec($sql);
    $results=array();
    $results['totalBrutNumere']=0;
    $results['totalTaxNumere']=0;
    $results['totalPlataJucatoriNumere']=0;
    $results['totalPlataJucatoriTaxaNumere']=0;
    $results['totalNetNumere']=0;

    while($result->hasNext()){
        $row=$result->next();
        $results['cur_code_for_web']=$row['cur_code_for_web'];
        $results['totalBrutNumere']+=$row['venit_brut'];
        $results['totalTaxNumere']+=$row['taxa_5'];
        $results['totalPlataJucatoriNumere']+=$row['plati_jucatori'];
        $results['totalPlataJucatoriTaxaNumere']+=$row['plati_jucatori_taxa'];
        $results['totalNetNumere']+=$row['venit_net'];
    }
    if(!$numerePercent) {
        if($brutOrNet==1) {
            $totalToCompare =$results['totalNetNumere'];
        }
        else{
            $totalToCompare=$results['totalBrutNumere']- $results['totalTaxNumere'];
        }

        if ( $totalToCompare<= 5000) {
            $results['taxPercentNumere'] = 40;
        } elseif ($totalToCompare > 5000 && $totalToCompare <= 10000) {
            $results['taxPercentNumere'] = 45;
        } elseif ($totalToCompare > 10000) {
            $results['taxPercentNumere'] = 50;
        }
    }else{
        $results['taxPercentNumere']=$numerePercent;
    }

    return $results;
}


function calculateAffiliateCommissionsDetailed($club,$date){
    global $dbh;
    list($user_betting, $pass_betting, $host_betting, $db_betting,$db_port) = explode(":",$_SERVER["BETTING_DSN"]);
    Db::setConnectionInfo($db_betting,$user_betting,$pass_betting,"mysql",$host_betting,$db_port);
    $results=array();
    $results['total']=0;
    $interval=(getBettingInterval($date));
    foreach($interval as $k=>$v){
        $section=array();
        $section['start']=$v['start'];
        $section['end']=$v['end'];
        $section['total']=0;
        $section['totalComm']=0;

        $section['1_comm']=0;
        $section['1_tax']=0;

        $section['3_comm']=0;
        $section['3_tax']=0;

        $section['5_comm']=0;
        $section['5_tax']=0;

        $section['7_comm']=0;
        $section['7_tax']=0;

        $section['10_comm']=0;
        $section['10_tax']=0;

        $section['13_comm']=0;
        $section['13_tax']=0;

        $section['TVincita']=0;

        $date_start=$v['start'];
        $date_end=$v['end'];


        $sql="select inte.*, COALESCE(sum(TVincita), 0) TVincita from (
            SELECT t.IDUtente userid, t.TStato stato, j.jur_id AS clubId, j.jur_name club_name,
            sum(IF(numeroBets = 1,importoPuntata * numeroSistemiComplementari,0)) AS 1_comm,
            sum(IF(numeroBets = 1,s.Tax,0)) AS 1_tax,
            sum(IF(numeroBets = 2,importoPuntata * numeroSistemiComplementari,0)) AS 3_comm,
            sum(IF(numeroBets = 2,s.Tax,0)) AS 3_tax,
            sum(IF(numeroBets BETWEEN 3 AND 5,importoPuntata * numeroSistemiComplementari,0)) AS 5_comm,
            sum(IF(numeroBets BETWEEN 3 AND 5,s.Tax,0)) AS 5_tax,
            sum(IF(numeroBets BETWEEN 6 AND 8,importoPuntata * numeroSistemiComplementari,0)) AS 7_comm,
            sum(IF(numeroBets BETWEEN 6 AND 8,s.Tax,0)) AS 7_tax,
            sum(IF(numeroBets BETWEEN 9 AND 11,importoPuntata * numeroSistemiComplementari,0)) AS 10_comm,
            sum(IF(numeroBets BETWEEN 9 AND 11,s.Tax,0)) AS 10_tax,
            sum(IF(numeroBets > 11,importoPuntata * numeroSistemiComplementari,0)) AS 13_comm,
            sum(IF(numeroBets > 11,s.Tax,0)) AS 13_tax
            FROM Ticket_dev t JOIN Sistemi s ON t.IDTicket = s.IDTicket
            JOIN punter u ON t.IDUtente = u.pun_id
            JOIN jurisdiction j ON u.pun_betting_club = j.jur_id
            JOIN jurisdiction jd ON j.jur_parent_id = jd.jur_id
            JOIN jurisdiction jr ON jd.jur_parent_id = jr.jur_id
            JOIN jurisdiction jn ON jr.jur_parent_id = jn.jur_id
            WHERE t.TStato IN (2, 0)
            AND t.TStamp BETWEEN '".$dbh->escape($date_start)." 00:00:00' AND '".$dbh->escape($date_end)." 23:59:59'
            and j.jur_id = ".$dbh->escape($club)."
        group by clubId
        ) inte LEFT JOIN Ticket_dev t2 ON t2.IDUtente = inte.userid
            AND TStampAccredito BETWEEN '".$dbh->escape($date_start)." 00:00:00' AND '".$dbh->escape($date_end)." 23:59:59'
            AND Tstato = 1
        GROUP BY clubId";
        $result = Db::getResult($sql);
        foreach ($result as $rs=>$rv){
            $comm1=$rv['1_comm']+$rv['1_tax'];
            $section['1_comm']+=$comm1;

            $comm3=$rv['3_comm']+$rv['3_tax'];
            $section['3_comm']+=$comm3;

            $comm5=$rv['5_comm']+$rv['5_tax'];
            $section['5_comm']+=$comm5;

            $comm7=$rv['7_comm']+$rv['7_tax'];
            $section['7_comm']+=$comm7;

            $comm10=$rv['10_comm']+$rv['10_tax'];
            $section['10_comm']+=$comm10;

            $comm13=$rv['13_comm']+$rv['13_tax'];
            $section['13_comm']+=$comm13;

            $section['TVincita']+=$rv['TVincita'];

            $section['total']+=($comm1 +$comm3+$comm5+$comm7+$comm10+$comm13);
            $section['totalComm']=($comm1/100+($comm3/100*3)+($comm5/100*5)+($comm7/100*7)+($comm10/10)+($comm13/100/13));
            $results['total']+=$section['totalComm'];
        }
        array_push($results,$section);

    }
    return $results;
}


