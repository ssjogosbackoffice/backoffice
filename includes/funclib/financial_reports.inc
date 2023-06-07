<?php
define("CACHE_VALUE", 2678400);
/********************
 * FINANCIAL
 ********************/
include "JurisdictionsList.class.inc";

$jur_list = JurisdictionsList::getInstance($dbh);
  
function getFinancialByClub() {
  global $dbh, $jurisdiction , $jur_list;
  global $start_date, $end_date, $today_date, $yesterday_date, $search_today;
  global $jur_list;

  $archive_date = (($search_today) ? $yesterday_date : $end_date);
  $sql = "SELECT c.jur_id as clubid, c.jur_name as club, d.jur_name as district" .
         "     , r.jur_name as region, n.jur_name as nation" .
      	 "     , coalesce(sum(cus_csh_amount), 0) as financial, 0 as totalbet, 0 as totalwin, 0 as rake " .
	       "  FROM customer_cashdesk_daily" .
	       "    JOIN punter ON cus_csh_pun_id=pun_id" .
       	 "    JOIN jurisdiction c ON pun_betting_club = c.jur_id" .
       	 "    JOIN jurisdiction d ON c.jur_parent_id = d.jur_id" .
	       "    JOIN jurisdiction r ON d.jur_parent_id = r.jur_id" .
	       "    JOIN jurisdiction n ON r.jur_parent_id = n.jur_id" .
	       "    AND cus_csh_day BETWEEN '$start_date' AND '$archive_date' ";
  // Filter by jurisdiction
  if ( $jurisdiction ){
    $sql .= ' AND cus_csh_pun_id IN ' .
            '   (SELECT pun_id FROM punter WHERE pun_betting_club IN ' .
	          '     ( SELECT jur_id FROM jurisdiction' .
	          '         WHERE jur_id = ' . $jurisdiction .  //club?
	          '         OR jur_parent_id = ' . $jurisdiction  . //district?
	          '         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = '  .$jurisdiction  . ')' . //region?
	          '         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.'))' . //nation?
	          '     ) ' .
	          '   ) ';
  }
  if ( 'club' == $_SESSION['jurisdiction_class'] ) { //If a club admin user
    $sql .= ' AND cus_csh_pun_id IN ' .
	          '  ( SELECT pun_id FROM punter WHERE pun_betting_club =' . $_SESSION['jurisdiction_id'] . ' AND pun_id=cus_csh_pun_id ) ';
  }
  elseif ( 'district' == $_SESSION['jurisdiction_class'] ) { //If a district manager admin user
    $sql .= ' AND cus_csh_pun_id IN ' .
	          '     ( SELECT pun_id FROM punter WHERE pun_betting_club IN' .
	          '       (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id = ' . $_SESSION['jurisdiction_id'] . ' ) ' .
	          '     )';
  }
  elseif ( 'region' == $_SESSION['jurisdiction_class'] ) { //If a region manager admin user
    $sql .= ' AND cus_csh_pun_id IN ' .
      	    '   (SELECT pun_id FROM punter WHERE pun_betting_club IN ' .
      	    '     (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
      	    '          (  SELECT jur_id FROM jurisdiction ' .
      	    '             WHERE jur_parent_id = ' .$_SESSION['jurisdiction_id'] .
      	    '          )' .
      	    '     ) ' .
      	    '   )';
  }
  elseif ('nation' == $_SESSION['jurisdiction_class'] ) { //if a nation manager admin user
    $sql .= ' AND cus_csh_pun_id IN ' .
            '   (SELECT pun_id FROM punter WHERE pun_betting_club IN ' .
            '       (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
            '            (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
            '                 (SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$_SESSION['jurisdiction_id'].')'.
            '            )'.
            '       )'.
            '   )';
  }
  else{
    if ( 'casino' != $_SESSION['jurisdiction_class'] ){
      dieWithError("Invalid user Jurisdiction Class!");
    }
  }
   /*
  if(empty($jurisdiction)){
    $jurisdiction = $_SESSION['jurisdiction_id'];
  }
  
  $sql = "SELECT c.jur_id as clubid, c.jur_name as club, d.jur_name as district" .
         "     , r.jur_name as region, n.jur_name as nation" .
      	 "     , coalesce(sum(cus_csh_amount), 0) as financial, 0 as totalbet, 0 as totalwin, 0 as rake, 0 as tournament, 0 as casino " .
	       "  FROM customer_cashdesk_daily" .
	       "    JOIN punter         ON cus_csh_pun_id=pun_id" .
       	 "    JOIN jurisdiction c ON pun_betting_club = c.jur_id" .
       	 "    JOIN jurisdiction d ON c.jur_parent_id = d.jur_id" .
	       "    JOIN jurisdiction r ON d.jur_parent_id = r.jur_id" .
	       "    JOIN jurisdiction n ON r.jur_parent_id = n.jur_id";
	       
  $sql .= " WHERE 1=1";
  $sql .= " AND cus_csh_day BETWEEN to_date('$start_date', 'YYYY-mm-dd hh24:mi:ss') AND to_date('$archive_date', 'YYYY-mm-dd hh24:mi:ss') ";  
  
  $jurisdictions_list = $jur_list->getChildListForWhereClause($jurisdiction, "pun_betting_club", "club");
  if(!empty($jurisdictions_list)){
    $sql .= " AND ($jurisdictions_list)";
  }

  die($sql);
  */

  $sql .= " GROUP BY n.jur_name, r.jur_name, d.jur_name, c.jur_name, c.jur_id";
  $sql .= " ORDER by c.jur_name";
  
  //echo $sql;
  //die("<pre>$sql</pre>");
  $rs = $dbh->doCachedQuery($sql, $CACHE_VALUE);
  $search_today = false;
  
  if ($search_today) {
    $sql = "SELECT c.jur_id as clubid, c.jur_name as club, d.jur_name as district" .
           "     , r.jur_name as region, n.jur_name as nation" .
      	   "     , coalesce(sum(ctr_amount), 0) as financial, 0 as rake, 0 as tournament" .
      	   "  FROM customer_transaction" .
      	   "    JOIN punter on ctr_pun_id=pun_id" .
      	   "    JOIN jurisdiction c ON pun_betting_club = c.jur_id" .
      	   "    JOIN jurisdiction d ON c.jur_parent_id = d.jur_id" .
      	   "    JOIN jurisdiction r ON d.jur_parent_id = r.jur_id" .
      	   "    JOIN jurisdiction n ON r.jur_parent_id = n.jur_id" .
      	   "  WHERE ctr_pme_code='CSH'" .
      	   "    AND ctr_tty_id != ".T_TYPE_GAME_RESULT .
      	   "    AND ctr_time BETWEEN '$today_date' AND '$end_date' ";

    // Filter by jurisdiction
    if ( $jurisdiction ){
      $sql .= ' AND ctr_pun_id IN ' .
              '   (SELECT pun_id FROM punter WHERE pun_betting_club IN ' .
      	      '     ( SELECT jur_id FROM jurisdiction' .
      	      '         WHERE jur_id = ' . $jurisdiction .  //club?
      	      '         OR jur_parent_id = ' . $jurisdiction  . //district?
      	      '         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = '  .$jurisdiction  . ')' . //region?
      	      '         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.'))' . //nation?
      	      '     ) ' .
      	      '   ) ';
    }
    if ( 'club' == $_SESSION['jurisdiction_class'] ) { //If a club admin user
      //only show transactions for punters in same club as admin user
      $sql .= ' AND ctr_pun_id IN ' .
      	      '  ( SELECT pun_id FROM punter WHERE pun_betting_club =' . $_SESSION['jurisdiction_id'] . ' AND pun_id=ctr_pun_id ) ';
    }
    elseif ( 'district' == $_SESSION['jurisdiction_class'] ) { //If a district manager admin user
      // only select punters where their betting club (bottom level jurisdiction)
      // is under the admin user's district
      $sql .= ' AND ctr_pun_id IN ' .
      	      '     ( SELECT pun_id FROM punter WHERE pun_betting_club IN' .
      	      '       (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id = ' . $_SESSION['jurisdiction_id'] . ' ) ' .
      	      '     )';
    }
    elseif ( 'region' == $_SESSION['jurisdiction_class'] ) { //If a region manager admin user
      // only select punters where their betting club (bottom level jurisdiction)
      // is under the admin user's district
      // select only  records within admin user's jurisdiction
      $sql .= ' AND ctr_pun_id IN ' .
      	      '   (SELECT pun_id FROM punter WHERE pun_betting_club IN ' .
      	      '     (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
      	      '          (  SELECT jur_id FROM jurisdiction ' .
      	      '             WHERE jur_parent_id = ' .$_SESSION['jurisdiction_id'] .
      	      '          )' .
      	      '     ) ' .
      	      '   )';
    }
    elseif ('nation' == $_SESSION['jurisdiction_class'] ) { //if a nation manager admin user
      $sql .= ' AND ctr_pun_id IN ' .
              '   (SELECT pun_id FROM punter WHERE pun_betting_club IN ' .
              '       (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
              '            (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
              '                 (SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$_SESSION['jurisdiction_id'].')'.
              '            )'.
              '       )'.
              '   )';
    }
    else{
      if ( 'casino' != $_SESSION['jurisdiction_class'] ){
        dieWithError("Invalid user Jurisdiction Class!");
      }
    }

    $sql .= " GROUP BY n.jur_name, r.jur_name, d.jur_name, c.jur_name, c.jur_id";
    $sql .= " ORDER by c.jur_name";
    $rs2  = $dbh->doCachedQuery($sql, CACHE_VALUE);
    
    $num_rows2 = $rs2->getNumRows();
    //if records found
    if ( $num_rows2 > 0 ) {
      $rs2->resetRecPtr();
      while ( $rs2->hasNext() ) {  //append other columns to records
        $rec2 = $rs2->next();
        $rs->resetRecPtr();  //to start searching for same game
        //search for matching regions in original result set for previous days
        $found = false;
        while ( $rs->next() ) {
          $rec = & $rs->currentRef();
          if ( $rec['clubid'] == $rec2['clubid'] ) { //match found
            $rec['financial']  += $rec2['financial'];
            $found = true;
            break; //since found
          }
        }
        if(!$found){
          $rs->appendRecords(array($rec2));
        }
      }
    }
  }
  if($rs)$rs->resetRecPtr();
  return $rs;
}

