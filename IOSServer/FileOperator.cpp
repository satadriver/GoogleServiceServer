#include "FileOperator.h"
#include <windows.h>
#include "iosServer.h"


DWORD FileOperator::GetIPFromConfigFile()
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


int FileOperator::TripSufixName(char* strFullName, char * dstfn, char * surfix)
{
	lstrcpyA(dstfn, strFullName);
	int surfixlen = lstrlenA(surfix);

	int len = lstrlenA(dstfn) - surfixlen;

	if (memcmp(dstfn + len, surfix, surfixlen) == 0)
	{
		dstfn[len] = 0;
		return TRUE;
	}

	return FALSE;
}



int FileOperator::GetPathFromFullName(char* strFullName, char * strDst)
{
	lstrcpyA(strDst, strFullName);

	for (int i = lstrlenA(strDst) - 1; i >= 0; i--)
	{
		if (strDst[i] == 0x5c)
		{
			strDst[i + 1] = 0;
			return TRUE;
		}
	}

	return FALSE;
}


int FileOperator::getfilesize(string fn)
{
	HANDLE hfile = CreateFileA(fn.c_str(), GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hfile == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	int filesize = GetFileSize(hfile, 0);
	CloseHandle(hfile);
	return filesize;
}

int FileOperator::fileWriter(string filename, const char * lpdate, int datesize) {
	int ret = 0;

	FILE * fp = fopen(filename.c_str(), "ab+");
	if (fp <= 0)
	{
		printf("fileReader fopen file:%s error\r\n", filename.c_str());
		return FALSE;
	}

	ret = fwrite(lpdate, 1, datesize, fp);
	fclose(fp);
	if (ret == FALSE)
	{
		return FALSE;
	}

	return datesize;
}


int FileOperator::fileWriter(string filename, const char * lpdate, int datesize, int cover) {
	int ret = 0;

	FILE * fp = 0;
	if (cover) {
		fp = fopen(filename.c_str(), "wb");
	}
	else {
		fp = fopen(filename.c_str(), "ab+");
	}

	if (fp <= 0)
	{
		printf("fileReader fopen file:%s error\r\n", filename.c_str());
		return FALSE;
	}

	ret = fwrite(lpdate, 1, datesize, fp);
	fclose(fp);
	if (ret == FALSE)
	{
		return FALSE;
	}

	return datesize;
}

int FileOperator::fileReader(string cmdfn, char* szCmd, int buflimit) {
	int iRet = 0;
	HANDLE hFileMessage = CreateFileA(cmdfn.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFileMessage != INVALID_HANDLE_VALUE)
	{
		int dwFileSize = GetFileSize(hFileMessage, 0);
		//if be reset-lock-screen-password,it may be empty,so file size 0 is ok
		if (dwFileSize > 0 && dwFileSize < buflimit)
		{
			DWORD dwCnt = 0;

			iRet = ReadFile(hFileMessage, szCmd, dwFileSize, &dwCnt, 0);

			CloseHandle(hFileMessage);
			*(szCmd + dwFileSize) = 0;
			return dwFileSize;
		}

		CloseHandle(hFileMessage);
	}

	return FALSE;
}

int FileOperator::fileReader(string filename, char ** lpbuf, int *bufsize) {
	int ret = 0;

	FILE * fp = fopen(filename.c_str(), "rb");
	if (fp <= 0)
	{
		printf("fileReader fopen file:%s error\r\n", filename.c_str());
		return FALSE;
	}

	ret = fseek(fp, 0, FILE_END);

	int filesize = ftell(fp);

	ret = fseek(fp, 0, FILE_BEGIN);

	*bufsize = filesize;

	*lpbuf = new char[filesize + 0x1000];

	ret = fread(*lpbuf, 1, filesize, fp);
	fclose(fp);
	if (ret <= FALSE)
	{
		delete *lpbuf;
		return FALSE;
	}

	*(*lpbuf + filesize) = 0;
	return filesize;
}