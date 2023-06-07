<?php  
  define("IMG_PATH" , "/games/%s/%s/admin_tokens/");
  define("PAYOUT_PATH" , "/games/%s/%s/");  
require_once 'Transcript.class.inc';
require_once 'transcript.inc.php';
require_once 'general_functions.inc';
  
  class blackjacklive extends Transcript
  {
      function blackjacklive(){
  }
      
      public function getNames(){
          return "Black Jack";
      }
      
      function getBet()
      {
         $betData = split(";" , $this->betString);

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
      /*
       * 
       * p0 1:0:4|D~0;12|C~0;12|S~0;25:0:dealer
		 p1 1:0:0:0:0:DP_paradiso79:4960
		 p2 1:100:11|D~0;6|D~0;17:0:200:DP_107972:4958
		 p3 0:0:0:0:0:null:0
       * 
       */
      function getReadbleTranscript()
      {

      	$pos = strpos($this->betString, 'handerase');
      	if($pos===false){
      	$html='';
      	}
      	else {
      		$html ="<h1  style='color:red'>HAND ERASED!!</h1>";
      	}
      	settype($this->punterResString, "string");
        list($playerid,$playercards)=explode("=", $this->punterResString);
        settype($this->resString, "string");
        //echo $this->punterResString . "<br/>";
        $tables = split(",",$this->resString);
        $count = count($tables);        
        $html .= "<div id='cards' style='width:600px;margin:auto;'>";
	        $html .= "<table>";	        
	        for($i = 0; $i < $count; $i++){
	        	$table = $tables[$i];
	        	$tableInfo = split("=" , $table);
	        	//echo $tableInfo[0] . " ";
	        	//echo $tableInfo[1] . "<br/>";
	        	$player = split(":", $tableInfo[1]);
	        	if($tableInfo[0] == "p0"){
	        		$html .= "<tr>";
		        		$html .= "<table style='width:500px;' rules=rows cellpadding='5' >";
				        	$html .= "<tr>";
				        		$html .= "<td class='label' width='350px' colspan='2'>".strtoupper($player[4])."</td>";
				        		$html .= "<td class='label'>POINTS</td>";
				        	$html .= "</tr>";
				        	if($player[2] != "0"){
				        		$listCard = split(";", $player[2]);
				        		$html .= "<tr>";
				        			$html .= "<td>Cards</td>";
				        			$html .= "<td><div style='float:left;'>".$this->getCards($player[2])."</div></td>";
				        			$html .= "<td>".$listCard[count($listCard) -1]."</td>";
				        		$html .= "</tr>";
				        		$html .= "<tr><td></td></tr>";
				        	}
			        	//$html .= "</table>";
			        $html .= "</tr>";
	        	}else{
		        	if($player[5] != "null"){
		        		$html .= "<tr>";
			        		$html .= "<table style='width:500px;' rules=rows cellpadding='5' >";
				        		$html .= "<tr>";				        		
					        		$html .= "<td class='label' style='width:65px;'>Position: </td><td style='width:30px;'>".$tableInfo[0]."</td>";
					        		$html .= "<td class='label' style='width:60px;' ><span style='font-weight: bold;'>Player:</td><td> " .strtoupper($player[5]) .  " </span></td>"; 
		        	        		$html .="<td class='label'>OPTIONS</td>";
					        		$html .= "<td class='label'>POINTS</td>";					        		
				        			$html .= "</tr>";
		        				
		        		if($tableInfo[0]==$playerid)
					        		{					        			
					        			$infos=split(":",$playercards);					        		
					        		}
					        		
			        		if($player[2] != "0"){
			        			$listCard = split(";", $player[2]);
			        			$html .= "<tr>";
				        			$html .= "<td colspan='2'>Cards</td>";
				        			$html .= "<td colspan='2'><div style='float:left;'>".$this->getCards($player[2])."</div></td>";				        			
			        				$html .="<td>";	
			        				if($infos[7]=='BJ0') 
					        				{
					        					$html .="BLACKJACK!!<br>";
					        				}
			        				if(($infos[7]=='D0') ||($infos[8]=='D0') || ($infos[9]=='D0'))
					        				{
					        					$html .="DOUBLE <br>";
					        				}
					        		if($infos[7]=='I0' || $infos[8]=='I0' || $infos[9]=='I0')
					        		{
					        			$html .="INSURANCE<br>";
					        		}
					        				$html .="</td>";			        			
				        			$html .= "<td>".$listCard[count($listCard) -1]."</td>";
			        			$html .= "</tr>";
			        			
			        		}else{
			        			$html .= "<tr>";
			        				$html .= "<td  colspan='3'><h2>SAT BUT DIDN'T PLAY</h2></td>";
			        			$html .= "</tr>";
			        		}
		        			if($player[3] != "0"){
		        				$listCardSplit = split(";", $player[3]);
			        			$html .= "<tr>";
				        			$html .= "<td colspan='2'>Split</td>";
				        			$html .= "<td colspan='2'><div style='float:left;'>".$this->getCards($player[3])."</div></td>";
				        			$html .="<td>";	
				        			if(($infos[7]=='D1') ||($infos[8]=='D1') || ($infos[9]=='D1'))
					        				{
					        					$html .="DOUBLE <br>";
					        				}
		        			if(($infos[7]=='D2') ||($infos[8]=='D2') || ($infos[9]=='D2'))
					        				{
					        					$html .="DOUBLE <br>";
					        				}
					        		if($infos[7]=='I1' || $infos[8]=='I1' || $infos[9]=='I1')
					        		{
					        			$html .="INSURANCE<br>";
					        		}
					        				$html .="</td>";	
				        			
				        			$html .= "<td>".$listCardSplit[count($listCardSplit) -1]."</td>";
			        			$html .= "</tr>";
			        			
			        		}else{
			        			$html .= "<tr>";
			        				$html .= "<td colspan='3'><h2>NO SPLIT</h2></td>";
			        			$html .= "</tr>";
			        		}
			        		//$html .= "</table>";
		        		$html .= "</tr>";
		        	}else{
		        		$html .= "<tr>";
			        		$html .= "<table style='width:500px;' rules=rows cellpadding='5' >";
				        		$html .= "<tr>";
					        		$html .= "<td class='label' style='width:65px;'>Position:</td><td style='width:30px;'> ".$tableInfo[0]."</td>";
					        		$html .= "<td class='label' style='width:60px;'><span style='font-weight: bold;'>Player:</span></td>"; 
					        		$html .= "<td colspan='3'>NO SEAT</td>";
				        		$html .= "</tr>";
				        		if($tableInfo[0]!='p5'){
				        		$html .= "<tr><td></td></tr>";}
			        		//$html .= "</table>";
		        		$html .= "</tr>";
		        	}
	        	}
	        }
        
        	$html .= "</table>";
        $html .= "</div>"; 
//        $dealerCards = split(";", $dealerCards[0]);
//        $count = count($dealerCards);
//        $html = "<div align='center' class='container " . $this->path . "'>";
//        $html .= "<div class='box " . $this->path . "'>";
//        $html .= "<table><tr><td colspan='".($count-1)."'>Dealer Card</td></tr><tr>";
//        for($i = 0; $i < ($count -1); $i++){
//        	list($cardType, $cardValue) = split("\|", $dealerCards[$i]);
//        	$html .= "<td align='center'>". getCard(strtolower(trim($cardValue)."".trim($cardType))) . "</td>";
//        }
//        $html .= "</tr>";
//        $playerCards = split("~", $this->resultString);
//        $playerCards = split(";", $playerCards[0]);
//        $plCount = count($playerCards);
//        $html .= "<tr><td colspan='".($plCount-1)."'>Player Card</td></tr>";
//        for($i = 0; $i < ($plCount -1); $i++){
//        	list($cardType, $cardValue) = split("\|", $playerCards[$i]);
//        	$html .= "<td align='center'>". getCard(strtolower(trim($cardValue)."".trim($cardType))) . "</td>";
//        }
//        $html .= "</tr>";
//      
        	return $html;
      }
      
      function getCards($cards){
      	$cardHtml = "";
      	$listCard = split(";", $cards);
      	for($i = 0; $i < count($listCard) - 1; $i++){
      		$card = split("~", $listCard[$i]);
      		list($cardValue,$cardType) = split("\|", $card[0]);
      		$cardHtml .= getCard(strtolower(trim($cardValue)."".trim($cardType)));
      	}
      	return $cardHtml; 
      }
  }

?>