function getFinancialByDistrict() {
	global $dbh, $jurisdiction, $jur_list;
	global $start_date, $end_date, $today_date, $yesterday_date, $search_today;

	$archive_date = (($search_today) ? $yesterday_date : $end_date);
	$sql = "SELECT d.jur_id as districtid, d.jur_name as district" .
        	"     , r.jur_name as region, n.jur_name as nation" .
        	"     , coalesce(sum(cus_csh_amount), 0) as financial, 0 as totalbet, 0 as totalwin, 0 as rake, 0 as tournament, 0 as casino" .
        	"  FROM customer_cashdesk_daily" .
        	"    JOIN punter ON cus_csh_pun_id=pun_id" .
        	"    JOIN jurisdiction c ON pun_betting_club = c.jur_id" .
        	"    JOIN jurisdiction d ON c.jur_parent_id = d.jur_id" .
        	"    JOIN jurisdiction r ON d.jur_parent_id = r.jur_id" .
        	"    JOIN jurisdiction n ON r.jur_parent_id = n.jur_id" .
        	"    AND cus_csh_day BETWEEN '$start_date', AND '$archive_date' ";
	// Filter by jurisdiction
	if ( $jurisdiction ){
		$sql .= ' AND cus_csh_pun_id IN ' .
        		'   (SELECT pun_id FROM punter WHERE pun_betting_club IN ' .
        		'     ( SELECT jur_id FROM jurisdiction' .
        		'         WHERE jur_id = ' . $jurisdiction .  //club?
        		'         OR jur_parent_id = ' . $jurisdiction . //district?
        		'         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = ' . $jurisdiction . ')' . //region?
        		'         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.'))' . //nation?
        		'     ) ' .
        		'   ) ';
	}
	if ( 'nation' == $_SESSION['jurisdiction_class'] ) {
		$sql .= " AND d.jur_id IN " .
        		"      (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN " .
        		"          (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " . $_SESSION["jurisdiction_id"] . ")" .
        		"      )";
	}
	elseif ( 'region' == $_SESSION['jurisdiction_class'] ) {
		$sql .= " AND d.jur_id IN " .
        		"      (  SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " .$_SESSION["jurisdiction_id"] . ")";
	}
	elseif ( 'district' == $_SESSION['jurisdiction_class'] ) {
		$sql .= " AND d.jur_id = " . $_SESSION['jurisdiction_id'];
	}
	else {
		if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
			$sql .= " AND 1 = 0";
		}
	}
	$sql .= " GROUP BY n.jur_name, r.jur_name, d.jur_name, d.jur_id";
	$sql .= " ORDER by d.jur_name";
	
  if(empty($jurisdiction)){
    $jurisdiction = $_SESSION['jurisdiction_id'];
  }
  
  $sql = "SELECT c.jur_id as clubid, c.jur_name as club, d.jur_name as district" .
         "     , r.jur_name as region, n.jur_name as nation" .
      	 "     , coalesce(sum(cus_csh_amount), 0) as financial, 0 as totalbet, 0 as totalwin, 0 as rake, 0 as tournament, 0 as casino " .
	       "  FROM customer_cashdesk_daily" .
	       "    JOIN punter         ON cus_csh_pun_id=pun_id" .
       	 "    JOIN jurisdiction c ON pun_betting_club = c.jur_id" .
       	 "    JOIN jurisdiction d ON c.jur_parent_id = d.jur_id" .
	       "    JOIN jurisdiction r ON d.jur_parent_id = r.jur_id" .
	       "    JOIN jurisdiction n ON r.jur_parent_id = n.jur_id";
	       
  $sql .= " WHERE 1=1";
  $sql .= " AND cus_csh_day BETWEEN '$start_date' AND '$archive_date'";  
  
  $jurisdictions_list = $jur_list->getChildListForWhereClause($jurisdiction, "pun_betting_club", "club");
  if(!empty($jurisdictions_list)){
    $sql .= " AND ($jurisdictions_list)";
  }

	$sql .= " GROUP BY n.jur_name, r.jur_name, d.jur_name, d.jur_id";
	$sql .= " ORDER by d.jur_name";

	$rs = $dbh->doCachedQuery($sql, CACHE_VALUE);

	if ($search_today) {
		$sql = "SELECT d.jur_id as districtid, d.jur_name as district" .
        		"     , r.jur_name as region, n.jur_name as nation" .
        		"     , coalesce(sum(ctr_amount), 0) as financial, 0 as rake, 0 as tournament" .
        		"  FROM customer_transaction" .
        		"    JOIN punter on ctr_pun_id=pun_id" .
        		"    JOIN jurisdiction c ON pun_betting_club = c.jur_id" .
        		"    JOIN jurisdiction d ON c.jur_parent_id = d.jur_id" .
        		"    JOIN jurisdiction r ON d.jur_parent_id = r.jur_id" .
        		"    JOIN jurisdiction n ON r.jur_parent_id = n.jur_id" .
        		"  WHERE ctr_pme_code='CSH'" .
        		"    AND ctr_tty_id != ".T_TYPE_GAME_RESULT .
        		"    AND ctr_time BETWEEN '$today_date' AND '$end_date' ";

		// Filter by jurisdiction
		if ( $jurisdiction ){
			$sql .= ' AND ctr_pun_id IN ' .
        			'   (SELECT pun_id FROM punter WHERE pun_betting_club IN ' .
        			'     ( SELECT jur_id FROM jurisdiction' .
        			'         WHERE jur_id = ' . $jurisdiction .  //club?
        			'         OR jur_parent_id = ' . $jurisdiction  . //district?
        			'         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = '  .$jurisdiction  . ')' . //region?
        			'         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.'))' . //nation?
        			'     ) ' .
        			'   ) ';
		}
		if ( 'nation' == $_SESSION['jurisdiction_class'] ) {
			$sql .= " AND d.jur_id IN " .
        			"      (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN " .
        			"          (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " . $_SESSION["jurisdiction_id"] . ")" .
        			"      )";
		}
		elseif ( 'region' == $_SESSION['jurisdiction_class'] ) {
			$sql .= " AND d.jur_id IN " .
        			"      (  SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " .$_SESSION["jurisdiction_id"] . ")";
		}
		elseif ( 'district' == $_SESSION['jurisdiction_class'] ) {
			$sql .= " AND d.jur_id = " . $_SESSION['jurisdiction_id'];
		}
		else {
			if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
				$sql .= " AND 1 = 0";
			}
		}

		$sql .= " GROUP BY n.jur_name, r.jur_name, d.jur_name, d.jur_id";
		$sql .= " ORDER by d.jur_name";
		$rs2  = $dbh->doCachedQuery($sql, CACHE_VALUE);
		$num_rows2 = $rs2->getNumRows();
		//if records found
		if ( $num_rows2 > 0 ) {
			$rs2->resetRecPtr();
			while ( $rs2->hasNext() ) {  //append other columns to records
				$rec2 = $rs2->next();
				$rs->resetRecPtr();  //to start searching for same game
				//search for matching regions in original result set for previous days
				$found = false;
				while ( $rs->next() ) {
					$rec = & $rs->currentRef();
					if ( $rec['districtid'] == $rec2['districtid'] ) { //match found
						$rec['financial']  += $rec2['financial'];
						$found = true;
						break; //since found
					}
				}
				if ( ! $found ) {
					$rs->appendRecords(array($rec2));
				}
			}
		}
	}
	if($rs)$rs->resetRecPtr();
	return $rs;
}

function getFinancialByRegion() {
	global $dbh, $jurisdiction;
	global $start_date, $end_date, $today_date, $yesterday_date, $search_today;

	$archive_date = (($search_today) ? $yesterday_date : $end_date);
	$sql = "SELECT r.jur_id as regionid, r.jur_name as region" .
        	"     , n.jur_name as nation" .
        	"     , coalesce(sum(cus_csh_amount), 0) as financial, 0 as totalbet, 0 as totalwin, 0 as rake, 0 as tournament" .
        	"  FROM customer_cashdesk_daily" .
        	"    JOIN punter ON cus_csh_pun_id=pun_id" .
        	"    JOIN jurisdiction c ON pun_betting_club = c.jur_id" .
        	"    JOIN jurisdiction d ON c.jur_parent_id = d.jur_id" .
        	"    JOIN jurisdiction r ON d.jur_parent_id = r.jur_id" .
        	"    JOIN jurisdiction n ON r.jur_parent_id = n.jur_id" .
        	"    AND cus_csh_day BETWEEN '$start_date' AND '$archive_date'";
	// Filter by jurisdiction
	if ( $jurisdiction ){
		$sql .= ' AND cus_csh_pun_id IN ' .
        		'   (SELECT pun_id FROM punter WHERE pun_betting_club IN ' .
        		'     ( SELECT jur_id FROM jurisdiction' .
        		'         WHERE jur_id = ' . $jurisdiction .  //club?
        		'         OR jur_parent_id = ' . $jurisdiction  . //district?
        		'         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = '  .$jurisdiction  . ')' . //region?
        		'         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.'))' . //nation?
        		'     ) ' .
        		'   ) ';
	}
	if ( 'nation' == $_SESSION['jurisdiction_class'] ) {
		$sql .= " AND r.jur_id IN " .
        		"      (  SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " .$_SESSION["jurisdiction_id"] . ")";
	}
	elseif ( 'region' == $_SESSION['jurisdiction_class'] ) {
		$sql .= " AND r.jur_id = " . $_SESSION['jurisdiction_id'];
	}
	else {
		if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
			$sql .= " AND 1 = 0";
		}
	}
	$sql .= " GROUP BY n.jur_name, r.jur_name, r.jur_id";
	$sql .= " ORDER BY r.jur_name";

	$rs = $dbh->doCachedQuery($sql, CACHE_VALUE);

	if ($search_today) {
		$sql = "SELECT r.jur_id as regionid, r.jur_name as region" .
        		"     , n.jur_name as nation" .
        		"     , coalesce(sum(ctr_amount), 0) as financial, 0 as rake, 0 as tournament" .
        		"  FROM customer_transaction" .
        		"    JOIN punter on ctr_pun_id=pun_id" .
        		"    JOIN jurisdiction c ON pun_betting_club = c.jur_id" .
        		"    JOIN jurisdiction d ON c.jur_parent_id = d.jur_id" .
        		"    JOIN jurisdiction r ON d.jur_parent_id = r.jur_id" .
        		"    JOIN jurisdiction n ON r.jur_parent_id = n.jur_id" .
        		"  WHERE ctr_pme_code='CSH'" .
        		"    AND ctr_tty_id != ".T_TYPE_GAME_RESULT .
        		"    AND ctr_time BETWEEN '$today_date' AND '$end_date' ";

		// Filter by jurisdiction
		if ( $jurisdiction ){
			$sql .= ' AND ctr_pun_id IN ' .
        			'   (SELECT pun_id FROM punter WHERE pun_betting_club IN ' .
        			'     ( SELECT jur_id FROM jurisdiction' .
        			'         WHERE jur_id = ' . $jurisdiction .  //club?
        			'         OR jur_parent_id = ' . $jurisdiction  . //district?
        			'         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = '  .$jurisdiction  . ')' . //region?
        			'         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.'))' . //nation?
        			'     ) ' .
        			'   ) ';
		}
		if ( 'nation' == $_SESSION['jurisdiction_class'] ) {
			$sql .= " AND r.jur_id IN " .
        			"      (  SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " .$_SESSION["jurisdiction_id"] . ")";
		}
		elseif ( 'region' == $_SESSION['jurisdiction_class'] ) {
			$sql .= " AND r.jur_id = " . $_SESSION['jurisdiction_id'];
		}
		else {
			if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
				$sql .= " AND 1 = 0";
			}
		}

		$sql .= " GROUP BY n.jur_name, r.jur_name, r.jur_id";
		$sql .= " ORDER by r.jur_name";
		$rs2  = $dbh->doCachedQuery($sql, CACHE_VALUE);
		$num_rows2 = $rs2->getNumRows();
		//if records found
		if ( $num_rows2 > 0 ) {
			$rs2->resetRecPtr();
			while ( $rs2->hasNext() ) {  //append other columns to records
				$rec2 = $rs2->next();
				$rs->resetRecPtr();  //to start searching for same game
				//search for matching regions in original result set for previous days
				$found = false;
				while ( $rs->next() ) {
					$rec = & $rs->currentRef();
					if ( $rec['regionid'] == $rec2['regionid'] ) { //match found
						$rec['financial']  += $rec2['financial'];
						$found = true;
						break; //since found
					}
				}
				if ( ! $found ) {
					$rs->appendRecords(array($rec2));
				}
			}
		}
	}
	if($rs)$rs->resetRecPtr();
	return $rs;
}

function getFinancialByNation() {
	global $dbh, $jurisdiction;
	global $start_date, $end_date, $today_date, $yesterday_date, $search_today;

	$archive_date = (($search_today) ? $yesterday_date : $end_date);
	$sql = "SELECT n.jur_id as nationid, n.jur_name as nation" .
        	"     , coalesce(sum(cus_csh_amount), 0) as financial, 0 as totalbet, 0 as totalwin, 0 as rake, 0 as tournament" .
        	"  FROM customer_cashdesk_daily" .
        	"    JOIN punter ON cus_csh_pun_id=pun_id" .
        	"    JOIN jurisdiction c ON pun_betting_club = c.jur_id" .
        	"    JOIN jurisdiction d ON c.jur_parent_id = d.jur_id" .
        	"    JOIN jurisdiction r ON d.jur_parent_id = r.jur_id" .
        	"    JOIN jurisdiction n ON r.jur_parent_id = n.jur_id" .
        	"    AND cus_csh_day BETWEEN '$start_date' AND '$archive_date'";

	// Filter by jurisdiction
	if ( $jurisdiction ){
		$sql .= ' AND cus_csh_pun_id IN ' .
        		'   (SELECT pun_id FROM punter WHERE pun_betting_club IN ' .
        		'     ( SELECT jur_id FROM jurisdiction' .
        		'         WHERE jur_id = ' . $jurisdiction .  //club?
        		'         OR jur_parent_id = ' . $jurisdiction  . //district?
        		'         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = '  .$jurisdiction  . ')' . //region?
        		'         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.'))' . //nation?
        		'     ) ' .
        		'   ) ';
	}

	if ( 'nation' == $_SESSION['jurisdiction_class'] ) {
		$sql .= " AND n.jur_id = " . $_SESSION['jurisdiction_id'];
	}
	else {
		if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
			$sql .= " AND 1 = 0";
		}
	}
	$sql .= " GROUP BY n.jur_name, n.jur_id";
	$sql .= " ORDER by n.jur_name";
  
	$rs = $dbh->doCachedQuery($sql, CACHE_VALUE);

	if ($search_today) {
		$sql = "SELECT n.jur_id as nationid, n.jur_name as nation" .
        		"     , coalesce(sum(ctr_amount), 0) as financial, 0 as rake, 0 as tournament" .
        		"  FROM customer_transaction" .
        		"    JOIN punter on ctr_pun_id=pun_id" .
        		"    JOIN jurisdiction c ON pun_betting_club = c.jur_id" .
        		"    JOIN jurisdiction d ON c.jur_parent_id = d.jur_id" .
        		"    JOIN jurisdiction r ON d.jur_parent_id = r.jur_id" .
        		"    JOIN jurisdiction n ON r.jur_parent_id = n.jur_id" .
        		"  WHERE ctr_pme_code='CSH'" .
        		"    AND ctr_tty_id != ".T_TYPE_GAME_RESULT .
        		"    AND ctr_time BETWEEN '$today_date' AND '$end_date' ";

		// Filter by jurisdiction
		if ( $jurisdiction ){
			$sql .= ' AND ctr_pun_id IN ' .
        			'   (SELECT pun_id FROM punter WHERE pun_betting_club IN ' .
        			'     ( SELECT jur_id FROM jurisdiction' .
        			'         WHERE jur_id = ' . $jurisdiction .  //club?
        			'         OR jur_parent_id = ' . $jurisdiction  . //district?
        			'         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = '  .$jurisdiction  . ')' . //region?
        			'         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.'))' . //nation?
        			'     ) ' .
        			'   ) ';
		}
		if ( 'nation' == $_SESSION['jurisdiction_class'] ) {
			$sql .= " AND r.jur_id = " . $_SESSION['jurisdiction_id'];
		}
		else {
			if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
				$sql .= " AND 1 = 0";
			}
		}
		if(is_null($rs)){ return null;}
		$sql .= " GROUP BY n.jur_name, n.jur_id";
		$sql .= " ORDER by n.jur_name";
		$rs2  = $dbh->doCachedQuery($sql, CACHE_VALUE);
		$num_rows2 = $rs2->getNumRows();
		//if records found

		if ( $num_rows2 > 0 ) {
			$rs2->resetRecPtr();
			while ( $rs2->hasNext() ) {  //append other columns to records
				$rec2 = $rs2->next();
				$rs->resetRecPtr();  //to start searching for same game
				//search for matching regions in original result set for previous days
				$found = false;
				while ( $rs->next() ) {
					$rec = & $rs->currentRef();
					if ( $rec['nationid'] == $rec2['nationid'] ) { //match found
						$rec['financial']  += $rec2['financial'];
						$found = true;
						break; //since found
					}
				}
				if ( ! $found ) {
					$rs->appendRecords(array($rec2));
				}
			}
		}
	}
	if($rs)$rs->resetRecPtr();
	return $rs;
}

