<?php
//var_dump($_SESSION);
$action = getPostGetVar('action');

if($action=""){
    ?>
    <table>
        <tr>
            <th><?=$lang->getLang("Search customers")?></th>
        </tr>
        <form name="search" method="POST"  action="<?=refFormPage('searchcustumer/searchcustumer')?>&action=search">
            <tr>
                <td ><?=$lang->getLang("Search for")?>: </td>
                <td >
                    <select name="field">
                        <option value="1" > <?=$lang->getLang("Username")?> </option>
                        <option value="2" > <?=$lang->getLang("User Id")?> </option>
                        <option value="3" > <?=$lang->getLang("Email")?> </option>
                        <option value="4" > <?=$lang->getLang("Ip")?> </option>
                    </select>
                </td>
            </tr>
            <tr>
                <td><?=$lang->getLang("With value")?>: </td>
                <td> <input name="value" type="text" value="" /></td>
            </tr>
            <tr>
                <td><?=$lang->getLang("Exact")?>: </td>
                <td> <input type="checkbox" name="exact"/></td>
            </tr>
            <tr>
                <td colspan="2">
                    <input type="hidden" name="action" value="search"/>
                    <input type="submit" value="<?=$lang->getLang("Search")?>">
                </td>
            </tr>
        </form>
    </table>

    <table>
        <tr>
            <th class="label" ><?=$lang->getLang("Player Name")?></th>
            <th class="label" ><?=$lang->getLang("User Id")?></th>
            <th class="label" ><?=$lang->getLang("Username")?></th>
            <th class="label" ><?=$lang->getLang("Access")?></th>
            <th class="label" ><?=$lang->getLang("Email")?></th>
            <th class="label" ><?=$lang->getLang("Balance")?></th>
        </tr>
        <tr>
            <td colspan="6"><?=$lang->getLang("Search")?></td>
        </tr>
    </table>
<?
}else{
    if($action="search"){
        ?>
        <table>
            <tr>
                <th><?=$lang->getLang("Search customers")?></th>
            </tr>
            <form name="search" method="POST" action="<?=refFormPage('searchcustumer/searchcustumer')?>&action=search">
                <tr>
                    <td ><?=$lang->getLang("Search for")?> : </td>
                    <td >
                        <select name="field">
                            <option value="1" <?=(($_POST ['field']==1)?'selected':'') ?> ><?=$lang->getLang("Username")?> </option>
                            <option value="2" <?=(($_POST ['field']==2)?'selected':'') ?> ><?=$lang->getLang("User Id")?> </option>
                            <option value="3" <?=(($_POST ['field']==3)?'selected':'') ?> ><?=$lang->getLang("Email")?> </option>
                            <option value="4" <?=(($_POST ['field']==4)?'selected':'') ?> ><?=$lang->getLang("Ip")?> </option>
                        </select>
                    </td>
                </tr>
                <tr>
                    <td><?=$lang->getLang("With value")?> : </td>
                    <td> <input name="value" type="text" value="<?=$_POST ['value']?>" /></td>
                </tr>
                <tr>
                    <td><?=$lang->getLang("Exact")?>: </td>
                    <td> <input type="checkbox" name="exact" /></td>
                </tr>
                <tr>
                    <td colspan="2">
                        <input type="submit" value="<?=$lang->getLang("Search")?>">
                    </td>
                </tr>
            </form>
        </table>
        <table>
            <tr>
                <th class="label" ><?=$lang->getLang("Player Name")?></th>
                <th class="label" ><?=$lang->getLang("User Id")?></th>
                <th class="label" ><?=$lang->getLang("Username")?></th>
                <th class="label" ><?=$lang->getLang("Access")?></th>
                <th class="label" ><?=$lang->getLang("Email")?></th>
                <th class="label" ><?=$lang->getLang("Balance")?></th>
            </tr>
            <?=searchUser();?>
        </table>
    <?
    }
}


function searchUser() {
    global $dbh;
    $str="";
    $fieldtype = ($_POST ['field']) ? $_POST ['field'] : "";
    $fieldvalue = ($_POST ['value']) ? $_POST ['value'] : "";
    $exact = ($_POST ['exact'] == "on") ? true : false;

    $sql = createSearchUserSql ( $fieldtype, $fieldvalue, $exact );

    $dbh->doQuery( $sql );
    $rs =$dbh->getResults();
    //http://10.10.0.2/backoffice/admin/?page=customers/customer_view&customer_id=84727&header_page=
    while ( $row = $rs->fetchRow() ) {
        $str.="<tr>" .
            "<td><a href='".refPage('customers/customer_view')."&customer_id={$row['userid']}'> {$row['playername']} </a></td>" .
            "<td>{$row['userid']}</td>" .
            "<td>{$row['username']}</td>" .
            "<td>{$row['pun_access']}</td>" .
            "<td>{$row['email']}</td>" .
            "<td>{$row['balance']}</td>" .
            "</tr>";
    }

    return $str;
}

