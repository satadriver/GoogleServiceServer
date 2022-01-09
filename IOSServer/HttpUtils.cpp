
#include "HttpUtils.h"


string HttpUtils::getFileNameFromUrl(const char * lppacket, int len) {
	string url = "";

	int offset = 0;
	if (memcmp(lppacket, "GET /", 5) == 0)
	{
		offset = 4;
	}
	else if (memcmp(lppacket, "POST /", 6) == 0) {
		offset = 5;
	}
	else {
		//return url;
	}

	const char *packhdr = lppacket + offset;

	char * lphdr = strstr((char*)packhdr, " HTTP/1.1\r\n");
	if (lphdr)
	{
		int urllen = lphdr - packhdr;
		url = string(packhdr, urllen);
	}
	else {
		lphdr = strstr((char*)packhdr, " HTTP/1.0\r\n");
		if (lphdr)
		{
			int urllen = lphdr - packhdr;
			url = string(packhdr, urllen);
		}
	}

	int pos = url.find("?");
	if (pos != -1)
	{
		url = url.substr(0, pos);
	}

	pos = url.find("&");
	if (pos != -1)
	{
		url = url.substr(0, pos);
	}
	return url;
}