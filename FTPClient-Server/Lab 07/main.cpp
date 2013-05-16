#include "FTP.h"

#define LEN  512

void testRETR(FTP & ftp)
{
	char *cmd ="";	
	cmd = "PWD\r\n";
	ftp.cmdExecuter(cmd);

	cmd = "CWD /\r\n";
	ftp.cmdExecuter(cmd);


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

int main()
{
	FTP ftp;
	char cmd[LEN];
	int len;

	//testRETR(ftp);
	gets(cmd);
	gets(cmd);
	
	while(true)
	{
		
		len = strlen(cmd);
		cmd[len] = '\r';
		cmd[len+1] = '\n';
		cmd[len+2] = '\0';

		ftp.cmdExecuter(cmd);

		if( strncmp(cmd, "QUIT", 4) == 0 )
			break;

		gets(cmd);
	}
}