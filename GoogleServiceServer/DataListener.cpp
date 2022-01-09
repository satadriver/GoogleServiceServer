
#include <windows.h>
#include "GoogleServiceServer.h"
#include "PublicFunction.h"
#include "DataProcess.h"
#include "DataListener.h"
#include "networkHelper.h"



DWORD __stdcall DataListener::DataProcessListener() {
	__try{
		char szShowInfo[1024];
		int ret = 0;
		int s = NetworkHelper::listener(SERVER_DATA_PORT);
		if (s <= 0)
		{
			wsprintfA(szShowInfo,"listen port:%u error\r\n", SERVER_DATA_PORT);
			WriteLogFile(szShowInfo);
			ExitProcess(0);
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

				int bufLimit = LIMIT_DATA_SIZE;
				if (setsockopt(sockclient, SOL_SOCKET, SO_RCVBUF, (char*)&bufLimit, sizeof(DWORD)) == SOCKET_ERROR)
				{
					closesocket(sockclient);
					wsprintfA(szShowInfo, "DataProcessListener recv buffer size setsockopt error code:%u\r\n", GetLastError());
					continue;
				}

				if (setsockopt(sockclient, SOL_SOCKET, SO_SNDBUF, (char*)&bufLimit, sizeof(DWORD)) == SOCKET_ERROR)
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
					Sleep(50);
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
	}
	__except (1) {
		WriteLogFile("data listener exception\r\n");
	}
	return TRUE;
}