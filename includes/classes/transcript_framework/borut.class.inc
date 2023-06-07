<?php
define ( "TOTAL_LINES5", 25 );
define ( "IMG_PATH5", "../media/images/handimages/wildabouthot" );
define ( "PAYOUT_PATH5", "../media/images/handimages/wildabouthot" );
require_once 'Transcript.class.inc';
require_once 'transcript.inc.php';
require_once 'general_functions.inc';
class borut extends Transcript {	
	public function getNames() {
		return "borut";
	}	
	//   125;5	
	function getBet() {
		$betData = split ( ";", $this->betString );	
		$html = "<center><table>";
		$betData[1]=strstr($betData[1],"@",true);		
		if ($betData [1] == 0) {
			$html .= "<tr><td><b>PLAY IN FREESPIN</b></td></tr>";
		} else {			
			$html .= "<tr><td>PAY: </td><td><b>" . getInDollars ( $betData [1] / 100 ) . "</b> FOR LINE </td>";
			$html .= "<tr><td>TOTAL LINES:</td><td><b>" . number_format ( $betData [0] / $betData [1], 0 ) . "</b></td></tr>";
		}
		$html .= "</table></center>";
		return $html;
	}
	//      3,75,2~5~8|13,75,2~5~8;5|4|1|12|5|1|10|6|1|1|2|4|11|12|12    
	//		500;50@5,2500,2~4~6;5|7|3|9|3|5|3|0|1|5|0|9|3|4|7
	function getReadbleTranscript() {		
		$html = '';
		$this->setLinesSymbolsPosition ();
		$this->path = "borut";
		settype ( $this->betString, "string" );
		$result = new StringTokenizer ( $this->betString, "@" );
		if ($result->countTokens () >= 2) {
			$bets = $result->nextToken ();
			$betData = split ( ";", $bets);
			list($totalbet,$linebet)=explode(";", $bets);
			$numberlines=$totalbet/$linebet;						
			$results = $result->nextToken ();		
			list($wins,$symbol,$bonus)=explode(";", $results);			
			$left_lines_html = "";
			$right_lines_html = "<div class='rightline " . $this->path . "'>";
			$left_lines_html .= "<div class='leftline " . $this->path . "'>";
			$totalsLine = array ();
			$right_lines_html .= "<table class='tableline'><tr><td colspan=2 class='label'>Line</td><td class='label'>Bet</td><td class='label'>Win</td></tr>";
			$left_lines_html .= "<table class='tableline'><tr><td colspan=2 class='label'>Line</td><td class='label'>Bet</td><td class='label'>Win</td></tr>";
			settype ( $winline, "string" );
			$winning_lines = new StringTokenizer ( $wins, "|" );
			while ( $winning_lines->hasMoreTokens () ) {
				list ( $line_id, $line_win, $symbols_pos ) = explode ( ",", $winning_lines->nextToken () );
						$list = array ($line_id, $line_win, $symbols_pos );
				$totalsLine [$line_id] = implode ( ",", $list );
			
		//$winning_lines_html .= "<li>Linea $line_id " . getInDollars($line_win/100) . "<input type='radio' name='lines' class='line' line_pos='".$this->totalLines[$line_id -1]."' line='$symbols_pos'/></li>";
			}
			
			for($i = 0; $i < TOTAL_LINES5; $i ++) {
				$line_id = $i + 1;
				$html_str = "";
				if (! is_null ( $totalsLine [$line_id] )) {
					list ( $lines_id, $line_win, $symbols_pos ) = explode ( ",", $totalsLine [$line_id] );						
					$html_str .= "<tr>";
					$html_str .= "<td><input type='radio' name='lines' class='line' line_pos='" . $this->totalLines [$line_id - 1] . "' line='$symbols_pos'/></td>";
					if($line_id<=$numberlines)
					{
					$html_str .= "<td class='line_id'> $line_id </td><td>" . getInDollars ( $betData [1] / 100 ) . "</td> <td class='line_dollar'>" . getInDollars ( $line_win / 100 ) . "</td>";
					}
					else {
						$html_str .= "<td class='line_id'> $line_id </td><td>" . getInDollars ( 0 ) . "</td> <td class='line_dollar'>" . getInDollars ( $line_win / 100 ) . "</td>";
					}				
					$html_str .= "</tr>";					
				} 
				else {
					$html_str .= "<tr>";
					$html_str .= "<td><input type='radio' name='lines' class='line' line_pos='" . $this->totalLines [$line_id - 1] . "'/></td>";
					if($line_id<=$numberlines)
					{
					$html_str .= "<td class='line_id'> $line_id </td><td>" . getInDollars ( $betData [1] / 100 ) . "</td><td class='line_dollar'>" . getInDollars ( 0 ) . "</td>";
					}
					else
					{
							$html_str .= "<td class='line_id'> $line_id </td><td>" . getInDollars (0) . "</td><td class='line_dollar'>" . getInDollars ( 0 ) . "</td>";
					}
					$html_str .= "</tr>";			
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
				if (strpos ( $bonusData [0], "W" ) !== false) {
					
					$bonussymbols=split(",",$bonusData[1]);
					
					if($bonussymbols[0]%3==0)
					{
						$wild =($bonussymbols[0]+1).",".($bonussymbols[0]+2).",".$bonussymbols[0];						
					}
					if($bonussymbols[0]%3==1)
					{
						$wild=($bonussymbols[0]-1).",".($bonussymbols[0]+1).",".$bonussymbols[0];						
					}
					if($bonussymbols[0]%3==2)
					{
						$wild =($bonussymbols[0]-1).",".($bonussymbols[0]-2).",".$bonussymbols[0];						
					}	
					if (isset($bonussymbols[1])){					
						if($bonussymbols[1]%3==0)
						{
							$wild.=",".$bonussymbols[1].",".($bonussymbols[1]+2).",".($bonussymbols[1]+1);						
						}
						if($bonussymbols[1]%3==1)
						{
							$wild.=",".($bonussymbols[1]-1).",".($bonussymbols[1]+1).",".$bonussymbols[1];						
						}
						if($bonussymbols[1]%3==2)
						{
							$wild.=",".($bonussymbols[1]-2).",".($bonussymbols[1]-1).",".$bonussymbols[1];						
						}	
					}					
					$bonusHtml .= "<center><table cellpadding='4' cellspacing='0'><tr><td class='label'><b>Wild</b></td><td class='label'><b>Symbol</b></th></tr>";					
					$bonusHtml .= "<tr><td class='content'>" . $this->getBonusImage ( IMG_PATH5, "sym_10" ) . "</td><td class='content'><a href=\"javascript:showBonusSymbol('$wild');\">Show</a></td></tr>";
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
				$img = $this->getResultImage ( IMG_PATH5, $sym_id );
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
		}		
		
		
		$html .= "<div id='borut_paytable'><center><b>PAYTABLE</b></center><table cellpadding='4'>";
		$html .= "<tr><td class='label'>Symbol/<br>multiplier</td> <td class='label'> x 3 </td><td class='label'> x 4 </td><td class='label'> x 5 </td></tr>";
		$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH5, '9' ) . "</td><td>100</td><td>500</td><td>1500</td></tr>";
		$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH5, '8' ) . "</td><td>100</td><td>300</td><td>1000</td></tr>";
		$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH5, '6' ) . "</td><td>100</td><td>250</td><td>750</td></tr>";
		$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH5, '5' ) . "</td><td>50</td><td>200</td><td>500</td></tr>";
		$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH5, '4' ) . " " . $this->getResultImage ( IMG_PATH5, '3' ) . " " . $this->getResultImage ( IMG_PATH5, '2' ) . "</td><td>50</td><td>100</td><td>250</td></tr>";
		$html .= "</table>";		
		$html .= "<table cellpadding='4'><tr><td class='label'><center>Bonus <br>symbols</center></td><td class='label'> x 3</td></tr>";
		$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH5, '11' ) . "<td>Bonus</td></tr>";
		$html .= "</table></div>";
		return $html;
	}	
	function setLinesSymbolsPosition() {		
		$this->totalLines = array ("1;4;7;10;13", 
									"0;3;6;9;12",
									"2;5;8;11;14", 
									"0;4;8;10;12", 
									"2;4;6;10;14", 
									"0;3;7;11;14", 
									"2;5;7;9;12", 
									"1;3;7;9;12", 
									"1;3;7;11;13", 
									"0;4;7;10;14", 
									"2;4;7;10;12", 
									"1;3;6;10;14", 
									"1;5;8;10;12", 
									"1;4;6;10;14", 
									"1;4;8;10;12", 
									"0;3;7;11;13", 
									"2;5;7;9;13", 
									"14;11;7;3;1", 
									"1;5;7;9;12", 
									"0;3;6;10;14", 
									"2;5;8;10;12", 
									"0;4;8;11;14", 
									"2;4;6;9;12", 
									"0;4;6;10;12", 
									"2;4;8;10;14" );	
	}
}
?>