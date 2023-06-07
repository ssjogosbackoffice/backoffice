<?php
define ( "TOTAL_LINES_SPHINX", 10 );
define ( "IMG_PATH_SPHINX", "../media/images/handimages/sphinx" );
define ( "DICES_SPHINX", "../media/images/handimages/sphinx/dices" );
require_once 'Transcript.class.inc';
require_once 'transcript.inc.php';
require_once 'general_functions.inc';
class sphinx extends Transcript {
	public function getNames() {
		return "sphinx";
	}
	function preparestring() {
		$this->betString = substr ( $this->betString, 2 );
	}
	function getBet() {
		self::preparestring ();
		$betData = split ( ";", $this->betString );
		$html = "";
		if ($betData [1] == 0) {
			$html .= "<b>PLAY IN FREESPIN</b>";
		} else {
			$html .= "PAY: <b>" . getInDollars ( $betData [1] / 100 ) . "</b> FOR LINE <br>";
			$html .= "TOTAL LINES: <b>" . number_format ( $betData [0] / $betData [1], 0 ) . "</b>";
		}
		return $html;
	}
	
	function getReadbleTranscript() {
		self::preparestring ();
		$html = '';
		$this->setLinesSymbolsPosition ();
		settype ( $this->betString, "string" );
		list ( $bets, $hand, $subId, $dices ) = explode ( "@", $this->betString );
		list ( $bet, $betperline ) = explode ( ";", $bets );
		$numberlines = $bet / $betperline;
		$result = new StringTokenizer ( $hand, "#" );
		if ($result->countTokens () >= 2) {
			$totalwin = $result->nextToken ();
			$handDetailssymbol = $result->nextToken ();
			list($symbol,$multiplier,$winline)=explode('-',$handDetailssymbol);
			$jackpot1 = $result->nextToken ();
			$bonus = $result->nextToken ();
			$jackpot = $result->nextToken ();
			$left_lines_html = "";
			$right_lines_html = "<div class='rightline " . $this->path . "'>";
			$left_lines_html .= "<div class='leftline " . $this->path . "'>";
			$totalsLine = array ();
			$right_lines_html .= "<table class='tableline'><tr><td colspan=2 class='label'>Line</td><td class='label'>Bet</td><td class='label'>Win</td></tr>";
			$left_lines_html .= "<table class='tableline'><tr><td colspan=2 class='label'>Line</td><td class='label'>Bet</td><td class='label'>Win</td></tr>";
			settype ( $winline, "string" );
			$winning_lines = new StringTokenizer ( $winline, ";" );
			while ( $winning_lines->hasMoreTokens () ) {
				list ( $line_id,  $winningSymbol,$symbols_pos,$line_win ) = explode ( "~", $winning_lines->nextToken () );
				$symbols_pos = str_replace ( "|", "~", $symbols_pos );
				$list = array (
						$line_id,
						$symbols_pos,
						$line_win 
				);
				$totalsLine [$line_id] = implode ( ",", $list );
			}
			
			for($i = 0; $i < TOTAL_LINES_SPHINX; $i ++) {
				$line_id = $i + 1;
				$html_str = "";
				if (! is_null ( $totalsLine [$line_id] )) {
					list ( $line_id, $symbols_pos, $line_win ) = explode ( ",", $totalsLine [$line_id] );
					if ($line_id <= $numberlines) {
						$html_str .= "<tr>";
						$html_str .= "<td><input type='radio' name='lines' class='line' line_pos='" . $this->totalLines [$line_id - 1] . "' line='$symbols_pos'/></td>";
						$html_str .= "<td class='line_id'> $line_id </td><td>" . getInDollars ( $betperline / 100 ) . "</td> <td class='line_dollar'>" . getInDollars ( $line_win / 100 ) . "</td>";
						
						$html_str .= "</tr>";
					}
				} else {
					if ($line_id <= $numberlines) {
						$html_str .= "<tr>";
						$html_str .= "<td><input type='radio' name='lines' class='line' line_pos='" . $this->totalLines [$line_id - 1] . "'/></td>";
						$html_str .= "<td class='line_id'> $line_id </td><td>" . getInDollars ( $betperline / 100 ) . "</td><td class='line_dollar'>" . getInDollars ( 0 ) . "</td>";
						$html_str .= "</tr>";
					}
				}
				
				$lineCheck = floor ( TOTAL_LINES_SPHINX / 2 );
				if ($i < $lineCheck) {
					$left_lines_html .= $html_str;
				} else {
					$right_lines_html .= $html_str;
				}
			}
			
			$right_lines_html .= "</table>";
			$left_lines_html .= "</table>";
			
			$left_lines_html .= "</div>";
			$right_lines_html .= "</div>";
			settype ( $symbol, "string" );
			$symbolList = new StringTokenizer ( $symbol, "|" );
			
			$html .= "<div class='centered container " . $this->path . "'>";

					if ($dices != '') {
		/*	$html .= "<table>
					<tr><td class='label'>Dice Game Results</td></tr>
					<tr><td>";
			$dice = new StringTokenizer ( $dices, '#' );
			while ( $dice->hasMoreTokens () ) {
				$alldices = $dice->nextToken ();
				if ($alldices != '') {
					$html .= "<table><tr>";
					list ( $currDice, $winonthis ) = explode ( ";", $alldices );
					list ( $winamount, $winningdice ) = explode ( "-", $winonthis );
					$thisDices = new StringTokenizer ( $currDice, "-" );
					While ( $thisDices->hasMoreTokens () ) {
						$dicethis = $thisDices->nextToken ();
						list ( $thisDice, $betamount ) = explode ( ":", $dicethis );
						$html .= "<td>" . $this->getResultImage ( DICES_SPHINX, "$thisDice" ) . "<br>$betamount</td>";
					}
					$html .= "<td>Result " . $this->getResultImage ( DICES_SPHINX, "$winningdice" ) . "<br> Won " . getInDollars ( $winamount / 100 ) . " </td></tr><table>";
				}
			}
			$html .= "</td></tr></table>";*/

			$html.="<table style='margin:auto'>
					<tr><td class='label' colspan='2'>Dice Game Results</td></tr>";
			$dice = new StringTokenizer ( $dices, '#' );
			$count=1;
			while ( $dice->hasMoreTokens () ) {
				$alldices = $dice->nextToken ();
				if ($alldices != '') {
				list ( $currDice, $winonthis ) = explode ( ";", $alldices );
				$html.="<tr><td>".$currDice." ".$count."</td><td>".getInDollars($winonthis/100)."</td></tr>";
				$count++;
				}
			}
			$html.="</table>";
		}
			if ($bonus) {
			$bonusData = split ( "~", $bonus );
				$bonusHtml = "<div id='bonuscontainer'>";
				$bonusHtml .= "<center><table cellpadding='4' cellspacing='0'><tr><td class='label'><b>Bonus</b></td><td class='label'><b>".getInDollars($bonusData[1]/100)."</b></td></tr>";
                $bonusHtml .= "</table></center></div>";
                $html.=$bonusHtml;
			/*	$bonusData = split ( "~", $bonus );
				if (strpos ( $bonusData [0], "B" ) !== false) {
					$bonusHtml .= "<center><table cellpadding='4' cellspacing='0'><tr><td class='label'><b>Bonus</b></td><td class='label'><b>Win</b></td><td class='label'><b>Symbol</b></th></tr>";
					$bonusHtml .= "<tr><td class='content'>" . $this->getBonusImage ( IMG_PATH_SPHINX, $bonusData [0] ) . "</td><td class='content'>" . getInDollars ( $bonusData [1] / 100 ) . "</td><td class='content'><a href=\"javascript:showBonusSymbol('$bonusData[2]');\">Show</a></td></tr>";
					$bonusHtml .= "</table></center></div>";
					
					$html .= $bonusHtml;
				} else if (strpos ( $bonusData [0], "F" ) !== false) {
					$bonusHtml .= "<center><table cellpadding='4' cellspacing='0'><tr><td class='label'><b>FreeSpins</b></td></tr>";
					$bonusHtml .= "<tr><td class='content'>" . $bonusData [2] . "</td></tr>";
					$bonusHtml .= "</table></div></center>";
					$html .= $bonusHtml;
				}*/
			}
			$html .= $left_lines_html;
			$html .= "<div class='box " . $this->path . "'>";
			$html .= "<ul id='columnList'>";
			
			$pos = 0;
			while ( $symbolList->hasMoreTokens () ) {
				$sym_id = $symbolList->nextToken ();
				$img = $this->getResultImage ( IMG_PATH_SPHINX, $sym_id );
				if ($pos % 3 == 0) {
					$html .= "<li id='column_" . ($pos / 3) . "'><ul>";
				}
				$html .= "<li class='symbol' id='sym_$pos'>" . $img . "</li>";
				$pos ++;
				if ($pos % 3 == 0) {
					$html .= "</ul></li>";
				}
			}
			
			$html .= "</ul>";
			$html .= "<br style='clear:both;'/>";
			$html .= "</div>";
			$html .= $right_lines_html;


			$html .= "<div id='paytable_sphinx'><table cellpadding='4'>";
			$html .= "<tr><td class='label'>Symbol/<br>multiplier</td> <td class='label'> x 3 </td><td class='label'> x 4 </td><td class='label'> x 5 </td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_SPHINX, '0' ) . "</td><td>5</td><td>10</td><td>100</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_SPHINX, '2' ) . "</td><td>5</td><td>10</td><td>100</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_SPHINX, '1' ) . "</td><td>5</td><td>10</td><td>100</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_SPHINX, '3' ) . "</td><td>10</td><td>20</td><td>175</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_SPHINX, '4' ) . "</td><td>10</td><td>30</td><td>200</td></tr>";
			
