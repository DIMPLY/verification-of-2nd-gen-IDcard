<?php
require "use_daoru.php";
//获取相关变量
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
//获取变量结束


//更新index_base，如果身份证刷卡，则直接写入，如果只有姓名和卡号，则购买时才写入。
if($folk&&$birthday&&$addr&&$agency&&$expire&&$expirebegin&&$SAMID){
	$query = "insert into index_base (cardid, name, sex, nationality, birth, address, issued, expiration, expirebegin, cardtype, SAMID, IINSNDN,face,cardfront,cardback) values ($cardid, $name, $gender, $folk, $birthday, $addr, $agency, $expire, $expirebegin, 1, $SAMID, $IINSNDN,\"$face\",\"$cardfront\",\"$cardback\")";
	mysql_query($query);
}
else if($buy==1){
	$query="insert into index_base (cardid, name) values ($cardid, $name)";
	mysql_query($query);
}

//如需购票，还要更新index_b2a
if($buy==1){

	$bid = mysql_insert_id();
	$time=date("Y-m-d",time());
	$time.=":";

	//先取出该活动的limit
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