<?php
check_access('create_skins', true);
if ( 'club' != $_SESSION['jurisdiction_class']) {
  include (include_content('jurisdiction_filters.php'));
}
?>
<style>
  a.red{
    color:Red;
  }
  a.green{
    color: Green;
  }
</style>
<div class="bodyHD">Skins Manager</div>
<div class="bodyHD3">
  <?=getClassLink(refFormPage($_GET["page"] . "&action=new"), "New Skin")?> | <?=getClassLink(refFormPage($_GET["page"]), "Return to List")?>
</div>
<br/><br/>

<?php

define("WEBSITE_ROOT", realpath(WEB_ROOT . "/../"));
define("SKINS_PATH",   realpath(WEBSITE_ROOT . "/skins/"));



$skin = getSkinDetailsByJurisdiction($_SESSION["jurisdiction_id"], $_SESSION["jurisdiction_class"]);

$action = globalize("action");

if(empty($action)){
  $action = "show";
}

switch ($action){
  case "save":
    checkAndSaveData($_POST);
    break;
  case "update":
    $jurisdiction_id = $_POST["jurisdiction"];
    $skin_id         = $_POST["skn_id"];
    $skin_name       = $_POST["skn_name"];
    $skin_url        = $_POST["skn_url"];
    $skin_foldername = $_POST["skn_foldername"];

    settype($jurisdiction_id, "integer");
    settype($skin_id, "integer");

    if($jurisdiction_id == 0){
      addError("jurisdiction", "Select a valid jurisdiction");
    }

    if(strlen($skin_name) > 20){
      addError("skn_name", "Skin name too long, lenght must be 20 chars max");
    }

    if(!areErrors()){
      $sql = "SELECT COUNT(*) AS skins FROM skins WHERE lower(skn_name) = lower('" . $dbh->escape($skin_name) . "') AND skn_id != $skin_id";
      $rs  = $dbh->exec($sql);
      $row = $rs->next();

      if($row["skins"] > 0){
        addError("skn_name", "Chose another skin name, $skin_name is taken");
      }
    }

    if(!areErrors()){
      $dbh->begin();
      $sql = "UPDATE skins SET skn_jur_id = $jurisdiction_id, skn_url = '" . $dbh->escape($skin_url) . "', skn_foldername = '" . $dbh->escape($skin_foldername) . "', skn_name = '" . $dbh->escape($skin_name) . "' WHERE skn_id = $skin_id";
      $rs = $dbh->exec($sql);
      if($rs === 1){
        printInfoMessage("Skin Updated!");
        $dbh->commit();
      }else{
        addError("", "Impossible to update skin, maybe a duplicate");
        $dbh->rollback();
      }
    }
    showSkinDetails($skin_id);
    break;
  case "enable":
    $skin_id = $_REQUEST["skn_id"];
    $sql = "UPDATE skins SET skn_status = '1' WHERE skn_id = $skin_id";
    $rs = $dbh->exec($sql);
    if($rs === 1){
      printInfoMessage("Skin Updated!");
    }else{
      addError("", "Impossible to update skin<!--$sql-->");
    }
    showSkinsList();
    break;
  case "disable":
    $skin_id = $_REQUEST["skn_id"];
    $sql = "UPDATE skins SET skn_status = '0' WHERE skn_id = $skin_id";
    $rs = $dbh->exec($sql);
    if($rs === 1){
      printInfoMessage("Skin Updated!");
    }else{
      addError("", "Impossible to update skin<!--$sql-->");
    }
    showSkinsList();
    break;
  case "show":
    showSkinsList();
    break;
  case "details":
    $skin_id = globalize("skn_id");
    settype($skin_id, "integer");
    showSkinDetails($skin_id);
    break;
  case "new":
    $default = array();
    showSkinEditPage($default);
    break;
}

