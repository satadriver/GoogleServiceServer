
#include "Additional.h"
#include "GoogleServiceServer.h"

#include <string>

using namespace std;

//android id 16 bytes length
int Additional::checkImei(unsigned char * imei) {
	if ( (lstrlenA((char*)imei) < (IMEI_IMSI_PHONE_SIZE - 2)) || lstrlenA((char*)imei) > IMEI_IMSI_PHONE_SIZE) {
		return FALSE;
	}

	for (int i = 0; i < lstrlenA((char*)imei); i++)
	{
		unsigned int d = imei[i];
		if ((d >= '0' && d <= '9') || (d >= 'a' && d <= 'f') || (d >= 'A' && d <= 'F'))
		{
			continue;
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}



int Additional::checkUserName(unsigned char * username) {
	if (*username == 0  ) {
		return FALSE;
	}

	for (int i = 0; i < lstrlenA((char*)username); i++)
	{
		if (isalpha(username[i]) || isdigit(username[i]))
		{
			continue;
		}
		else {
			return FALSE;
		}
	}

	return TRUE;
}



string Additional::getCmdFileName(string curpath,string name,string imei) {

	string filename = curpath + "\\" + COMMAND_DIR_NAME + "\\" + imei  + "_" + name;
	return filename;
}


string Additional::getDownloadFileName(string curpath, string name) {

	string filename = curpath + "\\" + DOWNLOAD_FILE_DIR_NAME + "\\" + name;
	return filename;
}


string Additional::getUploadFileName(string curpath, string name) {

	string filename = curpath + "\\" + UPLOAD_FILE_DIR_NAME + "\\" + name;
	return filename;
}


string Additional::getConfigFileName(string curpath, string imei,string username,string cfgfname) {

	string filename = curpath + "\\" + username + "\\" + imei + "\\" + cfgfname;
	return filename;
}


