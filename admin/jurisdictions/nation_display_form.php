<?
/*$menu_info=file_get_contents('../../config/menu.ini');
$skin_properties=file_get_contents('../../config/skinsproperties.ini');
$dbh->exec('SET SESSION group_concat_max_len = 4000000');
*/?><!--
--><?/* $games_info=$dbh->queryOne("select concat ('[slotmachines]\n\n',
            coalesce((select GROUP_CONCAT('\tgames[] = ', ggi_button_image_name, '@', ggi_game_id, '@', ggi_width, '@', ggi_height, '@', ggi_pes_group, '@', ggi_has_forfun, '@', ggi_web_name SEPARATOR '\n')
            from game_global_info
            where ggi_category = 'slots' and (ggi_pes_group is null or ggi_pes_group = '0' or ggi_pes_group = '')), ''),
            '\n\n[videopoker]\n\n',
            coalesce((select GROUP_CONCAT('\tgames[] = ', ggi_button_image_name, '@', ggi_game_id, '@', ggi_width, '@', ggi_height, '@', ggi_pes_group, '@', ggi_has_forfun, '@', ggi_web_name SEPARATOR '\n')
            from game_global_info
            where ggi_category = 'video poker' and (ggi_pes_group is null or ggi_pes_group = '0' or ggi_pes_group = '')), ''),
            '\n\n[tablegames]\n\n',
            coalesce((select GROUP_CONCAT('\tgames[] = ', ggi_button_image_name, '@', ggi_game_id, '@', ggi_width, '@', ggi_height, '@', ggi_pes_group, '@', ggi_has_forfun, '@', ggi_web_name SEPARATOR '\n')
            from game_global_info
            where ggi_category = 'table games' and (ggi_pes_group is null or ggi_pes_group = '0' or ggi_pes_group = '')),''),
            '\n\n[live]\n\n',
            coalesce((select GROUP_CONCAT('\tgames[] = ', ggi_button_image_name, '@', ggi_game_id, '@', ggi_width, '@', ggi_height, '@', ggi_pes_group, '@', ggi_has_forfun, '@', ggi_web_name SEPARATOR '\n')
            from game_global_info
            where ggi_category = 'live casino' and (ggi_pes_group is null or ggi_pes_group = '0' or ggi_pes_group = '')), ''),
            '\n\n[commondraw]\n\n',
            coalesce((select GROUP_CONCAT('\tgames[] = ', ggi_button_image_name, '@', ggi_game_id, '@', ggi_width, '@', ggi_height, '@', ggi_pes_group, '@', ggi_has_forfun, '@', ggi_web_name SEPARATOR '\n')
            from game_global_info
            where ggi_category = 'common draw' and (ggi_pes_group is null or ggi_pes_group = '0' or ggi_pes_group = '')), ''),
            '\n\n[dicegames]\n\n',
            coalesce((select GROUP_CONCAT('\tgames[] = ', ggi_button_image_name, '@', ggi_game_id, '@', ggi_width, '@', ggi_height, '@', ggi_pes_group, '@', ggi_has_forfun, '@', ggi_web_name SEPARATOR '\n')
            from game_global_info
            where ggi_category = 'dice games' and (ggi_pes_group is null or ggi_pes_group = '0' or ggi_pes_group = '')), ''),
            '\n\n[virtual]\n\n',
            coalesce((select GROUP_CONCAT('\tgames[] = ', ggi_button_image_name, '@', ggi_game_id, '@', ggi_width, '@', ggi_height, '@', ggi_pes_group, '@', ggi_has_forfun, '@', ggi_web_name SEPARATOR '\n')
            from game_global_info
            where ggi_category = 'virtual' and (ggi_pes_group is null or ggi_pes_group = '0' or ggi_pes_group = '')), '')
            ) as game_info");*/

//  var_dump($games_info);

