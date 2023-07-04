<?php


/**
 * Class Skin
 */

require_once '../../includes/php/Logger.php';
class Skin{
    public $jurisdiction,$games;
    public $defaultMenuParams=array('home' => array('id' => 'Home',
        'href' => '/home'),
        'casino' => array('id' => 'Casino',
            'href' => '/casino'),
        'casino live' => array('id' => 'Casino Live',
            'href' => '/casino_live'),
        'virtual' => array('id' => 'Virtual',
            'href' => '/virtual'),
        'betting' => array('id' => 'Betting',
            'href' => '/betting'),
        'livebetting' => array('id' => 'Live betting',
            'href' => '/livebetting'),
        'poker' => array('id' => 'Poker',
            'href' => '/poker'),
        'playtech' => array('id' => 'Playtech',
            'href' => '/playtech'),
        'bankroulette' => array('id' => 'Bankroulette',
            'href' => '/bankroulette'),
        'zecchinetta' => array('id' => 'Zechinetta',
            'href' => '/zecchinetta')

    );

    public $type='Desktop';

    public $skinInfo;

    public $layouts=array('no_banners_internal','no_banners','big_banners','headerandfooter','latin','onlycontent','full','menuandcontent','2017','2018','2019');

    public $defaultSkinConfiguration=array(
        'general' => array(/*'host'=>'goldenfish168.egamingcssc.net',
            'secure_host'=>'https://goldenfish168.egamingcssc.net',
            'skinname' => 'Goldenfish168',
            'domain' => 'goldenfish168.egamingcssc.net',*/
            'languages' => 'en,it,pt,es,nl,fr,ro,ua',
            'default' => 'en',
            'currency_name' => 'euro',
            'banned_countries'=>'uk,us,um,vi,BE,RU,IN,BR,CN,SE,PL,FI,HK,PT,NL,GR,SG,ID,NO,ES,HU,CA,PK,TR,LV,CZ,CH,AE,US,FR',
            'provider_code_mapping'=>'',
            'activation'=>1
        ),

        'games'=>array(
           // 'sections'=>' casino_live,casino,betting,betting_live,poker',
            'poker2dCode'=>'C001,euro,http://poker.euro-betting.com/www/Lobby',
            'poker2dCodeCashier'=>'enabled',
          //  'poker2dExtra '=>' C001,euro,&#8364;,http://poker.euro-betting.com/www/Lobby,1'),
        ),
       /* salvez direct 'support'=>array(
            'subject'=>'general,betting,casino'),*/

        'layout'=>array(
            'home'=>' no_banners_internal',
            'content'=>' no_banners_internal',
            'redirect'=>' headerandfooter'),

        /*'betting'=>array(
            'url '=>' http://goldenfish168.egamingcssc.net/betting',
            'mobile_url'=>' http://msports-itainment.uat.biahosted.com'),*/

        /*'banners'=>array(
            'url '=>'http://banners.goldenfish168.egamingcssc.net'),

        'version'=>array(
            'css'=>'000001',
            'js'=>'000001'),*/

        'payments'=>array(
            'disabled_operations'=>'deposit',
            'deposit_disabled'=>'ecuador_wallet',
            'withdraw_disabled'=>'visa,bank_transfer,ecuador_wallet',
            'currencies'=>'DOP - Dominican Peso,VWYGHT;USD - Dollars,VIBINN;ARS - Argentine Peso,QKAXQM;CRC - Costa Rica Colon,SRHJYU;NIO - Nicaraguan Cordoba,XDXDYK;UYU - Uruguayan Peso,BDLUVU')
    );





    public function __construct($jurisdiction=false){
        if($jurisdiction) {
            $this->jurisdiction = $jurisdiction;
        }
    }

    /**
     * @return mixed
     */
    public function getType()
    {
        return $this->type;
    }

    /**
     * @param mixed $type
     */
    public function setType($type)
    {
        $this->type = $type;
    }


    public function getMenuFromDb(){
        global $dbh;
        $sql='Select '.($this->type=='Mobile'?'scn_mobile_menu_info':'scn_menu_info').' from jurisdiction_configuration where scn_jur_id='.$dbh->escape($this->jurisdiction);
        return parse_ini_string($dbh->queryOne($sql),true);
    }




