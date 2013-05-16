#include "FTP.h"

#define LEN  512
int main()
{
	FTP ftp;
	char *cmd ="";
	char tmp[LEN];
	char tmp2[LEN];

	//---------------------------------//
	printf("Username: ");
	scanf("%s", &tmp);

	cmd = "USER ";
	strcpy(tmp2, cmd);
	strcat(tmp2, tmp);
	strcat(tmp2, "\r\n");

	ftp.cmdExecuter(tmp2);
	//---------------------------------//

	printf("Password: ");
	scanf("%s", tmp);
	cmd = "PASS ";
	
	strcpy(tmp2, cmd);
	strcat(tmp2, tmp);
	strcat(tmp2, "\r\n");

	ftp.cmdExecuter(tmp2);
	//---------------------------------//
	printf("\nlogged in successfully...\n");
	

	cmd = "PWD\r\n";
	ftp.cmdExecuter(cmd);

	cmd = "CWD /\r\n";
	ftp.cmdExecuter(cmd);

	//cmd = "  I\r\n";
	//ftp.cmdExecuter(cmd);

	//---------------------------------//
	
	//Entering Passive Mode ... 
	/*
	If we are working in passive mode we need 2 socket one for data and the other one for cmd 
	*/
	cmd = "PASV\r\n";
	ftp.cmdExecuter(cmd);
	
	cmd = "SIZE input.txt\r\n";
	ftp.cmdExecuter(cmd);

	cmd = "RETR input.txt\r\n";
	ftp.cmdExecuter(cmd);

}