<?php

   // translate: lib/localization.php
   // version 2.0 RC1
   //

   // LocalizationTemplateEngine class
   // 
   
   class LocalizationTemplateEngine 
   {
      var $dbh;
      var $dbh_roe;
      var $user_id;
      var $errors;
      
      var $cache_pages;
      var $cache_content;
      var $cache_languages;

      var $page_complete;
      
      function LocalizationTemplateEngine()
      {
         $this->set_db_handler(new NullDbHandler);
         $this->set_user_restrictions();
         
         $this->cache_pages     = array();
         $this->cache_content   = array();
         $this->cache_languages = array();
         
         // dbh: return on error
         // this allows us to return false on a failed database query
         $this->dbh_roe         = true;
      }
      

      /**
         @return mixed
         @param  mixed  $page_identifier
         @param  string $page_field
         
         retrieve a page from the database
      */
      
      function get_page($page_identifier, $page_field = 'id', $override_restrictions = false)
      {
         // sanitize arguments
         $page_field = strtolower($page_field);
         
         if(!is_bool($page_identifier))
            $page_identifier = strtolower($page_identifier);

         
         if(is_array($this->cache_pages))
         {
            if($page_field == 'id')
            {
               // if we're identifying our page based on the page id
               // we can save the trouble of iterating through our page
               // cache, and just access it directly.
               
               if($this->cache_pages[$page_identifier])
                  return $this->cache_pages[$page_identifier];
            }
            else
            {
               // if we're not identifying our page based on the page id
               // we must iterate through the page cache to try and find
               // what we're looking for
               
               foreach($this->cache_pages as $page)
               {
                  if(strtolower($page[$page_field]) == $page_identifier)
                     return $page;
               }
            }
         }
         
         // if the page isnt in our page cache already, try and find it in
         // the database, and if we find it, add it to our cache.
         
         if(isset($this->user_id) && $override_restrictions == false)
            $uqr = 'AND l.id IN (SELECT lang_id FROM localization_users_languages WHERE user_id = ' . $this->user_id . ')';
          
            
         if(is_bool($page_identifier))
            $sqr = 'AND p.' . addslashes($page_field) . ' = ' . (($page_identifier) ? 'true' : 'false') . ' ';
         else
            $sqr = 'AND lower(p.' . addslashes($page_field) . ') = \'' . addslashes($page_identifier) . '\' ';
            
         // query the database
         
         // even if we have a user query restriction set ($uqr) that MAY not return pages
         // in the default language, we still must retrieve them so that we can offset the
         // lastsaved value for a page against it (so we know if it requires editing or not)
         
         $sql = 'SELECT p.id, p.name, p.template, p.enabled, l.id AS "lang_id", ' .
                '(SELECT lastsaved FROM localization_pages_status WHERE page_id = p.id and lang_id = l.id) AS "lastsaved", ' .
                '(SELECT queued FROM localization_pages_status WHERE page_id = p.id and lang_id = l.id) AS "queued" ' .
                'FROM localization_pages p, localization_languages l WHERE l.enabled = TRUE ' .
                'AND ((l.id = 0 ' . $sqr . ') OR (1=1 ' .                
                $uqr .
                $sqr .
                ')) ORDER BY p.id, l.id;';
                
         $res = $this->db_exec($sql);
         
         if($res == false || $res->getNumRows() <= 0)
            return false;

         
         // add each page to our page cache

         $available_languages = $this->get_available_languages();
         
         while($res->hasNext())
         {
            $page = $res->next();
            
            if($page['lang_id'] == 0)
            {
               // store the lastsaved value of this page (in the default language) locally
               // so we can use it as a base to determine if another page requires editing
               // or not
               
               $local_cache[$page['id']]['lastsaved'] = $page['lastsaved'];
               
               // if the current user cannot edit pages in the default language,
               // do not taint the page cache by storing this data
               
               if(isset($available_languages[0]) == false && $override_restrictions == false)
                  continue;
            }
  
             
            $page['queued']  = ($page['queued'] == 't')  ? true : false;
            $page['enabled'] = ($page['enabled'] == 't') ? true : false;
            
            if(!isset($this->cache_pages[$page['id']]['id']))
            {
               $this->cache_pages[$page['id']]['id']       = $page['id'];
               $this->cache_pages[$page['id']]['name']     = $page['name'];
               $this->cache_pages[$page['id']]['template'] = $page['template'];
               $this->cache_pages[$page['id']]['enabled']  = $page['enabled'];
            }
            
            // calculate the current status of this page

            if($page['queued'])
            {
               // if the page is queued, we don't need to offset it against
               // the default version of this page
               
               $status = 'queued';
            }
            else
            {
               // match the last saved time for this page (in whatever language) against the locally
               // cached last saved time in the default language.
               
               if($page['lastsaved'] && $page['lastsaved'] >= $local_cache[$page['id']]['lastsaved'])
               {
                  // last saved time is set and newer than (or equal to) the default last saved time
                  // then this page does not require editing
                  $status = 'saved';
               }
               else
               {
                  // if the default version of this page is newer than this version, we must flag the
                  // page as requiring editing
                  $status = 'not saved';
               }
            }
               

            $this->cache_pages[$page['id']]['languages'][$page['lang_id']]['lastsaved'] = $page['lastsaved'];
            $this->cache_pages[$page['id']]['languages'][$page['lang_id']]['status']    = $status;
         }

         
         return $this->cache_pages[$page['id']];
      }
      
      
      /**
         @return mixed
         @param  int $page_id
         
         retrieve a page from the database based on the page id
      */
      
      function get_page_by_id($page_id, $override_restrictions = false)
      {
         return $this->get_page($page_id, 'id', $override_restrictions);
      }

      
      /**
         @return mixed
         @param  string $page_name
         
         retrieve a page from the database based on the page name
      */

      function get_page_by_name($page_name, $override_restrictions = false)
      {
         return $this->get_page($page_name, 'name', $override_restrictions);
      }

      
      /**
         @return mixed
         @param  string $page_template
         
         retrieve a page from the database based on the page template
      */

      function get_page_by_template($page_template, $override_restrictions = false)
      {
         return $this->get_page($page_template, 'template', $override_restrictions);
      }
      
      
      /**
         @return mixed
         
         retrieve all available pages from the database
      */

      function get_available_pages()
      {
         $this->cache_pages = array();
         
         $this->get_page(true, 'enabled');
         return $this->cache_pages;
      }

      
      /**
         @return mixed
         @param  mixed  $language_identifier
         @param  string $language_field
         
         retrieve a language from the database
      */

      function get_language($language_identifier, $language_field = 'id', $override_restrictions = false)
      {
         // sanitize arguments
         $language_field = strtolower($language_field);
         
         if(!is_bool($language_identifier))
            $language_identifier = strtolower($language_identifier);
         
            
         if(is_array($this->cache_languages))
         {
            if($language_field == 'id')
            {
               // if we're identifying our language based on the language id
               // we can save the trouble of iterating through our language
               // cache, and just access it directly.
               
               if($this->cache_languages[$language_identifier])
                  return $this->cache_languages[$language_identifier];
            }
            else
            {
               // if we're not identifying our language based on the language id
               // we must iterate through the language cache to try and find
               // what we're looking for
               
               foreach($this->cache_languages as $language)
               {
                  if(strtolower($language[$language_field]) == $language_identifier)
                     return $language;
               }
            }
         }
         
         // if the language isnt in our language cache already, try and find it in
         // the database, and if we find it, add it to our cache.

         if(isset($this->user_id) && $override_restrictions == false)
            $uqr = 'AND id IN (SELECT lang_id FROM localization_users_languages WHERE user_id = ' . $this->user_id . ') ';
          
            
         if(is_bool($language_identifier))
            $sqr = 'AND ' . addslashes($language_field) . ' = ' . (($language_identifier) ? 'true' : 'false') . ' ';
         else
            $sqr = 'AND lower(' . addslashes($language_field) . ') = \'' . addslashes($language_identifier) . '\' ';

         
         $sql = 'SELECT id, code, name FROM localization_languages WHERE 1=1 ' . $uqr . $sqr;
         $res = $this->db_exec($sql);

         if($res == false)
            return false;
         
         if($res->getNumRows() > 0)
         {
            while($res->hasNext())
            {
               $language = $res->next();
               $this->cache_languages[$language['id']] = $language;
            }
            
            return $language;
         }
         
         return false;
      }

      
      /**
         @return mixed
         @param  int  $language_id
         
         retrieve a language from the database based on the language id
      */

      function get_language_by_id($language_id, $override_restrictions = false)
      {
         return $this->get_language($language_id, 'id', $override_restrictions);
      }

      
      /**
         @return mixed
         @param  string  $language_name
         
         retrieve a language from the database based on the language name
      */

      function get_language_by_name($language_name, $override_restrictions = false)
      {
         return $this->get_language($language_name, 'name', $override_restrictions);
      }

      
      /**
         @return mixed
         @param  string  $language_code
         
         retrieve a language from the database based on the language code
      */

      function get_language_by_code($language_code, $override_restrictions = false)
      {
         return $this->get_language($language_code, 'code', $override_restrictions);
      }
      
      
      /**
         @return mixed
         
         retrieve all available languages from the database
      */

      function get_available_languages()
      {
         $this->cache_languages = array();
         
         $this->get_language(true, 'enabled');
         return $this->cache_languages;
      }
      
      
      /**
         @return bool
         @param  array $languages
         
         forcefully set the current available languages
      */
      
      function set_available_languages($languages)
      {
         if(!is_array($languages))
            return false;

         foreach($languages as $language)
         {
            if(isset($language['id']))
               $this->cache_languages[$language['id']] = $language;
         }
            
         return true;
      }
      
      
      /**
         @return mixed
         @param  string $template
         
         read content fields from template
      */
      
      function get_page_fields($template)
      {
         // open our template
         if(($fp = @fopen($template, "r")) == false)
            return false;

         // calculate exactly which fields we require from the database
         while(!feof($fp))
         {
            // match our content tag
            preg_match_all("/<content[\s]+([^=]*=[^>]*)>/si", fgets($fp), $m);

            if(is_array($m))
            {
               // retrieve all tag attributes
               foreach($m[1] as $val)
               {
                  preg_match_all("/([\w]+)=[\"\']*([$]*)([\w]+)[\"\']*/si", $val, $sm);

                  if(is_array($m))
                  {
                     // get each attribute
                     foreach($sm[1] as $key => $val)
                     {
                        $sret[strtolower($val)]['type']  = ($sm[2][$key] == '$') ? 'variable' : 'constant';
                        $sret[strtolower($val)]['value'] =  $sm[3][$key];
                     }

                     // store all attributes in our fields array
                     if(isset($sret['id']))
                        $fields[$sret['id']['value']] = $sret;

                     unset($sret);
                  }
               }
            }

            unset($m);
         }

         // close our template
         fclose($fp);
        
         // update page / content id matrix
         $this->set_page_fields($template, $fields);

         // format the fields we require correctly and return
         if(is_array($fields))
         {
            // success
            ksort($fields);

            // retrieve all other templates that use these content ids
            $matches = $this->get_page_fields_conflicts_by_template($template, $fields);

            // and assign them to our fields array
            if(is_array($matches))
            {
               foreach($matches as $id => $val)
               {
                  $fields[$id]['conflicts'] = $val;
               }
            }
             
            return $fields;
         }
         
         return false;
      }
            
      
      /**
         @return mixed
         @param  array $fields
         @param  int   $page_id (optional)
         
         retrieve an array of content field conflicts
      */
      
      function get_page_fields_conflicts($fields, $page_id = NULL)
      {
         // sanitize
         if(is_array($fields) == false)
            $fields = array(intval($fields) => 1);
         
         // do we have page restrictions ?
         if($page_id != NULL)
            $pr =  ' AND page_id != ' . intval($page_id);
            
         $id_list = "content_id = '" . implode("' OR content_id = '", array_keys($fields)) . "'";

         $sql     = 'SELECT content_id, (select name from localization_pages where id = page_id) as "page_name" ' .
                    'FROM localization_pages_content_ids WHERE (' . $id_list . ') ' . $pr;

         $res     = $this->db_exec($sql);
         
         if($res == false)
            return false;

         while($res->hasNext())
         {
            $cntblock = $res->next();
            $matched[$cntblock['content_id']][] = $cntblock['page_name']; 
         }
         
         return $matched;
      }


      function get_page_id_by_fields($fields)
      {
         // sanitize
         if(is_array($fields) == false)
            $fields = array(intval($fields) => 1);
         
         $id_list = "content_id = '" . implode("' OR content_id = '", array_keys($fields)) . "'";

         $sql     = 'SELECT DISTINCT page_id FROM localization_pages_content_ids WHERE (' . $id_list . ')';
         $res     = $this->db_exec($sql);

         if($res == false)
            return false;

         while($res->hasNext())
         {
            $page = $res->next();
            $matched[$page['page_id']] = $page['page_id'];
         }
         
         return $matched;
      }
      
      
      /**
         @return mixed
         @param  string $template
         @param  array  $fields
         
         retrieve an array of content field conflicts
      */
      
      function get_page_fields_conflicts_by_template($template, $fields)
      {
         $page    = $this->get_page_by_template($template);
         $page_id = $page['id'];
         
         return $this->get_page_fields_conflicts($fields, $page_id);
      }
      
      
      /**
         @return mixed
         @param  int    $page_id
         @param  array  $fields
         
         retrieve an array of content field conflicts
      */
      
      function get_page_fields_conflicts_by_page_id($page_id, $fields)
      {
         return $this->get_page_fields_conflicts($fields, $page_id);
      }
      
      
      /**
         @return bool
         @param  string $template
         @param  array  $fields
         
         update the page / content id matrix
      */
      
      function set_page_fields($template, $fields)
      {
         $page    = $this->get_page_by_template($template);
         $page_id = $page['id'];
         
         return $this->set_page_fields_by_page_id($page_id, $fields);

      }
      
      
      /**
         @return bool
         @param  int   $page_id
         @param  array $fields
         
         update the page / content id matrix
      */
      
      function set_page_fields_by_page_id($page_id, $fields)
      {
         if(is_array($fields) == false)
            return false;
         
         $page_id = intval($page_id);

         
         // delete all the current entries for this page
         $sql = 'DELETE FROM localization_pages_content_ids WHERE page_id = ' . $page_id;
         $res = $this->db_exec($sql);
         
         if($res == false)
            return false;
         
         
         // create new entries for this page
         foreach($fields as $field => $val)
         {
            $sql = 'INSERT INTO localization_pages_content_ids (page_id, content_id) VALUES (' . $page_id . ', ' . intval($field) . ')';
            $res = $this->db_exec($sql);
            
            if($res == false)
               $errors = true;
         }
         
         return ($errors) ? true : false;
      }
      
      
      /**
         @return bool
         @param  array $fields
         
         retrieve content from the database
      */
      
      function get_content($fields, $clear_cache = true)
      {
     
         // format our query for only those fields we need    
         if(is_array($fields))
         {
            $id_list = "id = '" . implode("' OR id = '", array_keys($fields)) . "'";
            $cir     = "AND (" . $id_list . ")";
         }

         // this should probably be rewritten to only include content in the available
         // languages (and the default language)
         
         $sql = "SELECT * FROM localization_content WHERE (content != '' OR language = 0) " . $cir;
         $res = $this->db_exec($sql);

         if($res == false || $res->getNumRows() <= 0)
            return false;
            
         // clear any currently cached content
         if($clear_cache)
            unset($this->cache_content);
         
         while($res->hasNext())
         {
            $result = $res->next();
            $this->cache_content[$result['language']][$result['id']] = $result;
         }
         
         return true;
      }
      
      
      /**
         @return string
         @param  int  $text_id
         @param  int  $language
         @param  bool $up_to_date
         
         retrieve the textual content for this content id
      */
      
      function get_text($text_id, $language, $up_to_date = false)
      {
         // sanitize arguments
         $text_id  = intval($text_id);
         $language = intval($language);
         
         // if this content entry does not already exist in our content cache,
         // retrieve it from the database and add it to the cache

         if(isset($this->cache_content[$language][$text_id]) == false)
            $this->get_content(array($text_id => true), false);
         
         // if we're attempting to retrieve the most up to date version of our
         // content (in either the current or default language), compare the
         // last saved timestamps of each to determine which content should
         // be returned
          
         if($up_to_date && $language != 0)
         {
            $lastsaved_language = $this->cache_content[$language][$text_id]['lastsaved'];
            $lastsaved_default  = $this->cache_content[0][$text_id]['lastsaved'];
            
            if($lastsaved_language < $lastsaved_default)
            {
               // if the current content for this content block is older
               // than the default content (ie. it needs to be updated),
               // replace it with the most current content and set
               // page_complete to false
               
               $language = 0;
               $this->page_complete = false;
            }
         }
         
         
         // if this content entry does not exist in our cache, even after
         // we attempted to update the cache above, return an empty string
         
         if(isset($this->cache_content[$language][$text_id]) == false)
            return '';
         
            
         // return content
         
         return $this->cache_content[$language][$text_id]['content'];
      }
      
      
      /**
         @return string
         @param  string $content_tag
         @param  int    $language
         
         parse the given content tag (from a template) and return the actual content intended to replace it 
      */
      
      function get_text_callback($content_tag, $language)
      {
         $content_tag = stripslashes($content_tag);

         // default content tag arguments
         $carg['casino_full_name'] = array('type' => 'variable', 'value' => 'casino_full_name');
         $carg['casino']           = array('type' => 'variable', 'value' => 'casino_full_name');
         $carg['site_name']        = array('type' => 'constant', 'value' => 'site_name');
         $carg['nonsecure_host']   = array('type' => 'constant', 'value' => 'nonsecure_host');
         $carg['secure_host']      = array('type' => 'constant', 'value' => 'secure_host');
         
         // split our content tag arguments up and assign them to our array
         preg_match_all("/([\w]+)=[\"\']*([$]*)([\w\{\}\]\[,]*)[\"\']*/si", $content_tag, $m);

         foreach($m[1] as $key => $val)
         {
            $carg[strtolower($val)]['type']  = ($m[2][$key] == '$') ? 'variable' : 'constant';
            $carg[strtolower($val)]['value'] =  $m[3][$key];
         }

         if(isset($carg['id']))
            $ret[$carg['id']['value']] = $carg;

         $text = $this->get_text(intval($carg['id']['value']), $language, true);

         
         // determine exactly what we're doing with new lines; converting them
         // to html breaks (<br>), to '\n', or just leaving them be
         
         $noparse        = strtolower($ret[$carg['id']['value']]['noparse']['value']);        
         $option_newline = strtolower($ret[$carg['id']['value']]['option_newline']['value']);

         // v1.0 compat
         if($noparse == true && empty($option_newline))
            $option_newline = 'literal';
            
         switch($option_newline)
         {
            case 'literal':
               $text = str_replace("\n", '\n', $text);
               break;
            case 'noparse':
            default:
               $text = str_replace("\n", '<br/>', $text);
               break;
         }
         
         // clean up
         unset($m);

         // convert all variable/constant place holders in our text to their actual php equiv
         preg_match_all("/\[%([^\]]*)%]?\]/si", $text, $m);

         // strip php tags from our content
         $text = str_replace('<?', '&lt;?', $text);
         $text = str_replace('<%', '&lt;%', $text);
         $text = str_replace('?>', '?&gt;', $text);
         $text = str_replace('%>', '%&gt;', $text);

         // determine if we have to prepend/append php tags
         $var_start = '<?=';
         $var_end   = '?>'; 
         $tag_type  = '';
         if (!empty($ret[$carg['id']['value']]['type'])) {
            $tag_type = strtolower($ret[$carg['id']['value']]['type']['value']);
            if ($tag_type == 'smarty') {
              $var_start = '{';
              $var_end   = '}';
            } else if ($tag_type == 'php') {
              $var_start = '';
              $var_end   = '';
            } else if ($tag_type == 'xml') {
              $text = '<![CDATA['.$text.']]>';
            }
         }

         if(is_array($m))
         {
            // loop through each place holder
            foreach($m[1] as $val)
            {
               // reference everything in lowercase
               $lcstr_val = strtolower($val);

               // determine the variable prefix
               if ((!empty($tag_type) && ($tag_type == "smarty")) ||
                   ($ret[$carg['id']['value']][$lcstr_val]['type'] == 'variable'))
                  $var_prefix = '$';
               else
                  $var_prefix = '';

               // substitute our place holders with their respective php equiv
               if(isset($ret[$carg['id']['value']][$lcstr_val]['value']))
                  $text = str_replace('[%' . $val . '%]', $var_start . $var_prefix . $ret[$carg['id']['value']][$lcstr_val]['value'] . $var_end, $text);
            }
         }

         // return the entire replacement string, fixing any already escaped new lines.
         return (empty($ret[$carg['id']['value']]['type'])) ? $text : str_replace("\\\\\\n", '\\\n', str_replace("\\\\n", '\n', addcslashes($text, "\\\"")));
      }
      
      
      /**
         @return bool
         @param  int       $page_id
         @param  int       $language
         @param  timestmap $timestamp
         
         update this page's last saved timestamp
      */
      
      function set_page_lastsaved($page_id, $language, $timestamp)
      {
         // sanitize arguments
         $page_id   = intval($page_id);
         $language  = intval($language);
         $timestamp = ($timestamp) ? "'" . addslashes($timestamp) . "'" : 'null';
         
         // attempt to update the page entry
         $sql = 'UPDATE localization_pages_status SET lastsaved = ' . $timestamp . ' WHERE page_id = ' . $page_id . ' AND lang_id = ' . $language;
         $this->db_exec($sql);
         
         // determine whether to insert or update our record
         if($this->dbh->getAffectedRows() <= 0)
         {
            $sql = 'INSERT INTO localization_pages_status (page_id, lang_id, lastsaved, queued) VALUES (' . $page_id . ', ' . $language . ', ' . $timestamp . ', false)';
            $this->db_exec($sql);
            
            if($this->dbh->getAffectedRows() < 0)
               return false;
               
            return true;
         }         
         
         return true;
      }
      
      
      /**
         @return bool
         @param  int  $page_id
         @param  int  $language
         @param  bool $queued
         
         set the queued status of the given page
      */
      
      function set_page_queued($page_id, $language, $queued)
      {
         // sanitize arguments
         $page_id  = intval($page_id);
         $language = intval($language);
         $queued   = ($queued) ? 'true' : 'false';
          
         $sql = 'UPDATE localization_pages_status SET queued = ' . $queued . ' ' .
                'WHERE page_id = ' . $page_id . ' AND lang_id = ' . $language;

         $this->db_exec($sql);
         
         if($this->dbh->getAffectedRows() <= 0)
         {
            $sql = 'INSERT INTO localization_pages_status (page_id, lang_id, queued, lastsaved) VALUES (' .
                   $page_id . ', ' . $language . ', ' . $queued . ', \'' . gmdate('Y-m-d G:i:s') . '\')';
                   
            $this->db_exec($sql);
            
            if($this->dbh->getAffectedRows() <= 0)
               return false;
               
            return true;
         }
         
         return true;
      }
      
      
      /**
         @return bool
         @param  int    $text_id
         @param  string $text
         @param  int    $language
         
         set the text value of a content id for a given language
      */
      
      function set_text($text_id, $text, $language, $timestamp = false)
      {
         // sanitize arguments
         $language  = intval($language);
         $text_id   = intval($text_id);
         $text      = addslashes($text);
         $timestamp = ($timestamp) ? $timestamp : gmdate('Y-m-d G:i:s');
         
         $sql      = 'UPDATE localization_content SET content = \'' . $text . '\', lastsaved = \'' . 
                     $timestamp . '\' WHERE id = ' . $text_id . ' AND language = ' . $language;
                     
         $this->db_exec($sql);
         
         if($this->dbh->getAffectedRows() <= 0)
         {
            $sql = 'INSERT INTO localization_content (id, language, content, lastsaved) VALUES (' .
                    $text_id . ', ' . $language . ', \'' . $text . '\', \'' . $timestamp . '\')';
                    
            $this->db_exec($sql);
            
            if($this->dbh->getAffectedRows() <= 0)
               return false;
         }

         return true;
      }
      
      
      /**
         @return bool
         @param  int    $text_id
         @param  string $description
         
         set the description value of a content id
      */
      
      function set_text_description($text_id, $description)
      {
         // sanitize arguments
         $text_id     = intval($text_id);
         $description = addslashes($description);
         
         $sql = 'UPDATE localization_content SET description = \'' . $description . '\' WHERE ' .
                'id = ' . $text_id . ' AND language = 0';
         
         $this->db_exec($sql);
         
         if($this->dbh->getAffectedRows() <= 0)
         {
            $sql = 'INSERT INTO localization_content (id, language, lastsaved, enabled, description) VALUES (' .
                   $text_id . ', 0, \'' . gmdate('Y-m-d G:i:s') . '\', true, \'' . $description . '\'';
                   
            $this->db_exec($sql);
            
            if($this->dbh->getAffectedRows() <= 0)
               return false;
         }
         
         return true;
      }
      
      
      /**
         @return string
         @param  int $text_id
         
         retrieve the description of a content entry
      */
      
      function get_text_description($text_id)
      {
         if(isset($this->cache_content[0][$text_id]['description']) == false)
            $this->get_text($text_id, 0);
         
         return $this->cache_content[0][$text_id]['description'];
      }
      
      
      /**
         @return mixed
         @param  string $template
         
         read content fields from template
      */
      
      function template_read($template)
      {
         return $this->get_page_fields($template);
      }
      
      
      /**
         @return bool
         @param  string $input
         @param  string $output
         @param  int    $language
         
         generate a page in a specific language from the given input template
      */
      
      function template_write($input, $output, $language)
      {
         // default
         $this->page_complete = true;
         
         // sanitize arguments
         $language  = intval($language);
                  
         // open input/output files for reading/writing
         $fp_input  = @fopen($input, "r");
         $fp_output = @fopen($output, "w");
         
         // return false if there was an error opening either file
         if(!$fp_input || !$fp_output)
            return false;

         // read input file
         while(!feof($fp_input))
         {
            // replace any content tag found in the input file with the correct
            // content and write it to our output files
            
            $callback = "\$this->get_text_callback('\\1', " . $language . ");";
            fputs($fp_output, preg_replace('/<content[\s]+([^=]*=[^>]*)>/esi', $callback, fgets($fp_input)));
         }

         // close input/output files
         fclose($fp_input);
         fclose($fp_output);
         
         return true;
      }
      
      
      /**
         @return bool
         
         add a page to the database
      */
      
      function template_add($name, $template, $enabled = true)
      {
         $sql = "INSERT INTO localization_pages (name, template, enabled) VALUES ('" . addslashes($name) . "', '" .
                addslashes($template) . "', " . (($enabled) ? 'true' : 'false') . ")";
                
         $this->db_exec($sql);
         
         return ($this->dbh->getAffectedRows() > 0) ? true : false;
      }
      
      
      /**
         @return bool
         @param  int $page_id
         
         delete a page from the database
      */
      
      function template_delete($page_id)
      {
         // sanitize arguments
         $page_id   = intval($page_id);
         
         $sql  = "DELETE FROM localization_pages_content_ids WHERE page_id = " . $page_id . "; ";
         $sql .= "DELETE FROM localization_pages_status WHERE page_id = " . $page_id . "; ";
         $sql .= "DELETE FROM localization_pages WHERE id = " . $page_id . "; ";
         
         $this->db_exec($sql);
         
         return ($this->dbh->getAffectedRows() > 0) ? true : false;
      }
      
      
      /**
         @return void
         @param  object &$dbh
         
         set the internal database handler object
      */
      
      function set_db_handler(&$dbh)
      {
         $this->dbh = &$dbh;
      }
      
      
      /**
         @return void
         
         set the internal user id to enforce user restrictions
      */
      
      function set_user_restrictions()
      {
         // if we're being called from the website (which has user level restrictions)
         // and the current user is not an administrator, set the id of the user so
         // that we can restrict our sql queries.
         
         if(!$_SESSION['localization']['auth']['admin'] && isset($_SESSION['localization']['auth']['id']))
            $this->user_id = intval($_SESSION['localization']['auth']['id']);
      }
      

      /**
         @return mixed
         @param  string $sql
         
         execute a sql query against the database
      */
      
      function db_exec($sql)
      {
         return $this->dbh->exec($sql, $this->dbh_roe);
      }
   }

   
   // null_db_handler class
   // 
   // the lte class requires an external database handler class so that it can query the database
   // and will default to this class instead, allowing us to exiting cleanly if a handler has not
   // been specified
   
   class NullDbHandler
   {
      
      // php5 allows us to overload class methods, so any call to this class will result
      // in our clean error response

      function __call($m, $a)
      {
         echo "\nerror: no database handler has been set (see LocalizationTemplateEngine::set_db_handler())\n\n";
         exit;
      }

      
      // php4 doesn't allow us to overload class methods, so we must fake called methods
      // and call __call() from within
      
      function exec($a, $b = 1)
      {
         $this->__call($a, $b);
      }
   }
?>
