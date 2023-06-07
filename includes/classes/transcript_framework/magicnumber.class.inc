<?php
  require_once 'Transcript.class.inc';
  require_once 'transcript.inc.php';
  class magicnumber extends Transcript
  {
      function magicnumber(){
      }
    
      public function getNames(){
      return "magic number";
      }
      
      function getBet()
      {
         $betData = split(";" , $this->betString);
         $html = "";
         
      //   if($betData[1] == 0){
      //   $html .= "<b>PLAY IN FREESPIN</b>";        
      //   }else{
      //   $html .= "PAY: <b>" . getInDollars($betData[1]/100) . "</b> FOR LINE <br>";
      //   $html .= "TOTAL LINE: <b>" . number_format($betData[0] / $betData[1],0) . " </b>"; 
      //  	}
         return ""; 
      }
      
      function getReadbleTranscript()
      {      	
        /* 
         	6@25@p1=S|2;C|1;D|12;C|12;S|3~0;0;1;1;0~0#        
        	1@p1=0;0;1;1;0@p1=S|6;C|7;D|12;C|12;D|4~0;0;1;1;0~25# 
        	3@p1=0;0;1;1;0@p1=H|3;H|8;D|12;C|12;S|2~0;0;1;1;0~50#
         	4@p1=0@p1=S|5~100 
         	
         	6@0@p1=D|2;S|8;C|11;D|10;D|9~0;0;0;0;0~0;5;0;0;0~0#
         	1@p1=0;1;1;1;1@p1=P|0;S|8;C|11;D|10;D|9~1;1;1;1;1~5;5;0;0;0~110
         	         	
         	6@100@p1=C|10;D|10;S|9;C|1;H|12~0;0;0;0;0~0#
         	1@p1=1;1;0;0;0@p1=C|10;D|10;P|0;P|0;H|8~1;1;1;1;0~300#
         	5@p1=1@C|11;C|6;H|8;S|7;D|1
         */	
        settype($this->betString, "string");
        $result = new StringTokenizer($this->betString, "#");
        $count = $result->countTokens();
        $i = 0;
        $handList = array();
        while($hand = $result->nextToken())
        {        	
        	if($i < ($count))
        	{
            list($operationid,$bet,$hands)=explode("@", $hand);
            if($operationid==4 || $operationid==5)
            {
            	list($cards,$win) = explode("~", $hands);
            }
            else
            {
             list($cards,$hold,$win) = explode("~", $hands);
             $holdList  = split(";", $hold);            	
            }
            $cardsList = split(";", $cards);
            for($z =0 ; $z < count($cardsList); $z++)
            {
           		if($z==0)
            	{
            		$cardsList[0]=substr($cardsList[0], 3);
            	}
             	list($cardType, $cardValue) = split("\\|", $cardsList[$z]);
              	$cardsList[$z] = getCard(strtolower(trim($cardValue)."".trim($cardType)));
            }           
            array_push($handList, array("operationid"=>$operationid,"bet"=>$bet,"win"=>$win,"cards"=>$cardsList,"hold" =>$holdList));
         $i++;
        }
	    $html = "<div class='container " . $this->path . "'>";
        $html .= "<div class='box " . $this->path . "'>";
        $html .= "<table rules=rows>";                          
        for($i = 0; $i < count($handList); $i++)
        {
		   $hand = $handList[$i];
		   $wins = $hand["win"]/100;
		   $str = "";
		   $html .= "<tr><td><table cellspacing=7 cellpadding=2 >";
       		switch($i)
       		{
			case 0:
	            	{
		               $str = "First Hand";
		               switch($hand["operationid"])
		               {
			             	case 1:
			               		{
			               			$str .=" - Add ";
			               		}
			               	break;
			               		
			               	case 3:
			               		{
			               			$str .=" - Retry ";
			               		}
			               	break;
			               		
			               	case 4:
			               		{
			               			$str .=" - Double ";
			               		}
			               	break;
			               		
			               	case 6:
			               		{
			               			$str .=" - Bet ";
			               		}
			               	break;
			            }		            	
	            	}
	            break;
	            
	            case 1:
	               $str = "<br/>Second Hand";
	               switch($hand["operationid"])
	               {
	               		case "1":
	               			$str .=" - Add ";
	               		break;
	               		
	               		case "3":
	               			$str .=" - Retry ";
	               		break;
	               		
	               		case "4":
	               			$str .=" - Double ";
	               		break;
	               		
	               		case "6":
	               			$str .=" - Bet ";
	               		break;
	               	}
	            	
	            break;
                
	            case 2:
	               if($hand["operationid"]==3 )
	               {
	                 $str = " <br/> Retry";
	               }
	               else
	               {
	                 $str = " <br/> Double";
	               }
	            break;
	             case 3:
	               {
	                 $str = " <br/> Double";
	               }
	            break;
	            
	            case 4:
	               {
	                 $str = " <br/> Double ";
	               }
	            break;
	            
	           case 5:
	               {
	                 $str = " <br/> Double fourth time";
	               }
	            break;
	            
	            default:
	               $str = " <br/> Double";
	            break;	            
	           }
	       $html .= "<th colspan='5'>$str</th><tr>";
	       $cards = $hand["cards"];
           $hold  = $hand["hold"];           
           for($z = 0; $z < count($cards); $z++)
           {
           	$html .= "<td";
              if($hold[$z] == 1)
              {
               if ($wins !='0')
               {
               	$html .=" class='wincard'";
               }
               else 
               {
              	$html .= " class='hold'";
               }
              }
              $html .= ">$cards[$z]</td>";              
           }
           $html .="</tr></table></td>"; 
           $html .= " <td>";
          	if ($hand["operationid"]==6)
        	  {
	          	$hand["bet"]=$hand["bet"]/100;  
	          	$_SESSION['bet']=$hand["bet"];
	          	$html .="<b><br>Bet: ".$hand["bet"]."&#8364; <b><br/>";	
         	 }
           	if ($hand["operationid"]==4)
           	{
           		$hand["bet"]=substr($hand["bet"], 3);
           		$hand["bet"]=$hand["bet"]/100;           		
				if($hand["bet"]=="0")
				{
					$html .="<br /><b>Bet: Black<br><br>Win: ".$wins."&#8364; </b>";
				}				
				else 
				{
					$html .="<br /><b>Bet: Red<br><br> Win: ".$wins."&#8364; </b>";
				}
           	}
       		 elseif($hand["operationid"]==3)
				{
					$html .="<br><b><br>Bet: ".$_SESSION['bet']."&#8364; <b><br/><br/>Win: ".$wins."&#8364; </b>";
					$_SESSION['bet']="";
				}
        	 elseif($hand["operationid"]==5)
				{
        		$hand["bet"]=substr($hand["bet"], 3);
				switch($hand["bet"])
	               {
	               		case "0":
	               			$html .="<b> Bet: Clubs</b>";
	               		break;
	               		
	               		case "1":
	               			$html .="<b>Bet: Diams</b>";
	               		break;
	               		
	               		case "2":
	               			$html .="<b> Bet: Spades</b>";
	               		break;
	               		
	               		case "3":
	               			$html .="<b>Bet: Hearts</b>";
	               		break;
	               	}
	               	$html .="<b><br/><br/>Win: ".$wins."&#8364; </b>";
					$_SESSION['bet']="";				
				}				
			else
           		{
        	   	$html .="<br /><b>Win: ".$wins."&#8364;</b>";
           		}
           $html .= "</td>";
        }
        }       
        $html .= "</tr></table>";
        return $html;           
      }      
      }
?>