function showSkinDetails($skin_id){
  global $dbh;
  $sql = "SELECT
            skn_id, skn_name, skn_url, skn_foldername, skn_status, owner.jur_id owner_jur_id, owner.jur_name owner_jur_name, owner.jur_class owner_jur_class, club.jur_id club_id, club.jur_name club_name
          FROM
            skins
          JOIN
            jurisdiction owner ON skn_jur_id = owner.jur_id
          JOIN
            jurisdiction club ON skn_jur_id = club.jur_id
          WHERE skn_id = $skin_id";

  $rs = $dbh->exec($sql);
  showErrors();
  if($rs->getNumRows() > 0){
    $row = $rs->next();
    ?>
    <form name="skins_editor" action="<?=refFormPage($_GET["page"] . "&action=update")?>" method="POST">
      <table cellpadding="4" cellspacing="1" border=0>
        <tr cellpadding="4">
          <th colspan="2" class="formheading">Skin Details</th>
        </tr>

        <tr valign="top">
          <td class="label">Skin ID</td>
          <td class="content">
          <?=$row["skn_id"]?>
          <input name="skn_id" type="hidden" value="<?=$row["skn_id"]?>"/>
          </td>
        </tr>

        <tr valign="top">
          <td class="label">Skin Name</td>
          <td <? form_td_style('skn_name'); ?>><input type="text" name="skn_name" value="<?=eregi_replace("''", "", $row["skn_name"])?>" size="30">
            <?php echo err_field('skn_name'); ?>
          </td>
        </tr>

        <tr valign="top">
          <td class="label">Skin Website</td>
          <td <? form_td_style('skn_url'); ?>><input type="text" name="skn_url" value="<?=eregi_replace("''", "", $row["skn_url"])?>" size="30">
            <?php echo err_field('skn_url'); ?>
          </td>
        </tr>

        <tr valign="top">
          <td class="label">Skin Folder Name</td>
          <td class="content"><?php getSkinsSelect($row["skn_foldername"]) ?></td>
        </tr>

        <?php
        $jurisdiction_level = $row["owner_jur_class"];
        $jurisdiction_id    = $row["owner_jur_id"];
        ?>

        <tr valign=top>
          <td class=label>Jurisdiction</td>
          <td <?form_td_style ('jurisdiction')?>>
            <?php if($jurisdiction_level == 'casino') : ?>
              Casino
              <input type="hidden" name="jurisdiction" value="1"/>
            <?php else : ?>
              <table cellpadding="0" cellspacing="0" border="0">
              <tr>
                <td>
                   <select id="jurisdiction_level" name="jurisdiction_level" onChange="change_jurisdiction(this)">
                       <option value="club"<?if ( 'club' == $jurisdiction_level ) echo 'selected="selected"'; ?>>Betting Club</option>
                      <?php if ( 'casino' == $_SESSION['jurisdiction_class'] || 'nation' == $_SESSION['jurisdiction_class'] || 'region' == $_SESSION['jurisdiction_class'] ) { ?>
                              <option value="district"<?if ( 'district' == $jurisdiction_level ) echo 'selected="selected"'; ?>>District</option>
                      <?php } ?>
                      <?php if ( 'casino' == $_SESSION['jurisdiction_class'] || 'nation' == $_SESSION['jurisdiction_class'] ){ //show region option ?>
                               <option value="region"<?if ( 'region' == $jurisdiction_level || ! $jurisdiction_level ) echo 'selected="selected"'; ?>>Region</option>
                      <?php } ?>
                      <?php if ( 'casino' == $_SESSION['jurisdiction_class'] ){ //show region option ?>
                               <option value="nation"<?if ( 'nation' == $jurisdiction_level || ! $jurisdiction_level ) echo 'selected="selected"'; ?>>Nation</option>
                      <?php } ?>
                   </select>
                 </td>
                 <td>
                   <div id="jurisdiction_select" class="content">
                     <?php
                     echo get_jurisdiction_select($jurisdiction_level, $jurisdiction_id);
                     ?>
                   </div>
                 </td>
               </tr>
               </table>
             <?php endif; ?>
             <?php echo err_field('jurisdiction', $no_br=true); ?>
           </td>
        </tr>

        <tr valign="top">
          <td class="label">Skin Club</td>
          <td class="content"><?= $row["club_id"] . ", " . $row["club_name"]?></td>
        </tr>

        <tr>
          <td class="label" valign="top">Admin Users</td>
          <td class="content">
          <ol>
          <?php
          $sql = "select aus_id, aus_name, aus_username from admin_user WHERE aus_jurisdiction_id = $jurisdiction_id";
          $rs  = $dbh->exec($sql);

          if($rs->hasNext()){
            while ($rs->hasNext()) {
              $row = $rs->next();
              ?>
                <li>
                <?= getLink("/admin_users/admin_user_edit.php?id=" . $row["aus_id"], $row["aus_name"] . " (user: " . $row["aus_username"] . ")") ?>
                </li>
              <?php
            }
          }else{
            ?>
            <li>No admin user manage this skin</li>
            <?php
          }

          ?>
          </ol>
          </td>
        </tr>

        <tr valign="top">
          <td colspan="2" align="center">
          <input type="reset"/>&nbsp;<input type="submit"/>
          </td>
        </tr>

      </table>
    </form>

    <?php
  }else{
    addError("", "No skin data found");
    showErrors();
  }
}

