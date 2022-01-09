#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;


typedef struct {
	string filepath;
	string filename;
	string imei;
	string username;
}KEEPDATA,*LPKEEPDATA;

class DataKeeper {
public:

	static int DataKeeper::add(KEEPDATA data);
	static int keep(string filepath,string name,const char * data,int len,string username, string imei);
	static int __stdcall DataKeeper::recommit();

	static int init();
};