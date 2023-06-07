<?php
require 'SocketClient.class.php';
define ( 'TOTEM_SOCKET_SEPARATOR', '@' );
define ( 'SOCKET_REQ_CONNECT', '4004' );
function logOnTable($ttm_code, $logNote, $totem_club) {
	global $dbh;
	$sql = "select ttm_id from totem where ttm_code = " . $dbh->quote ( $ttm_code );
	$ttm_id = $dbh->queryOne ( $sql );
	$datecreate = date ( "y-m-d H:i:s" );
	$ip = $_SERVER ["REMOTE_ADDR"];
	$sqlrequest3 = "INSERT INTO totem_log(ttl_ttm_id, ttl_ip, ttl_note, ttl_date, ttl_tlt_id, ttl_club_id) VALUES(" . $dbh->quote ( $ttm_id, 'integer' ) . "," . $dbh->quote ( $ip ) . "," . $dbh->quote ( $logNote ) . "," . $dbh->quote ( $datecreate, 'date' ) . ", 3," . $dbh->quote ( $totem_club ) . ")";
	$result3 = $dbh->exec ( $sqlrequest3 );
}
if ($_POST ['pAccept']) {
	
	$ttt_ID = $_POST ['tr_id'];
	$totemCode = $_POST ['tcode'];
		
		$sql = "Select ttm_ip, ttm_totem_md5 , ttm_club_id  from totem where ttm_code = " . $dbh->quote ( $totemCode );
		$row = $dbh->queryRow ( $sql );
		$ip = $row["ttm_ip"];
		$totemMD5 = $row["ttm_totem_md5"];
		$totemClubId = $row['ttm_club_id'];
		$socket = new SocketClient ();
		$socket->connect ( $ip, 9000 );
		
		if ($socket->isConnected ()) {
			
			$rCode = SOCKET_REQ_CONNECT;
			$counter = "10000";
			$body = "1";
			$controlMD5 = hash ( 'md5', $totemCode . $counter . $rCode . $body . $totemMD5, false );
			$socket->write ( $totemCode . TOTEM_SOCKET_SEPARATOR . $counter . TOTEM_SOCKET_SEPARATOR . $rCode . TOTEM_SOCKET_SEPARATOR . $body . TOTEM_SOCKET_SEPARATOR . $controlMD5 );
			
			$response = $socket->read ();
			if ($response != '' && ! is_null ( $response )) {
				$respArray = split ( TOTEM_SOCKET_SEPARATOR, $response );
				$md5Resp = hash ( 'md5', $respArray [0] . $respArray [1] . $respArray [2] . $respArray [3] . $totemMD5 );
				$socketMd5 = substr ( $respArray [4], 0, 32 );
				settype ( $md5Resp, 'string' );
				settype ( $socketMd5, 'string' );
				if ($md5Resp != $socketMd5) {
					die ( "-102" );
				} else {
					settype ( $respArray [3], 'integer' );
					$sql = "UPDATE totem_transtrack set ttt_status = 'C' where ttt_id = " . $ttt_ID;
					$res = $dbh->exec ( $sql );
					
					if ($respArray [3] == 1) {
						logOnTable ( $totemCode, 'Accept transaction and ublock totem  by user ' . $_SESSION ['admin_username'], $totemClubId );
						die("0");
					} else {
						logOnTable ( $totemCode, 'Accept transaction by user' . $_SESSION ['admin_username'] . ' but totem was already actived', $totemClubId );
						die("0");
					}
				
				}
			} else {
				die("-104");
			}
		} else {
			die("-101");
		}	 
}
?>