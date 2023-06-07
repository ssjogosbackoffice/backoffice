<?php
define ( "IMG_PATH", "/games/%s/%s/admin_tokens/" );
define ( "PAYOUT_PATH", "/games/%s/%s/" );
define ( "IMG_PATH_mega_lotto", "../media/images/handimages/mega_lotto" );

class mega_lotto extends Transcript {

    function mega_lotto() {

    }

    public function getNames() {
        return "mega_lotto";
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
        $html='';
        $tax =getInDollars($this->taxString);
        $erase = "handerased";
        $poserased=strpos($this->betString,$erase);
        $reserased=strpos($this->resultString,$erase);
        if($poserased!==false || $reserased!==false ){
            $html .= "<h3 style='color:red;text-align: center'>HAND ERASED</h3><br />";
        }
        else{
        settype ( $this->resultString, "string" );
        $result = new StringTokenizer ( $this->betString, "@" );
        list($cCode,$countryName,$lotteryType,$lotteryCode,$lotteryName,$extractionDate,$lottoResult)=explode(';',$this->resString);
        $lName=explode('-',$lotteryName);
        $resBalls=explode(',',$lottoResult);
        $winResults=explode("|",$this->resultString);
        $winningBallsAll=end($winResults);
            $winningBalls=explode(";",$winningBallsAll);
            $winningBalls=$winningBalls[0];

        $html .="<center><div class='container " . $this->path . "' style='width:800px'><div class='centered'><table style='text-align:center;float:left;' rules=rows cellspacing='2' cellpadding='4'>";
        //$html .="<tr><td colspan='7'class='bold'>Detailed ticket</td></tr><tr class='formheading'>";
        $html .="<td class='label'>Lottery Name</td><td class='label'>Bet Type</td><td class='label'>Extraction Date </td></tr>";
        $html .="<tr><td>".$lName[0]."</td><td>".$lName[1]."</td><td>".$extractionDate."</td></tr></table>";
        $html .="<div style='float:right;width:460px';><table class='mega_lottoResults' style='text-align:center;' rules=rows cellspacing='2' cellpadding='4'><tr class='formheading'><td class='label'colspan=' ".(count($resBalls))." '>Extracted Balls </td></tr></table>";
        $html .="<div class='BallsBetOn'>";
        $winningBall=explode(',',$winningBalls);
        if($lottoResult!=NULL){
        foreach($resBalls as $key => $ball){
            $html .="<div class='lottoBallResult'>".$ball."</div>";
            if($key==9){
            }
        }
        }
        else{
            $html .="<h2 class='red'> Waiting for Result </h2>";
        }
        $html .="</div></div><div class='clearfix'></div>";
        while ($result->hasMoreTokens()) {
            $id=$result->nextToken();
            $all=$result->nextToken();
            $allBets=explode('|',$all);
            $ticketId=$result->nextToken();
            $betAmount=$allBets[3];
            $betSistem=$allBets[4];
            $betCombinations=$allBets[5];
            $sistems=explode(';',$betCombinations);
            $html .="<table><tr><td class='bold'>Ticket id:</td><td>".$ticketId."</td><tr><td class='bold'>System:</td><td>".$betSistem."</td></tr><tr><td class='bold'>Total Combinations :</td><td>".$allBets[6]." </td></tr><td class='bold'>Net Bet :</td><td>".getInDollars($allBets[3]/ 100)."</td></tr><tr><td class='bold'>Bet Tax :</td><td>".$tax."</td></tr></table><div clas='clearfix'></div>";
            $html .="<table style='text-align:center;float:left;' rules=rows cellspacing='2' cellpadding='4'><tr class='formheading'>";
            $html .="<td class='label'>Balls</td><td class='label'>Quote</td><td class='label'>Poss. Win</td>";
            foreach($sistems as $key => $v){
                list($sistem,$ballBet,$ballsQuote,$possWin,$etc)=explode(':',$v);
                   $class='';
                $html .="<tr>";
                if(count($winningBall)==(substr($sistem,1)) && $winningBall[0]!=''){
                    $class=' class="blink"';
                }
                $html.="<td $class>".(substr($sistem,1))."</td>
                             <td $class>".$ballsQuote."</td>
                             <td $class>".getInDollars($possWin)."</td></td>";
            }
            $html .="</table>";
            $betNumbers=explode(',',$allBets[2]);
            $html .="<div style='float:right;width:460px'><table class='mega_lottoResults' style='text-align:center;float:left;' rules=rows cellspacing='2' cellpadding='4'><tr class='formheading'><td class='label'colspan=' ".(count($resBalls))." '>BET ON   : ".$winResults[0]."</td></tr></table>";
            $html .="<div class='BallsBetOn'>";
            foreach($betNumbers as $key => $ball){
                $html .="<div class='";
                if(in_array($ball,$winningBall) ){
                    $html .="lottoBallWin";
                }
                else{
                    $html .="lottoBallResult";
                }
                $html .="'>".$ball."</div>";
                if($key==9){
                    $html .="";
                }
            }
            $html .="</div></div>";
            $bet=$result->nextToken();
            $betTeams=$result->nextToken();
            $betTeams=explode(';',$betTeams);
            list($betType,$bet)=explode('!',$bet);
            list($bet,$jackpot)=explode('#',$bet);
            //* modified by Razvan//*


            $html .="</center>";

            $allbets=explode(';',$bet);
            foreach($allbets as $ak=>$av){
                list($betAmount,$bet)=explode('%',$av);
                $bets=explode('&',$bet);
            }
        };
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
