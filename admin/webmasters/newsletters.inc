<?php 
$action = $_REQUEST["action"];

define("SERVER_PATH", "/www/test/request_url.php");
define("SERVER_ADDR", "10.10.0.2");

switch ($action) {
  case "send":
    $ch = curl_init();

    $data_to_send = array(
		                    "subject" => $_POST["subject"],
												"body"    => $_POST["body"]
		                );

    $options = array(
    CURLOPT_URL            => "http://" . SERVER_ADDR . SERVER_PATH,
    CURLOPT_BINARYTRANSFER => true,
    CURLOPT_POST           => true,
    CURLOPT_POSTFIELDS     => $data_to_send,
    CURLOPT_RETURNTRANSFER => true,
    CURLOPT_CONNECTTIMEOUT => 5,
    CURLOPT_TIMEOUT        => 5,
    );

    curl_setopt_array($ch, $options);

    $output = curl_exec($ch);
    $info   = curl_getinfo($ch);

    if($info['http_code'] != 200){
    	echo "Problem connecting to " . SERVER_ADDR;
      $output = null;
    }
		echo "L'output di http://" . SERVER_ADDR . SERVER_PATH;
		var_dump($output);
		exit();

    // close cURL resource, and free up system resources
    curl_close($ch);
    echo $output;
    break;
}


?>
<h2>Invio newsletters</h2>
<script type="text/javascript" src="/media/nic/nicEdit.js">
</script>
<script type="text/javascript">
    bkLib.onDomLoaded(function(){
        nicEditors.allTextAreas()
    });
</script>
<style media="screen">
    
    #newsletter td {
        padding: 5px;
    }
    
    #newsletter {
        border: #CCC 2px solid;
    }
</style>
<form action="<?=refFormPage($_GET["page"] . "&action=send")?>" method="POST">

    <table cellpadding="4" cellspacing="1" border="0" id="newsletter">
        <tr>
            <td class="label" colspan="2">
                Nuovo messaggio della newsletter
            </td>
        </tr>
        <tr>
            <td class="label" width="70px">
                Oggetto
            </td>
            <td class="content">
                <input type="text" name="subject" size="50" style="font-size:14px"/>
            </td>
        </tr>
        <tr>
            <td colspan="2" class="label">
                Messaggio
            </td>
        </tr>
        <tr>
            <td colspan="2">
                <textarea name="body" cols="80" rows="30">
                </textarea>
            </td>
        </tr>
        <tr>
            <td colspan="2" align="center">
                <input type="submit" value="Invia Messaggio agli iscritti della newsletter"/>
            </td>
        </tr>
    </table>
</form>