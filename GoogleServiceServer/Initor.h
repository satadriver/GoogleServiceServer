#pragma once

#include <windows.h>


class Initor {
public:
	static int get_local_ip(int& ip);
	static int Initor::CheckIfProgramExist();
	static DWORD __stdcall Initor::OpenFireWallPort();

	static void sysmlinked();
};