<?php
//@@ Marian
require_once 'Transcript.class.inc';
require_once 'transcript.inc.php';
require_once 'general_functions.inc';
class dograces extends Transcript {
	public function getNames() {
		return "dograces";
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
//	 254.5
	#6_4_2_1_3_5
	#2_3-6~4
	#false-0.0~false-0.0
	#9.5,3.02,3.68,4.94,10.2,4.33,19.75,2.65,3.29,4.46,19.33,4.3,8.86,9.64,2.81,2.89,5.36,7.43,25.18,31.26,42.37,183.64,40.85,59.65,9.94,13.47,58.38,12.99,72.68,9.75,16.41,71.13,15.82,97.57,13.09,16.25,95.49,21.24,201.45,27.03,33.56,45.49,43.86,85.52,11.47,14.25,19.31,83.7
	#1235,2011-12-16 08:43:05,3.0;1_4;3.68;0-4.0;2_3;10.2;1-1.0; 6~4:3.02;1


	function getReadbleTranscript() {
		$betData = explode ( ";", $this->betString );
		$html = '';
		$this->path = "dog_races";
    	settype ( $this->betString, "string" );
		$thisresult = explode("@", $this->betString);
		$request=explode ("!",$thisresult['1']  );
		$requestquotes=$request['1'];
		$requests=new StringTokenizer($requestquotes,"-");
		$totalbets=0;
		while($requests->hasMoreTokens()){
			list($quotabet,$rest)=explode(";",$requests->nextToken());
			$totalbets=$totalbets+$quotabet;
		}
//		200;1?1_1-200;2?1~2-200;2?3~4-200;1?5_2-200;2?6~2-200;2?5~6-900;1?2_2-900;1?2_3-900;2?2~6-900;1?1_2-900;1?1_3"
		$result = new StringTokenizer ( $thisresult['2'], "#" );
		if ($result->countTokens () >= 1) {
		$totalwin= $result->nextToken ();
		$winning_video= $result->nextToken ();
		$first=$winning_video['0'];
		$second=$winning_video['1'];
		$third=$winning_video['2'];
		$fourth=$winning_video['3'];
		$fifth=$winning_video['4'];
		$sixth=$winning_video['5'];
		$order=$first.",".$second.",".$third.",".$fourth.",".$fifth.",".$sixth;
		$winning_games = $result->nextToken();
		$jackpot = $result->nextToken ();
		$quote_list = $result->nextToken ();
         $ticket=  $result->nextToken ();
           list($ticketid,$time,$thisbet)=explode(',',$ticket);
		settype ( $symbol, "string" );
		$quotes = new StringTokenizer ( $quote_list, "," );
		$html .= "<center><div class='container " . $this->path . "'>";
            //* modified by Razvan//*
            $erase = "handerased";
            $poserased=strpos($this->betString,$erase);
            $reserased=strpos($this->resultString,$erase);
            if($poserased!==false || $reserased!==false ){
                $html .= "<h3 style='color:red;text-align: center'>HAND ERASED</h3><br />";
            }
            //* modified by Razvan//*
		$html .="<div><table  style='text-align:right; font-size:14px;' cellspacing='1' cellpadding='3'>
				<tr><td class='label' style='font-size:16px;'>Ticket id</td><td style='text-align:left;font-size:16px;'><b>$ticketid</b></td></tr>
				<tr><td class='label' style='font-size:16px;'>Time</td><td style='text-align:left;font-size:16px;'><b>$time</b></td></tr>
				<tr><td class='label' style='font-size:16px;'>Result</td><td style='text-align:left;font-size:16px;'><b>$order</b></td></tr>
				<tr><td class='label' style='font-size:16px;'>Total bet</td><td style='text-align:left;font-size:16px;'><b>".getInDollars($totalbets/100)."</b></td></tr>
				<tr><td class='label' style='font-size:16px;'>Total win</td><td style='text-align:left;font-size:16px;'><b>".getInDollars($totalwin/100)."</b></td></tr>";
		$bonusHtml='';
		list($normaljackpot,$randomjackpot)=explode("~", $jackpot);
			if($normaljackpot['0']!='f'){
				list($normaljackpotyes,$normaljackpotwin)=explode("-",$normaljackpot);
					$bonusHtml .= "<tr><td class='label' style='font-size:16px;'>Jackpot</td><td style='text-align:left;font-size:16px;'><b>".getInDollars($normaljackpotwin/100)."</b></td></tr>";
			}
				if($randomjackpot['0']!='f'){
				list($randomjackpotyes,$randomjackpotwin)=explode("-",$randomjackpot);
				$bonusHtml .= "<tr><td class='label' style='font-size:16px;'>Random Jackpot</td><td style='text-align:left;font-size:16px;'><b>".getInDollars($randomjackpotwin/100)."</b></td></tr>";
			}
			$html .= $bonusHtml;
			$html .="</table></div><br><br>";
			$html .= "<center><div class='box " . $this->path . "'>";
            $combinedquotes="<br><table rules=rows  style='text-align:center'   cellpadding='3' ><tr class='formheading'><td class='label' colspan='6'>Quotes for combined bets</td></tr><tr class='formheading'><td class='label'>1</td><td class='label'>2</td><td class='label'>3</td><td class='label'>4</td><td class='label'>5</td><td class='label'>6</td>";
            $singlequotes = "<table  style='text-align:center'  rules=rows cellpadding='5' ><tr class='formheading'><td class='label' colspan='6'>Quotes for single bet </td></tr><tr class='formheading'><td class='label'>1</td><td class='label'>2</td><td class='label'>3</td><td class='label'>4</td><td class='label'>5</td><td class='label'>6</td>";
			$pos = 0;

            while ( $quotes->hasMoreTokens () ) {
				$quote_id = $quotes->nextToken ();
				if ($pos % 6 == 0 && ($pos<=18 || $pos>=48)) {
                    $singlequotes .= "<tr>";
				}
				if ($pos==18 || $pos==24||$pos==30 || $pos==36 ||$pos==42 || $pos==48 ) {
					$combinedquotes .= "<td></td>";
				}
				if($pos>=18 && $pos < 48){
                $combinedquotes .= "<td class='quote'>" . $quote_id. "</td>";
                }
                else{
                    $singlequotes .="<td class='quote'>" . $quote_id. "</td>";
                }

				if ($pos==17 || $pos==22 || $pos==27 ||$pos==32 || $pos==37 ||$pos==42) {
					$combinedquotes .= "</tr><tr>";
				}
				$pos ++;
				if ($pos % 6 == 0 && ($pos<=18 || $pos>=48)) {
					$singlequotes .= "</tr>";
				}

			}
            $singlequotes .="</table>";
            $combinedquotes .="</table>";
			$html .= $singlequotes.$combinedquotes."</div>";
			$html .="<div style='float:left;'><table style='text-align:center'   rules=rows cellspacing='2' cellpadding='4' ><tr class='formheading'><td class='label'colspan='5' >Detailed ticket</td></tr><tr class='formheading'><td class='label'>Bet</td><td  class='label'>Bet type</td><td  class='label'>Bet on</td><td  class='label'>Quota</td><td  class='label'>Win</td></tr>";

            $bets= new StringTokenizer ( $thisbet, "-" );
			while($bets->hasMoreTokens()){
				list($betamount,$beton,$quota,$win)=explode(";",  $bets->nextToken());
				if($win!='0'){
					$win=$betamount*$quota;
				}
				$html .="<tr";
				if($win!='0'){
					$html .=" style='background-color:green;'";
				}
				$html .="><td>".getInDollars($betamount/100)."</td><td>";
				if(strpbrk($beton, '_') != FALSE){
					list($dognr,$dogpos)=explode("_",$beton);
					$html .="
					Dog - Position </td><td>
					Dog number: $dognr <br>Postion: $dogpos </td><td>$quota</td><td>".getInDollars($win/100)."</td></tr>";
				}
				elseif(strpbrk($beton, '~') != FALSE){

					$dognr=split("~",$beton);
                    if($dognr[0]==$dognr[1]){
                        if(count($dognr)==2){
                            $html.="1 or 2nd </td><td>";
                        }
                        else{
                            $html.="1,2nd or 3rd</td><td>";
                        }
                        $html .="Dog number: $dognr[0]<br>";
                    }
                    else{
                        if(count($dognr)==2){
                            $html.="First<br /> Second </td><td>";
                        }
                        elseif(count($dognr)==3){
                            $html.="Tris</td><td>";
                        }
                        elseif(count($dognr)==6){
                            $html.="SuperSix</td><td>";
                        }
                        for($i=0;$i<count($dognr);$i++){
                            $html .="Dog number: $dognr[$i]<br>";
                        }
                    }

					$html .="  </td><td>$quota</td><td>".getInDollars($win/100)."</td></tr>";
				}
			}
			$html .="</table></div>";
			$html .= "<br style='clear:both;'/>";
			$html .= "</div></center>";
		}
		return $html;
	}
}
?>