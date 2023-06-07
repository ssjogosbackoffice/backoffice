<?php
require_once 'Error.class.inc';

/**
 * Class Jurisdiction
 */
class Jurisdiction
{
    public $id;
    public $class;
    public $parentId;
    public $parentClass;
    public $jurisdictionClasses = array('casino', 'nation', 'region', 'district', 'club');
    public $name, $code, $address1, $address2,  $postc, $phone, $notes, $has_live, $currency, $vatCode = null, $country,
           $city, $accessType, $longitude, $latitude, $startHours, $endHours;
    public $skinId, $UTC;
    public $status;
    public $available_credit,$overdraft,$cash_in_hand,$total_overdraft_received,$overdraft_start_time,$reserved_fund,$bonus;


    function __construct($id=false,$class=false)
    {

        if($id){
            $this->id=$id;
            if($class){
                $this->class=$class;

            }
            else{
                $this->class=$_SESSION['jurisdiction_class'];
            }

            $this->getJurisdictionInfo();
        }
    }


    /**
     * @param Jurisdiction $otherJurisdiction
     * @return bool
     */
    public function getAllEntitiesBetween(Jurisdiction $otherJurisdiction){
        global $dbh;

        switch ($this->getClass())
        {
            case 'casino':
                $params =" ca.jur_id as casino,n.jur_id as nation,r.jur_id as region,d.jur_id as district,c.jur_id as club ";
                $from = " FROM jurisdiction ca
                        LEFT JOIN jurisdiction n on ca.jur_id = n.jur_parent_id
                        LEFT JOIN jurisdiction r ON n.jur_id = r.jur_parent_id
                        LEFT JOIN jurisdiction d on r.jur_id = d.jur_parent_id
                        LEFT JOIN jurisdiction c on d .jur_id =c.jur_parent_id  ";
                break;
            case 'nation':
                $params =" n.jur_id as nation,r.jur_id as region,d.jur_id as district,c.jur_id as club ";
                $from = "FROM
                        jurisdiction n
                        LEFT JOIN jurisdiction r ON n.jur_id = r.jur_parent_id
                        LEFT JOIN jurisdiction d on r.jur_id = d.jur_parent_id
                        LEFT JOIN jurisdiction c on d .jur_id =c.jur_parent_id  ";
                break;
            case 'region':
                $params =" r.jur_id as region,d.jur_id as district,c.jur_id as club ";
                $from = " FROM
                        jurisdiction r
                        LEFT JOIN jurisdiction d on r.jur_id = d.jur_parent_id
                        LEFT JOIN jurisdiction c on d .jur_id =c.jur_parent_id  ";
                break;
            case 'district':
                $params =" d.jur_id as district,c.jur_id as club ";
                $from = " FROM  jurisdiction d
                        LEFT JOIN jurisdiction c on d .jur_id =c.jur_parent_id  ";
                break;
            default:
                $params =" c.jur_id as club ";
                $from = " FROM jurisdiction c";
        }



        $params=explode(',',$params);
        switch ($otherJurisdiction->getClass() ){
            case 'nation':
                $params=array_slice($params,0,count($params)-3);
                $where=" Where n.jur_id=".$otherJurisdiction->getId();
                $group=" Group by n.jur_id";
                break;
            case 'region':
                $params=array_slice($params,0,count($params)-2);
                $where=" Where r.jur_id=".$otherJurisdiction->getId();
                $group=" Group by r.jur_id";
                break;
            case 'district':
                $params=array_slice($params,0,count($params)-1);
                $where=" Where d.jur_id=".$otherJurisdiction->getId();
                $group=" Group by d.jur_id";
                break;
            default:
                $where=" Where c.jur_id=".$otherJurisdiction->getId();
                $group=" Group by c.jur_id";

        }

        $params=implode(',',$params);
        $sql ="SELECT $params $from $where $group";
        $result=$dbh->queryRow($sql);
        return $result;
    }



    /**
     * @return mixed
     */
    public function getStatus()
    {
        return $this->status;
    }

    /**
     * @param mixed $status
     */
    public function setStatus($status)
    {
        $this->status = $status;
    }

    /**
     * @return mixed
     */
    public function getAvailableCredit()
    {
        return $this->available_credit;
    }

    /**
     * @param mixed $available_credit
     */
    public function setAvailableCredit($available_credit)
    {
        $this->available_credit = $available_credit;
    }

    /**
     * @return mixed
     */
    public function getOverdraft()
    {
        return $this->overdraft;
    }

    /**
     * @param mixed $overdraft
     */
    public function setOverdraft($overdraft)
    {
        $this->overdraft = $overdraft;
    }

