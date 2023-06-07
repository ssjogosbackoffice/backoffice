<?php



  /**
 * Class Financial
 */

  class Financial{
     public $level;
     public $start;
     public $end;
     public $jurisdictionClass;
     public $jurisdictionId;
     public $jur_totals;
     public $result;
     public $fin;
     public $totals=array();
     public $totalsRakeSum=0;
     public $totalsBetSum=0;
     public $totalsWinSum=0;
     public $totalsFinSum=0;
     public $totalsBonus=0;
     public $totalsBonusFin=0;
     public $totalsPerc=array();
     public $bettingProfilesTypes=array();

    protected function getBettingProfiles(){
        global $dbh;
        $sql="Select * from ProfiliCommissioniTipi";
        $result=$dbh->exec($sql,false,true);
        return $result;
    }






     public $query_jur_selectors = array(
    "nation" => "n.jur_name n_jur_name",
    "region" => "n.jur_name n_jur_name, r.jur_name r_jur_name",
    "district" => "n.jur_name n_jur_name, r.jur_name r_jur_name, d.jur_name d_jur_name",
    "club" => "n.jur_name n_jur_name, r.jur_name r_jur_name, d.jur_name d_jur_name, c.jur_name c_jur_name"
    );
    public $query_jur_orders = array(
        "nation" => "n.jur_name",
        "region" => "n.jur_name, r.jur_name",
        "district" => "n.jur_name, r.jur_name, d.jur_name",
        "club" => "n.jur_name, r.jur_name, d.jur_name, c.jur_name"
    );
    public $query_jur_groups = array(
        "nation" => "n.jur_id",
        "region" => "n.jur_id, r.jur_id",
        "district" => "n.jur_id, r.jur_id, d.jur_id",
        "club" => "n.jur_id, r.jur_id, d.jur_id, c.jur_id"
    );
    public $query_jur_payments = array(
        "nation" => "n.jur_id",
        "region" => "r.jur_id",
        "district" => "d.jur_id",
        "club" => "c.jur_id"
    );
    public $query_jur_currency = array(
        "nation" => "n.jur_currency",
        "region" => "r.jur_currency",
        "district" => "d.jur_currency",
        "club" => "c.jur_currency"
    );


    protected function getStartJurisdictionLevel(){
        if($_SESSION['jurisdiction_class'] == 'casino'){
            return 'nation';
        }
        if($_SESSION['jurisdiction_class'] == 'nation'){
            return 'region';
        }
        if($_SESSION['jurisdiction_class'] == 'region'){
            return 'district';
        }
        if($_SESSION['jurisdiction_class'] == 'district'){
            return 'club';
        }
    }

 public function isValidBettingPeriod(){
    $month=date('Y-m',strtotime($this->start));
    if(date('D',strtotime($month))=='Tue'  && $month.'-01'==$this->start){
        $lastTuesday = $month.'-01';
        $lastDay = date('Y-m-d',strtotime("Last Monday of $month"));
    }
    else {
        $nextMonth=date('Y-m',strtotime($month.' +1 month'));
        $lastTuesday = date('Y-m-d',strtotime("Last Tuesday of $month"));
        $lastDay = date('Y-m-d',strtotime("Last Monday of $nextMonth"));
    }

    if($lastTuesday==$this->start && $lastDay==$this->end){

        return true;
    }
   return false;
}
}



?>
