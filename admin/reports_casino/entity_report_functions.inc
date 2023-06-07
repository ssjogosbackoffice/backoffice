<?php

if(!isset($_SESSION['jurisdiction_id'])||$_SESSION['jurisdiction_id']=='')
{
    ?>
    <div class="error"><?=$lang->getLang("Please login again!")?></div>
    <?
    die();
}
include_once 'JurisdictionsList.class.inc';
$jurClass = JurisdictionsList::getInstance ( $dbh );
$hierarchy = $jurClass->getJurisdictionTree($_SESSION['jurisdiction_id']);

switch ($_POST['action'])
{
    case "0":
    {
        $jurisdictionid=$_POST['jurid'];
        showJurisdictionReport($jurisdictionid);
    }
        break;

    case "1":
    {
        $jurisdictionid=$_POST['jurid'];
        $totals=$_POST['results'];
        if(isset($_POST['startrow']))
        {
            $startrow=$_POST['startrow'];
        }
        else
        {
            $startrow='0';
        }
        showClubReport($jurisdictionid,$totals,$startrow);
    }
        break;
    case "2":
    {
        $jurisdictionid=$_POST['jurid'];
        $totals=$_POST['results'];
        if(isset($_POST['startrow']))
        {
            $startrow=$_POST['startrow'];
        }
        else
        {
            $startrow='0';
        }
        refreshClubReport($jurisdictionid,$totals,$startrow);
    }
        break;
    case "3":
    {
        $customer=$_POST['customer'];
        $jurid=$_POST['jurid'];
        searchSpecificCustomer($customer,$jurid);
    }
        break;
    case "4":
    {
        $jur_id=$_POST['jur_id'];
        $totals=$_POST['results'];
        if(isset($_POST['startrow']))
        {
            $startrow=$_POST['startrow'];
        }
        else
        {
            $startrow='0';
        }
        $start_date=$_POST['start_date'];
        $end_date=$_POST['end_date'];
        if(isset($_POST['recperpage']))
        {
            $recperpage=$_POST['recperpage'];
        }
        else
        {
            $recperpage='1000';
        }
        refreshTransactionHistory($jur_id,$totals,$startrow,$start_date,$end_date,$recperpage);
    }

        break;
    case "5":
    {
        $jurisdiction=$_POST['jurid'];
        $class=$_POST['jurclass'];
        $fclub=$_POST['fclub'];
        die(getJurisdictionSubtotals($jurisdiction,$class,$fclub));
    }
        break;
}

