<?php
try
{
    $payload = json_decode($_REQUEST['payload']);
}
catch(Exception $e)
{
    exit(0);
}

file_put_contents('logs/github.txt', print_r($payload, TRUE), FILE_APPEND);

if($payload->ref === 'refs/heads/master')
{
    exec('/var/www/skittle-development/git_pull_ff8lk892kfppqxrt784qoicheu321.sh');
}