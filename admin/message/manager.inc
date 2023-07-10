<script type="text/javascript" src="media/nic/nicEdit.js"></script>
<script src="media/jquery.multiselect.js" type="text/javascript"></script>
<link rel="stylesheet" type="text/css" href="media/jquery.multiselect.css" title="core_style">
<script type="text/javascript">
    $(function(){
        $("#select").multiselect({selectedList: 100}).multiselectfilter();
    });

    function readMessage($msg_id){
        location.href = "<?=refPage("message/manager&action=read") . "&id="?>" + $msg_id;
    }
    bkLib.onDomLoaded(
        function(){
            nicEditors.allTextAreas();
            var jurclass_sel = document.getElementById("jurisdiction_level");
            if (jurclass_sel) {
                var msg = "";
                var length = jurclass_sel.options.length;
                var removed = 0;
                for (i = 0; i < length; i++) {
                    if (jurclass_sel.options[i - removed].value == "<?=$_SESSION["jurisdiction_class"]?>") {
                        jurclass_sel.remove(i - removed);
                        removed++;
                    }
                }
            }
        }
    );

    function checkSendMail(){
        var msg  = "";

        var conf = false;

        var jurclass_sel     = document.getElementById("jurisdiction_level");
        var jurisdiction_sel = document.forms[0].jurisdiction;

        if(jurclass_sel.value == ""){
            alert("<?=$lang->getLang("Add a jurisdiction type to send the message")?>");
        }else if(jurisdiction_sel.value == ""){
            var w = jurisdiction_sel.selectedIndex;
            var count = jurisdiction_sel.options.length;
            conf = confirm('<?=$lang->getLang("Are you sure you want to send this message to all")?> ' + count + ' <?=$lang->getLang("administrators of")?> ' + jurclass_sel.value + ' <?=$lang->getLang("under your structure?")?> ');
        }else{
            var w = jurisdiction_sel.selectedIndex;
            var selected_text = jurisdiction_sel.options[w].text;
            conf = confirm("<?=$lang->getLang("Are you sure you want to send this message to administrators of")?> " + jurclass_sel.value + " '" + selected_text + "'?");
        }
        return conf;
    }

    function showJurisdictionSelect(){
        $('#players_div').hide()
        $('#jurisdiction_div').show();
    }

    function showPlayersSelect(){
        $('#jurisdiction_div').hide()
        $('#players_div').show();
    }

    function getplayerslike(){
        player=$('#player_username').val();
        if(player.length>3){
            $.post("services/general_services.inc",{action:'1',player:player}, function(data){
                $('#select_div').show();

                dataobj=data.split(",");
                $.each(dataobj, function (index, value) {
                    values=value.split(";");

                    optionValue = values[0].trim();
                    if(optionValue != '') {
                        if ($('#select option[value='+optionValue+']').length === 0){ //value does not exist so add
                            $('#select').append($('<option>', {
                                value: optionValue,
                                text: values[1]
                            }));
                        }
                    }
                });
                
                $("#select").multiselect("open");
                $('#select').multiselect('refresh').multiselectfilter();
            });
        }
    }

    function getElementsByTagName(node, tagName) {
        tagName=tagName.toUpperCase();
        var els = [];
        if (node.elements) {
            node = node.elements;
        }
        for (i in node) {
            if (node[i] && tagName == node[i].tagName) {
                els.push(node[i]);
                if (node[i].id) {
                    els[node[i].id] = node[i];
                }
            }
        }
        return els;
    }

    function setCheckedState (form, state) {
        if (!form) {
            form = document;
        }
        if (!form.getElementsByTagName) {
            form.getElementsByTagName = function(tagName) {
                return getElementsByTagName(this, tagName);
            }
        }

        var inputs = form.getElementsByTagName('input');
        for (i=0; i<inputs.length; ++i) {
            if ('checkbox' == inputs[i].type.toLowerCase() && !inputs[i].disabled) {
                inputs[i].checked=state;
            }
        }
    }

    function checkAll(form) {
        setCheckedState(form, true);
    }

    function uncheckAll(form) {
        setCheckedState(form, false);
    }
</script>

<div id="messages">
<?php
//Modificata Manuel Rinaldi,Marian
function getMailStyle(&$row){
    $ret = "";
    switch($row["aum_status"]){
        case 0:
            $ret = ' new';
            break;
        case 1:
            $ret = '';
            break;
    }
    return $ret;

}

