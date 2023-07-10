<?php /** MODIFICATO MANUEL RINALDI **/
header('Content-type: text/html; charset=UTF-8');
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
    <title><?=$title?></title>
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/style.css" title="core_style" />
    <link rel="Stylesheet" type="text/css" href="<?= secure_host ?>/media/jquery-ui.css" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/fancybox/source/jquery.fancybox.css" >
   <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media//select2/select2.css" >
   <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/js/jAlert/jAlert.css" >

    <script src="<?= secure_host ?>/media/jquery.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/fancybox/source/jquery.fancybox.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/jquery.min.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/ui/js/jquery-ui-1.9.0.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/jfunctions.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/stm31.js" type="text/javascript"></script>

    <script type="text/javascript" src="<?= secure_host ?>/media/jquery-migrate-1.2.1.min.js" ></script>
    <script src="<?= secure_host ?>/media/ui/js/jquery.timepicker.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/select2/select2.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/jquery.alerts.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/js/jAlert/jAlert.min.js" type="text/javascript"></script>
    <?php
    if (!empty($more_headers)) {
        print $more_headers;
    }
    // query string (minus language argument)
    $qs = mb_ereg_replace('[&]*set_language=[^&]*', '', $_SERVER['QUERY_STRING']);
    // if ( $head_include ) {
    //     require_once ($head_include);
    // }
    global $lang;
    ?>
</head>

<body style="background-color:white" leftmargin="0" topmargin="0" rightmargin="0" bottommargin="0" marginwidth="0" marginheight="0"
    <?php

    if ( !empty($onload) ) {
        $onload_code = "$onload;";
    }
    if (isset($_SESSION['restrict_access_msg']) && $_SESSION['restrict_access_msg']) {
        unset($_SESSION['restrict_access_msg']);
        $jsmsg = mb_ereg_replace("'", "\'", "You do not have the proper privileges to access the section you requested");
        $onload_code .= "jAlert('".$jsmsg."')";
    } else {
        $jalert = isset($alert) ? $alert : (isset($_SESSION['alert_message']) ? $_SESSION['alert_message'] : '');
    
        if (!empty($jalert)) {
            $onload_code .= "jAlert('".$jalert."')";
            unset($_SESSION['alert_message']);
        }
    }
    // if ( $_SESSION['restrict_access_msg'] ){
    //     unset ($_SESSION['restrict_access_msg']);
    //     $jsmsg = mb_ereg_replace("'", "\'", "You do not have the proper privileges to access the section you requested");
    //     $onload_code .= "jAlert('".$jsmsg."')";
    // } else {
    //     $jalert  = empty($alert) ? $_SESSION['alert_message'] : $alert;

    //     if ( !empty($jalert) ) {
    //         $onload_code .= "jAlert('".$jalert."')";
    //         unset ($_SESSION['alert_message']);
    //     }
    // }

    if ( !empty($onload_code) )
    {   echo 'onload ="'.$onload_code.'"';
    }
    ?>>
