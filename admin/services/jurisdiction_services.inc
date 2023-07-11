<?php
require_once $_SERVER['DOCUMENT_ROOT'] . '/backoffice/includes/funclib/general_functions.inc';
require_once $_SERVER['DOCUMENT_ROOT'] . '/backoffice/config/backoffice.conf.inc';

if(isset($_GET['action'])){
    switch ($_GET['action']){
        case "0":{
            die($_GET['callback']."(".json_encode(getAdminOfJurisdiction($_GET['admlk'])).")");
            break;
        }
        case "getJurisdictions": {
            $jur_id = (isset($_GET['id'])) ? substr($_GET['id'],1) : $_SESSION['jurisdiction_id'];
            $includeParent = $_GET['parent'];
            $skin = (isset($_GET['skinid'])) ? $_GET['skinid'] : false;
            $status = $_GET['status'];
            $_SESSION['jurisdiction_search_status'] = $status;
            die(getListOfChildJurisdictions($jur_id, $includeParent, $skin, $status));
            break;
        }

        case 'getJurisdictionParents':{
           die(getJurisdictionPathById($_GET['str']));
            break;
        }
        case 'getJurisdictionsByName':{
           die(getJurisdictionsByName($_GET['str']));
            break;
        }

        default:{
            die('Invalid operation');
            break;
        }
    }
}
switch ($_POST['action'])
{
    case "0":
    {
        refreshJurisdictionCredit();
        break;
    }
    case "1":
        $jur_id = isset($_POST['jur_id']) ? $_POST['jur_id'] : -1;
        if ($jur_id == 'none') {
            return;
        }
        $parent = $_POST['parent'];
        getPercentageOf($parent, $jur_id);
        break;
    case "2":
        $jur_id = $_POST['jur_id'];
        $class = $_POST["jur_class"];
        $percType = $_POST['perc_type'];
        getMinMaxPercentage($jur_id, $class, $percType);
        break;
    case "3":
        $jur_id = $_POST['jur_id'];
        if ($jur_id == 'none') {
            return;
        }
        $node = isset($_POST["node"]) ? $_POST["node"] : -1;
        getPercentageByClass($jur_id , $node);
        break;

    case "4":
        $adminlike=$_GET['admlk'];
        die(var_dump($_GET['admlk']));
        break;
    case "5":
        die(getJurisdictionInfo($_POST['jur_id']));
        break;

    case "6":
        $jur_id = isset($_POST['jur_id']) ? $_POST['jur_id'] : -1;
        if ($jur_id == 'none') {
            return;
        }
        $parent = $_POST['parent'];
        getLivePercentageOf($parent, $jur_id);
        break;
    case "7":
        $jur_id = isset($_POST['jur_id']) ? $_POST['jur_id'] : -1;
        die(getSkinByJurisdiction($jur_id));
        break;
    case "8":
        $jurisdictionType = $_POST['jurisdiction'];
        $resp = getDisabledJurisdictionsByType($jurisdictionType);
        session_start();
        $_SESSION['lastMenuResp'] = $resp;
        //var_dump($_SERVER['lastMenuResp']);
        die($resp);
        break;
    case "9":
        $searchKeyword = $_POST['searchKeyword'];
        $resp = getDisabledJurisdictionsByKeyword($searchKeyword);
        session_start();
        $_SESSION['lastMenuResp'] = $resp;
        die($resp);
        break;
    case "10":
        $resp = getAllDisabledJurisdictions();
        session_start();
        $_SESSION['lastMenuResp'] = $resp;
        die($resp);
        break;
    case "11":
        session_start();
        if (isset($_SESSION['lastMenuResp'])) {
            $resp = $_SESSION['lastMenuResp'];
        } else {
            $resp = "";
        }
        die($resp);
        break;
    default:{
        die('Invalid operation');
        break;
    }


}


