<?php
?>
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree3/themes/default/style.css"  />
    <script src="<?= secure_host ?>/media/jstree3/jstree.min.js" type="text/javascript"></script>
<style>
    .jstree-default a.jstree-search { color:green; }
</style>
<script>
    $(function(){


     $tree=$('#jurisdictionTree').jstree({

        "plugins" : [
            "search",
            "types",
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
                    var selectOption = $('#status').val();
                    return {'id': node.id, 'action': 'getJurisdictions', 'status': selectOption};
                },
                'dataType':'json'
            }
        }
        /*,
        "search": {
            "show_only_matches" : true,
            'ajax': {
                'url': '/services/jurisdiction_services.inc?action=getJurisdictionParents',
                'dataType':'json'
            }
        }*/

    }).bind("select_node.jstree", function (e, data) {
            $('#jurisdictionTree').jstree('save_state');
        }) .on("activate_node.jstree", function(e,data){
            window.location.href = data.node.a_attr.href;
        })
        .on('ready.jstree', function (e, data) {
            <?
            $jurSelected = 1;
            if(isset($_GET['node']) ) {
                $jurSelected=getJurisdictionPathByIdAndClass($_REQUEST['node'],$_REQUEST['class']);
            ?>
             jurToSelect = <?=$jurSelected;?>;
             jurToOpen = <?=$jurSelected;?>;
            jurToOpen.shift();
            data.instance.set_state({ core : {
                'selected':  jurToSelect,
                open : jurToOpen

            } });
            <? } ?>
        });


      //  $("#searchJurisdictionButton").on('click', function(){

        $('#jurisdictionTreeSearch').keyup(function () {
            if(this.value.length>3){
                var start        = new Date().getTime();
                $.ajax({
                    url: "/services/jurisdiction_services.inc",
                    xhrFields: {
                        withCredentials: true
                    },
                    type: "GET",
                    dataType: "json",
                    data: {
                         action: "getJurisdictionsByName",
                         str: $('#jurisdictionTreeSearch').val()
                    }
                }).done(function (data) {
                        var res = "<ul>";
                        for (var jurisdictions in data) {

                            res += "<li><span class='title " + jurisdictions + "'>" + jurisdictions + "</span>";
                            res += "<ol class='" + jurisdictions + "'>";
                            for (var j = 0; j < data[jurisdictions].length; j++) {
                                res += "<li><span class='category'>" + jurisdictions + "</span> <a href='" + "<?=$_SERVER['PHP_SELF'] . "?node="?>" + data[jurisdictions][j]['id'] + "&class="+jurisdictions+"'>" + data[jurisdictions][j]['name'] + "</a> ";
                                res += "</li>";
                            }
                            res += "</ol>";
                            res += "</li>";
                        }
                        res += "</ul>";
                        var end = new Date().getTime();

                        res = "<span class='tip'><?=$lang->getLang("Time taken to execute your request")?> " + ((end-start)/1000) + " s</span><br/>" + res;
                        $('#searchResult').html(res);
                    })
                    .always(function () {
                    });
            }
            else if  (this.value.length==0){
                $('#searchResult').empty();
            }
        });
    });

    // make and ajax call on select status change to jurisdiction_services.inc
    $(document).on('change','#status',function(){
        //alert("PROBANDO");
        $.ajax({
            url: "/services/jurisdiction_services.inc",
            type: "GET",
            dataType: "json",
            data: {
                id: "",
                action: "getJurisdictions",
                status:  $('#status').val()
            },
            success: function(result) {

                // refresh the tree
                $tree.jstree("refresh");
            }
        });

    });
    
<?php 
$jurStatus = '1';
if (isset($_SESSION['jurisdiction_search_status'])) {
    $jurStatus = $_SESSION['jurisdiction_search_status'];
} else {
    $_SESSION['jurisdiction_search_status'] = $jurStatus;
} // end if - else

?>
</script>
<table>
<tr>
    <td><h4>Status:</h4></td>
    <td>
        <select id="status">
            <option value="2" <?php if ($jurStatus == '2') echo "selected='selected'"?>>All</option>
            <option value="1" <?php if ($jurStatus == '1') echo "selected='selected'"?>>Enabled</option>
        </select>
    </td>
</tr>
</table>

<input type="text" id='jurisdictionTreeSearch' style="float:left;padding: 5px;width: 94%" placeholder="<?=$lang->getLang("Search (min. 4 char)")?>">
<br/>

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


<div id="jurisdictionTree">
</div>

<?php

