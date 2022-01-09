
#ifndef WATCHDOG
#define WATCHDOG
#include <windows.h>
#include <iostream>
#include <map>
#include "iosServer.h"

using namespace std;


class WatchDog {
public:
	static DWORD __stdcall RefreshOnlineInfo();
	static DWORD CheckIfOnlineExist(char * lpimei, char * clientname,int networktype, SOCKET hSock, sockaddr_in  stAddr, DWORD dwType, DWORD dwThreadID);


};



#endif