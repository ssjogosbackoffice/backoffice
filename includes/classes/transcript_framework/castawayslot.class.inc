<script type="text/javascript">
   function showHold(symbol){
	    $(".symbol").removeClass("blink_line");
	    $(".symbol").removeClass("blink");
	    blinking_pos     = symbol.split(";");
	    for(var i = 0 ; i < blinking_pos.length ; i++){
	      $("#sym_" + blinking_pos[i]).removeClass("blink_line");
	      $("#sym_" + blinking_pos[i]).addClass("hold").fadeOut().fadeIn();
	    }
	  }
</script>
<?php
define ( "TOTAL_LINES3", 25 );
define ( "IMG_PATH3", "../media/images/handimages/castaway_slot" );
define ( "PAYOUT_PATH3", "../media/images/handimages/castaway_slot" );
require_once 'Transcript.class.inc';
require_once 'transcript.inc.php';
require_once 'general_functions.inc';


class castawayslot extends Transcript {
	public function getNames() {
		return "castawayslot";
	}//   125;5
	
	function getBet() {
		$betData = split ( ";", $this->betString );
		$html = "<center><table>";
		if ($betData [1] == 0) {
			$html .= "<tr><td><b>PLAY IN FREESPIN</b></td></tr>";
		} else {
			$html .= "<tr><td class='label'>PAY: </td><td><b>" . getInDollars ( $betData [1] / 100 ) . "</b> FOR LINE </td>";
			$html .= "<tr><td class='label'>TOTAL LINE:</td><td> <b>" . number_format ( $betData [0] / $betData [1], 0 ) . "</b></td></tr>";
		}
		$html .= "</table></center>";
		return $html;
	}
	// 6@2500~1-2-3-4-5-6-7-8-9-10-11-12-13-14-15-16-17-18-19-20-21-22-23-24-25~0;0;0;0;0;0;0;0;0;0;0;0;0;0;0@0@null@true-7@null@null@11|6|11|9|0|10|1|2|1|0|3|2|10|7|10-0;0;0;0;0;0;0;0;0;0;0;0;0;0;0#
	// 1@2500~1-2-3-4-5-6-7-8-9-10-11-12-13-14-15-16-17-18-19-20-21-22-23-24-25~0;1;0;1;0;0;0;1;0;0;1;0;0;1;0@0@null@true-6@false@false@2|6|4|9|7|0|11|2|9|10|3|0|3|7|10-0;1;0;1;0;0;0;1;0;0;1;0;0;1;0	
	function getReadbleTranscript() {
		$html = '<center><table cellpadding="10" rules=rows>';
		$this->path = "castawayslot";
		$position = 0;
		$results = new StringTokenizer ( $this->betString, "#" );
		while ( $results->hasMoreTokens () ) {
			$this->setLinesSymbolsPosition ();
			$result = new StringTokenizer ( $results->nextToken (), "@" );
			if ($result->countTokens () >= 2) {
				$position = $position + 1;
				$html .= '<tr><td valign="center">';
				if ($position == '1') {
					$html .= "<center><b>FIRST HAND</b></center>";
				}
				if ($position == '2') {
					$html .= "<center><b>SECOND HAND</b></center>";
				}
				$type = $result->nextToken ();
				$bet = $result->nextToken ();
				list ( $betamount, $numberlines, $hold ) = explode ( "~", $bet );
				$linesbet = split ( "-", $numberlines );
				$linebet = $betamount / (count ( $linesbet ));				
				$win = $result->nextToken ();
				$winline = $result->nextToken ();
				$freespins = $result->nextToken ();
				$bonus = $result->nextToken ();
				$jackpot = $result->nextToken ();
				$symbols = $result->nextToken ();
				list ( $symbol, $symbolshold ) = explode ( "-", $symbols );
				$holdsimbols = explode ( ";", $symbolshold );
				$left_lines_html = "";
				$holded=array();				
				foreach ($holdsimbols as $key=>$value)
				{
					if($value=='1'){
						array_push($holded, $key);
					}
				}
				$holded=implode(";", $holded);						
				if ($position == '1') {
				} else {
						$html .= "<center><table cellpadding='4' cellspacing='0' border='1'><tr><td class='label' align='center'><b>Holded</b></td></tr>";
						$html .= "<tr><td class='content'><a href=\"javascript:showHold('$holded');\">Show holded</a></td></tr>";
						$html .= "</table></center>";
				}
				$right_lines_html = "<div class='rightline " . $this->path . "'>";
				$left_lines_html .= "<div class='leftline " . $this->path . "'>";
				$totalsLine = array ();
				$right_lines_html .= "<table class='tableline'><tr><td colspan=2 class='label'>Line</td><td class='label'>Bet</td><td class='label'>Win</td></tr>";
				$left_lines_html .= "<table class='tableline'><tr><td colspan=2 class='label'>Line</td><td class='label'>Bet</td><td class='label'>Win</td></tr>";
				settype ( $winline, "string" );
				$winning_lines = new StringTokenizer ( $winline, "~" );
				while ( $winning_lines->hasMoreTokens () ) {
					list ( $line_id, $symbols_pos, $winningsymbol, $symbolsocc, $line_win ) = explode ( "-", $winning_lines->nextToken () );
					$list = array ($line_id, $symbols_pos, $winningsymbol, $symbolsocc, $line_win );
					$totalsLine [$line_id] = implode ( ",", $list );				
			//$winning_lines_html .= "<li>Linea $line_id " . getInDollars($line_win/100) . "<input type='radio' name='lines' class='line' line_pos='".$this->totalLines[$line_id -1]."' line='$symbols_pos'/></li>";
			//6@625~1-2-3-4-5-6-7-8-9-10-11-12-13-14-15-16-17-18-19-20-21-22-23-24-25~0;0;0;0;0;0;0;0;0;0;0;0;0;0;0@0@null@false-0@null@null@10|0|7|7|11|9|2|10|3|2|5|10|9|11|4-0;0;0;0;0;0;0;0;0;0;0;0;0;0;0#
			//1@625~1-2-3-4-5-6-7-8-9-10-11-12-13-14-15-16-17-18-19-20-21-22-23-24-25~0;0;0;0;0;0;0;0;0;0;0;0;0;1;1@375@9-2_4_8_12_15-10-2-375@false-0@false@false@7|10|11|10|6|5|7|1|11|10|6|11|1|11|4-0;0;0;0;0;0;0;0;0;0;0;0;0;1;1			
				}			
				for($i = 0; $i < TOTAL_LINES3; $i ++) {
					$line_id = $i + 1;
					$html_str = "";
					if (! is_null ( $totalsLine [$line_id] )) {
						list ( $line_id, $symbols_pos, $winningsymbol, $symbolsocc, $line_win ) = explode ( ",", $totalsLine [$line_id] );
						$html_str .= "<tr>";
						$winning_symbols = explode("_", $symbols_pos);
						if($line_id<=13){
									$winning_symbols=array_slice($winning_symbols,0,$symbolsocc);
						}
						else 
						{
							$winning_symbols=array_slice($winning_symbols,5-$symbolsocc);								
						}	

					foreach ($winning_symbols as &$value) {
    						$value = $value - 1;
						}
						
						$symbols_winning = implode ( "~", $winning_symbols );										
						if($line_id<=count ( $linesbet ))
						{
						$html_str .= "<td><input type='radio' name='lines' class='line' line_pos='" . $this->totalLines [$line_id - 1] . "' line='" . $symbols_winning . "'/></td>";
						$html_str .= "<td class='line_id'> $line_id </td><td>" . getInDollars ( $linebet / 100 ) . "</td> <td class='line_dollar'>" . getInDollars ( $line_win / 100 ) . "</td>";
						$html_str .= "</tr>";
						}
					} else {
						if($line_id<=count ( $linesbet ))
						{
						$html_str .= "<tr>";
						$html_str .= "<td><input type='radio' name='lines' class='line' line_pos='" . $this->totalLines [$line_id - 1] . "'/></td>";
						$html_str .= "<td class='line_id'> $line_id </td><td>" . getInDollars ( $linebet / 100 ) . "</td><td class='line_dollar'>" . getInDollars ( 0 ) . "</td>";
						$html_str .= "</tr>";
						}
					}
					$lineCheck = floor ( TOTAL_LINES3 / 2 );
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
				$bonusHtml = "<center><table><tr>";
				if ($freespins) {
					$bonusHtml .= "<td><div id='bonuscontainer'>";
					$bonusData = split ( "-", $freespins );
					if (strpos ( $bonusData [0], "t" ) !== false) {
						$bonusHtml .= "<center><table cellpadding='4' cellspacing='0'><tr><td class='label'><b>FreeSpins</b></td></tr>";
						$bonusHtml .= "<tr><td class='content'>" . $bonusData [1] . "</td></tr>";
						$bonusHtml .= "</table></div></center></td>";
					}
				}				
				if ($jackpot) {
					$bonusHtml .= "<td><div id='bonuscontainer'>";
					$bonusData = split ( "-", $jackpot );
					if (strpos ( $bonusData [0], "t" ) !== false) {
						$bonusHtml .= "<center><table cellpadding='4' cellspacing='0'><tr><td class='label'><b>Jackpot</b></td></tr>";
						$bonusHtml .= "<tr><td class='content'>" . getInDollars ( $bonusData [1] ) . "</td></tr>";
						$bonusHtml .= "</table></div></center></td>";
					}
				}
				if ($bonus) {
					$bonusHtml .= "<td><div id='bonuscontainer'>";
					$bonusData = split ( "~", $bonus );
					if (strpos ( $bonusData [0], "t" ) !== false) {
						$bonuswin = split ( "-", $bonusData [0] );
						$bonusHtml .= "<center><table cellpadding='4' cellspacing='0'><tr><td class='label'><b>Bonus</b></td><td class='label'><b>Win</b></td><td class='label'><b>Symbol</b></th></tr>";
						$bonusHtml .= "<tr><td class='content'><img src='" . IMG_PATH3 . "/" . $bonusData [2] . ".png' width='80' height='80' /></td><td class='content'>" . getInDollars ( $bonuswin [1] / 100 ) . "</td><td class='content'><a href=\"javascript:showBonusSymbol('$bonusData[1]');\">Show</a></td></tr>";
						$bonusHtml .= "</table></center></div></td>";					
					}
				}
									
					
				
				
				$bonusHtml .= "</tr></table></center>";
				$html .= $bonusHtml;
				if ($position == '1') {
				} else {
					$html .= $left_lines_html;
				}
				if ($position == '1') {
					$html .= "<div style='margin-left:240px'><center>";
				}
				$html .= "<div class='box " . $this->path . "'>";
				$html .= "<ul id='columnList'>";				
				$pos = 0;
				while ( $symbolList->hasMoreTokens () ) {
					$sym_id = $symbolList->nextToken ();
					$img = $this->getResultImage ( IMG_PATH3, $sym_id + 1 );
					if ($pos % 3 == 0) {						
						$html .= "<li id='column_" . ($pos / 3) . "'><ul>";
					}
					if ($position == '1') {
						$html .= "<li>" . $img . "</li>";
					} else {						
						
						$html .= "<li class='symbol' id='sym_$pos'>" . $img . "</li>";
					}
					$pos ++;
					if ($pos % 3 == 0) {
						$html .= "</ul></li>";
					}
				}				
				$html .= "</ul>";
				$html .= "<br style='clear:both;'/>";
				$html .= "</div>";
				if ($position == '1') {
					$html .= "</div>";
				} else {
					$html .= $right_lines_html;
				}
				$html .= "</td></tr>";
			}
		}
		$html .= "</table></center>";
		$html .= "<div id='castawayslot_paytable'><center><b>PAYTABLE</b></center><table cellpadding='4'>";
		$html .= "<tr><td class='label'>Symbol/<br>multiplier</td> <td class='label'> x 3 </td><td class='label'> x 4 </td><td class='label'> x 5 </td></tr>";
		$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH3, '1' ) . "</td><td>2</td><td>5</td><td>8</td></tr>";
		$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH3, '2' ) . "</td><td>4</td><td>7</td><td>10</td></tr>";
		$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH3, '3' ) . "</td><td>7</td><td>10</td><td>14</td></tr>";
		$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH3, '4' ) . "</td><td>8</td><td>12</td><td>14</td></tr>";
		$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH3, '5' ) . "</td><td>11</td><td>15</td><td>20</td></tr>";
		$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH3, '6' ) . "</td><td>15</td><td>20</td><td>25</td></tr>";
		$html .= "</table>";
		$html .= "<table cellpadding='4'><tr><td class='label'>Symbol/<br>multiplier</td> <td class='label'> x 2 </td><td class='label'> x 3 </td><td class='label'> x 4 </td><td class='label'> x 5 </td></tr>";
		$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH3, '7' ) . "</td><td>0</td><td>21</td><td>27</td><td>30</td></tr>";
		$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH3, '8' ) . "</td><td>5</td><td>23</td><td>28</td><td>32</td></tr>";
		$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH3, '9' ) . "</td><td>10</td><td>30</td><td>40</td><td>45</td></tr>";
		$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH3, '10' ) . "</td><td>15</td><td>40</td><td>60</td><td>70</td></tr>";
		$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH3, '11' ) . "</td><td>15</td><td>70</td><td>90</td><td>95</td></tr>";
		$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH3, '12' ) . "</td><td>0</td><td>80</td><td>100</td><td>150</td></tr>";
		$html .= "</table>";
		$html .= "<table cellpadding='4'><tr><td class='label'><center>Bonus <br>symbols</center></td><td class='label'>Description</td></tr>";
		$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH3, '8' ) . " " . $this->getResultImage ( IMG_PATH3, '13' ) . " " . $this->getResultImage ( IMG_PATH3, '9' ) . "</td><td>Cast Race</td></tr>";
		$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH3, '12' ) . " " . $this->getResultImage ( IMG_PATH3, '13' ) . "</td><td>Treasure Hunt</td></tr>";
		$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH3, '3' ) . " " . " " . $this->getResultImage ( IMG_PATH3, '13' ) . " " . $this->getResultImage ( IMG_PATH3, '3' ) . "</td><td>Message in <br>a bottle</td></tr>";
		$html .= "</table></div>";
		return $html;
	}
	
	function setLinesSymbolsPosition() {		
		$this->totalLines = array ("1;4;7;10;13", "0;3;6;9;12", "2;5;8;11;14", "0;4;8;10;12", "2;4;6;10;14", "0;3;7;11;14", "2;5;7;9;12", "1;5;7;9;12", "1;3;7;11;14", "0;4;7;10;14", "2;4;7;10;12", "0;3;6;10;14", "2;5;8;10;12", "1;4;8;10;12", "1;4;6;10;14", "1;5;8;10;12", "1;3;6;10;14", "2;5;7;9;13", "0;3;7;11;13", "2;4;6;9;12", "0;4;8;11;14", "1;3;7;11;13", "1;5;7;9;13", "2;4;6;10;12", "0;4;8;10;14" );	
	}
}
?>