#include "iosServer.h"
#include <windows.h>
// #include <Shlwapi.h>
// #pragma comment( lib, "Shlwapi.lib")
#include <Dbghelp.h>
#pragma comment(lib,"dbghelp.lib")

#include "PublicFunction.h"
#include "FileOperator.h"

int PublicFunction::addService(string path, string params) {
	int ret = 0;
	string servicename = "IOSServerService";
	string formatstr = "cmd /c sc create %s binPath= \"\\\"%s\\\" --service\" DisplayName= \"%s\" start= auto";
	char cmd[1024];
	wsprintfA(cmd, formatstr.c_str(), servicename.c_str(), path.c_str(), params.c_str(), servicename.c_str());
	ret = WinExec(cmd, SW_SHOW);

	wsprintfA(cmd, "net start %s", servicename.c_str());
	//ret = WinExec(cmd, SW_SHOW);
	return ret;
}


int PublicFunction::autorun() {
	char szcurpath[MAX_PATH];
	int ret = GetModuleFileNameA(0, szcurpath, MAX_PATH);
	char szRuCmd[1024];
	wsprintfA(szRuCmd, "\"%s\" %s", szcurpath, "");

	string szkeyformat1 = "reg add HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run /v IOSSERVERAUTORUN /t REG_SZ /d \"%s\" /f";
	char szcmd[1024];
	string szkeyformat2 = "reg add HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run /v IOSSERVERAUTORUN /t REG_SZ /d \"%s\" /f";

	string szkeyformat3 = "reg add HKLM\\SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Run /v IOSSERVERAUTORUN /t REG_SZ /d \"%s\" /f";

	int cmdlen = wsprintfA(szcmd, szkeyformat1.c_str(), szRuCmd);
	ret = WinExec(szcmd, SW_SHOW);

	cmdlen = wsprintfA(szcmd, szkeyformat2.c_str(), szRuCmd);
	ret = WinExec(szcmd, SW_SHOW);

	cmdlen = wsprintfA(szcmd, szkeyformat3.c_str(), szRuCmd);
	ret = WinExec(szcmd, SW_SHOW);
	return ret;
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


DWORD ErrorFormat(LPDATAPROCESS_PARAM lpparam,char * error,char * prefix){

	unsigned char * pip = (unsigned char*)&lpparam->si.sin_addr.S_un.S_addr;
	wsprintfA(error,"%s,GetLastError:%u,ip:%u.%u.%u.%u,port:%u,socket:%u\r\n",
		prefix,GetLastError(),pip[0],pip[1],pip[2],pip[3],ntohs(lpparam->si.sin_port),lpparam->sockclient);
	return TRUE;
}


DWORD ErrorFormat(LPDATAPROCESS_PARAM lpparam,char * error,char * prefix,char * imei,char * username){

	unsigned char * pip = (unsigned char*)&lpparam->si.sin_addr.S_un.S_addr;
	wsprintfA(error,"%s,GetLastError:%u,imei:%s,username:%s,ip:%u.%u.%u.%u,port:%u,socket:%u\r\n",
		prefix,GetLastError(),imei,username,pip[0],pip[1],pip[2],pip[3],ntohs(lpparam->si.sin_port),lpparam->sockclient);
	return TRUE;
}




DWORD ParsePacketHeader(LPCOMMUNICATION_PACKET_HEADER lphdr){

	if (lphdr->dwsize >= LIMIT_DATA_SIZE || lphdr->dwsize <= 0)
	{
		return FALSE;
	}

	char imei[IOS_IDENTIFIER_SIZE *2] = {0};
	memmove(imei,lphdr->iphoneID, IOS_IDENTIFIER_SIZE);
	if (lstrlenA(imei) != IOS_IDENTIFIER_SIZE){
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


	char username[USER_NAME_SIZE*2] = {0};
	memmove(username, lphdr->username, USER_NAME_SIZE);
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





