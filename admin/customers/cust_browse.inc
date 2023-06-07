<div  class=bodyHD>View Customers</div>
<?
$serialize_filename = "cust_browse.".session_id();
$max_rows = 100;
$filter = 'x=y';
$doQuery = getPostGetVar('doQuery');

$member_type = getPostGetVar('member_type');
$account_locked = getPostGetVar('account_locked');
$account_activated = getPostGetVar('account_activated');
$account_investigated = getPostGetVar('account_investigated');
$transient = getPostGetVar('transient');

globalise('start_row');
globalise('num_rows');
globalise('operation');
globalise('customer_id');

//select all punters
include "cust_list_sql_head.inc";

//if financial/non-financial member (filter) is selected
if ( 'financial' == strtolower($member_type) ){
    $sql .= " and pun_member_type = 'FINANCIAL'";
    $title = "";
    $filter = "member_type=financial";
}elseif ( 'nonfinancial' == strtolower($member_type) ){
    $sql .= " and pun_member_type = 'NONFINANCIAL'";
    $title = "";
    $filter = "member_type=nonfinancial";
}
else
    $title = "Browse all Members";

if ( $account_locked ){
    $sql .= " and pun_access = 'deny'";
    $sql .= " and pun_registration_status = 'activated'";
    $title = "Browse Customers with LOCKED Accounts";
    $filter .= "&account_locked=yes";
}

if ( $account_investigated ){
    $sql .= " and pun_investigate = 1";
    $title = "Browse Customers Under INVESTIGATION";
    $filter .= "&account_investigated=yes";

}

if ( $account_activated )
{
    if ( 'yes' == $account_activated )
    {  $sql .= " and pun_registration_status = 'activated'";
        $sql .= " and pun_member_type = 'NONFINANCIAL'";
        $title = "Browse Nonfinancial Customers with ACTIVATED Accounts";
        $filter .= "&account_activated=yes";
    }
    else
    {  $sql .= " and pun_registration_status != 'activated'";
        $title = "Browse Customers with UNACTIVATED Accounts";
        $filter .= "&account_activated=no";
    }
}


if ( $transient )
{   $sql .= " and pun_identified = -1";
    $title = "Transient Customers";
    $filter .= "&transient=true";
}

if ( ! $title )
    $title = "Browse customers";

//if country of member (filter) is selected
if ( !isBlank($country_code) )
{  $sql .= " and pun_cou_code = '$country_code'";
    $title .= " from ".getCountryName($country_code);
    $filter .= "&country_code=$country_code";
}


if ( $doQuery )
    include(include_content('customers/cust_search_exec.inc'));

?>
<div class=subnavmenu>
    <?printLink(refpage("customers/customers"), "Back to Main menu");?>
    &nbsp;&gt;&nbsp;<b><?=$title?></b>
</div>
<br>
<? include(include_content('customers/cust_search_show_results.inc')); ?>