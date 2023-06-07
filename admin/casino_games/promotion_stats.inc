<?php check_access('casino_view_promotion_reports', true);?>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<script src="/media/jquery.dataTables.js" type="text/javascript"></script>
<script>
    $(function(){
        $(".display").dataTable({'sPaginationType':"full_numbers","sDom": 'T<"clear">lfrtip',"oTableTools": {
            "aButtons": [
                "copy",
                "csv",
                "xls",
                "pdf"
            ]
        }
        });
        $('.viewActivePromotionUsers,.viewInactivePromotionUsers').click(function(){
            var tr=$(this).closest('tr');
            var active=($(this).hasClass('viewActivePromotionUsers') ? 1 : 2 );
            var promotion=$.trim($(tr).find('td:eq(0)').text());
            var year=$.trim($(tr).find('td:eq(1)').text());
            $.getJSON('/services/general_services.inc',
                {
                    "action":10,
                    "promotion":promotion,
                    "year":year,
                    "active":active
                },
                function(data){
                    var table="<table class='display table UsersTableDetails'>" +
                        "<thead>" +
                            "<tr>" +
                                "<th class='unwrappable'>User /<br />Club</th>"+
                                "<th>Time <br />used</th>"+
                                "<th class='unwrappable'>Credit /<br />Bonus</th>"+
                                "<th>Total <br /> bonus</th>"+
                                "<th class='unwrappable'>Bonus left /<br />Bonus real</th>"+
                                "<th class='unwrappable'>Total bet /<br />Total win</th>"+
                            "</tr>" +
                        "</thead>";

                    $.each( data, function( key, val ) {
                      table+="<tr>" +
                          "<td><a href='/manage_bonus/"+val[0]+"'>"+val[0]+"</a><br /><span class='tip'>("+val[1]+")</span></td>"+
                          "<td>"+val[2]+"</td>"+
                          "<td class='unwrappable'>Credit: "+val[8]+"<br />Bonus: "+val[9]+"</td>"+
                          "<td>"+val[3]+"</td>"+
                          "<td class='unwrappable'>Left: "+val[4]+"<br />Real: "+val[5]+"</td>"+
                          "<td class='unwrappable'>Bet: "+val[6]+"<br />Win: "+val[7]+"</td>"+
                          "</tr>";
                    });
                    table+="</table>";

                    $('#usersPromotionContainer').empty().append(table);
                    $(".UsersTableDetails").dataTable({'sPaginationType':"full_numbers","sDom": 'T<"clear">lfrtip',"oTableTools": {
                        "aButtons": [
                            "copy",
                            "csv",
                            "xls",
                            "pdf"
                        ]
                    }
                    });
                });

        });
    });

</script>

<?php
/**
 *
 * @author Marian
 */
