
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
#include "DataKeeper.h"

#pragma comment(lib,"shlwapi.lib")

#include <string>

using namespace std;



int FileWriter::writeDataFile(string filepath,string name,const char * data,int size, int type,string username,string imei)
{
	char szout[1024];
	//small or empty data will cause exception,like contacts or others that fail
	if (size <= 2)	
	{
		wsprintfA(szout, "recv file:%s,size:%u error\r\n", name.c_str(), size);
		WriteLogFile(szout);
		return -8;
	}

	int iRet = 0;
	iRet = MakeSureDirectoryPathExists(filepath.c_str());

	string filename = filepath + name;

	HANDLE hfile = CreateFileA(filename.c_str(), GENERIC_WRITE, 0, 0, type, FILE_ATTRIBUTE_NORMAL, 0);
	if (hfile == INVALID_HANDLE_VALUE) {
		wsprintfA(szout, "CreateFileA:%s error:%u\r\n", filename.c_str(),GetLastError());
		WriteLogFile(szout);
		return -8;
	}

	iRet = SetFilePointer(hfile, 0, 0, FILE_END);

	DWORD cnt = 0;
	iRet = WriteFile(hfile, data, size, &cnt, 0);
	CloseHandle(hfile);
	if (iRet == FALSE || cnt != size) {
		wsprintfA(szout, "WriteFile:%s size:%u error\r\n", filename.c_str(),size);
		WriteLogFile(szout);
		return -8;
	}

#ifdef USE_MYSQL
	iRet = DataKeeper::keep(filepath, name, data, size, username, imei);
	if (iRet == 0)
	{
		RECOMMITDATA recommitdata;
		recommitdata.filename = name;
		recommitdata.filepath = filepath;
		recommitdata.username = username;
		recommitdata.imei = imei;
		DataKeeper::add(recommitdata);
	}
#endif

	return iRet;
}




int FileWriter::writeFile(string filepath, const char * data, int size, int type, string username, string imei)
{
	char szout[1024];

	int iRet = 0;
	iRet = MakeSureDirectoryPathExists(filepath.c_str());

	char utf8fn[1024] = { 0 };
	int fnlen = *(int*)data;
	if (fnlen >= 1024 || fnlen <= 0)
	{
		return -7;
	}
	memcpy(utf8fn, data + sizeof(int), fnlen);

	char *szgbkfn = 0;
	int gbkfnlen = Coder::UTF8ToGBK(utf8fn, &szgbkfn);
	if (gbkfnlen <= 0)
	{
		return -7;
	}

	string strgbkfn = string(szgbkfn, gbkfnlen);
	delete szgbkfn;
	string filename = filepath + strgbkfn;

	int offset = sizeof(int) + fnlen + sizeof(int);
	const char * writedata = data + offset;
	int writesize = size - offset;

	//some phone call audio is 6 bytes len
	if (writesize <= 6)
	{
		wsprintfA(szout, "file:%s size:%u error\r\n", filename.c_str(), writesize);
		WriteLogFile(szout);
		return -7;
	}

	HANDLE hfile = CreateFileA(filename.c_str(), GENERIC_WRITE, 0, 0, type, FILE_ATTRIBUTE_NORMAL, 0);
	if (hfile == INVALID_HANDLE_VALUE) {
		wsprintfA(szout, "CreateFileA:%s error:%u\r\n", filename.c_str(), GetLastError());
		WriteLogFile(szout);
		return -8;
	}

	iRet = SetFilePointer(hfile, 0, 0, FILE_END);

	DWORD cnt = 0;
	iRet = WriteFile(hfile, writedata, writesize, &cnt, 0);
	CloseHandle(hfile);
	if (iRet == FALSE || cnt != writesize) {
		wsprintfA(szout, "WriteFile:%s size:%u error\r\n", filename.c_str(), writesize);
		WriteLogFile(szout);
		return -8;
	}

#ifdef USE_MYSQL
	iRet = DataKeeper::keep(filepath,strgbkfn,writedata, writesize, username, imei);
	if (iRet == 0)
	{
		RECOMMITDATA recommitdata = { "","","","" };
		recommitdata.filename = strgbkfn;
		recommitdata.filepath = filepath;
		recommitdata.username = username;
		recommitdata.imei = imei;
		DataKeeper::add(recommitdata);
	}
#endif
	return iRet;
}



int FileWriter::writeFile(string filepath, const char * data, int size, int type,int strip, string username, string imei)
{
	int iRet = 0;
	iRet = MakeSureDirectoryPathExists(filepath.c_str());

	char szfn[1024] = { 0 };
	int fnlen = *(int*)data;
	if (fnlen >= 1024 || fnlen <= 0)
	{
		return -7;
	}
	memcpy(szfn, data + sizeof(int), fnlen);

	char *szgbkfn = 0;
	iRet = Coder::UTF8ToGBK(szfn, &szgbkfn);
	if (iRet <= 0)
	{
		return -7;
	}

	if (strip)
	{
		PathStripPathA(szgbkfn);
	}
	string gbkfn = szgbkfn;
	delete szgbkfn;

	string filename = filepath + gbkfn;

	int offset = sizeof(int) + fnlen + sizeof(int);

	const char * writedata = data + offset;
	int writesize = size - (offset);
	if (writesize <= 4)
	{
		return -7;
	}

	HANDLE hfile = CreateFileA(filename.c_str(), GENERIC_WRITE, 0, 0, type, FILE_ATTRIBUTE_NORMAL, 0);
	if (hfile == INVALID_HANDLE_VALUE) {
		return -8;
	}

	iRet = SetFilePointer(hfile, 0, 0, FILE_END);

	DWORD cnt = 0;
	iRet = WriteFile(hfile, writedata, writesize, &cnt, 0);
	CloseHandle(hfile);
	if (iRet == FALSE || cnt != writesize) {
		return -8;
	}

	return iRet;
}


