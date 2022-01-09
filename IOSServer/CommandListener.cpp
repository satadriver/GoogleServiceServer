
#include "CommandListener.h"


DWORD __stdcall CommandListener::Commandlistener() {
	char szShowInfo[1024];
	int ret = 0;

	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET) {
		WriteLogFile("CommandListener socket error\r\n");
		return FALSE;
	}
	else {
// 		int bOptval = TRUE;
// 		ret = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&bOptval, sizeof(int));
// 		if (ret) {
// 			WriteLogFile("Commandlistener setsockopt error\r\n");
// 		}
	}

	SOCKADDR_IN si = { 0 };
	si.sin_addr.S_un.S_addr = INADDR_ANY;
	si.sin_family = AF_INET;
	si.sin_port = ntohs(SERVER_CMD_PORT);
	ret = bind(s, (sockaddr*)&si, sizeof(SOCKADDR_IN));
	if (ret == INVALID_SOCKET) {
		WriteLogFile("CommandListener bind error\r\n");
		closesocket(s);
		return ret;
	}

	ret = listen(s, MAX_SOCKET_LISTEN_COUNT);
	if (ret == INVALID_SOCKET) {
		WriteLogFile("CommandListener listen error\r\n");
		closesocket(s);
		return ret;
	}

	SOCKADDR_IN siclient = { 0 };
	while (TRUE) {
		int addrlen = sizeof(SOCKADDR_IN);
		SOCKET sockclient = accept(s, (sockaddr*)&siclient, &addrlen);
		if (sockclient != INVALID_SOCKET)
		{
			DWORD dwSockTimeOut = CMDSOCK_TIME_OUT_VALUE;
			if (setsockopt(sockclient, SOL_SOCKET, SO_RCVTIMEO, (char*)&dwSockTimeOut, sizeof(DWORD)) == SOCKET_ERROR)
			{
				closesocket(sockclient);
				wsprintfA(szShowInfo, "CommandListener recv time setsockopt error code:%u\r\n", GetLastError());
				WriteLogFile(szShowInfo);
				continue;
			}

			dwSockTimeOut = CMDSOCK_TIME_OUT_VALUE;
			if (setsockopt(sockclient, SOL_SOCKET, SO_SNDTIMEO, (char*)&dwSockTimeOut, sizeof(DWORD)) == SOCKET_ERROR)
			{
				closesocket(sockclient);
				wsprintfA(szShowInfo, "CommandListener send time setsockopt error code:%u\r\n", GetLastError());
				continue;
			}

			DATAPROCESS_PARAM stparam = { 0 };
			stparam.si = siclient;
			stparam.sockclient = sockclient;
			ret = GetCurrentDirectoryA(MAX_PATH, stparam.currentpath);
			if (ret) {
				HANDLE h = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)CommandProcess::CommandProcessor, &stparam, 0, &stparam.threadid);
				CloseHandle(h);
				Sleep(0);
			}

			continue;
		}
		else {
			WriteLogFile("CommandListener accept error\r\n");
			continue;
		}
	}

	closesocket(s);
	return TRUE;
}