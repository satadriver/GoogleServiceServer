#include <windows.h>
#include "iosServer.h"
#include "PublicFunction.h"
#include "Decomp.h"
#include <Dbghelp.h>
#pragma comment(lib,"dbghelp.lib")
#include "PacketMaker.h"
#include "Coder.h"


int localFileRecver(char * lpdata,int len,char * filename,char * lpdstfn,LPDATAPROCESS_PARAM lpparam,
	char * username,char * imei,int withparam,int append){
	DATAPROCESS_PARAM stparam = *lpparam;

	char errorbuf[1024];
	char szlog[1024];
	int iRet = 0;

	COMMUNICATION_PACKET_HEADER hdr = *(LPCOMMUNICATION_PACKET_HEADER)(lpdata);
	char * lpuserdata = lpdata + sizeof(COMMUNICATION_PACKET_HEADER);
	int userdatasize = len - sizeof(COMMUNICATION_PACKET_HEADER);

	iRet = MakeSureDirectoryPathExists(filename);

	HANDLE hfile = CreateFileA(filename,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	if (hfile != INVALID_HANDLE_VALUE)
	{
		DWORD dwcnt = 0;
		iRet = WriteFile(hfile, lpuserdata,userdatasize,&dwcnt,0);

		int totalrecvsize = len;
		int totaldatasize = hdr.dwsize;
		int recvlen = 0;

		while ((recvlen = recv(stparam.sockclient, lpdata, NETWORK_DATABUF_SIZE, 0)) > 0) {
			iRet = WriteFile(hfile, lpdata, recvlen, &dwcnt, 0);
			totalrecvsize += recvlen;
		}

		CloseHandle(hfile);

		//iRet = DecompressFromFileBlock(filename);

		iRet = DecryptAndDecompFile(lpparam, filename, lpdstfn, imei, username, withparam, append,hdr.reservedname);
		return TRUE;
	}
	else {
		wsprintfA(szlog, "RecvNetworkLargeFile open file error:%s", filename);
		ErrorFormat(&stparam, errorbuf, szlog,imei,username);
		WriteLogFile(errorbuf);
	}

	return FALSE;
}








int localFileSender(char * lpbuf,int cmd,const char * localfn,char * gbkparamfn,LPDATAPROCESS_PARAM lpparam,char * username,char * imei)
{

	char userfn[MAX_MESSAGE_SIZE] = { 0 };
	int iRet = Coder::GBKFNToUTF8FN(gbkparamfn, userfn, MAX_MESSAGE_SIZE);

	char errorbuf[1024];

	HANDLE hFileDownLoadLocal = CreateFileA(localfn,GENERIC_READ | GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if (hFileDownLoadLocal != INVALID_HANDLE_VALUE )
	{
		int dwFileSize = GetFileSize(hFileDownLoadLocal,0);
		if (dwFileSize <= 0)
		{
			return FALSE;
		}

		char lpDownloadSendBuf[MAX_MESSAGE_SIZE];
		int iLen = PacketMaker::hdrWithNameAndSize(lpDownloadSendBuf, cmd, imei, userfn, dwFileSize);

		int iRet =	send(lpparam->sockclient,lpDownloadSendBuf,iLen,0);
		if (iRet <= 0)
		{
			CloseHandle(hFileDownLoadLocal);
			ErrorFormat(lpparam,errorbuf,"downloadfile send first packet error",imei,username);
			WriteLogFile(errorbuf);
			return NETWORKERROR;
		}

		int sendtimes = dwFileSize / NETWORK_DATABUF_SIZE;
		int sendmod = dwFileSize % NETWORK_DATABUF_SIZE;
		DWORD dwCnt = 0;
		for (int i =0; i < sendtimes; i++)
		{
			iRet = ReadFile(hFileDownLoadLocal,lpbuf,NETWORK_DATABUF_SIZE,&dwCnt,0);
			if (iRet <= 0 || dwCnt != NETWORK_DATABUF_SIZE)
			{
				CloseHandle(hFileDownLoadLocal);
				ErrorFormat(lpparam,errorbuf,"downloadfile read file error",imei,username);
				WriteLogFile(errorbuf);
				return NETWORKERROR;
			}
			iRet = send(lpparam->sockclient,lpbuf,NETWORK_DATABUF_SIZE,0);
			if (iRet <= 0)
			{
				CloseHandle(hFileDownLoadLocal);
				ErrorFormat(lpparam,errorbuf,"downloadfile send file error",imei,username);
				WriteLogFile(errorbuf);
				return NETWORKERROR;
			}
		}

		if (sendmod)
		{
			iRet = ReadFile(hFileDownLoadLocal,lpbuf,sendmod,&dwCnt,0);
			if (iRet <= 0 || dwCnt != sendmod)
			{
				CloseHandle(hFileDownLoadLocal);
				ErrorFormat(lpparam,errorbuf,"downloadfile read file mod error",imei,username);
				WriteLogFile(errorbuf);
				return NETWORKERROR;
			}
			iRet = send(lpparam->sockclient,lpbuf,sendmod,0);
			if (iRet <= 0)
			{
				CloseHandle(hFileDownLoadLocal);
				ErrorFormat(lpparam,errorbuf,"downloadfile send file mode error",imei,username);
				WriteLogFile(errorbuf);
				return NETWORKERROR;
			}
		}

		CloseHandle(hFileDownLoadLocal);

		ErrorFormat(lpparam,errorbuf,"downloadfile send file ok",imei,username);
		WriteLogFile(errorbuf);
		
		return TRUE;
	}
	else
	{
		ErrorFormat(lpparam,errorbuf,"downloadfile open local file error",imei,username);
		WriteLogFile(errorbuf);
	}

	return FALSE;
}