function showJurisdictionReport($jurid)
{
    global $dbh,$lang,$symbol;


    /*$sql="select @temp_jur_id := jur_id as jur_id,jur_status, jur_name, jur_class, jur_available_credit,jur_overdraft,jur_overdraft_start_time,jur_reserved_fund,
    jur_tot_overdraft_received,
    IF(jur_class = 'club', (SELECT count( * ) FROM punter WHERE pun_betting_club = jur_id),
    (SELECT count( * ) FROM jurisdiction j2 WHERE j2.jur_parent_id = @temp_jur_id)
    ) as sub_agency_num	from jurisdiction where ";
    */
    $sql="
    select @temp_jur_id := jur_id as jur_id,
          jur_status,
          jur_name,
          jur_class,
          cur_code_for_web,
          @affiliateId := IF(jur_class = 'club', (SELECT aff_id FROM admin_user, affiliate WHERE aus_jurisdiction_id = jur_id AND aus_id = aff_aus_id AND aff_is_club = 1), 0) as affiliateId,
          IF(jur_class = 'club' AND @affiliateId > 0, (SELECT act_credits FROM affiliate_credit WHERE act_aff_id = @affiliateId), jur_available_credit) as jur_available_credit,
          IF(jur_class = 'club' AND @affiliateId > 0, (SELECT act_overdraft FROM affiliate_credit WHERE act_aff_id = @affiliateId), jur_overdraft) as jur_overdraft,
          IF(jur_class = 'club' AND @affiliateId > 0, (SELECT act_overdraft_start_time FROM affiliate_credit WHERE act_aff_id = @affiliateId), jur_overdraft_start_time) as jur_overdraft_start_time,
          IF(jur_class = 'club' AND @affiliateId > 0, (SELECT act_tot_overdraft_received FROM affiliate_credit WHERE act_aff_id = @affiliateId), jur_tot_overdraft_received) as jur_tot_overdraft_received,
          IF(jur_class = 'club', (SELECT count( * ) FROM punter WHERE pun_betting_club = jur_id), (SELECT count( * ) FROM jurisdiction j2 WHERE j2.jur_parent_id = @temp_jur_id) ) as sub_agency_num,
          IF(jur_class = 'club', (SELECT count( * ) FROM punter WHERE pun_betting_club = @temp_jur_id AND pun_aff_id > 0 AND pun_aus_id > 0) - 1, 0) as num_sub_affiliate
          from jurisdiction
          LEFT JOIN currency on jur_currency=cur_id
          WHERE  ";

    if($_SESSION['jurisdiction_class']=='club'){
        $sql.=" jur_id = ".$dbh->escape($jurid);
    }
    else{
        $sql.=" jur_parent_id =".$dbh->escape($jurid);
    }
       $result  = $dbh->exec($sql);
    if(!$result->hasNext()){
        ?>
        <span class="tip"><?=$lang->getLang("No result found")?></span>
        <?
        return;
    }
    else {
        while($result->hasNext()){
            $row = $result->next();
            $symbol=$row['cur_code_for_web'];
            ?>
            <div style="margin-left:20px">
                <table style="width:100%;border:none;border-collapse:collapse;">
                    <tr class="<?=$row['jur_class']?>entity">
                        <td <?if ($row['jur_status']!='1'){?>style="text-decoration:line-through;color:red;"<? } ?> ><img id="img_<?=$row['jur_id']?>" style="margin-left:10px;width:13px;cursor:pointer;height:11px;" src="media/images/branch_expand.gif" onclick="<?if($row['jur_class']!='club'){?>getSubEntity<?}else{?>getClubEntity<?}?>('<?=$row['jur_id']?>','<?=$row['sub_agency_num']?>','<?=$row['jur_class']?>_details');">
                                   <span class="tip">
                                       <?=ucfirst($row['jur_class']) ?>

                                       <? if($row['jur_class']=='club' && $row['affiliateid'] !='' )  { ?>
                                           ( Affiliate)
                                       <? } ?>

                                       <?="&nbsp;".($result->CurrentIndex+1)."/".$result->NumRows?> </span>
                            <span style="font-weight:bold"><?=$row['jur_name']?></span>
                                   <span class="tip"><?=$row['sub_agency_num']."  ".subagency($row['jur_class'])?>
                                       <? if($row['jur_class']=='club' && $row['affiliateid'] !='' )  { ?>
                                           / <?=$row['num_sub_affiliate'] ?> subaffiliate<?=($row['num_sub_affiliate']> 1 ? 's':'')?>
                                       <? } ?>
                                   </span>
                            <span id="<?=$row['jur_id']?>Totals" class="fright" style="height: 100%">
                                <? if($_SESSION['jurisdiction_class']=='club'){
                                    $forceclub=1;
                                }
                                else{
                                    $forceclub=0;
                                }
                                ?>
                                <button  class='entitybtn' onclick="getSubEntityTotals('<?=$row['jur_id']?>','<?=$row['jur_class']?>','<?=$row['jur_id']?>Totals',<?=$forceclub?>)"><?=$lang->getLang("Show totals")?></button>
                            </span>
                        </td>
                        <td style="width:185px;<?if ($row['jur_status']=='0'){?> text-decoration:line-through;color:red;<? } ?> text-align:right;">
                            <?=$lang->getLang("Credit")?> <span style="color:<?if($row['jur_available_credit']< 0){?>red<?}else{?>green<?}?>"> <?=getInDollars($row['jur_available_credit'])?><br></span>
                            <?=$lang->getLang("Overdraft")?> <span style="color:<?if($row['jur_overdraft']< 0){?>red<?}else{?>green<?}?>"><?=getInDollars($row['jur_overdraft'])?></span></td>
                        <td style='width: 20px'><a href="/reports_casino/transactions.php?jur_id=<?=$row['jur_id'];?>"><img src="<?=image_dir?>/icon_financial.gif" border="0" style='width:20px; height: 15px;'></a>
                            <div class="infoTooltip"  id="<?=$row['jur_id']?>_tooltipDiv">
                                <img src="<?=image_dir?>/money.png" width="20px" height="20px" style="vertical-align: bottom;cursor: pointer" onclick="showtooltiptable('<?=$row['jur_id']?>_tooltipDiv')" >
                                <table class="tooltiptable" >
                                    <tr><td class="label" colspan="2"> <?=$lang->getLang("Additional information for")?> <?=$row['jur_name']?><br /></td> </tr>
                                    <tr><td class="label"> <?=$lang->getLang("Credits")?>: </td><td><?=getInDollars($row['jur_available_credit'])?></td></tr>
                                    <tr><td class="label"> <?=$lang->getLang("Overdraft")?>: </td><td><?=getInDollars($row['jur_overdraft'])?></td></tr>
                                    <tr><td class="label"> <?=$lang->getLang("Total overdraft received")?>: </td><td><?=getInDollars($row['jur_tot_overdraft_received'])?></td></tr>
                                    <tr><td class="label"> <?=$lang->getLang("Reserved funds")?> : </td><td><?=getInDollars($row['jur_reserved_fund'])?></td></tr>
                                    <tr><td class="label"><?=$lang->getLang("Overdraft start time")?> : </td><td><?=($row['jur_overdraft_start_time']!='' ? $row['jur_overdraft_start_time']:'none') ?></td></tr>
                                </table>
                            </div>
                        </td>
                    </tr>
                    <tr><td colspan="3"><div style="margin-left:20px;display:none;"  class="<?=$row['jur_class']?>_details" id="<?=$row['jur_id']?>"></div></td></tr>
                </table>
            </div>
        <?
        }
    }
}
function showClubReport($jurid,$totals,$startrow)
{
    global $dbh,$lang,$symbol;

    $sql="SELECT pun_reg_date,pun_member_type, pun_username, pun_access, pcr_credits,pcr_bonus,cur_code_for_web
			FROM punter
			JOIN punter_credit on pcr_pun_id=pun_id
			JOIN jurisdiction on pun_betting_club=jur_id
			JOIN currency on jur_currency=cur_id
			WHERE pun_betting_club = '$jurid'
			and pun_id = pcr_pun_id  LIMIT $startrow, 1000 ";
    $result  = $dbh->doCachedQuery($sql,60);
    if(!$result->hasNext()){
        ?>
        <span class="tip"><?=$lang->getLang("No result found")?></span>
        <?
        return;
    }
    else {
        if ($totals>1000){
            ?>
            <button style="margin-left:20px" id="toggle_<?=$jurid?>"><?= $lang->getLang("Search a specific user")?></button>
            <script>
                $(function() {
                    $( '#toggle_<?=$jurid?>')
                        .button()
                        .click(function() {
                            toggle('search_entity_<?=$jurid?>');
                        });
                    return false;
                });
            </script>
            <div id="search_entity_<?=$jurid?>" style="border:1px solid lightblue;margin:5px 0px 10px 20px" class="ttm_details">
                <div id="search_entity_form_<?=$jurid?>" style="margin:10px 0px 0px 10px">
                    <table>
                        <tr><td class="label">Username: </td>
                            <td class="content">
                                <input type="text" name="entity_name_<?=$jurid?>" value="">
                                <input type="hidden" name="jurisdiction_<?=$jurid?>" value="<?=$jurid?>">
                            </td>
                        </tr>
                        <tr><td colspan="2"><button id="do_entity_search_<?=$jurid?>"><?=$lang->getLang('Search')?></button>
                                <script>
                                    $(function() {
                                        $( '#do_entity_search_<?=$jurid?>')
                                            .button()
                                            .click(function() {
                                                $.post("reports_casino/entity_report_functions.inc",{customer:$('input:text[name=entity_name_<?=$jurid?>]').val(),jurid:$('input:hidden[name=jurisdiction_<?=$jurid?>]').val(),action:"3"}, function(data){
                                                    $('#search_entity_result_<?=$jurid?>').empty().append( data );
                                                });
                                                $('#search_entity_result_<?=$jurid?>').slideDown('fast');;
                                            });
                                        return false;
                                    });
                                </script>
                            </td>
                        </tr>
                    </table>
                </div>
                <div id="search_entity_result_<?=$jurid?>" style="margin:10px">
                </div>
            </div>
        <? }?>
        <div style="margin-left:20px" id="paginator_<?=$jurid?>">
            <?
            while($result->hasNext()){
                $row = $result->next();
                $symbol=$row['cur_code_for_web'];
                ?>
                <p class="totementity">
                    <span <?if(strtoupper($row['pun_access'])=='DENY'){?>style="text-decoration:line-through;color:red"<?}?>><span class="tip" style="margin-left:20px"><?=ucfirst($row['pun_member_type'])."&nbsp; ".($result->CurrentIndex+1+$startrow)."/".$totals?> </span> <span style="font-weight:bold"><?=$row['pun_username']?></span></span><span style="float:right;display:table;text-align:right;color:<?if($row['pcr_credits']==0){?>red<?}else{?>green<?}?>"><?if(strtolower($row['pun_member_type'])!='affiliate'){?>Credit <?=getInDollars($row['pcr_credits'])?> ( Bonus: <?=getInDollars($row['pcr_bonus'])?> ) <? } ?></span>
                </p>
            <?
            }
            ?>
        </div>
        <?if ($totals>1000){?>
            <div id="paginator_control_<?=$jurid?>" style="margin-left:20px">
                <link href="media/smartpaginator/smartpaginator.css" rel="stylesheet" type="text/css" />
                <script src="media/smartpaginator/smartpaginator.js" type="text/javascript"></script>
                <script>
                    $('#paginator_control_<?=$jurid?>').smartpaginator({ totalrecords: <?=$totals?>,
                        recordsperpage: 1000,
                        theme: 'black',
                        controlsalways:true,
                        onchange: function(recordStartIndex) {
                            $.post("reports_casino/entity_report_functions.inc",{action:"2",jurid:<?=$jurid?>,results:"<?=$totals?>",startrow:recordStartIndex}, function(data){
                                $('#paginator_<?=$jurid?>').fadeOut("slow").fadeIn("slow").empty().append( data );
                            });
                        }
                    });
                </script>
            </div>
        <?
        }
    }

}

