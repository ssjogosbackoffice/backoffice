<?php

$admn_totem=check_access ( 'ttm_totem_admin');
if($admn_totem==true){
require 'SocketClient.class.php';

define ( 'TOTEM_SOCKET_SEPARATOR', '@' );
define ( 'SOCKET_REQ_ALLOWDENY', '4002' );
define ( 'SOCKET_REQ_CONNDISCONN', '4003' );
define ( 'SOCKET_REQ_CONNECT', '4004' );
define ( 'SOCKET_REQ_SHUTDOWN', '4005' );
define ( 'SOCKET_REQ_VNC', '4006' );
define ( 'SOCKET_REQ_SSH', '4007' );
define ( 'SOCKET_REQ_WINDOWS' , '4008');
if (! isset ( $_POST['totem'] )) {
	addError ( "", $lang->getLang("Totem code don't found. Check the get parameters" ));
	showErrors ();
	die ();
}


function send($totem_code, $totem_status, $user_id, $totemMD5) {
	global $dbh,$lang;
	$sql = "select pun_customer_number from punter where pun_id = " . $user_id;
	$user_number = $dbh->queryOne($sql);
	$md5 = hash('md5', '3012' . $totem_code . $totemMD5);

	$result = "";
	// Viene generata la chiave per l'autenticit� del messaggio
	$DataToSend ="operationtype=3012&cd=" . $totem_code . "&userid=" . $user_number . "&status=" . $totem_status . "&md5=" . $md5 . "&suboperation=1";

	// Apro la connessione
	$ch=curl_init(WEB_APP_TOTEM);

	// Imposto gli headers HTTP
	// imposto curl per protocollo https
	curl_setopt($ch,CURLOPT_SSL_VERIFYPEER, FALSE);
	curl_setopt($ch,CURLOPT_POST,1);

	// Invio i dati
	curl_setopt($ch,CURLOPT_POSTFIELDS, $DataToSend);

	//imposta la variabile PHP
	curl_setopt($ch,CURLOPT_RETURNTRANSFER, 1);

	//Ricevo la risposta dal server
	$result=curl_exec($ch);
	error_log("CURL Request: " . $result);
	return $result;
}

$ttmCode = $_POST['totem'];
$sql_totem = "Select ttm_ip, 
			  		 ttm_totem_md5, 
			  		 ttm_code, 
			  		 ttm_access, 
			  		 ttm_club_id,
			  		 jur_name, 
			  		 ttm_pun_id, 
			  		 pcr_credits,
			  		  cur_code_for_web
			  		    from totem
			  		     left join punter_credit on pcr_pun_id = ttm_pun_id
			  		     left join jurisdiction on jur_id = ttm_club_id
			  		     join currency on jur_currency=cur_id
			  		     where ttm_code = " . $dbh->quote ( $ttmCode );

$result = $dbh->queryRow ( $sql_totem );
if (! $result) {
	addError ( '', $lang->getLang("Totem don't found. Check the code" ));
	showErrors ();
	die ();
}

$totemIP = $result ['ttm_ip'];
$totemMD5 = $result ['ttm_totem_md5'];
$totemCode = $result ['ttm_code'];
$totemAccess = $result ['ttm_access'];
$totemClubId = $result ['ttm_club_id'];
$totemClub = $result ['jur_name'];
$totemId = $result ['ttm_pun_id'];
$totemCredits = $result ['pcr_credits'];
	$currency= $result['cur_code_for_web'];

$socket = new SocketClient ();
$socket->connect ( $totemIP, 9000 );
if (! $socket->isConnected ()) {
	addError ( '', $lang->getLang("Connection with totem is unavailable" )); // .
	                                                             // $socket->getErrorMessage());
	showErrors ();
	
	?>
		<table>
		<tr>
			<td class="content"><?=$lang->getLang('To try connecting again, press on Connect')?></td>
		<tr>	
	</table>
		<button id="connect<?=$ttmCode?>"><?=$lang->getLang('Connect')?>
				<script>
						 $(function() {
						$( '#connect<?=$ttmCode?>')
					            .button()
					            .click(function() {
					            	$.post("totem/totem_connection_individual.inc",{totem:'<?=$ttmCode?>'}, function(data){			
					            	$('#connect<?=$ttmCode?>').parent().closest('div').empty().append( data );
						            });
						            });
							return false;
							});
						</script>	
			</button>
<?php
	
	die ();

}
function logOnTable($ttm_code, $logNote, $totem_club) {
	global $dbh;
	$sql = "select ttm_id from totem where ttm_code = " . $dbh->quote ( $ttm_code );
	$ttm_id = $dbh->queryOne ( $sql );
	$datecreate = date ( "y-m-d H:i:s" );
	$ip = $_SERVER ["REMOTE_ADDR"];
	$sqlrequest3 = "INSERT INTO totem_log(ttl_ttm_id, ttl_ip, ttl_note, ttl_date, ttl_tlt_id, ttl_club_id) VALUES(" . $dbh->quote ( $ttm_id, 'integer' ) . "," . $dbh->quote ( $ip ) . "," . $dbh->quote ( $logNote ) . "," . $dbh->quote ( $datecreate, 'date' ) . ", 3," . $dbh->quote ( $totem_club ) . ")";
	$result3 = $dbh->exec ( $sqlrequest3 );

}
if (isset ( $_POST ['socket_send'] )) {
// 	if (isset ( $_POST ['vncon'] )) {
// 		$rCode = SOCKET_REQ_VNC;
// 		$counter = "10000";
// 		$body = "1";
// 		$controlMD5 = hash ( 'md5', $totemCode . $counter . $rCode . $body . $totemMD5, false );
// 		$socket->write ( $totemCode . TOTEM_SOCKET_SEPARATOR . $counter . TOTEM_SOCKET_SEPARATOR . $rCode . TOTEM_SOCKET_SEPARATOR . $body . TOTEM_SOCKET_SEPARATOR . $controlMD5 );
		
// 		$response = $socket->read ();
// 		if ($response != '' && ! is_null ( $response )) {
// 			$respArray = split ( TOTEM_SOCKET_SEPARATOR, $response );
// 			$md5Resp = trim ( hash ( 'md5', $respArray [0] . $respArray [1] . $respArray [2] . $respArray [3] . $totemMD5 ) );
// 			$socketMd5 = substr ( $respArray [4], 0, 32 );
// 			settype ( $md5Resp, 'string' );
// 			settype ( $socketMd5, 'string' );
// 			if ($md5Resp != $socketMd5) {
// 				addError ( '', "VNC ON response md5 doesn't match" );
// 			} else {
// 				settype ( $respArray [3], 'integer' );
// 				if ($respArray [3] == 1) {
// 					logOnTable ( $totemCode, 'Totem VNC Activated by user ' . $_SESSION ['admin_username'], $totemClubId );
// 					addResult ( '', "VNC attivato da questo momento e' possibile entrare" );
// 				} else {
// 					addError ( '', "Il Totem non e' riuscito ad attivare VNC" );
// 				}
			
// 			}
// 		} else {
// 			addError ( '', "Il Totem non ha risposto" );
// 		}
	
// 	}
// 	if (isset ( $_POST ['vncoff'] )) {
// 		$rCode = SOCKET_REQ_VNC;
// 		$counter = "10000";
// 		$body = "2";
// 		$controlMD5 = hash ( 'md5', $totemCode . $counter . $rCode . $body . $totemMD5, false );
// 		$socket->write ( $totemCode . TOTEM_SOCKET_SEPARATOR . $counter . TOTEM_SOCKET_SEPARATOR . $rCode . TOTEM_SOCKET_SEPARATOR . $body . TOTEM_SOCKET_SEPARATOR . $controlMD5 );
		
// 		$response = $socket->read ();
// 		if ($response != '' && ! is_null ( $response )) {
// 			$respArray = split ( TOTEM_SOCKET_SEPARATOR, $response );
// 			$md5Resp = hash ( 'md5', $respArray [0] . $respArray [1] . $respArray [2] . $respArray [3] . $totemMD5 );
// 			$socketMd5 = substr ( $respArray [4], 0, 32 );
// 			settype ( $md5Resp, 'string' );
// 			settype ( $socketMd5, 'string' );
// 			if ($md5Resp != $socketMd5) {
// 				addError ( '', "VNC OFF response md5 doesn't match" );
// 			} else {
// 				settype ( $respArray [3], 'integer' );
// 				if ($respArray [3] == 1) {
// 					logOnTable ( $totemCode, 'Totem VNC Deactivated by user ' . $_SESSION ['admin_username'], $totemClubId );
// 					addResult ( '', "VNC disattivato da questo momento non e' piu' accessibile" );
// 				} else {
// 					addError ( '', "Il Totem non e' riuscito a disattivare VNC" );
// 				}
			
// 			}
// 		} else {
// 			addError ( '', "Il Totem non ha risposto" );
// 		}
// 	}
// 	if (isset ( $_POST ['sshon'] )) {
// 		$rCode = SOCKET_REQ_SSH;
// 		$counter = "10000";
// 		$body = "1";
// 		$controlMD5 = hash ( 'md5', $totemCode . $counter . $rCode . $body . $totemMD5, false );
// 		$socket->write ( $totemCode . TOTEM_SOCKET_SEPARATOR . $counter . TOTEM_SOCKET_SEPARATOR . $rCode . TOTEM_SOCKET_SEPARATOR . $body . TOTEM_SOCKET_SEPARATOR . $controlMD5 );
		
// 		$response = $socket->read ();
// 		if ($response != '' && ! is_null ( $response )) {
// 			$respArray = split ( TOTEM_SOCKET_SEPARATOR, $response );
// 			$md5Resp = trim ( hash ( 'md5', $respArray [0] . $respArray [1] . $respArray [2] . $respArray [3] . $totemMD5 ) );
// 			$socketMd5 = substr ( $respArray [4], 0, 32 );
// 			settype ( $md5Resp, 'string' );
// 			settype ( $socketMd5, 'string' );
// 			if ($md5Resp != $socketMd5) {
// 				addError ( '', "SSH ON response md5 doesn't match" );
// 			} else {
// 				settype ( $respArray [3], 'integer' );
// 				if ($respArray [3] == 1) {
// 					logOnTable ( $totemCode, 'Totem SSH Activated by user ' . $_SESSION ['admin_username'], $totemClubId );
// 					addResult ( '', "SSH attivato da questo momento e' possibile entrare" );
// 				} else {
// 					addError ( '', "Il Totem non e' riuscito ad attivare SSH" );
// 				}
			
// 			}
// 		} else {
// 			addError ( '', "Il Totem non ha risposto" );
// 		}
	
// 	}
// 	if (isset ( $_POST ['sshoff'] )) {
// 		$rCode = SOCKET_REQ_SSH;
// 		$counter = "10000";
// 		$body = "2";
// 		$controlMD5 = hash ( 'md5', $totemCode . $counter . $rCode . $body . $totemMD5, false );
// 		$socket->write ( $totemCode . TOTEM_SOCKET_SEPARATOR . $counter . TOTEM_SOCKET_SEPARATOR . $rCode . TOTEM_SOCKET_SEPARATOR . $body . TOTEM_SOCKET_SEPARATOR . $controlMD5 );
		
// 		$response = $socket->read ();
// 		if ($response != '' && ! is_null ( $response )) {
// 			$respArray = split ( TOTEM_SOCKET_SEPARATOR, $response );
// 			$md5Resp = hash ( 'md5', $respArray [0] . $respArray [1] . $respArray [2] . $respArray [3] . $totemMD5 );
// 			$socketMd5 = substr ( $respArray [4], 0, 32 );
// 			settype ( $md5Resp, 'string' );
// 			settype ( $socketMd5, 'string' );
// 			if ($md5Resp != $socketMd5) {
// 				addError ( '', "SSH OFF response md5 doesn't match" );
// 			} else {
// 				settype ( $respArray [3], 'integer' );
// 				if ($respArray [3] == 1) {
// 					logOnTable ( $totemCode, 'Totem SSH Deactivated by user ' . $_SESSION ['admin_username'], $totemClubId );
// 					addResult ( '', "SSH disattivato da questo momento non e' piu' accessibile" );
// 				} else {
// 					addError ( '', "Il Totem non e' riuscito a disattivare SSH" );
// 				}
			
// 			}
// 		} else {
// 			addError ( '', "Il Totem non ha risposto" );
// 		}
// 	}
	if (isset ( $_POST ['allowon'] )) {
		$sql = "UPDATE totem set ttm_access = 'allow' where ttm_code = " . $dbh->quote ( $totemCode );
		$res = $dbh->exec ( $sql );
		if ($res) {
			send($totemCode, 'allow', $totemId, $totemMD5);
			
			logOnTable ( $totemCode, 'Totem Allowed by user ' . $_SESSION ['admin_username'], $totemClubId );
			$totemAccess = 'ALLOW';
			$rCode = SOCKET_REQ_ALLOWDENY;
			$counter = "10000";
			$body = "1";
			$controlMD5 = hash ( 'md5', $totemCode . $counter . $rCode . $body . $totemMD5, false );
			$socket->write ( $totemCode . TOTEM_SOCKET_SEPARATOR . $counter . TOTEM_SOCKET_SEPARATOR . $rCode . TOTEM_SOCKET_SEPARATOR . $body . TOTEM_SOCKET_SEPARATOR . $controlMD5 );
			
			$response = $socket->read ();
			if ($response != '' && ! is_null ( $response )) {
				$respArray = split ( TOTEM_SOCKET_SEPARATOR, $response );
				$md5Resp = trim ( hash ( 'md5', $respArray [0] . $respArray [1] . $respArray [2] . $respArray [3] . $totemMD5 ) );
				$socketMd5 = substr ( $respArray [4], 0, 32 );
				settype ( $md5Resp, 'string' );
				settype ( $socketMd5, 'string' );
				if ($md5Resp != $socketMd5) {
					addError ( '', "ALLOW response md5 doesn't match" );
				} else {
					settype ( $respArray [3], 'integer' );
					if ($respArray [3] == 1) {
                        addResult ( '', $lang->getLang("The totem is now enabled" ));
					} else {
                        addError ( '', $lang->getLang("Unable to activate the totem") );
					}
				    
				}
			} else {
                addError ( '',  $lang->getLang("The totem is unavailable" ));
			}
		} else {
            addError ( '', $lang->getLang("Impossible to execute the query") );
		}
	
	}
	if (isset ( $_POST ['denyon'] )) {
		$sql = "UPDATE totem set ttm_access = 'deny' where ttm_code = " . $dbh->quote ( $totemCode );
		$res = $dbh->exec ( $sql );
		if ($res) {
			send($totemCode, 'deny', $totemId, $totemMD5);
			
			logOnTable ( $totemCode, 'Totem Denied by user ' . $_SESSION ['admin_username'], $totemClubId );
			$totemAccess = 'DENY';
			$rCode = SOCKET_REQ_ALLOWDENY;
			$counter = "10000";
			$body = "0";
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
					addError ( '', "DENY response md5 doesn't match" );
				} else {
					settype ( $respArray [3], 'integer' );
					if ($respArray [3] == 1) {
                        addResult ( '', $lang->getLang("The totem is now blocked" ));
					} else {
                        addError ( '', $lang->getLang("Unable to block totem" ));
					}
				
				}
			} else {
                addError ( '',  $lang->getLang("The totem is unavailable" ));
			}
		} else {
            addError ( '', $lang->getLang("Impossible to execute the query") );
		}
	}
