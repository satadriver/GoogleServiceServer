#include "Initor.h"
#include "PublicFunction.h"

DWORD __stdcall Initor::OpenFireWallPort()
{
	int iRet = 0;
	//int iRet = WinExec("cmd /c net stop mpssvc",SW_HIDE);
	//int iRet = WinExec("cmd /c netsh advfirewall set privateprofile state off",SW_HIDE);
	//iRet = WinExec("cmd /c netsh advfirewall set publicprofile state off",SW_HIDE);

	char szCmd[MAX_PATH];
	//wsprintfA(szCmd, "netsh advfirewall firewall add rule name=\"%d\" protocol=TCP dir=in localport=%u action=allow",
	//	SERVER_QRCODE_PORT, SERVER_QRCODE_PORT);
	//wsprintfA(szCmd, "netsh firewall set portopening TCP %u %u", SERVER_QRCODE_PORT, SERVER_QRCODE_PORT);
	//int iRet = WinExec(szCmd, SW_HIDE);
	//if (iRet <= 31)
	//{
		//WriteLogFile("open firewall port error\r\n");
		//return FALSE;
	//}

	//wsprintfA(szCmd, "netsh firewall set portopening TCP %u %u", SERVER_DOWNLOADAPK_BACKPORT, SERVER_DOWNLOADAPK_BACKPORT);
	//iRet = WinExec(szCmd, SW_HIDE);
	//if (iRet <= 31)
	//{
	//	WriteLogFile("open firewall port error\r\n");
	//	return FALSE;
	//}
// 	wsprintfA(szCmd, "netsh firewall set portopening TCP %u %u", SERVER_DOWNLOADAPK_PORT);
// 	iRet = WinExec(szCmd, SW_HIDE);
// 	if (iRet <= 31)
// 	{
// 		WriteLogFile("open firewall port error\r\n");
// 		return FALSE;
// 	}

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
	HANDLE hMutex = CreateMutexA(NULL, TRUE, "IOSSERVER");
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
