<?php

  define("SERVER_IP" , "localhost");
  define("SERVER_PORT", 50001);
  
  define("SERVER_USER" , "123456");
  define("MESSAGE_COUNTER" , 10000);
  
  define("REQ_USER_ONLINE"      , 1); // Numero di utenti online
  define("REQ_CASH"             , 2); // Cassa del singolo club
  define("REQ_BW_SINGLE_PLAYER" , 3); // Bet and Win del singolo giocatore
  define("REQ_BW_TOTAL_PLAYER"  , 4); // Bet and Win dei giocatori totali
  define("REQ_USER_LIST"        , 5); // Richiesta della lista degli utenti
  define("REQ_TOT_BET_WIN"      , 6); // Richiesta del totale bet e totale win
  define("REQ_NUMB_OF_ITER"     , 7); // Richiesta del numero di partite fatte
  define("REQ_GAME_LIST"        , 8); // Richiesta lista giochi
  define("REQ_GAME_STATUS"      , 9); // Richiesta staus spegnimento accensione
  define("REQ_USER_KILLER"      ,10); // Richiesta per killare il giocatore
  
  
  define("MAFIA" , 1001);
  define("VIDEOPOKER" , 2001);
  define("POKERCOMMONDRAW", 3001);
  define("BORUT", 1003);
  
  define("ERR_REQUEST_NULL"   , -1);
  define("ERR_SOCKET_OPEN"    , -2);
  define("ERR_LENGTH_MESSAGE" , -3);

  if (!class_exists('Communicate')) {
    
    class Communicate{
        
         var $request;
         var $response;
         var $errno;
         var $errnum;
         var $timeout;
         var $responseMessage;
         var $sock;
         var $lastTypeSent;
        
        public function setRequest($game , $type , $msg){
            
            $this->lastTypeSent = $type;
            $this->request = SERVER_USER . " " . MESSAGE_COUNTER ." " . $game . " " . $type . " " .  $msg ."\n";
            //echo $this->request;
             
        }
        
        public function getLastTypeSent(){
            return $this->lastTypeSent;
        }
        
        public function setTimeout($timeout){
            $this->timeout = $timeout;
        }
        
        public function getErrorMessage(){
            $this->errmsg;
        }
        
        public function getErrorNumber(){
            $this->errnum;
        }
        
        public function sendRequest(){
            if($this->request != ""){
              if($this->sock = fsockopen(SERVER_IP, SERVER_PORT, $this->errnum, $this->errmsg, $this->timeout)){
                 
                 fwrite($this->sock, $this->request);
                 
                 $this->receiveResponse();
                 
                 return $this->getResponseMessage();
                 
                 fclose($this->sock);
              }else{
                 return ERR_SOCKET_OPEN;
              }  
            }else{
               return ERR_REQUEST_NULL;
            }
        }
        
        public function receiveResponse(){
            $this->response = fgets($this->sock , 4096);
            
            $this->parseMessage();
        }
        
        public function getResponse(){
            return $this->response;
        }
        
        public function getResponseMessage(){
            return $this->responseMessage;
        }
        
        public function parseMessage(){
            
            $count = strlen($this->response);
            
            if($count > 0){
               $dataList = split(" " , $this->response);
               $this->responseMessage = $dataList[count($dataList) - 1];
               
            }else{
               return ERR_LENGTH_MESSAGE;
            } 
            
        }
    }
  
  }

/*
function getResponseForMessage($request){
  if($sock = fsockopen("10.10.0.11", 50001, $errno, $errstr, 5)){
  
    fwrite($sock, $request);
    
    $data = fgets($sock,2048);
    
    
    $count = strlen($data);
     
    if($count > 0){
      $tmp = split(" ",$data);
      
      $dataList["srvuser"] = $tmp[0];
      $dataList["counter"] = $tmp[1];
      $dataList["games"]   = $tmp[2];
      $dataList["type"]    = $tmp[3];
      $dataList["request"] = $tmp[4];
      $dataList["response"] = $tmp[5];
      return $dataList;
    }else{
      return "Nessun Messaggio Ricevuto";
    }
    
    fclose($sock);
  } 
}

*/

?>

