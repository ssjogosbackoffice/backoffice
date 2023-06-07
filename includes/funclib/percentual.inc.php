<script type="text/javascript">
    function showPercOptions(optiontoshow){

        $('.hidden').hide();
        $('.hidden').find('input:text').attr("disabled",  "disabled");
        $("#"+optiontoshow).show().css('visibility','visible');
        $("#"+optiontoshow).find('input:text').removeAttr("disabled");

        liveoption=parseInt(optiontoshow)+4;
        if(parseInt(optiontoshow)==9){
            liveoption=10;
        }
        if(parseInt(optiontoshow)==11){
            liveoption=12;
        }
        $('.hidden_live').hide();

        $('.hidden_live').find('input:text').attr("disabled",  "disabled");
        $("#"+liveoption).show();
        $("#"+liveoption).find('input:text').removeAttr("disabled");
        $("#betting_live").val(liveoption);
        $("#betting_live_select").val(liveoption);
    }
/*    function showLivePercOptions(optiontoshow){
        $('.hidden_live').hide();
        // $('.hidden').find('input:text').val('');
        $('.hidden_live').find('input:text').attr("disabled",  "disabled");
        $("#"+optiontoshow).show();
        $("#"+optiontoshow).find('input:text').removeAttr("disabled");
    }*/



    function checkInsertedPercs(){
        percType=$('#betting').val();
        currentVal=$('#tcommParams').val();
        consoel.log(currentVal);
        if(currentVal!=''){
            $('.comissionOverlay').show();
        }
        else{
            $('.comissionOverlay').hide();
        }
        currentVal=currentVal.split(",");
        if(percType==2){
            corLength=30;
        }
        else{
            corLength=31;
        }

        if(currentVal.length!=corLength && $('#tcommParams').val()!=''){
            $('#tcommParams').css('border-color','red');
            $('#tcommParamsError').html("Invalid number of parameters,"+(corLength-currentVal.length)+' more missing').show();
        }
        else if($('#tcommParams').val()!=''){
            $('#tcommParams').css('border-color','green');
            $('#tcommParamsError').hide();
            for (key in currentVal) {
                pos = parseInt(key) + 2;
                if(currentVal[key]>50){
                    $('#tcommParams').css('border-color','red');
                    $('#tcommParamsError').html("Params value should be smaller than 50").show();
                }
                else {

                    if (percType == 2) {
                        $('#2 table tr:nth-child(' + pos + ') td:nth-child(2) input').val(currentVal[key]);
                    }
                    else {
                        $('#4 table tr:nth-child(' + pos + ') td:nth-child(2) input').val(currentVal[key]);
                    }
                }
            }
        }
        else{
            $('#tcommParams').css('border-color','grey');
            $('#tcommParamsError').hide();
        }

    }


    function setPercsValues(){

        commType=[];
        if($('#betting').val()==2) {
            $('.tparams').show();
            commType = $('#2 input[name^=commissions]').map(function (idx, elem) {
                return $(elem).val();
            }).get();
        }
        else if($('#betting').val()==4){
            $('.tparams').show();
             commType = $('#4 input[name^=monthly]').map(function (idx, elem) {
                return $(elem).val();
            }).get();
            commType.unshift($('#bonusmensile').val());
        }
        else{
            $('.tparams').hide();
        }
        $('#tcommParams').val(commType.join(','));
        if($('#tcommParams').val()!=''){
            $('.comissionOverlay').show();
        }
    }



    $(document).ready(function(){
        setPercsValues();

        $("#tcommParams, #feedtax").keydown(function (e) {
            // Allow: backspace, delete, tab, escape, enter and .
            if ($.inArray(e.keyCode, [46, 8, 9, 27, 13, 110,188, 190]) !== -1 ||
                    // Allow: Ctrl+A, Command+A
                (e.keyCode == 65 && ( e.ctrlKey === true || e.metaKey === true ) ) ||
                    // Allow: home, end, left, right, down, up
                (e.keyCode >= 35 && e.keyCode <= 40)) {
                // let it happen, don't do anything
                if(($(this).attr("id"))=='feedtax' && e.keyCode == 188 ){
                    e.preventDefault();
                }else
                {
                    return;
                }
            }
            // Ensure that it is a number and stop the keypress
            console.log(($(this).attr("id"))=='feedtax');
            console.log(e.keyCode == 188 );
            if ((e.shiftKey || (e.keyCode < 48 || e.keyCode > 57)) && (e.keyCode < 96 || e.keyCode > 105) || (($(this).attr("id"))=='feedtax' && e.keyCode == 188 )) {
                e.preventDefault();
            }
        });
        showPercOptions( document.getElementById("betting").value);
        $('#betting').on('change',function(){
            setPercsValues();
        });
        $('#tcommParams').on('blur',function(){
            checkInsertedPercs();
        });
       // showLivePercOptions( document.getElementById("betting_live").value)
    });

    function validate(evt) {
        var theEvent = evt || window.event;
        var key = theEvent.keyCode || theEvent.which;
        key = String.fromCharCode( key );
        var regex = /[0-9]/;
        if( !regex.test(key) ) {
            //theEvent.returnValue = false;
            //if(theEvent.preventDefault) theEvent.preventDefault();
        }
    }
    function validateValue(inputText, id_jurisdiction, class_jur, perc_type) {
        $.post("/services/jurisdiction_services.inc", {"action":2,"jur_id":id_jurisdiction,"jur_class":class_jur,"perc_type":perc_type}, function(data){
            parameters = data.split(";");
            if(parseInt(inputText.value) > parseInt(parameters[0]) || parseInt(inputText.value) < parseInt(parameters[2])){
                jAlert(data);
                jAlert(parameters[3]);
                inputText.value = parseInt(parameters[1]);
            }

        });
    }
    function showHide(divClass,divText){
        if ($("."+divClass).is(":hidden")){
            $("."+divClass).fadeIn("slow");
            $(divText).html("<?=$lang->getLang("hide")?> &#9650;");
        }
        else{
            $("."+divClass).fadeOut("slow");
            $(divText).html("<?=$lang->getLang("show")?> &#9660;");
        }
    }
