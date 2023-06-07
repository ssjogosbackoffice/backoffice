<div  class=bodyHD>Customers</div>
<div class=subnavmenu>
    <?printLink(refpage("customers/customers"), "Main menu");?>
    &nbsp;&gt;&nbsp;
    <b>Advanced Search</b>
</div><br>
<?
$serialize_filename = "cust_srchadv.".session_id();
$filter = 'x=y';
globalise('start_row');
globalise('num_rows');
globalise('max_rows');
globalise('doQuery');
globalise('start_search');
globalise('email');
globalise('first_name');
globalise('middle_name');
globalise('last_name');
globalise('g_member_type');
globalise('show_search_results');


if ( $doQuery )
{

    //define query
    include "cust_list_sql_head.inc";

    if ( !isBlank($email) )
    {  $sql .= " and  pun_email like ('%$email%')";
        $filter .= "&email=$email";
    }

    if ( !isBlank($first_name) )
    {  $sql .= " and pun_first_name like ('%$first_name%')";
        $filter .= "&first_name=$first_name";
    }

    if ( !isBlank($middle_name) )
    {  $sql .= " and pun_middle_name like ('%$middle_name%')";
        $filter .= "&middle_name=$middle_name";
    }

    if ( !isBlank($last_name) )
    {  $sql .= " and pun_last_name like ('%$last_name%')";
        $filter .= "&last_name=$last_name";
    }

    //if financial/non-financial member (filter) is selected
    if ( 'financial' == strtolower($g_member_type) )
    {  $sql .= " and pun_member_type = 'FINANCIAL'";
        $filter .= "&g_member_type=financial";
    }
    elseif ( 'nonfinancial' == strtolower($g_member_type) )
    {  $sql .= " and pun_member_type = 'NONFINANCIAL'";
        $filter .= "&g_member_type=nonfinancial";
    }

    if ( $account_locked )
        if ( 'yes' == $account_locked )
        {  $sql .= " and pun_access = 'deny'";
            $filter="&account_locked=yes";
        }
        else
        {  $sql .= " and pun_access = 'allow'";
            $filter="&account_locked=no";
        }


    if ( $account_investigated )
    {  $sql .= " and pun_investigate = 1";
        $filter .= "&account_investigated=yes";
    }

    if ( $account_activated )
        if ( 'yes' == $account_activated )
        {  $sql .= " and pun_registration_status = 'activated'";
            $filter="&account_activated=yes";
        }
        else
        {  $sql .= " and pun_registration_status != 'activated'";
            $filter="&account_activated=no";
        }

    if ( !isBlank($country_code) )
    {  $sql .= " and pun_cou_code = '$country_code'";
        $filter="&country_code=$country_code";
    }

    if ( $filter != 'x=y' ) //at least one filter field has been entered/selected
        include(include_content('customers/cust_search_exec.inc'));
    else
        addError("", "At least one search field must be entered or selected before submitting");
}
if ( $show_search_results )
{
    $post_vars = array('page'=>$page,'search_string'=>$search_string, 'filter'=>$filter,'start_search'=>$start_search);
    include(include_content('customers/cust_search_show_results.inc'));
}
?>
<? showErrors(); ?>
<P>
    Type whole or part search words in the appropriate fields, then click "Search"
    <br><br>
    <? form_head($action=SELF, $name="cust_advanced_search", $method="POST", $onsubmit="") ?>
    <input type="hidden" name="start_search" value="yes">
    <input type="hidden" name="customer_id">
    <input type="hidden" name="doQuery" value="yes">
    <input type="hidden" name="max_rows" value="2">
<table cellpadding=4 cellspacing=1 border=0>
    <tr valign=top>
        <td colspan=2 class=formheading><b>Advanced Search</b></td>
    </tr>
    <tr>
        <td class=label><b>Email</b></td>
        <td class=content><input type=text size=27 name=email value="<?=$email;?>"></span></td>
    </tr>
    <tr>
        <td class=label><b>First name</b></td>
        <td class=content><input type=text size=27 name=first_name value="<?=$first_name;?>"></span></td>
    </tr>
    <tr>
        <td class=label><b>Middle name</b></td>
        <td class=content><input type=text size=27 name=middle_name value="<?=$middle_name;?>"></span></td>
    </tr>
    <tr>
        <td class=label><b>Last name</b></td>
        <td class=content><input type=text size=27 name=last_name value="<?=$last_name;?>"></span></td>
    </tr>
    <tr>
        <td  class=label>City/Suburb</td>
        <td class=content><input type=text size=27 name=city_suburb value="<?=$city_suburb;?>"></td>
    </tr>
    <tr>
        <td  class=label>Country</td>
        <td class=content><?countrySelect();?></span></td>
    </tr>
    <tr>
        <td  class=label>Membership</td>
        <td class=content>
            <input type="radio" name="g_member_type" value="" <?if ( ! $g_member_type ) echo 'checked'?>>All
            &nbsp;&nbsp;
            <input type="radio" name="g_member_type" value="financial" <? if ( 'financial' ==  $g_member_type ) echo 'checked'?>>Financial
            &nbsp;&nbsp;
            <input type="radio" name="g_member_type" value="nonfinancial" <? if ( 'nonfinancial' ==  $g_member_type ) echo 'checked'?>>Nonfinancial
            &nbsp;&nbsp;
        </td>
    </tr>
    <tr>
        <td  class=label>Account access</td>
        <td class=content>
            <input type="radio" name="account_locked" value="" <?if ( ! $account_locked ) echo 'checked'?>>All
            &nbsp;&nbsp;
            <input type="radio" name="account_locked" value="no" <? if ( 'no' ==  $account_locked ) echo 'checked'?>>Open
            &nbsp;&nbsp;
            <input type="radio" name="account_locked" value="yes" <? if ( 'yes' ==  $account_locked ) echo 'checked'?>>Locked
            &nbsp;&nbsp;
        </td>
    </tr>
    <tr>
        <td  class=label>Number of  Records per page</td>
        <td class=content>
            <select name=max_rows>
                <option value=10 <? if ( 10 == $max_rows ) echo 'selected'; ?>>10
                <option value=30 <? if ( 30 == $max_rows ) echo 'selected'; ?>>30
                <option value=60 <? if ( 60 == $max_rows ) echo 'selected'; ?>>60
                <option value=100 <? if ( 100 == $max_rows ) echo 'selected'; ?>>100
            </select>
            </span>
        </td>
    </tr>
    <tr>
        <td>&nbsp;</td>
        <td><input type="image" src="<?=image_dir;?>/button_submit.gif"></td>
    </tr>
</table>
</form>
