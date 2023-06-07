<div class="modal" id="myModal" style="display: none;margin-top:15%">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-body centered" >
                Loading... <img src="media/images/ajax-loader.gif">
            </div>
        </div>
    </div>
</div>
<?php
$date_end= (isset($_POST['date_end'])) ? $_POST['date_end'] : '';
$day_end= (isset($_POST['day_end'])) ? $_POST['day_end'] : '';
?>

<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap-responsive.css" title="core_style" />
<style>

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
</style>
<script src="media/jquery.alerts.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<script type="text/javascript">
    $("body").on({
        ajaxStart: function() {
            $('#myModal').modal('show');
        },
        ajaxStop: function() {
            $('#myModal').modal('hide');
        }
    });
    $(function() {
        <? if($day_end!=''){
        ?>
        $("#monthdetails").show();
        <?
        }?>
        $( "#date_end" ).datepicker({
            changeMonth: true,
            changeYear: true,
            showButtonPanel: true,
            dateFormat: 'yy-mm',
            onClose: function(dateText, inst) {
                var month = $("#ui-datepicker-div .ui-datepicker-month :selected").val();
                var year = $("#ui-datepicker-div .ui-datepicker-year :selected").val();
                var currentDate = new Date();
                var currentMonth=currentDate.getMonth();
                var currentYear=currentDate.getFullYear();
                $(this).datepicker('setDate', new Date(year, month, 1));
                if(year==currentYear && month==currentMonth){
                   $("#monthdetails").show();
                }
                else{
                    $("#monthdetails").hide();
                }
            }
        });

        $( "#day_end" ).datepicker({
            changeMonth: true,
            changeYear: true,
            minDate: $('#ds').val(),
            maxDate:-1
        });
        $("#date_end").focus(function () {
            $(".ui-datepicker-calendar").hide();
            $("#ui-datepicker-div").position({
                my: "center top",
                at: "center bottom",
                of: $(this)
            });
        });
    });
    function duration(duration,divId)
    {
        $('#'+divId).empty().append('<?=$lang->getLang("Time took to execute your request")?>: '+duration);
    }

