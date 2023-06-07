<?php
include_once ('jurisdiction_filters.php');
check_access("transactions_search", true);
$adminTranSearch = check_access("admin_transaction_search");
$userTranSearch = check_access("user_transaction_search");
$userProviderTranSearch = check_access("user_transaction_search");
$affiliateTranSearch = check_access("affiliate_transaction_search");
$partnerWrongTransSeach = check_access("partner_wrong_transaction_search");
$sType = (isset($_REQUEST['srctype']) ? $_REQUEST['srctype'] : "0");
$defaultmonth = (isset($_POST['monthHelper'])) ? $_POST['monthHelper'] : false;
$defaultweek = (isset($_POST['weekHelper'])) ? $_POST['weekHelper'] : false;
$from = (isset($_POST['from'])) ? $_POST['from'] : date("Y-m-d H:i", time());
$to = (isset($_POST['to'])) ? $_POST['to'] : date("Y-m-d H:i", time());
$includeTotals = true;
$jurisdiction = (isset($_POST['jurisdiction'])) ? $_POST['jurisdiction'] : 0;
globalise('action');
globalise('field');
globalise('value');
globalise('from');
globalise('typetransaction');
globalise('operator');
globalise('jurisdiction_level');
globalise('jurisdiction');

if ($userProviderTranSearch) {
    define("SEARCH_MAX_HOURS", (24 * 40) + 1);
} else {
    if ($_POST["field"] == 7) {
        define("SEARCH_MAX_HOURS", 24 * 7);
    } else {
        define("SEARCH_MAX_HOURS", (24 * 31) + 1);
    }
} // end if
if (! isset($from)) {
    $from = date("Y-m-d H:i", strtotime("now - 2 hours"));
}
if (! isset($to)) {
    $to = date("Y-m-d H:i", strtotime("now"));
}
if (isset($_POST['jurisdiction_level'])) {
    $jurisdiction_level = $_POST['jurisdiction_level'];
}

?>
<link rel="stylesheet" type="text/css"
	href="<?= secure_host ?>/media/jquery.multiselect.css"
	title="core_style">
<script src="<?= secure_host ?>/media/jquery.multiselect.js"
	type="text/javascript"></script>
<script src="<?= secure_host ?>/media/ui/js/jquery.timepicker.js"
	type="text/javascript"></script>

<script type="text/javascript">
        $(function() {
            <? if($partnerWrongTransSeach) { ?>
             $("#partners").multiselect({
                multiple: true,
                noneSelectedText:'<?=$lang->getLang("Select a partner")?>',
                selectedList: 3
            }).multiselectfilter();
            <? }?>

            $("select[name='field']").on('change',function(){

                if($(this).val()==7){
                   $("input[name='value']").keydown(function (e) {
                       // Allow: backspace, delete, tab, escape, enter and .
                       if ($.inArray(e.keyCode, [46, 8, 9, 27, 13, 110, 190]) !== -1 ||
                               // Allow: Ctrl+A, Command+A
                           (e.keyCode == 65 && ( e.ctrlKey === true || e.metaKey === true ) ) ||
                               // Allow: home, end, left, right, down, up
                           (e.keyCode >= 35 && e.keyCode <= 40)) {
                           // let it happen, don't do anything
                           return;
                       }
                       // Ensure that it is a number and stop the keypress
                       if ((e.shiftKey || (e.keyCode < 48 || e.keyCode > 57)) && (e.keyCode < 96 || e.keyCode > 105)) {
                           e.preventDefault();
                       }
                   }).prop('type','number');
                    $('#partnerValue').show();
               }
                else if($(this).val()==0){
                    $("input[name='value']").unbind('keydown').prop('type','text');
                    $('#partnerValue').hide();
                }
                else{
                    $("input[name='value']").unbind('keydown').prop('type','text');
                    $('#partnerValue').show();
                }
            });
            <?if(isset($_POST['resultDiv'])){?>
            $('#<?=$_POST['resultDiv']?>').show();
            $("#searchType").val('<?=$_POST['resultDiv']?>');

            <? }?>
            $( "input[name='from']" ).datetimepicker({
                changeMonth: true,
                numberOfMonths:1,
                onSelect: function (selectedDate) {
                    $("input[name='to']").datepicker("option", "minDate", selectedDate);
                    dateMax = $("input[name='from']").datepicker("getDate");
                    var rangeMax = new Date(dateMax.getFullYear(), dateMax.getMonth()+1 , dateMax.getDate());
                    $("input[name='to']").datepicker("option", "maxDate", rangeMax);
                }
            });
            $( "input[name='to']" ).datetimepicker({
                changeMonth: true,
                numberOfMonths:1,
                hour: 23,
                minute: 59
            });
            <? if ($affiliateTranSearch) { ?>
            $("#jurisdiction").multiselect({
                multiple: false,
                header: "Select an affiliate",
                noneSelectedText: "Select an Affiliate",
                selectedList: 1
            }).multiselectfilter();
            <? } ?>
            $('#searchType').change(function() {
                $('.hidden').hide();
                $('select[name="field"]').trigger('change');
                $('#' + $(this).find('option:selected').val()).show();
            });
        });

        $(document).on('change', "[name='monthHelper']", function() {
            var d = new Date($(this).val());
            createMonthHelper(d);
        });
        $(document).on('change', "[name='weekHelper']", function() {
            createWeekHelper($(this).val());
        });

        function createWeekHelper(dateobj){
            dates=dateobj.split('~');
            firstDay=dates[0];
            lastDay=dates[1];
            $( "input[name='from']" ).datetimepicker('setDate', firstDay);
            $( "input[name='to']" ).datetimepicker('setDate', lastDay);
            $('.monthHelper').val('');
        }

        function createMonthHelper(dateobj){
            var firstDay = new Date(dateobj.getFullYear(), dateobj.getMonth(), 1);
            $("input[name='from']" ).datetimepicker('setDate', firstDay);
            var lastDay = new Date(dateobj.getFullYear(), dateobj.getMonth() + 1, 0,23,59);
            $("input[name='to']" ).datetimepicker('setDate', lastDay);
            $('.weekHelper').val('');
        }
    </script>
<?php
// created by Marian

if ((strtotime($to) - strtotime($from)) / 60 / 60 > SEARCH_MAX_HOURS) {
    addError("", $lang->getLang("You have selected an interval too long"));
}

