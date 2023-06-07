<?php
 define("CROUPIER",0);
 define("SUPERVISOR", 1);
 define("ADMINISTRATOR",2);
 
 define("ORPHELINS",0);
 define("VOISINS", 1);
 define("TIER",2);
 
 $ORPHELINS = array(1,6,9,14,17,20,31,34);
 $VOISINS = array(0,2,3,4,7,12,15,18,19,21,22,25,26,28,29,32,35);
 $TIER = array(5,8,10,11,13,16,23,24,27,30,33,36);
  
 function checkTypeCroupier($crptype){
 	if($crptype == SUPERVISOR){
 		return "Supervisor";
 	}else if($crptype == CROUPIER){
 		return "Croupier";
 	}else if($crptype == ADMINISTRATOR){
		return "Administrator";
	}
 }
 function getCountColor($numbers,$color){
 	$count = 0;
 	for($i =0 ; $i < count($numbers); $i++){
 		if($numbers[$i]["color"] == $color){
 			$count++;
 		}
 	}
 	return $count;
 }
 function getCountInSector($numbers,$type){
 	$count = 0;
 	for($i =0 ; $i < count($numbers); $i++){
 		if($numbers[$i]["sector"] == $type){
 			$count++;
 		}
 	}
 	return $count;
 }
 function getCountNumber($numbers,$number){
 	$count = 0;
 	for($i =0 ; $i < count($numbers); $i++){
 		if($numbers[$i]["number"] == $number){
 			$count++;
 		}
 	}
 	return $count;
 }
 
 
 function getSector($number){
 	global $ORPHELINS,$VOISINS,$TIER;
 	settype($number, 'integer');
 	for($i =0 ; $i < count($ORPHELINS); $i++){
 		settype($ORPHELINS[$i],'integer');
 		if($ORPHELINS[$i] == $number){
 			return ORPHELINS;
 		}
 	}
 	for($i =0 ; $i < count($VOISINS); $i++){
 		settype($VOISINS[$i],'integer');
 		if($VOISINS[$i] == $number){
 			return VOISINS;
 		}
 	}
 	for($i =0 ; $i < count($TIER); $i++){
 		settype($TIER[$i],'integer');
 		if($TIER[$i] == $number){
 			return TIER;
 		}
 	}

 }
 
 function printSector($extract){
 	global $ORPHELINS,$VOISINS,$TIER;
 	list($number, $color) = split("\|",$extract);
 	settype($number, 'integer');
 	for($i =0 ; $i < count($ORPHELINS); $i++){
 		settype($ORPHELINS[$i],'integer');
 		if($ORPHELINS[$i] == $number){
 			return "ORPHELINS";
 		}
 	}
 	for($i =0 ; $i < count($VOISINS); $i++){
 		settype($VOISINS[$i],'integer');
 		if($VOISINS[$i] == $number){
 			return "VOISINS";
 		}
 	}
 	for($i =0 ; $i < count($TIER); $i++){
 		settype($TIER[$i],'integer');
 		if($TIER[$i] == $number){
 			return "TIER";
 		}
 	}
 }
 function printExtractNumber($extract){
 	list($number, $color) = split("\|",$extract);
 	$hex = "";
 	settype($color, 'integer');
 	switch($color){
 		case 0:
 		 	$hex = "#3CAB1A";
 		 break;
 		case 1:
 			$hex = "#000000";
 		 break;
 		case 2:
 			$hex = "#FF0000";
 		 break;
 	}
 	return '<div style="color:'.$hex.';">'.$number.'</div>';
 }
?>
