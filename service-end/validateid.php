<?php
//�ύ����\���֤��\���
//����ȫƥ�䣬�򷵻�1�����л��-1�����޻
//��ȫ��ƥ�䷵��0
//��������ģ��ƥ�䣬����һ�����֤��ID:xxxxxxxxxx,xxxxxxxxx,xxxxxxxxx
//���֤�ŵ���ģ��ƥ�䣬����һ������NAME:XXX,XX,XXX,XX
require "use_daoru.php";

$userid = trim($_REQUEST['userid']);
$name = trim($_REQUEST['name']);
$aid = trim($_REQUEST['aid']);
$time=date("Y-m-d",time());

if($name&&$userid){


	//��ȷ����
	$query = "select id from index_base where cardid = $userid and name = $name";
	$result = mysql_query($query);
	//��ȡ����
	//if(!$result){
		//$message = 'Invalid query: '.mysql_error()."\n";
		//$message.= 'Whole query: '.$query;
		//die($message);
	//}
	$num = mysql_num_rows($result);

}

else $num=0;
//����ҵ����û�
if($num!=0){
	$row = mysql_fetch_row($result);
	$uid = $row[0];
	$query = "select id from index_b2a where `limit` like '%".$time."%' and bid = $uid and aid = $aid ";
	$result = mysql_query($query);
//	$row = mysql_fetch_row($result);
	if(mysql_num_rows($result))die("1");//������¼����
	else die("-1");//������¼������
}
//����Ҳ���
else {
	if($userid!="''"){
		//�����֤��ģ������
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