    public function createMenuOptions($current=false){
        global $lang;
        ?>
        <table class="table display table-striped" id="tableMenu">
            <thead>
            <tr>
                <th>Section</th>
                <th>Name</th>
                <th>Enabled</th></tr></thead><?
            foreach ($this->defaultMenuParams as $k=>$v){
                if($current){
                    foreach($current as $k2=>$v2){
                        if($k2==$k){
                            $v['id']=$v2['id'];
                            $v['checked']=true;
                        }
                    }
                }
                ?>
                <tr data-section="<?=$k?>">
                    <td><?=$k?></td>
                    <td><input type="text" name="<?=$k?>_section" class="sectionName" value="<?=$v['id']?>"></td>
                    <td>
                        <!--<label class="checkbox">
                <input type="checkbox" name="<?/*=$k*/?>" <?/*=($v['checked']?'checked':'')*/?> >-->
                        <input type="radio" name="<?=$k?>" class="yes" id="<?=$k?>yes" <?=($v['checked']?'checked':'')?>    />
                        <input type="radio" name="<?=$k?>" class="no" id="<?=$k?>no" class="no"  <?=(!isset($v['checked'])? "checked" :"" )?>   />
                        <div class="switch" style="margin:auto">
                            <label class="labelYes" for="<?=$k?>yes"><?=$lang->getLang("Enabled")?></label>
                            <label class="labelNo" for="<?=$k?>no"><?=$lang->getLang("Disabled")?></label>
                            <span></span>
                        </div>

                        <!--</label>-->
                    </td>
                </tr>
                <?
            }
            ?>
        </table>
        <div class="clearfix"></div><br />
        <button class="btn btn-primary updateMenu" data-type="<?=$this->type?>" >Update</button> <button class="btn btn-info updateMenu" data-type="<?=$this->type?>" data-children="true">Update underlying jurisdictions</button>
        <?
    }

    public function setMenu($menu, $underLvl=false){
        $currentMenu=$this->getMenuFromDb();
        $menuString='';
        foreach($menu as $k=>$v){
            $menuString.="[".$v->name."]".PHP_EOL;
            $menuString.='   id="'.$v->id.'"'.PHP_EOL;
            $menuString.="   href=".$this->defaultMenuParams[$v->name]['href'].PHP_EOL.PHP_EOL;
            if(isset($currentMenu[$v->name]['submenu'])){
                foreach($currentMenu[$v->name]['submenu'] as $subK=>$subV){
                    $menuString.="   submenu[] =".$subV.PHP_EOL;

                }
            }
        }
       // return $menuString;
       return $this->saveMenu($menuString,$underLvl);
    }


    /**
     * @param $newMenu
     * @param $value
     * @return bool|int|RecordSet
     */
    private function saveMenu($newMenu,$underLvl=false){
        global $dbh;
        $this->extractSkinInfo();
        $allChildren=false;
        $sql="Update  jurisdiction_configuration set ".($this->type=='Mobile'?'scn_mobile_menu_info':'scn_menu_info')."=".$dbh->prepareString($newMenu)." where scn_jur_id=".$dbh->escape($this->jurisdiction);
        if($underLvl && $underLvl!='false'){
            include_once 'JurisdictionsList.class.inc';
            $jurisdiction = JurisdictionsList::getInstance ( $dbh );
            $allChildren=implode(",",$jurisdiction->getChilJurisdictionsIdList($this->jurisdiction));
            $sql="Update  jurisdiction_configuration set ".($this->type=='Mobile'?'scn_mobile_menu_info':'scn_menu_info')."=".$dbh->prepareString($newMenu)." where scn_jur_id IN  (".$dbh->escape($allChildren).")";

        }
        $this->sendRefreshToWebpp(false,$allChildren);
        return $dbh->exec($sql);
    }

    /**
     * @return mixed
     */
    public function getJurisdiction()
    {
        return $this->jurisdiction;
    }

    /**
     * @param mixed $jurisdiction
     */
    public function setJurisdiction($jurisdiction)
    {
        $this->jurisdiction = $jurisdiction;
    }



    /**
     * @return array
     */
    public function getDefaultMenuParams()
    {
        return $this->defaultMenuParams;
    }

    /**
     * @return array
     */
    public function getDefaultSkinConfiguration()
    {
        return $this->defaultSkinConfiguration;
    }


    public function getGamesFromDb($parsed=false){
        global $dbh;
        $sql='Select '.($this->type=='Mobile'?'scn_mobile_games_info':'scn_games_info').' from jurisdiction_configuration where scn_jur_id='.$dbh->escape($this->jurisdiction);
        if($parsed){
            return parse_ini_string($dbh->queryOne($sql),true);
        }
        return $dbh->queryOne($sql);
    }

