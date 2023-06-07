<?php
define ( "TOTAL_LINES_fruitcocktail2", 9 );
define ( "IMG_PATH_fruitcocktail2", "../media/images/handimages/fruitcocktail2" );
define ( "PAYOUT_PATH_fruitcocktail2", "../media/images/handimages/fruitcocktail2" );
class fruitcocktail2 extends Transcript
{
    public function getNames()
    {
        return "fruitcocktail2";
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
        $this->path="fruitcocktail2";
        settype ( $this->betString, "string" );



        $result = new StringTokenizer ( $this->betString, "@" );
        if ($result->countTokens () >= 2) {

            $subid=$result->nextToken();
            $linebets= $result->nextToken ();
            list($totalbets,$betsperline)=explode(";", $linebets);
            $numberlines=$totalbets/$betsperline;
            $winninglines=$result->nextToken();
            $subcall=$result->nextToken();
            $gamble=$result->nextToken();
            list($totalwin,$winlines,$freespin,$bonus)=explode("#",$winninglines);

                if($bonus!='false'){
                $b=explode(';',$bonus);
                    $bonusline=$b['1'];

                    $bns=$b['0'];

                    $bs=list($val,$bid,$nroll)=explode("-",$bns);



                    $linee=explode("~",$bonusline);

                    $totalbonuswin=0;
                    foreach($linee as $k => $v){
                        list($bonusSym,$bonuswinsym,$amount_win)=explode('_',$v);
                        $bal=list($sym_id,$match,$bwin)=explode("_",$amount_win);
                        $pl=list($bon1,$bon2,$bon3)=explode(',',$bwin);
                        $btotwin=$bon1+$bon2+$bon3;
                         $totalwin+=$btotwin;
                        }
                $bonusHtml  = "<div>";
                $bonusHtml .= "<center><table cellpadding='2' cellspacing='0'><tr><td class='label'>Bonus Win</td></tr>";
                $bonusHtml .= "<tr><td class='content'>".getInDollars($totalwin/100)."</td></tr>";
                $bonusHtml .= "</table></center></div><br /><br />";
                $html .=$bonusHtml;



            }
            settype ( $winlines, "string" );
            if($freespin!='false'){
                list($freespin_true,$freespin_nr)=explode('-',$freespin);
                $bonusHtml  = "<div id='bonuscontainer'>";
                $bonusHtml .= "<center><table cellpadding='2' cellspacing='0'><tr><td class='label'><b>Freespins left</b></td></tr>";
                $bonusHtml .= "<tr><td class='content'>".$freespin_nr."</td></tr>";
                $bonusHtml .= "</table></center></div>";
                $html .=$bonusHtml;
            }
            $results=new StringTokenizer($winlines, ",");
            while($results->hasMoreTokens()) {
                list($symbols,$multiplier,$winline)=explode('-',$results->nextToken(),3);
                $left_lines_html  = "";
                $right_lines_html = "<div class='rightline " . $this->path . "'>";
                $left_lines_html .= "<div class='leftline " . $this->path . "'>";
                $totalsLine = array ();
                $right_lines_html .= "<table class='tableline'><tr><td colspan=2 class='label'>Line</td><td class='label'>Bet</td><td class='label'>Win</td></tr>";
                $left_lines_html  .= "<table class='tableline'><tr><td colspan=2 class='label'>Line</td><td class='label'>Bet</td><td class='label'>Win</td></tr>";
                settype ( $winline, "string" );
                $thislines= new StringTokenizer ( $winline, ";" );
                while ( $thislines->hasMoreTokens () )
                {
                    $currentline=$thislines->nextToken();
                    if($results->pos==2){
                        list($thisline,$line_win)=explode('-',$currentline);
                        list($line_id,$symbol_id,$symbols_pos) = explode ( "~", $thisline );
                    }
                    else{
                        list ($line_id,$symbol_id,$symbols_pos,$line_win ) = explode ( "~", $currentline );
                    }
                    if($line_id=='0'){
                        $bonussymbols=$symbols_pos;
                        $bonuswin=$line_win;
                        $html .="<div id='bodyHD centered'>
                                            <table style='margin: auto'>
                                                <tr>
                                                    <td class='label'>Bonus symbol</td>
                                                    <td class='label'>Win</td>
                                                    <td class='label'>Show</td>
                                                </tr>
                                                <tr>
                                                    <td>".$this->getResultImage ( IMG_PATH_fruitcocktail2, $symbol_id)."</td>
                                                    <td>".getInDollars($bonuswin/100)."</td>
                                                    <td><a href=\"javascript:showBonusSymbolBookOfRa('$bonussymbols');\">Show</a></td>
                                                </tr>
                                                </table>
                                        </div>";
                    }
                    $listall = array ($line_id,$symbol_id, $symbols_pos, $line_win );

                    if($totalsLine [$line_id]!=''){
                        $totalsLine [$line_id."_extra"]=implode ( ",", $listall );
                    }
                    else{
                    $totalsLine [$line_id] = implode ( ",", $listall );
                    }


                }

                for($i = 0; $i <= TOTAL_LINES_fruitcocktail2; $i++)
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
               if(isset ( $totalsLine [$line_id."_extra"] )){
                    list ( $line_id,$symbol_id, $symbols_pos,$line_win ) = explode ( ",", $totalsLine [$line_id."_extra"] );


                    if($line_id<=$numberlines )

                    {

                        $html_str .= "<tr>";
                        $html_str .= "<td><input type='radio' name='lines' class='metalline' part='$results->pos' line_pos='" . $this->totalLines [$line_id - 1] . "' line='$symbols_pos'/></td>";
                        $html_str .= "<td class='line_id'> $line_id </td><td>" . getInDollars ( $betsperline / 100 ) . "</td> <td class='line_dollar'>" . getInDollars ( $line_win / 100 ) . "</td>";
                        $html_str .= "</tr>";
                    }
                }

                    $lineCheck = floor ( TOTAL_LINES_fruitcocktail2/ 2 );
                    if ($i <= $lineCheck)
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
                    $img = $this->getResultImage ( IMG_PATH_fruitcocktail2, $sym_id );
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
                $html .= "</div>";
            }

            if(($gamble!='')){
                $gamble=explode('#',$gamble);
                $gambleHtml  = "<div id='bonuscontainer'>";
                $gamble_nr=1;
                $gambleHtml .= "<center><table cellpadding='2' cellspacing='0'><tr><td class='label'><b>Gamble nr.</b></td><td class='label'><b>Gamble win</b></td></tr>";
                foreach($gamble as $k=>$v){
                    list($gamble_true,$gamble_amount)=explode(';',$v);
                    if($gamble_true!=''){
                        $gambleHtml .= "<tr><td class='content'>$gamble_nr</td><td class='content'>".getInDollars($gamble_amount/100)."</td></tr>";
                        $gamble_nr++;
                    }
                }
                $gambleHtml .= "</table></center></div>";
                $html .=$gambleHtml;
            }

            $html .= '<br><div id="fruitcocktail2_paytable">
                                <div id="gallery-wrap">
                                    <ul id="gallery">
                                        <li><img src="'.IMG_PATH_fruitcocktail2.'/paytable1.png" width=100% height=400px /></li>
                                        <li><img src="'.IMG_PATH_fruitcocktail2.'/paytable2.png" width=100% height=400px /></li>
                                        <li><img src="'.IMG_PATH_fruitcocktail2.'/paytable3.png" width=100% height=400px /></li>
                                        <li><img src="'.IMG_PATH_fruitcocktail2.'/paytable4.png" width=100% height=400px /></li>
                                        <li><img src="'.IMG_PATH_fruitcocktail2.'/paytable5.png" width=100% height=400px /></li>
                                        <li><img src="'.IMG_PATH_fruitcocktail2.'/paytable6.png" width=100% height=400px /></li>
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
            "0;3;7;9;12",
            "2;5;7;11;14",
            "1;5;8;11;13",
            "1;3;6;9;13",
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
            "0;4;8;10;14" );
    }
}
?>