function searchAdminTransaction()
{
    $starttime = microtime(true);
    global $dbh, $lang, $from, $to, $value;
    $searchType = $_POST["field"];
    $all = 0;
    $in = "0";
    if (isset($_POST['transferTransactions'])) {
        $in .= ",19,20";
    }
    if (isset($_POST['overdraftTransactions'])) {
        $in .= ",21,22";
    }
    if (isset($_POST['gameTransactions'])) {
        $in .= ",7,8";
    }
    if (isset($_POST['commissionTransactions'])) {
        $in .= ",23,24";
    }
    if (isset($_POST['cardTransactions'])) {
        $in .= ",3,10 ";
    }

    if (! isset($_POST['cardTransactions']) && ! isset($_POST['transferTransactions']) && ! isset($_POST['overdraftTransactions']) && ! isset($_POST['gameTransactions']) && ! isset($_POST['commissionTransactions'])) {
        $all = 1;
    }

    if (empty($value) && $searchType != "jurisdiction") {
        addError("", $lang->getLang("Please enter a value to search"));
        showErrors();
    } else {

        $sql = "Select att_id,
                att_time,
                att_note,
                att_transaction_id,
                IF (att_tty_id IS NOT NULL, (select tty_description from transaction_type where tty_id = att_tty_id), 'N') tty_description,
                IF (att_aus_id IS NOT NULL, (select aus_username from admin_user where aus_id = att_aus_id), 'N') aus_username,
                IF (att_aus_final_id IS NOT NULL, (select aus_username from admin_user where aus_id = att_aus_final_id), 'N') aus_final_username,
                IF (att_pun_id IS NOT NULL, (select pun_username from punter where pun_id = att_pun_id), 'N') pun_username,
                IF (att_jur_id IS NOT NULL, (select jur_name from jurisdiction where jur_id=att_jur_id), 'N') first_jurisdiction,
                IF (att_final_jur_id IS NOT NULL, (select jur_name from jurisdiction where jur_id=att_final_jur_id), 'N') second_jurisdiction,
                att_amount,
                att_overdraft,
                att_aus_id,
                att_jur_avail_credit,
                att_jur_final_avail_credit,
                att_jur_avail_overdraft,
                att_jur_final_avail_overdraft,
                cur_code_for_web
                FROM admin_user_transaction,admin_user,jurisdiction,currency
                WHERE  1=1
                AND jur_id=aus_jurisdiction_id
                AND cur_id=jur_currency
                AND att_aus_id=aus_id ";
        if (isset($_POST['transferTransactions']) || isset($_POST['cardTransactions']) || isset($_POST['overdraftTransactions']) || isset($_POST['gameTransactions']) || isset($_POST['commissionTransactions'])) {
            $sql .= " AND (att_tty_id in ($in)) ";
        }

        if ($searchType == "name") {
            // Username
            $sql .= " AND  aus_username=" . $dbh->prepareString($value);
        }
        if ($searchType == "transaction") {
            // user id
            $sql .= " AND att_transaction_id=" . $dbh->prepareString($value);
        }
        if ($searchType == "jurisdiction") {
            if ($_POST['jurisdiction'] == '') {
                addError('', 'Please select a jurisdiction');
            }
            $sql .= " AND (att_jur_id=" . $_POST['jurisdiction'] . "
                OR att_final_jur_id=" . $_POST['jurisdiction'] . ")";
        }
        if ($searchType != 3) {
            $sql .= " AND att_time BETWEEN '$from' AND '$to:59'";
        }

        if ($_SESSION['jurisdiction_class'] != 'casino') {
            $sql .= " AND aus_id IN  (select aus_id from admin_user aus2,jurisdiction c ";

            if ($_SESSION['jurisdiction_class'] == 'nation') {
                $sql .= ",jurisdiction n,jurisdiction r,jurisdiction d WHERE  c.jur_parent_id=d.jur_id
                        AND d.jur_parent_id=r.jur_id
                        AND r.jur_parent_id=n.jur_id
                        And n.jur_id=" . $_SESSION['jurisdiction_id'] . " AND aus2.aus_id = aus_id
                        AND (aus2.aus_jurisdiction_id = n.jur_id OR aus2.aus_jurisdiction_id = r.jur_id OR aus2.aus_jurisdiction_id = d.jur_id  OR aus2.aus_jurisdiction_id = c.jur_id)) ";
            }
            if ($_SESSION['jurisdiction_class'] == 'region') {
                $sql .= ",jurisdiction r,jurisdiction d WHERE c.jur_parent_id=d.jur_id
                           AND d.jur_parent_id=r.jur_id
                           AND r.jur_id=" . $_SESSION['jurisdiction_id'] . " AND aus2.aus_id = aus_id
                           AND (aus2.aus_jurisdiction_id = r.jur_id OR aus2.aus_jurisdiction_id = d.jur_id  OR aus2.aus_jurisdiction_id = c.jur_id)) ";
            }
            if ($_SESSION['jurisdiction_class'] == 'district') {
                $sql .= ",jurisdiction d WHERE c.jur_parent_id=d.jur_id
                        AND d.jur_id=" . $_SESSION['jurisdiction_id'] . " AND aus2.aus_id = aus_id
                        AND (aus2.aus_jurisdiction_id = d.jur_id  OR aus2.aus_jurisdiction_id = c.jur_id)) ";
            }

            if ($_SESSION['jurisdiction_class'] == 'club') {
                $sql .= " WHERE c.jur_id=" . $_SESSION['jurisdiction_id'] . " AND aus2.aus_id = aus_id
                        AND (aus2.aus_jurisdiction_id = c.jur_id)) ";
            }
        }

        if (isset($_POST['cardTransactions']) || $all == 1) {
            $sql .= " UNION ALL
                    SELECT ptr_id att_id,
                    ptr_start att_time,
                    ptr_note att_note,
                    ptr_uniqueid att_transaction_id,
                    (select tty_description from transaction_type where tty_id = ptr_tty_id) tty_description,
                    IF (ptr_admin_user_id IS NOT NULL, (select aus_username from admin_user where aus_id = ptr_admin_user_id), 'N') aus_username,
                    'N' aus_final_username,
                    'N' pun_username,
                    'N' first_jurisdiction,
                    'N' second_jurisdiction,
                    ptr_amount att_amount,
                    0 att_overdraft,
                    ptr_admin_user_id att_aus_id,
                    ptr_available_credit att_jur_avail_credit,
                    0 att_jur_final_avail_credit,
                    ptr_available_overdraft att_jur_avail_overdraft,
                    0 att_jur_final_avail_overdraft,
                    '' cur_code_for_web
                    FROM processor_transaction, admin_user  WHERE  1=1
                    AND (ptr_status ='P')
                    AND ptr_admin_user_id = aus_id ";
            if (isset($_POST['cardTransactions'])) {
                $sql .= " AND (ptr_tty_id= 3 OR ptr_tty_id=10 ) ";
            }

            if ($searchType == "name") {
                $sql .= " AND aus_username=" . $dbh->prepareString($value);
            }
            if ($searchType == "transaction") {
                $sql .= " AND ptr_uniqueid=" . $dbh->prepareString($value);
            }
            if ($searchType == "jurisdiction") {
                if ($_POST['jurisdiction'] == '') {
                    addError('', 'Please select a jurisdiction');
                }
                $sql .= " AND ptr_admin_user_id  IN (SELECT aus_id FROM admin_user where aus_jurisdiction_id=" . $dbh->prepareString($_POST['jurisdiction']) . ")";
            }
            if ($searchType != 3) {
                $sql .= " AND ptr_start >= '$from' AND ptr_end <= '$to:59'";
            }

            if ($_SESSION['jurisdiction_class'] != 'casino') {
                $sql .= " AND aus_id IN  (select aus_id from admin_user aus2,jurisdiction c ";

                if ($_SESSION['jurisdiction_class'] == 'nation') {
                    $sql .= ",jurisdiction n,jurisdiction r,jurisdiction d WHERE  c.jur_parent_id=d.jur_id
                          AND d.jur_parent_id=r.jur_id
                          AND r.jur_parent_id=n.jur_id
                          And n.jur_id=" . $_SESSION['jurisdiction_id'] . " AND aus2.aus_id = aus_id
                          AND (aus2.aus_jurisdiction_id = n.jur_id OR aus2.aus_jurisdiction_id = r.jur_id OR aus2.aus_jurisdiction_id = d.jur_id  OR aus2.aus_jurisdiction_id = c.jur_id)) ";
                }
                if ($_SESSION['jurisdiction_class'] == 'region') {
                    $sql .= ",jurisdiction r,jurisdiction d WHERE c.jur_parent_id=d.jur_id
                        AND d.jur_parent_id=r.jur_id
                        AND r.jur_id=" . $_SESSION['jurisdiction_id'] . " AND aus2.aus_id = aus_id
                        AND (aus2.aus_jurisdiction_id = r.jur_id OR aus2.aus_jurisdiction_id = d.jur_id  OR aus2.aus_jurisdiction_id = c.jur_id)) ";
                }
                if ($_SESSION['jurisdiction_class'] == 'district') {
                    $sql .= ",jurisdiction d WHERE c.jur_parent_id=d.jur_id
                        AND d.jur_id=" . $_SESSION['jurisdiction_id'] . " AND aus2.aus_id = aus_id
                        AND (aus2.aus_jurisdiction_id = d.jur_id  OR aus2.aus_jurisdiction_id = c.jur_id)) ";
                }

                if ($_SESSION['jurisdiction_class'] == 'club') {
                    $sql .= "  WHERE c.jur_id=" . $_SESSION['jurisdiction_id'] . " AND aus2.aus_id = aus_id
                        AND (aus2.aus_jurisdiction_id = c.jur_id)) ";
                }
            }
        }

        $sql .= " ORDER BY att_time";
        if (! areErrors()) {
            $rs = $dbh->doCachedQuery($sql, 0);

            $cols_arr = array(
                "Time",
                "Transaction Type",
                "Admin",
                "Transaction Id",
                "Amount",
                "Overdraft",
                "From",
                "Available Credit",
                "Available Overdraft",
                "To",
                "Available Credit ",
                "Available Overdraft ",
                "Note"
            );

            $val_fmt_arr = array(
                "Time" => 'return $rec["att_time"];',
                "Note" => 'return $rec["att_note"];',
                "Transaction Type" => 'return $rec["tty_description"];',
                "Admin" => 'return getClassLink("admin_users/admin_user_edit.php?id={$rec["att_aus_id"]}", $rec["aus_username"]);',
                "Transaction Id" => 'return $rec["att_transaction_id"];',
                "From" => 'return ($rec["first_jurisdiction"]);',
                "To" => 'return $rec["second_jurisdiction"];',
                "Amount" => 'return getInDollars($rec["att_amount"],2,$rec["cur_code_for_web"]);',
                "Overdraft" => 'return getInDollars($rec["att_overdraft"],2,$rec["cur_code_for_web"]);',
                "Available Credit" => 'return "Before:".(($rec["first_jurisdiction"]=="casino" ) ? " Unlimited" : getInDollars($rec["att_jur_avail_credit"],2,$rec["cur_code_for_web"]))."<br / >After:&nbsp;&nbsp;&nbsp;".(($rec["first_jurisdiction"]=="casino" ) ? " Unlimited" :getInDollars($rec["att_jur_avail_credit"] - $rec["att_amount"],2,$rec["cur_code_for_web"] )) ;',
                "Available Overdraft" => 'return "Before:".(($rec["first_jurisdiction"]=="casino" ) ? " Unlimited" : getInDollars($rec["att_jur_avail_overdraft"],2,$rec["cur_code_for_web"]))."<br / >After:&nbsp;&nbsp;&nbsp;".(($rec["first_jurisdiction"]=="casino" ) ? " Unlimited" :getInDollars($rec["att_jur_avail_overdraft"] - $rec["att_overdraft"],2,$rec["cur_code_for_web"] )) ;',
                "Available Credit " => 'return "Before:".(($rec["second_jurisdiction"]=="casino" ) ? " Unlimited" : getInDollars($rec["att_jur_final_avail_credit"],2,$rec["cur_code_for_web"]))."<br / >After:&nbsp;&nbsp;&nbsp;".(($rec["second_jurisdiction"]=="casino" ) ? " Unlimited" : getInDollars($rec["att_jur_final_avail_credit"] + $rec["att_amount"],2,$rec["cur_code_for_web"] )) ;',
                "Available Overdraft " => 'return "Before:".(($rec["second_jurisdiction"]=="casino" ) ? " Unlimited" : getInDollars($rec["att_jur_final_avail_overdraft"],2,$rec["cur_code_for_web"]))."<br / >After:&nbsp;&nbsp;&nbsp;".(($rec["second_jurisdiction"]=="casino" ) ? " Unlimited" : getInDollars($rec["att_jur_final_avail_overdraft"] + $rec["att_overdraft"] ,2,$rec["cur_code_for_web"] )) ;'
            );

            $cell_fmt_arr = array();
            $numrow = $rs->getNumRows();
            $endtime = microtime(true);
            $duration = $endtime - $starttime;
            $duration = number_format($duration, 4, ',', '') . "s";
            ?>
<div class="tip"><?=$lang->getLang("Time taken to execute your request")?>:<?=$duration?></div>
<?

            if ($numrow != 0) {
                $qp = new QueryPrinter($rs);
                $qp->printTable($rs, $cols_arr, "", $val_fmt_arr, $cell_fmt_arr, 0, 0, "", $width = "100%", 1, 2000, false);
            } else {
                addError("", $lang->getLang("No result found"));
                showErrors();
            }
        } else {
            showErrors();
        }
    }
}

