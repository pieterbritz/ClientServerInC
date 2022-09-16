#include <stdio.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr

#include "v_client.h"
#include "menu.h"


int CreateLocalSocket(int *desriptor)
{
	//Create socket
	*desriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (*desriptor == -1)
	{
		printf("Error: Could not create socket.\r\n");
        return -1;
	}
    return 0;
}

int ConfigureLocalSocket(struct sockaddr_in *server, char *server_ip)
{
	server->sin_addr.s_addr = inet_addr(server_ip);
	server->sin_family = AF_INET;
	server->sin_port = htons( 80 );
}

int ConnectToServer(int socket, struct sockaddr_in *server, int serverSize)
{
	return connect(socket, (struct sockaddr *)&server, serverSize);
}

int main (int argc, char *argv[])
{
	int socket_desc;
	struct sockaddr_in server;
	char *serverIp;
	char *message;
	
	if (argc == 1)
	{
		printf("No server IP was supplied. Please check usage.\n");
		printf("Usage: client <server_ip>\n");
		printf("server_ip: IP4 format, eg. 192.168.10.10\n");
		return 1;
	}
	(void)sprintf(serverIp, argv[1], "%s");

	// Draw the menu
    menu();

	// Create a local socket to communicate via
	if(CreateLocalSocket(&socket_desc) < 0)
    {
        printf("Error - Fail to connect.\r\n");
    }	

	// Configure the socket with the server IP to connect to.
	ConfigureLocalSocket((struct sockaddr_in *)&server, serverIp);

	//Connect to remote server
	if (ConnectToServer(socket_desc, (struct sockaddr_in *)&server, sizeof(server)) < 0)
	{
		puts("connect error");
		return 1;
	}
	
	puts("Connected\n");
	
	//Send some data
	message = "GET / HTTP/1.1\r\n\r\n";
	if( send(socket_desc, message, strlen(message), 0) < 0)
	{
		puts("Send failed");
		return 1;
	}
	puts("Data Send\n");

    return 0;
}