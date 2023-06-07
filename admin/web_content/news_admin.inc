<?check_access('manage_news',$doredirect=true);?>
<?
function updateNews ($new_id, $new_title, $new_short_description, $new_body, $new_start_date, $new_end_date)
{  global $dbh;

    $new_title = escapeSingleQuotes($new_title);
    $new_short_description = escapeSingleQuotes($new_short_description);
    $new_body = escapeSingleQuotes($new_body);

    $sql  = "select new_id, new_title, new_short_description";
    $sql .= ", new_body, new_start_date, new_end_date, new_create_date";
    $sql .= ", new_modified_date from news where new_id = $new_id";
    $rs = $dbh->exec($sql);
    $row = $rs->next();

    $prev_new_id 			   = $row['new_id'];
    $prev_new_title 			   = escapeSingleQuotes($row['new_title']);
    $prev_new_short_description = escapeSingleQuotes($row['new_short_description']);
    $prev_new_body 			   = escapeSingleQuotes($row['new_body']);
    $prev_new_start_date 	   = $row['new_start_date'];
    $prev_new_end_date 		   = $row['new_end_date'];

    if ( $prev_new_title != $new_title )
        $log .= "title:$prev_new_title=$new_title|";
    if ( $prev_new_short_description != $new_short_description )
        $log .= "new_short_description:$prev_new_short_description=$new_short_description|";
    if ( $prev_new_body != $new_body )
        $log .= "new_body:$prev_new_body=$new_body|";
    if ( shortDate($prev_new_start_date) != shortDate($new_start_date) )
        $log .= "new_start_date:$prev_new_start_date=$new_start_date|";
    if ( shortDate($prev_new_start_date) != shortDate($new_start_date) )
        $log .= "new_end_date:$prev_new_start_date=$new_start_date";



    $sql  = " update news";
    $sql .= "  set new_title = '$new_title',";
    $sql .= "      new_short_description = '$new_short_description',";
    $sql .= "      new_body = '$new_body',";
    $sql .= "      new_start_date = '$new_start_date',";
    $sql .= "      new_end_date = '$new_end_date',";
    $sql .= "      new_create_date = new_create_date,";
    $sql .= "      new_modified_date = NOW()";
    $sql .= " where new_id = $new_id";

    $dbh->begin();

    if ( $log)
    {  if ( substr($log, -1) == '|' )
        $log = substr($log, 0, strlen($log)-1);
        doAdminUserLog($_SESSION['admin_id'], 'modify_news', $log, $pun_id="NULL");
    }


    $dbh->exec($sql);
    $dbh->commit();
}


function saveNews ($new_id, $new_title, $new_short_description, $new_body, $new_start_date, $new_end_date)
{  global $dbh;

    $next_news_id = $dbh->nextSequence('seq_new_id');

    $sql  = "insert into news ( new_id, new_title, new_short_description";
    $sql .= ", new_body, new_start_date, new_end_date, new_create_date";
    $sql .= ", new_modified_date )";
    $sql .= " values ($next_news_id, '$new_title', '$new_short_description'";
    $sql .= ", '$new_body', '$new_start_date','$new_end_date',";
    $sql .= "  CURRENT_TIMESTAMP, CURRENT_TIMESTAMP)";

    $dbh->begin();
    $dbh->exec($sql);

    $log = "new_id:$next_news_id|new_title:$new_title|new_short_description:$new_short_description|";
    $log .= "new_body:$new_body|new_start_date:$new_start_date|new_end_date:$new_end_date";

    doAdminUserLog($_SESSION['admin_id'], 'add_news', $log, $pun_id="NULL");

    $dbh->commit();
    return $next_news_id;
}


function deleteNews ($new_id)
{  global $dbh;

    $rs = $dbh->exec("select new_title from news where new_id = $new_id");

    if ( $rs->getNumRows() )
    {  $row = $rs->next();
        $new_title = $row['new_title'];
        $log = "new_id:$new_id|new_title:$new_title";

        $dbh->begin();
        $dbh->exec("delete from news where new_id = $new_id");   //delete news item from table
        doAdminUserLog($_SESSION['admin_id'], 'delete_news', $log, $pun_id="NULL"); //log delete
        $dbh->commit();
        return $dbh->getAffectedRows();
    }
    return 0;
}


