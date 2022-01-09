#pragma once


#include <iostream>
#include <windows.h>

using namespace std;


class HttpUtils {
public:
	static string HttpUtils::getFileNameFromUrl(const char * lppacket, int len);
};