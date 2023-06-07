<?php
/**
 * @author Quirino Massaro
 * @version 1.0
 *
 * Contiene una serie di metodi che: effettuano la valutazione di una mano (hand) di Poker composta da
 * 2,5,6 o 7 carte; restituiscono il punto vincente e le carte che lo compongono a partire da una mano.
 * Ogni oggetto GameResult contiene un oggetto di questa classe per effettuare le valutazioni delle mani
 * di Poker.
 *
 * Very fast hand evaluator
 * @Author Indiana, http://pokerai.org/pf3
 * @Copyright 2007
 */
class TexasHoldEmPokerUtility {

	// Array statici utilizzati dai metodi della classe per effettuare le valutazioni
	
	private static $div4 = array(0,0,0,0, 1,1,1,1, 2,2,2,2, 3,3,3,3, 4,4,4,4, 5,5,5,5, 6,6,6,6, 7,7,7,7, 8,8,8,8, 9,9,9,9, 10,10,10,10, 11,11,11,11, 12,12,12,12, 13,13,13,13);
	
	private static $mod4 = array(0,1,2,3, 0,1,2,3, 0,1,2,3, 0,1,2,3, 0,1,2,3, 0,1,2,3, 0,1,2,3, 0,1,2,3, 0,1,2,3, 0,1,2,3, 0,1,2,3, 0,1,2,3, 0,1,2,3, 0,1,2,3);
		
	private static $mult0 = array();
	private static $mult1 = array();
	private static $mult2 = array();
	private static $mult3 = array();
	private static $mult4 = array();
	private static $mult5 = array();
	
	// static final $mul1 = 1, mul2 = mul1 * 15, mul3 = mul2 * 15, mul4 = mul3 * 15, mul5 = mul4 * 15;
	private static $mul = array(0, 1, 15, 225, 3375, 50625);
	
	// Gli array vengono popolati con dei valori interi che rimarrano costanti
	private static function init() {
		for ($z = 0; $z<14; $z++) {
			self::$mult0[$z]=self::$mul[0] * $z;
        }
		for ($z = 0; $z<14; $z++)
        {
			self::$mult1[$z]=self::$mul[1] * $z;
        }
		for ($z = 0; $z<14; $z++)
        {
			self::$mult2[$z]=self::$mul[2] * $z;
        }
		for ($z = 0; $z<14; $z++){
			self::$mult3[$z]=self::$mul[3] * $z;
        }
		for ($z = 0; $z<14; $z++){
			self::$mult4[$z]=self::$mul[4] * $z;
        }
		for ($z = 0; $z<14; $z++){
			self::$mult5[$z]=self::$mul[5] * $z;
        }

	}
	
	
	// Proprietà
	
	private $z0;
	private $z1;
	private $z2;
	private $z3;
	private $z4;
	
	// Variabile temporanea che memorizza l'id del punto migliore temporaneo
	private $tempBestHandId;
	
	// Variabile che memorizza l'id del punto migliore (memorizzata durante una valutazione)
	private $bestHandId;
	
	// Array che memorizza gli id delle 5 carte che compongono il punto migliore (memorizzato durante una valutazione)
	private $bestHandCardsArray=array();
	
	
	/**
	 * Costruttore di classe
	 * 
	 */
	public function TexasHoldEmPokerUtility() {
		$this->tempBestHandId = -1;
		$this->bestHandId = -1;
		$this->bestHandCardsArray = null;
		self::init();
	}	
	
	/**
	 * Restituisce l'id del punto migliore calcolato dai metodi di valutazione invocati in precedenza 
	 * 
	 * @return id del punto migliore
	 */
	public function getBestHandId() {
		return $this->bestHandId;
	}
	
	/**
	 * Restituisce l'array delle carte che formano il punto migliore calcolato dai metodi di valutazione 
	 * invocati in precedenza 
	 * 
	 * @return array delle carte che formano il punto migliore
	 */
	public function getBestHandCardsArray() {
		return $this->bestHandCardsArray;
	}
	
