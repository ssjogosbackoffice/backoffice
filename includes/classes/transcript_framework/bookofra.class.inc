<script type="text/javascript">
    function showBonusSymbolBookOfRa(symbol){
        $(".symbol").removeClass("blink_line");
        $(".symbol").removeClass("blink");
        blinking_pos     = symbol.split("|");
        for(var i = 0 ; i < blinking_pos.length ; i++){
            $("#sym_" + blinking_pos[i]+"_1").removeClass("blink_line");
            $("#sym_" + blinking_pos[i]+"_1").addClass("blink").fadeOut().fadeIn();
        }
    }
</script>
<?php
/**
 * User: marian
 * Date: 6/10/18
 * Time: 3:57 PM
 */
define ( "TOTAL_LINES_RA", 10 );
define ( "IMG_PATH_RA", "../media/images/handimages/bookofra" );
require_once 'Transcript.class.inc';
require_once 'transcript.inc.php';
require_once 'general_functions.inc';
class bookofra extends Transcript {

    public function getNames() {
        return "bookofra";
    }
    function getBet() {
        $betData = explode( "@", $this->betString );
        $lines=explode(";",$betData[1]);
        if ($betData [0] == 49) {
            $html = "<b>PLAY IN FREESPIN</b>";
        } else {
            $html = "PAY: <b>" . getInDollars ( $lines[1] / 100 ) . "</b> FOR LINE <br>";
            $html .= "TOTAL LINES: <b>" . number_format ( $lines[0] / $lines[1], 0 ) . "</b>";
        }
        return $html;
    }
    function getReadbleTranscript()
    {
        $betData = explode( "@", $this->betString );
        $html = '';
        $lines=explode(";",$betData[1]);
        $numberlines=$lines[0]/$lines[1];
        $this->setLinesSymbolsPosition ();
        settype ( $betData[2], "string" );
        $result = new StringTokenizer ( $betData[2], "#" );
        if ($result->countTokens () >= 2) {
            $totalWin = $result->nextToken ();
            $winline= $result->nextToken();
            $symbols = $result->nextToken ();
            $bonus = $result->nextToken ();
            if($betData[0]==49){
                list($winnormal,$winexpansion)=explode("-",$winline);
            }
            if($winnormal!='null'||$betData[0]==6){
                $left_lines_html = "";
                $right_lines_html = "<div class='rightline " . $this->path . "'>";
                $left_lines_html .= "<div class='leftline " . $this->path . "'>";
                $totalsLine = array ();
                $right_lines_html .= "<table class='tableline'><tr><td colspan=2 class='label'>Line</td><td class='label'>Bet</td><td class='label'>Win</td></tr>";
                $left_lines_html .= "<table class='tableline'><tr><td colspan=2 class='label'>Line</td><td class='label'>Bet</td><td class='label'>Win</td></tr>";
                settype ( $winline, "string" );
                if($betData[0]==6){
                    list($wlines,$plm)=explode("-",$winline);
                $winning_lines = new StringTokenizer ( $wlines, ";" );
                }
                else{
                    $winning_lines = new StringTokenizer ( $winnormal, ";" );
                }
                while ( $winning_lines->hasMoreTokens () ) {
                    list ( $line_id,  $symbols_pos,$line_win ) = explode ( "~", $winning_lines->nextToken () );
                    if($line_id=='0'){
                        $bonussymbols=$symbols_pos;
                        $bonuswin=$line_win;
                    }
                    else{
                    $list = array ($line_id, $symbols_pos, $line_win );
                    $totalsLine [$line_id] = implode ( ",", $list );
                    }
                }
                for($i = 0; $i < TOTAL_LINES_RA; $i ++) {
                    $line_id = $i + 1;
                    $html_str = "";
                    if (isset( $totalsLine [$line_id] )) {
                        list ( $line_id,  $symbols_pos,$line_win ) = explode ( ",", $totalsLine [$line_id] );
                        if($line_id<=$numberlines)
                        {
                            $html_str .= "<tr>";
                            $html_str .= "<td><input type='radio' name='lines' class='metalline' part='1' line_pos='" . $this->totalLines [$line_id - 1] . "' line='$symbols_pos'/></td>";
                            $html_str .= "<td class='line_id'> $line_id </td><td>" . getInDollars ( $betData[1] / 100 ) . "</td> <td class='line_dollar'>" . getInDollars ( $line_win / 100 ) . "</td>";
                            $html_str .= "</tr>";
                        }
                    } else {
                        if($line_id<=$numberlines)
                        {
                            $html_str .= "<tr>";
                            $html_str .= "<td><input type='radio' name='lines' class='metalline' part='1' line_pos='" . $this->totalLines [$line_id - 1] . "'/></td>";
                            $html_str .= "<td class='line_id'> $line_id </td><td>" . getInDollars ( $betData[1] / 100 ) . "</td><td class='line_dollar'>" . getInDollars ( 0 ) . "</td>";
                            $html_str .= "</tr>";
                        }
                    }
                    $lineCheck = floor ( TOTAL_LINES_RA / 2 );
                    if ($i < $lineCheck) {
                        $left_lines_html .= $html_str;
                    } else {
                        $right_lines_html .= $html_str;
                    }
                }
                $right_lines_html .= "</table>";
                $left_lines_html .= "</table>";
                $left_lines_html .= "</div>";
                $right_lines_html .= "</div>";
                list($symbol,$symbols2)=explode("-",$symbols);
                settype ( $symbol, "string" );
                $symbolList = new StringTokenizer ( $symbol, "|" );
                $html .= "<div class='container " . $this->path . "'>";
                $html .= $left_lines_html;
                $html .= "<div class='box " . $this->path . "'>";
                $html .= "<ul id='columnList'>";
                $pos = 0;
                while ( $symbolList->hasMoreTokens () ) {
                    $sym_id = $symbolList->nextToken ();
                    $img = $this->getResultImage ( IMG_PATH_RA, $sym_id );
                    if ($pos % 3 == 0) {
                        $html .= "<li id='column_" . ($pos / 3) . "'><ul>";
                    }
                    $html .= "<li class='symbol' id='sym_".$pos."_1'>" . $img . "</li>";
                    $pos ++;
                    if ($pos % 3 == 0) {
                        $html .= "</ul></li>";
                    }
                }
                $html .= "</ul>";
                $html .= "<br style='clear:both;'/>";
                $html .= "</div>";
                $html .= $right_lines_html;
                $html .= "<div class='clearfix'></div><br>";
            }
            if($betData[0]==49){
                if($winnormal!='null' && $winexpansion=='null'){
                }
                else{
                $left_lines_html = "";
                $right_lines_html = "<div class='rightline " . $this->path . "'>";
                $left_lines_html .= "<div class='leftline " . $this->path . "'>";
                $totalsLine = array ();
                $right_lines_html .= "<table class='tableline'><tr><td colspan=2 class='label'>Line</td><td class='label'>Bet</td><td class='label'>Win</td></tr>";
                $left_lines_html .= "<table class='tableline'><tr><td colspan=2 class='label'>Line</td><td class='label'>Bet</td><td class='label'>Win</td></tr>";
                settype ( $winline, "string" );
                $winning_lines = new StringTokenizer ( $winexpansion, ";" );
                while ( $winning_lines->hasMoreTokens () ) {
                    list ( $line_id,  $symbols_pos,$line_win ) = explode ( "~", $winning_lines->nextToken () );
                    $list = array ($line_id, $symbols_pos, $line_win );
                    $totalsLine [$line_id] = implode ( ",", $list );
                }
                for($i = 0; $i < TOTAL_LINES_RA; $i ++) {
                    $line_id = $i + 1;
                    $html_str = "";
                    if (isset( $totalsLine [$line_id] )) {
                        list ( $line_id,  $symbols_pos,$line_win ) = explode ( ",", $totalsLine [$line_id] );
                        if($line_id<=$numberlines)
                        {
                            $html_str .= "<tr>";
                            $html_str .= "<td><input type='radio' name='lines' class='metalline' part='2' line_pos='" . $this->totalLines [$line_id - 1] . "' line='$symbols_pos'/></td>";
                            $html_str .= "<td class='line_id'> $line_id </td><td>" . getInDollars ( $betData[1] / 100 ) . "</td> <td class='line_dollar'>" . getInDollars ( $line_win / 100 ) . "</td>";
                            $html_str .= "</tr>";
                        }
                    } else {
                        if($line_id<=$numberlines)
                        {
                            $html_str .= "<tr>";
                            $html_str .= "<td><input type='radio' name='lines' class='metalline' part='2' line_pos='" . $this->totalLines [$line_id - 1] . "'/></td>";
                            $html_str .= "<td class='line_id'> $line_id </td><td>" . getInDollars ( $betData[1] / 100 ) . "</td><td class='line_dollar'>" . getInDollars ( 0 ) . "</td>";
                            $html_str .= "</tr>";
                        }
                    }
                    $lineCheck = floor ( TOTAL_LINES_RA / 2 );
                    if ($i < $lineCheck) {
                        $left_lines_html .= $html_str;
                    } else {
                        $right_lines_html .= $html_str;
                    }

                }
                $right_lines_html .= "</table>";
                $left_lines_html .= "</table>";
                $left_lines_html .= "</div>";
                $right_lines_html .= "</div>";
                list($symbol,$symbols2)=explode("-",$symbols);
                settype ( $symbols2, "string" );
                if($winexpansion!='null'){
                    $symbolList = new StringTokenizer ( $symbols2, "|" );
                }
                else{
                    if($winnormal=='null'){
                    $symbolList = new StringTokenizer ( $symbol, "|" );
                    }
                }
                $html .= "<div class='container " . $this->path . "'>";
                $html .= $left_lines_html;
                $html .= "<div class='box " . $this->path . "'>";
                $html .= "<ul id='columnList'>";
                $pos = 0;
                while ( $symbolList->hasMoreTokens () ) {
                    $sym_id = $symbolList->nextToken ();
                    $img = $this->getResultImage ( IMG_PATH_RA, $sym_id  );
                    if ($pos % 3 == 0) {
                        $html .= "<li id='column_" . ($pos / 3) . "'><ul>";
                    }
                    $html .= "<li class='symbol' id='sym_".$pos."_2'>" . $img . "</li>";
                    $pos ++;
                    if ($pos % 3 == 0) {
                        $html .= "</ul></li>";
                    }
                }
                $html .= "</ul>";
                $html .= "<br style='clear:both;'/>";
                $html .= "</div>";
                $html .= $right_lines_html;
                $html .= "<div class='clearfix'></div><br>";
            }
            }
            if(strpos($bonus,'true')===0){
                list($bonustrue,$specialsymbol,$numberofspins)=explode("~",$bonus);
                $html .="<div id='bodyHD centered'><table style='margin: auto'><tr><td class='label'>Bonus symbol</td><td class='label'>Freespins</td><td class='label'>Win</td class='label'><td class='label'>Show</td></tr>
                        <tr><td>".$this->getResultImage ( IMG_PATH_RA, $specialsymbol )."</td><td>$numberofspins</td><td>".getInDollars($bonuswin/100)."</td><td><a href=\"javascript:showBonusSymbolBookOfRa('$bonussymbols');\">Show</a></td></tr></table></div>";
            }
            if($betData[4]!=''){
                $html .="<div id='double' class='centered'><div class='bodyHd centered'> Double</div><br> ";
                $doubles=explode('#',$betData[4]);
                $html .='<table style="margin:auto"><tr><td class="label">Player choice</td><td class="label">Result</td><td class="label">Win</td></tr>';
                for ($j=0;$j<(count($doubles)-1);$j++ ){
                    list($doubleon,$doubleresult)=explode(';',$doubles[$j]);
                    list($credittodouble,$colorchosen)=explode("-",$doubleon);
                    list($doublewin,$doublecolorresult,$doublesign)=explode("-",$doubleresult);
                    if($colorchosen=='R'){
                        $colorchosen='RED';
                    }
                    else
                    {
                        $colorchosen='BLACK';
                    }
                    if($doublecolorresult=='R'){
                        $doublecolorresult='RED';
                    }
                    else
                    {
                        $doubleresult='BLACK';
                    }
                    $html.="<tr><td>$colorchosen</td><td>$doublecolorresult</td><td>".getInDollars($doublewin/100)."</td></tr>";
                }
                $html .="</table></div>";
            }
            $html .= "<div class='clearfix'></div><br><br>
                      <div class='bodyHD centered'>PAYTABLE</div><br>
                      <div class='clearfix'></div>
                      <div id='bookofra_paytable'></div>";
        }
        return $html;
    }
    function setLinesSymbolsPosition() {
        $this->totalLines = array ("1;4;7;10;13", "0;3;6;9;12", "2;5;8;11;14", "0;4;8;10;12", "2;4;6;10;14", "1;5;8;11;13", "1;3;6;9;13", "2;5;7;9;12", "0;3;7;11;14", "2;4;7;10;12" );
    }
}
?>
