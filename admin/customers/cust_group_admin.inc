<?check_access('manage_groups',$doredirect=true);?>
<?php include_once("CacheFileManager.class.inc") ?>
<script language="Javascript" type="text/javascript">

    function getDummyOption() {
        return new Option("<?for($i=0; $i<45;$i++) echo " ";?>", "");
    }

    function addCustomer(form)
    {  var customers_in_group = form.elements["customers_in_group[]"];
        var customers_not_in_group = form.elements["customers_other[]"];
        var option = null;
        var option_arr = null;
        var text = null;
        var value = null;

        num_members = customers_in_group.length - 1;
        num_customers = customers_not_in_group.length - 1;

        customer_arr = customers_not_in_group.options;
        member_arr = customers_in_group.options;

        for ( var i=0; i<num_customers; i++ )
        {  if ( customer_arr[i].selected && customer_arr[i].value != "" )
        {  value = customer_arr[i].value;
            text  = customer_arr[i].text;
            option = new Option(text, value);

            member_arr[num_members] = null; //strip off place holder from end of array
            member_arr[num_members] = option; //add option to group member array
            num_members++;   //increment number of group members to refelct new option
            member_arr[num_members] = getDummyOption(); //add placeholder at end again
            customer_arr[i] = null; //set option of added customer to null (removed from "not in group list")
            num_customers--;  //number of available customers has been reduced
            i--;
        }
        }
    }


    function removeCustomer(form)
    {  var customers_in_group = form.elements["customers_in_group[]"];
        var customers_not_in_group = form.elements["customers_other[]"];
        var option = null;
        var option_arr = null;
        var text = null;
        var value = null;

        num_members = customers_in_group.length - 1;
        num_customers = customers_not_in_group.length - 1;

        customer_arr = customers_not_in_group.options;
        member_arr = customers_in_group.options;

        for ( var i=0; i<num_members; i++ )
        {  if ( member_arr[i].selected && member_arr[i].value != "" )
        {  value = member_arr[i].value;
            text  = member_arr[i].text;
            option = new Option(text, value);

            customer_arr[num_customers] = null; //strip off place holder from end of array
            customer_arr[num_customers] = option; //add option to group member array
            num_customers++;   //increment number of group members to refelct new option
            customer_arr[num_customers] = getDummyOption(); //add placeholder at end again
            member_arr[i] = null; //set option of added customer to null (removed from "not in group list")
            num_members--;  //number of available customers has been reduced
            i--;
        }
        }
    }


    function confirmDelete(group_code, num_members)
    {	if ( num_members > 0 )
    {
        <?php
           $group_name = "\"+group_code+\"";
           $number     = "\"+num_members+\"";
        ?>

        var mesg  = "";

        if (window.confirm(mesg))
            window.location = '<?=refPage($page)."&group_code=";?>'+group_code+'&mode=delete';
    }
    else
        window.location = '<?=refPage($page)?>&group_code='+group_code+'&mode=delete';
    }


</script>
<div class="bodyHD">Customer Group Management</div>
<div class="bodyHD2">View Groups</div>

<?php

function checkFields($group_code, $description)
{  global $dbh;

    $group_code = strtoupper($group_code);

    if ( isBlank($group_code) )
        addError("", "A group code must be entered");
    else
    {  $num_with_code = $dbh->countQuery("select count(*) from cgroup where cgr_code =  '$group_code'");
        if  ( $num_with_code > 0 )
            addError("", "There is already a group with the code $group_code");
    }

    if ( isBlank($description) )
        addError("", "A group description must be entered");
    else
    {  $num_with_descr = $dbh->countQuery("select count(*) from cgroup where lower(cgr_description) =  lower('$description')");
        if  ( $num_with_descr > 0 )
            addError("", "There is already a group with the description \"$description\"");
    }

    return ! areErrors();
}


