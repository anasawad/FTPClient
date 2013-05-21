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


	/*Initialize WinSock2 library */
	void initialize();
	/*Initialize new Socket and connect to server*/
	int InitSocket(SOCKET&,char*, char*, addrinfo,addrinfo*);
	/*gets port number from response in PSV mode*/
	void getPortFromMsg(char*, int[]);
	/*Handling the passive mode and initializing data socket*/
	void passiveMode();
	/*Handling the active mode*/
	void activeMode();
	/*gets the user name and password required for the server connection*/
	void getAuthentication();
	/*getting file name from STOR filename command and upload it*/
	void uploadFile();
	/*getting file name from RETR filename command, make a new file with this name locally and write data inside it*/
	void downloadFile();
	/*get file name from addressee	Directory\subDirectory\filename*/
	char* getFilename(char* rcvd);
	/*get the whole file address from STOR, RETR commands*/
	char* getFileAddress(char* rcvd);
	/*gets the downloaded file extension from the server's response*/
	char* getFileExtension(char* rcvd);

public:
	FTP();
	FTP(char* srvrIP);
	void cmdExecuter(char*);
	~FTP();
};

#endif