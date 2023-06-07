<?php

/**
 * Created by JetBrains PhpStorm.
 * User: marian
 * Date: 12/9/13
 * Time: 1:40 PM
 */


if(!empty($_SERVER['HTTP_X_REQUESTED_WITH']) && strtolower($_SERVER['HTTP_X_REQUESTED_WITH']) == 'xmlhttprequest' && $_SESSION['admin_username']) {
    /* special ajax here */
    $action = $_REQUEST['action'];
    switch($action)
    {
        case "listAffiliates":
            $start = $_POST["iDisplayStart"];
            $length = $_POST["iDisplayLength"];
            $result = getAffiliates($start, $length);
            echo $result;
            break;

        case 'getAffiliateTree':
             getAffiliateTree($_POST['affiliate']);
            break;
        case 'deactivate':
            disableAffiliate($_POST['affiliate']);
            break;

        case 'activate':
            activateAffiliate($_POST['affiliate']);
            break;
    }
}

//AND pun_activation_method='admin'
function getAffiliates($start,$length){
    global $dbh;
    $sql="Select pun_registration_status,pun_betting_club,pun_username,pun_first_name,pun_last_name,pun_reg_date,pun_last_name,club.jur_name,pun_member_type from punter
          Join jurisdiction club on pun_betting_club=club.jur_id  ";
    $sql2="Select count(*) from punter
    Join jurisdiction club on pun_betting_club=club.jur_id ";

    $jur_id = $_SESSION["jurisdiction_id"];

    if($_SESSION["jurisdiction_class"] != "casino"){
        $sql .= "
  		       JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
             JOIN jurisdiction regional ON regional.jur_id = district.jur_parent_id
             JOIN jurisdiction nation   ON nation.jur_id   = regional.jur_parent_id";
        $sql2 .="
  		       JOIN jurisdiction district ON district.jur_id = club.jur_parent_id
             JOIN jurisdiction regional ON regional.jur_id = district.jur_parent_id
             JOIN jurisdiction nation   ON nation.jur_id   = regional.jur_parent_id";
    }
    switch($_SESSION["jurisdiction_class"]){
        case "club":
            $sql .= " WHERE club.jur_id = $jur_id ";
            $sql2 .= " WHERE club.jur_id = $jur_id ";
            break;
        case "district":
            $sql .= " WHERE district.jur_id = $jur_id ";
            $sql2 .= " WHERE district.jur_id = $jur_id ";
            break;
        case "region":
            $sql .= " WHERE regional.jur_id = $jur_id ";
            $sql2 .= " WHERE regional.jur_id = $jur_id ";
            break;
        case "nation":
            $sql .= " WHERE nation.jur_id = $jur_id ";
            $sql2 .= " WHERE nation.jur_id = $jur_id ";
            break;
        case "casino":
            $sql .= " WHERE 1=1 ";
            $sql2 .= " WHERE 1=1 ";
            break;
        default:
            $sql .= " WHERE 1=0 ";
            break;
    }



    if(isset($_POST['code']) && $_POST['code']!=''){
        $sql.=" AND pun_aff_id=".$dbh->escape($_POST['code']);
        $sql2.=" AND pun_aff_id=".$dbh->escape($_POST['code']);
        $totresult=$dbh->queryOne($sql2);
    }
    else{
       $sql.=" AND pun_member_type ='affiliate'  and pun_registration_status IS NOT NULL AND pun_activation_method='admin'
          AND pun_reg_date BETWEEN '".$dbh->escape($_POST['date_start'])." 00:00:00' AND '".$dbh->escape($_POST['date_end'])." 23:59:59'
          limit $start,$length ";

            $sql2 .=" AND pun_member_type ='affiliate' AND pun_activation_method='admin' and pun_registration_status IS NOT NULL   AND pun_reg_date AND pun_reg_date BETWEEN '".$dbh->escape($_POST['date_start'])."' AND '".$dbh->escape($_POST['date_end'])."'";
        $totresult=$dbh->queryOne($sql2);
          }

    $rs=$dbh->exec($sql);

    $affiliates=array();
    while ( $rs->hasNext () ) {
        $section=array();
        $row = $rs->next ();
        $section[0]=$row['pun_betting_club'];
        $section[1]=$row['pun_username'];
        $section[2]=$row['pun_first_name'];
        $section[3]=$row['pun_last_name'];
        $section[4]=$row['pun_reg_date'];
        $section[5]=$row['jur_name'];
        $section[6]=$row['pun_member_type'];
        $section[7]="<button class='btn btn-small btn-primary' id='aff_".$row['pun_betting_club']."' name='activate'>Activate</button> <button class='btn btn-small btn-danger' id='aff_".$row['pun_betting_club']."_deact' name='activate'>Dectivate</button> ";
        $section[8]=$row['pun_registration_status'];
        array_push($affiliates,$section);
    }
    return toJsonTable($totresult,$affiliates);
}

function getAffiliateTree($affiliate){
global $dbh;
$sql="SELECT
            n.jur_id,
            n.jur_name nation,
            r.jur_id,
            r.jur_name region,
            d.jur_id,
            d.jur_name district,
            c.jur_id,
            c.jur_name club
      FROM  jurisdiction n,
            jurisdiction r,
            jurisdiction d,
            jurisdiction c
      WHERE n.jur_id = r.jur_parent_id
      AND r.jur_id = d.jur_parent_id
      AND d.jur_id = c.jur_parent_id
      AND c.jur_id = ".$dbh->escape($affiliate);
    $return="<ul>";
    $rs=$dbh->exec($sql);
    while ( $rs->hasNext () ) {
        $row = $rs->next ();
      $return.="<li rel='nation' data-jstree='{ \"type\" : \"nation\" }' >
                <a>".$row['nation']."</a>
                        <ul>
                            <li rel='region' data-jstree='{ \"type\" : \"region\" }'><a>".$row['region']."</a>
                                <ul>
                                    <li rel='district' data-jstree='{ \"type\" : \"district\" }'><a>".$row['district']."</a>
                                        <ul>
                                            <li rel='affiliate' id='html_".$affiliate."' data-jstree='{ \"type\" : \"affiliate\" }'><a>".$row['club']."</a>
                                            </li>
                                        </ul>
                                    </li>
                                </ul>
                            </li>
                        </ul> ".
          "</li>";
    }
    $return.="</ul>";
    echo  $return;
    die();
}

function disableAffiliate($affiliate)
{
    global $dbh;
    $sql="Update punter set pun_registration_status = NULL where pun_username='".$dbh->escape($affiliate)."'";
     return $dbh->exec($sql);
}

function activateAffiliate($affiliate)
{
    global $dbh;
    $url=$dbh->queryOne("Select pun_registration_status from punter where pun_username='".$dbh->escape($affiliate)."'");
    $ch=curl_init();
    curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
    curl_setopt($ch,CURLOPT_URL,$url);
    $result=curl_exec($ch);
    curl_close($ch);
    return $result;
}
?>