function checkAndSaveData($data){
  global $dbh;

  $newStructure = ($data["structure"] == "new");

  if($newStructure){
    $valid_props = array(
      array("name" => "skn_name",             "type" => "string",  "beautyname" => "Skin Name",          "nullable" => false),
      array("name" => "jur_code",             "type" => "string",  "beautyname" => "Jurisdiction Code",  "nullable" => false),
      array("name" => "skn_url",              "type" => "string",  "beautyname" => "Skin Url",           "nullable" => false),
      array("name" => "skn_foldername",       "type" => "string",  "beautyname" => "Skin Folder",        "nullable" => false),
      array("name" => "aus_password",         "type" => "string",  "beautyname" => "Password",           "nullable" => false),
      array("name" => "aus_password1",        "type" => "string",  "beautyname" => "Password",           "nullable" => false),
      array("name" => "aus_username",         "type" => "string",  "beautyname" => "Admin User Username","nullable" => false),
      array("name" => "aus_name",             "type" => "string",  "beautyname" => "Admin User Name",    "nullable" => false),
      array("name" => "aus_email",            "type" => "string",  "beautyname" => "Admin User Email",   "nullable" => false),
      array("name" => "aus_country",          "type" => "string",  "beautyname" => "Admin User Country", "nullable" => false),
      array("name" => "jur_vat_code",         "type" => "integer", "beautyname" => "VAT Code",           "nullable" => false),
      array("name" => "jur_address1",         "type" => "string",  "beautyname" => "Address Line 1",     "nullable" => false),
      array("name" => "jur_address2",         "type" => "string",  "beautyname" => "Address Line 2",     "nullable" => true),
      array("name" => "aus_city_suburb",      "type" => "string",  "beautyname" => "City",               "nullable" => false),
      array("name" => "jur_postcode_zip",     "type" => "string",  "beautyname" => "Zip Code",           "nullable" => false),
      array("name" => "jur_phone",            "type" => "string",  "beautyname" => "Phone Number",       "nullable" => true),
      array("name" => "structure",            "type" => "string",  "beautyname" => "Structure",          "nullable" => false)
    );
  }else{
    //old structure
    $valid_props = array(
      array("name" => "skn_name",             "type" => "string",  "beautyname" => "Skin Name",          "nullable" => false),
      array("name" => "jur_code",             "type" => "string",  "beautyname" => "Jurisdiction Code",  "nullable" => false),
      array("name" => "skn_url",              "type" => "string",  "beautyname" => "Skin Url",           "nullable" => false),
      array("name" => "skn_foldername",       "type" => "string",  "beautyname" => "Skin Folder",        "nullable" => false),
      array("name" => "jur_vat_code",         "type" => "integer", "beautyname" => "VAT Code",           "nullable" => false),
      array("name" => "jurisdiction",         "type" => "integer", "beautyname" => "Jurisdiction",       "nullable" => false),
      array("name" => "jurisdiction_level",   "type" => "string",  "beautyname" => "Jurisdiction Level", "nullable" => false),
      array("name" => "structure",            "type" => "string",  "beautyname" => "Structure",          "nullable" => false)
    );
  }

  $properties = array();


  //Controllo dei tipi

  foreach ($valid_props as $prop){
    if(checkType($_POST[$prop["name"]], $prop["type"], $prop["rules"], $prop["nullable"])){
      $properties[$prop["name"]] = trim($_POST[$prop["name"]]);
    }else{
      addError($prop["name"], $prop["beautyname"] . " '" . $_POST[$prop["name"]] . "' must be a " . $prop["type"]);
    }
  }

  //formattazione dati
  $properties["jur_code"] = strtolower($properties["jur_code"]);

  if(!eregi("https?://", $properties["skn_url"])){
    $properties["skn_url"] = "http://" . $properties["skn_url"];
  }

  $pattern = '/^[a-zA-Z]+[:\/\/]+[A-Za-z0-9\-_]+\\.+[A-Za-z0-9\.\/%&=\?\-_]+$/i';
  if(!preg_match($pattern, $properties["skn_url"])){
    addError("skn_url", "Url '" . $properties["skn_url"] . "' doesn't seem to be a valid url");
  }

  $pattern = '#[a-z0-9!\#$%&\'*+/=?^_`{|}~-]+(?:\.[a-z0-9!\#$%&\'*+/=?^_`{|}~-]+)*@(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?#';

  if($newStructure && !preg_match($pattern, $properties["aus_email"])){
    addError("aus_email", "Email '" . $properties["aus_email"] . "' doesn't seem to be a valid email");
  }

  if($newStructure && !preg_match("/[a-z]{3}/i", $properties["jur_code"])){
    addError("jur_code", "The code must be by 3 letters");
  }

  //password
  if($newStructure && $properties["aus_password"] != $properties["aus_password1"]){
    addError("aus_password", "Password and password repeat are different");
    addError("aus_password1", "Password and password repeat are different");
  }

  if(strlen($properties["skn_name"]) > 20){
    addError("skn_name", "Skin name too long, lenght must be 20 chars max");
  }

  if(!areErrors()){
    //controlli approfonditi
    $sql = "SELECT COUNT(*) AS skins FROM skins WHERE lower(skn_name) = lower('" . $dbh->escape($properties["skn_name"]) . "')";
    $rs  = $dbh->exec($sql);
    $row = $rs->next();

    if($row["skins"] > 0){
      addError("skn_name", "Chose another skin name, " . $properties["skn_name"] . " is taken");
    }


    //controllo codice jurisdizione e nome giurisdizione
    $sql = "SELECT jur_name, jur_code FROM jurisdiction WHERE LOWER(jur_code) = LOWER('" . $dbh->escape($properties["jur_code"], "text") . "') OR LOWER(jur_name) = LOWER('" . $dbh->escape($properties["jur_name"]) . "')";
    $rs  = $dbh->exec($sql);

    while ($rs->hasNext()){
      $row = $rs->next();
      if($row["jur_name"] == $properties["skn_name"]){
        addError("skn_name", "Chose another skin name, '" . $properties["skn_name"] . "' is taken by another jurisdiction");
      }
      if($row["jur_code"] == $properties["jur_code"]){
        addError("jur_code", "Chose another jurisdiction code, '" . $properties["jur_code"] . "' is taken");
      }
    }

    if($newStructure){
      $sql = "SELECT aus_username, aus_email FROM admin_user WHERE LOWER(aus_username) = LOWER('" . $dbh->escape($properties["aus_username"]) . "') OR LOWER(aus_email) = LOWER('" . $dbh->escape($properties["aus_email"]) . "')";
      $rs  = $dbh->exec($sql);

      while ($rs->hasNext()){
        $row = $rs->next();
        if($row["aus_username"] == $properties["aus_username"]){
          addError("aus_username", "Chose another admin user username, '" . $properties["aus_username"] . "' is taken");
        }
        if($row["aus_email"] == $properties["aus_email"]){
          addError("aus_email", "Chose another admin user email, '" . $properties["aus_email"] . "' is taken");
        }
      }
    }

    if(DOT_IT){
      if(!preg_match("/^[0-9]{11}$/i", $properties["jur_vat_code"])){
        addError("jur_vat_code", "Partita IVA non valida");
      }

      /*
      $sql = "SELECT COUNT(*) AS jurisdictions FROM jurisdiction WHERE jur_vat_code = '" . $dbh->escape($properties["jur_vat_code"]) . "' AND jur_class = 'club'";
      $rs  = $dbh->exec($sql);
      $row = $rs->next();

      if($row["jurisdictions"] > 0){
        addError("jur_vat_code", "Chose another jurisdiction VAT code, code " . $properties["jur_vat_code"] . " is associated to a club");
      }
      */

      if(!areErrors()){
        try{
          $url = 'http://ec.europa.eu/taxation_customs/vies/api/checkVatPort?wsdl';
          $vies = new SoapClient($url);
          $a = array(
                 countryCode => "IT",
                 vatNumber => $properties["jur_vat_code"]
               );

          $ret = $vies->checkVat($a);

          if($ret->valid != "1"){
            addError("jur_vat_code", "Invalid VAT code");
          }
        }catch (Exception $e){
          addError("jur_vat_code", "Problem contacting central database of VAT code, retry in a minute");
        }
      }


    }

  }

  if(!areErrors()){
    //creazione club skin
    $dbh->begin();

    $skin_club_jur_id = $dbh->nextSequence("JUR_ID_SEQ");
    $skin_code        = $dbh->nextSequence("SKN_JUR_CODE_SEQ");

    if(strlen($skin_code) < 3){
      $cycle = 3 - strlen($skin_code);
      $appo  = "";
      for($i = 0 ; $i < $cycle ; $i++){
        $appo .= $properties["jur_code"][$i];
      }
      $skin_code = $appo . $skin_code;
    }

    //$skin_code
    $sql = "SELECT jur_id FROM jurisdiction WHERE LOWER(jur_name) = 'internet' AND jur_class = 'district'";
    $jur_parent_id = $dbh->queryOne($sql);
    $sql = "INSERT INTO
              jurisdiction (jur_id, jur_name, jur_class,
                            jur_parent_id, jur_address1,
                            jur_address2, jur_postcode_zip,
                            jur_notes, jur_phone,
                            jur_code, jur_vat_code)
            VALUES
                           ($skin_club_jur_id, '" . $dbh->escape($properties["skn_name"]) . "', 'club',
                            $jur_parent_id, '" . $dbh->escape($properties["jur_address1"] . "\n" . $properties["jur_address2"]) . "',
                            '" . $dbh->escape($properties["aus_city_suburb"]) . "', '" . $dbh->escape($properties["jur_postcode_zip"]) . "',
                            '" . $dbh->escape($properties["skn_name"]) . " Skin Manager', '" . $dbh->escape($properties["jur_phone"]) . "',
                            '" . $dbh->escape($skin_code) . "', '" . $dbh->escape($properties["jur_vat_code"]) . "'
                           )";

          
    $rs = $dbh->exec($sql);

    if($rs === 1){
      if($newStructure){
        $region_jur_id = $dbh->nextSequence("JUR_ID_SEQ");
        
        $sql = "SELECT jur_id FROM jurisdiction WHERE LOWER(jur_name) = 'skins' AND jur_class = 'nation'";
        $jur_parent_id = $dbh->queryOne($sql);
        $sql = "INSERT INTO
                  jurisdiction (jur_id, jur_name, jur_class,
                                jur_parent_id, jur_address1,
                                jur_address2, jur_postcode_zip,
                                jur_notes, jur_phone,
                                jur_code, jur_vat_code)
                VALUES
                               ($region_jur_id, '" . $dbh->escape($properties["skn_name"]) . "', 'region',
                                $jur_parent_id, '" . $dbh->escape($properties["jur_address1"] . "\n" . $properties["jur_address2"]) . "',
                                '" . $dbh->escape($properties["aus_city_suburb"]) . "', '" . $dbh->escape($properties["jur_postcode_zip"]) . "',
                                '" . $dbh->escape($properties["skn_name"]) . " Skin Manager', '" . $dbh->escape($properties["jur_phone"]) . "',
                                '" . $dbh->escape($properties["jur_code"]) . "', '" . $dbh->escape($properties["jur_vat_code"]) . "'
                               )";

        $rs = $dbh->exec($sql);
        $success = ($rs === 1);
      }else{
        $region_jur_id = $properties["jurisdiction"];
        $success       = true;
      }

      if($success){
        $skin_id = $dbh->nextSequence("SKN_ID_SEQ");
        $sql = "INSERT INTO
                  skins (skn_id, skn_name, skn_url, skn_foldername, skn_status, skn_jur_id, skn_internet_club_id)
                VALUES
                  ($skin_id, '" . $dbh->escape($properties["skn_name"]) . "', '" . $dbh->escape($properties["skn_url"]) . "',
                  '" . $dbh->escape($properties["skn_foldername"]) . "', '1', $region_jur_id, $skin_club_jur_id
                  )";

        $rs = $dbh->exec($sql);
        if($rs === 1){
          $rs = 1;
          if($newStructure){
            $admin_user_id = $dbh->nextSequence("AUS_ID_SEQ");
            
            $sql = "SELECT cou_id FROM country WHERE cou_code = '" . $dbh->escape($properties["aus_country"]) . "'";
            $country_code = $dbh->queryOne($sql);
            
            $sql = "SELECT aty_id FROM admin_user_type WHERE aty_code = 'SKINMANAGER'";
            $aty_id_code = $dbh->queryOne($sql);
            
            $sql = "INSERT INTO
                      admin_user (aus_id, aus_name, aus_username,
                                  aus_password, aus_aty_id, aus_access,
                                  aus_email, aus_address1, aus_address2, aus_city_suburb,
                                  aus_state_province, aus_postcode_zip,
                                  aus_phone_work, aus_cou_id, aus_notes,
                                  aus_jurisdiction_id
                                  )
                      VALUES(
                        $admin_user_id, '" . $dbh->escape($properties["aus_name"]) . "', '" . $dbh->escape($properties["aus_username"]) . "',
                        '" . md5($properties["aus_password"]) . "', $aty_id_code, 'ALLOW',
                        '" . $dbh->escape($properties["aus_email"]) . "', '" . $dbh->escape($properties["aus_address1"]) . "', '" . $dbh->escape($properties["aus_address2"]) . "', '" . $dbh->escape($properties["aus_city_suburb"]) . "',
                        '" . $dbh->escape($properties["aus_country"]) . "', '" . $dbh->escape($properties["jur_postcode_zip"]) . "',
                        '" . $dbh->escape($properties["jur_phone"]) . "',$country_code, '" . $dbh->escape($properties["skn_name"]) . " Skin Manager',
                        $region_jur_id
                      )";
            $rs = $dbh->exec($sql);
          }
          if($rs === 1){
            $dbh->commit();
            ?>
            <div class="bodyHD2">Skin successful created!</div>
            <div class="bodyHD1">Skin Betting club for website register is: <strong><?=$skin_club_jur_id?></strong></div>
            <?php
            showSkinDetails($skin_id);
            exit();
          }else{
            addError("", "Problem creating admin user <!--$sql-->");
          }
        }else{
          addError("", "Problem creating skin <!--$sql-->");
        }
      }else{
        addError("", "Problem creating region <!--$sql-->");
      }
    }else{
      addError("", "Problem creating club <!--$sql-->");
    }
    if(areErrors()){
      $dbh->rollback();
    }
  }

  if(areErrors()){
    showErrors();
    showSkinEditPage($properties);
  }

}




