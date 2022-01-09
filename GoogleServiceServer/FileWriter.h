#pragma once


#ifndef FILEWRITER
#define FILEWRITER

#include <windows.h>
#include <iostream>
#include "GoogleServiceServer.h"

using namespace std;

class FileWriter {
public:
	static int writeDataFile(string filepath,string filename,const char * data, int size, int type, string username, string imei);
	static int writeFile(string filepath,const char * data, int size, int type, string username, string imei);

	static int FileWriter::writeFile(string filepath, const char * data, int size, int type, int strip, string username, string imei);
};


#endif