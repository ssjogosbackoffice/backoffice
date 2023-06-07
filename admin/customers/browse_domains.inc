<?include 'cache_file.class.inc';?>
    <div class=bodyHD></div>
<?
//$conn = casinoConnect();
globalise('start_row');
globalise('num_rows');
globalise('max_rows');
globalise('doQuery');
globalise('asc_desc');
globalise('phase');
globalise('domain');

if ( 'viewaccounts' == $phase )
{  $serialize_filename = "cust_ip_address.".session_id();
    ?>
    <div class=subnavmenu>
        <?printLink(refpage('customers/customers'), "Main Menu");?>
        &nbsp;&gt;&nbsp;
        <?printLink(refpage("$page&doQuery=yes"), "");?>
        &nbsp;&gt;&nbsp;
        <b></b>
    </div><br>
    <?
    $filter = "domain=$domain";

    if ( $doQuery )
    {  include "cust_list_sql_head.inc";
        $sql .= " and substring(pun_email from position('@' in pun_email)+1 for char_length(pun_email)) = '$domain'";

        include(include_content('customers/cust_search_exec.inc'));
    }

    $post_vars = array('page'=>$page,'search_string'=>$search_string, 'filter'=>$filter);
    include(include_content('customers/cust_search_show_results.inc'));
}
else
{
    ?>
    <div class=subnavmenu>
        <?printLink(refpage('customers/customers'), "Main Menu");?>
        &nbsp;&gt;&nbsp;
        <b></b>
    </div><br>
    <?
    $cache_filename = 'ip_addr_cache.'.session_id();

    if ( $doQuery )
    {  $sql  = "select distinct substring(pun_email from position('@' in pun_email)+1 for char_length(pun_email))  " .
        "as \"domain\", count(*) from punter ";

        if($_SESSION['betting_club'])
            $sql .= "WHERE punter.pun_betting_club = " . $_SESSION['betting_club'] . " ";

        $sql .= "group by substring(pun_email from position('@' in pun_email)+1 for char_length(pun_email)) ";

        switch ($orderby )
        {
            case "domain":
                $sql .= "order by substring(pun_email from position('@' in pun_email)+1 for char_length(pun_email))";
                break;
            case "country":
                $head_country_arrow = $arrow;
                $sql .= "order by cou_name";
                break;
            default:
                $head_count_arrow = $arrow;
                $sql .= "order by count(*)";
                break;
        }


        if ( 'desc' == $asc_desc )
            $sql .= " desc";
        else
            $sql .= " asc";

        // $res = sqlExec($conn, $sql);
        // $num_rows = sqlNumRows($res);

        $res = $dbh->exec($sql);
        $num_rows = $res->getNumRows();


        if ( $num_rows > 0 )
        {
            $cache_writer = new CacheFileWriter($cache_filename);
            $cache_writer->open();

            for ( $i=0; $i<$num_rows; $i++ )
            {
                //$row = sqlResult($res, $i);
                $row           = $res->next();

                $domain        = $row['domain'];
                $domain_count  = $row['count'];
                $ip_address    = getHostByName($domain);
                $ip_country    = $row[2] = ipToCountryName($ip_address);

                $domain = getLink(refPage("$page&phase=viewaccounts&domain=$domain&doQuery=yes"),  $domain, "");

                if ( isBlank($ip_country) )
                    $ip_country = $row[2] = "<font color=red>Not found</font>";

                $cache_writer->writeLine("$domain|$ip_address|$ip_country|$domain_count");
            }
            ?>
            <?       $cache_writer->close();
            $show_search_results = true;
            $num_rows_displayed =0;
        }
    }

    if ( $show_search_results )
    {  if ( file_exists(cache_file_dir."/$cache_filename") )
    {  if ( 'desc' == $asc_desc )
    {  $link = refPage($page)."&$filter&asc_desc=asc&orderby=$orderby";
        $arrow =  ' <a href="'.$link.'"><img src="'.image_dir.'/arrow_down_formlabel_off.gif" border=0></a>';
        $link_asc_desc = 'asc';
    }
    else
    {  $link = refPage($page)."&$filter&asc_desc=desc&orderby=$orderby";
        $arrow =  ' <a href="'.$link.'"><img src="'.image_dir.'/arrow_up_formlabel_off.gif" border=0></a>';
        $link_asc_desc = 'desc';
    }

        switch ($orderby )
        {  case "domain":
            $head_domain_arrow = $arrow;
            break;
            case "ip":
                $head_ip_arrow = $arrow;
                break;
//            case "country":
//               $head_country_arrow = $arrow;
//               break;
            default:
                $head_count_arrow = $arrow;
                break;
        }

        $c_displayer = new CacheFileDisplayManager($cache_filename, $num_rows, $max_rows='30', $num_rows_displayed);
        $c_displayer->open();

        $head_domain  = getLink(refPage($page)."&orderby=domain&asc_desc=$link_asc_desc&doQuery=yes", "", "").$head_domain_arrow;
        $head_country = getLink(refPage($page)."&orderby=country&asc_desc=$link_asc_desc&doQuery=yes", "", "").$head_country_arrow;
        $head_count   = getLink(refPage($page)."&orderby=count&asc_desc=$link_asc_desc&doQuery=yes", "", "").$head_count_arrow;


//         $head_domain = getLink2(refPage($page)."&orderby=domain&asc_desc=$link_asc_desc&doQuery=yes", "Domain", "_self", 8).$head_domain_arrow;
//         $head_country = getLink2(refPage($page)."&orderby=country&asc_desc=$link_asc_desc&doQuery=yes", "Country", "_self", 8).$head_country_arrow;
//         $head_count = getLink2(refPage($page)."&orderby=count&asc_desc=$link_asc_desc&doQuery=yes", "Number of Accounts", "_self", 8).$head_count_arrow;

        $disp_heading = array($head_domain, "", "", $head_count);
        $field_disp = array(0,1,2, 3);

        $post_vars = array('page'=>$page);

        $c_displayer->writeTable($disp_heading, $field_disp, $financial='', '#0044A7', '#D0DEF2', 'white', 'black', $post_vars);
        $c_displayer->close();
    }
    }
}
?>