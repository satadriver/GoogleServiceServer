

#include "CryptoUtils.h"



extern string gKey = "fuck all ios and android and windows and linux and unix crackers";

extern string gCmdKey = "freedom is to free yourself";



DWORD CryptoUtils::xorCryptData(char * data, int datalen, char * key, int keylen, char * dstdata, int dstdatalen) {

	if (keylen <= 0 || datalen <= 0 || dstdatalen <= 0 || dstdatalen < datalen) {
		return FALSE;
	}

	for (int i = 0, j = 0; i < datalen; i++) {
		dstdata[i] = (data[i] ^ key[j]);
		j++;
		if (j >= keylen) {
			j = 0;
		}
	}

	return TRUE;
}