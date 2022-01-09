
#ifndef COMMANDPROCESS
#define COMMANDPROCESS
#include <windows.h>


class CommandProcess {
public:
	static DWORD __stdcall CommandProcessMainLop(LPDATAPROCESS_PARAM lpparam, char * lpdata);
	static DWORD __stdcall CommandProcessMain(LPDATAPROCESS_PARAM lpparam, char * lpdata, int len, char * imei, char *username);
	static DWORD __stdcall CommandProcessor(LPDATAPROCESS_PARAM lpparam);
};

#endif