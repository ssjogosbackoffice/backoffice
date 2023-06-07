<?php
define ( "TOTAL_LINES", 25 );
define ( "IMG_PATH", "../media/images/handimages/mafia_25" );
define ( "PAYOUT_PATH", "../media/images/handimages/mafia_25" );
require_once 'Transcript.class.inc';
require_once 'transcript.inc.php';
require_once 'general_functions.inc';
class mafia_25 extends Transcript {
	
	public function getNames() {
		return "mafia_25";
	}
	
	//   125;5
	
	function preparestring() {
		$this->betString= substr ( $this->betString, 2 );
		list ( $this->betString, $this->resultString ) = explode ( "@", $this->betString);
	}
	
	function getBet() {
		self::preparestring();
		$betData = split ( ";", $this->betString );
		
		if ($betData [1] == 0) {
			$html .= "<b>PLAY IN FREESPIN</b>";
		} else {
			$html .= "PAY: <b>" . getInDollars ( $betData [1] / 100 ) . "</b> FOR LINE <br>";
			$html .= "TOTAL LINES: <b>" . number_format ( $betData [0] / $betData [1], 0 ) . "</b>";
		}
		return $html;
	}
	//      3,75,2~5~8|13,75,2~5~8;5|4|1|12|5|1|10|6|1|1|2|4|11|12|12    
	function getReadbleTranscript()
	 {

	 	
	 	self::preparestring();
	 	$betData = split ( ";", $this->betString );
		$html = '';
		$numberlines=$betData[0]/$betData[1];
		$this->setLinesSymbolsPosition ();
		settype ( $this->resultString, "string" );
		$result = new StringTokenizer ( $this->resultString, ";" );
		if ($result->countTokens () >= 2) {
			$winline = $result->nextToken ();
			$symbol = $result->nextToken ();
			$bonus = $result->nextToken ();
			
			$left_lines_html = "";
			$right_lines_html = "<div class='rightline " . $this->path . "'>";
			$left_lines_html .= "<div class='leftline " . $this->path . "'>";
			$totalsLine = array ();
			$right_lines_html .= "<table class='tableline'><tr><td colspan=2 class='label'>Line</td><td class='label'>Bet</td><td class='label'>Win</td></tr>";
			$left_lines_html .= "<table class='tableline'><tr><td colspan=2 class='label'>Line</td><td class='label'>Bet</td><td class='label'>Win</td></tr>";
			
			settype ( $winline, "string" );
			$winning_lines = new StringTokenizer ( $winline, "|" );
			while ( $winning_lines->hasMoreTokens () ) {
				list ( $line_id, $line_win, $symbols_pos ) = explode ( ",", $winning_lines->nextToken () );
				
				$list = array ($line_id, $line_win, $symbols_pos );
				$totalsLine [$line_id] = implode ( ",", $list );
			
		//$winning_lines_html .= "<li>Linea $line_id " . getInDollars($line_win/100) . "<input type='radio' name='lines' class='line' line_pos='".$this->totalLines[$line_id -1]."' line='$symbols_pos'/></li>";
			}
			
			for($i = 0; $i < TOTAL_LINES; $i ++) {
				$line_id = $i + 1;
				$html_str = "";
				if (! is_null ( $totalsLine [$line_id] )) {
					list ( $lines_id, $line_win, $symbols_pos ) = explode ( ",", $totalsLine [$line_id] );
					if($line_id<=$numberlines)
					{
					$html_str .= "<tr>";
					$html_str .= "<td><input type='radio' name='lines' class='line' line_pos='" . $this->totalLines [$line_id - 1] . "' line='$symbols_pos'/></td>";
					$html_str .= "<td class='line_id'> $line_id </td><td>" . getInDollars ( $betData [1] / 100 ) . "</td> <td class='line_dollar'>" . getInDollars ( $line_win / 100 ) . "</td>";
					
					$html_str .= "</tr>";
					}
				} else {
					if($line_id<=$numberlines)
					{
					$html_str .= "<tr>";
					$html_str .= "<td><input type='radio' name='lines' class='line' line_pos='" . $this->totalLines [$line_id - 1] . "'/></td>";
					$html_str .= "<td class='line_id'> $line_id </td><td>" . getInDollars ( $betData [1] / 100 ) . "</td><td class='line_dollar'>" . getInDollars ( 0 ) . "</td>";
					$html_str .= "</tr>";
					}
				
				}
				
				$lineCheck = floor ( TOTAL_LINES / 2 );
				if ($i <= $lineCheck) {
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
			
			$html .= "<div class='container " . $this->path . "'>";
			
			if ($bonus) {
				
				$bonusHtml = "<div id='bonuscontainer'>";
				
				$bonusData = split ( "~", $bonus );
				if (strpos ( $bonusData [0], "B" ) !== false) {
					$bonusHtml .= "<center><table cellpadding='4' cellspacing='0'><tr><td class='label'><b>Bonus</b></td><td class='label'><b>Win</b></td><td class='label'><b>Symbol</b></th></tr>";
					$bonusHtml .= "<tr><td class='content'>" . $this->getBonusImage ( IMG_PATH, $bonusData [0] ) . "</td><td class='content'>" . getInDollars ( $bonusData [1] / 100 ) . "</td><td class='content'><a href=\"javascript:showBonusSymbol('$bonusData[2]');\">Show</a></td></tr>";
					$bonusHtml .= "</table></center></div>";
					
					$html .= $bonusHtml;
				} else if (strpos ( $bonusData [0], "F" ) !== false) {
					$bonusHtml .= "<center><table cellpadding='4' cellspacing='0'><tr><td class='label'><b>FreeSpins</b></td></tr>";
					$bonusHtml .= "<tr><td class='content'>" . $bonusData [2] . "</td></tr>";
					$bonusHtml .= "</table></div></center>";
					$html .= $bonusHtml;
				}
			}			
			$html .= $left_lines_html;
			$html .= "<div class='box " . $this->path . "'>";
			$html .= "<ul id='columnList'>";
			
			$pos = 0;
			while ( $symbolList->hasMoreTokens () ) {
				$sym_id = $symbolList->nextToken ();
				$img = $this->getResultImage ( IMG_PATH, $sym_id );
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
			
			$html .= "<div id='paytable'><table cellpadding='4'>";
			$html .= "<tr><td class='label'>Symbol/<br>multiplier</td> <td class='label'> x 3 </td><td class='label'> x 4 </td><td class='label'> x 5 </td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH, '1' ) . "</td><td>15</td><td>40</td><td>80</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH, '2' ) . "</td><td>25</td><td>50</td><td>100</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH, '3' ) . "</td><td>30</td><td>60</td><td>120</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH, '4' ) . "</td><td>40</td><td>72</td><td>125</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH, '5' ) . "</td><td>50</td><td>80</td><td>140</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH, '6' ) . "</td><td>60</td><td>90</td><td>150</td></tr>";
			$html .= "</table>";
			$html .= "<table cellpadding='4'><tr><td class='label'>Symbol/<br>multiplier</td> <td class='label'> x 2 </td><td class='label'> x 3 </td><td class='label'> x 4 </td><td class='label'> x 5 </td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH, '7' ) . "</td><td>0</td><td>75</td><td>100</td><td>160</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH, '8' ) . "</td><td>5</td><td>80</td><td>120</td><td>175</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH, '9' ) . "</td><td>10</td><td>90</td><td>125</td><td>180</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH, '10' ) . "</td><td>15</td><td>100</td><td>150</td><td>200</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH, '11' ) . "</td><td>15</td><td>100</td><td>150</td><td>200</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH, '12' ) . "</td><td>0</td><td>50</td><td>100</td><td>200</td></tr>";
			$html .= "</table>";
			$html .= "<table cellpadding='4'><tr><td class='label'><center>Bonus <br>symbols</center></td><td class='label'>Position</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH, '8' ) . " " . $this->getResultImage ( IMG_PATH, '9' ) . "</td><td>Colums:2,3,4</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH, '12' ) . " " . $this->getResultImage ( IMG_PATH, '2' ) . "</td><td>Colums:2,3,4</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH, '5' ) . " " . $this->getResultImage ( IMG_PATH, '10' ) . "</td><td>Anywhere</td></tr>";
			$html .= "</table></div>";
		}
		
		return $html;
	
	}
	
	function setLinesSymbolsPosition() {
		
		$this->totalLines = array ("1;4;7;10;13", "0;3;6;9;12", "2;5;8;11;14", "0;4;8;10;12", "2;4;6;10;14", "0;3;7;11;14", "2;5;7;9;12", "1;5;7;9;12", "1;3;7;11;14", "0;4;7;10;14", "2;4;7;10;12", "0;3;6;10;14", "2;5;8;10;12", "1;4;8;10;12", "1;4;6;10;14", "1;5;8;10;12", "1;3;6;10;14", "2;5;7;9;13", "0;3;7;11;13", "2;4;6;9;12", "0;4;8;11;14", "1;3;7;11;13", "1;5;7;9;13", "2;4;6;10;12", "0;4;8;10;14" );
	
	}
}

?>