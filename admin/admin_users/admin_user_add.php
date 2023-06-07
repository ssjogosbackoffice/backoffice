<?php
check_access("manage_admin_users", true);
function checkFields ($username, $email, $full_name, $access, $user_type, $jurisdiction_id, $cou_code, $password="", $password_confirm="") {
    global $dbh,$lang;
    $jurisdiction_id = (int) $jurisdiction_id;
    $user_type = (int) $user_type;
    $exists=$dbh->queryOne("Select count(aus_id) from  jurisdiction left join  admin_user on AUS_JURISDICTION_ID=jur_id where JUR_ACCESS_TYPE='WEBSITE'  and jur_id= ".$dbh->escape($jurisdiction_id));

    if($exists>0){
        addError('',"There is already an admin user for this jurisdiction");
    }
    if ( isBlank($username) ){
        addError("username", $lang->getLang("A username must be entered"));
    }
    else {
        $sql = "select count(*) from admin_user where lower(aus_username) = '" . mb_strtolower($username) . "'";
        if($dbh->countQuery($sql))
            addError("username", $lang->getLang("The username already exists"));
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
        $sql .= ",  aus_access, aus_aty_id, aus_sales_code,aus_jurisdiction_id, aus_cou_id, aus_creation_time)";
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


        $dbh->begin();
        if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting") betting_begin();
        $casino_db = $dbh->exec($sql);
        $betting_db = ($_SERVER['THIRD_PART_BACKOFFICE'] == "betting") ? Db::execute($sql) : true;
        if($betting_db && $casino_db){
            $dbh->commit();
            if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting") Db::commitTransaction();
            doAdminUserLog($_SESSION['admin_id'], 'add_admin_user', $log, $pun_id='NULL');
            return TRUE;
        }else{
            $dbh->rollback();
            if($_SERVER['THIRD_PART_BACKOFFICE'] == "betting") Db::rollbackTransaction();
            return FALSE;
        }
    }
    return FALSE;
}

$op = $_POST['op'];

if ( 'Save' == $op )
{   $saved = save_admin_user ($_POST['username'], $_POST['email'], $_POST['full_name'], $_POST['access'], $_POST['user_type'], $_POST['jurisdiction'], $_POST['user_country'], $_POST['password'], $_POST['password_confirm']);

    if (  TRUE == $saved ){
        $_SESSION['alert_message'] = 'Administrator user &quot;' . $_POST['full_name'] . '&quot; saved';
        header("Location: " . '/admin_users/');
        exit;
    }else{
        $jurisdiction_id = (int) $_POST['jurisdiction'];
        $user_type = (int) $_POST['user_type'];
        /****

        If a jurisdiction was selected, set the jurisdiction class variable
        to the same class as the jurisdiction that was selected on the "Add" form.

        This class is used to determine which group of jurisdictions to display
        in the jurisdictions select (a list of districts, a list of regions or
        a list of clubs).

        The current logged in user still can only access certain jurisdictions
        as specified in the DB, according to his user type.

        The way the select lists are populated
        (in jurisdiction_changeer.js.php.tpl) restricts what will appear in the list,
        so an attacker cannot circumvent the restrictions by submitting just any
        jurisdiction ID.

        Before an admin user record is inserted or updated, the jurisdiction id and
        user types are checked against the permissions of the currently logged in
        user.
         ***/

        if ( $jurisdiction_id ){
            $sql = 'select jur_class from jurisdiction where jur_id = ' . $jurisdiction_id;
            $aty_jurisdiction_class = $dbh->queryOne($sql);
        }
        elseif ( $user_type ) {
            //get the list of jurisdictions in same class selected admin user typr
            $sql = 'select aty_jurisdiction_class from admin_user_type where aty_id = ' .$user_type;
            $aty_jurisdiction_class  = $dbh->queryOne($sql);
        }
    }
}
?>
<?
$title=$lang->getLang("Add User");
include ('header.inc'); ?>
<?php include('jurisdiction_changer_js.php') ;?>
<div class=bodyHD><?=$lang->getLang("Administrator Users")?></div>
<div class=subnavmenu>
    <?printLink("/admin_users/", $lang->getLang("Back to Main menu"));?>
    &nbsp;&gt;&nbsp;
    <b><?=$lang->getLang("Add an Administrator User")?></b>
