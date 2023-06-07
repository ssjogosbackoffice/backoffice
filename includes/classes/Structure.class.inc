<?php


class Structure{
    public $id,$code,$name,$percentualsCasino,$percentualsBetting,$gamesInfo,$menuInfo,$structureConfiguration,$bettingProfile,$bettingLiveProfile,$address,$zip,$phone,$notes,$haslive,$longitute,$latitude,$startHours,$endHours,$level,$parentId,$parentLevel,$children,$UTC,$country,$city,$skinId,$currency;
    private $accessType='backoffice';
    public $vatCode=null;

    function __construct($id=null,$level=null)
    {
        if($id){
            $this->setId($id);
        }
        if($level){
            $this->setLevel($level);
        }
    }

    public function saveStructure(){
        global $dbh , $lang;
        $this->name=trim($this->name);
        if (!$this->name) {
            return [-1,$lang->getLang("A Nation name must be entered")];
        }
        elseif($this->structureNameExists($this->name)) {
            return [-1, $lang->getLang("The structure name you entered already exists")];
        }
        if ( isBlank($this->country) ) {
            return [-1,$lang->getLang("Please select a country")];
        }
        else{
            $this->country=getCountry($this->country);
        }
        if ( isBlank($this->city) ) {
            return [-1,$lang->getLang("Please enter a city")];
        }
        else{
            $this->city=explode(',',$this->city);
            $this->city=$this->city[0];
        }
        if (isBlank($this->code)) {
            return [-1,$lang->getLang("An account code must be entered")];
        }
        elseif (mb_strlen($this->code) != 6 || !mb_ereg_match("^[A-Za-z]*$", $this->code)) {
            return [-1,$lang->getLang("The account code has to contain only 6 alphabetical characters")];
        } elseif($this->structureCodeExists($this->code)) {
            return [-1,$lang->getLang("The account code you entered already exists for another jurisdiction.")];
        }
        if(!$this->haslive){
            $this->haslive=true;
        }
        if($this->level=='club'){
            if ( isBlank($this->zip) )
            return [-1,$lang->getLang("Blank Postcode, please insert one")];

            if ( isBlank($this->startHours) )
                $this->startHours="00:00";

            if ( isBlank($this->endHours) )
                $this->endHours="00-00";

            $hours=$this->startHours."-".$this->endHours;
            if($this->latitude && $this->longitute){
                $localization=$this->latitude.';'.$this->longitute;
            }else{
                $localization='';
            }
            $this->haslive = (($this->haslive == true) ? "1" : "0");
        }
        if($this->level!='nation'){
            $parentJurisdictionPaymentSettings=$this->getJurisdictionPaymentSettings($this->parentId);
            if(!$parentJurisdictionPaymentSettings){
                return [-1,$lang->getLang("You have to set the percentage of the district before to go forward")];
            }
            if(!$this->currency) {
                $this->currency=$this->getJurisdictionCurrencyId($this->parentId);
                if (!is_numeric($this->currency) || $this->currency == '') {
                    $this->currency = 1;
                }
            }
            if(!$this->skinId){
                $this->setSkinId($this->getJurisdictionSkinId($this->parentId));
                if(!is_numeric($this->setSkinId)){
                    $this->setSkinId('NULL');
                }
            }
            if(!$this->UTC){
                $this->setUTC($this->getJurisdictionUTC($this->parentId));

            }

        }else{
            $this->parentId=1;
        }

        if(!$this->checkPercentualsCasino()){
            return [-1,$lang->getLang("You have to set the percentage fields to save the new %", $this->getLevel())];
        }


        if(!$this->checkPercentualsBetting()){
            return [-1,$lang->getLang("You have to set the percentage fields to save the new %", $this->getLevel())];

        }

        if (!areErrors()) {
            $dbh->begin();
            if(!$this->id){
                $this->setId($dbh->nextSequence('JUR_ID_SEQ'));
            }

            $sql = "INSERT INTO jurisdiction 
                                           (jur_id, 
                                            jur_name, 
                                            jur_code, 
                                            jur_class, 
                                            jur_address1,
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
                " VALUES (" . $this->id . ", 
                " . $dbh->prepareString($this->name) . ", 
                " . $dbh->prepareString($this->code) . ", 
                " . $dbh->prepareString($this->level)  .",
                " . $dbh->prepareString($this->address) . ", 
                " . $dbh->prepareString($this->zip) . ", 
                " . $dbh->prepareString($this->phone) .", 
                " . $dbh->prepareString($this->notes) . ", 
                " . $this->parentId . ", 
                " . $this->haslive . ",
                $this->currency, 
                '$this->vatCode',
                '$this->country',
                '$this->city',
                $this->skinId,
                $this->UTC,
                ".$dbh->prepareString(strtoupper($this->accessType)).",
                ".$dbh->prepareString($localization).",
                ".$dbh->prepareString($hours).")";
            if($dbh->exec($sql)){
                $perc_res = $this->savePercentuals($this->percentualsCasino, $this->id, $this->parentId,$this->bettingProfile,$this->bettingLiveProfile);
                if($perc_res){
                    if($this->saveStructureConfiguration()){
                      //  $dbh->commit();
                        return [1,$this->id];
                    }
                    else{

                        $dbh->rollback();
                        return [-1,''];
                    }
                }
                else{

                    $dbh->rollback();
                    return [-1,'Database Error - Could not create percentage jurisdiction record'];
                }
            }
            else{
                $dbh->rollback();
                return [-1,'Database Error - Could not create jurisdiction record'];
            }
        }

    }


