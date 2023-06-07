<?php
require_once 'financial_casino/financial_functions.inc';
switch($_POST['action'])
{
    case '1':
    {
        $parent=$_POST['parent'];
        $child=$_POST['child'];
        calculatePercent($parent,$child,true);
        break;
    }

    case '2':
    {
        $class=$_POST['thisclass'];
        $id=$_POST['id'];
        $betting=$_POST['betting'];
        getStartDate($class,$id,$betting);
        break;
    }
    case '3':
    {
        $date_start=$_POST['date_start'];
        $date_end=$_POST['date_end'];
        $bet=$_POST['bet'];
        $win=$_POST['win'];
        $rake=$_POST['rake'];
        $cash=$_POST['cash'];
        $parent=$_POST['parent'];
        $child=$_POST['child'];
        $fin=$_POST['fin'];
        $perc=$_POST['perc'];
        $note=$_POST['note'];
        $ca=$_POST['ca'];
        $n=$_POST['n'];
        $re=$_POST['re'];
        $d=$_POST['d'];
        $c=$_POST['c'];
        doClosure($date_start,$date_end,$bet,$win,$rake,$cash,$parent,$child,$fin,$perc,$note,$ca,$n,$re,$d,$c);
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
    case 5:
    {
        $pid=$_POST['pid'];
        $pval=$_POST['pval'];
        updateNote($pid,$pval);
        break;
    }
    case 6:
    {
        $parent=$_POST['parent'];
        $child=$_POST['child'];
        calculatePercentBetting($parent,$child,true);
        break;
    }

    case 7:
    {
        $date_start=$_POST['date_start'];
        $date_end=$_POST['date_end'];
        $bet=$_POST['bet'];
        $win=$_POST['win'];
        $rake=$_POST['rake'];
        $cash=$_POST['cash'];
        $parent=$_POST['parent'];
        $child=$_POST['child'];
        $fin=$_POST['fin'];
        $perc=$_POST['perc'];
        $note=$_POST['note'];
        $ca=$_POST['ca'];
        $n=$_POST['n'];
        $re=$_POST['re'];
        $d=$_POST['d'];
        $c=$_POST['c'];
        doBettingClosure($date_start,$date_end,$bet,$win,$rake,$cash,$parent,$child,$fin,$perc,$note,$ca,$n,$re,$d,$c);
        break;
    }

}

function calculatePercent($parent,$child,$return=false){
    global $dbh;

    $sql ="SELECT COALESCE( jps1.jps_perc_casino_live, 0 ) AS jps_perc_casino_live,
          COALESCE( jps1.jps_perc_casino, 0 ) AS jps_perc_casino,
          COALESCE( jps1.jps_perc_poker_live, 0 ) AS jps_perc_poker_live,
          COALESCE( jps1.jps_perc_poker, 0 ) AS jps_perc_poker_2d,
          COALESCE( jps1.jps_perc_virtual, 0 ) AS jps_perc_virtual,
          COALESCE( jps1.jps_perc_royalpk, 0 ) AS jps_perc_royalpk,
          COALESCE( jps1.jps_perc_casinoext, 0 ) AS jps_perc_casinoext,
          COALESCE( jps2.jps_perc_casino_live, 0 ) AS jps_perc_casino_live2,
          COALESCE( jps2.jps_perc_casino, 0 ) AS jps_perc_casino2,
          COALESCE( jps2.jps_perc_poker_live, 0 ) AS jps_perc_poker_live2,
          COALESCE( jps2.jps_perc_poker, 0 ) AS jps_perc_poker_2d_2,
          COALESCE( jps2.jps_perc_virtual, 0 ) AS jps_perc_virtual2,
          COALESCE( jps2.jps_perc_royalpk, 0 ) AS jps_perc_royalpk2,
          COALESCE( jps2.jps_perc_casinoext, 0 ) AS jps_perc_casinoext2
    FROM jurisdiction_payment_settings jps1, jurisdiction_payment_settings jps2
    WHERE  jps1.jps_jur_id =$parent
    AND jps2.jps_jur_id =$child ";

    $sql ="SELECT COALESCE( jps1.jps_perc_casino_live, 0 ) AS jps_perc_casino_live,
          COALESCE( jps1.jps_perc_casino, 0 ) AS jps_perc_casino,
          COALESCE( jps1.jps_perc_poker_live, 0 ) AS jps_perc_poker_live,
          COALESCE( jps1.jps_perc_poker, 0 ) AS jps_perc_poker_2d,
          COALESCE( jps1.jps_perc_virtual, 0 ) AS jps_perc_virtual,
          COALESCE( jps2.jps_perc_casino_live, 0 ) AS jps_perc_casino_live2,
          COALESCE( jps2.jps_perc_casino, 0 ) AS jps_perc_casino2,
          COALESCE( jps2.jps_perc_poker_live, 0 ) AS jps_perc_poker_live2,
          COALESCE( jps2.jps_perc_poker, 0 ) AS jps_perc_poker_2d_2,
          COALESCE( jps2.jps_perc_virtual, 0 ) AS jps_perc_virtual2
    FROM jurisdiction_payment_settings jps1, jurisdiction_payment_settings jps2
    WHERE  jps1.jps_jur_id =$parent
    AND jps2.jps_jur_id =$child ";

    $result = $dbh->doCachedQuery($sql, 0);
    while($result->hasNext())
    {
        $row=$result->next();

        if($row['jps_perc_casino2']=='0'){
            $configure=' - The casino percent<br>';
        }
        $_SESSION['casino']=$row['jps_perc_casino'];
        $_SESSION['casino2']=$row['jps_perc_casino2'];

        $casinolive=$row['jps_perc_casino_live']-$row['jps_perc_casino_live2'];
        if($row['jps_perc_casino_live2']=='0'){
            $configure .='- The casino live percent<br>';
        }
        $_SESSION['casino_live']=$row['jps_perc_casino_live'];
        $_SESSION['casino_live2']=$row['jps_perc_casino_live2'];

        if($row['jps_perc_poker_live2']=='0'){
            $configure .='- The poker live percent<br>';
        }
        $_SESSION['poker']=$row['jps_perc_poker_live'];
        $_SESSION['poker2']=$row['jps_perc_poker_live2'];

        if($row['jps_perc_poker_2d_2']=='0'){
            $configure .='- The poker 2d percent<br>';
        }
        $_SESSION['poker_2d']=$row['jps_perc_poker_2d'];
        $_SESSION['poker_2d_2']=$row['jps_perc_poker_2d_2'];



        if($row['jps_perc_virtual2']=='0'){
            $configure .='- The virtual percent<br>';
        }
        $_SESSION['virtual']=$row['jps_perc_virtual'];
        $_SESSION['virtual2']=$row['jps_perc_virtual2'];

        if($row['royalpk2']=='0'){
            $configure .='- The RoyalPk percent<br>';
        }
        $_SESSION['royalpk']=$row['jps_perc_royalpk'];
        $_SESSION['royalpk2']=$row['jps_perc_royalpk2'];

        if($row['casinoext2']=='0'){
            $configure .='- The CasinoExt percent<br>';
        }
        $_SESSION['casinoext']=$row['jps_perc_casinoext'];
        $_SESSION['casinoext2']=$row['jps_perc_casinoext2'];
        $percent = '';

        if($row['jps_perc_casino2']){
            $percent .= "Casino: ".$row['jps_perc_casino2']." %";
        }
        if($row['jps_perc_casino_live2']){
            $percent .= " <br />";
            $percent .= "Casino Live: ".$row['jps_perc_casino_live2']." %";
        }
        if($row['jps_perc_casino_live2']){
            $percent .= " <br />";
            $percent .= "Virtual: ".$row['jps_perc_casino_live2']." %";
        }
        if($row['jps_perc_royalpk2']){
            $percent .= " <br />";
            $percent .= "RoyalPk: ".$row['jps_perc_royalpk2']." %";
        }
        if($row['jps_perc_casinoext2']){
            $percent .= " <br />";
            $percent .= "CasinoExt: ".$row['jps_perc_casinoext2']." %";
        }
        if($row['jps_perc_poker_2d_2']){
            $percent .= " <br />";
            $percent .= "Poker: ".$row['jps_perc_poker_2d_2']." %";
        }
        if($row['jps_perc_poker_live2']){
            $percent .= " <br />";
            $percent .= "Poker Live: ".$row['jps_perc_poker_live2']." %";
        }

        $percent .= "##";

        if($row['jps_perc_casino2']){
            $percent .= "#C: ".$row['jps_perc_casino2'];
        }
        if($row['jps_perc_casino_live2']){
            $percent .= "#CL: ".$row['jps_perc_casino_live2'];
        }
        if($row['jps_perc_virtual2']){
            $percent .= "#V: ".$row['jps_perc_virtual2'];
        }
        if($row['jps_perc_royalpk2']){
            $percent .= "#RPk: ".$row['jps_perc_royalpk2'];
        }
        if($row['jps_perc_casinoext2']){
            $percent .= "#RCEx: ".$row['jps_perc_casinoext2'];
        }
        if($row['jps_perc_poker_2d_2']){
            $percent .= "#P2D: ".$row['jps_perc_poker_2d_2'];
        }
        if($row['jps_perc_poker_live2']){
            $percent .= "#PL: ".$row['jps_perc_poker_live2'];
        }

//        $percent= "Casino: ".$row['jps_perc_casino2']." % <br />
//             Casino Live:".$row['jps_perc_casino_live2']." %<br />
//             Virtual: ".$row['jps_perc_virtual2']." %<br />
//             RoyalPk: ".$row['jps_perc_royalpk2']." %<br />
//             CasinoExt: ".$row['jps_perc_casinoext2']." %<br />
//             Poker: ".$row['jps_perc_poker_2d_2']." %<br />
//             Poker Live: ".$row['jps_perc_poker_live2']." %###
//             C: ".$row['jps_perc_casino2']."#CL:".$row['jps_perc_casino_live2']."#V: ".$row['jps_perc_virtual2']."#RPk: ".$row['jps_perc_royalpk2']."#RCEx: ".$row['jps_perc_casinoext2']."#P2D: ".$row['jps_perc_poker_2d_2']."#PL: ".$row['jps_perc_poker_live2'];
    }
    if($return===true){
        if($configure!=''){
            echo $configure;
        }
        else{
            echo $percent;
        }
    }
    else{
        return $result;
    }
}

function calculatePercentBetting($parent,$child,$return=false){
    global $dbh ,$lang;
    $sql ="SELECT COALESCE( jps1.jps_perc_betting, 0 ) AS jps_perc_betting,
          COALESCE( jps1.jps_perc_live_betting, 0 ) AS jps_perc_betting_live,
          COALESCE( jps2.jps_perc_betting, 0 ) AS jps_perc_betting2,
          COALESCE( jps2.jps_perc_live_betting, 0 ) AS jps_perc_betting_live2
          FROM jurisdiction_payment_settings jps1, jurisdiction_payment_settings jps2
          WHERE  jps1.jps_jur_id =$parent
          AND jps2.jps_jur_id =$child ";
    $result = $dbh->doCachedQuery($sql, 0);
    while($result->hasNext())
    {
        $row=$result->next();

        if($row['jps_perc_betting2']=='0'){
            $configure=' - '.$lang->getLang("Betting Percentage").'<br>';
        }
        $_SESSION['betting']=$row['jps_perc_betting'];
        $_SESSION['betting2']=$row['jps_perc_betting2'];

        if($row['jps_perc_betting_live2']=='0'){
            $configure=' - '.$lang->getLang("The betting live percent").'<br>';
        }
        $_SESSION['betting']=$row['jps_perc_betting_live'];
        $_SESSION['betting2']=$row['jps_perc_betting_live2'];

        $percent=getBettingPercentage($row['jps_perc_betting2'],'type')." ".getBettingPercentage($row['jps_perc_betting2'])."% #".getBettingPercentage($row['jps_perc_betting_live2'],'type')." ".getBettingPercentage($row['jps_perc_betting2'])."% ";
    }
    if($return===true){
        if($configure!=''){
            echo $configure;
        }
        else{
            echo $percent;
        }
    }
    else{
        return $result;
    }
}


function getStartDate($class,$id,$betting=null)
{
    global $dbh ,$lang ;
    $sql="SELECT max(pce_END_DAY) day from jurisdiction_partial_closure where pce_jur_$class=$id ";
    if($class=='nation'){
        $sql.=' AND pce_jur_region=0';
    }
    elseif($class=='region'){
        $sql.=" AND pce_jur_district=0";
    }
    elseif($class=='district'){
        $sql .=" AND pce_jur_club=0";
    }
    if(!$betting){
        $sql.=" AND pce_gam_category=0";
    }
    else{
        $sql.=" AND pce_gam_category=7";
    }
    $result = $dbh->exec($sql);
    while($result->hasNext()){
        $row=$result->next();
        if($row['day']==''){
            $sql="SELECT min(jds_day) day from jurisdiction_daily_summary where jds_jur_$class=$id";
            $result = $dbh->exec($sql);
            while($result->hasNext()){
                $row=$result->next();
                if($row['day']==''){
                    $return= "<span class='errorbold'>".$lang->getLang('No data available')."</span>";
                }
                else {
                    $return=substr($row['day'],0,10);
                    if($betting){
                        $day=substr($row['day'],0,10);
                        $dayStart=date('Y-m-d',strtotime("next Tuesday",strtotime($day)));
                        $week=date('Y-m-d',strtotime("$dayStart + 6 days"));
                        if($week > date('Y-m-d',time()-3600*24)){
                            $week=0;
                        }
                        $month1= date('Y-m-d',strtotime("Last Monday of".date('Y-m',strtotime("$dayStart+".(+1)." months"))));
                        if($month1 > date('Y-m-d',time()-3600*24)){
                            $month1=0;
                        }
                        $month2= date('Y-m-d',strtotime("Last Monday of".date('Y-m',strtotime("$dayStart+".(+2)." months"))));
                        if($month2 > date('Y-m-d',time()-3600*24)){
                            $month2=0;
                        }
                        $month4= date('Y-m-d',strtotime("Last Monday of".date('Y-m',strtotime("$dayStart+".(+4)." months"))));
                        if($month4 > date('Y-m-d',time()-3600*24)){
                            $month4=0;
                        }
                        $return=$dayStart.";".$week.";".$month1.";".$month2.";".$month4;
                    }
                }
            }
        }
        else
        {
            $return= date('Y-m-d', strtotime((substr($row['day'],0,10)). ' + 1 days'));
            if($betting){
                $day=substr($row['day'],0,10);
                $dayStart=date('Y-m-d',strtotime("next Tuesday",strtotime($day)));
                $week=date('Y-m-d',strtotime("$dayStart + 6 days"));
                if($week > date('Y-m-d',time()-3600*24)){
                    $week=0;
                }
                $month1= date('Y-m-d',strtotime("Last Monday of".date('Y-m',strtotime("$dayStart+".(+1)." months"))));
                if($month1 > date('Y-m-d',time()-3600*24)){
                    $month1=0;
                }
                $month2= date('Y-m-d',strtotime("Last Monday of".date('Y-m',strtotime("$dayStart+".(+2)." months"))));
                if($month2 > date('Y-m-d',time()-3600*24)){
                    $month2=0;
                }
                $month4= date('Y-m-d',strtotime("Last Monday of".date('Y-m',strtotime("$dayStart+".(+4)." months"))));
                if($month4 > date('Y-m-d',time()-3600*24)){
                    $month4=0;
                }
                $return=$dayStart.";".$week.";".$month1.";".$month2.";".$month4;
            }
        }
    }
    die($return);
}

function doClosure($date_start,$date_end,$bet,$win,$rake,$cash,$parent,$child,$fin,$perc,$note,$ca,$n,$re=NULL,$d=NULL,$c=NULL){
    global $dbh;
    $id=$_SESSION['admin_id'];
    $perc=trim(urldecode($perc));

    $note=$dbh->escape($note);
    $note .="^^^Ca:". ($_SESSION['casino2']).";".$_SESSION['casinonet'].
        "#CL:". ($_SESSION['casino_live2']).";".$_SESSION['casinolivenet'].
        "#P:". ($_SESSION['poker2']).";".$_SESSION['pokernet'].
        "#V:". ($_SESSION['virtual2']).";".$_SESSION['virtualnet'].
        "#RPK:". ($_SESSION['royalpk2']).";".$_SESSION['royalpknet'].
        "#CEx:". ($_SESSION['casinoext2']).";".$_SESSION['casinoext'].
        "#P2D:".($_SESSION['poker_2d_2']).";".$_SESSION['poker_2d_net'];
    $sql="INSERT INTO jurisdiction_partial_closure (pce_start_day,pce_end_day,pce_jur_casino,pce_jur_nation";
    if($re){
        $sql.=',pce_jur_region';
    }
    if($d){
        $sql .=',pce_jur_district';
    }
    if($c){
        $sql .=',pce_jur_club';
    }
    $sql.=",pce_bet,pce_win,pce_rake,pce_cash_in_hand,pce_profit,pce_profit_child,pce_fin,pce_perc,pce_note,pce_aus_id) VALUES ('$date_start','$date_end','$ca','$n'";
    if($re){
        $sql.=",'$re'";
    }
    if($d){
        $sql .=",'$d'";
    }
    if($c){
        $sql .=",'$c'";
    }
    $sql.=",'$bet','$win','$rake','$cash','$parent','$child','$fin','$perc','$note','$id')";
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


function doBettingClosure($date_start,$date_end,$bet,$win,$rake,$cash,$parent,$child,$fin,$perc,$note,$ca,$n,$re=NULL,$d=NULL,$c=NULL){
    global $dbh;
    $note=$dbh->escape($note);
    $id=$_SESSION['admin_id'];
    $note .="^^^#B:". ($_SESSION['betting_perc'])."#BL:".($_SESSION['betting_live_perc']);
    $sql="INSERT INTO jurisdiction_partial_closure (pce_start_day,pce_end_day,pce_jur_casino,pce_jur_nation";
    if($re){
        $sql.=',pce_jur_region';
    }
    if($d){
        $sql .=',pce_jur_district';
    }
    if($c){
        $sql .=',pce_jur_club';
    }
    $sql.=",pce_bet,pce_win,pce_rake,pce_cash_in_hand,pce_profit,pce_profit_child,pce_fin,pce_perc,pce_note,pce_aus_id,pce_gam_category) VALUES ('$date_start','$date_end','$ca','$n'";
    if($re){
        $sql.=",'$re'";
    }
    if($d){
        $sql .=",'$d'";
    }
    if($c){
        $sql .=",'$c'";
    }
    $sql.=",'$bet','$win','$rake','$cash','$parent','$child','$fin','$perc','$note','$id','7')";

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




function updateNote($pid,$val){
    global $dbh;
    $val=$dbh->escape($val);
    $sql="SELECT pce_note from jurisdiction_partial_closure where pce_id=$pid";
    $result=$dbh->queryOne($sql);
    list($usernote,$detailsnote)=explode('^^^',$result);
    $result=$val."^^^".$detailsnote;
    $update="UPDATE jurisdiction_partial_closure set pce_note='$result' where pce_id=$pid ";
    $dbh->exec($update);
}



function getSubJurisdiction($jurisdictionClass){
    $subjurisdiction='';

    if($jurisdictionClass=='casino')
    {
        $subjurisdiction='nation';
    }
    elseif($jurisdictionClass=='nation')
    {
        $subjurisdiction='region';
    }
    elseif($jurisdictionClass=='region')
    {
        $subjurisdiction='district';
    }
    elseif($jurisdictionClass=='district'){
        $subjurisdiction='club';
    }
    return $subjurisdiction;
}

function getjurisdictionChild ($id,$name,$default=null){
    global $dbh;
    $sql= "SELECT jur_id,jur_name from jurisdiction where jur_parent_id='$id'";
    $result=$dbh->exec($sql);
    $str  = "<select name='$name' id='$name'>";
    while($result->hasNext())
    {
        $row=$result->next();
        $str .=  '<option value="' . $row['jur_id'].'"';
        if(isset($default)&&$default==$row['jur_id']){
            $str .=" selected=selected ";
        }
        $str.= ">". $row['jur_name'] . '</option>';
    }
    $str .=  '</select>';
    return $str;
}



function calculatePartialClosure($parent,$child,$date_start,$date_end)
{

    global $dbh;
    $parentClass=$_SESSION['jurisdiction_class'];
    $sql=" select jds_jur_nation nation_id,cur_code_for_web ";

    if($parentClass=='nation')
    {
        $sql .='  ,coalesce(jds_jur_region, 0) region_id ';
    }

    if($parentClass=='region')
    {
        $sql .='  ,coalesce(jds_jur_region, 0) region_id, coalesce(jds_jur_district, 0) district_id ';
    }
    if($parentClass=='district')
    {

        $sql .='  ,coalesce(jds_jur_region, 0) region_id, coalesce(jds_jur_district, 0) district_id , coalesce(jds_jur_club, 0) club_id ';
    }


    $sql.=" ,
      coalesce(cur_level.jps_perc_casino_live, 0) as jps_perc_casino_live,
                coalesce(cur_level.jps_perc_casino, 0) as jps_perc_casino,
                coalesce(cur_level.jps_perc_poker_live, 0) as jps_perc_poker_live,
                coalesce(cur_level.jps_perc_poker, 0) as jps_perc_poker_2D,
                coalesce(cur_level.jps_perc_virtual, 0) as jps_perc_virtual,
                

              sum(if (jds_gam_category = 1, jds_rake, 0)) as casino_rake,
              sum(if (jds_gam_category = 1, jds_rake, 0)) as casino_fin,
              sum(if (jds_gam_category = 1, jds_bet, 0)) as casino_bet,
              sum(if (jds_gam_category = 1, jds_win, 0)) as casino_win,

              sum(if (jds_gam_category = 2, jds_rake, 0)) as casino_live_rake,
              sum(if (jds_gam_category = 2, jds_rake, 0)) as casino_live_fin,
              sum(if (jds_gam_category = 2, jds_bet, 0)) as casino_live_bet,
              sum(if (jds_gam_category = 2, jds_win, 0)) as casino_live_win,

              sum(if (jds_gam_category = 3, jds_rake, 0)) as virtual_rake,
              sum(if (jds_gam_category = 3, jds_fin, 0)) as virtual_fin,
              sum(if (jds_gam_category = 3, jds_bet, 0)) as virtual_bet,
              sum(if (jds_gam_category = 3, jds_win, 0)) as virtual_win,

              sum(if (jds_gam_category = 5, jds_rake, 0)) as poker_live_rake,
              sum(if (jds_gam_category = 5, jds_fin, 0)) as poker_live_fin,
              sum(if (jds_gam_category = 5, jds_bet, 0)) as poker_live_bet,
              sum(if (jds_gam_category = 5, jds_win, 0)) as poker_live_win,

              sum(if (jds_gam_category = 8, jds_rake, 0)) as poker_2d_rake,
              sum(if (jds_gam_category = 8, jds_fin, 0)) as poker_2d_fin,
              sum(if (jds_gam_category = 8, jds_bet, 0)) as poker_2d_bet,
              sum(if (jds_gam_category = 8, jds_win, 0)) as poker_2d_win,

              sum(if (jds_gam_category = 10, jds_rake, 0)) as royalpk_rake,
              sum(if (jds_gam_category = 10, jds_rake, 0)) as royalpk_fin,
              sum(if (jds_gam_category = 10, jds_bet, 0)) as royalpk_bet,
              sum(if (jds_gam_category = 10, jds_win, 0)) as royalpk_win,

              sum(if (jds_gam_category = 4, jds_rake, 0)) as casinoext_rake,
              sum(if (jds_gam_category = 4, jds_rake, 0)) as casinoext_fin,
              sum(if (jds_gam_category = 4, jds_bet, 0)) as casinoext_bet,
              sum(if (jds_gam_category = 4, jds_win, 0)) as casinoext_win

               FROM jurisdiction_daily_summary

             ";

    if($parentClass=='casino'){
        $sql .="
        LEFT JOIN jurisdiction_payment_settings cur_level on cur_level.jps_jur_id =jds_jur_nation
        LEFT JOIN jurisdiction on jur_id=jds_jur_nation
        LEFT JOIN currency on jur_currency=cur_id
        WHERE  jds_jur_nation= ".$dbh->escape($child)."
                    AND jds_day >= '$date_start'
                    AND jds_day <= '$date_end'
                    AND jds_gam_category !=7
                    AND jds_gam_category !=9
                    GROUP BY jds_jur_nation ";
    }

    if($parentClass=='nation'){
        $sql .="
        LEFT JOIN jurisdiction_payment_settings cur_level on cur_level.jps_jur_id =jds_jur_region
        LEFT JOIN jurisdiction on jur_id=jds_jur_region
        LEFT JOIN currency on jur_currency=cur_id
        WHERE  jds_jur_nation = ".$dbh->escape($parent)."
                  AND   jds_jur_region= ".$dbh->escape($child)."
                  AND jds_day >= '$date_start' AND jds_day <= '$date_end'
                  AND jds_gam_category !=7
                  AND jds_gam_category !=9
                  GROUP BY jds_jur_nation,jds_jur_region ";
    }
    if($parentClass=='region')
    {
        $sql .="LEFT JOIN jurisdiction_payment_settings cur_level on cur_level.jps_jur_id =jds_jur_district
                LEFT JOIN jurisdiction on jur_id=jds_jur_district
                LEFT JOIN currency on jur_currency=cur_id
                WHERE  jds_jur_region= ".$dbh->escape($parent)."
                 AND   jds_jur_district= ".$dbh->escape($child)."
                 AND jds_day >= '$date_start' AND jds_day <= '$date_end'
                 AND jds_gam_category !=7
                 AND jds_gam_category !=9
                 GROUP BY jds_jur_nation, jds_jur_region, jds_jur_district ";
    }
    if($parentClass=='district')
    {
        $sql .=" LEFT JOIN jurisdiction_payment_settings cur_level on cur_level.jps_jur_id =jds_jur_club
                 LEFT JOIN jurisdiction on jur_id=jds_jur_club
                 LEFT JOIN currency on jur_currency=cur_id
                    WHERE  jds_jur_district =".$dbh->escape($parent)."
                    AND jds_jur_club= ".$dbh->escape($child)."
                    AND jds_day >= '$date_start' AND jds_day <= '$date_end'
                    AND jds_gam_category !=7
                    AND jds_gam_category !=9
                    GROUP BY jds_jur_nation, jds_jur_region, jds_jur_district ";

    }
    $result = $dbh->doCachedQuery($sql, 0,true);
    if($result->getNumRows()>0){
        return $result;
    }
    else{
        return '0';
    }

}


function calculateBettingPartialClosure($parent,$child,$date_start,$date_end)
{
    global $dbh;
    $parentClass=$_SESSION['jurisdiction_class'];
    $sql=" select cur_code_for_web,
                jds_jur_nation nation_id ,
                coalesce(jds_jur_region, 0) region_id,
                coalesce(jds_jur_district, 0) district_id ,
                coalesce(jds_jur_club, 0) club_id  ,
                sum(if (jds_gam_category = 7, jds_rake, 0)) as betting_rake,
                sum(if (jds_gam_category = 7, jds_bet, 0)) as betting_bet,
                sum(if (jds_gam_category = 7, jds_win, 0)) as betting_win,
                sum(if (jds_gam_category = 9, jds_rake, 0)) as betting_rake_live,
                sum(if (jds_gam_category = 9, jds_bet, 0)) as betting_bet_live,
                sum(if (jds_gam_category = 9, jds_win, 0)) as betting_win_live,
                coalesce(bet_n.jps_perc_betting, 0) as jps_perc_betting_n,
                coalesce(bet_n.jps_perc_live_betting, 0) as jps_perc_betting_live_n,
                coalesce(bet_r.jps_perc_betting, 0) as jps_perc_betting_r,
                coalesce(bet_r.jps_perc_live_betting, 0) as jps_perc_betting_live_r,
                coalesce(bet_d.jps_perc_betting, 0) as jps_perc_betting_d,
                coalesce(bet_d.jps_perc_live_betting, 0) as jps_perc_betting_live_d,
                coalesce(bet_c.jps_perc_betting, 0) as jps_perc_betting_c,
                coalesce(bet_c.jps_perc_live_betting, 0) as jps_perc_betting_live_c
                FROM currency,jurisdiction_daily_summary
                LEFT JOIN jurisdiction c on c.jur_id = jds_jur_club
                LEFT JOIN jurisdiction d on d.jur_id = jds_jur_district
                LEFT JOIN jurisdiction r on r.jur_id = jds_jur_region
                LEFT JOIN jurisdiction n on n.jur_id = jds_jur_nation
                LEFT JOIN jurisdiction_payment_settings bet_n on bet_n.jps_jur_id = n.jur_id
                LEFT JOIN jurisdiction_payment_settings bet_r on bet_r.jps_jur_id = r.jur_id
                LEFT JOIN jurisdiction_payment_settings bet_d on bet_d.jps_jur_id = d.jur_id
                LEFT JOIN jurisdiction_payment_settings bet_c on bet_c.jps_jur_id = c.jur_id
              WHERE jds_day >= '$date_start' AND jds_day <= '$date_end'  ";
    if($parentClass == 'district'){
        $sql .= " and c.jur_id = $child
            AND c.jur_currency=cur_id
        ";

    }
    if($parentClass == 'region'){
        $sql .= " and d.jur_id = $child
                AND d.jur_currency=cur_id ";
    }
    if($parentClass == 'nation'){
        $sql .= " and r.jur_id = $child
        AND r.jur_currency=cur_id ";
    }
    if($parentClass == 'casino'){
        $sql .= " and n.jur_id = $child
        AND n.jur_currency=cur_id ";
    }

    $sql.=" GROUP BY n.jur_id, r.jur_id, d.jur_id, c.jur_id";
    $result = $dbh->doCachedQuery($sql, 0,true);

    if($result->getNumRows()>0){
        return $result;
    }
    else{
        return '0';
    }
}

?>