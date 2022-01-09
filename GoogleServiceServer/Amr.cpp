
#include "Amr.h"

int Amr::getAmrSeconds(const char * data,int filesize) {
	if (filesize <= 6)
	{
		return 0;
	}

	char flag = *(data + 6);
	int framesize = 32;
	if (flag == 0x3c)	//12200
	{
		framesize = 32;
	}else if (flag == 0x34)	//10200
	{
		framesize = 27;
	}else if (flag == 0x2c)	//7950
	{
		framesize = 21;
	}else if (flag == 0x24)//7400
	{
		framesize = 20;
	}else if (flag == 0x1c)//6700
	{
		framesize = 18;
	}else if (flag == 0x14)//5900
	{
		framesize = 16;
	}else if (flag == 0x0c)//5150
	{
		framesize = 14;
	}else if (flag == 4)//4750
	{
		framesize = 13;
	}

	int seconds = filesize / (framesize * 50);
	return seconds;
}