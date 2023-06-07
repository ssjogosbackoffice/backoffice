<?php
check_access("transactions_search", true);
$adminTranSearch=check_access("admin_transaction_search");
$userTranSearch=check_access("user_transaction_search");
$affiliateTranSearch=check_access("affiliate_transaction_search");
$searchType=(isset($_REQUEST['srctype']) ? $_REQUEST['srctype'] : "0" );
?>
<script src="<?= secure_host ?>/media/ui/js/jquery.timepicker.js" type="text/javascript"></script>


<script type="text/javascript">
    $(function() {
        $( "#from" ).datetimepicker({
            changeMonth: true,
            numberOfMonths:1
        });
        $( "#to" ).datetimepicker({
            changeMonth: true,
            numberOfMonths:1
        });

        <? if ($adminTranSearch) { ?>
        $("#adminFrom").datetimepicker({
            numberOfMonths: 1,
            dateFormat: 'yy-mm-dd',
            onSelect: function (selectedDate) {
                $("#adminTo").datetimepicker("option", "minDate", selectedDate);
                dateMax = $("#adminFrom").datetimepicker("getDate");
                var rangeMax = new Date(dateMax.getFullYear(), dateMax.getMonth() , dateMax.getDate()+7);
                $("#adminTo").datetimepicker("option", "maxDate", rangeMax);
            }
        });
        if($("#adminFrom").val()!==''){
            dateMax = $("#adminFrom").datetimepicker("getDate");
            var rangeMax = new Date(dateMax.getFullYear(), dateMax.getMonth() , dateMax.getDate()+7);
            dateMin=$("#adminFrom").val();
        }
        else{
            rangeMax='';
            dateMin='';
        }
        $("#adminTo").datetimepicker({
            maxDate:rangeMax,
            minDate:dateMin,
            numberOfMonths: 1,
            dateFormat: 'yy-mm-dd'
        });
        <? } ?>
    });

</script>
<?php
//modified by Marian
globalise('action');
globalise('field');
globalise('value');
globalise('from');
globalise('to');
globalise('typetransaction');
globalise('operator');

define("SEARCH_MAX_HOURS", 24*7);

if(!isset($from)){    $from = date("Y-m-d H:i", strtotime("now - 2 hours"));}
if(!isset($to)){ $to = date("Y-m-d H:i", strtotime("now"));}
if(isset($_POST['jurisdiction_level'])){
    $jurisdiction_level = $_POST['jurisdiction_level'];
}

if(($to - $from) / 60 / 60 > SEARCH_MAX_HOURS){
    addError("",  $lang->getLang("You have selected an interval too long"));
}

