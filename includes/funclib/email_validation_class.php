<?php
/*
 * email_validation.php
 *
 * @(#) $Header: /srv/cvsroot/europa/funclib/email_validation_class.php,v 1.1.1.1 2006/02/15 10:36:10 giusc Exp $
 *
 */

define('EMAIL_ERR_BAD_FORMAT', 1);
define('EMAIL_ERR_HOST_NO_MBOX', 2);
define('EMAIL_ERR_HOST_NO_RESOLV', 3);
define('EMAIL_ERR_HOST_NO_CONN', 4);
 
class email_validation_class
{
	var $email_regular_expression="^([-!#\$%&'*+./0-9=?A-Z^_`a-z{|}~])+@([-!#\$%&'*+/0-9=?A-Z^_`a-z{|}~]+\\.)+[a-zA-Z]{2,6}\$";
	var $timeout=0;
	var $data_timeout=0;
	var $localhost="";
	var $localuser="";
	var $debug=0;
	var $html_debug=0;
	var $exclude_address="";
	var $getmxrr="GetMXRR";
	var $error_code;

	var $next_token="";
	var $preg;

	Function Tokenize($string,$separator="")
	{
		if(!strcmp($separator,""))
		{
			$separator=$string;
			$string=$this->next_token;
		}
		for($character=0;$character<strlen($separator);$character++)
		{
			if(GetType($position=strpos($string,$separator[$character]))=="integer")
				$found=(IsSet($found) ? min($found,$position) : $position);
		}
		if(IsSet($found))
		{
			$this->next_token=substr($string,$found+1);
			return(substr($string,0,$found));
		}
		else
		{
			$this->next_token="";
			return($string);
		}
	}

	Function OutputDebug($message)
	{
		$message.="\n";
		if($this->html_debug)
			$message=str_replace("\n","<br />\n",HtmlEntities($message));
		echo $message;
		flush();
	}

	Function GetLine($connection)
	{
		for($line="";;)
		{
			if(feof($connection))
				return(0);
			$line.=fgets($connection,100);
			$length=strlen($line);
			if($length>=2
			&& substr($line,$length-2,2)=="\r\n")
			{
				$line=substr($line,0,$length-2);
				if($this->debug)
					$this->OutputDebug("S $line");
				return($line);
			}
		}
	}

	Function PutLine($connection,$line)
	{
		if($this->debug)
			$this->OutputDebug("C $line");
		return(fputs($connection,"$line\r\n"));
	}

	
	function ValidateEmailAddress($email)
	{
		if(IsSet($this->preg))
		{
			if(strlen($this->preg))
			{	
				$match = preg_match($this->preg,$email);
				
				if ( ! $match )
				   $this->error_code = EMAIL_ERR_BAD_FORMAT;
				   
				return $match;			
			}
		}
		else
		{
			$this->preg=(function_exists("preg_match") ? "/".str_replace("/", "\\/", $this->email_regular_expression)."/" : "");
			return($this->ValidateEmailAddress($email));
		}
		return(eregi($this->email_regular_expression,$email)!=0);
	}
	

	Function ValidateEmailHost($email,&$hosts)
	{		
		if(!$this->ValidateEmailAddress($email))
		{   
			return(0);
		}
		
		$user=$this->Tokenize($email,"@");
		$domain=$this->Tokenize("");
		$hosts=$weights=array();
		$getmxrr=$this->getmxrr;
		if(function_exists($getmxrr)
		&& $getmxrr($domain,$hosts,$weights))
		{
			$mxhosts=array();
			for($host=0;$host<count($hosts);$host++)
				$mxhosts[$weights[$host]]=$hosts[$host];
			KSort($mxhosts);
			for(Reset($mxhosts),$host=0;$host<count($mxhosts);Next($mxhosts),$host++)
				$hosts[$host]=$mxhosts[Key($mxhosts)];
		}
		else
		{
			if(strcmp($ip=@gethostbyname($domain),$domain)
			&& (strlen($this->exclude_address)==0
			|| strcmp(@gethostbyname($this->exclude_address),$ip)))
				$hosts[]=$domain;
		}
		
		if ( count($hosts) < 1 )
		   $this->error_code = EMAIL_ERR_NO_HOST;
		   	
		return(count($hosts)!=0);
	}

