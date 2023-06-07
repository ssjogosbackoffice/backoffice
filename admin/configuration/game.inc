<?php
$internal=check_access('manage_games',$redirect=true);
include_once 'game_functions.inc';
require_once 'WebRequest.class.inc';
$jur =(isset($_GET['jur'])?$_GET['jur']: (isset($_REQUEST['jur']) ? substr($_REQUEST['jur'],1) : $_SESSION['jurisdiction_id'] ));
$cls = (isset($_REQUEST['cls']) ? $_REQUEST['cls'] : $_SESSION['jurisdiction_class'] );
$jurisdiction_id = (isset($_REQUEST['jurisdictions']) ? $_REQUEST['jurisdictions'] : $_SESSION['jurisdiction_id']);
$jurisdiction_class = (isset($_REQUEST['jurisdictionsType']) ? $_REQUEST['jurisdictionsType'] : $_SESSION['jurisdiction_class']);
$date_start = date("Y-m-d H:i");
$from = date("Y-m-d 00:00",strtotime("-1 month"));
$to = date("Y-m-d 23:59");

function addGameCurlRequest($url, $gameId) {
    $data="operationcode=2011&op=1&sername=game&auxid=$gameId";
    $webReq = new WebRequest($url);
    $webReq->setOptions($data);
    $webReq->sendRequest();
    return $webReq->getResponse();
}

function editGameCurlRequest($url, $gameId) {
    $data="operationcode=2011&op=0&sername=game&auxid=$gameId";
    $webReq = new WebRequest($url);
    $webReq->setOptions($data);
    $webReq->sendRequest();
    return  $webReq->getResponse();
}
function gameList($default='') {
    global $dbh,$lang;
    $sql = "Select * from game WHERE gam_id > 1000";
    $result = $dbh->doCachedQuery ( $sql, 0 );
    $return =" <select name='t_id' id='t_id'>";
    while ($result->hasNext()){
        $row = $result->next();
        $return.="<option style='width:206px;' value='".$row["gam_id"]."'";
        if($default==$row['gam_id']){
            $return .=" selected='selected' ";
        }
        $return.=">".$row["gam_name"]." [".$row["gam_id"]."]</option>";
    }
    $return.="</select>";
    return $return;
}
/**
 * @author Marian, @maintainer Alexandra
 */

