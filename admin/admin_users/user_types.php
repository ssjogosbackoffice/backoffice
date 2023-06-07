<?check_access('manage_admin_user_types',$doredirect=true);?>
<? $title = 'Administrator Users';?>
<? $onload = "sortTable ('name','asc')";?>
<? include('header.inc'); ?>
<?php
include 'inputbox.class.inc';
global $lang;

$box = new InputBox();
$box->printJavascriptFunctions();
$addbox_visibility="hidden";
$editbox_visibility="hidden";

//define add admin user type box

$fields[0] = array('label'=>$lang->getLang("Code"), 'markup'=>'<input type="text" id="code" name="code" value="'.( $err_msg ? $_POST['code'] : '' ).'" size="40">');

$fields[1] = array('label'=>$lang->getLang("Name"), 'markup'=>'<input type="text" id="name" name="name" value="'.( $err_msg ? $_POST['name'] : '' ).'" size="40">');

$sel  = '<select id="jurisdiction_class" name="jurisdiction_class">' .
    '<option value=""> - '.$lang->getLang("select").' - </option>' .
    '<option value="casino"' . ( 'casino' == $_POST['jurisdiction_class'] && $err_msg ? ' selected' : '') . '>Casino</option>' .
    '<option value="club"' . ( 'club' == $_POST['jurisdiction_class'] && $err_msg  ? ' selected' : '') . '>Club</option>' .
    '<option value="district"' . ( 'district' == $_POST['jurisdiction_class'] && $err_msg  ? ' selected' : '') . '>District</option>' .
    '<option value="region"' . ( 'region' == $_POST['jurisdiction_class'] && $err_msg  ? ' selected' : '') . '>Region</option>' .
    '<option value="nation"' . ( 'nation' == $_POST['jurisdiction_class'] && $err_msg  ? ' selected' : '') . '>Nation</option>' .
    '</select>';

$fields[2] = array('label'=>$lang->getLang('Entity Type'), 'markup'=>$sel);

$fields[3] = array('label'=>$lang->getLang('Level'), 'markup'=>'<input type="text" id="level" name="level" value="'.( $err_msg ? $_POST['level'] : '' ).'" value="N/A" size="3">');

$buttons[0] = '<input type="button" onClick="saveUserType()" name="op" value='.$lang->getLang("Save").' style="width:60px;margin-bottom:2px" />';

$close_action = "document.getElementById('jurisdiction_class').selectedIndex=0;document.getElementById('level').value='';" .
    "document.getElementById('code').value='';document.getElementById('name').value='';hideInputBox('addbox');hideDisablePane()";

$buttons[1] = '<input type="button" value='.$lang->getLang("Cancel").' onClick="'.$close_action.'" style="width:60px;margin-bottom:2px" />';

$hidden_input['orderby'] = $orderby;
$hidden_input['direction'] = $direction;

$box->write('addbox', $fields, $buttons, $lang->getLang('Add an Administrator User'), $lang->getLang('Enter Details'), $width="350", $height="220", $visibility='hidden', $inputbox_errors,$hidden_input, $close_action);

//define edit admin user type box

$fields[0] = array('label'=>'Code', 'markup'=>'<input type="text" id="e_code" name="e_code" value="'.( $err_msg ? $_POST['e_code'] : '' ).'" size="40">');

$fields[1] = array('label'=>$lang->getLang("Name"), 'markup'=>'<input type="text" id="e_name" name="e_name" value="'.( $err_msg ? $_POST['e_name'] : '' ).'" size="40">');



