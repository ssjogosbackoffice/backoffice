<?php

check_access('manage_gamehost', true);
require_once 'RedisClient.php';
include_once 'WebRequest.class.inc';

$redisClient = new RedisClient(REDISSERVER, REDISPORT, $dbLog);

function addGameHostCurlRequest($url, $gameHostId) {
    $data="operationcode=2011&op=1&sername=game_host&auxid=$gameHostId";
    $webReq = new WebRequest($url);
    $webReq->setOptions($data);
    $webReq->sendRequest();
    return  $webReq->getResponse();
}

function editGameHostCurlRequest($url, $gameHostId) {
    $data="operationcode=2011&op=0&sername=game_host&auxid=$gameHostId";
    $webReq = new WebRequest($url);
    $webReq->setOptions($data);
    $webReq->sendRequest();
    return  $webReq->getResponse();
}

function deleteGameHostCurlRequest($url, $gameHostId) {
    $data="operationcode=2011&op=2&sername=game_host&auxid=$gameHostId";
    $webReq = new WebRequest($url);
    $webReq->setOptions($data);
    $webReq->sendRequest();
    return  $webReq->getResponse();
}

//external server to send data after a succesfull insert, update and delete
$url = $_SERVER['WEB_SERVICES'];


//Server side implementation of datatables
if($_GET['listHosts'] == 1){

    //Create limit part of the sql querry
    $start = $_GET['start'];
    $length = $_GET['length'];
    $limitQuerry = "LIMIT $start , $length";

    //Create order by part of sql querry
    $columns = array('hst_name','hst_string','hst_string', 'hst_status', 'hst_host_info', 'hst_game_status_info');
    $columnNumber =$_GET['order'][0]['column'];
    $ascOrDesc = $_GET['order'][0]['dir'];
    $colName = $columns[$columnNumber];
    $orderByQuerry = " ORDER BY " . $colName . " " .$ascOrDesc ;

    //Create search part of sql querry
    if($_GET['search']['value'] != "") {

        $searchableColumn1 = "hst_name";
        $searchableColumn2 = "hst_string";
        $searchableColumn3 = "hst_status";
        $searchableColumn4 = "hst_host_info";
        $searchKeyword = $dbh->prepareString($_GET['search']['value']);
        $searchKeyword = trim($searchKeyword, "'");
        $sqlLikeStmnt = "where ($searchableColumn1 LIKE '%$searchKeyword%' or $searchableColumn2 LIKE '%$searchKeyword%' or $searchableColumn3 LIKE '%$searchKeyword%' or $searchableColumn4 LIKE '%$searchKeyword%')";


//    SELECT gh.*, jur_id, jur_name, jur_class FROM game_host gh LEFT JOIN jurisdiction ON gh.hst_jur_id = jur_id;
        $sql = "SELECT gh.*, jur_id, jur_name, jur_class FROM game_host gh LEFT JOIN jurisdiction ON gh.hst_jur_id = jur_id $sqlLikeStmnt $orderByQuerry $limitQuerry";
        $totalsSql = "SELECT COUNT(*) FROM game_host $sqlLikeStmnt";
    }
    else {
        $sql = "SELECT gh.*, jur_id, jur_name, jur_class FROM game_host gh LEFT JOIN jurisdiction ON gh.hst_jur_id = jur_id $orderByQuerry $limitQuerry";

        //$sql = "SELECT *,  jur_id, jur_name, jur_class FROM game_host LEFT JOIN jurisdiction ON hst_jur_id = jur_id";
        //var_dump($sql);
        $totalsSql = "SELECT COUNT(*) FROM game_host";
    }

    $response =  $dbh->doCachedQuery($sql, 0);
    $total = $dbh->queryOne($totalsSql);

    $tableArray = array();

    while ($response->hasNext()) {
        $row = $response->next();
        $id = $row['hst_id'];
        
        // check if we have data in reddis
        $reddisKeyword = "HOST-GAME-INFO-$id";
        $reddisData = $redisClient->get($reddisKeyword);

        if($reddisData != NULL) {
            $result = json_decode($reddisData, true);
            $section = array();
            $section[0] = $result['hostName'];
            $myData = $result['hostString'];
            $hostDetailsArr = explode("~", $myData);
            $section[1] = $hostDetailsArr[0];
            $gameDetails = "";
            if(count($hostDetailsArr) == 2) {
                $gameDetails = $hostDetailsArr[1];
            }
            else if (count($hostDetailsArr) == 3) {
                $gameDetails = $hostDetailsArr[2];
            } else {
                $gameDetails = "";
            }

            $httId = $result['hostType'];
            if($httId == "5"){
                $sql = "select pes_name from providers where pes_game_code = '$gameDetails'";
                $gameNameObj = $dbh->doCachedQuery($sql, 0);
                $gameName = "";
                while ($gameNameObj->hasNext()){
                    $row = $gameNameObj->next();
                    $gameName = $row['pes_name'];
                }
                $section[2] = $gameName;
            }
            else {
                $section[2] = "<button class='btn btn-small viewDetails' data-toggle='modal' data-target='#viewGames' data-id='$gameDetails'>View</button>";
            }

            if ($result['hostStatus'] == "1" ){
                $section[3] = "<span class='text-success'>Enabled</span>";
            } else {
                $section[3] = "<span class='text-error'>Disabled</span>";
            }
            if($result['hostInfo'] == NULL) {
                $section[4] = "";
            } else {
                $section[4] =  $result['hostInfo'];
            }

            $statusDetails = $result['gameStatusInfo'];
            if($statusDetails == NULL) {
                $section[5] = "-";
            } else {
                $section[5] = "<button class='btn btn-small viewGameStatusDetails' data-toggle='modal' data-target='#viewHstStatusDetails' viewGameStatusDet='$statusDetails'>View</button>";
            }
            $hstJurClass = $hstJurName = $hstJurId = "";
            if ($row['jur_name'] != NULL) {
                $hstJurClass =  $row['jur_class'];
                $hstJurName = $row['jur_name'];
                $hstJurId = $row['jur_id'];
                $section[6] = "<span data-class='$hstJurClass' data-name='$hstJurName' data-id='$hstJurId'> ". $hstJurName . " (". $hstJurClass. ")</span>";
            } else {
                $section[6] = "";
            }
            $section[7] = "<div class='btn-group'>
                            <button type='button' class='btn btn-small dropdown-toggle' data-toggle='dropdown'> Action <span class='caret'></span></button>
                            <ul class='dropdown-menu' style='min-width: 65px'>
                                <li>
                                    <a class='editHost' data-jur-class='$hstJurClass' data-jur-name='$hstJurName' data-jur-id='$hstJurId' data-toggle='modal' data-target='#editHostModal' data-id='$id' data-hstString='$myData' data-httId='$httId'><i style='padding-right: 5px;' class='icon-pencil'> </i>     Edit</a>
                                </li>
                                <li>
                                    <a class='deleteHost' data-id='$id'><i style='padding-right: 5px;' class='icon-trash'> </i>Delete</a>
                                </li>                                
                            </ul>
                        </div>";
            array_push($tableArray, $section);
        } else {
            $section = array();
            $section[0] = $row['hst_name'];
            $myData = $row['hst_string'];
            $hostDetailsArr = explode("~", $myData);

            $section[1] = $hostDetailsArr[0];
            $gameDetails = "";
            if(count($hostDetailsArr) == 2) {
                $gameDetails = $hostDetailsArr[1];
            }
            else if (count($hostDetailsArr) == 3) {
                $gameDetails = $hostDetailsArr[2];
            }else {
                $gameDetails = "";
            }

            if ($row['hst_status'] === "1" ){
                $section[3] = "<span class='text-success'>Enabled</span>";
            }
            else {
                $section[3] = "<span class='text-error'>Disabled</span>";
            }

            $statusDetails = $row['hst_game_status_info'];
            $hostGameStatusInfoDat = "";
            if($statusDetails == NULL) {
                $section[5] = "-";
            }
            else {
                $section[5] = "<button class='btn btn-small viewGameStatusDetails' data-toggle='modal' data-target='#viewHstStatusDetails' viewGameStatusDet='$statusDetails'>View</button>";
            }
            $section[2] = "";

            $httId = $row['hst_htt_id'];
            $id = $row['hst_id'];
            $hostActive = $row['hst_active'];
            $hostGroup = $row['hst_group'];

            if($httId == 5){
                $sql = "select pes_name from providers where pes_game_code = '$gameDetails'";
                $gameNameObj = $dbh->doCachedQuery($sql, 0);
                $gameName = "";
                while ($gameNameObj->hasNext()){
                    $row = $gameNameObj->next();
                    $gameName = $row['pes_name'];
                }
                $section[2] = $gameName;
            }
            else {
                $section[2] = "<button class='btn btn-small viewDetails' data-toggle='modal' data-target='#viewGames' data-id='$gameDetails'>View</button>";
            }
            $section[4] =  $row['hst_host_info'];

            $hstJurClass = $hstJurName = $hstJurId = "";
            if ($row['jur_name'] != NULL) {
                $hstJurClass =  $row['jur_class'];
                $hstJurName = $row['jur_name'];
                $section[6] = "<span data-jur-class='$hstJurClass' data-jur-name='$hstJurName' data-jur-id='$hstJurId'> ". $hstJurName . " (". $hstJurClass. ")</span>";
            } else {
                $section[6] = "";
            }
            $section[7] = "<div class='btn-group'>
                            <button type='button' class='btn btn-small btn-warning dropdown-toggle' data-toggle='dropdown'> Action <span class='caret'></span></button>
                            <ul class='dropdown-menu' style='min-width: 45px'>
                                <li>
                                    <a class='editHost' data-jur-class='$hstJurClass' data-jur-name='$hstJurName' data-jur-id='$hstJurId' data-toggle='modal' data-target='#editHostModal' data-id='$id' data-hstString='$myData' data-hstActive='$hostActive' data-hstGroup='$hostGroup' data-httId='$httId' data-stsDetail='$statusDetails'><i style='padding-right: 5px;' class='icon-pencil'> </i>     Edit</a>
                                </li>
                                <li>
                                    <a class='deleteHost' data-id='$id'><i style='padding-right: 5px;' class='icon-trash'> </i>Delete</a>
                                </li>
                                <li>
                                    <a class='addHostToCache' data-jur-id='$hstJurId' data-id='$id' data-hstString='$myData' data-hstActive='$hostActive' data-hstGroup='$hostGroup' data-httId='$httId' data-stsDetail='$statusDetails'>Add to cache </a>
                                </li>
                            </ul>
                        </div>";

            array_push($tableArray,$section);
        }
    }
    //var_dump($tableArray);

    $output= array(
        "draw"            => intval($_GET['draw']),
        "recordsTotal"    => intval($total),
        "recordsFiltered" => intval($total),
        "data"            => $tableArray
    );

    die(json_encode($output));

}

