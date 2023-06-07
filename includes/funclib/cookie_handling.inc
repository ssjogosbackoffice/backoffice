<?php
    
    // if set_language get arg was passed to script
    // copy the value  to permanent and tempory cookie (just in case perm cookie is not accepted)
    
    $set_lang = '';
    
    if (  in_array($_GET['set_language'],$site_avail_langs) )
    {    
        $set_lang = $_GET['set_language'];        
    }

       
    // PERMANENT COOKIE:
    
    // Handle permanent cookie.  Used to store permanent settings for user. logged in or logged out
    // The permanent cookie is used to:
    //  - store permanent user settings for when logged out or for revisits
            
    $ck_val  = '';
    $expires = time()+60*60*24*365;  // "permanent" expire in one year
    

    $cookie_perm = new Cookie('cookie_perm', $value="", $expires, $path='/', $domain=$_SERVER['HTTP_HOST'], $secure=0); //initialize cookie class
      

    if (  ! empty($set_lang) )
    {    
        $cookie_perm->setFieldValue('language', $set_lang);  //replace lang field value with $_GET['set_language']
        $cookie_perm->set(); //reset cookie
    }
    else 
    {   
        // no language change - just set permanent cookie if it was not sent  from the browser in the last request
        $ck_default_lang = 'EN';
        
        if ( ! $cookie_perm->wasSentByClient() )
        {         
            if ( !empty($_SERVER['HTTP_ACCEPT_LANGUAGE']) );
            {       
                $accept_lang =  strtoupper(substr($_SERVER['HTTP_ACCEPT_LANGUAGE'], 0, 2));
                
                if ( in_array($accept_lang, $site_avail_langs) )
                {
                    $ck_default_lang = $accept_lang;
                }
            }
            
            $cookie_perm->setFieldValue('language', $ck_default_lang); // default to english
            $cookie_perm->set();
        }
    }
    
    
    // TEMPORARY COOKIE:
    
    // Handle temporary cookie (not to be confused with session cookies for https only)
    // The temporary cookie is a composite cookie used to:
    //  - check that the browser accepts cookies before login
    //  - store session variables for non-https requests (the session cookie is sent over https only)
    // Destroyed on site logout,  session timeout or browser close
    
    // Instantiate the cookie class - will set value to $_COOKIE val if exists (wasSentByClient

    $cookie = new Cookie('cookie', $value="", $expires=0, $path='/', $domain=$_SERVER['HTTP_HOST'], $secure=0); //initialize cookie class
                    
    if ( ! empty($set_lang) )   // change language if requested
    {
        $cookie->setFieldValue('language', $set_lang); //set language in temporary cookie
        $cookie->set();  //send cookie to browser
    }
    elseif ( $cookie_perm->wasSentByClient() )
    {
        $cookie->setFieldValue('language', $cookie_perm->getFieldValue('language')); //set language in temporary cookie
        $cookie->set();  //send cookie to browser
    }
    else
    {   
        // no language change - just set temporary cookie if it was not sent -
        // from the browser in the last request

        if ( ! $cookie->wasSentByClient() )
        {                              
            
            $cookie->setFieldValue('presess', 'ok'); // placeholder value - we only use composite cookes... name:val|name2:val2
            
            $ck_default_lang = 'EN';
            
            if ( !empty($_SERVER['HTTP_ACCEPT_LANGUAGE']) );
            {       
                $accept_lang =  strtoupper(substr($_SERVER['HTTP_ACCEPT_LANGUAGE'], 0, 2));
                
                if ( in_array($accept_lang, $site_avail_langs) )
                {
                    $ck_default_lang = $accept_lang;
                }
            }
            
            $cookie->setFieldValue('language', $ck_default_lang); // default to english
            $cookie->set();
        }
    }
    
?>