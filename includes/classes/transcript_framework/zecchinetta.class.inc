<?php
require_once 'Transcript.class.inc';
require_once 'transcript.inc.php';
define("CARD_PATH" , "/live/zecchinetta/cards/");
define("TABLE_PATH" , "/live/zecchinetta/table.png");

define("LEFT",1);
define("TOPLEFT",2);
define("TOP",3);
define("TOPRIGHT",4);
define("RIGHT",5);
define("BOTTOMRIGHT",6);
define("BOTTOM",7);
define("BOTTOMLEFT",8);
define("CENTER",9);

class zecchinetta extends Transcript
{

    public $playersArr=array();

    public $card_list=array();

    function getBet() {

        $this->resultString; //2446328,103459-ngr&39-394	->	id_hand,id_ticket&id_card-win bet in cents
        $this->betString; // 6@39~200 -> 6@id_card~bet in cents
        $this->resString; // 0,1591,1,RNT43,9646049,-1;1,1431,1,RNT31,299757,-1;2,1422,7,RNT22,220267,-1;3,-1,-1,-1,-1,-1;4,1429,1,RNT29,115209,1;5,-1,-1,-1,-1,-1;6,-1,-1,-1,-1,-1;7,-1,-1,-1,-1,-1;8,-1,-1,-1,-1,-1;9,-1,-1,-1,-1,-1@35,39,8,4,25,6,3,5,12,18@4,-17.12
                          // position,id_user,avatar_id,username,credit,banker yes or not;… position,id_user,avatar_id,username,credit,banker yes or not@id_card,id_card,..id_card@position_banker,balance_banker

        $players_bets = explode("@",$this->resString)[0];
        $cards = explode("@",$this->resString)[1];
        $banker = explode("@",$this->resString)[2];

        $win_card = explode('&',$this->resultString)[1];
        $win_card = explode('-',$win_card);

        $card_bet = explode('@',$this->betString)[1];
        $card_bet = explode('~',$card_bet);

        $this->playersArr = explode(";",$players_bets);
        $c = explode(",",$cards);
        $b = explode(",",$banker);

        if($c[0] != '') {
            foreach ($c as $card) {

                    $this->card_list[] = $card . ".png";

            }
        }

        $players_bets = explode("@",$this->resString)[0];
        $this->playersArr = explode(";",$players_bets);

        list($handDetails,$betresult)=explode('&',$this->resultString);
        $betresult = new StringTokenizer ( $betresult, ";" );
        $resultbets = array ();
        $totalwin = 0;
        while ( $betresults = $betresult->nextToken () ) {
            list ( $betids, $betwin ) = explode ( "-", $betresults );
            array_push ( $resultbets, array ("betid" => $betids, "betwin" => $betwin ) );
            $totalwin  += $betwin;
        }

        $this->betString = explode ( "@", $this->betString );

        $i = 0;
        $html = '<br>';

        $bets = [];
        $wins = [];

//        echo "<pre align='left'>";
//        print_r($betresult->tokens[0]);
//        echo '<br>';
//        print_r($this->betString);

        foreach ($this->betString as $card_bet){
            if(strpos($card_bet, '~')){
                $card_bet = explode('~',$card_bet);
                $bets[$card_bet[0]] = $card_bet[1];
            }
        }

        $win_info = explode('@',$betresult->tokens[0]);
        foreach ($win_info as $card_win) {
            $card_win = explode('-',$card_win);
            $wins[$card_win[0]] = $card_win[1];
        }
//        print_r($bets);
//        echo '<br>';
//        print_r($wins);
//        echo '<br>';
//        print_r($c);
//        echo "</pre>";




        if(!empty($this->card_list)) {
            $html .= "<div style='min-width: 64px; float: left;margin-top:5px; margin-left: 5px; margin-right: 5px;'>";
            $html .= "<div style='min-width: 62px; '>Banker:<br>".getInDollars($b[1])."</div>";
            $html .= "<div style='background:url(\"" . CARD_PATH . $this->card_list[2] . "\") center center no-repeat; border-radius:5px;  border:2px red solid; height:100px; width:60px; background-size: 60px 100px; '></div>";
            $html .= "<div style='min-width: 62px;'>card id:".($c[2]).(($i<3)?'<br>#1':'<br>#'.($i-1))."</div>";
            $html .= "</div>";
            foreach ($this->card_list as $i => $card) {
                if($i != 2) {
                    $html .= "<div style='min-width: 64px; float: left; margin-top:5px; margin-left: 5px; margin-right: 5px;'>";
                    $html .= (isset($bets[$c[$i]]) && isset($wins[$c[$i]]))?"<div style='min-width: 62px; '>Bet: ".((isset($bets[$c[$i]]))?getInDollars($bets[$c[$i]]/100):0)."<br>Win: ".((isset($wins[$c[$i]]))?getInDollars($wins[$c[$i]]/100):0)."</div>":'<br>&nbsp;&nbsp;&nbsp;';
                    $html .= "<div style='background:url(\"" . CARD_PATH . $card . "\") center center no-repeat; border-radius:5px;  margin-top:2px; border:1px #999 solid; height:100px; width:60px; background-size: 60px 100px;'></div>";
                    $html .= "<div style='min-width: 62px; '>card id:".($c[$i]).(($i<3)?'<br>#1':'<br>#'.($i-1))."</div>";
                    $html .= "</div>";
                }
            }
        }

        return $html;
    }

