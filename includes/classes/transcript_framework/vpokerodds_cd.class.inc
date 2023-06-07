<?php
define ( "IMG_PATH", "/games/%s/%s/admin_tokens/" );
define ( "PAYOUT_PATH", "/games/%s/%s/" );
class vpokerodds_cd extends Transcript {
	
	function vpokerodds_cd() {
	}
	
	public function getNames() {
		return "vpokerodds_cd";
	}
	
	function getBet() {
		$betData = split ( ";", $this->betString );
		$html = "";
		if ($betData [1] == 0) {
			$html .= "<b>PLAY IN FREESPIN</b>";
		} else {
			$html .= "PAY: <b>" . getInDollars ( $betData [1] / 100 ) . "</b> FOR LINE <br>";
			$html .= "TOTAL LINE: <b>" . number_format ( $betData [0] / $betData [1], 0 ) . " </b>";
		}
		return "";
	}
	
	function getReadbleTranscript() {
		$cards_arr = array (
				"0" => "C|1",
				"1" => "D|1",
				"2" => "S|1",
				"3" => "H|1",
				"4" => "C|2",
				"5" => "D|2",
				"6" => "S|2",
				"7" => "H|2",
				"8" => "C|3",
				"9" => "D|3",
				"10" => "S|3",
				"11" => "H|3",
				"12" => "C|4",
				"13" => "D|4",
				"14" => "S|4",
				"15" => "H|4",
				"16" => "C|5",
				"17" => "D|5",
				"18" => "S|5",
				"19" => "H|5",
				"20" => "C|6",
				"21" => "D|6",
				"22" => "S|6",
				"23" => "H|6",
				"24" => "C|7",
				"25" => "D|7",
				"26" => "S|7",
				"27" => "H|7",
				"28" => "C|8",
				"29" => "D|8",
				"30" => "S|8",
				"31" => "H|8",
				"32" => "C|9",
				"33" => "D|9",
				"34" => "S|9",
				"35" => "H|9",
				"36" => "C|10",
				"37" => "D|10",
				"38" => "S|10",
				"39" => "H|10",
				"40" => "C|11",
				"41" => "D|11",
				"42" => "S|11",
				"43" => "H|11",
				"44" => "C|12",
				"45" => "D|12",
				"46" => "S|12",
				"47" => "H|12",
				"48" => "C|0",
				"49" => "D|0",
				"50" => "S|0",
				"51" => "H|0" 
		);
		settype ( $this->resultString, "string" );
		$result = new StringTokenizer ( $this->resString, "@" );
		$count = $result->countTokens ();
		$i = 0;
		$handList = array ();
		$totalbet = 0;
		$totalwin = 0;
		while ( $hand = $result->nextToken () ) {
			if ($i < ($count)) {
				list ( $phase, $cards ) = explode ( "!", $hand );
                if($i==0){
                    list($ticketsts,$playerhand)=explode('&',$cards);
                    $playercard=explode(';',$playerhand);
                    foreach($playercard as $k=>$v){
                        $playercard[$k]=substr($v,2);
                    }
                    $cards=implode(',',$playercard);
                }
                $cardsList = split ( ",", $cards );

				foreach ( $cardsList as $k => $v ) {
					foreach ( $cards_arr as $k2 => $v2 ) {
						if ($v == $k2) {
							$cardsList [$k] = $v2;
						}
					}
				}
				for($z = 0; $z < count ( $cardsList ); $z ++) {
					list ( $cardType, $cardValue ) = split ( "\|", $cardsList [$z] );
					$cardsList [$z] = getCard ( strtolower ( trim ( $cardValue ) . "" . trim ( $cardType ) ) );
				}
				array_push ( $handList, array (
						"cards" => $cardsList,
						'phase' => $phase 
				) );
			}
			$i ++;
		}
		$html = "<div class='container " . $this->path . "'>";
        //* modified by Razvan//*
        $erase = "handerased";
        $poserased=strpos($this->betString,$erase);
        $reserased=strpos($this->resultString,$erase);
        if($poserased!==false || $reserased!==false ){
            $html .= "<h3 style='color:red;text-align: center'>HAND ERASED</h3><br />";
        }
        //* modified by Razvan//*
		$html .= "<div class='box " . $this->path . "'>";
		$html .= "<table><tr>";
		for($i = 0; $i < count ( $handList ); $i ++) {
			$hand = $handList [$i];
			$str = "";
			$html .= "<td><table style='float:left'>";
			$str = self::getPhase ( $hand ['phase'] );
			$html .= "<th colspan='5' class='label'>$str</th><tr>";
			$cards = $hand ["cards"];
			if ($hand ['phase'] == '0') {
				for($z = 0; $z < count ( $cards ); $z ++) {
					if ($z % 2 == 0) {
						$html .= "<tr><td><b> Player " . ($z / 2 + 1) . ":</b></td>";
					}
					$html .= "<td";
					$html .= ">$cards[$z]</td>";
					if ($z % 2 == 1) {
						$html .= "</tr>";
					}
				}
			} else {
				for($z = 0; $z < count ( $cards ); $z ++) {
					$html .= "<td>$cards[$z]</td>";
				}
				$html .= "</tr></table>";
			}
		}
		$html .= "</td>";
		$html .= "</tr></table><table><tr><td class='label'>Bet Phase</td><td class='label'>Bet On</td><td class='label'>Amount</td><td class='label'>Quote</td><td class='label'>Win</td></tr>";
		list ( $operationid, $betsMade ) = explode ( "@", $this->betString );
		list ( $betsPhase, $bets ) = explode ( "!", $betsMade );
		$bet = explode ( ';', $bets );
		list ( $ticket, $betspayd ) = explode ( '&', $this->resultString );
		$betspayd = split ( ";", $betspayd );
		foreach ( $bet as $betkey => $betvalue ) {
			list ( $betphase, $bettype, $betValue, $betQuote ) = explode ( "-", $betvalue );
			$html .= "<tr><td>" . self::getPhase ( $betphase ) . "</td><td>";
			if ($betphase > 3) {
				$html .= self::getBetValue ( $bettype );
			} else {
				$html .= " Player " . ($bettype + 1);
			}
			$totalbet = $totalbet + $betValue;
			$html .= "	</td><td>" . getInDollars ( $betValue / 100 ) . "</td><td>$betQuote</td><td>";
			foreach ( $betspayd as $betpaydid => $betpaydvalue ) {
				list ( $betid, $payd ) = explode ( "-", $betpaydvalue );
				if ($betkey == $betid) {
					$totalwin = $totalwin + $payd;
					if ($payd > 0) {
						$html .= "<div class='ttm_label'>";
					}
					$html .= getInDollars ( $payd / 100 );
					if ($payd > 0) {
						$html .= "</div>";
					}
				}
			}
			$html .= "</td></tr>";
		}
		$html .= "</table>";
		$html .= "<br><table><tr><td class='label'>Total Bet</td><td class='label'>Total Win</td></tr>";
		$html .= "<tr><td><b>" . getInDollars ( $totalbet / 100 ) . "</b></td><td><b>" . getInDollars ( $totalwin / 100 ) . "</b></td></tr></table>";
		return $html;
	}
	
	private function getPhase($phase) {
		$phases = array (
				"0" => "Hole",
				"1" => "Flop",
				"2" => "Turn",
				"3" => "Final",
				"4" => "Best Hand",
				"5" => "Jackpot" 
		);
		foreach ( $phases as $key => $value ) {
			if ($key == $phase) {
				return $value;
			}
		}
	}
	
	private function getBetValue($bet) {
		$hands_arr = array (
				"0" => "Carta Alta",
				"1" => "Coppia",
				"2" => "Doppia Coppia",
				"3" => "Tris",
				"4" => "Scala",
				"5" => "Colore",
				"6" => "Full",
				"7" => "Poker",
				"8" => "Scala Colore",
				"9" => "Scala Reale" 
		);
		foreach ( $hands_arr as $key => $value ) {
			if ($key == $bet) {
				return $value;
			}
		}
	}
}
?>