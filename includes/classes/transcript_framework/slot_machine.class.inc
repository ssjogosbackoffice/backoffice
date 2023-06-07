<?php
  define("TOTAL_LINES" , 25);
  define("IMGS_PATH" , "/games/%s/%s/admin_tokens/");
  define("PAYOUT_PATH" , "/games/%s/%s/");
  
  class slot_machine extends Transcript
  {
      
      
           
      function slot_machine(){
         
      }
      
      public function getNames(){
          return "slot_machine";
      }
      
      function getBet()
      {
        
         $betData = split(";" , $this->betString);
         
         $html = "";
         if($betData[1] == 0){
           $html .= "<b>PLAY IN FREESPIN</b>";        
         }else{
           $html .= "PAY: <b>" . getInDollars($betData[1]/100) . "</b> FOR LINE <br>";
           $html .= "TOTAL LINE: <b>" . number_format($betData[0] / $betData[1],0) . " </b>"; 
         }
         return $html; 
      }
      
      function getReadbleTranscript()
      {
        $this->setLinesSymbolsPosition($this->path); 
        settype($this->resultString, "string");
        $result = new StringTokenizer($this->resultString, ";");
        
        if($result->countTokens() >= 2){
           $winline = $result->nextToken();
           $symbol  = $result->nextToken();
           $bonus   = $result->nextToken();
           
           
           $left_lines_html  = "";
           $right_lines_html = "<div class='rightline " . $this->path . "'>";
           $left_lines_html .= "<div class='leftline " . $this->path . "'>";
           
           
           $totalsLine = array();
          
           $right_lines_html .= "<ul>";
           $left_lines_html  .= "<ul>";
           
           settype($winline, "string");
           $winning_lines = new StringTokenizer($winline, "|");
           
           while($winning_lines->hasMoreTokens()){
              list($line_id, $line_win, $symbols_pos) = explode(",", $winning_lines->nextToken());
              $list = array($line_id, $line_win, $symbols_pos);
              $totalsLine[$line_id] = implode("," , $list);
              //$winning_lines_html .= "<li>Linea $line_id " . getInDollars($line_win/100) . "<input type='radio' name='lines' class='line' line_pos='".$this->totalLines[$line_id -1]."' line='$symbols_pos'/></li>";
           }
           
           for($i = 0; $i < TOTAL_LINES; $i++){
              $line_id = $i + 1;
              $html_str = "";
              if(!is_null($totalsLine[$line_id])){
                 list($lines_id, $line_win, $symbols_pos) = explode(",", $totalsLine[$line_id]);
                 $html_str .= "<li><table><tr>";
                 $html_str .= "<td><input type='radio' name='lines' class='line' line_pos='".$this->totalLines[$line_id -1]."' line='$symbols_pos'/></td>";
                 $html_str .= "<td class='line_id'> Linea $line_id </td><td class='line_dollar'>" . getInDollars($line_win/100) . "</td>";
                 
                 $html_str .= "</tr></table></li>";
              }else{
                 $html_str .= "<li><table><tr>";
                 $html_str .= "<td><input type='radio' name='lines' class='line' line_pos='".$this->totalLines[$line_id -1]."'/></td>";
                 $html_str .= "<td class='line_id'> Linea $line_id </td><td class='line_dollar'>" . getInDollars(0) . "</td>";
                 
                 $html_str .= "</tr></table></li>";
                 
              }
              
              $lineCheck = floor(TOTAL_LINES / 2);
              
              if($i <= $lineCheck){
                  $left_lines_html .= $html_str;
              }else{
                  $right_lines_html .= $html_str;
              }
          
           }
           
           $right_lines_html .= "</ul>";
           $left_lines_html  .= "</ul>";
           
           $left_lines_html .= "</div>";
           $right_lines_html .= "</div>";
           settype($symbol, "string");
           $symbolList = new StringTokenizer($symbol, "|");
           
           
           
           $html  .= "<div class='container " . $this->path . "'>";
           
           if($bonus){
              
             $bonusHtml = "<div id='bonuscontainer'>";
            
             $bonusData = split("~", $bonus);
             if(strpos($bonusData[0], "B") !== false){
                $bonusHtml .="<table cellpadding='4' cellspacing='0'><tr><td class='label'><b>Bonus</b></td><td class='label'><b>Win</b></td><td class='label'><b>Symbol</b></th></tr>";
                $bonusHtml .= "<tr><td class='content'>".$this->getBonusImage(IMG_PATH,$bonusData[0])."</td><td class='content'>".getInDollars($bonusData[1] / 100)."</td><td class='content'><a href=\"javascript:showBonusSymbol('$bonusData[2]');\">Show</a></td></tr>";
                $bonusHtml .="</table></div>";
            
                $html .=$bonusHtml;
             }
             
             if(strpos($bonusData[0], "FW") !== false){
                $bonusHtml .="<table cellpadding='4' cellspacing='0'><tr><td class='label'><b>Wild</b></td></tr>";
                $bonusHtml .= "<tr><td class='content'>Wild Into Freespin</td></tr>";
                $bonusHtml .="</table></div>";
            
                $html .=$bonusHtml;
             }else if(strpos($bonusData[0], "F") !== false){
                $bonusHtml .="<table cellpadding='4' cellspacing='0'><tr><td class='label'><b>FreeSpin</b></td></tr>";
                $bonusHtml .= "<tr><td class='content'>".$bonusData[2]."</td></tr>";
                $bonusHtml .="</table></div>";
            
                $html .=$bonusHtml;
             }
           }
           
           $html .= $left_lines_html;
           $html .= "<div class='box " . $this->path . "'>";
           
           $html .= "<ul id='columnList'>";
           
           $pos = 0;
           while($symbolList->hasMoreTokens()){
             
              $sym_id = $symbolList->nextToken();
              $img = $this->getResultImage(IMGS_PATH,$sym_id);
              if($pos % 3 == 0){
                $html .= "<li id='column_" . ($pos / 3) . "'><ul>";
              }
              $html .= "<li class='symbol' id='sym_$pos'>" . $img . "</li>";
              $pos++;
              if($pos % 3 == 0){
                $html .= "</ul></li>";
              }
           }
           $html .= "</ul>";
           $html .= "<br style='clear:both;'/>";
           $html .= "</div>";
           $html .= $right_lines_html;
           
           $html .= "<script type='text/javascript'>";
           $html .= "function openPaytable(){";
           $html .= " var so = new SWFObject('/games/".$this->getNames()."/".$this->path."/logo/payout.swf', 'swfcontent', '481', '339', '10', '#000000');";
           $html .= " so.addParam('allowFullScreen','true');";
           $html .= " so.addParam('wmode' , 'transparent');";
           $html .= " so.write('paytable');";
           $html .= "}";
           $html .= "</script>";
           $html .= "<div id='paytable'>";
           $html .= "</div>";
           $html .= "</div>";
         
           
           
        }
        
        
        return $html;
        
      }
      
      function setLinesSymbolsPosition(){
        
        if($this->path == "mafia_25"){
          $this->totalLines = array(
            "1;4;7;10;13",
            "0;3;6;9;12",
            "2;5;8;11;14",
            "0;4;8;10;12",
            "2;4;6;10;14",
            "0;3;7;11;14",
            "2;5;7;9;12",
            "1;5;7;9;12",
            "1;3;7;11;14",
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
            "1;3;7;11;13",
            "1;5;7;9;13",
            "2;4;6;10;12",
            "0;4;8;10;14"
          );
        }
        
        if($this->path == "borut"){
          $this->totalLines = array(
            "1;4;7;10;13",
            "0;3;6;9;12",
            "2;5;8;11;14",
            "0;4;8;10;12",
            "2;4;6;10;14",
            "0;3;7;11;14",
            "2;5;7;9;12",
            "1;3;7;10;13",
            "13;9;7;5;1",
            "14;10;7;4;0",
            "12;10;7;4;2",
            "14;10;6;3;0",
            "12;10;8;5;1",
            "14;10;6;4;1",
            "1;4;8;10;12",
            "0;3;7;11;13",
            "13;9;7;5;2",
            "14;11;7;3;1",
            "1;5;7;9;12",
            "0;3;6;10;14",
            "12;10;8;5;2",
            "14;11;8;4;0",
            "12;9;6;4;2",
            "12;10;6;4;0",
            "14;10;8;4;2"
          );
        }
        
        
      }
  }

?>