</script>
<?php

/**
 * @param $jur_id
 * @param $jur_class
 * @param $node
 * @param string $child_class
 */


function showPercentage($jur_id,$jur_class,$node = -1, $child_class = "",$defaultClass='label'){
    global $dbh,$lang;
    //check if this id exist
    $enabled=hasPermissions($child_class);
    ?>
    <tr>
        <td class="formheading" colspan="2"><?=$lang->getLang("Payments Settings")?>
            <span class="fright" style="cursor:pointer " onclick="showHide('paymentsContainer',this)"><?=$lang->getLang('Hide')?> &#9650;</span></td>
    </tr>
    <tr><td class="label">All</td><td class="content"><input type="number" id="new_perc" maxlength="2" style="margin-right: 5px;"><input type="button" id="apply_perc" class="entitybtn" style="width:60px;margin-bottom:2px;" value="Apply"></td></tr>
    <tr class="paymentsContainer">
        <td class="<?=$defaultClass?>"><?=$lang->getLang("Type")?></td><td class="<?=$defaultClass?>"><?=$lang->getLang("Percentuals")?></td>
    </tr>
    <?php
    $sql="SELECT * FROM jurisdiction_payment_settings WHERE JPS_JUR_ID='$jur_id' ";

    $parentResult=$dbh->queryRow($sql);
    if(!$parentResult)
    {
        if($jur_class != "casino"){
            ?>
            <tr class="paymentsContainer">
                <td class="content" colspan="2"><?=$lang->getLang("Before to insert percentage your up level has to set your percentage!")?></td>
            </tr>
            <?php
            return;
        }
    }

    if($node != -1){

        $sql = "SELECT * FROM jurisdiction_payment_settings where jps_jur_id = '$node'";
        $result = $dbh->queryRow($sql);
    }
    $sql = "desc jurisdiction_payment_settings";
    $fields = $dbh->exec($sql);
    while($fields->hasNext()){
        $field = $fields->next();
        //echo " <br/>".  $field['field']. " ";
        if(strpos($field['field'],"PERC") > 0){
            //echo $field['field'] . " ";
            //echo $parentResult[$field['field']] . " ";
            if($parentResult[strtolower($field['field'])] == NULL) $parentResult[strtolower($field['field'])] = "99";
            if($parentResult[strtolower($field['field'])] > 0){
                //  echo $field['field'];
                if(strpos($field['field'],"BETTING") > 0){
                    continue;
                }
                ?> <tr class="paymentsContainer">
                    <td class="<?=$defaultClass?>"><?=substr($field['field'], 9) == "PLAYTECH" ? "PLAYTECH / SKYWIND" : substr($field['field'], 9)?></td>
                    <td class="content">
                        <?php if(!$enabled){
                            ?>
                            <input type="hidden" id="<?=strtolower($field['field'])?>" name="<?="percentuals[".$field['field']."]"?>" value="<?=$result[strtolower($field['field'])]?>" />
                        <?php
                        }
                        createSelect($field['field'], 0, $parentResult[strtolower($field['field'])], $result[strtolower($field['field'])], $node, $child_class, $enabled) ?>
                    </td>
                </tr>
            <?php
            }
        }
    }
}


/**
 * @param $jur_id
 * @param $jur_class
 * @param $child_class
 * @param $node
 * @param $enabled
 */
