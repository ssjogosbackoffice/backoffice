<?php
/**
 * @author Marian
 * @date 2014-03-10
 */
$bannerProps=parse_ini_file(WEB_CONFIG.'/banners.ini',true);
$skinResrictions=array();
foreach($bannerProps['skins'] as $k=>$v){
    $thisComponents=explode(',',$v);
    $section=array();
    foreach($thisComponents as $k2=>$v2){
        $section[$k2+1]['width']=$bannerProps[$v2]['width'];
        $section[$k2+1]['height']=$bannerProps[$v2]['height'];
        $section[$k2+1]['type']=$v2;
    }
    $skinResrictions[$k]=$section;
}
//var_dump($skinResrictions);
check_access('manage_banners',true);
$view_banner_images=check_access('banner_view_images');
$manage_banner_images=check_access('banner_insert_images');
$bettingsports=check_access('betting_banners');
$active=(isset($_POST["operation"])) ? $_POST["operation"] : 'list' ;
$status = (isset($_POST["status"])) ? $_POST["status"] : 2 ;
$sectionSearch=(isset($_POST["sectionSearch"])) ? $_POST["sectionSearch"] : 0 ;
if(isset($_POST['doQuery'])&& $_POST['operation']=='add'){
    $id=$_POST['bnr'];
    $title = $_POST['title'];
    $subtitle=$_POST['subtitle'];
    $skin=$_POST['skin'];
    $section=$_POST['section'];
    $date_start=$_POST['date_start'];
    $date_end=$_POST['date_end'];
    $status_new=$_POST['status_new'];
    $body=$_POST['body'];
    $file=$_FILES;
    $betId=$_POST['betId'];
    $quoteType=$_POST['eventResults'];
    $href=$_POST['bannerActionSelect'];
    if($href=='1'){
        $href.=','.$_POST['hrefb'];
    }
    if($href=='2'){
        $href.=','.$_POST['hrefg'];
    }
}
$skinsList=getSkins($_SESSION['jurisdiction_id']);
$skins = array();
while($skinsList->hasNext()){
    $row = $skinsList->next();
    $skins[$row['skn_id']] = $row['skn_name'];
}
$sectionList=getSection();
$sections=array();
while($sectionList->hasNext()){
    $row = $sectionList->next();
    $sections[$row['bsc_id']] = $row['bsc_description'];

}
$gamesList=getAllGames();
$games=array();
while($gamesList->hasNext()){
    $row = $gamesList->next();
    $games[$row['gam_id']] = $row['gam_name'];
}
?>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css"  />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" />
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/ui/js/jquery.timepicker.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/jquery.dataTables.js" type="text/javascript"></script>
<script type="text/javascript">
    $(function() {
        dimensions=<?=json_encode($skinResrictions)?>;
        <?
             $allSkindimensions=array();
             $allSkindimensions['structures']=array();
             foreach($bannerProps as $k=>$v){
                if($k!='skins'){
                    $currdimensions=array($v['width'],$v['height']);
                                     if(!in_array($currdimensions,$allSkindimensions)){
                                     array_push($allSkindimensions,$currdimensions);
                                     array_push($allSkindimensions['structures'],$k);
                    }
                    else{
                         $allSkindimensions['structures'][array_search($currdimensions,$allSkindimensions)].=",".$k;
                    }
                }
             }
             ?>
        allSkinsDimensions=<?=json_encode($allSkindimensions)?>;


        var res=[];
        $.each(allSkinsDimensions['structures'], function(indexR, valueR) {
            valueR=valueR.split(',');
            $.each(valueR, function(indexRV, valueRV) {
                res[valueRV]=indexR;
            });
        });

        $('#skin').on('change',function(){
            skin=$(this).val();
            section=$("#section").val();
            if(skin==0){
                var currentDimensions=[];

                $.each(dimensions, function(index, value) {
                    var newDimensions=(value[section]['width']+","+value[section]['height']);
                    if(currentDimensions.indexOf(newDimensions)<0){
                        var found = res[value[section]['type']];
                        currentDimensions[found]=newDimensions;
                    }
                });
                uploadImageContainers=" ";
                if(section==4){
                    $.each(currentDimensions, function(index, value) {
                        if(index!='structures' && typeof(value) != "undefined"){
                            uploadImageContainers+=
                                "<div><div class='label label-inverse'><?=$lang->getLang("Banner")?>("+currentDimensions[index]+")</div>"+
                                "<br>"+
                                "<div class='controls'>"+
                                "<input type='file'  name='file["+index+"]'  value='default.inc'  >"+
                                "</div>"+
                                "</div>";
                        }
                    });

                }
                else {


                    $.each(allSkinsDimensions, function (index, value) {
                        if (index != 'structures') {
                            uploadImageContainers +=
                                "<div><div class='label label-inverse'><?=$lang->getLang("Banner")?>(" + allSkinsDimensions[index] + ")</div>" +
                                "<br>" +
                                "<div class='controls'>" +
                                "<input type='file'  name='file[]'  value='default.inc'  >" +
                                "</div>" +
                                "</div>";
                        }
                    });
                }
                $("#bannerImageUploaderContainer").empty().append(uploadImageContainers);
            }
            else{

                uploadImageContainers=" ";
                var newWidth=(typeof(dimensions[skin]) != "undefined" && typeof(dimensions[skin][section]) != 'undefined' ? dimensions[skin][section]["width"] : dimensions["default"][section]["width"]);
                var newHeight=(typeof(dimensions[skin]) != "undefined" && typeof(dimensions[skin][section]) != 'undefined' ? dimensions[skin][section]["height"] : dimensions["default"][section]["height"]);
                uploadImageContainers+="<div><div class='label label-inverse'><?=$lang->getLang("Banner")?>("+newWidth+","+newHeight+")</div>"+
                        "<br>"+
                        "<div class='controls'>"+
                        "<input type='file'  name='file' id='file' value='default.inc'  >"+
                        "</div>"+
                        "</div>";
            $("#bannerImageUploaderContainer").empty().append(uploadImageContainers);
            }
        }).trigger('change');

        $('#section').change(function() {
            skin=$('#skin').val();
            section=$(this).val();
            if(skin==0){

 /*               var currentDimensions={};
                $.each(dimensions, function(index, value) {
                   var newDimensions=[value[section]['width'],value[section]['height']];

                    if( !currentDimensions.hasOwnProperty(newDimensions)){

                        currentDimensions[newDimensions]=true;
                       // console.log(currentDimensions.indexOf(newDimensions));

                    }*/
                var currentDimensions=[];
                $.each(dimensions, function(index, value) {
                    var newDimensions=(value[section]['width']+","+value[section]['height']);
                    if(currentDimensions.indexOf(newDimensions)<0){
                        var found = res[value[section]['type']];
                        currentDimensions[found]=newDimensions;
                    }
                });
                uploadImageContainers=" ";
                if(section==4){
                    $.each(currentDimensions, function(index, value) {
                        if(index!='structures' && typeof(value) != "undefined"){

                            uploadImageContainers+=
                                "<div><div class='label label-inverse'><?=$lang->getLang("Banner")?>("+currentDimensions[index]+")</div>"+
                                "<br>"+
                                "<div class='controls'>"+
                                "<input type='file'  name='file["+index+"]'  value='default.inc'  >"+
                                "</div>"+
                                "</div>";
                        }
                    });

                }
            else{
                    $.each(allSkinsDimensions, function(index, value) {
                        if(index!='structures'){
                            uploadImageContainers+=
                                "<div><div class='label label-inverse'><?=$lang->getLang("Banner")?>("+allSkinsDimensions[index]+")</div>"+
                                "<br>"+
                                "<div class='controls'>"+
                                "<input type='file'  name='file[]'  value='default.inc'  >"+
                                "</div>"+
                                "</div>";
                        }
                    });
                }
                $("#bannerImageUploaderContainer").empty().append(uploadImageContainers);
            }
            else{
                uploadImageContainers=" ";
                var newWidth=(typeof(dimensions[skin]) != "undefined" && typeof(dimensions[skin][section]) != 'undefined' ? dimensions[skin][section]["width"] : dimensions["default"][section]["width"]);
                var newHeight=(typeof(dimensions[skin]) != "undefined" && typeof(dimensions[skin][section]) != 'undefined' ? dimensions[skin][section]["height"] : dimensions["default"][section]["height"]);
                uploadImageContainers+="<div><div class='label label-inverse'><?=$lang->getLang("Banner")?>("+newWidth+","+newHeight+")</div>"+
                    "<br>"+
                    "<div class='controls'>"+
                    "<input type='file'  name='file' id='file' value='default.inc'  >"+
                    "</div>"+
                    "</div>";
                $("#bannerImageUploaderContainer").empty().append(uploadImageContainers);
            }

            if($(this).find('option:selected').val()==4){
                $('#bettingContainer').show();
            }
            else{
                $('#bettingContainer').hide();
            }
        });

        $('.preview').click(function(e) {
            if(ImageExists($(this).attr('data-img-url'))){
                $('#myModal img').attr('src', $(this).attr('data-img-url'));
            }else{
                $('#myModal img').attr('src', "/media/images/unavailable.jpg");
            }
        });
        if($("#section").find('option:selected').val()==4){
            $('#bettingContainer').show();
        }
        else{
            $('#bettingContainer').hide();
        }

        $("#getBetEvents").on('click', function() {
            var event_id = $('#betId').attr('value');
            $.post("/services/general_services.inc",{action:"6",event_id:event_id}, function(data){
                var evt = $.parseJSON(data);
                var html = '<option value="0"><?=$lang->getLang('Select quote type')?></option>\n';
                for(var i in evt) {
                    if(!evt.hasOwnProperty(i)) { continue; }
                    html += '<option value=' + evt[i].IDBet + '>' + (evt[i].Descrizione) + '</option>';
                }
                $('#eventResults').html(html);
            });
        });


        $('#h' + $("#bannerActionSelect").find('option:selected').val()).show();
        $('#bannerActionSelect').change(function() {
            $('.hidden').hide();
            $('#h' + $(this).find('option:selected').val()).show();

        });
        $(".display").dataTable({'sPaginationType':"full_numbers",'iDisplayLength': 100,
                "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 'no-sort' ] }] ,"order": [[ 0, "desc" ]]}

        );
        $('#date_start').datetimepicker();
        $('#date_end').datetimepicker();
        $("#bannerTable").on('click', '.modifyBanner', function() {
            var objj = $(this).closest('tr');
            $('input[name="title"]').val($.trim($(objj).find('td:eq(1)').text()));
            $('input[name="subtitle"]').val($.trim($(objj).find('td:eq(2)').text()));
            $('textarea[name="body"]').val($.trim($(objj).find('td:eq(3)').text()));
            $("#skin option:contains(" + $.trim($(objj).find('td:eq(4)').text()) + ")").prop('selected', true);
            //$("select[name='section'] option[text=" + $.trim($(objj).find('td:eq(5)').text()) + "]").prop('selected', true);
            $('select[name="section"] option').filter(function () {return $(this).html() ==$.trim($(objj).find('td:eq(5)').text()); }).prop("selected", true);;
            var bnrAction=$.trim($(objj).find('td:eq(6)').text()).split(':');
            var bnrType=$.trim(bnrAction[0]);
            var bnrHref=$.trim(bnrAction[1]);
            if(bnrType=='Section'){

                $("select[name='bannerActionSelect']").val(1).change();
                $("select[name='hrefb'] option:contains(" + $.trim(bnrHref) + ")").prop('selected', true);
            }
            if(bnrType=='Game'){
                $("select[name='bannerActionSelect']").val(2).change();
                $("select[name='hrefg'] option").filter(function() {
                    return $(this).text() == bnrHref;
                }).prop('selected', true);
            }
            if(bnrType=='Event'){
                $("#betId").val(bnrHref);
                <?if(check_access('betting_banners')){?>
                $("#bettingContainer").show();
                <? } ?>
            }
            $("#status_new option:contains(" + $.trim($(objj).find('td:eq(7)').text()) + ")").prop('selected', true);
            $('input[name="date_start"]').val($.trim($(objj).find('td:eq(8)').text()));
            $('input[name="date_end"]').val($.trim($(objj).find('td:eq(9)').text()));
            $('input[name="bnr"]').val($.trim($(objj).attr('id')));
            $('a[href="#insert"]').click();
        });
        $("#bannerTable").on('click', '.deleteBanner', function() {
            var bnr = $(this).closest('tr').attr('id');
            jConfirm("<?=$lang->getLang("Are you sure you want to delete this banner?")?>","",function (answer) {
                if(answer){
                    $.post("/services/general_services.inc",{action:"5",bnr:bnr}, function(data){
                        if(data==1){
                            $('#delResult').empty().append("<h4 class='alert alert-success fade in'><button type='button' class='close' data-dismiss='alert'>&times;</button><?=$lang->getLang("You have successfully deleted the banner!")?></h4><br />");
                            $('#'+bnr).hide();
                        }
                        else
                        {
                            $('#delResult').empty().append("<h4 class='alert alert-error fade in'><button type='button' class='close' data-dismiss='alert'>&times;</button><?=$lang->getLang("An error occurred!Banner not deleted!")?></h4><br />");
                        }
                    });
                }
            });
        });

        $('#img').change(function() {
            showPreview(this);
        });

    });
    function ImageExists(image_url){

        var http = new XMLHttpRequest();

        http.open('HEAD', image_url, false);
        http.send();

        return http.status != 404;
    }

    function showPreview(input) {
        if (input.files && input.files[0]) {
            var reader = new FileReader();
            reader.onload = function (e) {
                $('#preview').attr('src', e.target.result).show();
            }
            reader.readAsDataURL(input.files[0]);
        }
    }
</script>
<style>
    .close{
        opacity: 0.6;
    }
    .close:hover{
        opacity: 0.8;
    }
</style>
<div id="myModal" class="modal hide fade" style="width: auto;background-color:transparent;box-shadow: 0 0 40px black" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
    <div class="modal-body" style="padding: 0px">
        <button type="button" class="close" data-dismiss="modal" aria-hidden="true" style="position: absolute;padding: 10px;color:white;right: 0px">Close</button>
        <img src="#"/>
    </div>
</div>


<div class="container" style="position: absolute;left:0;right: 0;width: 100%" >
<div class="container-fluid">
<table class="table table-bordered">
<tr><td class='navbar-inner'><h3 class="text-center"><?=$lang->getLang("Banners")?></h3></td></tr>
<tr>
<td>
<ul class="nav nav-tabs" id='myTab'>
    <li <?=($active=='list') ? "class='active'":""?>><a href="#list" data-toggle="tab"><?=$lang->getLang("List")?></a></li>
    <li <?=($active=='add') ? "class='active'":""?>><a href="#insert" data-toggle="tab"><?=$lang->getLang("Add/Modify")?></a></li>
    <?if($view_banner_images) { ?>
        <li <?=($active=='mngImg') ? "class='active'":""?>><a href="#mngImg" data-toggle="tab"><?=$lang->getLang("Banner Images")?></a></li>
    <? } ?>
</ul>
<div class="tab-content" style="overflow: visible">
<div class="tab-pane fade <?=($active=='list') ? "in active":""?>" id="list">
<?php
form_head()?>
<?php printHiddenInput('doQuery', 'yes');?>
<?php printHiddenInput('operation', 'list');?>
<div class='well'>

    <div class='row'>
        <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Status")?></div>
            <br>
            <div class="controls">
                <div class="input-prepend">
                    <span class="add-on"><i class="icon-random"></i></span>
                    <select name='status'>
                        <option value='2' <?=($status=='2') ? "selected='selected'":""?>><?=$lang->getLang("All")?></option>
                        <option value='1' <?=($status=='1') ? "selected='selected'":""?>><?=$lang->getLang("Active")?></option>
                        <option value='0' <?=($status=='0') ? "selected='selected'":""?>><?=$lang->getLang("Inactive")?></option>
                    </select>
                </div>
            </div>
        </div>

        <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Section")?></div>
            <br>
            <div class="controls">
                <div class="input-prepend">
                    <span class="add-on"><i class="icon-random"></i></span>
                    <select name="sectionSearch" id="sectionSearch">
                        <option  value="0" <?=(0==$sectionSearch? 'selected=selected': '')?> ><?=$lang->getLang('All')?></option>
                        <?php foreach($sections as $key => $value){
                            if($key==4){
                                if(check_access('betting_banners')){ ?>
                                    <option  value="<?=$key?>" <?=($key==$sectionSearch? 'selected=selected': '')?> ><?=$value?></option>
                                <? }
                            }
                            else{ ?>
                                <option  value="<?=$key?>" <?=($key==$sectionSearch? 'selected=selected': '')?> ><?=$value?></option><?php
                            }
                        } ?>
                    </select>
                </div>
            </div>
        </div>
    </div>
    <button class="btn btn-primary" ><?=$lang->getLang("Search")?></button><br />
    </form><br />
</div>
<div id="delResult"></div><br />
<?php
if(isset($_POST['doQuery']) && $_POST['operation']=='list'){
    ?>
    <div id='results'>
        <?
        $date_start=$_POST['date_start'];
        $date_end=$_POST['date_end'];
        $d1 = strtotime("$date_start");
        $d2 = strtotime("$date_end");
        if($d2-$d1<0){
            addError("",$lang->getLang("Please choose an end date greater than start date"));
        }
        if(!areErrors())
        {
            $sql="Select banner.*,skn_name,skn_id,skn_foldername,bsc_description,bsc_id from banner
                          LEFT JOIN skins on bnn_skn_id=skn_id
                          LEFT JOIN banner_section on bsc_id=bnn_bsc_id";
            if($_SESSION["jurisdiction_class"] != "casino"){
                $sql .= "
                            LEFT JOIN jurisdiction club ON club.jur_id = skn_jur_id
  		                    LEFT JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
                            LEFT JOIN jurisdiction regional ON regional.jur_id = district.jur_parent_id
                            LEFT JOIN jurisdiction nation   ON nation.jur_id   = regional.jur_parent_id";
            }
            switch($_SESSION["jurisdiction_class"]){
                case "club":
                    $sql .= " WHERE ( club.jur_id = ".$_SESSION['jurisdiction_id'];
                    break;
                case "district":
                    $sql .= " WHERE ( district.jur_id = ".$_SESSION['jurisdiction_id'];
                    break;
                case "region":
                    $sql .= " WHERE ( regional.jur_id = ".$_SESSION['jurisdiction_id'];
                    break;
                case "nation":
                    $sql .= " WHERE ( nation.jur_id = ".$_SESSION['jurisdiction_id'];
                    break;
                case "casino":
                    $sql .= " WHERE ( 1=1";
                    break;
                default:
                    $sql .= " WHERE ( 1=0";
                    break;
            }

            $sql .="   OR bnn_skn_id=0 ) ";
            if($status!='2'){
                $sql .=" AND ( bnn_status=$status";
                if($status==1){
                    $sql .="   AND bnn_start_date <= NOW()
                                       AND bnn_end_date >= NOW() )";
                }
                if($status==0){
                    $sql .="   OR bnn_start_date >= NOW()
                                       OR bnn_end_date <= NOW() )";
                }
            }
            if($sectionSearch!=0){
                $sql .=" AND BNN_BSC_ID=$sectionSearch";
            }
            if(!check_access('betting_banners')){
                $sql.=" AND bnn_bsc_id != 4";
            }

            $starttime = microtime(true);

            $result = $dbh->exec($sql);
            $endtime = microtime(true);
            $duration = $endtime - $starttime;
            $duration=number_format($duration, 4, ',', '')."s";
            $time="<div class='tip fleft'>".$lang->getLang("Time taken to execute your request").":".$duration."</div><br /><br /><br />";
            ?>
            <?=$time?>
            <?
            if($result->getNumRows()==0){
                addError("",$lang->getLang("No result found"));
                showErrors();
            }
            else
            { ?>
                <table class='table display table-hover table-striped table-bordered' id="bannerTable" >
                    <thead>
                    <tr>
                        <th style="white-space: nowrap"><?=$lang->getLang("Time created")?></th>
                        <th><?=$lang->getLang("Title")?></th>
                        <th><?=$lang->getLang("Subtitle")?></th>
                        <th><?=$lang->getLang("Content")?></th>
                        <th><?=$lang->getLang("Skin")?></th>
                        <th><?=$lang->getLang("Section")?></th>
                        <th><?=$lang->getLang("Banner Action")?></th>
                        <th><?=$lang->getLang("Status")?></th>
                        <th style="white-space: nowrap"><?=$lang->getLang("Start Date")?></th>
                        <th><?=$lang->getLang("End Date")?></th>
                        <th><?=$lang->getLang("Image")?></th>
                        <th><?=$lang->getLang("Action")?></th>
                    </tr>
                    </thead>
                    <tbody>
                    <?
                    while($result->hasNext())
                    {
                        $row=$result->next();
                        ?>
                        <tr id='<?=$row['bnn_id']?>'>
                            <td><?=$row['bnn_time']?></td>
                            <td><?=$row['bnn_title']?></td>
                            <td><?=$row['bnn_subtitle']?></td>
                            <td><?=$row['bnn_body']?></td>
                            <td><?
                                if(strtolower($row['bnn_skn_id'])=='0'){
                                    $row['skn_name']='All';
                                }
                                ?>
                                <?=$row['skn_name']?></td>
                            <td><?=$row['bsc_description']?></td>

                            <td><?=($row['bnn_event_id']!=''?"Event:".$row['bnn_event_id'] :getBannerHref($row['bnn_href_section'],$games,$sections))?></td>
                            <td><span class="<?=($row['bnn_status']=='0') ? "text-error":"text-success"?>">
                                        <?if($row['bnn_status']=='0'){
                                            ?><?=$lang->getLang("Inactive")?>
                                        <?
                                        }
                                        else{
                                            ?>
                                            <?=$lang->getLang("Active")?>
                                        <?
                                        }
                                        ?>
                                    </span></td>
                            <td><?=$row['bnn_start_date']?></td>
                            <td><?=$row['bnn_end_date']?>
                                <?if(strtotime($row['bnn_end_date']) < strtotime('now')){ ?>
                                    <span class="text-error">(<?=$lang->getLang("expired")?>)</span>
                                <? }?>
                            </td>
                            <td>
                                <a href="#myModal" data-toggle="modal" class='preview' data-img-url="/media/banners/<?=($row['skn_id']==0?'default':$skinResrictions[$row['skn_id']][$row['bsc_id']]['type'])?>/<?=$row['bnn_img_path']?>"><?=$row['bnn_img_path']?></a>
                            </td>
                            <td>
                                <?if(($row['skn_name']=='All' && check_access('manage_all_banners')) || $row['skn_name']!='All') { ?>
                                    <div class="btn-group">
                                        <button class="btn btn-primary dropdown-toggle" data-toggle="dropdown"><?=$lang->getLang("Action")?>  <span class="caret"></span></button>
                                        <ul class="dropdown-menu" style="min-width:80px ">
                                            <li class="modifyBanner"><a href="javascript:void(0)"><i class="icon-pencil"></i> <?=$lang->getLang("Modify")?></a></li>
                                            <li class="deleteBanner"><a href="javascript:void(0)"><i class="icon-trash"></i> <?=$lang->getLang("Delete")?></a></li>
                                        </ul>
                                    </div>
                                <? } else { ?>
                                    <?=$lang->getLang("No action available")?>
                                <? } ?>
                            </td>
                        </tr>
                    <?
                    }
                    ?>
                    </tbody>
                </table>
            <?
            }
        }
        else{
            showErrors();
        }
        ?>
    </div>
