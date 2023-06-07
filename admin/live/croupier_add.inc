<script type="text/javascript">
    function comeToList(){
        location.href = "<?=$_SERVER['PHP_SELF']?>"+"?page=live/croupier&header_page=";
    }

    $(function () {
        $("#date").datepicker({
            changeMonth: true,
            changeYear:true,
            numberOfMonths: 1,
            maxDate: '-18Y',
            onSelect: function( selectedDate ) {
                dateMax = $("#date").datepicker("getDate");
                var m = dateMax.getMonth();
                var d= dateMax.getDate();
                zodiac=birthdaySign(m,d);
                $('#shown_zodiac').empty().append(''+zodiac+'');
                $('input#zodiac').val(''+zodiac+'');
            }
        });
    });


    function birthdaySign(m, d){
        var sign, zodiac= [
            [20, 'Aquarius'],
            [19, 'Pisces'],
            [21, 'Aries'],
            [20, 'Taurus'],
            [21, 'Gemeni'],
            [22, 'Cancer'],
            [23, 'Leo'],
            [23, 'Virgo'],
            [23, 'Libra'],
            [23, 'Scorpio'],
            [22, 'Sagittarius'],
            [22, 'Capricorn']
        ];
        if(m== 0 && d<20) return zodiac[11].slice(1);
        else sign= zodiac[m];
        if(d<sign[0]) sign= zodiac[m-1];
        return sign.slice(1);
    }

</script>
<?php
check_access('live_croupier_add', $redirect=true);

function formValidate($name, $surname, $username,$date, $passwd,$hobby,$zodiac,$file){
    global $dbh,$lang;
    if($name == ""){
        addError("",$lang->getLang("Empty name"));
    }
    if(!preg_match('/^[a-zA-Z]{1,30}$/', $name)){
        addError("",$lang->getLang("The name has to contain only letters"));
    }
    if($surname == ""){
        addError("",$lang->getLang("Empty Last Name"));
    }
    if(!preg_match('/^[a-zA-Z]{1,30}$/', $surname)){
        addError("",$lang->getLang("The last name has to contain only letters"));
    }
    if($username == ""){
        addError("",$lang->getLang("Empty username"));
    }
    if($file== ""){
        addError("",$lang->getLang("Please choose a file"));
    }
    if($hobby== ""){
        addError("",$lang->getLang("Please enter at least one hobby"));
    }
    if($zodiac== ""){
        addError("",$lang->getLang("Please enter the zodiac"));
    }
    if ((($_FILES["file"]["type"] == "image/gif")
        || ($_FILES["file"]["type"] == "image/jpeg")
        || ($_FILES["file"]["type"] == "image/jpg")))
    {
        if ($_FILES["file"]["error"] > 0)
        {
            addError(""," Error Code:". $file['error']);

        }
    }
    else
    {
        addError("",$lang->getLang("Invalid file!Please choose another."));
    }

    //		}else{
//		
//			$sql = "select count(*) from croupier where LOWER(crp_username) = LOWER('$username')";
//			$count = $dbh->queryOne($sql);
//			settype($count,'integer');
//			if($count > 0){
//				addError("","L'username gia' esiste per un'altra croupier");
//			}
//		}
    if(strlen($passwd) < 6 || strlen($passwd) > 6){
        addError("",$lang->getLang("The password has to contain at least 6 characters"));
    }
    if(!preg_match('/^[a-zA-Z0-9]{1,30}$/', $username)){
        addError("",$lang->getLang("The username has to contain only letters"));
    }
    if(!preg_match('/^[0-9]{4}-[0-9]{2}-[0-9]{2}$/', $date)){
        addError("",$lang->getLang("Wrong date"));
    }
    if(areErrors() > 0){
        return false;
    }else{
        return true;
    }
}

if($_POST['form_submitted'] == "yes"){
    $name = $_POST['name'];
    $surname = $_POST['surname'];
    $username = $_POST['username'];
    $date = $_POST['date'];
    $type = $_POST['type'];
    $passwd = $_POST['passwd'];
    $access = $_POST['access'];
    $hobby=$_POST['hobby'];
    $zodiac=$_POST['zodiac'];
    $file=$_FILES['file'];
    $validate = formValidate($name, $surname, $username, $date, $passwd,$hobby,$zodiac,$file);
    if(!$validate){
        showErrors();
    }else{
        $sql = "Select MAX(crp_id) from croupier";
        $max = $dbh->queryOne($sql);
        settype($max,'integer');
        $crpcode = 30000 + $max + 1;
        $sql = "Insert into croupier (crp_code, crp_first_name, crp_last_name, crp_username, crp_type, crp_access, crp_date_of_birth, crp_passwd,crp_hobby,crp_zodiac)".
            " values('CPR_$crpcode', '$name', '$surname', '$username', $type, '$access', '$date', '". md5($passwd) . "', '$hobby', '$zodiac')";
        $rs = $dbh->exec($sql);
        move_uploaded_file($_FILES["file"]["tmp_name"],	PHOTOS.$crpcode.".jpeg");
        $filename = PHOTOS.$crpcode.".jpeg";
        $heightnew = 211;
        $widthnew=195;
        header('Content-Type: image/jpeg');
        list($width, $height) = getimagesize($filename);
        $newwidth = $widthnew;
        $newheight = $heightnew;
        $thumb = imagecreatetruecolor($newwidth, $newheight);
        $source = imagecreatefromjpeg($filename);
        imagecopyresized($thumb, $source, 0, 0, 0, 0, $newwidth, $newheight, $width, $height);
        imagejpeg($thumb,PHOTOS.$crpcode.".jpeg");
        settype($rs, 'integer');
        if($rs == 0){
            addError("",$lang->getLang("The croupier wasn't added.Please check the data"));
            showResult();
        }else{
            header("Location: ".$_SERVER['PHP_SELF']."?page=live/croupier&header_page=");
        }
    }
}

