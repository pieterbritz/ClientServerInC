#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <curses.h>
#include <termios.h>
#include <dirent.h> 

#include "menu.h"
#include "v_client.h"

bool keepRunning = true;

void DrawMenu(menu_t *menuData)
{
    system("clear");
    printf("============================================================\n");
    printf("=  Versient TCP Client v0.1                                =\n");
    printf("============================================================\n");
    printf("=     Options:                                             =\n");
    printf("=     ========                                             =\n");
    printf("=     (C)onnect to                %s : %d             =\n", menuData->ip, menuData->port);
    printf("=     Cl(o)se connection                                   =\n");
    printf("=     (L)ogging                                            =\n");
    printf("=     Lis(t) files                                         =\n");
    printf("=     (D)ownload file                                      =\n");
    printf("=     E(x)it                                               =\n");
    printf("=                                                          =\n");
    printf("=                                                          =\n");
    printf("=    Connection status:           %s            =\n", (menuData->connected)? "Connected    ": "Not Connected");
    printf("=    Bytes Send: %.06d      Bytes Received: %.06d        =\n", menuData->dataTxCount, menuData->dataRxCount);
    printf("=                                                          =\n");
    printf("============================================================\n");
    if (menuData->message != NULL) printf("%s", menuData->message);
}

void drawMore(char *message)
{
    printf("%s", message);
}

int ListFiles(void)
{
  DIR *d;
  struct dirent *dir;
  d = opendir(".");
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      printf("%s\n", dir->d_name);
    }
    closedir(d);
  }
  return(0);
}

// Menu implementation
void menu(menu_t *menuData)
{
    int command, res;

    DrawMenu(menuData);
    while (keepRunning)
    {
        command = getchar();
        switch(command)
        {
        case 'c':
        case 'C':
            if((res = ConnectToServer()) < 0)
            {
                menuData->connected = false;
            }
            else
            {
                menuData->connected = true;
            }
            DrawMenu(menuData);
            if (menuData->connected) { printf("menu: You are now connected.\n"); } else {printf("Menu: Connection failed.(err%d)\n", res);}
            break;

        case 'o':
        case 'O':
            if (CloseConnection() == 0)
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

        case 't':
        case 'T':
            requestFileList();
            DrawMenu(menuData);
           break;

        case 'd':
        case 'D':
            requestDownload();
            break;

        case 'x':
        case 'X':
            printf("Quit\n");
            keepRunning = false;
            break;

        default:
//            DrawMenu(menu->ip, menu->connected, menu->dataRxCount, menu->dataTxCount);
//            printf("Unexpected input %d\n", command);
            break;
        } // switch(command)
    } // while (keepRunning)
    
} // void menu(menu_t *menuData)