    protected function increaseOverdraft($amount){
     $this->overdraft+=$amount;
    }

    /**
     * @return mixed
     */
    public function getCashInHand()
    {
        return $this->cash_in_hand;
    }

    /**
     * @param mixed $cash_in_hand
     */
    public function setCashInHand($cash_in_hand)
    {
        $this->cash_in_hand = $cash_in_hand;
    }

    /**
     * @return mixed
     */
    public function getTotalOverdraftReceived()
    {
        return $this->total_overdraft_received;
    }

    /**
     * @param mixed $total_overdraft_received
     */
    public function setTotalOverdraftReceived($total_overdraft_received)
    {
        $this->total_overdraft_received = $total_overdraft_received;
    }

    /**
     * @return mixed
     */
    public function getOverdraftStartTime()
    {
        return $this->overdraft_start_time;
    }

    /**
     * @param mixed $overdraft_start_time
     */
    public function setOverdraftStartTime($overdraft_start_time)
    {
        $this->overdraft_start_time = $overdraft_start_time;
    }

    /**
     * @return mixed
     */
    public function getReservedFund()
    {
        return $this->reserved_fund;
    }

    /**
     * @param mixed $reserved_fund
     */
    public function setReservedFund($reserved_fund)
    {
        $this->reserved_fund = $reserved_fund;
    }

    /**
     * @return mixed
     */
    public function getBonus()
    {
        return $this->bonus;
    }

    /**
     * @param mixed $bonus
     */
    public function setBonus($bonus)
    {
        $this->bonus = $bonus;
    }




    /**
     * @return mixed
     */
    public function getId()
    {
        return $this->id;
    }

    /**
     * @param mixed $id
     */
    public function setId($id)
    {
        $this->id = $id;
    }

    /**
     * @return mixed
     */
    public function getClass()
    {
        return $this->class;
    }

    /**
     * @param mixed $class
     */
    public function setClass($class)
    {
        $this->class = $class;
    }

    /**
     * @return mixed
     */
    public function getParentId()
    {
        return $this->parentId;
    }

    /**
     * @param mixed $parentId
     */
    public function setParentId($parentId)
    {
        $this->parentId = $parentId;
    }

    /**
     * @return mixed
     */
    public function getSkinId()
    {
        return $this->skinId;
    }

    /**
     * @param mixed $skinId
     */
    public function setSkinId($skinId)
    {
        $this->skinId = $skinId;
    }

    /**
     * @return mixed
     */
    public function getUTC()
    {
        return $this->UTC;
    }

    /**
     * @param mixed $UTC
     */
    public function setUTC($UTC)
    {
        $this->UTC = $UTC;
    }



    /**
     * @return array
     */
    public function getJurisdictionClasses()
    {
        return $this->jurisdictionClasses;
    }

    /**
     * @param array $jurisdictionClasses
     */
    public function setJurisdictionClasses($jurisdictionClasses)
    {
        $this->jurisdictionClasses = $jurisdictionClasses;
    }

    /**
     * @return mixed
     */
    public function getName()
    {
        return $this->name;
    }

    /**
     * @param mixed $name
     */
    public function setName($name)
    {
        $this->name = $name;
    }

    /**
     * @return mixed
     */
    public function getCode()
    {
        return $this->code;
    }

    /**
     * @param mixed $code
     */
    public function setCode($code)
    {
        $this->code = $code;
    }

    /**
     * @return mixed
     */
    public function getAddress1()
    {
        return $this->address1;
    }

    /**
     * @param mixed $address1
     */
    public function setAddress1($address1)
    {
        $this->address1 = $address1;
    }

    /**
     * @return mixed
     */
    public function getAddress2()
    {
        return $this->address2;
    }

    /**
     * @param mixed $address2
     */
    public function setAddress2($address2)
    {
        $this->address2 = $address2;
    }

    /**
     * @return mixed
     */
    public function getPostc()
    {
        return $this->postc;
    }

    /**
     * @param mixed $postc
     */
    public function setPostc($postc)
    {
        $this->postc = $postc;
    }

    /**
     * @return mixed
     */
    public function getPhone()
    {
        return $this->phone;
    }

    /**
     * @param mixed $phone
     */
    public function setPhone($phone)
    {
        $this->phone = $phone;
    }

    /**
     * @return mixed
     */
    public function getNotes()
    {
        return $this->notes;
    }

    /**
     * @param mixed $notes
     */
    public function setNotes($notes)
    {
        $this->notes = $notes;
    }

    /**
     * @return mixed
     */
    public function getHasLive()
    {
        return $this->has_live;
    }

