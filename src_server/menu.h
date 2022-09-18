
#ifndef __MENU_H__
#define __MENU_H__

#include <stdio.h>
#include <stdint.h>
#include <curses.h>

#define MAX_CONNECTIONS 80

typedef struct menu_t
{
    char *ip;               // Server IP
    bool connected;         // Server Started
    int connections;        // Nunber of connections
    char connIp[MAX_CONNECTIONS][INET6_ADDRSTRLEN];
    uint32_t dataRxCount;
    uint32_t dataTxCount;
} menu_t;


void menu(menu_t *menu);
void DrawMenu(menu_t *menu);



#endif // __MENU_H__