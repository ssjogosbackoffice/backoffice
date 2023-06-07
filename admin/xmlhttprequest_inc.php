<script language="Javascript">
    var req;
    var which;
    function retrieveURL(url) {
        if (window.XMLHttpRequest) { // Non-IE browsers
          req = new XMLHttpRequest();
          req.onreadystatechange = processStateChange;
          try {
            req.open("GET", url, true);
          } catch (e) {
            alert(e);
          }
          req.send(null);
        } else if (window.ActiveXObject) { // IE
          req = new ActiveXObject("Microsoft.XMLHTTP");
          if (req) {
            req.onreadystatechange = processStateChange;
            req.open("GET", url, true);
            req.send();
          }
        }
    }

    function processStateChange(){
        if (req.readyState == 4){ 
            // Complete
            if (req.status == 200){   
                // OK response
                document.getElementById(contentArea).innerHTML = req.responseText;
            }else{
                alert("Problem: " + req.statusText);    
            }
        }
    }
    var contentArea = 'urlContent';
    var func = null;
</script>