function checkFieldsForUpdate($group_code, $description)
{  global $dbh, $group_code;

    $group_code = strtoupper($group_code);

    if ( isBlank($group_code) )
        addError("", "A group code must be entered");
    else
    {  if ( $group_code != $group_code )
    {  $num_with_code = countQuery("select count(*) from cgroup where cgr_code =  '$group_code'");
        if  ( $num_with_code > 0 )
            addError("", "There is already a group with the code $group_code");
    }
    }

    if ( isBlank($description) )
        addError("", "A group description must be entered");
    else
    {  $description = strtolower($description);
        $num_with_descr = $dbh->countQuery("select count(*) from cgroup where lower(cgr_description) =  lower('$description') and cgr_code != '$group_code'");
        if  ( $num_with_descr > 0 )
            addError("", "There is already a group with the description \"$description\"");
    }

    return ! areErrors();
}




function hasMembers($group_code)
{  return (numMembers($group_code) > 0 );
}



globalise('mode');
globalise('group_code');
globalise('form_submitted');
globalise('description');
globalise('notes');

$group_code  = getPostGetVar('group_code');
$group_code  = escapeSingleQuotes($group_code);
$description = escapeSingleQuotes($description);
$notes 	     = escapeSingleQuotes($notes);

if ( 'update' == $mode )
{
    if ( checkFieldsForUpdate($group_code, $description) )
    {
        $sql = "UPDATE cgroup" .
            " SET cgr_description = '$description', cgr_notes = '$notes'" .
            " WHERE cgr_code = '$group_code'";
        $dbh->exec($sql);
        $_SESSION['alert_message'] = "Group $group_code Updated";
        jscriptBlock("window.location='".refPage($page)."'");
        exit;
    }
    else
        $mode = 'browse';
}

