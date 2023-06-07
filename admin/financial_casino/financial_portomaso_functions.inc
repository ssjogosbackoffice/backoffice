<?php



/**
 * @param $date_start
 * @param $date_end
 * @param $jur_totals
 * @param $betwin
 * @param $financial
 * @param $jur_level
 * @param $jur_sel_id
 * @return bool|int|RecordSet
 */
function getFinancialData($date_start, $date_end, $jur_totals, $betwin, $financial,$jur_level, $jur_sel_id){
    global $dbh;

    $sql = " SELECT gam_name, sum(cus_res_bet) tot_bet, sum(cus_res_win) tot_win, sum(cus_res_bet) - sum(cus_res_win) tot_net, count(cus_res_pun_id) users, jur_currency currency, cur_name
            FROM customer_result_daily, punter, currency, jurisdiction, game
            WHERE cus_res_day BETWEEN '$date_start' AND '$date_end'
            AND pun_id = cus_res_pun_id
            AND jur_id = pun_betting_club
            AND jur_currency = cur_id
            AND cus_res_gam_id in (250,6001,6002,6003,6004,6005)
            AND gam_id = cus_res_gam_id
            GROUP BY cus_res_gam_id, jur_currency";
    
    //var_dump($sql);
    $result = $dbh->doCachedQuery($sql, 0,true);
    return $result;
}




function getFinancialByCustomer($clubid,$date_start, $date_end, $betwin, $portomaso=0) {
    global $dbh,$lang;
    $sql = " SELECT gam_name, sum(cus_res_bet) tot_bet, sum(cus_res_win) tot_win, sum(cus_res_bet) - sum(cus_res_win) tot_net, count(cus_res_pun_id) users, jur_currency currency, cur_name
            FROM customer_result_daily, punter, currency, jurisdiction, game
            WHERE cus_res_day BETWEEN '$date_start' AND '$date_end'
            AND pun_id = cus_res_pun_id
            AND jur_id = pun_betting_club
            AND jur_currency = cur_id
            AND cus_res_gam_id in (250,6001,6002,6003,6004,6005)
            AND gam_id = cus_res_gam_id
            GROUP BY cus_res_gam_id, jur_currency";

    $result = $dbh->doCachedQuery($sql, 0);
    if(!$result->hasNext()){
        ?>
        <span class="tip"><?=$lang->getLang("No result found")?></span>
        <?
        return;
    }
    else {
        ?>
        <table class="display" style="width:100%;" id="userDetails">
            <thead>
            <tr class="header">
                <th class="label centered" style="width:300px;"><?=$lang->getLang("Customer")?></th>
 
                <?php
                if($portomaso==1){ 
                    if($betwin)
                    {?>
                        <th class="label centered">Portomaso<br><?=$lang->getLang("Bet")?></th>
                        <th class="label centered">Portomaso<br><?=$lang->getLang("Win")?></th>
                    <?php }
                    ?>
                    <th class="label centered">Portomaso<br><?=$lang->getLang("Net")?></th>
                    <?if($financial)
                    { ?>
                        <th class="label centered">Portomaso<br><?=$lang->getLang("Fin")?></th>
                    <?php } } ?>
                <?if($financial)
                { ?>
                    <th class="label centered"><?=$lang->getLang("Financial")?></th>
                <? }?>

            </tr>
            </thead>
            <tbody>
            <?
            $i=0;
            while($result->hasNext()){
                $row = $result->next();

                if($row["currency"]!=$_SESSION["currency_id"]){
                    $diffCurr=true;
                    foreach($row as $n=>$nv){
                        if(((strpos($n,'bet') !== false) || (strpos($n,'win') !== false) || (strpos($n,'rake') !== false) || (strpos($n,'fin') !== false))&&(strpos($n,'betting') === false)){
                            if($nv!=0){
                                if($_SESSION["currency_id"]==1){
                                    $row[$n]=$nv/$_SESSION["currencies"][$row["currency"]]["value"];
                                }
                                else{
                                    $row[$n]=$nv*$_SESSION["currencies"][$_SESSION["currency_id"]]["value"];
                                }
                            }
                        }
                    }
                }

                ?>
                <tr class='<?=(($i++%2==0)?("odd"):("even"))?>'>
                    <td class="content light" style="text-align:left;" ><a target="_blank" href="/?page=customers/customer_view&customer_id=<?=$row["cus_res_pun_id"]?>"><?=$row["pun_username"]?></a></td>

                    <?php
                    if($portomaso==1){
                        if($betwin)
                        {?>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["portomaso_bet"])?><br />
                                <span class="tip"> <?=$row['jps_perc_infinity']."%=".getInDollars($row["portomaso_bet"]/100*$row['jps_perc_infinity'])?></span></td>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["portomaso_win"])?><br />
                                <span class="tip"><?=$row['jps_perc_infinity']."%=".getInDollars($row["portomaso_win"]/100*$row['jps_perc_infinity'])?></span></td>
                        <?php }?>
                        <td class="content light" style="text-align:right"><?=getInDollars($row["portomaso_rake"])?><br />
                            <span class="tip"><?=$row['jps_perc_infinity']."%=".getInDollars($row["portomaso_rake"]/100*$row['jps_perc_infinity'])?></span></td>
                        <?if($financial)
                        {?>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["portomaso_fin"])?><br />
                                <span class="tip"><?=$row['jps_perc_infinity']."%=".getInDollars($row["portomaso_fin"]/100*$row['jps_perc_infinity'])?></span></td>
                        <?php } } ?>

                    
                    <?if($financial)
                    { ?>
                        <td class="content dark" style="text-align:right"><?=getInDollars($row["casino_fin"]+$row["casino_live_fin"]+$row["virtual_fin"]+$row["poker_live_fin"]+$row["poker_2d_fin"]+$row["betting_fin"] +$row["betting_fin_live"] +$row["casino_bankroulette_fin"] /*+$row['royalpk_fin']+$row['casino_forex'] */+$row['playtech_fin']+$row['evolution_fin'] +$row['infinity_fin'] )?></td>
                    <? }?>
                </tr>

                <?
            }

            ?>
            </tbody></table>
        <?
    }
}



