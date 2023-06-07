<?php
$orderby   = getPostGetVar('orderby');
$operation = getPostGetVar('operation');
$asc_desc  = getPostGetVar('asc_desc');

if($max_rows == NULL)
    $max_rows = 20;

if ( 'delete' == $operation )
    if ( check_access('delete_customer') )
        deleteCustomer((int)$_GET['customer_id']);

if ( 'desc' == $asc_desc || !$orderby )
    $asc_desc = 'desc';
else
    $asc_desc = 'asc';

switch ($orderby )
{  case "number":
    $sql .= " order by pun_customer_number";
    $sql .= ", upper(pun_middle_name)";
    break;
    case "country":
        $sql .= " order by upper(cou_name)";
        break;
    case "member_type":
        $sql .= " order by upper(pun_member_type)";
        break;
    case "status":
        $sql .= " order by lower(pun_registration_status), lower(pun_access) ";
        break;
    case "city":
        $sql .= " order by lower(pun_city_suburb)";
        break;
    case "funds":
        $sql .= " order by pcr_credits";
        break;
    case "bets":
        $sql .= " order by pcr_total_bets";
        break;
    case "wins":
        $sql .= " order by pcr_total_wins";
        break;
    case "access":
        $sql .= " order by pun_access";
        break;
    case "email":
        $sql .= " order by lower(pun_email)";
        break;
    case "investigate":
        $sql .= " order by lower(pun_investigate  )";
        break;
    case "num_logins":
        $sql .= " order by lower(pun_num_logins)";
        break;
    case "last_login":
        $sql .= " order by pun_last_logged_in";
        break;
    case "deposits":
        $sql .= " order by pcr_total_deposits";
        break;
    case "withdrawals":
        $sql .= " order by pcr_total_withdrawals";
        break;
    case "name":
        $sql .= " order by upper(pun_last_name) $asc_desc, upper(pun_first_name)";
        $sql .= ", upper(pun_middle_name)";
        break;
    case "fees":
        $sql .= " order by pcr_fees_retained";
        break;
    case "returned":
        $sql .= " order by pcr_total_bets-pcr_total_wins";
        break;

    case "Betting Club":
        $sql .= " order by \"Betting Club\"";
        break;
    case "District":
        $sql .= " order by \"District\"";
        break;
    case "Region":
        $sql .= " order by \"Region\"";
        break;
    case "Nation":
        $sql .= " order by \"Nation\"";
        break;
    //order by name
    default:{
    $orderby = 'reg_date';
    $sql .= " order by pun_reg_date";
    }
}



function inConfirmationList($medium, $confirmation_str) {
    if ( $confirmation_str ){
        $arr = explode(',', strtolower($confirmation_str));
        return in_array(strtolower($medium), $arr);
    }
    return false;
}


$sql .= " $asc_desc"; //ascending or descending
//echo $sql;
//die($sql);

$rs  =$dbh->exec($sql);

//$rs  =  $dbh->getResults();//execute search query
$num_rows = $rs->getNumRows();

