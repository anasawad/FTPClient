#ifndef FTP_CPP_
#define FTP_CPP_
#include "FTP.h"

/*constructor ya Anas :P :D*/
FTP::FTP()
{
	newPort = NULL;
	mode = SEND_MODE::NOT_ASSIGNED;
	connectSocket = INVALID_SOCKET;
	sendSocket = new SOCKET();
	initialize();
	srvrIP = "127.0.0.1";
	InitSocket(connectSocket,srvrIP,FTP_PORT,hints, result);
	getAuthentication();
}

/*da ba2a constructor brdo ya Anas :P :D*/
FTP::FTP(char* srvr)
{
	connectSocket = INVALID_SOCKET;
	sendSocket = new SOCKET();
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
		if(mode != SEND_MODE::ACTV)
		{
			iResult = connect( connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
			if (iResult == SOCKET_ERROR)
			{
				closesocket(connectSocket);
				connectSocket = INVALID_SOCKET;
				continue;
			}
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


void FTP::getPortFromMsg(char* address, int port[])
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


	while(isdigit(address[i]))//getting second port number
		n2[j++] = address[i++];
	n2[j] = '\0';


	
	port[0] = atoi(n1);
	port[1] = atoi(n2);
}

/*
executing commands
*/
void FTP::cmdExecuter(char* cmd)
{
	int len = strlen(cmd);
	//converting the command to lower case
	for(int i=0; i< len; i++)
	{
		cmd[i] = tolower(cmd[i]);
	}

	sendBuffer = new char[len];
	sendBuffer = strcpy(sendBuffer, cmd);

	//making stor command wihtout my path
	if(!strncmp(cmd, "stor", 4) )
	{
		int j=strlen(cmd);
		while(cmd[j--] != '\\'); //last '\'
		j++;
		while(cmd[j] != ' ')
			cmd[j--] = ' ';
	}

	send(connectSocket, cmd, len, 0);
	Sleep(800);

	if(!strncmp(cmd, "list", 4) || !strncmp(cmd, "nlst", 4))
	{
		
		if(mode == SEND_MODE::ACTV)
			*sendSocket = accept(tmpSocket, (struct sockaddr *)&their_addr, &addr_size);


		do
		{
			iResult = recv(*sendSocket, rcvdBuffer, DEFAULT_BUFLEN, 0); 

			if(iResult > 0)
				rcvdBuffer[iResult] = '\0', printf("%s", rcvdBuffer);

		}while(iResult > 0);

		printf("\n");
	}
	else if(!strncmp(cmd, "retr", 4) )
		downloadFile();
	else if(!strncmp(cmd, "stor", 4) )
		uploadFile();
	else
	{
		iResult = recv(connectSocket, rcvdBuffer, DEFAULT_BUFLEN, 0);
	
		if(iResult > 0)
			rcvdBuffer[iResult] = '\0', printf("%s\n", rcvdBuffer);
	}

	//if(mode == SEND_MODE::PASV)
	//	passiveMode();
	//else if(mode == SEND_MODE::ACTV)
	//	activeMode();
	//
	
	if(!strncmp(cmd, "pasv", 4))
	{
		mode = SEND_MODE::PASV;
		passiveMode();
	}
	else if(!strncmp(cmd, "port", 4) )
	{
		mode = SEND_MODE::ACTV;
		activeMode();
	}
}

/*
get file name from addresse
Directory\subDirectory\filename
*/

char* FTP::getFilename(char* rcvd)
{
	int i=0, j =strlen(rcvd);
	char* res = new char[j];

	while(rcvd[j] != '\\' //to last '\' (STOR Dirname\filename)
		&& rcvd[j] != ' ' // or got space (STOR filename)
		&& rcvd[j] != '/'
		)
		j--;
	
	j++;//begin after last '\\'
	while(rcvd[j] != '\r' && rcvd[j] != '\0')
		res[i++] = rcvd[j++];

	res[i] = '\0';
	return res;
}

/*
get the whole file address from STOR, RETR commands
(STOR || RETR) filename\r\n
e.g. RETR boda\anas\morsy\shika.txt
*/
char* FTP::getFileAddress(char *rcvd)
{
	int i=0, j =0;
	char* res = new char[strlen(rcvd)];

	while(rcvd[i++] != ' '); // go to first space

	while(rcvd[i] != '\r')
		res[j++] = rcvd[i++];

	res[j] = '\0';

	return res;
}

char* FTP::getFileExtension(char* rcvd)
{
	int i=0, j=0;
	char* ext = new char[strlen(rcvd)];
	while(rcvd[i++] != '.');

	while(rcvd[i] != '\r' && rcvd[i] != '\0')
		ext[j++] = rcvd[i++];

	ext[j] = '\0';

	return ext;
}
/*
getting file name from RETR filename command, make a new file with this name locally
and write data inside it
*/
void FTP::downloadFile()
{
	char* addr = getFileAddress(sendBuffer);
	char* name = getFilename(addr);
	char* ext = getFileExtension(name);
	FILE *f;
	
	if(!strcmp(ext, "pcx") ||  !strcmp(ext, "xlsx"))
		f = fopen(name, "wb");
	else
		f = fopen(name, "w");

	if(mode == SEND_MODE::ACTV)
		*sendSocket = accept(tmpSocket, (struct sockaddr *)&their_addr, &addr_size);

	
	int sz = 0;
	do
	{
		iResult = recv(*sendSocket, rcvdBuffer, DEFAULT_BUFLEN, 0); 

		if(iResult > 0)
		{
			//sz+=iResult;
			rcvdBuffer[iResult] = '\0';
			printf("%s", rcvdBuffer);

			fwrite(rcvdBuffer, sizeof(char), iResult, f);
		}

	}while(iResult > 0);

	printf("\n");

	fclose(f);
}

/*
getting file name from STOR filename command and upload it
*/
void FTP::uploadFile()
{
	char* addr = getFileAddress(sendBuffer);//
	char* name = getFilename(addr);
	char *ext = getFileExtension(name);
	bool pcx = !strcmp(ext, "pcx") ||  !strcmp(ext, "xlsx");
	FILE *f;

	if(pcx)
		f = fopen(name, "rb");
	else
		f = fopen(name, "r");

	if(mode = SEND_MODE::ACTV)
		*sendSocket = accept(tmpSocket, (struct sockaddr *)&their_addr, &addr_size);

	
	int i=0;
	
	while(!feof(f))
	{
		getc(f);
		i++;
	}

	i--;
	char* tmp = new char[i];
	fclose(f);

	if(pcx)
		f = fopen(name, "rb");
	else
		f = fopen(name, "r");

	fread(tmp, sizeof(char), i, f);
	tmp[i] = '\0';

	//uploading to FTP
	send(*sendSocket, tmp, i, 0);

	fclose(f);
}

/*
initializes the sendSocket for exchanging data
*/
void FTP::passiveMode()
{
	//char* newPort = new char[4];
	//if(newPort == NULL)
	//{
		newPort = new char[5];
		int port[2];
		getPortFromMsg(rcvdBuffer, port);
		iResult = port[0]*256 + port[1];
		newPort = itoa(iResult,newPort,10);
	//}
	sendSocket = new SOCKET;
	InitSocket(*sendSocket,srvrIP,newPort,hints, result);	
}
 
void FTP::activeMode()
{
	//char* newPort = new char[4];
	
//	if(newPort == NULL)
	//{
		newPort = new char[5];
		int port[2];
		getPortFromMsg(sendBuffer, port);
		iResult = port[0]*256 + port[1];
		newPort = itoa(iResult,newPort,10);
		getaddrinfo(srvrIP, newPort, &hints, &result);
		tmpSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	//}

	

	bind(tmpSocket, result->ai_addr, result->ai_addrlen);
	listen(tmpSocket, 5);
	addr_size = sizeof (their_addr);
    //*sendSocket = accept(tmp, (struct sockaddr *)&their_addr, &addr_size);
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
	closesocket(*sendSocket);

	WSACleanup();
}

#endif