$doQuery = $_POST['doQuery'];
$row_to_page = (isset($_POST['resforpage'])) ? $_POST['resforpage'] : 20;
$year = date("Y", time());
$defaultYear=(isset($_POST["year"])?$_POST['year']:$year);
$status = (isset($_POST["status"])) ? $_POST["status"] : 0 ;
?>
<table class="table table-bordered">
    <tr>
        <td class="navbar-inner" ><h3 class="text-center"><?=$lang->getLang("Promotions status")?></h3></td>
    </tr>
    <tr>
        <td>

            <table>
                <tr>
                    <td  style="border-left: 0">
                        <div class="well">
                            <form action="/promotion_stats" method="post">
                                <?php printHiddenInput('doQuery', 'yes');?>
                            <div class="row">
                                <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Year")?></div>
                                    <br>
                                    <div class="controls">
                                        <div class="input-prepend">
                                            <span class="add-on"><i class="icon-random"></i></span>
                                            <select name='year'>
                                                <?for($i=$year-5; $i<$year+5; $i++){ ?>
                                                    <option value='<?=$i?>' <?=($defaultYear==$i) ? "selected='selected'":""?> ><?=$i?></option>
                                                <? } ?>
                                            </select>
                                        </div>
                                    </div>
                                </div>
                                <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Status")?></div>
                                    <br>
                                    <div class="controls">
                                        <div class="input-prepend">
                                            <span class="add-on"><i class="icon-random"></i></span>
                                            <select name='status'>
                                                <option value='0' <?=($status=='0') ? "selected='selected'":""?> ><?=$lang->getLang("All")?></option>
                                                <option value='1' <?=($status=='1') ? "selected='selected'":""?>><?=$lang->getLang("Active")?></option>
                                                <option value='2' <?=($status=='2') ? "selected='selected'":""?>><?=$lang->getLang("Inactive")?></option>
                                            </select>
                                        </div>
                                    </div>
                                </div>
                            </div>
                            <div class="row">
                                <span class="span2"> <button class="btn btn-primary" id="SearchTicket"><?=$lang->getLang("Search")?></button></span>
                            </div><br />
                                </form>
                        </div>

                        <?php
                        if($doQuery){
                        if(areErrors()){
                            showErrors();
                        }
                        else{
                        $sql=" select prm_id, prm_description, prm_start_date, prm_end_date, prm_status, prm_op_code, prm_ptn_id, prm_pes_code_list,  active_promotion, closed_promotion, tot_bonus, won, tot_bet, tot_win
                            FROM
                            (select ppr_prm_id prom_id, sum(if (ppr_status <> 2, 1, 0)) active_promotion, sum(if (ppr_status = 2, 1, 0)) closed_promotion, sum(ppr_total_bonus) tot_bonus, sum(ppr_bonus_converted_in_real) won, sum(ppr_tot_bet) tot_bet, sum(ppr_tot_win) tot_win
                                from punter_promotion
                                group by ppr_prm_id
                            ) t RIGHT JOIN promotion ON prm_id = prom_id
                        WHERE  prm_start_date <= '".$dbh->escape($_POST['year'])."-12-31' AND prm_end_date >= '".$dbh->escape($_POST['year'])."-01-01'";
                        if($status!=0){
                            $sql .= " AND prm_status=".$dbh->escape($status);
                        }


                        $starttime = microtime(true);
                        $rs=$dbh->exec($sql);
                        $endtime = microtime(true);
                        $duration = $endtime - $starttime;
                        $duration=number_format($duration, 4, ',', '')."s";
                        $time="<div class='tip fleft'>".$lang->getLang("Time taken to execute your request").":".$duration."</div><br /><br /><br />";
                        $numrow=$rs->getNumRows();
                        echo $time;
                        if($numrow>0)
                        {
                        ?>
                        <table class='display  table table-striped table-bordered table-condensed'>
                            <thead>
                            <tr><th class="hidden">Id</th>
                                <th class="unwrappable"><?=$lang->getLang("Start date")?></th>
                                <th class="unwrappable"><?=$lang->getLang("End date")?></th>
                                <th><?=$lang->getLang("Status")?></th>
                                <th><?=$lang->getLang("Description")?></th>
                                <th class="unwrappable"><?=$lang->getLang("Op code")?></th>
                                <th><?=$lang->getLang("Users with active bonus")?></th>
                                <th><?=$lang->getLang("Users with inactive bonus")?></th>
                                <th><?=$lang->getLang("Total bonus")?></th>
                                <th><?=$lang->getLang("Won")?></th>
                                <th><?=$lang->getLang("Total bet")?></th>
                                <th><?=$lang->getLang("Total win")?></th>
                            </tr>
                            </thead>
                            <?
                            while($rs->hasNext()){
                                $row=$rs->next();
                                ?>
                                <tr>
                                    <td class="hidden"><?=$row['prm_id']?></td>
                                    <td><?=$row['prm_start_date'];?></td>
                                    <td><?=$row['prm_end_date'];?></td>
                                    <td><?if($row["prm_status"]=='2'){?>
                                            <span class="error uppercased"> <?=$lang->getLang('Inactive')?></span>
                                        <? }else{?>
                                            <span class="result uppercased"> <?=$lang->getLang('Active')?></span>
                                        <?}?>
                                    </td>
                                    <td><?=$row['prm_description'];?></td>
                                    <td><?=$row['prm_op_code'];?></td>
                                    <td><button class="viewActivePromotionUsers btn btn-small "><?=(is_null($row['active_promotion'])? 0 : $row['active_promotion'] );?></button></td>
                                    <td><button class="viewInactivePromotionUsers  btn  btn-small "><?=(is_null($row['closed_promotion'])?0:$row['closed_promotion']);?></button></td>
                                    <td><?=getInDollars($row['tot_bonus']);?></td>
                                    <td><?=getInDollars($row['won']);?></td>
                                    <td><?=getInDollars($row['tot_bet']);?></td>
                                    <td><?=getInDollars($row['tot_win']);?></td>
                                </tr>
                            <? }
                            }else{
                                addError("", $lang->getLang("No result found"));
                                showErrors();
                            }
                            }
                            }
                            ?>
                        </table>

                    </td>

                    <td>
                        <div id="usersPromotionContainer">

                        </div>
                    </td>
                </tr>
            </table>
        </td>
    </tr>
</table>