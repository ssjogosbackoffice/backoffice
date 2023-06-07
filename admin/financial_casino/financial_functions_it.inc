<?php
$query_jur_selectors = array(
    "nation" => "n.jur_name n_jur_name",
    "region" => "n.jur_name n_jur_name, r.jur_name r_jur_name",
    "district" => "n.jur_name n_jur_name, r.jur_name r_jur_name, d.jur_name d_jur_name",
    "club" => "n.jur_name n_jur_name, r.jur_name r_jur_name, d.jur_name d_jur_name, c.jur_name c_jur_name"
);
$query_jur_orders = array(
    "nation" => "n.jur_name",
    "region" => "n.jur_name, r.jur_name",
    "district" => "n.jur_name, r.jur_name, d.jur_name",
    "club" => "n.jur_name, r.jur_name, d.jur_name, c.jur_name"
);
$query_jur_groups = array(
    "nation" => "n.jur_id",
    "region" => "n.jur_id, r.jur_id",
    "district" => "n.jur_id, r.jur_id, d.jur_id",
    "club" => "n.jur_id, r.jur_id, d.jur_id, c.jur_id"
);
function getFinancialData($date_start, $date_end, $jur_totals, $betwin, $financial,$jur_level, $jur_sel_id){
    global $dbh, $query_jur_selectors,$query_jur_groups, $query_jur_orders;
    $sql = "select coalesce(jps1.jps_perc_casino_live, 0) as jps_perc_casino_live,
	            coalesce(jps1.jps_perc_casino, 0) as jps_perc_casino,
	            coalesce(jps1.jps_perc_poker_live, 0) as jps_perc_poker_live,
	            coalesce(jps1.jps_perc_virtual, 0) as jps_perc_virtual,
	            coalesce(jps1.jps_perc_gaminator, 0) as jps_perc_gaminator,";
    if($_SESSION['jurisdiction_class']!='club'){

        $sql .="   coalesce(jps2.jps_perc_casino_live, 0) as jps_perc_casino_live2,
	            coalesce(jps2.jps_perc_casino, 0) as jps_perc_casino2,
	            coalesce(jps2.jps_perc_poker_live, 0) as jps_perc_poker_live2,
	            coalesce(jps2.jps_perc_virtual, 0) as jps_perc_virtual2,
	            coalesce(jps2.jps_perc_gaminator, 0) as jps_perc_gaminator2,";
    }
    $sql.="cus_res_day,
                cus_res_club_id, ";

    if($betwin){
        $sql .= "sum(if (gam_category = 'casino', cus_res_bet, 0)) as casino_bet,
                 sum(if (gam_category = 'casino', cus_res_win, 0)) as casino_win,
                 sum(if (gam_category = 'casino_live', cus_res_bet, 0)) as casino_live_bet,
                 sum(if (gam_category = 'casino_live', cus_res_win, 0)) as casino_live_win,
                 sum(if (gam_category = 'casino_agency', cus_res_bet, 0)) as virtual_bet,
                 sum(if (gam_category = 'casino_agency', cus_res_win, 0)) as virtual_win,
                 sum(if (gam_category = 'provider_poker_live', cus_res_bet, 0)) as poker_live_bet,
                 sum(if (gam_category = 'provider_poker_live', cus_res_win, 0)) as poker_live_win,
                 sum(if (gam_category = 'provider_gaminator', cus_res_bet, 0)) as gaminator_bet,
                 sum(if (gam_category = 'provider_gaminator', cus_res_win, 0)) as gaminator_win,
                 sum(if (gam_category = 'provider_poker_live', cus_res_fin, 0)) as poker_live_fin, ";
    }
    if($financial){
        $sql .= "sum(if (gam_category = 'provider_gaminator', cus_res_fin, 0)) as gaminator_fin,
                 sum(if (gam_category = 'casino_live', cus_res_fin, 0)) as casino_live_fin,
                 sum(if (gam_category = 'casino_agency', cus_res_fin, 0)) as virtual_fin,
                 sum(if (gam_category = 'casino', cus_res_fin, 0)) as casino_fin, ";
    }

    $sql .= " sum(if (gam_category = 'casino', cus_res_rake, 0)) as casino_rake,
                sum(if (gam_category = 'casino_live', cus_res_rake, 0)) as casino_live_rake,
                sum(if (gam_category = 'casino_agency', cus_res_rake, 0)) as virtual_rake,
                sum(if (gam_category = 'provider_poker_live', cus_res_rake, 0)) as poker_live_rake,
                sum(if (gam_category = 'provider_gaminator', cus_res_rake, 0)) as gaminator_rake ,

                $query_jur_selectors[$jur_totals]

                from
                    customer_result_daily
                        LEFT JOIN game on gam_id = cus_res_gam_id
                        LEFT JOIN jurisdiction_payment_settings jps1 on jps1.jps_jur_id = ".$_SESSION['jurisdiction_id']."
                        JOIN jurisdiction c on c.jur_id = cus_res_club_id
                        JOIN jurisdiction d on d.jur_id = c.jur_parent_id
                        JOIN jurisdiction r on r.jur_id = d.jur_parent_id
                        JOIN jurisdiction n on n.jur_id = r.jur_parent_id";
    if($_SESSION['jurisdiction_class']=='casino'){

        $sql .="   LEFT JOIN jurisdiction_payment_settings jps2 on jps2.jps_jur_id = n.jur_id ";
    }
    if($_SESSION['jurisdiction_class']=='nation'){

        $sql .="   LEFT JOIN jurisdiction_payment_settings jps2 on jps2.jps_jur_id = r.jur_id ";
    }
    if($_SESSION['jurisdiction_class']=='region'){

        $sql .="   LEFT JOIN jurisdiction_payment_settings jps2 on jps2.jps_jur_id = d.jur_id ";
    }
    if($_SESSION['jurisdiction_class']=='district'){

        $sql .="   LEFT JOIN jurisdiction_payment_settings jps2 on jps2.jps_jur_id = c.jur_id ";
    }
    $sql .="       WHERE
                                cus_res_day BETWEEN '$date_start 00:00:00' AND '$date_end 23:59:59' ";

    if($jur_sel_id == ""){
        if($_SESSION['jurisdiction_class'] == 'nation'){
            $sql .= " and r.jur_parent_id = ". $_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class'] == 'region'){
            $sql .= " and d.jur_parent_id = ". $_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class'] == 'district'){
            $sql .= " and c.jur_parent_id = ". $_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class'] == 'club'){
            $sql .= " and c.jur_id = " . $_SESSION['jurisdiction_id'];
        }
    }else{
        if($jur_level == 'club'){
            $sql .= " and c.jur_id = $jur_sel_id";
        }
        if($jur_level == 'district'){
            $sql .= " and d.jur_id = $jur_sel_id";
        }
        if($jur_level == 'region'){
            $sql .= " and r.jur_id = $jur_sel_id";
        }
        if($jur_level == 'nation'){
            $sql .= " and n.jur_id = $jur_sel_id";
        }
    }
    $sql.=" GROUP BY $query_jur_groups[$jur_totals]
                                        ORDER BY $query_jur_orders[$jur_totals]";
    $result = $dbh->doCachedQuery($sql, 0);
    return $result;

}

