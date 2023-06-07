<?php
function getBettingProfiles(){
    global $dbh;
    $sql="Select * from ProfiliCommissioniTipi";
    $result=$dbh->exec($sql,false,true);
    return $result;
}
/*$bettingProfilesTypes=true; */
$bettingProfilesTypes=array();
$bettingProfiles=getBettingProfiles();

while($bettingProfiles->hasNext()){
    $row=$bettingProfiles->next();
    $bettingProfilesTypes[$row['idprofilocommissionetipo']]=array('name'=>$row['nome'],
        'formulas'=>array('casino'  => $row['formulacasino'],
            'nation'  => $row['formulanation'],
            'region'  => $row['formularegion'],
            'district'=> $row['formuladistrict'])

    );
}
$query_jur_selectors = array(
    "nation" => "n.jur_name n_jur_name,n.jur_id n_jur_id",
    "region" => "n.jur_name n_jur_name,n.jur_id n_jur_id, r.jur_name r_jur_name,r.jur_id r_jur_id",
    "district" => "n.jur_name n_jur_name,n.jur_id n_jur_id, r.jur_name r_jur_name,r.jur_id r_jur_id, d.jur_name d_jur_name,d.jur_id d_jur_id",
    "club" => "n.jur_name n_jur_name,n.jur_id n_jur_id, r.jur_name r_jur_name,r.jur_id r_jur_id, d.jur_name d_jur_name,d.jur_id d_jur_id, c.jur_name c_jur_name,c.jur_id c_jur_id"
);
$query_jur_orders = array(
    "nation" => "n.jur_name",
    "region" => "n.jur_name, r.jur_name",
    "district" => "n.jur_name, r.jur_name, d.jur_name",
    "club" => "n.jur_name, r.jur_name, d.jur_name, c.jur_name"
);
$query_jur_groups = array(
    "nation" => "n.jur_id",
    "region" => "n.jur_id, r.jur_id",
    "district" => "n.jur_id, r.jur_id, d.jur_id",
    "club" => "n.jur_id, r.jur_id, d.jur_id, c.jur_id"
);
$query_jur_payments = array(
    "nation" => "n.jur_id",
    "region" => "r.jur_id",
    "district" => "d.jur_id",
    "club" => "c.jur_id"
);
$query_jur_currency = array(
    "nation" => "n.jur_currency",
    "region" => "r.jur_currency",
    "district" => "d.jur_currency",
    "club" => "c.jur_currency"
);


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
    global $dbh, $query_jur_selectors,$query_jur_groups, $query_jur_orders,$query_jur_payments,$betting,$query_jur_currency;

    $sql = "select
                coalesce(cur_level.jps_perc_casino, 0) as jps_perc_casino,
                coalesce(bet_n.jps_perc_casino, 0) as jps_perc_casino_n,
                coalesce(bet_r.jps_perc_casino, 0) as jps_perc_casino_r,
                coalesce(bet_d.jps_perc_casino, 0) as jps_perc_casino_d,
                coalesce(bet_c.jps_perc_casino, 0) as jps_perc_casino_c,
                
                coalesce(cur_level.jps_perc_bankroulette, 0) as jps_perc_bankroulette,
                coalesce(bet_n.jps_perc_bankroulette, 0) as jps_perc_bankroulette_n,
                coalesce(bet_r.jps_perc_bankroulette, 0) as jps_perc_bankroulette_r,
                coalesce(bet_d.jps_perc_bankroulette, 0) as jps_perc_bankroulette_d,
                coalesce(bet_c.jps_perc_bankroulette, 0) as jps_perc_bankroulette_c,
                
                coalesce(cur_level.jps_perc_zecchinetta, 0) as jps_perc_zecchinetta,
                coalesce(bet_n.jps_perc_zecchinetta, 0) as jps_perc_zecchinetta_n,
                coalesce(bet_r.jps_perc_zecchinetta, 0) as jps_perc_zecchinetta_r,
                coalesce(bet_d.jps_perc_zecchinetta, 0) as jps_perc_zecchinetta_d,
                coalesce(bet_c.jps_perc_zecchinetta, 0) as jps_perc_zecchinetta_c,
                
                coalesce(cur_level.jps_perc_greek, 0) as jps_perc_greek,
                coalesce(bet_n.jps_perc_greek, 0) as jps_perc_greek_n,
                coalesce(bet_r.jps_perc_greek, 0) as jps_perc_greek_r,
                coalesce(bet_d.jps_perc_greek, 0) as jps_perc_greek_d,
                coalesce(bet_c.jps_perc_greek, 0) as jps_perc_greek_c,
                
                coalesce(cur_level.jps_perc_casino_live, 0) as jps_perc_casino_live,
                coalesce(bet_n.jps_perc_casino_live, 0) as jps_perc_casino_live_n,
                coalesce(bet_r.jps_perc_casino_live, 0) as jps_perc_casino_live_r,
                coalesce(bet_d.jps_perc_casino_live, 0) as jps_perc_casino_live_d,
                coalesce(bet_c.jps_perc_casino_live, 0) as jps_perc_casino_live_c,

                coalesce(cur_level.jps_perc_virtual, 0) as jps_perc_virtual,
                coalesce(bet_n.jps_perc_virtual, 0) as jps_perc_virtual_n,
                coalesce(bet_r.jps_perc_virtual, 0) as jps_perc_virtual_r,
                coalesce(bet_d.jps_perc_virtual, 0) as jps_perc_virtual_d,
                coalesce(bet_c.jps_perc_virtual, 0) as jps_perc_virtual_c,

                coalesce(cur_level.jps_perc_poker_live, 0) as jps_perc_poker_live,
                coalesce(bet_n.jps_perc_poker_live, 0) as jps_perc_poker_live_n,
                coalesce(bet_r.jps_perc_poker_live, 0) as jps_perc_poker_live_r,
                coalesce(bet_d.jps_perc_poker_live, 0) as jps_perc_poker_live_d,
                coalesce(bet_c.jps_perc_poker_live, 0) as jps_perc_poker_live_c,

                coalesce(cur_level.jps_perc_poker, 0) as jps_perc_poker_2d,
                coalesce(bet_n.jps_perc_poker, 0) as jps_perc_poker_2d_n,
                coalesce(bet_r.jps_perc_poker, 0) as jps_perc_poker_2d_r,
                coalesce(bet_d.jps_perc_poker, 0) as jps_perc_poker_2d_d,
                coalesce(bet_c.jps_perc_poker, 0) as jps_perc_poker_2d_c, " ;


    $sql.="    coalesce(cur_level.jps_perc_playtech, 0) as jps_perc_playtech,
                coalesce(bet_n.jps_perc_playtech, 0) as jps_perc_playtech_n,
                coalesce(bet_r.jps_perc_playtech, 0) as jps_perc_playtech_r,
                coalesce(bet_d.jps_perc_playtech, 0) as jps_perc_playtech_d,
                coalesce(bet_c.jps_perc_playtech, 0) as jps_perc_playtech_c,
                
                coalesce(cur_level.jps_perc_evolution, 0) as jps_perc_evolution,
                coalesce(bet_n.jps_perc_evolution, 0) as jps_perc_evolution_n,
                coalesce(bet_r.jps_perc_evolution, 0) as jps_perc_evolution_r,
                coalesce(bet_d.jps_perc_evolution, 0) as jps_perc_evolution_d,
                coalesce(bet_c.jps_perc_evolution, 0) as jps_perc_evolution_c,

                coalesce(cur_level.jps_perc_infinity, 0) as jps_perc_infinity,
                coalesce(bet_n.jps_perc_infinity, 0) as jps_perc_infinity_n,
                coalesce(bet_r.jps_perc_infinity, 0) as jps_perc_infinity_r,
                coalesce(bet_d.jps_perc_infinity, 0) as jps_perc_infinity_d,
                coalesce(bet_c.jps_perc_infinity, 0) as jps_perc_infinity_c,

                coalesce(cur_level.jps_perc_zeus, 0) as jps_perc_zeus,
                coalesce(bet_n.jps_perc_zeus, 0) as jps_perc_zeus_n,
                coalesce(bet_r.jps_perc_zeus, 0) as jps_perc_zeus_r,
                coalesce(bet_d.jps_perc_zeus, 0) as jps_perc_zeus_d,
                coalesce(bet_c.jps_perc_zeus, 0) as jps_perc_zeus_c,

                coalesce(cur_level.jps_perc_totem, 0) as jps_perc_totem,
                coalesce(bet_n.jps_perc_totem, 0) as jps_perc_totem_n,
                coalesce(bet_r.jps_perc_totem, 0) as jps_perc_totem_r,
                coalesce(bet_d.jps_perc_totem, 0) as jps_perc_totem_d,
                coalesce(bet_c.jps_perc_totem, 0) as jps_perc_totem_c,

                coalesce(cur_level.jps_perc_betting, 0) as jps_perc_betting,
                coalesce(cur_level.jps_perc_live_betting, 0) as jps_perc_betting_live,
                coalesce(bet_n.jps_perc_betting, 0) as jps_perc_betting_n,
                coalesce(bet_n.jps_perc_live_betting, 0) as jps_perc_betting_live_n,
                coalesce(bet_r.jps_perc_betting, 0) as jps_perc_betting_r,
                coalesce(bet_r.jps_perc_live_betting, 0) as jps_perc_betting_live_r,
                coalesce(bet_d.jps_perc_betting, 0) as jps_perc_betting_d,
                coalesce(bet_d.jps_perc_live_betting, 0) as jps_perc_betting_live_d,
                coalesce(bet_c.jps_perc_betting, 0) as jps_perc_betting_c,
                coalesce(bet_c.jps_perc_live_betting, 0) as jps_perc_betting_live_c,
                jds_day,jds_jur_club, ";

    $sql .= "sum(if (jds_gam_category = 1, jds_bet, 0)) as casino_bet,
                 sum(if (jds_gam_category = 1, jds_win, 0)) as casino_win,
                 sum(if (jds_gam_category = 1, jds_rake, 0)) as casino_fin,

                 sum(if (jds_gam_category = 2, jds_bet, 0)) as casino_live_bet,
                 sum(if (jds_gam_category = 2, jds_win, 0)) as casino_live_win,
                 sum(if (jds_gam_category = 2, jds_rake, 0)) as casino_live_fin,

                 sum(if (jds_gam_category = 3, jds_bet, 0)) as virtual_bet,
                 sum(if (jds_gam_category = 3, jds_win, 0)) as virtual_win,
                 sum(if (jds_gam_category = 3, jds_rake, 0)) as virtual_fin,

                 sum(if (jds_gam_category = 5, jds_bet, 0)) as poker_live_bet,
                 sum(if (jds_gam_category = 5, jds_win, 0)) as poker_live_win,
                 sum(if (jds_gam_category = 5, jds_fin, 0)) as poker_live_fin,

                 sum(if (jds_gam_category = 8, jds_bet, 0)) as poker_2d_bet,
                 sum(if (jds_gam_category = 8, jds_win, 0)) as poker_2d_win,
                 sum(if (jds_gam_category = 8, jds_bet, 0)) - sum(if (jds_gam_category = 8, jds_win, 0)) as poker_2d_net,
                 sum(if (jds_gam_category = 8, jds_fin, 0)) as poker_2d_fin, ";


    $sql .=" sum(if (jds_gam_category = 4, jds_bet, 0)) as playtech_bet,
                 sum(if (jds_gam_category = 4, jds_win, 0)) as playtech_win,
                 sum(if (jds_gam_category = 4, jds_rake, 0)) as playtech_fin,
                 
                 sum(if (jds_gam_category = 10, jds_bet, 0)) as evolution_bet,
                 sum(if (jds_gam_category = 10, jds_win, 0)) as evolution_win,
                 sum(if (jds_gam_category = 10, jds_rake, 0)) as evolution_fin,

                 sum(if (jds_gam_category in (12, 21), jds_bet, 0)) as infinity_bet,
                 sum(if (jds_gam_category in (12, 21), jds_win, 0)) as infinity_win,
                 sum(if (jds_gam_category in (12, 21), jds_rake, 0)) as infinity_fin,

                 sum(if (jds_gam_category = 22, jds_bet, 0)) as portomaso_bet,
                 sum(if (jds_gam_category = 22, jds_win, 0)) as portomaso_win,
                 sum(if (jds_gam_category = 22, jds_rake, 0)) as portomaso_fin,
                 
                 sum(if (jds_gam_category in (14, 18), jds_bet, 0)) as casino_bankroulette_bet,
                 sum(if (jds_gam_category in (14, 18), jds_win, 0)) as casino_bankroulette_win,
                 sum(if (jds_gam_category in (14, 18), jds_fin, 0)) as casino_bankroulette_fin,
                 
                 sum(if (jds_gam_category = 15, jds_bet, 0)) as casino_zecchinetta_bet,
                 sum(if (jds_gam_category = 15, jds_win, 0)) as casino_zecchinetta_win,
                 sum(if (jds_gam_category = 15, jds_fin, 0)) as casino_zecchinetta_fin,
                 
                 sum(if (jds_gam_category = 17, jds_bet, 0)) as casino_greek_bet,
                 sum(if (jds_gam_category = 17, jds_win, 0)) as casino_greek_win,
                 sum(if (jds_gam_category = 17, jds_fin, 0)) as casino_greek_fin,

                 sum(if (jds_gam_category = 23, jds_bet, 0)) as zeus_bet,
                 sum(if (jds_gam_category = 23, jds_win, 0)) as zeus_win,
                 sum(if (jds_gam_category = 23, jds_rake, 0)) as zeus_fin,
                 
                 sum(if (jds_gam_category = 11, jds_bet, 0)) as totem_bet,
                 sum(if (jds_gam_category = 11, jds_win, 0)) as totem_win,
                 sum(if (jds_gam_category = 11, jds_rake, 0)) as totem_fin, ";
    $sql .="     sum(if (jds_gam_category = 7, jds_fin, 0)) as betting_fin,
                 sum(if (jds_gam_category = 7, jds_bet, 0)) as betting_bet,
                 sum(if (jds_gam_category = 7, jds_win, 0)) as betting_win,
                 sum(if (jds_gam_category = 9, jds_bet, 0)) as betting_bet_live,
                 sum(if (jds_gam_category = 9, jds_fin, 0)) as betting_fin_live,
                 sum(if (jds_gam_category = 9, jds_win, 0)) as betting_win_live, ";


    $sql .= "
             sum(if (jds_gam_category = 1, jds_rake, 0)) as casino_rake,
             sum(if (jds_gam_category = 2, jds_rake, 0)) as casino_live_rake,
             sum(if (jds_gam_category = 3, jds_rake, 0)) as virtual_rake,
             sum(if (jds_gam_category = 5, jds_rake, 0)) as poker_live_rake,
             sum(if (jds_gam_category = 6, jds_fin, 0)) as fin,
             sum(if (jds_gam_category = 7, jds_rake, 0)) as betting_rake ,
             sum(if (jds_gam_category = 8, jds_rake, 0)) as poker_2d_rake,
             sum(if (jds_gam_category = 9, jds_rake, 0)) as betting_rake_live ,
             sum(if (jds_gam_category in (14, 18), jds_rake, 0)) as casino_bankroulette_rake, 
             sum(if (jds_gam_category = 15, jds_rake, 0)) as casino_zecchinetta_rake, 
             sum(if (jds_gam_category = 17, jds_rake, 0)) as casino_greek_rake, ";
    /*sum(if (jds_gam_category = 10, jds_rake, 0)) as royalpk_rake ,

    sum(if (jds_gam_category = 13, jds_rake, 0)) as forex_rake ,*/

    $sql .= "    sum(if (jds_gam_category = 4, jds_rake, 0)) as playtech_rake ,
    sum(if (jds_gam_category = 10, jds_rake, 0)) as evolution_rake ,
     sum(if (jds_gam_category in (12, 21), jds_rake, 0)) as infinity_rake ,
     sum(if (jds_gam_category = 22, jds_rake, 0)) as portomaso_rake ,
     sum(if (jds_gam_category = 23, jds_rake, 0)) as zeus_rake ,
             sum(if (jds_gam_category = 11, jds_rake, 0)) as totem_rake ,
             $query_jur_currency[$jur_totals] as currency,
                $query_jur_selectors[$jur_totals]
                from
                    jurisdiction_daily_summary
                        LEFT JOIN game_category on gct_id= jds_gam_category

                        JOIN jurisdiction c on c.jur_id = jds_jur_club
                        JOIN jurisdiction d on d.jur_id = jds_jur_district
                        JOIN jurisdiction r on r.jur_id = jds_jur_region
                        JOIN jurisdiction n on n.jur_id = jds_jur_nation
                        LEFT JOIN jurisdiction_payment_settings cur_level on cur_level.jps_jur_id = ".$query_jur_payments[$jur_totals]."
                        LEFT JOIN jurisdiction_payment_settings bet_n on bet_n.jps_jur_id = n.jur_id
                        LEFT JOIN jurisdiction_payment_settings bet_r on bet_r.jps_jur_id = r.jur_id
                        LEFT JOIN jurisdiction_payment_settings bet_d on bet_d.jps_jur_id = d.jur_id
                        LEFT JOIN jurisdiction_payment_settings bet_c on bet_c.jps_jur_id = c.jur_id
                        WHERE jds_day >= '$date_start' AND jds_day <= '$date_end'  ";

    if($jur_sel_id == ""){
        if($_SESSION['jurisdiction_class'] == 'nation'){
            $sql .= " and r.jur_parent_id = ". $_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class'] == 'region'){
            $sql .= " and d.jur_parent_id = ". $_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class'] == 'district'){
            $sql .= " and c.jur_parent_id = ". $_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class'] == 'club'){
            $sql .= " and c.jur_id = " . $_SESSION['jurisdiction_id'];
        }
    }else{
        if($jur_level == 'club'){
            $sql .= " and c.jur_id = $jur_sel_id";
        }
        if($jur_level == 'district'){
            $sql .= " and d.jur_id = $jur_sel_id";
        }
        if($jur_level == 'region'){
            $sql .= " and r.jur_id = $jur_sel_id";
        }
        if($jur_level == 'nation'){
            $sql .= " and n.jur_id = $jur_sel_id";
        }
    }
    $sql .=" GROUP BY $query_jur_groups[$jur_totals] ";
    if($betting==1){
        $sql.=" HAVING (betting_bet<>0 or betting_win<>0 or betting_rake<>0 or
                     betting_bet_live<>0 or betting_win_live<>0 or betting_rake_live<>0)  ";
    }
    else{
        $sql .=" HAVING ( casino_bet<>0 or casino_win<>0 or casino_rake<>0 or
                     casino_bankroulette_bet<>0 or casino_bankroulette_win<>0 or casino_bankroulette_rake<>0 or
                     casino_zecchinetta_bet<>0 or casino_zecchinetta_win<>0 or casino_zecchinetta_rake<>0 or
                     zeus_bet<>0 or zeus_win<>0 or zeus_rake<>0 or
                     casino_greek_bet<>0 or casino_greek_win<>0 or casino_greek_rake<>0 or
                     casino_live_bet<>0 or casino_live_win<>0 or casino_live_rake<>0 or
                     virtual_bet<>0 or virtual_win<>0 or virtual_rake<>0 or
                     poker_live_bet<>0 or poker_live_win<>0 or poker_live_rake<>0 or
                     poker_2d_bet<>0 or poker_2d_win<>0 or poker_2d_rake<>0 or ";

        /*royalpk_bet<>0 or royalpk_win<>0 or royalpk_rake<>0 or
         forex_bet<>0 or forex_win<>0 or forex_rake<>0 or
        */
        $sql .="     playtech_bet<>0 or playtech_win<>0 or playtech_rake<>0 or
                     evolution_bet<>0 or evolution_win<>0 or evolution_rake<>0 or
                     infinity_bet<>0 or infinity_win<>0 or infinity_rake<>0 or
                    totem_bet<>0 or totem_win<>0 or totem_rake<>0 or poker_2d_fin <> 0 or poker_live_fin <> 0 )";
    }

    $sql.="  ORDER BY $query_jur_orders[$jur_totals]";
    
    //var_dump($sql);
    $result = $dbh->doCachedQuery($sql, 0,true);
    return $result;
}
function getBettingFinancialData($date_start, $date_end, $jur_totals, $betwin, $financial,$jur_level, $jur_sel_id){
    global $dbh, $query_jur_selectors,$query_jur_groups, $query_jur_orders,$query_jur_payments,$betting,$query_jur_currency;

    $sql = "select   coalesce(cur_level.jps_perc_betting, 0) as jps_perc_betting,
                coalesce(cur_level.jps_perc_live_betting, 0) as jps_perc_betting_live,
                coalesce(bet_n.jps_perc_betting, 0) as jps_perc_betting_n,
                coalesce(bet_n.jps_perc_live_betting, 0) as jps_perc_betting_live_n,
                coalesce(bet_r.jps_perc_betting, 0) as jps_perc_betting_r,
                coalesce(bet_r.jps_perc_live_betting, 0) as jps_perc_betting_live_r,
                coalesce(bet_d.jps_perc_betting, 0) as jps_perc_betting_d,
                coalesce(bet_d.jps_perc_live_betting, 0) as jps_perc_betting_live_d,
                coalesce(bet_c.jps_perc_betting, 0) as jps_perc_betting_c,
                coalesce(bet_c.jps_perc_live_betting, 0) as jps_perc_betting_live_c,
                jds_day,jds_jur_club, ";

    $sql .= " sum(if (jds_gam_category = 7, jds_fin, 0)) as betting_fin,
                 sum(if (jds_gam_category = 7, jds_bet, 0)) as betting_bet,
                 sum(if (jds_gam_category = 7, jds_win, 0)) as betting_win,
                 sum(if (jds_gam_category = 9, jds_bet, 0)) as betting_bet_live,
                 sum(if (jds_gam_category = 9, jds_fin, 0)) as betting_fin_live,
                 sum(if (jds_gam_category = 9, jds_win, 0)) as betting_win_live, ";


    $sql .= "
             sum(if (jds_gam_category = 6, jds_fin, 0)) as fin,
             sum(if (jds_gam_category = 7, jds_rake, 0)) as betting_rake ,
             sum(if (jds_gam_category = 9, jds_rake, 0)) as betting_rake_live , ";


    $sql .= "    
             $query_jur_currency[$jur_totals] as currency,
                $query_jur_selectors[$jur_totals]
                from
                    jurisdiction_daily_summary
                        LEFT JOIN game_category on gct_id= jds_gam_category

                        JOIN jurisdiction c on c.jur_id = jds_jur_club
                        JOIN jurisdiction d on d.jur_id = jds_jur_district
                        JOIN jurisdiction r on r.jur_id = jds_jur_region
                        JOIN jurisdiction n on n.jur_id = jds_jur_nation
                        LEFT JOIN jurisdiction_payment_settings cur_level on cur_level.jps_jur_id = ".$query_jur_payments[$jur_totals]."
                        LEFT JOIN jurisdiction_payment_settings bet_n on bet_n.jps_jur_id = n.jur_id
                        LEFT JOIN jurisdiction_payment_settings bet_r on bet_r.jps_jur_id = r.jur_id
                        LEFT JOIN jurisdiction_payment_settings bet_d on bet_d.jps_jur_id = d.jur_id
                        LEFT JOIN jurisdiction_payment_settings bet_c on bet_c.jps_jur_id = c.jur_id
                        WHERE jds_day >= '$date_start' AND jds_day <= '$date_end'  ";

    if($jur_sel_id == ""){
        if($_SESSION['jurisdiction_class'] == 'nation'){
            $sql .= " and r.jur_parent_id = ". $_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class'] == 'region'){
            $sql .= " and d.jur_parent_id = ". $_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class'] == 'district'){
            $sql .= " and c.jur_parent_id = ". $_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class'] == 'club'){
            $sql .= " and c.jur_id = " . $_SESSION['jurisdiction_id'];
        }
    }else{
        if($jur_level == 'club'){
            $sql .= " and c.jur_id = $jur_sel_id";
        }
        if($jur_level == 'district'){
            $sql .= " and d.jur_id = $jur_sel_id";
        }
        if($jur_level == 'region'){
            $sql .= " and r.jur_id = $jur_sel_id";
        }
        if($jur_level == 'nation'){
            $sql .= " and n.jur_id = $jur_sel_id";
        }
    }
    $sql .=" GROUP BY $query_jur_groups[$jur_totals] ";
    $sql.=" HAVING (betting_bet<>0 or betting_win<>0 or betting_rake<>0 or
                     betting_bet_live<>0 or betting_win_live<>0 or betting_rake_live<>0)  ";




    $sql.="  ORDER BY $query_jur_orders[$jur_totals]";

    $result = $dbh->doCachedQuery($sql, 0,true);
    return $result;
}




