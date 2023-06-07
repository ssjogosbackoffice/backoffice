<?php
if ( ! class_exists('cache_file_reader') )
    include 'cache_file_classes_inc.php';

$sess_id   = session_id();// $_GET['sid'];  //session id


if ( true == $use_global_vars ) {
    $filename = $cache_fname;
}
else {
    $cf             = clean($_GET['cf']);      // cache filename minuse sid suffix
    $start_rec      = (int)$_GET['start_rec']; // first record to display
    $num_recs       = (int)$_GET['num_recs']; // number of records in cache file
    $recs_per_page  = (int)$_GET['recs_per_page']; // num of recs to display per screen
    $sess_id        = clean($_GET['sid']); // $_GET['sid'];  //session id

    // remember start_rec counts from 0
    $filename = $cf . '-' . $sess_id;
}

if ( $recs_per_page > 500 )
    $recs_per_page = 500;


$num_recs_remain = $num_recs - $start_rec;

// sequence of last record to display for this screen, counting from 0
$last_rec_scr = $start_rec + min($recs_per_page,$num_recs_remain) -1;

$cols = array('id'=>0, 'time'=>1, 'transaction type'=>2, "amount"=>3, 'transaction num'=>4,
    "customer id"=>5, "result id"=>6, "payment method"=>7,
    "ctr_status"=>8, "ctr_void"=>9, "balance"=>10);


function get_col ($rec, $col_name) {
    global $cols;

    $ind = $cols[$col_name];
    return $rec[$ind];
}

$cr = new cache_file_reader($filename);
$cr->open();

if ( $cr->is_open() ) {
    $cr->to_line($start_rec);

    $i = $start_rec;

    //** display table
    ?>
    <div>
        <?php
        if ( $last_rec_scr - $start_rec > 1 )
        {
            //adjust to count from 1                                
            ?>     <div style="font-size:8pt;color:#BF0E38">Displaying records <?=($start_rec+1)?> - <?=($last_rec_scr+1)?></div>
        <?php
        }
        else
        {
            ?>
            <div style="font-size:8pt;color:#BF0E38">Displaying record <?=($start_rec+1)?></div>
        <?php
        }
        ?>
        <br>
        <table cellpadding="4" cellspacing="1" border="0" width="600">
            <tr>
                <th class="label">Time</th>
                <th class="label">Transaction</th>
                <th class="label">Transaction Type</th>
                <th class="label">Payment Method</th>
                <th class="label">Amount</th>
                <th class="label"></th>
            </tr>
            <?php
            // loop through recors and display
            $line = $cr->read_line(); // read a record

            while (  !empty($line) && $i <= $last_rec_scr )  //loop until read last record to display
            {
                $rec = $cr->line_to_array($line);

                // highlight voided	results
                if( empty($rec[9]) )
                    $cell_fmt = '';
                else
                    $cell_fmt   = ' style="background-color: #FF9999;"';
                ?>
                <tr>
                    <td class="content"<?=$cell_fmt;?>><?=shortDate(get_col($rec, 'time'),true);?></td>
                    <td class="content"<?=$cell_fmt;?>><?=get_col($rec, 'transaction num')?></td>
                    <td class="content"<?=$cell_fmt;?>><?=get_col($rec, 'transaction type')?></td>
                    <td class="content"<?=$cell_fmt;?>><?=get_col($rec, 'payment method')?></td>
                    <td class="content" align="right"<?=$cell_fmt;?>><?=getInDollars(get_col($rec, 'amount'));?></td>

                    <td class="content" align="right"<?=$cell_fmt;?>><?=getInDollars($rec[count($rec)-1]);?></td>
                </tr>
                <?php
                $line = $cr->read_line(); // read a record
                $i++;
            }
            ?>
        </table>
    </div>
<?php  } ?>