function getFieldValues ($new_id="")
{  global $dbh;
    $row = array();

    $sql  = "select new_id, new_title, new_short_description, new_body, new_start_date,";
    $sql .= "  new_end_date, new_start_date, new_end_date";
    $sql .= " from news";

    if ( $new_id )
        $sql .= " where new_id = $new_id";

    $sql .= " order by new_title";

    $rs = $dbh->exec($sql);
    $num_rows = $dbh->getNumRows();

    if ( $num_rows > 0 )
        $row = $rs->next();
    return $row;
}


function newsSelector( $news_records, $num_recs)
{  global $new_id;    //current global news record id
    echo "<select name=new_id onChange='this.form.curr_rec_index.value=this.selectedIndex;this.form.submit(); return true'>";

    if ( $num_recs < 1 )
        echo '<option value="">[No News Items]';
    else
    {
        for ( $i=0; $i<$num_recs; $i++ )
        {
            $l_new_id  = $news_records[$i]['new_id'];
            $new_title = $news_records[$i]['new_title'];
            if ( strlen($new_title) > 40 )
                $new_title = substr($new_title, 0, 40)."...";


            echo "<option value=$l_new_id ";
            if ( $new_id == $l_new_id  )
                echo " selected";
            echo ">$new_title";
        }
    }
    echo "</select>";
}


function updateScrollList()
{
    global $dbh;

    $today = dbDateToday();
    //get all
    $sql  = "SELECT new_id, new_title, new_short_description" .
        ", new_start_date FROM news " .
        " WHERE '$today' BETWEEN new_start_date AND new_end_date" .
        " ORDER BY new_start_date desc, new_title";

    $rs   = $dbh->exec($sql);
    $num_rows = $rs->getNumRows();

    if ( $num_rows > 0 )
    {  $f = fopen(SITE_ROOT.'/html/secure/news-scroll.txt', 'w');

        while ( $rs->hasNext() )
        {  $row = $rs->next();
            $line = shortDate($row['new_start_date']).' - '.$row['new_title'].'|'.ereg_replace("\n", " ", ereg_replace("\r\n", " ",$row['new_short_description'])). '|'.nonsecure_host.'/news.html?view_mode=detail&new_id='.$row['new_id'];
            fputs($f, "$line\n");
        }
        fclose($f);
    }
}


globalise('mode');
globalise('new_start_day');
globalise('new_start_month');
globalise('new_start_year');
globalise('new_end_day');
globalise('new_end_month');
globalise('new_end_year');
globalise('new_title');
globalise('new_short_description');
globalise('new_body');
globalise('curr_rec_index');
globalise('new_id');

if ( !$mode ) $mode = "view";
?>
    <div class=bodyHD>Website News</div>
    <br>
    <div class=bodyHD2>
        <?
        if ( 'change' == $mode )
            echo "Change News Item";
        elseif ( 'add' == $mode )
            echo "Add News Item";
        else
            echo "View News Item";
        ?>
    </div>
<?

switch ( $mode )
{ case "update":
    $new_start_date = toDatabaseTimestamp($new_start_day, $new_start_month, $new_start_year);
    $new_end_date = toDatabaseTimestamp($new_end_day, $new_end_month, $new_end_year);
    updateNews ($new_id, $new_title, $new_short_description, $new_body, $new_start_date, $new_end_date);
    break;
    case "save":
        $new_start_date = toDatabaseTimestamp($new_start_day, $new_start_month, $new_start_year);
        $new_end_date = toDatabaseTimestamp($new_end_day, $new_end_month, $new_end_year);
        $new_id = saveNews ($new_id, $new_title, $new_short_description, $new_body, $new_start_date, $new_end_date);
        break;
    case "delete":
        deleteNews ($new_id);
}

if ( 'update' == $mode || 'save' == $mode || 'delete' == $mode )
    updateScrollList();