function showBettingPercentage($jur_id,$jur_class,$child_class,$node = -1, $enabled,$defaultClass='label'){
    global $lang, $dbh;

    if($enabled===true){
        $enabled=canChangeBettingPercs();
    }
    elseif($enabled==-1){
        $enabled=true;
    }
    //die(var_dump(strtotime('now')- strtotime('Last Tuesday of '.date('F o', strtotime("-1 month")))));
    $banco = isset($_POST["commisionbanco"]) ? $_POST["commisionbanco"] : ($child_class == 'club' ? "50" : "20");
    $bancoplus = isset($_POST["commisionbancoplus"]) ? $_POST["commisionbancoplus"] : ($child_class == 'club' ? "40"  : "15");
    $extraprofit = isset($_POST["extraprofit"]) ? $_POST["extraprofit"] : ($child_class == 'club' ? "50" : "20");
    $feedtax = isset($_POST["feedtax"]) ? $_POST["feedtax"] : '3';

    $commission_default_value = array("3","5","8","8","11","11","13","13","13","14","14","14","15","15","15","15","15","15","18","18","18","18","18","18","20","20","20","20","20","20");
    $commission_default_value = isset($_POST["commissions"]) ? $_POST['commissions'] : $commission_default_value;

    $monthly_default_value = array("2","4","6","6","9","9","9","11","11","11","13","13","13","13","15","15","15","15","15","15","17","17","17","17","19","19","19","19","19","19");
    $monthly_default_value = isset($_POST["monthly"]) ? $_POST['monthly'] : $monthly_default_value;
    $bonus_monthly = isset($_POST["bonusmensile"]) ? $_POST["bonusmensile"] : "35";
    $bettingType = isset($_POST["betting"]) ? $_POST["betting"] : "1";




    $error = false;
    if($node != -1){


        $parentPercentageString=explode(';',$dbh->queryOne("SELECT JPS_PERC_BETTING FROM jurisdiction_payment_settings,jurisdiction where jps_jur_id=  jur_parent_id AND JUR_ID='$node' "));

        $parentType=explode(":",$parentPercentageString[0]);
        $parentPercentage=explode(":",$parentPercentageString[1]);

        $sql = "Select JPS_PERC_BETTING from jurisdiction_payment_settings where jps_jur_id = " . $node;
        $percentages = $dbh->queryOne($sql);
        if(!$percentages){
            $error = true;
        }else{
            //type:3;percs:0~3,1~5,2~8,3~8,4~11,5~11,6~13,7~13,8~13,9~14,10~14,11~14,12~15,13~15,14~15,15~15,16~15,17~15,18~18,19~18,2
            $bettingSettings = explode(";",$percentages);
            list($label, $type) = explode(":",$bettingSettings[0]);
            $bettingType = $type;
            list($label, $percentage) = explode(":",$bettingSettings[1]);
            switch($bettingType){
                case 1:
                    $banco = $percentage;
                    break;
                case 3:
                    $bancoplus = $percentage;
                    break;
                case 2:
                    $commission_default_value = explode(",",$percentage);
                    break;
                case 4:
                    $percentageList = explode(",",$percentage);
                    $bonus_monthly = array_shift($percentageList);
                    $monthly_default_value = $percentageList;
                    break;
                case 9:
                    $extraprofit = $percentage;
                    break;
                case 11:
                    $feedtax = $percentage;
                    break;
            }
        }
    }

    ?>
    <tr>
        <td class="formheading" colspan="2"><?=$lang->getLang("Betting Settings")?><span class="fright" style="cursor:pointer" onclick="showHide('bettingContainer',this)"><?=$lang->getLang('Hide')?> &#9650;</span> </td>
    </tr>
    <tr class="bettingContainer">
        <td class="<?=$defaultClass?>"><?=$lang->getLang("Type")?></td><td class="<?=$defaultClass?>"><?=$lang->getLang("Percentuals")?></td>
    </tr>

    <tr class="bettingContainer"><td  class="<?=$defaultClass?>" style="vertical-align: top"><?=$lang->getLang("BETTING")?></td>
        <td colspan="2">
            <div class="label"><?=$lang->getLang("Commission type")?>:</div><br>
            <?php if(!$enabled){ ?> <input type="hidden" name="betting" value="<?=$bettingType?>" /> <?php } ?>
            <?php if($error){ ?> <span style="color:red;"><?=$lang->getLang("Not Saved")?></span><br/> <?php } ?>
            <select name="betting" <?php if(!$enabled){ ?> disabled="disabled"  <?php } ?> id="betting" onchange="showPercOptions(this.value)">
                <option value="1" <?php if($bettingType == 1){ ?> selected <?php }?>>Banco</option>
                <option value="3" <?php if($bettingType == 3){ ?> selected <?php }?>>Banco Plus</option>
                <?php if($child_class == "club"){ ?>
                    <option value="2" <?php if($bettingType == 2){ ?> selected <?php }?>>Provvigioni</option>
                    <option value="4" <?php if($bettingType == 4){ ?> selected <?php }?>>Misto</option>
                <?php } ?>
                <?if($child_class != "club"){?>
                <option value="9" <?php if($bettingType == 9){ ?> selected <?php }?>>Extra Profit</option>
            <? }?>
                <?if($child_class=='nation'){?>
                    <option value="11" <?php if($bettingType == 11){ ?> selected <?php }?>>FeedTax</option>
                <? } ?>
            </select>
            <br>

            <br><div class="label tparams"><?=$lang->getLang("Text commission parameters")?></div>
            <div class="tparams">
                <br>
                <input  <?php if(!$enabled){ ?> readonly="readonly"  <?php } ?> type="text" name="tcommParams" id="tcommParams" value="<?=$banco?>" size="50" ><br />
                <div class="error hidden" id="tcommParamsError"></div>
                <br />
            </div>


            <br><div class="label"><?=$lang->getLang("Commission parameters")?></div><br>
            <div id="1" class="hidden">
                <input style="width: 35px"  <?php if(!$enabled){ ?> readonly="readonly"  <?php } ?> <?php if($parentType[1]==1){ ?> max="<?=$parentPercentage[1]?>"  <?php } ?> type="number"  name="commisionbanco" maxlength="2" value="<?=$banco?>" size="2" <?if($child_class!='club') { ?>onblur="validateValue(this,<?=$node?>,'<?=$child_class?>','JPS_PERC_BETTING')" <? } ?> onkeypress="validate(event)">%
            </div>
            <div id="3" class="hidden">
                <input <?php if(!$enabled){ ?> readonly="readonly"  <?php } ?> type="number"  <?php if($parentType[1]==3){ ?> max="<?=$parentPercentage[1]?>"  <?php } ?> name="commisionbancoplus" maxlength="2" size="2" value="<?=$bancoplus?>" <?if($child_class!='club') { ?>onblur="validateValue(this,<?=$node?>,'<?=$child_class?>','JPS_PERC_BETTING')" <? } ?>  onkeypress="validate(event)">%
            </div>
            <div id="2"  class="hidden" style="position:relative;">
                <div class="comissionOverlay"></div>
                <table style="width:100%;text-align: left" >
                    <tr><th><?=$lang->getLang("Grouping")?></th><th><?=$lang->getLang("Commission")?></th></tr>
                    <? for($i=1;$i<=30;$i++){?>
                        <tr><td><?=$i?></td><td><input <?php if(!$enabled){ ?> readonly="readonly"  <?php } ?> type="text" value="<?=$commission_default_value[$i-1]?>" <?if($child_class!='club') { ?>onblur="validateValue(this,<?=$node?>,'<?=$child_class?>','JPS_PERC_BETTING')" <? } ?>  name="commissions[]" maxlength="2" size="2" onkeypress="validate(event)">%</td></tr>
                    <?
                    }
                    ?>
                </table>
            </div>
            <div id="4" class="hidden" style="position:relative;" >
                <div class="comissionOverlay"></div>
                <table style="width:100%;text-align: left">
                    <tr><th><?=$lang->getLang("Grouping")?></th><th><?=$lang->getLang("Commission")?></th></tr>
                    <tr><td><?=$lang->getLang("Monthly Bonus")?></td><td><input type="text" <?php if(!$enabled){ ?> readonly="readonly"  <?php } ?> value="<?=$bonus_monthly?>" id="bonusmensile" name="bonusmensile" maxlength="2" size="2" onkeypress="validate(event)">%</td></tr>
                    <? for($i=1;$i<=30;$i++){?>
                        <tr><td><?=$i?></td><td><input type="text" <?php if(!$enabled){ ?> readonly="readonly"  <?php } ?> value="<?=$monthly_default_value[$i-1]?>" <?if($child_class!='club') { ?>onblur="validateValue(this,<?=$node?>,'<?=$child_class?>','JPS_PERC_BETTING')" <? } ?>  name="monthly[]" maxlength="2" size="2" onkeypress="validate(event)">%</td></tr>
                    <?
                    }
                    ?>
                </table>
            </div>
            <div id="9" class="hidden">
                <input <?php if(!$enabled){ ?> readonly="readonly"  <?php } ?> type="text" name="extraprofit" maxlength="2" size="2" onblur="validateValue(this,<?=$node?>,'<?=$child_class?>','JPS_PERC_BETTING')" value="<?=$extraprofit?>" onkeypress="validate(event)">%
            </div>
            <div id="11" class="hidden">
                <input <?php if(!$enabled){ ?> readonly="readonly"  <?php } ?> id="feedtax" type="text" name="feedtax" maxlength="4" size="4" value="<?=$feedtax?>" onkeypress="validate(event)">%
            </div>
        </td>
    </tr>
<?
}


