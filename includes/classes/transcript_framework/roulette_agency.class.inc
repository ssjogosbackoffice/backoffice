<?php
require_once 'Transcript.class.inc';
require_once 'transcript.inc.php';
  define("ROULETTE_PATH" , "/live/liveimage/numbers/");
  define("SINGLE_NUMBER",1000);
  define("DOUBLE_NUMBER",2000);
  define("THIRD_NUMBER",3000);
  define("FOURTH_NUMBER",4000);
  define("SIXTH_NUMBER",5000);
  define("AMBO",'spec11001');
  define("TERNO",'spec11002');
  
  define("LEFT",1);
  define("TOPLEFT",2);
  define("TOP",3);
  define("TOPRIGHT",4);
  define("RIGHT",5);
  define("BOTTOMRIGHT",6);
  define("BOTTOM",7);
  define("BOTTOMLEFT",8);
  define("CENTER",9);
  
  class roulette_agency extends Transcript
  {
  	var $GRAMBLING = array(
            array(0),
            array(1,2,3),
            array(4,5,6),
            array(7,8,9),
            array(10,11,12),
            array(13,14,15),
            array(16,17,18),
            array(19,20,21),
            array(22,23,24),
            array(25,26,27),
            array(28,29,30),
            array(31,32,33),
            array(34,35,36)
    );
    var $GROUPS = array(
          "6001" => "1st12",
          "6002" => "2nd12",
          "6003" => "3nd12",
          "7003" => "Third Column",
          "7002" => "Second Column",
          "7001" => "First Column",
          "8001" => "1to18",
          "8002" => "19to36",
          "9001" => "EVEN",
          "9002" => "ODD",
          "10001" => "BLACK",
          "10002" => "RED",
    	'3017'	 => 'Terzina01',
    	'3047'	 => 'Terzina02',
    	'3077'	 => 'Terzina03',
    	'3107'	 => 'Terzina04',
    	'3137'	 => 'Terzina05',
    	'3167'	 => 'Terzina06',
    	'3197'	 => 'Terzina07',
    	'3227'	 => 'Terzina08',
    	'3257'	 => 'Terzina09',
    	'3287'   => 'Terzina10',
    	'3317'	 =>'Terzina11',
    	'3347'	 => 'Terzina12',
    	'5016'	 => 'Sestina01',
    	'5046'	 => 'Sestina02',
    	'5076'   => 'Sestina03',
    	'5106'	 => 'Sestina04',
    	'5136'	 => 'Sestina05',
    	'5166'	 => 'Sestina06',
    	'5196'	 => 'Sestina07',
    	'5226'   => 'Sestina08',
    	'5256'	 => 'Sestina09',
    	'5286'	 => 'Sestina10',
    	'5316'	 => 'Sestina11'
    );
  	function roulette_agency(){
         
    }
  
  	function getBet() {
		//    1@p1=6002~10|6003~10|6001~10|4114~10|4234~10|4314~10|2015~1000|4014~1000@
		//    4234;0;10;500000~4114;1;10;499990~4314;2;10;499979~6001;3;10;499969~6002;4;10;499959~4014;5;1000;499949~6003;6;10;498949~2015;7;1000;498939
		//    p1=0~0|1~0|2~90|3~0|4~0|5~0|6~30|7~0
		$betresult = new StringTokenizer ( $this->punterResString, "|" );
		$resultbets = array ();
		$totalwin = "0";
        $specHtml='';

        if (strpos($this->punterResString, '#jackpot-') !== false) {
            $jackpot = explode( '#jackpot-', $this->punterResString);
            $jackpotWin = $jackpot[1] /100;
            //var_dump($jackpotWin / 100);
        }
        $jackpotTitleHtml = (isset($jackpotWin)) ? "<table><tr><td style='background-color:#4A7EB6;color:white;'>Jackpot Win</td>" : "" ;
        $jacpotTextHtml =  (isset($jackpotWin)) ? "<td  class='winner'><center>".getInDollars($jackpotWin) . "</center></td></tr></table>" : "";


        if($this->punterResString==''){
            $currentlyplaying=true;
        }
		while ( $betresult->hasMoreTokens() ) {
			$betresults = $betresult->nextToken();
            if (strpos($betresults,'spec') !== false) {
                list($specticket,$specticketwin)=explode('#',$betresults);
                 $specdetails = explode ( "~", $specticket );
                $betids=array_shift($specdetails);

                list($betwin,$winornot)=explode("~",$specticketwin);
                if(strtolower($winornot)!='won' && strtolower($winornot)!='lost'){
                    $currentlyplaying=true;
                }
                array_push ( $resultbets, array ("betid" => $betids, "betwin" => $betwin ) );
                $specHtml="<table><thead><tr><th class='label'>Chosen</th><th class='label'>Hand Id</th><th class='label'>Result</th></tr></thead>";
                 foreach ($specdetails as $k=>$v){
                  $specdetailscurrent=explode(';',$v);
                     $specHtml.="<tr><td class='centered'>".$specdetailscurrent[0]."</td><td class='centered'>".$specdetailscurrent[1]."</td><td class='centered'>".$specdetailscurrent[2]."</td></tr>";
                 }
                $specHtml.="</table>";
            }
            else{
			list ( $betids, $betwin ) = explode ( "~", $betresults );
			array_push ( $resultbets, array ("betid" => $betids, "betwin" => $betwin ) );
            }
			$totalwin = $totalwin + $betwin;
		}

		list ( $ticketname,$operation,$bet, $response ) = explode ( "@", $this->betString );
        $result = new StringTokenizer ( $response, "~" );
        if (strpos($bet,'spec') !== false) {
            $specialbet=explode('~',substr($bet,strpos($bet,'spec')));
        }
		$count = $result->countTokens ();
		$i = 0;
		$totalbets = "0";
        //* modified by Razvan//*
        $erase = "handerased";
        $poserased=strpos($this->betString,$erase);
        $reserased=strpos($this->resultString,$erase);
        if($poserased!==false || $reserased!==false ){
            $html = "<h3 style='color:red;text-align: center'>HAND ERASED</h3><br />";
        }
        //* modified by Razvan//*

        if($currentlyplaying){
            $html = "<h3 style='color:red;text-align: center'>Hand currently playing</h3><br />";
        }else{

		$html .="<div style='float:left;'>$jackpotTitleHtml $jacpotTextHtml <table  cellspacing='10' cellpadding='4' rules=rows><tr ><td style='background-color:#4A7EB6;color:white;' >Bet on</td><td style='background-color:#4A7EB6;color:white;'>&nbsp;&nbsp;Bet</td><td style='background-color:#4A7EB6;color:white;'>Available credit</td><td style='background-color:#4A7EB6;color:white;'>Win</td>";

        while ( $bets = $result->nextToken () ) {
			if ($i < ($count)) {
				list ( $betnumbers, $betid, $value, $credit ) = explode ( ";", $bets );
				$code = $betnumbers;
				$totalbets = $totalbets + $value;
				if (strpos($betnumbers,'spec') !== false) {
                    $type=$betnumbers;
                    $betid=$betnumbers;
                }
                else
                {
                $type = $code - ($code % 1000);
                }
				$numbbet = floor ( ($code % 1000) / 10 );
				$side = ($code % 1000) % 10;
				$betString = "";
				switch ($type) {
					case SINGLE_NUMBER :
						$betString = $numbbet . " Singolo";
						break;
					case DOUBLE_NUMBER :
						$betString = $this->doubleNumber ( $numbbet, $side );
						break;
					case THIRD_NUMBER :
						$betString = $this->thirdNumber ( $numbbet, $side );
						break;
					
					case FOURTH_NUMBER :
						$betString = $this->fourthNumber ( $numbbet, $side );
						break;
					
					case SIXTH_NUMBER :
						$betString = $this->sixNumber ( $numbbet, $side );
						break;
                    case AMBO:
                        $betString = "AMBO : ".$specialbet[2].",".$specialbet[3];
                        break;
                    case TERNO:
                        $betString = "TERNO : ".$specialbet[2].",".$specialbet[3].",".$specialbet[4];
                        break;
					default :
						$betString = $this->GROUPS [$code];
						break;
				}

				$html .="<tr>";
				$class = "";
			    foreach ( $resultbets as $k => $v ) {
			    	   	if ($betid == $v ["betid"]) {
						if($v ["betwin"]!= 0){
						$class =" style='background-color:green;'";
					}
				}
			}				
				$html.="<td $class>" . $betString . "</td><td $class>&nbsp;&nbsp;" . getInDollars($value / 100) . "</td><td $class><center>" . getInDollars($credit / 100) . " </center></td><td $class>";
				foreach ( $resultbets as $k => $v ) {
					if ($betid == $v ["betid"]) {
						$html .=getInDollars($v ["betwin"] / 100) . "</td></tr> ";
					}
				}
			}
		}		
		$html .= "</table>$specHtml</div>";
        }
		return $html;
  	}
    
    function doubleNumber($numbbet, $side){
    	$grbPos = -1;
        $pos = -1;
        $found = false;
        $bet = "";
        for($i = 0; $i < count($this->GRAMBLING); $i++){
            $numbersLine = $this->GRAMBLING[$i];
            for($z = 0; $z < count($numbersLine); $z++){
                if($numbbet == $numbersLine[$z]){
                      $grbPos = $i;
                      $pos = $z;
                      $found = true;
                      break;
                }
            }
            if($found){
                 break;
            }
         }
         switch($side){
            case LEFT:
                if($numbbet == 0){
                   $bet = $this->GRAMBLING[$grbPos][$pos] . "," . $this->GRAMBLING[$grbPos - 1][$pos];
                }else{
                    $bet = $this->GRAMBLING[$grbPos - 1][$pos]. "," . $this->GRAMBLING[$grbPos][$pos];
                }
            break;
            case TOP:
                $bet = $this->GRAMBLING[$grbPos][$pos] . "," . $this->GRAMBLING[$grbPos][$pos + 1];
            break;
            case BOTTOM:
            	$bet = $this->GRAMBLING[$grbPos][$pos - 1] . "," . $this->GRAMBLING[$grbPos][$pos];
            	break;
            case RIGHT:
                $bet = $this->GRAMBLING[$grbPos][$pos] . "," . $this->GRAMBLING[$grbPos + 1][$pos];
            break;
         }
         return $bet;
    }
    
    function thirdNumber($numbbet, $side){
    	$grbPos = -1;
        $pos = -1;
        $found = false;
        $bet = "";
        for($i = 0; $i < count($this->GRAMBLING); $i++){
            $numbersLine = $this->GRAMBLING[$i];
            for($z = 0; $z < count($numbersLine); $z++){
                if($numbbet == $numbersLine[$z]){
                      $grbPos = $i;
                      $pos = $z;
                      $found = true;
                      break;
                }
            }
            if($found){
                 break;
            }
         }
         switch($side){
            case TOPLEFT:
                $bet = "0," . $this->GRAMBLING[$grbPos][$pos] . "," . $this->GRAMBLING[$grbPos][$pos + 1];
                break;
            case BOTTOMLEFT:
                	$bet = "0," . $this->GRAMBLING[$grbPos][$pos-1] . "," . $this->GRAMBLING[$grbPos][$pos];
                	break;
            case BOTTOM:
                $bet = $this->GRAMBLING[$grbPos][$pos] . "," . $this->GRAMBLING[$grbPos][$pos + 1] . "," . $this->GRAMBLING[$grbPos][$pos + 2];
                break;
         }
         return $bet;
    }
    
    function fourthNumber($numbbet, $side){
    	$grbPos = -1;
        $pos = -1;
        $found = false;
        $bet = "";
        for($i = 0; $i < count($this->GRAMBLING); $i++){
            $numbersLine = $this->GRAMBLING[$i];
            for($z = 0; $z < count($numbersLine); $z++){
                if($numbbet == $numbersLine[$z]){
                      $grbPos = $i;
                      $pos = $z;
                      $found = true;
                      break;
                }
            }
            if($found){
                 break;
            }
         }
         switch($side){
            case BOTTOMLEFT:
               $bet = "0," . $this->GRAMBLING[$grbPos][$pos] . "," . $this->GRAMBLING[$grbPos][$pos + 1] . "," . $this->GRAMBLING[$grbPos][$pos + 2];
               break;
            case TOPRIGHT:
               $bet = $this->GRAMBLING[$grbPos][$pos] . "," . $this->GRAMBLING[$grbPos][$pos + 1] . "," . $this->GRAMBLING[$grbPos + 1][$pos] . "," . $this->GRAMBLING[$grbPos + 1][$pos + 1];
               break;
         }
         return $bet;
    }
    
    function sixNumber($numbbet, $side){
    	$grbPos = -1;
        $pos = -1;
        $found = false;
        $bet = "";
        for($i = 0; $i < count($this->GRAMBLING); $i++){
            $numbersLine = $this->GRAMBLING[$i];
            for($z = 0; $z < count($numbersLine); $z++){
                if($numbbet == $numbersLine[$z]){
                      $grbPos = $i;
                      $pos = $z;
                      $found = true;
                      break;
                }
            }
            if($found){
                 break;
            }
         }
         $bet = $this->GRAMBLING[$grbPos][$pos] . "," . $this->GRAMBLING[$grbPos][$pos + 1] . "," . $this->GRAMBLING[$grbPos][$pos + 2] . "," . $this->GRAMBLING[$grbPos + 1][$pos] . "," . $this->GRAMBLING[$grbPos + 1][$pos + 1] . "," . $this->GRAMBLING[$grbPos + 1][$pos + 2];
         return $bet;
    }
    
   
    function getReadbleTranscript()
    {
    	settype($this->resString, "string");    	
    	list($number, $color) = split("\\|", $this->resString);
    	return "<div align='center' style='font-weight:bold; font-size:large;' >Winning number:<br/><img src='".ROULETTE_PATH."".trim($number).".jpg' /></div>";
    }
  }  
?>