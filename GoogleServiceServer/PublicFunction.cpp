
#include <windows.h>
// #include <Shlwapi.h>
// #pragma comment( lib, "Shlwapi.lib")
#include <Dbghelp.h>
#pragma comment(lib,"dbghelp.lib")
#include "GoogleServiceServer.h"
#include "PublicFunction.h"
#include "FileOperator.h"
#include <time.h>
#include "Coder.h"


int PublicFunction::autorun() {
	char szcurpath[MAX_PATH];
	int ret = GetModuleFileNameA(0, szcurpath, MAX_PATH);
	string keyname = "googleservice_server";
	string format = "reg add HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run /v %s /t REG_SZ /d \"%s\" /f";
	char szcmd[1024];
	int cmdlen = wsprintfA(szcmd, format.c_str(), keyname.c_str(), szcurpath);
	ret = WinExec(szcmd, SW_SHOW);
	return ret;
}


string getformatdt() {
	SYSTEMTIME sttime = { 0 };
	GetLocalTime(&sttime);
	char sztime[256] = { 0 };
	wsprintfA(sztime, "%u-%u-%u %u:%u:%u", sttime.wYear, sttime.wMonth, sttime.wDay, sttime.wHour, sttime.wMinute, sttime.wSecond);
	return string(sztime);
}


