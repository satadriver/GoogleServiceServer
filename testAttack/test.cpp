#include <windows.h>
#include <string>
#include <WinSock.h>

#include<wininet.h>

#pragma comment(lib,"wininet.lib")

#include "include\\openssl\\ssl.h"
#include "include\\openssl\\err.h"

#pragma comment(lib,"ws2_32.lib")

#pragma comment(lib,"./lib\\libcrypto.lib")
#pragma comment(lib,"./lib\\libssl.lib")
#pragma comment(lib,"./lib\\openssl.lib")

#include <Nb30.h>
#pragma comment(lib, "netapi32.lib")

typedef struct _ASTAT_
{
	ADAPTER_STATUS adapt;

	NAME_BUFFER NameBuff[4096];

} ASTAT, *PASTAT;

using namespace std;

//https://dldir1.qq.com/weixin/android/wxweb/updateConfig.xml

DWORD getIPFromName(string host) {
	hostent * myhost = gethostbyname(host.c_str());
	if (myhost <= 0)
	{
		return 0;
	}

	ULONG  pPIp = *(DWORD*)((CHAR*)myhost + sizeof(hostent) - sizeof(DWORD_PTR));
	ULONG  pIp = *(ULONG*)pPIp;
	DWORD dwip = *(DWORD*)pIp;

	return dwip;
}

int fileWriter(string filename, const char * lpdate, int datesize) {
	int ret = 0;

	FILE * fp = fopen(filename.c_str(), "ab+");
	if (fp <= 0)
	{
		printf("fileReader fopen file:%s error\r\n", filename.c_str());
		return FALSE;
	}

	ret = fwrite(lpdate, 1, datesize, fp);
	fclose(fp);
	if (ret == FALSE)
	{
		return FALSE;
	}

	return datesize;
}

int ReadPendingData(char * lpdata, int size, SSL * ssl) {
	int ret = 0;
	int recvcnt = 0;

	int sslerror = 0;

	int accesssize = SSL_peek(ssl, lpdata + recvcnt, size - recvcnt);
	if (accesssize <= 0)
	{
		return recvcnt;
	}

	do
	{
		ret = SSL_read(ssl, lpdata + recvcnt, size - recvcnt);
		//SSL_get_error(ssl, ret) == SSL_ERROR_WANT_READ
		//pendsize = SSL_pending(ssl);	//always return 0
		sslerror = SSL_get_error(ssl, ret);
		if (sslerror != SSL_ERROR_NONE || ret <= 0)
		{
			return recvcnt;
		}
		else {
			recvcnt += ret;
		}
	} while (recvcnt < accesssize);


	int nextaccesssize = 0;
	if (accesssize == 1)
	{
		nextaccesssize = SSL_peek(ssl, lpdata + recvcnt, size - recvcnt);
		if (nextaccesssize > 0)
		{
			ret = SSL_read(ssl, lpdata + recvcnt, size - recvcnt);
			sslerror = SSL_get_error(ssl, ret);
			if (sslerror != SSL_ERROR_NONE || ret <= 0)
			{
				printf("ReadPendingData sencondary SSL_read error:%u\r\n", sslerror);
				return recvcnt;
			}
			else {
				recvcnt += ret;
			}
		}
	}

	if (recvcnt != accesssize + nextaccesssize)
	{
		printf("ReadPendingData result recvcnt:%u,accesssize:%u error\r\n", recvcnt, accesssize);
	}
	return recvcnt;
}

int test() {

	int ret = 0;



	string host = "update.browser.qq.com";

	DWORD ip = getIPFromName(host);

	sockaddr_in si = { 0 };
	si.sin_family = AF_INET;
	si.sin_port = ntohs(443);
	si.sin_addr.S_un.S_addr = ip;


	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


	ret = connect(s, (sockaddr*)&si, sizeof(sockaddr_in));
	if (ret )
	{
		printf("connect error:%u\r\n", WSAGetLastError());
		return -1;
	}
	char buf[0x8000] = { 0 };

	char * httpfmt = 
		//update.browser.qq.com/qbrowser
		//"GET /weixin/android/wxweb/updateConfig.xml HTTP/1.1\r\n"
		"POST /qbrowser HTTP/1.1\r\n"
		"User-Agent: Dalvik/2.1.0 (Linux; U; Android 6.0.1; OPPO A57 Build/MMB29M)\r\n"
		"Host: update.browser.qq.com\r\n"
		"Connection: Keep-Alive\r\n"
		"Accept-Encoding: gzip\r\n\r\n";

	ret = send(s, httpfmt, lstrlenA(httpfmt), 0);


	ret = recv(s, buf, 0x8000, 0);

	return 0;

}

