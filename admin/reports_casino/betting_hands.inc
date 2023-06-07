<script src="<?= secure_host ?>/media/jquery.dataTables.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/auth_functions.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>

<script src="<?= secure_host ?>/media/ui/js/jquery.timepicker.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media//select2/select2.css" >
<script src="<?= secure_host ?>/media/select2/select2.js" type="text/javascript"></script>
<style>
    .select2-container .select2-choice {
        border-radius: 0;
        height: 28px;
        border-top-right-radius: 4px;
        border-bottom-right-radius: 4px;
    }
    #loadingDiv {
        display: none;
        background-color: rgba(255, 255, 255, 0.47);
        font-size: 12px;
        z-index: 1010;
        position: fixed;
        top: 0;
        bottom: 0;
        left: 0;
        right: 0;
        margin: auto;
        padding-top: 200px;
    }
</style>
<script type="text/javascript">
    $(function() {
        var servicesUrl="/services/general_services.inc";
        /*    $('#searchValue').typeahead({
         source: function(query, process) {
         return $.ajax({
         url: "/services/general_services.inc",
         type: 'get',
         data: {value: query , action:'getUsers' , type:$('#searchBy').val()},
         dataType: 'json',
         success: function(json) {
         return typeof json.options == 'undefined' ? false : process(json);
         }
         });
         }
         }); */

        <?php
        if(!isset($_REQUEST['username'])){
        ?>
        $("#searchValue").select2({
            placeholder: 'Type to search',
            ajax: {
                url: servicesUrl,
                dataType: 'json',
                delay: 250,
                data: function (params,page) {
                    return {
                        page: page,
                        start:page*10,
                        value: params ,
                        action:'getUsers' ,
                        type:$('#searchBy').val(),
                        exactResult:$('[name="includetotals"]:checked').val()
                    };
                },
                processResults: function (data, params) {
                    params.page = params.page || 1;
                    return {
                        results: $.map(data, function (item) {
                            return {
                                text: item.username,
                                id: item.customer_number
                            }
                        }),
                        pagination: {
                            more: (params.page * 30) < data.recordsTotal
                        }
                    };
                },
                cache: true
            },
            escapeMarkup: function (markup) { return markup; }, // let our custom formatter work
            minimumInputLength: 3
        });
    <?
        }
        ?>



        $( "#from" ).datetimepicker({
            changeMonth: true,
            numberOfMonths:1,
            onChangeMonthYear:function(year, month, i){
                var day = i.selectedDay;
                $(this).datepicker('setDate', new Date(year, month-1, day));
            }

        });
        $( "#to" ).datetimepicker({
            changeMonth: true,
            numberOfMonths:1,
            onChangeMonthYear:function(year, month, i){
                var day = i.selectedDay;
                $(this).datepicker('setDate', new Date(year, month-1, day));
            }
        });

        $(".display").dataTable({'sPaginationType':"full_numbers",'iDisplayLength': 100});



        $('#searchLabel').text('<?=$lang->getLang("Search")?> '+ $('#searchBy').find('option:selected').text());
        if($('#searchBy').find('option:selected').val()!=3){
            $('#dateContainer').show();

        }
        else{
            $('#dateContainer').hide();
        }

        $('#searchBy').change(function() {
            $('#searchLabel').text('<?=$lang->getLang("Search")?> '+ $(this).find('option:selected').text());
            if($(this).find('option:selected').val()!=3){
                $('#dateContainer').show();
            }
            else{
                $('#dateContainer').hide();
            }
        });
        $('#viewIframe').on('click',function(){

            if($('#searchValue').val()==''){
               jAlert('Please select an user');
            }
            else {
                $('#loadingDiv').show();
                try {
                    $.ajax({
                        url: servicesUrl,
                        method: "POST",
                        data: {
                            action: 'getUserSession',
                            <?
                            if(!isset($_REQUEST['username'])){ ?>
                            username: $('#searchValue').select2('data')['text'],
                            <? }else {?>
                            username: $('#searchValue').text().trim(),
                            <? } ?>
                            extuid: $('#searchValue').val()
                        }
                    }).done(function (data) {
                        if(data<0) {
                            $.jAlert({
                                'title': 'Error',
                                'content': 'Unable to find user details!',
                                'theme': 'red',
                                'btns': {'text':'Ok', 'theme': 'red'}
                            });
                         }
                        else{
                            $('#betting_result_iframe').attr('src', '<?=BETTING_HISTORY_URL?>?walletcode=<?=BETTING_WCODE?>&token=' + data + '&lang=en&skinid=J87HHU65').show();

                        }
                        })
                        .fail(function (error) {
                            console.log(error);
                            return false
                        }).always(function () {
                        $('#loadingDiv').hide();
                    });
                }
                catch (e) {
                    console.log(e);
                    $('#loadingDiv').hide();
                }
            }
        });

        <?php
        if(isset($_REQUEST['username']))
        {
        ?>
        $('#viewIframe').trigger('click');
        <?
        }
        ?>

    });

</script>

<?php

