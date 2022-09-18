/*
* 		v_client.c - TCP Client developed for Virscient
*		Author: Pieter Britz
*		Date: 13 September 2022
*
*		This program is a TCP client with a menu in which you can connect
*		and disconnect from your indicated server. You can list the 
*		files from it's docs directory, and download them one by one.
*/

#include <stdio.h>
#include<string.h>	//strlen
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>	//inet_addr
#include <dirent.h> 

#include "../include/logger.h"
#include "menu.h"

#define SIZE 1024
typedef struct clientData_t
{
    int socketfd;
    struct sockaddr_in serverAddr;
    int serverSize;
	char *server_ip;
	int port;

}clientData_t;
clientData_t clientData;
menu_t menuData;
char rxBuff[SIZE];

int CreateLocalSocket(int *desriptor)
{
	//Create socket
	*desriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (*desriptor == -1)
	{
		logger(__FUNCTION__, "Error: Could not create socket.\r\n");
        return -2;
	}
    return 0;
}

int ConfigureLocalSocket(struct sockaddr_in *server, char *server_ip, int port)
{
	if ((server == NULL) || (server_ip == NULL))
		return -3;
	server->sin_addr.s_addr = inet_addr("127.0.0.1"); //INADDR_ANY; //inet_addr(server_ip);
	server->sin_family = AF_INET;
	server->sin_port = port;
	return 0;
}

int ConnectToServer(void)
{
	char buff[600];
	int result = 0;
	if((result = CreateLocalSocket(&clientData.socketfd)) < 0)
		return result;
	memset(&clientData.serverAddr, 0, sizeof(clientData.serverAddr));
	if((result = ConfigureLocalSocket(&clientData.serverAddr, &clientData.server_ip[0], clientData.port)) < 0)
		return result;
	clientData.serverSize = sizeof(clientData.serverAddr);
	result = connect(clientData.socketfd, (struct sockaddr *)&clientData.serverAddr, clientData.serverSize);
	snprintf(buff, sizeof(buff), "socket:%d, serverAddr:%d, size:%d, port:%d, result:%d", clientData.socketfd, clientData.serverAddr.sin_addr.s_addr,
			 clientData.serverSize, clientData.port, result);
	logger(__FUNCTION__, buff);
	return result;
}

int CloseConnection(void)
{
	menuData.dataTxCount = 0;
	menuData.dataRxCount = 0;
	return close(clientData.socketfd);
}

void send_file(FILE *fp, int sockfd)
{
	int n;
	char data[SIZE] = {0};

	while(fgets(data, SIZE, fp) != NULL)
	{
		if (send(sockfd, data, sizeof(data), 0) == -1) 
		{
			logger(__FUNCTION__, "[-]Error in sending file.");
			exit(1);
		}
		memset(data, 0, SIZE);
	}
}

void write_file(int sockfd, char *filename)
{
    int n;
    FILE *fp;
    char buffer[SIZE];

    fp = fopen(filename, "w");
    while (1) 
    {
        n = recv(sockfd, buffer, SIZE, 0);
        if (n <= 0)
        {
            break;
            return;
        }
        fprintf(fp, "%s", buffer);
        memset(buffer, 0, SIZE);
    }
    return;
}

void requestFileList(void)
{
	char txBuff[32] = "t";
	int bytesRead = 0;
	int bytesWrite = 0;
	char tmpBuff[500];

	memset(rxBuff, 0, sizeof(rxBuff));
	bytesWrite = write(clientData.socketfd, txBuff, sizeof(txBuff));
		sprintf(tmpBuff, "bytesWrite:%d", bytesWrite);
		logger(__FUNCTION__, tmpBuff);
	menuData.dataTxCount += bytesWrite;
	menuData.sizeOfMessage = 0;
	do
	{
		bytesRead = read(clientData.socketfd, rxBuff, sizeof(rxBuff));
		sprintf(tmpBuff, "bytesRead:%d", bytesRead);
		logger(__FUNCTION__, tmpBuff);
		menuData.dataRxCount += bytesRead;
		drawMore(rxBuff);
		memset(rxBuff, 0, SIZE);
	}
	while(bytesRead > 0);
}

/*

*/
int main (int argc, char *argv[])
{
	char *ip = "127.0.0.1";
	int port = 8080;
	int e;

	int sockfd;
	struct sockaddr_in server_addr;
	FILE *fp;
	char *filename = "send.txt";

	int socket_desc;
	
	if (argc == 1)
	{
		printf("No server IP was supplied. Please check usage.\n");
		printf("Usage: client <server_ip> <port>\n");
		printf("server_ip: IP4 format, eg. 192.168.10.10\n");
		printf("server port: TCP port number eg. 8080\n");
		return 1;
	}
	clientData.server_ip = (char *)malloc(sizeof(argv[1]));
	(void)sprintf(clientData.server_ip, argv[1], "%s");
	menuData.port = clientData.port = port = atoi(argv[2]);

	menuData.ip = clientData.server_ip;
	menuData.connected = false;
	menuData.dataRxCount = 0;
	menuData.dataTxCount = 0;
	// Draw the menu
    menu(&menuData);


    return 0;
}