    /**
     * @param mixed $has_live
     */
    public function setHasLive($has_live)
    {
        $this->has_live = $has_live;
    }

    /**
     * @return mixed
     */
    public function getCurrency()
    {
        return $this->currency;
    }

    /**
     * @param mixed $currency
     */
    public function setCurrency($currency)
    {
        $this->currency = $currency;
    }

    /**
     * @return null
     */
    public function getVatCode()
    {
        return $this->vatCode;
    }

    /**
     * @param null $vatCode
     */
    public function setVatCode($vatCode)
    {
        $this->vatCode = $vatCode;
    }

    /**
     * @return mixed
     */
    public function getCountry()
    {
        return $this->country;
    }

    /**
     * @param mixed $country
     */
    public function setCountry($country)
    {
        $this->country = $country;
    }

    /**
     * @return mixed
     */
    public function getCity()
    {
        return $this->city;
    }

    /**
     * @param mixed $city
     */
    public function setCity($city)
    {
        $this->city = $city;
    }

    /**
     * @return mixed
     */
    public function getAccessType()
    {
        return $this->accessType;
    }

    /**
     * @param mixed $accessType
     */
    public function setAccessType($accessType)
    {
        $this->accessType = $accessType;
    }

    /**
     * @return mixed
     */
    public function getLongitude()
    {
        return $this->longitude;
    }

    /**
     * @param mixed $longitude
     */
    public function setLongitude($longitude)
    {
        $this->longitude = $longitude;
    }

    /**
     * @return mixed
     */
    public function getLatitude()
    {
        return $this->latitude;
    }

    /**
     * @param mixed $latitude
     */
    public function setLatitude($latitude)
    {
        $this->latitude = $latitude;
    }

    /**
     * @return mixed
     */
    public function getStartHours()
    {
        return $this->startHours;
    }

    /**
     * @param mixed $startHours
     */
    public function setStartHours($startHours)
    {
        $this->startHours = $startHours;
    }

    /**
     * @return mixed
     */
    public function getEndHours()
    {
        return $this->endHours;
    }

    /**
     * @param mixed $endHours
     */
    public function setEndHours($endHours)
    {
        $this->endHours = $endHours;
    }


    /**
     * @return mixed
     */
    public function getParentClass()
    {
        return $this->jurisdictionClasses[array_search($this->class, $this->jurisdictionClasses) - 1];
    }

    /**
     * @param $class
     */
    public function setParentClass($class)
    {
        $this->parentClass=$class;
    }

    /**
     * @return array
     */
    public function getParentsClasses()
    {
        return array_slice($this->jurisdictionClasses, -array_search($this->class, $this->jurisdictionClasses));
    }


    /**
     * @return bool
     */
    public function getJurisdictionInfo()
    {
        global $dbh;
        $sql = "Select ".$this->getClass().".* from jurisdiction ".$this->getClass();
        $sql .= self::checkItBelongsToCurrentJurisdiction();
        $sql.= " AND ".$this->getClass().".jur_id=" . $dbh->escape($this->id);
        $jurisdiction=$dbh->queryRow($sql);
        if($jurisdiction) {
            $this->setClass($jurisdiction['jur_class']);
            $this->setParentId($jurisdiction['jur_parent_id']);
            $this->setName($jurisdiction['jur_name']);
            $this->setCode($jurisdiction['jur_code']);
            $this->setAddress1($jurisdiction['jur_address1']);
            $this->setAddress2($jurisdiction['jur_address2']);
            $this->setPostc($jurisdiction['jur_postcode_zip']);
            $this->setCountry($jurisdiction['jur_cou_id']);
            $this->setCity($jurisdiction['jur_city']);
            $this->setNotes($jurisdiction['jur_notes']);
            $this->setCurrency($jurisdiction['jur_currency']);
            $this->setAccessType($jurisdiction['jur_access_type']);
            $this->setSkinId($jurisdiction['jur_skn_id']);
            $this->setUTC($jurisdiction['jur_time_utc']);
            $this->setStatus($jurisdiction['jur_status']);
            $this->setAvailableCredit($jurisdiction['jur_available_credit']);
            $this->setOverdraft($jurisdiction['jur_overdraft']);
            $this->setCashInHand($jurisdiction['jur_cash_in_hand']);
            $this->setTotalOverdraftReceived($jurisdiction['jur_tot_overdraft_received']);
            $this->setOverdraftStartTime($jurisdiction['jur_overdraft_start_time']);
            $this->setReservedFund($jurisdiction['jur_reserved_fund']);
            $this->setBonus($jurisdiction['jur_bonus']);
            return true;
        }
        return false;
    }


