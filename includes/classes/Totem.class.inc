<?php
/**
 * Created by Marian
 * Date: 13/02/2017
 * Time: 13.42
 * File: TotemManager.class.inc
 */


class Totem {
    private $code;
    private $id;
    public $config_params;
    private $md5;
    private $configKey;
    private $access;
    private $punId;

    public function constuct(){

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
    public function getConfigParams()
    {
        return $this->config_params;
    }

    /**
     * @param mixed $config_params
     */
    public function setConfigParams($config_params)
    {
        $this->config_params = $config_params;
    }

    /**
     * @return mixed
     */
    public function getMd5()
    {
        return $this->md5;
    }

    /**
     * @param mixed $md5
     */
    public function setMd5($md5)
    {
        $this->md5 = $md5;
    }

    /**
     * @return mixed
     */
    public function getConfigKey()
    {
        return $this->configKey;
    }

    /**
     * @param mixed $configKey
     */
    public function setConfigKey($configKey)
    {
        $this->configKey = $configKey;
    }

    /**
     * @return mixed
     */
    public function getAccess()
    {
        return $this->access;
    }

    /**
     * @param mixed $access
     */
    public function setAccess($access)
    {
        $this->access = $access;
    }

    /**
     * @return mixed
     */
    public function getPunId()
    {
        return $this->punId;
    }

    /**
     * @param mixed $punId
     */
    public function setPunId($punId)
    {
        $this->punId = $punId;
    }

    public function updateGames($newGames,$gameCategories){
        try {
            $gamesArr = [];
            foreach ($newGames as $gk => $gv) {
                $gv = explode(';', $gv);
                if (!isset($gamesArr[$gameCategories->$gv[0]])) {
                    $gamesArr[$gameCategories->$gv[0]] = $gameCategories->$gv[0] . "," . $gv[1];
                } else {
                    $gamesArr[$gameCategories->$gv[0]] .= ";" . $gv[1];
                }
            }
            $gamesArr = implode('|', $gamesArr);
            $gamesArr = 'gameid:' . $gamesArr;
            $currentParams = explode('~', $this->getConfigParams());
            $customCat='custcat:';
            foreach ($gameCategories as $k=>$v){
                  $customCat.=$k."-".$v.";";

            }
            $customCat=substr($customCat, 0, -1);


            foreach($currentParams as $key => $value){
                list($label, $content) =  explode(":", $value);
                if($label == 'custcat'){
                    $hascurrparam=$key;
                }
                if($label == 'gameid'){
                    unset($currentParams[$key]);
                }
            }
            if($hascurrparam){
                $currentParams[$hascurrparam]=$customCat;
            }
            else{
                array_push($currentParams,$customCat);
            }
            array_push($currentParams,$gamesArr);
           // $currentParams[0] = $gamesArr;
            $this->setConfigParams(implode('~', $currentParams));

            return $this->saveConfigParams();
          //  return true;
        }
        catch (Exception $e){
            return false;
        }
    }

    public function saveConfigParams(){
        global$dbh;
        $sql="Update totem set ttm_conf_param=".$dbh->prepareString($this->config_params)." WHERE ttm_code=".$dbh->prepareString($this->code);
        return $dbh->exec($sql);
    }

}