function refreshClubReport($jurid,$totals,$startrow)
{
    global $dbh,$lang;
    $sql="SELECT pun_reg_date,pun_member_type, pun_username, pun_access, pcr_credits
	FROM punter, punter_credit
	WHERE pun_betting_club = '$jurid'
	and pun_id = pcr_pun_id  LIMIT $startrow, 1000 ";
    $result  = $dbh->doCachedQuery($sql,60);
    if(!$result->hasNext()){
        ?>
        <span class="tip"><?=$lang->getLang("No result found")?></span>
        <?
        return;
    }
    else {
        while($result->hasNext()){
            $row = $result->next();
            ?>
            <p class="totementity">
						    <span  <?if(strtoupper($row['pun_access'])=='DENY'){?>style="text-decoration:line-through;color:red"<?}?>>
						   <span class="tip" style="margin-left:20px"><?=ucfirst($row['pun_member_type'])."&nbsp; ".($result->CurrentIndex+1+$startrow)."/".$totals?> </span> <span style="font-weight:bold"><?=$row['pun_username']?></span></span><span style="float:right;text-align:right;color:<?if($row['pcr_credits']==0){?>red<?}else{?>green<?}?>">Credit <?=getInDollars($row['pcr_credits'])?></span>
            </p>
        <?
        }
    }
}