function searchusertransaction()
{
    global $dbh, $field, $value, $from, $to, $typetransaction, $operator, $lang;
    $searchType = $_POST["field"];
    $includeTotals = $_POST['includeTotals'];
    if (empty($value)) {
        addError("", $lang->getLang("Please enter a value to search"));
        showErrors();
    } elseif (($searchType == 7 || $searchType == 2) && ! is_numeric($value)) {
        addError("", $lang->getLang("Please enter a numeric value"));
        showErrors();
    } else {
        $starttime = microtime(true);
        $result = array();

        $all = 0;
        $in = "0";
        if (isset($_POST['transferTransactions'])) {
            $in .= ",19,20";
        }
        if (isset($_POST['overdraftTransactions'])) {
            $in .= ",21,22";
        }
        if (isset($_POST['gameTransactions'])) {
            $in .= ",7,8";
        }
        if (isset($_POST['commissionTransactions'])) {
            $in .= ",23,24";
        }
        if (isset($_POST['cardTransactions'])) {
            $in .= ",3,10 ";
        }

        if (! isset($_POST['cardTransactions']) && ! isset($_POST['transferTransactions']) && ! isset($_POST['overdraftTransactions']) && ! isset($_POST['gameTransactions']) && ! isset($_POST['commissionTransactions'])) {
            $all = 1;
        }

        $jurisdiction_query_join = "";

        $jur_id = $_SESSION["jurisdiction_id"];

        if ($_SESSION["jurisdiction_class"] != "casino") {
            $jurisdiction_query_join .= " JOIN jurisdiction club     ON club.jur_id     = pun_betting_club
  		            JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
                    JOIN jurisdiction regional ON regional.jur_id = district.jur_parent_id
                    JOIN jurisdiction nation   ON nation.jur_id   = regional.jur_parent_id ";
        }

        switch ($_SESSION["jurisdiction_class"]) {
            case "club":
                $jurisdiction_query_join .= " WHERE club.jur_id = $jur_id ";
                break;
            case "district":
                $jurisdiction_query_join .= " WHERE district.jur_id = $jur_id ";
                break;
            case "region":
                $jurisdiction_query_join .= " WHERE regional.jur_id = $jur_id ";
                break;
            case "nation":
                $jurisdiction_query_join .= " WHERE nation.jur_id = $jur_id ";
                break;
            case "casino":
                break;
        }

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
            aus_username,
            aus_id,
            ctr_notes,
            ctr_tty_id,
            ctr_pme_code,
            ctr_aus_id,
            ctr_transaction_num,
            ctr_bonus_available,
            ctr_amount_converted,
            ccs_conversion_rate,
            curr1.cur_code_for_web cur_code_for_web,
            curr2.cur_name converted_currency,
            curr2.cur_code_for_web converted_currency_sym
          FROM
            customer_transaction
            JOIN punter ON pun_id = ctr_pun_id
            JOIN transaction_type ON  tty_id = ctr_tty_id
            LEFT JOIN admin_user ON aus_id = ctr_aus_id
            LEFT JOIN currencies_conversion_session on ccs_id=ctr_ccs_id
            LEFT join jurisdiction on pun_betting_club =jur_id
            LEFT join currency curr1 on  curr1.cur_id=jur_currency
            LEFT join currency curr2 on  ccs_cur_id_to=curr2.cur_id

          ";

        if ($searchType == 7) {
            $sql .= " JOIN partner_users on pnu_local_id=pun_id";
        }

        $sql .= $jurisdiction_query_join;
        $sql .= " WHERE 1=1 ";
        $value = $dbh->escape($value);

        if ($searchType == 4) {
            settype($value, "float");
            $sql .= " AND ctr_amount = $value";
        } elseif ($searchType == 7) {
            $sql .= " AND pnu_id =" . $dbh->escape($value);
        } else {
            if (empty($operator)) {
                switch ($searchType) {
                    case 1:
                        // Username
                        $sql .= " AND pun_username =" . $dbh->prepareString($value);
                        break;
                    case 2:
                        // user id
                        settype($value, "integer");
                        $sql .= " AND pun_customer_number = $value";
                        break;
                    case 3:
                        $ctr_id = $value;
                        settype($ctr_id, "integer");
                        // transaction
                        $sql .= " AND ( ctr_transaction_num = '$value')";
                        break;
                }
            } elseif ($operator == "ALL") {
                $operator = $dbh->escape($operator);
                $sql .= " AND ctr_aus_id IN (SELECT aus_id FROM admin_user JOIN admin_user_type ON aty_id = aus_aty_id AND (aty_code = 'SUPPOPERATOR' OR aty_code = 'SUPPMANAGER'))";
            } else {
                settype($operator, "integer");
                $sql .= " AND ctr_aus_id = $operator";
            }
        }

        if ($searchType != 3) {
            $sql .= " AND ctr_time BETWEEN '$from' AND '$to:59'";
        }
        if (isset($_POST['transferTransactions']) || isset($_POST['cardTransactions']) || isset($_POST['overdraftTransactions']) || isset($_POST['gameTransactions']) || isset($_POST['commissionTransactions'])) {
            $sql .= " AND (ctr_tty_id in ($in)) ";
        }

        if (isset($_POST['cardTransactions']) || $all == 1) {
            $sql .= " UNION ALL
                        SELECT
                        ptr_pun_id pun_id,
                        pun_username,
                        ptr_id ctr_id,
                        ptr_pun_id ctr_pun_id,
                        ptr_available_credit ctr_balance_available,
                        ptr_start ctr_time,
                        ptr_amount ctr_amount,
                        'N' ctr_tbl_id,
                        (Select tty_description from transaction_type where tty_id=ptr_tty_id) tty_description,
                        IF (ptr_aus_id IS NOT NULL, (select aus_username from admin_user where aus_id=ptr_aus_id),null) aus_username,
                        ptr_aus_id aus_id,
                        ptr_note ctr_notes,
                        ptr_tty_id  ctr_tty_id,
                        'CSH' ctr_pme_code,
                        ptr_aus_id ctr_aus_id,
                        ptr_uniqueid ctr_transaction_num,
                        'N' ctr_bonus_available,
                        '' ctr_amount_converted,
                        '' ccs_conversion_rate,
                        '' cur_code_for_web,
                        '' converted_currency,
                        '' converted_currency_sym
                        FROM processor_transaction
                        JOIN punter ON ptr_pun_id = pun_id ";

            if ($searchType == 7) {
                $sql .= "JOIN partner_users ON pnu_local_id=pun_id AND pnu_id =" . $dbh->escape($value);
            }
            $sql .= $jurisdiction_query_join;
            $sql .= " WHERE ptr_status = 'P' ";
            if (isset($_POST['cardTransactions'])) {
                $sql .= " AND (ptr_tty_id= 3 OR ptr_tty_id=10 ) ";
            }

            if ($searchType == 4) {
                settype($value, "float");
                $sql .= " AND ptr_amount = $value";
            } else {
                if (empty($operator)) {
                    switch ($searchType) {
                        case 1:
                            // Username
                            $sql .= " AND pun_username  = " . $dbh->prepareString($value);
                            break;
                        case 2:
                            // user id
                            settype($value, "integer");
                            $sql .= " AND pun_customer_number = $value";
                            break;
                        case 3:
                            $ctr_id = $value;
                            settype($ctr_id, "integer");
                            // transaction
                            $sql .= " AND ( ptr_uniqueid = '$value')";
                            break;
                    }
                } elseif ($operator == "ALL") {
                    $operator = $dbh->escape($operator);
                    $sql .= " AND ptr_aus_id IN (SELECT aus_id FROM admin_user JOIN admin_user_type ON aty_id = aus_aty_id AND (aty_code = 'SUPPOPERATOR' OR aty_code = 'SUPPMANAGER'))";
                } else {
                    settype($operator, "integer");
                    $sql .= " AND ptr_aus_id = $operator";
                }
            }
            if ($searchType != 3) {
                $sql .= " AND ptr_start BETWEEN '$from' AND '$to:59'";
            }
        }
        if (isset($_POST['pesPendingTransactions']) || $all == 1) {
            $sql .= " UNION ALL
                        SELECT
                        tpg_pun_id pun_id,
                        pun_username,
                        tpg_id ctr_id,
                        tpg_pun_id ctr_pun_id,
                        tpg_balance_available ctr_balance_available,
                        tpg_creation ctr_time,
                        tpg_amount ctr_amount,
                        'N' ctr_tbl_id,
                        if (tpg_type = 1, 'Pending Deposit', 'Pending Withdraw') tty_description,
                        IF (tpg_aus_id IS NOT NULL, (select aus_username from admin_user where aus_id=tpg_aus_id),null) aus_username,
                        tpg_aus_id aus_id,
                        tpg_note ctr_notes,
                        if (tpg_type = 1, 7, 8)  ctr_tty_id,
                        'CSH' ctr_pme_code,
                        tpg_aus_id ctr_aus_id,
                        tpg_ctr_transaction_num ctr_transaction_num,
                        'N' ctr_bonus_available,
                        '' ctr_amount_converted,
                        '' ccs_conversion_rate,
                        '' cur_code_for_web,
                        '' converted_currency,
                        '' converted_currency_sym
                        FROM transaction_pending
                        JOIN punter ON tpg_pun_id = pun_id AND tpg_status = 1 ";
            
            if ($searchType == 7) {
                $sql .= "JOIN partner_users ON pnu_local_id=pun_id AND pnu_id =" . $dbh->escape($value);
            }
            $sql .= $jurisdiction_query_join;
            $sql .= " WHERE 1 = 1 ";
            if (isset($_POST['cardTransactions'])) {
                $sql .= " AND (ptr_tty_id= 3 OR ptr_tty_id=10 ) ";
            }
            
            if ($searchType == 4) {
                settype($value, "float");
                $sql .= " AND tpg_amount = $value";
            } else {
                if (empty($operator)) {
                    switch ($searchType) {
                        case 1:
                            // Username
                            $sql .= " AND pun_username  = " . $dbh->prepareString($value);
                            break;
                        case 2:
                            // user id
                            settype($value, "integer");
                            $sql .= " AND pun_customer_number = $value";
                            break;
                        case 3:
                            $ctr_id = $value;
                            settype($ctr_id, "integer");
                            // transaction
                            $sql .= " AND ( tpg_ctr_transaction_num = '$value')";
                            break;
                    }
                } elseif ($operator == "ALL") {
                    $operator = $dbh->escape($operator);
                    $sql .= " AND tpg_aus_id IN (SELECT aus_id FROM admin_user JOIN admin_user_type ON aty_id = aus_aty_id AND (aty_code = 'SUPPOPERATOR' OR aty_code = 'SUPPMANAGER'))";
                } else {
                    settype($operator, "integer");
                    $sql .= " AND tpg_aus_id = $operator";
                }
            }
            if ($searchType != 3) {
                $sql .= " AND tpg_creation BETWEEN '$from' AND '$to:59'";
            }
        }
        
        $sql .= " ORDER BY  ctr_time ASC";
        $rs = $dbh->exec($sql);
        $cashIn = 0;
        $cashOut = 0;
        if ($includeTotals) {
            while ($rs->hasNext()) {
                $row = $rs->next();
                if ($row['ctr_amount'] < 0) {
                    $cashOut += $row['ctr_amount'];
                } else {
                    $cashIn += $row['ctr_amount'];
                }
            }
        }

        /*
         * $sum_fmt_arr = array(
         * 'Tot Deposit'=>'return getInDollars($totals["Bets"]+0);',
         * 'Tot Withdraw'=>'return getInDollars($totals["Payouts"]+0);',
         * 'Diff'=>'return getInDollars($totals["Total Casino Revenue"]+0);'
         * );
         */

        $cols_arr = array(
            "TransactionID",
            "Username",
            "Description",
            "Admin User",
            "Detail",
            "Amount",
            "Amount Converted",
            "Conversion Rate",
            "Balance",
            "Bonus",
            "Date",
            "Action"
        );

        $val_fmt_arr = array(
            "TransactionID" => '$ret = $rec["ctr_id"];
                       if(!empty($rec["ctr_transaction_num"])){
                         $ret .= " - " . $rec["ctr_transaction_num"];
                       }
                       return $ret;',
            "Username" => 'return getClassLink(refPage("customers/customer_view&customer_id=" . $rec["pun_id"]), $rec["pun_username"]);',
            "Description" => 'if($rec["ctr_amount"]< 0 && $rec["tty_description"]=="Table deposit"){
                         return "<span style=\'color:red;\'>" . $row["tty_description"]. " Cash IN coins</span>";
                       }elseif($rec["tty_description"]=="Table deposit"){
                         return $rec["tty_description"] . " Cash OUT coins";
                       }elseif($rec["ctr_pme_code"] == "BON"){
                         return "Bonus Refund";
                       }elseif($rec["ctr_pme_code"] == "PRV" && $rec["ctr_amount"] > 0){
                         return "Provider Deposit";
                       }elseif($rec["ctr_pme_code"] == "PRV" && $rec["ctr_amount"] <= 0){
                         return "Provider Withdrawal";
                       }elseif ($rec["tty_description"] == "Pending Deposit" || $rec["tty_description"] == "Pending Withdraw") {
                         return "<span style=\'color:#FFA500;\'>" . $rec["tty_description"]. "</span>";
                       }else{
                         return $rec["tty_description"];
                       }',
            "Admin User" => 'return $rec["aus_username"];',
            "Detail" => 'if(!empty($rec["tbl_name"])){
                         $ret = $rec["tbl_id"] . " - " . $rec["tbl_name"] . " " . getInDollars($rec["tth_smallblind"], (($rec["tth_smallblind"] >= 1)?(0):(2))) . "/" . getInDollars($rec["tth_smallblind"] * 2, (($rec["tth_smallblind"] * 2 >= 1)?(0):(2))) . " " . $rec["tth_maxplayersnum"] . "p.";
                       }elseif(!empty($rec["trn_id"])){
                         $ret = "Tourney: " . $rec["trn_id"] . " - " . $rec["trn_name"];
                       }elseif(($rec["ctr_tty_id"] == 3 || $rec["ctr_tty_id"] == 10)){
                         $ret ="External Id: " . $rec["ctr_transaction_num"];
                       }
                       if(!empty($ret)){
                         $ret .= "<br/>";
                       }
                       if(!empty($rec["ctr_notes"])){
                         $ret .= "Note: " . $rec["ctr_notes"];
                       };
                       return $ret;
                       ',
            "Date" => 'return ($rec["ctr_time"]);',
            "Amount" => 'return getInDollars($rec["ctr_amount"],2,$rec["cur_code_for_web"]);',
            "Amount Converted" => 'return ($rec["ctr_amount_converted"] !=""  ? getInDollars($rec["ctr_amount_converted"],2,$rec["converted_currency_sym"]) ." ".$rec["converted_currency"]: "");',
            "Conversion Rate" => 'return $rec["ccs_conversion_rate"];',
            "Balance" => 'return getInDollars($rec["ctr_balance_available"],2,$rec["cur_code_for_web"]);',
            "Bonus" => 'return getInDollars($rec["ctr_bonus_available"]);',
            "Action" => 'if(!empty($rec["ctr_tbl_id"]) && $rec["ctr_amount"]< 0 && $rec["ctr_tty_id"] == 4){
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

        $cell_fmt_arr = array();
        $numrow = $rs->getNumRows();
        $endtime = microtime(true);
        $duration = $endtime - $starttime;
        $duration = number_format($duration, 4, ',', '') . "s";

        ?>
<div class="clearfix"></div>
<br />
<br />
<div class="tip"><?=$lang->getLang("Time taken to execute your request")?>:<?=$duration?></div>
<? if($includeTotals) { ?>
<table class="fleft table table-bordered">
	<tr>
		<td class="label"
			style="font-size: 15px; padding: 7px; border-radius: 3px">Total
			Deposit</td>
		<td class="label"
			style="padding: 7px; font-size: 15px; border-radius: 3px">Total
			Withdraw</td>
		<td class="label centered"
			style="padding: 7px; border-radius: 3px; font-size: 15px">Diff</td>
	</tr>
	<tr>
		<td style="font-size: 15px"><?=getInDollars($cashIn)?></td>
		<td style="font-size: 15px"><?=getInDollars($cashOut)?></td>
		<td style="font-size: 15px"><?=getInDollars($cashIn+$cashOut)?></td>
	</tr>
</table>
<? } ?>
        <?
        if ($numrow != 0) {
            $qp = new QueryPrinter($rs);
            $qp->printTable($rs, $cols_arr, "", $val_fmt_arr, $cell_fmt_arr, 0, 0, "", $width = "100%", 1, 2000, false);
        } else {
            addError("", $lang->getLang("No result found"));
            showErrors();
        }
    }
}

