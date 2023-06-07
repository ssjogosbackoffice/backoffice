<?php
  
  require_once 'StringTokenizer.class.php';
  require_once 'slot_machine.class.inc';
//  require_once 'video_poker.class.inc';
  require_once 'dograces.class.inc';  
  require_once 'baccaratlive.class.inc';
  require_once 'castawaypoker.class.inc';
  require_once 'magicnumber.class.inc';
  require_once 'roulette.class.inc';
  require_once 'roulette_live.class.inc';
  require_once 'roulette_live_st.class.inc';
  require_once 'roulette_agency.class.inc';
  require_once 'mafia_25.class.inc';
  require_once 'casino_war.class.inc';
  require_once 'fruitslot.class.inc';
  require_once 'castawayslot.class.inc';
  require_once 'roulette_live.class.inc';
  require_once 'blackjacklive.class.inc';
  require_once 'baccaratlive.class.inc';
  require_once 'borut.class.inc';  
  require_once 'fullmetalslot.class.inc';
  require_once 'baccarat_3d.class.inc';
  require_once 'ballomatic_cd.class.inc';
  require_once 'blackjack_3d.class.inc';
  require_once 'roulette_com_draw.class.inc';
  require_once 'diceslots.class.inc';
  require_once 'dograces_cd.class.inc';
  require_once 'sphinx.class.inc';
  require_once 'championfever.class.inc';
  require_once 'vpokerodds_cd.class.inc';
  require_once 'fourfawlplay.class.inc';
  require_once 'bookofra.class.inc';
  require_once 'westernpoker.class.inc';
  require_once 'gonzos.class.inc';
  require_once 'lostfruit.class.inc';
  require_once 'riseofra.class.inc';
  require_once 'vsoccer_cd.class.inc';
  require_once 'burninghot.class.inc';
  require_once 'pokerface.class.inc';
  require_once 'bookofwarrior.class.inc';
  require_once 'columbus.class.inc';
  require_once 'dolphinspearl.class.inc';
  require_once 'luckylady.class.inc';
  require_once 'candyland.class.inc';
  require_once 'ricedice.class.inc';
  require_once 'starburst.class.inc';
  require_once 'ricedicejackpot.class.inc';
  require_once 'fruitcocktail2.class.inc';
  require_once 'highway.class.inc';
  require_once 'candylandjackpot.class.inc';
  require_once 'vpokerodds.class.inc';
  require_once 'doublemagic.class.inc';
  require_once 'vsoccer_cd_multimatch.class.inc';
  require_once 'dadosdice.class.inc';
  require_once 'diceofra.class.inc';
  require_once 'bloodsuckers.class.inc';
  require_once 'bet6.class.inc';
  require_once 'bigeasy.class.inc';
  require_once 'mr_math.class.inc';
  require_once 'vfc.class.inc';
  require_once 'pokerdices.class.inc';
  require_once 'diceewarriors.class.inc';
  require_once 'blooddices.class.inc';
  require_once 'lostdices.class.inc';
  require_once 'slotbot.class.inc';
  require_once 'lostisland.class.inc';
  require_once 'riverqueen.class.inc';
  require_once 'mayanquest.class.inc';
  require_once 'magicisland.class.inc';
  require_once 'fullmoon.class.inc';
  require_once 'sunnyparadise.class.inc';
  require_once 'magiccards.class.inc';
  require_once 'farmsplay.class.inc';
  require_once 'starshine.class.inc';
  require_once 'stardices.class.inc';
  require_once 'bobandmike.class.inc';
  require_once 'triplestar.class.inc';
 // require_once 'vdogscdmultimatch.class.inc';
  require_once 'bankroulette.class.inc';
  require_once 'backgammon.class.inc';
  require_once 'survivors.class.inc';
  require_once 'bookofamon.class.inc';
  require_once 'blooddrinkers.class.inc';
  require_once 'pharaohs.class.inc';
  require_once 'wildlife.class.inc';
  require_once 'wildsafari.class.inc';
  require_once 'mega_lotto.class.inc';
  require_once 'zecchinetta.class.inc';



  abstract class Transcript
  {
      var $instance;
      var $game;
      var $resultString;
      var $betString;
      var $resString;
      var $punterResString;
      var $path;
      var $driver;
      var $betTime;
      var $taxString;

      public static function getInstance($transcript_driver)
      {
        $instance = new $transcript_driver();
        return $instance;
   
      }
      
      public abstract function getReadbleTranscript();
      
      public abstract function getBet();
      
      public function setPath($path_game){
      	$this->path = $path_game;
      }
      
      public function setGame($tbl_name)
      {
        $this->game = $tbl_name;
      }
      
      public function setPunResString($result)
      {
      	$this->punterResString = $result;
      }
      
      public function setString($result)
      {
        $this->resultString = $result;
      }
      
      public function setBetString($bet)
      {
        $this->betString  = $bet;
      }
      public function setTaxString($tax)
      {
          $this->taxString  = $tax;
      }
      public function setResString($resstring)
      {
        $this->resString = $resstring;
      }
      public function setBetTime($time)
      {
          $this->betTime = $time;
      }
      
      function getResultImage($img_path , $symbol_id){

        $img_path = sprintf($img_path , $this->getNames(), $this->path);
        $str = "<img src='".$img_path."/sym_".$symbol_id. ".png' />";
        return $str;
      }
      
      function getBonusImage($img_path , $bonus_id){
        $img_path = sprintf($img_path , $this->getNames(), $this->path);
        $str = "<img src='".$img_path."/".$bonus_id. ".png' />";
        return $str;
      }
  }

?>
