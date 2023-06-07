<script language="javascript" type ="text/javascript">
    function doDelete(get_str, cus_name){
        <?php
           $cus_name = "\"+ cus_name + \"";
        ?>
        if(window.confirm(""))
            window.location = '<?=refPage($page)?>&'+get_str;
    }
</script>
<?

$post_vars = array('page'=>$page,'search_string'=>$search_string, 'filter'=>$filter, 'customer_id'=>'', 'operation'=>'', 'doQuery'=>'');
$filter .="&search_string=$search_string&doQuery=yes&num_rows=$num_rows&num_rows_displayed=$num_rows_displayed&max_rows=$max_rows";

if ( file_exists(cache_file_dir."/$serialize_filename") ){
    $image = '<img src="'.image_dir.'/clear.gif" width=16 height=12 border=0>';
    function ascDescLink ($field_label){
        global $filter, $orderby, $asc_desc, $page;

        $arrow_up_off   =  '<img src="'.image_dir.'/arrow_up_formlabel_off.gif" border="0">';
        $arrow_up_on   =  '<img src="'.image_dir.'/arrow_up_formlabel_on.gif" border="0">';

        $arrow_down_off =  '<img src="'.image_dir.'/arrow_down_formlabel_off.gif" border=0>';
        $arrow_down_on =  '<img src="'.image_dir.'/arrow_down_formlabel_on.gif" border=0>';

        if ( $field_label == $orderby  ){
            if ( 'desc' == $asc_desc ) {
                $asc_link = getLink(refPage($page)."&orderby=$field_label&asc_desc=asc&$filter", $arrow_up_off);
                $desc_link = $arrow_down_on;
            } else {
                $desc_link = getLink(refPage($page)."&orderby=$field_label&asc_desc=desc&$filter", $arrow_down_off);
                $asc_link = $arrow_up_on;
            }
        } else {
            $asc_link = getLink(refPage($page)."&orderby=$field_label&asc_desc=asc&$filter", $arrow_up_off);
            $desc_link = getLink(refPage($page)."&orderby=$field_label&asc_desc=desc&$filter", $arrow_down_off);
        }
        return " $asc_link<nobr/>&nbsp;$desc_link";
    }

    $h_customer     = "Name".ascDescLink ('name')."<br/>Email".ascDescLink ('email');;
    $h_location     = "City".ascDescLink ('city').'<br/>'."Country".ascDescLink ('country');
    $h_login        = "Reg. Date".ascDescLink ('reg_date').'<br/>'."Last Login".ascDescLink ('last_login').'<br>'."Num. Logins".ascDescLink ('num_logins');
    $h_balance      = "Funds".ascDescLink ('funds');
    $h_transactions = "Deposits".ascDescLink ('deposits').'<br/>'."Fees Retained".ascDescLink ('fees').'<br>'."Withdraw".ascDescLink ('withdrawals');
    $h_bets         = "Bets".ascDescLink ('bets').'<br/>'."Returned".ascDescLink ('wins').'<br>'."Total ".ascDescLink ('returned');
    $h_club         = "Betting Club".ascDescLink ('Betting Club');
    $h_district     = "District".ascDescLink ('District');
    $h_region       = "Region".ascDescLink ('Region');
//XXX
    $h_nation       = "Nation ".ascDescLink ('Nation');
    $header_arr = array($h_customer,$h_location,$h_login,$h_balance,$h_transactions,$h_bets, $h_club, $h_district, $h_region,$h_nation );
    $header = '<tr valign="top"><td class="label">'.implode('</td><td class="label">',$header_arr).'</td></tr>';

    $cell_fmt_arr = array (
        'Balance' => 'class="content" align="right"',
        'Transactions' => 'class="content" align="right"',
        'Bets' => 'class="content" align="right"'
    );

    if ( ! $rs )
        $rs = RecordSet::unserialize($serialize_filename);
//var_dump($rs);
    $qp = new QueryPrinter($rs);

    $cols_arr = array('Customer', 'Location', 'Login', 'Balance', 'Transactions', 'Bets', 'Betting Club', 'District', 'Region','Nation');
    $qp->printTable($rs,$cols_arr, $post_vars, $val_fmt_arr, $cell_fmt_arr, $sum_fmt_arr, $excel_filename="", $header, $width, $start_row, $bygamemax_rows=100, $hide_msg=false, $form_name='recorddisplayform');

    ?><br/><br/><br/>
<?
}
?>
