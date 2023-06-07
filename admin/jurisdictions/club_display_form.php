<script type="text/javascript" src="https://maps.googleapis.com/maps/api/js?libraries=places&sensor=false&&language=en&region=US"></script>
<script language="javascript" type="text/javascript">
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
        })

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
        
        var startTimeTextBox = $('#hoursStart');
        var endTimeTextBox = $('#hoursEnd');

        $.timepicker.timeRange(
            startTimeTextBox,
            endTimeTextBox,
            {
                minInterval: (1000*60*30), // 1hr
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

    function doRefresh(form){
        var msg = "<?=$lang->getLang("Refreshing will reload the saved description for this %, causing any unsaved changes to be lost. Are you sure you want to do this?","club")?>";

        if ( window.confirm(msg) )
            window.location = '<?$_SERVER['PHP_SELF']?>?node=<?=$node;?>';
    }

    <? if (check_access("delete_jurisdictions", false)) :?>
    function deleteJurisdiction(jur_id){
        jConfirm("<?=$lang->getLang("Block %?",$club_name)?>","",function (answer) {
            if(answer){
                var lock_users = false;
                var lock_punters = false;
                jConfirm("<?=$lang->getLang("Lock all % of % structure?",$lang->getLang("administrators"),$club_name)?>","",function (answer) {
                    lock_users = answer;
                    jConfirm("<?=$lang->getLang("Lock all % of % structure?",$lang->getLang("players"),$club_name)?>","",function (answer) {
                        lock_punters = answer;
                        location.href='<?=$_SERVER['PHP_SELF']?>?op=Delete&node=' + jur_id + "&lock_users=" + lock_users + "&lock_punters=" + lock_punters;
                    });
                });
            }
        });
        return false;
    }
    function restoreJurisdiction(jur_id){
        jConfirm("<?=$lang->getLang("Unblock %?",$club_name)?>","",function (answer) {
            if(answer){
                var unlock_users = false;
                var unlock_punters = false;
                jConfirm("<?=$lang->getLang("Unlock all % of % structure?",$lang->getLang("administrators"),$club_name)?>","",function (answer) {
                    unlock_users = answer;
                    jConfirm("<?=$lang->getLang("Unlock all % of % structure?",$lang->getLang("players"),$club_name)?>","",function (answer) {
                        unlock_punters = answer;
                        location.href='<?=$_SERVER['PHP_SELF']?>?op=Restore&node=' + jur_id + "&unlock_users=" + unlock_users + "&unlock_punters=" + unlock_punters;
                    });
                });
            }
        });
        return false;
    }
    <?php endif; ?>

</script>
<?php
$params=explode('~',$params);
foreach($params as $key => $value){
    list($label, $content) =  explode(":", $value);
    if($label == 'tax'){
        $taxDetails = explode(";",$content);
        $taxKey=$key;
    }
    if($label=='conf'){
        $conf = explode(";",$content);
        $confKey=$key;
    }
}

$district_id = isset($_POST["district"]) ? $_POST['district'] : $district;
?>
<?php if ( areErrors() ) { showErrors(); echo "<br />";}?>

<?$canModify=check_access('manage_jurisdiction_update')?>
<?if($canModify) {?>
<form name="club_display_form" action="<?php echo $_SERVER['PHP_SELF']?>" method="POST">
    <input type=hidden name=node value="<?=$node;?>" />
    <input type=hidden name=class value="club" />
    <input type=hidden name="op" value="Update" />
    <input type=hidden name="jur_status" value="<?=$jur_status?>" />
    <? } ?>
    <table cellpadding=0 cellspacing="1" border="0">
        <tr>
            <td>
                <table cellpadding="4" cellspacing="1" border="0" width="370">
                    <tr>
                        <td class="formheading" colspan="2"><?=$lang->getLang("Details of")?> <?=$club_name?></td>
                    </tr>
                    <tr>
                        <td class="label" width="130"><?=$lang->getLang("Club Name")?></td>
                        <td class="content"><input type="text" class="lettersAndNumbers" name="club_name" value="<?=replace_quotes($club_name)?>" size=30 /></td>
                    </tr>
                    <tr>
                        <td class="label" width="130"><?=$lang->getLang("Account Code")?> </td>
                        <td class="content"><?=replace_quotes($account_code)?></td>
                    </tr>
                    <tr>
                        <td class=label width=110><?=$lang->getLang("Access Type")?><div id="accessType"></div><br /></td>
                        <td class=content><select name="accessType">
                                <option value="BACKOFFICE" <?=($accessType=='BACKOFFICE'?"selected":'')?> >Backoffice</option>
                                <option value="WEBSITE" <?=($accessType=='WEBSITE'?"selected":'')?>>Website</option>
                            </select>
                        </td>
                    </tr>
                    <tr>
                        <td class=label width=110><?=$lang->getLang("Currency Type")?><br /></td>
                        <td class=content><?$currency=getCurrentCurrency($node);?> <?=$currency['cur_name']?> ( <?=$currency['cur_code_for_web']?> )</td>
                    </tr>
                    <tr>
                        <td class=label width=110><?=$lang->getLang("Available Credit")?><br /></td>
                        <td class=content>
                            <button class="entitybtn fleft" type="button" id="showJurisdictionCurrencyButon"><?=$lang->getLang("Show")?></button>
                            <div id="jurisdictionCreditsContainer" class="hidden"><?=getInDollars($row["jur_available_credit"],2,$currency['cur_code_for_web']) ;?></div>
                        </td>
                    </tr>
                    <tr>
                        <td class=label width=110><?=$lang->getLang("Timezone")?><br /></td>
                        <td class=content><?=$row["jur_time_utc"]?>:00 UTC</td>
                    </tr>
                    <tr>
                        <td class=label width=110><?=$lang->getLang("Skin")?><br /></td>
                        <td class=content><?$skin=getCurrentSkin($node);
                            if(($skin['skn_name']=='' || is_null($skin['skn_name']))&& $skinId!=-1)
                            {
                                $skin['skn_name']=$lang->getLang("All");
                            }
                            elseif($skinId==-1){
                                $skin['skn_name']=$lang->getLang("Skin not configured");
                            }

                            ?> <?=$skin['skn_name']?></td>
                    </tr>
                    <?if($affiliateUsername!='') { ?>
                    <tr>
                        <td class=label width=110><?=$lang->getLang("Affiliate")?><br /></td>
                        <td class=content>
                            <?=getClassLink(refPage("customers/customer_view&customer_id={$affiliateId}"), $affiliateUsername, '', "_blank");?>
                        </td>
                    </tr>
                     <? } ?>

                    <tr>
                        <td class="label" width="130"><?=$lang->getLang("District")?></td>
                        <td class="content">
                            <select name="district" id="district" onchange="getSelectedValue();">
                                <?php
                                if($_SESSION["jurisdiction_class"] == "district"){
                                    echo  get_jurisdiction_options('district', $id=$_SESSION['jurisdiction_id'], $hide_internet=true);
                                }else{
                                    echo  get_jurisdiction_options('district', $district_id, $hide_internet=true);
                                }
                                ?>
                            </select>
                            <input type="hidden" name="currency" value="<?=$currency?>" />
                        </td>
                        <td> </td>
                    </tr>
                    <tr>
                        <td class=label><?=$lang->getLang("Country")?></td>
                        <td <? form_td_style('country'); ?>>
                            <?countrySelect('country', "", $country,false,'country'); ?>
                            <?php echo err_field('user_country'); ?>
                        </td>
                    </tr>

                    <tr>
                        <td class="label" width="130"><?=$lang->getLang("City")?></td>
                        <td class="content"><input type="text" name="city" id="city" value="<?=$city?>" size=30 /></td>
                    </tr>

                    <tr>
                        <td class="label" width="130"><?=$lang->getLang("Address Line 1")?></td>
                        <td class="content"><input type="text" name="address1" id="address" value="<?=replace_quotes($address1)?>" size=30 /></td>
                    </tr>
                    <tr>
                        <td class="label" width="130"><?=$lang->getLang("Address Line 2")?></td>
                        <td class="content"><input type="text" name="address2" value="<?=replace_quotes($address2)?>" size=30 /></td>
                    </tr>

                    <tr>
                        <td class="label" width="130"><?=$lang->getLang("Postcode or Zipcode")?></td>
                        <td class="content"><input type="text" name="postc" value="<?=replace_quotes($postc)?>" size=30 /></td>
                    </tr>
                    <tr>
                        <td class="label" width="130"><?=$lang->getLang("Longitude")?></td>
                        <td class="content">
                            <input type="text" id="longitudeS" disabled="disabled" value="<?=replace_quotes($longitude)?>" size=30 />
                            <input type="hidden" name="longitude" id="longitude" value="<?=replace_quotes($longitude)?>" size=30 />
                        </td>
                    </tr>
                    <tr>
                        <td class="label" width="130"><?=$lang->getLang("Latitude")?></td>
                        <td class="content">
                            <input type="text"  id="latitudeS" disabled="disabled" value="<?=replace_quotes($latitude)?>" size=30 />
                            <input type="hidden" name="latitude" id="latitude"  value="<?=replace_quotes($latitude)?>" size=30 />
                        </td>
                    </tr>
                    <tr>
                        <td class="label" width="130"><?=$lang->getLang("Working Hours")?> </td>
                        <td class="content">
                            <input type="text" name="hoursStart" id="hoursStart" value="<?=$hoursStart?>" size=10 > -
                            <input type="text" name="hoursEnd" id="hoursEnd" value="<?=$hoursEnd?>" size=10 >

                        </td>
                    </tr>

                    <tr>
                        <td class="label" width="130"><?=$lang->getLang("Contact Number")?> </td>
                        <td class="content"><input type="text" name="phone" value="<?=replace_quotes($phone)?>" size=30 /></td>
                    </tr>
                   <?php if ($_SESSION['jurisdiction_id'] == 1) { ?>
                        <tr>
                            <td class="label" width="130"><?=$lang->getLang("Limit new users")?></td>
                            <td class="content"><input type="text" name="users_limit" id="users_limit" value="<?=$jurUsersLimit?>" size=20 /></td>
                        </tr>
                    <? } ?>
                    <?php if(defined("DOT_IT") && DOT_IT == true) : ?>
                        <tr>
                            <td class="label" width="130"><?=$lang->getLang("VAT")?></td>
                            <td class="content"><?= replace_quotes($vatCode) ?></td>
                        </tr>
                    <?php endif; ?>
                    <tr>
                        <td class="label" width="130"><?=$lang->getLang("Creation Date")?></td>
                        <td class="content"><?= date("Y-m-d H:i", strtotime($row["jur_creation_date"])) ?></td>
                    </tr>
                    <? if (check_access("delete_jurisdictions", false)) :?>
                        <? if($jur_status == 1) : ?>
                        <tr valign="top">
                            <td class="label" width="130"><?=$lang->getLang("Access")?></td>
                            <td class="content"><button onclick="return deleteJurisdiction(<?=$node?>);"><?=$lang->getLang("Block %?",$club_name)?></button></td>
                        </tr>
                        <?php endif; ?>
                        <? if($jur_status == 0) : ?>
                            <tr valign="top">
                                <td class="label" width="130"><?=$lang->getLang("Access")?></td>
                                <td class="content"><button onclick="return restoreJurisdiction(<?=$node?>);"><?=$lang->getLang("Unblock %?",$club_name)?></button></td>
                            </tr>
                        <?php endif; ?>
                    <?php endif; ?>

                    <tr valign="top">
                        <td class="label"><?=$lang->getLang("Processor enabled")?></td>
                        <td class="content">
                            <select class="select2" style="width: 100%; max-width: 250px;" name="processors[]" multiple>
                                <?php
                                foreach ($processors as $proc) {
                                    ?>
                                    <option value="<?=$proc['ppc_code']?>" <? if(in_array($proc['ppc_code'],$u_processors)) echo 'selected'; ?>><?=$proc['prc_name']?></option>
                                <?php
                                }
                                ?>
                            </select>
                        </td>
                    </tr>

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

                                    <?
                                    $disabled=true;
                                    $taxVal='';
                                    foreach($conf as $k=>$v){
                                        $v=explode('=',$v);
                                        if($v[0]=='virtual_jackpot' && $v[1]=='ON'){
                                            $disabled=false;
                                            $taxVal=$v[1];
                                        }
                                    }
                                    ?>
                                    <td><label><input type="checkbox" <?=!$disabled?'checked':''?>   class="taxCheckbox" value="virtual_jackpot"></label></td>
                                    <td>Virtual jackpot</td>
                                    <td>ON<input type="hidden" <?=!$disabled?'':'disabled'?> id="virtual_jackpot" class="tax"   name="virtual_jackpot" style="width: 50px"  value="ON"></td>
                                </tr>

                            </table>
                        </td>
                    </tr>


                    <tr valign="top">
                        <td class="label" width="130"><?=$lang->getLang("Notes")?></td>
                        <td class="content"><textarea name="notes" rows="14" cols="40" wrap="hard"><?=replace_quotes($notes);?></textarea></td>
                    </tr>
                    <table id="casino_perc" width="90%">
                        <?php
                        include_once "funclib/percentual.inc.php";
                        showPercentage($_SESSION["jurisdiction_id"],$_SESSION["jurisdiction_class"],$node, "club");

                        ?>
                    </table>
                    <table id="error_mess_payments" width="90%" style="display:none;">
                        <tr>
                            <td class="formheading" colspan='2'><?=$lang->getLang("Payments Percentage");?></td>
                        </tr>
                        <tr>
                            <td class='content' colspan='2'><?=$lang->getLang("Before to insert percentage to this new % your up level has to set your percentage!","club")?></td>
                        </tr>
                    </table>
                    <table id="betting_perc" width="90%">
                        <?php
                            showBettingPercentage($_SESSION["jurisdiction_id"],$_SESSION["jurisdiction_class"], "club",$node,check_access("betting_percentage_setting"));
                        ?>
                    </table>
                    <table id="error_mess" width="90%" style="display:none;">
                        <tr>
                            <td class="formheading" colspan='2'><?=$lang->getLang("Betting Percentage");?></td>
                        </tr>
                        <tr>
                            <td class='content' colspan='2'><?=$lang->getLang("Before to insert percentage your up level has to set your percentage!")?></td>
                        </tr>
                    </table>
                    <table id="betting_live_perc" width="90%">
                        <?php
                        showLiveBettingPercentage($_SESSION["jurisdiction_id"],$_SESSION["jurisdiction_class"], "club",$node, check_access("betting_percentage_setting"));
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
                </table>
            </td>
            <td  valign="top" colspan="2" width="70">
                <?if($canModify) {?>
                <input type="submit" id="sub_update" style="width:60px;margin-bottom:2px;" class="entitybtn disabledBtn" value="<?=$lang->getLang("Update")?>" />
                <br />
                <input type="button" id="btn_refresh" style="width:60px;margin-bottom:2px" class="entitybtn disabledBtn" value="<?=$lang->getLang("Refresh")?>" onClick="doRefresh(this.form)"/>
                <br />
                <? } ?>
                <?if(check_access('manage_admin_users')){
                    ?>
                    <a class="addAdminUser" href="/admin_user_add">Add an administrator</a>
                    <?
                }

                if(check_access('manage_admin_users', false)){
                    if(!empty($jur_list)){
                        $jur_list->getAdminUsersList($node);
                    }
                }
                ?>
            </td>
        </tr>
    </table>
    <?$canModify=check_access('manage_jurisdiction_update')?>
    <?if($canModify) {?>
</form>
<? }?>
<script type="text/javascript">
    $(document).ready(function(){
        getSelectedValue();
    });

    function getSelectedValue(){
        $val = $('#district').val();
        $.post("/services/jurisdiction_services.inc", {"action":1,"jur_id":$val,"parent":"b"}, function(data){
            if(!data){
                $('#error_mess').show();
                $('#betting_perc').hide();
            }else{
                $('#betting_perc').show();
                $('#error_mess').hide();
            }
        });
        $.post("/services/jurisdiction_services.inc", {"action":3,"jur_id":$val,"parent":"b","node":<?=$node?>}, function(data){
            if(data == -1){
                $('#error_mess_payments').show();
                $('#casino_perc').hide();
            }else{
                var result = data.split("~");
                $('#casino_perc').show();
                var details = result[1].split(";");
                for(var i = 0; i < details.length; i++){
                    var percDetail = details[i].split(":");
                    if(parseInt(result[0]) === 1) {

                        percDetail[1] = parseInt(percDetail[1]) - 15;
                        if(parseInt(percDetail[1]) < 0) percDetail[1] = 0;

                        $("#sel_"+percDetail[0]).css("color","red");
                        $("#text_"+percDetail[0]).html("<span style='color:red;' ><?=$lang->getLang('Not saved')?></span>");
                    }else{
                        $("#sel_"+percDetail[0]).css("color","black");
                        $("#text_"+percDetail[0]).html("");
                    }
                    $("#"+percDetail[0]).val(percDetail[1]);
                    $("#sel_"+percDetail[0]).val(percDetail[1]);
                }
                $('#error_mess_payments').hide();
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