function getStartJurisdictionLevel(){
    if($_SESSION['jurisdiction_class'] == 'casino'){
        return 'nation';
    }
    if($_SESSION['jurisdiction_class'] == 'nation'){
        return 'region';
    }
    if($_SESSION['jurisdiction_class'] == 'region'){
        return 'district';
    }
    if($_SESSION['jurisdiction_class'] == 'district'){
        return 'club';
    }
}
?>

<?php
function getRadioGroupsLevel($jur_level, $jur_selected){
    if ('nation' == $jur_level) {
        $str = '<input type="radio" value="nation" name="jur_level"';
        if($jur_selected == 'nation') $str .='checked';
        $str.='>Nation</input>';

        $str .= '<input type="radio" value="region" name="jur_level"';
        if($jur_selected == 'region') $str .='checked';
        $str.='>Region</input>';

        $str .= '<input type="radio" value="district" name="jur_level"';
        if($jur_selected == 'district') $str .='checked';
        $str.='>District</input>';

        $str .= '<input type="radio" value="club" name="jur_level"';
        if($jur_selected == 'club') $str .='checked';
        $str.='>Betting Club</input>';
        return $str;
    }
    if ('region' == $jur_level){
        $str = '<input type="radio" value="region" name="jur_level"';
        if($jur_selected == 'region') $str .='checked';
        $str.='>Region</input>';

        $str .= '<input type="radio" value="district" name="jur_level"';
        if($jur_selected == 'district') $str .='checked';
        $str.='>District</input>';

        $str .= '<input type="radio" value="club" name="jur_level"';
        if($jur_selected == 'club') $str .='checked';
        $str.='>Betting Club</input>';
        return $str;
    }
    if ('district' == $jur_level){
        $str = '<input type="radio" value="district" name="jur_level"';
        if($jur_selected == 'district') $str .='checked';
        $str.='>District</input>';

        $str .= '<input type="radio" value="club" name="jur_level"';
        if($jur_selected == 'club') $str .='checked';
        $str.='>Betting Club</input>';
        return $str;
    }

    $str = '<input type="radio" value="club" name="jur_level"';
    if($jur_selected == 'club') $str .='checked';
    $str.='>Betting Club</input>';
    return $str;
}


