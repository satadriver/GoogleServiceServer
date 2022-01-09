#include "jsonParser.h"
#include <string>

using namespace std;

string JsonParser::getQuotaStringValue(string data, string key) {
	string findkey = string("\"") + key + "\"";
	int pos = data.find(findkey);
	if (pos >= 0)
	{
		int nowpos = pos + findkey.length();
		int next = data.find(":", nowpos);
		if (next >= 0)
		{
			next++;
			int nnext = data.find("\"\\\"", next);	//"WIFI":"\"dlink\""
			if (nnext >= 0)
			{
				nnext += 3;
				int nnnext = data.find("\\\"\"", nnext);
				if (nnnext >= 0)
				{
					return data.substr(nnext, nnnext - nnext);
				}
			}
		}
	}

	return "";
}


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
	int pos = data.find(findkey);
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




vector<string> *JsonParser::getJsonFromArray(string substr) {
	//vector <string> ret(0x10000);			//size is 0x10000
	vector <string> *ret = new vector<string>;
	//ret->reserve(0x10000);		//size is real size
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
				if (value.length() > 2)
				{
					ret->push_back(value);
				}
				
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


vector<string> *JsonParser::splitFromEnter(string substr) {

	//vector <string> ret(0x10000);
	vector <string> *ret = new vector<string>;
	//ret->reserve(0x10000);
	//ret.clear();

	while (1)
	{
		int pos = substr.find("\r\n");
		if (pos >= 0)
		{
			string value = substr.substr(0,pos);
			if (value.length() > 0)
			{
				ret->push_back(value);
			}
			
			substr = substr.substr(pos + 2);
		}
		else {
			break;
		}
	}

	return ret;
}