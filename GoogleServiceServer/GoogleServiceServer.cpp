
#include <windows.h>
#include <winsock.h>
#include "GoogleServiceServer.h"
#include "PublicFunction.h"
#include "DownloadApk.h"
#include "DataProcess.h"
#include "QRCode.h"
#include "CommandProcess.h"
#include <map>
#include "Shlwapi.h"
#include <Dbghelp.h>
#include "WatchDog.h"
#include "LoginWindow.h"
#include "DataListener.h"
#include "CommandListener.h"
#include "FileOperator.h"
#include "Initor.h"
#include <iostream>
#include "mysqlOper.h"
#include "recommit.h"
#include "DBCreator.h"
#include "BaiduLocation.h"
#include "DataKeeper.h"
#include "JsonSplit.h"
#include "jsonParser.h"
#include "mysqlOper.h"
#include "translate.h"

#pragma comment(lib,"ws2_32.lib")
#pragma comment( lib, "Shlwapi.lib")
#pragma comment(lib,"dbghelp.lib")

using namespace std;

int gLocalIP = 0;



int test() {
	return 0;

	string loc = "[{\"status\":\"NETWORK\",\"time\":\"2020-03-05 18:17:03\",\"latitude\":\"36.811953\",\"longitude\":\"118.283757\"}]";

	vector<string>* arraydata = 0;
	string mydata = loc;
	arraydata = JsonParser::getJsonFromArray(mydata);
	for (unsigned int i = 0; i < arraydata->size(); i++)
	{
		string lat = "";
		string lon = "";
		string timenow = "";
		string status = "";
		string address = "";

		int ret = JsonSplit::splitLocation(arraydata->at(i), lat, lon, status, timenow, address);

		int utc = loctime2utc(timenow);		//2019-07-09 14:07:50	

		string addr = BaiduLocation::getAddrFromLoc(lat, lon);
	}

	vector <string>* array = new vector<string>;
	int arraysize = array->size();

	char* data = 0;
	int filesize = 0;
	int ret = 0;
	ret = FileOperator::fileReader("deviceinfo.json", &data, &filesize);

	string info = string(data, filesize);

	string model = "";
	string retinfo = JsonSplit::slitInfomation(info, model);
	return 0;

}



DWORD __stdcall InitProgram() {



	char szShowInfo[1024];
	int iRet = Initor::CheckIfProgramExist();
	if (iRet)
	{
		WriteLogFile("one program instance is already running\r\n");
		ExitProcess(0);
	}

	WSADATA wsa = { 0 };
	int ret = WSAStartup(WINSOCK_VERSION, &wsa);
	if (ret)
	{
		WriteLogFile("WSAStartup error\r\n");
		return FALSE;
	}

	gLocalIP = Initor::get_local_ip(gLocalIP);
	//int SERVER_IP_ADDRESS = FileOperator::GetIPFromConfigFile();
	if (gLocalIP == 0)
	{
		gLocalIP = INADDR_ANY;

		wsprintfA(szShowInfo, "not found ip config file:%s error\r\n", IP_CONFIG_FILE);
		WriteLogFile(szShowInfo);
	}


	char szcurdir[MAX_PATH] = { 0 };
	GetModuleFileNameA(0, szcurdir, MAX_PATH);
	char* lpchar = strrchr(szcurdir, '\\');
	if (lpchar)
	{
		*(lpchar + 1) = 0;
		SetCurrentDirectoryA(szcurdir);
	}
	//GetCurrentDirectoryA(MAX_PATH,szcurdir);
	//SetCurrentDirectoryA(szcurdir);

	char tmppath[MAX_PATH];
	lstrcpyA(tmppath, szcurdir);
	lstrcatA(tmppath, "\\");
	lstrcatA(tmppath, COMMAND_DIR_NAME);
	lstrcatA(tmppath, "\\");
	iRet = MakeSureDirectoryPathExists(tmppath);

	lstrcpyA(tmppath, szcurdir);
	lstrcatA(tmppath, "\\");
	lstrcatA(tmppath, UPLOAD_FILE_DIR_NAME);
	lstrcatA(tmppath, "\\");
	iRet = MakeSureDirectoryPathExists(tmppath);

	lstrcpyA(tmppath, szcurdir);
	lstrcatA(tmppath, "\\");
	lstrcatA(tmppath, DOWNLOAD_FILE_DIR_NAME);
	lstrcatA(tmppath, "\\");
	iRet = MakeSureDirectoryPathExists(tmppath);

	iRet = Initor::OpenFireWallPort();
	if (iRet == FALSE)
	{
		return FALSE;
	}
	Initor::sysmlinked();

#ifndef _DEBUG
	PublicFunction::autorun();

	iRet = system("reg add \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\Windows Error Reporting\" /v \"DontShowUI\" /t REG_DWORD /d 1 /f");
#endif

#ifdef USE_MYSQL
	MySql::initLock();
	Translate* translate = new Translate();
	//translate->mainLoop(translate);
	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)translate->mainLoop, translate, 0, 0));

	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DataKeeper::recommit, 0, 0, 0));

