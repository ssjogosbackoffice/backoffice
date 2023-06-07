<?php


/**
 * Class Lotto
 */
class Lotto{
    public $code;
    private $id;
    public $name;
    public $country;
    public $siteUrl;
    public $extractionDays=array();
    public $type;
    public $status;
    public $odds;
    public $config;
    public $extractionType;

    public function __construct($code){
        $this->code=$code;
    }

    /**
     * @param $field
     * @param $value
     * @return bool|int|RecordSet
     */
    private function saveLottoModify($field,$value){
        global $dbh;
        $sql="Update game_lotto set ".$field."=".$dbh->prepareString($value)." WHERE gls_code=".$dbh->prepareString($this->code);
        return $dbh->exec($sql);
    }

    /**
     * @param $country
     * @return mixed
     */
    public function setCountry($country){
        $this->country=$country;
        return $this->saveLottoModify('gls_cou_code',$this->country);
    }


    /**
     * @return mixed
     */
    public function getCountry()
    {
        return $this->country;
    }

    /**
     * @param $name
     * @return mixed
     */
    public  function setName($name){
        $this->name=$name;
        return $this->saveLottoModify('gls_name',$this->name);
    }

    /**
     * @return mixed
     */
    public function getName()
    {
        return $this->name;
    }


    /**
     * @param $url
     * @return mixed
     */
    public  function setSiteUrl($url){
        $this->siteUrl=$url;
        return $this->saveLottoModify('gls_result_site_url',$this->siteUrl);
    }

    /**
     * @return mixed
     */
    public function getSiteUrl()
    {
        return $this->siteUrl;
    }

    /**
     * @param mixed $status
     */
    public function setStatus($status)
    {
        $this->status=$status;
        return $this->saveLottoModify('gls_status',$this->status);
    }

    /**
     * @return mixed
     */
    public function getStatus()
    {
        return $this->status;
    }

    /**
     * @param array $extractionDays
     */
    public function setExtractionDays($extractionDays)
    {
        $this->extractionDays = $extractionDays;
        return $this->saveLottoModify('gls_extraction_days',$this->extractionDays);
    }

    /**
     * @return array
     */
    public function getExtractionDays()
    {
        return $this->extractionDays;
    }

    /**
     * @param mixed $type
     */
    public function setType($type)
    {
        $this->type = $type;
        return $this->saveLottoModify('gls_type',$this->type);

    }

    /**
     * @return mixed
     */
    public function getType()
    {
        return $this->type;
    }


    /**
     * @param bool $start
     * @param bool $end
     * @return bool|int|RecordSet
     */
    public function getExtractionResults($start=false,$end=false){
        global $dbh;
        if(!$start){
            $start=date('Y-m-d');
        }
        if(!$end){
            $end=date('Y-m-d');
        }
        $sql="Select lrs_result from lottos_result where LRS_GLS_CODE=".$dbh->prepareString($this->code)." AND lrs_time_of_extraction BETWEEN '".
            $dbh->escape($start)." 00:00:00' AND '".$dbh->escape($end)." 23:59:59' AND lrs_result is not null";
        return $dbh->doCachedQuery($sql,60);
    }


    /**
     * @param $result
     * @param bool $probability
     * @return array
     */
    public function getNumberAppearence($result,$probability=false){
        $resultArr=array();
        foreach ($result as $k=>$v){
            $currentResult=explode(',',$v['lrs_result']);
            foreach($currentResult as $pos=>$number){
                $resultArr[$number]++;
            }
        }
        if($probability){
            $total=array_sum($resultArr);
            $return=array_map(function($value) use ($total){
               return round($value*100/$total,2);
            },$resultArr);
            ksort($return);
            return $return;
        }

        return $resultArr;
    }

    /**
    * @return mixed
    */
    public  function getOdds()
    {
     global $dbh;
     $sql="Select gls_odds from game_lotto where gls_code=".$dbh->prepareString($this->code);
     return $dbh->queryOne($sql);
    }
    /**
     * @param mixed $odds
     */
    public  function setOdds($odds)
    {
        $this->odds = $odds;
        return $this->saveLottoModify('gls_odds',$this->odds);
    }
    /**
    * @return mixed
    */
    public  function getConfig()
    {
     global $dbh;
     $sql="Select gls_config from game_lotto where gls_code=".$dbh->prepareString($this->code);
     return $dbh->queryOne($sql);

    }/**
     * @param mixed $config
     */
    public  function setConfig($config)
    {
        $this->config = $config;
        return $this->saveLottoModify('gls_config',$this->config);
    }
    /**
         * @return mixed
         */
        public  function getExtractionType()
        {
            return $this->extractionType;
        }/**
         * @param mixed $extractionType
         */
        public  function setExtractionType($extractionType)
        {
            $this->extractionType = $extractionType;
             return $this->saveLottoModify('gls_extraction_numbers',$extractionType);
        }



}