/**
 * @param $name
 * @param $min_value
 * @param $max_value
 * @param $default
 * @param $node
 * @param $child_class
 * @param $enabled
 */
function createSelect($name, $min_value, $max_value, $default, $node, $child_class, $enabled){
    ?>
    <select style="float:left;margin-right:10px;" <?php if(!$enabled){ ?> disabled="disabled" <?php } ?> id="sel_<?=strtolower($name)?>" name="<?="percentuals[".$name."]"?>" onblur="validateValue(this,'<?=$node?>','<?=$child_class?>','<?=$name?>')">
        <?php
        for($i = $min_value ; $i <= $max_value; $i++){
            printOption($name, "$i", "$i%", ($default == $i));
        }
        ?>
    </select>
    <div style="padding-top:3px;width:150px;" id="text_<?=strtolower($name)?>"></div>
<?php
}


/**
 * @param $percentuals
 * @param $node
 * @param $parent
 * @param $type
 * @param $live_type
 * @return bool|int|RecordSet
 */
function savePercentuals($percentuals, $node, $parent,$type,$live_type){
    global $dbh;
    $sql="SELECT * FROM jurisdiction_payment_settings WHERE JPS_JUR_ID=$parent ";
    $parentResult=$dbh->queryRow($sql,true);
    if(!$parentResult)
    {
        return FALSE;
    }
    $sql = "Select count(*) from jurisdiction_payment_settings where jps_jur_id = " . $node;
    $exists = $dbh->queryOne($sql,true);

    $betting_value = getBettingString($type);
    $live_betting_value=getLiveBettingString($live_type);

    if($exists == 0){
        $sql2= "INSERT INTO jurisdiction_services (jss_jur_id) VALUES ($node);";
        if($dbh->exec($sql2,false, true)){
        $sql="Insert into jurisdiction_payment_settings(JPS_JUR_ID ,JPS_PERC_BETTING,JPS_PERC_LIVE_BETTING, ";
        foreach($percentuals as $key => $value){
            $sql .= "$key, ";
        }
        $sql = substr($sql , 0, strlen($sql) - 2);
        $sql .= ") VALUES ( $node, '".$betting_value."', '".$live_betting_value."', ";
        foreach($percentuals as $key => $value){
            $sql .= "$value, ";
        }
        $sql = substr($sql , 0, strlen($sql) - 2);
        $sql .= ")";
        }
        else{
            return false;
        }
    }else{
        $sql = "UPDATE jurisdiction_payment_settings SET JPS_PERC_BETTING = '".$betting_value."',JPS_PERC_LIVE_BETTING = '".$live_betting_value."', ";

        if(!is_null($percentuals)){
            foreach($percentuals as $key => $value){
                $sql .= "$key = $value, ";
            }
            $sql = substr($sql , 0, strlen($sql) - 2);
        }
        $sql .= " where jps_jur_id = ". $node;
    }
    $ret= $dbh->exec($sql,false,true);

    if($ret){
                $sql="DELETE FROM jurisdiction_betting_comm WHERE jbc_jur_id= $node";
                $result=$dbh->exec($sql,false,true);
                switch($type){
                    case 1:
                        $sql = "INSERT INTO jurisdiction_betting_comm (jbc_comm_type,jbc_jur_id,jbc_grouping,jbc_reg_date,jbc_perc) VALUES (1,$node,0,CURRENT_TIMESTAMP,".$_POST["commisionbanco"].")";
                        break;
                    case 3:
                        $sql = "INSERT INTO jurisdiction_betting_comm (jbc_comm_type,jbc_jur_id,jbc_grouping,jbc_reg_date,jbc_perc) VALUES (3,$node,0,CURRENT_TIMESTAMP,".$_POST["commisionbancoplus"].")";
                        break;
                    case 2:
                        $sql = "INSERT INTO jurisdiction_betting_comm (jbc_comm_type,jbc_jur_id,jbc_grouping,jbc_reg_date,jbc_perc) VALUES ";
                        $y = 0;
                        for($i=0; $i < count($_POST["commissions"]); $i++){
                            $y++;
                            $sql .= " (2,$node,". $y .",CURRENT_TIMESTAMP,".$_POST["commissions"][$i]."),";
                        }
                        $sql  = substr($sql, 0 ,strlen($sql)-1);
                        break;
                    case 4:
                        $sql = "INSERT INTO jurisdiction_betting_comm (jbc_comm_type,jbc_jur_id,jbc_grouping,jbc_reg_date,jbc_perc) VALUES (4,$node,0,CURRENT_TIMESTAMP,".$_POST["bonusmensile"]."),";
                        $y = 0;
                        for($i=0; $i < count($_POST["monthly"]); $i++){
                            $y++;
                            $sql .= "  (4,$node,".$y.",CURRENT_TIMESTAMP,".$_POST["monthly"][$i]."),";
                        }
                        $sql = substr($sql, 0 ,strlen($sql)-1);
                        break;
                    case 9:
                        $sql = "INSERT INTO jurisdiction_betting_comm (jbc_comm_type,jbc_jur_id,jbc_grouping,jbc_reg_date,jbc_perc) VALUES (9,$node,0,CURRENT_TIMESTAMP,".$_POST["extraprofit"].")";
                        break;

                    case 11:
                        $sql = "INSERT INTO jurisdiction_betting_comm (jbc_comm_type,jbc_jur_id,jbc_grouping,jbc_reg_date,jbc_perc) VALUES (11,$node,0,CURRENT_TIMESTAMP,".$_POST["feedtax"].")";
                        break;
                }
                $bettingAddResult=$dbh->exec($sql,false, true);
                if(!$bettingAddResult){
                    return false;
                }
                elsE{

                    switch($live_type){

                        case 5:
                            $sql = "INSERT INTO jurisdiction_betting_comm (jbc_comm_type,jbc_jur_id,jbc_grouping,jbc_reg_date,jbc_perc) VALUES (5,$node,0,CURRENT_TIMESTAMP,".$_POST["commisionbanco"].")";
                            break;
                        case 7:
                            $sql = "INSERT INTO jurisdiction_betting_comm (jbc_comm_type,jbc_jur_id,jbc_grouping,jbc_reg_date,jbc_perc) VALUES (7,$node,0,CURRENT_TIMESTAMP,".$_POST["commisionbancoplus"].")";
                            break;
                        case 6:
                            $sql = "INSERT INTO jurisdiction_betting_comm (jbc_comm_type,jbc_jur_id,jbc_grouping,jbc_reg_date,jbc_perc) VALUES ";
                            $y = 0;
                            for($i=0; $i < count($_POST["commissions"]); $i++){
                                $y++;
                                $sql .= " (6,$node,". $y .",CURRENT_TIMESTAMP,".$_POST["commissions_live"][$i]."),";
                            }
                            $sql  = substr($sql, 0 ,strlen($sql)-1);
                            break;
                        case 8:
                            $sql = "INSERT INTO jurisdiction_betting_comm (jbc_comm_type,jbc_jur_id,jbc_grouping,jbc_reg_date,jbc_perc) VALUES (8,$node,0,CURRENT_TIMESTAMP,".$_POST["bonusmensile"]."),";
                            $y = 0;
                            for($i=0; $i < count($_POST["monthly"]); $i++){
                                $y++;
                                $sql .= "  (8,$node,".$y.",CURRENT_TIMESTAMP,".$_POST["monthly_live"][$i]."),";
                            }
                            $sql = substr($sql, 0 ,strlen($sql)-1);
                            break;
                        case 10:
                            $sql = "INSERT INTO jurisdiction_betting_comm (jbc_comm_type,jbc_jur_id,jbc_grouping,jbc_reg_date,jbc_perc) VALUES (10,$node,0,CURRENT_TIMESTAMP,".$_POST["extraprofit"].")";
                            break;
                        case 12:
                            $sql = "INSERT INTO jurisdiction_betting_comm (jbc_comm_type,jbc_jur_id,jbc_grouping,jbc_reg_date,jbc_perc) VALUES (12,$node,0,CURRENT_TIMESTAMP,".$_POST["feedtax_live"].")";
                            break;
                    }
                    return $dbh->exec($sql,false, true);
                }
    }
    else{
        return false;
    }
}


