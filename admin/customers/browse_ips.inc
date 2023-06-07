<?include 'cache_file.class.inc';?>
    <div class=level2heading></div>
<?
$conn = casinoConnect();

globalise('start_row');
globalise('num_rows');
globalise('max_rows');
globalise('doQuery');

if ( 'viewaccounts' == $phase )
{  $cache_filename = "cust_ip_address.".session_id();
    ?>
    <div class=subnavmenu>
        <?printLink(refpage('customers/customers'), "");?>
        &nbsp;&gt;&nbsp;
        <?printLink(refpage("$page&doQuery=yes"), "");?>
        &nbsp;&gt;&nbsp;
        <b>View accounts for IP address <?=$ip?></b>
    </div><br>
    <?

    if ( !isValidIp($ip) )
        dieWithError("");

    $filter = "ip=$ip";

    if ( $doQuery )
    {  include "cust_list_sql_head.inc";
        $sql .= " and pun_id IN";
        $sql .= " ( select distinct clg_pun_id from customer_log";
        $sql .= "   where clg_ip_address = '$ip' ";
        $sql .= "   and clg_clt_code = 'ACCOUNT_CREATION' )";

        include(include_content('customers/cust_search_exec.inc'));
    }

    $post_vars = array('page'=>$page,'search_string'=>$search_string, 'filter'=>$filter);
    include(include_content('customers/cust_search_show_results.inc'));
}
else
{
    ?>
    <div class=subnavmenu>
        <?printLink(refpage('customers/customers'), "");?>
        &nbsp;&gt;&nbsp;
        <b></b>
    </div><br>
    <?

    $cache_filename = 'ip_addr_cache.'.session_id();


    if ( $doQuery )
    {  $sql  = "select clg_ip_address, count(*) from customer_log";
        $sql .= " where clg_clt_code = 'ACCOUNT_CREATION' group by clg_ip_address";
        $res =sqlExec($conn, $sql);
        $num_rows = sqlNumRows($res);

        if ( $num_rows > 0 )
        {  $cache_writer = new CacheFileWriter($cache_filename);
            $cache_writer->open();

            for ( $i=0; $i<$num_rows; $i++ )
            {  $row = sqlResult($res, $i);
                $ip_address = $row[0];
                $ip_count   = $row[1];
                $ip_country = $row[2] = ipToCountryName($ip_address);

                $ip_address = getLink(refPage("$page&phase=viewaccounts&ip=$ip_address&doQuery=yes"),  $ip_address, "_self", 8);


                if ( isBlank($ip_country) )
                    $ip_country = $row[2] = "<font color=red></font>";

                $cache_writer->writeLine("$ip_address|$ip_country|$ip_count");
            }
            ?>
            <?    $cache_writer->close();
            $show_search_results = true;
            $num_rows_displayed =0;
        }
    }

    if ( $show_search_results )
    {  if ( file_exists(cache_file_dir."/$cache_filename") )
    {  if ( 'desc' == $asc_desc )
    {  $link = refPage($page)."&$filter&asc_desc=asc&orderby=$orderby";
        $arrow =  ' <a href="'.$link.'"><img src="'.image_dir.'/arrow_white_down.gif" border=0></a>';
        $link_asc_desc = 'asc';
    }
    else
    {  $link = refPage($page)."&$filter&asc_desc=desc&orderby=$orderby";
        $arrow =  ' <a href="'.$link.'"><img src="'.image_dir.'/arrow_white_up.gif" border=0></a>';
        $link_asc_desc = 'desc';
    }

        switch ($orderby )
        {  case "ip":
            $head_ip_arrow = $arrow;
            break;
            case "country":
                $head_country_arrow = $arrow;
                break;
            default:
                $head_count_arrow = $arrow;
                break;
        }

        $c_displayer = new CacheFileDisplayManager($cache_filename, $num_rows, $max_rows='30', $num_rows_displayed);
        $c_displayer->open();

        $head_ip = getLink2(refPage($page)."&orderby=ip&asc_desc=$link_asc_desc&doQuery=yes", "", "_self", 8).$head_ip_arrow;
        $head_country = getLink2(refPage($page)."&orderby=country&asc_desc=$link_asc_desc&doQuery=yes", "", "_self", 8).$head_ip_arrow;
        $head_count = getLink2(refPage($page)."&orderby=count&asc_desc=$link_asc_desc&doQuery=yes", "", "_self", 8).$head_ip_arrow;

        $disp_heading = array($head_ip, $head_country, $head_count);
        $field_disp = array(0,1,2);

        $post_vars = array('page'=>$page);

        $c_displayer->writeTable($disp_heading, $field_disp, $financial='', '#0044A7', '#D0DEF2', 'white', 'black', $post_vars);
        $c_displayer->close();
    }
    }
}
?>