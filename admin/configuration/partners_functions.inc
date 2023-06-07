<?php
/**
 * @author Marian
 */
/*function getPartners() {
	global $dbh;
	$sql = "select jur_name, jur_status, jur_available_credit, jur_currency,cur_code_for_web, ptn.*
            from partners ptn, jurisdiction,currency
            where ptn_jur_id = jur_id
            AND jur_currency=cur_id
            ";
	$result = $dbh->doCachedQuery ( $sql, 0 );
	return $result;
}*/


// function checkWebsite($website) {
// 	$webtype = '';
// 	$web = strpos ( $website, "web://" );
// 	if ($web === false) {
// 		$sock = strpos ( $website, "socket://" );
// 		if ($sock === false) {
// 			$webservice = strpos ( $website, "webservice://" );
// 			if ($webservice !== false) {
// 				$webtype = "webservice://";
// 				$website = substr ( $website, 11 );
// 			}
// 		} else {
// 			$webtype = "socket://";
// 			$website = substr ( $website, 9 );
// 		}
// 	} else {
// 		$webtype = "web://";
// 		$website = substr ( $website, 6 );
// 	}
// 	$_SESSION ['webtype'] = $webtype;
// 	return $website;
// }


// function checkIp($ip){
// $ip=str_replace('|', ',', $ip);
// return $ip;
// }



function updatePartner($id,$name,$website,$ips,$notes,$club,$key,$status)
{
	global $dbh;
	$ips=str_replace(",",'|', $ips);
	$notes="gid:".$notes;
	$sql="Update partners set ptn_name='$name',ptn_website='$website',ptn_allowip='$ips',ptn_notes='$notes',ptn_jur_id='$club',ptn_key='$key',ptn_status='$status' where ptn_id='$id'";
	$dbh->exec($sql);
}



function delPartner($id){
	global $dbh;
	$sql="DELETE FROM partners where ptn_id='$id'";
	$result=$dbh->exec($sql);
}

function delProvider($id){
	global $dbh;
	$sql="DELETE FROM providers where pes_id='$id'";
	$result=$dbh->exec($sql);
}

//
//function addPartner($name,$website,$ips,$notes,$club,$key,$status)
//{
//	global $dbh;
//	if($name!=''){
//	$sql="SELECT * FROM partners where ptn_name='$name'";
//    if($result=$dbh->queryRow($sql))
//    {
//    	echo '1';
//    	die();
//   	}
//	$ptn=" select last_seq_id('PTN_ID_SEQ') as NEXTVAL ";
//	$rs2=$dbh->queryRow($ptn);
//	$ips=str_replace(",",'|', $ips);
//	$notes="gid:".$notes;
//
//		//$_REQUEST['exactResult']==2? true : null
//	$sql="INSERT INTO partners (ptn_id,ptn_name,ptn_website,ptn_allowip,ptn_notes,ptn_jur_id,ptn_key,ptn_external_id,ptn_status)
//			          VALUES('".$rs2['nextval']."','$name','$website', '$ips','$notes','$club','$key','".$rs2['nextval']."','$status')";
//	if($result=$dbh->exec($sql))
//	{
//		echo '2';
//		die();
//	}
//	else{
//		echo '3';
//		die();
//	}
//	}
//	else{
//		echo '3';
//		die();
//	}
//}

?>