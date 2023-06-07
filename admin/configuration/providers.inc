<?php include_once 'partners_functions.inc';
$internal=check_access('manage_providers',$redirect=true);
/**
 *
 * @author Marian & @maintainer Alexandra
 */

$currency=getAllCurrencies();
if(isset($_POST['removeOldProvider'])) {
    $oldProviderId = $_POST['oldProviderId'];
    $sql = "DELETE FROM provider_users WHERE prs_provider_id = $oldProviderId";
    $response = $dbh->exec($sql);
    $response = "".$response;
    die($response);
}
?>

<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap-responsive.css" title="core_style" />
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<script src="/media/jquery.dataTables.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="/media/js/multiselect/css/multi-select.css" title="core_style">
<script src="/media/js/multiselect/js/jquery.multi-select.js" type="text/javascript"></script>
<script src="/media/jquery.quicksearch.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/select2/select2-4/css/select2.css" >
<script src="<?= secure_host ?>/media/select2/select2-4/js/select2.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/js/sweetalert/sweetalert.css"  />
<script src="<?= secure_host ?>/media/js/sweetalert/sweetalert.min.js" type="text/javascript"></script>
<script type="text/javascript">
    $(function() {
        addModalEnabled = false;
        getProviders();
         providerTtable = $(".display").dataTable({'sPaginationType':"full_numbers"});
        $('#occurrency').select2({
            width: "220px"
        });
        var inputIp='<div class="entry input-append">' +
            '<input class="form-control" name="ips[]" type="text" placeholder="Allowed Ips" />' +
            '<button class="btn btn-success btn-add" type="button">' +
            '<span class="glyphicon glyphicon-plus">+</span>' +
            '</button>' +
            '</div>';
        $(document).on('click','.showDetails',function(){
            $(this).siblings('.details').toggle();
        });
        $(document).on('click', '.btn-add', function(e){
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

        $(document).on('click',".showAddModal", function() {
            addModalEnabled = true;
            $('#action').val('addProvider');
            $('#field').empty().append(inputIp);
            $('#addProvider').text('Add');
            $('#providerForm')[0].reset();
            $('#group').val("");
            $('#providerid').val("");
            $('#providerNewId').val("");
            $('#transfer_type').val("NONE");
            $('#mcurrency').val(1);
            $('#hmcurrency').val(0);
            $('#occurrency').val('');
            $('#occurrency').closest('td').find('.select2-search-choice').remove();

        });
        $(document).on('click',".updateProviders", function() {
            addModalEnabled = false;
            $('#providerForm')[0].reset();
            $('#field').empty();
            $('#action').val('updateProvider');
            $('#addProvider').text('Update');
            var provider = $(this).closest('tr');
            var otherCurrencies=$.map($(provider).find('td:eq(13)').text().split(';'),$.trim);

            var allowedIps=$(provider).find('td:eq(2)').text().split('|');
            allowedIps.forEach(function(ip) {
                var controlForm = $('#field'),
                    currentEntry = $(inputIp),
                    newEntry = $(currentEntry.clone()).appendTo(controlForm);
                newEntry.find('input').val(ip);
                controlForm.find('.entry:not(:last) .btn-add')
                    .removeClass('btn-add').addClass('btn-remove')
                    .removeClass('btn-success').addClass('btn-danger')
                    .html('<span class="glyphicon glyphicon-minus">-</span>');
            });

            $('#providerid').val($.trim($(provider).find('td:eq(0)').text()));
            $('#providerNewId').val($.trim($(provider).find('td:eq(0)').text()));
            $('#name').val($.trim($(provider).find('td:eq(1)').text()));
            $("#group").val($.trim($(provider).find('td:eq(3)').text()));
            $("#remoteId").val($.trim($(provider).find('td:eq(4)').text()));

            $("#games").val($.trim($(provider).find('td:eq(5)').text()));
            $("#gameId").val($.trim($(provider).find('td:eq(6)').text()));
            $("#key").val($.trim($(provider).find('td:eq(7)').text()));
            $("#websiteURL").val($.trim($(provider).find('td:eq(8)').text()));

            var status = 0;
            if($(provider).find('td:eq(9)').text() == 'Enabled') status = 1;
            else if($(provider).find('td:eq(9)').text() == 'Disabled') status = 0;
            else status = 2;
            $("#status").val(status);

            $("#transfer_type").val($.trim($(provider).find('td:eq(10)').text())=='NONE'?'NONE':($.trim($(provider).find('td:eq(10)').text())));
            $("#params").val($.trim($(provider).find('td:eq(11) .params').map(function(i, element){return $(element).text()}).get().join("~")));
            $("#mcurrency option").filter(function() {
                return $(this).text() == $.trim($(provider).find('td:eq(12)').text());
            }).prop('selected', true);
            $("#occurrency option").filter(function() {
                return otherCurrencies.indexOf($.trim($(this).text()))>=0;
            }).prop('selected', true);
            $("#occurrency").trigger('change.select2');
            $("#hmcurrency").val($.trim($(provider).find('td:eq(14)').text())=='Yes'?1:0);

            var provId = $('#providerNewId').val();

            $('#providerNewId').change(function(){
                var newProvId = $('#providerNewId').val();
                if(provId != newProvId && !addModalEnabled) {
                    swal({
                        title: "Provider Warning",
                        text: "If you continue, the old provider will be removed",
                        type: 'warning',
                        showCancelButton: true,
                    },
                        function() {
                            $.post("configuration/providers.inc", {
                                removeOldProvider : "yes",
                                oldProviderId: provId
                            }, function(data) {
                                if (data == "1") {
                                    swal({
                                        title: "Provider removed",
                                        text: "The old provider was removed",
                                        type: "success"
                                    });
                                } else {
                                    swal({
                                        title: "Provider unchanged",
                                        text: "The old provider could not be removed",
                                        type: "error"
                                    });
                                }
                            })
                        })
                }
            });
        });

        $(document).on('click',".cloneProviders", function() {
            addModalEnabled = false;
            $('#providerForm')[0].reset();
            $('#field').empty();
            $('#action').val('addProvider');
            $('#addProvider').text('Clone');
            var provider = $(this).closest('tr');
            var otherCurrencies=$.map($(provider).find('td:eq(13)').text().split(';'),$.trim);

            var allowedIps=$(provider).find('td:eq(2)').text().split('|');
            allowedIps.forEach(function(ip) {
                var controlForm = $('#field'),
                    currentEntry = $(inputIp),
                    newEntry = $(currentEntry.clone()).appendTo(controlForm);
                newEntry.find('input').val(ip);
                controlForm.find('.entry:not(:last) .btn-add')
                    .removeClass('btn-add').addClass('btn-remove')
                    .removeClass('btn-success').addClass('btn-danger')
                    .html('<span class="glyphicon glyphicon-minus">-</span>');
            });

            // $('#providerid').val('');
            $('#providerNewId').val('');
            $('#name').val($.trim($(provider).find('td:eq(1)').text()));
            $("#group").val($.trim($(provider).find('td:eq(3)').text()));
            // $("#remoteId").val($.trim($(provider).find('td:eq(4)').text()));

            // $("#games").val($.trim($(provider).find('td:eq(5)').text()));
            $("#gameId").val($.trim($(provider).find('td:eq(6)').text()));
            // $("#key").val($.trim($(provider).find('td:eq(7)').text()));
            $("#websiteURL").val($.trim($(provider).find('td:eq(8)').text()));

            var status = 0;
            if($(provider).find('td:eq(9)').text() == 'Enabled') status = 1;
            else if($(provider).find('td:eq(9)').text() == 'Disabled') status = 0;
            else status = 2;
            $("#status").val(status);

            $("#transfer_type").val($.trim($(provider).find('td:eq(10)').text())=='NONE'?'NONE':'ALL');
            $("#params").val($.trim($(provider).find('td:eq(11) .params').map(function(i, element){return $(element).text()}).get().join("~")));
            $("#mcurrency option").filter(function() {
                return $(this).text() == $.trim($(provider).find('td:eq(12)').text());
            }).prop('selected', true);
            $("#occurrency option").filter(function() {
                return otherCurrencies.indexOf($.trim($(this).text()))>=0;
            }).prop('selected', true);
            $("#occurrency").trigger('change.select2');
            $("#hmcurrency").val($.trim($(provider).find('td:eq(14)').text())=='Yes'?1:0);

            $('#providerNewId').off('change');
        });


        $(document).on('click','#addProvider',function(){

            $.ajax({
                url: '/services/general_services.inc',
                method: "POST",
                data:$('#providerForm').serializeArray()
            }) .done(function(data) {
                data=data.replaceAll(/\s/g,'');
                if(data=='1'){
                    $.jAlert({
                        'title': 'Success',
                        'content': 'Operation successful!',
                        'theme': 'green',
                        'btns': {'text':'Ok', 'theme': 'green'}
                    });
                    getProviders();
                    return false;

                }
                else if(data=='2')
                {
                    $.jAlert({
                        'title': 'Error',
                        'content': 'Provider name already exists!',
                        'theme': 'red',
                        'btns': {'text':'Ok', 'theme': 'red'}
                    });
                    return false;
                }
                else if(data=='4')
                {
                    $.jAlert({
                        'title': 'Error',
                        'content': 'Provider key already exists!',
                        'theme': 'red',
                        'btns': {'text':'Ok', 'theme': 'red'}
                    });
                    return false;
                }
                else if(data=='5')
                {
                    $.jAlert({
                        'title': 'Error',
                        'content': 'Provider id already exists!',
                        'theme': 'red',
                        'btns': {'text':'Ok', 'theme': 'red'}
                    });
                    return false;
                }
                else if(data=='3')
                {
                    $.jAlert({
                        'title': 'Error',
                        'content': 'Provider game code already exists!',
                        'theme': 'red',
                        'btns': {'text':'Ok', 'theme': 'red'}
                    });
                    return false;
                }
                else
                {
                    $.jAlert({
                        'title': 'Error',
                        'content': 'An error has occurred.Please try again!',
                        'theme': 'red',
                        'btns': {'text':'Ok', 'theme': 'red'}
                    });
                    return false;
                }

            })
                .fail(function(error) {
                    console.log(error);
                });
        });

        function getProviders(){
            $.ajax({
                url: '/services/general_services.inc',
                method: "POST",
                data: {
                    action:'getProviders'
                }
            }) .done(function(data) {
                $('#providersContainer').empty().append(data);
                $("#providersContainer>.display").DataTable({
                    'sPaginationType':"full_numbers",
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
        <h3 class="text-info text-center" id="modalHeader"><?=$lang->getLang("Providers")?> </h3> <br />
    </div>
    <div class="modal-body">
        <form id="providerForm">
            <table class="table  table-striped">
                <input type="hidden" name="action" id="action" value="addProvider">
                <input type="hidden" name="providerid" id="providerid" value="">
                <tr>
                    <td><?=$lang->getLang("Id")?></td>
                    <td><input class="form-control" type="text" name="providerNewId" id="providerNewId" value=""></td>
                </tr>
                <tr>
                    <td><?=$lang->getLang("Name")?></td>
                    <td><input type="text" id="name" name="name" value=''></td>
                </tr>
                <tr>
                    <td><?=$lang->getLang("Allowed ips")?></td>
                    <td>
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
                    <td><?=$lang->getLang("Provider group")?></td>
                    <td><input type="text" id='group' name='group'></td>
                </tr>
                <tr>
                    <td><?=$lang->getLang("Remote id")?></td>
                    <td><input type="text" id='remoteId' name='remoteId'></td>
                </tr>
                <tr>
                    <td><?=$lang->getLang("Game code")?></td>
                    <td><input type="text" id="games" name="games" value=''></td>
                </tr>
                <tr>
                    <td><?=$lang->getLang("Game id")?></td>
                    <td><input type="text" id="gameId" name="gameId" value=''></td>
                </tr>
                <tr>
                    <td><?=$lang->getLang("Key")?></td>
                    <td><input type="text" id="key" name="key" value=''></td>
                </tr>
                <tr>
                    <td><?=$lang->getLang("Website")?></td>
                    <td><input type="text" id='websiteURL' name='websiteURL'></td>
                </tr>
               <tr>
                    <td><?=$lang->getLang("Status")?></td>
                    <td>
                        <select id="status" name="status">
                            <option value="1">Enabled</option>
                            <option value="0">Disabled</option>
                            <option value="2">Enabled with withraw</option>
                        </select>
                    </td>
                </tr>
                <tr>
                    <td><?=$lang->getLang("Transfer type")?></td>
                    <td>
                        <select id="transfer_type" name="transfer_type">
                            <option value="NONE">None</option>
                            <option value="ALL">All</option>
                            <option value="seamless">seamless</option>
                        </select>
                    </td>
                </tr>
                <tr>
                    <td><?=$lang->getLang("Params")?></td>
                    <td><textarea cols="3" rows="5" id="params" name='params'></textarea></td>
                </tr>
                <tr>
                    <td><?=$lang->getLang("Master currency")?></td>
                    <td>
                        <select id='mcurrency' name='mcurrency'>
                         <?   while ($currency->hasNext()) {
                                    $row = $currency->next();
                                    ?>
                            <option value="<?=$row['cur_id']?>"><?=$row['cur_code']?></option>
                        <? }?>
                        </select>

                    </td>
                </tr>
                <tr>
                    <td><?=$lang->getLang("Other currency")?></td>
                    <td>
                        <select name='ocurrency[]'  id='occurrency' multiple='multiple' >
                            <? $currency->resetRecPtr();
                              while ($currency->hasNext()) {
                                $row = $currency->next();
                                ?>
                                <option value='<?=$row['cur_id']?>'  ><?=$row['cur_code']?></option>
                            <? } ?>
                        </select>

                    </td>
                </tr>
                <tr>
                    <td><?=$lang->getLang("Has multi currency")?></td>
                    <td>
                        <select id="hmcurrency" name="hmcurrency">
                            <option value="1">Yes</option>
                            <option value="0">No</option>
                        </select>
                    </td>
                </tr>
            </table>
        </form>
    </div>
    <div class="modal-footer">
        <button class="btn" data-dismiss="modal" aria-hidden="true"><?=$lang->getLang("Cancel")?></button>
        <button class="btn btn-primary" id="addProvider" name="save" ><?=$lang->getLang("Add")?></button>
    </div>
</div>


<div class="container" style="position: absolute;left:0;right: 0;width: 100%" >
    <div class="container-fluid">
        <table class="table table-bordered">
            <tr><td class='navbar-inner'><h3 class="text-center text-error"><?=$lang->getLang("Manage Providers")?></h3></td></tr>
            <tr>
                <td>
                    <div><a href="#myModal" class="fright btn btn-info btn-small showAddModal" style="color:white;text-decoration: none;" data-toggle="modal" ><?=$lang->getLang("Add a new provider")?></a></div><br><br><br>
                    <div id="providersContainer"></div>
                </td>
            </tr>
        </table>
    </div>
</div>
