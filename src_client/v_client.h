/*
 * Header: v_client.h
 */

#ifndef __V_CLIENT_H__
#define __V_CLIENT_H__


int ConnectToServer(void);
int AcceptConnections(void);
int CloseConnection(void);
void requestFileList(void);
void requestDownload(void);



#endif // __V_CLIENT_H__