function getFinancialByCustomer() {
	global $dbh, $jurisdiction;
	global $start_date, $end_date, $today_date, $yesterday_date, $search_today;

	$archive_date = (($search_today) ? $yesterday_date : $end_date);
	$sql = "SELECT pun_id as customer_id, pun_username as customer, c.jur_name as club" .
         "     , coalesce(sum(cus_csh_amount), 0) as financial, 0 as totalbet, 0 as totalwin, 0 as rake, 0 as tournament" .
         "  FROM customer_cashdesk_daily" .
         "    JOIN punter ON cus_csh_pun_id=pun_id" .
         "    JOIN jurisdiction c ON pun_betting_club = c.jur_id" .
         "    AND cus_csh_day BETWEEN '$start_date' AND '$archive_date' ";

	if ( 'club' == $_SESSION['jurisdiction_class'] ) {
		$sql .= " AND c.jur_id = " . $_SESSION['jurisdiction_id'];
	}
	else {
		if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
			$sql .= " AND 1 = 0";
		}
	}
	$sql .= " GROUP BY c.jur_name, pun_id, pun_username ";
//	$sql .= " ORDER by n.jur_name";
	$rs = $dbh->doCachedQuery($sql, CACHE_VALUE);

	if($rs)$rs->resetRecPtr();
	return $rs;
}



/***********************
 * RAKE
 ***********************/
function getRakeByClub() {
  global $dbh, $jurisdiction;
  global $start_date, $end_date, $today_date, $yesterday_date, $search_today;

  $archive_date = (($search_today) ? $yesterday_date : $end_date);

  $sql  = "SELECT c.jur_id as clubid, c.jur_name as club " .
          "     , d.jur_name as district, r.jur_name as region, n.jur_name as nation " .
          "     , 0 as financial, coalesce(sum(cus_res_bet), 0) as totalbet, coalesce(sum(cus_res_win), 0) as totalwin " .
          "     , coalesce(sum(cus_res_rake), 0) AS rake, 0 as tournament" .
          " FROM customer_result_daily " .
          "  JOIN punter on cus_res_pun_id=pun_id " .
          "  JOIN jurisdiction c on pun_betting_club=c.jur_id " .
          "  JOIN jurisdiction d on c.jur_parent_id=d.jur_id " .
          "  JOIN jurisdiction r on d.jur_parent_id=r.jur_id " .
          "  JOIN jurisdiction n on r.jur_parent_id=n.jur_id " .
          " WHERE cus_res_day BETWEEN '$start_date' AND '$archive_date'";
  if ( $jurisdiction ) {
    $sql .=  ' AND (  ( pun_betting_club =  '.$jurisdiction . ')' .
             '        OR' .
             '        (    pun_betting_club IN ' .   //if district jurisdiction selected,
             '             ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id=' . $jurisdiction . ' )' .
             '        )' .
             '        OR' .
             '        (    pun_betting_club IN ' .    //if region jurisdiction selected,
             '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction .' )' .
             '             )' .
             '        )' .
             '        OR' .
             '        (    pun_betting_club IN ' .
             '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                     (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.' ) ' .
             '                 )' .
             '             )' .
             '        )' .
             '     )';
  }
  if ( 'nation' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND c.jur_id IN " .
            "      (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN " .
            "          (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN " .
            "              (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " . $_SESSION["jurisdiction_id"] . ") " .
            "          )" .
            "      )";
  }
  elseif ( 'region' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND c.jur_id IN " .
            "      (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN " .
            "          (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id=" . $_SESSION["jurisdiction_id"] . ") " .
            "      )";
  }
  elseif ( 'district' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND c.jur_id IN " .
            "      (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " . $_SESSION['jurisdiction_id'] . " )";
  }
  elseif ( 'club' == $_SESSION['jurisdiction_class']) {
    $sql .= " AND c.jur_id = " . $_SESSION["jurisdiction_id"];
  }
  else {
    if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
      $sql .= " AND 1 = 0";
    }
  }
  $sql .= ' GROUP BY n.jur_name, r.jur_name, d.jur_name, c.jur_name, c.jur_id ORDER BY c.jur_name';
  //echo $sql;
  $rs = $dbh->doCachedQuery($sql, CACHE_VALUE);


  if ($search_today) {
    $sql  = "SELECT c.jur_id as clubid, c.jur_name as club ".
            "     , d.jur_name as district, r.jur_name as region, n.jur_name as nation " .
            "     , 0 as financial, coalesce(sum(pre_rake), 0) AS rake, 0 as tournament" .
            " FROM result " .
            "  JOIN punter_result ON punter_result.pre_res_id = result.res_id " .
            "  JOIN punter ON punter_result.pre_pun_id = punter.pun_id " .
            "  JOIN jurisdiction c ON pun_betting_club = c.jur_id " .
            "  JOIN jurisdiction d ON c.jur_parent_id  = d.jur_id " .
            "  JOIN jurisdiction r ON d.jur_parent_id  = r.jur_id " .
            "  JOIN jurisdiction n ON r.jur_parent_id  = n.jur_id " .
            " WHERE res_date BETWEEN '$today_date' AND '$end_date' ";

    if ( $jurisdiction ) {
       $sql .=  ' AND (  ( pun_betting_club =  '.$jurisdiction . ')' .
                '        OR' .
                '        (    pun_betting_club IN ' .   //if district jurisdiction selected,
                '             ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id=' . $jurisdiction . ' )' .
                '        )' .
                '        OR' .
                '        (    pun_betting_club IN ' .    //if region jurisdiction selected,
                '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
                '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction .' )' .
                '             )' .
                '        )' .
                '        OR' .
                '        (    pun_betting_club IN ' .
                '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
                '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
                '                     (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.' ) ' .
                '                 )' .
                '             )' .
                '        )' .
                '     )';
    }
    if ( 'nation' == $_SESSION['jurisdiction_class'] ) {
      $sql .= " AND c.jur_id IN " .
              "      (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN " .
              "          (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN " .
              "              (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " . $_SESSION["jurisdiction_id"] . ") " .
              "          )" .
              "      )";
    }
    elseif ( 'region' == $_SESSION['jurisdiction_class'] ) {
      $sql .= " AND c.jur_id IN " .
              "      (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN " .
              "          (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id=" . $_SESSION["jurisdiction_id"] . ") " .
              "      )";
    }
    elseif ( 'district' == $_SESSION['jurisdiction_class'] ) {
      $sql .= " AND c.jur_id IN " .
              "      (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " . $_SESSION['jurisdiction_id'] . " )";
    }
    else {
      if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
        $sql .= " AND 1 = 0";
      }
    }
    $sql .= ' GROUP BY n.jur_name, r.jur_name, d.jur_name, c.jur_name, c.jur_id ORDER BY c.jur_name';
    $rs2       = $dbh->doCachedQuery($sql, CACHE_VALUE);
    $num_rows2 = $rs2->getNumRows();

    //if records found
    if ( $num_rows2 > 0 ) {
      $rs2->resetRecPtr();
      while ( $rs2->hasNext() ) {  //append other columns to records
        $rec2 = $rs2->next();
        $rs->resetRecPtr();  //to start searching for same game
        //search for matching regions in original result set for previous days
        $found = false;
        while ( $rs->next() ) {
          $rec = & $rs->currentRef();
          if ( $rec['clubid'] == $rec2['clubid'] ) { //match found
            ///////add fields to original recordset
            $rec['rake']        += $rec2['rake'];
            $found = true;
            break; //since found
          }
        }
        if ( ! $found ) {
          $rs->appendRecords(array($rec2));
        }
      }
    }
  }
  if($rs)$rs->resetRecPtr();
  return $rs;
}

function getRakeByDistrict() {
  global $dbh, $jurisdiction;
  global $start_date, $end_date, $today_date, $yesterday_date, $search_today;

  $archive_date = (($search_today) ? $yesterday_date : $end_date);

  $sql  = "SELECT d.jur_id as districtid " .
          "    , d.jur_name AS district " .
          "    , r.jur_name AS region   " .
          "    , n.jur_name AS nation   " .
          "    , 0 as financial, coalesce(sum(cus_res_bet), 0) as totalbet, coalesce(sum(cus_res_win), 0) as totalwin " .
          "    , coalesce(sum(cus_res_rake), 0) AS rake, 0 as tournament, 0 as casino" .
          " FROM customer_result_daily" .
          "  JOIN punter ON cus_res_pun_id = punter.pun_id " .
          "  JOIN jurisdiction c ON punter.pun_betting_club=c.jur_id " .
          "  JOIN jurisdiction d ON c.jur_parent_id=d.jur_id " .
          "  JOIN jurisdiction r ON d.jur_parent_id=r.jur_id " .
          "  JOIN jurisdiction n ON r.jur_parent_id=n.jur_id " .
          " WHERE cus_res_day BETWEEN '$start_date' AND '$archive_date'";
  if ( $jurisdiction ) {
    $sql .=  ' AND (  ( pun_betting_club =  '.$jurisdiction . ')' .
             '        OR' .
             '        (    pun_betting_club IN ' .   //if district jurisdiction selected,
             '             ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id=' . $jurisdiction . ' )' .
             '        )' .
             '        OR' .
             '        (    pun_betting_club IN ' .    //if region jurisdiction selected,
             '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction .' )' .
             '             )' .
             '        )' .
             '        OR' .
             '        (    pun_betting_club IN ' .
             '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                     (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.' ) ' .
             '                 )' .
             '             )' .
             '        )' .
             '     )';
  }

  if ( 'nation' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND d.jur_id IN " .
            "      (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN " .
            "          (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " . $_SESSION["jurisdiction_id"] . ")" .
            "      )";
  }
  elseif ( 'region' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND d.jur_id IN " .
            "      (  SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " .$_SESSION["jurisdiction_id"] . ")";
  }
  elseif ( 'district' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND d.jur_id = " . $_SESSION['jurisdiction_id'];
  }
  else {
    if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
      $sql .= " AND 1 = 0";
    }
  }
  $sql .= ' GROUP BY n.jur_name, r.jur_name, d.jur_name, d.jur_id';
  $rs = $dbh->doCachedQuery($sql, CACHE_VALUE);

  if ($search_today) {
    $sql  = "SELECT d.jur_id as districtid ".
            "     , d.jur_name as district, r.jur_name as region, n.jur_name as nation " .
            "     , 0 as financial, coalesce(sum(pre_rake), 0) AS rake, 0 as tournament" .
            " FROM result " .
            "  JOIN punter_result ON punter_result.pre_res_id = result.res_id " .
            "  JOIN punter ON punter_result.pre_pun_id = punter.pun_id " .
            "  JOIN jurisdiction c ON pun_betting_club = c.jur_id " .
            "  JOIN jurisdiction d ON c.jur_parent_id  = d.jur_id " .
            "  JOIN jurisdiction r ON d.jur_parent_id  = r.jur_id " .
            "  JOIN jurisdiction n ON r.jur_parent_id  = n.jur_id " .
            " WHERE res_date BETWEEN '$today_date' AND '$end_date' ";
    if ( $jurisdiction ) {
      $sql .=  ' AND (  ( pun_betting_club =  '.$jurisdiction . ')' .
               '        OR' .
               '        (    pun_betting_club IN ' .   //if district jurisdiction selected,
               '             ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id=' . $jurisdiction . ' )' .
               '        )' .
               '        OR' .
               '        (    pun_betting_club IN ' .    //if region jurisdiction selected,
               '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
               '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction .' )' .
               '             )' .
               '        )' .
               '        OR' .
               '        (    pun_betting_club IN ' .
               '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
               '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
               '                     (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.' ) ' .
               '                 )' .
               '             )' .
               '        )' .
               '     )';
    }

    if ( 'nation' == $_SESSION['jurisdiction_class'] ) {
      $sql .= " AND d.jur_id IN " .
              "      (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN " .
              "          (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " . $_SESSION["jurisdiction_id"] . ")" .
              "      )";
    }
    elseif ( 'region' == $_SESSION['jurisdiction_class'] ) {
      $sql .= " AND d.jur_parent_id = " . $_SESSION["jurisdiction_id"];
    }
    elseif ( 'district' == $_SESSION['jurisdiction_class'] ) {
      $sql .= " AND d.jur_id = " . $_SESSION['jurisdiction_id'];
    }
    else {
      if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
        $sql .= " AND 1 = 0";
      }
    }

    $sql      .= ' GROUP BY n.jur_name, r.jur_name, d.jur_name, d.jur_id ORDER BY d.jur_name';
    $rs2       = $dbh->doCachedQuery($sql, CACHE_VALUE);
    $num_rows2 = $rs2->getNumRows();

    //if records found
    if ( $num_rows2 > 0 ) {
      $rs2->resetRecPtr();
      while ( $rs2->hasNext() ) {  //append other columns to records
        $rec2 = $rs2->next();
        $rs->resetRecPtr();  //to start searching for same game
        //search for matching regions in original result set for previous days
        $found = false;
        while ( $rs->next() ) {
          $rec = & $rs->currentRef();
          if ( $rec['districtid'] == $rec2['districtid'] ) { //match found
            ///////add fields to original recordset
            $rec['rake']        += $rec2['rake'];
            $found = true;
            break; //since found
          }
        }
        if ( ! $found ) {
          $rs->appendRecords(array($rec2));
        }
      }
    }
  }
   if($rs)$rs->resetRecPtr();
  return $rs;
}

