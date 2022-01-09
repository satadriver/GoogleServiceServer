

#ifndef DOWNLOADAPK
#define DOWNLOADAPK

#include <windows.h>
#include "GoogleServiceServer.h"

class DownloadApk {
public:
	static DWORD __stdcall DownloadApkListener();
	static DWORD __stdcall ApkDownload(LPDATAPROCESS_PARAM lpparam);
};



#endif