#define SSL_BLOCK_TIME 3000

int testssl() {
	int iRet = 0;

	int sockToServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockToServer <= 0)
	{

		return FALSE;
	}


	string host = "dldir1.qq.com";

	DWORD dwip = getIPFromName(host);

	sockaddr_in saToServer = { 0 };
	saToServer.sin_addr.S_un.S_addr = dwip;
	saToServer.sin_port = ntohs(443);
	saToServer.sin_family = AF_INET;


	iRet = connect(sockToServer, (struct sockaddr *)&(saToServer), sizeof(sockaddr_in));
	if (iRet)
	{
		printf("SSLProxy connect server:%s,ip:%08x error:%u\r\n", host.c_str(),
			saToServer.sin_addr.S_un.S_addr, WSAGetLastError());
		return FALSE;
	}

	SSL_CTX *ctxToServer = SSL_CTX_new(SSLv23_client_method());
	//SSL_CTX *ctxToServer = SSL_CTX_new(TLSv1_2_client_method());
	if ((int)ctxToServer <= 0)
	{
		printf("%s SSLProxy server SSL_CTX_new error\n", host.c_str());
		return FALSE;
	}

	SSL *SSLToServer = SSL_new(ctxToServer);
	if ((int)SSLToServer <= 0)
	{
		printf("%s SSL_new server error\n", host.c_str());
		return FALSE;
	}

	iRet = SSL_set_fd(SSLToServer, sockToServer);
	if (iRet != 1)
	{
		printf("SSLProxy %s SSL_set_fd errorcode:%d,description:%s,return:%d\n", host.c_str(),
			SSL_get_error(SSLToServer, iRet), SSL_state_string_long(SSLToServer), iRet);
		return FALSE;
	}

	iRet = SSL_connect(SSLToServer);
	if (iRet != 1)
	{
		printf("SSLProxy %s SSL_connect errorcode:%d,description:%s,return:%d\n", host.c_str(),
			SSL_get_error(SSLToServer, iRet), SSL_state_string_long(SSLToServer), iRet);
		return FALSE;
	}


	char * httpfmt =
		"GET /weixin/android/wxweb/updateConfig.xml HTTP/1.1\r\n"
		//"POST /qbrowser HTTP/1.1\r\n"
		"User-Agent: Dalvik/2.1.0 (Linux; U; Android 6.0.1; OPPO A57 Build/MMB29M)\r\n"
		"Host: dldir1.qq.com\r\n"
		"Connection: Keep-Alive\r\n"
		"Accept-Encoding: gzip\r\n\r\n";

	iRet = SSL_write(SSLToServer, httpfmt, lstrlenA(httpfmt));
	if (iRet != lstrlenA(httpfmt))
	{
		printf("SSLProxy %s SSL_write errorcode:%d,description:%s,return:%d\n", host.c_str(),
			SSL_get_error(SSLToServer, iRet), SSL_state_string_long(SSLToServer), iRet);
		return FALSE;
	}

	char szrecvbuf[0x8000];

	iRet = ReadPendingData(szrecvbuf, 0x8000, SSLToServer);
	//iRet = SSL_read(SSLToServer, szrecvbuf, 0x8000);
	if (iRet)
	{
		fileWriter("test.txt", szrecvbuf, iRet);
	}
	


	return TRUE;
}