function getRakeByRegion() {
  global $dbh, $jurisdiction;
  global $start_date, $end_date, $today_date, $yesterday_date, $search_today;

  $archive_date = (($search_today) ? $yesterday_date : $end_date);

  $sql  = "SELECT r.jur_id as regionid " .
          "    , r.jur_name AS region  " .
          "    , n.jur_name AS nation  " .
          "    , 0 as financial, coalesce(sum(cus_res_bet), 0) as totalbet, coalesce(sum(cus_res_win), 0) as totalwin " .
          "    , coalesce(sum(cus_res_rake), 0) AS rake, 0 as tournament" .
          " FROM customer_result_daily" .
          "  JOIN punter ON cus_res_pun_id = punter.pun_id " .
          "  JOIN jurisdiction c ON punter.pun_betting_club=c.jur_id " .
          "  JOIN jurisdiction d ON c.jur_parent_id=d.jur_id " .
          "  JOIN jurisdiction r ON d.jur_parent_id=r.jur_id " .
          "  JOIN jurisdiction n ON r.jur_parent_id=n.jur_id " .
          " WHERE cus_res_day BETWEEN '$start_date' AND '$archive_date' ";
  if ( $jurisdiction ) {
    $sql .=  ' AND (  ( pun_betting_club =  '.$jurisdiction . ')' .
             '        OR' .
             '        (    pun_betting_club IN ' .   //if district jurisdiction selected,
             '             ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id=' . $jurisdiction . ' )' .
             '        )' .
             '        OR' .
             '        (    pun_betting_club IN ' .    //if region jurisdiction selected,
             '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction .' )' .
             '             )' .
             '        )' .
             '        OR' .
             '        (    pun_betting_club IN ' .
             '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                     (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.' ) ' .
             '                 )' .
             '             )' .
             '        )' .
             '     )';
  }

  if ( 'nation' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND r.jur_id IN " .
            "      (  SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " .$_SESSION["jurisdiction_id"] . ")";
  }
  elseif ( 'region' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND r.jur_id = " . $_SESSION['jurisdiction_id'];
  }
  else {
    if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
      $sql .= " AND 1 = 0";
    }
  }
  $sql .= ' GROUP BY n.jur_name, r.jur_name, r.jur_id ORDER BY r.jur_name';

  $rs = $dbh->doCachedQuery($sql, CACHE_VALUE);


  if ($search_today) {
    $sql  = "SELECT r.jur_id as regionid ".
            "     , r.jur_name as region, n.jur_name as nation " .
            "     , 0 as financial, coalesce(sum(pre_rake), 0) AS rake, 0 as tournament" .
            " FROM result " .
            "  JOIN punter_result ON punter_result.pre_res_id = result.res_id " .
            "  JOIN punter ON punter_result.pre_pun_id = punter.pun_id " .
            "  JOIN jurisdiction c ON pun_betting_club = c.jur_id " .
            "  JOIN jurisdiction d ON c.jur_parent_id  = d.jur_id " .
            "  JOIN jurisdiction r ON d.jur_parent_id  = r.jur_id " .
            "  JOIN jurisdiction n ON r.jur_parent_id  = n.jur_id " .
            " WHERE res_date BETWEEN '$today_date' AND '$end_date' ";
    if ( $jurisdiction ) {
      $sql .=  ' AND (  ( pun_betting_club =  '.$jurisdiction . ')' .
               '        OR' .
               '        (    pun_betting_club IN ' .   //if district jurisdiction selected,
               '             ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id=' . $jurisdiction . ' )' .
               '        )' .
               '        OR' .
               '        (    pun_betting_club IN ' .    //if region jurisdiction selected,
               '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
               '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction .' )' .
               '             )' .
               '        )' .
               '        OR' .
               '        (    pun_betting_club IN ' .
               '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
               '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
               '                     (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.' ) ' .
               '                 )' .
               '             )' .
               '        )' .
               '     )';
    }

    if ( 'nation' == $_SESSION['jurisdiction_class'] ) {
      $sql .= " AND r.jur_parent_id = " . $_SESSION["jurisdiction_id"];
    }
    elseif ( 'region' == $_SESSION['jurisdiction_class'] ) {
      $sql .= " AND r.jur_id = " . $_SESSION['jurisdiction_id'];
    }
    else {
      if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
        $sql .= " AND 1 = 0";
      }
    }

    $sql      .= ' GROUP BY n.jur_name, r.jur_name, r.jur_id';
    $rs2       = $dbh->doCachedQuery($sql, CACHE_VALUE);
    $num_rows2 = $rs2->getNumRows();

    //if records found
    if ( $num_rows2 > 0 ) {
      $rs2->resetRecPtr();
      while ( $rs2->hasNext() ) {  //append other columns to records
        $rec2 = $rs2->next();
        $rs->resetRecPtr();  //to start searching for same game
        //search for matching regions in original result set for previous days
        $found = false;
        while ( $rs->next() ) {
          $rec = & $rs->currentRef();
          if ( $rec['regionid'] == $rec2['regionid'] ) { //match found
            ///////add fields to original recordset
            $rec['rake']        += $rec2['rake'];
            $found = true;
            break; //since found
          }
        }
        if ( ! $found ) {
          $rs->appendRecords(array($rec2));
        }
      }
    }
  }
   if($rs)$rs->resetRecPtr();
  return $rs;
}

function getRakeByNation() {
  global $dbh, $jurisdiction;
  global $start_date, $end_date, $today_date, $yesterday_date, $search_today;

  $archive_date = (($search_today) ? $yesterday_date : $end_date);

  $sql  = "SELECT n.jur_id as nationid" .
          "    , n.jur_name AS nation  " .
          "    , 0 as financial, coalesce(sum(cus_res_bet), 0) as totalbet, coalesce(sum(cus_res_win), 0) as totalwin " .
          "    , coalesce(sum(cus_res_rake), 0) AS rake, 0 as tournament" .
          " FROM customer_result_daily" .
          "  JOIN punter ON cus_res_pun_id = punter.pun_id " .
          "  JOIN jurisdiction c ON punter.pun_betting_club=c.jur_id " .
          "  JOIN jurisdiction d ON c.jur_parent_id=d.jur_id " .
          "  JOIN jurisdiction r ON d.jur_parent_id=r.jur_id " .
          "  JOIN jurisdiction n ON r.jur_parent_id=n.jur_id " .
          " WHERE cus_res_day BETWEEN '$start_date' AND '$archive_date' ";
  if ( $jurisdiction ) {
    $sql .=  ' AND (  ( pun_betting_club =  '.$jurisdiction . ')' .
             '        OR' .
             '        (    pun_betting_club IN ' .   //if district jurisdiction selected,
             '             ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id=' . $jurisdiction . ' )' .
             '        )' .
             '        OR' .
             '        (    pun_betting_club IN ' .    //if region jurisdiction selected,
             '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction .' )' .
             '             )' .
             '        )' .
             '        OR' .
             '        (    pun_betting_club IN ' .
             '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                     (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.' ) ' .
             '                 )' .
             '             )' .
             '        )' .
             '     )';
  }

  if ( 'nation' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND n.jur_id = " . $_SESSION['jurisdiction_id'];
  }
  else {
    if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
      $sql .= " AND 1 = 0";
    }
  }
  $sql .= ' GROUP BY n.jur_name, n.jur_id ORDER BY n.jur_name';

  $rs = $dbh->doCachedQuery($sql, CACHE_VALUE);


  if ($search_today) {
    $sql  = "SELECT n.jur_id as nationid ".
            "     , n.jur_name as nation " .
            "     , 0 as financial, coalesce(sum(pre_rake), 0) AS rake, 0 as tournament" .
            " FROM result " .
            "  JOIN punter_result ON punter_result.pre_res_id = result.res_id " .
            "  JOIN punter ON punter_result.pre_pun_id = punter.pun_id " .
            "  JOIN jurisdiction c ON pun_betting_club = c.jur_id " .
            "  JOIN jurisdiction d ON c.jur_parent_id  = d.jur_id " .
            "  JOIN jurisdiction r ON d.jur_parent_id  = r.jur_id " .
            "  JOIN jurisdiction n ON r.jur_parent_id  = n.jur_id " .
            " WHERE res_date BETWEEN '$today_date' AND '$end_date' ";
    if ( $jurisdiction ) {
      $sql .=  ' AND (  ( pun_betting_club =  '.$jurisdiction . ')' .
               '        OR' .
               '        (    pun_betting_club IN ' .   //if district jurisdiction selected,
               '             ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id=' . $jurisdiction . ' )' .
               '        )' .
               '        OR' .
               '        (    pun_betting_club IN ' .    //if region jurisdiction selected,
               '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
               '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction .' )' .
               '             )' .
               '        )' .
               '        OR' .
               '        (    pun_betting_club IN ' .
               '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
               '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
               '                     (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.' ) ' .
               '                 )' .
               '             )' .
               '        )' .
               '     )';
    }

    if ( 'nation' == $_SESSION['jurisdiction_class'] ) {
      $sql .= " AND n.jur_id = " . $_SESSION['jurisdiction_id'];
    }
    else {
      if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
        $sql .= " AND 1 = 0";
      }
    }

    $sql      .= ' GROUP BY n.jur_name, n.jur_id';
    $rs2       = $dbh->doCachedQuery($sql, CACHE_VALUE);
    $num_rows2 = $rs2->getNumRows();

    //if records found
    if ( $num_rows2 > 0 ) {
      $rs2->resetRecPtr();
      while ( $rs2->hasNext() ) {  //append other columns to records
        $rec2 = $rs2->next();
        $rs->resetRecPtr();  //to start searching for same game
        //search for matching regions in original result set for previous days
        $found = false;
        while ( $rs->next() ) {
          $rec = & $rs->currentRef();
          if ( $rec['nationid'] == $rec2['nationid'] ) { //match found
            ///////add fields to original recordset
            $rec['rake']        += $rec2['rake'];
            $found = true;
            break; //since found
          }
        }
        if ( ! $found ) {
          $rs->appendRecords(array($rec2));
        }
      }
    }
  }
   if($rs)$rs->resetRecPtr();
  return $rs;
}

