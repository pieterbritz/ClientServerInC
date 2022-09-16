#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <curses.h>
#include <termios.h>


void DrawMenu(bool connected, uint32_t dataRxCount, uint32_t dataTxCount)
{
    system("clear");
    printf("============================================================\n");
    printf("=  Versient TCP Client v0.1                                =\n");
    printf("============================================================\n");
    printf("=     Options:                                             =\n");
    printf("=     ========                                             =\n");
    printf("=     (C)onnect to                                         =\n");
    printf("=     E(x)it                                               =\n");
    printf("=     (L)ogging                                            =\n");
    printf("=     (D)ownload file                                      =\n");
    printf("=                                                          =\n");
    printf("=                                                          =\n");
    printf("=    Connection status:           %s            =\n", (connected)? "Connected    ": "Not Connected");
    printf("=    Bytes Send: %.06d      Bytes Received: %.06d  =\n", dataTxCount, dataRxCount);
    printf("=                                                          =\n");
    printf("============================================================\n");
}

// Menu implementation
int menu(void)
{
    int command;

    DrawMenu(false, 0, 0);
    while ((command = getchar()) != EOF)
    {
        switch(command)
        {
        case 'c':
        case 'C':
            DrawMenu(true, 0, 0);
            printf("You are now connected.\n");
            break;

        case 'x':
        case 'X':
            DrawMenu(false, 0, 0);
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

        case 'q':
        case 'Q':
            printf("Quit\n");
            exit(0);
            break;

        default:
            printf("Unexpected input %d\n", command);
            break;
        }
    }
    return 0;
}