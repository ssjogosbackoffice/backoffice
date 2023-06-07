<?php
require_once 'Jurisdiction.class.inc';
require_once 'account_functions.php';

/**
 * Created by PhpStorm.
 * User: marian
 * Date: 15/04/16
 * Time: 15:37
 */
class EntityCreditsHandler extends Jurisdiction{
    /**
     * @param Jurisdiction $to
     * @param $amount
     * @param bool $note
     * @return bool
     */
    public function sendCredits(EntityCreditsHandler $to, $amount, $note=false){
        global $dbh,$dbLog,$lang;
       /* $this->getJurisdictionInfo();
        $to->getJurisdictionInfo();*/
        $dbh->begin();
        $supresetov=0;
        if($this->amountExceedsCredit($amount)){
            $dbh->rollback();
            return false;
        }
        if((strtotime('now')-strtotime($this->getOverdraftStartTime())> (3600*24*7)) &&
            $this->getClass()!='casino' && $this->getAvailableCredit()<=0 && $this->getOverdraftStartTime()!=''){
            $dbh->rollback();
            addError("", '-136');
            return false;
        }else{
            $dbLog->info('Starting send '.$amount. " to ".$to->getName());
            $transfer_type=19;
            $jurisdictionsBetween=$this->getAllEntitiesBetween($to);
            $keys = array_keys($jurisdictionsBetween);
            for ($i = 0; $i < count($keys); $i++) {
                $cur = $jurisdictionsBetween[$keys[$i]];
                $next = $jurisdictionsBetween[$keys[$i+1]];
                if($next!=''){
                    if(!is_int(adminUserCreditLogMultiple(
                            $note,
                            $transfer_type,
                            $_SESSION['admin_id'],
                            $cur,
                            $next,
                            $amount,
                            0,
                            $i)
                    )){
                        $dbLog->info($lang->getLang(Error::$errors['-10008']));
                        addError('','-10008');
                        $dbh->rollback();
                        return false;
                    };
                }
            }

            if($this->getAvailableCredit()<$amount){
                if($this->getAvailableCredit()<0){
                    $this->setOverdraft(($this->getOverdraft()-$amount));
                }
                else{
                    if((strtotime('now')-strtotime($this->getOverdraftStartTime())> (3600*24*7)) && $this->getClass()!='casino'){
                        $supresetov=1;
                    }
                    $this->setOverdraft($this->getOverdraft()+($this->getAvailableCredit()-$amount));
                }
                $this->setAvailableCredit($this->getAvailableCredit()-$amount);
            }
            else{
                $this->setAvailableCredit($this->getAvailableCredit()-$amount);
            }
            if($to->getAvailableCredit()<0){
                $max_overdraft=$to->getTotalOverdraftReceived()-$to->getOverdraft();
                if($max_overdraft>0){
                    if($to->getAvailableCredit()+$amount<0){
                        if($max_overdraft>$amount){
                            $to->increaseOverdraft($amount);
                        }
                        else{
                            $to->increaseOverdraft($max_overdraft);
                        }
                    }
                    else{
                        if($max_overdraft>(-$to->getAvailableCredit())){
                            $to->increaseOverdraft(-$to->getAvailableCredit());
                        }
                        else{
                            $to->increaseOverdraft($max_overdraft);
                        }
                    }
                }
            }
            $to->setAvailableCredit($to->getAvailableCredit()+$amount);
            $fromUpdated = $this->updateJurisdictionCredits($this->getAvailableCredit(),$this->getOverdraft(),false,$supresetov);
            $toUpdated   = $to->updateJurisdictionCredits($to->getAvailableCredit(),$to->getOverdraft(),false,0);
            if($fromUpdated&&$toUpdated){
                $dbLog->info('Transaction successfully');
                $dbh->commit();
                return true;
            }
        }
        $to->getJurisdictionInfo();
        $dbh->rollback();
        return false;
    }


