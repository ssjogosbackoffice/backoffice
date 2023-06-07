/**
 * Created with JetBrains PhpStorm.
 * User: Marian
 * Date: 02/12/13
 * Time: 15.08
 * To change this template use File | Settings | File Templates.
 */
var $url = "/configuration/affiliates_services.php";
var ctIMTable = null;
var emptyMessID = null;
var intvID = null;
$(document).ready(function(){
    $( "#date_start" ).datepicker({
        dateFormat: 'yy-mm-dd'

    });
    $( "#date_end" ).datepicker({
        dateFormat: 'yy-mm-dd'
    });
    ctIMTable = ListTable.getInstance("affiliatesInfoTable");
    ctIMTable.setTarget($url);
    ctIMTable.defaultParams.aoColumns = [
        {"bVisible": false},
        null,
        null,
        null,
        null,
        null,
        null,
        null,
        {"bVisible": false}
    ];
    ctIMTable.create();

    /*$("#ppp").click(function(){
        $.ajax({
            url:$url,
            data: "action=payProcess",
            type: "POST",
            beforeSend: function(jqXHR, settings){
                $("#overlay").show();
            },
            success:function(data,textStatus,jqXHR){
                data = $.parseJSON(data);
                if(data.status == 1){
                    $('#category_mng').jAlert('Process Payed successfully', "success", "proc_succ_id");
                }else{
                    $('#category_mng').jAlert('Process not payed or arleady active', "fatal", "event_err_id");
                }
                $("#overlay").hide();
            }
        });
    });
*/




});

function updateCategories()
{
    ctIMTable.update();
}


function getAffiliateTree(aData)
{

    var affiliate = aData[0];
    $.ajax({
        url:$url,
        data: "action=getAffiliateTree&affiliate="+affiliate,
        type: "POST",
        success:function(data,textStatus,jqXHR){

            $("#affiliatesInfoTree").empty().html(data);
            $("#affiliatesInfoTree")
                .jstree({
                    "plugins" : [
                         "dnd", "search",
                        "state", "types", "wholerow"
                    ],
                    "ui": {
                       "initially_select": ["html_"+affiliate]
                    },


                "types" : {
                "default" : {
                    "max_depth" : -4,
                    "max_children" : -2

                },
                "folder" : {
                    "valid_children" : [ "default", "folder" ]
                },
                "drive" : {
                    "valid_children" : [ "default", "folder" ],
                    "icon" : {
                        "image" : "/static/v.1.0pre/_demo/root.png"
                    }

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
                "affiliate" : {
                    "icon": "../media/images/a.png"

                }
            }


    }).bind("loaded.jstree", function (event, data) {
                    // you get two params - event & data - check the core docs for a detailed description
                    $(this).jstree("open_all");
                })

        }
    });

    ctIMTable.update();
}





