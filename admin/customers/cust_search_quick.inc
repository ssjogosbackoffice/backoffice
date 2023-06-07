<? //include 'cache_file.class.inc'; ?>

<div  class="bodyHD">Customers</div>
<div class="subnavmenu">
    <?printLink(refpage("customers/customers"), "Main menu");?>
    &nbsp;&gt;&nbsp;
    <strong>Quick Search</strong>
</div><br>
<?
globalize('doQuery');
globalize('search_string');
globalise('start_row');
globalise('num_rows');
globalise('max_rows');
globalise('show_search_results');

$search_string = trim($search_string);

$serialize_filename = "cust_srch_q.srlz.".session_id();
$filter="search_string=$search_string&form_submitted=yes";

if ( $doQuery ){   //define query
    include "cust_list_sql_head.inc";

    if ( isBlank($search_string) )
        addError("search_string", "Please enter a search word");
    else
    {  if ( ctype_digit($search_string) ) //assume customer number
        $sql .= " and pun_customer_number = $search_string";
    else
    {  //$search_string = strtolower($search_string);

        $sql .= " and ( pun_first_name like ('%$search_string%')	";
        $sql .= "       or pun_middle_name like ('%$search_string%')";
        $sql .= "       or pun_last_name like ('%$search_string%')";
        $sql .= "       or pun_email like ('%$search_string%')";
        $sql .= "       or pun_city_suburb like ('%$search_string%')";
        $sql .= "       or cou_name like ('%$search_string%')";
        $sql .= "       or pun_username like ('%$search_string%')";

//         $sql .= " and ( pun_username like('%$search_string%')"; 
        $sql .= " )";
    }

        include(include_content('customers/cust_search_exec.inc'));
    }
}
if ( $show_search_results ) {
    $post_vars = array('page'=>$page,'search_string'=>$search_string, 'filter'=>$filter);
    include(include_content('customers/cust_search_show_results.inc'));
}

?>
<? showErrors(); ?>
<form action="<?=SELF;?>" method="POST">
    <input type="hidden" name="page" value="customers/cust_search_quick">
    <input type="hidden" name="start_search" value="yes">
    <input type="hidden" name="doQuery" value="yes">
    <input type="hidden" name="max_rows" value="2">

    <table cellpadding="4" cellspacing="1" border="0">
        <tr valign="top">
            <td colspan="2" class="formheading"><strong>Quick Search</strong></td>
            <td rowspan="2"><? printLink(refPage('customers/cust_search_advanced'), "Advanced Search.", "_self",8);?></td>
        </tr>
        <tr>
            <td width="120" class="label">Search:</td>
            <td class="content"><input type="text" name="search_string" value="<?=$search_string?>">&nbsp;</span>
            </td>
        </tr>
        <tr>
            <td  class="label">Number of  Records per page</td>
            <td class="content">
                <select name="max_rows">
                    <option value="10" <? if ( 10 == $max_rows ) echo 'selected'; ?>>10
                    <option value="30" <? if ( 30 == $max_rows ) echo 'selected'; ?>>30
                    <option value="60" <? if ( 60 == $max_rows ) echo 'selected'; ?>>60
                    <option value="100" <? if ( 100 == $max_rows ) echo 'selected'; ?>>100
                </select>
                </span>
            </td>
        <tr>
        <tr>
            <td>&nbsp;</td>
            <td><input type=image src="<?=image_dir;?>/button_submit.gif" border=0></td>
        </tr>
    </table>
</form>
