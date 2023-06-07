<?php
require_once 'Transcript.class.inc';
require_once 'transcript.inc.php';
require_once 'general_functions.inc';
define("ballomatic_img" , "../media/images/handimages/ballomatic/");
// define("LOTTO",1);
// define("ULTIMO NUMERO",2);
// define("COLORE PREDOMINANTE",3);
// define("TOTALE IN UN RANGE",4);
// define("ULTIMO COLORE",5);
// define("PALLA PIU' ALTA PRIMA O ULTIMA",6);
// define("NUMERO PALLE DI UN COLORE",7);
// define("PIU' PARI O PIU' DISPARI",8);
// define("SOMMA PARI O DISPARI",9);
// define("SUPERSIX",10);

class ballomatic_cd extends Transcript {
	function ballomatic_cd() {
	}
	
	public function getNames() {
		return "ballomatic_cd";
	}	
	function getBet() {
		$betData = split ( ";", $this->betString );
		
	
		return "";
	}	

	
	function getReadbleTranscript() 
	{	
		$details=split(";",$this->resultString);
		$jackpot=$details['2'];
		$result=explode('@',$this->resString);
		$balls=explode("-",$result[0]);
	    $html ="<div class='centered'>";
	    ?>
	    		<script>
		function toggle(id)
		{	
		    if ( $('#'+id).is(":hidden")) {	
				$('.help').slideUp('fast');
				$('#'+id).slideDown('fast');
			} else {
				$('#'+id).slideUp('fast');
			}            
			return false;
		}
		</script>
		
		<?php
        //* modified by Razvan//*
        $erase = "handerased";
        $poserased=strpos($this->betString,$erase);
        $reserased=strpos($this->resultString,$erase);
        if($poserased!==false || $reserased!==false ){
            $html .= "<h3 style='color:red;text-align: center'>HAND ERASED</h3><br />";
        }
        //* modified by Razvan//*
		$html .=" <table id='helptable' bgcolor='#a99a9'  cellspacing='0' cellpadding='1' >

		<tr><td class='formheading centered' style='font-size:x-large;'>HELP</td></tr>
		<tr><td class='content centered'>
				<INPUT type='image' src='".ballomatic_img."btn_1.png' style='width:90px; height:60px;'  onclick=toggle('lotto');>
				<INPUT type='image' src='".ballomatic_img."btn_2.png' style='width:90px; height:60px;'  onclick=toggle('ultimo');>		
				<INPUT type='image' src='".ballomatic_img."btn_3.png' style='width:90px; height:60px;'  onclick=toggle('colore');>
				<INPUT type='image' src='".ballomatic_img."btn_4.png' style='width:90px; height:60px;'  onclick=toggle('totale');>
				<INPUT type='image' src='".ballomatic_img."btn_5.png' style='width:90px; height:60px;'  onclick=toggle('ucolore');>
				<INPUT type='image' src='".ballomatic_img."btn_6.png' style='width:90px; height:60px;'  onclick=toggle('ppou');>
				<INPUT type='image' src='".ballomatic_img."btn_7.png'  style='width:90px; height:60px;'  onclick=toggle('numero');>
	    		<INPUT type='image' src='".ballomatic_img."btn_8.png' style='width:90px; height:60px;'   onclick=toggle('ppod');>
				<INPUT type='image' src='".ballomatic_img."btn_9.png' style='width:90px; height:60px;'  onclick=toggle('spod');>
				<INPUT type='image' src='".ballomatic_img."btn_10.png' style='width:120px; height:35px;'  onclick=toggle('supersix');>
		</td></tr>
		<tr><td> <div class='help content'id='lotto'>Lo scopo del lotto &#232; indovinare esattamente 1,2,3 o 4 numeri estratti. Per vincere i numeri selezionati devono corrispondere ai risultati dell'ultima estrazione. Per esempio se vengono selezionati 3 numeri, tutti e 3 i numeri devono corrispondere. Se solo due di questi corrispondono, il match risulter&#224; perso.
Per effettuare una scommessa Lotto selezionare i numeri dal menu cliccando su questi. Puoi selezionare 1,2,3 o 4 numeri. Clicca su conferma per effettuare la puntata.</div>
				<div class='help content' id='ultimo'>Lo scopo della puntata Ultimo numero &#232; di indovinare esattamente quale sar&#224; l'ultimo numero presente sulla barra dei numeri estratti.

Se il numero selezionato apparir&#224; nell'estrazione finale, la puntata viene ritenuta vincente. Per effettuare la scommessa su Ultimo numero selezionare uno dei numeri da 1 a 36 dalla schermata di selezione. Clicca conferma per confermare la puntata. </div>
				<div class='help content' id='colore'> Lo scopo della puntata sul colore predominante &#232; di indovinare esattamete se saranno presenti piu colori gialli rossi o verdi nell'estrazione finale delle 6 palle. Ad esempio, se l'estrazione finale consiste di 3 palle gialle, 1 verde e 2 rosse, Il giallo risulter&#224; vincente. Se l'estrazione finale sar&#224; 3 palle gialle e 3 rosse non ci sar&#224; nessun vincente poich&#232; i gialli ed i rossi avranno un punteggio di parit&#224; (3 a 3).
Per effettuare una scommessa Colore predominante, selezionare giallo verde o rosso dallo schermo di selezione delle puntate. Clicca su conferma per effettuare la puntata.</div>		
				<div class='help content' id='totale'>Lo scopo della puntata Total &#232; indovinare se la somma dei numeri estratti sar&#224; in una fascia di numeri Bassi, Alti o medi.
Per effettuare una scommessa sul Totale, selezionare alto medio o basso dalla schermata di selezione apposita. Clicca su conferma per effettuare la puntata.</div>	   
				<div class='help content' id='ucolore'> Lo scopo della puntata 'ultimo colore' &#232; quello di indovinare quale sar&#224; il colore della sesta palla (l'ultima estratta tra le sei). Se il colore scelto corrisponde a quello estrattto, la puntata &#232; ritenuta vincente.

Per effettuare una puntata sull' Ulitmo colore, selezionare uno dei tre colori dallo schermo di selezione delle puntate. Clicca su conferma per effettuare la puntata.</div>	
				<div class='help content' id='ppou'>
 Lo scopo della puntata 'primo o ultimo' &#232; quello di indovinare se il punteggio del primo numero estratto sar&#224; superiore o inferiore rispetto all'ultimo estratto.

Per effettuare una puntata su 'primo o ultimo' selezionare o la freccia alta o quella bassa, la freccia alta sta a significare che la prima palla sar&#224; piu alta dell'ultima, mentre la freccia bassa significher&#224; che la prima palla sar&#224; piu bassa. Effettuare la puntata dalla schermata apposita di selezione e cliccare su conferma per effettuare la puntata.</div>		
				<div class='help content' id='numero'> Lo scopo della puntata 'numero colore' &#232; di indovinare quante palle di un determinato colore usciranno. Dovrai indovinare il colore tra 0 e 6 palle. Per vincere tutti i colori selezionati devono combaciare con i risultati dell'ultima mano estratta. Per esempio, selezionando 3 colori, 3 e solo 3 devono combaciare. Se solo due di questi non combaceranno, la puntata non &#232; ritenuta vincente, allo stesso modo, se 4 saranno i colori estratti, la puntata &#232; ritenuta non vincente.

Per effettuare una puntata su 'numero colori' selezionare il colore dalla schermata di selezione e seguentemente selezionare quante volte si pensa che il colore sar&#224; estratto. Cliccare su conferma per effettuare la puntata.</div>	
				<div class='help content' id='ppod'>Lo scopo della puntata +pari o +dispari e di indovinare se ci saranno piu palle con numeri dispari o piu palle con numeri pari delle sei estratte.

La scelta effettuata dovr&#224; apparire almeno una volta in piu delle altre. Per esempio, se nell'estrazione finale vi saranno 3 palle dispari e 3 pari, non ci sar&#224; nessun vincitore.

Per effettuare una puntata su 'piu pari o piu dispari' selezionare pari o dispari dalla schermata di selezione. Per confermare la puntata cliccare su conferma.</div>		
				<div class='help content' id='spod'> Lo scopo della puntata Pari o Dispari &#232; quello di indovinare se la somma totale dei 6 numeri estratti sar&#224; un numero pari o dispari.

Per effettuare tale puntata selezionare dalla schermata di selezione apposita 'pari' o 'dispari'.   Cliccare su conferma per effettuare la puntata</div>
				<div class='help content' id='supersix'>Per concorrere all'estrazione Supersix sar&#224; necessario cliccare sul pulsante 'play' nel box apposito.
L'ammontare fissato per tale scommessa &#232; 1 euro.
E' possibile puntare su un supersix unico per mano.
Se alla fine della mano di gioco le palle estratte saranno le stesse mostrate nell'area supersix, sar&#224; vinto il supersix dell'ammontare descritto nell'area sovrastante.
Nel caso in cui piu di un giocatore risulti vincitore del supersix, l'ammontare totale sar&#224; diviso tra i vari vincitori. L'ammontare del supersix si incrementa continuamente finche il premio non viene vinto.</div></td></tr>
		</table>";		
	    
	    $html.="<table style='margin:auto;'><tr>";
		$html .="<td><table style='margin:auto;' ><tr><td class='label'colspan='6' style='font-size:x-large;'>Result Balls</td></tr><tr>";
		for($i=0;$i<6;$i++)
		{
			list($value,$color)=explode('|',$balls[$i]);
			$html .="<td class='ball "; 
			if($color=='V')
			{
				$html .="green";
			}
			elseif ($color=='G')
			{
				$html .="yellow";
			}
			else 
			{
				$html .="red";
			}			
		$html .="'>$value</td>";
		}
		$html .="</tr></table></td><td><table  style='margin:auto;' bgcolor='#a99a9'  cellspacing='1' cellpadding='5' border='0' ><tr><td class='label' style='font-size:x-large'>Jackpot</td></tr><tr><td class='content centered' style='font-size:large' >".getInDollars( $jackpot/100)."</td></tr></table></td>
				<tr></table>";
		
		//	11753924,442-gqx,2012-10-11=14:21:38;
		//	7!100!V-6!0@8!100!P!270@9!100!P!0@1!100!1-33-20-29!0@2!100!14!0@3!100!G!0@4!100!M!270@5!100!G!0@6!100!U!180@7!100!R-5!0@8!100!D!0@9!100!P!0@10!100!2-5-6-19-24-31!0;
		//  0


$betarray=array("1"=>"LOTTO",
				"2"=>"ULTIMO NUMERO",
				"3"=>"COLORE PREDOMINANTE",
				"4"=>"TOTALE IN UN RANGE",
				"5"=>"ULTIMO COLORE",
				"6"=>"PALLA PIU' ALTA PRIMA O ULTIMA",
				"7"=>"NUMERO PALLE DI UN COLORE",
				"8"=>"PIU' PARI O PIU' DISPARI",
				"9"=>"SOMMA PARI O DISPARI",
				"10"=>"SUPERSIX");


$bets= new StringTokenizer ( $details['1'], "@" );
$html .="<table style='margin:auto;' bgcolor='#a99a9'  cellspacing='1' cellpadding='5' border='0'><tr><td class='label large' >Bet</td><td class='label large'>Bet Description</td> <td class='label large'>Amount</td><td class='label large'>Win</td></tr>";
$totalbet=0;
$totalwin=0;
while($bets->hasMoreTokens()){
	list($bettype,$betamount,$bet,$win)=explode("!",  $bets->nextToken());	
	$html.="<tr><td class='content'>";
	
	if($bet=='U')
	{
		$bet='ULTIMA';
	}
	elseif($bet=='P')
	{
		if($bettype=='6')
		{
			$bet='PRIMA';
		}
		else
		{
		$bet='PARI';
		}
	}
	elseif($bet=='D')
	{
		$bet='DISPARI';
	}
	elseif($bet=='L')
	{
		$bet='BASSO';
	}
	elseif($bet=='H')
	{
		$bet='ALTO';
	}
	elseif($bet=='M')
	{
		$bet='MEDIO';
	}
	elseif($bet[0]=='G' || $bet[0]=='R' || $bet[0]=='V')
	{
		$bet=split('-', $bet);
		if($bet['0']=='G')
		{
			$bet['0']='GIALLO';
		}
		elseif ($bet['0']=='R')
		{
			$bet['0']='ROSSO';
		}
		elseif ($bet['0']=='V')
		{
			$bet['0']='VERDE';
		}
		$bet=implode('-', $bet);		
	}
	$html .="$bet</td><td class='content'>";
	foreach ($betarray as $key=>$value){
	if($bettype==$key)
	{
		$html .="$value";
	}
}
$totalbet=$totalbet+$betamount;
$totalwin=$totalwin+$win;
$html .="</td><td class='content'>".getInDollars($betamount/100)."</td><td class='content'>".getInDollars($win/100)."</td></tr>";
}
$html .="<tr><td class='content' colspan='4'></td></tr>";
$html .="<tr><td class='content' colspan='2'><b>Total :   ".getInDollars(($totalwin-$totalbet)/100)."</b></td><td class='content'><b>".getInDollars($totalbet/100)."</b></td><td class='content'><b>".getInDollars($totalwin/100)."</b></tr>";
$html .="</table></div>";
return $html;
}
}
?>