function getRakeByCustomer() {
  global $dbh, $jurisdiction;
  global $start_date, $end_date, $today_date, $yesterday_date, $search_today;

  $archive_date = (($search_today) ? $yesterday_date : $end_date);

  $sql  = "SELECT pun_id as customer_id, pun_username as customer, 0 as financial, coalesce(sum(cus_res_rake), 0) AS rake, " .
          "  coalesce(sum(cus_res_bet), 0) as totalbet, coalesce(sum(cus_res_win), 0) as totalwin, 0 as tournament, c.jur_name as club " .
          " FROM customer_result_daily" .
          "  JOIN punter ON cus_res_pun_id = punter.pun_id " .
          "  JOIN jurisdiction c ON punter.pun_betting_club=c.jur_id " .
          " WHERE c.jur_id = " . $_SESSION["jurisdiction_id"] .
          "  AND cus_res_day BETWEEN '$start_date' AND '$archive_date' ";

  //TODO Rendere possibile ricerca anche per livelli superiori
  if ("club" == $_SESSION["jurisdiction_class"]) {
    $sql .= " AND c.jur_id = " . $_SESSION["jurisdiction_id"];
  }
  else {
    $sql .= ' AND 1 = 0 ';
  }

  $sql .= ' GROUP BY c.jur_name, pun_id, pun_username';
//  $sql .= ' ORDER BY pun_username';

  $rs = $dbh->doCachedQuery($sql, CACHE_VALUE);
  if ($rs) $rs->resetRecPtr();
  return $rs;
}



/******************
 * TOURNAMENT
 ******************/
function getTournamentFeeByClub() {
  global $dbh, $jurisdiction, $max_date;
  global $start_date, $end_date;


  $sql = "SELECT jur_id as clubid, jur_name as club, 0 as financial, 0 as rake, coalesce(sum(tnt_fee), 0) AS tournament " .
        "   FROM tournament_entries " .
        "   JOIN tournament ON tournament.trn_id=tournament_entries.tnt_trnid " .
        "   JOIN punter     ON tournament_entries.tnt_punid=punter.pun_id " .
        "   JOIN jurisdiction c ON punter.pun_betting_club=c.jur_id " .
        "  WHERE tournament.trn_status=5 " .
        "   AND tournament.trn_start BETWEEN '$start_date' AND '$end_date' ";
  if ( $jurisdiction ) {
    $sql .=  ' AND (  ( pun_betting_club =  '.$jurisdiction . ')' .
             '        OR' .
             '        (    pun_betting_club IN ' .   //if district jurisdiction selected,
             '             ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id=' . $jurisdiction . ' )' .
             '        )' .
             '        OR' .
             '        (    pun_betting_club IN ' .    //if region jurisdiction selected,
             '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction .' )' .
             '             )' .
             '        )' .
             '        OR' .
             '        (    pun_betting_club IN ' .
             '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                     (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.' ) ' .
             '                 )' .
             '             )' .
             '        )' .
             '     )';
  }
  if ( 'nation' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND c.jur_id IN " .
            "      (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN " .
            "          (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN " .
            "              (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " . $_SESSION["jurisdiction_id"] . ") " .
            "         )" .
            "      )";
  }
  elseif ( 'region' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND c.jur_id IN " .
            "      (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN " .
            "          (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id=" . $_SESSION["jurisdiction_id"] . ") " .
            "      )";
  }
  elseif ( 'district' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND c.jur_id IN " .
            "      (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " . $_SESSION['jurisdiction_id'] . " )";
  }
  elseif ( 'club' == $_SESSION['jurisdiction_class']) {
    $sql .= " AND c.jur_id = " . $_SESSION["jurisdiction_id"];
  }
  else {
    if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
      $sql .= " AND 1 = 0";
    }
  }
  $sql .= ' GROUP BY c.jur_id, jur_name';

  return $dbh->doCachedQuery($sql, CACHE_VALUE);
}

function getTournamentFeeByDistrict() {
  global $dbh, $jurisdiction, $max_date;
  global $start_date, $end_date;


  $sql = "SELECT d.jur_id as districtid, d.jur_name as district, 0 as financial, 0 as rake, 0 as casino, coalesce(sum(tnt_fee), 0) AS tournament " .
        "   FROM tournament_entries " .
        "   JOIN tournament ON tournament.trn_id=tournament_entries.tnt_trnid " .
        "   JOIN punter     ON tournament_entries.tnt_punid=punter.pun_id " .
        "   JOIN jurisdiction c ON punter.pun_betting_club=c.jur_id " .
        "   JOIN jurisdiction d ON c.jur_parent_id = d.jur_id " .
        "  WHERE tournament.trn_status=5 " .
        "   AND tournament.trn_start BETWEEN '$start_date' AND '$end_date' ";
  if ( $jurisdiction ) {
    $sql .=  ' AND (  ( pun_betting_club =  '.$jurisdiction . ')' .
             '        OR' .
             '        (    pun_betting_club IN ' .   //if district jurisdiction selected,
             '             ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id=' . $jurisdiction . ' )' .
             '        )' .
             '        OR' .
             '        (    pun_betting_club IN ' .    //if region jurisdiction selected,
             '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction .' )' .
             '             )' .
             '        )' .
             '        OR' .
             '        (    pun_betting_club IN ' .
             '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                     (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.' ) ' .
             '                 )' .
             '             )' .
             '        )' .
             '     )';
  }

  if ( 'nation' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND d.jur_id IN " .
            "      (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN " .
            "          (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " . $_SESSION["jurisdiction_id"] . ")" .
            "      )";
  }
  elseif ( 'region' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND d.jur_parent_id = " . $_SESSION["jurisdiction_id"];
  }
  elseif ( 'district' == $_SESSION['jurisdiction_class'] ) {
   $sql .= " AND d.jur_id = " . $_SESSION['jurisdiction_id'];
  }
  else {
   if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
     $sql .= " AND 1 = 0";
   }
  }

  $sql .= ' GROUP BY d.jur_id, d.jur_name';
  
  return $dbh->doCachedQuery($sql, CACHE_VALUE);
}

function getTournamentFeeByRegion() {
  global $dbh, $jurisdiction, $max_date;
  global $start_date, $end_date;


  $sql = "SELECT r.jur_id as regionid, r.jur_name as region, 0 as financial, 0 as rake, coalesce(sum(tnt_fee), 0) AS tournament " .
        "   FROM tournament_entries " .
        "   JOIN tournament ON tournament.trn_id=tournament_entries.tnt_trnid " .
        "   JOIN punter     ON tournament_entries.tnt_punid=punter.pun_id " .
        "   JOIN jurisdiction c ON punter.pun_betting_club=c.jur_id " .
        "   JOIN jurisdiction d ON c.jur_parent_id = d.jur_id " .
        "   JOIN jurisdiction r ON d.jur_parent_id = r.jur_id " .
        "  WHERE tournament.trn_status=5 " .
        "   AND tournament.trn_start BETWEEN '$start_date' AND '$end_date' ";
  if ( $jurisdiction ) {
    $sql .=  ' AND (  ( pun_betting_club =  '.$jurisdiction . ')' .
             '        OR' .
             '        (    pun_betting_club IN ' .   //if district jurisdiction selected,
             '             ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id=' . $jurisdiction . ' )' .
             '        )' .
             '        OR' .
             '        (    pun_betting_club IN ' .    //if region jurisdiction selected,
             '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction .' )' .
             '             )' .
             '        )' .
             '        OR' .
             '        (    pun_betting_club IN ' .
             '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                     (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.' ) ' .
             '                 )' .
             '             )' .
             '        )' .
             '     )';
  }

  if ( 'nation' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND r.jur_parent_id = " . $_SESSION["jurisdiction_id"];
  }
  elseif ( 'region' == $_SESSION['jurisdiction_class'] ) {
   $sql .= " AND r.jur_id = " . $_SESSION['jurisdiction_id'];
  }
  else {
   if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
     $sql .= " AND 1 = 0";
   }
  }

  $sql .= ' GROUP BY r.jur_id, r.jur_name';
  return $dbh->doCachedQuery($sql, CACHE_VALUE);
}

function getTournamentFeeByNation() {
  global $dbh, $jurisdiction, $max_date;
  global $start_date, $end_date;


  $sql = "SELECT n.jur_id as nationid, 0 as financial, 0 as rake, coalesce(sum(tnt_fee), 0) AS tournament " .
        "   FROM tournament_entries " .
        "   JOIN tournament ON tournament.trn_id=tournament_entries.tnt_trnid " .
        "   JOIN punter     ON tournament_entries.tnt_punid=punter.pun_id " .
        "   JOIN jurisdiction c ON punter.pun_betting_club=c.jur_id " .
        "   JOIN jurisdiction d ON c.jur_parent_id = d.jur_id " .
        "   JOIN jurisdiction r ON d.jur_parent_id = r.jur_id " .
        "   JOIN jurisdiction n ON r.jur_parent_id = n.jur_id " .
        "  WHERE tournament.trn_status=5 " .
        "   AND tournament.trn_start BETWEEN '$start_date' AND '$end_date' ";

  if ( $jurisdiction ) {
    $sql .=  ' AND (  ( pun_betting_club =  '.$jurisdiction . ')' .
             '        OR' .
             '        (    pun_betting_club IN ' .   //if district jurisdiction selected,
             '             ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id=' . $jurisdiction . ' )' .
             '        )' .
             '        OR' .
             '        (    pun_betting_club IN ' .    //if region jurisdiction selected,
             '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction .' )' .
             '             )' .
             '        )' .
             '        OR' .
             '        (    pun_betting_club IN ' .
             '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                     (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.' ) ' .
             '                 )' .
             '             )' .
             '        )' .
             '     )';
  }

  if ( 'nation' == $_SESSION['jurisdiction_class'] ) {
   $sql .= " AND n.jur_id = " . $_SESSION['jurisdiction_id'];
  }
  else {
   if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
     $sql .= " AND 1 = 0";
   }
  }

  $sql .= ' GROUP BY n.jur_id';
  return $dbh->doCachedQuery($sql, CACHE_VALUE);
}

function getTournamentFeeByCustomer() {
  global $dbh, $jurisdiction, $max_date;
  global $start_date, $end_date;


  $sql = "SELECT pun_id as customer_id, pun_username as customer,  c.jur_name as club, 0 as financial, 0 as rake" .
         " , coalesce(sum(tnt_fee), 0) AS tournament " .
         "   FROM tournament_entries " .
         "   JOIN tournament ON tournament.trn_id=tournament_entries.tnt_trnid " .
         "   JOIN punter     ON tournament_entries.tnt_punid=punter.pun_id " .
         "   JOIN jurisdiction c ON punter.pun_betting_club=c.jur_id " .
         "  WHERE tournament.trn_status=5 " .
         "   AND tournament.trn_start BETWEEN '$start_date' AND '$end_date'";
  if ( 'club' == $_SESSION["jurisdiction_class"]) {
    $sql .= " AND c.jur_id=".$_SESSION["jurisdiction_id"];
  }
  else {
    $sql .= " AND 1=0";
  }

  $sql .= ' GROUP BY c.jur_name, pun_id, pun_username';
  return $dbh->doCachedQuery($sql, CACHE_VALUE);
}