function getAllDisabledJurisdictions() {
    global $dbh;
    $sql = "SELECT  JUR_CLASS, JUR_NAME, JUR_ID FROM jurisdiction where JUR_STATUS = '0' order by jur_class";
    $result = $dbh->doCachedQuery($sql, 0);
    $jurNameList = array();
    while($result->hasNext()) {
        $row = $result->next();
        $section = array();
        $section[0] = $row['jur_class'];
        $section[1] = $row['jur_name'];
        $section[2] = $row['jur_id'];
        array_push($jurNameList, $section);
    }
    return json_encode($jurNameList);
}


function getDisabledJurisdictionsByKeyword($searchKeyword) {
    global $dbh;
    $sql = "SELECT JUR_CLASS, JUR_NAME, JUR_ID  FROM jurisdiction where JUR_STATUS = '0' and JUR_NAME like '%$searchKeyword%' order by JUR_CLASS";
    $result = $dbh->doCachedQuery($sql, 0);

    $jurList = array();
    while($result->hasNext()){
        $row = $result->next();
        $section = array();
        $section[0] = $row['jur_class'];
        $section[1] = $row['jur_name'];
        $section[2] = $row['jur_id'];
        array_push($jurList, $section);
    }
    return json_encode($jurList);

}


function getDisabledJurisdictionsByType($jurisdictionType) {
    global $dbh;
    $sql = "SELECT JUR_NAME, JUR_ID, JUR_CLASS FROM jurisdiction where JUR_CLASS = '$jurisdictionType' AND JUR_STATUS = '0'";
    $result = $dbh->doCachedQuery($sql,0);
    //var_dump($result);
    $jurNameList = array();
    while($result->hasNext()){
        $row = $result->next();
        $section = array();
        $section[0] = $row['jur_class'];
        $section[1] = $row['jur_name'];
        $section[2] = $row['jur_id'];
        array_push($jurNameList, $section);
    }
    return json_encode($jurNameList);
}


function refreshJurisdictionCredit(){
    global $dbh;
    $sql  = "SELECT jur_available_credit,jur_overdraft,jur_reserved_fund,jur_tot_overdraft_received from jurisdiction WHERE jur_id= ".db_quote($_SESSION['jurisdiction_id']);
    $rs   = $dbh->queryRow($sql);
    $_SESSION['jurisdiction_pending']=$rs['jur_reserved_fund'];
    $_SESSION['jurisdiction_overdraft']=$rs['jur_overdraft'];
    $_SESSION['jurisdiction_credit']=$rs['jur_available_credit'];
    $_SESSION['jurisdiction_overdraft_received']=$rs['jur_tot_overdraft_received'];
    die(getInDollars($rs['jur_available_credit'])."~".getInDollars($rs['jur_reserved_fund'])."~".getInDollars($rs['jur_overdraft'])."~".getInDollars($rs['jur_tot_overdraft_received']));

}


function getPercentageByClass($parentNode = -1, $node = -1){
    global $dbh;
    if($node != -1){
        $sql = "SELECT jur_parent_id from jurisdiction where jur_id = $node";
        $jur_parent_id = $dbh->queryOne($sql);
        if($jur_parent_id == $parentNode){
            $sql = "Select * from jurisdiction_payment_settings where jps_jur_id = $node";
            $percentuals = $dbh->queryRow($sql);
            if($percentuals){
                $message = "0~";
                foreach($percentuals as $key => $value){
                    if((strpos($key, "perc") > 0) && !(strpos($key, "betting"))){
                        $message .= "$key:$value;";
                    }
                }
                die(substr($message,0,strlen($message)-1));
            }
        }
    }
    if($parentNode != -1){
        $sql = "Select * from jurisdiction_payment_settings where jps_jur_id = $parentNode";
        $result = $dbh->queryRow($sql);
        if($result){
            $message = "1~";
            foreach($result as $key => $value){
                if((strpos($key, "perc") > 0) && !(strpos($key, "betting"))){
                    $message .= "$key:$value;";
                }
            }
            die(substr($message,0,strlen($message)-1));
        }else{
            die("-1");
        }
    }
}
function getPercentageOf($parent, $jur_id, $jur_class = ""){
    global $dbh;
    $sql="SELECT jps_perc_betting from jurisdiction_payment_settings where jps_jur_id = $jur_id";
    $parentResult = $dbh->queryRow($sql);
    $perc=explode(";",$parentResult['jps_perc_betting']);
    $parentType=explode(":",$perc[0]);
    die($parentType[1]);
}