	/**
	 * Valuta una mano di Poker costituita da 2 sole carte;
	 * restituisce un indice di valutazione intero, maggiore è il valore del punto
	 * contenuto nella mano maggiore sarà l'indice
	 * 
	 * @param all	[I] array contenente le 2 carte della mano
	 * 
	 * @return indice di valutazione della mano
	 */
	public function evaluate2($all) {
		$a0 = $all[0];
		$a1 = $all[1];
		$z0 = self::$div4[$a0];
		$z1 = self::$div4[$a1];
		$zzTemp = 0;
		
		
		// Controllo se le 2 carte sono uguali
		if ($z0 == $z1) {
			// Salvo come punto vincente: One pair - id=1
			$this->bestHandId = 1;
			
			
			// Restituisco la valutazione
			return 200 + $z0;
		}
		
		// Controllo se la prima carta è minore della seconda
		if ($z0 < $z1) {
			// Scambio le carte in modo da averle ordinate in ordine decrescente
			$zzTemp = $z0;
			$z0 = $z1;
			$z1 = $zzTemp;		
		}
		
		// Salvo come punto vincente: High card - id=0
		$this->bestHandId = 0;
					
		
		// Restituisco la valutazione
		return ($z0 * $z0) + $z1;
	}	
	
	/**
	 * Valuta una mano di Poker costituita da 5 carte;
	 * restituisce un indice di valutazione intero, maggiore è il valore del punto
	 * contenuto nella mano maggiore sarà l'indice
	 * 
	 * @param all	[I] array contenente le 5 carte della mano
	 * 
	 * @return indice di valutazione della mano
	 */
	public function evaluate5($all) {
		$max = 0;
		$temp='';
	    //sort($all);
	    $a0 = $all[0];
	    $a1 = $all[1];
	    $a2 = $all[2];
	    $a3 = $all[3];
	    $a4 = $all[4];
	    /**/
	    // sort
	    $swap='';
        if ($a0 < $a1) { $swap = $a0; $a0 = $a1; $a1 = $swap; }
        if ($a0 < $a2) { $swap = $a0; $a0 = $a2; $a2 = $swap; }
        if ($a0 < $a3) { $swap = $a0; $a0 = $a3; $a3 = $swap; }
        if ($a0 < $a4) { $swap = $a0; $a0 = $a4; $a4 = $swap; }
        if ($a1 < $a2) { $swap = $a1; $a1 = $a2; $a2 = $swap; }
        if ($a1 < $a3) { $swap = $a1; $a1 = $a3; $a3 = $swap; }
        if ($a1 < $a4) { $swap = $a1; $a1 = $a4; $a4 = $swap; }
        if ($a2 < $a3) { $swap = $a2; $a2 = $a3; $a3 = $swap; }
        if ($a2 < $a4) { $swap = $a2; $a2 = $a4; $a4 = $swap; }
        if ($a3 < $a4) { $swap = $a3; $a3 = $a4; $a4 = $swap; }





	    // general check for flush
	   $flush = false;

	    $suits = array();
	    $suits[self::$mod4[$a0]]++; 
	    $suits[self::$mod4[$a1]]++; 
	    $suits[self::$mod4[$a2]]++; 
	    $suits[self::$mod4[$a3]]++; 
	    $suits[self::$mod4[$a4]]++;
	    if ($suits[0] > 4 || $suits[1] > 4 ||$suits[2] > 4 ||$suits[3] > 4)
	    	$flush = true;

	    $zz0 = self::$div4[$a0];
	    $zz1 = self::$div4[$a1];
	    $zz2 = self::$div4[$a2];
	    $zz3 = self::$div4[$a3];
	    $zz4 = self::$div4[$a4];
	    $this->z0 = $zz0;
	    $this->z1 = $zz1;
	    $this->z2 = $zz2;
	    $this->z3 = $zz3;
	    $this->z4 = $zz4;

	    $temp = $this->value2($a0, $a1, $a2, $a3, $a4, $flush); 
	    if ($temp > $max) {
	    	$max = $temp;
	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;

	    	$this->bestHandCardsArray = array($a0, $a1, $a2, $a3, $a4);
	    }

	    return $max;
	}

