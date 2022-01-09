#pragma once

#include <vector>

using namespace std;

class XMLParser {
public:
	static int XMLParser::getvalue(char * str, char * name,char * dst,int * dstlen,int base64flag);
	static vector<string> XMLParser::getvalues(char * str, char * node, vector<string>names,int utf8flag);
};