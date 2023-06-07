<?php
require_once 'Transcript.class.inc';
require_once 'magiccards.class.inc';
require_once 'transcript.inc.php';
class magiccards extends Transcript {
    function magiccards() {
    }
    public function getNames() {
        return "magiccards";
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
                        if (strpos ( $freespin, "false" ) != false) {
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
                            case "B":{
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
                            }
                            case "H":{
                                if($request=='H'){
                                    $request=$this->getCardSymbol('h');
                                }
                                break;
                            }
                            case "D":{
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
                            }
                                break;
                        }
                        $html .="<td style='font-weight:bold;'>Bet on  " .$request."</td>";
                        $html .= "</tr></table><td style='font-weight:bold;'>Win - ".getInDollars($cardsetc/100)."</td>";
                    }
                }
            }

        /*    if($subcallid==4){



            }
            elseif ($i > 1) {
                list ( $win, $cards, $continue ) = explode ( "~", $hands );
            } else {
                list ( $win, $wintype, $cards, $propose, $freespin ) = explode ( "~", $hands );
            }
            if ($i == 1) {
                $holdList = split ( ";", $propose );
            }
            $cardsList = split ( ";", $cards );
            for($z = 0; $z < count ( $cardsList ); $z ++) {
                list ( $cardType, $cardValue ) = split ( "\\|", $cardsList [$z] );
                $cardsList [$z] = getCard ( strtolower ( trim ( $cardValue ) . "" . trim ( $cardType ) ) );
            }*/
          /*  array_push ( $handList, array (
                "operationid" => $subcallid,
                "bet" => $request,
                "win" => $win,
                "wintype" => $wintype,
                "cards" => $cardsList,
                "hold" => $holdList,
                "continue" => $continue,
                "freespin" => $freespin
            ) );*/
            $i ++;

        }

        $html .="<td colspan='2'><table><tr>";