if ( 'browse' == $mode )
{  $sql = "select cgr_description, cgr_notes from cgroup";
    $sql .= " where cgr_code = '$group_code'";
    $rs = $dbh->exec($sql);
    $num_rows = $rs->getNumRows();
    if ( 1 == $num_rows )
    {  $row = $rs->next();
        $description = $row['cgr_description'];
        $notes = $row['cgr_notes'];
        $num_members = numMembers($group_code)


        ?>
        <div class=subnavmenu>
            <?printLink(refPage("customers/cust_group_admin"), "Group List", "_self", 8);?>&nbsp;&gt;&nbsp;
            <b>Edit group <?=$group_code?></b>
        </div>
        <? form_head(SELF, 'edit_form') ?>
        <input type="hidden" name="mode" value="update">
        <input type=hidden name="group_code" value="<?=$group_code?>">
        <P>
            <?showErrors();?>
        <P>
        <table cellpadding=0 cellspacing=1 border=0>
            <tr valign=top>
                <td>
                    <table cellpadding=4 cellspacing=1 border=0>
                        <? sideTableRow("Group Code", $group_code);?>
                        <? sideTableRow("Description", "<input type=\"text\" size=30 maxLength=255 name=\"description\" value=\"$description\">");?>
                        <? sideTableRow("Notes", "<textarea name=\"notes\" rows=10 cols=29 maxLength=255 name=\"notes\" value=\"$notes\">$notes</textarea>");?>
                        <? sideTableRow("Number of Members", $num_members.'  <input type="button" value="members..." onClick="javascript:openWindow(\'/customers/cust_grp_members_pop.html?group_code='.$group_code.'\', \'grpmembers\', 400, 400, \'no\', \'yes\')">');?>
                    </table>
                </td>
                <td>
                    &nbsp;&nbsp;
                </td>
                <td>
                    <input type=image src="<?=image_dir;?>/button_update.gif"><br>
                    <a href="javascript:confirmDelete('<?=$group_code?>', <?=$num_members?>)"><img src="<?=image_dir;?>/button_delete.gif" border=0></a>
                </td>
            </tr>
        </table>
        <br>
        </form>
    <? }
    ?>

<?
}
elseif ( 'create' == $mode )
{
    ?>
    <div style="background-color:#efefef; width:200">
        <?printLink(refPage($page),"Group List", "_self", 8);?>&nbsp;&gt;&nbsp;
        <b>Create a group</b>

    </div>
    <br>
    <?
    if ( $_POST['form_submitted'] )
    {  if ( checkFields($group_code, $description) )
    {  $group_code = strtoupper($group_code);
        $sql = "INSERT into cgroup (cgr_code, cgr_description, cgr_notes)";
        $sql .= " VALUES (upper('$group_code'), '$description', '$cgr_notes')";
        $dbh->exec($sql);
        doAdminUserLog($_SESSION['admin_id'], "create_group", $group_code, $pun_id='NULL');
        jalert("Group $group_code created");
        jscriptBlock("window.location='".refPage($page)."'");
        ?> <P>


        <span class=head2>Members of group <?=$group_code?></span>
        <P><b>There are no members in this group.</b> <?printLink("javascript:openWindow('/customers/cust_grp_members_pop.html?group_code=$group_code', 400, 400, 'no', 'yes')", "Click here to add members", "_self", 8);?>
        <P>
    <?
    }
    }

    if ( !$form_submitted || areErrors() )
    {

        ?>
        <?showErrors()?>
        <? form_head() ?>
        <input type="hidden" name="mode" value="<?=$mode?>">
        <table cellpadding=4 cellspacing=1 border=0 width=300>
            <? sideTableRow("Group Code", "<input type=\"text\" size=30 maxLength=255 name=\"group_code\" value=\"$group_code\">");?>
            <? sideTableRow("Description", "<input type=\"text\" size=30 maxLength=255 name=\"description\" value=\"$description\">");?>
            <? sideTableRow("Notes", "<textarea name=\"notes\" rows=10 cols=29 maxLength=255 name=\"notes\" value=\"$notes\">$notes</textarea>");?>
            <tr>
                <td>&nbsp;</td>
                <td><input type=image src="<?=image_dir;?>/button_submit.gif"></td>
            </tr>
        </table>
        </form>
    <? }
}
elseif ( 'delete' == $mode )
{  $dbh->begin();
    $sql = "delete from cgroup where cgr_code = '$group_code'";
    $dbh->exec($sql);
    doAdminUserLog($_SESSION['admin_id'], "delete_group", $group_code, $pun_id='NULL');
    $dbh->commit();

    jalert("Group $group_code deleted");
    header("Location: ".refPage($page));
    exit;
}
else
{  $sql = "select cgr_code, cgr_description from cgroup";
    $rs = $dbh->exec($sql);
    $num_rows = $rs->getNumRows();

    if ( $num_rows > 0 ) //if groups exist
    {  $labels = array('Group Code', 'Description', 'Members');
        topTableheading($labels, $width="", $cellpadding=4, $cellspacing=1, $border=0, $bgcolor='#efefef');

        while ( $rs->hasNext() )
        {  $row = $rs->next();
            $group_code = $row['cgr_code'];
            $group_description  = $row['cgr_description'];
            $num_members = numMembers($group_code);
            ?>
            <tr>
                <td class=content><?=printLink(refPage($page)."&mode=browse&group_code=$group_code", "$group_code", 'contentlink', "_self", 8);?></td>
                <td class=content><?=$group_description?></td>
                <td class=content><?=$num_members?></td>
            </tr>
        <?    }
        ?>    </table>
    <?
    }
    else //no customer groups in cgroup table
    {
        ?> <P><b>There are no customer groups.</b>
    <? }
    ?>
    <form ACTION="<?=SELF?>" method="POST">
        <?printHiddenInput("mode", "create");?>
        <?printHiddenInput("page", $page);?>
        <input type=image src="<?=image_dir?>/button_add.gif">
    </form>
<?
}
?>
