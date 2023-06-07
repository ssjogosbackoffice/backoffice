<?php

$ini_array = parse_ini_file('config.ini',true);

$last=date ("Y-m-d  H:i:s ");

?>
    <link rel="stylesheet" type="text/css" href="media/output.css">
    <link rel="stylesheet" type="text/css" href="media/bootstrap/css/bootstrap.css">
    <link rel="javascript" type="text/javascript" href="media/bootstrap/js/bootstrap.js">
    <script>
        $(document).ready(function(){
            $("#hide").click(function(){
                $(".file").html("");
                $(".mains").hide();
		$(".overlay").hide();
            });
        });
        function openLink(address){
            $(".file").load('/services/dbase/getFile.php?file='+address);
            $('.mains').show();
            $(".overlay").show();
        }
    </script>
<div class="col-md-4">
    <table class="table table-bordered table-striped table table-hover">
        <tr> <th> Date  </th> <td> <?php echo $last; ?> </td> <tr>
        <tr><th> Status </th>  <th>Filename </th>  <th> IP Slave </th> <th> Slave Port </th> <th> IP Master </th> <th> Master Port </th><th>  Info </th></tr>
        <?php
        $path = $ini_array['default']['path'];
        $command = $ini_array['default']['command'];
        $settings = $ini_array['database']['settings'];
        $host2=$ini_array['database']['settings'];
        if($handle = opendir($path)){
        while(false !==($file = readdir($handle))){
            if ($file!="." && $file!=".." && $file!=".log" && $file!=".htaccess"){
                $lines = file($path.$file);
                $IO=explode(': ',$lines[11]);
                $SQL=explode(': ',$lines[12]);
                $host=explode(': ',$lines[2]);
                $port=explode(': ',$lines[4]);
                $io=$IO[1];
                $sql=$SQL[1];
                $link=$path.$file;
                $row=array($io,$sql,$file,$host[1],$port[1]);
                ?>
                <tr class="danger">
                    <td> <?php if( strtolower(trim($io))=='yes' && strtolower(trim($sql))=='yes' )
                        {
                            echo '<div class="green">  </div>';
                        }
                        else{
                            echo '<div class="red">  </div>';
                        }
                        ?>
                    </td>
                    <td><?php echo $file; ?> </td>
                    <td><?php
                        foreach($host2 as $k1 => $v2) {
                            $slave=explode("$",$v2);
                                if(($slave[4].".log")==$file){
                                     echo $slave[0];
                                }
                        }
                        ?>
                    </td>
                    <td>
                        <?php
                        foreach($host2 as $k1 => $v2) {
                            $slave=explode("$",$v2);
                            if(($slave[4].".log")==$file){
                                echo $slave[1];
                            }
                        }
                        ?>
                         </td>
                    <td><?php echo $host[1]; ?>  </td>
                    <td><?php echo $port[1]; ?>  </td>
                 <!--   <td>  <button class="btn btn-primary" onclick="openLink('<?php echo $link; ?>');">Open</button> </td> -->
			<td>  <button class="btn btn-primary" onclick="">Open</button> </td>
                </tr>
            <?php
            }
        }
        ?>
    </table>
</div> </br>
<div class="overlay"></div>
<div class="mains">
    <div class='file'></div>
    <button class="btn btn-danger btn-xs" id="hide">Close</button>
</div>
<?php
}
?>