if ( $mode != "add" )
{  //get all news records
    $sql  = "select new_id, new_title, new_short_description, new_body";
    $sql .= ", new_start_date, new_end_date, new_create_date,";
    $sql .= "  new_modified_date from news order by new_title";
    $rs   = $dbh->exec($sql);
    $num_rows = $rs->getNumRows();

    //store records(arrays) in an array - $news_records becomes a 2D array
    $i=0;
    while ( $rs->hasNext() )
    {  $row = $rs->next();
        if ( $mode == "save" )
            if ( $row['new_id'] == $new_id )
                $curr_rec_index = $i;
        $news_records[$i] = $row;
        $i++;
    }
    $num_recs = $num_rows;;

    if ( !$news_records[$curr_rec_index] ) //record with cuurent array index doesn't exist
        if ( $news_records[$curr_rec_index-1] )
            $curr_rec_index--;
        else
            $curr_rec_index = 0;

    //get field values for displaying
    $new_id                = $news_records[$curr_rec_index]['new_id'];
    $new_title             = replace_quotes(ereg_replace("\"", "&quot;", $news_records[$curr_rec_index]['new_title']));
    $new_short_description = replace_quotes(ereg_replace("\"", "&quot;", $news_records[$curr_rec_index]['new_short_description']));
    $new_body              = replace_quotes(ereg_replace("\"", "&quot;", $news_records[$curr_rec_index]['new_body']));

    $new_start_date        = formatDate($news_records[$curr_rec_index]['new_start_date']);
    $new_start_date_arr    = splitDate($news_records[$curr_rec_index]['new_start_date']);
    $new_start_day         = $new_start_date_arr["day"];
    $new_start_month       = $new_start_date_arr["month"];
    $new_start_year        = $new_start_date_arr["year"];

    $new_end_date          = formatDate($news_records[$curr_rec_index]['new_end_date']);
    $new_end_date_arr      = splitDate($news_records[$curr_rec_index]['new_end_date']);
    $new_end_day           = $new_end_date_arr["day"];
    $new_end_month         = $new_end_date_arr["month"];
    $new_end_year          = $new_end_date_arr["year"];

    $new_create_date       = $news_records[$curr_rec_index]['new_create_date'];
    $new_modified_date     = $news_records[$curr_rec_index]['new_modified_date'];
}

if ( !($mode == "change" || $mode == "add") )
    $mode = "view";

/*************** NEWS ADMIN FORM START ***************/
?>
    <script language="JavaScript">
        //<!--

        function allFieldsFilled(fob)
        {
            var str = "";
            if ( fob.new_title.value == "" )
                str += "\nTitle";
            if ( fob.new_short_description.value == "" )
                str += "\nShort Description";
            if ( fob.new_body.value == "" )
                str += "\nBody";
            if ( str != "" )
            {   alert("Please ensure the following fields are completed\n" + str);
                return false;
            }
            return true;
        }


        function doSubmit(mode)
        {
            var formobj = document.forms.news_admin;
            var msg = "";

            formobj.mode.value = mode;

            switch ( mode )
            {
                case "save":
                    if ( allFieldsFilled(formobj)  )
                        if ( window.confirm("Save News Item?") )
                            formobj.submit();
                    break;
                case "update":
                    if ( ChangesMade )
                    { if ( allFieldsFilled(formobj) )
                        if ( window.confirm("Update News Item?") )
                            formobj.submit();
                    }
                    else
                        formobj.submit();
                    break;
                case "delete":
                    if ( window.confirm("Delete News Item?") )
                        formobj.submit();
                    break;
                case "cancel":
                    if ( ChangesMade )
                    {  if ( window.confirm("Discard Changes?") )
                        formobj.submit();
                        break;
                    }    //if no changes made, controls falls to default (submit form)
                default:
                    formobj.submit();
            }
        }

        var ChangesMade=false;

        //-->
    </script>
<?
$num_news_items = $dbh->countQuery("select count(*) from news");