    /**
     * @return string
     */
    private function checkItBelongsToCurrentJurisdiction()
    {
        $jur_id = $_SESSION["jurisdiction_id"];
        $return = '';

        switch ($this->class) {
            case "club":
                $return .= "
  		        JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
                JOIN jurisdiction region ON region.jur_id = district.jur_parent_id
                JOIN jurisdiction nation   ON nation.jur_id   = region.jur_parent_id";
                break;
            case "district":
                $return .= "
                JOIN jurisdiction region ON region.jur_id = district.jur_parent_id
                JOIN jurisdiction nation   ON nation.jur_id   = region.jur_parent_id";
                break;
            case "region":
                $return .= "
                JOIN jurisdiction nation   ON nation.jur_id   = region.jur_parent_id";
                break;
            default : {
                $return .= " ";
            }
                break;
        }

        switch ($_SESSION["jurisdiction_class"]) {
            case "club":
                $return .= " WHERE club.jur_id = $jur_id";
                break;
            case "district":
                $return .= " WHERE district.jur_id = $jur_id";
                break;
            case "region":
                $return .= " WHERE region.jur_id = $jur_id";
                break;
            case "nation":
                $return .= " WHERE nation.jur_id = $jur_id";
                break;

            case "casino":
                $return .= " WHERE 1=1";
                break;
        }
        return $return;
    }