	/**
	 * Valuta una mano di Poker costituita da 6 carte;
	 * restituisce un indice di valutazione intero, maggiore è il valore del punto
	 * contenuto nella mano maggiore sarà l'indice
	 * 
	 * @param all	[I] array contenente le 6 carte della mano
	 * 
	 * @return indice di valutazione della mano
	 */	
	public function evaluate6($all) {
		$max = 0;
        $temp='';
	    //sort($all);
	    $a0 = $all[0];
	    $a1 = $all[1];
	    $a2 = $all[2];
	    $a3 = $all[3];
	    $a4 = $all[4];
	    $a5 = $all[5];
	    /**/
	    // sort
	    $swap='';
	    if ($a0 < $a1) { $swap = $a0; $a0 = $a1; $a1 = $swap; }
	    if ($a0 < $a2) { $swap = $a0; $a0 = $a2; $a2 = $swap; }
	    if ($a0 < $a3) { $swap = $a0; $a0 = $a3; $a3 = $swap; }
	    if ($a0 < $a4) { $swap = $a0; $a0 = $a4; $a4 = $swap; }
	    if ($a0 < $a5) { $swap = $a0; $a0 = $a5; $a5 = $swap; }
	    if ($a1 < $a2) { $swap = $a1; $a1 = $a2; $a2 = $swap; }
	    if ($a1 < $a3) { $swap = $a1; $a1 = $a3; $a3 = $swap; }
	    if ($a1 < $a4) { $swap = $a1; $a1 = $a4; $a4 = $swap; }
	    if ($a1 < $a5) { $swap = $a1; $a1 = $a5; $a5 = $swap; }
	    if ($a2 < $a3) { $swap = $a2; $a2 = $a3; $a3 = $swap; }
	    if ($a2 < $a4) { $swap = $a2; $a2 = $a4; $a4 = $swap; }
	    if ($a2 < $a5) { $swap = $a2; $a2 = $a5; $a5 = $swap; }
	    if ($a3 < $a4) { $swap = $a3; $a3 = $a4; $a4 = $swap; }
	    if ($a3 < $a5) { $swap = $a3; $a3 = $a5; $a5 = $swap; }
	    if ($a4 < $a5) { $swap = $a4; $a4 = $a5; $a5 = $swap; }

	    // general check for flush
	     $flush = false;

	    $suits = array();
	    $suits[self::$mod4[$a0]]++; 
	    $suits[self::$mod4[$a1]]++; 
	    $suits[self::$mod4[$a2]]++; 
	    $suits[self::$mod4[$a3]]++;
	    $suits[self::$mod4[$a4]]++; 
	    $suits[self::$mod4[$a5]]++;
	    if ($suits[0] > 4 || $suits[1] > 4 ||$suits[2] > 4 ||$suits[3] > 4) 
	    	$flush = true;

	    $z0 = self::$div4[$a0];
	    $z1 = self::$div4[$a1];
	    $z2 = self::$div4[$a2];
	    $z3 = self::$div4[$a3];
	    $z4 = self::$div4[$a4];
	    $z5 = self::$div4[$a5];
	    $this->z0 = $z0; 
	    $this->z1 = $z1; 
	    $this->z2 = $z2; 
	    $this->z3 = $z3; 
	    $this->z4 = $z4;
	    
	    $temp = $this->value2($a0, $a1, $a2, $a3, $a4, $flush); 
	    if ($temp > $max) {
	    	$max = $temp;	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;
	    	$this->bestHandCardsArray = array($a0, $a1, $a2, $a3, $a4);
	    }
	    
	    $this->z4 = $z5;
	    $temp = $this->value2($a0, $a1, $a2, $a3, $a5, $flush); 
	    if ($temp > $max) {
	    	$max = $temp;	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;
	    	$this->bestHandCardsArray = array($a0, $a1, $a2, $a3, $a5);
	    }
	    
	    $this->z3 = $z4; 
	    $this->z4 = $z5;
	    $temp = $this->value2($a0, $a1, $a2, $a4, $a5, $flush); 
	    if ($temp > $max) {
	    	$max = $temp;	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;
	    	$this->bestHandCardsArray = array($a0, $a1, $a2, $a4, $a5);
	    }
	 
	    $this->z2 = $z3; 
	    $this->z3 = $z4; 
	    $this->z4 = $z5;
	    $temp = $this->value2($a0, $a1, $a3, $a4, $a5, $flush); 
	    if ($temp > $max) {
	    	$max = $temp;	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;
	    	$this->bestHandCardsArray = array($a0, $a1, $a3, $a4, $a5);
	    }
	    
	    $this->z1 = $z2; 
	    $this->z2 = $z3; 
	    $this->z3 = $z4; 
	    $this->z4 = $z5;
	    $temp = $this->value2($a0, $a2, $a3, $a4, $a5, $flush); 
	    if ($temp > $max) {
	    	$max = $temp;	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;
	    	$this->bestHandCardsArray = array($a0, $a2, $a3, $a4, $a5);
	    }
	    
	    $this->z0 = $z1; 
	    $this->z1 = $z2; 
	    $this->z2 = $z3; 
	    $this->z3 = $z4; 
	    $this->z4 = $z5;
	    $temp = $this->value2($a1, $a2, $a3, $a4, $a5, $flush); 
	    if ($temp > $max) {
	    	$max = $temp;	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;
	    	$this->bestHandCardsArray = array($a1, $a2, $a3, $a4, $a5);
	    }
	    
	    
	    return $max;
	}

