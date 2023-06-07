<?php
include_once("financial_portomaso_functions.inc");
if(isset($_POST['action'])&&$_POST['action']==1)
{
    $_POST['ttm']=false;
    getFinancialByCustomer($_POST['club'],$_POST['date_start'],$_POST['date_end'],$_POST['betwin'],$_POST['portomaso']);
}  
?>
