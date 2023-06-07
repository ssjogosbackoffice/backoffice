<?php
check_access('view_admin_currencies', true);


if (isset($_POST['currency_Master'])) {

      $updatedConvRate = $dbh->prepareString($_POST['convRate']);
      $updatedConvRate = trim($updatedConvRate, "'");
      $tableDate = $dbh->prepareString($_POST['date']);
      $tableDate = trim($tableDate, "'");
      $currencySec = $dbh->prepareString($_POST['curSecondary']);
      $currencySec = trim($currencySec, "'");

      $sql = "UPDATE currencies_conversion SET CCN_CONVERS_VALUE = '$updatedConvRate' WHERE CCN_DATE = '$tableDate' AND CCN_CUR_SECONDARY = '$currencySec'";
      $updateCurrency = $dbh->exec($sql);
      $updateCurrency = "" .$updateCurrency;

      die($updateCurrency);
  }

if(isset($_GET['startDate'])){

        $columns = array('CCN_DATE','CCN_CUR_MASTER','CCN_CUR_SECONDARY', 'CCN_CONVERS_VALUE','CCN_LAST_UPDATE');
        $columnNumber =$_GET['order'][0]['column'];
        $ascOrDesc = $_GET['order'][0]['dir'];
        $colName = "";
        $colName = $columns[$columnNumber];
        $orderBy = " ORDER BY " . $colName . " " .$ascOrDesc ;

        $searchKeyword = $dbh->prepareString($_GET['search']['value']);
        $searchKeyword = trim($searchKeyword, "'");
        $sqlLikeStmnt = "and CCN_CUR_SECONDARY like '%$searchKeyword%'";

        $selectedStartDate = $dbh->prepareString($_GET['startDate']);
        $selectedStartDate = trim($selectedStartDate, "'");
        $selectedEndDate = $dbh->prepareString($_GET['endDate']);
        $selectedEndDate = trim($selectedEndDate, "'");
        $selectedCurrency = $_GET['currency'];

        $sql = "";
        if($selectedCurrency == "") {
            $selectedEndDate = $selectedStartDate;
            $sql = "SELECT * FROM currencies_conversion WHERE CCN_DATE Between '$selectedStartDate' and '$selectedEndDate' $sqlLikeStmnt $orderBy LIMIT ".$_GET['start']. " ,".$_GET['length'];
            $totalsSql="SELECT count(*) FROM currencies_conversion WHERE CCN_DATE Between '$selectedStartDate' and '$selectedEndDate'";
        }
        else {

            $currencyCodeSQL = "SELECT CUR_CODE FROM currency WHERE CUR_NAME = '$selectedCurrency'";
            $currencyCodeObj =  $dbh->doCachedQuery($currencyCodeSQL, 0);
            $currencyCode = $currencyCodeObj->Records[0]['cur_code'];
            $sql = "SELECT * FROM currencies_conversion WHERE CCN_DATE Between '$selectedStartDate' and '$selectedEndDate' and CCN_CUR_SECONDARY = '$currencyCode' $sqlLikeStmnt $orderBy LIMIT ".$_GET['start']. " ,".$_GET['length'];
            $totalsSql="SELECT count(*) FROM currencies_conversion WHERE CCN_DATE Between '$selectedStartDate' and '$selectedEndDate' and CCN_CUR_SECONDARY = '$currencyCode'";
        }

        $currenciesByDateObj = $dbh->doCachedQuery($sql, 0);
        $totals=$dbh->queryOne($totalsSql);
        $tableArr = array();


        while ($currenciesByDateObj->hasNext()){
            $row=$currenciesByDateObj->next();
            $section = array();
            $section[0] = $row['ccn_date'];
            $section[1] = $row['ccn_cur_master'];
            $section[2] = $row['ccn_cur_secondary'];
            $section[3] = $row['ccn_convers_value'];
            $section[4] = $row['ccn_last_update'];
            $section[5] = "<button class='updateConvRate btn btn-primary' data-toggle='modal' data-target='#edit'><i class='icon-pencil'> </i>Edit</button>";
            array_push($tableArr,$section);
        }

    $output= array(
        "draw"            => intval( $_GET['draw'] ),
        "recordsTotal"    => intval($totals),
        "recordsFiltered" => intval( $totals ),
        "data"            => $tableArr
    );

    die(json_encode($output));

}


