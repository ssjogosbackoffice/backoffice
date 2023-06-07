<?php
switch($_POST['action'])
{
    case '1':
    {
        $parent=$_POST['parent'];
        $child=$_POST['child'];
        calculatePercent($parent,$child);
        break;
    }

    case '2':
    {
        $class=$_POST['thisclass'];
        $id=$_POST['id'];
        getStartDate($class,$id);
        break;
    }
    case '3':
    {
        $date_start=$_POST['date_start'];
        $date_end=$_POST['date_end'];
        $bet=$_POST['bet'];
        $win=$_POST['win'];
        $rake=$_POST['rake'];
        $fin=$_POST['fin'];
        $perc=$_POST['perc'];
        $note=$_POST['note'];
        $id=$_POST['id'];
        $ca=$_POST['ca'];
        $n=$_POST['n'];
        $re=$_POST['re'];
        $d=$_POST['d'];
        $c=$_POST['c'];
        doClosure($date_start,$date_end,$bet,$win,$rake,$fin,$perc,$note,$id,$ca,$n,$re,$d,$c);
        break;
    }
    case 4:
    {
        $parent=$_POST['parent'];
        $parentclass=$_POST['parentclass'];
        $date_start=$_POST['date_start'];
        getClosure($parent,$parentclass,$date_start);
        break;
    }

}
function calculatePercent($parent,$child){
    global $dbh;
    $sql ="SELECT COALESCE( jps1.jps_perc_casino_live, 0 ) AS jps_perc_casino_live,
          COALESCE( jps1.jps_perc_casino, 0 ) AS jps_perc_casino,
          COALESCE( jps1.jps_perc_poker_live, 0 ) AS jps_perc_poker_live,
          COALESCE( jps1.jps_perc_virtual, 0 ) AS jps_perc_virtual,
          COALESCE( jps1.jps_perc_gaminator, 0 ) AS jps_perc_gaminator,
          COALESCE( jps2.jps_perc_casino_live, 0 ) AS jps_perc_casino_live2,
          COALESCE( jps2.jps_perc_casino, 0 ) AS jps_perc_casino2,
          COALESCE( jps2.jps_perc_poker_live, 0 ) AS jps_perc_poker_live2,
          COALESCE( jps2.jps_perc_virtual, 0 ) AS jps_perc_virtual2,
          COALESCE( jps2.jps_perc_gaminator, 0 ) AS jps_perc_gaminator2
FROM jurisdiction_payment_settings jps1, jurisdiction_payment_settings jps2
WHERE  jps1.jps_jur_id =$parent
AND jps2.jps_jur_id =$child
	  ";
    $result = $dbh->doCachedQuery($sql, 0);
    while($result->hasNext())
    {
        $row=$result->next();

        $casino=$row['jps_perc_casino']-$row['jps_perc_casino2'];
        if($row['jps_perc_casino2']=='0'){
            $configure=' - The casino percent<br>';
        }
        $_SESSION['casino']=$casino;

        $casinolive=$row['jps_perc_casino_live']-$row['jps_perc_casino_live2'];
        if($row['jps_perc_casino_live2']=='0'){
            $configure .='- The casino live percent<br>';
        }
        $_SESSION['casino_live']=$casinolive;

        $poker=$row['jps_perc_poker_live']-$row['jps_perc_poker_live2'];
        if($row['jps_perc_poker_live2']=='0'){
            $configure .='- The poker percent<br>';
        }
        $_SESSION['poker']=$poker;

        $gaminator=$row['jps_perc_gaminator']-$row['jps_perc_gaminator2'];
        if($row['jps_perc_gaminator2']=='0'){
            $configure .='- The gaminator percent<br>';
        }
        $_SESSION['gaminator']=$gaminator;

        $virtual=$row['jps_perc_virtual']-$row['jps_perc_virtual2'];
        if($row['jps_perc_virtual2']=='0'){
            $configure .='- The virtual percent<br>';
        }
        $_SESSION['virtual']=$virtual;

        $percent=($casino+$poker+$gaminator+$casinolive+$virtual)/5;
    }
    if($configure!=''){
        echo $configure;
    }
    else{
        echo $percent;
    }
}

function getStartDate($class,$id)
{
    global $dbh;
    $sql="SELECT max(TTC_END_DAY) day from totem_closure where ttc_jur_$class=$id";
    $result = $dbh->exec($sql);
    while($result->hasNext()){
        $row=$result->next();
        if($row['day']==''){
            $sql="SELECT min(jds_day) day from totem_closure where jds_jur_$class=$id";
            $result = $dbh->exec($sql);
            while($result->hasNext()){
                $row=$result->next();
                if($row['day']==''){
                    echo "<span class='errorbold'>No data available</span>";
                }
                else {

                    echo substr($row['day'],0,10);
                }
            }
        }
        else
        {
            echo date('Y-m-d', strtotime((substr($row['day'],0,10)). ' + 1 days'));
        }
    }
}
function doClosure($date_start,$date_end,$bet,$win,$rake,$fin,$perc,$note,$id,$ca,$n,$re=NULL,$d=NULL,$c=NULL){
    global $dbh;
    $note .="<br>Casino =>". $_SESSION['casino']."%<br>Casino Live => ". $_SESSION['casino_live']."%<br>Poker =>". $_SESSION['poker']."%<br>Gaminator=>". $_SESSION['gaminator']."%<br>Virtual =>". $_SESSION['virtual']."%";
    $sql="INSERT INTO totem_closure (ttc_start_day,ttc_end_day,ttc_jur_casino,ttc_jur_nation";
    if($re){
        $sql.=',ttc_jur_region';
    }
    if($d){
        $sql .=',ttc_jur_district';
    }
    if($c){
        $sql .=',ttc_jur_club';
    }
    $sql.=",ttc_bet,ttc_win,ttc_rake,ttc_fin,ttc_perc,ttc_note,ttc_aus_id) VALUES ('$date_start','$date_end','$ca','$n'";
    if($re){
        $sql.=",'$re'";
    }
    if($d){
        $sql .=",'$d'";
    }
    if($c){
        $sql .=",'$c'";
    }
    $sql.=",'$bet','$win','$rake','$fin','$perc','$note','$id')";
    $result = $dbh->exec($sql);

    if(is_int($result)&&$result==1)
    {
        echo '1';
    }
    else
    {
        echo '0';
    }
}

?>