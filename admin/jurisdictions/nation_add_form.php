<?php if ( areErrors() ) { showErrors(); echo "<br />";}
function formatOffset($offset) {
    $hours = $offset / 3600;
    $remainder = $offset % 3600;
    $sign = $hours > 0 ? '+' : '-';
    $hour = (int) abs($hours);
    $minutes = (int) abs($remainder / 60);

    if ($hour == 0 AND $minutes == 0) {
        $sign = ' ';
    }
    return $sign . str_pad($hour, 2, '0', STR_PAD_LEFT) .':'. str_pad($minutes,2, '0');

}

?>
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
                });
            });
        $('#skin, #country').select2({dropdownAutoWidth : true});
        });
    </script>
    <form action="<?php echo $_SERVER['PHP_SELF']?>" method="POST">
    <input type="hidden" name="class" value="nation">
    <input type="hidden" name="op" value="Save">
    <table cellpadding=4 cellspacing=1 border="0">
        <tr valign="top">
            <td class="formheading" colspan="2"><?=$lang->getLang("Add a Nation")?></td>
            <td rowspan="3" width="80">
                <input type="submit" id="sub_save" style="width:60px;margin-bottom:2px" value="<?=$lang->getLang("Save")?>" />
    	        <br />   
    	        <input type="submit" id="sub_cancel" style="width:60px" value="<?=$lang->getLang("cancel")?>" />
    	    </td>
        </tr>
        <tr valign="top">
            <td class="label"><?=$lang->getLang("Nation name")?></td>
            <td class="content"><input type=text id="nation_name" class="lettersAndNumbers" name="nation_name" size="26" value="<?=replace_quotes($_POST['nation_name']);?>"></td>
		</tr>
            <?php
            if(empty($_POST['account_code'])){
              $_POST['account_code'] = get_free_jurcode();
            }
            ?>
		<tr>
            <td class=label width=110><?=$lang->getLang("Account Code")?> <br /><div class="labelnote">(<?=$lang->getLang("6 Characters")?>)</div></td>
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
        <tr>
            <td class=label width=110><?=$lang->getLang("Currency Type")?><div id="cur"></div><br /></td>
            <td class=content><?=getAllCurrency($currency);?></td>
        </tr>
        <tr>
            <td class=label width=110><?=$lang->getLang("Skin")?><div id="skn"></div><br /></td>
            <?if($_SESSION['jurisdiction_class']=='casino'){?>
            <td class='content'>
                <select name="skin" id="skin">
                <?$allSkins=$dbh->exec("Select * from skins");

                while ($allSkins->hasNext()) {
                    $row = $allSkins->next();
                    ?>
                    <option value="<?=$row['skn_id']?>"><?=$row['skn_name']?></option>
                <?
                    }
                ?>

                </select>
            </td>
            <? }else{ ?>

            <td class=content><?=$lang->getLang("No skin available")?></td>
            <? }?>
        </tr>
        <tr>
            <td class=label><?=$lang->getLang("Country")?></td>
            <td <? form_td_style('country');
            ?>>
                <?countrySelect('country', "", $_POST['country'],false,'country'); ?>
                <?php echo err_field('user_country'); ?>
            </td>
        </tr>
        <tr>
            <td class="label" width="130"><?=$lang->getLang("City")?></td>
            <td class="content"><input type="text" name="city" id="city" value="<?=$city?>" size=30 /></td>
        </tr>
        <tr>
            <td class=label><?=$lang->getLang("Timezone")?></td>
            <td><select name="time">
                    <?for($i=-12;$i<=12;$i++){
                        ?>
                        <option value="<?=$i?>"><?=$i?>:00 UTC</option>
                    <?
                    }
                    ?>
                    <!--   --><?/*
                $utc = new DateTimeZone('UTC');
                $dt = new DateTime('now', $utc);
                $current_datezone=date_default_timezone_get();

                foreach(DateTimeZone::listIdentifiers() as $tz) {
                    $current_tz = new DateTimeZone($tz);
                    $offset =  $current_tz->getOffset($dt);
                    $transition =  $current_tz->getTransitions($dt->getTimestamp(), $dt->getTimestamp());
                    $abbr = $transition[0]['abbr'];
                    echo '<option value="' .$tz. '" '.($tz==$current_datezone ? "selected": '').'>' .$tz. ' [' .$abbr. ' '. formatOffset($offset). ']</option>';

                }*/?>
                </select>
            </td>
        </tr>
        <?php 
        if ($_SESSION['jurisdiction_id'] == 1) { ?>
         <tr>
            <td class="label" width="130"><?=$lang->getLang("Limit new str.")?></td>
            <td class="content"><input type="text" name="childs_limit" id="childs_limit" value="20" size=20 /></td>
        </tr>
        <? } ?>
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
        <?php
        //addedd
            include_once "funclib/percentual.inc.php";
            showPercentage($_SESSION["jurisdiction_id"],$_SESSION["jurisdiction_class"],-1,"nation");
        //-- added
        ?>
        </tr>
        <table id="betting_perc" width="90%">
            <?php
              showBettingPercentage($_SESSION["jurisdiction_id"],$_SESSION["jurisdiction_class"], "nation",-1,-1);
            ?>
        </table>
        <table id="betting_live_perc" width="90%">
            <?php
            showLiveBettingPercentage($_SESSION["jurisdiction_id"],$_SESSION["jurisdiction_class"], "nation",-1, -1);
            ?>
        </table>
        <table cellpadding=0 cellspacing="1" border=0 >
            <tr>
                <td colspan='2'>
                    <a href="javascript:void(0);" onclick="showAdminAdd();" id="adminAddShow"><?=$lang->getLang("Add an administrator for this %", "nation")?> &#9660;</a>
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
                                    <option value="3"><?=$lang->getLang("National Manager")?></option>
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
                $("#adminAddShow").html("<?=$lang->getLang("Add an administrator for this %" , "nation")?> &#9660;");
            }
            return false;
        }
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
        $(document).ready(function () {
            $.each($('select[name*="JPS_PERC"]'),function () {
                $(this).val('65');
            });
        });
     </script>
