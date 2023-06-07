<?php
    include_once 'JurisdictionsList.class.inc';
	//Totem Montly Activity
	$jur_id = $_REQUEST['jurid'];
	$jur_class=$_REQUEST['jurclass'];
    $jurClass = JurisdictionsList::getInstance ( $dbh );
	$actualMonth = date("F");
	
// 	if($jur_class=="casino")
// 	{
// 		$jur_class='nation';
// 	}
// 	elseif ($jur_class=='nation')
// 	{
// 		$jur_class='region';
// 	}
// 	elseif ($jur_class=='region')
// 	{
// 		$jur_class='district';		
// 	}
// 	else
// 	{
// 		$jur_class='club';
// 	}
	$clubsid=$jurClass->getChilJurisdictionsIdList($jur_id, 'club' );	
	function multi_implode($array, $glue) {
    $ret = '';
    	foreach ($array as $item) {
        	if (is_array($item)) {
	            $ret .= multi_implode($item, $glue) . $glue;
    	    } else {
    	    	$ret .= $item . $glue;
        	}
    	}    	
    	$ret = substr($ret, 0, 0-strlen($glue));
    return $ret;
	}
	$clubs=multi_implode($clubsid,",");
     
?>
<script type="text/javascript">
<!--
//    var actualMonth = new Date().getMonth();
//    var monthNames = [ "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" ];

//$(document).ready(function(){
//	makeRequest(actualMonth+1);
//});
    
function makeRequest(datestart,dateto)
{
	$.post("totem/totem_query_service.inc",{datestart:+datestart,dateto:+dateto,action:"0",clubs:"<?php echo $clubs?>",jurid:"<?=$jur_id?>",jurclass:"<?=$jur_class?>"}, function(data){
 	 $("#month").fadeOut("slow").fadeIn("slow").empty().append( data );
	});
};

	/*function prevMonth(){
		if(actualMonth > 0){			
			var monthLabelId = actualMonth - 1; 
			actualMonth = monthLabelId;
			makeRequest(actualMonth+1);
		}
		$("#month_label").html(monthNames[monthLabelId]);		
	}
	function nextMonth(){
		var dates = new Date();
		if(actualMonth < dates.getMonth()){
			var monthLabelId = actualMonth + 1; 
			actualMonth = monthLabelId;
			makeRequest(actualMonth+1);
		}
		$("#month_label").html(monthNames[monthLabelId]);
	}
	*/
//-->
</script>

<script type="text/javascript">
  $(function() {
        $( "#date_start" ).datepicker({            
            changeMonth: true,
            numberOfMonths:1,
            onSelect: function( selectedDate ) {
                $( "#date_end" ).datepicker( "option", "minDate", selectedDate );
                $( "#date_end" ).datepicker( "option", "maxDate", "+1M");
            }
        });
        $( "#date_end" ).datepicker({
            changeMonth: true,
            numberOfMonths:1,
            onSelect: function( selectedDate ) {
                $( "#date_start" ).datepicker( "option", "maxDate", selectedDate );
            }
        });
    });
</script>
<div style="text-align:center;"><h3><?=$lang->getLang("Activity Report")?></h3></div>
<div style="text-align:center;">
<table style="margin:auto">
 <!-- <td onclick="prevMonth()" width="50px"><img src="<?=secure_host?>/media/images/left.gif" style="width:15px;"></td>
      <td><h4 id="month_label"><?=$actualMonth?></h4>
      <td onclick="nextMonth()" width="50px"><img src="<?=secure_host?>/media/images/right.gif" style="width:15px;"></td>
       -->
     	<form action="<?=$_SERVER['HTTP_REFERER']?>" method="POST" name="search">
     	<tr>
		<input type="hidden" name="do_search" value="1" />			 
		  		<td class="label"><?=$lang->getLang('From')?>:</td>
				<td class="content"><input type="text" id="date_start" name="date_start"  value="<?=$date_start?>" /></td>
		</tr>
		<tr>
				<td class="label"><?=$lang->getLang('Until')?>:</td>
				<td class="content">
 				<input type="text" id="date_end" name="date_end"  value="<?=$date_end?>" /></td>
		</tr>
		<tr><td></td></tr>
   		</form>
   		<tr><td colspan="2"><button id="submit"><?=$lang->getLang("View")?></button>
   					<script>
						 $(function(){
						    $('#submit')
					            .button()
					            .click(function() {
					            	$.post("totem/totem_query_service.inc",{date_start:$('input:text[name=date_start]').val(),date_end:$('input:text[name=date_end]').val(),action:"0",clubs:"<?= $clubs?>",jurid:"<?=$jur_id?>",jurclass:"<?=$jur_class?>"}, function(data){			
					            		 $("#month").fadeOut("slow").fadeIn("slow").empty().append( data );					            		 
						            	});
						            });
							return false;
							});
					</script>
   			</td>
		</tr>           
  </table>
  <div class="tip"id="duration"></div>
   <div style="text-align: center;" id="month">  	
  </div>  
</div>