function getVirtualFinancialData($date_start, $date_end, $jur_totals, $betwin, $financial,$jur_level, $jur_sel_id){
    global $dbh, $query_jur_selectors,$query_jur_groups, $query_jur_orders,$query_jur_payments;

    $sql = " select coalesce(cur_level.jps_perc_virtual, 0) as jps_perc_virtual,
                coalesce(virtual_n.jps_perc_virtual, 0) as jps_perc_virtual_n,
                coalesce(virtual_r.jps_perc_virtual, 0) as jps_perc_virtual_r,
                coalesce(virtual_d.jps_perc_virtual, 0) as jps_perc_virtual_d,
                coalesce(virtual_c.jps_perc_virtual, 0) as jps_perc_virtual_c,
                jds_day,jds_jur_club, ";
    $sql .= " sum(if (jds_gam_category = 3, jds_bet, 0)) as virtual_bet,
                 sum(if (jds_gam_category = 3, jds_win, 0)) as virtual_win,
                 sum(if (jds_gam_category = 3, jds_fin, 0)) as virtual_fin,";

    $sql .= " sum(if (jds_gam_category = 3, jds_rake, 0)) as virtual_rake,
                $query_jur_selectors[$jur_totals]
                from
                    jurisdiction_daily_summary
                        LEFT JOIN game_category on gct_id= jds_gam_category

                        JOIN jurisdiction c on c.jur_id = jds_jur_club
                        JOIN jurisdiction d on d.jur_id = jds_jur_district
                        JOIN jurisdiction r on r.jur_id = jds_jur_region
                        JOIN jurisdiction n on n.jur_id = jds_jur_nation
                        LEFT JOIN jurisdiction_payment_settings cur_level on cur_level.jps_jur_id = ".$query_jur_payments[$jur_totals]."
                        LEFT JOIN jurisdiction_payment_settings virtual_n on virtual_n.jps_jur_id = n.jur_id
                        LEFT JOIN jurisdiction_payment_settings virtual_r on virtual_r.jps_jur_id = r.jur_id
                        LEFT JOIN jurisdiction_payment_settings virtual_d on virtual_d.jps_jur_id = d.jur_id
                        LEFT JOIN jurisdiction_payment_settings virtual_c on virtual_c.jps_jur_id = c.jur_id

                        WHERE jds_day >= '$date_start' AND jds_day <= '$date_end'  ";

    if($jur_sel_id == ""){
        if($_SESSION['jurisdiction_class'] == 'nation'){
            $sql .= " and r.jur_parent_id = ". $_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class'] == 'region'){
            $sql .= " and d.jur_parent_id = ". $_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class'] == 'district'){
            $sql .= " and c.jur_parent_id = ". $_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class'] == 'club'){
            $sql .= " and c.jur_id = " . $_SESSION['jurisdiction_id'];
        }
    }else{
        if($jur_level == 'club'){
            $sql .= " and c.jur_id = $jur_sel_id";
        }
        if($jur_level == 'district'){
            $sql .= " and d.jur_id = $jur_sel_id";
        }
        if($jur_level == 'region'){
            $sql .= " and r.jur_id = $jur_sel_id";
        }
        if($jur_level == 'nation'){
            $sql .= " and n.jur_id = $jur_sel_id";
        }
    }
    $sql.=" GROUP BY $query_jur_groups[$jur_totals]  ORDER BY $query_jur_orders[$jur_totals]";
    $result = $dbh->doCachedQuery($sql, 10,true);
    return $result;
}

/**
 * @return string
 */
function getStartJurisdictionLevel(){
    if($_SESSION['jurisdiction_class'] == 'casino'){
        return 'nation';
    }
    if($_SESSION['jurisdiction_class'] == 'nation'){
        return 'region';
    }
    if($_SESSION['jurisdiction_class'] == 'region'){
        return 'district';
    }
    if($_SESSION['jurisdiction_class'] == 'district'){
        return 'club';
    }
}

/**
 * @param $jur_level
 * @param $jur_selected
 * @return string
 */
function getRadioGroupsLevel($jur_level, $jur_selected){
    global $lang;
    if ('nation' == $jur_level) {
        $str = '<input type="radio" value="nation" name="jur_level"';
        if($jur_selected == 'nation') $str .='checked';
        $str.='>'.$lang->getLang("Nation").'</input>';

        $str .= '<input type="radio" value="region" name="jur_level"';
        if($jur_selected == 'region') $str .='checked';
        $str.='>'.$lang->getLang("Region").'</input>';

        $str .= '<input type="radio" value="district" name="jur_level"';
        if($jur_selected == 'district') $str .='checked';
        $str.='>'.$lang->getLang("District").'</input>';

        $str .= '<input type="radio" value="club" name="jur_level"';
        if($jur_selected == 'club') $str .='checked';
        $str.='>'.$lang->getLang("Betting Club").'</input>';
        return $str;
    }
    if ('region' == $jur_level){
        $str = '<input type="radio" value="region" name="jur_level"';
        if($jur_selected == 'region') $str .='checked';
        $str.='>'.$lang->getLang("Region").'</input>';

        $str .= '<input type="radio" value="district" name="jur_level"';
        if($jur_selected == 'district') $str .='checked';
        $str.='>'.$lang->getLang("District").'</input>';

        $str .= '<input type="radio" value="club" name="jur_level"';
        if($jur_selected == 'club') $str .='checked';
        $str.='>'.$lang->getLang("Betting Club").'</input>';
        return $str;
    }
    if ('district' == $jur_level){
        $str = '<input type="radio" value="district" name="jur_level"';
        if($jur_selected == 'district') $str .='checked';
        $str.='>'.$lang->getLang("District").'</input>';

        $str .= '<input type="radio" value="club" name="jur_level"';
        if($jur_selected == 'club') $str .='checked';
        $str.='>'.$lang->getLang("Betting Club").'</input>';
        return $str;
    }

    $str = '<input type="radio" value="club" name="jur_level"';
    if($jur_selected == 'club') $str .='checked';
    $str.='>'.$lang->getLang("Betting Club").'</input>';
    return $str;
}