/******************
* CASINO
******************/
function getCasinoByClub($partnerid) {
  global $dbh, $jurisdiction;
  global $start_date, $end_date, $today_date, $yesterday_date, $search_today;

  $archive_date = (($search_today) ? $yesterday_date : $end_date);
  $sql = "SELECT c.jur_id as clubid, c.jur_name as club, d.jur_name as district" .
         "     , r.jur_name as region, n.jur_name as nation" .
         "     , coalesce(sum(cus_cas_amount), 0) as casino " .
         "  FROM customer_casino_daily" .
         "    JOIN punter ON cus_cas_pun_id=pun_id" .
         "    JOIN jurisdiction c ON pun_betting_club = c.jur_id" .
         "    JOIN jurisdiction d ON c.jur_parent_id = d.jur_id" .
         "    JOIN jurisdiction r ON d.jur_parent_id = r.jur_id" .
         "    JOIN jurisdiction n ON r.jur_parent_id = n.jur_id" .
         "  WHERE cus_cas_day BETWEEN '$start_date' AND '$archive_date' ";
  $sql .= " AND cus_cas_ptn_id = $partnerid ";

  // Filter by jurisdiction
  if ( $jurisdiction ){
    $sql .= ' AND cus_cas_pun_id IN ' .
            '   (SELECT pun_id FROM punter WHERE pun_betting_club IN ' .
            '     ( SELECT jur_id FROM jurisdiction' .
            '         WHERE jur_id = ' . $jurisdiction .  //club?
            '         OR jur_parent_id = ' . $jurisdiction  . //district?
            '         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = '  .$jurisdiction  . ')' . //region?
            '         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.'))' . //nation?
            '     ) ' .
            '   ) ';
  }

  if ( 'club' == $_SESSION['jurisdiction_class'] ) { //If a club admin user
    $sql .= ' AND cus_cas_pun_id IN ' .
            '  ( SELECT pun_id FROM punter WHERE pun_betting_club =' . $_SESSION['jurisdiction_id'] . ' AND pun_id=cus_cas_pun_id ) ';
  }
  elseif ( 'district' == $_SESSION['jurisdiction_class'] ) { //If a district manager admin user
    $sql .= ' AND cus_cas_pun_id IN ' .
            '     ( SELECT pun_id FROM punter WHERE pun_betting_club IN' .
            '       (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id = ' . $_SESSION['jurisdiction_id'] . ' ) ' .
            '     )';
  }
  elseif ( 'region' == $_SESSION['jurisdiction_class'] ) { //If a region manager admin user
    $sql .= ' AND cus_cas_pun_id IN ' .
            '   (SELECT pun_id FROM punter WHERE pun_betting_club IN ' .
            '     (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
            '          (  SELECT jur_id FROM jurisdiction ' .
            '             WHERE jur_parent_id = ' .$_SESSION['jurisdiction_id'] .
            '          )' .
            '     ) ' .
            '   )';
  }
  elseif ('nation' == $_SESSION['jurisdiction_class'] ) { //if a nation manager admin user
    $sql .= ' AND cus_cas_pun_id IN ' .
            '   (SELECT pun_id FROM punter WHERE pun_betting_club IN ' .
            '       (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
            '            (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
            '                 (SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$_SESSION['jurisdiction_id'].')'.
            '            )'.
            '       )'.
            '   )';
  }
  else{
    if ( 'casino' != $_SESSION['jurisdiction_class'] ){
      dieWithError("Invalid user Jurisdiction Class!");
    }
  }

  $sql .= " GROUP BY n.jur_name, r.jur_name, d.jur_name, c.jur_name, c.jur_id";
  $sql .= " ORDER by c.jur_name";

  $rs = $dbh->doCachedQuery($sql, CACHE_VALUE);
  //echo $sql;

  if ($search_today) {
    $sql = "SELECT c.jur_id as clubid, c.jur_name as club, d.jur_name as district" .
           "     , r.jur_name as region, n.jur_name as nation" .
    	     "     , coalesce(sum(ctr_amount), 0) as casino, 0 as rake, 0 as tournament" .
           "  FROM customer_transaction" .
           "    JOIN punter on ctr_pun_id=pun_id" .
           "    JOIN jurisdiction c ON pun_betting_club = c.jur_id" .
           "    JOIN jurisdiction d ON c.jur_parent_id = d.jur_id" .
           "    JOIN jurisdiction r ON d.jur_parent_id = r.jur_id" .
           "    JOIN jurisdiction n ON r.jur_parent_id = n.jur_id" .
           "  WHERE ctr_tty_id = 11 " .
           "    AND ctr_time BETWEEN '$today_date' AND '$end_date' ";
    //TODO Da decommentare quando si modifica customer_transaction
    //  Per ora le prende tutte. Dato che Royal PK E' l'unica sara' sempre 4
    //$sql .= " AND  ctr_ptn_id = $partnerid ";

    // Filter by jurisdiction
    if ( $jurisdiction ){
      $sql .= ' AND ctr_pun_id IN ' .
              '   (SELECT pun_id FROM punter WHERE pun_betting_club IN ' .
              '     ( SELECT jur_id FROM jurisdiction' .
              '         WHERE jur_id = ' . $jurisdiction .  //club?
              '         OR jur_parent_id = ' . $jurisdiction  . //district?
              '         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = '  .$jurisdiction  . ')' . //region?
              '         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.'))' . //nation?
              '     ) ' .
              '   ) ';
    }

    if ( 'club' == $_SESSION['jurisdiction_class'] ) { //If a club admin user
      //only show transactions for punters in same club as admin user
      $sql .= ' AND ctr_pun_id IN ' .
              '  ( SELECT pun_id FROM punter WHERE pun_betting_club =' . $_SESSION['jurisdiction_id'] . ' AND pun_id=ctr_pun_id ) ';
    }
    elseif ( 'district' == $_SESSION['jurisdiction_class'] ) { //If a district manager admin user
      // only select punters where their betting club (bottom level jurisdiction)
      // is under the admin user's district
      $sql .= ' AND ctr_pun_id IN ' .
              '     ( SELECT pun_id FROM punter WHERE pun_betting_club IN' .
              '       (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id = ' . $_SESSION['jurisdiction_id'] . ' ) ' .
              '     )';
    }
    elseif ( 'region' == $_SESSION['jurisdiction_class'] ) { //If a region manager admin user
      // only select punters where their betting club (bottom level jurisdiction)
      // is under the admin user's district
      // select only  records within admin user's jurisdiction
      $sql .= ' AND ctr_pun_id IN ' .
              '   (SELECT pun_id FROM punter WHERE pun_betting_club IN ' .
              '     (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
              '          (  SELECT jur_id FROM jurisdiction ' .
              '             WHERE jur_parent_id = ' .$_SESSION['jurisdiction_id'] .
              '          )' .
              '     ) ' .
              '   )';
    }
    elseif ('nation' == $_SESSION['jurisdiction_class'] ) { //if a nation manager admin user
      $sql .= ' AND ctr_pun_id IN ' .
              '   (SELECT pun_id FROM punter WHERE pun_betting_club IN ' .
              '       (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
              '            (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
              '                 (SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$_SESSION['jurisdiction_id'].')'.
              '            )'.
              '       )'.
              '   )';
    }
    else{
      if ( 'casino' != $_SESSION['jurisdiction_class'] ){
        dieWithError("Invalid user Jurisdiction Class!");
      }
    }

    $sql .= " GROUP BY n.jur_name, r.jur_name, d.jur_name, c.jur_name, c.jur_id";
    $sql .= " ORDER by c.jur_name";
    $rs2  = $dbh->doCachedQuery($sql, CACHE_VALUE);
    $num_rows2 = $rs2->getNumRows();
    //if records found
    if ( $num_rows2 > 0 ) {
      $rs2->resetRecPtr();
      while ( $rs2->hasNext() ) {  //append other columns to records
        $rec2 = $rs2->next();
        $rs->resetRecPtr();  //to start searching for same game
        //search for matching regions in original result set for previous days
        $found = false;
        while ( $rs->next() ) {
          $rec = & $rs->currentRef();
          if ( $rec['clubid'] == $rec2['clubid'] ) { //match found
            $rec['financial']  += $rec2['financial'];
            $found = true;
            break; //since found
          }
        }
        if(!$found){
          $rs->appendRecords(array($rec2));
        }
      }
    }
  }
  if($rs)$rs->resetRecPtr();
  return $rs;
}

function getCasinoByDistrict($partnerid) {
  global $dbh, $jurisdiction;
  global $start_date, $end_date, $today_date, $yesterday_date, $search_today;

  $archive_date = (($search_today) ? $yesterday_date : $end_date);
  $sql = "SELECT d.jur_id as districtid" .
    	   "     , coalesce(sum(cus_cas_amount), 0) as casino, 0 as rake, 0 as tournament" .
         "  FROM customer_casino_daily" .
         "    JOIN punter ON cus_cas_pun_id=pun_id " .
         "    JOIN jurisdiction c ON pun_betting_club = c.jur_id " .
         "    JOIN jurisdiction d ON c.jur_parent_id = d.jur_id  " .
         "    JOIN jurisdiction r ON d.jur_parent_id = r.jur_id  " .
         "    JOIN jurisdiction n ON r.jur_parent_id = n.jur_id  " .
         "  WHERE cus_cas_day BETWEEN '$start_date' AND '$archive_date' ";
  $sql .= " AND cus_cas_ptn_id = $partnerid ";

  // Filter by jurisdiction
  if ( $jurisdiction ){
    $sql .= ' AND cus_cas_pun_id IN ' .
            '   (SELECT pun_id FROM punter WHERE pun_betting_club IN ' .
            '     ( SELECT jur_id FROM jurisdiction' .
            '         WHERE jur_id = ' . $jurisdiction .  //club?
            '         OR jur_parent_id = ' . $jurisdiction  . //district?
            '         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = '  .$jurisdiction  . ')' . //region?
            '         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.'))' . //nation?
            '     ) ' .
            '   ) ';
  }

  if ( 'nation' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND d.jur_id IN " .
            "      (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN " .
            "          (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " . $_SESSION["jurisdiction_id"] . ")" .
            "      )";
  }
  elseif ( 'region' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND d.jur_id IN " .
            "      (  SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " .$_SESSION["jurisdiction_id"] . ")";
  }
  elseif ( 'district' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND d.jur_id = " . $_SESSION['jurisdiction_id'];
  }
  else {
    if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
      $sql .= " AND 1 = 0";
    }
  }

  $sql .= " GROUP BY n.jur_name, r.jur_name, d.jur_name, d.jur_id";
  $sql .= " ORDER by d.jur_name";

  $rs = $dbh->doCachedQuery($sql, CACHE_VALUE);

  if ($search_today) {
    $sql = "SELECT d.jur_id as districtid, d.jur_name as district" .
           "     , r.jur_name as region, n.jur_name as nation" .
    	     "     , coalesce(sum(ctr_amount), 0) as casino" .
           "  FROM customer_transaction" .
           "    JOIN punter on ctr_pun_id=pun_id" .
           "    JOIN jurisdiction c ON pun_betting_club = c.jur_id" .
           "    JOIN jurisdiction d ON c.jur_parent_id = d.jur_id" .
           "    JOIN jurisdiction r ON d.jur_parent_id = r.jur_id" .
           "    JOIN jurisdiction n ON r.jur_parent_id = n.jur_id" .
           "  WHERE ctr_tty_id = 11 " .
           "    AND ctr_time BETWEEN '$today_date' AND '$end_date' ";
    //TODO Da decommentare quando si modifica customer_transaction
    //  Per ora le prende tutte. Dato che Royal PK E' l'unica sara' sempre 4
    //$sql .= " AND  ctr_ptn_id = $partnerid ";

    // Filter by jurisdiction
    if ( $jurisdiction ){
      $sql .= ' AND ctr_pun_id IN ' .
              '   (SELECT pun_id FROM punter WHERE pun_betting_club IN ' .
              '     ( SELECT jur_id FROM jurisdiction' .
              '         WHERE jur_id = ' . $jurisdiction .  //club?
              '         OR jur_parent_id = ' . $jurisdiction  . //district?
              '         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = '  .$jurisdiction  . ')' . //region?
              '         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.'))' . //nation?
              '     ) ' .
              '   ) ';
    }

    if ( 'nation' == $_SESSION['jurisdiction_class'] ) {
      $sql .= " AND d.jur_id IN " .
              "      (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN " .
              "          (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " . $_SESSION["jurisdiction_id"] . ")" .
              "      )";
    }
    elseif ( 'region' == $_SESSION['jurisdiction_class'] ) {
      $sql .= " AND d.jur_id IN " .
              "      (  SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " .$_SESSION["jurisdiction_id"] . ")";
    }
    elseif ( 'district' == $_SESSION['jurisdiction_class'] ) {
      $sql .= " AND d.jur_id = " . $_SESSION['jurisdiction_id'];
    }
    else {
      if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
        $sql .= " AND 1 = 0";
      }
    }

    $sql .= " GROUP BY n.jur_name, r.jur_name, d.jur_name, d.jur_id";
    $sql .= " ORDER by d.jur_name";

    $rs2  = $dbh->doCachedQuery($sql, CACHE_VALUE);
    $num_rows2 = $rs2->getNumRows();
    //if records found
    if ( $num_rows2 > 0 ) {
      $rs2->resetRecPtr();
      while ( $rs2->hasNext() ) {  //append other columns to records
        $rec2 = $rs2->next();
        $rs->resetRecPtr();  //to start searching for same game
        //search for matching regions in original result set for previous days
        $found = false;
        while ( $rs->next() ) {
          $rec = & $rs->currentRef();
          if ( $rec['districtid'] == $rec2['districtid'] ) { //match found
            $rec['financial']  += $rec2['financial'];
            $found = true;
            break; //since found
          }
        }
        if ( ! $found ) {
          $rs->appendRecords(array($rec2));
        }
      }
    }
  }
  if($rs)$rs->resetRecPtr();
  return $rs;
}