<?
}
?>
</div>

<div class="tab-pane fade <?=($active=='add') ? "in active":""?>" id="insert">
    <form action="/banners" method="post" enctype="multipart/form-data" id="bannerFormCreator">
        <?php printHiddenInput('doQuery', 'yes');?>
        <?php printHiddenInput('operation', 'add');
        ?>
        <div class='well'>
           <!-- <h4 class="alert alert-info" id="'minimumDimensions">
                <?/*=$lang->getLang("The image has to be at least <span id='bannerWidth'>%</span> * <span id='bannerHeight'> %</span> pixels", BANNERS_WIDTH , BANNERS_HEIGHT)*/?></h4>-->
            <table style="width: 100%">
                <tr><td style="border: 0px;">
                        <div><div class='label label-inverse'><?=$lang->getLang("Title")?></div>
                            <br>
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-align-justify"></i></span>
                                    <input type='text'  name='title'  placeholder="<?=$lang->getLang("Title")?>" value='<?=$title?>' required="required">
                                </div>
                            </div>
                        </div><br />
                        <div><div class='label label-inverse'><?=$lang->getLang("Subtitle")?></div>
                            <br />
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-lock"></i></span>
                                    <input type='text'  name='subtitle'  placeholder="<?=$lang->getLang("Subtitle")?>" required="required" value="<?=$subtitle?>">
                                </div>
                            </div>
                        </div>
                    </td>
                    <td style="border: 0px;">
                        <div><div class='label label-inverse'><?=$lang->getLang("Body")?></div>
                            <br>
                            <div class="controls">
                                <textarea  name='body' id='body' cols="100" rows="6" placeholder="<?=$lang->getLang("Insert  body")?>" ><?=$body?></textarea>
                            </div>
                        </div>
                    </td>
                    <td style="border: 0px;">
                        <div><div class='label label-inverse'><?=$lang->getLang("Date Start")?></div>
                            <br>
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-calendar"></i></span>
                                    <input type='text'  name='date_start' id='date_start' required="required" placeholder="<?=$lang->getLang("The start date")?>" value="<?=$date_start?>">
                                </div>
                            </div>
                        </div>
                        <div><div class='label label-inverse'><?=$lang->getLang("End Date")?></div>
                            <br>
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-calendar"></i></span>
                                    <input type='text'  name='date_end' id='date_end' required="required" placeholder='<?=$lang->getLang("The end date")?>' value="<?=$date_end?>">
                                </div>
                            </div>
                        </div>
                        <div><div class='label label-inverse'><?=$lang->getLang("Status")?></div>
                            <br>
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-random"></i></span>
                                    <select name='status_new' id='status_new'>
                                        <option value='1' <?=($status_new=='1'? 'selected=selected': '')?> ><?=$lang->getLang("Active")?></option>
                                        <option value='0' <?=($status_new=='0'? 'selected=selected': '')?>><?=$lang->getLang("Inactive")?></option>
                                    </select>
                                </div>
                            </div>
                        </div>
                    </td>
                    <td style="border: 0px;">
                        <div><div class='label label-inverse'><?=$lang->getLang("Skin")?></div>
                            <br>
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-globe"></i></span>
                                    <select name="skin" id="skin">
                                        <?if($_SESSION["jurisdiction_class"]=='casino') { ?>
                                        <option  value="0" <?=(0==$skin? 'selected=selected': '')?> ><?=$lang->getLang('All')?></option>
                                        <? } ?>
                                        <?php foreach($skins as $key => $value){
                                            if($key!='0'){
                                                ?>
                                                <option  value="<?=$key?>" <?=($key==$skin? 'selected=selected': '')?> ><?=$value?></option><?php
                                            }
                                        } ?>
                                    </select>
                                </div>
                            </div>
                        </div>
                        <div>
                            <div class='label label-inverse'><?=$lang->getLang("Section")?></div>
                            <br>
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-lock"></i></span>
                                    <select name="section" id="section">
                                        <?php foreach($sections as $key => $value){
                                            if($key==4){
                                                if(check_access('betting_banners')){
                                                    ?>
                                                    <option  value="<?=$key?>" <?=($key==$section? 'selected=selected': '')?> ><?=$value?></option>
                                                <?
                                                }
                                            }
                                            else{
                                                ?>
                                                <option  value="<?=$key?>" <?=($key==$section? 'selected=selected': '')?> ><?=$value?></option><?php
                                            }
                                        } ?>
                                    </select>
                                </div>
                            </div>
                        </div>
                        <div id="bannerImageUploaderContainer">
                            <div><div class='label label-inverse'><?=$lang->getLang("Banner")?></div>
                                <br>
                                <div class="controls">
                                    <input type='file'  name='file' id='file' value='default.inc'  >
                                </div>
                            </div>
                        </div>
                    </td>
                    <td style="border: 0px;">

                        <div><div class='label label-inverse'><?=$lang->getLang("Banner Action")?></div>
                            <br>
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-lock"></i></span>
                                    <select name="bannerActionSelect" id="bannerActionSelect">
                                        <option value='0' <?=(0==$_POST['bannerActionSelect']? 'selected=selected': '')?>><?=$lang->getLang('None')?></option>
                                        <option value='1' <?=(1==$_POST['bannerActionSelect']? 'selected=selected': '')?>><?=$lang->getLang('Site Section')?></option>
                                        <option value='2' <?=(2==$_POST['bannerActionSelect']? 'selected=selected': '')?>><?=$lang->getLang('Open Game')?></option>
                                    </select>
                                </div>
                            </div>
                        </div>
                        <div style="margin-left: 15px"> <div id='h1' class='hidden'><div class='label label-inverse'><?=$lang->getLang("Site Section")?></div>
                                <br>
                                <div class="controls">
                                    <div class="input-prepend">
                                        <span class="add-on"><i class="icon-lock"></i></span>
                                        <select name="hrefb" id="hrefb">
                                            <?php foreach($sections as $key => $value){
                                                ?>
                                                <option  value="<?=$key?>" <?=($key==$_POST['hrefb']? 'selected=selected': '')?> ><?=$value?></option><?php
                                            } ?>
                                        </select>
                                    </div>
                                </div>
                            </div>
                            <div id='h2' class='hidden'><div class='label label-inverse'><?=$lang->getLang("Open Game")?></div>
                                <br>
                                <div class="controls">
                                    <div class="input-prepend">
                                        <span class="add-on"><i class="icon-lock"></i></span>
                                        <select name="hrefg" id="hrefg">
                                            <?php foreach($games as $key => $value){
                                                ?>
                                                <option  value="<?=$key?>" <?=($key==$_POST['hrefg']? 'selected=selected': '')?> ><?=$value?></option><?php
                                            } ?>
                                        </select>
                                    </div>
                                </div>
                            </div>
                        </div>

                    </td>
                </tr>
            </table>
            <br />

            <div class="hidden" id="bettingContainer"><div class='label label-inverse'><?=$lang->getLang("Betting Event Id")?></div>
                <br>
                <div class="control-group">
                    <div class="input-append">
                        <input type='text'  name='betId' id='betId'  placeholder='Bet Id' value="<?=$betId?>">
                        <input type="button" id='getBetEvents' class="btn btn-info" value="Extract!">
                    </div>
                    <select id='eventResults' name='eventResults'></select>
                </div>
            </div>

            <br />

            <input type="hidden" name='bnr' value='<?=$id?>'>
            <button class="btn btn-primary" ><?=$lang->getLang("Save")?></button>
            <br>
        </div>

        <div>
            <?
            if(isset($_POST['doQuery'])&& $_POST['operation']=='add'){

                addBanner($id,$title,$subtitle,$skin,$section,$date_start,$date_end,$status_new,$body,$file,$href,BANNERS_WIDTH,BANNERS_HEIGHT,$betId,$quoteType);
            }
            ?>
        </div>
    </form>