/**
 * @param $clubid
 * @param $date_start
 * @param $date_end
 * @param $betwin
 * @param $financial
 */
function getUsersFinancialDetailsByClub($clubid,$date_start, $date_end, $casino=0,$casino_live=0,$virtual=0,$poker2d=0,$pokerl=0,$royalpk=0,$playtech=0,$evolution=0,$infinity=0,$portomaso=0,$totem=0,$betting=0,$forex=0,$bankroulette=0,$zecchinetta=0,$greek=0,$zeus=0) {
    ini_set('memory_limit', '1999M' );
    global $dbh,$lang,$jur_sel_id,$row_to_page;
    $sql = " select pun_username,cus_res_pun_id,
     cus_res_day, jur_name,cur_id as currency,
	 coalesce(jps_perc_casino, 0) as jps_perc_casino,
     coalesce(jps_perc_casino_live, 0) as jps_perc_casino_live,
     coalesce(jps_perc_virtual, 0) as jps_perc_virtual,
     coalesce(jps_perc_poker_live, 0) as jps_perc_poker_live,
     coalesce(jps_perc_poker, 0) as jps_perc_poker_2d,
     coalesce(jps_perc_playtech, 0) as jps_perc_playtech,
     coalesce(jps_perc_evolution, 0) as jps_perc_evolution,
     coalesce(jps_perc_bankroulette, 0) as jps_perc_bankroulette,
     coalesce(jps_perc_zecchinetta, 0) as jps_perc_zecchinetta,
     coalesce(jps_perc_greek, 0) as jps_perc_greek,
     coalesce(jps_perc_infinity, 0) as jps_perc_infinity,
     coalesce(jps_perc_zeus, 0) as jps_perc_zeus,";/*
     coalesce(jps_perc_forex, 0) as jps_perc_forex,
     coalesce(jps_perc_royalpk, 0) as jps_perc_royalpk,*/

    $sql .= "sum(if (gam_category = 'casino', cus_res_bet, 0)) as casino_bet,
	 sum(if (gam_category = 'casino', cus_res_win, 0)) as casino_win,
	 sum(if (gam_category = 'casino_live', cus_res_bet, 0)) as casino_live_bet,
	 sum(if (gam_category = 'casino_live', cus_res_win, 0)) as casino_live_win,
	 sum(if (gam_category = 'casino_agency', cus_res_bet, 0)) as virtual_bet,
	 sum(if (gam_category = 'casino_agency', cus_res_win, 0)) as virtual_win,
	 sum(if (gam_category = 'casino_multitable_bank', cus_res_bet, 0)) as casino_bankroulette_bet,
	 sum(if (gam_category = 'casino_multitable_bank', cus_res_win, 0)) as casino_bankroulette_win,
     sum(if (gam_category = 'casino_multitable_bank', cus_res_rake, 0)) as casino_bankroulette_rake,
	 sum(if (gam_category = 'casino_multitable_zecchinetta', cus_res_bet, 0)) as casino_zecchinetta_bet,
	 sum(if (gam_category = 'casino_multitable_zecchinetta', cus_res_win, 0)) as casino_zecchinetta_win,
     sum(if (gam_category = 'casino_multitable_zecchinetta', cus_res_rake, 0)) as casino_zecchinetta_rake,";

    /*sum(if (gam_category = 'provider_casino_rpk', cus_res_bet, 0)) as royalpk_bet,
    sum(if (gam_category = 'provider_casino_rpk', cus_res_win, 0)) as royalpk_win,
     sum(if (gam_category = 'provider_casino_forex', cus_res_bet, 0)) as forex_bet,
    sum(if (gam_category = 'provider_casino_forex', cus_res_win, 0)) as forex_win,*/


    $sql .= "
	 sum(if (gam_category = 'provider_greekcasino', cus_res_bet, 0)) as casino_greek_bet,
	 sum(if (gam_category = 'provider_greekcasino', cus_res_win, 0)) as casino_greek_win,
     sum(if (gam_category = 'provider_greekcasino', cus_res_rake, 0)) as casino_greek_rake, 
     
	 sum(if (gam_category = 'provider_playtech', cus_res_bet, 0)) as playtech_bet,
	 sum(if (gam_category = 'provider_playtech', cus_res_win, 0)) as playtech_win,
	 
	 sum(if (gam_category = 'provider_evolution', cus_res_bet, 0)) as evolution_bet,
	 sum(if (gam_category = 'provider_evolution', cus_res_win, 0)) as evolution_win,
	 
	 sum(if (gam_category = 'provider_zeus', cus_res_bet, 0)) as zeus_bet,
	 sum(if (gam_category = 'provider_zeus', cus_res_win, 0)) as zeus_win,
	 
	 sum(if (gam_category in ('provider_infinity', 'provider_riseup'), cus_res_bet, 0)) as infinity_bet,
	 sum(if (gam_category in ('provider_infinity', 'provider_riseup'), cus_res_win, 0)) as infinity_win,
	 
	 sum(if (gam_category = 'provider_portomaso', cus_res_bet, 0)) as infinity_bet,
	 sum(if (gam_category = 'provider_portomaso', cus_res_win, 0)) as infinity_win,

	 sum(if (gam_category = 'provider_poker_live', cus_res_bet, 0)) as poker_live_bet,
	 sum(if (gam_category = 'provider_poker_live', cus_res_win, 0)) as poker_live_win,
	 sum(if (gam_category = 'provider_poker', cus_res_bet, 0)) as poker_2d_bet,
	 sum(if (gam_category = 'provider_poker', cus_res_win, 0)) as poker_2d_win,
     sum(if (gam_category = 'provider_poker', cus_res_bet, 0)) - sum(if (gam_category = 'provider_poker', cus_res_win, 0)) as poker_2d_net,";



    $sql .= " sum(if (gam_category = 'casino', cus_res_rake, 0)) as casino_rake,
                sum(if (gam_category = 'casino_live', cus_res_rake, 0)) as casino_live_rake,
                sum(if (gam_category = 'casino_agency', cus_res_rake, 0)) as virtual_rake, ";
    /*          sum(if (gam_category = 'provider_casino_rpk', cus_res_rake, 0)) as royalpk_rake,
            sum(if (gam_category = 'provider_casino_forex', cus_res_rake, 0)) as forex_rake,
    */
    $sql .= "   sum(if (gam_category = 'provider_playtech', cus_res_rake, 0)) as playtech_rake,
                sum(if (gam_category = 'provider_evolution', cus_res_rake, 0)) as evolution_rake,
                sum(if (gam_category = 'provider_zeus', cus_res_rake, 0)) as zeus_rake,
                sum(if (gam_category in ('provider_infinity', 'provider_riseup'), cus_res_rake, 0)) as infinity_rake,
                sum(if (gam_category = 'provider_portomaso', cus_res_rake, 0)) as portomaso_rake,
                sum(if (gam_category = 'provider_poker_live', cus_res_rake, 0)) as poker_live_rake,
                sum(if (gam_category = 'provider_poker', cus_res_rake, 0)) as poker_2d_rake,
                sum(if (gam_category = 'provider_poker_live', cus_res_tournament_bet, 0)) as poker_live_tournament_bet,
                sum(if (gam_category = 'provider_poker_live', cus_res_tournament_win, 0)) as poker_live_tournament_win,
                sum(if (gam_category = 'provider_poker_live', cus_res_tournament_rake, 0)) as poker_live_tournament_rake,
                sum(if (gam_category = 'provider_poker', cus_res_tournament_bet, 0)) as poker_2d_tournament_bet,
                sum(if (gam_category = 'provider_poker', cus_res_tournament_win, 0)) as poker_2d_tournament_win,
                sum(if (gam_category = 'provider_poker', cus_res_tournament_bet, 0)) - sum(if (gam_category = 'provider_poker', cus_res_tournament_win, 0)) as poker_2d_tournament_net,
                sum(if (gam_category = 'provider_poker', cus_res_tournament_rake, 0)) as poker_2d_tournament_rake
                from customer_result_daily
            LEFT JOIN game on gam_id = cus_res_gam_id
            LEFT JOIN jurisdiction_payment_settings on jps_jur_id = cus_res_club_id
            LEFT JOIN punter on pun_id = cus_res_pun_id
            LEFT JOIN jurisdiction on jur_id = cus_res_club_id
            LEFT JOIN currency on jur_currency=cur_id
            WHERE cus_res_club_id = ".$dbh->escape($clubid)."
            AND cus_res_day BETWEEN '$date_start 00:00:00' AND '$date_end 23:59:59'
            GROUP BY cus_res_pun_id";

    $start_row = (isset ( $_POST ['start_row'] )) ? $_POST ['start_row'] : 1;
    $res = $dbh->doCachedQuery($sql, 20,true);




    $totals=array();
    while($res->hasNext()){
        $row = $res->next();
        if($row["currency"]!=$_SESSION["currency_id"]){
            $diffCurr=true;
            foreach($row as $n=>$nv){
                if(((strpos($n,'bet') !== false) || (strpos($n,'win') !== false) || (strpos($n,'rake') !== false) || (strpos($n,'fin') !== false))&&(strpos($n,'betting') === false)){
                    if($nv!=0){
                        if($_SESSION["currency_id"]==1){
                            $res->Records[$res->CurrentIndex][$n]=$nv/$_SESSION["currencies"][$row["currency"]]["value"];
                            $row[$n]=$nv/$_SESSION["currencies"][$row["currency"]]["value"];
                        }
                        else{
                            $res->Records[$res->CurrentIndex][$n]=$nv*$_SESSION["currencies"][$_SESSION["currency_id"]]["value"];
                            $row[$n]=$nv*$_SESSION["currencies"][$_SESSION["currency_id"]]["value"];
                        }
                    }
                }
            }
        }

        $totals['casino_gross']+=$row['casino_bet'];
        $totals['casino_net']+=$row['casino_rake'];

        $totals['casino_live_gross']+=$row['casino_live_bet'];
        $totals['casino_live_net']+=$row['casino_live_rake'];

        $totals['virtual_gross']+=$row['virtual_bet'];
        $totals['virtual_net']+=$row['virtual_rake'];

        /*$totals['royalpk_gross']+=$row['royalpk_bet'];
        $totals['royalpk_net']+=$row['royalpk_rake'];

        $totals['forex_gross']+=$row['forex_bet'];
        $totals['forex_net']+=$row['forex_rake'];*/


        $totals['evolution_gross']+=$row['evolution_bet'];
        $totals['evolution_net']+=$row['evolution_rake'];
        
        $totals['zeus_gross']+=$row['zeus_bet'];
        $totals['zeus_net']+=$row['zeus_rake'];

        $totals['infinity_gross']+=$row['infinity_bet'];
        $totals['infinity_net']+=$row['infinity_rake'];
        
        $totals['portomaso_gross']+=$row['portomaso_bet'];
        $totals['portomaso_net']+=$row['portomaso_rake'];

        $totals['bankroulette_gross']+=$row['casino_bankroulette_bet'];
        $totals['zecchinetta_gross']+=$row['casino_zecchinetta_bet'];
        $totals['greek_gross']+=$row['casino_greek_bet'];

        $totals['poker_live_rake']+=($row['poker_live_rake']+$row['poker_live_tournament_rake']);

        $totals['poker_2d_rake']+=($row['poker_2d_rake']+$row['poker_2d_tournament_rake']);
        $totals['poker_2d_net']+=($row['poker_2d_net']+$row['poker_2d_tournament_net']);
        

        $totals['net_casino']+=$row['casino_rake'];
        $totals['net_casino_live']+=$row['casino_live_rake'];
        $totals['net_virtual']+=$row['virtual_rake'];
        /*$totals['net_royalpk']+=$row['royalpk_rake'];
        $totals['net_forex']+=$row['forex_rake'];*/
        $totals['net_playtech']+=$row['playtech_rake'];
        $totals['net_evolution']+=$row['evolution_rake'];
        $totals['net_infinity']+=$row['infinity_rake'];
        $totals['net_portomaso']+=$row['portomaso_rake'];
        $totals['net_zeus']+=$row['zeus_rake'];
        $totals['net_bankroulette']+=$row['casino_bankroulette_rake'];
        $totals['net_zecchinetta']+=$row['casino_zecchinetta_rake'];
        $totals['net_greek']+=$row['casino_greek_rake'];

        $totals['net_poker_live']+=$row['poker_live_rake'];
        $totals['net_poker_2d']+=$row['poker_2d_rake'];

        $totals['perc_casino']+=($row['casino_rake']/100*$row["jps_perc_casino"]);
        $totals['perc_casino_live']+= ($row['casino_live_rake']/100*$row["jps_perc_casino_live"]);
        $totals['perc_virtual']+= ($row['virtual_rake']/100*$row["jps_perc_virtual"]);

        /*$totals['perc_royalpk']+= ($row['royalpk_rake']/100*$row["jps_perc_royalpk"]);
        $totals['perc_forex']+= ($row['forex_rake']/100*$row["jps_perc_forex"]);
        */
        $totals['perc_playtech']+= ($row['playtech_rake']/100*$row["jps_perc_playtech"]);

        $totals['perc_evolution']+= ($row['evolution_rake']/100*$row["jps_perc_evolution"]);

        $totals['perc_bankroulette']+= ($row['casino_bankroulette_rake']/100*$row["jps_perc_bankroulette"]);
        $totals['perc_zecchinetta']+= ($row['casino_zecchinetta_rake']/100*$row["jps_perc_zecchinetta"]);
        $totals['perc_greek']+= ($row['casino_greek_rake']/100*$row["jps_perc_greek"]);
        $totals['perc_infinity']+= ($row['infinity_rake']/100*$row["jps_perc_infinity"]);
        $totals['perc_portomaso']+= ($row['portomaso_rake']/100*$row["jps_perc_infinity"]);

        $totals['perc_poker_live']+=(($row["poker_live_rake"]+$row["poker_live_tournament_rake"])/100*$row["jps_perc_poker_live"]);
        $totals['perc_poker_2d']+=(($row["poker_2d_rake"]+$row["poker_2d_tournament_rake"])/100*$row["jps_perc_poker_2d"]);
    }
    $cols_arr = array();
    $totalNetFormula="0";
    $totalPercFormula="0";
    $header = "<tr valign=top><td class='label centered'>customer</td>";
    $subheader="<tr valign=top><td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Username</td> ";
    $totalsHeader="<br /><br /><table bgcolor='#a99a9' cellpadding='4' cellspacing='1' border='0' style='margin:auto'><tr>";
    $totalsSubheader="<tr>";
    $totalsContent="<tr>";
    array_push($cols_arr,'customer');
    if($casino==1){
        $header.="   <td class='label centered' colspan='2'>casino</td>";
        $subheader.="<td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Gross</td><td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Net</td>";
        $totalsHeader.=" <td class='label centered' colspan='2'>Total casino</td>";
        $totalsSubheader.="<td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Gross</td><td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Net</td>";
        $totalsContent.="<td class='content'>".getInDollars($totals['casino_gross'])."</td> <td class='content'>".getInDollars($totals['casino_net'])."</td>";
        $totals['net']+=$totals['net_casino'];
        $totals['perc']+=$totals['perc_casino'];
        $totalNetFormula.='+$rec["casino_rake"]';
        $totalPercFormula.='+$rec["casino_rake"]/100*$rec["jps_perc_casino"]';
        array_push($cols_arr,'casino');

    }
    if($casino_live==1){
        $header.="  <td class='label centered' colspan='2'>casino live</td>";
        $subheader.="                <td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Gross</td><td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Net</td>";
        $totalsHeader.=" <td class='label centered' colspan='2'>Total casino live</td>";
        $totalsSubheader.="<td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Gross</td><td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Net</td>";
        $totalsContent.="<td class='content'>".getInDollars($totals['casino_live_gross'])."</td> <td class='content'>".getInDollars($totals['casino_live_net'])."</td>";
        $totals['net']+=$totals['net_casino_live'];
        $totals['perc']+=$totals['perc_casino_live'];
        $totalNetFormula.='+$rec["casino_live_rake"]';
        $totalPercFormula.='+$rec["casino_live_rake"]/100*$rec["jps_perc_casino_live"]';
        array_push($cols_arr, 'casino live');
    }

    if($bankroulette==1){
        $header.="<td class='label centered' colspan='2'>bankroulette</td>";
        $subheader.="<td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Gross</td><td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'> Net</td>";
        $totalsHeader.=" <td class='label centered' colspan='2'>Total BankRoulette</td>";
        $totalsSubheader.="<td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Gross</td><td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Net</td>";
        $totalsContent.="<td class='content'>".getInDollars($totals['bankroulette_gross'])."</td> <td class='content'>".getInDollars($totals['net_bankroulette'])."</td>";
        $totals['net']+=$totals['net_bankroulette'];
        $totals['perc']+=$totals['perc_bankroulette'];
        $totalNetFormula.='+$rec["casino_bankroulette_rake"]';
        $totalPercFormula.='+$rec["casino_bankroulette_rake"]/100*$rec["jps_perc_bankroulette"]';
        array_push($cols_arr,'bankroulette');
    }
    
    if($zeus==1){
        $header.="<td class='label centered' colspan='2'>zeus</td>";
        $subheader.="<td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Gross</td><td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'> Net</td>";
        $totalsHeader.=" <td class='label centered' colspan='2'>Total zeus</td>";
        $totalsSubheader.="<td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Gross</td><td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Net</td>";
        $totalsContent.="<td class='content'>".getInDollars($totals['zeus_gross'])."</td> <td class='content'>".getInDollars($totals['zeus_net'])."</td>";
        $totals['net']+=$totals['net_zeus'];
        $totals['perc']+=$totals['perc_zeus'];
        $totalNetFormula.='+$rec["zeus_rake"]';
        $totalPercFormula.='+$rec["zeus_rake"]/100*$rec["jps_perc_zeus"]';
        array_push($cols_arr,'zeus');
    }
    
    if($evolution==1){
        $header.="<td class='label centered' colspan='2'>evolution</td>";
        $subheader.="<td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Gross</td><td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'> Net</td>";
        $totalsHeader.=" <td class='label centered' colspan='2'>Total evolution</td>";
        $totalsSubheader.="<td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Gross</td><td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Net</td>";
        $totalsContent.="<td class='content'>".getInDollars($totals['evolution_gross'])."</td> <td class='content'>".getInDollars($totals['evolution_net'])."</td>";
        $totals['net']+=$totals['net_evolution'];
        $totals['perc']+=$totals['perc_evolution'];
        $totalNetFormula.='+$rec["evolution_rake"]';
        $totalPercFormula.='+$rec["evolution_rake"]/100*$rec["jps_perc_evolution"]';
        array_push($cols_arr,'evolution');
    }
    
    if($infinity==1){
        $header.="<td class='label centered' colspan='2'>infinity</td>";
        $subheader.="<td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Gross</td><td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'> Net</td>";
        $totalsHeader.=" <td class='label centered' colspan='2'>Total casino borut</td>";
        $totalsSubheader.="<td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Gross</td><td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Net</td>";
        $totalsContent.="<td class='content'>".getInDollars($totals['infinity_gross'])."</td> <td class='content'>".getInDollars($totals['infinity_net'])."</td>";
        $totals['net']+=$totals['net_infinity'];
        $totals['perc']+=$totals['perc_infinity'];
        $totalNetFormula.='+$rec["infinity_rake"]';
        $totalPercFormula.='+$rec["infinity_rake"]/100*$rec["jps_perc_infinity"]';
        array_push($cols_arr,'infinity');
    }
    
    if($portomaso==1){
        $header.="<td class='label centered' colspan='2'>infinity</td>";
        $subheader.="<td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Gross</td><td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'> Net</td>";
        $totalsHeader.=" <td class='label centered' colspan='2'>Total casino borut</td>";
        $totalsSubheader.="<td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Gross</td><td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Net</td>";
        $totalsContent.="<td class='content'>".getInDollars($totals['portomaso_gross'])."</td> <td class='content'>".getInDollars($totals['portomaso_net'])."</td>";
        $totals['net']+=$totals['net_portomaso'];
        $totals['perc']+=$totals['perc_portomaso'];
        $totalNetFormula.='+$rec["portomaso_rake"]';
        $totalPercFormula.='+$rec["portomaso_rake"]/100*$rec["jps_perc_infinity"]';
        array_push($cols_arr,'infinity');
    }
    if($poker2d==1){
        $header.="<td class='label centered' >poker 2d</td>";
        $subheader.="<td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Rake</td>";
        $totalsHeader.=" <td class='label centered' >Total poker 2d</td>";
        $totalsSubheader.="<td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Rake</td>";
        $totalsContent.="<td class='content'>".getInDollars($totals['poker_2d_rake'])."</td>";
        $totals['net']+=$totals['net_poker_2d'];
        $totals['perc']+=$totals['perc_poker_2d'];
        $totalNetFormula.='+$rec["poker_2d_rake"]';
        $totalPercFormula.='+(($rec["poker_2d_rake"]+$rec["poker_2d_tournament_rake"])/100*$rec["jps_perc_poker_2d"])';
        array_push($cols_arr, 'poker 2d');
    }
    $header.="<td class='label centered' colspan='2'>total</td></tr>";
    $subheader.=" <td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Net+Rake</td><td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'> x%</td>";
    $totalsHeader.=" <td class='label centered' colspan='2'>Total</td></tr>";
    $totalsSubheader.="<td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'>Net+Rake</td><td class='content centered' style='border-bottom: 2px solid rgb(0, 0, 0);'> x%</td></tr>";
    $totalsContent.="<td class='content'>".getInDollars($totals['net'])."</td><td class='content'>".getInDollars($totals['perc'])."</td></tr></table><br />";
    $header.=$subheader;
    array_push($cols_arr, 'total');
    $post_vars = array('active'=>'usersFinancial',
        'doQuery'=>'yes',
        'date_start'=>$date_start,
        'date_end'=>$date_end,
        'jurisdiction'=>$jur_sel_id,
        'jur_level'=>'club',
        'casino'=>$casino,
        'casino_live'=>$casino_live,
        'bankroulette'=>$bankroulette,
        'zeus'=>$zeus,/*
        'virtual'=>$virtual,
        'royalpk'=>$royalpk,
        'playtech'=>$playtech,
        'zecchinetta'=>$zecchinetta,
        'greek'=>$greek,
        'forex'=>$forex,*/
        'evolution'=>$evolution,
        'infinity'=>$infinity,
        'portomaso'=>$portomaso,
        'pokerl'=>$pokerl,
        'poker2d'=>$poker2d
    );




    $cell_fmt_arr = array (
        'customer'               => 'style="text-align:right;white-space: nowrap;" class="content dark"',
        'casino'                 => 'style="text-align:right;white-space: nowrap;" class="content light"',
        'casino live'            => 'style="text-align:right;white-space: nowrap;" class="content dark"',
        'bankroulette'              => 'style="text-align:right;white-space: nowrap;" class="content light"',
        'zeus'              => 'style="text-align:right;white-space: nowrap;" class="content light"',
        /*'virtual'                => 'style="text-align:right;white-space: nowrap;" class="content light" ',
        'royalpk'                => 'style="text-align:right;white-space: nowrap;" class="content dark"',
        'zecchinetta'              => 'style="text-align:right;white-space: nowrap;" class="content light"',
        'greek'              => 'style="text-align:right;white-space: nowrap;" class="content light"',
        'forex'              => 'style="text-align:right;white-space: nowrap;" class="content light"',
        'playtech'              => 'style="text-align:right;white-space: nowrap;" class="content light"',*/
        'evolution'              => 'style="text-align:right;white-space: nowrap;" class="content light"',
        'infinity'              => 'style="text-align:right;white-space: nowrap;" class="content light"',
        'portomaso'              => 'style="text-align:right;white-space: nowrap;" class="content light"',

        'poker live'             => 'style="text-align:right;white-space: nowrap;" class="content dark"',
        'poker 2d'               => 'style="text-align:right;white-space: nowrap;" class="content light"'
    );
    $val_fmt_arr = array (
        'customer'                 => 'return $rec["pun_username"];',
        'casino'                   => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_bet"]))."</td><td style=\"text-align: right\">".getInDollars($rec["casino_rake"]);',
        'casino live'              => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_live_bet"]))."</td><td style=\"text-align: right\">".getInDollars($rec["casino_live_fin"]);',
        /* 'virtual'                  => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["virtual_bet"]))."</td><td style=\"text-align: right\">".getInDollars($rec["virtual_rake"]);',
        'royalpk'                  => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["royalpk_bet"]))."</td><td style=\"text-align: right\">".getInDollars($rec["royalpk_rake"]);',
         'forex'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["forex_bet"]))."</td><td style=\"text-align: right\">".getInDollars($rec["forex_rake"]);',
        'playtech'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["playtech_bet"]))."</td><td style=\"text-align: right\">".getInDollars($rec["playtech_rake"]);',
        'zecchinetta'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_zecchinetta_bet"]))."</td><td style=\"text-align: right\">".getInDollars($rec["casino_zecchinetta_rake"]);',
        'greek'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_greek_bet"]))."</td><td style=\"text-align: right\">".getInDollars($rec["casino_greek_rake"]);',*/
        
        'bankroulette'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["casino_bankroulette_bet"]))."</td><td style=\"text-align: right\">".getInDollars($rec["casino_bankroulette_rake"]);',
        'zeus'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["zeus_bet"]))."</td><td style=\"text-align: right\">".getInDollars($rec["zeus_rake"]);',
        'evolution'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["evolution_bet"]))."</td><td style=\"text-align: right\">".getInDollars($rec["evolution_rake"]);',
        'infinity'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["infinity_bet"]))."</td><td style=\"text-align: right\">".getInDollars($rec["infinity_rake"]);',
        'portomaso'                => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["portomaso_bet"]))."</td><td style=\"text-align: right\">".getInDollars($rec["portomaso_rake"]);',
        'poker live'               => 'return preg_replace("/\&nbsp; /", "",getInDollars($rec["poker_live_rake"]+$rec["poker_live_tournament_rake"]));',
        'poker 2d'                 => 'return getInDollars(($rec["poker_2d_rake"]+$rec["poker_2d_tournament_rake"]));',
        'total'                    => 'return getInDollars('.$totalNetFormula.')."</td><td style=\"text-align: right\">".getInDollars('.$totalPercFormula.');'
    );
    $totalTable=$totalsHeader.$totalsSubheader.$totalsContent;

    echo $totalTable;
    $qp = new QueryPrinter($res);
    $qp->printTable($res,$cols_arr, $post_vars, $val_fmt_arr, $cell_fmt_arr, '', '', $header, $width="100%", $start_row, $row_to_page,false,"",false);
    //$qp->dumpToExcelFile($res,$cols_arr,$val_fmt_arr,$excel_filename,$header);

}