// 	if (isset ( $_POST ['connecton'] )) {
// 		$rCode = SOCKET_REQ_CONNECT;
// 		$counter = "10000";
// 		$body = "1";
// 		$controlMD5 = hash ( 'md5', $totemCode . $counter . $rCode . $body . $totemMD5, false );
// 		$socket->write ( $totemCode . TOTEM_SOCKET_SEPARATOR . $counter . TOTEM_SOCKET_SEPARATOR . $rCode . TOTEM_SOCKET_SEPARATOR . $body . TOTEM_SOCKET_SEPARATOR . $controlMD5 );
		
// 		$response = $socket->read ();
// 		if ($response != '' && ! is_null ( $response )) {
// 			$respArray = split ( TOTEM_SOCKET_SEPARATOR, $response );
// 			$md5Resp = hash ( 'md5', $respArray [0] . $respArray [1] . $respArray [2] . $respArray [3] . $totemMD5 );
// 			$socketMd5 = substr ( $respArray [4], 0, 32 );
// 			settype ( $md5Resp, 'string' );
// 			settype ( $socketMd5, 'string' );
// 			if ($md5Resp != $socketMd5) {
// 				addError ( '', "CONNECT ON response md5 doesn't match" );
// 			} else {
// 				settype ( $respArray [3], 'integer' );
// 				if ($respArray [3] == 1) {
// 					logOnTable ( $totemCode, 'Totem Connected by user ' . $_SESSION ['admin_username'], $totemClubId );
// 					addResult ( '', "Attivato da questo momento e' accessibile" );
// 				} else {
// 					addError ( '', "Il Totem non e' riuscito a connettersi" );
// 				}
			
