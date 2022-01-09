#include <windows.h>
#include <TlHelp32.h>
#include <time.h>
#include <stdio.h>
#include <iostream>
#include "main.h"

#include <string>

using namespace std;


#define LOG_FILENAME "deamon.log"
//#define EXE_FILE_NAME "GoogleServiceServer.exe"
#define EXE_FILE_NAME "IosServer.exe"
#define MAX_HANDLES_COUNT 10000
#define MAX_TIME_DURATION 24*60*60
#define MAX_THREAD_CNT 1024

DWORD WriteLogFile(const char * pData)
{
	string fn = LOG_FILENAME;
	int iRet = 0;
	FILE * fpFile = 0;
	iRet = fopen_s(&fpFile, fn.c_str(), "ab+");
	if (fpFile > 0)
	{
		string sztime = getDateTime() + " ";
		iRet = fwrite(sztime.c_str(), 1, sztime.length(), fpFile);
		int writelen = lstrlenA(pData);
		iRet = fwrite(pData, 1, writelen, fpFile);
		fclose(fpFile);
		if (iRet != writelen)
		{
			printf("写文件:%s错误:%u\n", fn.c_str(), GetLastError());
			return FALSE;
		}
		return TRUE;
	}
	else
	{
		printf("打开文件:%s错误:%u\n", fn.c_str(), GetLastError());
		return FALSE;
	}
	return FALSE;
}

string getDateTime() {
	SYSTEMTIME sttime = { 0 };
	GetLocalTime(&sttime);

	char sztime[MAX_PATH] = { 0 };
	int len = wsprintfA(sztime, "%u/%u/%u %u:%u:%u", sttime.wYear, sttime.wMonth, sttime.wDay, sttime.wHour, sttime.wMinute, sttime.wSecond);
	return string(sztime);
}

DWORD GetProcessidFromName(char * strname, int & threadcnt)
{
	wchar_t name[MAX_PATH] = { 0 };
	int ret = 0;
	ret = MultiByteToWideChar(CP_ACP, 0, strname, lstrlenA(strname), name, MAX_PATH);

	PROCESSENTRY32 pe = { 0 };
	DWORD id = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapshot, &pe)) {
		return 0;
	}

	while (1)
	{
		pe.dwSize = sizeof(PROCESSENTRY32);
		if (Process32Next(hSnapshot, &pe) == FALSE)
		{
			break;
		}

		if (lstrcmpiW(pe.szExeFile, name) == 0)
		{
			id = pe.th32ProcessID;
			threadcnt = pe.cntThreads;
			break;
		}
	}
	CloseHandle(hSnapshot);
	return id;
}

int autorun() {
	char szcurpath[MAX_PATH];
	int ret = GetModuleFileNameA(0, szcurpath, MAX_PATH);
	string format = "reg add HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run /v AUTORUN /t REG_SZ /d \"%s\" /f";
	char szcmd[1024];
	int cmdlen = wsprintfA(szcmd, format.c_str(), szcurpath);
	ret = WinExec(szcmd, SW_SHOW);
	return ret;
}


int killProcess(string procname) {
	int ret = 0;
	string cmdformat = "taskkill /im %s /f";
	char szcmd[1024];
	int len = wsprintfA(szcmd, cmdformat.c_str(), procname.c_str());
	ret = WinExec(szcmd, SW_SHOW);
	return ret;
}

int startProc(string procname) {
	int ret = 0;
	char szpath[MAX_PATH] = { 0 };
	ret = GetCurrentDirectoryA(MAX_PATH, szpath);

	char szcmd[1024];
	string filename = string(szpath) + "\\" + procname;
	wsprintfA(szcmd, "\"%s\" %s %s %s", filename.c_str(), "server", "123456", "1");
	ret = WinExec(szcmd, SW_SHOW);
	return ret;
}

int __stdcall WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {

	int ret = 0;
	char szout[1024];

	ret = autorun();

	ret = system("reg add \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\Windows Error Reporting\" /v \"DontShowUI\" /t REG_DWORD /d 1 /f");

	time_t lasttime = time(0);

	int argc = 0;
	wchar_t * * params = CommandLineToArgvW(GetCommandLineW(), &argc);

	while (1)
	{
		time_t now = time(0);
		int eclapsetime = now - lasttime;

		int threadcnt = 0;

		int procid = GetProcessidFromName(EXE_FILE_NAME, threadcnt);
		if (procid > 0)
		{
			if (threadcnt < MAX_THREAD_CNT)
			{
				HANDLE hproc = OpenProcess(PROCESS_ALL_ACCESS, 0, procid);
				if (hproc > 0)
				{
					DWORD prochandles = 0;
					ret = GetProcessHandleCount(hproc, &prochandles);
					CloseHandle(hproc);

					if (prochandles >= MAX_HANDLES_COUNT)
					{
						lasttime = now;
						killProcess(EXE_FILE_NAME);
						wsprintfA(szout, "kill process for handle count:%u,seconds:%u\r\n", prochandles, eclapsetime);
						WriteLogFile(szout);
					}
					else {

						if (eclapsetime >= MAX_TIME_DURATION)
						{
							lasttime = now;
							killProcess(EXE_FILE_NAME);

							wsprintfA(szout, "kill process for seconds:%u,handle count:%u\r\n", eclapsetime, prochandles);
							WriteLogFile(szout);
						}
						else {
							//status ok
						}
					}
				}
				else {
					wsprintfA(szout, "OpenProcess id:%u error\r\n", procid);
					WriteLogFile(szout);
				}
			}
			else {
				lasttime = now;
				killProcess(EXE_FILE_NAME);

				wsprintfA(szout, "kill process for thread count:%u,seconds:%u\r\n", threadcnt, eclapsetime);
				WriteLogFile(szout);
			}
		}
		else {
			ret = startProc(EXE_FILE_NAME);
			wsprintfA(szout, "start process\r\n");
			WriteLogFile(szout);
		}

		Sleep(6000);
	}

	return 0;
}