function getMailIcon(&$row){
    $ret = "";
    switch($row["aum_status"]){
        case 0:
            $ret .= '<img src="' . image_dir . '/email_new.gif" />';
            break;
        case 1:
            $ret .= '<img src="' . image_dir . '/email_read.gif" />';
            break;
    }
    switch($row["aum_level"]){
        case 2:
            $ret .= '<img src="' . image_dir . '/alert.gif" />';
            break;
    }
    return $ret;
}

function getMailFromName(&$row){
    $ret = $row["from_name"] . " (" . $row["from_jurclass"] . " " . $row["from_jurname"] . ")";
    if(empty($row["from_name"]) && $row["from_jurname"] == "casino"){
        $ret = "System Administrator";
    }elseif(!empty($row["from_name"]) && $row["from_jurname"] == "casino"){
        $ret = $row["from_name"] . " (Administration)";
    }

    return $ret;
}

function getMailToName(&$row){
    $ret = $row["to_name"] . " (" . $row["to_jurclass"] . " " . $row["to_jurname"] . ")";
    if(empty($row["to_name"]) && $row["to_jurname"] == "casino"){
        $ret = "System Administrator";
    }elseif(!empty($row["to_name"]) && $row["to_jurname"] == "casino"){
        $ret = $row["to_name"] . " (Administration)";
    }

    return $ret;
}

function readMessage($msg_id){
    global $dbh;
    settype($msg_id, "integer");
    $messages =& $_SESSION["messages"]["inbox"];
    foreach($messages as $k => $m){
        if($m["aum_id"] == $msg_id){
            unset($messages[$k]);
        }
    }
    $my_jurisdiction = $_SESSION["admin_id"];

    $sql = "UPDATE admin_user_mailbox SET aum_read_date = CURRENT_TIMESTAMP, aum_status = '1' WHERE aum_id = $msg_id AND aum_to = $my_jurisdiction AND aum_read_date IS NULL";
    return ($dbh->exec($sql) == 1);
}



