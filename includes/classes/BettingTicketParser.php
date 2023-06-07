<?php
/**
 * Created by PhpStorm.
 * User: marian
 * Date: 15.12.2017
 * Time: 18:35
 */
list($user_betting, $pass_betting, $host_betting, $db_betting) = explode(":",$_SERVER["BETTING_DSN"]);
Db::setConnectionInfo($db_betting,$user_betting,$pass_betting,"mysql",$host_betting);
class BettingTicketParser
{
    //function preprint($obj){
        //print "<pre>". print_r($obj, true) ."</pre>";
    //}

    private $xml;
    private $xmlString;
    private $userID,$amount,$state,$TAccreditato,$TStampAccreditato,$win,$paid,$betModeType,$ticketCode,
        $ip,$note,$customerTransactionsID,$refundAmount;
    private $eventList=[];
    private $availableTypes=[
        "PlaceBet"=>"0",
        "AwardWinnings"=>"1",
        "RefundBet"=>"2",
        "LossSignal"=>"1"
    ];

    private $availableBetModesTypes=[
        "PreLive"=>0,
        "Live"=>1,
        "Mixed"=>2
    ];

    public function __construct($xmlString){
        $this->xmlString=$xmlString;
        try {
            //$xmlString = file_get_contents($filePath);
            if ($xmlString != '') {
                $this->xml = new SimpleXMLElement($xmlString);
                $this->prepareData();
            }
            else{
                throw new Exception("Invalid xml");
            }
        }
        catch (Exception $e) {
            die($e->getMessage());
        }
    }

    private function prepareData(){
        //echo "entered";
        //var_dump($this->xml);
        //$test = 'Select pun_id from punter where pun_customer_number='.(string)$this->xml->Method->Params->ExternalUserID->attributes()->Value[0];
        //$test =  (string)$this->xml->Method->Params->ExternalUserID->attributes()->Value[0];

        $this->userID=DB::getValue('Select pun_id from punter where pun_customer_number='.(string)$this->xml->Method->Params->ExternalUserID->attributes()->Value[0]);
        //var_dump($test);
        //echo "Here tooo!";

        if(isset($this->xml->Method->Params->BetAmount)) {
            $this->amount = number_format(((string)$this->xml->Method->Params->BetAmount->attributes()->Value[0])/100,2);
        }

        $this->state=$this->availableTypes[(string)$this->xml->Method->attributes()->Name];
        if((string)$this->xml->Method->attributes()->Name=='AwardWinnings'){
            $this->win=number_format(((string)$this->xml->Method->Params->WinAmount->attributes()->Value)/100,2);
        }

        if((string)$this->xml->Method->attributes()->Name=='RefundBet'){
            $this->refundAmount= number_format(((string)$this->xml->Method->Params->RefundAmount->attributes()->Value)/100,2);
        }

        $this->TAccreditato=($this->state!='0'?'1':0);
        $this->TStampAccreditato=date('Y-m-d H:i:s');

        if($this->win>0 && $this->state!=1){
            $this->paid=1;
        }

        $this->betModeType=$this->availableBetModesTypes[(string)$this->xml->Method->Params->BetMode->attributes()->Value];
        if(isset($this->xml->Method->Params->BetReferenceNum)) {
            $this->ticketCode = (string)$this->xml->Method->Params->BetReferenceNum->attributes()->Value;
        }
        elseif (isset($this->xml->Method->Params->WinReferenceNum->attributes()->Value)) {
            $this->ticketCode = (string)$this->xml->Method->Params->WinReferenceNum->attributes()->Value;
        }
        $this->customerTransactionsID=(string)$this->xml->Method->Params->TransactionID->attributes()->Value;
        if(isset($this->xml->Method->Params->ClientIP)) {
            $this->ip = (string)$this->xml->Method->Params->ClientIP->attributes()->Value;
        }

     /*   if (isset($this->xml->Method->Params->Bet->Events)) {
            $this->note = (string)$this->xml->Method->Params->Bet->Events->attributes()->Value;
        }*/
        if (isset($this->xml->Method->Params->Bet->EventList)) {
            foreach($this->xml->Method->Params->Bet->EventList->Event as $k=>$v){
                foreach($v->Market as $vmk=>$vmv) {

                    $section = [];
                    $section['IDBet'] = 0;
                    $section['IDAuto'] = 0;
                    $section['Variabile'] = 0;
                    if (isset($v->EventID)) {
                        $section['IDEvento'] = (string)$v->EventID->attributes()->Value;
                    }
                    if (isset($vmv->Odds)) {
                        $section['Quota'] = (string)$vmv->Odds->attributes()->Value;
                    }

                    $section['DescrizioneEvento'] = (string)$v->attributes()->Value;
                    if (isset($v->EventDate)) {
                        $section['DataEvento'] = (string)$v->EventDate->attributes()->Value;
                    }
                        $section['DescrizioneTipo'] = (string)$vmv->attributes()->Value;
                    if (isset($vmv->Outcome)) {
                        $section['DescrizioneProno'] = (string)$vmv->Outcome->attributes()->Value;
                    }
                    array_push($this->eventList, $section);
                }
            }
        }
    }