$sel  = '<select id="e_jurisdiction_class" name="e_jurisdiction_class">' .
    '<option value="">- '.$lang->getLang('Please select').' -</option>' .
    '<option value="casino"' . ( 'casino' == $_POST['e_jurisdiction_class'] && $err_msg ? ' selected' : '') . '>Casino</option>' .
    '<option value="club"' . ( 'club' == $_POST['e_jurisdiction_class'] && $err_msg  ? ' selected' : '') . '>Club</option>' .
    '<option value="district"' . ( 'district' == $_POST['e_jurisdiction_class'] && $err_msg  ? ' selected' : '') . '>District</option>' .
    '<option value="region"' . ( 'region' == $_POST['e_jurisdiction_class'] && $err_msg  ? ' selected' : '') . '>Region</option>' .
    '<option value="nation"' . ( 'nation' == $_POST['e_jurisdiction_class'] && $err_msg  ? ' selected' : '') . '>Nation</option>' .
    '</select>';

$fields[2] = array('label'=>$lang->getLang("Entity Type"), 'markup'=>$sel);

$fields[3] = array('label'=>$lang->getLang("Level"), 'markup'=>'<input type="text" id="e_level" name="e_level" value="'.( $err_msg ? $_POST['e_level'] : '' ).'" size="3">');



$close_action = "var type_id = document.getElementById('type_id').value;document.getElementById('e_jurisdiction_class').selectedIndex=0;document.getElementById('e_level').value='';document.getElementById('td_buttons['+type_id+']').style.backgroundColor='#f9f9f9';
            document.getElementById('td_names['+type_id+']').style.backgroundColor='#FFFFFF';
            document.getElementById('td_codes['+type_id+']').style.backgroundColor='#FFFFFF';
            document.getElementById('td_levels['+type_id+']').style.backgroundColor='#FFFFFF';
            document.getElementById('td_jurisdiction_classes['+type_id+']').style.backgroundColor='#FFFFFF';" .
    "document.getElementById('e_code').value='';document.getElementById('e_name').value='';hideInputBox('editbox');hideDisablePane()";

$hidden_input['orderby'] = $orderby;
$hidden_input['direction'] = $direction;
$hidden_input['type_id'] = '';

$buttons[0] = '<input type="button" onClick="updateUserType()" name="op" value='.$lang->getLang("Update").' style="width:60px;margin-bottom:2px" />';
$buttons[1] = '<input type="button" value='.$lang->getLang("Cancel").' onClick="'.$close_action.'" style="width:60px;margin-bottom:2px" />';

$box->write('editbox', $fields, $buttons, "Edit an Admin User Type", 'Enter Details:', $width="350", $height="220", $visibility='hidden', $inputbox_errors,$hidden_input, $close_action);

?>
<script language="JavaScript">

function showDisablePane(){
    var pane = document.getElementById('disable_pane');
    pane.style.width  = xClientWidth();
    pane.style.height = xClientHeight();
    pane.style.visibility = 'visible';
}

function hideDisablePane(){
    var pane = document.getElementById('disable_pane');
    pane.style.visibility = 'hidden';
}

function sortTable (orderby,direction){
    document.getElementById('orderby').value   = orderby;
    document.getElementById('direction').value = direction;

    retrieveURL('/admin_users/user_type_list.php?orderby='+orderby+'&direction='+direction, 'updateTable', true);
}


function retrieveURL(url, resultFunction, show_loading_bar) {
    if ( true == show_loading_bar ){
        showLoadingBar();
        showDisablePane();
    }
    if (window.XMLHttpRequest) {
        // Non-IE browsers
        req = new XMLHttpRequest();
        req.onreadystatechange = eval(resultFunction);

        try {
            req.open("GET", url, true);
        }catch (e) {
            alert(e);
        }
        req.send(null);
    } else if (window.ActiveXObject) {
        // IE
        req = new ActiveXObject("Microsoft.XMLHTTP");
        if (req) {
            req.onreadystatechange = eval(resultFunction);
            req.open("GET", url, true);
            req.send();
        }
    }
}