/**
 * @return bool
 */
function checkPercentualsCasino(){
    $percentuals = $_POST["percentuals"];
    foreach($percentuals as $key => $value){
        if($value == 0) return false;
    }
    return true;
}


/**
 * @return bool
 */
function checkPercentualsFields(){
    $type = $_POST["betting"];
    switch($type){
        case 1:
            $commision = $_POST["commisionbanco"];
            if($commision == "") return false;
            break;
        case 3:
            $commision = $_POST["commisionbancoplus"];
            if($commision == "") return false;
            break;
        case 2:
            $commision = $_POST["commissions"];
            foreach($commision as $key => $value){
                if($value == "") return false;
            }
            break;
        case 4:
            $bonusM = $_POST["bonusmensile"];
            if($bonusM == "") return false;
            $commision = $_POST["monthly"];
            foreach($commision as $key => $value){
                if($value == "") return false;
            }
            break;
        case 9:
            $commision = $_POST["extraprofit"];
            if($commision == "") return false;
            break;
    }
    return true;
}


/**
 * @param $type
 * @param $node
 * @return bool|int
 */
function saveBettingPercentuals($type, $node){

    return save_betting_percentuals($type, $node);
}


/**
 * @param $type
 * @return string
 */
function getBettingString($type){
    switch($type){
        case 1:
            $commission = $_POST["commisionbanco"];
            return "type:1;percs:".$commission;
        case 3:
            $commission = $_POST["commisionbancoplus"];
            return "type:3;percs:".$commission;
        case 2:
            $commission = $_POST["commissions"];
            $commission_value = "";
            foreach($commission as $key => $value){
                $commission_value .= "$value,";
            }
            $commission_value = substr($commission_value, 0 , strlen($commission_value)-1);
            return "type:2;percs:".$commission_value;
        case 4:
            $bonusM = $_POST["bonusmensile"];
            $commission = $_POST["monthly"];
            $commission_value = "";
            foreach($commission as $key => $value){
                $commission_value .= "$value,";
            }
            $commission_value = substr($commission_value, 0 , strlen($commission_value)-1);
            return "type:4;percs:".$bonusM.",".$commission_value;
        case 9:
            $commission = $_POST["extraprofit"];
            return "type:9;percs:".$commission;
        case 11:
            $commission = $_POST["feedtax"];
            return "type:11;percs:".$commission;
    }
}


