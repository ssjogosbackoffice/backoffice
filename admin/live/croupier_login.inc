<script type="text/javascript">
    function comeToList(){
        location.href = "<?=$_SERVER['PHP_SELF']?>"+"?page=live/croupier&header_page=";
    }
</script>
<table border="0" cellpadding="0" cellspacing="0">
    <tr valign="top">
        <td>
            <?php
            check_access('live_croupier_login', $redirect=true);
            include_once("funclive.inc.php");

            globalize($from);
            globalize($to);

            $from = date("Y-m-d H:i", strtotime("now - 2 hours"));
            $to = date("Y-m-d H:i", strtotime("now"));

            if($_POST['form_submitted'] == "yes"){
                $from = $_POST['from'];
                $to = $_POST['to'];
            }
            setupDateInputJs("from", "to");
            ?>
            <div class="bodyHD">Login Croupier</div>

            <?php

            $sql = "Select crp_code, crl_logged_in, crl_logged_out, crl_ipaddress, gam_name, crp_first_name, crp_last_name, crl_note from croupier_log left join croupier on crp_code = crl_crp_code left join game on gam_id = crl_gam_id where crp_id = " . $_GET['cid'] . " AND crl_logged_in > '$from' and crl_logged_in < '$to'";
            $rs = $dbh->exec($sql);

            $cols_arr    = array('Croupier Code','Name', 'Log IN' ,'Log OUT', 'ip','Game','Note');

            $val_fmt_arr = array (
                'Croupier Code' => 'return substr($rec["crp_code"] ,4);',
                'Name'          => 'return $rec["crp_last_name"]. " " . $rec["crp_first_name"];',
                'Log IN'        => 'return $rec["crl_logged_in"];',
                'Log OUT'       => 'return $rec["crl_logged_out"];',
                'ip'            => 'return $rec["crl_ipaddress"];',
                'Game'          => 'return $rec["gam_name"];',
                'Note'          => 'return $rec["crl_note"];'
            );

            $cell_fmt_arr = array ();

            $numrow = $rs->getNumRows();
            if($numrow > 0){
                $qp = new QueryPrinter($rs);
                $qp->printTable($rs, $cols_arr, "", $val_fmt_arr, $cell_fmt_arr, 0, 0, "", $width="100%", 1, $numrow, false);
            }else{
                addError("", "No login found :)");
                showErrors();
            }

            ?>
        </td>
        <td width="50"></td>
        <td>
            <input type="button" value="<?=$lang->getLang("Back")?>" onclick="comeToList();"/>
        </td>
    </tr>
</table>
<?form_head(refFormPage($_GET["page"]) . "&cid=".$_GET['cid']."&action=show", "search", "POST");?>
<table border="0" cellpadding="0" cellspacing="0">
    <tr valign="top">
        <td>
            <table border="0" cellpadding="0" cellspacing="0">
                <tr>
                    <td colspan="3" class="formheading"><?=$lang->getLang("Date")?></td>
                </tr>
                <tr>
                    <td width="150" class="label"><?=$lang->getLang("From")?></td>
                    <td class="content"><?=createDateInput("from", $from, false, "search")?></td>
                </tr>
                <tr>
                    <td width="150" class="label"><?=$lang->getLang("Until")?></td>
                    <td class="content"><?=createDateInput("to", $to, false, "search")?></td>
                </tr>
                <tr>
                    <td><input type="submit" value="<?=$lang->getLang("Search")?>" /></td>
                </tr>
            </table>
        </td>
    </tr>
</table>