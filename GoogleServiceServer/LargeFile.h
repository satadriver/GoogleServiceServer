#pragma once

#include "GoogleServiceServer.h"
#include <iostream>

using namespace std;

class LargeFile {
public:
	static int recvLargeFile(LPDATAPROCESS_PARAM lpparam, char * lpdata, int recvlen, LPCOMMUNICATION_PACKET_HEADER lphdr,
		string username, string imei);
};