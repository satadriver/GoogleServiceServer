#pragma once

#include <iostream>

using namespace std;

typedef struct {
	string filepath;
	string filename;
	string username;
	string imei;
}RECOMMITDATA, *LPRECOMMITDATA;

class DataKeeper {
public:
	static int keep(string filepath, string name, const char * data, int size, string username, string imei);

	static int __stdcall DataKeeper::recommit();

	static int add(RECOMMITDATA);

	static int removeClient(string imei);
	static int removeUser(string username);

	static void getpath(void);
	static void findfiles(string path,string username,string imei);
};