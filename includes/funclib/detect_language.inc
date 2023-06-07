<?php
function detectLanguage() 
{
      global $cookie_perm,   // permanent cookie
             $cookie,       // temporary cookie
             $customer_row,  // customer record (will be set if the customer is logged in)
             $site_avail_langs; // available site languages

      // if the customer is logged in, but the permanent cookie containing 
      // the language preference was not sent with the last request.
      // then we default to the punter's preferred language in their db record.
      // the permanent cookie is resent to the customer when a set_language GET arg 
      // added to a requested

      if ( $cookie_perm->wasSentByClient() )  // check permanent cookie first
      {
          // use value from cookie
          $language_code = substr($cookie_perm->getFieldValue('language'), 0, 2);

      }
      elseif ( $cookie->wasSentByClient() )  // else check temporary cookie
      {
          // use value from cookie
          $language_code = substr($cookie->getFieldValue('language'), 0, 2);
      }
      else // no cookies sent by browser? use browser accept language
      {
            if ( !empty($_SERVER['HTTP_ACCEPT_LANGUAGE']) );
            {
                $accept_lang =  strtoupper(substr($_SERVER['HTTP_ACCEPT_LANGUAGE'], 0, 2));

                if ( in_array($accept_lang, $site_avail_langs) )
                {
                    $language_code = $accept_lang;
                }
            }
      }

      if ( ! in_array($language_code, $site_avail_langs) )
      {
        unset($language_code);
      }

      $default_code  = 'EN';

      if( empty($language_code) )
         $language_code = $default_code;
      return strtolower($language_code);
}
?>
