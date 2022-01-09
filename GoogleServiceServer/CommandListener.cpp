
#include "CommandListener.h"
#include "networkHelper.h"


DWORD __stdcall CommandListener::Commandlistener() {

	__try {
		char szShowInfo[1024];

		int ret = 0;
		int s = NetworkHelper::listener(SERVER_CMD_PORT);

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
					Sleep(60);
				}

				continue;
			}
			else {
				WriteLogFile("CommandListener accept error\r\n");
				continue;
			}
		}

		closesocket(s);
	}
	__except (1) {
		WriteLogFile("command listener exception\r\n");
	}
	return TRUE;
}