</script>
<table class="table">
    <tr><td class='navbar-inner'><h3 class="text-center"><?=$lang->getLang('Monthly Report')?></h3></td></tr>
    <tr><td class='navbar-inner'>
            <?
            /**
             * @author Marian
             */
            form_head()?>
            <?php printHiddenInput('doQuery', 'yes');?>
            <?php printHiddenInput('search_type', 'date_range');?>
            <div class='well'>
                <div class='row'>
                    <div class='span4'><div class='label label-inverse'><?=$lang->getLang("Report Type")?></div> <br>
                        <div>
                            <label class="radio">
                                <input class='span' type='radio' name='rt' value='s' <?if($_POST['rt']=='s'){?> checked="checked"<?}?> ><?=$lang->getLang("Amount")?>
                            </label>
                            <label class="radio">
                                <input class='span' type='radio' name='rt' value='b' <?if($_POST['rt']=='b'){?> checked="checked"<?}?>><?=$lang->getLang("Bets")?>
                            </label>
                        </div>
                    </div>
                    <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Month")?></div>
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
                    <div class='span4'>
                        <div class='label label-inverse'><?=$lang->getLang("Records per page")?></div> <br>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-list-alt"></i></span>
                                <select name="rowstopage" id="rowstopage" style="width:100px;">
                                    <option value="100" <?if ($_POST['rowstopage']=='100'){?>selected="selected"<? }?> >100</option>
                                    <option value="300" <?if ($_POST['rowstopage']=='300'){?>selected="selected"<? }?>>300</option>
                                    <option value="500" <?if ($_POST['rowstopage']=='500'){?>selected="selected"<? }?>>500</option>
                                </select>
                            </div>
                        </div>
                    </div>
                    <div id='monthdetails' style="display: none" class="span3">
                    <div class="span2"><div class='label label-inverse' id='datestartlabel'><?=$lang->getLang("From")?></div> <br>
                        <span class="add-on"><i class="icon-calendar"></i>
                        <span  id='day_start'><?=date('Y-m-01');?></span></span>
                        <input type='hidden' name='day_start' id='ds' value="<?=date('Y-m-01')?>">
                    </div><br>
                    <div class="span2"><div class='label label-inverse'><?=$lang->getLang("Until")?></div>
                        <br><br>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-calendar"></i></span>
                                <input type='text'  name='day_end' id='day_end' value="<?=$day_end?>">
                            </div>
                        </div>
                    </div>
                    </div>
                </div><br>
                <div class='row'>
                    <div class='span4'>
                        <button class="btn btn-primary" ><?=$lang->getLang("View")?></button>
                    </div>
                    <?php
                    if(isset($_POST['doQuery'])){
                        ?>
                        <div class='span3'><img src='/media/images/excel.png'style="width:30px; height:30px" >
                            <?  if(file_exists("documents/".$day_end.".xls")){?>
                                <a href='documents/<?=$day_end?>.xls'><?=$lang->getLang("Download as Excel")?></a>
                            <? }
                            else{
                                ?>
                                <span>There is no report generated!</span>
                            <?
                            }
                            ?>
                        </div>
                    <?
                    }?>
                </div>
                <?php
                if(isset($_POST['doQuery'])){

                function readfile_chunked($filename,$retbytes=true) {
                    $chunksize = 1*(1024*1024); // how many bytes per chunk
                    $buffer = '';
                    $cnt =0;

                }


                    ?>
                    <div id='duration' class="tip fleft"><?=$lang->getLang("Time took to execute your request")?>: <?=$duration?></div>
                <? }
                ?>
                </form><br><br>
            </div>
            <?php
            if(isset($_POST['doQuery'])){
                $rowstopage=$_POST['rowstopage'];
                if(!isset($_POST['rt'])){
                    ?>
                    <div class='well'>
                        <h2 class="text-error"><?=$lang->getLang("Choose report type")?>!</h2>
                    </div>
                <?
                }
                else{
                    ?>
                    <div id='results'>
                        <?
                        $sql="SELECT  pun_username,
                                  pun_id,
                                  pun_first_name,
                                  pun_last_name,
                                  pun_email,
                                  cms_available_credit,
                                  cms_tot_bet,
                                  ";
                        if(date("n", strtotime($date_end)) != date("n")) {
                            $day=date("Y-m-t", strtotime($date_end));
                        }
                        else{
                            $day=$_POST['day_end'];
                        }
                               $sql.=" cur_name,
                                  cou_name,
                                  pun_dob,
                                  pun_last_logged_in
                          FROM    punter,
                                  customer_monthly_summary,
                                  country,
                                  currency
                          WHERE   cms_pun_id = pun_id
                          AND     cou_id = pun_cou_id
                          AND     cms_cur_code_id = cur_code_id
                          AND     cms_day='$day'
                          GROUP BY pun_id
                          LIMIT 0,$rowstopage";
                        $starttime = microtime(true);
                        $result=$dbh->exec($sql);
                        $sql2="SELECT COUNT( * )
                              FROM punter";
                        $tot_rows=$dbh->queryOne($sql2);
                        $endtime = microtime(true);
                        $duration = $endtime - $starttime;
                        $duration=number_format($duration, 4, ',', '')."s";
                        ?>
                        <script>duration('<?=$duration?>','duration');</script>
                        <?
                        if($result->getNumRows()<1){
                            ?>
                            <div class='well'>
                                <h2 class="text-error"><?=$lang->getLang("No result found")?>!</h2>
                            </div>
                        <?
                        }
                        else{
                        $total_amount=0;
                        ?>
                        <table class='table table-bordered table-hover' id='paginator'>
                            <thead>
                            <tr><th class='alert alert-success'><strong><?=$lang->getLang("Username")?></strong></th>
                                <th class='alert alert-success'><strong><?=$lang->getLang("User code")?></strong></th>
                                <th class='alert alert-info'><strong><?=$lang->getLang("Name")?></strong></th>
                                <th class='alert alert-success'><strong><?=$lang->getLang("Lastname")?></strong></th>
                                <th class='alert alert-info'><strong><?=$lang->getLang("Email")?></strong></th>
                                <th class='alert alert-success'><strong><?if($_POST['rt']=='s'){?><?=$lang->getLang("Amount")?><?}else{?><?=$lang->getLang("Bets")?><?}?></strong></th>
                                <th class='alert alert-info'><strong><?=$lang->getLang("Currency")?></strong></th>
                                <th class='alert alert-info'><strong><?=$lang->getLang("Country")?></strong></th>
                                <th class='alert alert-info'><strong><?=$lang->getLang("Birthday")?></strong></th>
                                <th  class='alert alert-success'><strong><?=$lang->getLang("Last Login")?></strong></th>
                                <th class='alert alert-success'><strong><?=$lang->getLang('Total bets')?></strong></th>
                            </tr>
                            </thead><tbody id='tablebody'>
                            <?
                            while($result->hasNext())
                            {
                                $row=$result->next();
                                $total_amount += $row["cms_available_credit"];
                                ?>
                                <tr>
                                    <td class="text-success"><p><?=$row['pun_username']?></p></td>
                                    <td class="text-info"><p><?=$row['pun_id']?></p></td>
                                    <td class="text-success"><p><?=$row['pun_first_name']?></p></td>
                                    <td class="text-info"><p><?=$row['pun_last_name']?></p></td>
                                    <td class="text-success"><p><?=$row['pun_email']?></p></td>
                                    <td class="text-info"><p class='text-right'><?if($_POST['rt']=='s'){?><?=getInDollars($row["cms_available_credit"])?><?}else{?><?=getInDollars($row["cms_tot_bet"])?><?}?></p></td>
                                    <td class="text-success"><p><?=$row['cur_name']?></p></td>
                                    <td class="text-info"><p><?=$row['cou_name']?></p></td>
                                    <td class="text-info"><p><?=$row['pun_dob']?></p></td>
                                    <td class="text-info"><p><?=$row['pun_last_logged_in']?></p></td>
                                    <td class="text-info"><p><?=getInDollars($row['cms_tot_bet'])?></p></td>
                                </tr>
                            <?
                            }

                            ?>
                            <tr>
                                <td class="text-success" colspan="11" style="font-weight: bold"><?=$lang->getLang("Total Player Balance");?>  <?=getInDollars($total_amount)?></td>
                            </tr>
                            </tbody>
                            <?if ($tot_rows> $rowstopage){?>
                                <div id="paginator_control" style="width: 99%;height:32px;">
                                    <link href="media/smartpaginator/smartpaginator.css" rel="stylesheet" type="text/css" />
                                    <script src="media/smartpaginator/smartpaginator.js" type="text/javascript"></script>
                                    <script>
                                        $('#paginator_control').smartpaginator({ totalrecords:<?=$tot_rows?>,
                                            recordsperpage: <?=$rowstopage?>,
                                            theme: 'black',
                                            controlsalways:true,
                                            onchange: function(recordStartIndex) {
                                                $.post("services/general_services.inc",{action:"2",date_end:'<?=$date_end?>',day_end:'<?=$day_end?>',rows:"<?=$rowstopage?>",start:recordStartIndex}, function(data){
                                                    $('#tablebody').empty().append( data );
                                                });
                                            }
                                        });
                                    </script>
                                </div>
                            <?
                            }
                            }?>
                    </div>
                <?
                }
            }
            ?>
        </td></tr></table>

