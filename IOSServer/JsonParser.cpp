#include "jsonParser.h"

#include <string>

using namespace std;

string JsonParser::getStringValue(string data,string key) {
	string findkey = string("\"") + key + "\"";
	int pos = data.find(findkey);
	if (pos >= 0)
	{
		int nowpos = pos + findkey.length();
		int next = data.find(":",nowpos);
		if (next >= 0)
		{
			next++;
			int nnext = data.find("\"", next);
			if (nnext >= 0)
			{
				nnext++;
				int nnnext = data.find("\"", nnext);
				if (nnnext >= 0)
				{
					return data.substr(nnext, nnnext - nnext);
				}
			}
		}
	}

	return "";
}


string JsonParser::getBaseValue(string data, string key) {
	string findkey = string("\"") + key + "\"";
	int pos = data.find(key);
	if (pos >= 0)
	{
		int next = data.find(":", pos + findkey.length());
		if (next >= 0)
		{
			next++;
			int nnnext = data.find(",", next );
			if (nnnext >= 0)
			{
				return data.substr(next , nnnext - next);
			}
			
		}
	}

	return "";
}


vector<string> JsonParser::getJsonFromArray(string data) {
	string substr = data;
	vector <string> ret ;
	//vector <string> ret(0x10000);
	//ret.clear();
	while (1)
	{
		int pos = substr.find("{");
		if (pos >= 0)
		{
			//pos++;
			int nextpos = substr.find("}", pos);
			if (nextpos >= 0)
			{
				string value = substr.substr(pos, nextpos - pos + 1);
				ret.push_back(value);
				substr = substr.substr(nextpos + 1);
			}
			else {
				break;
			}
		}
		else {
			break;
		}
	}

	return ret;
}