</div>
<br><br>
<? showErrors($only_global=TRUE);?>
<form action="<?=$_SERVER['PHP_SELF'];?>" method="POST">
    <input type=hidden name="op" value="Save">
    <table cellpadding=0 cellspacing="1" border=0>
        <tr valign="top">
            <td>
                <table cellpadding=4 cellspacing="1" border=0 width=350>
                    <tr valign="top">
                        <td class=label width=110><?=$lang->getLang("Username")?></td>
                        <td <? form_td_style('username'); ?>><input type="text" name="username" value="<?=replace_quotes($_POST['username'])?>" size=30>
                            <?php echo err_field('username'); ?>
                        </td>
                    </tr>
                    <tr valign="top">
                        <td class=label width=110><?=$lang->getLang("Password")?></td>
                        <td <? form_td_style('password'); ?>><input type="password" name="password" value="<?=replace_quotes($_POST['password'])?>" size=30>
                            <?php echo err_field('password'); ?>
                        </td>
                    </tr>
                    <tr valign="top">
                        <td class=label width=110><?=$lang->getLang("Confirm Password")?></td>
                        <td <? form_td_style('password_confirm'); ?>t><input type="password" name="password_confirm" value="<?=replace_quotes($_POST['password_confirm'])?>" size=30>
                            <?php echo err_field('password_confirm'); ?>
                        </td>
                    </tr>
                    <tr valign="top">
                        <td class=label width=110><?=$lang->getLang("Full Name")?></td>
                        <td <? form_td_style('full_name'); ?>><input type="text" name="full_name" value="<?=$_POST['full_name']?>" size=30>
                            <?php echo err_field('full_name'); ?>
                        </td>
                    </tr>
                    <tr valign="top">
                        <td class=label width=110><?=$lang->getLang("Email")?></td>
                        <td <? form_td_style('email'); ?>><input type="text" name="email" value="<?=$_POST['email']?>" size=30>
                            <?php echo err_field('email'); ?></td>
                    </tr>
                    <tr valign="top">
                        <td class=label><?=$lang->getLang("Country")?></td>
                        <td <? form_td_style('user_country'); ?>>
                            <?php if(DOT_IT) : ?>
                                Italia<input type="hidden" name="user_country" value="IT"/>
                            <?php else : ?>
                                <? countrySelect('user_country', "", $_POST['user_country']); ?>
                                <?php echo err_field('user_country'); ?>
                            <?php endif; ?>
                        </td>
                    </tr>
                    <tr valign="top">
                        <td class=label><?=$lang->getLang("Type")?></td>
                        <td <? form_td_style('user_type'); ?> >
                            <?php echo get_user_type_select($user_type); ?>
                            <?php echo err_field('user_type'); ?>
                        </td>
                    </tr>
                    <tr valign=top>
                        <td class=label>
                            <div class="label" id="jurisdiction_label">

                                <?php
                                if ( 'nation' == $jurisdiction_class )
                                    echo 'Nation';
                                if ( 'region' == $jurisdiction_class )
                                    echo 'Region';
                                elseif ( 'district' == $jurisdiction_class )
                                    echo 'District';
                                elseif ( 'club' == $jurisdiction_class )
                                    echo 'Betting Club';
                                ?>
                            </div>
                        </td>
                        <td <? form_td_style('jurisdiction'); ?>>
                            <div id="jurisdiction_select" class="content">
                                <?php echo get_jurisdiction_select($aty_jurisdiction_class, $jurisdiction_id);?>
                            </div>
                            <?php echo err_field('jurisdiction', $no_br=true); ?>
                        </td>
                    </tr>
                    <tr valign="top">
                        <td class=label width=110><?=$lang->getLang("Access")?></td>
                        <td <? form_td_style('access'); ?>>
                            <input type="radio" name="access" value="ALLOW" <? if ('ALLOW' == $_POST['access']) echo 'checked'; ?>><?=$lang->getLang("Allow")?>
                            &nbsp;&nbsp;&nbsp;
                            <input type="radio" name="access" value="DENY" <? if ('DENY' == $_POST['access']) echo 'checked'; ?>><?=$lang->getLang("Deny")?>
                            <?php echo err_field('access'); ?>
                        </td>
                    </tr>
                    <tr valign="top">
                        <td class=label width=110><?=$lang->getLang("Access Type")?></td>
                        <td><div id="adminAccessType">Please choose an jurisdiction first</div>
                    </tr>
                </table>
            </td>
            <td  valign=top colspan="2">
                <input type=image src="<?=image_dir;?>/button_save.gif" border=0><br>
                <a href="/admin_users/"><img src="<?=image_dir;?>/button_cancel.gif" border=0></a>
            </td>
        </tr>
    </table>
</form>

<script>
    $(document).on('change', "select[name='jurisdiction']",function(){
        $.post("/services/general_services.inc",{
            action:"getJurisdictionAccessType",
            jurisdiction:$(this).val()
        },function(data){
            if(data=="-1"){
                $('#adminAccessType').html("Please choose an jurisdiction first");
             }
            else{
                $('#adminAccessType').html(data);
            }
        })
    });

</script>
<? include 'footer.inc'; ?>
