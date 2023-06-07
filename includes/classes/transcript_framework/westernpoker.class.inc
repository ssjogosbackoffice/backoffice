<?php
require_once 'Transcript.class.inc';
require_once 'transcript.inc.php';
class westernpoker extends Transcript {
    function westernpoker() {
    }
    public function getNames() {
        return "Western Poker";
    }
    function getBet() {
        $betData = split ( ";", $this->betString );
        $html = "";
        return "";
    }
    function getReadbleTranscript() {
        $paytable = array (
            '3' => 'Three of a kind',
            '4' => 'Straight',
            '5' => 'Flush',
            '6' => 'Full House',
            '7' => '4 of a kind',
            '8' => 'Straight Flush',
            '9' => 'Royal flush'
        );
        settype ( $this->betString, "string" );
        $thisString = explode ( "@", $this->betString );
        $thisString = array_chunk ( $thisString, 3 );
        $i = 0;
        $handList = array ();
        foreach ( $thisString as $k => $v ) {
            $subcallid = $v [0];
            $request = $v [1];
            $hands = $v [2];
            if ($i > 1) {
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
            }
            array_push ( $handList, array (
                "operationid" => $subcallid,
                "bet" => $request,
                "win" => $win,
                "wintype" => $wintype,
                "cards" => $cardsList,
                "hold" => $holdList,
                "continue" => $continue,
                "freespin" => $freespin
            ) );
            $i ++;
        }
        $html = "<div class='container " . $this->path . "'>";
        $html .= "<div class='box " . $this->path . "'>";
        $html .= "<table rules=rows>";
        for($i = 0; $i < count ( $handList ); $i ++) {
            $hand = $handList [$i];
            $wins = $hand ["win"] / 100;
            $str = "";
            list ( $bonustype, $bonusoccurance ) = explode ( ";", $hand ['bet'] );
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
                            }
                        } else {
                            if ($bonustype == 1) {
                                $str .= "  Bonus Double<br>";
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
        }
        $html .= "</table></table>";
        $html .= "<table><tr><th class='label' colspan='2' ><b>PAYTABLE</b></th></tr>
				<tr><td><img src='../media/images/handimages/westernpoker/paytable_western.png' ></td>";
        $html .= "</tr></table>";
        return $html;
    }
}
?>