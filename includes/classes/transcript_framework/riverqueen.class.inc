<?php
define ( "TOTAL_LINES_riverqueen", 50 );
define ( "IMG_PATH_riverqueen", "../media/images/handimages/riverqueen" );
define ( "PAYOUT_PATH_riverqueen", "../media/images/handimages/riverqueen" );
class riverqueen extends Transcript
{
    public function getNames()
    {
        return "riverqueen";
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
        $this->path="riverqueen";
        settype ( $this->betString, "string" );
        $result = new StringTokenizer ( $this->betString, "@" );
        //var_dump($result);
        if ($result->countTokens () >= 2) {
            $subid=$result->nextToken();
            $linebets= $result->nextToken ();
            //var_dump($linebets);

            list($totalbets,$betsperline)=explode(";", $linebets);
            $numberlines=$totalbets/$betsperline;
            $winninglines=$result->nextToken();
            list($totalwin,$winlines,$freespin,$bonus)=explode("#",$winninglines);

            if($freespin!='false')
            {
                list($freespintype,$freespins)=explode("-", $freespin);
                $bonusHtml = "<div id='bonuscontainer'>";
                $bonusHtml .= "<center><table cellpadding='2' cellspacing='0'><tr><td class='label'><b>Freespins remaining</b></td></tr>";
                $bonusHtml .= "<tr><td class='content'>" . $freespins . "</td></tr>";
                $bonusHtml .= "</table></center></div>";
                $bonusHtml .="</div>";
                $html .=$bonusHtml;
            }
            if($bonus!='false')
            {
                list($type,$bonusnr,$bonuswin)=explode("-", $bonus);

                $plm=explode(',',$bonuswin);

                $bonuswin =array_sum($plm);




                $bonusHtml = "<div id='bonuscontainer'>";
                $bonusHtml .= "<center><table cellpadding='2' cellspacing='0'><tr><td class='label'><b>Bonus Win</b></td></tr>";
                $bonusHtml .= "<tr><td class='content'>" . getInDollars ( $bonuswin / 100 )  . "</td></tr>";
                $bonusHtml .= "</table></center></div>";
                $bonusHtml .="</div>";
                $html .=$bonusHtml;
            }




            settype ( $winlines, "string" );
            $results=new StringTokenizer($winlines, ",");
            while($results->hasMoreTokens()) {
                $winline=$results->nextToken();
                $left_lines_html = "";
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

                        if($freespin!='false')
                        {
                            $multiplier=$multiplier*3;

                        }

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
                $plm=$result->nextToken();
                $gamble=$result->nextToken();
                $gamblenr= new StringTokenizer ($gamble, "#");

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


                for($i = 0; $i < TOTAL_LINES_riverqueen; $i ++)
                {
                    $line_id = $i + 1;
                    $html_str = "";
                    if (isset ( $totalsLine [$line_id] ))
                    {
                        list ( $line_id,$symbol_id, $symbols_pos,$line_win ) = explode ( ",", $totalsLine [$line_id] );
                        if($line_id<=$numberlines)
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

                    $lineCheck = floor ( TOTAL_LINES_riverqueen / 2 );
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
                $html .= "<div class='container " . $this->path . "'  style='clear:both;'/> <br />";

                $symbolList = new StringTokenizer ( $symbols, "|" );
                $html .=$left_lines_html;
                $html .= "<div class='box " . $this->path . "'>";

                $html .= "<ul id='columnList'>";
                $pos = 0;
                while ( $symbolList->hasMoreTokens () ) {
                    $sym_id = $symbolList->nextToken ();
                    $img = $this->getResultImage ( IMG_PATH_riverqueen, $sym_id );
                    if ($pos % 4 == 0) {
                        $html .= "<li id='column_" . ($pos / 4) . "'><ul>";
                    }
                    $html .= "<li class='symbol' id='sym_".$pos."_".$results->pos."'>" . $img . "</li>";
                    $pos ++;
                    if ($pos % 4 == 0) {
                        $html .= "</ul></li>";
                    }
                }
                $html .= "</ul>";
                $html .= "<br style='clear:both;'/>";
                $html .= "</div>";
                $html .=$right_lines_html;
                $html .= "</div>";


            }
            $html .= '<div class="clearfix"></div>
            <br /><div id="riverqueen_paytable">
                                <div id="gallery-wrap">
                                    <ul id="gallery">
                                        <li><img src="'.IMG_PATH_riverqueen.'/Paytable1.png" width=100% height=100% alt="" /></li>
                                        <li><img src="'.IMG_PATH_riverqueen.'/Paytable2.png" width=100% height=500px alt="" /></li>
                                        <li><img src="'.IMG_PATH_riverqueen.'/Paytable3.png" width=100% height=500px alt="" /></li>
                                    </ul>
                                </div>
                            </div>';
        }
        return $html;
    }

    function setLinesSymbolsPosition() {
        $this->totalLines = array (
            "0;4;8;12;16",
            "1;5;9;13;17",
            "2;6;10;14;18",
            "3;7;11;15;19",
            "0;5;10;13;16",
            "1;6;11;14;17",
            "2;5;8;13;18",
            "3;6;9;14;19",
            "0;4;8;12;17",
            "1;5;9;13;16",
            "2;6;10;14;19",
            "3;7;11;15;18",
            "0;5;9;13;17",
            "1;5;9;13;18",
            "2;6;10;14;17",
            "3;6;10;14;18",
            "1;4;8;12;16",
            "1;6;10;14;18",
            "2;5;9;13;17",
            "2;7;11;15;19",
            "0;4;8;13;18",
            "1;5;9;14;19",
            "2;6;10;13;16",
            "3;7;11;14;17",
            "0;5;10;14;18",
            "1;6;11;15;19",
            "2;5;8;12;16",
            "3;6;9;13;17",
            "0;4;9;12;16",
            "1;5;8;13;17",
            "2;6;11;14;18",
            "3;7;10;15;19",
            "0;5;9;13;16",
            "1;5;10;13;17",
            "2;6;9;14;18",
            "3;6;10;14;19",
            "1;4;8;12;17",
            "1;6;10;14;17",
            "2;5;9;13;18",
            "2;7;11;15;18",
            "0;5;8;13;16",
            "1;4;9;12;17",
            "2;7;10;15;18",
            "3;6;11;14;19",
            "1;4;9;14;17",
            "1;6;9;12;17",
            "2;5;10;13;18",
            "2;7;10;15;18",
            "1;6;9;14;17",
            "2;5;10;13;18",
        );
    }
}
//string(114) "9|0|7|1|5|7|9|6|7|3|7|4|9|7|7|7|8|6|0|3-1-27~7~2|5|8~320;50~7~2|5|10|13~1200;7~7~2|5|8|13~1200;47~7~2|5|10|15~1200"
//string(114) "9|0|7|1|5|7|9|6|7|3|7|4|9|7|7|7|8|6|0|3-1-27~7~2|5|8~320;50~7~2|5|10|13~1200;7~7~2|5|8|13~1200;47~7~2|5|10|15~1200"
?>

