<?
ob_end_clean();

$customer_id   = $_REQUEST["customer_id"];
$remove_points = $_REQUEST["remove_points"];

settype($customer_id,   "integer");
settype($remove_points, "integer");


$punter_data = getCustomer($customer_id);

/*
echo "<pre>";
var_dump($punter_data);
echo "</pre>";
*/

if(empty($punter_data)){
  addError("", "Punter not found");
  showErrors();
  exit();
}

if($_REQUEST["form_submitted"] == "yes"){
  $sql = "UPDATE punter_credit SET pcr_funtokens = pcr_funtokens - $remove_points WHERE pcr_pun_id = $customer_id AND pcr_funtokens >= $remove_points";
  $rs = $dbh->exec($sql);
  
  if($rs == 1){
    jalert("Tolti correttamente $remove_points IPS all'utente " . $punter_data["username"]);
  }else{
    addError("", "Impossibile togliere $remove_points IPS all'utente " . $punter_data["username"] . " controllare la richiesta.");
    showErrors();
  }
  
}

$sql = "SELECT lvl_id, lvl_name, plv_points, TO_CHAR(plv_creation, 'YYYY-MM-DD') AS plv_creation, TO_CHAR(plv_last_update, 'YYYY-MM-DD') AS plv_last_update, plv_total_points, pcr_funtokens, lvl_name FROM punter_levels JOIN punter_credit ON pcr_pun_id = plv_punid JOIN levels ON lvl_id = plv_lvlid WHERE plv_punid = $customer_id";

$rs = $dbh->exec($sql);

if(!$rs->hasNext()){
  addError("", "Punter not found");
  showErrors();
  exit();  
}

$punter_points = $rs->next();

?>

<table width="100%">
  <tr>
    <td class="label">Utente</td>
    <td class="content"><?=$punter_data["username"]?></td>
  </tr>
  <tr>
    <td class="label">IPS</td>
    <td class="content"><?=$punter_points["pcr_funtokens"]?></td>
  </tr>
  <tr>
    <td class="label">IPS Totali</td>
    <td class="content"><?=$punter_points["plv_total_points"]?> dal <?=$punter_points["plv_creation"]?></td>
  </tr>
  <tr>
    <td class="label">IPP</td>
    <td class="content"><?=$punter_points["plv_points"]?></td>
  </tr>
  <tr>
    <td class="label">Livello Utente</td>
    <td class="content">Liv. <?=$punter_points["lvl_id"]+1?> <b><?=$punter_points["lvl_name"]?></b>, assegnato il <?=$punter_points["plv_last_update"]?></td>
  </tr>
  
  <?form_head(SELF, "customer_points", "POST")?>
    <input type="hidden" name="customer_id" value="<?=$customer_id?>"/>
    <tr>
      <td class="label">Punti da togliere</td>
      <td class="content"><input type="text" name="remove_points" value="<?=$remove_points?>"/></td>
    </tr>    
    <tr>
      <td class="content">&nbsp;</td>
      <td class="content"><input type="submit" value="Rimuovi i punti"/></td>
    </tr>    

  </form>
  
</table>