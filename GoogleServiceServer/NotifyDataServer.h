#pragma once
#include <windows.h>
#include <iostream>

using namespace std;


class NotifyDataServer {
public:
	static int __stdcall NotifyData(LPCMDNOTIFY lpnot);
};