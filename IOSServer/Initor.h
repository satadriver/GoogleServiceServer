#pragma once

#include <windows.h>


class Initor {
public:
	static int Initor::CheckIfProgramExist();
	static DWORD __stdcall Initor::OpenFireWallPort();
};