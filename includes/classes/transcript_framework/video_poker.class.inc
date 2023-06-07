<?php
  
  define("IMG_PATH" , "/games/%s/%s/admin_tokens/");
  define("PAYOUT_PATH" , "/games/%s/%s/");
  
  class video_poker extends Transcript
  {
      function videopoker(){
      }
      
      public function getNames(){
          return "videopoker";
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
         return ""; 
      }
      
      function getReadbleTranscript()
      {
        
        settype($this->resultString, "string");
        //return ".." . $this->resultString;
        $result = new StringTokenizer($this->resultString, "-");
        
        $count = $result->countTokens();
        
        $i = 0;
        $handList = array();
        while($hand = $result->nextToken()){
          if($i < ($count)){
            list($cards, $hold,$win) = explode("~",$hand);
            $cardsList = split(";", $cards);
            for($z =0 ; $z < count($cardsList); $z++){
              list($cardType, $cardValue) = split("\|", $cardsList[$z]);
              $cardsList[$z] = getCard(strtolower(trim($cardValue)."".trim($cardType)));
            }
            $holdList  = split(";", $hold);
            array_push($handList, array("win" => $win, "cards" => $cardsList, "hold" => $holdList)); 
          }
          $i++;
        }
        $html = "<div class='container " . $this->path . "'>";
        $html .= "<div class='box " . $this->path . "'>";
        $html .= "<ul>";
        
        
        for($i = 0; $i < count($handList); $i++){
           $hand = $handList[$i];
           $str = "";        
           $html .= "<li><table>";
           switch($i){
             case 0:
               $str = "Prima Mano";
               break;
             case 1:
               $str = "Seconda Mano";
               break;
            case 2:
               if(count($hand["cards"]) > 2){
                 $str = "Rebet";
               }else{
                 $str = "Double";
               }
               break;
            default:
               $str = "Double";
               break;
            
           }
           $html .= "<th colspan='5'>$str</th><tr>";
           $cards = $hand["cards"];
           $hold  = $hand["hold"];
           for($z = 0; $z < count($cards); $z++){
              $html .= "<td";
              if($hold[$z] == 1){
               $html .= " class='hold'";
              }
              $html .= ">$cards[$z]</td>";
              
           }
           $html .= "</tr></table>";
           
           
        }
        
        /**
            ;9,S~7,C~4,D~0,C~1,C;0,0,0,0,0 
            0;4,H~12,H~7,S~4,S~0,S;0,0,0,0,0 
        */
        $html .= "</li>";
        $html .= "</ul>";
        return $html;
        
              
      }
      
  }

?>