<?php
check_access('manage_accept_affiliate_registration',true);


$date_start = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d", time() - (60 * 60 * 24));
$date_end = (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d", time() + (60 * 60 * 24));
?>
<style>
    #affiliatesInfoTable{
        white-space: nowrap;
    }
    #affiliatesInfoTree{
        width: 200px;
    }

</style>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />

<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<script src="/media/jquery.dataTables.js" type="text/javascript"></script>
<script src="/media/affiliatesJs/ListTable.js" type="text/javascript"></script>
<script src="/media/affiliatesJs/resultFunctions.js" type="text/javascript"></script>
<script src="/media/jstree/jstree.js" type="text/javascript"></script>
<script src="/media/jquery.alerts.js" type="text/javascript"></script>

<div class="container">
    <div id="testresult"></div>
    <table class="table table-bordered">
        <tr>
            <td class="navbar-inner" colspan="2"><h3 class="text-center"><?=$lang->getLang("Confirm affiliate")?></h3></td>
        </tr>
        <tr>
            <td>
                <div class="well">
                    <div id="affiliatesInfoContainer">
                        <form name="affiliatesInfoTable_formFilter" id="affiliatesInfoTable_formFilter" method="post" action='affiliate_configuration'>
                            <div>
                                <div class="input-prepend">
                                    <span  class="add-on"><?=$lang->getLang("From")?></span>
                                    <input class="span2" type='text'  name='date_start' id='date_start' value="<?=$date_start?>">
                                </div>
                            </div>
                            <div>
                                <div class="input-prepend">
                                    <span class="add-on">&nbsp;<?=$lang->getLang("Until")?>&nbsp;</span>
                                    <input type='text' class="span2"  name='date_end' id='date_end' value="<?=$date_end?>">
                                </div>28
                            </div>
                            <button class="btn btn-primary"><?=$lang->getLang("Get Data")?></button>
                            <fieldset>
                                <legend><?=$lang->getLang("Affiliates")?></legend>
                                <div class="control-group">
                                    <input type="hidden" name="code" value="<?=$_POST['code']?>" />
                                    <input type="hidden" name="action" value="listAffiliates" />
                                </div>
                            </fieldset>
                        </form>
                        <table id="affiliatesInfoTable" class="display table table-striped table-bordered table-condensed">
                            <thead>
                            <tr>
                                <th>Pun id</th>
                                <th><?=$lang->getLang("Affiliate")?></th>
                                <th><?=$lang->getLang("First Name")?></th>
                                <th><?=$lang->getLang("Last Name")?></th>
                                <th><?=$lang->getLang("Registration Date")?></th>
                                <th><?=$lang->getLang("Jurisdiction Name")?></th>
                                <th><?=$lang->getLang("Affiliate Type")?></th>
                                <th><?=$lang->getLang("Operation")?></th>
                                <th>Reg</th>
                            </tr>
                            </thead>
                            <tbody>
                            </tbody>
                        </table>
                    </div>
            </td>
            <td style='vertical-align: top'>
                <div id="affiliatesInfoTree"><?=$lang->getLang("Affiliate information content")?></div>
            </td>
        </tr>
    </table>
</div>