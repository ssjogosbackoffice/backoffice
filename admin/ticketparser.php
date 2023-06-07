<?php
/**
 * Created by PhpStorm.
 * User: marian
 * Date: 18.12.2017
 * Time: 12:28
 */

require_once 'BettingTicketParser.php';


if(!isset($_POST['tkinfo'])){

    die('Invalid xml ');
}
$ticket=new BettingTicketParser($_POST['tkinfo']);
die($ticket->updateTicket()==1?'Success':'Error');
//die($ticket->updateTicket());


?>