function getMinMaxPercentage($jur_id, $jur_class, $perctype){
    global $dbh, $lang;
    $sql = "SELECT ";
    if($jur_class == "nation"){
        $sql .= " j2.jur_id level_id,
 	              jps2.$perctype level_perc,
 	              j3.jur_id below_id,
	              max(jps3.$perctype) below_perc ";
    }
    if($jur_class == "region"){
        $sql .= " j1.jur_id above_id,
                    jps1.$perctype above_perc,
                    j2.jur_id level_id,
 	                jps2.$perctype level_perc,
	                j3.jur_id below_id,
	                max(jps3.$perctype) below_perc ";
    }
    if($jur_class == "district"){
        $sql .= " j1.jur_id above_id,
                    jps1.$perctype above_perc,
                    j2.jur_id level_id,
 	                jps2.$perctype level_perc ";
    }
    if($jur_class == "club"){
        $sql .= "  j1.jur_id above_id,
                    jps1.$perctype above_perc,
                    j2.jur_id level_id,
                    jps2.$perctype level_perc ";
    }
    $sql .= "FROM jurisdiction j2
             LEFT JOIN jurisdiction j1 ON j2.jur_parent_id = j1.jur_id
             LEFT JOIN jurisdiction j3 ON j2.jur_id = j3.jur_parent_id
             LEFT JOIN jurisdiction_payment_settings jps1 ON j1.jur_id = jps1.jps_jur_id
             LEFT JOIN jurisdiction_payment_settings jps2 ON j2.jur_id = jps2.jps_jur_id
             LEFT JOIN jurisdiction_payment_settings jps3 ON j3.jur_id = jps3.jps_jur_id
             WHERE j2.jur_id =$jur_id";
    //die($sql);
    $result = $dbh->queryRow($sql);
    $above = isset($result["above_perc"]) ? $result["above_perc"] : "100";
    $level = $result["level_perc"];
    $below = isset($result["below_perc"]) ? $result["below_perc"] : "0";
    if($perctype == "JPS_PERC_BETTING" || $perctype=="JPS_PERC_LIVE_BETTING"){
        if(strpos($above, "percs") > 0){
            $percs = explode(";",$above);
            $above = explode(":",$percs[1]);
            $above = $above[1];
        }
        $percs = explode(";",$level);
        $level = explode(":",$percs[1]);
        $level = $level[1];
        if(strpos($below, "percs") > 0){
            $percs = explode(";",$below);
            $below = explode(":",$percs[1]);
            $below = $below[1];
        }
    }
    $message = $lang->getLang("You can set only a percentage between % and %", $below , $above);
    die($above.";".$level.";".$below.";".$message);
}

