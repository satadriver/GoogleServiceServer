#include <windows.h>
#include "GoogleServiceServer.h"
#include "PublicFunction.h"



int localFileRecver(char * lpdata, int len, char * filename, char * lpdstfn,
	LPDATAPROCESS_PARAM lpparam, char * username, char * imei, int withparam, int append);
int localFileSender(char * lpbuf, int cmd, const char * localfn, char * gbkparamfn, LPDATAPROCESS_PARAM lpparam, char * username, char * imei);