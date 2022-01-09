

#include "GoogleServiceServer.h"
#include "PublicFunction.h"
#include <time.h>
#include "FileOperator.h"
#include "WatchDog.h"
#include <iostream>

#include <string>

using namespace std;

static map <string, ONLINEINFO> ClientOnlineMap;

CRITICAL_SECTION g_OnlineCS = { 0 };


string formattime() {
	char szTime[MAX_PATH] = { 0 };
	SYSTEMTIME stTime = { 0 };
	GetLocalTime(&stTime);
	wsprintfA(szTime, "%u-%u-%u %d:%d:%d",
		stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
	return szTime;
}


DWORD __stdcall WatchDog::RefreshOnlineInfo(){

	InitializeCriticalSection(&g_OnlineCS);
	
	char szShowInfo[1024];

	int ret = 0;

	while (TRUE)
	{

		//DeleteFileA(CLIENT_ONLINE_FILE_NAME);
		ret = FileOperator::fileWriter(CLIENT_ONLINE_FILE_NAME, "", 0, 1);

		EnterCriticalSection(&g_OnlineCS);

		__try {
			time_t tmNow = time(0);
			
			map <string, ONLINEINFO> ::iterator maponlineit;

			for (maponlineit = ClientOnlineMap.begin(); maponlineit != ClientOnlineMap.end(); ) 
			{
				if ( (tmNow - maponlineit->second.tmLastOnlineTime <= CLIENT_COMMAND_TIME_INTERVAL * 9)  )
				{

					string strip = getipstr(maponlineit->second.stAddr.sin_addr.S_un.S_addr);

					DWORD dwPort = ntohs(maponlineit->second.stAddr.sin_port);

					string strtimenow = formattime();

					string imei = string(maponlineit->second.IMEI, IMEI_IMSI_PHONE_SIZE);

					string client = string(maponlineit->second.ClientName, IMEI_IMSI_PHONE_SIZE);

					int iLen = wsprintfA(szShowInfo, 
						"Client:%s IMEI:%s network:%u IP:%s PORT:%u socket:%u thread id:%u type:%u time:%s current time_t:%I64d record time_t:%I64d online\r\n",
						client.c_str(), imei.c_str(), maponlineit->second.networktype, strip.c_str(), dwPort,
						maponlineit->second.hSock, maponlineit->second.dwThreadID, maponlineit->second.dwType, strtimenow.c_str(),
						tmNow, maponlineit->second.tmLastOnlineTime);

					ret = FileOperator::fileWriter(CLIENT_ONLINE_FILE_NAME, szShowInfo, iLen);

					maponlineit++;
				}
				else if ( (tmNow - maponlineit->second.tmLastOnlineTime > CLIENT_COMMAND_TIME_INTERVAL * 9) )
				{
// 					HANDLE hThread = OpenThread(THREAD_ALL_ACCESS,0,maponlineit->second.dwThreadID);
// 					if (hThread)
// 					{
// 						wsprintfA(szShowInfo, "thread id:%u is already alive error,maybe working hard\r\n", maponlineit->second.dwThreadID);
// 						WriteLogFile(szShowInfo);
// 						//TerminateThread(hThread,0);
// 						CloseHandle(hThread);
// 
// 						maponlineit++;
// 					}

					ClientOnlineMap.erase(maponlineit++);
				}
			}
		}
		__except (1) {
			WriteLogFile("RefreshOnlineInfo exception\r\n");
		}

		LeaveCriticalSection(&g_OnlineCS);

		Sleep(CLIENT_COMMAND_TIME_INTERVAL * 1000);
	}
	
	return TRUE;
}



DWORD WatchDog::CheckIfOnlineExist(char * lpimei,char * clientname,int networktype,SOCKET hSock,sockaddr_in  stAddr,DWORD dwType,DWORD dwThreadID){

	EnterCriticalSection(&g_OnlineCS);

	map <string, ONLINEINFO> ::iterator maponlineit;

	maponlineit = ClientOnlineMap.find(string(lpimei));
	if (maponlineit != ClientOnlineMap.end())
	{
		maponlineit->second.networktype = networktype;
		maponlineit->second.tmLastOnlineTime = time(0);
		maponlineit->second.dwThreadID = dwThreadID;
		maponlineit->second.dwType = dwType;
		maponlineit->second.hSock = hSock;
		maponlineit->second.stAddr = stAddr;
		memcpy(maponlineit->second.IMEI,lpimei, lstrlenA(lpimei));
		memcpy(maponlineit->second.ClientName, clientname, lstrlenA(clientname));
	}
	else{
		ONLINEINFO stOnlineInfo = {0};
		memcpy(stOnlineInfo.IMEI,lpimei,lstrlenA(lpimei));
		memcpy(stOnlineInfo.ClientName, clientname,lstrlenA(clientname));
		stOnlineInfo.networktype = networktype;
		stOnlineInfo.hSock = hSock;
		stOnlineInfo.stAddr = stAddr;
		stOnlineInfo.tmLastOnlineTime = time(0);
		stOnlineInfo.dwType = dwType;
		stOnlineInfo.dwThreadID = dwThreadID;

		ClientOnlineMap.insert(map<string,ONLINEINFO>::value_type(string(lpimei),stOnlineInfo));
	}

	LeaveCriticalSection(&g_OnlineCS);
	return TRUE;
}