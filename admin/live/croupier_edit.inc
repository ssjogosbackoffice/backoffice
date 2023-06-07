<script type="text/javascript">
    function comeToList(){
        location.href = "<?=$_SERVER['PHP_SELF']?>"+"?page=live/croupier&header_page=";
    }
    function updateCroupierCode(croupierID){
        var crpcode = prompt("<?=$lang->getLang("Insert the new 5 numbers code")?>");
        if(crpcode == ""){
            alert("<?=$lang->getLang("Empty code")?>");
        }else if(crpcode != "" && crpcode != null){
            $.ajax({
                url: "live/croupier_func.inc.php",
                data: {reqCode:0, code:crpcode, cid:croupierID},
                success: function(data){
                    if(data == "1"){
                        alert("Dati aggiornati");
                        location.href = "<?=$_SERVER['PHP_SELF']?>"+"?page=live/croupier_edit&header_page=&cid="+croupierID;
                    }else if(data == "0"){
                        alert("<?=$lang->getLang("The data wasn't updated")?>");
                    }else if(data == "-1"){
                        alert("<?=$lang->getLang("Insert the new 5 numbers code")?>");
                    }else if(data == "-2"){
                        alert("<?=$lang->getLang("The code is already used by another croupier")?>");
                    }
                }
            });
        }
    }

    function readURL(input) {
        if (input.files && input.files[0]) {
            var reader = new FileReader();

            reader.onload = function (e) {
                $('#blah').attr('src', e.target.result);
                $('#blah').show();            }

            reader.readAsDataURL(input.files[0]);
        }
    }


    $(function () {
        $("#date_start").datepicker({
            changeMonth: true,
            changeYear:true,
            numberOfMonths: 1,
            maxDate: '-18Y',
            onSelect: function( selectedDate ) {
                dateMax = $("#date_start").datepicker("getDate");
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
check_access('live_croupier_edit', $redirect=true);

$croupier_login  = check_access('croupier_login');
$croupier_number = check_access('croupier_number');
include_once("funclive.inc.php");

if($_POST['form_submitted'] == "yes"){
    $mName=$_POST['name'];
    $cc=$_POST['cc'];
    $mLastName=$_POST['lastname'];
    $mUsername = $_POST['username'];
    $mDate = $_POST['date_start'];
    $mType = $_POST['type'];
    $mAccess = $_POST['access'];
    $mzodiac= $_POST['zodiac'];
    $mhobby= $_POST['hobby'];
    $password = $_POST['passwd'];
    $value = preg_match('/^[a-zA-Z0-9]{1,30}$/', $mUsername);
    $countpass = strlen($password);
    if(isset($_FILES["file"])){
        if ((($_FILES["file"]["type"] == "image/gif")
            || ($_FILES["file"]["type"] == "image/jpeg")
            || ($_FILES["file"]["type"] == "image/png")
            || ($_FILES["file"]["type"] == "image/jpg")))
        {
            if ($_FILES["file"]["error"] > 0)
            {
                addError(""," Error Code:". $file['error']);

            }
            else
            {
                $sql=
                    move_uploaded_file($_FILES["file"]["tmp_name"],	PHOTOS.($cc).".jpeg");
                $filename = PHOTOS.($cc).".jpeg";
                $heightnew = 211;
                $widthnew=195;
                // header('Content-Type: image/jpeg');
                list($width, $height) = getimagesize($filename);
                $newwidth = $widthnew;
                $newheight = $heightnew;
                $thumb = imagecreatetruecolor($newwidth, $newheight);
                if($_FILES["file"]["type"] == "image/png"){
                    $source = imagecreatefrompng($filename);
                }
                else
                {
                    $source = imagecreatefromjpeg($filename);
                }
                imagecopyresized($thumb, $source, 0, 0, 0, 0, $newwidth, $newheight, $width, $height);
                if($_FILES["file"]["type"] == "image/png"){
                    imagepng($thumb,PHOTOS.($cc).".png");
                }
                else{
                    imagejpeg($thumb,PHOTOS.($cc).".jpeg");
                }
                addResult("","Photo successfully updated");
                //unlink ($_FILES["file"]["tmp_name"]);
            }
        }
    }
    if($mName==''){
        addError("",$lang->getLang("Empty name"));
        showErrors();
    }
    elseif($mLastName==''){
        addError("",$lang->getLang("Empty Last Name"));
        showErrors();
    }
    if(!$value){
        addError("",$lang->getLang("The username has to contain only letters"));
        showErrors();
    }else if($password != "" && ($countpass < 6 || $countpass > 6)){
        addError("",$lang->getLang("The password has to contain at least 6 characters"));
        showErrors();

    }else{
        if($password == ""){
            $sql = "Update croupier set crp_first_name='$mName',crp_last_name='$mLastName', crp_username = '$mUsername' , crp_date_of_birth = '$mDate' ,crp_type = $mType, crp_access = '$mAccess',crp_hobby = '$mhobby',crp_zodiac = '$mzodiac'  where crp_id = $_GET[cid]";
        }else{
            $sql = "Update croupier set crp_first_name='$mName',crp_last_name='$mLastName',crp_username = '$mUsername' , crp_date_of_birth = '$mDate', crp_passwd = '" . md5($password) ."' ,crp_type = $mType, crp_access = '$mAccess',crp_hobby = '$mhobby',crp_zodiac = '$mzodiac' where crp_id = $_GET[cid]";
        }
        $rs = $dbh->exec($sql);
        if($rs == 0){
            if($_FILES["file"]["type"]==''){
                addError("",$lang->getLang("Data not modified or already updated"));
            }
            showErrors();


        }else{
            addResult("","Data successfully updated");

        }
        showResult();
    }
}


$sql = "Select * from croupier where crp_id = " . $_GET['cid'];
$rs = $dbh->exec($sql);
$row = $rs->next();
//Dati Croupier
$code     = substr($row["crp_code"],4);
$name     = $row["crp_first_name"];
$surname  = $row["crp_last_name"];
$username = $row["crp_username"];
$hobby    = $row["crp_hobby"];
$zodiac   = $row["crp_zodiac"];

$reg_date = $row["crp_date_of_birth"];
if($reg_date == ""){
    $reg_date = date("Y-m-d",time());
}
$reg_date=substr($reg_date,0,10);

$type   = $row["crp_type"];
$access = $row["crp_access"];
$zodiac = $row["crp_zodiac"];
?>
<div class="bodyHD"><?=$lang->getLang("Manage Croupier")?> </div>
<?form_head(refFormPage($_GET["page"]) . "&cid=".$_GET['cid']."&action=show", "search", "POST",'','','multipart/form-data');?>
<table border="0" cellspacing="0" cellpadding="0">
    <tr valign="top">
        <td>
            <table border="0" cellpadding="4" cellspacing="1">
                <tr>
                    <td colspan="2" class="formheading"><?=$lang->getLang('Croupier Data')?></td>
                </tr>
                <tr>
                    <td width="100" class="label"><?=$lang->getLang('Code')?></td><td class="content"><?=$code?><input type="hidden"name='cc' value="<?=$code?>"></td>
                </tr>
                <tr>
                    <td width="100" class="label"><?=$lang->getLang('Name')?></td><td class="content"><input type="text" name='name' value="<?=$name?>"></td>
                </tr>
                <tr>
                    <td width="100" class="label"><?=$lang->getLang('Last Name')?></td><td class="content"><input type="text" name="lastname" value="<?=$surname?>"></td>
                </tr>
                <tr>
                    <td width="100" class="label"><?=$lang->getLang('Username')?></td><td class="content"><input type="input" name="username" value="<?=$username?>" /></td>
                </tr>
                <tr>
                    <td width="100" class="label"><?=$lang->getLang('Birthdate')?></td><td class="content"> <input type='text' name='date_start' id='date_start' value="<?= $reg_date ?>"></td>
                </tr>
                <tr>
                    <td width="100" class="label"><?=$lang->getLang('Password')?></td><td class="content"><input type="input" name="passwd" value="" /></td>
                </tr>
                <tr>
                    <td width="100" class="label"><?=$lang->getLang('Type')?></td>
                    <td class="content">
                        <select name="type">
                            <option value="0" <?php if($type == 0){ ?> selected <?php } ?>>Croupier</option>
                            <option value="1" <?php if($type == 1){ ?> selected <?php } ?>>Supervisor</option>
                            <option value="2" <?php if($type == 2){ ?> selected <?php } ?>>Administrator</option>
                        </select>
                    </td>
                </tr>
                <tr>
                    <td width="100" class="label"><?=$lang->getLang('Access')?></td>
                    <td class="content">
                        <select name="access">
                            <option value="allow" <?php if($access == "allow"){ ?> selected <?php } ?>><?=$lang->getLang('Allow')?></option>
                            <option value="deny" <?php if($access == "deny"){ ?> selected <?php } ?>><?=$lang->getLang('Deny')?></option>
                        </select>
                    </td>
                </tr>
                <tr>
                    <td width="100" class="label"><?=$lang->getLang('Hobby')?></td><td class="content"> <input type='text' name='hobby' value="<?= $hobby ?>"></td>
                </tr>
                <tr>
                    <td width="100" class="label"><?=$lang->getLang('Zodiac')?></td>
                    <td class="content">
                        <div id='shown_zodiac'><?=$zodiac?></div>
                        <input type='hidden' name='zodiac' id='zodiac' value='<?=$zodiac?>'>

                    </td>
                </tr>
                <tr>
                    <td width="100" class="label"><?=$lang->getLang('Current Photo')?></td><td class="content">


                        <?
                        if (file_exists(PHOTOS.$code.".jpeg")) {
                            ?>
                            <img src='<?=PHOTOS.$code.".jpeg"?>'  style='width: 100px; height:100px'><br>
                        <?
                        } else {
                            ?>
                            No photo
                        <?}?>
                    </td>
                </tr>
                <tr>
                    <td width="100" class="label"><?=$lang->getLang('New Photo')?></td><td class="content">
                        <img id="blah" src="#" style='display:none;width: 100px; height:100px' alt="your image" /><br>
                        <input type='file' onchange="readURL(this);" name='file' />
                    </td>
                </tr>
            </table>
        </td>
        <td width="30"></td>
        <td>
            <table border="0" cellpadding="4" cellspacing="1">
                <tr>
                    <td width="200" colspan="2" class="formheading"><?=$lang->getLang('Other Operations')?></td>
                </tr>
                <?php if($croupier_login) { ?>
                    <tr>
                        <td width="100" class="label"><?=$lang->getLang('Extracted Numbers')?></td><td class="content"><a href="<?=$_SERVER['PHP_SELF']?>?page=live/croupier_number&header_page=&cid=<?=$_GET['cid']?>">Vedi numeri estratti</a></td>
                    </tr>
                <?php }
                if($croupier_login) { ?>
                    <tr>
                        <td width="100" class="label"><?=$lang->getLang('Login')?></td><td class="content"><a href="<?=$_SERVER['PHP_SELF']?>?page=live/croupier_login&header_page=&cid=<?=$_GET['cid']?>">Vedi login effettuati</a></td>
                    </tr>
                <?php }
                if($_SESSION["aty_code"] == "SUPERUSER"){ ?>
                    <tr>
                        <td width="100" class="label"><?=$lang->getLang("Modify")?></td><td class="content"><a href="javascript:updateCroupierCode(<?=$_GET['cid']?>);"><?=$lang->getLang("Change code")?></a></td>
                    </tr>
                <?php } ?>
            </table>
        </td>
        <td width="30"></td>
        <td>
            <table border="0" cellpadding="4" cellspacing="1">
                <tr>
                    <td><input type="submit" value="<?=$lang->getLang('Save')?>" /><br/>
                        <a href="javascript:history.go(-1)"><input type="button" value="<?=$lang->getLang('Back')?>" /></a></td>
                </tr>
            </table>
        </td>
    </tr>
</table>