			$html .= "<table cellpadding='4'><tr><td class='label'>Symbol/<br>multiplier</td> <td class='label'> x 2 </td><td class='label'> x 3 </td><td class='label'> x 4 </td><td class='label'> x 5 </td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_SPHINX, '5' ) . "</td><td>0</td><td>10</td><td>100</td><td>250</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_SPHINX, '6' ) . "</td><td>0</td><td>10</td><td>125</td><td>300</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_SPHINX, '7' ) . "</td><td>0</td><td>10</td><td>250</td><td>400</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_SPHINX, '9' ) . "</td><td>0</td><td>20</td><td>50</td><td>0</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_SPHINX, '8' ) . "</td><td>5</td><td>150</td><td>350</td><td>450</td></tr>";
			$html .= "</table>";
			$html .= "<table cellpadding='4'><tr><td class='label'><center>Bonus <br>symbol</center></td><td class='label'>Description</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_SPHINX, '10' ) . " </td><td>Appear in any<br>  position on a line <br> of active play.<br> It trigger the<br> bonus of the sphinx</td></tr>";
			$html .= "</table></div>";
		}

		
		return $html;
	}
	
	function setLinesSymbolsPosition() {
		$this->totalLines = array (
				"1;4;7;10;13",
				"0;3;6;9;12",
				"2;5;8;11;14",
				"0;4;8;10;12",
				"2;4;6;10;14",
				"0;3;7;9;12",
				"2;5;7;11;14",
				"1;5;8;11;13",
				"1;3;6;9;13",
				"0;5;8;11;12" 
		);
	}
}
?>