function searchAdminTransaction(){
$starttime = microtime(true);
    global $dbh,$lang,$from, $to,$value;
    $searchType = $_POST["field"];
    $sql="Select att_time,
                att_note,
                att_transaction_id,
                IF (att_tty_id IS NOT NULL, (select tty_description from transaction_type where tty_id = att_tty_id), 'N') tty_description,
                IF (att_aus_id IS NOT NULL, (select aus_username from admin_user where aus_id = att_aus_id), 'N') aus_username,
                IF (att_aus_final_id IS NOT NULL, (select aus_username from admin_user where aus_id = att_aus_final_id), 'N') aus_final_username,
                IF (att_pun_id IS NOT NULL, (select pun_username from punter where pun_id = att_pun_id), 'N') pun_username,
                IF (att_jur_id IS NOT NULL, (select jur_name from jurisdiction where jur_id=att_jur_id), 'N') first_jurisdiction,
                IF (att_final_jur_id IS NOT NULL, (select jur_name from jurisdiction where jur_id=att_final_jur_id), 'N') second_jurisdiction,
                att_amount,
                att_jur_avail_credit,
                att_jur_final_avail_credit
                FROM admin_user_transaction
                WHERE  1=1 ";

    if($searchType=="name"){
        //Username
        $sql .= " AND att_aus_id  IN (SELECT aus_id FROM admin_user where aus_username=".$dbh->prepareString($value).")";
    }
    if($searchType=="transaction"){
        //user id
        $sql .= " AND att_transaction_id=". $dbh->prepareString($value);
    }
    if($searchType=="jurisdiction"){
        if($_POST['jurisdiction']==''){
            addError('','Please select a jurisdiction');
        }
        $sql.=" AND (att_jur_id=".$_POST['jurisdiction']."
                OR att_final_jur_id=".$_POST['jurisdiction'].")";

    }
    if($searchType != 3){
        $sql .= " AND att_time BETWEEN '$from' AND '$to' ";
    }
    /*     $jurisdictionClass=$_SESSION['jurisdiction_class'];
         $jurisdictionPostClass=$_POST['jurisdiction_level'];
         $params="SELECT  att_time,
                       att_note,
                       IF (att_tty_id IS NOT NULL, (select tty_description from transaction_type where tty_id = att_tty_id), 'N') tty_description,
                       IF (att_aus_id IS NOT NULL, (select aus_username from admin_user where aus_id = att_aus_id), 'N') aus_username,
                       IF (att_aus_final_id IS NOT NULL, (select aus_username from admin_user where aus_id = att_aus_final_id), 'N') aus_final_username,
                       IF (att_pun_id IS NOT NULL, (select pun_username from punter where pun_id = att_pun_id), 'N') pun_username,
                       IF (att_jur_id IS NOT NULL, (select jur_name from jurisdiction where jur_id=att_jur_id), 'N') first_jurisdiction,
                       IF (att_final_jur_id IS NOT NULL, (select jur_name from jurisdiction where jur_id=att_final_jur_id), 'N') second_jurisdiction,
                       att_amount,
                       att_jur_avail_credit,
                       att_jur_final_avail_credit";
           $sql.="      FROM admin_user_transaction aut, admin_user aun,admin_user aur,admin_user aud,admin_user auc, jurisdiction n
                 WHERE n.jur_id =4
                 AND n.jur_id = aus_jurisdiction_id
                 AND ( aus_id = att_aus_id
                     OR aus_id = att_aus_final_id
                 )
               ORDER BY ATT_TIME
             LIMIT 0 , 30";

         switch ($jurisdictionClass)
         {
             case 'casino':
                 $searchFrom =array('casino'=> " jurisdiction ca,admin_user auca",
                                     'nation'=> " jurisdiction n,admin_user aun",
                                     "region"=>"jurisdiction r,admin_user aur",
                                     "district"=>"jurisdiction d,admin_user aud",
                                     "club"=>"jurisdiction c,admin_user auc");
                 break;
             case 'nation':
                 $searchFrom =array('nation'=> " jurisdiction n,admin_user aun",
                                    "region"=>"jurisdiction r,admin_user aur",
                                     "district"=>"jurisdiction d,admin_user aud",
                                     "club"=>"jurisdiction c,admin_user auc");
                 break;
             case 'region':
                 $searchFrom =array("region"=>"jurisdiction r,admin_user aur",
                                     "district"=>"jurisdiction d,admin_user aud",
                                     "club"=>"jurisdiction c,admin_user auc");
                 break;
             case 'district':
                 $searchFrom =array("district"=>"jurisdiction d,admin_user aud",
                                     "club"=>"jurisdiction c,admin_user auc");
                 break;
             default:
                 $searchFrom =array("club"=>"jurisdiction c,admin_user auc");
         }

         $count=-1;
         foreach ($searchFrom as $k=>$v){
             if($k==$jurisdictionPostClass){
                 break;
             }
             $count++;
         }
         array_splice($searchFrom,1,$count);
         if(!isset($_POST['subjurisdictions'])){
             array_splice($searchFrom,2);
         }
         $searchFrom=implode(',',$searchFrom);
        $where=' WHERE 1=1 ';
         if($searchType != 3){
             $where .= " AND att_time BETWEEN '$from' AND '$to' ";
         }
         switch ($jurisdictionPostClass ){
             case 'nation':
                 $where.="   AND n.jur_id=".$_POST['jurisdiction']."
                             AND n.jur_id=aun.aus_jurisdiction_id
                             AND (aun.aus_id = att_aus_id OR aun.aus_id = att_aus_final_id ";
                 if(isset($_POST['subjurisdictions'])){
                     $where .=" OR aur.aus_id = att_aus_id OR aur.aus_id = att_aus_final_id
                                 OR aud.aus_id = att_aus_id  OR aud.aus_id = att_aus_final_id
                                  OR auc.aus_id = att_aus_id OR auc.aus_id = att_aus_final_id )
                              AND r.jur_id=aur.aus_jurisdiction_id
                              AND d.jur_id=aud.aus_jurisdiction_id
                              AND c.jur_id=auc.aus_jurisdiction_id ";
                 }
                 else{
                     $where.=") ";
                 }

                 break;
             case 'region':
                 $where.=" AND r.jur_id=".$_POST['jurisdiction']."
                         AND r.jur_id = aur.aus_jurisdiction_id
                         AND (aur.aus_id = att_aus_id OR aur.aus_id = att_aus_final_id ";
                 if(isset($_POST['subjurisdictions'])){
                     $where .=" OR aud.aus_id = att_aus_id  OR aud.aus_id = att_aus_final_id
                                 OR auc.aus_id = att_aus_id OR auc.aus_id = att_aus_final_id )
                              AND d.jur_id=aud.aus_jurisdiction_id
                              AND c.jur_id=auc.aus_jurisdiction_id ";
                 }
                 else{
                     $where.=") ";
                 }
                 break;
             case 'district':
                 $where.="    AND d.jur_id=".$_POST['jurisdiction']."
                             AND d.jur_id=aud.aus_jurisdiction_id
                             AND (aud.aus_id = att_aus_id OR aud.aus_id = att_aus_final_id ";
                 if(isset($_POST['subjurisdictions'])){
                     $where.="
                              OR auc.aus_id = att_aus_id OR auc.aus_id = att_aus_final_id )
                              AND c.jur_id=auc.aus_jurisdiction_id ";
                 }
                 else{
                     $where.=") ";
                 }
                 break;
             default:
                 $where.="    AND c.jur_id=".$_POST['jurisdiction']."
                             AND c.jur_id = auc.aus_jurisdiction_id
                             AND (auc.aus_id = att_aus_id OR auc.aus_id = att_aus_final_id)";

         }
         $order="ORDER BY ATT_TIME";

         $sql =" $params FROM admin_user_transaction, $searchFrom $where $order";;
     }*/


    if(!areErrors()){
        $rs=$dbh->doCachedQuery($sql,0);

        $cols_arr = array("Time","Transaction Type","Admin","Transaction Id","Amount","From","Available Credit","To","Available Credit ","Note");

        $val_fmt_arr = array (
            "Time"                          =>  'return $rec["att_time"];',
            "Note"                          =>  'return $rec["att_note"];',
            "Transaction Type"              =>  'return $rec["tty_description"];',
            "Admin"                         =>  'return $rec["aus_username"];',
            "Transaction Id"                =>  'return $rec["att_transaction_id"];',
            "From"                          =>  'return ($rec["first_jurisdiction"]);',
            "To"                            =>  'return $rec["second_jurisdiction"];',
            "Amount"                        =>  'return getInDollars($rec["att_amount"]);',
            "Available Credit"              =>  'return "Before:".(($rec["first_jurisdiction"]=="casino" ) ? " Unlimited" : getInDollars($rec["att_jur_avail_credit"]))."<br / >After:&nbsp;&nbsp;&nbsp;".(($rec["first_jurisdiction"]=="casino" ) ? " Unlimited" :getInDollars($rec["att_jur_avail_credit"] - $rec["att_amount"] )) ;',
            "Available Credit "             =>  'return "Before:".(($rec["second_jurisdiction"]=="casino" ) ? " Unlimited" : getInDollars($rec["att_jur_final_avail_credit"]))."<br / >After:&nbsp;&nbsp;&nbsp;".(($rec["second_jurisdiction"]=="casino" ) ? " Unlimited" : getInDollars($rec["att_jur_final_avail_credit"] + $rec["att_amount"] )) ;'
        );

        $cell_fmt_arr = array ();
        $numrow = $rs->getNumRows();
$endtime = microtime(true);
$duration = $endtime - $starttime;
$duration=number_format($duration, 4, ',', '')."s";
?>
<div class="tip"><?=$lang->getLang("Time took to execute your request")?>:<?=$duration?></div>
       <? if($numrow!=0){
            $qp = new QueryPrinter($rs);
            $qp->printTable($rs, $cols_arr, "", $val_fmt_arr, $cell_fmt_arr, 0, 0,"",$width="100%", 1, 2000, false);
        }else{
            addError("", $lang->getLang("No result found"));
            showErrors();
        }

    }
    else{
        showErrors();
    }
}
function searchusertransaction() {
    global $dbh, $field, $value, $from, $to, $typetransaction, $operator,$lang;

    $result = array();
    $searchType = $_POST["field"];
    $sql = "SELECT
            pun_id,
            pun_username,
            ctr_id,
            ctr_pun_id,
            ctr_balance_available,
            ctr_time,
            ctr_amount,
            ctr_tbl_id,
            tty_description,
            tbl_id,
            tbl_name,
            aus_username,
            ctr_notes,
            ctr_tty_id,
            ctr_pme_code,                                                                                                   
            ctr_aus_id,
            ctr_transaction_num,
            tth_smallblind,
            tth_maxplayersnum
          FROM
            customer_transaction
            JOIN punter ON pun_id = ctr_pun_id
            JOIN transaction_type ON  tty_id = ctr_tty_id
            LEFT JOIN tables ON tbl_id = ctr_tbl_id
            LEFT JOIN tables_themes ON tbl_themeid = tth_id
            LEFT JOIN admin_user ON aus_id = ctr_aus_id
            
          ";
    $jur_id = $_SESSION["jurisdiction_id"];

    if($_SESSION["jurisdiction_class"] != "casino"){
        $sql .= "JOIN jurisdiction club     ON club.jur_id     = pun_betting_club
  		       JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
             JOIN jurisdiction regional ON regional.jur_id = district.jur_parent_id
             JOIN jurisdiction nation   ON nation.jur_id   = regional.jur_parent_id";
    }

    switch($_SESSION["jurisdiction_class"]){
        case "club":
            $sql .= " WHERE club.jur_id = $jur_id";
            break;
        case "district":
            $sql .= " WHERE district.jur_id = $jur_id";
            break;
        case "region":
            $sql .= " WHERE regional.jur_id = $jur_id";
            break;
        case "nation":
            $sql .= " WHERE nation.jur_id = $jur_id";
            break;
        case "casino":
            $sql .= " WHERE 1=1";
            break;
    }

    $value = $dbh->escape($value);

    if($searchType == 4){
        settype($value, "float");
        $sql .= " AND ctr_amount = $value";
    }

    if(empty($operator)){
        switch($searchType){
            case 1:
                //Username
                $sql .= " AND ctr_pun_id IN (SELECT pun_id FROM punter WHERE lower(pun_username) = lower('$value'))";
                break;
            case 2:
                //user id
                settype($value, "integer");
                $sql .= " AND pun_customer_number = $value";
                break;
            case 3:
                $ctr_id = $value;
                settype($ctr_id, "integer");
                //transaction
                $sql .= " AND (ctr_id = $ctr_id OR ctr_transaction_num = '$value')";
                break;
        }
    }
    elseif($operator == "ALL"){
        $operator = $dbh->escape($operator);
        $sql .= " AND ctr_aus_id IN (SELECT aus_id FROM admin_user JOIN admin_user_type ON aty_id = aus_aty_id AND (aty_code = 'SUPPOPERATOR' OR aty_code = 'SUPPMANAGER'))";
    }
    else{
        settype($operator, "integer");
        $sql .= " AND ctr_aus_id = $operator";
    }

    if($searchType != 3){
        $sql .= " AND ctr_time BETWEEN '$from' AND '$to'";
    }

    $sql .= " ORDER BY ctr_time, ctr_id ASC";
    $rs = $dbh->exec($sql);

    $cols_arr = array("TransactionID", "Username", "Description", "Admin User", "Detail", "Amount", "Balance", "Date", "Action");

    $val_fmt_arr = array (
        "TransactionID" =>'$ret = $rec["ctr_id"];
                       if(!empty($rec["ctr_transaction_num"])){
                         $ret .= " - " . $rec["ctr_transaction_num"];
                       }
                       return $ret;',
        "Username"      =>'return getClassLink(refPage("customers/customer_view&customer_id=" . $rec["pun_id"]), $rec["pun_username"]);',
        "Description"   =>'if($rec["ctr_amount"]< 0 && $rec["tty_description"]=="Table deposit"){
                         return "<span style=\'color:red;\'>" . $row["tty_description"]. " Cash IN coins</span>";
                       }elseif($rec["tty_description"]=="Table deposit"){
                         return $rec["tty_description"] . " Cash OUT coins";
                       }elseif($rec["ctr_pme_code"] == "BON"){
                         return "Bonus Refund";
                       }elseif($rec["ctr_pme_code"] == "PRV" && $rec["ctr_amount"] > 0){
                         return "Provider Deposit";
                       }elseif($rec["ctr_pme_code"] == "PRV" && $rec["ctr_amount"] <= 0){
                         return "Provider Withdrawal";
                       }else{
                         return $rec["tty_description"];
                       }',
        "Admin User"    =>'return $rec["aus_username"];',
        "Detail"        =>'if(!empty($rec["tbl_name"])){
                         $ret = $rec["tbl_id"] . " - " . $rec["tbl_name"] . " " . getInDollars($rec["tth_smallblind"], (($rec["tth_smallblind"] >= 1)?(0):(2))) . "/" . getInDollars($rec["tth_smallblind"] * 2, (($rec["tth_smallblind"] * 2 >= 1)?(0):(2))) . " " . $rec["tth_maxplayersnum"] . "p.";
                       }elseif(!empty($rec["trn_id"])){
                         $ret = "Tourney: " . $rec["trn_id"] . " - " . $rec["trn_name"];
                       }elseif(($rec["ctr_tty_id"] == 3 || $rec["ctr_tty_id"] == 10)){
                         $ret ="External Id: " . $rec["ptr_uniqueid"];
                       }
                       if(!empty($ret)){
                         $ret .= "<br/>";
                       }
                       if(!empty($rec["ctr_notes"]) && $_SESSION["jurisdiction_class"] == "casino"){
                         $ret .= "Note: " . $rec["ctr_notes"];
                       };
                       return $ret;
                       ',
        "Date"          =>'return ($rec["ctr_time"]);',
        "Amount"        =>'return getInDollars($rec["ctr_amount"]);',
        "Balance"       =>'return getInDollars($rec["ctr_balance_available"]);',
        "Action"        =>'if(!empty($rec["ctr_tbl_id"]) && $rec["ctr_amount"]< 0 && $rec["ctr_tty_id"] == 4){
                         return getClassLink("' . refPage('casino_games/searchuserhand') . '&userid=" . $rec["ctr_pun_id"] . "&tbl_id=" . $rec["ctr_tbl_id"] . "&from=" . $rec["ctr_time"] . "&to=" . date("Y-m-d H:i", (strtotime($rec["ctr_time"]) + 60 * 60 * 3)), "View Hands", "", "_blank");
                       }elseif(!empty($rec["trn_id"])){
                         return getClassLink(refPage("casino_games/tournaments") . "&action=watch&id=" . $rec["trn_id"], "View Tourney Details", "", "_blank");
                       }elseif(($rec["ctr_tty_id"] == 3 || $rec["ctr_tty_id"] == 10)){
                         return getClassLink(refPage("transactions/search_proc_transaction") . "&search=watch&form_submitted=yes&field=6&username=" . $rec["pun_username"] . "&trans_code=" . $rec["ctr_transaction_num"] . "&date_start=" . substr($rec["ctr_time"],0,10), "View Deposit Details", "", "_blank");
                       }else if(!empty($rec["ctr_res_id"])){
                         return getClassLink("' . refPage('casino_games/searchuserhand') . '&userid=" . $rec["ctr_pun_id"] . "&handid=" . $rec["res_game_num"] . "&from=" . $rec["ctr_time"] . "&to=" . date("Y-m-d H:i", (strtotime($rec["ctr_time"]) + 60 * 60 * 3)), "View Hands", "", "_blank");
                       }else{
                         return "<span style=\'color:#CCC;\'>No action available</span>";
                       };'
    );
    $cell_fmt_arr = array ();
    $numrow = $rs->getNumRows();
    if($numrow!=0){
        $qp = new QueryPrinter($rs);
        $qp->printTable($rs, $cols_arr, "", $val_fmt_arr, $cell_fmt_arr, 0, 0,"",$width="100%", 1, 2000, false);
    }else{
        addError("", $lang->getLang("No result found"));
        showErrors();
    }
}


