<?check_access('financial_onjn',true);
/**
 * @author Marian
 */
include_once 'JurisdictionsList.class.inc';
$jurClass = JurisdictionsList::getInstance ( $dbh );
$hierarchy = $jurClass->getJurisdictionTree($_SESSION['jurisdiction_id']);
?>
    <link rel="stylesheet" type="text/css" href="/media/jquery.multiselect.css" title="core_style">
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/dataTablesExtra/TableTools-2.2.0/css/dataTables.tableTools.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
    <style>
        #companies{
            width: 270px !important;
        }
        .ui-multiselect-checkboxes label input {
            vertical-align: inherit;
            margin: 5px;
        }
        .ui-multiselect{
            max-width: none;
        }
        #searchTable{
            background-color: transparent !important;
            width:440px;
            margin: auto;
        }

    </style>
    <script src="/media/jquery.multiselect.js" type="text/javascript"></script>
    <script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
    <script src="/media/jquery.dataTables.js" type="text/javascript"></script>
    <script src="/media/jquery.alerts.js" type="text/javascript"></script>
    <script src="/media/dataTablesExtra/TableTools-2.2.0/js/dataTables.tableTools.js" type="text/javascript"></script>

    <div class="container" style="position: absolute;left:0;right: 0;width: 100%" >
    <?php
    require_once "financial_casino/financial_functions.inc";
    global $lang;
    $date_start = (isset($_POST['date_start'])) ? $_POST['date_start'] : date("Y-m-d", time() - (60 * 60 * 24));
    $date_end = (isset($_POST['date_end'])) ? $_POST['date_end'] : date("Y-m-d");
    $defaultmonth= (isset($_POST['monthHelper'])) ? $_POST['monthHelper'] : false;
    $defaultweek= (isset($_POST['weekHelper'])) ? $_POST['weekHelper'] : false;
    $companies=(isset($_POST['companies']) ? $_POST['companies'] : false);
    $dstr=(($_POST['dstr']!='') ? explode(',',$_POST['dstr']) : array());

    ?>
    <script type="text/javascript">
        dstr=[<?=$_POST['dstr']?>];

        $(function() {


            $('#companies').multiselect({
                selectedList: 4,
                noneSelectedText:"Select an agency",
                click: function(event, ui){
                    dstr=jQuery.grep(dstr, function(value) {
                        return value != $('#companies option[value="'+ui.value+'"]').parent().attr('value');
                    });
                    $('#dstr').val(dstr);
                },
                uncheckAll: function(event, ui){
                    dstr=[];
                    $('#dstr').val('');
                },
                optgrouptoggle: function(event, ui){
                    if(ui.checked){
                        dstr.push(ui.grpvalue);
                    }
                    else{
                        dstr=jQuery.grep(dstr, function(value) {
                            return value != ui.grpvalue;
                        });
                    }
                    $('#dstr').val(dstr);
                }
            }).multiselectfilter();
            $('.ui-multiselect').addClass('btn');
        });

    </script>
    <center>
        <table class="table table-bordered" style="margin: auto;width: auto">
            <tr>
                <td class="navbar-inner" colspan="2"><h3 class="text-center"> <?=$lang->getLang("User credits")?></h3></td>
            </tr>
            <tr>
                <td style='vertical-align: top'>
                    <div class="well">
                        <br/>
                        <p>
                        <form action="/user_credits" method="post">
                            <?php printHiddenInput('doQuery', 'yes');?>
                            <?php printHiddenInput('search_type', 'date_range');
                            global $lang;
                            ?>
                            <input type="hidden" id="dstr" name="dstr"  value="<?=$_POST['dstr']?>" />
                            <table id="searchTable" class="table table-bordered">
                                <tbody>
                                <tr>
                                    <td><div class='label label-inverse'><?=$lang->getLang("Club")?></div></td>
                                    <td>
                                        <?
                                        $result=getAllClubsAndDistricts();
                                        $districts=array();
                                        while($result->hasNext()){
                                            $row=$result->next();
                                            if(!is_array($districts[$row['dis_jur_id']])){
                                                $districts[$row['dis_jur_id']]=array();
                                                $districts[$row['dis_jur_id']]['name']=$row['dis_jur_name'];
                                            }
                                            $districts[$row['dis_jur_id']]['clubs'][$row['cl_jur_id']]=$row['cl_jur_name'] ;
                                        }
                                        ?>
                                        <select name="companies[]" id="companies" multiple="multiple" style="display:none">
                                            <?foreach($districts as $district=>$clubs){ ?>
                                            <?if($_SESSION['jurisdiction_class']!='club') { ?>
                                            <optgroup label="<?=$clubs['name']?>" value="<?=$district?>"> <? }
                                                foreach($clubs['clubs'] as $club=>$v){ ?>
                                                    <option value="<?=$club?>"   <?=(in_array($club,$companies)===false && in_array($clubs['id'],$dstr)===false)  ? '' : 'selected'?>><?=$v?></option>
                                                <?
                                                }
                                                } ?>
                                        </select>
                                    </td>
                                </tr>
                                <tr><td colspan="2"><input type="submit" class="btn btn-primary" value="<?=$lang->getLang("Submit")?>" ></td> </tr>
                                </tbody>
                            </table>

                            <div class="clearfix"></div>
                    </div>
                    <div>
                        <?if(isset($_POST['doQuery'])) {
                            if($companies==''){
                                ?>
                                <h4 class="alert alert-error fade in"><button type='button' class='close fade in' data-dismiss='alert'>&times;</button><?=$lang->getLang('Please select at least one agency');?></h4>
                            <?
                            }else{
                                $companies=implode(",",$companies);
                                $sql="SELECT count(pun_id) from punter,punter_credit,jurisdiction
                                      where pun_id=pcr_pun_id
                                      AND pun_betting_club=jur_id
                                      AND pun_betting_club in ($companies)";
                                $totals=$dbh->queryOne($sql);
                                if($totals>0){
                                    ?>
                                    <script>
                                        $(document).ready( function() {
                                            $('#userCredits').dataTable( {
                                                "bProcessing": true,
                                                "bServerSide": true,
                                                "bSort":false,
                                                "sPaginationType":"full_numbers",
                                                "bFilter":false,
                                                "sAjaxSource": "services/general_services.inc",
                                                "fnServerParams": function ( aoData ) {
                                                    aoData.push( { "name": "action", "value": '9' },
                                                        { "name": "totals", "value": '<?=$totals?>' },
                                                        { "name": "clubs", "value":"<?=$companies?>" });
                                                }
                                            });
                                        });
                                    </script>



                                    <table class="table table-bordered table-striped display" id="userCredits">
                                        <thead>
                                        <tr>
                                            <th><?=$lang->getLang('User')?></th>
                                            <th><?=$lang->getLang('Club')?></th>
                                            <th><?=$lang->getLang('Credits')?></th>
                                        </tr></thead>
                                        <tbody>
                                        </tbody></table> <br /><br />
                                <? }
                                else
                                {
                                    ?>
                                    <h4 class="alert alert-error fade in"><button type='button' class='close fade in' data-dismiss='alert'>&times;</button><?=$lang->getLang('No result found');?></h4>
                                <?}
                            }
                        }
                        ?>
                    </div>
                </td>
            </tr>
        </table>
    </center>
    </div>