//View game list handling
if($_POST['listGames'] == 1) {

    $gamesId = $_POST['gamesId'];
    $sql = "select gam_name from game where gam_id  = ";
    $gameIdList = explode(";", $gamesId);
    if(count($gameIdList) > 0) {
        foreach($gameIdList as $data) {
            $sql .= $data;
            $sql .= " or gam_id = ";
        }
    }
    $sql = substr($sql, 0 , -13);

    $gamesList = $dbh->doCachedQuery($sql, 0);
    //die(var_dump(json_encode([1,$gamesList])));
    $gamesArr = [];
    while ($gamesList->hasNext()){
        $row = $gamesList->next();
        array_push($gamesArr, $row['gam_name']);
    }
    die(json_encode($gamesArr));
}

//Adds a new Host in db and redis and makes a request to external server with specific data
if(($_POST['addHostName'] != "") && ($_POST['addHostString'] != "")){

    $hstId = $dbh->nextSequence('HST_ID_SEQ');

    //Get the values from Ajax call and prepares them for insertion
    $hstName = $dbh->prepareString($_POST['addHostName']);
    $hstName = trim($hstName, "'");

    $hstString = $dbh->prepareString($_POST['addHostString']);
    $hstString = trim($hstString, "'");

    $hstActive = $dbh->prepareString($_POST['addHostActive']);
    $hstActive = trim($hstActive, "'");

    $hstStatus = $dbh->prepareString($_POST['addHostStatus']);
    $hstStatus = trim($hstStatus, "'");

    $hstGroup = $dbh->prepareString($_POST['addHostGroup']);
    $hstGroup = trim($hstGroup, "'");

    $hstHttId = $dbh->prepareString($_POST['addHostHttId']);
    $hstHttId = trim($hstHttId, "'");

    $hstJurId = $dbh->prepareString($_POST['addHostJurId']);
    $hstJurId = trim($hstJurId, "'");

    if($hstJurId != 0) {
        $insertSql = "INSERT INTO game_host (hst_id, hst_name, hst_string, hst_active, hst_status, hst_group, hst_htt_id, hst_jur_id ) VALUES ('$hstId', '$hstName', '$hstString', '$hstActive', '$hstStatus', '$hstGroup', '$hstHttId', '$hstJurId')";
    }
    else {
        $insertSql = "INSERT INTO game_host (hst_id, hst_name, hst_string, hst_active, hst_status, hst_group, hst_htt_id ) VALUES ('$hstId', '$hstName', '$hstString', '$hstActive', '$hstStatus', '$hstGroup', '$hstHttId')";
    }
    //insert statement

    $newHostAdded = $dbh->exec($insertSql);
    $newHostAdded = "" .$newHostAdded;

    //Add data to reddis
    $keyword = "HOST-GAME-INFO-$hstId";

    $hostDetailsArr = explode("~", $hstString);

    if(count($hostDetailsArr) == 2) {
        $gameDetails = $hostDetailsArr[1];
    }
    else if (count($hostDetailsArr) == 3) {
        $gameDetails = $hostDetailsArr[2];
    } else {
        $gameDetails = "";
    }

    $data = [
        'hostId' => $hstId,
        'hostName' =>  $hstName,
        'hostString' => $hstString,
        'hostActive' => $hstActive,
        'hostStatus' => $hstStatus,
        'hostGroup' => $hstGroup,
        'hostType' => $hstHttId,
        'unparsedGameList' => $gameDetails,
        'gameStatusInfo' => null,
        'hostInfo' => null,
        'hostJurId' => $hstJurId
    ];
    $dataJson = json_encode($data);

    //set the redis data
    $redisResponse = $redisClient->set($keyword, $dataJson);

    //send external server data
    addGameHostCurlRequest($url, $hstId);

    die($newHostAdded);
}

