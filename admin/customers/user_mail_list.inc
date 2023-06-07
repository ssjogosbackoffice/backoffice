<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<div class="well">
    <div class="centered"><h1 class='text-error'><?=$lang->getLang("Active users mail list")?></h1></div><br>
    <div>
        <?php
        $sql="Select pun_username,pun_email
                from punter
                where pun_access='allow' and pun_preferences IS NULL ";
        $result=$dbh->doCachedQuery($sql,0);
        ?>
        <table class='table table-hover table-striped table-bordered' >
            <thead>
            <tr  class='alert alert-info'><th><?=$lang->getLang("User")?></th>
            <th><?=$lang->getLang("Email")?></th>
            </thead>
            <tbody>
            <?
            while($result->hasNext())
            {
                $row=$result->next();
                ?>
                <tr>
                    <td>
                        <?=$row['pun_username']?>
                    </td>
                    <td>
                        <?=$row['pun_email']?>
                    </td>
                </tr>
            <?
            }
            ?>
    </div>
</div>