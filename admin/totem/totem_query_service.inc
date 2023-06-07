<style>
    #dialog label, #dialog input { display:block; }
    #dialog label { margin-top: 0.5em; }
    #dialog input, #dialog textarea { width: 95%; }
    #tabs { margin-top: 1em; }
    #tabs li .ui-icon-close { float: left; margin: 0.4em 0.2em 0 0; cursor: pointer; }
    #add_tab { cursor: pointer; }
</style>
<script type="text/javascript">
var content = "";
var title="";
var tabTemplate = "<li><a href='#{href}'>#{label}</a> <span class='ui-icon ui-icon-close'>Remove Tab</span></li>";
var tabCounter = 7;
var tabs = $( "#tabs" ).tabs();

function toggle(id)
		{
		    if ($('#'+id).is(":hidden")) {
				$('.ttm_details').slideUp('fast');
				$('#'+id).slideDown('fast');
			} else {
				$('#'+id).slideUp('fast');
			}
			return false;
		}

function hide(details)
		{
			$('.'+details).slideUp('fast');
			return false;
		}

function showId(id)
{
		$('#'+id).slideDown('fast');
		return false;
}

function duration(duration,divId)
		{
			$('#'+divId).empty().append('<?=$lang->getLang("Time taken to execute your request")?>: '+duration);
		}

function openModifyTotem(totem_code){
		$.get("totem/totem_modify_individual.inc?ttcode="+totem_code, function(data) {
		totemContent= data;
		addTab(totem_code,totemContent,'modify');
		$('#detailcontent').animate({scrollTop: 0}, 800);
    	});
}

function openConnectTotem(totem_code){
		$.post("totem/totem_connection_individual.inc?ttcode=",{totem:totem_code}, function(data) {
		totemContent= data;
		addTab(totem_code,totemContent,'administrate');
		$('#detailcontent').animate({scrollTop: 0}, 800);
		});
}

function openLogTotem(totem_code){
		$.get("totem/totem_log_individual.inc?ttcode="+totem_code, function(data){
		totemContent= data;
		addTab(totem_code,totemContent,'log');
		$('#detailcontent').animate({scrollTop: 0}, 800);
		});
}

function openGamesTotem(totem_code){
	if($('#games').length){
	return false;
	}
	else {
		$.get("totem/totem_games.inc?ttcode=" + totem_code, function (data) {
			totemContent = data;
			addTab(totem_code, totemContent, 'games');
			$('#detailcontent').animate({scrollTop: 0}, 800);
		});
	}
}

function openTransactionTotem(totem_code){
		$.get("totem/totem_transtrack_individual.inc?ttcode="+totem_code, function(data) {
	    totemContent= data;
		addTab(totem_code,totemContent,'transactions');
		$('#detailcontent').animate({scrollTop: 0}, 800);
	    });
}

function openResetPass(totem_code){
	$.get("totem/totem_modify_pass.inc?ttcode="+totem_code, function(data) {
	totemContent= data;
	addTab(totem_code,totemContent,'settings');
	$('#detailcontent').animate({scrollTop: 0}, 800);
	});
}

