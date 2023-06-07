<?php
/**
 * Created by CSLIVEGAMES.
 * User: Manuel
 * Date: 07/06/13
 * Time: 11.23
 * File: ${FILE_NAME}
 */
/*
 *
 * SELECT res_id, res_date, res_string, res_dealer_id, pre_pun_id, pre_game_num, pre_bet, pre_win, pre_bet_string, pre_bets_paid, pre_pun_ip
FROM result, punter_result
WHERE res_id = pre_res_id
AND res_date
BETWEEN  '2013-06-07 00:00:00'
AND  '2013-06-07 23:59:00'
AND res_gam_id =6002
ORDER BY res_id DESC
LIMIT 0 , 30

 */
//1@p1=3022~300|2045~150|2125~150|2185~150|2195~150|2325~150|4254~300|1019~100|2065~50|2145~50|2175~50|2315~50|1059~100|1249~100|1169~100|1339~50|1239~50|1109~50|1029~50|1259~50|1179~50|1349~50|1069~50@2065;0;50;5450~1339;1;50;5400~1019;2;100;5350~3022;3;300;5250~1249;4;100;4950~1069;5;50;4850~2175;6;50;4800~2315;7;50;4750~1059;8;100;4700~2195;9;150;4600~1259;10;50;4450~1029;11;50;4400~2045;12;150;4350~2185;13;150;4200~1169;14;100;4050~1239;15;50;3950~2325;16;150;3900~4254;17;300;3750~2145;18;50;3450~1109;19;50;3400~2125;20;150;3350~1349;21;50;3200~1179;22;50;3150
define('ROULETTE_SINGLE', 6002);
define('ROULETTE_DOUBLE', 6001);

$sql = "SELECT DISTINCT *
        FROM  `game_host`
        WHERE hst_string LIKE  '%6002%'
        OR hst_string LIKE  '%6001%'
        LIMIT 0 , 30 "

//Socket Request
$socket = new SocketClient();
$socket->connect('193.242.104.15',40004);
if($socket->isConnected()){
    $socket->write("");
}
?>
<style>
    .number{
        background-color: #006600;
        width: 30px;
        height: 40px;
        text-align: center;
    }
    .zero{
        border-radius: 70px 0px 0px 70px;
        background-color: #006600;
        width: 30px;
        text-align: center;
    }
    .zerodiv{
        border-radius: 5px;
        max-width: 25px;
        max-height: 25px;
        text-align: center;
        margin:auto;
        font-size: 15px;
        font-weight: bold;
        color:white;
        vertical-align: middle;
        background-color: #00bd00;
    }
    .red{
        border-radius: 5px;
        max-width: 25px;
        max-height: 25px;
        text-align: center;
        margin:auto;
        font-size: 15px;
        font-weight: bold;
        color:white;
        vertical-align: middle;
    }
    .black{
        border-radius: 5px;
        max-width: 25px;
        max-height: 25px;
        text-align: center;
        margin:auto;
        font-size: 15px;
        font-weight: bold;
        background-color: #000000;
        color:white;
        vertical-align: middle;
    }
    .rotateText{
        -ms-transform:rotate(270deg); /* IE 9 */
        -moz-transform:rotate(270deg); /* Firefox */
        -webkit-transform:rotate(270deg); /* Safari and Chrome */
        -o-transform:rotate(270deg); /* Opera */
        font-size: 15px;
        font-weight: bold;
        color:white;
        text-align: center;
    }
    .sections{
        text-align: center;
        background-color: #006600;
        height: 30px;
        font-size: 15px;
        font-weight: bold;
        color:white;
    }