function getAdminOfJurisdiction($admlike){
    global $dbh;
    $jurclass=$_SESSION['jurisdiction_class'];
    switch ($jurclass)
    {
        case 'casino':
           $sql="
                select auca.aus_username, ca.jur_name
                from jurisdiction ca, admin_user auca
                where auca.aus_username LIKE '%".$dbh->escape($admlike)."%'
                and auca.aus_jurisdiction_id = ca.jur_id
                and ca.jur_id = 1

                UNION ALL
                select aun.aus_username, n.jur_name
                from jurisdiction n, admin_user aun
                where aun.aus_username LIKE '%".$dbh->escape($admlike)."%'
                and aun.aus_jurisdiction_id = n.jur_id
                and n.jur_parent_id = 1

                UNION ALL
                select aur.aus_username, r.jur_name
                from jurisdiction n, jurisdiction r, admin_user aur
                where aur.aus_username LIKE '%".$dbh->escape($admlike)."%'
                and n.jur_parent_id = 1
                and r.jur_parent_id = n.jur_id
                and aur.aus_jurisdiction_id = r.jur_id

                UNION ALL
                select aud.aus_username, d.jur_name
                from jurisdiction n, jurisdiction r, jurisdiction d, admin_user aud
                where aud.aus_username LIKE '%".$dbh->escape($admlike)."%'
                and n.jur_parent_id = 1
                and r.jur_parent_id = n.jur_id
                and d.jur_parent_id = r.jur_id
                and aud.aus_jurisdiction_id = d.jur_id

                UNION ALL
                select auc.aus_username, c.jur_name
                from jurisdiction n, jurisdiction r, jurisdiction d, jurisdiction c, admin_user auc
                where auc.aus_username LIKE '%".$dbh->escape($admlike)."%'
                and n.jur_parent_id = 1
                and r.jur_parent_id = n.jur_id
                and d.jur_parent_id = r.jur_id
                and c.jur_parent_id = d.jur_id
                and auc.aus_jurisdiction_id = c.jur_id";
            break;

        case 'nation':
            $sql="
                select aun.aus_username, n.jur_name
                from jurisdiction n, admin_user aun
                where aun.aus_username LIKE '%".$dbh->escape($admlike)."%'
                and aun.aus_jurisdiction_id = n.jur_id
                and n.jur_id =".$_SESSION['jurisdiction_id']."

                UNION ALL
                select aur.aus_username, r.jur_name
                from jurisdiction n, jurisdiction r, admin_user aur
                where aur.aus_username LIKE '%".$dbh->escape($admlike)."%'
                and n.jur_id = ".$_SESSION['jurisdiction_id']."
                and r.jur_parent_id = n.jur_id
                and aur.aus_jurisdiction_id = r.jur_id

                UNION ALL
                select aud.aus_username, d.jur_name
                from jurisdiction n, jurisdiction r, jurisdiction d, admin_user aud
                where aud.aus_username LIKE '%".$dbh->escape($admlike)."%'
                and n.jur_id = ".$_SESSION['jurisdiction_id']."
                and r.jur_parent_id = n.jur_id
                and d.jur_parent_id = r.jur_id
                and aud.aus_jurisdiction_id = d.jur_id

                UNION ALL
                select auc.aus_username, c.jur_name
                from jurisdiction n, jurisdiction r, jurisdiction d, jurisdiction c, admin_user auc
                where auc.aus_username LIKE '%".$dbh->escape($admlike)."%'
                and n.jur_id = ".$_SESSION['jurisdiction_id']."
                and r.jur_parent_id = n.jur_id
                and d.jur_parent_id = r.jur_id
                and c.jur_parent_id = d.jur_id
                and auc.aus_jurisdiction_id = c.jur_id";
            break;
        case 'region':
            $sql="
                select aur.aus_username, r.jur_name
                from  jurisdiction r, admin_user aur
                where aur.aus_username LIKE '%".$dbh->escape($admlike)."%'
                and r.jur_id = ".$_SESSION['jurisdiction_id']."
                and aur.aus_jurisdiction_id = r.jur_id

                UNION ALL
                select aud.aus_username, d.jur_name
                from jurisdiction r, jurisdiction d, admin_user aud
                where aud.aus_username LIKE '%".$dbh->escape($admlike)."%'
                and r.jur_id = ".$_SESSION['jurisdiction_id']."
                and d.jur_parent_id = r.jur_id
                and aud.aus_jurisdiction_id = d.jur_id

                UNION ALL
                select auc.aus_username, c.jur_name
                from jurisdiction r, jurisdiction d, jurisdiction c, admin_user auc
                where auc.aus_username LIKE '%".$dbh->escape($admlike)."%'
                and r.jur_id = ".$_SESSION['jurisdiction_id']."
                and d.jur_parent_id = r.jur_id
                and c.jur_parent_id = d.jur_id
                and auc.aus_jurisdiction_id = c.jur_id";
            break;
        case 'district':
            $sql="
                select aud.aus_username, d.jur_name
                from  jurisdiction d, admin_user aud
                where aud.aus_username LIKE '%".$dbh->escape($admlike)."%'
                and d.jur_id = ".$_SESSION['jurisdiction_id']."
                and aud.aus_jurisdiction_id = d.jur_id

                UNION ALL
                select auc.aus_username, c.jur_name
                from  jurisdiction d, jurisdiction c, admin_user auc
                where auc.aus_username LIKE '%".$dbh->escape($admlike)."%'
                and d.jur_id = ".$_SESSION['jurisdiction_id']."
                and c.jur_parent_id = d.jur_id
                and auc.aus_jurisdiction_id = c.jur_id";
            break;

        case 'club':
            $sql="
                select auc.aus_username, c.jur_name
                from  jurisdiction c, admin_user auc
                where auc.aus_username LIKE '%".$dbh->escape($admlike)."%'
                and c.jur_id = ".$_SESSION['jurisdiction_id']."
                and auc.aus_jurisdiction_id = c.jur_id";
    }
    $result=$dbh->doCachedQuery($sql,0);
    $arrResults=array();

    while($result->hasNext()){
        $row=$result->next();
        array_push($arrResults,array("user"=>$row['aus_username'],"jurisdiction"=>$row['jur_name']));
    }
    return $arrResults;
    /*
     * SELECT  aus_username,
     *         jur_name
     * FROM admin_user LEFT OUTER JOIN jurisdiction j1 ON aus_jurisdiction_id = j1.jur_id WHERE ( aus_jurisdiction_id IN ( SELECT j2.jur_id FROM jurisdiction j2 WHERE j2.jur_parent_id = 1) OR aus_jurisdiction_id IN ( SELECT j2.jur_id FROM jurisdiction j2 WHERE j2.jur_parent_id IN ( SELECT j3.jur_id FROM jurisdiction j3 WHERE j3.jur_parent_id = 1) ) OR aus_jurisdiction_id IN ( SELECT j2.jur_id FROM jurisdiction j2 WHERE j2.jur_parent_id IN ( SELECT j3.jur_id FROM jurisdiction j3 WHERE j3.jur_parent_id IN ( SELECT j4.jur_id FROM jurisdiction j4 WHERE j4.jur_parent_id = 1 ) ) ) OR aus_jurisdiction_id IN ( SELECT j2.jur_id FROM jurisdiction j2 WHERE j2.jur_parent_id IN ( SELECT j3.jur_id FROM jurisdiction j3 WHERE j3.jur_parent_id IN ( SELECT j4.jur_id FROM jurisdiction j4 WHERE j4.jur_parent_id IN ( SELECT j5.jur_id FROM jurisdiction j5 WHERE j5.jur_parent_id = 1 ) ) ) ) OR ( aus_jurisdiction_id = 1 ) )   AND (aus_username LIKE '%asd%' )  ORDER BY LOWER(TRIM(jur_name)), LOWER(TRIM(aus_username))
     */
}


