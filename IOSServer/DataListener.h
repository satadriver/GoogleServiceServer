#pragma once

#ifndef DATALISTENER
#define DATALISTENER

#include <windows.h>
#include <iostream>


using namespace std;

class DataListener {
public:
	static DWORD __stdcall DataListener::DataProcessListener();

};
#endif