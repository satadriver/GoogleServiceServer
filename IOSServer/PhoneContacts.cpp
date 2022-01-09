#include "PhoneContacts.h"
#include "jsonParser.h"

int PhoneContacts::splitContacts(string input,string & name, string &phone) {
	name = JsonParser::getStringValue(input, "name");
	phone = JsonParser::getStringValue(input, "phone");
	return 0;
}