function postToURL(url, qstr, resultFunction, show_loading_bar) {

    if ( true == show_loading_bar ){
        showLoadingBar();
        showDisablePane();
    }

    if (window.XMLHttpRequest) {
        // Non-IE browsers
        req = new XMLHttpRequest();
        req.onreadystatechange = eval(resultFunction);
        try {
            req.open("POST", url, false);
            req.setRequestHeader('Content-Type','application/x-www-form-urlencoded');
            req.send(qstr);
        } catch (e) {
            alert(e);
        }
    } else if (window.ActiveXObject) {
        // IE
        req = new ActiveXObject("Microsoft.XMLHTTP");
        if (req){
            req.onreadystatechange = eval(resultFunction);
            req.open("POST", url, true);
            req.setRequestHeader('Content-Type','application/x-www-form-urlencoded');
            req.send(qstr);
        }
    }
}

function updateTable(){
    if (req.readyState == 4){
        // Complete
        if (req.status == 200){
            // OK response
            document.getElementById(contentArea).innerHTML = req.responseText;
        }else {
            alert("Status: " + req.status + " Problem: updateTable " + req.statusText);
        }
        hideLoadingBar();
        hideDisablePane();
    }
}

function showResponseAndRefresh(response, redisplay_box){
    if (req.readyState == 4){
        hideLoadingBar();
        hideDisablePane();
        // Complete
        if (req.status == 200){
            //refresh list
            document.getElementById(contentArea).innerHTML = req.responseText;
            var response_arr = response.split(',');
            var status       = response_arr[0];
            var msg          = response_arr[1];
            // if unsuccessful update response, show inout box again
            if ( '1' != status ){
                if ( redisplay_box != '' && redisplay_box != null ){
                    showInputBox(redisplay_box, false);
                    showDisablePane();
                }
            }
            alert(msg);
        }else{
            alert("Status: " + req.status + " Problem refreshing list " + req.statusText);
        }
    }
}

function handleDeleteResponse(orderby,direction){
    if (req.readyState == 4){
        // Complete
        if (req.status == 200){
            // OK response
            var response = req.responseText;
            func =  function (){
                showResponseAndRefresh(response, null);
            }

            retrieveURL('/admin_users/user_type_list.php?orderby='+orderby+'&direction='+direction,func);
        }else {
            alert("Status: " + req.status + " Problem: alertResponse " + req.statusText);
        }
    }
}


function handleUpdateResponse(){
    if (req.readyState == 4){
        // Complete
        if (req.status == 200){
            // OK response
            var response = req.responseText;
            func =  function (){
                showResponseAndRefresh(response, 'editbox');
            }
            orderby   = document.getElementById('orderby').value;
            direction = document.getElementById('direction').value;
            retrieveURL('/admin_users/user_type_list.php?orderby='+orderby+'&direction='+direction, func,true);
        }else{
            alert("Status: " + req.status + " Problem: alertResponse " + req.statusText);
            hideLoadingBar();
            hideDisablePane();
        }
    }
}

function handleSaveResponse(){
    if (req.readyState == 4){
        // Complete
        if (req.status == 200){
            // OK response
            var response = req.responseText;
            func =  function (){
                showResponseAndRefresh(response, null);
            }
            orderby   = document.getElementById('orderby').value;
            direction = document.getElementById('direction').value;
            retrieveURL('/admin_users/user_type_list.php?orderby='+orderby+'&direction='+direction, func,true);
        }else{
            alert("Status: " + req.status + " Problem: alertResponse " + req.statusText);
            hideLoadingBar();
            hideDisablePane();
        }
    }
}

function deleteUserType(type_id, name, orderby, direction){
    document.getElementById('td_buttons['+type_id+']').style.backgroundColor ='lightblue';
    document.getElementById('td_names['+type_id+']').style.backgroundColor   ='lightblue';
    document.getElementById('td_codes['+type_id+']').style.backgroundColor   ='lightblue';
    document.getElementById('td_levels['+type_id+']').style.backgroundColor  ='lightblue';
    document.getElementById('td_jurisdiction_classes['+type_id+']').style.backgroundColor='lightblue';

    var deleteusertype = confirm('Delete selected user type?');

    var func = function () { handleDeleteResponse(orderby, direction); }

    if(deleteusertype){
        retrieveURL('/admin_users/user_type_delete.php?id=' + type_id, func, true);
    }else{
        document.getElementById('td_buttons['+type_id+']').style.backgroundColor='#ffffff';
        document.getElementById('td_names['+type_id+']').style.backgroundColor='#ffffff';
        document.getElementById('td_codes['+type_id+']').style.backgroundColor='#ffffff';
        document.getElementById('td_levels['+type_id+']').style.backgroundColor='#ffffff';
        document.getElementById('td_jurisdiction_classes['+type_id+']').style.backgroundColor='#ffffff';
    }
}

