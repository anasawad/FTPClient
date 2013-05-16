#ifndef FTP_CPP_
#define FTP_CPP_
#include "FTP.h"

/*constructor ya Anas :P :D*/
FTP::FTP()
{
	connectSocket = INVALID_SOCKET;
	dataSocket = INVALID_SOCKET;
	initialize();
	srvrIP = "127.0.0.1";
	InitSocket(connectSocket,srvrIP,FTP_PORT,hints, result);
	getAuthentication();
}

/*da ba2a constructor brdo ya Anas :P :D*/
FTP::FTP(char* srvr)
{
	connectSocket = INVALID_SOCKET;
	dataSocket = INVALID_SOCKET;
	initialize();
	srvrIP = srvr;
	InitSocket(connectSocket,srvrIP,FTP_PORT,hints, result);
	getAuthentication();
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

	u_long iMode = 1;
	ioctlsocket(connectSocket,FIONBIO, &iMode);

	return iResult;
}


void FTP::getPortFromResponse(char* address, int port[])
{
	int n, sz = strlen(address), count =0, i, j;
	char* n1 = new char[5];
	char* n2 = new char[5];


	//192,168,1,1,80,80
	for(i = 0 ; i < sz ; i++)//go to the third '4'
	{
		if(address[i] != ',' && count > 3)
		{
			break;
		}	

		if(address[i] == ',')
			count++;

	}

	j=0;
	while(address[i] != ',')//getting first port number
		n1[j++] = address[i++];

	n1[j] = '\0';

	j=0;
	i++;

	while(address[i] != ')')//getting second port number
		n2[j++] = address[i++];
	n2[j] = '\0';


	
	port[0] = atoi(n1);
	port[1] = atoi(n2);
}

/*
executing commands
*/
void FTP::cmdExecuter(const char* cmd)
{
	send(connectSocket, cmd, (int)strlen(cmd), 0 );
	Sleep(800);


	if(strncmp(cmd, "RETR", 4) == 0 || strncmp(cmd, "retr", 4) == 0 )
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
	
	
	if(strncmp(cmd, "PASV", 4) == 0 || strncmp(cmd, "pasv", 4) == 0)
	{
		passiveMode();
	}
}

/*
initializes the dataSocket for exchanging data
*/
void FTP::passiveMode()
{
	char* newPort = new char[4];
	int port[2];
	getPortFromResponse(rcvdBuffer, port);
	iResult = port[0]*256 + port[1];
	newPort = itoa(iResult,newPort,10);

	InitSocket(dataSocket,srvrIP,newPort,hints, result);	
}

/*gets user and password to connect to server*/
void FTP::getAuthentication()
{
	char tmp[DEFAULT_BUFLEN];
	char tmp2[DEFAULT_BUFLEN];
	char* cmd;

	//---------------------------------//
	printf("Username: ");
	scanf("%s", &tmp);

	cmd = "USER ";
	strcpy(tmp2, cmd);
	strcat(tmp2, tmp);
	strcat(tmp2, "\r\n");

	cmdExecuter(tmp2);
	//---------------------------------//

	printf("Password: ");
	scanf("%s", tmp);
	cmd = "PASS ";
	
	strcpy(tmp2, cmd);
	strcat(tmp2, tmp);
	strcat(tmp2, "\r\n");

	cmdExecuter(tmp2);
	//---------------------------------//
}

FTP::~FTP()
{
	printf("Done !!... \n");
	closesocket(connectSocket);
	closesocket(dataSocket);
	WSACleanup();
}

#endif