// 			}
// 		} else {
// 			addError ( '', "Il Totem non ha risposto" );
// 		}
// 	}
// 	if (isset ( $_POST ['connectoff'] )) {
// 		$rCode = SOCKET_REQ_CONNECT;
// 		$counter = "10000";
// 		$body = "0";
// 		$controlMD5 = hash ( 'md5', $totemCode . $counter . $rCode . $body . $totemMD5, false );
// 		$socket->write ( $totemCode . TOTEM_SOCKET_SEPARATOR . $counter . TOTEM_SOCKET_SEPARATOR . $rCode . TOTEM_SOCKET_SEPARATOR . $body . TOTEM_SOCKET_SEPARATOR . $controlMD5 );
		
// 		$response = $socket->read ();
// 		if ($response != '' && ! is_null ( $response )) {
// 			$respArray = split ( TOTEM_SOCKET_SEPARATOR, $response );
// 			$md5Resp = hash ( 'md5', $respArray [0] . $respArray [1] . $respArray [2] . $respArray [3] . $totemMD5 );
// 			$socketMd5 = substr ( $respArray [4], 0, 32 );
// 			settype ( $md5Resp, 'string' );
// 			settype ( $socketMd5, 'string' );
// 			if ($md5Resp != $socketMd5) {
// 				addError ( '', "CONNECT OFF response md5 doesn't match" );
// 			} else {
// 				settype ( $respArray [3], 'integer' );
// 				if ($respArray [3] == 1) {
// 					logOnTable ( $totemCode, 'Totem Disconnected by user ' . $_SESSION ['admin_username'], $totemClubId );
// 					addResult ( '', "Disconnesso da questo momento non e' piu' accessibile" );
// 				} else {
// 					addError ( '', "Il Totem non e' riuscito a disconnetersi" );
// 				}
			
