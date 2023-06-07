<?php
// @@ Marian
// 2013-01-11
require_once 'Transcript.class.inc';
require_once 'transcript.inc.php';
require_once 'general_functions.inc';
class dograces_cd extends Transcript {
	public function getNames() {
		return "dograces_cd";
	}
	function getBet() {
	}
	function getReadbleTranscript() {

		$html = '';


		$this->path = "dograces_cd";
		settype ( $this->betString, "string" );
		$thisresult = split ( "@", $this->betString );
		$request = split ( "!", $thisresult ['1'] );
		$order = $this->resString;
		$requestquotes = $request ['1'];
		$requests = new StringTokenizer ( $requestquotes, "-" );
		$totalbets = 0;
		while ( $requests->hasMoreTokens () ) {
			list ( $quotabet, $rest ) = explode ( ";", $requests->nextToken () );
			$totalbets = $totalbets + $quotabet;
		}
		if (isset ( $request ['2'] )) {
			$totalbets = $totalbets + 100;
		}
		$result = new StringTokenizer ( $thisresult ['2'], "#" );
		
		if ($result->countTokens () >= 1) {
			$totalwin = $result->nextToken ();
			$ticket = $result->nextToken ();
			list ( $ticketid, $time, $thisbet, $jackpot ) = explode ( ",", $ticket );
			$quote_list = $result->nextToken ();
			settype ( $symbol, "string" );
			$quotes = new StringTokenizer ( $quote_list, "," );
            //* modified by Razvan//*
            $erase = "handerased";
            $poserased=strpos($this->betString,$erase);
            $reserased=strpos($this->resultString,$erase);
            if($poserased!==false || $reserased!==false ){
                $html .= "<h3 style='color:red;text-align: center'>HAND ERASED</h3><br />";
            }
            //* modified by Razvan//*

			$html .= "<center><div class='container " . $this->path . "'>";


			$html .= "<div><table  style='text-align:right; font-size:14px;' cellspacing='1' cellpadding='3'>
								<tr><td class='label' style='font-size:16px;'>Ticket id</td>
									<td style='text-align:left;font-size:16px;'><b>$ticketid</b></td></tr>
								<tr><td class='label' style='font-size:16px;'>Time</td>
									<td style='text-align:left;font-size:16px;'><b>$time</b></td></tr>
								<tr><td class='label' style='font-size:16px;'>Result</td>
									<td style='text-align:left;font-size:16px;'><b>$order</b></td></tr>
								<tr><td class='label' style='font-size:16px;'>Total bet</td>
									<td style='text-align:left;font-size:16px;'><b>" . getInDollars ( $totalbets / 100 ) . "</b></td></tr>
								<tr><td class='label' style='font-size:16px;'>Total win</td>
									<td style='text-align:left;font-size:16px;'><b>" . getInDollars ( $totalwin / 100 ) . "</b></td></tr>";
			$bonusHtml = '';

			list ( $normaljackpot, $randomjackpot ) = explode ( "~", $jackpot );
			if ($normaljackpot ['0'] != 'f') {
				list ( $normaljackpotyes, $normaljackpotwin ) = explode ( "-", $normaljackpot );
				$bonusHtml .= "<tr><td class='label' style='font-size:16px;'>Jackpot</td>
									<td style='text-align:left;font-size:16px;'><b>" . getInDollars ( $normaljackpotwin / 100 ) . "</b></td></tr>";
			}
			if ($randomjackpot ['0'] != 'f') {
				list ( $randomjackpotyes, $randomjackpotwin ) = explode ( "-", $randomjackpot );
				$bonusHtml .= "<tr><td class='label' style='font-size:16px;'>Random Jackpot</td>
								   <td style='text-align:left;font-size:16px;'><b>" . getInDollars ( $randomjackpotwin / 100 ) . "</b></td></tr>";
			}
			$html .= $bonusHtml;
			$html .= "</table></div><br><br>";
			$html .= "<center><div class='box " . $this->path . "'>";
			$pos = 0;
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
			$html .= "<div style='float:left;'>
							<table style='text-align:center' rules=rows cellspacing='2' cellpadding='4' >
									<tr class='formheading'><td class='label'colspan='5'>Detailed ticket</td></tr>
									<tr class='formheading'><td class='label'>Bet</td>
									             <td class='label'>Bet type</td>
									            <td class='label'>Bet on</td>
															<td class='label'>Quota</td><td class='label'>Win</td></tr>";
			$bets = new StringTokenizer ( $thisbet, "-" );
			while ( $bets->hasMoreTokens () ) {
				list ( $betamount, $beton, $quota, $win ) = explode ( ";", $bets->nextToken () );
				if ($beton == '0~0~0~0~0~0') {
					break;
				}
				if ($win != '0') {
					$win = $betamount * $quota;
				}
				$html .= "<tr";
				if ($win != '0') {
					$html .= " style='background-color:green;'";
				}
				$html .= "><td>" . getInDollars ( $betamount / 100 ) . "</td><td>";
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
			$html .= "</table></div>";
			$html .= "<br style='clear:both;'/>";
			$html .= "</div></center>";
		}
		return $html;
	}
}
?>