/**
 * @param $id
 * @return array|mixed|string
 */
function getJurisdictionInfo($id)
{
    global $dbh,$lang;
    $sql = "SELECT jur_id, jur_name, jur_available_credit,jur_overdraft,jur_overdraft_start_time,jur_tot_overdraft_received,cur_code_for_web FROM jurisdiction join currency on jur_currency=cur_id WHERE jur_id =$dbh->escapeQuotesAndSlashes($id) ";
     $rs = $dbh->doCachedQuery($sql,0);
        $num_rows = $rs->getNumRows();
    if ( $num_rows > 0){
        $summary='';
            while($rs->hasNext()){
                $row  = $rs->next();
                $id   = $row['jur_id'];
                $name = $row['jur_name'];
                $available_credit = getInDollars($row['jur_available_credit'],2,$row['cur_code_for_web']);
                $selected = ($_POST['jurisdiction'] == $id) ? 'selected="selected"' :  '';
                $summary.="<table id='".$id."Info'  style='padding-left: 200px;'>
                            <tr><td class='label'>".$lang->getLang("Credit")."</td><td>".$available_credit."</td></tr>
                             <tr><td class='label'>".$lang->getLang("Overdraft")."</td><td>".getInDollars($row['jur_overdraft'],2,$row['cur_code_for_web'])."</td></tr>
                            <tr><td class='label'>".$lang->getLang("Total Overdraft Received")."</td><td>".getInDollars($row['jur_tot_overdraft_received'],2,$row['cur_code_for_web'])."</td></tr>
                            <tr><td class='label'>".$lang->getLang("Overdraft start time")."</td><td>";
                $summary.=($row['jur_overdraft_start_time']=='') ? 'none': $row['jur_overdraft_start_time'];
                $summary.=" </td></tr></table>";
            }
      return $summary;
    }
    return $lang->getLang("There is no info about that jurisdiction");
}


