<?php check_access('view_admin_user_log', $doredirect=true); ?>
<?php
//$cache_filename     = 'aus_log_'.$_SESSION['admin_id'].'.'.session_id();
$serialize_filename = 'aus_sz_'.$_SESSION['admin_id'].'.'.session_id();  //where the serialized resultset is stored
$curr_aus_id        = (int)globalise('curr_aus_id');  //the admin user if passed to preset the "Admin User" select box

//select all admin users from table
$sql = "SELECT aus_id, aus_name, aus_password, aty_name, aty_id FROM admin_user, admin_user_type" .
    " WHERE aus_aty_id = aty_id" .
    " ORDER BY aus_name desc";
$rs  = $dbh->exec($sql);

$num_rows_usr_list = $rs->getNumRows(); //total number of admin users
$i=0;
if ( $num_rows_usr_list > 0 ) {

///START JAVASCRIPT CODE
    ?>
    <script type="text/javascript">
        //class to hold option details for an admin user
        function aus_obj (id,name,type,type_name) {
            this.id   = id;
            this.name = name;
            this.type = type;
            this.type_name = type_name;
        }

        //remove an admin user option
        function removeOption(select_name, index) {
            var select = document.forms['log_search'][select_name];
            select.options[index] = null;
        }

        //add an admin user option to the end
        function addOption(select_name, text, value) {
            var select = document.forms['log_search'][select_name];
            select.options[select.options.length] = new Option(text,value);
        }

        //remove all admin users options of a type
        function removeType(select_name, type) {
            var val;
            var num_options = document.forms['log_search'][select_name].options.length;

            for ( var i=1; i<num_options; i++ ) {
                val = document.forms['log_search'][select_name].options[i].value; //get id of user

                if (  admin_users[val].type == type ) //if the type of the user is == passed type parameter
                {
                    removeOption(select_name,i); //remove the user from the select box
                    num_options--;                //reduce the number of options
                    i--;
                }
            }
        }


        //add all admin users options of a type
        function addType(select_name, type)
        {	for ( var i in admin_users )
        {  if ( admin_users[i].type == type )
            addOption(select_name,admin_users[i].name+' ('+admin_users[i].type_name+')',admin_users[i].id);
        }
        }


        //handles event of checking/unchecking a type checkbox
        function repopulate(select_name, checkbox)
        {	var sorted_options = new Array();

            if ( checkbox.checked ) //if admin user type checked
                addType(select_name,checkbox.value); //add users of type
            else //if admin user type unchecked
                removeType(select_name,checkbox.value);  //remove users of type

            var user_select  = document.forms['log_search'][select_name]; //the user select box object
            var num_options = user_select.length;  //number of items in select box

            //remove all options from select, and store in another array
            for ( var i=1; i<num_options; i++ )
            {	var id = user_select.options[i].value; //get user id of current option (value)
                //store admin user object

                sorted_options [sorted_options.length++] = admin_users[id]; //new aus_obj(admin_users[id].id, admin_users[id].name, admin_users[id].type, admin_users[id].type_name);
                removeOption(select_name,i); //remove option from select object
                i--;
                num_options--;
            }
            sorted_options.sort(sortByName); //sort array of admin user objects
            updateSelect (select_name, sorted_options ); //re-add sorted options to select list
        }


        //updates the select box with new options
        function updateSelect (select_name, sorted_options )
        {  var num_items  = sorted_options.length; //number if items in sorted_options array

            //iterate through sorted_options array
            for ( var i in sorted_options )
            {   var text = sorted_options[i].name+' ('+sorted_options[i].type_name+')';  //get text for current option
                var val  = sorted_options[i].id //get value for current option
                addOption('curr_aus_id', text, val); //create and add option
            }
        }


        //sort array by name ascending
        function sortByName(a,b) {
            var x = a.name.toLowerCase();
            var y = b.name.toLowerCase();
            return ((x < y) ? 1 : ((x > y) ? -1 : 0));
        }


        function toggleDisableTypeFilters(selected_index,num_types)
        {   var disabled = false;

            if ( selected_index > 0 )
                disabled = true;

            for ( i=0; i<num_aus_types; i++ )
            {   document.forms["log_search"].elements["user_types["+i+"]"].disabled = disabled;
            }
        }

    </script>

    <script type="text/javascript">
        <?php
        //////////////////
        //Create user type options markup for HTML admin_user select list in search form
        //Also Define admin_users javascript array, used as a referenc point to remove and add admin users from the select list
        ?>
        var admin_users = new Array();
        var num_users = <?=$num_rows_usr_list;?>;
        <?php
            while ( $rs->hasNext() ) {
              $row = $rs->next();
              $options .= "<option value=".$row['aus_id']." ".($curr_aus_id == $row['aus_id'] ? " selected" : "").">".$row['aus_name']."  (".$row['aty_name'].")"."\n";
        ?>
        admin_users[<?=$row['aus_id']?>] = new aus_obj(<?=$row['aus_id'];?>, "<?=ereg_replace('"', '\"', $row['aus_name']);?>","<?=$row['aty_id'];?>","<?=$row['aty_name'];?>");
        <?php     $i++;
            }
        ?>
    </script>

<?php
}
?>
<div class=bodyHD><?=$lang->getLang("Admin User Logs")?></div>
<?php backLink("admin_user_search",$lang->getLang('View All Admin Users'));?><br>
<?php
globalise('form_submitted');
$user_types = globalise('user_types');  //array of types

