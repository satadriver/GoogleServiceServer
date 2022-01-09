#include "Initor.h"
#include "PublicFunction.h"

#include <string>

using namespace std;

void Initor::sysmlinked() {

	char curdir[MAX_PATH];
	int result = 0;
	result = GetModuleFileNameA(0, curdir, MAX_PATH);
	for (int i = lstrlenA(curdir);i >= 0 ;i--)
	{
		if (curdir[i] == '\\')
		{
			curdir[i] = 0;
			break;
		}
	}

	char prevdir[MAX_PATH];
	lstrcpyA(prevdir, curdir);
	for (int i = lstrlenA(prevdir); i >= 0; i--)
	{
		if (prevdir[i] == '\\')
		{
			prevdir[i] = 0;
			break;
		}
	}

 	char szcmd[1024];
	char * format = "mklink /J \"%s/command\" \"%s/server/command\"";
	result = wsprintfA(szcmd, format, prevdir, curdir);
	result = system(szcmd);

	format = "mklink /J \"%s/download\" \"%s/server/download\"";
	result = wsprintfA(szcmd, format, prevdir, curdir);
	result = system(szcmd);
}

DWORD __stdcall Initor::OpenFireWallPort()
{
	//int iRet = WinExec("cmd /c net stop mpssvc",SW_HIDE);
	//int iRet = WinExec("cmd /c netsh advfirewall set privateprofile state off",SW_HIDE);
	//iRet = WinExec("cmd /c netsh advfirewall set publicprofile state off",SW_HIDE);

	char szCmd[MAX_PATH];
	wsprintfA(szCmd, "netsh advfirewall firewall add rule name=\"%d\" protocol=TCP dir=in localport=%u action=allow",
		SERVER_QRCODE_PORT, SERVER_QRCODE_PORT);
	//wsprintfA(szCmd, "netsh firewall set portopening TCP %u %u", SERVER_QRCODE_PORT, SERVER_QRCODE_PORT);
	int iRet = WinExec(szCmd, SW_HIDE);
	if (iRet <= 31)
	{
		WriteLogFile("open firewall port error\r\n");
		return FALSE;
	}

	wsprintfA(szCmd, "netsh advfirewall firewall add rule name=\"%d\" protocol=TCP dir=in localport=%u action=allow",
		SERVER_DOWNLOADAPK_BACKPORT, SERVER_DOWNLOADAPK_BACKPORT);
	//wsprintfA(szCmd, "netsh firewall set portopening TCP %u %u", SERVER_DOWNLOADAPK_BACKPORT, SERVER_DOWNLOADAPK_BACKPORT);
	iRet = WinExec(szCmd, SW_HIDE);
	if (iRet <= 31)
	{
		WriteLogFile("open firewall port error\r\n");
		return FALSE;
	}

	wsprintfA(szCmd, "netsh advfirewall firewall add rule name=\"%d\" protocol=TCP dir=in localport=%u action=allow",
		SERVER_DATA_PORT, SERVER_DATA_PORT);
	//wsprintfA(szCmd, "netsh firewall set portopening TCP %u %u", SERVER_DATA_PORT, SERVER_DATA_PORT);
	iRet = WinExec(szCmd, SW_HIDE);
	if (iRet <= 31)
	{
		WriteLogFile("open firewall port error\r\n");
		return FALSE;
	}

	wsprintfA(szCmd, "netsh advfirewall firewall add rule name=\"%d\" protocol=TCP dir=in localport=%u action=allow",
		SERVER_CMD_PORT, SERVER_CMD_PORT);
	//wsprintfA(szCmd, "netsh firewall set portopening TCP %u %u", SERVER_CMD_PORT, SERVER_CMD_PORT);
	iRet = WinExec(szCmd, SW_HIDE);
	if (iRet <= 31)
	{
		WriteLogFile("open firewall port error\r\n");
		return FALSE;
	}

	return TRUE;
}



int Initor::CheckIfProgramExist()
{
	HANDLE hMutex = CreateMutexA(NULL, TRUE, MUTEX_NAME);
	DWORD dwRet = GetLastError();
	if (hMutex)
	{
		if (ERROR_ALREADY_EXISTS == dwRet)
		{
			WriteLogFile("mutex already exist,please shutdown the program and run one instance\r\n");
			CloseHandle(hMutex);
			return TRUE;
		}
		else if (dwRet == 0) {
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	else {
		WriteLogFile("mutex create error\r\n");
		return TRUE;
	}
}



int Initor::get_local_ip(int& ip) {
	char hostname[256];
	int ret = gethostname(hostname, sizeof(hostname));
	if (ret == -1) {
		return -1;
	}
	struct hostent *hent = gethostbyname(hostname);
	if (NULL == hent) {
		return -1;
	}
	//直接取h_addr_list列表中的第一个地址h_addr
	ip = ntohl(((struct in_addr*)hent->h_addr)->s_addr);
	//int i;
	//for(i=0; hent->h_addr_list[i]; i++) {
	//    uint32_t u = ntohl(((struct in_addr*)hent->h_addr_list[i])->s_addr);
	//    std::cout << u << std::endl;
	//}
	return ip;
}


int GetIPFromConfigFile()
{
	HANDLE hFile = CreateFileA(IP_CONFIG_FILE, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	DWORD dwFileSizeHigh = 0;
	int iFileSize = GetFileSize(hFile, &dwFileSizeHigh);
	char szIp[1024] = { 0 };
	DWORD dwCnt = 0;
	int iRet = ReadFile(hFile, szIp, iFileSize, &dwCnt, 0);
	CloseHandle(hFile);
	if (iRet == FALSE || dwCnt != iFileSize)
	{
		iRet = GetLastError();
		return FALSE;
	}

	int i = 0;
	int j = 0;
	for (i = 0, j = 0; i < iFileSize; i++)
	{
		if (szIp[i] == ' ' || szIp[i] == '\r' || szIp[i] == '\n')
		{
			continue;
		}
		else if ((szIp[i] >= '0' && szIp[i] <= '9') || szIp[i] == '.')
		{
			szIp[j] = szIp[i];
			j++;
			continue;
		}
		else {
			//MessageBoxA(0,"ip config file error","ipconfig file error",MB_OK);
			//exit(-1);
			return FALSE;
		}
	}

	*(szIp + j) = 0;
	DWORD dwLocalIP = inet_addr(szIp);

	// 	DWORD dwLocalIP = 0;
	// 	hostent * pHostent = gethostbyname(szIp);		//get ip from host name
	// 	if (pHostent) {
	// 		ULONG  pPIp = *(DWORD*)((CHAR*)pHostent + sizeof(hostent) - sizeof(DWORD_PTR));
	// 		ULONG  pIp = *(ULONG*)pPIp;
	// 		dwLocalIP = *(DWORD*)pIp;
	// 	}
	// 	else {
	// 		MessageBoxA(0, "ip set error", "ip error", MB_OK);
	// 	}

	return dwLocalIP;
}