/**
 * @param $jurisdiction
 * @return bool
 */
function hasPermissions($jurisdiction){
    $enabled=check_access('jurisdiction_modify_percentage');
    if($enabled){
        if(strpos(getAllSubJurisdictions($_SESSION['jurisdiction_class']), strtolower($jurisdiction))===false){
            return false;
        }
        else{
            return true;
        }
    }
    else{
        return false;
    }
}





/**
 * @param $jur_id
 * @param $jur_class
 * @param $child_class
 * @param $node
 * @param $enabled
 */
function showLiveBettingPercentage($jur_id,$jur_class,$child_class,$node = -1, $enabled,$defaultClass='label'){
    global $lang, $dbh;
    if($enabled===true){
        $enabled=canChangeBettingPercs();
    }
    elseif($enabled==-1){
        $enabled=true;
    }
    //die(var_dump(strtotime('now')- strtotime('Last Tuesday of '.date('F o', strtotime("-1 month")))));
    $banco = isset($_POST["commisionbanco_live"]) ? $_POST["commisionbanco_live"] : ($child_class == 'club' ? "50" : "20");
    $bancoplus = isset($_POST["commisionbancoplus_live"]) ? $_POST["commisionbancoplus_live"] : ($child_class == 'club' ? "40"  : "15");
    $extraprofit = isset($_POST["extraprofit_live"]) ? $_POST["extraprofit_live"] : ($child_class == 'club' ? "50" : "20");
    $feedtax = isset($_POST["feedtax_live"]) ? $_POST["feedtax_live"] : 3;

    $commission_default_value = array("1","2","3","4","5","7","9","11","11","13","13","13","13","15","15","17","17","17","20","20","20","20","20","20","20","20","20","20","20","20");
    $commission_default_value = isset($_POST["commissions_live"]) ? $_POST['commissions_live'] : $commission_default_value;

    $monthly_default_value = array("1","2","3","4","5","7","9","11","11","13","13","13","13","15","15","15","17","17","17","20","20","20","20","20","20","20","20","20","20","20");
    $monthly_default_value = isset($_POST["monthly_live"]) ? $_POST['monthly_live'] : $monthly_default_value;
    $bonus_monthly = isset($_POST["bonusmensile_live"]) ? $_POST["bonusmensile_live"] : "35";
    $bettingType = isset($_POST["betting_live"]) ? $_POST["betting_live"] : "5";
    $error = false;
    if($node != -1){
        $sql = "Select JPS_PERC_LIVE_BETTING from jurisdiction_payment_settings where jps_jur_id = " . $node;
        $percentages = $dbh->queryOne($sql);
        if(!$percentages){
            $error = true;
        }else{
            //type:3;percs:0~3,1~5,2~8,3~8,4~11,5~11,6~13,7~13,8~13,9~14,10~14,11~14,12~15,13~15,14~15,15~15,16~15,17~15,18~18,19~18,2
            $bettingSettings = explode(";",$percentages);
            list($label, $type) = explode(":",$bettingSettings[0]);
            $bettingType = $type;
            list($label, $percentage) = explode(":",$bettingSettings[1]);
            switch($bettingType){
                case 5:
                    $banco = $percentage;
                    break;
                case 7:
                    $bancoplus = $percentage;
                    break;
                case 6:
                    $commission_default_value = explode(",",$percentage);
                    break;
                case 8:
                    $percentageList = explode(",",$percentage);
                    $bonus_monthly = array_shift($percentageList);
                    $monthly_default_value = $percentageList;
                    break;
                case 10:
                    $extraprofit = $percentage;
                    break;
                case 12:
                    $feedtax = $percentage;
                    break;
            }
        }
    }
    ?>
    <tr>
        <td class="formheading" colspan="2"><?=$lang->getLang("Live Betting Settings")?> <span class="fright" style="cursor:pointer" onclick="showHide('bettingLiveContainer',this)"><?=$lang->getLang('Hide')?> &#9650; </span> </td>
    </tr>
    <tr class="bettingLiveContainer">
        <td class="<?=$defaultClass?>"><?=$lang->getLang("Type")?></td><td class="<?=$defaultClass?>"><?=$lang->getLang("Percentuals")?></td>
    </tr>

    <tr class="bettingLiveContainer"><td  class="<?=$defaultClass?>" style="vertical-align: top"><?=$lang->getLang("LIVE BETTING")?> </td>
        <td colspan="2">
            <div class="label"><?=$lang->getLang("Commission type")?>:</div><br>

             <input type="hidden" id="betting_live" name="betting_live" value="<?=$bettingType?>" />
            <?php if($error){ ?> <span style="color:red;"><?=$lang->getLang("Not Saved")?></span><br/> <?php } ?>
            <select id="betting_live_select" disabled>
                <option value="5" <?php if($bettingType == 5){ ?> selected <?php }?>>Banco Live</option>
                <option value="7" <?php if($bettingType == 7){ ?> selected <?php }?>>Banco Plus Live</option>
                <?php if($child_class == "club"){ ?>
                    <option value="6" <?php if($bettingType == 6){ ?> selected <?php }?>>Provvigioni Live</option>
                    <option value="8" <?php if($bettingType == 8){ ?> selected <?php }?>>Misto Live</option>
                <?php } ?>
                <?if($child_class != "club"){ ?>
                <option value="10" <?php if($bettingType == 10){ ?> selected <?php }?>>Extra Profit Live</option>
                <? }?>
                <?if($child_class=='nation'){?>
                    <option value="12" <?php if($bettingType == 12){ ?> selected <?php }?>>FeedTax B Live</option>
                <? } ?>
            </select><br>
            <br><div class="label"><?=$lang->getLang("Commission parameters")?></div><br>
            <div id="5" class="hidden_live">
                <input  readonly="readonly"  type="text" name="commisionbanco_live" maxlength="2" value="<?=$banco?>" size="2" onblur="validateValue(this,<?=$node?>,'<?=$child_class?>','JPS_PERC_LIVE_BETTING')" onkeypress="validate(event)">%
            </div>
            <div id="7" class="hidden_live">
                <input  readonly="readonly"  type="text" name="commisionbancoplus_live"maxlength="2" size="2" value="<?=$bancoplus?>" onkeypress="validate(event)">%
            </div>
            <div id="6"  class="hidden_live">
                <table style="width:100%;text-align: left" >
                    <tr><th><?=$lang->getLang("Grouping")?></th><th><?=$lang->getLang("Commission")?></th></tr>
                    <? for($i=1;$i<=30;$i++){?>
                        <tr><td><?=$i?></td><td><input <?php if(!$enabled){ ?> readonly="readonly"   <?php } ?> onblur="validateValue(this,<?=$node?>,'<?=$child_class?>','JPS_PERC_LIVE_BETTING')" type="text" value="<?=$commission_default_value[$i-1]?>" name="commissions_live[]" maxlength="2" size="2" onkeypress="validate(event)">%</td></tr>
                    <?
                    }
                    ?>
                </table>
            </div>
            <div id="8" class="hidden_live" >
                <table style="width:100%;text-align: left">
                    <tr><th><?=$lang->getLang("Grouping")?></th><th><?=$lang->getLang("Commission")?></th></tr>
                    <tr><td><?=$lang->getLang("Monthly Bonus")?></td><td><input type="text"  readonly="readonly"   value="<?=$bonus_monthly?>" name="bonusmensile_live" maxlength="2" size="2" onkeypress="validate(event)">%</td></tr>
                    <? for($i=1;$i<=30;$i++){?>
                        <tr><td><?=$i?></td><td><input type="text" <?php if(!$enabled){ ?> readonly="readonly"  <?php } ?> value="<?=$monthly_default_value[$i-1]?>" onblur="validateValue(this,<?=$node?>,'<?=$child_class?>','JPS_PERC_LIVE_BETTING')" name="monthly_live[]" maxlength="2" size="2" onkeypress="validate(event)">%</td></tr>
                    <?
                    }
                    ?>
                </table>
            </div>
            <div id="10" class="hidden_live">
                <input  readonly="readonly"  type="text" name="extraprofitlive_live" maxlength="2" value="<?=$extraprofit?>" size="2" onblur="validateValue(this,<?=$node?>,'<?=$child_class?>','JPS_PERC_LIVE_BETTING')" onkeypress="validate(event)">%
            </div>
            <div id="12" class="hidden_live">
                <input  type="text" name="feedtax_live" maxlength="4" value="<?=$feedtax?>" size="4" onkeypress="validate(event)">%
            </div>
        </td>
    </tr>
<?
}


