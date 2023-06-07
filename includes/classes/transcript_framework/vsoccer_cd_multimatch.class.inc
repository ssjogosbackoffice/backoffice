<?php
define ( "IMG_PATH", "/games/%s/%s/admin_tokens/" );
define ( "PAYOUT_PATH", "/games/%s/%s/" );
class vsoccer_cd_multimatch extends Transcript {

    function vsoccer_cd_multimatch() {

    }

    public function getNames() {
        return "vsoccer_cd_multimatch";
    }

    function getBet() {
        $betData = split ( ";", $this->betString );
        $html = "";
        if ($betData [1] == 0) {
            $html .= "<b>PLAY IN FREESPIN</b>";
        } else {
            $html .= "PAY: <b>" . getInDollars ( $betData [1] / 100 ) . "</b> FOR LINE <br>";
            $html .= "TOTAL LINE: <b>" . number_format ( $betData [0] / $betData [1], 0 ) . " </b>";
        }
        return "";
    }


    private function getTeam($team){
        $this->teamName='';
        $teams1=array(
            0  => "Albania",
            1  => "Austria",
            2  => "Bosnia",
            3  => "Bulgaria",
            4  => "Ciprus",
            5  => "Croatia",
            6  => "Czech",
            7  => "Denmark",
            8  => "England",
            9  => "Estonia",
            10 => "Finland",
            11 => "France",
            12 => "Galles",
            13 => "Georgia",
            14 => "Germany",
            15 => "Greece",
            16 => "Holland",
            17 => "Ireland",
            18 => "Island",
            19 => "Israel",
            20 => "Italy",
            21 => "Lituania",
            22 => "Norway",
            23 => "Poland",
            24 => "Portugal",
            25 => "Romania",
            26 => "Russia",
            27 => "Scotland",
            28 => "Serbia",
            29 => "Slovakia",
            30 => "Slovenia",
            31 => "Spain",
            32 => "Sweden",
            33 => "Switzerland",
            34 => "Ukraine",
            35 => "Juventus",
            36 => "Roma",
            37 => "Napoli",
            38 => "Fiorentina",
            39 => "Inter",
            40 => "Parma",
            41 => "Torino",
            42 => "Milan",
            43 => "Lazio",
            44 => "Verona",
            45 => "Atalanta",
            46 => "Sampdoria",
            47 => "Udinese",
            48 => "Genoa",
            49 => "Cagliari",
            50 => "Chievo",
            51 => "Sassuolo",
            52 => "Catania",
            53 => "Bologna",
            54 => "Livorno"
        );

        $teams2 =array(0 => "Atalanta",
            1  => "Bologna",
            2  => "Cagliari",
            3  => "Catania",
            4  => "Chievo",
            5  => "Fiorentina",
            6  => "Genoa",
            7  => "Inter",
            8  => "Juventus",
            9  => "Lazio",
            10 => "Livorno",
            11 => "Milan",
            12 => "Napoli",
            13 => "Parma",
            14 => "Roma",
            15 => "Sampdoria",
            16 => "Sassuolo",
            17 => "Torino",
            18 => "Udinese",
            19 => "Verona",
            20 => "Algeria",
            21 => "Argentina",
            22 => "Australia",
            23 => "Brasile",
            24 => "Corea",
            25 => "Ecuador",
            26 => "Francia",
            27 => "Germania",
            28 => "Ghana",
            29 => "Inghilterra",
            30 => "Italia",
            31 => "Nigeria",
            32 => "Olanda",
            33 => "Portogallo",
            34 => "Slovacchia",
            35 => "Spagna",
            36 => "Usa",
            37 => "Palermo",
            38 => "Fiorentina",
            39 => "Inter",
            40 => "Parma",
            41 => "Torino",
            42 => "Milan",
            43 => "Lazio",
            44 => "Verona",
            45 => "Atalanta",
            46 => "Sampdoria",
            47 => "Udinese",
            48 => "Genoa",
            49 => "Cagliari",
            50 => "Chievo",
            51 => "Sassuolo",
            52 => "Catania",
            53 => "Bologna",
            54 => "Livorno"
        );


        if(strtotime($this->betTime) < strtotime('2014-08-14')){
            $teams=$teams1;
        }
        else{
            $teams=$teams2;
        }

        foreach($teams as $k=>$v){
            if($team==$k){
                $this->teamName=$v;
            }
        }
        return $this->teamName;
    }

