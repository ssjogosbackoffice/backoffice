<?php
$internal=check_access('manage_games',$redirect=true);
include_once 'game_functions.inc';
/**
 * @author Marian
 */
if (isset ( $_POST ['add'] )) {
    $name = $_POST ['name'];
    $id= $_POST ['id'];
    $minbet=  $_POST ['minbet'];
    $maxbet= $_POST ['maxbet'];
    $maxpayout = $_POST ['maxpayout'];
    $minposbet= $_POST ['minposbet'];
    $maxposbet= $_POST ['maxposbet'];
    $extcode= $_POST ['extcode'];
    $category= $_POST ['category'];
    $type= $_POST ['type'];
    $localization= $_POST ['localization'];
    addGame($id,$name,$minbet,$maxbet,$maxpayout,$minposbet,$maxposbet,$extcode,$category,$type,$localization);
} else {
    if ($_POST ['updel'] == '1') {
        $name = $_POST ['name'];
        $id= $_POST ['id'];
        $minbet=  $_POST ['minbet'];
        $maxbet= $_POST ['maxbet'];
        $maxpayout = $_POST ['maxpayout'];
        $minposbet= $_POST ['minposbet'];
        $maxposbet= $_POST ['maxposbet'];
        $extcode= $_POST ['extcode'];
        $category= $_POST ['category'];
        $type= $_POST ['type'];
        $localization= $_POST ['localization'];
        $update = updateGame($id,$name,$minbet,$maxbet,$maxpayout,$minposbet,$maxposbet,$extcode,$category,$type,$localization);
        ?>
        <script>
            $(function() {
                showUpdate('<?=$id?>');
                <?php if($update=='1'){?>
                jAlert('<?=$lang->getLang("You have successfully updated the game")?> <?=$name?>');
                <?php } elseif ($update=='0'){ ?>
                jAlert("<?=$lang->getLang("You haven't made modifies")?>");
                <?php } else {?>
                jAlert('<?=$update?>');
                <?php }?>
            });
        </script>
    <?php
    }
    if ($_POST ['updel'] == '2') {
        $id = $_POST ['id'];
        delGame( $id );
        ?>
        <script>
            $(function() {
                jAlert('<?=$lang->getLang("You have successully deleted a game")?>');
            });
        </script>
    <?php
    }
    ?>
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
    <link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap-responsive.css" title="core_style" />
    <script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
    <script src="media/jquery.alerts.js" type="text/javascript"></script>
    <script type="text/javascript">
        function showAddPart(){
            $("#add_part").show('slow');
            $("#part").show('slow');
        }
        $(function() {
            $("form").submit( function(event, skip) {
                if (skip) {
                    return true;
                }
                event.preventDefault();
                var self = this;
                jConfirm(
                    '<?=$lang->getLang("Are you sure you want to do this?")?>',
                    '<?=$lang->getLang("Confirm")?>',
                    function(r) {
                        if (r) {
                            $(self).trigger('submit', true);
                        }
                    }
                );
            });
        });



        function addPart(){
            $.post("configuration/game.inc",{add:'1',
                id:$('#id').val(),
                name:$('#name').val(),
                minbet:$('#minbet').val(),
                maxbet:$('#maxbet').val(),
                maxpayout:$('#maxpayout').val(),
                minposbet:$('#minposbet').val(),
                maxposbet:$('#maxposbet').val(),
                extcode:$('#extcode').val(),
                category:$('#category').val(),
                type:$('#type').val(),
                localization:$('#localization').val() }, function(data){
                if(data=='2'){
                    $('#addResult').removeClass().addClass("result");
                    $('#addResult').empty().append('<?=$lang->getLang("Successfully added game!")?>' );
                }
                else if(data=='1')
                {
                    $('#addResult').removeClass().addClass("error");
                    $('#addResult').empty().append( '<?=$lang->getLang("Game already exists!")?>');
                }
                else
                {
                    $('#addResult').removeClass().addClass("error");
                    $('#addResult').empty().append( '<?=$lang->getLang("An error has occurred.Please try again!")?>');
                }
            });
        }





        function hideAddPart()
        {
            $("#part").hide('slow');
            $("#add_part").hide('slow');
        }

        function showModifyPart(){
            $("#add_part").hide("slow");
            toggle("modify_part");
        }

        function showUpdate(id){
            $(".update").hide('slow');
            show(id+'bg');
            show(id);
            $("html, body").animate({scrollTop:0}, 'slow');
        }
        function hide(id){
            $("#"+id).hide('slow');
        }

        function show(id){
            $("#"+id).show('slow');
        }
        function toggle(id)
        {
            if ($('#'+id).is(":hidden")) {
                $('#'+id).show('slow');
            } else {
                $('#'+id).hide('slow');
            }
            return false;
        }

        function selectAllGames(id)
        {
            $("#"+id).hide();
            $('#'+id).find(':checkbox').each(function() {
                this.checked = true;
            });
        }
        function deselectAllGames(id)
        {
            $('#'+id).find(':checkbox').each(function() {
                this.checked = false;
            });
            $("#"+id).show('slow');
        }
    </script>
    <div class="centered"><h1 class='text-error'><?=$lang->getLang("Manage Games")?></h1></div><br>
    <div>
        <div id="add_part"></div>
        <div id='part'>
            <a href="<?=refPage('configuration/game','init')?>"><button type="button"
                                                                        class="close" style="color: darkblue">x</button></a>
            <div class="bodyHD"><?=$lang->getLang("Add a new game")?></div><br>
            <table style='margin: auto; text-align: left'>
                <tr>
                    <td class="alert alert-info"><?=$lang->getLang("Id")?></td>
                    <td><input type="text" id="id" value=''></td>
                </tr>
                <tr>
                    <td class="alert alert-info"><?=$lang->getLang("Name")?></td>
                    <td><input type="text" id="name" value=''></td>
                </tr>
                <tr>
                    <td class="alert alert-info"><?=$lang->getLang("Min Bet")?></td>
                    <td><input type="text" id="minbet" value=''></td>
                </tr>
                <tr>
                    <td class="alert alert-info"><?=$lang->getLang("Max Bet")?></td>
                    <td><input type="text" id="maxbet" value=''></td>
                </tr>
                <tr>
                    <td class="alert alert-info"><?=$lang->getLang("Max Payout")?></td>
                    <td><input type="text" id='maxpayout' ></td>
                </tr>
                <tr>
                    <td class="alert alert-info"><?=$lang->getLang("Min Pos Bet")?></td>
                    <td><input type="text"  id='minposbet' value=''></td>
                </tr>
                <tr>
                    <td class="alert alert-info"><?=$lang->getLang("Max Pos Bet")?></td>
                    <td><input type="text"  id='maxposbet' value=''></td>
                </tr>
                <tr>
                    <td class="alert alert-info"><?=$lang->getLang("Ext Code")?></td>
                    <td><input type="text" id='extcode' value=''></td>
                </tr>
                <tr>
                    <td class="alert alert-info"><?=$lang->getLang("Category")?></td>
                    <td><select name='category' id='category'><?
                            $categories= getGameCategories();
                            while ( $categories->hasNext () ) {
                                $category = $categories->next ();
                                ?><option value='<?=$category['gct_name']?>'><?=$category['gct_name']?></option>
                            <?php }?>
                        </select></td>
                </tr>
                <tr>
                    <td class="alert alert-info"><?=$lang->getLang("Type")?></td>
                    <td><select name='type' id='type'><?
                            $types= getGameType();
                            while ( $types->hasNext () ) {
                                $type= $types->next ();
                                ?><option value='<?=$type['gte_name']?>'><?=$type['gte_name']?></option>
                            <?php }?>
                        </select></td>
                </tr>
                <tr>
                    <td class="alert alert-info"><?=$lang->getLang("Localization")?></td>
                    <td><input type="text"  id='localization' value=''></td>
                </tr>
            </table><br>
            <div>
                <button id='add' class="btn btn-primary"
                        onclick='addPart()'><?=$lang->getLang("Add")?></button><br>
                <div id="addResult"></div>
            </div>

        </div>
    </div>
    <div id="partners">
        <div><button class='btn btn-primary' style='float: left'
                     onclick='showAddPart()'><?=$lang->getLang("Add a new game")?></button></div><br><br><br>
        <table class="table table-striped">
            <thead>
            <tr>
                <th class='alert alert-info'><?=$lang->getLang("Id")?></th>
                <th class='alert alert-info'><?=$lang->getLang("Name")?></th>
                <th class='alert alert-info'><?=$lang->getLang("Min Bet")?></th>
                <th class='alert alert-info'><?=$lang->getLang("Max Bet")?></th>
                <th class='alert alert-info'><?=$lang->getLang("Max Payout")?></th>
                <th class='alert alert-info'><?=$lang->getLang("Min Pos Bet")?></th>
                <th class='alert alert-info'><?=$lang->getLang("Max Pos Bet")?></th>
                <th class='alert alert-info'><?=$lang->getLang("Ext Code")?></th>
                <th class='alert alert-info'><?=$lang->getLang("Category")?></th>
                <th class='alert alert-info'><?=$lang->getLang("Type")?></th>
                <th class='alert alert-info'><?=$lang->getLang("Localization")?></th>
                <th class='alert alert-info'><?=$lang->getLang("Action")?></th>
            </tr>
            </thead>
            <tbody>
            <?php
            $rs = getGamesList();
            while ( $rs->hasNext () ) {
                $row = $rs->next ();
                ?>
                <tr>
                    <td><?=$row['gam_id'];?></td>
                    <td><strong><?=$row['gam_name']?></strong></td>
                    <td><?=$row['gam_min_bet'];?></td>
                    <td><?=$row['gam_max_bet']?></td>
                    <td><?=$row['gam_max_payout']?></td>
                    <td><?=$row['gam_min_pos_bet']?></td>
                    <td><?=$row['gam_max_pos_bet']?></td>
                    <td><?=$row['gam_ext_code']?></td>
                    <td><?=$row['gam_category']?></td>
                    <td><?=$row['gam_type']?></td>
                    <td><?=$row['gam_localization']?></td>
                    <td style="min-width: 70px;"><img  class="imgUpd"
                                                       style="width: 25px; height: 25px; vertical-align: bottom; cursor: pointer; float: left;"
                                                       src="live/liveimage/edit.png"
                                                       onclick="showUpdate('<?=$row['gam_id']?>')"><?php /*form_head()*/?><!--
                        <input type="hidden" name='id' value='<?/*=$row['gam_id']*/?>'> <input   type="hidden" name='updel' value='2'> <input type="image" class="imgUpd"  src="media/images/delete.jpg" alt="Delete this game"
                                                                         style="width: 35px; height: 25px;margin-left: 5px; border-radius: 7px;" />
                        </form>-->
                        <div class="partnersbg" id="<?=$row['gam_id']?>bg"></div>
                        <div class='update' id="<?=$row['gam_id']?>">
                            <a href="<?=refPage('configuration/game','init')?>"><button
                                    type="button" class="close" style="color: darkblue">x</button></a>
                            <?php form_head()?>
                            <input  type="hidden" name='updel' value='1'>
                            <input  type="hidden" name='id' value='<?=$row['gam_id']?>'>
                            <table style='margin: auto; float: left; text-align: left'>
                                <tr>
                                    <td class="bodyHD" colspan="2"><?=$lang->getLang("Update game")?> <?=$row['gam_name']?> </td>
                                </tr>
                                <tr>
                                    <td  class='alert-info'><?=$lang->getLang("Id")?></td>
                                    <td><?=$row['gam_id']?></td>
                                </tr>
                                <tr>
                                    <td  class='alert-info' style='background-color: rgb(217, 237, 247);'><?=$lang->getLang("Name")?></td>
                                    <td><input type="text" name='name' value='<?=$row['gam_name']?>'></td>
                                </tr>
                                <tr>
                                    <td  class='alert-info'><?=$lang->getLang("Min Bet")?></td>
                                    <td><input type="text" name='minbet' value='<?=$row['gam_min_bet'];?>'
                                               style='width: 100%'></td>
                                </tr>
                                <tr>
                                    <td  class='alert-info'  style='background-color: rgb(217, 237, 247);'><?=$lang->getLang("Max Bet")?></td>
                                    <td><input type="text" name='maxbet'
                                               value='<?=$row['gam_max_bet']?>'></td>
                                </tr>
                                <tr>
                                    <td  class='alert-info'><?=$lang->getLang("Max Payout")?></td>
                                    <td><input type="text" name='maxpayout'
                                               value='<?=$row['gam_max_payout']?>'></td>
                                </tr>
                                <tr>
                                    <td  class='alert-info'  style='background-color: rgb(217, 237, 247);'><?=$lang->getLang("Min Pos Bet")?></td>
                                    <td><input type="text" name='minposbet'
                                               value='<?=$row['gam_min_pos_bet']?>'></td>
                                </tr>
                                <tr>
                                    <td  class='alert-info'><?=$lang->getLang("Max Pos Bet")?></td>
                                    <td><input type="text" name='maxposbet'
                                               value='<?=$row['gam_max_pos_bet']?>'></td>
                                </tr>
                                <tr>
                                    <td  class='alert-info'  style='background-color: rgb(217, 237, 247);'><?=$lang->getLang("Ext Code")?></td>
                                    <td><input type="text" name="ext_code"
                                               value='<?=$row['gam_ext_code']?>'></td>
                                </tr>
                                <tr>
                                    <td  class='alert-info'><?=$lang->getLang("Category")?></td>
                                    <td><select name='category' id='category'><?
                                            $categories= getGameCategories();
                                            while ( $categories->hasNext () ) {
                                                $category = $categories->next ();
                                                ?><option value='<?=$category['gct_name']?>' <?php if ($category['gct_name']==$row['gam_category']){?> selected="selected"<? }?>><?=$category['gct_name']?></option>
                                            <?php }?>
                                        </select></td>
                                </tr>
                                <tr>
                                    <td  class='alert-info'  style='background-color: rgb(217, 237, 247);'><?=$lang->getLang("Type")?></td>
                                    <td><select name='type' id='type'><?
                                            $types= getGameType();
                                            while ( $types->hasNext () ) {
                                                $type= $types->next ();
                                                ?><option value='<?=$type['gte_name']?>' <?php if ($type['gte_name']==$row['gam_type']){?>selected="selected"<? }?> ><?=$type['gte_name']?></option>
                                            <?php }?>
                                        </select></td>
                                </tr>
                                <tr>
                                    <td  class='alert-info'><?=$lang->getLang("Localization")?></td>
                                    <td><input type="text" name="localization"
                                               value='<?=$row['gam_localization']?>'></td>
                                </tr>
                                <tr>
                                    <td><input type="submit" class="btn btn-primary" value='<?=$lang->getLang("Update")?>'>&nbsp;<input type="reset" class="btn btn-warning" value='<?=$lang->getLang("Reset")?>'>&nbsp;<a href="<?=refPage('configuration/game','init')?>"><button type="button"  class="btn btn-danger" ><?=$lang->getLang("Back")?></button></a></td>
                                </tr>
                            </table>
                            </form>
                        </div>
                    </td>
                </tr>
            <?php }?>
            </tbody>
        </table>
    </div>
<?php } ?>
