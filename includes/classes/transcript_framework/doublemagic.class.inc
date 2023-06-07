<?php
define ( "TOTAL_LINES_doublemagic", 20 );
define ( "IMG_PATH_doublemagic", "../media/images/handimages/doublemagic" );
define ( "PAYOUT_PATH_doublemagic", "../media/images/handimages/doublemagic" );
?>

<?
class doublemagic extends Transcript
{
    public function getNames()
    {
        return "doublemagic";
    }

    function getBet()
    {

    }

    function getReadbleTranscript()
    {
        ?>
        <link rel="stylesheet" type="text/css" href="/media/table.css" />
<?
        $html ='';
        $betTypes=array(1=>0.05,
                        2=>0.10,
                        3=>0.25);
        settype ( $this->betString, "string" );
        $this->setLinesSymbolsPosition ();
        $result = new StringTokenizer ( $this->betString, "@" );
        $html="<div class='container centered'>";
        $extraBallHtml='';
        $extraBallTotalCost='';
        $extraBallTotalWin='';
        while($result->hasMoreTokens()) {
            $current=$result->nextToken();
            if($result->pos==1){
                list($totalb,$cartoons)=explode("#",$current);
                list($bet,$betAmount)=explode('~',$totalb);
                list($betFake,$betReal)=explode(':',$betAmount);
                $cartoons=explode('|',$cartoons);
                $currentCartoon='<table style="border:20px;border-spacing:50px;padding: 5px;margin:auto"><tr>';

                foreach($cartoons as $k=>$v){
                    if($k==2){
                        $currentCartoon.="</tr><tr>";
                    }
                    $currentCartoon.="<td>";
                    $currentCartoon.="<table class='doublemagic'>";
                    $currentCartoon.='<tr><th colspan="5">Ticket '.($k+1).'</th></tr><tr>';
                    $currentNumbers=explode(',',$v);
                    for($i=0;$i<count($currentNumbers);$i++){
                        if($i%5==0 &&$i!=0){
                            $currentCartoon.="</tr><tr>";
                        }
                        $currentCartoon .="<td id='".$k."_".$i."'>".$currentNumbers[$i]."</td>";
                    }
                    $currentCartoon.="</tr></table></td>";
                }
                $currentCartoon.="</tr></table>";
            }

            if($result->pos==2){
                $extracted=explode('#',$current);
                $extracted_numbers=explode(',',$extracted[0]);
                $firstWin=explode(":",$extracted[1]);
                $winningLines=explode('~',$extracted[2]);
                foreach($winningLines as $k=>$v){
                    if($v!=''){
                        $thisLineWinningSymbols=explode(",",$this->totalLines[$v]);
                        foreach($thisLineWinningSymbols as $k2=>$v2){
                            if($v2=='0'){
                                ?>
                                <script>
                                    $(function(){
                                        $('#<?=$k."_".$k2?>').addClass('errorbold');
                                    });
                                </script>
                            <?
                            }
                        }
                    }
                }
                $firstExtraBallCost=explode(':',$extracted[4]);
                $previousExtraBallCost=$firstExtraBallCost[1];
                $extractedHtml='<h1>Extracted numbers:</h1> ';
                for($i=0;$i<count($extracted_numbers);$i++)
                {
                    $randomcolor=  sprintf('#%06X', mt_rand(0, 0xFFFFFF));
                    $extractedHtml .="<div class='ball fleft doublemagicBall' style='background-color:$randomcolor;line-height:36px'";
                    $extractedHtml .="><div style='vertical-align: middle;color:white; display:inline-block; '> $extracted_numbers[$i]</div></div>";
                }
            }
            if($result->pos>2){
                $randomcolor=  sprintf('#%06X', mt_rand(0, 0xFFFFFF));
                list($extraBall,$win,$winningLines,$extraBallCost)=explode('#',$current);
                $win=explode(':',$win);
                $winningLines=explode("~",$winningLines);
                foreach($winningLines as $k=>$v){
                    if($v!=''){
                    $thisLineWinningSymbols=explode(",",$this->totalLines[$v]);
                        foreach($thisLineWinningSymbols as $k2=>$v2){
                            if($v2=='0'){
                                ?>
                                <script>
                                    $(function(){
                                    $('#<?=$k."_".$k2?>').addClass('errorbold');
                                    });
                                </script>
                                <?
                            }
                        }
                    }
                }
                $extraBallTotalCost+=$previousExtraBallCost;
                $extraBallTotalWin+=$win[1];
                $extraBallHtml.="
                <table style='margin: auto;margin-right:20px;border:1px solid;width: auto;float: left'>
                    <tr><th>Extra ball</th>
                        <th>
                            <div class='ball fleft doublemagicBall' style='background-color:".$randomcolor.";line-height:36px'";
                $extraBallHtml .="><div style='vertical-align: middle;color:white; display:inline-block; '> $extraBall</div></th></tr>".
                       "<tr><td>Win</td><td>".getInDollars($win[1])."</td></tr>".
                    "<tr><td>Cost</td><td>".getInDollars($previousExtraBallCost)."</td></tr></table>";
                $extraBallCost=explode(':',$extraBallCost);
                $previousExtraBallCost=$extraBallCost[1];
            }
        }



        $html.="<div class='clearfix'></div><br />".
               "<table class='beautify' style='width:auto;margin:auto'>
               <thead>
               <tr>
                        <th>Bet</th>
                        <th>Win</th>
                        <th>ExtraBall Bet</th>
                        <th>ExtraBall Win</th>
                        <th>Total Bet</th>
                        <th>Total Win</th>
               </tr>
               </thead>".
               "<tr><td>".getInDollars($betReal)."</td>
                    <td>".getInDollars($firstWin[1])."</td>
                    <td>".getInDollars($extraBallTotalCost)."</td>
                    <td>".getInDollars($extraBallTotalWin)."</td>
                    <td>".getInDollars($betReal+$extraBallTotalCost)."</td>
                    <td>".getInDollars($extraBallTotalWin+$firstWin[1])."</td></tr></table>";
        $html.=$currentCartoon.$extractedHtml."<div class='clearfix'></div><br />".$extraBallHtml;

        $html.="</div>";


        return $html;
    }

    function setLinesSymbolsPosition() {
        $this->totalLines = array (
            "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0",
            "0,0,0,0,0,0,1,1,1,0,0,0,0,0,0",
            "0,1,0,1,0,0,0,0,0,0,0,1,0,1,0",
            "0,0,0,0,0,1,0,1,0,1,1,0,0,0,1",
            "1,1,1,1,1,0,0,0,0,0,0,0,0,0,0",
            "0,0,0,0,0,0,0,0,0,0,1,1,1,1,1",
            "0,0,0,0,0,1,1,1,1,1,0,0,0,0,0",
            "0,0,0,0,0,1,0,1,0,1,1,0,1,0,1",
            "0,1,1,1,0,0,0,0,0,0,1,1,0,1,1",
            "1,1,0,1,1,1,0,1,0,1,0,0,0,0,0",
            "0,1,0,1,0,1,0,1,0,1,0,1,0,1,0",
            "0,1,1,1,0,0,1,1,1,0,0,1,1,1,0",
            "1,1,1,1,1,0,0,0,0,0,1,1,1,1,1",
            "1,1,0,1,1,1,0,1,0,1,0,1,1,1,0",
            "1,1,1,1,1,1,1,1,1,1,0,0,0,0,0",
            "0,0,0,0,0,1,1,1,1,1,1,1,1,1,1"
        );
    }


}
?>
