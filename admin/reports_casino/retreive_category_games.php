<?php
global $dbh;
$category=$_POST['category'];
$sql="Select gam_id, gam_name from game where gam_category='$category'";
$result=$dbh->exec($sql);
$games= array();
while ($result->hasNext()) {
    $row = $result->next();
    $games[$row['gam_id']] = $row['gam_name'];
}
$gamestochoose='';
foreach ($games as $k=>$v)
{
    $gamestochoose .="<label><input type='checkbox'  name='games[]' value= $k />$v </label><br>";
}
echo $gamestochoose;

?>