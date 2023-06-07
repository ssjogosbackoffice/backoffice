<?
header('Content-type:text/html; charset=utf-8');
globalise('hand_id');
$symbol =$_GET['curr'];
require_once 'GameTranscript.class.inc';
require_once 'transcript.inc.php';

?>
<HTML>
<HEAD>
    <TITLE>Hand #<?=$hand_id?></TITLE>
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/style.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/fancybox/source/jquery.fancybox.css" >

    <script type="text/javascript" src="<?= secure_host ?>/media/jquery.js" ></script>
    <script type="text/javascript" src="<?= secure_host ?>/media/swfobject.js" ></script>
    <script src="<?= secure_host ?>/media/fancybox/source/jquery.fancybox.js" type="text/javascript"></script>
    <style>
        .card{
            float:none;
            display:table-cell;
            min-width:40px;
            height:60px;
            margin:2px;
            padding:1px;
            background-color:#FFF;
            background-size:100% 100%;
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
            color:green;
            font-weight:bold;
            text-decoration:blink;
        }

    </style>
    <script type="text/javascript">
        function blinkSymbols2(symbols , symbols_pos,part){
            $(".symbol").removeClass("blink_line");
            $(".symbol").removeClass("blink");
            blinking_pos     = symbols_pos.split(";");

            for(var i = 0 ; i < blinking_pos.length ; i++){
                $("#sym_" + blinking_pos[i]+"_"+part).addClass("blink_line");
            }
            if(symbols != undefined){
                blinking_symbols = symbols.split("|");
                for(var i = 0 ; i < blinking_symbols.length ; i++){
                    $("#sym_" + blinking_symbols[i]+"_"+part).removeClass("blink_line");
                    $("#sym_" + blinking_symbols[i]+"_"+part).addClass("blink").fadeOut().fadeIn();
                }
            }
        }

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
            $(".metalline").click(function(){
                blinkSymbols2($(this).attr("line"),$(this).attr("line_pos"),$(this).attr("part"));
            });

            //openPaytable();
        });

    </script>
</HEAD>
<body leftmargin="0" topmargin="0" rightmargin="0" bottommargin="0" marginwidth="0" class=body>
<div id="urlContent">
    <div id="bettingHistoryContainer"></div>
    <?if($_GET['betting']=='true'){ ?>
        <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
        <script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
        <script>
            var bettingUrl='<?=$bettingUrl?>';
            $(function(){
                try {
                    $.post("../services/general_services.inc",{action:"getBettingHandHistory",ticket:"<?=$_GET['gmn']?>"}, function(data){
                        data=JSON.parse(data);
                        $('#bettingHistoryContainer').empty().append(data['html']);
                    });
                }
                catch(err) {
                    alert('<?=$lang->getLang("An error has occured.Please try again later")?>');
                }
            })
        </script>
    <?
    }
    else{
        ?>
        <?handsdetail($hand_id)?>
    <? } ?>

