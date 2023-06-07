<?php
ini_set('include_path', ini_get('include_path') . ":" .  WEB_ROOT . "/includes/classes/transcript_framework");
require_once "DatabaseHandler.class.inc";
require_once "transcript_framework/Transcript.class.inc";
class GameTranscript
{

    public static function getBet($hand_id, $uid, $gmn,$extradetails=false,$game=false)
    {
        if($game) {
            $game_detail = GameTranscript::getGamesDetail2($hand_id, $uid, $gmn, $extradetails);
        } else {
            $game_detail = GameTranscript::getGamesDetail($hand_id, $uid, $gmn, $extradetails);
        }
        $str=false;

        if($game_detail){
            $transcript = Transcript::getInstance($game_detail['gam_admin_history_class']);

            $transcript->setGame($game_detail['gam_name']);

            $transcript->setPunResString($game_detail['pre_bets_paid']);

            $transcript->setBetString($game_detail['pre_bet_string']);

            $transcript->setString($game_detail['pre_bets_paid']);

            $transcript->setResString($game_detail['res_string']);

            $str = $transcript->getBet();
        }
        return $str;
    }



    public static function getResultString($hand_id, $uid, $gmn,$extradetails=false)
    {

        $game_detail = GameTranscript::getGamesDetail2($hand_id, $uid, $gmn,$extradetails);

        $transcript = Transcript::getInstance($game_detail['gam_admin_history_class']);

        $transcript->setPath($game_detail['gam_admin_history_class']);

        $transcript->setBetString($game_detail['pre_bet_string']);

        $transcript->setGame($game_detail['gam_name']);

        $transcript->setString($game_detail['pre_bets_paid']);

        $transcript->setPunResString($game_detail['pre_bet_string']);

        $transcript->setResString($game_detail['res_string']);

        $transcript->setBetTime($game_detail['pre_time']);

        $transcript->setTaxString($game_detail['pre_bet_tax']);

        $str = $transcript->getReadbleTranscript();



        return $str;

    }

    public static function getGamesDetail($hand_id, $uid, $gmn,$extradetails=false)
    {
        $dbh = new DatabaseHandler();
        $dbh->connect();

        settype($hand_id, 'integer');
        settype($uid, 'integer');
        settype($gmn, 'integer');
        if($extradetails){
            $sql="SELECT gam_localization, gam_name,gam_admin_history_class, pre_bet_string, pre_bets_paid,pre_time, res_string,PRE_BET,PRE_WIN , pre_bet_tax
                FROM game, result, punter_result
                WHERE pre_id = ".$dbh->escape($hand_id)."
                AND res_id = pre_res_id
                AND pre_pun_id = ".$dbh->escape($uid)."
                AND res_gam_id = gam_id";
        }
        elseif(EXT_PARTNER_ID > 0){
            $sql="SELECT g.gam_localization, g.gam_name, g.gam_admin_history_class, pre_bet_string, pre_bets_paid,pre_time, res_string ,pre_bet_tax
        FROM game g, ext_punter_result
        WHERE pre_id = $hand_id
        AND pre_pun_id = $uid
        AND pre_game_num =  $gmn
        AND res_gam_id = g.gam_id";
        }
        else{
            $sql = "SELECT gam_localization, gam_name,gam_admin_history_class, pre_bet_string, pre_bets_paid,pre_time, res_string ,pre_bet_tax
                FROM game, result, punter_result
                WHERE pre_id = $hand_id
                AND res_id = pre_res_id
                AND pre_pun_id = $uid
                AND pre_game_num =  $gmn
                AND res_gam_id = gam_id";
        }

//        var_dump($sql);die();
        $game_detail = $dbh->queryRow($sql);
//        $game_detail = $dbh->exec($sql);
//        var_dump($game_detail);die();
        return $game_detail;
    }

    public static function getGamesDetail2($hand_id, $uid, $gmn,$extradetails=false)
    {
//        print_r('eeeeeeeeeeeeee '.$hand_id);
        $dbh = new DatabaseHandler();
        $dbh->connect();

        settype($hand_id, 'integer');
        settype($uid, 'integer');
        settype($gmn, 'integer');
        if($extradetails){
            $sql="SELECT gam_localization, gam_name,gam_admin_history_class, pre_bet_string, pre_bets_paid,pre_time, res_string,PRE_BET,PRE_WIN , pre_bet_tax
                FROM game, result, punter_result
                WHERE pre_res_id = ".$dbh->escape($hand_id)."
                AND res_id = pre_res_id
                AND pre_pun_id = ".$dbh->escape($uid)."
                AND res_gam_id = gam_id";
        }
        elseif(EXT_PARTNER_ID > 0){
            $sql="SELECT g.gam_localization, g.gam_name, g.gam_admin_history_class, pre_bet_string, pre_bets_paid,pre_time, res_string ,pre_bet_tax
        FROM game g, ext_punter_result
        WHERE pre_res_id = $hand_id
        AND pre_pun_id = $uid
        AND pre_game_num =  $gmn
        AND res_gam_id = g.gam_id";
        }
        else{
            $sql = "SELECT gam_localization, gam_name,gam_admin_history_class, pre_bet_string, pre_bets_paid,pre_time, res_string ,pre_bet_tax
                FROM game, result, punter_result
                WHERE pre_res_id = $hand_id
                AND res_id = pre_res_id
                AND pre_pun_id = $uid
                AND pre_game_num =  $gmn
                AND res_gam_id = gam_id";
        }

//        var_dump($sql);die();
        $game_detail = $dbh->queryRow($sql);
//        $game_detail = $dbh->exec($sql);
//        var_dump($game_detail);die();
        return $game_detail;
    }

    public static function getRouletteBet($hand_id, $uid , $gmn)
    {
        $game_detail = GameTranscript::getGamesDetailRoulette($hand_id, $uid , $gmn);

        $transcript = Transcript::getInstance($game_detail['gam_admin_history_class']);

        $transcript->setGame($game_detail['gam_name']);

        $transcript->setBetString($game_detail['ctr_notes']);

        $str = $transcript->getBet();

        return $str;
    }

    public static function getRouletteCDString($hand_id, $uid , $gmn)
    {
        $game_detail = GameTranscript::getGamesDetailRoulette($hand_id, $uid , $gmn);

        $transcript = Transcript::getInstance($game_detail['gam_admin_history_class']);

        $transcript->setPath($game_detail['tbl_name']);

        $transcript->setGame($game_detail['gam_name']);

        $transcript->setString($game_detail['ctr_notes']);

        $transcript->setPunResString($game_detail['ctr_notes']);

        $transcript->setResString($game_detail['res_string']);

        $str = $transcript->getReadbleTranscript();

        return $str;
    }

    public static function getGamesDetailRoulette($hand_id, $uid , $gmn)
    {
        $dbh = new DatabaseHandler();
        $dbh->connect();

        settype($hand_id, 'integer');
        settype($uid, 'integer');
        settype($gmn, 'integer');
        $sql = "SELECT gam_localization, gam_name ,gam_history_admin_class, tbl_name , ctr_notes, ctr_amount, res_string
             FROM game 
             JOIN tables ON (tbl_id + tbl_themeid) = gam_id
             JOIN tables_themes ON  tth_id = tbl_themeid
             JOIN result ON res_tbl_id = tbl_id
             JOIN customer_transaction ON ctr_res_id = res_id
             WHERE ctr_res_id = $hand_id AND tbl_themeid = (res_gam_id - (res_gam_id % 1000)) AND ctr_pun_id = $uid AND ctr_id = ". $gmn;


        $game_detail = $dbh->queryRow($sql);

        return $game_detail;
    }

}

?>