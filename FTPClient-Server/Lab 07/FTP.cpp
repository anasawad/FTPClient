#ifndef FTP_CPP_
#define FTP_CPP_
#include "FTP.h"


FTP::FTP()
{
	connectSocket = INVALID_SOCKET;
	dataSocket = INVALID_SOCKET;
	//rcvdBuffer = "";
	initialize();
	srvrIP = "127.0.0.1";
	InitSocket(connectSocket,srvrIP,FTP_PORT,hints, result);

	u_long iMode = 1;
	ioctlsocket(connectSocket,FIONBIO, &iMode);
}

FTP::FTP(char* srvr)
{
	connectSocket = INVALID_SOCKET;
	dataSocket = INVALID_SOCKET;
	initialize();
	srvrIP = srvr;
	InitSocket(connectSocket,srvrIP,FTP_PORT,hints, result);
}

void FTP::initialize()
{
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) 
		printf("WSAStartup failed with error: %d\n", iResult);


	ZeroMemory( &hints, sizeof(hints) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM; 
	hints.ai_protocol = IPPROTO_TCP;
}


int FTP::InitSocket(SOCKET &connectSocket, char* serverIP, char* portNumber, addrinfo hints,addrinfo* result)
{
	struct addrinfo *ptr = NULL;
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


int FTP::parsePort(char* data)
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

void FTP::cmdExecuter(const char* cmd)
{
	send(connectSocket, cmd, (int)strlen(cmd), 0 );
	Sleep(800);


	if(strncmp(cmd, "RETR", 4) == 0)
	{
		
		do
		{
			iResult = recv(dataSocket, rcvdBuffer, DEFAULT_BUFLEN, 0); 
			if(iResult > 0)
			{
				rcvdBuffer[iResult] = '\0';
				printf("%s", rcvdBuffer);
			}
		}while(iResult > 0);

		 
	}
	else
		iResult = recv(connectSocket, rcvdBuffer, DEFAULT_BUFLEN, 0);

	

	if(iResult > 0)
	{
		rcvdBuffer[iResult] = '\0';
		printf("%s\n", rcvdBuffer);
	}
	
	
	if(strncmp(cmd, "PASV", 4) == 0)
	{
		passiveMode();
	}
}

void FTP::passiveMode()
{
	char* newPort = new char[4];
	iResult = parsePort(rcvdBuffer);
	newPort = itoa(iResult,newPort,10);

	InitSocket(dataSocket,srvrIP,newPort,hints, result);	
}


FTP::~FTP()
{
	printf("Done !!... \n");
	closesocket(connectSocket);
	closesocket(dataSocket);
	WSACleanup();
}

#endif