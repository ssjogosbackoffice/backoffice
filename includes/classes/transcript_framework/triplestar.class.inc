<?php
require_once 'Transcript.class.inc';
require_once 'triplestar.class.inc';
require_once 'transcript.inc.php';
class triplestar2015 extends Transcript {
    function triplestar() {
    }
    public function getNames() {
        return "triplestar";
    }
    function getBet() {
        $betData = split ( ";", $this->betString );
        $html = "";
        return "";
    }
    function getReadbleTranscript() {
        $paytable = array (
            '1' => 'One Pair',
            '2' => 'Two Pairs',
            '3' => 'Three of a kind',
            '4' => 'Straight',
            '5' => 'Flush',
            '6' => 'Full House',
            '7' => '4 of a kind',
            '8' => 'Straight Flush',
            '9' => '5 of a kind',
            '10' => 'Wild royal flush',
            '11' => '4 jokers'
        );



        settype ( $this->betString, "string" );
        $thisString = explode ( "#", $this->betString );

        $html = "<div class='container " . $this->path . "'>";
        $html .= "<div class='box " . $this->path . "'>";
        $html .= "<table rules=rows>";
        $i = 0;
        $handList = array ();
        foreach ( $thisString as $k => $v ) {
            //$initialbet=
            $str = "";
            list($subcallid , $next , $cardsetc)=explode('@',$v);
            $request = $next;
            $hands = $cardsetc;
            switch ($subcallid){
                case 6 : {
                    if($subcallid==6){
                    list ( $bet, $wintype, $cards, $propose, $freespin ) = explode ( "~", $hands );
                        if (strpos ( $freespin, "true" ) != false) {
                            list($free,$freeLeft)=explode('-',$freespin);
                            $html .="<table cellpadding='3''><tr><td><table cellpadding='3'><tr><th colspan='5'>FREESPIN !  </th> <th colspan='5'>  Freespins Left - ".$freeLeft." </th></tr><tr>";
                        }
                        $nexthand=explode('@',$thisString[$k+1]);
                        $propose=$nexthand[1];
                        $holdList = explode ( ";", $propose );
                        $cardsList = explode ( ";", $cards );
                        $html .="<table cellpadding='3'><tr><td><table cellpadding='3'><tr><th colspan='5'>".($k+1)." st HAND - BET</th></tr><tr>";
                        for($z = 0; $z < count ( $cardsList ); $z ++) {
                            list ( $cardType, $cardValue ) = explode ( "|", $cardsList [$z] );
                            $cardsList [$z] = getCard ( strtolower ( trim ( $cardValue ) . "" . trim ( $cardType ) ) );
                            $html .= "<td ";
                            if($holdList[$z]==1){
                                $html .= "class='hold'>".$cardsList [$z].'</td>';
                            }
                            else{
                                $html .= "class='card'>".$cardsList[$z]."</td>";
                            }
                        }
                        $html .= "</tr></table><td style='font-weight:bold;'>Bet - ".getInDollars($next/100)."</td>";
                    }
                 break;
                }
                case 1:{
                    if($subcallid==1){
                        list ( $propose,$bet,$cards,  $wintype,  $freespin ) = explode ( "~", $hands );
                        $holdList = explode ( ";", $next );
                        $wincards = explode (";",$wintype);
                        $cardsList = explode ( ";", $cards );
                        $html .="<table cellpadding='3'><tr><td><table cellpadding='3'><tr><th colspan='5'>".($k+1)."nd Hand - Add Card</th></tr><tr>";

                        if($propose < 1){
                        for($z = 0; $z < count ( $cardsList ); $z ++) {
                            list ( $cardType, $cardValue ) = explode ( "|", $cardsList [$z] );
                            $cardsList [$z] = getCard ( strtolower ( trim ( $cardValue ) . "" . trim ( $cardType ) ) );
                            $html .= "<td ";
                            if($holdList[$z]==1){
                                $html .= "class='hold'>".$cardsList [$z].'</td>';
                            }
                            else{
                                $html .= "class='card'>".$cardsList[$z]."</td>";
                            }
                        }
                        $html .= "</tr></table><td style='font-weight:bold;'> </td></td>";
                    }
                        else{
                        /*$html .="<table cellpadding='3'><tr><td><table cellpadding='3'><tr>";*/
                        for($z = 0; $z < count ( $cardsList ); $z ++) {
                            list ( $cardType, $cardValue ) = explode ( "|", $cardsList [$z] );
                            $cardsList [$z] = getCard ( strtolower ( trim ( $cardValue ) . "" . trim ( $cardType ) ) );
                            $html .= "<td ";
                            if($wincards[$z]==1){
                                $html .= "class='wincard'>".$cardsList [$z].'</td>';
                            }
                            else{
                                $html .= "class='card'>".$cardsList[$z]."</td>";
                            }
                        }
                        $html .= "</tr></table><td style='font-weight:bold;'>";
                        if($propose > 0){
                            $html .="Win - ".getInDollars($propose/100)."</br>(".$paytable[$bet].")</td>";
                        }
                        else{
                            $html .="</td>";
                        }
                        }
                }
                    break;
                }
                case 3: {
                    if($subcallid==3){
                        list ( $propose,$bet,$cards,  $wintype,  $freespin ) = explode ( "~", $hands );
                        $holdList = explode ( ";", $next );
                        $wincards = explode (";",$wintype);
                        $cardsList = explode ( ";", $cards );
                        $html .="<table cellpadding='3'><tr><td><table cellpadding='3'><tr><th colspan='5'>".($k+1)."nd Hand - Retry</th></tr><tr>";

                        if($propose < 0){
                        for($z = 0; $z < count ( $cardsList ); $z ++) {
                            list ( $cardType, $cardValue ) = explode ( "|", $cardsList [$z] );
                            $cardsList [$z] = getCard ( strtolower ( trim ( $cardValue ) . "" . trim ( $cardType ) ) );
                            $html .= "<td ";

                            if($holdList[$z]==1){

                                $html .= "class='hold'>".$cardsList [$z].'</td>';
                            }
                            else{

                                $html .= "class='card'>".$cardsList[$z]."</td>";
                            }
                        }
                        $html .= "</tr></table><td style='font-weight:bold;'> Bet - ".$next."</td>";
                        }
                        else{

                       /* $html .="<table cellpadding='3'><tr><td><table cellpadding='3'><tr>";*/
                        for($z = 0; $z < count ( $cardsList ); $z ++) {
                            list ( $cardType, $cardValue ) = explode ( "|", $cardsList [$z] );
                                   $cardsList [$z] = getCard ( strtolower ( trim ( $cardValue ) . "" . trim ( $cardType ) ) );
                            $html .= "<td ";
                            if($wincards[$z]==1){
                                $html .= "class='wincard'>".$cardsList [$z].'</td>';
                            }
                            else{
                                $html .= "class='card'>".$cardsList[$z]."</td>";
                            }
                        }
                        $html .= "</tr></table><td style='font-weight:bold;'>";
                        if($propose > 0){
                            $html .="Win - ".getInDollars($propose/100)."</br>(".$paytable[$bet].")</td>";
                        }
                        else{
                            $html .="</td>";
                        }
                        }
                    }
                    break;
                }
                case 4: {
                    $html .="<table cellpadding='3'><tr><td><table cellpadding='3'><tr><th colspan='2'>Double Bonus</th></tr><tr>";
                    if($subcallid==4){
                        switch ($request) {
                            /*case "B":{
                                if($request=='B'){
                                    $request=$this->getCardSymbol('b');
                                }
                                break;
                            }
                            case "R":{
                                if($request=='R'){
                                    $request=$this->getCardSymbol('r');
                                }
                                break;
                            }
                            case "S":{
                                if($request=='S'){
                                    $request=$this->getCardSymbol('s');
                                }
                                break;
                            }*/
                            case "H":{
                                if($request=='H'){
                                    $request='High';
                                }
                                break;
                            }
                           /* case "D":{
                                if($request=='D'){
                                    $request=$this->getCardSymbol('d');
                                }
                                break;
                            }
                            case "C":{
                                if($request=='C'){
                                    $request=$this->getCardSymbol('c');
                                }
                                break;
                            }*/
                            case "L":{
                                if($request=='L'){
                                    $request='Low';
                                }
                                break;
                            }
                                break;
                        }
                        $html .="<td style='font-weight:bold;'>Bet on  " .$request."</td>";
                        $html .= "</tr></table><td style='font-weight:bold;'>Win - ".getInDollars($cardsetc/100)."</td>";
                    }
                }
                case 14:{
                    if($subcallid==14){

                        $star=substr_count($next,'1');
                        if($star==1){

                            $html .="<td style='font-weight: bold'>Bonus <img src='../media/images/handimages/triplestar/bonusStar.png' width='20px' height='20px' > Multiplier X3</td>";
                        }
                        elseif($star==2){
                            $html .="<td style='font-weight:bold'>Bonus <img src='../media/images/handimages/triplestar/bonusStar.png' width='20px' height='20px' > Multiplier X9</td>";
                        }
                    }
                }
                    break;
            }
            $i ++;
        }
        $html .="<td colspan='2'><table><tr>";
        $html .= "</table></table>";
        $html .= "<table><tr><th class='label' colspan='2' ><b>PAYTABLE</b></th></tr>
				<tr><td><img src='../media/images/handimages/triplestar/paytable1.png' width='825px' height='185px' ></td>";
        if ($handList ['fever'] == 'true') {
            $html .= "<td><img src='../media/images/handimages/maggicacards/fever.png' style='height:100%' ></td>";
        } elseif ($handList ['hilo'] == 'true') {
            $html .= "<td><img src='../media/images/handimages/triplestar/hilo.png' style='height:100%' ></td>";
        }
        $html .= "</tr></table>";
        return $html;
    }




