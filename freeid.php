#!/usr/local/bin/php
<?php

$db_conn = pg_connect("host=localhost user=postgres dbname=pokertranslate");
$busy_id = array();
if ($db_conn) {
  pg_set_client_encoding($db_conn, 'UNICODE');
  $max_id = 10000;
  $q = "SELECT id from localization_content group by id order by id";
  $res = pg_query($db_conn, $q);
  if ($res) {
    if (pg_num_rows($res) > 0) {
      while ($cnt_data = pg_fetch_assoc($res)) {
        array_push($busy_id, $cnt_data["id"]);
      }
      $mod = 0;
      for ($i = 0; $i < $max_id; $i++) {
        if (!in_array($i, $busy_id)) {
          print "$i\t";
          $mod++;
          if (($mod % 5) == 0)
            print "\n";
        }
      }
      print "\n";
    } else {
      die("Nessun risultato");
    }
  }
  pg_close($db_conn);
} else {
  die ("Impossibile aprire una connessione con il database");
}
?>