/**
 * @param $rec
 * @return string
 */
function calculatePerc($rec){
    global $casino,$casino_live,$virtual,$pokerl,$poker2d,$royalpk,$playtech,$evolution,$infinity,$portomaso,$totem,$forex,$bankroulette,$zecchinetta,$greek;
    $ret='';
    if($casino==1){
        $ret +=$rec["casino_rake"];
    }
    if($casino_live==1){
        $ret +=$rec["casino_live_rake"];
    }
    if($virtual==1){
        $ret +=$rec["virtual_rake"];
    }
    if($royalpk==1){
        $ret +=$rec["royalpk_rake"];
    }
    if($playtech==1){
        $ret +=$rec["playtech_rake"];
    }
    if($evolution==1){
        $ret +=$rec["evolution_rake"];
    }
    if($infinity==1){
        $ret +=$rec["infinity_rake"];
    }
    if($portomaso==1){
        $ret +=$rec["portomaso_rake"];
    }
    if($forex==1){
        $ret +=$rec["forex_rake"];
    }
    if($totem==1){
        $ret +=$rec["totem_rake"];
    }
    if($pokerl==1){
        $ret +=$rec["poker_live_rake"];
    }
    if($poker2d==1){
        $ret +=$rec["poker_2d_rake"];
    }
    if($bankroulette==1){
        $ret +=$rec["casino_bankroulette_rake"];
    }
    if($zecchinetta==1){
        $ret +=$rec["casino_zecchinetta_rake"];
    }
    if($greek==1){
        $ret +=$rec["casino_greek_rake"];
    }
    return $ret;
}


function calculateFin($rec){
    global $casino,$casino_live,$virtual,$pokerl,$poker2d,$royalpk,$playtech,$evolution,$infinity,$portomaso,$totem,$forex,$bankroulette,$zecchinetta,$greek;
    $ret='';
    if($casino==1){
        $ret +=$rec["casino_fin"];
    }
    if($casino_live==1){
        $ret +=$rec["casino_live_fin"];
    }
    if($virtual==1){
        $ret +=$rec["virtual_fin"];
    }
    if($royalpk==1){
        $ret +=$rec["royalpk_fin"];
    }

    if($playtech==1){
        $ret +=$rec["playtech_fin"];
    }
    if($evolution==1){
        $ret +=$rec["evolution_fin"];
    }

    if($infinity==1){
        $ret +=$rec["infinity_fin"];
    }
    if($portomaso==1){
        $ret +=$rec["portomaso_fin"];
    }
    if($forex==1){
        $ret +=$rec["forex_fin"];
    }
    if($totem==1){
        $ret +=$rec["totem_rake"];
    }
    if($pokerl==1){
        $ret +=$rec["poker_live_fin"];
    }
    if($poker2d==1){
        $ret +=$rec["poker_2d_fin"];
    }
    if($bankroulette==1){
        $ret +=$rec["casino_bankroulette_fin"];
    }
    if($zecchinetta==1){
        $ret +=$rec["casino_zecchinetta_fin"];
    }
    if($greek==1){
        $ret +=$rec["casino_greek_fin"];
    }
    return $ret;
}



