<?


if (isset($_POST["extsys"]) && $_POST["extsys"] == 'true') {
    destroySession();
    exit();
} else {
    if ( isLoggedIn() )
    {	destroySession();
    header("Location: ".refPage());
    exit();
    }           
}


?><!--
<P align=center class=mesg>
You have been logged out.
<P align=center>
To login again, click <? printLink(refPage('login'),"Click here"); ?>&nbsp;&nbsp;&nbsp;
To close this window, click <? printLink("javascript:window.close()","Click here"); ?>&nbsp;&nbsp;&nbsp;
<P>-->