<script type="text/javascript" src="https://maps.googleapis.com/maps/api/js?libraries=places&sensor=false&&language=en&region=US"></script>
<script language="javascript" type="text/javascript">
    $(function(){
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
        })
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

    function doRefresh(form) {
        var msg = "<?=$lang->getLang("Refreshing will reload the saved description for this %, causing any unsaved changes to be lost. Are you sure you want to do this?",'district')?>";

        if ( window.confirm(msg) ) {
            window.location = '<?$_SERVER['PHP_SELF']?>?node=<?=$node;?>';
        }
    }
    <? if (check_access("delete_jurisdictions", false)) :?>
    function deleteJurisdiction(jur_id){
        jConfirm("<?=$lang->getLang("Block %?",$district_name)?>","",function (answer) {
            if(answer){
                var lock_users = false;
                var lock_punters = false;
                jConfirm("<?=$lang->getLang("Lock all % of % structure?",$lang->getLang("administrators"),$district_name)?>","",function (answer) {
                    lock_users = answer;
                    jConfirm("<?=$lang->getLang("Lock all % of % structure?",$lang->getLang("players"),$district_name)?>","",function (answer) {
                        lock_punters = answer;
                        location.href='<?=$_SERVER['PHP_SELF']?>?op=Delete&node=' + jur_id + "&lock_users=" + lock_users + "&lock_punters=" + lock_punters;
                    });
                });
            }
        });
        return false;
    }
    function restoreJurisdiction(jur_id){
        jConfirm("<?=$lang->getLang("Unblock %?",$district_name)?>","",function (answer) {
            if(answer){
                var unlock_users = false;
                var unlock_punters = false;
                jConfirm("<?=$lang->getLang("Unlock all % of % structure?",$lang->getLang("administrators"),$district_name)?>","",function (answer) {
                    unlock_users = answer;
                    jConfirm("<?=$lang->getLang("Unlock all % of % structure?",$lang->getLang("players"),$district_name)?>","",function (answer) {
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
<?$canModify=check_access('manage_jurisdiction_update')?>
<?if($canModify) {?>
<form name="district_display_form" action="<?=$_SERVER['PHP_SELF']?>" method="POST">
    <input type="hidden" name="node" value="<?=$node?>" />
    <input type="hidden" name="class" value="district" />
    <input type="hidden" name="op" value="Update" />
    <input type="hidden" name="jur_status" value="<?=$jur_status?>" />
  <? }  ?>
    <?php if ( areErrors() ) { showErrors(); echo "<br /><br />";} ?>
    <table cellpadding= "0" cellspacing=1 border=0>
        <tr valign="top">
            <td>
                <table cellpadding= "4" cellspacing=1 border=0>
                    <tr valign="top">
                        <td colspan="2" class="formheading"><?=$lang->getLang("Details of")?> <?=$district_name?></td>
                    </tr>
                    <tr valign="top">
                        <td class=label><?=$lang->getLang("District Name")?></td>
                        <td class=content><input type="text" class="lettersAndNumbers" name="district_name" id="district_name" value="<?=$district_name;?>" /></td>
                        <td>&nbsp;</td>
                    </tr>
                    <tr>
                        <td class=label width=110><?=$lang->getLang("Account Code")?><br /><div class="labelnote"><?=$lang->getLang("(3 characters)")?></div></td>
                        <td class=content><?=replace_quotes($account_code)?></td>
                    </tr>
                    <tr>
                        <td class=label width=110><?=$lang->getLang("Access Type")?><div id="accessType"></div><br /></td>
                        <td class=content><select name="accessType">
                                <option value="BACKOFFICE" <?=($accessType=='BACKOFFICE'?"selected":'')?> >Backoffice</option>
                                <option value="WEBSITE" <?=($accessType=='WEBSITE'?"selected":'')?>>Website</option>
                            </select>
                        </td>
                    </tr>
                    <tr valign="top">
                        <td class=label><?=$lang->getLang("Region")?></td>
                        <td class=content>
                            <select name="region_id" id="region_id" onchange="getSelectedValue();">
                                <?php
                                if($_SESSION["jurisdiction_class"] == "region"){
                                    echo  get_jurisdiction_options('region', $id=$_SESSION['jurisdiction_id'], $hide_internet=true);
                                }else{
                                    echo  get_jurisdiction_options('region', $region_id, $hide_internet=true);
                                }
                                ?>
                            </select>
                            <input type="hidden" name="currency" value="<?=$currency?>" />
                        </td>
                        <td>&nbsp;</td>
                    </tr>
                    <tr>
                        <td class=label width=110><?=$lang->getLang("Currency Type")?><br /></td>
                        <td class=content><?$currency=getCurrentCurrency($node);?> <?=$currency['cur_name']?> ( <?=$currency['cur_code_for_web']?> )</td>
                    </tr>
                    <tr>
                        <td class=label width=110><?=$lang->getLang("Available Credit")?><br /></td>
                        <td class=content>
                            <button class="entitybtn  fleft" type="button" id="showJurisdictionCurrencyButon"><?=$lang->getLang("Show")?></button>
                            <div id="jurisdictionCreditsContainer" class="hidden"><?=getInDollars($row["jur_available_credit"],2,getCurrentCurrency($node,true)) ;?></div>
                        </td>
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
                    <tr>
                        <td class=label width=110><?=$lang->getLang("Creation Time")?><br /></td>
                        <td class=content><?=date("Y-m-d H:i", strtotime($row["jur_creation_date"])) ?></td>
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
                        <td class=label width=110><?=$lang->getLang("Timezone")?><br /></td>
                        <td class=content><?=$row["jur_time_utc"]?>:00 UTC</td>
                    </tr>
                                 <?php if ($_SESSION['jurisdiction_id'] == 1) { ?>
                                 <tr>
                                    <td class="label" width="130"><?=$lang->getLang("Limit new str.")?></td>
                                    <td class="content"><input type="text" name="childs_limit" id="childs_limit" value="<?=$jurChildsLimit?>" size=20 /></td>
                                </tr>
                                <? } ?>
                    <? if (check_access("delete_jurisdictions", false)) :?>
                        <? if($jur_status == 1) : ?>
                            <tr valign="top">
                                <td class="label" width="130"><?=$lang->getLang("Access")?></td>
                                <td class="content"><button onclick="return deleteJurisdiction(<?=$node?>);"><?=$lang->getLang("Block %?",$district_name)?></button></td>
                            </tr>
                        <?php endif; ?>
                        <? if($jur_status == 0) : ?>
                            <tr valign="top">
                                <td class="label" width="130"><?=$lang->getLang("Access")?></td>
                                <td class="content"><button onclick="return restoreJurisdiction(<?=$node?>);"><?=$lang->getLang("Unblock %?",$district_name)?></button></td>
                            </tr>
                        <?php endif; ?>
                    <?php endif; ?>

                    <tr valign="top">
                        <td class="label"><?=$lang->getLang("Processor enabled")?></td>
                        <td class="content">
                            <select class="select2" style="width: 100%; max-width: 250px;" name="processors[]" id="processors" multiple>
                                <?php
                                foreach ($processors as $proc) {
                                    ?>
                                    <option value="<?=$proc['ppc_code']?>" <? if(in_array($proc['ppc_code'],$u_processors)) echo 'selected'; ?>><?=$proc['prc_name']?></option>
                                <?php
                                }
                                ?>
                            </select>
                            <br>
                            <button id="update_processor" data-jur="<?=$node?>" data-name="<?=$district_name?>" data-class="district" class="btn btn-small"><?=$lang->getLang("Update sub")?></button>
                        </td>
                    </tr>
                    <table id="casino_perc" width="90%">
                        <?php
                        include_once "funclib/percentual.inc.php";
                        showPercentage($_SESSION["jurisdiction_id"],$_SESSION["jurisdiction_class"],$node, "district");

                        ?>
                    </table>
                    <table id="error_mess_payments" width="90%" style="display:none;">
                        <tr>
                            <td class="formheading" colspan='2'><?=$lang->getLang("Payments Percentage");?></td>
                        </tr>
                        <tr>
                            <td class='content' colspan='2'><?=$lang->getLang("Before to insert percentage to this new % your up level has to set your percentage!","district")?></td>
                        </tr>
                    </table>
                    <table id="betting_perc" width="90%">
                        <?php
                        showBettingPercentage($_SESSION["jurisdiction_id"],$_SESSION["jurisdiction_class"], "district", $node, check_access("betting_percentage_setting"));
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
                        showLiveBettingPercentage($_SESSION["jurisdiction_id"],$_SESSION["jurisdiction_class"], "district",$node, check_access("betting_percentage_setting"));
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
            <td width=80>
                <!--   <input type="submit" id="sub_add" name="op" style="width:60px;margin-bottom:2px;" value="Add" />
                   <br />   -->
                <?if($canModify) {?>
                <input type="submit" class="entitybtn disabledBtn" id="sub_update" style="width:60px;margin-bottom:2px;" value="<?=$lang->getLang("Update")?>" />
                <br />
                <input type="button" class="entitybtn disabledBtn" id="btn_refresh" style="width:60px;margin-bottom:2px" value="<?=$lang->getLang("Refresh")?>" onClick="doRefresh(this.form)"/>
                    <br />
                <? } ?>

                <?if(check_access('manage_admin_users')){ ?>

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
    <?if($canModify) {?>
</form>
<? } ?>
<script type="text/javascript">
    $(document).ready(function(){
        getSelectedValue();
    });
    function getSelectedValue(){
        $val = $('#region_id').val();
        $.post("/services/jurisdiction_services.inc", {"action":1,"jur_id":$val,"parent":"b"}, function(data){
            if(!data){
                $('#error_mess').show();
                $('#betting_perc').hide();
            }else{
                if(data==1){
                   /* $('#betting').val(data).trigger('change');*/
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