function printMailForm(&$row=null, $reply=false){
    global $jurisdiction_options,$lang;
    ?>
    <h2><img src="<?=image_dir?>/email_edit.gif"/> Mail editor</h2>
    <form method="POST" action="<?=refPage("message/manager") . "&action=send";?>" <!-- onsubmit="return checkSendMail()--> ">
  <table>
      <?php if($reply) : ?>
          <input type="hidden" name="mail[to]" value="<?=$row["from_id"]?>"/>
          <tr>
              <td colspan="2"><h2>Response to <i><?=getMailFromName($row)?></i></h2></td>
          </tr>
          <tr>
              <td><?=$lang->getLang("To")?>:</td>
              <td><?=getMailFromName($row)?></td>
          </tr>
          <tr>
              <td><?=$lang->getLang("Subject")?>:</td>
              <td><input size="80" type="text" name="mail[subject]" value="RE: <?=ereg_replace('"', '\"', $row["aum_subject"])?>"/></td>
          </tr>
      <?php else : ?>
          <tr>
              <td><?=$lang->getLang("To")?>:</td>
              <td class="content">
                  <input type="hidden" name="mail[new]" value="true"/>
                  <input type="radio" name='sendto' onclick="showPlayersSelect();" id='players_ck' value='players'>  <?=$lang->getLang("Players")?><br>
                  <div id='players_div' style='display:none;margin-left: 25px'>
                      Search Player: <input type='text' onkeyup='getplayerslike()' id='player_username'><br>
                      <div id='select_div' style='display:none'>
                          <h3>Selected players:</h3>
                          <select id='select' name="playersto[]" multiple='multiple'>
                          </select>
                          <hr>
                      </div>
                  </div><br>
                  <input type="radio" name='sendto' onclick="showJurisdictionSelect()" id='jurisdiction_ck' value='jurisdiction'>Jurisdiction<br>
                  <div id='jurisdiction_div' style='display:none;margin-left: 25px;'>
                      <input type="checkbox" name='jurisdiction_admins'><?=$lang->getLang("Administration")?>
                      <input type="checkbox" name='jurisdiction_players'><?=$lang->getLang("Players")?>
                      <input type="checkbox" name='jurisdiction_affiliates'><?=$lang->getLang("Affiliates")?>
                      <br>
                      <?php
                      $jurisdiction_level    = $rs["jur_class"];
                      $jurisdiction          = $rs["prm_jur_id"];
                      $_POST['jurisdiction'] = $jurisdiction;
                      if ( 'casino'   == $_SESSION['jurisdiction_class'] ||
                          'region'   == $_SESSION['jurisdiction_class'] ||
                          'district' == $_SESSION['jurisdiction_class'] ||
                          'nation'   == $_SESSION['jurisdiction_class']) {
                          include ('jurisdiction_filters.php');
                      }
                      foreach($jurisdiction_options as $key => $opt){
                          if($key == $_SESSION["jurisdiction_class"]){
                              unset($jurisdiction_options[$key]);
                          }
                      }
                      include('jurisdiction_selector.php');
                      ?>
                  </div>
              </td>
              <?php if (!empty($jurisdiction_level)) : ?>
                  <script type="text/javascript">
                      <?="print_" . $jurisdiction_level . "_select();"?>
                  </script>
              <?php endif; ?>
          </tr>
          <tr>
              <td>Subject:</td>
              <td><input size="80" type="text" name="mail[subject]" value="<?=preg_replace('/"/', '\"', $row["aum_subject"])?>"/></td>
          </tr>
      <?php endif; ?>
      <tr>
          <td>Priority:</td>
          <td>
              <select name="mail[priority]">
                  <?printOption("", "0", "Normal",     ($row["aum_level"] == 0))?>
                  <?printOption("", "1", "High",       ($row["aum_level"] == 1))?>
                  <?printOption("", "2", "Critical",   ($row["aum_level"] == 2))?>
              </select>
          </td>
      </tr>
      <?php if($reply) : ?>
          <tr>
              <td colspan="2"><textarea name="mail[body]" rows="25" wrap="hard"><br/><quote><b><?=getMailFromName($row)?> wrote:</b><br/><?=eregi_replace("''", "'", $row["aum_body"])?></quote></textarea></td>
          </tr>
      <?php else : ?>
          <tr>
              <td colspan="2"><textarea name="mail[body]" rows="25" wrap="hard"><?=$row["aum_body"]?></textarea></td>
          </tr>
      <?php endif; ?>
      <tr>
          <td><input type="reset"  value="Reset"/></td>
          <td><input type="submit" value="<?=$lang->getLang('Send message')?>" /></td>
      </tr>



      <?php if(!$reply) : ?>
          <tr>
              <td colspan="2"><b><?=$lang->getLang("Special Codes")?>:</b></td>
          </tr>
          <tr>
              <td><b>[name]</b></td><td><b><?=$lang->getLang("Insert the administrator's name")?></b></td>
          </tr>
          <tr>
              <td><b>[jur]</b></td><td><b><?=$lang->getLang("Insert the jurisdiction's name")?></b></td>
          </tr>
          <tr>
              <td><b>[type]</b></td><td><b><?=$lang->getLang("Insert the jurisdiction's type (club, district, region, nation)")?></b></td>
          </tr>
      <?php endif; ?>
  </table>
  </form>
  <?php
}

$admin_user_id = $_SESSION['admin_id'];
$action        = isset($_REQUEST["action"]) ? $_REQUEST["action"] : null;
$msg_id        = isset($_REQUEST["id"]) ? $_REQUEST["id"] : null;

settype($msg_id, "integer");

if($action == "write" && $_SESSION["jurisdiction_class"] == "club"){
    $action = "show";
}

//$default_link = getLink(refPage("message/manager&action=show"), "Return to INBOX") . " | " . getLink(refPage("message/manager&action=sent"), "Sent Messages") . " | " . getLink(refPage("message/manager&action=write"), "Create new message");

$default_link = getLink(refPage("message/manager&action=show"), "<img src='" . image_dir . "/email_new.gif'/> Return to INBOX (" . count($_SESSION["messages"]["inbox"]) . ")")
    . " | " . getLink(refPage("message/manager&action=bin"), "<img src='" . image_dir . "/bin.gif'/> Recycle Bin");

