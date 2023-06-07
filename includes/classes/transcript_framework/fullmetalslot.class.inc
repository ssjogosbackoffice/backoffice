<?php
define ( "TOTAL_LINES", 25 );
define ( "IMG_PATH_FULLMETAL", "../media/images/handimages/fullmetal" );
define ( "PAYOUT_PATH_FULLMETAL", "../media/images/handimages/fullmetal" );
require_once 'Transcript.class.inc';
require_once 'transcript.inc.php';
require_once 'general_functions.inc';
class fullmetalslot extends Transcript 
{	
	public function getNames() 
	{
		return "fullmetalslot";
	}	
		
	function getBet() 
	{		
           $betData = split ( "@", $this->betString );  
           $bets=split(";",$betData[1]);
           $html .= "PAY: <b>" . getInDollars ( $bets [1] / 100 ) . "</b> FOR LINE <br>";
           $html .= "TOTAL LINES: <b>" . number_format ( $bets [0] / $bets[1], 0 ) . "</b>";
  
           return $html;
	}
	 
	function getReadbleTranscript()
	 {	 	
	 	$html ='';
//	 	$betData = split ( ";", $this->betString );
//		$html = '';
//		$numberlines=$betData[0]/$betData[1];
		$this->setLinesSymbolsPosition ();
		$this->path="fullmetal";
		settype ( $this->betString, "string" );		
		$result = new StringTokenizer ( $this->betString, "@" );
		if ($result->countTokens () >= 2) {		
			$subcallid=$result->nextToken();
			$linebets= $result->nextToken ();
				list($totalbets,$betsperline)=explode(";", $linebets);
				$numberlines=$totalbets/$betsperline;		
			$winninglines=$result->nextToken();			
				list($totalwin,$winlines)=explode("&",$winninglines);						
			$bonus = $result->nextToken ();
			$jackpot= $result->nextToken();
			$lvlxp=$result->nextToken();
			list($lvl,$xp)=explode("~", $lvlxp);	
			$userinfo="<div>";
			$userinfo .= "<center><table cellpadding='2' cellspacing='0' border='1'><tr><td class='label'><b>User level</b></td><td class='label'><b>Xp</b></th></tr>";
			$userinfo .= "<tr><td class='content'><center>" . $lvl . "</center></td><td class='content'>" . $xp. "</td></tr>";
			$userinfo .= "</table></center></div>";							
			$userinfo .="</div>";
			$html .= $userinfo;	
			if($bonus=='true')
			{
				$subcallbonus=$result->nextToken();
				$bonustypes=$result->nextToken();
					list($bonustype,$ambient)=explode("~", $bonustypes);
					if($bonustype=='1')
					{
						$bonustype= 'Wargames';
					}
					elseif ($bonustype=='2')
					{
						$bonustype=='Match';
					}
					else
					{
						$bonustype='Conquer';	
					}
				$bonuswin=$result->nextToken();
					list($winafterbonus,$winbonus)=explode("~", $bonuswin);
					$bonusHtml = "<div id='bonuscontainer'>";
					$bonusHtml .= "<center><table cellpadding='2' cellspacing='0'><tr><td class='label'><b>Bonus Type</b></td><td class='label'><b>Win</b></th></tr>";
					$bonusHtml .= "<tr><td class='content'>" . $bonustype . "</td><td class='content'>" . $winbonus. "</td></tr>";
					$bonusHtml .= "</table></center></div>";							
					$bonusHtml .="</div>";
					$html .=$bonusHtml;				
			}
			if($jackpot=='true')
			{
			$jackpothtml="<br><div>";
			$jackpothtml .= "<center><table cellpadding='2' cellspacing='0' border='1'><tr><td class='label'><b>Jackpot</b></td></tr>";
			$jackpothtml .= "<tr><td class='content'><center>" .$totalwin. "</center></td></tr>";
			$jackpothtml .= "</table></center></div>";							
			$jackpothtml .="</div><br>";
			$html .= $jackpothtml;	
			}
				
			if($totalwin>0)
			{
				settype ( $winlines, "string" );	
				$results=new StringTokenizer($winlines, "-");
				while($results->hasMoreTokens()) {
				$winline=$results->nextToken();					
					$left_lines_html = "";			
				$right_lines_html = "<div class='rightline " . $this->path . "'>";
				$left_lines_html .= "<div class='leftline " . $this->path . "'>";
				$totalsLine = array ();
				$right_lines_html .= "<table class='tableline'><tr><td colspan=2 class='label'>Line</td><td class='label'>Bet</td><td class='label'>Win</td></tr>";
				$left_lines_html .= "<table class='tableline'><tr><td colspan=2 class='label'>Line</td><td class='label'>Bet</td><td class='label'>Win</td></tr>";									
				settype ( $winline, "string" );
				$winning_lines = new StringTokenizer ( $winline, ";" );
				$numberwinlines=$winning_lines->countTokens();
				while ( $winning_lines->hasMoreTokens () &&$winning_lines->pos<$numberwinlines-1 ) 
				{
					list ( $line_id, $symbols_pos,$line_win ) = explode ( "~", $winning_lines->nextToken () );				
					$list = array ($line_id,  $symbols_pos, $line_win, );
					$totalsLine [$line_id] = implode ( ",", $list );

				}
				$symbols=$winning_lines->tokens[$numberwinlines-1];			
				for($i = 0; $i < TOTAL_LINES; $i ++) 
				{
				$line_id = $i + 1;
				$html_str = "";
				if (isset ( $totalsLine [$line_id] )) 
				{
					list ( $lines_id, $symbols_pos,$line_win ) = explode ( ",", $totalsLine [$line_id] );
					if($line_id<=$numberlines)
					{
						$html_str .= "<tr>";
						$html_str .= "<td><input type='radio' name='lines' class='metalline' part='$results->pos' line_pos='" . $this->totalLines [$line_id - 1] . "' line='$symbols_pos'/></td>";
						$html_str .= "<td class='line_id'> $line_id </td><td>" . getInDollars ( $betsperline / 100 ) . "</td> <td class='line_dollar'>" . getInDollars ( $line_win / 100 ) . "</td>";					
						$html_str .= "</tr>";
					}
				}
				else 
				{
					if($line_id<=$numberlines)
					{
						$html_str .= "<tr>";
						$html_str .= "<td><input type='radio' name='lines' class='metalline' part='$results->pos' line_pos='" . $this->totalLines [$line_id - 1] . "'/></td>";
						$html_str .= "<td class='line_id'> $line_id </td><td>" . getInDollars ( $betsperline / 100 ) . "</td><td class='line_dollar'>" . getInDollars ( 0 ) . "</td>";
						$html_str .= "</tr>";
					}				
				}
				
				$lineCheck = floor ( TOTAL_LINES / 2 );
				if ($i <= $lineCheck) 
				{
					$left_lines_html .= $html_str;
				} 
				else 
				{
					$right_lines_html .= $html_str;
				}			
			}
			
			$right_lines_html .= "</table>";
			$left_lines_html .= "</table>";			
			$left_lines_html .= "</div>";
			$right_lines_html .= "</div>";
			$html .= "<div class='container " . $this->path . "'  style='clear:both;'/>";	
			
			$symbolList = new StringTokenizer ( $symbols, "|" );
			$html .=$left_lines_html;
			$html .= "<div class='box " . $this->path . "'>";
			$html .= "<ul id='columnList'>";			
			$pos = 0;
			while ( $symbolList->hasMoreTokens () ) {
				$sym_id = $symbolList->nextToken ();
				$img = $this->getResultImage ( IMG_PATH_FULLMETAL, $sym_id );
				if ($pos % 3 == 0) {
					$html .= "<li id='column_" . ($pos / 3) . "'><ul>";
				}
				$html .= "<li class='symbol' id='sym_".$pos."_".$results->pos."'>" . $img . "</li>";
				$pos ++;
				if ($pos % 3 == 0) {
					$html .= "</ul></li>";
				}
			}		
			$html .= "</ul>";
			$html .= "<br style='clear:both;'/>";
			$html .= "</div>";
			$html .=$right_lines_html;
			$html .= "</div>";
			}
				
			}	
			else 
			{
				$symbol1=$winlines;
						
			$html .= "<div class='container " . $this->path . "'>";		
			$symbolList = new StringTokenizer ( $symbol1, "|" );			
			$html .= "<div style='margin-left:243px;'><div class='box " . $this->path . "'>";
			$html .= "<ul id='columnList'>";			
			$pos = 0;
			while ( $symbolList->hasMoreTokens () ) {
				$sym_id = $symbolList->nextToken ();
				$img = $this->getResultImage ( IMG_PATH_FULLMETAL, $sym_id );
				if ($pos % 3 == 0) {
					$html .= "<li id='column_" . ($pos / 3) . "'><ul>";
				}
				$html .= "<li>" . $img . "</li>";
				$pos ++;
				if ($pos % 3 == 0) {
					$html .= "</ul></li>";
				}
			}		
			$html .= "</ul>";			
			$html .= "</div></div>";
			$html .= "<br style='clear:both;'/>";						
			}
			$html .= "<div id='fullmetal_paytable'><table cellpadding='4'>";
			$html .= "<tr><td class='label'>Symbol/<br>occurence</td> <td class='label'> 3 </td><td class='label'> 4 </td><td class='label'> 5 </td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_FULLMETAL, '0' ) . "</td><td>x 2 </td><td>x 5 </td><td>x 8</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_FULLMETAL, '1' ) . "</td><td>x 4 </td><td>x 7 </td><td>x 10</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_FULLMETAL, '2' ) . "</td><td>x 7 </td><td>x 10</td><td>x 14</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_FULLMETAL, '3' ) . "</td><td>x 8 </td><td>x 12</td><td>x 18</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_FULLMETAL, '4' ) . "</td><td>x 11</td><td>x 15</td><td>x 20</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_FULLMETAL, '5' ) . "</td><td>x 15</td><td>x 20</td><td>x 25</td></tr>";
			$html .= "</table>";
			$html .= "<table cellpadding='4'><tr><td class='label'>Symbol/<br>occurrence</td> <td class='label'> x 2 </td><td class='label'> x 3 </td><td class='label'> x 4 </td><td class='label'> x 5 </td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_FULLMETAL, '6' ) . "</td><td> 0 </td><td>x 21</td><td>x 27</td><td>x 30</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_FULLMETAL, '7' ) . "</td><td> 0 </td><td>x 23</td><td>x 28</td><td>x 32</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_FULLMETAL, '8' ) . "</td><td>x 5</td><td>x 30</td><td>x 40</td><td>x 45</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_FULLMETAL, '9' ) . "</td><td>x 7</td><td>x 40</td><td>x 60</td><td>x 70</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_FULLMETAL, '10' ) . "</td><td>x 9</td><td>x 70</td><td>x 90</td><td>x 95</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_FULLMETAL, '11' ) . "</td><td>x 11</td><td>x 80</td><td>x 100</td><td>x 150</td></tr>";
			$html .= "</table>";
			$html .= "<table cellpadding='4'><tr><td class='label'><center>Bonus <br>symbols</center></td><td class='label'>Description</td></tr>";
			$html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_FULLMETAL, '12' ) . " </td><td>Bonus symbol</td></tr>";
			$html .= " <tr><td class='label'>" . "<img src='".IMG_PATH_FULLMETAL."/sym_14.jpg' /> </td><td>Choose a battlefield <br> and kill your enemies</td></tr>";
			$html .= " <tr><td class='label'>" . "<img src='".IMG_PATH_FULLMETAL."/sym_15.jpg' /> </td><td>Start to conquer the world <br> and increase your win</td></tr>";
			$html .= " <tr><td class='label'>" . "<img src='".IMG_PATH_FULLMETAL."/sym_16.jpg' /> </td><td>Choose a card that matches <br> that one shown on table</td></tr>";
			$html .= "</table></div>";
		}		
		return $html;	
	}
	
	function setLinesSymbolsPosition() {		
		$this->totalLines = array ("1;4;7;10;13", "0;3;6;9;12", "2;5;8;11;14", "0;4;8;10;12", "2;4;6;10;14", "0;3;7;11;14", "2;5;7;9;12", "1;5;7;9;12", "1;3;7;11;14", "0;4;7;10;14", "2;4;7;10;12", "0;3;6;10;14", "2;5;8;10;12", "1;4;8;10;12", "1;4;6;10;14", "1;5;8;10;12", "1;3;6;10;14", "2;5;7;9;13", "0;3;7;11;13", "2;4;6;9;12", "0;4;8;11;14", "1;3;7;11;13", "1;5;7;9;13", "2;4;6;10;12", "0;4;8;10;14" );	
	}
}
?>