	/**
	 * Valuta una mano di Poker costituita da 7 carte;
	 * restituisce un indice di valutazione intero, maggiore è il valore del punto
	 * contenuto nella mano maggiore sarà l'indice
	 * 
	 * @param all	[I] array contenente le 7 carte della mano
	 * 
	 * @return indice di valutazione della mano
	 */	
	public function evaluate7($all) {
	   $max = 0; $temp='';
	   //sort($all);
	   $a0 = $all[0];
	   $a1 = $all[1];
	   $a2 = $all[2];
	   $a3 = $all[3];
	   $a4 = $all[4];
	   $a5 = $all[5];
	   $a6 = $all[6];
	   /**/
	   // sort
	   $swap='';
	   if ($a0 < $a1) { $swap = $a0; $a0 = $a1; $a1 = $swap; }
	   if ($a0 < $a2) { $swap = $a0; $a0 = $a2; $a2 = $swap; }
	   if ($a0 < $a3) { $swap = $a0; $a0 = $a3; $a3 = $swap; }
	   if ($a0 < $a4) { $swap = $a0; $a0 = $a4; $a4 = $swap; }
	   if ($a0 < $a5) { $swap = $a0; $a0 = $a5; $a5 = $swap; }
	   if ($a0 < $a6) { $swap = $a0; $a0 = $a6; $a6 = $swap; }
	   if ($a1 < $a2) { $swap = $a1; $a1 = $a2; $a2 = $swap; }
	   if ($a1 < $a3) { $swap = $a1; $a1 = $a3; $a3 = $swap; }
	   if ($a1 < $a4) { $swap = $a1; $a1 = $a4; $a4 = $swap; }
	   if ($a1 < $a5) { $swap = $a1; $a1 = $a5; $a5 = $swap; }
	   if ($a1 < $a6) { $swap = $a1; $a1 = $a6; $a6 = $swap; }
	   if ($a2 < $a3) { $swap = $a2; $a2 = $a3; $a3 = $swap; }
	   if ($a2 < $a4) { $swap = $a2; $a2 = $a4; $a4 = $swap; }
	   if ($a2 < $a5) { $swap = $a2; $a2 = $a5; $a5 = $swap; }
	   if ($a2 < $a6) { $swap = $a2; $a2 = $a6; $a6 = $swap; }
	   if ($a3 < $a4) { $swap = $a3; $a3 = $a4; $a4 = $swap; }
	   if ($a3 < $a5) { $swap = $a3; $a3 = $a5; $a5 = $swap; }
	   if ($a3 < $a6) { $swap = $a3; $a3 = $a6; $a6 = $swap; }
	   if ($a4 < $a5) { $swap = $a4; $a4 = $a5; $a5 = $swap; }
	   if ($a4 < $a6) { $swap = $a4; $a4 = $a6; $a6 = $swap; }
	   if ($a5 < $a6) { $swap = $a5; $a5 = $a6; $a6 = $swap; }

	   // general check for flush
	   $flush = false;

	   $suits = array();
	   $suits[self::$mod4[$a0]]++; 
	   $suits[self::$mod4[$a1]]++; 
	   $suits[self::$mod4[$a2]]++; 
	   $suits[self::$mod4[$a3]]++;
	   $suits[self::$mod4[$a4]]++; 
	   $suits[self::$mod4[$a5]]++; 
	   $suits[self::$mod4[$a6]]++;
	   if ($suits[0] > 4 || $suits[1] > 4 ||$suits[2] > 4 ||$suits[3] > 4)
		   $flush = true;
	   
	   $z0 = self::$div4[$a0];
	   $z1 = self::$div4[$a1];
	   $z2 = self::$div4[$a2];
	   $z3 = self::$div4[$a3];
	   $z4 = self::$div4[$a4];
	   $z5 = self::$div4[$a5];
	   $z6 = self::$div4[$a6];
	   $this->z0 = $z0; 
	   $this->z1 = $z1; 
	   $this->z2 = $z2; 
	   $this->z3 = $z3; 
	   $this->z4 = $z4;
	   $temp = $this->value2($a0, $a1, $a2, $a3, $a4, $flush); 
	   if ($temp > $max) {
	    	$max = $temp;	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;
	    	$this->bestHandCardsArray = array($a0, $a1, $a2, $a3, $a4);
	    }
	   
	   $this->z4 = $z5;
	   $temp = $this->value2($a0, $a1, $a2, $a3, $a5, $flush); 
	   if ($temp > $max) {
	    	$max = $temp;	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;
	    	$this->bestHandCardsArray = array($a0, $a1, $a2, $a3, $a5);
	    }
	   
	   $this->z4 = $z6;
	   $temp = $this->value2($a0, $a1, $a2, $a3, $a6, $flush); 
	   if ($temp > $max) {
	    	$max = $temp;	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;
	    	$this->bestHandCardsArray = array($a0, $a1, $a2, $a3, $a6);
	    }
	   
	   $this->z3 = $z4; 
	   $this->z4 = $z5;
	   $temp = $this->value2($a0, $a1, $a2, $a4, $a5, $flush); 
	   if ($temp > $max) {
	    	$max = $temp;	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;
	    	$this->bestHandCardsArray = array($a0, $a1, $a2, $a4, $a5);
	    }
	   
	   $this->z3 = $z4; 
	   $this->z4 = $z6;
	   $temp = $this->value2($a0, $a1, $a2, $a4, $a6, $flush); 
	   if ($temp > $max) {
	    	$max = $temp;	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;
	    	$this->bestHandCardsArray = array($a0, $a1, $a2, $a4, $a6);
	    }
	   
	   $this->z3 = $z5; 
	   $this->z4 = $z6;
	   $temp = $this->value2($a0, $a1, $a2, $a5, $a6, $flush); 
	   if ($temp > $max) {
	    	$max = $temp;	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;
	    	$this->bestHandCardsArray = array($a0, $a1, $a2, $a5, $a6);
	    }
	   
	   $this->z2 = $z3; 
	   $this->z3 = $z4; 
	   $this->z4 = $z5;
	   $temp = $this->value2($a0, $a1, $a3, $a4, $a5, $flush); 
	   if ($temp > $max) {
	    	$max = $temp;	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;
	    	$this->bestHandCardsArray = array($a0, $a1, $a3, $a4, $a5);
	    }
	   
	   $this->z3 = $z4; 
	   $this->z4 = $z6;
	   $temp = $this->value2($a0, $a1, $a3, $a4, $a6, $flush); 
	   if ($temp > $max) {
	    	$max = $temp;	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;
	    	$this->bestHandCardsArray = array($a0, $a1, $a3, $a4, $a6);
	    }
	   
	   $this->z3 = $z5; 
	   $this->z4 = $z6;
	   $temp = $this->value2($a0, $a1, $a3, $a5, $a6, $flush); 
	   if ($temp > $max) {
	    	$max = $temp;	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;
	    	$this->bestHandCardsArray = array($a0, $a1, $a3, $a5, $a6);
	    }
	   
	   $this->z2 = $z4; 
	   $temp = $this->value2($a0, $a1, $a4, $a5, $a6, $flush); 
	   if ($temp > $max) {
	    	$max = $temp;	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;
	    	$this->bestHandCardsArray = array($a0, $a1, $a4, $a5, $a6);
	    }
	   
	   $this->z1 = $z2; 
	   $this->z2 = $z3; 
	   $this->z3 = $z4; 
	   $this->z4 = $z5;
	   $temp = $this->value2($a0, $a2, $a3, $a4, $a5, $flush); 
	   if ($temp > $max) {
	    	$max = $temp;	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;
	    	$this->bestHandCardsArray = array($a0, $a2, $a3, $a4, $a5);
	    }
	   
	   $this->z4 = $z6;
	   $temp = $this->value2($a0, $a2, $a3, $a4, $a6, $flush); 
	   if ($temp > $max) {
	    	$max = $temp;	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;
	    	$this->bestHandCardsArray = array($a0, $a2, $a3, $a4, $a6);
	    }
	   
	   $this->z3 = $z5; 
	   $this->z4 = $z6;
	   $temp = $this->value2($a0, $a2, $a3, $a5, $a6, $flush); 
	   if ($temp > $max) {
	    	$max = $temp;	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;
	    	$this->bestHandCardsArray = array($a0, $a2, $a3, $a5, $a6);
	    }
	   
	   $this->z2 = $z4; 
	   $this->z3 = $z5; 
	   $this->z4 = $z6;	   
	   $temp = $this->value2($a0, $a2, $a4, $a5, $a6, $flush); 
	   if ($temp > $max) {
	    	$max = $temp;	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;
	    	$this->bestHandCardsArray = array($a0, $a2, $a4, $a5, $a6);
	    }
	   
	   $this->z1 = $z3; 
	   $this->z2 = $z4; 
	   $this->z3 = $z5; 
	   $this->z4 = $z6;
	   $temp = $this->value2($a0, $a3, $a4, $a5, $a6, $flush); 
	   if ($temp > $max) {
	    	$max = $temp;	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;
	    	$this->bestHandCardsArray = array($a0, $a3, $a4, $a5, $a6);
	    }
	   
	   $this->z0 = $z1; 
	   $this->z1 = $z2; 
	   $this->z2 = $z3; 
	   $this->z3 = $z4; 
	   $this->z4 = $z5;
	   $temp = $this->value2($a1, $a2, $a3, $a4, $a5, $flush); 
	   if ($temp > $max) {
	    	$max = $temp;	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;
	    	$this->bestHandCardsArray = array($a1, $a2, $a3, $a4, $a5);
	    }
	   
	   $this->z4 = $z6;
	   $temp = $this->value2($a1, $a2, $a3, $a4, $a6, $flush); 
	   if ($temp > $max) {
	    	$max = $temp;	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;
	    	$this->bestHandCardsArray = array($a1, $a2, $a3, $a4, $a6);
	    }
	   
	   $this->z3 = $z5; 
	   $this->z4 = $z6;
	   $temp = $this->value2($a1, $a2, $a3, $a5, $a6, $flush); 
	   if ($temp > $max) {
	    	$max = $temp;	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;
	    	$this->bestHandCardsArray = array($a1, $a2, $a3, $a5, $a6);
	    }
	   
	   $this->z2 = $z4; 
	   $this->z3 = $z5; 
	   $this->z4 = $z6;
	   $temp = $this->value2($a1, $a2, $a4, $a5, $a6, $flush); 
	   if ($temp > $max) {
	    	$max = $temp;	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;
	    	$this->bestHandCardsArray = array($a1, $a2, $a4, $a5, $a6);
	    }
	   
	   $this->z1 = $z3; 
	   $this->z2 = $z4; 
	   $this->z3 = $z5; 
	   $this->z4 = $z6;
	   $temp = $this->value2($a1, $a3, $a4, $a5, $a6, $flush); 
	   if ($temp > $max) {
	    	$max = $temp;	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;
	    	$this->bestHandCardsArray = array($a1, $a3, $a4, $a5, $a6);
	    }
	   
	   $this->z0 = $z2; 
	   $this->z1 = $z3; 
	   $this->z2 = $z4; 
	   $this->z3 = $z5; 
	   $this->z4 = $z6;
	   $temp = $this->value2($a2, $a3, $a4, $a5, $a6, $flush); 
	   if ($temp > $max) {
	    	$max = $temp;	    	
	    	// Imposto l'id del punto migliore e le carte che lo compongono
	    	$this->bestHandId = $this->tempBestHandId;
	    	$this->bestHandCardsArray = array($a2, $a3, $a4, $a5, $a6);
	    }

	   
	   return $max;
	}