    public function getAllAvailableGames()
    {
        global $dbh;
        $sql = "SELECT pes_name, ggi_category , ggi_button_image_name, ggi_web_name, ggi_game_id, ggi_width, ggi_height, ggi_has_forfun, pes_game_code
                    FROM egamessystem.jurisdiction
                    JOIN egamessystem.skins ON jur_id = " . $dbh->escape($this->jurisdiction) . " AND skn_id = jur_skn_id
                    JOIN egamessystem.providers ON skn_pes_code_enabled like concat('%', pes_game_code, '%')
                    JOIN egamessystem.game_global_info ON ggi_pes_group = pes_group
                    UNION ALL
                    SELECT 'Internal' pes_name, ggi_category , ggi_button_image_name, ggi_web_name, ggi_game_id, ggi_width, ggi_height, ggi_has_forfun, ggi_game_id pes_game_code
                    FROM egamessystem.game_global_info
                    WHERE ggi_pes_group is null
                    ORDER BY ggi_category";
        $result = $dbh->doCachedQuery($sql, 0);
        return $result;
    }

    
    // TODO can be removed
    public function prepareAvailableGames(){
        $result=$this->getAllAvailableGames();
        $allGames = [];
        $currCategory = '';
        $result->resetRecPtr();
        while ($result->hasNext()) {
            $row = $result->next();
            if($row['pes_name']==''){
                $row['pes_name']='Internal';
            }
            if ($row['ggi_category'] != $currCategory) {
                $currCategory = $row['ggi_category'];
                if (!isset($allGames[$currCategory])) {
                    $allGames[$currCategory] = [];
                }
            }
            if (!isset($allGames[$currCategory][$row['pes_name']])) {
                $allGames[$currCategory][$row['pes_name']] = [];
            }
            array_push($allGames[$currCategory][$row['pes_name']], ['image'=>$row['ggi_button_image_name'] ,'name'=> $row['ggi_web_name'] , 'id'=> $row['pes_game_code'] ]);

        }
        return $allGames;
    }
    