void testhttp()
{

	DWORD byteread = 0;
	char buffer[0x4000] = { 0 };

	HINTERNET internetopen;

	internetopen = InternetOpenA("Testing", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (internetopen == NULL)
	{
		return;
	}

	char * httpfmt =
		"GET /weixin/android/wxweb/updateConfig.xml HTTP/1.1\r\n"
		//"POST /qbrowser HTTP/1.1\r\n"
		"User-Agent: Dalvik/2.1.0 (Linux; U; Android 6.0.1; OPPO A57 Build/MMB29M)\r\n"
		"Host: dldir1.qq.com\r\n"
		"Connection: Keep-Alive\r\n"
		"Accept-Encoding: gzip\r\n\r\n";

	HINTERNET internetopenurl;
	internetopenurl = InternetOpenUrlA(internetopen, 
		//"http://b-api.ins.miaopai.com/client/plugin.json?plugininfo=[]&t=12664918&unique_id=df27269d-531f-38fc-9862-8d2432923efd&version=5.2.30&AndroidId=863857039555171&udid=15308E18AB5B71F8B56CA17D1450B8C1&channel=vivo_market", 
		"https://dldir1.qq.com",
		httpfmt,
		lstrlenA(httpfmt), INTERNET_FLAG_RELOAD, 0);
	if (internetopenurl == NULL)
	{
		printf("error:%u\r\n", WSAGetLastError());//12150
		goto there;
	}

	BOOL hwrite;
	DWORD written;
	HANDLE createfile;
	createfile = CreateFileA("http.txt", GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (createfile == INVALID_HANDLE_VALUE)
	{

		goto next;
	}

	BOOL internetreadfile;
	while (1)
	{
		internetreadfile = InternetReadFile(internetopenurl, buffer, sizeof(buffer), &byteread);
		if (byteread == 0)
			break;
		hwrite = WriteFile(createfile, buffer, byteread, &written, NULL);
		if (hwrite == 0)
		{
			goto here;
		}
	}

here:
	CloseHandle(createfile);
next:
	InternetCloseHandle(internetopenurl);
there:
	InternetCloseHandle(internetopen);

}



void get_mac(string &mac)
{

	NCB Ncb;

	ASTAT Adapter;

	UCHAR uRetCode;

	LANA_ENUM lenum;

	memset(&Ncb, 0, sizeof(Ncb));

	Ncb.ncb_command = NCBENUM;

	Ncb.ncb_buffer = (UCHAR *)&lenum;

	Ncb.ncb_length = sizeof(lenum);

	uRetCode = Netbios(&Ncb);

	for (int i = 0; i < lenum.length; i++)
	{

		memset(&Ncb, 0, sizeof(Ncb));

		Ncb.ncb_command = NCBRESET;

		Ncb.ncb_lana_num = lenum.lana[i];

		uRetCode = Netbios(&Ncb);

		memset(&Ncb, 0, sizeof(Ncb));

		Ncb.ncb_command = NCBFINDNAME;

		Ncb.ncb_lana_num = lenum.lana[i];

		strcpy((char *)Ncb.ncb_callname, "*");

		Ncb.ncb_buffer = (unsigned char *)&Adapter;

		Ncb.ncb_length = sizeof(Adapter);

		uRetCode = Netbios(&Ncb);

		if (uRetCode == 0)
		{
			char tmp[128];

			sprintf(tmp, "%02x-%02x-%02x-%02x-%02x-%02x",

				Adapter.adapt.adapter_address[0],

				Adapter.adapt.adapter_address[1],

				Adapter.adapt.adapter_address[2],
				Adapter.adapt.adapter_address[3],
				Adapter.adapt.adapter_address[4],
				Adapter.adapt.adapter_address[5]

			);

			mac = tmp;
		}
	}

	int i = 0;
	printf("ok");
	return;
}


int testncb() {
	int ret = 0;
	char buf[4096] = { 0 };
	NCB ncb = { 0 };
	ncb.ncb_command = NCBRESET;		// ÉèÖÃÖ´ÐÐNCBRESET
	ncb.ncb_lana_num = 0;
	ret = Netbios(&ncb);

	memset(&ncb, 0, sizeof(NCB));

	ncb.ncb_command = NCBFINDNAME;
	ncb.ncb_lana_num = 0;
	ncb.ncb_buffer = (unsigned char *)buf;
	ncb.ncb_length = 4096;
	ret = Netbios(&ncb);
	if (ret != NRC_GOODRET)
		exit(-1);


	return 0;
}


int main() {

	WSADATA wsa;
	WSAStartup(0x0202, &wsa);

	string mac = "";
	get_mac(mac);

	testncb();

	int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);


	char bs[50] = { 0x0,0x00,0x0,0x10,0x0,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x20,0x43,0x4b,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x0,0x0,0x21,0x0,0x1 };

	char buf[4096];


	sockaddr_in si = { 0 };
	si.sin_family = AF_INET;
	si.sin_port = ntohs(137);
	si.sin_addr.S_un.S_addr = inet_addr("192.168.1.7");


	int sendlen = sendto(s,bs, 50, 0, (sockaddr*)&si, sizeof(sockaddr));

	int recvaddrlen = sizeof(sockaddr);
	int recvlen = recvfrom(s, buf, 4096, 0, (sockaddr*)&si, &recvaddrlen);



	//DWORD ip = inet_addr("123456.789.abc.def");

	//testhttp();

	//test();
	testssl();

	return 0;
}