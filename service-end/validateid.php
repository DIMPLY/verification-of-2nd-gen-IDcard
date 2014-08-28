<?php
//提交姓名\身份证号\活动号
//有完全匹配，则返回1代表有活动，-1代表无活动
//完全无匹配返回0
//姓名单项模糊匹配，返回一串身份证号ID:xxxxxxxxxx,xxxxxxxxx,xxxxxxxxx
//身份证号单项模糊匹配，返回一串姓名NAME:XXX,XX,XXX,XX
require "use_daoru.php";

$userid = trim($_REQUEST['userid']);
$name = trim($_REQUEST['name']);
$aid = trim($_REQUEST['aid']);
$time=date("Y-m-d",time());

if($name&&$userid){


	//精确查找
	$query = "select id from index_base where cardid = $userid and name = $name";
	$result = mysql_query($query);
	//提取数据
	//if(!$result){
		//$message = 'Invalid query: '.mysql_error()."\n";
		//$message.= 'Whole query: '.$query;
		//die($message);
	//}
	$num = mysql_num_rows($result);

}

else $num=0;
//如果找到该用户
if($num!=0){
	$row = mysql_fetch_row($result);
	$uid = $row[0];
	$query = "select id from index_b2a where `limit` like '%".$time."%' and bid = $uid and aid = $aid ";
	$result = mysql_query($query);
//	$row = mysql_fetch_row($result);
	if(mysql_num_rows($result))die("1");//当天活动记录存在
	else die("-1");//当天活动记录不存在
}
//如果找不到
else {
	if($userid!="''"){
		//按身份证号模糊查找
		$query = "select id,name from index_base where cardid = $userid";
		$result = mysql_query($query);
	//	if(!$result){
	//	$message = 'Invalid query: '.mysql_error()."\n";	
	//	$message.= 'Whole query: '.$query;
	//	die($message);
	//	}
		$num = mysql_num_rows($result);

		for($i=0;$i<$num;$i++){
			$uid = mysql_result($result,$i);
			$query = "select * from index_b2a where bid = $uid and aid = $aid and `limit` like '%".$time."%'";
			$rslt = mysql_query($query);
			if($i==0)echo("NAME:");
			if(mysql_num_rows($rslt)){
				echo(mysql_result($result,$i,1));
			}
			else echo("0+".mysql_result($result,$i,1));
			echo(",");
		}
	}

	if($name!="''"){
		$query = "select id,cardid from index_base where name = $name";
		$result = mysql_query($query);
	//	if(!$result){
	//		$message = 'Invalid query: '.mysql_error()."\n";
	//		$message.= 'Whole query: '.$query;
	//		die($message);
	//	}

		for($j=0;$j<mysql_num_rows($result);$j++){
			$uid = mysql_result($result,$j);
			$query = "select * from index_b2a where bid = $uid and aid = $aid and `limit` like '%".$time."%'";
			$rslt = mysql_query($query);
			if($j==0)echo("ID:");
			if(mysql_num_rows($rslt)){
				echo(mysql_result($result,$j,1));
			}
			else echo("0+".mysql_result($result,$j,1));
			echo(",");
		}
	}

}



?>