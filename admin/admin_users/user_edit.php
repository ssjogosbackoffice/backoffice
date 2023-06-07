<?php include(include_content('jurisdiction_changer_js.php')) ;?>
<?php print_jurisdiction_js_functions($user_type); ?>
<script language="javascript" type="text/javascript">

function doRefresh(form)
{
    var msg = "<?=$lang->getLang("Refreshing will reload the saved description for this club, causing any unsaved changes to be lost.<br/><br/>Are you sure you want to do this?")?>";
                
    if ( window.confirm(msg) )
        window.location = '<?$_SERVER['PHP_SELF']?>?op=Change&id=<?=$id?>';             	
}

</script>
<?php

function checkFields ($id, $username, $email, $full_name, $access, $user_type, $jurisdiction_id, $cou_code, $password="", $password_confirm="", $set_pwd=false) {  
	global $dbh,$lang;
	$id = (int) $id;
	$jurisdiction_id = (int) $jurisdiction_id;
	$user_type = (int) $user_type;
	
	if ( isBlank($username) ){
        addError("username", $lang->getLang("A username must be entered"));
	}else {  
		$sql = "select count(*) from admin_user where aus_username = '" . mb_strtolower($username) . "' AND aus_id != " . $id;	
		if($dbh->countQuery($sql))
		   addError("username", $lang->getLang("The username entered already exists, please enter a different username"));
	}

	if ( isBlank($email) ){
		addError("email", $lang->getLang("An email address must be entered"));
	}elseif ( ! isValidEmail($email) ){
        addError("email", $lang->getLang("The email address you entered appears to be invalid"));
    }
    
    if ( isBlank($full_name) ){
    	addError("full_name", $lang->getLang("The administrator user's full name must be entered"));
    }

    
    if ( isBlank($access) ){   
    	addError("access", $lang->getLang("The administrator access must be selected"));
    }elseif ( 'ALLOW' != $access && 'DENY' != $access ){   
    	addError("access",$lang->getLang("Account access can be 'ALLOW' or 'DENY' only"));
    }

   if ( isBlank($cou_code) )
      addError("user_country", $lang->getLang("A country must be selected"));
   elseif ( ! getCountry($cou_code) )
      addError("user_country", $lang->getLang("Invalid country"));
      
    if ( TRUE == $set_pwd ){  //if modify user and set pwd checkbox is checked
    
        if ( isBlank($password) ){    
    	    addError("password",$lang->getLang("Insert a password for user"));
    	    $passwd_err = true;   
        } 
        if ( isBlank($password_confirm) ){    
    	    addError("password_confirm",$lang->getLang("Insert the password confirm"));
    	    $passwd_err = true;   
        }
       
        if ( ! $passwd_err ){
       	    if ( $password != $password_confirm ){
       	   	    addError("password", $lang->getLang("The password and password confirmation do not match");
       	   	    addError("password_confirm", $lang->getLang("The password and password confirmation do not match");
       	    }
         }
    }    
                   
    	//does the logged in admin user have permission to modify this admin user (id = $id)?
    	
    	$sql = "SELECT aus_jurisdiction_id, aty_jurisdiction_class, aty_level, jur_parent_id as parent_id, " .
    	       "( SELECT j2.jur_parent_id from jurisdiction j2 where j2.jur_id = j1.jur_parent_id ) as grandparent_id, " .
    	       "( SELECT j2.jur_parent_id FROM jurisdiction j2 WHERE j2.jur_id = ( SELECT jur_parent_id from jurisdiction j3 where j3.jur_id = j1.jur_parent_id)) as greatgrandparent_id" . 
    	       " FROM admin_user_type, admin_user,jurisdiction j1 WHERE aus_aty_id = aty_id AND aus_jurisdiction_id = jur_id" .
    	       " AND aus_id = $id";
    	
    	$rs = $dbh->exec($sql);    	
    	$num_rows = $rs->getNumRows();

    	if ( 1 == $num_rows ){
    	    $row = $rs->next();
    	    $juris_id       = $row['aus_jurisdiction_id'];
    	    $juris_class    = $row['aus_jurisdiction_class'];
    	    $level          = $row['aty_level'];
    	    $parent_id      = $row['parent_id'];
    	    $grandparent_id = $row['grandparent_id'];
    	    $greatgrandparent_id = $row['greatgrandparent_id'];
    	    
    	    if ( $_SESSION['jurisdiction_id'] == $juris_id ){ //if admin user in same jurisdiction
    	       if ( $level <= $_SESSION['aty_level'] ){
    	           addError("",$lang->getLang("You do not have the permission to modify this administrator user"));
    	       }
    	    }elseif ( $parent_id != $_SESSION['jurisdiction_id'] && $grandparent_id != $_SESSION['jurisdiction_id'] && $greatgrandparent_id != $_SESSION['jurisdiction_id']){
    	       addError("",$lang->getLang("You do not have the permission to modify this administrator user"));
    	    }    	  
    	}    		
    	//showval($greatgrandparent_id
    $user_type_err = FALSE;
    if ( ! $user_type ){
        addError("user_type", $lang->getLang("The administrator user type must be selected"));
        $user_type_err = TRUE;
    }else{	
    	//does the logged in admin user have permission to  select the admin user type $user_type?
   	    $sql = "SELECT count(*) FROM admin_user_type WHERE " . $user_type . " IN " .
              " (  " .
              "    SELECT aty_id" .
              "    FROM admin_user_type WHERE ( 1=1";
                   
        switch ( $_SESSION['jurisdiction_class'] )
        {  
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
          
                 $sql .= "AND ( aty_jurisdiction_class  = 'region' " .
                             "       OR aty_jurisdiction_class  = 'district' " .
                             "       OR aty_jurisdiction_class  = 'club' )";
                 break;
                            
            case 'casino':
            
                 if ( 'SUPERUSER' == $_SESSION['aty_code']  )
                 {
                     	$sql .= "AND aty_code != 'SUPERUSER'";
                 }
                 else{
                     $sql .= "AND ( aty_jurisdiction_class  = 'region' " .
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
           
        if ( $count < 1 )
        {
           	addError("user_type", $lang->getLang("Invalid user type"));
           	$user_type_err = TRUE;
        }               
    }
    
    
    if ( FALSE == $user_type_err )
    {
        $sql = "SELECT aty_jurisdiction_class from admin_user_type where aty_id = $user_type";
        $rs  = $dbh->exec($sql);
        $row = $rs->next();
        	
        $aty_jur_class = $row['aty_jurisdiction_class'];
    	
        if ( ! $jurisdiction_id )
        {   
        	if ( $aty_jur_class != 'casino' )  
        	   addError("jurisdiction", $lang->getLang("A Jurisdiction must be selected"));
        }
        else
        {	
   	        //does the logged in admin user have permission to select the jurisdiction $jurisdiction?
   	   
   	        $sql = "SELECT count(*) FROM jurisdiction j1 WHERE j1.jur_id = " . $jurisdiction_id .
   	               " and j1.jur_class = '$aty_jur_class' AND ( 1=1 ";
                   
            switch ( $_SESSION['jurisdiction_class'] )
            {  
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
                            "       OR jur_parent_id IN ( SELECT j2.jur_id from jurisdiction j2 where j2.jur_parent_id = ". $_SESSION['jurisdiction_id'] ."))";
                
                   break;
                            
                case 'casino':
          
                    $sql .= " AND (  jur_class = 'region' " .
                        "       OR jur_class  = 'district' " .
                        "       OR jur_class  = 'club' )";
                    break;
                    
                case 'superuser':
                    break;
                    
                default:
                    $sql .= " AND 0 ";                   
            }            
            
            // or we can select admin users in the same jurisdiction that are lower-level 
        // (a larger aty_level than the logged in user)
                
        $sql  .= ") OR " .
                 "(  jur_class = '" . $_SESSION['jurisdiction_class']."'" .                 
                 "   AND jur_id = '" . $_SESSION['jurisdiction_id']."'" .                 
                 ") ";
           
            $count = $dbh->countQuery($sql);
            
            if ( $count < 1 )
            {
                addError("jurisdiction", $lang->getLang("Invalid Jurisdiction"));
            }
        }               
    }
           	
    return ! areErrors();
}


function update_admin_user ($id, $username, $email, $name, $access, $user_type, $jurisdiction_id, $cou_code, $set_pwd="", $password="", $password_confirm="")
{   
	global $dbh;
	
	if ( checkFields($id, $username, $email, $name, $access, $user_type, $jurisdiction_id, $cou_code, $password, $password_confirm, $set_pwd) ){  
        // check if the selected admin_user type jurisdiction class is casino. 
        // If so, set the admin user jurisdiction to the casino jurisdiction id 
        $sql = "SELECT aty_jurisdiction_class FROM admin_user_type WHERE aty_id = $user_type";
        $rs = $dbh->exec($sql);
        $num_rows = $rs->getNumRows();
        $row = $rs->next();
        $jur_class = $row['aty_jurisdiction_class'];
        
        if ( $jur_class == 'casino' ){
            $jurisdiction_id = 1; //casino jurisdiciton
        }
        
   	    $username = escapeSingleQuotes($username);
   	    $email    = escapeSingleQuotes($email);
  	    $md5_password = md5($password);
  	    $name     = escapeSingleQuotes($name);
  	    $jurisdiction_id = ( $jurisdiction_id ? $jurisdiction_id : 'NULL');

  	    $cou       = getCountry($cou_code);
        $cou_id    = $cou['cou_id'];
  	   
  	    //define sql to update the current administrator user record
   	    $sql = "UPDATE admin_user" .
               " SET aus_username         = " . $dbh->prepareString($username) .
	           ",    aus_name             = " . $dbh->prepareString($name) .
		       ",    aus_email            = " . $dbh->prepareString($email) . 
		       ",    aus_access           = " . $dbh->prepareString($access) . 
		       ",    aus_aty_id           = " . $user_type .
		       ",    aus_jurisdiction_id  = " . $jurisdiction_id .
		       ",    aus_cou_id           = " . $cou_id;
		       
		
		if ( $set_pwd ){
      	    $sql .= ", aus_password = " . $dbh->prepareString($md5_password);
		}       
		
		$sql .= " WHERE aus_id = $id";
		
		if ( $_SESSION['jurisdiction_id'] != 1  ){ // jurisdictions below the casino jurisdiction have an ID
             $sql .= ' AND ' .
          	         '((aus_jurisdiction_id =  '. $_SESSION['jurisdiction_id'] . ' AND (SELECT aty_level FROM admin_user_type WHERE aty_id = aus_aty_id) > ' . $_SESSION['aty_level'] .' )' .
		             ' OR   (aus_jurisdiction_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = ' . $_SESSION['jurisdiction_id'] . '))' .
		             ' OR   (aus_jurisdiction_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = ' . $_SESSION['jurisdiction_id'] . '))))';
        }
    
        
      
      ////////////////////////////////////////////////////////////
      ////Define log string
      
      $prev_vals = getAdminUser($id);	//get old values
      
      $prev_vals['username'] = escapeSingleQuotes($prev_vals['username']);
      $prev_vals['email']    = escapeSingleQuotes($prev_vals['email']);
      $prev_vals['name']     = escapeSingleQuotes($prev_vals['name']);
      
      
      $log = "Admin User ID:$curr_aus_id=$curr_aus_id|";
      
      if ( $prev_vals['username'] != $username )
      	$log .= "aus_username:".$prev_vals['username']."=$username|";

      if ( $set_pwd )
      	if ( $prev_vals['password'] != $md5_password )
      		$log .= "aus_password:".$prev_vals['password']."=$md5_password|";
      		
      if ( $prev_vals['name'] != $name )
      	$log .= "aus_name:".$prev_vals['name']."=$name|";
      	
      if ( $prev_vals['email'] != $email )
      	$log .= "aus_email:".$prev_vals['email']."=$email|";
      	
      if ( $prev_vals['access'] != $access )
      	$log .= "aus_access:".addslashes($prev_vals['access'])."=$access|";
      
      if ( $prev_vals['aty_id'] != $user_type )
      	$log .= "aus_aty_id:".$prev_vals['aty_id']."=$user_type|";
     
      if($prev_vals['cou_id'] != $cou_id)
         $log .= "aus_cou_id:" . $prev_vals['cou_id'] . "=" . $cou_id . "|";
      	
      if ( $log) 
      {	 $log = substr($log, 0, strlen($log)-1);	       
      	 doAdminUserLog($_SESSION['admin_id'], 'modify_admin_user', $log, $pun_id="NULL");
      }
      
      //execute sql
      $rs = $dbh->exec($sql);

      
      $alert_message = "Administrator details updated";

      //jalert($alert_message);
      return TRUE;
   }
   return FALSE;
}


$op = $_GET['op'];

if ( 'Update' == $op )
{
    $id = (int)$_POST['id'];
            
    if ( 1 == $id )
    {
        $display_form = 'user_display_form.php';
        break;
    }
       
    $updated = update_admin_user ($_POST['id'], $_POST['username'], $_POST['email'], $_POST['name'], $_POST['access'], $_POST['user_type'], $_POST['jurisdiction'], $_POST['user_country'], $_POST['set_pwd'], $_POST['password'], $_POST['password_confirm']);
			
	if ( TRUE == $updated )
	{ 	
        $alert = $lang->getLang('Details of administrator user " % " updated', $_POST['name']);
        $display_form = 'user_display_form.php';		            
    }
	else
	{
        $username               = $_POST['username'];
        $email                  = $_POST['email'];
        $name                   = $_POST['name'];
        $access                 = $_POST['access'];
        $user_type              = $_POST['user_type'];
        $aus_jurisdiction_id    = $_POST['jurisdiction'];
               	               	
        if ( $user_type )  
        {                   	
            //get the list of jurisdictions in same class selected admin user typr
            $sql = 'select aty_jurisdiction_class from admin_user_type where aty_id = ' .$user_type;
            $rs  = $dbh->exec($sql); 
            $row = $rs->next();
            	    
            if ( $row )
                $aty_jurisdiction_class = $row['aty_jurisdiction_class'];
        }
        $display_form = 'user_edit_form.php';
    }
}

?>
<? include ('header.inc'); ?> 
<div class=bodyHD><?=$lang->getLang("Edit Administrator User")?> <?=$name?><?=$name?></div>
<br />
<? showErrors($only_global=TRUE);?>
<form action="<?=$_SERVER['PHP_SELF'];?>" method="POST">
<input type="hidden" name="id" value="<?=$id?>">
<table cellpadding=0 cellspacing="1" border=0>
   <tr valign="top">
      <td>
         <table cellpadding=4 cellspacing="1" border=0>
            <tr valign="top" width="150">
               <td class=label><?=$lang->getLang("Username")?></td>
               <td <? form_td_style('username'); ?>><input type="text" name="username" value="<?=replace_quotes($username)?>" size=30>
                  <?php echo err_field('username'); ?>               
               </td>
            </tr>
            <tr valign="top">
               <td class=label><?=$lang->getLang("Password")?> </td>
               <td <? form_td_style('password'); ?>><input type="password" name="password" value="<?=replace_quotes($password)?>" size=30 disabled="disabled" />
                   <br />
                   <input type="checkbox" name="set_pwd" <? if ($set_pwd ) echo "checked";?> onClick="if ( this.checked) { this.form.password.disabled=false; this.form.password_confirm.disabled=false; } else { this.form.password.disabled=true;this.form.password_confirm.disabled=true;}"> Set a new password
                  <?php echo err_field('password'); ?>
               </td>	
            </tr>
            <tr valign="top">
               <td class=label><?=$lang->getLang("Confirm Password")?> </td>
               <td <? form_td_style('password_confirm'); ?>><input type="password" name="password_confirm" value="<?=replace_quotes($password_confirm)?>" size=30 disabled="disabled" /> 
                   <?php echo err_field('password_confirm'); ?>            
               </td>
            </tr>
            <tr valign="top">
               <td class=label><?=$lang->getLang("Full Name")?></td>
               <td <? form_td_style('full_name'); ?>><input type="text" name="name" value="<?=replace_quotes($name)?>" size=30>
               <?php echo err_field('full_name'); ?>
               </td>
            </tr>
            <tr valign="top">
               <td class=label><?=$lang->getLang("Email")?> </td>
               <td <? form_td_style('email'); ?>><input type="text" name="email" value="<?=replace_quotes($email)?>" size=30>
               <?php echo err_field('email'); ?></td>
            </tr>
            <tr valign="top">
               <td class=label><?=$lang->getLang("Country")?> </td>
               <td <? form_td_style('user_country'); ?>>
                   <?php countrySelect('user_country', '', $user_country);?>
                   <?php echo err_field('user_country'); ?>
               </td>    
            </tr>
            <tr valign="top">
               <td class=label><?=$lang->getLang("Type")?> </td>
               <td <? form_td_style('user_type'); ?> >
                    <?php echo get_user_type_select($user_type); ?>
                    <?php echo err_field('user_type'); ?>
               </td>    
            </tr>
           <tr valign=top>
				<td class=label>
				    <div class="label" id="jurisdiction_label">
				    
                <?php           
                   
                   if ( 'region' == $aty_jurisdiction_class ) 
                       echo 'Region';
                   elseif ( 'district' == $aty_jurisdiction_class ) 
                       echo 'District';
                   elseif ( 'club' == $aty_jurisdiction_class ) 
                       echo 'Betting Club ';
                ?>               
                   </div>
				</td>
				<td <? form_td_style('jurisdiction'); ?>>
				
                    <div id="jurisdiction_select" class="content">
<?php 
    
   echo get_jurisdiction_select($aty_jurisdiction_class, $aus_jurisdiction_id);

?>
                      </div>
                  <?php echo err_field('jurisdiction', $no_br=true); ?>
				</td>
			</tr>   
			<tr valign="top">
               <td class=label><?=$lang->getLang("Access")?></td>
               <td <? form_td_style('access'); ?>>
                  <input type="radio" name="access" value="ALLOW" <? if ('ALLOW' == $access) echo 'checked'; ?>><?=$lang->getLang("Allow")?>
                  &nbsp;&nbsp;&nbsp;
                  <input type="radio" name="access" value="DENY" <? if ('DENY' == $access) echo 'checked'; ?>><?=$lang->getLang("Deny")?>
                  <?php echo err_field('access'); ?>
               </td>
            </tr>
         </table>
      </td>
      <td width="20">
      &nbsp;
      </td>
      <td  valign=top colspan="2" width="100">
            
            <input type="submit" id="sub_update" name="op" style="width:70px;margin-bottom:2px;" value="<?=$lang->getLang("Update")?>" />
    	    <br />   
    	    <input type="button" id="btn_refresh" name="op" style="width:70px;margin-bottom:2px" value="<?=$lang->getLang("Refresh")?>" onClick="doRefresh(this.form)" />
    	    <br />
    	    <input type="button" id="btn_back" name="op" style="width:70px;margin-bottom:2px" value="<?=$lang->getLang("Back to List")?>" onClick="window.location='/admin_users/admin_users.php'" />
    	    <br />       
      </td>     
   </tr>
</table>
</form>
<? include 'footer.inc';?>
