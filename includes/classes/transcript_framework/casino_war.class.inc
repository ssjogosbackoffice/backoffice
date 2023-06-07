<?php
require_once 'Transcript.class.inc';
require_once 'transcript.inc.php';
require_once 'general_functions.inc';
class casino_war extends Transcript {
	function casino_war() {
	}
	
	public function getNames() {
		return "casino_war";
	}	
	function getBet() {
		$betData = split ( ";", $this->betString );
		
		//$html = "";
		//cif($betData[1] == 0){
		// $html .= "<b>PLAY IN FREESPIN</b>";        
		//}else{
		//  $html .= "PAY: <b>" . getInDollars($betData[1]/100) . "</b> FOR LINE <br>";
		//  $html .= "TOTAL LINE: <b>" . number_format($betData[0] / $betData[1],0) . " </b>"; 
		// }
		//return $html; 
		return "";
	}	
	//6@p1=4000@S|10;11~D|10;11~4000#
	//      1@p1@H|9;10~S|8;9~0	
	function getReadbleTranscript() {
		$result = new StringTokenizer ( $this->betString, "#" );
		$count = $result->countTokens ();
		$i = 0;
		$handList = array ();
		while ( $result->hasMoreTokens () ) {
			$hand = $result->nextToken ();
			if ($i < ($count)) {
				list ( $operationid, $bet, $hands ) = explode ( "@", $hand );
				list ( $dealer, $player, $win ) = explode ( "~", $hands );
				list ( $dealerCards, $dealerCardsValue ) = explode ( ";", $dealer );
				list ( $cardType, $cardValue ) = split ( "\\|", $dealerCards );
				$dealercardsList = getCard ( strtolower ( trim ( $cardValue ) . "" . trim ( $cardType ) ) );
				list ( $playerCards, $playerCardsValue ) = explode ( ";", $player );
				list ( $cardType, $cardValue ) = split ( "\\|", $playerCards );
				$playercardsList = getCard ( strtolower ( trim ( $cardValue ) . "" . trim ( $cardType ) ) );
				if($operationid==6)
				{
					$bet=substr($bet, 3);
				}
				array_push ( $handList, array ("operationid" => $operationid, "bet" => $bet, "win" => $win, "dealercards" => $dealercardsList, "playercards" => $playercardsList ) );
				$i ++;
			}
		}
		
		$html = "<div align='center' class='container " . $this->path . "'>";
		$html .= "<div class='box " . $this->path . "'><table rules=rows>";
		for($i = 0; $i < count ( $handList ); $i ++) {
			$hand = $handList [$i];
     		if ($hand ["operationid"] == '6') {
	     		$html .= "<tr><td><table cellspacing='10' cellpadding='5'><tr><td class='label'>Dealer Card</td><td class='label'>Player card</td><td class='label'>Bet</td><td class='label'>Win</td></tr>";
				$html .= "<tr><td align='center'>" . $hand ['dealercards'] . "</td><td align='center'>" . $hand ['playercards'] . "</td><td><b>".($hand['bet']/100)." &#8364;</b></td><td><b>".($hand['win']/100)."&#8364;</b></td></tr>";
				$html .= "</table></td></tr>";
				$handbet=($hand['bet']/100);
			}
			if ($hand ["operationid"] == '1') {
				$html .= "<tr><td><table cellpadding='5' cellspacing='10'><th colspan='4' class='formheading'>War</th><tr><td class='label'>Dealer Card</td><td class='label'>Player card</td><td class='label'>Bet</td><td class='label'>Win</td></tr>";
				$html .= "<tr><td align='center'>" . $hand ['dealercards'] . "</td><td align='center'>" . $hand ['playercards'] . "</td><td><b>".($handbet*$i*2)." &#8364;</b></td><td><b>".($hand['win']/100)."&#8364;</b></td></tr>";
				$html .= "</table></td></tr>";
			}
		}
		$html .="</table></div></div>";
		return $html;
	
	}

}

?>