function returnStatus($status,$amount){

    $strToRet="<span>".getInDollars($amount)."</span><br>";
    $strToRet.="<span ";
    if ($status=='A'){
        $strToRet.=' class="result">Accepted';
    }
    elseif ($status=='D'){
        $strToRet.=' class="error">Declined';
    }
    elseif ($status=='P')
    {
        $strToRet .='class ="pending">Pending';

    }
    return $strToRet;
}


function returnWith($aus='N',$username='N',$otheraff='N',$proc='N',$otherAvailableCredit,$otherAvailableOverdraft){

    $strToRet='';
    if($aus!='N'){
        $strToRet .=" Admin ".$aus."<br />
                          Available credit: ".getInDollars($otherAvailableCredit)."<br />
                          Available overdraft: ".getInDollars($otherAvailableOverdraft);
    }
    elseif($username!='N'){
        $strToRet .=" User ".$username."<br />
                      Available credit: ".getInDollars($otherAvailableCredit)."<br />";
    }
    elseif($otheraff!='N'){
        $strToRet .=" Subaffiliate ".$otheraff."<br />
                      Available credit: ".getInDollars($otherAvailableCredit)."<br />
                      Available overdraft: ".getInDollars($otherAvailableOverdraft);
    }
    else{
        $strToRet.=$proc;
    }


    return $strToRet;

}


