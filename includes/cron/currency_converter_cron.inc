<?PHP
/*
1. Check if inside the table 'currencies_conversion' are registered the conversion of the currencies for the current day

2. If the information are already inserted, will exit; otherwise:

    Will extract the complete list of currencies managed inside the table 'currency'
    Will take the currency with id 1 and will ask to the remote page: the rate change
    When all the information are extracted, will save the rate change inside the table 'currencies_conversion'

3. Manage in GET, the param 'date': the page must be able to manage the extraction for a specific date;
   if the param is null or is not send take automatically the current date
 */



$DEBUG = true;

// dependancies
ini_set('include_path', realpath(dirname(__FILE__).'/../../includes/classes').PATH_SEPARATOR.ini_get('include_path'));

require_once("DatabaseHandler.class.inc");
require_once(__DIR__ . "/../../config/database.conf.inc");

    $dbh = new DatabaseHandler(DBCONF);
    $dbh->connect();
    $date = new DateTime();
    $formattedDate = $date->format('Y-m-d');
    $formattedDateWithTimestamp = $date->format('Y-m-d H:i:s');

    $argInput = $argv[1];
    if (isset($argv[1])) {

        if (preg_match("/^[0-9]{4}-(0[1-9]|1[0-2])-(0[1-9]|[1-2][0-9]|3[0-1])$/", $argInput)) {
            //print(" This is a correct date format\n");
            $formattedDate = $argInput;
        }
        else  {
            print("Invalid date format. a valid date format is YYYY-MM-DD\n");
        }
    }

    // declaration and initialization of an array to store currencies
    $currenciesArray = [];
    $currencyAndCurrencyRatePair = [];

    // declaration and initialization of variables that store the currencies which are exchanged
    $currencyToConvertFrom = "EUR";
    $currencyToConvertTo = "";



    //get the currencies listed in the db
    $currencyRecords = $dbh->doCachedQuery("SELECT * FROM currency WHERE cur_static_change IS NULL",0);


    //Get the static_changed Currencies
    $staticCurrencies = $dbh->doCachedQuery("SELECT * FROM currency WHERE cur_static_change IS NOT NULL",0);



    while($staticCurrencies->hasNext()) {
        $row = $staticCurrencies->next();
        $cur_code = $row['cur_code'];
        $cur_static_value = $row['cur_static_change'];
        $currencyAndCurrencyRatePair[$cur_code] = $cur_static_value;
    }


    while($currencyRecords->hasNext()){
        $row = $currencyRecords->next();
        //remove euro from currency list
        if($row['cur_id'] == 1) {
            $currencyToConvertFrom = $row['cur_code'];
        } else {
            array_push($currenciesArray, $row['cur_code']);
        }
    }


//    var_dump($currenciesArray);

    // check the db table currencies_conversion for entries on the specific date
    $currenciesObj = $dbh->doCachedQuery("SELECT * FROM currencies_conversion WHERE CCN_DATE = '$formattedDate'", 0);
    $entriesOfCurrencies = $currenciesObj->getNumRows();


    // check to see if any records already exist
    if ($entriesOfCurrencies <= 0 ) {
        $removedDuplicates = array_unique($currenciesArray);

        foreach ($removedDuplicates as $key => $value) {
            $currencyToConvertTo = $value;
            $url = "http://currencies.apps.grandtrunk.net/getrate/".$formattedDate . "/". $currencyToConvertFrom. "/" . $currencyToConvertTo;
            $payload = file_get_contents($url);
            $currencyAndCurrencyRatePair[$value] = $payload;
        }

        var_dump($currencyAndCurrencyRatePair);

        /*
            Send the data to the database
        */
        $sql = "INSERT INTO currencies_conversion (CCN_DATE, CCN_CUR_MASTER, CCN_CUR_SECONDARY, CCN_CONVERS_VALUE, CCN_LAST_UPDATE) VALUES ";
//        var_dump($currencyAndCurrencyRatePair);
        foreach($currencyAndCurrencyRatePair as $key => $value) {
            if($value != false && $value != "False" && $key != $currencyToConvertFrom) {

               $sql.=" ('$formattedDate', '$currencyToConvertFrom', '$key', '$value', '$formattedDateWithTimestamp'),";
            }
        }
        $sql=substr($sql,0,-1);
        $sql .= ", ('$formattedDate', 'EUR', 'BVE', '1', '$formattedDateWithTimestamp')";
        var_dump($sql);

        $result = $dbh->exec($sql);

        echo ($result . " of new entries were converted and inserted in the database at " . $formattedDateWithTimestamp.  " for " . $formattedDate."\n");



    } else {
        print($entriesOfCurrencies.  " records already exist\n");
    }


?>