?>

<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/js/sweetalert/sweetalert.css"  />
<script src="<?= secure_host ?>/media/js/sweetalert/sweetalert.min.js" type="text/javascript"></script>
<style>
    .hidden-date{
        display: none;
    }
</style>

<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
<script src="../media/jquery.dataTables.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />

<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>

<script>
    $( document ).ready(function() {

     //Update and close modal
    $('#saveConvRate').on('click',function(){
        updateConvRate();
    });

        var startDatepicker = $('#startDate');
        startDatepicker.datepicker({
            dateFormat: 'yy-mm-dd',
            onClose: function() {
                if($('#currencyType').val() == "") {
                    $('#endDate').val($('#startDate').val());
                }
            }
        });

        var endDatepicker = $('#endDate');
        endDatepicker.datepicker();

        // set the datepickers to current date
        startDatepicker.datepicker('setDate', new Date());
        endDatepicker.datepicker('setDate', new Date());

        // shows and hides the second datepicker upon currency select
        $('#currencyType').change(function(){
            if($(this).val() == ""){
                $('.hidden-date').css({"display":"none"});
                $('#endDate').val($('#startDate').val());
            }
            else{
                $('.hidden-date').css({"display":"block"});
            }
        });

       // datatable initialization and processing
       var resultsTable = $('#partnersTableContainer').DataTable( {
            "processing": true,
            "serverSide": true,
            "ajax": {
                "url": "configuration/currencyConverter.inc",
                "data": function(d) {
                    d.startDate = $("#startDate").val();
                    d.endDate = $("#endDate").val();
                    d.currency= $("#currencyType").val();
                }
            },
            'sPaginationType':"full_numbers",
           "language": {
               "emptyTable": function() {
                   if ($('#startDate').val() ==  $('#endDate').val()) {
                       return "No data available in table on " + $('#startDate').val()
                   }
                   return "No data available in table between " + $('#startDate').val() + " and " + $('#endDate').val()
               }
           },
           "ordering": true

        } );

        $('#viewButton').on('click',function () {

            var start = $('#startDate').datepicker('getDate');
            var end   = $('#endDate').datepicker('getDate');
            var days   = (end - start)/1000/60/60/24;

            if(days > 61) {
                swal({
                    title: "2 Months Allowed Error",
                    text: "Maximum 2 months allowed",
                    type: "error"
                });
                
            } else if (days < 0) {
                swal({
                    title: "Invalid end date",
                    text: "The start date has to be lower than the end date.",
                    type: "error"
                });
                
            }
            else {
                resultsTable.ajax.reload();
            }
       });

        function updateConvRate(){

            $.post("configuration/currencyConverter.inc", {
                    date:$('#date').val(),
                    currency_Master:$("#curMaster").val(),
                    curSecondary:$("#curSecondary").val(),
                    convRate:$("#convRate").val(),
                },
                function(data) {
                    if (data == 1 ) {
                        swal({
                            title: "Record Updated",
                            text: "The conversion rate has been successfully updated",
                            type: "success"
                        });
                        resultsTable.ajax.reload();
                    }
                    else {
                        swal({
                            title: "Error",
                            text: "Data was not updated",
                            type: "error"
                        });
                    }
                }
            );
        }

    });

    $(document).on('click',".updateConvRate", function() {
        var row = $(this).closest("tr");
        $("#date").val($.trim($(row).find('td:eq(0)').text()));
        $("#curMaster").val($.trim($(row).find('td:eq(1)').text()));
        $("#curSecondary").val($.trim($(row).find('td:eq(2)').text()));
        $("#convRate").val($.trim($(row).find('td:eq(3)').text()));
        $("#lastUpdate").val($.trim($(row).find('td:eq(4)').text()));
    });

</script>