function searchProviderUserTransaction()
{
    global $dbh, $field, $value, $from, $to, $typetransaction, $operator, $lang, $jurisdiction_level, $jurisdiction;
    $searchType = $_POST["field"];
    $includeTotals = $_POST['includeTotals'];
    if (! isset($jurisdiction_level)) {
        $jurisdiction_level = $_SESSION["jurisdiction_class"];
    }
    if (! isset($jurisdiction)) {
        $jurisdiction = $_SESSION["jurisdiction_id"];
        if ($jurisdiction == 1) {
            $jurisdiction_level = 'casino';
        } // end if
    }

    // echo ' Jurisdiction: ' . $jurisdiction_level . '<br/>';
    // echo ' Jurisdiction: ' . $jurisdiction . "<br/>";

    $jur_id = $jurisdiction;

    if (($searchType == 7 || $searchType == 2) && ! is_numeric($value)) {
        addError("", $lang->getLang("Please enter a numeric value"));
        showErrors();
    } else {
        $starttime = microtime(true);
        $result = array();

        $value = $dbh->escape($value);
        $sqlUserId = "";
        $sqlTransactionId = " 1=1 ";
        if ($searchType == 7) {
            $sqlUserId .= " AND pnu_id =" . $dbh->escape($value);
        }

        if (empty($operator)) {
            switch ($searchType) {
                case 1:
                    // Username
                    $sqlUserId .= " AND pun_username =" . $dbh->prepareString($value);
                    break;
                case 2:
                    // user id
                    settype($value, "integer");
                    $sqlUserId .= " AND pun_customer_number = $value";
                    break;
                case 3:
                    $ctr_id = $value;
                    settype($ctr_id, "integer");
                    // transaction
                    $sqlTransactionId .= " AND ctr_transaction_num = '$value'";
                    break;
            }
        }

        if ($searchType != 3) {
            $sqlTransactionId .= " AND ctr_time BETWEEN '$from' AND '$to:59'";
        }
        $sql = "SELECT * FROM
                (SELECT 
                    pun_id,
                    pun_username,
                    ctr_id,
                    ctr_pun_id,
                    ctr_balance_available,
                    ctr_time,
                    ctr_amount,
                    IF(ctr_tty_id = 7,
                        'Customer Deposit',
                        'Customer Withdrawal') tty_description,
                    ctr_notes,
                    ctr_tty_id,
                    ctr_transaction_num,
                    ctr_bonus_available,
                    ctr_amount_converted,
                    ccs_conversion_rate,
                    curr1.cur_code_for_web cur_code_for_web,
                    curr2.cur_name converted_currency,
                    curr2.cur_code_for_web converted_currency_sym,
                    club.jur_name club_name,
                    club.jur_id club_id,
                    district.jur_name district_name,
                    district.jur_id district_id,
                    regional.jur_name regional_name,
                    regional.jur_id regional_id,
                    nation.jur_name nation_name,
                    nation.jur_id nation_id
                FROM (SELECT * FROM customer_transaction JOIN
                    (SELECT pes_id FROM providers WHERE pes_game_code LIKE 'CA%') t ON ctr_pesid = t.pes_id 
                    WHERE " . $sqlTransactionId . ") t2        
                JOIN punter ON pun_id = ctr_pun_id " . $sqlUserId . "
                JOIN jurisdiction club ON club.jur_id = pun_betting_club
                JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
                JOIN jurisdiction regional ON regional.jur_id = district.jur_parent_id
                JOIN jurisdiction nation ON nation.jur_id = regional.jur_parent_id
                LEFT JOIN currencies_conversion_session ON ccs_id = ctr_ccs_id
                LEFT JOIN currency curr1 ON curr1.cur_id = club.jur_currency
                LEFT JOIN currency curr2 ON ccs_cur_id_to = curr2.cur_id) t3 ";

        // echo $sql;
        // exit;

        switch ($jurisdiction_level) {
            case "club":
                $sql .= " WHERE club_id = $jur_id";
                break;
            case "district":
                $sql .= " WHERE district_id = $jur_id";
                break;
            case "region":
                $sql .= " WHERE regional_id = $jur_id";
                break;
            case "nation":
                $sql .= " WHERE nation_id = $jur_id";
                break;
            case "casino":
                $sql .= " WHERE 1=1";
                break;
        }

        $sql .= " ORDER BY  ctr_time ASC";

        // echo $sql;
        $rs = $dbh->exec($sql);
        // echo $sql;
        $cashIn = 0;
        $cashOut = 0;
        if ($includeTotals) {
            while ($rs->hasNext()) {
                $row = $rs->next();
                if ($row['ctr_amount'] < 0) {
                    if ($row['ctr_amount_converted'] > 0) {
                        $cashOut += $row['ctr_amount_converted'];
                    } else {
                        $cashOut += $row['ctr_amount'];
                    }
                } else {
                    if ($row['ctr_amount_converted'] > 0) {
                        $cashIn += $row['ctr_amount_converted'];
                    } else {
                        $cashIn += $row['ctr_amount'];
                    }
                }
            }
        }

        $cols_arr = array(
            "TransactionID",
            "Club",
            "Username",
            "Description",
            "Detail",
            "Amount",
            "Amount Converted",
            "Conversion Rate",
            "Balance",
            "Bonus",
            "Date",
            "Action"
        );

        $val_fmt_arr = array(
            "TransactionID" => '$ret = $rec["ctr_id"];
                       if(!empty($rec["ctr_transaction_num"])){
                         $ret .= " - " . $rec["ctr_transaction_num"];
                       }
                       return $ret;',
            "Club" => 'return($rec["club_name"]);',
            "Username" => 'return getClassLink(refPage("customers/customer_view&customer_id=" . $rec["pun_id"]), $rec["pun_username"]);',
            "Description" => 'if($rec["ctr_amount"]< 0 && $rec["tty_description"]=="Table deposit"){
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
            "Detail" => 'if(!empty($rec["tbl_name"])){
                         $ret = $rec["tbl_id"] . " - " . $rec["tbl_name"] . " " . getInDollars($rec["tth_smallblind"], (($rec["tth_smallblind"] >= 1)?(0):(2))) . "/" . getInDollars($rec["tth_smallblind"] * 2, (($rec["tth_smallblind"] * 2 >= 1)?(0):(2))) . " " . $rec["tth_maxplayersnum"] . "p.";
                       }elseif(!empty($rec["trn_id"])){
                         $ret = "Tourney: " . $rec["trn_id"] . " - " . $rec["trn_name"];
                       }elseif(($rec["ctr_tty_id"] == 3 || $rec["ctr_tty_id"] == 10)){
                         $ret ="External Id: " . $rec["ctr_transaction_num"];
                       }
                       if(!empty($ret)){
                         $ret .= "<br/>";
                       }
                       if(!empty($rec["ctr_notes"])){
                         $ret .= "Note: " . $rec["ctr_notes"];
                       };
                       return $ret;
                       ',
            "Amount" => 'return getInDollars($rec["ctr_amount"],2,$rec["cur_code_for_web"]);',
            "Amount Converted" => 'return ($rec["ctr_amount_converted"] !=""  ? getInDollars($rec["ctr_amount_converted"],2,$rec["converted_currency_sym"]) ." ".$rec["converted_currency"]: "");',
            "Conversion Rate" => 'return $rec["ccs_conversion_rate"];',
            "Balance" => 'return getInDollars($rec["ctr_balance_available"],2,$rec["cur_code_for_web"]);',
            "Bonus" => 'return getInDollars($rec["ctr_bonus_available"]);',
            "Date" => 'return ($rec["ctr_time"]);',
            "Action" => 'if(!empty($rec["ctr_tbl_id"]) && $rec["ctr_amount"]< 0 && $rec["ctr_tty_id"] == 4){
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

        $cell_fmt_arr = array();
        $numrow = $rs->getNumRows();
        $endtime = microtime(true);
        $duration = $endtime - $starttime;
        $duration = number_format($duration, 4, ',', '') . "s";

        ?>
<div class="clearfix"></div>
<br />
<br />
<div class="tip"><?=$lang->getLang("Time taken to execute your request")?>:<?=$duration?></div>

<?
        if ($numrow != 0) {
            if ($includeTotals) {
                ?>
<table class="fleft table table-bordered">
	<tr>
		<td class="label"
			style="font-size: 15px; padding: 7px; border-radius: 3px">Total
			Deposit</td>
		<td class="label"
			style="padding: 7px; font-size: 15px; border-radius: 3px">Total
			Withdraw</td>
		<td class="label centered"
			style="padding: 7px; border-radius: 3px; font-size: 15px">Diff</td>
	</tr>
	<tr>
		<td style="font-size: 15px"><?=getInDollars($cashIn)?></td>
		<td style="font-size: 15px"><?=getInDollars($cashOut)?></td>
		<td style="font-size: 15px"><?=getInDollars($cashIn+$cashOut)?></td>
	</tr>
</table>
<?
            }
            $qp = new QueryPrinter($rs);
            $qp->printTable($rs, $cols_arr, "", $val_fmt_arr, $cell_fmt_arr, 0, 0, "", $width = "100%", 1, 2000, false);
        } else {
            addError("", $lang->getLang("No result found"));
            showErrors();
        }
    }
}