/**
 * @param $type
 * @return string
 */
function getLiveBettingString($type){
    switch($type){
        case 5:
            $commission = $_POST["commisionbanco"];
            return "type:5;percs:".$commission;
        case 7:
            $commission = $_POST["commisionbancoplus"];
            return "type:7;percs:".$commission;
        case 6:
            $commission = $_POST["commissions_live"];
            $commission_value = "";
            foreach($commission as $key => $value){
                $commission_value .= "$value,";
            }
            $commission_value = substr($commission_value, 0 , strlen($commission_value)-1);
            return "type:6;percs:".$commission_value;
        case 8:
            $bonusM = $_POST["bonusmensile"];
            $commission = $_POST["monthly_live"];
            $commission_value = "";
            foreach($commission as $key => $value){
                $commission_value .= "$value,";
            }
            $commission_value = substr($commission_value, 0 , strlen($commission_value)-1);
            return "type:8;percs:".$bonusM.",".$commission_value;
        case 10:
            $commission = $_POST["extraprofit"];
            return "type:10;percs:".$commission;

        case 12:
            $commission = $_POST["feedtax_live"];
            return "type:12;percs:".$commission;
    }
}


function checkBettingPercentages($jur_id){
    global $dbh;
    $sql="SELECT jps perc_betting from jurisdiction_payment_setting where jps_jur_id=".$dbh->escape($jur_id);
    $result=$dbh->queryOne($sql);


}

function canChangeBettingPercs(){
   if(check_access('betting_force_percentage_modify')){
       return true;
   }
    $now=date('Y-m-d');
    $lastMonday= date('Y-m-d',strtotime("Last Monday of".date('Y-m')));
    $lastDayToModify=date('Y-m-d',strtotime($lastMonday." + 5 days"));
    $previousLastMonday= date('Y-m-d',strtotime("Last Monday of".date('Y-m',strtotime("$now+".(-1)." months"))));
    $previousLastDayToModify= date('Y-m-d',strtotime($previousLastMonday." + 5 days"));
    if((($lastMonday<=$now)&&($now<=$lastDayToModify)) || (($previousLastMonday<=$now)&&($now<=$previousLastDayToModify))){
        return true;
    }
    return false;

}

?>