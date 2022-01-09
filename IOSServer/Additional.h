#pragma once

#ifndef ADDITIONAL
#define ADDITIONAL

#include <windows.h>
#include <iostream>


using namespace std;

class Additional {
public:
	static int checkImei(char * imei);
	static int checkUserName(char * username);

	static string Additional::getCmdFileName(string curpath, string name, string imei);
	static string Additional::getUploadFileName(string curpath, string name);
	static string Additional::getDownloadFileName(string curpath, string name);
	static string Additional::getConfigFileName(string curpath, string imei, string username,string cfgfn);

};
#endif