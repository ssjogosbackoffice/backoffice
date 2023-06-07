<?php
PutEnv("NLS_LANG=AMERICAN_AMERICA.WE8ISO8859P1");
include_once 'include/sites.inc.php';
$poker_create_tournaments  = check_access('manage_badwords', true);

?>

<div class="bodyHD">Bad Words Manager</div>
<br/>
<?php
$action   = $_REQUEST["action"];
$id       = $_REQUEST["bad_id"];

settype($id, "integer");

switch ($action){
  case "save":
    $save_action = "save_new";
    globalize("bad_string");
    $bad_string = strtolower($bad_string);
    
    if(substr($bad_string, 0, 1) == "*"){
      $bad_string = substr($bad_string, 1);
      $bad_string = preg_replace('#\W#', '.', $bad_string);
      $bad_string = "*$bad_string";
    }
    
    $sql = "INSERT INTO banned_words VALUES (bad_id_seq.nextval, " . escapeAndQuote($bad_string, "string") . ")";
    $res = $dbh->exec($sql);

    if($res === 1){
      printInfoMessage("Bad word '$bad_string' inserted!");
    }else{
      printInfoMessage("Word $bad_string must be yet in the list");
    }
    
    clearErrors();
    showBadWords();
    break;
  case "delete":
    $sql = "DELETE banned_words WHERE bad_id = $id";
    $rs     = $dbh->exec($sql);
    printInfoMessage("Bad word deleted!");
    showBadWords();
    break;
  default:
    showBadWords();
    break;
}


function showBadWords(){
  global $dbh;

  ?>
  <table>
    <tr>
      <form name="news_editor" action="<?=refFormPage($_GET["page"] . "&action=save")?>" method="POST">
      <td class="content"><input type="text" name="bad_string"/></td>
      <td class="content"><input type="submit" value="Add"/></td>
      </form>
    </tr>
  </table>
  <?php
   
  $sql = "SELECT 
            bad_id, bad_string
          FROM 
            banned_words ORDER BY bad_string";
  
  $rs = $dbh->exec($sql);

  $numrow = $rs->getNumRows();
  if($numrow > 0){
    $cols_arr    = array("Bad Word", "Delete");
    $val_fmt_arr = array(
      "Bad Word" => 'return $rec["bad_string"];',
      "Delete"   => 'return getClassLink(refFormPage($_GET["page"] . "&action=delete&bad_id=" . $rec["bad_id"]), "Delete");'
    );
    
    $qp = new QueryPrinter($rs);
    $qp->printTable($rs, $cols_arr, "", $val_fmt_arr, null, 0, 0, "", $width="100%", 1, 2000, false);
  }else{
    addError("", "No bad words found :)");
    showErrors();
  }
  
}
?>