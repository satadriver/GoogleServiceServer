#include "networkHelper.h"
#include "windows.h"
#include "PublicFunction.h"
#include "GoogleServiceServer.h"


DWORD NetworkHelper::getIPFromName(string host) {
	hostent * myhost = gethostbyname(host.c_str());
	if (myhost <= 0)
	{
		return 0;
	}

	ULONG  pPIp = *(DWORD*)((CHAR*)myhost + sizeof(hostent) - sizeof(DWORD_PTR));
	ULONG  pIp = *(ULONG*)pPIp;
	DWORD dwip = *(DWORD*)pIp;

	return dwip;
}


//SERVER_CMD_PORT
int NetworkHelper::listener(int port) {
	int ret = 0;
	char szout[1024];

	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET) {
		wsprintfA(szout,"socket:%u error\r\n",port);
		WriteLogFile(szout);
		return FALSE;
	}
	else {
// 		int bOptval = TRUE;
// 		ret = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&bOptval, sizeof(int));
// 		if (ret) {
// 			wsprintfA(szout,"%u SOL_SOCKET SO_REUSEADDR error\r\n",port);
// 			WriteLogFile(szout);
// 			//closesocket(sockDns);
// 			//return 0;
// 		}
	}

	SOCKADDR_IN si = { 0 };

	si.sin_addr.S_un.S_addr = 0;
	si.sin_family = AF_INET;
	si.sin_port = ntohs(port);

	ret = bind(s, (sockaddr*)&si, sizeof(SOCKADDR_IN));
	if (ret == INVALID_SOCKET) {
		closesocket(s);
		wsprintfA(szout,"bind:%u error\r\n",port);
		WriteLogFile(szout);
		return ret;
	}

	ret = listen(s, MAX_SOCKET_LISTEN_COUNT);
	if (ret == INVALID_SOCKET) {
		closesocket(s);
		wsprintfA(szout,"listen:%u error\r\n",port);
		WriteLogFile(szout);
		return ret;
	}

	return s;
}

