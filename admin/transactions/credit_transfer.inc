<?php
check_access('transactions_gateway',true);
define('TRANSFER_CREDIT', 3011);
include_once 'Error.class.inc';
$active=(isset($_POST["operation"])) ? $_POST["operation"] : 'history' ;
$date_start= (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d",time()-3600*24);
$date_end= (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d");
$operationtype = TRANSFER_CREDIT;
$skinid=SKINID;
$userip=getIpAddress();
$username=$_SESSION['admin_username'];
$md5=md5($skinid.$username.WEBAPP_KEY);
$datatoSend="md5=$md5&operationtype=$operationtype&skinid=$skinid&username=$username&userip=$userip&list=true&auid=".$_SESSION['admin_id'];
$processors=sendToWebApp($datatoSend,WEB_APP_WEBSITES);
if($processors<0){
    $error=new Error($processors);
    addError('',$error->checkError());
    showErrors();
}
else{
    list($dproc,$wproc)=explode("#",$processors);
}
$dproc=explode(';',$dproc);
if(strpos(strtolower($wproc),'bank transfer')===false && $wproc!=''){
   $wproc.=";BANK TRANSFER,bank_transfer";
}
$wproc=explode(';',$wproc);

?>
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap-responsive.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
    <style>
     .top{
            padding: 0px;
     }
     .error-popover{
            background-color: rgb(242, 222, 222);
            border-color: rgb(238, 211, 215);
            box-shadow: none;
            color: rgb(185, 74, 72);
            cursor: pointer;
            max-width: none;
            z-index: 1099;
     }
     .popover.error-popover .arrow {
            border-top-color: rgb(238, 211, 215);
            left: 30px;
     }
     .popover.top .arrow:after {
            bottom: 1px;
            margin-left: -10px;
            border-top-color: rgb(240, 228, 228);
            border-bottom-width: 0;
     }
     div.error {
         font-size: 11px;
         font-weight: normal;
         color: rgb(255, 0, 0);
         background-color: transparent;
         padding: 0px;
     }
    </style>
    <script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
    <script src="/media/jquery.alerts.js" type="text/javascript"></script>
    <script src="/media/jquery.dataTables.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/jquery.validate.js" type="text/javascript"></script>
    <script type="text/javascript">
        $.validator.addMethod("alphanumeric", function(value, element) {
            return this.optional(element) || /^[a-z 0-9\_ ]+$/i.test(value);
        }, "Use only letters, numbers, or underscore.");
        $(function() {
            $("#withdrawform").validate({
                rules: {
                    wp: {
                        required: true,
                        digits: true,
                        min:10
                    },
                    dpbankw: {
                        required: true,
                        alphanumeric:true
                    },
                    dpbicw: {
                        required: true,
                        alphanumeric:true
                    },
                    dpnamew: {
                        required: true,
                        alphanumeric:true
                    },
                    dpibanw: {
                        required: true,
                        alphanumeric:true,
                        iban:true
                    }
                },
                messages:{
                    wp:{
                        required: "<?=$lang->getLang("Please enter the amount")?>",
                        digits: " <?=$lang->getLang("Only numbers are allowed!")?>",
                        min:" <?=$lang->getLang("The minimum amount is 10 ".((isset($_SESSION['currency']) && $_SESSION['currency']!='') ? $_SESSION['currency'] : "euro" ))?>"
                    },
                    dpbankw:{
                        required: "<?=$lang->getLang("Please enter your bank")?>"
                    },
                    dpbicw:{
                        required: "<?=$lang->getLang("Please enter your bic")?>"
                    },dpnamew:{
                        required: "<?=$lang->getLang("Please enter your name and lastname")?>"
                    },
                    dpibanw:{
                        required: "<?=$lang->getLang("Please enter your IBAN")?>"
                    }
                },
                showErrors: function(errorMap, errorList) {
                    $.each( this.successList , function(index, value) {
                        $(value).popover('hide');
                        $(value).closest('.control-group').removeClass('error').addClass('success');
                        $(value).closest('.controls').prev().removeClass('label-inverse label-important').addClass('label-success');
                    });
                    $.each( errorList , function(index, value) {
                        var _popover = $(value.element).popover({
                            trigger: 'manual',
                            placement: 'top',
                            content: value.message,
                            template: '<div class="popover error-popover">' +
                                '       <div class="arrow">' +
                                '       </div>' +
                                '<div class="popover-inner  alert-error">' +
                                '<div class="popover-content  alert-error"><p></p></div></div></div>'
                        });
                        _popover.data('popover').options.content = value.message;
                        $(value.element).popover('show');
                        $(value.element).closest('.control-group').addClass('error');
                        $(value.element).closest('.controls').prev().removeClass('label-inverse').addClass('label-important');
                    });
                }
            });

            $("#dodeposit").validate({
                rules: {
                    dp: {
                        required: true,
                        digits: true,
                        min:10
                    },
                    dpbank: {
                        required: true,
                        alphanumeric:true
                    },
                    dpbic: {
                        required: true,
                        alphanumeric:true
                    },
                    dpname: {
                        required: true,
                        alphanumeric:true
                    },
                    dpiban: {
                        required: true,
                        iban:true
                    }
                },
                messages:{
                    dp:{
                        required: " <?=$lang->getLang("Please enter the amount")?>",
                        digits: "<?=$lang->getLang("Only numbers are allowed!")?>",
                        min:"<?=$lang->getLang("The minimum amount is 10 euro")?>"
                    },
                    dpbank:{
                        required: " <?=$lang->getLang("Please enter your bank")?>"
                    },
                    dpbic:{
                        required: " <?=$lang->getLang("Please enter your bic")?>"
                    },dpname:{
                        required: " <?=$lang->getLang("Please enter your name and lastname")?>}"
                    },
                    dpiban:{
                        required: " <?=$lang->getLang("Please enter your IBAN")?>"
                    }
                },
                showErrors: function(errorMap, errorList) {
                    $.each( this.successList , function(index, value) {
                        $(value).popover('hide');
                        $(value).closest('.control-group').removeClass('error').addClass('success');
                        $(value).closest('.controls').prev().removeClass('label-inverse label-important').addClass('label-success');
                    });
                    $.each( errorList , function(index, value) {
                        var _popover = $(value.element).popover({
                            trigger: 'manual',
                            placement: 'top',
                            content: value.message,
                            template: '<div class="popover error-popover">' +
                                '       <div class="arrow">' +
                                '       </div>' +
                                '<div class="popover-inner  alert-error">' +
                                '<div class="popover-content  alert-error"><p></p></div></div></div>'
                        });
                        _popover.data('popover').options.content = value.message;
                        $(value.element).popover('show');
                        $(value.element).closest('.control-group').addClass('error');
                        $(value.element).closest('.controls').prev().removeClass('label-inverse').addClass('label-important');

                    });
                }

            });

            $.validator.addMethod("iban", function(value, element) {
                var iban = value.replace(/ /g,'').toUpperCase(); // remove spaces and to upper case
                var countrycode = iban.substring(0,2);
                if(countrycode=='MT'){
                    $(".swift").hide();
                }
                else{
                    $(".swift").show();
                }
                if (this.optional(element)) {
                    return true;
                }
                if (!(/^([a-zA-Z0-9]{4} ){2,8}[a-zA-Z0-9]{1,4}|[a-zA-Z0-9]{12,34}$/.test(value))) {
                    return false;
                }

                var bbancountrypatterns = {
                    'AL': "\\d{8}[\\dA-Z]{16}",
                    'AD': "\\d{8}[\\dA-Z]{12}",
                    'AT': "\\d{16}",
                    'AZ': "[\\dA-Z]{4}\\d{20}",
                    'BE': "\\d{12}",
                    'BH': "[A-Z]{4}[\\dA-Z]{14}",
                    'BA': "\\d{16}",
                    'BR': "\\d{23}[A-Z][\\dA-Z]",
                    'BG': "[A-Z]{4}\\d{6}[\\dA-Z]{8}",
                    'CR': "\\d{17}",
                    'HR': "\\d{17}",
                    'CY': "\\d{8}[\\dA-Z]{16}",
                    'CZ': "\\d{20}",
                    'DK': "\\d{14}",
                    'DO': "[A-Z]{4}\\d{20}",
                    'EE': "\\d{16}",
                    'FO': "\\d{14}",
                    'FI': "\\d{14}",
                    'FR': "\\d{10}[\\dA-Z]{11}\\d{2}",
                    'GE': "[\\dA-Z]{2}\\d{16}",
                    'DE': "\\d{18}",
                    'GI': "[A-Z]{4}[\\dA-Z]{15}",
                    'GR': "\\d{7}[\\dA-Z]{16}",
                    'GL': "\\d{14}",
                    'GT': "[\\dA-Z]{4}[\\dA-Z]{20}",
                    'HU': "\\d{24}",
                    'IS': "\\d{22}",
                    'IE': "[\\dA-Z]{4}\\d{14}",
                    'IL': "\\d{19}",
                    'IT': "[A-Z]\\d{10}[\\dA-Z]{12}",
                    'KZ': "\\d{3}[\\dA-Z]{13}",
                    'KW': "[A-Z]{4}[\\dA-Z]{22}",
                    'LV': "[A-Z]{4}[\\dA-Z]{13}",
                    'LB': "\\d{4}[\\dA-Z]{20}",
                    'LI': "\\d{5}[\\dA-Z]{12}",
                    'LT': "\\d{16}",
                    'LU': "\\d{3}[\\dA-Z]{13}",
                    'MK': "\\d{3}[\\dA-Z]{10}\\d{2}",
                    'MT': "[A-Z]{4}\\d{5}[\\dA-Z]{18}",
                    'MR': "\\d{23}",
                    'MU': "[A-Z]{4}\\d{19}[A-Z]{3}",
                    'MC': "\\d{10}[\\dA-Z]{11}\\d{2}",
                    'MD': "[\\dA-Z]{2}\\d{18}",
                    'ME': "\\d{18}",
                    'NL': "[A-Z]{4}\\d{10}",
                    'NO': "\\d{11}",
                    'PK': "[\\dA-Z]{4}\\d{16}",
                    'PS': "[\\dA-Z]{4}\\d{21}",
                    'PL': "\\d{24}",
                    'PT': "\\d{21}",
                    'RO': "[A-Z]{4}[\\dA-Z]{16}",
                    'SM': "[A-Z]\\d{10}[\\dA-Z]{12}",
                    'SA': "\\d{2}[\\dA-Z]{18}",
                    'RS': "\\d{18}",
                    'SK': "\\d{20}",
                    'SI': "\\d{15}",
                    'ES': "\\d{20}",
                    'SE': "\\d{20}",
                    'CH': "\\d{5}[\\dA-Z]{12}",
                    'TN': "\\d{20}",
                    'TR': "\\d{5}[\\dA-Z]{17}",
                    'AE': "\\d{3}\\d{16}",
                    'GB': "[A-Z]{4}\\d{14}",
                    'VG': "[\\dA-Z]{4}\\d{16}"
                };
                var bbanpattern = bbancountrypatterns[countrycode];
                // As new countries will start using IBAN in the
                // future, we only check if the countrycode is known.
                // This prevents false negatives, while almost all
                // false positives introduced by this, will be caught
                // by the checksum validation below anyway.
                // Strict checking should return FALSE for unknown
                // countries.
                if (typeof bbanpattern !== 'undefined') {
                    var ibanregexp = new RegExp("^[A-Z]{2}\\d{2}" + bbanpattern + "$", "");
                    if (!(ibanregexp.test(iban))) {
                        return false; // invalid country specific format
                    }
                }

                // now check the checksum, first convert to digits
                var ibancheck = iban.substring(4,iban.length) + iban.substring(0,4);
                var ibancheckdigits = "";
                var leadingZeroes = true;
                var charAt;
                for (var i =0; i<ibancheck.length; i++) {
                    charAt = ibancheck.charAt(i);
                    if (charAt !== "0") {
                        leadingZeroes = false;
                    }
                    if (!leadingZeroes) {
                        ibancheckdigits += "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ".indexOf(charAt);
                    }
                }

                // calculate the result of: ibancheckdigits % 97
                var cRest = '';
                var cOperator = '';
                for (var p=0; p<ibancheckdigits.length; p++) {
                    var cChar = ibancheckdigits.charAt(p);
                    cOperator = '' + cRest + '' + cChar;
                    cRest = cOperator % 97;
                }
                return cRest === 1;
            }, "Please specify a valid IBAN");


            if(($("#proc").find('option:selected').text()).toLowerCase()=='bank transfer'){
                $('.bktrns').show();
            }
            else{
                $('.bktrns').hide();
            }

            $('#proc').change(function() {
                if(($(this).find('option:selected').text()).toLowerCase()=='bank transfer'){
                    $('.bktrns').show();
                }
                else{
                    $('.bktrns').hide();
                }
            });



            if(($('#wproc').find('option:selected').text()).toLowerCase()=='bank transfer'){
                $('.bktrnsw').show();
            }
            else{
                $('.bktrnsw').hide();
            }
            $('#wproc').change(function() {
                if(($(this).find('option:selected').text()).toLowerCase()=='bank transfer'){
                    $('.bktrnsw').show();
                }
                else{
                    $('.bktrnsw').hide();
                }
            });

            $( "#date_start" ).datepicker({
                changeMonth: true,
                changeYear:true,
                maxDate:-1,
                numberOfMonths:1
            });
            $( "#date_end" ).datepicker({
                changeMonth: true,
                changeYear:true,
                numberOfMonths:1
            });

            $('#historyTable').dataTable({
                "sPaginationType": "full_numbers"
            });
        });

        function autoResize(id){
            var newheight;
            var newwidth;

            if(document.getElementById){
                newheight=document.getElementById(id).contentWindow.document .body.scrollHeight;
                newwidth=document.getElementById(id).contentWindow.document .body.scrollWidth;
            }

            document.getElementById(id).height= (newheight) + "px";
            document.getElementById(id).width= (newwidth) + "px";
        }

    </script>
    <table class="table table-bordered">
    <tr><td class='navbar-inner'><h3 class="text-center"><?=$lang->getLang("Transactions")?></h3></td></tr>
    <tr>
    <td>
    <ul class="nav nav-tabs" id='myTab'>
        <li id='deposit_li' <?=($active=='deposit') ? "class='active'":""?>><a href="#deposit" data-toggle="tab"><button class="btn btn-success"><?=$lang->getLang("Deposit")?></button></a></li>
        <li id='withdraw_li' <?=($active=='withdraw') ? "class='active'":""?>><a href="#withdraw" data-toggle="tab"><button class="btn btn-success"><?=$lang->getLang("Withdraw")?></button> </a></li>
        <li id='history_li' <?=($active=='history') ? "class='active'":""?>><a href="#history" data-toggle="tab"><button class="btn btn-success"><?=$lang->getLang("Transactions")?></button> </a></li>
    </ul>
    <div class="tab-content" style="overflow: visible">
    <div class="tab-pane fade <?=($active=='deposit') ? "in active":""?>" id="deposit">
        <form action='/transactions_manage' method='post' id='dodeposit'>
            <?php
            /**
             * @author Marian
             */

            ?>
            <input type="hidden" name='operation' value='deposit' >
            <div class='well'>
                <table class="table-condensed"><tr><td>
                <div class='row'>
                    <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Select Method")?></div>
                        <br>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-random"></i></span>
                                <select name='proc' id='proc'>
                                    <?foreach($dproc as $k=>$v){
                                        list($pt,$pv)=explode(",",$v);
                                        if(strtolower($pt)=='bank transfer'){
                                            list($bkcmp,$bkibn,$bkname,$bkbic)=explode('~',$pv);
                                            $bkcmp=explode(':',$bkcmp);

                                            $bkibn=explode(':',$bkibn);

                                            $bkname=explode(':',$bkname);

                                            $bkbic=explode(':',$bkbic);

                                            $pv='bank_transfer';
                                        }

                                        ?>
                                        <option value='<?=$pv?>' ><?=$pt?></option>
                                    <? }?>
                                </select>
                            </div>
                        </div>
                    </div>
                </div>

                <div class='row'>
                    <div class="span3 specific_date control-group control-group"><div class='label label-inverse'><?=$lang->getLang("Amount")?></div>
                        <br>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-globe"></i></span>
                                <input type='number' min=10  name='dp'  value="<?=$_POST['dp']?>">
                            </div>
                        </div>
                    </div>
                    <div class="span3 bktrns control-group control-group"><div class='label label-inverse'><?=$lang->getLang("Bank")?></div>
                        <br>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-globe"></i></span>
                                <input type='text' name='dpbank'  >
                            </div>
                        </div>
                    </div>
                </div>
                <div class='row bktrns'>
                    <div class="span3 specific_date control-group control-group"><div class='label label-inverse'><?=$lang->getLang("Name,Lastname")?></div>
                        <br>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-globe"></i></span>
                                <input type='text' name='dpname'  value="<?=$_POST['dp']?>">
                            </div>
                        </div>
                    </div>
                    <div class="span3 control-group control-group swift"><div class='label label-inverse'><?=$lang->getLang("BIC ( SWIFT )")?></div>
                        <br>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-globe"></i></span>
                                <input type='text' name='dpbic'  >
                            </div>
                        </div>
                    </div>
                </div>
                <div class='row bktrns'>

                    <div class="span5 control-group control-group"><div class='label label-inverse'><?=$lang->getLang("IBAN")?></div>
                        <br>
                        <div class="controls">
                            <div class="input-prepend">
                                <span class="add-on"><i class="icon-globe"></i></span>
                                <input type='text' name='dpiban'  >
                            </div>
                        </div>
                    </div>
                </div>
                <button class="btn btn-primary" ><?=$lang->getLang("Deposit")?></button><br /><br />
                <br />
                <iframe name="result_iframe" id='result_iframe' onLoad="autoResize('result_iframe');" style="height: 425px;min-width: 600px;display:none;border:0px;" width="100%" height="100%" ></iframe><br>
                        </td><td>
                <div class="bktrns">
                    <table class='table'>
                        <tr class='bktrns success'>
                            <td><span class="label label-success"><?=$lang->getLang("Company")?> </span></td><td>  <?=$bkcmp['1']?></td>
                        </tr>
                        <tr class='bktrns error'>
                            <td>
                                <span class="label label-important"><?=$lang->getLang("Bank")?> </span></td><td>   <?=$bkname['1']?>
                            </td>
                        </tr>
                        <tr class='bktrns warning'>
                            <td>
                                <span class="label label-warning"><?=$lang->getLang("IBAN")?> </span></td><td>   <?=$bkibn['1']?>
                            </td>
                        </tr>
                        <tr class='bktrns info'>
                            <td>
                               <span class="label label-info"> <?=$lang->getLang("BIC( SWIFT )")?> </span></td><td>   <?=$bkbic['1']?>
                            </td>
                        </tr>
                    </table>


                </div>
                        </td></tr></table>
                </div>
        </form>
        <?if(isset($_POST['dp']) && ($_POST['dp'] >= 10))
        {
            $operationtype=TRANSFER_CREDIT;
            $amount=$_POST['dp']*100;
            $procid=$_POST['proc'];
            if($procid=='bank_transfer'){
                $ubd='name:'.$_POST['dpname']."~iban:".$_POST['dpiban']."~bank:".$_POST['dpbank']."~bic:".$_POST['dpbic'];
            }
            $md5=md5($skinid.$username.WEBAPP_KEY);
            $datatoSend="amount=$amount&paycode=$procid&md5=$md5&operationtype=$operationtype&skinid=$skinid&username=$username&userip=$userip&ubd=$ubd&auid=".$_SESSION['admin_id'];
            $response=sendToWebApp($datatoSend,WEB_APP_WEBSITES);

            if($response<0){
                $error=new Error($response);
                addError('',$error->checkError());
                showErrors();
            }
            else{
                if($response=='0')
                {
                    ?>
                    <script>
                        jAlert('You request has been received.','Success',function(answer){
                            if(answer){
                                $("#history form").submit();
                            }
                        });
                    </script>
                <?
                }
                else{
                list($responsestatus,$ptype,$url,$params)=explode("~",$response);
                ?>
                    <form id='doformpost' action="<?=$url?>" method='post' target="result_iframe">
                        <? if($ptype==3){ ?>
                        <script type="text/javascript">
                            $("#result_iframe").height("750");
                        </script>
                            <?=$params?>
                        <? } else { ?>
                        <input type='hidden' name='params' value='<?=$params?>'>
                        <? } ?>
                    </form>
                    <script type="text/javascript">
                        $("#doformpost").submit();
                        $("#result_iframe").show();
                    </script>
                <?
                }
            }
        }
        ?>
    </div>

    <div class="tab-pane fade <?=($active=='withdraw') ? "in active":""?>" id="withdraw">
        <form action='/transactions_manage' method='post' id='withdrawform'>
            <input type="hidden" name='operation' value='withdraw' >
            <div class='well'>
                <?
                if(empty($wproc[0])){
                    ?>
                    <div class="span3 alert alert-error"><?=$lang->getLang('You have to make at least one deposit before!')?></div><br /> <br />
                <?
                }
                else{
                    ?>
                    <div class='row'>
                        <div class="span3 control-group control-group"><div class='label label-inverse'><?=$lang->getLang("Select Method")?></div>
                            <br>
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-random"></i></span>
                                    <select name='wproc' id='wproc'>
                                        <?foreach($wproc as $k=>$v){
                                            list($pt,$pv)=explode(",",$v);
                                            if(strtolower($pt)=='bank transfer'){
                                                list($bkcmpw,$bkibnw,$bknamew,$bkbicw)=explode('~',$pv);
                                                $bkcmpw=explode(':',$bkcmpw);

                                                $bkibnw=explode(':',$bkibnw);

                                                $bknamew=explode(':',$bknamew);

                                                $bkbicw=explode(':',$bkbicw);

                                                $pv='bank_transfer';
                                            }
                                            ?>
                                            <option value='<?=$pv?>'<?=($_POST['wproc']==$pv ?'selected=selected' :'')?>><?=$pt?></option>
                                        <? }?>
                                    </select>
                                </div>
                            </div>
                        </div>
                    </div>
                    <div class='row'>
                        <div class="span3 control-group control-group"><div class='label label-inverse'><?=$lang->getLang("Amount")?></div>
                            <br>
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-globe"></i></span>
                                    <input type='number' min=10  name='wp'  value="">
                                </div>
                            </div>
                        </div>
                        <div class="span3 bktrnsw control-group control-group"><div class='label label-inverse'><?=$lang->getLang("Bank")?></div>
                            <br>
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-globe"></i></span>
                                    <input type='text'  name='dpbankw'  value="<?=$bknamew['1']?>">
                                </div>
                            </div>
                        </div>
                    </div>

                    <div class='row bktrnsw'>
                        <div class="span3 control-group control-group"><div class='label label-inverse'><?=$lang->getLang("Name,Lastname")?></div>
                            <br>
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-globe"></i></span>
                                    <input type='text' name='dpnamew'  value="<?=$bkcmpw['1']?>">
                                </div>
                            </div>
                        </div>
                        <div class="span3 control-group control-group swift"><div class='label label-inverse'><?=$lang->getLang("BIC ( SWIFT )")?></div>
                            <br>
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-globe"></i></span>
                                    <input type='text'  name='dpbicw'  value="<?=$bkbicw['1']?>">
                                </div>
                            </div>
                        </div>
                    </div>
                    <div class='row bktrnsw'>
                        <div class="span3 control-group control-group"><div class='label label-inverse'><?=$lang->getLang("IBAN")?></div>
                            <br>
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-globe"></i></span>
                                    <input type='text' style='width: 300px' name='dpibanw'  value="<?=$bkibnw['1']?>">
                                </div>
                            </div>
                        </div>
                    </div>
                    <button class="btn btn-primary" ><?=$lang->getLang("Withdraw")?></button>
                <?
                }
                ?>
            </div>
        </form>
        <?if(isset($_POST['wp']) && ($_POST['wp'] >= 10))
        {
            $operationtype=TRANSFER_CREDIT;
            $amount=-($_POST['wp']*100);
            $procid=$_POST['wproc'];
            if($procid=='bank_transfer'){
                $ubd='name:'.$_POST['dpnamew']."~iban:".$_POST['dpibanw']."~bank:".$_POST['dpbankw']."~bic:".$_POST['dpbicw'];
            }
            $md5=md5($skinid.$username.WEBAPP_KEY);
            $datatoSend="amount=$amount&paycode=$procid&md5=$md5&operationtype=$operationtype&skinid=$skinid&username=$username&userip=$userip&ubd=$ubd&auid=".$_SESSION['admin_id'];
            $response=sendToWebApp($datatoSend,WEB_APP_WEBSITES);
            if($response<0){
                $error=new Error($response);
                addError('',$error->checkError());
                showErrors();
            }
            else{
                if($response=='0')
                {
                    ?>
                    <script>
                        jAlert('You request has been received.Your transaction is now waiting approval!','Success',function(answer){
                            if(answer){
                                $("#history form").submit();
                            }
                        });
                    </script>
                <?
                }
            }
        }
        ?>
    </div>

    <div class="tab-pane fade <?=($active=='history') ? "in active":""?>" id="history">
        <?php form_head()?>
        <input type="hidden" name='operation' value='history' >
        <div class='well'>
            <div class='row'>
                <div class="span3 specific_date"><div class='label label-inverse'><?=$lang->getLang("From")?></div>
                    <br>
                    <div class="controls">
                        <div class="input-prepend">
                            <span class="add-on"><i class="icon-calendar"></i></span>
                            <input type='text'  name='date_start' id='date_start' value="<?=$date_start?>">
                        </div>
                    </div>
                </div>
                <div class="span3 specific_date"><div class='label label-inverse'><?=$lang->getLang("Until")?></div>
                    <br>
                    <div class="controls">
                        <div class="input-prepend">
                            <span class="add-on"><i class="icon-calendar"></i></span>
                            <input type='text'  name='date_end' id='date_end' value="<?=$date_end?>">
                        </div>
                    </div>
                </div>
            </div>
            <button class="btn btn-primary" ><?=$lang->getLang("Search")?></button>
            <br><br>
            <div>
                <?if(isset($_POST['form_submitted']) && ($active == 'history')){
                    searchAdminTransaction();
                }
                ?>
            </div>
        </div>
        </form>
    </div>
    </td>
    </tr>
    </table>

<?
function searchAdminTransaction(){
    $starttime = microtime(true);
    global $dbh,$lang,$date_start, $date_end;
    $searchType = $_POST["field"];
    $sql="Select att_time,
                att_note,
                att_transaction_id,
                IF (att_tty_id IS NOT NULL, (select tty_description from transaction_type where tty_id = att_tty_id), 'N') tty_description,
                IF (att_aus_id IS NOT NULL, (select aus_username from admin_user where aus_id = att_aus_id), 'N') aus_username,
                IF (att_aus_final_id IS NOT NULL, (select aus_username from admin_user where aus_id = att_aus_final_id), 'N') aus_final_username,
                IF (att_pun_id IS NOT NULL, (select pun_username from punter where pun_id = att_pun_id), 'N') pun_username,
                IF (att_jur_id IS NOT NULL, (select jur_name from jurisdiction where jur_id=att_jur_id), 'N') first_jurisdiction,
                IF (att_final_jur_id IS NOT NULL, (select jur_name from jurisdiction where jur_id=att_final_jur_id), 'N') second_jurisdiction,
                att_amount,
                att_overdraft,
                'A' status,
                att_jur_avail_credit,
                att_jur_final_avail_credit,
                att_jur_avail_overdraft,
                att_jur_final_avail_overdraft,
                IF (att_final_jur_id IS NOT NULL, (select cur_code_for_web from currency join jurisdiction on jur_currency=cur_id where jur_id=att_final_jur_id), (select cur_code_for_web from currency join jurisdiction on jur_currency=cur_id where jur_id=att_jur_id)) cur_code_for_web
                FROM admin_user_transaction
                WHERE  1=1 ";

    $sql .= " AND att_aus_id  IN (SELECT aus_id FROM admin_user where aus_username=".$dbh->prepareString($_SESSION['admin_username']).")";
    if($searchType != 3){
        $sql .= " AND att_time BETWEEN '$date_start 00:00:00' AND '$date_end 23:59:59'";
    }

    $sql.=" UNION ALL
                    SELECT ptr_start att_time,
                            ptr_note att_note,
                            ptr_uniqueid att_transaction_id,
                            (select tty_description from transaction_type where tty_id = ptr_tty_id) tty_description,
                            IF (ptr_admin_user_id IS NOT NULL, (select aus_username from admin_user where aus_id = ptr_admin_user_id), 'N') aus_username,
                            'N' aus_final_username,
                            'N' pun_username,
                            ptr_ppc_code first_jurisdiction,
                            'N' second_jurisdiction,
                            ptr_amount att_amount,
                            0 att_overdraft,
                            ptr_status status,
                            ptr_available_credit att_jur_avail_credit,
                            0 att_jur_final_avail_credit,
                             ptr_available_overdraft att_jur_avail_overdraft,
                             0 att_jur_final_avail_overdraft,
                            '".$_SESSION['currency_html']."' cur_code_for_web
                            FROM processor_transaction
                             WHERE
                               (ptr_status != 'A')

                              ";

    $sql .= " AND ptr_admin_user_id  = (SELECT aus_id FROM admin_user where aus_username=".$dbh->prepareString($_SESSION['admin_username']).")";
    if($searchType != 3){
        $sql .= " AND ptr_start >= '$date_start 00:00:00' AND ptr_end <= '$date_end 23:59:59'";
    }
    if(!areErrors()){
        $return="<table class='display table table-striped table-bordered table-hover table-condensed' id='historyTable'><thead>";
        $return.="<tr><th style='white-space: nowrap;'>".$lang->getLang('Time')."</th>
                        <th style='white-space: nowrap;'>".$lang->getLang('Transaction Type')."</th>
                        <th style='white-space: nowrap;'>".$lang->getLang('Admin')."</th>
                        <th style='white-space: nowrap;'>".$lang->getLang('Transaction Id')."</th>
                        <th style='white-space: nowrap;'>".$lang->getLang('Amount')."</th>
                        <th style='white-space: nowrap;'>".$lang->getLang('Overdraft')."</th>
                        <th style='white-space: nowrap;'>".$lang->getLang('From')."</th>
                        <th style='white-space: nowrap;'>".$lang->getLang('Available Credit')."</th>
                        <th style='white-space: nowrap;'>".$lang->getLang('Available Overdraft')."</th>
                        <th style='white-space: nowrap;'>".$lang->getLang('To')."</th>
                        <th style='white-space: nowrap;'>".$lang->getLang('Available Credit')."</th>
                        <th style='white-space: nowrap;'>".$lang->getLang('Available Overdraft')."</th>
                        <th>".$lang->getLang('Note')."</th>
                  </tr></thead>";
        $rs=$dbh->doCachedQuery($sql,0);
        while ($rs->hasNext()){
            $row=$rs->next();
            $return.="<tr><td>".$row['att_time']."</td>
                        <td>".$row['tty_description']."</td>
                        <td>".$row['aus_username']."</td>
                        <td>".$row['att_transaction_id']."</td>
                        <td>".getInDollars($row['att_amount'],2,$row['cur_code_for_web']);

            $return.="<br> <span ";

            if($row['status']=='A'){
                $return .='class="result" >Accepted ';
            }
            elseif ($row['status']=='D'){
                $return .='class="error">'.$lang->getLang("Declined") ;
            }
            elseif($row['status']=='P'){
                $return.= 'class="pending">Pending ';
            }
            $return.="</span></td>
                        <td>".getInDollars($row['att_overdraft'],2,$row['cur_code_for_web'])."</td>
                        <td>".$row['first_jurisdiction']."</td>
                        <td>Before:".(($row["first_jurisdiction"]=="casino" ) ? " Unlimited" : getInDollars($row["att_jur_avail_credit"],2,$row['cur_code_for_web']))."<br / >After:&nbsp;&nbsp;&nbsp;".(($row["first_jurisdiction"]=="casino" ) ? " Unlimited" :getInDollars($row["att_jur_avail_credit"] - $row["att_amount"],2,$row['cur_code_for_web'] ))."</td>";
            $return.="<td>Before:".(($row["first_jurisdiction"]=="casino" ) ? " Unlimited" : getInDollars($row["att_jur_avail_overdraft"],2,$row['cur_code_for_web']))."<br / >After:&nbsp;&nbsp;&nbsp;".(($row["first_jurisdiction"]=="casino" ) ? " Unlimited" :getInDollars($row["att_jur_avail_overdraft"] - $row["att_overdraft"],2,$row['cur_code_for_web'] ))."</td>";
            $return.="<td>".$row['second_jurisdiction']."</td>";
            $return.="<td>Before:".(($row["second_jurisdiction"]=="casino" ) ? " Unlimited" : getInDollars($row["att_jur_final_avail_credit"],2,$row['cur_code_for_web']))."<br / >After:&nbsp;&nbsp;&nbsp;".(($row["second_jurisdiction"]=="casino" ) ? " Unlimited" :getInDollars($row["att_jur_final_avail_credit"] + $row["att_amount"],2,$row['cur_code_for_web'] ))."</td>";
            $return.="<td>Before:".(($row["second_jurisdiction"]=="casino" ) ? " Unlimited" : getInDollars($row["att_jur_final_avail_overdraft"],2,$row['cur_code_for_web']))."<br / >After:&nbsp;&nbsp;&nbsp;".(($row["second_jurisdiction"]=="casino" ) ? " Unlimited" :getInDollars($row["att_jur_final_avail_overdraft"] + $row["att_overdraft"],2,$row['cur_code_for_web'] ))."</td>";
            $return.="<td>".$row['att_note']."</td>
                    </tr>";
        }
        $return.="</table>";

        die($return);
    }
    else{
        showErrors();
    }
}
?>