// 			}
// 		} else {
// 			addError ( '', "Il Totem non ha risposto" );
// 		}
// 	}
	if (isset ( $_POST ['restart'] )) {
		$rCode = SOCKET_REQ_SHUTDOWN;
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
				addError ( '', "RESTART response md5 doesn't match" );
			} else {
				settype ( $respArray [3], 'integer' );
				if ($respArray [3] == 1) {
					logOnTable ( $totemCode, 'Totem Restarted by user ' . $_SESSION ['admin_username'], $totemClubId );
                    addResult ( '', $lang->getLang("The totem has recived the restart" ));
				} else {
                    addError ( '', $lang->getLang("The totem wasn't unable to restart" ));
				}
			
			}
		} else {
            addError ( '',$lang->getLang("The totem is unavailable" ));
		}
	}
	if (isset ( $_POST ['shutdown'] )) {
		$rCode = SOCKET_REQ_SHUTDOWN;
		$counter = "10000";
		$body = "2";
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
				addError ( '', "SHUTDOWN response md5 doesn't match" );
			} else {
				settype ( $respArray [3], 'integer' );
				if ($respArray [3] == 1) {
					logOnTable ( $totemCode, 'Totem Turned off by user ' . $_SESSION ['admin_username'], $totemClubId );
                    addResult ( '', $lang->getLang("The totem has recived the shutdown" ));
				} else {
                    addError ( '', $lang->getLang("The totem wasn't unable to shutdown" ));
				}
			
			}
		} else {
            addError ( '', $lang->getLang("The totem is unavailable" ));
		}
	}
