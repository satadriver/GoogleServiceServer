#pragma once

#include <windows.h>
#include <iostream>

using namespace std;

class NetworkHelper {
public:
	static int listener(int port);

	static DWORD getIPFromName(string host);
};