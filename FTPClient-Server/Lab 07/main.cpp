#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <algorithm>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN	512
#define FTP_PORT		"21"

static char dataPort[DEFAULT_BUFLEN];
int Reverse(int num);
void SendRequest(SOCKET, char*, char*,bool);
void SendRequest(SOCKET, char*, char*,bool, char*);
void SendRecv(SOCKET, const char*, char*, int, bool);
void SendRecv(SOCKET, const char*, char*, int, bool, char*);
int InitSocket(SOCKET&,char*, char*, addrinfo,addrinfo*);
int parsePort(char*);


void test(SOCKET initializedSocket, const char* sendBuffer, char* rcvdBuffer)
{
	send( initializedSocket, sendBuffer, (int)strlen(sendBuffer), 0 );
	Sleep(800);
	int r = recv(initializedSocket, rcvdBuffer, (int)strlen(rcvdBuffer), 0);
	rcvdBuffer[r] = '\0';
	
}

int __cdecl main()
{
	WSADATA			wsaData;
	SOCKET			connectSocket = INVALID_SOCKET;
	SOCKET			dataSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL;
	struct addrinfo *result1 = NULL;
	struct addrinfo *ptr = NULL;
	struct addrinfo	hints;
	char			szUsername[55];
	char			szPassword[55];
	char			*szServerNameIP;
	char			*Buffer;
	char			*request;
	char			temp[100];
	int				iResult;
	int				iResult1;


	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory( &hints, sizeof(hints) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM; 
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	szServerNameIP = "127.0.0.1";
	InitSocket(connectSocket,szServerNameIP,FTP_PORT,hints, result);

	//Logging .. after the socket is ready 

	printf("\nLogging ... \n");
	Buffer = new char[DEFAULT_BUFLEN];
	printf("Username: ");
	scanf("%s", &szUsername);

	char* user = "USER ";
	int len;
	strcpy(temp, user);
	strcat(temp, szUsername);
	strcat(temp, "\r\n");
	SendRequest(connectSocket,temp,Buffer,false);

	printf("Password: ");
	scanf("%s", &szPassword);
	char* pass = "PASS ";
	strcpy(temp, pass);
	strcat(temp, szPassword);
	strcat(temp, "\r\n");
	SendRequest(connectSocket,temp,Buffer,false);

	printf("\nlogged in successfully...\n");

	request = "PWD\r\n";
	SendRequest(connectSocket,request,Buffer,false);

	request = "CWD /\r\n";
	SendRequest(connectSocket,request,Buffer,false);

	request = "  I\r\n";
	SendRequest(connectSocket,request,Buffer,false);


	//Entering Passive Mode ... 
	/*
	If we are working in passive mode we need 2 socket one for data and the other one for request 
	*/
	request = "PASV\r\n";
	test(connectSocket, request, Buffer);
	printf("%s\n", Buffer);
	int n = parsePort(Buffer);
	printf("%d\n", n);
	char* newPort = new char[4];
	newPort = itoa(n,newPort,10);

	InitSocket(dataSocket,szServerNameIP,newPort,hints, result);

	request = "SIZE kokoz.txt\r\n";
	test(connectSocket, request, Buffer);
	printf("%s\n", Buffer);
	//SendRequest(connectSocket,request,Buffer,false);

	request = "RETR kokoz.txt\r\n";
	test(connectSocket, request, Buffer);
	printf("%s\n", Buffer);
	//SendRequest(connectSocket,request,Buffer,false);


	printf("Done !!... \n");
	closesocket(connectSocket);
	WSACleanup();
	return 0;
}

int parsePort(char* data)
{
	int n, sz = strlen(data), count =0, i, j;
	char* n1 = new char[5];
	char* n2 = new char[5];

	//i = find(data, data+sz, '>');
	
	//192,168,1,1,80,80
	for(i = 0 ; i < sz ; i++)
	{
		if(data[i] != ',' && count > 3)
		{
			break;
		}	

		if(data[i] == ',')
			count++;
		
	}

	j=0;
	while(data[i] != ',')
		n1[j++] = data[i++];
	
	n1[j] = '\0';

	j=0;
	i++;

	while(data[i] != ')')
		n2[j++] = data[i++];
	n2[j] = '\0';

	
	int port[2];
	port[0] = atoi(n1);
	port[1] = atoi(n2);
	
	n = port[0]*256 + port[1];
	return n;
}

int Reverse(int num)
{
	int new_num = 0;
	while(num > 0)
	{
		new_num = new_num*10 + (num % 10);
		num = num/10;
	}
	return new_num;
}



void SendRequest(SOCKET connectSocket, char* request,char* Buffer,bool isData)
{
	int len;
	//len = (int)strlen(request);
	char* x;
	SendRecv(connectSocket,request,Buffer,DEFAULT_BUFLEN,isData);
	Sleep(500);//mo
}

void SendRequest(SOCKET connectSocket, char* request,char* Buffer,bool isData, char* dataRcvd)
{
	int len;
	//len = (int)strlen(request);

	SendRecv(connectSocket,request,Buffer,DEFAULT_BUFLEN,isData, dataRcvd);
	Sleep(500);//mo
}

void SendRecv(SOCKET initializedSocket, const char* szBufferToSend, char* szBufferToFill, int iBufferLen, bool isData)
{
	//printf("isData = %d\n", isData);
	int iResult;
	iResult = send( initializedSocket, szBufferToSend, (int)strlen(szBufferToSend), 0 );
	if (iResult == SOCKET_ERROR) {
		wprintf(L"send failed with error: %d\n", WSAGetLastError());
		closesocket(initializedSocket);
		WSACleanup();
		return;
	}
	printf("Bytes Sent: %ld\n", iResult);

	//Activate communication unlock mode 
	u_long iMode = 1;
	ioctlsocket(initializedSocket,FIONBIO, &iMode);

	int count =0 ;
	do 
	{
		iResult = recv(initializedSocket, szBufferToFill, iBufferLen, 0);
		//printf("iResult: %d\n", iResult);

		if (iResult > 0)
		{
			//printf("iResult>0\n");
			if(isData)
			{
				printf("isData = true\n");
				char* arr = new char[DEFAULT_BUFLEN];
				//Sleep(1000);
				for (int i =0 ; i < iResult; i++)
				{
					arr[count] = szBufferToFill[i];
					//dataPort[count] = szBufferToFill[i];//[iResult];
					//++szBufferToFill;
					count++;
				}
			}
			szBufferToFill[iResult] = '\0';

			printf("Bytes received: %d\n", iResult);
			continue;
		}
		else if(iResult == 0)
		{
			printf("Connection Closed...\n");break;
		}
	} while (iResult > 0);
}



void SendRecv(SOCKET initializedSocket, const char* szBufferToSend, char* szBufferToFill, int iBufferLen, bool isData, char* dataRcvd)
{
	//printf("isData = %d\n", isData);
	int iResult;
	iResult = send( initializedSocket, szBufferToSend, (int)strlen(szBufferToSend), 0 );
	if (iResult == SOCKET_ERROR) {
		wprintf(L"send failed with error: %d\n", WSAGetLastError());
		closesocket(initializedSocket);
		WSACleanup();
		return;
	}
	printf("Bytes Sent: %ld\n", iResult);

	//Activate communication unlock mode 
	u_long iMode = 1;
	ioctlsocket(initializedSocket,FIONBIO, &iMode);

	int count =0 ;
	do 
	{
		iResult = recv(initializedSocket, szBufferToFill, iBufferLen, 0);
		//printf("iResult: %d\n", iResult);

		if (iResult > 0)
		{
			//printf("iResult>0\n");
			if(isData)
			{
				printf("isData = true\n");
				
				for (int i =0 ; i < iResult; i++)
				{
					dataRcvd[i] = szBufferToFill[i];
					count++;
				}
			}
			dataRcvd[iResult] = '\0';
			szBufferToFill[iResult] = '\0';

			printf("Bytes received: %d\n", iResult);
			continue;
		}
		else if(iResult == 0)
		{
			printf("Connection Closed...\n");break;
		}
	} while (iResult > 0);
}


int InitSocket(SOCKET &connectSocket, char* serverIP, char* portNumber, addrinfo hints,addrinfo* result)
{
	struct addrinfo *ptr = NULL;
	int iResult;
	iResult = getaddrinfo(serverIP, portNumber, &hints, &result);
	if ( iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for(ptr=result; ptr != NULL ;ptr=ptr->ai_next)
	{
		// Create a SOCKET for connecting to server
		connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (connectSocket == INVALID_SOCKET )
		{
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}
		// Connect to server.
		iResult = connect( connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(connectSocket);
			connectSocket = INVALID_SOCKET;
			continue;
		}
		printf("Socket Initialized, it's ready now ... \n");
		break;
	}

	freeaddrinfo(result);
	if (connectSocket == INVALID_SOCKET )
	{
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}
	return iResult;
}