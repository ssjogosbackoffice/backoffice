<?php
$internal=check_access('manage_skins',$redirect=true);
include_once 'skins_functions.inc';
/**
 *
 * @author Marian
 */
?>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap-responsive.css" title="core_style" />
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<script src="/media/jquery.dataTables.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media//select2/select2.css" >
<script src="<?= secure_host ?>/media/select2/select2.js" type="text/javascript"></script>
<script src="/media/auth_functions.js" type="text/javascript"></script>
<style>
    #addNewStructureContainer{
        display: none;
    }
    .formheading{
        color:black;
    }
</style>
<script type="text/javascript">
    $(function() {
        $('#toggleStuctureContainer').on('change',function(){
            $('#addNewStructureContainer').toggle();
        });

        getSkins();


        $(".display").dataTable({'sPaginationType':"full_numbers"});
        $('#pes').select2({
            width: "220px"
        });


        var inputIp='<div class="entry input-append">' +
            '<input class="form-control" name="ips[]" type="text" placeholder="Allowed Ips" />' +
            '<button class="btn btn-success btn-add" type="button">' +
            '<span class="glyphicon glyphicon-plus">+</span>' +
            '</button>' +
            '</div>';

        $(document).on('click', '.btn-add', function(e)
        {
            e.preventDefault();
            var controlForm = $('#field'),
                currentEntry = $(inputIp),
                newEntry = $(currentEntry.clone()).appendTo(controlForm);

            newEntry.find('input').val('');
            controlForm.find('.entry:not(:last) .btn-add')
                .removeClass('btn-add').addClass('btn-remove')
                .removeClass('btn-success').addClass('btn-danger')
                .html('<span class="glyphicon glyphicon-minus">-</span>');
        }).on('click', '.btn-remove', function(e)
        {
            $(this).parents('.entry:first').remove();
            e.preventDefault();
            return false;
        });

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

        $(document).on('click',".showAddModal", function() {
            $('#action').val('addSkin');
            $('#structureContainer').show();
            $('#toggleStuctureContainer').prop('checked',false);
            $('#addNewStructureContainer').hide();
            $('#field').empty().append(inputIp);
            $('#addSkin').text('Add');
            $('#skinForm')[0].reset();
            $('#key').val(MD5(Date.now().toString()).substring(0,8));
            $('#name').on('blur',function(){
                $("#folder").val($(this).val().toLowerCase());
            });
            $('#domainURL').on('blur',function(){
                $(this).val($(this).val().replace('http://',''));
              /*  var $val=$(this).val();
                splitArr = $val.split('.'),
                    arrLen = splitArr.length;
                domain = splitArr[arrLen - 2] + '.' + splitArr[arrLen - 1];*/
            domain=$(this).val();
                if(domain!='') {
                    $("#websiteURL").val('http://' + domain);
                    $("#mediaURL").val('http://media.' + domain);
                    $("#gamelauncherURL").val('http://flashgames.' + domain + '/www/gamelauncher.php');
                    $("#bettingURL").val('http://betting.' + domain);
                    $("#backofficeURL").val('admin.' + domain);
                    $("#activationURL").val('http://srv.' + domain + '/services/sites/UserAccountActivation.jsp');
                    $("#gatewayURL").val(domain);
                    $("#kioskURL").val('http://kiosk.' + domain);
                    $("#bannersURL").val('http://banners.' + domain);
                }
            });
        });

        $(document).on('click',".updateSkins", function() {
            $('#domainURL').off('blur');
            $('#name').off('blur');
            $('#structureContainer').hide();
            $('#skinForm')[0].reset();
            $('#field').empty();
            $('#action').val('updateSkin');
            $('#addSkin').text('Update');
            var skin = $(this).closest('tr');
            var providers=$.trim($(skin).find('td:eq(9) .details').text()).split(',');
            var allowedIps=$(skin).find('td:eq(5) .ip');

            allowedIps.each(function() {

                var controlForm = $('#field'),
                    currentEntry = $(inputIp),
                    newEntry = $(currentEntry.clone()).appendTo(controlForm);
                newEntry.find('input').val($(this).text());
                controlForm.find('.entry:not(:last) .btn-add')
                    .removeClass('btn-add').addClass('btn-remove')
                    .removeClass('btn-success').addClass('btn-danger')
                    .html('<span class="glyphicon glyphicon-minus">-</span>');
            });
            $('#addGeneralGame').html('<?=$lang->getLang("Update")?>');
            $('#modalGeneralHeader').html('<?=$lang->getLang("Update game")?>');
            $('#skinid').val($.trim($(skin).find('td:eq(0)').text()));
            $('#name').val($.trim($(skin).find('td:eq(1)').text()));
            $("#domainURL").val($.trim($(skin).find('td:eq(2)').find('.domain').text()));
            $("#websiteURL").val($.trim($(skin).find('td:eq(2)').find('.website').text()));
            $("#mediaURL").val($.trim($(skin).find('td:eq(2)').find('.media').text()));
            $("#gamelauncherURL").val($.trim($(skin).find('td:eq(2)').find('.gamelauncher').text()));
            $("#bettingURL").val($.trim($(skin).find('td:eq(2)').find('.betting').text()));
            $("#backofficeURL").val($.trim($(skin).find('td:eq(2)').find('.backoffice').text()));
            $("#activationURL").val($.trim($(skin).find('td:eq(2)').find('.activation').text()));
            $("#gatewayURL").val($.trim($(skin).find('td:eq(2)').find('.gateway').text()));
            $("#kioskURL").val($.trim($(skin).find('td:eq(2)').find('.kiosk').text()));
            $("#bannersURL").val($.trim($(skin).find('td:eq(2)').find('.banners').text()));
            $("#folder").val($.trim($(skin).find('td:eq(3)').text()));
            $("#status").val($.trim($(skin).find('td:eq(4)').text())=='Enabled'?1:0);
            $("#gHeight").val($.trim($(skin).find('td:eq(5)').text()));
            $("#clubs option").filter(function() {
                return $(this).text() == $.trim($(skin).find('td:eq(6)').text());
            }).prop('selected', true);
            $("#key").val($.trim($(skin).find('td:eq(7)').text()));
            $("#email").val($.trim($(skin).find('td:eq(8)').find('.email').text()));
            $("#smtp").val($.trim($(skin).find('td:eq(8)').find('.smtp').text()));

            $("#pes option").filter(function() {
                return providers.indexOf($(this).text())>=0;
            }).prop('selected', true);
            $("#pes").trigger('change.select2');
            $("#params").val($.trim($(skin).find('td:eq(10) .params').map(function(i, element){return $(element).text()}).get().join("~")));
        });
        $(document).on('click','.showDetails',function(){
            $(this).siblings('.details').toggle();
        });

        $(document).on('click','#addSkin',function(){
            $.ajax({
                url: '/services/general_services.inc',
                method: "POST",
                data:$('#skinForm').serializeArray(),
                dataType:'json'

            }) .done(function(data) {

                if(data[0]=='1'){
                    $.jAlert({
                        'title': 'Success',
                        'content': 'Operation successful!',
                        'theme': 'green',
                        'btns': {'text':'Ok', 'theme': 'green'}
                    });
                    getSkins();
                    return false;

                }

                    $.jAlert({
                        'title': 'Error',
                        'content': data[1],
                        'theme': 'red',
                        'btns': {'text':'Ok', 'theme': 'red'}
                    });
                    return false;

            })
                .fail(function(error) {
                    console.log(error);
                });
        });

        function getSkins(){
            $.ajax({
                url: '/services/general_services.inc',
                method: "POST",
                data: {
                    action:'getSkins'
                }
            }) .done(function(data) {
                $('#skinsContainer').empty().append(data);
                $("#skinsContainer>.display").DataTable({'sPaginationType':"full_numbers",
                    "rowCallback" : function(nRow) {
                        $('.details').css('display','none');
                    }
                });

            })
                .fail(function(error) {
                    console.log(error);
                });
        }

    });

