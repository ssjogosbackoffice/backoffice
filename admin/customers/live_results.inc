<?php check_access('betting_live_transactions', true);
$jur =(isset($_GET['jur'])?$_GET['jur']: (isset($_REQUEST['jur']) ? substr($_REQUEST['jur'],1) : $_SESSION['jurisdiction_id'] ));
$cls = (isset($_REQUEST['cls']) ? $_REQUEST['cls'] : $_SESSION['jurisdiction_class'] );
$date_end = (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d");
$username = (isset($_POST['username']) ? $_POST['username'] : '');
$searchType = (isset($_POST['searchType']) ? $_POST['searchType'] : '0');
$jurisdiction_id = (isset($_REQUEST['jurisdictions']) ? substr($_REQUEST['jurisdictions'] ,1) : $_SESSION['jurisdiction_id']);
$jurisdiction_class = (isset($_REQUEST['jurisdictionsType']) ? $_REQUEST['jurisdictionsType'] : $_SESSION['jurisdiction_class']);

?>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
<script src="/media/jquery.dataTables.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
 <script src="<?= secure_host ?>/media/jstree3/jstree.js" type="text/javascript"></script>
<script type="text/javascript">
    $(function() {
        $( "#date_start" ).datepicker({
            changeMonth: true,
            numberOfMonths:1
        });
        $(".tableDetailed").dataTable({'sPaginationType':"full_numbers"});
             var $tree=$('#jurisdictionTree').jstree({

            "plugins" : [
                "search",
                "types",
                "unique",
                "state"
            ],
            "types": {
                "casino": {
                    "icon": "../media/images/ca.png"
                }, "nation": {
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
            ,'core' : {
                'data' : {
                    'url' : function (node) {
                        return '/services/jurisdiction_services.inc';
                    },
                    'data' : function (node) {
                        return {'id': node.id, 'action': 'getJurisdictions',parent:true};
                    },
                    'dataType':'json'
                }
            }
        }).bind("select_node.jstree",  function (e, data) {
            $('.jur').val(data.node.id);
            $('.cls').val(data.node.li_attr.class);
        }).on('ready.jstree', function (e, data) {
                <?
                 if($jurisdiction_class!='casino') {
                     $jurSelected = getJurisdictionPathByIdAndClass($jurisdiction_id, $jurisdiction_class);
                 }
            else{
                $jurSelected=json_encode(array("n1"));
            }
                ?>
                jurToSelect = <?=$jurSelected;?>;
                jurToOpen = <?=$jurSelected;?>;
                jurToOpen.shift();
                data.instance.set_state({ core : {
                    'selected':  [jurToSelect[0]],
                    open : jurToOpen

                } });
            });

        $('#jurisdictionTreeSearch').keyup(function () {
            if(this.value.length>3){
                var start        = new Date().getTime();
                $.ajax({
                    url: "/services/jurisdiction_services.inc",
                    xhrFields: {
                        withCredentials: true
                    },
                    type: "GET",
                    dataType: "json",
                    data: {
                         action: "getJurisdictionsByName",
                         str: $('#jurisdictionTreeSearch').val()
                    }
                }).done(function (data) {
                        var res = "<ul>";
                        for (var jurisdictions in data) {
                            res += "<li><span class='title " + jurisdictions + "'>" + jurisdictions + "</span>";
                            res += "<ol class='" + jurisdictions + "'>";
                            for (var j = 0; j < data[jurisdictions].length; j++) {
                                res += "<li><span class='category'>" + jurisdictions + "</span> <a href='/live_results?jurisdictions=n" + data[jurisdictions][j]['id'] + "&jurisdictionsType="+jurisdictions+"'>" + data[jurisdictions][j]['name'] + "</a> ";
                                res += "</li>";
                            }
                            res += "</ol>";
                            res += "</li>";
                        }
                        res += "</ul>";
                        var end = new Date().getTime();

                        res = "<span class='tip'><?=$lang->getLang("Time taken to execute your request")?> " + ((end-start)/1000) + " s</span><br/>" + res;
                        $('#searchResult').html(res);
                    })
                    .always(function () {
                    });
            }
            else if  (this.value.length==0){
                $('#searchResult').empty();
            }
        });
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

    $(document).on('change', '.monthHelper', function() {
        var d = new Date($(this).val());
        createMonthHelper(d);
    });
    $(document).on('change', '.weekHelper', function() {
        createWeekHelper($(this).val());
    });

    function createWeekHelper(dateobj){
        if (typeof dateobj !== 'undefined' || dateobj!=''){
            dates=dateobj.split('~');
            firstDay=dates[0];
            lastDay=dates[1];
            $( ".dateStartHelper" ).datepicker('setDate', firstDay);
            $( ".dateEndHelper" ).datepicker('setDate', lastDay);
            $('.monthHelper').val('');
        }
    }

    function createMonthHelper(dateobj){
        if (typeof dateobj !== 'undefined' || dateobj!=''){
            var firstDay = new Date(dateobj.getFullYear(), dateobj.getMonth(), 1);
            $( ".dateStartHelper" ).datepicker('setDate', firstDay);
            var lastDay = new Date(dateobj.getFullYear(), dateobj.getMonth() + 1, 0);
            $( ".dateEndHelper" ).datepicker('setDate', lastDay);
            $('.weekHelper').val('');
        }
    }


       $('#liveResultsForm').on('submit',function(e){

       var jurisdiction=$('#jurisdictionTree').jstree().get_selected(true);
       if(jurisdiction.length<1){
           jAlert('<?=$lang->getLang("Please select a jurisdiction")?>');
           e.preventDefault();
       }
       else{
            var jurisdictionClass=jurisdiction[0].li_attr.class;
            var jurisdictionId=jurisdiction[0].id;
            $('#jurisdictions').val(jurisdictionId);
              $('#jurisdictionsType').val(jurisdictionClass);
              $(this).submit();

       }
           });


    });

    function getUserDetails(user,date,endDate,userName){
        $.post("/services/general_services.inc",{action:'7',user:user,date:date,endDate:endDate}, function(data){
            $(".userDetails"+userName).empty().append( data );
            $(".userlivedetails"+userName+date+endDate+user).dataTable({'sPaginationType':"full_numbers"});
        });

    }
</script>
<form action="/live_results" method="post" id="liveResultsForm">
<?php
/**
 *
 * @author Marian
 */
printHiddenInput('doQuery', 'yes');?>
<?php
$doQuery = $_POST['doQuery'];
$date_start = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d", time());
?>
<input type="hidden" name="jurisdictions" id="jurisdictions" value="">
<input type="hidden" name="jurisdictionsType" id="jurisdictionsType" value="">
<div class="container" style="position: absolute;left:0;right: 0;width: 100%" >
<table class="table table-bordered">
    <tr>
        <td class="navbar-inner"  colspan="2" ><h3 class="text-center"><?=$lang->getLang("Betting Transactions")?></h3></td>
    </tr>
    <tr>
    <td class="span4">
      <div class="contentAdmin">
                            <div class="contentHeadAdmin"><?=$lang->getLang("Jurisdiction chooser")?></div>
                            <div class="well">
                                <div class="alert hidden alert-danger" id="jurAlert">
                                    <?=$lang->getLang("No jurisdiction found")?>
                                </div>

                                <input type="text" id='jurisdictionTreeSearch' style="float:left;padding: 5px;width: 94%" placeholder="<?=$lang->getLang("Search (min. 4 char)")?>">
                            <div id="searchResult"></div>
                            <div class="clearfix"></div>
                             <br/>
                            <div id="jurisdictionTree"></div>
                            </div>
                        </div>
    </td>
        <td>
            <div class="well">
                <div class="row">

                    <div class="span6">
                      <table class="searchTable  table-bordered">
                            <tbody>
                            <tr>
                                <td><div class='label label-inverse'><?=$lang->getLang("Choose month")?></div></td>
                                <td><div class="controls">
                                        <div class="input-prepend">
                                            <span class="add-on"><i class="icon-calendar"></i></span>
                                            <?=month_select_box()?>
                                        </div>
                                    </div></td>
                            </tr>
                            <tr>
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
                                <td><div class='label label-inverse'><?=$lang->getLang("From")?></div></td>
                                <td> <div class="controls">
                                        <div class="input-prepend">
                                            <span class="add-on"><i class="icon-calendar"></i></span>
                                            <input type='text' name='date_start' id='date_start' class="dateStartHelper" placeholder="Date Start" value="<?=$date_start?>" >
                                        </div>
                                    </div></td>
                            </tr>
                            <tr>
                                <td><div class='label label-inverse'><?=$lang->getLang("Until")?></div></td>
                                <td><div class="controls">
                                        <div class="input-prepend">
                                            <span class="add-on"><i class="icon-calendar"></i></span>
                                            <input type='text' name='date_end' id='date_end' class="dateEndHelper"  placeholder="Date End" value="<?=$date_end?>">
                                        </div>
                                    </div></td>
                            </tr>
                            </tbody>
                        </table>
                    </div>
                   <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Username")?></div>
                            <br>
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-user"></i></span>
                                    <input type='text'  name='username' id='username' placeholder="Username"  value="<?=$username?>">
                                </div>
                            </div> <br />
                   <div class='label label-inverse'><?=$lang->getLang("Username search")?></div>
            <br />
            <input type="radio" name="searchType" class="yes" id="yes" <?=$searchType=='0'?'checked':''?> value="0" />
            <input type="radio" name="searchType" class="alternative" id="no"  <?=$searchType=='1'?'checked':''?> value="1" />
            <div class="switch grey">
                <label class="labelYes" for="yes"><?=$lang->getLang("Exactly")?></label>
                <label class="labelNo" for="no"><?=$lang->getLang("Contains")?></label>
                <span></span>
            </div>
            </div>
                </div>
                <div class="row">
                    <span class="span2"> <button class="btn btn-primary" id="SearchTicket"><?=$lang->getLang("Search")?></button></span>
                </div><br />
            </div>
<!--<table>-->
<!--<tr>-->
<!--<td>-->

<div class="">
            <?php

            if($doQuery){
            if($date_start ==''){
                addError("", $lang->getLang("Please choose a date"));
            }
            /*   if($tid==''){
                   addError("", $lang->getLang("Please enter the ticked id"));
               }*/

            if(areErrors()){
                showErrors();
            }
            else{
            $starttime = microtime(true);
            $rs=searchusertransaction();
           // var_dump($rs);
            $endtime = microtime(true);
            $duration = $endtime - $starttime;
            $duration=number_format($duration, 4, ',', '')."s";
            $time="<div class='tip fleft'>".$lang->getLang("Time taken to execute your request").":".$duration."</div><br /><br /><br />";


            $numrow=$rs->getNumRows();
            echo $time;
            $totals=[];
            if($numrow>0){
            foreach ($rs->Records as $k=>$v){
            foreach($v as $k2=>$v2){
            $totals[$k2]+=$v2;
            }
            }
     ?>
     <ul class="nav nav-tabs" id ="myTab">
        <li class="active"><a  data-toggle="tab" href="#results" >Results</a></li>
<!--        <li><a class="needToggle" href="#showparticular">Show particular</a> </li>-->
     </ul>

     <div class="tab-content">
     <div class="tab-pane active" id="results">
        <table class='display table table-striped table-bordered table-condensed'>

            <?php if($totals['tot_bet_live'] == 0 || $totals['tot_bet_prematch'] == 0){?>
            <thead>
            <tr>
                <th><?=$lang->getLang("Total bet")?></th>
                <th><?=$lang->getLang("Total win")?></th>
                <th><?=$lang->getLang("Total net")?></th>
                <th><?=$lang->getLang("Total tickets")?></th>
            </tr>
            </thead>
            <tbody>
            <tr>
               <td><?=getInDollars($totals['tot_bet_prematch']+$totals['tot_bet_live'])?></td>
                <td><?=getInDollars($totals['tot_win_prematch']+$totals['tot_win_live'])?></td>
                <td><?=getInDollars($totals['tot_bet_prematch']-$totals['tot_win_prematch']+$totals['tot_bet_live']-$totals['tot_win_live'])?></td>

                <td><?=$totals['transactions']?></td>
             </tr>
            </tbody>



            <?php }else{?>
                 <thead>

            <tr>
                <th><?=$lang->getLang("Total bet prematch")?></th>
                <th><?=$lang->getLang("Total win prematch")?></th>
                <th><?=$lang->getLang("Total net prematch")?></th>
                <th><?=$lang->getLang("Total bet live")?></th>
                <th><?=$lang->getLang("Total win live")?></th>
                <th><?=$lang->getLang("Total net live")?></th>
                <th><?=$lang->getLang("Total bet")?></th>
                <th><?=$lang->getLang("Total win")?></th>
                <th><?=$lang->getLang("Total net")?></th>
                <th><?=$lang->getLang("Total tickets")?></th>
            </tr>
            </thead>
            <tbody>
            <tr>
                <td><?=getInDollars($totals['tot_bet_prematch'])?></td>
                <td><?=getInDollars($totals['tot_win_prematch'])?></td>
                <td><?=getInDollars($totals['tot_bet_prematch']-$totals['tot_win_prematch'])?></td>
                <td><?=getInDollars($totals['tot_bet_live'])?></td>
                <td><?=getInDollars($totals['tot_win_live'])?></td>
                <td><?=getInDollars($totals['tot_bet_live']-$totals['tot_win_live'])?></td>
                <td><?=getInDollars($totals['tot_bet_prematch']+$totals['tot_bet_live'])?></td>
                <td><?=getInDollars($totals['tot_win_prematch']+$totals['tot_win_live'])?></td>
                <td><?=getInDollars($totals['tot_bet_prematch']-$totals['tot_win_prematch']+$totals['tot_bet_live']-$totals['tot_win_live'])?></td>
                <td><?=$totals['transactions']?></td>
             </tr>
            </tbody>
           <?php }; ?>
        </table>
        <br />
        <br />
<table class='display tableDetailed table table-striped table-bordered table-condensed'>
 <?php if($totals['tot_bet_live'] == 0 || $totals['tot_bet_prematch'] == 0){?>

                <thead>
                <tr>
                <th><?=$lang->getLang("Username")?></th>

                    <th><?=$lang->getLang("Total bet")?></th>
                    <th><?=$lang->getLang("Total win")?></th>
                    <th><?=$lang->getLang("Rake")?></th>

                </tr>
                </thead>
                <?php }else{?>

                <thead>
                <tr>
                    <th><?=$lang->getLang("Username")?></th>
                    <th><?=$lang->getLang("Total bet prematch")?></th>
                    <th><?=$lang->getLang("Total win prematch")?></th>
                    <th><?=$lang->getLang("Total net prematch")?></th>
                    <th><?=$lang->getLang("Total bet live")?></th>
                    <th><?=$lang->getLang("Total win live")?></th>
                    <th><?=$lang->getLang("Total net live")?></th>
                    <th><?=$lang->getLang("Total bet")?></th>
                    <th><?=$lang->getLang("Total win")?></th>
                    <th><?=$lang->getLang("Rake")?></th>
                </tr>
                </thead>

                <?php }; ?>
                <?
                while($rs->hasNext()){
                    $row=$rs->next();
                    ?>
                     <?php if($totals['tot_bet_live'] == 0 || $totals['tot_bet_prematch'] == 0){?>
                    <tr>
                        <td><span class="tabChange" style="cursor:pointer" onclick="getUserDetails('<?=$row['ctr_pun_id']?>','<?=$date_start?>','<?=$date_end?>','<?=$row['pun_username']?>')"><?=$row['pun_username']?></span> <a target="_blank" href="/betting_history_hands?username=<?=$row['pun_username']?>&uid=<?=$row['pun_customer_number']?>&from=<?=$date_start?>&to=<?=$date_end?>" class="fright aright btn btn-mini"><i class="icon-eye-open"></i></a></td>

                        <td><?=getInDollars($row["tot_bet_prematch"]+$row['tot_bet_live'],2,$row["cur_code_for_web"]);?></td>
                        <td><?=getInDollars($row["tot_win_prematch"]+$row['tot_win_live'],2,$row["cur_code_for_web"]);?></td>
                        <th><?=getInDollars($row["tot_bet_live"]+$row['tot_bet_prematch']-$row['tot_win_live']-$row['tot_win_prematch'],2,$row["cur_code_for_web"])?></th>
                    </tr>
                    <?php }else{?>
                    <tr>
                        <td><span class="tabChange" style="cursor:pointer" onclick="getUserDetails('<?=$row['ctr_pun_id']?>','<?=$date_start?>','<?=$date_end?>','<?=$row['pun_username']?>')"><?=$row['pun_username']?></span> <a target="_blank" href="/betting_history_hands?username=<?=$row['pun_username']?>&uid=<?=$row['pun_customer_number']?>&from=<?=$date_start?>&to=<?=$date_end?>" class="fright aright btn btn-mini"><i class="icon-eye-open"></i></a></td>
                        <td><?=getInDollars($row["tot_bet_prematch"],2,$row["cur_code_for_web"]);?></td>
                        <td><?=getInDollars($row["tot_win_prematch"],2,$row["cur_code_for_web"]);?></td>
                        <td><?=getInDollars($row["tot_bet_prematch"]-$row["tot_win_prematch"],2,$row["cur_code_for_web"]);?></td>
                        <td><?=getInDollars($row["tot_bet_live"],2,$row["cur_code_for_web"]);?></td>
                        <td><?=getInDollars($row["tot_win_live"],2,$row["cur_code_for_web"]);?></td>
                        <td><?=getInDollars($row["tot_bet_live"]-$row["tot_win_live"],2,$row["cur_code_for_web"]);?></td>
                        <td><?=getInDollars($row["tot_bet_prematch"]+$row['tot_bet_live'],2,$row["cur_code_for_web"]);?></td>
                        <td><?=getInDollars($row["tot_win_prematch"]+$row['tot_win_live'],2,$row["cur_code_for_web"]);?></td>
                        <th><?=getInDollars($row["tot_bet_live"]+$row['tot_bet_prematch']-$row['tot_win_live']-$row['tot_win_prematch'],2,$row["cur_code_for_web"])?></th>
                    </tr>
                    <?php };?>
                <? }
                }
                else{
                    addError("", $lang->getLang("No result found"));
                    showErrors();
                }
                }
                }
                ?>
                </td>
                </tr>
            </table>
                </div>

<!--<div class="tab-pane" id="showparticular">-->
<!--            <div id="userDetails">-->
<!--            </div></div>-->


<!--        </td>-->
<!--    </tr>-->
<!--</table>--></div>
</div>

</div>


<script>
    $(function(){
        $('#myTab a:first').tab('show');
    });



    $('body').on("click ", ".tabChange", function(e){
        e.preventDefault();
        var name = $(this).text();
        var nextTab = $('#myTab li').size()+1;

        // create the tab
  	$('<li><a href="#tab'+nextTab+'" data-toggle="tab"> '+name+'<button class="close closeTab" type="button" >×</button></a></li>').appendTo('#myTab');

  	// create the tab content
  	$('<div class="tab-pane" id="tab'+nextTab+'"><div class="userDetails'+name+'"></div></div>').appendTo('.tab-content');

  	// make the new tab active
  	$('#tabs a:last').tab('show');

        $('.needToggle').attr("data-toggle","tab");
        $('.needToggle').text(name);
        $('#myTab a:last').tab('show');
    });

      $('body').on("click ", ".closeTab", function (e) {
        e.preventDefault();
        //there are multiple elements which has .closeTab icon so close the tab whose close icon is clicked
        var tabContentId = $(this).parent().attr("href");
        $(this).parent().parent().remove(); //remove li of tab
        $('#myTab a:last').tab('show'); // Select first tab
        $(tabContentId).remove(); //remove respective tab content

    });

</script>
<?

function showChild($childArray,$onlyChild=false,$agencyCode=false,$class=false){

    foreach($childArray as $key => $value){
        if(!$class) {
            if ($value['class'] == 'casino') {
                $class_type = "casino";
            } elseif ($value['class'] == 'nation') {
                $class_type = "nation";
            } elseif ($value['class'] == 'region') {
                $class_type = "region";
            } elseif ($value['class'] == 'district') {
                $class_type = "district";
            } elseif ($value['class'] == 'club') {
                $class_type = "club";
            }
        }
        if(!is_null($value['child'])){
            if(!$onlyChild ||($onlyChild && strpos(getAllSubJurisdictions($_SESSION['jurisdiction_class']), strtolower($class_type))!==false )){ ?>
            <li data-jstree='{ "type" : "<?=$class_type?>" }' class="jstree-closed <?=$class_type?>" id="<?=$value['id']?>"><?=(($agencyCode && $class_type!='casino')? "[".$value['code']."] " :'' )?><?=strtoupper($value['name'])?><ul><?php
            }
            showChild($value['child'],$onlyChild,$agencyCode);

            if(!$onlyChild ||($onlyChild && strpos(getAllSubJurisdictions($_SESSION['jurisdiction_class']), strtolower($class_type))!==false )){ ?>
            </ul></li><?php }
        }else{
            if(!$onlyChild ||($onlyChild && strpos(getAllSubJurisdictions($_SESSION['jurisdiction_class']), strtolower($class_type))!==false ) || $class_type=='club'){ ?>
           <li data-jstree='{ "type" : "<?=$class_type?>" }' class="<?=$class_type?>" id="<?=$value['id']?>"><?=($agencyCode? "[".$value['code']."] " :'' )?><?=strtoupper($value['name'])?></li><?php }
        }
    }
}






function searchusertransaction() {
    global $dbh, $date_start,$date_end,$username,$searchType,$jurisdiction_id,$jurisdiction_class;
    $sql = "select
              ctr_transaction_num,
              ctr_pun_id,
              ctr_gam_id,
              pun_username,
              pun_customer_number,
              count(ctr_transaction_num) as transactions,
              sum(case when ctr_tty_id = 8 and ctr_gam_id=210 then ABS(ctr_amount) else 0 end ) tot_bet_prematch,
              sum(case when ctr_tty_id = 7 and ctr_gam_id=210 then ctr_amount else 0 end ) tot_win_prematch, 
              sum(case when ctr_tty_id = 8 and ctr_gam_id=213 then ABS(ctr_amount) else 0 end ) tot_bet_live,
              sum(case when ctr_tty_id = 7 and ctr_gam_id=213 then ctr_amount else 0 end ) tot_win_live,
              cur_code_for_web
            from customer_transaction, punter
            JOIN jurisdiction club ON jur_id = pun_betting_club
            JOIN currency on jur_currency=cur_id
            ";

        if($_SESSION['jurisdiction_class']=='casino'){
            $sql.=" JOIN jurisdiction district ON club.jur_parent_id = district.jur_id
                    JOIN jurisdiction region ON district.jur_parent_id = region.jur_id
                    JOIN jurisdiction nation ON region.jur_parent_id = nation.jur_id
                    WHERE nation.jur_parent_id=1 ";
        }
        if($_SESSION['jurisdiction_class']=='nation'){
            $sql.=" JOIN jurisdiction district ON club.jur_parent_id = district.jur_id
                    JOIN jurisdiction region ON district.jur_parent_id = region.jur_id
                    JOIN jurisdiction nation ON region.jur_parent_id = nation.jur_id
                    WHERE nation.jur_id=".$_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class']=='region'){
            $sql.=" JOIN jurisdiction district ON club.jur_parent_id = district.jur_id
                    JOIN jurisdiction region ON district.jur_parent_id = region.jur_id
                    WHERE region.jur_id=".$_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class']=='district'){
            $sql.=" JOIN jurisdiction district ON club.jur_parent_id = district.jur_id
                    WHERE district.jur_id=".$_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class']=='club'){
            $sql.="WHERE jur_id=".$_SESSION['jurisdiction_id'];
        }

        if($jurisdiction_class=='nation'){
        $sql.=" AND nation.jur_id=".$dbh->escape($jurisdiction_id);
        }

        if($jurisdiction_class=='region'){
        $sql.=" AND region.jur_id=".$dbh->escape($jurisdiction_id);
        }
        if($jurisdiction_class=='district'){
        $sql.=" AND district.jur_id=".$dbh->escape($jurisdiction_id);
        }
        if($jurisdiction_class=='club'){
        $sql.=" AND club.jur_id=".$dbh->escape($jurisdiction_id);
        }



         if($username!=''){
         if($searchType=='0'){
         $sql.=" AND pun_username=".$dbh->prepareString($username);
         }
         else{
         $sql.=" AND pun_username like  '%".$dbh->escape($username)."%'";

         }

         }
            $sql .=" AND ctr_time >= '$date_start 00:00:00' and ctr_time < '$date_end 23:59:59'
              and ctr_gam_id IN (213,210) 
              and ctr_pun_id = pun_id
            group by ctr_pun_id";
    $rs = $dbh->exec($sql);
    return $rs;
}

