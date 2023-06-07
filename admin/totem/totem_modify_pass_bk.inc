<?php
 $passcode_totem=check_access('ttm_totem_passcode',true);?>

<script type="text/javascript">
$(document).ready(function(){
	$("input#codepass").bind("keydown", function (e) {
		  return (e.keyCode > 48 && e.keyCode <= 54) || (e.keyCode > 96 && e.keyCode <= 102) || e.keyCode == 8 
	});
});
</script>
<?php
 include_once 'JurisdictionsList.class.inc';
 define("LABEL_GAME", 'gameid');
 define("LABEL_BILL" , 'bill');
 define("LABEL_WITHDRAWAL", 'withdrawal');
 define("LABEL_SKINS", "skins");
 define("LABEL_TPOINT", "tpoint");
 define("LABEL_TCODE", "tpcode");
 define("LABEL_TPCARD", "cardonly");
if(isset($_GET['ttcode'])){
 	
 	$ttmCode = $_GET['ttcode'];
}
 	if(isset($_POST['modify'])){      
 		$ttmCode= $_POST['ttmCode'];
 		$tpcode = $_POST['code_pass'];
 		$tpoint = $_POST['code_en'];
 		$tpcard = $_POST['card'];
 		$sql = "Select ttm_totem_md5 from totem where ttm_code = " . $dbh->quote($ttmCode);
 		$md5Key = $dbh->queryOne($sql);
 		$md5Key = md5($md5Key);
 		$buffer = $tpcode;
 		// get the amount of bytes to pad
 		// very simple ASCII key and IV
 		$key = substr($md5Key, 0, 24);
 		//$iv = mcrypt_create_iv (mcrypt_get_block_size (MCRYPT_TripleDES, MCRYPT_MODE_CBC), MCRYPT_DEV_RANDOM);
 		// hex encode the return value
 		$tpcode = (base64_encode(mcrypt_encrypt(MCRYPT_TripleDES, $key, $buffer, MCRYPT_MODE_CBC, "00000000")));

 		$sql = "Select ttm_conf_param from totem where ttm_code = " . $dbh->quote($ttmCode);
 		$confparam = $dbh->queryOne($sql);
 		if(substr($confparam,0,strpos($confparam,'tpoint'))!=''){ 		
 			$confparam=substr($confparam,0,strpos($confparam,'tpoint'));
 			$confparam=$confparam."tpoint:".$tpoint."~tpcode:".$tpcode."~cardonly:".$tpcard;
 		} 	
 		else {
 				$confparam=$confparam."~tpoint:".$tpoint."~tpcode:".$tpcode."~cardonly:".$tpcard;
 		}
 		$update = "UPDATE totem set ttm_conf_param = " .$dbh->quote($confparam) ." where ttm_code = " . $dbh->quote($ttmCode);
 		$res = $dbh->exec($update);
 		if($res === 1){
 			addResult('', $lang->getLang('Parameters successfully updated'));
 		}else{
 			addError('', $lang->getLang("Parameters weren't updated"));
 		}
 	}
 	showResult();
 	showErrors();
 	
 	$sql = "SELECT ttm_conf_param, ttm_totem_md5 from totem where ttm_code = "  . $dbh->quote($ttmCode);
 	$resQuery = $dbh->queryRow($sql);
 	$totemConfigure = $resQuery['ttm_conf_param'];
 	$totemListConf = split('~', $totemConfigure); 
 	$tpoint = "n";
 	$tpcode = "";
 	$tpcard="n";
 	foreach($totemListConf as $key => $value){
 		list($label, $content) =  split(":", $value); 	
 		if($label == LABEL_TPOINT){
 			$tpoint = $content;
 		}
 		if($label == LABEL_TCODE){
 			$tpcode = $content;
 		}
 		if($label == LABEL_TPCARD){
 			$tpcard=$content;
 		}
 	}
 	$md5Key = $resQuery['ttm_totem_md5'];
 	$md5Key = md5($md5Key);
 	$key = substr($md5Key, 0, 24);
 	//$iv = mcrypt_create_iv (mcrypt_get_block_size (MCRYPT_TripleDES, MCRYPT_MODE_CBC), MCRYPT_DEV_RANDOM);
 	$buffer = trim(base64_decode($tpcode));
 	$tpcode = trim(mcrypt_decrypt(MCRYPT_TripleDES, $key, $buffer, MCRYPT_MODE_CBC, "00000000")); 
 	$disable = "disabled";
 	if($_SESSION['aty_code'] == "SUPERUSER"){
 		$disable = "";
 	}
 	?> 	
 	  <center>
 	  <br/>
 	  <p class="bodyHD">
 	       <?=$lang->getLang("CHOOSE TOTEM PASS")?>
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
           <tr>
               <td class="label"><?=$lang->getLang("Proxy host")?>: </td>
               <td class="content">
               <input type="text">
               </td>
           </tr>
           <tr>
               <td class="label"><?=$lang->getLang("Proxy port")?>: </td>
               <td class="content">
               <input type="text">
               </td>
           </tr>
           <tr>
               <td class="label"><?=$lang->getLang("Proxy username")?>: </td>
               <td class="content">
               <input type="text">
               </td>
           </tr>
           <tr>
               <td class="label"><?=$lang->getLang("Proxy password")?>: </td>
               <td class="content">
               <input type="password">
               </td>
           </tr>
 	   </table>
 	  <button id="reset<?=$ttmCode?>"><?=$lang->getLang("Submit")?>
			<script>
						 $(function() {
						$( '#reset<?=$ttmCode?>')
					            .button()
					            .click(function() {
					            	$.post("totem/totem_modify_pass.inc",{
					            	    ttmCode:'<?=$ttmCode?>',
                                        modify:'1',
                                        code_pass:$('input:text[name=code_pass]').val(),
                                        code_en:$('select[name=code_en] option:selected').val(),
                                        card:$('select[name=card] option:selected').val()},
                                        function(data){
					            	        $('#reset<?=$ttmCode?>').parent().closest('div').empty().append( data );
						            	});
						            });
							return false;
							});
						</script>
			
			
			
			</button>
 	   </center>