/**
 * @param $rec
 * @param int $type
 * @return float|string
 */
function calculateCurrentSubPerc($rec){
    global $casino,$casino_live,$virtual,$pokerl,$poker2d,$royalpk,$playtech,$evolution,$infinity,$portomaso,$totem,$forex,$bankroulette,$zecchinetta,$greek;
    $ret='';
    if($casino==1){
        $ret +=$rec["casino_rake"]/100* ($rec["jps_perc_casino"]);
    }
    if($casino_live==1){
        $ret +=$rec["casino_live_rake"]/100* ($rec["jps_perc_casino_live"]);
    }
    if($virtual==1){
        $ret +=$rec["virtual_rake"]/100* ($rec["jps_perc_virtual"]);
    }
    if($royalpk==1){
        $ret +=$rec["royalpk_rake"]/100* ($rec["jps_perc_royalpk"]);
    }
    if($playtech==1){
        $ret +=$rec["playtech_rake"]/100* ($rec["jps_perc_playtech"]);
    }
    if($evolution==1){
        $ret +=$rec["evolution_rake"]/100* ($rec["jps_perc_evolution"]);
    }
    if($infinity==1){
        $ret +=$rec["infinity_rake"]/100* ($rec["jps_perc_infinity"]);
    }
    if($portomaso==1){
        $ret +=$rec["portomaso_rake"]/100* ($rec["jps_perc_infinity"]);
    }
    if($forex==1){
        $ret +=$rec["forex_rake"]/100* ($rec["jps_perc_forex"]);
    }
    if($totem==1){
        $ret +=$rec["totem_rake"]/100* ($rec["jps_perc_totem"]);
    }
    if($pokerl==1){
        $ret +=$rec["poker_live_rake"]/100* ($rec["jps_perc_poker_live"]);
    }
    if($poker2d==1){
        $ret +=$rec["poker_2d_rake"]/100* ($rec["jps_perc_poker_2d"]);
    }
    if($bankroulette==1){
        $ret +=$rec["casino_bankroulette_rake"]/100* ($rec["jps_perc_bankroulette"]);
    }
    if($zecchinetta==1){
        $ret +=$rec["casino_zecchinetta_rake"]/100* ($rec["jps_perc_zecchinetta"]);
    }
    if($greek==1){
        $ret +=$rec["casino_greek_rake"]/100* ($rec["jps_perc_greek"]);
    }
    return $ret;
}

