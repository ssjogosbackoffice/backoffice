<?php

require_once 'Financial.class.inc';
/**
 * Created by PhpStorm.
 * User: marian
 * Date: 13/01/16
 * Time: 14:37
 */
class FinancialBetting extends Financial
{

    public function __construct($start,$end){
       $bettingProfiles=$this->getBettingProfiles();
        while($bettingProfiles->hasNext()){
            $row=$bettingProfiles->next();
        $this->bettingProfilesTypes[$row['idprofilocommissionetipo']]=array('name'=>$row['nome'],
            'formulas'=>array('casino'  => $row['formulacasino'],
                'nation'  => $row['formulanation'],
                'region'  => $row['formularegion'],
                'district'=> $row['formuladistrict'])
        );
    }
        $this->start=$start;
        $this->end=$end;
        $this->getFinancialData();
        $this->calculateTotals();
    }


    /**
    * @return bool|int|RecordSet
    */
    private function getFinancialData(){
    $jur_totals=$this->getStartJurisdictionLevel();
    global $dbh;
    $sql = "select coalesce(cur_level.jps_perc_betting, 0) as jps_perc_betting,
             coalesce(cur_level.jps_perc_live_betting, 0) as jps_perc_betting_live,
             coalesce(bet_n.jps_perc_betting, 0) as jps_perc_betting_n,
             coalesce(bet_n.jps_perc_live_betting, 0) as jps_perc_betting_live_n,
             coalesce(bet_r.jps_perc_betting, 0) as jps_perc_betting_r,
             coalesce(bet_r.jps_perc_live_betting, 0) as jps_perc_betting_live_r,
             coalesce(bet_d.jps_perc_betting, 0) as jps_perc_betting_d,
             coalesce(bet_d.jps_perc_live_betting, 0) as jps_perc_betting_live_d,
             coalesce(bet_c.jps_perc_betting, 0) as jps_perc_betting_c,
             coalesce(bet_c.jps_perc_live_betting, 0) as jps_perc_betting_live_c,
             jds_day,jds_jur_club,
             sum(if (jds_gam_category = 7, jds_fin, 0)) as betting_fin,
             sum(if (jds_gam_category = 7, jds_bet, 0)) as betting_bet,
             sum(if (jds_gam_category = 7, jds_win, 0)) as betting_win,
             sum(if (jds_gam_category = 9, jds_bet, 0)) as betting_bet_live,
             sum(if (jds_gam_category = 9, jds_fin, 0)) as betting_fin_live,
             sum(if (jds_gam_category = 9, jds_win, 0)) as betting_win_live,
             sum(if (jds_gam_category = 7, jds_rake, 0)) as betting_rake ,
             sum(if (jds_gam_category = 9, jds_rake, 0)) as betting_rake_live ,
             ";
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
  $sql .=" GROUP BY ".$this->query_jur_groups[$jur_totals]." HAVING (betting_bet<>0 or betting_win<>0 or betting_rake<>0 or
                     betting_bet_live<>0 or betting_win_live<>0 or betting_rake_live<>0) ";


    $sql.="  ORDER BY ".$this->query_jur_orders[$jur_totals];

