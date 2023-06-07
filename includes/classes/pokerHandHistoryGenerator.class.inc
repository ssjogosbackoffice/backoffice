<?php

/**
 * Created by PhpStorm.
 * User: marian
 * Date: 08/12/15
 * Time: 10:27
 */

class pokerHandHistoryGenerator
{
    public $tableGeneralDetails;
    public $players;
    public $hands;
    public $chat;
    public $seats;/**
 * @return mixed
 */
public  function getSeats()
{
    return $this->seats;
}/**
 * @param mixed $seats
 */
public  function setSeats($seats)
{
    $this->seats = $seats;
}



/**
 * @return mixed
 */
public  function getTableGeneralDetails()
{
    return $this->tableGeneralDetails;
}/**
 * @param mixed $tableGeneralDetails
 */
public  function setTableGeneralDetails($tableGeneralDetails)
{
    $this->tableGeneralDetails = $tableGeneralDetails;
}/**
 * @return mixed
 */
public  function getPlayers()
{
    return $this->players;
}/**
 * @param mixed $players
 */
public  function setPlayers($players)
{
    $this->players = $players;
}/**
 * @return mixed
 */
public  function getHands()
{
    return $this->hands;
}/**
 * @param mixed $hands
 */
public  function setHands($hands)
{
    $this->hands = $hands;
}/**
 * @return mixed
 */
public  function getChat()
{
    return $this->chat;
}/**
 * @param mixed $chat
 */
public  function setChat($chat)
{
    $this->chat = $chat;
}/**
 * @return array
 */
public  function getCardsArr()
{
    return $this->cards_arr;
}/**
 * @param array $cards_arr
 */
public  function setCardsArr($cards_arr)
{
    $this->cards_arr = $cards_arr;
}/**
 * @return array
 */
public  function getWinningHandType()
{
    return $this->winningHandType;
}/**
 * @param array $winningHandType
 */
public  function setWinningHandType($winningHandType)
{
    $this->winningHandType = $winningHandType;
}

    private $cards_arr = array (
    "0" => "2c",
    "1" => "2d",
    "2" => "2s",
    "3" => "2h",
    "4" => "3c",
    "5" => "3d",
    "6" => "3s",
    "7" => "3h",
    "8" => "4c",
    "9" => "4d",
    "10" => "4s",
    "11" => "4h",
    "12" => "5c",
    "13" => "5d",
    "14" => "5s",
    "15" => "5h",
    "16" => "6c",
    "17" => "6d",
    "18" => "6s",
    "19" => "6h",
    "20" => "7c",
    "21" => "7d",
    "22" => "7s",
    "23" => "7h",
    "24" => "8c",
    "25" => "8d",
    "26" => "8s",
    "27" => "8h",
    "28" => "9c",
    "29" => "9d",
    "30" => "9s",
    "31" => "9h",
    "32" => "Tc",
    "33" => "Td",
    "34" => "Ts",
    "35" => "Th",
    "36" => "Jc",
    "37" => "Jd",
    "38" => "Js",
    "39" => "Jh",
    "40" => "Qc",
    "41" => "Qd",
    "42" => "Qs",
    "43" => "Qh",
    "44" => "Kc",
    "45" => "Kd",
    "46" => "Ks",
    "47" => "Kh",
    "48" => "Ac",
    "49" => "Ad",
    "50" => "As",
    "51" => "Ah"
);

   private $winningHandType=array(
    9 => "Royal Flush",
    8 => "Straight Flush",
    7 => "Four of a kind",
    6 => "Full house",
    5 => "Flush",
    4 => "Straight",
    3 => "Three of a kind",
  	2 => "Two Pair",
  	1 => "One Pair",
    0 => "High card"
);

    function __construct(){

    }


    public function  getTableGeneralInfo(){

    }

   private function getCurrentCardValue($card){
    return $this->cards_arr[$card];
    }


     public function showCards($cards)
        {
            $htmlText = "";
            $count = strlen($cards) / 3;
            for ($i=0;$i<$count;$i++)
            {
                $htmlText .= "<img src='cardimg.php?c=".substr($cards, $i*3, 2)."' border='0' width='20' height='28'>";
            }
            return $htmlText;
     }

