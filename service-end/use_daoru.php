<?php

$username='root';
$userpass='huyuan19900518';
$dbhost='localhost';
$dbdatabase='test';

//����һ������
$db_connect=mysql_connect($dbhost,$username,$userpass) or die("Unable to connect to MySQL!");

//ѡ��һ����Ҫ���������ݿ�
$opendb = mysql_select_db($dbdatabase,$db_connect) or die("Unable to open database!");

mysql_query("set names 'utf8'");

?>