function createSearchUserSql($fieldtype, $fieldvalue, $exact) {
    $sql = "";
    switch ( intval ( $fieldtype )) {
        case 1 ://username
            $sql = "SELECT  punter.pun_id as UserID,punter.pun_username as username ,pun_access,punter.pun_email as email ,".
                " CONCAT(CONCAT(punter.pun_first_name,' '),punter.pun_last_name) as playername,  ".
                " punter_credit.pcr_credits as balance,".
                " punter_credit.pcr_bonus_credits as bonus".
                " FROM punter ".
                " JOIN punter_credit ON punter_credit.pcr_pun_id  = punter.pun_id ";
            if (!empty($_SESSION['jurisdiction_class']) && !empty($_SESSION['jurisdiction_id'])){
                if ($_SESSION['jurisdiction_class'] == "club"){
                    $sql .= " JOIN jurisdiction c ON c.jur_id = pun_betting_club And c.jur_id =".$_SESSION['jurisdiction_id'];
                }else{
                    if ($_SESSION['jurisdiction_class'] == "district"){
                        $sql .= " JOIN jurisdiction c ON c.jur_id = pun_betting_club ".
                            "JOIN jurisdiction d ON d.jur_id = c.jur_parent_id AND d.jur_id =".$_SESSION['jurisdiction_id'];
                    }else{
                        if ($_SESSION['jurisdiction_class'] == "region"){
                            $sql .= " JOIN jurisdiction c ON c.jur_id = pun_betting_club ".
                                " JOIN jurisdiction d ON d.jur_id = c.jur_parent_id ".
                                " JOIN jurisdiction r ON r.jur_id = d.jur_parent_id AND r.jur_id =".$_SESSION['jurisdiction_id'];
                        }else{
                            if ($_SESSION['jurisdiction_class'] == "nation"){
                                $sql .= " JOIN jurisdiction c ON c.jur_id = pun_betting_club ".
                                    " JOIN jurisdiction d ON d.jur_id = c.jur_parent_id ".
                                    " JOIN jurisdiction r ON r.jur_id = d.jur_parent_id ".
                                    " JOIN jurisdiction n ON n.jur_id = r.jur_parent_id AND n.jur_id =".$_SESSION['jurisdiction_id'];
                            }
                        }
                    }
                }
            }
            $sql .= " WHERE".
                " lower(punter.pun_username) ". (($exact) ? "=lower('" . $fieldvalue . "')" : " LIKE lower('%" . $fieldvalue . "%')")." ";
            break;
        case 2 ://userid
            $sql = "SELECT distinct(jur_id), punter.pun_id as UserID,punter.pun_username as username ,pun_access,punter.pun_email as email ,".
                " CONCAT(CONCAT(punter.pun_first_name,' '),punter.pun_last_name) as playername,  ".
                " punter_credit.pcr_credits as balance,".
                " punter_credit.pcr_bonus_credits as bonus".
                " FROM punter".
                " JOIN punter_credit ON punter_credit.pcr_pun_id  = punter.pun_id ";
            if (!empty($_SESSION['jurisdiction_class']) && !empty($_SESSION['jurisdiction_id'])){
                if ($_SESSION['jurisdiction_class'] == "club"){
                    $sql .= " JOIN jurisdiction c ON c.jur_id = pun_betting_club And c.jur_id =".$_SESSION['jurisdiction_id'];
                }else{
                    if ($_SESSION['jurisdiction_class'] == "district"){
                        $sql .= " JOIN jurisdiction c ON c.jur_id = pun_betting_club ".
                            "JOIN jurisdiction d ON d.jur_id = c.jur_parent_id AND d.jur_id =".$_SESSION['jurisdiction_id'];
                    }else{
                        if ($_SESSION['jurisdiction_class'] == "region"){
                            $sql .= " JOIN jurisdiction c ON c.jur_id = pun_betting_club ".
                                " JOIN jurisdiction d ON d.jur_id = c.jur_parent_id ".
                                " JOIN jurisdiction r ON r.jur_id = d.jur_parent_id AND r.jur_id =".$_SESSION['jurisdiction_id'];
                        }else{
                            if ($_SESSION['jurisdiction_class'] == "nation"){
                                $sql .= " JOIN jurisdiction c ON c.jur_id = pun_betting_club ".
                                    " JOIN jurisdiction d ON d.jur_id = c.jur_parent_id ".
                                    " JOIN jurisdiction r ON r.jur_id = d.jur_parent_id ".
                                    " JOIN jurisdiction n ON n.jur_id = r.jur_parent_id AND n.jur_id =".$_SESSION['jurisdiction_id'];
                            }
                        }
                    }
                }
            }
            $sql .= " WHERE ".
                " punter.pun_id =". $fieldvalue." ";
            break;
        case 3 ://email
            $sql = "SELECT  punter.pun_id as UserID,punter.pun_username as username ,pun_access,punter.pun_email as email ,". //distinct(jur_id),
                " CONCAT(CONCAT(punter.pun_first_name,' '),punter.pun_last_name) as playername,  ".
                " punter_credit.pcr_credits as balance,".
                " punter_credit.pcr_bonus_credits as bonus".
                " FROM punter".
                " JOIN punter_credit ON punter_credit.pcr_pun_id  = punter.pun_id ";
            if (!empty($_SESSION['jurisdiction_class']) && !empty($_SESSION['jurisdiction_id'])){
                if ($_SESSION['jurisdiction_class'] == "club"){
                    $sql .= " JOIN jurisdiction c ON c.jur_id = pun_betting_club And c.jur_id =".$_SESSION['jurisdiction_id'];
                }else{
                    if ($_SESSION['jurisdiction_class'] == "district"){
                        $sql .= " JOIN jurisdiction c ON c.jur_id = pun_betting_club ".
                            "JOIN jurisdiction d ON d.jur_id = c.jur_parent_id AND d.jur_id =".$_SESSION['jurisdiction_id'];
                    }else{
                        if ($_SESSION['jurisdiction_class'] == "region"){
                            $sql .= " JOIN jurisdiction c ON c.jur_id = pun_betting_club ".
                                " JOIN jurisdiction d ON d.jur_id = c.jur_parent_id ".
                                " JOIN jurisdiction r ON r.jur_id = d.jur_parent_id AND r.jur_id =".$_SESSION['jurisdiction_id'];
                        }else{
                            if ($_SESSION['jurisdiction_class'] == "nation"){
                                $sql .= " JOIN jurisdiction c ON c.jur_id = pun_betting_club ".
                                    " JOIN jurisdiction d ON d.jur_id = c.jur_parent_id ".
                                    " JOIN jurisdiction r ON r.jur_id = d.jur_parent_id ".
                                    " JOIN jurisdiction n ON n.jur_id = r.jur_parent_id AND n.jur_id =".$_SESSION['jurisdiction_id'];
                            }
                        }
                    }
                }
            }
            $sql .= " WHERE".
                " lower(punter.pun_email) ". (($exact) ? "=lower('" . $fieldvalue . "')" : " LIKE lower('%" . $fieldvalue . "%')")." ";
            /*if (!empty($_SESSION['userinfo']['jur_id'])){
             $sql .= "  AND pun_betting_club = ".$_SESSION['userinfo']['jur_id'];
            }*/
            break;
        case 4 ://ip
            $sql = "SELECT distinct(punter.pun_id) as UserID,punter.pun_username as username ,pun_access,punter.pun_email as email ,".
                " CONCAT(CONCAT(punter.pun_first_name,' '),punter.pun_last_name) as playername,  ".
                " punter_credit.pcr_credits as balance,".
                " punter_credit.pcr_bonus_credits as bonus".
                " FROM punter".
                " JOIN punter_credit ON punter_credit.pcr_pun_id  = punter.pun_id ".
                " JOIN customer_log ON clg_punid  = punter.pun_id ";
            if (!empty($_SESSION['jurisdiction_class']) && !empty($_SESSION['jurisdiction_id'])){
                if ($_SESSION['jurisdiction_class'] == "club"){
                    $sql .= " JOIN jurisdiction c ON c.jur_id = pun_betting_club And c.jur_id =".$_SESSION['jurisdiction_id'];
                }else{
                    if ($_SESSION['jurisdiction_class'] == "district"){
                        $sql .= " JOIN jurisdiction c ON c.jur_id = pun_betting_club ".
                            "JOIN jurisdiction d ON d.jur_id = c.jur_parent_id AND d.jur_id =".$_SESSION['jurisdiction_id'];
                    }else{
                        if ($_SESSION['jurisdiction_class'] == "region"){
                            $sql .= " JOIN jurisdiction c ON c.jur_id = pun_betting_club ".
                                " JOIN jurisdiction d ON d.jur_id = c.jur_parent_id ".
                                " JOIN jurisdiction r ON r.jur_id = d.jur_parent_id AND r.jur_id =".$_SESSION['jurisdiction_id'];
                        }else{
                            if ($_SESSION['jurisdiction_class'] == "nation"){
                                $sql .= " JOIN jurisdiction c ON c.jur_id = pun_betting_club ".
                                    " JOIN jurisdiction d ON d.jur_id = c.jur_parent_id ".
                                    " JOIN jurisdiction r ON r.jur_id = d.jur_parent_id ".
                                    " JOIN jurisdiction n ON n.jur_id = r.jur_parent_id AND n.jur_id =".$_SESSION['jurisdiction_id'];
                            }
                        }
                    }
                }
            }
            $sql .= " WHERE".
                " clg_ipaddress  ". (($exact) ? "='" . $fieldvalue . "'" : " LIKE '%" . $fieldvalue . "%'")." ";
            break;
    }
    return $sql;
}



?>