   public  function getWinningCards($cards,$type){
    if($type==0){
        $hand=array();
        $max= array_search(max($cards),$cards);
        $hand[$max]=$cards[$max];
        return $hand;

    }
    elseif($type==1 || $type==2){
        $cardsVal=array_map(array($this, 'getCardRank'),$cards);
        asort($cardsVal);
        $hand=array();
        foreach($cardsVal as $k=>$v){
             if($v===$cardsVal[$k+1]){
                 $hand[$k]=$cards[$k];
                 $hand[$k+1]=$cards[$k+1];
             }
        }
        return $hand;
    }
    elseif($type==3){
       $cardsVal=array_map(array($this, 'getCardRank'),$cards);
        asort($cardsVal);
        $hand=array();
        foreach($cardsVal as $k=>$v){
            if($v==$cardsVal[$k+1]&& $v==$cardsVal[$k+2]){
                $hand[$k]=$cards[$k];
                $hand[$k+1]=$cards[$k+1];
                $hand[$k+2]=$cards[$k+2];
            }
        }
        return $hand;
    }
    elseif($type==7){
      $cardsVal=array_map(array($this, 'getCardRank'),$cards);
        asort($cardsVal);
        $hand=array();
        foreach($cardsVal as $k=>$v){
            if($v==$cardsVal[$k+1]&& $v==$cardsVal[$k+2] && $v==$cardsVal[$k+3]){
                $hand[$k]=$cards[$k];
                $hand[$k+1]=$cards[$k+1];
                $hand[$k+2]=$cards[$k+2];
                $hand[$k+3]=$cards[$k+3];
            }
        }
        return $hand;
    }
    else{
        return$cards;
    }
}

