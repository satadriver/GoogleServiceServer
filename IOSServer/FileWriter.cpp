

#include "FileWriter.h"
#include "DataProcess.h"
#include <Dbghelp.h>
#include "PublicFunction.h"
#include "Coder.h"
#include "shlwapi.h"
#include "iosmysqlOper.h"
#include "phonelocation.h"
#include "PhoneContacts.h"
#include "jsonParser.h"
#include "DataKeeper.h"

#include <string>

using namespace std;
#pragma comment(lib,"shlwapi.lib")




int FileWriter::writeDataFile(string filepath,string name,const char * data,int size, int type,string user,string imei)
{
	int iRet = 0;
	char szout[1024];
	iRet = MakeSureDirectoryPathExists(filepath.c_str());

	string filename = filepath + name;

	HANDLE hfile = CreateFileA(filename.c_str(), GENERIC_WRITE, 0, 0, type, FILE_ATTRIBUTE_NORMAL, 0);
	if (hfile == INVALID_HANDLE_VALUE) {
		wsprintfA("CreateFileA:%s error\r\n", filename.c_str());
		WriteLogFile(szout);
		return -7;
	}

	iRet = SetFilePointer(hfile, 0, 0, FILE_END);

	DWORD cnt = 0;
	iRet = WriteFile(hfile, data, size, &cnt, 0);
	CloseHandle(hfile);
	if (iRet == FALSE || cnt != size) {
		wsprintfA("WriteFile:%s error\r\n", filename.c_str());
		WriteLogFile(szout);
		return -7;
	}

	//iRet = DataProcess::DataNotify(filename);

	iRet = DataKeeper::keep(filepath, name, data, size, user, imei);
	if (iRet <= 0)
	{
		KEEPDATA data;
		data.filename = name;
		data.filepath = filepath;
		data.imei = imei;
		data.username = user;
		DataKeeper::add(data);
	}

	return iRet;
}




int FileWriter::writeFile(const char * filepath, const char * data, int size, int type, string user, string imei)
{
	int iRet = 0;
	char szout[1024];

	iRet = MakeSureDirectoryPathExists(filepath);

	char szfn[MAX_MESSAGE_SIZE] = { 0 };
	int fnlen = *(int*)data;
	if (fnlen >= MAX_MESSAGE_SIZE || fnlen <= 0)
	{
		wsprintfA(szout, "parse file name error,user:%s,imei:%s\r\n", user.c_str(), imei.c_str());
		WriteLogFile(szout);
		return -6;
	}
	memmove(szfn, data + sizeof(int), fnlen);

	char szgbkfn[MAX_MESSAGE_SIZE];
	iRet = Coder::UTF8FNToGBKFN(szfn, fnlen, szgbkfn, MAX_MESSAGE_SIZE);
	if (iRet <= 0)
	{
		return -6;
	}

	string subpath = string(szgbkfn);
	int pos = subpath.find("/");
	if (pos >= 0)
	{
		PathStripPathA(szgbkfn);
		subpath = szgbkfn;
	}
	else {
		pos = subpath.find("\\");
		if (pos >= 0)
		{
			PathStripPathA(szgbkfn);
			subpath = szgbkfn;
		}
	}

	string filename = filepath + subpath;

	int offset = sizeof(int) + fnlen + sizeof(int);

	const char * writedata = data + offset;
	int writesize = size - (offset);

	HANDLE hfile = CreateFileA(filename.c_str(), GENERIC_WRITE, 0, 0, type, FILE_ATTRIBUTE_NORMAL, 0);
	if (hfile == INVALID_HANDLE_VALUE) {
		wsprintfA("CreateFileA:%s error\r\n", filename.c_str());
		WriteLogFile(szout);
		return -7;
	}

	iRet = SetFilePointer(hfile, 0, 0, FILE_END);

	DWORD cnt = 0;
	iRet = WriteFile(hfile, writedata, writesize, &cnt, 0);
	CloseHandle(hfile);
	if (iRet == FALSE || cnt != writesize) {
		wsprintfA("WriteFile:%s error\r\n", filename.c_str());
		WriteLogFile(szout);
		return -7;
	}

// 	if (filename.find("\\Download\\") != -1 || filename.find("\\Audio\\") != -1 ||
// 		filename.find("\\Video\\") != -1 || filename.find("\\QQFiles\\") != -1 || filename.find("\\QQVideo\\") != -1 ||
// 		filename.find("\\QQAudio\\") != -1 || filename.find("\\QQPhoto\\") != -1 || filename.find("\\QQHead\\") != -1||
// 		filename.find("\\WeiXinPhoto\\") != -1 || filename.find("\\WeiXinAudio\\") != -1 ||
// 		filename.find("\\WeiXinVideo\\") != -1 || filename.find("\\OfficeFiles\\") != -1 
// 		)
// 	{
// 		return iRet;
// 	}
// 	iRet = DataProcess::DataNotify(filename);

	iRet = DataKeeper::keep(filepath, subpath, writedata, writesize, user, imei);
	if (iRet <= 0)
	{
		KEEPDATA data;
		data.filename = subpath;
		data.filepath = filepath;
		data.imei = imei;
		data.username = user;
		DataKeeper::add(data);
	}

	return iRet;
}



// int FileWriter::writeFile(const char * path, const char * data, int size, int type,int strip)
// {
// 	int iRet = 0;
// 	iRet = MakeSureDirectoryPathExists(path);
// 
// 	char szfn[MAX_MESSAGE_SIZE] = { 0 };
// 	int fnlen = *(int*)data;
// 	if (fnlen >= MAX_MESSAGE_SIZE)
// 	{
// 		return FALSE;
// 	}
// 	memmove(szfn, data + sizeof(int), fnlen);
// 
// 	char szgbkfn[MAX_MESSAGE_SIZE];
// 	iRet = Coder::UTF8FNToGBKFN(szfn, fnlen, szgbkfn, MAX_MESSAGE_SIZE);
// 	if (iRet <= 0)
// 	{
// 		return FALSE;
// 	}
// 
// 	if (strip)
// 	{
// 		PathStripPathA(szgbkfn);
// 	}
//
// 	string filename = path + string(szgbkfn);
// 
// 	int offset = sizeof(int) + fnlen + sizeof(int);
// 
// 	const char * writedata = data + offset;
// 	int writesize = size - (offset);
// 
// 	HANDLE hfile = CreateFileA(filename.c_str(), GENERIC_WRITE, 0, 0, type, FILE_ATTRIBUTE_NORMAL, 0);
// 	if (hfile == INVALID_HANDLE_VALUE) {
// 		return FALSE;
// 	}
// 
// 	iRet = SetFilePointer(hfile, 0, 0, FILE_END);
// 
// 	DWORD cnt = 0;
// 	iRet = WriteFile(hfile, writedata, writesize, &cnt, 0);
// 	CloseHandle(hfile);
// 	if (iRet == FALSE || cnt != writesize) {
// 		return FALSE;
// 	}
// 
// 	iRet = DataProcess::DataNotify(filename);
// 	return iRet;
// }





