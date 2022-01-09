
#include "PacketMaker.h"
#include "iosServer.h"



int PacketMaker::hdrWithNameAndSize(char lphdr[], int cmd, char * imei, char * name, int datasize) {

	int namelen = lstrlenA(name);

	int iLen = 0;
	iLen += sizeof(DWORD);
	*(DWORD*)(lphdr + iLen) = cmd;
	iLen += sizeof(DWORD);
	memmove(lphdr + iLen, imei, IOS_IDENTIFIER_SIZE);
	iLen += IOS_IDENTIFIER_SIZE;
	*(DWORD*)(lphdr + iLen) = namelen;
	iLen += sizeof(DWORD);
	lstrcpyA(lphdr + iLen, name);
	iLen += namelen;
	*(DWORD*)(lphdr + iLen) = datasize;
	iLen += sizeof(DWORD);
	*(DWORD*)(lphdr) = (iLen + datasize);
	return iLen;
}


int PacketMaker::hdrWithName(char lphdr[], int cmd, char * imei, char * name) {

	int namelen = lstrlenA(name);

	int iLen = 0;
	iLen += sizeof(DWORD);
	*(DWORD*)(lphdr + iLen) = cmd;
	iLen += sizeof(DWORD);
	memmove(lphdr + iLen, imei, IOS_IDENTIFIER_SIZE);
	iLen += IOS_IDENTIFIER_SIZE;

	*(DWORD*)(lphdr + iLen) = namelen;
	iLen += sizeof(DWORD);
	lstrcpyA(lphdr + iLen, name);
	iLen += namelen;

	*(DWORD*)lphdr = iLen;

	return iLen;
}



int PacketMaker::hdr(char szData[],int cmd,char * imei) {
	int iLen = 0;
	*(DWORD*)(szData + iLen) = sizeof(COMMUNICATION_PACKET_HEADER_CMD);
	iLen += sizeof(DWORD);
	*(DWORD*)(szData + iLen) = cmd;
	iLen += sizeof(DWORD);
	memmove(szData + iLen, imei, IOS_IDENTIFIER_SIZE);
	iLen += IOS_IDENTIFIER_SIZE;
	return iLen;
}


int PacketMaker::hdrWithValue(char szData[],int cmd,char * imei,int value)
{
	int iLen = 0;
	*(DWORD*)(szData + iLen) = 0;
	iLen += sizeof(DWORD);
	*(DWORD*)(szData + iLen) = cmd;
	iLen += sizeof(DWORD);
	memmove(szData + iLen, imei, IOS_IDENTIFIER_SIZE);
	iLen += IOS_IDENTIFIER_SIZE;

	*(DWORD*)(szData + iLen) = value;
	iLen += sizeof(DWORD);

	*(DWORD*)szData = iLen;

	return iLen;
}


int PacketMaker::hdrWithStringString(char szData[], int cmd, char * imei, char * str1,char *str2) {

	int str1len = lstrlenA(str1);
	int str2len = lstrlenA(str2);

	int iLen = 0;
	iLen += sizeof(DWORD);
	*(DWORD*)(szData + iLen) = cmd;
	iLen += sizeof(DWORD);
	memmove(szData + iLen, imei, IOS_IDENTIFIER_SIZE);
	iLen += IOS_IDENTIFIER_SIZE;

	*(DWORD*)(szData + iLen) = str1len;
	iLen += sizeof(DWORD);
	lstrcpyA(szData + iLen, str1);
	iLen += str1len;

	*(DWORD*)(szData + iLen) = str2len;
	iLen += sizeof(DWORD);
	lstrcpyA(szData + iLen, str2);
	iLen += str2len;

	*(DWORD*)(szData) = iLen;

	return iLen;
}


int PacketMaker::hdrWithStringStringData(char *lpDownloadSendBuf,char * imei,int cmd,char  classfunc[2][MAX_MESSAGE_SIZE],int dwFileSize) {
	int iLen = 0;
	iLen += sizeof(DWORD);
	*(DWORD*)(lpDownloadSendBuf + iLen) = cmd;
	iLen += sizeof(DWORD);
	memmove(lpDownloadSendBuf + iLen, imei,  IOS_IDENTIFIER_SIZE);
	iLen += IOS_IDENTIFIER_SIZE;
	*(DWORD*)(lpDownloadSendBuf + iLen) = lstrlenA(classfunc[0]);
	iLen += sizeof(DWORD);
	lstrcpyA(lpDownloadSendBuf + iLen, classfunc[0]);
	iLen += lstrlenA(classfunc[0]);
	*(DWORD*)(lpDownloadSendBuf + iLen) = lstrlenA(classfunc[1]);
	iLen += sizeof(DWORD);
	lstrcpyA(lpDownloadSendBuf + iLen, classfunc[1]);
	iLen += lstrlenA(classfunc[1]);

	*(DWORD*)(lpDownloadSendBuf + iLen) = dwFileSize;
	iLen += sizeof(DWORD);

	return iLen;
}



int PacketMaker::hdrWithStringStringString(char *lpDownloadSendBuf, char * imei, int cmd, char  classfunc[3][MAX_MESSAGE_SIZE]) {
	int iLen = 0;
	iLen += sizeof(DWORD);
	*(DWORD*)(lpDownloadSendBuf + iLen) = cmd;
	iLen += sizeof(DWORD);
	memmove(lpDownloadSendBuf + iLen, imei, IOS_IDENTIFIER_SIZE);
	iLen += IOS_IDENTIFIER_SIZE;

	*(DWORD*)(lpDownloadSendBuf + iLen) = lstrlenA(classfunc[0]);
	iLen += sizeof(DWORD);
	lstrcpyA(lpDownloadSendBuf + iLen, classfunc[0]);
	iLen += lstrlenA(classfunc[0]);

	*(DWORD*)(lpDownloadSendBuf + iLen) = lstrlenA(classfunc[1]);
	iLen += sizeof(DWORD);
	lstrcpyA(lpDownloadSendBuf + iLen, classfunc[1]);
	iLen += lstrlenA(classfunc[1]);

	*(DWORD*)(lpDownloadSendBuf + iLen) = lstrlenA(classfunc[2]);
	iLen += sizeof(DWORD);
	lstrcpyA(lpDownloadSendBuf + iLen, classfunc[2]);
	iLen += lstrlenA(classfunc[2]);

	*(DWORD*)(lpDownloadSendBuf) = iLen;

	return iLen;
}