    // TODO can be removed
    public function createGamesSelect(){
        $games=$this->prepareAvailableGames();
        $currentlyGames=$this->getGamesFromDb(true);
        ?>
        <select id="games" name="games[]" multiple="multiple" class="testing">
            <? foreach($games as $k=>$v) {
            foreach ($v as $pvk => $pvv) { ?>

            <optgroup label="<?= $k ." - ".$pvk   ?>">
                <? foreach ($pvv as $k2 => $v2) {
                    $selected = false;
                    if (is_array($currentlyGames[$k]['games'])) {
                        foreach ($currentlyGames[$k]['games'] as $gk => $gv) {
                            $gv = explode('@', $gv);

                            if ($gv[0] == $v2['image'] && $gv[1] == $v2['id']) {
                                $selected = true;
                            }
                        }
                    }
                    ?>
                    <option value='<?= $v2['image'] ?>##<?= $v2['pes_game_code'] ?>##<?= $k ?>' <?= ($selected ? 'selected' : '') ?> ><?= $v2['name'] ?></option>
                    <?
                }
                }
                }
                ?>
        </select><br />
        <button class="btn btn-primary updateSkinGames" data-type="<?=$this->type?>">Save</button>  <button class="btn btn-info updateSkinGames" data-type="<?=$this->type?>" data-children=true>Save underlying levels</button>
        <?
    }

    
    public function createGamesSelectNew($providers){
        if($providers == '') return 2;
        global $dbh;
            $sql = "INSERT INTO egamessystem.games_configured SELECT scn_jur_id, ".($this->type=='Mobile'?'scn_mobile_games_info':'scn_games_info')." FROM egamessystem.jurisdiction_configuration WHERE scn_jur_id = " . $dbh->escape($this->jurisdiction);
            $dbh->doCachedQuery($sql, 0);
            $sql = "SELECT CONCAT(pes_name) label_name , CONCAT(ggi_button_image_name, '##', pes_game_code, '##', ggi_category) game_string, ggi_web_name,
                    if (gcd_jur_id is not null, '1', '0') configured
                    FROM
                    (SELECT pes_name, ggi_category , ggi_button_image_name, ggi_web_name, ggi_game_id, ggi_width, ggi_height, ggi_has_forfun, pes_game_code
                    FROM egamessystem.jurisdiction
                    JOIN egamessystem.skins ON jur_id = " . $dbh->escape($this->jurisdiction) . " AND skn_id = jur_skn_id
                    JOIN egamessystem.providers ON pes_game_code in ($providers) AND (jur_currency = pes_master_currency OR pes_other_currency like concat('%', jur_currency, '%'))
                    JOIN egamessystem.game_global_info ON ggi_pes_group = pes_group
                    UNION ALL 
                    SELECT 'Internal' pes_name, ggi_category , ggi_button_image_name, ggi_web_name, ggi_game_id, ggi_width, ggi_height, ggi_has_forfun, ggi_game_id pes_game_code
                    FROM egamessystem.game_global_info 
                    WHERE ggi_pes_group is null
                    ORDER BY ggi_category ) t
                    LEFT JOIN egamessystem.games_configured ON gcd_jur_id = " . $dbh->escape($this->jurisdiction) . " AND gcd_games LIKE CONCAT('%', pes_game_code, '@', ggi_has_forfun, '@', ggi_web_name, '@', pes_name, '%')
                    ORDER BY label_name, ggi_web_name";

            $result = $dbh->doCachedQuery($sql, 0);
            $sql = "DELETE FROM egamessystem.games_configured WHERE gcd_jur_id = " . $dbh->escape($this->jurisdiction);
            $dbh->doCachedQuery($sql, 0);
        ?>
        <select id="games" name="games[]" multiple="multiple" >
            <?  $labelName = '';
                $result->resetRecPtr();
                while ($result->hasNext()) {
                    $row = $result->next();
                    if($row['label_name'] != $labelName) {
                        $labelName = $row['label_name'];
                        ?> 
                        <optgroup label="<?= $labelName ?>"> 
                        <?
                    } // end if
                    ?>
                    <option value='<?= $row['game_string'] ?>' <?= ($row['configured'] == '1' ? 'selected' : '') ?> ><?= $row['ggi_web_name'] ?></option>
                    <?
                } // end while
                if ($this->jurisdiction != 1) { ?>
        </select><br />
        
        <button class="btn btn-primary updateSkinGames" data-type="<?=$this->type?>">Save</button>  <button class="btn btn-info updateSkinGames" data-type="<?=$this->type?>" data-children=true>Save underlying levels</button>
        		<? } // end if 
    }
    
    
    public function setGames(array $newGames=null,$underLvl=false)
    {
        $allGames = $this->getAllAvailableGames();
        $allGames->resetRecPtr();
        $currCategory = '';
        $skinConfiguration=$this->getSkinProperties(true);
        // $skinConfiguration['general']['domain']='srv.egamingcssc.net';
        $saveStr ="[urls]".PHP_EOL;
        $saveStr.=  "  fun  = 'http://srv.".$skinConfiguration['general']['domain']."/services/GameLauncherForFun?hst=2&'".PHP_EOL . 
                    "  real = 'http://srv.".$skinConfiguration['general']['domain']."/services/GameLauncher?hst=2&ptnc=wbsite&'".PHP_EOL;
        while ($allGames->hasNext()) {
            $row = $allGames->next();
            if(is_array($newGames)) {
                if (in_array($row['ggi_button_image_name'] . '##' . $row['pes_game_code'] . '##' . $row['ggi_category'], $newGames)) {
                    if ($row['ggi_category'] != $currCategory) {
                        $currCategory = $row['ggi_category'];
                        $saveStr .= "[" . $currCategory . "]" . PHP_EOL;
                    }
                    $saveStr .= '   games[]="' . $row['ggi_button_image_name'] . '@' . $row['ggi_game_id'] . '@' . $row['ggi_width'] . '@' . $row['ggi_height'] . '@' . $row['pes_game_code'] . '@' . $row['ggi_has_forfun'] . '@' . $row['ggi_web_name']. '@' . $row['pes_name'] . '"' . PHP_EOL;
                }
            } 
        }
        return $this->saveGames($saveStr,$underLvl);
    }

    private function saveGames($games,$underLvl=false){
        global $dbh;
        $this->extractSkinInfo();
        $allChildren=false;
        $sql="Update  jurisdiction_configuration set ".($this->type=='Mobile'?'scn_mobile_games_info':'scn_games_info')."=".$dbh->prepareString($games)." where scn_jur_id=".$dbh->escape($this->jurisdiction);
        if($underLvl && $underLvl!='false'){
            include_once 'JurisdictionsList.class.inc';
            $jurisdiction = JurisdictionsList::getInstance ( $dbh );
            $allChildren=implode(",",$jurisdiction->getChilJurisdictionsIdList($this->jurisdiction));
            $sql="Update  jurisdiction_configuration set ".($this->type=='Mobile'?'scn_mobile_games_info':'scn_games_info')."=".$dbh->prepareString($games)." where scn_jur_id IN (".$dbh->escape($allChildren).")";

        }
        $this->sendRefreshToWebpp(false,$allChildren);
        return $dbh->exec($sql);
    }

