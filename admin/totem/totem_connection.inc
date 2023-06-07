<?php
check_access ( 'connect_to_totem', $redirect = true );
require 'SocketClient.class.php';

define ( 'TOTEM_SOCKET_SEPARATOR', '@' );
define ( 'SOCKET_REQ_ALLOWDENY', '4002' );
define ( 'SOCKET_REQ_CONNDISCONN', '4003' );
define ( 'SOCKET_REQ_CONNECT', '4004' );
define ( 'SOCKET_REQ_SHUTDOWN', '4005' );
define ( 'SOCKET_REQ_VNC', '4006' );
define ( 'SOCKET_REQ_SSH', '4007' );
define ( 'SOCKET_REQ_WINDOWS' , '4008');
if (! isset ( $_GET ['ttcode'] )) {
    addError ( "", $lang->getLang("totem code don't found. Check the get parameters" ));
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

$ttmCode = $_GET ['ttcode'];
$sql_totem = "Select ttm_ip, 
			  		 ttm_totem_md5, 
			  		 ttm_code, 
			  		 ttm_access, 
			  		 ttm_club_id,
			  		 jur_name, 
			  		 ttm_pun_id, 
			  		 pcr_credits 
			  		    from totem
			  		     left join punter_credit on pcr_pun_id = ttm_pun_id
			  		     left join jurisdiction on jur_id = ttm_club_id
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

$socket = new SocketClient ();
//die($totemIP);
$socket->connect ( $totemIP, 9000 );
if (! $socket->isConnected ()) {
    addError ( '', $lang->getLang("Totem connection is not available" )); // .
    // $socket->getErrorMessage());
    showErrors ();

    ?>
    <form action="<?=$_SERVER['PHP_SELF']."?".$_SERVER['QUERY_STRING']?>"
          method="POST">
        <table>
            <tr>
                <td class="content"><?=$lang->getLang('To try connecting again, press on Connect')?></td>
            <tr>
        </table>
        <input type="submit" value="<?=$lang->getLang("Connect")?>" />
    </form>
    <?php

    die ();

}
function logOnTable($ttm_code, $logNote, $totem_club) {
    global $dbh,$lang;
    $sql = "select ttm_id from totem where ttm_code = " . $dbh->quote ( $ttm_code );
    $ttm_id = $dbh->queryOne ( $sql );
    $datecreate = date ( "y-m-d H:i:s" );
    $ip = $_SERVER ["REMOTE_ADDR"];
    $sqlrequest3 = "INSERT INTO totem_log(ttl_ttm_id, ttl_ip, ttl_note, ttl_date, ttl_tlt_id, ttl_club_id) VALUES(" . $dbh->quote ( $ttm_id, 'integer' ) . "," . $dbh->quote ( $ip ) . "," . $dbh->quote ( $logNote ) . "," . $dbh->quote ( $datecreate, 'date' ) . ", 3," . $dbh->quote ( $totem_club ) . ")";
    $result3 = $dbh->exec ( $sqlrequest3 );

}
if (isset ( $_POST ['socket_send'] )) {
    if (isset ( $_POST ['vncon'] )) {
        $rCode = SOCKET_REQ_VNC;
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
                addError ( '', $lang->getLang("VNC ON response md5 doesn't match" ));
            } else {
                settype ( $respArray [3], 'integer' );
                if ($respArray [3] == 1) {
                    logOnTable ( $totemCode, 'Totem VNC Activated by user ' . $_SESSION ['admin_username'], $totemClubId );
                    addResult ( '', $lang->getLang("VNC is active.You can now enter" ));
                } else {
                    addError ( '', $lang->getLang("The VNC wasn't activated on this totem" ));
                }

            }
        } else {
            addError ( '', $lang->getLang("The totem is unavailable" ));
        }

    }
    if (isset ( $_POST ['vncoff'] )) {
        $rCode = SOCKET_REQ_VNC;
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
                addError ( '', $lang->getLang("VNC OFF response md5 doesn't match" ));
            } else {
                settype ( $respArray [3], 'integer' );
                if ($respArray [3] == 1) {
                    logOnTable ( $totemCode, 'Totem VNC Deactivated by user ' . $_SESSION ['admin_username'], $totemClubId );
                    addResult ( '', $lang->getLang("VNC disabled.From this moment, the totem is no longer available" ));
                } else {
                    addError ( '', $lang->getLang("The totem was unable to disable VNC" ));
                }

            }
        } else {
            addError ( '',  $lang->getLang("The totem is unavailable" ));
        }
    }
    if (isset ( $_POST ['sshon'] )) {
        $rCode = SOCKET_REQ_SSH;
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
                addError ( '', $lang->getLang("SSH ON response md5 doesn't match" ));
            } else {
                settype ( $respArray [3], 'integer' );
                if ($respArray [3] == 1) {
                    logOnTable ( $totemCode, 'Totem SSH Activated by user ' . $_SESSION ['admin_username'], $totemClubId );
                    addResult ( '', $lang->getLang("SSH is active.You can now enter" ));
                } else {
                    addError ( '', $lang->getLang("The totem was unable to enable SSH" ));
                }

            }
        } else {
            addError ( '', $lang->getLang("The totem is unavailable" ));
        }

    }
    if (isset ( $_POST ['sshoff'] )) {
        $rCode = SOCKET_REQ_SSH;
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
                addError ( '', $lang->getLang("SSH OFF response md5 doesn't match" ));
            } else {
                settype ( $respArray [3], 'integer' );
                if ($respArray [3] == 1) {
                    logOnTable ( $totemCode, 'Totem SSH Deactivated by user ' . $_SESSION ['admin_username'], $totemClubId );
                    addResult ( '', $lang->getLang("SSH disabled.From this moment, is no longer available" ));
                } else {
                    addError ( '', $lang->getLang("The totem was unable to disable SSH" ));
                }

            }
        } else {
            addError ( '', $lang->getLang("The totem is unavailable" ));
        }
    }
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
                    addError ( '', $lang->getLang("ALLOW response md5 doesn't match" ));
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
                    addError ( '', $lang->getLang("DENY response md5 doesn't match" ));
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
    if (isset ( $_POST ['connecton'] )) {
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
                addError ( '', $lang->getLang("CONNECT ON response md5 doesn't match" ));
            } else {
                settype ( $respArray [3], 'integer' );
                if ($respArray [3] == 1) {
                    logOnTable ( $totemCode, 'Totem Connected by user ' . $_SESSION ['admin_username'], $totemClubId );
                    addResult ( '', $lang->getLang("Activated.From this moment is now available" ));
                } else {
                    addError ( '', $lang->getLang("Unable to connect to totem" ));
                }

            }
        } else {
            addError ( '', $lang->getLang("The totem is unavailable" ));
        }
    }
    if (isset ( $_POST ['connectoff'] )) {
        $rCode = SOCKET_REQ_CONNECT;
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
                addError ( '', $lang->getLang("CONNECT OFF response md5 doesn't match" ));
            } else {
                settype ( $respArray [3], 'integer' );
                if ($respArray [3] == 1) {
                    logOnTable ( $totemCode, 'Totem Disconnected by user ' . $_SESSION ['admin_username'], $totemClubId );
                    addResult ( '', $lang->getLang("Disconnected.From this moment, is no longer available" ));
                } else {
                    addError ( '', $lang->getLang("The totem wasn't unable to disconnect" ));
                }

            }
        } else {
            addError ( '', $lang->getLang("The totem is unavailable" ));
        }
    }
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
                addError ( '', $lang->getLang("RESTART response md5 doesn't match" ));
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
                addError ( '', $lang->getLang("SHUTDOWN response md5 doesn't match" ));
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
    if (isset ( $_POST ['winsblock'] )) {
        $rCode = SOCKET_REQ_WINDOWS;
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
                addError ( '', $lang->getLang("WINSBLOCK response md5 doesn't match" ));
            } else {
                settype ( $respArray [3], 'integer' );
                if ($respArray [3] == 1) {
                    logOnTable ( $totemCode, 'Totem Windows Sblocked by user ' . $_SESSION ['admin_username'], $totemClubId );
                    addResult ( '', $lang->getLang("The totem has recived the unblock signal" ));
                } else {
                    addError ( '', $lang->getLang("The totem was unable to unblock" ));
                }

            }
        } else {
            addError ( '', $lang->getLang("The totem is unavailable" ));
        }
    }
    if (isset ( $_POST ['winblock'] )) {
        $rCode = SOCKET_REQ_WINDOWS;
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
                addError ( '', $lang->getLang("WINBLOCK response md5 doesn't match" ));
            } else {
                settype ( $respArray [3], 'integer' );
                if ($respArray [3] == 1) {
                    logOnTable ( $totemCode, 'Totem Windows Blocked by user ' . $_SESSION ['admin_username'], $totemClubId );
                    addResult ( '', $lang->getLang("The totem has recived the block signal" ));
                } else {
                    addError ( '', $lang->getLang("The totem was unable to block" ));
                }

            }
        } else {
            addError ( '', $lang->getLang("The totem is unavailable" ));
        }
    }
}
/*if (strtoupper ( $totemAccess ) == 'ALLOW') {
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
		addError ( '', "Il Totem non ha risposto" );
	}
}*/
?>
<form action="<?=$_SERVER['PHP_SELF']."?".$_SERVER['QUERY_STRING']?>"
      method="POST">
    <input type="hidden" name="socket_send" />
    <div class="bodyHD"><?=$lang->getLang('Totem Administration')?><?=$totemCode?></div>
    <table>
        <tr>
            <td class="label"><?=$lang->getLang('Ip')?></td>
            <td class="content"><?=$totemIP?></td>
        </tr>
        <tr>
            <td class="label"><?=$lang->getLang('Access')?>:</td>
            <td class="content"><?=$totemAccess?></td>
        </tr>
        <tr>
            <td class="label"><?=$lang->getLang('User Id')?>:</td>
            <td class="content"><?=$totemId?></td>
        </tr>
        <tr>
            <td class="label"><?=$lang->getLang('Credits')?>:</td>
            <td class="content"><?=getInDollars($totemCredits)?></td>
        </tr>
    </table>

    <hr />

    <table>
        <tr>
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
                <td><input type="submit" name="connecton" value="<?=$lang->getLang("Connect")?>" /></td>
                <td><input type="submit" name="connectoff" value="<?=$lang->getLang("Disconnect")?>" /></td>
            </tr>
        <?php  } ?>
        <tr>
            <td class="content"><img src="<?=image_dir?>/uvnc.png" width="20"
                                     height="20" /></td>
            <td><input type="submit" name="vncon" value="<?=$lang->getLang("Activate VNC")?>" /></td>
            <td><input type="submit" name="vncoff" value="<?=$lang->getLang("Deactivate VNC")?>" /></td>
        </tr>
        <!-- 	<tr>
		 	<td class="content"><img src="<?=image_dir?>/ssh.png" width="20"
				height="20" /></td>
			<td><input type="submit" name="sshon" value="Attiva SSH" /></td>
			<td><input type="submit" name="sshoff" value="Disattiva SSH" /></td>
		</tr>-->
        <tr>
            <td class="content"><img src="<?=image_dir?>/shutdown.png" width="20"
                                     height="20" /></td>
            <td><input type="submit" name="restart" value="<?=$lang->getLang("Restart the totem")?>" /></td>
            <td><input type="submit" name="shutdown" value="<?=$lang->getLang("Shut down the totem")?>" /></td>
        </tr>
        <tr>
            <td class="content"><img src="<?=image_dir?>/windows.png" width="20"
                                     height="20" /></td>
            <td><input type="submit" name="winsblock" value="<?=$lang->getLang("Unblock Windows")?>" /></td>
            <td><input type="submit" name="winblock" value="<?=$lang->getLang("Block Windows")?>" /></td>
        </tr>
    </table>

</form>
<?=showErrors()?>
<?=showResult();?>
