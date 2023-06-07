

<?php
check_access("manage_jurisdictions", true);
if($_REQUEST['op'] != 'update_subjur_processors') {
require 'header.inc';
// IF is closed down in the next php section search for '#if close#'
?>
    <script type="text/javascript">

        $(function () {
            $('.lettersAndNumbers').bind('keypress', function (e) {
                    console.log(e.which);
                    if ((e.which < 48 && e.which != 45 && e.which != 8 && e.which != 0) ||
                        (e.which > 57 && e.which < 65) ||
                        (e.which > 90 && e.which < 97 && e.which != 95) ||
                        e.which > 122) {
                        e.preventDefault();
                    }
                }
            );

            var tabs = $("#tabs");
            tabs.tabs();

            if ($('#firstTab').hasClass('ui-state-active')) {
                $('#tabs-2').hide();
            }

            $('#secondTab').on("click", function () {
                //console.log("here");
                $('#tabs-2').show();
                $('.disabledBtn').hide();
                $('.addAdminUser').hide();
                // $('#listDetails').html("Please choose from the list on the left");

            });

            $('#firstTab').on("click", function () {
                $('#tabs-2').hide();
                $('.disabledBtn').show();
            });
        });

        $(window).load(function () {
            var response = $('#disabledList');

            $.ajax({
                url: "/services/jurisdiction_services.inc",
                type: "POST",
                dataType: "json",
                data: {
                    action: "11"
                },
                success: function (result) {
                    // Print the current finds
                    if (result != null) {
                        var res = displayDisabledJurList(result);
                        response.html(res);
                    } else {
                        console.log("Result is NULL");
                    }

                }
            });

            var jurId = "<?php echo $_SESSION['disabled_jur_id']; ?>";
            var jurClass = "<?php echo $_SESSION['disabled_jur_class']; ?>";
            var resp = $('#listDetails');

            $.ajax({
                url: "/jurisdictions/index.php",
                type: "GET",
                data: {
                    node: jurId,
                    class: jurClass,
                    pgd: "disabled"
                },
                success: function (result) {
                    resp.empty();
                    var stringToSearch = '<form name="';

                    var posStart = result.indexOf(stringToSearch);
                    var posEnd = result.lastIndexOf("</form>");
                    //console.log(posEnd);


                    var form = result.slice(posStart + 1, posEnd);
                    //console.log(result);
                    var secondOccurenceStart = form.indexOf(stringToSearch);
                    //console.log(secondOccurenceStart);
                    var newForm = form.slice(secondOccurenceStart + 1, posEnd);

                    var thirdOccurenceStart = newForm.indexOf(stringToSearch);
                    var newForm2 = newForm.slice(thirdOccurenceStart, posEnd);
                    resp.html(newForm2);
                }
            }).done(function () {
                $('#tabs-2').find('.disabledBtn').remove();
                $('#tabs-2').find('.addAdminUser').remove();
            });
        });

        $(document).on('click', '#showJurisdictionCurrencyButon', function () {
            //console.log("Here");
            if ($("#jurisdictionCreditsContainer").is(":visible")) {
                $(this).html("<?=$lang->getLang('Show')?>");
                $("#jurisdictionCreditsContainer").fadeOut("slow");
            } else {
                $(this).html("<?=$lang->getLang('Hide')?>");
                $("#jurisdictionCreditsContainer").fadeIn("slow");
            }
        });

        $(document).on('click', '#searchDisabledByType', function () {
            // console.log("Something");
            var jurisdiction = $('#selectJurisdiction').val();
            var disabledPrintout = $('#disabledList');
            //console.log(jurisdiction);
            $.ajax({
                url: "/services/jurisdiction_services.inc",
                type: "POST",
                dataType: "json",
                data: {
                    action: "8",
                    jurisdiction: jurisdiction,
                },
                success: function (result) {

                    // check to see what type is the jurisdiction
                    if (jurisdiction == "Nation") {
                        // empty the list
                        disabledPrintout.empty();

                        // print the correct result in the div
                        var res = "<span class='title nation'>Nation</span><ol class='nation'>";
                        for (var i = 0; i < result.length; i++) {

                            res += "<li><a class='viewDisabledDetails' data-id='" + result[i][2] + "' data-class='nation'>" + result[i][1] + "</a></li>";
                        }
                        res += "<ol>";
                        disabledPrintout.html(res);

                    } else if (jurisdiction == "Region") {
                        disabledPrintout.empty();
                        var res = "<span class='title region'>Region</span><ol class='region'>";
                        for (var i = 0; i < result.length; i++) {

                            res += "<li><a class='viewDisabledDetails' data-id='" + result[i][2] + "' data-class='region'>" + result[i][1] + "</a></li>";
                        }
                        res += "<ol>";
                        disabledPrintout.html(res);
                    } else if (jurisdiction == "District") {
                        disabledPrintout.empty();
                        var res = "<span class='title district'>District</span><ol class='district'>";
                        for (var i = 0; i < result.length; i++) {

                            res += "<li><a class='viewDisabledDetails' data-id='" + result[i][2] + "' data-class='district'>" + result[i][1] + "</a></li>";
                        }
                        res += "<ol>";
                        disabledPrintout.html(res);
                    } else if (jurisdiction == "Club") {
                        disabledPrintout.empty();
                        var res = "<span class='title club'>Club</span><ol class='club'>";
                        for (var i = 0; i < result.length; i++) {

                            res += "<li><a class='viewDisabledDetails' data-id='" + result[i][2] + "' data-class='club'>" + result[i][1] + "</a></li>";
                        }
                        res += "<ol>";
                        disabledPrintout.html(res);
                    } else {
                        disabledPrintout.empty();
                        var res = "<span class='title nation'>No results found</span>";
                        disabledPrintout.html(res);

                    }
                }
            })
        });

        $(document).on('click', '#searchDisabled', function () {
            var searchKeyword = $('#disabledSearch').val();
            var disabledPrintout = $('#disabledList');

            if (searchKeyword.length > 3) {
                $.ajax({
                    url: "/services/jurisdiction_services.inc",
                    type: "POST",
                    dataType: "json",
                    data: {
                        action: "9",
                        searchKeyword: searchKeyword
                    },
                    success: function (data) {

                        // Empties the previous list
                        disabledPrintout.empty();

                        //Prints the found results
                        var result = displayDisabledJurList(data);
                        disabledPrintout.html(result);
                    }
                })
            } else {
                disabledPrintout.empty();
                var res = "<span class='title nation'>4 Letters Minimum</span>";
                disabledPrintout.html(res);
            }
        });


        $(document).on('click', '#viewAllDisabled', function () {

            var disabledPrintout = $('#disabledList');

            $.ajax({
                url: "/services/jurisdiction_services.inc",
                type: "POST",
                dataType: "json",
                data: {
                    action: "10"
                },
                success: function (data) {
                    // Empties the previous list
                    disabledPrintout.empty();

                    // Print the current finds
                    var result = displayDisabledJurList(data);
                    disabledPrintout.html(result);
                }
            });
        });


        $(document).on('click', '.viewDisabledDetails', function () {
            var id = $(this).attr('data-id');
            var disabledClass = $(this).attr('data-class');

            var response = $('#listDetails');

            $.ajax({
                url: "/jurisdictions/index.php",
                type: "GET",
                data: {
                    node: id,
                    class: disabledClass,
                    pgd: "disabled"
                },
                success: function (result) {
                    response.empty();

                    var stringToSearch = '<form name="';

                    var posStart = result.indexOf(stringToSearch);
                    var posEnd = result.lastIndexOf("</form>");
                    var form = result.slice(posStart + 1, posEnd);
                    var secondOccurenceStart = form.indexOf(stringToSearch);
                    //console.log(secondOccurenceStart);
                    newForm = form.slice(secondOccurenceStart + 1, posEnd);

                    var thirdOccurenceStart = newForm.indexOf(stringToSearch);
                    newForm = newForm.slice(thirdOccurenceStart, posEnd);

                    response.html(newForm);
                    $('.disabledBtn').hide();
                    $('.addAdminUser').hide();
                }
            });
        })


        function displayDisabledJurList(data) {
            var result = "";
            var title = "";
            var list = "";
            var listItems = "";

            if (data != null) {
                if (data.length == 0) {
                    result = "<span class='title nation'>No results found</span>";
                } else {
                    for (var i = 0; i < data.length; i++) {

                        // assign a title
                        if (data[i][0] == "nation") {
                            title = "</ol><span class='title nation'>Nation</span>";
                            list = "<ol class='nation'>";
                            listItems = "<li><a class='viewDisabledDetails' data-id='" + data[i][2] + "' data-class='nation'>" + data[i][1] + "</a></li>";

                        } else if (data[i][0] == "region") {
                            title = "</ol><span class='title region'>Region</span>";
                            list = "<ol class='region'>";
                            listItems = "<li><a class='viewDisabledDetails' data-id='" + data[i][2] + "' data-class='region'>" + data[i][1] + "</a></li>";
                        } else if (data[i][0] == "district") {
                            title = "</ol><span class='title district'>District</span>";
                            list = "<ol class='district'>";
                            listItems = "<li><a class='viewDisabledDetails' data-id='" + data[i][2] + "' data-class='district'>" + data[i][1] + "</a></li>";
                        } else if (data[i][0] == "club") {
                            title = "</ol><span class='title club'>Club</span>";
                            list = "<ol class='club'>";
                            listItems = "<li><a class='viewDisabledDetails' data-id='" + data[i][2] + "' data-class='club'>" + data[i][1] + "</a></li>";
                        }

                        if (isTitle(title) && !containsTitle(title, result)) {
                            result += title;
                            result += list;
                        }
                        result += listItems;
                    }
                }
            } else {
                console.log("null data")
            }

            return result;
        }

        function isTitle(title) {
            return (title == "</ol><span class='title nation'>Nation</span>") || (title == "</ol><span class='title region'>Region</span>") || (title == "</ol><span class='title district'>District</span>") || (title == "</ol><span class='title club'>Club</span>");
        }

        function containsTitle(title, response) {
            return response.includes(title);
        }


        function restoreJurisdiction(jur_id) {
            jConfirm("<?=$lang->getLang("Unblock %?", $nation_name)?>", "", function (answer) {

                if (answer) {
                    var unlock_users = false;
                    var unlock_punters = false;
                    jConfirm("<?=$lang->getLang("Unlock all % of % structure?", $lang->getLang("administrators"), $nation_name)?>", "", function (answer) {
                        unlock_users = answer;
                        jConfirm("<?=$lang->getLang("Unlock all % of % structure?", $lang->getLang("players"), $nation_name)?>", "", function (answer) {
                            unlock_punters = answer;
                            location.href = '<?=$_SERVER['PHP_SELF']?>?op=Restore&node=' + jur_id + "&unlock_users=" + unlock_users + "&unlock_punters=" + unlock_punters;
                        });
                    });
                }
            });
            return false;
        }

    </script>

    <style>

        #tabs {
            min-width: 800px;
        }

        .left-align {
            float: left;
        }
    </style>


