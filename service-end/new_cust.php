<?php
require "use_daoru.php";
//��ȡ��ر���
$cardid = trim($_REQUEST['cardid']);
$name =trim($_REQUEST['name']);

$gender = $_REQUEST['gender'];
$folk = $_REQUEST['folk'];
$birthday = $_REQUEST['birthday'];
$addr = $_REQUEST['addr'];
$agency = trim($_REQUEST['agency']);
$expire = $_REQUEST['expire'];
$expirebegin = $_REQUEST['expirebegin'];
$SAMID = trim($_REQUEST['SAMID']);
$IINSNDN = trim($_REQUEST['IINSNDN']);


$face = str_replace(' ','+',trim($_REQUEST['face'],"'"));
$cardfront = str_replace(' ','+',trim($_REQUEST['cardfront'],"'"));
$cardback = str_replace(' ','+',trim($_REQUEST['cardback'],"'"));

$face = addslashes(base64_decode($face));
$cardfront = addslashes(base64_decode($cardfront));
$cardback = addslashes(base64_decode($cardback));

$aid = trim($_REQUEST['aid'],"'");
$buy = trim($_REQUEST['buy'],"'");
//��ȡ��������


//����index_base��������֤ˢ������ֱ��д�룬���ֻ�������Ϳ��ţ�����ʱ��д�롣
if($folk&&$birthday&&$addr&&$agency&&$expire&&$expirebegin&&$SAMID){
	$query = "insert into index_base (cardid, name, sex, nationality, birth, address, issued, expiration, expirebegin, cardtype, SAMID, IINSNDN,face,cardfront,cardback) values ($cardid, $name, $gender, $folk, $birthday, $addr, $agency, $expire, $expirebegin, 1, $SAMID, $IINSNDN,\"$face\",\"$cardfront\",\"$cardback\")";
	mysql_query($query);
}
else if($buy==1){
	$query="insert into index_base (cardid, name) values ($cardid, $name)";
	mysql_query($query);
}

//���蹺Ʊ����Ҫ����index_b2a
if($buy==1){

	$bid = mysql_insert_id();
	$time=date("Y-m-d",time());
	$time.=":";

	//��ȡ���û��limit
	$query = "select `limit` from index_activities where id = $aid";
	$result = mysql_query($query);
	$row = mysql_fetch_row($result);
	//var_dump($row);
	$time.=$row[0];

	$query = "insert into index_b2a (bid, aid, `limit`, now) values ($bid, $aid, '$time', 1)";
	$result = mysql_query($query);
	if($result)echo("1");
	else echo("0");
}

?>