if ( $num_rows  > 0 ){
    $num_rows_displayed = 0;
    $delete_image = '<img src="'.image_dir.'/button_delete.gif" border=0>';  //delete button
    //begin formatting rows
    $i=0;

    while ( $row = $rs->next() ){
//   	var_dump($row);  
        // if a transient customer
        if ( empty($row['pun_first_name']) && empty($row['pun_middle_name']) && empty($row['pun_last_name']))
            $name_disp = $row['pun_username'];
        else
            $name_disp = $row['pun_username'] . "<br/><font color='Gray'>" . $row['pun_last_name'] . ', ' . $row['pun_first_name']. ' ' . $row['pun_middle_name'] . "</font>";

        $display[$i]['Customer'] .= '<table cellpadding=0 cellspacing=0 border=0><tr valign=top><td class=small colspan=2>';
        //"Customer" column
        $display[$i]['Customer'] .= getLink(refPage('customers/customer_view&customer_id='.$row['pun_id']), $name_disp, 'contentlink');
        $display[$i]['Customer'] .= '<br>'.$row['pun_email'];
        if ( inConfirmationList('email', $row['pun_confirmation_list']) )
            $display[$i]['Customer'] .= '<img src="'.image_dir.'/icon_activated_sm.gif">';
        $display[$i]['Customer'] .= '</td></tr><tr><td class=small>'.$row['pun_customer_number'].'</td><td>';

        if ( 'FINANCIAL' == $row['pun_member_type'] )
            $display[$i]['Customer'] .= '<img src="'.image_dir.'/icon_financial.gif" alt="Financial">';
        else
            $display[$i]['Customer'] .= '<img src="'.image_dir.'/icon_nonfinancial.gif" alt="Nonfinancial">';

        if ( -1 == $row['pun_identified'] )
            $display[$i]['Customer'] .= '<img src="'.image_dir.'/icon_transient.gif" alt="Transient customer">';

        if ( 'deny' == $row['pun_access'] )
            $display[$i]['Customer'] .= '<img src="'.image_dir.'/icon_lock.gif" alt="Account Locked">';

        if ( $row['pun_investigate'] )
            $display[$i]['Customer'] .= '<img src="'.image_dir.'/icon_investigate.gif" alt="Account Investigate">';

        if ( $row['pun_pre_registered'] )
            $display[$i]['Customer'] .= '<img src="'.image_dir.'/icon_preg.gif" alt="Pregistration">';

        switch ( $row['pun_registration_status'] )
        {  case 'pending':
            $display[$i]['Customer'] .= '<img src="'.image_dir.'/icon_waiting.gif" alt="Activation Pending">';
            break;
            case 'activated':
                $display[$i]['Customer'] .= '<img src="'.image_dir.'/icon_activated.gif" alt="Account activated">';
                break;
            case 'email bounced':
                $display[$i]['Customer'] .= '<img src="'.image_dir.'/icon_email_fail.gif" alt="Email send failed">';
                break;
            default:
                $display[$i]['Customer'] .=  $row['pun_member_type'];
        }

        $display[$i]['Customer'] .= '</td></tr></table>';

        $x = floor($i / $max_rows);

        $start_row = $x*$max_rows+1;


        $getstr = "doQuery=yes&operation=delete&customer_id=".$row['pun_id']."&start_row=$start_row&num_rows=$num_rows&num_rows_displayed=$num_rows_displayed&max_rows=$max_rows&$filter";

        if ( ! $row['pun_num_logins'] && $row['pcr_total_deposits'] == 0 && hasCustomerDeletePriv($_SESSION['aty_code']) )
            $display[$i]['Customer'] .= "<br><br><A HREF=\"javascript:doDelete('$getstr','" . replace_quotes($dbh->escapeQuotesAndSlashes(trim($name_disp))) . "')\">".$delete_image.'</A>';


        //"Location" Column
        $display[$i]['Location'] = $row['pun_city_suburb'].'<br>'.$row['cou_name'];
        if ( $row['pun_phone_mobile'] )
        {  $display[$i]['Location'] .= '<br>'.$row['pun_phone_mobile'];
            if ( inConfirmationList('mobile', $row['pun_confirmation_list']) )
                $display[$i]['Location'] .= '<img src="'.image_dir.'/icon_activated_sm.gif">';
        }
        //"Login" Column
        $last_login = dbTsToPhpTs($row['pun_last_logged_in']);
        $reg_date = dbTsToPhpTs($row['pun_reg_date']);

        if ( $last_login < $reg_date ) // (defaults to epoch on customer record insert)
            $last_login = '(Never logged in)';
        else
            $last_login = shortDate($row['pun_last_logged_in'],true);

        $display[$i]['Login']  = shortDate($row['pun_reg_date'],true).'<br>'.$last_login;
        $display[$i]['Login'] .= '<br>'.( $row['pun_num_logins'] ? $row['pun_num_logins'] : 0 );

        $func = ( 'FINANCIAL' == $row['pun_member_type'] ? 'getInDollars' : 'getInFunTokens');

        //Account Balance Column
        $display[$i]['Balance'] .= $func($row['pcr_credits'],2,$row['pun_currency']);

        //Transactions Column
        $display[$i]['Transactions'] .= $func($row['pcr_total_deposits']);
        $display[$i]['Transactions'] .= '<br>'.$func($row['pcr_fees_retained']);
        $display[$i]['Transactions'] .= '<br>'.$func($row['pcr_total_withdrawals']);

        //Bets Column
        $display[$i]['Bets'] .= $func($row['pcr_total_bets']);
        $display[$i]['Bets'] .= '<br>'.$func($row['pcr_total_wins']);
        $display[$i]['Bets'] .= '<br>'.$func($row['pcr_total_bets']-$row['pcr_total_wins']);

        //Jurisdiction Columns
        /*echo "<pre>";
        var_dump($row);
        echo "</pre>";*/
        $display[$i]['Betting Club'] = $row['betting club'];
        $display[$i]['District'] = $row['district'];
        $display[$i]['Region'] = $row['region'];
        $display[$i]['Nation'] = $row['nation'];
        $i++;
    }
    //var_dump($display);
    //die();
    $rs = new RecordSet($display);
    $num_rows =$rs->getNumRows();

    //if ( 1 == $num_rows ){
    //header("Location: ".refPage('customers/customer_view&customer_id='.$row['pun_id']));
    //exit;
    //}
    $rs->serialize($serialize_filename);

    if ( ! $start_row || ($start_row > $max_rows))
        $start_row=1;

    $show_search_results = true;
}else{
    printInfoMessage("No Matching Customers Found");
}
