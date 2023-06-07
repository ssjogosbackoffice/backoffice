<?php



function getGamesList() {
    global $dbh;
    $sql = "Select * from game";
    $result = $dbh->doCachedQuery ( $sql, 0 );
    return $result;
}
function getTournamentList($from,$until,$status) {
    global $dbh;
    if($status != 'all') {
        $status = " AND gtt_status = $status";
    } else {
        $status = '';
    }
    $sql = "Select * from game_tournament, game WHERE gtt_start_time between '$from' and '$until' $status AND  gtt_gam_id = gam_id";
    $result = $dbh->doCachedQuery ( $sql, 0 );
    return $result;
}
function getGeneralGames() {
    global $dbh;
    $sql = "Select * from game_global_info";
    $result = $dbh->doCachedQuery ( $sql, 0 );
    return $result;
}
function  updateGame($id,$name, $minbet, $maxbet, $maxpayout, $minposbet, $maxposbet, $smallblind,$bigblind,$extcode, $category, $type, $localization,$adminhistoryclass,$iscommondraw, $isvideo, $clientType, $gameVersion, $isprivate = 0) {
    global $dbh;
    $sql = "Update game set gam_name='$name',gam_min_bet='$minbet',gam_max_bet='$maxbet',gam_max_payout='$maxpayout',gam_min_pos_bet='$minposbet',gam_max_pos_bet='$maxposbet',
            gam_small_blind='$smallblind',gam_big_blind='$bigblind',gam_ext_code='$extcode',gam_category='$category',gam_type='$type',gam_localization= " . (!empty($localization) ? $dbh->prepareString($localization) : "NULL") . ", 
            gam_admin_history_class=".(!empty($adminhistoryclass) ? $dbh->prepareString($adminhistoryclass) : "NULL") .",gam_is_common_draw='$iscommondraw', gam_params=" . (!empty($isvideo) ? $dbh->prepareString($isvideo) : "NULL") . ", gam_client='$clientType', gam_ver ='$gameVersion', gam_is_private = '$isprivate' where gam_id='$id'";
    return $dbh->exec ( $sql );
}
/*function delGame($id) {
    global $dbh;
    $sql = "DELETE FROM game where gam_id='$id'";
    $result = $dbh->exec ( $sql );
} */
function addGame($id,$name, $minbet, $maxbet, $maxpayout, $minposbet, $maxposbet,$smallblind,$bigblind, $extcode, $category, $type, $localization,$adminhistoryclass,$iscommondraw, $isvideo, $clientType, $gameVersion, $isprivate = 0) {
    global $dbh;
    $localization= ($localization == "")? NULL : $localization;
    if ($name != '') {
        $sql = "SELECT * from game where gam_id='$id'";
        if ($result = $dbh->queryRow ( $sql )) {
            return '2';
            die ();
        }
        $sql = "INSERT INTO game (gam_id,gam_name,gam_min_bet,gam_max_bet,gam_max_payout,gam_min_pos_bet,gam_max_pos_bet,gam_small_blind,gam_big_blind,gam_ext_code,gam_category,gam_type,gam_localization,gam_admin_history_class,gam_is_common_draw,gam_params, gam_client, gam_ver, gam_is_private)
				         VALUES('$id','$name','$minbet','$maxbet','$maxpayout','$minposbet','$maxposbet','$smallblind','$bigblind','$extcode','$category','$type'," . (!empty($localization) ? $dbh->prepareString($localization) : "NULL") . ",".(!empty($adminhistoryclass) ? $dbh->prepareString($adminhistoryclass) : "NULL") .",'$iscommondraw', " . (!empty($isvideo) ? $dbh->prepareString($isvideo) : "NULL") . ", '$clientType', '$gameVersion', '$isprivate')";

        if ($result = $dbh->exec ( $sql )) {
            return '1';
        } else {
            return '3';
        }
    }
}

function addNewGeneralGame($category,$buttonImage, $gameId, $width, $height, $partnerCode, $hasPlayForFun=0 ,$webName=false) {
    global $dbh;

        $sql = "INSERT INTO game_global_info(
                                        ggi_category,
                                        ggi_button_image_name,
                                        ggi_game_id,
                                        ggi_width,
                                        ggi_height,
                                        ggi_pes_group,
                                        ggi_has_forfun,
                                        ggi_web_name
                          )
				         VALUES('$category','$buttonImage','$gameId','$width','$height','$partnerCode','$hasPlayForFun','$webName')";
      //die(var_dump($sql));
        if ($result = $dbh->exec ( $sql )) {
            echo '1';
            die ();
        } else {
            echo '3';
            die ();
        }
    }

function addNewTournamentGame($id,$name,$description,$start_time,$start_hour,$start_minute,$min_in,$max_in,$amount,$duration,$bstart,$astart,$status=1) {
    global $dbh;

        $start_hour = $start_hour.":".$start_minute;
        $sql = "INSERT INTO game_tournament (
                             gtt_gam_id,
                             gtt_name,
                             gtt_description,
                             gtt_start_time,
                             gtt_start_hour,
                             gtt_min_inscriptions,
                             gtt_max_inscriptions,
                             gtt_amount,
                             gtt_game_duration_time,
                             gtt_time_wait_bstart,
                             gtt_time_wait_astart,
                             gtt_status
                          )
             VALUES('$id','$name','$description','$start_time','$start_hour','$min_in','$max_in','$amount','$duration','$bstart','$astart','$status')";
//      die(var_dump($sql));
        if ($result = $dbh->exec ( $sql )) {
            die ('1');
        } else {
            echo '3';
            die ();
        }
    }


function  updateGeneralGames($id,$category,$buttonImage, $gameId, $width, $height, $partnerCode, $hasPlayForFu ,$webName) {
    global $dbh;
    $sql = "Update game_global_info set ggi_category='$category',
            ggi_button_image_name='$buttonImage',
            ggi_game_id='$gameId',
            ggi_width='$width',
            ggi_height='$height',
            ggi_pes_group='$partnerCode',
            ggi_has_forfun='$hasPlayForFu',
            ggi_web_name='$webName'
             where ggi_id='$id'";
    return $dbh->exec ( $sql );
}
function  updateTournamentGame($id,$gam_id,$name,$description,$start_time,$start_hour,$start_minute,$min_in,$max_in,$amount,$duration,$bstart,$astart,$status) {
    global $dbh;
    $start_hour = $start_hour.":".$start_minute;
    $sql = "UPDATE game_tournament SET gtt_gam_id = $gam_id,
            gtt_name='$name',
            gtt_description='$description',
            gtt_start_time='$start_time',
            gtt_start_hour='$start_hour',
            gtt_min_inscriptions='$min_in',
            gtt_max_inscriptions='$max_in',
            gtt_amount='$amount',
            gtt_game_duration_time='$duration',
            gtt_time_wait_bstart='$bstart',
            gtt_time_wait_astart='$astart',
            gtt_status='$status'
             WHERE gtt_id='$id'";

    return $dbh->exec ( $sql );
}


function getGameCategories() {
    global $dbh;
    $sql = "select * from game_category";
    $result = $dbh->doCachedQuery ( $sql, 0 );
    return $result;
}
function getGameType() {
    global $dbh;
    $sql = "select * from game_type";
    $result = $dbh->doCachedQuery ( $sql, 0 );
    return $result;
}
?>