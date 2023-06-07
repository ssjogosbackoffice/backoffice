<?php check_access('totem_ticket', true);
/**
 * @author Marian
 */
include_once 'JurisdictionsList.class.inc';
$date_start = (isset($_POST['date_start']) &&$_POST['date_start']!='-1') ? $_POST['date_start'] : date("Y-m-d", time() - (60 * 60 * 24));
$date_end = (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d", time());
$type = (isset($_POST['type']) ? $_POST['type'] : 'd' );
if($type=='m'){
    $date_start=$_POST['monthHelperStart']."-01";
    $date_end=date('Y-m-t',strtotime($_POST['monthHelperEnd']));
}
$datetime1 = new DateTime($date_start);
$datetime2 = new DateTime($date_end);
$interval = $datetime1->diff($datetime2);
$totemHierarchy=getTotemHierarchy();
$active = (isset($_POST['active']) ? $_POST['active'] : 'general' );
?>

    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
    <style>
        .searchTable {
            background-color: rgba(0, 0, 0, 0) !important;
            width: 440px !important;
            margin:auto;
            margin-bottom: 30px;
        }
    </style>
    <script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/highcharts.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/modules/exporting.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/jstree/jstree.js" type="text/javascript"></script>
    <script src="/media/jquery.dataTables.js" type="text/javascript"></script>
    <script type="text/javascript">
        var servicesUrl="/services/totem_services.php";
        $(function () {



            $('.periodType').trigger('change');
            $('#selectAll').change(function(){
                $("#deselectAll").prop('checked', false);
                $('#jurisdictionTree').jstree('select_all');
            });
            $('#deselectAll').change(function(){
                $("#selectAll").prop('checked', false);
                $('#jurisdictionTree').jstree('deselect_all');
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

            $("#jurisdictionTree")
                .jstree({
                    "plugins" : [
                        "search", "checkbox" ,
                        "types"
                    ],
                    "checkbox" : {
                        "three_state":true,
                        "whole_node":true
                    },
                    "search":{
                        "fuzzy":false,
                        "show_only_matches":true
                    },
                    "types" : {
                        "nation" : {
                            "icon" : "../media/images/n.png"
                        },
                        "region" : {
                            "icon" : "../media/images/r.png"
                        },
                        "district" : {
                            "icon" :  "../media/images/d.png"
                        },
                        "club" : {
                            "icon": "../media/images/c.png"
                        },
                        "totem" : {
                            "icon": "../media/images/t.png"
                        }
                    }
                }).bind("select_node.jstree",  function (e, data) {
                    $("#deselectAll").prop('checked', false);
                    var allselected=true;
                    $.each( data.instance.get_node('#').children, function( index, value ){
                        if(!data.instance.is_selected(value)){
                            allselected=false;
                        }
                        if(allselected==true){
                            $("#selectAll").prop('checked', true);
                        }
                        else{
                            $("#selectAll").prop('checked', false);
                        }
                    });

                }).bind("deselect_node.jstree",  function (e, data) {
                    $("#deselectAll").prop('checked', false);
                    return $("#selectAll").prop('checked', false);
                }).bind("loaded.jstree", function (event, data) {
                    $('#selectAll').trigger('click');
                    if($.trim(data.instance.element.context.innerText)=='-1'){
                        $('#jurisdictionTree').empty().append("<div class='alert alert-error'><strong><?=$lang->getLang("No result found")?>!</strong></div>");
                    }
                }).bind("select_all.jstree",  function (e, data) {
                    $("#selectAll").prop('checked', true);
                    return $("#selectAll").prop('checked', true);
                }).bind("open_all.jstree",  function (e, data) {
                    var allselected=true;
                    $.each( data.instance.get_node('#').children, function( index, value ){
                        if(!data.instance.is_selected(value)){
                            allselected=false;
                        }
                        if(allselected==true){
                            $("#selectAll").prop('checked', true);
                        }
                        else{
                            $("#selectAll").prop('checked', false);
                        }
                    });
                    if($('#jurisdictionTreeSearch').val()!=''){
                        $("#jurisdictionTree").jstree('search',$('#jurisdictionTreeSearch').val());
                    }
                }).bind("search.jstree", function (e, data) {
                    if (data.nodes.length == 0){
                        $('#jurAlert').fadeIn();
                    }
                    else{
                        $('#jurAlert').fadeOut();
                    }
                });

            $('#jurisdictionTree').jstree('select_all');
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

     results=$('#ticketsResultTable').DataTable( {
        "processing": true,
        "serverSide": true,
        columnDefs: [
            {
                targets: 2 ,
                className:'ticketCode'
            }
            ],
        "ajax":{
            "url": servicesUrl,
            "data": function ( d ) {
                d.pk = "getTotemTickets";
                d.totems= JSON.stringify($('#jurisdictionTree').jstree().get_selected());
                d.startDate=$("#date_start").val();
                d.endDate=($("#date_end").closest('tr').is(":visible")?$("#date_end").val() : '');
                d.ticketCode=$("#tcode").val();
                d.status=$("#status").val();
            }
        }, 'iDisplayLength': 10,'sPaginationType':"full_numbers"
    });


            $('#getTickets').click(function() {
              results.ajax.reload();
            });


        });

        $(document).on('click','.payTicket',function(){
            var ticketCode=$(this).closest("tr").find('.ticketCode').text().trim();
            jConfirm("<?=$lang->getLang("Are you sure you want to pay this ticket?")?>","Pay ticket",function (answer) {
                if(answer){
                    $.post(servicesUrl,{
                        pk:"payTicket",
                        ticket:ticketCode
                    }, function(data){
                     if(data==1){
                         results.ajax.reload( null, false );
                     }
                    })
                }
            });
        });



        /* date functions  */
        $(document).on('change', ".periodType", function() {
            var currVal=$(this).val();
            if(currVal==3){
                $('#date_end').closest('tr').hide();
                $('#dateFromLabel').html('Day');
            }
            else{
                $('#date_end').closest('tr').show();
                $('#dateFromLabel').html('From');
            }
            $(this).closest('table').find('.timeHelpers').hide();
            $(this).closest('table').find('.period'+currVal).show();
        });
        $(document).on('change', '.monthHelper', function() {
            var d = new Date($(this).val());
            createMonthHelper(d,$(this));
        });
        $(document).on('change', '.weekHelper', function() {
            createWeekHelper($(this).val(),$(this));
        });

        function createWeekHelper(dateobj,container){
            if (typeof dateobj !== 'undefined' || dateobj!=''){
                dates=dateobj.split('~');
                firstDay=dates[0];
                lastDay=dates[1];
                container.closest('table').find( ".dateStartHelper" ).datepicker('setDate', firstDay);
                container.closest('table').find( ".dateEndHelper" ).datepicker('setDate', lastDay);
                container.closest('table').find('.monthHelper').val('');
            }
        }

        function createMonthHelper(dateobj,container){
            if (typeof dateobj !== 'undefined' || dateobj!=''){
                var firstDay = new Date(dateobj.getFullYear(), dateobj.getMonth(), 1);
                container.closest('table').find( ".dateStartHelper" ).datepicker('setDate', firstDay);
                var lastDay = new Date(dateobj.getFullYear(), dateobj.getMonth() + 1, 0);
                container.closest('table').find( ".dateEndHelper" ).datepicker('setDate', lastDay);
                container.closest('table').find( ".weekHelper").val('');
            }
        }
        /* end date functions  */
    </script>

    <div class="container" style="position: absolute;left:0;right: 0;width: 100%" >
        <table class="table table-bordered">
            <tr>
                <td class="navbar-inner" colspan="2">
                    <h3 class="text-center"><?=$lang->getLang("Totem Ticket")?></h3>
                </td>
            </tr>
            <tr>
                <td class="span4">
                    <div class="contentAdmin">
                        <div class="contentHeadAdmin"><?=$lang->getLang("Choose totems")?></div>
                        <div class="well">
                            <div class="alert hidden alert-danger" id="jurAlert">
                                <?=$lang->getLang("No result found")?>
                            </div>
                            <label class="checkbox fleft">
                                <input type="checkbox" id="selectAll" value="1"><?=$lang->getLang("Select All")?>
                            </label>
                            <label class="checkbox fright">
                                <input type="checkbox" id="deselectAll" value="1"><?=$lang->getLang("Deselect All")?>
                            </label>
                            <input type="text" id='jurisdictionTreeSearch' placeholder="<?=$lang->getLang("Search totem")?>">

                            <div id="jurisdictionTree">
                                <ul>
                                    <?=showChild($totemHierarchy,true, true);?>
                                </ul>
                            </div>
                        </div>
                    </div>
                </td>
                <td style='vertical-align: top'>
                    <div class="well">
                        <div class="row"><div class="span7">
                                <table  class="searchTable table table-bordered">
                                    <tbody>
                                    <tr><td>
                                            <div class='label label-inverse'><?=$lang->getLang("Choose period type")?></div>
                                        </td>
                                        <td>
                                            <div class="controls">
                                                <div class="input-prepend">
                                                    <span class="add-on"><i class="icon-calendar"></i></span>
                                            <select name="periodType"  class="periodType">
                                                <option value="0" <?=($periodType==0?'selected': '')?>><?=$lang->getLang("Custom")?></option>
                                                <option value="3" <?=($periodType==3?'selected': '')?>><?=$lang->getLang("Daily")?></option>
                                                <option value="1" <?=($periodType==1?'selected': '')?>><?=$lang->getLang("Weekly")?></option>
                                                <option value="2" <?=($periodType==2?'selected': '')?>><?=$lang->getLang("Monthly")?></option>

                                            </select>
                                                    </div></div>
                                        </td>
                                    </tr>
                                    <tr  class="period2 timeHelpers">
                                        <td><div class='label label-inverse'><?=$lang->getLang("Choose month")?></div></td>
                                        <td>
                                            <div class="controls">
                                                <div class="input-prepend">
                                                    <span class="add-on"><i class="icon-calendar"></i></span>
                                                    <?=month_select_box()?>
                                                </div>
                                            </div></td>
                                    </tr>
                                    <tr  class="period1 timeHelpers">
                                        <td><div class='label label-inverse'><?=$lang->getLang("Choose week")?></div></td>
                                        <td>
                                            <div class="controls">
                                                <div class="input-prepend">
                                                    <span class="add-on"><i class="icon-calendar"></i></span>
                                                    <?=week_select_box()?>
                                                </div>
                                            </div>
                                        </td>
                                    </tr>

                                    <tr>
                                        <td><div class='label label-inverse' id="dateFromLabel"><?=$lang->getLang("From")?></div></td>
                                        <td><div class="controls">
                                                <div class="input-prepend">
                                                    <span class="add-on"><i class="icon-calendar"></i></span>
                                                    <input type='text' name='date_start' id='date_start' class="dateStartHelper"  value="<?=$date_start?>" >
                                                </div>
                                            </div>
                                        </td>
                                    </tr>
                                    <tr>
                                        <td><div class='label label-inverse'><?=$lang->getLang("Until")?></div></td>
                                        <td><div class="controls">
                                                <div class="input-prepend">
                                                    <span class="add-on"><i class="icon-calendar"></i></span>
                                                    <input type='text' name='date_end' id='date_end' class="dateEndHelper" value="<?=$date_end?>">
                                                </div>
                                            </div>
                                        </td>
                                    </tr>
                                    <tr>
                                        <td><div class='label label-inverse'><?=$lang->getLang("Status")?></div></td>
                                        <td>
                                            <div class="controls">
                                                <div class="input-prepend">
                                                    <span class="add-on"><i class="icon-th"></i></span>
                                                    <select name="status" id="status" >
                                                        <option value="3" ><?=$lang->getLang("All")?></option>
                                                        <option value="0" ><?=$lang->getLang("Opened")?></option>
                                                        <option value="1" ><?=$lang->getLang("Reused")?></option>
                                                        <option value="2" ><?=$lang->getLang("Withdrawn")?></option>
                                                    </select>
                                                    </div>
                                            </div>
                                        </td>
                                    </tr>
                                    <tr>
                                        <td><div class='label label-inverse'><?=$lang->getLang("Ticket code")?></div></td>
                                        <td><div class="controls">
                                                <div class="input-prepend">
                                                    <span class="add-on"><i class="icon-th"></i></span>
                                                    <input type='text' name='tcode' id='tcode' placeholder="Optional">
                                                </div>
                                            </div>
                                        </td>
                                    </tr>
                                    <tr><td colspan="2">
                                            <button class="btn btn-info" id="getTickets">Show Tickets</button>
                                        </td> </tr></tbody></table>
                                <br />
                                <br />
                                <div id='duration' class="tip fleft"></div>
                                <br />
                            </div></div>
                        <div class="container-fluid">
                            <div class="row">
                                <br/>
                                <table id="ticketsResultTable" class="display table table-striped table-hover table-bordered table-condensed" >
                                    <thead>
                                    <tr>
                                        <th>Totem code</th>
                                        <th>User</th>
                                        <th>Ticked code</th>
                                        <th>Ticket time</th>
                                        <th>Ticket last update</th>
                                        <th>Credits</th>
                                        <th>Point</th>
                                        <th>Status</th>
                                        <th>Pay</th>

                                    </tr>
                                    </thead>
                                    <tbody>
                                    <tr>
                                        <td></td>
                                        <td></td>
                                        <td class="ticketCode"></td>
                                        <td></td>
                                        <td></td>
                                        <td></td>
                                        <td></td>
                                        <td></td>
                                    </tr>
                                    </tbody>
                                </table>
                            </div>
                        </div>
                    </div>
                </td>
            </tr>
        </table>
    </div>
<?



function getTotemByJurisdiction($jurisdiction,$class){
    global $dbh;
    $sql="SELECT n.jur_name nation, n.jur_id nation_id,n.jur_code nation_code,
        r.jur_name region, r.jur_id region_id, r.jur_code region_code,
        d.jur_name district, d.jur_id district_id,d.jur_code district_code,
        c.jur_name club, c.jur_id club_id, c.jur_code club_code, ttm_id, ttm_code
        FROM  jurisdiction n
        JOIN jurisdiction r ON r.jur_parent_id = n.jur_id
        JOIN jurisdiction d ON d.jur_parent_id = r.jur_id
        JOIN  jurisdiction c ON c.jur_parent_id = d.jur_id
        JOIN totem on ttm_club_id=c.jur_id
        AND c.jur_class = 'club'
        WHERE ttm_id is not null
        ";

    if($class=='region')
    {
        $sql.="AND  n.jur_id=$jurisdiction";
    }
    elseif($class=='district'){
        $sql.="AND r.jur_id=$jurisdiction";
    }
    elseif($class=='club'){

        if($_SESSION['jurisdiction_class']=='club'){
            $sql.="AND c.jur_id=$jurisdiction";
        }
        else{
            $sql.="AND d.jur_id=$jurisdiction";
        }
    }
    $sql.=" GROUP BY ttm_id ORDER BY n.jur_id,r.jur_id,d.jur_id,c.jur_id, ttm_id";
    $result=$dbh->doCachedQuery($sql,0);
    return $result;

}

function getTotemHierarchy(){
    $class=getSubJurisdiction($_SESSION['jurisdiction_class']);
    $jurisdiction=$_SESSION['jurisdiction_id'];
    $nodes=getTotemByJurisdiction($jurisdiction,$class);

    if($nodes->getNumRows()>0){
        $jurisdictions=array();
        while ( $nodes->hasNext () ) {
            $row = $nodes->next ();
            if(!is_array($jurisdictions[$row['nation_id']])){
                $jurisdictions[$row['nation_id']]['id']=$row['nation_id'];
                $jurisdictions[$row['nation_id']]['class']='nation';
                $jurisdictions[$row['nation_id']]['name']=$row['nation'];
                $jurisdictions[$row['nation_id']]['code']=strtoupper($row['nation_code']);
                $jurisdictions[$row['nation_id']]['child']=array();
            }

            if(!is_array($jurisdictions[$row['nation_id']]['child'][$row['region_id']])){
                $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['id']=$row['region_id'];
                $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['code']=strtoupper($row['region_code']);
                $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['class']='region';
                $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['name']=$row['region'];
                $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child']=array();
            }

            if(!is_array($jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']])){
                $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['id']=$row['district_id'];
                $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['code']=strtoupper($row['district_code']);
                $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['class']='district';
                $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['name']=$row['district'];
                $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['child']=array();
            }

            if(!is_array($jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['child'][$row['club_id']])){
                $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['child'][$row['club_id']]['id']=$row['club_id'];
                $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['child'][$row['club_id']]['code']=strtoupper($row['club_code']);
                $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['child'][$row['club_id']]['name']=$row['club'];
                $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['child'][$row['club_id']]['class']='club';
                $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['child'][$row['club_id']]['child']=array();
            }

            $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['child'][$row['club_id']]['child'][$row['ttm_id']]['id']=$row['ttm_id'];
            $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['child'][$row['club_id']]['child'][$row['ttm_id']]['code']=strtoupper($row['ttm_code']);
            $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['child'][$row['club_id']]['child'][$row['ttm_id']]['name']=$row['ttm_code'];
            $jurisdictions[$row['nation_id']]['child'][$row['region_id']]['child'][$row['district_id']]['child'][$row['club_id']]['child'][$row['ttm_id']]['class']='totem';
        }
        $return=$jurisdictions;

    }else{
        $return ="<div class='alert alert-error'><strong>-1</strong></div>";
    }
    return ($return);
}

/**
 * @param $jurisdictionClass
 * @return string
 */
function getSubJurisdiction($jurisdictionClass){
    $subjurisdiction='';

    if($jurisdictionClass=='casino')
    {
        $subjurisdiction='nation';
    }
    elseif($jurisdictionClass=='nation')
    {
        $subjurisdiction='region';
    }
    elseif($jurisdictionClass=='region')
    {
        $subjurisdiction='district';
    }
    elseif($jurisdictionClass=='district'){
        $subjurisdiction='club';
    }
    elseif($jurisdictionClass=='club'){
        $subjurisdiction='club';
    }
    return $subjurisdiction;
}


function showChild($childArray,$onlyChild=false,$agencyCode=false){

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
        }elseif($value['class'] == 'totem'){
            $class_type = "totem";
        }
        if(!is_null($value['child'])){
            if(!$onlyChild ||($onlyChild && strpos(getAllSubJurisdictionsTotem($_SESSION['jurisdiction_class']), strtolower($class_type))!==false )){ ?>
                <li data-jstree='{ "type" : "<?=$class_type?>" }' class="jstree-closed <?=$class_type?>" id="<?=$value['id']?>"><?=(($agencyCode && $class_type!='casino')? "[".$value['code']."] " :'' )?><?=strtoupper($value['name'])?><ul><?php
            }
            showChild($value['child'],$onlyChild,$agencyCode);

            if(!$onlyChild ||($onlyChild && strpos(getAllSubJurisdictionsTotem($_SESSION['jurisdiction_class']), strtolower($class_type))!==false )){ ?>
                </ul></li><?php }
        }else{
            if(!$onlyChild ||($onlyChild && strpos(getAllSubJurisdictionsTotem($_SESSION['jurisdiction_class']), strtolower($class_type))!==false ) || $class_type=='club'){ ?>
                <li data-jstree='{ "type" : "<?=$class_type?>" }' class="<?=$class_type?>" id="<?=$value['name']?>"><?=strtoupper($value['name'])?></li><?php }
        }
    }
}

/**
 * @param $jurisdictionClass
 * @return string
 */
function getAllSubJurisdictionsTotem($jurisdictionClass){
    $subjurisdiction='';
    $jurisdictionClass=strtolower($jurisdictionClass);
    if($jurisdictionClass=='casino')
    {
        $subjurisdiction='nation,region,district,club,totem';
    }
    elseif($jurisdictionClass=='nation')
    {
        $subjurisdiction='region,district,club,totem';
    }
    elseif($jurisdictionClass=='region')
    {
        $subjurisdiction='district,club,totem';
    }
    elseif($jurisdictionClass=='district'){
        $subjurisdiction='club,totem';
    }
    elseif($jurisdictionClass=='club'){
        $subjurisdiction='totem';
    }
    return $subjurisdiction;
}