function showSkinEditPage($rs=null){
  ?>
  <script type="text/javascript">
  function getCheckedValue(radioObj) {
  	if(!radioObj)
  		return "";
  	var radioLength = radioObj.length;
  	if(radioLength == undefined)
  		if(radioObj.checked)
  			return radioObj.value;
  		else
  			return "";
  	for(var i = 0; i < radioLength; i++) {
  		if(radioObj[i].checked) {
  			return radioObj[i].value;
  		}
  	}
  	return "";
  }

  function refreshSkinForm($radioButton){
    $selected = getCheckedValue($radioButton);

    $types = ["old", "new"];

    for(var $i = 0 ; $i < $types.length ; $i++){
      $form = document.getElementById($types[$i]);

      if($form != undefined){
        $form.style.display = "none";

        if($types[$i] == $selected){
          $form.style.display = "table";
        }

        $form.width = "100%";
      }else{
        alert("Not found " + $types[$i]);
      }

    }

  }
  </script>
  <div class="bodyHD2">Welcome to skins creator</div>

  <form name="skins_editor" action="<?=refFormPage($_GET["page"] . "&action=save")?>" method="POST">
  <table cellpadding=0 cellspacing="1" border=0>
      <tr>
      <td>
        <table cellpadding="4" cellspacing="1" border=0>

          <tr cellpadding="4">
            <th colspan="2" class="formheading">Skin Details</th>
          </tr>

          <tr valign="top">
            <td class="label">Skin Name</td>
            <td <? form_td_style('skn_name'); ?>><input type="text" name="skn_name" value="<?=eregi_replace("''", "", $rs["skn_name"])?>" size="50">
              <?php echo err_field('skn_name'); ?>
            </td>
          </tr>

          <tr valign="top">
            <td class="label">3 letters code</td>
            <td <? form_td_style('jur_code'); ?>><input type="text" name="jur_code" value="<?=eregi_replace("''", "", $rs["jur_code"])?>" size="5" maxlength="3">
              <?php echo err_field('jur_code'); ?>
            </td>
          </tr>

          <tr valign="top">
            <td class="label">Skin Url</td>
            <td <? form_td_style('skn_url'); ?>><input type="text" name="skn_url" value="<?=eregi_replace("''", "", $rs["skn_url"])?>" size="50">
              <?php echo err_field('skn_url'); ?>
            </td>
          </tr>

          <tr valign="top">
            <td class="label">Skin Folder Name</td>
            <td>
            <?php
            getSkinsSelect($rs["skn_foldername"]);
            ?>
            </td>
          </tr>

          <tr valign="top">
            <td class="label">VAT Code</td>
            <td <? form_td_style('jur_vat_code'); ?>><input type="text" name="jur_vat_code" value="<?=eregi_replace("''", "", $rs["jur_vat_code"])?>" size="50">
              <?php echo err_field('jur_vat_code'); ?>
            </td>
          </tr>

          <tr valign="top">
            <td class="label">Structure<br/></td>
            <td>
              <input type="radio" name="structure" onchange="refreshSkinForm(this)" value="old" <?=(($rs["structure"] != "new")?("checked"):(""))?>>Use an existing structure</input><br/>
              <input type="radio" name="structure" onchange="refreshSkinForm(this)" value="new" <?=(($rs["structure"] == "new")?("checked"):(""))?>>Create Regional &amp; Administrator</input>
              <br/>
              <span style="font-size:9px">Regional under SKINS national</span>
            </td>
          </tr>
        </table>

        <table cellpadding="4" cellspacing="1" border=0 width="100%" id="old" style="<?=(($rs["structure"] != "new")?("display:table"):("display:none"))?>">
          <tr cellpadding="4">
            <th colspan="2" class="formheading">Existing Jurisdiction Details</th>
          </tr>
          <?php
            $jurisdiction_level = $rs["jurisdiction_level"];
            include(include_content('jurisdiction_filter_tabrow.php'));
          ?>
          <script type="text/javascript">
          var select = document.getElementById("jurisdiction_level");
          change_jurisdiction(select);
          </script>
        </table>

        <table cellpadding="4" cellspacing="1" border=0 id="new" style="<?=(($rs["structure"] == "new")?("display:table"):("display:none"))?>">
          <tr cellpadding="4">
            <th colspan="2" class="formheading">Administrator Details</th>
          </tr>

          <tr valign="top">
            <td class="label">Username</td>
            <td <? form_td_style('aus_username'); ?>><input type="text" name="aus_username" value="<?=eregi_replace("''", "", $rs["aus_username"])?>" size="50">
              <?php echo err_field('aus_username'); ?>
            </td>
          </tr>

          <tr valign="top">
            <td class="label">Password</td>
            <td <? form_td_style('aus_password'); ?>><input type="password" name="aus_password" value="<?=eregi_replace("''", "", $rs["aus_password"])?>" size="50">
              <?php echo err_field('aus_password'); ?>
            </td>
          </tr>

          <tr valign="top">
            <td class="label">Password Confirm</td>
            <td <? form_td_style('aus_password1'); ?>><input type="password" name="aus_password1" value="<?=eregi_replace("''", "", $rs["aus_password1"])?>" size="50">
              <?php echo err_field('aus_password1'); ?>
            </td>
          </tr>

          <tr valign="top">
            <td class="label">Complete Name</td>
            <td <? form_td_style('aus_name'); ?>><input type="text" name="aus_name" value="<?=eregi_replace("''", "", $rs["aus_name"])?>" size="50">
              <?php echo err_field('aus_name'); ?>
            </td>
          </tr>

          <tr valign="top">
            <td class="label">Email Address</td>
            <td <? form_td_style('aus_email'); ?>><input type="text" name="aus_email" value="<?=eregi_replace("''", "", $rs["aus_email"])?>" size="50">
              <?php echo err_field('aus_email'); ?>
            </td>
          </tr>

          <tr valign="top">
            <td class="label">Address Line 1</td>
            <td <? form_td_style('jur_address1'); ?>><input type="text" name="jur_address1" value="<?=eregi_replace("''", "", $rs["jur_address1"])?>" size="50">
              <?php echo err_field('jur_address1'); ?>
            </td>
          </tr>

          <tr valign="top">
            <td class="label">Address Line 2</td>
            <td <? form_td_style('jur_address2'); ?>><input type="text" name="jur_address2" value="<?=eregi_replace("''", "", $rs["jur_address2"])?>" size="50">
              <?php echo err_field('jur_address2'); ?>
            </td>
          </tr>

          <tr valign="top">
            <td class="label">City</td>
            <td <? form_td_style('aus_city_suburb'); ?>><input type="text" name="aus_city_suburb" value="<?=eregi_replace("''", "", $rs["aus_city_suburb"])?>" size="50">
              <?php echo err_field('aus_city_suburb'); ?>
            </td>
          </tr>

          <tr valign="top">
            <td class="label">Zip Code</td>
            <td <? form_td_style('jur_postcode_zip'); ?>><input type="text" name="jur_postcode_zip" value="<?=eregi_replace("''", "", $rs["jur_postcode_zip"])?>" size="10">
              <?php echo err_field('jur_postcode_zip'); ?>
            </td>
          </tr>

          <tr valign="top">
            <td class="label">Phone Number</td>
            <td <? form_td_style('jur_phone'); ?>><input type="text" name="jur_phone" value="<?=eregi_replace("''", "", $rs["jur_phone"])?>" size="10">
              <?php echo err_field('jur_phone'); ?>
            </td>
          </tr>
          <?php if(DOT_IT) : ?>
          <input type="hidden" name="aus_country" value="IT"/>
          <?php else : ?>
          <tr valign="top">
             <td class=label>Country</td>
             <td <? form_td_style('aus_country'); ?>>
                 <?php countrySelect('aus_country', '', $rs['aus_country']); ?>
                 <?php echo err_field('aus_country'); ?>
             </td>
          </tr>
          <?php endif; ?>
        </table>
        </td>
      </tr>
    </table>

    <table cellpadding="4" cellspacing="1" border=0>
      <tr valign="top">
        <td colspan="2" align="center">
        <input type="reset"/>&nbsp;<input type="submit"/>
        </td>
      </tr>
    </table>
  </form>

  <?php
}

