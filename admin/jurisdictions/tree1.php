    <script language="javascript">
    
        function node_msover (div, juris_class)
        {  
        	div.style.backgroundColor = 'blue';
        	div.style.color = 'white';
        	div.style.cursor = 'pointer';
        	////alert(div.style.width);
        	//showNodeOptions(div,juris_class);        	
        }
        
        function node_msout (div, juris_class)
        {        	
        	div.style.backgroundColor = 'white';
        	div.style.color = '#122970';
        }    
        
        function deleteNode (node, name, jur_class)
        {
        	if ( window.confirm('Delete the ' + jur_class + ' "' + name + '"?') )
        	   window.location = '<?=$_SERVER['PHP_SELF'];?>?op=Delete&node=' + node; 
        }    
        
        function showNodeMenu(div, juris_class)
        {
        	//alert(div.style.width);\
        	var x=event.clientX;
			var y=event.clientY;
			
			var button = event.button;
		//	div.style.backgroundColor = 'blue';
        //	div.style.color = 'white';
        //	div.style.cursor = 'pointer';
        	
            if ( 2 == button )
            {
			   //div.style.left = x + 2;
               document.getElementById('region_popup').style.left = x;
               document.getElementById('region_popup').style.top = y;
               document.getElementById('region_popup').style.visibility ='visible';	          
            }
        }
        
    </script> 
    
    <div id="region_popup" 
        style="position:absolute;left:0px;top:0px;visibility:hidden;background-color:gray;font-size:8pt;z-index:57"
        onmouseout="document.getElementBy='hidden'" onmouseout="this.style.visibility='hidden'">
        Edit<br>
        Add a District<br>
    </div>    
    
    <?php

    function print_tree ($tree, $curr_node, $level=0)
    {  
    	global $node, $dbh;
    	
    	static $height_offset = HEIGHT_OFFSET;
   	
    	$label = $tree[$curr_node]['jur_name'];
    	
    	if ( 0 == $level )
    	{	
    		echo '<div style="position:relative;background-color:#ffffff;font-size:8pt" class="tree_menu_item">';
    	    	              /*onmouseover="node_msover(this, \''.$tree[$curr_node]['jur_class'].'\')" 
    	    	              onmouseout="node_msout(this, \''.$tree[$curr_node]['jur_class'].'\')"
    	    	              onClick="window.location=\'' . $_SERVER['PHP_SELF'] . "?node=" . $curr_node . '\'"*/
    		
    		echo '<b>' . mb_strtoupper($label) . '</b>';
    		echo "</div>";
    		//$height_offset +=20;
    	}
    	else
    	{   
    		if ( 'region' == $tree[$curr_node]['jur_class'] )
    		{
    			echo mb_strtoupper($label);
    		}
    		else
    		{  
    			echo $label;
    		}
    		echo '</div></td>';
    		
    		
    		if(!$tree[$curr_node]['children'])
    		{
    		   // make sure that the node is not part of the internet branch, as it is
    		   // the default assignment for new customers
    		   
    		   if(strtolower($tree[$curr_node]['jur_name']) != 'internet')
    		   {
       		   // only create delete node links to those node's below our jurisdiction class    
       		   // 
    	   	   // this stops a region/district manager from deleting their own region/district,
    		      //	that should be left up to their parent admin (casino and region users respectively)
    		       	
    	    	   $display_delete_node = false;
    	    	
       	    	switch($_SESSION['jurisdiction_class'])
       	    	{
    	       	   case 'casino':
    	       	      $display_delete_node = true;
    	    	         break;
    	    	      
       	    	   case 'region':
       	    	      if($tree[$curr_node]['jur_class'] == 'district' || $tree[$curr_node]['jur_class'] == 'club')
    	       	         $display_delete_node = true;
    	       	      break;
    	    	      
    	    	      case 'district':
    	    	         if($tree[$curr_node]['jur_class'] == 'club')
    	    	            $display_delete_node = true;
    	    	         break;
    	    	   }    	
    	    	
    	    	   if($display_delete_node)
    	    	   {
    	    	      // make sure that this node has no users attached to it
    	    	      $sql = 'SELECT pun_id FROM punter WHERE pun_betting_club = ' . $tree[$curr_node]['jur_id'] . ' ' .
    	    	             'UNION ALL (SELECT aus_id FROM admin_user WHERE aus_jurisdiction_id = ' . $tree[$curr_node]['jur_id'] . ')';
    	    	          
    	    	      $rs  = $dbh->exec($sql);
    	    	   
    	    	      
    	    	      // if there are no users attached, display the delete icon
    	    	      if($rs->getNumRows() <= 0)
    	    	      {
       		         echo '<td>';
    		    		
       		         $link = "javascript:deleteNode(" . $curr_node . ", '" . $label . "', '" . $tree[$curr_node]['jur_class'] ."');";
       	   	      $alt  = 'delete ' . $label . ' ' . $tree[$curr_node]['jur_class'];
    		    		
       		         echo '<a href="' . $link . '" >';
    	   	         echo '<img src="' . image_dir . '/jurisdiction_del.gif" alt="' . $alt . '" border="0"/>';
    		            echo '</a>';
    		            echo '</td>';
    	    	      }
    	    	   }
    	    	}
    		}   
    		    
    		echo '</tr></table>';
    		
    	}
    	    	

    	$children = $tree[$curr_node]['children'];    	

    	if ( $children )
    	{ 
   	   foreach ( $children as $child_node )
    	   {   
    	      $height_offset += 24;
    	    	$left = $par_div_left + 20 * $level+1;
    	    	$width = mb_strlen($tree[$child_node]['jur_name']) * 6;
    	    	    	    	
    	    	if($child_node == $node)
    	    	{   
    	    	   $bgcolor     = 'white';
    	    	   $color       = 'red';
    	    	   $font_weight = 'bold';
    	    	}
    	    	else
    	    	{   
    	    		$bgcolor     = 'white';
    	    	   $color       = '#122970';
    	    	   $font_weight = 'normal';
    	    	}
    	    	
    	    	
    	    	// only create links to those node's below our jurisdiction class    	    	
    	    	$display_node_as_link = false;
    	    	
    	    	switch($_SESSION['jurisdiction_class'])
    	    	{
    	    	   case 'casino':
    	    	      $display_node_as_link = true;
    	    	      break;
    	    	      
    	    	   case 'region':
    	    	      if($tree[$child_node]['jur_class'] == 'district' || $tree[$child_node]['jur_class'] == 'club')
    	    	         $display_node_as_link = true;
    	    	      break;
    	    	      
    	    	   case 'district':
    	    	      if($tree[$child_node]['jur_class'] == 'club')
    	    	         $display_node_as_link = true;
    	    	      break;
    	    	}    	    	
    	    	
    	    	// if the current node is above or equal to our jurisdiction class
    	    	// make it black, this season's new ... black.
    	    	
    	    	if($display_node_as_link == false)
    	    	{
    	    	   $color       = "#000000";
    	    	   $font_weight = "bold";
    	    	}
    	    	
    	    	echo '<table cellpadding="0" cellspacing="0">';
    	    	echo '<tr><td>';
    	    	echo '<img src="' . image_dir . '/clear.gif" height="13" width="' . ($level*13) . '" />';
    	    	echo '<img src="' . image_dir . '/arrow_tree.gif" width="13" height="13" />';   
    	    	echo "</td>";
    	    	echo '<td>';	
    	    	
    	    	echo '<div id="node'.$child_node.'" style="border:none;position:relative;padding:2px;color:'.$color.';background-color:'.$bgcolor.';font-size:8pt;font-weight:'.$font_weight.'"'; 
    	    	   
    	    	if($child_node != $node && $display_node_as_link == true)
    	    	{ 
    	    	    echo ' onmouseover="node_msover(this, \''.$tree[$child_node]['jur_class'].'\')"  
    	    	           onmouseout="node_msout(this, \''.$tree[$child_node]['jur_class'].'\')"
    	    	           onClick="window.location=\'' . $_SERVER['PHP_SELF'] . "?node=" . $child_node . '\'"';
    	    	}
    	    	echo  '>';
    		    
    		   print_tree($tree, $child_node, $level+1);	
    	    }   	    	
    	}
    }

    // fake a jurisdiction id for top level (casino) admin user
    $jur_id = ($_SESSION['jurisdiction_id']) ? $_SESSION['jurisdiction_id'] : 1;
    
            // retrieve the appropriate section of the jurisdiction tree
    $sql  = 'SELECT jur_id, jur_name, jur_class,  coalesce(jur_parent_id, \'0\') as parent_id ' .            
            'FROM jurisdiction, account ' .
            "WHERE jur_name !=  'Internet' AND (" .
            // current level (district/region/casino)            
            'jur_id =  '. $jur_id . ' ' .
            // parent
            'OR (jur_id = (SELECT jur_parent_id FROM jurisdiction WHERE jur_id = ' . $jur_id . ')) ' .
            // grandparent
            'OR (jur_id = (SELECT jur_parent_id FROM jurisdiction WHERE jur_id = (SELECT jur_parent_id FROM jurisdiction WHERE jur_id = ' . $jur_id . '))) ' .
            // children 
            'OR (jur_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = ' . $jur_id . ')) ' .
            // grandchildren
            'OR (jur_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = ' . $jur_id . '))) ' .
            // great grandchildren
            'OR (jur_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id IN (SELECT jur_id FROM jurisdiction WHERE jur_parent_id = ' . $jur_id . '))))' .
            // superfluous 7th comment
            ") " .
            'order by jur_name';

    $rs  = $dbh->exec($sql);
    $num_rows = $rs->getNumRows();

    if ( $num_rows > 0 )
    {
        $row = $rs->next();       
        
        while ( $row )
        {   
        	$jur_id = $row['jur_id'];
       	    $tree[$jur_id] = $row;       	
       	    $row = $rs->next();
        }   
    }
    
    
    foreach ( $tree as $id => $value )
    {  
    	$par_id = $tree[$id]['parent_id'];
    	   
    	if ( $par_id )
    	    $tree[$par_id]['children'][] = $id;
    	else
    	      $root = $id;
    }
    

    print_tree($tree, $root, $level=0);
?>