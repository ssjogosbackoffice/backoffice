<?php
require_once 'Lotto.class.inc';

if(isset($_REQUEST['pk'])){
    global $lang;
    $code=$_POST['code'];
    if(is_array($_POST['code'])){
        $code=$_POST['code'][0];
    }
    $lotto=new Lotto(escapeSingleQuotes($code));
    switch($_REQUEST['pk'])
    {
        case "updateDays":
            try{
                if(!$_POST['value']){
                    throw new Exception($lang->getLang('Invalid Days'));
                }
                else{
                    $newDays=implode(',',$_POST['value']);
                    $return=$lotto->setExtractionDays($newDays);
                }
            }
            catch(Exception $e){
                error_log($e->getMessage());
                $return=-1;
            }
            die($return);
            break;
        case "updateType":
            try{
                if(!$_POST['value']){
                    throw new Exception($lang->getLang('Invalid Type'));
                }
                else{
                    $type=$_POST['value'];
                    $return=$lotto->setType($type);
                }
            }
            catch(Exception $e){
                error_log($e->getMessage());
                $return=-1;
            }
            die($return);
            break;
        case "updateStatus":
            try{
                if($_POST['value']==''){
                    throw new Exception($lang->getLang('Invalid Status'));
                }
                else{
                    $status=$_POST['value'];
                    $return=$lotto->setStatus($status);
                }
            }
            catch(Exception $e){
                error_log($e->getMessage());
                $return="-1";
            }
            die($return);
            break;
        case "updateCountries":
            try{
                if(!$_POST['value']){
                    throw new Exception($lang->getLang('Invalid country code'));
                }
                else{
                    $country=$_POST['value'];
                    $return=$lotto->setCountry($country);
                }
            }
            catch(Exception $e){
                error_log($e->getMessage());
                $return=-1;
            }
            die($return);
            break;
        case "updateSiteUrl":
            try{
                if(!$_POST['value']){
                    throw new Exception($lang->getLang('Invalid site url'));
                }
                else{
                    $url=$_POST['value'];
                    $return=$lotto->setSiteUrl($url);
                }
            }
            catch(Exception $e){
                error_log($e->getMessage());
                $return=-1;
            }
            die($return);
            break;

        case "updateExtractionType":
            try{
                if(!$_POST['value']){
                    throw new Exception($lang->getLang('Invalid extraction type'));
                }
                else{
                    $extractionType=$_POST['value'];
                    $return=$lotto->setExtractionType($extractionType);
                }
            }
            catch(Exception $e){
                error_log($e->getMessage());
                $return=-1;
            }
            die($return);
            break;
        case "updateOdds":
            $odds=explode(";",$lotto->getOdds());
            if($_REQUEST['name']==''){
                $_REQUEST['name']="0";
            }
            $odds[$_REQUEST['name']]=$_REQUEST['value'];
            $odds=implode(";",$odds);
            try{
                if(!$_POST['value']){
                    throw new Exception($lang->getLang('Invalid odds'));
                }
                else{
                    $return=$lotto->setOdds($odds);
                }
            }
            catch(Exception $e){
                error_log($e->getMessage());
                $return=-1;
            }
            die($return);
            break;
        case "updateConfiguration":
            $config=explode(";",$lotto->getConfig());
             if($_REQUEST['name']==''){
                 $_REQUEST['name']="0";
             }
            $config[$_REQUEST['name']]=$_REQUEST['value'];
            $config=implode(";",$config);
            try{
                if(!$_POST['value']){
                    throw new Exception($lang->getLang('Invalid config'));
                }
                else{

                    $return=$lotto->setConfig($config);
                }
            }
            catch(Exception $e){
                error_log($e->getMessage());
                $return=-1;
            }
            die($return);
            break;
        case "addNewLotto":{
            global $dbh;
            if(check_access('lotto_manage')){
                $return='';
                if($_POST['lottoName']==''){
                    $return.= $lang->getLang("Invalid name");
                }
                if($_POST['lottoCode']==''){
                    $return.= "<br />".$lang->getLang("Invalid code");
                }
                if($_POST['lottoCountry']==''){
                    $return.= "<br />".$lang->getLang("Invalid country");
                }
                if($_POST['lottoSiteUrl']==''){
                    $return.= "<br />".$lang->getLang("Invalid siteUrl");
                }
                if($_POST['lottoExtractionDays']==''){
                    $return.= "<br />".$lang->getLang("Invalid extraction days");
                }
                if($_POST['lottoType']==''){
                    $return.= "<br />".$lang->getLang("Invalid type");
                }
                if($_POST['lottoOdds']==''){
                    $return.= "<br />".$lang->getLang("Invalid 0dds");
                }  if($_POST['lottoConfig']==''){
                    $return.= "<br />".$lang->getLang("Invalid configuration");
                }
                if($_POST['lottoStatus']==''){
                    $return.= "<br />".$lang->getLang("Invalid status");
                }
                if($_POST['lottoExtractionNumbers']==''){
                    $return.= "<br />".$lang->getLang("Invalid extraction numbers");
                }
                if($return==''){
                    $sql="Insert into game_lotto(gls_code,gls_cou_code,gls_name,gls_result_site_url, gls_extraction_days,gls_type,gls_odds,gls_config,gls_status,gls_extraction_numbers)
                    Values (".$dbh->prepareString($_POST['lottoCode']).",".
                        $dbh->prepareString($_POST['lottoCountry']).",".
                        $dbh->prepareString($_POST['lottoName']).",".
                        $dbh->prepareString($_POST['lottoSiteUrl']).",".
                        $dbh->prepareString(implode(',',$_POST['lottoExtractionDays'])).",".
                        $dbh->prepareString($_POST['lottoType']).",".
                        $dbh->prepareString($_POST['lottoOdds']).",".
                        $dbh->prepareString($_POST['lottoConfig'].";0-0;0-0;0").",".
                        $dbh->prepareString($_POST['lottoStatus']).",".
                        $dbh->prepareString($_POST['lottoExtractionNumbers']).")";
                    if($dbh->exec($sql)){
                        die("1");
                    }
                }
            }
            else{
                $return="-1";
            }
            die($return);
            break;
        }
        case "getLottoResults":{
            getLottosResults($_REQUEST);
            break;
        }
        case "getLottoErrors":{
            getLottosErrors($_REQUEST);
            break;
        }
        case "getLotteries":{
            getLotteries($_REQUEST);
            break;
        }
        case "getLotteriesIncomingResults":{
            getLotteriesIncomingExtractions($_REQUEST);
            break;
        }
        case "getLotteryDetails":{
            $date_start = (isset($_REQUEST['startDate']) && $_REQUEST['startDate']!='-1') ? $_REQUEST['startDate'] : date("Y-m-d", time() - (180 * 3600 * 24));
            $date_end = (isset($_REQUEST['endDate'])) ? $_REQUEST['endDate'] : date("Y-m-d");
            $resultArr=$lotto->getNumberAppearence($lotto->getExtractionResults($date_start,$date_end)->toArray());
            die(json_encode($resultArr));
        }
        case "getLottoExtractionDetails":{
            die(getLottoExtractionResult($_REQUEST['extractionCode'],$_REQUEST['date']));
        }
        case "updateLottoExtraction":{
            die(updateExtraction($_REQUEST['extractionCode'],$_REQUEST['date'],$_REQUEST['resultValue'],$_REQUEST['extraResultValue'],$_REQUEST['timeofExtraction']));
        }
        case "checkForResults":{
            die (checkResultsExists(json_decode($_REQUEST['lotteries'])));

        }
        default:
            die('Invalid request');
            break;
    }
}

function getLottosResults($request){
    global $dbh;
    global $lang;
    $columns = array('gls_name','cou_name','lrs_extraction_code','lrs_id', 'lrs_result','lrs_extra_result','lrs_time','lrs_time_of_extraction','gls_result_site_url', 'gls_cou_code','lrs_gls_code' );
    $sTable = " lottos_result left join game_lotto on lrs_gls_code=gls_code left join country on cou_code=gls_cou_code " ;
    $limit = '';
    if ( isset($request['start']) && $request['length'] != -1 ) {
        $limit = "LIMIT ".intval($request['start']).", ".intval($request['length']);
    }
    $order = '';
    if ( isset($request['order']) && count($request['order']) ) {
        $orderBy = array();
        for ( $i=0, $ien=count($request['order']) ; $i<$ien ; $i++ ) {
            $columnIdx = intval($request['order'][$i]['column']);
            $requestColumn = $request['columns'][$columnIdx];
            $column = $columns[ $columnIdx ];

            if ( $requestColumn['orderable'] == 'true' ) {
                $dir = $request['order'][$i]['dir'] === 'asc' ?
                    'ASC' :
                    'DESC';
                $orderBy[] = '`'.$column.'` '.$dir;
            }
        }
        $order = 'ORDER BY '.implode(', ', $orderBy);
    }

    $timenow=date("Y-m-d H:i:s");
    $sWhere="WHERE gls_status=1 and lrs_time_of_extraction BETWEEN ".$dbh->prepareString($request['startDate'].":00")." AND ".$dbh->prepareString($request["endDate"].":59")."  ";
    if($request['code']!=''){
        $code=json_decode($request['code']);
        if(is_array($code)){
            $code=implode("','",$code);
        }
        $sWhere .=" AND lrs_gls_code  IN ('$code')" ;
    }

    $type=json_decode($request['type']);
    if($type!='0'){
        if($type==1) {
            $sWhere .= " AND lrs_result  IS NULL";
        }else{
            $sWhere .= " AND lrs_result  IS NOT NULL";
        }
    }

    $str = $request['search']['value'];
    if ( isset($request['search']) && $request['search']['value'] != '' ) {
        $extrasWhere = " AND ";
        $extrasWhere .=" (gls_name  LIKE '%".mysql_real_escape_string( $str )."%' OR lrs_extraction_code LIKE '%".$dbh->escape($str)."%' )";
    }
    $sQuery = "  SELECT SQL_CALC_FOUND_ROWS `".str_replace(" , ", " ", implode("`, `", $columns))."`
    FROM   $sTable
    $sWhere
    $extrasWhere
    $order
    $limit";
    $rResult = $dbh->exec($sQuery)  ;
    $sQuery = " SELECT FOUND_ROWS() ";
    $iFilteredTotal = $dbh->queryOne($sQuery) ;
    $rResultFilterTotal = $dbh->queryOne("SELECT count(*) from $sTable $sWhere");

    $tableArray=array();
    while($rResult->hasNext()){
        $row=$rResult->next();
        $section=array();
        $section[0]="<a target='_blank' href=".$row['gls_result_site_url'].">".$row['gls_name']." <br /><span class='tip'>".$row['gls_result_site_url']."</span></a><div class='hidden lottoCode'>".$row['lrs_gls_code']."</div>";
        $section[1]=$row['cou_name'];
        $section[2]="<a class='lottoExtractionCode'>".$row['lrs_extraction_code']."</a>";
        $section[3]=$row['lrs_id'];
        $section[4]=($row['lrs_result']!=''?$row['lrs_result']:"<span class='text-danger'>".$lang->getLang('Waiting for result')."</span>");
        $section[5]=$row["lrs_extra_result"];
        $section[6]=$row["lrs_time"];
        $section[7]=$row["lrs_time_of_extraction"];
        array_push($tableArray,$section);
    }

    $output= array(
        "draw"            => intval( $request['draw'] ),
        "recordsTotal"    => intval( $rResultFilterTotal ),
        "recordsFiltered" => intval( $iFilteredTotal ),
        "data"            => $tableArray
    );
    echo json_encode( $output);
}


function getLottosErrors($request){
    global $dbh;
    $columns = array( 'gls_name', 'cou_name','lte_time','lte_error_description', 'lte_gls_code');
    $sTable = " lottos_errors left join game_lotto on lte_gls_code=gls_code left join country on cou_code=gls_cou_code " ;
    $limit = '';
    if ( isset($request['start']) && $request['length'] != -1 ) {
        $limit = "LIMIT ".intval($request['start']).", ".intval($request['length']);
    }
    $order = '';
    if ( isset($request['order']) && count($request['order']) ) {
        $orderBy = array();
        for ( $i=0, $ien=count($request['order']) ; $i<$ien ; $i++ ) {
            $columnIdx = intval($request['order'][$i]['column']);
            $requestColumn = $request['columns'][$columnIdx];
            $column = $columns[ $columnIdx ];

            if ( $requestColumn['orderable'] == 'true' ) {
                $dir = $request['order'][$i]['dir'] === 'asc' ?
                    'ASC' :
                    'DESC';
                $orderBy[] = '`'.$column.'` '.$dir;
            }
        }
        $order = 'ORDER BY '.implode(', ', $orderBy);
    }

    $sWhere="WHERE lte_time BETWEEN ".$dbh->prepareString($request['startDate'].":00")." AND ".$dbh->prepareString($request["endDate"].":59");
    if($request['code']!=''){
        $code=json_decode($request['code']);
        if(is_array($code)){
            $code=implode("','",$code);
        }
        $sWhere .=" AND lte_gls_code  IN ('$code')" ;
    }

    $str = $request['search']['value'];
    if ( isset($request['search']) && $request['search']['value'] != '' ) {
        $extrasWhere = " AND ";
        $extrasWhere .=" gls_name  LIKE '%".mysql_real_escape_string( $str )."%' ";
    }
    $sQuery = "  SELECT SQL_CALC_FOUND_ROWS `".str_replace(" , ", " ", implode("`, `", $columns))."`
    FROM   $sTable
    $sWhere
    $extrasWhere
    $order
    $limit";
    $rResult = $dbh->exec($sQuery)  ;

    $sQuery = " SELECT FOUND_ROWS() ";
    $iFilteredTotal = $dbh->queryOne($sQuery) ;
    $rResultFilterTotal = $dbh->queryOne("SELECT count(*) from $sTable $sWhere");

    $tableArray=array();
    while($rResult->hasNext()){
        $row=$rResult->next();
        $section=array();
        $section[0]="<a target='_blank' href=".$row['gls_result_site_url'].">".$row['gls_name']." <br /><span class='tip'>".$row['gls_result_site_url']."</span></a><div class='hidden lottoCode'>".$row['lte_gls_code']."</div>";
        $section[1]=$row['cou_name'];
        $section[2]=$row["lte_time"];
        $section[3]=$row["lte_error_description"];
        array_push($tableArray,$section);
    }

    $output= array(
        "draw"            => intval( $request['draw'] ),
        "recordsTotal"    => intval( $rResultFilterTotal ),
        "recordsFiltered" => intval( $iFilteredTotal ),
        "data"            => $tableArray
    );
    echo json_encode( $output);
}





function showBall($result){
    $formatted=explode(',',$result['lrs_result']);
    $return='';
    $classes=array("green","yellow","pink","orange","violet","blue","red");
    foreach($formatted as $k=>$v){
        $return.="<div class='ballbet6 ".$classes[array_rand($classes,1)]."'>$v</div>";
    }
    return $return;
}


/**
 * @param $id
 * @param $date
 * @return string
 */
function getLottoExtractionResult($id,$date){
    global $dbh;
    $sql=" Select lrs_extraction_code,lrs_gls_code, gls_name,gls_result_site_url, gls_cou_code, lrs_result ,lrs_extra_result,lrs_time,lrs_time_of_extraction from lottos_result left join game_lotto on lrs_gls_code=gls_code  where lrs_extraction_code=".$dbh->prepareString($id);
    $result=$dbh->queryRow($sql);
    $return[0]=$result['gls_name']." ( ".$result['gls_cou_code']." )";
    $return[1]=$result['lrs_result'];
    $return[2]=$result['lrs_extra_result'];
    $return[3]=$result['lrs_time'];
    $return[4]=$result['lrs_time_of_extraction'];
    $return[5]=$result['lrs_extraction_code'];
    return json_encode($return);

}


function updateExtraction($code,$date,$result=false,$extraResult=false,$timeOfExtraction)
{
    global $dbh;
    if (!(checkHashedPassword($_SESSION['admin_username'], $_POST['token']))) {
        die("-2");
    }
    if (check_access('lotto_casino_result_manage')) {
        if (!$code) {
            return 'Invalid lottery code';
        }
        if (!$date) {
            error_log('Invalid lottery date');
            return '-1';
        }
        $sql = "Update lottos_result set ";
        $currLog = '';
        if ($result) {
            $sql .= "lrs_result=" . $dbh->prepareString($result);
            $previousRes = $dbh->queryOne("Select lrs_result from lottos_result where lrs_extraction_code=" . $dbh->prepareString($code));
            if ($previousRes != $result) {
                $currLog .= "Has changed the lotto result for lottery " . $dbh->escape($code . "($date) from " . $previousRes . " to " . $result . ";");
            }
        }
        if ($extraResult) {
            if ($result) {
                $sql .= ",";
            }
            $sql .= "lrs_extra_result=" . $dbh->prepareString($extraResult);
            $previousRes = $dbh->queryOne("Select lrs_result from lottos_result where lrs_extraction_code=" . $dbh->prepareString($code)  );
            if ($previousRes != $extraResult) {
                $currLog .= "Has changed the lotto extra result for lottery " . $dbh->escape($code . "($date) from $previousRes to " . $extraResult . ";");
            }
        }

        if ($timeOfExtraction) {
            if ($result || $extraResult) {
                $sql .= ",";
            }
            $sql .= "lrs_time_of_extraction=" . $dbh->prepareString($timeOfExtraction);
            $previousRes = $dbh->queryOne("Select lrs_time_of_extraction from lottos_result where lrs_extraction_code=" . $dbh->prepareString($code) );
            if ($previousRes != $timeOfExtraction) {
                $currLog .= "Has changed the lotto extractionTime for lottery " . $dbh->escape($code . "($date) from $previousRes to " . $timeOfExtraction . ";");
            }
        }

        $sql .= "  where lrs_extraction_code=" . $dbh->prepareString($code);
        if ($dbh->exec($sql)) {
            if ($currLog != '') {
                doAdminUserLog($_SESSION['admin_id'], 'lotto_result_update', $currLog);
            }
            return "1";
        } else {
            return "-1";
        }
    } else {
        return "-1";
    }

}


/**
 * @param bool $country
 * @param bool $extractionDays
 * @param int $type
 * @param int $status
 * @return bool|int|RecordSet
 */
function getLotteries(){
    global $dbh;

    $columns = array('gls_name','gls_code', 'gls_cou_code', 'gls_result_site_url', 'gls_extraction_days','gls_extraction_numbers','gls_type','gls_status','gls_odds','gls_config','gls_id');
    $sTable1 = "   game_lotto,lottos_result " ;
    $sTable2 = "   game_lotto " ;
    $limit = '';
    if ( isset($_REQUEST['start']) && $_REQUEST['length'] != -1 && $_REQUEST['order'][0]['column']!=9 ) {
        $limit = "LIMIT ".intval($_REQUEST['start']).", ".intval($_REQUEST['length']);
    }
    $order = '';
    if ( isset($_REQUEST['order']) && count($_REQUEST['order']) && $_REQUEST['order'][0]['column']!=9 ) {
        $orderBy = array();
        for ( $i=0, $ien=count($_REQUEST['order']) ; $i<$ien ; $i++ ) {
            $columnIdx = intval($_REQUEST['order'][$i]['column']);
            $_REQUESTColumn = $_REQUEST['columns'][$columnIdx];
            $column = $columns[ $columnIdx ];

            if ( $_REQUESTColumn['orderable'] == 'true' ) {
                $dir = $_REQUEST['order'][$i]['dir'] === 'asc' ?
                    'ASC' :
                    'DESC';
                $orderBy[] = '`'.$column.'` '.$dir;
            }
        }
        $order = 'ORDER BY '.implode(', ', $orderBy);
    }

    $sWhere1="WHERE gls_code = lrs_gls_code
        and  LRS_TIME_OF_EXTRACTION between DATE_FORMAT(DATE_ADD(NOW(), INTERVAL -1 DAY), '%Y-%m-%d 23:59:59') and DATE_FORMAT(DATE_ADD(NOW(), INTERVAL 13 DAY), '%Y-%m-%d 23:59:59') ";

    $sWhere2=" WHERE gls_status=0 ";
    if($_REQUEST['country']!=''){
        $country=json_decode($_REQUEST['country']);
        if(is_array($country)){
            $country=implode("','",$country);
        }
        $sWhere1 .=" AND gls_cou_code  IN ('$country')" ;
        $sWhere2 .=" AND gls_cou_code  IN ('$country')" ;
    }
    $extractionDays =json_decode($_REQUEST['extractionDays']);
    if($extractionDays && count($extractionDays)<7){
        if(is_array($extractionDays)){
            $extractionDays=implode("",$extractionDays);
        }
        $sWhere1.=" AND gls_extraction_days  REGEXP '([$extractionDays])'" ;
        $sWhere2 .=" AND gls_extraction_days  REGEXP '([$extractionDays])'" ;
    }
    $type=$_REQUEST['type'];
    if($type!='2'){
        $sWhere1.=" AND gls_type =".$dbh->escape($type) ;
        $sWhere2 .=" AND gls_type =".$dbh->escape($type) ;
    }
    $status=strval($_REQUEST['status']);
    if($status!="2"){
        $sWhere1.=" AND gls_status =".$dbh->escape($status) ;
        $sWhere2 .=" AND gls_status =".$dbh->escape($status) ;
    }
    $lottos=json_decode($_REQUEST['lottos']);
    if($lottos){
        if(is_array($lottos)){
            $lottos=implode("','",$lottos);
        }
        $sWhere1.=" AND gls_code IN ('$lottos')" ;
        $sWhere2 .=" AND gls_code IN ('$lottos')" ;
    }
    $groupBy=" group by gls_code ";

    $str = $_REQUEST['search']['value'];
    if ( isset($_REQUEST['search']) && $_REQUEST['search']['value'] != '' ) {
        $extrasWhere = " AND ";
        $extrasWhere .=" (gls_name  LIKE '%".mysql_real_escape_string( $str )."%' ";
        $extrasWhere .=" OR gls_code  LIKE '%".mysql_real_escape_string( $str )."%' )";
    }
    $sQuery = "  SELECT SQL_CALC_FOUND_ROWS `".str_replace(" , ", " ", implode("`, `", $columns))."` , count(*) as nrExtractions , gls_extractions_counter
    FROM   $sTable1
    $sWhere1
    $extrasWhere
    $groupBy";
    if($_REQUEST['verified']==1){
        $sQuery.=" HAVING nrExtractions=gls_extractions_counter";
    }
    elseif($_REQUEST['verified']==2){
        $sQuery.=" HAVING nrExtractions<>gls_extractions_counter";
    }
    $sQuery.=" UNION ALL
    SELECT `".str_replace(" , ", " ", implode("`, `", $columns))."` ,0 as nrExtractions ,0 gls_extractions_counter
    FROM   $sTable2
    $sWhere2
    $extrasWhere
    $groupBy
    $order
    $limit  ";

    $rResult = $dbh->exec($sQuery)  ;

    $sQuery = " SELECT FOUND_ROWS() ";
    $iFilteredTotal = $dbh->queryOne($sQuery) ;
    //$rResultFilterTotal = $dbh->queryOne("SELECT count(*) from $sTable $sWhere1 $groupBy");
    $count=0;
    $tableArray=array();
    while($rResult->hasNext()){
        $row=$rResult->next();
        $section=array();
        $configuration=explode(";",$row['gls_config']);
        $maxOdds=$configuration[0]-1;
        $odds=explode(";",$row['gls_odds']);
        if($maxOdds>=10){
            $maxOdds=9;
        }
        $section[0]=$row['gls_name']."<br />".($row['gls_extractions_counter']==$row['nrextractions'] && $row['gls_status']!=0 ? "<span class='text-success'>Verified" : ($row['gls_status']==0? "<span class='tip'>( Disabled )": "<span class='text-error'>Failed verification"))."</span>" ;
        $section[1]=$row['gls_code'];
        $section[2]='<a href="#" class="country" data-type="select2"  data-value="'.$row['gls_cou_code'].'" data-title="Select country"></a>';
        $section[3]='<a class="editableSiteClass" style="width: 200px;word-break: break-all" data-title="Enter extraction site">'.$row['gls_result_site_url'].'</a>';
        $section[4]='<a href="#" data-id="'.$row['gls_code'].'" class="daysSelect" data-type="checklist" data-value="'.$row['gls_extraction_days'].'" data-title="Select days"  data-original-title="" title="" style="background-color: rgba(0, 0, 0, 0);">'.daysReadable($row['gls_extraction_days']).'</a>';
        $section[5]='<a class="editableExtractionType" data-title="Enter extraction type">'.$row['gls_extraction_numbers'].'</a>';
        $section[6]='<a href="#" class="simpleTypeSelect" data-type="select"  data-title="Type">'.($row['gls_type']==0? "Lotto":"Keno").'</a>';
        $section[7]='<div data-type="select"   data-title="Enable/disable" class="statusEditableSelect '.($row['gls_status']==0?  'text-error ">Disabled' : ' text-success"> Enabled').'</div>';
        $section[8]='<div class="editableOddsClass" data-title="Edit odds">';
        for($i=0;$i<=$maxOdds;$i++){
            $section[8].="<a href='#' data-name='".$i."' data-type='text' data-value='".(isset($odds[$i])?(($odds[$i]=='TO FILL')?'Empty':$odds[$i]):'')."' title='Quota ".$i."' class='editable-click ".(isset($odds[$i])? '':'editable-empty')."'>".(isset($odds[$i])? (($odds[$i]=='TO FILL')?'Empty':$odds[$i]):'Empty')."</a>&nbsp;";
        }
        $section[8].='</div>';
        $section[9]='<div class="editableConfigClass" data-title="Edit configuration">';
        for($i=0;$i<=2;$i++){
            if($i==0){
                $title="Balls extracted";
            }
            elseif($i==1){
                $title="Balls chosen";
            }
            else{
                $title="Min/max balls";
            }
            $section[9].="<div>".$title." :<a href='#' data-name='".$i."' data-type='text' data-value='".$configuration[$i]."' title='".$title."' class='editable-click'>".$configuration[$i]."</a></div>";
        }
        $section[9].="</div>";
        $section[10]='<a href="'.$row['gls_result_site_url'].'" target="_blank"><button class="btn btn-small showLotteryDetails">View site</button></a>';
        /*  $section[9]=calculateTimeUntilExtraction($row['gls_extraction_days'],$row['gls_extraction_numbers'],true);*/
        $count++;
        array_push($tableArray,$section);
    }
    /*       if($_REQUEST['order'][0]['column']==9){
               usort($tableArray,function($a,$b){
                 if($_REQUEST['order'][0]['dir']=='asc') {
                     return $a[9] - $b[9];
                 }
                 elsE{
                       return $b[9] - $a[9];
                 }
               });
               $tableArray = array_slice($tableArray, $_REQUEST['start'],10);
           }
            $count=0;
          foreach($tableArray as $k=>$v){


                $tableArray[$k][9]='<div id="clock'.$count.'" class="countdownContainer">'.$tableArray[$k][9]."</div>";

            }*/

    $output= array(
        "draw"            => intval( $_REQUEST['draw'] ),
        "recordsTotal"    => intval( $iFilteredTotal ),
        "recordsFiltered" => intval( $iFilteredTotal ),
        "data"            => $tableArray
    );
    echo json_encode( $output);
}

function getLotteriesIncomingExtractions(){
    global $dbh;

    $columns = array('gls_name','lrs_extraction_code', 'cou_name', 'gls_result_site_url', 'lrs_time_of_extraction');
    $sTable = "   game_lotto left join country on cou_code=gls_cou_code
                  left join lottos_result on lrs_gls_code=gls_code " ;
    $limit = '';
    if ( isset($_REQUEST['start']) && $_REQUEST['length'] != -1 && $_REQUEST['order'][0]['column']!=4 ) {
        $limit = "LIMIT ".intval($_REQUEST['start']).", ".intval($_REQUEST['length']);
    }
    $order = '';
    if ( isset($_REQUEST['order']) && count($_REQUEST['order']) && $_REQUEST['order'][0]['column']!=4 ) {
        $orderBy = array();
        for ( $i=0, $ien=count($_REQUEST['order']) ; $i<$ien ; $i++ ) {
            $columnIdx = intval($_REQUEST['order'][$i]['column']);
            $_REQUESTColumn = $_REQUEST['columns'][$columnIdx];
            $column = $columns[ $columnIdx ];

            if ( $_REQUESTColumn['orderable'] == 'true' ) {
                $dir = $_REQUEST['order'][$i]['dir'] === 'asc' ?
                    'ASC' :
                    'DESC';
                $orderBy[] = '`'.$column.'` '.$dir;
            }
        }
        $order = 'ORDER BY '.implode(', ', $orderBy);
    }

    $sWhere="WHERE lrs_result is null and lrs_time_of_extraction > now() and gls_status=1 ";


    $lottos=json_decode($_REQUEST['lottos']);
    if($lottos){
        if(is_array($lottos)){
            $lottos=implode("','",$lottos);
        }
        $sWhere .=" AND gls_code IN ('$lottos')" ;
    }
    $group =" GROUP BY lrs_extraction_code";

    $str = $_REQUEST['search']['value'];
    if ( isset($_REQUEST['search']) && $_REQUEST['search']['value'] != '' ) {
        $extrasWhere = " AND ";
        $extrasWhere .=" (gls_name  LIKE '%".mysql_real_escape_string( $str )."%' ";
        $extrasWhere .=" OR gls_code  LIKE '%".mysql_real_escape_string( $str )."%' )";
    }
    $sQuery = "  SELECT SQL_CALC_FOUND_ROWS `".str_replace(" , ", " ", implode("`, `", $columns))."`
    FROM   $sTable
    $sWhere
    $extrasWhere
    $group
    $order
    $limit";
    $rResult = $dbh->exec($sQuery)  ;
    $sQuery = " SELECT FOUND_ROWS() ";
    $iFilteredTotal = $dbh->queryOne($sQuery) ;
    $rResultFilterTotal = $dbh->queryOne("SELECT count(*) from $sTable $sWhere");
    $count=0;
    $tableArray=array();
    while($rResult->hasNext()){
        $row=$rResult->next();
        $section=array();
        $section[0]=$row['gls_name'];
        $section[1]=$row['lrs_extraction_code'];
        $section[2]=$row['cou_name'];
        $section[3]='<a href="'.$row['gls_result_site_url'].'" target="_blank">'.$row['gls_result_site_url'].'</a>';
        //  $section[4]=calculateTimeUntilExtraction($row['gls_extraction_days'],$row['gls_extraction_numbers'],true);
        $section[4]= array(strtotime($row['lrs_time_of_extraction'])-strtotime('now')-0,$row['lrs_time_of_extraction']);
        $count++;
        array_push($tableArray,$section);
    }
    if($_REQUEST['order'][0]['column']==4){
        usort($tableArray,function($a,$b){
            if($_REQUEST['order'][0]['dir']=='asc') {
                return $a[4][0] - $b[4][0];
            }
            elsE{
                return $b[4][0] - $a[4][0];
            }
        });
        $tableArray = array_slice($tableArray, $_REQUEST['start'],intval($_REQUEST['length']));
    }
    $count=0;
    foreach($tableArray as $k=>$v){
        $class='';

        if($v[4][0]<=600){
            $class='lessThanTenMinutes';
        }
        elseif($v[4][0]<=3600){
            $class='lessThanHour';
        }
        $tableArray[$k][4]='<div id="clock'.$count.'" class="countdownContainer '.$class.'" data-end="'.$tableArray[$k][4][1].'" >'.$tableArray[$k][4][0]."</div>";
    }

    $output= array(
        "draw"            => intval( $_REQUEST['draw'] ),
        "recordsTotal"    => intval( $rResultFilterTotal ),
        "recordsFiltered" => intval( $iFilteredTotal ),
        "data"            => $tableArray
    );
    echo json_encode( $output);

}
/*function getLotteriesIncomingExtractions(){
    global $dbh;

    $columns = array('gls_name','gls_code', 'cou_name', 'gls_result_site_url', 'gls_extraction_numbers','gls_extraction_days');
    $sTable = "   game_lotto left join country on cou_code=gls_cou_code " ;
    $limit = '';
    if ( isset($_REQUEST['start']) && $_REQUEST['length'] != -1 && $_REQUEST['order'][0]['column']!=4 ) {
        $limit = "LIMIT ".intval($_REQUEST['start']).", ".intval($_REQUEST['length']);
    }
    $order = '';
    if ( isset($_REQUEST['order']) && count($_REQUEST['order']) && $_REQUEST['order'][0]['column']!=4 ) {
        $orderBy = array();
        for ( $i=0, $ien=count($_REQUEST['order']) ; $i<$ien ; $i++ ) {
            $columnIdx = intval($_REQUEST['order'][$i]['column']);
            $_REQUESTColumn = $_REQUEST['columns'][$columnIdx];
            $column = $columns[ $columnIdx ];

            if ( $_REQUESTColumn['orderable'] == 'true' ) {
                $dir = $_REQUEST['order'][$i]['dir'] === 'asc' ?
                    'ASC' :
                    'DESC';
                $orderBy[] = '`'.$column.'` '.$dir;
            }
        }
        $order = 'ORDER BY '.implode(', ', $orderBy);
    }

    $sWhere="WHERE 1=1 ";


    $lottos=json_decode($_REQUEST['lottos']);
    if($lottos){
        if(is_array($lottos)){
            $lottos=implode("','",$lottos);
        }
        $sWhere .=" AND gls_code IN ('$lottos')" ;
    }


    $str = $_REQUEST['search']['value'];
    if ( isset($_REQUEST['search']) && $_REQUEST['search']['value'] != '' ) {
        $extrasWhere = " AND ";
        $extrasWhere .=" (gls_name  LIKE '%".mysql_real_escape_string( $str )."%' ";
        $extrasWhere .=" OR gls_code  LIKE '%".mysql_real_escape_string( $str )."%' )";
    }
    $sQuery = "  SELECT SQL_CALC_FOUND_ROWS `".str_replace(" , ", " ", implode("`, `", $columns))."`
    FROM   $sTable
    $sWhere
    $extrasWhere
    $order
    $limit";

    $rResult = $dbh->exec($sQuery)  ;
    $sQuery = " SELECT FOUND_ROWS() ";
    $iFilteredTotal = $dbh->queryOne($sQuery) ;
    $rResultFilterTotal = $dbh->queryOne("SELECT count(*) from $sTable $sWhere");
    $count=0;
    $tableArray=array();
    while($rResult->hasNext()){
        $row=$rResult->next();
        $section=array();
        $section[0]=$row['gls_name'];
        $section[1]=$row['gls_code'];
        $section[2]=$row['cou_name'];
        $section[3]='<a href="'.$row['gls_result_site_url'].'" target="_blank">'.$row['gls_result_site_url'].'</a>';
        $section[4]=calculateTimeUntilExtraction($row['gls_extraction_days'],$row['gls_extraction_numbers'],true);
        $count++;
        array_push($tableArray,$section);
    }
    if($_REQUEST['order'][0]['column']==4){
        usort($tableArray,function($a,$b){
            if($_REQUEST['order'][0]['dir']=='asc') {
                return $a[4][0] - $b[4][0];
            }
            elsE{
                return $b[4][0] - $a[4][0];
            }
        });
        $tableArray = array_slice($tableArray, $_REQUEST['start'],intval($_REQUEST['length']));
    }
    $count=0;
    foreach($tableArray as $k=>$v){
        $class='';

        if($v[4][0]<=600){
            $class='lessThanTenMinutes';
        }
        elseif($v[4][0]<=3600){
            $class='lessThanHour';
        }
        $tableArray[$k][4]='<div id="clock'.$count.'" class="countdownContainer '.$class.'" data-end="'.$tableArray[$k][4][1].'" >'.$tableArray[$k][4][0]."</div>";
    }

    $output= array(
        "draw"            => intval( $_REQUEST['draw'] ),
        "recordsTotal"    => intval( $rResultFilterTotal ),
        "recordsFiltered" => intval( $iFilteredTotal ),
        "data"            => $tableArray
    );
    echo json_encode( $output);

}*/

function sortResultsBy($a, $b)
{
    return strcmp($a->display_name, $b->display_name);
}


function formatExtractionType($extractionString){
    list($type,$extrationTime)=explode('~',$extractionString);
    $extrationTime=explode('~',$extrationTime);


}
/*function formatCountry($country){
    $formattedCountryArr=array();
    foreach($country as $k=>$v){
        $formattedCountryArr[$v['cou_code']]=$v['cou_name'];
    }
    return $formattedCountryArr;
}*/

function calculateTimeUntilExtraction($daysOfExtractions,$timeUntilNextExtraction,$diff=false) {
    $daysOfExtractions=explode(",",$daysOfExtractions);
    $time='';
    $extractionDays=array();
    $nextExtractionDay=date('N');
    list($type,$timeString)=explode('~',$timeUntilNextExtraction);
    $extractions=explode(",",$timeString);
    foreach($daysOfExtractions as $k=>$v){
        $extractionDays[$v]=$extractions[$k];
    }
    $isExtractionToday=isExtractionToday($extractionDays,$nextExtractionDay,$type);
    /*  $previous=wasPreviousExtractionToday($extractionDays,$nextExtractionDay,$type);
      if(!$previous){
          $previous=getPreviousExtractionDate($extractionDays,$nextExtractionDay,$type);
      }*/

    if($isExtractionToday){
        $time=$isExtractionToday;

    }
    else{
        $nextExtractionDay=getNextExtractionDays($extractionDays,$nextExtractionDay+1);
        $time=calculateTimeofExtraction($nextExtractionDay,$extractionDays[$nextExtractionDay]);
    }
    if($diff){
        return  array(strtotime($time)-strtotime('now')-0,$time);
    }
    return $time;
}

function calculateTimeofExtraction($day,$hours){
    $currentDay=date('N');
    list($hoursOfExtractions,$interval)=explode(";",$hours);
    $hoursOfExtractions=explode('-',$hoursOfExtractions);
    if($day<$currentDay){
        $daysUntilNextExtraction=(7-$currentDay)+$day;
    }
    else{
        $daysUntilNextExtraction=($day-$currentDay);
    }
    $today=date("Y-m-d");
    $time=date('Y-m-d', strtotime($today." + $daysUntilNextExtraction days"))." ".$hoursOfExtractions[1];
    return $time;
}



function wasPreviousExtractionToday($extractionDays,$day,$type){

    $found=false;
    $previous=false;
    $timeNow=strtotime("now");
    if(isset($extractionDays[$day]) && count($extractionDays)>1){

        list($currentDays,$interval)=explode(';',$extractionDays[$day]);
        $nextExtractionDetails=explode('-', $currentDays);
        // return $extractionDays."==>".$today."==>".$type;
        foreach($nextExtractionDetails as $k=>$v){
            if($k!=0 && !$found) {
                if (strtolower($type) == 'f') {
                    if ($timeNow < strtotime(date('Y-m-d')." ".$v)) {
                        $found = true ;
                    }
                    elsE{
                        $previous= date('Y-m-d') . " " . $v ;
                    }
                }
                else {
                    $firstTime = date('Y-m-d') . " " . $v;
                    $firstTimeTimeStamp = strtotime($firstTime);
                    for ($i = 0; $i <= $nextExtractionDetails[0]; $i++) {
                        if(!$found) {
                            if (strtolower($type[1]) == 'm') {
                                $currInterval = $firstTimeTimeStamp + ( (60 * $i*$interval));
                            } elsE {
                                $currInterval = $firstTimeTimeStamp + ((3600 * $i*$interval));
                            }
                            if (strtotime('now')<$currInterval  ) {
                                $found = true;
                            }
                            else{
                                $previous=date('Y-m-d H:i', $currInterval);
                            }
                        }
                    }
                }
            }
        }

    }


    return $previous;
}


function isExtractionToday($extractionDays,$today,$type,$previous=false){
    $found=false;
    $timeNow=strtotime("now");

    if(isset($extractionDays[$today])){
        list($currentDays,$interval)=explode(';',$extractionDays[$today]);
        $nextExtractionDetails=explode('-', $currentDays);
        // return $extractionDays."==>".$today."==>".$type;
        foreach($nextExtractionDetails as $k=>$v){
            if($k!=0 && !$found) {
                if (strtolower($type) == 'f') {
                    if ($timeNow < strtotime(date('Y-m-d')." ".$v)) {
                        $found = date('Y-m-d') . " " . $v ;
                    }
                } else {
                    $firstTime = date('Y-m-d') . " " . $v;
                    $firstTimeTimeStamp = strtotime($firstTime);

                    for ($i = 0; $i <= $nextExtractionDetails[0]; $i++) {

                        if(!$found) {
                            if (strtolower($type[1]) == 'm') {
                                $currInterval = $firstTimeTimeStamp + ( (60 * $i*$interval));
                            } elsE {
                                $currInterval = $firstTimeTimeStamp + ((3600 * $i*$interval));
                            }

                            if ($currInterval > strtotime('now')) {
                                $found = date('Y-m-d H:i', $currInterval);
                            }
                        }
                    }
                }
            }
        }
    }

    return $found;
}


function getNextExtractionDays($extractionDays,$currentDay){

    if(isset($extractionDays[$currentDay])){
        return $currentDay;
    }
    else{
        $nextExtraction='';
        foreach($extractionDays as $key=>$value){
            if($nextExtraction=='' || $key >= $currentDay){
                $nextExtraction=$key;
            }
            if($key>=$currentDay){
                break;
            }
        }
        return $nextExtraction;
    }
}

function getPreviousExtractionDate($extractionDays,$currentDay,$type){
    $found=false;
    $previous=false;
    foreach($extractionDays as $k=>$v){
        if(!$found) {
            if ($k >= $currentDay) {
                if($previous){
                    $found = true;
                }
                if (count($extractionDays) == 1) {
                    $previous = $k;
                }

            } elsE {
                $previous = $k;
            }
        }
    }
    if(!$found){
        $previous=$k;
    }

    if(date('N')-$previous>0){
        $diff=date('N')-$previous;
    }
    else{
        $diff=7+date('N')-$previous;
    }
    list($currentDays,$interval)=explode(';',$extractionDays[$previous]);
    $nextExtractionDetails=explode('-', $currentDays);
    if (strtolower($type) == 'f') {
        $time=date("Y-m-d")." ".end($nextExtractionDetails);
    }
    else {
        $firstTime = date('Y-m-d') . " " . end($nextExtractionDetails);
        $firstTimeTimeStamp = strtotime($firstTime);
        if (strtolower($type[1]) == 'm') {
            $currInterval = $firstTimeTimeStamp + ( (60 * $nextExtractionDetails[0]*$interval));
        } elsE {
            $currInterval = $firstTimeTimeStamp + ((3600 * $nextExtractionDetails[0]*$interval));
        }
        $time=date("Y-m-d H:i",$currInterval);
    }

    $time=date('Y-m-d H:i', strtotime($time." - $diff days"));



    return $time;
}



/**
 * @param $days
 * @return string
 */
function daysReadable($days){
    $days=explode(',',$days);
    $formattedDays= array_map(function($val) {
        $daysArr=array(1=>"Monday",
            2=>"Tuesday",
            3=>"Wednesday",
            4=>"Thursday",
            5=>"Friday",
            6=>"Saturday",
            7=>"Sunday");
        return $daysArr[$val]; }, $days);
    return implode(',',$formattedDays);
}



function checkResultsExists($lotteries){
    global $dbh;
    $a=array();
    if(count($lotteries)>0) {
        $sql=" SELECT lrs_extraction_code, lrs_time from lottos_result where LRS_RESULT IS NOT NULL  AND ( ";
        foreach ($lotteries as $k => $v) {
            $sql.="  lrs_extraction_code=".$dbh->prepareString($k)." OR";
        }
        $sql=substr($sql,0,-2);
        $sql.=")";
        $result= $dbh->exec($sql);
        while($result->hasNext()) {
            $row = $result->next();
            $a[$row['lrs_extraction_code']]=$row['lrs_time'];
        }
    }
    return json_encode($a);
}

