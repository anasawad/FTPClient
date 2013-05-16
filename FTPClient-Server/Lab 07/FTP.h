#ifndef FTP_H_
#define FTP_H_H

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN	512
#define FTP_PORT		"21"

class FTP
{
private:
	char rcvdBuffer[DEFAULT_BUFLEN];
	char *sendBuffer;	
	char *srvrIP;
	int iResult;

	WSADATA			wsaData;
	SOCKET			connectSocket;// = INVALID_SOCKET;
	SOCKET			dataSocket;// = INVALID_SOCKET;
	struct addrinfo *result;//= NULL;
	struct addrinfo	hints;


	void initialize();
	int InitSocket(SOCKET&,char*, char*, addrinfo,addrinfo*);
	void getPortFromResponse(char*, int[]);
	void passiveMode();
	void activeMode();
	void getAuthentication();
public:
	FTP();
	FTP(char* srvrIP);
	void cmdExecuter(const char*);
	~FTP();

};

#endif