</script>


<div id="myModal" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
    <div class="modal-header">
        <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
        <h3 class="text-info text-center" id="modalHeader"><?=$lang->getLang("Skins")?> </h3> <br />
    </div>
    <div class="modal-body">
        <form id="skinForm">
            <table class="table  table-striped">
                <input type="hidden" name="action" id="action" value="addSkin">
                <input type="hidden" name="skinid" id="skinid" value="">
                <tr>
                    <td><?=$lang->getLang("Name")?></td>
                    <td><input type="text" id="name" name="name" value=''></td>
                </tr>
                <tr>
                    <td><?=$lang->getLang("Domain")?></td>
                    <td><input type="text" id="domainURL" name="domainURL" value=''></td>
                </tr>
                <tr>
                    <td><?=$lang->getLang("Website")?></td>
                    <td><input type="text" id='websiteURL' name='websiteURL'></td>
                </tr>
                <tr>
                    <td><?=$lang->getLang("Media")?></td>
                    <td><input type="text" id='mediaURL' name='mediaURL'></td>
                </tr>

                <tr>
                    <td><?=$lang->getLang("Game launcher")?></td>
                    <td><input type="text" id='gamelauncherURL' name='gamelauncherURL'></td>
                </tr>
                <tr>
                    <td><?=$lang->getLang("Betting")?></td>
                    <td><input type="text" id='bettingURL' name='bettingURL'></td>
                </tr>
                <tr>
                    <td><?=$lang->getLang("Backoffice")?></td>
                    <td><input type="text" id='backofficeURL' name='backofficeURL'></td>
                </tr>
                <tr>
                    <td><?=$lang->getLang("Activation")?></td>
                    <td><input type="text" id='activationURL' name='activationURL'></td>
                </tr>
                <tr>
                    <td><?=$lang->getLang("Gateway")?></td>
                    <td><input type="text" id='gatewayURL' name='gatewayURL'></td>
                </tr>
                <tr>
                    <td><?=$lang->getLang("Kiosk")?></td>
                    <td><input type="text" id='kioskURL' name='kioskURL' ></td>
                </tr>
                <tr>
                    <td><?=$lang->getLang("Banners")?></td>
                    <td><input type="text" id='bannersURL' name='bannersURL'></td>
                </tr>

                <tr>
                    <td><?=$lang->getLang("Folder")?></td>
                    <td><input type="text" id="folder" name='folder' value=''></td>
                </tr>
                <tr>
                    <td><?=$lang->getLang("Status")?></td>
                    <td>
                        <select id="status" name="status">
                            <option value="1">Enabled</option>
                            <option value="0">Disabled</option>
                        </select>
                    </td>
                </tr>
                <tr>
                    <td><?=$lang->getLang("Allowed ips")?></td>
                    <td><!--<div id="allips">
                            <input type="text" id='ips' name='ips'>
                            <button id="addIp">+</button>
                            <button id="removeIp">-</button>
                        </div>-->
                        <div id="field">
                            <div class="entry input-append">
                                <input class="form-control" name="ips[]" type="text" placeholder="Allowed Ips" />
                                <button class="btn btn-success btn-add" type="button">
                                    <span class="glyphicon glyphicon-plus">+</span>
                                </button>
                            </div>
                        </div>

                    </td>
                </tr>

                <tr>
                    <td><?=$lang->getLang("Club")?></td>
                    <td><select id='clubs' name="clubs"><?
                            $clubs = getAllClubs ();
                            while ( $clubs->hasNext () ) {
                                $club = $clubs->next ();
                                ?><option value='<?=$club['jur_id']?>'><?=$club['jur_name']?></option>
                            <?php }?>
                        </select></td>
                </tr>
                <tr>
                    <td><?=$lang->getLang("Key")?></td>
                    <td><input type="text" id="key" name="key" value=''></td>
                </tr>
                <tr>
                    <td><?=$lang->getLang("Email")?></td>
                    <td><input type="text" id="email" name='email' value=''></td>
                </tr>
                <tr>
                    <td>SMTP</td>
                    <td><input type="text" id="smtp" name='smtp' value=''></td>
                </tr>
                <tr>
                    <td>Providers enabled</td>
                    <td>
                        <select name='pes[]'  id='pes' multiple='multiple' >
                            <?
                            $providers=$dbh->doCachedQuery("Select * from providers");
                            while($providers->hasNext()){
                                $row=$providers->next();
                                ?>
                                <option value='<?=$row['pes_game_code']?>'  ><?=$row['pes_name']?></option>
                            <? } ?>
                        </select></td>
                </tr>
                <tr>
                    <td>Params</td>
                    <td><textarea cols="3" rows="5" id="params" name='params'></textarea></td>
                </tr>
                <tr id="structureContainer">
                    <td>Create structure</td>
                    <td><input type="checkbox" id="toggleStuctureContainer" name="addStructure"><br />
                        <div id="addNewStructureContainer" style="display: none;">
                            <table class="table table-condensed table-bordered" >
                                <tr>
                                    <td colspan="2"><h5><?=$lang->getLang("Structure details")?></h5></td>
                                </tr>
                                <?php
                             /*   if(empty($_POST['account_code'])){
                                    $_POST['account_code'] = strtoupper(get_free_jurcode());
                                }
                                */?><!--
                                <tr>
                                    <td><?/*=$lang->getLang("Account Code")*/?></td>
                                    <td><input type="text" name="account_code" value="<?/*=replace_quotes($_POST['account_code'])*/?>"   maxlength="6" placeholder="6 characters"/></td>
                                </tr>-->
                                <tr>
                                    <td><?=$lang->getLang("Currency Type")?><div id="cur"></div><br /></td>
                                    <td><?=getAllCurrency($currency);?></td>
                                </tr>
                                <tr>
                                    <td><?=$lang->getLang("Country")?></td>
                                    <td <? form_td_style('country'); ?>>
                                        <?countrySelect('country', "", $_POST['user_country'],false,'country'); ?>
                                        <?php echo err_field('user_country'); ?>
                                    </td>
                                </tr>

                                <tr>
                                    <td><?=$lang->getLang("City")?></td>
                                    <td><input type="text" name="city" id="city" value="" /></td>
                                </tr>

                                <tr>
                                    <td><?=$lang->getLang("Timezone")?></td>
                                    <td><select name="utc">
                                            <?for($i=-12;$i<=12;$i++){
                                                ?>
                                                <option value="<?=$i?>" <?=($i==0? 'selected':'')?> ><?=$i?>:00 UTC</option>
                                                <?
                                            }
                                            ?>
                                        </select>
                                    </td>
                                </tr>

                                <tr>
                                    <td><?=$lang->getLang("Address Line 1")?></td>
                                    <td><input type="text" name="address1" id="address"  value="<?php if ( areErrors() ) { echo replace_quotes($_POST['address1']); } ?>"  /></td>
                                </tr>
                                <tr>
                                    <td><?=$lang->getLang("Postcode or Zipcode")?></td>
                                    <td><input type="text" name="postc" value="<?php if ( areErrors() ) { echo replace_quotes($_POST['postc']); } ?>" /></td>
                                </tr>
                                <tr>
                                    <td><?=$lang->getLang("Contact Number")?> </td>
                                    <td><input type="text" name="phone" value="<?php if ( areErrors() ) { echo replace_quotes($_POST['phone']); } ?>"  /></td>
                                </tr>
                                <tr>
                                    <td valign="top"><?=$lang->getLang("Notes")?></td>
                                    <td ><textarea name="notes" rows="4" cols="4" wrap="hard"><?php if ( areErrors() ) { echo replace_quotes($_POST['notes']); } ?></textarea></td>
                                </tr>
                            </table>

                        </div>

                    </td>
                </tr>
            </table>
        </form>
    </div>
    <div class="modal-footer">
        <button class="btn" data-dismiss="modal" aria-hidden="true"><?=$lang->getLang("Cancel")?></button>
        <button class="btn btn-primary" id="addSkin" name="save" ><?=$lang->getLang("Add")?></button>
    </div>
</div>


<div class="container" style="position: absolute;left:0;right: 0;width: 100%" >
    <div class="container-fluid">
        <table class="table table-bordered">
            <tr><td class='navbar-inner'><h3 class="text-center text-error"><?=$lang->getLang("Manage Skins")?></h3></td></tr>
            <tr>
                <td>
                    <div><a href="#myModal" class="fright btn btn-info btn-small showAddModal" style="color:white;text-decoration: none;" data-toggle="modal" ><?=$lang->getLang("Add a new skin")?></a></div><br><br><br>
                    <div id="skinsContainer"></div>
                </td>
            </tr>
        </table>
    </div>
</div>



