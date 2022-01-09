#pragma once

#include <windows.h>

#include <iostream>

using namespace std;

extern string gKey;

extern string gCmdKey;


class CryptoUtils {
public:
	static DWORD xorCryptData(char * data, int datalen, char * key, int keylen, char * dstdata, int dstdatalen);
};
