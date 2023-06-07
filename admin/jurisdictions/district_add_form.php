<?php if ( areErrors() ) { showErrors(); echo "<br /><br />";}?>
<style>
    .pac-container:after{
        content:none !important;
    }
</style>
<script type="text/javascript" src="https://maps.googleapis.com/maps/api/js?libraries=places&sensor=false&&language=<?=$_SESSION['language']?>&region=US"></script>

<script type="text/javascript">
    $(function(){
        $('#city').on('focus',function(){
            input = this, options = {
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
        })
    });
</script>
<form action="<?php echo $_SERVER['PHP_SELF']?>" method="POST">
    <input type="hidden" name="class" value="district">
    <input type="hidden" name="op" value="Save">
    <table cellpadding=4 cellspacing=1 border="0">
        <tr valign="top">
            <td class="formheading" colspan="2"><?=$lang->getLang("Add a District")?></td>
            <td rowspan="3" width="80">
                <input type="submit" id="sub_save"  style="width:60px;margin-bottom:2px" value="<?=$lang->getLang("Save")?>" />
                <br />
                <input type="button" onClick="window.location='<?=$_SERVER['PHP_SELF']?>'"  style="width:60px" value="<?=$lang->getLang("cancel")?>" />
            </td>
        </tr>
        <tr valign="top">
            <td class="label"><?=$lang->getLang("District Name")?></td>
            <td class="content"><input type=text class="lettersAndNumbers" id="district_name" name="district_name" size="26" value="<?php if ( areErrors()  ) { echo $_POST['district_name']; }?>"></td>
        </tr>
        <?php
        if(empty($_POST['account_code'])){
            $_POST['account_code'] = get_free_jurcode();
        }
        ?>
        <tr>
            <td class=label width="130"><?=$lang->getLang("Account Code")?> <br /><div class="labelnote"><?=$lang->getLang("(6 Characters)")?></div></td>
            <td class=content><input type="text" name="account_code" value="<?=replace_quotes($_POST['account_code'])?>" size=11 /></td>
        </tr>
        <tr>
            <td class=label width=110><?=$lang->getLang("Access Type")?><div id="accessType"></div><br /></td>
            <td class=content><select name="accessType">
                    <option value="BACKOFFICE">Backoffice</option>
                    <option value="WEBSITE">Website</option>
                </select>
            </td>
        </tr>
        <tr valign="top">
            <td class=label><?=$lang->getLang("Region")?></td>
            <td class=content>

                <select name="region_id" id="region_id" onchange="getSelectedValue();">
                    <option value="none">None</option>
                    <?php
                    if($_SESSION["jurisdiction_class"] == "region"){
                        echo  get_jurisdiction_options('region', $id=$_SESSION['jurisdiction_id'], $hide_internet=true);
                    }else{
                        echo  get_jurisdiction_options('region', $_POST["region_id"], $hide_internet=true);
                    }
                    ?>
                </select>
            </td>
            <td></td>
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
        <?php 
        if ($_SESSION['jurisdiction_id'] == 1) { ?>
         <tr>
            <td class="label" width="130"><?=$lang->getLang("Limit new str.")?></td>
            <td class="content"><input type="text" name="childs_limit" id="childs_limit" value="20" size=20 /></td>
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
        <? } ?>
        <table id="casino_perc" width="90%">
            <?php
            include_once "funclib/percentual.inc.php";
            showPercentage($_SESSION["jurisdiction_id"],$_SESSION["jurisdiction_class"],-1,"district");
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
               showBettingPercentage($_SESSION["jurisdiction_id"],$_SESSION["jurisdiction_class"], "district",-1,-1);
            ?>
        </table>
        <table id="error_mess" width="90%" style="display:none;">
            <tr>
                <td class="formheading" colspan='2'><?=$lang->getLang("Betting Percentage");?></td>
            </tr>
            <tr>
                <td class='content' colspan='2'><?=$lang->getLang("Before to insert percentage to this new % your up level has to set your percentage!","district")?></td>
            </tr>
        </table>
        <table id="betting_live_perc" width="90%">
            <?php
            showLiveBettingPercentage($_SESSION["jurisdiction_id"],$_SESSION["jurisdiction_class"], "district",-1, -1);
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
                    <a href="javascript:void(0);" onclick="showAdminAdd();" id="adminAddShow"><?=$lang->getLang("Add an administrator for this %" , "district")?> &#9660;</a>
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
                            <td <? form_td_style('password_confirm'); ?>t><input type="password" name="password_confirm" value="<?=replace_quotes($_POST['password_confirm'])?>" size=30>
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
                                    <option value="5"><?=$lang->getLang("District Manager")?></option>
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
</form>
<script type="text/javascript">
    function showAdminAdd(){
        if ($("#adminAdd").is(":hidden")){
            $("#adminAdd").fadeIn("slow");
            $("#adminAddShow").html("<?=$lang->getLang("Don't add an administrator")?> &#9650;");
        }
        else{
            $("#adminAdd").fadeOut("slow");
            $('input[name="username"]').val('');
            $("#adminAddShow").html("<?=$lang->getLang("Add an administrator for this %" , "district")?> &#9660;");
        }
        return false;
    }
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
        $val = $('#region_id').val();
        if($val == 'none'){
            $.each($('#casino_perc').find('select[name*="JPS_PERC"]'),function () {
                $(this).val('45');
            });
        }
        $.post("/services/jurisdiction_services.inc", {"action":1,"jur_id":$val,"parent":"b"}, function(data){
            if(!data){
                $('#error_mess').show();
                $('#betting_perc').hide();
            }else{
                if(data==1){
                    $('#betting').val(data).trigger('change');
                    $('#betting option:not(:selected)').attr('disabled', true);
                    $('#betting option[value="1"],  #betting option[value="3"]').attr('disabled', false);
                    $('#betting_perc').show();
                    $('#error_mess').hide();
                }
                else{
                    $('#betting option').attr('disabled', false);
                }
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

    
    
    