<?php
 $passcode_totem=check_access('ttm_totem_passcode',true);?>


<?php
 include_once 'JurisdictionsList.class.inc';
  require_once("Totem.class.inc");
 define("LABEL_GAME", 'gameid');
 define("LABEL_BILL" , 'bill');
 define("LABEL_WITHDRAWAL", 'withdrawal');
 define("LABEL_SKINS", "skins");
 define("LABEL_TPOINT", "tpoint");
 define("LABEL_TCODE", "tpcode");
 define("LABEL_TPCARD", "cardonly");
 define("LABEL_PROXY", "proxy");
 define("LABEL_TAX", "tax");
$ttmCode = (isset($_POST['ttcode'])?$_POST['ttcode']:$_GET ['ttcode']);
$totem=new Totem();
$totem->setCode($ttmCode);
$totem->setConfigParams($dbh->queryOne ( "SELECT ttm_conf_param FROM `totem` WHERE ttm_code = " . $dbh->quote ( $ttmCode ) ));
//gameid:Playtech,218-ano-C022;218-arc-C022;218-art-C022;218-ashace-C022;218-ashadv-C022;218-ashamw-C022;218-ashbob-C022;218-ashcpl-C022;218-ashctw-C022;218-ashfmf-C022;218-ashfmn-C022;218-ashfta-C022;218-ashglss-C022;218-ashhotj-C022;218-ashhwt-C022;218-ashjah-C022;218-ashjcb-C022;218-ashjut-C022;218-ashlcl-C022;218-ashlob-C022;218-ashndm-C022;218-ashsbd-C022;218-ashwgaa-C022;218-ashwnoz-C022;218-ashwwm-C022;218-avng-C022;218-bib-C022;218-bl-C022;218-bob-C022;218-bs-C022;218-bt-C022;218-c7-C022;218-cam-C022;218-catqc-C022;218-chl-C022;218-cm-C022;218-cnpr-C022;218-ct-C022;218-ctiv-C022;218-dlm-C022;218-dnr-C022;218-donq-C022;218-eas-C022;218-esm-C022;218-evj-C022;218-fbr-C022;218-fcgz-C022;218-fdt-C022;218-fdtjg-C022;218-fff-C022;218-fm-C022;218-fnfr-C022;218-fnfrj-C022;218-fow-C022;218-foy-C022;218-frtf-C022;218-fsf-C022;218-fxf-C022;218-ges-C022;218-glg-C022;218-glrj-C022;218-gos-C022;218-gro-C022;218-gts52-C022;218-gtsaod-C022;218-gtsatq-C022;218-gtsbtg-C022;218-gtscbl-C022;218-gtscnb-C022;218-gtsdgk-C022;218-gtsdrdv-C022;218-gtsfc-C022;218-gtsmrln-C022;218-gtspor-C022;218-gtssmbr-C022;218-gtswg-C022;218-gtswng-C022;218-hf-C022;218-hh-C022;218-hk-C022;218-hlk2-C022;218-ir-C022;218-ir2-C022;218-irl-C022;218-irm3-C022;218-irmn3-C022;218-jpgt-C022;218-kkg-C022;218-lm-C022;218-lndg-C022;218-lvb-C022;218-mgstk-C022;218-mobdt-C022;218-mrcb-C022;218-multm-C022;218-nian-C022;218-nk-C022;218-op-C022;218-paw-C022;218-pgv-C022;218-phot-C022;218-pmn-C022;218-pnp-C022;218-pst-C022;218-rky-C022;218-rng2-C022;218-samz-C022;218-shmst-C022;218-sib-C022;218-sis-C022;218-sol-C022;218-sp-C022;218-spidc-C022;218-spm-C022;218-srcg-C022;218-ssp-C022;218-sx-C022;218-tht-C022;218-tism-C022;218-tmqd-C022;218-topg-C022;218-tpd2-C022;218-trm-C022;218-ttc-C022;218-vcstd-C022;218-whk-C022;218-wis-C022;218-wkw-C022;218-wlg-C022;218-xmn50-C022;218-zcjb-C022;218-zctz-C022;218-zeus-C022;218-ash3brg-C022;218-jb_mh-C022;218-jb_mh50-C022;218-videopoker-C022;218-frr-C022;218-pbro-C022;218-mobro-C022;218-ro_g-C022;218-bj_mh5-C022;218-bjs-C022;218-mobbj-C022;218-babaws-C022;218-baws-C022;218-cheaa-C022;218-bjp-C022;218-bal-C022;218-bjl-C022;218-ubjl-C022;218-rol-C022;218-7bal-C022|Artbet,210-2056_Jumpin_Pot-C010;210-2057_Kitty_Twins-C010;210-2058_African_Sunsets-C010;210-2070_Tesla-C010;210-2071_DaVinci_CodeX-C010;210-6441_Ramses_Treasure-C010;210-3631_Dragon_Lady-C010;210-6453_King_Of_Wealth-C010;210-3639_Burning_Flame-C010;210-6465_Fortune_Lions-C010;210-3651_Royal_Gems-C010;210-6469_Caligula-C010;210-3146_Cleopatra_Jewels-C010;210-3147_Atlantis_World-C010;210-3417_Money_Farm-C010;210-6505_Three_Kings-C010;210-6509_Golden_Dragon-C010;210-6511_Lucky_Babies-C010;210-6517_Thunder_Bird-C010;210-3247_Crystal_Mystery-C010;210-3255_Five_Star_Luxury-C010;210-4281_Storming_Flame-C010;210-3273_More_Cash-C010;210-6097_Star_Cash-C010;210-6375_88_Riches-C010;210-6379_Jade_Treasure-C010;210-2028_Dragon_King-C010;210-2029_Lady_Luck-C010;210-2030_Explosive_Reels-C010;210-2031_Wolf_Quest-C010;210-2033_Magic_Unicorn-C010;210-2034_Fortune_Panda-C010;210-2035_Wild_Dolphin-C010;210-2036_Dancing_Lions-C010;210-2037_Venetia-C010;210-2038_Phoenix_Princess-C010;210-2039_Ancient_Gong-C010;210-2040_Power_Dragon-C010;210-3461_ES_Livecasino-C010;210-6579_ES_Livecasino_testonly-C010;210-3517_ES_Livecasino_Holdem-C010;210-3541_ES_Livecasino_Blackjack_A-C010;210-6091_AG_Live_casino-C010|Slot Machines,1046;1034;1044;1043;1042;1041;1001;1039;1038;1037;1036;1035;1040;1047;1048;1049;1050;1051;1052;1055;1057;1058;1059;1060;1061;1062;1010;1002;1003;1004;1005;1006;1007;1008;1009;1033;1011;1012;1013;1014;1015;1016;1018;1032;1031;1030;1029;1028;1027;1026;1024;1017;1019;1020;1021;1022;1023;1025|Table Games,4002;4003;5001;4001;9001;9005|Common Draw,9021;9019;9013;9009;9002;5002;9007|Videopoker,2006;2005;2004;2003;2001~bill:5;10;20;50;0;0;0;0;0;0;0;0;0;0;0;0~withdrawal:3~skins:virtualsys~tpoint:n~tpcode:Null~cardonly:n~custcat:Playtech-Playtech;Artbet-Artbet;Slot Machines-Slot Machines;Table Games-Table Games;Common draw-Common Draw;Videopoker-Videopoker
$totemListConf = explode('~',  $totem->getConfigParams());
$tpoint = "n";
$tpcode = "";
$tpcard="n";
$proxyDetails=[];
foreach($totemListConf as $key => $value){
    list($label, $content) =  split(":", $value);
    if($label == LABEL_TPOINT){
        $tpoint = $content;
        $tpointKey = $key;
    }
    if($label == LABEL_TCODE){
        $tpcode = $content;
        $tpcodeKey=$key;
    }
    if($label == LABEL_TPCARD){
        $tpcard=$content;
        $tpcardKey=$key;
    }
    if($label == LABEL_GAME){
        $gameContent = $content;
    }
    if($label == LABEL_CUSTOM_CAT){
        $customCat = explode(";",$content);
    }
    if($label == LABEL_PROXY){
        $proxyDetails = explode(";",$content);
         $proxyKey=$key;
    }
    if($label == LABEL_TAX){
        $taxDetails = explode(";",$content);
         $taxKey=$key;
    }
}
$totem->setMd5(md5($dbh->queryOne("Select ttm_totem_md5 from totem where ttm_code = " . $dbh->quote($ttmCode))));
$totem->setConfigKey(substr($totem->getMd5(), 0, 24));