<div>
    <table class="headerTable" cellpadding=0 cellspacing=0 border=0	 width="100%">
        <tr>
            <td colspan=3 bgcolor="#000000"><img src="<?=image_dir;?>/clear.gif" width=1 height=10></td>
        </tr>
        <tr height=60 valign=top>
            <!-- DEFAULT: href="/index.php?page=lastaccess" -->
            <td width=30%><a href="index.php?page=lastaccess"><img src="<?=image_dir?>/header_logo_seclev.gif"></a></td>
            <td width="40%"> <div style="text-align: center;font-size: 25px;color:orange;text-decoration: none">
                    <?php
                        // the logoUrl's site doesn't exists
                        $logoUrl=$dbh->queryOne("Select skn_media_url from skins where skn_id=".SKINID);
                        if(@get_headers($logoUrl."/main/site/media/images/logo_small.png")[0]!='HTTP/1.1 404 Not Found') {
                    ?>
                            <a href="/"><img src="<?=$logoUrl?>/main/site/media/images/logo_small.png" style="height: 60px"></a>
                        <?php } ?>
                </div>
            </td>
            <td width="30%">
                <table cellpadding=0 cellspacing=0 border=0 width="100%">
                    <tr>
                        <td bgcolor="#000000" ><img src="<?=image_dir;?>/clear.gif" width=1 height=14></td>
                        <td style="vertical-align: top">

                                <div class="fright"  style="color:white">
                                    <span style="font-style:italic"><?=$lang->getLang("Server Time")?>:</span>
                                    <span>
                                        <? 
                                        echo date('j/m/Y H:i:s');
                                        ?>
                                    </span>
                                </div>
                        </td>
                    </tr>
                    <tr>
                        <td bgcolor=black>
                            <div style="text-align: center;font-size: 25px;color:orange;text-decoration: none">




                            <?/*if(defined('WEB_SITE_URL') && isset($_SESSION['jurisdiction_id'])) { */?><!--
                                    <a href="http://<?/*=WEB_SITE_URL*/?>" style="font-size: 25px;color:white;text-decoration: none" target="_blank"><?/*=WEB_SITE_URL*/?></a>
                            --><?/* }*/?>&nbsp;</div>
                        </td>
                        <td style="background-color: black;width: 30%">
                            <?if(isset($_SESSION['admin_id'])){ ?>
                                <div class="fright">
                                    <a style="vertical-align: bottom" id="logoutBtn" href="<?=refPage('logout');?>">Logout  <img src='<?=image_dir?>/shutdown.png'   width='20px'    height='20px' style="vertical-align: bottom" alt="LOGOUT"/></a>
                                </div>
                            <? } ?>
                        </td>
                    </tr>
                    <tr><td colspan="3" style="vertical-align: bottom;width: 100%">
                            <div  style="float:right;background-color:black;color:white;font-size:8pt;font-weight:bold"><?=$lang->getLang("Language")?>:&nbsp;
                                <?php
                                if(is_array($site_avail_langs)) {
                                    foreach($site_avail_langs as $lang_name => $lang_code) {
                                        ?>
                                        <a href="<?=SELF;?>?<?=$qs?>&set_language=<?=$lang_code?>"><img src="<?=image_dir?>/flag_<?=strtolower($lang_code)?>.gif" alt="<?=replace_quotes($lang_name)?>" border="0" align="absmiddle"></a>
                                    <?php
                                    }
                                }
                                ?>
                            </div>
                        </td>
                    </tr>
                </table>
            </td>
        <tr>
    </table>
