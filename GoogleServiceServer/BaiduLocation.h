#pragma once
#include <iostream>

using namespace std;

class BaiduLocation {
public:
	static string getAddrFromLoc(string lat, string lon);
	static void BaiduLocation::initLocation();
};