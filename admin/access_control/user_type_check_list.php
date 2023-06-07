<div class="content" style="position:relative;overflow:auto;width:250px;height:180px;">
       <?php
       
           $sql = "SELECT aty_id, aty_name, aty_jurisdiction_class, aty_level" .
                  "  FROM admin_user_type" . 
                  "  WHERE aty_code != 'ONLINECREDITCARD'" . 
                  "  ORDER by aty_name";
           $rs       = $dbh->exec($sql);       
           $num_rows = $rs->getNumRows();
           
           while ($rs->hasNext()){
              $row                    = $rs->next();
           	  $aty_id                 = $row['aty_id'];
           	  $aty_name               = $row['aty_name'];
           	  $jurisdiction_class     = $row['aty_jurisdiction_class'];
           	  $aty_level              = $row['aty_level'];
           	  
           	  $selected = '';
           	  
           	  if (  $_POST['user_types'][$aty_id] || $user_types[$aty_id] )
           	      $selected = ' checked="checked"';
           	  else
           	      $selected = '';           	  
       ?>     
          <input type="checkbox" name="user_types[<?php echo $aty_id?>]"<?php echo $selected?>><?php echo $aty_name?> <span style="font-style:none;color:gray">(<?php echo $jurisdiction_class?>)</span><br />
       
       <?php
           }
       ?>
</div>   