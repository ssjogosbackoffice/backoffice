<?php
define ( "IMG_PATH", "/games/%s/%s/admin_tokens/" );
define ( "PAYOUT_PATH", "/games/%s/%s/" );
define ( "IMG_PATH_vfc", "../media/images/handimages/vfc" );

class vfc extends Transcript {

    function vfc() {

    }

    public function getNames() {
        return "vfc";
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
            0  => "OvinceSaintPreux",
            1  => "RyanJimmo",
            2  => "AndreiArlovski",
            3  => "BrendanSchaub",
            4  => "RyanBader",
            5  => "RafaelFeijad",
            6  => "DannyCastillo",
            7  => "CharlieBrenneman",
            8  => "JonJones",
            9  => "GloverTeixeira",
            10  => "RashadEvans",
            11	=> "DanHenderson",
            12	=> "AlexisDavis",
            13	=> "RosiSexton",
            14	=> "RoyNelson",
            15	=> "StipeMiocic",
            16	=> "IgorPokrajac",
            17	=> "YvesJabouin",
            18	=> "DustinPague",
            19	=> "PhilDavis",
            20	=> "AnthonyJohnson",
            21	=> "MaxHolloway",
            22	=> "AndreFili",
            23	=> "JimMiller",
            24	=> "YancyMedeiros",
            25	=> "EddieWineland",
            26	=> "WillCampuzano",
            27	=> "GeorgesStPierre",
            28	=> "JohnyHendricks",
            29	=> "DennisSilver",
            30	=> "NateMohr",
            31	=> "JoshKoscheck",
            32	=> "TyronWoodley",
            33	=> "ChaelSonnen",
            34	=> "RoryMacDonald",
            35	=> "RobbieLawler",
            36	=> "TimElliot",
            37	=> "AliBagautinov",
            38	=> "ShogunRua",
            39	=> "AlanBelcher",
            40	=> "BobbyGreen",
            41	=> "ChicoCamus",
            42	=> "DustinKimura",
            43	=> "EvanDunham",
            44	=> "GleisonTibau",
            45	=> "IsaacVallieFlagg",
            46	=> "JacobVolkmann",
            47	=> "KurtHolobaugh",
            48	=> "MichaelBisping",
            49	=> "PatHealy",
            50	=> "ThalesLeites",
            51	=> "StevenSiler",
            52	=> "VinnyMagalhaes",
            53	=> "YvesEdwards",
            54	=> "DemtriousJohnson",
            55	=> "TomWatson",
            56	=> "JohnLineker",
            57	=> "JoseMaria",
            58	=> "JoseAldo",
            59	=> "KoreanZombie",
            60	=> "LyotoMachida",
            61	=> "ErikKoch",
            62	=> "DustinPoirier",
            63	=> "BenRothwell",
            64	=> "BrandonVera",
            65	=> "ChadMendes",
            66	=> "ClayGuida",
            67	=> "BensonHenderson",
            68	=> "AnthonyPettis",
            69	=> "CarlosCondit",
            70	=> "DiegoSanchez",
            71	=> "MylesJury",
            72	=> "JakeShields",
            73	=> "HectorLombard",
            74	=> "ChrisWeidman",
            75	=> "AndersonSilva",
            76	=> "DiegoBrandao",
            77	=> "MichaelJohnson",
            78	=> "FabricioCamoes",
            79	=> "RicardoLamas",
            80	=> "DennisBermudez",
            81	=> "RondaRousey",
            82	=> "MieshaTate",
            83	=> "WilliamPatolino",
            84	=> "BobbyVoelker",
            85	=> "AlIaquinta",
            86	=> "MitchClarke",
            87	=> "ChrisHoldsworth",
            88	=> "NickDiaz",
            89	=> "MichaelChiesa",
            90	=> "FranciscoTrinaldo",
            91	=> "MikeRicci",
            92	=> "ColinFletcher",
            93	=> "NickRing",
            94	=> "ChrisCamozzi",
            95	=> "TonyFerguson",
            96	=> "KatsunoriKikuno",
            97	=> "CodyGibson",
            98	=> "AljamainSterling",
            99	=> "DanielCormier",
            100	=> "PatrickCummins",
            101	=> "RobertWhittaker",
            102	=> "StephenThompson",
            103	=> "SaraMcMann",
            104	=> "DemianMaia",
            105	=> "TjWaldburger",
            106	=> "MikePyle",
            107	=> "MichelPrazeres",
            108	=> "JesseRonson",
            109	=> "NandorGuelmino",
            110	=> "DanielOmielanczuk",
            111	=> "RolandDelorme",
            112	=> "AlexCaceres",
            113	=> "CainVelasquez",
            114	=> "JuniorDosSantos",
            115	=> "BrianBowles",
            116	=> "GeorgeRoop",
            117	=> "BigfootSilva",
            118	=> "ColtonSmith",
            119	=> "DennisBermudez",
            120	=> "DonaldCerrone",
            121	=> "KjNoons",
            122	=> "JamesTeHuna",
            123	=> "GrayMaynard",
            124	=> "TjGrant",
            125	=> "MarkHunt",
            126	=> "ClintHester",
            127	=> "AndyEnz",
            128	=> "NeilMagny",
            129	=> "GasanUmalatov",
            130	=> "TonyMartin",
            131	=> "RashidMagomedov",
            132	=> "MarkBocek",
            133	=> "RafaelDosAnjos",
            134	=> "FrankieEdgar",
            135	=> "CharlesOliveira",
            136	=> "TimKennedy",
            137	=> "RogerGracie",
            138	=> "MarkMunoz",
            139	=> "TimBoetsch",
            140	=> "CubSwanson",
            141	=> "MarkHominick",
            142	=> "PabloGarza",
            143	=> "FrancisCarmont",
            144	=> "TomLawlor",
            145	=> "BradPickett",
            146	=> "ErikPerez",
            147	=> "ByronBloodworth",
            148	=> "MelvinGuillard",
            149	=> "JamieVarner",
            150	=> "RobFont",
            151	=> "MarcusBrimage",
            152	=> "RussellDoane",
            153	=> "UriahHall",
            154	=> "ThiagoSantos",
            155	=> "UrijahFaber",
            156	=> "PaigeVanZant",
            157	=> "KailinCurran",
            158	=> "ChrisLeben"
        );