function refreshTransactionHistory($jur_id,$totals,$id_start,$start_date,$end_date,$recperpage)
{
    global $dbh,$lang;
    $starttime = microtime(true);
    $sql = "SELECT *, (select jur_name FROM jurisdiction WHERE jur_id = cre_entity_id) jur_entity_name,
            (select jur_name FROM jurisdiction WHERE jur_id = cre_other_entity_id) jur_other_entity_name
            FROM credit_transfer
            WHERE DATE(cre_time)
                BETWEEN '$start_date 00:00:00' AND '$end_date 23:59:59'
                  AND cre_entity_id = '$jur_id'
                  order by cre_id desc  LIMIT $id_start,  $recperpage";

    $result  = $dbh->exec($sql);
    $endtime = microtime(true);
    $duration = $endtime - $starttime;
    $duration=number_format($duration, 4, ',', '')."s";
    $html="<script>duration('".$duration."','duration');</script>";
    if(!$result->hasNext()){
        $html.=' <span class="tip">'.$lang->getLang("No result found").'</span>';
        return;
    }
    else {
        $html.="<table cellpadding='5' cellspacing='1' border='0'  style='min-width: 700px;'>
                <tr>
                    <th class='label' style='padding: 5px'>".$lang->getLang('ID')."</th>
                    <th class='label' style='padding: 5px'>".$lang->getLang('Time')."</th>
                    <th class='label' style='padding: 5px'>".$lang->getLang('Entity')."</th>
                    <th class='label' style='padding: 5px'>".$lang->getLang('Other Entity')."</th>
                    <th class='label' style='padding: 5px'>".$lang->getLang('Credit Received')."</th>
                    <th class='label' style='padding: 5px'>".$lang->getLang('Credit Transferred')."</th>
                </tr>";
        while($result->hasNext()){
            $row = $result->next();
            $id                = $row['cre_id'];
            $time              = $row['cre_time'];
            $amount            = $row['cre_amount'];
            $entity = $row['jur_entity_name'];
            $other_entity_name = $row['jur_other_entity_name'];
            $direction         = $row['cre_direction'];
            $bg = ('IN' == $direction) ? 'lightgreen' : '#F3C3C7';
            $lastid=$row['cre_id'];
            $html .="<tr> <td class='content' style='background-color:$bg'>$id</td>
                <td class='content' style='background-color:$bg'>$time</td>
                <td class='content' style='background-color:$bg'>$entity</td>
                <td class='content' style='background-color:$bg'>$other_entity_name</td>
                <td class='content' style='background-color:$bg' align='right'>";
            if ( 'IN' == $direction)
                $html.=getInDollars($amount);
            else
                $html.='&nbsp;';
            $html .="</td>
                <td class='content' style='background-color:$bg' align='right'>";

            if ( 'OUT' == $direction)
            {
                $html.=getInDollars($amount);
            }
            else
            {
                $html.='&nbsp;';
            }
            $html.=" </td>
            </tr>";

        }
        $html.='</table>';
        echo $html;
    }
}