function searchAffiliateTransaction(){
    global $dbh,$lang,$date_start, $date_end,$aff_id,$jur_id;
    $starttime = microtime(true);
    $sql="SELECT atr_time,
                         atr_amount,
                         atr_overdraft,
                         atr_available_credit,
                         atr_available_overdraft,
                         atr_other_available_credit,
                         atr_other_available_overdraft,
                         IF (atr_aff_id IS NOT NULL, (select pun_username FROM punter WHERE pun_aff_id = atr_aff_id AND pun_aus_id IS NOT NULL), 'N') atr_aff_username,
                         IF (atr_aus_id IS NOT NULL, (select aus_username FROM admin_user WHERE aus_id = atr_aus_id), 'N') atr_aus_username,
                         IF (atr_other_pun_id IS NOT NULL, (select pun_username FROM punter WHERE pun_id = atr_other_pun_id), 'N') atr_pun_username,
                         IF (atr_other_aff_id IS NOT NULL, (select pun_username FROM punter WHERE pun_aff_id = atr_other_aff_id AND pun_aus_id IS NOT NULL), 'N') atr_other_aff_username,
                         coalesce(atr_notes, 'NONE') atr_notes,
                         atr_transaction_id,
                         IF (atr_tty_id IS NOT NULL, (select tty_description from transaction_type where atr_tty_id = tty_id), 'N') tty_description,
			             IF (atr_ptr_id IS NOT NULL, (select ptr_ppc_code FROM processor_transaction WHERE atr_ptr_id = ptr_id), 'N') ptr_ppc_code,
			             'A' status
                        FROM affiliate_transaction
                        WHERE atr_time BETWEEN  '$date_start 00:00:00' AND '$date_end 23:59:59'
                        AND (atr_aff_id = ".$dbh->escape($aff_id)." OR atr_other_aff_id = ".$dbh->escape($aff_id).")";
    $sql.=" UNION ALL
                    SELECT ptr_start atr_time,
                            ptr_amount atr_amount,
                            0 atr_overdraft,
                            ptr_available_credit atr_available_credit,
                            ptr_available_overdraft atr_available_overdraft,
                            0 atr_other_available_credit,
                            0 atr_other_available_overdraft,
                            IF (ptr_pun_id IS NOT NULL, (select pun_username FROM punter WHERE pun_id = ptr_pun_id), 'N') atr_aff_username,
                            'N' atr_aus_username, 'N' atr_pun_username, 'N' atr_other_aff_username, coalesce(ptr_note, 'NONE') atr_notes, ptr_uniqueid atr_transaction_id,
                            IF (ptr_tty_id IS NOT NULL, (select tty_description from transaction_type where tty_id = ptr_tty_id), 'N') tty_description, ptr_ppc_code, ptr_status status
                            FROM processor_transaction, punter
                        WHERE ptr_start BETWEEN  '$date_start 00:00:00' AND '$date_end 23:59:59'
                        AND pun_aff_id = ".$dbh->escape($aff_id)."
                        AND pun_aus_id IS NOT NULL
                        AND ptr_pun_id = pun_id
                        AND (ptr_status = 'D' OR ptr_status = 'P')
                        order by atr_time DESC ";

    if(!areErrors()){
        $rs=$dbh->doCachedQuery($sql,0);

        $cols_arr = array("Date","Operation","Amount","Overdraft","Available Credit","Available Overdraft","With","Note","Transaction Id");

        $val_fmt_arr = array (
            "Date"                          =>  'return $rec["atr_time"];',
            "Operation"                     =>  'return $rec["tty_description"];',
            "Amount"                        =>  'return returnStatus($rec["status"],$rec["atr_amount"]);',
            "Overdraft"                     =>  'return getInDollars($rec["atr_overdraft"]);',
            "Available Credit"              =>  'return getInDollars($rec["atr_available_credit"]);',
            "Available Overdraft"           =>  'return getInDollars($rec["atr_available_overdraft"]);',
            "With"                          =>  'return returnWith($rec["atr_aus_username"],$rec["atr_pun_username"],$rec["atr_other_aff_username"],$rec["ptr_ppc_code"],$rec["atr_other_available_credit"],$rec["atr_other_available_overdraft"]);',
            "Note"                          =>  'return ($rec["atr_notes"]=="" ? "--" : $rec["atr_notes"]);',
            "Transaction Id"                =>  'return $rec["atr_transaction_id"];'
        );

        $cell_fmt_arr = array ();
        $numrow = $rs->getNumRows();
        $endtime = microtime(true);
        $duration = $endtime - $starttime;
        $duration=number_format($duration, 4, ',', '')."s";
        ?>
        <br /><div class="tip"><?=$lang->getLang("Time took to execute your request")?>:<?=$duration?></div>
        <? if($numrow!=0){
            $post_vars = array('jur_id'=>$jur_id, 'date_start'=>$date_start,'date_end'=>$date_end,'doQuery'=>$_POST['doQuery'] );
            $qp = new QueryPrinter($rs);
            $qp->printTable($rs, $cols_arr, $post_vars, $val_fmt_arr, $cell_fmt_arr, 0, 0,"",$width="100%", $start_row = ((isset ( $_POST ['start_row'] )) ? $_POST ['start_row'] : 1), $_POST['max_rows'], false);
        }else{
            addError("", $lang->getLang("No result found"));
            showErrors();
        }

    }
    else{
        showErrors();
    }
}