    public function updateStructure(){
        global $dbh,$lang;
        if ( isBlank($this->name) ) {
            addError("", $lang->getLang("Please enter a valid name"));
        }
        if ( isBlank($this->country) ) {
            addError("", $lang->getLang("Please select a country"));
        }
        else{
            $this->country=getCountry($this->country);
        }

        if ( isBlank($this->city) ) {
            addError("", $lang->getLang("Please enter a city"));
        }
        else{
            $this->city=explode(',',$this->city);
            $this->city=$this->city[0];
        }
        $structureNameExistst=$this->structureNameExists($this->name,$this->id);
        if($structureNameExistst && $structureNameExistst!=$this->getId()){
            addError('',$lang->getLang('The name you entered already exists'));
        }

        if ( isBlank($this->address) )
            addError("", $lang->getLang("An address (line 1) must be entered"));
        if ( isBlank($this->parentId) ) {
            addError("", $lang->getLang("Invalid parent id"));
        }
        if($this->level=='club') {
            if (isBlank($this->zip))
                addError("", $lang->getLang("Blank Postcode, please insert one"));

            if (isBlank($this->phone))
                addError("", $lang->getLang('A phone number must be entered'));
        }
        $this->haslive = (($this->haslive == true) ? "1" : "0");
        if ( isBlank($this->startHours) )
            $this->startHours="00:00";

        if ( isBlank($this->endHours) )
            $this->endHours="00-00";

        $hours=$this->startHours."-".$this->endHours;
        if($this->latitude && $this->longitute){
            $localization=$this->latitude.';'.$this->longitute;
        }else{
            $localization='';
        }

        if($this->getLevel()!='nation') {
            $parentJurisdictionPaymentSettings = $this->getJurisdictionPaymentSettings($this->parentId);
            if (!$parentJurisdictionPaymentSettings) {
                addError("percetage", $lang->getLang("You have to set the percentage of the district before to go forward"));
            }

            if(!$this->currency) {
                $this->currency=$this->getJurisdictionCurrencyId($this->parentId);
                if (!is_numeric($this->currency) || $this->currency == '') {
                    $this->currency = 1;
                }
            }
            if(!$this->skinId){
                $this->setSkinId($this->getJurisdictionSkinId($this->parentId));
                if(!is_numeric($this->setSkinId)){
                    $this->setSkinId('NULL');
                }
            }
            if(!$this->UTC){
                $this->setUTC($this->getJurisdictionUTC($this->parentId));

            }
        }
        if (!$this->checkPercentualsCasino()) {
            addError("percetage", $lang->getLang("You have to set the percentage fields to save the new %", $this->getLevel()));
        }

        if(!$this->checkPercentualsBetting()){
            addError("percetage", $lang->getLang("You have to set the percentage fields to save the new %", $this->getLevel()));
        }
        if ( ! areErrors() ){
            $dbh->begin();


            $sql = "UPDATE jurisdiction" .
                " SET jur_name  = " . $dbh->prepareString($this->name) .
                ", jur_address1 = " . $dbh->prepareString($this->address) .
                ", jur_parent_id = " . $this->parentId .
                ", jur_postcode_zip = " . $dbh->prepareString($this->zip) .
                ", jur_phone = " . $dbh->prepareString($this->phone) .
                ", jur_notes = " .$dbh->prepareString($this->notes) .
                ", jur_has_live_games = " . $dbh->prepareString($this->haslive) .
                ", jur_currency =".$dbh->prepareString($this->currency).
                ", jur_cou_id = " . $dbh->prepareString($this->country) .
                ", jur_city =".$dbh->prepareString($this->city).
                ", jur_skn_id =".$dbh->prepareString($this->skinId).
                ", jur_time_utc =".$dbh->prepareString($this->UTC).
                ", jur_access_type =".$dbh->prepareString(strtoupper($this->accessType)).
                ", jur_agency_hour =".$dbh->prepareString(strtoupper($hours)).
                ", jur_google_map_info =".$dbh->prepareString(strtoupper($localization)).
                " WHERE jur_id = " . $this->id;
            $ret=$dbh->exec($sql);
            $casino_db_trans = false;
            $update=array('jur_currency'=>intval($this->getCurrency()),
                'jur_time_utc'=>intval($this->getUTC()),
                'jur_skn_id'=>  intval($this->getSkinId())
            );

            updateAllHierarchy($this->id,'club',$update);
            updateAllHierarchy($this->id,'club',false,'pun_site_id',$this->skinId);
            if ( !$ret ) {
                addError("", $lang->getLang('Database Error - Could not create jurisdiction record'));
                $casino_db_trans = false;
            }else{
                $perc_res = $this->savePercentuals($this->percentualsCasino, $this->id, $this->parentId,$this->bettingProfile,$this->bettingLiveProfile);
                if($perc_res){
                    $dbh->commit();
                    return true;
                }else{

                    addError("", $lang->getLang('Database Error - Could not create percentage jurisdiction record'));
                    $dbh->rollback();
                    return FALSE;
                }
            }

        }
        return FALSE;

    }


