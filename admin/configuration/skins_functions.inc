<?php
require_once '../../includes/classes/Structure.class.inc';
function getSkins() {
	global $dbh;
	$sql = "Select * from skins";
	$result = $dbh->doCachedQuery ( $sql, 0 );
	return $result;
}
function updateSkin($props) {
	global $dbh;
	$sql = "Update skins
        SET
        skn_name=".$dbh->prepareString($props['name']).",
        skn_url=".$dbh->prepareString($props['websiteURL']).",
        skn_media_url=".$dbh->prepareString($props['mediaURL']).",
        skn_game_launcher=".$dbh->prepareString($props['gamelauncherURL']).",
        skn_betting_url=".$dbh->prepareString($props['bettingURL']).",
        skn_backoffice_url=".$dbh->prepareString($props['backofficeURL']).",
        skn_reg_activation_url= ".$dbh->prepareString($props['activationURL']).",
        skn_foldername= ".$dbh->prepareString($props['folder']).",
        skn_status=  ".$dbh->prepareString($props['status']).",
        skn_allowip=".$dbh->prepareString(implode("|",$props['ips'])).",
        skn_jur_id= ".$dbh->prepareString($props['clubs']).",
        skn_key= ".$dbh->prepareString($props['key']).",
        skn_email=".$dbh->prepareString($props['email']).",
        skn_smtp=".$dbh->prepareString($props['smtp']).",
        skn_pes_code_enabled= ".$dbh->prepareString(implode(",",$props['pes'])).",
        skn_params=  ".$dbh->prepareString($props['params']).",
        skn_gateway_response_url=  ".$dbh->prepareString($props['gatewayURL']).",
        skn_kiosk_url= ".$dbh->prepareString($props['kioskURL']).",
        skn_domain= ".$dbh->prepareString($props['domainURL']).",
        skn_banner_url= ".$dbh->prepareString($props['bannersURL'])." 
             where skn_id=".$dbh->prepareString($props['skinid']);
	return json_encode([$dbh->exec ( $sql )]);
}
function delSkin($id) {
	global $dbh;
	$sql = "DELETE FROM skins where skn_id='$id'";
	$result = $dbh->exec ( $sql );
}
function addSkin($props) {
	global $dbh;
	if ($props['name'] != '') {
		$sql = "SELECT * from skins where skn_name=".$dbh->prepareString($props['name']);
		if ($result = $dbh->queryRow ( $sql )) {
            return json_encode([-1,'Skin already exists']);
		}
		$dbh->begin();
		$sql = "SELECT last_seq_id('SKN_ID_SEQ') as NEXTVAL";
		$rs = $dbh->queryRow ( $sql );
		$sql = "INSERT INTO skins(
        skn_id,
        skn_name,
        skn_url,
        skn_media_url,
        skn_game_launcher,
        skn_betting_url,
        skn_backoffice_url,
        skn_reg_activation_url,
        skn_foldername,
        skn_status,
        skn_allowip,
        skn_jur_id,
        skn_key,
        skn_email,
        skn_smtp,
        skn_pes_code_enabled,
        skn_params,
        skn_gateway_response_url,
        skn_kiosk_url,
        skn_domain,
        skn_banner_url
        
        )
        VALUES('" . $rs ['nextval'] . "',
        ".$dbh->prepareString($props['name']).",
        ".$dbh->prepareString($props['websiteURL']).",
        ".$dbh->prepareString($props['mediaURL']).",
        ".$dbh->prepareString($props['gamelauncherURL']).",
        ".$dbh->prepareString($props['bettingURL']).",
        ".$dbh->prepareString($props['backofficeURL']).",
        ".$dbh->prepareString($props['activationURL']).",
        ".$dbh->prepareString($props['folder']).",
        ".$dbh->prepareString($props['status']).",
        ".$dbh->prepareString(implode("|",$props['ips'])).",
        ".$dbh->prepareString($props['clubs']).",
        ".$dbh->prepareString($props['key']).",
        ".$dbh->prepareString($props['email']).",
        ".$dbh->prepareString($props['smtp']).",
        ".$dbh->prepareString(implode(",",$props['pes'])).",
        ".$dbh->prepareString($props['params']).",
        ".$dbh->prepareString($props['gatewayURL']).",
        ".$dbh->prepareString($props['kioskURL']).",
        ".$dbh->prepareString($props['domainURL']).",
        ".$dbh->prepareString($props['bannersURL']).")";

		if ($result = $dbh->exec ( $sql )) {
            if($_REQUEST['addStructure']=='on'){

                $sql = "desc jurisdiction_payment_settings";
                $fields = $dbh->exec($sql);
                $casinoPercNation=[];
                $casinoPercRegion=[];
                $casinoPercDistrict=[];
                $casinoPercClub=[];
                while($fields->hasNext()) {
                    $field = $fields->next();

                    if(strpos($field['field'],"PERC") > 0){
                        if(strpos($field['field'],"BETTING") > 0){
                            continue;
                        }
                        $casinoPercNation[$field['field']]=90;
                        $casinoPercRegion[$field['field']]=75;
                        $casinoPercDistrict[$field['field']]=60;
                        $casinoPercClub[$field['field']]=45;


                    }
                }



                $currencyCode=$dbh->queryOne('Select cur_code from currency where cur_id='.$dbh->escape($_REQUEST['currency']));
                $nation=new Structure(null,'nation');
                $nation->setName($_REQUEST['name']."-".$currencyCode."-N");
                $nation->setCode(get_free_jurcode());
                $nation->setAccessType('BACKOFFICE');
                $nation->setCurrency($_REQUEST['currency']);
                $nation->setSkinId($rs ['nextval']);
                $nation->setCountry($_POST['country']);
                $nation->setCity($_POST['city']);
                $nation->setUTC($_REQUEST['utc']);
                $nation->setPercentualsCasino($casinoPercNation);
                $nation->setBettingLiveProfile(5);
                $nation->setBettingProfile(1);
                $nation->setAddress($_REQUEST['address1']);
                $nation->setZip($_REQUEST['postc']);
                $nation->setPhone($_REQUEST['phone']);
                $save=$nation->saveStructure();
                if($save[0]==1){
                    $region=new Structure(null,'region');
                    $region->setParentId($save[1]);
                    $region->setName($_REQUEST['name']."-".$currencyCode."-R");
                    $region->setCode(get_free_jurcode());
                    $region->setAccessType('BACKOFFICE');
                    $region->setCurrency($_REQUEST['currency']);
                    $region->setSkinId($rs ['nextval']);
                    $region->setCountry($_POST['country']);
                    $region->setCity($_POST['city']);
                    $region->setUTC($_REQUEST['utc']);
                    $region->setPercentualsCasino($casinoPercRegion);
                    $region->setBettingLiveProfile(5);
                    $region->setBettingProfile(1);
                    $region->setAddress($_REQUEST['address1']);
                    $region->setZip($_REQUEST['postc']);
                    $region->setPhone($_REQUEST['phone']);
                    $save=$region->saveStructure();
                    if($save[0]==1) {
                        $district=new Structure(null,'district');
                        $district->setParentId($save[1]);
                        $district->setName($_REQUEST['name']."-".$currencyCode."-D");
                        $district->setCode(get_free_jurcode());
                        $district->setAccessType('BACKOFFICE');
                        $district->setCurrency($_REQUEST['currency']);
                        $district->setSkinId($rs ['nextval']);
                        $district->setCountry($_POST['country']);
                        $district->setCity($_POST['city']);
                        $district->setUTC($_REQUEST['utc']);
                        $district->setPercentualsCasino($casinoPercDistrict);
                        $district->setBettingLiveProfile(5);
                        $district->setBettingProfile(1);
                        $district->setAddress($_REQUEST['address1']);
                        $district->setZip($_REQUEST['postc']);
                        $district->setPhone($_REQUEST['phone']);
                        $save=$district->saveStructure();
                        if($save[0]==1) {
                            $club=new Structure(null,'club');
                            $club->setParentId($save[1]);
                            $club->setName($_REQUEST['name']."-".$currencyCode."-C");
                            $club->setCode(get_free_jurcode());
                            $club->setAccessType('BACKOFFICE');
                            $club->setCurrency($_REQUEST['currency']);
                            $club->setSkinId($rs ['nextval']);
                            $club->setCountry($_POST['country']);
                            $club->setCity($_POST['city']);
                            $club->setUTC($_REQUEST['utc']);
                            $club->setPercentualsCasino($casinoPercDistrict);
                            $club->setBettingLiveProfile(5);
                            $club->setBettingProfile(1);
                            $club->setAddress($_REQUEST['address1']);
                            $club->setZip($_REQUEST['postc']);
                            $club->setPhone($_REQUEST['phone']);
                            $club=$club->saveStructure();
                            if($save[0]==1) {
                                $dbh->commit();
                                return $rs ['nextval'];
//                                return json_encode([1,'Successfully created skin']);
                            }
                            else{
                                $dbh->rollback();
                                return json_encode([-1,$save[1]]);
                            }
                        }
                        else{
                            $dbh->rollback();
                            return json_encode([-1,$save[1]]);
                        }
                    }
                    else{
                        $dbh->rollback();
                        return json_encode([-1,$save[1]]);
                    }
                }
                else{
                    $dbh->rollback();
                    return json_encode([-1,$save[1]]);
                }

                //$nation->setEndHours($_POST['hoursEnd']);
                //$nation->setStartHours($_POST['hoursStart']);
            }
            else{
                $dbh->commit();
                return $rs ['nextval'];
//                return json_encode([1,'Successfully created skin']);
            }
							//echo '1';
		} else {
            return json_encode([-1,'Unable to add skin']);
		}
	}
}

?>