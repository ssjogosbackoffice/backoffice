    <script language="javascript" type="text/javascript">
        function doRefresh(form)
        { 
            var msg = "<?=$lang->getLang("Refreshing will reload the saved description for this task, causing any unsaved changes to be lost.")?>\n\n";
                msg += "<?=$lang->getLang("Are you sure you want to do this?")?>";
                
           if ( window.confirm(msg) )
               form.submit();                 	
        }
        function sendForm(operation){
            $('#sub_update').val(operation);
        }
    </script>
    
    
   <div class=bodyHD>Administrator Task Definitions - Edit</div>
   <br />
    
   <?php if ( areErrors() ) { showErrors(); echo "<br /><br />";}?>
   
   <form action="<?=$_SERVER['PHP_SELF']?>" method="POST">
   <input type="hidden" name="op" value="Change" />
   <input type="hidden" name="task_code" value="<?=$task_code?>" />
   <table cellpadding= "4" cellspacing=1 border=0>
       <tr valign="top">
	       <td class=label><?=$lang->getLang("Task Code")?></td>
		   <td class=content>		    
		      <?=$task_code?>
    	   </td>
    	   <td>&nbsp;</td>
    	   <td rowspan="3">&nbsp;</td>
           <td rowspan="3" class=small width="160">
    	        <input type="submit" id="sub_update" name="op" style="width:80px;margin-bottom:2px;" onclick="sendForm('Update');" value="<?=$lang->getLang("Update")?>" />
    	        <br />   
    	        <input type="button" id="btn_refresh" name="op" style="width:80px;margin-bottom:2px" value="<?=$lang->getLang("Refresh")?>" onClick="doRefresh(this.form)"/>
    	        <br />
    	        <input type="button" style="width:80px;margin-bottom:2px" value="<?=$lang->getLang("Back To List")?>" onClick="window.location='<?=$_SERVER['PHP_SELF'];?>'"/>
    	        <br />
    	    </td>
	   </tr> 
		<tr valign="top">
		   <td class=label><?=$lang->getLang("Task Description")?></td>
		   <td class=content><textarea id="task_description" name="task_description" cols="30" rows="10"><?=$task_description?></textarea></td> 
		   
		</tr>
	
    	<tr valign="top">
	       <td class=label><?=$lang->getLang("Administrator Users with Access")?></td>
		   <td class=content>	
		   
		      <?php  require_once 'user_type_check_list.php'?>
    	   </td>
    	   <td>&nbsp;</td>
	   </tr> 
    </table>
    </form>