<?php
	define('CHANGE_CROUPIER_CODE', 0);
	define('SEARCH_CROUPIER_NAME', 1);
	if(isset($_GET['reqCode'])){
		$request = $_GET['reqCode'];
		switch($request){
			case CHANGE_CROUPIER_CODE:
				$code = $_GET['code'];
				$crp_id = $_GET['cid'];
				saveNewCode($code, $crp_id);
				break; 
			case SEARCH_CROUPIER_NAME:
				$value = $_GET['value'];
				searchCroupier($value);
				break;
		} 
	}
	function searchCroupier($value){
		global $dbh;
		$sql = "Select crp_id, crp_first_name, crp_last_name from croupier where LOWER(crp_first_name) LIKE LOWER('$value%') OR LOWER(crp_last_name) LIKE LOWER('$value%')";
		$rs = $dbh->doCachedQuery($sql,3000);
		$valueSearched = "";
		while($rs->hasNext()){
			$row = $rs->next();
			$valueSearched .= $row['crp_id'].":".$row['crp_last_name']." ".$row['crp_first_name'].";";
		}
		$valueSearched = substr($valueSearched, 0, strlen($valueSearched)-1);
		die($valueSearched);
	}
	function saveNewCode($code,$crp_id){
		global $dbh;
		if($crp_id != "" && $code != "" && strlen($code) == 5 && preg_match("/^[0-9]{5}$/",$code)){
			$sql = "Select count(*) from croupier where crp_code = 'CPR_$code'";
			$count = $dbh->queryOne($sql);
			settype($count,'integer');
			if($count > 0){
				die("-2");
			}
			$sql = "Update croupier set crp_code = 'CPR_$code' where crp_id = $crp_id";
			$res = $dbh->exec($sql);
			if($res == 1){
				die("1");
			}else{
				die("0");
			}
		}else{
			die("-1");
		}
	}
?>