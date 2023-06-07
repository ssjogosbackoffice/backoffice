<?php
$internal=check_access('manage_skins',$redirect=true);
include_once 'skins_functions.inc';
/**
 *
 * @author Marian
 */
if (isset ( $_POST ['add'] )) {
	$name = $_POST ['name'];
	if (isset ( $_POST ['webtype'] )) {
		$webtype = $_POST ['webtype'] . "://";
	}
	if (isset ( $_POST ['http'] )) {
		$http = $_POST ['http'] . "://";
	}
	$website = $webtype . $http . $_POST ['website'];
	$ips = $_POST ['ips'];
	$folder = $_POST ['folder'];
	$club = $_POST ['clubs'];
	$key = $_POST ['key'];
	$status = $_POST ['status'];
	$internetclub = $_POST ['internetclub'];
	$email = $_POST ['email'];
	$smtp = $_POST ['smtp'];
	addSkin ( $name, $website, $ips, $folder, $club, $key, $status, $internetclub, $email, $smtp );
} else {
	if ($_POST ['updel'] == '1') {
		for($i = 0; $i <= ($_POST ['nrips']); $i ++) {
			if (isset ( $ips )) {
				if ($_POST ["ips_$i"] != '') {
					$ips = $ips . "|" . $_POST ["ips_$i"];
				}
			} else {
				$ips = $_POST ["ips_$i"];
			}
		}
		$id = $_POST ['id'];
		$name = $_POST ['name'];
		$website = $_POST ['site'];
		$club = $_POST ['clubs'];
		$key = $_POST ['key'];
		$folder = $_POST ['folder'];
		$status = $_POST ['status'];
		$internetclub = $_POST ['iclub'];
		$email = $_POST ['email'];
		$smtp = $_POST ['smtp'];
		$update = updateSkin ( $id, $name, $website, $folder, $ips, $internetclub, $club, $email, $smtp, $key, $status );
		?>
<script>
	$(function() {
		showUpdate('<?=$id?>');
		<?php if($update=='1'){?>
		jAlert('<?=$lang->getLang("You have successfully updated the skin")?> <?=$name?>');
		<?php } elseif ($update=='0'){ ?>
		jAlert("<?=$lang->getLang("You haven't made modifies")?>)";
		<?php } else {?>
		jAlert('<?=$update?>');
		<?php }?>
	    });
	</script>
<?php
	}
	if ($_POST ['updel'] == '2') {
		$id = $_POST ['id'];
		delSkin ( $id );
		?>
<script>
	$(function() {
		jAlert('<?=$lang->getLang("You have successully deleted a skin")?>');
	    });
	</script>
<?php
	}
	?>
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap-responsive.css" title="core_style" />
    <script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
    <script src="/media/jquery.dataTables.js" type="text/javascript"></script>
<script type="text/javascript">
function disableHttps()
{
	document.getElementById("http").checked=true;
	document.getElementById("https").disabled=true;
}

function enableHttps()
{
	document.getElementById("https").disabled=false;
}

function showAddPart(){
	$("#add_part").show('slow');
	$("#part").show('slow');
}

function getAllGames(id){
	$('#Games_'+id+' input:checked').each(function() {
		if(typeof(games) != "undefined" && games !== null) {
			 games=games+','+$(this).attr('value');;
		}
		else{
	    games=$(this).attr('value');
		}
	});
	return games;
}

function addPart(){
	$('#allips input').each(function() {
		if(typeof(selected) != "undefined" && selected !== null) {
			 selected=selected+'|'+$(this).attr('value');;
		}
		else{
	    selected=$(this).attr('value');
		}
	});
	
			
	$.post("configuration/skins.inc",{add:'1',
		name:$('#name').val(),
		webtype:$('input:radio[name=webtype]:checked').val(),
		http:$('input:radio[name=http]:checked').val(),
		website:$('#website').val(),
		ips:selected,
		folder:$('#folder').val(),
		internetclub:$('#iclub').val(),
		clubs:$('#clubs').val(),
		smtp:$('#smtp').val(),
		email:$('#e').val(),
		key:$('#key').val(),
		status:$('#status').val() }, function(data){
	if(data=='2'){
			$('#addResult').removeClass().addClass("result");
			$('#addResult').empty().append( '<?=$lang->getLang("Successfully added skin!")?>');
	}
	else if(data=='1')
	{
			$('#addResult').removeClass().addClass("error");
			$('#addResult').empty().append( '<?=$lang->getLang("Skin already exists!")?>');
	}
	else 
		{
			$('#addResult').removeClass().addClass("error");
			$('#addResult').empty().append( '<?=$lang->getLang("An error has occurred.Please try again!")?>');
		}
		});
	}
	
$(function() {
	 $("form").submit( function(event, skip) {
		         if (skip) {
		             return true;
		         }
		         event.preventDefault();
		         var self = this;
		         jConfirm(
		             'Are you sure?',
		             'Please confirm',
		             function(r) {
		                 if (r) {
		                     $(self).trigger('submit', true);
		                 }
		             }
		         );
		     });
var fields=0;
$("#addIp").click(function(){
	fields=fields+1;
	$('#nrips').attr('val',fields);
    var strValidDOM = "<input type='text' name='ips"+fields+"' value='' >";
    $("#addIp").parent().append(strValidDOM);
    
});

$("#removeIp").click(function(){
	if(fields>0){
		fields=fields-1;
		$('#nrips').attr('val',fields);
    $("#removeIp").parent().find("input:last").remove(); //remove last
	}
});
});

function addip(id){
	var fieldsup=$('#nripsup'+id).val();
		fieldsup=parseInt(fieldsup)+1;
		$('#nripsup'+id).val(fieldsup);
	    var strValidDOM = "<input type='text' name='ips_"+fieldsup+"' value='' >";
	    $("#addIpup"+id).parent().append(strValidDOM);
	}
	
function removeip(id){
		var fieldsup=$('#nripsup'+id).val();
		if(fieldsup>1){
			fieldsup=fieldsup-1;
			$('#nripsup'+id).val(fieldsup);
	    $("#removeIpup"+id).parent().find("input:last").remove(); 
		}
	}

function hideAddPart()
{
	$("#part").hide('slow');
	$("#add_part").hide('slow');
}

function showModifyPart(){
	$("#add_part").hide("slow");
	toggle("modify_part");
}

function showUpdate(id){
	$(".update").hide('slow');
	show(id+'bg');
	show(id);
}
function hide(id){
	$("#"+id).hide('slow');
}

function show(id){
	$("#"+id).show('slow');
}
function toggle(id)
{	
    if ($('#'+id).is(":hidden")) {	
		$('#'+id).show('slow');
	} else {
		$('#'+id).hide('slow');
	}            
	return false;
}

function selectAllGames(id)
{
	$("#"+id).hide();
	    	$('#'+id).find(':checkbox').each(function() {
	            this.checked = true;                        
	      	        });
}
function deselectAllGames(id)
{
	    	$('#'+id).find(':checkbox').each(function() {
	            this.checked = false;                        
	      	        });
	    	$("#"+id).show('slow');
}
</script>
    <div class="container" style="position: absolute;left:0;right: 0;width: 100%" >
        <div class="container-fluid">
            <table class="table table-bordered">
                <tr><td class='navbar-inner'><h3 class="text-center text-error"><?=$lang->getLang("Manage Skins")?></h3></td></tr>
                <tr>
                    <td>
                        <div>
                            <div id="add_part"></div>
                            <div id='part'>
                                <a href="<?=refPage('configuration/skins','init')?>"><button type="button"
                                                                                             class="close" style="color: darkblue">X</button></a>
                                <table class="table display">
                                    <input type="hidden" name="nrips" id="nrips" value="">
                                    <tr>
                                        <td class="label"><?=$lang->getLang("Skin")?></td>
                                        <td><input type="text" id="name" value=''></td>
                                    </tr>
                                    <tr>
                                        <td class="label"><?=$lang->getLang("Type")?></td>
                                        <td><input type="radio" id="webtype" name='webtype' value='web'
                                                   onclick="enableHttps();">Web <br> <input type="radio" id="webtype"
                                                                                            name='webtype' value='socket' onclick="disableHttps();">Socket<br>
                                            <input type="radio" id="webtype" name='webtype' value='webservice'
                                                   onclick="enableHttps();"><?=$lang->getLang("Websevice")?></td>
                                    </tr>
                                    <tr>
                                        <td class="label">Http/Https</td>
                                        <td><input type="radio" id="http" name='http' value='http'>Http <br>
                                            <input type="radio" id="https" name='http' value='https'>Https</td>
                                    </tr>
                                    <tr>
                                        <td class="label"><?=$lang->getLang("Website")?></td>
                                        <td><input type="text" id='website' style='width: 100%'></td>
                                    </tr>
                                    <tr>
                                        <td class="label"><?=$lang->getLang("Folder")?></td>
                                        <td><input type="text" id="folder" name='folder' value=''></td>
                                    </tr>
                                    <tr>
                                        <td class="label"><?=$lang->getLang("Allowed ips")?></td>
                                        <td style="width: 240px;"><div id="allips">
                                                <input type="text" id='ips' name='ips'>
                                                <button id="addIp">+</button>
                                                <button id="removeIp">-</button>
                                            </div></td>
                                    </tr>
                                    <tr>
                                        <td class="label"><?=$lang->getLang("Internet Club")?></td>
                                        <td><input type="text" id="iclub" value=''></td>
                                    </tr>
                                    <tr>
                                        <td class="label"><?=$lang->getLang("Club")?></td>
                                        <td><select id='clubs'><?
                                                $clubs = getAllClubs ();
                                                while ( $clubs->hasNext () ) {
                                                    $club = $clubs->next ();
                                                    ?><option value='<?=$club['jur_id']?>'><?=$club['jur_name']?></option>
                                                <?php }?>
                                            </select></td>
                                    </tr>
                                    <tr>
                                        <td class="label"><?=$lang->getLang("Email")?></td>
                                        <td><input type="text" id="e" name='e' value=''></td>
                                    </tr>
                                    <tr>
                                        <td class="label">SMTP</td>
                                        <td><input type="text" id="smtp" name='smtp' value=''></td>
                                    </tr>
                                    <tr>
                                        <td class="label"><?=$lang->getLang("Key")?></td>
                                        <td><input type="text" id="key" value=''></td>
                                    </tr>
                                    <tr>
                                        <td class="label"><?=$lang->getLang("Status")?></td>
                                        <td><input type="text" id="status" value=''></td>
                                    </tr>
                                    <tr>
                                        <td colspan="2"><button id='add' class="button blue"
                                                                onclick='addPart()'><?=$lang->getLang("Add")?></button></td>
                                    </tr>
                                    <tr>
                                        <td colspan="2"><div id="addResult"></div></td>
                                    </tr>
                                </table>
                            </div>
                        </div>
                        <div id="partners">
                            <div><a href="#myModal" class="fright btn btn-info btn-small showAddModal" style="color:white" data-toggle="modal" ><?=$lang->getLang("Add a new skin")?></a></div><br><br><br>

                            <table class="table display">
                                <thead>
                                <tr>
                                    <td colspan='8'><button class='button blue' style='float: left'
                                                            onclick='showAddPart()'><?=$lang->getLang("Add a new skin")?></button></td>
                                </tr>
                                <tr class="formheading">
                                    <th class="label"><?=$lang->getLang("Id")?></th>
                                    <th class="label"><?=$lang->getLang("Skin Name")?></th>
                                    <th class="label"><?=$lang->getLang("Website")?></th>
                                    <th class="label"><?=$lang->getLang("Folder")?></th>
                                    <th class="label"><?=$lang->getLang("Allowed ips")?></th>
                                    <th class="label"><?=$lang->getLang("Internet Club")?></th>
                                    <th class="label"><?=$lang->getLang("Club")?></th>
                                    <th class="label"><?=$lang->getLang("Email")?></th>
                                    <th class="label">SMTP</th>
                                    <th class="label"><?=$lang->getLang("Key")?></th>
                                    <th class="label"><?=$lang->getLang("Status")?></th>
                                    <th class="content"></th>
                                </tr>
                                </thead>
                                <tbody>
                                <?php
                                $rs = getSkins ();
                                while ( $rs->hasNext () ) {
                                    $row = $rs->next ();
                                    ?>
                                    <tr>
                                        <td class="content"><?=$row['skn_id'];?></td>
                                        <td class="content"><strong><?=$row['skn_name']?></strong></td>
                                        <td class="content"><?=$row['skn_url'];?></td>
                                        <td class="content"><?=$row['skn_foldername']?></td>
                                        <td class="content"><?=$row['skn_allowip']?></td>
                                        <td class="content"><?=$row['skn_internet_club_id']?></td>
                                        <td class="content"><?=getClubName($row['skn_jur_id'])?></td>
                                        <td class="content"><?=$row['skn_email']?></td>
                                        <td class="content"><?=$row['skn_smtp']?></td>
                                        <td class="content"><?=$row['skn_key']?></td>
                                        <td class="content"><?=$row['skn_status']?></td>
                                        <td class="content" style="min-width: 60px;"><img
                                                    style="width: 25px; height: 25px; vertical-align: bottom; cursor: pointer; float: left;"
                                                    src="live/liveimage/edit.png"
                                                    onclick="showUpdate('<?=$row['skn_id']?>')"><?php form_head()?>
                                            <input type="hidden" name='id' value='<?=$row['skn_id']?>'> <input
                                                    type="hidden" name='updel' value='2'> <input type="image"
                                                                                                 src="media/images/delete.jpg" alt="Delete this skin"
                                                                                                 style="width: 35px; height: 25px;" />
                                            </form></td>
                                    </tr>
                                    <tr>
                                        <td colspan='8'>
                                            <div class="partnersbg" id="<?=$row['skn_id']?>bg"></div>
                                            <div class='update' id="<?=$row['skn_id']?>">
                                                <a href="<?=refPage('configuration/skins','init')?>"><button
                                                            type="button" class="close" style="color: darkblue">X</button></a>
                                                <?php form_head()?>
                                                <input type="hidden" name='id' value='<?=$row['skn_id']?>'> <input
                                                        type="hidden" name='updel' value='1'>
                                                <table style='margin: auto; float: left; text-align: left'>
                                                    <tr>
                                                        <td class="bodyHD" colspan="2"><?=$lang->getLang("Update skin")?> <?=$row['skn_name']?> </td>
                                                    </tr>
                                                    <tr>
                                                        <td class="label"><?=$lang->getLang("Id")?></td>
                                                        <td><?=$row['skn_id']?></td>
                                                    </tr>
                                                    <tr>
                                                        <td class="label"><?=$lang->getLang("Skin")?></td>
                                                        <td><input type="text" name='name' value='<?=$row['skn_name']?>'></td>
                                                    </tr>
                                                    <tr>
                                                        <td class="label"><?=$lang->getLang("Website")?></td>
                                                        <td><input type="text" name='site' value='<?=$row['skn_url'];?>'
                                                                   style='width: 100%'></td>
                                                    </tr>
                                                    <tr>
                                                        <td class="label">Folder</td>
                                                        <td><input type="text" name='folder'
                                                                   value='<?=$row['skn_foldername']?>'></td>
                                                    </tr>
                                                    <tr>
                                                        <td class="label"><?=$lang->getLang("Allowed ips")?></td>
                                                        <td style="width: 240px;">
                                                            <div id="allipsup"><?php
                                                                $iplist = explode ( '|', $row ['skn_allowip'] );
                                                                foreach ( $iplist as $k => $v ) {
                                                                    ?><input type="text" name='ips_<?=$k?>' value='<?=$v?>'>
                                                                    <?php if ($k < 1) {?>
                                                                        <button id="addIpup<?=$row['skn_id']?>"
                                                                                onclick="addip(<?=$row['skn_id']?>)" type="button">+</button>
                                                                        <button type="button" onclick="removeip(<?=$row['skn_id']?>)"
                                                                                id="removeIpup<?=$row['skn_id']?>">-</button>
                                                                        <?php
                                                                    }
                                                                }
                                                                ?>
                                                            </div><input type="hidden" id="nripsup<?=$row['skn_id']?>"
                                                                         name='nripsup<?=$row['skn_id']?>' value='<?=count($iplist)?>'>
                                                            <input type="hidden" name='nrips' id='nrips' value=''>
                                                        </td>
                                                    </tr>
                                                    <tr>
                                                        <td class="label"><?=$lang->getLang("Internet Club")?></td>
                                                        <td><input type="text" name='iclub'
                                                                   value='<?=$row['skn_internet_club_id']?>'></td>
                                                    </tr>
                                                    <tr>
                                                        <td class="label"><?=$lang->getLang("Club")?></td>
                                                        <td><select name='clubs'><?
                                                                $clubs = getAllClubs ();
                                                                while ( $clubs->hasNext () ) {
                                                                    $club = $clubs->next ();
                                                                    ?><option value='<?=$club['jur_id']?>'
                                                                    <? if($club['jur_id']==$row['skn_jur_id']){?>
                                                                        selected=selected <?}?>><?=$club['jur_name']?></option>
                                                                <?php }?>
                                                            </select></td>
                                                    </tr>
                                                    <tr>
                                                        <td class="label"><?=$lang->getLang("Email")?></td>
                                                        <td><input type="text" name="email"
                                                                   value='<?=$row['skn_email']?>'></td>
                                                    </tr>
                                                    <tr>
                                                        <td class="label">SMTP</td>
                                                        <td><input type="text" name="smtp" value='<?=$row['skn_smtp']?>'></td>
                                                    </tr>
                                                    <tr>
                                                        <td class="label"><?=$lang->getLang("Key")?></td>
                                                        <td><input type="text" name="key" value='<?=$row['skn_key']?>'></td>
                                                    </tr>
                                                    <tr>
                                                        <td class="label"><?=$lang->getLang("Status")?></td>
                                                        <td><input type="text" name="status"
                                                                   value='<?=$row['skn_status']?>'></td>
                                                    </tr>
                                                    <tr>
                                                        <td><input type="submit" class="button blue" value='<?=$lang->getLang("Update")?>'
                                                                   onclick='nrips.value=nripsup<?=$row['skn_id']?>.value;'></td>
                                                    </tr>
                                                </table>
                                                </form>
                                            </div>
                                        </td>
                                    </tr>
                                <?php }?>
                                </tbody>
                            </table>
                        </div>
                    </td>
                </tr>
            </table>
        </div>
    </div>



<?php } ?>
