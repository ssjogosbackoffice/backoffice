<HTML>
<HEAD>
<TITLE>Insert New Note</TITLE>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/style.css" title="core_style" />
</head>
<body>
<?php
globalise('customer_id');
globalise('customer_name');
globalise('note');
globalise('action');

function insertnewnote(){
	global $dbh;
	global $customer_id;
	global $note;
	$id= $dbh->nextSequence('AUN_ID_SEQ');
	$note=$dbh->prepareString($_POST['note']);
	$sql="Insert into admin_user_note (aun_id,aun_pun_id,aun_aus_id,aun_date,aun_note) values " .
	     " ($id,$customer_id,{$_SESSION['admin_id']},CURRENT_TIMESTAMP,$note)    ";
  
	$rs=$dbh->exec($sql);
	if($rs){
		echo "<b style='color:green;'>Nota Inserita con successo</b>";
	}else{
		echo "<b style='color:red;'>Errore nell'inserimento della nota</b>";
	}		
}

if($action=="insertnote"){
  insertnewnote();	
}else{
 ?>
   <form name="note" method="POST" action="<?='/customers/customer_add_note.php?action=insertnote'?>" >
      <table>
        <tr> <td colspan="2" class="formheading"> Insert New Note</td></tr>
       <tr>
         <td class="label"> User : </td><td> <?=$customer_name?> <input type="hidden" name="customer_id" value="<?=$customer_id?>" /></td>
       </tr>
       <tr>
         <td class="label"> Note : </td><td> <textarea name="note" rows="10" cols="50" wrap="off"></textarea></td>
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
