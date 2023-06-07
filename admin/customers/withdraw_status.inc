<script src="/media/jquery.dataTables.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>

<script>
    $(function(){
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


        $('#searchBy').change(function() {
            if($(this).val()!='1'){
                $('.period').show();
                $('#valueContainer').hide();
            }
            else{
                $('.period').hide();
                $('#valueContainer').show();

            }
        }).trigger('change');

    })

</script>
<?php
$date_start = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d", time());
$date_end = (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d");
$searchValue=(isset($_REQUEST['searchValue'])?$_REQUEST['searchValue']:'');
$searchBy=(isset($_POST['searchBy'])?$_POST['searchBy']:'1');

//var_dump($sql);

?>
<table class="table table-bordered" >
    <tr>
        <td class="navbar-inner" ><h3 class="text-center"><?=$lang->getLang("Withdraw lock status")?></h3></td>
    </tr>
    <tr>
        <td>
            <div class="well">
                <form action="/?page=customers/withdraw_status" method="post">
                                <div class="span5">
                                    <div class='label label-inverse'><?=$lang->getLang("Search by")?></div>
                                    <br>
                                    <div class="controls">
                                        <div class="input-prepend">
                                            <span class="add-on"><i class="icon-user"></i></span>
                                            <select name="searchBy" id="searchBy">
                                                <option <?=($searchBy==1 ? 'selected="selected"': '')?> value="1"  ><?=$lang->getLang("Username")?></option>
                                                <option <?=($searchBy==2 ? 'selected="selected"': '')?> value="2"  ><?=$lang->getLang("Last update")?></option>
                                                <option <?=($searchBy==3 ? 'selected="selected"': '')?> value="3"  ><?=$lang->getLang("Creation Date")?></option>
                                            </select>
                                        </div>
                                    </div>
                                    <div id="valueContainer"><div class='label label-inverse' id="searchLabel"><?=$lang->getLang("Value")?></div>
                                        <br />
                                        <div class="controls">
                                            <div class="input-prepend">
                                                <span class="add-on"><i class="icon-edit"></i></span>
                                                <input type='text'  name='searchValue'  placeholder="<?=$lang->getLang('Search')?>" required="required" value="<?=$searchValue?>">
                                            </div>
                                        </div>
                                    </div>
                                </div>

                    <table  class="period">
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
                                        <input type='text' name='date_start' id='date_start' class="dateStartHelper"  value="<?=$date_start?>" >
                                    </div>
                                </div></td>
                        </tr>
                        <tr>
                            <td><div class='label label-inverse'><?=$lang->getLang("Until")?></div></td>
                            <td><div class="controls">
                                    <div class="input-prepend">
                                        <span class="add-on"><i class="icon-calendar"></i></span>
                                        <input type='text' name='date_end' id='date_end' class="dateEndHelper" value="<?=$date_end?>">
                                    </div>
                                </div></td>
                        </tr>

                        </tbody>
                    </table>

                    <div class="clearfix"></div>
                    <input type="hidden" value="searchuserhands" name="action" />
                    <button class="btn btn-primary" ><?=$lang->getLang("Search")?></button>
                    </form>
            </div>

        </td>
    </tr>
    <tr><td>
    <?
        if($searchValue!=''){
            if($searchBy==1){
                $where=" AND pun_username=".$dbh->prepareString($searchValue);
            }
            elseif($searchBy==2){
                $where=" AND ptd_last_update BETWEEN ".$dbh->prepareString($date_start." 00:00:00")."  AND ".$dbh->prepareString($date_end ." 23:59:59") ;
            }
            else{
                $where=" AND ptd_creation_date BETWEEN ".$dbh->prepareString($date_start." 00:00:00")."  AND ".$dbh->prepareString($date_end." 23:59:59");
            }
        $sql="Select * from punter_temp_deposit_and_game_info join punter on ptd_pun_id=pun_id  " .doJurisdictionCheck()." ".$where;
            $result=$dbh->exec($sql);
            if($result->getNumRows()>0){
            ?>
            <table class='display displayInfo table table-striped table-bordered table-condensed' >
                <thead>
                <tr>
                    <th><?=$lang->getLang("User")?></th>
                    <th><?=$lang->getLang("Creation date")?></th>
                    <th><?=$lang->getLang("Last update")?></th>
                    <th><?=$lang->getLang("Last deposit")?></th>
                    <th><?=$lang->getLang("Total bet")?></th>
                    <th><?=$lang->getLang("Total win")?></th>
                    <th><?=$lang->getLang("Withdrawable credit")?></th>
                </tr>
                </thead>
                <tbody>
            <?
            while($result->hasNext()) {
                $row = $result->next();
                $totDeposit= $row['ptd_tot_cur_bet']+$row['ptd_tot_cur_prov_bet'];
                $totWin= $row['ptd_tot_cur_win']+$row['ptd_tot_cur_prov_win'];
                ?>
                <tr>
                    <td><?=$row['pun_username']?></td>
                    <td><?=$row['ptd_creation_date']?></td>
                    <td><?=$row['ptd_last_update']?></td>
                    <td><?=getInDollars($row['ptd_last_current_deposit'])?></td>
                    <td><?=getInDollars($totDeposit)?></td>
                    <td><?=getInDollars($totWin)?></td>
                    <td><?=getInDollars($totWin-$row['ptd_last_current_deposit']>0? $totWin-$row['ptd_last_current_deposit'] : 0)?></td>
                </tr>
                <?
            ?>
            <?
            }
            ?>
            </tbody>
            </table>
                <script>$('.display').DataTable({'sPaginationType':"full_numbers"});</script>
            <?
            }
            else{
                ?>

                <div class='alert alert-success'><button type='button' class='close' data-dismiss='alert'>&times;</button><strong><?=$lang->getLang('No withdraw lock')?></strong></div>
                <?
            }
        }
    ?>


        </td></tr>
</table>
<br>