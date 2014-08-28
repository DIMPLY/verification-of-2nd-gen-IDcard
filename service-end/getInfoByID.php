<?php
require "use_daoru.php";
$ID = trim($_REQUEST['code']);

$query = "select name,sex,nationality,birth,address,cardid,issued,expirebegin,expiration,SAMID,face from index_base where cardid=\"$ID\"";

$result = mysql_query($query);
	if(!$result){
	$message = 'Invalid query: '.mysql_error()."\n";
	$message.= 'Whole query: '.$query;
	die($message);
	}
//var_dump($result);
$row = mysql_fetch_row($result);
//var_dump($row);
$num = count($row);
for($i=0;$i<$num;$i++){
	if($i==$num-1)echo base64_encode($row[$i]).",";
	else if($i==1)echo $row[$i]=='0'?"女,":"男,";
	else echo $row[$i].",";
}

?>