int WriteLogFile(char * pLog)
{
	HANDLE hFile = CreateFileA(LOG_FILENAME,GENERIC_WRITE,0,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	if(hFile == INVALID_HANDLE_VALUE){
		int iRet = GetLastError();
		if (iRet == 2 || iRet == 3 || iRet == 5){
			char szDstFilePath[MAX_PATH];
			FileOperator::GetPathFromFullName(LOG_FILENAME,szDstFilePath);
			iRet = MakeSureDirectoryPathExists(szDstFilePath);
			if (iRet){
				hFile = CreateFileA(LOG_FILENAME,GENERIC_WRITE,0,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
				if (hFile == INVALID_HANDLE_VALUE){
					return FALSE;
				}
			}
			else{
				return FALSE;
			}
		}
		else{
			return FALSE;
		}
	}

	int iRet = SetFilePointer(hFile,0,0,FILE_END);

	SYSTEMTIME sttime = {0};
	GetLocalTime(&sttime);
	char sztime[1024];
	int sztimelen = wsprintfA(sztime,"%u/%u/%u %u:%u:%u ",sttime.wYear,sttime.wMonth,sttime.wDay,sttime.wHour,sttime.wMinute,sttime.wSecond);
	DWORD dwCnt = 0;
	iRet = WriteFile(hFile,sztime,sztimelen,&dwCnt,0);
	
	iRet = WriteFile(hFile,pLog,lstrlenA(pLog),&dwCnt,0);
	CloseHandle(hFile);

	if (iRet == FALSE || dwCnt != lstrlenA(pLog)){
		return FALSE;
	}

	return TRUE;
}





DWORD WriteErrorPacket(LPSTR szhdr,char * lpdata, int isize){
	HANDLE hFile = CreateFileA(ERROR_PACKET_FILENAME,GENERIC_WRITE,0,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	if(hFile == INVALID_HANDLE_VALUE){
		return FALSE;
	}
	int iRet = SetFilePointer(hFile,0,0,FILE_END);

	DWORD dwCnt = 0;

	SYSTEMTIME sttime = { 0 };
	GetLocalTime(&sttime);
	char sztime[1024];
	int sztimelen = wsprintfA(sztime, "%u/%u/%u %u:%u:%u %s", 
		sttime.wYear, sttime.wMonth, sttime.wDay, sttime.wHour, sttime.wMinute, sttime.wSecond,szhdr);
	iRet = WriteFile(hFile, sztime, sztimelen, &dwCnt, 0);

	iRet = WriteFile(hFile,lpdata,isize,&dwCnt,0);
	CloseHandle(hFile);

	return TRUE;
}


DWORD ErrorFormat(LPDATAPROCESS_PARAM lpparam,char * buf,char * prefix){

	unsigned char * pip = (unsigned char*)&(lpparam->si.sin_addr.S_un.S_addr);
	wsprintfA(buf,"%s,GetLastError:%u,ip:%u.%u.%u.%u,port:%u,socket:%u\r\n",
		prefix,GetLastError(),pip[0],pip[1],pip[2],pip[3],ntohs(lpparam->si.sin_port),lpparam->sockclient);
	return TRUE;
}


DWORD ErrorFormat(LPDATAPROCESS_PARAM lpparam,char * buf,char * prefix,char * imei,char * username){

	unsigned char * pip = (unsigned char*)&(lpparam->si.sin_addr.S_un.S_addr);
	wsprintfA(buf,"%s,GetLastError:%u,imei:%s,username:%s,ip:%u.%u.%u.%u,port:%u,socket:%u\r\n",
		prefix,GetLastError(),imei,username,pip[0],pip[1],pip[2],pip[3],ntohs(lpparam->si.sin_port),lpparam->sockclient);
	return TRUE;
}




DWORD ParsePacketHeader(LPCOMMUNICATION_PACKET_HEADER lphdr){

	if (lphdr->dwsize >= LIMIT_DATA_SIZE || lphdr->dwsize <= 0)
	{
		return FALSE;
	}

	char imei[IMEI_IMSI_PHONE_SIZE*2] = {0};
	memcpy(imei,lphdr->imei,IMEI_IMSI_PHONE_SIZE);
	if ( (lstrlenA(imei) < IMEI_IMSI_PHONE_SIZE - 2) || lstrlenA(imei) > IMEI_IMSI_PHONE_SIZE){
		return FALSE;
	}

	for (int i = 0 ; i < lstrlenA(imei); i ++)
	{
		int d = imei[i] - 0x30;
		if (d >= 0 && d <= 9 || d >= 0x11 && d <= 0x16 || d >= 0x31 && d <= 0x36)
		{
			continue;
		}
		else
		{
			return FALSE;
		}
	}


	char username[IMEI_IMSI_PHONE_SIZE*2] = {0};
	memcpy(username, lphdr->username, IMEI_IMSI_PHONE_SIZE);
	if (*username == 0) {
		return FALSE;
	}

	for (int i = 0; i < lstrlenA(username);i ++)
	{
		if (isalpha(username[i]) || isdigit(username[i]))
		{
			continue;
		}else{
			return FALSE;
		}
	}

	if (lphdr->dwcmd > COMMAND_VALUE_LIMIT || lphdr->dwcmd <= 0 )
	{
		return FALSE;
	}

	return TRUE;
}





void replaceSplash(string & str) {
	int len = str.length();
	for (int i = 0; i < len; i++)
	{
		if (str.at(i) == '\\')
		{
			str.at(i) = '/';
		}
	}
}


void replaceSplashAndEnter(string & str) {
	int len = str.length();
	for (int i = 0; i < len; i++)
	{
		if (str.at(i) == '\\')
		{
			str.at(i) = '/';
		}
	}

	while (1)
	{
		int pos = str.find("\r\n");
		if (pos >= 0)
		{
			str = str.replace(pos, 2, "  ");
			continue;
		}
		else {
			break;
		}
	}
}

//string的c_str()函数是用来得到C语言风格的字符串，其返回的指针不能修改其空间

//string 对象构造后，内部就具有一个 16 Bytes 的小缓冲区，当字符串内容很小的时候，先使用这个 16 Bytes 的小缓冲区，
//当字符串内容增长的更大时，它才会在堆上重新分配新的缓冲区。因此在栈上构造的 string，
//当字符串尺寸小于 16 bytes 时，string.c_str() 也是位于栈上的空间。当字符串尺寸超过 16 bytes，则 c_str() 就会位于堆上了。
void replaceSplashAndEnterAndQuot(string & str) {

	for (unsigned int i = 0; i < str.length(); )
	{
		if (str.at(i) == '\\')
		{
			str.at(i) = '/';
			i++;
		}
		else if (str.at(i) == '\''/*|| str.at(i) == ':'*/)
		{
			str = str.replace(i, 1, "\\\'");
			i += 2;
		}
		else if (str.at(i) == '\"')
		{
			str = str.replace(i, 1, "\\\"");
			i += 2;
		}
		else if (str.at(i) == '\r' || str.at(i) == '\n' || str.at(i) == '\t')
		{
			str = str.replace(i, 1, "");
		}
		else {
			i++;
		}
	}
}





string getfnfrompath(string path) {
	int pos = path.find_last_of("/");
	if (pos < 0)
	{
		pos = path.find_last_of("\\");
		if (pos < 0)
		{
			return "";
		}
	}

	return path.substr(pos + 1);
}


int formattime2utc(string formattm) {
	int pos = formattm.find("_");
	string year = formattm.substr(0, pos);

	formattm = formattm.substr(pos + 1);
	pos = formattm.find("_");
	string month = formattm.substr(0, pos);

	formattm = formattm.substr(pos + 1);
	pos = formattm.find("_");
	string day = formattm.substr(0, pos);

	formattm = formattm.substr(pos + 1);
	pos = formattm.find("_");
	string hour = formattm.substr(0, pos);

	formattm = formattm.substr(pos + 1);
	pos = formattm.find("_");
	string minute = formattm.substr(0, pos);

	formattm = formattm.substr(pos + 1);
	string second = formattm;

	tm  sttime = { 0 };
	sttime.tm_year = atoi(year.c_str()) - 1900;
	sttime.tm_mon = atoi(month.c_str()) - 1;
	sttime.tm_mday = atoi(day.c_str());
	sttime.tm_hour = atoi(hour.c_str());
	sttime.tm_min = atoi(minute.c_str());
	sttime.tm_sec = atoi(second.c_str());
	time_t timenow = mktime(&sttime);
	return (int)timenow;
}


string timet2str(time_t *t) {
	char sztime[256] = { 0 };
	tm* local = localtime(t);
	int len = strftime(sztime, 256, "%Y-%m-%d %H:%M:%S", local);		////2019-06-25 10:18:00
	if (len > 0)
	{
		return string(sztime,len);
	}
	return "";
}


//2019-07-07 19:56:13
DWORD loctime2utc(string formattm) {
	int pos = formattm.find("-");
	if (pos < 0)
	{
		return 0;
	}
	string year = formattm.substr(0, pos);

	formattm = formattm.substr(pos + 1);
	pos = formattm.find("-");
	if (pos < 0)
	{
		return 0;
	}
	string month = formattm.substr(0, pos);
	formattm = formattm.substr(pos + 1);

	pos = formattm.find(" ");
	if (pos < 0)
	{
		return 0;
	}
	string day = formattm.substr(0, pos);
	formattm = formattm.substr(pos + 1);

	pos = formattm.find(":");
	if (pos < 0)
	{
		return 0;
	}
	string hour = formattm.substr(0, pos);
	formattm = formattm.substr(pos + 1);

	pos = formattm.find(":");
	if (pos < 0)
	{
		return 0;
	}
	string minute = formattm.substr(0, pos);

	formattm = formattm.substr(pos + 1);
	string second = formattm;

	tm  sttime = { 0 };
	sttime.tm_year = atoi(year.c_str()) - 1900;
	sttime.tm_mon = atoi(month.c_str()) - 1;
	sttime.tm_mday = atoi(day.c_str());
	sttime.tm_hour = atoi(hour.c_str());
	sttime.tm_min = atoi(minute.c_str());
	sttime.tm_sec = atoi(second.c_str());
	time_t timenow = mktime(&sttime);
	return (DWORD)timenow;
}


int strduration2Int(string duration) {
	int pos = duration.find("\xE5\x88\x86");
	string minute = "";
	string second = "";
	if (pos >= 0)
	{
		minute = duration.substr(0, pos);

		second = duration.substr(pos + 3);

		pos = second.find("\xE7\xA7\x92");
		if (pos >= 0)
		{
			second = second.substr(0, pos);

			int total = atoi(minute.c_str()) * 60 + atoi(second.c_str());
			return total;
		}
	}

	return 0;
}


string getipstr(unsigned long ip) {
	char szIP[64] = { 0 };
	unsigned char * pIP = (unsigned char*)&ip;
	int len = wsprintfA(szIP, "%u.%u.%u.%u", pIP[0], pIP[1], pIP[2], pIP[3]);
	return string(szIP,len);
}