function getCasinoByRegion($partnerid) {
  global $dbh, $jurisdiction;
  global $start_date, $end_date, $today_date, $yesterday_date, $search_today;

  $archive_date = (($search_today) ? $yesterday_date : $end_date);
  $sql = "SELECT r.jur_id as regionid, r.jur_name as region" .
         "     , n.jur_name as nation" .
         "     , coalesce(sum(cus_cas_amount), 0) as casino, 0 as rake, 0 as tournament, 0 as financial" .
         "  FROM customer_casino_daily" .
         "    JOIN punter ON cus_cas_pun_id=pun_id" .
         "    JOIN jurisdiction c ON pun_betting_club = c.jur_id" .
         "    JOIN jurisdiction d ON c.jur_parent_id  = d.jur_id" .
         "    JOIN jurisdiction r ON d.jur_parent_id  = r.jur_id" .
         "    JOIN jurisdiction n ON r.jur_parent_id  = n.jur_id" .
         "  WHERE cus_cas_day BETWEEN '$start_date' AND '$archive_date' ";
  $sql .= " AND cus_cas_ptn_id = $partnerid";

  // Filter by jurisdiction
  if ( $jurisdiction ){
    $sql .= ' AND cus_cas_pun_id IN ' .
            '   (SELECT pun_id FROM punter WHERE pun_betting_club IN ' .
            '     ( SELECT jur_id FROM jurisdiction' .
            '         WHERE jur_id = ' . $jurisdiction .  //club?
            '         OR jur_parent_id = ' . $jurisdiction  . //district?
            '         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = '  .$jurisdiction  . ')' . //region?
            '         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.'))' . //nation?
            '     ) ' .
            '   ) ';
  }

  if ( 'nation' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND r.jur_id IN " .
            "      (  SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " .$_SESSION["jurisdiction_id"] . ")";
  }
  elseif ( 'region' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND r.jur_id = " . $_SESSION['jurisdiction_id'];
  }
  else {
    if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
      $sql .= " AND 1 = 0";
    }
  }

  $sql .= " GROUP BY n.jur_name, r.jur_name, r.jur_id";
  $sql .= " ORDER BY r.jur_name";

  $rs = $dbh->doCachedQuery($sql, CACHE_VALUE);

  if ($search_today) {
    $sql = "SELECT r.jur_id as regionid, r.jur_name as region" .
           "     , n.jur_name as nation" .
    	     "     , coalesce(sum(ctr_amount), 0) as casino, 0 as financial, 0 as rake, 0 as tournament" .
           "  FROM customer_transaction" .
           "    JOIN punter on ctr_pun_id=pun_id" .
           "    JOIN jurisdiction c ON pun_betting_club = c.jur_id" .
           "    JOIN jurisdiction d ON c.jur_parent_id = d.jur_id" .
           "    JOIN jurisdiction r ON d.jur_parent_id = r.jur_id" .
           "    JOIN jurisdiction n ON r.jur_parent_id = n.jur_id" .
           "  WHERE ctr_tty_id = 11 " .
           "  AND ctr_time BETWEEN '$today_date' AND '$end_date' ";
    //TODO Da decommentare quando si modifica customer_transaction
    //  Per ora le prende tutte. Dato che Royal PK E' l'unica sara' sempre 4
    //$sql .= " AND  ctr_ptn_id = $partnerid ";

    // Filter by jurisdiction
    if ( $jurisdiction ){
      $sql .= ' AND ctr_pun_id IN ' .
              '   (SELECT pun_id FROM punter WHERE pun_betting_club IN ' .
              '     ( SELECT jur_id FROM jurisdiction' .
              '         WHERE jur_id = ' . $jurisdiction .      //club?
              '         OR jur_parent_id = ' . $jurisdiction  . //district?
              '         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = '  .$jurisdiction  . ')' . //region?
              '         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.'))' . //nation?
              '     ) ' .
              '   ) ';
    }

    if ( 'nation' == $_SESSION['jurisdiction_class'] ) {
      $sql .= " AND r.jur_id IN " .
              "      (  SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " .$_SESSION["jurisdiction_id"] . ")";
    }
    elseif ( 'region' == $_SESSION['jurisdiction_class'] ) {
      $sql .= " AND r.jur_id = " . $_SESSION['jurisdiction_id'];
    }
    else {
      if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
        $sql .= " AND 1 = 0";
      }
    }

    $sql .= " GROUP BY n.jur_name, r.jur_name, r.jur_id";
    $sql .= " ORDER by r.jur_name";


    $rs2  = $dbh->doCachedQuery($sql, CACHE_VALUE);
    $num_rows2 = $rs2->getNumRows();
    //if records found
    if ( $num_rows2 > 0 ) {
      $rs2->resetRecPtr();
      while ( $rs2->hasNext() ) {  //append other columns to records
        $rec2 = $rs2->next();
        $rs->resetRecPtr();  //to start searching for same game
        //search for matching regions in original result set for previous days
        $found = false;
        while ( $rs->next() ) {
          $rec = & $rs->currentRef();
          if ( $rec['regionid'] == $rec2['regionid'] ) { //match found
            $rec['financial']  += $rec2['financial'];
            $found = true;
            break; //since found
          }
        }
        if ( ! $found ) {
          $rs->appendRecords(array($rec2));
        }
      }
    }
  }
   if($rs)$rs->resetRecPtr();
  return $rs;
}

function getCasinoByNation($partnerid) {
  global $dbh, $jurisdiction;
  global $start_date, $end_date, $today_date, $yesterday_date, $search_today;

  $archive_date = (($search_today) ? $yesterday_date : $end_date);
  $sql =  "SELECT n.jur_id as nationid, n.jur_name as nation" .
          "     , coalesce(sum(cus_cas_amount), 0) as casino, 0 as financial, 0 as rake, 0 as tournament" .
          "  FROM customer_casino_daily" .
          "    JOIN punter ON cus_cas_pun_id=pun_id" .
          "    JOIN jurisdiction c ON pun_betting_club = c.jur_id" .
          "    JOIN jurisdiction d ON c.jur_parent_id  = d.jur_id" .
          "    JOIN jurisdiction r ON d.jur_parent_id  = r.jur_id" .
          "    JOIN jurisdiction n ON r.jur_parent_id  = n.jur_id" .
          "  WHERE cus_cas_day BETWEEN '$start_date' AND '$archive_date' ";
  $sql .= " AND cus_cas_ptn_id = $partnerid ";

  // Filter by jurisdiction
  if ( $jurisdiction ){
    $sql .= ' AND cus_cas_pun_id IN ' .
            '   (SELECT pun_id FROM punter WHERE pun_betting_club IN ' .
            '     ( SELECT jur_id FROM jurisdiction' .
            '         WHERE jur_id = ' . $jurisdiction .  //club?
            '         OR jur_parent_id = ' . $jurisdiction  . //district?
            '         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = '  .$jurisdiction  . ')' . //region?
            '         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.'))' . //nation?
            '     ) ' .
            '   ) ';
  }

  if ( 'nation' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND n.jur_id = " . $_SESSION['jurisdiction_id'];
  }
  else {
    if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
      $sql .= " AND 1 = 0";
    }
  }

  $sql .= " GROUP BY n.jur_name, n.jur_id";
  $sql .= " ORDER by n.jur_name";

  $rs = $dbh->doCachedQuery($sql, CACHE_VALUE);

  if ($search_today) {
    $sql = "SELECT n.jur_id as nationid, n.jur_name as nation" .
    	     "     , coalesce(sum(ctr_amount), 0) as casino,0 as financial, 0 as rake, 0 as tournament" .
           "  FROM customer_transaction" .
           "    JOIN punter on ctr_pun_id=pun_id" .
           "    JOIN jurisdiction c ON pun_betting_club = c.jur_id" .
           "    JOIN jurisdiction d ON c.jur_parent_id = d.jur_id" .
           "    JOIN jurisdiction r ON d.jur_parent_id = r.jur_id" .
           "    JOIN jurisdiction n ON r.jur_parent_id = n.jur_id" .
           "  WHERE ctr_tty_id = 11" .
           "  AND ctr_ptn_id = $partnerid " .
           "    AND ctr_time BETWEEN '$today_date' AND '$end_date' ";

    // Filter by jurisdiction
    if ( $jurisdiction ){
      $sql .= ' AND ctr_pun_id IN ' .
              '   (SELECT pun_id FROM punter WHERE pun_betting_club IN ' .
              '     ( SELECT jur_id FROM jurisdiction' .
              '         WHERE jur_id = ' . $jurisdiction .  //club?
              '         OR jur_parent_id = ' . $jurisdiction  . //district?
              '         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = '  .$jurisdiction  . ')' . //region?
              '         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.'))' . //nation?
              '     ) ' .
              '   ) ';
    }
    //TODO Da decommentare quando si modifica customer_transaction
    //  Per ora le prende tutte. Dato che Royal PK E' l'unica sara' sempre 4
    //$sql .= " AND  ctr_ptn_id = $partnerid ";

    if ( 'nation' == $_SESSION['jurisdiction_class'] ) {
      $sql .= " AND r.jur_id = " . $_SESSION['jurisdiction_id'];
    }
    else {
      if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
        $sql .= " AND 1 = 0";
      }
    }
    if(is_null($rs)){ return null;}
    $sql .= " GROUP BY n.jur_name, n.jur_id";
    $sql .= " ORDER by n.jur_name";
    $rs2  = $dbh->doCachedQuery($sql, CACHE_VALUE);
    $num_rows2 = $rs2->getNumRows();
    //if records found

    if ( $num_rows2 > 0 ) {
      $rs2->resetRecPtr();
      while ( $rs2->hasNext() ) {  //append other columns to records
        $rec2 = $rs2->next();
        $rs->resetRecPtr();  //to start searching for same game
        //search for matching regions in original result set for previous days
        $found = false;
        while ( $rs->next() ) {
          $rec = & $rs->currentRef();
          if ( $rec['nationid'] == $rec2['nationid'] ) { //match found
            $rec['financial']  += $rec2['financial'];
            $found = true;
            break; //since found
          }
        }
        if ( ! $found ) {
          $rs->appendRecords(array($rec2));
        }
      }
    }
  }
  if($rs)$rs->resetRecPtr();
  return $rs;
}

function getCasinoByCustomer($partnerid) {
  global $dbh, $jurisdiction;
  global $start_date, $end_date, $today_date, $yesterday_date, $search_today;

  $archive_date = (($search_today) ? $yesterday_date : $end_date);
  $sql =  "SELECT pun_id as customer_id, pun_username as customer, c.jur_name as club" .
          "     , coalesce(sum(cus_cas_amount), 0) as casino, 0 as financial, 0 as rake, 0 as tournament" .
          "  FROM customer_casino_daily" .
          "    JOIN punter ON cus_cas_pun_id=pun_id" .
          "    JOIN jurisdiction c ON pun_betting_club = c.jur_id" .
          "  WHERE cus_cas_day BETWEEN '$start_date' AND '$archive_date' ";
  $sql .= " AND cus_cas_ptn_id = $partnerid ";

  if ( 'club' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND c.jur_id = " . $_SESSION['jurisdiction_id'];
  }
  else {
    $sql .= " AND 1 = 0";
  }

  $sql .= " GROUP BY c.jur_name, pun_id, pun_username ";
  $rs = $dbh->doCachedQuery($sql, CACHE_VALUE);

  if($rs)$rs->resetRecPtr();
  return $rs;
}

