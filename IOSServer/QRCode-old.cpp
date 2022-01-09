
/*
#include <windows.h>
#include "PublicFunction.h"
#include "DataProcess.h"
#include "QRCode.h"




DWORD __stdcall QRCode::QRCodeListener(){
	char szShowInfo[1024];
	SOCKET s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (s == INVALID_SOCKET){
		WriteLogFile("QRCodeListener socket error\r\n");
		return FALSE;
	}

	SOCKADDR_IN si = {0};
	//si.sin_addr.S_un.S_addr = inet_addr(SERVER_IP_ADDRESS);
	si.sin_addr.S_un.S_addr = SERVER_IP_ADDRESS;
	si.sin_family = AF_INET;
	si.sin_port = ntohs(SERVER_QRCODE_PORT);

	int ret = bind(s,(sockaddr*)&si,sizeof(SOCKADDR_IN));
	if (ret == INVALID_SOCKET){
		WriteLogFile("QRCodeListener bind error\r\n");
		closesocket(s);
		return ret;
	}

	ret = listen(s,MAX_LISTEN_COUNT);
	if (ret == INVALID_SOCKET){
		WriteLogFile("QRCodeListener listen error\r\n");
		closesocket(s);
		return ret;
	}

	SOCKADDR_IN siclient = {0};
	while(TRUE){
		int addrlen = sizeof(SOCKADDR_IN);

		SOCKET sockclient = accept(s,(sockaddr*)&siclient,&addrlen);
		if (sockclient != INVALID_SOCKET)
		{
			DWORD dwSockTimeOut = DATASOCK_TIME_OUT_VALUE;
			if (setsockopt(sockclient,SOL_SOCKET,SO_RCVTIMEO,(char*)&dwSockTimeOut,sizeof(DWORD))==SOCKET_ERROR)
			{
				wsprintfA(szShowInfo,"QRCodeListener recv time setsockopt error code:%u\r\n",GetLastError());
				WriteLogFile(szShowInfo);
				continue;
			}

			dwSockTimeOut = DATASOCK_TIME_OUT_VALUE;
			if (setsockopt(sockclient,SOL_SOCKET,SO_SNDTIMEO,(char*)&dwSockTimeOut,sizeof(DWORD))==SOCKET_ERROR)
			{
				
				wsprintfA(szShowInfo,"QRCodeListener send time setsockopt error code:%u\r\n",GetLastError());
				continue;
			}
			//WriteLogFile("QRCodeListener accept ok\r\n");
			DATAPROCESS_PARAM stparam = {0};
			stparam.si = siclient;
			stparam.sockclient = sockclient;
			ret = GetCurrentDirectoryA(MAX_PATH,stparam.currentpath);
			if(ret){
				HANDLE h = CreateThread(0,0,(LPTHREAD_START_ROUTINE)QRCodeProcess,&stparam,0,0);
				CloseHandle(h);
			}
			continue;
		}
		else{
			WriteLogFile("QRCodeListener accept error\r\n");
			continue;
		}
	}
	return TRUE;
}


#define QRCodeProcessException 4


DWORD __stdcall QRCode::QRCodeProcess(LPDATAPROCESS_PARAM lpparam){
	DATAPROCESS_PARAM stparam = * lpparam;
	char szLog[1024];

	try{
		//throw QRCodeProcessException;

		char lpdata[NETWORK_DATABUF_SIZE];
		int dwPackSize = recv(stparam.sockclient,lpdata,NETWORK_DATABUF_SIZE,0);
		if (dwPackSize <= 0){
			
			ErrorFormat(&stparam,szLog,"QRCodeProcess first packet error");
			WriteLogFile(szLog);
			closesocket(stparam.sockclient);
			return FALSE;
		}

		int size = 0;
		char szgetformat[] = "GET /%s";
		char szapkico[] = "GET /favicon.ico";
		char szpictureurl[MAX_PATH];
		wsprintfA(szpictureurl,szgetformat,QRCODE_FILENAME);

		char szhttpechohdrText[] = 
			"HTTP/1.1 200 OK\r\n"
			"Content-type: text/html;charset=utf-8\r\n"
			"Content-Language: zh-CN;en-US\r\n"
			"Connection: keep-alive\r\n"
			"Content-Encoding: gzip/deflate\r\n"
			"Content-Length: %u\r\n\r\n";

		char szhttpechohdrPhoto[] = 
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: image/jpeg\r\n"
			"Content-Language: zh-CN;en-US\r\n"
			"Connection: keep-alive\r\n"
			"Content-Encoding: gzip/deflate\r\n"
			"Content-Length: %u\r\n\r\n";

		if (memcmp(lpdata,"GET / ",6) == 0)
		{
			char szhtmlFormat[] = 
				"<html>\r\n"
				"<head>\r\n"
				"<title>%s</title>\r\n"
				"</head>\r\n"
				"<body>\r\n"				
				"<p><b><font size=\"%s\" color=\"%s\">%s</font></b></p>\r\n"
				"<img src=\"%s\" height=\"320\" width=\"320\"/>"
				"</body>\r\n"
				"</html>\r\n";

			char szBodyFontSize[] = "12";
			char szBodyColor[] = "black";
			char szHtmlTitle[] = "Google Service Download From QRCode";
			char szHtmlBody[] = "Google Service";
			char szhtml[0x1000];
			int htmllen = wsprintfA(szhtml,szhtmlFormat,szHtmlTitle,szBodyFontSize,szBodyColor,szHtmlBody,QRCODE_FILENAME);

			char szsendbuf[0x10000];
			int sendlen = wsprintfA(szsendbuf,szhttpechohdrText,htmllen);
			memmove(szsendbuf + sendlen, szhtml,htmllen);
			sendlen += htmllen;

			size = send(stparam.sockclient,szsendbuf,sendlen,0);
			closesocket(stparam.sockclient);
			if (size <= 0)
			{
				ErrorFormat(&stparam,szLog,"QRCodeProcess send html error");
				WriteLogFile(szLog);
				return FALSE;
			}
			else{
				return TRUE;
			}
		}
		else if (memcmp(lpdata,szpictureurl,lstrlenA(szpictureurl)) == 0 )
		{
			HANDLE hFile = CreateFileA(QRCODE_FILENAME,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
			if(hFile == INVALID_HANDLE_VALUE){
				WriteLogFile("QRCodeProcess not found qrcode file in local folder\r\n");
				closesocket(stparam.sockclient);
				return FALSE;
			}

			DWORD dwFileSize = GetFileSize(hFile,0);
			char * szsendbuf = new char[dwFileSize + 0x1000];
			int sendlen = wsprintfA(szsendbuf,szhttpechohdrPhoto,dwFileSize);

			DWORD dwCnt = 0;
			int iRet = ReadFile(hFile,szsendbuf + sendlen, dwFileSize,&dwCnt,0);
			CloseHandle(hFile);
			if (iRet == FALSE || dwCnt != dwFileSize){
				closesocket(stparam.sockclient);
				delete [] szsendbuf;
				return FALSE;
			}

			sendlen += dwFileSize;
			size = send(stparam.sockclient,szsendbuf,sendlen,0);
			closesocket(stparam.sockclient);
			delete [] szsendbuf;
			if (size <= 0)
			{
				return FALSE;
			}

			SYSTEMTIME sttime = { 0 };
			GetLocalTime(&sttime);
			unsigned char ucip[4];
			memmove(ucip, &stparam.si.sin_addr.S_un.S_addr, 4);
			wsprintfA(szLog, "load qrcode,client ip:%u_%u_%u_%u,port:%u,time:%u/%u/%u %u:%u:%u\r\n", ucip[0], ucip[1], ucip[2], ucip[3],stparam.si.sin_port,
				sttime.wYear, sttime.wMonth,sttime.wDay, sttime.wHour, sttime.wMinute, sttime.wSecond);
			WriteLogFile(szLog);
			return TRUE;
		}
		else if (memcmp(lpdata,szapkico,lstrlenA(szapkico)) == 0)
		{
			HANDLE hFile = CreateFileA(APK_ICO_FILENAME,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
			if(hFile == INVALID_HANDLE_VALUE){
				WriteLogFile("QRCodeProcess not found ico file in local folder\r\n");
				closesocket(stparam.sockclient);
				return FALSE;
			}

			DWORD dwFileSize = GetFileSize(hFile,0);
			char * szsendbuf = new char[dwFileSize + 0x1000];
			int sendlen = wsprintfA(szsendbuf,szhttpechohdrPhoto,dwFileSize);

			DWORD dwCnt = 0;
			int iRet = ReadFile(hFile,szsendbuf + sendlen, dwFileSize,&dwCnt,0);
			CloseHandle(hFile);
			if (iRet == FALSE || dwCnt != dwFileSize){
				closesocket(stparam.sockclient);
				delete [] szsendbuf;
				return FALSE;
			}

			sendlen += dwFileSize;
			size = send(stparam.sockclient,szsendbuf,sendlen,0);
			closesocket(stparam.sockclient);
			delete [] szsendbuf;
			if (size <= 0)
			{
				return FALSE;
			}
			return TRUE;
		}
		else if (memcmp(lpdata,"HEAD ",5) == 0)
		{
			char szheadurl[MAX_PATH];
			wsprintfA(szheadurl,"HEAD /%s",QRCODE_FILENAME);

			HANDLE hFile = CreateFileA(QRCODE_FILENAME,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
			if(hFile == INVALID_HANDLE_VALUE){
				WriteLogFile("QRCodeProcess HEAD not found qrcode file in local folder\r\n");
				closesocket(stparam.sockclient);
				return FALSE;
			}
			DWORD dwFileSize = GetFileSize(hFile,0);
			CloseHandle(hFile);

			char szsendbuf[0x1000];
			int sendlen = wsprintfA(szsendbuf,szhttpechohdrText,dwFileSize);

			size = send(stparam.sockclient,szsendbuf,sendlen,0);
			closesocket(stparam.sockclient);
			if (size <= 0)
			{
				return FALSE;
			}
			return TRUE;
		}
		else{
			WriteLogFile("QRCodeProcess recv unrecognized packet error\r\n");
			closesocket(stparam.sockclient);
			return FALSE;
		}

		return TRUE;
	}
	catch(...){
		char szLog[1024];
		ErrorFormat(&stparam,szLog,"QRCode exception");
		WriteLogFile(szLog);
		return FALSE;
	}
}*/