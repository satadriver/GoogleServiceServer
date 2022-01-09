
#pragma once

#include <windows.h>
#include "GoogleServiceServer.h"

#include <iostream>

using namespace std;

class PublicFunction {
public:
	static int autorun();
};

int WriteLogFile(char * pLog);
DWORD WriteErrorPacket(LPSTR szhdr, char * lpdata, int isize);
DWORD ErrorFormat(LPDATAPROCESS_PARAM lpparam,char * error,char * prefix);
DWORD ErrorFormat(LPDATAPROCESS_PARAM lpparam,char * error,char * prefix,char * imei,char * username);

string getformatdt();

DWORD ParsePacketHeader(LPCOMMUNICATION_PACKET_HEADER lphdr);


void replaceSplash(string & str);

//void replaceSplashAndEnter(string & str);
//void replaceQuot(string & str);
void replaceSplashAndEnterAndQuot(string & str);

string getfnfrompath(string path);

int formattime2utc(string formattm);

DWORD loctime2utc(string formattm);

string timet2str(time_t *t);

int strduration2Int(string duration);

string getipstr(unsigned long ip);

