<?php
/**
 *
 * @author Marian
 */
check_access('virtual_enable_games',true);
require_once('virtual_functions.inc');
include_once 'JurisdictionsList.class.inc';
$jurClass = JurisdictionsList::getInstance ( $dbh );
$hierarchy = $jurClass->getJurisdictionTree($_SESSION['jurisdiction_id']);

?>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
<script src="/media/jquery.dataTables.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<script src="/media/jstree/jstree.js" type="text/javascript"></script>
<script src="/media/jquery.alerts.js" type="text/javascript"></script>

<script type=text/javascript>
    $("body").on({
        ajaxStart: function() {
            $('#myModal').show();
        },
        ajaxStop: function() {
            $('#myModal').hide();
        }
    });
    $(function() {
        var to = false;
        $('#jurisdictionTreeSearch').keyup(function () {
            $("#jurisdictionTree").jstree('open_all');
            if(to) { clearTimeout(to); }
            to = setTimeout(function () {
                var v = $('#jurisdictionTreeSearch').val();
                $('#jurisdictionTree').jstree(true).search(v);
            }, 250);
        });


        $("#jurisdictionTree")
            .jstree({
                "plugins" : [
                    "search",
                    "types"
                ],
                "search":{
                    "fuzzy":false,
                    "show_only_matches":true
                },
                "types" : {
                    "casino" : {
                        "icon" : "../media/images/ca.png"
                    },
                    "nation" : {
                        "icon" : "../media/images/n.png"
                    },
                    "region" : {
                        "icon" : "../media/images/r.png"

                    },
                    "district" : {
                        "icon" :  "../media/images/d.png"

                    },
                    "club" : {
                        "icon": "../media/images/c.png"
                    }
                }
            }).bind("select_node.jstree",  function (e, data) {
                if(data.node.type=='club'){
                    $.post("/virtual/virtual_functions.inc",{action:"3",jurid:data.node.id}, function(data){
                        $("#virtualgames").empty().append( data );
                    });
                }
                else{
                    $("#virtualgames").empty().append( "<div class='alert alert-error'><?=$lang->getLang("Please select a")?> <b>club </b></div>" );
                }
            });
    });

</script>
<div class="modal" id="myModal" style="display: none;margin-top:15%">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-body centered" >
                Loading... <img src="media/images/ajax-loader.gif">
            </div>
        </div>
    </div>
</div>
<div class="container" style="position: absolute;left:0;right: 0;width: 100%" >
    <table class="table table-bordered">
        <tr>
            <td class="navbar-inner" colspan="2"><h3 class="text-center"><?=$lang->getLang("Manage Virtual Games")?></h3></td>
        </tr>
        <tr>
            <td class="span4">
                <div class="contentAdmin">
                    <div class="contentHeadAdmin"><?=$lang->getLang("Choose a club")?></div>
                    <div class="well">
                        <input type="text" id='jurisdictionTreeSearch' placeholder="<?=$lang->getLang("Search a jurisdiction")?>">
                        <div id="jurisdictionTree">
                            <ul>
                                <?=showChild($hierarchy);?>
                            </ul>
                        </div>
                    </div>
                </div>
            </td>
            <td style='vertical-align: top'>
                <div class="well">
                    <div id="virtualgames">

                    </div>
                </div>
            </td>
        </tr>
    </table>
</div>