    public function getCardSymbol($cardSim){
        $circle="style='background-color:";
        $seeds = array("&clubs;","&diams;","&hearts;","&spades;","&#9786;",$circle);
        switch ($cardSim){
            case "r":
                $cardSim = $seeds[5];
                $color = "#FF0000";
                break;
            case "b":
                $cardSim = $seeds[5];
                $color = "#000000";
                break;
            case "c":
                $cardSim = $seeds[0];
                $color = "#000000";
                break;
            case "d":
                $cardSim = $seeds[1];
                $color = "#FF0000";
                break;
            case "h":
                $cardSim = $seeds[2];
                $color = "#FF0000";
                break;
            case "s":
                $cardSim = $seeds[3];
                $color = "#000000";
                break;
            case "p":
                $cardSim = $seeds[4];
                $color = "#FF0000";
                break;


            default:
                break;
        }

        if( $cardSim != $circle){
            $card_style = "<span class='card' style='color:" . $color . "'> <span class='seed' style='font-size:46px'>" .$cardSim . "</span></span>";
        }
        else{
            $card_style = "<span class='card' style='color:" .$color . ";height:50px;'><br/> <span class='seed' style='font-size:30px;background-color:'".$color."'><div ".$circle.$color.";width: 30px;
  height: 30px;
  border-radius: 50%;
  margin-left: 5px;
  margin-top: -3px;
  margin-bottom: 17px;'></div></span></span>";
        }

        return $card_style;



    }

}

?>