//Edits a host in db and redis
if(($_POST['editHostName'] != "") && ($_POST['editHostString'] != "")){

    //set the variables from post
    $edtHostId = $dbh->prepareString($_POST['editHostId']);
    $edtHostId = trim($edtHostId, "'");

    $edtHostName = $dbh->prepareString($_POST['editHostName']);
    $edtHostName = trim($edtHostName, "'");

    $edtHostString = $dbh->prepareString($_POST['editHostString']);
    $edtHostString = trim($edtHostString, "'");
    
    $edtHostStatus = $dbh->prepareString($_POST['editHostStatus']);
    $edtHostStatus = trim($edtHostStatus, "'");

    $edtHostHttId = $dbh->prepareString($_POST['editHostHttID']);
    $edtHostHttId = trim($edtHostHttId, "'");

    $edtHostActive = $dbh->prepareString($_POST['editHostActive']);
    $edtHostActive = trim($edtHostActive, "'");

    $editHostGroup = $dbh->prepareString($_POST['editHostGroup']);
    $editHostGroup = trim($editHostGroup, "'");

    $editUnparsedGameList = $dbh->prepareString($_POST['editUnparsedGameList']);
    $editUnparsedGameList = trim($editUnparsedGameList, "'");

    $editHostInfo = $dbh->prepareString($_POST['editHostInfo']);
    $editHostInfo = trim($editHostInfo, "'");

    $editGameStatusInfo = $dbh->prepareString($_POST['editGameStatusInfo']);
    $editGameStatusInfo = trim($editGameStatusInfo, "'");

    $editHstJurId = $dbh->prepareString($_POST['editHostJurId']);
    $editHstJurId  = trim($editHstJurId , "'");

    if($editHstJurId != 0) {
        $updateSQL = "UPDATE game_host SET hst_jur_id = '$editHstJurId', hst_name = '$edtHostName', hst_string = '$edtHostString', hst_status = '$edtHostStatus', hst_htt_id = '$edtHostHttId' WHERE hst_id = '$edtHostId' ";
    }
    else {
        $updateSQL = "UPDATE game_host SET hst_jur_id = NULL, hst_name = '$edtHostName', hst_string = '$edtHostString', hst_status = '$edtHostStatus', hst_htt_id = '$edtHostHttId' WHERE hst_id = '$edtHostId' ";
    }



    // update the data in db
    $updatedHost = $dbh->exec($updateSQL);
    $updatedHost = "" .$updatedHost;


    //Update data to reddis
    $keyword = "HOST-GAME-INFO-$edtHostId";

    //get the old record from redis
    $redisData = $redisClient->get($keyword);

    if($redisData != NULL) {
        $redisData = json_decode($redisData, true);

        $hostDetailsArr = explode("~", $edtHostString);

        if (count($hostDetailsArr) == 2) {
            $gameDetails = $hostDetailsArr[1];
        } else if (count($hostDetailsArr) == 3) {
            $gameDetails = $hostDetailsArr[2];
        } else {
            $gameDetails = "";
        }

        $data = [
            'hostId' => $edtHostId,
            'hostName' => $edtHostName,
            'hostString' => $edtHostString,
            'hostActive' => $redisData['hostActive'],
            'hostStatus' => $edtHostStatus,
            'hostGroup' => $redisData['hostGroup'],
            'hostType' => $edtHostHttId,
            'unparsedGameList' => $gameDetails,
            'gameStatusInfo' => $redisData['gameStatusInfo'],
            'hostInfo' => $redisData['hostInfo'],
            'hostJurId' => $editHstJurId
        ];
        $dataJson = json_encode($data);

        //set the redis data
        $redisResponse = $redisClient->set($keyword, $dataJson);
        //$redisData = $redisClient->get($keyword);
        //print($redisData);
    }
    else {
        $data = [
            'hostId' => $edtHostId,
            'hostName' => $edtHostName,
            'hostString' => $edtHostString,
            'hostActive' => $edtHostActive,
            'hostStatus' => $edtHostStatus,
            'hostGroup' => $editHostGroup,
            'hostType' => $edtHostHttId,
            'unparsedGameList' => $editUnparsedGameList,
            'gameStatusInfo' => $editGameStatusInfo,
            'hostInfo' => $editHostInfo,
            'hostJurId' => $editHstJurId
        ];
        $dataJson = json_encode($data);

        //set the redis data
        $redisResponse = $redisClient->set($keyword, $dataJson);
    }

    //send data to external server
    editGameHostCurlRequest($url, $edtHostId);

    die($updatedHost);
}

