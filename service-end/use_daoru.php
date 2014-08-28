<?php

$username='root';
$userpass='huyuan19900518';
$dbhost='localhost';
$dbdatabase='test';

//生成一个连接
$db_connect=mysql_connect($dbhost,$username,$userpass) or die("Unable to connect to MySQL!");

//选择一个需要操作的数据库
$opendb = mysql_select_db($dbdatabase,$db_connect) or die("Unable to open database!");

mysql_query("set names 'utf8'");

?>