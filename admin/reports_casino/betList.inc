<?php
check_access('betting_live_transactions', true);

$searchType = (isset($_POST['searchType']) ? $_POST['searchType'] : '0');
$date_start = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d", time());
$date_end = (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d");

function jurisdictionCheck(){
    $jur_id = $_SESSION["jurisdiction_id"];
    $return='';

    switch($_SESSION["jurisdiction_class"]){
        case "club":
            $return .= " AND c.jur_id = $jur_id";
            break;
        case "district":
            $return .= " AND d.jur_id = $jur_id";
            break;
        case "region":
            $return .= " AND r.jur_id = $jur_id";
            break;
        case "nation":
            $return .= " AND n.jur_id = $jur_id";
            break;
    }
    return $return;
}

if($_POST['betId'] != "") {

    $ticketId = $_POST['betId'];
    $sql = "SELECT DataEvento, DescrizioneEvento, IDEvento, DescrizioneTipo, DescrizioneProno, Quota FROM betting.Scommesse1 WHERE IDTicket = '$ticketId' order by DataEvento asc";

    $response =  $dbh->doCachedQuery($sql, 0);
    //var_dump($response);
    $tableArr = array();
    while ($response->hasNext()) {
        $row = $response->next();
        $section = array();
        $section[0] = $row['dataevento'];
        $section[1] = $row['descrizioneevento'];
        $section[2] = $row['idevento'];
        $section[3] = $row['descrizionetipo'];
        $section[4] = $row['descrizioneprono'];
        $section[5] = $row['quota'];
        array_push($tableArr, $section);
    }
    //var_dump($tableArr);
    die(json_encode($tableArr));
}

if ($_GET['listBets'] == 1) {

    $dateStart = $_GET['dateStart'];
    $dateEnd = $_GET['endDate'];
    $ticketId = $_GET['ticketId'];
    $username = $_GET['username'];
    $searchLike = $_GET['searchUsrType'];
    $searchStatus = $_GET['status'];
    $ticketType = $_GET['ticketType'];

    $jurisdictionSql = jurisdictionCheck();

    $dateStart .= " 00:00:00";
    $dateEnd .= " 23:59:59";

    //Create limit part of the sql querry
    $start = $_GET['start'];
    $length = $_GET['length'];
    $limitQuerry = "LIMIT $start , $length";

    //Create order by part of sql querry
    $columns = array('tstamp','idticket','pun_username', 'importo', 'tvincita', 'tstato', 'taccreditato', 'idtipoticket', 'tstampaccredito', 'lastupdate');
    $columnNumber =$_GET['order'][0]['column'];
    $ascOrDesc = $_GET['order'][0]['dir'];
    $colName = $columns[$columnNumber];
    $orderByQuerry = " ORDER BY " . $colName . " " .$ascOrDesc ;

    //set the sql for the ticket status search
    if(count($searchStatus) == 1) {
        if($searchStatus[0] == 0) {
            $statusSql = " in (0,1,2) ";
        }
        else {
            $statusValue = $searchStatus[0] - 1;
            $statusSql = " = $statusValue ";
        }
        //echo $statusSql;
    }
    else {
        $statusSql = ' in (';
        foreach( $searchStatus as $value){
            $statusSql .= $value -1 .",";
        }
        $statusSql = substr($statusSql,0, -1);
        $statusSql .= ")";
        //echo  $statusSql;
    }

    //set the sql for the ticket status search
    if(count($ticketType) == 1) {
        if($ticketType[0] == 0) {
            $ticketTypeSql = " in (0,1,2) ";
        }
        else {
            $ticketValue = $ticketType[0] - 1;
            $ticketTypeSql = " = $ticketValue ";
        }
        //echo $statusSql;
    }
    else {
        $ticketTypeSql = ' in (';
        foreach( $ticketType as $value){
            $ticketTypeSql .= $value -1 .",";
        }
        $ticketTypeSql = substr($ticketTypeSql,0, -1);
        $ticketTypeSql .= ")";
        //echo  $statusSql;
    }


//    //set the sql for the ticket type search
//    if ($ticketType == 0) {
//        $ticketTypeSql = " in (0,1,2) ";
//    } else if ($ticketType == 1) {
//        $ticketTypeSql = " = 0";
//    } else if ($ticketType == 2) {
//        $ticketTypeSql = " = 1";
//    } else if ($ticketType == 3) {
//        $ticketTypeSql = " = 2";
//    }

    $sql =  "SELECT IDTicket, TStamp, pun_id, pun_username, Importo, TStato, TAccreditato, TVincita, TStampAccredito, LastUpdate, IDTipoTicket, IPAddress, Note, IPAddress, CodiceTicket 
             FROM betting.Ticket_dev, punter, jurisdiction n, jurisdiction r, jurisdiction d, jurisdiction c ";
    $commonWhereSql = " AND pun_betting_club = c.jur_id 
                       AND n.jur_parent_id = 1 
                       AND n.jur_id = r.jur_parent_id 
                       AND r.jur_id = d.jur_parent_id 
                       AND d.jur_id = c.jur_parent_id ";

    $ticketSql = "WHERE CodiceTicket = '$ticketId' ";
    $sqlAnd = " AND TStato $ticketTypeSql  AND IDTipoTicket in (0,1,2)
                AND pun_id = IDUtente $commonWhereSql   $jurisdictionSql $orderByQuerry  $limitQuerry";
    // search by id

    $totalSql =  "SELECT COUNT(IDTicket) num_ticket, sum(Importo) tot_bet, sum(TVincita) tot_win 
        FROM betting.Ticket_dev, punter, jurisdiction n, jurisdiction r, jurisdiction d, jurisdiction c ";
    $totalTicketSql = " WHERE CodiceTicket = '$ticketId' ";
    $totalTicketAndSql = " AND TStato $ticketTypeSql 
                           AND IDTipoTicket in (0,1,2) 
                           AND pun_id = IDUtente ";
    if($ticketId != "") {
        $sql .= $ticketSql;
        $sql .= $sqlAnd;

        $totalSql .= $totalTicketSql . $commonWhereSql;
        $totalSql .= $totalTicketAndSql;

        $response = $dbh->doCachedQuery($totalSql, 0);
        //var_dump($response);
        if ($response->hasNext()) {
            $row = $response->next();
            $totals = $row['num_ticket'];
            $totalBet = $row['tot_bet'];
            $totalWin = $row['tot_win'];
        }
        $response =  $dbh->doCachedQuery($sql, 0);
        $tableArr = array();

        while ($response->hasNext()){

            $row = $response->next();
            $section = array();
            $section[0] = $row['tstamp'];
            $ticketId = $row['idticket'];
            $codiceTicket =$row['codiceticket'];

            $section[1] = "<span class='idTicket'>$codiceTicket</span>";
            $section[2] = $row['pun_username'];
            $section[3] = $row['importo'];

            $win = $row['tvincita'];

            $section[4] = "<a href='#' class=editableWin data-type='text' data-title='Edit Win' data-id='$ticketId' > $win </a>";

            $status = $row['tstato'];

            if($status == 0){
                $section[5] = "<a href='#' class='editableStatus' data-type='select' data-title='Select Status'><span class='label'>Open</span></a>";
            }else if ($status == 1) {
                $section[5] = "<a href='#' class='editableStatus' data-type='select' data-title='Select Status'><span class='label label-info'>Closed</span></a>";
            }else if ($status == 2) {
                $section[5] = "<a href='#' class='editableStatus' data-type='select' data-title='Select Status'><span class='label label-important'>Erased</span></a>";
            }

            $accredited = $row['taccreditato'];
            if ($accredited == 0) {
                $section[6] ="<a href='#' class='editAccredited' data-type='select' data-title='Select Accredited'><span class='text-error'>False</span></a>";
            }else if ($accredited == 1) {
                $section[6] = "<a href='#' class='editAccredited' data-type='select' data-title='Select Accredited'><span class='text-success'>True</span></a>";
            }

            $section[7] = $row['idtipoticket'];
            $ticketType = $row['idtipoticket'];
            if ($ticketType == 0) {
                $section[7] = "<a href='#' class='editTicketType' data-type='select' data-title='Select Ticket Type'><span class='label label-success'>Prematch</span></a>";
            } else if ($ticketType == 1){
                $section[7] = "<a href='#' class='editTicketType' data-type='select' data-title='Select Ticket Type'><span class='label label-info'>Live</span></a>";
            } else if ($ticketType == 2) {
                $section[7] = "<a href='#' class='editTicketType' data-type='select' data-title='Select Ticket Type'><span class='label label-warning'>Mixed</span></a>";
            }



            //<a href="#" id="event" data-type="combodate" data-template='D MMM YYYY  HH:mm' data-format='YYYY-MM-DD HH:mm'
            //   data-viewformat="MMM D, YYYY, HH:mm" data-title="Setup event date and time" class="editable editable-click editable-empty">Empty</a>

            $section[8] = $row['tstampaccredito'];
            $timestampAccredited = $row['tstampaccredito'];

            $section[8] = "<a href='#' class='timestampAccredited editable editable-click editable-open' data-type='combodate' data-template='D MMM YYYY  HH:mm:ss' data-format='YYYY-MM-DD HH:mm:ss' data-title='Set a new timestamp for accredited'>$timestampAccredited</a>";

            if ($row['tstampaccredito'] == '1999-01-01 00:00:00' ) {
                $section[8] = "<a href='#' class='timestampAccredited editable editable-click editable-open' data-type='combodate' data-template='D MMM YYYY  HH:mm:ss' data-format='YYYY-MM-DD HH:mm:ss' data-title='Set a new timestamp for accredited'>-</a>";
            }
                $section[9] = $row['lastupdate'];

            $section[10] = "<button class='btn btn-small viewBetDetails' data-id='$ticketId'>Details</button>";
            array_push($tableArr,$section);
        }

        $output= array(
            "draw"            => intval($_GET['draw']),
            "recordsTotal"    => intval($totals),
            "recordsFiltered" => intval($totals),
            "data"            => $tableArr,
            "totBet"          => $totalBet,
           "totalWin"         => $totalWin
        );

        die(json_encode($output));
    }
    //search by date
    else {
        $sql="
            SELECT IDTicket, TStamp, pun_id, pun_username, Importo, TStato, TAccreditato, TVincita, TStampAccredito, LastUpdate, IDTipoTicket, IPAddress, Note, IPAddress, CodiceTicket 
                   FROM betting.Ticket_dev, punter, jurisdiction n, jurisdiction r, jurisdiction d, jurisdiction c 
                   WHERE pun_id = IDUtente 
                   $commonWhereSql        
            ";
        $totalSql ="
            SELECT COUNT(IDTicket) num_ticket, sum(Importo) tot_bet, sum(TVincita) tot_win 
                   FROM betting.Ticket_dev, punter, jurisdiction n, jurisdiction r, jurisdiction d, jurisdiction c 
                   WHERE pun_id = IDUtente 
                   $commonWhereSql
            ";

        if($_GET['search']['value'] != "") {
            $searchableColumn1 = "CodiceTicket";
            $searchableColumn2 = "pun_username";
            $searchKeyword = $dbh->prepareString($_GET['search']['value']);
            $searchKeyword = trim($searchKeyword, "'");
            $sqlLikeStmnt = " and ($searchableColumn1 LIKE '%$searchKeyword%' or $searchableColumn2 LIKE '%$searchKeyword%')";

 // end if
            $sql .= $sqlLikeStmnt;
        }
        if($username != "") {
            $sql .= " AND pun_username " . (($searchLike == 0 ? " = '" : " like '%")) . $username . (($searchLike == 0 ? "'" : "%'"));
            $totalSql .= " AND pun_username " . (($searchLike == 0 ? " = '" : " like '%")) . $username . (($searchLike == 0 ? "'" : "%'"));
        }
        $sql .= " AND TStamp BETWEEN '$dateStart' AND '$dateEnd' 
                              AND TStato $statusSql
                              AND IDTipoTicket $ticketTypeSql $jurisdictionSql  $orderByQuerry $limitQuerry";
        $totalSql .= " AND TStamp BETWEEN '$dateStart' AND '$dateEnd' 
                                  $sqlLikeStmnt
                                 AND TStato $statusSql 
                                 AND IDTipoTicket $ticketTypeSql ";
        //var_dump($sql);
        $response = $dbh->doCachedQuery($totalSql, 0);
        $total = 0;
        if ($response->hasNext()) {
            $row = $response->next();
            $total = $row['num_ticket'];
            $totalBet = $row['tot_bet'];
            $totalWin = $row['tot_win'];
        }

       $response =  $dbh->doCachedQuery($sql, 0);
       $tableArr = array();
       while ($response->hasNext()){

           $row = $response->next();
           $section = array();
           $section[0] = $row['tstamp'];
           $ticketId = $row['idticket'];
           $codiceTicket =$row['codiceticket'];

           $section[1] = "<span class='idTicket'>$codiceTicket</span>";
           $section[2] = $row['pun_username'];
           $section[3] = $row['importo'];
           $win = $row['tvincita'];

           $section[4] = "<a href='#' class='editableWin' data-type='text' data-title='Edit Win' data-id='$ticketId' > $win </a>";

           $status = $row['tstato'];
           if($status == 0){
               $section[5] = "<a href='#' class='editableStatus' data-type='select' data-title='Select Status'><span class='label'>Open</span></a>";
           }else if ($status == 1) {
               $section[5] = "<a href='#' class='editableStatus' data-type='select' data-title='Select Status'><span class='label label-info'>Closed</span></a>";
           }else if ($status == 2) {
               $section[5] = "<a href='#' class='editableStatus' data-type='select' data-title='Select Status'><span class='label label-important'>Erased</span></a>";
           }


           $accredited = $row['taccreditato'];
           if ($accredited == 0) {
               $section[6] ="<a href='#' class='editAccredited' data-type='select' data-title='Select Accredited'><span class='text-error'>False</span></a>";
           }else if ($accredited == 1) {
               $section[6] = "<a href='#' class='editAccredited' data-type='select' data-title='Select Accredited'><span class='text-success'>True</span></a>";
           }

           $section[7] = $row['idtipoticket'];

           //editTicketType
           $ticketType = $row['idtipoticket'];
           if ($ticketType == 0) {
               $section[7] = "<a href='#' class='editTicketType' data-type='select' data-title='Select Ticket Type'><span class='label label-success'>Prematch</span></a>";
           } else if ($ticketType == 1){
               $section[7] = "<a href='#' class='editTicketType' data-type='select' data-title='Select Ticket Type'><span class='label label-info'>Live</span></a>";
           } else if ($ticketType == 2) {
               $section[7] = "<a href='#' class='editTicketType' data-type='select' data-title='Select Ticket Type'><span class='label label-warning'>Mixed</span></a>";
           }


           //<a href="#" id="event" data-type="combodate" data-template="D MMM YYYY  HH:mm" data-format="YYYY-MM-DD HH:mm" data-viewformat='MMM D, YYYY, HH:mm'
           //   data-viewformat="MMM D, YYYY, HH:mm" data-title="Setup event date and time" class="editable editable-click editable-empty">Empty</a>

           $timestampAccredited = $row['tstampaccredito'];
           $section[8] = "<a href='#' class='timestampAccredited editable editable-click editable-open' data-type='combodate' data-template='D MMM YYYY  HH:mm:ss' data-format='YYYY-MM-DD HH:mm:ss' data-title='Set a new timestamp for accredited'>$timestampAccredited</a>";

           if ($row['tstampaccredito'] == '1999-01-01 00:00:00' ) {
               $section[8] = "<a href='#' class='timestampAccredited editable editable-click editable-open' data-type='combodate' data-template='D MMM YYYY  HH:mm:ss' data-format='YYYY-MM-DD HH:mm:ss' data-title='Set a new timestamp for accredited'></a>";
           }

           $section[9] = $row['lastupdate'];

           $section[10] = "<button class='btn btn-small viewBetDetails' data-id='$ticketId'>Details</button>";
           array_push($tableArr,$section);
       }

       $output= array(
           "draw"            => intval($_GET['draw']),
           "recordsTotal"    => intval($total),
           "recordsFiltered" => intval($total),
           "data"            => $tableArr,
           "totBet"       =>  $totalBet,
           "totalWin"     =>  $totalWin
       );

       die(json_encode($output));
   }
}

if($_POST['pk'] == 'updateWin') {
    $newWin = $_POST['value'];
    $tktId = $_POST['tcktId'];

    //Also update the timestamp on TStampAccredito
    //get current time
    $time = new DateTime();
    $timeNow = $time->format('Y-m-d H:i:s');

    $updateWinSQL = "UPDATE betting.Ticket_dev SET LastUpdate = '$timeNow', TVincita = '$newWin'  where IDTicket = $tktId";
    $updatedHost = $dbh->exec($updateWinSQL);
    $updatedHost = "" .$updatedHost;
    die($updatedHost);

}

if($_POST['pk'] == 'updateStatus'){
    $newStatus = $_POST['value'];
    $tktId = $_POST['tcktId'];

    $time = new DateTime();
    $timeNow = $time->format('Y-m-d H:i:s');

    $updateStatusSql = "UPDATE betting.Ticket_dev SET LastUpdate = '$timeNow', TStato = '$newStatus' where IDTicket = '$tktId'"  ;
    $udpatedStatus = $dbh->exec($updateStatusSql);
    $udpatedStatus = "" .$udpatedStatus;
    die($udpatedStatus);
}

if($_POST['pk'] == 'updateAccredited'){

    $isAccredited = $_POST['value'];
    $tktId = $_POST['tcktId'];

    $time = new DateTime();
    $timeNow = $time->format('Y-m-d H:i:s');

    $updateAccreditedSql = "UPDATE betting.Ticket_dev SET LastUpdate = '$timeNow', TAccreditato = '$isAccredited' where IDTicket = '$tktId'"  ;
    $udpatedAccredited = $dbh->exec($updateAccreditedSql);
    $udpatedAccredited = "" .$udpatedAccredited;
    die($udpatedAccredited);
}

if($_POST['pk'] == 'updateTicketType') {

    $ticketType = $_POST['value'];
    $tktId = $_POST['tcktId'];

    $time = new DateTime();
    $timeNow = $time->format('Y-m-d H:i:s');

    $updateTicketTypeSql = "UPDATE betting.Ticket_dev SET LastUpdate = '$timeNow', IDTipoTicket = '$ticketType' where IDTicket = '$tktId'"  ;
    $udpatedTicketType = $dbh->exec($updateTicketTypeSql);
    $udpatedTicketType = "" .$udpatedTicketType;
    die($udpatedTicketType);
}

if($_POST['pk'] == 'updateTSAccredited'){

    $updatedAccrdDate = $_POST['value'];
    $tktId = $_POST['tcktId'];
    $updateAcrdtDate = "UPDATE betting.Ticket_dev SET TStampAccredito = '$updatedAccrdDate' where IDTicket = '$tktId'"  ;
    $updateDate = $dbh->exec($updateAcrdtDate);
    $updateDate = "" .$updateDate;
    die($updateDate);

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
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/bootstrap-editable/bootstrap-editable/css/bootstrap-editable.css" >
<script src="<?= secure_host ?>/media/bootstrap/bootstrap-editable/bootstrap-editable/js/bootstrap-editable.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/js/moment.js" type="text/javascript"></script>


<?php

?>

<script>

 $(function() {
     
     var disableAllStatus  = $('.unckecAllStatus');
     var selectAllStatus = $('#inlineCheckbox1');
     disableAllStatus.on( 'change', function() {
         if (disableAllStatus.is(":checked"))
         {
             selectAllStatus.prop('checked', false);
         }
     });
     selectAllStatus.on('change', function() {
         if (selectAllStatus.is(":checked")) {
             disableAllStatus.prop('checked', false);
         }
     });

     var disableAllTicket  = $('.uncheckAllTicket');
     var selectAllTicket = $('#inlineTicketCheckbox');
     disableAllTicket.on( 'change', function() {
         if (disableAllTicket.is(":checked"))
         {
             selectAllTicket.prop('checked', false);
         }
     });
     selectAllTicket.on('change', function() {
         if (selectAllTicket.is(":checked")) {
             disableAllTicket.prop('checked', false);
         }
     });





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

 var statusVals = [0];
 var ticketVals =[0];

 // displays game status details in a modal
 $(document).on('click',"#searchTicket", function() {
     statusVals = [];
     ticketVals  =[];

     //getting the status checkbox values
     var statusCheckboxes = document.getElementsByClassName('statusCheckbox');
     console.log(statusCheckboxes);
     for (var i=0, n=statusCheckboxes.length;i<n;i++)
     {
         if (statusCheckboxes[i].checked)
         {
             statusVals.push(statusCheckboxes[i].value);
         }
     }
     if (statusVals.length == 0) {
         statusVals = [0];
         $('#inlineCheckbox1').prop('checked', true);
     }
     //getting the status checkbox values
     var ticketCheckboxes = document.getElementsByClassName('ticketCheckbox');
     for (var i = 0, n = ticketCheckboxes.length; i < n; i++)
     {
         if (ticketCheckboxes[i].checked)
         {
             ticketVals.push(ticketCheckboxes[i].value);
         }
     }
     if (ticketVals.length == 0) {
         ticketVals = [0];
         $('#inlineTicketCheckbox').prop('checked', true);
     }
     hostTable.ajax.reload();
 });


 $(document).on('click',".hasDatepicker", function() {
     $('#ticketId').attr('readonly',true);
     $(".hasDatepicker").attr('readonly',false);
 });
 $(document).on('click',"#ticketId", function() {
     $('.unckecAllStatus').prop('checked', false);
     $('.uncheckAllTicket').prop('checked', false);
     $('#inlineCheckbox1').prop('checked', true);
     $('#inlineTicketCheckbox').prop('checked', true);
     $('.hasDatepicker').attr('readonly',true);
     $('#username').attr('readonly',true);
     $(this).attr('readonly',false);
 });
 $(document).on('click',"#username", function() {
     $('.hasDatepicker').attr('readonly',false);
     $('#ticketId').attr('readonly',true);
     $(this).attr('readonly',false);
 });


    $(document).on('click','.close', function() {

        var hostId = $(this).attr('data-id');
        var anchor = $(this).siblings('a');
        $(anchor.attr('href')).remove();
        $(this).parent().remove();
        $('#' + hostId).remove();
        $("#navTabList a:last").tab("show");
    });


 $(document).on('click','.viewBetDetails', function() {

     var betId = $(this).attr('data-id');
     //make an ajax request to server and return the data needed to populate the table
     $.ajax({
         url: 'reports_casino/betList.inc',
         method: "POST",
         data: {
             betId : betId
         }
     }).done(function (data) {
         if (data ) {
             obj = JSON.parse(data);
             //console.log(obj);
             var tableBody = "";
            for (var i = 0; i < obj.length ; i++){
                var myVar = obj[i];
                //console.log(myVar);
                tableBody += "<tr>";
                for (var j = 0; j < obj[i].length; j++){
                    var row = "<td>" + obj[i][j] + "</td>";
                    tableBody += row;
                    //console.log(temp);
                }
                tableBody += "</tr>";
            }
             var newNavPill = "<li><a data-toggle='tab' href='#" + betId + "'>" + betId +" Details <span data-id='"+ betId + "' class='close'>X</span></a></li>";
             var newNavPillContent = "<div id='" + betId + "' class='tab-pane fade'><h3> Ticket  " + betId +" Details </h3><table class ='table table-condensed '><thead>" +
                 "<tr><th>Start Date</th><th>Event</th><th>Event Id</th><th>OddType</th><th>Betted</th></tr></thead>" + tableBody + "</table></div>";
             
             $('#navTabList').append(newNavPill);
             $('#navTabContent').append(newNavPillContent);
//             $("#searchBet").scrollTop(0);

             $(window).scrollTop(0);
         }
     }).fail(function (error) {
         console.log(error);
     });
     
 });
 $(document).ready(function () {

     // Initializing the datatable
     hostTable = $('#betTable').DataTable({
         "processing": true,
         "serverSide": true,
         "ajax": {
             "url": "reports_casino/betList.inc",
             "data": function (d) {
                 d.listBets = "1";
                 d.dateStart = $("#date_start").val();
                 d.endDate = $("#date_end").val();
                 d.ticketId = $("#ticketId").val();
                 d.username = $("#username").val();
                 d.searchUsrType =  $('input[name=searchType]:checked').val();
                 d.status = statusVals;
                 d.ticketType = ticketVals;
             }
         },
         'sPaginationType': "full_numbers",
         "columnDefs": [
             {
                 "orderable": false, "targets": 10
             }
         ],
         "fnDrawCallback": function (nRow, aData, iDisplayIndex, iDisplayIndexFull) {

             // Displays the sum of the win and sum of the bets
             if(nRow.json.totBet == null ){
                 nRow.json.totBet = 0;
             }
             if (nRow.json.totalWin == null) {
                 nRow.json.totalWin = 0;
             }
             $('#totalBet').empty().html(nRow.json.totBet);
             $('#totalWin').empty().html(nRow.json.totalWin);

             $('.editableWin').editable({
                 url:'/reports_casino/betList.inc',
                 send:'always',
                 pk:'updateWin',
                 validate: function(value) {
                     if ($.isNumeric(value) == '') {
                         return "Only numbers are allowed";
                     }
//                     value = parseFloat(Math.round(value * 100) / 100).toFixed(2);
                     //console.log(myVal);
                 },
                 params: function(params) {
                     params.tcktId = $(this).closest('tr').find('.idTicket').html();
                     //console.log(params);
                     return params;
                 },
                 success: function(response, newValue) {
                     hostTable.ajax.reload();
                 },
                 error: function(response, newValue) {
                     jAlert('An error has occured');
                 }
             });

             $('.editableStatus').editable({
                 source: [{value: 0, text: "Open"}, {value: 1, text: 'Closed'}, {value: 2, text: 'Erased'}],
                 url:'/reports_casino/betList.inc',
                 send:'always',
                 pk:'updateStatus',
                 select2: {
                     width: 200,
                     placeholder: 'Select status',
                     allowClear: true
                 },
                 params: function(params) {
                     params.tcktId = $(this).closest('tr').find('.idTicket').html();
                     return params;
                 },
                 success: function(response, newValue) {
                     hostTable.ajax.reload();
                 },
                 error: function(response, newValue) {
                     jAlert('An error has occured');
                 }
             });


             $('.editAccredited').editable({
                 source: [{value: 0, text: "False"}, {value: 1, text: 'True'}],
                 url:'/reports_casino/betList.inc',
                 send:'always',
                 pk:'updateAccredited',
                 params: function(params) {
                     params.tcktId = $(this).closest('tr').find('.idTicket').html();
                     return params;
                 },
                 success: function(response, newValue) {
                     hostTable.ajax.reload();
                 },
                 error: function(response, newValue) {
                     jAlert('An error has occured');
                 }
             });

             $('.editTicketType').editable({
                 source: [{value: 0, text: "Prematch"}, {value: 1, text: 'Live'}, {value: 2, text: 'Mixed'}],
                 url:'/reports_casino/betList.inc',
                 send:'always',
                 pk:'updateTicketType',
                 params: function(params) {
                     params.tcktId = $(this).closest('tr').find('.idTicket').html();
                     return params;
                 },
                 success: function(response, newValue) {
                     hostTable.ajax.reload();
                 },
                 error: function(response, newValue) {
                     jAlert('An error has occured');
                 }
             });


             $('.timestampAccredited').editable({
                 placement: 'left',placement: 'left',
                 combodate: {
                     minuteStep: 1,
                     maxYear: 2020,
                     firstItem: 'name',
                 },
                 url:'/reports_casino/betList.inc',
                 send:'always',
                 pk:'updateTSAccredited',
                 params: function(params) {
                     params.tcktId = $(this).closest('tr').find('.idTicket').html();
                     return params;
                 },
                 success: function(response, newValue) {
                     hostTable.ajax.reload();
                 },
                 error: function(response, newValue) {
                     jAlert('An error has occured');
                 }

             });

         }
     });
 });
</script>


<style>
    .nav-tabs > li .close {
        margin: -2px 0 0 10px;
        font-size: 13px;
    }
</style>

<div class="container-fluid ">

    <ul class="nav nav-tabs" id="navTabList">
        <li class="active"><a data-toggle="tab" href="#searchBet">Search Bet List</a></li>
</ul>

<div class="tab-content" id="navTabContent">
    <div id="searchBet" class="tab-pane fade in active">
        <table class="table table-bordered" style="width:100%">
            <tr>
                <td class="navbar-inner" colspan="2"  >
                    <h3 class="text-center"><?=$lang->getLang("Bet List")?></h3>
                </td>
            </tr>
            <tr>
                <td colspan="2">
                    <div class="well">
                        <div class="row">

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
                                            </div></td>
                                    </tr>
                                    <tr>
                                        <td><div class='label label-inverse'><?=$lang->getLang("Codice ticket")?></div></td>
                                        <td> <div class="controls">
                                                <div class="input-prepend">
                                                    <span class="add-on"><i class="icon-list"></i></span>
                                                    <input type='text' placeholder="Search id" id="ticketId">
                                                </div>
                                            </div>
                                        </td>
                                    </tr>
                                    </tbody>
                                </table>
                            </div>
                            <div class="span4">
                                <div class='label label-inverse'><?=$lang->getLang("Username")?></div>
                                <br>
                                <div class="controls">
                                    <div class="input-prepend">
                                        <span class="add-on"><i class="icon-user"></i></span>
                                        <input type='text'  name='username' id='username' placeholder="Type in username">
                                    </div>
                                </div>
                                <br />
                                <div class='label label-inverse'><?=$lang->getLang("Username search")?></div>
                                    <br />
                                    <input type="radio" name="searchType" class="yes" id="yes" <?=$searchType=='0'?'checked':''?> value="0" />
                                    <input type="radio" name="searchType" class="alternative" id="no"  <?=$searchType=='1'?'checked':''?> value="1" />
                                    <div class="switch grey">
                                    <label class="labelYes" for="yes"><?=$lang->getLang("Exactly")?></label>
                                    <label class="labelNo" for="no"><?=$lang->getLang("Contains")?></label>
                                    </div>
                                <br />
                                <div class='label label-inverse'><?=$lang->getLang("Status")?></div>
                                <div class="controls">
                                    <label class="checkbox inline">
                                        <input type="checkbox" class="statusCheckbox" id="inlineCheckbox1" value="0" checked> All
                                    </label>
                                    <label class="checkbox inline">
                                        <input type="checkbox" class="statusCheckbox unckecAllStatus" value="1"> Open
                                    </label>
                                    <label class="checkbox inline">
                                        <input type="checkbox" class="statusCheckbox unckecAllStatus" value="2"> Closed
                                    </label>
                                    <label class="checkbox inline">
                                        <input type="checkbox" class="statusCheckbox unckecAllStatus"  value="3"> Erased
                                    </label>
                                </div><br/>

                                <div class='label label-inverse'><?=$lang->getLang("Ticket Type")?></div>
                                <div class="controls">
                                    <label class="checkbox inline">
                                        <input type="checkbox" class="ticketCheckbox " id="inlineTicketCheckbox" value="0" checked> All
                                    </label>
                                    <label class="checkbox inline">
                                        <input type="checkbox" class="ticketCheckbox uncheckAllTicket" value="1"> Prematch
                                    </label>
                                    <label class="checkbox inline">
                                        <input type="checkbox" class="ticketCheckbox uncheckAllTicket" value="2"> Live
                                    </label>
                                    <label class="checkbox inline">
                                        <input type="checkbox" class="ticketCheckbox uncheckAllTicket"  value="3"> Mixed
                                    </label>
                                </div>

                            </div>
                        </div>
                        <div class="row">
                            <br/>
                            <span class="span2"> <button class="btn btn-primary" id="searchTicket"><?=$lang->getLang("Search")?></button></span>
                        </div><br />
                    </div>
                </td>
            </tr>

            <tr>
                <td>
                    <table class='display table table-striped table-hover table-bordered table-condensed no-footer'>
                        <thead>
                        <tr>
                            <th>Total Bet</th>
                            <th>Total Win</th>
                        </tr>
                        </thead>
                        <tbody>
                        <tr>
                            <td id="totalBet">0</td>
                            <td id="totalWin">0</td>
                        </tr>
                        </tbody>
                    </table>
                </td>
            </tr>
            <tr>
                <td>
                    <table id="betTable" class='display table table-striped table-hover table-bordered table-condensed no-footer' style="width: 100%">
                        <thead>
                            <tr>
                                <th><?=$lang->getLang("Date")?></th>
                                <th><?=$lang->getLang("Codice Ticket")?></th>
                                <th><?=$lang->getLang("User Name")?></th>
                                <th><?=$lang->getLang("Bet")?></th>
                                <th><?=$lang->getLang("Win")?></th>
                                <th><?=$lang->getLang("Status")?></th>
                                <th><?=$lang->getLang("Accredited")?></th>
                                <th><?=$lang->getLang("Ticket Type")?></th>
                                <th><?=$lang->getLang("TimeStamp Accredited")?></th>
                                <th><?=$lang->getLang("Last Update")?></th>
                                <th><?=$lang->getLang("Details")?></th>
                            </tr>
                        </thead>
                    </table>
                </td>
            </tr>
        </table>
    </div>
</div>
</div>

