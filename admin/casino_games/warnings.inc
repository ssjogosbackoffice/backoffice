<?php check_access('customers_dangerous',$redirect=true);
/**
 * @author Marian
 */
define("WARNING_RAKE", 400);
define("WARNING_MIN_WINS",  1000);
define("HISTORY_DAYS",      90);
$date_start= (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d",time()-3600*24);
$date_end= (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d");
$period = (isset($_POST["period"])) ? $_POST["period"] : 0 ;
$rake = (isset($_POST["rake"])) ? $_POST["rake"] : WARNING_RAKE ;
$gt = (isset($_POST["gt"])) ? $_POST["gt"] : 0 ;
$action = $_GET["action"];
?>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap-responsive.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/dataTablesExtra/TableTools-2.2.0/css/dataTables.tableTools.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />

<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/jquery.dataTables.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/jstree/jstree.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/jquery.alerts.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/highcharts.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/modules/exporting.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/dataTablesExtra/TableTools-2.2.0/js/dataTables.tableTools.js" type="text/javascript"></script>
<style>
    #casino_container a,#provider_container a{
        text-decoration: none;
    }
    .short{
        min-width: 105px;
    }
    .short > .btn{
        background-image: none;
    }
    .black .btn {
        background-image: none;
    }
    .pager ul {
        min-width: 300px;
    }
    .pager div.short input {
        padding: 0px;
    }

</style>
<script type="text/javascript">
    $(function() {
        $("#date_start").datepicker({
            changeMonth: true,
            numberOfMonths: 1,
            onSelect: function (selectedDate) {
                $("#date_end").datepicker("option", "minDate", selectedDate);
                dateMax = $("#date_start").datepicker("getDate");
                var rangeMax = new Date(dateMax.getFullYear(), dateMax.getMonth() + 2, dateMax.getDate());
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
        $(".alignment .btn").click(function() {
            // whenever a button is clicked, set the hidden helper
            $("#period").val($(this).val());
            if($(this).val()==0){
                $(".specific_date").hide("fast");
            }
            else{
                $(".specific_date").show("fast");
            }
        });
    });
    function toggle(id){
        $('#'+id).toggle();
    }
    function getUserHistory(u,userid,days){
        var username=$(u)
            .closest('tr')
            .children(":eq(1)")
            .text();
        $('#myModalLabel').empty().append( '<span>Details for last '+days+' days for <span class="text-info">' + username+'</span><span>');
        $.post("services/general_services.inc",{action:"4",user:userid,days:days}, function(data){
            $('#userhistory').empty().append( data );
        });

    }
</script>
<div id="cust_info" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
    <div class="modal-header">
        <button type="button" class="close" data-dismiss="modal" aria-hidden="true">x</button>
        <h3 id="myModalLabel"></h3>
    </div>
    <div class="modal-body">
        <p id='userhistory'></p>
    </div>
    <div class="modal-footer">
        <button class="btn" data-dismiss="modal" aria-hidden="true"><?=$lang->getLang("Close")?></button>
    </div>
</div>
<table class="table table-bordered">
<tr><td class='navbar-inner'><h3 class="text-center"><?=$lang->getLang("Dangerous Players")?></h3></td></tr>
<tr>
<td>
<form action="/dangerous" method='POST'>
<?php printHiddenInput('doQuery', 'yes');?>
<div class='well'>
<div class='row'>
    <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Period")?></div>
        <br>
        <div class="btn-group alignment" data-toggle="buttons-radio">
            <button type="button" class="btn btn-primary <?=($period=='0') ? "active":""?>" value="0"><?=$lang->getLang("Today")?></button>
            <button type="button" class="btn btn-primary <?=($period=='1') ? "active":""?>" value="1"><?=$lang->getLang("Specific period")?></button>
        </div>
        <input type="hidden" id="period" name='period' value="<?=$period?>" />
    </div>
    <div class="span3 specific_date" <?=($period=='0') ? "style='display:none'":""?>><div class='label label-inverse'><?=$lang->getLang("From")?></div>
        <br>
        <div class="controls">
            <div class="input-prepend">
                <span class="add-on"><i class="icon-calendar"></i></span>
                <input type='text'  name='date_start' id='date_start' value="<?=$date_start?>">
            </div>
        </div>
    </div>
</div><br>
<div class='row'>
    <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Rake")?></div>
        <br>
        <div class="controls">
            <div class="input-prepend">
                <span class="add-on"><i class="icon-random"></i></span>
                <input type="text" name='rake' value="<?=$rake?>" >
            </div>
        </div>
    </div>
    <div class="span3 specific_date" <?=($period=='0') ? "style='display:none'":""?>><div class='label label-inverse'><?=$lang->getLang("Until")?></div>
        <br>
        <div class="controls">
            <div class="input-prepend">
                <span class="add-on"><i class="icon-calendar"></i></span>
                <input type='text'  name='date_end' id='date_end' value="<?=$date_end?>">
            </div>
        </div>
    </div>
</div><br>
<div class='row'>
    <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Game type")?></div>
        <br>
        <div class="controls">
            <div class="input-prepend">
                <span class="add-on"><i class="icon-random"></i></span>
                <select name='gt' >
                    <option <?if($gt==0)  {?>selected="selected"<?}?> value='0'><?=$lang->getLang("All")?></option>
                    <option <?if($gt==1)  {?>selected="selected"<?}?> value='1'><?=$lang->getLang("Casino Games")?></option>
                    <option <?if($gt==2)  {?>selected="selected"<?}?> value='2'><?=$lang->getLang("Provider Games")?></option>
                </select>
            </div>
        </div>
    </div>
</div>
<button class="btn btn-primary" ><?=$lang->getLang("Search")?></button>
</form><br><br>
<?php
if(isset($_POST['doQuery'])){
    ?>
    <div id='results'>
    <?
    $date_start=$_POST['date_start'];
    $date_end=$_POST['date_end'];
    $d1 = strtotime("$date_start");
    $d2 = strtotime("$date_end");
    if($d2-$d1<0){
        addError("",$lang->getLang("Please choose an end date greater than start date"));
    }
    if(!areErrors())
    {
        $today=date("Y-m-d");
        if($_POST['period']==0){
            $date_start=$today;
            $date_end=$today;
        }
        $starttime = microtime(true);
        $old="SELECT cus_res_pun_id pun_id, cus_res_gam_id gam_id, pun_username, pun_access, club.jur_id club_id , club.jur_name club,gam_name, max(cus_res_day) as max_day,cur_code_for_web, ";
        if($_SESSION['jurisdiction_class']=='casino'){
            $old.=" district.jur_id district_id, district.jur_name district,region.jur_id region_id, region.jur_name region, nation.jur_id nation_id, nation.jur_name nation, ";
        }
        if($_SESSION['jurisdiction_class']=='nation'){
            $old.=" district.jur_id district_id, district.jur_name district,region.jur_id region_id, region.jur_name region, nation.jur_id nation_id, nation.jur_name nation, ";
        }
        if($_SESSION['jurisdiction_class']=='region'){
            $old.=" district.jur_id district_id, district.jur_name district,region.jur_id region_id, region.jur_name region, ";
        }
        if($_SESSION['jurisdiction_class']=='district'){

            $old.=" district.jur_id district_id, district.jur_name district,";
        }
        $old .="pcr_credits, SUM(cus_res_bet) AS bets,
                SUM(cus_res_win) AS wins, @rake := SUM(cus_res_bet) - SUM(cus_res_win) as rake
                FROM game,customer_result_daily
                JOIN punter on cus_res_pun_id = pun_id
                JOIN punter_credit ON cus_res_pun_id = pcr_pun_id
                JOIN jurisdiction club ON jur_id = pun_betting_club
                JOIN currency on jur_currency=cur_id
                ";

        if($_SESSION['jurisdiction_class']=='casino'){
            $old.=" JOIN jurisdiction district ON club.jur_parent_id = district.jur_id
                    JOIN jurisdiction region ON district.jur_parent_id = region.jur_id
                    JOIN jurisdiction nation ON region.jur_parent_id = nation.jur_id
                    WHERE 1=1 ";
        }
        if($_SESSION['jurisdiction_class']=='nation'){
            $old.=" JOIN jurisdiction district ON club.jur_parent_id = district.jur_id
                    JOIN jurisdiction region ON district.jur_parent_id = region.jur_id
                    JOIN jurisdiction nation ON region.jur_parent_id = nation.jur_id
                    WHERE nation.jur_id=".$_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class']=='region'){
            $old.=" JOIN jurisdiction district ON club.jur_parent_id = district.jur_id
                    JOIN jurisdiction region ON district.jur_parent_id = region.jur_id
                    WHERE region.jur_id=".$_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class']=='district'){

            $old.=" JOIN jurisdiction district ON club.jur_parent_id = district.jur_id
                    WHERE district.jur_id=".$_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class']=='club'){
            $old.="WHERE jur_id=".$_SESSION['jurisdiction_id'];
        }

        $old.=" AND cus_res_day >= '$date_start' AND cus_res_day <= '$date_end'
                AND cus_res_gam_id= gam_id ";
        if($gt=='1'){
            $old.=' AND gam_id>1000 ';
        }
        elseif($gt=='2'){
            $old.=" AND gam_id<1000 ";
        }
        $old.=" GROUP BY cus_res_pun_id, cus_res_gam_id, club.jur_id";

        $todaycasino ="SELECT pre_pun_id pun_id, res_gam_id gam_id, pun_username, pun_access, club.jur_id club_id , club.jur_name club,gam_name,max(pre_time) as max_day,cur_code_for_web,";
        if($_SESSION['jurisdiction_class']=='casino'){
            $todaycasino.=" district.jur_id district_id, district.jur_name district,region.jur_id region_id, region.jur_name region, nation.jur_id nation_id, nation.jur_name nation, ";
        }
        if($_SESSION['jurisdiction_class']=='nation'){
            $todaycasino.=" district.jur_id district_id, district.jur_name district,region.jur_id region_id, region.jur_name region, nation.jur_id nation_id, nation.jur_name nation, ";
        }
        if($_SESSION['jurisdiction_class']=='region'){
            $todaycasino.=" district.jur_id district_id, district.jur_name district,region.jur_id region_id, region.jur_name region, ";
        }
        if($_SESSION['jurisdiction_class']=='district'){

            $todaycasino.=" district.jur_id district_id, district.jur_name district,";
        }
        $todaycasino .=" pcr_credits, SUM(pre_bet) AS bets,
                                SUM(pre_win) AS wins, @rake := SUM(pre_bet) - SUM(pre_win) AS rake";
        if(EXT_PARTNER_ID>0){
            $todaycasino.=" FROM ext_punter_result
                JOIN punter on pre_pun_id = pun_betting_club JOIN punter_credit ON pun_id = pcr_pun_id ";
        }

         elsE{ $todaycasino.=" FROM result, game, punter_result
                                JOIN punter on pre_pun_id = pun_id
                                JOIN punter_credit ON pre_pun_id = pcr_pun_id ";
            }
             $todaycasino.=" JOIN jurisdiction club ON jur_id = pun_betting_club
                             JOIN currency on jur_currency=cur_id ";

        if($_SESSION['jurisdiction_class']=='casino'){
            $todaycasino.=" JOIN jurisdiction district ON club.jur_parent_id = district.jur_id
                JOIN jurisdiction region ON district.jur_parent_id = region.jur_id
                        JOIN jurisdiction nation ON region.jur_parent_id = nation.jur_id
                WHERE 1=1 ";
        }
        if($_SESSION['jurisdiction_class']=='nation'){
            $todaycasino.=" JOIN jurisdiction district ON club.jur_parent_id = district.jur_id
                JOIN jurisdiction region ON district.jur_parent_id = region.jur_id
                        JOIN jurisdiction nation ON region.jur_parent_id = nation.jur_id
                        WHERE nation.jur_id=".$_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class']=='region'){
            $todaycasino.=" JOIN jurisdiction district ON club.jur_parent_id = district.jur_id
                JOIN jurisdiction region ON district.jur_parent_id = region.jur_id
                        WHERE region.jur_id=".$_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class']=='district'){

            $todaycasino.=" JOIN jurisdiction district ON club.jur_parent_id = district.jur_id
                WHERE district.jur_id=".$_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class']=='club'){
            $todaycasino.="WHERE jur_id=".$_SESSION['jurisdiction_id'];
        }


        $todaycasino.="  AND pre_time > '$today' ";
        if(EXT_PARTNER_ID==0 || !defined('EXT_PARTNER_ID')){
        $todaycasino.=" AND res_id = pre_res_id
                                AND res_gam_id=gam_id ";

        }
        if($gt=='1'){
            $todaycasino.=' AND gam_id>1000 ';
        }
        elseif($gt=='2'){
            $todaycasino.=" AND gam_id<1000 ";
        }
        $todaycasino.=" GROUP BY pre_pun_id, res_gam_id, club.jur_id";



        $todayproviders="SELECT ctr_pun_id pun_id, ctr_gam_id gam_id, pun_username, pun_access, club.jur_id club_id , club.jur_name club,gam_name,max(ctr_time) as max_day,cur_code_for_web, ";
        if($_SESSION['jurisdiction_class']=='casino'){
            $todayproviders.=" district.jur_id district_id, district.jur_name district,region.jur_id region_id, region.jur_name region, nation.jur_id nation_id, nation.jur_name nation, ";
        }
        if($_SESSION['jurisdiction_class']=='nation'){
            $todayproviders.=" district.jur_id district_id, district.jur_name district,region.jur_id region_id, region.jur_name region, nation.jur_id nation_id, nation.jur_name nation, ";
        }
        if($_SESSION['jurisdiction_class']=='region'){
            $todayproviders.=" district.jur_id district_id, district.jur_name district,region.jur_id region_id, region.jur_name region, ";
        }
        if($_SESSION['jurisdiction_class']=='district'){

            $todayproviders.=" district.jur_id district_id, district.jur_name district,";
        }
        $todayproviders .="  pcr_credits,
                                COALESCE(ABS(SUM(CASE WHEN ctr_tty_id =8 THEN ctr_amount END )), 0) bets,
                                COALESCE(SUM(CASE WHEN ctr_tty_id =7 THEN ctr_amount END ), 0) wins,
                                COALESCE(ABS(SUM(CASE WHEN ctr_tty_id =8 THEN ctr_amount END )), 0) -
                                COALESCE(SUM(CASE WHEN ctr_tty_id =7 THEN ctr_amount END ), 0) rake
                                FROM game,customer_transaction
                                JOIN punter on ctr_pun_id = pun_id
                                JOIN punter_credit ON ctr_pun_id = pcr_pun_id
                                JOIN jurisdiction club ON jur_id = pun_betting_club
                                JOIN currency on jur_currency=cur_id
                                ";

        if($_SESSION['jurisdiction_class']=='casino'){
            $todayproviders.=" JOIN jurisdiction district ON club.jur_parent_id = district.jur_id
                JOIN jurisdiction region ON district.jur_parent_id = region.jur_id
                        JOIN jurisdiction nation ON region.jur_parent_id = nation.jur_id
                WHERE 1=1 ";
        }
        if($_SESSION['jurisdiction_class']=='nation'){
            $todayproviders.=" JOIN jurisdiction district ON club.jur_parent_id = district.jur_id
                JOIN jurisdiction region ON district.jur_parent_id = region.jur_id
                        JOIN jurisdiction nation ON region.jur_parent_id = nation.jur_id
                        WHERE nation.jur_id=".$_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class']=='region'){
            $todayproviders.=" JOIN jurisdiction district ON club.jur_parent_id = district.jur_id
                JOIN jurisdiction region ON district.jur_parent_id = region.jur_id
                        WHERE region.jur_id=".$_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class']=='district'){

            $todayproviders.=" JOIN jurisdiction district ON club.jur_parent_id = district.jur_id
                WHERE district.jur_id=".$_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class']=='club'){
            $todayproviders.="WHERE jur_id=".$_SESSION['jurisdiction_id'];
        }

        $todayproviders.="  AND ctr_time > date_format(CURRENT_TIMESTAMP(),'%Y%m%d000000')
                                AND ctr_pesid > 0
                                AND ctr_tty_id in (7,8)
                                AND ctr_gam_id= gam_id ";
        if($gt=='1'){
            $todayproviders.='AND gam_id>1000 ';
        }
        elseif($gt=='2'){
            $todayproviders.="AND gam_id<1000 ";
        }
        $todayproviders .=" GROUP BY ctr_pun_id, ctr_gam_id, club.jur_id";

        if($date_start==$today){
            $sql=$todaycasino." UNION ALL ".$todayproviders;
        }
        else{
            if($date_end==$today){
                $sql=$old." UNION ALL ".$todaycasino." UNION ALL ".$todayproviders;
            }
            else{
                $sql=$old;
            }
        }
        $sql .=" ORDER BY rake  asc
                         LIMIT 0, 3000";

        $result = $dbh->exec($sql);
        $endtime = microtime(true);
        $duration = $endtime - $starttime;
        $duration=number_format($duration, 4, ',', '')."s";?>
        <div class="tip fleft"><?=$lang->getLang("Time taken to execute your request")?>: <?=$duration?></div><br><br>
        <?

        $casino="<div><div><span class='btn btn-primary centered' onclick=toggle('casino_container')>".$lang->getLang("Casino Games")."</span></div><br><div id='casino_container'> <div id='paginator_casino' style='width:100%'></div>
                                <table class='table display table-hover table-striped table-bordered' id='casino'>
                                <thead>                                ";
        $providers="<div><div><span class='btn btn-primary centered' onclick=toggle('provider_container')>".$lang->getLang("Provider Games")."</span></div><br><div id='provider_container'> <div id='paginator_providers' style='width:100%'></div>
                                <table class='table display table-hover table-striped table-bordered'  id='providers'>
                                <thead>";
        $common="<tr  class='btn-info'>
                                    <th>".$lang->getLang("Date")."</th>
                                    <th>".$lang->getLang("Username")."</th>
                                    <th>".$lang->getLang("Game")."</th>
                                    <th class='no-sort'>".$lang->getLang("Balance")."</th>
                                    <th><strong>".$lang->getLang("Club")."</strong></th>";
        if($_SESSION['jurisdiction_class']=='district'){
            $common.="<th><strong>".$lang->getLang("District")."</strong></th>";
        }
        if($_SESSION['jurisdiction_class']=='region'){
            $common.="<th><strong>".$lang->getLang("District")."</strong></th>
                              <th><strong>".$lang->getLang("Region")."</strong></th>";
        }
        if($_SESSION['jurisdiction_class']=='nation' || $_SESSION['jurisdiction_class']=='casino'){
            $common.="<th><strong>".$lang->getLang("District")."</strong></th>
                              <th><strong>".$lang->getLang("Region")."</strong></th>
                              <th><strong>".$lang->getLang("Nation")."</strong></th>";
        }
        $common.="<th class='no-sort'><strong>".$lang->getLang("Bet")."</strong></th>
                                    <th class='no-sort'><strong>".$lang->getLang("Win")."</strong></th>
                                    <th><strong>".$lang->getLang("Rake")."</strong></th>
                                    <th class='no-sort'><strong>".$lang->getLang("Action")."</strong></th>
                                </tr>
                                </thead>
                                <tbody>";
        $casino.=$common;
        $providers .=$common;
        $count_casino=0;
        $count_provider=0;
        while($result->hasNext())
        {
            $row=$result->next();
            if($row['rake']<-$rake){
                $commons="<tr><td>".$row["max_day"]."</td>
                              <td><a class='" . $row["pun_access"] . "' href='" . refFormPage("customers/customer_view&customer_id=" . $row["pun_id"] . "&header_page=") . "' target='blank'>" . $row["pun_username"] . (($row["pun_access"] == "deny")?(" (Locked)"):("")) . "</a></td>
                              <td>".$row["gam_name"]."</td>
                              <td>".getInDollars($row["pcr_credits"],2,$row["cur_code_for_web"])."</td>
                              <td>".getClassLink("/jurisdictions/?node=" . $row["club_id"], $row["club"], '','_blank')."</td>";
                if($_SESSION['jurisdiction_class']=='district'){
                    $commons.=" <td>".getClassLink("/jurisdictions/?node=" . $row["district_id"], $row["district"],'','_blank')."</td>";
                }
                if($_SESSION['jurisdiction_class']=='region'){
                    $commons.=" <td>".getClassLink("/jurisdictions/?node=" . $row["district_id"], $row["district"], '','_blank')."</td>
                                <td>".getClassLink("/jurisdictions/?node=" . $row["region_id"], $row["region"],'','_blank')."</td>";
                }
                if($_SESSION['jurisdiction_class']=='nation' || $_SESSION['jurisdiction_class']=='casino'){
                    $commons.=" <td>".getClassLink("/jurisdictions/?node=" . $row["district_id"], $row["district"],'','_blank')."</td>
                                <td>".getClassLink("/jurisdictions/?node=" . $row["region_id"], $row["region"],'','_blank')."</td>
                                <td>".getClassLink("/jurisdictions/?node=" . $row["nation_id"], $row["nation"],'','_blank' )."</td>";
                }
                $commons.="     <td>".getInDollars($row["bets"],2,$row["cur_code_for_web"])."</td>
                                <td>".getInDollars($row["wins"],2,$row["cur_code_for_web"])."</td>
                                <td>".getInDollars($row["rake"],2,$row["cur_code_for_web"])."</td>
                                <td><a href='#cust_info' style='color:white;text-decoration:none' onclick='getUserHistory(this,".$row["pun_id"].",30);' role='button' class='btn btn-success' data-toggle='modal'>".$lang->getLang('View last % days', 30 ). "</a>
                                    <a href='#cust_info' style='color:white;text-decoration:none' onclick='getUserHistory(this,".$row["pun_id"].",60);' role='button' class='btn btn-success' data-toggle='modal'>".$lang->getLang('View last % days', 60 ). "</a>
                                    <a href='#cust_info' style='color:white;text-decoration:none' onclick='getUserHistory(this,".$row["pun_id"].",90);' role='button' class='btn btn-success' data-toggle='modal'>".$lang->getLang('View last % days', 90 ). "</a>
                                </td>
                           </tr>";
                if($row['gam_id']<'1000'){
                    $providers.=$commons;
                    $count_provider++;
                }
                elseif($row['gam_id']>'1000'){
                    $casino .=$commons;
                    $count_casino++;
                }
            }
        }
        $casino.="        </tbody></table>

                        <script>
                                     $('#casino').dataTable({
                                      'aoColumnDefs' : [ {
                                       'bSortable' : false,
                                      'aTargets' : [ 'no-sort' ]
                                      } ],
                                      'sPaginationType':'full_numbers'}
                                );
                           /* $('#paginator_casino').smartpaginator({ totalrecords:$count_casino,
                                recordsperpage: 20,
                                datacontainer: 'casino',
                                dataelement: 'tr',
                                theme: 'black'});*/
                        </script>
                    </div>
                    </div>";

        $providers.="        </tbody>
                            </table>
                              <script>
                              $('#providers').dataTable({
                                 'aoColumnDefs' : [ {
                                 'bSortable' : false,
                                 'aTargets' : [ 'no-sort' ]
                               } ],
                              'sPaginationType':'full_numbers'});
                         /*   $('#paginator_providers').smartpaginator({ totalrecords:$count_provider,
                                recordsperpage: 20,
                                datacontainer: 'providers',
                                dataelement: 'tr',
                                theme: 'black'});*/
                        </script>
                    </div>
                    </div>";
        if($count_provider<1){
            $providers="<div><div><span class='btn btn-primary centered' onclick=toggle('provider_container')>".$lang->getLang("Provider Games")."</span></div><br><h3 id='provider_container' class='text-error'>".$lang->getLang("No result found")." !</h3></div></div>";
        }
        if($count_casino<1){
            $casino="<div><div><span class='btn btn-primary centered' onclick=toggle('casino_container')>".$lang->getLang("Casino Games")."</span></div><br><h3 id='casino_container' class='text-error'>".$lang->getLang("No result found")."</h3></div></div>";
        }

        if($gt=='0'){

            $html = $casino.$providers;
        }
        elseif($gt=='1'){

            $html = $casino;
        }
        else{
            $html=$providers;
        }
        ?> <?=$html?>
    <?
    }
    else{
        showErrors();
    }
    ?>
    </div>
<?
}
?>
</div>
</td>
</tr>
</table>
<?php

?>
