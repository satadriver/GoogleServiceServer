#pragma once
#ifndef COMMANDLISTENER
#define COMMANDLISTENER

#include <windows.h>
#include "iosServer.h"
#include "PublicFunction.h"
#include "CommandProcess.h"


class CommandListener {
public:

	static DWORD __stdcall CommandListener::Commandlistener();
};

#endif
