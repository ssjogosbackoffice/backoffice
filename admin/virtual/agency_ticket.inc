<?php check_access('virtual_view_agency_ticket', true);?>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<script src="/media/jquery.dataTables.js" type="text/javascript"></script>
<script type="text/javascript">
    $(function() {
        $( "#date_start" ).datepicker({
            changeMonth: true,
            numberOfMonths:1
        });
        $(".display").dataTable({'sPaginationType':"full_numbers"});
    });
</script>

<?php
/**
 *
 * @author Marian
 */
form_head()?>
<?php printHiddenInput('doQuery', 'yes');?>
<?php printHiddenInput('search_type', 'date_range');?>
<?php
$doQuery = $_POST['doQuery'];
$row_to_page = (isset($_POST['resforpage'])) ? $_POST['resforpage'] : 20;
$date_start = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d", time());
$tid = $_POST['ticketid'] ;
$game=$_POST['games'];

?>
    <table class="table table-bordered">
        <tr>
            <td class="navbar-inner" ><h3 class="text-center"><?=$lang->getLang("Search ticket")?></h3></td>
        </tr>
        <tr>
            <td>
                <div class="well">
                    <div class="row">
                        <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Date")?></div>
                            <br>
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-random"></i></span>
                                    <input type='text' name='date_start' id='date_start'  value="<?=$date_start?>" >
                                </div>
                            </div>
                        </div>

                        <?php
                        $sql = "select gam_id, gam_name from game where gam_category='casino_agency'";
                        $resGames = $dbh->doCachedQuery($sql,0);
                        $gameList = array();
                        while($resGames->hasNext()){
                            $row = $resGames->next();
                            $gameList[$row['gam_id']] = $row['gam_name'];
                        }
                        $activatedGames = $_POST['games'];
                        ?>
                        <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Games")?></div>
                            <br>
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-random"></i></span>
                                    <select name=games>
                                        <?php foreach($gameList as $key => $value){
                                            ?>
                                            <option  value="<?=$key?>"<?php if($activatedGames==$key){?>selected="selected" <?php  }?>  ><?=$value?> </option><?php
                                        } ?>
                                    </select>
                                    <input type=hidden name="previousgame" value="<?=$activatedGames?>">
                                </div>
                            </div>
                        </div>

                        <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Ticket Id")?></div>
                            <br>
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-lock"></i></span>
                                    <input type='text'  name='ticketid' id='ticketid' placeholder="<?=$lang->getLang("Ticket Id ( Optional )")?>" maxlength="32" value="">
                                </div>
                            </div>
                        </div>
                    </div>
                    <div class="row">
                        <span class="span2"> <button class="btn btn-primary" id="SearchTicket"><?=$lang->getLang("Search")?></button></span>
                    </div><br />
                </div>

                <?php
                if($doQuery){
                if($date_start ==''){
                    addError("", $lang->getLang("Please choose a date"));
                }
                /*   if($tid==''){
                       addError("", $lang->getLang("Please enter the ticked id"));
                   }*/
                if($game==''){
                    addError("", $lang->getLang("Please choose a game"));
                }

                if(areErrors()){
                    showErrors();
                }
                else{
                $tid=  preg_replace('#\D.*$#', '', $tid);

                if($game==215){

                    $sql="
        SELECT  pun_username,
                gam_name,
                IDTicket PRE_RES_ID,
                CodiceTicket pre_game_num,
                c.jur_name club_name,
               IDUtente PRE_PUN_ID,
               IF(TStato <> 4, coalesce(Importo, 0) + coalesce(Tax, 0), 0) PRE_BET,
               if (Tax>0 AND  Importo > 600, (Importo - 600) * 0.25, 0) tot_win_tax,
               TVincita PRE_WIN,
               IPAddress PRE_PUN_IP,
               TStamp pre_time,
               '-' PRE_BALANCE,
               Pagato is_payed,
               Tstato,
               TAccreditato,
               Tax pre_bet_tax,
               c.jur_id jur_id,
               cur_code_for_web
               FROM Ticket_dev, virtual_games_user, punter, jurisdiction c, jurisdiction d, jurisdiction r, jurisdiction n,game,currency
               WHERE ExtIDUtente = vgu_pun_id
               AND pun_id = vgu_pun_id
                AND TStamp BETWEEN  '$date_start 00:00:00' AND '$date_start 23:59:59'
                AND vgu_gam_id=gam_id
                AND c.jur_id = PUN_BETTING_CLUB
                AND c.jur_parent_id = d.jur_id
                and c.jur_currency=cur_id
                AND d.jur_parent_id = r. jur_id
                AND r.jur_parent_id = n.jur_id
                AND n.jur_parent_id = 1 ";
                    $jur_id = $_SESSION["jurisdiction_id"];
                    switch($_SESSION["jurisdiction_class"]){
                        case "club":
                            $sql .= " AND c.jur_id = $jur_id";
                            break;
                        case "district":
                            $sql .= " AND d.jur_id = $jur_id";
                            break;
                        case "region":
                            $sql .= " AND r.jur_id = $jur_id";
                            break;
                        case "nation":
                            $sql .= " AND n.jur_id = $jur_id";
                            break;
                    }
                    if($tid!=''){ $sql.=" AND (CodiceTicket = $tid OR IDTicket=$tid) ";}
                }
                elseif ($game==216){


        $sql=" SELECT  pun_username,
               gam_name,
               LTK_ID PRE_RES_ID,
               ltk_code pre_game_num,
               c.jur_name club_name,
               LTK_PUN_ID PRE_PUN_ID,
               IF(LTK_STATE <> 4, coalesce(LTK_AMOUNT, 0) + coalesce(LTK_TAX, 0), 0) PRE_BET,
               if (LTK_TAX>0 AND LTK_AMOUNT > 600, (LTK_AMOUNT - 600) * 0.25, 0) tot_win_tax, LTK_WIN PRE_WIN,
               LTK_IPADD PRE_PUN_IP, LTK_TSTAMP pre_time, '-' PRE_BALANCE, LTK_PAYED is_payed, LTK_STATE, '-' TAccreditato,
               LTK_TAX pre_bet_tax,
               c.jur_id jur_id,
               cur_code_for_web
               FROM live_tickets, virtual_games_user, punter, jurisdiction c, jurisdiction d, jurisdiction r, jurisdiction n,game,currency
               WHERE LTK_PUN_ID = vgu_pun_id
               AND pun_id = vgu_pun_id
                AND LTK_TSTAMP BETWEEN  '$date_start 00:00:00' AND '$date_start 23:59:59'
                AND vgu_gam_id=gam_id
                AND c.jur_id = PUN_BETTING_CLUB
                AND c.jur_parent_id = d.jur_id
                and c.jur_currency=cur_id
                AND d.jur_parent_id = r. jur_id
                AND r.jur_parent_id = n.jur_id
                AND n.jur_parent_id = 1 ";
                    $jur_id = $_SESSION["jurisdiction_id"];
                    switch($_SESSION["jurisdiction_class"]){
                        case "club":
                            $sql .= " AND c.jur_id = $jur_id";
                            break;
                        case "district":
                            $sql .= " AND d.jur_id = $jur_id";
                            break;
                        case "region":
                            $sql .= " AND r.jur_id = $jur_id";
                            break;
                        case "nation":
                            $sql .= " AND n.jur_id = $jur_id";
                            break;
                    }
                    if($tid!=''){ $sql.=" AND (CodiceTicket = $tid OR IDTicket=$tid) ";}


                }

                else{
                $sql=" SELECT pun_username,
                                  PRE_RES_ID,
                                  pre_game_num,
                                  PRE_PUN_ID,
                                  PRE_BET,
                                  PRE_WIN,
                                  PRE_BALANCE,
                                  PRE_TBL_ID,
                                  PRE_PUN_IP,
                                  pre_bet_string ,
                                  res_id,
                                  pre_time,
                                  gam_name, pre_balance,
                                  IF (pre_bets_paid like '%payed', 1, 0) is_payed,
                                  if (pre_bet_string like '%handerased', 1, 0) is_erased,
                                  pre_win_tax,
                                  cur_code_for_web ";
                    if(TAX_CALCULATION_ACTIVE=='ON'){
                        $sql.= " ,pre_bet_tax ";
                    }
                     $sql.="      FROM punter_result, result, game,punter
                     JOIN jurisdiction club     ON club.jur_id     = pun_betting_club
                     JOIN currency on cur_id=jur_currency ";

                $jur_id = $_SESSION["jurisdiction_id"];

                if($_SESSION["jurisdiction_class"] != "casino"){
                    $sql .= "
  		                            JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
                                    JOIN jurisdiction regional ON regional.jur_id = district.jur_parent_id
                                    JOIN jurisdiction nation   ON nation.jur_id   = regional.jur_parent_id";
                }

                switch($_SESSION["jurisdiction_class"]){
                    case "club":
                        $sql .= " WHERE club.jur_id = $jur_id";
                        break;
                    case "district":
                        $sql .= " WHERE district.jur_id = $jur_id";
                        break;
                    case "region":
                        $sql .= " WHERE regional.jur_id = $jur_id";
                        break;
                    case "nation":
                        $sql .= " WHERE nation.jur_id = $jur_id";
                        break;
                    case "casino":
                        $sql .= " WHERE 1=1";
                        break;
                    default:
                        $sql .= " WHERE 1=0";
                        break;
                }
                $sql.=" AND res_id=pre_res_id
                            AND res_gam_id=$game
                            AND gam_id = res_gam_id
                            AND pre_pun_id = pun_id
                            AND pre_time BETWEEN  '$date_start 00:00:00' AND '$date_start 23:59:59' ";

                if($tid!='') $sql.=" AND pre_game_num = $tid";
                }
                $starttime = microtime(true);

                $rs=$dbh->exec($sql);
                $endtime = microtime(true);
                $duration = $endtime - $starttime;
                $duration=number_format($duration, 4, ',', '')."s";
                $time="<div class='tip fleft'>".$lang->getLang("Time taken to execute your request").":".$duration."</div><br /><br /><br />";
                $betting='';
                if($game==215 || $game==216){
                    $betting="&betting=true";
                }

                $numrow=$rs->getNumRows();
                echo $time;
                if($numrow>0){?>

                <table class='display  table table-striped table-bordered table-condensed'>
                    <thead>
                    <tr>
                        <th class="unwrappable"><?=$lang->getLang("Hand ID")?></th>
                        <th class="unwrappable"><?=$lang->getLang("Game Nr")?></th>
                        <th><?=$lang->getLang("Date")?></th>
                        <th class="unwrappable"><?=$lang->getLang("User Name")?></th>
                        <th><?=$lang->getLang("Game")?></th>
                        <th><?=$lang->getLang("Bet")?></th>
                        <?if(TAX_CALCULATION_ACTIVE=='ON'){ ?>
                        <th class="unwrappable"><?=$lang->getLang("Bet Tax")?></th>
                        <? } ?>
                        <th><?=$lang->getLang("Win")?></th>
                        <?if(check_access('virtual_cashier_show_credit')){ ?>
                            <th><?=$lang->getLang("Balance")?></th>
                         <? }?>
                        <th><?=$lang->getLang("Status")?></th>
                        <th><?=$lang->getLang("Ip")?></th>
                    </tr>
                    </thead>
                    <?
                    while($rs->hasNext()){
                        $row=$rs->next();
                        ?><tr>
                        <td><?=getClassLink("javascript:openWindow('/casino_games/hand_details.php?gmn={$row["pre_game_num"]}&uid={$row["pre_pun_id"]}&hand_id={$row["pre_res_id"]}".$betting."','hand{$row["pre_res_id"]}','950','980','0','1')", $row["pre_res_id"], "contentlink");?></td>
                        <td><?=getClassLink("javascript:openWindow('/casino_games/hand_details.php?gmn={$row["pre_game_num"]}&uid={$row["pre_pun_id"]}&hand_id={$row["pre_res_id"]}".$betting."','hand{$row["pre_res_id"]}','950','980','0','1')", $row["pre_game_num"], "contentlink");?></td>
                        <td><?=($row["pre_time"]);?></td>
                        <td><?=getClassLink("javascript:openWindow('".refPage("customers/customer_view&customer_id={$row["pre_pun_id"]}")."','user{$row["pun_username"]}','950','980','0','1')", $row["pun_username"], "contentlink");?></td>
                        <td class="unwrappable"><?=getGamesName($row["gam_name"]);?></td>
                        <td><?=getInDollars($row["pre_bet"],2,$row['cur_code_for_web']);?></td>
                        <?if(TAX_CALCULATION_ACTIVE=='ON'){ ?>
                        <td class="unwrappable"><?=($row['pre_bet_tax'] !='' ? getInDollars(($row['pre_bet']- $row['pre_bet_tax']),3,$row['cur_code_for_web'])."<br />
                              <span class='tip'>Tax : ".getInDollars($row['pre_bet_tax'],3,$row['cur_code_for_web'])."</span>" : '--' )?></td>
                        <? } ?>
                        <td><?=getInDollars($row["pre_win"],2,$row['cur_code_for_web']);?><br /> <span class='tip'>Tax : <?=getInDollars($row['pre_win_tax'],3)?></span></td>
                        <?if(check_access('virtual_cashier_show_credit')){ ?>
                            <td><?=getInDollars($row["pre_balance"],2,$row['cur_code_for_web']);?></td>
                        <? }?>
                        <td><?if($row["is_erased"]=='1' || ($game==215 && $row['tstato']==4)){?>
                           <span class="error uppercased"> <?=$lang->getLang('Erased')?></span>
                            <?}elseif($row["is_payed"]=='1') {?>
                                <span class="result uppercased"> <?=$lang->getLang('Paid')?></span>
                           <? }else{?>
                                <span class="text-error uppercased"> <?=$lang->getLang('Not paid')?></span>
                            <?}?>
                            </td>
                        <td><?=getClassLink("javascript:openWindow('".refPage('customers/search&field=4&search_string=' . urlencode(ereg_replace("[^0-9.]", "", $row["pre_pun_ip"]))) ."','ip','950','980','0','1')", $row["pre_pun_ip"], "contentlink");?></td>
                        </tr>
                    <? }
                    }else{
                        addError("", $lang->getLang("No result found"));
                        showErrors();
                    }
                    }
                    }
                    ?>
                    </td>
                    </tr>
                </table>
            </td>
        </tr>
    </table>