?>
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

    function doRefresh(form){
        var msg = "<?=$lang->getLang("Refreshing will reload the saved description for this %, causing any unsaved changes to be lost. Are you sure you want to do this?","nation")?>";
        if ( window.confirm(msg)){
            window.location = "<?=$_SERVER['PHP_SELF'].'?node='.$node?>";
        }
    }

    <? if (check_access("delete_jurisdictions", false)) :?>
    function deleteJurisdiction(jur_id){
        jConfirm("<?=$lang->getLang("Block %?",$nation_name)?>","",function (answer) {
            if(answer){
                var lock_users = false;
                var lock_punters = false;
                jConfirm("<?=$lang->getLang("Lock all % of % structure?",$lang->getLang("administrators"),$nation_name)?>","",function (answer) {
                    lock_users = answer;
                    jConfirm("<?=$lang->getLang("Lock all % of % structure?",$lang->getLang("players"),$nation_name)?>","",function (answer) {
                        lock_punters = answer;
                        location.href='<?=$_SERVER['PHP_SELF']?>?op=Delete&node=' + jur_id + "&lock_users=" + lock_users + "&lock_punters=" + lock_punters;
                    });
                });
            }
        });
        return false;
    }
    function restoreJurisdiction(jur_id){
        jConfirm("<?=$lang->getLang("Unblock %?",$nation_name)?>","",function (answer) {

            if(answer){
                var unlock_users = false;
                var unlock_punters = false;
                jConfirm("<?=$lang->getLang("Unlock all % of % structure?",$lang->getLang("administrators"),$nation_name)?>","",function (answer) {
                    unlock_users = answer;
                    jConfirm("<?=$lang->getLang("Unlock all % of % structure?",$lang->getLang("players"),$nation_name)?>","",function (answer) {
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
<?php if ( areErrors() ) { showErrors(); echo "<br /><br />";}?>
<?$canModify=check_access('manage_jurisdiction_update');
if($canModify) { ?>

<form name="nation_display_form" action="<?=$_SERVER['PHP_SELF']?>" method="POST">
    <input type="hidden" name="node" value="<?=$node?>" />
    <input type="hidden" name="class" value="nation" />
    <input type="hidden" name="op" value="Update" />
    <input type="hidden" name="jur_status" value="<?=$jur_status?>" />
    <? } ?>
    <table cellpadding=0 cellspacing=0 border=0>
        <tr valign="top">
            <td>
                <table cellpadding= "0" cellspacing=1 border=0>
                    <tr valign="top">
                        <td>
                            <table cellpadding= "4" cellspacing=1 border=0>
                                <tr valign="top">
                                    <td class="formheading" colspan="2"><?=$lang->getLang("Details of")?> <?=$nation_name?></td>
                                </tr>
                                <tr valign="top">
                                    <td class=label><?=$lang->getLang("Nation Name")?></td>
                                    <td class=content><input type="text" class="lettersAndNumbers" name="nation_name" id="nation_name" value="<?=$nation_name;?>" /></td>
                                    <td></td>
                                </tr>
                                <tr>
                                    <td class=label width=110><?=$lang->getLang("Account Code")?><br /><div class="labelnote"><?=$lang->getLang("(6 characters)")?></div></td>
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
                                <tr>
                                    <td class=label width=110><?=$lang->getLang("Currency Type")?><br /></td>
                                    <td class=content><?=getAllCurrency($currency);?></td>
                                </tr>
                                <tr>
                                    <td class=label width=110><?=$lang->getLang("Available Credit")?><br /></td>
                                    <td class=content>
                                        <button class="entitybtn fleft" type="button" id="showJurisdictionCurrencyButon"><?=$lang->getLang("Show")?></button>
                                        <div id="jurisdictionCreditsContainer" class="hidden"><?=getInDollars($row["jur_available_credit"],2,getCurrentCurrency($node,true)) ;?></div>
                                    </td>
                                </tr>
                                <tr>
                                    <td class=label width=110><?=$lang->getLang("Skin")?><div id="skn"></div><br /></td>
                                    <!--<td class=content><?/*$skin=getCurrentSkin($node);
                                        $allSkins=getJurisdictionSkins($node,'nation');
                                        if(($allSkins->getNumRows()==0 || $allSkins->getNumRows()==1 )) {
                                            if($allSkins->getNumRows()==1  ){ */?>
                                                <?/*=createSkinSelect($allSkins,$skin['skn_id'],($skinId==-1? $skinId:false));*/?>
                                        <?/*    }
                                            elseif($skinId==-1) { */?>
                                                <?/*=$lang->getLang("Skin not configured")*/?>
                                           <?/* }
                                            else{*/?>
                                                <?/*=($skin['skn_name']!=''?$skin['skn_name']:'Casino')*/?>
                                            <?/*}
                                         }
                                        elseif($skinId==-1){ */?>
                                            <?/*=$lang->getLang("Skin not configured")*/?>
                                        <?/* }
                                        else{*/?>
                                            <?/*=($skin['skn_name']!=''?$skin['skn_name']:'Casino')*/?>
                                        <?/*};*/?>
                                    </td>-->
                                    <td class='content'>
                                        <select name="skin" id="skin">
                                            <?$allSkins=$dbh->exec("Select * from skins");
                                            while ($allSkins->hasNext()) {
                                                $row2 = $allSkins->next();
                                                ?>
                                                <option value="<?=$row2['skn_id']?>" <?=($skinId==$row2['skn_id']? 'selected':'') ?>><?=$row2['skn_name']?></option>
                                                <?
                                            }
                                            ?>
                                        </select>
                                    </td>
                                </tr>
                                <tr>
                                    <td class=label width=110><?=$lang->getLang("Creation Time")?><br /></td>
                                    <td class=content> <?= date("Y-m-d H:i", strtotime($row["jur_creation_date"])) ?></td>
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
                                 <?php if ($_SESSION['jurisdiction_id'] == 1) { ?>
                                 <tr>
                                    <td class="label" width="130"><?=$lang->getLang("Limit new str.")?></td>
                                    <td class="content"><input type="text" name="childs_limit" id="childs_limit" value="<?=$jurChildsLimit?>" size=20 /></td>
                                </tr>
                                <? } ?>
                                <tr>
                                    <td class=label><?=$lang->getLang("Timezone")?></td>
                                    <td>
                                        <?if($_SESSION['aty_code'] == "SUPERUSER") { ?>
                                            <select name="time">
                                                <?for($i=-12;$i<=12;$i++){
                                                    ?>
                                                    <option <?=$i==$time?"selected":""?>  value="<?=$i?>"><?=$i?>:00 UTC</option>
                                                    <?
                                                }
                                                ?>
                                            </select>
                                        <? } else { ?>
                                            <?=$time?>
                                        <? }?>
                                    </td>
                                </tr>

                                <? if (check_access("delete_jurisdictions", false)) :?>
                                    <? if($jur_status == 1) : ?>
                                        <tr valign="top">
                                            <td class="label" width="130"><?=$lang->getLang("Access")?></td>
                                            <td class="content"><button class="entitybtn " onclick="return deleteJurisdiction(<?=$node?>);"><?=$lang->getLang("Block %?",$nation_name)?></button></td>
                                        </tr>
                                    <?php endif; ?>
                                    <? if($jur_status == 0) : ?>
                                        <tr valign="top">
                                            <td class="label" width="130"><?=$lang->getLang("Access")?></td>
                                            <td class="content"><button class="entitybtn " onclick="return restoreJurisdiction(<?=$node?>);"><?=$lang->getLang("Unblock %?",$nation_name)?></button></td>
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
                                        <button id="update_processor" data-jur="<?=$node?>" data-name="<?=$nation_name?>" data-class="nation" class="btn btn-small"><?=$lang->getLang("Update sub")?></button>
                                    </td>
                                </tr>

                                <tr>
                                    <?php
                                    //addedd

                                    include_once "funclib/percentual.inc.php";
                                    showPercentage($_SESSION["jurisdiction_id"],$_SESSION["jurisdiction_class"],$node, "nation");
                                    //-- added
                                    ?>
                                </tr>
                                <table id="betting_perc" width="90%">
                                    <?php
                                    showBettingPercentage($_SESSION["jurisdiction_id"],$_SESSION["jurisdiction_class"], "nation",$node, check_access("betting_percentage_setting"));

                                    ?>
                                </table>
                                <table id="betting_live_perc" width="90%">
                                    <?php
                                    showLiveBettingPercentage($_SESSION["jurisdiction_id"],$_SESSION["jurisdiction_class"], "nation",$node, check_access("betting_percentage_setting"));
                                    ?>
                                </table>
                            </table>
                        </td>
                        <td width=80>
                            <? if($canModify) { ?>
                                <input type="submit" id="sub_update" class="entitybtn disabledBtn" style="width:60px;margin-bottom:2px;" value="<?=$lang->getLang("Update")?>"  />
                                <br />
                                <input type="button" id="btn_refresh" class="entitybtn disabledBtn"  style="width:60px;margin-bottom:2px"value="<?=$lang->getLang("Refresh")?>" onClick="doRefresh(this.form)"/>
                                <br />
                            <?  } ?>
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
            </td>
        </tr>
    </table>
    <?if($canModify == 1) { ?>
</form>
<? } ?>