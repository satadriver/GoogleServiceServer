
#include "BaseSocket.h"
#include "BaseSocket.h"
#include <WinSock2.h>
#include <stdio.h>



SOCKET BaseSocket::listenPort(unsigned long ip, unsigned int usPort) {

	int iRet = 0;
	int sockListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockListen < 0)
	{
		printf("port:%u socket error\n", usPort);
		return -1;
	}

	sockaddr_in saListen = { 0 };
	saListen.sin_addr.s_addr = ip;
	saListen.sin_family = AF_INET;
	saListen.sin_port = ntohs(usPort);

	int bOptval = 1;
	iRet = setsockopt(sockListen, SOL_SOCKET, SO_REUSEADDR, (char *)&bOptval, sizeof(bOptval));
	if (iRet < 0) {
		closesocket(sockListen);
		printf("port:%u setsockopt error\n", usPort);
		return -1;
	}


	iRet = bind(sockListen, (sockaddr*)&saListen, sizeof(sockaddr_in));
	if (iRet < 0)
	{
		perror("bind");
		closesocket(sockListen);
		printf("port:%u bind error\n", usPort);
		return -1;
	}

	iRet = listen(sockListen, MAX_LISTEN_COUNT);
	if (iRet < 0)
	{
		closesocket(sockListen);
		printf("port:%u listen error\n", usPort);
		return -1;
	}

	return sockListen;

}



SOCKET BaseSocket::listenPort(unsigned int usPort) {

	int iRet = 0;
	int sockListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockListen < 0)
	{
		printf("port:%u socket error\n", usPort);
		return -1;
	}

	sockaddr_in saListen = { 0 };
	saListen.sin_addr.s_addr = INADDR_ANY;
	saListen.sin_family = AF_INET;
	saListen.sin_port = ntohs(usPort);

	// 	int bOptval = 1;
	// 	iRet = setsockopt(sockListen, SOL_SOCKET, SO_REUSEADDR, (char *)&bOptval, sizeof(bOptval));
	// 	if (iRet < 0) {
	// 		closesocket(sockListen);
	// 		printf("port:%u setsockopt error\n", usPort);
	// 		return -1;
	// 	}


	iRet = bind(sockListen, (sockaddr*)&saListen, sizeof(sockaddr_in));
	if (iRet < 0)
	{
		perror("bind");
		closesocket(sockListen);
		printf("port:%u bind error\n", usPort);
		return -1;
	}

	iRet = listen(sockListen, MAX_LISTEN_COUNT);
	if (iRet < 0)
	{
		closesocket(sockListen);
		printf("port:%u listen error\n", usPort);
		return -1;
	}

	return sockListen;

}



SOCKET BaseSocket::connectServer(unsigned long ip, int usPort) {
	int iRet = 0;
	int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET)
	{
		printf("port:%u socket error\n", usPort);
		return -1;
	}

	sockaddr_in sa = { 0 };
	sa.sin_addr.S_un.S_addr = ip;
	sa.sin_family = AF_INET;
	sa.sin_port = ntohs(usPort);

	iRet = connect(s, (sockaddr*)&sa, sizeof(sockaddr_in));
	if (iRet < 0)
	{
		closesocket(s);
		printf("port:%u listen error\n", usPort);
		return -1;
	}

	return s;
}