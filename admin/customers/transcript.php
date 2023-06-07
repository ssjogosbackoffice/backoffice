<?php
//costanti lingua dei dettagli
define("DETHAND_ID",           "Hand #");
define("DETHAND_THEME",        "Room");
define("DETHAND_RAKE",         "Rake");
define("DETHAND_PLAYER",       "Player Details");
define("DETHAND_HAND",         "Bet Details");
define("DETHAND_WIN",          "Winning");
define("DETHAND_WINNER",       "Winner");
define("DETHAND_BET",          "Bet");
define("DETHAND_BETS",         "Bets");
define("DETHAND_CARDS",        "Cards");
define("DETHAND_DEACARDS",     "Community Cards");
define("DETHAND_USERNAME",     "User");
define("DETHAND_RANK",         "Hand");
define("DETHAND_SEAT",         "Seat");
define("DETHAND_START",        "Starting credit");
define("DETHAND_END",          "Ending credit");

define("DETHAND_ACT_BLIND",      "%s blind %s");
define("DETHAND_ACT_BBLIND",     "%s big blind %s");
define("DETHAND_ACT_ANTE",       "%s posts ante %s");
define("DETHAND_ACT_DEALCHECKS", "%s plays");
define("DETHAND_ACT_DEALCALLS",  "%s checks");
define("DETHAND_ACT_BETS",       "%s bets %s");
define("DETHAND_ACT_FOLDS",      "%s folds");
define("DETHAND_ACT_RAISES",     "%s raises to %s");
define("DETHAND_ACT_DEALCARDS",  "Cards being dealt");
define("DETHAND_ACT_FLOP",       "Flop is being dealt %s");
define("DETHAND_ACT_TURN",       "Turn is being dealt %s");
define("DETHAND_ACT_RIVER",      "River is being dealt %s");
define("DETHAND_ACT_ALLINS",     "%s goes allin");
define("DETHAND_ACT_SHOWCARDS",  "%s shows cards %s %s");
define("DETHAND_ACT_AWAY",       "%s changes status in 'away'");
define("DETHAND_ACT_AWAYBACK",   "%s is back from 'away'");
define("DETHAND_ACT_POT",        "The %s is of %s");
define("DETHAND_ACT_WINNERS",    "%s wins %s:");
define("DETHAND_ACT_WINNERS_DETAILS","&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;%s from %s");
define("DETHAND_ACT_MONEYBACK",  "%s gets %s back");
define("DETHAND_ACT_TIMEOUT",    "%s timed out");
define("DETHAND_ACT_FORCESTANDUP", "Game makes %s stand up");
define("DETHAND_ACT_SEAT",       "%s has %s on the table");
define("DETHAND_ACT_RAKE",       "%s pays %s rake");
define("ATTENTION",              "Warning");

//array per risultati poker
$hand_name    = array(
    "%s High",
    "A pair of %s",
    "Two pairs of %s and %s",
    "Three of a kind, %s",
    "Straight/%s",
    "Flush/%s",
    "Fullhouse %s",
    "Four of a kind, %s",
    "Royal flush/%s"
);

$rank_name    = array(
    "two",
    "three",
    "four",
    "five",
    "six",
    "seven",
    "eight",
    "nine",
    "ten",
    "jack",
    "queen",
    "king",
    "ace"
);

$rankPlu_name = array(
    "twos",
    "threes",
    "fours",
    "fives",
    "sixes",
    "sevens",
    "eights",
    "nines",
    "tens",
    "jacks",
    "queens",
    "kings",
    "aces"
);

$arrnoexist   = array(
    "There are no tables with these limits",
    "no existing tables of",
    "no existing table containing",
    "the player you're searching is not available in any table",
    "All the tables are empty",
    "All the tables are full"
);
?>
