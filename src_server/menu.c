#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <curses.h>

#include "v_server.h"
#include "menu.h"


void DrawMenu(menu_t *menu)
{
    system("clear");
    printf("============================================================\n");
    printf("=  Versient TCP Server v0.1                                =\n");
    printf("============================================================\n");
    printf("=     Options:                                             =\n");
    printf("=     ========                                             =\n");
    printf("=     (S)tart              Server IP: %s                   =\n", menu->ip);
    printf("=     sto(p)                                               =\n");
    printf("=     (L)ogging                                            =\n");
    printf("=     E(x)it                                               =\n");
    printf("=                                                          =\n");
    printf("=                                                          =\n");
    printf("=    Server status:         %s                           =\n", (menu->connected)? "Started": "Stoped ");
    printf("=    Connections:           %.02d                            =\n", menu->connections);
    printf("=    Bytes Send: %.06d      Bytes Received: %.06d  =\n", menu->dataTxCount, menu->dataRxCount);
    printf("=                                                          =\n");
    printf("============================================================\n");
}

// Menu implementation
void menu(menu_t *menuData)
{
    int command;

    DrawMenu(menuData);
    while ((command = getchar()) != EOF)
    {
        switch(command)
        {
        case 's':
        case 'S':
            if(StartServer() < 0)
            {
                menuData->connected = false;
            }
            else
            {
                menuData->connected = true;
            }
            DrawMenu(menuData);
            printf("Server started.\n");
            AcceptConnections();
            break;

        case 'p':
        case 'P':
            if(StopServer() == 0)
            {
                menuData->connected = false;
            }
            DrawMenu(menuData);
            printf("Closed connection.\n");
            break;

        case 'l':
        case 'L':
            printf("Logging to a file.\n");
            break;

        case 'd':
        case 'D':
            printf("Download file\n");
            break;

        case 'x':
        case 'X':
            printf("Quit\n");
            exit(0);
            break;

        default:
            // DrawMenu(menuData);
            // printf("Unexpected input %d\n", command);
            break;
        }
    }
}