function calculateRowNet($rec,$class){
    global $casino,$casino_live,$virtual,$pokerl,$poker2d,$royalpk,$playtech,$evolution,$infinity,$portomaso,$totem,$forex,$bankroulette,$zecchinetta,$greek;
    $ret='';
    $totFin=calculateFin($rec);
    if($casino==1){

        if($class=='casino'){
            $ret +=($rec["casino_rake"]*(100-$rec['jps_perc_casino_n'])/100);
        }
        if($class=='nation'){
            $ret +=($rec["casino_rake"]*($rec['jps_perc_casino_n']-$rec['jps_perc_casino_r'])/100);
        }
        if($class=='region'){
            $ret +=($rec["casino_rake"]*($rec['jps_perc_casino_r']-$rec['jps_perc_casino_d'])/100);
        }
        if($class=='district'){
            $ret +=($rec["casino_rake"]*($rec['jps_perc_casino_d']-$rec['jps_perc_casino_c'])/100);
        }
    }
    if($casino_live==1){
        if($class=='casino'){
            $ret +=$rec["casino_live_rake"]/100*(100-$rec['jps_perc_casino_live_n']);
        }
        if($class=='nation'){
            $ret +=$rec["casino_live_rake"]/100*($rec['jps_perc_casino_live_n']-$rec['jps_perc_casino_live_r']);
        }
        if($class=='region'){
            $ret +=$rec["casino_live_rake"]/100*($rec['jps_perc_casino_live_r']-$rec['jps_perc_casino_live_d']);
        }
        if($class=='district'){
            $ret +=$rec["casino_live_rake"]/100*($rec['jps_perc_casino_live_d']-$rec['jps_perc_casino_live_c']);
        }
    }
    if($virtual==1){
        if($class=='casino'){
            $ret +=$rec["virtual_rake"]/100*(100-$rec['jps_perc_virtual_n']);
        }
        if($class=='nation'){
            $ret +=$rec["virtual_rake"]/100*($rec['jps_perc_virtual_n']-$rec['jps_perc_virtual_r']);
        }
        if($class=='region'){
            $ret +=$rec["virtual_rake"]/100*($rec['jps_perc_virtual_r']-$rec['jps_perc_virtual_d']);
        }
        if($class=='district'){
            $ret +=$rec["virtual_rake"]/100*($rec['jps_perc_virtual_d']-$rec['jps_perc_virtual_c']);
        }
    }


    if($royalpk==1){
        if($class=='casino'){
            $ret +=$rec["royalpk_rake"]/100*(100-$rec['jps_perc_royalpk_n']);
        }
        if($class=='nation'){
            $ret +=$rec["royalpk_rake"]/100*($rec['jps_perc_royalpk_n']-$rec['jps_perc_royalpk_r']);
        }
        if($class=='region'){
            $ret +=$rec["royalpk_rake"]/100*($rec['jps_perc_royalpk_r']-$rec['jps_perc_royalpk_d']);
        }
        if($class=='district'){
            $ret +=$rec["royalpk_rake"]/100*($rec['jps_perc_royalpk_d']-$rec['jps_perc_royalpk_c']);
        }

    }

    if($playtech==1){
        if($class=='casino'){
            $ret +=$rec["playtech_rake"]/100*(100-$rec['jps_perc_playtech_n']);
        }
        if($class=='nation'){
            $ret +=$rec["playtech_rake"]/100*($rec['jps_perc_playtech_n']-$rec['jps_perc_playtech_r']);
        }
        if($class=='region'){
            $ret +=$rec["playtech_rake"]/100*($rec['jps_perc_playtech_r']-$rec['jps_perc_playtech_d']);
        }
        if($class=='district'){
            $ret +=$rec["playtech_rake"]/100*($rec['jps_perc_playtech_d']-$rec['jps_perc_playtech_c']);
        }

    }

    if($evolution==1){
        if($class=='casino'){
            $ret +=$rec["evolution_rake"]/100*(100-$rec['jps_perc_evolution_n']);
        }
        if($class=='nation'){
            $ret +=$rec["evolution_rake"]/100*($rec['jps_perc_evolution_n']-$rec['jps_perc_evolution_r']);
        }
        if($class=='region'){
            $ret +=$rec["evolution_rake"]/100*($rec['jps_perc_evolution_r']-$rec['jps_perc_evolution_d']);
        }
        if($class=='district'){
            $ret +=$rec["evolution_rake"]/100*($rec['jps_perc_evolution_d']-$rec['jps_perc_evolution_c']);
        }

    }

    if($infinity==1){
        if($class=='casino'){
            $ret +=$rec["infinity_rake"]/100*(100-$rec['jps_perc_infinity_n']);
        }
        if($class=='nation'){
            $ret +=$rec["infinity_rake"]/100*($rec['jps_perc_infinity_n']-$rec['jps_perc_infinity_r']);
        }
        if($class=='region'){
            $ret +=$rec["infinity_rake"]/100*($rec['jps_perc_infinity_r']-$rec['jps_perc_infinity_d']);
        }
        if($class=='district'){
            $ret +=$rec["infinity_rake"]/100*($rec['jps_perc_infinity_d']-$rec['jps_perc_infinity_c']);
        }

    }
    
    if($portomaso==1){
        if($class=='casino'){
            $ret +=$rec["portomaso_rake"]/100*(100-$rec['jps_perc_infinity_n']);
        }
        if($class=='nation'){
            $ret +=$rec["portomaso_rake"]/100*($rec['jps_perc_infinity_n']-$rec['jps_perc_infinity_r']);
        }
        if($class=='region'){
            $ret +=$rec["portomaso_rake"]/100*($rec['jps_perc_infinity_r']-$rec['jps_perc_infinity_d']);
        }
        if($class=='district'){
            $ret +=$rec["portomaso_rake"]/100*($rec['jps_perc_infinity_d']-$rec['jps_perc_infinity_c']);
        }
        
    }

    if($forex==1){
        if($class=='casino'){
            $ret +=$rec["forex_rake"]/100*(100-$rec['jps_perc_forex_n']);
        }
        if($class=='nation'){
            $ret +=$rec["forex_rake"]/100*($rec['jps_perc_forex_n']-$rec['jps_perc_forex_r']);
        }
        if($class=='region'){
            $ret +=$rec["forex_rake"]/100*($rec['jps_perc_forex_r']-$rec['jps_perc_forex_d']);
        }
        if($class=='district'){
            $ret +=$rec["forex_rake"]/100*($rec['jps_perc_forex_d']-$rec['jps_perc_forex_c']);
        }

    }

    if($totem==1){
        if($class=='casino'){
            $ret +=$rec["totem_rake"]/100*(100-$rec['jps_perc_totem_n']);
        }
        if($class=='nation'){
            $ret +=$rec["totem_rake"]/100*($rec['jps_perc_totem_n']-$rec['jps_perc_totem_r']);
        }
        if($class=='region'){
            $ret +=$rec["totem_rake"]/100*($rec['jps_perc_totem_r']-$rec['jps_perc_totem_d']);
        }
        if($class=='district'){
            $ret +=$rec["totem_rake"]/100*($rec['jps_perc_totem_d']-$rec['jps_perc_totem_c']);
        }

    }

    if($pokerl==1){

        if($class=='casino'){
            $ret +=$rec["poker_live_rake"]/100*(100-$rec['jps_perc_poker_live_n']);
        }
        if($class=='nation'){

            $ret +=$rec["poker_live_rake"]/100*($rec['jps_perc_poker_live_n']-$rec['jps_perc_poker_live_r']);
        }
        if($class=='region'){
            $ret +=$rec["poker_live_rake"]/100*($rec['jps_perc_poker_live_r']-$rec['jps_perc_poker_live_d']);
        }
        if($class=='district'){
            $ret +=$rec["poker_live_rake"]/100*($rec['jps_perc_poker_live_d']-$rec['jps_perc_poker_live_c']);
        }
    }
    if($poker2d==1){

        if($class=='casino'){
            $ret +=$rec["poker_2d_rake"]/100*(100-$rec['jps_perc_poker_n']);
        }
        if($class=='nation'){
            $ret +=$rec["poker_2d_rake"]/100*($rec['jps_perc_poker_n']-$rec['jps_perc_poker_r']);
        }
        if($class=='region'){
            $ret +=$rec["poker_2d_rake"]/100*($rec['jps_perc_poker_r']-$rec['jps_perc_poker_d']);
        }
        if($class=='district'){
            $ret +=$rec["poker_2d_rake"]/100*($rec['jps_perc_poker_d']-$rec['jps_perc_poker_c']);
        }
    }
    if($bankroulette==1){

        if($class=='casino'){
            $ret +=$rec["casino_bankroulette_rake"]/100*(100-$rec['jps_perc_bankroulette_n']);
        }
        if($class=='nation'){
            $ret +=$rec["casino_bankroulette_rake"]/100*($rec['jps_perc_bankroulette_n']-$rec['jps_perc_bankroulette_r']);
        }
        if($class=='region'){
            $ret +=$rec["casino_bankroulette_rake"]/100*($rec['jps_perc_bankroulette_r']-$rec['jps_perc_bankroulette_d']);
        }
        if($class=='district'){
            $ret +=$rec["casino_bankroulette_rake"]/100*($rec['jps_perc_bankroulette_d']-$rec['jps_perc_bankroulette_c']);
        }
    }
    if($zecchinetta==1){

        if($class=='casino'){
            $ret +=$rec["casino_zecchinetta_rake"]/100*(100-$rec['jps_perc_zecchinetta_n']);
        }
        if($class=='nation'){
            $ret +=$rec["casino_zecchinetta_rake"]/100*($rec['jps_perc_zecchinetta_n']-$rec['jps_perc_zecchinetta_r']);
        }
        if($class=='region'){
            $ret +=$rec["casino_zecchinetta_rake"]/100*($rec['jps_perc_zecchinetta_r']-$rec['jps_perc_zecchinetta_d']);
        }
        if($class=='district'){
            $ret +=$rec["casino_zecchinetta_rake"]/100*($rec['jps_perc_zecchinetta_d']-$rec['jps_perc_zecchinetta_c']);
        }
    }
    if($greek==1){

        if($class=='casino'){
            $ret +=$rec["casino_greek_rake"]/100*(100-$rec['jps_perc_greek_n']);
        }
        if($class=='nation'){
            $ret +=$rec["casino_greek_rake"]/100*($rec['jps_perc_greek_n']-$rec['jps_perc_greek_r']);
        }
        if($class=='region'){
            $ret +=$rec["casino_greek_rake"]/100*($rec['jps_perc_greek_r']-$rec['jps_perc_greek_d']);
        }
        if($class=='district'){
            $ret +=$rec["casino_greek_rake"]/100*($rec['jps_perc_greek_d']-$rec['jps_perc_greek_c']);
        }
    }
    return $ret;
}


?>