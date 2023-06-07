<script type="text/javascript">
function show(id) {
    var state = document.getElementById(id).style.display;
        if (state == 'block') {
            document.getElementById(id).style.display = 'block';
        } else {
            document.getElementById(id).style.display = 'block';
        }
    }
$(function() {
$( "#date_start" ).datepicker({
    changeMonth: true,
    numberOfMonths:1
    });
});

</script>
<?php 
//@Marian
check_access('live_currently_hands',true);
?>
<?php form_head()?>
<?php printHiddenInput('doQuery', 'yes');?>
<?php printHiddenInput('search_type', 'date_range');?>
<?php  
    require_once 'PHPExcel.php';
    require_once 'PHPExcel/Writer/Excel2007.php';
    require_once 'PHPExcel/Style/Fill.php';
    require_once 'PHPExcel/Style/Color.php';
	$doQuery = $_POST['doQuery'];
    $row_to_page = (isset($_POST['resforpage'])) ? $_POST['resforpage'] : 20;
    $date_start = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d", time());
    $start_row =  1;
    $hours=$_POST['hours'];
?>
<table align="center" cellpadding="4" cellspacing="1" border="0">
  <tr valign=middle>
    <td class=label> <?=$lang->getLang("Date")?></td>
    <td class=content>
        <input type='text' name='date_start' id='date_start'  value="<?=$date_start?>" >
    </td>
  </tr>
    <tr valign=middle>
        <td class=label><?=$lang->getLang("Choose hours")?></td>
        <td class=content>
            <select name="hours" size="6" >
                <option value="0" <?php if($hours=='0'){ ?> selected="selected" <?php } ?>>00:00 - 04:00</option>
                <option value="1" <?php if($hours=='1'){ ?> selected="selected" <?php } ?>>04:00 - 08:00</option>
                <option value="2" <?php if($hours=='2'){ ?> selected="selected" <?php } ?>>08:00 - 12:00</option>
                <option value="3" <?php if($hours=='3'){ ?> selected="selected" <?php } ?>>12:00 - 16:00</option>
                <option value="4" <?php if($hours=='4'){ ?> selected="selected" <?php } ?>>16:00 - 20:00</option>
                <option value="5" <?php if($hours=='5'){ ?> selected="selected" <?php } ?>>20:00 - 23:59</option>
            </select>
        </td>
    </tr>
    <?php
     $sql = "select gam_id, gam_name from game where gam_category='casino_live'";
     $resGames = $dbh->doCachedQuery($sql,0);
     $gameList = array();
     while($resGames->hasNext()){
     	$row = $resGames->next();
     	$gameList[$row['gam_id']] = $row['gam_name'];     	
     }
     $activatedGames = $_POST['games'];     
  ?>
   <tr>
     <td class="label"><?=$lang->getLang("Select the games")?></td>
 	 <td>
 	  <p style=" width:300px; overflow: auto; color: #000; margin-bottom: 1.5em;">
 	    <select name=games>
  <option value=""><?=$lang->getLang("Please choose a game")?></option>
  <?php foreach($gameList as $key => $value){
  			   ?>
  			   <option  value="<?=$key?>"<?php if($activatedGames==$key){?>selected="selected" <?php  }?>  ><?=$value?> </option><?php
 	       } ?>
 	       </select>  
 	       <input type=hidden name="previousgame" value="<?=$activatedGames?>">
 	   </p>
 	   </td>
 	</tr>

    <tr>
        <td class="label"><?=$lang->getLang("Result in Page")?></td>
        <td class="content"><select name="resforpage">
                <option <?php if($row_to_page == 20){?> selected <?php }?>
                    value="20">20</option>
                <option <?php if($row_to_page == 50){?> selected <?php }?>
                    value="50">50</option>
                <option <?php if($row_to_page == 100){?> selected <?php }?>
                    value="100">100</option>
            </select></td>
    </tr>
  <tr>
    <td class=content> </td>
    <td class=content><input type="submit" value="<?=$lang->getLang("View")?>" onclick="this.form.start_row.value=0;this.form.submit()" /></td>
  </tr>
  </table>
    <?php
   if($doQuery){  	
     	if($activatedGames ==''){
            addError("", $lang->getLang("You have to select at least one game!"));

    }
       if($hours ==''){
           addError("",  $lang->getLang("Please select hours!"));

       }
       if($hours=='0')
       {
         $date_start_search="00:00";
         $date_end_search="04:00";
       }
       if($hours=='1')
       {
           $date_start_search="04:00";
           $date_end_search="08:00";
       }
       if($hours=='2')
       {
           $date_start_search="08:00";
           $date_end_search="12:00";
       }
       if($hours=='3')
       {
           $date_start_search="12:00";
           $date_end_search="16:00";
       }
       if($hours=='4')
       {
           $date_start_search="16:00";
           $date_end_search="20:00";
       }
       if($hours=='5')
       {
           $date_start_search="20:00";
           $date_end_search="23:59";
       }

    $sql="SELECT r.res_id, r.res_date, r.res_string, pre_bet, pre_win, pun_username,pre_game_num,pre_pun_id,pre_res_id,";
    if(DOT_IT==true)
	  {
	  	$sql .="pre_external_id,";
	  }   
	   $sql.="pre_pun_ip
			FROM result r inner join (SELECT res_id from (select r2.res_id from result r2, punter_result pr2
			where r2.res_id = pr2.pre_res_id
			and r2.res_gam_id = '$activatedGames'
			and r2.res_date BETWEEN '$date_start $date_start_search'
			AND '$date_start $date_end_search'
			GROUP BY r2.res_id 
			ORDER BY r2.res_date DESC
			limit 20000) user_rank, (select @rownum:=0) r
			) b on r.res_id = b.res_id, punter_result pr, punter
			where pr.pre_res_id = r.res_id
			and pre_pun_id = pun_id";
       	   $resultGames = $dbh->exec($sql);
    $result = array();    
    $numrow=$resultGames->getNumRows();   
    while($resultGames->hasNext()){
        $row = $resultGames->next();
        $erased=strrpos($row["res_string"], 'handerased');
  if(is_null($row["res_string"]))
  {
  	$row["res_string"]="<font color='blue'>".$lang->getLang("Running")."</font>";
  } 
  elseif($erased){  
  $row["res_string"]="<font color='red'>".$lang->getLang("Hand erased")."</font>";
  }  
  else 
  {
  	$row["res_string"]="<font color='green'>".$lang->getLang("Completed")."</font>";
  }
        array_push($result,$row);
  } 
  $rss= new Recordset($result); 

if(isset($_POST['previousgame']))
{
	if($_POST['previousgame']!=$activatedGames){
		$start_row=1;
	}
}

  if($numrow!=0){
          ?>
      <div id="paginatorlive">
          <table style="width:100%;" id="paginator">
          <tr class="header">
              <th class="label centered"><?=$lang->getLang('Date')?></th>

                  <th class="label centered"><?=$lang->getLang('Hand ID')?></th>
                  <th class="label centered"><?=$lang->getLang('Game Nr')?></th>


              <?php
              if(DOT_IT!=false)
              {
                ?>
                  <th class="label centered">AAMS ID</th>
              <?php
              }
                ?>
                  <th class="label centered"><?=$lang->getLang('Username')?></th>
                  <th class="label centered"><?=$lang->getLang("Ip")?></th>
                  <th class="label centered"><?=$lang->getLang("Bet")?></th>
                  <th class="label centered"><?=$lang->getLang('Win')?></th>
                  <th class="label centered"><?=$lang->getLang('Status')?></th>
          </tr>
          <?
          while($rss->hasNext()){
              $row = $rss->next();
              ?>
              <tr>
                  <td class="content" style="text-align:right"><?=$row["res_date"]?></td>
                  <td class="content" style="text-align:right"><?=getClassLink("javascript:openWindow('/casino_games/hand_details.php?gmn={$row["pre_game_num"]}&uid={$row["pre_pun_id"]}&hand_id={$row["res_id"]}','hand{$row["pre_res_id"]}','950','980','0','1')", $row["res_id"], "contentlink");?></td>
                  <td class="content" style="text-align:right"><?=getClassLink("javascript:openWindow('/casino_games/hand_details.php?gmn={$row["pre_game_num"]}&uid={$row["pre_pun_id"]}&hand_id={$row["res_id"]}','hand{$row["pre_res_id"]}','950','980','0','1')", $row["pre_game_num"], "contentlink");?></td>
                  <?php
                  if(DOT_IT!=false)
                  {
                      ?>
                      <td class="content" style="text-align:right"><?= $row["pre_external_id"]?></td>
                  <?php }
                   ?>
                      <td class="content" style="text-align:right"><?=getClassLink( refPage("customers/customer_view&customer_id={$row['pre_pun_id']}") , $row["pun_username"]);?></td>
                      <td class="content" style="text-align:right"><?=getClassLink(refPage("customers/search&field=4&search_string=" . urlencode(preg_replace('[^0-9\.]', "", $row["pre_pun_ip"]))), $row["pre_pun_ip"])?></td>
                      <td class="content" style="text-align:right"><?=getInDollars($row["pre_bet"])?></td>
                      <td class="content" style="text-align:right"><?=getInDollars($row["pre_win"])?></td>
                      <td class="content" style="text-align:right"><?=$row["res_string"]?></td>
              </tr>
              <? } ?>
             <?if ($rss->NumRows>$row_to_page){?>
              <div id="paginator_control" style="width:100%">
                  <link href="media/smartpaginator/smartpaginator.css" rel="stylesheet" type="text/css" />
                  <script src="media/smartpaginator/smartpaginator.js" type="text/javascript"></script>
                  <script>
                      $('#paginator_control').smartpaginator({ totalrecords:<?=$rss->NumRows?>,
                          recordsperpage: <?=$row_to_page?>,
                          datacontainer: 'paginator',
                          dataelement: 'tr',
                         theme: 'black'
                      });
                      show('paginatorlive');
                  </script>
              </div>

          <?
          }
              else{
                  ?>
                  <script>
                      show('paginatorlive');
                  </script>
                    <?
              }
      ?>
      </div>
  <?php

  }else{
    addError("", $lang->getLang("No result found"));
    showErrors();
  }
}  
?>