if($_SESSION["jurisdiction_class"] != "club"){
    $default_link .= " | " . getLink(refPage("message/manager&action=write"), "<img src='" . image_dir . "/email_edit.gif'/> Create new message");
}

?>

<?=$default_link?>
<br/><br/>

<?php

if($action == "delete"){
    $messages_to_delete = $_POST["message"];
    $msg_ids            = array();

    if(count($messages_to_delete) > 0){
        foreach($messages_to_delete as $key => $d){
            settype($key, "integer");
            array_push($msg_ids, $key);
        }
        $sql = "UPDATE admin_user_mailbox SET aum_status = 2 WHERE aum_id IN (" . implode(",", $msg_ids) . ") AND (aum_to = $admin_user_id OR aum_from = $admin_user_id) AND aum_status > 0";
        $res = $dbh->exec($sql);
        $msg = "$res".$lang->getLang("messages deleted correctly");
        printInfoMessage($msg);
        jalert($msg);
    }
    $action = "show";
}
if($action == "recycle"){
    $messages_to_delete = $_POST["message"];
    $msg_ids            = array();

    if(count($messages_to_delete) > 0){
        foreach($messages_to_delete as $key => $d){
            settype($key, "integer");
            array_push($msg_ids, $key);
        }
        $sql = "UPDATE admin_user_mailbox SET aum_status = 1 WHERE aum_id IN (" . implode(",", $msg_ids) . ") AND (aum_to = $admin_user_id OR aum_from = $admin_user_id) AND aum_status > 0";
        $res = $dbh->exec($sql);

        $msg = "$res".$lang->getLang("messages recycled correctly");
        printInfoMessage($msg);
        jalert($msg);
    }
    $action = "show";
}


