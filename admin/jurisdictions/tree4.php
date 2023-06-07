<?php
include "JurisdictionsList.class.inc";
$jur_list = JurisdictionsList::getInstance($dbh);
$list = $jur_list->getChildJurisdictions($_SESSION['jurisdiction_id'],0);
$tree = $jur_list->getJurisdictionTree($_SESSION['jurisdiction_id'], 0);

?>
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree3/themes/default/style.css"  />
    <script src="<?= secure_host ?>/media/jstree3/jstree.min.js" type="text/javascript"></script>
<style>
    .jstree-default a.jstree-search { color:green; }
</style>
<script>
    $(function(){


    var $tree=$('#jurisdictionTree').jstree({

        "plugins" : [
            "search",
            "types",
            "state",
            "unique"
        ],
        "types": {
            "casino": {
                "icon": "../media/images/ca.png"
            }, "nation": {
                "icon": "../media/images/n.png"
            },
            "region": {
                "icon": "../media/images/r.png"
            },
            "district": {
                "icon": "../media/images/d.png"
            },
            "club": {
                "icon": "../media/images/c.png"
            }
        }

       /* ,'core' : {
            'data': {
                'url': '/services/jurisdiction_services.inc',
                'data': function (node) {
                    return {'id': node.id, 'action': 'getJurisdictions'};
                }
            }
        }*/

        ,'core' : {
            'data' : {
                'url' : function (node) {
                    return '/services/jurisdiction_services.inc';
                },
                'data' : function (node) {
                    return {'id': node.id, 'action': 'getJurisdictions'};
                },
                'dataType':'json'
            }
        },
        "search": {
            "show_only_matches" : true,
            'ajax': {
                'url': '/services/jurisdiction_services.inc?action=getJurisdictionParents',
                'dataType':'json'
            }
        }

    }).bind("select_node.jstree", function (e, data) {
            $('#jurisdictionTree').jstree('save_state');
        }) .on("activate_node.jstree", function(e,data){
            window.location.href = data.node.a_attr.href;
        });


        $("#searchJurisdictionButton").on('click', function(){
            if($('#jurisdictionTreeSearch').val().length>3){
                $tree.jstree(true).search($('#jurisdictionTreeSearch').val());
            }
            else{
                $tree.jstree("clear_search");
            }
        });
        /*$('#jurisdictionTreeSearch').keyup(function () {
            if(this.value.length>3){
                $tree.jstree(true).search($('#jurisdictionTreeSearch').val());
            }
            else{
                $tree.jstree("clear_search");
            }
        });*/
        $('#jurisdictionTreeSearch').keyup(function () {
            if(this.value.length==0){

                $tree.jstree("clear_search");
            }
        });

    });
</script>
<input type="text" id='jurisdictionTreeSearch' style="float:left;padding: 5px" placeholder="<?=$lang->getLang("Search (min. 4 char)")?>">&nbsp; <button class="small entitybtn" id="searchJurisdictionButton">Search</button> <br />
    <div id="searchResult"></div>
    <br/>
<ul class="legend">
        <li class="casino"><span class="tip"><span class="icon">C</span></span>Casino</li>
        <li class="nation"><span class="tip"><span class="icon">N</span></span>Nation</li>
        <li class="region"><span class="tip"><span class="icon">R</span></span>Region</li>
        <li class="district"><span class="tip"><span class="icon">D</span></span>District</li>
        <li class="club"><span class="tip"><span class="icon">C</span></span>Club</li>
</ul>
<br/>
<!--<div id="jurisdictionTree">
    <ul><?/*=buildJurisdictionsTree($tree);*/?></ul>
</div>-->

<div id="jurisdictionTree">
</div>