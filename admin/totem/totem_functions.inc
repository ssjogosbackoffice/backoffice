<?php
if(isset($_POST['block'])){
	$totemcode=$_POST['totem'];
	$ttm_act_status=$_POST['status'];
	$sql = "Update totem set ttm_comm_key = NULL
	, ttm_act_status = " . $ttm_act_status . "
	, ttm_access =  'deny'
	, ttm_note = NULL
	, ttm_ip =  NULL
	, ttm_club_id = NULL
	, ttm_pun_id = NULL
	, ttm_sess_id = NULL
	, ttm_last_login = NULL
	, ttm_conf_param = NULL
	WHERE UPPER(ttm_code) = UPPER('" . $totemcode . "')";
	$res = $dbh->exec ( $sql );
}

if(isset($_POST['access']))
{
	$totem=$_POST['totem'];
	$access=$_POST['access'];
	if(strtoupper ( $access ) == 'ALLOW')
	{
		$access='deny';
	}
	else 
	{
		$access='allow';
	}
	$sql = "Update totem set 
	ttm_access =  '$access'
	WHERE UPPER(ttm_code) = UPPER('" . $totem . "')";
	$res = $dbh->exec ( $sql );
}
	?>