    /**
     * @return bool|string`
     */
    public function createJurisdiction($autocommit=true)
    {
        global $dbh, $lang,$dbLog;
        if (isBlank($this->name)) {
            addError("", $lang->getLang(Error::$errors['-10001']) );
        }
        $this->name = trim($this->name);
        if (isBlank($this->country)) {
            addError("",$lang->getLang(Error::$errors['-110']));
        } else {
            $this->country = getCountry($this->country);
        }

        if (isBlank($this->city)) {
            addError("", $lang->getLang(Error::$errors['-111']));
        } else {
            $this->city = explode(',', $this->city);
            $this->city = $this->city[0];
        }
        if (isBlank($this->class)) {
            addError("", $lang->getLang(Error::$errors['-10002']));
        } else {
            if ($this->checkJurisdictionAlreadyExistsByName()) {
                addError('',$lang->getLang(Error::$errors['-152']));
            }
        }
        if (isBlank($this->code)) {
            addError("", $lang->getLang(Error::$errors['-81']));
        } elseif (mb_strlen($this->code) != 6 || !mb_ereg_match("^[a-zA-Z]*$", $this->code)) {
            addError("", $lang->getLang(Error::$errors['-81']));
        } else {
            if ($this->checkJurisdictionAlreadyExistsByCode()) {
                addError("",$lang->getLang(Error::$errors['-81']));
            }
        }
        if (isBlank($this->address1))
            addError("",$lang->getLang(Error::$errors['-10003']));

        if (isBlank($this->parentId)) {
            addError("", $lang->getLang(Error::$errors['-10004']));
        } else {
            $this->parentId = (int)$this->parentId;


            if (!$this->isValidParent()) {
                addError('',$lang->getLang(Error::$errors['-10004']));
            }
        }
        if (isBlank($this->postc))
            addError("",$lang->getLang(Error::$errors['-112']));

        if (isBlank($this->startHours))
            $this->startHours = "00:00";

        if (isBlank($this->endHours))
            $this->endHours = "00:00";

        $hours = $this->startHours . "-" . $this->endHours;
        if ( $this->latitude & $this->longitude) {
            $localization = $this->latitude . ';' . $this->longitude;
        } else {
            $localization = '';
        }
        $this->has_live = (($this->has_live == true) ? "1" : "0");


        if (!$this->checkParentPercentages()) {
            addError("percetage", $lang->getLang(Error::$errors['-10005']));
        }
        require_once "funclib/percentual.inc.php";
        if (!checkPercentualsCasino()) {
            addError("", $lang->getLang(Error::$errors['-10006']));
        }
        if (!checkPercentualsFields()) {
            addError("", $lang->getLang(Error::$errors['-10006']));
        }
        if (!areErrors()) {
            $next_id = $dbh->nextSequence('JUR_ID_SEQ');
            if($autocommit) {
                $dbh->begin();
            }
            $this->currency = $this->getParentCurrency();
            if (!is_numeric($this->currency) || $this->currency == '') {
                $this->currency = 1;
            }
            $this->skinId = $this->getParentSkin();
            if (!is_numeric($this->skinId)) {
                $this->skinId = 'NULL';
            }
            $this->UTC = $this->getParentUTC();

            $sql = "INSERT INTO jurisdiction (
                      jur_id,
                      jur_name,
                      jur_code,
                      jur_class,
                      jur_address1,
                      jur_address2,
                      jur_postcode_zip,
                      jur_phone,
                      jur_notes,
                      jur_parent_id,
                      jur_has_live_games,
                      jur_currency,
                      jur_vat_code,
                      jur_cou_id,
                      jur_city,
                      jur_skn_id,
                      jur_time_utc,
                      jur_access_type,
                      jur_google_map_info,
                      jur_agency_hour)" .
                " VALUES (
                      " . $next_id . ",
                      " . $dbh->prepareString($this->name) . ",
                      " . $dbh->prepareString($this->code) . ",
                      " . $dbh->prepareString($this->class) . ",
                      " . $dbh->prepareString($this->address1) . ",
                      " . $dbh->prepareString($this->address2) . ",
                      " . $dbh->prepareString($this->postc) . ",
                      " . $dbh->prepareString($this->phone) . ",
                      " . $dbh->prepareString($this->notes) . ",
                      " . $dbh->escape($this->parentId) . ",
                      " . $dbh->escape($this->has_live) . ",
                      " . $dbh->prepareString($this->currency) . ",
                      " . $dbh->prepareString($this->vatCode) . ",
                      " . $dbh->prepareString($this->country) . ",
                      " . $dbh->prepareString($this->city) . ",
                      " . $dbh->prepareString($this->skinId) . ",
                      " . $dbh->prepareString($this->UTC) . ",
                      " . $dbh->prepareString(strtoupper($this->accessType)) . ",
                      " . $dbh->prepareString($localization) . ",
                      " . $dbh->prepareString($hours) .
                ")";
            $ret = $dbh->exec($sql);
            $casino_db_trans = false;
            if (!$ret) {
                addError("", $lang->getLang('Database Error - Could not create jurisdiction record'));
                $casino_db_trans = false;
            } else {
                $perc_res = savePercentuals($_POST["percentuals"], $next_id, $this->parentId, $_POST["betting"], $_POST['betting_live']);
                if ($perc_res) {
                    $casino_db_trans = true;
                } else {
                    $casino_db_trans = false;
                    addError("", $lang->getLang('Database Error - Could not create percentage jurisdiction record'));
                }
            }
            if ($casino_db_trans) {
                if($autocommit) {
                    $dbh->commit();
                }
                return $next_id;
            } else {
                if($autocommit) {
                    $dbh->rollback();
                }
                return FALSE;
            }
        } else {
            $dbLog->info(getErrors());
            return false;
        }
    }

    /**
     * @return bool
     */
    private function checkJurisdictionAlreadyExistsByName()
    {
        global $dbh;
        $sql = "SELECT count(*) FROM jurisdiction WHERE jur_name = " . $dbh->prepareString($this->name) . " AND jur_name != 'internet'";
        $count = $dbh->countQuery($sql);
        return $count > 0;
    }

    /**
     * @return bool
     */
    private function checkJurisdictionAlreadyExistsByCode()
    {
        global $dbh;
        $sql = "SELECT count(*) FROM jurisdiction WHERE jur_code = " . $dbh->prepareString(strtolower($this->code));
        $count = $dbh->countQuery($sql);
        return $count > 0;
    }

    /**
     * @return bool
     */
    private function isValidParent()
    {
        global $dbh;
        $sql = "SELECT count(*) FROM jurisdiction WHERE jur_id = " . $dbh->escape($this->parentId) . " AND jur_class = '$this->parentClass'";
        $valid_district_count = $dbh->countQuery($sql);
        return $valid_district_count == "1";
    }

    /**
     * @return bool
     */
    private function checkParentPercentages()
    {
        global $dbh;
        $sql = "SELECT * FROM jurisdiction_payment_settings WHERE JPS_JUR_ID='" . $dbh->escape($this->parentId) . "' ";
        $parentResult = $dbh->queryRow($sql);
        return $parentResult;
    }

    /**
     * @return mixed
     */
    public function getParentCurrency()
    {
        global $dbh;
        return $dbh->queryOne('SELECT jur_currency from jurisdiction where jur_id=' . $dbh->escape($this->parentId));
    }

    /**
     * @return mixed
     */
    public function getParentSkin()
    {
        global $dbh;
        return $dbh->queryOne('SELECT jur_skn_id from jurisdiction where jur_id=' . $dbh->escape($this->parentId));
    }

    /**
     * @return mixed
     */
    private function getParentUTC()
    {
        global $dbh;
        return $dbh->queryOne('SELECT jur_time_utc from jurisdiction where jur_id=' . $dbh->escape($this->parentId));
    }

}