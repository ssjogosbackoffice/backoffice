<script type="text/javascript">
    function openList(){
        location.href = "<?=$_SERVER['PHP_SELF']?>"+"?page=live/recovery&header_page=";
    }
</script>
<?php
check_access('live_recovery', $redirect=true);
require_once 'parsers.inc.php';

$gramblingNumber = array(
    "1st12" => array(1,2,3,4,5,6,7,8,9,10,11,12),
    "2nd12" => array(13,14,15,16,17,18,19,20,21,22,23,24),
    "3nd12" => array(25,26,27,28,29,30,31,32,33,34,35,36),
    "Third Column" => array(3,6,9,12,15,18,21,24,27,30,33,36),
    "Second Column" => array(2,5,8,11,14,17,20,23,26,29,32,35),
    "First Column" => array(1,4,7,10,13,16,19,22,25,28,31,34),
    "ODD" => array(1,3,5,7,9,11,13,15,17,19,21,23,25,27,29,31,33,35),
    "EVEN" => array(2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36),
    "1to18" => array(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18),
    "19to36" => array(19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36),
    "BLACK" => array(2,4,6,8,10,11,13,15,17,20,22,24,26,28,29,31,33,35),
    "RED" => array(1,3,5,7,9,12,14,16,18,19,21,23,25,27,30,32,34,36)
);
$payouts = array(
    "1" => 36,
    "2" => 18,
    "3" => 12,
    "4" => 9,
    "6" => 6,
    "1st12" => 3,
    "2nd12" => 3,
    "3nd12" => 3,
    "Third Column" => 3,
    "Second Column" => 3,
    "First Column" => 3,
    "ODD" => 2,
    "EVEN" => 2,
    "1to18" => 2,
    "19to36" => 2,
    "BLACK" => 2,
    "RED" => 2
);

if(isset($_SESSION['updstr'])){
    $playerString = $_SESSION['updstr'];
    //TODO TRANSACTION FOR UPDATE MONEY
    addResult("", "Tutte le vincite sono state accreditate");
    showResult();
    unset($_SESSION['updstr']);
    ?>
    <input type="button" value="Torna alla lista" onclick="openList();">
    <?php
    return;
}

if(isset($_GET['rid'])){
    $recoveryId = $_GET['rid'];

    $sql = "SELECT rte_recovery_result, rte_res_id FROM recovery_result WHERE rte_id = ". $recoveryId;
    $rs = $dbh->queryRow($sql);

    list($recoveryResult, $color) = split("\|",$rs['rte_recovery_result']);
    $resId = $rs['rte_res_id'];

    $sql = "SELECT pre_pun_id, pun_username , pre_bet, pre_bet_string FROM punter_result JOIN punter on pun_id = pre_pun_id WHERE pre_res_id = $resId AND pre_win = 0 ORDER BY pre_pun_id";
    $rs = $dbh->exec($sql);
    $updateString = "";
    ?>
    <form method="POST" action="<?=$_SERVER['PHP_SELF']?>?page=live/process&rid=<?=$recoveryId?>&header_page=">
        <table border="0" cellspacing="0" cellpadding="0">
            <tr><td>
                    <?php
                    $actualId = 0;
                    $totalBet = 0;
                    $totalWin = 0;
                    while($rs->hasNext()){
                    $row = $rs->next();
                    $username = $row['pun_username'];
                    $punid = $row['pre_pun_id'];
                    if($actualId != $punid){
                    if($actualId != 0){
                    $updateString .= $actualId.";".$totalWin."~";
                    ?>
            <tr><td colspan="2"></td><td><?=getInDollars($totalBet)?></td><td><?=getInDollars($totalWin)?></td></tr>
            <tr><td colspan="4" class="content"></tr>
            <tr><td colspan="4" class="content"></tr>
        </table>
        <?php
        }
        ?>
        <table border="0" cellspacing="1" cellpadding="4">
            <tr><td width="300" colspan="4" class="formheading">Giocatore <?=$username?></td></tr>
            <tr><td width="75" class="label">ID Utente</td><td width="125" class="label">Puntate</td><td width="50" class="label">Puntato</td><td width="50" class="label">Vincita</td></tr>
            <?php
            $actualId = $punid;
            $totalWin = 0;
            $totalBet = 0;
            }
            $betAmount = $row['pre_bet'];
            $code = $row['pre_bet_string'];
            $betString = getBet($code);
            $win = 0;
            if(!strrpos($betString, ",")){
                $numbers = $gramblingNumber[$betString];
                for($i = 0; $i < count($numbers); $i++){
                    $number = $numbers[$i];
                    if($number == $recoveryResult){
                        $win = $win + ($betAmount * $payouts[$betString]);
                    }
                }
            }else{
                $betArray = split(",", $betString);
                $count = count($betArray);
                for($i = 0; $i < $count; $i++){
                    $number = $betArray[$i];
                    if($number == $recoveryResult){
                        $win = $win + ($betAmount * $payouts[$count]);
                    }
                }
            }

            $totalWin = $totalWin + $win;
            $totalBet = $totalBet + $betAmount;
            ?>
            <tr><td class="content"><?=$username?></td><td class="content"><?=$betString?></td><td class="content"><?=getInDollars($betAmount)?></td><td class="content"><?=getInDollars($win)?></td></tr>
            <?php
            }
            $updateString .= $punid.";".$totalWin;
            $_SESSION["updstr"] = $updateString;
            ?>
            <tr><td colspan="2"></td><td><?=getInDollars($totalBet)?></td><td><?=getInDollars($totalWin)?></td></tr>
            <tr><td colspan="4" class="content"></tr>
            <tr><td colspan="4" class="content"></tr>
        </table>
        </td></tr>
        </table>
    </form>
    <?php
    $totalWin = 0;
    $totalBet = 0;
}else{
    header("Location: ".$_SERVER['PHP_SELF']."?page=live/recovery&header_page=");
}
?>