function showSkinsList(){
  global $dbh;
  $sql = "SELECT skn_id, skn_name, skn_url, skn_status, COUNT(pun_id) AS punters, jur_class FROM skins JOIN jurisdiction ON skn_jur_id = jur_id LEFT JOIN punter ON pun_betting_club = skn_internet_club_id GROUP BY skn_id, skn_name, skn_url, skn_status, jur_class,jur_parent_id ORDER BY COALESCE(jur_parent_id,0) ASC";
  $rs = $dbh->exec($sql);

  $cols_arr = array("ID", "Name", "Website", "Players", "Active", "Details");

  $val_fmt_arr = array(
    'ID'        => 'return $rec["skn_id"];',
    'Name'      => 'return $rec["skn_name"] . " (" . $rec["jur_class"] . ")";',
    'Website'   => 'return getLink($rec["skn_url"], $rec["skn_url"], "", "_blank");',
    'Players'   => 'return $rec["punters"];',
    'Active'    => 'if($rec["skn_id"] != 0){return getStatus($rec);}else{return "Default";};',
    'Details'   => 'return getClassLink(refFormPage($_GET["page"] . "&action=details&skn_id="   . $rec["skn_id"]), "Details");'
  );

  $numrow = $rs->getNumRows();
  if($numrow > 0){
    $qp = new QueryPrinter($rs);
    $qp->printTable($rs, $cols_arr, "", $val_fmt_arr, null, 0, 0, "", $width="100%", 1, 2000, false);
  }else{
    addError("", "No skins found");
    showErrors();
  }

}

function getStatus($rec){
  if($rec["skn_status"] == '0'){
    return "<span>" . getClassLink(refFormPage($_GET["page"] . "&action=enable&skn_id="   . $rec["skn_id"]), "Disabled", "red") . "</span>";
  }else{
    return "<span>" . getClassLink(refFormPage($_GET["page"] . "&action=disable&skn_id="   . $rec["skn_id"]), "Enabled", "green") . "</span>";
  }
}

function getSkinsSelect($default=null){
  ?>
  <select name="skn_foldername">
  <?php
  $skinDir = opendir(SKINS_PATH);
  $skinsArray = array();

  while($file = readdir($skinDir)){
    if(!ereg("^[:\.]", $file)){
      array_push($skinsArray, $file);
    }
  }
  sort($skinsArray);
  foreach ($skinsArray as $skin) {
  	printOption("skn_foldername", $skin, ucfirst($skin), ($default == $skin));
  }
  ?>
  </select>
  <?php
}

?>