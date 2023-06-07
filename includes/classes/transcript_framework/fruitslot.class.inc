<?php
define ( "TOTAL_LINES2", 9 );
define ( "IMG_PATH2", "../media/images/handimages/slot_fruit" );
define ( "PAYOUT_PATH2", "../media/images/handimages/slot_fruit" );
require_once 'Transcript.class.inc';
require_once 'transcript.inc.php';
class fruitslot extends Transcript {	
	public function getNames() {
		return "fruitslot";
	}
	//   125;5
	//6@spin_18!9~1-2-3-4-5-6-7-8-9@25.0@2_5_8_11_14-1-3-25.0@false-0@false@11|1|6|3|1|0|7|1|7|11|9|0|8|10|10
	function getBet() {
		//		$betData = split ( ";", $this->betString );
	//		$html = "<center><table>";
	//		if ($betData [1] == 0) {
	//			$html .= "<tr><td><b>PLAY IN FREESPIN</b></td></tr>";
	//		} else {
	//			$html .= "<tr><td class='label'>PAY: </td><td><b>" . getInDollars ( $betData [1] ) . "</b> FOR LINE </td>";
	//			$html .= "<tr><td class='label'>TOTAL LINE:</td><td> <b>" . number_format ( $betData [0] / $betData [1], 0 ) . "</b></td></tr>";
	//		}
	//		$html .= "</table></center>";
	//		return $html;
	}
	// 3,75,2~5~8|13,75,2~5~8;5|4|1|12|5|1|10|6|1|1|2|4|11|12|12    
	// 6@spin_18!9~1-2-3-4-5-6-7-8-9@25.0@2_5_8_11_14-1-3-25.0@false-0@false@11|1|6|3|1|0|7|1|7|11|9|0|8|10|10
	// 6@spin_18!0.45~1-2-3-4-5-6-7-8-9@5.4999995@1_5_9_11_13-11-2-2.99~2_6_9_12_14-10-2-2.49@true-2@false@11|10|6|7|11|10|0|6|6|6|6|4|8|3|9
	//49@bonus_18!red@0.0@C_8@false-0@false@0|7|11|11|11|2|9|2|1|7|4|6|10|8|0
	function getReadbleTranscript() {
		
		$html = '';
		$this->setLinesSymbolsPosition ();
		settype ( $this->betStringg, "string" );
		$result = new StringTokenizer ( $this->betString, "@" );
		if ($result->countTokens () >= 2) {
			if ($this->betString[0] == "6") {
				$bettype = $result->nextToken ();
				$type = $result->nextToken ();
				list ( $typebet, $bettotal ) = explode ( "!", $type );
				list ( $totalbet, $lines ) = explode ( "~", $bettotal );
				$linesbet = split ( "-", $lines );
				$linebet = $totalbet / (count ( $linesbet ));
				$win = $result->nextToken ();
				$winline = $result->nextToken ();
				$freespins = $result->nextToken ();
				$bonus = $result->nextToken ();
				$symbol = $result->nextToken ();
			} else {
				$bettype = $result->nextToken ();
				$bet = $result->nextToken ();
				$win = $result->nextToken ();
				$resultcard = $result->nextToken ();
				list ( $cardType, $cardValue ) = split ( "_", $resultcard );
				$cardresult = getCard ( strtolower ( trim ( $cardValue ) . "" . trim ( $cardType ) ) );
				$freespins = $result->nextToken ();
				$bonus = $result->nextToken ();
				$symbol = $result->nextToken ();
			}
			$left_lines_html = "";
			$left_lines_html .= "<div class='leftline slot_fruit'>";
			$totalsLine = array ();
			$left_lines_html .= "<table class='tableline'><tr><td colspan=2 class='label'>Line</td><td class='label'>Bet</td><td class='label'>Win</td></tr>";
			settype ( $winline, "string" );
			$winning_lines = new StringTokenizer ( $winline, "~" );
			while ( $winning_lines->hasMoreTokens () ) {
				list ( $symbols_pos, $symbolwin_id, $symbol_occurance, $line_win ) = explode ( "-", $winning_lines->nextToken () );
				$symbols_pos = str_replace ( "_", ";", $symbols_pos );
				foreach ( $this->totalLines as $k => $v ) {
					if ($v == $symbols_pos) {
						$line_id = $k + 1;
					}
				}
				$list = array ($line_id, $symbols_pos, $symbolwin_id, $symbol_occurance, $line_win );
				$totalsLine [$line_id] = implode ( ",", $list );
			}			
			$symbols = explode ( "|", $symbol );
			for($z = 0; $z < count ( $symbols ); $z ++) {
				if ($symbols [$z] == 12) {
					$bonussymbol = $z + 1;
				}
			}
			for($i = 0; $i < TOTAL_LINES2; $i ++) {
				$line_id = $i + 1;
				$html_str = "";
				if (! is_null ( $totalsLine [$line_id] )) {
					list ( $line_id, $symbols_pos, $symbolwin_id, $symbol_occurance, $line_win ) = explode ( ",", $totalsLine [$line_id] );
					$html_str .= "<tr>";
					$linesymbols = explode ( ";", $symbols_pos );
					$occurance = 0;
					$winning_symbols = array ();
					for($j = 0; $j < count ( $linesymbols ); $j ++) {						
						if ($symbols [$linesymbols [$j] - 1] == $symbolwin_id) {
							if($line_id<=13){
							$occurance = $occurance + 1;
							if ($occurance <= $symbol_occurance) {
								array_push ( $winning_symbols, $linesymbols [$j] );
							}
							}
							else
							{
								array_push ( $winning_symbols, $linesymbols [$j] );
								$startingpos=count($winning_symbols) -$symbol_occurance;
								$winning_symbols=array_slice($winning_symbols, $startingpos);
							}
						}
					}
					$symbols_winning = implode ( "~", $winning_symbols );
					if($line_id<=count ( $linesbet ))
     {
					$html_str .= "<td><input type='radio' name='lines' class='line' line_pos='" . $this->totalLines [$line_id - 1] . "' line='$symbols_winning'/></td>";
					$html_str .= "<td class='line_id'> $line_id </td><td>" . getInDollars ( $linebet ) . "</td> <td class='line_dollar'>" . getInDollars ( $line_win ) . "</td>";
					$html_str .= "</tr>";
     }
				} else {
					if($line_id<=count ( $linesbet ))
					{
					$html_str .= "<tr>";
					$html_str .= "<td><input type='radio' name='lines' class='line' line_pos='" . $this->totalLines [$line_id - 1] . "'/></td>";
					$html_str .= "<td class='line_id'> $line_id </td><td>" . getInDollars ( $linebet ) . "</td><td class='line_dollar'>" . getInDollars ( 0 ) . "</td>";
					$html_str .= "</tr>";
					}
				}
				$left_lines_html .= $html_str;
			}
			$left_lines_html .= "</table>";
			$left_lines_html .= "</div>";
			settype ( $symbol, "string" );
			$symbolList = new StringTokenizer ( $symbol, "|" );
			$html .= "<div class='container slot_fruit'>";
			$bonusHtml="<center><table><tr>";
			if ($bet) {
				$bonusHtml .= "<td><div id='bonuscontainer'>";
				$bonusData = split ( "!", $bet );
				if (strpos ( $bonusData [0], "b" ) !== false) {
					if ($bonusData [1] == 'H') {
						$bonusData [1] = 'Hearts';
					} elseif ($bonusData [1] == 'C') {
						$bonusData [1] = 'Clubs';
					} elseif ($bonusData [1] == 'D') {
						$bonusData [1] = 'Diams';
					} elseif ($bonusData [1] == 'S') {
						$bonusData [1] = 'Spades';
					}
					$bonusHtml .= "<center><table cellpadding='4' cellspacing='0'><tr><td class='formheading' colspan='3'>BonusSymbol</td></tr><tr><td class='label'><b>Bet</b></td><td class='label'><b>Result</b></td><td class='label'><b>Win</b></th></tr>";
					$bonusHtml .= "<tr><td class='content'>" . $bonusData [1] . "</td><td class='content'>" . $cardresult . "</td><td class='content'>$win</td></tr>";
					$bonusHtml .= "</table></center></div></td>";
				
				}
			}
			if ($bonussymbol) {
				$bonusHtml .= "<td><div id='bonuscontainer'>";
				$bonusHtml .= "<table cellpadding='4' cellspacing='0'><tr><td class='label'><b>Bonus</b></td><td class='label'><b>Win</b></td><td class='label'><b>Symbol</b></th></tr>";
				$bonusHtml .= "<tr><td class='content'>" . $this->getResultImage ( IMG_PATH2, '13' ) . "</td><td class='content'>" . getInDollars ( 50 ) . "</td><td class='content'><a href=\"javascript:showBonusSymbol('$bonussymbol');\">Show</a></td></tr>";
				$bonusHtml .= "</table></div></td>";		
			}			
			if ($freespins) {
				$bonusHtml .= "<td><div id='bonuscontainer'>";
				$bonusData = split ( "-", $freespins );
				if (strpos ( $bonusData [0], "t" ) !== false) {
					$bonusHtml .= "<center><table cellpadding='4' cellspacing='0'><tr><td class='label'><b>FreeSpins</b></td></tr>";
					$bonusHtml .= "<tr><td class='content'>" . $bonusData [1] . "</td></tr>";
					$bonusHtml .= "</table></div></center></td>";
				}
			}
			$bonusHtml.="</tr></table></center>";
			$html .= $bonusHtml;
			$html .= $left_lines_html;
			$html .= "<div class='box slot_fruit'>";
			$html .= "<ul id='columnList'>";
			$pos = 0;
			while ( $symbolList->hasMoreTokens () ) {
				$sym_id = $symbolList->nextToken ();
				$img = $this->getResultImage ( IMG_PATH2, $sym_id + 1 );
				if ($pos % 3 == 0) {
					$html .= "<li id='column_" . ($pos / 3) . "'><ul>";
				}
				$html .= "<li class='symbol' id='sym_" . ($pos + 1) . "'>" . $img . "</li>";
				$pos ++;
				if ($pos % 3 == 0) {
					$html .= "</ul></li>";
				}
			}
			$html .= "</ul>";
			$html .= "<br style='clear:both;'/>";
			$html .= "</div>";
			$html .= "<div id='paytable'><table cellpadding='4'>";
			$html .= "<tr><td class='label'>Symbol/<br>multiplier</td> <td class='label'> x 3 </td><td class='label'> x 4 </td><td class='label'> x 5 </td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH2, '1' ) . "</td><td>15</td><td>40</td><td>80</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH2, '2' ) . "</td><td>25</td><td>50</td><td>100</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH2, '3' ) . "</td><td>30</td><td>60</td><td>120</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH2, '4' ) . "</td><td>40</td><td>72</td><td>125</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH2, '5' ) . "</td><td>50</td><td>80</td><td>140</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH2, '6' ) . "</td><td>60</td><td>90</td><td>150</td></tr>";
			$html .= "</table>";
			$html .= "<table cellpadding='4'><tr><td class='label'>Symbol/<br>multiplier</td> <td class='label'> x 2 </td><td class='label'> x 3 </td><td class='label'> x 4 </td><td class='label'> x 5 </td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH2, '7' ) . "</td><td>0</td><td>75</td><td>100</td><td>160</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH2, '8' ) . "</td><td>5</td><td>80</td><td>120</td><td>175</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH2, '9' ) . "</td><td>45</td><td>90</td><td>125</td><td>180</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH2, '10' ) . "</td><td>30</td><td>65</td><td>95</td><td>100</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH2, '11' ) . "</td><td>50</td><td>100</td><td>150</td><td>200</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH2, '12' ) . "</td><td>60</td><td>120</td><td>160</td><td>220</td></tr>";
			$html .= "</table>";
			$html .= "<table cellpadding='4'><tr><td class='label'><center>Bonus/<br>Freespins</center></td><td class='label'><center>Description</center></td></tr>";
			$html .= " <tr><td class='label'><center>" . $this->getResultImage ( IMG_PATH2, '13' ) . "</center></td><td><b>Ti fa vincere 50 &#8364;</b></td></tr>";
			$html .= " <tr><td class='label bonusfreespin'>" . $this->getResultImage ( IMG_PATH2, '14' ) . "</td><td><b>Con una combinatione vincente<br>a partire da sinistra ottieni<br> crediti bonus</b></td></tr>";
			$html .= " <tr><td class='label bonusfreespin'>" . $this->getResultImage ( IMG_PATH2, '15' ) . "</td><td><b>Con una combinatione vincente<br>a partire da sinistra ottieni <br>free spins</b></td></tr>";
			$html .= "</table></div>";
		}
		return $html;
	}
	
	function setLinesSymbolsPosition() {
		$this->totalLines = array ("2;5;8;11;14", 
								   "1;4;7;10;13", 
								   "3;6;9;12;15", 
								   "1;5;9;11;13", 
								   "3;5;7;11;15", 
								   "1;4;8;10;13", 
								   "3;6;8;12;15", 
								   "2;4;7;10;14", 
								   "2;6;9;12;14" );
	}
}
?>