    private function prepareTicketUpdate(){

        $fields=['IDUtente'=>$this->userID,
            'TStato'=>$this->state,
            'TAccreditato'=>$this->TAccreditato,
            'IDTipoTicket'=>$this->betModeType,
            'CodiceTicket'=>$this->ticketCode,
            'CustomerTransactionsID' =>$this->customerTransactionsID,
            'LastUpdate'=>date('Y-m-d H:i:s')
        ];
        if($this->ip!=''){
            $fields['IPAddress'] = $this->ip;
        }
        if($this->note!=''){
            $fields['Note'] = $this->note;
        }

        if($this->state==0 ) {
            $fields['Importo'] = $this->amount;
        }
        elseif($this->state==1){
            $fields['TStampAccredito']=$this->TStampAccreditato;
            if($this->win>0){
                $fields['TVincita']=$this->win;
            }
            if($this->paid>0){
                $fields['Pagato']=$this->paid;
            }
        }else if($this->state==2) {
            $fields['TStampAccredito']=$this->TStampAccreditato;
            if ($this->win > 0) {
                $fields['TVincita'] = $this->win;
            }
            if ($this->paid > 0) {
                $fields['Pagato'] = $this->paid;
            }
        }
        return $fields;
    }

    /**
     * Method to update ticket; if not result is found, than an insert is made
     */
    public function updateTicket(){
        DB::beginTransaction();
        $fields=$this->prepareTicketUpdate();
        $sql=" Update Ticket_dev set ";
        foreach ($fields as $k=>$v){
            if($k=='CustomerTransactionsID'){
                $sql .= $k . "= concat(CustomerTransactionsID,'," .$v . "'),";
            }
            else {
                $sql .= $k . "=" . DB::prepareString ($v) . ',';
            }
        }
        if($this->state==2){
            $sql .=  "Note = concat(Note,' BetAmount " . $this->refundAmount . "'),";
        }
        $sql=substr($sql,0,-1);
        $sql.=" WHERE CodiceTicket = ".DB::prepareString ($this->ticketCode);
         $update=DB::execute($sql);
        if($update){
            $this->updateSingleEvents();
            DB::commitTransaction();
            return $update;
        }
        else{
            return $this->save();
        }
    }

    private function updateSingleEvents(){
        $ticketId=DB::getValue('Select IDTicket from Ticket_dev where CodiceTicket='.$this->ticketCode);
        $sql="Update Scommesse1 set Stato=".$this->state.", Accreditato=".$this->TAccreditato.",TStamp=".DB::prepareString($this->TStampAccreditato)." WHERE IDTicket=".$ticketId;
        return DB::execute($sql);
    }


    public function save(){

        $fields=$this->prepareTicketUpdate();
        $sql=" Insert into Ticket_dev ( ";
        $values=' Values (';
        foreach ($fields as $k=>$v){
            $sql.=$k.",";
            $values.=DB::prepareString ($v).',';
        }
        $sql=substr($sql,0,-1);
        $values=substr($values,0,-1);
        $sql.=") ". $values." )";
        $result=DB::execute($sql);
         try{
            if($result){
                $this->saveIndividualEvents();
                DB::commitTransaction();
            }
            else{
                DB::rollbackTransaction();
                $this->saveToFile();
            }
        }
        catch (Exception $e){
            DB::rollbackTransaction();
            $this->saveToFile();
            $result=false;
        }
        return $result;
    }


    private function saveIndividualEvents(){
        $sql=" Insert into Scommesse1 (IDTicket,Stato,Accreditato,DescrizioneCategoria,";
        $first=0;
        $values=' Values' ;

        foreach ($this->eventList as $kEv=>$event){
            $values.='('.DB::prepareString (DB::getLastInsertId()) . ','.$this->state.",".$this->TAccreditato.",NULL,";
            foreach($event as $k=>$v) {
                if($first<1) {
                    $sql .= $k . ",";
                }
                $values.=DB::prepareString ($v).',';
            }
            $values=substr($values,0,-1).'),';
            $first++;

        }
        $sql=substr($sql,0,-1);
        $values=substr($values,0,-1);
        $sql.=") ". $values;
        $result=DB::execute($sql);
        return $result;
    }

    private function saveToFile(){
        $filename=WEB_ADMIN."/documents/bettingParser/".$this->ticketCode.".xml";
        if(!is_dir(dirname($filename))){
            mk_dir(dirname($filename));
        }
        $file=fopen($filename, "w");
        fwrite($file, $this->xmlString);
        fclose($file);
    }
}
?>