/**
 * @param $parent
 * @param $jur_id
 * @param string $jur_class
 */
function getLivePercentageOf($parent, $jur_id, $jur_class = ""){
    global $dbh;
    $sql="SELECT jps_perc_live_betting from jurisdiction_payment_settings where jps_jur_id = $jur_id";
    $parentResult = $dbh->queryRow($sql);
    die($parentResult);
}





/*function getListOfChildJurisdictions($id=false){
    global $dbh,$lang;
    if(!$id){
        $id=$_SESSION['jurisdiction_id'];
    }
    $sql="Select jur_id,jur_class,jur_name,jur_creation_date,jur_status, (select count(*) from jurisdiction j1 where j1.jur_parent_id=jurisdiction.jur_id) as children from jurisdiction where jur_parent_id=".$dbh->escape($id);
    $result=$dbh->doCachedQuery($sql,0);

    $return="<ul>";
    while($result->hasNext()){
        $row=$result->next();
        $blocked='';
        if($row['jur_status']!='1'){
            $blocked=' <span class="tip" style="color:red">('.$lang->getLang("blocked").')</span>';
        }
        if($row['children']>0){
            $return .= "<li class='jstree-closed " . $row['jur_class'] . "'  data-jstree='{ \"type\" : \"" . $row['jur_class'] . "\" }'  id='" . $row['jur_id'] . "'>" . $row['jur_name'] . " <span class='tip'>(".$row['children']." ".getSubjurisdictionClass($row['jur_class']).($row['children']>1?'s':'').")</span>".$blocked;
            $return.="<a href='/jurisdictions/index.php?node=".$row['jur_id']."'>".$row['jur_name']."</a>";
            $return.="</li>";
        }
        else {
            $return .= "<li class='" . $row['jur_class'] . "'  data-jstree='{ \"type\" : \"" . $row['jur_class'] . "\" }'  id='" . $row['jur_id'] . "'>" . $row['jur_name'] . $blocked;
            $return.="<a href='/jurisdictions/index.php?node=".$row['jur_id']."'>".$row['jur_name']."</a>";
            $return.="</li>";
        }

    }
    $return.="</ul>";
    return $return;
}*/