// 	if (isset ( $_POST ['winsblock'] )) {
// 		$rCode = SOCKET_REQ_WINDOWS;
// 		$counter = "10000";
// 		$body = "1";
// 		$controlMD5 = hash ( 'md5', $totemCode . $counter . $rCode . $body . $totemMD5, false );
// 		$socket->write ( $totemCode . TOTEM_SOCKET_SEPARATOR . $counter . TOTEM_SOCKET_SEPARATOR . $rCode . TOTEM_SOCKET_SEPARATOR . $body . TOTEM_SOCKET_SEPARATOR . $controlMD5 );
	
// 		$response = $socket->read ();
// 		if ($response != '' && ! is_null ( $response )) {
// 			$respArray = split ( TOTEM_SOCKET_SEPARATOR, $response );
// 			$md5Resp = hash ( 'md5', $respArray [0] . $respArray [1] . $respArray [2] . $respArray [3] . $totemMD5 );
// 			$socketMd5 = substr ( $respArray [4], 0, 32 );
// 			settype ( $md5Resp, 'string' );
// 			settype ( $socketMd5, 'string' );
// 			if ($md5Resp != $socketMd5) {
// 				addError ( '', "WINSBLOCK response md5 doesn't match" );
// 			} else {
// 				settype ( $respArray [3], 'integer' );
// 				if ($respArray [3] == 1) {
// 					logOnTable ( $totemCode, 'Totem Windows Sblocked by user ' . $_SESSION ['admin_username'], $totemClubId );
// 					addResult ( '', "Il totem ha ricevuto lo sblocco" );
// 				} else {
// 					addError ( '', "Il Totem non e' riuscito a sbloccarsi" );
// 				}
					
