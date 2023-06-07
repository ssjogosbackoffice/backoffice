<?php
  
  define("IMG_PATH" , "/games/%s/%s/admin_tokens/");
  define("PAYOUT_PATH" , "/games/%s/%s/");
  
  class common_draw extends Transcript
  {
   
      function common_draw(){
      }
      
      public function getNames(){
          return "commondraw";
      }
      
      function getBet()
      {
         $betData = split(";" , $this->betString);
         $html = "";
         $html .= "PAY: <b>" . getInDollars($betData[1]/100) . "</b><br/>";
         $bet = $betData[0];
         $game = $bet - ($bet % 1000);
         $gamestr = "";
         switch($game){
            case 3000:
             $gamestr = "FLOP";
             break;
            case 4000:
             $gamestr = "TURN";
             break;
            case 5000:
             $gamestr = "RIVER";
             break;
         }
         $html .= "BET ON: <b>" . $gamestr . " </b><br/>";
         if(count($betData) > 2){
           $suite = "";
           $cardvalue="";
           $listcard = split("\|", $betData[2]);
           switch($listcard[0]){
              case "H":
                $suite = "&hearts;";
                break;
              case "D":
                $suite = "&diamonds;";
                break;
              case "S":
                $suite = "&spades;";
                break;
              case "C":
                $suite = "&clubs";
                break;
                
           }
           $card = $listcard[1];
           switch($card){
              case 0:
                $cardvalue = "A";
                break;
              case 10:
                $cardvalue = "J";
                break;
              case 11:
                $cardvalue = "Q";
                break;
              case 12:
                $cardvalue = "K";
                break;
              default:
                $cardvalue = $card+1;
                break;
           }
           $html .= "RESULTS: <b>" .$cardvalue . $suite . "</b>"; 
         }else{
            $value = "";
            $results = $bet % 1000;
            switch($results){
              case 1:
                $value = "PAIR";
                break;
              case 2:
                $value = "DOUBLE PAIR";
                break;
              case 3:
                $value = "TRIS";
                break;
              case 4:
                $value = "STRAIGHT";
                break;
              case 5:
                $value = "FLUSH";
                break;
              case 6:
                $value = "FULL";  
                break;
              case 7:
                $value = "POKER";
                break;
              case 9:
                $value = "ROYAL STRAIGHT";
                break;
              case 11:
                $value = "JACKPOT";
                break;
            }
            $html .= "RESULTS: <b>" .$value . "</b>";
         } 
         return $html; 
      }
      
      function getReadbleTranscript()
      {
           
        settype($this->resString, "string");     
        settype($this->resultString, "string");
        $result = new StringTokenizer($this->resultString, "~");
        $card = new StringTokenizer($this->resString, ";");
        
        $html = "<div class='container " . $this->game . "'>";
        $html .= "<div class='box " . $this->game . "'>";
        $html .= "<table>";
        $html .= "<tr>";
        $winner = false;
        $wins = array();
        while($result->hasMoreTokens()){
          $win = $result->nextToken();
          if($win == 0){
            $html .= "<td>Perdente</td>";
          }else{
            $html .= "<td>Vincente</td>";
            $winner = true;
          }
          array_push($wins,$win);           
        }
        $html .= "</tr>";
        $html .= "<tr>";
        $i = 0;
        while($cards = $card->nextToken()){
           
          $cardlist = split("~",$cards);
          $cards = trim($cardlist[1]).trim($cardlist[0]);
          $html .= "<td ";
          if($wins[$i] == 1){
              $html .= "class='wincard'";
          }
          $html .= ">" .getCard(strtolower($cards))."</td>" ;
          $i++;
        }
        
        
        $html .= "</tr>";
        if($winner){
          $html .="<tr><td colspan='5' align='center'><h2>Hai Vinto</h2></td></tr>";
        }else{
          $html .="<tr><td colspan='5' align='center'><h2>Hai Perso</h2></td></tr>";
        }
        $html .= "</ul>";
        return $html;
        
              
      }
      
  }

?>