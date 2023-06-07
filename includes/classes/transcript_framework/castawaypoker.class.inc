<?php
require_once 'Transcript.class.inc';
require_once 'transcript.inc.php';
class castawaypoker extends Transcript {
	function castawaypoker() {
	}
	
	public function getNames() {
		return "Cast Away";
	}
	
	function getBet() {
		$betData = split ( ";", $this->betString );
		$html = "";
		//   if($betData[1] == 0){
		//   $html .= "<b>PLAY IN FREESPIN</b>";        
		//   }else{
		//   $html .= "PAY: <b>" . getInDollars($betData[1]/100) . "</b> FOR LINE <br>";
		//   $html .= "TOTAL LINE: <b>" . number_format($betData[0] / $betData[1],0) . " </b>"; 
		//  	}
		return "";
	}
	
	function getReadbleTranscript() {
		/* 
         	6@25@p1=S|2;C|1;D|12;C|12;S|3~0;0;1;1;0~0#        
        	1@p1=0;0;1;1;0@p1=S|6;C|7;D|12;C|12;D|4~0;0;1;1;0~25# 
        	3@p1=0;0;1;1;0@p1=H|3;H|8;D|12;C|12;S|2~0;0;1;1;0~50#
         	4@p1=0@p1=S|5~100 
         	
         	6@0@p1=D|2;S|8;C|11;D|10;D|9~0;0;0;0;0~0;5;0;0;0~0#
         	1@p1=0;1;1;1;1@p1=P|0;S|8;C|11;D|10;D|9~1;1;1;1;1~5;5;0;0;0~110
         	         	
         	6@100@p1=C|10;D|10;S|9;C|1;H|12~0;0;0;0;0~0#
         	1@p1=1;1;0;0;0@p1=C|10;D|10;P|0;P|0;H|8~1;1;1;1;0~300#
         	5@p1=1@C|11;C|6;H|8;S|7;D|1
         	
         	6@25@p1=S|10;C|9;D|7;S|12;H|12~0;0;0;0;0~0#
         	1@p1=0;0;0;1;1@p1=P|0;H|6;C|2;S|12;H|12~1;0;0;1;1~25#
         	5@p1=2@H|0;C|3;H|6;D|4;D|8
         	
         	6@25@p1=H|8;S|10;S|2;C|5;S|8~0;0;0;0;0~0#
         	1@p1=0;1;1;0;1@p1=P|0;S|10;S|2;P|0;S|8~1;1;1;1;1~0;0;0;0;0~50
         	
         	6@0@p1=H|3;C|9;S|0;H|6;S|4~0;0;0;0;0~5;0;0;5;0~0#
         	1@p1=0;0;0;0;0@p1=P|0;P|0;H|9;D|3;H|7~1;1;1;0;0~5;5;5;0;5~2520   
         	
         	6@2500@p1=S|0;H|11;D|8;C|6;S|3~0;0;0;0;0~0#
         	1@p1=0;0;0;0;0@p1=P|0;P|0;C|11;C|5;C|12~1;1;1;1;1~0;0;0;0;0~5000
        
         */
		settype ( $this->betString, "string" );
		$result = new StringTokenizer ( $this->betString, "#" );
		$count = $result->countTokens ();
		$i = 0;
		$handList = array ();
		$firstHand;
		while ( $result->hasMoreTokens () ) {
			$hand = $result->nextToken ();
			if ($i < ($count)) {
				list ( $operationid, $bet, $hands ) = explode ( "@", $hand );
				if ($i == 0) {
					$firstHand = $bet;
				}
				if ($firstHand == "0") {
					list ( $cards, $hold, $spincards, $win ) = explode ( "~", $hands );
					$freespin = " FREESPIN <br/> ";
				} else {
					list ( $cards, $hold, $win ) = explode ( "~", $hands );
				}
				$holdList = split ( ";", $hold );
				$cardsList = split ( ";", $cards );
				for($z = 0; $z < count ( $cardsList ); $z ++) {
					if ($z == 0 && $operationid != 5) {
						$cardsList [0] = substr ( $cardsList [0], 3 );
					}
					list ( $cardType, $cardValue ) = split ( "\\|", $cardsList [$z] );
					$cardsList [$z] = getCard ( strtolower ( trim ( $cardValue ) . "" . trim ( $cardType ) ) );
				}
				array_push ( $handList, array ("operationid" => $operationid, "bet" => $bet, "win" => $win, "cards" => $cardsList, "hold" => $holdList ) );
				$i ++;
			}
			$html = "<div class='container " . $this->path . "'>";
			$html .= "<div class='box " . $this->path . "'>";
			$html .= "<table rules=rows>";
			for($i = 0; $i < count ( $handList ); $i ++) {
				$hand = $handList [$i];
				$previoushand = $handList [$i - 1];
				$nexthand = $handList [$i + 1];
				$wins = $hand ["win"] / 100;
				$str = "";
				$html .= "<tr><td><table cellspacing=7 cellpadding=2 >";
				switch ($i) {
					case 0 :
						{
							$str = $freespin . " First Hand";
							switch ($hand ["operationid"]) {
								case 1 :
									{
										$str .= " - Add ";
									}
									break;
								
								case 5 :
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
						$str = "<br/>Second Hand";
						switch ($hand ["operationid"]) {
							case "1" :
								{
									$str .= " - Add ";
								}
							break;
							
							case "5" :
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
					
					case 2 :
						{
							$str = "  Bonus";
							$bonuswin = ($previoushand ["win"] / 100) * 2;
						}
						break;
					
					case 3 :
						{
							$str = " Bonus - Second time ";
							$bonuswin = $bonuswin * 2;
						}
						break;
					
					case 4 :
						{
							$str = " Bonus - Third time ";
							$bonuswin = $bonuswin * 2;
						}
						break;
					
					case 5 :
						{
							$str = " Bonus - Fourth time ";
							$bonuswin = $bonuswin * 2;
						}
						break;
					
					case 3 :
						{
							$str = " Bonus - Fifth time ";
							$bonuswin = $bonuswin * 2;
						}
						break;
					
					default :
						{
							$str = " <br/> Double";
						}
						break;
				}
				if ($hand ["operationid"] != "5") {
					$html .= "<th colspan='5'>$str</th><tr>";
				} else {
					$html .= "<th colspan='5'>$str</th><tr><td><b>Higher than:</b></td><td colspan='4'></td></tr><tr>";
				}
				$cards = $hand ["cards"];
				$hold = $hand ["hold"];
				if ($hand ["operationid"] != "6") {
					$hand ["bet"] = substr ( $hand ["bet"], 3 );
				}
				for($z = 0; $z < count ( $cards ); $z ++) {
					$html .= "<td";
					if ($hold [$z] == 1) {
						if ($wins != '0') {
							$html .= " class='wincard'";
						} else {
							$html .= " class='hold'";
						}
						
					}
					if ($hand ["operationid"] == 5){
						if($cards[$hand["bet"]]==$cards[$z]){
							if($nexthand!="")
							{
							$html .=" class='wincard'";
							}
							else
							{
								$html .=" class='hold'";
							}
							
						}						
						}
					$html .= "> $cards[$z]</td>";
				}
				$html .= "</tr></table></td><td class='vline'> </td>";
				$html .= " <td>";
				if ($hand ["operationid"] == 6) {
					$hand ["bet"] = $hand ["bet"] / 100;
					$_SESSION ['bet'] = $hand ["bet"];
					$html .= "<b><br>Bet: " . $hand ["bet"] . " &#8364;<b><br/>";
				}
				if ($hand ["operationid"] == 5) {					
					//$previoushand["win"]=($wins/2)*100;
					$html .= "<table cellspacing=7 cellpadding=2><tr><td><b><br>Player's choise:</b></td><tr><td>" .$cards [$hand ["bet"]]."</td></tr><tr><td><b>Win: ";
					if ($nexthand == "") {
						$html .= "0";
					} else {
						$html .= $bonuswin;
					}
					$html .= " &#8364</b></td></tr></table>";
				} else {
					$html .= "<br /><b>Win: " . $wins . " &#8364;</b>";
				}
				$html .= "</td>";
			}
		}
		$html .= "</tr></table>";
		return $html;
	}
}
?>