switch($action){
    case "write":
        printMailForm();
        break;
    case "send":
        $request =& $_POST["mail"];
        foreach($request as $k => $r){
            $request[$k] = trim($r);
        }
        if($_POST['sendto']=='players'){
            if(count($_POST['playersto'])<1){
                addError("", $lang->getLang("Please choose at least one player"));
            }
            $to=implode(",",$_POST['playersto']);
        if(!areErrors()){

            $sql = "insert into customer_message
                  select last_seq_id('CMG_SEQ'), NOW(), @read_time :='0000-00-00 00:00:00', @gms_aus_id :=".$_SESSION['jurisdiction_id'].", pun_id, @subject := '".$dbh->escape($_POST['mail']['subject'])."', @body := '".$dbh->escape($_POST['mail']['body'])."', @ip_address := NULL, @status := '0', @level := 0
                  from punter WHERE pun_id in ($to)";
            $res=$dbh->exec($sql);
        if($res > 0){
            ?>
            <script type="text/javascript">
                alert("Message <?=ereg_replace('"', '\"', $request["subject"])?> sent correctly");
                location.href = "<?=refPage('message/manager')?>";
            </script>
        <?php
        }
        }else{
            showErrors();
        }
        }
        else{
        $to          = $request["to"];
        settype($to, "integer");
        $new_email   = ($request["new"] == "true");
        $jur_class   = $_POST["jurisdiction_level"];
        $jur_id      = $_POST["jurisdiction"];
        $priority    = $request["priority"];

        settype($priority, "integer");

        settype($jur_id, "integer");

        if(empty($to) && !$new_email){
            addError("", $lang->getLang("Invalid address"));
        }


        if($new_email && empty($jur_class)){
            addError("", $lang->getLang("Select a jurisdiction"));


        }

        if(!isset($_POST['jurisdiction_admins']) && !isset($_POST['jurisdiction_players']) && $new_email){
            addError("", $lang->getLang("Select at least one option of players or administration"));
        }

        $clean_body = strip_tags($request["body"]);
        $clean_body = trim($clean_body);

        $clean_subj = strip_tags($request["subject"]);
        $clean_subj = trim($clean_subj);

        if(empty($clean_body)){
            addError("", $lang->getLang("Insert a message"));
        }

        if(empty($clean_subj)){
            addError("", $lang->getLang("Insert a subject"));
        }

        $jur_sql = "";




        if(!areErrors()){

        if(isset($_POST['jurisdiction_players'])&& isset($jur_class)){

        $sql = "insert into customer_message
                  select last_seq_id('CMG_SEQ'), NOW(), @read_time :='0000-00-00 00:00:00', @gms_aus_id :=".$_SESSION['jurisdiction_id'].", pun_id, @subject := '".$dbh->escape($_POST['mail']['subject'])."', @body := '".$dbh->escape($_POST['mail']['body'])."', @ip_address := NULL, @status := '0', @level := 0
                  from punter ";

        if($_SESSION["jurisdiction_class"] == "club"){
            $sql .="  where pun_betting_club =".$_SESSION['jurisdiction_id'] ;

        }
        elseif($_SESSION["jurisdiction_class"] == "district"){
            $sql .=", jurisdiction c ,jurisdiction d where d.jur_id=".$_SESSION['jurisdiction_id'];
            $sql.="  and pun_betting_club = c.jur_id";
            $sql.="  and c.jur_parent_id = d.jur_id";

        }
        elseif($_SESSION["jurisdiction_class"] == "region"){
                $sql .=", jurisdiction c ,jurisdiction d,jurisdiction r where r.jur_id=".$_SESSION['jurisdiction_id'];
                $sql.="  and pun_betting_club = c.jur_id";
                $sql.="  and c.jur_parent_id = d.jur_id";
                $sql.="  and  d.jur_parent_id = r.jur_id";
        }
        elseif($_SESSION["jurisdiction_class"] == "nation"){
            $sql .=", jurisdiction c ,jurisdiction d,jurisdiction r,jurisdiction n where n.jur_id=".$_SESSION['jurisdiction_id'];
            $sql.=" and pun_betting_club = c.jur_id";
            $sql.=" and c.jur_parent_id = d.jur_id
                    and  d.jur_parent_id = r.jur_id
                    and r.jur_parent_id = n.jur_id";
        }
        elseif($_SESSION["jurisdiction_class"] == "casino"){
            $sql .=", jurisdiction c ,jurisdiction d,jurisdiction r,jurisdiction n where pun_betting_club = c.jur_id";
            $sql.=" and c.jur_parent_id = d.jur_id
                    and  d.jur_parent_id = r.jur_id
                    and r.jur_parent_id = n.jur_id
                    and n.jur_parent_id=1 ";
        }

       if($jur_class=='club' && !empty($jur_id)){
                $sql .= " AND c.jur_id = ".$dbh->escape($jur_id);

       }
        if($jur_class=='district' && !empty($jur_id)){
                $sql .= " AND d.jur_id =".$dbh->escape($jur_id);
        }
        if($jur_class=='region' && !empty($jur_id)){
                $sql .= " AND r.jur_id=".$dbh->escape($jur_id);
        }
        if($jur_class=='nation' && !empty($jur_id)){
                $sql .= " AND n.jur_id=".$dbh->escape($jur_id);
        }
        $res=$dbh->exec($sql);
        if($res > 0){
        ?>
            <script type="text/javascript">
                alert("Message <?=ereg_replace('"', '\"', $request["subject"])?> sent correctly to players");
            </script>
        <?php
        }
        }

        if(isset($request["to"])){
            $_POST['jurisdiction_admins']='true';
        }

        if(isset($_POST['jurisdiction_admins'])){

        if($new_email && !empty($jur_class)){
            //echo "jur_class: $jur_class<br/>";

            $jur_levels = array("casino", "nation", "region", "district", "club");

            $my_jurisdiction = $_SESSION["jurisdiction_id"];
            $my_jur_class    = $_SESSION["jurisdiction_class"];

            $my_level        = array_search($my_jur_class, $jur_levels);
            $to_level        = array_search($jur_class,    $jur_levels);

            //echo "My level: $my_level, to level: $to_level<br/>";
            $difference      = $to_level - $my_level;
            //echo "Difference: $difference<br/>";

            $jur_sql = "SELECT aus_id, aus_name, jur_level_{$jur_class}.jur_name AS jur_name, jur_level_{$jur_class}.jur_class AS jur_class FROM admin_user\n";

            if($my_level >= $to_level){
                addError("", $lang->getLang("Invalid jurisdiction level"));
            }
            if(!areErrors()){
                $first = true;
                while($difference > 0){
                    //echo "Level: " . $jur_levels[$difference] . "<br/>";
                    $jur_sql .= " JOIN jurisdiction jur_level_{$jur_levels[$my_level+$difference]} ON \n";
                    if($first){
                        $jur_sql .= " aus_jurisdiction_id = jur_level_{$jur_levels[$my_level+$difference]}.jur_id\n";
                        $first = false;
                    }
                    else{
                        $jur_sql .= " jur_level_" . $jur_levels[$my_level+$difference] . ".jur_id = jur_level_{$jur_levels[$my_level+$difference + 1]}.jur_parent_id\n";
                    }
                    $difference--;
                }
                $jur_sql  .= " WHERE jur_level_{$jur_class}.jur_class = '" . $dbh->escape($jur_class) . "' AND jur_level_{$jur_levels[$my_level + 1]}.jur_parent_id = $my_jurisdiction";
                if(!empty($jur_id)){
                    $jur_sql .= " AND jur_level_{$jur_class}.jur_id = $jur_id";
                }
                $jur_sql  .= " ORDER BY LOWER(TRIM(aus_name))";
                $receivers = $dbh->exec($jur_sql);
            }
        }

        if(!areErrors()){
        $special_words = array(
            "\[name\]" => "aus_name",
            "\[jur\]"  => "jur_name",
            "\[type\]" => "jur_class"
        );

        $counter = 0;
        $message_in_one_insert = 50;
        if($new_email){
            $sql .= "INSERT INTO admin_user_mailbox VALUES ";
            while($receivers->hasNext()){
                $rec = $receivers->next();
                $msg      = $request["body"];
                $subject = $request["subject"];

                foreach($special_words as $key => $replace){
                    $subject = eregi_replace("$key", ereg_replace("[^a-zA-Z0-9]", " ", $rec[$replace]), $subject);
                    $msg     = eregi_replace("$key", ereg_replace("[^a-zA-Z0-9]", " ", $rec[$replace]), $msg);
                }

                $msg_id = $dbh->nextSequence("AUM_ID_SEQ");
                $sql .= "  ($msg_id, $admin_user_id, " . $rec["aus_id"] . ", '" . $dbh->escape($subject) . "', '" . $dbh->escape($msg) . "', '" . $dbh->escape(getIpAddress()) . "', '0', CURRENT_TIMESTAMP, NULL, $priority),";

                //echo "Message to " . $rec["aus_name"] . " (" . $rec["jur_class"] . " " . $rec["jur_name"] . ") sent correctly<br/>";

                $counter++;
            }
            $sql= substr($sql, 0, -1);

            if($counter > 0){
                //$sql = "SELECT * FROM dual";

                $res = $dbh->exec($sql);
            }


        }else{
            $msg_id = $dbh->nextSequence("AUM_ID_SEQ");
            $sql = "INSERT INTO admin_user_mailbox VALUES ($msg_id, $admin_user_id, $to, '" . $dbh->escape($request["subject"]) . "', '" . $dbh->escape($request["body"]) . "', '" . $dbh->escape(getIpAddress()) . "', '0', CURRENT_TIMESTAMP, NULL, $priority)";

            $res = $dbh->exec($sql);
        }

         if($res > 0){
        ?>
            <script type="text/javascript">
                alert("Message <?=ereg_replace('"', '\"', $request["subject"])?> sent correctly to administration");
                location.href = "<?=refPage('message/manager')?>";
            </script>
        <?php
        }
        }
        }
         }else{
            showErrors();

            $default = array(
                "aum_subject"  => $request["subject"],
                "aum_body"     => $request["body"],
                "aum_level"    => $request["priority"]
            );

            printMailForm($default, !$new_email);
        }
        }


        break;
    case "delete":

    case "read":
        $sql = "SELECT
              aum_id, aum_send_date, 
              f.aus_id AS from_id, f.aus_name AS from_name, fj.jur_class AS from_jurclass, 
              fj.jur_name AS from_jurname,
              t.aus_id AS to_id, t.aus_name AS to_name, tj.jur_class AS to_jurclass, 
              tj.jur_name AS to_jurname,
              aum_subject, aum_body, aum_level, aum_status 
            FROM 
              admin_user_mailbox
            JOIN admin_user f ON f.aus_id = aum_from
            JOIN jurisdiction fj ON fj.jur_id = f.aus_jurisdiction_id
            JOIN admin_user t ON t.aus_id = aum_to
            JOIN jurisdiction tj ON tj.jur_id = t.aus_jurisdiction_id
            WHERE aum_status <= 2 AND (aum_to = $admin_user_id OR aum_from = $admin_user_id) AND aum_id = $msg_id";

        $rs = $dbh->exec($sql);

        if($rs->hasNext()){
            $row = $rs->next();
            ?>
            <div id="message">
                <div class="header">
                    <div class="subject"><?=$lang->getLang('From')?>: &nbsp; <?=getMailFromName($row)?></div>
                    <div class="subject"><?=$lang->getLang('To')?>: &nbsp; <?=getMailToName($row)?></div>
                    <br/>
                    <div class="subject"><?=$lang->getLang('Subject')?>: <?=$row["aum_subject"]?></div>
                    <div class="date"><?=date("d F Y - H:i", strtotime($row["aum_send_date"]))?></div>
                    <div class="body_read"><?=$row["aum_body"]?></div>
                </div>
                <?php if($row["to_id"] == $admin_user_id && !empty($row["to_id"])) : ?>
                    <div class="response">
                        <?php
                        printMailForm($row, true);
                        ?>
                    </div>
                <?php endif; ?>
            </div>
        <?php
        }else{
            echo $lang->getLang("Message not found");
            dieWithError($lang->getLang("Message not found"));
        }

        readMessage($row["aum_id"]);
        echo "<br/><br/>$message_list_link";
        break;
    case "sent":
        ?>
        <h2>Sent messages</h2>
        <?php
        $sql = "SELECT aum_id, aum_send_date, aum_subject, aum_level, aum_status, t.aus_id AS to_id, t.aus_name AS to_name, tj.jur_class AS to_jurclass,  tj.jur_name AS to_jurname
           FROM admin_user_mailbox 
           JOIN admin_user t ON t.aus_id = aum_to
           JOIN jurisdiction tj ON tj.jur_id = t.aus_jurisdiction_id
           WHERE aum_status < 2 AND aum_from = $admin_user_id ORDER BY aum_id DESC";
        $rs = $dbh->exec($sql);

        $c = 0;
        $last_date = "";
        if($rs->hasNext()){
            while($rs->hasNext()){
                $row  = $rs->next();
                $date = strtotime($row["aum_send_date"]);
                $day  = date("d F Y", $date);
                if($day != $last_date){
                    ?>
                    <div class="checkbox"><input type="checkbox"/></div>
                    <div class="daterow sent"><?=$day?></div>
                    <?php
                    $last_date = $day;
                }
                ?>
                <div class="checkbox"><input type="checkbox"/></div>
                <div class="row level_<?=$row["aum_level"]?> <?=(($c++%2==0)?("odd"):("even"))?><?=getMailStyle($row);?>" onclick="readMessage(<?=$row["aum_id"]?>);">
                    <div class="status"><?=getMailIcon($row)?></div>
                    <div class="sender"><?=getMailToName($row)?></div>
                    <div class="subject"><?=$row["aum_subject"]?></div>
                    <div class="date"><?=date("H:i", $date)?></div>
                    <br style="clear:both"/>
                </div>
            <?php
            }
        }else{
            ?>
            <h3><?=$lang->getLang("No messages in inbox")?></h3>
        <?php
        }
        break;
    case "bin":
        ?>
        <h2><img src='<?=image_dir?>/bin.gif'/> Recycle Bin</h2>
        <?php
        $sql = "SELECT aum_id, aum_send_date, f.aus_name AS from_name, fj.jur_name AS from_jurname, aum_subject, aum_level, aum_status, fj.jur_class AS from_jurclass
           FROM admin_user_mailbox 
           JOIN admin_user f ON f.aus_id = aum_from 
           JOIN jurisdiction fj ON fj.jur_id = f.aus_jurisdiction_id
           WHERE aum_status = 2 AND aum_to = $admin_user_id ORDER BY aum_id DESC";
        $rs = $dbh->exec($sql);

        $c = 0;
        $last_date = "";
        if($rs->hasNext()){
            ?>
            <form action="<?=refPage("message/manager&action=recycle")?>"  method="POST">
                <a href="javascript:void(0);" onclick="checkAll(document.forms[0])">Select All</a> | <a href="javascript:void(0);" onclick="uncheckAll(document.forms[0])">Select None</a>
                <input type="Submit" value="Recycle selected"/>

                <?php
                while($rs->hasNext()){
                    $row  = $rs->next();
                    $date = strtotime($row["aum_send_date"]);
                    $day  = date("d F Y", $date);
                    if($day != $last_date){
                        ?>
                        <div class="daterow inbox"><?=$day?></div>
                        <?php
                        $last_date = $day;
                    }
                    ?>
                    <div class="checkbox">
                        <input name="message[<?=$row["aum_id"]?>]" type="checkbox" <?=(($row["aum_status"] > 0)?(""):('disabled="disabled"'))?>/>
                    </div>
                    <div class="row level_<?=$row["aum_level"]?> <?=(($c++%2==0)?("odd"):("even"))?><?=getMailStyle($row);?>" onclick="readMessage(<?=$row["aum_id"]?>);">
                        <div class="status">&nbsp;<?=getMailIcon($row)?></div>
                        <div class="sender"><?=getMailFromName($row)?></div>
                        <div class="subject"><?=$row["aum_subject"]?></div>
                        <div class="date"><?=date("H:i", $date)?></div>
                        <br style="clear:both"/>
                    </div>
                <?php
                }
                ?>
                <a href="javascript:void(0);" onclick="checkAll(document.forms[0])">Select All</a> | <a href="javascript:void(0);" onclick="uncheckAll(document.forms[0])">Select None</a>
                <input type="Submit" value="Recycle selected"/>
            </form>
        <?php
        }else{
            ?>
            <h3><?=$lang->getLang("No messages in inbox")?></h3>
        <?php
        }
        ?>
        <?php
        break;
    default:
        ?>
            <h2><img src='<?=image_dir?>/email_new.gif'/> Inbox</h2>
        <?php
        $sql = "SELECT aum_id, aum_send_date, f.aus_name AS from_name, fj.jur_name AS from_jurname, aum_subject, aum_level, aum_status, fj.jur_class AS from_jurclass
           FROM admin_user_mailbox 
           JOIN admin_user f ON f.aus_id = aum_from 
           JOIN jurisdiction fj ON fj.jur_id = f.aus_jurisdiction_id
           WHERE aum_status < 2 AND aum_to = $admin_user_id ORDER BY aum_id DESC";
        $rs = $dbh->exec($sql);
        $c = 0;
        $last_date = "";
        if($rs->hasNext()){
            ?>
            <form action="<?=refPage("message/manager&action=delete")?>"  method="POST">
                <a href="javascript:void(0);" onclick="checkAll(document.forms[0])">Select All</a> | <a href="javascript:void(0);" onclick="uncheckAll(document.forms[0])">Select None</a>
                <input type="Submit" value="Delete selected"/>
                <?php
                while($rs->hasNext()){
                    $row  = $rs->next();
                    $date = strtotime($row["aum_send_date"]);
                    $day  = date("d F Y", $date);
                    if($day != $last_date){
                        ?>
                        <div class="daterow inbox"><?=$day?></div>
                        <?php
                        $last_date = $day;
                    }
                    ?>
                    <div class="checkbox">
                        <input name="message[<?=$row["aum_id"]?>]" type="checkbox" <?=(($row["aum_status"] > 0)?(""):('disabled="disabled"'))?>/>
                    </div>
                    <div class="row level_<?=$row["aum_level"]?> <?=(($c++%2==0)?("odd"):("even"))?><?=getMailStyle($row);?>" onclick="readMessage(<?=$row["aum_id"]?>);">
                        <div class="status">&nbsp;<?=getMailIcon($row)?></div>
                        <div class="sender"><?=getMailFromName($row)?></div>
                        <div class="subject"><?=$row["aum_subject"]?></div>
                        <div class="date"><?=date("H:i", $date)?></div>
                        <br style="clear:both"/>
                    </div>
                <?php
                }
                ?>
                <a href="javascript:void(0);" onclick="checkAll(document.forms[0])">Select All</a> | <a href="javascript:void(0);" onclick="uncheckAll(document.forms[0])">Select None</a>
                <input type="Submit" value="Delete selected"/>
            </form>
        <?php
        }else{
            ?>
            <?=$lang->getLang("No messages in inbox")?>
        <?php
        }
        ?>
        <?php
        break;
}

?>
<br/>
<?=$default_link?>

</div>