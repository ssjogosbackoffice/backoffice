<?php
include_once('StringTokenizer.class.php');
ini_set("include_path", ini_get("include_path") . ":" . WEB_ROOT . "/admin/customers");
include_once('transcript.php');

function getCard($cardid){
  /*$cardid = trim($cardid);
  if(strstr($cardid, ' ')){
    $cardid = str_replace(' ','' , $cardid);
  }
  $card_style ="";
  $CardArr = str_split($cardid, 2);

  $len_CardArr = count($CardArr);

  if(is_null($cardid) || empty($cardid) || $len_CardArr < 1 || $cardid == 'null'){
    return '<span class="card back">&nbsp;</span><span class="card back">&nbsp;</span>';
  }*/

  //for ($i = 0; $i < $len_CardArr ; $i++){
    //$firstCard   = getCardValue($CardArr[$i]);
    $firstCard   = getCardValue($cardid);
    $colorFirst  = $firstCard[1];
    if($firstCard[2]==''){
    	$card_style .= "<span class='card' style='color:" . $colorFirst . "'><img src='../media/images/joker.jpg'  style='width:40px;height:60px'/></span>";
    }
    else{
    $card_style .= "<span class='card' style='color:" . $colorFirst . "'>" . $firstCard[2] . "<br/> <span class='seed'>" .$firstCard[0] . "</span></span>";
    }
  //}

  /*$firstCard = $CardArr[0];
  $secondCard = $CardArr[1];
  $firstCard = getCardValue($firstCard);
  $secondCard = getCardValue($secondCard);
  $colorFirst = $firstCard[1];
  $colorSecond = $secondCard[1];*/
  return  $card_style;//"<span style='color:$colorFirst'> ".$firstCard[0].$firstCard[2]." </span><span style='color:$colorSecond'> ".$secondCard[0].$secondCard[2]. "</span>";
}

function getCardValue($card){

  $cardType = str_split($card);
  
  if(count($cardType) == 2){
    $card_seed = $cardType[1];
    $card_numb = $cardType[0];
  }else if(count($cardType) == 3){
    $card_seed = $cardType[2];
    $card_numb = $cardType[0].$cardType[1];
  }
  $seeds = array("&clubs;","&diams;","&hearts;","&spades;","&#9786;");
  switch ($card_seed){
    case "c":
      $card_seed = $seeds[0];
      $color = "#000000";
      break;
    case "d":
      $card_seed = $seeds[1];
      $color = "#FF0000";
      break;
    case "h":
      $card_seed = $seeds[2];
      $color = "#FF0000";
      break;
    case "s":
      $card_seed = $seeds[3];
      $color = "#000000";
      break;
    case "p":
    	$card_seed = $seeds[4];
    	$color = "#FF0000";
    break;
    	
      
    default:
      break;
  }
  $card_numb = $card_numb + 1;
  switch($card_numb){
    case "11":
      $card_numb="J";
      break;
    case "12":
      $card_numb="Q";
      break;
    case "13":
      $card_numb="K";
      break;
    case "1":
      $card_numb="A";
      if ($card_seed=="&#9786;"){
      	$card_numb="";
      }
      break;
    default:
      break;
  }
  $cardRet = array();
  $cardRet[0] = $card_seed;
  $cardRet[1] = $color;
  $cardRet[2] = $card_numb;
  return $cardRet;
}