// 			}
// 		} else {
// 			addError ( '', "Il Totem non ha risposto" );
// 		}
// 	}
// 	if (isset ( $_POST ['winblock'] )) {
// 		$rCode = SOCKET_REQ_WINDOWS;
// 		$counter = "10000";
// 		$body = "0";
// 		$controlMD5 = hash ( 'md5', $totemCode . $counter . $rCode . $body . $totemMD5, false );
// 		$socket->write ( $totemCode . TOTEM_SOCKET_SEPARATOR . $counter . TOTEM_SOCKET_SEPARATOR . $rCode . TOTEM_SOCKET_SEPARATOR . $body . TOTEM_SOCKET_SEPARATOR . $controlMD5 );
	
// 		$response = $socket->read ();
// 		if ($response != '' && ! is_null ( $response )) {
// 			$respArray = split ( TOTEM_SOCKET_SEPARATOR, $response );
// 			$md5Resp = hash ( 'md5', $respArray [0] . $respArray [1] . $respArray [2] . $respArray [3] . $totemMD5 );
// 			$socketMd5 = substr ( $respArray [4], 0, 32 );
// 			settype ( $md5Resp, 'string' );
// 			settype ( $socketMd5, 'string' );
// 			if ($md5Resp != $socketMd5) {
// 				addError ( '', "WINBLOCK response md5 doesn't match" );
// 			} else {
// 				settype ( $respArray [3], 'integer' );
// 				if ($respArray [3] == 1) {
// 					logOnTable ( $totemCode, 'Totem Windows Blocked by user ' . $_SESSION ['admin_username'], $totemClubId );
// 					addResult ( '', "Il totem ha ricevuto il blocco" );
// 				} else {
// 					addError ( '', "Il Totem non e' riuscito a bloccarsi" );
// 				}
					