function returnStatus($status, $amount, $currency)
{
    $strToRet = "<span>" . getInDollars($amount, 2, $currency) . "</span><br>";
    $strToRet .= "<span ";
    if ($status == 'A') {
        $strToRet .= ' class="result">Accepted';
    } elseif ($status == 'D') {
        $strToRet .= ' class="error">Declined';
    } elseif ($status == 'P') {
        $strToRet .= 'class ="pending">Pending';
    }
    return $strToRet;
}

function returnWith($aus = 'N', $username = 'N', $otheraff = 'N', $proc = 'N', $otherAvailableCredit, $otherAvailableOverdraft, $currency)
{
    $strToRet = '';
    if ($aus != 'N') {
        $strToRet .= " Admin " . $aus . "<br />
                          Available credit: " . getInDollars($otherAvailableCredit, 2, $currency) . "<br />
                          Available overdraft: " . getInDollars($otherAvailableOverdraft, 2, $currency);
    } elseif ($username != 'N') {
        $strToRet .= " User " . $username . "<br />
                      Available credit: " . getInDollars($otherAvailableCredit, 2, $currency) . "<br />";
    } elseif ($otheraff != 'N') {
        $strToRet .= " Subaffiliate " . $otheraff . "<br />
                      Available credit: " . getInDollars($otherAvailableCredit, 2, $currency) . "<br />
                      Available overdraft: " . getInDollars($otherAvailableOverdraft, 2, $currency);
    } else {
        $strToRet .= $proc;
    }

    return $strToRet;
}

function getJurisdictionsAffiliates($jur_id)
{
    global $dbh;
    $jur_class = $_SESSION['jurisdiction_class'];
    $sql = "select c.jur_name club_name,c.jur_id club_id,d.jur_name district_name,aus_username,act_credits,act_overdraft,aff_id,act_overdraft_start_time
            FROM jurisdiction c
            JOIN jurisdiction d on d.jur_id = c.jur_parent_id
            JOIN admin_user on c.jur_id=aus_jurisdiction_id
            JOIN affiliate on aff_aus_id=aus_id
            JOIN affiliate_credit on act_aff_id=aff_id
    ";
    if ($jur_class == "club") {
        $sql .= " WHERE c.jur_id = " . $jur_id;
    } elseif ($jur_class == "district") {
        $sql .= "
				  WHERE d.jur_id = " . $jur_id;
    } elseif ($jur_class == "region") {
        $sql .= "
				  JOIN jurisdiction r on r.jur_id = d.jur_parent_id
				  WHERE r.jur_id=" . $jur_id;
    } elseif ($jur_class == "nation") {
        $sql .= " JOIN jurisdiction r on r.jur_id = d.jur_parent_id
				 JOIN jurisdiction n on n.jur_id = r.jur_parent_id
				WHERE n.jur_id=" . $jur_id;
    } else {
        $sql .= " WHERE 1=1";
    }
    $sql .= " AND c.jur_class='club'
            AND aff_is_club=1
    ";
    $result = $dbh->exec($sql, false, true);
    return $result;
}

