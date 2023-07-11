<?php
  
  function get_all_jurisdiction_options ($selected_id='') {
     global $dbh;
     include_once "JurisdictionsList.class.inc";
     $jur_list = JurisdictionsList::getInstance($dbh);
     $ret = $jur_list->getChildJurisdictions($_SESSION['jurisdiction_id']);
     return $ret;
   }

    function print_jurisdiction_js_functions(){
   	    global $dbh, $jscript_type_to_class_map;
        ?>
        <script language="javascript" type="text/javascript">
<?php if ( 'casino' == $_SESSION['jurisdiction_class'] )  { ?>
        function print_nation_select(id){
            var str = '<?php echo get_jurisdiction_select ('nation', $selected_id='')?>';
            document.getElementById(id).innerHTML = str;
        }
<?php }?>

<?php if ( 'casino' == $_SESSION['jurisdiction_class'] || 'nation' == $_SESSION['jurisdiction_class'])  { ?>
        function print_region_select(id){
            var str = '<?php echo get_jurisdiction_select ('region', $selected_id='')?>';
            document.getElementById(id).innerHTML = str;
        }

<?php } ?>
<?php if ( 'casino' == $_SESSION['jurisdiction_class'] || 'nation' == $_SESSION['jurisdiction_class'] || 'region' == $_SESSION['jurisdiction_class'] )  { ?>
        function print_district_select(id){
            var str = '<?php echo get_jurisdiction_select ('district', $selected_id='')?>';
            document.getElementById(id).innerHTML = str;
        }
<?php } ?>

<?php if ( 'casino' == $_SESSION['jurisdiction_class'] || 'nation' == $_SESSION['jurisdiction_class'] ||
           'region' == $_SESSION['jurisdiction_class'] || 'district' == $_SESSION['jurisdiction_class'])  { ?>
        function print_club_select(id){
            var str = '<?php echo get_jurisdiction_select ('club', $selected_id='')?>';
            document.getElementById(id).innerHTML = str;
        }
<?php }?>
      function print_place_holder(id){
          id = typeof id !== 'undefined' ? id : 'jurisdiction_select';
          document.getElementById(id).innerHTML = str;
      }

      function change_jurisdiction(select,id){
          id = typeof id !== 'undefined' ? id : 'jurisdiction_select';
        var juris_class = select.options[select.selectedIndex].value;
        switch ( juris_class ){
<?php if ( 'casino' == $_SESSION['jurisdiction_class'])  { ?>
      	        case 'nation' :
      	            print_nation_select(id);
      	            break;
<?php } ?>
<?php if ( 'casino' == $_SESSION['jurisdiction_class'] || 'nation' == $_SESSION['jurisdiction_class'] ||
           'region' == $_SESSION['jurisdiction_class'] || 'district' == $_SESSION['jurisdiction_class'])  { ?>
                case 'club' :
      	            print_club_select(id);
      	            break;
<?php } ?>
<?php if ( 'casino' == $_SESSION['jurisdiction_class'] ||'nation' == $_SESSION['jurisdiction_class'] || 'region' == $_SESSION['jurisdiction_class'])  { ?>
      	        case 'district' :
      	            print_district_select(id);
      	            break;
<?php } ?>
<?php if ( 'casino' == $_SESSION['jurisdiction_class'] ||  'nation' == $_SESSION['jurisdiction_class'] )  { ?>
      	        case 'region' :
      	            print_region_select(id);
      	            break;
<?php } ?>
      	        default :
      	           print_place_holder(id);
      	           break;
            }
        }

        function show_one_jurisdiction_level (level){
       	  var select = document.getElementById('jurisdiction_level');
          var num_options = select.options.length;

          for ( var i=0; i<num_options; i++ ){
            select.options[i--] = null;
            num_options--;
          }
          if ( 'nation' == level ){
            nation_option = new Option('National', 'nation');
	    select.options[0] = nation_option;
          }
          else{
            if ( 'region' == level ){
              region_option = new Option('Region', 'region');
	      select.options[0] = region_option;
            }
            else{
              if ( 'district' == level ){
                district_option = new Option('District', 'district');
	        select.options[0] = district_option;
              }
              else{
                if ( 'club' == level ){
        	  club_option = new Option('Betting Club ', 'club');
	          select.options[0] = club_option;
        	}
              }
            }
          }
          change_jurisdiction(select);
        }

        function remove_jurisdiction_level (level){
        	var select = document.getElementById('jurisdiction_level');
        	var num_options = select.options.length;

        	for ( var i=0; i<num_options; i++ ){
                if ( select.options[i].value == level ){
                  select.options[i] = null;
                  break;
                }
        	}
  	     }

        function select_jurisdiction_level (level) {
          var select = document.getElementById('jurisdiction_level');
          var num_options = select.options.length;
          for (var i=0; i < num_options; i++) {
            if (select.options[i].value == level) {
              select.selectedIndex = i;
              change_jurisdiction(select);
              break;
            }
          }
        }

        function show_all_jurisdiction_levels (){
        	var select = document.getElementById('jurisdiction_level');
        	var num_options = select.options.length;

        	for ( var i=0; i<num_options; i++ ){
                select.options[i--] = null;
                num_options--;
        	}
        	club_option = new Option('Betting Club ', 'club');
        	select.options[num_options++] = club_option;
<?php      if ( 'casino' == $_SESSION['jurisdiction_class'] || 'nation' == $_SESSION['jurisdiction_class'] ){?>
                nation_option = new Option('Nation', 'nation');
                select.options[num_options++] = nation_option ;
                region_option = new Option('Region', 'region');
	            select.options[num_options++] = region_option;
                district_option = new Option('District', 'district');
	            select.options[num_options++] = district_option;
<?php       }elseif ( 'region' == $_SESSION['jurisdiction_class'] ){
?>	            district_option = new Option('District', 'district');
	            select.options[num_options++] = district_option;
<?php       }
?>  	}
        </script>
<?php
   }

   function get_jurisdiction_select ($class='', $selected_id=''){
   	   global $jurisdiction_options, $hide_option_one, $lang;
   	   $str = '';
   	   if ( 'casino' == $class || 'nation' == $class ||  'region' == $class || 'district' == $class  || 'club' == $class  ) {
   	      if ( 'nation' == $class )
   	         $option_one = $lang->getLang("All Nations");
   	      elseif ( 'region' == $class )
   	         $option_one = $lang->getLang("All Regions");
   	      elseif ( 'district' == $class )
   	         $option_one = $lang->getLang("All Districts");
          elseif ( 'club' == $class )
   	         $option_one = $lang->getLang("All Betting Clubs");
   	      $options = $jurisdiction_options[$class];
          $str  = '<select name="jurisdiction">';
          if ( is_array($options)  ){
            if(!$hide_option_one && count($options) > 1)
              $str .= '<option value=""> - ' . $option_one .'- </option>';
   	          foreach ( $options as $key => $arr ){
   	  	        $selected = '';
   	  	        if ( (isset($_POST['jurisdiction']) && $_POST['jurisdiction'] == $arr['id']) || $selected_id == $arr['id'] ){
   	  	   	      $selected = 'selected';
   	  	        }
   	   	        $str .=  '<option value="' . $arr['id']  .  '"' . $selected . '>' . str_replace("'", "\'", $arr['name']) . '</option>';
   	          }
   	          $str .=  '</select>';
            }
   	     }
   	     return $str;
   }

   $jurisdiction_options = get_all_jurisdiction_options();
   print_jurisdiction_js_functions();
?>
