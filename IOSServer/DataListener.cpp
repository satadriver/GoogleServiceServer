
#include <windows.h>
#include "iosServer.h"
#include "PublicFunction.h"
#include "DataProcess.h"
#include "DataListener.h"




DWORD __stdcall DataListener::DataProcessListener() {
	char szShowInfo[1024];
	int ret = 0;

	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET) {
		WriteLogFile("DataProcessListener socket error\r\n");
		return FALSE;
	}
	else {
// 		int bOptval = TRUE;
// 		ret = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&bOptval, sizeof(int));
// 		if (ret) {
// 			WriteLogFile("DataProcessListener setsockopt error\r\n");
// 		}
	}

	SOCKADDR_IN si = { 0 };
	si.sin_addr.S_un.S_addr = INADDR_ANY;
	si.sin_family = AF_INET;
	si.sin_port = ntohs(SERVER_DATA_PORT);
	ret = bind(s, (sockaddr*)&si, sizeof(SOCKADDR_IN));
	if (ret == INVALID_SOCKET) {
		closesocket(s);
		WriteLogFile("DataProcessListener bind error\r\n");
		return ret;
	}

	ret = listen(s, MAX_SOCKET_LISTEN_COUNT);
	if (ret == INVALID_SOCKET) {
		closesocket(s);
		WriteLogFile("DataProcessListener listen error\r\n");
		return ret;
	}

	SOCKADDR_IN siclient = { 0 };
	while (TRUE) {
		int addrlen = sizeof(SOCKADDR_IN);
		SOCKET sockclient = accept(s, (sockaddr*)&siclient, &addrlen);
		if (sockclient != INVALID_SOCKET)
		{
			DWORD dwSockTimeOut = DATASOCK_TIME_OUT_VALUE;
			if (setsockopt(sockclient, SOL_SOCKET, SO_RCVTIMEO, (char*)&dwSockTimeOut, sizeof(DWORD)) == SOCKET_ERROR)
			{
				closesocket(sockclient);
				wsprintfA(szShowInfo, "DataProcessListener recv time limit setsockopt error code:%u\r\n", GetLastError());
				WriteLogFile(szShowInfo);
				continue;
			}

			dwSockTimeOut = DATASOCK_TIME_OUT_VALUE;
			if (setsockopt(sockclient, SOL_SOCKET, SO_SNDTIMEO, (char*)&dwSockTimeOut, sizeof(DWORD)) == SOCKET_ERROR)
			{
				closesocket(sockclient);
				wsprintfA(szShowInfo, "DataProcessListener send time limit setsockopt error code:%u\r\n", GetLastError());
				continue;
			}

			dwSockTimeOut = NETWORK_DATABUF_SIZE;
			if (setsockopt(sockclient, SOL_SOCKET, SO_RCVBUF, (char*)&dwSockTimeOut, sizeof(DWORD)) == SOCKET_ERROR)
			{
				closesocket(sockclient);
				wsprintfA(szShowInfo, "DataProcessListener recv buffer size setsockopt error code:%u\r\n", GetLastError());
				continue;
			}
			dwSockTimeOut = NETWORK_DATABUF_SIZE;
			if (setsockopt(sockclient, SOL_SOCKET, SO_SNDBUF, (char*)&dwSockTimeOut, sizeof(DWORD)) == SOCKET_ERROR)
			{
				closesocket(sockclient);
				wsprintfA(szShowInfo, "DataProcessListener send buffer size setsockopt error code:%u\r\n", GetLastError());
				continue;
			}

			DATAPROCESS_PARAM stparam = { 0 };
			stparam.si = siclient;
			stparam.sockclient = sockclient;
			ret = GetCurrentDirectoryA(MAX_PATH, stparam.currentpath);
			if (ret) {
				HANDLE h = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DataProcess::DataProcessor, &stparam, 0, &stparam.threadid);
				CloseHandle(h);
			}

			continue;
		}
		else {
			wsprintfA(szShowInfo, "DataProcessListener accept error,socket:%u,error:%u\r\n", sockclient, GetLastError());
			WriteLogFile(szShowInfo);
			continue;
		}
	}

	closesocket(s);
	return TRUE;
}