function getProviderByClub($provider_id = null){
  global $dbh, $jurisdiction;
  global $start_date, $end_date, $today_date, $yesterday_date, $search_today;

  $sql = "SELECT c.jur_id as clubid, c.jur_name as club, d.jur_name as district, r.jur_name as region, n.jur_name as nation, cus_pro_bet_type, SUM(cus_pro_bet) AS cus_pro_bet, SUM(cus_pro_win) AS cus_pro_win
  FROM customer_provider_daily 
  JOIN punter ON pun_id = cus_pro_pun_id
  JOIN jurisdiction c ON pun_betting_club = c.jur_id
  JOIN jurisdiction d ON c.jur_parent_id = d.jur_id
  JOIN jurisdiction r ON d.jur_parent_id = r.jur_id
  JOIN jurisdiction n ON r.jur_parent_id = n.jur_id
  WHERE cus_pro_day BETWEEN '$start_date' AND '$end_date'";  
  // Filter by jurisdiction
  if ( $jurisdiction ){
    $sql .= ' AND cus_pro_pun_id IN ' .
            '   (SELECT pun_id FROM punter WHERE pun_betting_club IN ' .
	          '     ( SELECT jur_id FROM jurisdiction' .
	          '         WHERE jur_id = ' . $jurisdiction .  //club?
	          '         OR jur_parent_id = ' . $jurisdiction  . //district?
	          '         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = '  .$jurisdiction  . ')' . //region?
	          '         OR jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.'))' . //nation?
	          '     ) ' .
	          '   ) ';
  }
  if ( 'club' == $_SESSION['jurisdiction_class'] ) { //If a club admin user
    $sql .= ' AND cus_pro_pun_id IN ' .
	          '  ( SELECT pun_id FROM punter WHERE pun_betting_club =' . $_SESSION['jurisdiction_id'] . ' AND pun_id=cus_pro_pun_id ) ';
  }
  elseif ( 'district' == $_SESSION['jurisdiction_class'] ) { //If a district manager admin user
    $sql .= ' AND cus_pro_pun_id IN ' .
	          '     ( SELECT pun_id FROM punter WHERE pun_betting_club IN' .
	          '       (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id = ' . $_SESSION['jurisdiction_id'] . ' ) ' .
	          '     )';
  }
  elseif ( 'region' == $_SESSION['jurisdiction_class'] ) { //If a region manager admin user
    $sql .= ' AND cus_pro_pun_id IN ' .
      	    '   (SELECT pun_id FROM punter WHERE pun_betting_club IN ' .
      	    '     (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
      	    '          (  SELECT jur_id FROM jurisdiction ' .
      	    '             WHERE jur_parent_id = ' .$_SESSION['jurisdiction_id'] .
      	    '          )' .
      	    '     ) ' .
      	    '   )';
  }
  elseif ('nation' == $_SESSION['jurisdiction_class'] ) { //if a nation manager admin user
    $sql .= ' AND cus_pro_pun_id IN ' .
            '   (SELECT pun_id FROM punter WHERE pun_betting_club IN ' .
            '       (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
            '            (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
            '                 (SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$_SESSION['jurisdiction_id'].')'.
            '            )'.
            '       )'.
            '   )';
  }
  else{
    if ( 'casino' != $_SESSION['jurisdiction_class'] ){
      dieWithError("Invalid user Jurisdiction Class!");
    }
  }

  
  if(!empty($provider_id)){
    $sql .= " AND cus_pro_provider_id = $provider_id";
  }
  
  $sql .= " GROUP BY n.jur_name, r.jur_name, d.jur_name, c.jur_name, c.jur_id, cus_pro_bet_type";

  $rs  = $dbh->doCachedQuery($sql, CACHE_VALUE);
  
  $bettings  = $rs->Records;
  $bettings2 = $rs->Records; 

  foreach($bettings as $k => $main_betting){
    //Analizing fathers
    
    unset($bettings[$k]["cus_pro_bet_type"]);
    unset($bettings[$k]["cus_pro_bet"]);
    unset($bettings[$k]["cus_pro_win"]);
    $found = false;
    
    foreach($bettings2 as $i => $child_betting){
      //Comparing with father and sons
      
      if($main_betting["clubid"] == $child_betting["clubid"]){
        //Adding data from child to father
        $bettings[$k][$child_betting["cus_pro_bet_type"] . "_bet"] = $child_betting["cus_pro_bet"];
        $bettings[$k][$child_betting["cus_pro_bet_type"] . "_win"] = $child_betting["cus_pro_win"];
        $found = true;
        unset($bettings2[$i]);
      }
    }
    
    if(!$found){
      unset($bettings[$k]);
    }
    
  }

  $rs->Records = $bettings;
  return $rs;
}

function getProviderByDistrict($provider_id = null){
  global $dbh, $jurisdiction;
  global $start_date, $end_date, $today_date, $yesterday_date, $search_today;

  $sql = "SELECT d.jur_id as districtid, d.jur_name as district, r.jur_name as region, n.jur_name as nation, cus_pro_bet_type, SUM(cus_pro_bet) AS cus_pro_bet, SUM(cus_pro_win) AS cus_pro_win
  FROM customer_provider_daily 
  JOIN punter ON pun_id = cus_pro_pun_id
  JOIN jurisdiction c ON pun_betting_club = c.jur_id
  JOIN jurisdiction d ON c.jur_parent_id = d.jur_id
  JOIN jurisdiction r ON d.jur_parent_id = r.jur_id
  JOIN jurisdiction n ON r.jur_parent_id = n.jur_id
  WHERE cus_pro_day BETWEEN '$start_date' AND '$end_date'";  
  // Filter by jurisdiction
  if ( $jurisdiction ) {
    $sql .=  ' AND (  ( pun_betting_club =  '.$jurisdiction . ')' .
             '        OR' .
             '        (    pun_betting_club IN ' .   //if district jurisdiction selected,
             '             ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id=' . $jurisdiction . ' )' .
             '        )' .
             '        OR' .
             '        (    pun_betting_club IN ' .    //if region jurisdiction selected,
             '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction .' )' .
             '             )' .
             '        )' .
             '        OR' .
             '        (    pun_betting_club IN ' .
             '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                     (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.' ) ' .
             '                 )' .
             '             )' .
             '        )' .
             '     )';
  }

  if ( 'nation' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND d.jur_id IN " .
            "      (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN " .
            "          (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " . $_SESSION["jurisdiction_id"] . ")" .
            "      )";
  }
  elseif ( 'region' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND d.jur_id IN " .
            "      (  SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " .$_SESSION["jurisdiction_id"] . ")";
  }
  elseif ( 'district' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND d.jur_id = " . $_SESSION['jurisdiction_id'];
  }
  else {
    if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
      $sql .= " AND 1 = 0";
    }
  }

  if(!empty($provider_id)){
    $sql .= " AND cus_pro_provider_id = $provider_id";
  }
  
  $sql .= " GROUP BY n.jur_name, r.jur_name, d.jur_name, d.jur_id, cus_pro_bet_type";

  $rs  = $dbh->doCachedQuery($sql, CACHE_VALUE);
  
  $bettings  = $rs->Records;
  $bettings2 = $rs->Records; 

  foreach($bettings as $k => $main_betting){
    //Analizing fathers
    
    unset($bettings[$k]["cus_pro_bet_type"]);
    unset($bettings[$k]["cus_pro_bet"]);
    unset($bettings[$k]["cus_pro_win"]);
    $found = false;
    
    foreach($bettings2 as $i => $child_betting){
      //Comparing with father and sons
      
      if($main_betting["districtid"] == $child_betting["districtid"]){
        //Adding data from child to father
        $bettings[$k][$child_betting["cus_pro_bet_type"] . "_bet"] = $child_betting["cus_pro_bet"];
        $bettings[$k][$child_betting["cus_pro_bet_type"] . "_win"] = $child_betting["cus_pro_win"];
        $found = true;
        unset($bettings2[$i]);
      }
    }
    
    if(!$found){
      unset($bettings[$k]);
    }
    
  }
  
  $rs->Records = $bettings;
  return $rs;
}


function getProviderByRegion($provider_id = null){
  global $dbh, $jurisdiction;
  global $start_date, $end_date, $today_date, $yesterday_date, $search_today;

  $sql = "SELECT r.jur_id as regionid, r.jur_name as region, n.jur_name as nation, cus_pro_bet_type, SUM(cus_pro_bet) AS cus_pro_bet, SUM(cus_pro_win) AS cus_pro_win
  FROM customer_provider_daily 
  JOIN punter ON pun_id = cus_pro_pun_id
  JOIN jurisdiction c ON pun_betting_club = c.jur_id
  JOIN jurisdiction d ON c.jur_parent_id = d.jur_id
  JOIN jurisdiction r ON d.jur_parent_id = r.jur_id
  JOIN jurisdiction n ON r.jur_parent_id = n.jur_id
  WHERE cus_pro_day BETWEEN '$start_date' AND '$end_date'";  
  // Filter by jurisdiction
  if ( $jurisdiction ) {
    $sql .=  ' AND (  ( pun_betting_club =  '.$jurisdiction . ')' .
             '        OR' .
             '        (    pun_betting_club IN ' .   //if district jurisdiction selected,
             '             ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id=' . $jurisdiction . ' )' .
             '        )' .
             '        OR' .
             '        (    pun_betting_club IN ' .    //if region jurisdiction selected,
             '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction .' )' .
             '             )' .
             '        )' .
             '        OR' .
             '        (    pun_betting_club IN ' .
             '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                     (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.' ) ' .
             '                 )' .
             '             )' .
             '        )' .
             '     )';
  }

  if ( 'nation' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND r.jur_id IN " .
            "      (  SELECT jur_id FROM jurisdiction WHERE jur_parent_id = " .$_SESSION["jurisdiction_id"] . ")";
  }
  elseif ( 'region' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND r.jur_id = " . $_SESSION['jurisdiction_id'];
  }
  else {
    if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
      $sql .= " AND 1 = 0";
    }
  }

  if(!empty($provider_id)){
    $sql .= " AND cus_pro_provider_id = $provider_id";
  }
  
  $sql .= " GROUP BY n.jur_name, r.jur_name, r.jur_id, cus_pro_bet_type";

  $rs  = $dbh->doCachedQuery($sql, CACHE_VALUE);
  
  $bettings  = $rs->Records;
  $bettings2 = $rs->Records; 

  foreach($bettings as $k => $main_betting){
    //Analizing fathers
    
    unset($bettings[$k]["cus_pro_bet_type"]);
    unset($bettings[$k]["cus_pro_bet"]);
    unset($bettings[$k]["cus_pro_win"]);
    $found = false;
    
    foreach($bettings2 as $i => $child_betting){
      //Comparing with father and sons
      
      if($main_betting["regionid"] == $child_betting["regionid"]){
        //Adding data from child to father
        $bettings[$k][$child_betting["cus_pro_bet_type"] . "_bet"] = $child_betting["cus_pro_bet"];
        $bettings[$k][$child_betting["cus_pro_bet_type"] . "_win"] = $child_betting["cus_pro_win"];
        $found = true;
        unset($bettings2[$i]);
      }
    }
    
    if(!$found){
      unset($bettings[$k]);
    }
    
  }

  $rs->Records = $bettings;
  return $rs;
}


function getProviderByNation($provider_id = null){
  global $dbh, $jurisdiction;
  global $start_date, $end_date, $today_date, $yesterday_date, $search_today;

  $sql = "SELECT n.jur_id as nationid, n.jur_name as nation, cus_pro_bet_type, SUM(cus_pro_bet) AS cus_pro_bet, SUM(cus_pro_win) AS cus_pro_win
  FROM customer_provider_daily 
  JOIN punter ON pun_id = cus_pro_pun_id
  JOIN jurisdiction c ON pun_betting_club = c.jur_id
  JOIN jurisdiction d ON c.jur_parent_id = d.jur_id
  JOIN jurisdiction r ON d.jur_parent_id = r.jur_id
  JOIN jurisdiction n ON r.jur_parent_id = n.jur_id
  WHERE cus_pro_day BETWEEN '$start_date' AND '$end_date'";  
  // Filter by jurisdiction
  if ( $jurisdiction ) {
    $sql .=  ' AND (  ( pun_betting_club =  '.$jurisdiction . ')' .
             '        OR' .
             '        (    pun_betting_club IN ' .   //if district jurisdiction selected,
             '             ( SELECT jur_id FROM jurisdiction WHERE jur_parent_id=' . $jurisdiction . ' )' .
             '        )' .
             '        OR' .
             '        (    pun_betting_club IN ' .    //if region jurisdiction selected,
             '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction .' )' .
             '             )' .
             '        )' .
             '        OR' .
             '        (    pun_betting_club IN ' .
             '             (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                 (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN ' .
             '                     (   SELECT jur_id FROM jurisdiction WHERE jur_parent_id='.$jurisdiction.' ) ' .
             '                 )' .
             '             )' .
             '        )' .
             '     )';
  }

  if ( 'nation' == $_SESSION['jurisdiction_class'] ) {
    $sql .= " AND n.jur_id = " . $_SESSION['jurisdiction_id'];
  }
  else {
    if ( 'casino' != $_SESSION['jurisdiction_class'] ) {
      $sql .= " AND 1 = 0";
    }
  }

  if(!empty($provider_id)){
    $sql .= " AND cus_pro_provider_id = $provider_id";
  }
  
  $sql .= " GROUP BY n.jur_name, n.jur_id, cus_pro_bet_type";

  $rs  = $dbh->doCachedQuery($sql, CACHE_VALUE);
  
  $bettings  = $rs->Records;
  $bettings2 = $rs->Records; 

  foreach($bettings as $k => $main_betting){
    //Analizing fathers
    
    unset($bettings[$k]["cus_pro_bet_type"]);
    unset($bettings[$k]["cus_pro_bet"]);
    unset($bettings[$k]["cus_pro_win"]);
    $found = false;
    
    foreach($bettings2 as $i => $child_betting){
      //Comparing with father and sons
      
      if($main_betting["nationid"] == $child_betting["nationid"]){
        //Adding data from child to father
        $bettings[$k][$child_betting["cus_pro_bet_type"] . "_bet"] = $child_betting["cus_pro_bet"];
        $bettings[$k][$child_betting["cus_pro_bet_type"] . "_win"] = $child_betting["cus_pro_win"];
        $found = true;
        unset($bettings2[$i]);
      }
    }
    
    if(!$found){
      unset($bettings[$k]);
    }
    
  }

  $rs->Records = $bettings;
  /*
  echo "<pre>";
  echo "Provider $provider_id\n";
  var_dump($rs);
  echo "</pre>";
  //*/

  //$ret = new RecordSet($bettings);

  return $rs;
}



?>