    public function saveStructureConfiguration(){
        global $dbh;
        if($this->level!='nation') {
            return $dbh->exec('INSERT INTO jurisdiction_configuration (SCN_JUR_ID, SCN_GAMES_INFO, SCN_MENU_INFO, SCN_SKIN_PROPERTIES)
                                        SELECT ' . $this->getId() . ', SCN_GAMES_INFO, SCN_MENU_INFO, SCN_SKIN_PROPERTIES 
                                        FROM jurisdiction_configuration
                                        WHERE SCN_JUR_ID = ' . $this->getParentId());
        }
        else{

            $menu_info=file_get_contents('../../config/menu.ini');
            $skin_properties=file_get_contents('../../config/skinsproperties.ini');
            $dbh->exec('SET SESSION group_concat_max_len = 4000000');
            $games_info=$dbh->queryOne("select concat ('[slots]\n\n', 
            coalesce((select GROUP_CONCAT('\tgames[] = ', ggi_button_image_name, '@', ggi_game_id, '@', ggi_width, '@', ggi_height, '@', ggi_pes_group, '@', ggi_has_forfun, '@', ggi_web_name SEPARATOR '\n')
            from game_global_info
            where ggi_category = 'slots' and (ggi_pes_group is null or ggi_pes_group = '0' or ggi_pes_group = '')), ''),
            '\n\n[video poker]\n\n',
            coalesce((select GROUP_CONCAT('\tgames[] = ', ggi_button_image_name, '@', ggi_game_id, '@', ggi_width, '@', ggi_height, '@', ggi_pes_group, '@', ggi_has_forfun, '@', ggi_web_name SEPARATOR '\n')
            from game_global_info
            where ggi_category = 'video poker' and (ggi_pes_group is null or ggi_pes_group = '0' or ggi_pes_group = '')), ''),
            '\n\n[table games]\n\n',
            coalesce((select GROUP_CONCAT('\tgames[] = ', ggi_button_image_name, '@', ggi_game_id, '@', ggi_width, '@', ggi_height, '@', ggi_pes_group, '@', ggi_has_forfun, '@', ggi_web_name SEPARATOR '\n')
            from game_global_info
            where ggi_category = 'table games' and (ggi_pes_group is null or ggi_pes_group = '0' or ggi_pes_group = '')),''),
            '\n\n[live casino]\n\n',
            coalesce((select GROUP_CONCAT('\tgames[] = ', ggi_button_image_name, '@', ggi_game_id, '@', ggi_width, '@', ggi_height, '@', ggi_pes_group, '@', ggi_has_forfun, '@', ggi_web_name SEPARATOR '\n')
            from game_global_info
            where ggi_category = 'live casino' and (ggi_pes_group is null or ggi_pes_group = '0' or ggi_pes_group = '')), ''),
            '\n\n[common draw]\n\n',
            coalesce((select GROUP_CONCAT('\tgames[] = ', ggi_button_image_name, '@', ggi_game_id, '@', ggi_width, '@', ggi_height, '@', ggi_pes_group, '@', ggi_has_forfun, '@', ggi_web_name SEPARATOR '\n')
            from game_global_info
            where ggi_category = 'common draw' and (ggi_pes_group is null or ggi_pes_group = '0' or ggi_pes_group = '')), ''),
            '\n\n[dice games]\n\n',
            coalesce((select GROUP_CONCAT('\tgames[] = ', ggi_button_image_name, '@', ggi_game_id, '@', ggi_width, '@', ggi_height, '@', ggi_pes_group, '@', ggi_has_forfun, '@', ggi_web_name SEPARATOR '\n')
            from game_global_info
            where ggi_category = 'dice games' and (ggi_pes_group is null or ggi_pes_group = '0' or ggi_pes_group = '')), ''),
            '\n\n[virtual]\n\n',
            coalesce((select GROUP_CONCAT('\tgames[] = ', ggi_button_image_name, '@', ggi_game_id, '@', ggi_width, '@', ggi_height, '@', ggi_pes_group, '@', ggi_has_forfun, '@', ggi_web_name SEPARATOR '\n')
            from game_global_info
            where ggi_category = 'virtual' and (ggi_pes_group is null or ggi_pes_group = '0' or ggi_pes_group = '')), '')
            ) as game_info");

            return $dbh->exec('INSERT INTO jurisdiction_configuration (SCN_JUR_ID, SCN_GAMES_INFO, SCN_MENU_INFO, SCN_SKIN_PROPERTIES)
                              VALUES('.$this->id.','.$dbh->prepareString($games_info).','.$dbh->prepareString($menu_info).','.$dbh->prepareString($skin_properties).')');


        }
    }

    public function getJurisdictionCurrencyId($jurisdiction_id){
        global $dbh;
        return $dbh->queryOne('SELECT jur_currency from jurisdiction where jur_id='.$dbh->escape($jurisdiction_id));
    }

    public function getJurisdictionSkinId($jurisdiction_id){
        global $dbh;
        return $dbh->queryOne('SELECT jur_skn_id from jurisdiction where jur_id='.$dbh->escape($jurisdiction_id));
    }
    public function getJurisdictionUTC($jurisdiction_id){
        global $dbh;
        return $dbh->queryOne('SELECT jur_time_utc from jurisdiction where jur_id='.$dbh->escape($jurisdiction_id));
    }

    private function  checkPercentualsCasino(){
        foreach($this->percentualsCasino as $key => $value){
            if($value == 0) return false;
        }
        return true;
    }

    private function checkPercentualsBetting(){
        switch($this->bettingProfile){
            case 1:
                $commision = ($_POST["commisionbanco"]?$_POST["commisionbanco"]:50);
                if($commision == "") return false;
                break;
            case 3:
                $commision = $_POST["commisionbancoplus"];
                if($commision == "") return false;
                break;
            case 2:
                $commision = $_POST["commissions"];
                foreach($commision as $key => $value){
                    if($value == "") return false;
                }
                break;
            case 4:
                $bonusM = $_POST["bonusmensile"];
                if($bonusM == "") return false;
                $commision = $_POST["monthly"];
                foreach($commision as $key => $value){
                    if($value == "") return false;
                }
                break;
            case 9:
                $commision = $_POST["extraprofit"];
                if($commision == "") return false;
                break;
        }
        return true;
    }

    public function getJurisdictionPaymentSettings($jurisdiction_id){
        global $dbh;
        $sql="SELECT * FROM jurisdiction_payment_settings WHERE JPS_JUR_ID= ".$dbh->escape($jurisdiction_id);
        return $dbh->queryRow($sql);

    }

    public function structureNameExists($name,$returnId=false){
        global $dbh;
        $sql = "SELECT jur_id FROM jurisdiction WHERE LOWER(jur_name) = LOWER(".$dbh->prepareString($name).")";
        $count = $dbh->queryOne($sql);
        if($returnId){
            return $count;
        }
        return ($count > 0 ? true :false);
    }
    public function structureCodeExists($code){
        global $dbh;
        $sql = "SELECT count(*) FROM jurisdiction WHERE LOWER(jur_code) = " . $dbh->prepareString(strtolower($code));
        $count = $dbh->countQuery($sql);
        return ($count > 0 ? true :false);
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
    public function getAddress()
    {
        return $this->address;
    }

    /**
     * @param mixed $address
     */
    public function setAddress($address)
    {
        $this->address = $address;
    }

    /**
     * @return mixed
     */
    public function getZip()
    {
        return $this->zip;
    }

    /**
     * @param mixed $zip
     */
    public function setZip($zip)
    {
        $this->zip = $zip;
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
    public function getHaslive()
    {
        return $this->haslive;
    }

    /**
     * @param mixed $haslive
     */
    public function setHaslive($haslive)
    {
        $this->haslive = $haslive;
    }

    /**
     * @return mixed
     */
    public function getLongitute()
    {
        return $this->longitute;
    }

    /**
     * @param mixed $longitute
     */
    public function setLongitute($longitute)
    {
        $this->longitute = $longitute;
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
    public function getLevel()
    {
        return $this->level;
    }

    /**
     * @param mixed $level
     */
    public function setLevel($level)
    {
        $this->level = $level;
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
    public function getParentLevel()
    {
        return $this->parentLevel;
    }

    /**
     * @param mixed $parentLevel
     */
    public function setParentLevel($parentLevel)
    {
        $this->parentLevel = $parentLevel;
    }

    /**
     * @return mixed
     */
    public function getChildren()
    {
        return $this->children;
    }

    /**
     * @param mixed $children
     */
    public function setChildren($children)
    {
        $this->children = $children;
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
     * @return string
     */
    public function getAccessType()
    {
        return $this->accessType;
    }

    /**
     * @param string $accessType
     */
    public function setAccessType($accessType)
    {
        $this->accessType = $accessType;
    }

    /**
     * @return mixed
     */
    public function getPercentualsCasino()
    {
        return $this->percentualsCasino;
    }

    /**
     * @param mixed $percentualsCasino
     */
    public function setPercentualsCasino($percentualsCasino)
    {
        $this->percentualsCasino = $percentualsCasino;
    }

    /**
     * @return mixed
     */
    public function getPercentualsBetting()
    {
        return $this->percentualsBetting;
    }

    /**
     * @param mixed $percentualsBetting
     */
    public function setPercentualsBetting($percentualsBetting)
    {
        $this->percentualsBetting = $percentualsBetting;
    }

    /**
     * @return mixed
     */
    public function getGamesInfo()
    {
        return $this->gamesInfo;
    }

    /**
     * @param mixed $gamesInfo
     */
    public function setGamesInfo($gamesInfo)
    {
        $this->gamesInfo = $gamesInfo;
    }

    /**
     * @return mixed
     */
    public function getMenuInfo()
    {
        return $this->menuInfo;
    }

    /**
     * @param mixed $menuInfo
     */
    public function setMenuInfo($menuInfo)
    {
        $this->menuInfo = $menuInfo;
    }

    /**
     * @return mixed
     */
    public function getStructureConfiguration()
    {
        return $this->structureConfiguration;
    }

    /**
     * @param mixed $structureConfiguration
     */
    public function setStructureConfiguration($structureConfiguration)
    {
        $this->structureConfiguration = $structureConfiguration;
    }

    /**
     * @return mixed
     */
    public function getBettingProfile()
    {
        return $this->bettingProfile;
    }

    /**
     * @param mixed $bettingProfile
     */
    public function setBettingProfile($bettingProfile)
    {
        $this->bettingProfile = $bettingProfile;
    }

    /**
     * @return mixed
     */
    public function getBettingLiveProfile()
    {
        return $this->bettingLiveProfile;
    }

    /**
     * @param mixed $bettingLiveProfile
     */
    public function setBettingLiveProfile($bettingLiveProfile)
    {
        $this->bettingLiveProfile = $bettingLiveProfile;
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


    public function savePercentuals($percentuals, $node, $parent,$type,$live_type){
        global $dbh;
        $sql="SELECT * FROM jurisdiction_payment_settings WHERE JPS_JUR_ID=$parent ";
        $parentResult=$dbh->queryRow($sql,true);
        if(!$parentResult)
        {
            return FALSE;
        }
        $sql = "Select count(*) from jurisdiction_payment_settings where jps_jur_id = " . $node;
        $exists = $dbh->queryOne($sql,true);

        $betting_value = self::getBettingString($type);
        $live_betting_value=self::getLiveBettingString($live_type);

        if($exists == 0){
            $sql2= "INSERT INTO jurisdiction_services (jss_jur_id) VALUES ($node);";
            if($dbh->exec($sql2,false, true)){
                $sql="Insert into jurisdiction_payment_settings(JPS_JUR_ID ,JPS_PERC_BETTING,JPS_PERC_LIVE_BETTING, ";
                foreach($percentuals as $key => $value){
                    $sql .= "$key, ";
                }
                $sql = substr($sql , 0, strlen($sql) - 2);
                $sql .= ") VALUES ( $node, '".$betting_value."', '".$live_betting_value."', ";
                foreach($percentuals as $key => $value){
                    $sql .= "$value, ";
                }
                $sql = substr($sql , 0, strlen($sql) - 2);
                $sql .= ")";
            }
            else{
                return false;
            }
        }else{
            $sql = "UPDATE jurisdiction_payment_settings SET JPS_PERC_BETTING = '".$betting_value."',JPS_PERC_LIVE_BETTING = '".$live_betting_value."', ";

            if(!is_null($percentuals)){
                foreach($percentuals as $key => $value){
                    $sql .= "$key = $value, ";
                }
                $sql = substr($sql , 0, strlen($sql) - 2);
            }
            $sql .= " where jps_jur_id = ". $node;
        }
        $ret= $dbh->exec($sql,false,true);

        if($ret){
            $sql="DELETE FROM jurisdiction_betting_comm WHERE jbc_jur_id= $node";
            $result=$dbh->exec($sql,false,true);
            switch($type){
                case 1:
                    $sql = "INSERT INTO jurisdiction_betting_comm (jbc_comm_type,jbc_jur_id,jbc_grouping,jbc_reg_date,jbc_perc) VALUES (1,$node,0,CURRENT_TIMESTAMP,".self::getBettingString($type,true).")";
                    break;
                case 3:
                    $sql = "INSERT INTO jurisdiction_betting_comm (jbc_comm_type,jbc_jur_id,jbc_grouping,jbc_reg_date,jbc_perc) VALUES (3,$node,0,CURRENT_TIMESTAMP,".self::getBettingString($type,true).")";
                    break;
                case 2:
                    $sql = "INSERT INTO jurisdiction_betting_comm (jbc_comm_type,jbc_jur_id,jbc_grouping,jbc_reg_date,jbc_perc) VALUES ";
                    $y = 0;
                    for($i=0; $i < count($_POST["commissions"]); $i++){
                        $y++;
                        $sql .= " (2,$node,". $y .",CURRENT_TIMESTAMP,".$_POST["commissions"][$i]."),";
                    }
                    $sql  = substr($sql, 0 ,strlen($sql)-1);
                    break;
                case 4:
                    $sql = "INSERT INTO jurisdiction_betting_comm (jbc_comm_type,jbc_jur_id,jbc_grouping,jbc_reg_date,jbc_perc) VALUES (4,$node,0,CURRENT_TIMESTAMP,".$_POST["bonusmensile"]."),";
                    $y = 0;
                    for($i=0; $i < count($_POST["monthly"]); $i++){
                        $y++;
                        $sql .= "  (4,$node,".$y.",CURRENT_TIMESTAMP,".$_POST["monthly"][$i]."),";
                    }
                    $sql = substr($sql, 0 ,strlen($sql)-1);
                    break;
                case 9:
                    $sql = "INSERT INTO jurisdiction_betting_comm (jbc_comm_type,jbc_jur_id,jbc_grouping,jbc_reg_date,jbc_perc) VALUES (9,$node,0,CURRENT_TIMESTAMP,".$_POST["extraprofit"].")";
                    break;

                case 11:
                    $sql = "INSERT INTO jurisdiction_betting_comm (jbc_comm_type,jbc_jur_id,jbc_grouping,jbc_reg_date,jbc_perc) VALUES (11,$node,0,CURRENT_TIMESTAMP,".$_POST["feedtax"].")";
                    break;
            }
            $bettingAddResult=$dbh->exec($sql,false, true);
            if(!$bettingAddResult){
                return false;
            }
            elsE{

                switch($live_type){

                    case 5:
                        $sql = "INSERT INTO jurisdiction_betting_comm (jbc_comm_type,jbc_jur_id,jbc_grouping,jbc_reg_date,jbc_perc) VALUES (5,$node,0,CURRENT_TIMESTAMP,".self::getBettingString($type,true).")";
                        break;
                    case 7:
                        $sql = "INSERT INTO jurisdiction_betting_comm (jbc_comm_type,jbc_jur_id,jbc_grouping,jbc_reg_date,jbc_perc) VALUES (7,$node,0,CURRENT_TIMESTAMP,".self::getBettingString($type,true).")";
                        break;
                    case 6:
                        $sql = "INSERT INTO jurisdiction_betting_comm (jbc_comm_type,jbc_jur_id,jbc_grouping,jbc_reg_date,jbc_perc) VALUES ";
                        $y = 0;
                        for($i=0; $i < count($_POST["commissions"]); $i++){
                            $y++;
                            $sql .= " (6,$node,". $y .",CURRENT_TIMESTAMP,".$_POST["commissions_live"][$i]."),";
                        }
                        $sql  = substr($sql, 0 ,strlen($sql)-1);
                        break;
                    case 8:
                        $sql = "INSERT INTO jurisdiction_betting_comm (jbc_comm_type,jbc_jur_id,jbc_grouping,jbc_reg_date,jbc_perc) VALUES (8,$node,0,CURRENT_TIMESTAMP,".$_POST["bonusmensile"]."),";
                        $y = 0;
                        for($i=0; $i < count($_POST["monthly"]); $i++){
                            $y++;
                            $sql .= "  (8,$node,".$y.",CURRENT_TIMESTAMP,".$_POST["monthly_live"][$i]."),";
                        }
                        $sql = substr($sql, 0 ,strlen($sql)-1);
                        break;
                    case 10:
                        $sql = "INSERT INTO jurisdiction_betting_comm (jbc_comm_type,jbc_jur_id,jbc_grouping,jbc_reg_date,jbc_perc) VALUES (10,$node,0,CURRENT_TIMESTAMP,".$_POST["extraprofit"].")";
                        break;
                    case 12:
                        $sql = "INSERT INTO jurisdiction_betting_comm (jbc_comm_type,jbc_jur_id,jbc_grouping,jbc_reg_date,jbc_perc) VALUES (12,$node,0,CURRENT_TIMESTAMP,".$_POST["feedtax_live"].")";
                        break;
                }
                return $dbh->exec($sql,false, true);
            }
        }
        else{
            return false;
        }
    }

    private static function  getBettingString($type,$value=false){
        switch($type){
            case 1:
                $commission = ($_POST["commisionbanco"]?$_POST["commisionbanco"]:50);
                if($value){
                    return $commission;
                }
                return "type:1;percs:".$commission;
            case 3:
                $commission = $_POST["commisionbancoplus"];
                if($value){
                    return $commission;
                }
                return "type:3;percs:".$commission;
            case 2:
                $commission = $_POST["commissions"];
                $commission_value = "";
                foreach($commission as $key => $value){
                    $commission_value .= "$value,";
                }
                $commission_value = substr($commission_value, 0 , strlen($commission_value)-1);
                return "type:2;percs:".$commission_value;
            case 4:
                $bonusM = $_POST["bonusmensile"];
                $commission = $_POST["monthly"];
                $commission_value = "";
                foreach($commission as $key => $value){
                    $commission_value .= "$value,";
                }
                $commission_value = substr($commission_value, 0 , strlen($commission_value)-1);
                return "type:4;percs:".$bonusM.",".$commission_value;
            case 9:
                $commission = $_POST["extraprofit"];
                return "type:9;percs:".$commission;
            case 11:
                $commission = $_POST["feedtax"];
                return "type:11;percs:".$commission;
        }
    }
    private static function getLiveBettingString($type){
        switch($type){
            case 5:
                $commission = ($_POST["commisionbanco"]?$_POST["commisionbanco"]:50);
                return "type:5;percs:".$commission;
            case 7:
                $commission = $_POST["commisionbancoplus"];
                return "type:7;percs:".$commission;
            case 6:
                $commission = $_POST["commissions_live"];
                $commission_value = "";
                foreach($commission as $key => $value){
                    $commission_value .= "$value,";
                }
                $commission_value = substr($commission_value, 0 , strlen($commission_value)-1);
                return "type:6;percs:".$commission_value;
            case 8:
                $bonusM = $_POST["bonusmensile"];
                $commission = $_POST["monthly_live"];
                $commission_value = "";
                foreach($commission as $key => $value){
                    $commission_value .= "$value,";
                }
                $commission_value = substr($commission_value, 0 , strlen($commission_value)-1);
                return "type:8;percs:".$bonusM.",".$commission_value;
            case 10:
                $commission = $_POST["extraprofit"];
                return "type:10;percs:".$commission;

            case 12:
                $commission = $_POST["feedtax_live"];
                return "type:12;percs:".$commission;
        }
    }

}

?>