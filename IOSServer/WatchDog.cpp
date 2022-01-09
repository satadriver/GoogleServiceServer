

#include "iosServer.h"
#include "PublicFunction.h"
#include <time.h>

#include "WatchDog.h"

#include <string>

using namespace std;

static map <string, ONLINEINFO> gClientOnlineMap;

CRITICAL_SECTION g_iosMap;

#define ONLINE_CIRCLE_CNT 15

DWORD __stdcall WatchDog::RefreshOnlineInfo(){
	
	InitializeCriticalSection(&g_iosMap);

	char szShowInfo[1024];
	while (TRUE)
	{

		HANDLE hOnline = CreateFileA(CLIENT_ONLINE_FILE_NAME, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 
			0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (hOnline == INVALID_HANDLE_VALUE)
		{
			Sleep(CLIENT_COMMAND_TIME_INTERVAL * 1000);
			continue;
		}

		HANDLE hThreadInfo = CreateFileA(THREAD_INFO_FILE_NAME, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 
			0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (hThreadInfo == INVALID_HANDLE_VALUE)
		{
			CloseHandle(hOnline);
			Sleep(CLIENT_COMMAND_TIME_INTERVAL * 1000);
			continue;
		}

		EnterCriticalSection(&g_iosMap);

		__try {
			time_t tmNow = time(0);
			DWORD dwNumber = 0;
			map <string, ONLINEINFO> ::iterator maponlineit;
			for(maponlineit = gClientOnlineMap.begin(); maponlineit != gClientOnlineMap.end(); ){
				if ( (tmNow - maponlineit->second.tmLastOnlineTime <= CLIENT_COMMAND_TIME_INTERVAL * ONLINE_CIRCLE_CNT) && 
					(tmNow >= maponlineit->second.tmLastOnlineTime) )
				{
					char szIP[64];
					DWORD dwLocalIP = maponlineit->second.stAddr.sin_addr.S_un.S_addr;
					unsigned char * pIP = (unsigned char*)&dwLocalIP;
					wsprintfA(szIP,"%u.%u.%u.%u",pIP[0],pIP[1],pIP[2],pIP[3]);

					DWORD dwPort = ntohs(maponlineit->second.stAddr.sin_port);

					char szTime[MAX_PATH];
					SYSTEMTIME stTime = {0};
					GetLocalTime(&stTime);
					wsprintfA(szTime,"%u-%u-%u %d:%d:%d",stTime.wYear,stTime.wMonth,stTime.wDay,stTime.wHour,stTime.wMinute,stTime.wSecond);

					string imei = string(maponlineit->second.iphoneID, IOS_IDENTIFIER_SIZE);
					string client = string(maponlineit->second.ClientName, USER_NAME_SIZE);
					int iLen = wsprintfA(szShowInfo,
						"{\"username\":\"%s\","
						"\"ID\":\"%s\","
						"\"networktype\":%u,"
						"\"IP\":\"%s\","
						"\"PORT\":%u,"
						"\"socket\":%u,"
						"\"thread id\":%u,"
						"\"type\":%u,"
						"\"time\":\"%s\","
						"\"current time_t\":%I64d,"
						"\"record time_t\":%I64d}\r\n",
						client.c_str(),imei.c_str(),maponlineit->second.networktype,szIP,dwPort,
						maponlineit->second.hSock,maponlineit->second.dwThreadID,maponlineit->second.dwType,szTime,
						tmNow,maponlineit->second.tmLastOnlineTime);
					DWORD dwCnt = 0;
					int iRet = WriteFile(hOnline,szShowInfo,iLen,&dwCnt,0);

					maponlineit ++;
					continue;
				}
				else if ((tmNow - maponlineit->second.tmLastOnlineTime > CLIENT_COMMAND_TIME_INTERVAL * ONLINE_CIRCLE_CNT) && 
					(tmNow > maponlineit->second.tmLastOnlineTime) )
				{	
					HANDLE hThread = OpenThread(THREAD_ALL_ACCESS,0,maponlineit->second.dwThreadID);
					if (hThread)
					{
						CloseHandle(hThread);
					}

					gClientOnlineMap.erase(maponlineit++);
					continue;
				}
				else{
					char szIP[64];
					DWORD dwIPAddr = maponlineit->second.stAddr.sin_addr.S_un.S_addr;
					unsigned char * pIP = (unsigned char*)&dwIPAddr;
					wsprintfA(szIP,"%u.%u.%u.%u",pIP[0],pIP[1],pIP[2],pIP[3]);

					DWORD dwPort = ntohs(maponlineit->second.stAddr.sin_port);

					SYSTEMTIME stTime = {0};
					GetLocalTime(&stTime);
					char szTime[MAX_PATH];
					wsprintfA(szTime,"%u-%u-%u %d:%d:%d",stTime.wYear,stTime.wMonth,stTime.wDay,stTime.wHour,stTime.wMinute,stTime.wSecond);

					string imei = string(maponlineit->second.iphoneID, IOS_IDENTIFIER_SIZE);
					string client = string(maponlineit->second.ClientName, USER_NAME_SIZE);

					int iLen = wsprintfA(szShowInfo,
						"NO:%u\r\nIMEI:%s\r\nusername:%s\r\nIP:%s\r\nPORT:%u\r\ntime:%s\r\ncurrent time_t:%I64d record time_t:%I64d\r\nsocket:%u\r\nthread type:%u\r\nthread id:%u\r\n\r\n",
						dwNumber, imei.c_str(), client.c_str(),
						szIP,dwPort,szTime,tmNow,maponlineit->second.tmLastOnlineTime,maponlineit->second.hSock,
						maponlineit->second.dwType,maponlineit->second.dwThreadID);
					DWORD dwCnt = 0;
					int iRet = WriteFile(hThreadInfo,szShowInfo,iLen,&dwCnt,0);

					dwNumber ++;

					gClientOnlineMap.erase(maponlineit++);
					continue;
				}
			}
		}
		__except (1) {
			WriteLogFile("RefreshOnlineInfo exception\r\n");
		}

		LeaveCriticalSection(&g_iosMap);

		CloseHandle(hOnline);

		CloseHandle(hThreadInfo);

		Sleep(CLIENT_COMMAND_TIME_INTERVAL * 1000);
	}

	return TRUE;
}



DWORD WatchDog::CheckIfOnlineExist(char * lpimei,char * clientname,int networktype,SOCKET hSock,sockaddr_in  stAddr,DWORD dwType,DWORD dwThreadID)
{
	EnterCriticalSection(&g_iosMap);

	map <string, ONLINEINFO> ::iterator maponlineit;
	maponlineit = gClientOnlineMap.find(string(lpimei));
	if (maponlineit != gClientOnlineMap.end())
	{
		maponlineit->second.networktype = networktype;
		maponlineit->second.tmLastOnlineTime = time(0);
		maponlineit->second.dwThreadID = dwThreadID;
		maponlineit->second.dwType = dwType;
		maponlineit->second.hSock = hSock;
		maponlineit->second.stAddr = stAddr;
		lstrcpyA(maponlineit->second.iphoneID,lpimei);
		lstrcpyA(maponlineit->second.ClientName, clientname);
	}
	else{
		ONLINEINFO stOnlineInfo = {0};
		stOnlineInfo.networktype = networktype;
		stOnlineInfo.hSock = hSock;
		stOnlineInfo.stAddr = stAddr;
		stOnlineInfo.tmLastOnlineTime = time(0);
		stOnlineInfo.dwType = dwType;
		stOnlineInfo.dwThreadID = dwThreadID;
		lstrcpyA(stOnlineInfo.ClientName, clientname);
		lstrcpyA(stOnlineInfo.iphoneID, lpimei);
		gClientOnlineMap.insert(map<string,ONLINEINFO>::value_type(string(lpimei),stOnlineInfo));
	}

	LeaveCriticalSection(&g_iosMap);
	return TRUE;
}