function getResultHand($handValue){
  global $hand_name, $rank_name, $rankPlu_name;
  $Poker_High = 0;
  $Poker_Pair = 1;
  $Poker_TwoPair = 2;
  $Poker_ThreeKind = 3;
  $Poker_Straight = 4;
  $Poker_Flush = 5;
  $Poker_FullHouse = 6;
  $Poker_FourKind = 7;
  $Poker_StraightFlush = 8;
  $Poker_FiveKind = 9;
  $Poker_Num_Hands = 10;
  $Poker_Num_Ranks = 13;
  $Id_Group_Size = ($Poker_Num_Ranks*$Poker_Num_Ranks*$Poker_Num_Ranks*$Poker_Num_Ranks*$Poker_Num_Ranks);

  $type  = floor(abs($handValue / $Id_Group_Size));
  $ident = floor(abs($handValue % $Id_Group_Size));

  $ident2 = "";
  
  switch ($type){
    case $Poker_High:
      $ident =floor(abs($ident / ($Poker_Num_Ranks * $Poker_Num_Ranks * $Poker_Num_Ranks * $Poker_Num_Ranks)));
      $string =sprintf($hand_name[$type], $rank_name[$ident]);
      if($ident == 0){
        $string = '<span style="color:#CCC">No valid Hand Rank</span>';
      }
      
      
      break;
    case $Poker_Flush:
      $ident =floor(abs($ident / ($Poker_Num_Ranks * $Poker_Num_Ranks * $Poker_Num_Ranks * $Poker_Num_Ranks)));
      $string = sprintf($hand_name[$type], $rank_name[$ident]);
      break;
    case $Poker_Pair:
      $ident = floor(abs($ident / ($Poker_Num_Ranks * $Poker_Num_Ranks * $Poker_Num_Ranks)));
      $string = sprintf($hand_name[$type], $rankPlu_name[$ident]);
      break;
    case $Poker_TwoPair:
      $ident2 = floor(abs($ident / ($Poker_Num_Ranks * $Poker_Num_Ranks)));
      $ident  = floor(abs(($ident % ($Poker_Num_Ranks * $Poker_Num_Ranks)) / $Poker_Num_Ranks));
      

      
      $string = sprintf($hand_name[$type], $rankPlu_name[$ident],$rankPlu_name[$ident2]);
      
      break;
    case $Poker_ThreeKind:
      $ident = floor(abs($ident / ($Poker_Num_Ranks * $Poker_Num_Ranks)));
      $string = sprintf($hand_name[$type], $rankPlu_name[$ident]);
      break;
    case $Poker_FullHouse:
      $ident  = floor(abs($ident / $Poker_Num_Ranks));
      $ident2 = floor(abs($ident %  $Poker_Num_Ranks));
      $string = sprintf($hand_name[$type], $rankPlu_name[$ident],$rankPlu_name[$ident2]);
      break;
    case $Poker_FourKind:
      $ident  = floor(abs($ident / $Poker_Num_Ranks));
      $string = sprintf($hand_name[$type], $rankPlu_name[$ident]);
      break;
    case $Poker_Straight:
      $string = sprintf($hand_name[$type], $rank_name[$ident]);
      break;
    case $Poker_StraightFlush:
      $string = $hand_name[$type];
      break;
    case $Poker_FiveKind:
      $string = sprintf($hand_name[$type], $rank_name[$ident]);
      break;
  }
  
  return $string;
}


define('TRANSCRIPT_ACTION_GAMESTAGE',    'G');
define('TRANSCRIPT_ACTION_DEALCARDS',    'H');
define('TRANSCRIPT_ACTION_FLOP',         'F');
define('TRANSCRIPT_ACTION_TURN',         'T');
define('TRANSCRIPT_ACTION_RIVER',        'R');
define('TRANSCRIPT_ACTION_ANTE',         'A');
define('TRANSCRIPT_ACTION_BLIND',        'b');
define('TRANSCRIPT_ACTION_BBLIND',       'B');

define('TRANSCRIPT_ACTION_CHECKS',       'c');
define('TRANSCRIPT_ACTION_BETS',         'e');
define('TRANSCRIPT_ACTION_CALLS',        'C');
define('TRANSCRIPT_ACTION_RAISES',       'r');
define('TRANSCRIPT_ACTION_FOLDS',        'f');
define('TRANSCRIPT_ACTION_ALLINS',       'a');

define('TRANSCRIPT_ACTION_SHOWCARDS',    's');
define('TRANSCRIPT_ACTION_AWAY',         'y');
define('TRANSCRIPT_ACTION_AWAYBACK',     'Y');
define('TRANSCRIPT_ACTION_TIMEOUT',      't');
define('TRANSCRIPT_ACTION_POT',          'P');
define('TRANSCRIPT_ACTION_WINNERS',      'W');
define('TRANSCRIPT_ACTION_REFUND',       'M');
define('TRANSCRIPT_ACTION_FORCEDSTANDUP','x');
define('TRANSCRIPT_ACTION_PLAYERS',      'P');
define('TRANSCRIPT_ACTION_RAKE',         'k');


/**
  y away
  Y torno
  t timeout
  P pot
  W vincitori
  M restituiti i soldi
 */
