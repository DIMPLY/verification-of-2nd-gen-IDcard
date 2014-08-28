<?php

require "use_daoru.php";
//获取相关变量
$matchid = trim($_REQUEST['matchid']);
$matchname = trim($_REQUEST['matchname']);

$cardid = trim($_REQUEST['cardid']);
$name = trim($_REQUEST['name']);

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


//$fp = fopen("/www/modernsky/test_id_validater/1.jpg","wb") or die("can't open file");
//fwrite($cardfront,$fp);
//fclose($fp);

$aid = $_REQUEST['aid'];
$ticket = trim($_REQUEST['ticket'],"'");
$time=date("Y-m-d",time());

//查找匹配的用户bid
$query = "select id from index_base where cardid = $matchid and name = $matchname";
$result = mysql_query($query);
//	if(!$result){
//	$message = 'Invalid query: '.mysql_error()."\n";
//	$message.= 'Whole query: '.$query;
//	die($message);
//	}
$row = mysql_fetch_row($result);
$bid = $row[0];

//更新该bid下的index_base二代证信息
if($folk&&$birthday&&$addr&&$agency&&$expire&&$expirebegin&&$SAMID){
	$query = "update index_base set cardtype=1, name=$name, sex=$gender, nationality=$folk, birth=$birthday, address=$addr,expiration=$expire, expirebegin=$expirebegin,  cardid=$cardid,issued=$agency,SAMID= $SAMID,IINSNDN=$IINSNDN,face=\"$face\",cardfront=\"$cardfront\",cardback=\"$cardback\" where id=$bid";
	$result = mysql_query($query);
//	if(!$result){
//	$message = 'Invalid query: '.mysql_error()."\n";
//	$message.= 'Whole query: '.$query;
//	die($message);
//	}
//echo "updated";
}
else if($matchid!=$cardid||$matchname!=$name){
	if($cardid!="''")mysql_query("update index_base set cardid=$cardid where id=$bid");
	if($name!="''")mysql_query("update index_base set name=$name where id=$bid");
	$result = mysql_query("update index_base set cardtype=1 where id=$bid");

	//if(!$result){
	//$message = 'Invalid query: '.mysql_error()."\n";
	//$message.= 'Whole query: '.$query;
	//die($message);
	//}
}

//已有票的情况
if($ticket=='1'){
	$query="select `limit`,`now`,`id` from index_b2a where bid=$bid and aid=$aid and `limit` like '%$time%'";
	$result = mysql_query($query);
	$row = mysql_fetch_row($result);
	$limit=$row[0];$now=$row[1];$id=$row[2];
	$limits=explode(',',$limit);
	for($i=0;$i<count($limits);$i++){
		$temp = explode(':',$limits[$i]);
		if($temp[0]==$time)$num_limit=$temp[1];
	}
	if($now==NULL)$now=0;
	if($num_limit=='0'||$now<$num_limit){
	//now=now+1
	$query="update index_b2a set now=$now+1 where id=$id";
	mysql_query($query);
	die('0');
	}
	else die($id." ".$limit);
}
//新购票情况，
else if($ticket=='0'){
	$addlimit = $time.":";
	//先取出该活动的limit
	$query = "select `limit` from index_activities where id = $aid";
	$result = mysql_query($query);
	$row = mysql_fetch_row($result);
	//var_dump($row);
	$addlimit.=$row[0];

	$query="update index_b2a set now=1,`limit`=concat(`limit`,',','$addlimit') where bid=$bid and aid=$aid";
	mysql_query($query);

//	if(!$result){
//	$message = 'Invalid query: '.mysql_error()."\n";
//	$message.= 'Whole query: '.$query;
//	die($message);
//	}

$i =  mysql_affected_rows();
	if($i>0)die("0");
	else{
		$query="insert into index_b2a (bid,aid,`limit`,now) values ($bid,$aid,'$addlimit',1)";
		if(mysql_query($query))die("0");
		else die($query);
	}
}
else die("not 0 and not 1");
?>