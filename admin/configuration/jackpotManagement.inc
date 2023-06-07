<?php

//check_access('manage_jackpot', true);
check_access('manage_jackpot', true);
require_once 'RedisClient.php';

$redisClient=new RedisClient(REDISSERVER,REDISPORT,$dbLog);

/**
 * Created by PhpStorm.
 * User: alexandra
 * Date: 13.12.2017
 * Time: 14:44
 */

//adds a table record to reddis
if(isset($_POST['jkpotId']) && isset($_POST['gameId']) && isset($_POST['gameName'])) {

    $gamId = $_POST['gameId'];
    $gamName = $_POST['gameName'];
    $date = $_POST['date'];
    $amount = $_POST['amount'];
    $maxAmount = $_POST['maxAmount'];
    $level = $_POST['level'];

    // create the key to update redis
    $keyword = "JKT-". $gamId."-".$level;

    // set data in Json format
    $data = [
        'gamId' => $gamId,
        'gamName' =>  $gamName,
        'date' => $date,
        'amount' => $amount,
        'maxAmount' => $maxAmount,
        'level' => $level
    ];
    $dataJson = json_encode($data);

    //set the redis data
    $redisResponse = $redisClient->set($keyword, $dataJson);
    //$redisData = $redisClient->get($keyword);
    //print($redisData);

    $response =$redisResponse->getPayload();
    if($response == "OK") {
        $myResponse = "". 1;
        die($myResponse);
    }
}

//adds a new record to db and to redis
if(isset($_POST['gameId']) && isset($_POST['date']) && isset($_POST['amount']) && isset($_POST['maxAmount']) && isset($_POST['level'])) {

    //Get the values from Post call and prepares them for insertion
    $updtGameName = $dbh->prepareString($_POST['addGameName']);
    $updtGameName = trim($updtGameName, "'");

    $updtGamId = $dbh->prepareString($_POST['gameId']);
    $updtGamId = trim($updtGamId, "'");

    $updtGamAmount = $dbh->prepareString($_POST['amount']);
    $updtGamAmount = trim($updtGamAmount, "'");

    $updtDate = $dbh->prepareString($_POST['date']);
    $updtDate = trim($updtDate, "'");

    $updtMaxAmount = $dbh->prepareString($_POST['maxAmount']);
    $updtMaxAmount = trim($updtMaxAmount, "'");

    $updtLevel = $dbh->prepareString($_POST['level']);
    $updtLevel = trim($updtLevel, "'");


    // create the key to update redis
    $keyword = "JKT-". $updtGamId."-".$updtLevel;

    $data = [
        'gamId' => $updtGamId,
        'gamName' =>  $updtGameName,
        'date' => $updtDate,
        'amount' => $updtGamAmount,
        'maxAmount' => $updtMaxAmount,
        'level' => $updtLevel
    ];
    $dataJson = json_encode($data);

    //set the redis data
    $redisData = $redisClient->set($keyword, $dataJson);

    // SQL insert querry
    $insertJackpotSql = "INSERT INTO jackpot (jkt_gam_id, jkt_amount, jkt_reg_date, jkt_max_amount, jkt_level) VALUES ('$updtGamId','$updtGamAmount',' $updtDate', '$updtMaxAmount', '$updtLevel')";

    // SQL insert
    $newJackpot = $dbh->exec($insertJackpotSql);
    $newJackpot = "" .$newJackpot;

    die($newJackpot);
}

// edits an existing record in db and redis
if(isset($_POST['editGameId']) && isset($_POST['editDate']) && isset($_POST['editAmount']) && isset($_POST['editMaxAmount']) && isset($_POST['editLevel'])) {

    $edtGameName = $dbh->prepareString($_POST['editGameName']);
    $edtGameName = trim($edtGameName, "'");

    $edtJkptId = $dbh->prepareString($_POST['editId']);
    $edtJkptId = trim($edtJkptId, "'");

    //Get the values from Ajax call and prepares them for update
    $edtGamId = $dbh->prepareString($_POST['editGameId']);
    $edtGamId = trim($edtGamId, "'");

    $edtGamDate = $dbh->prepareString($_POST['editDate']);
    $edtGamDate = trim($edtGamDate, "'");

    $edtAmnt = $dbh->prepareString($_POST['editAmount']);
    $edtAmnt = trim($edtAmnt, "'");

    $edtMaxAmnt = $dbh->prepareString($_POST['editMaxAmount']);
    $edtMaxAmnt = trim($edtMaxAmnt, "'");

    $edtLevel = $dbh->prepareString($_POST['editLevel']);
    $edtLevel = trim($edtLevel, "'");

    $edtOldLevel = $_POST['editOldLevel'];

    // create the key to update redis
    $keyword = "JKT-". $edtGamId."-".$edtLevel;

    $data = [
        'gamId' => $edtGamId,
        'gamName' =>  $edtGameName,
        'date' => $edtGamDate,
        'amount' => $edtAmnt,
        'maxAmount' => $edtMaxAmnt,
        'level' => $edtLevel
    ];
    $dataJson = json_encode($data);

    //delete the old data
    $deleteKeyword = "JKT-".$edtGamId."-".$edtOldLevel;
    $redisClient->delete($deleteKeyword);
    //set the redis new data
    $redisData = $redisClient->set($keyword, $dataJson);

    // SQL UPDATE QUERRY
    $editSql = "UPDATE jackpot SET jkt_gam_id = '$edtGamId', jkt_amount = '$edtAmnt', jkt_max_amount = '$edtMaxAmnt', jkt_level = '$edtLevel' WHERE jkt_id = '$edtJkptId'";
    $updatedJackpot = $dbh->exec($editSql);
    $updatedJackpot = "" .$updatedJackpot;
    die($updatedJackpot);
}