        $teams2 =array(
            0  => "OvinceSaintPreux",
            1  => "RyanJimmo",
            2  => "AndreiArlovski",
            3  => "BrendanSchaub",
            4  => "RyanBader",
            5  => "RafaelFeijad",
            6  => "DannyCastillo",
            7  => "CharlieBrenneman",
            8  => "JonJones",
            9  => "GloverTeixeira",
            10 => "	RashadEvans",
            11	=> "DanHenderson",
            12	=> "AlexisDavis",
            13	=> "RosiSexton",
            14	=> "RoyNelson",
            15	=> "StipeMiocic",
            16	=> "IgorPokrajac",
            17	=> "YvesJabouin",
            18	=> "DustinPague",
            19	=> "PhilDavis",
            20	=> "AnthonyJohnson",
            21	=> "MaxHolloway",
            22	=> "AndreFili",
            23	=> "JimMiller",
            24	=> "YancyMedeiros",
            25	=> "EddieWineland",
            26	=> "WillCampuzano",
            27	=> "GeorgesStPierre",
            28	=> "JohnyHendricks",
            29	=> "DennisSilver",
            30	=> "NateMohr",
            31	=> "JoshKoscheck",
            32	=> "TyronWoodley",
            33	=> "ChaelSonnen",
            34	=> "RoryMacDonald",
            35	=> "RobbieLawler",
            36	=> "TimElliot",
            37	=> "AliBagautinov",
            38	=> "ShogunRua",
            39	=> "AlanBelcher",
            40	=> "BobbyGreen",
            41	=> "ChicoCamus",
            42	=> "DustinKimura",
            43	=> "EvanDunham",
            44	=> "GleisonTibau",
            45	=> "IsaacVallieFlagg",
            46	=> "JacobVolkmann",
            47	=> "KurtHolobaugh",
            48	=> "MichaelBisping",
            49	=> "PatHealy",
            50	=> "ThalesLeites",
            51	=> "StevenSiler",
            52	=> "VinnyMagalhaes",
            53	=> "YvesEdwards",
            54	=> "DemtriousJohnson",
            55	=> "TomWatson",
            56	=> "JohnLineker",
            57	=> "JoseMaria",
            58	=> "JoseAldo",
            59	=> "KoreanZombie",
            60	=> "LyotoMachida",
            61	=> "ErikKoch",
            62	=> "DustinPoirier",
            63	=> "BenRothwell",
            64	=> "BrandonVera",
            65	=> "ChadMendes",
            66	=> "ClayGuida",
            67	=> "BensonHenderson",
            68	=> "AnthonyPettis",
            69	=> "CarlosCondit",
            70	=> "DiegoSanchez",
            71	=> "MylesJury",
            72	=> "JakeShields",
            73	=> "HectorLombard",
            74	=> "ChrisWeidman",
            75	=> "AndersonSilva",
            76	=> "DiegoBrandao",
            77	=> "MichaelJohnson",
            78	=> "FabricioCamoes",
            79	=> "RicardoLamas",
            80	=> "DennisBermudez",
            81	=> "RondaRousey",
            82	=> "MieshaTate",
            83	=> "WilliamPatolino",
            84	=> "BobbyVoelker",
            85	=> "AlIaquinta",
            86	=> "MitchClarke",
            87	=> "ChrisHoldsworth",
            88	=> "NickDiaz",
            89	=> "MichaelChiesa",
            90	=> "FranciscoTrinaldo",
            91	=> "MikeRicci",
            92	=> "ColinFletcher",
            93	=> "NickRing",
            94	=> "ChrisCamozzi",
            95	=> "TonyFerguson",
            96	=> "KatsunoriKikuno",
            97	=> "CodyGibson",
            98	=> "AljamainSterling",
            99	=> "DanielCormier",
            100	=> "PatrickCummins",
            101	=> "RobertWhittaker",
            102	=> "StephenThompson",
            103	=> "SaraMcMann",
            104	=> "DemianMaia",
            105	=> "TjWaldburger",
            106	=> "MikePyle",
            107	=> "MichelPrazeres",
            108	=> "JesseRonson",
            109	=> "NandorGuelmino",
            110	=> "DanielOmielanczuk",
            111	=> "RolandDelorme",
            112	=> "AlexCaceres",
            113	=> "CainVelasquez",
            114	=> "JuniorDosSantos",
            115	=> "BrianBowles",
            116	=> "GeorgeRoop",
            117	=> "BigfootSilva",
            118	=> "ColtonSmith",
            119	=> "DennisBermudez",
            120	=> "DonaldCerrone",
            121	=> "KjNoons",
            122	=> "JamesTeHuna",
            123	=> "GrayMaynard",
            124	=> "TjGrant",
            125	=> "MarkHunt",
            126	=> "ClintHester",
            127	=> "AndyEnz",
            128	=> "NeilMagny",
            129	=> "GasanUmalatov",
            130	=> "TonyMartin",
            131	=> "RashidMagomedov",
            132	=> "MarkBocek",
            133	=> "RafaelDosAnjos",
            134	=> "FrankieEdgar",
            135	=> "CharlesOliveira",
            136	=> "TimKennedy",
            137	=> "RogerGracie",
            138	=> "MarkMunoz",
            139	=> "TimBoetsch",
            140	=> "CubSwanson",
            141	=> "MarkHominick",
            142	=> "PabloGarza",
            143	=> "FrancisCarmont",
            144	=> "TomLawlor",
            145	=> "BradPickett",
            146	=> "ErikPerez",
            147	=> "ByronBloodworth",
            148	=> "MelvinGuillard",
            149	=> "JamieVarner",
            150	=> "RobFont",
            151	=> "MarcusBrimage",
            152	=> "RussellDoane",
            153	=> "UriahHall",
            154	=> "ThiagoSantos",
            155	=> "UrijahFaber",
            156	=> "PaigeVanZant",
            157	=> "KailinCurran",
            158	=> "ChrisLeben"
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
            0 =>"Match Winner",

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
        $parts=explode("%",$this->resString);
        $result = new StringTokenizer ( $this->betString, "!" );
        $html='';
        if(strlen($parts[0])!=""){

            while ($result->hasMoreTokens()) {
            $events=array();
            $bet=$result->nextToken();
            list($idk,$ticketName,$betType)=explode('@',$bet);
            $betTeams=$result->nextToken();
            list($bet,$jackpotbet)=explode('#',$betTeams);

            list($allbetsresults,$gameResults)=explode('@',$this->resultString);
            list($tktnm,$allbetsresults)=explode('&',$allbetsresults);
            list($allbetsresults,$jackpotwin)=explode('#',$allbetsresults);
            $allbetsresults=explode(';',$allbetsresults);


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
            $allbets=explode(';',$bet);
            foreach($allbets as $ak => $av){
                list($betAmount,$curentBet)=explode('%',$av);
                $bets=explode('&',$curentBet);
                $html.="<div class='centered'><table id='betscontainer' ><tr><td class='label'>Ticket id:".$ticketName . "</td><td class='label'>Type: ".$this->getBetPhase($betType)."</td><td class='label'>Game nr. ". ($numbets+1) ."</td><td class='label'>Bet Amount: ". getInDollars($betAmount/100) ."</td><td class='label' colspan='3'>Win:<div id='winning_".$numbets."'>". getInDollars($win/100) ."</div></td></tr>";
                $html.="<tr><td class='sublabel'>Players</td><td class='sublabel'>Class</td><td class='sublabel'>Bet on player</td><td class='sublabel'>Bet Amount</td><td class='sublabel'>Quote</td><td class='sublabel'>Winner</td></tr>";
                foreach($bets as $k=>$v){
                    list($eventTeams,$bet_type,$bet_on,$betquote)=explode(',',$v);
                    list($fighter1,$fighter2)=explode('@',$eventTeams);
                    $events[$numbets]['bet_type']=$this->getBetType($bet_type);
                    $events[$numbets]['bet_on']=$bet_on;
                    $events[$numbets]['quote']=$betquote;
                    foreach($parts as $btk => $btv){
                        $players=explode(";",$btv);
                        foreach($players  as $bet1 => $single){
                            list($eventbetid,$teamsAll,$winner)=explode(',',$single);
                            if($teamsAll==$eventTeams){
                            $events[$numbets]['img1']=$fighter1;
                            $events[$numbets]['img2']=$fighter2;
                                $fighter1=preg_split('/(?=[A-Z])/',$this->getTeam($fighter1));
                                $fighter2=preg_split('/(?=[A-Z])/',$this->getTeam($fighter2));

                            $events[$numbets]['team1']=$fighter1[1]." ".$fighter1[2]." ".$fighter1[3]." ".$fighter1[4];
                            $events[$numbets]['team2']=$fighter2[1]." ".$fighter2[2]." ".$fighter2[3]." ".$fighter2[4];
                            $events[$numbets]['winner']=$winner;
                            }

                        }
                    }
                    foreach($allbetsresults as $abk=>$abv){
                        list($betid,$betresult)=explode('-',$abv);
                        if($betid==$numbets){
                            list($playedwin,$betresult)=explode('!',$betresult);
                            $betresult=explode("|",$betresult);
                            $thiswin=0;
                            foreach($betresult as $nmbk=>$nmbv){
                                list($predictionid,$thisbetwin)=explode(',',$nmbv);
                                $thiswin +=$thisbetwin;
                            }
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
                                <td style='min-width:120px !important;'><div class='vfc_team1_bg".($events[$numbets]['winner']==1? " border_green" : '' )."'><div class='vfc_team1'><img src=".IMG_PATH_vfc."/fighter_img_".str_pad($events[$numbets]['img1'],3,'0',STR_PAD_LEFT).".png><div class='teamNames'>". $events[$numbets]['team1']."</div></div></div><div class='versus'></div><div class='vfc_team2_bg".($events[$numbets]['winner']==2? " border_green" : '' )."'><div class='vfc_team2'><img src='".IMG_PATH_vfc."/fighter_img_".str_pad($events[$numbets]['img2'],3,'0',STR_PAD_LEFT) .".png'><div class='teamNames'>". $events[$numbets]['team2']."</div></div></div></td>
                                <td style='min-width:120px !important;'>".$events[$numbets]['bet_type']."</td>
                                <td style='min-width:70px !important;'>".$events[$numbets]['bet_on']."</td>
                                <td style='min-width:70px !important;'>".getInDollars(($betAmount/count($bets))/100)."</td>
                                <td style='min-width:40px !important;'>".$events[$numbets]['quote']."</td>
                                <td style='min-width:40px !important;'>".$events[$numbets]['winner']."</td>
                             </tr>";
                }

                $numbets++;
                $numbets++;
                $html.="</table></div><br>";
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
        }
        else{
            $html .="<div  class='centered'><h1 class='red'>Game in Progress! Waiting for Result</h1></div>";

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
