<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/dataTablesExtra/TableTools-2.2.0/css/dataTables.tableTools.css" title="core_style" />
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/jquery.dataTables.js" type="text/javascript"></script>
<?$status = (isset($_POST['status'])) ? $_POST['status'] : '0';?>
<?$months = (isset($_POST['months'])) ? $_POST['months'] : '30';?>
<?if(isset($_POST['status'])){ ?>
<script type="text/javascript" charset="utf-8">
    $(document).ready(function() {
        var startTime=null;
        var endTime=null;
        $(document).ajaxStart(function(){
            startTime=new Date;
        });
        $(document).ajaxStop(function(){
            endTime = new Date;
            totalTime = endTime - startTime;
            $("#duration").html('<?=$lang->getLang("Time taken to execute your request")?>:'+ totalTime/1000+" s");

        });

        $('.display').dataTable( {
            "sPaginationType": "full_numbers",
            "processing": true,
            "serverSide": true,
            "sServerMethod": "POST",
            "sAjaxSource": "../services/general_services.inc",
            "fnServerParams": function ( aoData ) {
                aoData.push( { "name": "action", "value": 'getInactivePlayers'},{ "name": "status", "value": $('#status').val()}, { "name": "months", "value": $('#months').val()});
            },
            "aoColumns": [
                { "bSearchable": true },
                { "bSearchable": true },
                { "bSearchable": true }
            ]
        } );

/*

        setTimeout(
            function()
            {
                data={action:"generateInactiveCsv",status: $('#status').val()};
                $.ajax({
                    type: "POST",
                    url: "../services/general_services.inc",
                    data: data,
                    async:true
                });

                interval=setInterval(checkFile,1000);
            }, 2000);*/


        var interval = null;
        file='<?=defined('INACTIVE_USERS_FILE_PATH')? INACTIVE_USERS_FILE_PATH : '/documents/inactivePlayers.csv'?>';
        function checkFile(){
            var http = new XMLHttpRequest();
            http.open('HEAD', file, false);
            http.send();
            if(http.status==200){
                $('#generatingExcel').empty().append("<a href='"+file+"'><?=$lang->getLang("Download as csv")?></a>");
                clearInterval(interval);
            }
        }
        <?if($_SESSION['jurisdiction_class']=='casino'){?>

        setTimeout(
            function()
            {
                interval=setInterval(checkFile,1000);
            }, 2000);
        <? }?>


    } );
</script>
<? }?>
<div class="well">
    <div class="centered"><h1 class='text-error'><?=$lang->getLang("Inactive players")?></h1></div><br />
    <br />
    <br />
    <form action="/inactive_customers" method="post">
        <div class="row">
            <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Group by")?></div>
                <br>
                <div class="controls">
                    <div class="input-prepend">
                        <span class="add-on"><i class="icon-align-justify"></i></span>
                        <select name="status" id="status">
                            <option value="0" <?=($status==0?'selected':'')?>><?=$lang->getLang("All")?></option>
                            <option value="1" <?=($status==1?'selected':'')?>><?=$lang->getLang("With credits")?></option>
                            <option value="2" <?=($status==2?'selected':'')?>><?=$lang->getLang("Without credits")?></option>
                        </select>
                    </div>
                </div>
            </div>
            <div class="span3" id="month"><div class='label label-inverse'><?=$lang->getLang("Months")?></div>
                <br>
                <div class="controls">
                    <div class="input-prepend">
                        <span class="add-on"><i class="icon-calendar"></i></span>
                        <input type='text' style="width: 50px"  name='months' id='months' value="<?=$months?>">
                    </div>
                </div>
            </div>
        </div>
        <div class="clearfix"></div>
        <div class="row">
            <span class="span2"><button class="btn btn-primary" id="SearchTicket"><?=$lang->getLang("Search")?></button></span>
        </div><br /><br />
    </form>
    <br />
    <br />
    <div id='duration' class="tip fleft"></div>
   <span id="generatingExcel" class="fright">
        <!--<?/*=$lang->getLang("Generating report..")*/?> <img src="media/images/ajax-loader.gif"> -->
    </span>
    <?if(isset($_POST['status'])){?>
        <br />
        <br />
    <div>
        <table class='display table table-hover table-striped table-bordered' style="width: 100%" >
            <thead>
            <tr>
                <th><?=$lang->getLang("User")?></th>
                <th><?=$lang->getLang("Last login")?></th>
                <th><?=$lang->getLang("Available Credit")?></th>
            </tr>
            </thead>
            <tbody>
            </tbody>
        </table>
        <br />
        <br />
        <div class="clearfix"></div>
    </div>
        <? }?>

</div>