function getReadableTranscriptHeader($transcript_in, $realmoney=true){
  $multiplier = 100;
  if(!$realmoney){
    $multiplier = 1;
  }

  $transcript  = "";
  $transcript_t = new StringTokenizer($transcript_in, ':');
  while($row = $transcript_t->nextToken()){
    $addToTranscript = '';
    $action  = substr($row, 0, 1);
    $details = substr($row, 1);
    switch($action){
      case TRANSCRIPT_ACTION_PLAYERS:
        //5;totti;4200,4;potenza;3800,GN
        //$addToTranscript .= $details;
        $players = explode(',', $details);
        foreach ($players as $player){
          if(!empty($player)){
            $plr = explode(';', $player);
            $addToTranscript .= '<div>' . sprintf(DETHAND_ACT_SEAT, $plr[1], getInDollars($plr[2] / $multiplier)) . '</div>';
          }
        }
        break;
    }
    $transcript .= '<div>' . $addToTranscript . '</div>';
  }
  return $transcript;
}

function getReadableTranscript($transcript_in, $realmoney=true){
  $gamestages = array(
  'N' => 'New Hand',
  'B' => 'Anteblind',
  'P' => 'Preflop',
  'F' => 'Flop',
  'T' => 'Turn',
  'R' => 'River',
  'S' => 'Showdown'
  );

  $multiplier = 100;
  if(!$realmoney){
    $multiplier = 1;
  }

  $transcript  = "";

  $transcript_t = new StringTokenizer($transcript_in, ':');
  $counter = 0;
  while($row = $transcript_t->nextToken()){
    $addToTranscript = '';
    $action  = substr($row, 0, 1);
    $details = substr($row, 1);
    switch($action){
      case TRANSCRIPT_ACTION_GAMESTAGE:
        if($details != 'N'){
          $addToTranscript .= '<span class="gameStage">' . $gamestages[$details] . '</span>';
        }
        break;
      case TRANSCRIPT_ACTION_ANTE:
        $playersDetail = explode(',',$details);
        $addToTranscript .= sprintf(DETHAND_ACT_ANTE, $playersDetail[0], getInDollars($playersDetail[1] / $multiplier));
        break;
      case TRANSCRIPT_ACTION_BLIND:
        $playersDetail = explode(',',$details);
        $addToTranscript .= sprintf(DETHAND_ACT_BLIND, $playersDetail[0], getInDollars($playersDetail[1] / $multiplier));
        break;
      case TRANSCRIPT_ACTION_BBLIND:
        $playersDetail = explode(',',$details);
        $addToTranscript .= sprintf(DETHAND_ACT_BBLIND, $playersDetail[0], getInDollars($playersDetail[1] / $multiplier));
        break;
      case TRANSCRIPT_ACTION_DEALCARDS:
        $addToTranscript .= DETHAND_ACT_DEALCARDS;
        break;
      case TRANSCRIPT_ACTION_CALLS:
        $playersDetail = explode(',',$details);
        $addToTranscript .= sprintf(DETHAND_ACT_DEALCALLS, $playersDetail[0], getInDollars($playersDetail[1] / $multiplier));
        break;
      case TRANSCRIPT_ACTION_CHECKS:
        $addToTranscript .= sprintf(DETHAND_ACT_DEALCHECKS, $details);
        break;
      case TRANSCRIPT_ACTION_FOLDS:
        $addToTranscript .= sprintf(DETHAND_ACT_FOLDS, $details);
        break;
      case TRANSCRIPT_ACTION_FLOP:
        $addToTranscript .= sprintf(DETHAND_ACT_FLOP, getCard($details));
        break;
      case TRANSCRIPT_ACTION_TURN:
        $addToTranscript .= sprintf(DETHAND_ACT_TURN, getCard($details));
        break;
      case TRANSCRIPT_ACTION_BETS:
        $playersDetail = explode(',',$details);
        $addToTranscript .= sprintf(DETHAND_ACT_BETS, $playersDetail[0], getInDollars($playersDetail[1] / $multiplier));
        break;
      case TRANSCRIPT_ACTION_RAISES:
        $playersDetail = explode(',',$details);
        $addToTranscript .= sprintf(DETHAND_ACT_RAISES, $playersDetail[0], getInDollars($playersDetail[1] / $multiplier));
        break;
      case TRANSCRIPT_ACTION_ALLINS:
        $playersDetail = explode(',',$details);
        $addToTranscript .= sprintf(DETHAND_ACT_ALLINS, $playersDetail[0], getInDollars($playersDetail[1] / $multiplier));
        break;
      case TRANSCRIPT_ACTION_SHOWCARDS:
        $playersDetail = explode(',',$details);
        $addToTranscript .= sprintf(DETHAND_ACT_SHOWCARDS, $playersDetail[0], getCard($playersDetail[1]), getResultHand($playersDetail[2]));
        break;
      case TRANSCRIPT_ACTION_RIVER:
        $addToTranscript .= sprintf(DETHAND_ACT_RIVER, getCard($details));
        break;
      case TRANSCRIPT_ACTION_AWAY:
        $addToTranscript .= sprintf(DETHAND_ACT_AWAY, $details);
        break;
      case TRANSCRIPT_ACTION_AWAYBACK:
        $addToTranscript .= sprintf(DETHAND_ACT_AWAYBACK, $details);
        break;
      case TRANSCRIPT_ACTION_REFUND:
        $playerDetail = explode(',', $details);
        $addToTranscript .= sprintf(DETHAND_ACT_MONEYBACK, $playerDetail[0], getInDollars($playerDetail[1] / $multiplier));
        break;
      case TRANSCRIPT_ACTION_FORCEDSTANDUP:
        //username, codice 0 1
        //0 senza soldi
        //1 limite mani assenti
        $playerDetail = explode(',', $details);
        $addToTranscript .= sprintf(DETHAND_ACT_FORCESTANDUP, $playersDetail[0]) . " ";

        switch($playerDetail){
          case 0:
            $addToTranscript .= DETHAND_ACT_FORCESTANDUP_CREDITS;
            break;
          case 1:
            $addToTranscript .= DETHAND_ACT_FORCESTANDUP_TOOHANDS;
            break;
          case 2:
            $addToTranscript .= DETHAND_ACT_FORCESTANDUP_TOOROUNDS;
            break;
          default:
            break;
        }

        break;
      case TRANSCRIPT_ACTION_POT:
        $pots_details = explode(';', $details);
        $total_pots  = $pots_details[0];
        $pots        = explode(',', $pots_details[1]);
        for($i = 0 ; $i < $total_pots ; $i++){
          list($potnum, $potval) = explode('=', $pots[$i]);
          $pot_detail = explode('=', $pots[$i]);
          if($i == 0){
            $pot_id = 'main pot';
          }else{
            $pot_id = 'pot #' . $i;
          }
          $addToTranscript .= sprintf(DETHAND_ACT_POT, $pot_id, getInDollars($pot_detail[1] / $multiplier)) . '<br/>';
        }
        break;
      case TRANSCRIPT_ACTION_WINNERS:
        list($plr_name, $plr_winning_pots ,$plr_win) = explode(',',$details);
        $plr_wins_from = explode(' ', $plr_winning_pots);
        $addToTranscript .= sprintf(DETHAND_ACT_WINNERS,$plr_name,getInDollars($plr_win / $multiplier)) . '<br/>';
        foreach ($plr_wins_from as $winnings) {
          list($pot_id, $pot_value) = explode('=', $winnings);
          if($pot_id == 0){
            $pot_id = 'main pot';
          }else{
            $pot_id = 'pot #' . $pot_id;
          }
          if($pot_value > 0){
            $addToTranscript .= sprintf(DETHAND_ACT_WINNERS_DETAILS, getInDollars($pot_value / $multiplier), $pot_id) . '<br/>';
          }
        }
        break;
      case TRANSCRIPT_ACTION_TIMEOUT:
        $addToTranscript      .= sprintf(DETHAND_ACT_TIMEOUT, $details);
        break;
      case TRANSCRIPT_ACTION_RAKE:
        $playersDetail    = explode(',', $details);

        if($playersDetail[1]>0){
          $addToTranscript .= sprintf(DETHAND_ACT_RAKE, $playersDetail[0], getInDollars($playersDetail[1] / $multiplier));
        }

        break;
      case TRANSCRIPT_ACTION_SKIPBLIND:
        $addToTranscript      .= sprintf(DETHAND_ACT_SKIPBLIND, $details);
        break;
    }
    $source = '';
    if(isset($_GET['trnscrpt_visible'])){
      $source = "<font size='-3'>$row</font><br/>\n";
    }
    $transcript .= $source . '<div>' . $addToTranscript . '</div>';
    $counter++;
  }
  return $transcript;
}

?>
