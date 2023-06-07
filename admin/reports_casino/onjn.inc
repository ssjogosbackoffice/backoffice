<?check_access('financial_onjn',true);
/**
 * @author Marian
 */
include_once 'JurisdictionsList.class.inc';
$jurClass = JurisdictionsList::getInstance ( $dbh );
$hierarchy = $jurClass->getJurisdictionTree($_SESSION['jurisdiction_id']);
$status = (isset($_POST["status"])) ? $_POST["status"] : 0 ;
?>
    <link rel="stylesheet" type="text/css" href="/media/jquery.multiselect.css" title="core_style">
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/js/dataTables/extensions/Buttons/css/buttons.dataTables.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/js/dataTables/css/jquery.dataTables.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
    <style>
        #companies{
            width: 270px !important;
        }
        .ui-multiselect-checkboxes label input {
            vertical-align: inherit;
            margin: 5px;
        }
        .ui-multiselect{
            max-width: none;
        }
        #searchTable{
            background-color: transparent !important;
            width:440px;
            margin: auto;
        }
        .dataTables_paginate {
            width:auto;
        }

    </style>
    <script src="/media/jquery.multiselect.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
    <script src="/media/jquery.alerts.js" type="text/javascript"></script>
    <script src="/media/jstree/jstree.js" type="text/javascript"></script>

    <script src="/media/js/dataTables/js/jquery.dataTables.js" type="text/javascript"></script>
    <script src="/media/js/dataTables/extensions/Buttons/js/dataTables.buttons.js" type="text/javascript"></script>
    <script src="/media/js/dataTables/extensions/Buttons/js/jszip.min.js" type="text/javascript"></script>
    <script src="/media/js/dataTables/extensions/Buttons/js/pdfmake.min.js" type="text/javascript"></script>
    <script src="/media/js/dataTables/extensions/Buttons/js/vfs_fonts.js" type="text/javascript"></script>
    <script src="/media/js/dataTables/extensions/Buttons/js/buttons.html5.min.js" type="text/javascript"></script>
    <script src="/media/js/dataTables/extensions/Buttons/js/buttons.flash.min.js" type="text/javascript"></script>
    <script src="/media/js/dataTables/extensions/Buttons/js/buttons.print.min.js" type="text/javascript"></script>


    <div class="container" style="position: absolute;left:0;right: 0;width: 100%" >
    <?php
    require_once "financial_casino/financial_functions.inc";
    $date_start = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d", time() - (60 * 60 * 24));
    $date_end = (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d");
    $defaultmonth= (isset($_POST['monthHelper'])) ? $_POST['monthHelper'] : false;
    $defaultweek= (isset($_POST['weekHelper'])) ? $_POST['weekHelper'] : false;
    $companies=(isset($_POST['companies']) ? $_POST['companies'] : false);
    $dstr=(($_POST['dstr']!='') ? explode(',',$_POST['dstr']) : array());

    ?>
    <script type="text/javascript">
        dstr=[<?=$_POST['dstr']?>];

        $(function() {
            var to = false;
            $("#betResultsTable").DataTable( {
                dom: 'Bfrtip',
                lengthMenu: [
                    [ 10, 50, 100, -1 ],
                    [ '10 results', '50 results', '100 results', 'Show all' ]
                ],

                buttons: [
                    'pageLength',
                    {
                        extend: 'excelHtml5',
                        title: 'Report'
                    },
                    {
                        extend: 'pdfHtml5',
                        orientation: 'landscape',
                        pageSize: 'LEGAL',
                        title: 'Report'
                    }
                ]
            } );

            $('#jurisdictionTreeSearch').keyup(function () {
                $("#jurisdictionTree").jstree('open_all');
                if(to) { clearTimeout(to); }
                to = setTimeout(function () {
                    var v = $('#jurisdictionTreeSearch').val();
                    $('#jurisdictionTree').jstree(true).search(v);
                }, 250);
            });


            $("#jurisdictionTree")
                .jstree({
                    "plugins" : [
                        "search",
                        "types",
                        "state"
                    ],
                    "search":{
                        "fuzzy":false,
                        "show_only_matches":true
                    },
                    "types" : {
                        "casino" : {
                            "icon" : "../media/images/ca.png"
                        },
                        "district" : {
                            "icon" :  "../media/images/c.png"
                        },
                        "club" : {
                            "icon": "../media/images/a.png"
                        }
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
            var el=  $('#companies').multiselect({
                selectedList: 4,
                noneSelectedText:'<?=$lang->getLang("Select an agency")?>',
                click: function(event, ui){
                    dstr=jQuery.grep(dstr, function(value) {
                        return value != $('#companies option[value="'+ui.value+'"]').parent().attr('value');
                    });
                    $('#dstr').val(dstr);
                },
                uncheckAll: function(event, ui){
                    dstr=[];
                    $('#dstr').val('');
                },
                optgrouptoggle: function(event, ui){
                    if(ui.checked){
                        dstr.push(ui.grpvalue);
                    }
                    else{
                        dstr=jQuery.grep(dstr, function(value) {
                            return value != ui.grpvalue;
                        });
                    }
                    $('#dstr').val(dstr);
                }
            }).multiselectfilter();
            $('.ui-multiselect').addClass('btn');

            $('#status').on('change',function(){
                if($(this).val()!='0'){
                    statusToSearch=$(this).val();
                    if(statusToSearch=='1'){
                        $("#companies").find('option').each(function() {
                            if($(this).data('status')!='1'){
                                $(this).prop("selected", false).attr('disabled','disabled');
                            }
                            else{
                                $(this).attr('disabled',false);
                            }
                        });
                    }
                    else{
                        $("#companies").find('option').each(function() {
                            if($(this).data('status')=='1'){
                                $(this).prop("selected", false).attr('disabled','disabled');
                            }
                            else{
                                $(this).attr('disabled',false);
                            }
                        });
                    }

                }
                else{
                    $("#companies").find('option').each(function() {
                        $(this).attr('disabled', false);
                    });
                }
                el.multiselect('refresh').multiselect('checkAll');
            });

        });


        $(document).on('change', '#monthHelper', function() {
            var d = new Date($(this).val());
            createMonthHelper(d);
        });
        $(document).on('change', '#weekHelper', function() {
            createWeekHelper($(this).val());
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

    </script>
    <center>
        <table class="table table-bordered" style="margin: auto;width: auto">
            <tr>
                <td class="navbar-inner" colspan="2"><h3 class="text-center"> <?=$lang->getLang("ONJN Reports")?></h3></td>
            </tr>
            <tr>
                <td class="span4">
                    <div class="contentAdmin">
                        <div class="contentHeadAdmin"><?=$lang->getLang("Agency helper")?></div>
                        <div class="well">
                            <input type="text" id='jurisdictionTreeSearch' placeholder="<?=$lang->getLang("Search an agency")?>">
                            <div id="jurisdictionTree">
                                <ul>
                                    <li data-jstree='{ "type" : "casino" }' class="jstree-closed casino" ><?=$lang->getLang('COMPANIES')?>
                                        <ul>

                                            <?=showChild($hierarchy);?>
                                        </ul>
                                    </li>
                                </ul>
                            </div>
                        </div>
                    </div>
                </td>
                <td style='vertical-align: top'>
                    <div class="well">
                        <br/>
                        <p>
                        <form action="/onjn" method="post">
                            <?php printHiddenInput('doQuery', 'yes');?>
                            <?php printHiddenInput('search_type', 'date_range');?>
                            <input type="hidden" id="dstr" name="dstr"  value="<?=$_POST['dstr']?>" />
                            <table id="searchTable" class="table table-bordered">
                                <tbody>
                                <tr>
                                    <td><div class='label label-inverse'><?=$lang->getLang("Choose month")?></div></td>
                                    <td><div class="controls">
                                            <div class="input-prepend">
                                                <span class="add-on"><i class="icon-calendar"></i></span>
                                                <?=month_select_box($defaultmonth)?>
                                            </div>
                                        </div></td>
                                </tr>
                                <tr>
                                    <td><div class='label label-inverse'><?=$lang->getLang("Choose week")?></div></td>
                                    <td>
                                        <div class="controls">
                                            <div class="input-prepend">
                                                <span class="add-on"><i class="icon-calendar"></i></span>
                                                <?=week_select_box($defaultweek)?>
                                            </div>
                                        </div>
                                    </td>
                                </tr>
                                <tr>
                                    <td><div class='label label-inverse'><?=$lang->getLang("From")?></div></td>
                                    <td> <div class="controls">
                                            <div class="input-prepend">
                                                <span class="add-on"><i class="icon-calendar"></i></span>
                                                <input type='text' name='date_start' id='date_start'  value="<?=$date_start?>" >
                                            </div>
                                        </div></td>
                                </tr>
                                <tr>
                                    <td><div class='label label-inverse'><?=$lang->getLang("Until")?></div></td>
                                    <td><div class="controls">
                                            <div class="input-prepend">
                                                <span class="add-on"><i class="icon-calendar"></i></span>
                                                <input type='text' name='date_end' id='date_end' value="<?=$date_end?>">
                                            </div>
                                        </div></td>
                                </tr>
                                <tr>
                                    <td><div class='label label-inverse'><?=$lang->getLang("Agency")?></div></td>
                                    <td>
                                        <?

                                        $result=getAllClubsAndDistricts();
                                        $districts=array();

                                        while($result->hasNext()){
                                            $row=$result->next();
                                            if(!is_array($districts[$row['dis_jur_id']])){
                                                $districts[$row['dis_jur_id']]=array();
                                                $districts[$row['dis_jur_id']]['name']=$row['dis_jur_name'];
                                                $districts[$row['dis_jur_id']]['id']=$row['dis_jur_id'];
                                            }
                                            $districts[$row['dis_jur_id']]['clubs'][$row['cl_jur_id']]=array($row['cl_jur_name'],$row['cl_jur_status']) ;
                                        }
                                        ?>
                                        <select name="companies[]" id="companies" multiple="multiple" style="display:none">
                                            <?foreach($districts as $district=>$clubs){ ?>
                                            <?if($_SESSION['jurisdiction_class']!='club') { ?>
                                            <optgroup label="<?=$clubs['name']?>" value="<?=$district?>"> <? }
                                                foreach($clubs['clubs'] as $club=>$v){ ?>
                                                    <option value="<?=$club?>" data-status="<?=$v[1]?>" <?=(($status==2 &&$v[1]==1) || ($status==1 &&$v[1]!=1)) ? 'disabled': '' ?>    <?=(in_array($club,$companies)===false && in_array($clubs['id'],$dstr)===false)  ? '' : 'selected'?>><?=$v[0]?></option>
                                                    <?
                                                }
                                                } ?>
                                        </select>
                                    </td>
                                </tr>
                                <tr>
                                    <td><div class='label label-inverse'><?=$lang->getLang("Status")?></div></td>
                                    <td><div class="controls">
                                            <div class="input-prepend">
                                                <span class="add-on"><i class="icon-random"></i></span>
                                                <select name='status' id="status">
                                                    <option value='0' <?=($status=='0') ? "selected='selected'":""?> ><?=$lang->getLang("All")?></option>
                                                    <option value='1' <?=($status=='1') ? "selected='selected'":""?>><?=$lang->getLang('Allow')?></option>
                                                    <option value='2' <?=($status=='2') ? "selected='selected'":""?>><?=$lang->getLang("Deny")?></option>
                                                </select>
                                            </div>
                                        </div>
                                    </td>
                                </tr>
                                <tr><td colspan="2"><input type="submit" class="btn btn-primary" value="<?=$lang->getLang("Submit")?>" ></td> </tr>
                                </tbody>
                            </table>

                            <div class="clearfix"></div>
                    </div>
                    <div>
                        <?if(isset($_POST['doQuery'])) {
                            if($companies==''){
                                ?>
                                <h4 class="alert alert-error fade in"><button type='button' class='close fade in' data-dismiss='alert'>&times;</button><?=$lang->getLang('Please select at least one agency');?></h4>
                            <?
                            }else{
                                $nrclubs=$result->getNumRows();
                                $totalbet=0;
                                $totalWin=0;
                                $totalWinTax=0;
                                $totalTickets=0;
                                $totalTicketsErased=0;

                                $starttime = microtime(true);
                                $financial=getAgencyFianancialData($date_start,$date_end,$companies);
                                $endtime = microtime(true);
                                $duration = $endtime - $starttime;
                                $duration=number_format($duration, 4, ',', '')."s";
                                ?>
                                <div class="tip fleft"><?=$lang->getLang("Time taken to execute your request")?>:<?=$duration?></div><br /><br />
                                <h4 class=" fleft"><?=$lang->getLang("Organizer")?>:RG FAIRPLAY SRL</h4><br /><br />
                                <?
                                if($financial->getNumRows()>0){
                                    ?>

                                    <?while($financial->hasNext()){
                                        $row=$financial->next();
                                        $totalbet+=$row['tot_bet'];
                                        $totalWin+=$row['tot_win'];
                                        $totalWinTax+=$row['tot_win_tax'];
                                        $totalTickets+=$row['tickets_played'];
                                        $totalTicketsErased+=$row['tickets_erased'];
                                        $betResults.="<tr>
                                        <td class='unwrappable'>".$row['dis_jur_name']."</td>
                                        <td class='unwrappable'>".$row['cl_jur_name']."</td>
                                        <td class='unwrappable'>".$row['cl_jur_city']."</td>
                                        <td class='unwrappable'>".$row['cl_jur_address']."</td>
                                        <td class='unwrappable'>".$row['cl_jur_postcode_zip']."</td>
                                        <td class='unwrappable'>";
                                        if($row['cl_jur_status']==1){
                                            $betResults .="<span class='result'>".$lang->getLang('Active')."</span>";
                                        }else{
                                            $betResults .="<span class='error'>".$lang->getLang('Inactive')."</span>";
                                        }
                                        $betResults.="</td>
                                        <td class='unwrappable'>".getInDollars($row['tot_bet'],2,$row['cur_code_for_web'])."</td>
                                        <td class='unwrappable'>".getInDollars($row['tot_win']-$row['tot_win_tax'],2,$row['cur_code_for_web'])."</td>
                                        <td class='unwrappable'>".$row['tickets_played']."</td>
                                        <td class='unwrappable'>".$row['tickets_erased']."</td>
                                        <td class='unwrappable'>".getInDollars($row['tot_bet']-$row['tot_win'],2,$row['cur_code_for_web'])."</td>
                                        <td class='unwrappable'>".getInDollars(($row['tot_bet']-$row['tot_win'])/100*16,2,$row['cur_code_for_web'])."</td>
                                        </tr>";
                                    }

                                    ?>
                                    <table class='display table table-striped table-bordered table-hover table-condensed'>
                                        <thead>
                                        <tr><th style="font-weight: bold"><?=$lang->getLang('Total receipts')?></th>
                                            <th style="font-weight: bold"><?=$lang->getLang('Total awards')?></th>
                                            <th style="font-weight: bold"><?=$lang->getLang('Total tickets issued')?></th>
                                            <th style="font-weight: bold"><?=$lang->getLang('Total tickets canceled')?></th>
                                            <th style="font-weight: bold"><?=$lang->getLang('Revenue')?></th>
                                            <th style="font-weight: bold"><?=$lang->getLang('Authorization fee')?></th>
                                        </tr>
                                        </thead>
                                        <tbody>
                                        <tr>
                                        <td><?=getInDollars($totalbet);?></td>
                                        <td><?=getInDollars($totalWin);?></td>
                                        <td><?=$totalTickets;?></td>
                                        <td><?=$totalTicketsErased;?></td>
                                        <td><?=getInDollars($totalbet-$totalWin)?></td>
                                         <td><?=getInDollars(($totalbet-$totalWin)/100*16)?></td>
                                        </tr>
                                        </tbody>
                                    </table><br /><br />
                                    <table class="table table-bordered table-striped display" id="betResultsTable">
                                        <thead>
                                        <tr>
                                            <th class='unwrappable'><?=$lang->getLang('Company')?></th>
                                            <th class='unwrappable'><?=$lang->getLang('Agency')?></th>
                                            <th class='unwrappable'><?=$lang->getLang('City')?></th>
                                            <th class='unwrappable'><?=$lang->getLang('Address')?></th>
                                            <th class='unwrappable'><?=$lang->getLang('Zip')?></th>
                                            <th class='unwrappable'><?=$lang->getLang('Status')?></th>
                                            <th class='unwrappable'><?=$lang->getLang('Total receipts')?></th>
                                            <th class='unwrappable'><?=$lang->getLang('Total awards')?></th>
                                            <th class='unwrappable'><?=$lang->getLang('Total tickets issued')?></th>
                                            <th class='unwrappable'><?=$lang->getLang('Total tickets canceled')?></th>
                                            <th class='unwrappable'><?=$lang->getLang('Revenue')?></th>
                                            <th class='unwrappable'><?=$lang->getLang('Authorization fee')?></th>
                                        </tr></thead>
                                        <tbody>
                                        <?=$betResults?>
                                        </tbody></table> <br /><br />
                                    <table class='display table table-striped table-bordered table-hover table-condensed'>
                                        <thead>
                                        <tr><th style="font-weight: bold"><?=$lang->getLang('Total receipts')?></th>
                                            <th style="font-weight: bold"><?=$lang->getLang('Total awards')?></th>
                                            <th style="font-weight: bold"><?=$lang->getLang('Total tickets issued')?></th>
                                            <th style="font-weight: bold"><?=$lang->getLang('Total tickets canceled')?></th>
                                            <th style="font-weight: bold"><?=$lang->getLang('Revenue')?></th>
                                            <th style="font-weight: bold"><?=$lang->getLang('Authorization fee')?></th>
                                        </tr>
                                        </thead>
                                        <tbody>
                                        <tr>
                                            <td><?=getInDollars($totalbet);?></td>
                                            <td><?=getInDollars($totalWin);?></td>
                                            <td><?=$totalTickets;?></td>
                                            <td><?=$totalTicketsErased;?></td>
                                            <td><?=getInDollars($totalbet-$totalWin)?></td>
                                            <td><?=getInDollars(($totalbet-$totalWin)/100*16)?></td>
                                        </tr>
                                        </tbody>
                                    </table>
                                <? }
                                else
                                {
                                    ?>
                                    <h4 class="alert alert-error fade in"><button type='button' class='close fade in' data-dismiss='alert'>&times;</button><?=$lang->getLang('No result found');?></h4>
                                <?}
                            }
                        }
                        ?>
                    </div>
                </td>
            </tr>
        </table>
    </center>
    </div>

<?



function getAgencyFianancialData($date_start,$date_end,$companies){
    global $dbh,$nrclubs,$districts,$dstr;
    $today=date('Y-m-d');
    $totcompanies=count($companies);
    $allclubs=implode(",",$companies);
    if($nrclubs!=$totcompanies){
        foreach ($companies as $k=>$v){
            foreach($dstr as $k2=>$v2){
                if(array_key_exists($v,$districts[$v2]['clubs'])){
                    unset($companies[$k]);
                }
            }
        }
    }
    $companies=implode(",",$companies);

    if(strtotime($date_start)<strtotime($today)){
        if(strtotime($date_end)==strtotime($today)){
//            $sql=$sql1;
	      $sql=getVirtualFinancialInfoOldAndCurrent($date_start,$date_end,$companies,$totcompanies,$allclubs);
        }
        else{
//            $sql=$sql2;
              $sql=getVirtualFinancialInfoOld($date_start,$date_end,$companies,$totcompanies,$allclubs);
        }
    }
    else{
//        $sql=$sql3;
	  $sql=getVirtualFinancialInfoCurrent($date_start,$date_end,$companies,$totcompanies,$allclubs);
    }
    return $dbh->exec($sql,false,true);

}


function showChild($childArray){
    foreach($childArray as $key => $value){

        if($value['class'] == 'casino'){
            $class_type = "casino";
        }elseif($value['class'] == 'nation'){
            $class_type = "nation";
        }elseif($value['class'] == 'region'){
            $class_type = "region";
        }elseif($value['class'] == 'district'){
            $class_type = "district";
        }elseif($value['class'] == 'club'){
            $class_type = "club";
        }
        if(!is_null($value['child'])){
            if($class_type=='club'||$class_type=='district'){
                ?>

                <li data-jstree='{ "type" : "<?=$class_type?>" }' class="jstree-closed <?=$class_type?>" id="<?=$value['id']?>"><?=($_SESSION['jurisdiction_class']=='district'? getClubName($_SESSION['jurisdiction_id']) : strtoupper($value['name']))?>
                <ul> <? } ?> <?php showChild($value['child']);
            ?>
            <?if($class_type=='club'||$class_type=='district'){ ?>
                </ul>
                </li>
            <? } ?>
        <?php
        }else{
            if($class_type=='club'||$class_type=='district'){ ?>

                <li data-jstree='{ "type" : "<?=$class_type?>" }' class="<?=$class_type?>" id="<?=$value['id']?>"><?=strtoupper($value['name'])?></li><?php }
        }
    }
}

/**
 * Generate the query used to extract the virtual financial info old and for the current day
 *
 */
function getVirtualFinancialInfoOldAndCurrent($date_start,$date_end,$companies,$totcompanies,$allclubs){
    global $dbh,$nrclubs,$districts,$dstr;
    $today=date('Y-m-d');
    $yesterday=date('Y-m-d',time() - 60 * 60 * 24);
    $sql1="SELECT dis_jur_id, dis_jur_name, cl_jur_id, cl_jur_name, cl_jur_city, cl_jur_address, cl_jur_postcode_zip,
      cl_jur_status, sum(tot_bet) tot_bet, sum(tot_win) tot_win,sum(tot_win_tax) tot_win_tax,sum(tickets_played) tickets_played, sum(tickets_erased) tickets_erased, cur_code_for_web
     FROM (
    SELECT d.jur_id dis_jur_id, d.jur_name dis_jur_name, c.jur_id cl_jur_id, c.jur_name cl_jur_name, c.jur_city cl_jur_city, c.jur_address1 cl_jur_address, c.jur_postcode_zip cl_jur_postcode_zip, c.jur_status cl_jur_status,
    COALESCE(sum(vds_bet), 0) tot_bet,
    COALESCE(sum(vds_ticket_paid_amount), 0) tot_win,
    COALESCE(sum(vds_ticket_paid_amount_tax), 0)  tot_win_tax,
    COALESCE(sum(vds_tickets), 0)  tickets_played,
    COALESCE(sum(vds_ticket_erased), 0)  tickets_erased,
    cur_code_for_web
    FROM jurisdiction ca
    JOIN jurisdiction n ON n.jur_parent_id = ca.jur_id ";
    if($_SESSION['jurisdiction_class']=='nation'){
        $sql1 .=" AND n.jur_id=". $_SESSION['jurisdiction_id'];
    }
    $sql1.=" JOIN jurisdiction r ON r.jur_parent_id = n.jur_id ";
    if($_SESSION['jurisdiction_class']=='region'){
        $sql1 .=" AND r.jur_id=". $_SESSION['jurisdiction_id'];
    }
    $sql1.=" JOIN jurisdiction d ON d.jur_parent_id = r.jur_id ";
    if($_SESSION['jurisdiction_class']=='district'){
        $sql1 .=" AND d.jur_id=". $_SESSION['jurisdiction_id'];
    }
    elseif($_POST['dstr']!='' && $companies=='' && $nrclubs!=$totcompanies){
        $sql1.=" AND d.jur_id IN (".$dbh->escape($_POST['dstr']).")";
    }

    $sql1.=" JOIN jurisdiction c ON c.jur_parent_id = d.jur_id ";
    if($_SESSION['jurisdiction_class']=='club'){
        $sql1 .=" AND c.jur_id=". $_SESSION['jurisdiction_id'];
    }
    elseif($companies!='' && $nrclubs!=$totcompanies){
        $sql1.=" AND c.jur_id IN ($allclubs) " ;
    }
    $sql1 .="
    JOIN currency ON c.jur_currency=cur_id
    LEFT JOIN virtual_daily_summary ON
            c.jur_id = vds_club_id
            AND vds_date BETWEEN '".$dbh->escape($date_start)."' AND '".$dbh->escape($date_end)."'
            GROUP BY dis_jur_id, cl_jur_id

        UNION ALL
        SELECT d.jur_id dis_jur_id,
            d.jur_name dis_jur_name,
            c.jur_id cl_jur_id,
            c.jur_name cl_jur_name,
            c.jur_city cl_jur_city,
            c.jur_address1 cl_jur_address,
            c.jur_postcode_zip cl_jur_postcode_zip,
            c.jur_status cl_jur_status,
            COALESCE(sum(pre_bet), 0) tot_bet,
            0 tot_win,
            0 tot_win_tax,
            count(pre_game_num) tickets_played,
            sum(case when pre_bet_string like '%handerased' then 1 else 0 end) tickets_erased,
            cur_code_for_web
            FROM jurisdiction ca
            JOIN jurisdiction n ON n.jur_parent_id = ca.jur_id ";
    if($_SESSION['jurisdiction_class']=='nation'){
        $sql1 .=" AND n.jur_id=". $_SESSION['jurisdiction_id'];
    }

    $sql1.=" JOIN jurisdiction r ON r.jur_parent_id = n.jur_id ";

    if($_SESSION['jurisdiction_class']=='region'){
        $sql1 .=" AND r.jur_id=". $_SESSION['jurisdiction_id'];
    }
    $sql1.=" JOIN jurisdiction d ON d.jur_parent_id = r.jur_id ";
    if($_SESSION['jurisdiction_class']=='district'){
        $sql1 .=" AND d.jur_id=". $_SESSION['jurisdiction_id'];
    }
    elseif($_POST['dstr']!='' && $companies=='' && $nrclubs!=$totcompanies){
        $sql1.=" AND d.jur_id IN (".$dbh->escape($_POST['dstr']).")";
    }

    $sql1.=" JOIN jurisdiction c ON c.jur_parent_id = d.jur_id
            JOIN punter ON pun_betting_club = c.jur_id ";
    if($_SESSION['jurisdiction_class']=='club'){
        $sql1 .=" AND pun_betting_club=". $_SESSION['jurisdiction_id'];
    }
    if($companies!='' && $nrclubs!=$totcompanies){
        $sql1.=" AND pun_betting_club IN ($allclubs) " ;
    }
    $sql1 .="
    JOIN currency ON c.jur_currency=cur_id
    JOIN virtual_games_user ON vgu_pun_id = pun_id
    LEFT JOIN punter_result  ON pre_pun_id = pun_id AND pre_time BETWEEN '".$today." 00:00:00' AND '".$today." 23:59:59'
                GROUP BY dis_jur_id, cl_jur_id ";

    $sql1.="
    UNION ALL
        SELECT d.jur_id dis_jur_id, d.jur_name dis_jur_name, c.jur_id cl_jur_id, c.jur_name cl_jur_name, c.jur_city cl_jur_city,
        c.jur_address1 cl_jur_address, c.jur_postcode_zip cl_jur_postcode_zip, c.jur_status cl_jur_status,
        0 tot_bet,
        sum(vtp_amount_payed) tot_win,
        sum(vtp_amount_payed * 0.01) tot_win_tax,
        0 tickets_played,
        0 tickets_erased,
        cur_code_for_web
        FROM jurisdiction ca
        JOIN jurisdiction n ON n.jur_parent_id = ca.jur_id ";
        if($_SESSION['jurisdiction_class']=='nation'){
            $sql1 .=" AND n.jur_id=". $_SESSION['jurisdiction_id'];
        }
        $sql1.=" JOIN jurisdiction r ON r.jur_parent_id = n.jur_id ";
        if($_SESSION['jurisdiction_class']=='region'){
            $sql1 .=" AND r.jur_id=". $_SESSION['jurisdiction_id'];
        }
        $sql1.=" JOIN jurisdiction d ON d.jur_parent_id = r.jur_id ";
        if($_SESSION['jurisdiction_class']=='district'){
            $sql1 .=" AND d.jur_id=". $_SESSION['jurisdiction_id'];
        }
        elseif($_POST['dstr']!='' && $companies=='' && $nrclubs!=$totcompanies){
            $sql1.=" AND d.jur_id IN (".$dbh->escape($_POST['dstr']).")";
        }

        $sql1.=" JOIN jurisdiction c ON c.jur_parent_id = d.jur_id ";
        if($_SESSION['jurisdiction_class']=='club'){
            $sql1 .=" AND c.jur_id=". $_SESSION['jurisdiction_id'];
        }
        elseif($companies!='' && $nrclubs!=$totcompanies){
            $sql1.=" AND c.jur_id IN ($allclubs) " ;
        }
     $sql1.="   JOIN punter ON pun_betting_club = c.jur_id
                JOIN currency ON c.jur_currency=cur_id
        JOIN virtual_ticket_payed ON vtp_pun_id = pun_id AND vtp_time BETWEEN '".$today." 00:00:00' AND '".$today." 23:59:59'
        GROUP BY dis_jur_id, cl_jur_id    ";

    $sql1.="UNION ALL SELECT d.jur_id dis_jur_id, d.jur_name dis_jur_name, c.jur_id cl_jur_id, c.jur_name cl_jur_name, c.jur_city cl_jur_city, c.jur_address1 cl_jur_address, c.jur_postcode_zip cl_jur_postcode_zip, c.jur_status cl_jur_status,
    sum(IF(TStato <> 4, coalesce(Importo, 0) + coalesce(Tax, 0), 0)) total_bet,
    0 total_win,
    0 tot_win_tax,
    count(CodiceTicket) tickets_played,
    sum(case when TStato = 4 then 1 else 0 end) tickets_erased,
    cur_code_for_web
    FROM virtual_games_user JOIN punter ON vgu_pun_id = pun_id
    JOIN jurisdiction c on pun_betting_club=c.jur_id ";
    if($companies!='' && $nrclubs!=$totcompanies){
        $sql1.=" AND pun_betting_club IN ($allclubs) " ;
    }
    $sql1.=" JOIN jurisdiction d ON c.jur_parent_id = d.jur_id ";
    if($_SESSION['jurisdiction_class']=='district'){
        $sql1 .=" AND d.jur_id=". $_SESSION['jurisdiction_id'];
    }
    elseif($_POST['dstr']!='' && $companies=='' && $nrclubs!=$totcompanies){
        $sql1.=" AND d.jur_id IN (".$dbh->escape($_POST['dstr']).")";
    }
    $sql1.=" JOIN jurisdiction r ON d.jur_parent_id =r.jur_id ";

    if($_SESSION['jurisdiction_class']=='region'){
        $sql1 .=" AND r.jur_id=". $_SESSION['jurisdiction_id'];
    }

   $sql1.=" JOIN jurisdiction n on r.jur_parent_id = n.jur_id and n.jur_parent_id = 1
            JOIN currency ON c.jur_currency=cur_id
            LEFT JOIN Ticket_dev ON ExtIDUtente = pun_id AND TStamp BETWEEN '".$today." 00:00:00' AND '".$today." 23:59:59'
            GROUP BY dis_jur_id, cl_jur_id    ";



    $sql1.=" ) t
    GROUP BY dis_jur_id, cl_jur_id
    ORDER BY dis_jur_id, cl_jur_id";
    return $sql1;
} // end function getVirtualFinancialInfoOldAndCurrent()


/**
 * Return the query generated to extract the virtual financial info less than the current day
 */
function getVirtualFinancialInfoOld($date_start,$date_end,$companies,$totcompanies,$allclubs){
    global $dbh,$nrclubs,$districts,$dstr;
    $sql2=" SELECT d.jur_id dis_jur_id,
                d.jur_name dis_jur_name,
                c.jur_id cl_jur_id,
                c.jur_name cl_jur_name,
                c.jur_city cl_jur_city,
                c.jur_address1 cl_jur_address,
                c.jur_postcode_zip cl_jur_postcode_zip,
                c.jur_status cl_jur_status,
                COALESCE(sum(vds_bet), 0) tot_bet,
                COALESCE(sum(vds_ticket_paid_amount), 0) tot_win,
                COALESCE(sum(vds_ticket_paid_amount_tax), 0) tot_win_tax,
                COALESCE(sum(vds_tickets), 0)  tickets_played,
                COALESCE(sum(vds_ticket_erased), 0)  tickets_erased,
                cur_code_for_web
    FROM jurisdiction ca
    JOIN jurisdiction n ON n.jur_parent_id = ca.jur_id ";
    if($_SESSION['jurisdiction_class']=='nation'){
        $sql2 .=" AND n.jur_id=". $_SESSION['jurisdiction_id'];
    }

    $sql2.=" JOIN jurisdiction r ON r.jur_parent_id = n.jur_id ";

    if($_SESSION['jurisdiction_class']=='region'){
        $sql2 .=" AND r.jur_id=". $_SESSION['jurisdiction_id'];
    }
    $sql2.=" JOIN jurisdiction d ON d.jur_parent_id = r.jur_id ";
    if($_SESSION['jurisdiction_class']=='district'){
        $sql2 .=" AND d.jur_id=". $_SESSION['jurisdiction_id'];
    }
    elseif($_POST['dstr']!='' && $companies=='' && $nrclubs!=$totcompanies){
        $sql2.=" AND d.jur_id IN (".$dbh->escape($_POST['dstr']).")";
    }

    $sql2.=" JOIN jurisdiction c ON c.jur_parent_id = d.jur_id ";
    if($_SESSION['jurisdiction_class']=='club'){
        $sql2 .=" AND c.jur_id=". $_SESSION['jurisdiction_id'];
    }
    elseif($companies!='' && $nrclubs!=$totcompanies){
        $sql2.=" AND c.jur_id IN ($allclubs) " ;
    }
    $sql2 .=" LEFT JOIN virtual_daily_summary ON
            c.jur_id = vds_club_id
            AND vds_date BETWEEN '".$dbh->escape($date_start)."' AND '".$dbh->escape($date_end)."'
            LEFT JOIN currency ON c.jur_currency=cur_id
            GROUP BY dis_jur_id, cl_jur_id ";
    return $sql2;
} // end function getVirtualFinancialInfoOld()



/**
 * Return the query generated to extract the virtual financial info less than today
 */
function getVirtualFinancialInfoCurrent($date_start,$date_end,$companies,$totcompanies,$allclubs){
    global $dbh,$nrclubs,$districts,$dstr;
    $today=date('Y-m-d');

    $sql3="
     SELECT dis_jur_id, dis_jur_name, cl_jur_id, cl_jur_name, cl_jur_city, cl_jur_address, cl_jur_postcode_zip,
      cl_jur_status, sum(tot_bet) tot_bet, sum(tot_win) tot_win,sum(tot_win_tax) tot_win_tax,sum(tickets_played) tickets_played, sum(tickets_erased) tickets_erased,cur_code_for_web
     FROM (
 SELECT d.jur_id dis_jur_id,d.jur_name dis_jur_name,
          c.jur_id cl_jur_id,
          c.jur_name cl_jur_name,
          c.jur_city cl_jur_city,
          c.jur_address1 cl_jur_address,
          c.jur_postcode_zip cl_jur_postcode_zip,
          c.jur_status cl_jur_status,
          COALESCE(sum(pre_bet), 0) tot_bet,
          0 tot_win,
          0 tot_win_tax,
          count(pre_game_num) tickets_played,
          sum(case when pre_bet_string like '%handerased' then 1 else 0 end) tickets_erased,
          cur_code_for_web
            FROM jurisdiction ca
            JOIN jurisdiction n ON n.jur_parent_id = ca.jur_id ";
    if($_SESSION['jurisdiction_class']=='nation'){
        $sql3 .=" AND n.jur_id=". $_SESSION['jurisdiction_id'];
    }
    $sql3.=" JOIN jurisdiction r ON r.jur_parent_id = n.jur_id ";
    if($_SESSION['jurisdiction_class']=='region'){
        $sql3 .=" AND r.jur_id=". $_SESSION['jurisdiction_id'];
    }
    $sql3.=" JOIN jurisdiction d ON d.jur_parent_id = r.jur_id ";
    if($_SESSION['jurisdiction_class']=='district'){
        $sql3 .=" AND d.jur_id=". $_SESSION['jurisdiction_id'];
    }
    elseif($_POST['dstr']!='' && $companies=='' && $nrclubs!=$totcompanies){
        $sql3.=" AND d.jur_id IN (".$dbh->escape($_POST['dstr']).")";
    }
    $sql3.=" JOIN jurisdiction c ON c.jur_parent_id = d.jur_id";
    if($_SESSION['jurisdiction_class']=='club'){
        $sql3 .=" AND c.jur_id=". $_SESSION['jurisdiction_id'];
    }
    elseif($companies!='' && $nrclubs!=$totcompanies){
        $sql3.=" AND c.jur_id IN ($allclubs) " ;
    }
     $sql3.=" LEFT JOIN punter ON pun_betting_club = c.jur_id
    LEFT JOIN currency ON c.jur_currency=cur_id ";
    $sql3 .="
    JOIN virtual_games_user ON vgu_pun_id = pun_id
    LEFT JOIN punter_result ON pre_pun_id = pun_id AND pre_time BETWEEN '".$today." 00:00:00' AND '".$today." 23:59:59'
                GROUP BY dis_jur_id, cl_jur_id";

    $sql3.="
    UNION ALL
    SELECT d.jur_id dis_jur_id, d.jur_name dis_jur_name, c.jur_id cl_jur_id, c.jur_name cl_jur_name, c.jur_city cl_jur_city,
    c.jur_address1 cl_jur_address, c.jur_postcode_zip cl_jur_postcode_zip, c.jur_status cl_jur_status,
    0 tot_bet,
    sum(vtp_amount_payed) tot_win,
    sum(vtp_amount_payed * 0.01) tot_win_tax,
    0 tickets_played,
    0 tickets_erased,
    cur_code_for_web
         FROM jurisdiction ca
        JOIN jurisdiction n ON n.jur_parent_id = ca.jur_id ";
    if($_SESSION['jurisdiction_class']=='nation'){
        $sql3 .=" AND n.jur_id=". $_SESSION['jurisdiction_id'];
    }
    $sql3.=" JOIN jurisdiction r ON r.jur_parent_id = n.jur_id ";
    if($_SESSION['jurisdiction_class']=='region'){
        $sql3 .=" AND r.jur_id=". $_SESSION['jurisdiction_id'];
    }
    $sql3.=" JOIN jurisdiction d ON d.jur_parent_id = r.jur_id ";
    if($_SESSION['jurisdiction_class']=='district'){
        $sql3 .=" AND d.jur_id=". $_SESSION['jurisdiction_id'];
    }
    elseif($_POST['dstr']!='' && $companies=='' && $nrclubs!=$totcompanies){
        $sql3.=" AND d.jur_id IN (".$dbh->escape($_POST['dstr']).")";
    }

    $sql3.=" JOIN jurisdiction c ON c.jur_parent_id = d.jur_id ";
    if($_SESSION['jurisdiction_class']=='club'){
        $sql3 .=" AND c.jur_id=". $_SESSION['jurisdiction_id'];
    }
    elseif($companies!='' && $nrclubs!=$totcompanies){
        $sql3.=" AND c.jur_id IN ($allclubs) " ;
    }
    $sql3.="   JOIN punter ON pun_betting_club = c.jur_id
        LEFT JOIN currency ON c.jur_currency=cur_id
        JOIN virtual_ticket_payed ON vtp_pun_id = pun_id AND vtp_time BETWEEN '".$today." 00:00:00' AND '".$today." 23:59:59'
        GROUP BY dis_jur_id, cl_jur_id   ";


    $sql3.=" UNION ALL SELECT d.jur_id dis_jur_id, d.jur_name dis_jur_name, c.jur_id cl_jur_id, c.jur_name cl_jur_name, c.jur_city cl_jur_city, c.jur_address1 cl_jur_address, c.jur_postcode_zip cl_jur_postcode_zip, c.jur_status cl_jur_status,
    sum(IF(TStato <> 4, coalesce(Importo, 0) + coalesce(Tax, 0), 0)) total_bet,
    0 total_win,
    0 tot_win_tax,
    count(CodiceTicket) tickets_played,
    sum(case when TStato = 4 then 1 else 0 end) tickets_erased,
    cur_code_for_web
    FROM virtual_games_user JOIN punter ON vgu_pun_id = pun_id
    JOIN jurisdiction c on pun_betting_club=c.jur_id ";
    if($companies!='' && $nrclubs!=$totcompanies){
        $sql3.=" AND pun_betting_club IN ($allclubs) " ;
    }
    $sql3.=" JOIN jurisdiction d ON c.jur_parent_id = d.jur_id ";
    if($_SESSION['jurisdiction_class']=='district'){
        $sql3 .=" AND d.jur_id=". $_SESSION['jurisdiction_id'];
    }
    elseif($_POST['dstr']!='' && $companies=='' && $nrclubs!=$totcompanies){
        $sql3.=" AND d.jur_id IN (".$dbh->escape($_POST['dstr']).")";
    }
    $sql3.=" JOIN jurisdiction r ON d.jur_parent_id =r.jur_id ";

    if($_SESSION['jurisdiction_class']=='region'){
        $sql3 .=" AND r.jur_id=". $_SESSION['jurisdiction_id'];
    }

    $sql3.=" JOIN jurisdiction n on r.jur_parent_id = n.jur_id and n.jur_parent_id = 1
            LEFT JOIN currency ON c.jur_currency=cur_id
            LEFT JOIN Ticket_dev ON ExtIDUtente = pun_id AND TStamp BETWEEN '".$today." 00:00:00' AND '".$today." 23:59:59'
            GROUP BY dis_jur_id, cl_jur_id ) t
    GROUP BY dis_jur_id, cl_jur_id
    ORDER BY dis_jur_id, cl_jur_id   ";
    return $sql3;
} // end function getVirtualFinancialInfoCurren()


?>
