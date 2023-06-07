<?php
require_once("SiteCache.class.php");

if ( ! class_exists('JurisdictionsList' )){
    class JurisdictionsList{
        private $dbh;
        private static $instance;

        const JURISDICTIONS_CACHE_NAME = "jurisdictions/jur_%s";

        public function __construct(&$dbh){
            $this->dbh = $dbh;
        }

        public static function getInstance(&$dbh = null){
            if(self::$instance == null){
                self::$instance = new JurisdictionsList($dbh);
            }
            return self::$instance;
        }

        private function getJurCacheFileName($jur_id){
            return sprintf(self::JURISDICTIONS_CACHE_NAME, $jur_id);
        }

        private function saveJurList($jur_id, &$jur_list){
            $cache_name = $this->getJurCacheFileName($jur_id);
            return SiteCache::store($cache_name, serialize($jur_list));
        }

        private function loadJurList($jur_id){
            $cache_name = $this->getJurCacheFileName($jur_id);
            return unserialize(SiteCache::fetch($cache_name));
        }

        public function destroyJurisdictionCache($jur_id){
            //echo "Delete " . $this->getJurisdictionClass($jur_id) . " " . $jur_id . "<br/>";
            $cache_name = $this->getJurCacheFileName($jur_id);
            error_log("Delete $cache_name");
            SiteCache::delete($cache_name);
            //error_log("Delete jurisdictions/class_$jur_id");
            //SiteCache::delete("jurisdictions/class_$jur_id");
            error_log("Delete jurisdictions/tree_$jur_id");
            SiteCache::delete("jurisdictions/tree_$jur_id");
            error_log("Delete jurisdictions/ptree_$jur_id");
            SiteCache::delete("jurisdictions/ptree_$jur_id");

            //error_log("Delete jurisdictions/hierarchy_$jur_id");
            //SiteCache::delete("jurisdictions/hierarchy_$jur_id");
        }

        private function getJurisdictionClass($jur_id){
           // $jur_class = SiteCache::fetch("jurisdictions/class_$jur_id");
            if(empty($jur_class)){
                $sql = "SELECT jur_class FROM jurisdiction WHERE jur_id = " . $jur_id;
                $rs  = $this->dbh->exec($sql);

                if($rs->hasNext()){
                    $jurisdiction = $rs->next();
                    $jur_class = $jurisdiction["jur_class"];
                   // SiteCache::store("jurisdictions/class_$jur_id", $jur_class);
                }
            }
            return $jur_class;
        }

        public function getJurisdictionHierarchy($jur_id,$only_class=false){
            if(empty($hierarchy)){
                $jur_class = $this->getJurisdictionClass($jur_id);
                $sql = null;
                switch($jur_class){
                    case "casino":
                        if($only_class){
                            $sql = "SELECT c.jur_name casino FROM jurisdiction c WHERE c.jur_id = " . $jur_id;
                        }
                        else{
                            $sql = "SELECT c.jur_id casino FROM jurisdiction c WHERE c.jur_id = " . $jur_id;
                        }
                        break;
                    case "nation":
                        if($only_class){
                            $sql = "SELECT n.jur_name nation, c.jur_name casino FROM jurisdiction n JOIN jurisdiction c ON n.jur_parent_id = c.jur_id AND n.jur_id = " . $jur_id;
                        }
                        else{
                            $sql = "SELECT n.jur_id nation, c.jur_id casino FROM jurisdiction n JOIN jurisdiction c ON n.jur_parent_id = c.jur_id AND n.jur_id = " . $jur_id;
                        }
                        break;
                    case "region":
                        if($only_class){
                            $sql = "SELECT r.jur_name region, n.jur_name nation, c.jur_name casino FROM jurisdiction r JOIN jurisdiction n ON n.jur_id = r.jur_parent_id JOIN jurisdiction c ON c.jur_id = n.jur_parent_id AND r.jur_id = " . $jur_id;
                        }
                        else{
                            $sql = "SELECT r.jur_id region, n.jur_id nation, c.jur_id casino FROM jurisdiction r JOIN jurisdiction n ON n.jur_id = r.jur_parent_id JOIN jurisdiction c ON c.jur_id = n.jur_parent_id AND r.jur_id = " . $jur_id;
                        }
                        break;
                    case "district":
                        if($only_class){
                            $sql = "SELECT d.jur_name district , r.jur_name region, n.jur_name nation, c.jur_name casino FROM jurisdiction d JOIN jurisdiction r ON r.jur_id = d.jur_parent_id JOIN jurisdiction n ON n.jur_id = r.jur_parent_id JOIN jurisdiction c ON c.jur_id = n.jur_parent_id AND d.jur_id = " . $jur_id;
                        }
                        else{
                            $sql = "SELECT d.jur_id district , r.jur_id region, n.jur_id nation, c.jur_id casino FROM jurisdiction d JOIN jurisdiction r ON r.jur_id = d.jur_parent_id JOIN jurisdiction n ON n.jur_id = r.jur_parent_id JOIN jurisdiction c ON c.jur_id = n.jur_parent_id AND d.jur_id = " . $jur_id;
                        }
                        break;
                    case "club":
                        if($only_class){
                            $sql = "SELECT cl.jur_name club, d.jur_name district, r.jur_name region, n.jur_name nation, c.jur_name casino FROM jurisdiction cl JOIN jurisdiction d ON d.jur_id = cl.jur_parent_id JOIN jurisdiction r ON r.jur_id = d.jur_parent_id JOIN jurisdiction n ON n.jur_id = r.jur_parent_id JOIN jurisdiction c ON c.jur_id = n.jur_parent_id AND cl.jur_id = " . $jur_id;
                        }
                        else{
                            $sql = "SELECT cl.jur_id club, d.jur_id district, r.jur_id region, n.jur_id nation, c.jur_id casino FROM jurisdiction cl JOIN jurisdiction d ON d.jur_id = cl.jur_parent_id JOIN jurisdiction r ON r.jur_id = d.jur_parent_id JOIN jurisdiction n ON n.jur_id = r.jur_parent_id JOIN jurisdiction c ON c.jur_id = n.jur_parent_id AND cl.jur_id = " . $jur_id;
                        }
                        break;
                }
                if(!empty($sql)){
                    $rs = $this->dbh->exec($sql);
                    if($rs->hasNext()){
                        $hierarchy = $rs->next();
                    }
                }
            }

            return $hierarchy;
        }

        public function destroyParentsJurisdictionsCache($jur_id){
            $this->destroyChildsJurisdictionsCache($jur_id);
            $hierarchy = $this->getJurisdictionHierarchy($jur_id);
            foreach($hierarchy as $jur_to_remove){
                $this->destroyJurisdictionCache($jur_to_remove);
            }
        }

        public function destroyChildsJurisdictionsCache($jur_id){
            $list = $this->getChildJurisdictions($jur_id);
            foreach($list as $class){
                foreach($class as $jur){
                    $this->destroyJurisdictionCache($jur["id"]);
                }
            }
        }

        public function getJurisdictionTree($jur_id, $jur_status = -1){
            $cache_name = "jurisdictions/tree_" . $jur_id;

           // $cached_tree = unserialize(SiteCache::fetch($cache_name));
            $cached_tree = null;
            if(empty($cached_tree)){
                $list      = $this->getChildJurisdictions($jur_id, $jur_status);
                $jur_class = $this->getJurisdictionClass($jur_id);

                $list[$jur_class] = array(
                    array("id" => $jur_id, "name" => $jur_class, "class" => $jur_class)
                );

                if($jur_class == "district" || $jur_class == "region" || $jur_class == "nation" || $jur_class == "casino"){
                    if(!empty($list["club"])){
                        foreach($list["club"] as $child_key => $child){
                            if(!empty($list["district"])){
                                foreach($list["district"] as $parent_key => $parent){
                                    if($parent["id"] == $child["parent_id"]){
                                        if(is_null($list["district"][$parent_key]["child"])){
                                            $list["district"][$parent_key]["child"] = array();
                                        }
                                        $list["district"][$parent_key]["child"][] = $child;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }

                if($jur_class == "region" || $jur_class == "nation" || $jur_class == "casino"){
                    if(!empty($list["district"])){
                        foreach($list["district"] as $child_key => $child){
                            if(!empty($list["region"])){
                                foreach($list["region"] as $parent_key => $parent){
                                    if($parent["id"] == $child["parent_id"]){
                                        //echo "_Adding district " . $child["name"] . "(" . $child["id"] . ")" . " to region " . $parent["name"] . "(" . $parent["id"] . ")<br/>";
                                        if(is_null($list["region"][$parent_key]["child"])){
                                            $list["region"][$parent_key]["child"] = array();
                                        }
                                        $list["region"][$parent_key]["child"][] = $child;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }

                if($jur_class == "nation" || $jur_class == "casino"){
                    if(!empty($list["region"])){
                        foreach($list["region"] as $child_key => $child){
                            if(!empty($list["nation"])){
                                foreach($list["nation"] as $parent_key => $parent){
                                    if($parent["id"] == $child["parent_id"]){
                                        //echo "Adding region " . $child["name"] . "(" . $child["id"] . ")" . " to nation " . $parent["name"] . "(" . $parent["id"] . ")<br/>";
                                        if(is_null($list["nation"][$parent_key]["child"])){
                                            $list["nation"][$parent_key]["child"] = array();
                                        }
                                        $list["nation"][$parent_key]["child"][] = $child;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }

                if($jur_class == "casino"){
                    if(!empty($list["nation"])){
                        foreach($list["nation"] as $child_key => $child){
                            foreach($list["casino"] as $parent_key => $parent){
                                if($parent["id"] == $child["parent_id"]){
                                    //echo "Adding " . $child["name"] . "(" . $child["id"] . ")" . " to casino " . $parent["name"] . "<br/>";
                                    if(is_null($list["casino"][$parent_key]["child"])){
                                        $list["casino"][$parent_key]["child"] = array();
                                    }
                                    $list["casino"][$parent_key]["child"][] = $child;
                                    break;
                                }
                            }
                        }
                    }
                }
                //SiteCache::store($cache_name, serialize($list[$jur_class]));
                $cached_tree = $list[$jur_class];
            }

            return $cached_tree;
        }

        public function getAdminUsersList($jur_id){
            global $lang;
            $sql = "SELECT aus_id, aus_username, aus_access, aus_last_logged_in, aus_num_logins, aty_name FROM admin_user JOIN admin_user_type ON aty_id = aus_aty_id WHERE aus_jurisdiction_id = $jur_id ORDER BY LOWER(aus_username)";
            $rs = $this->dbh->exec($sql);
            if(!$rs->hasNext()){
                ?>
                <span class="tip"><?=$lang->getLang("No admin users found")?></span>
                <?php
                return;
            }
            ?>
            <br/>
            <ol class="admin_user_list"><span class="tip"><?=$lang->getLang("Administrators")?></span>
                <?php
                while($rs->hasNext()){
                    $row = $rs->next();
                    ?>
                    <li class="<?=$row["aus_access"]?>"><?=getLink("/admin_users/admin_user_edit.php?id=" . $row["aus_id"], $row["aus_username"] . (($row["aus_access"] != "ALLOW")?(" \"" . ucfirst(strtolower($row["aus_access"])) . "\""):(""))) ?><br/><span class="tip"><?=$row["aty_name"]?><br/>
                            <?php if(($row["aus_last_logged_in"]) > 0) : ?>
                                <?php if(function_exists("getReadableDate")) : ?>
                                    Last login, <?= getReadableDate(strtotime($row["aus_last_logged_in"])) ?>
                                <?php else : ?>
                                    Last: <?=date("Y-m-d H:i", strtotime($row["aus_last_logged_in"]))?>
                                <?php endif; ?>
                            <?php else : ?>
                                <span class="error"><?=$lang->getLang("Never logged in")?></span>
                            <?php endif; ?>
        </span></li>
                <?php
                }
                ?>
            </ol>
        <?php
        }

        public function getChilJurisdictionsIdList($jur_id, $only_class = null, $jur_status = -1){
            $jur_id_list = array();
            $list        = $this->getChildJurisdictions($jur_id, $jur_status);

            if(is_null($only_class)){
                foreach($list as $class){
                    if(count($class) > 0){
                        foreach($class as $jur){
                            $jur_id_list[] = $jur["id"];
                        }
                    }
                }
            }else{
                if(count($list[$only_class]) > 0){
                    foreach($list[$only_class] as $jur){
                        $jur_id_list[] = $jur["id"];
                    }
                }
            }

            return $jur_id_list;
        }

        public function getChildListForWhereClause($jur_id, $db_field = "jur_id", $jur_class = null){
            $child_id_list   = $this->getChilJurisdictionsIdList($jur_id, $jur_class);
            $chunked_lists   = array_chunk($child_id_list, 1000);

            $ret_arr = array();
            foreach($chunked_lists as $list){
                $ret_arr[] = "$db_field IN (" . implode(",", $list) . ")";
            }
            return implode(" OR ", $ret_arr);
        }

        public function getChilJurisdictionsDataList($jur_id, $only_class = null){
            $jur_id_list = array();
            $list        = $this->getChildJurisdictions($jur_id);

            if(is_null($only_class)){
                foreach($list as $class){
                    if(count($class) > 0){
                        foreach($class as $jur){
                            $jur_id_list[] = array($jur["id"], $jur["name"]);
                        }
                    }
                }
            }else{
                if(count($list[$only_class]) > 0){
                    foreach($list[$only_class] as $jur){
                        $jur_id_list[] = array($jur["id"], $jur["name"]);
                    }
                }
            }

            return $jur_id_list;
        }

        public function getChildJurisdictions($jur_id, $jur_status = -1){
            //echo "getChildJurisdictions($jur_id)<br/>";
            global $noAffiliates;

            //$cached_list = $this->loadJurList($jur_id);
                $jur_class = $this->getJurisdictionClass($jur_id);
                $sql = "SELECT jur_id, jur_name, jur_code, jur_class, jur_parent_id, jur_creation_date, jur_vat_code, jur_status FROM jurisdiction c";
                switch($jur_class) {
                    case 'club':


                        //get user type that are subordinate to the session user type, within his club
                        $sql .= " WHERE ((c.jur_id  = " . $jur_id. " AND c.jur_status = 1) OR (c.jur_id = $jur_id))";
                        if($noAffiliates){
                            $sql.=" and jur_id not in (
                                select aus_jurisdiction_id from affiliate, admin_user
                                where aff_aus_id = aus_id
                            )";
                        }

                        break;
                    case 'district':
                        //get clubs that are subordinate to session user's district
                        if($jur_status == -1){
                            $sql .= " WHERE ((jur_status = 1 AND jur_parent_id  = " . $jur_id . ") OR (jur_id = $jur_id))";
                        }else{
                            $sql .= " WHERE ((jur_parent_id  = " . $jur_id . ") OR (jur_id = $jur_id))";
                        }
                        if($noAffiliates){
                           $sql.=" and jur_id not in (
                                select aus_jurisdiction_id from affiliate, admin_user
                                where aff_aus_id = aus_id
                            )";
                        }

                        break;
                    case 'region':
                        //get districts and clubs that are subordinate to session user's district
                        if($jur_status == -1){
                            $sql .= " WHERE ((( jur_status = 1 AND jur_parent_id = " . $jur_id  .
                                "       OR ( jur_parent_id IN ( SELECT j2.jur_id FROM jurisdiction j2 WHERE j2.jur_status = 1 AND j2.jur_parent_id = ". $jur_id .")))) OR (jur_id = $jur_id))";
                        }else{
                            $sql .= " WHERE ((( jur_parent_id = " . $jur_id  .
                                "       OR ( jur_parent_id IN ( SELECT j2.jur_id FROM jurisdiction j2 WHERE j2.jur_parent_id = ". $jur_id .")))) OR (jur_id = $jur_id))";
                        }
                        if($noAffiliates){
                            $sql.=" and jur_id not in (
                                select aus_jurisdiction_id from affiliate, admin_user
                                where aff_aus_id = aus_id
                            )";
                        }
                        break;
                    case 'nation':
                        if($jur_status == -1){
                            $sql .= "  WHERE ((( jur_status = 1 AND jur_parent_id = " . $jur_id  .  //regions
                                "        OR ( jur_parent_id IN ( SELECT j2.jur_id FROM jurisdiction j2 WHERE j2.jur_status = 1 AND j2.jur_parent_id = ". $jur_id .") AND jur_status = 1) " . //districts
                                "        OR ( jur_parent_id IN ( SELECT j2.jur_id FROM jurisdiction j2 WHERE j2.jur_status = 1 AND j2.jur_parent_id IN ( " .
                                "                                SELECT j3.jur_id FROM jurisdiction j3 WHERE j3.jur_status = 1 AND j3.jur_parent_id =  " . $jur_id . ") ) )" . //clubs
                                "        ) AND jur_status = 1) OR (jur_id = $jur_id))";
                        }else{
                            $sql .= "  WHERE ((( jur_parent_id = " . $jur_id  .  //regions
                                "        OR ( jur_parent_id IN ( SELECT j2.jur_id FROM jurisdiction j2 WHERE j2.jur_parent_id = ". $jur_id .")) " . //districts
                                "        OR ( jur_parent_id IN ( SELECT j2.jur_id FROM jurisdiction j2 WHERE j2.jur_parent_id IN ( " .
                                "                                SELECT j3.jur_id FROM jurisdiction j3 WHERE j3.jur_parent_id =  " . $jur_id . ") ) )" . //clubs
                                "        )) OR (jur_id = $jur_id))";
                        }
                        if($noAffiliates){
                            $sql.=" and jur_id not in (
                                select aus_jurisdiction_id from affiliate, admin_user
                                where aff_aus_id = aus_id
                            )";
                        }
                        break;
                    case 'casino':
                        /*
                         $sql .= "  WHERE ( jur_status = 1 AND jur_parent_id = " . $jur_id  .  //regions
                                 "        OR ( jur_parent_id IN ( SELECT j2.jur_id FROM jurisdiction j2 WHERE j2.jur_status = 1 AND j2.jur_parent_id = ". $jur_id .") AND jur_status = 1) " . //districts
                                 "        OR ( jur_parent_id IN ( SELECT j2.jur_id FROM jurisdiction j2 WHERE j2.jur_status = 1 AND j2.jur_parent_id IN ( " .
                                 "                                SELECT j3.jur_id FROM jurisdiction j3 WHERE j3.jur_status = 1 AND j3.jur_parent_id =  " . $jur_id . ") ) )" . //clubs
                                 "        OR ( jur_parent_id IN ( SELECT j2.jur_id FROM jurisdiction j2 WHERE j2.jur_status = 1 AND j2.jur_parent_id IN ( " .
                                 "                                SELECT j3.jur_id FROM jurisdiction j3 WHERE j3.jur_status = 1 AND j3.jur_parent_id IN (
                                                                  SELECT j4.jur_id FROM jurisdiction j4 WHERE j4.jur_status = 1 AND j4.jur_parent_id = " . $jur_id . ") ) ) )" . //clubs
                                 "        ) AND jur_status = 1";
                                 */
                        if($jur_status == -1){
                            $sql .= " WHERE jur_status = 1 ";
                        }
                        if($noAffiliates){
                            $sql.=" and jur_id not in (
                                select aus_jurisdiction_id from affiliate, admin_user
                                where aff_aus_id = aus_id
                            )";
                        }
                        break;
                    case 'support':
                        break;
                    default:
                        $sql .= " WHERE FALSE ";
                }

                $sql .= " ";
                $sql .= ' ORDER by LOWER(jur_name)';

                $rs   = $this->dbh->exec($sql);
                $num_rows = $rs->getNumRows();

                $cached_list = array();

                if ( $num_rows > 0 ) {
                    while ($row= $rs->next()) {
                        $class    = $row['jur_class'];
                        if(!is_array($cached_list[$class])){
                            $cached_list[$class] = array();
                        }
                        array_push($cached_list[$class], array('id' => $row['jur_id'],'code' => $row["jur_code"], 'vat_code' => $row["jur_vat_code"], 'class' => $row['jur_class'], 'name'=> $row['jur_name'], 'parent_id' => $row['jur_parent_id'], 'date' => strtotime($row['jur_creation_date']), 'jur_status' => $row['jur_status']));
                    }
                }

          /*      if(!$this->saveJurList($jur_id, $cached_list)){
                    error_log("Impossible to save cached jurisdictions for jurisdiction $jur_id");
                }*/


            return $cached_list;
        }
    }
}
?>