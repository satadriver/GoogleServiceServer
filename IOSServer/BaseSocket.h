#pragma once


#include <iostream>
#include <winsock2.h>



using namespace std;


class BaseSocket {
public:
	static SOCKET BaseSocket::listenPort(unsigned long ip, unsigned int usPort);

	static SOCKET BaseSocket::listenPort(unsigned int usPort);

	static SOCKET BaseSocket::connectServer(unsigned long ip, int usPort);

};