function getPartnersFinancial($partner,$date_start, $date_end,$casino,$poker,$royalpk){
    global $dbh;
    if(is_array($partner))
    {
        $partner=implode(",",$partner);
    }


    $datetime1 =  date("Y-m",strtotime($date_start));
    $datetime2 =  date("Y-m",strtotime($date_end));
    $sameMonth = ($datetime1==$datetime2);
    if($sameMonth){
        $dateFormat="date_format(jds_day, '%Y-%m')";

    }
    else{
        $dateFormat="jds_day";
    }
    $sql= "select $dateFormat MONTH, ptn_name NAME,cur_id as currency, cur_code_for_web,cur_code,
sum(CASE WHEN jds_gam_category in (1, 2) THEN jds_bet ELSE 0 END ) CASINO_BET,
sum(CASE WHEN jds_gam_category in (1, 2) THEN jds_win ELSE 0 END ) CASINO_WIN,
sum(CASE WHEN jds_gam_category in (1, 2) THEN jds_rake ELSE 0 END ) CASINO_NET, ";
    /*sum(CASE WHEN jds_gam_category = 10 THEN jds_bet ELSE 0 END ) ROYALPK_BET,
    sum(CASE WHEN jds_gam_category = 10 THEN jds_win ELSE 0 END ) ROYALPK_WIN,
    sum(CASE WHEN jds_gam_category = 10 THEN jds_rake ELSE 0 END ) ROYALPK_RAKE,
    sum(CASE WHEN jds_gam_category = 13 THEN jds_bet ELSE 0 END ) FOREX_BET,
    sum(CASE WHEN jds_gam_category = 13 THEN jds_win ELSE 0 END ) FOREX_WIN,
    sum(CASE WHEN jds_gam_category = 13 THEN jds_rake ELSE 0 END ) FOREX_RAKE,*/

    $sql.="sum(CASE WHEN jds_gam_category = 7 THEN jds_bet ELSE 0 END ) BETTING_BET,
sum(CASE WHEN jds_gam_category = 7 THEN jds_win ELSE 0 END ) BETTING_WIN,
sum(CASE WHEN jds_gam_category = 7 THEN jds_bet ELSE 0 END ) - sum(CASE WHEN jds_gam_category = 7 THEN jds_win ELSE 0 END ) BETTING_RAKE,

sum(CASE WHEN jds_gam_category = 9 THEN jds_bet ELSE 0 END ) BETTING_LIVE_BET,
sum(CASE WHEN jds_gam_category = 9 THEN jds_win ELSE 0 END ) BETTING_LIVE_WIN,
sum(CASE WHEN jds_gam_category = 9 THEN jds_bet ELSE 0 END ) - sum(CASE WHEN jds_gam_category = 9 THEN jds_win ELSE 0 END ) BETTING_LIVE_RAKE,

sum(CASE WHEN jds_gam_category = 5 THEN jds_bet ELSE 0 END ) POKER_LIVE_BET,
sum(CASE WHEN jds_gam_category = 5 THEN jds_win ELSE 0 END ) POKER_LIVE_WIN,
sum(CASE WHEN jds_gam_category = 5 THEN jds_bet ELSE 0 END ) - sum(CASE WHEN jds_gam_category = 5 THEN jds_win ELSE 0 END ) POKER_LIVE_DIFF,
sum(CASE WHEN jds_gam_category = 5 THEN jds_rake ELSE 0 END ) POKER_LIVE_RAKE,
sum(CASE WHEN jds_gam_category = 8 THEN jds_bet ELSE 0 END ) POKER_BET,
sum(CASE WHEN jds_gam_category = 8 THEN jds_win ELSE 0 END ) POKER_WIN,
sum(CASE WHEN jds_gam_category = 8 THEN jds_bet ELSE 0 END ) - sum(CASE WHEN jds_gam_category = 8 THEN jds_win ELSE 0 END ) POKER_DIFF,
sum(CASE WHEN jds_gam_category = 8 THEN jds_rake ELSE 0 END ) POKER_RAKE
FROM partners,jurisdiction,currency, jurisdiction_daily_summary
WHERE jds_day BETWEEN '".$date_start."' AND '".$date_end."'
and jds_jur_club = ptn_jur_id
and jur_id=ptn_jur_id
and jur_currency=cur_id
and ptn_id in($partner)
group by ptn_id";
    return $dbh->doCachedQuery($sql, 20,true);
}