if(isset($_POST['modify'])){
    if(isset($_POST['code_en'])&& $_POST['code_en']!=''){
        $tpoint = $_POST['code_en'];
        if($tpointKey!=''){
            $totemListConf[$tpointKey]=LABEL_TPOINT.":".$tpoint;
        }
        else{
            array_push($totemListConf,LABEL_TPOINT.":".$tpoint);
        }
    }

    if(isset($_POST['code_pass'])&& $_POST['code_pass']!=''){
        $tpcode = $_POST['code_pass'];
        $buffer = $tpcode;
        // get the amount of bytes to pad
        // very simple ASCII key and IV

        //$iv = mcrypt_create_iv (mcrypt_get_block_size (MCRYPT_TripleDES, MCRYPT_MODE_CBC), MCRYPT_DEV_RANDOM);
        // hex encode the return value
        $tpcode = (base64_encode(mcrypt_encrypt(MCRYPT_TripleDES, $totem->getConfigKey(), $buffer, MCRYPT_MODE_CBC, "00000000")));

        if($tpcodeKey!=''){
            $totemListConf[$tpcodeKey]=LABEL_TCODE.":".$tpcode;
        }
        else{
            array_push($totemListConf,LABEL_TCODE.":".$tpcode);
        }

    }

    if(isset($_POST['card'])&& $_POST['card']!=''){
        $tpcard = $_POST['card'];
        if($tpcardKey!=''){
            $totemListConf[$tpcardKey]=LABEL_TPCARD.":".$tpcard;
        }
        else{
            array_push($totemListConf,LABEL_TPCARD.":".$tpcard);
        }
    }

    if(isset($_POST['proxyHost'])){
        $proxyDetails[0]=$_POST['proxyHost'];
    }
    if(isset($_POST['proxyPort'])){
        $proxyDetails[1]=$_POST['proxyPort'];
    }
    if(isset($_POST['proxyUsername'])){
        $proxyDetails[2]=$_POST['proxyUsername'];
    }
    if(isset($_POST['proxyPassword'])){
        $proxyDetails[3]=$_POST['proxyPassword'];
    }
    if($proxyKey!=''){
        $totemListConf[$proxyKey]=LABEL_PROXY.":".implode(';',$proxyDetails);
    }
    else{
        array_push($totemListConf,LABEL_PROXY.":".implode(';',$proxyDetails));
    }

    $taxValue='';
    if(isset($_POST['tax'])&& $_POST['tax']!=''){
        $taxValue=LABEL_TAX.":";
        foreach($_POST['tax'] as $k=>$v ){
            $taxValue.=$v['name']."=".$v['value'].";";
        }
        $taxValue=substr($taxValue, 0, -1);
    }
    if($taxKey!=''){
        if($taxValue!='') {
            $totemListConf[$taxKey] = $taxValue;
        }
        else{
            unset($totemListConf[$taxKey]);
        }
    }
    else{
        if($taxValue!='') {
            array_push($totemListConf, $taxValue);
        }
    }


    $totem->setConfigParams(implode('~', $totemListConf));
 		if($totem->saveConfigParams()=== 1){
 		    $taxDetails='';
            foreach($totemListConf as $key => $value){
                list($label, $content) =  split(":", $value);
                if($label == LABEL_TPOINT){
                    $tpoint = $content;
                    $tpointKey = $key;
                }
                if($label == LABEL_TCODE){
                    $tpcode = $content;
                    $tpcodeKey=$key;
                }
                if($label == LABEL_TPCARD){
                    $tpcard=$content;
                    $tpcardKey=$key;
                }
                if($label == LABEL_GAME){
                    $gameContent = $content;
                }
                if($label == LABEL_CUSTOM_CAT){
                    $customCat = explode(";",$content);
                }
                if($label == LABEL_PROXY){
                    $proxyDetails = explode(";",$content);
                    $proxyKey=$key;
                }
                if($label == LABEL_TAX){
                    $taxDetails = explode(";",$content);
                    $taxKey=$key;
                }
            }
 			addResult('', $lang->getLang('Parameters successfully updated'));
 		}else{
 			addError('', $lang->getLang("Parameters weren't updated"));
 		}
    showResult();
    showErrors();
}


 	$buffer = trim(base64_decode($tpcode));
 	$tpcode = trim(mcrypt_decrypt(MCRYPT_TripleDES, $totem->getConfigKey(), $buffer, MCRYPT_MODE_CBC, "00000000"));
 	$disable = "disabled";
 	if($_SESSION['aty_code'] == "SUPERUSER"){
 		$disable = "";
 	}
 	?>
