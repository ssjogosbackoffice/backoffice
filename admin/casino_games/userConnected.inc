<?php
check_access('lotto_manage', true);
if(isset($_GET['listDetails'])) {

    $columns = array('username','jur_name','partner_name', 'usl_pun_sess_id','usl_pun_member_type', 'usl_jur_currency', 'usl_session_validity', 'usl_session_last_use', 'usl_session_start', 'diff');
    $columnNumber =$_GET['order'][0]['column'];
    $ascOrDesc = $_GET['order'][0]['dir'];
    $colName = $columns[$columnNumber];
    $orderBy = " ORDER BY " . $colName . " " .$ascOrDesc;

    //Create limit part of the sql querry
    $start = $_GET['start'];
    $length = $_GET['length'];
    $limitQuerry = "LIMIT $start , $length";

    //Create search part of sql querry
    if($_GET['search']['value'] != "") {
        $searchableColumn1 = "usl_pun_username";
        $searchableColumn2 = "usl_jur_name";
        $searchableColumn3 = "usl_pun_member_type";
        $searchKeyword = $dbh->prepareString($_GET['search']['value']);
        $searchKeyword = trim($searchKeyword, "'");
        $sqlLikeStmnt = "and ($searchableColumn1 LIKE '%$searchKeyword%' or $searchableColumn2 LIKE '%$searchKeyword%' or $searchableColumn3 LIKE '%$searchKeyword%' )";
        $sqlLikeStmntTotals = " WHERE ($searchableColumn1 LIKE '%$searchKeyword%' or $searchableColumn2 LIKE '%$searchKeyword%' )";

        $sql = "SELECT usl_pun_username username, COALESCE((select ptn_name from partners where ptn_id = usl_ptn_id and ptn_id > 0), 'NONE') partner_name,
            usl_pun_sess_id, usl_pun_member_type, c.jur_name,COALESCE((select cur_name from currency where cur_id = usl_jur_currency), 'NONE') usl_jur_currency,
             usl_session_validity, usl_session_last_use, usl_session_start, ABS(timestampdiff(MINUTE, usl_session_last_use, usl_session_start)) diff
            FROM system_cache.user_logged, jurisdiction n, jurisdiction r, jurisdiction d, jurisdiction c
            WHERE n.jur_parent_id = 1 AND r.jur_parent_id = n.jur_id AND d.jur_parent_id = r.jur_id AND c.jur_parent_id = d.jur_id AND c.jur_id = usl_pun_betting_club
            $sqlLikeStmnt $orderBy $limitQuerry;";
        $totalsSQL = "SELECT count(*) FROM system_cache.user_logged $sqlLikeStmntTotals";
    }
    else {
        $sql = "SELECT usl_pun_username username, COALESCE((select ptn_name from partners where ptn_id = usl_ptn_id and ptn_id > 0), 'NONE') partner_name,
            usl_pun_sess_id, usl_pun_member_type, c.jur_name,COALESCE((select cur_name from currency where cur_id = usl_jur_currency), 'NONE') usl_jur_currency,
             usl_session_validity, usl_session_last_use, usl_session_start, ABS(timestampdiff(MINUTE, usl_session_last_use, usl_session_start)) diff
            FROM system_cache.user_logged, jurisdiction n, jurisdiction r, jurisdiction d, jurisdiction c
            WHERE n.jur_parent_id = 1 AND r.jur_parent_id = n.jur_id AND d.jur_parent_id = r.jur_id AND c.jur_parent_id = d.jur_id AND c.jur_id = usl_pun_betting_club 
            $orderBy $limitQuerry;";
        $totalsSQL = "SELECT count( * ) FROM system_cache.user_logged";
    }

    $response =  $dbh->doCachedQuery($sql, 0);
    $totals=$dbh->queryOne($totalsSQL);

    $tableArr= [];
    while ($response->hasNext()){
        $row = $response->next();
        $section = array();
        $section[0] = $row['username'];
        $section[1] = $row['jur_name'];
        if($row['partner_name'] === "NONE") {
            $section[2] = "-";
        }
        else{
            $section[2] = $row['partner_name'];
        }
        $section[3] = $row['usl_pun_sess_id'];
        $section[4] = $row['usl_pun_member_type'];
        $section[5] = $row['usl_jur_currency'];
        $section[6] = $row['usl_session_validity'] / 1000 / 60;
        $section[7] = $row['usl_session_last_use'];
        $section[8] = $row['usl_session_start'];
        $section[9] = $row['diff'];
        array_push($tableArr,$section);
    }

    $output= array(
        "draw"            => intval( $_GET['draw'] ),
        "recordsTotal"    => intval($totals),
        "recordsFiltered" => intval($totals),
        "data"            => $tableArr
    );

    die(json_encode($output));
}
?>


<table class="table table-bordered" style="width: calc(100vw - 200px);">
    <tr>
        <td class="navbar-inner" colspan="2"  ><h3 class="text-center"><?=$lang->getLang("User Status")?></h3></td>
    </tr>
    <tr>
        <td>
            <h5 class="text-center"><?=$lang->getLang("Totals")?></h5>
            <table id="userCounter" class='display table table-striped table-hover table-bordered table-condensed dataTable no-footer' style="width: 100%">
                <thead>
                    <tr>
                        <th><?=$lang->getLang("Counter")?></th>
                        <th><?=$lang->getLang("Member type")?></th>
                    </tr>
                </thead>
                <?php     $sql = "SELECT count( * ) counter, usl_pun_member_type FROM system_cache.user_logged group by usl_pun_member_type";
                          $result = $dbh->doCachedQuery($sql, 0);
                          while($result->hasNext()) {
                              $row = $result->next();
                              $totalUsers = $row['counter'];
                              $userType = $row ['usl_pun_member_type']; ?>
                              <tr>
                                  <td>
                                      <?=$lang->getLang("$totalUsers")?>
                                  </td>
                                  <td>
                                      <?=$lang->getLang("$userType")?>
                                  </td>
                              </tr>

                              <?php
                          }
                ?>
            </table>
        </td>
    </tr>
    <tr>
        <td>
            <h5 class="text-center"><?=$lang->getLang("Details")?></h5>
            <table id="userDetails" class='display table table-striped table-hover table-bordered table-condensed dataTable no-footer' style="width: 100%">
                <thead>
                <tr>
                    <th><?=$lang->getLang("User Name")?></th>
                    <th><?=$lang->getLang("Club")?></th>
                    <th><?=$lang->getLang("Partner name")?></th>
                    <th><?=$lang->getLang("User session Id")?></th>
                    <th><?=$lang->getLang("User member type")?></th>
                    <th><?=$lang->getLang("Currency")?></th>
                    <th><?=$lang->getLang("Session Validity (mins)")?></th>
                    <th><?=$lang->getLang("Last use")?></th>
                    <th><?=$lang->getLang("Start time")?></th>
                    <th><?=$lang->getLang("Minutes elapsed")?></th>
                </tr>
                </thead>
            </table>
        </td>
    </tr>
</table>