<div id="tabs">
    <ul>
        <li id="firstTab"><a href="#tabs-1">View Jurisdictions</a></li>
        <li id="secondTab"><a href="#tabs-2">View Disabled Jurisdictions</a></li>
    </ul>
    <div class="Tab" id="tabs-1">
        <div class="bodyHD"><?= $lang->getLang("Jurisdictions") ?></div>
        <br/>
        <?php
        }
        // #if close# if(isset($_REQUEST['op']) && $_REQUEST['op'] != 'update_subjur_processors') {
        function getCurrentCurrency($node,$onlyCode=false){
            global $dbh;
            $currency=$dbh->queryRow("SELECT cur_name,cur_code_for_web from jurisdiction join currency on jur_currency=cur_id  where jur_id=".$dbh->escape($node));
            if($onlyCode){
                return $currency['cur_code_for_web'];
            }
            return $currency;
        }

        function getCurrentSkin($node){
            global $dbh;
            $curSkin=$dbh->queryRow("SELECT skn_id,skn_name from skins join jurisdiction on jur_skn_id=skn_id  where jur_id=".$dbh->escape($node),true);
            return $curSkin;
        }

        function checkFields ($username, $email, $full_name, $access, $user_type, $jurisdiction_id, $cou_code, $password="", $password_confirm="") {
            global $dbh,$lang;
            $jurisdiction_id = (int) $jurisdiction_id;
            $user_type = (int) $user_type;

            if ( isBlank($username) ){
                addError("username", $lang->getLang("A username must be entered"));
            }
            else {
                $sql = "select count(*) from admin_user where lower(aus_username) = '" . mb_strtolower($username) . "'";
                if($dbh->countQuery($sql))
                    addError("username", $lang->getLang("The username entered already exists, please enter a different username"));
            }

            if ( isBlank($email) ){
                addError("email", $lang->getLang("An email address must be entered"));
            }
            elseif ( ! isValidEmail($email) ){
                addError("email", $lang->getLang("The email address you entered appears to be invalid"));
            }

            if ( isBlank($full_name) ){
                addError("full_name", $lang->getLang("The administrator user's full name must be entered"));
            }

            if ( isBlank($access) ){
                addError("access", $lang->getLang("The administrator access must be selected"));
            }
            elseif ( 'ALLOW' != $access && 'DENY' != $access ){
                addError("access",$lang->getLang("Account access can be 'ALLOW' or 'DENY' only"));
            }

            if ( isBlank($cou_code) )
                addError("user_country", $lang->getLang("A country must be selected"));
            elseif ( ! getCountry($cou_code) )
                addError("user_country", $lang->getLang("Invalid country"));

            if ( isBlank($password) ){
                addError("password",$lang->getLang("An administrator password must be entered"));
                $passwd_err = true;
            }

            if ( isBlank($password_confirm) ){
                addError("password_confirm",$lang->getLang("An administrator password confirmation must be entered"));
                $passwd_err = true;
            }

            if ( ! $passwd_err ){
                if ( $password != $password_confirm ){
                    addError("password",$lang->getLang("The password and password confirmation do not match"));
                    addError("password_confirm",$lang->getLang("The password and password confirmation do not match"));
                }
            }

            $user_type_err = FALSE;

            if ( ! $user_type ){
                addError("user_type", $lang->getLang("The administrator user type must be selected"));
                $user_type_err = TRUE;
            }else{
                //does the logged in admin user have permission to  select the admin user type $user_type?
                $sql = "SELECT count(*) FROM admin_user_type WHERE " . $user_type . " IN " .
                    " (  " .
                    "    SELECT aty_id" .
                    "    FROM admin_user_type  WHERE ( 1=1 ";

                switch ( $_SESSION['jurisdiction_class'] ){
                    case 'club':
                        //get user type that are subordinate to the session user type, within his club
                        $sql .= " AND aty_jurisdiction_class  = 'club'" .
                            " AND aty_level  >  " . $_SESSION['aty_level'];
                        break;
                    case 'district':
                        //get clubs that are subordinate to session user's district
                        $sql .= " AND aty_jurisdiction_class  = 'club' ";
                        break;
                    case 'region':
                        $sql .= " AND ( aty_jurisdiction_class  = 'district' " .
                            "       OR aty_jurisdiction_class  = 'club' )";
                        break;
                    case 'nation':
                        $sql .= " AND ( aty_jurisdiction_class  = 'region' " .
                            "        OR aty_jurisdiction_class  = 'district'" .
                            "       OR aty_jurisdiction_class  = 'club' )";
                        break;
                    case 'casino':
                        if ( 'SUPERUSER' == $_SESSION['aty_code']  ){
                            $sql .= "AND aty_code != 'SUPERUSER'";
                        }
                        else{
                            $sql .= "AND ( aty_jurisdiction_class  = 'region' " .
                                "       OR aty_jurisdiction_class  = 'nation' " .
                                "       OR aty_jurisdiction_class  = 'district' " .
                                "       OR aty_jurisdiction_class  = 'club' )";
                        }
                        break;

                    default:
                        $sql .= " AND 0 ";
                }

                $sql  .= ") OR " .
                    "(  aty_level  >  " . $_SESSION['aty_level'] .
                    "   AND  aty_jurisdiction_class = '" . $_SESSION['jurisdiction_class']."'" .
                    ") ";

                $sql .= ')';

                $count = $dbh->countQuery($sql);
                if ( $count < 1 ){
                    addError("user_type", $lang->getLang("Invalid user type"));
                    $user_type_err = TRUE;
                }
            }

            if ( FALSE == $user_type_err ){
                $sql = "SELECT aty_jurisdiction_class from admin_user_type where aty_id = $user_type";
                $rs  = $dbh->exec($sql);
                $row = $rs->next();
                $aty_jur_class = $row['aty_jurisdiction_class'];
                if ( ! $jurisdiction_id ){
                    if ( $aty_jur_class != 'casino' )
                        addError("jurisdiction", $lang->getLang("A Jurisdiction must be selected"));
                }else{
                    //does the logged in admin user have permission to select the jurisdiction $jurisdiction?

                    $sql = "SELECT count(*) FROM jurisdiction j1 WHERE j1.jur_id = " . $jurisdiction_id .
                        " and j1.jur_class = '$aty_jur_class'  AND ( 1=1 ";
                    switch ( $_SESSION['jurisdiction_class'] ){
                        case 'club':
                            //get user type that are subordinate to the session user type, within his club
                            $sql .= " AND jur_id  = " . $_SESSION['jurisdiction_id'];
                            break;
                        case 'district':
                            //get clubs that are subordinate to session user's district
                            $sql .= " AND jur_parent_id  = " . $_SESSION['jurisdiction_id'];
                            break;
                        case 'region':
                            //get districts and clubs that are subordinate to session user's district
                            $sql .= " AND ( jur_parent_id = " . $_SESSION['jurisdiction_id']  .
                                "       OR jur_parent_id IN ( SELECT j2.jur_id from jurisdiction j2 where j2.jur_parent_id = ". $jurisdiction_id ."))";
                            break;
                        case 'nation':
                            //get districts and clubs that are subordinate to session user's district
                            $sql .= " AND ( jur_parent_id = " . $_SESSION['jurisdiction_id']  .
                                "       OR jur_parent_id IN ( SELECT j2.jur_id from jurisdiction j2 where j2.jur_parent_id IN" .
                                "              ( SELECT j3.jur_id from jurisdiction j3 where j3.jur_parent_id=". $jurisdiction_id .")))";
                            break;
                        case 'casino':
                            $sql .= " AND (  jur_class = 'region' " .
                                "       OR jur_class  = 'district' " .
                                "       OR jur_class  = 'nation' " .
                                "       OR jur_class  = 'club' )";
                            break;
                        default:
                            $sql .= " AND 0 ";
                    }

                    $sql  .= ") OR " .
                        "(  jur_class = '" . $_SESSION['jurisdiction_class']."'" .
                        "   AND jur_id = '" . $_SESSION['jurisdiction_id']."'" .
                        ") ";
                    $count = $dbh->countQuery($sql);

                    if ( $count < 1 )
                    {
                        addError("jurisdiction", $lang->getLang("Invalid user type"));
                    }
                }
            }
            return ! areErrors();
        }
        

        function save_admin_user ($username, $email, $name, $access, $user_type, $jurisdiction_id, $cou_code, $password,$password_confirm) {
            global $dbh;

            if ( checkFields($username, $email, $name, $access, $user_type, $jurisdiction_id, $cou_code, $password, $password_confirm) ){

                // check if the selected admin_user type jurisdiction class is casino.
                // If so, set the admin user jurisdiction to the casino jurisdiction id

                $sql        = "SELECT aty_jurisdiction_class FROM admin_user_type WHERE aty_id = $user_type";
                $jur_class  = $dbh->queryOne($sql);

                if ( $jur_class == 'casino' ){
                    $jurisdiction_id = 1; //casino jurisdiciton
                }

                //format variables for insertion into database
                $username  = $dbh->escapeQuotesAndSlashes($username);
                $password  = $dbh->escapeQuotesAndSlashes(md5($password));
                $full_name = $dbh->escapeQuotesAndSlashes($name);
                $email     = $dbh->escapeQuotesAndSlashes($email);
                $jurisdiction_id = ( $jurisdiction_id ? $jurisdiction_id : 'NULL');

                $cou       = getCountry($cou_code);
                $cou_id    = $cou['cou_id'];

                $aus_id = $dbh->nextSequence('aus_id_seq');
                $sales_code = $aus_id ."-".substr(randomString($short=true),0, 4);

                $sql  = "insert into admin_user";
                $sql .= " ( aus_id, aus_username, aus_password, aus_name, aus_email";
                $sql .= ",  aus_access, aus_aty_id, aus_sales_code,aus_jurisdiction_id, aus_cou_id,aus_creation_time)";
                $sql .= " values ( $aus_id, '$username', '$password', '$full_name'";
                $sql .= ", '$email', 'ALLOW', $user_type, '$sales_code', $jurisdiction_id, $cou_id,NOW())";

                $log  = "aus_id:$aus_id|"          .
                    "aus_username:$username|"       .
                    "aus_password:(suppressed)|"    .
                    "aus_full_name:$full_name|"     .
                    "aus_email:$email|"             .
                    "aus_access:ALLOW|"             .
                    "aus_aty_id:$$user_type|"          .
                    "aus_sales_code:$sales_code|"   .
                    "aus_jurisdiction_id:$jurisdiction_id|" .
                    "aus_cou_id:$cou_id";

                $casino_db = $dbh->exec($sql);
                $betting_db = ($_SERVER['THIRD_PART_BACKOFFICE'] == "betting") ? Db::execute($sql) : true;
                if($betting_db && $casino_db){
                    doAdminUserLog($_SESSION['admin_id'], 'add_admin_user', $log, $pun_id='NULL');
                    return TRUE;
                }else{
                    return FALSE;
                }
            }
            return FALSE;
        }
        
        ob_start();
        
        function get_jurisdiction_options($jurisdiction_class,$id=''){
            global $dbh;
            $options = '<option>No ' . $jurisdiction_class .' found</option>';

            require_once("JurisdictionsList.class.inc");
            $jur_list = JurisdictionsList::getInstance($dbh);
            $list = $jur_list->getChildJurisdictions($_SESSION['jurisdiction_id']);
            if(!empty($list[$jurisdiction_class])){
                $options = "";
                foreach($list[$jurisdiction_class] as $jur){
                    $selected = (($id == $jur["id"])?("selected='selected'"):(""));
                    $options .= '<option value="' . $jur["id"] . '"' . $selected . '>' . $jur["name"];
                }
            }

            return $options;
        }


        $node = (int) ( $_POST['node'] ? $_POST['node'] : $_GET['node']);
        $op   = ( $_POST['op'] ? $_POST['op'] : $_GET['op']);

        //
        $jur_pg_disabled = ( $_POST['pgd'] ? $_POST['pgd'] : $_GET['pgd']);

        if ( 'Save' == $op || 'Add' == $op)  {
            $class  =  ( $_POST['class'] ? $_POST['class'] : $_GET['class']);
        }


        else {
            if ( $node && 1 != $node ){
                $sql = "SELECT j1.jur_name, j1.jur_code,j1.jur_class,  jur_currency ,jur_skn_id FROM jurisdiction j1  WHERE jur_id =" . $node;
                $rs = $dbh->exec($sql);
                $num_rows =  $rs->getNumRows();
                if ( 1 == $num_rows  ) {
                    $row = $rs->next();
                    $class             = $row['jur_class'];
                    $jurisdiction_name = $row['jur_name'];
                    $currency          = $row['jur_currency'];
                    $skinId             = $row['jur_skn_id'];
                }
            }
        }



        $sql = "SELECT jur_name, jur_code FROM jurisdiction WHERE jur_id = " . $_SESSION['jurisdiction_id'];

        $rs  = $dbh->exec($sql);

        $row       = $rs->next();
        $jur_class = $row['jur_class'];
        $jur_name  = $row['jur_name'];
        $createjurisdiction = check_access("manage_jurisdiction_add");



        $sql = "SELECT prc_name, prc_id, ppc_code 
                FROM egamessystem.processor, egamessystem.processor_payment
                WHERE prc_id = ppc_prc_id
                GROUP BY ppc_code";

        $result = $dbh->exec($sql);
        $processors = $result->Records;

        if ( ! isset($jur_pg_disabled) ) {
            switch ( $class ){
                case 'nation' :
                    require_once ('nation.php');
                    break;
                case 'region' :
                    require_once ('region.php');
                    break;
                case 'district' :
                    require_once ('district.php');
                    break;
                case 'club' :
                    require_once ('club.php');
                    break;
                default :
                    echo $lang->getLang("Please select from the menu on the left");
            }

        }
        $out = ob_get_clean();

        ?>

        <table cellpadding="0" cellspacing="0" border="0">
            <tr valign="top">
                <td>
                    <table cellpadding="4" cellspacing="1" border="0">
                        <?if($createjurisdiction && $_SESSION['jur_childs_limit'] > 0) { ?>
                            <tr>
                                <td class="label">
                                    <?=$lang->getLang("Create")?>  (<?=$lang->getLang("Elapsed")?>: <?=$_SESSION['jur_childs_limit']?> ) 
                                </td>
                            </tr>
                            <tr>
                                <td bgcolor="white">
                                    <?php
                                    switch($_SESSION['jurisdiction_class']){
                                        case 'casino':
                                            ?>&nbsp;<a class="tree_link"  href="<?=$_SERVER['PHP_SELF']?>?op=Add&class=nation&prev_class=<?=$class?>"><?=$lang->getLang("New Nation")?> </a><br><?php
                                        case 'nation' :
                                            ?>&nbsp;<a class="tree_link"  href="<?=$_SERVER['PHP_SELF']?>?op=Add&class=region&prev_class=<?=$class?>"><?=$lang->getLang("New Region")?></a><br><?php
                                        case 'region':
                                            ?>&nbsp;<a class="tree_link" href="<?=$_SERVER['PHP_SELF']?>?op=Add&class=district&prev_class=<?=$class?>"><?=$lang->getLang("New District")?></a><br><?php
                                        case 'district':
                                            ?>&nbsp;<a class="tree_link" href="<?=$_SERVER['PHP_SELF']?>?op=Add&class=club&prev_class=<?=$class?>"><?=$lang->getLang("New Club")?></a><?php
                                    }
                                    ?>
                                    <br/><br/>
                                </td>
                            </tr>
                        <? }?>
                        <tr>
                            <td class="label"><?=$lang->getLang("Jurisdiction Chooser")?></td>
                        </tr>
                        <tr>
                            <td bgcolor="white">
                                <?php if ( ! isset($jur_pg_disabled) ) {
                                require_once('tree.php');
                                } ?>
                            </td>
                        </tr>
                    </table>
                </td>
                <td width="50"></td>
                <td>

                    <?php echo $out;?>
                </td>
            </tr>
        </table>
    </div>

    <div class="Tab" id="tabs-2" >
        <table cellpadding="4" cellspacing="1" border="0">
            <tr valign="top">
                <td >
                    <h2 class="bodyHD"><?=$lang->getLang("Disabled Jurisdictions")?> </h2>

                    <table cellpadding="4" cellspacing="1" border="0" class="left-align">

                        <tr>
                            <td class="label">
                                Choose Jurisdiction
                            </td>
                        </tr>
                        <tr>
                            <td>
                                <div style="text-align: center">
                                    <select id="selectJurisdiction">
                                        <option><?=$lang->getLang("Nation")?></option>
                                        <option><?=$lang->getLang("Region")?></option>
                                        <option><?=$lang->getLang("District")?></option>
                                        <option><?=$lang->getLang("Club")?></option>
                                    </select>
                                    <button id="searchDisabledByType"><?=$lang->getLang("Search")?></button>
                                </div>
                            </td>
                        </tr>
                        <tr>
                            <td class="label">
                                Search Jurisdiction name
                            </td>
                        </tr>
                        <tr>
                            <td>
                                <div>
                                    <input type="text" id='disabledSearch' style="float:left; padding: 5px; width: 70%; display: inline" placeholder="<?=$lang->getLang("Search (min. 4 char)")?>"/>
                                    <button id="searchDisabled" style="padding: 3px"><?=$lang->getLang("Go")?></button>
                                </div>
                            </td>
                        </tr>

                        <tr>
                            <td class="label">
                                View All
                            </td>
                        </tr>
                        <tr>
                            <td>
                                <div style="text-align: center">
                                    <button id="viewAllDisabled" style="display: inline;" class="ui-button"><?=$lang->getLang("List All")?></button>
                                </div>
                            </td>
                        </tr>
                        <tr>
                            <td>
                                <div id="disabledList">
                                </div>
                            </td>
                        </tr>
                    </table>
                </td>

                <td width="50"></td>
                <td>
                    <br>
                    <br>
                    <div id="listDetails">