<script type="text/javascript">
    $(document).ready(function(){
        $("input#codepass").bind("keydown", function (e) {
            return (e.keyCode > 48 && e.keyCode <= 54) || (e.keyCode > 96 && e.keyCode <= 102) || e.keyCode == 8
        });
    });
</script>
 	  <center>
 	  <br/>
 	  <p class="bodyHD">
 	       <?=$lang->getLang("Change totem settings")?>
 	   </p>
 	   <table>
 	   	<tr>
 	   	<td class="label"><?=$lang->getLang("Code Enabled")?>:</td>
 	   	<td class="content">
 	   	<select name="code_en" id="codeenable" >
 	   	<option value="y" <?php if($tpoint=='y') { ?> selected <?php } ?> >y</option>
 	   	<option value="n" <?php if($tpoint=='n') { ?> selected <?php } ?> >n </option>
 	   	
 	   	</select>
 	   	</td>
 	   	</tr>
 	   	<tr>
 	   	<td class="label"><?=$lang->getLang("Code password")?>:</td>
 	   	<td class="content">
 	   	 <input type="text" value="<?=$tpcode?>" name="code_pass" id="codepass" size="8" maxlength="6" />
 	   	</td>
 	   	</tr>
 	   	<tr>
 	   	<td class="label"><?=$lang->getLang("Access only with the card")?>:</td>
 	   	<td class="content">
 	   	<select name="card" id="card" >
 	   	<? if($tpoint!='n'){?>
 	   	<option value="y" <?php if($tpcard=='y') { ?> selected <?php } ?> >y</option>
 	   	<?}?>
 	   	<option value="n" <?php if($tpcard=='n') { ?> selected <?php } ?> >n </option> 	   	
 	   	</select>
 	   	</td>
 	   	</tr>
           <?if(check_access('totem_configure_proxy')) { ?>
           <tr>
               <td class="label"><?=$lang->getLang("Proxy host")?>: </td>
               <td class="content">
               <input type="text" name="proxyHost" class="proxyComponents" value="<?=$proxyDetails[0]?>">
               </td>
           </tr>
           <tr>
               <td class="label"><?=$lang->getLang("Proxy port")?>: </td>
               <td class="content">
               <input type="text"  name="proxyPort" class="proxyComponents" value="<?=$proxyDetails[1]?>">
               </td>
           </tr>
           <tr>
               <td class="label"><?=$lang->getLang("Proxy username")?>: </td>
               <td class="content">
               <input type="text"  name="proxyUsername" class="proxyComponents" value="<?=$proxyDetails[2]?>">
               </td>
           </tr>
           <tr>
               <td class="label"><?=$lang->getLang("Proxy password")?>: </td>
               <td class="content">
               <input type="password"  name="proxyPassword" class="proxyComponents" value="<?=$proxyDetails[3]?>">
               </td>
           </tr>
           <!--<tr>
               <td class="label"><?/*=$lang->getLang("Cashout Tax")*/?>: </td>
               <td class="content">
               <table>
                   <thead>
                   <tr><th>Type</th><th>Value</th></tr>
                   </thead>
                   <tr>
                       <?/*
                       $disabled=true;
                       $taxVal='';
                       foreach($taxDetails as $k=>$v){
                           $v=explode('=',$v);
                           if($v[0]=='virtual_cash_out'){
                               $disabled=false;
                               $taxVal=$v[1];
                           }
                       }
                       */?>
                       <td><label><input type="checkbox" <?/*=!$disabled?'checked':''*/?>   class="taxCheckbox" value="virtual_cash_out">Virtual cashout tax</label></td>
                       <td><input type="text" <?/*=!$disabled?'':'disabled'*/?> id="virtual_cash_out" class="tax"   name="virtual_cash_out" style="width: 50px"  value="<?/*=!$disabled?$taxVal:''*/?>"></td>
                   </tr>
               </table>
               </td>
           </tr>-->
           <? } ?>
 	   </table><br />
 	  <button class="btn btn-primary" id="reset<?=$ttmCode?>"><?=$lang->getLang("Update settings")?>
			<script>

						 $(function() {
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
                             $( '#reset<?=$ttmCode?>').off('click');
						$( '#reset<?=$ttmCode?>')
					            .on('click',function() {
                            oneNotEmpty=false;
                            oneEmpty=false;
					                $('.proxyComponents').each(function(){
					                   if($(this).val()!=''){
					                       oneNotEmpty=true;
                                       }
                                       else{
                                           oneEmpty=true;
                                       }
                                    });
					                if(oneNotEmpty!=false && oneEmpty!=false){
					                    jAlert('Please complete all proxy fields');
                                    }
                                    else {
                                        $.post("totem/totem_modify_pass.inc", {
                                                ttcode: '<?=$ttmCode?>',
                                                modify: '1',
                                                code_pass: $('input:text[name=code_pass]').val(),
                                                code_en: $('select[name=code_en] option:selected').val(),
                                                card: $('select[name=card] option:selected').val(),
                                                proxyHost: $('input:text[name=proxyHost]').val(),
                                                proxyPort: $('input:text[name=proxyPort]').val(),
                                                proxyUsername: $('input:text[name=proxyUsername]').val(),
                                                proxyPassword: $('input:password[name=proxyPassword]').val(),
                                                tax:$('.tax').serializeArray()
                                            },
                                            function (data) {
                                                $('#reset<?=$ttmCode?>').parent().closest('div').empty().append(data);
                                            });
                                    }
						            });
							return false;
							});
						</script>
	        </button>
          <?if(check_access('totem_configure_games')) { ?>
          <br /><br />
          - Or -  <br /><br />
          <button class="btn btn-info" type="button"  onclick="openGamesTotem('<?=$ttmCode?>');">Manage games</button>
          <? }?>
 	   </center>