    function getReadbleTranscript() // winning hand
    {
        settype($this->resString, "string");
        $resultString=explode('@',$this->resString);
        $this->playersArr=explode(';',$resultString[0]);

        list($number, $color) = split("\\|", $resultString[1]);
        return $this->createTableView();
    }

    private function createTableView(){

        $players_bets = explode("@",$this->resString)[0];

        $this->playersArr = explode(";",$players_bets);

        $players_bets = explode("@",$this->resString)[0];
        $this->playersArr = explode(";",$players_bets);

        $html="<div class='label' style='padding:2px'>Hand details</div><br /><br /><div style='position:relative; width: 100%; height: 500px; background-size: contain; background-image:url(".TABLE_PATH."); backgroud-position:center center; background-repeat:no-repeat;'> ";

        foreach ($this->playersArr as $index => $player){
            $playerArr = explode(',',$player);
            $left = 0;
            $top='55px';
            if($playerArr[0]==9){
                $left = "223px";
            }
            if($playerArr[0]==0){
                $left = "393px";
            }
            if($playerArr[0]==1){
                $left = "560px";
            }
            if($playerArr[0]==2){
                $top = "155px";
                $left = "700px";
            }
            if($playerArr[0]==3){
                $top = "254px";
                $left = "700px";
            }
            if($playerArr[0]==4){
                $top = "357px";
                $left = "560px";
            }
            if($playerArr[0]==5){
                $top = "357px";
                $left = "393px";
            }
            if($playerArr[0]==6){
                $top = "357px";
                $left = "223px";

            }
            if($playerArr[0]==7){
                $top = "254px";
                $left = "82px";
            }
            if($playerArr[0]==8){
                $top = "155px";
                $left = "82px";
            }
            $color = 'rgba(250, 250, 250, 0.5)';
            if($playerArr[1] > -1) {

                if($playerArr[5] == 1){
                    $color = 'rgba(250, 200, 200, 0.7)';
                } else {
                    $color = 'rgba(250, 250, 250, 0.7)';
                }
                $html .= "<div class='multiTableUserInfo' style=' text-align:center; padding-top: 5px; width:120px; height: 50px; border-radius: 5px; background-color: $color; position: absolute; top:$top; left:$left;'>";

                $html .= "<span style='font-size:15px;'>".$playerArr[3]."</span><br><span style='font-size:15px;'>".getInDollars($playerArr[4]/100)."</span>";
                if($playerArr[5] == 1){
                    $html .= "<br><span class='tip'>(Banker) $playerArr[0]</span>";
                } else {
                    $html .= "<br><span class='tip'>pos: $playerArr[0]</span>";
                }
                $html .= "</div>";
            } else {
                $html .= "<div class='multiTableUserInfo' style=' text-align:center; padding-top: 8px; width:120px; height: 50px; border-radius: 5px; background-color: $color; position: absolute; top:$top; left:$left;'>";

                $html .= "</div>";
            }
        }
        $html.="</div>";

        return $html;

    }
}
?>