function updateUserType(){
    hideInputBox('editbox');
    var type_id     = document.getElementById('type_id').value;
    var name        = document.getElementById('e_name').value;
    var code        = document.getElementById('e_code').value;
    var level       = document.getElementById('e_level').value;
    var juris_class = document.getElementById('e_jurisdiction_class').options[document.getElementById('e_jurisdiction_class').selectedIndex].value;

    var qstr = 'id='+type_id+'&name='+name+'&code='+code+'&juris_class='+juris_class+'&level='+level;
    var func = function () { handleUpdateResponse(); }
    retrieveURL('/admin_users/user_type_update.php?'+qstr, func, true);
}

function saveUserType(){
    hideInputBox('addbox');
    var name        = document.getElementById('name').value;
    var code        = document.getElementById('code').value;
    var level       = document.getElementById('level').value;
    var juris_class = document.getElementById('jurisdiction_class').options[document.getElementById('jurisdiction_class').selectedIndex].value;
    var qstr = 'name='+name+'&code='+code+'&juris_class='+juris_class+'&level='+level;
    var func = function () { handleSaveResponse(); }

    retrieveURL('/admin_users/user_type_save.php?'+qstr, func, true);
}

function showEditBox (type_id, name, code, juris_class, level){

    jurisdiction_classes = new Array();
    jurisdiction_classes['casino']   = 1;
    jurisdiction_classes['club']     = 2;
    jurisdiction_classes['district'] = 3;
    jurisdiction_classes['region']   = 4;
    jurisdiction_classes['nation']   = 5;

    document.getElementById('td_buttons['+type_id+']').style.backgroundColor    = 'lightblue';
    document.getElementById('td_names['+type_id+']').style.backgroundColor      = 'lightblue';
    document.getElementById('td_codes['+type_id+']').style.backgroundColor      = 'lightblue';
    document.getElementById('td_levels['+type_id+']').style.backgroundColor     = 'lightblue';
    document.getElementById('td_jurisdiction_classes['+type_id+']').style.backgroundColor = 'lightblue';

    document.getElementById('type_id').value = type_id;

    document.getElementById('e_name').value = name;
    document.getElementById('e_code').value = code;
    document.getElementById('e_level').value = level;
    document.getElementById('e_jurisdiction_class').selectedIndex = jurisdiction_classes[juris_class];

    showInputBox('editbox', false);
    showDisablePane();
}

function showAddBox(){
    showInputBox('addbox', true);
    showDisablePane();
}

var contentArea = 'urlContent';

</script>
<div class=bodyHD><?=$lang->getLang("Administrator User Type")?></div>
<br/>
<table cellpadding="0" cellspacing="0" border="0">
    <tr valign="top">
        <td>
            <div id="urlContent"></div>
        </td>
        <td width="16">&nbsp;</td>
        <td>
            <a href="javascript:showAddBox()"><img border="0" src="<?=image_dir?>/button_add.gif"></a>
        </td>
    </tr>
</table>
<div id="disable_pane" style="visibility:hidden;position:absolute;left:0px;top:0px;width:100%;height:100%;z-index:99;filter:alpha(Opacity=20);-moz-opacity:0.1;">
    <input type="hidden" id="tabtaker" />
</div>
<?php include 'loading_bar_inc.php';?>
<form>
    <input type="hidden" id="orderby" value="name">
    <input type="hidden" id="direction" value="asc">
</form>
<? include 'footer.inc'; ?>

