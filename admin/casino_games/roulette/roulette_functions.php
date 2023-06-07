<?php
/**
 * Created by CSLIVEGAMES.
 * User: Manuel
 * Date: 07/06/13
 * Time: 11.34
 * File: roulette_functions.php
 */

switch($_GET['action']){

}

function getDailyResults(){
    SELECT res_id, res_date, res_string, res_dealer_id, pre_pun_id, pre_game_num, pre_bet, pre_win, pre_bet_string, pre_bets_paid, pre_pun_ip
FROM result, punter_result
WHERE res_id = pre_res_id
        AND res_date
BETWEEN  '2013-06-07 00:00:00'
        AND  '2013-06-07 23:59:00'
            AND res_gam_id =6002
ORDER BY res_id DESC
LIMIT 0 , 30
}

function getHandsBets(){
    SELECT res_id, res_date, res_string, res_dealer_id, pre_pun_id, pre_game_num, pre_bet, pre_win, pre_bet_string, pre_bets_paid, pre_pun_ip
FROM result, punter_result
WHERE res_id = pre_res_id
        AND res_date
BETWEEN  '2013-06-07 00:00:00'
        AND  '2013-06-07 23:59:00'
        AND res_string is null
            AND res_gam_id =6002
ORDER BY res_id DESC
LIMIT 0 , 30
}

?>