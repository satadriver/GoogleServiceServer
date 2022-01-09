#include "FileOperator.h"
#include <windows.h>
#include "GoogleServiceServer.h"





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
