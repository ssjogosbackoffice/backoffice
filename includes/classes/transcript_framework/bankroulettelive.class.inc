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

class bankroulette extends Transcript
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

    public $playersArr=array();

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
        "10002" => "RED",
        '3017'	 => 'Terzina01',
        '3047'	 => 'Terzina02',
        '3077'	 => 'Terzina03',
        '3107'	 => 'Terzina04',
        '3137'	 => 'Terzina05',
        '3167'	 => 'Terzina06',
        '3197'	 => 'Terzina07',
        '3227'	 => 'Terzina08',
        '3257'	 => 'Terzina09',
        '3287'   => 'Terzina10',
        '3317'	 =>'Terzina11',
        '3347'	 => 'Terzina12',
        '5016'	 => 'Sestina01',
        '5046'	 => 'Sestina02',
        '5076'   => 'Sestina03',
        '5106'	 => 'Sestina04',
        '5136'	 => 'Sestina05',
        '5166'	 => 'Sestina06',
        '5196'	 => 'Sestina07',
        '5226'   => 'Sestina08',
        '5256'	 => 'Sestina09',
        '5286'	 => 'Sestina10',
        '5316'	 => 'Sestina11',
        '6014'   => '1,2,3,4,5,6',
        '6044'   => '4, 5, 6, 7, 8, 9',
        '6074'   => '7, 8, 9, 10, 11, 12',
        '6104'   => '10, 11, 12, 13, 14, 15',
        '6134'   => '13, 14, 15, 16, 17, 18',
        '6164'   => '16, 17, 18, 19, 20, 21',
        '6194'   => '19, 20, 21, 22, 23, 24',
        '6224'   => '22, 23, 24, 25, 26, 27',
        '6254'   => '25, 26, 27, 28, 29, 30',
        '6284'   => '28, 29, 30, 31, 32, 33',
        '6314'   => '31, 32, 33, 34, 35, 36',
        '3022'   => '0, 2, 3',
        '1359'   => '0, 1, 2',
        '3013'   => '1, 2, 3',
        '3043'   => '4, 5, 6',
        '3073'   => '7, 8, 9',
        '3103'   => '10, 11, 12',
        '3133'   => '13, 14, 15',
        '3163'   => '16, 17, 18',
        '3193'   => '19, 20, 21',
        '3223'   => '22, 23, 24',
        '3253'   => '25, 26, 27',
        '3283'   => '28, 29, 30',
        '3313'   => '31, 32, 33',
        '3343'   => '34, 35, 36',
    );
    function bankroulette(){

    }

    function getBet() {

        /*	   print_r("betstring=>".$this->betString."<br />");
                 print_r("resstring=>".$this->resString."<br />");
                print_r("resultString=>".$this->resultString."<br />");*/
        list($handDetails,$betresult)=explode('&',$this->resultString);
        $betresult = new StringTokenizer ( $betresult, ";" );
        $resultbets = array ();
        $totalwin = 0;
        while ( $betresults = $betresult->nextToken () ) {

            list ( $betids, $betwin ) = explode ( "-", $betresults );
            array_push ( $resultbets, array ("betid" => $betids, "betwin" => $betwin ) );
            $totalwin  += $betwin;
        }

        $this->betString = explode ( "@", $this->betString );
        if(count($this->betString)>1){
            $result = new StringTokenizer ( $this->betString[1], ";" );
            $blind=$this->betString[2];
            $count = $result->countTokens ();
        }
        else{
            $result='';
            $blind=$this->betString[0];
        }
        $i = 0;
        $totalbets = "0";
        $html="";
        if($blind!=''){
            $blind=explode('#',$blind);
            list($blindAmount,$blindPercentage)=explode(';',$blind[1]);
            $html .="<table  cellspacing='10' cellpadding='4' rules=rows><tr><td style='background-color:#4A7EB6;color:white;' >".$blind[0]."</td><td>".getInDollars($blind[1]/100)."</td></tr>";
            if(isset($blindPercentage)){
                $html.="<tr><td style='background-color:#4A7EB6;color:white;' >Banked percent</td><td>".$blindPercentage."%</td></tr>";
            }
            if(stripos($handDetails,'BankerBalance')!==false){
                list($banker,$bankerWinAmount)=explode('#',$handDetails);
                $bankerWinAmount=explode(';',$bankerWinAmount);
                $html.="<tr><td style='background-color:#4A7EB6;color:white;' >Win</td><td>".getInDollars($bankerWinAmount[0]/100)."</td></tr>";
                $html.="<tr><td style='background-color:#4A7EB6;color:white;' >Total players bet</td><td>".getInDollars($bankerWinAmount[1]/100)."</td></tr>";
                $html.="<tr><td style='background-color:#4A7EB6;color:white;' >Total players win</td><td>".getInDollars($bankerWinAmount[2]/100)."</td></tr>";

            }
            $html.="</table></br />";
        }
        if($result!=''){
            $html.="<div style='float:left;'><table  cellspacing='10' cellpadding='4' rules=rows><tr ><td style='background-color:#4A7EB6;color:white;' >Bet on</td><td style='background-color:#4A7EB6;color:white;'>&nbsp;&nbsp;Bet</td><td style='background-color:#4A7EB6;color:white;'>Win</td>";
            while ( $bets = $result->nextToken () ) {
                if ($i < ($count)) {
                    list ( $betnumbers,  $value ) = explode ( "~", $bets );
                    $code = $betnumbers;
                    $totalbets = $totalbets + $value;
                    $type = $code - ($code % 1000);

                    $numbbet = floor ( ($code % 1000) / 10 );
                    $side = ($code % 1000) % 10;
                    $betString = "";
                    if(isset($this->GROUPS [$code])){
                        $betString = $this->GROUPS [$code];
                    }else{
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
                    }
                    $html .="<tr>";
                    $class = "";
                    foreach ( $resultbets as $k => $v ) {
                        if ($betnumbers == $v ["betid"]) {
                            if($v ["betwin"]!= 0){
                                $class =" style='background-color:green;'";
                            }
                        }
                    }
                    $html.="<td $class>" . $betString . "</td><td $class>&nbsp;&nbsp;" . ($value / 100) . "&#8364;</td><td $class>";
                    foreach ( $resultbets as $k => $v ) {
                        if ($betnumbers == $v ["betid"]) {
                            $html .=($v ["betwin"] / 100) . "&#8364;</td></tr> ";
                        }
                    }
                }
            }
            $html .= "</table></div>";
        }

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
                }else{
                    $bet = "0," . $this->GRAMBLING[$grbPos][$pos];
                }
                break;
            case TOP:
                $bet = $this->GRAMBLING[$grbPos][$pos] . "," . $this->GRAMBLING[$grbPos][$pos + 1];
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
        $resultString=explode('@',$this->resString);
        $this->playersArr=explode(';',$resultString[0]);

        list($number, $color) = split("\\|", $resultString[1]);
        return "<div align='center' style='font-weight:bold; font-size:large;color:green' >Winning number:<br/> <br /><img src='".ROULETTE_PATH."".trim($number).".jpg' style='  border: 2px solid rgb(7, 7, 7);border-radius: 5px;box-shadow: 0px 0px 17px 3px rgb(0, 0, 0);' /></div>
    	<br /><br />".$this->createTableView();
    }

    private function createTableView(){
        globalise('gmn');
        global $hand_id,$gmn;
        $html="<div class='label' style='padding:2px'>Hand details</div><br /><br />
                <div id='multiTableContainer' class='centered'>  ";
        $html.="<div id='multiTableContainerEllipse' class='centered'>  ";

        foreach($this->playersArr as $k=>$v){
            $v=explode(',',$v);
            $html.="<div>";
            if($v[3]!='-1'){
                $html.="<div class='multiTableUserInfo' data-user='".$v[1]."'>";
                if($v[5]!=-1){
                    $html.="<div class='bankerDealer fleft'>B </div>";
                    $html.="<div class='multiTableUsernameContainer'>".$v['3']."</div><span class='tip'>(Banker)</span>";
                }
                else{
                    $html.="<div class='multiTableUsernameContainer'>".$v['3']."</div>";
                }
                $html.="
        <br /><div style='  font-size: 10px;color: rgb(7, 7, 7);'>Credit:".getInDollars($v[4]/100)."</div>
        </div>";
            }

            $html.="</div>";
        }
        $html.="</div></div>";
        $html .="

       <script>

       function getUserHand(user,hand,gmn){
               $.post('/services/general_services.inc',{action:'getUserHandDetails',
                user:user,
                hand:hand,
                gmn:gmn
            }, function(data){
             $.fancybox(data);
            });

       }
       $(document).ready(function () {
       $('.multiTableUserInfo').on('click',function(){

         getUserHand($(this).data('user'),".$hand_id.",".$gmn.");
       });
        var radius = 240;
        var fields = $('#multiTableContainerEllipse>div');

        container = $('#multiTableContainerEllipse'),
        width = container.width(),
        height = container.height(),
        angle = 0,
        step = (2*Math.PI) / fields.length;

        currStep=0;

        fields.each(function() {
        diff=0;
        if(currStep==0 || currStep==1 || currStep==fields.length-1){
        diff=-55;
        if(currStep==0){
        diff=-85;
        }
        }
        if(currStep==4 || currStep==5 || currStep==6){
        diff=70;
        if(currStep==5){
        diff=95;
        }

        }

            var x = Math.round(width/2 + radius * Math.cos(angle) - $(this).width()/2 +diff);
            var y = Math.round(height/2 + radius * Math.sin(angle) - $(this).height()/2 );
            $(this).css({
                left: x + 'px',
                top: y + 'px'
            });
            angle += step;
            currStep++;
        });
        });
        </script>
        ";
        return $html;

    }
}


?>
