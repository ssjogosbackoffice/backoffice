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
$day_start= (isset($_POST['day_start'])) ? $_POST['day_start'] :  date("Y-m-d", time()-3600*24);
$date_end= (isset($_POST['date_end'])) ? $_POST['date_end'] : '';
$day_end= (isset($_POST['day_end'])) ? $_POST['day_end'] :  date("Y-m-d", time());
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
        $( "#day_start" ).datepicker({
            changeMonth: true,
            changeYear: true,
            maxDate:-1
        });

        $( "#day_end" ).datepicker({
            changeMonth: true,
            changeYear: true,
            minDate: $('#ds').val(),
            maxDate:-1
        });
    });
    function duration(duration,divId)
    {
        $('#'+divId).empty().append('<?=$lang->getLang("Time took to execute your request")?>: '+duration);
    }

</script>
<table class="table">
    <tr><td class='navbar-inner'><h3 class="text-center"><?=$lang->getLang('Monthly Played')?></h3></td></tr>
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
                                <input class='span' type='radio' name='rt' value='played' checked="checked"><?=$lang->getLang("Played")?>
                            </label>
                        </div>
                    </div>
                    <div class="span3"><div class='label label-inverse'><?=$lang->getLang("From")?></div>
                        <br>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-calendar"></i></span>
                                <input type='text'  name='day_start' id='day_start' value="<?=$day_start?>">
                            </div>
                        </div>
                    </div><br>
                </div><br>
                <div class='row'>
                    <div class='span4'>
                        <div class='label label-inverse'><?=$lang->getLang("Records per page")?></div> <br>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-list-alt"></i></span>
                                <select name="rowstopage" id="rowstopage" style="width:100px;">
                                    <option value="20" <?if ($_POST['rowstopage']=='20'){?>selected="selected"<? }?> >100</option>
                                    <option value="300" <?if ($_POST['rowstopage']=='300'){?>selected="selected"<? }?>>300</option>
                                    <option value="500" <?if ($_POST['rowstopage']=='500'){?>selected="selected"<? }?>>500</option>
                                </select>
                            </div>
                        </div>
                    </div>
                    <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Until")?></div>
                        <br>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-calendar"></i></span>
                                <input type='text'  name='day_end' id='day_end' value="<?=$day_end?>">
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
                        <div id='excelContainer' class='span3'><img src='/media/images/excel.png'style="width:30px; height:30px" >
                                <a href="documents/Games played from <?=$day_start?> until <?=$day_end?>.xlsx"><?=$lang->getLang("Download as Excel")?></a>
                        </div>
                    <?
                    }?>
                </div>
                <?php
                if(isset($_POST['doQuery'])){?>
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

                     $sql="
                      select pun_username username,
                             ctr_time date,
                             ABS(ctr_amount) bet_amount,
                             ctr_notes note from
                             customer_transaction, punter
                             where ctr_time BETWEEN '$day_start 00:00:00' AND '$day_end 23:59:59'
                             and ctr_pun_id = pun_id
                             and ctr_tty_id = 8
                             and ctr_gam_id = 210
                             and ctr_pme_code = 'CSH'
                             order by ctr_time asc
                             LIMIT 0,$rowstopage ";
                        var_dump($sql);
                        $sql2="
                             select count(*) from
                             customer_transaction, punter
                             where ctr_time BETWEEN '$day_start 00:00:00' AND '$day_end 23:59:59'
                             and ctr_pun_id = pun_id
                             and ctr_tty_id = 8
                             and ctr_gam_id = 210
                             and ctr_pme_code = 'CSH'
                             order by ctr_time asc ";
                        $starttime = microtime(true);
                        $result=$dbh->exec($sql);
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
                                <script type="text/javascript">$('#excelContainer').hide();</script>
                            </div>
                        <?
                        }
                        else{
                        require_once 'PHPExcel.php';
                        require_once 'PHPExcel/Writer/Excel2007.php';
                        require_once 'PHPExcel/Style/Fill.php';
                        require_once 'PHPExcel/Style/Color.php';
                        $rowId = 1;
                        $objPHPExcel = new PHPExcel();
                        $objPHPExcel->getProperties()->setCreator("CsLiveGames");
                        $objPHPExcel->getProperties()->setLastModifiedBy("CsLiveGames");
                        $objPHPExcel->getProperties()->setTitle("Monthly Played for " . $day);
                        $objPHPExcel->setActiveSheetIndex(0);
                        $objPHPExcel->getActiveSheet()->SetCellValue("A".$rowId, "Username:");
                        $objPHPExcel->getActiveSheet()->SetCellValue("B".$rowId, "Date");
                        $objPHPExcel->getActiveSheet()->SetCellValue("C".$rowId, "Bet Amount");
                        $objPHPExcel->getActiveSheet()->getStyle("A".$rowId.":D".$rowId)->getFont()->getColor()->setARGB(PHPExcel_Style_Color::COLOR_WHITE);
                        $objPHPExcel->getActiveSheet()->getStyle("A".$rowId.":D".$rowId)->getFill()->setFillType(PHPExcel_Style_Fill::FILL_SOLID)->getStartColor()->setARGB('FF0000DD');
                        $objPHPExcel->getActiveSheet()->SetCellValue("D".$rowId, "Note");
                        ?>
                        <table class='table table-bordered table-hover' id='paginator'>
                            <thead>
                            <tr><th class='alert alert-success'><strong><?=$lang->getLang("Username")?></strong></th>
                                <th class='alert alert-success'><strong><?=$lang->getLang("Date")?></strong></th>
                                <th class='alert alert-info'><strong><?=$lang->getLang("Bet Amount")?></strong></th>
                                <th class='alert alert-success'><strong><?=$lang->getLang('Note')?></strong></th>
                            </tr>
                            </thead><tbody id='tablebody'>
                            <?
                            $totalbet=0;
                            while($result->hasNext())
                            {
                                $row=$result->next();
                                $rowId++;
                                $objPHPExcel->getActiveSheet()->getColumnDimension('A')->setAutoSize(true);
                                $objPHPExcel->getActiveSheet()->getColumnDimension('B')->setAutoSize(true);
                                $objPHPExcel->getActiveSheet()->getColumnDimension('C')->setAutoSize(true);
                                $objPHPExcel->getActiveSheet()->getColumnDimension('D')->setAutoSize(true);

                                $objPHPExcel->getActiveSheet()->SetCellValue("A".$rowId, $row['username']);
                                $objPHPExcel->getActiveSheet()->SetCellValue("B".$rowId, $row["date"]);
                                $objPHPExcel->getActiveSheet()->SetCellValue("C".$rowId, $row["bet_amount"]);
                                $objPHPExcel->getActiveSheet()->SetCellValue("D".$rowId, $row["note"]);
                                $totalbet +=$row['bet_amount'];
                                ?>
                                <tr>
                                    <td class="text-success"><p><?=$row['username']?></p></td>
                                    <td class="text-info"><p><?=$row['date']?></p></td>
                                    <td class="text-success"><p><?=getInDollars($row['bet_amount'])?></p></td>
                                    <td class="text-info"><p><?=$row['note']?></p></td>
                                </tr>
                            <?
                            }
                            $rowId++;
                            $objPHPExcel->getActiveSheet()->SetCellValue("A".$rowId, 'Total Bet');
                            $objPHPExcel->getActiveSheet()->SetCellValue("C".$rowId, $totalbet);
                            $rowId++;
                            $objPHPExcel->getActiveSheet()->SetCellValue("A".$rowId, 'Total LGA Tax');
                            $objPHPExcel->getActiveSheet()->SetCellValue("C".$rowId, $totalbet*0.005);

                            $objWriter = new PHPExcel_Writer_Excel2007($objPHPExcel);
                            $objWriter->save("documents/Games played from $day_start until $day_end.xlsx");
                            ?>
                            <tr><td colspan="4" style="font-weight: bold"><?=$lang->getLang('Total bet')?>: <?=getInDollars($totalbet);?><br />
                                                      <?=$lang->getLang('Total LGA Tax')?>: <?=getInDollars($totalbet*0.005)?>
                            </td>
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
                                                $.post("services/general_services.inc",{action:"3",day_start:'<?=$day_start?>',day_end:'<?=$day_end?>',rows:"<?=$rowstopage?>",start:recordStartIndex}, function(data){
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
        </td>
    </tr>
</table>

