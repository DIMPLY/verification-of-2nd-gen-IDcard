<?php
require "use_daoru.php";

$id = trim($_REQUEST['id']);
$limit = trim($_REQUEST['limit']);
$time=date("Y-m-d",time());


$query = "update index_b2a set now=now+1, `limit`=$limit where id=$id";
$result = mysql_query($query);
if(!$result){
$message = 'Invalid query: '.mysql_error()."\n";
$message.= 'Whole query: '.$query;
die($message);
}


?>