// delte a record in db and in redis
if(isset($_POST['deleteId']) && isset($_POST['deleteGamId']) && isset($_POST['deleteLevel'])) {

    $jackpotiD = $_POST['deleteId'];
    $gameID = $_POST['deleteGamId'];
    $jackpotLevel = $_POST['deleteLevel'];

    // create the key to update redis
    $keyword = "JKT-". $gameID."-".$jackpotLevel;

    // deletes from redis
    $redisClient->delete($keyword);

    // delete from db
    $sql = "DELETE FROM jackpot where jkt_id = '$jackpotiD'";

    $response = $dbh->exec($sql);
    $response = "".$response;
    die($response);
}

// lists the table in datatables format
if(isset($_GET['listJackpots'])){

    //Create limit part of the sql querry
    $start = $_GET['start'];
    $length = $_GET['length'];
    $limitQuerry = "LIMIT $start , $length";

    //Create order by part of sql querry
    $columns = array('jkt_gam_id','gam_name','jkt_reg_date', 'jkt_amount', 'jkt_max_amount','jkt_level');
    $columnNumber =$_GET['order'][0]['column'];
    $ascOrDesc = $_GET['order'][0]['dir'];
    $colName = $columns[$columnNumber];
    $orderByQuerry = " ORDER BY " . $colName . " " .$ascOrDesc ;

     //Create search part of sql querry
    if(isset($_GET['search']['value'])) {
        $searchableColumn1 = "gam_name";
        $searchableColumn2 = "jkt_gam_id";
        $searchableColumn3 = "";
        $searchKeyword = $dbh->prepareString($_GET['search']['value']);
        $searchKeyword = trim($searchKeyword, "'");
        $sqlLikeStmnt = "and ($searchableColumn1 LIKE '%$searchKeyword%' or $searchableColumn2 LIKE '%$searchKeyword%' )";

        $sql = "select j.*, gam_name from jackpot j, game where gam_id = jkt_gam_id $sqlLikeStmnt $orderByQuerry $limitQuerry";
        $totalsSql = "SELECT COUNT(*) FROM jackpot, game WHERE gam_id = jkt_gam_id $sqlLikeStmnt";
    }
    else {
        $sql = "select j.*, gam_name from jackpot j, game where gam_id = jkt_gam_id $orderByQuerry  $limitQuerry";
        $totalsSql = "SELECT COUNT(*) FROM jackpot, game WHERE gam_id = jkt_gam_id";
    }
    
    // performs the sql querry on the db
    $totals=$dbh->queryOne($totalsSql);
    $jackpotsObj = $dbh->doCachedQuery($sql, 0);
    $tableArr = array();
    while ($jackpotsObj->hasNext()){

        $row = $jackpotsObj->next();
        // check if we have data in reddis
        $reddisKeyword = "JKT-".$row['jkt_gam_id']."-".$row['jkt_level'];

        $reddisData = $redisClient->get($reddisKeyword);
        if($reddisData != NULL) {
            $result = json_decode($reddisData, true);
            $section = array();
            $btnData = $row['jkt_id'];
            $section[0] = $result['gamId'];
            $section[1] = $row['gam_name'];
            $section[2] = $result['date'];
            $section[3] = $result['amount'];
            $section[4] = $result['maxAmount'];
            $section[5] = $result['level'];
            $section[6] = "<div class='btn-group'> 
                            <button type='button' class='btn btn-small dropdown-toggle' data-toggle='dropdown'> Action <span class='caret'></span></button>
                            <ul class='dropdown-menu' style='min-width: 65px'>
                                <li>
                                    <a class='editJackpot' data-toggle='modal' data-target='#editJackpot' data-id='$btnData'><i style='padding-right: 5px;' class='icon-pencil'> </i>     Edit</a>
                                </li>
                                <li>
                                    <a class='deleteJackpot' data-id='$btnData'><i style='padding-right: 5px;' class='icon-trash'> </i>Delete</a>
                                </li>
                            </ul>
                          </div>";
            array_push($tableArr,$section);
        }
        else {
            $section = array();
            $btnData = $row['jkt_id'];
            $section[0] = $row['jkt_gam_id'];
            $section[1] = $row['gam_name'];
            $section[2] = $row['jkt_reg_date'];
            $section[3] = $row['jkt_amount'];
            $section[4] = $row['jkt_max_amount'];
            $section[5] = $row['jkt_level'];
            $section[6] = "<div class='btn-group'> 
                            <button type='button' class='btn btn-small btn-warning dropdown-toggle' data-toggle='dropdown'> Action <span class='caret'></span></button>
                            <ul class='dropdown-menu' style='min-width: 65px'>
                                <li>
                                    <a class='editJackpot' data-toggle='modal' data-target='#editJackpot' data-id='$btnData'><i style='padding-right: 5px;' class='icon-pencil'> </i>     Edit</a>
                                </li>
                                <li>
                                    <a class='deleteJackpot' data-id='$btnData'><i style='padding-right: 5px;' class='icon-trash'> </i>Delete</a>
                                </li>
                                <li>
                                    <a class='addItemToCache' data-id='$btnData'>Add to cache </a>
                                </li>

                            </ul>
                          </div>";
            array_push($tableArr,$section);
        }

    }
    $output= array(
        "draw"            => intval($_GET['draw']),
        "recordsTotal"    => intval($totals),
        "recordsFiltered" => intval($totals),
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
    $( document ).ready(function() {

        // Initializing the datatable
        jackpotTable = $('#jackpotTable').DataTable({
            "processing": true,
            "serverSide": true,
            "ajax": {
                "url": "configuration/jackpotManagement.inc",
                "data": function (d) {
                    d.listJackpots = "1";
                }
            },
            'sPaginationType': "full_numbers",
            "columnDefs": [
                { "orderable": false, "targets": 6 }
            ]
        });

        // set the timestamp on current date
        var myDate = new Date();
        var now = (myDate.getFullYear() + '-' +(myDate.getMonth() + 1) + '-' +  myDate.getDate() + ' '+myDate.getHours()+ ':'+myDate.getMinutes()+ ':'+myDate.getSeconds());
        $('#date').attr('value', now);

        //adds a new jackpot
        $('#addJackpot').on('click',function(){
            addJackpot();
        });

        //edit a jackpot
        $('#editJackpotData').on('click', function() {
            editJackpot();
        });

        // Edit an existing jackpot
        function editJackpot( ) {

            // make sure all inputs are correct
            if($("#editAmount").val() == ""){
                swal({
                    title: "Edit Record error",
                    text: "You must input an amount",
                    type: "error"
                });
            } else if ($("#editMaxAmount").val() == ""){
                swal({
                    title: "Edit Record error",
                    text: "You must input a max amount",
                    type: "error"
                });
            } else {
                $.post("configuration/jackpotManagement.inc", {
                        editId: id,
                        editOldLevel: oldLevel,
                        editGameId: $('#editGameSelect').val(),
                        editGameName: $("#editGameSelect :selected").text(),
                        editDate: $("#editDate").val(),
                        editAmount: $("#editAmount").val(),
                        editMaxAmount: $("#editMaxAmount").val(),
                        editLevel: $("#editLevel").val(),
                    },
                    function (data) {
                        if (data == "1") {
                            swal({
                                title: "Record Edited",
                                text: "The record was edited",
                                type: "success"
                            });
                            //reload the table
                            jackpotTable.ajax.reload();
                        }
                        else {
                            swal({
                                title: "Edit Record error",
                                text: "Record was not edited",
                                type: "error"
                            });
                        }
                    }
                );
            }
        }

        //Adds a new jackpot
        function addJackpot() {

            // Perform check that all input Fields are required before making the ajax call
            var amount = $("#amount").val();
            var maxAmount = $("#maxAmount").val();

            if ((amount != "") && (maxAmount != "")) {

                //send the data to the database with ajax post
                $.post("configuration/jackpotManagement.inc", {
                        gameId:$('#gameSelect :selected').val(),
                        addGameName: $("#gameSelect :selected").text(),
                        date:$("#date").val(),
                        amount:$("#amount").val(),
                        maxAmount:$("#maxAmount").val(),
                        level:$("#level").find(" :selected").text(),
                    },
                    function(data) {
                        if (data == "1" ) {
                            swal({
                                title: "Jackpot Added",
                                text: "The jackpot was succesfully added",
                                type: "success"
                            });
                            //reload the table
                            jackpotTable.ajax.reload();
                        }
                        else {
                            swal({
                                title: "Add Record error",
                                text: "Record was not added",
                                type: "error"
                            });
                        }
                    }
                );
            }
            else {
                swal({
                    title: "Mandatory Fields Required",
                    text: "Enter both an ammount and a max amount",
                    type: "error"
                });
            }
            $("#amount").val("");
            $("#maxAmount").val("");
        }
    });

    var oldLevel;

    $(document).on('click',".editJackpot", function() {
        id = $(this).attr('data-id');
        var row = $(this).closest("tr");
        oldLevel = row.find('td:eq(5)').text();
        $("#editGameSelect").val($.trim($(row).find('td:eq(0)').text()));
        $("#editDate").val($.trim($(row).find('td:eq(2)').text()));
        $("#editAmount").val($.trim($(row).find('td:eq(3)').text()));
        $("#editMaxAmount").val($.trim($(row).find('td:eq(4)').text()));
        $("#editLevel").val($.trim($(row).find('td:eq(5)').text()));
    });

    $(document).on('click', ".addItemToCache", function() {
        var jkpotId = $(this).attr('data-id');
        var tr = $(this).closest("tr");
        var gameId = tr.find('td:eq(0)').text();
        var gameName = tr.find('td:eq(1)').text();
        var date = tr.find('td:eq(2)').text();
        var amount = tr.find('td:eq(3)').text();
        var maxAmount = tr.find('td:eq(4)').text();
        var level = tr.find('td:eq(5)').text();

        $.post("configuration/jackpotManagement.inc", {
            jkpotId: jkpotId,
            gameId : gameId,
            gameName: gameName,
            date : date,
            amount : amount,
            maxAmount : maxAmount,
            level : level
        },
        function(data){
            if (data == "1") {
                swal({
                    title: "Record Updated",
                    text: "The record has been successfully updated",
                    type: "success"
                });
                jackpotTable.ajax.reload();
            }
        }
        );
    });

    //deletes a jackpot
    $(document).on('click', ".deleteJackpot", function() {
        var entryId = $(this).attr('data-id');

        var tr = $(this).closest("tr");
        var gameId = tr.find('td:eq(0)').text();
        var gameLevel = tr.find('td:eq(5)').text();

        swal({
            title: "Are you sure?",
            text: "The record will be deleted",
            type: 'warning',
            showCancelButton: true,

        },
            function() {
                $.post("configuration/jackpotManagement.inc", {
                    deleteId: entryId,
                    deleteGamId: gameId,
                    deleteLevel: gameLevel
                }, function(data) {
                    if (data == "1") {
                        swal({
                            title: "Record Deleted",
                            text: "The record was succesfully deleted",
                            type: "success"
                        });
                        jackpotTable.ajax.reload();
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

    <!-- Add a Jackpot Modal Start  -->
    <div class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true" id="addJackpotModal">
        <div class="modal-header"> <!-- Modal header -->
            <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
            <h3 class="text-info text-center"><?=$lang->getLang("Add a jackpot")?></h3> <br />
        </div>
        <div class="modal-body"> <!-- Modal body -->
            <p>
            <table class='table table-condensed '>
                <tr><td class='blackTd'><?=$lang->getLang("Game")?></td>
                    <td>
                        <select name='gameSelect' id="gameSelect">
                            <?php
                            $gameNameQuerry = "SELECT GAM_NAME, GAM_ID FROM game";
                            $allGameNamesFromDB = $dbh->doCachedQuery($gameNameQuerry, 0);

                            while($allGameNamesFromDB->hasNext()){
                                $row=$allGameNamesFromDB->next();
                                $gameName = $row['gam_name'];
                                ?>
                                <option value='<?=$row['gam_id'] ?>'><?=$lang->getLang("$gameName")?></option>
                                <?php
                            }
                            ?>
                        </select>
                    </td>
                </tr>

                <tr>
                    <td class="blackTd"><?=$lang->getLang("Date")?></td>
                    <td><input type="text" id="date" disabled/></td>
                </tr>

                <tr>
                    <td class="blackTd"><?=$lang->getLang("Amount")?></td>
                    <td><input type="number"  step="0.01" id="amount" required/></td>
                </tr>

                <tr>
                    <td class="blackTd"><?=$lang->getLang("Max Amount")?></td>
                    <td><input type="number"  step="0.01" id="maxAmount" required/></td>
                </tr>

                <tr>
                    <td class="blackTd"><?=$lang->getLang("Level")?></td>
                    <td>
                        <select name='level' id="level">
                            <option value=''><?=$lang->getLang("1")?></option>
                            <option value=''><?=$lang->getLang("2")?></option>
                            <option value=''><?=$lang->getLang("3")?></option>
                        </select>
                    </td>
                </tr>
            </table>
            </p>
        </div>
        <div class="modal-footer"> <!-- Modal footer -->
            <button type="button" class="btn btn-default" data-dismiss="modal">Close</button>
            <button class="btn btn-primary" id="addJackpot" data-dismiss="modal" ><?=$lang->getLang("Save")?></button>
        </div>
    </div> <!-- Add a Jackpot Modal End  -->


<div class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true" id="editJackpot"> <!-- Edit a Jackpot Modal Start  -->
    <div class="modal-header"> <!-- Modal header -->
        <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
        <h3 class="text-info text-center"><?=$lang->getLang("Edit jackpot")?></h3> <br />
    </div>
    <div class="modal-body"> <!-- Modal body -->
        <p>
        <table class='table table-condensed '>
            <tr><td class='blackTd'><?=$lang->getLang("Game")?></td>
                <td>
                    <select name='gameSelect' id="editGameSelect">
                        <?php
                        $gameNameQuerry = "SELECT GAM_NAME, GAM_ID FROM game";
                        $allGameNamesFromDB = $dbh->doCachedQuery($gameNameQuerry, 0);
                        while($allGameNamesFromDB->hasNext()){
                            $row=$allGameNamesFromDB->next();
                            $gameName = $row['gam_name'];
                            ?>
                            <option value='<?=$row['gam_id'] ?>'><?=$lang->getLang("$gameName")?></option>
                            <?php
                        }
                        ?>
                    </select>
                </td>
            </tr>
            <tr>
                <td class="blackTd"><?=$lang->getLang("Date")?></td>
                <td><input type="text" id="editDate" disabled/></td>
            </tr>

            <tr>
                <td class="blackTd"><?=$lang->getLang("Amount")?></td>
                <td><input type="number"  step="0.01" id="editAmount" required/></td>
            </tr>

            <tr>
                <td class="blackTd"><?=$lang->getLang("Max Amount")?></td>
                <td><input type="number"  step="0.01" id="editMaxAmount" required/></td>
            </tr>

            <tr>
                <td class="blackTd"><?=$lang->getLang("Level")?></td>
                <td>
                    <select name='level' id="editLevel">
                        <option value='1'><?=$lang->getLang("1")?></option>
                        <option value='2'><?=$lang->getLang("2")?></option>
                        <option value='3'><?=$lang->getLang("3")?></option>
                    </select>
                </td>
            </tr>
        </table>
        </p>
    </div>
    <div class="modal-footer"> <!-- Modal footer -->
        <button type="button" class="btn btn-default" data-dismiss="modal">Close</button>
        <button class="btn btn-primary" id="editJackpotData" data-dismiss="modal" ><?=$lang->getLang("Save")?></button>
    </div>
</div><!--Edit Modal end -->


<table class="table table-bordered" style="width: calc(100vw - 200px);"">
    <tr>
        <td class="navbar-inner" colspan="2"><h3 class="text-center"><?=$lang->getLang("Manage Jackpot")?></h3></td>
    </tr>
    <tr>
        <td colspan="2"  >
            <a class="fright btn btn-info btn-small" style="color:white" data-toggle="modal" data-target="#addJackpotModal" ><?=$lang->getLang("Add a jackpot")?></a>
        </td>
    </tr>
    <tr>
        <td>
            <table id="jackpotTable" class='display table table-striped table-hover table-bordered table-condensed dataTable no-footer' style="width: 100%">
                <thead>
                <tr>
                    <th><?=$lang->getLang("Game id")?></th>
                    <th><?=$lang->getLang("Game name")?></th>
                    <th><?=$lang->getLang("Creation date")?></th>
                    <th><?=$lang->getLang("Amount")?></th>
                    <th><?=$lang->getLang("Max Amount")?></th>
                    <th><?=$lang->getLang("Level")?></th>
                    <th><?=$lang->getLang("Operation")?></th>
                </tr>
                </thead>
            </table>
        </td>
    </tr>
</table>

