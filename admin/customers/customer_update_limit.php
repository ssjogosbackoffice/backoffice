<HTML>
<HEAD>
<TITLE>Update Deposit Limit</TITLE>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/style.css" title="core_style" />
</head>
<body>
<?php
globalise('customer_id');
globalise('customer_name');
globalise('limit');
globalise('action');

function updateuserdepositlimit(){
	global $dbh;
	global $customer_id;
	global $limit;
	$sql="Update punter set PUN_DAILY_DEPOSITS_LIMIT=$limit where pun_id=$customer_id";
	$rs=$dbh->exec($sql);
	if($rs){
		echo "<b style='color:green;'>Limite Aggiornamto con successo</b>";
	}else{
		echo "<b style='color:red;'>Errore nell'Aggiornamto del limite</b>";
	}		
}

if($action=="update"){
  updateuserdepositlimit();	
}else{
 ?>
   <form name="note" method="POST" action="<?='/customers/customer_update_limit.php?action=update'?>" >
      <table>
        <tr> <td colspan="2" class="formheading"> Update Daily Allowance </td></tr>
       <tr>
         <td class="label"> User : </td><td> <?=$customer_name?> <input type="hidden" name="customer_id" value="<?=$customer_id?>" /></td>
       </tr>
       <tr>
         <td class="label"> limit : </td><td> <input type="text" name="limit" value=""/></td>
       </tr>
       <tr>
          <td colspan="2"> <input type="submit" name="salva" value="salva" /> </td>
       </tr>
      </table>
   </form>
 <?	
}
?>
</body>
</html>