</div>
<?if($view_banner_images){ ?>
    <div class="tab-pane fade <?=($active=='mngImg') ? "in active":""?>" id="mngImg">
        <?if($manage_banner_images) { ?>
            <div class="well">
                <h4 class="alert alert-info"><?=$lang->getLang("The image has to be at least % * % px for best performance",BANNERS_WIDTH,BANNERS_HEIGHT)?></h4>

                <form action="/banners" method="post" enctype="multipart/form-data">
                    <?php printHiddenInput('doQuery', 'yes');?>
                    <?php printHiddenInput('operation', 'mngImg'); ?>
                    <div class="row">
                        <div class="span4"><div class='label label-inverse'><?=$lang->getLang("Image name")?></div>
                            <br>
                            <div class="controls">
                                <div class="input-prepend">
                                    <span class="add-on"><i class="icon-align-justify"></i></span>
                                    <input type='text'  name='imgName'  placeholder="<?=$lang->getLang("Image name")?>" value='' required="required" >
                                </div>
                            </div>
                        </div>
                        <div class="span3"><div class='label label-inverse'><?=$lang->getLang("Image")?></div>
                            <br>
                            <div class="controls">
                                <input type='file'   name='img' id='img' >
                            </div>
                        </div>
                        <div>
                            <img  id="preview" src="#" style='display:none;width: 200px; height:85px' alt="your image" /><br>
                        </div>
                    </div>
                    <br />
                    <div>
                        <input type="submit" class="btn btn-info" value="<?=$lang->getLang("Upload")?>" >
                    </div>
                </form>

                <br />
                <div>
                    <?
                    if(isset($_POST['doQuery'])&& $_POST['operation']=='mngImg'){
                        uploadImg($_FILES['img'],BANNERS_WIDTH,BANNERS_HEIGHT,$_POST['imgName']);
                    }
                    ?>
                </div>
            </div>
        <? } ?>
        <div>

            <div style="width: 60%;margin: auto">
                <table class='table display table-condensed table-hover table-striped table-bordered' >
                    <thead>
                    <tr>
                        <th style="white-space: nowrap"><?=$lang->getLang("Filename")?></th>
                        <th><?=$lang->getLang("Size")?></th>
                        <th><?=$lang->getLang("Time created")?></th>
                        <th><?=$lang->getLang("Last accessed")?></th>
                        <th class="no-sort"><?=$lang->getLang("Preview")?></th>
                        <th class="no-sort"><?=$lang->getLang("Download")?></th>
                    </tr>
                    </thead>
                    <tbody>
                    <?
                    $files=getBannersList();
                    foreach($files as $k=>$v){
                        ?>
                        <tr>
                            <td><?=$k?></td>
                            <td  style="width:80px "><?=formatBytes($v[7])?></td>
                            <td  style="width:140px "><?=date("Y-m-d H:i:s",$v[9])?></td>
                            <td style="width:140px "><?=date("Y-m-d H:i:s",$v[8])?></td>
                            <td  style="width:100px ">
                                <a href="#myModal" data-toggle="modal" class="preview" data-img-url="<?=(defined('BANNERS_SUBDOMAIN')? BANNERS_SUBDOMAIN :'')?>/media/banners/<?=$k?>">
                                    <button class="btn btn-small"><?=$lang->getLang("Preview")?></button>
                                </a>
                            </td>
                            <td  style="width:100px ">
                                <a href="<?="/configuration/download.php?banner=".$k?>"><button class="btn btn-small">
                                        <?=$lang->getLang("Download")?>
                                    </button></a></td>
                        </tr>
                    <?
                    }
                    ?>
                    </tbody>
                </table>
            </div>
        </div>
    </div>
<? } ?>
</div>
</td>
</tr>
</table>
<?php

