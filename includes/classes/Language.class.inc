<?php
/**
 * Created by CSLIVEGAMES.
 * User: Manuel
 * Date: 12/06/13
 * Time: 13.42
 * File: Language.class.inc
 */

class Language {
    private $language_code;
    private $lang_array;
    function __construct($available_langs){
        if(isset($_SESSION['language'])){
            $this->language_code = $_SESSION['language'];
        }
        else { // no cookies sent by browser? use browser accept language
            if ( !empty($_SERVER['HTTP_ACCEPT_LANGUAGE']) ) {
                $accept_lang =  strtoupper(substr($_SERVER['HTTP_ACCEPT_LANGUAGE'], 0, 2));

                if ( in_array($accept_lang, $available_langs) ) {
                    $this->language_code = $accept_lang;
                    $_SESSION['language'] = $accept_lang;
                }
            }
        }
        include_once(LANGUAGES_PATH.'/lang_'.strtolower($this->language_code).'.inc');
        error_log("i will load the file " . LANGUAGES_PATH.'/lang_'.strtolower($this->language_code).'.inc');
        //include_once(LANGUAGES_PATH.'/lang_'.strtolower("it").'.inc');
        $this->lang_array = $language;
    }
    function getLang(){
        $arg_list = func_num_args();
        $text = func_get_arg(0);
        $args = func_get_args();
        array_shift($args);
        $arg_list--;
        if($arg_list == 0){
            if(($this->lang_array[strtolower($text)])!=''){
                return ucfirst($this->lang_array[strtolower($text)]);
            }else{
                return ucfirst($text);
            }
        }else{
            $elements = count($args);
            if(!is_null($this->lang_array[strtolower($text)])){
                $languageString = $this->lang_array[strtolower($text)];

                //return ucfirst(str_replace('%', $args, $this->lang_array[strtolower($text)]));
            }else{
                $languageString = $text;
                //return ucfirst(str_replace('%', $args, $text));
            }

            $languageString = explode(" " , $languageString);
            for($i = 0; $i < count($languageString) ; $i++){
                if(strpos($languageString[$i],"%") === false) continue;
                $languageString[$i] = str_replace("%",array_shift($args),$languageString[$i]);
            }
            $languageString = implode(" ", $languageString);
            $languageString = ucfirst($languageString);
            return $languageString;
        }

    }
}