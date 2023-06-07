<link rel="stylesheet" type="text/css" href="<?= secure_host ?>/media/bootstrap/css/bootstrap.css" title="core_style" />
<script src="<?= secure_host ?>/media/bootstrap/js/bootstrap.js" type="text/javascript"></script>
<div class="well">
    <div>
        <h2><?=$lang->getLang("Support")?></h2>
        <h4><?=$lang->getLang("Please complete the form below, and we'll do our best to resolve your problem")?>.</h4>
        <form id='livehelp' action="/livehelp" method="post">
            <div>
                <table>
                    <tr>
                        <td><h5 class="fleft"><?=$lang->getLang("What is your question about?")?></h5><br>
                                <select name='help_category'  id='help_category'>
                                    <option value='Account'><?=$lang->getLang("Account")?></option>
                                    <option value='Payments'><?=$lang->getLang("Payments")?></option>
                                    <option value='Other'><?=$lang->getLang("Other")?></option>
                                </select>
                        </td>
                    </tr>
                </table>
            </div><br />
            <div class="row">
            <div class="span4"><div class="label label-inverse"><?=$lang->getLang("Name")?></div>
                <br>
                <div class="controls">
                    <div class="input-prepend">
                        <span class="add-on"><i class="icon-align-justify"></i></span>
                        <input type="text" name='name' value='' placeholder="<?=$lang->getLang("Your name here")?>" />
                    </div>
                </div>
            </div>
                <div class="span4"><div class="label label-inverse"><?=$lang->getLang("Subject")?></div>
                    <br>
                    <div class="controls">
                        <div class="input-prepend">
                            <span class="add-on"><i class="icon-align-justify"></i></span>
                            <input type="text" name='subject' value=''class="contact_form" placeholder="<?=$lang->getLang("The subject")?>" />
                        </div>
                    </div>
                </div>
            </div>

            <div class="row">
                <div class="span4"><div class="label label-inverse"><?=$lang->getLang("Email")?></div>
                    <br>
                    <div class="controls">
                        <div class="input-prepend">
                            <span class="add-on"><i class="icon-align-justify"></i></span>
                            <input type="text" name='email' value='' class="contact_form" placeholder="<?=$lang->getLang("Your email here")?>" />
                        </div>
                    </div>
                </div>
                <div class="span4"><div class="label label-inverse"><?=$lang->getLang("Email")?></div>
                    <br>
                    <div class="controls">
                        <textarea rows="10" style="width: 100%" name='message' class="contact_form" placeholder="<?=$lang->getLang("Your question")?>" ></textarea>
                    </div>
                </div>
            </div>

            <input class="btn btn-primary" type="submit" value="<?=$lang->getLang("Submit")?>" />
        </form>
    </div>
    <div class="clearfix"></div>
</div>


<?php
    if(isset($_POST['subject'])){
        require_once("Mail.php");
        $from = $_POST['email'];
        $to = "marian@rgfairplay.com";
        $subject="[".$_POST['help_category']."]";
        $subject.=  " "  . $_POST['subject'];
        if($_POST['subject']==''){
            addError('',$lang->getLang('Empty subject'));
        }
        if($_POST['email']==''){
            addError('',$lang->getLang('Empty email'));
        }
        if($_POST['message']==''){
            addError('',$lang->getLang('Empty message'));
        }
        if($_POST['name']==''){
            addError('',$lang->getLang('Empty name'));
        }
        $body = $_POST['message'];

        $host = "10.15.0.8";

        $headers = array('From' => $from, 'To' => $to, 'Subject' => $subject);

        $smtp = Mail::factory('smtp', array ('host' => $host,
            'auth' => false));

        $mail = $smtp->send($to, $headers, $body);

       ?> <div id='results'>

        <?
        if(areErrors()){
            showErrors();
        }
        else
        {
            if ( PEAR::isError($mail) )
            {
                addError('', $mail->getMessage());
            }
            if(areErrors())
            {
                showErrors();
            }
            else
            {
            ?>
                <h3 class="text-success"><?=$lang->getLang('Mail successfully delivered!')?></h3>
            <?
            }
        }
        ?>
        </div>
    <?

    }





?>