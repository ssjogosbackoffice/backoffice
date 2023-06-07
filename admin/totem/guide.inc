<?php
//check_access('manage_totems', true);
$jurisdiction_id    = $_SESSION["jurisdiction_id"];
$jurisdiction_class = $_SESSION["jurisdiction_class"];

?>
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<style>
ol li{
  margin-bottom:5px;
}
</style>
<div class="bodyHD"><?=$lang->getLang("Guida per l'installazione del software del totem e la sua gestione")?></div>
<div style="width:500px;">
<?=$lang->getLang("Per installare il programma per il totem bisogna assicurarsi che sia presente nella macchina il seguente software:")?><br/>
<ol>
  <li><?=getLink("http://www.microsoft.com/downloads/it-it/details.aspx?familyid=333325fd-ae52-4e35-b531-508d977d32a6&displaylang=it", "NET Framework 3.5", null, "_blank")?></li>
  <li><?=$lang->getLang("Identificare la porta come del lettore")?></li>
</ol>
<br/>
    <?=$lang->getLang("Dopo aver installato questi programmi, bisogna scaricare il programma del totem, disponibile per il download a ")?><?=getLink("/totem/install/BarneyInstaller.exe", $lang->getLang("questo link"))?>,<br/>
<?=$lang->getLang("verr' scaricato il file BarneyInstaller.exe, che si consiglia di lasciare sul Desktop.")?><br/><br/>
<?=$lang->getLang("Bisognera' a questo punto lanciare il programma BarneyInstall.exe e seguire i seguenti passaggi:")?><br/>
<ol>
  <li><?=$lang->getLang("Seguire i passaggi di installazione fino al riavvio del computer")?></li>
  <li><?=$lang->getLang("Al riavvio selezionare la porta del lettore di banconote")?></li>
  <li><?=$lang->getLang("Attendere il termine dell'installazione.")?></li>
  <li><?=$lang->getLang("Al primo avvio del sistema verra' richiesto il codice agenzia. Il codice e' quello che appare in alto dove e' scritto Totem Code <i>(in caso di accesso con un'utente di livello superiore a club entrare con le credenziali del club sotto quale viene installato il totem)")?></i></li>
</ol>

  <h3>Software [ <span style="color:blue"><?=(substr(DOMAIN,0,1)=='.'?substr(DOMAIN,1) : DOMAIN)?></span> ]</h3>
  <table class="table display table-hover table-striped" style="width: 500px;margin: auto;margin-left:0">
    <thead> <tr><th>Nr</th><th><?=$lang->getLang("File")?></th><th><?=$lang->getLang("Download")?></th></tr></thead>
    <tbody>
    <tr><td>1</td><td><?=$lang->getLang("Net Framework 3.5")?></td>
      <td>
        <a  class="btn btn-medium" target="_blank" href="<?= "http://www.microsoft.com/downloads/it-it/details.aspx?familyid=333325fd-ae52-4e35-b531-508d977d32a6&displaylang=it" ?>" ><i class="icon-download"></i> <?=$lang->getLang("Download")?></a>
      </td>
    </tr> <tr><td>2</td><td><?=$lang->getLang("Totem Installer")?></td>
      <td>
        <a  class="btn btn-medium" href="<?= "/totem/install/BarneyInstaller.exe" ?>" ><i class="icon-download"></i> <?=$lang->getLang("Download")?></a>
      </td>
    </tr>

    </tbody>
  </table><br /><br />