function getListOfChildJurisdictions($id=false, $includeParent=false,$skinid=false, $status=false)
{
    define("NEW_ELAPSED_SECONDS", 60 * 60 * 48);
    global $dbh, $lang;
    if (!$id) {
        $id = $_SESSION['jurisdiction_id'];
    }
    $sql = "Select jur_id,jur_class,jur_name,jur_creation_date,jur_status,
           (select count(*) from jurisdiction j1 where j1.jur_parent_id=jurisdiction.jur_id) as children
           from jurisdiction where jur_parent_id=" . $dbh->escape($id);
    if($skinid){
        $sql.=" AND jur_skn_id=".$dbh->escape($skinid);
    }
    if($status == '0'){
        $sql .=  " AND jur_status = 0";
    }
    else if ($status == '1') {
        $sql .=  " AND jur_status = 1";
    }

    $result = $dbh->doCachedQuery($sql, 0);


    $return = array();
    $now = time();
    while ($result->hasNext()) {
        $row = $result->next();
        $section = array();
        $blocked = '';
        if ($row['jur_status'] != '1') {
            $blocked = ' <span class="tip" style="color:red">(' . $lang->getLang("blocked") . ')</span>';
        }
        if ($row['children'] > 0) {
            $section['children'] = true;
            $section['li_attr']=array('class'=>$row['jur_class'],  'data-jstree'=>'{ "type" : "' . $row['jur_class'] . '" }');
            $section['id'] = "n".$row['jur_id'];
            $section['icon'] ='/media/images/'.substr($row['jur_class'],0,1).'.png';
            $section['a_attr'] = array("href"=> '/jurisdictions/index.php?node=' . $row['jur_id'].'&class='.$row['jur_class'] );
            $section['text'] =   $row['jur_name'] . " <span class='tip'>(" . $row['children'] . " " . getSubjurisdictionClass($row['jur_class']) . ($row['children'] > 1 ? 's' : '') . ")</span>" . $blocked;
            if($now - strtotime($row["jur_creation_date"]) <= NEW_ELAPSED_SECONDS){
                $section['text'].= "<img src='".image_dir . "/icon_new.gif'/>";

                if(function_exists("getReadableDate")){
                    $section['text'].=  getReadableDate(strtotime($row["jur_creation_date"]));
                }
            }

        } else {

            $section['li_attr']=array('class'=>$row['jur_class'],  'data-jstree'=>'{ "type" : "' . $row['jur_class'] . '" }');
                      $section['id'] = "n".$row['jur_id'];
            $section['icon'] ='/media/images/'.substr($row['jur_class'],0,1).'.png';
            $section['a_attr'] = array("href"=> '/jurisdictions/index.php?node=' . $row['jur_id'].'&class='.$row['jur_class'] );
            $section['text'] .=  $row['jur_name'] . $blocked;
            if($now - strtotime($row["jur_creation_date"]) <= NEW_ELAPSED_SECONDS){
                $section['text'].= "<img src='".image_dir . "/icon_new.gif'/>";

                if(function_exists("getReadableDate")){
                    $section['text'].=  getReadableDate(strtotime($row["jur_creation_date"]));
                }
            }


        }

            array_push($return, $section);

    }

    if($includeParent){
        $sql="Select jur_id,jur_class,jur_name,jur_creation_date,jur_status,
           (select count(*) from jurisdiction j1 where j1.jur_parent_id=jurisdiction.jur_id) as children
           from jurisdiction where jur_id=" . $dbh->escape($id);

        $row=$dbh->queryRow($sql);
        $section['li_attr']=array('class'=>$row['jur_class'],  'data-jstree'=>'{ "type" : "' . $row['jur_class'] . '" }');
        $section['id'] = "n".$row['jur_id'];
        $section['icon'] ='/media/images/'.substr($row['jur_class'],0,1).'.png';
        $section['a_attr'] = array("href"=> '/jurisdictions/index.php?node=' . $row['jur_id'].'&class='.$row['jur_class'] );
        $section['text'] =  $row['jur_name'] ;
        $section['state']=array("opened"=>true);
        if($now - strtotime($row["jur_creation_date"]) <= NEW_ELAPSED_SECONDS){
            $section['text'].= "<img src='".image_dir . "/icon_new.gif'/>";

            if(function_exists("getReadableDate")){
                $section['text'].=  getReadableDate(strtotime($row["jur_creation_date"]));
            }
        }
        $section['children']=$return;
        return json_encode($section);

    }

    return json_encode($return);
}