	/**
	 * Calcola il valore di una mano in accordo con tutte le regole sulle carte ed utilizzando le variabili
	 * statiche di classe valorizzate dal metodo che lo invoca
	 * 
	 * calculate the value of a hand according all rules
	 * cards that are given as input here should be sorted in desceding order!
	 * 
	 * @param x1		[I] id della prima carta della mano
	 * @param x2		[I] id della seconda carta della mano
	 * @param x3		[I] id della terza carta della mano
	 * @param x4		[I] id della quarta carta della mano
	 * @param x5		[I] id della quinta carta della mano
	 * @param flush		[I] flag che indica se è presente un colore 
	 * 
	 * @return indice di valutazione della mano
	 */	  
	public function value2($x1, $x2, $x3, $x4, $x5,  $flush) {

		

	    if ($flush) {
	    	$c0 = self::$mod4[$x1];
            $c1 = self::$mod4[$x2];
            $c2 = self::$mod4[$x3];
            $c3 = self::$mod4[$x4];
            $c4 = self::$mod4[$x5];
	    	$flush = ($c0 == $c1 && $c1 == $c2 && $c2 == $c3 && $c3 == $c4);
	    }

	    // Num of diff cards
	    $diff = 0;
	    if ($this->z0 != $this->z1) 
	    	$diff++;
	    if ($this->z1 != $this->z2) 
	    	$diff++;
	    if ($this->z2 != $this->z3) 
	    	$diff++;
	    if ($this->z3 != $this->z4) 
	    	$diff++;

	     $straight = false;
	    //$straight = ($z4 == $z0-4 && $z1 == $z0-1 && $z2 == $z1-1 && $z3 == $z2-1); // detect A $straight?
	    
	    if ($diff == 4) {
	    	if ($this->z4 == $this->z0 - 4) 
	    		$straight = true;
	    	else 
	    		if ($this->z0 == 12 && $this->z1 == 3) 
	    			$straight = true;  // A 5 4 3 2
	    }

	    
	    // 1. royal flush, 2. $straight flush
	    if ($straight && $flush) {
	    	// Salvo come punto vincente: $straight Flush - id=8
	    	$this->tempBestHandId = 8;
    		
	    	if ($this->z0 == 12 && $this->z1 == 3)
	    		return 10000000 + $this->z1; // this is 5-high $straight flush A 5 4 3 2, return 5 instead of A
	    		      
	    	// Controllo se il punto è Royal Flush o $straight Flush
	    	if ($this->z0 == 12) {
	    		// Salvo come punto vincente: Royal Flush - id=9
	    		$this->tempBestHandId = 9;
	    	}	    	

    		return 10000000 + $this->z0; // only highest card matters, rest are the same
	    }

	    
	    // 3. four of a kind
	    if ($diff == 1) {
	    	if ($this->z1 == $this->z2 && $this->z2 == $this->z3) { // può essere aaaab oppure abbbb
	    		// Salvo come punto vincente: Four of a kind - id=7
	    		$this->tempBestHandId = 7;
	    			    		
	    		if ($this->z0 != $this->z1)
	    			return 8000000 + self::$mult5[$this->z1] + $this->z0;	// abbbb	(es. QJJJJ)
	    		else return 8000000 + self::$mult5[$this->z1] + $this->z4;	// aaaab	(es. JJJJT)
	    	}
	    	
	    	// 4. full hause
	    	// diff == 1
	    	
	    	// Salvo come punto vincente: Full house - id=6
	    	$this->tempBestHandId = 6;
    		
	    	if ($this->z0 == $this->z2) { // aaabb	(Es. JJJTT)
	    		return 7000000 + self::$mult5[$this->z2] + $this->z4;
	    	} 
	    	else { // aabbb		(Es. QQJJJ)
	    		return 7000000 + self::$mult5[$this->z2] + $this->z0;
	    	}
	   }


	   // 5. Flush
	   if ($flush) {
		   // Salvo come punto vincente: Flush - id=5
		   $this->tempBestHandId = 5;

		   return 6000000 + self::$mult5[$this->z0] + self::$mult4[$this->z1] + self::$mult3[$this->z2] + self::$mult2[$this->z3] + self::$mult1[$this->z4];     // all card matters
	   }

	   
	   // 6. $straight
	   if ($straight) {
		   // Salvo come punto vincente: $straight - id=4
		   $this->tempBestHandId = 4;
		   		   
		   if ($this->z0 == 12 && $this->z1 == 3) 
			   return 5000000 + $this->z1; // this is 5-high $straight A 5 4 3 2, return 5 instead of A
	      
		   return 5000000 + $this->z0; // Only the highest card matters, rest are ignored
	   }

	   
	   // 7. Three of a kind
	   if ($diff == 2) {
		   // Salvo come punto vincente: Three of a kind - id=3
		   $this->tempBestHandId = 3;
		   
		   // aaabc, abbbc, abccc
		   if ($this->z2 == $this->z0) 	    		
			   return 4000000 + self::$mult5[$this->z2] + self::$mult2[$this->z3] + $this->z4;	// aaabc (es. JJJT6)
		   
		   if ($this->z2 == $this->z4)
			   return 4000000 + self::$mult5[$this->z2] + self::$mult2[$this->z0] + $this->z1;	// abccc (es. AKJJJ)
		   		   
		   if ($this->z1 == $this->z3)
			   return 4000000 + self::$mult5[$this->z2] + self::$mult2[$this->z0] + $this->z4; // abbbc (es. AJJJ6)
		   
		  
		   // 8. Two pairs
		   // diff == 2	      
		   // aabbc, aabcc, abbcc
		   
		   // Salvo come punto vincente: Two pairs - id=2
		   $this->tempBestHandId = 2;
		   		   
		   if ($this->z0 == $this->z1) {
			   if ($this->z2 == $this->z3) 
				   return 3000000 + self::$mult5[$this->z0] + self::$mult3[$this->z2] + self::$mult2[$this->z4]; // aabbc (es. JJ886)
			   
			   if ($this->z3 == $this->z4) 
				   return 3000000 + self::$mult5[$this->z0] + self::$mult3[$this->z3] + self::$mult2[$this->z2]; // aabcc (es. JJ866)
	      }
		   
	      if ($this->z1 == $this->z2 && $this->z3 == $this->z4) 
	    	  return 3000000 + self::$mult5[$this->z1] + self::$mult3[$this->z3] + self::$mult2[$this->z0]; // abbcc (es. J8866)
	   }
	   

	   // 9. One pair
	   if ($diff == 3) {
		   // Salvo come punto vincente: One pair - id=1
		   $this->tempBestHandId = 1;
		   
		   // aabcd, abbcd, abccd, abcdd
		   if ($this->z0 == $this->z1) 
			   return 2000000 + self::$mult5[$this->z0] + self::$mult3[$this->z2] + self::$mult2[$this->z3] + self::$mult1[$this->z4]; // aabcd (es. TT986)
		   
		   if ($this->z1 == $this->z2) 
			   return 2000000 + self::$mult5[$this->z1] + self::$mult3[$this->z0] + self::$mult2[$this->z3] + self::$mult1[$this->z4]; // abbcd (es. T9986)
		   
		   if ($this->z2 == $this->z3) 
			   return 2000000 + self::$mult5[$this->z2] + self::$mult3[$this->z0] + self::$mult2[$this->z1] + self::$mult1[$this->z4]; // abccd (es. T9886)
		   
		   if ($this->z3 == $this->z4) 
			   return 2000000 + self::$mult5[$this->z3] + self::$mult3[$this->z0] + self::$mult2[$this->z1] + self::$mult1[$this->z2]; // abcdd (es. T9866)
	   }


	   // Salvo come punto vincente: High Card - id=0
	   $this->tempBestHandId = 0;
	   
	   
	   // 10. High card
	   return self::$mult5[$this->z0] + self::$mult4[$this->z1] + self::$mult3[$this->z2] + self::$mult2[$this->z3] + self::$mult1[$this->z4];   // all card matters
	 }
}

?>