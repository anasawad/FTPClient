#ifndef FTP_H_
#define FTP_H_H

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <fstream>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN	100000
#define FTP_PORT		"21"

enum SEND_MODE{ACTV, PASV, NOT_ASSIGNED};

class FTP
{
private:
	char rcvdBuffer[DEFAULT_BUFLEN];
	char *sendBuffer;	
	char *srvrIP;
	int iResult;
	SEND_MODE mode;
	char *newPort;

	struct sockaddr_storage their_addr;
    socklen_t addr_size;

	WSADATA			wsaData;
	SOCKET			connectSocket;
	SOCKET			tmpSocket;
	SOCKET			*sendSocket;
	struct addrinfo *result;//= NULL;
	struct addrinfo	hints;


	void initialize();
	int InitSocket(SOCKET&,char*, char*, addrinfo,addrinfo*);
	void getPortFromMsg(char*, int[]);
	void passiveMode();
	void activeMode();
	void getAuthentication();
	void uploadFile();
	void downloadFile();
	char* getFilename(char* rcvd);
	char* getFileAddress(char* rcvd);
	char* getFileExtension(char* rcvd);
public:
	FTP();
	FTP(char* srvrIP);
	void cmdExecuter(char*);
	~FTP();
};

#endif