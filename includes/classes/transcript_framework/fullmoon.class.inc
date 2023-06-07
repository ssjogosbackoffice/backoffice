<script type="text/javascript">
    function showWin( symbols_pos,set){
        blinking_pos     = symbols_pos.split("|");
        for(var i = 0 ; i < blinking_pos.length ; i++){
            $("#symbol_" + blinking_pos[i],'#'+set).addClass("blink_line");
        }
    };
</script>
<?php
define ( "IMG_PATH_fullmoon", "../media/images/handimages/fullmoon" );
define ( "PAYOUT_PATH_fullmoon", "../media/images/handimages/fullmoon" );
require_once 'Transcript.class.inc';
require_once 'transcript.inc.php';
class fullmoon extends Transcript {
    function getResultImage($img_path , $symbol_id){

        $img_path = sprintf($img_path , $this->getNames(), $this->path);
        $str = "<img src='".$img_path."/sym_".$symbol_id. ".png' width=40px height=40px />";
        return $str;
    }

    function fullmoon() {
    }
    public function getNames() {
        return "fullmoon";
    }
    public function getBet() {
    }
    function str_lreplace($search, $replace, $subject) {
        $pos = strrpos ( $subject, $search );

        if ($pos !== false) {
            $subject = substr_replace ( $subject, $replace, $pos, strlen ( $search ) );
        }

        return $subject;
    }
    public function getReadbleTranscript() {
        /*
         * 6!25!1-3,3;5-4,5;7-1,2# 6!0!2-4,1;6-1,3;8-6,4# 6!1!0-5,1;3-2,5;1-3,3#
         * 6!2!1-2,2;2-1,4;4-5,3# 6!1!5-4,5;3-3,4;0-2,4# 6!2!4-5,3;1-2,2;1-6,6#
         * 6!0!3-3,4;7-1,2;7-2,1# 6!2!8-4,6;6-1,3;2-3,2@2-0@false#
         * 6!1!1-5,5;3-6,1;2-3,2@1-20@false&4~0|4|8~20# 6!0!0-0@false"
         */
        settype ( $this->betString, "string" );
        $result = new StringTokenizer ( $this->betString, "#" );
        $count = $result->countTokens ();
        $i = 0;
        $handList = array ();
        $html = "<div class='container " . $this->path . "'>";
        $html .= "<div class='box " . $this->path . "'>";
        $totalpoints = 0;
        $totalbonus = 0;
        $setpoints = 0;
        $firstset = '<div id="0" style="margin-left:120px"><div>';
        $firstsetpoints = 0;
        $secondset = '<div id="1"><div>';
        $secondesetpoints = 0;
        $thirdset = '<div id="2"><div>';
        $thirdsetpoints = 0;
        $fourthset = '<div id="3"><div>';
        $fourthsetpoints = 0;
        while ( ($hand = $result->nextToken ()) ) {
            $linewin = 0;
            if ($i < ($count - 1)) {
                $pos = strpos ( $hand, "@" );
            }
            if ($pos === false) {
                $tiles = split ( "!", $hand );
                $tileset = $tiles ['1'];
                $tiles = $tiles ['2'];

            } else {
                if ($i < ($count - 1)) {
                    list ( $bets, $win1, $results ) = explode ( "@", $hand );
                } else {
                    list ( $bets, $results ) = explode ( "@", $hand );
                }
                $tiles = split ( "!", $bets );
                $tileset = $tiles ['1'];
                $tiles = $tiles ['2'];
                list ( $bonus, $WinLine ) = explode ( "&", $results );
                if ($bonus != 'false') {
                    $bonus = substr ( $bonus, 4 );
                    $totalbonus = $totalbonus + $bonus;
                }
                $wininglines = explode ( ";", $WinLine );
                foreach ( $wininglines as $key => $value ) {
                    list ( $lineid, $linesymbols, $thiswin ) = explode ( "~", $value );
                    $linesymbols = explode ( "|", $linesymbols );
                    foreach ( $linesymbols as $k => $v ) {
                        $linesymbols [$k] = $v + 1;
                    }
                    $linesymbols = implode ( "|", $linesymbols );
                    if ($thiswin != '') {
                        ?>
                        <script>
                            $(document).ready(function(){
                                showWin('<?=$linesymbols?>','<?=$tileset?>');
                            });
                        </script>
                    <?
                    }
                    $linewin = $linewin + $thiswin;
                }
            }

            if ($i > 0) {
                switch ($tileset) {
                    case 0 :
                        $firstset .= $currhtml;
                        $firstsetpoints = $firstsetpoints + $linewin;
                        break;
                    case 1 :
                        $secondset .= $currhtml;
                        $secondesetpoints = $secondesetpoints + $linewin;
                        break;
                    case 2 :
                        $thirdset .= $currhtml;
                        $thirdsetpoints = $thirdsetpoints + $linewin;
                        break;
                    case 3 :
                        $fourthset .= $currhtml;
                        $fourthsetpoints = $fourthsetpoints + $linewin;
                        break;
                }
            }
            $currhtml = '';
            $symbolList = new StringTokenizer ( $tiles, ";" );
            $currhtml .= "<ul>";
            $pos = 0;
            while ( $symbolList->hasMoreTokens () ) {
                $sym_id = $symbolList->nextToken ();
                $img = $this->getResultImage ( IMG_PATH_fullmoon, $sym_id + 1 );
                $currhtml .= "<li class='symbol_full' id='symbol_" . ($pos + 1) . "'>" . $img . "</li>";
                $pos ++;
            }
            $currhtml .= "</ul>";
            $i ++;
        }
        $firstset .= "</div><div class='points'>$firstsetpoints</div></div>";
        $secondset .= "</div><div class='points'>$secondesetpoints</div></div>";
        $thirdset .= "</div><div class='points'>$thirdsetpoints</div></div>";
        $fourthset .= "</div><div class='points'>$fourthsetpoints</div></div>";
        $totalpoints = $firstsetpoints + $secondesetpoints + $thirdsetpoints + $fourthsetpoints;
        $firstset = $this->checkSymbols ( $firstset );
        $secondset = $this->checkSymbols ( $secondset );
        $thirdset = $this->checkSymbols ( $thirdset );
        $html .= $firstset;
        $html .= $secondset;
        $html .= $thirdset;

        if ($count == 13) {
            $fourthset = $this->checkSymbols ( $fourthset );
            $html .= $fourthset;
        }
        list ( $wins, $multiplier, $extra, $jackpot ) = explode ( "@", $this->resultString );
        list ( $totalwin, $points ) = explode ( ";", $wins );
        $html .= "</div><div><table style='width:100%'><tr><td><table >";
        if($totalbonus!=0){
            $html .="<tr><td class='label' style='font-size:20px'>Bonus</td><td style='font-size:20px'>$totalbonus</td></tr>";
        }
        $html .="<tr><td class='label' style='font-size:20px'>Total points";
        if($totalbonus!=0){
            $html .="<br><div class='note'>(including bonus)</div>";
        }
        $html .="</td><td style='font-size:20px'> $totalpoints</td></tr>
		  <tr><td class='label' style='font-size:20px'> Multiplier</td><td style='font-size:20px'> $multiplier</td></tr>
		<tr><td class='label' style='font-size:20px'>Total win</td><td style='font-size:20px'>" . getInDollars ( $totalwin / 100 ) . " </td></tr>
							
							  <tr><td class='label' style='font-size:20px'>Extra points</td><td style='font-size:20px'> $extra</td></tr>
							  <tr><td class='label' style='font-size:20px'>Jackpot</td><td style='font-size:20px'>$jackpot</td></tr></table></td>";
        $legend = "<td></td></tr></table></div>";
        $html .= $legend;
        $html .= "</div>";
        return $html;
    }
    private function checkSymbols($set) {
        $set = substr_replace ( $set, "symbol_7", strrpos ( $set, "symbol_1" ), 8 );
        $set = substr_replace ( $set, "symbol_8", strrpos ( $set, "symbol_2" ), 8 );
        $set = substr_replace ( $set, "symbol_9", strrpos ( $set, "symbol_3" ), 8 );
        $set = substr_replace ( $set, "symbol_4", strrpos ( $set, "symbol_1" ), 8 );
        $set = substr_replace ( $set, "symbol_5", strrpos ( $set, "symbol_2" ), 8 );
        $set = substr_replace ( $set, "symbol_6", strrpos ( $set, "symbol_3" ), 8 );
        return $set;
    }
}
?>
