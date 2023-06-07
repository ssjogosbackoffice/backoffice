<?php
    define('DIR_CACHE_FILES', WEB_ROOT.'/cache');
    
    
    class cache_file_writer
    {   
        var $filename;
        var $filepath;
        var $file;
       
        
        function cache_file_writer($filename)
        {
            $this->filename = $filename;           
            $this->filepath = DIR_CACHE_FILES. "/" . $this->filename;
        }
        
        
        function open ()
        {      
            $this->file = fopen($this->filepath, 'w');
            
            return !empty($this->file);
        }
        
        
        function is_open ()
        {   
            return !empty($this->file);
        }
        
        
        function close ()
        {   
            fclose($this->file);
        }
       
       
        function write_all ($recordset)
        { 
            foreach ( $recordset as $ind => $record)
            {
                $str = implode('|', $record);
                $written = fputs($this->file, $str ."\n");
                
                if ( ! $written )
                    return false;
            }
        }
       
       
        function write_line ($file, $record)
        { 
            $str = implode('|', $record);
            $written = fputs($f, $str ."\n");
                
            if ( ! $written )
                return false;        
            return true;
        }
    } 
    
    
    
    class cache_file_reader
    { 
        var $filename;
        var $filepath;
        var $file;
       
        
        function cache_file_reader($filename)
        {
            $this->filename = $filename;
            $this->filepath = DIR_CACHE_FILES. "/" . $this->filename;
        }
        
        
        function open ()
        {      
            $this->file = fopen($this->filepath, 'r');
            
            return !empty($this->file);
        }
        
        
        function is_open ()
        {   
            return !empty($this->file);
        }
        
        
        function close ()
        {   
            fclose($this->file);
        }
       
        
        function get_filename()
        {
            return $this->filename;   
        }
        
        
        function get_filepath()
        {
            return $this->filepath;   
        }
        
        
        function to_start ()
        {
            rewind($this->file);
        }
        
        
        function to_line ($num,$len=2048)
        {        
            $this->to_start();
                    
            for ( $i=0; $i<$num; $i++ )
            {
                if ( feof($this->file) )
                    return false;      
                    
                $line = fgets($this->file, $len);
            }
            
            return true;        
        }
        
        
        function read_line ($len=2048)
        { 
            $arr = array();
            
            $line = fgets($this->file, $len);
                        
            return $line;
        }
        
        
        function line_to_array($line)
        {
            return explode('|', $line);   
        }
        
        
        function eof()
        {
            return feof($this->file);
        }
       
       
        function read_all ()
        { 
            $arr = array();
            
            rewind($this->file);
            
            $i=0;    
            $line = fgets($this->file, $len);
            
            while ( !feof($this->file) )
            {
                $arr[$i] = explode('|', $line);
                $line = fgets($this->file, 2048);
                $i++;
            }
            
            return $arr;
        }
    }
?>