define("SEARCH_MAX_HOURS", 1000);
globalise('action');
globalise('searchBy');
globalise('searchValue');
globalise('from');
globalise('to');
globalise('games');
globalise('includetotals');

$name='searchuserhand';
if(isset($_REQUEST['username'])){
    $searchBy=1;
    $searchValue=$_REQUEST['username'];
}
if($start_time=strtotime($from)){
    $from = date("Y-m-d H:i", $start_time);
}else{
    $from = date("Y-m-d 00:00");
}

if($end_time=strtotime($to)){
    $to = date("Y-m-d H:i", $end_time);
}else{
    $to = date("Y-m-d 23:59");
}



?>
<div id='loadingDiv' class="centered"><img src="<?=image_dir?>/ajax-loader.gif" /></div>

<table class="table table-bordered" >
    <tr>
        <td class="navbar-inner" ><h3 class="text-center"><?=$lang->getLang("BIA Tickets")?></h3></td>
    </tr>
    <tr>
        <td>
            <div class="well">
                <form action="" method="post" id="searchuserhand" name="searchuserhand">
                    <table>
                        <tr>
                            <td style="border: 0px;">
                                <?if(!isset($_REQUEST['username'])){ ?>
                                <div class="span4">
                                    <div class='label label-inverse'><?=$lang->getLang("Search by")?></div>
                                    <br>
                                    <div class="controls">
                                        <div class="input-prepend">
                                            <span class="add-on"><i class="icon-user"></i></span>
                                            <select name="searchBy" id="searchBy">
                                                <option <?=($searchBy==1 ? 'selected="selected"': '')?> value="1"  ><?=$lang->getLang("Username")?></option>
                                                <option <?=($searchBy==2 ? 'selected="selected"': '')?> value="2"  ><?=$lang->getLang("Email")?></option>
                                            </select>
                                        </div>
                                    </div>
                                </div>
                                <? } ?>
                                <div class="span4"><div class='label label-inverse' id="searchLabel"><?=$lang->getLang("Search Username")?></div>
                                    <br />
                                    <div class="controls">
                                        <div class="input-prepend">
                                            <span class="add-on"><i class="icon-list"></i></span>
                                            <?if(isset($_REQUEST['username'])){ ?>
                                                <select type='text'  id="searchValue" name='searchValue'>
                                                    <option value="<?=$_REQUEST['uid']?>" selected><?=$_REQUEST['username']?></option>
                                                </select>
                                            <? }
                                            else {
                                            ?>
                                            <input type='text'  id="searchValue" name='searchValue'  placeholder="<?=$lang->getLang('Type to start searching')?>"  style="width: 200px" required="required" value="<?=$searchValue?>">
                                            <? } ?>
                                        </div>
                                    </div>
                                </div>
                                <?if(!isset($_REQUEST['username'])){ ?>
                                <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Exact match")?></div>
                                    <br />
                                    <input type="radio" name="includetotals" <?=($includetotals=="2"? "checked" :"" )?> class="yes" id="yes"  value="2" />
                                    <input type="radio" name="includetotals" class="no" id="no" <?=($includetotals!="2"? "checked" :"" )?>  value="1" />
                                    <div class="switch">
                                        <label class="labelYes" for="yes"><?=$lang->getLang("Yes")?></label>
                                        <label class="labelNo" for="no"><?=$lang->getLang("No")?></label>
                                        <span></span>
                                    </div>
                                </div>
                                <? }?>
                            </td>
                        </tr>
                        <tr>
                            <td style="border: 0;" id="dateContainer">
                                <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Date")?></div>
                                    <br>
                                    <div class="controls">
                                        <div class="input-prepend">
                                            <span class="add-on"><i class="icon-calendar"></i></span>
                                            <?createDateHelper("date_helper", SEARCH_MAX_HOURS, "from", "to", $name)?>
                                        </div>
                                    </div>
                                </div>
                                <div class="span4"><div class='label label-inverse'><?=$lang->getLang("From")?></div>
                                    <br>
                                    <div class="controls">
                                        <div class="input-prepend">
                                            <span class="add-on"><i class="icon-calendar"></i></span>
                                            <input type='text' name='from' id='from'  value="<?=$from?>" >
                                        </div>
                                    </div>
                                </div>
                                <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Until")?></div>
                                    <br>
                                    <div class="controls">
                                        <div class="input-prepend">
                                            <span class="add-on"><i class="icon-calendar"></i></span>
                                            <input type='text' name='to' id='to'  value="<?=$to?>" >
                                        </div>
                                    </div>
                                </div>
                            </td>
                        </tr>
                    </table>
                    <br />
                    <br />
                    <input type="hidden" value="searchuserhands" name="action" />
                    <button type="button" class="btn btn-primary" id="viewIframe"><?=$lang->getLang("View")?></button>
                    <br>
                </form>
            </div>

        </td>
    </tr>
</table>

<iframe name="betting_result_iframe" id='betting_result_iframe'  style="width:100%;height: 550px;text-align:center;display: none">
</iframe><br>
<?


setupDateInputJs("from", "to");


?>