if (isset($_POST['section'])){
if($_POST['section']=='internal'){
    $name = $_POST ['name'];
    $id= $_POST ['id'];
    $minbet=  $_POST ['minbet'];
    $maxbet= $_POST ['maxbet'];
    $maxpayout = $_POST ['maxpayout'];
    $minposbet= $_POST ['minposbet'];
    $maxposbet= $_POST ['maxposbet'];
    $smallblind= $_POST ['smallblind'];
    $bigblind= $_POST ['bigblind'];
    $extcode= $_POST ['extcode'];
    $category= $_POST ['category'];
    $type= $_POST ['type'];
    $localization= $_POST['localization'];
    $adminhistoryclass= $_POST['adminhistoryclass'];
    $iscommondraw=($_POST['iscommondraw']=='1'?'1':'0');
    $clientType = $_POST['clientType'];
    $isprivate = $_POST['private'];
    $isVideo = $_POST ['isVideo'];
    $videoTime = $_POST ['dealerTime'];
    $videoType = $_POST['videoType'];
    $videoData = NULL;
    $gameVersion = $_POST['gameVersion'];

    $prev_params = $_POST['prev_params']; //==========================================================
    if (isset ( $_POST ['operationtype'] ) && $_POST['operationtype']==$lang->getLang("Add")) {
        $prev_params = '';
    }
    if($isVideo == "true"){
        $videoData = "isvideo:true";
        if ($videoTime != "") {
            $videoData .="~dealer_timer:".$videoTime;
        }
        if($videoType != "") {
            $videoData .="~type:". $videoType;
        }
    } elseif($isVideo == "false"){
        $videoData = "isvideo:false";
        if ($videoTime != "") {
            $videoData .="~dealer_timer:".$videoTime;
        }
        if($videoType != "") {
            $videoData .="~type:". $videoType;
        }
    } elseif ($videoTime != "") {
        $videoData .="dealer_timer:". $videoTime;
        if($videoType != "") {
            $videoData .="~type:". $videoType;
        }
    } elseif ($videoType != ""){
        $videoData .="type:".$videoType;
    } else {
        $videoData = NULL;
    }

    if ($prev_params != "") {
        if($videoData != null) {
            $videoData = $prev_params . "~" . $videoData;
        } else {
            $videoData = $prev_params;
        }
    }

    if (isset ( $_POST ['operationtype'] ) && $_POST['operationtype']==$lang->getLang("Add")) {
        $response = addGame($id,$name,$minbet,$maxbet,$maxpayout,$minposbet,$maxposbet,$smallblind,$bigblind,$extcode,$category,$type,$localization,$adminhistoryclass,$iscommondraw,$videoData,$clientType, $gameVersion, $isprivate);

        if($response ==  '1') {

            $url = $_SEREVER['WEB_SERVICES'];
            $serverResponse = addGameCurlRequest($url, $id);
            //send request to server url
            echo$response;
            die();
        }
        else {
            echo$response;
            die();
        }
    } elseif  (isset ($_POST ['operationtype']) && $_POST['operationtype'] == $lang->getLang("Update")) {
        $update = updateGame($id, $name, $minbet, $maxbet, $maxpayout, $minposbet, $maxposbet, $smallblind, $bigblind, $extcode, $category, $type, $localization, $adminhistoryclass, $iscommondraw, $videoData, $clientType, $gameVersion, $isprivate);

        if($update ==  '1') {
            $url = $_SERVER['WEB_SERVICES'];
            $serverResponse = editGameCurlRequest($url, $id);
            //send request to server url
        }
        echo $update;
        die();

    }
}elseif ($_POST['section']=='general') {
    $category=$_POST['category'];
    $bimage=$_POST['bimage'];
    $ggid=$_POST['ggid'];
    $gwidth=$_POST['gwidth'];
    $gheight=$_POST['gheight'];
    $gpes=$_POST['gpes'];
    $playforfun=(($_POST['playforfun']==true && $_POST['playforfun']!='false')?1:0);
    $webname=$_POST['webname'];
    if (isset ( $_POST ['operationtype'] ) && $_POST['operationtype']==$lang->getLang("Add") ) {
        unset($_SESSION['allGames']);
        die(addNewGeneralGame($category,$bimage,$ggid,$gwidth,$gheight,$gpes,$playforfun,$webname));

    } elseif  (isset ($_POST ['operationtype']) && $_POST['operationtype'] == $lang->getLang("Update")) {
        $id=$_POST['id'];
        unset($_SESSION['allGames']);
        $update = updateGeneralGames($id, $category, $bimage, $ggid, $gwidth, $gheight, $gpes, $playforfun, $webname);

        echo $update ;
        die();
    }
}elseif ($_POST['section']=='tournament_game') {
    $id=$_POST['id'];
    $gam_id = $_POST['t_id'];
    $name=$_POST['t_name'];
    $description=$_POST['t_description'];
    $start_time=$_POST['t_start_time'];
    $start_hour=$_POST['t_start_hour'];
    $start_minute=$_POST['t_start_minute'];
    $min_in=$_POST['t_min_in'];
    $max_in=$_POST['t_max_in'];
    $amount=$_POST['t_amount'];
    $duration=$_POST['t_duration'];
    $bstart=$_POST['t_bstart'];
    $astart=$_POST['t_astart'];
    $status=$_POST['t_status'];
    if (isset ( $_POST ['operationtype'] ) && $_POST['operationtype']==$lang->getLang("Add") ) {
        die(addNewTournamentGame($gam_id,$name,$description,$start_time,$start_hour,$start_minute,$min_in,$max_in,$amount,$duration,$bstart,$astart,$status));
    }elseif  (isset ($_POST ['operationtype']) && $_POST['operationtype'] == $lang->getLang("Update")) {
        $update = updateTournamentGame($id,$gam_id,$name,$description,$start_time,$start_hour,$start_minute,$min_in,$max_in,$amount,$duration,$bstart,$astart,$status);
        echo $update ;
        die();
    }
}


/*if ($_POST ['updel'] == '2') {
    $id = $_POST ['id'];
    delGame( $id );
    */?><!--
        <script>
            $(function() {
                jAlert('<?/*=$lang->getLang("You have successully deleted a game")*/?>');
            });
        </script>
    --><?php
/*    }
    */?>
<? } ?>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap-responsive.css" title="core_style" />
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<script src="/media/jquery.dataTables.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
<script src="<?= secure_host ?>/media/jstree3/jstree.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="/media/js/multiselect/css/multi-select.css" title="core_style">
<script src="/media/js/multiselect/js/jquery.multi-select.js" type="text/javascript"></script>
<script src="/media/jquery.quicksearch.js" type="text/javascript"></script>
<style>.ms-container .ms-list {height: 500px}</style>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/select2/select2-4/css/select2.css" >
<script src="<?= secure_host ?>/media/select2/select2-4/js/select2.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/js/sweetalert/sweetalert.css"  />
<script src="<?= secure_host ?>/media/js/sweetalert/sweetalert.min.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/ui/js/jquery.timepicker.js" type="text/javascript"></script>
<script type="text/javascript">
    $("body").on({
        ajaxStart: function() {
            $('#loadingModal').show();
        },
        ajaxStop: function() {
            $('#loadingModal').hide();
        },
        ajaxError:function(){
            $('#myModal').hide();
        }
    });

    $(function() {

        $('.skinMenuButton').on('click',function(){
            getMenu($(this).data("type"));
        });
        $('#skinMenuButton').on('click',function(){
            $('#skinMenu .active>a').trigger('click');
        });

        $('.skinGamesButton').on('click',function(){
            getSkinProviders($(this).data("type"));
        });
        $('#skinGamesButton').on('click',function(){
            $('#skinGames .active>a').trigger('click');
        });

        $('#skinSettingsDesktopButton, #skinSettingsMobileButton').on('click',function(){
            getSkinSettings($(this).data("type"));
        });
        $('#skinSettingsButton').on('click',function(){
            $('#skinSettings .active>a').trigger('click');
        });

        $(document).on('click','.updateMenu',function(){
            updateMenu($(this).data('type'),$(this).data('children'));
        });

        $(document).on('click','.jstree-anchor',function(){
            $('#subsectionSelector>.active>a').trigger('click');
        });


        $(document).on('click','.updateSkinConfiguration',function(){
            updateSkinConfiguration($(this).data('type'),$(this).data('children'));
        });


        $tree=$('#jurisdictionTree').jstree({

            "plugins" : [
                "search",
                "types",
                "unique",
                "state"
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
            ,'core' : {
                'data' : {
                    'url' : function (node) {
                        return '/services/jurisdiction_services.inc';
                    },
                    'data' : function (node) {
                        return {'id': node.id,'skinid':$("[name='skin']").val(), 'action': 'getJurisdictions',parent:true};
                    },
                    'dataType':'json'
                }
            }
        }).on('ready.jstree', function (e, data) {
            <?
            if($jurisdiction_class!='casino') {
                $jurSelected = getJurisdictionPathByIdAndClass($jurisdiction_id, $jurisdiction_class);
            }
            else{
                $jurSelected=json_encode(array("n1"));
            }
            ?>
            jurToSelect = <?=$jurSelected;?>;
            jurToOpen = <?=$jurSelected;?>;
            jurToOpen.shift();
            data.instance.clear_state();
            data.instance.set_state({ core : {
                selected:  [jurToSelect[0]],
                open : jurToOpen
            } });
            <?
            if(isset($_REQUEST['section']) && $_REQUEST['section']=='structures' ){?>

            $('a[href="#<?=$_REQUEST['section']?>"]').trigger('click');

            <?}?>

        }).on( "select_node.jstree", function(evt, data){
            $('#skinGamesButton').trigger('click');
        });

        $('#jurisdictionTreeSearch').keyup(function () {
            if(this.value.length>3){
                var start = new Date().getTime();
                var subsection = "";
                if($('#skinMenuDesktop').hasClass('active')) {
                    subsection = 'skinMenu';
                }
                if($('#skinGames').hasClass('active')) {
                    subsection = 'skinGames';
                }
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
                            res += "<li><span class='category'>" + jurisdictions + "</span> <a href='/games_management?section=structures&subsection="+subsection+"&jurisdictions=" + data[jurisdictions][j]['id'] + "&jurisdictionsType="+jurisdictions+"'>" + data[jurisdictions][j]['name'] + "</a> ";
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

        getInternalGames();
        getGeneralGames();
        getTournamentGames()
        $("#skinButtonFilter").on('click', function() {
            $tree.jstree(true).refresh();
        });

        global_params = '';
        $(document).on('click',".updateTournamentGame", function() {
            var game = $(this).closest('tr');
            var start_hour = $.trim($(game).find('td:eq(4)').text()).split(':');
            if(start_hour.length > 1) {
                $("#t_start_hour").val(start_hour[0]).change();
                $("#t_start_minute").val(start_hour[1]).change();
            }
            $('#update_id').val(game.data('entry'));
            $('#addTournament_btn').html('<?=$lang->getLang("Update")?>');
            $('#tournamentModal #modalHeader').html('<?=$lang->getLang("Update Tournament")?>');
            $("#t_id").val($.trim($(game).find('td:eq(0)').data('game'))).change().prop('disabled',true);
            $("#t_name").val($.trim($(game).find('td:eq(1)').text()));
            $("#t_description").val($.trim($(game).find('td:eq(2)').text()));
            $("#t_start_time").val($.trim($(game).find('td:eq(3)').text()));

            $("#t_min_in").val($.trim($(game).find('td:eq(5)').text()));
            $("#t_max_in").val($.trim($(game).find('td:eq(6)').text()));
            $("#t_amount").val($.trim($(game).find('td:eq(7) .cAmount').text().replace(/,/g, '.')));
            $("#t_duration").val($.trim($(game).find('td:eq(8)').text()));
            $("#t_bstart").val($.trim($(game).find('td:eq(9)').text()));
            $("#t_astart").val($.trim($(game).find('td:eq(10)').text()));
            $("#t_status option").filter(function() {
                return $(this).text() == $.trim($(game).find('td:eq(11)').text());
            }).prop('selected', true);
            $("#t_status").prop('disabled', false);
        });

        $(document).on('click',".showAddtournamentModal", function() {
            $('#update_id').val('');
            $('#addTournament_btn').html('<?=$lang->getLang("Add")?>');
            $('#tournamentModal #modalHeader').html('<?=$lang->getLang("Add tournament")?>');
            $("#t_id").val('').change().prop('disabled',false);
            $("#t_name").val('');
            $("#t_description").val('');
            $("#t_start_time").val('<?=$date_start?>');
            $("#t_start_hour").val('00').change();
            $("#t_start_minute").val('00').change();
            $("#t_min_in").val(0);
            $("#t_max_in").val(0);
            $("#t_amount").val(0);
            $("#t_duration").val(0);
            $("#t_bstart").val(0);
            $("#t_astart").val(0);
            $("#t_status").val(1).change().prop('disabled', true);

        });

        $(document).on('click',".updateGame", function() {
            var game = $(this).closest('tr');

            $('#addGame').html('<?=$lang->getLang("Update")?>');
            $('#modalHeader').html('<?=$lang->getLang("Update game")?>');
            $("#id").val($.trim($(game).find('td:eq(0)').text())).prop('disabled',true);
            $("#name").val($.trim($(game).find('td:eq(1)').text()));
            $("#minbet").val($.trim($(game).find('td:eq(2)').text()));
            $("#maxbet").val($.trim($(game).find('td:eq(3)').text()));
            $("#maxpayout").val($.trim($(game).find('td:eq(4)').text()));
            $("#minposbet").val($.trim($(game).find('td:eq(5)').text()));
            $("#maxposbet").val($.trim($(game).find('td:eq(6)').text()));
            $("#smallblind").val($.trim($(game).find('td:eq(7)').text()));
            $("#bigblind").val($.trim($(game).find('td:eq(8)').text()));
            $("#extcode").val($.trim($(game).find('td:eq(9)').text()));
            $("#clientType").val($.trim($(game).find('td:eq(16)').text()));
            $("#gameVersion").val($.trim($(game).find('td:eq(17)').text()));
            $("#private").val(($.trim($(game).find('td:eq(18)').text()) == 'True'?1:0));
            $("#category option").filter(function() {
                return $(this).text() == $.trim($(game).find('td:eq(10)').text());
            }).prop('selected', true);
            $("#type option").filter(function() {
                return $(this).text() == $.trim($(game).find('td:eq(11)').text());
            }).prop('selected', true);

            $("#localization").val($.trim($(game).find('td:eq(12)').text()));
            $("#adminhistoryclass").val($.trim($(game).find('td:eq(13)').text()));
            $("#iscommondraw").prop('checked', $.trim($(game).find('td:eq(14)').text())=='Yes'?true:false);

            var params = game.find('td:eq(15)').text();
            if (params == "") {
                $('#moreParams').attr('checked', false);
                $("#isVideo").val('');
                $('#dealerTime').val('');
                $('#videoType').val('');
                $('.hidden-video').hide();
            }
            else {
                $('.hidden-video').show();
                $('#moreParams').attr('checked', true);
                var result = params.split("~");

                var array_to_save = [];

                $.each(result, function (index,item) {
                    if (item.indexOf(":") != -1) {
                        array_to_save.push(item);
                    } else {
                        global_params += item + '~';
                    }
                });

                global_params = global_params.slice(0,-1);

                var paramLength = array_to_save.length;

                if(paramLength == 1) {
                    var myResult = result[0].split(":");
                    if (myResult[0] == "isvideo") {
                        $('#isVideo').val(myResult[1]);
                        $('#dealerTime').val("");
                        $('#videoType').val("");
                    } else if (myResult[0] == "dealer_timer") {
                        $('#isVideo').val("");
                        $('#dealerTime').val(myResult[1]);
                        $('#videoType').val("");
                    } else if (myResult[0] == "dealer_timer") {
                        $('#isVideo').val("");
                        $('#dealerTime').val("");
                        $('#videoType').val(myResult[1]);
                    }

                }else if (paramLength == 2) {
                    var firstResult = array_to_save[0].split(":");
                    var secondResult = array_to_save[1].split(":");
                    if (firstResult[0] == "isvideo" ) {
                        $('#isVideo').val(firstResult[1]);
                        if (secondResult[0]  == "dealer_timer") {
                            $('#dealerTime').val(secondResult[1]);
                            $('#videoType').val("");
                        }else if (secondResult[0] =="type") {
                            $('#videoType').val(secondResult[1]);
                        }
                    } else if (firstResult[0] == "dealer_timer") {
                        $('#isVideo').val("");
                        $('#dealerTime').val(firstResult[1]);
                        $('#videoType').val(secondResult[1]);
                    }
                }
                else if (paramLength == 3) {
                    var firstResult = array_to_save[0].split(":");
                    $('#isVideo').val(firstResult[1]);
                    var secondResult = array_to_save[1].split(":");
                    $('#dealerTime').val(secondResult[1]);
                    var thirdResult = array_to_save[2].split(":");
                    $('#videoType').val(thirdResult[1]);
                }

            }

        });

        $(document).on('click',".updateGeneralGame", function() {
            var game = $(this).closest('tr');
            $('#addGeneralGame').html('<?=$lang->getLang("Update")?>');
            $('#modalGeneralHeader').html('<?=$lang->getLang("Update game")?>');
            $('#generalGameId').val($.trim($(game).find('td:eq(0)').text()));
            $("#gCategory option").filter(function() {
                return $(this).val() == $.trim($(game).find('td:eq(1)').text());
            }).prop('selected', true);
            $("#gButtonImage").val($.trim($(game).find('td:eq(2)').text()));
            $("#gGID").val($.trim($(game).find('td:eq(3)').text()));
            $("#gWidth").val($.trim($(game).find('td:eq(4)').text()));
            $("#gHeight").val($.trim($(game).find('td:eq(5)').text()));
            $("#gPes").val($.trim($(game).find('td:eq(6)').text()));
            $("#gPlayForFun").prop('checked', $.trim($(game).find('td:eq(7)').text())=='Yes'?true:false);
            $("#gWebName").val($.trim($(game).find('td:eq(8)').text()));

        });

        $(document).on('click',".showAddGeneralModal", function() {
            $('#addGeneralGame').html('<?=$lang->getLang("Add")?>');
            $('#modalGeneralHeader').html('<?=$lang->getLang("Add game")?>');
            $('#generalGameId').val('');
            $("#gButtonImage").val('');
            $("#gGID").val('');
            $("#gWidth").val(905);
            $("#gHeight").val(640);
            $("#gPes").val('');
            $("#gPlayForFun").prop('checked', false);
            $("#gWebName").val('');

        });

        $(document).on('click',".showAddModal", function() {
            $('#addGame').html('<?=$lang->getLang("Add")?>');
            $('#modalHeader').html('<?=$lang->getLang("Add a new game")?>');
            $("#id").val('').prop('disabled',false);
            $("#name").val('');
            $("#minbet").val(0);
            $("#maxbet").val(100);
            $("#maxpayout").val(100);
            $("#minposbet").val(0);
            $("#maxposbet").val(100);
            $("#smallblind").val(0);
            $("#bigblind").val(100);
            $("#extcode").val('');
            $("#localization").val('');
            $("#adminhistoryclass").val('');
            $("#iscommondraw").prop('checked', false);
            $("#moreParams").prop('checked', false);
            $("#isVideo").val('');
            $('#dealerTime').val('');
            $('#videoType').val('');
            $('#gameVersion').val('');
            $('#private').val('');
            $('.hidden-video').hide();
        });

        $(document).on('click',".updateSkinGames", function() {
            var jurisdiction=($tree.jstree(true).get_selected(true));
            updateSkinGames($(this).data('type'),$(this).data('children'));
        });
    });

    $(document).on('click', "#moreParams", function(){
        if( $(this).is(':checked')) {
            $('.hidden-video').show();
        }
        else {
            $("#moreParams").prop('checked', false);
            $("#isVideo").val('');
            $('#dealerTime').val(''),
                $('#videoType').val(''),
                $('.hidden-video').hide();
        }
    });

    $(document).ready(function () {
        //called when key is pressed in textbox
        $("#dealerTime").keypress(function (e) {
            //if the letter is not digit don't type anything
            if ((e.which >= 48 && e.which <= 57) || (e.which >= 65 && e.which <= 90) || e.which == 95 || (e.which >= 97 && e.which <= 122) ) {
            } else {
                return false;
            }
            // if (e.which != 8 && e.which != 0 && (e.which < 48 || (e.which > 57 && e.which < 65) || (e.which > 90 && e.which < 95) || e.which > 95)) {
            //     return false;
            // }
        });

        (function($) {
            $.fn.inputFilter = function(inputFilter,min,max) {
                return this.on("input keydown keyup mousedown mouseup select contextmenu drop", function() {
                    if (inputFilter(this.value)) {
                        this.oldValue = this.value;
                        this.oldSelectionStart = this.selectionStart;
                        this.oldSelectionEnd = this.selectionEnd;
                    } else if (this.hasOwnProperty("oldValue")) {
                        this.value = this.oldValue;
                        this.setSelectionRange(this.oldSelectionStart, this.oldSelectionEnd);

                    }
                });
            };
        }(jQuery));

        //        Integer values (both positive and negative):
        //        /^-?\d*$/.test(value)
        //        Integer values (positive only):
        //        /^\d*$/.test(value)
        //        Integer values (positive and up to a particular limit):
        //        /^\d*$/.test(value) && (value === "" || parseInt(value) <= 500)
        //        Floating point values (allowing both . and , as decimal separator):
        //        /^-?\d*[.,]?\d*$/.test(value)
        //        Currency values (i.e. at most two decimal places):
        //        /^-?\d*[.,]?\d{0,2}$/.test(value)
        //        Currency values positive only(i.e. at most two decimal places):
        //        /^\d*[.,]?\d{0,2}$/.test(value)
        //        Hexadecimal values:
        //        /^[0-9a-f]*$/i.test(value)
        //        Alphanumeric no injectino:
        //        /^[0-9a-zA-Za-åa-ö-w-я-`/() ?&%:#=+]*$/i.test(value)

        $('.inputnumberdec').inputFilter(function (value) {
            return /^\d*[.]?\d{0,2}$/.test(value);
        });
        $('.inputnumber').inputFilter(function (value) {
            return /^\d*$/.test(value);
        });
    });

    function addTournament() {
        var id = $('#update_id').val();
        var t_id = $('#t_id').val();
        var t_name = $('#t_name').val();
        var t_description = $('#t_description').val();
        var t_start_time = $('#t_start_time').val();
        var t_start_hour = $('#t_start_hour').val();
        var t_start_minute = $('#t_start_minute').val();
        var t_min_in = $('#t_min_in').val();
        var t_max_in = $('#t_max_in').val();
        var t_amount = $('#t_amount').val();
        var t_duration = $('#t_duration').val();
        var t_bstart = $('#t_bstart').val();
        var t_astart = $('#t_astart').val();
        var t_status = $('#t_status').val();

        if (t_name == '') {
            $.jAlert({
                'title': 'Error',
                'content': 'Invalid Name',
                'theme': 'red',
                'btns': {'text':'Ok', 'theme': 'red'}
            });
        }else if (t_min_in <= 0) {
            $.jAlert({
                'title': 'Error',
                'content': 'Invalid Min inscriptions',
                'theme': 'red',
                'btns': {'text':'Ok', 'theme': 'red'}
            });
        }else if (t_max_in <= 0) {
            $.jAlert({
                'title': 'Error',
                'content': 'Invalid Max inscriptions',
                'theme': 'red',
                'btns': {'text':'Ok', 'theme': 'red'}
            });
        }else if (t_duration <= 0) {
            $.jAlert({
                'title': 'Error',
                'content': 'Invalid Duration',
                'theme': 'red',
                'btns': {'text':'Ok', 'theme': 'red'}
            });
        }else if (t_bstart <= 0) {
            $.jAlert({
                'title': 'Error',
                'content': 'Invalid Time wait bstart',
                'theme': 'red',
                'btns': {'text':'Ok', 'theme': 'red'}
            });
        }else if (t_astart <= 0) {
            $.jAlert({
                'title': 'Error',
                'content': 'Invalid Time wait astart',
                'theme': 'red',
                'btns': {'text':'Ok', 'theme': 'red'}
            });
        } else {
            $('#tournamentModal').modal("hide");
            $.post("configuration/game.inc",{
                section:'tournament_game',
                operationtype:$('#addTournament_btn').text(),
                id:id,
                t_id:t_id,
                t_name:t_name,
                t_description:t_description,
                t_start_time:t_start_time,
                t_start_hour:t_start_hour,
                t_start_minute:t_start_minute,
                t_min_in:t_min_in,
                t_max_in:t_max_in,
                t_amount:t_amount,
                t_duration:t_duration,
                t_bstart:t_bstart,
                t_astart:t_astart,
                t_status:t_status,
            }, function(data){
                data = data.replace(/\s/g,'');
                if(data=='1'){
                    console.log('close modal');
                    $.jAlert({
                        'title': 'Success',
                        'content': 'Operation successful!',
                        'theme': 'green',
                        'btns': {'text':'Ok', 'theme': 'green'}
                    });
                    getTournamentGames();

                    return false;
                }
                else if(data=='2')
                {
                    $.jAlert({
                        'title': 'Error',
                        'content': 'Game already exists!',
                        'theme': 'red',
                        'btns': {'text':'Ok', 'theme': 'red'}
                    });
                    return false;
                }
                else
                {
                    $.jAlert({
                        'title': 'Error',
                        'content': 'An error has occurred. Please try again!',
                        'theme': 'red',
                        'btns': {'text':'Ok', 'theme': 'red'}
                    });
                    return false;
                }
            });
        }

    }


    function addGame(){

        //set the input fields to their values

        var videoTime = $('#dealerTime').val();
        if (videoTime != "") {
            var videoTimeCheck = parseInt(videoTime);
            if (videoTimeCheck > 600 || videoTimeCheck <= 0) {
                $.jAlert({
                    'title': 'Error',
                    'content': 'Invalid param minutes, max 600 allowed',
                    'theme': 'red',
                    'btns': {'text':'Ok', 'theme': 'red'}
                });
            } else {
                $('#myModal').modal("hide");
                $.post("configuration/game.inc",{
                    section:'internal',
                    operationtype:$('#addGame').text(),
                    id:$('#id').val(),
                    name:$('#name').val(),
                    minbet:$('#minbet').val(),
                    maxbet:$('#maxbet').val(),
                    maxpayout:$('#maxpayout').val(),
                    minposbet:$('#minposbet').val(),
                    maxposbet:$('#maxposbet').val(),
                    smallblind:$('#smallblind').val(),
                    bigblind:$('#bigblind').val(),
                    extcode:$('#extcode').val(),
                    category:$('#category').val(),
                    type:$('#type').val(),
                    localization:$('#localization').val(),
                    adminhistoryclass:$('#adminhistoryclass').val(),
                    iscommondraw:($('#iscommondraw').is(':checked')?'1':'0'),
                    clientType : $('#clientType').val(),
                    gameVersion : $("#gameVersion").val(),
                    private : $('#private').val(),
                    isVideo:$('#isVideo').val(),
                    dealerTime:$('#dealerTime').val(),
                    videoType:$('#videoType').val(),
                    prev_params: global_params
                }, function(data){
                    if(data=='1'){
                        console.log('close modal');
                        $.jAlert({
                            'title': 'Success',
                            'content': 'Operation successful!',
                            'theme': 'green',
                            'btns': {'text':'Ok', 'theme': 'green'}
                        });
                        getInternalGames();
                        $("#isVideo").val();
                        $('#dealerTime').val();
                        $('#videoType').val();

                        return false;
                    }
                    else if(data=='2')
                    {
                        $.jAlert({
                            'title': 'Error',
                            'content': 'Game already exists!',
                            'theme': 'red',
                            'btns': {'text':'Ok', 'theme': 'red'}
                        });
                        return false;
                    }
                    else
                    {
                        $.jAlert({
                            'title': 'Error',
                            'content': 'An error has occurred. Please try again!',
                            'theme': 'red',
                            'btns': {'text':'Ok', 'theme': 'red'}
                        });
                        return false;
                    }
                });
            }
        } else {
            $.post("configuration/game.inc",{
                section:'internal',
                operationtype:$('#addGame').text(),
                id:$('#id').val(),
                name:$('#name').val(),
                minbet:$('#minbet').val(),
                maxbet:$('#maxbet').val(),
                maxpayout:$('#maxpayout').val(),
                minposbet:$('#minposbet').val(),
                maxposbet:$('#maxposbet').val(),
                smallblind:$('#smallblind').val(),
                bigblind:$('#bigblind').val(),
                extcode:$('#extcode').val(),
                category:$('#category').val(),
                type:$('#type').val(),
                localization:$('#localization').val(),
                adminhistoryclass:$('#adminhistoryclass').val(),
                iscommondraw:($('#iscommondraw').is(':checked')?'1':'0'),
                clientType : $('#clientType').val(),
                isVideo:$('#isVideo').val(),
                dealerTime:$('#dealerTime').val(),
                videoType:$('#videoType').val(),
                gameVersion : $("#gameVersion").val(),
                private : $('#private').val(),
            }, function(data){
                if(data=='1'){
                    $.jAlert({
                        'title': 'Success',
                        'content': 'Operation successful!',
                        'theme': 'green',
                        'btns': {'text':'Ok', 'theme': 'green'}
                    });
                    getInternalGames();
                    $("#isVideo").val();
                    $('#dealerTime').val();
                    $('#videoType').val();
                    return false;
                }
                else if(data=='2')
                {
                    $.jAlert({
                        'title': 'Error',
                        'content': 'Game already exists!',
                        'theme': 'red',
                        'btns': {'text':'Ok', 'theme': 'red'}
                    });
                    return false;
                }
                else
                {
                    $.jAlert({
                        'title': 'Error',
                        'content': 'An error has occurred. Please try again!',
                        'theme': 'red',
                        'btns': {'text':'Ok', 'theme': 'red'}
                    });
                    return false;
                }
            });
        }
        $('#myModal').toggle();
    }

    function addNewGeneralGame(){
        $.post("configuration/game.inc",{
            section:'general',
            operationtype:$('#addGeneralGame').text(),
            id:$('#generalGameId').val(),
            category:$('#gCategory').val(),
            bimage:$('#gButtonImage').val(),
            ggid:$('#gGID').val(),
            gwidth:$('#gWidth').val(),
            gheight:$('#gHeight').val(),
            gpes:$('#gPes').val(),
            playforfun:$('#gPlayForFun').is(':checked'),
            webname:$('#gWebName').val(),
        }, function(data){
            if(data=='1'){
                $.jAlert({
                    'title': 'Success',
                    'content': 'Operation successful!',
                    'theme': 'green',
                    'btns': {'text':'Ok', 'theme': 'green'}
                });
                getGeneralGames();
                return false;

            }
            else if(data=='2')
            {
                $.jAlert({
                    'title': 'Error',
                    'content': 'Game already exists!',
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
        });
    }

    function getInternalGames(){
        $.ajax({
            url: '/services/general_services.inc',
            method: "POST",
            data: {
                action:'getGames'
            }
        }) .done(function(data) {
            $('#internalGamesContainer').empty().append(data);
            $("#internalGamesContainer>.display").dataTable({'sPaginationType':"full_numbers"});

        })
            .fail(function(error) {
                console.log(error);
            });
    }

    function getTournamentGames(){
        var from = $('#filter_from').val();
        var until = $('#filter_to').val();
        var status = $('#filter_t_status').val();

        $.ajax({
            url: '/services/general_services.inc',
            method: "POST",
            data: {
                action:'getTournament',
                from:from,
                until:until,
                status:status,
            }
        }).done(function(data) {
            $('#tournamentGamesContainer').empty().append(data);
            $("#tournamentGamesContainer>.display").dataTable({
                'sPaginationType':"full_numbers",
                'paging': true,
                'pageLength': '100',
                "lengthMenu": [[10, 25, 50, 100], [10, 25, 50, 100]],
            });
        }).fail(function(error) {
            console.log(error);
        });
    }

    function getGeneralGames(){
        $.ajax({
            url: '/services/general_services.inc',
            method: "POST",
            data: {
                action:'getGeneralGames'
            }
        }) .done(function(data) {
            $('#generalGamesContainer').empty().append(data);
            $("#generalGamesContainer>.display").dataTable({'sPaginationType':"full_numbers"});

        })
            .fail(function(error) {
                console.log(error);
            });
    }
    function getMenu(type){
        type = typeof type !== 'undefined' ? type : 'Desktop';

        var jurisdiction=($tree.jstree(true).get_selected(true));
        console.log('jurisdiction');
        console.log(jurisdiction);
        if(jurisdiction==''){
            $.jAlert({
                'title': 'Error',
                'content': "Please select at least one jurisdiction 3",
                'theme': 'red',
                'btns': {'text':'Ok', 'theme': 'red'}
            });
        } else {
            $.ajax({
                url: '/services/general_services.inc',
                method: "POST",
                data: {
                    action: 'getSkinMenu',
                    jurisdiction: jurisdiction[0].id.substring(1),
                    type:type
                }
            }).done(function (data) {
                $('.skinMenuContent').empty();
                $('#skinMenuContent'+type).empty().append(data);

            })
                .fail(function (error) {
                    console.log(error);
                });
        }
    }
    function updateMenu(type,underLvl){
        type = typeof type !== 'undefined' ? type : 'Desktop';
        var jurisdiction=($tree.jstree(true).get_selected(true));
        if(jurisdiction==''){
            $.jAlert({
                'title': 'Error',
                'content': "Please select at least one jurisdiction 4",
                'theme': 'red',
                'btns': {'text':'Ok', 'theme': 'red'}
            });
        }
        else {
            var menu=[];
            $('#tableMenu').find('tbody tr').each(function(){
                var section=$(this).data('section');
                var curr={};
                if($(this).find('.yes').is(":checked")) {
                    curr['name'] = section, curr["id"] = $(this).find('.sectionName').val();
                    menu.push(curr);
                }
            });
            $.ajax({
                url: '/services/general_services.inc',
                method: "POST",
                data: {
                    action: 'updateSkinMenu',
                    menu: JSON.stringify(menu),
                    jurisdiction: jurisdiction[0].id.substring(1),
                    underLvl:underLvl,
                    type:type
                }
            }).done(function (data) {
                if(data>='1'){
                    swal("Success", "Successfully updated menu", "success");
                    return false;
                }
                else
                {
                    console.log(data);
                    swal({
                        title: "Error!",
                        text: "Unable to update menu.",
                        type: "error"
                    });
                    return false;
                }
            })
                .fail(function (error) {
                    console.log(error);
                });
        }
    }
    function updateSkinGames(type,underLvl){
        type = typeof type !== 'undefined' ? type : 'Desktop';
        var jurisdiction=($tree.jstree(true).get_selected(true));
        if(jurisdiction==''){
            $.jAlert({
                'title': 'Error',
                'content': "Please select at least one jurisdiction 5",
                'theme': 'red',
                'btns': {'text':'Ok', 'theme': 'red'}
            });
        }
        else {

            $.ajax({
                url: '/services/general_services.inc',
                method: "POST",
                data: {
                    action:'updateSkinGames',
                    games:JSON.stringify($('#games').val()),
                    jurisdiction: jurisdiction[0].id.substring(1),
                    underLvl:underLvl,
                    type:type
                }
            }).done(function (data) {
                if($.trim(data)>='1'){
                    swal("Success", "Successfully saved games", "success");
                    return false;
                }
                else
                {
                    console.log($.trim(data));
                    swal({
                        title: "Error!",
                        text: "Unable to update jurisdiction games",
                        type: "error"
                    });

                    return false;
                }
            })
                .fail(function (error) {
                    console.log(error);
                });
        }
    }

    function updateSkinConfiguration(type,underLvl){
        type = typeof type !== 'undefined' ? type : 'Desktop';
        var jurisdiction=($tree.jstree(true).get_selected(true));
        if(jurisdiction==''){
            $.jAlert({
                'title': 'Error',
                'content': "Please select at least one jurisdiction 6",
                'theme': 'red',
                'btns': {'text':'Ok', 'theme': 'red'}
            });
        }
        else {

            $.ajax({
                url: '/services/general_services.inc',
                method: "POST",
                data: {
                    action: 'updateSkinSettings',
                    settings:$('#skinForm').serialize(),
                    jurisdiction: jurisdiction[0].id.substring(1),
                    type:type,
                    underLvl:underLvl
                }
            }).done(function (data) {
                if(data>='1'){
                    swal("Success", "Successfully saved skin settings", "success");
                    return false;
                }
                else
                {
                    console.log(data);
                    swal({
                        title: "Error!",
                        text: "Unable to update skin settings",
                        type: "error"
                    });
                    return false;
                }
            })
                .fail(function (error) {
                    console.log(error);
                });
        }
    }

    function getSkinGames(type,providers){
        type = typeof type !== 'undefined' ? type : 'Desktop';
        var jurisdiction=($tree.jstree(true).get_selected(true));
        if(jurisdiction==''){
            $.jAlert({
                'title': 'Error',
                'content': "Please select at least one jurisdiction 7",
                'theme': 'red',
                'btns': {'text':'Ok', 'theme': 'red'}
            });
        }
        else {
            $.ajax({
                url: '/services/general_services.inc',
                method: "POST",
                data: {
                    action: 'getSkinGames',
                    // action: 'getSkinProviders',
                    jurisdiction: jurisdiction[0].id.substring(1),
                    type:type,
                    providers:providers
                }

            }).done(function (data) {

                $('.skinGamesContent').empty();
                if(data == 2) {
                    $.jAlert({
                        'title': 'Error',
                        'content': "No data for selected providers",
                        'theme': 'red',
                        'btns': {'text':'Ok', 'theme': 'red'}
                    });
                    return false;
                }
                $('#skinGamesContent'+type).empty().append(data);
                $("#games").multiSelect({ selectableOptgroup: true,
                    keepOrder:true,
                    selectableHeader: "<div  style='    background-color: #4A7EB6;padding: 15px;font-size: 20px;min-height: 0;color:white;font-weight: bold;text-align: center;'>Not enabled</div><input type='text' style='width:calc(100% - 22px);padding: 10px;margin-bottom: 0;border-radius: 0' class='search-input' autocomplete='off' placeholder='Search'>",
                    selectionHeader: "<div style='    background-color: #4A7EB6;padding: 15px;font-size: 20px;min-height: 0;color: #55ff55;;font-weight: bold;text-align: center;'>Enabled</div><input type='text' style='width:calc(100% - 22px);padding: 10px;margin-bottom: 0;border-radius: 0' class='search-input' autocomplete='off' placeholder='Search'>",
                    afterInit: function(ms){
                        var that = this,
                            $selectableSearch = that.$selectableUl.prev(),
                            $selectionSearch = that.$selectionUl.prev(),
                            selectableSearchString = '#'+that.$container.attr('id')+' .ms-elem-selectable:not(.ms-selected)',
                            selectionSearchString = '#'+that.$container.attr('id')+' .ms-elem-selection.ms-selected';

                        that.qs1 = $selectableSearch.quicksearch(selectableSearchString)
                            .on('keydown', function(e){
                                if (e.which === 40){
                                    that.$selectableUl.focus();
                                    return false;
                                }
                            });

                        that.qs2 = $selectionSearch.quicksearch(selectionSearchString)
                            .on('keydown', function(e){
                                if (e.which == 40){
                                    that.$selectionUl.focus();
                                    return false;
                                }
                            });
                    },
                    afterSelect: function(){
                        this.qs1.cache();
                        this.qs2.cache();
                    },
                    afterDeselect: function(){
                        this.qs1.cache();
                        this.qs2.cache();
                    }
                });
            }).fail(function (error) {
                    console.log(error);
            });
        }
    }

    function getSkinProviders(type){
        type = typeof type !== 'undefined' ? type : 'Desktop';
        var jurisdiction=($tree.jstree(true).get_selected(true));
        if(jurisdiction==''){
            $.jAlert({
                'title': 'Error',
                'content': "Please select at least one jurisdiction 8",
                'theme': 'red',
                'btns': {'text':'Ok', 'theme': 'red'}
            });
        }
        else {
            $.ajax({
                url: '/services/general_services.inc',
                method: "POST",
                data: {
                    // action: 'getSkinGames',
                    action: 'getSkinProviders',
                    jurisdiction: jurisdiction[0].id.substring(1),
                    type:type
                }
            }).done(function (data) {
                var header = '<div style="position:relative">'
                        +'<div class="providers_list"><?=$lang->getLang("Providers list")?></div>'
                        +' <button class="btn btn-success" data-type="'+type+'" id="load_selected_provider_games"><?=$lang->getLang("Load games")?></button>'
                    +'</div>';
                $('#skinProvidersContent'+type).empty().append(header);
                $('#skinProvidersContent'+type).append(data);

            }).fail(function (error) {
                console.log(error);
            });
        }
    }


    function getSkinSettings(type){
        type = typeof type !== 'undefined' ? type : 'Desktop';
        var jurisdiction=($tree.jstree(true).get_selected(true));
        if(jurisdiction==''){
            $.jAlert({
                'title': 'Error',
                'content': "Please select at least one jurisdiction 2 1",
                'theme': 'red',
                'btns': {'text':'Ok', 'theme': 'red'}
            });
        } else {
            $.ajax({
                url: '/services/general_services.inc',
                method: "POST",
                data: {
                    action: 'getSkinSettings',
                    jurisdiction: jurisdiction[0].id.substring(1),
                    type:type
                }
            }).done(function (data) {
                $('.skinSettingsContent').empty();
                $('#skinSettingsContent'+type).empty().append(data);
                $('.selectMultiple').select2({
                    width: "220px"
                });
            })
                .fail(function (error) {
                    console.log(error);
                });
        }
    }
    $(function() {
        $("#t_start_time").datetimepicker({
            changeMonth: true,
            changeYear: true,
            numberOfMonths: 1,
            showSecond: false,
            timeFormat: 'HH:mm',
            onChangeMonthYear: function (year, month, i) {
                var day = i.selectedDay;
                $(this).datepicker('setDate', new Date(year, month - 1, day));
            }

        });
        $( "#filter_from" ).datetimepicker({
            changeMonth: true,
            changeYear: true,
            numberOfMonths: 1,
            showSecond: false,
            timeFormat: 'HH:mm',
            onChangeMonthYear: function (year, month, i) {
                var day = i.selectedDay;
                $(this).datepicker('setDate', new Date(year, month - 1, day));
            }

        });
        $( "#filter_to" ).datetimepicker({
            changeMonth: true,
            changeYear: true,
            numberOfMonths: 1,
            showSecond: false,
            timeFormat: 'HH:mm',
            onChangeMonthYear: function (year, month, i) {
                var day = i.selectedDay;
                $(this).datepicker('setDate', new Date(year, month - 1, day));
            }
        });

        $("#t_id").select2({
            width: "220px"
        });

        $(document).off('click','#load_selected_provider_games');
        $(document).on('click','#load_selected_provider_games', function () {
            var providers = '';
            $('.check_provider').each(function () {
                if($(this).is(':checked')) {
                    var code = $(this).data('code');
                    if(code != '') {
                        if (providers != '') providers += ",";
                        providers += "'" + code + "'";
                    }
                }
            });
            getSkinGames($(this).data("type"),providers);
        });
    });

</script>

<style>
    .hidden-video{
        display: none;
    }
    .time_select {
        width: 50px;
    }
    .skinProvidersContent {
        float: left;
        width: 33%;
    }
    .skinGamesContent {
        float: right;
        width: 66%;
    }

    /* checkbox style start =======================================================*/
    .nice_checkbox:hover {
        background-color: #08c;
        color: #fff;
    }
    .nice_checkbox {
        border: 1px #ddd solid;
        height: 34px;
        display: block;
        position: relative;
        padding-left: 40px;
        margin: 0px 5px 5px 0px;
        cursor: pointer;
        font-size: 18px;
        -webkit-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;
        user-select: none;
        line-height: 34px;
    }

    /* Hide the browser's default checkbox */
    .nice_checkbox input {
        position: absolute;
        opacity: 0;
        cursor: pointer;
        height: 0;
        width: 0;
    }

    /* Create a custom checkbox */
    .checkmark {
        position: absolute;
        top: 5px;
        left: 5px;
        height: 25px;
        width: 25px;
        background-color: #eee;
    }

    /* On mouse-over, add a grey background color */
    .nice_checkbox:hover input ~ .checkmark {
        background-color: #fff;
    }

    /* When the checkbox is checked, add a blue background */
    .nice_checkbox input:checked ~ .checkmark {
        background-color: #08c;
    }
    
    .nice_checkbox input:checked:hover ~ .checkmark {
        background-color: #fff;
    }

    /* Create the checkmark/indicator (hidden when not checked) */
    .checkmark:after {
        content: "";
        position: absolute;
        display: none;
    }

    /* Show the checkmark when checked */
    .nice_checkbox input:checked ~ .checkmark:after {
        display: block;
    }
    .nice_checkbox input:checked:hover ~ .checkmark:after {
        border-color: #000;
    }

    /* Style the checkmark/indicator */
    .nice_checkbox .checkmark:after {
        left: 9px;
        top: 5px;
        width: 5px;
        height: 10px;
        border: solid white;
        border-width: 0 3px 3px 0;
        -webkit-transform: rotate(45deg);
        -ms-transform: rotate(45deg);
        transform: rotate(45deg);
    }
    .providers_list {
        font-size: 20px;
        font-weight: bold;
        height: 35px;
        line-height: 35px;
    }
    #load_selected_provider_games {
        position: absolute;
        right: 5px;
        top:0;
    }
</style>

<div class="modal" id="loadingModal" style="display: none;margin-top:15%">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-body centered" >
                <?=$lang->getLang("Loading ...")?><img src="media/images/ajax-loader.gif">
            </div>
        </div>
    </div>
</div>


<div id="myModal" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
    <div class="modal-header">
        <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
        <h3 class="text-info text-center" id="modalHeader"><?=$lang->getLang("Add a new internal game")?> </h3> <br />
    </div>
    <div class="modal-body">
        <table class="table table-striped" style='margin: auto; text-align: left'>
            <tr>
                <td><?=$lang->getLang("Id")?></td>
                <td><input type="text" id="id" value=''></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Name")?></td>
                <td><input type="text" id="name" value=''></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Min Bet")?></td>
                <td><input type="text" id="minbet" class="inputnumberdec" value=''></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Max Bet")?></td>
                <td><input type="text" id="maxbet" class="inputnumberdec" value=''></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Max Payout")?></td>
                <td><input type="text" id='maxpayout' class="inputnumberdec" ></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Min Pos Bet")?></td>
                <td><input type="text"  id='minposbet' class="inputnumber" value='1'></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Max Pos Bet")?></td>
                <td><input type="text"  id='maxposbet' class="inputnumber" value='100'></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Small blind")?></td>
                <td><input type="text"  id='smallblind' class="inputnumberdec" value='0'></td>
            </tr> <tr>
                <td><?=$lang->getLang("Big blind")?></td>
                <td><input type="text"  id='bigblind' class="inputnumberdec" value='0'></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Ext Code")?></td>
                <td><input type="text" id='extcode' class="inputnumber" value=''></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Category")?></td>
                <td><select name='category' id='category'><?
                        $categories= getGameCategories();
                        while ( $categories->hasNext () ) {
                            $category = $categories->next ();
                            ?><option value='<?=$category['gct_name']?>'><?=$category['gct_name']?></option>
                        <?php }?>
                    </select></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Type")?></td>
                <td><select name='type' id='type'><?
                        $types= getGameType();
                        while ( $types->hasNext () ) {
                            $type= $types->next ();
                            ?><option value='<?=$type['gte_name']?>'><?=$type['gte_name']?></option>
                        <?php }?>
                    </select>
                </td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Localization")?></td>
                <td><input type="text"  id='localization' value=''></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Admin history class")?></td>
                <td><input type="text"  id='adminhistoryclass' value=''></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Is commondraw")?></td>
                <td><input type="checkbox"  id='iscommondraw' value='1' ></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Client type")?></td>
                <td><select id="clientType">
                        <option value='FLASH'><?=$lang->getLang("Flash")?></option>
                        <option value='HTML5'><?=$lang->getLang("HTML5")?></option>
                    </select>
                </td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Game version")?></td>
                <td><input type="number"  id='gameVersion' value='' step="0.01" ></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Is Private")?></td>
                <td><select id="private">
                        <option value='0' selected><?=$lang->getLang("False")?></option>
                        <option value='1'><?=$lang->getLang("True")?></option>
                    </select>
                </td>
            </tr>
            <tr>
                <td><?=$lang->getLang("More params")?></td>
                <td><input type="checkbox"  id='moreParams'></td>
            </tr>
            <tr class="hidden-video">
                <td><?=$lang->getLang("Is video")?></td>
                <td><select id="isVideo">
                        <option value=''><?=$lang->getLang("")?></option>
                        <option value='true'><?=$lang->getLang("True")?></option>
                        <option value='false'><?=$lang->getLang("False")?></option>
                    </select>
                </td>
            </tr>
            <tr class="hidden-video">
                <td><?=$lang->getLang("Dealer time")?></td>
                <td><input type="text" id="dealerTime" val=''> Seconds</td>
            </tr>
            <tr class="hidden-video">
                <td><?=$lang->getLang("Video type")?></td>
                <td>
                    <select id="videoType">
                        <option value=''><?=$lang->getLang("")?></option>
                        <option value='1'><?=$lang->getLang("1")?></option>
                        <option value='2'><?=$lang->getLang("2")?></option>
                    </select>
                </td>
            </tr>

        </table><br>
        <div id="addResult"></div>

    </div>
    <div class="modal-footer">
        <button class="btn" data-dismiss="modal" aria-hidden="true"><?=$lang->getLang("Cancel")?></button>
        <button class="btn btn-primary" id="addGame" name="save"  onclick='addGame()'><?=$lang->getLang("Add")?></button>
    </div>
</div>

<div id="tournamentModal" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="tournamentModalLabel" aria-hidden="true">
    <div class="modal-header">
        <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
        <h3 class="text-info text-center" id="modalHeader"><?=$lang->getLang("Add tournament")?> </h3> <br />
    </div>
    <input type="hidden" id="update_id" >
    <div class="modal-body">
        <table class="table table-striped" style='margin: auto; text-align: left'>
            <tr>
                <td><?=$lang->getLang("Game")?></td>
                <td><?=gameList()?></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Name")?></td>
                <td><input type="text" id="t_name" value=''></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Description")?></td>
                <td><textarea  id="t_description"></textarea></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Start time")?></td>
                <td><input type="text" name="t_start_time" id="t_start_time" value="<?=$date_start?>"></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Start hour")?></td>
                <td>
                    <select class="time_select" name="t_start_hour" id="t_start_hour">
                        <?php
                        for($i=0;$i<24;$i++) {
                            $val = $i<10?'0'.$i:$i;
                            echo "<option value='$val'>$val</option >";
                        }
                        ?>
                    </select>
                    :
                    <select  class="time_select" name="t_start_minute" id="t_start_minute">
                        <?php
                        for($i=0;$i<60;$i++) {
                            $val = $i<10?'0'.$i:$i;
                            echo "<option value='$val'>$val</option >";
                        }
                        ?>
                    </select>
                </td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Min inscriptions")?></td>
                <td><input type="text"  id='t_min_in' class="inputnumber" value='0'></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Max inscriptions")?></td>
                <td><input type="text"  id='t_max_in' class="inputnumber" value='0'></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Amount")?></td>
                <td><input type="text"  id='t_amount' class="inputnumberdec" value='0'></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Game duration")?></td>
                <td><input type="text"  id='t_duration' class="inputnumber" value='0'></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Time wait bstart")?></td>
                <td><input type="text"  id='t_bstart' class="inputnumber" value='0'></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Time wait astart")?></td>
                <td><input type="text"  id='t_astart' class="inputnumber" value='0'></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Status")?></td>
                <td><select id="t_status">
                        <option value='0'><?=$lang->getLang("Closed")?></option>
                        <option value='1'><?=$lang->getLang("Open")?></option>
                        <option value='2'><?=$lang->getLang("Erased")?></option>
                    </select></td>
            </tr>

        </table><br>
        <div id="t_addResult"></div>

    </div>
    <div class="modal-footer">
        <button class="btn" data-dismiss="modal" aria-hidden="true"><?=$lang->getLang("Cancel")?></button>
        <button class="btn btn-primary" id="addTournament_btn" name="save"  onclick='addTournament()'><?=$lang->getLang("Add")?></button>
    </div>
</div>

<div id="insertGeneralModal" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
    <div class="modal-header">
        <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
        <h3 class="text-info text-center" id="modalGeneralHeader"><?=$lang->getLang("Add a new game")?> </h3> <br />
    </div>
    <input type="hidden" value="" id="generalGameId">
    <div class="modal-body">
        <table class="table table-striped" style='margin: auto; text-align: left'>
            <tr>
                <td><?=$lang->getLang("Category")?></td>
                <td>
                    <select name='gCategory' id='gCategory'>
                        <option value='live casino'>Live casino</option>
                        <option value='slots'>Slots</option>
                        <option value='table games'>Table games</option>
                        <option value='video poker'>Video poker</option>
                    </select>
                </td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Button image")?></td>
                <td><input type="text" id="gButtonImage" value='' placeholder="Button image source"></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Game id")?></td>
                <td><input type="text" id="gGID" placeholder="Game id" ></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Width")?></td>
                <td><input type="number" id="gWidth" placeholder="Game window width" value="905"></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Height")?></td>
                <td><input type="text" id='gHeight' placeholder="Game window height" value="640" ></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Provider code")?></td>
                <td><input type="text"  id='gPes' placeholder="Provider code"></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Has play for fun")?></td>
                <td><input type="checkbox"  id='gPlayForFun' ></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Web name")?></td>
                <td><input type="text"  id='gWebName' placeholder="Web name"></td>
            </tr>
        </table><br>
        <div id="addResult"></div>
    </div>
    <div class="modal-footer">
        <button class="btn" data-dismiss="modal" aria-hidden="true"><?=$lang->getLang("Cancel")?></button>
        <button class="btn btn-primary" id="addGeneralGame" name="save"  onclick='addNewGeneralGame()'><?=$lang->getLang("Add")?></button>
    </div>
</div>

<div class="container" style="position: absolute;left:0;right: 0;width: 100%" >
    <div class="container-fluid">
        <table class="table table-bordered">
            <tr><td class='navbar-inner'><h3 class="text-center text-error"><?=$lang->getLang("Manage Games")?></h3></td></tr>
            <tr>
                <td>
                    <ul class="nav nav-tabs" id='myTab'>
                        <li class="active"><a href="#internal" data-toggle="tab"><?=$lang->getLang("Internal games")?></a></li>
                        <li><a href="#general" data-toggle="tab"><?=$lang->getLang("General games")?></a></li>
                        <li><a href="#tournament" data-toggle="tab"><?=$lang->getLang("Turnament games")?></a></li>
                        <li><a href="#structures" data-toggle="tab"><?=$lang->getLang("Structures configs")?></a></li>
                    </ul>
                    <div class="tab-content" style="overflow: visible">
                        <div class="tab-pane fade-in active" id="internal">
                            <div><a href="#myModal" class="fright btn btn-info btn-small showAddModal" style="color:white" data-toggle="modal" ><?=$lang->getLang("Add a new game")?></a></div><br><br><br>
                            <div id="internalGamesContainer"></div>
                        </div>
                        <div class="tab-pane fade" id="general">
                            <div><a href="#insertGeneralModal" class="fright btn btn-info btn-small showAddGeneralModal" style="color:white" data-toggle="modal" ><?=$lang->getLang("Add a new general game")?></a></div><br><br><br>
                            <div id="generalGamesContainer"></div>
                        </div>
                        <div class="tab-pane fade-in" id="tournament">
                            <div>
                                <h4><?=$lang->getLang("Filter")?></h4>

                                <div class="row-fluid">
                                    <div class="span8 t_filter">
                                        <div class="span3">
                                            <div class='label label-inverse'><?=$lang->getLang("From")?></div>
                                            <div class="controls">
                                                <div class="input-prepend">
                                                    <span class="add-on"><i class="icon-calendar"></i></span>
                                                    <input type='text' name='from' id='filter_from'  value="<?=$from?>" />
                                                </div>
                                            </div>
                                        </div>

                                        <div class="span3">
                                            <div class='label label-inverse'><?=$lang->getLang("Status")?></div>
                                            <div class="controls">
                                                <div class="input-prepend">
                                                    <select name="filter_t_status" id="filter_t_status">
                                                        <option value="all"><?=$lang->getLang("All")?></option>
                                                        <option value="1" selected><?=$lang->getLang("Open")?></option>
                                                        <option value="0"><?=$lang->getLang("Closed")?></option>
                                                        <option value="2"><?=$lang->getLang("Erased")?></option>
                                                    </select>
                                                </div>
                                            </div>
                                        </div>
                                    </div>
                                </div>
                                <div class="row-fluid">
                                    <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Until")?></div>
                                        <div class="controls">
                                            <div class="input-prepend">
                                                <span class="add-on"><i class="icon-calendar"></i></span>
                                                <input type='text' name='to' id='filter_to'  value="<?=$to?>" />
                                            </div>
                                        </div>
                                    </div>

                                </div>

                            </div>
                            <div>
                                <button onclick="getTournamentGames()" class="fleft btn btn-default btn-small " ><?=$lang->getLang("View")?></button>
                                <a href="#tournamentModal" class="fright btn btn-info btn-small showAddtournamentModal" style="color:white" data-toggle="modal" ><?=$lang->getLang("Add tournament")?></a>
                            </div><br><br><br>
                            <div id="tournamentGamesContainer"></div>
                        </div>
                        <div class="tab-pane fade" id="structures">
                            <fieldset>
                                <legend>Filter
                                    <label>Skin</label>
                                    <? $allSkins=getJurisdictionSkins();?>
                                    <?=createSkinSelect($allSkins,-1,false,true)?><br />
                                    <button type="button"  class="btn" id="skinButtonFilter">View</button> <div class="clearfix"></div><br />
                                </legend>
                            </fieldset>
                            <table class="table table-bordered">

                                <tr>
                                    <td class="span4">
                                        <div class="contentAdmin">
                                            <div class="contentHeadAdmin"><?=$lang->getLang("Jurisdiction chooser")?></div>
                                            <div class="well" style="max-height: 800px;overflow: auto">
                                                <div class="alert hidden alert-danger" id="jurAlert">
                                                    <?=$lang->getLang("No jurisdiction found")?>
                                                </div>
                                                <input type="text" id='jurisdictionTreeSearch' style="float:left;padding: 5px;width: 94%" placeholder="<?=$lang->getLang("Search (min. 4 char)")?>">
                                                <div id="searchResult"></div>
                                                <div class="clearfix"></div>
                                                <br/>
                                                <div id="jurisdictionTree"></div>
                                            </div>
                                        </div>
                                    </td>
                                    <td>
                                        <div class="tabbable">
                                            <ul class="nav nav-tabs" id="subsectionSelector">
                                                <li class="active"><a href="#skinGames" id="skinGamesButton"  data-toggle="tab"><button class="btn btn-inverse" id="SearchJurisdictions"><?=$lang->getLang("Games")?></button></a></li>
                                                <li><a href="#skinMenu" id="skinMenuButton"  data-toggle="tab"><button class="btn btn-inverse"><?=$lang->getLang("Menu")?></button></a></li>
                                                <li><a href="#skinSettings" id="skinSettingsButton" data-toggle="tab"><button class="btn btn-inverse"><?=$lang->getLang("Settings")?></button></a></li>
                                            </ul>
                                            <div class="tab-content">


                                                <div class="tab-pane active" id="skinGames">
                                                    <div class="tabbable tabs-left">
                                                        <ul class="nav nav-tabs">
                                                            <li class="active"><a href="#skinGamesDesktop" class="skinGamesButton" id="skinGamesDesktopButton" data-toggle="tab"><button class="btn btn-success" ><?=$lang->getLang("Desktop")?></button></a></li>
                                                            <li><a href="#skinGamesMobile" class="skinGamesButton" data-type="Mobile" id="skinGamesMobileButton" data-toggle="tab"><button class="btn btn-success"><?=$lang->getLang("Mobile")?></button></a></li>
                                                        </ul>
                                                        <div class="tab-content">
                                                            <div class="tab-pane active" id="skinGamesDesktop">
                                                                <div id="skinProvidersContentDesktop"  class="skinProvidersContent"></div>
                                                                <div id="skinGamesContentDesktop"  class="skinGamesContent" ></div>
                                                            </div>
                                                            <div class="tab-pane" id="skinGamesMobile">
                                                                <div id="skinProvidersContentMobile"  class="skinProvidersContent"></div>
                                                                <div id="skinGamesContentMobile" class="skinGamesContent"></div>
                                                            </div>
                                                        </div>
                                                    </div>


                                                </div>
                                                <div class="tab-pane" id="skinMenu">
                                                    <div class="tabbable tabs-left">
                                                        <ul class="nav nav-tabs">
                                                            <li class="active"><a href="#skinMenuDesktop" class="skinMenuButton" id="skinMenuDesktopButton" data-toggle="tab"><button class="btn btn-success" ><?=$lang->getLang("Desktop")?></button></a></li>
                                                            <li><a href="#skinMenuMobile" class="skinMenuButton" data-type="Mobile" id="skinMenuMobileButton" data-toggle="tab"><button class="btn btn-success"><?=$lang->getLang("Mobile")?></button></a></li>
                                                        </ul>
                                                        <div class="tab-content">
                                                            <div class="tab-pane active" id="skinMenuDesktop">
                                                                <div id="skinMenuContentDesktop"  class="skinMenuContent"></div>
                                                            </div>
                                                            <div class="tab-pane" id="skinMenuMobile">
                                                                <div id="skinMenuContentMobile" class="skinMenuContent"></div>
                                                            </div>
                                                        </div>
                                                    </div>


                                                </div>
                                                <div class="tab-pane" id="skinSettings">
                                                    <div class="tabbable tabs-left">
                                                        <ul class="nav nav-tabs">
                                                            <li class="active"><a href="#skinSettingsDesktop" id="skinSettingsDesktopButton" data-type="Desktop" data-toggle="tab"><button class="btn btn-success" ><?=$lang->getLang("Desktop")?></button></a></li>
                                                            <li><a href="#skinSettingsMobile" id="skinSettingsMobileButton" data-type="Mobile" data-toggle="tab"><button class="btn btn-success"><?=$lang->getLang("Mobile")?></button></a></li>
                                                        </ul>
                                                        <div class="tab-content">
                                                            <div class="tab-pane active" id="skinSettingsDesktop">
                                                                <div id="skinSettingsContentDesktop" class="skinSettingsContent"></div>
                                                            </div>
                                                            <div class="tab-pane" id="skinSettingsMobile">
                                                                <div id="skinSettingsContentMobile" class="skinSettingsContent"></div>
                                                            </div>
                                                        </div>
                                                    </div>
                                                </div>
                                            </div>
                                        </div>
                                    </td>
                                </tr>
                            </table>
                        </div>
                    </div>
                </td>
            </tr>
        </table>
    </div>
</div>



