<?
globalise('hand_id');
require_once 'GameTranscript.class.inc';
require_once 'transcript.inc.php';

?>
<HTML>
<HEAD>
<TITLE>Hand #<?=$hand_id?></TITLE>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/style.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/style.css" title="core_style" />
<script type="text/javascript" src="<?= secure_host ?>/media/jquery.js" ></script>
<script type="text/javascript" src="<?= secure_host ?>/media/swfobject.js" ></script>
<style>
.card{
  float:none;
  background-image:none;
  border:1px solid #000;
  display:table-cell;
  min-width:40px;
  height:60px;
  margin:2px;
  padding:1px;
  background-color:#FFF;
  font-size:18px;
  text-align:center;
  padding:3px;
}

.hold{
  background-color:#000;
}

.seed{
   font-size:30px;
}

.gameStage{
  color:#000;
 
  margin-top:20px;
  margin-bottom:5px;
  margin-right:10px;
  font-weight:bold;
  width:100%;
  display:block;
  padding:5px;
}
.transcriptHeader{
  padding:0px;
}


.winner, a.winner{
  color:#Green;
  font-weight:bold;
  text-decoration:blink;
}

</style>
<script type="text/javascript">
  
 
  function blinkSymbols(symbols , symbols_pos){
    $(".symbol").removeClass("blink_line");
    $(".symbol").removeClass("blink");
    blinking_pos     = symbols_pos.split(";");
    
    for(var i = 0 ; i < blinking_pos.length ; i++){
      $("#sym_" + blinking_pos[i]).addClass("blink_line");
    }
    if(symbols != undefined){
      blinking_symbols = symbols.split("~");
      for(var i = 0 ; i < blinking_symbols.length ; i++){
        $("#sym_" + blinking_symbols[i]).removeClass("blink_line");
        $("#sym_" + blinking_symbols[i]).addClass("blink").fadeOut().fadeIn();
      }
    }
  }
  
  function showBonusSymbol(symbol){
    $(".symbol").removeClass("blink_line");
    $(".symbol").removeClass("blink");
    blinking_pos     = symbol.split(",");
    for(var i = 0 ; i < blinking_pos.length ; i++){
      $("#sym_" + blinking_pos[i]).removeClass("blink_line");
      $("#sym_" + blinking_pos[i]).addClass("blink").fadeOut().fadeIn();
    }

  }
  
  $(document).ready(function(){
    $(".line").click(function(){
      blinkSymbols($(this).attr("line"),$(this).attr("line_pos"));
    });
    
    openPaytable();
   });
   
</script>
</HEAD>
<body leftmargin="0" topmargin="0" rightmargin="0" bottommargin="0" marginwidth="0" class=body>
<div id="urlContent"><?handsdetail($hand_id)?></div>
<?php


$players = array();
$ips_r   = array();

function getPunterBalance($punter){
  global $players;
  return $players[$punter]["balance"];
}

function getPunterSeatid($punter){
  global $players;
  return "<img src='" . $players[$punter]["seatid"] . "'>";
}


function checkIp($ip){
  global $players, $ips_r;
  $ips_r[$ip]++;
  
  return getClassLink(refPage("customers/search&field=4&search_string=" . urlencode(ereg_replace("[^0-9\.]", "", $ip)) ), $ip, (($ips_r[$ip] > 1)?("red"):("")), "_blank");
}

function handsdetail($hand_id){
  global $dbh, $players,$lang;
  $sql = "SELECT res_id, res_total_bets,gam_name, ctr_notes, res_total_rake, res_string, res_date, tbl_id, tbl_name FROM result JOIN tables ON tbl_id = res_tbl_id JOIN game on gam_id = res_gam_id JOIN customer_transaction on ctr_res_id = res_id WHERE res_id = $hand_id  AND tbl_themeid = (res_gam_id - (res_gam_id % 1000)) AND ctr_pun_id = " . $_GET["uid"]. "  AND ctr_id = " . $_GET["gmn"];
  
  $rs = $dbh->exec($sql);
  $num_rows = $rs->getNumRows();
  
  if($num_rows > 0){
    $game_general_details = $rs->next();
    
    $transcript = "<div class='transcriptHeader'><h2>".ucfirst(returnRealGamesName($game_general_details['gam_name']))."</h2></div>" . GameTranscript::getRouletteCDString($hand_id, $_GET["uid"], $_GET["gmn"]) . "<br style='clear:both'/>";
    ?>
    <div align="center">
    
      <table  cellspacing="1" border="0" width="100%">
        <tr>
          <td class="label" colspan="10" width="600px">Hand #<?=$hand_id?> <span style="float:right"><?=$table_general_details["res_date"]?></span></td>
        </tr>
        <tr>
          <td class="label">Table:</td><td><?=$game_general_details["tbl_id"] . " - " . $game_general_details["gam_name"]?></td>
        </tr>
        
        
      </table>
  
      <?php
      
      $sql = "SELECT
                ctr_amount,
                ctr_pun_id,
                pun_username,
                ctr_notes
    		       FROM
    		         customer_transaction
    		       JOIN 
    		         punter on pun_id = ctr_pun_id
    		       WHERE 
    		         ctr_res_id=$hand_id AND ctr_pun_id = " .$_GET["uid"] ."  AND ctr_id = ". $_GET["gmn"] ;
    
      $rs  = $dbh->exec($sql);
      
      $num_rows = $rs->getNumRows();
      
      $rs2 = array();
      if($num_rows > 0){
        
        //blocca club indiscreti
        if($_SESSION["jurisdiction_class"] != "casino"){
            $my_player_found = false;
            while($rs->hasNext()){
              $row = $rs->next();
              if($row["pun_betting_club"] == $_SESSION["jurisdiction_id"]){
                $my_player_found = true;
                break;
              }
            }
            
            if(!$my_player_found){
              dieWithError("No details");
            }
            $rs->resetRecPtr();
        }
        //fine blocca club indiscreti
        
        while ($rs->hasNext()){
          $row = $rs->next();
          //$row['pre_bet_string'] = explode(";" $row['pre_bet_string']); 
          array_push($rs2, $row);
        }
        
        $cols_arr    = array("Username", "Amount", "Bet For Line");
        
        /*if($_SESSION["jurisdiction_class"] == "casino"){
          array_push($cols_arr, "IP");
        }*/
        
        $val_fmt_arr = array (
        'Username'      =>'return getClassLink(refPage("customers/customer_view&customer_id={$rec["pun_id"]}"), $rec["pun_username"], (($rec["pre_win"] > 0)?("winner"):(null)), "_blank");',
        'Amount'           =>'return getInDollars($rec["ctr_amount"]);',
        'Bet For Line'  =>'return GameTranscript::getRouletteBet('.$hand_id.','.$_GET["uid"] .','.$_GET["gmn"].');',
        'IP'            =>'return checkIp($rec["pre_pun_ip"]);'
        
        );
        
        $rs = New RecordSet($rs2);
        
        $qp = new QueryPrinter($rs);
        $qp->printTable($rs, $cols_arr, "", $val_fmt_arr, $cell_fmt_arr, 0, 0, "", $width="100%", 1, 2000, true);
      }
      ?>
    </div>
    <?php
  }else{
    addError("", $lang->getLang("No hand %,",$hand_id));
    showErrors();
  }

  ?>
  <table width="100%">
    <tr>
      <td class="label">Transcript</td>
    </tr>
    <tr>
      <td class="content"><?=$transcript?></td>
    </tr>
    
  </table>
  <?
  
  return;
}

?>
</body>
</html>