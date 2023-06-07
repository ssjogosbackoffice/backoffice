<?php

$searchType = (isset($_POST['searchType']) ? $_POST['searchType'] : '0');
$date_start = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d", time());
$date_end = (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d");


// Datatables implementation of the current transaction in rollback
if($_GET['crtRlbkTrans']){

    //Create limit part of the sql querry
    $start = $_GET['start'];
    $length = $_GET['length'];
    $limitQuerry = "LIMIT $start , $length";

    //Create order by part of sql querry
    $columns = array('ttc_application_name','ttc_insert_time','ttc_retry', 'ttc_amount', 'ttc_transaction_id', 'pun_username', 'ttc_note', 'ptn_name', 'pes_name', 'ttc_status');
    $columnNumber =$_GET['order'][0]['column'];
    $ascOrDesc = $_GET['order'][0]['dir'];
    $colName = $columns[$columnNumber];
    $orderByQuerry = " ORDER BY " . $colName . " " .$ascOrDesc ;

    $sql =" SELECT ttc.*, pun_username, COALESCE(ptn_name, 'INTERNAL') ptn_name, COALESCE(pes_name, ' - ') pes_name
            FROM punter
            JOIN transaction_to_recover ttc ON pun_id = ttc_pun_id
            LEFT JOIN partners ON ttc_ptn_id = ptn_id 
            LEFT JOIN providers ON ttc_pes_id = pes_id ";

    $totalsSql = "SELECT count(*) from ( $sql) x";
    $sql .= " $orderByQuerry $limitQuerry";

    $result = $dbh->exec($sql, 0);

    $totals = $dbh->queryOne($totalsSql);
    $tableArr = array();
    while ($result->hasNext()){
        $row = $result->next();
        $section = array();
        $section[0] = $row['ttc_application_name'];
        $section[1] = $row['ttc_insert_time'];
        $section[2] = $row['ttc_retry'];
        $section[3] = $row['ttc_amount'];
        $section[4] = $row['ttc_transaction_id'];
        $section[5] = $row['pun_username'];
        $section[6] = $row['ttc_note'];
        $section[7] = $row['ptn_name'];
        $section[8] = $row['pes_name'];
        $status = $row['ttc_status'];
        if ($status == "0") {
            $section[9] = "<span class='text-error'>Open</span>";
        }else {
            $section[9] = "<span class='text-success'>Managed</span>";
        }

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

// Datatables implementation of the rolledback transactions
if($_GET['listRlbkTrans']){

    $startDate = $_GET['dateStart'];
    $endDate = $_GET['endDate'];
    $startDate .= " 00:00:00";
    $endDate .= " 23:59:59";

    $partnerId = $_GET['partner'];
    $providerId = $_GET['provider'];

    $userDataSearch = $_GET['username'];
    $srchType = $_GET['searchUsrType'];
    $searchUsrData = $_GET['searchUsrDataType'];

    //Create limit part of the sql querry
    $start = $_GET['start'];
    $length = $_GET['length'];
    $limitQuerry = "LIMIT $start , $length";

    //Create order by part of sql querry
    $columns = array('ttr_application_name','ttr_insert_time','ttr_managing_time', 'ttr_retry', 'ttr_amount', 'ttr_transaction_id', 'pun_username', 'ttr_note', 'ptn_name', 'pes_name');
    $columnNumber =$_GET['order'][0]['column'];
    $ascOrDesc = $_GET['order'][0]['dir'];
    $colName = $columns[$columnNumber];
    $orderByQuerry = " ORDER BY " . $colName . " " .$ascOrDesc ;

    $sql = "SELECT ttr.*, pun_username, pun_email, pun_id,
            COALESCE(ptn_name, 'INTERNAL') ptn_name, COALESCE(pes_name, ' - ') pes_name
            FROM punter
            JOIN transaction_recovered ttr ON ttr_insert_time BETWEEN '$startDate' AND '$endDate' AND pun_id = ttr_pun_id ";
    if ($userDataSearch != "" && $searchUsrData != "none") {
        if($searchUsrData == "username"){
            $perc =  ($srchType == '1' ? "%" : "");
            $sql .= " AND pun_username " . ($srchType == '0' ? " = " : " like ") . "'" . $perc . $userDataSearch . $perc . "'";
        } else if ($searchUsrData == "email") {
            $perc =  ($srchType == '1' ? "%" : "");
            $sql .= " AND pun_email " . ($srchType == '0' ? " = " : " like ") . "'" . $perc . $userDataSearch . $perc . "'";
        } else if ($searchUsrData == "usrID") {
            $perc =  ($srchType == '1' ? "%" : "");
            $sql .= " AND pun_id " . ($srchType == '0' ? " = " : " like ") . "'" . $perc . $userDataSearch . $perc . "'";
        }
    }
    $partnerBodyQuery = " JOIN partners ON ttr_ptn_id = ptn_id ";
    $providerBodyQuery = " JOIN providers ON ttr_pes_id = pes_id";

    if($partnerId != '0' && $providerId =='0') {

        $sql .= "$partnerBodyQuery AND ptn_id = $partnerId  LEFT $providerBodyQuery ";
        $totalsSql = "SELECT count(*) From( $sql) t ";
        $sql .=  "$orderByQuerry $limitQuerry" ;

    } else if ($partnerId == '0' && $providerId != '0' ) {
        $sql .= " $providerBodyQuery AND pes_id = $providerId LEFT $partnerBodyQuery ";
        $totalsSql = "SELECT count(*) From( $sql) t ";
        $sql .=  "$orderByQuerry $limitQuerry" ;

    } else if ($partnerId != '0' && $providerId != '0' ) {
        $sql .= "$partnerBodyQuery AND ptn_id = $partnerId $providerBodyQuery AND pes_id = $providerId";
        $totalsSql = "SELECT count(*) From( $sql) t ";
        $sql .=  "$orderByQuerry $limitQuerry" ;
    } else {
        $sql .= "LEFT $partnerBodyQuery LEFT $providerBodyQuery";
        $totalsSql = "SELECT count(*) From( $sql) t ";
        $sql .=  "$orderByQuerry $limitQuerry" ;
    }

    $result = $dbh->exec($sql, 0);
    //var_dump($result);

    //Totals part of sql querry
    $totals = $dbh->queryOne($totalsSql);

    $tableArr = array();

    while ($result->hasNext()){
        $row = $result->next();
        $section = array();
        $section[0] = $row['ttr_application_name'];
        $section[1] = $row['ttr_insert_time'];
        $section[2] = $row['ttr_managing_time'];
        $section[3] = $row['ttr_retry'];
        $section[4] = $row['ttr_amount'];
        $section[5] = $row['ttr_transaction_id'];

        $userEmail = $row['pun_email'];
        $userID = $row['pun_id'];
        $userName = $row['pun_username'];

        $section[6] = "<span>$userName</span><br/><span>$userEmail</span><br/><span>$userID</span>";
        $section[7] = $row['ttr_note'];
        $section[8] = $row['ptn_name'];
        $section[9] = $row['pes_name'];
        array_push($tableArr, $section);
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
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
<script src="../media/jquery.dataTables.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>

<script>

    $(function() {

        $( "#date_start" ).datepicker({
            changeMonth: true,
            numberOfMonths:1
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


        // Initializing the datatable
        rolledbackTable = $('#rolledbackTable').DataTable({
            "processing": true,
            "serverSide": true,
            "searching": false,
            "sPaginationType": "full_numbers",
            "ajax": {
                "url": "transactions/rolledback_transactions.inc",
                "data": function (d) {
                    d.listRlbkTrans = "1";
                    d.dateStart = $("#date_start").val();
                    d.endDate = $("#date_end").val();
                    d.partner = $("#partnerSelect").val();
                    d.provider = $("#providerSelect").val();
                    d.username = $("#userName").val();
                    d.searchUsrType =  $('input[name=searchType]:checked').val();
                    d.searchUsrDataType = $('div.srchSwitch.active').attr("id");
                }
            }
        });

        // Initializing the datatable
        crtRlbkTrans = $('#crtRlbkTrans').DataTable({
            "processing": true,
            "serverSide": true,
            "searching": false,
            "sPaginationType": "full_numbers",
            "ajax": {
                "url": "transactions/rolledback_transactions.inc",
                "data": function (d) {
                    d.crtRlbkTrans = "1";
                }
            }
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
    });

    // Search rolledback transactions
    $(document).on('click','#searchRlbkTrns', function() {
        var test = $('div.srchSwitch.active').attr("id");
        var searchBoxText = $("#userName").val();
        var errorText = $("#showErr");

        if( test == "username"){
            if(searchBoxText == "") {
                errorText.html("Please enter a user name");
            }
            else {
                errorText.empty();
                rolledbackTable.ajax.reload();
            }
        } else if( test == "email") {
            if(searchBoxText == "") {
                errorText.html("Please enter an email");
            }
            else {
                errorText.empty();
                rolledbackTable.ajax.reload();
            }
        } else if( test == "usrID") {
            if(searchBoxText == "") {
                errorText.html("Please enter a user ID");
            }
            else {
                errorText.empty();
                rolledbackTable.ajax.reload();
            }
        } else {
            errorText.empty();
            rolledbackTable.ajax.reload();
        }
    });

</script>
<style>
    .switch label {
        color: #33393c;
    }
</style>
<?php

?>

<table class="table table-bordered" style="width: calc(100vw - 300px);">
    <tr>
        <td class="navbar-inner" colspan="2"  ><h3 class="text-center"><?=$lang->getLang("Rolledback Transactions")?></h3></td>
    </tr>

    <tr>
        <td>
            <ul class="nav nav-tabs">
                <li class="active"><a data-toggle="tab" href="#home"><button class="btn btn-inverse">Transaction recovered</button></a></li>
                <li><a data-toggle="tab" href="#menu1"><button class="btn btn-inverse">Current transaction in rollback</button></a></li>
            </ul>

            <div class="tab-content">

                <div id="home" class="tab-pane fade in active">
                    <h3 style="text-align: center">Transaction recovered</h3>
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
                                    </div>
                                </td>
                            </tr>
                            </tbody>
                        </table>
                    </div>

                    <div class="span6">
                        <table>
                            <tbody>
                            <tr>
                                <td><div class='label label-inverse'><?=$lang->getLang("Partner")?></div></td>
                                <td>
                                    <div class="controls">
                                        <div class="input-prepend">
                                            <span class="add-on"><i class="icon-list"></i></span>
                                            <select  id="partnerSelect">
                                                <option value='0'><?=$lang->getLang("All")?></option>
                                                <?php
                                                $sql = "SELECT PTN_ID, PTN_NAME from partners";
                                                $ptnNameObj = $dbh->doCachedQuery($sql, 0);
                                                while ($ptnNameObj->hasNext()){
                                                    $row = $ptnNameObj->next();
                                                    $ptn_id = $row['ptn_id'];
                                                    $ptn_name = $row['ptn_name'];
                                                    ?>
                                                    <option value='<?php echo $ptn_id ?>'><?=$lang->getLang($ptn_name)?></option>
                                                    <?php
                                                }
                                                ?>
                                        </div>
                                    </div>
                                </td>
                            </tr>
                            <tr>
                                <td><div class='label label-inverse'><?=$lang->getLang("Providers")?></div></td>
                                <td>
                                    <div class="controls">
                                        <div class="input-prepend">
                                            <span class="add-on"><i class="icon-list"></i></span>
                                            <select  id="providerSelect" style="min-width: 158px">
                                                <option value='0'><?=$lang->getLang("All")?></option>
                                                <?php
                                                $sql = "SELECT pes_id, pes_name from providers";
                                                $ptnNameObj = $dbh->doCachedQuery($sql, 0);
                                                while ($ptnNameObj->hasNext()){
                                                    $row = $ptnNameObj->next();
                                                    $ptn_id = $row['pes_id'];
                                                    $ptn_name = $row['pes_name'];
                                                    ?>
                                                    <option value='<?php echo $ptn_id ?>'><?=$lang->getLang($ptn_name)?></option>
                                                    <?php
                                                }
                                                ?>
                                        </div>
                                    </div>
                                </td>
                            </tr>
                            <tr>
                                <td><div class='label label-inverse'><?=$lang->getLang("User Info")?></div></td>
                                <td>
                                    <div class="controls">

                                        <div id="tab" class="btn-group" data-toggle="buttons-radio">
                                            <a href="#none" class="btn active" data-toggle="tab">None</a>
                                            <a href="#username" class="btn" data-toggle="tab">Username</a>
                                            <a href="#email" class="btn" data-toggle="tab">Email</a>
                                            <a href="#usrID" class="btn"  data-toggle="tab">ID</a>
                                        </div>

                                        <div class="tab-content" id="searchSwitch">
                                            <div class="tab-pane srchSwitch active" id="none">Search by user disabled</div>
                                            <div class="tab-pane srchSwitch" id="username">Search by username</div>
                                            <div class="tab-pane srchSwitch" id="email">Search by email</div>
                                            <div class="tab-pane srchSwitch" id="usrID">Search by user ID </div>
                                        </div>

                                        <div class="input-prepend">
                                            <span class="add-on"><i class="icon-list"></i></span>
                                            <input type='text' id="userName" placeholder="Search user info" id="">
                                            <p class="error" id="showErr"></p>
                                        </div>
                                    </div>
                                </td>
                            </tr>
                            <tr>
                                <td><div class='label label-inverse'><?=$lang->getLang("User info search")?></div></td>
                                <td>
                                    <input type="radio" name="searchType" class="yes" id="yes" <?=$searchType=='0'?'checked':''?> value="0" />
                                    <input type="radio" name="searchType" class="alternative" id="no"  <?=$searchType=='1'?'checked':''?> value="1" />
                                    <div class="switch btn-primary" >
                                        <label class="labelYes" for="yes"><?=$lang->getLang("Exactly")?></label>
                                        <label class="labelNo" for="no"><?=$lang->getLang("Contains")?></label>
                                    </div>
                                </td>
                            </tr>
                            <tr>
                                <td>

                                </td>
                                <td>
                                    <div style="text-align: center">
                                        <button id="searchRlbkTrns" class="btn btn-primary" id=""><?=$lang->getLang("Search")?></button>
                                    </div>
                                </td>
                            </tr>
                            </tbody>
                        </table>
                        <br/>
                        <br/>
                        <br/>
                    </div>

                    <table id="rolledbackTable" class='display table table-striped table-hover table-bordered table-condensed no-footer' style="width: 100%">
                        <thead>
                        <tr>
                            <th><?=$lang->getLang("Application name")?></th>
                            <th><?=$lang->getLang("Time")?></th>
                            <th><?=$lang->getLang("Resolved time")?></th>
                            <th><?=$lang->getLang("Retry")?></th>
                            <th><?=$lang->getLang("Amount")?></th>
                            <th><?=$lang->getLang("Tranzaction id")?></th>
                            <th><?=$lang->getLang("User")?></th>
                            <th><?=$lang->getLang("Note")?></th>
                            <th><?=$lang->getLang("Partner name")?></th>
                            <th><?=$lang->getLang("Provider name")?></th>
                        </tr>
                        </thead>
                    </table>
                </div>

                <div id="menu1" class="tab-pane fade">
                    <table class="table table-bordered" style="width: calc(100vw - 300px);">
                        <tr>
                            <td class="navbar-inner" colspan="2"  ><h3 class="text-center"><?=$lang->getLang("Current transaction in rollback")?></h3></td>
                        </tr>
                        <tr>
                            <table id="crtRlbkTrans" class='display table table-striped table-hover table-bordered table-condensed no-footer' style="width: 100%">
                                <thead>
                                <tr>
                                    <th><?=$lang->getLang("Application name")?></th>
                                    <th><?=$lang->getLang("Time")?></th>
                                    <th><?=$lang->getLang("Retry")?></th>
                                    <th><?=$lang->getLang("Amount")?></th>
                                    <th><?=$lang->getLang("Tranzaction id")?></th>
                                    <th><?=$lang->getLang("User")?></th>
                                    <th><?=$lang->getLang("Note")?></th>
                                    <th><?=$lang->getLang("Partner name")?></th>
                                    <th><?=$lang->getLang("Provider name")?></th>
                                    <th><?=$lang->getLang("Status")?></th>
                                </tr>
                                </thead>
                            </table>
                        </tr>
                    </table>
                </div>
            </div>
        </td>
    </tr>
</table>
