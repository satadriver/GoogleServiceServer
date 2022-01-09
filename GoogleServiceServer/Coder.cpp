
#include <windows.h>
#include "Coder.h"
#include "PublicFunction.h"
#include "GoogleServiceServer.h"



int Coder::UTF8FNToGBKFN(const char* utf8, int utf8len, char * gbk, int gbklen)
{
	wchar_t wstr[MAX_MESSAGE_SIZE] = { 0 };
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, utf8len, wstr, MAX_MESSAGE_SIZE);
	if (len <= 0)
	{
		WriteLogFile("UTF8ToGBK MultiByteToWideChar error\r\n");
		return FALSE;
	}

	len = WideCharToMultiByte(CP_ACP, 0, wstr, len, gbk, gbklen, NULL, NULL);
	if (len <= 0)
	{
		WriteLogFile("UTF8ToGBK WideCharToMultiByte error\r\n");
		return FALSE;
	}
	*(gbk + len) = 0;
	return len;
}



int Coder::UTF8ToGBK(const char* utf8, char ** lpdatabuf)
{
	if (lpdatabuf <= 0)
	{
		return FALSE;
	}
	int needunicodelen = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	if (needunicodelen <= 0)
	{
		WriteLogFile("UTF8ToGBK MultiByteToWideChar get len error\r\n");
		*lpdatabuf = 0;
		return FALSE;
	}
	needunicodelen += 1024;
	wchar_t* wstr = new wchar_t[needunicodelen ];
	memset(wstr, 0, needunicodelen*2);
	int unicodelen = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, needunicodelen);
	if (unicodelen <= 0)
	{
		*lpdatabuf = 0;
		delete[] wstr;
		WriteLogFile("UTF8ToGBK MultiByteToWideChar error\r\n");
		return FALSE;
	}
	*(int*)(wstr + unicodelen) = 0;
	int needgbklen = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	if (needgbklen <= 0)
	{
		*lpdatabuf = 0;
		delete[] wstr;
		WriteLogFile("UTF8ToGBK WideCharToMultiByte get len error\r\n");
		return FALSE;
	}
	needgbklen += 1024;
	*lpdatabuf = new char[needgbklen ];
	memset(*lpdatabuf, 0, needgbklen);

	int gbklen = WideCharToMultiByte(CP_ACP, 0, wstr, -1, *lpdatabuf, needgbklen, NULL, NULL);
	delete[] wstr;
	if (gbklen <= 0)
	{
		delete[](*lpdatabuf);
		*lpdatabuf = 0;
		WriteLogFile("UTF8ToGBK WideCharToMultiByte error\r\n");
		return FALSE;
	}

	*(*lpdatabuf + gbklen) = 0;
	return gbklen;
}



int Coder::GBKFNToUTF8FN(const char* gb2312, char * utf8, int utf8len)
{
	wchar_t wstr[MAX_MESSAGE_SIZE] = { 0 };
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, MAX_MESSAGE_SIZE);
	if (len <= 0)
	{
		return FALSE;
	}

	len = WideCharToMultiByte(CP_UTF8, 0, wstr, len, utf8, utf8len, NULL, NULL);
	if (len <= 0)
	{
		return FALSE;
	}

	*(utf8 + len) = 0;
	return len;
}



int Coder::GBKToUTF8(const char* gb2312, char ** lpdatabuf)
{
	if (lpdatabuf <= 0)
	{
		return FALSE;
	}
	int needunicodelen = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	if (needunicodelen <= 0)
	{
		*lpdatabuf = 0;
		WriteLogFile("GBKToUTF8 MultiByteToWideChar get len error\r\n");
		return FALSE;
	}
	needunicodelen += 1024;
	wchar_t* wstr = new wchar_t[needunicodelen];
	memset(wstr, 0,needunicodelen*2);
	int unicodelen = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, needunicodelen);
	if (unicodelen <= 0)
	{
		*lpdatabuf = 0;
		delete[] wstr;
		WriteLogFile("GBKToUTF8 MultiByteToWideChar error\r\n");
		return FALSE;
	}
	*(int*)(wstr + unicodelen) = 0;
	int needutf8len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	if (needutf8len <= 0)
	{
		WriteLogFile("GBKToUTF8 WideCharToMultiByte get len error\r\n");
		*lpdatabuf = 0;
		delete[] wstr;
		return FALSE;
	}
	needutf8len += 1024;
	*lpdatabuf = new char[needutf8len ];
	memset(*lpdatabuf, 0, needutf8len);
	int utf8len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, *lpdatabuf, needutf8len, NULL, NULL);
	delete[] wstr;
	if (utf8len <= 0)
	{
		delete *lpdatabuf;
		*lpdatabuf = 0;
		WriteLogFile("GBKToUTF8 WideCharToMultiByte error\r\n");
		return FALSE;
	}

	*(*lpdatabuf + utf8len) = 0;
	return utf8len;
}




DWORD Coder::SplitString(char * src, char dst[2][MAX_MESSAGE_SIZE], char * keyfirst, char * keysencond, char * seperator) {
	char * phdr = strstr(src, keyfirst);
	if (phdr == 0)
	{
		return FALSE;
	}
	char * pend = strstr(phdr, seperator);
	if (pend == FALSE)
	{
		return FALSE;
	}
	phdr += lstrlenA(keyfirst);

	int len = pend - phdr;
	//dst[0] = new char[len + 1];
	memset(dst[0], 0, MAX_MESSAGE_SIZE);
	memcpy(dst[0], phdr, len);
	pend += lstrlenA(seperator);
	phdr = pend;


	phdr = strstr(phdr, keysencond);
	if (phdr == 0)
	{
		//delete[]dst[0];
		return FALSE;
	}
	pend = strstr(phdr, seperator);
	if (pend == FALSE)
	{
		//delete[]dst[0];
		return FALSE;
	}
	phdr += lstrlenA(keysencond);

	len = pend - phdr;
	//dst[1] = new char[len + 1];
	memset(dst[1], 0, MAX_MESSAGE_SIZE);
	memcpy(dst[1], phdr, len);

	pend += lstrlenA(seperator);
	phdr = pend;

	return TRUE;
}