if ( $num_news_items > 0 || 'add' == $mode )
{
    ?>
    <? form_head(SELF, "news_admin","GET") ?>
    <? if ( $mode != "view" ) { ?>
    <input type=hidden name="new_id" value=<? echo $new_id;?>>
<? } ?>
    <input type=hidden name=mode value=<? echo $mode;?>>
    <input type=hidden name=curr_rec_index value=<? echo $curr_rec_index;?>>
    <table cellspacing="0" cellpadding="0" border="0" background="<?=image_dir?>/clear.gif">
        <tr>
            <td>
                <table cellpadding=4 cellspacing=1 border=0>

                    <tr>
                        <td class=label width=95>Title</td>
                        <td width=400 class=content valign=top>
                            <? if ( $mode == "view")
                                newsSelector($news_records, $num_recs);
                            else
                            {
                                ?>   <input name=new_title value="<? if ( $mode != "add" ) echo $new_title; ?>" type=text <?

                                if (browserName() == "netscape" )
                                {
                                    ?> size=48<?
                                }
                                else
                                {
                                    ?> size=60<?
                                } setEditable();
                                ?> maxlength=100>
                            <?   } ?></td>
                    </tr>
                    <?         if ( $mode != "add" ) { ?>
                        <tr>
                            <td class=label width=95 valign=top>Created</td>
                            <td class=content><?=formatDate($new_create_date); ?></td>
                        </tr>
                        <tr>
                            <td class=label width=95 valign=top>Created</td>
                            <td class=content><?=formatDate($new_modified_date); ?></td>
                        </tr>
                    <? } ?>
                    <tr>
                        <td class=label valign=top>Start Date</td>
                        <td class=content><?
                            if ( $mode == "view" )
                                echo "$new_start_date";
                            else
                                date_selector("new_start_day", "new_start_month", "new_start_year", date('Y'), date('Y')+1, "today");
                            ?></td>
                    </tr>
                    <tr>
                        <td class=label valign=top>End Date</td>
                        <td class=content><?
                            if ( $mode == "view" )
                                echo "$new_end_date";
                            else
                                date_selector("new_end_day", "new_end_month", "new_end_year", date('Y'), date('Y')+3, "todaynextyear");
                            ?></td>
                    </tr>
                    <tr>
                        <td  class=label valign=top>Short Description</td>
                        <td  class=content>
                            <textarea name=new_short_description<?
                                if (browserName() == "netscape" ) {
                                    ?> cols=47<?
                                } else {
                                    ?> cols=60 <?
                                }?> rows=8 wrap="virtual" <? setEditable(); ?>><? if ($mode != "add") echo $new_short_description; ?></textarea></td>
                    </tr>
                    <tr>
                        <td class=label valign=top>Body</td>
                        <td class=content>
                            <textarea name=new_body<?
                                if (browserName() == "netscape" ) {
                                    ?> cols=47<?
                                } else {
                                    ?> cols=60<?
                                }?> rows=25 wrap="virtual" <? setEditable(); ?>><? if ($mode != "add") echo $new_body; ?></textarea></td>
                    </tr>
                </table>
            </td>
            <td valign=top align=left>
                <?   if ( $mode == "view" )
                {?>      <a href="javascript:doSubmit('add')"><img src="<?=image_dir;?>/button_add.gif" border=0></a><br>
                    <?      if ( $new_id )
                {?>    <a href="javascript:doSubmit('change')"><img src="<?=image_dir;?>/button_change.gif" border=0></a><br>
                    <a href="javascript:doSubmit('delete')"><img src="<?=image_dir;?>/button_delete.gif" border=0></a><br>
                <?
                }
                }
                elseif ( $mode == "change" )
                {?> <a href="javascript:doSubmit('update')"><img src="<?=image_dir;?>/button_update.gif" border=0></a><br>
                    <a href="javascript:doSubmit('cancel')"><img src="<?=image_dir;?>/button_cancel.gif" border=0></a>
                <?
                }
                elseif ( $mode == "add" )
                {?>  <a href="javascript:doSubmit('save')"><img src="<?=image_dir;?>/button_save.gif" border=0></a><br>
                <a href="javascript:doSubmit('cancel')"><img src="<?=image_dir;?>/button_cancel.gif" border=0>
                    <?  }  ?>
            </td>
        </tr>
    </table>
<?
}
else
{
    ?>
    <P>
        There are currently no news items
<P>
    <? form_head() ?>
    <input type="hidden" name="mode" value="add">
    <input type=image src="<?=image_dir;?>/button_add.gif" border=0>
    </form>
<?
}
?>