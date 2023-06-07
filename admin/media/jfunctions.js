/* 
calendarPopup
- pops up calendar (in site html root)
*/
function calendarPopup(daySelect, monthSelect, yearSelect)
{  var day = daySelect.options[daySelect.selectedIndex].value;
   var month = monthSelect.options[monthSelect.selectedIndex].value;
   var year = yearSelect.options[yearSelect.selectedIndex].value;
   var targetForm = daySelect.form.name;

   openWindow ('/media/calendar_selector_pop.php?day_name=day&month_name=month&year_name=year&targetForm='+targetForm+'&day='+day+'&month='+month+'&year='+year, 'calendarselector', 270, 230, 'no','no');
}


/* 
msover
- Simple image "mouse over" event handler.  
- Assumes mouseover image variable name to be suffixed with "_off"
*/
function msover (imgName) 
{	var new_im = null;
	if ( document.images ) 
	{
		new_im = eval(imgName + "_on.src");
		document.images[imgName].src = new_im;
	}
    
}
 

/* 
msout
- Simple image "mouse out" event handler.  
- Assumes mouseout image variable name to be suffixed with "_off"
*/
function msout ( imgName )
{	var new_im = null;
	
	if ( document.images )
	{	new_im = eval(imgName + "_off.src");
		document.images[imgName].src = new_im;
	}
}  



/* 
msover2
- Image "mouse over/out" event handler to change a specific image in the HTML document
- Takes image "tag" name and new image variable name and replaces src attribute of the image tag
*/
function imageChange (imgTagName, newImgName)
{  var new_im = null;
   
	if ( document.images )
	{	new_im = eval(newImgName + ".src");
		document.images[imgTagName].src = new_im;
	}
}



/* 
openWindow
- Opens a new window with no menubars
- Centers window on screen
*/
function openWindow (url, windowname, width, height, status, scrollbars)
{	var windowFeatures = "";
	var win;

   var screenX = Math.floor(screen.availWidth/2 - width/2);
   var screenY = Math.floor(screen.availHeight/2 - height/2);

   windowFeatures  = "toolbar=0,resizable=yes,dependent=yes,personalbar=0";
   windowFeatures += ",scrollbars="+scrollbars+",menubar=0";
   windowFeatures += ",height="+height+",width="+width+",status="+status;   
   windowFeatures += ",screenX="+screenX+",screenY="+screenY;   
   windowFeatures += ",left="+screenX+",top="+screenY;   

   win = window.open(url, windowname, windowFeatures);
}


function clearPlaceHolder(textobject, placeholdertext)
{	if ( textobject.value == placeholdertext )
	{	textobject.value = ""
	}
}



function openInfoWindow(url)
{	var windowFeatures = "";
	var win;

	windowname='x';
	windowFeatures  = "toolbar=0,resizable=no,dependent=yes,personalbar=0";
    windowFeatures += ",scrollbars=yes,menubar=0,width=500,height=350,status=no";   

	win = window.open(url, windowname, windowFeatures);
	win.focus();
}

function getAttributes(url) {
	var arr = url.split('?');

	alert(arr[1]);
}

function openNoteWindow (url, windowname)
{  openWindow (url, windowname, 400, 300, 'no', 'yes')
}


function replaceAttribute(url, attributename, value)
{  var attrPair = "";

   //if no query string 
   if ( url.indexOf('?') == -1 )
      return url + '?' + attributename + '=' + value;
   
   //split url into http path and query string components
   var urlArr = url.split('?');
   var httpPath = urlArr[0];  //actual http file path
   var qStr  = urlArr[1];  //query string component, contains attribute=value strings
   
   //break up query string into cells of attriubute=value strings
   var qStrArr = qStr.split('&');
   var numAttributes = qStrArr.length;
   var newQStrArr = new Array(numAttributes);
   
   if ( numAttributes > 0  ) //attributes exist
   {  for ( var i=0; i<numAttributes; i++ )
      {  
         attrPair = qStrArr[i];  //get attribute/value pair
         attrPairArr = attrPair.split('='); //split attribute into array(attrname, attrvalue)

         if ( attributename.toLowerCase() == attrPairArr[0].toLowerCase() ) //if attributename function 
         {                                                        //...parameter matches 
            attrPairArr[1] = value;                           //... this attribue name, change value cell
            attrPair = attrPairArr.join('=')     //join into string
         }
         newQStrArr[i] = attrPair;   //assign new attribute/value pair to new qstr array
      }  

      var newQStr = newQStrArr.join('&');
      var newUrl = httpPath + '?' + newQStr;
   }
   else
      var newUrl = httpPath + '?' + attributename + '=' + value;

   return newUrl;
}