    public function createSettingsView(){
        $this->extractSkinInfo();
        $view='<div class="well"> <form class="form-horizontal" id="skinForm">';
        $allcountries=getAvailableCountries();
        $paymentProcessors=getAllPaymentProcessors();
        $currencies=getAllCurrencies();
        $providers=getProviders(true);
        if($this->getSkinProperties(true)){
            foreach ($this->getSkinProperties(true) as $key=>$value){
                foreach($value as $key2=>$value2){
                    $this->defaultSkinConfiguration[$key][$key2]=$value2;
                }
            }
//            echo "<pre>";
//                print_r($this->defaultSkinConfiguration);
//                echo "</pre>";
        }
        $banner_found = false;

        foreach ($this->defaultSkinConfiguration as $k=>$v) {
//            if($k == 'general'){
//                echo "<pre>";
//                print_r($v);
//                echo "</pre>";
//            }
            if ($k == 'general' || $k == 'games' || $k == 'layout' || $k == 'payments' || $k== 'banners') {
                if($k=='banners')$view .= "<div class='span5'> <h2 class='text-info text-capitalize'>URLs</h2>";
                else $view .= "<div class='span5'> <h2 class='text-info text-capitalize'>" . $k . "</h2>";
                foreach ($v as $k2 => $v2) {
                    if ($k2 == 'activation') {
                        $view .= '<div class="control-group">
                        <label class="control-label text-success" for="' . $k2 . '">Activation</label>
                        <div class="controls"><select id="' . $k2 . '" name="' . $k2 . '">';
                            $view .= '<option value="1" '.($v2==1?'selected':'').' >True</option>';
                            $view .= '<option value="0" '.($v2==0?'selected':'').' >False</option>';
                        $view .= '</select></div>
                      </div>';
                    }
                    if ($k2 == 'languages') {
                        $view .= '<div class="control-group">
                        <label class="control-label text-success" for="' . $k2 . '">Languages</label>
                        <div class="controls"><select class="selectMultiple" multiple="multiple" id="' . $k2 . '" name="' . $k2 . '[]">';
                        $v2 = explode(',', $v2);
                        while ($allcountries->hasNext()) {
                            $row = $allcountries->next();
                            if ($row['cou_code'] == 'US') {
                                $row['cou_code'] = 'EN';
                            }

                            $view .= '<option value="' . $row['cou_code'] . '" ' . (in_array($row['cou_code'], array_map('strtoupper', $v2)) ? 'selected' : '') . ' >' . $row['cou_name'] . '</option>';
                        }
                        $view .= '</select></div>
                      </div>';
                    } elseif ($k2 == 'default' && $k == 'general') {
                        $view .= '<div class="control-group">
                        <label class="control-label text-success" for="' . $k2 . '">Default language</label>
                        <div class="controls"><select id="' . $k2 . '" name="' . $k2 . '">';
                        $allcountries->resetRecPtr();
                        while ($allcountries->hasNext()) {
                            $row = $allcountries->next();
                            if ($row['cou_code'] == 'US') {
                                $row['cou_code'] = 'EN';
                            }
                            $view .= '<option value="' . $row['cou_code'] . '" ' . ((strtolower($row['cou_code']) == strtolower($v2) || ($row['cou_code'] == 'US' && strtolower($v2) == 'en')) ? 'selected' : '') . ' >' . $row['cou_name'] . '</option>';
                        }

                        $view .= '</select></div>
                      </div>';
                    } elseif ($k2 == 'currency_name' && $k == 'general') {
                        $view .= '<div class="control-group">
                        <label class="control-label text-success" for="' . $k2 . '">Currency</label>
                        <div class="controls">
                          <input type="text" disabled id="' . $k2 . '" name="' . $k2 . '" placeholder="' . $k2 . '" value="' .self::getCurrencyName() . '">
                        </div>
                      </div>';
                    } elseif ($k2 == 'banned_countries') {
                        $view .= '<div class="control-group">
                        <label class="control-label text-success" for="' . $k2 . '">Banned countries</label>
                        <div class="controls"><select class="selectMultiple" multiple="multiple" id="' . $k2 . '" name="' . $k2 . '[]" >';
                        $v2 = explode(',', $v2);
                        $allcountries->resetRecPtr();
                        while ($allcountries->hasNext()) {
                            $row = $allcountries->next();
                            if ($row['cou_code'] == 'US') {
                                $row['cou_code'] = 'EN';
                            }

                            $view .= '<option value="' . $row['cou_code'] . '" ' . (in_array($row['cou_code'], array_map('strtoupper', $v2)) ? 'selected' : '') . ' >' . $row['cou_name'] . '</option>';
                        }
                        $view .= '</select></div>
                      </div>';
                    }
                    elseif (($k2=='provider_code_mapping')){
                        if ($_SESSION['jurisdiction_class'] == 'casino') {
                            $view .= '<div class="control-group">
                        <label class="control-label text-success" for="' . $k2 . '">Provider mapping</label>
                        <div class="controls">
                          <input type="text" id="' . $k2 . '" name="' . $k2 . '" placeholder="' . $k2 . '" value="' . $v2 . '">
                        </div>
                      </div>';
                        }
                    }

                    elseif ($k == 'layout') {

                        if($_SESSION['jurisdiction_class'] == 'casino'){
                            $view .= '<div class="control-group">
                        <label class="control-label text-success" for="' . $k2 . '">' . $k2 . '</label>
                        <div class="controls"><select  id="' . $k2 . '" name="' . $k2 . '">';

                            foreach($this->layouts as $lk=>$lv) {

                                $view .= '<option value="'.$lv.'" '.($v2==$lv?'selected':'').' >'.$lv.'</option> ';
                            }
                            $view.='
                         </select>
                          </div>
                      </div>';

                        } else {

                            $view .= '<div class="control-group">
                        <label class="control-label text-success" for="' . $k2 . '">' . $k2 . '</label>
                        <div class="controls"><select  id="' . $k2 . '" name="' . $k2 . '">
                         <option value="" selected disabled="disabled">Default</option>
                         </select>
                          </div>
                      </div>';
                        }
                    }
                    elseif ($k == 'banners') {
                        $banner_found = true;
                        $view .= '<div class="control-group">
                        <label class="control-label text-success" for="' . $k2 . '">Banner</label>
                        <div class="controls">
                       <input type="text" id="burl" name="burl" placeholder="url" value="' . $v2 . '">
                          </div>
                      </div>';
                    } elseif ($k2 == 'disabled_operations') {
                        $view .= '<div class="control-group">
                        <label class="control-label text-success" for="' . $k2 . '">Disabled Operations</label>
                        <div class="controls">
                        <select  id="' . $k2 . '"  name="' . $k2 . '">
                         <option value="" '.($v2==''?'selected':'').' >None</option>
                         <option value="withdraw"  '.($v2=='withdraw'?'selected':'').' >Withdraw</option>
                         <option value="deposit" '.($v2=='deposit'?'selected':'').' >Deposit</option>
                         <option value="deposit,withdraw" '.($v2=='deposit,withdraw'?'selected':'').'  >Both</option>
                         </select>
                          </div>
                      </div>';
                    } elseif ($k2 == 'poker2dCode') {
                        $v2 = explode(',', $v2);
                        $view .= '<div class="control-group">
                        <label class="control-label text-success" for="' . $k2 . '">Poker providers</label>
                        <div class="controls">
                        <select  id="ppc"  name="ppc">';
                        $view .= '<option value=""  > NONE </option>';

                        while ($providers->hasNext()) {
                            $row = $providers->next();
                            $view .= '<option value="' . $row['pes_game_code'] . '" ' . (strtolower($row['pes_game_code']) == strtolower($v2[0]) ? 'selected' : '') . ' >' . $row['pes_game_code'] . '</option>';
                        }
                        $view .= '</select>
                      </div></div>';

                        $view .= '<div class="control-group">
                        <label class="control-label text-success" for="' . $k2 . '">Poker currency</label>
                        <div class="controls">
                        <select id="pcur"  name="pcur">';
                        while ($currencies->hasNext()) {
                            $row = $currencies->next();

                            $view .= '<option value="' . $row['cur_code'] . '" ' . (strtolower($row['cur_code']) == strtolower($v2[1]) ? 'selected' : '') . ' >' . $row['cur_name'] . '</option>';
                        }
                        $view .= '     </select>
                          </div>
                      </div>';

                        $view .= '<div class="control-group">
                        <label class="control-label text-success" for="' . $k2 . '">Poker url</label>
                        <div class="controls">
                       <input type="text" id="purl" name="purl" placeholder="Poker url" value="' . $v2[2] . '">
                          </div>
                      </div>';
                    } elseif ($k2 == 'poker2dCodeCashier') {
                        $view .= '<div class="control-group">
                        <label class="control-label text-success" for="' . $k2 . '">Poker cashier status</label>
                        <div class="controls">
                        <select  id="' . $k2 . '"  name="' . $k2 . '">
                         <option value="disabled" '.($v2=='disabled'?'selected':'').' >Disabled</option>
                         <option value="enabled" '.($v2=='enabled'?'selected':'').'  >Enabled</option>
                         </select>
                          </div>
                      </div>';
                    } elseif ($k2 == 'deposit_disabled') {
                        $view .= '<div class="control-group">
                        <label class="control-label text-success" for="' . $k2 . '">Deposit disabled</label>
                        <div class="controls"><select multiple="multiple" class="selectMultiple" id="' . $k2 . '"  name="' . $k2 . '[]">';
                        $v2 = explode(',', $v2);
                        while ($paymentProcessors->hasNext()) {
                            $row = $paymentProcessors->next();
                            $view .= '<option value="' . $row['ppc_code'] . '" ' . (in_array($row['ppc_code'], array_map('strtoupper', $v2)) ? 'selected' : '') . ' >' . $row['ppc_name'] . '</option>';
                        }
                        $view .= '</select></div>
                      </div>';
                    } elseif ($k2 == 'withdraw_disabled') {
                        $view .= '<div class="control-group">
                        <label class="control-label text-success" for="' . $k2 . '">Withdraw disabled</label>
                        <div class="controls"><select multiple="multiple" class="selectMultiple" id="' . $k2 . '"  name="' . $k2 . '[]">';
                        $v2 = explode(',', $v2);
                        $paymentProcessors->resetRecPtr();
                        while ($paymentProcessors->hasNext()) {
                            $row = $paymentProcessors->next();
                            $view .= '<option value="' . $row['ppc_code'] . '" ' . (in_array($row['ppc_code'], array_map('strtoupper', $v2)) ? 'selected' : '') . ' >' . $row['ppc_name'] . '</option>';
                        }
                        $view .= '</select></div>
                      </div>';
                    } elseif ($k2 == 'currencies') {
                        if ($_SESSION['jurisdiction_class'] == 'casino') {
                            $view .= '<div class="control-group">
                        <label class="control-label text-success" for="' . $k2 . '">Currencies</label>
                        <div class="controls">
                          <input type="text" id="' . $k2 . '" name="' . $k2 . '" placeholder="' . $k2 . '" value="' . $v2 . '">
                        </div>
                      </div>';
                        }
                    }

                }
                $view .= "</div> ";
            }

        }
        if($banner_found == false){
            $view .= "<div class='span5'> <h2 class='text-info text-capitalize'>URLs</h2>";
            $view .= '<div class="control-group">
                        <label class="control-label text-success" for="burl">Banner</label>
                        <div class="controls">
                       <input type="text" id="burl" name="burl" placeholder="url" value="'.$this->skinInfo['skn_banner_url'].'">
                          </div>
                      </div>';
            $view .= "</div> ";
        }
        $view.="<div class='clearfix'></div>
        <button type='button' class='btn btn-primary updateSkinConfiguration' data-type=".$this->type." >Save</button> <button type='button' class='btn btn-info updateSkinConfiguration' data-children=true data-type=".$this->type." >Save underlying jurisdictions</button>
        </form></div>";
        return $view;
    }

