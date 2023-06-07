<?php

$sql = "SELECT pnu_id, pun_id, pun_username, pcr_credits FROM partner_users JOIN punter ON pun_id = pnu_local_id JOIN punter_credit ON pcr_pun_id = pnu_local_id ORDER BY LOWER(pun_username)";
$rs  = $dbh->exec($sql);
?>
<style media="print">
thead {
  display: table-header-group;
}

tbody tr td{
  border-bottom: 1px solid #000;
}

body {
  color : #000000; background : #ffffff; font-family : "Times New Roman", Times, serif; font-size : 12pt; 
}

</style>
<style media="print,screen">
thead{
  background-color: #FFCC00;
}

thead td{
  padding: 5px;
  font-weight:bold;
}

tbody td{
  padding: 2px;
}

.odd{
  background-color: #CCC;
}
</style>

<table>
  <thead>
    <tr>
      <td>Customer ID</td>
      <td>Nickname</td>
      <td>Balance</td>
    </tr>
  </thead>
  <tbody>
    <?php while($row = $rs->next()) : ?>
    <tr class="<?=(($i++%2==0)?("odd"):("even"))?>">
      <td><?=$row["pnu_id"]?></td>
      <td><?=$row["pun_username"]?></td>
      <td><?=getInDollars($row["pcr_credits"])?></td>
    </tr>
    <?php endwhile; ?>
  </tbody>
</table>