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
       Reports<br/>
       <ul>
<?php
      if ($game_reports) {
?>
         <li><a href="<?=pdaRefPage('games/stats'); ?>">Game Activity Search</a></li>
         <li><a href="<?=pdaRefPage('games/game_num_search'); ?>">Find a Result</a></li>
<?php
      }
      if ($payment_reports) {
?>
         <li><a href="<?=pdaRefPage('financial_reports/financial_transactions'); ?>">Date Range Search</a></li>
         <li><a href="<?=pdaRefPage('financial_reports/financial_transactions'); ?>&search_type=quick">Transaction Number Search</a></li>
<?php
      }
?>
         <li><a href="<?=secure_host;?>/admin/partials/">Partial accounts reset</a></li> 
<?php
      if ($entity_report) {
?>
         <li><a href="<?=secure_host?>/admin/reports/entities_new.php">Entities Credit Report</a></li> 
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
       Customers<br/>
       <ul>
<?php
         if ($view_customers) {
?>
       	 <li><a href="<?=pdaRefPage('customers/customers'); ?>">View Customers</a></li>
<?php
         }
         if ($view_groups) {
?>
         <li><a href="<?=pdaRefPage('customers/cust_group_admin'); ?>">View Groups</a></li>
<?php
         }
         if ($register_customers) {
?>
         <li><a href="<?=pdaRefPage('customers/add'); ?>">Register a Customer</a></li>
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
       Transactions<br/>
       <ul>
<?php
      if ($cashdesk_deposit) {
?>
         <li><a href="<?=pdaRefPage('transactions/cashdesk_deposit'); ?>">Deposit</a></li>
<?php
      }
      if ($cashdesk_withdrawal) {
?>
         <li><a href="<?=pdaRefPage('transactions/cashdesk_withdrawal'); ?>">Withdrawal</a></li>
<?php
      }
      if ($correttive_transaction) {
?>
         <li><a href="<?=pdaRefPage('transactions/correction'); ?>">Corrective Transaction</a></li>
<?php
      }
      if ($cashdesk_deposit) {
?>
         <li><a href="<?=secure_host?>/admin/customers/add_transient.php">New Customer</a></li>
<?php
      }
      if ($entity_credit_transfer) {
?>
         <li><a href="<?=secure_host?>/admin/transactions/entity_credit_transfer.inc">Credit Transfer</a></li>
<?php
      }
      if ($can_create_recharge || $can_apply_recharge) {
?>
         <li><a href="<?=secure_host?>/admin/selfrecharge/index.html">SELFRECHARGE</a></li>
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
       Configuration<br/>
       <ul>
<?php
       if ($admin_users) {
?>
         <li><a href="<?=secure_host.'/admin/admin_users/index.php'; ?>">Administrator Users</a></li>
<?php
       }
       if ($admin_user_types) {
?>
         <li><a href="<?=secure_host.'/admin/admin_users/user_types.php'; ?>">Administrator User Types</a></li>
<?php
       }
       if ($manage_jurisdictions) {
?>
         <li><a href="<?=secure_host.'/admin/jurisdictions/'; ?>">Jurisdictions</a></li>
<?php
       }
       if ($manage_tasks) {
?>
         <li><a href="<?=secure_host.'/admin/access_control/tasks.php'; ?>">Task Definitions</a></li>
<?php
       }
       if ($game_configuration) {
?>
         <li><a href="<?=pdaRefPage('games/config') ?>">Game Configuration</a></li>
<?php
       }
?>
       </ul>
<?php
     }
}
?>
