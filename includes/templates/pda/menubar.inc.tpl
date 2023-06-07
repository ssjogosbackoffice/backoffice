<?php
if (isLoggedIn()) {
    $game_reports = check_access('view_game_activity');
    $payment_reports = check_access('view_payment_transactions');
    $site_usage = check_access('view_site_usage');
    $entity_report = check_access('entity_report');
    $realtime_monitor = check_access('realtime_monitor');
    $wmc_transaction_query = check_access('wmc_transaction_query');

    if ($game_reports || $payment_reports || $site_usage || $entity_report || $realtime_monitor || $wmc_transaction_query) {
?>
       <content id="4348"/><br/>
       <ul>
<?php
      if ($game_reports) {
?>
         <li><a href="<?=pdaRefPage('games/stats'); ?>"><content id="5054" /></a></li>
         <li><a href="<?=pdaRefPage('games/game_num_search'); ?>"><content id="4031" /></a></li>
<?php
      }
      if ($payment_reports) {
?>
         <li><a href="<?=pdaRefPage('financial_reports/financial_transactions'); ?>"><content id="4044" /></a></li>
         <li><a href="<?=pdaRefPage('financial_reports/financial_transactions'); ?>&search_type=quick"><content id="4043" /></a></li>
<?php
      }
?>
         <li><a href="<?=secure_host;?>/admin/partials/"><content id="946" /></a></li> 
<?php
      if ($entity_report) {
?>
         <li><a href="<?=secure_host?>/admin/reports/entities_new.php"><content id="1069" /></a></li> 
<?php
      }
?>
       </ul>
<?php
    }

    $view_customers = check_access('view_customers');
    $view_groups = check_access('manage_groups');
    $register_customers = check_access('preregister_customer');
    if ($view_customers || $view_groups || $register_customers) {
?>
       <content id="4034"/><br/>
       <ul>
<?php
         if ($view_customers) {
?>
       	 <li><a href="<?=pdaRefPage('customers/customers'); ?>"><content id="4035" /></a></li>
<?php
         }
         if ($view_groups) {
?>
         <li><a href="<?=pdaRefPage('customers/cust_group_admin'); ?>"><content id="4036" /></a></li>
<?php
         }
         if ($register_customers) {
?>
         <li><a href="<?=pdaRefPage('customers/add'); ?>"><content id="4037" /></a></li>
<?php
         }
?>
       </ul>
<?php
    }
    $cashdesk_deposit = check_access('cashdesk_deposit');
    $cashdesk_withdrawal = check_access('cashdesk_withdrawal');
    $corrective_transaction = check_access('corrective_transaction');
    $entity_credit_transfer = check_access('entity_credit_transfer');
    $can_create_recharge = check_access("selfrecharge_create");
    $can_apply_recharge = check_access("selfrecharge_apply");

    if ($cashdesk_deposit || $caskdesk_withdrawal || $corrective_transaction ||
        $entity_credit_transfer || $can_create_recharge || $can_apply_recharge) {
?>
       <content id="4038" /><br/>
       <ul>
<?php
      if ($cashdesk_deposit) {
?>
         <li><a href="<?=pdaRefPage('transactions/cashdesk_deposit'); ?>"><content id="4039" /></a></li>
<?php
      }
      if ($cashdesk_withdrawal) {
?>
         <li><a href="<?=pdaRefPage('transactions/cashdesk_withdrawal'); ?>"><content id="4040" /></a></li>
<?php
      }
      if ($correttive_transaction) {
?>
         <li><a href="<?=pdaRefPage('transactions/correction'); ?>"><content id="4041" /></a></li>
<?php
      }
      if ($cashdesk_deposit) {
?>
         <li><a href="<?=secure_host?>/admin/customers/add_transient.php"><content id="220" /></a></li>
<?php
      }
      if ($entity_credit_transfer) {
?>
         <li><a href="<?=secure_host?>/admin/transactions/entity_credit_transfer.php"><content id="912" /></a></li>
<?php
      }
      if ($can_create_recharge || $can_apply_recharge) {
?>
         <li><a href="<?=secure_host?>/admin/selfrecharge/index.html"><content id="1240" /></a></li>
<?php
      }
?>
       </ul>
<?php
    }
    $admin_users = check_access('manage_admin_users');
    $admin_user_types = check_access('manage_admin_user_types');
    $manage_jurisdictions = check_access('manage_jurisdictions');
    $manage_tasks = check_access('manage_tasks');
    $game_configuration = check_access('game_configuration');

    if ($admin_users || $admin_user_types || $manage_jurisdictions || $manage_tasks || $game_configuration || $manage_news || $smart_cards_view) {
?>
       <content id="4032" /><br/>
       <ul>
<?php
       if ($admin_users) {
?>
         <li><a href="<?=secure_host.'/admin/admin_users/index.php'; ?>"><content id="4047" /></a></li>
<?php
       }
       if ($admin_user_types) {
?>
         <li><a href="<?=secure_host.'/admin/admin_users/user_types.php'; ?>"><content id="680" /></a></li>
<?php
       }
       if ($manage_jurisdictions) {
?>
         <li><a href="<?=secure_host.'/admin/jurisdictions/'; ?>"><content id="678" /></a></li>
<?php
       }
       if ($manage_tasks) {
?>
         <li><a href="<?=secure_host.'/admin/access_control/tasks.php'; ?>"><content id="696" /></a></li>
<?php
       }
       if ($game_configuration) {
?>
         <li><a href="<?=pdaRefPage('games/config') ?>"><content id="5027" /></a></li>
<?php
       }
?>
       </ul>
<?php
     }
}
?>