</style>
<div class="bodyHD centered">Roulette Monitor</div>
<br/>
<br/>
<br/>
<table style="width:900px;">
    <td style="width:445px;">
        <div class="bodyHD centered">Roulette Single</div>
        <table>
            <tr>
                <td rowspan="3" class="zero"><div class="zerodiv">0</div></td>
                <td class="number"><div class="red">3</div></td>
                <td class="number"><div class="black">6</div></td>
                <td class="number"><div class="red">9</div></td>
                <td class="number"><div class="red">12</div></td>
                <td class="number"><div class="black">15</div></td>
                <td class="number"><div class="red">18</div></td>
                <td class="number"><div class="red">21</div></td>
                <td class="number"><div class="black">24</div></td>
                <td class="number"><div class="red">27</div></td>
                <td class="number"><div class="red">30</div></td>
                <td class="number"><div class="black">33</div></td>
                <td class="number"><div class="red">36</div></td>
                <td class="number"><div class="rotateText">2to1</div></td>
            </tr>
            <tr>
                <td class="number"><div class="black">2</div></td>
                <td class="number"><div class="red">5</div></td>
                <td class="number"><div class="black">8</div></td>
                <td class="number"><div class="black">11</div></td>
                <td class="number"><div class="red">14</div></td>
                <td class="number"><div class="black">17</div></td>
                <td class="number"><div class="black">20</div></td>
                <td class="number"><div class="red">23</div></td>
                <td class="number"><div class="black">26</div></td>
                <td class="number"><div class="black">29</div></td>
                <td class="number"><div class="red">32</div></td>
                <td class="number"><div class="black">35</div></td>
                <td class="number"><div class="rotateText">2to1</div></td>
            </tr>
            <tr>
                <td class="number"><div class="red">1</div></td>
                <td class="number"><div class="black">4</div></td>
                <td class="number"><div class="red">7</div></td>
                <td class="number"><div class="black">10</div></td>
                <td class="number"><div class="black">13</div></td>
                <td class="number"><div class="red">16</div></td>
                <td class="number"><div class="red">19</div></td>
                <td class="number"><div class="black">22</div></td>
                <td class="number"><div class="red">25</div></td>
                <td class="number"><div class="black">28</div></td>
                <td class="number"><div class="black">31</div></td>
                <td class="number"><div class="red">34</div></td>
                <td class="number"><div class="rotateText">2to1</div></td>
            </tr>
            <tr>
                <td></td>
                <td colspan="4" class="sections">1st12</td>
                <td colspan="4" class="sections">2nd12</td>
                <td colspan="4" class="sections">3rd12</td>
            </tr>
            <tr>
                <td></td>
                <td colspan="2" class="sections">1to18</td>
                <td colspan="2" class="sections">Even</td>
                <td colspan="2" class="sections"><div class="black">&nbsp;</div></td>
                <td colspan="2" class="sections"><div class="red">&nbsp;</div></td>
                <td colspan="2" class="sections">Odds</td>
                <td colspan="2" class="sections">19to36</td>
            </tr>
        </table>
        <div class="timer">30 sec</div>
    </td>
    <td style="width:10px;"></td>
    <td style="width:445px;">
        <div class="bodyHD centered">Roulette Double</div>
        <table>
            <tr>
                <td rowspan="3" class="zero"><div class="zerodiv">0</div></td>
                <td class="number"><div class="red">3</div></td>
                <td class="number"><div class="black">6</div></td>
                <td class="number"><div class="red">9</div></td>
                <td class="number"><div class="red">12</div></td>
                <td class="number"><div class="black">15</div></td>
                <td class="number"><div class="red">18</div></td>
                <td class="number"><div class="red">21</div></td>
                <td class="number"><div class="black">24</div></td>
                <td class="number"><div class="red">27</div></td>
                <td class="number"><div class="red">30</div></td>
                <td class="number"><div class="black">33</div></td>
                <td class="number"><div class="red">36</div></td>
                <td class="number"><div class="rotateText">2to1</div></td>
            </tr>
            <tr>
                <td class="number"><div class="black">2</div></td>
                <td class="number"><div class="red">5</div></td>
                <td class="number"><div class="black">8</div></td>
                <td class="number"><div class="black">11</div></td>
                <td class="number"><div class="red">14</div></td>
                <td class="number"><div class="black">17</div></td>
                <td class="number"><div class="black">20</div></td>
                <td class="number"><div class="red">23</div></td>
                <td class="number"><div class="black">26</div></td>
                <td class="number"><div class="black">29</div></td>
                <td class="number"><div class="red">32</div></td>
                <td class="number"><div class="black">35</div></td>
                <td class="number"><div class="rotateText">2to1</div></td>
            </tr>
            <tr>
                <td class="number"><div class="red">1</div></td>
                <td class="number"><div class="black">4</div></td>
                <td class="number"><div class="red">7</div></td>
                <td class="number"><div class="black">10</div></td>
                <td class="number"><div class="black">13</div></td>
                <td class="number"><div class="red">16</div></td>
                <td class="number"><div class="red">19</div></td>
                <td class="number"><div class="black">22</div></td>
                <td class="number"><div class="red">25</div></td>
                <td class="number"><div class="black">28</div></td>
                <td class="number"><div class="black">31</div></td>
                <td class="number"><div class="red">34</div></td>
                <td class="number"><div class="rotateText">2to1</div></td>
            </tr>
            <tr>
                <td></td>
                <td colspan="4" class="sections">1st12</td>
                <td colspan="4" class="sections">2nd12</td>
                <td colspan="4" class="sections">3rd12</td>
            </tr>
            <tr>
                <td></td>
                <td colspan="2" class="sections">1to18</td>
                <td colspan="2" class="sections">Even</td>
                <td colspan="2" class="sections"><div class="black">&nbsp;</div></td>
                <td colspan="2" class="sections"><div class="red">&nbsp;</div></td>
                <td colspan="2" class="sections">Odds</td>
                <td colspan="2" class="sections">19to36</td>
            </tr>
        </table>
    </td>
</table>