function addTab(title,content,tabtitle) {
        var tabTitle=tabtitle;
        var tabContent=content;
        var label = title+"-"+tabTitle || "Tab " + tabCounter,
            id = "tabs-" + tabCounter,
            li = $( tabTemplate.replace( /#\{href\}/g, "#" + id ).replace( /#\{label\}/g, label ) ),
            tabContentHtml = tabContent|| "Tab " + tabCounter + " content.";
        	tabs.find( ".ui-tabs-nav" ).append( li );
        	tabs.append( "<div id='" + id + "'><div class='totem_log'>" + tabContentHtml + "</div></div>" );
        	tabs.tabs( "refresh" );
        	tabCounter++;
    }

$("#tabs").delegate("span.ui-icon-close",  "click", function() {
        var panelId = $( this ).closest( "li" ).remove().attr( "aria-controls" );
        $( "#" + panelId ).remove();
        tabs.tabs( "refresh" );
});

function getTotemDetails(id, jur_class)
	{
		$('#detail_'+id).empty().load('totem/totem_query_service.inc',{action:'2',jurid:id,jurclass:jur_class});
	}


function getTotemClosure(id, jur_class)
{
    $('#closure_'+id).empty().load('totem/totem_query_service.inc',{action:'11',jurid:id});
}

function TotemsDeposit(id, jur_class)
{
	$('#deposit_'+id).empty().load('totem/totem_query_service.inc',{action:'7',jurid:id,jurclass:jur_class});
}




function getClubDetails(id, jur_class)
{
	$.post("totem/totem_club_transtrack.inc",{do_search:'1',totemClub:id,jur_class:jur_class,date_club:$('input:text[name=date_club]').val()}, function(data){
		$('#detail_club_'+id).empty().append( data );
		});
}



function totemReset(totem,status,club,jurisdiction){
	$(function() {
        $( "#reset-confirm" ).dialog({
            resizable: false,
            height:140,
            modal: true,
            buttons: {
                "Yes": function() {
                    $( this ).dialog( "close" );
                    $.post("totem/totem_modify_individual.inc", { ttm_act_status:status,ttcode: totem, reset:'1' } );
					getTotemDetails(club,jurisdiction);
                },
                Cancel: function() {
                    $( this ).dialog( "close" );
                }
            }
        });
    });
	}


function switchToEdit(whichid,editterid,img)
{
    var which = document.getElementById(''+whichid);
    which.style.display = 'none';
    img.style.display = 'none';
    var editter = document.getElementById(''+editterid);
    editter.firstChild.value = which.innerHTML;
    editter.style.display = 'block';
}
function switchBack(containerid,save)
{
    var viewer = document.getElementById(containerid+'_text');
    var editter = document.getElementById(containerid+'_edit');

    if(save){
        viewer.innerHTML = editter.firstChild.value;
        $.post("reports_casino/partial_closure_service.inc",{action:'5',pid:containerid,pval:editter.firstChild.value}, function(data){
        });
    }
    editter.style.display = 'none';
    viewer.style.display = 'block';
    $('#'+containerid+'_img').show();
}




/*function resetClub(club){

	$(function() {
        $( "#resetClub" ).dialog({
            resizable: false,
            height:140,
            modal: true,
            buttons: {
                "Yes": function() {
                    $( this ).dialog( "close" );
                	$.post("totem/totem_query_service.inc",{action:'5',club:club});
                },
                Cancel: function() {
                    $( this ).dialog( "close" );
                }
            }
        });
    });
	}
*/

function shutdowntotem(totem)
	{
		$.post("totem/totem_connection_individual.inc",{totem:totem,socket_send:'1',restart:'1'});
	}


function changeAccess(totem,access,club,jurisdiction){

	$(function() {
        $( "#dialog-confirm" ).dialog({
            resizable: false,
            height:140,
            modal: true,
            buttons: {
                "Yes": function() {
                    $( this ).dialog( "close" );
                    if(access=='ALLOW'){
                	$.post("totem/totem_connection_individual.inc",{totem:totem,socket_send:'1',denyon:'1'});
                	getTotemDetails(club,jurisdiction);
                    }
                    else{
                    	$.post("totem/totem_connection_individual.inc",{totem:totem,socket_send:'1',allowon:'1'});
                    	getTotemDetails(club,jurisdiction);
                    }
                },
                Cancel: function() {
                    $( this ).dialog( "close" );
                }
            }
        });
    });
	}



function showCloseAll(){
$('.closureeach').hide();
    $('.closureall').show();
}
function showCloseEach(){
    $('.closureall').hide();
    $('.closureeach').show();
}

</script>

<?php
$action = $_POST['action'];
switch($action)
{
	case '0':
		$jurid=$_POST['jurid'];
		$jurclass=$_POST['jurclass'];
		$date_start=$_POST['date_start'];
		$date_end=$_POST['date_end'];
		monthQuery($jurid,$jurclass,$date_start,$date_end);
	break;

	case '1':
		$club=$_POST['clubs'];
		$type=$_POST['type'];
		getTotemList($club,$type);
	break;

	case '2':
		$jur_id=$_POST['jurid'];
		$jur_class=$_POST['jurclass'];
		getDetails($jur_id,$jur_class);
	break;

	case '3':
		$date_start=$_POST['date_start'];
		$date_end=$_POST['date_end'];
		$jur_id=$_POST['jurid'];
		$jur_class=$_POST['jurclass'];
		getTotemDetails($jur_id,$jur_class,$date_start,$date_end);
		break;

	case '4':
		$jur_id=$_POST['jurid'];
		$jur_class=$_POST['jurclass'];
		getTotems($jur_id, $jur_class);
		break;

	case '5':
		$club=$_POST['club'];
		global $dbh;
		$sql="SELECT * FROM totem WHERE ttm_club_id=$club";
		$result=$dbh->exec($sql);
		if(!$result->hasNext()){
			$html='No totem was modified';
		}
		else {
			while($result->hasNext()){
				$row = $result->next();
				?>
					<script type="text/javascript">
					shutdowntotem('<?=$row['ttm_code']?>');
					</script>
					<?
				}
				$html='Reset succesfully';
		}

		return $html;

		break;


	case '6':
    	{
    			$club=$_POST['clubs'];
    			getClubTransactions($club);
   		}
	break;


	case '7':
		$jur_id=$_POST['jurid'];
		$jur_class=$_POST['jurclass'];
		getDetailsDeposit($jur_id,$jur_class);

	break;



	case '8':
		$totem=$_POST['totemcode'];
		$user=$_POST['user'];
		$amount=$_POST['credits'];
		$totemMD5=$_POST['totemmd5'];
		$agid=$_POST['aid'];
		doTotemDeposit($totem,$user,$amount,$totemMD5,$agid);
		break;


	case '9':
		{
			$jur_id=$_POST['jurid'];
			getAgencyGames($jur_id);
		}
		break;

	case '10':
		{
			$license=$_POST['license'];
			$jur_id=$_POST['jurid'];
			cryptLicense($license,$jur_id);
            break;
		}
    case '11':{
        $jur_id=$_POST['jurid'];
        getTotemClosure($jur_id);
        break;
    }
    case '12':{
        $totemcode=$_POST['tc'];
        $totem=$_POST['tid'];
        $date_start=$_POST['date_start'];
        $date_end=$_POST['date_end'];
        $casinoperc=$_POST['cap'];
        $nationid=$_POST['nid'];
        $nationperc=$_POST['np'];
        $regionid=$_POST['rid'];
        $regionperc=$_POST['rp'];
        $districtid=$_POST['did'];
        $districtperc=$_POST['dp'];
        $clubid=$_POST['cid'];
        $clubperc=$_POST['cp'];
        calculateTotemClosure($totemcode,$totem,$date_start,$date_end,$casinoperc,$nationid,$nationperc,$regionid,$regionperc,$districtid,$districtperc,$clubid,$clubperc);
        break;
    }
    case '13':{
        $tid=$_POST['tid'];
        $date_start=$_POST['date_start'];
        $date_end=$_POST['date_end'];
        $dep=$_POST['dep'];
        $ww=$_POST['ww'];
        $fin=$_POST['fin'];
        $note=$_POST['note'];
        $nid=$_POST['nid'];
        $rid=$_POST['rid'];
        $did=$_POST['did'];
        $cid=$_POST['cid'];
        $cp=$_POST['cp'];
        doTotemClosure($tid,$date_start,$date_end,$dep,$ww,$fin,$cp,$note,$nid,$rid,$did,$cid);
        break;
    }
    case '14':{
        $jur_id=$_POST['jur_id'];
        $date_end=$_POST['date_end'];
        getTotemClosureByClub($jur_id,$date_end);
        break;
    }


    case '15':
    {
        $postsql=$_POST['sql'];
        $postsql=explode("@",$postsql);
        foreach($postsql as $k=>$v){
           $values=explode(',',$v);
           $values[11]=str_replace("^^^ca:", $_POST[$values[10]]."^^^ca:", "$values[11]");
            $v=implode(',',$values);
           $postsql[$k]=$v;
        }
        $postsql=implode(',',$postsql);
        doTotemClosureByCub($postsql);
        break;
    }


    case '16':
    {
       if(isset($_POST['cashin'])){
        getTotemClosureIn();
       }
        if(isset($_POST['cashout'])){
            getTotemClosureOut();
        }
       break;
    }
}

function getTotemAction($TotemCode,$access,$totemclub,$jurclass,$status) {

			global $dbh,$lang;
			$modify_totem = check_access ( 'modify_totem' );
			$log_totem=check_access('ttm_totem_log');
			$trans_totem=check_access('ttm_totem_trns');
			$passcode_totem=check_access('ttm_totem_passcode');
			$reset_totem=check_access('ttm_totem_reset');
			$admn_totem=check_access('ttm_totem_admin');
			$htmlAction  = "";
			$htmlAction  = ($log_totem)?'<img width="20" height="20" style="cursor: pointer;" src="' . image_dir . '/icon_log.gif" title="Totem log" onclick="openLogTotem(' . $dbh->quote ( $TotemCode ) . ');" />':'';
			$htmlAction .= ($trans_totem) ? '<img width="20" height="20" style="cursor: pointer;" src="' . image_dir . '/icon_financial.gif" title="Totem Transactions" onclick="openTransactionTotem(' . $dbh->quote ( $TotemCode ) . ');" />' : '';
			$htmlAction .= ($passcode_totem)?'<img width="20" height="20" style="cursor: pointer;" src="' . image_dir . '/icon_edit_on.gif" title="Settings" onclick="openResetPass(' . $dbh->quote ( $TotemCode ) . ');" />':'';
			//$htmlAction .= ($admn_totem) ? '<img width="20" height="20" style="cursor: pointer;" src="' . image_dir . '/icon_server_connect.gif" title="Connect to totem" onclick="openConnectTotem(' . $dbh->quote ( $TotemCode ) . ');" />' : '';
			$htmlAction .= (check_access('totem_configure_games')) ? '<img width="20" height="20" style="cursor: pointer;" src="' . image_dir . '/joker.jpg" title="Totem games" onclick="openGamesTotem(' . $dbh->quote ( $TotemCode ) . ');" />' : '';
			if((strtoupper($access)=='DENY')&&($reset_totem==true))
					{
						$htmlAction .='<img src="'.image_dir.'/reset.png" width="20" height="20" style="cursor: pointer;"';
						$htmlAction .="onclick=totemReset('$TotemCode','$status','$totemclub','$jurclass'); />";
					}
		// 			else
		// 			{
		// 				$htmlAction .='<img src="'.image_dir.'/icon_lock_open.gif"	width="20" height="20" style="cursor: pointer;" />';
		// 			}

		// 	$htmlAction .= ($modify_totem) ? '<img width="20" height="20" style="cursor: pointer;" src="' . image_dir . '/icon_edit_on.gif" onclick="openModifyTotem(' . $dbh->quote ( $TotemCode ) . ');" />' : '';
		// 	$htmlAction .= ($connect_to_totem) ? '<img width="20" height="20" style="cursor: pointer;" src="' . image_dir . '/icon_server_connect.gif" onclick="openConnectTotem(' . $dbh->quote ( $TotemCode ) . ');" />' : '';

			if ($htmlAction == "") {
				$htmlAction = $lang->getLang("You don't have privileges for any operation");
			}
			return $htmlAction;
}

function monthQuery($jurid,$jurclass,$date_start,$date_end){
			 global $dbh,$lang;
    		 $sql="SELECT jps_perc_totem FROM jurisdiction_payment_settings WHERE jps_jur_id=$jurid";
			 $parentProc=$dbh->queryOne($sql);
			 if($jurclass == "club"){
			 	$params="ttm_code,c.jur_name,c.jur_id,c.jur_class,coalesce(jps_perc_totem, 0) AS percentage ";
			 	$jurisdictions="jurisdiction c
			 				  LEFT JOIN jurisdiction_payment_settings ON jps_jur_id = c.jur_id  ";
			 	$join = "WHERE	c.jur_class = 'club'
			 	            AND c.jur_id = pun_betting_club
			 	            AND pun_id = ttt_pun_id
                            AND ttt_ttm_id = ttm_id
        					 AND c.jur_id='$jurid'
		     				 GROUP BY c.jur_id";
			 }
			 if($jurclass == "district"){
			 	$params="ttm_code,c.jur_name,c.jur_id,c.jur_class,coalesce(jps_perc_totem, 0)  AS percentage";
			 	$jurisdictions="jurisdiction d, jurisdiction c 
			 					LEFT JOIN jurisdiction_payment_settings ON jps_jur_id = c.jur_id  ";
			 	 	$join = "WHERE c.jur_class = 'club'
							 AND d.jur_id = c.jur_parent_id
							  AND c.jur_id = pun_betting_club
			 	            AND pun_id = ttt_pun_id
						 	 AND d.jur_id='$jurid'
						 	 AND ttm_id = ttt_ttm_id 
			 				 GROUP BY c.jur_id";
			 }
			 elseif($jurclass=="region"){
			 	$params="ttm_code,d.jur_name,d.jur_id,d.jur_class,coalesce(jps_perc_totem, 0) AS percentage ";
			 	$jurisdictions="jurisdiction c, jurisdiction r, jurisdiction d
								  LEFT JOIN jurisdiction_payment_settings ON jps_jur_id = d.jur_id  ";
                    		$join = "WHERE c.jur_class = 'club'
			 	 			 AND d.jur_class = 'district'
			 	 			 AND r.jur_class = 'region'
			 	 			 AND r.jur_id = d.jur_parent_id
							 AND d.jur_id = c.jur_parent_id
							  AND c.jur_id = pun_betting_club
			 	            AND pun_id = ttt_pun_id
						 	 AND r.jur_id='$jurid'
			 				 AND ttm_id = ttt_ttm_id
			 				 GROUP BY d.jur_id";
			 }
			 elseif ($jurclass=="nation"){
			 	$params="ttm_code,r.jur_name,r.jur_id,r.jur_class,coalesce(jps_perc_totem, 0) AS percentage ";
			 	$jurisdictions="jurisdiction c, jurisdiction d, jurisdiction n, jurisdiction r 
                			    LEFT JOIN jurisdiction_payment_settings ON jps_jur_id = r.jur_id  ";
			 	$join = "WHERE c.jur_class = 'club'
			 	 			 AND d.jur_class = 'district'
			 	 			 AND r.jur_class = 'region'
			 	 			 AND n.jur_class = 'nation'
							 AND n.jur_id=r.jur_parent_id
			 	 			 AND r.jur_id = d.jur_parent_id
							 AND d.jur_id = c.jur_parent_id
						 AND c.jur_id = pun_betting_club
			 	            AND pun_id = ttt_pun_id
						 	 AND n.jur_id='$jurid'
			 				 AND ttm_id = ttt_ttm_id 			 				   
			 	 			 GROUP BY r.jur_id";
			 }
			 elseif($jurclass=="casino"){
			 	$params="ttm_code,n.jur_name,n.jur_id,n.jur_class,coalesce(jps_perc_totem, 0) AS percentage ";
			 	$jurisdictions="jurisdiction c, jurisdiction d, jurisdiction r,jurisdiction ca, jurisdiction n  
                    		LEFT JOIN jurisdiction_payment_settings ON jps_jur_id = n.jur_id  ";
			 	$join = "WHERE c.jur_class = 'club'
			 	 			 AND d.jur_class = 'district'
			 	 			 AND r.jur_class = 'region'
			 	 			 AND n.jur_class = 'nation'
			 	 			 AND ca.jur_class= 'casino'
			 	 			 AND ca.jur_id=n.jur_parent_id 
			 	 			 AND n.jur_id=r.jur_parent_id
			 	 			 AND r.jur_id = d.jur_parent_id
							 AND d.jur_id = c.jur_parent_id
							 AND c.jur_id = pun_betting_club
			 	            AND pun_id = ttt_pun_id
						 	 AND ca.jur_id='$jurid'
						 	 AND ttm_id = ttt_ttm_id
			 				 GROUP BY n.jur_id";
			 }
			 $starttime = microtime(true);
			 $sql="SELECT $params, SUM( IF( ttt_amount > 0 and TIMESTAMP(ttt_time) > '$date_start 00:00:00' and TIMESTAMP(ttt_time) < '$date_end 23:59:59', ttt_amount, 0 ) ) AS plus, SUM( IF( ttt_amount <0 and TIMESTAMP(ttt_time) > '$date_start 00:00:00' and TIMESTAMP(ttt_time) < '$date_end 23:59:59', ttt_amount, 0 ) ) AS minus
			 FROM totem, totem_transtrack,punter,$jurisdictions $join";
			 $result  = $dbh->exec($sql);
			 $endtime = microtime(true);
			 $duration = $endtime - $starttime;
			 $duration=number_format($duration, 4, ',', '')."s";
			 $total=0;
			 	if($jurclass=="casino")
			 		{
			 			$jurclass='nation';
			 		}
			 		elseif ($jurclass=='nation')
			 		{
			 			$jurclass='region';
			 		}
			 		elseif ($jurclass=='region')
			 		{
			 			$jurclass='district';
			 		}
			 		else
			 		{
			 			$jurclass='club';
			 		}
			  ?>
			  <br><br><script>duration('<?=$duration?>','duration');</script>
			  <?php
			  	if(!$result->hasNext()){
					?>
					    <span class="tip"><?=$lang->getLang("No result found")?></span>
					<?php
					return;
					}
					else {
                    ?>
			  <table style="width:100%"><tr><td class="<?=$jurclass?>totem" ><?=ucfirst($jurclass)?></td><td class="<?=$jurclass?>totem"><?=$lang->getLang("Deposit")?></td><td class="<?=$jurclass?>totem"><?=$lang->getLang("Withdraw")?></td><td class="<?=$jurclass?>totem">Total</td><td class="<?=$jurclass?>totem"><?=ucfirst($jurclass)?> %</td><td class="<?=$jurclass?>totem">Net</td></tr>
			  <?php
			  $totalNet=0;
			   while($result->hasNext()){
			   $row = $result->next();
			   $total=$total+$row['minus']+$row['plus'];
			   ?>
			   <tr><td class="content" style="text-align:left">
						<button id="submit<?=$row['jur_id']?>" class="entitybtn" type="button"><?=$row['jur_name']?>
						<script>
									 $(function() {
									$( '#submit<?=$row['jur_id']?>')
								            .click(function() {
								            	<?if($row['jur_class']!='club')
								            	{?>
								            	$.post("totem/totem_query_service.inc",{date_start:'<?=$date_start?>',date_end:'<?=$date_end?>',action:"0",jurid:"<?=$row['jur_id']?>",jurclass:"<?=$row['jur_class']?>"}, function(data){
								            	<?}else{?>
								            		$.post("totem/totem_query_service.inc",{date_start:'<?=$date_start?>',date_end:'<?=$date_end?>',action:"3",jurid:<?=$row['jur_id']?>,jurclass:'<?=$row['jur_class']?>'}, function(data){
								            		<?}?>
									            	$('#<?=$row['jur_id']?>').empty().append( data );

									            	});
								            	hide('<?=$row['jur_class']?>_details');
								            	toggle('<?=$row['jur_id']?>');
									            });
										return false;
										});
									</script>
						</button>
			   </td><td class="content" style="text-align:right"><?=getInDollars($row['plus']); ?></td><td class="content" style="text-align:right"><?=getInDollars($row['minus']);?></td><td class="content" style="text-align:right"><?=getInDollars($row['plus']+$row['minus']);?></td><td class="content" style="text-align:right">% <?=$row['percentage']?></td><td class="content" style="text-align:right"><table style="width:100%"><tr><td class="<?=$jurclass?>totem"> % </td><td class="<?=$jurclass?>totem"> <?=((isset($_SESSION['currency_html']) && $_SESSION['currency_html']!='') ? $_SESSION['currency_html'] : "&euro;" )?> </td></tr><tr><td style="width:20%;"><?=$parentProc-$row['percentage']?>% </td><td> <?=getInDollars(($parentProc-$row['percentage'])*($row['plus']+$row['minus'])/100)?></td></tr></table></td></tr>
			   <tr><td colspan="6"><div style="margin-left:20px;display:none;"  class="<?=$row['jur_class']?>_details" id="<?=$row['jur_id']?>"></div></td></tr>
			   <?php
			   $totalNet=$totalNet+(($parentProc-$row['percentage'])*($row['plus']+$row['minus'])/100);
			   }
			   ?>
			<tr class="<?=$jurclass?>totem"><td  colspan="2"><?=$lang->getLang("Total")?></td><td class="content"  colspan="2"><?=getInDollars($total)?></td><td class="content">% <?=$parentProc?></td><td class="content"><?=getInDollars($totalNet)?></td></tr>
			<tr><td><br><br></td></tr>
			   </table>

			 <?php
			 }
			 die();
}

function getTotemList($club,$type){
			global $dbh,$lang;
            if($club == '') {
                ?>
                    <span class="tip"><?=$lang->getLang("No result found")?></span>
                <?php
                die();
            }
			$sql = "SELECT ttm_club_id, jur_class, jur_name, count(*) as tot_totem
							  FROM `totem` 
							  JOIN jurisdiction on jur_id = ttm_club_id 
							  WHERE ttm_club_id in ($club)	 
							  GROUP BY ttm_club_id 
							  ORDER BY ttm_club_id ASC ";
					 $result  = $dbh->exec($sql);
					  if(!$result->hasNext()){
					  	     ?>
					     	   <span class="tip"><?=$lang->getLang("No result found")?></span>
					        <?php
					        return;
					  }
					  else {
					  ?>
					     <table style="width:100%"><tr><td class="label"><?=$lang->getLang("Club Name")?></td><td class="label"><?=$lang->getLang("Total Totems")?></td></tr>
					   <?php
						while($result->hasNext()){
						$row = $result->next();
						if($type=='details'){
					    ?>
						<tr><td class="content" style="cursor: pointer;" onclick=getTotemDetails('<?=$row['ttm_club_id']; ?>','<?=$row['jur_class'];?>');toggle('detail_<?=$row['ttm_club_id']; ?>')><?=$row['jur_name']; ?></td><td class="content"><?=$row['tot_totem']?></td></tr>
						<tr><td colspan="2"><div id="detail_<?=$row['ttm_club_id']; ?>" class="ttm_details"></div></td></tr>
					<?php
					}
					elseif($type=='deposit')
					{
					?>
					<tr><td class="content" style="cursor: pointer;" onclick=TotemsDeposit('<?=$row['ttm_club_id']; ?>','<?=$row['jur_class'];?>');toggle('deposit_<?=$row['ttm_club_id']; ?>')><?=$row['jur_name']; ?></td><td class="content"><?=$row['tot_totem']?></td></tr>
					<tr><td colspan="2"><div id="deposit_<?=$row['ttm_club_id']; ?>" class="ttm_details"></div></td></tr>
					<?php
					}
                        elseif($type=='closure')
                        {
                            ?>
                            <tr><td class="content" style="cursor: pointer;" onclick=getTotemClosure('<?=$row['ttm_club_id']; ?>','<?=$row['jur_class'];?>');toggle('closure_<?=$row['ttm_club_id']; ?>')><?=$row['jur_name']; ?></td><td class="content"><?=$row['tot_totem']?></td></tr>
                            <tr><td colspan="2"><div id="closure_<?=$row['ttm_club_id']; ?>" class="ttm_details"></div></td></tr>
                        <?php
                        }
						}
					?>
						</table>
					<?php }
					  	die();
}

function getClubTransactions($club){
	global $dbh,$lang;
	if(isset($_POST['date_club']))
	{
		$date=$_POST['date_club'];
	}
	else
	{
		$date=date("Y-m-d");
	}
	?>
	<script type="text/javascript">
$(function() {
    $( "#date_club" ).datepicker();
});
	</script>
	<script language="javascript" type="text/javascript">

	function setDate(days){
		var z = 24*days; // number of hours ahead
		var zz = 1; // number of minutes ahead


		var nd = new Date(document.getElementById('date_club').value);  // USA date format
		nd = nd.getTime() + (3600000 * z);  // + z hours
		nd = nd + (60000 * zz);  // + zz minutes
		var d = new Date(nd);
		var mm = d.getMonth() +1;
		if (mm <10) {mm = "0" + mm}
		var dd = d.getDate();
		if (dd <10) {dd = "0" + dd}
		var yy = d.getFullYear();
		var hrs = d.getHours();
		if (hrs <10) {hrs = "0" + hrs}
		var mns = d.getMinutes();
		if (mns <10) {mns = "0" + mns}
		var secs = d.getSeconds();
		if (secs <10) {secs = "0" + secs}
		futureDate = (yy + "-" + mm + "-" + dd );  // UK date format
		document.getElementById('date_club').value=futureDate;
	}
</script>
	<center>
	<table>
		    <tr>
				<td><img style="width:15px; height:15px;" src="media/images/left.gif" onclick="setDate('-1');hide('ttm_details');"></td><td class="content"><input type="text" id="date_club" name="date_club"  value="<?=$date?>" onclick="hide('ttm_details');"/></td><td><img style="width:15px; height:15px;" src="media/images/right.gif" onclick="setDate('1');hide('ttm_details');"></td>
			</tr>
		</table>

</center>

	<?
	$sql = "SELECT ttm_club_id, jur_class, jur_name, count(*) as tot_totem
	FROM `totem`
	JOIN jurisdiction on jur_id = ttm_club_id
	WHERE ttm_club_id in ($club)
	GROUP BY ttm_club_id
	ORDER BY ttm_club_id ASC ";
	$result  = $dbh->exec($sql);
	if(!$result->hasNext()){
	?>
					     	   <span class="tip"><?=$lang->getLang("No result found")?></span>
					        <?php
					        return;
					  }
					  else {
					  ?>
					     <table style="width:100%"><tr><td class="label"><?=$lang->getLang("Club Name")?></td><td class="label"><?=$lang->getLang("Total Totem")?></td></tr>
					   <?php
						while($result->hasNext()){
						$row = $result->next();
					?>
						<tr><td class="content" style="cursor: pointer;" onclick=getClubDetails('<?=$row['ttm_club_id']; ?>','<?=$row['jur_class'];?>');toggle('detail_club_<?=$row['ttm_club_id']; ?>')><?=$row['jur_name']; ?></td><td class="content"><?=$row['tot_totem']?></td></tr>
						<tr><td colspan="2"><div id="detail_club_<?=$row['ttm_club_id']; ?>" class="ttm_details"></div></td></tr>
					<?php
						}
					?>
						</table>
					<?php }
					  	die();
}

function getDetails($jur_id,$jur_class){
		global $dbh,$lang;
				$sql="SELECT
                             ttm_code,
                             ttm_access,
                             ttm_act_status,
                             SUM( IF( ttt_amount > 0 and ttm_id = ttt_ttm_id AND DATE(ttt_time) = CURDATE(), ttt_amount, 0 )  ) AS plus,
                             SUM( IF( ttt_amount <0 and ttm_id = ttt_ttm_id AND DATE(ttt_time) = CURDATE(), ttt_amount, 0 ) ) AS minus,
                             cur_code_for_web
					from totem, totem_transtrack, punter,currency  ";
				if($jur_class == "club"){
					$sql .= ",jurisdiction c
							 WHERE (ttm_id = ttt_ttm_id OR ttm_id <> ttt_ttm_id)
							 AND ttm_pun_id = pun_id
							 AND pun_betting_club = c.jur_id
							 AND pun_betting_club = " . $jur_id."
							 AND c.jur_currency=cur_id
		 					 GROUP BY ttm_code";
				}
				if($jur_class == "district"){
					$sql .= "JOIN jurisdiction c on c.jur_id = ttm_club_id
					 		JOIN jurisdiction d on d.jur_id = c.jur_parent_id
					 		WHERE d.jur_currency=cur_id
					 		AND d.jur_id = " . $jur_id;
				}
				elseif($jur_class=="region"){
					$sql .="JOIN jurisdiction c on c.jur_id = ttm_club_id
							JOIN jurisdiction d on d.jur_id = c.jur_parent_id
							JOIN jurisdiction r on r.jur_id = d.jur_parent_id 
							WHERE r.jur_currency=cur_id
							AND r.jur_id=".$jur_id;
				}
				elseif ($jur_class=="nation"){
					$sql .="JOIN jurisdiction c on c.jur_id = ttm_club_id
							JOIN jurisdiction d on d.jur_id = c.jur_parent_id
							JOIN jurisdiction r on r.jur_id = d.jur_parent_id
							JOIN jurisdiction n on n.jur_id = r.jur_parent_id
							WHERE n.jur_currency=cur_id
							AND n.jur_id=".$jur_id;
				}
				elseif($jur_class=="casino"){
					$sql .="JOIN jurisdiction c on c.jur_id = ttm_club_id
							JOIN jurisdiction d on d.jur_id = c.jur_parent_id
							JOIN jurisdiction r on r.jur_id = d.jur_parent_id
							JOIN jurisdiction n on n.jur_id = r.jur_parent_id
							JOIN jurisdiction ca on ca.jur_id = n.jur_parent_id
							WHERE ca.jur_currency=cur_id
							AND ca.jur_id = ".$jur_id;
				}
				$result=$dbh->exec($sql);
				if(!$result->hasNext()){
				?>
				    <span class="tip"><?=$lang->getLang("No result found")?></span>
				<?php
				return;
				}
				else {
				?>
				<div style="display: none" id="dialog-confirm" title="<?=$lang->getLang('Change Access?')?>">
		    		<p><span class="ui-icon ui-icon-alert" style="float: left; margin: 0 7px 20px 0;"></span><?=$lang->getLang("You are attempting to change this totem's access.Are you sure?")?></p>
				</div>
					<table style="width:100%"><tr><td class="ttm_label"><?=$lang->getLang("Totem Code")?></td><td class="ttm_label"><?=$lang->getLang("Access")?></td><td class="ttm_label"><?=$lang->getLang("Today status")?></td><td class="ttm_label">Management</td></tr>
				<?php
				while($result->hasNext()){
					$row = $result->next();
				?>
					<tr><td class="content"><?=$row['ttm_code'];?></td>
                        <td class="content" onclick="changeAccess('<?=$row['ttm_code']?>','<?=strtoupper($row['ttm_access']);?>','<?=$jur_id?>','<?=$jur_class?>');" style="cursor:pointer;"><? if(strtoupper ( $row['ttm_access'] ) == 'ALLOW') {?><img style="width:20px; height:20px;" src="live/liveimage/allow.png"><? }else{?><img style="width:20px; height:20px;" src="live/liveimage/deny.png"><?}?></td>
                        <td class="content"><?=getInDollars($row['plus']-$row['minus'],2,$row['cur_code_for_web']) ?></td>
                        <td class="content"><?=getTotemAction($row['ttm_code'],$row['ttm_access'],$jur_id,$jur_class,$row['ttm_act_status']) ?></td></tr>
				<?php
					}
				?>
				</table>
				<?php
					}
}

function getTotemDetails($jur_id,$jur_class,$date_start,$date_end){

			global $dbh,$lang;
			$sql="SELECT ttm_code,SUM( IF( ttt_amount > 0 and TIMESTAMP(ttt_time) > '$date_start 00:00:00' and TIMESTAMP(ttt_time) < '$date_end 23:59:59' and ttm_id = ttt_ttm_id, ttt_amount, 0 ) ) AS plus, SUM( IF( ttt_amount <0 and TIMESTAMP(ttt_time) > '$date_start 00:00:00' and TIMESTAMP(ttt_time) < '$date_end 23:59:59' and ttm_id = ttt_ttm_id, ttt_amount, 0 ) ) AS minus
					from totem,totem_transtrack,punter ";
			if($jur_class == "club"){
				$sql .= " WHERE (ttm_id = ttt_ttm_id OR ttm_id <> ttt_ttm_id) AND ttm_pun_id = pun_id AND pun_betting_club  = " . $jur_id."
		 				 GROUP BY ttm_code";
			}
		// 	if($jur_class == "district"){
		// 		$sql .= "JOIN jurisdiction c on c.jur_id = ttm_club_id
		// 			 		JOIN jurisdiction d on d.jur_id = c.jur_parent_id
		// 			 		WHERE d.jur_id = " . $jur_id;
		// 	}
		// 	elseif($jur_class=="region"){
		// 		$sql .="JOIN jurisdiction c on c.jur_id = ttm_club_id
		// 					JOIN jurisdiction d on d.jur_id = c.jur_parent_id
		// 					JOIN jurisdiction r on r.jur_id = d.jur_parent_id
		// 					WHERE r.jur_id=".$jur_id;
		// 	}
		// 	elseif ($jur_class=="nation"){
		// 		$sql .="JOIN jurisdiction c on c.jur_id = ttm_club_id
		// 					JOIN jurisdiction d on d.jur_id = c.jur_parent_id
		// 					JOIN jurisdiction r on r.jur_id = d.jur_parent_id
		// 					JOIN jurisdiction n on n.jur_id = r.jur_parent_id
		// 					WHERE n.jur_id=".$jur_id;
		// 	}
		// 	elseif($jur_class=="casino"){
		// 		$sql .="JOIN jurisdiction c on c.jur_id = ttm_club_id
		// 					JOIN jurisdiction d on d.jur_id = c.jur_parent_id
		// 					JOIN jurisdiction r on r.jur_id = d.jur_parent_id
		// 					JOIN jurisdiction n on n.jur_id = r.jur_parent_id
		// 					JOIN jurisdiction ca on ca.jur_id = n.jur_parent_id
		// 					WHERE ca.jur_id = ".$jur_id;
		// 	}
			$result=$dbh->exec($sql);

			if(!$result->hasNext()){
				?>
				    <span class="tip"><?=$lang->getLang("No result found")?></span>
				<?php
				return;
				}
				else {
				?>
					<table style="width:100%"><tr><td class="ttm_label"><?=$lang->getLang("Totem")?></td><td class="ttm_label"><?=$lang->getLang("Deposit")?></td><td class="ttm_label"><?=$lang->getLang("Withdraw")?></td><td class="ttm_label">Total</td></tr>
				<?php
				while($result->hasNext()){
					$row = $result->next();
				?>
					<tr><td class="content"><?=$row['ttm_code'];?></td><td class="content"><?=getInDollars($row['plus']);?></td><td class="content"><?=getInDollars($row['minus']) ?></td><td class="content"><?=getInDollars($row['plus']+$row['minus']) ?></td></tr>
				<?php
					}
				?>
				</table>
				<?php
					}
}

function getTotems($jur_id,$jur_class)
{
			global $dbh;
			$sql="SELECT ttm_code from totem ";
			if($jur_class == "club"){
				$sql .= " WHERE ttm_club_id = " . $jur_id;
			}
			if($jur_class == "district"){
				$sql .= "JOIN jurisdiction c on c.jur_id = ttm_club_id
					 		JOIN jurisdiction d on d.jur_id = c.jur_parent_id
					 		WHERE d.jur_id = " . $jur_id;
			}
			elseif($jur_class=="region"){
				$sql .="JOIN jurisdiction c on c.jur_id = ttm_club_id
							JOIN jurisdiction d on d.jur_id = c.jur_parent_id
							JOIN jurisdiction r on r.jur_id = d.jur_parent_id
							WHERE r.jur_id=".$jur_id;
			}
			elseif ($jur_class=="nation"){
				$sql .="JOIN jurisdiction c on c.jur_id = ttm_club_id
							JOIN jurisdiction d on d.jur_id = c.jur_parent_id
							JOIN jurisdiction r on r.jur_id = d.jur_parent_id
							JOIN jurisdiction n on n.jur_id = r.jur_parent_id
							WHERE n.jur_id=".$jur_id;
			}
			elseif($jur_class=="casino"){
				$sql .="JOIN jurisdiction c on c.jur_id = ttm_club_id
							JOIN jurisdiction d on d.jur_id = c.jur_parent_id
							JOIN jurisdiction r on r.jur_id = d.jur_parent_id
							JOIN jurisdiction n on n.jur_id = r.jur_parent_id
							JOIN jurisdiction ca on ca.jur_id = n.jur_parent_id
							WHERE ca.jur_id = ".$jur_id;
			}
			$result=$dbh->exec($sql);
		?>
		- TOTEMS:
		<?
		$totems=array();
		$totaltotems=$result->NumRows;
		while($result->hasNext()){
			$row = $result->next();
				array_push($totems, $row['ttm_code']);
					}
		?>
		<?=$totaltotems?>
		<?if($jur_class=='club'){?>
	<!-- <br><img src="<?=image_dir?>/reset.png" width="60" height="25" style="cursor: pointer;" onclick="<? foreach($totems as $key=>$value) {?>shutdowntotem('<?=$value?>');<?}?>"/> -->
		<?}
		}
		die();



function getDetailsDeposit($jur_id,$jur_class){

			global $dbh,$lang;

			$sql="SELECT ttm_code,ttm_access,ttm_act_status,pcr_pun_id,pcr_credits,ttm_totem_md5,jur_code,pun_customer_number 
    					from totem
    					JOIN punter_credit on pcr_pun_id = ttm_pun_id
    					JOIN punter on pun_id = ttm_pun_id			
    					JOIN jurisdiction on jur_id = ttm_club_id ";
			if($jur_class == "club"){
				$sql .= "
							 WHERE ttm_club_id = " . $jur_id."
		 					 GROUP BY ttm_code";
			}
			$result=$dbh->exec($sql);
			if(!$result->hasNext()){
				?>
						    <span class="tip"><?=$lang->getLang("No result found")?></span>
						<?php
						return;
						}
						else {
						?>
							<table style="width:100%"><tr><td class="ttm_label"><?=$lang->getLang("Totem Code")?></td><td class="ttm_label"><?=$lang->getLang("Credit")?></td><td class="ttm_label"><?=$lang->getLang("Deposit")?></td><td class="ttm_label">Confirm</td></tr>
						<?php
						while($result->hasNext()){
							$row = $result->next();
						?>
							<tr><td class="content"><?=$row['ttm_code'];?></td><td class="content"><?=getInDollars($row['pcr_credits'])?> </td><td class="content"><input type="text" id="deposit_<?=$row['ttm_code']?>" name="deposit_<?=$row['ttm_code']?>"></td><td class="content"><button id="do_deposit<?=$row['ttm_code']?>">Confirm</button>
							<script>
									 $(function() {
									$( '#do_deposit<?=$row['ttm_code']?>')
								            .button()
								            .click(function() {
								                $( "#deposit-confirm" ).dialog({
								                    resizable: false,
								                    height:140,
								                    modal: true,
								                    buttons: {
								                        "Yes": function() {
								                            $( this ).dialog( "close" );
								                            $.post("totem/totem_query_service.inc",{aid:'<?=$row['jur_code']?>',totemmd5:'<?=$row['ttm_totem_md5']?>',totemcode:'<?=$row['ttm_code']?>',credits:$('input:text[name=deposit_<?=$row['ttm_code']?>]').val(), user:'<?=$row['pun_customer_number']?>',action:"8"}, function(data){
											            	});
													TotemsDeposit('<?=$jur_id?>','<?=$jur_class?>');
								                        },
								                        Cancel: function() {
								                            $( this ).dialog( "close" );
								                        }
								                    }
								                });

									        });
										return false;
										});
									</script>
							</td></tr>
						<?php
							}
						?>
						</table>
						<?php
							}
		}

function doTotemDeposit($totem_code, $user_id,$amount,$totemMD5,$agid) {
	global $dbh;
	$sql = "select pun_sess_id from punter where pun_customer_number = " . $user_id;
	$sess_id= $dbh->queryOne($sql);
	$transferid=time().$totem_code;
	$md5 = hash('md5', '3008' . $totem_code . $user_id.$sess_id.$amount.$transferid.$totemMD5);
	$result = "";
	$DataToSend ="operationtype=3008&cd=" . $totem_code . "&agid=" . $agid .  "&userid=" . $user_id . "&sessid=" . $sess_id. "&amount=" . $amount. "&transferid=" . $transferid."&md5=" . $md5 ;
	$ch=curl_init(WEB_APP_TOTEM);
	curl_setopt($ch,CURLOPT_SSL_VERIFYPEER, FALSE);
	curl_setopt($ch,CURLOPT_POST,1);
	curl_setopt($ch,CURLOPT_POSTFIELDS, $DataToSend);
	curl_setopt($ch,CURLOPT_RETURNTRANSFER, 1);
	$result=curl_exec($ch);
	error_log("CURL Request: " . $result);
	return $result;
}

function getAgencyGames($jur_id)
{
	global $dbh,$lang;
	include_once 'StringTokenizer.class.php';
	$thisGames=getAgencyLicense($jur_id);
	$games=split(";", $thisGames);
	$thisConstant=$games['0'];
	$gamesinfo=$games['1'];
	$gamess=array();
	$results = new StringTokenizer ( $gamesinfo, "~" );
	while ( $results->hasMoreTokens () ) {
		list($gameid,$gamename)=explode("&", $results->nextToken ());
		$gamess[$gameid] = $gamename;
		}

    $sql = "SELECT gam_id, gam_name FROM game WHERE gam_type = 'agency' ";

    if($jur_id == 300){
        $sql .= " AND gam_name LIKE '[Adjara]%' ";
    }else{
        $sql .= " AND gam_name NOT LIKE '[Adjara]%' ";
    }

    $result=$dbh->exec($sql);
	if(!$result->hasNext())
    {
	?>
	    <span class="tip"><?=$lang->getLang("No result found")?></span>
	<?
	return;
	}
	else
	{
	?>
		<table style="margin-left:10%; " id="checkboxes"><tr>
	<?
		while($result->hasNext())
			{
				$row = $result->next();
		?>
			<td><table><tr>
                        <td>
                            <input type="checkbox" value="<?=$row['gam_id']?>&<?=$row['gam_name']?>"<? foreach ($gamess as $k=>$v){ if($row['gam_id']==$k){?> checked="checked"<?}}?> name="<?=$row['gam_id']?>&<?=$row['gam_name']?>">
                        </td>
                        <td><img width="50" height="40"  src="media/images/<?=$row['gam_id']?>.png"><br> <?=$row['gam_name']?></td></td></tr>
                </table>
		<?
			}
		?>
		</tr></table>
				<button id="ConfirmGames"><?=$lang->getLang("Confirm")?>
						<script>
									 $(function() {
									$( '#ConfirmGames')
								            .button()
								            .click(function() {
								            	var selected = new Array();
								            	$('#checkboxes input:checked').each(function() {
								            	    selected.push($(this).attr('value'));
								            	});
								            		$.post("totem/totem_query_service.inc",{action:'10',license:';'+selected, jurid:<?=$jur_id?>}, function(data){
									            		alert("<?=$lang->getLang("Virtual Updated")?>");
									            	});
									            });
										return false;
										});
									</script>
						</button>
		<?
	}
}

function cryptLicense($license, $jur_id)
{
	global $dbh;
	$license = str_replace(',','~',$license);
	$license = "0".$license;
	$sql="SELECT jur_code
		FROM  `virtual_games`
		JOIN jurisdiction ON jur_code = vgs_jur_code
		WHERE jur_id = '$jur_id'";
	$exists = $dbh->queryOne($sql);
	if($exists == NULL){
		$sql = "INSERT INTO virtual_games (VGS_JUR_CODE, VGS_LICENSE_CODE) 
    		     VALUES ((SELECT jur_code FROM jurisdiction where jur_id = ".$jur_id."),'$license')";
	}else{
		$sql = "UPDATE virtual_games set VGS_LICENSE_CODE = '$license' , VGS_LAST_MODIFY = CURRENT_TIMESTAMP 
			    where VGS_JUR_CODE = (SELECT jur_code FROM jurisdiction where jur_id = ".$jur_id.")";
	}
	$dbh->exec($sql);
}

function getAgencyLicense($jur_id)
{
	global $dbh,$lang;
	$sql="SELECT vgs_license_code
		FROM  `virtual_games`
		JOIN jurisdiction ON jur_code = vgs_jur_code
		WHERE jur_id = '$jur_id'";
	$games=$dbh->queryOne($sql);
	return $games;

}

function getTotemClosure($jur_id){
    global $dbh,$lang;
    $sql=" SELECT min(ttt_time) ttt_time, max(ttc_end_day) ttc_end_day,ttm_code, ttm_id, c.jur_name club_name, c.jur_id club_id, d.jur_name district_name, d.jur_id district_id, r.jur_name region_name, r.jur_id region_id, n.jur_name nation_name, n.jur_id nation_id, COALESCE( jps1.jps_perc_totem, 0 ) AS perc_casino,
COALESCE( jps2.jps_perc_totem, 0 ) AS perc_nation,
COALESCE( jps3.jps_perc_totem, 0 ) AS perc_region,
COALESCE( jps4.jps_perc_totem, 0 ) AS perc_district,
COALESCE( jps5.jps_perc_totem, 0 ) AS perc_club
FROM totem, punter, jurisdiction c LEFT JOIN jurisdiction_payment_settings jps5 ON c.jur_id = jps5.jps_jur_id,
jurisdiction d LEFT JOIN jurisdiction_payment_settings jps4 ON d.jur_id = jps4.jps_jur_id,
jurisdiction r LEFT JOIN jurisdiction_payment_settings jps3 ON r.jur_id = jps3.jps_jur_id,
jurisdiction n LEFT JOIN jurisdiction_payment_settings jps2 ON n.jur_id = jps2.jps_jur_id,
jurisdiction ca LEFT JOIN jurisdiction_payment_settings jps1 ON ca.jur_id = jps1.jps_jur_id,
totem_transtrack LEFT JOIN totem_closure ON ttt_ttm_id = ttc_ttm_id
WHERE n.jur_parent_id = ca.jur_id and r.jur_parent_id = n.jur_id and d.jur_parent_id = r.jur_id and c.jur_parent_id = d.jur_id
and c.jur_id = pun_betting_club and pun_id = ttt_pun_id and ttt_ttm_id = ttm_id ";
    if($_SESSION['jurisdiction_class']=='nation'){
        $sql .=' AND  n.jur_id='.$_SESSION['jurisdiction_id'];
    }
    if($_SESSION['jurisdiction_class']=='region'){
        $sql .=' AND r.jur_id='.$_SESSION['jurisdiction_id'];
    }
    if($_SESSION['jurisdiction_class']=='district'){
        $sql .=' AND d.jur_id='.$_SESSION['jurisdiction_id'];
    }
    if($_SESSION['jurisdiction_class']=='club'){
        $sql .=' AND c.jur_id='.$_SESSION['jurisdiction_id'];
    }
    $sql .=" AND pun_betting_club=$jur_id
    GROUP BY c.jur_id, ttm_id
    order by ttm_code
    ";
    $result=$dbh->exec($sql);
    if(!$result->hasNext()){
        ?>
        <span class="tip"><?=$lang->getLang("No result found")?></span>
        <?php
        return;
    }
    else {

        ?>
        <table style="width:100%">
            <tr>
                <td>
                    <table style='width:100%'>
                        <tr>
                            <td style="text-align: left">
                                <input type='radio' name='showOption' onclick='showCloseAll()'><?=$lang->getLang("Close all totems")?>
                            </td>
                        </tr>
                        <tr>
                            <td style="text-align: left">
                                <input type='radio' name='showOption' onclick="showCloseEach()"><?=$lang->getLang("Close each totem")?>
                           </td>
                        </tr>
                    </table>
                </td>
            </tr>
            <tr>
                <td>
                    <table style="width:100%">
                        <tr class='closureall'>
                            <td>
                                <table style="width: 100%" ><tr><td class="districttotem"><?=$lang->getLang("Close all Totems")?></td></tr>
                                    <tr>
                                        <td>
                                            <table style="width: 100%">
                                                <tr><td class="ttm_label" colspan="2"><?=$lang->getLang("End Date")?></td></tr>
                                                <tr><td><?=$lang->getLang("To close all totems under this club insert an end date:")?></td><td class="text-align:right"><input type='text' name='endclosureall<?=$jur_id?>' id='endclosureall<?=$jur_id?>'>
                                                        <script type="text/javascript">
                                                            $(function() {
                                                                $( "#endclosureall<?=$jur_id?>" ).datepicker({
                                                                    changeMonth: true,
                                                                    changeYear:true,
                                                                    maxDate:-1,
                                                                    numberOfMonths:1
                                                                });
                                                            });
                                                        </script><button id='doallclosure<?=$jur_id?>'><?=$lang->getLang("Close All")?></button>
                                                        <script>
                                                            $(function(){
                                                                $( "#doallclosure<?=$jur_id?>")
                                                                    .button()
                                                                    .click(function() {
                                                                        $.post("totem/totem_query_service.inc",{action:'14',jur_id:'<?=$jur_id?>',date_end:$("#endclosureall<?=$jur_id?>").val()}, function(data){
                                                                            $("#partialclosure").empty().append( data );
                                                                            $('#closureresult').empty();
                                                                            $("#add_part").show('slow');
                                                                            $("#part").show('slow');
                                                                        });
                                                                    });
                                                                return false;
                                                            });
                                                        </script>
                                                    </td>
                                                </tr>
                                            </table>
                                        </td>
                                    </tr>
                                </table>
                            </td>
                        </tr>
                        <tr class="closureeach">
                            <td>
                                <table style='width:100%'><tr><td class="districttotem"><?=$lang->getLang("Close Totem By Totem")?></td></tr>
                                <tr><td>
                              <table style="width:100%" ><tr><td class="ttm_label"><?=$lang->getLang("Totem Code")?></td><td class="ttm_label"><?=$lang->getlang("From")?></td><td class="ttm_label"><?=$lang->getLang('Until')?></td></tr>
                            <?php
                            while($result->hasNext()){
                                $row = $result->next();
                                ?>
                                <tr><td class="content"><?=$row['ttm_code'];?></td>
                                    <td class="content" > <? if($row['ttc_end_day']!='')
                                        {
										$date1= date('Y-m-d H:i:s', strtotime(($row['ttc_end_day'])));
                                        $date= date('Y-m-d H:i:s', strtotime(($row['ttc_end_day']). ' + 1 seconds'));
                                        }
                                        else
                                        {
                                           $date1=substr($row['ttt_time'],0,10);
                                           $date=substr($row['ttt_time'],0,10);
                                        }
                                        echo $date;
                                        $end_date=date('Y-m-d H:i:s', strtotime(($date1. '+ 1 days')));
                                        ?>
                                        </td>
                                    <td class="content">
                                        <input type='text' name='date_end' id='closure_end<?=$row['ttm_code']?>' value='<?=$end_date?>'>
                                        <script type="text/javascript">
                                            $(function() {
                                                $( "#closure_end<?=$row['ttm_code']?>" ).datetimepicker({
                                                    changeMonth: true,
                                                    changeYear:true,
                                                    maxDate:-1,
                                                    numberOfMonths:1,
													timeFormat:'HH:mm:ss',
													onChangeMonthYear:function(year, month, i){
														var day = i.selectedDay;
														$(this).datepicker('setDate', new Date(year, month-1, day));
													}
                                                });
                                            });
                                        </script>

                                        <button id="calculateclosure<?=$row['ttm_code']?>"><?=$lang->getLang("Calculate")?></button>
                                        <script>
                                            $(function(){
                                                $( "#calculateclosure<?=$row['ttm_code']?>")
                                                    .button()
                                                    .click(function() {
                                                        $.post("totem/totem_query_service.inc",{action:'12',tc:'<?=$row['ttm_code']?>',tid:'<?=$row['ttm_id']?>',date_start:'<?=$date?>',date_end:$("#closure_end<?=$row['ttm_code']?>").val(),cap:'<?=$row['perc_casino']?>',nid:'<?=$row['nation_id']?>',np:'<?=$row['perc_nation']?>',rid:'<?=$row['region_id']?>',rp:'<?=$row['perc_region']?>',did:'<?=$row['district_id']?>',dp:'<?=$row['perc_district']?>',cid:'<?=$row['club_id']?>',cp:'<?=$row['perc_club']?>'}, function(data){
                                                            $("#partialclosure").empty().append( data );
                                                            $('#closureresult').empty();
                                                            $("#add_part").show('slow');
                                                            $("#part").show('slow');
                                                        });
                                                    });
                                                return false;
                                            });
                                        </script>
                                       </td></tr>
                                    <?php
                                    }
                                    ?>
                             </table>
                         </td></tr>
             </table></td>
            </tr>
        </table>
    <?
    }
}

function calculateTotemClosure($totemcode,$tid,$date_start,$date_end,$casinoperc,$nationid,$nationperc,$regionid,$regionperc,$districtid,$districtperc,$clubid,$clubperc){
    global $dbh,$lang;
    if($date_start>$date_end){
        addError('','Start Date is greater than End Date');

    }
    if($date_end==''){
        addError('','Empty end date!');
    }
    if(!areErrors()){
    $sql="select pun_betting_club, ttt_ttm_id, SUM( IF( ttt_amount > 0 and ttt_time BETWEEN '$date_start' and '$date_end', ttt_amount, 0 ) ) AS deposit,
    SUM( IF( ttt_amount <0 and ttt_time BETWEEN '$date_start' and '$date_end', ttt_amount, 0 ) ) AS withdraw
    from punter, totem_transtrack
    where ttt_pun_id = pun_id
    and pun_betting_club =  $clubid
    and ttt_ttm_id = $tid
    and ttt_time BETWEEN '$date_start' and '$date_end'";
	
        $result = $dbh->doCachedQuery ( $sql, 0 );
        while($result->hasNext()){
            $row=$result->next();
            $notetext="^^^ca:$casinoperc#n:$nationperc#r:$regionperc#d:$districtperc#c:$clubperc";
            ?>
            <table style='width:100%'>
            <tr><td class='label'><?=$lang->getLang("Totem Code")?></td><td class='label'><?=$lang->getLang("From")?></td><td class='label'><?=$lang->getLang("Until")?></td><td class='label'><?=$lang->getLang("Deposit")?></td><td class='label'><?=$lang->getLang("Withdraw")?></td><td class='label'>Financial</td><td class='label'>Note</td></tr>
            <tr><td><?=$totemcode?></td><td><?=$date_start?></td><td><?=$date_end?></td><td><?=getInDollars($row['deposit'])?></td><td><?=getInDollars($row['withdraw'])?></td><td><?=getInDollars($row['deposit']+$row['withdraw'])?></td><td>
                    <textarea cols='20' rows='3' id='note' ></textarea>
                </td></tr>
            <tr><td colspan='15' style='text-align: left'><button id='confirmClosure'><?=$lang->getLang("Confirm")?></button>
                    <script>
                        $(function(){
                            $( "#confirmClosure")
                                .button()
                                .click(function() {
                                    $.post("totem/totem_query_service.inc",{action:'13',
                                                                            tid:'<?=$tid?>',
                                                                            date_start:'<?=$date_start?>',
                                                                            date_end:'<?=$date_end?>',
                                                                            dep:'<?=$row['deposit']?>',
                                                                            ww:'<?=$row['withdraw']?>',
                                                                            fin:"<?=$row['deposit']+$row['withdraw']?>",
                                                                            note:$('#note').val()+'<?=$notetext?>',
                                                                            nid:'<?=$nationid?>',
                                                                            rid:'<?=$regionid?>',
                                                                            did:'<?=$districtid?>',
                                                                            cid:'<?=$clubid?>',
                                                                            cp:'<?=$clubperc?>'
                                                                          },
                                                    function(data){
                                                        $("#closureresult").empty().append(data);
                                                        if($("#success").text()=='Success done!'){
                                                             getTotemClosure('<?=$clubid?>','club');
                                                        }
                                                    });
                                    });
                            return false;
                        });
                    </script>
                </td></tr>
            </table></td></tr></table>
        <?
        }
    }
    else
    {
        showErrors();
    }
}

function doTotemClosure($tid,$date_start,$date_end,$dep,$ww,$fin,$cp,$note,$nid,$rid,$did,$cid){
    global $dbh,$lang;
    $sql="INSERT INTO totem_closure(ttc_start_day,ttc_end_day,ttc_jur_nation,ttc_jur_region,ttc_jur_district,ttc_jur_club,ttc_bet,ttc_win,ttc_fin,ttc_perc,ttc_ttm_id,ttc_note,ttc_aus_id)
                                                    VALUES ('$date_start','$date_end','$nid','$rid','$did','$cid','$dep','$ww','$fin','$cp','$tid','$note','".$_SESSION['jurisdiction_id']."')";
    $result = $dbh->doCachedQuery ( $sql, 0 );

   if(is_int($result)&&$result==1){
       echo '<div class="result" id="success">'.$lang->getLang("Success done!").'</div>';
   }
    else{
        echo '<div class="error">'.$lang->getLang("An error has occurred!").'</div>';
    }
}

function getTotemClosureByClub($jur_id,$end_date){
    global $dbh,$lang;
    $sql=" SELECT min(ttt_time) ttt_time, max(ttc_end_day) ttc_end_day,ttm_code, ttm_id, c.jur_name club_name, c.jur_id club_id, d.jur_name district_name, d.jur_id district_id, r.jur_name region_name, r.jur_id region_id, n.jur_name nation_name, n.jur_id nation_id, COALESCE( jps1.jps_perc_totem, 0 ) AS perc_casino,
    COALESCE( jps2.jps_perc_totem, 0 ) AS perc_nation,
    COALESCE( jps3.jps_perc_totem, 0 ) AS perc_region,
    COALESCE( jps4.jps_perc_totem, 0 ) AS perc_district,
    COALESCE( jps5.jps_perc_totem, 0 ) AS perc_club,
    SUM(IF( ttt_amount > 0 and ttt_time < '$end_date 23:59:59' AND  IF(ttc_end_day IS NOT NULL, ttc_end_day<ttt_time, 1=1), ttt_amount, 0 ) ) AS deposit,
    SUM(IF( ttt_amount < 0 and ttt_time < '$end_date 00:00:00' AND  IF(ttc_end_day IS NOT NULL, ttc_end_day<ttt_time, 1=1), ttt_amount, 0 ) ) AS withdraw
    FROM totem, punter, jurisdiction c LEFT JOIN jurisdiction_payment_settings jps5 ON c.jur_id = jps5.jps_jur_id,
    jurisdiction d LEFT JOIN jurisdiction_payment_settings jps4 ON d.jur_id = jps4.jps_jur_id,
    jurisdiction r LEFT JOIN jurisdiction_payment_settings jps3 ON r.jur_id = jps3.jps_jur_id,
    jurisdiction n LEFT JOIN jurisdiction_payment_settings jps2 ON n.jur_id = jps2.jps_jur_id,
    jurisdiction ca LEFT JOIN jurisdiction_payment_settings jps1 ON ca.jur_id = jps1.jps_jur_id,
    totem_transtrack LEFT JOIN totem_closure ON ttt_ttm_id = ttc_ttm_id AND ttc_end_day <= '$end_date'
    WHERE n.jur_parent_id = ca.jur_id and r.jur_parent_id = n.jur_id and d.jur_parent_id = r.jur_id and c.jur_parent_id = d.jur_id
    and c.jur_id = pun_betting_club and pun_id = ttt_pun_id and ttt_ttm_id = ttm_id ";
        if($_SESSION['jurisdiction_class']=='nation'){
            $sql .=' AND  n.jur_id='.$_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class']=='region'){
            $sql .=' AND r.jur_id='.$_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class']=='district'){
            $sql .=' AND d.jur_id='.$_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class']=='club'){
            $sql .=' AND c.jur_id='.$_SESSION['jurisdiction_id'];
        }
        $sql .=" AND pun_betting_club=$jur_id
        AND ttt_time < '$end_date'
    GROUP BY c.jur_id, ttm_id
    order by ttm_code";

    $result = $dbh->exec( $sql );
    ?>
    <table style='width:100%' id='plm'>
    <tr><th class='label'>Totem Code</th><th class='label'><?=$lang->getLang("From")?></th><th class='label'><?=$lang->getLang('Until')?></th><th class='label'><?=$lang->getLang('Deposit')?></th><th class='label'>Withdraw</th><th class='label'>Financial</th><th class='label'>Note</th></tr>
    <?
    while($result->hasNext()){
        $row=$result->next();

        if($row['ttc_end_day']!='')
        {
            $date_start= $row['ttc_end_day'];
        }
        else
        {
            $date_start=substr($row['ttt_time'],0,10);
        }
        ?>
            <tr><td><?=$row['ttm_code']?></td><td><?=$date_start?></td><td><?=$end_date?></td><td><?=getInDollars($row['deposit'])?></td><td><?=getInDollars($row['withdraw'])?></td><td><?=getInDollars($row['deposit']+$row['withdraw'])?></td>

                <td><?if($row['deposit']==0&&$row['withdraw']==0){?><p class='error'><?=$lang->getLang("Closure won't be saved for this totem")?></p><? } else {?><textarea cols='15' id="<?=$row['ttm_id']?>" style="height:100%;resize: none; "></textarea><?}?></td></tr>

    <?

    }$result->resetRecPtr();
    ?>
    </table>
   <div style='text-align: left'><button id='confirmClosure'><?=$lang->getLang("Confirm")?></button>
            <script>
                $(function(){
                    $( "#confirmClosure")
                        .button()
                        .click(function() {
                        <? $insertsql='';
                        while($result->hasNext()){
                        $row=$result->next();
                         if($row['ttc_end_day']==''){
                                $date_start=substr($row['ttt_time'],0,10);
                         }else {
                                $date_start=$row['ttc_end_day'];
                          }
                                $nid=$row['nation_id'];
                                $rid=$row['region_id'];
                                $did=$row['district_id'];
                                $cid=$row['club_id'];
                                $dep=$row['deposit'];
                                $ww=$row['withdraw'];
                                $fin=$row['deposit']+$row['withdraw'];
                                $cp=$row['perc_club'];
                                $tid=$row['ttm_id'];
                                $casinoperc=$row['perc_casino'];
                                $nationperc=$row['perc_nation'];
                                $regionperc=$row['perc_region'];
                                $districtperc=$row['perc_district'];
                                $notetext="^^^ca:$casinoperc#n:$nationperc#r:$regionperc#d:$districtperc#c:$cp";
                               if($ww==0 && $dep==0){
                               }
                               else{
                                if($result->CurrentIndex>0){
                               if($insertsql!=''){
                                    $insertsql.="@";
                                }
                                }
                        $insertsql.="('$date_start','$end_date','$nid','$rid','$did','$cid','$dep','$ww','$fin','$cp',$tid,'$notetext','".$_SESSION['admin_id']."')";
                        }
                        }
                        $result->resetRecPtr();
                        $insertsql.=";";
                        ?>
                            $.post("totem/totem_query_service.inc",{action:'15',sql:"<?=$insertsql?>", <? while($result->hasNext()){$row=$result->next()?><?=$row['ttm_id']?>:$('#<?=$row["ttm_id"]?>').val(),<?}?>
                                },
                                function(data){
                                    $("#closureresult").empty().append(data);
                                    if($("#success").text()=='Success done!'){
                                        getTotemClosure('<?=$cid?>','club');
                                    }
                                });
                        return false;
                        });

                });
            </script>
        </div>
<?
}

function doTotemClosureByCub($valuestoinsert){
    global $dbh,$lang;
    if($valuestoinsert!='^^^'){
            $sql="INSERT INTO totem_closure(ttc_start_day,ttc_end_day,ttc_jur_nation,ttc_jur_region,ttc_jur_district,ttc_jur_club,ttc_bet,ttc_win,ttc_fin,ttc_perc,ttc_ttm_id,ttc_note,ttc_aus_id)
                 VALUES $valuestoinsert";
        $result=$dbh->exec($sql);
            if(is_int($result)){
                echo '<div class="result" id="success">'.$lang->getLang("Success done!").'</div>';
            }
            else{
                echo '<div class="error">'.$lang->getLang("An error has occurred!").'</div>';
            }
        }
        else{
            echo '<div class="error">'.$lang->getLang("There is no closure for this period!").'</div>';
        }
}


function getTotemClosureIn($startdate=NULL,$enddate=NULL){
        global $dbh,$lang;
        $jurisdictionClass=$_SESSION['jurisdiction_class'];
        $jurisdictionId=$_SESSION['jurisdiction_id'];
        $sql="select ttc_start_day,ttc_end_day,ttc_time,";

        if($jurisdictionClass=='casino'){
            $sql .="n.jur_name jurisdiction_name,n.jur_id jurisdiction_id ";
        }
        elseif($jurisdictionClass=='nation')
        {
            $sql .="r.jur_name jurisdiction_name,r.jur_id jurisdiction_id ";
        }
        elseif($jurisdictionClass=='region')
        {
            $sql .="d.jur_name jurisdiction_name,d.jur_id jurisdiction_id ";
        }
        elseif($jurisdictionClass=='district'){
            $sql .="c.jur_name jurisdiction_name,c.jur_id jurisdiction_id ";
        }
        $sql.=",coalesce(ttc_bet, 0) bet, coalesce(ttc_win, 0) win, coalesce(ttc_rake, 0) rake, coalesce(ttc_perc,0) perc, aus_username,ttc_note,ttc_cash_in_hand,ttc_id,ttc_fin
    FROM jurisdiction n ";

        if($jurisdictionClass=='casino')
        {
            $sql.=" LEFT JOIN totem_closure ON n.jur_id = ttc_jur_nation
                LEFT JOIN admin_user on aus_id=ttc_aus_id
                WHERE n.jur_parent_id  = 1  AND ttc_ttm_id > 0 ";
            if(isset($_POST['jurisdiction'])&&($_POST['jurisdiction']!='*')){
                $sql .=" AND n.jur_id=".$_POST['jurisdiction'];
            }
            if(isset($startdate)){
                $sql.= " AND ttc_start_day >='$startdate'";
            }
            if(isset($enddate)){
                $sql .=" AND ttc_end_day <='$enddate'";
            }
            $sql .=" ORDER BY n.jur_id,ttc_start_day DESC";

        }
        elseif($jurisdictionClass=='nation')
        {
            $sql .="LEFT JOIN jurisdiction r ON n.jur_id = r.jur_parent_id
                LEFT JOIN totem_closure ON r.jur_id = ttc_jur_region
                LEFT JOIN admin_user on aus_id=ttc_aus_id
                WHERE n.jur_id = $jurisdictionId  AND ttc_ttm_id > 0 ";
            if(isset($_POST['jurisdiction'])&&($_POST['jurisdiction']!='*')){
                $sql .=" AND r.jur_id=".$_POST['jurisdiction'];
            }
            if(isset($startdate)){
                $sql.= " AND ttc_start_day >='$startdate'";
            }
            if(isset($enddate)){
                $sql .=" AND ttc_end_day <='$enddate'";
            }
            $sql .=" ORDER BY r.jur_id,ttc_start_day DESC";
        }
        elseif($jurisdictionClass=='region')
        {
            $sql .="LEFT JOIN jurisdiction r ON n.jur_id = r.jur_parent_id
                LEFT JOIN jurisdiction d ON r.jur_id = d.jur_parent_id
                LEFT JOIN totem_closure ON d.jur_id = ttc_jur_district
                LEFT JOIN admin_user on aus_id=ttc_aus_id
                WHERE r.jur_id = $jurisdictionId   AND ttc_ttm_id > 0 ";
            if(isset($_POST['jurisdiction'])&&($_POST['jurisdiction']!='*')){
                $sql .=" AND d.jur_id=".$_POST['jurisdiction'];
            }
            if(isset($startdate)){
                $sql.= " AND ttc_start_day >='$startdate'";
            }
            if(isset($enddate)){
                $sql .=" AND ttc_end_day <='$enddate'";
            }
            $sql .=" ORDER BY d.jur_id,ttc_start_day DESC";
        }
        elseif($jurisdictionClass=='district'){
            $sql .="LEFT JOIN jurisdiction r ON n.jur_id = r.jur_parent_id
                LEFT JOIN jurisdiction d ON r.jur_id = d.jur_parent_id
                LEFT JOIN jurisdiction c ON d.jur_id = c.jur_parent_id
                LEFT JOIN totem_closure ON c.jur_id = ttc_jur_club
                LEFT JOIN admin_user on aus_id=ttc_aus_id
                WHERE d.jur_id = $jurisdictionId   AND ttc_ttm_id > 0 ";
            if(isset($_POST['jurisdiction'])&&($_POST['jurisdiction']!='*')){
                $sql .=" AND c.jur_id=".$_POST['jurisdiction'];
            }
            if(isset($startdate)){
                $sql.= " AND ttc_start_day >='$startdate'";
            }
            if(isset($enddate)){
                $sql .=" AND ttc_end_day <='$enddate'";
            }
            $sql .=" ORDER BY c.jur_id,ttc_start_day DESC";
        }
        /*if($startdate==''){
            $startdate=date('Y-01-01');
         }
         $sql .=" AND ttc_start_day BETWEEN '$startdate' AND ";
         if($enddate==''){
             $enddate=date('Y-m-d');
         }
         $sql .="  '$enddate'";*/
	
        $result = $dbh->exec($sql);
        if($result->getNumRows()==0){
            addError("",$lang->getLang('No result found'));
            showErrors();
        }
        else{
        $parent_details=get_jurisdiction($_SESSION['jurisdiction_id']);
        ?>
    <br><table style="width: 100%">
        <thead>
        <tr>
            <th class='label' colspan='11'><strong class='bodyHD'><?=$lang->getLang("Cash-In")?></strong></th>

        </tr>
        <tr>
            <th class='label'><strong><?=$lang->getLang(ucfirst(getSubJurisdiction($_SESSION['jurisdiction_class'])))?></strong></th>
            <th class='label'><strong><?=$lang->getLang("From")?></strong></th>
            <th class='label'><strong><?=$lang->getLang("Until")?></strong></th>
            <th class='label'><strong><?=$lang->getLang("Total bet")?></strong></th>
            <th class='label'><strong><?=$lang->getLang("Total win")?></strong></th>
            <th class='label'><strong><?=$lang->getLang("Financial")?></strong></th>
            <th class='label'><strong><?=ucfirst($parent_details['jur_name'])?><?=$lang->getLang("Rake")?></strong></th>
            <th class='label'><strong><?=ucfirst(getSubJurisdiction($_SESSION['jurisdiction_class']))?> <?=$lang->getLang("Rake")?></strong></th>
            <!-- <th class='alert alert-info'><strong>Perc</strong></th> -->
            <th class='label'><strong><?=$lang->getLang("Note")?></strong></th>
            <th class='label'><strong><?=$lang->getLang("User")?></strong></th>
            <th class='label'><strong><?=$lang->getLang("Closure time")?></strong></th>

        </tr>
        </thead>
        <tbody>
        <?
        $entity='';
        while($result->hasNext()){
            $row=$result->next();
            if($row['win']==0 && $row['bet']==0 && $row['rake']== 0){
                ?>
                <tr>
                    <td class='text-error'><strong><?=$row['jurisdiction_name']?></strong></td>
                    <td colspan="9" class="text-error"><form action="<?=refFormPage('reports_casino/partial_closure')?>" method='post'><input type='hidden' name='jurisdiction' value='<?=$row['jurisdiction_id']?>'> <strong>Partial Closure never made!</strong> </td>

                </tr>
            <?
            }else{ list($usernote,$detailsnote)=explode('^^^',$row['ttc_note']);
                $totalparentrake=0;
                $detailsnote=explode('#',$detailsnote);
                foreach($detailsnote as $k=>$v){
                    list($type,$typeperc)=explode(":",$v);
                    if($_SESSION['jurisdiction_class']=='casino'){
                        if($type=='n'){
                            $childrake=$typeperc;
                        }

                    }
                    if($_SESSION['jurisdiction_class']=='nation'){
                        if($type=='r'){
                            $childrake=$typeperc;
                        }
                        if($type=='n'){
                            $parentrake=$typeperc;
                        }
                    }
                    if($_SESSION['jurisdiction_class']=='region'){
                        if($type=='d'){
                            $childrake=$typeperc;
                        }
                        if($type=='r'){
                            $parentrake=$typeperc;
                        }
                    }
                    if($_SESSION['jurisdiction_class']=='district'){
                        if($type=='c'){
                            $childrake=$typeperc;
                        }
                        if($type=='d'){
                            $parentrake=$typeperc;
                        }
                    }
                    if($_SESSION['jurisdiction_class']=='club'){
                            $childrake=$typeperc;

                    if($type=='d'){
                        $parentrake=0;
                    }
                    }
                }
                ?>
                <tr>
                    <td class="content"><strong><?=$row['jurisdiction_name']?></strong></td>
                    <td class="content"><strong><?=$row['ttc_start_day']?></strong></td>
                    <td class="content"><strong><?=$row['ttc_end_day']?></strong></td>
                    <td class="content"><strong><?=getInDollars($row['bet'])?></strong></td>
                    <td class="content"><strong><?=getInDollars($row['win'])?></strong></td>
                    <td class="content"><strong><?=getInDollars($row['ttc_fin'])?></strong></td>
                    <td class="content"><strong><?=getInDollars((100-$childrake)*$row['ttc_fin']/100)?>
                     <?if($_SESSION['jurisdiction_class']!='casino'){?>
                        <img src="media/images/calculator.gif"  style='float:right' onclick="toggle('<?=$row['ttc_id']?>')"><p class="closureall" id=<?=$row['ttc_id']?>>
                         <?=getInDollars((($parentrake-$childrake)*$row['ttc_fin'])/100);?></p>
                      <?}?>
                    </td>
                    <td class="content"><strong><?=getInDollars($childrake*$row['ttc_fin']/100)?></strong></td>
                    <!--<td><strong><?/*=$row['perc']*/?> %</strong></td>-->
                    <td class="content"><strong style="width: 90%;" class="fleft">
                            <div id="<?=$row['ttc_id']?>_text" > <?
                                if($usernote!=''){
                                    echo $usernote;
                                }
                                ?></div>
                            <div id="<?=$row['ttc_id']?>_edit" style="display: none;"><textarea style="width:100%;"></textarea><br /><div style='display: block;min-width: 100px;'><input type="button" class="button blue" value="save" style='padding:0px 3px;font-size: 10px;float:left;' onclick="switchBack('<?=$row['ttc_id']?>',true);" /><input type="button" class='button blue' style='padding:0px 3px;font-size: 10px;float: right' value="cancel" onclick="switchBack('<?=$row['ttc_id']?>',false);" /></div></div>
                        </strong>
                        <? if(strtolower($row['aus_username'])==strtolower($_SESSION['admin_username'])) { ?>
                        <img  src="live/liveimage/edit.png" id='<?=$row['ttc_id']?>_img' style='width: 20px;float:right' onclick="switchToEdit('<?=$row['ttc_id']?>_text','<?=$row['ttc_id']?>_edit',this);">
                    <?}?>
                    </td>
                    <td class="content"><strong><?=$row['aus_username']?></strong></td>
                    <td class="content"><strong><?=$row['ttc_time']?></strong></td>
                </tr>

            <?
            }
        }
        ?>
        </tbody>
    </table>
<?
    }
}


function getTotemClosureOut($startdate=NULL,$enddate=NULL){
        global $dbh,$lang;
        $jurisdictionClass=$_SESSION['jurisdiction_class'];
        $jurisdictionId=$_SESSION['jurisdiction_id'];
        $sql="select ttc_start_day,ttc_end_day,ttc_time,";
        if($jurisdictionClass=='nation')
        {
            $sql .="ca.jur_name jurisdiction_name,ca.jur_id jurisdiction_id ";
        }
        elseif($jurisdictionClass=='region')
        {
            $sql .="n.jur_name jurisdiction_name,n.jur_id jurisdiction_id ";
        }
        elseif($jurisdictionClass=='district'){
            $sql .="r.jur_name jurisdiction_name,r.jur_id jurisdiction_id ";
        }
        elseif($jurisdictionClass=='club'){
            $sql .="d.jur_name jurisdiction_name,d.jur_id jurisdiction_id ";
        }
        $sql.=",coalesce(ttc_bet, 0) bet, coalesce(ttc_win, 0) win, coalesce(ttc_rake, 0) rake, coalesce(ttc_perc,0) perc, aus_username,ttc_note,ttc_cash_in_hand,ttc_id,ttc_fin ";

        if($jurisdictionClass=='nation')
        {
            $sql .="FROM jurisdiction ca, jurisdiction n, totem_closure, admin_user
                WHERE ttc_jur_nation = $jurisdictionId AND n.jur_parent_id = ca.jur_id AND aus_id = ttc_aus_id AND ttc_jur_nation = n.jur_id ";
        }
        elseif($jurisdictionClass=='region')
        {
            $sql .=" FROM jurisdiction r
                LEFT JOIN jurisdiction n ON n.jur_id = r.jur_parent_id
                LEFT JOIN totem_closure ON n.jur_id = ttc_jur_nation
                LEFT JOIN admin_user on aus_id=ttc_aus_id
                WHERE r.jur_id = $jurisdictionId  AND ttc_jur_region = r.jur_id";
        }
        elseif($jurisdictionClass=='district'){
            $sql .="FROM jurisdiction d
                LEFT JOIN jurisdiction r ON r.jur_id = d.jur_parent_id
                LEFT JOIN jurisdiction n ON n.jur_id = r.jur_parent_id
                LEFT JOIN totem_closure ON n.jur_id = ttc_jur_nation
                LEFT JOIN admin_user on aus_id=ttc_aus_id
                WHERE d.jur_id = $jurisdictionId   AND ttc_jur_district = d.jur_id";
        }
        elseif($jurisdictionClass=='club'){
            $sql .="FROM jurisdiction c
                LEFT JOIN jurisdiction d ON d.jur_id = c.jur_parent_id
                LEFT JOIN jurisdiction r ON r.jur_id = d.jur_parent_id
                LEFT JOIN jurisdiction n ON n.jur_id = r.jur_parent_id
                LEFT JOIN totem_closure ON n.jur_id = ttc_jur_nation
                LEFT JOIN admin_user on aus_id=ttc_aus_id
                WHERE c.jur_id = $jurisdictionId AND ttc_jur_club = c.jur_id";
        }
        if(isset($startdate)){
            $sql.= " AND ttc_start_day >='$startdate'";
        }
        if(isset($enddate)){
            $sql .=" AND ttc_end_day <='$enddate'";
        }

        /*if($startdate==''){
            $startdate=date('Y-01-01');
         }
         $sql .=" AND ttc_start_day BETWEEN '$startdate' AND ";
         if($enddate==''){
             $enddate=date('Y-m-d');
         }
         $sql .="  '$enddate'";*/
        $result = $dbh->exec($sql);
        if($result->getNumRows()==0){
            addError('','No result found');
            showErrors();
        }
        else{
        ?>
    <br><table>
        <thead>
        <tr>
            <th class='label' colspan='11'><strong class='bodyHD'><?=$lang->getLang("Cash-Out")?></strong></th>
        </tr>
        <tr>
            <th class='label'><strong><?=$lang->getLang(ucfirst(getSuperJurisdiction($_SESSION['jurisdiction_class'])))?></strong></th>
            <th class='label'><strong><?=$lang->getLang("Start Date")?></strong></th>
            <th class='label'><strong><?=$lang->getLang("End Date")?></strong></th>
            <th class='label'><strong><?=$lang->getLang("Total bet")?></strong></th>
            <th class='label'><strong><?=$lang->getLang("Total win")?></strong></th>
            <th class='label'><strong><?=$lang->getLang("Financial")?></strong></th>
            <th class='label'><strong><?=$lang->getLang(ucfirst(getSuperJurisdiction($_SESSION['jurisdiction_class'])))?> <?=$lang->getLang("Rake")?></strong></th>
            <th class='label'><strong><?=$lang->getLang(ucfirst($_SESSION['jurisdiction_class']))?> <?=$lang->getLang("Rake")?></strong></th>
            <!-- <th class='alert alert-info'><strong>Perc</strong></th>-->
            <th class='label'><strong><?=$lang->getLang("Note")?></strong></th>
            <th class='label'><strong><?=$lang->getLang("User")?></strong></th>
            <th class='label'><strong><?=$lang->getLang("Closure time")?></strong></th>
        </tr>
        </thead>
        <tbody>
        <?  while($result->hasNext()){
            $row=$result->next();
            if($row['win']==0 && $row['bet']==0 && $row['rake']==0){
                ?>
                <tr>
                    <td><strong><?=$row['jurisdiction_name']?></strong></td>
                    <td colspan="9" class="text-error"><form action="<?=refFormPage('reports_casino/partial_closure
                            ')?>" method='post'><input type='hidden' name='jurisdiction' value='<?=$row['jurisdiction_id']?>'> <strong>Partial Closure never made.Please </strong> <button class="btn btn-danger" >Make first closure</button></form> </td>
                </tr>
            <?
            }else{
                list($usernote,$detailsnote)=explode('^^^',$row['ttc_note']);
                $totalparentrake=0;
                $detailsnote=explode('#',$detailsnote);
                foreach($detailsnote as $k=>$v){
                    list($type,$typeperc)=explode(":",$v);
                    if($_SESSION['jurisdiction_class']=='nation'){
                        if($type=='n'){
                            $parentrake=$typeperc;
                        }
                    }
                    if($_SESSION['jurisdiction_class']=='region'){
                        if($type=='r'){
                            $parentrake=$typeperc;
                        }
                    }
                    if($_SESSION['jurisdiction_class']=='district'){
                        if($type=='d'){
                            $parentrake=$typeperc;
                        }
                    }
                    if($_SESSION['jurisdiction_class']=='club'){
                        if($type=='c'){
                            $parentrake=$typeperc;
                        }
                    }
                }
                ?>
                <tr>
                    <td class='content'><strong><?=$row['jurisdiction_name']?></strong></td>
                    <td class='content'><strong><?=$row['ttc_start_day']?></strong></td>
                    <td class='content'><strong><?=$row['ttc_end_day']?></strong></td>
                    <td class='content'><strong><?=getInDollars($row['bet'])?></strong></td>
                    <td class='content'><strong><?=getInDollars($row['win'])?></strong></td>
                    <td class='content'><strong><?=getInDollars($row['ttc_fin'])?></strong></td>
                    <td class="content"><strong><?=getInDollars((100-$parentrake)*$row['ttc_fin']/100)?></td>
                    <td class="content"><strong><?=getInDollars($parentrake*$row['ttc_fin']/100)?></strong></td>
                    <!-- <td><strong><?/*=$row['perc']*/?> %</strong></td>-->
                    <td class='content'><strong><?=$usernote?></strong></td>
                    <td class='content'><strong><?=$row['aus_username']?></strong></td>
                    <td class='content'><strong><?=$row['ttc_time']?></strong></td>
                </tr>

            <?
            }
        }
        ?>
        </tbody>
    </table>
<?
    }
}


/*function searchTotemClosure(){
            global $dbh;
            $jurisdiction=$_SESSION['jurisdiction_id'];
            $jurisdiction_class=$_SESSION['jurisdiction_class'];
            $sql="SELECT * from totem_closure where ttc_jur_$jurisdiction_class=$jurisdiction and ttc_ttm_id !='0'";
            $result=$dbh->doCachedQuery($sql,0);
            */?><!--
            <br></r><table style="width: 100%">
                <tr>
                    <td class="label">Totem</td>
                    <td class="label">Start Date</td>
                    <td class="label">End Date</td>
                    <td class="label">Total Bet</td>
                    <td class="label">Total Win</td>
                    <td class="label">Financial</td>
                    <td class="label">Note</td>
                    <td class="label">User</td>
                </tr>
            <?/*
            while($result->hasNext()){
               $row=$result->next();
                */?>
                <tr>
                    <td class="content"><?/*=$row['ttc_ttm_id']*/?></td>
                    <td class="content"><?/*=$row['ttc_start_day']*/?></td>
                    <td class="content"><?/*=$row['ttc_end_day']*/?></td>
                    <td class="content"><?/*=$row['ttc_bet']*/?></td>
                    <td class="content"><?/*=$row['ttc_win']*/?></td>
                    <td class="content"><?/*=$row['ttc_fin']*/?></td>
                    <td class="content"><?/*=$row['ttc_note']*/?></td>
                    <td class="content"><?/*=$row['ttc_aus_id']*/?></td>
                </tr>
                <?/*
            }
            */?>
            </table>
            -->
<?
/*}*/


function getSubJurisdiction($jurisdictionClass){
    $subjurisdiction='';

    if($jurisdictionClass=='casino')
    {
        $subjurisdiction='nation';
    }
    elseif($jurisdictionClass=='nation')
    {
        $subjurisdiction='region';
    }
    elseif($jurisdictionClass=='region')
    {
        $subjurisdiction='district';
    }
    elseif($jurisdictionClass=='district'){
        $subjurisdiction='club';
    }
    return $subjurisdiction;
}

function getSuperJurisdiction($jurisdictionClass){
    $subjurisdiction='';

    if($jurisdictionClass=='region')
    {
        $subjurisdiction='nation';
    }
    elseif($jurisdictionClass=='nation')
    {
        $subjurisdiction='casino';
    }
    elseif($jurisdictionClass=='district')
    {
        $subjurisdiction='nation';
    }
    elseif($jurisdictionClass=='club'){
        $subjurisdiction='district';
    }
    return $subjurisdiction;
}
?>

<!--SELECT min(ttt_time), max(ttc_end_day),ttm_code, ttm_id, c.jur_name club_name, c.jur_id club_id, d.jur_name district_name, d.jur_id district_id, r.jur_name region_name, r.jur_id region_id, n.jur_name nation_name, n.jur_id nation_id, COALESCE( jps1.jps_perc_totem, 0 ) AS perc_casino,
COALESCE( jps2.jps_perc_totem, 0 ) AS perc_nation,
COALESCE( jps3.jps_perc_totem, 0 ) AS perc_region,
COALESCE( jps4.jps_perc_totem, 0 ) AS perc_district,
COALESCE( jps5.jps_perc_totem, 0 ) AS perc_club,
SUM( IF( ttt_amount > 0 and ttt_time BETWEEN '2012-01-01 00:00:00' and '2013-06-01 23:59:59', ttt_amount, 0 ) ) AS deposit,
SUM( IF( ttt_amount <0 and ttt_time BETWEEN '2012-01-01 00:00:00' and '2013-06-01 23:59:59', ttt_amount, 0 ) ) AS withdraw
FROM totem, punter, jurisdiction c LEFT JOIN jurisdiction_payment_settings jps5 ON c.jur_id = jps5.jps_jur_id,
jurisdiction d LEFT JOIN jurisdiction_payment_settings jps4 ON d.jur_id = jps4.jps_jur_id,
jurisdiction r LEFT JOIN jurisdiction_payment_settings jps3 ON r.jur_id = jps3.jps_jur_id,
jurisdiction n LEFT JOIN jurisdiction_payment_settings jps2 ON n.jur_id = jps2.jps_jur_id,
jurisdiction ca LEFT JOIN jurisdiction_payment_settings jps1 ON ca.jur_id = jps1.jps_jur_id,
totem_transtrack LEFT JOIN totem_closure ON ttt_ttm_id = ttc_ttm_id
WHERE n.jur_parent_id = 1 and r.jur_parent_id = n.jur_id and d.jur_parent_id = r.jur_id and c.jur_parent_id = d.jur_id
and c.jur_id = pun_betting_club and pun_id = ttt_pun_id and ttt_ttm_id = ttm_id and ttt_time BETWEEN '2012-01-01 00:00:00' and '2013-06-01 23:59:59' GROUP BY c.jur_id, ttm_id
order by ttm_code






select pun_betting_club, ttt_ttm_id, SUM( IF( ttt_amount > 0 and ttt_time BETWEEN '2012-01-01 00:00:00' and '2013-06-01 23:59:59', ttt_amount, 0 ) ) AS deposit,
SUM( IF( ttt_amount <0 and ttt_time BETWEEN '2012-01-01 00:00:00' and '2013-06-01 23:59:59', ttt_amount, 0 ) ) AS withdraw
from punter, totem_transtrack
where ttt_pun_id = pun_id
and pun_betting_club =  106
and ttt_ttm_id = 170
and ttt_time BETWEEN '2012-01-01 00:00:00' and '2013-06-01 23:59:59'
-->