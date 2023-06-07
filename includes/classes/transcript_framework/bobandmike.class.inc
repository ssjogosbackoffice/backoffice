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
define ( "TOTAL_LINES_bobandmike", 25 );
define ( "IMG_PATH_bobandmike", "../media/images/handimages/bobandmike" );
class bobemikegarage extends Transcript
{
    public function getNames()
    {
        return "bobandmike";
    }
    function getBet()
    {
        $betData = explode ( "@", $this->betString );
        $bets=explode(";",$betData[1]);
        $html='';
        $html .= "PAY: <b>" . getInDollars ( $bets [1] / 100 ) . "</b> FOR LINE <br>";
        $html .= "TOTAL LINES: <b>" . number_format ( $bets [0] / $bets[1], 0 ) . "</b>";
        return $html;
    }
    function getReadbleTranscript()
    {
        $html ='';
        $this->setLinesSymbolsPosition ();
        $this->path="bobandmike";
        settype ( $this->betString, "string" );
        $result = new StringTokenizer ( $this->betString, "@" );
        if ($result->countTokens () >= 2) {
            $subid=$result->nextToken();
            $linebets= $result->nextToken ();
            list($totalbets,$betsperline)=explode(";", $linebets);
            $numberlines=$totalbets/$betsperline;
            $winninglines=$result->nextToken();
            list($totalwin,$winlines,$freespin,$bonus,$jackpot)=explode("#",$winninglines);
            if($freespin!='false'){
                list($freespin_true,$freespin_nr)=explode('-',$freespin);
                $bonusHtml  = "<div id='bonuscontainer'>";
                $bonusHtml .= "<center><table cellpadding='2' cellspacing='0'><tr><td class='label'><b>Freespins left</b></td></tr>";
                $bonusHtml .= "<tr><td class='content'>".$freespin_nr."</td></tr>";
                $bonusHtml .= "</table></center></div>";
                $html .=$bonusHtml;
            }
            settype ( $winlines, "string" );
            if($bonus!='false'){
                list($bonus_true,$bonus_lvl,$bonus_multiplier)=explode('-',$bonus);
                $bonusHtml  = "<div id='bonuscontainer'>";
                $bonusHtml .= "<center><table cellpadding='2' cellspacing='0'><tr><td class='label'><b>Bonus Level</b></td><td class='label'><b>Bonus Multiplier</b></th></tr>";
                $bonusHtml .= "<tr><td class='content'>$bonus_lvl</td><td class='content'>" . $bonus_multiplier . "</td></tr>";
                $bonusHtml .= "</table></center></div>";
                $html .=$bonusHtml;
            }
            if($jackpot!='false'){
                list($jackpot_true,$jackpot_lvl,$jackpot_amount)=explode('-',$jackpot);
                $bonusHtml  = "<div id='bonuscontainer'>";
                $bonusHtml .= "<center><table cellpadding='2' cellspacing='0'><tr><td class='label'><b>Jackpot Level</b></td><td class='label'><b>Jackpot Amount</b></th></tr>";
                $bonusHtml .= "<tr><td class='content'>$jackpot_lvl</td><td class='content'>" . $jackpot_amount . "</td></tr>";
                $bonusHtml .= "</table></center></div>";
                $html .=$bonusHtml;
            }
            $results=new StringTokenizer($winlines, ",");
            while($results->hasMoreTokens()) {
                $winline=$results->nextToken();
                $left_lines_html  = "";
                $right_lines_html = "<div class='rightline " . $this->path . "'>";
                $left_lines_html .= "<div class='leftline " . $this->path . "'>";
                $totalsLine = array ();
                $right_lines_html .= "<table class='tableline'><tr><td colspan=2 class='label'>Line</td><td class='label'>Bet</td><td class='label'>Win</td></tr>";
                $left_lines_html .= "<table class='tableline'><tr><td colspan=2 class='label'>Line</td><td class='label'>Bet</td><td class='label'>Win</td></tr>";
                settype ( $winline, "string" );
                $winning_lines = new StringTokenizer ( $winline, "-" );
                while ( $winning_lines->hasMoreTokens () )
                {
                    $symbols=$winning_lines->nextToken();
                    if($winning_lines->hasMoreTokens()){
                        $multiplier=$winning_lines->nextToken();
                        $thislineswin= $winning_lines->nextToken();
                        $thislines= new StringTokenizer ( $thislineswin, ";" );
                        while ( $thislines->hasMoreTokens () )
                        {
                            $thisline=$thislines->nextToken();
                            list ($line_id,$symbol_id,$symbols_pos,$line_win ) = explode ( "~", $thisline );
                            $list = array ($line_id,$symbol_id, $symbols_pos, $line_win );
                            $totalsLine [$line_id] = implode ( ",", $list );
                        }
                    }
                }
                for($i = 0; $i < TOTAL_LINES_bobandmike; $i ++)
                {
                    $line_id = $i + 1;
                    $html_str = "";
                    if (isset ( $totalsLine [$line_id] ))
                    {
                        list ( $line_id,$symbol_id, $symbols_pos,$line_win ) = explode ( ",", $totalsLine [$line_id] );
                        if($line_id<=$numberlines )
                        {
                            $html_str .= "<tr>";
                            $html_str .= "<td><input type='radio' name='lines' class='metalline' part='$results->pos' line_pos='" . $this->totalLines [$line_id - 1] . "' line='$symbols_pos'/></td>";
                            $html_str .= "<td class='line_id'> $line_id </td><td>" . getInDollars ( $betsperline / 100 ) . "</td> <td class='line_dollar'>" . getInDollars ( $line_win / 100 ) . "</td>";
                            $html_str .= "</tr>";
                        }
                    }
                    else
                    {
                        if($line_id<=$numberlines)
                        {
                            $html_str .= "<tr>";
                            $html_str .= "<td><input type='radio' name='lines' class='metalline' part='$results->pos' line_pos='" . $this->totalLines [$line_id - 1] . "'/></td>";
                            $html_str .= "<td class='line_id'> $line_id </td><td>" . getInDollars ( $betsperline / 100 ) . "</td><td class='line_dollar'>" . getInDollars ( 0 ) . "</td>";
                            $html_str .= "</tr>";
                        }
                    }
                    $lineCheck = floor ( TOTAL_LINES_bobandmike / 2 );
                    if ($i < $lineCheck)
                    {
                        $left_lines_html .= $html_str;
                    }
                    else
                    {
                        $right_lines_html .= $html_str;
                    }
                }
                $right_lines_html .= "</table>";
                $left_lines_html .= "</table>";
                $left_lines_html .= "</div>";
                $right_lines_html .= "</div>";
                $html .= "<div class='container " . $this->path . "'  style='clear:both;'/>";
                $symbolList = new StringTokenizer ( $symbols, "|" );
                $html .=$left_lines_html;
                $html .= "<div class='box " . $this->path . "'>";
                $html .= "<ul id='columnList'>";
                $pos = 0;
                while ( $symbolList->hasMoreTokens () ) {
                    $sym_id = $symbolList->nextToken ();
                    $img = $this->getResultImage ( IMG_PATH_bobandmike, $sym_id );
                    if ($pos % 3 == 0) {
                        $html .= "<li id='column_" . ($pos / 3) . "'><ul>";
                    }
                    $html .= "<li class='symbol' id='sym_".$pos."_".$results->pos."'>" . $img . "</li>";
                    $pos ++;
                    if ($pos % 3 == 0) {
                        $html .= "</ul></li>";
                    }
                }
                $html .= "</ul>";
                $html .= "<br style='clear:both;'/>";
                $html .= "</div>";
                $html .=$right_lines_html;
                $html .= "</div><div class='clearfix'><div><br /><br />";
            }
            $html .= '<div id="bobandmike_paytable">
                                <div id="gallery-wrap">
                                    <ul id="gallery">
                                        <li><img src="'.IMG_PATH_bobandmike.'/Paytable1.png" width=100% height=470px alt="" /></li>
                                        <li><img src="'.IMG_PATH_bobandmike.'/Paytable2.png" width=100% height=470px alt="" /></li>
                                        <li><img src="'.IMG_PATH_bobandmike.'/Paytable3.png" width=100% height=470px alt="" /></li>
                                        <li><img src="'.IMG_PATH_bobandmike.'/Paytable4.png" width=100% height=470px alt="" /></li>
                                    </ul>
                                </div>
                            </div>';
        }
        return $html;
    }

    function setLinesSymbolsPosition() {
        $this->totalLines = array (
            "1;4;7;10;13",
            "0;3;6;9;12",
            "2;5;8;11;14",
            "0;4;8;10;12",
            "2;4;6;10;14",
            "0;3;7;11;14",
            "2;5;7;9;12",
            "1;5;7;9;12",
            "1;3;7;10;14",
            "0;4;7;10;14",
            "2;4;7;10;12",
            "0;3;6;10;14",
            "2;5;8;10;12",
            "1;4;8;10;12",
            "1;4;6;10;14",
            "1;5;8;10;12",
            "1;3;6;10;14",
            "2;5;7;9;13",
            "0;3;7;11;13",
            "2;4;6;9;12",
            "0;4;8;11;14",
            "1;5;7;9;13",
            "1;3;7;11;13",
            "2;4;6;10;12",
            "0;4;8;10;14"
        );
    }
}
?>