    public function getCardRank($card){
        $rank=floor($card/4);
    return $rank;
}


public function generateTableHeaderInfo(){

   echo '<table class="table display" width="100%">
        <thead>
        <tr>
            <th colspan="7">
                <span class="fleft">Hand #'.$this->tableGeneralDetails['hand_id'].'</span>
                <span class="fright">'.$this->tableGeneralDetails['hand_start'].'</span>
            </th>
        </tr>
        </thead>
        </table>
        <table class="table table-condensed display" width="100%">
        <thead>
        <tr>
            <th>Table</th>
            <th>Game</th>
            <th>Limit</th>
            <th>Small Blind</th>
            <th>Big Blind</th>
            <th>Total Bet</th>
            <th>Total Rake</th>
        </tr>
        </thead>
        <tr>
            <td class="content">'.$this->tableGeneralDetails['table'].'</td>
            <td class="content">'.$this->tableGeneralDetails['game_type'].'</td>
            <td class="content">'.$this->tableGeneralDetails['limit_type'].'</td>
            <td class="content">'.getInDollars($this->tableGeneralDetails['smValue']/100).'</td>
            <td class="content">'.getInDollars($this->tableGeneralDetails['bbValue']/100).'</td>
            <td class="content">'.getInDollars($this->tableGeneralDetails['bet']/100).'</td>
            <td class="content">'.getInDollars($this->tableGeneralDetails['rake']/100).'</td>
        </tr>
        </table>
        <table class="table display" width="100%">
        <thead>
        <tr>
            <th  colspan="7">
                Common cards
            </th>
        </tr>
        </thead>
        <tr>
            <td colspan="3" class="content">'.$this->showCards($this->tableGeneralDetails['board']).'</td>
        </tr>
    </table>';
}

public function showPlayerHandDetails(){
if ( $this->seats->getNumRows()>0 )
{
    echo '<table  class="table table-striped table-hover display" width="100%">
            <thead>
            <tr>
                <th>Username</th>';
                if($this->tableGeneralDetails['type']=='live'){
                    echo '<th>Seat</th>';
                }
                echo '<th>Cards</th>
                      <th>Hand Rank</th> ';
                if($this->tableGeneralDetails['type']=='live'){
                    echo '<th>Stack</th>';
                 }

                echo '<th>Bet</th>
                <th>Win</th>
                <th>Ip</th>
            </tr></thead>';

    while( $this->seats->hasNext() ) {
        $row=$this->seats->next();
        if($this->tableGeneralDetails['type']!='live'){
            $row['holecards'] = chunk_split($row['holecards'], 2, ' ');
        }
        echo ' <tr>
                <td><a '.($row['won']>0? 'class="winner"':'').' target="_blank" href="/?page=customers/customer_view&customer_id='.$row['player_id'].'"><span class="glyphicon glyphicon-user" aria-hidden="true"></span>  '.$this->players[$row['player_id']].'</a></td>';
                if($this->tableGeneralDetails['type']=='live'){
                  echo '<td>'.$row['seat'].'</td> ';
                    $row['cards_shown']="0";
                 }

                echo '<td>'.($row['cards_shown']!='1' || $_SESSION['jurisdiction_class']=='casino1'?$this->showCards($row['holecards']):$this->showCards('kj')).'</td>
                      <td>';

                $currentHandCards=trim($row['holecards'])." ".trim($this->tableGeneralDetails['board']);

                $currentHandCards=explode(" ",$currentHandCards);
                foreach($currentHandCards as $k=>$v){
                    $currentHandCards[$k]=array_search($v,$this->cards_arr);
                }
                $currentRank=new TexasHoldEmPokerUtility();
                if(count($currentHandCards)==2){
                    $currentHandRank=$currentRank->evaluate2($currentHandCards);
                }
                if(count($currentHandCards)==5){
                    $currentHandRank=$currentRank->evaluate5($currentHandCards);
                }
                if(count($currentHandCards)==6){
                    $currentHandRank=$currentRank->evaluate6($currentHandCards);
                }
                if(count($currentHandCards)==7){
                    $currentHandRank=$currentRank->evaluate7($currentHandCards);
                }
                 if($row['cards_shown']!='1' || $_SESSION['jurisdiction_class']=='casino1'){
                echo '
                <table>
                    <tr>
                        <td style="border-left:0 ;">'.$this->winningHandType[$currentRank->getBestHandId()].' :</td>
                        <td style="width: 50%;text-align: right;">'.$this->showCards(implode(" ",array_map(array($this,'getCurrentCardValue'),$this->getWinningCards($currentRank->getBestHandCardsArray(),$currentRank->getBestHandId())))).'</td>
                    </tr>
                </table>
                </td>';
                }
                else{
                    echo $this->showCards('jk');
                }
                if($this->tableGeneralDetails['type']=='live'){
                    echo '<td>'.getInDollars($row['stack']/100).'</td>';
                 }
                echo '<td>'.getInDollars($row['bets']/100).'</td>
                      <td '.($row['won']>0? 'class="winner"':'').'>'.getInDollars($row['won']/100).'</td>
                      <td>'.long2ip($row['ip']).'</td>
            </tr>';

    }

    echo '</table>';
    }

    }



public  function getHandsDetailed(){

     $rowchat=$this->seats->Records;
    $turn = -1;
    while ( $this->hands->hasNext() )
    {
        $row=$this->hands->next();
        if ( $turn < $row['turn_id'] )
        {
            if ( $row['turn_id'] == 6 )
                $turn = 5;

            for ($i=$turn+1;$i<=$row['turn_id'];$i++)
            {
                //show chat messages for this turn
                while ( $rowchat && $rowchat[0] <= $i )
                {
                    ListItemChat( $rowchat[1]."> ".$rowchat[2], "left" );
                }

                switch ( $i )
                {
                    case 0:
                        InitList(1, "events");
                        ListHeader("Preflop", 550);
                        break;
                    case 1:
                        EndList();
                        InitList(1, "events");
                        ListHeader("Dealing Holecards", 550);
                        break;
                    case 2:
                        EndList();
                        InitList(1, "events");
                        ListHeader("Dealing Flop ". $this->showCards(substr($this->tableGeneralDetails['board'],0,9)), 550);
                        break;
                    case 3:
                        EndList();
                        InitList(1, "events");
                        ListHeader("Dealing Turn ". $this->showCards(substr($this->tableGeneralDetails['board'],9,2)), 550);
                        break;
                    case 4:
                        EndList();
                        InitList(1, "events");
                        ListHeader("Dealing River ". $this->showCards(substr($this->tableGeneralDetails['board'],12,2)), 550);
                        break;
                    case 5:
                        EndList();
                        InitList(1, "events");
                        ListHeader("Showdown", 550);
                        break;
                    case 6:
                        EndList();
                        InitList(1, "events");
                        ListHeader("Hand Cancelled", 550);
                        break;
                }
            }

            $turn = $row['turn_id'];
        }

        $txt = $row['player_name']." ".$row['event_text']." ";
        if ( $row['param1'] > 0 )
            $txt .= getInDollars($row['param1']/100);

        ListItem( $txt, "left", "");
    }
    EndList();
}

}