?>
<div class="bodyHD"><?=$lang->getLang("Add a new croupier")?></div>
<?form_head(refFormPage($_GET["page"]) . "&action=show", "search", "POST",'','','multipart/form-data');?>
<table border="0" cellspacing="0" cellpadding="0">
    <tr valign="top">
        <td>
            <table border="0" cellspacing="1" cellpadding="4">
                <tr><td colspan="2" class="formheading"><?=$lang->getLang('Croupier Data')?></td></tr>
                <tr>
                    <td width="140" class="label"><?=$lang->getLang('Name')?></td>
                    <td class="content"><input type="input" value="<?=$name?>" name="name" /></td>
                </tr>
                <tr>
                    <td width="140" class="label"><?=$lang->getLang('Last Name')?></td>
                    <td class="content"><input type="input" value="<?=$surname?>" name="surname" /></td>
                </tr>
                <tr>
                    <td width="140" class="label"><?=$lang->getLang('Username')?></td>
                    <td class="content"><input type="input" value="<?=$username?>" name="username" /></td>
                </tr>
                <tr>
                    <td width="140" class="label"><?=$lang->getLang('Birthdate')?></td>
                    <td class="content"> <input type='text' name='date' id='date' value="<?= $date ?>"></td>
                </tr>
                <tr>
                    <td width="140" class="label"><?=$lang->getLang('Password')?></td>
                    <td class="content"><input type="input" value="<?=$passwd?>" name="passwd" /></td>
                </tr>
                <tr>
                    <td width="140" class="label"><?=$lang->getLang('Type')?></td>
                    <td class="content">
                        <select name="type">
                            <option value="0" <?php if($type == 0){ ?> selected <?php } ?>>Croupier</option>
                            <option value="1" <?php if($type == 1){ ?> selected <?php } ?>>Supervisor</option>
                            <option value="2" <?php if($type == 2){ ?> selected <?php } ?>>Administrator</option>
                        </select>
                    </td>
                </tr>
                <tr>
                    <td width="140" class="label"><?=$lang->getLang('Hobby')?></td>
                    <td class="content"><input type="input" value="" name="hobby" /></td>
                </tr>
                <tr>
                    <td width="140" class="label"><?=$lang->getLang('Zodiac')?></td>
                    <td class="content">
                        <div id='shown_zodiac'></div>
                        <input type='hidden' name='zodiac' id='zodiac'>
                        <!--  <select name='zodiac'>
                              <option value='aries'>Aries</option>
                              <option value='taurus'>Taurus</option>
                              <option value='gemini'>Gemini</option>
                              <option value='cancer'>Cancer</option>
                              <option value='leo'>Leo</option>
                              <option value='virgo'>Virgo</option>
                              <option value='libra'>Libra</option>
                              <option value='scorpio'>Scorpio</option>
                              <option value='sagittarius'>Sagittarius</option>
                              <option value='capricorn'>Capricorn</option>
                              <option value='aquarius'>Aquarius</option>
                              <option value='pisces'>Pisces</option>
                          </select>-->
                    </td>
                </tr>
                <tr>
                    <td width="140" class="label"><?=$lang->getLang('Access')?></td>
                    <td class="content">
                        <select name="access">
                            <option value="allow" <?php if($access == "allow"){ ?> selected <?php } ?>><?=$lang->getLang("Allow")?></option>
                            <option value="deny" <?php if($access == "deny" or $access == ""){ ?> selected <?php } ?>><?=$lang->getLang("Deny")?></option>
                        </select></td>
                </tr>
                <tr>
                    <td width="140" class="label"><?=$lang->getLang('Photo')?></td>
                    <td class="content"><input type="file" name="file" id="file" /></td>
                </tr>
            </table>
        <td width="50"></td>
        <td>
            <table border="0" cellpadding="4" cellspacing="1">
                <tr>
                    <td><input type="submit" value="<?=$lang->getLang("Add")?>" /><br/>
                        <input type="button" value="<?=$lang->getLang("Back")?>" onclick="comeToList();" /></td>
                </tr>
            </table>
        </td>
        </td>
    </tr>
</table>
