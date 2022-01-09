#include <windows.h>
#include "iosServer.h"
#include "PublicFunction.h"



class SendPhoneMessage {
public:
	static int CmdSendMessage(LPDATAPROCESS_PARAM  lpparam, char * imei, char * username, char * cmdfilename, int cmd);
	static DWORD GetPhoneAndMessage(char * src, char dst[2][MAX_MESSAGE_SIZE]);
};



