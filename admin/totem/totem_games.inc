<?php
check_access ( 'totem_configure_games', $redirect = true );
require_once("Totem.class.inc");
if (! isset ( $_GET ['ttcode'] ) && !isset($_POST['ttcode'])) {
    addError ( "", $lang->getLang("Totem code don't found. Check the get parameters" ));
    showErrors ();
    die ();
}



define("LABEL_GAME", 'gameid');
define("LABEL_BILL" , 'bill');
define("LABEL_WITHDRAWAL", 'withdrawal');
define("LABEL_SKINS", "skins");
define("LABEL_TPOINT", "tpoint");
define("LABEL_TCODE", "tpcode");
define("LABEL_CUSTOM_CAT", "custcat");
$totem_code = (isset($_POST['ttcode'])?$_POST['ttcode']:$_GET ['ttcode']);
$sql = "SELECT ttm_conf_param FROM `totem` WHERE ttm_code = " . $dbh->quote ( $totem_code ) ;


$res = $dbh->queryOne ( $sql );
if($res){
    $totem=new Totem();
    $totem->setCode($totem_code);
    $totem->setConfigParams($res);

    $totemListConf = explode('~',  $totem->getConfigParams());
    $gameContent = "";

    foreach($totemListConf as $key => $value){
        list($label, $content) =  split(":", $value);
        if($label == LABEL_GAME){
            $gameContent = $content;
        }

        if($label == LABEL_CUSTOM_CAT){
            $customCat = explode(";",$content);
        }
    }
    $gameCategories=explode("|",$gameContent);
    // $activatedGames = split(";",$gameContent);
    $availableGames=parse_ini_file(WEB_CONFIG."/totemGames.ini",true);
    // var_dump($gameCategories);
    //  var_dump($res);
    if(isset($_POST['games'])){
        $_POST['games']=json_decode($_POST['games']);
        $_POST['categories']=json_decode($_POST['categories']);
        if($totem->updateGames($_POST['games'],$_POST['categories'])){
            // var_dump($totem->getConfigParams());
            die('1');
        }
        else{
            die('-1');
        }

    }
}
?>
<link rel="stylesheet" type="text/css" href="/media/js/multiselect/css/multi-select.css" title="core_style">
<script src="/media/js/multiselect/js/jquery.multi-select.js" type="text/javascript"></script>
<script src="/media/jquery.quicksearch.js" type="text/javascript"></script>
<hr />
<center>
    <div class="bodyHD">Totem <?=strtoupper($totem_code)?> Games:</div><br />
    <form action="<?=$_SERVER['PHP_SELF']."?".$_SERVER['QUERY_STRING']?>"
          method="POST" name="search">
        <input type="hidden" name="do_search" value="1" />
        <h3 style="text-align: left">Custom category names</h3>
        <div>
        <?foreach($availableGames as $k=>$v) {
            $name=$k;
            if(isset($customCat)){
                foreach($customCat as $custK=>$custV){
                    list($custCatName,$custCatVal)=explode("-",$custV);
                    if($custCatName==$name){
                        $name=$custCatVal;
                    }
                }
            }
            ?>
            <div class="fleft" style="width: 40%;margin-left: 30px">
                <div class="half fleft aleft" style="color: #0085ff;"> <?=$k?>:</div>
                <div class="half fleft"> <input type="text" class="fleft categories" style="max-width: 100%" name="<?=$k?>" value="<?=$name?>"></div>
            </div>
        <? }?>
        </div><div class="clearfix"></div>
        <br />
        <table>
            <tr>
                <td>
                    <select id="games" name="games[]" multiple="multiple" >
                        <?
                        foreach($availableGames as $k=>$v) {

                            ?>
                            <optgroup label="<?= $k ?>">
                                <?

                                foreach ($v as $k2 => $v2) {
                                    if($k2!='C000'){
                                        list($providerCode, $gameCode) = explode('-', $k2);
                                    }

                                    $games = explode('##', $v2);
                                    foreach ($games as $k2 => $v2) {
                                        $selected = false;
                                        $game = explode('@', $v2);
                                        if (is_numeric($game[0])) {
                                            $value = $game[0];
                                        } else {
                                            $value = $gameCode . "-" . $game[0] . "-" . $providerCode;
                                        }

                                        foreach ($gameCategories as $kCategory => $vCategory) {
                                            list($category, $categoryGames) = explode(',', $vCategory);
                                            $categoryGames = explode(';', $categoryGames);
                                            foreach ($categoryGames as $ctgK => $ctgV) {
                                                if ($ctgV == $value) {
                                                    $selected = true;

                                                }
                                            }
                                        }
                                        ?>
                                        <option
                                            value='<?=$k?>;<?= $value ?>' <?= ($selected ? 'selected' : '') ?>><?= str_replace("_"," ",$game[2]) ?></option>
                                        <?
                                    }
                                }
                                ?>
                            </optgroup>
                            <?
                        }

                        ?>
                    </select>
                </td>
            </tr>
            <tr>

                <td>
                    <button class="btn btn-primary" type="button" id="submit<?=$totem_code?>">Save

                    </button>
                </td>
            </tr>
        </table>
    </form>