//no scroll, no status, not resizable
function openWindow1(url, width, height) {        
		var w_options = "toolbar=no,directories=no,menubar=no,width=" + width + ", height="+ height +",location=no,scrollbars=no,resizable=no,status=no";

        WN=window.open(url, 'newWin', w_options)
        WN.focus();
        WN.moveTo(20,20);
        
}


//scroll, no status, not resizable
function openWindow2(url, width, height) {        
		var w_options = "toolbar=no,directories=no,menubar=no,width=" + width + ", height="+ height +",location=no,scrollbars=yes,resizable=no,status=yes";

        WN=window.open(url, 'newWin', w_options)
        WN.focus();
        WN.moveTo(20,20);
}

//scroll and status, resizable
function openWindow3(url, width, height) {        
		var w_options = "toolbar=no,directories=no,menubar=no,width=" + width + ", height="+ height +",location=no,scrollbars=yes,resizable=no,status=yes";

        WN=window.open(url, 'newWin', w_options)
        WN.focus();
        WN.moveTo(20,20);
}



// x_core.js (extract), X v3.15.4, Cross-Browser.com DHTML Library
// Copyright (c) 2004 Michael Foster, Licensed LGPL (gnu.org)

var xVersion='3.15.3',xNN4,xOp7,xOp5or6,xIE4Up,xIE4,xIE5,xUA=navigator.userAgent.toLowerCase();
if (window.opera){
  xOp7=(xUA.indexOf('opera 7')!=-1 || xUA.indexOf('opera/7')!=-1);
  if (!xOp7) xOp5or6=(xUA.indexOf('opera 5')!=-1 || xUA.indexOf('opera/5')!=-1 || xUA.indexOf('opera 6')!=-1 || xUA.indexOf('opera/6')!=-1);
}
else if (document.all && xUA.indexOf('msie')!=-1) {
  xIE4Up=parseInt(navigator.appVersion)>=4;
  xIE4=xUA.indexOf('msie 4')!=-1;
  xIE5=xUA.indexOf('msie 5')!=-1;
}
else if (document.layers) {xNN4=true;}
xMoz=xUA.indexOf('gecko')!=-1;
xMac=xUA.indexOf('mac')!=-1;

// experimenting with CSS1Compat:
function xClientWidth() {
  var w=0;
  if(xOp5or6) w=window.innerWidth;
  else if(document.compatMode == 'CSS1Compat' && !window.opera && document.documentElement && document.documentElement.clientWidth)
    w=document.documentElement.clientWidth;
  else if(document.body && document.body.clientWidth)
    w=document.body.clientWidth;
  else if(xDef(window.innerWidth,window.innerHeight,document.height)) {
    w=window.innerWidth;
    if(document.height>window.innerHeight) w-=16;
  }
  return w;
}
// experimenting with CSS1Compat:
function xClientHeight() {
  var h=0;
  if(xOp5or6) h=window.innerHeight;
  else if(document.compatMode == 'CSS1Compat' && !window.opera && document.documentElement && document.documentElement.clientHeight)
    h=document.documentElement.clientHeight;
  else if(document.body && document.body.clientHeight)
    h=document.body.clientHeight;
  else if(xDef(window.innerWidth,window.innerHeight,document.width)) {
    h=window.innerHeight;
    if(document.width>window.innerWidth) h-=16;
  }
  return h;
}

function disableInput(){
	inputs = document.getElementsByTagName("input");

	for(i = 0 ; i < inputs.length ; i++){
		inputs[i].disabled = true;
	}

	forms = document.getElementsByTagName("form");

	for(i = 0 ; i < forms.length ; i++){
			forms[i].disabled = true;
	}
	
	var loading = document.getElementById("loading");
	loading.style.display = 'block';

}

function startDisableForm(){
	setTimeout(disableInput, 10);
}

function initFunction(){
	forms = document.getElementsByTagName("form");

	for(i = 0 ; i < forms.length ; i++){
			forms[i].onsubmit = startDisableForm;
	}
}
function getCredits()
{
    $.post("/services/jurisdiction_services.inc",{action:"0"}, function(data){
    credits=data.split('~');
        $("#jurisdiction_credit").fadeOut("slow").fadeIn("slow").empty().append( credits[0]);
        $("#pending_credit").fadeOut("slow").fadeIn("slow").empty().append( credits[1]);
        $("#overdraft_credit").fadeOut("slow").fadeIn("slow").empty().append( credits[2]);

    });
};

window.onload = initFunction;