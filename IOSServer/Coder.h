#pragma once

#include "iosServer.h"

class Coder {
public:
	static int GBKFNToUTF8FN(const char* gb2312, char * utf8, int utf8len);

	static int GBKToUTF8(const char* gb2312, char ** lpdatabuf);

	static int UTF8FNToGBKFN(const char* utf8, int utf8len, char * gbk, int gbklen);

	static int UTF8ToGBK(const char* utf8, char ** lpdatabuf);

	static DWORD SplitString(char * src, char dst[2][MAX_MESSAGE_SIZE], char * keyfirst, char * keysencond, char * seperator);
};