</div>
<div id="main_menu">
    <table cellpadding=0 cellspacing=0 border=0 width="100%">
        <tr>
            <td>
                <?
                include('menubar.inc'); ?>
            </td>
            <td style="text-align: right;width: auto">
                <?
                if(MENU_MESSAGES==1){
                    if(isset($_SESSION['admin_id'])){
                        $messages = checkMessages();
                        $found    = 0;
                        $found_id = 0;
                        if(count($messages)>0){
                            foreach($messages as $message){
                                if($message["aum_level"] >= 1 && $message["aum_level"] > $found){
                                    $found    = $message["aum_level"];
                                    $found_id = $message["aum_id"];
                                }
                            }
                        }
                        if(isset($_REQUEST["page"]) && $_REQUEST["page"] != "message/manager"){
                            if($found == 1){
                                if(!$_SESSION["just_logged"]){
                                    jAlert("Controlla la lista messaggi, ci sono dei messaggi importanti per te!");
                                    $_SESSION["just_logged"] = true;
                                }
                            }elseif($found == 2){
                                ?>
                                <script type="text/javascript">
                                    alert("Devi leggere il seguente messaggio per poter continuare.");
                                    location.href = "<?=refPage('message/manager&action=read&id=' . $found_id)?>";
                                </script>
                            <?php
                            }
                        }

                        if(count($messages) > 0){
                            ?>
                            <span id="nav">
           <a href="<?=refPage('message/manager')?>"><?=count($messages)?> <img src="<?=image_dir?>/email.gif" />&nbsp;</a>
           </span>
                        <?
                        }else{
                            ?>
                            <span id="nav">
           <a href="<?=refPage('message/manager')?>"><?=count($messages)?> <img src="<?=image_dir?>/email_read.gif" />&nbsp;</a>
           </span>
                        <?
                        }
                    }
                }
                ?>

            </td>
            <?php if(isset($SESSION['jurisdiction_class']) && $_SESSION['jurisdiction_class'] == 'club'){?>
                <td style="font-size:15pt">
                    <span style="font-style:italic"><?=$lang->getLang("Totem Code")?>:</span>
                    <span style="color:blue"><? echo $_SESSION['totem_code'];?></span>
                </td>
            <?php }?>

            <?php if(isLoggedIn()) {?>
                <td align=right style="width:auto" class="fright">
                    <table>
                        <tr>
                            <?
                            if(isset($_SESSION['currencies']))
                            {
                                $currency=$_SESSION['currencies'];
                            }
                            if(isset($currency) && check_access('view_admin_currencies', false)){
                            ?>
                            <td>
                                <script type="text/javascript">
                                    $(function(){

                                        $('.currencyImg').click(function() {
                                            img=$(this);
                                            currSrc=$(this).attr("src");
                                            var src = currSrc.replace("icon_active.gif", "loader.gif");
                                            $(this).attr("src", src);
                                            prev=$(this).prev();
                                            curVal=prev.val();
                                            curName=prev.attr('name');

                                            $.post("/services/general_services.inc",{action:"12",curName:curName,curVal:curVal}, function(data){
                                                //window.location.replace(window.location);
                                                    $('.resubmitForm').filter(':visible').submit();
                                                img.attr("src", currSrc);
                                            });
                                        });

                                        $('.refreshCurrency').click(function() {
                                            prev=$(this).siblings( "input[type=text]");
                                            curName=prev.attr('name');
                                            $.post("/services/general_services.inc",{action:"13",curName:curName}, function(data){
                                                prev.fadeOut().fadeIn().val(data);
                                                $('.resubmitForm').filter(':visible').submit();
                                            });
                                        });

                                        var container = $("#currencyContainer");
                                        $('#currencyIcon').click(function() {
                                            $('#creditContainer').hide();
                                            if(container.is(":hidden")){
                                                container.fadeIn("slow");
                                                $("#creditInfoArrow").html("&#9650;");
                                            }
                                            else{
                                                container.fadeOut("slow");
                                                $("#creditInfoArrow").html("&#9660;");
                                            }
                                        });
                                        $(document).mouseup(function (e)
                                        {
                                            if (!container.is(e.target) && container.has(e.target).length === 0)
                                            {
                                                container.fadeOut("slow");
                                            }
                                        });

                                    });
                                </script>
                                <div id="currencyIcon">
                                    <table cellpadding=0 cellspacing=0 border=0 >
                                        <tr><td> <img src="<?=image_dir?>/currency.png" style="width: 25px;float: left" alt="Currency"  /></td>
                                            <td><div id="currencyInfoArrow">&#9660;</div></td>
                                        </tr>
                                    </table>
                                </div>
                                <div class="fleft" id="currencyContainer">
                                    <span><table cellpadding=0 cellspacing=0 border=0 >
                                        <?
                                        $curDateUpd=0;
                                        foreach($currency as $k=>$v){
                                            if($curDateUpd==0){ ?>
                                            <tr><td colspan="2" style="text-align: center">Last Date : <?=$v['lastUpd']?></td> </tr>
                                            <?
                                            }
                                            ?>
                                            <tr><td><span  title="<?=$_SESSION['currency']?>" style="cursor: pointer"><?=$_SESSION['currency_html']?></span> / <span style="cursor: pointer" title="<?=$v['name']?>"><?=$v['symbol']?></span></td>
                                                <td><input class="currencyInput" type="text" name="<?=$k?>" value="<?=$v['value']?>"><img class="currencyImg" src='<?=image_dir?>/icon_active.gif' width='20px' style="vertical-align: middle;cursor:pointer"   /><img src="<?=image_dir?>/refresh.png" width="18px"  style="vertical-align: middle;cursor:pointer" class="refreshCurrency"></td>
                                            </tr>
                                        <?
                                            $curDateUpd++;
                                        }
                                        ?>
                                            </table>
                                    </span>

                                </div>
                                <? } ?>
                            </td>
                                <td style="font-size: 8pt;text-align: right" >
                                    <script type="text/javascript">
                                        $(function(){
                                            $('#userIcon').click(function() {
                                                $('#currencyContainer').hide();
                                                if($("#creditContainer").is(":hidden")){
                                                    $("#creditContainer").fadeIn("slow");
                                                    $("#userInfoArrow").html("&#9650;");
                                                }
                                                else{
                                                    $("#creditContainer").fadeOut("slow");
                                                    $("#userInfoArrow").html("&#9660;");
                                                }
                                            });
                                            $('#creditContainer').hover(function () {
                                                $("#creditContainer").stop(true,true).delay(2000).slideDown(1000);
                                                $("#userInfoArrow").html("&#9650;");
                                            }, function () {
                                                $("#creditContainer").stop(true,true).delay(2000).slideUp(1000);
                                                $("#userInfoArrow").html("&#9660;");
                                            });

                                        });
                                    </script>
                                    <div id="userIcon">
                                        <table cellpadding=0 cellspacing=0 border=0 >
                                            <tr><td><img src="<?=image_dir?>/user.png" style="width: 15px;height: 15px;float: left" alt="User"  /></td>
                                                <td><div id="userInfoArrow">&#9660;</div></td>
                                            </tr>
                                        </table>
                                    </div>
                                    <div class="fleft" id="creditContainer">
                                        <span><?=$lang->getLang("Username")?>:</span> <span style="font-weight: bold"><?=$_SESSION['admin_username'];?></span> &nbsp;&nbsp;<br />
                                        <span><?=$lang->getLang("User Type")?>:</span> <span style="font-weight: bold"><?=$_SESSION['aty_code'];?></span> &nbsp;&nbsp;
                                        <?php if(isLoggedIn()) {
                                        $jurisdiction_credit = getInDollars($_SESSION['jurisdiction_credit']);
                                        $jurisdiction_overdraft = getInDollars($_SESSION['jurisdiction_overdraft']);
                                        if(isset($_SESSION['jurisdiction_overdraft_received']))
                                        {
                                            $jurisdiction_overdraft_received =getInDollars($_SESSION['jurisdiction_overdraft_received']);
                                        }  else {
                                            $jurisdiction_overdraft_received = getInDollars(null);
                                        }
                                        $jurisdiction_pending = getInDollars($_SESSION['jurisdiction_pending']);
                                        if (!check_access('view_admin_current_credit', false)) {
                                            $jurisdiction_credit = getInDollars(0);
                                            $jurisdiction_overdraft = getInDollars(0);
                                            $jurisdiction_overdraft_received = getInDollars(0);
                                            $jurisdiction_pending = getInDollars(0);
                                        }?>
                                         <br /><br /><span><?=$lang->getLang("Credit")?>:</span> <span style="font-weight: bold" id='jurisdiction_credit'><?=$jurisdiction_credit;?></span> &nbsp;&nbsp;<br />
                                        <span><?=$lang->getLang("Overdraft")?>:</span> <span style="font-weight: bold" id='overdraft_credit'><?=$jurisdiction_overdraft;?></span><br />
                                        <span><?=$lang->getLang("Tot Ov. Received")?>:</span> <span style="font-weight: bold" id='overdraft_credit_received'><?=$jurisdiction_overdraft_received;?></span><br />
                                        <span><?=$lang->getLang("Pending")?>:</span> <span style="font-weight: bold" id='pending_credit'><?=$jurisdiction_pending;?></span>

                        <span class="fright">
                            <a id="refresh" href="javascript:getCredits();" class="span-1" onclick="if(!this.clicked){this.clicked = true; setTimeout('document.getElementById(\'refresh\').clicked = false;' ,10000); return true;} return false;">
                                <img src='<?=image_dir?>/refresh.png'   width='15px'    height='15px' style="vertical-align: bottom" />
                            </a>
                        </span><?php } ?>
                                    </div>
                                </td>


                            <td>
                    <?if(MENU_HELP==1){ ?>
                        <span><a class="small" href="index.php?page=message/livehelp"><img src='<?=image_dir?>/help.png'  width='15px'    height='15px' style="vertical-align: bottom" /><?=$lang->getLang('Help')?></a></span>
                    <? } ?>
                 |
                    <a class="small" href="<?=refPage('change-password');?>"><?=$lang->getLang("Change Password")?> </a></td></tr>
                    </table>
                </td>
            <?php } ?>
        </tr>
    </table>
</div>
<table cellpadding=0 cellspacing=0 border=0 width="100%">
    <tr>
        <td align="center">
            <table cellpadding=10 cellspacing=0 border=0>
                <tr>
                    <td align="left"><br /><br />