/**
 * @param $jur_id
 * @return bool|int|RecordSet
 */
function getSkins($jur_id){
    global $dbh;
    $sql="Select * from skins ";
    if($_SESSION["jurisdiction_class"] != "casino"){
        $sql .= "
             JOIN jurisdiction club ON club.jur_id = skn_jur_id
  		     JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
             JOIN jurisdiction regional ON regional.jur_id = district.jur_parent_id
             JOIN jurisdiction nation   ON nation.jur_id   = regional.jur_parent_id";
    }
    switch($_SESSION["jurisdiction_class"]){
        case "club":
            $sql .= " WHERE club.jur_id = $jur_id";
            break;
        case "district":
            $sql .= " WHERE district.jur_id = $jur_id";
            break;
        case "region":
            $sql .= " WHERE regional.jur_id = $jur_id";
            break;
        case "nation":
            $sql .= " WHERE nation.jur_id = $jur_id";
            break;
        case "casino":
            $sql .= " WHERE 1=1";
            break;
        default:
            $sql .= " WHERE 1=0";
            break;
    }
    return $dbh->exec($sql);
}

/**
 * @return bool|int|RecordSet
 */
function getSection(){
    global $dbh;
    $sql="Select * from banner_section  ";
    return $dbh->exec($sql);
}