// delte a record in db
if(isset($_POST['deleteId'])) {

    $dltHostId = $dbh->prepareString($_POST['deleteId']);
    $dltHostId = trim($dltHostId, "'");

    //delete from redis
    $keyword = "HOST-GAME-INFO-$dltHostId";
    $redisClient->delete($keyword);

    // delete from db
    $sql = "DELETE FROM game_host where hst_id = '$dltHostId'";
    
    $response = $dbh->exec($sql);
    $response = "".$response;

    //send data to external server
    deleteGameHostCurlRequest($url, $dltHostId);
    die($response);
}

// add a record to redis
if (isset($_POST['addToCache'])) {

    $hostId = $_POST['hostId'];
    $hostString = $_POST['hostString'];
    $hostName = $_POST['hostName'];
    $hostActive = $_POST['hostActive'];
    $hostStatus = $_POST['hostStatus'];
    $hostGroup = $_POST['hostGroup'];
    $hostType = $_POST['hostType'];
    $unparsedGameList = $_POST['unparsedGameList'];
    $hostInfo = $_POST['hostInfo'];
    $gameStatusInfo = $_POST['gameStatusInfo'];
    $hostJurId = $_POST['hostJurId'];
    if ($hostInfo == "") {
        $hostInfo = null;
    }
    if ($gameStatusInfo == ""){
        $gameStatusInfo = null;
    }
    // create the key to add to redis
    $keyword = "HOST-GAME-INFO-$hostId";

    // set data in Json format
    $data = [
        'hostId' => $hostId,
        'hostName' =>  $hostName,
        'hostString' => $hostString,
        'hostActive' => $hostActive,
        'hostStatus' => $hostStatus,
        'hostGroup' => $hostGroup,
        'hostType' => $hostType,
        'unparsedGameList' => $unparsedGameList,
        'gameStatusInfo' => $gameStatusInfo,
        'hostInfo' => $hostInfo,
        'hostJurId' => $hostJurId
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

//querries the db for the chosen jurisdiction
if(isset($_POST['jurChooser'])){

    $jurClass = $_POST['jurClass'];
    $sql = "SELECT jur_id, jur_name FROM jurisdiction WHERE jur_class = '$jurClass'";
    $response =  $dbh->doCachedQuery($sql, 0);
    $resp = array();
    while ($response->hasNext()) {
        $row = $response->next();
        $section = array();
        $section[0] = $row['jur_id'];
        $section[1] = $row['jur_name'];
        array_push($resp, $section);
    }
    die(json_encode($resp));
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


    $(document).on('change',"#jur-class", function() {

        var jurClass = $(this).val();

        //make an ajax request
        $.post("configuration/gameHost.inc", {
            jurChooser: 1,
            jurClass : jurClass
        }).done(function(data) {
            var jurname = $("#jur-name");
            jurname.empty();
            var deflt = "<option value = '0'> None </option>";
            jurname.append(deflt);
            var myObj = JSON.parse(data);
            for (var i = 0; i < myObj.length; i++) {
                // Create a select option for each select
                var select =" <option value='" + myObj[i][0] +"'>" +myObj[i][1] +"</option>";
                jurname.append(select);
            }
        })
    });

    $(document).on('change',"#edt-jur-class", function() {

        var jurClass = $(this).val();

        //make an ajax request
        $.post("configuration/gameHost.inc", {
            jurChooser: 1,
            jurClass : jurClass
        }).done(function(data) {
            var jurname = $("#edt-jur-name");
            jurname.empty();
            var deflt = "<option value = '0'> None </option>";
            jurname.append(deflt);
            var myObj = JSON.parse(data);
            for (var i = 0; i < myObj.length; i++) {
                // Create a select option for each select
                var select =" <option value='" + myObj[i][0] +"'>" +myObj[i][1] +"</option>";
                jurname.append(select);
            }
        })
    });


    $(document).ready(function () {

        // Initializing the datatable
        hostTable = $('#hostTable').DataTable({
            "processing": true,
            "serverSide": true,
            "ajax": {
                "url": "configuration/gameHost.inc",
                "data": function (d) {
                    d.listHosts = "1";
                }
            },
            'sPaginationType': "full_numbers",
            "columnDefs": [
                {
                    "orderable": false, "targets": [ 2, 6, 7 ]
                }
            ]
        });
    });

    //displays the game list in a modal
    $(document).on('click',".viewDetails", function() {
        // clear the search bar when modal opens
        $("#searchGame").val("");
        // get the data attributes which are passed to ajax
        gameId = $(this).attr('data-id');
        httId = $(this).attr('data-httdID');

        //make an ajax request
        $.post("configuration/gameHost.inc", {
                gamesId: gameId,
                httId: httId,
                listGames: 1
        }).done(function(data) {
            obj = JSON.parse(data);
            var text = "";
            for (var i = 0; i < obj.length; i++) {
                text += obj[i] + "<br>";
                $('.gmeList').html(text);
            }
        })
    });

    // displays game status details in a modal
    $(document).on('click',".viewGameStatusDetails", function() {
        var statusDetails = $(this).attr('viewGameStatusDet');
        $('.gmeStatusInfo').html(statusDetails);
    });

    // makes a call to the server to add a new host record
    $(document).on('click',"#saveHost", function() {
        //Get the values from the form and make an ajax request to server
        var hostName = $("#addHostName").val();
        var hostString = $("#addHostString").val();
        var hostActive = $("#addHostActive").val();
        var hostStatus = $("#hostStatus").val();
        var hostGroup = $("#addHostGroup").val();
        var hostHttId = $("#addHostHttID").val();
        var hostJurId = $("#jur-name").val();
        console.log(hostJurId);

        var isInvalidHost = true;

        var inputString = hostString.split("~");
        var string = inputString[0];
        var inputStringLength = inputString.length;
        //console.log(inputStringLength);

        if( hostHttId  == 2 ) {
            // check if valid internal game
            //
            // var regex = /^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]):[1-9][0-9]+$/i;
            // var isIp = regex.test(string);
            // if (isIp && (inputStringLength == 2)) {
            //     isInvalidHost = false;
            // }

            isInvalidHost = false;

        } else if ( hostHttId == 5) {
            // check if valid provider game

            var http = "http";
            var firstLetters = string.substr(0, 4);

            if((http == firstLetters) && (inputStringLength == 3)) {
                isInvalidHost = false;
            }
        }

        if (hostString == ""){
            swal({
                title: "Host String Error",
                text: "Please insert a host string",
                type: 'error',
                showCancelButton: true,

            });
        } else if (hostName == ""){
            swal({
                title: "Host name Error",
                text: "Please insert the host name",
                type: 'error',
                showCancelButton: true,
            });
        } else if (isInvalidHost) {
            swal({
                title: "Host String Error",
                text: "Invalid String type for game",
                type: 'error',
                showCancelButton: true,
            });
        } else {
            $.ajax({
                url: 'configuration/gameHost.inc',
                method: "POST",
                data: {
                    addHostName: hostName,
                    addHostString: hostString,
                    addHostActive: hostActive,
                    addHostStatus: hostStatus,
                    addHostGroup: hostGroup,
                    addHostHttId: hostHttId,
                    addHostJurId: hostJurId
                }
            }).done(function (data) {
                if (data == "1") {
                    swal({
                        title: "Record added",
                        text: "The record was succesfully added",
                        type: "success"
                    });
                    hostTable.ajax.reload();
                }
            })
                .fail(function (error) {
                    console.log(error);
                });
        }
    });

    //search box in the game list modal
    $(document).on('keyup',"#searchGame", function() {

        // get the input value on each keypress
        var searchGameKeyword = $("#searchGame").val();
        var searchGameKeywordToLower = searchGameKeyword.toLowerCase();
        var matchingIndexList = [];
        var finds = [];
        var gameList = obj;

        var gameListToLower = [];
        for(var i = 0; i < gameList.length; i++) {
            var gameName = gameList[i];
            var gameNameToLower = gameName.toLowerCase();
            gameListToLower.push(gameNameToLower);
        }

        var result = searchStringInArray(searchGameKeywordToLower, gameListToLower);


        for(var k = 0; k < matchingIndexList.length; k++) {
            var number = matchingIndexList[k];
            var itemFound = gameList[number];
            finds.push(itemFound);
        }

        // display the entries
        var text = " ";
        if(finds.length == 0) {
            $('.gmeList').html(text);
        }
        for (var i = 0; i < finds.length; i++) {
            text += finds[i] + "<br>";
            $('.gmeList').html(text);
        }

        function searchStringInArray (str, strArray) {
            for (var j=0; j<strArray.length; j++) {
                if (strArray[j].match(str))
                    matchingIndexList.push(j);
            }

            return -1;
        }
    });

    //sets the fields to empty or default value
    $(document).on('click',"#addNewHost", function() {
        $('#addHostName').val("");
        $('#addHostString').val("");
        $('#hostStatus').val('0');
        $('#addHostHttID').val('2');
        $('#jur-class').val('0');
        var jurname = $("#jur-name");
        jurname.empty();
        var deflt = "<option value = '0'> None </option>";
        jurname.append(deflt);
    });

    //sets the fields to empty or default value
    $(document).on('click',".editHost", function() {
        var row = $(this).closest("tr");

        id = $(this).attr('data-id');
        editHostActive = $(this).attr('data-hstActive');
        editHostGroup = $(this).attr('data-hstGroup');
        editHostInfo = $(row).find('td:eq(4)').text();
        editGameStatusInfo = $(this).attr('data-stsDetail');

        editHostJurClass = $(this).attr('data-jur-class');
        editHostJurName = $(this).attr('data-jur-name');
        editHostJurId = $(this).attr('data-jur-id');

        var jurname = $("#edt-jur-name");

        if(editHostJurClass != "") {
            console.log(editHostJurClass);
            $.ajax({
                url: 'configuration/gameHost.inc',
                method: "POST",
                data: {
                    jurChooser: 1,
                    jurClass : editHostJurClass
                }
            }).done(function (data) {
                jurname.empty();
                var deflt = "<option value = '0'> None </option>";
                jurname.append(deflt);
                var myObj = JSON.parse(data);
                for (var i = 0; i < myObj.length; i++) {
                    // Create a select option for each select
                    var select =" <option value='" + myObj[i][0] +"'>" +myObj[i][1] +"</option>";
                    jurname.append(select);
                }
                jurname.val(editHostJurId);
            })
        } else {
            jurname.empty();
            var deflt = "<option value = '0'> None </option>";
            jurname.append(deflt);
        }

        $('#edt-jur-class').val(editHostJurClass);

        var hstString = $(this).attr('data-hstString');
        var hstHttId = $(this).attr('data-httId');
        $("#editHostName").val($.trim($(row).find('td:eq(0)').text()));
        $("#editHostString").val(hstString);
        var myVar = $(row).find('td:eq(3)').text();
        var value = "";
        if (myVar == "Enabled") {
            value = 1;
        }
        else {
            value = 0;
        }
        $("#editHostStatus").val(value);
        $("#editHostHttID").val(hstHttId);
    });

    //deletes a host
    $(document).on('click', ".deleteHost", function() {
        var entryId = $(this).attr('data-id');
        swal({
                title: "Are you sure?",
                text: "The record will be deleted",
                type: 'warning',
                showCancelButton: true,
            },
            function() {
                $.post("configuration/gameHost.inc", {
                    deleteId: entryId
                }, function(data) {
                    if (data == "1") {
                        swal({
                            title: "Record Deleted",
                            text: "The record was succesfully deleted",
                            type: "success"
                        });
                        hostTable.ajax.reload();
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

    //edits a host
    $(document).on ('click', '#editHostData', function() {
    //Get the values from the form and make an ajax request to server

        var editHostName = $("#editHostName").val();
        var editHostString = $("#editHostString").val();
        var editHostStatus = $("#editHostStatus").val();
        var editHostHttID = $("#editHostHttID").val();
        var editHostJurId = $("#edt-jur-name").val();

       //console.log(editHostJurId);
        var isInvalidHost = true;



        var inputString = editHostString.split("~");
        var string = inputString[0];
        var inputStringLength = inputString.length;

        var games = "";
        if (inputStringLength == 2 ) {
            games = inputString[1];
        }
        else if (inputStringLength == 3){
            games = inputString[2];
        }

        if( editHostHttID  == 2 ) {
            //check if valid internal game

            // var regex = /^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]):[1-9][0-9]+$/i;
            // var result = regex.test(string);
            // if (result && (inputStringLength == 2)) {
            //     isInvalidHost = false;
            // }

            isInvalidHost = false;
        } else if ( editHostHttID == 5) {
            // check if valid provider game
            var http = "http";
            var firstLetters = string.substr(0, 4);
            if((http == firstLetters) && (inputStringLength == 3)) {
                isInvalidHost = false;
            }
        }


        if (editHostName == ""){
        swal({
            title: "Host Name Error",
            text: "Please insert a host name",
            type: 'error',
            showCancelButton: true,

        });
    } else if (editHostString == ""){
        swal({
            title: "Host String Error",
            text: "Please inser the host string",
            type: 'error',
            showCancelButton: true,
        });
    }else if (isInvalidHost) {
        swal({
            title: "Host String Error",
            text: "Invalid String type for game",
            type: 'error',
            showCancelButton: true,
        });
    } else {
        $.ajax({
            url: 'configuration/gameHost.inc',
            method: "POST",
            data: {
                editHostId : id,
                editHostName: editHostName,
                editHostString: editHostString,
                editHostActive: editHostActive,
                editHostStatus: editHostStatus,
                editHostGroup: editHostGroup,
                editHostHttID: editHostHttID,
                editUnparsedGameList: games,
                editHostInfo: editHostInfo,
                editGameStatusInfo: editGameStatusInfo,
                editHostJurId: editHostJurId
            }
        }).done(function (data) {
            if (data == 1) {
                swal({
                    title: "Record updated",
                    text: "The record was succesfully updated",
                    type: "success"
                });
                hostTable.ajax.reload();
            }
        }).fail(function (error) {
                console.log(error);
            });
    }
});

    //deletes a host
    $(document).on('click', ".closeGameList", function() {
        $('.gmeList').html("");
    });

    //adds an existing record to cache
    $(document).on('click', ".addHostToCache", function() {
        var hostId = $(this).attr('data-id');
        var hstString =  $(this).attr('data-hstString');
        var hostActive =  $(this).attr('data-hstActive');
        var hostGroup = $(this).attr('data-hstGroup');
        var hostType = $(this).attr('data-httId');
        var gameStatusInfo = $(this).attr('data-stsDetail');
        var hostJurId = $(this).attr('data-jur-id');


        var tr = $(this).closest("tr");
        var gameName = tr.find('td:eq(0)').text();
        var hostStatus = tr.find('td:eq(3)').text();

        var status = "";
        if(hostStatus == "Enabled") {
            status = 1;
        } else {
            status = 0;
        }
        var unparsdGameList = hstString.split("~");
        var length = unparsdGameList.length;
        var games = "";
        if (length == 2 ) {
            games = unparsdGameList[1];
        }
        else if (length == 3){
            games = unparsdGameList[2];
        }
        var hostInfo = tr.find('td:eq(4)').text();

        $.post("configuration/gameHost.inc", {
                addToCache: 1,
                hostId: hostId,
                hostString: hstString,
                hostName: gameName,
                hostActive: hostActive,
                hostStatus: status,
                hostGroup: hostGroup,
                hostType: hostType,
                unparsedGameList: games,
                hostInfo: hostInfo,
                gameStatusInfo: gameStatusInfo,
                hostJurId : hostJurId
            },
            function(data){
                if (data == "1") {
                    swal({
                        title: "Record Added To cache",
                        text: "The record has been successfully added to cache",
                        type: "success"
                    });
                    hostTable.ajax.reload();
                }
            }
        );
    });

</script>

<?php

/** Test case for Redis */
$hstId = "52";
$keyword = "HOST-GAME-INFO-$hstId";


$redisData = $redisClient->get($keyword);

$redisData = json_decode($redisData, true);
//$httId = $redisData['hostType'];
//var_dump($redisData);
//$redisClient->delete($keyword);

//echo "test";
//preprint($redisData);
?>
<!-- View Games Modal Start  -->
<div class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true" id="viewGames">
    <div class="modal-header"> <!-- Modal header -->
        <button type="button" class="close closeGameList" data-dismiss="modal" aria-hidden="true">&times;</button>
        <h3 class="text-info text-center"><?=$lang->getLang("View Games")?></h3> <br />
    </div>
    <div class="modal-body"> <!-- Modal body -->

        <table class='table table-condensed'>
            <tr>
                <td><input id="searchGame" type="text" class="form-control" placeholder="Search Game..."></td>
            </tr>

            <tr>
                <td class='blackTd'><?=$lang->getLang("Game Name")?></td>
            </tr>
            <tr>
                <td><p class="gmeList"></p></td>
            </tr>
        </table>
    </div>
    <div class="modal-footer"> <!-- Modal footer -->
        <button type="button" class="btn btn-default closeGameList" data-dismiss="modal">Close</button>
    </div>
</div> <!-- View Games Modal End  -->


<!-- View Host Status Details Start  -->
<div class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true" id="viewHstStatusDetails">
    <div class="modal-header"> <!-- Modal header -->
        <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
        <h3 class="text-info text-center"><?=$lang->getLang("View Game Status Info")?></h3> <br />
    </div>
    <div class="modal-body"> <!-- Modal body -->

        <table class='table table-condensed '>
            <tr>
                <td class='blackTd'><?=$lang->getLang("Game Status Info")?></td>
            </tr>
            <tr>
                <td><p class="gmeStatusInfo"></p></td>
            </tr>
        </table>
    </div>
    <div class="modal-footer"> <!-- Modal footer -->
        <button type="button" class="btn btn-default" data-dismiss="modal">Close</button>
    </div>
</div> <!-- View Host Status Details End  -->


<!-- Add Host Modal Start  -->
<div class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true" id="addHost">
    <div class="modal-header"> <!-- Modal header -->
        <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
        <h3 class="text-info text-center"><?=$lang->getLang("Add a Host")?></h3> <br />
    </div>
    <div class="modal-body"> <!-- Modal body -->

        <table class='table table-condensed '>
            <tr>
                <td class='blackTd'><?=$lang->getLang("Host Name")?></td>
                <td><input type="text" id="addHostName" /></td>
            </tr>
            <tr>
                <td class='blackTd'><?=$lang->getLang("Host String")?></td>
                <td><textarea class="form-control" rows="3" id="addHostString" ></textarea></td>

            </tr>
            <tr>
                <td class='blackTd'><?=$lang->getLang("Host Active")?></td>
                <td><input type="number" id="addHostActive"/ value ="1" disabled></td>
            </tr>

            <tr>
                <td class="blackTd"><?=$lang->getLang("Host Status")?></td>
                <td>
                    <select name='hostStatus' id="hostStatus">
                        <option value='0'><?=$lang->getLang("Disabled")?></option>
                        <option value='1'><?=$lang->getLang("Enabled")?></option>
                    </select>
                </td>
            </tr>

            <tr>
                <td class='blackTd'><?=$lang->getLang("Host Group")?></td>
                <td><input type="number" id="addHostGroup"/ value ="1" disabled></td>
            </tr>

            <tr>
                <td class="blackTd"><?=$lang->getLang("Host htt id")?></td>
                <td>
                    <select name='level' id="addHostHttID">
                        <option value='2'><?=$lang->getLang("Internal Games")?></option>
                        <option value='5'><?=$lang->getLang("Provider Games")?></option>
                    </select>
                </td>
            </tr>
            <tr>
                <td class="blackTd"><?=$lang->getLang("Jurisdiction class")?></td>
                <td>
                    <select id="jur-class">
                        <option value='0'><?=$lang->getLang("None")?></option>
                        <option value='nation'><?=$lang->getLang("Nation")?></option>
                        <option value='region'><?=$lang->getLang("Region")?></option>
                        <option value='district'><?=$lang->getLang("District")?></option>
                        <option value='club'><?=$lang->getLang("Club")?></option>
                    </select>
                </td>
            </tr>
            <tr>
                <td class="blackTd"><?=$lang->getLang("Jurisdiction name")?></td>
                <td>
                    <select name='level' id="jur-name">
                        <option value='0'><?=$lang->getLang("None")?></option>
                    </select>
                </td>
            </tr>
        </table>
    </div>
    <div class="modal-footer"> <!-- Modal footer -->
        <button class="btn btn-primary" id="saveHost" data-dismiss="modal" ><?=$lang->getLang("Save")?></button>
        <button type="button" class="btn btn-default" data-dismiss="modal">Close</button>
    </div>
</div> <!-- Add Games Modal End  -->


<!-- Edit Host Modal Start  -->
<div class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true" id="editHostModal">
    <div class="modal-header"> <!-- Modal header -->
        <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
        <h3 class="text-info text-center"><?=$lang->getLang("Edit a Host")?></h3> <br />
    </div>
    <div class="modal-body"> <!-- Modal body -->

        <table class='table table-condensed '>
            <tr>
                <td class='blackTd'><?=$lang->getLang("Host Name")?></td>
                <td><input type="text" id="editHostName" /></td>
            </tr>
            <tr>
                <td class='blackTd'><?=$lang->getLang("Host String")?></td>
                <td><textarea class="form-control" rows="3" id="editHostString" ></textarea></td>
            </tr>
            <tr>
                <td class='blackTd'><?=$lang->getLang("Host Active")?></td>
                <td><input type="number" step="1" id="editHostActive"/ value ="1" disabled></td>
            </tr>

            <tr>
                <td class="blackTd"><?=$lang->getLang("Host Status")?></td>
                <td>
                    <select name='hostStatus' id="editHostStatus">
                        <option value='0'><?=$lang->getLang("Disabled")?></option>
                        <option value='1'><?=$lang->getLang("Enabled")?></option>
                    </select>
                </td>
            </tr>

            <tr>
                <td class="blackTd"><?=$lang->getLang("Host htt id")?></td>
                <td>
                    <select name='level' id="editHostHttID">
                        <option value='2'><?=$lang->getLang("Internal Games")?></option>
                        <option value='5'><?=$lang->getLang("Provider Games")?></option>
                    </select>
                </td>
            </tr>
            <tr>
                <td class="blackTd"><?=$lang->getLang("Jurisdiction class")?></td>
                <td>
                    <select id="edt-jur-class">
                        <option value='0'><?=$lang->getLang("None")?></option>
                        <option value='nation'><?=$lang->getLang("Nation")?></option>
                        <option value='region'><?=$lang->getLang("Region")?></option>
                        <option value='district'><?=$lang->getLang("District")?></option>
                        <option value='club'><?=$lang->getLang("Club")?></option>
                    </select>
                </td>
            </tr>
            <tr>
                <td class="blackTd"><?=$lang->getLang("Jurisdiction name")?></td>
                <td>
                    <select name='level' id="edt-jur-name">
                        <option value='0'><?=$lang->getLang("None")?></option>
                    </select>
                </td>
            </tr>
        </table>
    </div>
    <div class="modal-footer"> <!-- Modal footer -->
        <button class="btn btn-primary" id="editHostData" data-dismiss="modal" ><?=$lang->getLang("Save")?></button>
        <button type="button" class="btn btn-default" data-dismiss="modal">Close</button>
    </div>
</div> <!-- Add Games Modal End  -->


<table class="table table-bordered" style="width: calc(100vw - 200px);">
    <tr>
        <td class="navbar-inner" colspan="2"  ><h3 class="text-center"><?=$lang->getLang("Game Host")?></h3></td>
    </tr>

    <tr>
        <td colspan="2"  >
            <a class="fright btn btn-info btn-small" id="addNewHost" style="color:white" data-toggle="modal" data-target="#addHost" ><?=$lang->getLang("Add host")?></a>
        </td>
    </tr>

    <tr>
        <td>
            <table id="hostTable" class='display table table-striped table-hover table-bordered table-condensed no-footer' style="width: 100%">
                <thead>
                    <tr>
                        <th><?=$lang->getLang("Name")?></th>
                        <th><?=$lang->getLang("Host")?></th>
                        <th><?=$lang->getLang("Game list")?></th>
                        <th><?=$lang->getLang("Status")?></th>
                        <th><?=$lang->getLang("Host Info")?></th>
                        <th><?=$lang->getLang("Host Game Status Info")?></th>
                        <th><?=$lang->getLang("Jurisdiction Name")?></th>
                        <th><?=$lang->getLang("Operation")?></th>
                    </tr>
                </thead>
            </table>
        </td>
    </tr>
</table>