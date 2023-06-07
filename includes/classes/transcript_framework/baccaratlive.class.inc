<?php 
//@Marian
require_once 'Transcript.class.inc';
require_once 'transcript.inc.php';
require_once 'general_functions.inc';
  
  class baccaratlive extends Transcript
  {
      function baccaratlive(){
      }
      
      public function getNames(){
          return "baccarat_live";
      }
      
      function getBet()
      {
       $betData = split(";" , $this->betString);
       return "";
      }

      function getReadbleTranscript()
      {        
        settype($this->punterResString, "string");
        settype($this->resString, "string");
//        p0=1:0:2|D~0;2|C~0;6:0:dealer,p1=1:1200:2|C~0;2|D~0;6:0:0:PUNTO:0

        $tables = split(",",$this->resString);
        $count = count($tables);  
      $pos = strpos($this->betString, 'handerase');
            
        $html ="<div><center>";
        
        	if($pos===false){
        		$html .="   <table style='text-align:center;' rules=rows cellpadding='5' ><tr><td class='label'>Bet on</td><td class='label'>Bet amount</td ><td class='label'>Win</td></tr>";
        	}
        	else {
        		$html .="<h1  style='color:red'>HAND ERASED!!</h1><table style='text-align:center;' rules=rows cellpadding='5' ><tr><td class='label'>Bet on</td><td class='label'>Bet amount</td ><td class='label'>Win</td></tr>";
        	}  
        
     
        	$result=split("@",$this->betString);        	
        	
        	$prebet=$result['1'];
        	$prewin=$result['2'];
       	  	list($id,$allbets)=explode("=",$prebet);        	
        	$bets= new StringTokenizer ( $allbets, "|" );				
			while($bets->hasMoreTokens()){
				list($beton,$betamount)=explode("~",  $bets->nextToken());
				if($beton=='P'){
					$beton="PUNTO";					
				}
				elseif($beton=='T')
				{
					$beton='TIE';					
				}
				else{
					$beton='BANKER';
				}
				$html .="<tr><td>$beton</td><td>".getInDollars($betamount/100)."</td>";
				$wins =new StringTokenizer($prewin,"|");
				while($wins->hasMoreTokens()){
					list($winon,$winamount)=explode("~",$wins->nextToken());
					if($winon=='P'){
						$winon='PUNTO';
					}
					elseif ($winon=='T')
					{
						$winon='TIE';
					}
					else{
						$winon='BANKER';
					}
					if($winon==$beton){
						$html .="<td>".getInDollars($winamount/100)."</td>";						
					}
				}
				$html .="</tr>";
			}	
        	
        $html .= "</table></center></div><br>";
        $html .= "<div id='cards' style='width:320px;margin:auto;'><center>";	              
	        for($i = 0; $i < $count; $i++){
	        	$table = $tables[$i];
	        	$tableInfo = split("=" , $table);	        	
	        	$player = split(":", $tableInfo[1]);
	        	if($tableInfo[0] == "p0"){	        		
		        		$html .= "<table style='width:156px; text-align:center;float:left;'>";
				        	$html .= "<tr>";
				        		$html .= "<td width='150px' style='font-size:16px;' class='label'>".strtoupper('BANKER')."</td></tr>";			        	
				        	if($player[2] != "0"){
				        		$listCard = split(";", $player[2]);
				        		$html .= "<tr>";
				        			$html .= "";
				        			$html .= "<td style='font-size:16px;'><center><div style='height:150px;'><br>".$this->getCards($player[2])."</div></center><br> POINTS: ".$listCard[count($listCard) -1]."</td>";
				        		$html .= "</tr>";
				        	}
			        	//$html .= "</table>";
			        $html .= "</tr>";
	        	}else{
		        	if($player[5] != "null"){
		        		$html .= "<tr>";
			        		$html .= "<table style='width:156px;text-align:center'>";
				        		$html .= "<tr><td width='150px' class='label'><span style='font-weight:bold; font-size:16px;'>" .strtoupper($player[5]) ." </span></td>"; 
				        		$html .= "</tr>";
			        		if($player[2] != "0"){
			        			$listCard = split(";", $player[2]);
			        			$html .= "<tr>";
				        			$html .= "<td style='font-size:16px;'><center><div style='height:150px;'><br>".$this->getCards($player[2])."</div></center><br>POINTS: ".$listCard[count($listCard) -1]."</td>";
			        			$html .= "</tr>";
			        		}else{
			        			$html .= "<tr>";
			        				$html .= "<td colspan='3'><h2>SAT, BUT DON'T PLAY</h2></td>";
			        			$html .= "</tr>";
			        		}		        			
		        		$html .= "</tr>";
		        	}else{
		        		$html .= "<tr>";
			        		$html .= "<table style='width:156px;";
				        		$html .= "<tr>";
					        		$html .= "<td class='label'>Position: ".$tableInfo[0]."</td>";
					        		$html .= "<td class='label'><span style='font-weight: bold;'>Player:</span></td>"; 
					        		$html .= "<td class='label'>NO SIT</td>";
				        		$html .= "</tr>";			        		
		        		$html .= "</tr>";
		        	}
	        	}
	        }
//        6@p1=P~200|T~200@P~0|T~1600
        	$html .= "</table></center></div>";
        	return $html;
      }      
      function getCards($cards){
      	$cardHtml = "";
      	$listCard = split(";", $cards);
      	for($i = 0; $i < count($listCard) - 1; $i++){
      		$card = split("~", $listCard[$i]);
      		list($cardValue,$cardType) = split("\\|", $card[0]);
      		if($i=='2'){
      			$cardHtml.="<br><div style='float:left; width:150px;'>".getCard(strtolower(trim($cardValue)."".trim($cardType)))." </div> ";
      		}
      		else {      		
      		$cardHtml .= "<div style='float:left; width:75px;'>".getCard(strtolower(trim($cardValue)."".trim($cardType)))." </div> ";
      		}      		
      	}
      	return $cardHtml; 
      }
  }
?>