function getJurisdictionPath($jur_name){
    global $dbh;
    $sql="Select n.jur_id nation_id,n.jur_name nation_name ,
                 r.jur_id region_id,r.jur_name region_name,
                 d.jur_id district_id,d.jur_name district_name,
                 c.jur_name club_name
                 from  jurisdiction n, jurisdiction r,jurisdiction d,jurisdiction c
     where
     c.jur_parent_id=d.jur_id
     AND d.jur_parent_id=r.jur_id
     AND r.jur_parent_id=n.jur_id
     AND n.jur_parent_id=1
     AND (
     c.jur_name like '%".$dbh->escape($jur_name)."%'
     OR r.jur_name like '%".$dbh->escape($jur_name)."%'
     OR d.jur_name like '%".$dbh->escape($jur_name)."%')
     ";
    
    $jurisdicitons=  $dbh->doCachedQuery($sql, 0);
    $return=array();
    while ($jurisdicitons->hasNext()) {
        $row = $jurisdicitons->next();
        if (stripos($row['club_name'], $jur_name) !== false) {
            array_push($return, "n" . $row['district_id']);
            array_push($return, "n" . $row['region_id']);
            array_push($return, "n" . $row['nation_id']);
        }
        elseif(stripos($row['district_name'], $jur_name) !== false) {
            array_push($return, "n" . $row['region_id']);
            array_push($return, "n" . $row['nation_id']);
        }
        elseif(stripos($row['region_name'], $jur_name) !== false) {
            array_push($return, "n" . $row['nation_id']);
        }
    }
    return json_encode($return);
}

function getJurisdictionPathById($id){
    global $dbh;
    $sql="Select n.jur_id nation_id,
                 r.jur_id region_id,
                 d.jur_id district_id,
                 c.jur_id club_id
                 from  jurisdiction n, jurisdiction r,jurisdiction d,jurisdiction c
     where
     c.jur_parent_id=d.jur_id
     AND d.jur_parent_id=r.jur_id
     AND r.jur_parent_id=n.jur_id
     AND n.jur_parent_id=1
     AND (
     c.jur_id =".$dbh->escape($id)."
     OR  r.jur_id =".$dbh->escape($id)."
     OR  d.jur_id =".$dbh->escape($id)." )
     ";
    $jurisdicitons=  $dbh->doCachedQuery($sql, 0);
    $return=array();
    while ($jurisdicitons->hasNext()) {
        $row = $jurisdicitons->next();
        if (stripos($row['club_id'], $id) !== false) {
            array_push($return, "n" . $row['district_id']);
            array_push($return, "n" . $row['region_id']);
            array_push($return, "n" . $row['nation_id']);
        }
        elseif(stripos($row['district_id'], $id) !== false) {
            array_push($return, "n" . $row['region_id']);
            array_push($return, "n" . $row['nation_id']);
        }
        elseif(stripos($row['region_id'], $id) !== false) {
            array_push($return, "n" . $row['nation_id']);
        }
    }
    return json_encode($return);
}

function getJurisdictionsByName($jur_name)
{
    
    global $dbh;
    $sql = "Select jur_id, jur_name,jur_class
                 from  jurisdiction
     where jur_name like '%" . $dbh->escape($jur_name) . "%'
     ";

    $jurisdicitons = $dbh->doCachedQuery($sql, 0);
    $return = array();
    while ($jurisdicitons->hasNext()) {
        $row = $jurisdicitons->next();
        if(!is_array($return[$row['jur_class']])){
            $return[$row['jur_class']]=array();
        }
        array_push($return[$row['jur_class']], array("id"=>$row['jur_id'],"name" => $row['jur_name']));
    }
    return json_encode($return);
}
?>