/**
 * @param $id
 * @param $title
 * @param $subtitle
 * @param $skin
 * @param $section
 * @param $date_start
 * @param $date_end
 * @param $status
 * @param $body
 * @param $file
 * @param int $width
 * @param int $height
 */
function addBanner($id,$title,$subtitle,$skin,$section,$date_start,$date_end,$status,$body,$file,$href,$width=700,$height=300,$event,$quoteType){
    global $dbh,$lang,$skinResrictions,$bannerProps,$allSkindimensions;


    if(is_array($file['file']['name'])) {

        foreach ($file['file']['name'] as $k => $v) {

            if ($v != '') {
                $newFile = true;
            }
        }
    }
    else{
        if($file['file']['name']!=''){
            $newFile = true;
        }
    }

    if($title==''){
        addError('',$lang->getLang('Please enter a title'));
    }
    if($subtitle==''){
        addError('',$lang->getLang('Please enter a subtitle'));
    }
    if($skin==''){
        addError('',$lang->getLang('Please choose the skin'));
    }
    if($section==''){
        addError('',$lang->getLang('Please choose the section'));
    }
    if($date_start==''){
        addError('',$lang->getLang('Please choose a start date'));
    }
    if($date_end==''){
        addError('',$lang->getLang('Please choose a end date'));
    }
    if($status==''){
        addError('',$lang->getLang('Please choose the status'));
    }
    if($id==''){
        if(!$newFile){
            addError("",$lang->getLang("Please choose a file"));
        }
    }
  /*  if($section==4){
        if($event!=''){
            if($quoteType=='' || $quoteType==0){
                addError("",$lang->getLang("Please select the type of the quote!"));
            }
        }

    }*/

    if($newFile){
        if($skin==0){
            foreach($_FILES['file']['name'] as $k=>$v){
                if(!isset($name)){
                    $name = $v;
                }
                if ((($_FILES["file"]["type"][$k] == "image/gif") || ($_FILES["file"]["type"][$k] == "image/jpeg") || ($_FILES["file"]["type"][$k] == "image/jpg") || ($_FILES["file"]["type"][$k] == "image/png")))
                {
                    if ($_FILES["file"]["error"][$k] > 0)
                    {
                        addError(""," Error:". $_FILES["file"]["error"][$k]);
                    }
                    else{
                        $types=explode(',',$allSkindimensions['structures'][$k]);

                        foreach($types as $keyType=>$type){
                            $width=$bannerProps[$type]['width'];
                            $height=$bannerProps[$type]['height'];
                            $filename = BANNERS."/".$type."/".$name;
                            if(!is_dir(dirname($filename))){
                                mk_dir(dirname($filename));
                            }
                            list($width_old, $height_old) = getimagesize($_FILES["file"]["tmp_name"][$k]);
                            $thumb = imagecreatetruecolor($width, $height);
                            if($_FILES["file"]["type"][$k] == "image/gif"){
                                $source = imagecreatefromgif($_FILES['file']['tmp_name'][$k]);
                                imagecopyresized($thumb, $source, 0, 0, 0, 0, $width, $height, $width_old, $height_old);
                                imagegif($thumb,$filename);
                            }
                            if($_FILES["file"]["type"][$k] == "image/jpeg" || $_FILES["file"]["type"][$k] == "image/jpg"){
                                $source = imagecreatefromjpeg($_FILES['file']['tmp_name'][$k]);
                                imagecopyresized($thumb, $source, 0, 0, 0, 0, $width, $height, $width_old, $height_old);
                                imagejpeg($thumb,$filename);
                            }
                            if($_FILES["file"]["type"][$k] == "image/png"){
                                $source = imagecreatefrompng($_FILES['file']['tmp_name'][$k]);
                                imagecopyresized($thumb, $source, 0, 0, 0, 0, $width, $height, $width_old, $height_old);
                                imagepng($thumb,$filename);

                            }

                            if(isset($_FILES['file']['error'][$k])){
                                switch ($_FILES['file']['error'][$k]){
                                    case UPLOAD_ERR_OK:
                                        break;
                                    case UPLOAD_ERR_INI_SIZE:
                                        addError('',$lang->getLang("The file is too big.Please choose a smaller one!"));
                                        break;
                                    case UPLOAD_ERR_PARTIAL:
                                        addError('',$lang->getLang("File uploaded partially"));
                                        break;
                                    case UPLOAD_ERR_NO_FILE:
                                        addError('',$lang->getLang("No file uploaded"));
                                        break;
                                    case UPLOAD_ERR_NO_TMP_DIR:
                                        addError('',$lang->getLang("Missing temporary folder.File not uploaded"));
                                        break;
                                    case UPLOAD_ERR_CANT_WRITE:
                                        addError('',$lang->getLang("Permissions denied to upload file.The file was not uploaded!"));
                                        break;
                                    case UPLOAD_ERR_EXTENSION:
                                        addError('',$lang->getLang("Wrong extension.The file was not uploaded!"));
                                        break;

                                    default:
                                        addError('',$lang->getLang("Unknown error.File not uploaded"));
                                }
                            }
                            if(!file_exists($filename)){
                                addError('',$lang->getLang("Permissions denied.Photo not uploaded"));
                            }
                        }
                    }
                    }
                else
                {
                    addError("",$lang->getLang("Invalid file!Please choose another."));
                }
            }
        }
        elsE{
            if ((($_FILES["file"]["type"] == "image/gif") || ($_FILES["file"]["type"] == "image/jpeg") || ($_FILES["file"]["type"] == "image/jpg") || ($_FILES["file"]["type"] == "image/png")))
            {
                if ($_FILES["file"]["error"] > 0)
                {
                    addError(""," Error Code:". $_FILES["file"]["error"]);
                }
                else{
                    $name = $_FILES["file"]["name"];
                    $width=isset($skinResrictions[$skin][$section]['width'])? $skinResrictions[$skin][$section]['width'] : $skinResrictions['default'][$section]['width'];
                    $height=isset($skinResrictions[$skin][$section]['height'])? $skinResrictions[$skin][$section]['height'] : $skinResrictions['default'][$section]['height'];
                    $type=isset($skinResrictions[$skin][$section]['type'])? $skinResrictions[$skin][$section]['type'] : $skinResrictions['default'][$section]['type'];
                    $filename = BANNERS."/".$type."/".$_FILES['file']['name'];
                    if(!is_dir(dirname($filename))){
                        mk_dir(dirname($filename));
                    }
                    list($width_old, $height_old) = getimagesize($_FILES["file"]["tmp_name"]);
                    if($width_old<$width){
                        addError('','Image width is less than '.$width);
                    }
                    if($height_old<$height){
                        addError('','Image height is less than '.$height);
                    }
                    $thumb = imagecreatetruecolor($width, $height);
                    if($_FILES["file"]["type"] == "image/gif"){
                        $source = imagecreatefromgif($_FILES['file']['tmp_name']);
                        imagecopyresized($thumb, $source, 0, 0, 0, 0, $width, $height, $width_old, $height_old);
                        imagegif($thumb,$filename);
                    }
                    if($_FILES["file"]["type"] == "image/jpeg" || $_FILES["file"]["type"] == "image/jpg"){
                        $source = imagecreatefromjpeg($_FILES['file']['tmp_name']);
                        imagecopyresized($thumb, $source, 0, 0, 0, 0, $width, $height, $width_old, $height_old);
                        imagejpeg($thumb,$filename);
                    }
                    if($_FILES["file"]["type"] == "image/png"){
                        $source = imagecreatefrompng($_FILES['file']['tmp_name']);
                        imagecopyresized($thumb, $source, 0, 0, 0, 0, $width, $height, $width_old, $height_old);
                        imagepng($thumb,$filename);

                    }

                    if(isset($_FILES['file']['error'])){
                        switch ($_FILES['file']['error']){
                            case UPLOAD_ERR_OK:
                                break;
                            case UPLOAD_ERR_INI_SIZE:
                                addError('',$lang->getLang("The file is too big.Please choose a smaller one!"));
                                break;
                            case UPLOAD_ERR_PARTIAL:
                                addError('',$lang->getLang("File uploaded partially"));
                                break;
                            case UPLOAD_ERR_NO_FILE:
                                addError('',$lang->getLang("No file uploaded"));
                                break;
                            case UPLOAD_ERR_NO_TMP_DIR:
                                addError('',$lang->getLang("Missing temporary folder.File not uploaded"));
                                break;
                            case UPLOAD_ERR_CANT_WRITE:
                                addError('',$lang->getLang("Permissions denied to upload file.The file was not uploaded!"));
                                break;
                            case UPLOAD_ERR_EXTENSION:
                                addError('',$lang->getLang("Wrong extension.The file was not uploaded!"));
                                break;

                            default:
                                addError('',$lang->getLang("Unknown error.File not uploaded"));
                        }
                    }
                    if(!file_exists($filename)){
                        addError('',$lang->getLang("Permissions denied.Photo not uploaded"));
                    }
                }
            }
    else
        {
            addError("",$lang->getLang("Invalid file!Please choose another."));
        }
    }
    }
    if(!areErrors()){
        if($id==''){
            $sql="INSERT INTO banner(BNN_TITLE,BNN_SUBTITLE,BNN_SKN_ID,BNN_BSC_ID,BNN_START_DATE,BNN_END_DATE,BNN_STATUS,BNN_BODY,BNN_IMG_PATH,BNN_HREF_SECTION";

            if($section==4){
                $sql.=",BNN_EVENT_ID,BNN_ODDS_ID";
            }
            $sql.=")VALUES('".$title."','".$subtitle."','".$skin."','".$section."','".$date_start."','".$date_end."','".$status."','".$body."','".$name."','".$href."'";
            if($section==4){
                $sql.=",'".$event."','".$quoteType."'";
            }
            $sql.=")";
        }
        else{
            $sql="UPDATE banner SET BNN_TITLE='".$dbh->escape($title)."',
                                    BNN_SUBTITLE='".$dbh->escape($subtitle)."',
                                    BNN_SKN_ID=".$dbh->escape($skin).",
                                    BNN_BSC_ID=".$dbh->escape($section).",
                                    BNN_START_DATE='".$dbh->escape($date_start)."',
                                    BNN_END_DATE='".$dbh->escape($date_end)."',
                                    BNN_STATUS=".$dbh->escape($status).",
                                    BNN_BODY='".$dbh->escape($body)."',
                                    BNN_HREF_SECTION='".$dbh->escape($href)."'
                                     ";

            if($section==4 && $quoteType!='' && $quoteType!=0){
                $sql.=",BNN_EVENT_ID='".$dbh->escape($event)."',
                      BNN_ODDS_ID='".$dbh->escape($quoteType)."' ";
            }
            if($newFile){
                $sql.=",BNN_IMG_PATH='".$dbh->escape($file['file']['name'])."'";
            }
            $sql.=" WHERE BNN_ID=".$dbh->escape($id);
        }
        $res=$dbh->exec($sql);
        if($res>0){
            if($id==''){
                ?>
                <h4 class="alert alert-info fade in"><button type='button' class='close' data-dismiss='alert'>&times;</button><?=$lang->getLang('You have successfully added a banner');?></h4>
            <?
            }
            else{
                ?>
                <h4 class="alert alert-info fade in"><button type='button' class='close' data-dismiss='alert'>&times;</button><?=$lang->getLang('You have successfully updated a banner');?></h4>
            <?
            }
            ?>
            <script>

                $('#bannerFormCreator').find("input[type=text],input[type=hidden],select, textarea").val("");

            </script>
        <?
        }
        elseif(areErrors())
        {
            unlink($filename);
            ?>
            <h2 class="span10 error"><?=showErrors();?></h2>
        <?
        }
        else{
            unlink($filename);

            ?>
            <h4 class="alert alert-error fade in"><button type='button' class='close fade in' data-dismiss='alert'>&times;</button><?=$lang->getLang('An error has occurred.Banner not inserted');?></h4>
        <?
        }
    }else{
        unlink($filename);
        ?>
        <span class="span10">
           <?
           showErrors();
           ?>
           </span>
    <?
    }
    return;
}