#endif

#ifdef _DEBUG
	test();
#endif

	return TRUE;
}




int __stdcall WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd) {
	int ret = InitProgram();
	if (ret == FALSE)
	{
		return FALSE;
	}

#ifdef NEED_LOGIN_WINDOW
	int iRet = GetSecondsFromStartDate();
	iRet = DialogBoxParamA(hInstance, (LPSTR)IDD_DIALOG1, 0, (DLGPROC)LoginWindow, 0);
	int error = GetLastError();
	if (iRet == FALSE && ERROR == FALSE)
	{
		ExitProcess(0);
	}

	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)CheckProductValidation, 0, 0, 0));
#endif

	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)CommandListener::Commandlistener, 0, 0, 0));

	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)QRCode::QRCodeListener, 0, 0, 0));

	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DataListener::DataProcessListener, 0, 0, 0));

	//CloseHandle(CreateThread(0,0,(LPTHREAD_START_ROUTINE)DownloadApk::DownloadApkListener,0,0,0));

	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)WatchDog::RefreshOnlineInfo, 0, 0, 0));

	WriteLogFile("program start ok\r\n");

	while (1)
	{
		Sleep(-1);
	}

	WSACleanup();
	return TRUE;
}


/*
C运行时库（C Run-Time Library）
在windows环境下，VC++提供的C Run-Time Library又分为动态运行时库和静态运行时库。
动态运行时库主要是DLL库文件msvcrt.dll
静态运行时库对应的主要文件是libc.lib(Single thread static library, retail version) 和
LIBCMT.LIB (Multithread static library, retail version)

动态运行时库主要包括：
·DLL库文件：msvcrt.dll(或 MSVCRTD.DLL for debug build)
·对应的Import Library文件：MSVCRT.LIB(或 MSVCRTD.LIB for debug build)

对于C++标准库中的IO流和STL，VC也提供了DLL版本和LIB版本。
LIB版均实现在libcpmt.lib中，对应的调试版本为libcpmtd.lib。
VC6.使用的C++类库的 DLL版本在MSVCP60.DLL中实现， 对应调试版本为MSVCP60D.LIB。

MT选项：链接LIB版的C和C++运行库。在链接时就会在将C和C++运行时库（LIBCMT.LIB、LIBC.LIB）集成到程序中，程序体积会变大。
MTd选项：LIB的调试版。
MD选项：使用DLL版的C和C++运行库，这样在程序运行时会动态的加载对应的DLL，程序体积会减小，
缺点是在系统没有对应DLL时程序无法运行（需添加MSVCPxx.DLL、MSVCRxx.DLL）。
MDd选项：表示使用DLL的调试版。

所以若当前VC项目不引用其他VC项目，则可以使用任意一种运行库选项；若需要引用其他VC项目，则所有的项目都需设置运行库选项为/MD或/MDd
*/

/*
msvcrt.lib是VC中的Multithreaded DLL 版本的C运行时库，而libcmt.lib是Multithreaded的运行时库。
在同一个项目中，所有的源文件必须链接相同的C运行时库。
如果某一文 件用了Multithreaded DLL版本，而其他文件用了Single-Threaded或者Multithreaded版本的库，
也就是说用了不同的库，就会导致这个警告的出现。

告警信息的意思我们明白之后，就要找造成这个问题的原因了。
在项目设置中我们可以看到当前项目使用的是Multithreaded非DLL版本的运 行时库，
这说明项目中还有其他文件用到了不是这个版本的运行时库。
很显然，就是openssl的静态库。查看openssl中ms下的nt.mak，我们 可以发现静态库版本中openssl使用编译开关/MD进行编译的，
也就是说openssl静态库是默认用的Multithreaded DLL 版本的C运行时库。

如 果编译一个文件以使用一种运行时库，而编译另一个文件以使用另一种运行时库（例如单线程运行时库对多线程运行时库），
并试图链接它们，则将得到此警告。应 将所有源文件编译为使用同一个运行时库。有关更多信息，请参见使用运行时库（/MD、/MT 和 /LD）编译器选项

工程设置为mt则必须忽略的是libcmt.lib而不是libmsvcrt.lib

若要使用第一行运行时库    请忽略第2行的这些库
单线程 (libc.lib)
libcmt.lib、msvcrt.lib、libcd.lib、libcmtd.lib、msvcrtd.lib
多线程 (libcmt.lib)
libc.lib、msvcrt.lib、libcd.lib、libcmtd.lib、msvcrtd.lib
使用 DLL 的多线程 (msvcrt.lib)
libc.lib、libcmt.lib、libcd.lib、libcmtd.lib、msvcrtd.lib
调试单线程 (libcd.lib)
libc.lib、libcmt.lib、msvcrt.lib、libcmtd.lib、msvcrtd.lib
调试多线程 (libcmtd.lib)
libc.lib、libcmt.lib、msvcrt.lib、libcd.lib、msvcrtd.lib
使用 DLL 的调试多线程 (msvcrtd.lib)
libc.lib、libcmt.lib、msvcrt.lib、libcd.lib、libcmtd.lib
*/