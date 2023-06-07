<?php
   function get_user_type_options (){
	   global $dbh, $jscript_type_to_class_map,$lang;
       $options = '';
      //var_dump($_SESSION['jurisdiction_class']);
       $sql = "    SELECT aty_id, aty_name, aty_jurisdiction_class FROM admin_user_type " .
              "      WHERE ";

       switch ( $_SESSION['jurisdiction_class'] ){
    	    case 'district':
            //get clubs that are subordinate to session user's district
            $sql .= "  (( aty_jurisdiction_class  = 'club' ) ";
            break;
          case 'region':
            $sql .= "  (( aty_jurisdiction_class  = 'district' " .
                    "       OR aty_jurisdiction_class  = 'club' )";
            break;
          case 'nation':
            $sql .= "  (( aty_jurisdiction_class  = 'region' " .
                    "       OR aty_jurisdiction_class  = 'district' " .
                    "       OR aty_jurisdiction_class  = 'club' )";
            break;
          case 'casino':
            $sql .= "  (( aty_jurisdiction_class  = 'nation' " .
                    "       OR aty_jurisdiction_class  = 'region' ".
                    "       OR aty_jurisdiction_class  = 'district' " .
                    "       OR aty_jurisdiction_class  = 'club' )";
            break;
          default:
            $sql .= "  ( 1=0 ";
       }
       // or we can select admin users in the same jurisdiction that are lower-level
       // (a larger aty_level than the logged in user)
       $sql  .= " OR " .
                " ( aty_level  >  " . $_SESSION['aty_level'] .
                "        AND  aty_jurisdiction_class = '" . $_SESSION['jurisdiction_class']."'" .
                " ) ) ";

       $sql  .= ' ORDER by aty_name';
       //var_dump($sql);
       $rs       = $dbh->exec($sql);
       $num_rows = intval($rs->getNumRows());
       $i=0;
       if ( $num_rows > 0 ){
           while ($row=$rs->next()){
           	   $selected = '';
        	   $id          = $row['aty_id'];
        	   $name        = $row['aty_name'];
        	   $class       = $row['aty_jurisdiction_class'];
        	   $options[$i] = array('id'=>$id, 'name'=>$name);
        	   $i++;
        	   $jscript_type_to_class_map .= 'user_type_classes['.$id.'] =  "'. $class .'"' . "\n";
           }
       }
       return $options;
  }

  function get_all_jurisdiction_options ($selected_id=''){
	   global $dbh;
	   
	   if(empty($selected_id)){
	     $selected_id = $_SESSION["jurisdiction_id"];
	   }
	   
     include_once "JurisdictionsList.class.inc";
     $jur_list = JurisdictionsList::getInstance($dbh);
     $list     = $jur_list->getChildJurisdictions($selected_id);
     return $list;
   }

    function print_jurisdiction_js_functions(){
   	    global $dbh, $jscript_type_to_class_map,$lang;
        ?>
        <script language="javascript" type="text/javascript">
        function print_nation_select(){
            var str = '<?php echo get_jurisdiction_select ('nation', $selected_id='')?>';
            if(document.getElementById('cur')){
   	          document.getElementById('cur').innerHTML='<?=$currency?>';
   	        }
            document.getElementById('jurisdiction_select').innerHTML = str;
            document.getElementById('jurisdiction_label').innerHTML = 'Nation';
        }

        function print_region_select(){
            var str = '<?php echo get_jurisdiction_select ('region', $selected_id='')?>';
   	        if(document.getElementById('cur')){
              document.getElementById('cur').innerHTML='<?=$currency?>';
            }
            document.getElementById('jurisdiction_select').innerHTML = str;
            document.getElementById('jurisdiction_label').innerHTML = 'Region';
        }


        function print_district_select(){
            var str = '<?php echo get_jurisdiction_select ('district', $selected_id='')?>';
   	        if(document.getElementById('cur')){
              document.getElementById('cur').innerHTML='<?=$currency?>';
            }
            document.getElementById('jurisdiction_select').innerHTML = str;
            document.getElementById('jurisdiction_label').innerHTML = 'District';
        }


        function print_club_select(){
            var str = '<?php echo get_jurisdiction_select ('club', $selected_id='')?>';
	        if(document.getElementById('cur')){
              document.getElementById('cur').innerHTML='<?=$currency?>';
            }
            document.getElementById('jurisdiction_select').innerHTML = str;
            document.getElementById('jurisdiction_label').innerHTML = 'Betting Club ';
        }

        function print_place_holder(){
            document.getElementById('jurisdiction_select').innerHTML = '[<?=$lang->getLang("Select a user type first")?>]';
            document.getElementById('jurisdiction_label').innerHTML = 'Jurisdiction';
        }

        function change_jurisdiction(select) {
          var id          = select.options[select.selectedIndex].value;
          var juris_class = user_type_classes[id];

          //alert(juris_class);
          if (juris_class == 'club'){ print_club_select(); }
          else if (juris_class == 'district'){ print_district_select(); }
          else if (juris_class == 'region'){ print_region_select(); }
          else if (juris_class == 'nation'){ print_nation_select(); }
          else { print_place_holder(); }
        }

        var user_type_classes= new Array();
       <?php echo $jscript_type_to_class_map?>
        </script>
<?php
   }

   function get_user_type_select ($selected_id=''){
   	  global $user_type_options,$lang;
   	  //var_dump($user_type_options);
  	  $str =  '<select name="user_type" onChange="change_jurisdiction(this)">';
  	  $str .= '<option value="">-'.$lang->getLang('Please select').' -</option>';

   	  foreach ($user_type_options as $key => $arr ){
   	  	   $selected = '';
   	  	   if ( $_POST['user_type'] == $arr['id'] || $selected_id == $arr['id'] ){
   	  	   	   $selected = ' selected="selected"';
   	  	   }
   	   	   $str .= '<option value="' . $arr['id']  .  '"' . $selected . '>' . $arr['name'] . '</option>';
   	  }
   	  $str .= '</select>';
   	  return $str;
   }

   function get_jurisdiction_select ($class='', $selected_id=''){
   	   global $jurisdiction_options,$lang,$hide_option_one;
   	   //var_dump($jurisdiction_options);
   	   //echo $class;
   	   if ('nation'==$class || 'region' == $class || 'district' == $class  || 'club' == $class  ){
   	   	   $options = $jurisdiction_options[$class];
           $str  = '<select name="jurisdiction">';
           if(!$hide_option_one){
           $str .= '<option value="">-'.$lang->getLang('Please select').'-</option>';
           }
           if ( is_array($options) ){
               foreach ( $options as $key => $arr ){
       	  	       $selected = '';
       	  	       if ( $_POST['user_type'] == $arr['id'] || $selected_id == $arr['id'] ){
       	  	   	       $selected = 'selected';
       	  	       }
       	   	       $str .=  '<option value="' . $arr['id']  .  '"' . $selected . '>' . $arr['name'] . '</option>';
       	       }
           }
   	       $str .=  '</select>';
   	   }
   	   else{
   	      $str = "[".$lang->getLang("Select a user type first")."]";
   	   }
   	   return $str;
   }

   $user_type_options = get_user_type_options();
   $jurisdiction_options = get_all_jurisdiction_options();

   print_jurisdiction_js_functions();
?>
