<?php global $lang; if ( areErrors() ) { showErrors(); echo "<br /><br />";}?>
<script type="text/javascript" src="https://maps.googleapis.com/maps/api/js?libraries=places&sensor=false&&language=<?=$_SESSION['language']?>&region=US"></script>
<script type="text/javascript">

    function showAdminAdd(){
        if ($("#adminAdd").is(":hidden")){
            $("#adminAdd").fadeIn("slow");
            $("#adminAddShow").html("<?=$lang->getLang("Don't add an administrator")?> &#9650;");
        }
        else{
            $("#adminAdd").fadeOut("slow");
           $('input[name="username"]').val('');
            $("#adminAddShow").html("<?=$lang->getLang("Add an administrator for this %" , "Club")?>  &#9660;");
        }
        return false;
    }
    $(function(){
        
        var regex=/^[0-9.\b]+$/;
        $(".tax").keypress(function(event) {
            var _event = event || window.event;
            var key = _event.keyCode || _event.which;
            key = String.fromCharCode(key);
            if(!regex.test(key)) {
                _event.returnValue = false;
                if (_event.preventDefault)
                    _event.preventDefault();
            }
        });
        $('.taxCheckbox').on('click',function(){
            $('#'+$(this).val()).attr('disabled',!this.checked);
        });

        var startTimeTextBox = $('#hoursStart');
        var endTimeTextBox = $('#hoursEnd');

        $.timepicker.timeRange(
            startTimeTextBox,
            endTimeTextBox,
            {
                minInterval: (1000*60*60*8), // 8hr
                timeFormat: 'HH:mm',
                start: {}, // start picker options
                end: {} // end picker options
            }
        );

        $('#country').select2({
            width: "158px"
        });
        geocoder = new google.maps.Geocoder();
    $('#city').on('focus',function(){
        input = this,
        options = {
            types: ['(cities)'],
            componentRestrictions: {
                country: $('#country option:selected').val()
            }
        };
        city = new google.maps.places.Autocomplete(input, options);
        google.maps.event.addListener(city, 'place_changed', function() {
            place = city.getPlace();
            $('#city').val(place.address_components[0].long_name);
        })
    });
        $('#address, #city, #country').on('blur',function(){
            var country=$('#country option:selected').text();
            var address=$('#address').val();
            var city=$('#city').val();

            if( country!='' && address!='' && city!='') {
                currAddress = address + ',' + city + ',' + country;
                geocoder.geocode({'address': currAddress}, function (results, status) {
                    $('#longitudeS,#longitude').val(results[0].geometry.location.lng());
                    $('#latitudeS,#latitude').val(results[0].geometry.location.lat());
                });
            }
        });



    });
</script>
<form action="<?= $_SERVER['PHP_SELF'] ?>" method="POST">
    <input type="hidden" name="node" value="<?=$node;?>" />
    <input type="hidden" name="class" value="club" />
    <input type="hidden" name="op" value="Save" />

    <table cellpadding="0" cellspacing="1" border="0">
        <tr>
            <td>
                <table cellpadding="4" cellspacing="1" border="0" width="370">
                    <tr>
                        <td class="formheading" colspan="2"><?=$lang->getLang("Add New Club")?></td>
                    </tr>
                    <tr>
                        <td class="label" width="130"><?=$lang->getLang("Club Name")?><br /><div class="labelnote"></div></td>
                        <td class="content"><input type="text" class="lettersAndNumbers" name="club_name" value="<?php if ( areErrors() ) { echo replace_quotes($_POST['club_name']); } ?>" size=10 /></td>
                    </tr>
                    <?php
                    if(empty($_POST['account_code'])){
                        $_POST['account_code'] = strtoupper(get_free_jurcode());
                    }
                    ?>
                    <tr>
                        <td class="label" width="130"><?=$lang->getLang("Account Code")?> <br /><div class="labelnote">(6 Characters)</div></td>
                        <td class="content"><input type="text" name="account_code" value="<?=replace_quotes($_POST['account_code'])?>" size="11"  maxlength="6"/></td>
                    </tr>
                    <tr>
                        <td class=label width=110><?=$lang->getLang("Access Type")?><div id="accessType"></div><br /></td>
                        <td class=content><select name="accessType">
                                <option value="BACKOFFICE">Backoffice</option>
                                <option value="WEBSITE">Website</option>
                            </select>
                        </td>
                    </tr>
                   <tr>
                        <td class="label" width="130"><?=$lang->getLang("District")?></td>
                        <td class="content">
                            <select name="district" id="district" onchange="getSelectedValue();">
                                <option value="none">None</option>
                                <?php
                                if($_SESSION["jurisdiction_class"] == "district"){
                                    echo  get_jurisdiction_options('district', $id=$_SESSION['jurisdiction_id'], $hide_internet=true);
                                }else{
                                    echo  get_jurisdiction_options('district', $_POST["district"], $hide_internet=true);
                                }
                                ?>
                            </select>
                            <input type="hidden" name="currency" value="<?=$currency?>" />
                        </td>
                        <td>&nbsp;</td>
                    </tr>
                    <tr>
                        <td class=label><?=$lang->getLang("Country")?></td>
                        <td <? form_td_style('country'); ?>>
                            <?countrySelect('country', "", $_POST['user_country'],false,'country'); ?>
                            <?php echo err_field('user_country'); ?>
                        </td>
                    </tr>

                    <tr>
                        <td class="label" width="130"><?=$lang->getLang("City")?></td>
                        <td class="content"><input type="text" name="city" id="city" value="" size=30 /></td>
                    </tr>

                    <tr>
                        <td class="label" width="130"><?=$lang->getLang("Address Line 1")?></td>
                        <td class="content"><input type="text" name="address1" id="address"  value="<?php if ( areErrors() ) { echo replace_quotes($_POST['address1']); } ?>" size=30 /></td>
                    </tr>
                    <tr>
                        <td class="label" width="130"><?=$lang->getLang("Address Line 2")?></td>
                        <td class="content"><input type="text" name="address2" value="<?php if ( areErrors() ) { echo replace_quotes($_POST['address2']); } ?>" size=30 /></td>
                    </tr>
                    <tr>
                        <td class="label" width="130"><?=$lang->getLang("Postcode or Zipcode")?></td>
                        <td class="content"><input type="text" name="postc" value="<?php if ( areErrors() ) { echo replace_quotes($_POST['postc']); } ?>" size=30 /></td>
                    </tr>
                    <tr>
                        <td class="label" width="130"><?=$lang->getLang("Longitude")?></td>
                        <td class="content">
                            <input type="text" id="longitudeS" disabled="disabled" value="<?php if ( areErrors() ) { echo replace_quotes($_POST['langitude']); } ?>" size=30 />
                            <input type="hidden" name="longitude" id="longitude" value="<?php if ( areErrors() ) { echo replace_quotes($_POST['langitude']); } ?>" size=30 />
                        </td>
                    </tr>
                    <tr>
                        <td class="label" width="130"><?=$lang->getLang("Latitude")?></td>
                        <td class="content">
                            <input type="text"  id="latitudeS" disabled="disabled" value="<?php if ( areErrors() ) { echo replace_quotes($_POST['latitude']); } ?>" size=30 />
                            <input type="hidden" name="latitude" id="latitude"  value="<?php if ( areErrors() ) { echo replace_quotes($_POST['latitude']); } ?>" size=30 />
                        </td>
                    </tr>
                    <tr>
                        <td class="label" width="130"><?=$lang->getLang("Working Hours")?> </td>
                        <td class="content">
                            <input type="text" name="hoursStart" id="hoursStart" value="<?php if ( areErrors() ) { echo replace_quotes($_POST['hoursStart']); } ?>" size=10 > -
                            <input type="text" name="hoursEnd" id="hoursEnd" value="<?php if ( areErrors() ) { echo replace_quotes($_POST['hoursEnd']); } ?>" size=10 >

                        </td>
                    </tr>
                    <tr>
                        <td class="label" width="130"><?=$lang->getLang("Contact Number")?> </td>
                        <td class="content"><input type="text" name="phone" value="<?php if ( areErrors() ) { echo replace_quotes($_POST['phone']); } ?>" size=30 /></td>
                    </tr>
                   <?php if ($_SESSION['jurisdiction_id'] == 1) { ?>
                        <tr>
                            <td class="label" width="130"><?=$lang->getLang("Limit new users")?></td>
                            <td class="content"><input type="text" name="users_limit" id="users_limit" value="30" size=20 /></td>
                        </tr>
                    <? } ?>
                    <!-- -->
                    <?php if(defined("DOT_IT") && DOT_IT == true) : ?>
                        <tr>
                            <td class="label" width="130"><?=$lang->getLang("VAT")?></td>
                            <td class="content"><input type="text" name="vat_code" value="<?php if ( areErrors() ) { echo replace_quotes($_POST['vat_code']); } ?>" size=30 /></td>
                        </tr>
                        
                    <?php endif; ?>

                    <tr valign="top">
                        <td class=label width=110><?=$lang->getLang("Config")?></td>
                        <td>
                            <table>
                                <thead>
                                <tr><th>Enabled</th><th>Name</th><th>Value</th></tr>
                                </thead>
                                <tr>
                                    <?
                                    $disabled=true;
                                    $taxVal='';
                                    foreach($taxDetails as $k=>$v){
                                        $v=explode('=',$v);
                                        if($v[0]=='virtual_cash_out'){
                                            $disabled=false;
                                            $taxVal=$v[1];
                                        }
                                    }

                                    ?>
                                    <td><label><input type="checkbox" <?=!$disabled?'checked':''?>   class="taxCheckbox" value="virtual_cash_out"></label></td><td>Virtual cashout tax</td>
                                    <td><input type="text" <?=!$disabled?'':'disabled'?> id="virtual_cash_out" class="tax"   name="tax[virtual_cash_out]" style="width: 50px"  value="<?=!$disabled?$taxVal:''?>"></td>
                                </tr>
                                <tr>
                                    <td><label><input type="checkbox"  checked  class="taxCheckbox" value="virtual_jackpot"></label></td>
                                    <td>Virtual jackpot</td>
                                    <td>ON<input type="hidden"  id="virtual_jackpot" class="tax"  checked  name="virtual_jackpot" style="width: 50px"  value="ON"></td>
                                </tr>
                            </table>
                        </td>
                    </tr>
                    <tr valign="top">
                        <td class="label"><?=$lang->getLang("Processor enabled")?></td>
                        <td class="content">
                            <select class="select2" style="width: 100%; max-width: 250px;" name="processors[]" multiple>
                                <?php
                                foreach ($processors as $proc) {
                                    ?>
                                    <option value="<?=$proc['ppc_code']?>" ><?=$proc['prc_name']?></option>
                                    <?php
                                }
                                ?>
                            </select>
                        </td>
                    </tr>
                    <tr>
                        <td class="label" width="130" valign="top"><?=$lang->getLang("Notes")?></td>
                        <td class="content"><textarea name="notes" rows="14" cols="40" wrap="hard"><?php if ( areErrors() ) { echo replace_quotes($_POST['notes']); } ?></textarea></td>
                    </tr>
                    <table id="casino_perc" width="90%">
                        <?php
                        include_once "funclib/percentual.inc.php";
                        showPercentage($_SESSION["jurisdiction_id"],$_SESSION["jurisdiction_class"],-1,"club");
                        ?>
                    </table>
                    <table id="error_mess_payments" width="90%" style="display:none;">
                        <tr>
                            <td class="formheading" colspan='2'><?=$lang->getLang("Payments Percentage");?></td>
                        </tr>
                        <tr>
                            <td class='content' colspan='2'><?=$lang->getLang("Before to insert percentage to this new % your up level has to set your percentage!","nation")?></td>
                        </tr>
                    </table>
                    <table id="betting_perc" width="90%">
                        <?php
                        showBettingPercentage($_SESSION["jurisdiction_id"],$_SESSION["jurisdiction_class"], "club",-1,-1);
                        ?>
                    </table>
                    <table id="error_mess" width="90%" style="display:none;">
                        <tr>
                            <td class="formheading" colspan='2'><?=$lang->getLang("Betting Percentage");?></td>
                        </tr>
                        <tr>
                            <td class='content' colspan='2'><?=$lang->getLang("Before to insert percentage to this new % your up level has to set your percentage!","club")?></td>
                        </tr>
                    </table>
                    <table id="betting_live_perc" width="90%">
                        <?php
                        showLiveBettingPercentage($_SESSION["jurisdiction_id"],$_SESSION["jurisdiction_class"], "club",-1, -1);
                        ?>
                    </table>
                    <table id="live_error_mess" width="90%" style="display:none;">
                        <tr>
                            <td class="formheading" colspan='2'><?=$lang->getLang("Live Betting Percentage");?></td>
                        </tr>
                        <tr>
                            <td class='content' colspan='2'><?=$lang->getLang("Before to insert percentage your up level has to set your percentage!")?></td>
                        </tr>
                    </table>
                    <table cellpadding=0 cellspacing="1" border=0 >
                        <tr>
                            <td colspan='2'>
                                <a href="javascript:void(0);" onclick="showAdminAdd();" id="adminAddShow"><?=$lang->getLang("Add an administrator for this %" , "Club")?> &#9660;</a>
                            </td>
                        </tr>
                    </table>
                    <table cellpadding=0 cellspacing="1" border=0 id='adminAdd' style="display: none">
                        <tr>
                            <td class="formheading" colspan='2'><?=$lang->getLang("Add admin");?></td>
                        </tr>
                        <tr valign="top">
                            <td>
                                <table cellpadding=4 cellspacing="1" border=0 width=350>
                                    <tr valign="top">
                                        <td class=label width=110><?=$lang->getLang("Username")?></td>
                                        <td <? form_td_style('username'); ?>><input type="text" name="username" value="<?=replace_quotes($_POST['username'])?>" size=30>
                                            <?php echo err_field('username'); ?>
                                        </td>
                                    </tr>
                                    <tr valign="top">
                                        <td class=label width=110><?=$lang->getLang("Password")?></td>
                                        <td <? form_td_style('password'); ?>><input type="password" name="password" value="<?=replace_quotes($_POST['password'])?>" size=30>
                                            <?php echo err_field('password'); ?>
                                        </td>
                                    </tr>
                                    <tr valign="top">
                                        <td class=label width=110><?=$lang->getLang("Confirm Password")?></td>
                                        <td <? form_td_style('password_confirm'); ?>><input type="password" name="password_confirm" value="<?=replace_quotes($_POST['password_confirm'])?>" size=30>
                                            <?php echo err_field('password_confirm'); ?>
                                        </td>
                                    </tr>
                                    <tr valign="top">
                                        <td class=label width=110><?=$lang->getLang("Full Name")?></td>
                                        <td <? form_td_style('full_name'); ?>><input type="text" name="full_name" value="<?=$_POST['full_name']?>" size=30>
                                            <?php echo err_field('full_name'); ?>
                                        </td>
                                    </tr>
                                    <tr valign="top">
                                        <td class=label width=110><?=$lang->getLang("Email")?></td>
                                        <td <? form_td_style('email'); ?>><input type="text" name="email" value="<?=$_POST['email']?>" size=30>
                                            <?php echo err_field('email'); ?></td>
                                    </tr>
                                    <tr valign="top">
                                        <td class=label><?=$lang->getLang("Country")?></td>
                                        <td <? form_td_style('user_country'); ?>>
                                            <?php if(DOT_IT) : ?>
                                                Italia<input type="hidden" name="user_country" value="IT"/>
                                            <?php else : ?>
                                                <? countrySelect('user_country', "", $_POST['user_country']); ?>
                                                <?php echo err_field('user_country'); ?>
                                            <?php endif; ?>
                                        </td>
                                    </tr>
                                    <tr valign="top">
                                        <td class=label><?=$lang->getLang("Type")?></td>
                                        <td <? form_td_style('user_type'); ?> >
                                            <select name='user_type'>
                                                <option value="6"><?=$lang->getLang("Club Manager")?></option>
                                            </select>

                                        </td>
                                    </tr>
                                    <tr valign="top">
                                        <td class=label width=110><?=$lang->getLang("Access")?></td>
                                        <td <? form_td_style('access'); ?>>
                                            <input type="radio" name="access" value="ALLOW" <? if ('ALLOW' == $_POST['access']) echo 'checked'; ?>><?=$lang->getLang("Allow")?>
                                            &nbsp;&nbsp;&nbsp;
                                            <input type="radio" name="access" value="DENY" <? if ('DENY' == $_POST['access']) echo 'checked'; ?>><?=$lang->getLang("Deny")?>
                                            <?php echo err_field('access'); ?>
                                        </td>
                                    </tr>

                                </table>
                            </td>
                        </tr>
                    </table>
                </table>
            </td>
            <td valign="top" colspan="2" width="80">
                <input type="submit" id="sub_update" style="width:60px;margin-bottom:2px;" class="entitybtn" value="<?=$lang->getLang("Save")?>" />
                <br />
                <input type="button" onClick="window.location='<?=$_SERVER['PHP_SELF']?>'" id="Cancel" class="entitybtn" style="width:60px;margin-bottom:2px" value="<?=$lang->getLang("cancel")?>" />
                <br />
            </td>
        </tr>
    </table>
</form>
<script type="text/javascript">
    $(document).ready(function(){
        getSelectedValue();
    });
    $(document).off('click','#apply_perc');
    $(document).on('click','#apply_perc', function (e) {
        e.preventDefault();
        var new_perc = parseInt($('#new_perc').val());
        if(new_perc >= 0 && new_perc <= 100) {
            $.each($('select[name*="JPS_PERC"]'), function () {
                $(this).val(new_perc);
            });
        }
    });
    function getSelectedValue(){
        $val = $('#district').val();
        if($val == 'none'){
            $.each($('#casino_perc').find('select[name*="JPS_PERC"]'),function () {
                $(this).val('35');
            });
        }
        $.post("/services/jurisdiction_services.inc", {"action":1,"jur_id":$val,"parent":"b"}, function(data){
            if(!data){
                $('#error_mess').show();
                $('#betting_perc').hide();
            }else{
                $('#betting_perc').show();
                $('#error_mess').hide();
            }
        });
        $.post("/services/jurisdiction_services.inc", {"action":3,"jur_id":$val,"parent":"b"}, function(data){
            if(data == -1){
                $('#error_mess_payments').show();
                $('#casino_perc').hide();
            }else{
                $('#casino_perc').show();
                if (data != '') {
                    var result = data.split("~");                    
                    var details = result[1].split(";");
                    for(var i = 0; i < details.length; i++){
                        var percDetail = details[i].split(":");
                        percDetail[1] = parseInt(percDetail[1]) - 15;
                        if(parseInt(percDetail[1]) < 0) percDetail[1] = 0;
                        $("#"+percDetail[0]).val(percDetail[1]);
                        $("#sel_"+percDetail[0]).val(percDetail[1]);
                    }
                    $('#error_mess_payments').hide();
                }
            }
        });
        $.post("/services/jurisdiction_services.inc", {"action":6,"jur_id":$val,"parent":"b"}, function(data){
            if(!data){
                $('#live_error_mess').show();
                $('#betting_live_perc').hide();
            }else{
                $('#betting_live_perc').show();
                $('#live_error_mess').hide();
            }
        });
    }
</script>