</div>
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
    global  $dbh,$players,$lang,$symbol;
    $hand_id=$dbh->escape($hand_id);

    $sql = "SELECT res_id,";
    if(DOT_IT==true)
    {
        $sql .="pre_external_id,";
    }
    if(EXT_PARTNER_ID>0){
        $sql.=" gam_name, pre_bet_string, pre_game_num, res_string, pre_time, pre_id
        FROM ext_punter_result, punter
        WHERE pre_res_id = $hand_id
        AND pre_pun_id = pun_customer_number
        AND pun_customer_number = " .$dbh->escape($_GET["uid"]) . "
        AND pre_game_num = " . $dbh->prepareString($_GET["gmn"]);
    }
    else {
        $sql.=" gam_name,
          pre_bet_string,
          pre_game_num,
          res_string,
          pre_time,
          pre_id
          FROM result
          JOIN punter_result on pre_res_id = res_id
          JOIN game on gam_id = res_gam_id
          WHERE pre_res_id = $hand_id
          AND pre_pun_id = " .$dbh->escape($_GET["uid"]) . "
          AND pre_game_num = " . $dbh->prepareString($_GET["gmn"]);
    }
    
    
    
    $rs = $dbh->exec($sql);
    $num_rows = $rs->getNumRows();
    if($num_rows > 0) {
        $game_general_details = $rs->next();
        $transcript = "<div class='transcriptHeader'><h2>" . ucfirst(returnRealGamesName($game_general_details['gam_name'])) . "</h2></div>" . GameTranscript::getResultString($hand_id, $_GET["uid"], $_GET["gmn"]) . "<br style='clear:both'/>";
        ?>
        <div align="center">
        <table  cellspacing="1" border="0" width="100%">
        <tr>
            <td class="label" colspan="10" width="600px">Hand #<?= $hand_id ?><span style="float:right"><?= $game_general_details["pre_time"] ?></span></td>
        </tr>
                <?php
                if ($game_general_details["gam_name"] != 'zecchinetta'){
                ?>
                <tr>
                    <td class="label">Table:</td><td><?= $game_general_details["tbl_id"] . " - " . $game_general_details["gam_name"] ?></td>
                </tr>
                <?php
                }
                ?>
                <?if(DOT_IT==true){?>
                    <tr>
                        <td class="label">AAMS ID:</td><td><?=$game_general_details["pre_external_id"];?></td>
                    </tr>
                <?}?>
            </table>

            <?php
            $sql = "SELECT
                pre_bet,
                pre_win,
                pre_rake,
                pre_balance,
                pre_pun_ip,
                pun_id,
                pun_username,
                pun_betting_club,
                pre_bet_string,
                pre_bets_paid,
                pre_id
                 ";
            if(EXT_PARTNER_ID>0){
                $sql .="
            FROM ext_punter_result, punter
            WHERE pre_res_id = $hand_id
            AND pre_pun_id = pun_customer_number
            AND pun_customer_number = " .$dbh->escape($_GET["uid"]) . "
            AND pre_game_num = " . $dbh->escape($_GET["gmn"]);
            }
            else{
                $sql .=" FROM punter_result
    		     JOIN punter on pun_id = pre_pun_id
    		     WHERE pre_res_id=$hand_id
    		          AND pre_pun_id = " .$dbh->escape($_GET["uid"]) . "
    		          AND pre_game_num = " . $dbh->escape($_GET["gmn"]);
            }
            
            $rs  = $dbh->exec($sql);
            $rs2 = array();
            if($num_rows > 0){


                //blocca club indiscreti
                if($_SESSION["jurisdiction_class"] != "casino"){
                    $my_player_found = false;
                    while($rs->hasNext()){
                        $row = $rs->next();
                        $jur_class=$_SESSION['jurisdiction_class'];
                        $jur_id=$_SESSION['jurisdiction_id'];
                        $clubTC= $row["pun_betting_club"];

                        if($jur_class=='club'){
                            if($row["pun_betting_club"] == $_SESSION["jurisdiction_id"]){
                                $my_player_found = true;
                                break;
                            }
                        }
                        else{
                            $sql="Select c.jur_id FROM jurisdiction c
                          JOIN jurisdiction d on d.jur_id = c.jur_parent_id
                        ";
                            if($jur_class == "district"){
                                $sql .= " WHERE d.jur_id = " . $jur_id . "
                                    AND c.jur_id=".$clubTC;
                            }
                            elseif($jur_class=="region"){
                                $sql .=" JOIN jurisdiction r on r.jur_id = d.jur_parent_id
                                  WHERE r.jur_id=".$jur_id."
                                  AND c.jur_id=".$clubTC;
                            }
                            elseif ($jur_class=="nation"){
                                $sql .=" JOIN jurisdiction r on r.jur_id = d.jur_parent_id
                                 JOIN jurisdiction n on n.jur_id = r.jur_parent_id
                                 WHERE n.jur_id=".$jur_id."
                                 AND c.jur_id=".$clubTC;
                            }
                            else{
                                $sql.="AND 1=0";
                            }
                        }
                    }

                    $result=$dbh->queryOne($sql);
                    if($result){
                        $my_player_found=true;
                    }

                    if(!$my_player_found){
                        dieWithError($lang->getLang("No details"));
                    }
                    $rs->resetRecPtr();
                }

                while ($rs->hasNext()){
                    $row = $rs->next();
                   // $row['pre_bet_string'] = explode(";" ,$row['pre_bet_string']);
                    array_push($rs2, $row);
                }

                //todo(Alexa): here
                if($game_general_details["gam_name"] == 'zecchinetta' ) {
                    print_r(GameTranscript::getBet($hand_id,$_GET["uid"], $_GET["gmn"],false,'zecchinetta'));
                    $cols_arr = array("Username", "Bet", "Win", "Rake");
                } else {
                    $cols_arr = array("Username", "Bet", "Win", "Rake", "Bet For Line");
                }
                if($_SESSION["jurisdiction_class"] == "casino"){
                    array_push($cols_arr, "IP");
                }

                if($game_general_details["gam_name"] == 'zecchinetta' ){
                    $val_fmt_arr = array (
                        'Username'      =>'return getClassLink(refPage("customers/customer_view&customer_id={$rec["pun_id"]}&curr={$symbol}"), $rec["pun_username"], (($rec["pre_win"] > 0)?("winner"):(null)), "_blank");',
                        'Bet'           =>'return getInDollars($rec["pre_bet"]);',
                        'Win'           =>'return getInDollars($rec["pre_win"]);',
                        'Rake'           =>'return getInDollars($rec["pre_rake"]);',
                        'IP'            =>'return checkIp($rec["pre_pun_ip"]);'
                    );
                } else {
                    $val_fmt_arr = array(
                        'Username' => 'return getClassLink(refPage("customers/customer_view&customer_id={$rec["pun_id"]}&curr={$symbol}"), $rec["pun_username"], (($rec["pre_win"] > 0)?("winner"):(null)), "_blank");',
                        'Bet' => 'return getInDollars($rec["pre_bet"]);',
                        'Bet For Line' => 'return GameTranscript::getBet($rec["pre_id"],' . $_GET["uid"] . ',' . $_GET["gmn"] . ');',
                        'Win' => 'return getInDollars($rec["pre_win"]);',
                        'Rake' => 'return getInDollars($rec["pre_rake"]);',
                        'IP' => 'return checkIp($rec["pre_pun_ip"]);'
                    );
                }

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