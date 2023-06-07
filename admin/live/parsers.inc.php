<?php
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
  
	$GRAMBLING = array(
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
    $GROUPS = array(
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
    function getBet($betString)
    {
    	global $GRAMBLING, $GROUPS;
    	$code = $betString;
    	$type = $code - ($code % 1000);
        $numbbet = floor(($code % 1000) / 10);
        $side = ($code % 1000) % 10;
        $betString = "";
        switch($type){
        	case SINGLE_NUMBER:
        		$betString =  $numbbet;
        		break;
        	case DOUBLE_NUMBER:
        		$betString = doubleNumber($numbbet, $side);
        		break;
        	case THIRD_NUMBER:
        		$betString = thirdNumber($numbbet, $side);
        		break;
        	case FOURTH_NUMBER:
        		$betString = fourthNumber($numbbet, $side);
        		break;
        	case SIXTH_NUMBER:
        	    $betString = sixNumber($numbbet, $side);
        		break;
        	default:
                $betString = $GROUPS[$code];
                break;
        }
        
        return $betString;
    }
    
    function doubleNumber($numbbet, $side){
    	global $GRAMBLING;
    	$grbPos = -1;
        $pos = -1;
        $found = false;
        $bet = "";
        for($i = 0; i < count($GRAMBLING); $i++){
            $numbersLine = $GRAMBLING[$i];
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
                   $bet = $GRAMBLING[$grbPos][$pos] . "," . $GRAMBLING[$grbPos - 1][$pos];
                }else{
                   $bet = "0," . $GRAMBLING[$grbPos][$pos];
                }
                break;
            case TOP:
                $bet = $GRAMBLING[$grbPos][$pos] . "," . $GRAMBLING[$grbPos][$pos + 1];
                break;
            case RIGHT:
                $bet = $GRAMBLING[$grbPos][$pos] . "," . $GRAMBLING[$grbPos + 1][$pos];
                break;
         }
         return $bet;
    }
    
    function thirdNumber($numbbet, $side){
    	global $GRAMBLING;
    	$grbPos = -1;
        $pos = -1;
        $found = false;
        $bet = "";
        for($i = 0; i < count($GRAMBLING); $i++){
            $numbersLine = $GRAMBLING[$i];
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
                $bet = "0," . $GRAMBLING[$grbPos][$pos] . "," . $GRAMBLING[$grbPos][$pos + 1];
                break;
            case BOTTOM:
                $bet = $GRAMBLING[$grbPos][$pos] . "," . $GRAMBLING[$grbPos][$pos + 1] . "," . $GRAMBLING[$grbPos][$pos + 2];
                break;
         }
         return $bet;
    }
    
    function fourthNumber($numbbet, $side){
    	global $GRAMBLING;
    	$grbPos = -1;
        $pos = -1;
        $found = false;
        $bet = "";
        for($i = 0; i < count($GRAMBLING); $i++){
            $numbersLine = $GRAMBLING[$i];
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
               $bet = "0," . $GRAMBLING[$grbPos][$pos] . "," . $GRAMBLING[$grbPos][$pos + 1] . "," . $GRAMBLING[$grbPos][$pos + 2];
               break;
            case TOPRIGHT:
               $bet = $GRAMBLING[$grbPos][$pos] . "," . $GRAMBLING[$grbPos][$pos + 1] . "," . $GRAMBLING[$grbPos + 1][$pos] . "," . $GRAMBLING[$grbPos + 1][$pos + 1];
               break;
         }
         return $bet;
    }
    
    function sixNumber($numbbet, $side){
    	global $GRAMBLING;
    	$grbPos = -1;
        $pos = -1;
        $found = false;
        $bet = "";
        for($i = 0; i < count($GRAMBLING); $i++){
            $numbersLine = $GRAMBLING[$i];
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
         $bet = $GRAMBLING[$grbPos][$pos] . "," . $GRAMBLING[$grbPos][$pos + 1] . "," . $GRAMBLING[$grbPos][$pos + 2] . "," . $GRAMBLING[$grbPos + 1][$pos] . "," . $GRAMBLING[$grbPos + 1][$pos + 1] . "," . $GRAMBLING[$grbPos + 1][$pos + 2];
         return $bet;
    }
?>