/*        for($i = 0; $i < count ( $handList ); $i ++) {
            $hand = $handList [$i];
            $wins = $hand ["win"] / 100;
            $str = "";
            list ( $bonustype, $bonusoccurance, $fever, $high ) = explode ( ";", $hand ['bet'] );
            if ($i < 2 || $bonustype == '1') {
                $html .= "<tr>";
            } elseif ($i == 2 && $bonustype == '2') {
                $html .= "<tr><td colspan='2'><table><tr>";
            }
            if ($i > 6 && ($i) % 5 == 2) {
                $html .= "<tr>";
            }
            if ($i < 3 || $bonustype == '1') {
                $html .= "<td><table cellspacing=3 cellpadding=1 >";
                switch ($i) {
                    case 0 :
                    {
                        if (strpos ( $hand ["freespin"], "true" ) !== false) {
                            $str = " FREESPIN! <br>";
                        }
                        $str .= " First Hand";
                        switch ($hand ["operationid"]) {
                            case 1 :
                            {
                                $str .= " - Add ";
                            }
                                break;

                            case 49 :
                            {
                                $str .= " - Bonus ";
                            }
                                break;

                            case 6 :
                            {
                                $str .= " - Bet ";
                            }
                                break;
                        }
                    }
                        break;

                    case 1 :
                        if (strpos ( $hand ["freespin"], "true" ) !== false) {
                            $str = " FREESPIN! <br>";
                        }
                        $str .= "<br/>Second Hand";
                        switch ($hand ["operationid"]) {
                            case "1" :
                            {
                                $str .= " - Add ";
                            }
                                break;
                            case "49" :
                            {
                                $str .= " - Bonus ";
                            }
                                break;
                            case "6" :
                            {
                                $str .= " - Bet ";
                            }
                                break;
                        }
                        break;
                    default :
                        {
                        if ($i > 2) {
                            if ($bonustype == 1) {
                                $str = " Double Bonus - $bonusoccurance time ";
                                $str .= " <br>";
                            } else {
                                $str = " Hi-Lo Bonus ";
                                $str .= " <br>";
                            }
                            if ($fever == 'true') {
                                $handList ['fever'] = 'true';
                                $str .= "<span class='ttm_label'><b> Fever : Activated</b></span><br>";
                                $handList [0] ["bet"] = split ( ";", $handList [0] ["bet"] );
                                $str .= " Bet:" . getInDollars ( ($handList [0] ["bet"] [0] / $handList [0] ["bet"] [1]) / 100 );
                            }
                        } else {
                            if ($bonustype == 1) {
                                $str .= "  Bonus Double<br>";
                                if ($fever == 'true') {
                                    $handList ['fever'] = 'true';
                                    if ($bonustype == 1) {
                                        $str .= " <span class='ttm_label'><b> Fever : Activated</b></span><br>";
                                        $handList [0] ["bet"] = split ( ";", $handList [0] ["bet"] );
                                        $str .= " Bet:" . getInDollars ( ($handList [0] ["bet"] [0] / $handList [0] ["bet"] [1]) / 100 );
                                    }
                                }
                            } else {
                                $str = " Hi-Lo Bonus <br> ";
                                if ($fever == 'true') {
                                    $handList ['hilo'] = 'true';
                                    $str .= " <span class='ttm_label'><b> Fever : Activated </b></span><br>";
                                    $handList [0] ["bet"] = split ( ";", $handList [0] ["bet"] );
                                    $str .= " Bet:" . getInDollars ( ($handList [0] ["bet"] [0] / $handList [0] ["bet"] [1]) / 100 );
                                }
                                $hand = $handList [$i + 1];
                                list ( $bonustype, $bonusoccurance, $fever, $high ) = explode ( ";", $hand ['bet'] );
                                $i ++;
                            }
                        }
                        }
                        break;
                }
                if ($hand ["operationid"] != "49") {
                    $html .= "<th colspan='5'>$str</th><tr>";
                } else {
                    $html .= "<th colspan='5'>$str</th></tr>";

                    if ($bonustype == '1') {
                        $html .= "<tr><td><b>Higher than:</b></td><td colspan='4'></td></tr>";
                    } else {
                        $html .= "<tr><td><b>First card:</b></td></tr>";
                    }
                    $html .= "<tr>";
                }
                $cards = $hand ["cards"];
                if ($i == 0) {
                    $hold = split ( ";", $handList [1] ["bet"] );
                } else {
                    $hold = $handList [1] ["hold"];
                }
                for($z = 0; $z < count ( $cards ); $z ++) {
                    $html .= "<td";
                    if ($hold [$z] == 1 && $i < 2) {
                        if ($wins != '0') {
                            $html .= " class='wincard'";
                        } else {
                            $html .= " class='hold'";
                        }
                    }
                    if ($bonustype == '2') {
                        $html .= ">" . $handList [$i - 1] ["cards"] [$z] . "<br><b> Currently<br> win:" . getInDollars ( $handList [$i - 1] ["win"] / 100 ) . "</b></td>";
                    } else {

                        $html .= "> $cards[$z]</td>";
                    }
                }
                $html .= "</tr></table></td>";
            }
            $html .= " <td>";
            if ($hand ["operationid"] == 6) {
                $hand ["bet"] = $hand ["bet"] / 100;
                if (strpos ( $hand ["freespin"], "true" ) !== false) {
                } else {
                    $html .= "<b><br>Bet: " . $hand ["bet"] . " &#8364;<b><br/>";
                }
            }
            if ($hand ["operationid"] == 49) {
                $html .= "<table cellspacing=7 cellpadding=2><tr><td><b>";
                if ($bonustype != '2') {
                    $html .= "<br>Player's card:</b></td><tr><td>";
                }
                if ($bonustype == '2') {
                    if ($high == '1') {
                        $html .= "<br><br><br><div style='margin-left:12px'>Higher </div>";
                    }
                    if ($high == '0') {
                        $html .= " <br><br><br><div style='margin-left:12px'>Lower </div> ";
                    }
                }
                if ($bonustype == "1") {
                    $html .= $hand ["cards"] [4];
                } else {
                    $html .= $hand ["cards"] [0];
                }
                $html .= "</td></tr><tr><td><b>Win: ";
                $html .= getInDollars ( $hand ["win"] / 100 );
                $html .= "</b></td>";
                $html .= "</tr></table>";
            } elseif ($i > 0) {
                if ($hand ["operationid"] != NULL) {
                    $html .= "<br /><b>Win: " . $wins . " &#8364;</b>";
                    foreach ( $paytable as $k => $v ) {
                        if ($hand ["wintype"] == $k) {
                            $html .= "<br><b>(" . $v . ")</b>";
                        }
                    }
                }
            }
            $html .= "</td>";

            if ($i < 2 || $bonustype == '1') {
                $html .= "</tr>";
            }
            if ($i > 5 && (($i) % 5) == 1) {
                $html .= "</tr>";
            }
        }*/

        $html .= "</table></table>";
        $html .= "<table><tr><th class='label' colspan='2' ><b>PAYTABLE</b></th></tr>
				<tr><td><img src='../media/images/handimages/magiccards/paytable1.png' ></td>";
        if ($handList ['fever'] == 'true') {
            $html .= "<td><img src='../media/images/handimages/maggicacards/fever.png' style='height:100%' ></td>";
        } elseif ($handList ['hilo'] == 'true') {
            $html .= "<td><img src='../media/images/handimages/magiccards/hilo.png' style='height:100%' ></td>";
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