	Function VerifyResultLines($connection,$code)
	{
		while(($line=$this->GetLine($connection)))
		{
			if(!strcmp($this->Tokenize($line," "),$code))
				return(1);
			if(strcmp($this->Tokenize($line,"-"),$code))
				return(0);
		}
		return(-1);
	}

	Function ValidateEmailBox($email)
	{
		if(!$this->ValidateEmailHost($email,$hosts))
			return(0);
		if(!strcmp($localhost=$this->localhost,"")
		&& !strcmp($localhost=getenv("SERVER_NAME"),"")
		&& !strcmp($localhost=getenv("HOST"),""))
		   $localhost="localhost";
		if(!strcmp($localuser=$this->localuser,"")
		&& !strcmp($localuser=getenv("USERNAME"),"")
		&& !strcmp($localuser=getenv("USER"),""))
		   $localuser="root";
		for($host=0;$host<count($hosts);$host++)
		{
			$domain=$hosts[$host];
			if(ereg('^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$',$domain))
				$ip=$domain;
			else
			{
				if($this->debug)
					$this->OutputDebug("Resolving host name \"".$hosts[$host]."\"...");
				if(!strcmp($ip=@gethostbyname($domain),$domain))
				{
					if($this->debug)
						$this->OutputDebug("Could not resolve host name \"".$hosts[$host]."\".");
					continue;
				}
			}
			if(strlen($this->exclude_address)
			&& !strcmp(@gethostbyname($this->exclude_address),$ip))
			{
				if($this->debug)
					$this->OutputDebug("Host address of \"".$hosts[$host]."\" is the exclude address");
				continue;
			}
			if($this->debug)
				$this->OutputDebug("Connecting to host address \"".$ip."\"...");
			if(($connection=($this->timeout ? @fsockopen($ip,25,$errno,$error,$this->timeout) : @fsockopen($ip,25))))
			{
				$timeout=($this->data_timeout ? $this->data_timeout : $this->timeout);
				if($timeout
				&& function_exists("socket_set_timeout"))
					socket_set_timeout($connection,$timeout,0);
				if($this->debug)
					$this->OutputDebug("Connected.");
				if($this->VerifyResultLines($connection,"220")>0
				&& $this->PutLine($connection,"HELO $localhost")
				&& $this->VerifyResultLines($connection,"250")>0
				&& $this->PutLine($connection,"MAIL FROM: <$localuser@$localhost>")
				&& $this->VerifyResultLines($connection,"250")>0
				&& $this->PutLine($connection,"RCPT TO: <$email>")
				&& ($result=$this->VerifyResultLines($connection,"250"))>=0)
				{
					if($result
					&& $this->PutLine($connection,"DATA"))
						$result=($this->VerifyResultLines($connection,"354")!=0);
					if($this->debug)
						$this->OutputDebug("This host states that the address is ".($result ? "" : "not ")."valid.");
					fclose($connection);
					if($this->debug)
						$this->OutputDebug("Disconnected.");
					return($result);
				}
				$this->error_code = EMAIL_ERR_HOST_NO_MBOX;
				
				if($this->debug)
					$this->OutputDebug("Unable to validate the address with this host.");
				fclose($connection);
				if($this->debug)
					$this->OutputDebug("Disconnected.");
			}
			else
			{
				if($this->debug)
					$this->OutputDebug("Failed.");
				$this->error_code = EMAIL_ERR_HOST_NO_CONN;
			}
		}
		return(-1);
	}
}
?>