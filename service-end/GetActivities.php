<?php

require "use_daoru.php";
$top = trim($_REQUEST['top']);
$time = time();
if($top=='\'0\'')$query = "select `id`,`name` from `index_activities` where `top`= ".$top." and `end` > ".time();
else $query = "select `id`,`name` from `index_activities` where `top`= ".$top;
$result = mysql_query($query);
if(!$result){
$message = 'Invalid query: '.mysql_error()."\n";
$message.= 'Whole query: '.$query;
die($message);
}

$num = mysql_num_rows($result);
for($i=0;$i<$num;$i++){
	$row = mysql_fetch_row($result);
	echo($row[0].":".$row[1].",");
}

?>