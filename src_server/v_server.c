/*
* 		v_sever.c - TCP Server developed for Virscient
*		Author: Pieter Britz
*		Date: 13 September 2022
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

#include "menu.h"
#include "../include/logger.h"

#define SIZE 1024
#define MAX_CONNECTIONS 80

typedef struct serverData_t
{
    int master_socket;
    int addrlen;
    int new_socket;
    int client_socket[30];
    int activity;
    int sd;                         // socket descriptor
    int max_sd;
    int max_clients;
    int valread;
    struct sockaddr_in address;
    fd_set readfds;                 // set of socket descriptors
    char *serverIp;                 // User supplied server IP.
    int port;
    char connIp[MAX_CONNECTIONS][INET6_ADDRSTRLEN];
    int connCount;
}serverData_t;
serverData_t serverData;
menu_t menuData;


int DoSendBuffer(int sessionFd, char *buffer, int length)
{
    size_t index = 0;
    char message[60];

    while (index < length) {
        ssize_t count = send(sessionFd, buffer+index, length - index, 0);
        if (count < 0)
        {
            snprintf(message, sizeof(message), "[-]failed to write to socket (errno=%d)", errno);
            logger(__FUNCTION__, message);
            if (errno == EINTR) break;
        } 
        else
        {
            index += count;
        }
    }
    return index;
}

void SendFile(FILE *fp, int sockfd)
{
    int n;
    char data[SIZE] = {0};

    while(fgets(data, SIZE, fp) != NULL) 
    {
        if (send(sockfd, data, sizeof(data), 0) == -1) 
        {
            logger(__FUNCTION__, "[-]Error in sending file.");
            break;
        }
        memset(data, 0, SIZE);
    }
}

void RequestFileName(int sockfd)
{
    char message[SIZE];
    
    memset(message, 0, SIZE);
    snprintf(message, sizeof(message), "Please send file name: ");
    if (send(sockfd, message, sizeof(message), 0) == -1) 
    {
        logger(__FUNCTION__, "[-]Error in sending message.");
    }
 
}


/* Read the file list from the docs directory */
int GetFileList(int sockfd)
{
	DIR *d;
	struct dirent *dir;
    char buffer[SIZE];

	d = opendir("../docs");
	if (d)
    {
	    while ((dir = readdir(d)) != NULL)
        {
            memset(buffer, 0, SIZE);
            snprintf(buffer, sizeof(dir->d_name) + 3, "%s\r\n", dir->d_name);
            DoSendBuffer(sockfd, buffer, sizeof(dir->d_name) + 3);
            logger(__FUNCTION__, buffer);
	    }
	    closedir(d);
	}
    return 0;
}

void handle_session(int socket_fd, char *cmd) {
    time_t now = time(0);
    char buffer[SIZE];

    size_t length = strftime(buffer, sizeof(buffer), "%a %b %d %T %Y\r\n", localtime(&now));
    logger(__FUNCTION__, buffer);
    if (length == 0)
    {
        snprintf(buffer, sizeof(buffer), "[-]Error: buffer overflow\r\n");
        logger(__FUNCTION__, buffer);
    }
    DoSendBuffer(socket_fd, buffer, length);
    
    switch(cmd[0])
    {
        case 't':
            GetFileList(socket_fd);
            break;
        case 'd':
            RequestFileName(socket_fd);
        default:
            break;
    }
    
}