function getFinancialByCustomer($clubid,$date_start, $date_end, $betwin, $financial) {
    global $dbh,$lang;
    $sql = " select pun_username, coalesce(jps_perc_casino_live, 0), cus_res_day, jur_name, ";
    if($betwin)
    {
        $sql .= "
	      sum(if (gam_category = 'casino_live', cus_res_bet, 0)) as casino_live_bet,
	      sum(if (gam_category = 'casino_live', cus_res_win, 0)) as casino_live_win,
	       ";
    }
    if($financial)
    {
        $sql .="
	     sum(if (gam_category = 'casino_live', cus_res_fin, 0)) as casino_live_fin,
	     ";
    }


    $sql .= "
    sum(if (gam_category = 'casino_live', cus_res_rake, 0)) as casino_live_rake
    from customer_result_daily
    LEFT JOIN game on gam_id = cus_res_gam_id
    LEFT JOIN jurisdiction_payment_settings on jps_jur_id = cus_res_club_id
    LEFT JOIN punter on pun_id = cus_res_pun_id
    LEFT JOIN jurisdiction on jur_id = cus_res_club_id
        WHERE cus_res_club_id = '$clubid'
              AND cus_res_day BETWEEN '$date_start 00:00:00' AND '$date_end 23:59:59'
            GROUP BY cus_res_pun_id";

    $result = $dbh->doCachedQuery($sql, 0);
    if(!$result->hasNext()){
        ?>
        <span class="tip"><?=$lang->getLang("No result found")?></span>
        <?
        return;
    }
    else {
        ?>
        <table style="width:100%;" id="paginator">
        <tr class="header">
            <th class="label centered" style="width:300px;"><?=$lang->getLang("Customer")?></th>
            <?php
            if($betwin)
            {?>
                <th class="label centered">Casino<br>Live Bet</th>
                <th class="label centered">Casino<br>Live Win</th>
            <?php }
            ?>
            <th class="label centered">Casino<br>Live Rake</th>
            <?php
            if($financial)
            {
                ?>
                <th class="label centered">Casino<br>Live<br>Financial</th>
            <?php }
            ?>
        </tr>
        <?
        while($result->hasNext()){
            $row = $result->next();
            ?>
            <tr class='odd'>
                <td class="content light" style="text-align:left;" ><?=$row["pun_username"]?></td>
                <?php

                if($betwin)
                {?>
                    <td class="content dark" style="text-align:right"><?=getInDollars($row["casino_live_bet"])?></td>
                    <td class="content dark" style="text-align:right"><?=getInDollars($row["casino_live_win"])?></td>
                <?php }?>
                <td class="content dark" style="text-align:right"><?=getInDollars($row["casino_live_rake"])?></td>
                <?php
                if($financial)
                {
                    ?>
                    <td class="content dark" style="text-align:right"><?=getInDollars($row["casino_live_fin"])?></td>
                <?php }
               ?>
            </tr>
        <?
        }
        ?>
        <?if ($result->NumRows> 20){?>
            <div id="paginator_control" style="width:100%">
                <link href="media/smartpaginator/smartpaginator.css" rel="stylesheet" type="text/css" />
                <script src="media/smartpaginator/smartpaginator.js" type="text/javascript"></script>
                <script>
                    $('#paginator_control').smartpaginator({ totalrecords:<?=$result->NumRows?>,
                        recordsperpage: 20,
                        datacontainer: 'paginator',
                        dataelement: 'tr',
                        theme: 'black'});
                </script>
            </div>
        <?
        }
    }
}
?>