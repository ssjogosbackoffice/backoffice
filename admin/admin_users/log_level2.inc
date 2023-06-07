<div class=bodyHD><?=$lang->getLang("Home Page")?></div>
<?php
globalise('log_id');
globalise('qstr');
?>
<?php backLink(SELF."?$qstr",'Back to List');?><br>

<?
$sql = 'SELECT alg_type as "log type", alg_data as "actions"' .
    ', alg_aus_id, alg_pun_id as "customer", alg_time as "time"' .
    ', aus_name as "user", aus_username as "username", aty_name as "user type"'.
    ' FROM admin_user_log, admin_user, admin_user_type'  .
    ' WHERE aus_aty_id = aty_id and alg_aus_id = aus_id' .
    " AND alg_id = $log_id";

$val_fmt_arr = array (
    'time' => 'return getClassLink("'.refPage('admin_users/log_level2').'&log_id=".$rec["log_id"], shortDate($rec["time"],true), "contentlink");',
    'log type' => 'return ereg_replace("_", " ", ucwords($rec["log type"]));',
);

$rs = $dbh->exec($sql);
$num_rows = $rs->getNumRows();

if ( $num_rows > 0 )
{	$row = $rs->next();

if ( $row['customer'] )
{
    $customer_row = getCustomer($row['customer']);
    $customer_str = $customer_row['id'].' / '.$customer_row['customer_number'].' / '.$customer_row['full_name'];

}
else
{  if ( 'modify_admin_user' ==$row['log type'] )
{
    $log_data = $row['actions'];

}
}

?>
<br>
<table cellpadding=4 cellspacing=1 border=0>
    <tr>
        <td class=formheading colspan=2><?=$lang->getLang("Copyright")?></td>
    </tr>
    <tr>
        <td class=label width=100><?=$lang->getLang("Date/Time")?><br/></td>
        <td class=content><?=shortDate($row['time'],true);?></td>
    </tr>
    <tr>
        <td class=label><?=$lang->getLang("Log Type")?></td>
        <td class=content style="color:blue"><?=ereg_replace('_', ' ', ucwords($row['log type']));?></td>
    </tr>
    <tr>
        <td class=label><?=$lang->getLang("Username")?></td>
        <td class=content><?=$row['username'];?></td>
    </tr>
    <tr>
        <td class=label><?=$lang->getLang("Full Name")?></td>
        <td class=content><?=$row['user'];?></td>
    </tr>
    <tr>
        <td class=label><?=$lang->getLang("User Type")?></td>
        <td class=content><?=$row['user type'];?></td>
    </tr>
    <tr>
        <td class=label><?=$lang->getLang("Customer")?></td>
        <td class=content><?=($row['customer'] ? $customer_str : 'N/A');?></td>
    </tr>
</table>
<br>

<?
if ( 'pre-register_customer' == $row['log type'] )
{
?>
<table cellpadding=4 cellspacing=1 border=0>
<tr>
    <td class=formheading colspan=2><?=$lang->getLang("Event Details")?></td>
</tr>
<tr>
    <td class=label><?=$lang->getLang("Field")?></td>
    <td class=label><?=$lang->getLang("Assigned Value")?></td>
</tr>
<?		$arr    = explode('|',$row['actions']);

foreach ($arr as $key=>$val)
{	$field_name = strtok($val,':');
    $field_val  = strtok(':');

    if ( 'dob' == $field_name )
    {	$field_val = shortDate($field_val);
    }
    if ( 'reg_date' == $field_name )
    {	$field_val = shortDate($field_val,true);
    }
    elseif ( 'aus_aty_id' == $field_name )
    {	$rs  = $dbh->exec("select aty_name from admin_user_type where aty_id = $field_val");
        $num_rows = $rs->getNumRows();

        if ( 1 == $num_rows )
        {	$row = $rs->next();
            $field_val = $row['aty_name'];
        }

    }
    ?>
    <tr>
        <td class=content style="font-weight:bold"><?=$field_name;?></td>
        <td class=content><?=$field_val;?></td>
    </tr>

<?
}
?>
</td>
</tr>
<?
}
elseif ( 'modify_customer' == $row['log type'] ) 	//If the log is regarding the modificatiuon of a customer's details
{
?>
<table cellpadding=4 cellspacing=1 border=0>
<tr>
    <td class=formheading colspan=3><?=$lang->getLang("Event Details")?></td>
</tr>
<tr>
    <td class=label><?=$lang->getLang("Field")?></td>
    <td class=label><?=$lang->getLang("Previous Value")?></td>
    <td class=label><?=$lang->getLang("New Value")?></td>
</tr>
<?
$arr = explode('|', $row['actions']); //explode into fields

foreach ($arr as $key => $val)
{
    $mod_str = $val;  //get string that describes modification
    $field_name	= strtok($mod_str, ':'); //get name of modified field
    $field_mod  = strtok(':'); //get prev value / next value pair
    $prev_val_arr = explode('=',$field_mod);
    $prev_val = $prev_val_arr[0]; //prev value
    $new_val  = $prev_val_arr[1]; //new value

    if ( $prev_val != $new_val )
    {
        ?>
        <tr>
            <td class=content style="font-weight:bold"><?=$field_name?></td>
            <td class=content><?=$prev_val?></td>
            <td class=content><?=$new_val?></td>
        </tr>

    <?			}
}
}
elseif ( 'add_admin_user' == $row['log type'] )
{
?>
<table cellpadding=4 cellspacing=1 border=0>
<tr>
    <td class=formheading colspan=2><?=$lang->getLang("Event Details")?></td>
</tr>
<tr>
    <td class=label><?=$lang->getLang("Field")?></td>
    <td class=label><?=$lang->getLang("Assigned Value")?></td>
</tr>
<?		$arr    = explode('|',$row['actions']);

foreach ($arr as $key=>$val)
{	$field_name = strtok($val,':');
    $field_val  = strtok(':');

    if ( 'aus_betting_club' == $field_name )
    {
        $field_val = getClubName($field_val);
    }
    elseif ( 'aus_aty_id' == $field_name )
    {	$rs  = $dbh->exec("select aty_name from admin_user_type where aty_id = $field_val");
        $num_rows = $rs->getNumRows();

        if ( 1 == $num_rows )
        {	$row = $rs->next();
            $field_val = $row['aty_name'];
        }

    }
    ?>
    <tr>
        <td class=content style="font-weight:bold"><?=$field_name;?></td>
        <td class=content><?=$field_val;?></td>
    </tr>

<?
}
?>
</td>
</tr>
<?
}
elseif ( 'delete_admin_user' == $row['log type'] )
{
?>
<table cellpadding=4 cellspacing=1 border=0>
<tr>
    <td class=formheading colspan=2><?=$lang->getLang("Event Details")?></td>
</tr>
<tr>
    <td class=label><?=$lang->getLang("Action")?></td>
    <td class=label><?=$lang->getLang("Details")?></td>
</tr>
<tr valign=top>
    <td class=content><?=$lang->getLang("Deleted Administrator User")?></td>
    <td class=content>
        <?		$aus_arr    = explode('|',$row['actions']);

        $str  = '<b>'.strtok($aus_arr[0],':').'</b>: '.strtok(':').'<br>';
        $str .= '<b>'.strtok($aus_arr[1],':').'</b>: '.strtok(':').'<br>';
        $str .= '<b>'.strtok($aus_arr[2],':').'</b>: '.strtok(':').'<br>';

        //get user type
        $aty_id_lbl   = strtok($aus_arr[3],':');
        $aty_id       = strtok(':');
        $sql = "select aty_name from admin_user_type where aty_id = $aty_id";
        $rs  = $dbh->exec($sql);
        $row = $rs->next();
        $aty_name = $row['aty_name'];

        $str   .= "<b>User Type</b>: $aty_name";
        echo $str;

        ?>
    </td>
</tr>
<?
}
elseif ( 'modify_admin_user' == $row['log type'] )
{
?>
<table cellpadding=4 cellspacing=1 border=0>
    <tr>
        <td class=formheading colspan=3><?=$lang->getLang("Event Details")?></td>
    </tr>
    <tr>
        <td class=label><?=$lang->getLang("Field")?></td>
        <td class=label><?=$lang->getLang("Previous Value")?></td>
        <td class=label><?=$lang->getLang("New Value")?></td>
    </tr>
    <?php
    $arr = explode('|', $row['actions']); //explode into fields

    foreach ($arr as $key => $val)
    {
        $mod_str      = $val;  //get string that describes modification
        $field_name   = strtok($mod_str, ':'); //get name of modified field
        $field_mod    =  strtok(':'); //get prev value / next value pair

        $prev_val_arr = explode('=',$field_mod);
        $prev_val = $prev_val_arr[0]; //prev value
        $new_val  = $prev_val_arr[1]; //new value

        if ( 'aus_aty_id' == $field_name )
        {	$field_name = 'user type';
            $rs = $dbh->exec("select aty_name from admin_user_type where aty_id = $prev_val");
            $row = $rs->next();
            $prev_val = $row['aty_name'];
            $rs = $dbh->exec("select aty_name from admin_user_type where aty_id = $new_val");
            $row = $rs->next();
            $new_val = $row['aty_name'];
        }

        if ( $prev_val != $new_val || $field_name == 'Admin User ID')
        {
            ?>
            <tr>
                <td class=content style="font-weight:bold"><?=$field_name?></td>
                <td class=content><?=$prev_val?></td>
                <td class=content><?=$new_val?></td>
            </tr>

        <?			}
    }
    }
    elseif ( 'add_news' == $row['log type'] )
    {
    ?>
    <table cellpadding=4 cellspacing=1 border=0>
        <tr>
            <td class=formheading colspan=2><?=$lang->getLang("Event Details")?></td>
        </tr>
        <tr>
            <td class=label><?=$lang->getLang("Field")?></td>
            <td class=label><?=$lang->getLang("Assigned Value")?></td>
        </tr>

        <?		$arr    = explode('|',$row['actions']);

        foreach ($arr as $key=>$val)
        {	$field_name = strtok($val,':');
            $field_val  = strtok(':');
            ?>
            <tr>
                <td class=content style="font-weight:bold"><?=$field_name;?></td>
                <td class=content><?=$field_val;?></td>
            </tr>
        <?		}
        ?>
        </td>
        </tr>
        <?
        }
        elseif ( 'add_betting_club' == $row['log type'] )
        {
        ?>
        <table cellpadding=4 cellspacing=1 border=0>
            <tr>
                <td class=formheading colspan=2><?=$lang->getLang("Event Details")?></td>
            </tr>
            <tr>
                <td class=label><?=$lang->getLang("Field")?></td>
                <td class=label><?=$lang->getLang("Assigned Value")?></td>
            </tr>

            <?		$arr    = explode('|',$row['actions']);

            foreach ($arr as $key=>$val)
            {	$field_name = strtok($val,':');
                $field_val  = strtok(':');
                ?>
                <tr>
                    <td class=content style="font-weight:bold"><?=$field_name;?></td>
                    <td class=content><?=$field_val;?></td>
                </tr>
            <?		}
            ?>
            </td>
            </tr>
            <?
            }
            elseif ( 'modify_betting_club' == $row['log type'] )
            {
            ?>
            <table cellpadding=4 cellspacing=1 border=0>
                <tr>
                    <td class=formheading colspan=3><?=$lang->getLang("Event Details")?></td>
                </tr>
                <tr>
                    <td class=label><?=$lang->getLang("Field")?></td>
                    <td class=label><?=$lang->getLang("Previous Value")?></td>
                    <td class=label><?=$lang->getLang("New Value")?></td>
                </tr>
                <?
                $arr = explode('|', $row['actions']); //explode into fields

                foreach ($arr as $key => $val)
                {
                    $mod_str = $val;  //get string that describes modification
                    $field_name	= strtok($mod_str, ':'); //get name of modified field
                    $field_mod  = strtok(':'); //get prev value / next value pair
                    $prev_val_arr = explode('=',$field_mod);
                    $prev_val = $prev_val_arr[0]; //prev value
                    $new_val  = $prev_val_arr[1]; //new value

                    if ( $prev_val != $new_val || $field_name == 'bcl_id' )
                    {
                        ?>
                        <tr>
                            <td class=content style="font-weight:bold"><?=$field_name?></td>
                            <td class=content><?=$prev_val?></td>
                            <td class=content><?=$new_val?></td>
                        </tr>

                    <?			}
                }
                }
                elseif ('admin_login' == $row['log type']) {
                ?>
                <table cellpadding=4 cellspacing=1 border=0>
                    <tr>
                        <td class=formheading colspan=2><?=$lang->getLang("Event Details")?></td>
                    </tr>
                    <tr>
                        <td class=label><?=$lang->getLang("Field")?></td>
                        <td class=label><?=$lang->getLang("Assigned Value")?></td>
                    </tr>
                    <?php
                    $arr    = explode('|',$row['actions']);

                    foreach ($arr as $key=>$val) {
                        $field_name = strtok($val,':');
                        $field_val  = strtok(':');
                        ?>
                        <tr>
                            <td class=content style="font-weight:bold"><?=$field_name;?></td>
                            <td class=content><?=$field_val;?></td>
                        </tr>
                    <?php
                    }
                    ?>
                    </td>
                    </tr>
                    <?php
                    }

                    }
                    ?>
                </table>
