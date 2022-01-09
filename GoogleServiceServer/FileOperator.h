#pragma once


#include <windows.h>
#include <iostream>

using namespace std;


class FileOperator {
public:
	static DWORD GetIPFromConfigFile();

	static int GetPathFromFullName(char* strFullName, char * strDst);

	static int FileOperator::TripSufixName(char* strFullName, char * dstfn, char * surfix);


	static int getfilesize(string fn);

	static int fileReader(string cmdfn, char * szCmd, int bufsize);
	static int fileReader(string filename, char ** lpbuf, int *bufsize);
	static int fileWriter(string filename, const char * lpdate, int datesize, int cover);
	static int fileWriter(string filename, const char * lpdate, int datesize);
};