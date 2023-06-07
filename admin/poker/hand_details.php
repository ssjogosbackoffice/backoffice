<?php
if(!isset($_SESSION['admin_id'])){
    die('Please login again');
}
require_once("formobjects.php");
require_once("pokerHandsEvaluator.class.inc");
require_once("transcript.inc.php");
require_once("pokerHandHistoryGenerator.class.inc");
$handid = $_REQUEST["hand_id"];
$type=(isset($_REQUEST['type']) ?$_REQUEST['type']:'live');
$pokerHandDetails=new pokerHandHistoryGenerator();
$board = "";
$tour_id = 0;
if($type=='live') {
    $query = "select * from poker.rep_show_games_header where game_id = " . $handid;
    $result = $dbh->doCachedQuery($query, 0, true);
    while ($result->hasNext()) {
        $row = $result->next();
        $details['table']=  $row['tablename'];
        $details['game_type'] = $row['game_type'];
        $details['limit_type']= $row['limit_type'];
        $details['tour_id'] = $row['tour_id'];
        $details['board'] = $row['board_cards'];
        $details['hand_start'] = $row['hand_start'];
        $details['bet']= $row['total_pot'];
        $details['rake'] = $row['total_rake'];
        $details['smValue'] = $row['sb_value'];
        $details['bbValue'] = $row['bb_value'];
    }

    $query = "select turn_id, player_name, text_msg from poker.rep_chat_messages where game_id = " . $handid;
    $reschat = $dbh->exec($query, false, true);
    $pokerHandDetails->setChat($reschat);

    $query = "select * from poker.rep_player_events where game_id = " . $handid;
    $result = $dbh->exec($query, false, true);
    $players = array();
    while ($result->hasNext()) {
        $row = $result->next();
        $players[$row['player_id']] = $row['player_name'];
    }
    $result->resetRecPtr();
    $query = "select * from poker.rep_seats where game_id = " . $handid;
    $seats = $dbh->exec($query,false,true);

}
else{
    $query = "select sum(pch_bet) total_pot,sum(pch_rake) total_rake,min(pch_deal_id) hand_start,pch_pct_id,pch_common_cards  from poker.punter_cash_table_hands
              where pch_deal_id = " . $handid;

    $result = $dbh->doCachedQuery($query, 0, true);
    while ($result->hasNext()) {
        $row = $result->next();
        $details['table']=  $row['pch_pct_id'];
        $details['game_type'] = 'Texas Holdem & Omaha';
        $details['limit_type']='-';
        $details['tour_id'] ='-';
        $details['board'] = chunk_split($row['pch_common_cards'], 2, ' ');
        $details['hand_start'] = $row['hand_start'];
        $details['bet']= $row['total_pot']*100;
        $details['rake'] = $row['total_rake']*100;
        $details['smValue'] = '';
        $details['bbValue'] = '';

    }

    $query = "select pun_id,pun_username from punter left join  poker.punter_cash_table_hands on pch_pun_id=pun_id where pch_deal_id = " . $handid;
    $result = $dbh->exec($query, false, true);

    $players = array();
    while ($result->hasNext()) {
        $row = $result->next();
        $players[$row['pun_id']] = $row['pun_username'];
    }
    $result->resetRecPtr();
    $query = "select
          pch_pun_id as player_id,
          pch_user_cards as holecards,
          pch_cards_shown as cards_shown,
          pch_bet*100 as bets,
          pch_win*100 as won,
           '' as ip from poker.punter_cash_table_hands where pch_deal_id= " . $handid;
    $seats = $dbh->exec($query,false,true);

}
$details['hand_id'] = $handid;
$details['type']=$type;
$pokerHandDetails->setTableGeneralDetails($details);

?>
<HTML>
<HEAD>
    <TITLE>Hand #<?=$handid?></TITLE>
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/style.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap3/css/bootstrap.css" title="core_style" />
    <style>
        .card{
            float:none;
            display:table-cell;
            min-width:40px;
            height:60px;
            margin:2px;
            background-color:#FFF;
            background-size:100% 100%;
            font-size:18px;
            text-align:center;
            padding:3px;
        }
        .hold{
            background-color:#000;
        }
        .seed{
            font-size:30px;
        }
        .gameStage{
            color:#000;
            margin-top:20px;
            margin-bottom:5px;
            margin-right:10px;
            font-weight:bold;
            width:100%;
            display:block;
            padding:5px;
        }
        .transcriptHeader{
            padding:0;
        }
        .winner, a.winner{
            color:green !important;
            font-weight:bold;
            text-decoration:blink;
        }

        table{
            width: 100%;
        }
    </style>
<body>

<div align="center">
 <?=$pokerHandDetails->generateTableHeaderInfo();?>
 <?
    $pokerHandDetails->setPlayers($players);
    $pokerHandDetails->setHands($result);
    $pokerHandDetails->setSeats($seats);
    $pokerHandDetails->showPlayerHandDetails();
echo "<br>";

if ( $pokerHandDetails->hands  )
{
    $pokerHandDetails->getHandsDetailed();
}
?>