// 			}
// 		} else {
// 			addError ( '', "Il Totem non ha risposto" );
// 		}
// 	}
}
if (strtoupper ( $totemAccess ) == 'ALLOW') {
	$rCode = SOCKET_REQ_CONNDISCONN;
	$counter = "10000";
	$body = "NONE";
	$controlMD5 = hash ( 'md5', $totemCode . $counter . $rCode . $body . $totemMD5, false );
	$socket->write ( $totemCode . TOTEM_SOCKET_SEPARATOR . $counter . TOTEM_SOCKET_SEPARATOR . $rCode . TOTEM_SOCKET_SEPARATOR . $body . TOTEM_SOCKET_SEPARATOR . $controlMD5 );
	$response = $socket->read ();
	if ($response != '' && ! is_null ( $response )) {
		$respArray = split ( TOTEM_SOCKET_SEPARATOR, $response );
		$md5Resp = trim ( hash ( 'md5', $respArray [0] . $respArray [1] . $respArray [2] . $respArray [3] . $totemMD5 ) );
		$socketMd5 = substr ( $respArray [4], 0, 32 );
		settype ( $md5Resp, 'string' );
		settype ( $socketMd5, 'string' );
		if ($md5Resp != $socketMd5) {
			addError ( '', "CONN STATUS response md5 doesn't match" );
		} else {
			settype ( $respArray [3], 'integer' );
			$totemIsLogged = $respArray [3];
		}
	} else {
        addError ( '', $lang->getLang("The totem is unavailable" ));
	}
}
?>
<!--  <form action="<?=$_SERVER['PHP_SELF']."?".$_SERVER['QUERY_STRING']?>" 
	method="POST">-->
		
	<p class="bodyHD">A<?=$lang->getLang('Totem Administration')?><?=$totemCode?></p>
	<table>
		<tr>
			<td class="label"><?=$lang->getLang('Ip')?>:</td>
			<td class="content"><?=$totemIP?></td>
		</tr>
		<tr>
			<td class="label"><?=$lang->getLang('Access')?>:</td>
			<td class="content"><? if(strtoupper ( $totemAccess) == 'ALLOW') {?><img style="width:20px; height:20px;" src="live/liveimage/allow.png"><? }else{?><img style="width:20px; height:20px;" src="live/liveimage/deny.png"><?}?></td>
		</tr>
		<tr>
			<td class="label"><?=$lang->getLang('Customer number')?>:</td>
			<td class="content"><?=$totemId?></td>
		</tr>
		<tr>
			<td class="label"><?=$lang->getLang('Credits')?>:</td>
			<td class="content"><?=getInDollars($totemCredits,2,$currency)?></td>
		</tr>
	</table>

	<hr />

	<table style="border-spacing:0px;">
		<!-- <tr>
		    <?php if(strtoupper($totemAccess) == "ALLOW"){?>
			<td class="content"><img src="<?=image_dir?>/icon_lock_open.gif"
				width="20" height="20" /></td>
			<?php }else{ ?>
			<td class="content"><img src="<?=image_dir?>/icon_lock_close.gif"
				width="20" height="20" /></td>
			<?php } ?>
			<td><input type="submit" name="allowon" value="ALLOW" /></td>
			<td><input type="submit" name="denyon" value="DENY" /></td>
		</tr>
		<?php if(strtoupper($totemAccess) == "ALLOW"){ ?>
		<tr>
		    <?php if($totemIsLogged == "1"){?>
			<td class="content"><img src="<?=image_dir?>/icon_online.gif"
				width="20" height="20" /></td>
			<?php }else{ ?>
			<td class="content"><img src="<?=image_dir?>/icon_offline.gif"
				width="20" height="20" /></td>
			<?php } ?>
			<td><input type="submit" name="connecton" value="Connetti" /></td>
			<td><input type="submit" name="connectoff" value="Disconnetti" /></td>
		</tr>
		<?php  } ?>
		<tr>
			<td class="content"><img src="<?=image_dir?>/uvnc.png" width="20"
				height="20" /></td>
			<td><input type="submit" name="vncon" value="Attiva VNC" /></td>
			<td><input type="submit" name="vncoff" value="Disattiva VNC" /></td>
		</tr>
	 	<tr>
		 	<td class="content"><img src="<?=image_dir?>/ssh.png" width="20"
				height="20" /></td>
			<td><input type="submit" name="sshon" value="Attiva SSH" /></td>
			<td><input type="submit" name="sshoff" value="Disattiva SSH" /></td>
		</tr>-->
		<tr class="content">
			<td><img src="<?=image_dir?>/shutdown.png" width="20"
				height="20" /></td>
			<td>		
			<button id="restart<?=$ttmCode?>"><?=$lang->getLang('Restart Totem')?>
				<script>
						 $(function() {
						$( '#restart<?=$ttmCode?>')
					            .button()
					            .click(function() {
					            	$.post("totem/totem_connection_individual.inc",{totem:'<?=$ttmCode?>',socket_send:'1',restart:'1',}, function(data){			
					            	$('#restart<?=$ttmCode?>').parent().closest('div').empty().append( data );
						            });
						            });
							return false;
							});
						</script>	
			</button>		
			</td>
			<td>
				<button id="shutdown<?=$ttmCode?>"><?=$lang->getLang('Shutdown Totem')?>
				<script>
						 $(function() {
						$( '#shutdown<?=$ttmCode?>')
					            .button()
					            .click(function() {
					            	$.post("totem/totem_connection_individual.inc",{totem:'<?=$ttmCode?>',socket_send:'1',shutdown:'1'}, function(data){
					            	$('#shutdown<?=$ttmCode?>').parent().closest('div').empty().append( data );
						            });
						            });
							return false;
							});
						</script>	
			</button>			
			</td>
		</tr>
	<!-- 	<tr>
			<td class="content"><img src="<?=image_dir?>/windows.png" width="20"
				height="20" /></td>
			<td><input type="submit" name="winsblock" value="Sblocca Windows" /></td>
			<td><input type="submit" name="winblock" value="Blocca Windows" /></td>
		</tr>
	-->
	</table>	
<?}else{?>
<div><?=$lang->getLang("You are not allowed to see this page!")?></div>
<?}?>
<!-- </form>-->
<?=showErrors()?>
<?=showResult();?>