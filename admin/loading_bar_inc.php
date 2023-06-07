<script language="javascript" type="text/javascript">
    
    function hideLoadingBar()
    {   
        document.getElementById('loading_bar').style.visibility = 'hidden';
        // hideDisablePane();
    }
    
    function showLoadingBar()
    {   
        var loading_bar = document.getElementById('loading_bar');
        loading_bar.style.left = Math.round(xClientWidth()/2 - parseInt(loading_bar.style.width)/2)+'px';
		loading_bar.style.top  = Math.round(xClientHeight()/2-50- parseInt(loading_bar.style.height)/2)+'px';
		loading_bar.style.visibility = 'visible';
		document.getElementById('img_load_anchor').focus();
		//showDisablePane();
    }
    
</script>
<?php

    $event_handler = "if ( window.event || window.Event ) {  if (event.which == 0 || event.keyCode == 9 ) { document.getElementById('img_load_anchor').focus();  } return false; }";
    
    if ( browserName() == 'ie' )
        $event = ' onKeyDown="'.$event_handler.'"';
    else
        $event = ' onKeyPress="'.$event_handler.'"';
    
?>

<div id="loading_bar"  style="position:absolute;text-align:center;padding:4px;border: thin solid;border-style:outset;background-color:#E2DDDD;z-index:100;width:180px;height:60px;visibility:hidden"><table width="100%" height="100%">
        <tr>
            <td style="font-size:10pt;font-type:arial;color:blue"><?=$lang->getLang("Loading. Please wait...")?></td>
        </tr>
        <tr>
            <td>
                <a href="javascript:void(0)" style="cursor:pointer" id="img_load_anchor"<?=$event?> tabIndex="1"><img id="img_load_bar" src="<?=image_dir?>/progress_bar.gif"  border="0"></a>
            </td>
        </tr>
    </table>
</div>