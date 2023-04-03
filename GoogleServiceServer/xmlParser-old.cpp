
#include <windows.h>
#include <iostream>
#include "xmlParser.h"
#include <stdarg.h>
#include "Base64.h"
#include "Coder.h"
using namespace std;






int XMLParser::getvalue(char * str,char * name,char *dst,int * dstlen,int base64flag) {
	int strsize = lstrlenA(str);

	string h = string("<") + (name) + ">";
	
	int cmpsize = strsize - h.length();

	for (int i = 0;i < cmpsize; i++)
	{
		if (memcmp(str+i,h.c_str(),h.length()) == 0)
		{
			i += h.length();

			string e = string("</") + (name) + ">";
			cmpsize = strsize - e.length();
			for (int j = i; j < cmpsize; j++)
			{
				if (memcmp(str + j , e.c_str(), e.length()) == 0)
				{
					int len = j - i;
					memcpy(dst, str + i, len);
					*(dst + len) = 0;

					if (base64flag)
					{
						len = Base64::Base64Decode(dst, dst, len);
						if (len )
						{
// 							char * utf8 = 0;
// 							len = Coder::GBKToUTF8((const char*)dst, &utf8);
// 							if (len > 0)
// 							{
// 								memcpy(dst, utf8, len);
// 								*(dst + len) = 0;
// 								delete utf8;
// 							}
						}
					}
					*dstlen = len;
					return j + e.length();
				}
			}
		}
	}
	return 0;
}



vector<string> XMLParser::getvalues(char * str, char * node, vector<string>names,int utf8flags) {

	vector<string> vr;

	int result = 0;

	char arrays[1024];
	int arrayslen = 0;
	

	char item[1024];
	int itemsize = 0;

	char*buf = str;

	int pos = 0;
	
	while(pos = getvalue(buf, node, arrays,&arrayslen,FALSE))
	{
		if (pos > 0)
		{
			buf += pos;

			char *subarray = arrays;
			for (int i = 0; i< names.size();i ++)
			{
				itemsize = 0;

				int subarraylen = getvalue(subarray,(char*) names[i].c_str(), item,&itemsize,FALSE);

				//subarray += subarraylen;

				result = Base64::Base64Decode(item, item, itemsize);
				if (result > 0)
				{
					if (utf8flags)
					{
						char * utf8 = 0;
						int len = Coder::GBKToUTF8((const char*)item, &utf8);
						if (len > 0)
						{
							memcpy(item, utf8, len);
							*(item + len) = 0;
							delete utf8;
						}
					}
					vr.push_back(item);
				}
			}		
		}
	}

	return vr;
}