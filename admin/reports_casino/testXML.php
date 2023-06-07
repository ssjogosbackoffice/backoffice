<?php
/**
 * Created by PhpStorm.
 * User: alexandra
 * Date: 05.04.2018
 * Time: 15:00
 */


$xmlstr = <<<XML
<?xml version='1.0' standalone='yes'?>
<Method Name="PlaceBet">
<Auth Login="" Password="" />
<Params>
<Token Type="string" Value="7298b43defd0f2651df0dd730814c935" />
      <TransactionID Type="int" Value="677152101214806" />
      <BetAmount Type="int" Value="487500" />
      <BetReferenceNum Type="string" Value="55593615" />
      <GameReference Type="string" Value="BIA_SPORTSBOOK" />
      <BetMode Type="string" Value="Live" />
      <Description Type="string" Value="SACRAMENTO KINGS vs. GOLDEN STATE WARRIORS: Total:M?s 205.5" />
      <ExternalUserID Type="string" Value="453453" />
      <FrontendType Type="int" Value="2" />
      <BetStatus Type="string" Value="R" />
      <SportIDs Type="string" Value="12" />
      <ClientIP Type="string" Value="170.51.97.35" />
      <Bet>
        <IsSystem Type="bool" Value="false" />
        <EventCount Type="int" Value="1" />
        <BankerCount Type="int" Value="0" />
        <Events Type="string" Value="SACRAMENTO KINGS vs. GOLDEN STATE WARRIORS: Total:M?s 205.5;" />
        <EventList>
          <Event Type="string" Value="SACRAMENTO KINGS vs. GOLDEN STATE WARRIORS">
            <EventID Type="int" Value="11148539" />
            <SportID Type="int" Value="12" />
            <ExtEventID Type="int" Value="12234836" />
            <EventDate Type="string" Value="2018-04-01 02:00" />
            <Market Type="string" Value="Total">
              <MarketID Type="string" Value="1002-1" />
              <ExtType Type="string" Value="5/0/205.5/O" />
              <Outcome Type="string" Value="over" />
              <Odds Type="double" Value="1.650" />
            </Market>
          </Event>
        </EventList>
        <BetStake Type="int" Value="487500">
          <CombLength Type="int" Value="1" />
          <Winnings Type="int" Value="804375" />
          <MultipleBonus Type="int" Value="0" />
          <Odds Type="double" Value="1.650" />
        </BetStake>
      </Bet>
      <SiteId Type="string" Value="677152" />
    </Params>
  </Method>
XML;


?>