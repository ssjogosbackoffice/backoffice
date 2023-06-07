<?php
   
   // translate: cron/generate_pages.php
   // version 2.0 RC1
   // 
   // 
   // usage: php generate_pages.php -c CONFIG -a|s|<t TEMPLATE> [-dh]
   // 
   //        -c CONFIG
   //           use specified database configuration file
   // 
   //        -a 
   //           generate pages from all available templates
   //
   //        -s 
   //           generate pages from template / language pairs currently
   //           in the database save queue
   // 
   //        -t TEMPLATE
   //           generate pages from the specified template
   // 
   //        -d
   //           turn on debug messages
   //         
   //        -h
   //           display this usage message
   // 

   
   // remove script execution time restrictions
   set_time_limit(0);

   // dependancies
   // dependancies
   $mydir = dirname(__FILE__);
   $incdir = realpath($mydir.'/../');
   $classdir = $incdir.'/classes';
   ini_set('include_path', $incdir.PATH_SEPARATOR.$classdir.PATH_SEPARATOR.ini_get('include_path'));
   require_once('DatabaseHandler.class.inc');
   require_once('Localization.class.inc');
      

   // 
   define('ERROR_FILE_NOT_FOUND' ,        'specified file does not exist');
   define('ERROR_CONFIG',                 'specified configuration file does not contain required database definitions');
   define('ERROR_PAGE_NOT_IN_DB' ,        'specified page not in database list');
   define('ERROR_PAGE_NOT_COMPLETE' ,     'page not complete');
   define('ERROR_NO_AVAILABLE_PAGES' ,    'no available pages were found');
   define('ERROR_NO_AVAILABLE_LANGUAGES', 'no available languages were found');
   
   define('GEN_ALL',                      'all');
   define('GEN_SAVED',                    'saved');
   
   $gen_basedir = null;
   $gen_outdir  = null;
   $gen_include = null;
   $gen_exclude = null;
   $gen_incpage = 0;
   
   // parse script arguments (errors or missing dependancies are handled internally)
   parse_arguments();



   $dbh = null;
   // validate our configuration file
   if((@include_once(get_value('gen_config'))) == false) {
     if (isset($_SERVER["TRANSLATE_DSN"])) {
       define('DSN', $_SERVER["TRANSLATE_DSN"]);
     }
     else {
       error(ERROR_FILE_NOT_FOUND . ' "' . get_value('gen_config') . '"');
     }
   }

   if (defined('DSN')) {
     $url = parse_url(DSN);
     if ($url["scheme"] == "pgsql") {
       if ($url["path"]{0} == '/')
         $url["path"] = substr($url["path"], 1);
       $dbh = new DatabaseHandler($url["host"], $url["path"], $url["user"], $url["pass"]);
     }
   }
   elseif (defined('_DBHOST')) {
     $dbh = new DatabaseHandler();
   }
   else {
     error(ERROR_CONFIG);
   }

   $lte = new LocalizationTemplateEngine();
   
   // connect to the database
   $dbh->connect();
   $lte->set_db_handler($dbh);   


   // precache available languages
   if(($available_languages = $lte->get_available_languages()) == false)
      error(ERROR_NO_AVAILABLE_LANGUAGES);
      
   debug("\ngenerating page list\n");

  // get all the templates that we need to generate pages from
   switch(get_value('gen_type'))
   {
      // generate from template / language pairs currently in the save queue
      case GEN_SAVED:
         $sql = 'SELECT (SELECT template FROM localization_pages WHERE id = page_id) AS "template", ' .
                '(SELECT code FROM localization_languages WHERE id = lang_id) AS "lang_code", ' . 
                'page_id, lang_id FROM localization_pages_status WHERE queued = true ORDER BY template, lang_id DESC';
                
         $res = $dbh->exec($sql);
         
         while($res->hasNext()) {
            $page = $res->next();
            if (can_include($page['template'])) {
              $pages_to_generate[$page['page_id']]['input'] = $page['template'];
              $pages_to_generate[$page['page_id']]['output'][$page['lang_id']] = generate_output_filepath($page['template'], $page['lang_code']);
            }
         }
         
         break;
         
         
      // generate from all available templates
      case GEN_ALL:
         $sql = 'SELECT id, template FROM localization_pages WHERE enabled = TRUE';
         $res = $dbh->exec($sql);

         while($res->hasNext()) {
            // add each template to our list
            if (can_include($template['template'])) {
              $pages_to_generate[$template['id']]['input'] = $template['template'];
            }
         }
         
         break;
         
         
      // generate from specific template
      default:
         $page     = $lte->get_page_by_template($template);
         
         // if page['id'] isn't set, then this template isn't defined in the database
         // and we must exit.
         
         if(is_null($page['id']))
            error(ERROR_PAGE_NOT_IN_DB . ' "' . $template . '"');
            
         $pages_to_generate[$page['id']]['input'] = $template;
   }
   
      
   // if we dont have any pages to generate, bail out now
   if(count($pages_to_generate) <= 0)
      error(ERROR_NO_AVAILABLE_PAGES, false);
   
      
   // i like things nice and orderly ... leave me alone :(
   uasort($pages_to_generate, 'sort_filename');
   
   // iterate through each template we have to generate pages from
   foreach($pages_to_generate as $page_id => $page)
   {
      
      // make sure the template exists still
      if(file_exists($page['input']) == false)
      {
         $error_runtime['warning'][] = $page['input'] . ' no longer exists';
         continue;
      }
      
      // get all content ids for this template
      $content_ids = $lte->get_page_fields($page['input']);
      
      // if this template has no content ids, add a fatal error message and
      // continue processing the next template (if there is one)
      
      if(empty($content_ids))
      {
         $error_runtime['fatal'][] = "no content ids found in " . $page['input'] . "\n       "  .
                                     "remove this entry from the database if no longer used\n";
         continue;
      }
      
      // get all content for this template
      $lte->get_content($content_ids);

      // if we dont know exactly which languages we have to generate pages for,
      // add each language that has atleast some content, regardless if it's outdated
      // or incomplete.
      
      if(isset($page['output']) == false || isset($page['output'][0]))
      {
         foreach($available_languages as $language)
         {
            // if we haven't already got this language as a version to generate and there
            // is atleast some content in this language, add this language to our pages
            // to generate.
         
            if(isset($page['output'][$language['id']]) == false)
            {   
               if(!empty($lte->cache_content[$language['id']]))
               {
                  $page['output'][$language['id']] = generate_output_filepath($page['input'], $language['code']);
               }
            }
         }
         
         // if we dont have anything to generate for this page, at all, add a fatal error
         // message and continue processing the next template (if there is one)
         
         if(is_array($page['output']) == false)
         {
            $error_runtime['fatal'][] = 'no content found for ' . $page['input'];
            continue;
         }
      }
      
      // current timestamp
      $timestamp = gmdate('Y-m-d G:i:s');
      
      // start generating each page
      foreach($page['output'] as $language => $output)
      {
         if($lte->template_write($page['input'], $output, $language))
         {
            debug('generated ' . $output);  

            if (get_value('gen_incpage') == 1) {
              $fname = preg_replace('#\.tpl$#', "",  basename($page['input'])); 
              $pname = dirname($output)."/$fname";
              $xname = $fname;
              $outdir = get_value('gen_outdir');
              if ($outdir != null) {
                $xname = str_replace($outdir, "", $pname);
              }
              //if (!file_exists($pname)) 
                file_put_contents($pname, '<'.'?php include(include_content("'.$xname.'")); ?'.'>');
            } 
            
            // page_complete defaults to true each time we call template_write,
            // but will be set to false by get_text_callback [called from template_write]
            // if any of the content was out of date and had to be substituted with
            // content from the english version.
            //
            // if content was substituted, remove the lastsaved value from the database.
            //
            // this allows a page to be completed, thus setting lastsaved > default.lastsaved,
            // then to be edited again and have content removed and still sorted correctly
            // as not saved.
            // 
            // the lastsaved value is only in relation to default.lastsaved and can be
            // null, regardless if it was actually saved at some point (perhaps the name
            // lastsaved is misleading ?)

            $lte->set_page_lastsaved($page_id, $language, ($lte->page_complete == false) ? NULL : $timestamp);
            
            // remove page from the save queue
            // 
            // this may be a tad on the time expensive side considering the page may not
            // be queued to begin with, but it allows us to make sure that the page is
            // definately not in the save queue (in realtime).
            // 
            // should the script be interrupted, or the queued status queried during
            // execution, it allows us to maintain correct queued statuses at all times
                        
            $lte->set_page_queued($page_id, $language, false);
         }
         else
         {
            $error_runtime['fatal'][] = 'unable to generate' . $output;
            
            // add this page to the save queue so we can try and regenerate it later.
            // 
            // this is usefull if we update an english page and have to regernerate all
            // other languages (if they have atleast some content) and there is an error
            // updating these pages.
            //
            // they will be added to the save queue so that they can be regenerated regardless
            // if the english version was successfull or not
            
            $lte->set_page_queued($page_id, $language, true);
         }
      }
   }
   
   // done
   //
   // now that we've generated all the pages that we could, display any errors
   // that have occurred during the execution of the script.
   //
   // errors marked as fatal apply only to the processing of that particular
   // template and we must display these errors to the user, regardless if
   // a debug value has been set.
   //
   // errors marked as warnings will only be displayed if debugging is enabled
   
   if(!empty($error_runtime))
   {
      // order errors so that warnings will be shown before fatal errors
      krsort($error_runtime);
      
      debug("\nthe following errors have occured:\n", (empty($error_runtime['fatal'])) ? false : true);
      
      foreach($error_runtime as $type => $errors)
      {

         foreach($errors as $err_msg)
            debug(strtoupper($type) . ': ' . $err_msg, ($type == 'fatal') ? true : false);
            
         debug('', ($type == 'fatal') ? true : false);
      }

   }

   debug('');
    
   // END
   

   // GENERAL FUNCTIONS
  
   
   /**
      @return void
      @desc   parse command line arguments
   */
   
   function parse_arguments()
   {
      // backwards compatability
      // 
      // attempt to parse command line arguments in the style defined by
      // earlier versions of this script
      
      if(parse_arguments_compatible_1())
         return;

         
      // parse command line arguments
      
      for($i = 1; $i < $_SERVER['argc']; $i++)
      {
         $arg = strtolower($_SERVER['argv'][$i]);
   
         if($arg[0] == '-')
         {
            $argsc = strlen($arg) - 1;
   
            for($x = 1; $x <= $argsc; $x++)
            {
               switch($arg[$x])
               {
                  // help
                  case 'h':
                  case '?':
                     usage(false);
                     break;
                  
                  // generation type: all
                  case 'a':
                     set_value('gen_type_count', get_value('gen_type_count') + 1);
                     set_value('gen_type', GEN_ALL);
                     break;
   
                  // generation type: saved
                  case 's':
                     set_value('gen_type_count', get_value('gen_type_count') + 1);
                     set_value('gen_type', GEN_SAVED);
                     break;
   
                  // generation type: template
                  case 't':
                     $template = parse_arguments_value($_SERVER['argv'][++$i]);
                     
                     if(!file_exists($template))
                        error(ERROR_FILE_NOT_FOUND . ' "' . $template . '"');
                  
                     set_value('gen_type_count', get_value('gen_type_count') + 1);
                     set_value('gen_type', $template);
                     break;
                     
                  // config file
                  case 'c':
                     $config = parse_arguments_value($_SERVER['argv'][++$i]);
                     set_value('gen_config', $config);
                     break;
                     
                  // debug
                  case 'd':
                     set_value('gen_debug', true);
                     break;

                 // basedir
                  case 'b':
                     $basedir = parse_arguments_value($_SERVER['argv'][++$i]);
                     set_value('gen_basedir', $basedir);
                     break;

                  // outdir
                  case 'o':
                     $outdir = parse_arguments_value($_SERVER['argv'][++$i]);
                     set_value('gen_outdir', $outdir);
                     break;

                 // incpage
                 case 'p':
                     set_value('gen_incpage', 1);
                     break;

                 // include
                 case 'i':
                     $nclude   = get_value('gen_include');
                     $nclude[] = parse_arguments_value($_SERVER['argv'][++$i]);
                     set_value('gen_include', $nclude);
                     break;

                 // exclude 
                 case 'e':
                     $xclude   = get_value('gen_exclude');
                     $xclude[] = parse_arguments_value($_SERVER['argv'][++$i]);
                     set_value('gen_exclude', $xclude);
                     break;
               }
            }
         }
      }

      // check required arguments
      
      // gen_config
      // make sure we were passed a configuration file.
      
      if(get_value('gen_config') == null)
         usage();
      
         
      // gen_type
      // make sure we were passed one (and only one) template generation type.
      //
      // this could be changed to allow the last gen type value to take power, but i think
      // it's cleaner to just error out here.
      
      if(get_value('gen_type_count') != 1)
         usage();
   }
   
   
   /**
      @return bool
      @desc   parse command line arguments - version 1.0 compatible
   */
   
   function parse_arguments_compatible_1()
   {
      $config = $_SERVER['argv'][1];
      $type   = $_SERVER['argv'][2];
      $debug  = $_SERVER['argv'][3];
      
      if($config[0] == '-' || $type[0] == '-' || $debug[0] == '-')
         return false;
      
      set_value('gen_config', $config);
      set_value('gen_type',   $type);
      
      if($debug)
         set_value('gen_debug', true);
      
      return true;
   }
   
   
   /**
      @return string
      @param  string $value
      @desc   validates the supplied argument, displays usage() on failure
   */
   
   function parse_arguments_value($value)
   {
      if(is_null($value) || $value[0] == '-')
         usage();
      else
         return $value;
   }

   
   /**
      @return string
      @param  string $template
      @param  string $language_code
      @desc   generate a localized (output) filename for a given template / language pair
   */
   function generate_output_name($template, $language_code)
   {
      // strip extension from filename (usually .tpl);
      $filename = substr($template, 0, strrpos($template, '.'));

      // determine where we need to insert our localized identifier (ie. '_en')
      $ins_pos  = strrpos($filename, '.');

      // return new filename
      return substr($filename, 0, $ins_pos) . '_' . strtolower($language_code) . substr($filename, $ins_pos);
   }

   function generate_output_filepath($template, $language_code) {
     $filepath = generate_output_name($template, $language_code);
     $basedir  = get_value('gen_basedir');
     $outdir   = get_value('gen_outdir');
     if ($basedir != null && $outdir != null) {
       $t = substr($basedir, -1);
       if ($t != DIRECTORY_SEPARATOR)
         $basedir .= DIRECTORY_SEPARATOR;
       $t = substr($outdir, -1);
       if ($t != DIRECTORY_SEPARATOR)
         $outdir  .= DIRECTORY_SEPARATOR;
       $filepath = str_replace($basedir, $outdir, $filepath);
     }
     return $filepath;
   }
   
   function can_include($template) {
     $pattern = get_value("gen_exclude");
     if ($pattern != null) {
       foreach ($pattern as $path) {
         if (strstr($template, $path) != FALSE) {
           return 0;
         }
       }
     }
     $pattern = get_value("gen_include");
     if ($pattern != null) {
       $found = 0;
       foreach ($pattern as $path) {
         if (strstr($template, $path) != FALSE) {
           $found = 1;
          break;
         }
       }
       if ($found) { return 1; }
       else { return 0; }
     }
     return 1;
   }
   
   
   /**
      @return mixed
      @param  string $var
      @param  mixed  $val
      @desc   set $$var to the value of $val
   */
   
   function set_value($var, $val)
   {
      global $$var;
      $$var = $val;
   }
   
   
   /**
      @return mixed
      @param  string $var
      @desc   returns the value of $$var
   */
   
   function get_value($var)
   {
      global $$var;
      return $$var;
   }

   
    /**
       @return int
       @param  array $a
       @param  array $b
       @desc   [callback] sort an array based on filename (a['input'], b['input']) taking into account files and directories
    **/
    
    function sort_filename($a, $b)
    {
        if(is_array($a)) { $a = $a['input']; }
        if(is_array($b)) { $b = $b['input']; }

        // both elements are equal
        if($a == $b)
           return 0;

        if(dirname($a) == dirname($b))
        {
           // if both elements are in the same directory, sort on filename
           return (basename($a) < basename($b)) ? -1 : 1;
        } else {
           // otherwise sort on directory name
           return (dirname($a) < dirname($b)) ? -1 : 1;
        }
    }
    
    
   /**
      @return void
      @param  string $msg
      @param  bool   $force
      @desc   display an error message before terminating
   */
   
   function error($msg, $force = true)
   {
      debug("\nerror: " . $msg . "\n", $force);
      exit;
   }
   
   
   /**
      @return void
      @param  string $msg
      @param  bool   $force
      @desc   display a message to the user
   */
   
   function debug($msg, $force = false)
   {
      if(get_value('gen_debug') || $force == true)
         echo $msg . "\n";
   }

   
   /**
      @return void
      @param  bool $simple
      @desc   display usage information to the user
   */
   
   function usage($simple = true) 
   {
      set_value('gen_debug', true);
      
      $usage = "usage: php " . $_SERVER['argv'][0] . " -c CONFIG -a|p|s|<t TEMPLATE> [-dh]\n" .
               "       [-b <basedir> -o <outdir>] -e <PATH>\n";
      
      if(!$simple)
      {
         $usage .= "\n" . 
                   "       -c CONFIG\n" .
                   "          use specified database configuration file" .
                   "\n\n" .
                   "       -a\n" .
                   "          generate pages from all available templates" .
                   "\n\n" .
                   "       -s\n" .
                   "          generate pages from template / language pairs currently\n" .
                   "          in the database save queue" .
                   "\n\n" .
                   "       -t TEMPLATE\n" .
                   "          generate pages from the specified template" .
                   "\n\n" .
                   "       -d\n" .
                   "          turn on debug messages" .
                   "\n\n" .
                   "       -h\n" .
                   "          display this message" .
                   "\n\n" .
                   "       -b <basedir> -o <outdir>\n" .
                   "          define the basedir for templates and outdir for generated pages" .
                   "\n\n" .
                   "       -p\n" .
                   "          create include page for each template" .
                   "\n\n" .
                   "       -i\n" .
                   "          include PATH from generation of files (only in ALL and SAVED mode)" .
                   "\n\n" .
                   "       -e\n" .
                   "          exclude PATH from generation of files (only in ALL and SAVED mode)" .
                   "\n";
      } else {
         $usage .= "\n" .
                   "       see -h for more information\n";
      }
               
      debug($usage);
      exit;
   } 
   
?>