?>
<style>
    .ui-autocomplete-loading {
        background: white url('/media/images/ui-anim_basic_16x16.gif') right center no-repeat;
    }
</style>
<script>
    <?if( $adminTranSearch ){?>
    $(function(){

            $("#value").autocomplete({
                source: function( request, response ) {
                    $.ajax({
                        url: "services/jurisdiction_services.inc",
                        dataType: 'jsonp',
                        data: {
                            action: 0,
                            admlk: request.term
                        },
                        success: function( data ) {
                            response( $.map( data, function( item ) {
                                return {
                                    label: item.user+" ("+item.jurisdiction+")" ,
                                    value: item.user
                                }
                            }));
                        },
                        error:function(jqXHR, textStatus, errorThrown) {
                            alert(textStatus);
                            alert(errorThrown);
                        }
                    });
                },
                minLength: 2,

                open: function() {
                    $( this ).removeClass( "ui-corner-all" ).addClass( "ui-corner-top" );
                },
                close: function() {
                    $( this ).removeClass( "ui-corner-top" ).addClass( "ui-corner-all" );
                }
            });



        $('#searchType').change(function() {
            $('.hidden').hide();
            $('#' + $(this).find('option:selected').val()).show();

        });
        $('#field').change(function()
        {
            if($(this).find('option:selected').val()=='jurisdiction'){
                $('#jurisdictionSelector,#subjurisdictions').show();
                $("#valueContainer").hide();
            }
            else{
                $('#jurisdictionSelector,#subjurisdictions').hide();
                $("#valueContainer").show();
            }
            if($(this).find('option:selected').val()!='name'){
                $("#value").autocomplete( "disable" );
            }
            else{
                $("#value").autocomplete( "enable" );
            }

        });


        <?if($_POST['field']!='jurisdiction'){?>
        $('#jurisdictionSelector,#subjurisdictions').hide();
        <?
        }
        else{
        ?>
        $('#valueContainer').hide();
        <?
        }
        if(isset($_POST['resultDiv'])){?>
        $('#<?=$_POST['resultDiv']?>').show();
        $("#searchType").val('<?=$_POST['resultDiv']?>');
        <? }?>
    });
    <? } ?>