function searchAffiliateTransaction()
{
    global $dbh, $lang, $from, $to;
    $aff_id = $dbh->escape($_POST['jurisdiction']);

    $all = 0;
    $in = "0";
    if (isset($_POST['transferTransactions'])) {
        $in .= ",19,20";
    }
    if (isset($_POST['overdraftTransactions'])) {
        $in .= ",21,22";
    }
    if (isset($_POST['gameTransactions'])) {
        $in .= ",7,8";
    }
    if (isset($_POST['commissionTransactions'])) {
        $in .= ",23,24";
    }
    if (isset($_POST['cardTransactions'])) {
        $in .= ",3,10 ";
    }

    if (! isset($_POST['cardTransactions']) && ! isset($_POST['transferTransactions']) && ! isset($_POST['overdraftTransactions']) && ! isset($_POST['gameTransactions']) && ! isset($_POST['commissionTransactions'])) {
        $all = 1;
    }

    if (! $aff_id || $aff_id == '') {
        addError("", $lang->getLang("Please select an affiliate"));
        showErrors();
    } else {
        $starttime = microtime(true);
        $sql = "SELECT        atr_id,
                        atr_time,
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
			             'A' status,
			             cur_code_for_web
                        FROM affiliate_transaction
                        JOIN affiliate on aff_id=atr_aff_id
                        JOIN admin_user on aus_id=aff_aus_id
                        JOIN jurisdiction on aus_jurisdiction_id=jur_id
                        JOIN currency on cur_id=jur_currency
                        WHERE atr_time BETWEEN  '$from:00' AND '$to:59'
                        AND (atr_aff_id = " . $dbh->escape($aff_id) . " OR atr_other_aff_id = " . $dbh->escape($aff_id) . ")";

        if (isset($_POST['transferTransactions']) || isset($_POST['cardTransactions']) || isset($_POST['overdraftTransactions']) || isset($_POST['gameTransactions']) || isset($_POST['commissionTransactions'])) {
            $sql .= " AND (atr_tty_id in ($in)) ";
        }

        if (isset($_POST['cardTransactions']) || $all == 1) {
            $sql .= " UNION ALL
                    SELECT ptr_id atr_id,
                      ptr_start atr_time,
                      ptr_amount atr_amount,
                      0 atr_overdraft,
                      ptr_available_credit atr_available_credit,
                      ptr_available_overdraft atr_available_overdraft,
                      0 atr_other_available_credit,
                      0 atr_other_available_overdraft,
                      IF (ptr_pun_id IS NOT NULL, (select pun_username FROM punter WHERE pun_id = ptr_pun_id), 'N') atr_aff_username,
                      'N' atr_aus_username, 'N' atr_pun_username, 'N' atr_other_aff_username, coalesce(ptr_note, 'NONE') atr_notes, ptr_uniqueid atr_transaction_id,
                      IF (ptr_tty_id IS NOT NULL, (select tty_description from transaction_type where tty_id = ptr_tty_id), 'N') tty_description, ptr_ppc_code, ptr_status status,
                      '' cur_code_for_web
                      FROM processor_transaction, punter,jurisdiction, currency
                      WHERE ptr_start BETWEEN  '$from:00' AND '$to:59'
                      AND pun_betting_club=jur_id
                      AND jur_currency=cur_id
                      AND pun_aff_id = " . $dbh->escape($aff_id) . "
                      AND pun_aus_id IS NOT NULL
                      AND ptr_pun_id = pun_id
                      AND (ptr_status = 'D' OR ptr_status = 'P')";

            if (isset($_POST['cardTransactions'])) {
                $sql .= " AND (ptr_tty_id= 3 OR ptr_tty_id=10 ) ";
            }

            $sql .= " order by atr_id,atr_time DESC ";
        }

        if (! areErrors()) {
            $rs = $dbh->doCachedQuery($sql, 0);

            $cols_arr = array(
                "Date",
                "Operation",
                "Amount",
                "Overdraft",
                "Available Credit",
                "Available Overdraft",
                "With",
                "Note",
                "Transaction Id"
            );

            $val_fmt_arr = array(
                "Date" => 'return $rec["atr_time"];',
                "Operation" => 'return $rec["tty_description"];',
                "Amount" => 'return returnStatus($rec["status"],$rec["atr_amount"],$rec["cur_code_for_web"]);',
                "Overdraft" => 'return getInDollars($rec["atr_overdraft"],2,$rec["cur_code_for_web"]);',
                "Available Credit" => 'return getInDollars($rec["atr_available_credit"],2,$rec["cur_code_for_web"]);',
                "Available Overdraft" => 'return getInDollars($rec["atr_available_overdraft"],2,$rec["cur_code_for_web"]);',
                "With" => 'return returnWith($rec["atr_aus_username"],$rec["atr_pun_username"],$rec["atr_other_aff_username"],$rec["ptr_ppc_code"],$rec["atr_other_available_credit"],$rec["atr_other_available_overdraft"],$rec["cur_code_for_web"]);',
                "Note" => 'return ($rec["atr_notes"]=="" ? "--" : $rec["atr_notes"]);',
                "Transaction Id" => 'return $rec["atr_transaction_id"];'
            );

            $cell_fmt_arr = array();
            $numrow = $rs->getNumRows();
            if ($numrow != 0) {
                $qp = new QueryPrinter($rs);
                $qp->printTable($rs, $cols_arr, "", $val_fmt_arr, $cell_fmt_arr, 0, 0, "", $width = "100%", 1, 2000, false);
            } else {
                addError("", $lang->getLang("No result found"));
                showErrors();
            }
        } else {
            showErrors();
        }
    }
}

function searchPartnersWrongTransaction()
{
    global $dbh, $field, $value, $from, $to, $typetransaction, $operator, $lang, $partners;

    $searchType = $_POST["field"];
    if (empty($value) && $searchType != 0) {
        addError("", $lang->getLang("Please enter a value to search"));
        showErrors();
    }
    if ($_POST['partners'] == '') {
        addError("", $lang->getLang("Please choose at least one partner"));
        showErrors();
    } elseif (($searchType == 7 || $searchType == 2) && ! is_numeric($value)) {
        addError("", $lang->getLang("Please enter a numeric value"));
        showErrors();
    } else {
        $starttime = microtime(true);
        $result = array();
        $sql = "SELECT
           transaction_recovered.*,pun_id,pun_username,ptn_name,
            curr1.cur_code_for_web cur_code_for_web
          FROM
            transaction_recovered
            JOIN partners on ttr_ptn_id=ptn_id
            JOIN punter ON ttr_pun_id = pun_id
            LEFT join jurisdiction on pun_betting_club =jur_id
            LEFT join currency curr1 on  curr1.cur_id=jur_currency
          ";
        $jur_id = $_SESSION["jurisdiction_id"];

        if ($_SESSION["jurisdiction_class"] != "casino") {
            $sql .= "JOIN jurisdiction club     ON club.jur_id     = pun_betting_club
  		            JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
                    JOIN jurisdiction regional ON regional.jur_id = district.jur_parent_id
                    JOIN jurisdiction nation   ON nation.jur_id   = regional.jur_parent_id";
        }

        switch ($_SESSION["jurisdiction_class"]) {
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

        $sql .= " AND ttr_ptn_id in(" . implode(",", $_POST['partners']) . ") ";

        if ($searchType == 4) {
            settype($value, "float");
            $sql .= " AND ttr_amount = " . $dbh->prepareString($value);
        } elseif ($searchType == 7) {
            $sql .= " AND ttr_ptn_id =" . $dbh->escape($value);
        } else {
            switch ($searchType) {
                case 1:
                    // Username
                    $sql .= " AND pun_username =" . $dbh->prepareString($value);
                    break;
                case 2:
                    // user id
                    settype($value, "integer");
                    $sql .= " AND pun_customer_number = $value";
                    break;
                case 3:
                    $ctr_id = $value;
                    settype($ctr_id, "integer");
                    // transaction
                    $sql .= " AND ( ttr_ctr_id = '$value')";
                    break;
            }
        }

        if ($searchType != 3) {
            $sql .= " AND ttr_insert_time BETWEEN '$from' AND '$to:59'";
        }

        $sql .= " ORDER BY  ttr_insert_time ASC";
        $rs = $dbh->exec($sql);

        $cols_arr = array(
            "TransactionID",
            "Username",
            "Partner",
            "Application",
            "Retry",
            "Detail",
            "Amount",
            "Insert Time",
            "Managing Time"
        );

        $val_fmt_arr = array(
            "TransactionID" => ' return $rec["ttr_ctr_id"];',
            "Username" => 'return getClassLink(refPage("customers/customer_view&customer_id=" . $rec["pun_id"]), $rec["pun_username"]);',
            "Partner" => 'return $rec["ptn_name"];',
            "Application" => 'return $rec["ttr_application_name"];',
            "Retry" => 'return $rec["ttr_retry"];',
            "Detail" => ' if(!empty($rec["ttr_note"])){
                                $ret = "Note: " . $rec["ttr_note"];
                                };
                                return $ret;',
            "Insert Time" => 'return ($rec["ttr_insert_time"]);',
            "Managing Time" => 'return ($rec["ttr_managing_time"]);',
            "Amount" => 'return getInDollars($rec["ttr_amount"],2,$rec["cur_code_for_web"]);'
        );
        $cell_fmt_arr = array();
        $numrow = $rs->getNumRows();
        $endtime = microtime(true);
        $duration = $endtime - $starttime;
        $duration = number_format($duration, 4, ',', '') . "s";
        ?>
<div class="clearfix"></div>
<br />
<br />
<div class="tip"><?=$lang->getLang("Time taken to execute your request")?>:<?=$duration?></div>
<?
        if ($numrow != 0) {
            $qp = new QueryPrinter($rs);
            $qp->printTable($rs, $cols_arr, "", $val_fmt_arr, $cell_fmt_arr, 0, 0, "", $width = "100%", 1, 2000, false);
        } else {
            addError("", $lang->getLang("No result found"));
            showErrors();
        }
    }
}
?>
<style>
        .ui-autocomplete-loading {
            background: white url('<?=image_dir?>/ui-anim_basic_16x16.gif') right center no-repeat;
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
            } else {
                ?>
            $('#valueContainer').hide();
            <?
            }
            ?>
        });
        <? } ?>


    </script>

<?php
// echo "Something";
?>




