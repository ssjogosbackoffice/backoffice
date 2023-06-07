<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/dataTablesExtra/TableTools-2.2.0/css/dataTables.tableTools.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/jstree/themes/default/style.css"  />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/table.css" title="core_style" />
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<script src="/media/jquery.dataTables.js" type="text/javascript"></script>
<script src="/media/jstree/jstree.js" type="text/javascript"></script>
<script src="/media/jquery.alerts.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/highcharts.js" type="text/javascript"></script>
<script src="<?= secure_host ?>/media/modules/exporting.js" type="text/javascript"></script>
<script src="/media/dataTablesExtra/TableTools-2.2.0/js/dataTables.tableTools.js" type="text/javascript"></script>
<?php
/**
 **  N
 */
check_access('virtual', true);

function formatBytes($bytes, $precision = 2) {
    $units = array('B', 'KB', 'MB', 'GB', 'TB');
    $bytes = max($bytes, 0);
    $pow = floor(($bytes ? log($bytes) : 0) / log(1024));
    $pow = min($pow, count($units) - 1);

    // Uncomment one of the following alternatives
    $bytes /= pow(1024, $pow);
    //$bytes /= (1 << (10 * $pow));

    return round($bytes, $precision) . ' ' . $units[$pow];
}
?>
<form action="virtual_documents.inc" method="post" enctype="multipart/form-data">
    <input type="hidden" name="upl" value="1" />
        <h3 style="color: #000000"> <?=$lang->getLang("Upload file")?></h3>
            <input type="file" name="file" id="file" required="" style="height: 22px; color: black;"><br>
        <label class="col-md-3 control-label" for="singlebutton"> </label>
            <button class="btn btn-inverse" type="submit"  name="singlebutton"><?=$lang->getLang("Upload")?></button>
</form>
<?php
if(isset ($_POST['upl'])){
    $doc=$_FILES['file'];
    $docname=$doc['name'];
    $doctype=$doc['type'];
    $doctmp=$doc['tmp_name'];
    $docerr=$doc['error'];
    $docsize=$doc['size'];
    $path=VIRTUAL_DOCUMENTS;
    $allowedExts = array("pdf");
    $temp = explode(".", $docname);
    $extension = end($temp);

    if (($doctype == "application/pdf")
        && ($docsize > 200000)
        && in_array($extension, $allowedExts))
    {
        if ($docerr > 0)
        {
            echo "Error Document";
        }
        else
        {
            if (file_exists($path."/".$docname))
            {
                $errorex=$lang->getLang("The file you are trying to upload already exists");
                echo $errorex;
            }
            else
            {
                move_uploaded_file($doctmp,$path."/".$docname);
                echo "The document ".$docname." has been successfully uploaded";
            }
        }
    }
    else
    {
        $errorfile=$lang->getLang("Invalid file!Please choose another.");
        echo $errorfile;
    }
}
?>
<div style="width: 100%;margin: auto">
    <table class='table display table-condensed table-hover table-striped table-bordered' >
        <thead>
        <tr>
            <th style="white-space: nowrap"><?=$lang->getLang("Filename")?></th>
            <th><?=$lang->getLang("Size")?></th>
            <th class="no-sort"><?=$lang->getLang("Download")?></th>
        </tr>
        </thead>
        <tbody>
        <?
        function getDocList(){
            $path=VIRTUAL_DOCUMENTS;
            $files = array_diff(scandir($path), array('..', '.'));
            $list=array();
            foreach($files as $k=>$v){
                $list[$v]=stat($path."/".$v);
            }
            return $list;
        }
        $files=getDocList();
        foreach($files as $k=>$v){
            ?>
            <tr>
                <td><?=$k?></td>
                <td><?=formatBytes($v[7])?> </td>

                <td  style="width:100px ">
                    <a href="<?="/virtual/download_documents.php?file=".$k?>"><button class="btn btn-small">
                            <?=$lang->getLang("Download")?>
                        </button></a></td>
            </tr>
        <?
        }
        ?>
        </tbody>
    </table>
</div>
