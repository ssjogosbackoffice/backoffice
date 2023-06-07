<?

class InputBox
{   
	var $Action;
	
    function InputBox($action=SELF)
    {
    	$this->Action = $action;
    }

    
    function printJavascriptFunctions()
    {
    	ob_start();
?>
			<script language="JavaScript" type="text/javascript">
			
					
			function clearInputBoxFields(id)
			{
                var form = document.forms['form'+id];
				var numelements = document.forms['form'+id].elements.length;
				
				for ( var i=0; i<numelements; i++ )
				{
                    if (	form.elements[i].type == 'text' )
                        form.elements[i].value = '';
                    else
                        if ( form.elements[i].type == 'checkbox' )
                            form.elements[i].checked = false;
                        else
                            if ( form.elements[i].type == 'select' )
                                form.elements[i].selectedIndex = 0;                
				}
			}
			
			
			// Hides an input box
			
			function hideInputBox (id)
			{   	
				//hide input box by making layer invisible
			    document.getElementById('input'+id).style.visibility = 'hidden';
			}
			
			
			function showInputBox (id, clear_fields)
			{    
			    var window_width  = document.body.clientWidth;  //get window width
		        var window_height = document.body.clientHeight; //get window height
		    	var inputbox = document.getElementById('input'+id); //get input box object (div)
		    
		    	if ( true == clear_fields )
                    clearInputBoxFields(id);
		    	
		        //set width and height of inputbox div
		        inputbox.style.left = Math.round(xClientWidth()/2 - parseInt(inputbox.style.width)/2)+'px';
		        inputbox.style.top = Math.round(xClientHeight()/2-50- parseInt(inputbox.style.height)/2)+'px';
				inputbox.style.visibility = 'visible';

				var form = document.forms['form'+id];
				var numelements = document.forms['form'+id].elements.length;
				
				for ( var i=0; i<numelements; i++ )
				{   
				    if (	form.elements[i].type == 'text' || form.elements[i].type == 'checkbox' 
				            || form.elements[i].type == 'select' )
				    {
                        form.elements[i].focus();
                        break;			          
				    }
				}
			}
			
			
			is_opera = (navigator.userAgent.toLowerCase().indexOf("opera") != -1);
			var curObj = null; /* current object wich be drag */
			var mouseY = 0;	/* mouse X */
			var moousX = 0;	/* mouse Y */
			var dx = 0;
			var dy = 0;
		
		
		    function dragThis(what)
		    {
		    	var tmp;
			    
			    if (document.all) tmp = document.all[what].style;
			    else if (document.getElementById) tmp = document.getElementById(what).style;
			    else if (document.layers) tmp=document.layers[what];
			    curObj = tmp;
			    
			    var str = "";
			    var i=0;
			    
			    str = tmp.left;	/* in ie style.left="12px" and in opere style.left="12" */
			    i= (!is_opera) ? str.substr(0,str.length-2) : str;
			    dx = mouseX - i;
			    str = tmp.top; 
			    i= (!is_opera) ? str.substr(0,str.length-2) : str;
			    	dy = mouseY - i;
			}
		
		
		    function mouseMove(e)
		    { 	mouseX = (document.all)? event.clientX : e.x;
		    	mouseY = (document.all)? event.clientY : e.y;
		        //mouseX =  event.clientX;
		        //mouseY = event.clientY;
		
		    	if (curObj)
		    	{   
		    		
		        	curObj.left= mouseX - dx;
		       		curObj.top = mouseY - dy;
		       	}
		    }
		
		
		    function mouseUp() {
		      	curObj = null;
		    }
		    
		    if (document.layers)
		    	document.captureEvents(Event.MOUSEMOVE);
		    document.onmousemove=mouseMove;
		    document.onmouseup=mouseUp;
		
			var displayedID = null;
			
			
			
			</script>
<?
		echo ob_get_clean();
    }
    
             
    /**
    * @return void
    * @param unknown $id
    * @param unknown $field_arr
    * @param unknown $buttons
    * @param unknown $title
    * @param unknown $msg
    * @param unknown $width
    * @param unknown $height
    * @param unknown $visibility
    * @param unknown $errors
    * @param unknown $hidden_input
    * @param unknown $close_action
    * @desc Enter description here...
    */
    function write($id, $field_arr, $buttons, $title, $msg, $width="400", $height="120", $visibility="hidden", $errors=array(),$hidden_input="", $close_action="")
    {   

    	//increase height of input box if errors are present
    	$num_errors =  count($errors);
    	if ( $num_errors > 0 )
    	{	for ($i=0; $i<$num_errors; $i++)
	    	{	$height += 10; 	    		
	    	}
    		
    	}    	
?>
		<script language=javascript>
		
			function doClose<?=$id?>()
			{
				hideInputBox('<?=$id?>');
				<?=$close_action;?>;
			}
		
		</script>
				
		<form id="form<?=$id?>" name="form<?=$inputbox_id?>" action="<?=$inputbox_action?>" method="POST">
			<input type=hidden name=left>
			<input type=hidden name=top>
<?		
		if ( $hidden_input )
		{
			foreach ( $hidden_input as $name => $val )
			{
?>				<input type="hidden" id="<?=$name?>" name="<?=$name?>" value="<?=$val?>">
<?				
			}			
		}
	
		$left = 0;
		$top = 0;
			
		if ( $_POST['left'] > 0  ) 
			$left = $_POST['left'];
		
		if ( $_POST['top'] > 0  ) 
			$top = $_POST['top'];
			
					
?>		
		<div id="input<?=$id?>" style="background-color:#E2DDDD;position:absolute;left:<?=$left?>px;top:<?=$top?>px;width:<?=$width?>px;height:<?=$height?>px;z-index:100;visibility:<?=$visibility?>;padding:0;border:thin solid;border-width:2;border-style:outset">
		<table cellpadding=4  cellspacing=0 bgcolor="#E2DDDD" border=0>
			<tr>
				<td class=formheading align=center colspan=2 style="background-color:#48669F;height:20;width:<?=$width?>px;"><div style="width:100%;height:100%" onmouseover="this.style.cursor = 'arrow';" onmousedown="dragThis('input<?=$id?>')" tabIndex=-1><table cellpadding=0 cellspacing=0 border=0 width=100%><tr><td width=95% align=center style="font-weight:bold;background-color:#48669F;color:white;font-size:8pt;">&nbsp;&nbsp;&nbsp;<?=$title?></td>
				<td style="background-color:#48669F"><a href="javascript:doClose<?=$id?>();"><img src="<?=image_dir?>/btn_close.gif"align=right valign=top border=0 onmousedown="javascript:void(0)" tabindex="1"></a></td></tr></table></div></td>
			</tr>
			<tr height="2">
			     <td colspan="2"><img src="<?=image_dir?>/clear.gif" width="1" height="2"/></td>
			</tr>
		<?
		if ( count($errors) > 0 )
		{	
?>
			<tr valign=top>
				<td colspan=2 class="err">
					<table cellpadding="2" cellspacing="0" border="0">
<?
					foreach ( $errors as $key=>$val )
					{
						echo '<tr valign="top"><td class="err">-</td><td class="err">' . $val .'</td></tr>';		    	
					}
			    		
?>				    </table
               </td>
			</tr>
<?		}
?>			
				</td>
			</tr>	
			<tr valign=top>
				<td colspan=2 align=center style="font-size:8pt;font-weight:bold">			
					<?=$msg?>
				</td>
			</tr>
		<?
		 	foreach ( $field_arr as $arr )
		   	{          
		?>  <tr>
			  	<td style="font-size:8pt;">&nbsp;&nbsp;<?=$arr['label']?></td>
			  	<td style="font-size:8pt;"><?=$arr['markup']?></td>
			 
			</tr>
		<? 	} 
		
		?>  
		    <tr height="1">
			     <td colspan="2"><img src="<?=image_dir?>/clear.gif" width="1" height="1"/></td>
			</tr>
		    <tr>
				<td colspan=2 align=center>
				
<?
			foreach ( $buttons as $val )
		   	{	
				echo $val . '&nbsp;';			    
			}
?>
			
				</td>
			</tr>
		</table>
		</div>
		</form>
<?   	
    }
}
?>