function searchSpecificCustomer($customer,$jurid)
{
    global $dbh,$lang;
    $sql="SELECT pun_reg_date,pun_member_type, pun_username, pun_access, pcr_credits
	FROM punter, punter_credit
	WHERE pun_betting_club = '$jurid'
	AND pun_username='$customer'
	and pun_id = pcr_pun_id   ";
    $result  = $dbh->doCachedQuery($sql,60);
    if(!$result->hasNext()){
        ?>
        <span class="tip"><?=$lang->getLang("There is no customer with this username in this club")?></span>
        <?
        return;
    }
    else {
        while($result->hasNext()){
            $row = $result->next();
            ?>
            <p class="totementity">
						    <span  <?if(strtoupper($row['pun_access'])=='DENY'){?>style="text-decoration:line-through;color:red"<?}?>>
						   <span class="tip" style="margin-left:20px"><?=ucfirst($row['pun_member_type'])."&nbsp; ".($result->CurrentIndex+1)."/".$result->NumRows?> </span> <span style="font-weight:bold"><?=$row['pun_username']?></span></span><span style="float:right;text-align:right;color:<?if($row['pcr_credits']==0){?>red<?}else{?>green<?}?>">Credit <?=getInDollars($row['pcr_credits'])?></span>
            </p>
        <?
        }
    }

}