<?if ($userTranSearch || $adminTranSearch || $affiliateTranSearch || $partnerWrongTransSeach || $userProviderTranSearch){ ?>
<br />
<div class=bodyHD><?=$lang->getLang("Transaction Search")?></div>
<div class="fleft">
	<select name='searchType' id="searchType">
            <?if($userTranSearch &&($sType=="0" || $sType=="1")) { ?>
                <option value='userSearch'><?=$lang->getLang("User Transactions")?></option>
            <? } if($adminTranSearch && ($sType=="0" || $sType=="2")) { ?>
                <option value='adminSearch'><?=$lang->getLang("Admin Transactions")?></option>
            <?
    }
    if ($affiliateTranSearch && ($sType == "0" || $sType == "3")) {
        ?>
                <option value='affiliateSearch'><?=$lang->getLang("Affiliate Transactions")?></option>
            <? } if($partnerWrongTransSeach &&($sType=="0" || $sType=="1")) { ?>
                <option value='pwrongtransactionsSearch'><?=$lang->getLang("Partner wrong transactions")?></option>
            <? } if($userProviderTranSearch &&($sType=="0" || $sType=="4")) { ?>
                <option value='providertransaction'><?=$lang->getLang("Poker transactions")?></option>
            <? } ?>
        </select>
</div>
<div class="clearfix"></div>
<br />


<?if ($userTranSearch ){ ?>
<div class="fleft  hidden" <?if(!isset($_REQUEST['field'])) {  ?>
	style="display: block" <? } ?> id="userSearch">

	<a href="/?page=customers/withdraw_status"
		style="color: black; text-decoration: none" class="entitybtn"
		target="_blank"><?=$lang->getLang("Withdraw Status check")?> </a> <br />
	<br />
	<div class=bodyHD><?=$lang->getLang("User Transaction Search")?></div>
            <?form_head(refFormPage($_GET["page"]) . "&action=show",$name="search", "POST");?>
            <input type="hidden" name='resultDiv' value="userSearch"> <input
		type="hidden" name='srctype' value="<?=$sType?>" />
	<table cellpadding=4 cellspacing=1 border=0>
		<tr>
			<td class="label"><?=$lang->getLang("Search by")?></td>
			<td class="content"><select name="field">
                            <?php
        $opt_arr = array(
            0 => array(
                "name" => "User Name",
                "value" => 1
            ),
            1 => array(
                "name" => "User Id",
                "value" => 2
            ),
            2 => array(
                "name" => "Transaction Id",
                "value" => 3
            ),
            7 => array(
                "name" => "Partner User ID",
                "value" => 7
            )
        );

        /*
         * if($_SESSION["aty_code"] == "SUPPMANAGER" || $_SESSION["aty_code"] == "SUPERUSER"){
         * $opt_arr[3] = array("name" => "Amount", "value" => 4);
         * }
         */

        foreach ($opt_arr as $opt) {
            printOption("field", $opt["value"], $opt["name"], ($opt["value"] == $field));
        }
        ?>
                        </select></td>
		</tr>
		<tr>
			<td class="label"><?=$lang->getLang("Value")?></td>
			<td class="content"><input type="text" name="value"
				value="<?=$value?>" /></td>
		</tr>
                <?php if($_SESSION["aty_code"] == "SUPPMANAGER" || $_SESSION["aty_code"] == "SUPERUSER") :?>
                    <?php
            $sql = "SELECT aus_id, aus_username FROM admin_user JOIN admin_user_type ON aty_id = aus_aty_id AND (aty_code = 'SUPPOPERATOR' OR aty_code = 'SUPPMANAGER') AND aus_access='ALLOW' ORDER BY aus_username";
            $rs = $dbh->exec($sql);
            ?>
                    <?php if($rs->hasNext()) : ?>
                        <tr>
			<td class="label"><?=$lang->getLang("Operator")?></td>
			<td class="content"><select name="operator">
                                    <?php
                printOption("operator", "", "None");
                printOption("operator", "ALL", "ALL", $operator == "ALL");
                while ($rs->hasNext()) {
                    $row = $rs->next();
                    printOption("operator", $row["aus_id"], " - " . $row["aus_username"], $operator == $row["aus_id"]);
                }
                ?>
                                </select></td>
		</tr>
                    <?php endif; ?>
                <?php endif; ?>
                <tr valign=top>
			<td class=label><?=$lang->getLang("Type")?></td>
			<td class=content><label><input type="checkbox" class="filters"
					name='gameTransactions'
					<?=(isset($_POST['gameTransactions'])? 'checked': '')?> value='1'><?=$lang->getLang("Game transactions")?></label>
				<br /> <label><input type="checkbox" class="filters"
					name='cardTransactions'
					<?=(isset($_POST['cardTransactions'])? 'checked': '')?> value='1'> <?=$lang->getLang("Card transactions")?> </label>
				<br /> <label> <input type="checkbox" class="filters"
					name='transferTransactions'
					<?=(isset($_POST['transferTransactions'])? 'checked': '')?>
					value='1'> <?=$lang->getLang("Transfer transactions")?></label> 
				<br /><label><input type="checkbox" class="filters"
					name='overdraftTransactions'
					<?=(isset($_POST['overdraftTransactions'])? 'checked': '')?>
					value='1'> <?=$lang->getLang("Overdraft transactions")?></label> <br />
				<label><input type="checkbox" class="filters"
					name='commissionTransactions'
					<?=(isset($_POST['commissionTransactions'])? 'checked': '')?>
					value='1'> <?=$lang->getLang("Commission transactions")?></label><br />
			</td>
		</tr>
		<tr valign=top>
			<td class=label><?=$lang->getLang("Include Totals")?></td>
			<td class=content><label><input type="checkbox" class="includeTotals"
					name='includeTotals'
					<?=(isset($_POST['includeTotals'])? 'checked': '')?> value='1'>Include
					totals</label> <br /></td>
		</tr>

		<tr valign=top class="timeHelper">
			<td class=label><?=$lang->getLang("Choose month")?></td>
			<td class=content>
                        <?=month_select_box($defaultmonth)?>
                    </td>
		</tr>

		<tr valign=top class="timeHelper">
			<td class=label><?=$lang->getLang("Choose week")?></td>
			<td class=content>
                        <?=week_select_box($defaultweek)?>
                    </td>
		</tr>
		<tr class="timeHelper">
			<td class="label"><?=$lang->getLang("From")?></td>
			<td class="content"><input type='text' name='from' id='from'
				value="<?=$from?>"></td>
		</tr>
		<tr class="timeHelper">
			<td class="label"><?=$lang->getLang("Until")?></td>
			<td class="content"><input type='text' name='to' id='to'
				value="<?=$to?>"></td>
		</tr>
		<tr>
			<td></td>
			<td class="content"><input type="hidden" name="action"
				value="searchtrans" /> <input type="submit" class="entitybtn"
				value="<?=$lang->getLang("Search")?>" /></td>
		</tr>
	</table>
	</form>
</div>
<? } ?>

    <?if ($userProviderTranSearch ){ ?>
        <?php
        $selector_unique_id = "poker";
        ?>
<div class="fleft <? if($userTranSearch){?>  hidden <? } ?>"
	id="providertransaction">
	<div class=bodyHD><?=$lang->getLang("Poker Transaction Search")?></div>
            <?form_head(refFormPage($_GET["page"]) . "&action=show",$name="search", "POST");?>
            <input type="hidden" name='resultDiv'
		value="providertransaction"> <input type="hidden" name='srctype'
		value="<?=$sType?>" />
	<table cellpadding=4 cellspacing=1 border=0>
		<tr>
			<td class="label"><?=$lang->getLang("Search by")?></td>
			<td class="content"><select name="field">
                            <?php
        $opt_arr = array(
            0 => array(
                "name" => "None",
                "value" => 0
            ),
            1 => array(
                "name" => "User Name",
                "value" => 1
            ),
            2 => array(
                "name" => "User Id",
                "value" => 2
            ),
            3 => array(
                "name" => "Transaction Id",
                "value" => 3
            )
        );
        foreach ($opt_arr as $opt) {
            printOption("field", $opt["value"], $opt["name"], ($opt["value"] == $field));
        }
        ?>
                        </select></td>
		</tr>
		<tr>
			<td class="label"><?=$lang->getLang("Value")?></td>
			<td class="content"><input type="text" name="value"
				value="<?=$value?>" /></td>
		</tr>
		<!--                -->
		<?php
        //
        // // $hide_option_one='hide';
        // include('jurisdiction_filters.php');
        include ('jurisdiction_filter_tabrow.php');
        // ?>


                <tr valign=top>
			<td class=label><?=$lang->getLang("Include Totals")?></td>
			<td class=content><label><input type="checkbox" class="includeTotals"
					name='includeTotals'
					<?=(isset($_POST['includeTotals']) || (isset($includeTotals))? 'checked': '')?>
					value='1'>Include totals</label> <br /></td>
		</tr>

		<tr valign=top class="timeHelper">
			<td class=label><?=$lang->getLang("Choose month")?></td>
			<td class=content>
                        <?=month_select_box($defaultmonth)?>
                    </td>
		</tr>

		<tr valign=top class="timeHelper">
			<td class=label><?=$lang->getLang("Choose week")?></td>
			<td class=content>
                        <?=week_select_box($defaultweek)?>
                    </td>
		</tr>
		<tr class="timeHelper">
			<td class="label"><?=$lang->getLang("From")?></td>
			<td class="content"><input type='text' name='from' id='providerFrom'
				value="<?=$from?>"></td>
		</tr>
		<tr class="timeHelper">
			<td class="label"><?=$lang->getLang("Until")?></td>
			<td class="content"><input type='text' name='to' id='providerTo'
				value="<?=$to?>"></td>
		</tr>
		<tr>
			<td></td>
			<td class="content"><input type="hidden" name="action"
				value="searchtrans" /> <input type="submit" class="entitybtn"
				value="<?=$lang->getLang("Search")?>" /></td>
		</tr>
	</table>
	</form>
</div>
<? } ?>
    <?php

    if ($adminTranSearch) {
        $selector_unique_id = '';
        ?>

<div class="fleft <? if($userTranSearch){?>  hidden <? } ?>"
	id="adminSearch">
	<div class=bodyHD><?=$lang->getLang("Admin Transaction Search")?></div>
            <?form_head(refFormPage($_GET["page"]) . "&action=show",$name="search", "POST");?>
            <input type="hidden" name='resultDiv' value="adminSearch"> <input
		type="hidden" name='srctype' value="<?=$sType?>" />
	<table cellpadding=4 cellspacing=1 border=0>
		<tr>
			<td class="label"><?=$lang->getLang("Search by")?></td>
			<td class="content"><select name="field" id='field'>
					<option
						<? $selected = ($_POST['field']=='name')?  "selected" :"" ?>
						<?=$selected?> value="name"><?=$lang->getLang("Admin Name")?></option>
					<option
						<? $selected = ($_POST['field']=='transaction')?  "selected" :"" ?>
						<?=$selected?> value='transaction'><?=$lang->getLang("Transaction Id")?></option>
					<option
						<? $selected = ($_POST['field']=='jurisdiction')?  "selected" :"" ?>
						<?=$selected?> value="jurisdiction"><?=$lang->getLang("Jurisdiction")?></option>
			</select></td>
		</tr>
		<tr id="valueContainer">
			<td class="label"><?=$lang->getLang("Value")?></td>
			<td class="content"><input type="text" name="value" id="value"
				value="<?=$value?>" /></td>
		</tr>
                <?php
        // $hide_option_one='hide';

        include ('jurisdiction_filter_tabrow.php');
        ?>
                <tr valign=top>
			<td class=label><?=$lang->getLang("Type")?></td>
			<td class=content><label><input type="checkbox" class="filters"
					name='gameTransactions'
					<?=(isset($_POST['gameTransactions'])? 'checked': '')?> value='1'><?=$lang->getLang("Game transactions")?></label>
				<br /> <label><input type="checkbox" class="filters"
					name='cardTransactions'
					<?=(isset($_POST['cardTransactions'])? 'checked': '')?> value='1'> <?=$lang->getLang("Card transactions")?> </label>
				<br /> <label> <input type="checkbox" class="filters"
					name='transferTransactions'
					<?=(isset($_POST['transferTransactions'])? 'checked': '')?>
					value='1'> <?=$lang->getLang("Transfer transactions")?></label> <br />
				<label><input type="checkbox" class="filters"
					name='overdraftTransactions'
					<?=(isset($_POST['overdraftTransactions'])? 'checked': '')?>
					value='1'> <?=$lang->getLang("Overdraft transactions")?></label> <br />
				<label><input type="checkbox" class="filters"
					name='commissionTransactions'
					<?=(isset($_POST['commissionTransactions'])? 'checked': '')?>
					value='1'> <?=$lang->getLang("Commission transactions")?></label></td>
		</tr>

		<tr valign=top class="timeHelper">
			<td class=label><?=$lang->getLang("Choose month")?></td>
			<td class=content>
                        <?=month_select_box($defaultmonth)?>
                    </td>
		</tr>

		<tr valign=top class="timeHelper">
			<td class=label><?=$lang->getLang("Choose week")?></td>
			<td class=content>
                        <?=week_select_box($defaultweek)?>
                    </td>
		</tr>
		<tr class="timeHelper">
			<td class="label"><?=$lang->getLang("From")?></td>
			<td class="content"><input type='text' name='from' id='adminFrom'
				value="<?=$from?>"></td>
		</tr>
		<tr class="timeHelper">
			<td class="label"><?=$lang->getLang("Until")?></td>
			<td class="content"><input type='text' name='to' id='adminTo'
				value="<?=$to?>"></td>
		</tr>
		<tr>
			<td></td>
			<td class="content"><input type="hidden" name="action"
				value="searchtrans" /> <input type="submit" class="entitybtn"
				value="<?=$lang->getLang("Search")?>" /></td>
		</tr>
	</table>
	</form>
</div>
<?
    }
    if ($affiliateTranSearch) {
        ?>

<div class="fleft <? if($userTranSearch){?>  hidden <? } ?>"
	id="affiliateSearch">
	<div class=bodyHD><?=$lang->getLang("Affiliate Transaction Search")?></div>
            <?form_head(refFormPage($_GET["page"]) . "&action=show",$name="search", "POST");?>
            <input type="hidden" name='field' value='affiliate'> <input
		type="hidden" name='resultDiv' value="affiliateSearch"> <input
		type="hidden" name='srctype' value="<?=$sType?>" />
	<table cellpadding=4 cellspacing=1 border=0>
		<tr>
		
		
		<tr>
			<td class="label"><?=$lang->getLang("Select Affiliate")?></td>
			<td>
                        <?php
        $result = getJurisdictionsAffiliates($_SESSION['jurisdiction_id']);
        $clubs = array();
        while ($result->hasNext()) {
            $row = $result->next();
            if (! is_array($clubs[$row['district_name']])) {
                $clubs[$row['district_name']] = array();
            }
            $clubs[$row['district_name']][$row['aff_id']] = array(
                "club" => $row['club_name'],
                "club_id" => $row['club_id'],
                "username" => $row['aus_username'],
                "availableCredit" => $row['act_credits'],
                "overdraft" => $row['act_overdraft'],
                "overdraft_time" => $row['act_overdraft_start_time']
            );
        }
        ?>
                        <select name='jurisdiction' id='jurisdiction'>
                            <?
        foreach ($clubs as $k => $v) {
            ?>
                                <optgroup label="<?=$k?>">
                                    <?

            foreach ($v as $k2 => $v2) {
                ?>
                                        <option value='<?=$k2?>'
							<?if($_POST['jurisdiction']==$k2 || $_GET['aff']==$k2 ){?>
							selected="selected" <? } ?>><?=$v2['username']?></option>
                                    <?
            }
            ?>
                                </optgroup>
                            <?
        }
        ?>
                        </select>
			</td>
		</tr>
		<tr valign=top>
			<td class=label><?=$lang->getLang("Type")?></td>
			<td class=content><label><input type="checkbox" class="filters"
					name='gameTransactions'
					<?=(isset($_POST['gameTransactions'])? 'checked': '')?> value='1'><?=$lang->getLang("Game transactions")?></label>
				<br /> <label><input type="checkbox" class="filters"
					name='cardTransactions'
					<?=(isset($_POST['cardTransactions'])? 'checked': '')?> value='1'> <?=$lang->getLang("Card transactions")?> </label>
				<br /> <label> <input type="checkbox" class="filters"
					name='transferTransactions'
					<?=(isset($_POST['transferTransactions'])? 'checked': '')?>
					value='1'> <?=$lang->getLang("Transfer transactions")?></label>
				<br /><label><input type="checkbox" class="filters"
					name='overdraftTransactions'
					<?=(isset($_POST['overdraftTransactions'])? 'checked': '')?>
					value='1'> <?=$lang->getLang("Overdraft transactions")?></label> <br />
				<label><input type="checkbox" class="filters"
					name='commissionTransactions'
					<?=(isset($_POST['commissionTransactions'])? 'checked': '')?>
					value='1'> <?=$lang->getLang("Commission transactions")?></label></td>
		</tr>
		<tr valign=top>
			<td class=label><?=$lang->getLang("Choose month")?></td>
			<td class=content>
                        <?=month_select_box($defaultmonth)?>
                    </td>
		</tr>

		<tr valign=top>
			<td class=label><?=$lang->getLang("Choose week")?></td>
			<td class=content>
                        <?=week_select_box($defaultweek)?>
                    </td>
		</tr>
		<tr>
			<td class="label"><?=$lang->getLang("From")?></td>
			<td class="content"><input type='text' name='from' id='affiliateFrom'
				value="<?=$from?>"></td>
		</tr>
		<tr>
			<td class="label"><?=$lang->getLang("Until")?></td>
			<td class="content"><input type='text' name='to' id='affiliateTo'
				value="<?=$to?>"></td>
		</tr>
		<tr>
			<td></td>
			<td class="content"><input type="hidden" name="action"
				value="searchtrans" /> <input type="submit" class="entitybtn"
				value="<?=$lang->getLang("Search")?>" /></td>
		</tr>
	</table>
	</form>
</div>
<?
    }

    if ($partnerWrongTransSeach) {
        $partners = $_POST['partners'];

        ?>
<div class="fleft <? if($userTranSearch){?>  hidden <? } ?>"
	id="pwrongtransactionsSearch">
	<div class=bodyHD><?=$lang->getLang("Partner wrong transactions")?></div>
            <?form_head(refFormPage($_GET["page"]) . "&action=show",$name="search", "POST");?>
            <input type="hidden" name='resultDiv'
		value="pwrongtransactionsSearch"> <input type="hidden" name='srctype'
		value="<?=$sType?>" />
	<table cellpadding=4 cellspacing=1 border=0>
		<tr>
			<td class="label">Choose partner</td>
			<td><select name="partners[]" id="partners" multiple="multiple">
                        <?

        $allPartners = getPartnersByJurisdiction();

        while ($allPartners->hasNext()) {
            $row = $allPartners->next();
            ?>
                            <option
						<?=(in_array( $row['ptn_id'],$partners))  ? 'selected' : ''?>
						value='<?=$row['ptn_id']?>'><?=$row['ptn_name']?></option>
                            <?
        }
        ?>
                        </select></td>
		</tr>
		<tr>
			<td class="label"><?=$lang->getLang("Search by")?></td>
			<td class="content"><select name="field">
                            <?php
        $opt_arr = array(
            0 => array(
                "name" => "All",
                "value" => 0
            ),
            1 => array(
                "name" => "User Name",
                "value" => 1
            ),
            2 => array(
                "name" => "Transaction Id",
                "value" => 3
            ),
            7 => array(
                "name" => "Partner User ID",
                "value" => 7
            )
        );

        /*
         * if($_SESSION["aty_code"] == "SUPPMANAGER" || $_SESSION["aty_code"] == "SUPERUSER"){
         * $opt_arr[3] = array("name" => "Amount", "value" => 4);
         * }
         */

        foreach ($opt_arr as $opt) {
            printOption("field", $opt["value"], $opt["name"], ($opt["value"] == $field));
        }
        ?>
                        </select></td>
		</tr>
		<tr id="partnerValue">
			<td class="label"><?=$lang->getLang("Value")?></td>
			<td class="content"><input type="text" name="value"
				value="<?=$value?>" /></td>
		</tr>

		<tr valign=top class="timeHelper">
			<td class=label><?=$lang->getLang("Choose month")?></td>
			<td class=content>
                        <?=month_select_box($defaultmonth)?>
                    </td>
		</tr>

		<tr valign=top class="timeHelper">
			<td class=label><?=$lang->getLang("Choose week")?></td>
			<td class=content>
                        <?=week_select_box($defaultweek)?>
                    </td>
		</tr>
		<tr class="timeHelper">
			<td class="label"><?=$lang->getLang("From")?></td>
			<td class="content"><input type='text' name='from'
				id='pwrongtransactionsFrom' value="<?=$from?>"></td>
		</tr>
		<tr class="timeHelper">
			<td class="label"><?=$lang->getLang("Until")?></td>
			<td class="content"><input type='text' name='to'
				id='pwrongtransactionsTo' value="<?=$to?>"></td>
		</tr>
		<tr>
			<td></td>
			<td class="content"><input type="hidden" name="action"
				value="searchtrans" /> <input type="submit" class="entitybtn"
				value="<?=$lang->getLang("Search")?>" /></td>
		</tr>
	</table>
	</form>
</div>
<?
    }
    ?>
<script>
        $(function(){
            $('.hidden').hide();
            $('#' + $('#searchType').find('option:selected').val()).show();
            $('select[name="field"]').change(function() {
                if($(this).val()==3 || $(this).val()=="transaction"){
                    $(".timeHelper").hide();
                }
                else{
                    $(".timeHelper").show();
                }
            }).trigger('change');
        });
    </script>
<?
    if (! areErrors()) {
        if ($action == "searchtrans" && $_POST['resultDiv'] == 'userSearch') {
            searchusertransaction();
        }
        if ($action == "searchtrans" && $_POST['resultDiv'] == 'adminSearch') {
            searchAdminTransaction();
        }
        if ($action == "searchtrans" && $_POST['resultDiv'] == 'affiliateSearch') {
            searchAffiliateTransaction();
        }
        if ($action == "searchtrans" && $_POST['resultDiv'] == 'pwrongtransactionsSearch') {
            searchPartnersWrongTransaction();
        }
        if ($action == "searchtrans" && $_POST['resultDiv'] == 'providertransaction') {
            searchProviderUserTransaction();
        }
    } else {
        showErrors();
    }
} else {
    ?>
<div class="error">You're not allowed to see this page</div>
<? } ?>
