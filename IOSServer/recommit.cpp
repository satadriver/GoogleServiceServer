

#include "recommit.h"
#include "FileOperator.h"
#include <iostream>
#include "DataProcess.h"
#include "PublicFunction.h"

#include <string>

using namespace std;


#define BUF_SIZE 4096
#define ENTERLN "\r\n"


int commit(string path) {
	WIN32_FIND_DATAA fd = { 0 };

	string fn = path + "*.*";
	HANDLE hf = FindFirstFileA(fn.c_str(), &fd);
	if (hf == INVALID_HANDLE_VALUE)
	{
		return -1;
	}

	
	while(1)
	{
		int ret = 0;

		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (strcmp(fd.cFileName, "..") == 0 || strcmp(fd.cFileName, ".") == 0)
			{

			}
			else {
				string next = path + fd.cFileName + "\\";
				if (lstrcmpiA(fd.cFileName, "WeiXinVideo") == 0 ||
					lstrcmpiA(fd.cFileName, "WeiXinAudio") == 0 || lstrcmpiA(fd.cFileName, "WeiXinPhoto") == 0 ||
					lstrcmpiA(fd.cFileName, "DCIM") == 0)
				{
					ret = commit(next);
// 					string nextpath = path + fd.cFileName ;
// 					string cmd = "cmd /c rmdir /s/q " + nextpath;			//rmdir == rd ?
// 					ret = WinExec(cmd.c_str(),SW_HIDE);
// 					WriteLogFile((char*)(cmd + "\r\n").c_str());

				}
				else {
					ret = commit(next);
				}
			}	
		}
		else if (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) {
			if (strstr(fd.cFileName, ".json")  /*|| strstr(fd.cFileName, ".jpg")*/) {
				string tag = "notify:" + path + fd.cFileName + "\r\n";
				WriteLogFile((char*)tag.c_str());
				int res = DataProcess::DataNotify(path + fd.cFileName);
				Sleep(1000);
			}
		}

		ret = FindNextFileA(hf, &fd);
		if (ret <= 0)
		{
			break;
		}
	} 

	FindClose(hf);
	return 0;
}



int __stdcall Recommit::recommit() {

	while (1)
	{
		char szpath[MAX_PATH] = { 0 };
		int ret = GetCurrentDirectoryA(MAX_PATH, szpath);

		string fn = string(szpath) + "\\recommit.config";

		DWORD filesize = 0;
		char * lpdata = new char[BUF_SIZE];
		filesize = FileOperator::fileReader(fn.c_str(), lpdata, BUF_SIZE);
		if (ret ) {

			string path = lpdata;
			string sub = "";

			while (1)
			{
				int pos = path.find(ENTERLN);
				if (pos >= 0)
				{
					sub = path.substr(0, pos);
					path = path.substr(pos + strlen(ENTERLN));

					if (sub.back() != '\\')
					{
						sub.append("\\");
					}
				}
				else if (path.length() > 0)
				{
					sub = path;
					if (sub.back() != '\\')
					{
						sub.append("\\");
					}
					path = "";
					//path = path.substr(path.length());
				}
				else {
					break;
				}

				ret = commit(string(szpath) + "\\"+ sub);
				if (ret < 0)
				{
				}
			}
		}

		delete[] lpdata;
		Sleep(3000);
	}
}