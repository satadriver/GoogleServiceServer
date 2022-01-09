
#ifndef QRCODE
#define QRCODE
#include <windows.h>

class QRCode {
public:
	static DWORD __stdcall QRCodeListener();
	static DWORD __stdcall QRCodeProcess(DATAPROCESS_PARAM param,char * lpdata,int size);

	static DWORD __stdcall QRCode::QRCodeProcessMain(LPDATAPROCESS_PARAM param);
};


#endif