    public function getSkinProperties($parsed=false){
        global $dbh;
        $sql='Select '.($this->type=='Mobile'?'scn_mobile_skin_properties':'scn_skin_properties').' from jurisdiction_configuration where scn_jur_id='.$dbh->escape($this->jurisdiction);
        if($parsed){
            return parse_ini_string($dbh->queryOne($sql),true);
        }
        return $dbh->queryOne($sql);
    }


    public function setSkinProperties($newConfiguration, $underLvl=false){
//        echo "<pre>";
//        print_r($newConfiguration);
//        echo  "</pre>";
        $configStr='';
        $this->extractSkinInfo();
        $skinConfiguration=$this->getSkinProperties(true);
        $skinConfiguration['general']['host']=str_replace('http://','',$this->skinInfo['skn_url']);
        $skinConfiguration['general']['secure_host']=str_replace('http://','https://',$this->skinInfo['skn_url']);
        $skinConfiguration['general']['skinname']=$this->skinInfo['skn_name'];
        $skinConfiguration['general']['domain']=$this->skinInfo['skn_domain'];
        $skinConfiguration['general']['currency_name']=$this->getCurrencyName();
        $skinConfiguration['general']['languages']=implode(',',array_map('strtolower',$newConfiguration['languages']));
        $skinConfiguration['general']['default']=strtolower($newConfiguration['default']);
        $skinConfiguration['general']['banned_countries']=implode(',',array_map('strtolower',$newConfiguration['banned_countries']));
        $skinConfiguration['general']['activation']=$newConfiguration['activation'];
        if(isset($newConfiguration['provider_code_mapping'])) {
            $skinConfiguration['general']['provider_code_mapping'] = $newConfiguration['provider_code_mapping'];
        }

        $skinConfiguration['support']['subject']='general,betting,casino';


        if(isset($newConfiguration['home'])) {
            $skinConfiguration['layout']['home'] = $newConfiguration['home'];
            $skinConfiguration['layout']['content'] = $newConfiguration['content'];
            $skinConfiguration['layout']['redirect'] = $newConfiguration['redirect'];
        }
        if(!isset($skinConfiguration['layout']['home'])) {
                $skinConfiguration['layout']['home'] = $this->layouts[0];
                $skinConfiguration['layout']['content'] = $this->layouts[0];
                $skinConfiguration['layout']['redirect'] = $this->layouts[3];
        }
        if(isset($newConfiguration['burl'])) {
            $skinConfiguration['banners']['url'] = $newConfiguration['burl'];
        }

        if(!isset($skinConfiguration['betting']['url'])) {
            $skinConfiguration['betting']['url'] = '/betting';
            $skinConfiguration['betting']['mobile_url'] = '/betting';
        }

        $skinConfiguration['version']['css']='00001';
        $skinConfiguration['version']['js']='00001';

        if($newConfiguration['ppc']==''){
            $skinConfiguration['games']['poker2dCode']='';
        }
        else{
            $skinConfiguration['games']['poker2dCode']=$newConfiguration['ppc'].",".$newConfiguration['pcur'].",".$newConfiguration['purl'];
            $skinConfiguration['games']['poker2dCodeCashier']=$newConfiguration['poker2dCodeCashier'];

        }
        if(sizeof($this->getMenuFromDb())>0){
            $skinConfiguration['games']['sections']=implode(',',array_keys($this->getMenuFromDb()));
        }

        $skinConfiguration['payments']['disabled_operations']=$newConfiguration['disabled_operations'];
        $skinConfiguration['payments']['deposit_disabled']=implode(',',$newConfiguration['deposit_disabled']);
        $skinConfiguration['payments']['withdraw_disabled']=implode(',',$newConfiguration['withdraw_disabled']);

        if(isset($newConfiguration['currencies'])) {
            $skinConfiguration['payments']['currencies'] = $newConfiguration['currencies'];
        }
        foreach($skinConfiguration as $k=>$v){
            $configStr .= "[" . $k . "]" . PHP_EOL;
            foreach($v as $k2=>$v2){
                $configStr .=$k2.'="'.$v2.'"'.PHP_EOL;
            }
        }
        return $this->saveSkinProperties($configStr, $underLvl);
    }

