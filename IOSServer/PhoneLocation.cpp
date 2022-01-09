#include "PhoneLocation.h"
#include "jsonParser.h"

int PhoneLocation::splitLocation(string locinfo, string &x, string & y, string &info, string &strtime) {
	info = JsonParser::getStringValue(locinfo,"info");
	strtime = JsonParser::getStringValue(locinfo, "time");
	x = JsonParser::getBaseValue(locinfo, "latitude");
	y = JsonParser::getBaseValue(locinfo, "longitude");

	return 0;
}