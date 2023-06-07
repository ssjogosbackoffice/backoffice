    function showDisablePane() {
      var pane = document.getElementById('disable_pane');
      pane.style.width  = xClientWidth();
      pane.style.height = xClientHeight();
      pane.style.visibility = 'visible';
    }

    function hideDisablePane() {
      var pane = document.getElementById('disable_pane');
      pane.style.visibility = 'hidden';
    }
    
    function sortTable (orderby,direction) {
      document.getElementById('orderby').value   = orderby;
      document.getElementById('direction').value = direction;                
      retrieveURL('<?=secure_host?>/admin/admin/admin_users/admin_user_list.php?orderby='+orderby+'&direction='+direction, 'updateTable', true);
    }
    
    function retrieveURL(url, resultFunction, show_loading_bar)  {
      if ( true == show_loading_bar ) {
        showLoadingBar();
        showDisablePane();  
      }

      if (window.XMLHttpRequest) { // Non-IE browsers
        req = new XMLHttpRequest();
        req.onreadystatechange = eval(resultFunction);
        try {
          req.open("GET", url, true);
        } catch (e) {
          alert(e);
        }
        req.send(null);
      } else if (window.ActiveXObject) { // IE
        req = new ActiveXObject("Microsoft.XMLHTTP");
        if (req) {
          req.onreadystatechange = eval(resultFunction);
          req.open("GET", url, true);
          req.send();
        }
      }
    }

    function updateTable() {
      if (req.readyState == 4) {
        // Complete
        if (req.status == 200) {
          // OK response
          document.getElementById(contentArea).innerHTML = req.responseText;
        } else {
          alert("Status: " + req.status + " Problem: updateTable " + req.statusText);    
        }
        hideLoadingBar();
        hideDisablePane();
      }        
    }
    
    
    function updateTableAndAlert(msg) {
      if (req.readyState == 4) {
        hideLoadingBar();
        hideDisablePane();
        // Complete
        if (req.status == 200) {
          // OK response
          document.getElementById(contentArea).innerHTML = req.responseText;
          alert(msg);                
        } else {
          alert("Status: " + req.status + " Problem: updateTableAndAlert " + req.statusText);    
        }
      }
    }
    
    
    function alertResponse() {
      if (req.readyState == 4) {
        // Complete    
        if (req.status == 200) {
          // OK response
          var msg = req.responseText;
          func =  function () { updateTableAndAlert(msg); }
          retrieveURL('<?=secure_host?>/admin/admin_users/admin_user_list.php?orderby=<?=$orderby?>&direction=<?=$direction?>',func);
        } else {
          alert("Status: " + req.status + " Problem: alertResponse " + req.statusText);    
        }
      }
    }
    
    function deleteUser(name, id, orderby, direction) {
      deleteuser = confirm('Delete admin user "' + name + '"?');
      if(deleteuser) {
        retrieveURL('<?=secure_host?>/admin/admin_users/admin_user_delete.php?id=' + id, 'alertResponse', true);            
      }
    }
    var contentArea = 'urlContent';
