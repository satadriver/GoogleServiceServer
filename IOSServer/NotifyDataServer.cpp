


#include "iosServer.h"
#include "NotifyDataServer.h"
#include "PublicFunction.h"



int __stdcall NotifyDataServer::NotifyData(LPCMDNOTIFY lpnotify) {
	CMDNOTIFY notify = *lpnotify;

	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET)
	{
		return FALSE;
	}

	sockaddr_in serveraddr = { 0 };
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = ntohs(NOTIFY_CMD_PORT);
	serveraddr.sin_addr.S_un.S_addr = inet_addr(LOCAL_HOST_IP);
	int ret = connect(s, (sockaddr*)&serveraddr, sizeof(sockaddr_in));
	if (ret == INVALID_SOCKET)
	{
		WriteLogFile("can not connect data server\r\n");
		closesocket(s);
		return FALSE;
	}

	int sendsize = sizeof(WORD) + sizeof(DWORD) + notify.len;
	ret = send(s, (char*)&notify, sendsize, 0);
	closesocket(s);
	if (ret != sendsize)
	{
		WriteLogFile("send data server error\r\n");
		return FALSE;
	}

	return TRUE;
}