     $this->result = $dbh->doCachedQuery($sql, 3600,true);
    return $this->result;
    }


    private function calculateTotals(){
                $caTot=0;
                $prevNTotR=0;
                $prevN='';
                $prevNProfile='';
                $prevRTotR=0;
                $prevR='';
                $prevRProfile='';
                $prevDTotR=0;
                $prevD='';
                $prevDProfile='';

                while($this->result->hasNext()){
                    $row = $this->result->next();
                    if($row["currency"]!=$_SESSION["currency_id"]){
                        foreach($row as $n=>$nv){
                            if(((strpos($n,'bet') !== false) || (strpos($n,'win') !== false) || (strpos($n,'rake') !== false) || (strpos($n,'fin') !== false))&&(strpos($n,'jps_perc') === false)){
                                if($nv!=0 && isset($_SESSION["currencies"][$row["currency"]]["value"])){
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

                    if($this->getBettingPercentage($row["jps_perc_betting_c"],"number")=="1" || $this->getBettingPercentage($row["jps_perc_betting_c"],"number")=="3" ){
                        $row['betting_rake']= $this->result->Records[$this->result->CurrentIndex]['betting_rake']=($row["betting_bet"]-$row["betting_win"])*$this->getBettingPercentage($row["jps_perc_betting_c"])/100;
                        $row['betting_rake_live']= $this->result->Records[$this->result->CurrentIndex]['betting_rake_live']=($row["betting_bet_live"]-$row["betting_win_live"])*$this->getBettingPercentage($row["jps_perc_betting_live_c"])/100;
                    }
                    $netTot=$row["betting_bet"]+$row["betting_bet_live"]- $row["betting_win"]-$row["betting_win_live"]-$row["betting_rake"]-$row["betting_rake_live"]-$this->calculateBonus($row);
                    $curN=$row['n_jur_name'];
                    $curNProfile=$this->getBettingPercentage($row['jps_perc_betting_n'],'numeric');
                    $curNProc=$this->getBettingPercentage($row['jps_perc_betting_n']);
                    $curR=$row['r_jur_name'];
                    $curRProfile=$this->getBettingPercentage($row['jps_perc_betting_r'],'numeric');
                    $curRProc=$this->getBettingPercentage($row['jps_perc_betting_r']);
                    $curD=$row['d_jur_name'];
                    $curDProfile=$this->getBettingPercentage($row['jps_perc_betting_d'],'numeric');
                    $curDProc=$this->getBettingPercentage($row['jps_perc_betting_d']);

                    if($prevD!=$curD && $prevD!=''){
                        if($prevDTotR<0 && $prevDProfile==3){
                            $prevRTotR+=$prevDTotR;
                        }
                        else{
                            $this->totals['district']+=$prevDTotR;
                        }
                        $prevDTotR=0;
                    }
                    if($prevR!=$curR && $prevR!=''){
                        if($prevRTotR<0 && $prevRProfile==3){
                            $prevNTotR+=$prevDTotR;
                        }
                        else{
                            $this->totals['region']+=$prevRTotR;
                        }
                        $prevRTotR=0;
                    }
                    if($prevN!=$curN && $prevN!=''){
                        if($prevNTotR<0 && $prevNProfile==3){
                            $caTot+=$prevNTotR;
                        }
                        else{
                            $this->totals['nation']+=$prevNTotR;
                        }
                        $prevNTotR=0;
                    }
                    if($netTot<0){
                        if($curNProfile==3){
                            $row['jps_perc_betting_n']=$this->changeBettingType($row['jps_perc_betting_n'],1);
                        }
                        $prevNTotR+=$this->calculateBettingRake($row,'nation',1,1);

                        if($curRProfile==3){
                            $row['jps_perc_betting_r']=$this->changeBettingType($row['jps_perc_betting_r'],1);
                        }
                         $prevRTotR+=$this->calculateBettingRake($row,'region',1,1);

                        if($curDProfile==3){
                            $row['jps_perc_betting_d']=$this->changeBettingType($row['jps_perc_betting_d'],1);
                        }
                        $prevDTotR+=$this->calculateBettingRake($row,'district',1,1);
                    }
                    else{
                        $prevNTotR+=$this->calculateBettingRake($row,'nation',1);
                        $prevRTotR+=$this->calculateBettingRake($row,'region',1);
                        $prevDTotR+=$this->calculateBettingRake($row,'district',1);
                    }
                    $caTot+=$this->calculateBettingRake($row,'casino',1);
                    $this->totals['bonus']+=$this->calculateBonus($row);
                    $prevD=$curD;
                    $prevDProfile=$curDProfile;
                    $prevR=$curR;
                    $prevRProfile=$curRProfile;
                    $prevN=$curN;
                    $prevNProfile=$curNProfile;
                    foreach($row as $key => $val){
                        if(is_numeric($val)){
                            $this->totals[$key] += $val;
                        }
                    }
                }
                    if($prevDTotR<0 && $prevDProfile==3){
                        $prevRTotR+=$prevDTotR;
                    }
                    else{
                        $this->totals['district']+=$prevDTotR;
                    }

                    if($prevRTotR<0 && $prevRProfile==3){
                        $prevNTotR+=$prevRTotR;
                    }
                    else{
                        $this->totals['region']+=$prevRTotR;
                    }
                    if($prevNTotR<0 && $prevNProfile==3){
                        $this->totals['casino']+=$prevNTotR;
                    }
                    else{
                        $this->totals['nation']+=$prevNTotR;
                    }

                $this->totals["casino"]+=$caTot;
                $this->totalsRakeSum +=($this->totals['betting_rake']);
                $this->totalsBetSum  +=$this->totals['betting_bet'];
                $this->totalsWinSum  +=$this->totals['betting_win'];
                $this->totalsBonus   +=$this->totals['bonus'];
                $this->totalsRakeSum +=($this->totals['betting_rake_live']);
                $this->totalsBetSum  +=$this->totals['betting_bet_live'];
                $this->totalsWinSum  +=$this->totals['betting_win_live'];
    }




  /**
* @param $percentage
* @param bool|false $onlytype
 * @return array
*/
 public function getBettingPercentage($percentage,$onlytype=false){

    list($type,$proc)=explode(";",$percentage);
    list($typename,$proctype)=explode(":",$type);
    list($name,$procent)=explode(":",$proc);
    if($onlytype){
        if($onlytype=='type'){
            return $this->bettingProfilesTypes[$proctype]['name'];
        }
        return $proctype;
    }
    else{
        if($proctype==4 || $proctype==8){
            $procent=explode(',',$procent);
            $procent=$procent[0];
        }
        return $procent;
    }
}

public function calculateBonus($rec){

    if(!$this->isValidBettingPeriod($this->start,$this->end)){
        return 0;
    }
    $rake=$rec["betting_rake"]+$rec["betting_rake_live"];
    $net=$rec["betting_bet"]+$rec["betting_bet_live"]- $rec["betting_win"]-$rec["betting_win_live"];
    if($this->getBettingPercentage($rec['jps_perc_betting_c'],"numeric")==4){
        $proc=$this->getBettingPercentage($rec['jps_perc_betting_c']);
        if(($proc*$net/100)<$rake){
            $proc=0;
        }
        else{
            $proc=($proc*$net/100)-$rake;
        }
    }
    else{
        $proc=0;
    }
    return $proc;
}


/**
 * @param $rec
 * @param $class
 * @param int $returnType
 * @return float|string
 */
public function calculateBettingRake($rec ,$class, $returnType = 0,$includeChild=0){
    $netbet=(($rec["betting_bet"]-$rec['betting_win'])-($rec['betting_rake'])-$this->calculateBonus($rec))+($rec["betting_bet_live"]-$rec['betting_win_live'])-($rec['betting_rake_live']);
    $nationPerc    = $this->getBettingPercentage($rec["jps_perc_betting_n"]);
    $casinoPerc    = 100-$nationPerc;
    $regionPerc    = $this->getBettingPercentage($rec["jps_perc_betting_r"]);
    $districtPerc  = $this->getBettingPercentage($rec["jps_perc_betting_d"]);

    $casinoProfileType    = $nationProfileType    = $this->getBettingPercentage($rec["jps_perc_betting_n"],"numeric");
    $regionProfileType    = $this->getBettingPercentage($rec["jps_perc_betting_r"],"numeric");
    $districtProfileType  = $this->getBettingPercentage($rec["jps_perc_betting_d"],"numeric");

    if($class=='casino'){
        $perctoret = $casinoPerc." % = ";
        if($includeChild==0){
            if($netbet<0 && $nationProfileType==3){
                $nationPerc=0;
            }
            if($netbet<0 && $regionProfileType==3){
                $regionPerc=0;
            }
            if($netbet<0 && $districtProfileType==3){
                $districtPerc=0;
            }
        }
        eval('$percvalue='.$this->bettingProfilesTypes[$casinoProfileType]['formulas']['casino'].";");
    }
    elseif($class=='nation'){
        $perctoret = $nationPerc."% = ";
        if($includeChild==0){
            if($netbet<0 && $regionProfileType==3){
                $regionPerc=0;
            }
            if($netbet<0 && $districtProfileType==3){
                $districtPerc=0;
            }
        }
        eval('$percvalue='.$this->bettingProfilesTypes[$nationProfileType]['formulas']['nation'].";");
    }
    elseif($class=='region'){
        $perctoret = $regionPerc."% = ";
        if($includeChild==0){
            if($netbet<0 && $districtProfileType==3){
                $districtPerc=0;
            }
        }
        eval('$percvalue='.$this->bettingProfilesTypes[$regionProfileType]['formulas']['region'].";");
    }
    elseif($class=='district'){
        $perctoret = $districtPerc."% = ";
        eval('$percvalue='.$this->bettingProfilesTypes[$districtProfileType]['formulas']['district'].";");
    }
    elseif($class=='club'){
        $perctoret = "%=";
        $percvalue = 0;
    }
    if($returnType == 0){
        return $perctoret.getInDollars($percvalue);
    }
    if($returnType == 1){
        return $percvalue;
    }
    if($returnType == 2){
        return $perctoret.($percvalue);
    }
 }

 private  function changeBettingType($perc,$newType){
    if($perc!=''){
        $bettingType=explode(";",$perc);
        $type=explode(':',$bettingType[0]);
        $type[1]=$newType;
        $type=implode(":",$type);
        $bettingType[0]=$type;
        $bettingType=implode(';',$bettingType);
    }
    else{
        $bettingType=0;
    }
    return $bettingType;
}

public function getTotalsByClass($class,$withCurrency=false){
   if($class=='casino') {
        $total=$this->totals['casino'];
      }
   if($class=='nation') {
        $total=$this->totals['nation'];
    }
    if($class=='region') {
        $total=$this->totals['region'];
    }
    if($class=='district') {
        $total=$this->totals['district'];
     }
    if($class=='club') {
        $total=$this->totalsRakeSum+$this->totalsBonus;
    }
    if($withCurrency){
              return getInDollars($total);
    }
   return $total;

}

}