    /**
     * @param EntityCreditsHandler $to
     * @param $amount
     * @param bool $note
     * @return bool
     */
    public function withdrawCredits(EntityCreditsHandler $to, $amount, $note=false){
        global $dbh,$dbLog,$lang;
        $transfer_type=20;
        $jurisdictionsBetween=$this->getAllEntitiesBetween($to);
        $jurisdictionsBetween=array_reverse($jurisdictionsBetween);
        $keys = array_keys($jurisdictionsBetween);
        for ($i = 0; $i < count($keys); $i++) {
            $cur = $jurisdictionsBetween[$keys[$i]];
            $next = $jurisdictionsBetween[$keys[$i+1]];
            if($next!=''){
                if(!is_int(adminUserCreditLogMultiple(
                        $note,
                        $transfer_type,
                        $_SESSION['admin_id'],
                        $next,
                        $cur,
                        -$amount,
                        0,
                        $i)
                )){
                    $dbLog->info($lang->getLang(Error::$errors['-10008']));
                    addError('',$lang->getLang(Error::$errors['-10008']));
                    $dbh->rollback();
                    return false;
                };
            }
        }

        $to->setAvailableCredit($to->getAvailableCredit()-$amount);
        if($this->getAvailableCredit() < 0){
            $max_overdraft=$amount;
            if($this->getOverdraft()!=$this->getTotalOverdraftReceived()){
                if(($this->getOverdraft()+$max_overdraft) > $this->getTotalOverdraftReceived()){
                    $this->setOverdraft($this->getTotalOverdraftReceived());
                }
                else{
                    $this->increaseOverdraft($max_overdraft);
                }
            }
        }
        $this->available_credit+=$amount;

        $fromUpdated = $this->updateJurisdictionCredits($this->getAvailableCredit(),$this->getOverdraft(),$this->getTotalOverdraftReceived(),0);
        $toUpdated   = $to->updateJurisdictionCredits($to->getAvailableCredit(),$to->getOverdraft(),$to->getTotalOverdraftReceived(),0);
        if($fromUpdated&&$toUpdated){
            $dbh->commit();
            return true;
        }
        $to->getJurisdictionInfo();
        $dbh->rollback();
        return false;
    }

    /**
     * @param $amount
     * @return bool
     */
    private function amountExceedsCredit($amount){
        global $lang;
        if (  $this->id!= 1 && $amount > ($this->getAvailableCredit()+$this->getOverdraft()) && $amount > ($this->getAvailableCredit()) && $amount > ($this->getOverdraft())){
            addError("", $lang->getLang(Error::$errors['-10007']));
            return true;
        }
        return false;
    }


    /**
     * @param $amount
     * @param $overdraft
     * @param bool $totalOverdraft
     * @param int $resetOverdraft
     * @return bool
     */
    public function updateJurisdictionCredits($amount, $overdraft, $totalOverdraft=false, $resetOverdraft=0){
        global $dbh;
        if(!$amount || $amount==''){
            $amount=0;
        }
        $sql = "UPDATE jurisdiction " .
            "SET jur_cash_in_hand = ".$dbh->prepareString($amount)."," .
            "    jur_available_credit = ".$dbh->prepareString($amount) ;
        if($overdraft || $overdraft==0){
            $sql.=" ,jur_overdraft = ".$dbh->prepareString($overdraft);
        }
        if($totalOverdraft!=false){
            $sql.= " ,jur_tot_overdraft_received =".$dbh->prepareString($totalOverdraft);
            if($totalOverdraft!=$this->getTotalOverdraftReceived()){
                doAdminUserLog($_SESSION['admin_id'],'overdraft modify','Changed total overdraft received  from '.$this->getTotalOverdraftReceived().' to '.$totalOverdraft.' for jurisdiction '.$this->getName(),'NULL',getIpAddress());
            }
        }
        if($resetOverdraft!=0){
            if($resetOverdraft==1){
                $sql.=",jur_overdraft_start_time= now() ";
                doAdminUserLog($_SESSION['admin_id'],'overdraft modify','Changed overdraft time from '.$this->getOverdraftStartTime().' to NOW() for jurisdiction '.$this->getName(),'NULL',getIpAddress());
            }
            else{
                $sql.=",jur_overdraft_start_time= NULL ";
                doAdminUserLog($_SESSION['admin_id'],'overdraft modify','Reset overdraft time for jurisdiction '.$this->getName(),'NULL',getIpAddress());
            }
        }
        $sql .=" WHERE jur_id = ".$this->id;
        $rs=$dbh->exec($sql);
        return ( 1 == count($rs) );
    }
}