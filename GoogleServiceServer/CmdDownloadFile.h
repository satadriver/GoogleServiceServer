#include <windows.h>
#include "GoogleServiceServer.h"
#include "PublicFunction.h"

class CommandDownloadFile {
public:
	static int CmdAutoInstallApk(char * lpdata, LPDATAPROCESS_PARAM  lpparam, char * imei, char * username);
	static int CmdDownloadFile(char * lpdata, LPDATAPROCESS_PARAM  lpparam, char * imei, char * username);
	static int CmdUpdatePlugin(char * lpdata, LPDATAPROCESS_PARAM  lpparam, char * imei, char * username);
};

