<?php
include_once("financial_functions.inc");
if(isset($_POST['action'])&&$_POST['action']==1)
{
    $_POST['ttm']=false;
    getFinancialByCustomer($_POST['club'],$_POST['date_start'],$_POST['date_end'],$_POST['betwin'],$_POST['financial'],$_POST['ca'],$_POST['cl'],$_POST['v'],$_POST['p2d'],$_POST['pl'],$_POST['rpk'],$_POST['playtech'],$_POST['evolution'],$_POST['infinity'],$_POST['portomaso'],$_POST['ttm'],$_POST['forex'],$_POST['bankr'], $_POST['zecc'], $_POST['gree'], $_POST['zeus']);
}  
?>
