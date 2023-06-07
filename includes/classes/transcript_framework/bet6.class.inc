<?php
require_once 'Transcript.class.inc';
require_once 'transcript.inc.php';
require_once 'general_functions.inc';
define("bet6_img" , "../media/images/handimages/bet6/");
define("LOTTO",1);
define("ULTIMO NUMERO",2);
define("COLORE PREDOMINANTE",3);
define("TOTALE IN UN RANGE",4);
define("ULTIMO COLORE",5);
define("PALLA PIU' ALTA PRIMA O ULTIMA",6);
define("NUMERO PALLE DI UN COLORE",7);
define("PIU' PARI O PIU' DISPARI",8);
define("SOMMA PARI O DISPARI",9);
define("SUPERSIX",10);
define("BET6",11);
class bet6 extends Transcript {
    function bet6() {
    }

    public function getNames() {
        return "bet6";
    }
    function getBet() {
        $betData = split ( ";", $this->betString );


        return "";
    }


    function getReadbleTranscript()
    {
        $result=explode('+',$this->resultString);
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

            function showInfo(id){
                $(".bet6hidden").hide();
                $("#table_"+id).show();
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

		<tr><td class='formheading centered' style='font-size:x-large; height:31px;width:300px'>HELP</td></tr>
		<tr><td class='content centered' style='vertical-align:top'>
			<!--	<INPUT type='image' src='".bet6_img."btn_1.png' style='width:90px; height:60px;'  onclick=toggle('lotto');> -->
				<INPUT type='image' src='".bet6_img."btn_2.png' style='width:90px; height:60px;'  onclick=toggle('ultimo');>
				<INPUT type='image' src='".bet6_img."btn_3.png' style='width:90px; height:60px;'  onclick=toggle('colore');>
				<INPUT type='image' src='".bet6_img."btn_4.png' style='width:90px; height:60px;'  onclick=toggle('totale');>
				<INPUT type='image' src='".bet6_img."btn_5.png' style='width:90px; height:60px;'  onclick=toggle('ucolore');>
				<INPUT type='image' src='".bet6_img."btn_6.png' style='width:90px; height:60px;'  onclick=toggle('ppou');>
				<INPUT type='image' src='".bet6_img."btn_7.png' style='width:90px; height:60px;'  onclick=toggle('numero');>
	    	<!--	<INPUT type='image' src='".bet6_img."btn_8.png' style='width:90px; height:60px;'  onclick=toggle('ppod');>
				<INPUT type='image' src='".bet6_img."btn_9.png' style='width:90px; height:60px;'  onclick=toggle('spod');>
				<INPUT type='image' src='".bet6_img."btn_10.png' style='width:120px; height:35px;' onclick=toggle('supersix');> -->
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



           $i=0;

        $html .="<ul class='buttonul'>";

        foreach($result as $k => $value){


            list($tiketdetails,$res)=explode(';',$value);

            list($handid,$tiketid,$date)=explode(',',$tiketdetails);
            $html.="<li><a href=javascript:showInfo('$i') ><button class='entitybtn'>".$handid."</button></a></li>";
            $i++;
        }
        $html .="</ul>";



        $betarray=array("1"=>"LOTTO",
            "2"=>"ULTIMO NUMERO",
            "3"=>"COLORE PREDOMINANTE",
            "4"=>"TOTALE IN UN RANGE",
            "5"=>"ULTIMO COLORE",
            "6"=>"PALLA PIU' ALTA PRIMA O ULTIMA",
            "7"=>"NUMERO PALLE DI UN COLORE",
            "8"=>"PIU' PARI O PIU' DISPARI",
            "9"=>"SOMMA PARI O DISPARI",
            "10"=>"SUPERSIX",
            "11"=>"BET6");


        $cotesall=explode('-',$this->resString);



        foreach($result as $k => $value){
           $info=explode(';',$value);
            $fmm=explode('@',$info['1']);
            $handDetails=explode('!',end($fmm));
            $allballs=end($handDetails);


            $html .="<table id=table_".$k." class='bet6hidden'";
            if($k!=0){
            $html.="style='display:none'";
            }
                $html.="><tr><td><table style='margin:auto; width: 494px !important;' >
                                    <tr>
                                        <th class='label'colspan='12' style='font-size:x-large;'>Result Balls</th>
                                    </tr>
                                    <tr>
                                        <td>";
                                $html .="<ul class='bet6'>";
                                for($i=0;$i<35;$i++)
                                {
                                    $balls=explode('-',$allballs);
                                    list($value,$color)=explode('|',$balls[$i]);
                                    $allcotes=explode(",",end($cotesall));

                                    $html .="<li><div class='ballbet6 bold ";
                                    switch ($color){
                                        case 'V';
                                            if($color=='V')
                                            {
                                                $html .="green";
                                            }
                                            break;
                                        case 'G';
                                            if($color=='G')
                                            {
                                                $html .="yellow";
                                            }
                                            break;
                                        case 'P';
                                            if ($color=='P')
                                            {
                                                $html .="pink";
                                            }
                                            break;
                                        case 'O';
                                            if ($color=='O')
                                            {
                                                $html .="orange";
                                            }
                                            break;
                                        case 'U';
                                            if ($color=='U')
                                            {
                                                $html .="violet";
                                            }
                                            break;
                                        case 'B';
                                            if ($color=='B')
                                            {
                                                $html .="blue";
                                            }
                                            break;
                                        case 'R';
                                            if ($color=='R')
                                            {
                                                $html .="red";
                                            }
                                            break;
                                    }
                                    $html .="'>$value</div><div class='cote'>$allcotes[$i]</div></li>";
                                }

                             $html .="
                    </td>
                    <td>  <table  style='margin:auto;' bgcolor='#a99a9'  cellspacing='1' cellpadding='5' border='0' ><tr><!--<td class='label' style='font-size:x-large'>Jackpot</td></tr><tr><td class='content centered' style='font-size:large' >".getInDollars( $jackpot/100)."</td>-->
                         </td></tr></table>
                         </td></tr></table>";



            $bets= new StringTokenizer ( $info['1'], "@" );
            $html .="</td></tr><tr><td><table style='margin:auto ' bgcolor='#a99a9'  cellspacing='1' cellpadding='5' border='0'><tr><td class='label large' >Multiplier</td><td class='label large' >Bet</td><td class='label large'>Bet Description</td> <td class='label large'>Amount</td><td class='label large'>Win</td></tr>";
            $totalbet=0;
            $totalwin=0;
            while($bets->hasMoreTokens()){
                list($bettype,$betamount,$bet,$win,$idk1,$idk2,$multiplier,$ballsplm)=explode("!",  $bets->nextToken());

                     $html.="<tr><td class='content'>".$multiplier."</td><td class='content'>";

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
                elseif($bet[0]=='G' || $bet[0]=='R' || $bet[0]=='V' || $bet[0]=='B' || $bet[0]=='U' || $bet[0]=='P'|| $bet[0]=='O')
                {
                    $bet=explode('-', $bet);

                    if($bet['0']=='G')
                    {
                        $bet['0']='YELLOW';
                    }
                    elseif ($bet['0']=='R')
                    {
                        $bet['0']='RED';
                    }
                    elseif ($bet['0']=='V')
                    {
                        $bet['0']='GREEN';
                    }
                    elseif ($bet['0']=='B')
                    {
                        $bet['0']='BLUE';
                    }
                    elseif ($bet['0']=='U')
                    {
                        $bet['0']='PURPLE';
                    }
                    elseif ($bet['0']=='P')
                    {
                        $bet['0']='PINK';
                    }
                    elseif ($bet['0']=='O')
                    {
                        $bet['0']='ORANGE';
                    }
                    $bet=implode(' - ', $bet);

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
            $html .="<tr><td class='content' colspan='5'></td></tr>";
            $html .="<tr><td class='content' colspan='1'></td><td class='content' colspan='2'><b>Total :   ".getInDollars(($totalwin-$totalbet)/100)."</b></td><td class='content'><b>".getInDollars($totalbet/100)."</b></td><td class='content'><b>".getInDollars($totalwin/100)."</b></tr>";
            $html .="</table></td></tr></table></div>";

        }





        return $html;
    }
}
?>