/**
 * @param $href
 * @param $games
 * @param $sections
 * @return string
 */
function getBannerHref($href,$games,$sections){
    if($href==''){
        $return='none';
    }
    else{
        list($hrefType,$hrefAction)=explode(',',$href);
        if($href==1){
            $return='Section : '.$sections[$hrefAction];
        }
        elseif($href=2){
            $return='Game: '. $games[$hrefAction];
        }
    }
    return $return;
}

/**
 * @return array
 */
function getBannersList(){

    $files = array_diff(scandir(BANNERS), array('..', '.'));
    $list=array();
    foreach($files as $k=>$v){
        if(!is_dir(BANNERS."/".$v)){
            $list[$v]=stat(BANNERS."/".$v);
        }
    }
    return $list;

}

/**
 * @param $bytes
 * @param int $precision
 * @return string
 */
function formatBytes($bytes, $precision = 2) {
    $units = array('B', 'KB', 'MB', 'GB', 'TB');

    $bytes = max($bytes, 0);
    $pow = floor(($bytes ? log($bytes) : 0) / log(1024));
    $pow = min($pow, count($units) - 1);
    $bytes /= pow(1024, $pow);

    return round($bytes, $precision) . ' ' . $units[$pow];
}

/**
 * @param $img
 * @param int $width
 * @param int $height
 * @param bool $newname
 */
