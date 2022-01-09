#pragma once

#include <windows.h>
#include "iosServer.h"

#include <iostream>

using namespace  std;

class PublicFunction {
public:
	static int autorun();

	static int addService(string path, string params);
};


int WriteLogFile(char * pLog);
DWORD WriteErrorPacket(LPSTR szhdr, char * lpdata, int isize);
DWORD ErrorFormat(LPDATAPROCESS_PARAM lpparam,char * error,char * prefix);
DWORD ErrorFormat(LPDATAPROCESS_PARAM lpparam,char * error,char * prefix,char * imei,char * username);


DWORD ParsePacketHeader(LPCOMMUNICATION_PACKET_HEADER lphdr);