<!-- Modal -->
<div id="edit" class="modal fade" role="dialog">
    <div class="modal-dialog">

        <!-- Modal content-->
        <div class="modal-content">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal">&times;</button>
                <h3 class="modal-title">Edit Conversion Rate</h3>
            </div>
            <!-- form data here -->
            <div class="modal-body">
                <table class="table table-striped" style='margin: auto; text-align: left'>
                    <tr>
                        <td><?=$lang->getLang("Date")?></td>
                        <td><input type="text" id="date" value='' disabled></td>
                    </tr>
                    <tr>
                        <td><?=$lang->getLang("Currency From")?></td>
                        <td><input type="text" id="curMaster" value='' disabled></td>
                    </tr>
                    <tr>
                        <td><?=$lang->getLang("Currency To")?></td>
                        <td><input type="text" id="curSecondary" value='' disabled></td>
                    </tr>
                    <tr>
                        <td><?=$lang->getLang("Conversion Rate")?></td>
                        <td><input type="number"  step="0.0001" id="convRate" value='' ></td>
                    </tr>
                    <tr>
                        <td><?=$lang->getLang("Last Update")?></td>
                        <td><input type="text" id="lastUpdate" value='' disabled></td>
                    </tr>
                </table>
            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-default" data-dismiss="modal">Close</button>
                <button class="btn btn-primary" id="saveConvRate" data-dismiss="modal" ><?=$lang->getLang("Save")?></button>
            </div>
        </div>

    </div>
</div>


<table class="table table-bordered" style="padding: 20px">
    <tr>
        <td class="navbar-inner" colspan="2"  ><h3 class="text-center"><?=$lang->getLang("View Currency Rates")?></h3></td>
    </tr>
    <tr>
        <td colspan="2">
            <div class="well">
                <form>
                    <div class="row">
                        <br>
                        <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Choose a start date")?></div>
                            <br>
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-calendar"></i></span> <input type="text" id="startDate" class="datepicker startDate" name="startDate"/>
                                </div>
                            </div>
                        </div>
                        <div class="span4 hidden-date">
                            <div class='label label-inverse'><?=$lang->getLang("Choose an end date")?></div>
                            <br>
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-calendar"></i></span> <input type="text" id="endDate" class="datepicker endDate" name="endDate"/>
                                </div>
                            </div>
                        </div>
                        <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Select a currency")?></div>
                            <br>
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-random"></i></span>
                                    <select name='currencySelect' id="currencyType">
                                        <option value=''><?=$lang->getLang("All")?></option>
                                        <?php
                                        $currencyQuery = "SELECT * FROM currency";
                                        $allCurrenciesFromDB = $dbh->doCachedQuery($currencyQuery, 0);
                                        $currencyNameList = [];
                                        foreach($allCurrenciesFromDB as $key => $value)  {

                                            foreach($value as $key2 => $value2 ) {
                                                if(!($value2['cur_name'] == "Euro") && !($value2['cur_name'] == "EURO"))
                                                array_push($currencyNameList, $value2['cur_name']);
                                            }
                                        }

                                            foreach($currencyNameList as $key => $value) {
                                        ?>
                                                <option value='<?php echo $value ?>'><?=$lang->getLang($value)?></option>
                                        <?php
                                            }
                                        ?>
                                    </select>
                                </div>

                        </div>
                        <div class="span3" style="text-align: center; padding-top: 18px">
                            <button type="button" class="btn btn-primary" id="viewButton"><?=$lang->getLang("Search")?></button>
                        </div>
                    </div>
                </form>
            </div>
        </td>
    </tr>
    <tr>
        <td>

            <div style="width: 100%">

                <table id="partnersTableContainer" class='display table table-striped table-hover table-bordered table-condensed' style="width: 90%">
                    <thead>
                    <tr>
                        <th><?=$lang->getLang("Date")?></th>
                        <th><?=$lang->getLang("Currency From")?></th>
                        <th><?=$lang->getLang("Currency To")?></th>
                        <th><?=$lang->getLang("Conversion rate")?></th>
                        <th><?=$lang->getLang("Last Update")?></th>
                        <th><?=$lang->getLang("Action")?></th>
                    </tr>
                    </thead>
                    <tbody>

                    </tbody>
                </table>
            </div>
        </td>

    </tr>
</table>