</center>

<script type="text/javascript">
    $(function() {
        /*$("#gamesEnabled").multiselect({selectedList: 4}).multiselectfilter();*/
        $( '#submit<?=$totem_code?>')

            .click(function() {
                var categories={};
                 $('.categories').each(function(){
                    categories[$(this).attr('name')]=$(this).val();
                });
                $.post("totem/totem_games.inc",{ttcode:'<?=$totem_code?>',games:JSON.stringify($('#games').val()),categories:JSON.stringify(categories)}, function(data){
                    if(data=='1'){
                        jAlert('Successfully saved');

                    }
                    else{
                        jAlert('An error has occured.Games where not saved correctly');
                    }
                });
                return false;
            });
        $("#games").multiSelect({ selectableOptgroup: true,
            selectableHeader: "<div class='label' style='padding: 15px;font-size: 14px'>Available</div><input type='text' style='width: 92%;padding: 10px;' class='search-input' autocomplete='off' placeholder='Search'>",
            selectionHeader: "<div class='label' style='padding: 15px;font-size: 14px;color: #46ff46;'>Enabled</div><input type='text' style='width: 92%;padding: 10px;' class='search-input' autocomplete='off' placeholder='Search'>",
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
    });
</script>
<!--<link rel="stylesheet" type="text/css" href="/media/jquery.multiselect.css" title="core_style">
<script src="/media/jquery.multiselect.js" type="text/javascript"></script>
-->




<!-- toetm 4S3GG string(1536) "gameid:New Games,1061;1057;1058;1059;2006;9024;1055;2005;9009;9019;9013;1035;1036;1037;1038;1039;1040;1042;1043;1044;1052;1048;1051;1049;1010;1012;1013;1014;1018;1019;1020;1021;1022;1023;1025;1026;1027;1028;1029;1030;1033;1034|Slot Machines,1061;1057;1058;1059;1055;1035;1009;1010;1012;1013;1014;1018;1019;1020;1021;1023;1024;1025;1026;1027;1028;1029;1030;1032;1033;1034;1036;1037;1038;1039;1040;1041;1042;1043;1048;1049;1050;1051;1052|Live Games,6001;7006;7001;7003|Video Poker,2001;2003;2004;2005;2006|Tables Games,5001;5002;9001;4001;4002;4003;9005|Virtual Game,9024;9002;9007;9019;9021;9013;9009|Dice Games,1006;1007;1008;1046;1047|VLT,205-ONVO_9-C009;205-ONVO_10-C009;205-ONVO_13-C009;205-ONVO_20-C009;205-ONVO_21-C009;205-ONVO_22-C009;205-ONVO_23-C009;205-ONVO_24-C009;205-ONVO_25-C009;205-ONVO_26-C009;205-ONVO_37-C009;205-ONVO_70-C009;205-ONVO_71-C009;205-ONVO_73-C009;205-ONVO_74-C009;205-ONVO_75-C009;205-ONVO_76-C009;205-ONVO_77-C009;205-ONVO_78-C009;205-ONVO_83-C009;205-ONVO_84-C009;205-ONVO_86-C009;205-ONVO_87-C009;205-ONVO_88-C009;205-ONVO_89-C009;205-ONVO_90-C009;205-ONVO_92-C009;205-ONVO_93-C009;205-ORYX_AB-C009;205-ORYX_AE-C009;205-ORYX_BB-C009;205-ORYX_DD-C009;205-ORYX_FF-C009;205-ORYX_GD-C009;205-ORYX_LE-C009;205-ORYX_LS-C009;205-ORYX_MA-C009;205-ORYX_MF-C009;205-ORYX_MP-C009;205-ORYX_QS-C009;205-ORYX_RC-C009;205-ORYX_S69-C009;205-ORYX_SC-C009;205-ORYX_ST-C009;205-ORYX_TG-C009;205-ORYX_TH-C009|Shake and go~bill:5;10;20;50;0;0;0;0;0;0;0;0;0;0;0;0~withdrawal:3~skins:virtualsys~tpoint:n~tpcode:Null~cardonly:n"   -->