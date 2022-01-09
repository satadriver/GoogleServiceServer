#pragma once

#ifndef COMMANDUTILS
#define COMMANDUTILS


#include <windows.h>
#include <iostream>
#include "GoogleServiceServer.h"


using namespace std;

class CommandUtils {
public:
	static int CommandUtils::SendHeartBeat(LPDATAPROCESS_PARAM lpparam, char * imei, char * username);

	static int CommandWithString(int cmd, char * lpcmdfn, LPDATAPROCESS_PARAM lpparam, char * imei, char * user);
	static int CommandWithValue(int cmd, char * lpcmdfn, LPDATAPROCESS_PARAM lpparam, char * imei, char * user);
	static int CommandSendConfig(int cmd, char * lpcmdfn, LPDATAPROCESS_PARAM lpparam, char * imei, char * user);
	static int GeneralCommand(int cmd, char * lpcmdfn, LPDATAPROCESS_PARAM lpparam, char * imei, char * user);

	static int CommandUtils::CommandWithStringStringString(int cmd, char * lpcmdfn, LPDATAPROCESS_PARAM lpparam, char * imei, char * user);

	static DWORD CommandUtils::GetStringStringString(char * src, char dst[3][MAX_MESSAGE_SIZE]);
};

#endif