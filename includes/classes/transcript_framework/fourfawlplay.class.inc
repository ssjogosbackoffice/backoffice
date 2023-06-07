<?php
/**
 * @author Marian
 */
require_once 'Transcript.class.inc';
require_once 'transcript.inc.php';
require_once 'general_functions.inc';
define ( "TOTAL_LINES_fourfawl", 10 );
define ( "IMG_PATH_fourfawl", "../media/images/handimages/fourfawl" );
define ( "PAYOUT_PATH_fourfawl", "../media/images/handimages/fourfawl" );

class  fourfawlplay extends Transcript {
    public function getNames() {
        return "fourfawlplay";
    }//   125;5

    function getBet() {
        $betDatas = explode ( "@" , $this->betString );
        $betData=explode(";",$betDatas[1]);
        $html = "<div class='centred'><table>";
        if ($betData [1] == 0) {
            $html .= "<tr><td><b>PLAY IN FREESPIN</b></td></tr>";
        } else {
            $html .= "<tr><td><b>" . getInDollars ( $betData [1] / 100 ) . "</td>";
            $html .= "<tr><td>TOTAL LINES:</td><td> <b>" . number_format ( $betData [0] / $betData [1], 0 ) . "</b></td></tr>";
        }
        $html .= "</table></center>";
        return $html;
    }
    function getReadbleTranscript() {
        $this->path = "fourfawlplay";
        $position = 0;
        $this->setLinesSymbolsPosition ();

        list($operationid,$firsthandwin,$firsthand,$secondopid,$secondhandhold,$secondhand,$riskid,$risk)=explode("@",$this->betString);

        $secondhandholdsymbols=explode("~",$secondhandhold);
        $firsthandholdsym=$secondhandholdsymbols[3];
        $firsthandholdsym=explode(";",$firsthandholdsym);
        $html ='<div class="centered">';

        $html .= "<b>FIRST SPIN</b><br><br>";

        $html .="<table style='margin: auto'><tr><td class='label'>Total bet</td><td class='label'>Bet per line</td><td class='label'>Number of hands</td><td class='label centered'> Win</td>";
        if($risk!=''){
            $risks=explode("#",$risk);
            foreach ($risks as $r=>$rv){
                list($prevwin,$thisrisk)=explode(";",$rv);

                $riskwin=$thisrisk;
                if($rv!=''){
                    $html .="<td class='label'><b>Double Win  </b> </td>";
                }

            }
        }
        else{
            $html .="</tr>";
        }

        list($firsthandtotal,$firsthandperline,$manches)=explode(";",$firsthandwin);
        $numberlines=$firsthandtotal/$firsthandperline;
        $html.="<tr><td class='centered'>".getInDollars($firsthandtotal/100)."</td><td class='centered'>".getInDollars($firsthandperline/100)."</td><td class='centered'>$manches</td>";

        $firstmanche ="</tr>";
        $firstmanche .="</table>";
        list($firsthandsymbols,$firsthandhold)=explode("-",$firsthand);
        $symbolList = new StringTokenizer ( $firsthandsymbols, "|" );
        $firstmanche .= "<div class='box " . $this->path . "' style='margin:auto;float:none'>";
        $firstmanche .= "<ul id='columnList'>";
        $pos = 0;
        while ( $symbolList->hasMoreTokens () ) {
            $sym_id = $symbolList->nextToken ();
            $img = $this->getResultImage ( IMG_PATH_fourfawl, $sym_id + 1 );
            if ($pos % 3 == 0) {
                $firstmanche .= "<li id='column_" . ($pos / 3) . "'><ul>";
            }
            $firstmanche .= "<li class='symbol' id='sym_$pos'>" . $img . "</li>";
            $pos ++;
            if ($pos % 3 == 0) {
                $firstmanche .= "</ul></li>";
            }
        }
        $firstmanche .= "</ul>";
        $firstmanche .= "<br style='clear:both;'/>";
        $firstmanche .= "</div>";
        $firstmanche.="</div>";
        $firstmanche .= "<br style='clear:both;'/>";
        $firstmanche .="<div class='centered'>";
        $firstmanche .="<br><b>SECOND SPIN</b>";
        $results = new StringTokenizer ( $secondhand, "," );

        while ( $results->hasMoreTokens ()) {
            $result=$results->nextToken();
            if($results->pos=='1')
            {


                if($riskwin<0){
                    $html.="<td>".getInDollars(0)." (lost at risk )</td>";
                }
                elseif($riskwin>0){
                    $html.="<td>".getInDollars($riskwin/100)." ( win at risk )</td>";
                }
                else{
                    $html.="<td>".getInDollars($result/100)."</td>";
                }
                if($risk!=''){
                    $risks=explode("#",$risk);
                    foreach ($risks as $r=>$rv){
                        list($prevwin,$thisrisk)=explode(";",$rv);

                        $riskwin=$thisrisk;
                        if($rv!=''){
                            $html .="<td class='centred'>" . getInDollars($thisrisk/100)."<br /></td>";
                        }

                    }
                }
                $html.=$firstmanche;
            }
            if($results->pos>1){
                $manche=($results->pos-1);
                $betDatas = explode ( "@" , $this->betString );
                $betData=explode(";",$betDatas[1]);

                list($mancheWin,$winninglines,$bonus,$symbols)=explode('#',$result);
                list($handsymbols,$nushceplm,$lines)=explode("-",$winninglines);
                $symbolList= new StringTokenizer ( $handsymbols, "|" );

                $html .="<div class='centered label'><b>Manche ".(($results->pos)-1)."</b></div><br /><div class='box_fourfawlplay_table'>";
                if($bonus!='false'){
                    list($bonustrue,$bonusmsg)=explode('-',$bonus);
                    $bonusses=explode(";",$bonusmsg);
                    $bwin=0;
                    $tryes=0;
                    foreach($bonusses as $b=>$bk){
                        list($trynr,$bamount)=explode("_",$bk);
                        $tryes=$trynr;
                        $bwin=$bwin+$bamount;

                    }
                    $html .="<table class='centered' style='margin: auto'><tr><td class='formheading' colspan=2>Bonus</td> </tr><tr><td class='label'>Tries</td><td class='label'>Win</td></tr><tr><td>$tryes</td><td>".getInDollars($bwin/100)."</td></tr></table>";
                }
                if($symbols!='false' || strpos($symbols,'true') ){
                    $html.="<div class='centered'><b>Golden Goose</b></div>";

                    list($bonustrue,$betAmount,)=explode('~',$symbols);
                    $bonusses=explode(";",$betAmount);
                    $bwin=0;
                    $tryes=0;
                    foreach($bonusses as $b=>$bk){
                        $html .="<table class='centered' style='margin: auto'><tr><td class='label'>Trie</td><td class='label'>Win</td></tr><tr><td>".($b+1)."</td><td>".getInDollars($bk/100)."</td></tr></table>";


                    }
                    // $html .="<table class='centered' style='margin: auto'><tr><td class='formheading' colspan=2>Golden Goose</td> </tr><tr><td class='label'>Tries</td><td class='label'>Win</td></tr><tr><td>$tryes</td><td>".getInDollars($bk/100)."</td></tr></table>";
                }
                $right_lines_html = "<div class='rightline " . $this->path . "'>";
                $left_lines_html = "<div class='leftline " . $this->path . "'>";
                $totalsLine = array ();
                $right_lines_html .= "<table class='tableline'><tr><td colspan=2 class='label'>Line</td><td class='label'>Bet</td><td class='label'>Win</td></tr>";
                $left_lines_html .= "<table class='tableline'><tr><td colspan=2 class='label'>Line</td><td class='label'>Bet</td><td class='label'>Win</td></tr>";
                settype ( $winninglines, "string" );
                $winning_lines = new StringTokenizer ( $lines, ";" );
                while ( $winning_lines->hasMoreTokens () ) {
                    list ( $line_id,$symbols, $symbols_pos,$line_win ) = explode ( "~", $winning_lines->nextToken () );
                    $list = array ($line_id,$symbols,$symbols_pos,$line_win );
                    $totalsLine [$line_id] = implode ( ",", $list );
                }
                for($i = 0; $i < TOTAL_LINES_fourfawl; $i ++) {
                    $line_id = $i + 1;
                    $html_str = "";
                    if (isset( $totalsLine [$line_id] )) {
                        list ( $line_id,$symbols, $symbols_pos,$line_win ) = explode ( ",", $totalsLine [$line_id] );
                        if($line_id<=$numberlines)
                        {
                            $html_str .= "<tr>";
                            $html_str .= "<td><input type='radio' name='lines' class='metalline' part='$manche' line_pos='" . $this->totalLines [$line_id - 1] . "' line='$symbols_pos'/></td>";
                            $html_str .= "<td class='line_id'> $line_id </td><td>" . getInDollars ($betData [1] / 100 ) . "</td> <td class='line_dollar'>" . getInDollars ( $line_win / 100 ) . "</td>";
                            $html_str .= "</tr>";
                        }
                    } else {
                        if($line_id<=$numberlines)
                        {
                            $html_str .= "<tr>";
                            $html_str .= "<td><input type='radio' name='lines' class='metalline' part='$manche' line_pos='" . $this->totalLines [$line_id - 1] . "'/></td>";
                            $html_str .= "<td class='line_id'> $line_id </td><td>" . getInDollars ( $betData [1] / 100 ) . "</td><td class='line_dollar'>" . getInDollars ( 0 ) . "</td>";
                            $html_str .= "</tr>";
                        }
                    }
                    $lineCheck = floor ( TOTAL_LINES_fourfawl / 2 );
                    if ($i < $lineCheck) {
                        $left_lines_html .= $html_str;
                    } else {
                        $right_lines_html .= $html_str;
                    }
                }
                $right_lines_html .= "</table></div>";
                $left_lines_html .= "</table></div>";
                $html.=$left_lines_html;
                $html .= "<div class='box " . $this->path . "'>";
                $html .= "<ul id='columnList'>";
                $pos = 0;
                $symbols=(explode('-',$symbols));

                $symbolListorig =explode('|', $firsthandsymbols);
                while ( $symbolList->hasMoreTokens ()) {
                    $sym_id = $symbolList->nextToken ();

                    if($sym_id==99){

                        $sym_id=$symbolListorig[$symbolList->pos-1];
                        $img = $this->getResultImage ( IMG_PATH_fourfawl, $sym_id + 1 );
                        if ($pos % 3 == 0) {
                            $html .= "<li id='column_" . ($pos / 3) . "'><ul>";
                        }
                        $html .= "<li class='symbol hold' id='sym_".$pos."_".$manche."'>" . $img . "</li>";
                        $pos ++;
                        if ($pos % 3 == 0) {
                            $html .= "</ul></li>";
                        }

                    }
                    else{
                        $img = $this->getResultImage ( IMG_PATH_fourfawl, $sym_id + 1 );
                        if ($pos % 3 == 0) {
                            $html .= "<li id='column_" . ($pos / 3) . "'><ul>";
                        }
                        $html .= "<li class='symbol' id='sym_".$pos."_".$manche."'>" . $img . "</li>";
                        $pos ++;
                        if ($pos % 3 == 0) {
                            $html .= "</ul></li>";
                        }
                    }
                }
                $html .= "</ul>";
                $html .= "</div></div>";
                $html.=$right_lines_html;


                $html .= "<br style='clear:both;'/><br>";
            }

        }
        $html .="</div>";
        $html .= "<br style='clear:both;'/>";
        $html .= "</table></center>";
        $html .= "<div id='fourfawlplay_paytable' class='centered'><b>PAYTABLE</b><table cellpadding='4'><br>";
        $html .= "<tr><td class='label'>Symbol/<br>multiplier</td> <td class='label'> x 3 </td><td class='label'> x 4 </td><td class='label'> x 5 </td></tr>";
        $html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_fourfawl, '1' ) . "</td><td>10</td><td>20</td><td>100</td></tr>";
        $html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_fourfawl, '2' ) . "</td><td>10</td><td>20</td><td>100</td></tr>";
        $html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_fourfawl, '3' ) . "</td><td>10</td><td>20</td><td>100</td></tr>";
        $html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_fourfawl, '5' ) . "</td><td>10</td><td>20</td><td>100</td></tr>";
        $html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_fourfawl, '6' ) . "</td><td>10</td><td>20</td><td>100</td></tr>";
        $html .= "</table>";
        $html .= "<table cellpadding='4'><tr><td class='label'>Symbol/<br>multiplier</td><td class='label'> x 3 </td><td class='label'> x 4 </td><td class='label'> x 5 </td></tr>";
        $html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_fourfawl, '4' ) . "</td><td>20</td><td>50</td><td>250</td></tr>";
        $html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_fourfawl, '9' ) . "</td><td>20</td><td>50</td><td>250</td></tr>";
        $html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_fourfawl, '7' ) . "</td><td>50</td><td>250</td><td>500</td></tr>";
        $html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_fourfawl, '8' ) . "</td><td>100</td><td>400</td><td>500</td></tr>";
        $html .= " <tr><td class='label'>" . $this->getResultImage ( IMG_PATH_fourfawl, '10' ) . "</td><td colspan='3'>Activates the<br>game of skill</td></tr>";
        $html .= "</table></div></div>";
        return $html;
    }
    function setLinesSymbolsPosition() {
        $this->totalLines = array ("1;4;7;10;13", "0;3;6;9;12", "2;5;8;11;14", "0;4;8;10;12", "2;4;6;10;14", "1;3;6;9;13", "1;5;8;11;13", "2;4;7;10;14", "0;4;7;10;12", "1;4;8;10;13");
    }
}
?>