function uploadImg($img,$width=700,$height=300,$newname=false){
    global $lang;
    $err=false;
    if(!defined("BANNERS")){
        define('BANNERS',WEB_ROOT.'/admin/media/banners');
    }
    if($img!=''){
        if ((($_FILES["img"]["type"] == "image/gif") || ($_FILES["img"]["type"] == "image/jpeg") || ($_FILES["img"]["type"] == "image/jpg") || ($_FILES["img"]["type"] == "image/png")))
        {
            if ($_FILES["img"]["error"] > 0)
            {
                ?>
                <h4 class="alert alert-error fade in"><button type='button' class='close fade in' data-dismiss='alert'>&times;</button><?=$lang->getLang('Error Code:'. $_FILES["img"]["error"]);?></h4>
                <?
                $err=true;
            }
            else{
                if($newname){
                    $filename = BANNERS."/$newname".".".end(explode(".", $_FILES['img']['name']));
                }
                else{
                    $filename = BANNERS."/".$_FILES['img']['name'];
                }
                list($width_old, $height_old) = getimagesize($_FILES["img"]["tmp_name"]);
                if($width_old<$width){
                    ?>
                    <h4 class="alert alert-error fade in"><button type='button' class='close fade in' data-dismiss='alert'>&times;</button><?=$lang->getLang("Image width is less than".BANNERS_WIDTH);?></h4>
                    <?
                    $err=true;
                }
                if($height_old<$height){
                    ?>
                    <h4 class="alert alert-error fade in"><button type='button' class='close fade in' data-dismiss='alert'>&times;</button><?=$lang->getLang('Image height is less than'.BANNERS_HEIGHT);?></h4>
                    <?
                    $err=true;
                }
                $thumb = imagecreatetruecolor($width, $height);
                if($_FILES["img"]["type"] == "image/gif"){
                    $source = imagecreatefromgif($_FILES['img']['tmp_name']);
                    imagecopyresized($thumb, $source, 0, 0, 0, 0, $width, $height, $width_old, $height_old);
                    imagegif($thumb,$filename);
                }
                if($_FILES["img"]["type"] == "image/jpeg" || $_FILES["img"]["type"] == "image/jpg"){
                    $source = imagecreatefromjpeg($_FILES['img']['tmp_name']);
                    imagecopyresized($thumb, $source, 0, 0, 0, 0, $width, $height, $width_old, $height_old);
                    imagejpeg($thumb,$filename);
                }
                if($_FILES["img"]["type"] == "image/png"){
                    $source = imagecreatefrompng($_FILES['img']['tmp_name']);
                    imagecopyresized($thumb, $source, 0, 0, 0, 0, $width, $height, $width_old, $height_old);
                    imagepng($thumb,$filename);

                }

                if(isset($_FILES['img']['error'])){
                    switch ($_FILES['img']['error']){
                        case UPLOAD_ERR_OK:
                            break;
                        case UPLOAD_ERR_INI_SIZE:
                            ?>
                            <h4 class="alert alert-error fade in"><button type='button' class='close fade in' data-dismiss='alert'>&times;</button><?=$lang->getLang('The file is too big.Please choose a smaller one!');?></h4>
                            <?
                            $err=true;
                            break;
                        case UPLOAD_ERR_PARTIAL:
                            ?>
                            <h4 class="alert alert-error fade in"><button type='button' class='close fade in' data-dismiss='alert'>&times;</button><?=$lang->getLang('File uploaded partially');?></h4>
                            <?
                            $err=true;
                            break;
                        case UPLOAD_ERR_NO_FILE:
                            ?>
                            <h4 class="alert alert-error fade in"><button type='button' class='close fade in' data-dismiss='alert'>&times;</button><?=$lang->getLang('No file uploaded');?></h4>
                            <?
                            $err=true;
                            break;
                        case UPLOAD_ERR_NO_TMP_DIR:
                            ?>
                            <h4 class="alert alert-error fade in"><button type='button' class='close fade in' data-dismiss='alert'>&times;</button><?=$lang->getLang('Missing temporary folder.File not uploaded');?></h4>
                            <?
                            $err=true;
                            break;
                        case UPLOAD_ERR_CANT_WRITE:
                            ?>
                            <h4 class="alert alert-error fade in"><button type='button' class='close fade in' data-dismiss='alert'>&times;</button><?=$lang->getLang('Permissions denied to upload file.The file was not uploaded!');?></h4>
                            <?
                            $err=true;
                            break;
                        case UPLOAD_ERR_EXTENSION:
                            ?>
                            <h4 class="alert alert-error fade in"><button type='button' class='close fade in' data-dismiss='alert'>&times;</button><?=$lang->getLang('Wrong extension.The file was not uploaded!');?></h4>
                            <?
                            $err=true;
                            break;

                        default:
                            ?>
                                <h4 class="alert alert-error fade in"><button type='button' class='close fade in' data-dismiss='alert'>&times;</button><?=$lang->getLang('Unknown error.File not uploaded');?></h4>
                            <?
                            $err=true;
                    }
                }
                if(!file_exists($filename)){
                    ?>
                    <h4 class="alert alert-error fade in"><button type='button' class='close fade in' data-dismiss='alert'>&times;</button><?=$lang->getLang('Permissions denied.Photo not uploaded');?></h4>
                    <?
                    $err=true;
                }
            }
        }
        else
        {
            ?>
            <h4 class="alert alert-error fade in"><button type='button' class='close fade in' data-dismiss='alert'>&times;</button><?=$lang->getLang('Invalid file!Please choose another.');?></h4>
            <?
            $err=true;
        }
    }
    else{
        $err=true;
        die(var_dump($img));
    }
    if(!$err){
        ?>
        <h4 class="alert alert-success fade in"><button type='button' class='close' data-dismiss='alert'>&times;</button><?=$lang->getLang('You have successfully added a photo');?></h4>
    <?
    }
}

?>
</div>

</div>

<? function reArrayFiles(&$file_post) {

    $file_ary = array();
    $file_count = count($file_post['name']);
    $file_keys = array_keys($file_post);

    for ($i=0; $i<$file_count; $i++) {
        foreach ($file_keys as $key) {
            $file_ary[$i][$key] = $file_post[$key][$i];
        }
    }

    return $file_ary;
}
?>