function subagency($agency)
{
    if($agency=='casino')
    {
        $subagency='nations';
    }
    elseif($agency=='nation')
    {
        $subagency='regions';
    }
    elseif($agency=='region')
    {
        $subagency='districts';
    }
    elseif($agency=='district')
    {
        $subagency='clubs';
    }
    elseif($agency=='club')
    {
        $subagency='customers';
    }
    return $subagency;
}


function getJurisdictionSubtotals($jurisdiction,$class,$fclub=0){
    global $dbh,$lang,$symbol;

    switch ($class){
        case "casino":{
            $sql="  select 'users_credit' jurisdiction, sum(pcr_credits) credit ,sum(pcr_bonus) bonus
                FROM punter_credit
                UNION ALL
                select 'affiliate_credit', coalesce(sum(act_credits), 0),0
                FROM affiliate_credit
                UNION ALL
                SELECT 'nation', sum(n.jur_available_credit) ,0
                FROM jurisdiction n
                WHERE n.jur_class = 'nation'
                UNION ALL
                select 'region', sum(r.jur_available_credit),0
                from jurisdiction n, jurisdiction r
                WHERE n.jur_class = 'nation'
                            AND r.jur_parent_id = n.jur_id
                UNION ALL
                select 'district', sum(d.jur_available_credit),0
                from jurisdiction n, jurisdiction r, jurisdiction d
                WHERE n.jur_class = 'nation'
                            AND r.jur_parent_id = n.jur_id
                            AND d.jur_parent_id = r.jur_id
                UNION ALL
                select 'club', sum(c.jur_available_credit),0
                from jurisdiction n, jurisdiction r, jurisdiction d, jurisdiction c
                WHERE n.jur_class = 'nation'
                            AND r.jur_parent_id = n.jur_id
                            AND d.jur_parent_id = r.jur_id
                            AND c.jur_parent_id = d.jur_id";
        }
            break;

        case "nation":{
            $sql="     select 'users_credit' jurisdiction, sum(pcr_credits) credit  ,sum(pcr_bonus) bonus, (select cur_code_for_web from currency where cur_id=c.jur_currency) as cur_code_for_web
                    FROM punter_credit, punter, jurisdiction c, jurisdiction d, jurisdiction r
                    WHERE pun_id = pcr_pun_id
                    AND pun_betting_club = c.jur_id
                    AND c.jur_parent_id = d.jur_id
                    AND d.jur_parent_id = r.jur_id
                    AND r.jur_parent_id = ".$jurisdiction."
                    UNION ALL
                    select 'affiliate_credit', coalesce(sum(act_credits), 0),0, (select cur_code_for_web from currency where cur_id=c.jur_currency) as cur_code_for_web
                    FROM affiliate_credit, punter, jurisdiction c, jurisdiction d, jurisdiction r
                    WHERE pun_aff_id = act_aff_id
                    AND pun_aus_id <> 0
                    AND pun_betting_club = c.jur_id
                    AND c.jur_parent_id = d.jur_id
                    AND d.jur_parent_id = r.jur_id
                    AND r.jur_parent_id =  ".$jurisdiction;
                    /*UNION ALL
                    SELECT 'n_credits', sum(n.jur_available_credit)
                    FROM jurisdiction n
                    WHERE n.jur_id =  ".$jurisdiction."*/
                    $sql.=" UNION ALL
                    select 'r_credits', sum(r.jur_available_credit),0, (select cur_code_for_web from currency where cur_id=r.jur_currency)
                    from jurisdiction n, jurisdiction r
                    WHERE n.jur_id =  ".$jurisdiction."
                    AND r.jur_parent_id = n.jur_id
                    UNION ALL
                    select 'd_credits', sum(d.jur_available_credit),0, (select cur_code_for_web from currency where cur_id=d.jur_currency)
                    from jurisdiction n, jurisdiction r, jurisdiction d
                    WHERE n.jur_id =  ".$jurisdiction."
                    AND r.jur_parent_id = n.jur_id
                    AND d.jur_parent_id = r.jur_id
                    UNION ALL
                    select 'c_credits', sum(c.jur_available_credit),0, (select cur_code_for_web from currency where cur_id=c.jur_currency) as cur_code_for_web
                    from jurisdiction n, jurisdiction r, jurisdiction d, jurisdiction c
                    WHERE n.jur_id =  ".$jurisdiction."
                    AND r.jur_parent_id = n.jur_id
                    AND d.jur_parent_id = r.jur_id
                    AND c.jur_parent_id = d.jur_id";

        }
            break;



        case "region":{
            $sql="   select 'users_credit' jurisdiction, sum(pcr_credits) credit ,sum(pcr_bonus) bonus, (select cur_code_for_web from currency where cur_id=c.jur_currency) as cur_code_for_web
                FROM punter_credit, punter, jurisdiction c, jurisdiction d
                WHERE pun_id = pcr_pun_id
                    AND pun_betting_club = c.jur_id
                    AND c.jur_parent_id = d.jur_id
                    AND d.jur_parent_id = ".$jurisdiction."
                UNION ALL
                select 'affiliate_credit', coalesce(sum(act_credits), 0),0, (select cur_code_for_web from currency where cur_id=c.jur_currency) as cur_code_for_web
                FROM affiliate_credit, punter, jurisdiction c, jurisdiction d
                WHERE pun_aff_id = act_aff_id
                            AND pun_aus_id <> 0
                            AND pun_betting_club = c.jur_id
                            AND c.jur_parent_id = d.jur_id
                            AND d.jur_parent_id = ".$jurisdiction."
                UNION ALL
                SELECT 'n_credits', 0 n_credits,0,''  ";
                /*UNION ALL
                select 'r_credits', sum(r.jur_available_credit)
                from jurisdiction r
                WHERE r.jur_id = ".$jurisdiction."*/
                $sql.="  UNION ALL
                select 'd_credits', sum(d.jur_available_credit),0,(select cur_code_for_web from currency where cur_id=d.jur_currency)
                from jurisdiction d
                WHERE d.jur_parent_id = ".$jurisdiction."
                UNION ALL
                select 'c_credits', sum(c.jur_available_credit),0,(select cur_code_for_web from currency where cur_id=c.jur_currency) as cur_code_for_web
                from jurisdiction d, jurisdiction c
                WHERE d.jur_parent_id = ".$jurisdiction."
                AND c.jur_parent_id = d.jur_id";
        }
            break;


        case "district":{

            $sql="
                 select 'users_credit' jurisdiction, sum(pcr_credits) credit ,sum(pcr_bonus) bonus, (select cur_code_for_web from currency where cur_id=c.jur_currency) as cur_code_for_web
                FROM punter_credit, punter, jurisdiction c
                WHERE pun_id = pcr_pun_id
                AND pun_betting_club = c.jur_id
                AND c.jur_parent_id =  ".$jurisdiction."
                UNION ALL
                select 'affiliate_credit', coalesce(sum(act_credits), 0),0,(select cur_code_for_web from currency where cur_id=c.jur_currency) as cur_code_for_web
                FROM affiliate_credit, punter, jurisdiction c
                WHERE pun_aff_id = act_aff_id
                AND pun_aus_id <> 0
                AND pun_betting_club = c.jur_id
                AND c.jur_parent_id =  ".$jurisdiction."
                UNION ALL
                SELECT 'n_credits', 0 n_credits,0,''
                UNION ALL
                select 'r_credits', 0 r_credits,0,''  ";
                /*UNION ALL
                select 'd_credits', sum(d.jur_available_credit)
                from jurisdiction d
                WHERE d.jur_id =  ".$jurisdiction."*/
                $sql.=" UNION ALL
                select 'c_credits', sum(c.jur_available_credit),0,(select cur_code_for_web from currency where cur_id=c.jur_currency) as cur_code_for_web
                from jurisdiction c
                WHERE c.jur_parent_id =  ".$jurisdiction;

        }
        break;
        case "club":{
            $sql="
                        select 'users_credit' jurisdiction, sum(pcr_credits) credit ,sum(pcr_bonus) bonus,(select cur_code_for_web from currency where cur_id=c.jur_currency) as cur_code_for_web
                        FROM punter_credit, punter, jurisdiction c
                        WHERE pun_id = pcr_pun_id
                        AND pun_betting_club = c.jur_id
                        and c.jur_id=".$jurisdiction."
                        UNION ALL
                        select 'affiliate_credit', coalesce(sum(act_credits), 0),0,(select cur_code_for_web from currency where cur_id=c.jur_currency) as cur_code_for_web
                        FROM affiliate_credit, punter, jurisdiction c
                        WHERE pun_aff_id = act_aff_id
                        AND pun_aus_id <> 0
                        AND pun_betting_club = c.jur_id
                        AND c.jur_id=".$jurisdiction."
                        UNION ALL
                        SELECT 'n_credits', 0 n_credits,0,''
                        UNION ALL
                        select 'r_credits', 0 r_credits,0,''
                        UNION ALL
                        select 'd_credits', 0 d_credits,0,'' ";
                       /* UNION ALL
                        select 'c_credits', sum(c.jur_available_credit)
                        from jurisdiction c
                        WHERE c.jur_id = ".$jurisdiction;*/
        }
        break;
    }
    $result=$dbh->exec($sql);
    $entity=0;
    $users=0;
    $bonus=0;
    while($result->hasNext()){
        $row=$result->next();
        if($row['jurisdiction']=='users_credit'){
            $users=$row['credit'];
            $bonus=$row['bonus'];
        }
        else{
            $entity+=$row['credit'];
        }
        if($row['cur_code_for_web']!='') {
            $symbol = $row['cur_code_for_web'];
        }
    }

    if($class==$_SESSION['jurisdiction_class'] && $fclub==0){
        $return="<table class='display table'><thead>
                <tr><th>".$lang->getLang("Total Entity Credit")."</th><th>".$lang->getLang("Total Users Credit")."</th><th>".$lang->getLang("Total Bonus")."</th></tr></thead><tr>";
                $return.="<td>".getInDollars($entity)."</td>";
                $return.="<td>".getInDollars($users)."</td>";
                $return.="<td>".getInDollars($bonus)."</td></tr>";
    }
    else{
        $return= "<table style='border-spacing:0px'>
        <tr><th class=".$class." style='font-weight:ligther;font-size:11px' >".$lang->getLang("Total Entity Credit").":&nbsp; </th><td style='font-weight:ligther;font-size:11px'> ".getInDollars($entity)."</td></tr>
        <tr><th class=".$class." style='font-weight:ligther;font-size:11px'>".$lang->getLang("Total Users Credit").":&nbsp; </th><td style='font-weight:ligther;font-size:11px'> ".getInDollars($users)."</td></tr>
        <tr><th class=".$class." style='font-weight:ligther;font-size:11px' >".$lang->getLang("Total Bonus").":&nbsp; </th><td style='font-weight:ligther;font-size:11px'> ".getInDollars($bonus)."</td></tr>";
    }


    $return.="<table>";
    return $return;
}

?>
