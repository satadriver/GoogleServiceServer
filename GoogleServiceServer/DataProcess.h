


#ifndef DATAPROCESS
#define DATAPROCESS

#include <windows.h>
#include <iostream>
#include "GoogleServiceServer.h"

using namespace std;


class DataProcess {
public:
	static DWORD __stdcall DataProcessor(LPDATAPROCESS_PARAM lpparam);
	static DWORD __stdcall DataProcessMain(LPDATAPROCESS_PARAM lpparam, char **lpdata);
	static DWORD __stdcall DataWriter(LPDATAPROCESS_PARAM lpparam,LPCOMMUNICATION_PACKET_HEADER lphdr, char * lpdata, int len,char * usernaem,char * imei);
	static DWORD __stdcall DataNotify(string filename);
};
#endif