<?php
                        $class = $_REQUEST['class'];


                        if (isset($jur_pg_disabled)) {
                            session_start();
                            $_SESSION['disabled_jur_id'] = $_REQUEST['node'];
                            $_SESSION['disabled_jur_class'] = $_REQUEST['class'];
                        }

                        switch ( $class ){
                            case 'nation' :
                                require_once ('nation.php'); ;
                                break;
                            case 'region' :
                                require_once ('region.php');
                                break;
                            case 'district' :
                                require_once ('district.php');
                                break;
                            case 'club' :
                                require_once ('club.php');
                                break;
                            default :
                                echo $lang->getLang("Please select from the menu on the left");
                        }
?>
                    </div>

                    <div>
                    </div>
                </td>
            </tr>
        </table>
    </div>
</div>
<script>
    $(document).ready(function () {
        $('.select2').select2({
            minimumResultsForSearch: -1
        });

        $('#update_processor').off('click');
        $('#update_processor').on('click',function (e) {
            e.preventDefault();
            var jur_class = $(this).data('class');
            var jur = $(this).data('jur');
            var processors = $('#processors').val();
            var name = $(this).data('name');
            $.ajax({
                url: "/jurisdictions/index.php?node="+jur+"&class="+jur_class,
                type: "POST",
                dataType: "json",
                data: {
                    op: "update_subjur_processors",
                    class: jur_class,
                    node: jur,
                    processors: processors,
                },
                success: function(data) {
                    jAlert('<?=$lang->getLang("You have successfully updated")?> '+name+' <?=$lang->getLang("and all sub jurisdictions")?>');
                }
            }).done(function (data) {
                jAlert('<?=$lang->getLang("You have successfully updated")?> '+name+' <?=$lang->getLang("and all sub jurisdictions")?>');
            });
            jAlert('<?=$lang->getLang("You have successfully updated")?> '+name+' <?=$lang->getLang("and all sub jurisdictions")?>');
        });
    });
</script>
<?php
    require 'footer.inc';

?>
