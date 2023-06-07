<?php
check_access('view_admin_currencies', true);

if(isset($_SESSION['admin_id']) || $_SESSION['admin_id'] == ''){
   // die('??????????');
    //Edits a Currency Rate
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

//Lists the currency conversion rates in datatable format
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
        $totals = $dbh->queryOne($totalsSql);
        $tableArr = array();

        while ($currenciesByDateObj->hasNext()){
            $row = $currenciesByDateObj->next();
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

//Lists the currencies in datatable format
    if(isset($_GET['viewCurrencyList'])){

        //Create limit part of the sql querry
        $start = $_GET['start'];
        $length = $_GET['length'];
        $limitQuerry = "LIMIT $start , $length";

        //Create order by part of sql querry
        $columns = array('cur_name','cur_code','cur_code_id', 'cur_code_for_web', 'cur_code_for_agency', 'cur_static_change');
        $columnNumber =$_GET['order'][0]['column'];
        $ascOrDesc = $_GET['order'][0]['dir'];
        $colName = $columns[$columnNumber];
        $orderByQuerry = " ORDER BY " . $colName . " " .$ascOrDesc ;

        $sql = "SELECT * FROM currency $orderByQuerry $limitQuerry";
        $totalsSql = "SELECT count(*) FROM currency";


        // Search by part of sql querry
        if($_GET['search']['value'] != "") {
            $searchableColumn1 = "cur_name";
            $searchableColumn2 = "cur_code";
            $searchableColumn3 = "cur_code_id";
            $searchKeyword = $dbh->prepareString($_GET['search']['value']);
            $searchKeyword = trim($searchKeyword, "'");
            $sqlLikeStmnt = " WHERE ($searchableColumn1 LIKE '%$searchKeyword%' or $searchableColumn2 LIKE '%$searchKeyword%' or $searchableColumn3 LIKE '%$searchKeyword%')";

            $sql = "SELECT * FROM currency $sqlLikeStmnt $orderByQuerry $limitQuerry";
            $totalsSql = "SELECT count(*) FROM currency $sqlLikeStmnt";
        }

        $totals = $dbh->queryOne($totalsSql);
        $response =  $dbh->doCachedQuery($sql, 0);
        $tableArr = array();

        while ($response->hasNext()) {
            $row = $response->next();
            $section = array();
            $id = $row['cur_id'];
            $currencyName = $row['cur_name'];
            $section[0] = "<a class='curName' data-id='$id'>$currencyName</a>";
            $section[1] = $row['cur_code'];
            $section[2] = $row['cur_code_id'];
            $currencySymbol = $row['cur_code_for_web'] ;
            if ($currencySymbol != ""){
                $codeForWeb = htmlentities($currencySymbol);

                $section[3] = "<span class='curCodeForWeb' data-id='$id'> $currencySymbol / $codeForWeb</span>";
            }else{
                $section[3] = "<span class='curCodeForWeb' data-id='$id'></span>";
            }

            $curCodeForAgency = $row['cur_code_for_agency'];
            $section[4] = "<span class='curCodeForAgency' data-id='$id'>$curCodeForAgency</span>";
            $curStaticChange = $row['cur_static_change'];
            $section[5] = "<span class='curStaticChange' data-id='$id'>$curStaticChange</span>";
            $section[6] = "<button class='btn deleteCurrency'  data-id='$id'><i class='icon-trash'> </i></button>";
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

    if($_POST['addNewCurrency'] == 1) {

        $currencyName = $_POST['curName'];
        $currencyCode = $_POST['curCode'];
        $currencyCodeId = $_POST['curCodeId'];
        $currencyCodeForWeb = $_POST['curCodeForWeb'];
        $currencyCodeForAgency = $_POST['curCodeForAgency'];
        $currencyStaticChange = $_POST['curStaticChange'];



        //Check to see if an entry with same currency code already exists
        $sql = "SELECT * FROM currency where cur_code = '$currencyCode'";
        $response =  $dbh->doCachedQuery($sql, 0);
        if($response->NumRows != 0){
            $recordExists = "2";
            die($recordExists);
        }
        else {
            // if currency Static Change is empty, set it to null before sending to DB
            if ($currencyStaticChange == "") {
                $sqlInsertCurrency = "INSERT INTO currency (CUR_NAME, CUR_CODE, CUR_CODE_ID, CUR_CODE_FOR_WEB, CUR_CODE_FOR_AGENCY) 
                                  VALUES ('$currencyName', '$currencyCode', '$currencyCodeId', '$currencyCodeForWeb', '$currencyCodeForAgency')";
            }
            else {
                $sqlInsertCurrency = "INSERT INTO currency (CUR_NAME, CUR_CODE, CUR_CODE_ID, CUR_CODE_FOR_WEB, CUR_CODE_FOR_AGENCY, CUR_STATIC_CHANGE) 
                                  VALUES ('$currencyName', '$currencyCode', '$currencyCodeId', '$currencyCodeForWeb', '$currencyCodeForAgency', '$currencyStaticChange')";
            }

            $newCurrency = $dbh->exec($sqlInsertCurrency);
            $newCurrency = "" .$newCurrency;
            die($newCurrency);
        }
    }

    if($_POST['pk'] == 'updateCurName'){

        $newCurName = $_POST['value'];
        $id = $_POST['curId'];

        $updateCurNameSQL = "UPDATE currency SET cur_name = '$newCurName' where cur_id = $id";
        $updateCurName = $dbh->exec($updateCurNameSQL);
        $updateCurName = "" .$updateCurName;
        die($updateCurName);
    }

    if($_POST['pk'] == 'updateCurCodeForWeb'){

        $newCurCodeForWeb = $_POST['value'];
        $id = $_POST['curId'];

        $updateCurCodeForWebSQL = "UPDATE currency SET cur_code_for_web = '$newCurCodeForWeb' where cur_id = $id";
        $updateCurCodeForWeb = $dbh->exec($updateCurCodeForWebSQL);
        $updateCurCodeForWeb = "" .$updateCurCodeForWeb;
        die($updateCurCodeForWeb);
    }

    if($_POST['pk'] == 'updateCurCodeForAgency'){

        $newCurCodeForAgency = $_POST['value'];
        $id = $_POST['curId'];

        $updateCurCodeForAgencySQL = "UPDATE currency SET cur_code_for_agency = '$newCurCodeForAgency' where cur_id = $id";
        $updateCurCodeForAgency = $dbh->exec($updateCurCodeForAgencySQL);
        $updateCurCodeForAgency = "" .$updateCurCodeForAgency;
        die($updateCurCodeForAgency);
    }

    if($_POST['pk'] == 'updateCurStaticChange'){

        $newCurStaticChange = $_POST['value'];
        $id = $_POST['curId'];

        if($newCurStaticChange == '') {
            $updateCurStaticChangeSQL = "UPDATE currency SET cur_static_change = NULL where cur_id = $id";
        }
        else {
            $updateCurStaticChangeSQL = "UPDATE currency SET cur_static_change = '$newCurStaticChange' where cur_id = $id";
        }

        $updateCurStaticChange = $dbh->exec($updateCurStaticChangeSQL);
        $updateCurStaticChange = "" .$updateCurStaticChange;
        die($updateCurStaticChange);
    }

    if($_POST['deleteCurrency'] == 'yes'){

        $currencyId = $_POST['deleteCurrencyId'];

        $sql = "DELETE FROM currency where cur_id = '$currencyId'";

        $response = $dbh->exec($sql);
        $response = "".$response;
        die($response);
    }
} else {
    die("<div class='alert alert-error'>Please login again!</div>");
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
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/bootstrap-editable/bootstrap-editable/css/bootstrap-editable.css" >
<script src="<?= secure_host ?>/media/bootstrap/bootstrap-editable/bootstrap-editable/js/bootstrap-editable.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/js/moment.js" type="text/javascript"></script>

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


        try {
            // datatable initialization and processing of the currency conversion rate table
            var resultsTable = $('#partnersTableContainer').DataTable({
                "processing": true,
                "serverSide": true,
                "ajax": {
                    "url": "configuration/currenciesManagement.inc",
                    "data": function(d) {
                        d.startDate = $("#startDate").val();
                        d.endDate = $("#endDate").val();
                        d.currency= $("#currencyType").val();
                    },
                    "error": function(xhr, status, error) {
                        console.log(xhr);
                        console.log(status);
                        console.log(error);
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

            });
        }
        catch(err) {
            jAlert('<?=$lang->getLang("An error has occured. Please try again later")?>');
        }


        // datatable initialization and processing of the currency list table
         currenciesListTable = $('#currenciesList').DataTable({
            "processing": true,
            "serverSide": true,
            "ajax": {
                "url": "configuration/currenciesManagement.inc",
                "data": function(d) {
                    d.viewCurrencyList ="1"
                    }
            },
            'sPaginationType':"full_numbers",
            "ordering": true,

            "fnDrawCallback": function (nRow, aData, iDisplayIndex, iDisplayIndexFull) {

                 $('.curName').editable({
                     placement: 'right',
                     url:'/configuration/currenciesManagement.inc',
                     send:'always',
                     pk:'updateCurName',
                     params: function(params) {
                         params.curId = $(this).attr('data-id');
                         //console.log(params);
                         return params;
                     },
                     success: function(response, newValue) {
                         //Prolly a swall of some sorts
                     },
                     error: function(response, newValue) {
                         jAlert('An error has occured');
                     }
                 });

                 $('.curCodeForWeb').editable({
                     placement: 'top',
                     url:'/configuration/currenciesManagement.inc',
                     send:'always',
                     pk:'updateCurCodeForWeb',
                     params: function(params) {
                         params.curId = $(this).attr('data-id');
                         //console.log(params);
                         return params;
                     },
                     success: function(response, newValue) {
                         //Prolly a swall of some sorts
                     },
                     error: function(response, newValue) {
                         jAlert('An error has occured');
                     }
                 });

                 $('.curCodeForAgency').editable({
                     placement: 'top',
                     url:'/configuration/currenciesManagement.inc',
                     send:'always',
                     pk:'updateCurCodeForAgency',
                     params: function(params) {
                         params.curId = $(this).attr('data-id');
                         //console.log(params);
                         return params;
                     },
                     success: function(response, newValue) {
                         //Prolly a swall of some sorts
                     },
                     error: function(response, newValue) {
                         jAlert('An error has occured');
                     }
                 });

                 $('.curStaticChange').editable({
                     placement: 'left',
                     url:'/configuration/currenciesManagement.inc',
                     send:'always',
                     pk:'updateCurStaticChange',
//                     validate: function(value) {
//                         if ($.isNumeric(value) == '') {
//                             return "Only numbers are allowed";
//                         }
//                     },
                     params: function(params) {
                         params.curId = $(this).attr('data-id');
                         //console.log(params);
                         return params;
                     },
                     success: function(response, newValue) {
                         //Prolly a swall of some sorts
                     },
                     error: function(response, newValue) {
                         jAlert('An error has occured');
                     }
                 });
             }

        });

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

            $.post("configuration/currenciesManagement.inc", {
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

    //Sets input fields to empty values
    $(document).on('click',"#addCurBtn", function() {
        $("#curName").val("");
        $("#curCode").val("");
        $("#curCodeId").val("");
        $("#curCodeForWeb").val("");
        $("#curCodeForAgency").val("");
        $("#curStaticChange").val("");
    });

    $(document).on('click',"#addNewCurrency", function() {
        var currencyName = $('#curName').val();
        var currencyCode = $('#curCode').val();
        var currencyCodeId = $('#curCodeId').val();

        var uprCurrencyCode = currencyCode.toUpperCase();
        var curCodeLength = uprCurrencyCode.length;

        //console.log(!isNaN(currencyCodeId));
        var myVar = currencyCodeId < 0 && currencyCodeId > 10000;
        var first = currencyCodeId < 0;
        var second = currencyCodeId > 10000;
        console.log(myVar);
        console.log("First " + first);
        console.log("Second " + second);


        if(currencyName == "") {
            swal({
                title: "Currency Name Error",
                text: "You must enter a currency name",
                type: "error"
            });
        } else if (currencyCode == "") {
            swal({
                title: "Currency Code Error",
                text: "You must enter a currency Code",
                type: "error"
            });
        } else if (currencyCodeId == ""){
            swal({
                title: "Currency Code ID Error",
                text: "You must enter a currency Code Id",
                type: "error"
            });
        } else if(curCodeLength != 3){
            swal({
                title: "Currency Code Error",
                text: "You must enter a valid currency code",
                type: "error"
            });
        } else if(currencyCodeId < 0){
            swal({
                title: "Currency Code ID Error",
                text: "Currency must be a number between 0 and 10000",
                type: "error"
            });
        } else if (currencyCodeId > 10000) {
            swal({
                title: "Currency Code ID Error",
                text: "Currency must be a number between 0 and 10000",
                type: "error"
            });
        } else {
            // makes a request to the server with the new fields
            $.post("configuration/currenciesManagement.inc", {
                    addNewCurrency: "1",
                    curName: currencyName,
                    curCode : uprCurrencyCode,
                    curCodeId: currencyCodeId,
                    curCodeForWeb: $('#curCodeForAgency').val(),
                    curCodeForAgency: $('#curCodeForWeb').val(),
                    curStaticChange: $('#curStaticChange').val()
            },
                function(data){
                    if (data == "1") {
                        swal({
                            title: "Record Updated",
                            text: "The record has been successfully updated",
                            type: "success"
                        });
                        currenciesListTable.ajax.reload();
                    } else if (data == "2"){
                        swal({
                            title: "Record Already Exists",
                            text: "You already have a currency with the same code",
                            type: "error"
                        });
                    }
                });
        }

    });

    $(document).on('click',".deleteCurrency", function() {

        var id = $(this).attr('data-id');
        //console.log(id);

        swal({
            title: "Are you sure?",
            text: "The record will be deleted",
            type: 'warning',
            showCancelButton: true,

        },
            function() {
                $.post("configuration/currenciesManagement.inc", {
                    deleteCurrency: "yes",
                    deleteCurrencyId: id
                }, function(data) {
                    if (data == "1") {
                        swal({
                            title: "Record Deleted",
                            text: "The record was succesfully deleted",
                            type: "success"
                        });
                        currenciesListTable.ajax.reload();
                    } else {
                        swal({
                            title: "Record was not deleted",
                            text: "The record was not deleted",
                            type: "error"
                        });
                    }
                })
            }
        )

    });

</script>

<?php
function preprint($obj){
    print "<pre>". print_r($obj, true) ."</pre>";
}
//preprint($_SESSION);

?>
<!-- Edit currencies conversion rate Modal Start  -->
<div class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true" id="edit">
    <div class="modal-header"> <!-- Modal header -->
        <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
        <h3 class="text-info text-center"><?=$lang->getLang("Edit Conversion Rate")?></h3> <br />
    </div>
    <div class="modal-body"> <!-- Modal body -->

        <table class='table table-condensed '>
            <tr>
                <td class='blackTd'><?=$lang->getLang("Date")?></td>
                <td><input type="text" id="date" value='' disabled></td>
            </tr>
            <tr>
                <td class='blackTd'><?=$lang->getLang("Currency From")?></td>
                <td><input type="text" id="curMaster" value='' disabled></td>
            </tr>
            <tr >
                <td class='blackTd'><?=$lang->getLang("Currency To")?></td>
                <td><input type="text" id="curSecondary" value='' disabled></td>
            </tr>
            <tr>
                <td class='blackTd'><?=$lang->getLang("Conversion Rate")?></td>
                <td><input type="number"  step="0.0001" id="convRate" value='' ></td>
            </tr>
            <tr>
                <td class='blackTd'><?=$lang->getLang("Last Update")?></td>
                <td><input type="text" id="lastUpdate" value='' disabled></td>
            </tr>
        </table>
    </div>
    <div class="modal-footer"><!-- Modal footer -->
        <button type="button" class="btn btn-default" data-dismiss="modal">Close</button>
        <button class="btn btn-primary" id="saveConvRate" data-dismiss="modal" ><?=$lang->getLang("Save")?></button>
    </div>
</div> <!-- Edit currencies conversion rate Modal End  -->


<!-- Add new currency Modal Start  -->
<div class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true" id="addCurrency">
    <div class="modal-header"> <!-- Modal header -->
        <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
        <h3 class="text-info text-center"><?=$lang->getLang("Add a new currency")?></h3> <br />
    </div>
    <div class="modal-body"> <!-- Modal body -->

        <table class='table table-condensed '>
            <tr>
                <td class='blackTd'><?=$lang->getLang("Currency Name")?></td>
                <td><input type="text" id="curName" value=''></td>
            </tr>
            <tr>
                <td class='blackTd'><?=$lang->getLang("Currency Code")?></td>
                <td><input type="text" id="curCode" value=''></td>
            </tr>
            <tr>
                <td class='blackTd'><?=$lang->getLang("Currency Code Id")?></td>
                <td><input type="number" min="0" max="10000" step="1" id="curCodeId" value=''></td>
            </tr>
            <tr>
                <td class='blackTd'><?=$lang->getLang("Currency Code for web")?></td>
                <td><input type="text" id="curCodeForWeb" value=''></td>
            </tr>
            <tr>
                <td class='blackTd'><?=$lang->getLang("Currency Code for agency")?></td>
                <td><input type="text" id="curCodeForAgency" value='' ></td>
            </tr>
            <tr>
                <td class='blackTd'><?=$lang->getLang("Currency Static Change")?></td>
                <td><input type="number" step="0.0001" id="curStaticChange" value=''></td>
            </tr>
        </table>
    </div>

    <div class="modal-footer"><!-- Modal footer -->
        <button type="button" class="btn btn-default" data-dismiss="modal">Close</button>
        <button class="btn btn-primary" id="addNewCurrency" data-dismiss="modal" ><?=$lang->getLang("Save")?></button>
    </div>
</div> <!-- Add new currency Modal End  -->


<div class="container-fluid ">

    <ul class="nav nav-tabs" id="navTabList">
        <li ><a data-toggle="tab" href="#currencyConversion"><?=$lang->getLang("Manage Currencies Rates")?></a></li>
        <li ><a data-toggle="tab" href="#currencyManage"><?=$lang->getLang("Manage currencies")?></a></li>
    </ul>


    <div class="tab-content" id="navTabContent">
        <div id="currencyConversion" class="tab-pane fade in active">
            <table class="table table-bordered" style="padding: 20px">
                <tr>
                    <td class="navbar-inner" colspan="2"  ><h3 class="text-center"><?=$lang->getLang("Manage Currency Conversion Rates")?></h3></td>
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
                            </table>
                        </div>
                    </td>
                </tr>


            </table>
        </div>

        <div id="currencyManage" class="tab-pane fade in ">

            <table class="table table-bordered" style="padding: 20px">
                <tr>
                    <td class="navbar-inner" colspan="2"  ><h3 class="text-center"><?=$lang->getLang("Manage currencies")?></h3></td>
                </tr>
                <tr>
                    <td colspan="2"  >
                        <a class="fright btn btn-info btn-small" style="color:white" id="addCurBtn" data-toggle="modal" data-target="#addCurrency" ><?=$lang->getLang("Add new currency")?></a>
                    </td>
                </tr>
                <tr>
                    <td>
                        <table id="currenciesList" class='display table table-striped table-hover table-bordered table-condensed no-footer' style="width: 100%">
                            <thead>
                                <tr>
                                    <th><?=$lang->getLang("Currency Name")?></th>
                                    <th><?=$lang->getLang("Currency Code")?></th>
                                    <th><?=$lang->getLang("Currency Code Id")?></th>
                                    <th><?=$lang->getLang("Currency Code for web")?></th>
                                    <th><?=$lang->getLang("Currency Code for agency")?></th>
                                    <th><?=$lang->getLang("Currency Static Change")?></th>
                                    <th><?=$lang->getLang("Operations")?></th>
                                </tr>
                            </thead>
                        </table>
                    </td>
                </tr>
            </table>
        </div>

    </div>

</div>