    private function getBetType($bet){
        $this->betType='';
        $betTypes=array(
            0 =>"SingleChance",
            1 =>"DoubleChance",
            2 =>"Under/Over 2.5",
            3 =>"FinalResult",
            4 =>"TotalGoals",
            5 =>"Goals/No Goals",
            6 =>"Under/Over 3.5",
            7 =>"First Half 1x2"
        );
        foreach ($betTypes as $k=>$v){
            if($bet==$k){
                $this->betType=$v;
            }
        }
        return $this->betType;

    }


    function getReadbleTranscript() {

        ?>
        <style>
            #betscontainer  td{
                padding:5px;
            }
            #betscontainer{
                padding: 5px;
            }
        </style>
        <?

/*        6@1!700%516,1,12,1.17&517,1,12,1.09&518,2,U,1.65#false
          6@50-cbc@1!667%516,1,12,1.17&517,1,12,1.09&518,2,U,1.65#false@516,36,33;517,16,18;518,7,10

          6@1!1000%0,2,U,1.52&5,2,U,2.19&1,6,U,1.25&6,2,U,1.75&2,6,O,2.19&7,0,x,4.99#false
          6@51-wui@1!952%0,2,U,1.52&5,2,U,2.19&1,6,U,1.25&6,2,U,1.75&2,6,O,2.19&7,0,x,4.99#false

          6@3!62%0,2,U,1.75&5,2,U,1.52&1,6,U,1.34&6,0,2,1.94;62%0,2,U,1.75&5,2,U,1.52&1,6,U,1.34&6,0,x,4.37;62%0,2,U,1.75&5,2,U,1.52&1,6,U,1.34&6,2,O,2.19;62%0,2,U,1.75&5,2,U,1.52&1,6,U,1.34&6,6,O,4.99;62%0,2,U,1.75&5,0,x,2.69&1,6,U,1.34&6,0,2,1.94;62%0,2,U,1.75&5,0,x,2.69&1,6,U,1.34&6,0,x,4.37;62%0,2,U,1.75&5,0,x,2.69&1,6,U,1.34&6,2,O,2.19;62%0,2,U,1.75&5,0,x,2.69&1,6,U,1.34&6,6,O,4.99;62%0,6,O,3.88&5,2,U,1.52&1,6,U,1.34&6,0,2,1.94;62%0,6,O,3.88&5,2,U,1.52&1,6,U,1.34&6,0,x,4.37;62%0,6,O,3.88&5,2,U,1.52&1,6,U,1.34&6,2,O,2.19;62%0,6,O,3.88&5,2,U,1.52&1,6,U,1.34&6,6,O,4.99;62%0,6,O,3.88&5,0,x,2.69&1,6,U,1.34&6,0,2,1.94;62%0,6,O,3.88&5,0,x,2.69&1,6,U,1.34&6,0,x,4.37;62%0,6,O,3.88&5,0,x,2.69&1,6,U,1.34&6,2,O,2.19;62%0,6,O,3.88&5,0,x,2.69&1,6,U,1.34&6,6,O,4.99#false
          6@52-bud@3!59%0,2,U,1.75&5,2,U,1.52&1,6,U,1.34&6,0,2,1.94;59%0,2,U,1.75&5,2,U,1.52&1,6,U,1.34&6,0,x,4.37;59%0,2,U,1.75&5,2,U,1.52&1,6,U,1.34&6,2,O,2.19;59%0,2,U,1.75&5,2,U,1.52&1,6,U,1.34&6,6,O,4.99;59%0,2,U,1.75&5,0,x,2.69&1,6,U,1.34&6,0,2,1.94;59%0,2,U,1.75&5,0,x,2.69&1,6,U,1.34&6,0,x,4.37;59%0,2,U,1.75&5,0,x,2.69&1,6,U,1.34&6,2,O,2.19;59%0,2,U,1.75&5,0,x,2.69&1,6,U,1.34&6,6,O,4.99;59%0,6,O,3.88&5,2,U,1.52&1,6,U,1.34&6,0,2,1.94;59%0,6,O,3.88&5,2,U,1.52&1,6,U,1.34&6,0,x,4.37;59%0,6,O,3.88&5,2,U,1.52&1,6,U,1.34&6,2,O,2.19;59%0,6,O,3.88&5,2,U,1.52&1,6,U,1.34&6,6,O,4.99;59%0,6,O,3.88&5,0,x,2.69&1,6,U,1.34&6,0,2,1.94;59%0,6,O,3.88&5,0,x,2.69&1,6,U,1.34&6,0,x,4.37;59%0,6,O,3.88&5,0,x,2.69&1,6,U,1.34&6,2,O,2.19;59%0,6,O,3.88&5,0,x,2.69&1,6,U,1.34&6,6,O,4.99#false


          515,OF,1-4,2@516,36,33;517,16,18;518,7,10;519,12,15;520,13,11;521,27,23;522,8,1;523,5,3@0,1,15,1-0;1,30,29,2-2;2,17,5,OF,4-2;3,0,18,1-1;4,22,21,0-3;5,13,16,3-1;6,32,27,OF,3-4;7,9,12,OF,3-4;8,4,19,OF,2-4
          515,OF,1-4,2@516,36,33;517,16,18;518,7,10;519,12,15;520,13,11;521,27,23;522,8,1;523,5,3@0,1,15,1-0;1,30,29,2-2;2,17,5,OF,4-2;3,0,18,1-1;4,22,21,0-3;5,13,16,3-1;6,32,27,OF,3-4;7,9,12,OF,3-4;8,4,19,OF,2-4"

          NULL
          517,OF,4-0,1@518,7,10;519,12,15;520,13,11;521,27,23;522,8,1;523,5,3;524,0,4;525,21,30@0,15,2,0-0;1,31,36,1-0;2,33,27,1-2;3,35,24,3-2;4,25,22,0-0;5,17,19,OF,4-4;6,34,32,3-1;7,8,12,OF,4-2;8,6,10,1-0"

           NULL
           519,2-0,1@520,13,11;521,27,23;522,8,1;523,5,3;524,0,4;525,21,30;526,6,14;527,19,2@0,11,7,1-1;1,10,8,0-1;2,14,19,1-2;3,29,24,OF,4-0;4,21,33,1-1;5,26,31,0-0;6,36,28,1-1;7,4,3,2-1;8,5,17,2-1

  */
        //var_dump($this->resString);

        $parts=explode("@",$this->resString);
        $result = new StringTokenizer ( $this->betString, "@" );
        $nr=$result->countTokens();
        $html='';
            while ($result->hasMoreTokens()) {
                $events=array();
//                if($nr ==2 && $this->resString=='' ||  $nr ==3 && $this->resString!=''){
//
//                    $winning=explode( "-",$this->resultString);
//                    list($win,$etc)=explode("!",$winning[2]);
//                    $id=$result->nextToken();
//                    if($nr==3){
//                    $ticketName=$result->nextToken();
//                    }
//
//                    $bet=$result->nextToken();
//                     $chosenTeams=$result->nextToken();
//                    list($betType,$bet)=explode('!',$bet);
//                    list($bet,$jackpotbet)=explode('#',$bet);
//
//
//                    list($allbetsresults,$gameResults)=explode('@',$this->resultString);
//                    list($tktnm,$allbetsresults)=explode('&',$allbetsresults);
//                    list($allbetsresults,$jackpotwin)=explode('#',$allbetsresults);
//                    $allbetsresults=explode(';',$allbetsresults);
//                    $gameResults=explode(";",$parts[2]);
//                }
//
//                else{
                    $id=$result->nextToken();
                    if($nr==4){
                        $ticketName=$result->nextToken();
                    };

               /* var_dump("resstring==>".$this->resString."<br>");
                var_dump("betstring==>".$this->betString."<br>");
                var_dump("result==>".$this->resultString."<br>");*/

                    $bet=$result->nextToken();
                    $betTeams=$result->nextToken();
                    $betTeams1=explode(';',$betTeams);
                    $cardGames=explode('@',$this->resString);
                    $betTeams2=explode(";",$cardGames[2]);
                    $betTeams=array_merge($betTeams1,$betTeams2);
                    list($betType,$bet)=explode('!',$bet);
                    list($bet,$jackpotbet)=explode('#',$bet);
                    list($allbetsresults,$gameResults)=explode('@',$this->resultString);

                list($tktnm,$allbetsresults)=explode('&',$allbetsresults);
                    list($allbetsresults,$jackpotwin)=explode('#',$allbetsresults);
                    $allbetsresults=explode(';',$allbetsresults);
                    $gameResults1=explode(';',$gameResults);
                    $gameResults2=explode(";",$parts[2]);
                //* modified by Razvan//*
                $erase = "handerased";

                $poserased=strpos($this->betString,$erase);
                $reserased=strpos($this->resultString,$erase);
                if($poserased!==false || $reserased!==false ){
                    $html .= "<h3 style='color:red;text-align: center'>HAND ERASED</h3><br />";
                }
                //* modified by Razvan//*
                $numbets=0;
                $html.='<h3>Ticket type: '.$this->getBetPhase($betType).'</h3>';
                if($jackpotbet!='false'){
                    $html.='<h4 id="jackpot">Bet on jackpot </h4>';
                }


                 $gameResultsArray=array();
                foreach($gameResults2 as $grk=>$grv){


                    list($grgameid,$fteam,$steam,$grgameresult)=explode(',',$grv,4);
                        $gameResultsArray[$grgameid]['gameresult']=$grgameresult;
                }
                foreach($gameResults1 as $grk=>$grv){

                    list($gmid,$grgameid,$grgameresult,$otherResult,$sign,$half)=explode(',',$grv);
                    if($grgameresult=='OF'){
                            $grgameresult.=$otherResult;
                        }
                            $gameResultsArray[$grgameid]['gameresult']=$grgameresult;
                        if(isset($sign)){

                            if((strlen($sign)==1) && isset($half))
                            {
                            $gameResultsArray[$grgameid]['half']=$half;
                            }
                            else{
                                $gameResultsArray[$grgameid]['half']=$sign;
                            }
                        }
                        elseif(strlen($otherResult)==1){
                            $gameResultsArray[$grgameid]['half']=$otherResult;
                        }
                    else
                    {
                        if($sign==NULL && $sign!='' ){
                            $gameResultsArray[$grgameid]['half']=$half;
                        }
                    }
                }

                $allbets=explode(';',$bet);
                foreach($allbets as $ak=>$av){
                    list($betAmount,$bet)=explode('%',$av);
                    $bets=explode('&',$bet);
                    $html.="<table id='betscontainer' ><tr><td class='label'>Ticket id:".$ticketName . "</td><td class='label'>Type: ".$this->getBetPhase($betType)."</td><td class='label'>Game nr. ". ($numbets+1) ."</td><td class='label'>Bet Amount: ". getInDollars($betAmount/100) ."</td><td class='label' colspan='3'>Win:<div id='winning_".$numbets."'>". getInDollars($win/100) ."</div></td></tr>";
                    $html.="<tr><td class='sublabel'>Teams</td><td class='sublabel'>Class</td><td class='sublabel'>Kind</td><td class='sublabel'>Bet Amount</td><td class='sublabel'>Quote</td><td class='sublabel'>First Half </td><td class='sublabel'>Result</td></tr>";
                    foreach($bets as $k=>$v){
                       // var_dump($v);

                        list($eventid,$bet_type,$bet_on,$betquote)=explode(',',$v);
                        $events[$numbets]['result']=$gameResultsArray[$eventid]['gameresult'];
                        $events[$numbets]['id']=$eventid;
                        $events[$numbets]['bet_type']=$this->getBetType($bet_type);
                        $events[$numbets]['bet_on']=$bet_on;
                        $events[$numbets]['quote']=$betquote;
                        //var_dump($betTeams);

                        foreach($betTeams as $btk=>$btv){

                            list($eventbetid,$team1,$team2,$res)=explode(',',$btv);

                            if($eventid==$eventbetid){
                                $events[$numbets]['team1']=$this->getTeam($team1);
                                $events[$numbets]['team2']=$this->getTeam($team2);
                                $events[$numbets]['half']=$gameResultsArray[$eventbetid]['half'];
                                $events[$numbets]['result']=$gameResultsArray[$eventbetid]['gameresult'];


                            }
                        }
                        foreach($allbetsresults as $abk=>$abv){

                            list($betid,$betresult)=explode('-',$abv);
                            if($betid==$numbets){
                                list($playedwin,$betresult)=explode('!',$betresult);
                                /*$betresult=explode("|",$betresult);
                                    $thiswin=0;
                                    foreach($betresult as $nmbk=>$nmbv){
                                        list($predictionid,$thisbetwin)=explode(',',$nmbv);
                                        $thiswin +=$thisbetwin;
                                    }*/
                                if($playedwin>0){
                                    ?>
                                    <script type="text/javascript">
                                        $(function() {
                                            $("#winning_<?=$numbets?>").empty().append('<?=getInDollars($playedwin/100)?>');
                                            $("#winning_<?=$numbets?>").parent().addClass('green');
                                        });
                                    </script>
                                <?
                                }
                                elseif($playedwin==0)
                                {
                                    ?>
                                    <script type="text/javascript">
                                        $(function() {
                                            $("#winning_<?=$numbets?>").empty().append('<?=getInDollars($playedwin/100)?>');
                                            $("#winning_<?=$numbets?>").parent().addClass('contenterror');
                                        });
                                    </script>
                                <?
                                }
                            }
                        }

                        $html .="<tr>
                                <td style='min-width:120px !important;'>".$events[$numbets]['team1']." - ".$events[$numbets]['team2']."</td>
                                <td style='min-width:120px !important;'>".$events[$numbets]['bet_type']."</td>
                                <td style='min-width:70px !important;'>".$events[$numbets]['bet_on']."</td>
                                <td style='min-width:70px !important;'>".getInDollars(($betAmount/count($bets))/100)."</td>
                                <td style='min-width:40px !important;'>".$events[$numbets]['quote']."</td>
                                <td style='min-width:40px !important;'>".$events[$numbets]['half']."</td>
                                <td style='min-width:40px !important;'>".$events[$numbets]['result']."</td>
                             </tr>";
                    }

                    $numbets++;
                    $html.="</table><br>";
                }
                if($jackpotwin!="false" && !is_null($jackpotwin)){
                    list($jackpottrue,$jackpotAmount)=explode('-',$jackpotwin);
                    ?>
                    <script type="text/javascript">
                        $(function() {
                            $("#jackpot").append('Has won the jackpot!!Amount : <?=getInDollars($jackpotAmount/100)?>').addClass('wincard');
                        });
                    </script>
                <?
                }
            }
        return $html;
    }

    private function getBetPhase($betType) {
        $phases = array (
            0 => "Single",
            1 => "Multiple",
            2 => "System",
            3 => "Integral"
        );
        foreach ( $phases as $key => $value ) {
            if ($key == $betType) {
                return $value;
            }
        }
    }
}
?>
