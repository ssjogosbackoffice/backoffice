<?php
define ( "IMG_PATH", "/games/%s/%s/admin_tokens/" );
define ( "PAYOUT_PATH", "/games/%s/%s/" );
class vsoccer_cd extends Transcript {

    function vsoccer_cd() {

    }

    public function getNames() {
        return "vsoccer_cd";
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
            2 =>"UnderOver25",
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

        /*$html='';

         /*resString    =>  55134,3-0@55135,15,30;
                                      55136,31,5;
                                      55137,15,26;
                                      55138,16,14;
                                      55139,33,28;
                                      55140,25,8;
                                      55141,10,21;
                                      55142,0,24;
                                      55143,23,18

         betString    =>  6@
                          759-vvh@
                          0!100%55134,0,1,2.5#true@
                          55134,20,17
         resulString  =>  759-vvh&0-250!0,1#false@
                          12678893,55134,3-0 */
        settype ( $this->resultString, "string" );
     /*   var_dump("resstring==>".$this->resString."<br>");
        var_dump("betstring==>".$this->betString."<br>");
        var_dump("result==>".$this->resultString."<br>");*/
        $result = new StringTokenizer ( $this->betString, "@" );
        $html='';

        while ($result->hasMoreTokens()) {
            $events=array();
            $id=$result->nextToken();
            if($result->countTokens()==4){
            $ticketName=$result->nextToken();
            }
            $bet=$result->nextToken();
            $betTeams=$result->nextToken();
            $betTeams=explode(';',$betTeams);
            list($betType,$bet)=explode('!',$bet);
            list($bet,$jackpot)=explode('#',$bet);
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
            if($jackpot!='false' ){
                $html.='<h4 id="jackpot">Bet on jackpot </h4>';
            }
            $allbets=explode(';',$bet);
            foreach($allbets as $ak=>$av){
                list($betAmount,$bet)=explode('%',$av);
                $bets=explode('&',$bet);


                $html.="<table id='betscontainer' ><tr><td class='label'>Ticket id:".$ticketName . "</td><td class='label'>Type: ".$this->getBetPhase($betType)."</td><td class='label'>Game nr. ". ($numbets+1) ."</td><td class='label'>Bet Amount: ". getInDollars($betAmount/100) ."</td><td class='label' colspan='3'>Win:<div id='winning_".$numbets."'>". getInDollars($win/100) ."</div></td></tr>";
                $html.="<tr><td class='sublabel'>Teams</td><td class='sublabel'>Class</td><td class='sublabel'>Kind</td><td class='sublabel'>Bet Amount</td><td class='sublabel'>Quote</td><td class='sublabel'>First Half Result</td><td class='sublabel'>Result</td></tr>";
                foreach($bets as $k=>$v){
                    list($eventid,$bet_type,$bet_on,$betquote)=explode(',',$v);
                    $events[$numbets]['id']=$eventid;
                    $events[$numbets]['bet_type']=$this->getBetType($bet_type);
                    $events[$numbets]['bet_on']=$bet_on;
                    $events[$numbets]['quote']=$betquote;
                    foreach($betTeams as $btk=>$btv){
                        list($eventbetid,$team1,$team2)=explode(',',$btv);
                        if($eventid==$eventbetid){
                            $events[$numbets]['team1']=$this->getTeam($team1);
                            $events[$numbets]['team2']=$this->getTeam($team2);
                        }
                    }
                    list($allbetsresults,$gameResults)=explode('@',$this->resultString);
                    list($tktnm,$allbetsresults)=explode('&',$allbetsresults);
                    list($allbetsresults,$jackpotwin)=explode('#',$allbetsresults);
                    $allbetsresults=explode(';',$allbetsresults);
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


                    $gameResults=explode(';',$gameResults);
                    foreach($gameResults as $grk=>$grv){
                        $results=explode(',',$grv);
                        $grnrid=$results[0];
                        $grgameid=$results[1];
                        $grgameresult=$results[2];
                        if(strtolower($results[2])=='of'){
                            $grgameresult=$results[3];
                        }
                            $firstHalfResult=$results[count($results)-1];

                        if($eventid==$grgameid){
                            $events[$numbets]['result']=$grgameresult;
                            $events[$numbets]['firstHalfResult']=$firstHalfResult;
                        }
                    }


                  /*  $gameScores=explode('@',$this->resString);
                    $thisGameScore=$gameScores[0];
                    list($grgameid,$grgameresult)=explode(',',$thisGameScore,2);

                    if($eventid==$grgameid){
                        $events[$numbets]['result']=$grgameresult;
                    }*/


                    $html .="<tr>
                                <td>".$events[$numbets]['team1']." - ".$events[$numbets]['team2']."</td>
                                <td>".$events[$numbets]['bet_type']."</td>
                                <td>".$events[$numbets]['bet_on']."</td>
                                <td>".getInDollars($betAmount/100)."</td>
                                <td>".$events[$numbets]['quote']."</td>
                                <td>".$events[$numbets]['firstHalfResult']."</td>
                                <td>".$events[$numbets]['result']."</td>
                             </tr>";
                }
                $numbets++;
                $html.="</table><br>";
            }
            if($jackpotwin!='false' && !is_null($jackpotwin)){
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
