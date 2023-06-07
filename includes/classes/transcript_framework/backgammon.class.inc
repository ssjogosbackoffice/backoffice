
<?php
require_once 'Transcript.class.inc';
  require_once 'transcript.inc.php';


  class backgammon extends Transcript
  {

    public $initialPositions=array();

    public $startPositions=array(array(
    6=>5,
    8=>3,
    13=>5,
    24=>2
    ),array(
    1=>2,
    12=>5,
    17=>3,
    19=>5
    ));

    public $moves=array();


  	function backgammon(){
          $this->setPath('backgammon');
    }

  	function getBet() {

	/*   print_r("betstring=>".$this->betString."<br />");
	     print_r("resstring=>".$this->resString."<br />");
	    print_r("resultString=>".$this->resultString."<br />");*/



        $html="";

		return $html;
  	}


    function getReadbleTranscript()
    {
    globalise('uid');
    global $uid;
      $playersRes=explode('@',$this->resString);
      $players=explode(';',$playersRes[0]);
      $playersRes[1]=explode(';',$playersRes[1]);
      $playerOneWin=explode('-',$playersRes[1][0]);
      $playerTwoWin=explode('-',$playersRes[1][1]);
      $playerOne=explode(',',$players[0]);
      $playerTwo=explode(',',$players[1]);
      $this->moves[0]=$this->startPositions;
      $html = "<div class='container " . $this->path . "'>";
      $html.="<button class='entitybtn fleft' style='font-size: large'> View detailed</button>";
      $html.="<table class='centered' style='width: 300px;margin:auto;'><tr>
				<td class='large'><img src='/media/images/user.png' style='  width:13px' /> ".$playerOne[3]."<br /> Available credit: ".getInDollars($playerOne[4]/100)."<br />".($playerOneWin[1]!='surrender'? 'Win: '.getInDollars($playerOneWin[1]/100):'surendered')."</td></tr>";
      $html.="<tr><td><div class='backgammonContainer fleft' style='width: 300px;height: 250px'></div> </td></tr>";
      $html .="<tr><td class='large'><img src='/media/images/user.png' style='  width:13px' /> ".$playerTwo[3]."<br /> Available credit: ".getInDollars($playerTwo[4]/100)."<br />".($playerTwoWin[1]!='surrender'?'Win: '.getInDollars($playerTwoWin[1]/100):'surendered')."</td></tr></table>


      ";

        $html.="<div class='label hidden details' style='padding:2px'>Hand details</div><br /><br />";
        $html .="<table class='hidden details'><tr><td>
                <table class='beautify beautify-hover' style='width: 500px' border='1px' cellpadding='5px' cellspacing='0px'>";
        $html.="<thead><tr><th class='label'>Movement Nr</th><th class='label'>Username</th><th class='label' style='width: 65%'>Movement</th><th class='label'>Dices</th></tr></thead>";
    	settype($this->resString, "string");
    		$this->betString = explode ( "@", $this->betString );
		list($subCall,$allBets)=explode('!',$this->betString[1]);
		$allBets=explode('|',$allBets);

		 $allMoves=array();
		if($uid==$playerOne[1]){
		$playerOneMoves=$this->getOpponentMoves($playerTwo[1],$playerTwo[3]);
			 foreach($playerOneMoves as $k=>$v ){
			 list($position,$movements,$dices)=explode('-',$v);
			 $allMoves[$position]=$movements."-".$dices."-".$playerTwo[3];
			}
			foreach($allBets as $k=>$v ){
			 list($position,$movements,$dices)=explode('-',$v);
			 $allMoves[$position]=$movements."-".$dices."-".$playerOne[3];
			}

		}
		elsE{
		$playerOneMoves=$this->getOpponentMoves($playerOne[1],$playerOne[3]);
			 foreach($playerOneMoves as $k=>$v ){
			 list($position,$movements,$dices)=explode('-',$v);
			 $allMoves[$position]=$movements."-".$dices."-".$playerOne[3];

			}
			foreach($allBets as $k=>$v ){
			 list($position,$movements,$dices)=explode('-',$v);
			 $allMoves[$position]=$movements."-".$dices."-".$playerTwo[3];
			}
		}

	   ksort($allMoves);
		$bets=array();
		foreach($allMoves as $k=>$v){
		    list($movements,$dices,$username)=explode('-',$v);
		     $bets[$k]=explode(';',$movements);
		    foreach($bets[$k] as $mvm => $mv){
		    	 $mv=explode(',',$mv);
                 if($username==$playerOne[3]){
		          $mv[0]=25-$mv[0];
		          $this->startPositions[1][$mv[0]]--;
		           if(isset($mv[1])){
		          	$mv[1]=25-$mv[1];
		          	$this->startPositions[1][$mv[1]]++;
		          }

		          if($this->startPositions[0][$mv[1]]>0){
		         		$this->startPositions[0][$mv[1]]=0;
		          }

		          $newMovements[$k[$mvm]]=implode(',',$mv);


                  }
		         else{
		            if(strtolower($mv[0])!='surrender'){
		              $this->startPositions[0][$mv[0]]--;
		          	  if(isset($mv[1])){
		            	$this->startPositions[0][$mv[1]]++;
		          		}
		              if($this->startPositions[1][$mv[1]]>0){
		         		$this->startPositions[1][$mv[1]]=0;
		          	  }
		            }
		         }
		    }

		    if($username==$playerOne[3]){

		      $playerOneMoves=implode(';',$newMovements);
		      $this->moves[$k]=array($playerOneMoves,$this->startPositions,$dices,$username);
		    }
		    else{

		    $this->moves[$k]=array($movements,$this->startPositions,$dices,$username);
		    }

		}

		foreach($this->moves as $k=>$v){
		if($k!=0){
            $bets[$k]=explode(';',$v[0]);
		    $html.="<tr><td class='centered movementNumberContainer' style='cursor:pointer' data-movement='".$k."'>".$k."</td><td>".$v[3]."</td><td>";

		    foreach($bets[$k] as $mvm => $mv){

		          $mv=explode(',',$mv);
		          if(strtolower($mv[0])=='surrender' || strtolower($mv[0]=='nomessage') ){
		            $html.="Surrendered";
		          }
		          else{
                    if(strtolower($mv[0])=='none'){
                      $html.="<span>No movement available</span>";
                    }
                    else{
		            $html.="<span class='movementDescription' data-type='".($v[3]==$playerOne[3]?' black':'')."' data-movement='".$k."' data-from='".$mv[0]."'' data-to='".$mv[1]."' > Moved a chip from position ".$mv[0]." to ".$mv[1]."</span><br >";
		            }
		          }
		    }
		    $html.="</td><td>";
		    $dices=explode(',',$v[2]);
            $html.="<img src='/media/images/handimages/dices/".$dices[0].".png' style='  width:30px' />";
            $html.="<img src='/media/images/handimages/dices/".$dices[1].".png' style=' width:30px' />";
		    $html.="</td></tr>";
            }

		}
        $html.="</table></td><td style='vertical-align: top'>";
		$html.="<div align='center' id='tableInfo' class='backgammonContainer'  >";
        $html.= "<table style='position:absolute;top:23px;width:auto;height=100%;color:white;left:21%;height: 400px;'>";
         $header="<tr>";
         $topContent="<tr>";
         $bottomContent="<tr>";
         $footer="<tr>";
         for($i=13;$i<=24;$i++){
            if($i==19){
                $header.="<td style='width:20px;height: 45px'></td>";
                $topContent .="<td style='width:20px;'></td>";
                $bottomContent .="<td style='width:20px;'></td>";
                $footer.="<td style='width:20px;vertical-align: bottom'></td>";
            }
            $header.="<td><span class='fleft' >".$i."</span></td>";
            $topContent.="<td style='vertical-align: top' ><div class='chipsContainer' id='".$i."'><div class='backgammonChip'></div></div></td>";
            $bottomContent.="<td style='vertical-align: bottom'><div class='chipsContainer'  id=".(25-$i)."><div class='backgammonChip black'></div></div></td>";
            $footer.="<td   style='vertical-align: bottom;height: 35px'><span class='footer' style='margin-left:2px;margin-right:5px;' >".(25-$i)."</span></td>";
         }
        $header.="</tr>";
        $topContent.="</tr>";
        $bottomContent.="</tr>";
        $footer.="</tr>";
        $html.=$header.$topContent.$bottomContent.$footer;
        $html.="</table></td></tr></table></div>";


         $html.="

 <script type='text/javascript'>
    	$(function(){
    	 $(window).scroll(function() {
        var distanceFromTop = $(document).scrollTop();
        if (distanceFromTop >= 600)
        {
            $('#tableInfo').addClass('fixed');
        }
        else
        {
            $('#tableInfo').removeClass('fixed');
        }
    		});


    	$('.entitybtn').on('click',function(){
    	  $('.details').toggleClass('hidden');
    	});
    	var moves=".json_encode($this->moves)."
    	 $('.movementNumberContainer').on('click',function(){
    	 buildMovementTable(moves[$(this).data('movement')][1]);
    	 });
    	 $('.movementDescription').on('click',function(){

    	   $('.backgammonChip').removeClass('blink_me blinkTo ');
    	    buildMovementTable(moves[$(this).data('movement')][1]);

    	    from=$(this).data('from');
    	    to=$(this).data('to');
    	     $('#'+from).append('<div class=\'backgammonChip '+$(this).data('type')+' \'></div>');
    	     if($('#'+to).children().length==0){
    	      $('#'+to).append('<div class=\'backgammonChip '+$(this).data('type')+' \'></div>');
    	      }
    		    if(from>=13){
    	     $('#'+from+' >div:last-child').addClass('blink_me');
    	    }
    	    else{
    	     $('#'+from+' >div:first-child').addClass('blink_me');
    	    }

    	    if(to >= 13) {
    	     $('#'+to+' >div:last-child').addClass('blinkTo');
    	    }
    	    else{
    	     $('#'+to+' >div:first-child').addClass('blinkTo');
    	    }
    	 });
    	 buildMovementTable(".json_encode($this->moves[0]).")
    	})

    	function buildMovementTable(tableInfo){
        $('.chipsContainer').empty();
          $.each(tableInfo , function(k, value) {
          $.each(value , function(i, val) {
            for(j =0; j < val; j++){
                 $('#'+i).append('<div class=\'backgammonChip '+(k>0?'black' : '')+'  \'></div>');
            }
            });
            });
    	}
    	</script> ";
    	return $html;
    }


    public function getOpponentMoves($oppoentId,$oppenentUsername){
      global $dbh,$hand_id;
      $oppmoves= $dbh->queryOne("Select pre_bet_string from punter_result where pre_res_id =".$dbh->escape($hand_id)." AND pre_pun_id=".$dbh->escape($oppoentId));
      $oppmoves=explode('!',$oppmoves);

     	$allBets=explode('|',$oppmoves[1]);
/*		$bets=array();
		foreach($allBets as $k=>$v){

		    list($position,$movements,$dices)=explode('-',$v);
		    $bets[$position]=explode(';',$movements);
		    foreach($bets[$position] as $mvm => $mv){
		          $mv=explode(',',$mv);
		          $mv[0]=25-$mv[0];
		          $mv[1]=25-$mv[1];

		          $this->startPositions[1][$mv[0]]--;
		          $this->startPositions[1][$mv[1]]++;
		          $newMovements[$mvm]=implode(',',$mv);

		    }
		    $playerOneMoves=implode(';',$newMovements);
		    $this->moves[$position]=array($playerOneMoves,$this->startPositions,$dices,$oppenentUsername);
		}*/
    return $allBets;
    }
  }

?>

