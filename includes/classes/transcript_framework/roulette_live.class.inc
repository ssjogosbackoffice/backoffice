<?php
require_once 'Transcript.class.inc';
  require_once 'transcript.inc.php';
  define("ROULETTE_PATH" , "/live/liveimage/numbers/");
  define("SINGLE_NUMBER",1000);
  define("DOUBLE_NUMBER",2000);
  define("THIRD_NUMBER",3000);
  define("FOURTH_NUMBER",4000);
  define("SIXTH_NUMBER",5000);
  
  define("LEFT",1);
  define("TOPLEFT",2);
  define("TOP",3);
  define("TOPRIGHT",4);
  define("RIGHT",5);
  define("BOTTOMRIGHT",6);
  define("BOTTOM",7);
  define("BOTTOMLEFT",8);
  define("CENTER",9);
  
  class roulette_live extends Transcript
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
          "10002" => "RED" 
    );
  	function roulette_live(){
         
    }
  
  	function getBet() {
		//    1@p1=6002~10|6003~10|6001~10|4114~10|4234~10|4314~10|2015~1000|4014~1000@
		//    4234;0;10;500000~4114;1;10;499990~4314;2;10;499979~6001;3;10;499969~6002;4;10;499959~4014;5;1000;499949~6003;6;10;498949~2015;7;1000;498939
		//    p1=0~0|1~0|2~90|3~0|4~0|5~0|6~30|7~0

/*        $this->betString="1@p1=1039~100|1159~100|1219~100|1009~100|1269~100|1299~100|1329~100|1079~100|1199~100|1259~100|1289~100|2023~100|4282~400|2151~200|2211~200|2351~200|2071~200|2221~200|3022~400

@3022;0;400;10000~2071;1;200;9600~1159;2;100;9400~1219;3;100;9300~1039;4;100;9200~1299;5;100;9100~1329;6;100;9000~1009;7;100;8900~1199;8;100;8800~1259;9;100;8700~
2351;10;200;8600~2151;11;200;8400~4282;12;400;8200~1269;13;100;7800~1289;14;100;7700~2221;15;200;7600~2023;16;100;7400~2211;17;200;7300~1079;18;100;7100";
        $this->punterResString=" p1=0~0|1~0|2~3600|3~0|4~0|5~0|6~0|7~0|8~0|9~0|10~0|11~3600|12~0|13~0|14~0|15~0|16~0|17~0|18~0";*/

        $this->betString = substr ( $this->betString, 5 );
        $this->punterResString = substr ( $this->punterResString, 3 );
		$pos = strpos($this->betString, 'handerase');
		if($pos===false){
			$html="";
		}
		else {
			$html="<h1  style='color:red'>HAND ERASED!!</h1>";
		}
		
		$betresult = new StringTokenizer ( $this->punterResString, "|" );
		$resultbets = array ();
		$totalwin = "0";
		while ( $betresults = $betresult->nextToken () ) {
			
			list ( $betids, $betwin ) = explode ( "~", $betresults );
			array_push ( $resultbets, array ("betid" => $betids, "betwin" => $betwin ) );
			$totalwin = $totalwin + $betwin;
		}

		list ( $bet, $response ) = explode ( "@", $this->betString );
		$result = new StringTokenizer ( $response, "~" );
		
		$count = $result->countTokens ();
		$i = 0;
		$totalbets = "0";

		$html.="<div style='float:left;'><table  cellspacing='10' cellpadding='4' rules=rows><tr ><td style='background-color:#4A7EB6;color:white;' >Bet on</td><td style='background-color:#4A7EB6;color:white;'>&nbsp;&nbsp;Bet</td><td style='background-color:#4A7EB6;color:white;'>Available credit</td><td style='background-color:#4A7EB6;color:white;'>Win</td>";
		while ( $bets = $result->nextToken () ) {
			if ($i < ($count)) {
				list ( $betnumbers, $betid, $value, $credit ) = explode ( ";", $bets );
				$code = $betnumbers;
				$totalbets = $totalbets + $value;
				$type = $code - ($code % 1000);
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
		$html .= "</table></div>";
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
                }elseif($numbbet<4){
                   $bet = "0," . $this->GRAMBLING[$grbPos][$pos];
                }
                else{
                    $bet=$this->GRAMBLING[$grbPos - 1][$pos].",".$this->GRAMBLING[$grbPos][$pos];
                }
            break;
            case TOP:
                $bet = $this->GRAMBLING[$grbPos][$pos] . "," . $this->GRAMBLING[$grbPos][$pos + 1];
            break;
             case BOTTOM:
                 $bet = $this->GRAMBLING[$grbPos][$pos] . "," . $this->GRAMBLING[$grbPos][$pos - 1];
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
             case TOPLEFT:
                 $bet = $this->GRAMBLING[$grbPos-1][$pos] . "," . $this->GRAMBLING[$grbPos-1][$pos+1] . "," . $this->GRAMBLING[$grbPos][$pos] . "," . $this->GRAMBLING[$grbPos][$pos+1];
                 break;
             case BOTTOMRIGHT:
                 $bet =$this->GRAMBLING[$grbPos][$pos - 1] . "," . $this->GRAMBLING[$grbPos][$pos] . "," . $this->GRAMBLING[$grbPos + 1 ][$pos -1 ] . "," . $this->GRAMBLING[$grbPos+1][$pos ] ;
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
    	return "<div align='center' class='large' >Winning number:<br/><img src='".ROULETTE_PATH."".trim($number).".jpg' /></div>";
    }
  }  
?>