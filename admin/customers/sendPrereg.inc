<?php
$pun_customer_number = $_GET["id"];
settype($pun_customer_number, "integer");

$punter = getPunter($pun_customer_number, true);

if(!$punter){
    addError("", "Account not found or you do not have the proper privileges to view the account");
}else{
    $jurisdiction_id    = $_SESSION["jurisdiction_id"];
    $jurisdiction_class = $_SESSION["jurisdiction_class"];
    $skin = getSkinDetailsByJurisdiction($jurisdiction_id, $jurisdiction_class);

    $skin_name       = $skin["skn_name"];
    $skin_url        = $skin["skn_url"];

    $first_name      = $punter["first_name"];
    $last_name       = $punter["last_name"];

    $preg_code       = $punter["preg_code"];
    $email_reg_code  = $punter["email_reg_code"];

    $pun_id          = $punter["id"];
    $email           = $punter["email"];
    $username        = $punter["username"];

    $activation_link = getLink("$skin_url/accounts/Activation?p=$preg_code", "$skin_url/accounts/Activation?p=$preg_code");
    $customer_number = $punter["pun_customer_number"];

    $pins            = "PIN1: $preg_code PIN2: $email_reg_code";

    /*
    $msg = "Ciao <i>$first_name</i> <i>$last_name</i>, benvenuto su $skin_name<br/><br/>
            Il tuo numero cliente da comunicare all'assistenza per qualsiasi problema &egrave; <b>$customer_number</b><br/><br/>
            Per attivare il tuo account $skin_name devi utilizzare quando richiesto dal sito il codice riportato in basso:<br/><br/><code>
            <b>$pins</b></code><br/><br/>oppure devi cliccare sul link seguente:<br/><br/>$activation_link<br/><br/>
            se il tuo client di posta elettronica non supporta i link,
            copia l'indirizzo qui sotto e incollalo nel browser (Internet Explorer o Mozilla Firefox).<br/>$activation_link<br/>
            <br/>  -------------------<br/><br/>Ti invitiamo a non rispondere a questa email, in quanto questa è solamente un
            indirizzo email di servizio,<br/><br/>per qualsiasi problema non esitare a contattare l'assistenza ai seguenti indirizzi email o tramite skype.<br/><br/>Email<br/><ul>";
    */

    $msg = "Hi <i>$first_name</i> <i>$last_name</i>, welcome to $skin_name<br/><br/>Your [% skin_name%] username is $email.<br/><br/>If you experience any problem please contact our customer support giving your customer number :  <b>$customer_number</b><br/><br/>To activate your account $skin_name you should use the code provided below: <br/><br/><b>$pins</b><br/><br/>or just clicking on the following link:<br/><br/>$activation_link<br/><br/>or copy this url in your browser:<br/>$activation_link<br/><br/> -------------------<br/><br/>Please do not reply to this email, this is only a service email address.<br/><br/>for any issue feel free to contact our support center at the following email addresses or via skype.<br/><br/>Email<br/><ul>";

    $domain = isolateDomainName($skin_url);
    $emailSent = sendEmail("no-reply@$domain", $email, "Benvenuto su " . $skin_name, $msg);


    if($emailSent){
        printInfoMessage("Email inviata correttamente");
        $log  = "SendPregMail:$email";
        doAdminUserLog($_SESSION['admin_id'], 'pre-register_customer', escapeSingleQuotes($log), $pun_id);
        addPunterNote( $pun_id, $_SESSION['admin_id'], "Pre-register email re-send");
    }else{
        addError("", "Problem sending email");
    }
    $_GET["customer_id"] = $pun_id;
    include(include_content('customers/customer_view.inc'));
}
?>