    private function saveSkinProperties($properties, $underLvl=false){
        global $dbh;
        $allChildren=false;
        $sql="Update  jurisdiction_configuration set ".($this->type=='Mobile'?'scn_mobile_skin_properties':'scn_skin_properties')."=".$dbh->prepareString($properties)." where scn_jur_id=".$dbh->escape($this->jurisdiction);
        if($underLvl && $underLvl!='false'){
            include_once 'JurisdictionsList.class.inc';
            $jurisdiction = JurisdictionsList::getInstance ( $dbh );
            $allChildren=implode(",",$jurisdiction->getChilJurisdictionsIdList($this->jurisdiction));
            $sql="Update  jurisdiction_configuration set ".($this->type=='Mobile'?'scn_mobile_skin_properties':'scn_skin_properties')."=".$dbh->prepareString($properties)." where scn_jur_id IN (".$dbh->escape($allChildren).")";
        }
//        $this->sendRefreshToWebpp(false,$allChildren);

        $result = $dbh->exec($sql);
        return $dbh->getAffectedRows();

    }

    public function extractSkinInfo(){
        $this->skinInfo=getSkinByJurisdiction($this->jurisdiction,true);
        return $this->skinInfo;
    }

    public function sendRefreshToWebpp($type=false,$jurlist=false){
        $dataToSend = "operationtype=3024&skinid=".$this->skinInfo['jur_skn_id']."&optype=2&jurid=".$this->jurisdiction;
        if($jurlist){
            $dataToSend.="&jurlist=".$jurlist;
        }
        $response = sendToWebApp($dataToSend, WEB_APP_WEBSITES);
        return $response;
    }

    private function getCurrencyName(){
        global $dbh;
        return strtolower($dbh->queryOne("Select cur_name from currency join jurisdiction on jur_currency=cur_id where jur_id=".$this->jurisdiction));
    }

}