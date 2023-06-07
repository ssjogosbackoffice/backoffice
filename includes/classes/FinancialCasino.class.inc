<?php

require_once 'Financial.class.inc';
/**
 * Class CasinoFinancial
 */

 class FinancialCasino extends Financial{

      public $options=array(1=>'casino',2=>'casino_live',3=>'virtual',4=>'casinoext',5=>'poker_live',8=>'poker',10=>'royalpk',11=>'totem',12=>'casinobor',13=>'forex');
      private $financial=true;

      public function __construct($start,$end){
       $this->start=$start;
       $this->end=$end;
       $this->getFinancialData();
       $this->calculateCasinoTotals();
     }

   /**
   * @return bool|int|RecordSet
   */
   private function getFinancialData(){
     global $dbh;
     $jur_totals=$this->getStartJurisdictionLevel();
        $sql = "select ";
        foreach($this->options as $k=>$v){
        $sql.="       coalesce(cur_level.jps_perc_".$v.", 0) as jps_perc_".$v.",
                coalesce(bet_n.jps_perc_".$v.", 0) as jps_perc_".$v."_n,
                coalesce(bet_r.jps_perc_".$v.", 0) as jps_perc_".$v."_r,
                coalesce(bet_d.jps_perc_".$v.", 0) as jps_perc_".$v."_d,
                coalesce(bet_c.jps_perc_".$v.", 0) as jps_perc_".$v."_c,
                sum(if (jds_gam_category = ".$k.", jds_bet, 0)) as ".$v."_bet,
                sum(if (jds_gam_category = ".$k.", jds_win, 0)) as ".$v."_win,
                sum(if (jds_gam_category =".$k.($k==5 || $k==8? ",jds_fin": ",jds_rake").", 0)) as ".$v."_fin,
                sum(if (jds_gam_category = ".$k.($k==6? ",jds_fin": ",jds_rake").", 0)) as ".$v."_rake,
                ";
        }
        $sql.="  jds_day,jds_jur_club, ";
        $sql .= $this->query_jur_currency[$jur_totals]." as currency,
                ".$this->query_jur_selectors[$jur_totals]."
                from
                    jurisdiction_daily_summary
                        LEFT JOIN game_category on gct_id= jds_gam_category
                        JOIN jurisdiction c on c.jur_id = jds_jur_club
                        JOIN jurisdiction d on d.jur_id = jds_jur_district
                        JOIN jurisdiction r on r.jur_id = jds_jur_region
                        JOIN jurisdiction n on n.jur_id = jds_jur_nation
                        LEFT JOIN jurisdiction_payment_settings cur_level on cur_level.jps_jur_id = ".$this->query_jur_payments[$jur_totals]."
                        LEFT JOIN jurisdiction_payment_settings bet_n on bet_n.jps_jur_id = n.jur_id
                        LEFT JOIN jurisdiction_payment_settings bet_r on bet_r.jps_jur_id = r.jur_id
                        LEFT JOIN jurisdiction_payment_settings bet_d on bet_d.jps_jur_id = d.jur_id
                        LEFT JOIN jurisdiction_payment_settings bet_c on bet_c.jps_jur_id = c.jur_id
                        WHERE jds_day >= '".$this->start."' AND jds_day <= '".$this->end."'  ";

    if($this->jurisdictionId == ""){
        if($_SESSION['jurisdiction_class'] == 'nation'){
            $sql .= " and r.jur_parent_id = ". $_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class'] == 'region'){
            $sql .= " and d.jur_parent_id = ". $_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class'] == 'district'){
            $sql .= " and c.jur_parent_id = ". $_SESSION['jurisdiction_id'];
        }
        if($_SESSION['jurisdiction_class'] == 'club'){
            $sql .= " and c.jur_id = " . $_SESSION['jurisdiction_id'];
        }
    }else{
        if($this->jurisdictionClass == 'club'){
            $sql .= " and c.jur_id = $this->jurisdictionId";
        }
        if($this->jurisdictionClass == 'district'){
            $sql .= " and d.jur_id = $this->jurisdictionId";
        }
        if($this->jurisdictionClass == 'region'){
            $sql .= " and r.jur_id = $this->jurisdictionId";
        }
        if($this->jurisdictionClass == 'nation'){
            $sql .= " and n.jur_id = $this->jurisdictionId";
        }
    }
    $sql .=" GROUP BY ".$this->query_jur_groups[$jur_totals]." HAVING (";
     foreach($this->options as $k=>$v){
        $sql.=  $v."_bet<>0 or ".$v."_win<>0 or ".$v."_rake<>0 or ";
     }
      $sql=substr($sql,0,-3);
    $sql.=" ) ORDER BY ".$this->query_jur_orders[$jur_totals];
    $this->result = $dbh->doCachedQuery($sql, 3600 ,true);
    return $this->result;
    }


    /**
    * generates all the bet, win, and rake
    */
    public function calculateCasinoTotals(){
                $diffCurr=false;
                while($this->result->hasNext()){
                    $row = $this->result->next();
                    if($row["currency"]!=$_SESSION["currency_id"]){
                        $diffCurr=true;
                        foreach($row as $n=>$nv){
                            if(((strpos($n,'bet') !== false) || (strpos($n,'win') !== false) || (strpos($n,'rake') !== false) || (strpos($n,'fin') !== false))&&(strpos($n,'betting') === false)){
                                if($nv!=0){
                                    if($_SESSION["currency_id"]==1){
                                        $this->result->Records[$this->result->CurrentIndex][$n]=$nv/$_SESSION["currencies"][$row["currency"]]["value"];
                                        $row[$n]=$nv/$_SESSION["currencies"][$row["currency"]]["value"];
                                    }
                                    else{
                                        $this->result->Records[$this->result->CurrentIndex][$n]=$nv*$_SESSION["currencies"][$_SESSION["currency_id"]]["value"];
                                        $row[$n]=$nv*$_SESSION["currencies"][$_SESSION["currency_id"]]["value"];
                                    }
                                }
                            }
                        }
                    }

                    foreach($this->options as $k=>$v){
                        $this->totalsPerc[$v.'_rake']+=(($row[$v."_rake"]/100)* ($row["jps_perc_".$v]));
                        $rake=$row[$v.'_rake']-$row[$v."_rake"]/100*($row["jps_perc_"].$v);
                        $net=($row[$v.'_fin']-($row[$v."_rake"]/100)* ($row["jps_perc_".$v]));
                        $category=$v."Perc";
                        $$category=100-$row['jps_perc_'.$v.'_n'];
                        $nationPerc=$row['jps_perc_'.$v.'_n'];
                        $regionPerc=$row['jps_perc_'.$v.'_r'];
                        $districtPerc=$row['jps_perc_'.$v.'_d'];
                        $clubPerc=$row['jps_perc_'.$v.'_c'];
                        $this->totals[$v.' net']+=$net-($rake*(($nationPerc-$regionPerc)*100/(100-$regionPerc)))/100;
                        if($_SESSION['jurisdiction_class']=='nation'){
                            $this->totals['nation net']+=($rake*(($nationPerc-$regionPerc)*100/(100-$regionPerc)))/100;
                        }
                        else{
                            $this->totals['nation net']+=$net-($rake*(($regionPerc-$districtPerc)*100/(100-$districtPerc)))/100;
                        }
                        if($_SESSION['jurisdiction_class']=='region'){
                            $this->totals['region net']+=($rake*(($regionPerc-$districtPerc)*100/(100-$districtPerc)))/100;
                        }
                        else{
                            $this->totals['region net']+=$net-($rake*(($districtPerc-$clubPerc)*100/(100-$clubPerc)))/100;
                        }
                        $this->totals['district net']+=($rake*(($districtPerc-$clubPerc)*100/(100-$clubPerc)))/100;

                    }
                    foreach($row as $key => $val){
                        if(is_numeric($val)){
                            $this->totals[$key] += $val;
                        }
                    }
                }
                $this->totalsRakeSum=0;
                $this->totalsBetSum=0;
                $this->totalsWinSum=0;
                $this->totalsFinSum=0;
                $this->totalsBonusFin=0;
                 foreach($this->options as $k=>$v){
                 if($k !='2' && $k!='5' && $k!=8 ) {
                    $this->totalsRakeSum +=$this->totals[$v.'_rake'];
                    $this->totalsBetSum +=$this->totals[$v.'_bet'];
                    $this->totalsWinSum +=$this->totals[$v.'_win'];
                    $this->totalsFinSum +=$this->totals[$v.'_fin'];
                    $this->totalsBonusFin+=$this->totals[$v.'_fin_bonus'];
                    }
                }
            }
         /**
         * @return mixed
         */
        public  function getTotalBet($withCurrency=false)
        {
            if($withCurrency){
              return getInDollars($this->totalsBetSum);
            }
            return $this->totalsBetSum;
        }
        /**
         * @return mixed
         */
        public  function getTotalWin($withCurrency=false)
        {
           if($withCurrency){
              return getInDollars($this->totalsWinSum);
            }
            return $this->totalsWinSum;
        }

     /**
         * @return mixed
         */
        public  function getTotalRake($withCurrency=false)
        {
            if($withCurrency){
              return getInDollars($this->totalsRakeSum);
            }
            return $this->totalsRakeSum;
        }

        /**
        * @param $category
        * @param bool|false $withCurrency
         * @return string
        */
        public function getCategoryBet($category,$withCurrency=false){
         if($withCurrency){
              return getInDollars($this->totals[$category."_bet"]);
            }
            return $this->totals[$category."_bet"];
        }

        /**
        * @param $category
        * @param bool|false $withCurrency
         * @return string
        */
        public function getCategoryWin($category,$withCurrency=false){
         if($withCurrency){
              return getInDollars($this->totals[$category."_win"]);
            }
            return $this->totals[$category."_win"];
        }

        /**
        * @param $category
        * @param bool|false $withCurrency
         * @return string
        */
        public function getCategoryRake($category,$withCurrency=false){
         if($withCurrency){
              return getInDollars($this->totals[$category."_rake"]);
            }
            return $this->totals[$category."_rake"];
        }

          public function getCategoryPerc($category,$withCurrency=false){
         if($withCurrency){
              return getInDollars($this->totalsPerc[$category."_rake"]);
            }
            return $this->totalsPerc[$category."_rake"];
        }

          public function getCategoryNet($category,$withCurrency=false){
         if($withCurrency){
              return getInDollars($this->getCategoryRake($category)-$this->getCategoryPerc($category));
            }
            return $this->getCategoryRake($category)-$this->getCategoryPerc($category);
        }

        /**
         * @return array
         */
        public  function getOptions()
        {
            return $this->options;
        }

        /**
         * @param array $options
         */
        public  function setOptions(array $options)
        {
            $this->options = $options;
        }


        /**
        * @param bool|false $withCurrency
         * @return int|string
        */public function getCasinoNet($withCurrency=false)
        {
            if($withCurrency){
              return getInDollars($this->getTotalRake()-$this->getCasinoPerc());
            }
            return $this->getTotalRake()-$this->getCasinoPerc();
        }


        /**
    * @param bool|false $withCurrency
     * @return number|string
    */public function getCasinoPerc($withCurrency=false)
        {
            if($withCurrency){
              return getInDollars(array_sum($this->totalsPerc)-($this->totalsPerc['poker_rake']+$this->totalsPerc['poker_live_rake']+$this->totalsPerc['casino_live_rake']));
            }
            return array_sum($this->totalsPerc)-($this->totalsPerc['poker_rake']+$this->totalsPerc['poker_live_rake']+$this->totalsPerc['casino_live_rake']);
        }

       /**
        * @param bool|false $withCurrency
         * @return int|string
        */public function getPokerNet($withCurrency=false)
        {
            if($withCurrency){
              return getInDollars($this->getCategoryRake('poker')+$this->getCategoryRake('poker_live')-$this->getCategoryPerc('poker')-$this->getCategoryPerc('poker_live'));
            }
            return $this->getCategoryRake('poker')+$this->getCategoryRake('poker_live')-$this->getCategoryPerc('poker')-$this->getCategoryPerc('poker_live');
        }


}