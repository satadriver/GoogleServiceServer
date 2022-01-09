#include "BaiduLocation.h"
#include <WinSock2.h>
#include "jsonparser.h"
#include "PublicFunction.h"
#include "networkHelper.h"

#include <string>

using namespace std;

unsigned long dwHostIP = 0;

void BaiduLocation::initLocation() {
	
	if (dwHostIP == 0)
	{
		string host = "api.map.baidu.com";
		dwHostIP = NetworkHelper::getIPFromName(host);
	}
	
	return;
}



//api.map.baidu.com/geocoder/v2/?callback=renderReverse&location=36.811953,118.283757&output=json&pois=1&ak=wWy2A8K94nhntYTYUHS19RXW
string BaiduLocation::getAddrFromLoc(string lat, string lon) {

	__try {
		int ret = 0;

		if (dwHostIP == 0)
		{
			initLocation();
			if (dwHostIP == 0)
			{
				WriteLogFile("get baidu host error\r\n");
				return "";
			}
		}

		int s = socket(AF_INET, SOCK_STREAM, 0);
		if (s <= 0)
		{
			return "";
		}

		sockaddr_in sa = { 0 };
		sa.sin_family = AF_INET;
		sa.sin_port = ntohs(80);
		sa.sin_addr.S_un.S_addr = dwHostIP;
		ret = connect(s, (sockaddr*)&sa, sizeof(sockaddr_in));
		if (ret < 0)
		{
			closesocket(s);
			return "";
		}

		char szsendbuf[4096];
		string format =
			"GET /geocoder/v2/?callback=renderReverse&location=%s,%s&output=json&pois=1&ak=wWy2A8K94nhntYTYUHS19RXW HTTP/1.1\r\n"
			"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
			"Accept-Language: zh-CN\r\n"
			//"Upgrade-Insecure-Requests: 1\r\n"
			//"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/64.0.3282.140 Safari/537.36 Edge/18.17763\r\n"
			//"Accept-Encoding: deflate\r\n"	//do not use gzip
			"Host: api.map.baidu.com\r\n"
			"Connection: Keep-Alive\r\n\r\n";
		int sendlen = wsprintfA(szsendbuf, format.c_str(), lat.c_str(), lon.c_str());
		ret = send(s, szsendbuf, sendlen, 0);
		if (ret <= 0)
		{
			closesocket(s);
			return "";
		}

#define BAIDU_LOC_BUF_SIZE 0X4000
		char szrecvbuf[BAIDU_LOC_BUF_SIZE+1024];
		int recvlen = recv(s, szrecvbuf, BAIDU_LOC_BUF_SIZE, 0);
		closesocket(s);
		if (recvlen <= 0)
		{
			return "";
		}

		//数组如果满了得话引起越界异常
		*(szrecvbuf + recvlen) = 0;
		if (memcmp(szrecvbuf, "HTTP/1.1 200 OK", lstrlenA("HTTP/1.1 200 OK")))
		{
			return "";
		}

		string  addr = JsonParser::getStringValue(szrecvbuf, "formatted_address");
		string  desc = JsonParser::getStringValue(szrecvbuf, "sematic_description");
		if (desc != "")
		{
			return addr + "(" + desc + ")";
		}
		return addr;
	}
	__except(1) {
		WriteLogFile("getAddrFromLoc exception\r\n");
		return "";
	}

}