/* This function allows for multiple client connections. */
int AcceptConnections(void)
{
    //a message
    char *message = "Virscient Server v1.0 \r\n";
    char msg[1050];
    struct sockaddr sa;
    socklen_t sa_len = sizeof(sa);
    char buffer[SIZE];
    int newclient_sock;
    struct sockaddr_in client_addr;
    int i;

    //accept the incoming connection
    serverData.addrlen = sizeof(serverData.address);
    puts("Waiting for connections ...");
    logger(__FUNCTION__, "Waiting for connections ...");

    while(TRUE)
    {
        //clear the socket set
        FD_ZERO(&serverData.readfds);

        //add master socket to set
        FD_SET(serverData.master_socket, &serverData.readfds);
        serverData.max_sd = serverData.master_socket;

        //add child sockets to set
        for ( i = 0 ; i < serverData.max_clients ; i++)
        {
            //socket descriptor
            serverData.sd = serverData.client_socket[i];

            //if valid socket descriptor then add to read list
            if(serverData.sd > 0)
                FD_SET( serverData.sd , &serverData.readfds);

            //highest file descriptor number, need it for the select function
            if(serverData.sd > serverData.max_sd)
                serverData.max_sd = serverData.sd;
        }

        //wait for an activity on one of the sockets , timeout is NULL ,
        //so wait indefinitely
        serverData.activity = select( serverData.max_sd + 1 , &serverData.readfds , NULL , NULL , NULL);

        if ((serverData.activity < 0) && (errno!=EINTR))
        {
            logger(__FUNCTION__, "select error");
        }

        //If something happened on the master socket ,
        //then its an incoming connection
        if (FD_ISSET(serverData.master_socket, &serverData.readfds))
        {
            if ((serverData.new_socket = accept(serverData.master_socket,
                (struct sockaddr *)&serverData.address, (socklen_t*)&serverData.addrlen))<0)
            {
            logger(__FUNCTION__, "accept");
            exit(EXIT_FAILURE);
            }
            menuData.connections = serverData.connCount++;
            DrawMenu(&menuData);

            //inform user of socket number - used in send and receive commands
            snprintf(msg, sizeof(msg), "New connection , socket fd is %d, ip is : %s, port : %d\n",
                    serverData.new_socket , inet_ntoa(serverData.address.sin_addr) , ntohs(serverData.address.sin_port));
            logger(__FUNCTION__, msg);

            //send new connection greeting message
            if( send(serverData.new_socket, message, strlen(message), 0) != strlen(message) )
            {
                logger(__FUNCTION__, "send");
            }

            puts("Welcome message sent successfully");
            logger(__FUNCTION__, "Welcome message sent successfully");

            //add new socket to array of sockets
            for (i = 0; i < serverData.max_clients; i++)
            {
                //if position is empty
                if( serverData.client_socket[i] == 0 )
                {
                    serverData.client_socket[i] = serverData.new_socket;
                    snprintf(msg, sizeof(msg), "Adding to list of sockets as %d\n" , i);
                    logger(__FUNCTION__, msg); 
                    
                    break;
                }
            }
        }

        //else its some IO operation on some other socket
        for (i = 0; i < serverData.max_clients; i++)
        {
            serverData.sd = serverData.client_socket[i];

            if (FD_ISSET( serverData.sd , &serverData.readfds))
            {
                //Check if it was for closing , and also read the
                //incoming message
                if ((serverData.valread = read( serverData.sd , buffer, SIZE)) == 0)
                {
                    //Somebody disconnected , get his details and print
                    getpeername(serverData.sd , (struct sockaddr*)&serverData.address , \
                    (socklen_t*)&serverData.addrlen);
                    snprintf(msg, sizeof(msg), "Host disconnected , ip %s , port %d \n" ,
                    inet_ntoa(serverData.address.sin_addr) , ntohs(serverData.address.sin_port));
                    logger(__FUNCTION__, msg);

                    //Close the socket and mark as 0 in list for reuse
                    close( serverData.sd );
                    serverData.client_socket[i] = 0;
                }
                else
                {
                    //set the string terminating NULL byte on the end
                    //of the data read
                    buffer[serverData.valread] = '\0';
                    //send(serverData.sd , buffer , strlen(buffer) , 0 );   // This will echo back what was received
                    snprintf(msg, sizeof(msg), "[+]Received from client: %s", buffer);
                    logger(__FUNCTION__, msg);
                    if (strcmp(buffer, "t") == 0)
                        handle_session(serverData.sd, "t");
                    else if (strcmp(buffer, "d") == 0)
                        handle_session(serverData.sd, "d");
                    else

                }
            }
        }
    }
}

int StartServer(void)
{
    int opt = TRUE;
    char buffer[SIZE];

    //create a master socket
    if( (serverData.master_socket = socket(AF_INET , SOCK_STREAM , 0)) < 0)
    {
        logger(__FUNCTION__, "[-]Error in creating socket");
        return -1;
    }
    logger(__FUNCTION__, "[+]Server socket created successfully.\n");

    //set master socket to allow multiple connections ,
    //this is just a good habit, it will work without this
    if( setsockopt(serverData.master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        logger(__FUNCTION__, "[-]Error in setsockopt");
        return -1;
    }

    //type of socket created
    serverData.address.sin_family = AF_INET;
    serverData.address.sin_addr.s_addr = INADDR_ANY;
    serverData.address.sin_port = htons( serverData.port );

    // Bind the socket to the given port.
    if (bind(serverData.master_socket, (struct sockaddr *)&serverData.address, sizeof(serverData.address)) < 0)
    {
        logger(__FUNCTION__, "[-]Error in bind");
        return -2;
    }
    logger(__FUNCTION__, "[+]Binding successfull.\n");

    // Start listening for the connections
    //try to specify maximum of 3 pending connections for the master socket
    if(listen(serverData.master_socket, 3) == 0)
    {
        logger(__FUNCTION__, "[+]Listening....\n");
    }
    else
    {
        logger(__FUNCTION__, "[-]Error in listening");
        return -3;
    }

    return 0;
}

int StopServer(void)
{
    logger(__FUNCTION__, "[+]Server Stoped.");
    return close(serverData.master_socket);
}

 
void write_file(int sockfd)
{
    int n;
    FILE *fp;
    char *filename = "recv.txt";
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

 
#define TRUE 1
#define FALSE 0
#define PORT 8888
 
int main(int argc, char *argv[])
{
    int i;

	if (argc < 3)
	{
		printf("No server IP was supplied. Please check usage.\n");
		printf("Usage: server <server_ip> <port>\n");
		printf("server_ip  : IP4 format, eg. 127.0.0.1\n");
		printf("server port: TCP port number eg. 8080\n");
		return -1;
	}
	serverData.serverIp = (char *)malloc(sizeof(argv[1]));
	(void)sprintf(serverData.serverIp, argv[1], "%s");
    serverData.port = atoi(argv[2]);

    serverData.max_clients = 30;

    char buffer[1025]; //data buffer of 1K

    //initialise all client_socket[] to 0 so not checked
    for (i = 0; i < serverData.max_clients; i++)
    {
        serverData.client_socket[i] = 0;
    }

   	menuData.ip = serverData.serverIp;
	menuData.connected = false;
    menuData.connections = 0;
	menuData.dataRxCount = 0;
	menuData.dataTxCount = 0;
	// Draw the menu
    menu(&menuData);

    return 0;
}
