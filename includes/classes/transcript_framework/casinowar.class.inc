<?php
  
  define("IMG_PATH" , "/games/%s/%s/admin_tokens/");
  define("PAYOUT_PATH" , "/games/%s/%s/");
  
  class casinowar extends Transcript
  {
      function casinowar(){
      }
      
      public function getNames(){
          return "CasinoWar";
      }
      
      function getBet()
      {
         $betData = split(";" , $this->betString);

         //$html = "";
         //cif($betData[1] == 0){
          // $html .= "<b>PLAY IN FREESPIN</b>";        
         //}else{
         //  $html .= "PAY: <b>" . getInDollars($betData[1]/100) . "</b> FOR LINE <br>";
         //  $html .= "TOTAL LINE: <b>" . number_format($betData[0] / $betData[1],0) . " </b>"; 
        // }
         //return $html; 
         return "";
      }
      
      function getReadbleTranscript()
      {
        
        settype($this->resultString, "string");
        settype($this->resString, "string");
        $dealerCards = split(";", $this->resString);
        list($cardType, $cardValue) = split("\|", $dealerCards[0]);
        $html = "<div align='center' class='container " . $this->path . "'>";
        $html .= "<div class='box " . $this->path . "'>";
        $html .= "<table><tr><td>Dealer Card</td></tr>";
        $html .= "<tr><td align='center'>". getCard(strtolower(trim($cardValue)."".trim($cardType))) . "</td></tr>";
        
        $html .= "<tr><td>Player Card</td></tr>";
        
        $playerCards = split(";", $this->resultString);
        list($cardType, $cardValue) = split("\|", $playerCards[0]);
        
        $html .= "<tr><td align='center'>". getCard(strtolower(trim($cardValue)."".trim($cardType))) . "</td></tr>";
        
        $html .= "</table>";
        return $html;
        
        
              
      }
      
  }

?>