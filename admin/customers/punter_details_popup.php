<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML>
<HEAD>
    <TITLE>Customer Details for funds transaction #<?=$ftr_id?></TITLE>
    <META NAME="Generator" CONTENT="EditPlus">
    <META NAME="Author" CONTENT="">
    <META NAME="Keywords" CONTENT="">
    <META NAME="Description" CONTENT="">

    <? include "admin/jscript_css.inc"; ?>
</HEAD>

<body background="images/bg.gif" leftmargin="0" topmargin="0" rightmargin="0" bottommargin="0" marginwidth="0" class=body>
<?
$conn = casinoConnect();
$punter_row = getPunter((int)$pun_id);

$customer_number = $punter_row['customer_number'];
$username       = $punter_row['username'];
$first_name     = $punter_row['first_name'];
$middle_name    = $punter_row['middle_name'];
$last_name      = $punter_row['last_name'];
$dob            = $punter_row['dob'];
$member_type    = $punter_row['member_type'];
$access         = $punter_row['access'];
$investigate         = $punter_row['investigate'];
$address_line1  = $punter_row['address_line1'];
$address_line2  = $punter_row['address_line2'];
$city_suburb    = $punter_row['city_suburb'];
$state_province = $punter_row['state_province'];
$postcode_zip   = $punter_row['postcode_zip'];
$pun_country_id = $punter_row['cou_id'];
$phone_business = $punter_row['phone_business'];
$phone_home     = $punter_row['phone_home'];
$phone_mobile   = $punter_row['phone_mobile'];
$fax            = $punter_row['phone_fax'];
$total_bets     = $punter_row['total_bets'];
$total_wins     = $punter_row['total_wins'];
$financial_reg_date    = $punter_row['financial_reg_date'];
$nonfinancial_reg_date = $punter_row['nonfinancial_reg_date'];


$full_name = concatNames($first_name, $middle_name, $last_name);
$address = getAddressString($punter_row);

?>
<br><br>
<div align=center class=level2heading>
    Details for customer #<?=$customer_number;?>: <?=$full_name;?>
</div>
<br>
<table cellpadding=4 cellspacing=1 border=0 align=center width=350>
    <tr>
        <td width=150 class=label>Customer Number</td>
        <td class=content><?=$customer_number;?></td>
    </tr>
    <tr>
        <td class=label>Name</td>
        <td class=content><?=$full_name;?></td>
    </tr>
    <tr>
        <td class=label>Date of Birth (d/m/y)</td>
        <td class=content><?=shortDate($dob);?></td>
    </tr>
    <tr>
        <td class=label>Member Type</td>
        <td class=content><?=ucwords(strtolower($member_type));?></td>
    </tr>
    <? if ( $nonfinancial_reg_date ) { ?>
        <tr>
            <td class=label>Nonfinancial Registration Date</td>
            <td class=content><?=shortDate($nonfinancial_reg_date);?></td>
        </tr>
    <? }  ?>
    <? if ( $financial_reg_date ) { ?>
        <tr>
            <td class=label>Financial Registration Date</td>
            <td class=content><?=shortDate($financial_reg_date);
                if ( $nonfinancial_reg_date )
                    echo " (Upgrade Date)";
                ?></td>
        </tr>
    <? }  ?>
    <tr valign=top>
        <td class=label><b>Access</b></td>
        <td class=content>
            <? if ('deny' == $access )
                echo "<font color=red><b>Account Locked</b></font>";
            else
                echo ucwords($access);?>
            <?
            if ( $investigate )
            {
                ?>    <br><font color=red><b>Under Investigation</b></font>
            <? }
            ?>
        </td>
    </tr>
    <tr>
        <td class=label>Address</td>
        <td class=content><?=$address;?></td>
    </tr>
    <tr>
        <td class=label>City or Suburb</td>
        <td class=content><?=$city_suburb;?></td>
    </tr>
    <tr>
        <td class=label>Postcode or Zipcode</td>
        <td class=content><?=$postcode_zip;?></td>
    </tr>
    <tr>
        <td class=label>Country</td>
        <td class=content><?=getCountryName($pun_country_id);?></td>
    </tr>
    <? if ( $phone_home ) { ?>
        <tr>
            <td class=label>Home Phone</td>
            <td class=content><?=$phone_home;?></td>
        </tr>
    <? } ?>
    <? if ( $phone_business ) { ?>
        <tr>
            <td class=label>Business Phone</td>
            <td class=content><?=$phone_business;?></td>
        </tr>
    <? } ?>
    <? if ( $phone_mobile ) { ?>
        <tr>
            <td class=label>Mobile Phone</td>
            <td class=content><?=$phone_mobile;?></td>
        </tr>
    <? } ?>
    <? if ( $fax ) { ?>
        <tr>
            <td class=label>Fax</td>
            <td class=content><?=$fax;?></td>
        </tr>
    <? } ?>
    <?
    if ( 'FINANCIAL' == $member_type )
    { ?>
        <tr>
            <td class=label>Total Bets</td>
            <td align=right class=content><font color=blue><?=printInDollars($total_bets);?></font></td>
        </tr>
        <tr>
            <td class=label>Total Wins</td>
            <td align=right class=content><?printInDollars($total_wins*-1);?></td>
        </tr>
        <tr>
            <td class=label>Total Revenue generated</td>
            <td align=right class=content><b><?printInDollars($total_bets-$total_wins);?></td>
        </tr>
    <?
    }
    else
    {
        ?><tr>
        <td class=label>Total Bets</td>
        <td  align=right class=content><?printInDollars($total_bets)?></td>
        </tr>
        <tr>
            <td class=label>Total Wins</td>
            <td align=right class=content><?printInDollars(-$total_wins);?></td>
        </tr>
    <?
    }
    ?>
</table>
<br><br>
<div align=center>
    <a href="javascript:window.close()"><img border=0 src="<?=image_dir?>/button_close.gif"></a>
</div>
</BODY>
</HTML>
