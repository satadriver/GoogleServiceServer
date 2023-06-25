
#include <windows.h>
#include "GoogleServiceServer.h"
#include "PublicFunction.h"
#include "DataProcess.h"
#include "QRCode.h"
#include "FileWriter.h"
#include <iostream>
#include "HttpUtils.h"
#include "networkHelper.h"
#include "FileOperator.h"

#include <string>

using namespace std;



DWORD __stdcall QRCode::QRCodeListener() {

	__try {
		int ret = 0;
		int s = NetworkHelper::listener(SERVER_QRCODE_PORT);

		SOCKADDR_IN siclient = { 0 };
		DATAPROCESS_PARAM stparam = { 0 };
		while (TRUE) {
			int addrlen = sizeof(SOCKADDR_IN);

			SOCKET sockclient = accept(s, (sockaddr*)&siclient, &addrlen);
			if (sockclient != INVALID_SOCKET)
			{
				stparam.si = siclient;
				stparam.sockclient = sockclient;
				memset(stparam.currentpath, 0, MAX_PATH);
				ret = GetCurrentDirectoryA(MAX_PATH, stparam.currentpath);
				if (ret) {
					HANDLE h = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)QRCodeProcessMain, &stparam, 0, 0);
					CloseHandle(h);
				}
				continue;
			}
			else {
				WriteLogFile("QRCodeListener accept error\r\n");
				continue;
			}
		}
		closesocket(s);
	}
	__except (1) {
		WriteLogFile("qrcode listener exception\r\n");
	}
	return TRUE;
}


#define QRCodeProcessException 4

//http://47.101.189.13:10012/
//http://192.168.10.183:10012/

DWORD __stdcall QRCode::QRCodeProcessMain(LPDATAPROCESS_PARAM lpparam) {
	DATAPROCESS_PARAM stparam = *lpparam;
	int ret = 0;
	char szLog[1024];
	char lpdata[0x1000] = { 0 };
	int dwPackSize = recv(stparam.sockclient, lpdata, 0x1000, 0);
	if (dwPackSize <= 0) {
		ret = GetLastError();
		ErrorFormat(&stparam, szLog, "QRCodeProcess first packet error");
		WriteLogFile(szLog);
	}
	else {
		*(lpdata + dwPackSize) = 0;
		ret = QRCodeProcess(stparam, lpdata, dwPackSize);
	}

	closesocket(stparam.sockclient);
	return ret;
}

DWORD __stdcall QRCode::QRCodeProcess(DATAPROCESS_PARAM stparam, char* lpdata, int dwPackSize) {

	int ret = 0;
	char szLog[1024];

	try {
		if (memcmp(lpdata, "GET / ", 6) == 0 || memcmp(lpdata, "GET /index.html", lstrlenA("GET /index.html")) == 0)
		{
			string htmfn = string(stparam.currentpath) + "/index.html";

			char szresponse[0x1000];
			char* szresponseformat = "HTTP/1.1 200 OK\r\n"
				"Content-Type: text/html\r\n"
				"Content-Length: %u\r\n"
				"Connection: keep-alive\r\n\r\n%s";

			char szsendbuf[0x8000];
			int filesize = FileOperator::fileReader(htmfn, szsendbuf, 0x4000);
			if (filesize <= 0)
			{
				return FALSE;
			}

			int responselen = sprintf(szresponse, szresponseformat, filesize, szsendbuf);

			ret = send(stparam.sockclient, szresponse, responselen, 0);
			if (ret <= 0)
			{
				ErrorFormat(&stparam, szLog, "QRCodeProcess send html error");
				WriteLogFile(szLog);
				return FALSE;
			}
			else {
				return TRUE;
			}
		}
		else if (strstr(lpdata, ".apk"))
		{
			string url = HttpUtils::getFileNameFromUrl(lpdata, dwPackSize);

			string path = string(stparam.currentpath) + url;

			int filesize = FileOperator::getfilesize(path);
			if (filesize > 0)
			{
				char* szresponseformat = "HTTP/1.1 200 OK\r\n"
					//"Content-Type: application/zip\r\n"
					"Content-Type: application/vnd.android.package-archive\r\n"
					"Content-Length: %u\r\n"
					"Connection: keep-alive\r\n\r\n";
				int APK_FILE_BUF_SIZE = filesize + 4096;
				char* lpbuf = new char[APK_FILE_BUF_SIZE];

				int httphdrlen = sprintf(lpbuf, szresponseformat, filesize);

				int apkfilesize = FileOperator::fileReader(path.c_str(), lpbuf + httphdrlen, APK_FILE_BUF_SIZE - httphdrlen);
				if (apkfilesize > 0)
				{
					ret = send(stparam.sockclient, lpbuf, filesize + httphdrlen, 0);
				}
				delete[] lpbuf;
			}

			return FALSE;
		}
		else {
			string url = HttpUtils::getFileNameFromUrl(lpdata, dwPackSize);
			wsprintfA(szLog, "QRCodeProcess recv unrecognized packet error:%s\r\n", (char*)url.c_str());
			WriteLogFile(szLog);
			return FALSE;
		}

		return TRUE;
	}
	catch (...) {
		ErrorFormat(&stparam, szLog, "QRCode exception");
		WriteLogFile(szLog);
		return FALSE;
	}
}