if ( $form_submitted )
{

    globalise('max_rows');
    globalise('num_rows');
    globalise('start_row');
    globalise('doQuery');

    globalise('start_day');
    globalise('start_month');
    globalise('start_year');
    globalise('end_day');
    globalise('end_month');
    globalise('end_year');


    //validate start date
    if ( ! checkDate($start_month,$start_day,$start_year) )
        addError("",$lang->getLang("Invalid Start Date"));
    //validate end date
    if ( ! checkDate($end_month,$end_day,$end_year) )
        addError("",$lang->getLang("Invalid End Date"));


    if ( $doQuery ) //should only be set with search form submission
    {
        $sql = 'SELECT alg_id as log_id, alg_type as "log type", alg_data as "actions"' .
            ', alg_aus_id, alg_pun_id as "customer", alg_time as "time"' .
            ', aus_name as "user", aus_username as "username", aty_name as "user type", alg_ipaddr as "ipaddress", alg_data as "details" '.
            ' FROM admin_user_log, admin_user, admin_user_type' .
            ' WHERE aus_aty_id = aty_id and alg_aus_id = aus_id';

        if ( $curr_aus_id ) {
            $sql .= " AND alg_aus_id = $curr_aus_id";
        }
        else
        {
            $num_user_types = count($user_types);

            if ( $num_user_types > 0 ) {
                $sql .= " AND ( 1=0";
                foreach ( $user_types as $key => $value ) {
                    $sql .= " OR aus_aty_id = $value";
                }
                $sql .= " )";
            }
            else
                addError("",$lang->getLang("If you do not select a user, you must check at least one 'Show User Types' checkbox")); //a user type must be checked if specific user not chosen
        }


        if ( ! areErrors() )   //if all user input ok
        {
            $start_date = toDatabaseDate($start_day,$start_month,$start_year);
            $end_mt = mktime(0,0,0,$end_month,$end_day+1,$end_year);
            $end_date = phpTsToDb($end_mt);

            $sql .= " AND alg_time BETWEEN " .
                " '$start_date' AND '$end_date' ";
            $sql .= " ORDER BY alg_time";

            $rs = $dbh->exec($sql);
            $num_rows = $rs->getNumRows();

            if ( $num_rows > 0 )  //if log records found
            {	$rs->resetRecPtr();

                while ( $rs->next() )
                {	$rec = & $rs->currentRef();

                    //Get customer name if alg_pun_id column set
                    $curr_pun_id = $rec['customer'];

                    if ( $curr_pun_id )
                    {  $sql = "SELECT pun_first_name || ' ' || pun_last_name as name" .
                        " FROM punter where pun_id = $curr_pun_id";
                        $rs2 = $dbh->exec($sql);
                        $row2 =  $rs2->next();
                        $rec['customer'] = $row2['name'];
                    }
                }
            }

            if ( $num_rows > $max_rows )  //only serialize if we can't display all the records on one page
                $rs->serialize($serialize_filename);
            else
                $noserialize = true;
        }
    }


    //////////////////////////////
    //get name=value pairs of all client-submitted variables(create query string for back link in next page)
    foreach ( $_POST as $key => $value ) {
        if ( is_array($_POST[$key])  ) {
            foreach ( $_POST[$key] as $ind => $val ) {
                $qstr .= $key.'['.$ind.']'."=$val&";
            }
        }
        else
            $qstr .= "$key=$value&";
    }

    foreach ( $_GET  as $key => $value ) {
        if ( is_array($_GET[$key])  ) {
            foreach ( $_GET[$key] as $ind => $val ) {
                $qstr .= $key.'['.$ind.']'."=$val&";
            }
        }
        else
            $qstr .= "$key=$value&";
    }

    $qstr = urlencode(substr($qstr,0,strlen($qstr)-1));  //get rid of last '&' and url encode;
    ////////////////////////////

    //include_once("Net/GeoIP.php");

    //$geoip     = Net_GeoIP::getInstance(WEB_INCLUDES."/GeoIP/GeoIP.dat");
    //$geoipcity = Net_GeoIP::getInstance(WEB_INCLUDES."/GeoIP/GeoLiteCity.dat");

    function determineCountry($ipaddr) {
        //global $geoip;
        return geoip_country_code_by_name($ipaddr);
    }

    function determineCity($ipaddr) {
        //global $geoipcity;
        //$location = $geoipcity->lookupLocation($ipaddr);
        $database = geoip_record_by_name($ipaddr);
        return $database["city"];
    }

    ////////////////////////////
    //Arrays that define column formats in reports
    $val_fmt_arr = array (
        'time' => 'return getClassLink("'.refPage('admin_users/log_level2').'&qstr='.$qstr.'&log_id=".$rec["log_id"], shortDate($rec["time"],true), "contentlink");',
        'log type' => 'return ereg_replace("_", " ", ucwords($rec["log type"]));',
        'country'  => 'return determineCountry($rec["ipaddress"]);',
        'city'     => 'return determineCity($rec["ipaddress"]);'
    );


    $exl_val_fmt_arr = array (
        'time' => 'return shortDate($rec["time"],true);',
    );


    $cell_fmt_arr = array (
        'time' => 'class=content align=right',
        'details' => 'class=content align=left style="max-width:500px;word-wrap: break-word;"',
    );
    ////////////////////////////

    if ( $num_rows > 0 ) {
        $post_vars = array('page'=>$page,'form_submitted'=>$form_submitted, 'start_date'=>$start_date,'end_date'=>$end_date,'date'=>$date, 'curr_aus_id'=>$aus_id, 'user_types'=>$user_types,'start_day'=>$start_day,'start_month'=>$start_month,'start_year'=>$start_year, 'end_day'=>$end_day,'end_month'=>$end_month,'end_year'=>$end_year);
        $cols_arr  = array('time','user', 'user type', 'log type','details', 'customer', 'ipaddress', 'country', 'city');
        if ( ! $noserialize )
            $rs = RecordSet::unserialize($serialize_filename);
        $qp = new QueryPrinter($rs);	//get instance of query report printer
        //pribnt html table
        $qp->printTable($rs, $cols_arr, $post_vars, $val_fmt_arr, $cell_fmt_arr, $sum_fmt_arr, $excel_filename, $header="", $width="", $start_row, $max_rows);
    }
    elseif ( ! areErrors() ) //no log records found, and no errors in user input
    {
        ?>
        <br/>
        <?php
        printInfoMessage($lang->getLang("0 Records found"));
    }
}
?>
<div>
    <br/>
    <?showErrors()?>
    <br/>
    <?form_head(SELF,'log_search')?>
    <input type="hidden" name="doQuery" value="yes">
    <table cellpadding="4" cellspacing="1" border="0">
        <tr>
            <td colspan=2 class=formheading><?=$lang->getLang("Search")?></td>
        </tr>
        <tr>
            <td class=label><?=$lang->getLang("Start Time")?></td>
            <td class=content>
                <?php date_selector('start_day', 'start_month', 'start_year', 2008, date('Y'),  dbDateToday());?>
                <?php time_selector('start_hour','start_minute',$select_options='',$default=array(0,0));?>
            </td>
        </tr>
        <tr>
            <td class=label><?=$lang->getLang("End Time")?></td>
            <td class=content>
                <?php date_selector('end_day', 'end_month', 'end_year', 2008, date('Y'),  dbDateToday());?>
                <?php time_selector('end_hour','end_minute',$select_options='',$default=array(23,59));?>
            </td>
        </tr>
        <tr>
            <td class=label>
                <?=$lang->getLang("Admin Users")?><br/>
                <div class=labelnote>(Overrides <i><?=$lang->getLang("Show user types")?></i>)</div>

            </td>
            <td class=content>
                <select name=curr_aus_id onchange="toggleDisableTypeFilters(this.selectedIndex)">
                    <option value=""> - <?=$lang->getLang("All Users")?> -
                        <?=$options;?>
                </select>
            </td>
        </tr>
        <tr valign=top>
            <td class=label><?=$lang->getLang("Show User Types")?></td>
            <td class=content>
                <?php
                $sql = " SELECT DISTINCT aty_id, aty_name FROM admin_user_type WHERE aty_id IN (SELECT aus_aty_id FROM admin_user)";
                $rs  = $dbh->exec($sql);
                $num_rows_aty = $rs->getNumRows();
                jscriptBlock("var num_aus_types = $num_rows_aty");
                if ( $num_rows_aty > 0 ) {
                    $i=0;
                    while ( $rs->hasNext() ) {
                        $row = $rs->next();
                        ?>
                        <input type=checkbox name="user_types[<?=$i?>]" value="<?=$row['aty_id']?>" <? if ( $user_types[$i] || (! $user_types && ! $form_submitted )) echo 'checked' ?> onclick="repopulate('curr_aus_id', this)"> <?=$row['aty_name']?>
                        <?php
                        echo "<br>";
                        $i++;
                    }
                }
                ?>
            </td>
        </tr>
        <tr>
            <td class=label><?=$lang->getLang("Records per page")?></td>
            <td class=content>
                <select name="max_rows">
                    <?printOption('max_rows', 10,'10');?>
                    <?printOption('max_rows', 20,'20');?>
                    <?printOption('max_rows', 60,'60', $is_default=true);?>
                    <?printOption('max_rows', 100,'100');?>
                </select>
            </td>
        </tr>
        <tr>
            <td>&nbsp;</td>
            <td>
                <input type="image" src="<?=image_dir?>/button_submit.gif">
            </td>
        </tr>
    </table>
    </form>
</div>
