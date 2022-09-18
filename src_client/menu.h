
#ifndef __MENU_H__
#define __MENU_H__

#include <stdio.h>
#include <stdint.h>
#include <curses.h>

typedef struct menu_t
{
    char *ip;
    int port;
    bool connected;
    uint32_t dataRxCount;
    uint32_t dataTxCount;
	char *message;
    int sizeOfMessage;
} menu_t;


void menu(menu_t *menuData);
void DrawMenu(menu_t *menuData);
void drawMore(char *message);



#endif // __MENU_H__