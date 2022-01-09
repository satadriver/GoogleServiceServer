#pragma once




#include <windows.h>
#include <iostream>
#include "iosServer.h"

using namespace std;

class FileWriter {
public:
	static int writeDataFile(string filepath,string filename,const char * data, int size, int type, string user, string imei);
	static int writeFile(const char * path,const char * data, int size, int type, string user, string imei);



};


