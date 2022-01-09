#include <windows.h>
#include "iosServer.h"
#include "PublicFunction.h"


class CommandUploadFile {
public:
	static int CmdUploadFile(char * lpUploadRecvBuf, LPDATAPROCESS_PARAM lpparam, char * imei, char * username);
};