function getFinancialByCustomer($clubid,$date_start, $date_end, $betwin, $financial,$casino=0,$casino_live=0,$virtual=0,$poker2d=0,$pokerl=0,$royalpk=0,$playtech=0,$evolution=0,$infinity=0,$portomaso=0,$totem=0,$forex=0,$bankroulette=0,$zecchinetta=0,$greek=0,$zeus=0) {
    global $dbh,$lang;
    $sql = " select pun_username,cus_res_pun_id,
    coalesce(jps_perc_casino_live, 0),
    coalesce(jps_perc_casino, 0),
    coalesce(jps_perc_poker_live, 0),
    coalesce(jps_perc_virtual, 0),
	 cus_res_day, jur_name,cur_id as currency,
	 coalesce(jps_perc_casino, 0) as jps_perc_casino,
	 coalesce(jps_perc_bankroulette, 0) as jps_perc_bankroulette,
	 coalesce(jps_perc_zecchinetta, 0) as jps_perc_zecchinetta,
	 coalesce(jps_perc_greek, 0) as jps_perc_greek,
     coalesce(jps_perc_casino_live, 0) as jps_perc_casino_live,
     coalesce(jps_perc_virtual, 0) as jps_perc_virtual,
     coalesce(jps_perc_poker_live, 0) as jps_perc_poker_live,
     coalesce(jps_perc_poker, 0) as jps_perc_poker_2d, ";

    /* coalesce(jps_perc_royalpk, 0) as jps_perc_royalpk,
     coalesce(jps_perc_forex, 0) as jps_perc_forex,
    */
    $sql.="coalesce(jps_perc_playtech, 0) as jps_perc_playtech,
     coalesce(jps_perc_evolution, 0) as jps_perc_evolution,
     coalesce(jps_perc_infinity, 0) as jps_perc_infinity,
     coalesce(jps_perc_zeus, 0) as jps_perc_zeus,

     coalesce(jps_perc_totem, 0) as jps_perc_totem,
     coalesce(jps_perc_betting, 0) as jps_perc_betting,
     coalesce(jps_perc_live_betting, 0) as jps_perc_betting_live, ";


    if($betwin)
    {
        $sql .= " sum(if (gam_category = 'casino', cus_res_bet, 0)) as casino_bet,
	                sum(if (gam_category = 'casino', cus_res_win, 0)) as casino_win,
	                sum(if (gam_category = 'casino_live', cus_res_bet, 0)) as casino_live_bet,
	                sum(if (gam_category = 'casino_live', cus_res_win, 0)) as casino_live_win,
	                sum(if (gam_category = 'casino_agency', cus_res_bet, 0)) as virtual_bet,
	                sum(if (gam_category = 'casino_agency', cus_res_win, 0)) as virtual_win, ";

        /* sum(if (gam_category = 'provider_casino_rpk', cus_res_bet, 0)) as royalpk_bet,
         sum(if (gam_category = 'provider_casino_rpk', cus_res_win, 0)) as royalpk_win,
         sum(if (gam_category = 'provider_casino_forex', cus_res_bet, 0)) as forex_bet,
         sum(if (gam_category = 'provider_casino_forex', cus_res_win, 0)) as forex_win,*/

        $sql.=" sum(if (gam_category = 'provider_playtech', cus_res_bet, 0)) as playtech_bet,
	                sum(if (gam_category = 'provider_playtech', cus_res_win, 0)) as playtech_win,
	                
	                sum(if (gam_category = 'provider_evolution', cus_res_bet, 0)) as evolution_bet,
	                sum(if (gam_category = 'provider_evolution', cus_res_win, 0)) as evolution_win,
	                
	                sum(if (gam_category = 'provider_zeus', cus_res_bet, 0)) as zeus_bet,
	                sum(if (gam_category = 'provider_zeus', cus_res_win, 0)) as zeus_win,
	                
	                sum(if (gam_category in ('provider_infinity', 'provider_riseup'), cus_res_bet, 0)) as infinity_bet,
	                sum(if (gam_category in ('provider_infinity', 'provider_riseup'), cus_res_win, 0)) as infinity_win,
	                
	                sum(if (gam_category = 'provider_portomaso', cus_res_bet, 0)) as portomaso_bet,
	                sum(if (gam_category = 'provider_portomaso', cus_res_win, 0)) as portomaso_win,

	                sum(if (gam_category = 'totem', cus_res_bet, 0)) as totem_bet,
	                sum(if (gam_category = 'totem', cus_res_win, 0)) as totem_win,
	                sum(if (gam_category = 'provider_poker_live', cus_res_bet, 0)) as poker_live_bet,
	                sum(if (gam_category = 'provider_poker_live', cus_res_win, 0)) as poker_live_win,
	                sum(if (gam_category = 'provider_poker', cus_res_bet, 0)) as poker_2d_bet,
	                sum(if (gam_category = 'provider_poker', cus_res_win, 0)) as poker_2d_win,
	                sum(if (gam_category = 'provider_betting', cus_res_bet, 0)) as betting_bet,
	                sum(if (gam_category = 'provider_betting', cus_res_win, 0)) as betting_win,
	                sum(if (gam_category = 'provider_live_betting', cus_res_bet, 0)) as betting_bet_live,
	                sum(if (gam_category = 'provider_live_betting', cus_res_win, 0)) as betting_win_live,
                    sum(if (gam_category = 'casino_multitable_bank', cus_res_bet, 0)) as casino_bankroulette_bet,
                    sum(if (gam_category = 'casino_multitable_bank', cus_res_win, 0)) as casino_bankroulette_win,
                    sum(if (gam_category = 'casino_multitable_zecchinetta', cus_res_bet, 0)) as casino_zecchinetta_bet,
                    sum(if (gam_category = 'casino_multitable_zecchinetta', cus_res_win, 0)) as casino_zecchinetta_win,
                    sum(if (gam_category = 'provider_greekcasino', cus_res_bet, 0)) as casino_greek_bet,
                    sum(if (gam_category = 'provider_greekcasino', cus_res_win, 0)) as casino_greek_win,
	                 ";
    }
    if($financial){
        $sql .= "sum(if (gam_category = 'provider_poker_live', cus_res_fin, 0)) as poker_fin,
             sum(if (gam_category = 'provider_poker', cus_res_fin, 0)) as poker_2d_fin,
             sum(if (gam_category = 'casino_live', cus_res_fin, 0)) as casino_live_fin,
             sum(if (gam_category = 'casino_agency', cus_res_fin, 0)) as virtual_fin,
             sum(if (gam_category = 'casino', cus_res_fin, 0)) as casino_fin, ";
        /* sum(if (gam_category = 'provider_casino_rpk', cus_res_fin, 0)) as royalpk_fin,
              sum(if (gam_category = 'provider_casino_forex', cus_res_fin, 0)) as forex_fin,*/
        $sql.="  sum(if (gam_category = 'provider_playtech', cus_res_fin, 0)) as playtech_fin,
           
           sum(if (gam_category = 'provider_evolution', cus_res_fin, 0)) as evolution_fin,
           
             sum(if (gam_category in ('provider_infinity', 'provider_riseup'), cus_res_fin, 0)) as infinity_fin,
           
             sum(if (gam_category = 'provider_portomaso', cus_res_fin, 0)) as portomaso_fin,
           
             sum(if (gam_category = 'provider_zeus', cus_res_fin, 0)) as zeus_fin,

             sum(if (gam_category = 'totem', cus_res_fin, 0)) as totem_fin,
             sum(if (gam_category = 'pr ovider_betting', cus_res_fin, 0)) as provider_fin,
             sum(if (gam_category = 'provider_live_betting', cus_res_fin, 0)) as provider_fin_live,";
    }
    $sql .= " sum(if (gam_category = 'casino', cus_res_rake, 0)) as casino_rake,
                sum(if (gam_category = 'casino_live', cus_res_rake, 0)) as casino_live_rake,
                sum(if (gam_category = 'casino_agency', cus_res_rake, 0)) as virtual_rake, ";
    /*   sum(if (gam_category = 'provider_casino_rpk', cus_res_rake, 0)) as royalpk_rake,
        sum(if (gam_category = 'provider_casino_forex', cus_res_rake, 0)) as forex_rake,*/
    $sql.="   sum(if (gam_category = 'provider_playtech', cus_res_rake, 0)) as playtech_rake,
             sum(if (gam_category = 'provider_evolution', cus_res_rake, 0)) as evolution_rake,
             sum(if (gam_category = 'provider_zeus', cus_res_rake, 0)) as zeus_rake,
                sum(if (gam_category in ('provider_infinity', 'provider_riseup'), cus_res_rake, 0)) as infinity_rake,
                sum(if (gam_category = 'provider_portomaso', cus_res_rake, 0)) as portomaso_rake,
                sum(if (gam_category = 'totem', cus_res_rake, 0)) as totem_rake,
                sum(if (gam_category = 'provider_poker_live', cus_res_rake, 0)) as poker_live_rake,
                sum(if (gam_category = 'provider_poker', cus_res_rake, 0)) as poker_2d_rake,
                sum(if (gam_category = 'provider_betting', cus_res_rake, 0)) as betting_rake,
                sum(if (gam_category = 'provider_live_betting', cus_res_rake, 0)) as betting_rake_live,
                sum(if (gam_category = 'casino_multitable_bank', cus_res_rake, 0)) as casino_bankroulette_rake,
                sum(if (gam_category = 'casino_multitable_bank', cus_res_fin, 0)) as casino_bankroulette_fin,
                sum(if (gam_category = 'casino_multitable_zecchinetta', cus_res_rake, 0)) as casino_zecchinetta_rake,
                sum(if (gam_category = 'casino_multitable_zecchinetta', cus_res_fin, 0)) as casino_zecchinetta_fin,
                sum(if (gam_category = 'provider_greekcasino', cus_res_rake, 0)) as casino_greek_rake,
                sum(if (gam_category = 'provider_greekcasino', cus_res_fin, 0)) as casino_greek_fin,
                sum(if (gam_category = 'provider_poker_live', cus_res_tournament_bet, 0)) as poker_live_tournament_bet,
                sum(if (gam_category = 'provider_poker_live', cus_res_tournament_win, 0)) as poker_live_tournament_win,
                sum(if (gam_category = 'provider_poker_live', cus_res_tournament_rake, 0)) as poker_live_tournament_rake,
                sum(if (gam_category = 'provider_poker', cus_res_tournament_bet, 0)) as poker_2d_tournament_bet,
                sum(if (gam_category = 'provider_poker', cus_res_tournament_win, 0)) as poker_2d_tournament_win,
                sum(if (gam_category = 'provider_poker', cus_res_tournament_rake, 0)) as poker_2d_tournament_rake
                from customer_result_daily
            LEFT JOIN game on gam_id = cus_res_gam_id
            LEFT JOIN jurisdiction_payment_settings on jps_jur_id = cus_res_club_id
            LEFT JOIN punter on pun_id = cus_res_pun_id
            LEFT JOIN jurisdiction on jur_id = cus_res_club_id
            LEFT JOIN currency on jur_currency=cur_id
            WHERE cus_res_club_id = '$clubid'
            AND cus_res_day BETWEEN '$date_start 00:00:00' AND '$date_end 23:59:59'
            GROUP BY cus_res_pun_id";
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
                if($casino==1){
                    if($betwin)
                    {?>
                        <th class="label centered">Casino<br><?=$lang->getLang("Bet")?></th>
                        <th class="label centered">Casino<br><?$lang->getLang("Win")?></th>
                    <?php }  ?>
                    <th class="label centered">Casino<br><?=$lang->getLang("Net")?></th>
                    <?if($financial)
                    { ?>
                        <th class="label centered">Casino<br><?=$lang->getLang("Fin")?></th>
                    <?php }
                }
                ?>
                <?php
                if($casino_live==1){
                    if($betwin)
                    {?>
                        <th class="label centered">Casino<br>Live Bet</th>
                        <th class="label centered">Casino<br>Live Win</th>
                    <?php }
                    ?>
                    <th class="label centered">Casino<br>Live Net</th>
                    <?php
                    if($financial)
                    {?>
                        <th class="label centered">Casino<br> Live Fin</th>
                    <?php } }  ?>
                <?php
                if($virtual==1){
                    if($betwin)
                    {?>
                        <th class="label centered">Virtual<br><?=$lang->getLang("Bet")?></th>
                        <th class="label centered">Virtual<br><?=$lang->getLang("Win")?></th>
                    <?php }
                    ?>
                    <th class="label centered">Virtual<br><?=$lang->getLang("Net")?></th>
                    <?if($financial)
                    { ?>
                        <th class="label centered">Virtual<br><?=$lang->getLang("Fin")?></th>
                    <?php } } ?>
                <?php
                if($bankroulette==1){
                    if($betwin)
                    {?>
                        <th class="label centered">Bankroulette<br><?=$lang->getLang("Bet")?></th>
                        <th class="label centered">Bankroulette<br><?=$lang->getLang("Win")?></th>
                    <?php }
                    ?>
                    <th class="label centered">Bankroulette<br><?=$lang->getLang("Rake")?></th>
                    <th class="label centered">Bankroulette<br><?=$lang->getLang("Fin")?></th>
                    <? } ?>
                <?php
                if($zecchinetta==1){
                    if($betwin)
                    {?>
                        <th class="label centered">Zecchinetta<br><?=$lang->getLang("Bet")?></th>
                        <th class="label centered">Zecchinetta<br><?=$lang->getLang("Win")?></th>
                    <?php }
                    ?>
                    <th class="label centered">Zecchinetta<br><?=$lang->getLang("Rake")?></th>
                    <th class="label centered">Zecchinetta<br><?=$lang->getLang("Fin")?></th>
                <? } ?>
                <?php
                if($greek==1){
                    if($betwin)
                    {?>
                        <th class="label centered">Greek<br><?=$lang->getLang("Bet")?></th>
                        <th class="label centered">Greek<br><?=$lang->getLang("Win")?></th>
                    <?php }
                    ?>
                    <th class="label centered">Greek<br><?=$lang->getLang("Net")?></th>
                <? } ?>
                <?php
                if($zeus==1){
                    if($betwin)
                    {?>
                        <th class="label centered">Zeus<br><?=$lang->getLang("Bet")?></th>
                        <th class="label centered">Zeus<br><?=$lang->getLang("Win")?></th>
                    <?php }
                    ?>
                    <th class="label centered">Zeus<br><?=$lang->getLang("Net")?></th>
                <? } ?>
                <?php
                if($royalpk==1){
                    if($betwin)
                    {?>
                        <th class="label centered">RoyalPk<br><?=$lang->getLang("Bet")?></th>
                        <th class="label centered">RoyalPk<br><?=$lang->getLang("Win")?></th>
                    <?php }
                    ?>
                    <th class="label centered">RoyalPk<br><?=$lang->getLang("Net")?></th>
                    <?if($financial)
                    { ?>
                        <th class="label centered">RoyalPk<br><?=$lang->getLang("Fin")?></th>
                    <?php } } ?>
                <?php
                if($playtech==1){
                    if($betwin)
                    {?>
                        <th class="label centered">playtech<br><?=$lang->getLang("Bet")?></th>
                        <th class="label centered">playtech<br><?=$lang->getLang("Win")?></th>
                    <?php }
                    ?>
                    <th class="label centered">playtech<br><?=$lang->getLang("Net")?></th>
                    <?if($financial)
                    { ?>
                        <th class="label centered">playtech<br><?=$lang->getLang("Fin")?></th>
                    <?php } } ?>

                <?php
                if($evolution==1){
                    if($betwin)
                    {?>
                        <th class="label centered">evolution<br><?=$lang->getLang("Bet")?></th>
                        <th class="label centered">evolution<br><?=$lang->getLang("Win")?></th>
                    <?php }
                    ?>
                    <th class="label centered">evolution<br><?=$lang->getLang("Net")?></th>
                    <?if($financial)
                    { ?>
                        <th class="label centered">evolution<br><?=$lang->getLang("Fin")?></th>
                    <?php } } ?>


                <?php
                if($infinity==1){
                    if($betwin)
                    {?>
                        <th class="label centered">infinity<br><?=$lang->getLang("Bet")?></th>
                        <th class="label centered">infinity<br><?=$lang->getLang("Win")?></th>
                    <?php }
                    ?>
                    <th class="label centered">infinity<br><?=$lang->getLang("Net")?></th>
                    <?if($financial)
                    { ?>
                        <th class="label centered">infinity<br><?=$lang->getLang("Fin")?></th>
                    <?php } } ?>
                    
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
                <?php
                if($forex==1){
                    if($betwin)
                    {?>
                        <th class="label centered">Forex<br><?=$lang->getLang("Bet")?></th>
                        <th class="label centered">Forex<br><?=$lang->getLang("Win")?></th>
                    <?php }
                    ?>
                    <th class="label centered">Forex<br><?=$lang->getLang("Net")?></th>
                    <?if($financial)
                    { ?>
                        <th class="label centered">Forex<br><?=$lang->getLang("Fin")?></th>
                    <?php } } ?>

                <?php
                if($totem==1){
                    if($betwin)
                    {?>
                        <th class="label centered">Totem<br><?=$lang->getLang("Bet")?></th>
                        <th class="label centered">Totem<br><?=$lang->getLang("Win")?></th>
                    <?php }
                    ?>
                    <th class="label centered">Totem<br><?=$lang->getLang("Net")?></th>
                    <?if($financial)
                    { ?>
                        <th class="label centered">Totem<br><?=$lang->getLang("Fin")?></th>
                    <?php } } ?>


                <?php
                if($pokerl==1){
                    if($betwin)
                    {?>
                        <th class="label centered">Poker Live<br><?=$lang->getLang("Bet")?></th>
                        <th class="label centered">Poker Live<br><?=$lang->getLang("Win")?></th>
                    <?php }
                    ?>
                    <th class="label centered">Poker Live<br><?=$lang->getLang("Rake")?></th>
                    <?if($financial)
                    { ?>
                        <th class="label centered">Poker Live<br><?=$lang->getLang("Fin")?></th>
                    <?php }   } ?>
                <?php
                if($poker2d==1){
                    if($betwin)
                    {?>
                        <th class="label centered">Poker 2d<br><?$lang->getLang("Bet")?></th>
                        <th class="label centered">Poker 2d<br><?=$lang->getLang("Win")?></th>
                        <th class="label centered">Poker 2d<br><?=$lang->getLang("Net")?></th>
                    <?php }
                    ?>
                    <th class="label centered">Poker 2d<br><?=$lang->getLang("Rake")?></th>
                    <?if($financial)
                    { ?>
                        <th class="label centered">Poker 2d<br><?=$lang->getLang("Fin")?></th>
                    <?php }  } ?>
                <?php
                if($betting==1){
                    if($betwin)
                    {?>
                        <th class="label centered">Betting<br><?=$lang->getLang("Bet")?></th>
                        <th class="label centered">Betting<br><?=$lang->getLang("Win")?></th>
                    <?php }
                    ?>
                    <th class="label centered">Betting<br><?=$lang->getLang("Net")?></th>
                    <?if($financial)
                    { ?>
                        <th class="label centered">Betting<br><?=$lang->getLang("Fin")?></th>
                    <?php }  ?>


                    <?php
                    if($betwin)
                    {?>
                        <th class="label centered">Betting Live<br><?=$lang->getLang("Bet")?></th>
                        <th class="label centered">Betting Live<br><?$lang->getLang("Win")?></th>
                    <?php }
                    ?>
                    <th class="label centered">Betting Live<br><?=$lang->getLang("Net")?></th>
                    <?if($financial)
                    { ?>
                        <th class="label centered">Betting Live<br><?=$lang->getLang("Fin")?></th>
                    <?php }   } ?>
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
                    if($casino==1){
                        if($betwin)
                        {?>
                            <td class="content dark" style="text-align:right"><?=getInDollars($row["casino_bet"])?><br />
                                <span class="tip"><?=$row['jps_perc_casino']."%=".getInDollars($row["casino_bet"]/100*$row['jps_perc_casino'])?></span>
                            </td>
                            <td class="content dark" style="text-align:right"><?=getInDollars($row["casino_win"])?><br />
                                <span class="tip"><?=$row['jps_perc_casino']."%=".getInDollars($row["casino_win"]/100*$row['jps_perc_casino'])?></span>
                            </td>
                        <?php }?>
                        <td class="content dark" style="text-align:right"><?=getInDollars($row["casino_rake"])?><br />
                            <span class="tip"> <?=$row['jps_perc_casino']."%=".getInDollars($row["casino_rake"]/100*$row['jps_perc_casino'])?></span>
                        </td>
                        <?if($financial)
                        {?>
                            <td class="content dark" style="text-align:right"><?=getInDollars($row["casino_fin"])?><br />
                                <span class="tip"> <?=$row['jps_perc_casino']."%=".getInDollars($row["casino_fin"]/100*$row['jps_perc_casino'])?></span>
                            </td>
                        <?php } } ?>
                    <?php
                    if($casino_live==1){
                        if($betwin)
                        {?>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["casino_live_bet"])?><br />
                                <span class="tip">  <?=$row['jps_perc_casino_live']."%=".getInDollars($row["casino_live_bet"]/100*$row['jps_perc_casino_live'])?></span></td>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["casino_live_win"])?><br />
                                <span class="tip"> <?=$row['jps_perc_casino_live']."%=".getInDollars($row["casino_live_win"]/100*$row['jps_perc_casino_live'])?></span></td>
                        <?php }?>
                        <td class="content light" style="text-align:right"><?=getInDollars($row["casino_live_rake"])?><br />
                            <span class="tip"><?=$row['jps_perc_casino_live']."%=".getInDollars($row["casino_live_rake"]/100*$row['jps_perc_casino_live'])?></span></td>
                        <?if($financial)
                        {?>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["casino_live_fin"])?><br />
                                <span class="tip">  <?=$row['jps_perc_casino_live']."%=".getInDollars($row["casino_live_fin"]/100*$row['jps_perc_casino_live'])?></span></td>
                        <?php } } ?>
                    <?php
                    if($virtual==1){
                        if($betwin)
                        {?>
                            <td class="content dark" style="text-align:right"><?=getInDollars($row["virtual_bet"])?><br />
                                <span class="tip"><?=$row['jps_perc_virtual']."%=".getInDollars($row["virtual_bet"]/100*$row['jps_perc_virtual'])?></span></td>
                            <td class="content dark" style="text-align:right"><?=getInDollars($row["virtual_win"])?><br />
                                <span class="tip"><?=$row['jps_perc_virtual']."%=".getInDollars($row["virtual_win"]/100*$row['jps_perc_virtual'])?></span></td>
                        <?php }?>
                        <td class="content dark" style="text-align:right"><?=getInDollars($row["virtual_rake"])?><br />
                            <span class="tip"><?=$row['jps_perc_virtual']."%=".getInDollars($row["virtual_rake"]/100*$row['jps_perc_virtual'])?></span></td>
                        <?if($financial)
                        {?>
                            <td class="content dark" style="text-align:right"><?=getInDollars($row["virtual_fin"])?><br />
                                <span class="tip"><?=$row['jps_perc_virtual']."%=".getInDollars($row["virtual_fin"]/100*$row['jps_perc_virtual'])?></span></td>
                        <?php } } ?>
                    <?php
                    if($bankroulette==1){
                        if($betwin) {?>
                            <td class="content dark" style="text-align:right"><?=getInDollars($row["casino_bankroulette_bet"])?><br />
                                <span class="tip"><?=$row['jps_perc_bankroulette']."%=".getInDollars($row["casino_bankroulette_bet"]/100*$row['jps_perc_bankroulette'])?></span></td>
                            <td class="content dark" style="text-align:right"><?=getInDollars($row["casino_bankroulette_win"])?><br />
                                <span class="tip"><?=$row['jps_perc_bankroulette']."%=".getInDollars($row["casino_bankroulette_win"]/100*$row['jps_perc_bankroulette'])?></span></td>
                        <?php }?>
                        <td class="content dark" style="text-align:right"><?=getInDollars($row["casino_bankroulette_rake"])?><br />
                            <span class="tip"><?=$row['jps_perc_bankroulette']."%=".getInDollars($row["casino_bankroulette_rake"]/100*$row['jps_perc_bankroulette'])?></span></td>
                        <td class="content dark" style="text-align:right"><?=getInDollars($row["casino_bankroulette_fin"])?><br />
                            <span class="tip"><?=$row['jps_perc_bankroulette']."%=".getInDollars($row["casino_bankroulette_fin"]/100*$row['jps_perc_bankroulette'])?></span></td>
                        <?php } ?>
                    <?php
                    if($zecchinetta==1){
                        if($betwin) {?>
                            <td class="content dark" style="text-align:right"><?=getInDollars($row["casino_zecchinetta_bet"])?><br />
                                <span class="tip"><?=$row['jps_perc_zecchinetta']."%=".getInDollars($row["casino_zecchinetta_bet"]/100*$row['jps_perc_zecchinetta'])?></span></td>
                            <td class="content dark" style="text-align:right"><?=getInDollars($row["casino_zecchinetta_win"])?><br />
                                <span class="tip"><?=$row['jps_perc_zecchinetta']."%=".getInDollars($row["casino_zecchinetta_win"]/100*$row['jps_perc_zecchinetta'])?></span></td>
                        <?php }?>
                        <td class="content dark" style="text-align:right"><?=getInDollars($row["casino_zecchinetta_rake"])?><br />
                            <span class="tip"><?=$row['jps_perc_zecchinetta']."%=".getInDollars($row["casino_zecchinetta_rake"]/100*$row['jps_perc_zecchinetta'])?></span></td>
                        <td class="content dark" style="text-align:right"><?=getInDollars($row["casino_zecchinetta_fin"])?><br />
                            <span class="tip"><?=$row['jps_perc_zecchinetta']."%=".getInDollars($row["casino_zecchinetta_fin"]/100*$row['jps_perc_zecchinetta'])?></span></td>
                    <?php } ?>
                    <?php
                    if($greek==1) {
                        if($betwin) {?>
                            <td class="content dark" style="text-align:right"><?=getInDollars($row["casino_greek_bet"])?><br />
                                <span class="tip"><?=$row['jps_perc_greek']."%=".getInDollars($row["casino_greek_bet"]/100*$row['jps_perc_greek'])?></span></td>
                            <td class="content dark" style="text-align:right"><?=getInDollars($row["casino_greek_win"])?><br />
                                <span class="tip"><?=$row['jps_perc_greek']."%=".getInDollars($row["casino_greek_win"]/100*$row['jps_perc_greek'])?></span></td>
                        <?php }?>
                        <td class="content dark" style="text-align:right"><?=getInDollars($row["casino_greek_rake"])?><br />
                            <span class="tip"><?=$row['jps_perc_greek']."%=".getInDollars($row["casino_greek_rake"]/100*$row['jps_perc_greek'])?></span></td>
                    <?php } ?>
                    <?php
                    if($zeus==1) {
                        if($betwin) {?>
                            <td class="content dark" style="text-align:right"><?=getInDollars($row["zeus_bet"])?><br />
                                <span class="tip"><?=$row['jps_perc_zeus']."%=".getInDollars($row["zeus_bet"]/100*$row['jps_perc_zeus'])?></span></td>
                            <td class="content dark" style="text-align:right"><?=getInDollars($row["zeus_win"])?><br />
                                <span class="tip"><?=$row['jps_perc_zeus']."%=".getInDollars($row["zeus_win"]/100*$row['jps_perc_zeus'])?></span></td>
                        <?php }?>
                        <td class="content dark" style="text-align:right"><?=getInDollars($row["zeus_rake"])?><br />
                            <span class="tip"><?=$row['jps_perc_zeus']."%=".getInDollars($row["zeus_rake"]/100*$row['jps_perc_zeus'])?></span></td>
                    <?php } ?>
                    <?php
                    if($royalpk==1){
                        if($betwin)
                        {?>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["royalpk_bet"])?><br />
                                <span class="tip"> <?=$row['jps_perc_royalpk']."%=".getInDollars($row["royalpk_bet"]/100*$row['jps_perc_royalpk'])?></span></td>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["royalpk_win"])?><br />
                                <span class="tip"><?=$row['jps_perc_royalpk']."%=".getInDollars($row["royalpk_win"]/100*$row['jps_perc_royalpk'])?></span></td>
                        <?php }?>
                        <td class="content light" style="text-align:right"><?=getInDollars($row["royalpk_rake"])?><br />
                            <span class="tip"><?=$row['jps_perc_royalpk']."%=".getInDollars($row["royalpk_rake"]/100*$row['jps_perc_royalpk'])?></span></td>
                        <?if($financial)
                        {?>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["royalpk_fin"])?><br />
                                <span class="tip"><?=$row['jps_perc_royalpk']."%=".getInDollars($row["royalpk_fin"]/100*$row['jps_perc_royalpk'])?></span></td>
                        <?php } } ?>

                    <?php
                    if($playtech==1){
                        if($betwin)
                        {?>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["playtech_bet"])?><br />
                                <span class="tip"> <?=$row['jps_perc_playtech']."%=".getInDollars($row["playtech_bet"]/100*$row['jps_perc_playtech'])?></span></td>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["playtech_win"])?><br />
                                <span class="tip"><?=$row['jps_perc_playtech']."%=".getInDollars($row["playtech_win"]/100*$row['jps_perc_playtech'])?></span></td>
                        <?php }?>
                        <td class="content light" style="text-align:right"><?=getInDollars($row["playtech_rake"])?><br />
                            <span class="tip"><?=$row['jps_perc_playtech']."%=".getInDollars($row["playtech_rake"]/100*$row['jps_perc_playtech'])?></span></td>
                        <?if($financial)
                        {?>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["playtech_fin"])?><br />
                                <span class="tip"><?=$row['jps_perc_playtech']."%=".getInDollars($row["playtech_fin"]/100*$row['jps_perc_playtech'])?></span></td>
                        <?php } } ?>

                    <?php
                    if($evolution==1){
                        if($betwin)
                        {?>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["evolution_bet"])?><br />
                                <span class="tip"> <?=$row['jps_perc_evolution']."%=".getInDollars($row["evolution_bet"]/100*$row['jps_perc_evolution'])?></span></td>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["evolution_win"])?><br />
                                <span class="tip"><?=$row['jps_perc_evolution']."%=".getInDollars($row["evolution_win"]/100*$row['jps_perc_evolution'])?></span></td>
                        <?php }?>
                        <td class="content light" style="text-align:right"><?=getInDollars($row["evolution_rake"])?><br />
                            <span class="tip"><?=$row['jps_perc_evolution']."%=".getInDollars($row["evolution_rake"]/100*$row['jps_perc_evolution'])?></span></td>
                        <?if($financial)
                        {?>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["evolution_fin"])?><br />
                                <span class="tip"><?=$row['jps_perc_evolution']."%=".getInDollars($row["evolution_fin"]/100*$row['jps_perc_evolution'])?></span></td>
                        <?php } } ?>

                    <?php
                    if($infinity==1){
                        if($betwin)
                        {?>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["infinity_bet"])?><br />
                                <span class="tip"> <?=$row['jps_perc_infinity']."%=".getInDollars($row["infinity_bet"]/100*$row['jps_perc_infinity'])?></span></td>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["infinity_win"])?><br />
                                <span class="tip"><?=$row['jps_perc_infinity']."%=".getInDollars($row["infinity_win"]/100*$row['jps_perc_infinity'])?></span></td>
                        <?php }?>
                        <td class="content light" style="text-align:right"><?=getInDollars($row["infinity_rake"])?><br />
                            <span class="tip"><?=$row['jps_perc_infinity']."%=".getInDollars($row["infinity_rake"]/100*$row['jps_perc_infinity'])?></span></td>
                        <?if($financial)
                        {?>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["infinity_fin"])?><br />
                                <span class="tip"><?=$row['jps_perc_infinity']."%=".getInDollars($row["infinity_fin"]/100*$row['jps_perc_infinity'])?></span></td>
                        <?php } } ?>

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
                        

                    <?php
                    if($forex==1){
                        if($betwin)
                        {?>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["forex_bet"])?><br />
                                <span class="tip"> <?=$row['jps_perc_forex']."%=".getInDollars($row["forex_bet"]/100*$row['jps_perc_forex'])?></span></td>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["forex_win"])?><br />
                                <span class="tip"><?=$row['jps_perc_forex']."%=".getInDollars($row["forex_win"]/100*$row['jps_perc_forex'])?></span></td>
                        <?php }?>
                        <td class="content light" style="text-align:right"><?=getInDollars($row["forex_rake"])?><br />
                            <span class="tip"><?=$row['jps_perc_forex']."%=".getInDollars($row["forex_rake"]/100*$row['jps_perc_forex'])?></span></td>
                        <?if($financial)
                        {?>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["forex_fin"])?><br />
                                <span class="tip"><?=$row['jps_perc_forex']."%=".getInDollars($row["forex_fin"]/100*$row['jps_perc_forex'])?></span></td>
                        <?php } } ?>

                    <?php
                    if($totem==1){
                        if($betwin)
                        {?>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["totem_bet"])?><br />
                                <span class="tip"><?=$row['jps_perc_totem']."%=".getInDollars($row["totem_bet"]/100*$row['jps_perc_totem'])?></span></td>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["totem_win"])?><br />
                                <span class="tip"><?=$row['jps_perc_totem']."%=".getInDollars($row["totem_win"]/100*$row['jps_perc_totem'])?></span></td>
                        <?php }?>
                        <td class="content light" style="text-align:right"><?=getInDollars($row["totem_rake"])?><br />
                            <span class="tip"><?=$row['jps_perc_totem']."%=".getInDollars($row["totem_rake"]/100*$row['jps_perc_totem'])?></span></td>
                        <?if($financial)
                        {?>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["totem_fin"])?><br />
                                <span class="tip"><?=$row['jps_perc_totem']."%=".getInDollars($row["totem_fin"]/100*$row['jps_perc_totem'])?></span></td>
                        <?php } } ?>

                    <?php
                    if($pokerl==1){
                        if($betwin)
                        {?>
                            <td class="content dark" style="text-align:right"><?=getInDollars($row["poker_live_bet"]+$row["poker_live_tournament_bet"])?><br />
                                <span class="tip">  <?=$row['jps_perc_poker_live']."%=".getInDollars(($row["poker_live_bet"]+$row["poker_live_tournament_bet"])/100*$row['jps_perc_poker_live'])?></span></td>
                            <td class="content dark" style="text-align:right"><?=getInDollars($row["poker_live_win"]+$row["poker_live_tournament_win"])?><br />
                                <span class="tip"><?=$row['jps_perc_poker_live']."%=".getInDollars(($row["poker_live_win"]+$row["poker_live_tournament_win"])/100*$row['jps_perc_poker_live'])?></span></td>
                        <?php }?>
                        <td class="content dark" style="text-align:right"><?=getInDollars($row["poker_live_rake"]+$row["poker_live_tournament_rake"])?><br />
                            <span class="tip"><?=$row['jps_perc_poker_live']."%=".getInDollars(($row["poker_live_rake"]+$row["poker_live_tournament_rake"])/100*$row['jps_perc_poker_live'])?></span></td>
                        <?if($financial)
                        {?>
                            <td class="content dark" style="text-align:right"><?=getInDollars($row["poker_fin"])?><br />
                                <span class="tip"><?=$row['jps_perc_poker_live']."%=".getInDollars($row["poker_fin"]/100*$row['jps_perc_poker_live'])?></span></td>
                        <?php } } ?>

                    <?php
                    if($poker2d==1){
                        if($betwin)
                        {?>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["poker_2d_bet"]+$row["poker_2d_tournament_bet"])?><br />
                                <span class="tip"> <?=$row['jps_perc_poker_2d']."%=".getInDollars(($row["poker_2d_bet"]+$row["poker_2d_tournament_bet"])/100*$row['jps_perc_poker_2d'])?></span></td>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["poker_2d_win"]+$row["poker_2d_tournament_win"])?><br />
                                <span class="tip"><?=$row['jps_perc_poker_2d']."%=".getInDollars(($row["poker_2d_win"]+$row["poker_2d_tournament_win"])/100*$row['jps_perc_poker_2d'])?></span></td>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["poker_2d_net"]+$row["poker_2d_tournament_net"])?><br />
                                <span class="tip"><?=$row['jps_perc_poker_2d']."%=".getInDollars(($row["poker_2d_net"]+$row["poker_2d_tournament_net"])/100*$row['jps_perc_poker_2d'])?></span></td>
                        <?php }?>
                        <td class="content light" style="text-align:right"><?=getInDollars($row["poker_2d_rake"]+$row["poker_2d_tournament_rake"])?><br />
                            <span class="tip"><?=$row['jps_perc_poker_2d']."%=".getInDollars(($row["poker_2d_rake"]+$row["poker_2d_tournament_rake"])/100*$row['jps_perc_poker_2d'])?></span></td>
                        <?if($financial)
                        {?>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["poker_2d_fin"])?><br />
                                <span class="tip">    <?=$row['jps_perc_poker_2d']."%=".getInDollars($row["poker_2d_fin"]/100*$row['jps_perc_poker_2d'])?></span></td>
                        <?php } }?>

                    <?php
                    if($betting==1){
                        if($betwin)
                        {?>
                            <td class="content dark" style="text-align:right"><?=getInDollars($row["betting_bet"])?><br />
                                <span class="tip"><?=$row['jps_perc_betting']."%=".getInDollars($row["betting_bet"]/100*$row['jps_perc_betting'])?></span></td>
                            <td class="content dark" style="text-align:right"><?=getInDollars($row["betting_win"])?><br />
                                <span class="tip">  <?=$row['jps_perc_betting']."%=".getInDollars($row["betting_win"]/100*$row['jps_perc_betting'])?></span></td>
                        <?php }?>
                        <td class="content dark" style="text-align:right"><?=getInDollars($row["betting_rake"])?><br />
                            <span class="tip"><?=$row['jps_perc_betting']."%=".getInDollars($row["betting_rake"]/100*$row['jps_perc_betting'])?></span></td>
                        <?if($financial)
                        {?>
                            <td class="content dark" style="text-align:right"><?=getInDollars($row["betting_fin"])?><br />
                                <span class="tip">  <?=$row['jps_perc_betting']."%=".getInDollars($row["betting_fin"]/100*$row['jps_perc_betting'])?></span></td>
                        <?php }?>

                        <?php
                        if($betwin)
                        {?>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["betting_bet_live"])?><br />
                                <span class="tip"><?=$row['jps_perc_betting_live']."%=".getInDollars($row["betting_bet_live"]/100*$row['jps_perc_betting_live'])?></span></td>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["betting_win_live"])?><br />
                                <span class="tip"><?=$row['jps_perc_betting_live']."%=".getInDollars($row["betting_win_live"]/100*$row['jps_perc_betting_live'])?></span></td>
                        <?php }?>
                        <td class="content light" style="text-align:right"><?=getInDollars($row["betting_rake_live"])?><br />
                            <span class="tip"><?=$row['jps_perc_betting_live']."%=".getInDollars($row["betting_rake_live"]/100*$row['jps_perc_betting_live'])?></span></td>
                        <?if($financial)
                        {?>
                            <td class="content light" style="text-align:right"><?=getInDollars($row["betting_fin_live"])?><br />
                                <span class="tip"><?=$row['jps_perc_betting_live']."%=".getInDollars($row["betting_fin_live"]/100*$row['jps_perc_betting_live'])?></span></td>
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
 * @param $class
 * @param int $returnType
 * @return float|string
 */
function calculateBettingRake($rec ,$class, $returnType = 0,$includeChild=0) {
    global $bettingProfilesTypes;
    $netbet=(($rec["betting_bet"]-$rec['betting_win'])-($rec['betting_rake'])-calculateBonus($rec));
    $totalnetbet=(($rec["betting_bet"]-$rec['betting_win'])-($rec['betting_rake'])+($rec["betting_bet_live"]-$rec['betting_win_live'])-($rec['betting_rake_live'])-calculateBonus($rec));
    $totalbet=$rec["betting_bet"];
    $nationPerc    = getBettingPercentage($rec["jps_perc_betting_n"]);
    $casinoPerc    = 100-$nationPerc;
    $regionPerc    = getBettingPercentage($rec["jps_perc_betting_r"]);
    $districtPerc  = getBettingPercentage($rec["jps_perc_betting_d"]);

    $casinoProfileType    = $nationProfileType    = getBettingPercentage($rec["jps_perc_betting_n"],"numeric");
    $regionProfileType    = getBettingPercentage($rec["jps_perc_betting_r"],"numeric");
    $districtProfileType  = getBettingPercentage($rec["jps_perc_betting_d"],"numeric");

    if($class=='casino'){
        $perctoret = $casinoPerc." % = ";
        if($includeChild==0){
            if($totalnetbet<0 && $nationProfileType==3){
                $nationPerc=0;
            }
            if($totalnetbet<0 && $regionProfileType==3){
                $regionPerc=0;
            }
            if($totalnetbet<0 && $districtProfileType==3){
                $districtPerc=0;
            }
        }

        eval('$percvalue='.$bettingProfilesTypes[$casinoProfileType]['formulas']['casino'].";");
    }
    elseif($class=='nation'){
        $perctoret = $nationPerc."% = ";

        if($includeChild==0){
            if($totalnetbet<0 && $regionProfileType==3){
                $regionPerc=0;
            }
            if($totalnetbet<0 && $districtProfileType==3){
                $districtPerc=0;
            }
        }
        eval('$percvalue='.$bettingProfilesTypes[$nationProfileType]['formulas']['nation'].";");
    }
    elseif($class=='region'){
        $perctoret = $regionPerc."% = ";
        if($includeChild==0){
            if($totalnetbet<0 && $districtProfileType==3){
                $districtPerc=0;
            }
        }
        eval('$percvalue='.$bettingProfilesTypes[$regionProfileType]['formulas']['region'].";");
    }
    elseif($class=='district'){
        $perctoret = $districtPerc."% = ";
        eval('$percvalue='.$bettingProfilesTypes[$districtProfileType]['formulas']['district'].";");
    }
    elseif($class=='club'){
        $perctoret = "%=";
        $percvalue = 0;
    }
    $percvalue+=calculateLiveBettingRake($rec ,$class, 1);
    if($returnType == 0){
        return $perctoret.getInDollars($percvalue);
    }
    if($returnType == 1){
        return $percvalue;
    }
    if($returnType == 2){
        return $perctoret.($percvalue);
    }


}

/**
 * @param $rec
 * @param int $returnType
 * @return float|string
 */
function calculateLiveBettingRake($rec ,$class, $returnType = 0,$includeChild=0){
    global $bettingProfilesTypes;
    $netbet=($rec["betting_bet_live"]-$rec['betting_win_live'])-($rec['betting_rake_live']);
    $totalnetbet=(($rec["betting_bet"]-$rec['betting_win'])-($rec['betting_rake'])+($rec["betting_bet_live"]-$rec['betting_win_live'])-($rec['betting_rake_live'])-calculateBonus($rec));
    $nationPerc    = getBettingPercentage($rec["jps_perc_betting_live_n"]);

    $casinoPerc    = 100-$nationPerc;
    $regionPerc    = getBettingPercentage($rec["jps_perc_betting_live_r"]);
    $districtPerc  = getBettingPercentage($rec["jps_perc_betting_live_d"]);

    $casinoProfileType    = $nationProfileType    = getBettingPercentage($rec["jps_perc_betting_live_n"],"numeric");
    $regionProfileType    = getBettingPercentage($rec["jps_perc_betting_live_r"],"numeric");
    $districtProfileType  = getBettingPercentage($rec["jps_perc_betting_live_d"],"numeric");

    if($class=='casino'){
        $perctoret = $casinoPerc." % = ";
        if($includeChild==0){
            if($totalnetbet<0 && $nationProfileType==7){
                $nationPerc=0;
            }
            if($totalnetbet<0 && $regionProfileType==7){
                $regionPerc=0;
            }
            if($totalnetbet<0 && $districtProfileType==7){
                $districtPerc=0;
            }
        }
        eval('$percvalue='.$bettingProfilesTypes[$casinoProfileType]['formulas']['casino'].";");
    }
    elseif($class=='nation'){
        $perctoret = $nationPerc."% = ";
        if($includeChild==0){
            if($totalnetbet<0 && $regionProfileType==7){
                $regionPerc=0;
            }
            if($totalnetbet<0 && $districtProfileType==7){
                $districtPerc=0;
            }
        }

        eval('$percvalue='.$bettingProfilesTypes[$nationProfileType]['formulas']['nation'].";");
    }
    elseif($class=='region'){
        $perctoret = $regionPerc."% = ";
        if($includeChild==0){
            if($totalnetbet<0 && $districtProfileType==7){
                $districtPerc=0;
            }
        }
        eval('$percvalue='.$bettingProfilesTypes[$regionProfileType]['formulas']['region'].";");
    }
    elseif($class=='district'){
        $perctoret = $districtPerc."% = ";
        eval('$percvalue='.$bettingProfilesTypes[$districtProfileType]['formulas']['district'].";");
    }
    elseif($class=='club'){
        $perctoret = "%=";
        $percvalue = 0;
    }
    if($returnType == 0){
        return $perctoret.getInDollars($percvalue);
    }
    if($returnType == 1){
        return $percvalue;
    }
    if($returnType == 2){
        return $perctoret.$percvalue;
    }
}

/**
 * @param $percentage
 * @param bool $onlytype
 * @return mixed
 */
function getBettingPercentage($percentage,$onlytype=false){
    global $bettingProfilesTypes;
    list($type,$proc)=explode(";",$percentage);
    list($typename,$proctype)=explode(":",$type);
    list($name,$procent)=explode(":",$proc);
    if($onlytype){
        if($onlytype=='type'){
            /* if($proctype=='11'){
                 return '';
             }*/
            return $bettingProfilesTypes[$proctype]['name'];
        }
        return $proctype;
    }
    else{
        if($proctype==4 || $proctype==8){
            $procent=explode(',',$procent);
            $procent=$procent[0];
        }

        return $procent;
    }
}

/**
 * @param $rec
 * @return string
 */
function calculatePerc($rec){
    global $casino,$casino_live,$virtual,$pokerl,$poker2d,$royalpk,$playtech,$evolution,$infinity,$portomaso,$totem,$forex,$bankroulette,$zecchinetta,$greek,$zeus;
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
    if($zeus==1){
        $ret +=$rec["zeus_rake"];
    }
    return $ret;
}
function calculateFin($rec){
    global $casino,$casino_live,$virtual,$pokerl,$poker2d,$royalpk,$playtech,$evolution,$infinity,$portomaso,$totem,$forex,$bankroulette,$zecchinetta,$greek,$zeus;
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
    if($zeus==1){
        $ret +=$rec["zeus_fin"];
    }
    return $ret;
}

function calculatePercBetting($rec){
    global $betting,$bettingl;
    $ret='';
    if($betting==1){
        $ret +=($rec["betting_bet"]-$rec['betting_win']);
    }
    if($bettingl==1){
        $ret +=($rec["betting_bet_live"]-$rec['betting_win_live']);
    }
    return $ret;
}



/**
 * @param $rec
 * @param int $type
 * @return float|string
 */
function calculateCurrentSubPerc($rec){
    global $casino,$casino_live,$virtual,$pokerl,$poker2d,$royalpk,$playtech,$evolution,$infinity,$portomaso,$totem,$forex,$bankroulette,$zecchinetta,$greek,$zeus;
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
    if($zeus==1){
        $ret +=$rec["zeus_rake"]/100* ($rec["jps_perc_zeus"]);
    }
    return $ret;
}

function calculateCurrentSubPercBetting($rec){
    global $betting,$bettingl;
    $ret='';
    if($betting==1){
        $ret+=calculateRowBetting($rec);
    }
    if($bettingl==1){
        $ret+=calculateRowBettingLive($rec);
    }
    return $ret;
}

/**
 * @param $rec
 * @return float|string
 */
function calculateRowBetting($rec){
    global $jur_groups_level;
    $ret='';
    if($jur_groups_level=='nation'){
        $ret+=calculateBettingRake($rec,'nation',1)+$rec['betting_rake'];
    }
    elseif($jur_groups_level=='region'){
        $ret+=calculateBettingRake($rec,'region',1)+$rec['betting_rake'];
    }
    elseif($jur_groups_level=='district'){
        $ret+=calculateBettingRake($rec,'district',1)+$rec['betting_rake'];
    }
    elseif($jur_groups_level=='club'){
        $ret+=$rec['betting_rake'];
    }
    return $ret;
}

/**
 * @param $rec
 * @return float|string
 */
function calculateRowBettingLive($rec){
    global $jur_groups_level;
    $ret='';
    if($jur_groups_level=='nation'){
        $ret+=calculateLiveBettingRake($rec,'nation',1)+$rec['betting_rake_live'];
    }
    elseif($jur_groups_level=='region'){
        $ret+=calculateLiveBettingRake($rec,'region',1)+$rec['betting_rake_live'];
    }
    elseif($jur_groups_level=='district'){
        $ret+=calculateLiveBettingRake($rec,'district',1)+$rec['betting_rake_live'];
    }
    elseif($jur_groups_level=='club'){
        $ret+=$rec['betting_rake_live'];
    }
    return $ret;
}

function calculateBettingRakeNet($rec ,$class, $returnType = 0){
    if($class=='casino'){
        return calculateBettingRake($rec,'casino',$returnType);
    }
    elseif($class=='nation'){
        return calculateBettingRake($rec,'nation',$returnType)-calculateBettingRake($rec,'region',$returnType);
    }
    elseif($class=='region'){
        return calculateBettingRake($rec,'region',$returnType)-calculateBettingRake($rec,'district',$returnType);
    }
    elseif($class=='district'){
        calculateBettingRake($rec,'district',$returnType);
    }
    elseif($class=='club'){
        calculateBettingRake($rec,'club',$returnType);
    }
}

function isValidBettingPeriod($start,$end){
    $month=date('Y-m',strtotime($start));
    if(date('D',strtotime($month))=='Tue'  && $month.'-01'==$start){
        $lastTuesday = $month.'-01';
        $lastDay = date('Y-m-d',strtotime("Last Monday of $month"));
    }
    else {
        $nextMonth=date('Y-m',strtotime($month.' +1 month'));
        $lastTuesday = date('Y-m-d',strtotime("Last Tuesday of $month"));
        $lastDay = date('Y-m-d',strtotime("Last Monday of $nextMonth"));
    }

    if($lastTuesday==$start && $lastDay==$end){

        return true;
    }
    return false;
}

function calculateBonus($rec){
    global $periodType,$date_start,$date_end;
    if($periodType==0 && !isValidBettingPeriod($date_start,$date_end)){
        return 0;
    }
    $rake=$rec["betting_rake"]+$rec["betting_rake_live"];
    $net=$rec["betting_bet"]+$rec["betting_bet_live"]- $rec["betting_win"]-$rec["betting_win_live"];
    if(getBettingPercentage($rec['jps_perc_betting_c'],"numeric")==4){
        $proc=getBettingPercentage($rec['jps_perc_betting_c']);
        if(($proc*$net/100)<$rake){
            $proc=0;
        }
        else{
            $proc=($proc*$net/100)-$rake;
        }
    }
    else{
        $proc=0;
    }
    return $proc;
}

function calculateRowNet($rec,$class){
    global $casino,$casino_live,$virtual,$pokerl,$poker2d,$royalpk,$playtech,$evolution,$infinity,$portomaso,$totem,$forex,$bankroulette,$zecchinetta,$greek,$zeus;
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
    if($zeus==1){
        
        if($class=='casino'){
            $ret +=$rec["zeus_rake"]/100*(100-$rec['jps_perc_zeus_n']);
        }
        if($class=='nation'){
            $ret +=$rec["zeus_rake"]/100*($rec['jps_perc_zeus_n']-$rec['jps_perc_zeus_r']);
        }
        if($class=='region'){
            $ret +=$rec["zeus_rake"]/100*($rec['jps_perc_zeus_r']-$rec['jps_perc_zeus_d']);
        }
        if($class=='district'){
            $ret +=$rec["zeus_rake"]/100*($rec['jps_perc_zeus_d']-$rec['jps_perc_zeus_c']);
        }
    }
    return $ret;
}

function changeBettingType($perc,$newType){
    if($perc!=''){
        $bettingType=explode(";",$perc);
        $type=explode(':',$bettingType[0]);
        $type[1]=$newType;
        $type=implode(":",$type);
        $bettingType[0]=$type;
        $bettingType=implode(';',$bettingType);
    }
    else{
        $bettingType=0;
    }
    return $bettingType;
}

function calculateTotalNet($rec){
    if($_SESSION['jurisdiction_class']=='casino'){
        return calculateBettingRake($rec,"casino",1);
    }
    if($_SESSION['jurisdiction_class']=='nation'){
        return calculateBettingRake($rec,"casino",1)+calculateBettingRake($rec,"nation",1);
    }
    if($_SESSION['jurisdiction_class']=='region'){
        return calculateBettingRake($rec,"casino",1)+calculateBettingRake($rec,"region",1);
    }
    if($_SESSION['jurisdiction_class']=='district'){
        return calculateBettingRake($rec,"casino",1)+calculateBettingRake($rec,"nation",1)+calculateBettingRake($rec,"region",1)+calculateBettingRake($rec,"district",1);;
    }
    if($_SESSION['jurisdiction_class']=='club'){
        return $rec["betting_rake"]+$rec["betting_rake_live"]+calculateBonus($rec);
    }


}

function returnBettingFinancialDataJson($date_start, $date_end,$jur_groups_level, $betwin, $financial,$jurisdiction_level,$jur_sel_id){
    global $betting;
    $betting_jurisdictions=array('casino_profit','nation_profit','region_profit','district_profit');
    $betting=1;
    $val_fmt_arr = array (
        'prematch_bet'             => 'return $rec["betting_bet"];',
        'live_bet'                 => 'return $rec["betting_bet_live"];',
        'prematch_win'             => 'return $rec["betting_win"];',
        'live_win'                 => 'return $rec["betting_win_live"];',
        'total_bet'                => 'return ($rec["betting_bet"]+$rec["betting_bet_live"]);',
        'total_win'                => 'return ($rec["betting_win"]+$rec["betting_win_live"]);',
        'utile'                    => 'return ($rec["betting_bet"]+$rec["betting_bet_live"]- $rec["betting_win"]-$rec["betting_win_live"]);',
        'club_rake'                => 'return getBettingPercentage($rec["jps_perc_betting_c"],"type").";".getBettingPercentage($rec["jps_perc_betting_c"]).":".($rec["betting_rake"] + $rec["betting_rake_live"]);',
        'club_bonus'               => 'return calculateBonus($rec);',
        'club_tot_profit'          => 'return ($rec["betting_rake"]+$rec["betting_rake_live"]+calculateBonus($rec));',
        'netto_bet'                => 'return ($rec["betting_bet"]+$rec["betting_bet_live"]- $rec["betting_win"]-$rec["betting_win_live"]-$rec["betting_rake"]-$rec["betting_rake_live"]-calculateBonus($rec));',
        'profile'                  => 'return ($rec["betting_rake"]);',
        'casino_profit'            => 'return (getBettingPercentage($rec["jps_perc_betting_n"],"type")=="FeedTax" ? $rec["betting_bet"] : ($rec["betting_bet"]+$rec["betting_bet_live"]-$rec["betting_win"]-$rec["betting_win_live"]-$rec["betting_rake"]-$rec["betting_rake_live"]-calculateBonus($rec)))
                                                     .";100:".(calculateBettingRake($rec,"casino",1));',
        'nation_profit'            => 'return (getBettingPercentage($rec["jps_perc_betting_n"],"type")=="FeedTax" ? (calculateBettingRake($rec,"nation",1)) : getBettingPercentage($rec["jps_perc_betting_n"],"type").";".getBettingPercentage($rec["jps_perc_betting_n"]).":"
                                                     .($_SESSION["jurisdiction_class"]=="nation" ? (calculateBettingRake($rec,"nation",1)+calculateBettingRake($rec,"region",1)+calculateBettingRake($rec,"district",1)).";" : "")
                                                     .(calculateBettingRake($rec,"nation",1)));',
        'region_profit'            => 'return getBettingPercentage($rec["jps_perc_betting_r"],"type").";".getBettingPercentage($rec["jps_perc_betting_r"]).":"
                                                     .($_SESSION["jurisdiction_class"]=="region" ? (calculateBettingRake($rec,"region",1)+calculateBettingRake($rec,"district",1)).";" : "")
                                                     .(calculateBettingRake($rec,"region",1));',
        'district_profit'          => 'return getBettingPercentage($rec["jps_perc_betting_d"],"type").";".getBettingPercentage($rec["jps_perc_betting_d"]).":".(calculateBettingRake($rec,"district",1));',
        'total_net'                => 'return (calculateTotalNet($rec));',
        'prematch_fin'             => 'return $rec["betting_fin"];',
        'prematch_net'             => 'return ($rec["betting_bet"]-$rec["betting_win"]);',
        'live_fin'                 => 'return ($rec["betting_fin_live"]);',
        'live_net'                 => 'return ($rec["betting_bet_live"]-$rec["betting_win_live"]);',
    );




    $cols_arr = array();

    if($betwin) {
        array_push($cols_arr, 'prematch_bet');
        array_push($cols_arr, 'prematch_win');
    }
    array_push($cols_arr,'prematch_net');
    if($financial){
        array_push($cols_arr,'prematch_fin');
    }
    if($betwin) {
        array_push($cols_arr, 'live_bet');
        array_push($cols_arr, 'live_win');
    }
    array_push($cols_arr,'live_net');
    if($financial){

        array_push($cols_arr,'live_fin');
    }
    array_push($cols_arr,'total_bet');
    array_push($cols_arr,'total_win');
    array_push($cols_arr,'utile');
    array_push($cols_arr,'club_rake');
    array_push($cols_arr,'club_bonus');
    array_push($cols_arr,'club_tot_profit');
    array_push($cols_arr,'netto_bet');
    $cols_arr=array_merge($cols_arr,$betting_jurisdictions);
    array_push($cols_arr,'total_net');




    $recordset= getBettingFinancialData($date_start, $date_end,$jur_groups_level, $betwin, $financial,$jurisdiction_level,$jur_sel_id);

    $recordset->resetRecPtr();
    while ( $recordset->hasNext() )
    {

        $rec = $recordset->next();
        if(getBettingPercentage($rec["jps_perc_betting_c"],"number")=="1" || getBettingPercentage($rec["jps_perc_betting_c"],"number")=="3" ){
            $rec['betting_rake']= $recordset->Records[$recordset->CurrentIndex]['betting_rake']=($rec["betting_bet"]-$rec["betting_win"])*getBettingPercentage($rec["jps_perc_betting_c"])/100;
            $rec['betting_rake_live']= $recordset->Records[$recordset->CurrentIndex]['betting_rake_live']=($rec["betting_bet_live"]-$rec["betting_win_live"])*getBettingPercentage($rec["jps_perc_betting_live_c"])/100;

        }
        foreach ( $cols_arr as $key => $col_name )
        {
            if ( $val_fmt_arr[$col_name] ){
                $function = create_function('$rec', $val_fmt_arr[$col_name]);
                // echo $col_name ." => ".$function($rec)." <br />";
                $recordset->Records[$recordset->CurrentIndex][$col_name] = $function($rec);
            }
        }
    }
    return $recordset->Records;
}

?>