</script>


<?if ($userTranSearch || $adminTranSearch){ ?>

    
    <br /><div class=bodyHD><?=$lang->getLang("Transaction Search")?></div>
    <div class="fleft">

        <?if ($adminTranSearch && $userTranSearch && $affiliateTranSearch){ ?>
            <select  name='searchType' id="searchType" >
                <?if($searchType=="0" ||$searchType=="1") { ?>
                <option value='userSearch'>User Transactions</option>
                <? } if($searchType=="0" ||$searchType=="2") { ?>
                <option value='adminSearch'>Admin Transactions</option>
                <? }
                if($searchType=="0" ||$searchType=="3") { ?>
                <option value='adminSearch'>Affiliate Transactions</option>
                <? } ?>
            </select>
        <? } elseif($userTranSearch){
            ?>
            <select  name='searchType' id="searchType" >
                <option value='userSearch'>User Transactions</option>
            </select>
        <?
        }



        elseif($adminTranSearch){
            ?>
            <select  name='searchType' id="searchType" >
                <option value='adminSearch'>Admin Transactions</option>
            </select>
        <? }

        ?>
    </div><br /><br />

    <?if ($userTranSearch){ ?>

        <br /><div class="fleft  hidden"  <? if(!isset($_POST['field'])) { ?> style="display: block"<? } ?>  id="userSearch">
            <div class=bodyHD><?=$lang->getLang("User Transaction Search")?></div>
            <?form_head(refFormPage($_GET["page"]) . "&action=show",$name="search", "POST");?>
            <input type="hidden" name='resultDiv' value="userSearch">
            <input type="hidden" name='srctype' value="<?=$searchType?>" />
            <table cellpadding=4 cellspacing=1 border=0>
                <tr>
                    <td class="label"><?=$lang->getLang("Search by")?></td>
                    <td class="content">
                        <select name="field">
                            <?php
                            $opt_arr = array(
                                0 => array("name" => "User Name",      "value" => 1),
                                1 => array("name" => "User Id",        "value" => 2),
                                2 => array("name" => "Transaction Id", "value" => 3)
                            );

                            if($_SESSION["aty_code"] == "SUPPMANAGER" || $_SESSION["aty_code"] == "SUPERUSER"){
                                $opt_arr[3] = array("name" => "Amount", "value" => 4);
                            }

                            foreach ($opt_arr as $opt){
                                printOption("field", $opt["value"], $opt["name"], ($opt["value"] == $field));
                            }
                            ?>
                        </select>
                    </td>
                </tr>
                <tr>
                    <td class="label"><?=$lang->getLang("Value")?></td>
                    <td class="content"><input type="text" name="value" value="<?=$value?>"/></td>
                </tr>
                <?php if($_SESSION["aty_code"] == "SUPPMANAGER" || $_SESSION["aty_code"] == "SUPERUSER") :?>
                    <?php
                    $sql = "SELECT aus_id, aus_username FROM admin_user JOIN admin_user_type ON aty_id = aus_aty_id AND (aty_code = 'SUPPOPERATOR' OR aty_code = 'SUPPMANAGER') AND aus_access='ALLOW' ORDER BY aus_username";
                    $rs = $dbh->exec($sql);
                    ?>
                    <?php if($rs->hasNext()) : ?>
                        <tr>
                            <td class="label"><?=$lang->getLang("Operator")?></td>
                            <td class="content">
                                <select name="operator">
                                    <?php
                                    printOption("operator", "", "None");
                                    printOption("operator", "ALL", "ALL", $operator == "ALL");
                                    while ($rs->hasNext()){
                                        $row = $rs->next();
                                        printOption("operator", $row["aus_id"], " - " . $row["aus_username"], $operator == $row["aus_id"]);
                                    }
                                    ?>
                                </select>
                            </td>
                        </tr>
                    <?php endif; ?>
                <?php endif; ?>
                <tr>
                    <td class="label"><?=$lang->getLang("From")?></td>
                    <td class="content"> <input type='text' name='from' id='from'  value="<?=$from?>" ></td>
                </tr>
                <tr>
                    <td class="label"><?=$lang->getLang("Until")?></td>
                    <td class="content"><input type='text' name='to' id='to' value="<?=$to?>"></td>
                </tr>
                <tr>
                    <td></td>
                    <td class="content">
                        <input type="hidden" name="action" value="searchtrans"/>
                        <input type="submit" value="<?=$lang->getLang("Search")?>"/>
                    </td>
                </tr>
            </table>
            </form>
        </div>
    <? } ?>
    <?if ($adminTranSearch) { ?>
        <div class="fleft <? if($userTranSearch){?>  hidden <? } ?>" id="adminSearch">
            <div class=bodyHD><?=$lang->getLang("Admin Transaction Search")?></div>
            <?form_head(refFormPage($_GET["page"]) . "&action=show",$name="search", "POST");?>
            <input type="hidden" name='resultDiv' value="adminSearch">
            <input type="hidden" name='srctype' value="<?=$searchType?>" />
            <table cellpadding=4 cellspacing=1 border=0>
                <tr>
                    <td class="label"><?=$lang->getLang("Search by")?></td>
                    <td class="content">
                        <select name="field" id='field'>
                            <option <? $selected = ($_POST['field']=='name')?  "selected" :"" ?><?=$selected?>  value="name">Admin Name</option>
                            <option <? $selected = ($_POST['field']=='transaction')?  "selected" :"" ?><?=$selected?> value='transaction'>Transaction Id</option>
                            <option <? $selected = ($_POST['field']=='jurisdiction')?  "selected" :"" ?><?=$selected?> value="jurisdiction">Jurisdiction</option>
                        </select>
                    </td>
                </tr>
                <tr id="valueContainer">
                    <td class="label"><?=$lang->getLang("Value")?></td>
                    <td class="content"><input type="text" name="value" id="value" value="<?=$value?>"/></td>
                </tr>
                <?php
                $hide_option_one='hide';
                include('jurisdiction_filters.php');
                include('jurisdiction_filter_tabrow.php');
                ?>
                <tr>
                    <td class="label"><?=$lang->getLang("From")?></td>
                    <td class="content"> <input type='text' name='from' id='adminFrom'  value="<?=$from?>" ></td>
                </tr>
                <tr>
                    <td class="label"><?=$lang->getLang("Until")?></td>
                    <td class="content"><input type='text' name='to' id='adminTo' value="<?=$to?>"></td>
                </tr>
                <tr>
                    <td></td>
                    <td class="content">
                        <input type="hidden" name="action" value="searchtrans"/>
                        <input type="submit" value="<?=$lang->getLang("Search")?>"/>
                    </td>
                </tr>
            </table>
            </form>
        </div>
    <? }
    if($action=="searchtrans" && $_POST['resultDiv']=='userSearch'){

        searchusertransaction();
    }

    if($action=="searchtrans" && $_POST['resultDiv']=='adminSearch'){
        searchAdminTransaction();
    }

}
else {
    ?>
    <div class="error">You're not allowed to see this page</div>
<? } ?>


